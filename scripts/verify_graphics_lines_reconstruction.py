#!/usr/bin/env python3
"""Verify actual PoshDrawLine/DisableLights against original x86.

Actual DRDrawLine and PaletteEntry16Bit bodies execute. BRender line/failure
and light-disable are explicit boundaries with complete arena/global/table
snapshots and callback mutations. Angle comparisons never expose NaN payloads
through scalar stores; all quiet/signaling NaN inputs must select original
unordered branches. Native coordinate/colour wrap is also compared.
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
SOURCE=ROOT/'src/carma2/newcommon/18-graphics2.c'
UTILITY=ROOT/'src/carma2/newcommon/41-utility.c'
HASH='9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
FUNCTIONS=dict(PoshDrawLine=0x47c740,DisableLights=0x47d6a0)
GLOBALS=dict(gBack_screen=0x74d360,gRender_palette=0x74a674,gNumber_of_lights=0x68c720,gLight_array=0x74b3e0)
BOUNDARIES=dict(BrPixelmapLine=0x538990,BrFailure=0x527c80,BrLightDisable=0x524ec0)
ADDRESSES=dict(FUNCTIONS,**GLOBALS,**BOUNDARIES)
FIELDS='mode angle colour x1 y1 x2 y2 backtype alt_type mutate count new_count replace repeat seed'.split()
HEAP,STACK,STOP,SIZE=0x20000000,0x1080000,0x1081000,0x10000
def words(values):return struct.pack('<'+'I'*len(values),*[v&0xffffffff for v in values])

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE')
    p.add_argument('--dependency-dir',type=Path,default=ROOT/'build-audit/joystick/python-deps')
    p.add_argument('--gcc',type=Path,default=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe')
    p.add_argument('--msvc-root',type=Path,default=ROOT/'build-tools/msvc-modern')
    p.add_argument('--msvc5-root',type=Path,default=ROOT/'build-tools/msvc500sp3')
    p.add_argument('--output',type=Path,default=ROOT/'build-audit/graphics-lines-tests')
    p.add_argument('--symbols',type=Path)
    p.add_argument('--recompiled',type=Path,default=ROOT/'build-matching/rec2.exe')
    p.add_argument('--case-index',type=int,action='append')
    args=p.parse_args();sys.path.insert(0,str(args.dependency_dir.resolve()))
    from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
    from unicorn.x86_const import UC_X86_REG_EIP,UC_X86_REG_ESP,UC_X86_REG_EAX,UC_X86_REG_ECX,UC_X86_REG_EDX,UC_X86_REG_FPCW
    binary=args.original.read_bytes()
    if hashlib.sha256(binary).hexdigest()!=HASH:raise SystemExit('Original executable hash differs')
    images=[(binary,ADDRESSES)]
    if args.symbols:
        m=json.loads(args.symbols.read_text());b=args.recompiled.read_bytes()
        if m['format']!=1 or m['original_sha256']!=HASH or m['recompiled_sha256']!=hashlib.sha256(b).hexdigest():raise SystemExit('Stale or incompatible graphics symbol map')
        mapped={s['orig']:s['recomp'] for s in m['symbols']};images.append((b,{name:mapped[a] for name,a in ADDRESSES.items()}))
    out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)
    driver=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "18-graphics2.h"
#include "41-utility.h"
#include "globvars.h"
typedef char layouts[sizeof(void*)==4&&sizeof(br_pixelmap)==68&&sizeof(br_actor)==100?1:-1];
enum {MODE,ANGLE,COLOUR,X1,Y1,X2,Y2,BACKTYPE,ALT_TYPE,MUTATE,COUNT,NEW_COUNT,REPLACE,REPEAT,SEED};
static unsigned a[15],line_calls,light_calls;static unsigned char* heap;
br_pixelmap *gBack_screen,*gRender_palette;int gNumber_of_lights;br_actor* gLight_array[50];
#define OBJ(type,off) ((type*)(heap+(off)))
static void C2_HOOK_CDECL event(unsigned code,unsigned n,const unsigned* values) {
 unsigned v[9],g[3],i;memset(v,0,sizeof(v));v[0]=code;for(i=0;i<n;i++)v[i+1]=values[i];fwrite(v,sizeof(v),1,stdout);
 g[0]=(unsigned)gBack_screen;g[1]=(unsigned)gRender_palette;g[2]=(unsigned)gNumber_of_lights;fwrite(g,sizeof(g),1,stdout);fwrite(gLight_array,sizeof(gLight_array),1,stdout);fwrite(heap,0x10000,1,stdout);
}
void C2_HOOK_CDECL BrPixelmapLine(br_pixelmap* dst,br_int_32 x1,br_int_32 y1,br_int_32 x2,br_int_32 y2,br_uint_32 colour) {unsigned v[6];v[0]=(unsigned)dst;v[1]=x1;v[2]=y1;v[3]=x2;v[4]=y2;v[5]=colour;event(1,6,v);memcpy(dst->pixels,&colour,4);line_calls++;if(a[MUTATE]&1)gBack_screen=OBJ(br_pixelmap,0x200);if(a[MUTATE]&2)gRender_palette=OBJ(br_pixelmap,0x400);}
void C2_HOOK_CDECL BrFailure(const char* message,...) {unsigned v=strcmp(message,"Unsupported back buffer type.")==0;event(2,1,&v);event(0,0,NULL);exit(0);}
void C2_HOOK_CDECL BrLightDisable(br_actor* actor) {unsigned v[2];v[0]=(unsigned)actor;v[1]=light_calls;event(3,2,v);actor->render_style=(br_uint_8)(0x80+light_calls);light_calls++;if(a[MUTATE])gNumber_of_lights=(int)a[NEW_COUNT];if(a[REPLACE])gLight_array[1]=OBJ(br_actor,0x2100);}
'''
    driver+=extract(UTILITY.read_text(),'PaletteEntry16Bit')
    driver+='\n'.join(extract(SOURCE.read_text(),name) for name in ('DRDrawLine',*FUNCTIONS))
    driver+=r'''
int C2_HOOK_CDECL main(int argc,char** argv) {
 FILE* f;unsigned i,j,value;float angle;
 if(argc!=2)return 90;
 if(!strcmp(argv[1],"schema")){printf("[68,100]\n");return 0;}
 f=fopen(argv[1],"rb");if(!f)return 91;if(fread(a,sizeof(a),1,f)!=1)return 92;fclose(f);
 heap=(unsigned char*)VirtualAlloc((void*)0x20000000,0x10000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(heap!=(void*)0x20000000)return 93;
 _setmode(_fileno(stdout),_O_BINARY);_controlfp(_PC_53,_MCW_PC);for(i=0;i<0x10000;i++)heap[i]=(unsigned char)(i*37+a[SEED]);
 for(i=0;i<4;i++){OBJ(br_pixelmap,0x100+0x100*i)->pixels=heap+0x5000+0x1000*i;OBJ(br_pixelmap,0x100+0x100*i)->type=(br_uint_8)(i==1?a[ALT_TYPE]:a[BACKTYPE]);}
 OBJ(br_pixelmap,0x500)->pixels=heap+0x9000;OBJ(br_pixelmap,0x500)->type=(br_uint_8)(a[BACKTYPE]^7);
 for(i=0;i<1024;i++){value=0xa5000000|(i*0x10307&0xffffff);memcpy(heap+0x7000+4*i,&value,4);value=0xb5000000|(i*0x30509&0xffffff);memcpy(heap+0x8000+4*i,&value,4);}
 for(i=0;i<50;i++)gLight_array[i]=OBJ(br_actor,0x1000+i*100);
 gBack_screen=OBJ(br_pixelmap,0x100);gRender_palette=OBJ(br_pixelmap,0x300);gNumber_of_lights=(int)a[COUNT];memcpy(&angle,a+ANGLE,4);
 for(j=0;j<a[REPEAT];j++)switch(a[MODE]){
 case 0:PoshDrawLine(OBJ(br_pixelmap,0x500),(int)a[X1],angle,(int)a[Y1],(int)a[X2],(int)a[Y2],(int)a[COLOUR]);break;
 case 1:DisableLights();break;default:return 94;}
 event(0,0,NULL);return 0;
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
        'mingw':[str(args.gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror',*['-I'+str(p) for p in includes],str(harness),'-o',str(out/'mingw.exe')],
    }
    for compiler,command in commands.items():
        r=subprocess.run(command,cwd=out,env=env if compiler=='msvc' else env5 if compiler=='msvc5' else None,text=True,capture_output=True);(out/f'{compiler}-build.txt').write_text(r.stdout+r.stderr)
        if r.returncode:raise RuntimeError(r.stdout+r.stderr)
        assert json.loads(subprocess.check_output([str(out/f'{compiler}.exe'),'schema'],text=True))==[68,100]
    def original(image,addresses,c):
        u=Uc(UC_ARCH_X86,UC_MODE_32);pe=struct.unpack_from('<I',image,60)[0];base=struct.unpack_from('<I',image,pe+52)[0];size=struct.unpack_from('<I',image,pe+80)[0];u.mem_map(base,(size+4095)&~4095);u.mem_map(HEAP,SIZE);u.mem_map(STACK-0x8000,0x10000)
        table=pe+24+struct.unpack_from('<H',image,pe+20)[0]
        for i in range(struct.unpack_from('<H',image,pe+6)[0]):
            _,rva,size,offset=struct.unpack_from('<IIII',image,table+40*i+8);u.mem_write(base+rva,image[offset:offset+size])
        def wr(a,v):u.mem_write(a,words([v]))
        def rd(a):return struct.unpack('<I',u.mem_read(a,4))[0]
        u.mem_write(HEAP,bytes((i*37+c['seed'])&255 for i in range(SIZE)))
        for i in range(4):wr(HEAP+0x100+0x100*i+8,HEAP+0x5000+0x1000*i);u.mem_write(HEAP+0x100+0x100*i+44,bytes([(c['alt_type'] if i==1 else c['backtype'])&255]))
        wr(HEAP+0x500+8,HEAP+0x9000);u.mem_write(HEAP+0x500+44,bytes([(c['backtype']^7)&255]))
        u.mem_write(HEAP+0x7000,words([0xa5000000|(i*0x10307&0xffffff) for i in range(1024)]));u.mem_write(HEAP+0x8000,words([0xb5000000|(i*0x30509&0xffffff) for i in range(1024)]))
        u.mem_write(addresses['gLight_array'],words([HEAP+0x1000+i*100 for i in range(50)]))
        for name,value in zip(GLOBALS,(HEAP+0x100,HEAP+0x300,c['count'])):wr(addresses[name],value)
        output=bytearray();line_calls=0;light_calls=0;failed=False
        def event(code,values):output.extend(words([code,*values,*([0]*(8-len(values)))]));output.extend(words([rd(addresses[n]) for n in list(GLOBALS)[:3]]));output.extend(u.mem_read(addresses['gLight_array'],200));output.extend(u.mem_read(HEAP,SIZE))
        reverse={addresses[n]:i+1 for i,n in enumerate(BOUNDARIES)}
        def hook(uc,address,size,context):
            nonlocal line_calls,light_calls,failed
            code=reverse[address];sp=uc.reg_read(UC_X86_REG_ESP)
            if code==1:
                vals=[rd(sp+4+4*i) for i in range(6)];event(code,vals);wr(rd(vals[0]+8),vals[5]);line_calls+=1
                if c['mutate']&1:wr(addresses['gBack_screen'],HEAP+0x200)
                if c['mutate']&2:wr(addresses['gRender_palette'],HEAP+0x400)
            elif code==2:
                pointer=rd(sp+4);valid=bytes(uc.mem_read(pointer,30))==b'Unsupported back buffer type.\0';event(code,[int(valid)]);event(0,[]);failed=True;uc.emu_stop();return
            else:
                actor=rd(sp+4);event(code,[actor,light_calls]);uc.mem_write(actor+32,bytes([(0x80+light_calls)&255]));light_calls+=1
                if c['mutate']:wr(addresses['gNumber_of_lights'],c['new_count'])
                if c['replace']:wr(addresses['gLight_array']+4,HEAP+0x2100)
            uc.reg_write(UC_X86_REG_EAX,0);uc.reg_write(UC_X86_REG_EIP,rd(sp));uc.reg_write(UC_X86_REG_ESP,sp+4)
        for a in reverse:u.hook_add(UC_HOOK_CODE,hook,begin=a,end=a)
        for _ in range(c['repeat']):
            vals=[c[f] for f in ('angle','y1','x2','y2','colour')] if c['mode']==0 else []
            u.mem_write(STACK,words([STOP,*vals]));u.reg_write(UC_X86_REG_ESP,STACK);u.reg_write(UC_X86_REG_ECX,HEAP+0x500);u.reg_write(UC_X86_REG_EDX,c['x1']&0xffffffff);u.reg_write(UC_X86_REG_FPCW,0x27f);u.emu_start(addresses[list(FUNCTIONS)[c['mode']]],STOP,count=1000000)
            if failed:return bytes(output)
            assert u.reg_read(UC_X86_REG_EIP)==STOP,'Graphics instruction budget exhausted'
            assert u.reg_read(UC_X86_REG_ESP)==STACK+4+len(vals)*4,'Graphics ABI stack cleanup differs'
        event(0,[]);return bytes(output)
    def fb(x):return struct.unpack('<I',struct.pack('<f',x))[0]
    def case(mode,**kw):
        c={f:0 for f in FIELDS};c.update(mode=mode,angle=fb(1),colour=-17,x1=11,y1=13,x2=29,y2=31,backtype=3,alt_type=5,count=3,new_count=3,repeat=1,seed=165);c.update(kw);return c
    angles=[fb(t)+delta for t in (0.785,1.57,2.356,3.926,4.712,5.498) for delta in (-1,0,1)]
    angles += [fb(x) for x in (-1,0,3.0,6.3)] + [0x80000000,0x7f800000,0xff800000,0x7fc00001,0xffc12345,0x7f800001,0xff800001]
    cases=[case(0,angle=angle,backtype=t,colour=colour) for angle in angles for t in (3,4,5) for colour in (-17,17)]
    cases += [case(0,angle=fb(angle),backtype=t,alt_type=alt,mutate=mutate,repeat=2) for angle in (0,1,2,4,5) for t,alt in ((3,5),(4,3),(5,4)) for mutate in (1,2,3)]
    cases += [case(0,angle=angle,colour=colour,x1=x,y1=x,x2=y,y2=y) for angle in (fb(0),fb(1),fb(2),fb(4),0x7f800001) for colour in (-2147483648,-1,0,255) for x,y in ((2147483647,-2147483648),(-2147483648,2147483647))]
    cases += [case(0,backtype=t,angle=fb(a),mutate=m,alt_type=6) for t in (0,6,255,3,5) for a in (0,1,4) for m in (0,1)]
    cases += [case(1,count=count,new_count=new,mutate=mutate,replace=replace,repeat=2) for count in (-1,0,1,3,50) for new in (-1,0,1,3,50) for mutate in (0,1) for replace in (0,1)]
    if args.case_index is not None and any(i<0 or i>=len(cases) for i in args.case_index):p.error(f'--case-index must be between 0 and {len(cases)-1}')
    results=[]
    for index,c in enumerate(cases):
        if args.case_index is not None and index not in args.case_index:continue
        expected=original(*images[0],c);fixture=out/'case.bin';fixture.write_bytes(words([c[f] for f in FIELDS]));checks=[(compiler,subprocess.check_output([str(out/f'{compiler}.exe'),str(fixture)])) for compiler in commands]
        if len(images)>1:checks.append(('matching PE',original(*images[1],c)))
        for compiler,actual in checks:
            if actual!=expected:
                prefix=out/f'failure-{compiler.replace(" ","-")}-{index}';prefix.with_suffix('.expected.bin').write_bytes(expected);prefix.with_suffix('.actual.bin').write_bytes(actual);diff=next((i for i,(a,b) in enumerate(zip(actual,expected)) if a!=b),None);prefix.with_suffix('.json').write_text(json.dumps(dict(case=c,compiler=compiler,first_difference=diff,expected_size=len(expected),actual_size=len(actual)),indent=2));raise AssertionError(f'Graphics {compiler} scenario {index} differs at {diff}; see {out}')
            results.append(dict(compiler=compiler,case_index=index,case=c,state_and_calls_sha256=hashlib.sha256(actual).hexdigest(),all_bytes_match=True))
        if (index+1)%100==0:print(f'Verified {index+1}/{len(cases)} graphics scenarios',flush=True)
    (out/'results.json').write_text(json.dumps(results,indent=2)+'\n')
    manifest=dict(original_sha256=HASH,comparisons=len(results),scenario_count=len(cases),complete_suite=args.case_index is None,all_cases_passed=True,functions=FUNCTIONS,compiler_commands=commands,harness_sha256=hashlib.sha256(driver.encode()).hexdigest(),source_sha256={str(path.relative_to(ROOT)):hashlib.sha256(path.read_bytes()).hexdigest() for path in (SOURCE,UTILITY,ROOT/'src/carma2/newcommon/18-graphics2.h',ROOT/'src/carma2/newcommon/41-utility.h',ROOT/'src/carma2/include/rec2_types.h')},limitations='Actual DRDrawLine/PaletteEntry16Bit execute. GPU/failure/light callbacks are controlled; complete state and call order plus count/array/palette mutations are captured. Comparisons of angle NaNs expose only unordered branch decisions, not float stores or payload arithmetic.')
    if len(images)>1:manifest.update(recompiled_sha256=hashlib.sha256(images[1][0]).hexdigest(),symbols_sha256=hashlib.sha256(args.symbols.read_bytes()).hexdigest())
    (out/'verification.json').write_text(json.dumps(manifest,indent=2)+'\n');print(f'Passed {len(results)} original/native graphics comparisons')

if __name__=='__main__':main()
