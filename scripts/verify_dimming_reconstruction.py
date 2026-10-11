#!/usr/bin/env python3
"""Compare three actual rectangle dimming bodies against original x86.

Uses repository 32-bit actor/model/vertex/pixelmap layouts and full byte arenas.
BRender update/add/render/remove are ordered controlled boundaries; callbacks
can mutate global pointers and parent/prev fields independently. Pixel shading
executes real table lookup loops including original unchecked corner behavior.
Integer coordinate conversions are finite; no scalar NaN transfer is involved.
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
HASH='9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
FUNCTIONS=dict(DeviouslyDimRectangle=0x47cad0,DimRectangle=0x47cbd0,DimRectangleClipped=0x47cce0)
GLOBALS=dict(gDim_model=0x74ca70,gDim_actor=0x74cf24,g2d_camera=0x74cac4,gBack_screen=0x74d360,gDepth_buffer=0x68b8a4,gDevious_2d=0x74cf60,gDepth_shade_table=0x79ec20,gDim_amount=0x79ec14)
BOUNDARIES=dict(BrModelUpdate=0x51f950,BrActorAdd=0x51dfe0,BrZbsSceneRender=0x522f30,BrActorRemove=0x51e070)
ADDRESSES=dict(FUNCTIONS,**GLOBALS,**BOUNDARIES)
FIELDS='mode left top right bottom corner devious row width height dim prev parent mutate repeat seed shade_row'.split()
HEAP,STACK,STOP,SIZE=0x20000000,0x1080000,0x1081000,0x10000
def words(values):return struct.pack('<'+'I'*len(values),*[v&0xffffffff for v in values])

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE')
    p.add_argument('--dependency-dir',type=Path,default=ROOT/'build-audit/joystick/python-deps')
    p.add_argument('--gcc',type=Path,default=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe')
    p.add_argument('--msvc-root',type=Path,default=ROOT/'build-tools/msvc-modern')
    p.add_argument('--msvc5-root',type=Path,default=ROOT/'build-tools/msvc500sp3')
    p.add_argument('--output',type=Path,default=ROOT/'build-audit/dimming-tests')
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
        if m['format']!=1 or m['original_sha256']!=HASH or m['recompiled_sha256']!=hashlib.sha256(b).hexdigest():raise SystemExit('Stale or incompatible dimming symbol map')
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
#include "33-depth.h"
#include "globvars.h"
#include "globvrbm.h"
typedef char layouts[sizeof(void*)==4&&sizeof(br_pixelmap)==68&&sizeof(br_actor)==100&&sizeof(br_vertex)==40&&sizeof(br_model)==92?1:-1];
enum {MODE,LEFT,TOP,RIGHT,BOTTOM,CORNER,DEVIOUS,ROW,WIDTH,HEIGHT,DIM,PREV,PARENT,MUTATE,REPEAT,SEED,SHADE_ROW};
static unsigned a[17];static unsigned char* heap;
br_model* gDim_model;br_actor *gDim_actor,*g2d_camera;br_pixelmap *gBack_screen,*gDepth_buffer,*gDepth_shade_table;
int gDevious_2d,gDim_amount;
#define OBJ(type,off) ((type*)(heap+(off)))
static void C2_HOOK_CDECL event(unsigned code,unsigned n,const unsigned* values) {
 unsigned v[9],g[8],i;memset(v,0,sizeof(v));v[0]=code;for(i=0;i<n;i++)v[i+1]=values[i];fwrite(v,sizeof(v),1,stdout);
 g[0]=(unsigned)gDim_model;g[1]=(unsigned)gDim_actor;g[2]=(unsigned)g2d_camera;g[3]=(unsigned)gBack_screen;g[4]=(unsigned)gDepth_buffer;g[5]=(unsigned)gDevious_2d;g[6]=(unsigned)gDepth_shade_table;g[7]=(unsigned)gDim_amount;
 fwrite(g,sizeof(g),1,stdout);fwrite(heap,0x10000,1,stdout);
}
void C2_HOOK_CDECL BrModelUpdate(br_model* model,br_uint_16 flags) {unsigned v[2];v[0]=(unsigned)model;v[1]=flags;event(1,2,v);if(a[MUTATE]&1){gDim_actor=OBJ(br_actor,0x900);gDim_model=OBJ(br_model,0x700);}}
br_actor* C2_HOOK_CDECL BrActorAdd(br_actor* parent,br_actor* actor) {unsigned v[2];v[0]=(unsigned)parent;v[1]=(unsigned)actor;event(2,2,v);actor->parent=parent;actor->prev=&parent->children;actor->next=NULL;parent->children=actor;if(a[MUTATE]&2)g2d_camera=OBJ(br_actor,0xb00);return actor;}
void C2_HOOK_CDECL BrZbsSceneRender(br_actor* world,br_actor* camera,br_pixelmap* colour,br_pixelmap* depth) {unsigned v[4];v[0]=(unsigned)world;v[1]=(unsigned)camera;v[2]=(unsigned)colour;v[3]=(unsigned)depth;event(3,4,v);if(a[MUTATE]&4){gDim_actor=OBJ(br_actor,0x900);gBack_screen=OBJ(br_pixelmap,0x200);gDepth_buffer=OBJ(br_pixelmap,0x100);}if(a[MUTATE]&8){gDim_actor->parent=NULL;gDim_actor->prev=NULL;}if(a[MUTATE]&16)gDim_actor->parent=OBJ(br_actor,0xa00);}
br_actor* C2_HOOK_CDECL BrActorRemove(br_actor* actor) {unsigned v=(unsigned)actor;event(4,1,&v);actor->parent=NULL;actor->prev=NULL;OBJ(br_actor,0xa00)->children=NULL;OBJ(br_actor,0xb00)->children=NULL;if(a[MUTATE]&32)gDim_actor=OBJ(br_actor,0x900);return actor;}
'''
    driver+='\n'.join(extract(SOURCE.read_text(),name) for name in FUNCTIONS)
    driver+=r'''
int C2_HOOK_CDECL main(int argc,char** argv) {
 FILE* f;unsigned i,j; br_pixelmap* map;br_actor* actor;
 if(argc!=2)return 90;
 if(!strcmp(argv[1],"schema")){printf("[68,100,40,92]\n");return 0;}
 f=fopen(argv[1],"rb");if(!f)return 91;if(fread(a,sizeof(a),1,f)!=1)return 92;fclose(f);
 heap=(unsigned char*)VirtualAlloc((void*)0x20000000,0x10000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(heap!=(void*)0x20000000)return 93;
 _setmode(_fileno(stdout),_O_BINARY);_controlfp(_PC_53,_MCW_PC);for(i=0;i<0x10000;i++)heap[i]=(unsigned char)(i*37+a[SEED]);
 for(i=0x6000;i<0xa000;i++)heap[i]=(unsigned char)(i*37+(i>>8)*13+a[SEED]);
 for(i=0;i<3;i++){map=OBJ(br_pixelmap,0x100+0x100*i);map->pixels=heap+0x4800+0x100*i;map->row_bytes=(br_int_16)a[ROW];map->width=(br_uint_16)a[WIDTH];map->height=(br_uint_16)a[HEIGHT];}
 OBJ(br_pixelmap,0x300)->pixels=heap+0x8000;OBJ(br_pixelmap,0x300)->row_bytes=(br_int_16)a[SHADE_ROW];
 for(i=0;i<2;i++){OBJ(br_model,0x600+0x100*i)->vertices=OBJ(br_vertex,0x1000+0x100*i);OBJ(br_model,0x600+0x100*i)->nvertices=4;actor=OBJ(br_actor,0x800+0x100*i);actor->prev=a[PREV]?(br_actor**)(heap+0xa08):NULL;actor->parent=a[PARENT]?OBJ(br_actor,0xa00):NULL;actor->next=NULL;actor->children=NULL;}
 OBJ(br_actor,0xa00)->children=NULL;OBJ(br_actor,0xb00)->children=NULL;
 gDim_model=OBJ(br_model,0x600);gDim_actor=OBJ(br_actor,0x800);g2d_camera=OBJ(br_actor,0xa00);gBack_screen=OBJ(br_pixelmap,0x100);gDepth_buffer=OBJ(br_pixelmap,0x200);gDepth_shade_table=OBJ(br_pixelmap,0x300);gDevious_2d=(int)a[DEVIOUS];gDim_amount=(int)a[DIM];
 for(j=0;j<a[REPEAT];j++)switch(a[MODE]){
 case 0:DeviouslyDimRectangle(OBJ(br_pixelmap,0x100),(int)a[LEFT],(int)a[TOP],(int)a[RIGHT],(int)a[BOTTOM],(int)a[CORNER]);break;
 case 1:DimRectangle(OBJ(br_pixelmap,0x100),(int)a[LEFT],(int)a[TOP],(int)a[RIGHT],(int)a[BOTTOM],(int)a[CORNER]);break;
 case 2:DimRectangleClipped(OBJ(br_pixelmap,0x100),(int)a[LEFT],(int)a[TOP],(int)a[RIGHT],(int)a[BOTTOM],(int)a[CORNER]);break;
 default:return 94;}
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
        assert json.loads(subprocess.check_output([str(out/f'{compiler}.exe'),'schema'],text=True))==[68,100,40,92]
    def original(image,addresses,c):
        u=Uc(UC_ARCH_X86,UC_MODE_32);pe=struct.unpack_from('<I',image,60)[0];base=struct.unpack_from('<I',image,pe+52)[0];size=struct.unpack_from('<I',image,pe+80)[0];u.mem_map(base,(size+4095)&~4095);u.mem_map(HEAP,SIZE);u.mem_map(STACK-0x8000,0x10000)
        table=pe+24+struct.unpack_from('<H',image,pe+20)[0]
        for i in range(struct.unpack_from('<H',image,pe+6)[0]):
            _,rva,size,offset=struct.unpack_from('<IIII',image,table+40*i+8);u.mem_write(base+rva,image[offset:offset+size])
        def wr(a,v):u.mem_write(a,words([v]))
        def rd(a):return struct.unpack('<I',u.mem_read(a,4))[0]
        u.mem_write(HEAP,bytes((i*37+c['seed'])&255 for i in range(SIZE)))
        u.mem_write(HEAP+0x6000,bytes((i*37+(i>>8)*13+c['seed'])&255 for i in range(0x6000,0xa000)))
        for i in range(3):
            ptr=HEAP+0x100+0x100*i;wr(ptr+8,HEAP+0x4800+0x100*i);u.mem_write(ptr+40,struct.pack('<H',c['row']&65535));u.mem_write(ptr+52,struct.pack('<HH',c['width']&65535,c['height']&65535))
        wr(HEAP+0x300+8,HEAP+0x8000);u.mem_write(HEAP+0x300+40,struct.pack('<H',c['shade_row']&65535))
        for i in range(2):
            wr(HEAP+0x600+0x100*i+8,HEAP+0x1000+0x100*i);u.mem_write(HEAP+0x600+0x100*i+16,b'\x04\0');actor=HEAP+0x800+0x100*i
            for offset,value in ((4,HEAP+0xa08 if c['prev'] else 0),(12,HEAP+0xa00 if c['parent'] else 0),(0,0),(8,0)):wr(actor+offset,value)
        wr(HEAP+0xa08,0);wr(HEAP+0xb08,0)
        for name,value in zip(GLOBALS,(HEAP+0x600,HEAP+0x800,HEAP+0xa00,HEAP+0x100,HEAP+0x200,c['devious'],HEAP+0x300,c['dim'])):wr(addresses[name],value)
        output=bytearray()
        def event(code,values):output.extend(words([code,*values,*([0]*(8-len(values)))]));output.extend(words([rd(addresses[n]) for n in GLOBALS]));output.extend(u.mem_read(HEAP,SIZE))
        reverse={addresses[n]:i+1 for i,n in enumerate(BOUNDARIES)}
        def hook(uc,address,size,context):
            code=reverse[address];sp=uc.reg_read(UC_X86_REG_ESP);result=0
            if code==1:
                event(code,[rd(sp+4),rd(sp+8)&65535])
                if c['mutate']&1:wr(addresses['gDim_actor'],HEAP+0x900);wr(addresses['gDim_model'],HEAP+0x700)
            elif code==2:
                parent,actor=rd(sp+4),rd(sp+8);event(code,[parent,actor]);wr(actor+12,parent);wr(actor+4,parent+8);wr(actor,0);wr(parent+8,actor);result=actor
                if c['mutate']&2:wr(addresses['g2d_camera'],HEAP+0xb00)
            elif code==3:
                event(code,[rd(sp+4+4*i) for i in range(4)])
                if c['mutate']&4:wr(addresses['gDim_actor'],HEAP+0x900);wr(addresses['gBack_screen'],HEAP+0x200);wr(addresses['gDepth_buffer'],HEAP+0x100)
                actor=rd(addresses['gDim_actor'])
                if c['mutate']&8:wr(actor+12,0);wr(actor+4,0)
                if c['mutate']&16:wr(actor+12,HEAP+0xa00)
            else:
                actor=rd(sp+4);event(code,[actor]);wr(actor+12,0);wr(actor+4,0);wr(HEAP+0xa08,0);wr(HEAP+0xb08,0);result=actor
                if c['mutate']&32:wr(addresses['gDim_actor'],HEAP+0x900)
            uc.reg_write(UC_X86_REG_EAX,result);uc.reg_write(UC_X86_REG_EIP,rd(sp));uc.reg_write(UC_X86_REG_ESP,sp+4)
        for a in reverse:u.hook_add(UC_HOOK_CODE,hook,begin=a,end=a)
        for _ in range(c['repeat']):
            vals=[c[f] for f in ('top','right','bottom','corner')];ecx=HEAP+0x100;edx=c['left']
            if c['mode']==1:vals=[ecx,edx,*vals]
            u.mem_write(STACK,words([STOP,*vals]));u.reg_write(UC_X86_REG_ESP,STACK);u.reg_write(UC_X86_REG_ECX,ecx);u.reg_write(UC_X86_REG_EDX,edx&0xffffffff);u.reg_write(UC_X86_REG_FPCW,0x27f);u.emu_start(addresses[list(FUNCTIONS)[c['mode']]],STOP,count=1000000)
            assert u.reg_read(UC_X86_REG_EIP)==STOP,'Dimming instruction budget exhausted'
            assert u.reg_read(UC_X86_REG_ESP)==STACK+4+(0 if c['mode']==1 else 16),'Dimming ABI stack cleanup differs'
        event(0,[]);return bytes(output)
    def case(mode,**kw):
        c={f:0 for f in FIELDS};c.update(mode=mode,left=1,top=1,right=7,bottom=5,corner=1,devious=1,row=16,width=8,height=6,dim=2,repeat=1,seed=165,shade_row=256);c.update(kw);return c
    cases=[case(mode,devious=flag,prev=prev,parent=parent,mutate=mutate,repeat=2) for mode in range(3) for flag in (1,-1) for prev in (0,1) for parent in (0,1) for mutate in (0,1,2,4,8,16,32,3,5,12,48)]
    cases += [case(mode,devious=0,left=l,top=t,right=r,bottom=b,corner=corner,row=row,dim=dim,shade_row=shade,repeat=2) for mode in (1,2) for l,t,r,b in ((1,1,7,5),(0,0,8,6),(-2,-1,9,7),(2,2,2,2),(3,2,4,3),(3,2,5,4),(5,4,3,2)) for corner in (0,1,-1) for row in (8,16,-16) for dim,shade in ((0,256),(2,256),(-2,256),(2,-256))]
    cases += [case(mode,devious=0,left=1,top=2,right=7,bottom=bottom,corner=corner,row=row,repeat=2) for mode in (1,2) for bottom in (1,2) for corner in (0,1,-1) for row in (8,-16)]
    cases += [case(mode,left=l,top=t,right=r,bottom=b,width=65535,height=65534,corner=0) for mode in (0,1,2) for l,t,r,b in ((0,0,0,0),(16777217,-16777219,16777221,16777223),(-2147483648,2147483647,2147483647,-2147483648),(-17,-19,65535,65534),(65534,65533,65535,65534))]
    if args.case_index is not None and any(i<0 or i>=len(cases) for i in args.case_index):p.error(f'--case-index must be between 0 and {len(cases)-1}')
    results=[]
    for index,c in enumerate(cases):
        if args.case_index is not None and index not in args.case_index:continue
        expected=original(*images[0],c);fixture=out/'case.bin';fixture.write_bytes(words([c[f] for f in FIELDS]));checks=[(compiler,subprocess.check_output([str(out/f'{compiler}.exe'),str(fixture)])) for compiler in commands]
        if len(images)>1:checks.append(('matching PE',original(*images[1],c)))
        for compiler,actual in checks:
            if actual!=expected:
                prefix=out/f'failure-{compiler.replace(" ","-")}-{index}';prefix.with_suffix('.expected.bin').write_bytes(expected);prefix.with_suffix('.actual.bin').write_bytes(actual);diff=next((i for i,(a,b) in enumerate(zip(actual,expected)) if a!=b),None);prefix.with_suffix('.json').write_text(json.dumps(dict(case=c,compiler=compiler,first_difference=diff,expected_size=len(expected),actual_size=len(actual)),indent=2));raise AssertionError(f'Dimming {compiler} scenario {index} differs at {diff}; see {out}')
            results.append(dict(compiler=compiler,case_index=index,case=c,state_and_calls_sha256=hashlib.sha256(actual).hexdigest(),all_bytes_match=True))
        if (index+1)%100==0:print(f'Verified {index+1}/{len(cases)} dimming scenarios',flush=True)
    (out/'results.json').write_text(json.dumps(results,indent=2)+'\n')
    manifest=dict(original_sha256=HASH,comparisons=len(results),scenario_count=len(cases),complete_suite=args.case_index is None,all_cases_passed=True,functions=FUNCTIONS,compiler_commands=commands,harness_sha256=hashlib.sha256(driver.encode()).hexdigest(),source_sha256={str(path.relative_to(ROOT)):hashlib.sha256(path.read_bytes()).hexdigest() for path in (SOURCE,ROOT/'src/carma2/newcommon/18-graphics2.h',ROOT/'src/carma2/include/rec2_types.h')},limitations='BRender callbacks are controlled with full arena/global snapshots and pointer/parent mutations. Shading uses full lookup tables and guarded valid arenas; original unchecked rectangle behavior is preserved. Inputs to scalar conversion are integers, not NaNs.')
    if len(images)>1:manifest.update(recompiled_sha256=hashlib.sha256(images[1][0]).hexdigest(),symbols_sha256=hashlib.sha256(args.symbols.read_bytes()).hexdigest())
    (out/'verification.json').write_text(json.dumps(manifest,indent=2)+'\n');print(f'Passed {len(results)} original/native dimming comparisons')

if __name__=='__main__':main()
