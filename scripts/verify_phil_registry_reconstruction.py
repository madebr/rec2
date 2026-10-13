#!/usr/bin/env python3
"""Physical original/native PHIL registry, activation and water-volume proof.

Actual PHIL/volume/Pling bodies execute at PC53. FindFace is the single explicit
controlled geometry boundary in this registry suite; no PHIL call is mocked.
The complete shared queue/header block, live globals, object arena, raw return
and callback-visible state are compared. Floating exception status is excluded.
"""
import argparse, ast, json, os, struct, subprocess
from pathlib import Path
from verify_tinted_native_reconstruction import extract
from verify_opponent_route_reconstruction import ROOT, HASH, pe, raw, sha, words

SOURCE = ROOT/'src/carma2/newcommon/60-phil.c'
VOLUME = SOURCE.parent/'55-volume.c'
ENTRIES = [('PHILInit',0x4b5cc0,86),('PHILDisable',0x4b5d20,11),
 ('PHILRemoveObject',0x4b5ea0,289),('PHILGetFirstObject',0x4b5fd0,20),
 ('PHILGetNextObject',0x4b5ff0,19),('PHILReturnObjectStatus',0x4b6010,40),
 ('PHILMakeObjectPassive',0x4b6040,72),('PHILSetPassiveObjectsMatrix',0x4b61a0,108),
 ('PHILGetObjectProperty',0x4b6530,220),('PHILMakeObjectActive',0x4b6090,259),
 ('ResetCarSpecialVolume',0x4ff530,152),('GetDefaultSpecialVolumeForWater',0x4ff110,6),
 ('EnablePlingMaterials',0x45d5c0,11),('DisablePlingMaterials',0x45d5d0,11)]
GLOBALS = {'gPHIL_registry':(0x6923e0,2532),'gPHIL_disabled':(0x65d004,4),
 'gPHIL_list_collision_infos':(0x74a5f0,4),'gPHIL_munging_objects':(0x74a5e8,4),
 'gPHIL_doing_physics':(0x74a5f8,4),'gPHIL_last_physics_tick':(0x74a5ec,4),
 'gPHIL_mechanics_time_sync':(0x74a5e4,4),'gDefault_water_spec_vol_real':(0x761f44,4),
 'gPling_materials':(0x5964c0,4)}
FIELDS = 'mode disabled type info status flags munging added count removal object next prev matrix velocity omega property scalar flag distance material identifier mutation sequence seed'.split()
RECORD_BYTES = 32 + 24 + 2532 + 8*4 + 0x10000

COMMON = r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "60-phil.h"
#include "55-volume.h"
#include "49-physics.h"
#include "00-car.h"
#include "34-finteray.h"
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison;
extern double builder_wide;
extern unsigned __cdecl BuilderInvoke(void);
static unsigned addresses[12],h[25],case_index,event_count;static unsigned char*heap;static unsigned ray_data[6];
#define OBJ(type,off) ((type*)(heap+0x200000+(off)))
#define OBJECT OBJ(tPhysics_object,0x100)
#define REG (*(tPHIL_registry_storage*)(heap+0x1023e0))
#undef gPHIL_registry
#define gPHIL_registry REG
#define gPHIL_disabled (*(int*)(heap+0xcd004))
#define gPHIL_list_collision_infos (*(tPhysics_object**)(heap+0x1ba5f0))
#define gPHIL_munging_objects (*(int*)(heap+0x1ba5e8))
#define gPHIL_doing_physics (*(int*)(heap+0x1ba5f8))
#define gPHIL_last_physics_tick (*(tU32*)(heap+0x1ba5ec))
#define gPHIL_mechanics_time_sync (*(int*)(heap+0x1ba5e4))
#define gDefault_water_spec_vol_real (*(tSpecial_volume**)(heap+0x1d1f44))
#define gPling_materials (*(int*)(heap+0x64c0))
enum {MODE,DISABLED,TYPE,INFO,STATUS,FLAGS,MUNGING,ADDED,COUNT,REMOVAL,OBJECT_INDEX,NEXT,PREV,MATRIX,VELOCITY,OMEGA,PROPERTY,SCALAR,FLAG,DISTANCE,MATERIAL,IDENTIFIER,MUTATION,SEQUENCE,SEED};
typedef char layouts[sizeof(void*)==4 && sizeof(tPHIL_registry_storage)==2532 && offsetof(tPHIL_registry_storage,queued)==16 && offsetof(tPHIL_registry_storage,count_removals)==2524 && offsetof(tPHIL_queued_objects,headers)==104 && sizeof(tPHIL_queued_object_info)==100 && sizeof(tPHIL_queued_header)==24 && offsetof(tPhysics_object,next)==0x220 && offsetof(tPhysics_object,field_0x240)==0x240 && offsetof(tPhysics_object,auto_special_volume)==0x25c ? 1:-1];
static void event(unsigned code,unsigned value){unsigned head[8];head[0]=case_index;head[1]=code;head[2]=value;head[3]=event_count++;memcpy(head+4,&builder_wide,8);head[6]=head[7]=0;fwrite(head,sizeof(head),1,stdout);fwrite(ray_data,sizeof(ray_data),1,stdout);fwrite(&REG,sizeof(REG),1,stdout);fwrite(&gPHIL_disabled,4,1,stdout);fwrite(&gPHIL_list_collision_infos,4,1,stdout);fwrite(&gPHIL_munging_objects,4,1,stdout);fwrite(&gPHIL_doing_physics,4,1,stdout);fwrite(&gPHIL_last_physics_tick,4,1,stdout);fwrite(&gPHIL_mechanics_time_sync,4,1,stdout);fwrite(&gDefault_water_spec_vol_real,4,1,stdout);fwrite(&gPling_materials,4,1,stdout);fwrite(heap+0x200000,0x10000,1,stdout);}
static void __fastcall RayBoundary(br_vector3*position,br_vector3*direction,br_vector3*normal,br_scalar*t,br_material**material){memcpy(ray_data,position,12);memcpy(ray_data+3,direction,12);event(1,0);if(h[MUTATION]&1)gDefault_water_spec_vol_real=OBJ(tSpecial_volume,0xb000);if(h[MUTATION]&2)gPHIL_disabled=1;if(h[MUTATION]&4)gPHIL_list_collision_infos=OBJ(tPhysics_object,0x1100);if(h[MUTATION]&8)OBJECT->next=OBJ(tPhysics_object,0x1100);normal->v[0]=1.f;normal->v[1]=2.f;normal->v[2]=3.f;memcpy(t,&h[DISTANCE],4);*material=h[MATERIAL]?OBJ(br_material,0x7000):NULL;}
void C2_HOOK_FASTCALL FindFace(br_vector3*position,br_vector3*direction,br_vector3*normal,br_scalar*t,br_material**material){RayBoundary(position,direction,normal,t,material);}
static unsigned invoke(unsigned mode,unsigned x,unsigned y,unsigned n,unsigned a,unsigned b){builder_mode=mode;builder_address=addresses[mode];builder_x=x;builder_y=y;builder_stack_count=n;builder_stack[0]=a;builder_stack[1]=b;builder_wide=0.0;if(BuilderInvoke())exit(14);event(0,builder_result);return builder_result;}
static void setup(void){unsigned i; tPhysics_object*object;tPHIL_queued_header*header;memset(&REG,0xa5,sizeof(REG));for(i=0;i<100;i++)REG.queued.headers[i].collision_info=NULL;gPHIL_disabled=(int)h[DISABLED];gPHIL_list_collision_infos=OBJECT;gPHIL_munging_objects=(int)h[MUNGING];gPHIL_doing_physics=0;gPHIL_last_physics_tick=0x12345678;gPHIL_mechanics_time_sync=0x87654321;gDefault_water_spec_vol_real=OBJ(tSpecial_volume,0xa000);gPling_materials=0x1234;REG.count_list=(int)h[COUNT];REG.count_removals=(int)h[REMOVAL];REG.count_adds=0x11223344;REG.object_added=(int)h[ADDED];
for(i=0;i<3;i++){object=OBJ(tPhysics_object,0x100+i*0x800);header=&REG.queued.headers[i];header->collision_info=object;header->field_0x8=h[STATUS];header->field_0x4=(tU16)h[FLAGS];memcpy(&header->field_0xc,&h[SCALAR],4);memcpy(&header->field_0x10,&h[SCALAR],4);memcpy(&header->field_0x14,&h[SCALAR],4);object->actor=OBJ(br_actor,0x5000+i*0x100);object->field_0x239=(tU8)h[TYPE];object->field_0x240=header;object->flags=(int)h[FLAGS];object->next=i<2?OBJ(tPhysics_object,0x100+(i+1)*0x800):NULL;object->prev=i?OBJ(tPhysics_object,0x100+(i-1)*0x800):NULL;}
object=OBJ(tPhysics_object,0x100+h[OBJECT_INDEX]*0x800);if(h[INFO]==0)object->field_0x240=NULL;else if(h[INFO]==2){object->field_0x240=&REG.queued.objects[0];REG.queued.objects[0].collision_info=object;REG.queued.objects[0].field_0x8=h[STATUS];REG.queued.objects[0].field_0x4=(tU16)h[FLAGS];}else if(h[INFO]==3)object->field_0x240=OBJ(tPHIL_queued_header,0x6800);else if(h[INFO]==4)object->field_0x240=(void*)&REG.count_list;
if(h[NEXT]==1)object->next=NULL;if(h[NEXT]==2)gPHIL_list_collision_infos=NULL;if(h[NEXT]==3)object->next=object;if(h[PREV]==1)REG.queued.headers[h[OBJECT_INDEX]].collision_info=NULL;if(h[PREV]==2)REG.queued.headers[99].collision_info=object;
OBJ(br_material,0x7000)->identifier=h[IDENTIFIER]==0?NULL:OBJ(char,0x7100);if(h[IDENTIFIER]==1)strcpy(OBJ(char,0x7100),"");if(h[IDENTIFIER]==2)strcpy(OBJ(char,0x7100),"!water");if(h[IDENTIFIER]==3)strcpy(OBJ(char,0x7100),"#water");if(h[IDENTIFIER]==4)strcpy(OBJ(char,0x7100),"water");if(h[IDENTIFIER]==5)strcpy(OBJ(char,0x7100),"!abcdefghijklmnopqrstuvwxyz");event_count=0;memset(ray_data,0,sizeof(ray_data));builder_poison=0xcccccccc;builder_wide=0.0;}
static int run(const char*path){FILE*f;unsigned mode,object,matrix,velocity,omega;SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);f=fopen(path,"rb");if(!f)return 10;heap=(unsigned char*)VirtualAlloc((void*)0x20000000,0x300000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(heap!=(void*)0x20000000)return 11;_setmode(_fileno(stdout),_O_BINARY);for(case_index=0;fread(h,sizeof(h),1,f)==1;case_index++){if(fread(heap+0x200000,0x10000,1,f)!=1)return 12;setup();_controlfp(_PC_53,_MCW_PC);_clearfp();object=(unsigned)OBJ(tPhysics_object,0x100+h[OBJECT_INDEX]*0x800);matrix=(unsigned)OBJ(br_matrix34,0x6000);velocity=h[VELOCITY]?(unsigned)OBJ(br_vector3,0x6200):0;omega=h[OMEGA]?(unsigned)OBJ(br_vector3,0x6300):0;if(h[MATRIX]==1)matrix=(unsigned)&((tPhysics_object*)object)->actor->t.t.mat;if(h[MATRIX]==2)matrix=(unsigned)&((tPhysics_object*)object)->actor->t.t.mat-4;if(h[MATRIX]==3)matrix=(unsigned)&((tPhysics_object*)object)->actor->t.t.mat+4;if(h[MATRIX]==4)matrix=(unsigned)&REG.queued.objects[0].field_0x30-4;if(h[MATRIX]==5)matrix=(unsigned)&REG.queued.objects[0].field_0x30+4;if(h[VELOCITY]==2)velocity=(unsigned)&((tPhysics_object*)object)->v-4;if(h[VELOCITY]==3)velocity=(unsigned)&((tPhysics_object*)object)->v+4;if(h[OMEGA]==2)omega=velocity;if(h[OMEGA]==3)omega=(unsigned)&((tPhysics_object*)object)->omega-4;if(h[OMEGA]==4)omega=(unsigned)&REG.queued.objects[0].field_0x24+4;mode=h[MODE];if(mode==9)invoke(mode,object,velocity,2,omega,h[FLAG]);else if(mode==7)invoke(mode,object,matrix,0,0,0);else if(mode==8)invoke(mode,object,h[PROPERTY],0,0,0);else invoke(mode,object,0,0,0,0);if(h[SEQUENCE]){invoke(3,0,0,0,0,0);invoke(4,object,0,0,0,0);invoke(5,object,0,0,0,0);}}fclose(f);return 0;}
'''

def literals():
    # Reuse the already reviewed ABI checker/PE-loader templates, not semantics.
    path=ROOT/'scripts/verify_opponent_navigation_reconstruction.py'
    tree=ast.parse(path.read_text());found={}
    for node in ast.walk(tree):
        if isinstance(node,ast.Assign):
            for target in node.targets:
                if isinstance(target,ast.Name) and target.id in ('caller','physical'):
                    value=node.value.right if isinstance(node.value,ast.BinOp) else node.value
                    assert isinstance(value,ast.Constant)
                    found[target.id]=value.value
    caller=found['caller'].replace('builder_poison;','builder_poison;double builder_wide;')
    caller=caller.replace(' cmp builder_mode,2\n jne resultready\n xor eax,eax\n resultready:', ' cmp builder_mode,8\n jne notwide\n fstp qword ptr builder_wide\n xor eax,eax\n notwide:\n cmp builder_mode,1\n je zeroreturn\n cmp builder_mode,10\n jne resultready\n zeroreturn:\n xor eax,eax\n resultready:')
    physical=found['physical'].replace('hdr[7]','hdr[14]').replace('n=hdr[6]','n=hdr[13]').replace('i<4;i++)addresses','i<12;i++)addresses')
    a=physical.index('image[hdr[4]]=0xe9;');b=physical.index('free(patch);',a)
    physical=physical[:a]+'image[hdr[12]]=0xe9;relative=(unsigned)RayBoundary-((unsigned)image+hdr[12]+5);memcpy(image+hdr[12]+1,&relative,4);'+physical[b:]
    return caller,physical,path

def compile_sources(out,native,physical,caller):
    m5=ROOT/'build-tools/msvc500sp3';env5=dict(os.environ);env5['PATH']=str(m5/'bin')+os.pathsep+str(m5/'redist')+os.pathsep+env5['PATH'];env5['INCLUDE']=str(m5/'include');env5['LIB']=str(m5/'lib')
    ms=sorted((ROOT/'build-tools/msvc-modern/VC/Tools/MSVC').iterdir())[-1];sdk=ROOT/'build-tools/msvc-modern/Windows Kits/10';v=sorted((sdk/'Include').iterdir())[-1].name;env=dict(os.environ);env['PATH']=str(ms/'bin/Hostx64/x86')+os.pathsep+env['PATH'];env['INCLUDE']=os.pathsep.join(map(str,[ms/'include',*[sdk/f'Include/{v}/{x}' for x in ('ucrt','shared','um')]]));env['LIB']=os.pathsep.join(map(str,[ms/'lib/x86',*[sdk/f'Lib/{v}/{x}/x86' for x in ('ucrt','um')]]))
    inc=[ROOT/x for x in ('src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/brender/include','src/carma2/s3/include','src/carma2/platform')];gcc=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'
    for name,data in [('caller',caller),('native',native),('physical',COMMON+physical)]: (out/f'{name}.c').write_text(data)
    commands={'caller':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Zl','/c',str(out/'caller.c'),'/Fo'+str(out/'caller.obj')],
     'msvc5':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(x) for x in inc],str(out/'native.c'),str(out/'caller.obj'),'/Fo'+str(out/'msvc5.obj'),'/Fe'+str(out/'msvc5.exe')],
     'msvc':[str(ms/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(x) for x in inc],str(out/'native.c'),str(out/'caller.obj'),'/Fo'+str(out/'msvc.obj'),'/Fe'+str(out/'msvc.exe')],
     'mingw':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation',*['-I'+str(x) for x in inc],str(out/'native.c'),str(out/'caller.obj'),'-o',str(out/'mingw.exe')],
     'physical':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-DREC2_MATCHING',*['-I'+str(x) for x in inc],str(out/'physical.c'),str(out/'caller.obj'),'-o',str(out/'physical.exe')]}
    for name,command in commands.items():
        result=subprocess.run(command,cwd=out,env=env5 if name in ('caller','msvc5') else env if name=='msvc' else None,capture_output=True,text=True);(out/f'{name}-build.txt').write_text(result.stdout+result.stderr)
        if result.returncode:raise RuntimeError(result.stdout+result.stderr)
    return commands

def fixtures():
    cases=[]
    def add(**v):
        c=dict(mode=2,disabled=0,type=1,info=1,status=1,flags=0xa5a5,munging=0,added=0,count=3,removal=0,object=0,next=0,prev=0,matrix=0,velocity=1,omega=1,property=0,scalar=0x3f812345,flag=1,distance=0x3f000000,material=1,identifier=2,mutation=0,sequence=0,seed=len(cases)+1);c.update(v);cases.append(c)
    for mode in range(12):
        for disabled in (0,1,0xffffffff,0x80000000):add(mode=mode,disabled=disabled)
    for mode in (2,4,5,6,7,8,9):
        for typ,info in ((0,0),(0,1),(1,0),(1,1),(1,3),(2,2)):
            for status in (0,1,2,0xffffffff):add(mode=mode,type=typ,info=info,status=status)
    for object_ in range(3):
        for nxt in range(4):
            for prev in range(3):add(mode=2,object=object_,next=nxt,prev=prev)
    for index in (0,1,24,99,628,629,0xffffffff):add(mode=2,munging=1,removal=index)
    for mode in (2,5,6):add(mode=mode,info=4)
    for typ,info in ((1,1),(2,2)):
        for matrix in range(6):add(mode=7,type=typ,info=info,matrix=matrix)
        for vel in range(4):
            for omega in range(5):
                for flag in (0,1,0xffffffff):add(mode=9,type=typ,info=info,velocity=vel,omega=omega,flag=flag)
    for prop in (0xffffffff,*range(10),0x80000000,0x7fffffff):
        for bits in (0,0x80000000,1,0x80000001,0x7f7fffff,0x7f800000,0xff800000,0x7fc01234,0xffc01234,0x7f800001,0xff800001,0x7fbfffff):add(mode=8,property=prop,scalar=bits)
    for mode in (9,10):
        for bits in (0x42c7ffff,0x42c80000,0x42c80001,0,0x80000000,0x7f800000,0xff800000,0x7fc01234,0xffc01234,0x7f800001,0xff800001):
            for identifier in range(6):add(mode=mode,distance=bits,identifier=identifier,mutation=1)
        for mutation in (0,2,4,8,15):add(mode=mode,mutation=mutation,sequence=1)
        add(mode=mode,material=0)
    return cases

def make_case(c):
    import random
    rng=random.Random(c['seed']);arena=bytearray(words([rng.getrandbits(32) for _ in range(0x10000//4)]))
    raw_ieee=[0x7f800001,0xff800001,0x7fbfffff,0xffbfffff,0x7fc12345,0xffc12345,0,0x80000000,1,0x80000001,0x7f800000,0xff800000]
    for off,n in ((0x6000,12),(0x6200,3),(0x6300,3),(0x168,6),(0x968,6),(0x1168,6)):
        arena[off:off+n*4]=words(raw_ieee[:n])
    return words([c[x] for x in FIELDS])+arena

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE');p.add_argument('--output',type=Path,default=ROOT/'build-audit/phil-registry-tests');p.add_argument('--symbols',type=Path);p.add_argument('--recompiled',type=Path);p.add_argument('--case-index',type=int,action='append');a=p.parse_args()
    if bool(a.symbols)!=bool(a.recompiled):p.error('--symbols and --recompiled required together')
    b=a.original.read_bytes();assert sha(b)==HASH;out=a.output.resolve();out.mkdir(parents=True,exist_ok=True)
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM
    cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
    def relocation(data,entries,globals_,ray,table):
        nt,base,size,sections=pe(data);refs={};closure=[];allowed={ray,*[e[1] for e in entries]}
        def translated(value):
            for name,(start,n) in globals_.items():
                if start<=value<start+n:return 0x20000000+GLOBALS[name][0]-0x590000+value-start,1
            assert base<=value<base+size,hex(value);return value-base,0
        for name,start,n in entries:
            insns=list(cs.disasm(raw(data,start,n),start));assert sum(i.size for i in insns)==n,(name,n)
            for i in insns:
                if i.mnemonic=='call' or i.mnemonic.startswith('j'):
                    op=i.operands[0]
                    if op.type==CS_OP_IMM:assert op.imm in allowed or (i.mnemonic.startswith('j') and start<=op.imm<start+n),(name,hex(i.address),i.op_str)
                    else:assert name=='PHILGetObjectProperty' and i.mnemonic=='jmp' and op.type==CS_OP_MEM and op.mem.disp==table,(name,i.op_str)
                for op in i.operands:
                    if op.type==CS_OP_MEM and base<=op.mem.disp<base+size:
                        assert i.disp_size==4;refs[i.address+i.disp_offset-base]=(op.mem.disp,*translated(op.mem.disp))
                    if op.type==CS_OP_IMM and base<=op.imm<base+size and not (i.mnemonic=='call' or i.mnemonic.startswith('j')):
                        assert i.imm_size==4;refs[i.address+i.imm_offset-base]=(op.imm,*translated(op.imm))
            closure.append(dict(name=name,address=start,length=n,sha256=sha(raw(data,start,n))))
        if table is not None:
            for i in range(9):
                value=struct.unpack('<I',raw(data,table+4*i,4))[0];assert entries[8][1]<=value<entries[8][1]+entries[8][2];refs[table+4*i-base]=(value,*translated(value))
        return dict(base=base,size=size,refs=refs,closure=closure,entries=[e[1]-base for e in entries[:12]],ray=ray-base)
    def save(meta,path):path.write_bytes(words([*meta['entries'],meta['ray'],len(meta['refs'])])+b''.join(words([k,*v]) for k,v in sorted(meta['refs'].items())))
    original=relocation(b,ENTRIES,GLOBALS,0x41e340,0x4b660c);save(original,out/'original-addresses.bin')
    caller,physical,template=literals();src=SOURCE.read_text();vol=VOLUME.read_text();native=COMMON+'\n'+extract(src,'PhilCopyWords')+'\n'+'\n'.join(extract(src,e[0]) for e in ENTRIES[:10])+'\n'+'\n'.join(extract(vol,n) for n in ('VolumeIdentifierHasLength','VolumeBelowSurfaceLimit','ResetCarSpecialVolume','GetDefaultSpecialVolumeForWater'))+'\n'+'\n'.join(extract((SOURCE.parent/'34-finteray.c').read_text(),n) for n in ('EnablePlingMaterials','DisablePlingMaterials'))
    native+='\nstatic volatile unsigned native_bodies[12];\n'
    for i,e in enumerate(ENTRIES[:12]):
        n=2 if i==9 else 0;args=','.join(['unsigned x','unsigned y']+[f'unsigned a{j}' for j in range(n)]);types=','.join(['unsigned','unsigned']+['unsigned']*n);values=','.join(['x','y']+[f'a{j}' for j in range(n)]);ret='double' if i==8 else 'void' if i in (1,10) else 'unsigned'
        native+=f'{ret if ret!="void" else "unsigned"} __fastcall adapter{i}({args}){{typedef {ret} (C2_HOOK_FASTCALL *body_fn)({types});'+('' if ret=='void' else 'return ')+f'((body_fn)(size_t)native_bodies[{i}])({values});'+('return 0;' if ret=='void' else '')+'}\n'
    native+='\nint C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;'+''.join(f'native_bodies[{i}]=(unsigned){e[0]};addresses[{i}]=(unsigned)adapter{i};' for i,e in enumerate(ENTRIES[:12]))+'return run(argv[1]);}\n'
    commands=compile_sources(out,native,physical,caller);all_cases=fixtures();cases=[all_cases[i] for i in a.case_index] if a.case_index else all_cases;fixture=out/'cases.bin';fixture.write_bytes(b''.join(make_case(c) for c in cases));records=[];failures=[]
    def execute(name,cmd):
        with (out/f'{name}.bin').open('wb') as stream:r=subprocess.run(cmd,stdout=stream,stderr=subprocess.PIPE,timeout=120)
        if r.returncode:raise RuntimeError(f'{name} returned {r.returncode}: {r.stderr!r}')
    def compare(name):
        count=0
        with (out/'original.bin').open('rb') as expected,(out/f'{name}.bin').open('rb') as actual:
            while True:
                e=expected.read(RECORD_BYTES);v=actual.read(RECORD_BYTES)
                if not e and not v:break
                assert len(e)==len(v)==RECORD_BYTES,'record framing'
                if e!=v:
                    first=next(i for i,(x,y) in enumerate(zip(e,v)) if x!=y);ci=struct.unpack_from('<I',e)[0];f=dict(compiler=name,case_index=ci,case=cases[ci],record=count,record_byte=first,original=e[max(0,first-12):first+20].hex(),actual=v[max(0,first-12):first+20].hex());failures.append(f);(out/f'{name}-failure.json').write_text(json.dumps(f,indent=2));break
                count+=1
        records.append(dict(compiler=name,scenarios=len(cases),records=count,match=not any(f['compiler']==name for f in failures),output_sha256=sha((out/f'{name}.bin').read_bytes())))
    original_command=[str(out/'physical.exe'),str(fixture),str(a.original.resolve()),str(out/'original-addresses.bin')]
    execute('original',original_command)
    for name in ('msvc5','msvc','mingw'):execute(name,[str(out/f'{name}.exe'),str(fixture)]);compare(name)
    linked=None;linked_command=None
    if a.symbols:
        import re
        data=a.recompiled.read_bytes();mapping=json.loads(a.symbols.read_text());assert mapping['format']==1 and mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(data)
        symbols={s['orig']:s['recomp'] for s in mapping['symbols']};map_path=a.recompiled.with_suffix('.map');map_bytes=map_path.read_bytes()
        functions={int(address,16):(name,owner.strip()) for name,address,owner in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',map_bytes.decode('latin1'),re.MULTILINE)}
        ray=symbols[0x41e340];initial=[symbols[e[1]] for e in ENTRIES];names={symbols[e[1]]:e[0] for e in ENTRIES};pending=list(initial);seen=set();entries=[];runtime=[];table=None
        helper_owners={'PhilCopyWords':'60-phil.c.obj','VolumeIdentifierHasLength':'55-volume.c.obj','VolumeBelowSurfaceLimit':'55-volume.c.obj'}
        while pending:
            start=pending.pop(0)
            if start in seen:continue
            seen.add(start);end=min(x for x in functions if x>start);name=names.get(start,functions[start][0])
            if name=='PHILGetObjectProperty':
                for ins in cs.disasm(raw(data,start,end-start),start):
                    if ins.mnemonic=='jmp' and ins.operands[0].type==CS_OP_MEM:
                        table=ins.operands[0].mem.disp
                        assert start<table and table+36<=end
                        end=table
                        break
            entries.append((name,start,end-start))
            for i in cs.disasm(raw(data,start,end-start),start):
                if not (i.mnemonic=='call' or i.mnemonic.startswith('j')):continue
                op=i.operands[0]
                if op.type!=CS_OP_IMM:
                    assert name=='PHILGetObjectProperty' and i.mnemonic=='jmp' and op.type==CS_OP_MEM,(name,i.op_str);assert table is None or table==op.mem.disp;table=op.mem.disp;continue
                if start<=op.imm<end or op.imm==ray:continue
                assert op.imm in functions,(name,hex(i.address),i.op_str);symbol,owner=functions[op.imm]
                if op.imm not in initial:
                    undecorated=symbol.lstrip('_').split('@')[0]
                    assert (undecorated in helper_owners and owner==helper_owners[undecorated]) or (symbol in ('_memcpy','_memmove','_memset') and owner.startswith('LIBC:')),(name,symbol,owner)
                    runtime.append(dict(address=op.imm,symbol=symbol,owner=owner))
                pending.append(op.imm)
        indexed={e[1]:e for e in entries};entries=[indexed[x] for x in initial]+[e for e in entries if e[1] not in initial]
        globals_={name:(symbols[old],n) for name,(old,n) in GLOBALS.items()};linked=relocation(data,entries,globals_,ray,table);linked.update(recompiled_sha256=sha(data),symbols_sha256=sha(a.symbols.read_bytes()),companion_map_sha256=sha(map_bytes),real_runtime_dependencies=runtime)
        save(linked,out/'linked-addresses.bin');linked_command=[str(out/'physical.exe'),str(fixture),str(a.recompiled.resolve()),str(out/'linked-addresses.bin')];execute('physical-matching',linked_command);compare('physical-matching')
    faults=[]
    # Deliberate null queued-info accesses are real original failures, not safe
    # defaults. Compare process failure class only; no post-fault state claim.
    for mode in (5,6,7,9):
        c=dict(all_cases[0],mode=mode,type=2,info=0);path=out/f'fault-{mode}.bin';path.write_bytes(make_case(c));fault_results={}
        cmds={'original':[original_command[0],str(path),*original_command[2:]],**{n:[str(out/f'{n}.exe'),str(path)] for n in ('msvc5','msvc','mingw')}}
        if linked_command:cmds['physical-matching']=[linked_command[0],str(path),*linked_command[2:]]
        for name,command in cmds.items():
            r=subprocess.run(command,stdout=subprocess.PIPE,stderr=subprocess.PIPE,timeout=30);fault_results[name]=r.returncode&0xffffffff
        assert set(fault_results.values())=={0xc0000005},(mode,fault_results)
        faults.append(dict(mode=mode,case=c,input_sha256=sha(path.read_bytes()),process_returncodes=fault_results))
    paths=[SOURCE,SOURCE.with_suffix('.h'),VOLUME,VOLUME.with_suffix('.h'),SOURCE.parent/'49-physics.h',SOURCE.parent/'34-finteray.c',SOURCE.parent/'34-finteray.h',ROOT/'src/carma2/include/rec2_types.h',template,Path(__file__).resolve()]
    manifest=dict(scenarios=len(cases),comparisons=len(cases)*len(records),fault_probes=faults,fault_comparisons=len(faults)*len(records),entries=ENTRIES[:12],original=original,linked=linked,original_sha256=sha(b),input_sha256=sha(fixture.read_bytes()),records=records,failures=failures,commands=commands,sources={str(x.relative_to(ROOT)):sha(x.read_bytes()) for x in paths},selected_case_indices=a.case_index,generated_files_sha256={n:sha((out/n).read_bytes()) for n in ('caller.c','caller.obj','native.c','physical.c','physical.exe','msvc5.exe','msvc.exe','mingw.exe')},limits=['FindFace controlled; actual geometry integration separate','float exception flags and non-x86 builds excluded','bounded queue/header aliases only; no arbitrary out-of-allocation overflow claim','fault probes compare access-violation class only, not indeterminate post-fault state'],identity_notes=['0x4b6010 and 0x4b6040 names recovered from behavior/canonical placeholders; original raw entries independently verified'])
    (out/'verification.json').write_text(json.dumps(manifest,indent=2)+'\n')
    if failures:raise SystemExit(json.dumps(failures,indent=2))
    print(f'PASS {len(cases)} scenarios / {manifest["comparisons"]} comparisons')

if __name__=='__main__':main()
