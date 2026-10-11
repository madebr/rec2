#!/usr/bin/env python3
"""Compare texture search with original x86 and three native compilers.

Directory enumeration and image loading are controlled boundaries. Actual
GetFileName/GetAdditionalFileName, uppercase, split and path bodies execute.
Fixtures use ASCII paths within the original fixed buffers. Snapshots compare
all external arena/global bytes and every initialized byte of the private list;
unwritten private stack bytes are excluded, never initialized in production.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import random
import struct
import subprocess
import sys

from verify_tinted_native_reconstruction import extract

ROOT = Path(__file__).resolve().parents[1]
HASH = '9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
SOURCE = ROOT / 'src/carma2/newcommon/41-utility.c'
WORLD = ROOT / 'src/carma2/newcommon/28-world3.c'
ADDRESSES = dict(LoadTextureTryAllLocations=0x513a30, GetFileName=0x5026b0,
    GetAdditionalFileName=0x502780, PFForEveryFile2=0x4b4d30, DRLdImg=0x48eb80,
    gApplication_path=0x75ba60, gCurrent_load_directory=0x762180,
    gCurrent_load_name=0x761a80, gDir_separator=0x761a5c,
    gGraf_specs=0x662208, gGraf_spec_index=0x762324,
    gPixelFlags=0x74ca18, gRender_palette=0x74a674, toupper=0x5771a0)
HEAP, STACK, STOP, RETURN, SCRATCH = 0x1000000, 0x1080000, 0x1090000, 0x1091000, 0x10a0000
SIZE = 4096


def words(values):
    return struct.pack('<' + 'I' * len(values), *[v & 0xffffffff for v in values])


def fixed(value, size):
    value = value.encode('ascii')
    assert len(value) < size
    return value + bytes(size - len(value))


def main():
    p = argparse.ArgumentParser(description=__doc__)
    for n, d in [('original', ROOT/'build-reference/originals/CARMA2_HW.EXE'),
                 ('recompiled', ROOT/'build-matching/rec2.exe'),
                 ('dependency-dir', ROOT/'build-audit/joystick/python-deps'),
                 ('output', ROOT/'build-audit/texture-search-tests'),
                 ('gcc', ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'),
                 ('msvc-root', ROOT/'build-tools/msvc-modern'),
                 ('msvc5-root', ROOT/'build-tools/msvc500sp3')]:
        p.add_argument('--'+n, type=Path, default=d)
    p.add_argument('--symbols', type=Path)
    p.add_argument('--case-index', type=int, action='append')
    args = p.parse_args()
    sys.path.insert(0, str(args.dependency_dir.resolve()))
    from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
    from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_EBX, UC_X86_REG_ECX, UC_X86_REG_EDX, UC_X86_REG_ESP, UC_X86_REG_EBP, UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EIP
    original = args.original.read_bytes()
    assert hashlib.sha256(original).hexdigest() == HASH
    images = [(original, ADDRESSES)]
    if args.symbols:
        m = json.loads(args.symbols.read_text()); rebuilt = args.recompiled.read_bytes()
        assert m['format'] == 1 and m['original_sha256'] == HASH
        assert m['recompiled_sha256'] == hashlib.sha256(rebuilt).hexdigest()
        mapped = {s['orig']: s['recomp'] for s in m['symbols']}
        images.append((rebuilt, {k: mapped[v] for k, v in ADDRESSES.items()}))
    out = args.output.resolve(); out.mkdir(parents=True, exist_ok=True)
    driver = r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <io.h>
#include "41-utility.h"
#include "28-world3.h"
#include "63-loading3.h"
#include "70-packfile.h"
#include "globvars.h"
#include "platform.h"
#include "rec2_macros.h"
typedef char layouts[sizeof(void*)==4&&sizeof(tName_list)==16004&&sizeof(tGraf_spec)==52?1:-1];
static unsigned h[16],counts[8],enums,loads;
static unsigned char arena[4096],known[512];
static char names[8][4][64];
static tName_list* current_list;
tPath_name gApplication_path;
char gCurrent_load_directory[256],gCurrent_load_name[256];
const char* gDir_separator;
tGraf_spec gGraf_specs[2];
int gGraf_spec_index;
br_pixelmap* gRender_palette;
tPixelFlags gPixelFlags;
static void emit(unsigned v){fwrite(&v,4,1,stdout);}
static unsigned norm(const void*p){unsigned v=(unsigned)(uintptr_t)p,b=(unsigned)(uintptr_t)arena;return v>=b&&v<b+4096?v-b+0x1000000:v;}
static void string(const char*s){unsigned n=(unsigned)strlen(s);emit(n);fwrite(s,n,1,stdout);}
static void snapshot(void){
 unsigned char copy[4096],specs[104];unsigned i,v;
 emit(gPixelFlags);emit(norm(gRender_palette));emit(gGraf_spec_index);emit(norm(gDir_separator));
 fwrite(gApplication_path,256,1,stdout);fwrite(gCurrent_load_directory,256,1,stdout);fwrite(gCurrent_load_name,256,1,stdout);
 memcpy(specs,gGraf_specs,104);for(i=0;i<2;i++){v=norm(gGraf_specs[i].data_dir_name);memcpy(specs+52*i+28,&v,4);}fwrite(specs,104,1,stdout);
 memcpy(copy,arena,4096);for(i=0;i<32;i++){memcpy(&v,copy+0x500+4*i,4);v=norm((void*)(uintptr_t)v);memcpy(copy+0x500+4*i,&v,4);}fwrite(copy,4096,1,stdout);
}
static void mark(int index){unsigned n,j;if(index<0||index>=32)exit(91);n=(unsigned)strlen(current_list->items[index]);if(n>=16)exit(92);for(j=0;j<=n;j++)known[16*index+j]=1;}
static void list_snapshot(void){unsigned i;unsigned char copy[512];emit(current_list->size);for(i=0;i<512;i++)copy[i]=known[i]?((unsigned char*)current_list->items)[i]:0;fwrite(copy,512,1,stdout);}
void C2_HOOK_FASTCALL PFForEveryFile2(const char*path,tEnumPathCallback callback,void*data){
 unsigned index=enums++,j;int before,result;char full[256];current_list=(tName_list*)data;
 if(index>=8){exit(93);}emit(1);emit(index);string(path);emit(callback==(tEnumPathCallback)GetFileName?0:1);snapshot();list_snapshot();
 for(j=0;j<counts[index];j++){
  strcpy(full,path);strcat(full,gDir_separator);strcat(full,names[index][j]);before=current_list->size;
  emit(2);string(full);result=callback(full,data);if(current_list->size>before)mark(before);
  emit(result);snapshot();list_snapshot();
 }
 if(index==h[9]){
  if(h[4]&1)gDir_separator=(char*)arena+0x20;
  if(h[4]&2){strcpy(gApplication_path,"MUTATED");strcpy(gCurrent_load_directory,"OTHER");strcpy(gCurrent_load_name,"CHANGED");}
  if(h[4]&4){gGraf_spec_index=1;gGraf_specs[1].data_dir_name=(char*)arena+0x380;}
  if(h[4]&8){gRender_palette=(br_pixelmap*)(arena+0xa00);gPixelFlags=(tPixelFlags)0xaabbccdd;}
  if(h[6]!=0x7fffffff)current_list->size=(int)h[6];
 }
 emit(3);snapshot();list_snapshot();
}
br_pixelmap* C2_HOOK_FASTCALL DRLdImg(const char*path,const char*name,br_pixelmap*palette,int flags,int*error){
 unsigned index=loads++,ret;int slot;
 if(index>=32){exit(94);}emit(4);emit(index);string(path);string(name);emit(norm(palette));emit(flags);snapshot();list_snapshot();
 *error=(int)(h[8]+index);
 if(index==h[10]){
  if(h[5]&1)current_list->size=0;
  if(h[5]&2){slot=current_list->size;strcpy(current_list->items[slot],"EXTRA");mark(slot);current_list->size++;}
  if(h[5]&4)current_list->size=-1;
  if(h[5]&8){strcpy(current_list->items[index+1],"CHANGED");mark((int)index+1);}
  if(h[5]&16){gPixelFlags=(tPixelFlags)0x87654321;gRender_palette=NULL;}
  if(h[5]&32)((char*)path)[0]='?';
 }
 ret=h[7]==1||(h[7]==2&&(index&1))?0:0x800+64*index;
 emit(5);emit(ret?0x1000000+ret:0);snapshot();list_snapshot();return ret?(br_pixelmap*)(arena+ret):NULL;
}
'''
    source = SOURCE.read_text(); world = WORLD.read_text()
    bodies = [extract(source, n) for n in ('Uppercaseificate','PathCat','SepDirAndFilename','LoadTextureTryAllLocations')]
    bodies += [extract(world, n) for n in ('GetFileName','GetAdditionalFileName')]
    driver += '\n'.join(bodies)
    driver += r'''
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned result;char*name;
 if(argc!=2){return 2;}f=fopen(argv[1],"rb");if(!f){return 3;}
 if(fread(h,64,1,f)!=1||fread(arena,4096,1,f)!=1||fread(gApplication_path,256,1,f)!=1||fread(gCurrent_load_directory,256,1,f)!=1||fread(gCurrent_load_name,256,1,f)!=1||fread(counts,32,1,f)!=1||fread(names,sizeof(names),1,f)!=1){return 4;}fclose(f);
 gDir_separator=(char*)arena;gGraf_spec_index=(int)h[3];gGraf_specs[0].data_dir_name=(char*)arena+0x300;gGraf_specs[1].data_dir_name=(char*)arena+0x380;gRender_palette=h[2]?(br_pixelmap*)(arena+h[2]):NULL;gPixelFlags=(tPixelFlags)h[1];
 name=h[11]==1?gCurrent_load_name:h[11]==2?gCurrent_load_directory:h[11]==3?gApplication_path:(char*)arena+0x100;
 _setmode(_fileno(stdout),_O_BINARY);emit(0);snapshot();result=LoadTextureTryAllLocations(name,(br_pixelmap**)(arena+0x500),(int)h[0]);emit(9);emit(result);snapshot();return 0;}
'''
    harness = out/'harness.c'; harness.write_text(driver)
    includes = [ROOT / n for n in ('src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/carma2/platform','src/carma2/s3/include','src/brender/include')]
    modern = args.msvc_root.resolve()/'VC/Tools/MSVC/14.44.35207'; sdk = args.msvc_root.resolve()/'Windows Kits/10'; version = '10.0.26100.0'
    env = dict(os.environ); env['PATH'] = str(modern/'bin/Hostx64/x86')+os.pathsep+env['PATH']; env['INCLUDE'] = os.pathsep.join(map(str,[modern/'include',*[sdk/f'Include/{version}/{k}' for k in ('ucrt','shared','um')]])); env['LIB'] = os.pathsep.join(map(str,[modern/'lib/x86',*[sdk/f'Lib/{version}/{k}/x86' for k in ('ucrt','um')]]))
    old = args.msvc5_root.resolve(); old_env = dict(os.environ); old_env['PATH'] = str(old/'bin')+os.pathsep+str(old/'redist')+os.pathsep+old_env['PATH']; old_env['INCLUDE'],old_env['LIB'] = str(old/'include'),str(old/'lib')
    commands = {
        'msvc':[str(modern/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(x) for x in includes],str(harness),'/Fo'+str(out/'msvc.obj'),'/Fe'+str(out/'msvc.exe')],
        'msvc5':[str(old/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(x) for x in includes],str(harness),'/Fo'+str(out/'msvc5.obj'),'/Fe'+str(out/'msvc5.exe')],
        'mingw':[str(args.gcc.resolve()),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-fno-strict-aliasing',*['-I'+str(x) for x in includes],str(harness),'-o',str(out/'mingw.exe')]}
    for compiler, command in commands.items():
        result = subprocess.run(command,cwd=out,env=env if compiler=='msvc' else old_env if compiler=='msvc5' else None,capture_output=True,text=True)
        (out/f'{compiler}-build.log').write_text(result.stdout+result.stderr)
        assert result.returncode==0,result.stdout+result.stderr

    def execute(data,a,h,arena,paths,files):
        u=Uc(UC_ARCH_X86,UC_MODE_32);u.mem_map(0x400000,0x500000);u.mem_map(HEAP,0x100000)
        pe=struct.unpack_from('<I',data,0x3c)[0];table=pe+24+struct.unpack_from('<H',data,pe+20)[0]
        for i in range(struct.unpack_from('<H',data,pe+6)[0]):
            _,rva,size,offset=struct.unpack_from('<4I',data,table+40*i+8)
            if size:u.mem_write(0x400000+rva,data[offset:offset+size])
        def rd(p):return struct.unpack('<I',u.mem_read(p,4))[0]
        def signed(v):return v if v<0x80000000 else v-0x100000000
        def wr(p,v):u.mem_write(p,words([v]))
        def string(p):
            b=bytearray()
            while u.mem_read(p,1)!=b'\0':b.extend(u.mem_read(p,1));p+=1;assert len(b)<256
            return bytes(b)
        events=bytearray();known=bytearray(512);state=dict(enums=0,loads=0,list=0,frame=None)
        def emit(*vs):events.extend(words(vs))
        def text(b):emit(len(b));events.extend(b)
        def snapshot():
            emit(rd(a['gPixelFlags']),rd(a['gRender_palette']),rd(a['gGraf_spec_index']),rd(a['gDir_separator']))
            for key in ('gApplication_path','gCurrent_load_directory','gCurrent_load_name'):events.extend(u.mem_read(a[key],256))
            events.extend(u.mem_read(a['gGraf_specs'],104));events.extend(u.mem_read(HEAP,SIZE))
        def mark(i):
            assert 0<=i<32
            n=len(string(state['list']+4+16*i));assert n<16
            known[16*i:16*i+n+1]=b'\1'*(n+1)
        def list_snapshot():
            emit(rd(state['list']));b=u.mem_read(state['list']+4,512);events.extend(v if k else 0 for v,k in zip(b,known))
        def ret(value=0,pop=0):
            sp=u.reg_read(UC_X86_REG_ESP);u.reg_write(UC_X86_REG_EAX,value&0xffffffff);u.reg_write(UC_X86_REG_EIP,rd(sp));u.reg_write(UC_X86_REG_ESP,sp+4+pop)
        def advance():
            f=state['frame'];index=f['index'];j=f['j']
            if j<len(files[index]):
                full=f['path']+string(rd(a['gDir_separator']))+files[index][j].encode('ascii');assert len(full)<256
                u.mem_write(SCRATCH,full+b'\0');emit(2);text(full);f['before']=signed(rd(state['list']))
                wr(f['sp']-4,RETURN);u.reg_write(UC_X86_REG_ESP,f['sp']-4);u.reg_write(UC_X86_REG_ECX,SCRATCH);u.reg_write(UC_X86_REG_EDX,state['list']);u.reg_write(UC_X86_REG_EIP,f['callback'])
            else:
                if index==h[9]:
                    if h[4]&1:wr(a['gDir_separator'],HEAP+0x20)
                    if h[4]&2:
                        for key,s in zip(('gApplication_path','gCurrent_load_directory','gCurrent_load_name'),(b'MUTATED',b'OTHER',b'CHANGED')):u.mem_write(a[key],s+b'\0')
                    if h[4]&4:wr(a['gGraf_spec_index'],1);wr(a['gGraf_specs']+52+28,HEAP+0x380)
                    if h[4]&8:wr(a['gRender_palette'],HEAP+0xa00);wr(a['gPixelFlags'],0xaabbccdd)
                    if h[6]!=0x7fffffff:wr(state['list'],h[6])
                emit(3);snapshot();list_snapshot();u.reg_write(UC_X86_REG_ESP,f['sp']);ret(pop=4);state['frame']=None
        def hook(uc,ip,size,_):
            sp=uc.reg_read(UC_X86_REG_ESP)
            if ip==a['toupper']:
                c=rd(sp+4);ret(c-32 if 97<=c<=122 else c)
            elif ip==a['PFForEveryFile2']:
                index=state['enums'];state['enums']+=1;assert index<8
                path=string(uc.reg_read(UC_X86_REG_ECX));callback=uc.reg_read(UC_X86_REG_EDX);state['list']=rd(sp+4)
                assert callback in (a['GetFileName'],a['GetAdditionalFileName'])
                emit(1,index);text(path);emit(int(callback!=a['GetFileName']));snapshot();list_snapshot()
                state['frame']=dict(index=index,j=0,sp=sp,path=path,callback=callback);advance()
            elif ip==RETURN:
                f=state['frame'];assert f is not None and sp==f['sp']
                if signed(rd(state['list']))>f['before']:mark(f['before'])
                emit(uc.reg_read(UC_X86_REG_EAX));snapshot();list_snapshot();f['j']+=1;advance()
            elif ip==a['DRLdImg']:
                index=state['loads'];state['loads']+=1;assert index<32
                path=uc.reg_read(UC_X86_REG_ECX);name=uc.reg_read(UC_X86_REG_EDX)
                emit(4,index);text(string(path));text(string(name));emit(rd(sp+4),rd(sp+8));snapshot();list_snapshot();wr(rd(sp+12),h[8]+index)
                if index==h[10]:
                    if h[5]&1:wr(state['list'],0)
                    if h[5]&2:
                        slot=signed(rd(state['list']));assert 0<=slot<32;u.mem_write(state['list']+4+16*slot,b'EXTRA\0');mark(slot);wr(state['list'],slot+1)
                    if h[5]&4:wr(state['list'],0xffffffff)
                    if h[5]&8:u.mem_write(state['list']+4+16*(index+1),b'CHANGED\0');mark(index+1)
                    if h[5]&16:wr(a['gPixelFlags'],0x87654321);wr(a['gRender_palette'],0)
                    if h[5]&32:u.mem_write(path,b'?')
                result=0 if h[7]==1 or h[7]==2 and index&1 else HEAP+0x800+64*index
                emit(5,result);snapshot();list_snapshot();ret(result,12)
        u.mem_write(HEAP,arena)
        for key,value in zip(('gApplication_path','gCurrent_load_directory','gCurrent_load_name'),paths):u.mem_write(a[key],fixed(value,256))
        specs=bytearray(104);struct.pack_into('<I',specs,28,HEAP+0x300);struct.pack_into('<I',specs,80,HEAP+0x380);u.mem_write(a['gGraf_specs'],bytes(specs))
        for k,v in dict(gDir_separator=HEAP,gGraf_spec_index=h[3],gRender_palette=HEAP+h[2] if h[2] else 0,gPixelFlags=h[1]).items():wr(a[k],v)
        emit(0);snapshot();u.hook_add(UC_HOOK_CODE,hook)
        saved={UC_X86_REG_EBX:0x11223344,UC_X86_REG_ESI:0x22334455,UC_X86_REG_EDI:0x33445566,UC_X86_REG_EBP:0x44556677}
        for register,value in saved.items():u.reg_write(register,value)
        u.mem_write(STACK,words([STOP,h[0]]));u.reg_write(UC_X86_REG_ESP,STACK);u.reg_write(UC_X86_REG_ECX,HEAP+0x100 if not h[11] else a[('gCurrent_load_name','gCurrent_load_directory','gApplication_path')[h[11]-1]]);u.reg_write(UC_X86_REG_EDX,HEAP+0x500)
        u.emu_start(a['LoadTextureTryAllLocations'],STOP,count=1000000)
        assert u.reg_read(UC_X86_REG_EIP)==STOP and u.reg_read(UC_X86_REG_ESP)==STACK+8
        assert all(u.reg_read(r)==v for r,v in saved.items())
        emit(9,u.reg_read(UC_X86_REG_EAX));snapshot();return bytes(events)

    base_files=[['alpha.tif','Mixed.pix'],['ALPHA.p08','other.P08'],['mixed.TIF','third.tif'],['other.p16','fourth.P16'],['fall.tif'],['more.p08'],['fall.TIF'],['last.p16']]
    cases=[]
    for directory in ('CARS','COMMON','INTRFACE','common',''):
        for variant in ('all','fallback','empty','last','duplicates','reject'):
            cases.append(dict(directory=directory,variant=variant))
    for key,values in [('capacity',[0,1,2,0xffffffff]),('returns',[1,2]),('error',[1,0xffffffff]),('index',[1]),('palette',[0]),('flags',[0,0xffffffff]),('separator',['/','::','']),('name',['.TIF','ABCD','multi.dot.tif','sub\\name.pix'])]:
        cases += [dict(**{key:v}) for v in values]
    # With no separator the callback sees the entire path as a stem; keep that
    # stem within the original 16-byte list slot instead of testing an overflow.
    for case in cases:
        if case.get('separator') == '':
            case.update(app='',directory='',load_name='',gfx='',name='E.TIF')
    for enum_mutate in (1,2,4,8,15):
        for at in (0,3,4,7):cases.append(dict(enum_mutate=enum_mutate,at=at,variant='fallback' if at>=4 else 'all'))
    for mutate in (1,2,4,8,16,32,18,48):
        for variant in ('all','fallback'):cases.append(dict(load_mutate=mutate,variant=variant))
    for size in (0,-1,-2147483648):cases.append(dict(forced=size,at=3));cases.append(dict(forced=size,at=7,variant='fallback'))
    for alias in (1,2,3):cases.append(dict(alias=alias))
    for empty in ('app','directory','load_name','gfx'):cases.append({empty:''})
    rng=random.Random(0x513a30)
    for i in range(30):cases.append(dict(random=i,capacity=rng.getrandbits(32),flags=rng.getrandbits(32),error=rng.getrandbits(32),returns=rng.randrange(3)))
    records=[]
    for index,case in enumerate(cases):
        if args.case_index and index not in args.case_index:continue
        h=[case.get('capacity',32),case.get('flags',0x10203040),case.get('palette',0xa00),case.get('index',0),case.get('enum_mutate',0),case.get('load_mutate',0),case.get('forced',0x7fffffff)&0xffffffff,case.get('returns',0),case.get('error',0),case.get('at',0),0,case.get('alias',0),0,0,0,0]
        arena=bytearray(b'\xa5'*SIZE)
        for offset,value,size in [(0,case.get('separator','\\'),32),(0x20,'/',32),(0x100,case.get('name','SMOKE.TIF'),256),(0x300,case.get('gfx','64X48X8'),128),(0x380,'32X20X8',128)]:arena[offset:offset+size]=fixed(value,size)
        paths=[case.get('app','C:\\DATA'),case.get('directory','CARS'),case.get('load_name','EAGLE')]
        files=[list(x) for x in base_files];variant=case.get('variant','all')
        if variant=='fallback':files[:4]=[[] for _ in range(4)]
        elif variant=='empty':files=[[] for _ in range(8)]
        elif variant=='last':files=[[] for _ in range(7)]+[['last.p16']]
        elif variant=='duplicates':files=[['x.tif','X.TIF'],['x.p08','X.PIX'],['x.tif'],['x.p16']]+[[] for _ in range(4)]
        elif variant=='reject':files=[['readme.txt','a.P08'],['bad.bin'],['f.tiff.txt','.TIF'],[]]+[[] for _ in range(4)]
        if 'random' in case:
            case_rng=random.Random(0x513a30+case['random'])
            files=[[f'{case_rng.choice(["a","b","C","d"])}.{case_rng.choice(["TIF","pix","p08","P16","txt"])}' for _ in range(case_rng.randrange(4))] for _ in range(8)]
        raw=words(h)+arena+b''.join(fixed(s,256) for s in paths)+words([len(x) for x in files])+b''.join(fixed(s,64) for names in files for s in names+['']*(4-len(names)))
        fixture=out/'case.bin';fixture.write_bytes(raw)
        try:
            expected=execute(*images[0],h,bytes(arena),paths,files)
        except Exception:
            (out/f'original-error-{index}.json').write_text(json.dumps(dict(case=case,paths=paths,files=files),indent=2))
            print(f'Original execution failed in fixture {index}: {case}',flush=True)
            raise
        checks=[(compiler,subprocess.check_output([str(out/f'{compiler}.exe'),str(fixture)])) for compiler in commands]
        if len(images)>1:checks.append(('matching PE',execute(*images[1],h,bytes(arena),paths,files)))
        for compiler,actual in checks:
            if actual!=expected:
                prefix=out/f'failure-{index}-{compiler.replace(" ","-")}'
                prefix.with_suffix('.expected.bin').write_bytes(expected);prefix.with_suffix('.actual.bin').write_bytes(actual)
                prefix.with_suffix('.json').write_text(json.dumps(dict(case=case,first_difference=next((i for i,(a,b) in enumerate(zip(expected,actual)) if a!=b),None),expected_size=len(expected),actual_size=len(actual)),indent=2))
                raise AssertionError(f'{compiler} texture search case {index} differs; see {prefix}')
            records.append(dict(case_index=index,compiler=compiler,fixture_sha256=hashlib.sha256(raw).hexdigest(),state_and_callbacks_sha256=hashlib.sha256(actual).hexdigest()))
    manifest=dict(original_sha256=HASH,verifier_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),source_sha256={str(f.relative_to(ROOT)):hashlib.sha256(f.read_bytes()).hexdigest() for f in (SOURCE,WORLD)},body_sha256=[hashlib.sha256(b.encode()).hexdigest() for b in bodies],compiler_commands=commands,scenario_count=len({r['case_index'] for r in records}),comparisons=len(records),complete_suite=not args.case_index,cases=records,limitations='ASCII fixed-buffer paths and bounded lists; OS enumeration and image decode are controlled, actual filename callbacks run. Uninitialized private list bytes are excluded; all initialized list bytes, external state, ordered paths/arguments and exact return/RET4 ABI are checked.')
    if args.symbols:manifest.update(recompiled_sha256=hashlib.sha256(images[1][0]).hexdigest(),symbols_sha256=hashlib.sha256(args.symbols.read_bytes()).hexdigest())
    (out/'verification.json').write_text(json.dumps(manifest,indent=2)+'\n')
    print(f'PASS: {manifest["scenario_count"]} texture search scenarios, {len(records)} comparisons.')


if __name__=='__main__':main()
