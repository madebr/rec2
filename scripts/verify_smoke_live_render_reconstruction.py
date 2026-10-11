#!/usr/bin/env python3
"""Verify RenderSmoke with the real seven-function replay-recording closure.

Full initial/final state and lossless state deltas at every callback are checked.
Renderer, transform, clock and random APIs are controlled boundaries; this does
not integrate a BRender driver. Recording and particle bodies are never mocked.
A physical original replay probe independently checks raw nonfinite metadata;
its renderer is stubbed after recording, and no Unicorn normalization is used.
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

ROOT=Path(__file__).resolve().parents[1]
ORIGINAL_HASH='9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
SOURCE=ROOT/'src/carma2/newcommon/32-spark.c'
PIPE=ROOT/'src/carma2/newcommon/23-piping.c'
ADDRESSES=dict(RenderSmoke=0x4fa910,CmpSmokeZ=0x4fb2f0,RenderRecordedSmokeCircles=0x4fb1b0,
    gSmoke=0x6a8ce0,gSmoke_flags=0x6aa590,gColumn_flags=0x6aa59c,gSmoke_column=0x6a96a8,
    gBR_smoke_structs=0x6a87f0,gBR_smoke_pointers=0x6a8760,gN_BR_smoke_structs=0x6aa56c,
    gShade_list=0x6b7840,gBlend_actor=0x74caac,gBlend_model=0x74cf30,gBlend_model2=0x74cf94,
    gDont_render_actor=0x761ce4,gNo_2d_effects=0x74cf68,gRendering_mirror=0x74b778,
    gCamera_to_world=0x74d460,gRearview_camera_to_world=0x74d560,gAction_replay_mode=0x676914,
    gProgram_state=0x75bb80,BrActorAdd=0x51dfe0,BrActorRemove=0x51e070,
    DRMatrix34TApplyP=0x515610,BrQsort=0x528410,BrMaterialUpdate=0x520e70,
    BrModelUpdate=0x51f950,BrZbsSceneRenderAdd=0x522ea0,SRandomBetween=0x513620,
    GetTotalTime=0x514c30,_rand=0x576970,_srand=0x576960,FatalError=0x44c230,
    gPipe_buffer_start=0x676908,gLocal_buffer=0x6768b4,gMr_chunky=0x6768a0,
    gPipe_callbacks=0x6768e8,gPipe_buffer_phys_end=0x6768f0,gPipe_buffer_working_end=0x6768c4,
    gPipe_buffer_oldest=0x6768ac,gPipe_record_ptr=0x6768bc,gLocal_buffer_size=0x6768f4,
    gReentrancy_count=0x6768e4,gReentrancy_array=0x6768d0)
HEAP,STACK,STOP,SIZE=0x1000000,0x1080000,0x1081000,0x14000
POINTERS=['gPipe_buffer_start','gLocal_buffer','gMr_chunky','gPipe_callbacks','gPipe_buffer_phys_end',
    'gPipe_buffer_working_end','gPipe_buffer_oldest','gPipe_record_ptr','gBlend_actor','gBlend_model','gBlend_model2','gDont_render_actor']


def main():
    p=argparse.ArgumentParser(description=__doc__)
    for n,d in [('original',ROOT/'build-reference/originals/CARMA2_HW.EXE'),('recompiled',ROOT/'build-matching/rec2.exe'),
            ('output',ROOT/'build-audit/smoke-live-render-tests'),('dependency-dir',ROOT/'build-audit/joystick/python-deps'),
            ('gcc',ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'),('msvc-root',ROOT/'build-tools/msvc-modern'),
            ('msvc5-root',ROOT/'build-tools/msvc500sp3')]:p.add_argument('--'+n,type=Path,default=d)
    p.add_argument('--symbols',type=Path)
    args=p.parse_args();sys.path.insert(0,str(args.dependency_dir.resolve()))
    from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
    from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_EAX,UC_X86_REG_ECX,UC_X86_REG_EDX,UC_X86_REG_FPCW
    from verify_tinted_native_reconstruction import extract
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    from capstone.x86 import X86_OP_MEM,X86_OP_IMM
    original=args.original.read_bytes();assert hashlib.sha256(original).hexdigest()==ORIGINAL_HASH
    images=[(original,ADDRESSES)]
    if args.symbols:
        m=json.loads(args.symbols.read_text());b=args.recompiled.read_bytes()
        assert m['format']==1 and m['original_sha256']==ORIGINAL_HASH and m['recompiled_sha256']==hashlib.sha256(b).hexdigest()
        syms={s['orig']:s for s in m['symbols']};images.append((b,{n:syms[a]['recomp'] for n,a in ADDRESSES.items()}))
    out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)

    def execute(data,a,h,arena,smoke,columns):
        u=Uc(UC_ARCH_X86,UC_MODE_32);u.mem_map(0x400000,0x500000);u.mem_map(HEAP,0x100000)
        pe=struct.unpack_from('<I',data,0x3c)[0];table=pe+24+struct.unpack_from('<H',data,pe+20)[0]
        for i in range(struct.unpack_from('<H',data,pe+6)[0]):
            _,rva,n,raw=struct.unpack_from('<IIII',data,table+40*i+8);u.mem_write(0x400000+rva,data[raw:raw+n])
        def wr(at,v):u.mem_write(at,struct.pack('<I',v&0xffffffff))
        def rd(at):return struct.unpack('<I',u.mem_read(at,4))[0]
        u.mem_write(HEAP,bytes(arena));u.mem_write(a['gSmoke'],bytes(smoke));u.mem_write(a['gSmoke_column'],bytes(columns))
        for n,i in [('gNo_2d_effects',0),('gAction_replay_mode',1),('gSmoke_flags',2),('gColumn_flags',3),('gRendering_mirror',4),('gN_BR_smoke_structs',10)]:wr(a[n],h[i])
        for n,off in [('gBlend_actor',0),('gBlend_model',0x4000),('gBlend_model2',0x4100),('gDont_render_actor',0x200),
                ('gLocal_buffer',0x7000),('gMr_chunky',0x7008),('gPipe_callbacks',0x13000),
                ('gPipe_buffer_phys_end',0x13000),('gPipe_buffer_working_end',0x13000),('gPipe_record_ptr',0xb000)]:wr(a[n],HEAP+off)
        wr(a['gPipe_buffer_start'],HEAP+0xb000 if h[8] else 0);wr(a['gPipe_buffer_oldest'],0)
        wr(a['gLocal_buffer_size'],8);wr(a['gReentrancy_count'],0)
        wr(a['gProgram_state']+0x28,h[9]);wr(a['gProgram_state']+0xbc,HEAP+0x300)
        for i in range(5):wr(a['gReentrancy_array']+4*i,20+i)
        for n in ['gCamera_to_world','gRearview_camera_to_world']:
            u.mem_write(a[n],struct.pack('<12f',1.,0.,0.,0.,1.,0.,0.,0.,1.,0.,0.,0.))
        for i in range(16):wr(a['gShade_list']+4*i,0x102030+i*0x010101)
        for i in range(35):
            at=a['gBR_smoke_structs']+36*i
            u.mem_write(at,struct.pack('<5fIfII',i+.25,-i*.5,1.,.05,.45,0x102030+i,.9,HEAP+0x1000+256*i,HEAP+0x4000))
            wr(a['gBR_smoke_pointers']+4*i,at)
        rng=[h[6]];previous=[b''];calls=[];counts={}
        def norm(v):
            if a['gBR_smoke_structs']<=v<a['gBR_smoke_structs']+1260:return ADDRESSES['gBR_smoke_structs']+v-a['gBR_smoke_structs']
            return v
        def full():
            parts=[struct.pack('<I',rd(a[n])) for n in POINTERS]
            parts.append(struct.pack('<9I',rd(a['gLocal_buffer_size']),rd(a['gReentrancy_count']),rd(a['gSmoke_flags']),rd(a['gColumn_flags']),rd(a['gNo_2d_effects']),rd(a['gN_BR_smoke_structs']),rd(a['gRendering_mirror']),rd(a['gAction_replay_mode']),rng[0]))
            for n,nbytes in [('gReentrancy_array',20),('gShade_list',64),('gCamera_to_world',48),('gRearview_camera_to_world',48),('gSmoke',1100),('gSmoke_column',1240),('gBR_smoke_structs',1260)]:parts.append(bytes(u.mem_read(a[n],nbytes)))
            parts.append(b''.join(struct.pack('<I',norm(rd(a['gBR_smoke_pointers']+4*i))) for i in range(35)))
            parts.append(bytes(u.mem_read(HEAP,SIZE)));return b''.join(parts)
        def delta():
            now=full();before=previous[0];i=0;pieces=[]
            while i<len(now):
                if now[i]==before[i]:i+=1;continue
                first=i
                while i<len(now) and now[i]!=before[i]:i+=1
                pieces.append(f'{first:08x}:'+now[first:i].hex())
            previous[0]=now;return ';'.join(pieces)
        baseline=full().hex();previous[0]=bytes.fromhex(baseline)
        wr(HEAP+0x91000,h[18]);u.mem_write(a['SRandomBetween'],b'\xd9\x05'+struct.pack('<I',HEAP+0x91000)+b'\xc2\x08\x00')
        names={a[n]:n for n in ['BrActorAdd','BrActorRemove','DRMatrix34TApplyP','BrQsort','BrMaterialUpdate','BrModelUpdate','BrZbsSceneRenderAdd','SRandomBetween','GetTotalTime','_rand','_srand','FatalError']}
        def hook(uc,at,size,context):
            if at not in names:return
            name=names[at];sp=uc.reg_read(UC_X86_REG_ESP);argv=[rd(sp+4+4*i) for i in range(4)];result=0;pop=0
            number=counts.get(name,0);counts[name]=number+1
            if name=='_rand':
                calls.append(['r',delta()]);rng[0]=(rng[0]*214013+2531011)&0xffffffff;result=(rng[0]>>16)&0x7fff
            elif name=='_srand':calls.append(['s',argv[0],delta()]);rng[0]=argv[0]
            elif name=='GetTotalTime':calls.append(['t',delta()]);result=123456
            elif name=='SRandomBetween':
                calls.append(['f',argv[0],argv[1],delta()])
                if number==0 and h[11]&1:wr(a['gShade_list']+28,0x112233)
                return
            elif name=='DRMatrix34TApplyP':
                dest=uc.reg_read(UC_X86_REG_ECX);src=uc.reg_read(UC_X86_REG_EDX)
                calls.append(['x',norm(dest),int(argv[0]==a['gRearview_camera_to_world']),bytes(u.mem_read(src,12)).hex(),delta()])
                u.mem_write(dest,struct.pack('<3f',1.,2.,3.) if h[20] else bytes(u.mem_read(src,12)));pop=4
                if number==0 and h[11]&2:
                    wr(a['gN_BR_smoke_structs'],3);wr(a['gRendering_mirror'],1-rd(a['gRendering_mirror']))
            elif name=='BrActorAdd':calls.append(['a',argv[0],argv[1],delta()]);wr(argv[1]+12,argv[0]);result=argv[1]
            elif name=='BrActorRemove':
                calls.append(['d',argv[0],delta()]);wr(argv[0]+12,0);result=argv[0]
                if number==0 and h[11]&4:wr(a['gBlend_actor'],HEAP+0x400)
            elif name=='BrQsort':
                assert argv[0]==a['gBR_smoke_pointers'] and argv[2]==4 and argv[3]==a['CmpSmokeZ']
                calls.append(['q',argv[1],delta()])
            elif name=='BrMaterialUpdate':
                calls.append(['m',argv[0],argv[1],delta()])
                if number==0 and h[11]&8:
                    wr(a['gBlend_actor'],HEAP+0x400);wr(a['gN_BR_smoke_structs'],0);wr(a['gNo_2d_effects'],0)
            elif name=='BrModelUpdate':calls.append(['u',argv[0],argv[1],delta()])
            elif name=='BrZbsSceneRenderAdd':calls.append(['v',argv[0],delta()])
            else:raise AssertionError('Unexpected FatalError')
            uc.reg_write(UC_X86_REG_EAX,result);uc.reg_write(UC_X86_REG_EIP,rd(sp));uc.reg_write(UC_X86_REG_ESP,sp+4+pop)
        u.hook_add(UC_HOOK_CODE,hook);u.reg_write(UC_X86_REG_FPCW,0x27f);wr(STACK,STOP)
        for i,v in enumerate([HEAP+0x100,0,h[5]]):wr(STACK+4+4*i,v)
        u.reg_write(UC_X86_REG_ESP,STACK);u.reg_write(UC_X86_REG_ECX,0);u.reg_write(UC_X86_REG_EDX,0)
        u.emu_start(a['RenderSmoke'],STOP,count=4000000);assert u.reg_read(UC_X86_REG_EIP)==STOP
        return dict(baseline=baseline,calls=calls,state=full().hex())

    source=SOURCE.read_text();pipe=PIPE.read_text()
    names=['CmpSmokeZ','RenderRecordedSmokeCircles','SmokeScalarRound','SmokeScalarBits','RecordSmokeCircle','SmokeCircle3D','DrawTheGlow','ReplaySmoke','RenderSmoke']
    pipe_names=['ReplayCopyForward','LengthOfSession','ARStartPipingSession','EndPipingSession2','AREndPipingSession','ARAddDataToSession','ARAddVariedDataToSession','ReplayScalarRound','AddSmokeToPipingSession']
    bodies=[extract(pipe,n) for n in pipe_names]+[extract(source,n) for n in names]
    driver=r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <float.h>
#include <math.h>
#include "32-spark.h"
#include "23-piping.h"
#include "41-utility.h"
#include "18-graphics2.h"
#include "52-errors.h"
#include "globvars.h"
#include "globvrpb.h"
#include "rec2_macros.h"
typedef char layouts[sizeof(void*)==4&&sizeof(tSmoke)==44&&sizeof(tSmoke_column)==124&&offsetof(tSmoke_column,pos)==52&&offsetof(tSmoke_column,lifetime)==20&&offsetof(tSmoke_column,colour)==28&&sizeof(tBRender_smoke)==36&&sizeof(br_actor)==100&&sizeof(br_material)==156&&sizeof(br_model)==92&&sizeof(br_vertex)==40?1:-1];
tSmoke gSmoke[25];int gSmoke_flags,gColumn_flags,gNo_2d_effects,gN_BR_smoke_structs,gRendering_mirror,gAction_replay_mode;tSmoke_column gSmoke_column[10];tBRender_smoke gBR_smoke_structs[35];tBRender_smoke*gBR_smoke_pointers[35];int gShade_list[16];br_actor*gBlend_actor,*gDont_render_actor;br_model*gBlend_model,*gBlend_model2;br_matrix34 gCamera_to_world,gRearview_camera_to_world;tProgram_state gProgram_state;
tU8*gPipe_buffer_start,*gLocal_buffer,*gMr_chunky,*gPipe_buffer_phys_end,*gPipe_buffer_working_end,*gPipe_buffer_oldest,*gPipe_record_ptr;const tReplay_callback*gPipe_callbacks;tU32 gLocal_buffer_size;int gReentrancy_count;tPipe_chunk_type gReentrancy_array[5];
static unsigned arena[0x14000/4],h[24],rng;static int events,random_calls,transform_calls,remove_calls,material_calls;
static unsigned char previous[90000];
static unsigned norm(unsigned v){unsigned b=(unsigned)(uintptr_t)arena,s=(unsigned)(uintptr_t)gBR_smoke_structs;if(v>=b&&v<=b+sizeof(arena))return v-b+0x1000000;if(v>=s&&v<s+sizeof(gBR_smoke_structs))return v-s+0x6a87f0;return v;}
static unsigned rd(unsigned off){unsigned v;memcpy(&v,(char*)arena+off,4);return v;}
static void wr(unsigned off,unsigned v){memcpy((char*)arena+off,&v,4);}
static void hex(const void*p,unsigned n){unsigned i;for(i=0;i<n;i++)printf("%02x",((const unsigned char*)p)[i]);}
static unsigned full(unsigned char*dest){unsigned prefix[21],i,j,v,off=0;unsigned char copy[sizeof(arena)];tBRender_smoke records[35];
    prefix[0]=norm((unsigned)(uintptr_t)gPipe_buffer_start);prefix[1]=norm((unsigned)(uintptr_t)gLocal_buffer);prefix[2]=norm((unsigned)(uintptr_t)gMr_chunky);prefix[3]=norm((unsigned)(uintptr_t)gPipe_callbacks);prefix[4]=norm((unsigned)(uintptr_t)gPipe_buffer_phys_end);prefix[5]=norm((unsigned)(uintptr_t)gPipe_buffer_working_end);prefix[6]=norm((unsigned)(uintptr_t)gPipe_buffer_oldest);prefix[7]=norm((unsigned)(uintptr_t)gPipe_record_ptr);prefix[8]=norm((unsigned)(uintptr_t)gBlend_actor);prefix[9]=norm((unsigned)(uintptr_t)gBlend_model);prefix[10]=norm((unsigned)(uintptr_t)gBlend_model2);prefix[11]=norm((unsigned)(uintptr_t)gDont_render_actor);
    prefix[12]=gLocal_buffer_size;prefix[13]=gReentrancy_count;prefix[14]=gSmoke_flags;prefix[15]=gColumn_flags;prefix[16]=gNo_2d_effects;prefix[17]=gN_BR_smoke_structs;prefix[18]=gRendering_mirror;prefix[19]=gAction_replay_mode;prefix[20]=rng;
#define APPEND(p,n) do{memcpy(dest+off,p,n);off+=n;}while(0)
    APPEND(prefix,sizeof(prefix));APPEND(gReentrancy_array,20);APPEND(gShade_list,64);APPEND(&gCamera_to_world,48);APPEND(&gRearview_camera_to_world,48);APPEND(gSmoke,1100);APPEND(gSmoke_column,1240);
    memcpy(records,gBR_smoke_structs,sizeof(records));for(i=0;i<35;i++){records[i].material=(br_material*)(uintptr_t)norm((unsigned)(uintptr_t)records[i].material);records[i].model=(br_model*)(uintptr_t)norm((unsigned)(uintptr_t)records[i].model);}APPEND(records,1260);
    for(i=0;i<35;i++){v=norm((unsigned)(uintptr_t)gBR_smoke_pointers[i]);APPEND(&v,4);}
    memcpy(copy,arena,sizeof(copy));for(i=0;i<5;i++)for(j=0;j<2;j++){unsigned k=i*0x100+12+12*j;v=norm(rd(k));memcpy(copy+k,&v,4);if(j==1){v=norm(rd(k+4));memcpy(copy+k+4,&v,4);}}
    for(i=0;i<35;i++){unsigned k=0x1000+256*i+0x58;v=norm(rd(k));memcpy(copy+k,&v,4);}for(i=0;i<2;i++){unsigned k=0x4000+256*i+8;v=norm(rd(k));memcpy(copy+k,&v,4);}APPEND(copy,sizeof(copy));return off;
}
static void delta(void){unsigned char now[90000];unsigned n=full(now),i=0,first;int pieces=0;while(i<n){if(now[i]==previous[i]){i++;continue;}first=i;while(i<n&&now[i]!=previous[i])i++;printf("%s%08x:",pieces++?";":"",first);hex(now+first,i-first);}memcpy(previous,now,n);}
static void event(const char*name){printf("%s[\"%s\"",events++?",":"",name);}
static void end(void){printf(",\"");delta();printf("\"]");}
static int C2_HOOK_CDECL test_rand(void){event("r");end();rng=rng*214013+2531011;return(rng>>16)&0x7fff;}
static void C2_HOOK_CDECL test_srand(unsigned seed){event("s");printf(",%u",seed);end();rng=seed;}
#define rand test_rand
#define srand test_srand
tU32 C2_HOOK_FASTCALL GetTotalTime(void){event("t");end();return 123456;}
br_scalar C2_HOOK_STDCALL SRandomBetween(br_scalar a,br_scalar b){unsigned ab,bb;float result;memcpy(&ab,&a,4);memcpy(&bb,&b,4);event("f");printf(",%u,%u",ab,bb);end();if(random_calls++==0&&(h[11]&1))gShade_list[7]=0x112233;memcpy(&result,&h[18],4);return result;}
void C2_HOOK_FASTCALL DRMatrix34TApplyP(br_vector3*dest,const br_vector3*src,const br_matrix34*mat){event("x");printf(",%u,%d,\"",norm((unsigned)(uintptr_t)dest),mat==&gRearview_camera_to_world);hex(src,12);printf("\"");end();if(h[20]){dest->v[0]=1.f;dest->v[1]=2.f;dest->v[2]=3.f;}else memcpy(dest,src,12);if(transform_calls++==0&&(h[11]&2)){gN_BR_smoke_structs=3;gRendering_mirror=1-gRendering_mirror;}}
br_actor* C2_HOOK_CDECL BrActorAdd(br_actor*parent,br_actor*actor){event("a");printf(",%u,%u",norm((unsigned)(uintptr_t)parent),norm((unsigned)(uintptr_t)actor));end();actor->parent=parent;return actor;}
br_actor* C2_HOOK_CDECL BrActorRemove(br_actor*actor){event("d");printf(",%u",norm((unsigned)(uintptr_t)actor));end();actor->parent=NULL;if(remove_calls++==0&&(h[11]&4))gBlend_actor=(br_actor*)((char*)arena+0x400);return actor;}
void C2_HOOK_CDECL BrQsort(void*base,unsigned count,unsigned size,br_qsort_cbfn cmp){if(base!=gBR_smoke_pointers||size!=4||cmp!=CmpSmokeZ)abort();event("q");printf(",%u",count);end();}
void C2_HOOK_CDECL BrMaterialUpdate(br_material*material,br_uint_16 flags){event("m");printf(",%u,%u",norm((unsigned)(uintptr_t)material),flags);end();if(material_calls++==0&&(h[11]&8)){gBlend_actor=(br_actor*)((char*)arena+0x400);gN_BR_smoke_structs=0;gNo_2d_effects=0;}}
void C2_HOOK_CDECL BrModelUpdate(br_model*model,br_uint_16 flags){event("u");printf(",%u,%u",norm((unsigned)(uintptr_t)model),flags);end();}
void C2_HOOK_CDECL BrZbsSceneRenderAdd(br_actor*actor){event("v");printf(",%u",norm((unsigned)(uintptr_t)actor));end();}
void C2_NORETURN C2_HOOK_CDECL FatalError(int index,...){(void)index;abort();}
'''
    driver+='\n\n'.join(bodies)
    driver+=r'''
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned i,n,v;unsigned char state[90000];if(argc!=2)return 1;f=fopen(argv[1],"rb");if(!f)return 2;if(fread(h,sizeof(h),1,f)!=1||fread(arena,sizeof(arena),1,f)!=1||fread(gSmoke,1100,1,f)!=1||fread(gSmoke_column,1240,1,f)!=1)return 3;fclose(f);_controlfp(_PC_53,_MCW_PC);
    gNo_2d_effects=h[0];gAction_replay_mode=h[1];gSmoke_flags=h[2];gColumn_flags=h[3];gRendering_mirror=h[4];gN_BR_smoke_structs=h[10];rng=h[6];gBlend_actor=(br_actor*)arena;gBlend_model=(br_model*)((char*)arena+0x4000);gBlend_model2=(br_model*)((char*)arena+0x4100);gDont_render_actor=(br_actor*)((char*)arena+0x200);
    gLocal_buffer=(tU8*)arena+0x7000;gMr_chunky=gLocal_buffer+8;gPipe_callbacks=(tReplay_callback*)((char*)arena+0x13000);gPipe_buffer_phys_end=(tU8*)arena+0x13000;gPipe_buffer_working_end=gPipe_buffer_phys_end;gPipe_record_ptr=(tU8*)arena+0xb000;gPipe_buffer_start=h[8]?gPipe_record_ptr:NULL;gPipe_buffer_oldest=NULL;gLocal_buffer_size=8;gReentrancy_count=0;for(i=0;i<5;i++)gReentrancy_array[i]=(tPipe_chunk_type)(20+i);gProgram_state.racing=h[9];gProgram_state.current_car.car_master_actor=(br_actor*)((char*)arena+0x300);
    memset(&gCamera_to_world,0,48);memset(&gRearview_camera_to_world,0,48);gCamera_to_world.m[0][0]=gCamera_to_world.m[1][1]=gCamera_to_world.m[2][2]=1.f;gRearview_camera_to_world=gCamera_to_world;
    for(i=0;i<16;i++)gShade_list[i]=0x102030+i*0x10101;
    for(i=0;i<35;i++){tBRender_smoke*s=&gBR_smoke_structs[i];s->pos.v[0]=i+.25f;s->pos.v[1]=(float)(-(int)i)*.5f;s->pos.v[2]=1.f;s->r=.05f;s->strength=.45f;s->col=0x102030+i;s->aspect=.9f;s->material=(br_material*)((char*)arena+0x1000+256*i);s->model=gBlend_model;gBR_smoke_pointers[i]=s;wr(0x1000+256*i+0x58,(unsigned)(uintptr_t)((char*)arena+0x6000));}
    for(i=0;i<5;i++){unsigned off=i*0x100+12;v=rd(off);if(v)wr(off,(unsigned)(uintptr_t)((char*)arena+v-0x1000000));}
    wr(0x4008,(unsigned)(uintptr_t)((char*)arena+0x5000));wr(0x4108,(unsigned)(uintptr_t)((char*)arena+0x5100));
    n=full(state);memcpy(previous,state,n);printf("{\"baseline\":\"");hex(state,n);printf("\",\"calls\":[");RenderSmoke(NULL,NULL,(br_actor*)((char*)arena+0x100),NULL,h[5]);printf("],\"state\":\"");n=full(state);hex(state,n);printf("\"}\n");return 0;
}
'''
    harness=out/'harness.c';harness.write_text(driver)
    includes=[ROOT/r for r in ['src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/carma2/platform','src/carma2/s3/include','src/brender/include']]
    msvc=sorted((args.msvc_root.resolve()/'VC/Tools/MSVC').iterdir())[-1];sdk=args.msvc_root.resolve()/'Windows Kits/10';version=sorted(p.name for p in (sdk/'Include').iterdir())[-1]
    env=dict(os.environ);env['PATH']=str(msvc/'bin/Hostx64/x86')+os.pathsep+env['PATH'];env['INCLUDE']=os.pathsep.join(map(str,[msvc/'include',*[sdk/f'Include/{version}/{r}' for r in ['ucrt','shared','um']]]));env['LIB']=os.pathsep.join(map(str,[msvc/'lib/x86',*[sdk/f'Lib/{version}/{r}/x86' for r in ['ucrt','um']]]))
    old=args.msvc5_root.resolve();oldenv=dict(os.environ);oldenv['PATH']=str(old/'bin')+os.pathsep+str(old/'redist')+os.pathsep+oldenv['PATH'];oldenv['INCLUDE']=str(old/'include');oldenv['LIB']=str(old/'lib')
    commands={
        'msvc':[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(r) for r in includes],str(harness),'/Fo'+str(out/'msvc.obj'),'/Fe'+str(out/'msvc.exe')],
        'msvc5':[str(old/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(r) for r in includes],str(harness),'/Fo'+str(out/'msvc5.obj'),'/Fe'+str(out/'msvc5.exe')],
        'mingw':[str(args.gcc.resolve()),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-fno-strict-aliasing',*['-I'+str(r) for r in includes],str(harness),'-o',str(out/'mingw.exe')]
    }
    for compiler,cmd in commands.items():
        build=subprocess.run(cmd,cwd=out,env=env if compiler=='msvc' else oldenv if compiler=='msvc5' else None,text=True,capture_output=True)
        (out/(compiler+'-build.log')).write_text(build.stdout+build.stderr);assert build.returncode==0,build.stdout+build.stderr
    probe_source=ROOT/'scripts/verify_smoke_replay_original_x87_probe.c'
    probe_command=[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX',
        '/D_CRT_SECURE_NO_WARNINGS',str(probe_source),'/Fo'+str(out/'replay-x87-probe.obj'),
        '/Fe'+str(out/'replay-x87-probe.exe'),'/link','kernel32.lib']
    probe_build=subprocess.run(probe_command,cwd=out,env=env,text=True,capture_output=True)
    (out/'replay-x87-probe-build.log').write_text(probe_build.stdout+probe_build.stderr)
    assert probe_build.returncode==0,probe_build.stdout+probe_build.stderr
    pe=struct.unpack_from('<I',original,0x3c)[0];table=pe+24+struct.unpack_from('<H',original,pe+20)[0]
    for i in range(struct.unpack_from('<H',original,pe+6)[0]):
        _,rva,n,raw=struct.unpack_from('<IIII',original,table+40*i+8)
        if rva<=0xfa910<rva+n:code=original[raw+0xfa910-rva:raw+0xfa910-rva+2205];break
    decoder=Cs(CS_ARCH_X86,CS_MODE_32);decoder.detail=True;patches=[]
    for instruction in decoder.disasm(code,0x4fa910):
        for operand in instruction.operands:
            offset=None
            if operand.type==X86_OP_MEM and 0x580000<=operand.mem.disp<0x800000 and instruction.disp_size==4:
                offset=instruction.address-0x4fa910+instruction.disp_offset
            if operand.type==X86_OP_IMM and instruction.mnemonic not in ('call','jmp') and 0x580000<=operand.imm<0x800000 and instruction.imm_size==4:
                offset=instruction.address-0x4fa910+instruction.imm_offset
            if offset is not None:patches.append((offset,struct.unpack_from('<I',code,offset)[0]))
    patch_path=out/'replay-probe-patches.bin'
    patch_path.write_bytes(struct.pack('<I',len(patches))+b''.join(struct.pack('<2I',*pair) for pair in patches))
    physical=[]
    def bits(v):return struct.unpack('<I',struct.pack('<f',v))[0]
    cases=[]
    for hardware in [0,1]:
        for replay in [0,1]:
            for flags in [0,1,3,0x1000000,0x80000000,0x1000001]:cases.append(dict(hardware=hardware,replay=replay,flags=flags))
    for lifetime in [0,1,3999,4000,4001,0xffffffff]:
        for hardware in [0,1]:cases.append(dict(hardware=hardware,columns=3,lifetime=lifetime))
    for time in [0,1,100,249,250,251,500,0x80000000,0xffffffff]:cases.append(dict(time=time,flags=3))
    for sync in [0,1,499,0x80000000,0xffffffff]:cases.append(dict(sync=sync,flags=3))
    for kind in [0,6,7,8,15,16,23,255]:cases.append(dict(kind=kind,flags=1))
    for strength in [0,0x80000000,bits(.00001),bits(.5),bits(-.5),0x7fc12345,0x7f800000,0x7f800001,0xff800001]:cases.append(dict(strength=strength,flags=3))
    for radius in [0,bits(-.05),bits(.05),bits(.249999),bits(.3),bits(.300001),0x7fc12345]:cases.append(dict(radius=radius,flags=3))
    for velocity in [-.87,-.4347826,-.1,-0.,0.,.1,.4347826,.87]:cases.append(dict(vy=bits(velocity),kind=0,flags=3,time=251))
    for vy in [0x7fc12345,0xffc12345]:cases.append(dict(vy=vy,kind=0,flags=3,time=251))
    for y in [-.02,-.04,-.01,.01,.04,.1]:cases.append(dict(columns=1,glow_y=y,flags=1))
    cases += [dict(radius=0x7fc12345,kind=16,flags=3),dict(radius=0x7f800000,opposite_radius=0xff800000,flags=3)]
    for mutate in [1,2,4,8,3,5,10,15]:cases.append(dict(mutate=mutate,columns=1,flags=3,parent=1))
    cases += [dict(flags=0x1ffffff,columns=0x3ff),dict(flags=3,columns=0x80000000),dict(flags=0,columns=0x3ff),dict(flags=3,mirror=1),dict(flags=3,pipe=0),dict(flags=3,racing=0)]
    rng=random.Random(0x4fa910)
    for i in range(30):cases.append(dict(flags=3,time=rng.randrange(600),random=i))
    replay_nonfinite=[dict(strength=0x7fc12345),dict(strength=0x7f800001),
        dict(radius=0x7fc12345),dict(radius=0x7f800001),dict(strength=0xff801234,kind=16),
        dict(radius=0x7fc12345,kind=16),dict(radius=0x7f800001,kind=16),dict(strength=0xffc12345,kind=16)]
    cases += [dict(replay=1,flags=1,physical=True,**case) for case in replay_nonfinite]
    # Capture raw glow coordinates at the transform boundary; return finite
    # transformed coordinates to isolate this originally MOV-only transfer.
    cases += [dict(columns=1,flags=0,glow_x=0x7f800001,glow_z=0xff801234,finite_transform=1),
        dict(columns=1,flags=0,glow_x=0xffc12345,glow_z=0x7fc23456,finite_transform=1)]
    results=[]
    for index,case in enumerate(cases):
        arena=bytearray(b'\xa5'*SIZE);smoke=bytearray(1100);columns=bytearray(1240)
        def put(buffer,off,value):struct.pack_into('<I',buffer,off,value&0xffffffff)
        for i in range(5):arena[i*0x100:i*0x100+100]=bytes(100)
        put(arena,12,HEAP+0x200 if case.get('parent',0) else 0)
        for i in range(35):put(arena,0x1000+256*i+0x58,HEAP+0x6000)
        put(arena,0x4008,HEAP+0x5000);put(arena,0x4108,HEAP+0x5100)
        struct.pack_into('<H',arena,0x4010,4);struct.pack_into('<H',arena,0x4110,6)
        struct.pack_into('<6I',arena,0x6000,133,1,190,75<<16,0,0)
        for i in range(71):arena[0x13000+24*i:0x13000+24*i+24]=bytes(24);put(arena,0x13000+24*i+4,10)
        put(arena,0x7000,15);put(arena,0x7004,0)
        for i in range(25):
            struct.pack_into('<I9fBBH',smoke,44*i,case.get('sync',i%2*125),i*.3+.1,-i*.2-.3,i*.4+.5,.25+i*.01,-.15,.35,.075,.65,.3,case.get('kind',i%16),1,0)
            for off,key in [(28,'radius'),(32,'strength'),(36,'decay'),(20,'vy')]:
                if key in case:put(smoke,44*i+off,case[key])
        for i in range(10):
            put(columns,124*i+20,case.get('lifetime',3000+i*200));put(columns,124*i+28,i%3)
            struct.pack_into('<3f',columns,124*i+52,i*.1,case.get('glow_y',i*.2),i*.3)
            for off,key in [(52,'glow_x'),(60,'glow_z')]:
                if key in case:put(columns,124*i+off,case[key])
        if 'opposite_radius' in case:put(smoke,44+28,case['opposite_radius'])
        if 'random' in case:
            for i in range(2):
                for off in [4,8,12,16,20,24]:struct.pack_into('<f',smoke,44*i+off,rng.uniform(-10,10))
                for off in [28,32,36]:struct.pack_into('<f',smoke,44*i+off,rng.uniform(.001,.9))
        h=[case.get('hardware',1),case.get('replay',0),case.get('flags',1),case.get('columns',0),case.get('mirror',0),case.get('time',250),0x12345678,case.get('parent',0),case.get('pipe',1),case.get('racing',1),2,case.get('mutate',0),0,0,0,0,0,0,bits(.73),0,case.get('finite_transform',0),0,0,0]
        fixture=out/'fixture.bin';fixture.write_bytes(struct.pack('<24I',*h)+arena+smoke+columns)
        expected=execute(*images[0],h,arena,smoke,columns);checks=[]
        if case.get('physical'):
            radius=case.get('radius',bits(.075));strength=case.get('strength',bits(.65));kind=case.get('kind',0)
            observed=json.loads(subprocess.check_output([str(out/'replay-x87-probe.exe'),str(args.original.resolve()),str(patch_path),str(radius),str(strength),str(kind)],text=True))
            state=bytes.fromhex(expected['state']);record=84+20+64+96+1100+1240
            assert observed['count']==1 and observed['input_radius']==radius and observed['input_strength']==strength
            for field,offset in [('radius',12),('strength',16),('aspect',24)]:
                assert observed[field]==struct.unpack_from('<I',state,record+offset)[0],(case,field,observed[field])
            physical.append(dict(case=case,original_metadata=observed))
        if len(images)>1:checks.append(('recompiled-x86',execute(*images[1],h,arena,smoke,columns)))
        for compiler in commands:checks.append((compiler,json.loads(subprocess.check_output([str(out/(compiler+'.exe')),str(fixture)],text=True))))
        for compiler,actual in checks:
            if actual!=expected:
                failure=out/f'failure-{compiler}-{index}.json';failure.write_text(json.dumps(dict(case=case,original=expected,actual=actual),indent=2))
                raise AssertionError(f'{compiler} RenderSmoke case {index} differs: {failure}')
            results.append(dict(case=case,compiler=compiler,passed=True,calls=len(actual['calls']),state_sha256=hashlib.sha256(bytes.fromhex(actual['state'])).hexdigest()))
    physical_evidence=dict(original_sha256=ORIGINAL_HASH,probe_source_sha256=hashlib.sha256(probe_source.read_bytes()).hexdigest(),probe_executable_sha256=hashlib.sha256((out/'replay-x87-probe.exe').read_bytes()).hexdigest(),compiler_command=probe_command,precision_control=53,relocated_data_references=patches,scope='Original replay aspect arithmetic and raw circle metadata; external APIs and final renderer are controlled return stubs.',cases=physical)
    (out/'physical-replay-x87.json').write_text(json.dumps(physical_evidence,indent=2)+'\n')
    verification=dict(scenario_count=len(cases),comparisons=len(results),original_sha256=ORIGINAL_HASH,source_sha256=hashlib.sha256(SOURCE.read_bytes()).hexdigest(),piping_source_sha256=hashlib.sha256(PIPE.read_bytes()).hexdigest(),bodies_sha256=[hashlib.sha256(b.encode()).hexdigest() for b in bodies],compiler_commands=commands,cases=results,physical_replay_x87=physical_evidence,oracle_normalizations=[])
    if args.symbols:
        verification.update(recompiled_sha256=hashlib.sha256(args.recompiled.read_bytes()).hexdigest(),symbols_sha256=hashlib.sha256(args.symbols.read_bytes()).hexdigest())
    (out/'verification.json').write_text(json.dumps(verification,indent=2)+'\n')
    print(f'PASS: {len(cases)} scenarios, {len(results)} complete RenderSmoke comparisons.')


if __name__=='__main__':main()
