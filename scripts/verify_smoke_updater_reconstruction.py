#!/usr/bin/env python3
"""Verify complete smoke updater closure against original x86 and physical original FPU.

Physical x86 executes original DoSmokeColumn, MungeSmokeColumn, IsOnNonCarHalf,
FlameAnimate, CreatePuffOfSmoke, matrix/vector leaves, rate, random and FTOL bytes.
Published trig and replay dependencies execute actual production C bodies.
CRT rand and BRender material-update callbacks are controlled boundaries.
Only proven unwritten replay packet padding and propagated copies are excluded.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import struct
import subprocess
import sys

ROOT=Path(__file__).resolve().parents[1]
SOURCE=ROOT/'src/carma2/newcommon/32-spark.c'
PIPE=ROOT/'src/carma2/newcommon/23-piping.c'
UTILITY=ROOT/'src/carma2/newcommon/41-utility.c'
TRIG=ROOT/'src/carma2/newcommon/36-trig.c'
MATRIX=ROOT/'src/brender/core/math/matrix34.c'
VECTOR=ROOT/'src/brender/core/math/vector.c'
CRUSH=ROOT/'src/carma2/newcommon/13-crush1.c'
ORIGINAL_HASH='9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
ADDR=dict(FlameAnimate=0x4fbdd0,ARGetReplayRate=0x402390,BrMaterialUpdate=0x520e70,BrMatrix34Scale=0x532ad0,FastScalarArcTan2Angle=0x512010,DRMatrix34RotateY=0x512170,gCamera_to_world=0x74d460,gFlame_map=0x6a8638,gFlame_frame_size=0x660118,gReplay_rate=0x676900,SRandomBetween=0x513620,SRandomPosNeg=0x513650,ftol=0x575b80,CreateSmokeColumn=0x4fb890,ConditionalSmokeColumn=0x4fcab0,SmudgeCar=0x4fc9c0,IRandomBetween=0x513520,rand=0x576970,AdjustSmokeColumn=0x4fb580,CreateSmokeColumn2=0x4fb630,AdjustFlame=0x4fba00,
    StopCarSmoking=0x4fca40,StopCarSmokingInstantly=0x4fca70,StopObjectSmokingInstantly=0x4fca90,
    IsCarSmoking=0x4fed90,AddSmokeColumnToPipingSession=0x4c70c0,AddFlameToPipingSession=0x4c70f0,
    ARStartPipingSession=0x4025e0,AREndPipingSession=0x402890,EndPipingSession2=0x402660,
    ARAddDataToSession=0x4028a0,ARAddVariedDataToSession=0x402930,LengthOfSession=0x403750,
    gSmoke_column=0x6a96a8,gColumn_flags=0x6aa59c,gColumn_next=0x6aa5a0,gSmoke_on=0x660110,
    gNon_track_actor=0x7634b8,gUNK_0069410c=0x69410c,gAction_replay_mode=0x676914,
    gProgram_state=0x75bb80,BrActorAdd=0x51dfe0,BrActorRemove=0x51e070,FatalError=0x44c230,
    gPipe_buffer_start=0x676908,gLocal_buffer=0x6768b4,gMr_chunky=0x6768a0,gPipe_callbacks=0x6768e8,
    gPipe_buffer_phys_end=0x6768f0,gPipe_buffer_working_end=0x6768c4,gPipe_buffer_oldest=0x6768ac,
    gPipe_record_ptr=0x6768bc,gLocal_buffer_size=0x6768f4,gReentrancy_count=0x6768e4,gReentrancy_array=0x6768d0)
ADDR.update(DoSmokeColumn=0x4fc0e0,MungeSmokeColumn=0x4fba40,IsOnNonCarHalf=0x42df70,CreatePuffOfSmoke=0x4fb330,BrMatrix34ApplyP=0x5334a0,BrMatrix34ApplyV=0x533520,BrVector3SetFloat=0x534590,gSmoke=0x6a8ce0,gSmoke_flags=0x6aa590,gSmoke_next=0x6aa594,gPHIL_mechanics_time_sync=0x74a5e4,gCar_to_view=0x655f54)
PTR=['gPipe_buffer_start','gLocal_buffer','gMr_chunky','gPipe_callbacks','gPipe_buffer_phys_end',
    'gPipe_buffer_working_end','gPipe_buffer_oldest','gPipe_record_ptr','gNon_track_actor']
VAL=['gColumn_flags','gColumn_next','gSmoke_on','gUNK_0069410c','gLocal_buffer_size','gReentrancy_count','gAction_replay_mode']
HEAP,STACK,STOP,SIZE=0x1000000,0x1080000,0x1081000,0x30000


def main():
    p=argparse.ArgumentParser(description=__doc__)
    for n,d in [('original',ROOT/'build-reference/originals/CARMA2_HW.EXE'),('recompiled',ROOT/'build-matching/rec2.exe'),
        ('output',ROOT/'build-audit/smoke-updater-tests'),('dependency-dir',ROOT/'build-audit/joystick/python-deps'),
        ('gcc',ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'),('msvc-root',ROOT/'build-tools/msvc-modern'),
        ('msvc5-root',ROOT/'build-tools/msvc500sp3')]:p.add_argument('--'+n,type=Path,default=d)
    p.add_argument('--symbols',type=Path)
    args=p.parse_args();sys.path.insert(0,str(args.dependency_dir.resolve()))
    from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
    from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_EAX,UC_X86_REG_ECX,UC_X86_REG_EDX,UC_X86_REG_FPCW,UC_X86_REG_EBX,UC_X86_REG_ESI,UC_X86_REG_EDI,UC_X86_REG_EBP
    from verify_tinted_native_reconstruction import extract
    original=args.original.read_bytes();assert hashlib.sha256(original).hexdigest()==ORIGINAL_HASH
    images=[(original,ADDR)]
    if args.symbols:
        m=json.loads(args.symbols.read_text());b=args.recompiled.read_bytes()
        assert m['original_sha256']==ORIGINAL_HASH and m['recompiled_sha256']==hashlib.sha256(b).hexdigest()
        syms={s['orig']:s for s in m['symbols']};images.append((b,{n:syms[a]['recomp'] for n,a in ADDR.items()}))
    out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)
    def execute(data,a,h,arena,columns):
        u=Uc(UC_ARCH_X86,UC_MODE_32);u.mem_map(0x400000,0x500000);u.mem_map(HEAP,0x100000)
        pe=struct.unpack_from('<I',data,0x3c)[0];table=pe+24+struct.unpack_from('<H',data,pe+20)[0]
        for i in range(struct.unpack_from('<H',data,pe+6)[0]):
            _,rva,n,raw=struct.unpack_from('<IIII',data,table+40*i+8);u.mem_write(0x400000+rva,data[raw:raw+n])
        def wr(at,v):u.mem_write(at,struct.pack('<I',v&0xffffffff))
        def rd(at):return struct.unpack('<I',u.mem_read(at,4))[0]
        u.mem_write(HEAP,bytes(arena));u.mem_write(a['gSmoke_column'],bytes(columns))
        for n,v in zip(VAL,[h[2],h[3],h[4],0x12345678,8,h[23],h[6]]):wr(a[n],v)
        for n,off in zip(PTR,[0xb000 if h[5] else None,0x7000,0x7008,0x13000,0x13000,0x13000,None,0xb000,0x200]):wr(a[n],0 if off is None else HEAP+off)
        for i in range(5):wr(a['gReentrancy_array']+4*i,20+i)
        u.mem_write(a['gProgram_state'],bytes(24272));wr(a['gProgram_state']+0x28,h[7]);wr(a['gProgram_state']+0x4c,h[15]);wr(a['gReplay_rate'],h[14]);wr(a['gSmoke_flags'],h[17]);wr(a['gSmoke_next'],h[20]);wr(a['gPHIL_mechanics_time_sync'],h[9]);wr(a['gCar_to_view'],HEAP+0x14000);u.mem_write(a['gSmoke'],bytes([0xa5])*1100)
        u.mem_write(a['gCamera_to_world'],struct.pack('<12I',0x3f800000,0,0,0,0x3f800000,0,h[11],0,h[12],0,0,0))
        for i in range(20):wr(a['gFlame_map']+4*i,HEAP+0x1a000+256*i)
        calls=[];rng_count=[0];padding=bytearray(SIZE);pointer_fields=bytearray(SIZE);pending=[];callback_count=[0]
        wr(a['gPipe_record_ptr'],HEAP+(h[21] or 0xb000))
        def snapshot():
            copy=bytearray(u.mem_read(HEAP,SIZE))
            for i in range(SIZE):
                if padding[i]:copy[i]=0
            return (b''.join(struct.pack('<I',rd(a[n])) for n in PTR+VAL)+bytes(u.mem_read(a['gReentrancy_array'],20))+bytes(u.mem_read(a['gSmoke_column'],1240))+copy+padding+pointer_fields+struct.pack('<I',rng_count[0])+bytes(u.mem_read(a['gCamera_to_world'],48))+bytes(u.mem_read(a['gFlame_map'],80))+bytes(u.mem_read(a['gReplay_rate'],4))+bytes(u.mem_read(a['gFlame_frame_size'],40))+bytes(u.mem_read(a['gSmoke'],1100))+b''.join(struct.pack('<I',rd(a[n])) for n in ['gSmoke_flags','gSmoke_next','gPHIL_mechanics_time_sync','gCar_to_view'])+bytes(u.mem_read(a['gProgram_state'],24272))).hex()
        def observe_add(kind,size):
            commit_pending()
            if not rd(a['gPipe_buffer_start']) or rd(a['gAction_replay_mode']) or not rd(a['gProgram_state']+0x28) or rd(a['gLocal_buffer_size'])+size+4>=15000:return
            pending.append((kind,size,rd(a['gMr_chunky'])-HEAP))
        def commit_pending():
            if not pending:return
            kind,size,dest=pending.pop()
            padding[dest:dest+size+4]=bytes(size+4);pointer_fields[dest:dest+size+4]=bytes(size+4)
            if kind in [17,18]:
                off=18 if kind==17 else 2;padding[dest+4+off:dest+6+off]=b'\1\1'
                if kind==17:
                    for off in [4,8,12]:pointer_fields[dest+4+off]=1
        def observe_end():
            commit_pending()
            if not rd(a['gPipe_buffer_start']) or rd(a['gAction_replay_mode']) or not rd(a['gProgram_state']+0x28):return
            local=rd(a['gLocal_buffer'])-HEAP;size=rd(a['gLocal_buffer_size'])
            padding[local+size:local+size+2]=b'\0\0';pointer_fields[local+size:local+size+2]=b'\0\0';size+=2
            if rd(HEAP+local+4) and size<=15000:
                dest=rd(a['gPipe_record_ptr'])-HEAP
                if HEAP+dest+size>rd(a['gPipe_buffer_phys_end']):dest=rd(a['gPipe_buffer_start'])-HEAP
                padding[dest:dest+size]=padding[local:local+size];pointer_fields[dest:dest+size]=pointer_fields[local:local+size]
        material_count=[0]
        def mutate_material(material):
            if material_count[0]==0:
                col=a['gSmoke_column']+124*h[1]
                if h[19]&1:wr(col+40,3)
                if h[19]&2:wr(a['gFlame_map']+4*rd(col+40),HEAP+0x1a000+256*19)
                if h[19]&4:wr(col+64,0x3dc12345);wr(col+76,0xbe112233);wr(col+88,0x7f800001);wr(col+100,0xff801234)
                if h[19]&8:wr(HEAP+0x700+0x400*h[1],HEAP+0x1c00)
                if h[19]&16:wr(HEAP+0x700+0x400*h[1]+28,HEAP+0x19d00)
                if h[19]&32:wr(a['gReplay_rate'],0xbf800000)
                if h[19]&64:wr(a['gAction_replay_mode'],1)
                if h[19]&256:wr(a['gColumn_flags'],512)
                if h[19]&512:wr(col+28,2)
                if h[19]&1024:wr(col+8,0)
                if h[19]&2048:wr(col+8,HEAP+0x28000)
                if h[19]&4096:wr(col+4,0)
            material_count[0]+=1
        def hook(uc,at,size,context):
            if at==a['BrMaterialUpdate']:
                sp=uc.reg_read(UC_X86_REG_ESP);material=rd(sp+4);flags=rd(sp+8);calls.append(['m',material,flags,snapshot()])
                mutate_material(material);uc.reg_write(UC_X86_REG_EIP,rd(sp));uc.reg_write(UC_X86_REG_ESP,sp+4);return
            if at==a['rand']:
                rng_count[0]+=1;calls.append(['r',h[10],snapshot()]);sp=uc.reg_read(UC_X86_REG_ESP)
                if rng_count[0]==1:
                    col=a['gSmoke_column']+124*h[1]
                    if h[24]&1:wr(col+52,0x3e923456)
                    if h[24]&2:wr(col+28,0)
                    if h[24]&4:wr(col+20,3999)
                    if h[24]&8:wr(a['gPHIL_mechanics_time_sync'],5678)
                    if h[24]&16:wr(col+8,0)
                uc.reg_write(UC_X86_REG_EAX,h[10]);uc.reg_write(UC_X86_REG_EIP,rd(sp));uc.reg_write(UC_X86_REG_ESP,sp+4);return
            if at==a['ARAddVariedDataToSession']:
                observe_add(rd(uc.reg_read(UC_X86_REG_ESP)+4),20);return
            if at==a['EndPipingSession2']:observe_end();return
            if at==0x1082000:
                packet=uc.reg_read(UC_X86_REG_ECX);kind=rd(rd(a['gLocal_buffer']));copy=bytearray(u.mem_read(packet,24));off=22 if kind==17 else 6;copy[off:off+2]=bytes(2)
                calls.append(['c',kind,copy.hex(),snapshot()])
                if callback_count[0]==0:
                    col=a['gSmoke_column']+124*h[1]
                    if h[22]&1:wr(a['gColumn_flags'],513)
                    if h[22]&2:wr(col+28,0)
                    if h[22]&4:wr(col+36,HEAP+0x1c00)
                callback_count[0]+=1;sp=uc.reg_read(UC_X86_REG_ESP);uc.reg_write(UC_X86_REG_EAX,0);uc.reg_write(UC_X86_REG_EIP,rd(sp));uc.reg_write(UC_X86_REG_ESP,sp+4);return
            if at not in (a['BrActorAdd'],a['BrActorRemove'],a['FatalError']):return
            sp=uc.reg_read(UC_X86_REG_ESP);arg=rd(sp+4)
            if at==a['FatalError']:raise AssertionError('Unexpected FatalError')
            if at==a['BrActorAdd']:
                actor=rd(sp+8);calls.append(['a',arg,actor,snapshot()]);wr(actor+12,arg);result=actor
            else:calls.append(['d',arg,snapshot()]);wr(arg+12,0);result=arg
            if h[19]:wr(a['gColumn_next'],h[19]-1)
            uc.reg_write(UC_X86_REG_EAX,result);uc.reg_write(UC_X86_REG_EIP,rd(sp));uc.reg_write(UC_X86_REG_ESP,sp+4)
        u.hook_add(UC_HOOK_CODE,hook);u.reg_write(UC_X86_REG_FPCW,0x27f)
        def invoke(name,x=0,y=0,tail=()):
            wr(STACK,STOP)
            for i,v in enumerate(tail):wr(STACK+4+4*i,v)
            u.reg_write(UC_X86_REG_ESP,STACK);u.reg_write(UC_X86_REG_ECX,x);u.reg_write(UC_X86_REG_EDX,y)
            preserved=[UC_X86_REG_EBX,UC_X86_REG_ESI,UC_X86_REG_EDI,UC_X86_REG_EBP]
            for i,r in enumerate(preserved):u.reg_write(r,0x12340000+i)
            u.emu_start(a[name],STOP,count=2000000);assert u.reg_read(UC_X86_REG_EIP)==STOP
            assert u.reg_read(UC_X86_REG_ESP)==STACK+4+4*len(tail)
            assert [u.reg_read(r) for r in preserved]==[0x12340000+i for i in range(4)]
            return u.reg_read(UC_X86_REG_EAX)
        value=None
        if h[0]==0:invoke('MungeSmokeColumn',h[13])
        elif h[0]==1:invoke('DoSmokeColumn',h[1],h[13],[HEAP+h[18]])
        else:value=invoke('IsOnNonCarHalf',HEAP+0x14000,HEAP+h[18])
        return dict(calls=calls,state=snapshot(),value=value)
    pipe_names=['ARGetReplayRate','ReplayCopyForward','LengthOfSession','ARStartPipingSession','EndPipingSession2','AREndPipingSession','ARAddDataToSession','ARAddVariedDataToSession','AddSmokeColumnToPipingSession','AddFlameToPipingSession']
    names=['SmokeScalarRound','SmokeScalarBits','FlameAnimate','CreatePuffOfSmoke','SmudgeCar','DoSmokeColumn','MungeSmokeColumn']
    trig_names=['TrigArcTanIsNaN','TrigArcTanAbs','TrigArcTanRatio','FastFloatArcTan2Angle','FastScalarArcTan2Angle','FastFloatSin','FastFloatCos','FastScalarSin','FastScalarCos','FastScalarSinAngle','FastScalarCosAngle','DRMatrix34RotateY']
    bodies=[extract(MATRIX.read_text(),n) for n in ['BrMatrix34Scale','BrMatrix34ApplyP','BrMatrix34ApplyV']]+[extract(VECTOR.read_text(),'BrVector3SetFloat'),extract(CRUSH.read_text(),'IsOnNonCarHalf')]+[extract(TRIG.read_text(),n) for n in trig_names]+[extract(UTILITY.read_text(),n) for n in ['UtilityRandomNegative','UtilityRandomScalar','IRandomBetween','FRandomBetween','SRandomBetween','SRandomPosNeg']]+[extract(PIPE.read_text(),n) for n in pipe_names]+[extract(SOURCE.read_text(),n) for n in names]
    # Read-only observers track only the proved undefined bytes and their copies;
    # all actual recording statements remain compiled and execute unchanged.
    compiled_bodies=[b.replace('    const tReplay_callback* callbacks;','    const tReplay_callback* callbacks;\n\n    observe_add(pType, 20);',1) if ' ARAddVariedDataToSession(' in b[:b.index('{')] else b.replace('    tPipe_chunk_type type;','    tPipe_chunk_type type;\n\n    observe_end();',1) if ' EndPipingSession2(' in b[:b.index('{')] else b for b in bodies]
    driver=r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <float.h>
#include "32-spark.h"
#include "13-crush1.h"
#include "49-physics.h"
#include "23-piping.h"
#include "52-errors.h"
#include "41-utility.h"
#include "36-trig.h"
#define M(row,col) mat->m[row][col]
#include "globvars.h"
#include "rec2_macros.h"
typedef char layouts[sizeof(tSmoke_column)==124&&sizeof(tSmoke_column_core)==16&&sizeof(br_actor)==100&&sizeof(tCar_spec)<=0x2000&&sizeof(tPhysics_object)<=0x800&&sizeof(tSpecial_volume)<=0x800&&offsetof(tCar_spec,collision_info)==8&&offsetof(tCar_spec,driver)==12&&offsetof(tCar_spec,car_master_actor)==16&&offsetof(tCar_spec,fire_vertex)==0xac&&offsetof(tCar_spec,knackered)==0x1d4&&offsetof(tPhysics_object,last_special_volume)==0x258&&offsetof(tSpecial_volume,gravity_multiplier)==0x88?1:-1];
typedef char updater_layouts[sizeof(tProgram_state)==24272&&offsetof(tProgram_state,current_car)==172&&offsetof(tProgram_state,cockpit_on)==0x4c&&offsetof(tCar_spec,driver_z_offset)==0x484&&offsetof(tCar_spec,car_actor)==0xe10&&offsetof(tCar_spec,car_crush_spec)==0x18d4&&offsetof(tCar_crush_spec,field_0x4b8)==0x4b8&&offsetof(tCar_crush_spec,field_0x548)==0x548&&offsetof(tCar_crush_spec,field_0x554)==0x554&&offsetof(tPhysics_object,shape)==0x20&&offsetof(tPhysics_object,v)==0x68&&offsetof(tPhysics_object,omega)==0x74&&offsetof(tPhysics_shape,common)+offsetof(tCollision_shape_common,bb)+offsetof(br_bounds3,min)+sizeof(br_scalar)==8&&offsetof(br_model,prepared)==76&&sizeof(fmt_vertex)==32?1:-1];
typedef void (C2_HOOK_FASTCALL* raw_flame_fn)(int,int,tU32,tU32,tU32,tU32);
tU8*gPipe_buffer_start,*gLocal_buffer,*gMr_chunky,*gPipe_buffer_phys_end,*gPipe_buffer_working_end,*gPipe_buffer_oldest,*gPipe_record_ptr;
const tReplay_callback*gPipe_callbacks;tU32 gLocal_buffer_size,gUNK_0069410c;int gReentrancy_count;tPipe_chunk_type gReentrancy_array[5];
int gSmoke_flags,gSmoke_next,gPHIL_mechanics_time_sync;tSmoke gSmoke[25];tCar_spec*gCar_to_view;
int gAction_replay_mode,gColumn_flags,gColumn_next,gSmoke_on;tProgram_state gProgram_state;tSmoke_column gSmoke_column[10];br_actor*gNon_track_actor;
static unsigned arena[0x30000/4],h[32];static int events,callback_count;static unsigned rng_count,material_count;static unsigned char padding[0x30000],pointer_fields[0x30000];static unsigned pending_dest,pending_kind,pending_size,pending;
br_matrix34 gCamera_to_world;br_pixelmap*gFlame_map[20];float gReplay_rate;
static int C2_HOOK_FASTCALL test_length(tPipe_chunk*packet);
static unsigned norm(unsigned v){unsigned b=(unsigned)(uintptr_t)arena;if(v>=b&&v<=b+sizeof(arena))return v-b+0x1000000;if(v==(unsigned)(uintptr_t)test_length)return 0x1082000;return v;}
static unsigned rd(unsigned off){unsigned v;memcpy(&v,(char*)arena+off,4);return v;}
static void wr(unsigned off,unsigned v){memcpy((char*)arena+off,&v,4);}
static void hex(const void*p,unsigned n){static const char digits[]="0123456789abcdef";char buffer[1024];unsigned i,j,count;const unsigned char*bytes=(const unsigned char*)p;for(i=0;i<n;i+=count){count=n-i;if(count>512)count=512;for(j=0;j<count;j++){buffer[2*j]=digits[bytes[i+j]>>4];buffer[2*j+1]=digits[bytes[i+j]&15];}fwrite(buffer,2,count,stdout);}}
static void commit_pending(void){unsigned off;if(!pending)return;pending=0;memset(padding+pending_dest,0,pending_size+4);memset(pointer_fields+pending_dest,0,pending_size+4);if(pending_kind==17||pending_kind==18){off=pending_kind==17?18:2;padding[pending_dest+4+off]=padding[pending_dest+5+off]=1;if(pending_kind==17){for(off=4;off<=12;off+=4)pointer_fields[pending_dest+4+off]=1;}}}
static void observe_add(int kind,unsigned size){commit_pending();if(!gPipe_buffer_start||gAction_replay_mode||!gProgram_state.racing||gLocal_buffer_size+size+4>=15000)return;pending=1;pending_dest=(unsigned)(gMr_chunky-(tU8*)arena);pending_kind=kind;pending_size=size;}
static void observe_end(void){unsigned local,size,dest;commit_pending();if(!gPipe_buffer_start||gAction_replay_mode||!gProgram_state.racing)return;local=(unsigned)(gLocal_buffer-(tU8*)arena);size=gLocal_buffer_size;memset(padding+local+size,0,2);memset(pointer_fields+local+size,0,2);size+=2;if(rd(local+4)&&size<=15000){dest=(unsigned)(gPipe_record_ptr-(tU8*)arena);if(gPipe_record_ptr+size>gPipe_buffer_phys_end)dest=(unsigned)(gPipe_buffer_start-(tU8*)arena);memcpy(padding+dest,padding+local,size);memcpy(pointer_fields+dest,pointer_fields+local,size);}}
static void snapshot(void){unsigned prefix[16],i,j,v;unsigned char copy[sizeof(arena)];tSmoke_column columns[10];
    prefix[0]=norm((unsigned)(uintptr_t)gPipe_buffer_start);prefix[1]=norm((unsigned)(uintptr_t)gLocal_buffer);prefix[2]=norm((unsigned)(uintptr_t)gMr_chunky);prefix[3]=norm((unsigned)(uintptr_t)gPipe_callbacks);prefix[4]=norm((unsigned)(uintptr_t)gPipe_buffer_phys_end);prefix[5]=norm((unsigned)(uintptr_t)gPipe_buffer_working_end);prefix[6]=norm((unsigned)(uintptr_t)gPipe_buffer_oldest);prefix[7]=norm((unsigned)(uintptr_t)gPipe_record_ptr);prefix[8]=norm((unsigned)(uintptr_t)gNon_track_actor);prefix[9]=gColumn_flags;prefix[10]=gColumn_next;prefix[11]=gSmoke_on;prefix[12]=gUNK_0069410c;prefix[13]=gLocal_buffer_size;prefix[14]=gReentrancy_count;prefix[15]=gAction_replay_mode;
    memcpy(columns,gSmoke_column,sizeof(columns));for(i=0;i<10;i++){columns[i].core.field_0x0=norm(columns[i].core.field_0x0);columns[i].core.car=(tCar_spec*)(uintptr_t)norm((unsigned)(uintptr_t)columns[i].core.car);columns[i].core.collision_info=(tPhysics_object*)(uintptr_t)norm((unsigned)(uintptr_t)columns[i].core.collision_info);columns[i].core.car_actor=(br_actor*)(uintptr_t)norm((unsigned)(uintptr_t)columns[i].core.car_actor);columns[i].flame_actor=(br_actor*)(uintptr_t)norm((unsigned)(uintptr_t)columns[i].flame_actor);}
    memcpy(copy,arena,sizeof(copy));for(i=0;i<46;i++)for(j=0;j<5;j++){unsigned off=i*0x100+(j<4?4*j:28);v=norm(rd(off));memcpy(copy+off,&v,4);}for(j=1;j<4;j++){v=norm(rd(0xc000+4*j));memcpy(copy+0xc000+4*j,&v,4);}
    {unsigned offsets[13]={0x14008,0x14010,0x14e10,0x158d4,0x16000,0x16020,0x16258,0x318,0x22018,0x2304c,0x24018,0x25010,0x28000};for(i=0;i<13;i++){v=norm(rd(offsets[i]));memcpy(copy+offsets[i],&v,4);}}
    for(i=0;i<46;i++){v=norm(rd(0x17000+256*i+64));memcpy(copy+0x17000+256*i+64,&v,4);}
    for(i=0;i<71;i++){v=norm(rd(0x13000+24*i+8));memcpy(copy+0x13000+24*i+8,&v,4);}
    for(i=0;i<sizeof(copy);i++){if(pointer_fields[i]){v=norm(rd(i));memcpy(copy+i,&v,4);}}
    for(i=0;i<sizeof(copy);i++){if(padding[i])copy[i]=0;}
    hex(prefix,sizeof(prefix));hex(gReentrancy_array,20);hex(columns,sizeof(columns));hex(copy,sizeof(copy));hex(padding,sizeof(padding));hex(pointer_fields,sizeof(pointer_fields));hex(&rng_count,4);hex(&gCamera_to_world,48);for(i=0;i<20;i++){v=norm((unsigned)(uintptr_t)gFlame_map[i]);hex(&v,4);}hex(&gReplay_rate,4);hex(gFlame_frame_size,40);hex(gSmoke,1100);{unsigned extra[4];extra[0]=gSmoke_flags;extra[1]=gSmoke_next;extra[2]=gPHIL_mechanics_time_sync;extra[3]=norm((unsigned)(uintptr_t)gCar_to_view);hex(extra,16);}hex(&gProgram_state,24272);
}
static int C2_HOOK_FASTCALL test_length(tPipe_chunk*packet){unsigned kind=rd((unsigned)(gLocal_buffer-(tU8*)arena)),off=kind==17?22:6,i,v;unsigned char copy[24];memcpy(copy,packet,24);memset(copy+off,0,2);if(kind==17){for(i=8;i<=16;i+=4){memcpy(&v,copy+i,4);v=norm(v);memcpy(copy+i,&v,4);}}printf("%s[\"c\",%u,\"",events++?",":"",kind);hex(copy,24);printf("\",\"");snapshot();printf("\"]");if(callback_count++==0){if(h[22]&1)gColumn_flags=513;if(h[22]&2)gSmoke_column[h[1]].colour=0;if(h[22]&4)gSmoke_column[h[1]].flame_actor=(br_actor*)((char*)arena+0x1c00);}return 0;}
br_actor*C2_HOOK_CDECL BrActorAdd(br_actor*parent,br_actor*actor){printf("%s[\"a\",%u,%u,\"",events++?",":"",norm((unsigned)(uintptr_t)parent),norm((unsigned)(uintptr_t)actor));snapshot();printf("\"]");actor->parent=parent;if(h[19])gColumn_next=h[19]-1;return actor;}
br_actor*C2_HOOK_CDECL BrActorRemove(br_actor*actor){printf("%s[\"d\",%u,\"",events++?",":"",norm((unsigned)(uintptr_t)actor));snapshot();printf("\"]");actor->parent=NULL;if(h[19])gColumn_next=h[19]-1;return actor;}
void C2_HOOK_CDECL BrMaterialUpdate(br_material*material,br_uint_16 flags){unsigned off=h[1];printf("%s[\"m\",%u,%u,\"",events++?",":"",norm((unsigned)(uintptr_t)material),flags);snapshot();printf("\"]");if(material_count++==0){if(h[19]&1)gSmoke_column[off].frame_count[0]=3;if(h[19]&2)gFlame_map[gSmoke_column[off].frame_count[0]]=(br_pixelmap*)((char*)arena+0x1a000+256*19);if(h[19]&4){unsigned b[4]={0x3dc12345,0xbe112233,0x7f800001,0xff801234};memcpy(&gSmoke_column[off].scale_x[0],b,4);memcpy(&gSmoke_column[off].scale_y[0],b+1,4);memcpy(&gSmoke_column[off].offset_x[0],b+2,4);memcpy(&gSmoke_column[off].offset_z[0],b+3,4);}if(h[19]&8)((br_actor*)((char*)arena+0x700+0x400*off))->next=(br_actor*)((char*)arena+0x1c00);if(h[19]&16)((br_actor*)((char*)arena+0x700+0x400*off))->material=(br_material*)((char*)arena+0x19d00);if(h[19]&32){unsigned b=0xbf800000;memcpy(&gReplay_rate,&b,4);}if(h[19]&64)gAction_replay_mode=1;if(h[19]&256)gColumn_flags=512;if(h[19]&512)gSmoke_column[off].colour=2;if(h[19]&1024)gSmoke_column[off].core.collision_info=NULL;if(h[19]&2048)gSmoke_column[off].core.collision_info=(tPhysics_object*)((char*)arena+0x28000);if(h[19]&4096)gSmoke_column[off].core.car=NULL;}}
void C2_NORETURN C2_HOOK_CDECL FatalError(int index,...){(void)index;abort();}
'''
    import re
    for source,pattern in [(SOURCE,r'static tU8 gFlame_frame_size\[20\]\[2\] = \{.*?\n\};'),(TRIG,r'float gFloat_sine_table\[91\] = \{.*?\n\};')]:
        match=re.search(pattern,source.read_text(),re.S);assert match;driver=driver.replace('static void snapshot(void)',match.group(0)+'\nstatic void snapshot(void)',1)
    driver+=r'''
static int C2_HOOK_CDECL test_rand(void){rng_count++;printf("%s[\"r\",%u,\"",events++?",":"",h[10]);snapshot();printf("\"]");if(rng_count==1){if(h[24]&1){unsigned bits=0x3e923456;memcpy(&gSmoke_column[h[1]].pos.v[0],&bits,4);}if(h[24]&2)gSmoke_column[h[1]].colour=0;if(h[24]&4)gSmoke_column[h[1]].lifetime=3999;if(h[24]&8)gPHIL_mechanics_time_sync=5678;if(h[24]&16)gSmoke_column[h[1]].core.collision_info=NULL;}return h[10];}
#define rand test_rand
'''
    driver+='\n\n'.join(compiled_bodies)
    driver+='\n#undef rand\n'
    # Preserve the original instruction bytes; relocate only original absolute
    # data operands and relative calls. Internal branches remain unmodified.
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    from capstone.x86 import X86_OP_MEM
    def raw_at(address,size):
        pe=struct.unpack_from('<I',original,0x3c)[0];table=pe+24+struct.unpack_from('<H',original,pe+20)[0]
        for i in range(struct.unpack_from('<H',original,pe+6)[0]):
            _,rva,n,raw=struct.unpack_from('<IIII',original,table+40*i+8)
            if 0x400000+rva<=address<0x400000+rva+n:return original[raw+address-0x400000-rva:raw+address-0x400000-rva+size]
        raise AssertionError(address)
    physical_code=[];patches=[];md=Cs(CS_ARCH_X86,CS_MODE_32);md.detail=True
    placement={'DoSmokeColumn':0,'MungeSmokeColumn':1024,'IsOnNonCarHalf':2048,'FlameAnimate':2304,'ARGetReplayRate':3328,'IRandomBetween':3584,'SRandomBetween':3840,'SRandomPosNeg':4096,'ftol':4352,'CreatePuffOfSmoke':4608,'BrMatrix34ApplyP':5376,'BrMatrix34ApplyV':5632,'BrVector3SetFloat':5888,'SmudgeCar':6144}
    sizes={'DoSmokeColumn':508,'MungeSmokeColumn':910,'IsOnNonCarHalf':86,'FlameAnimate':773,'ARGetReplayRate':7,'IRandomBetween':34,'SRandomBetween':38,'SRandomPosNeg':50,'ftol':39,'CreatePuffOfSmoke':473,'BrMatrix34ApplyP':116,'BrMatrix34ApplyV':107,'BrVector3SetFloat':25,'SmudgeCar':1}
    target={ADDR[n]:'(unsigned)(uintptr_t)(code+%d)'%off for n,off in placement.items()}
    target[ADDR['rand']]='(unsigned)(uintptr_t)test_rand'
    for n in ['FastScalarArcTan2Angle','DRMatrix34RotateY','BrMatrix34Scale','BrMaterialUpdate','ARStartPipingSession','AddFlameToPipingSession','AddSmokeColumnToPipingSession','AREndPipingSession','BrActorRemove']:target[ADDR[n]]='(unsigned)(uintptr_t)'+n
    data_target={ADDR[n]:'(unsigned)(uintptr_t)&'+n for n in ['gReplay_rate','gCamera_to_world','gSmoke_column','gFlame_map','gFlame_frame_size','gAction_replay_mode','gColumn_flags','gSmoke','gSmoke_flags','gSmoke_next','gSmoke_on','gCar_to_view','gPHIL_mechanics_time_sync','gProgram_state']}
    ranges=[('gSmoke_column',1240+64),('gSmoke',1100),('gCamera_to_world',48),('gFlame_frame_size',40),('gProgram_state',24272)]
    constants={}
    def mapped(address,width):
        if address in data_target:return data_target[address]
        for name,length in ranges:
            if ADDR[name]<=address<ADDR[name]+length:return '(unsigned)(uintptr_t)&%s+%d'%(name,address-ADDR[name])
        assert 0x580000<=address<0x600000,hex(address)
        key=(address,width);constants[key]=raw_at(address,width)
        return '(unsigned)(uintptr_t)physical_constant_%x_%d'%(address,width)
    driver+='\n#ifdef PHYSICAL_ORIGINAL\n'
    for name,size in sizes.items():
        raw=raw_at(ADDR[name],size);physical_code.append(dict(name=name,address=ADDR[name],body_bytes=size,sha256=hashlib.sha256(raw).hexdigest(),hex=raw.hex()))
        driver+='\nstatic const unsigned char physical_'+name+'[]={'+','.join(str(v) for v in raw)+'};\n'
        for ins in md.disasm(raw,ADDR[name]):
            off=placement[name]+ins.address-ADDR[name]
            if ins.mnemonic=='call':
                address=ins.operands[0].imm;assert address in target,(name,hex(address))
                patches.append(f'physical_patch(code+{off+ins.imm_offset}, {target[address]}-(unsigned)(uintptr_t)(code+{off+ins.size}));')
            else:
                for operand in ins.operands:
                    if operand.type==X86_OP_MEM and operand.mem.disp>=0x400000:
                        patches.append(f'physical_patch(code+{off+ins.disp_offset}, {mapped(operand.mem.disp,operand.size)});')
                    elif operand.type!=X86_OP_MEM and ins.imm_size==4 and ins.mnemonic in ['mov','cmp'] and 0x600000<=operand.imm<0x800000:
                        patches.append(f'physical_patch(code+{off+ins.imm_offset}, {mapped(operand.imm,4)});')
    for (address,width),raw in constants.items():driver+='static const unsigned char physical_constant_%x_%d[]={%s};\n'%(address,width,','.join(map(str,raw)))
    driver+=r'''
__declspec(dllimport) void* __stdcall VirtualAlloc(void*,unsigned,unsigned,unsigned);
static unsigned char*physical_base;
static void physical_patch(unsigned char*p,unsigned value){memcpy(p,&value,4);}
static void physical_init(void){unsigned char*code=(unsigned char*)VirtualAlloc(NULL,8192,0x3000,0x40);if(!code)abort();physical_base=code;
'''
    for name,off in placement.items():driver+=f'memcpy(code+{off},physical_{name},sizeof(physical_{name}));\n'
    driver+='\n'.join(patches)+'\n}\n'
    driver+=r'''
#define DoSmokeColumn ((void(C2_HOOK_FASTCALL*)(int,tU32,br_vector3*))(physical_base))
#define MungeSmokeColumn ((void(C2_HOOK_FASTCALL*)(tU32))(physical_base+1024))
#define IsOnNonCarHalf ((int(C2_HOOK_FASTCALL*)(tCar_spec*,const br_vector3*))(physical_base+2048))
#endif
'''

    driver+=r'''
static tU8*ptr(unsigned off){return off==0xffffffff?NULL:(tU8*)arena+off;}
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned i,j,v,value=0;if(argc!=2)return 1;f=fopen(argv[1],"rb");if(!f)return 2;if(fread(h,sizeof(h),1,f)!=1||fread(arena,sizeof(arena),1,f)!=1||fread(gSmoke_column,1240,1,f)!=1)return 3;fclose(f);_controlfp(_PC_53,_MCW_PC);
#ifdef PHYSICAL_ORIGINAL
    physical_init();
#endif
    gColumn_flags=h[2];gColumn_next=h[3];gSmoke_on=h[4];gUNK_0069410c=0x12345678;gAction_replay_mode=h[6];gProgram_state.racing=h[7];gProgram_state.cockpit_on=h[15];gSmoke_flags=h[17];gSmoke_next=h[20];gPHIL_mechanics_time_sync=h[9];gCar_to_view=(tCar_spec*)ptr(0x14000);memset(gSmoke,0xa5,sizeof(gSmoke));gNon_track_actor=(br_actor*)ptr(0x200);gLocal_buffer=ptr(0x7000);gMr_chunky=ptr(0x7008);gPipe_buffer_start=h[5]?ptr(0xb000):NULL;gPipe_callbacks=(tReplay_callback*)ptr(0x13000);gPipe_buffer_phys_end=ptr(0x13000);gPipe_buffer_working_end=gPipe_buffer_phys_end;gPipe_buffer_oldest=NULL;gPipe_record_ptr=ptr(0xb000);gLocal_buffer_size=8;gReentrancy_count=0;for(i=0;i<5;i++)gReentrancy_array[i]=(tPipe_chunk_type)(20+i);
    gPipe_record_ptr=ptr(h[21]?h[21]:0xb000);
    gReentrancy_count=h[23];
    for(i=0;i<71;i++){unsigned off=0x13000+24*i+8;if(rd(off)==0x1082000)wr(off,(unsigned)(uintptr_t)test_length);}
    for(i=0;i<10;i++){unsigned*core=(unsigned*)&gSmoke_column[i].core;for(j=1;j<4;j++)if(core[j])core[j]=(unsigned)(uintptr_t)arena+core[j]-0x1000000;gSmoke_column[i].flame_actor=(br_actor*)((char*)arena+(unsigned)(uintptr_t)gSmoke_column[i].flame_actor-0x1000000);}
    for(i=0;i<46;i++){for(j=0;j<5;j++){unsigned off=i*0x100+(j<4?4*j:28);v=rd(off);if(v>=0x1000000&&v<0x1030000)wr(off,(unsigned)(uintptr_t)arena+v-0x1000000);}}
    for(j=1;j<4;j++){unsigned off=0xc000+4*j;v=rd(off);if(v)wr(off,(unsigned)(uintptr_t)arena+v-0x1000000);}
    {unsigned offsets[13]={0x14008,0x14010,0x14e10,0x158d4,0x16000,0x16020,0x16258,0x318,0x22018,0x2304c,0x24018,0x25010,0x28000};for(i=0;i<13;i++){v=rd(offsets[i]);if(v>=0x1000000&&v<0x1030000)wr(offsets[i],(unsigned)(uintptr_t)arena+v-0x1000000);}}
    for(i=0;i<46;i++){unsigned off=0x17000+256*i+64;v=rd(off);if(v)wr(off,(unsigned)(uintptr_t)arena+v-0x1000000);}
    for(i=0;i<20;i++)gFlame_map[i]=(br_pixelmap*)ptr(0x1a000+256*i);
    memset(&gCamera_to_world,0,sizeof(gCamera_to_world));gCamera_to_world.m[0][0]=1.f;gCamera_to_world.m[1][1]=1.f;memcpy(&gCamera_to_world.m[2][0],h+11,4);memcpy(&gCamera_to_world.m[2][2],h+12,4);memcpy(&gReplay_rate,h+14,4);
    printf("{\"calls\":[");
    if(h[0]==0)MungeSmokeColumn(h[13]);else if(h[0]==1)DoSmokeColumn(h[1],h[13],(br_vector3*)ptr(h[18]));else value=IsOnNonCarHalf((tCar_spec*)ptr(0x14000),(br_vector3*)ptr(h[18]));
    printf("],\"state\":\"");snapshot();printf("\",\"value\":");(void)value;if(h[0]==2)printf("%u",value);else printf("null");printf("}\n");return 0;
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
        'mingw':[str(args.gcc.resolve()),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-fno-strict-aliasing',*['-I'+str(r) for r in includes],str(harness),'-o',str(out/'mingw.exe')]}
    commands['physical-original']=commands['msvc5'][:-2]+['/DPHYSICAL_ORIGINAL','/Fo'+str(out/'physical-original.obj'),'/Fe'+str(out/'physical-original.exe'),'/link','kernel32.lib']
    for compiler,cmd in commands.items():
        build=subprocess.run(cmd,cwd=out,env=env if compiler=='msvc' else oldenv if compiler in ['msvc5','physical-original'] else None,text=True,capture_output=True)
        (out/(compiler+'-build.log')).write_text(build.stdout+build.stderr);assert build.returncode==0,build.stdout+build.stderr
    cases=[]
    for mode in [0,1]:
        for core in [0,1,2]:
            for upright in [0,0x3dcccccc,0x3dcccccd,0x3dccccce,0xbdcccccc,0xbdcccccd,0xbdccccce,0x7fc12345,0x7f800001]:cases.append(dict(mode=mode,core=core,orientation=upright))
    for scalar in [0,0x80000000,1,0x3f123456,0xbf123456,0x7f800000,0xff800000,0x7fc12345,0xffc12345,0x7f800001,0xff801234]:
        for core in [1,2]:cases.append(dict(mode=1,scalar=scalar,core=core))
    for cockpit in [0,1]:
        for driver in [7,8]:
            for threshold in [0xbf800000,0,0x3f800000,0x7fc12345,0x7f800001]:cases.append(dict(mode=1,cockpit=cockpit,driver=driver,driver_z=threshold))
    for alias in [0x26000,0x26004,0x22050,0x22054]:
        for core in [1,2]:cases.append(dict(mode=1,alias=alias,core=core))
    for normal in [0,0x80000000,0x3f800000,0xbf800000,0x7f800000,0x7fc12345,0x7f800001]:
        for distance in [0,0x3f800000,0xbf800000]:cases.append(dict(mode=2,normal=normal,distance=distance))
    cases += [dict(mode=2,crush=False),dict(mode=2,crush_flag=0),dict(mode=2,crush_flag=0x80)]
    for replay in [0,1]:
        for flags in [0,1,512,513,0x80000000]:cases.append(dict(replay=replay,flags=flags))
        for colour in [0,1,2,-1]:cases.append(dict(replay=replay,colour=colour,frame=1))
    for time in [0,1,200,201,0x80000000,0xffffffff]:
        for life in [0,1,200,201,3999,4000,0x80000000,0xffffffff]:cases.append(dict(time=time,lifetime=life))
    for elapsed in [0,199,200,201,0x7fffffff,0xfffffffe,0xffffffff]:cases.append(dict(elapsed=elapsed,time=1))
    for white in [0,1,2,3,0xffffffff,0x80000000]:
        for rand in [0,1,16384,32767]:cases.append(dict(whiter=white,rand=rand,time=201))
    for velocity in [0x3f123456,0xbf123457,1,0x80000001,0x405cccce,0x7f800000,0xff800000,0x7fc12345,0x7f800001]:cases.append(dict(velocity=velocity,time=201))
    for physics in [False,True]:
        for colour in [0,2]:cases.append(dict(physics=physics,colour=colour,time=201))
    for timer in [0,1,2,0x80000000,0xffffffff]:cases.append(dict(colour=0,timer=timer,knackered=1,time=1,frame=1))
    for gate in range(8):cases.append(dict(gate=gate,lifetime=0,time=1,colour=0,callback=True))
    for reentrancy in [1,2]:cases.append(dict(gate=5,lifetime=0,time=1,callback=True,reentrancy=reentrancy))
    cases.append(dict(gate=5,lifetime=0,time=1,callback=True,record=0x12ffe))
    for mutation in [1,2,4,8,16,32,64,7,63]:cases.append(dict(colour=0,time=201,mutation=mutation,frame=1))
    for scalar in [0x7fc12345,0x7f800001,0xff801234]:cases.append(dict(mode=1,cockpit=1,driver=8,scalar=scalar))
    for normal_vector in [[0x3f800000,0x5b000000,0xdb000000],[0x3f800000,0xdb000000,0x5b000000],[0xbf800000,0x5b000000,0xdb000000]]:cases.append(dict(mode=2,normal_vector=normal_vector,scalar=0x3f800000,distance=0x3f000000))
    for mutation in [256,512,1024,2048,4096]:cases.append(dict(colour=0,time=201,mutation=mutation,frame=1,knackered=1,flags=513))
    for omega in [0x7f800001,0xff801234,0x7fc12345]:cases.append(dict(omega=omega,time=201))
    for mutation in [1,2,4,7]:cases.append(dict(gate=5,lifetime=0,time=1,callback=True,callback_mutate=mutation))
    for mutation in [1,2,4,8,16,31]:cases.append(dict(time=201,rng_mutate=mutation))
    for lifetime in [4200,4201,4202]:cases.append(dict(time=201,lifetime=lifetime))
    for replay in [0,1]:
        for frame in [-1,19]:cases.append(dict(replay=replay,frame=frame,colour=0,time=201,gate=5,callback=True))
    cases += [dict(time=201,smoke_on=0),dict(time=201,next=0,count=0xffffffff)]
    results=[];unicorn_differences=[]
    for index,case in enumerate(cases):
        h=[0]*32;h[0]=case.get('mode',0);h[1]=case.get('slot',0);h[2]=case.get('flags',1<<h[1]);h[3]=0;h[4]=case.get('smoke_on',1);h[5]=case.get('gate',0)&1;h[6]=case.get('replay',0);h[7]=(case.get('gate',0)>>2)&1;h[8]=case.get('frame',1)&0xffffffff;h[9]=0x12345678;h[10]=case.get('rand',16384);h[11]=0x3f800000;h[12]=0x3f800000;h[13]=case.get('time',1);h[14]=case.get('rate',0x3f800000);h[15]=case.get('cockpit',0);h[16]=case.get('driver',7);h[17]=0x80000000;h[18]=case.get('alias',0x1e000);h[19]=case.get('mutation',0);h[20]=case.get('next',24);h[21]=case.get('record',0);h[22]=case.get('callback_mutate',0);h[23]=case.get('reentrancy',0);h[24]=case.get('rng_mutate',0)
        arena=bytearray(b'\xa5'*SIZE);columns=bytearray(b'\xa5'*1240)
        def put(buf,off,v):struct.pack_into('<I',buf,off,v&0xffffffff)
        def fput(buf,off,v):struct.pack_into('<f',buf,off,v)
        def matrix(off):
            struct.pack_into('<12f',arena,off,1,0,0,0,1,0,0,0,1,0,0,0)
        for i in range(46):arena[256*i:256*i+100]=bytes(100);matrix(256*i+44)
        put(arena,0x408,HEAP+0x500);struct.pack_into('<4I',arena,0xc000,2,HEAP+0x400,HEAP+0x500,HEAP+0x300)
        arena[0x14000:0x17000]=bytes(0x3000)
        put(arena,0x14008,HEAP+0x16000);put(arena,0x1400c,h[16]);put(arena,0x14010,HEAP+0x400);put(arena,0x141d4,case.get('knackered',0));put(arena,0x14e10,HEAP+0x22000);put(arena,0x158d4,HEAP+0x20000 if case.get('crush',True) else 0);put(arena,0x14484,case.get('driver_z',0));put(arena,0x16000,HEAP+0x500);put(arena,0x16020,HEAP+0x21000)
        arena[0x20000:0x22800]=bytes(0x2800);arena[0x204b8]=case.get('crush_flag',0 if h[0]!=2 else 1);put(arena,0x20548,case.get('normal',0x3f800000));put(arena,0x2054c,0);put(arena,0x20550,0);put(arena,0x20554,case.get('distance',0));fput(arena,0x21008,-4.5)
        matrix(0x22000+44);fput(arena,0x22000+80,.11);fput(arena,0x22000+84,-.12);fput(arena,0x22000+88,.13)
        put(arena,0x400+44+16,case.get('orientation',0x3f800000))
        put(arena,0x300+24,HEAP+0x23000);put(arena,0x22000+24,HEAP+0x23000);put(arena,0x23000+76,HEAP+0x24000);put(arena,0x24000+24,HEAP+0x25000);put(arena,0x25000+16,HEAP+0x26000)
        for v in range(3):
            for j in range(3):put(arena,0x26000+32*v+4*j,case.get('scalar',0x3f123456+0x1000*v+0x100*j))
        for i in range(12):struct.pack_into('<H',arena,0x140ac+2*i,(0x8000+73*i)&0xffff)
        put(arena,0x16258,HEAP+0x16800);fput(arena,0x16888,1)
        for i in range(10):
            struct.pack_into('<4I',columns,124*i,case.get('core',1),HEAP+0x14000,HEAP+0x16000 if case.get('physics',True) else 0,HEAP+0x300)
            for off,val in [(16,case.get('elapsed',0)),(20,case.get('lifetime',10000)),(24,case.get('count',0)),(28,case.get('colour',2)),(32,case.get('whiter',0)),(112,case.get('timer',0)),(116,case.get('vertex',0)),(120,case.get('upright',1))]:put(columns,124*i+off,val)
            put(columns,124*i+36,HEAP+0x600+0x400*i);put(arena,0x600+0x400*i+8,HEAP+0x700+0x400*i)
            for j in range(3):put(arena,0x700+0x400*i+256*j,HEAP+0x800+0x400*i+256*j if j<2 else 0)
            for j in range(3):
                put(columns,124*i+40+4*j,h[8])
                for off in [64,76,88,100]:put(columns,124*i+off+4*j,0x3c012345+0x1000*i+0x100*j+off)
        for i in range(46):put(arena,256*i+28,HEAP+0x17000+256*i)
        for i in range(46):arena[0x17000+256*i:0x17000+256*i+256]=bytes(256);put(arena,0x17000+256*i+64,HEAP+0x1a000)
        for i in range(20):
            arena[0x1a000+256*i:0x1a000+256*i+256]=bytes(256)
            struct.pack_into('<HH',arena,0x1a000+256*i+52,20+i,30+i)
        put(arena,0x1c00,HEAP+0x1d00);put(arena,0x1d00,0)
        for i in range(3):put(arena,0x16068+4*i,case.get('velocity',0x3f123456+0x100*i));put(arena,0x16074+4*i,case.get('omega',0x3e123456+0x100*i));put(arena,0x1e000+4*i,case.get('scalar',0x3f123456+0x100*i))
        if 'normal_vector' in case:struct.pack_into('<3I',arena,0x20548,*case['normal_vector'])
        arena[0x28000:0x28800]=bytes(0x800);put(arena,0x28000,HEAP+0x500)
        pattern=case.get('pattern',0)
        if pattern:
            put(columns,28,2);put(columns,124*2+28,0 if pattern in [1,2] else 2)
            put(columns,124*2+20,0 if pattern in [2,4] else 1)
        put(arena,0x7000,17);put(arena,0x7004,0)
        for off in range(0xb000,0x13000-34,34):put(arena,off,17);put(arena,off+4,1);struct.pack_into('<H',arena,off+32,32)
        for i in range(71):arena[0x13000+24*i:0x13000+24*i+24]=bytes(24);put(arena,0x13000+24*i+4,20)
        if case.get('callback'):
            for kind in [17,18]:put(arena,0x13000+24*kind+8,0x1082000)
        fixture=out/'fixture.bin';fixture.write_bytes(struct.pack('<32I',*h)+arena+columns)
        physical=json.loads(subprocess.check_output([str(out/'physical-original.exe'),str(fixture)],text=True))
        expected=execute(*images[0],h,arena,columns);checks=[('physical-original',physical)]
        if len(images)>1:checks.append(('recompiled-x86',execute(*images[1],h,arena,columns)))
        for compiler in ['msvc5','msvc','mingw']:checks.append((compiler,json.loads(subprocess.check_output([str(out/(compiler+'.exe')),str(fixture)],text=True))))
        for compiler,actual in checks:
            if actual!=expected:
                failure=out/f'failure-{compiler}-{index}.json';failure.write_text(json.dumps(dict(case=case,original=expected,actual=actual),indent=2))
                raise AssertionError(f'{compiler} column case{index} differs: {failure}')
            results.append(dict(case=case,compiler=compiler,passed=True,state_sha256=hashlib.sha256(bytes.fromhex(actual['state'])).hexdigest()))
    physical_evidence=dict(original_sha256=ORIGINAL_HASH,precision_control=53,scope='Exact original smoke updater, predicate, flame, particle, matrix, vector-set, rate, random and FTOL bytes execute physically. Only original absolute global/constant operands and relative dependency calls are relocated. Original internal branches unchanged; published trig and replay dependencies execute actual production C bodies. CRT rand/BRender callbacks controlled.',bodies=physical_code,patches=patches,compiler_command=commands['physical-original'],executable_sha256=hashlib.sha256((out/'physical-original.exe').read_bytes()).hexdigest())
    (out/'physical-original.json').write_text(json.dumps(physical_evidence,indent=2)+'\n')
    verification=dict(scenario_count=len(cases),comparisons=len(results),original_sha256=ORIGINAL_HASH,source_sha256=hashlib.sha256(SOURCE.read_bytes()).hexdigest(),piping_source_sha256=hashlib.sha256(PIPE.read_bytes()).hexdigest(),utility_source_sha256=hashlib.sha256(UTILITY.read_bytes()).hexdigest(),bodies_sha256=[hashlib.sha256(b.encode()).hexdigest() for b in bodies],compiler_commands=commands,cases=results,physical_original=physical_evidence,padding_exclusions={'type17':[18,19],'type18':[2,3]},oracle_correction=None,limitations=['PC53 only; sticky FPU exception-status flags are outside observed game state.','Accessible valid column0..9, first prepared group/vertices0..2, valid car/crush/shape graph for active type1, three child actors, maps0..19 after callbacks; valid position vector with bounded aliases, car/physics inputs and controlled CRT rand0..32767.','Physical original updater/predicate/flame/particle/matrix/vector/rate/random/FTOL bytes use actual published trig/replay source dependency bodies; complete original/matching PE closures are independently executed in Unicorn with strict raw state equality.','Replay callbacks use original fixed20-byte payloads; type17 offsets18..19 and type18 offsets2..3 are unwritten padding, with only these bytes and their tracked propagated copies excluded.'],new_original_entries=['DoSmokeColumn4fc0e0','MungeSmokeColumn4fba40','IsOnNonCarHalf42df70'])
    if args.symbols:verification.update(recompiled_sha256=hashlib.sha256(args.recompiled.read_bytes()).hexdigest(),symbols_sha256=hashlib.sha256(args.symbols.read_bytes()).hexdigest())
    verification.update(crush_source_sha256=hashlib.sha256(CRUSH.read_bytes()).hexdigest(),vector_source_sha256=hashlib.sha256(VECTOR.read_bytes()).hexdigest(),header_sha256=hashlib.sha256((SOURCE.parent/'32-spark.h').read_bytes()).hexdigest(),verifier_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),unicorn_difference_count=len(unicorn_differences),trig_source_sha256=hashlib.sha256(TRIG.read_bytes()).hexdigest(),matrix_source_sha256=hashlib.sha256(MATRIX.read_bytes()).hexdigest())
    (out/'unicorn-differences.json').write_text(json.dumps(unicorn_differences,indent=2)+'\n')
    (out/'verification.json').write_text(json.dumps(verification,indent=2)+'\n')
    print(f'PASS: {len(cases)} scenarios, {len(results)} comparisons.')


if __name__=='__main__':main()
