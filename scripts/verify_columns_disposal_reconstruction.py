#!/usr/bin/env python3
"""Verify actual DisposeColumns against original x86 and three native compilers.

BRender registry/model/actor/memory release routines are explicit controlled
resource boundaries. They poison retained fixture storage without allocator
release, so duplicate references/frees remain observable rather than hidden.
Complete repository track/grid/actor/model/list arenas and globals are captured
at each ordered callback. Pointer/count replacement callbacks test original
reload versus capture behavior. No floating arithmetic or x87 normalization.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import struct
import subprocess
import sys
from verify_tinted_native_reconstruction import extract

ROOT=Path(__file__).resolve().parents[1]
SOURCE=ROOT/'src/carma2/newcommon/37-brucetrk.c'
HASH='9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
ADDRESSES=dict(DisposeColumns=0x40ca90,gCount_track_non_cars=0x679260,gNon_track_actor=0x7634b8,BrModelRemove=0x51ed30,BrModelFree=0x5217e0,BrMemFree=0x5275f0,BrActorRemove=0x51e070,BrActorFree=0x51e2c0)
FIELDS='nx nz count start pattern nc_pattern matrix list mutate nx_new nz_new count_new start_new seed'.split()
HEAP,STACK,STOP,SIZE=0x20000000,0x1080000,0x1081000,0x10000
def words(values):return struct.pack('<'+'I'*len(values),*[v&0xffffffff for v in values])

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE')
    p.add_argument('--dependency-dir',type=Path,default=ROOT/'build-audit/joystick/python-deps')
    p.add_argument('--gcc',type=Path,default=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe')
    p.add_argument('--msvc-root',type=Path,default=ROOT/'build-tools/msvc-modern')
    p.add_argument('--msvc5-root',type=Path,default=ROOT/'build-tools/msvc500sp3')
    p.add_argument('--output',type=Path,default=ROOT/'build-audit/columns-disposal-tests')
    p.add_argument('--symbols',type=Path)
    p.add_argument('--recompiled',type=Path,default=ROOT/'build-matching/rec2.exe')
    p.add_argument('--case-index',type=int,action='append')
    args=p.parse_args();sys.path.insert(0,str(args.dependency_dir.resolve()))
    from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
    from unicorn.x86_const import UC_X86_REG_EIP,UC_X86_REG_ESP,UC_X86_REG_EAX,UC_X86_REG_ECX,UC_X86_REG_EBX,UC_X86_REG_ESI,UC_X86_REG_EDI,UC_X86_REG_EBP
    binary=args.original.read_bytes()
    if hashlib.sha256(binary).hexdigest()!=HASH:raise SystemExit('Original executable hash differs')
    images=[(binary,ADDRESSES)]
    if args.symbols:
        m=json.loads(args.symbols.read_text());b=args.recompiled.read_bytes()
        if m['format']!=1 or m['original_sha256']!=HASH or m['recompiled_sha256']!=hashlib.sha256(b).hexdigest():raise SystemExit('Stale or incompatible columns disposal symbol map')
        mapped={s['orig']:s['recomp'] for s in m['symbols']};images.append((b,{n:mapped[a] for n,a in ADDRESSES.items()}))
    out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)
    driver=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include "37-brucetrk.h"
#include "globvars.h"
typedef char layouts[sizeof(void*)==4&&sizeof(tTrack_spec)==36&&sizeof(tTrack_square)==12&&sizeof(br_actor)==100&&sizeof(br_model)==92?1:-1];
enum {NX,NZ,COUNT,START,COLUMNS_PATTERN,NC_COLUMNS_PATTERN,MATRIX,LIST,MUTATE,NX_NEW,NZ_NEW,COUNT_NEW,START_NEW,SEED};
static unsigned a[14],model_frees,mem_frees,actor_frees;static unsigned char* heap;
int gCount_track_non_cars;br_actor* gNon_track_actor;
#define OBJ(type,off) ((type*)(heap+(off)))
#define TRACK OBJ(tTrack_spec,0x100)
static void C2_HOOK_CDECL event(unsigned code,unsigned n,const unsigned* values) {unsigned v[9],g[2],i;memset(v,0,sizeof(v));v[0]=code;for(i=0;i<n;i++)v[i+1]=values[i];fwrite(v,sizeof(v),1,stdout);g[0]=(unsigned)gCount_track_non_cars;g[1]=(unsigned)gNon_track_actor;fwrite(g,sizeof(g),1,stdout);fwrite(heap,0x10000,1,stdout);}
br_model* C2_HOOK_CDECL BrModelRemove(br_model* model) {unsigned i,v=(unsigned)model;event(1,1,&v);if(a[MUTATE]&1)for(i=0;i<32;i++){br_actor* actor=OBJ(br_actor,0x3000+(i/16)*0x800+(i%16)*100);if(actor->model==model)actor->model=OBJ(br_model,0x6000);}if(a[MUTATE]&128)TRACK->columns=NULL;return model;}
void C2_HOOK_CDECL BrModelFree(br_model* model) {unsigned v=(unsigned)model;event(2,1,&v);memset(model,0xdd,sizeof(*model));if(model_frees++==0){if(a[MUTATE]&2)TRACK->columns=OBJ(tTrack_square*,0x1100);if(a[MUTATE]&512){TRACK->ncolumns_x=(tU8)a[NX_NEW];TRACK->ncolumns_z=(tU8)a[NZ_NEW];}}}
void C2_HOOK_CDECL BrMemFree(void* memory) {unsigned off=(unsigned)memory-(unsigned)heap,size=0,v=(unsigned)memory;event(3,1,&v);if(memory){if(off==0x1000||off==0x1100)size=16;else if(off>=0x2000&&off<0x2800)size=128;else if(off==0x7020||off==0x7120)size=32;else abort();memset(memory,0xdd,size);}if(mem_frees++==0){if(a[MUTATE]&4)TRACK->columns=OBJ(tTrack_square*,0x1100);if(a[MUTATE]&8)TRACK->ncolumns_z=(tU8)a[NZ_NEW];if(a[MUTATE]&64)gCount_track_non_cars=(int)a[START_NEW];}}
br_actor* C2_HOOK_CDECL BrActorRemove(br_actor* actor) {unsigned v=(unsigned)actor;event(4,1,&v);actor->parent=NULL;if(a[MUTATE]&16)TRACK->non_car_list=OBJ(br_actor*,0x7120);return actor;}
void C2_HOOK_CDECL BrActorFree(br_actor* actor) {unsigned v=(unsigned)actor;event(5,1,&v);memset(actor,0xee,sizeof(*actor));if(actor_frees++==0){if(a[MUTATE]&32)TRACK->count_non_cars=(int)a[COUNT_NEW];if(a[MUTATE]&256){TRACK->non_car_list=NULL;TRACK->count_non_cars=(int)a[COUNT_NEW];}if(a[MUTATE]&1024)gNon_track_actor=NULL;if(a[MUTATE]&2048)gCount_track_non_cars=(int)a[START_NEW];}}
'''+extract(SOURCE.read_text(),'DisposeColumns')+r'''
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned i,g,r,c;int j;br_actor* actor;br_actor** list;tTrack_square* row;
 if(argc!=2)return 90;
 f=fopen(argv[1],"rb");if(!f)return 91;if(fread(a,sizeof(a),1,f)!=1)return 92;fclose(f);
 heap=(unsigned char*)VirtualAlloc((void*)0x20000000,0x10000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(heap!=(void*)0x20000000)return 93;
 _setmode(_fileno(stdout),_O_BINARY);for(i=0;i<0x10000;i++)heap[i]=(unsigned char)(i*37+a[SEED]);
 for(g=0;g<2;g++)for(r=0;r<4;r++){row=OBJ(tTrack_square,0x2000+g*0x400+r*0x80);OBJ(tTrack_square*,0x1000+g*0x100)[r]=row;for(c=0;c<4;c++){i=4*r+c;actor=OBJ(br_actor,0x3000+g*0x800+i*100);actor->model=a[COLUMNS_PATTERN]==3||(a[COLUMNS_PATTERN]==1&&i%3==1)?NULL:OBJ(br_model,0x5000+g*0x800+(a[COLUMNS_PATTERN]==4?0:i)*96);row[c].actor_0x8=a[COLUMNS_PATTERN]==2||(a[COLUMNS_PATTERN]==1&&i%3==0)?NULL:actor;}}
 gNon_track_actor=OBJ(br_actor,0x9000);
 for(g=0;g<2;g++){list=OBJ(br_actor*,0x7020+g*0x100);for(j=-2;j<8;j++){actor=OBJ(br_actor,0x8000+g*0x2000+(j+2)*100);actor->parent=a[NC_COLUMNS_PATTERN]==1||(a[NC_COLUMNS_PATTERN]==3&&(j+3)%3==1)?gNon_track_actor:NULL;list[j]=a[NC_COLUMNS_PATTERN]==2||(a[NC_COLUMNS_PATTERN]==3&&(j+3)%3==0)?NULL:actor;}}
 TRACK->ncolumns_x=(tU8)a[NX];TRACK->ncolumns_z=(tU8)a[NZ];TRACK->columns=a[MATRIX]?OBJ(tTrack_square*,0x1000):NULL;TRACK->count_non_cars=(int)a[COUNT];TRACK->non_car_list=a[LIST]?OBJ(br_actor*,0x7020):NULL;gCount_track_non_cars=(int)a[START];
 DisposeColumns(TRACK);event(0,0,NULL);return 0;
}
'''
    harness=out/'harness.c';harness.write_text(driver)
    includes=[ROOT/path for path in ('src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/carma2/platform','src/carma2/s3/include','src/brender/include')]
    msvc=sorted((args.msvc_root/'VC/Tools/MSVC').iterdir())[-1];sdk=args.msvc_root/'Windows Kits/10';version=sorted((sdk/'Include').iterdir())[-1].name
    env=dict(os.environ);env['PATH']=str(msvc/'bin/Hostx64/x86')+os.pathsep+env['PATH'];env['INCLUDE']=os.pathsep.join(map(str,[msvc/'include',*[sdk/f'Include/{version}/{part}' for part in ('ucrt','shared','um')]]));env['LIB']=os.pathsep.join(map(str,[msvc/'lib/x86',*[sdk/f'Lib/{version}/{part}/x86' for part in ('ucrt','um')]]))
    msvc5=args.msvc5_root.resolve();env5=dict(os.environ);env5['PATH']=str(msvc5/'bin')+os.pathsep+str(msvc5/'redist')+os.pathsep+env5['PATH'];env5['INCLUDE']=str(msvc5/'include');env5['LIB']=str(msvc5/'lib')
    commands={
        'msvc':[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(p) for p in includes],str(harness),'/Fo'+str(out/'msvc.obj'),'/Fe'+str(out/'msvc.exe')],
        'msvc5':[str(msvc5/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(p) for p in includes],str(harness),'/Fo'+str(out/'msvc5.obj'),'/Fe'+str(out/'msvc5.exe')],
        'mingw':[str(args.gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-fno-strict-aliasing',*['-I'+str(p) for p in includes],str(harness),'-o',str(out/'mingw.exe')],
    }
    for compiler,command in commands.items():
        r=subprocess.run(command,cwd=out,env=env if compiler=='msvc' else env5 if compiler=='msvc5' else None,text=True,capture_output=True);(out/f'{compiler}-build.txt').write_text(r.stdout+r.stderr)
        if r.returncode:raise RuntimeError(r.stdout+r.stderr)
    def execute(image,a,c):
        u=Uc(UC_ARCH_X86,UC_MODE_32);pe=struct.unpack_from('<I',image,60)[0];base=struct.unpack_from('<I',image,pe+52)[0];size=struct.unpack_from('<I',image,pe+80)[0];u.mem_map(base,(size+4095)&~4095);u.mem_map(HEAP,SIZE);u.mem_map(STACK-0x8000,0x10000)
        table=pe+24+struct.unpack_from('<H',image,pe+20)[0]
        for i in range(struct.unpack_from('<H',image,pe+6)[0]):
            _,rva,size,offset=struct.unpack_from('<IIII',image,table+40*i+8);u.mem_write(base+rva,image[offset:offset+size])
        def wr(p,v):u.mem_write(p,words([v]))
        def rd(p):return struct.unpack('<I',u.mem_read(p,4))[0]
        u.mem_write(HEAP,bytes((i*37+c['seed'])&255 for i in range(SIZE)))
        for g in range(2):
            for r in range(4):
                row=HEAP+0x2000+g*0x400+r*0x80;wr(HEAP+0x1000+g*0x100+4*r,row)
                for col in range(4):
                    i=4*r+col;actor=HEAP+0x3000+g*0x800+i*100;model=0 if c['pattern']==3 or c['pattern']==1 and i%3==1 else HEAP+0x5000+g*0x800+(0 if c['pattern']==4 else i)*96
                    wr(actor+24,model);wr(row+12*col+8,0 if c['pattern']==2 or c['pattern']==1 and i%3==0 else actor)
        for g in range(2):
            for j in range(-2,8):
                actor=HEAP+0x8000+g*0x2000+(j+2)*100;wr(actor+12,HEAP+0x9000 if c['nc_pattern']==1 or c['nc_pattern']==3 and j%3==1 else 0);wr(HEAP+0x7020+g*0x100+4*j,0 if c['nc_pattern']==2 or c['nc_pattern']==3 and j%3==0 else actor)
        track=HEAP+0x100;u.mem_write(track,bytes([c['nx'],c['nz']]));wr(track+24,HEAP+0x1000 if c['matrix'] else 0);wr(track+28,c['count']);wr(track+32,HEAP+0x7020 if c['list'] else 0);wr(a['gCount_track_non_cars'],c['start']);wr(a['gNon_track_actor'],HEAP+0x9000)
        output=bytearray();counts=dict(model=0,mem=0,actor=0)
        def event(code,v):output.extend(words([code,v,*([0]*7)] if code else [0]*9));output.extend(words([rd(a['gCount_track_non_cars']),rd(a['gNon_track_actor'])]));output.extend(u.mem_read(HEAP,SIZE))
        reverse={a[n]:i+1 for i,n in enumerate(('BrModelRemove','BrModelFree','BrMemFree','BrActorRemove','BrActorFree'))}
        def hook(uc,address,size,context):
            code=reverse[address];sp=uc.reg_read(UC_X86_REG_ESP);pointer=rd(sp+4);event(code,pointer);result=pointer
            if code==1:
                if c['mutate']&1:
                    for i in range(32):
                        actor=HEAP+0x3000+(i//16)*0x800+(i%16)*100
                        if rd(actor+24)==pointer:wr(actor+24,HEAP+0x6000)
                if c['mutate']&128:wr(track+24,0)
            elif code==2:
                uc.mem_write(pointer,b'\xdd'*92)
                if counts['model']==0:
                    if c['mutate']&2:wr(track+24,HEAP+0x1100)
                    if c['mutate']&512:uc.mem_write(track,bytes([c['nx_new'],c['nz_new']]))
                counts['model']+=1
            elif code==3:
                off=pointer-HEAP
                if pointer:
                    size=16 if off in (0x1000,0x1100) else 128 if 0x2000<=off<0x2800 else 32 if off in (0x7020,0x7120) else None
                    assert size is not None;uc.mem_write(pointer,b'\xdd'*size)
                if counts['mem']==0:
                    if c['mutate']&4:wr(track+24,HEAP+0x1100)
                    if c['mutate']&8:uc.mem_write(track+1,bytes([c['nz_new']]))
                    if c['mutate']&64:wr(a['gCount_track_non_cars'],c['start_new'])
                counts['mem']+=1
            elif code==4:
                wr(pointer+12,0)
                if c['mutate']&16:wr(track+32,HEAP+0x7120)
            else:
                uc.mem_write(pointer,b'\xee'*100)
                if counts['actor']==0:
                    if c['mutate']&32:wr(track+28,c['count_new'])
                    if c['mutate']&256:wr(track+32,0);wr(track+28,c['count_new'])
                    if c['mutate']&1024:wr(a['gNon_track_actor'],0)
                    if c['mutate']&2048:wr(a['gCount_track_non_cars'],c['start_new'])
                counts['actor']+=1
            uc.reg_write(UC_X86_REG_EAX,result);uc.reg_write(UC_X86_REG_EIP,rd(sp));uc.reg_write(UC_X86_REG_ESP,sp+4)
        for address in reverse:u.hook_add(UC_HOOK_CODE,hook,begin=address,end=address)
        saved={UC_X86_REG_EBX:0x12345678,UC_X86_REG_ESI:0x23456789,UC_X86_REG_EDI:0x3456789a,UC_X86_REG_EBP:0x456789ab}
        for r,v in saved.items():u.reg_write(r,v)
        u.mem_write(STACK,words([STOP]));u.reg_write(UC_X86_REG_ESP,STACK);u.reg_write(UC_X86_REG_ECX,track);u.emu_start(a['DisposeColumns'],STOP,count=1000000)
        assert u.reg_read(UC_X86_REG_EIP)==STOP and u.reg_read(UC_X86_REG_ESP)==STACK+4,'Columns disposal plain-RET ABI differs';assert all(u.reg_read(r)==v for r,v in saved.items())
        event(0,0);return bytes(output)
    def case(**kw):
        c={f:0 for f in FIELDS};c.update(nx=2,nz=2,count=3,start=0,matrix=1,list=1,nx_new=3,nz_new=3,count_new=5,start_new=1,seed=165);c.update(kw);return c
    cases=[case(nx=nx,nz=nz,pattern=pattern,nc_pattern=nc,list=lst) for nx,nz in ((0,0),(0,2),(2,0),(1,1),(2,3),(3,2)) for pattern in range(5) for nc in range(4) for lst in (0,1)]
    cases += [case(nx=0,nz=nz,matrix=0,count=count,start=start) for nz in (0,2) for count in (-1,0,3) for start in (-2,0,1,3,4)]
    cases += [case(mutate=m,nc_pattern=nc) for m in (1,2,3,4,8,16,32,48,64,65,512,513,514) for nc in (0,3)]
    cases += [case(mutate=512,nx_new=nx,nz_new=nz) for nx,nz in ((1,1),(1,3),(3,1),(3,3))]
    cases += [case(mutate=8,nz_new=nz) for nz in (1,3)]
    cases += [case(nx=1,nz=1,mutate=128),case(mutate=256,count_new=1)]
    cases += [case(mutate=m,nc_pattern=nc,start_new=7) for m in (1024,2048,3072,3088) for nc in (0,3)]
    if args.case_index is not None and any(i<0 or i>=len(cases) for i in args.case_index):p.error(f'--case-index must be between 0 and {len(cases)-1}')
    records=[]
    for index,c in enumerate(cases):
        if args.case_index is not None and index not in args.case_index:continue
        fixture=out/'case.bin';fixture.write_bytes(words([c[f] for f in FIELDS]));expected=execute(*images[0],c);checks=[(compiler,subprocess.check_output([str(out/f'{compiler}.exe'),str(fixture)])) for compiler in commands]
        if len(images)>1:checks.append(('matching PE',execute(*images[1],c)))
        for compiler,actual in checks:
            if actual!=expected:
                prefix=out/f'failure-{compiler.replace(" ","-")}-{index}';prefix.with_suffix('.expected.bin').write_bytes(expected);prefix.with_suffix('.actual.bin').write_bytes(actual);diff=next((i for i,(a,b) in enumerate(zip(actual,expected)) if a!=b),None);prefix.with_suffix('.json').write_text(json.dumps(dict(case=c,first_difference=diff,expected_size=len(expected),actual_size=len(actual)),indent=2));raise AssertionError(f'Columns {compiler} case {index} differs; see {out}')
            records.append(dict(compiler=compiler,case_index=index,case=c,state_and_callbacks_sha256=hashlib.sha256(actual).hexdigest()))
        if (index+1)%100==0:print(f'Verified {index+1}/{len(cases)} columns disposal cases',flush=True)
    manifest=dict(original_sha256=HASH,function=0x40ca90,global_address=0x679260,scenario_count=len(cases),comparisons=len(records),complete_suite=args.case_index is None,all_cases_passed=True,compiler_commands=commands,harness_sha256=hashlib.sha256(driver.encode()).hexdigest(),source_sha256={str(path.relative_to(ROOT)):hashlib.sha256(path.read_bytes()).hexdigest() for path in (SOURCE,ROOT/'src/carma2/newcommon/37-brucetrk.h',ROOT/'src/carma2/include/rec2_types.h')},cases=records,limitations='Resource callbacks are explicit controlled boundaries. Freed storage is poisoned and retained; duplicate frees remain visible. Callback count/pointer changes stay inside allocated fixture bounds; original unchecked invalid cases are not normalized. Full arenas/globals and ordered call args are compared; original/matching callee-saved registers and plain RET checked.')
    if len(images)>1:manifest.update(recompiled_sha256=hashlib.sha256(images[1][0]).hexdigest(),symbols_sha256=hashlib.sha256(args.symbols.read_bytes()).hexdigest())
    (out/'verification.json').write_text(json.dumps(manifest,indent=2)+'\n');print(f'Passed {len(records)} original/native columns disposal comparisons')

if __name__=='__main__':main()

