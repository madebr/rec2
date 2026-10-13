#!/usr/bin/env python3
"""Verify original smoke wrappers with real constructor/replay/random bodies.

Physical x86 executes the original wrappers, empty SmudgeCar and IRandomBetween
bytes. Its constructor and replay dependency closure is compiled from actual
production bodies. CRT rand and BRender callbacks are controlled boundaries.
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
ORIGINAL_HASH='9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
ADDR=dict(CreateSmokeColumn=0x4fb890,ConditionalSmokeColumn=0x4fcab0,SmudgeCar=0x4fc9c0,IRandomBetween=0x513520,rand=0x576970,AdjustSmokeColumn=0x4fb580,CreateSmokeColumn2=0x4fb630,AdjustFlame=0x4fba00,
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
PTR=['gPipe_buffer_start','gLocal_buffer','gMr_chunky','gPipe_callbacks','gPipe_buffer_phys_end',
    'gPipe_buffer_working_end','gPipe_buffer_oldest','gPipe_record_ptr','gNon_track_actor']
VAL=['gColumn_flags','gColumn_next','gSmoke_on','gUNK_0069410c','gLocal_buffer_size','gReentrancy_count','gAction_replay_mode']
HEAP,STACK,STOP,SIZE=0x1000000,0x1080000,0x1081000,0x20000


def main():
    p=argparse.ArgumentParser(description=__doc__)
    for n,d in [('original',ROOT/'build-reference/originals/CARMA2_HW.EXE'),('recompiled',ROOT/'build-matching/rec2.exe'),
        ('output',ROOT/'build-audit/smoke-wrapper-tests'),('dependency-dir',ROOT/'build-audit/joystick/python-deps'),
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
        wr(a['gProgram_state']+0x28,h[7]);calls=[];rng_count=[0];padding=bytearray(SIZE);pointer_fields=bytearray(SIZE);pending=[];callback_count=[0]
        wr(a['gPipe_record_ptr'],HEAP+(h[21] or 0xb000))
        def snapshot():
            copy=bytearray(u.mem_read(HEAP,SIZE))
            for i in range(SIZE):
                if padding[i]:copy[i]=0
            return (b''.join(struct.pack('<I',rd(a[n])) for n in PTR+VAL)+bytes(u.mem_read(a['gReentrancy_array'],20))+bytes(u.mem_read(a['gSmoke_column'],1240))+copy+padding+pointer_fields+struct.pack('<I',rng_count[0])).hex()
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
        def hook(uc,at,size,context):
            if at==a['rand']:
                rng_count[0]+=1;calls.append(['r',h[10],snapshot()]);sp=uc.reg_read(UC_X86_REG_ESP)
                uc.reg_write(UC_X86_REG_EAX,h[10]);uc.reg_write(UC_X86_REG_EIP,rd(sp));uc.reg_write(UC_X86_REG_ESP,sp+4);return
            if at==a['ARAddVariedDataToSession']:
                observe_add(rd(uc.reg_read(UC_X86_REG_ESP)+4),20);return
            if at==a['EndPipingSession2']:observe_end();return
            if at==0x1082000:
                packet=uc.reg_read(UC_X86_REG_ECX);kind=rd(rd(a['gLocal_buffer']));copy=bytearray(u.mem_read(packet,24));off=22 if kind==17 else 6;copy[off:off+2]=bytes(2)
                calls.append(['c',kind,copy.hex(),snapshot()])
                if callback_count[0]==0 and h[22]:wr(a['gColumn_next'],h[22]-1)
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
        if h[0]==0:invoke('CreateSmokeColumn',HEAP+h[18],h[9],[h[8],h[13]])
        elif h[0]==1:invoke('ConditionalSmokeColumn',HEAP+h[18] if h[18] else 0,h[16],[h[9]])
        elif h[0]==2:invoke('SmudgeCar',h[18],h[8])
        return dict(calls=calls,state=snapshot(),value=value)
    pipe_names=['ReplayCopyForward','LengthOfSession','ARStartPipingSession','EndPipingSession2','AREndPipingSession','ARAddDataToSession','ARAddVariedDataToSession','AddSmokeColumnToPipingSession','AddFlameToPipingSession']
    names=['CreateSmokeColumn2','SmudgeCar','CreateSmokeColumn','ConditionalSmokeColumn']
    bodies=[extract(UTILITY.read_text(),'IRandomBetween')]+[extract(PIPE.read_text(),n) for n in pipe_names]+[extract(SOURCE.read_text(),n) for n in names]
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
#include "23-piping.h"
#include "52-errors.h"
#include "41-utility.h"
#include "globvars.h"
#include "rec2_macros.h"
typedef char layouts[sizeof(tSmoke_column)==124&&sizeof(tSmoke_column_core)==16&&sizeof(br_actor)==100&&sizeof(tCar_spec)<=0x2000&&sizeof(tPhysics_object)<=0x800&&sizeof(tSpecial_volume)<=0x800&&offsetof(tCar_spec,collision_info)==8&&offsetof(tCar_spec,driver)==12&&offsetof(tCar_spec,car_master_actor)==16&&offsetof(tCar_spec,fire_vertex)==0xac&&offsetof(tCar_spec,knackered)==0x1d4&&offsetof(tPhysics_object,last_special_volume)==0x258&&offsetof(tSpecial_volume,gravity_multiplier)==0x88?1:-1];
typedef void (C2_HOOK_FASTCALL* raw_flame_fn)(int,int,tU32,tU32,tU32,tU32);
tU8*gPipe_buffer_start,*gLocal_buffer,*gMr_chunky,*gPipe_buffer_phys_end,*gPipe_buffer_working_end,*gPipe_buffer_oldest,*gPipe_record_ptr;
const tReplay_callback*gPipe_callbacks;tU32 gLocal_buffer_size,gUNK_0069410c;int gReentrancy_count;tPipe_chunk_type gReentrancy_array[5];
int gAction_replay_mode,gColumn_flags,gColumn_next,gSmoke_on;tProgram_state gProgram_state;tSmoke_column gSmoke_column[10];br_actor*gNon_track_actor;
static unsigned arena[0x20000/4],h[24];static int events,callback_count;static unsigned rng_count;static unsigned char padding[0x20000],pointer_fields[0x20000];static unsigned pending_dest,pending_kind,pending_size,pending;
static int C2_HOOK_FASTCALL test_length(tPipe_chunk*packet);
static unsigned norm(unsigned v){unsigned b=(unsigned)(uintptr_t)arena;if(v>=b&&v<=b+sizeof(arena))return v-b+0x1000000;if(v==(unsigned)(uintptr_t)test_length)return 0x1082000;return v;}
static unsigned rd(unsigned off){unsigned v;memcpy(&v,(char*)arena+off,4);return v;}
static void wr(unsigned off,unsigned v){memcpy((char*)arena+off,&v,4);}
static void hex(const void*p,unsigned n){unsigned i;for(i=0;i<n;i++)printf("%02x",((const unsigned char*)p)[i]);}
static void commit_pending(void){unsigned off;if(!pending)return;pending=0;memset(padding+pending_dest,0,pending_size+4);memset(pointer_fields+pending_dest,0,pending_size+4);if(pending_kind==17||pending_kind==18){off=pending_kind==17?18:2;padding[pending_dest+4+off]=padding[pending_dest+5+off]=1;if(pending_kind==17){for(off=4;off<=12;off+=4)pointer_fields[pending_dest+4+off]=1;}}}
static void observe_add(int kind,unsigned size){commit_pending();if(!gPipe_buffer_start||gAction_replay_mode||!gProgram_state.racing||gLocal_buffer_size+size+4>=15000)return;pending=1;pending_dest=(unsigned)(gMr_chunky-(tU8*)arena);pending_kind=kind;pending_size=size;}
static void observe_end(void){unsigned local,size,dest;commit_pending();if(!gPipe_buffer_start||gAction_replay_mode||!gProgram_state.racing)return;local=(unsigned)(gLocal_buffer-(tU8*)arena);size=gLocal_buffer_size;memset(padding+local+size,0,2);memset(pointer_fields+local+size,0,2);size+=2;if(rd(local+4)&&size<=15000){dest=(unsigned)(gPipe_record_ptr-(tU8*)arena);if(gPipe_record_ptr+size>gPipe_buffer_phys_end)dest=(unsigned)(gPipe_buffer_start-(tU8*)arena);memcpy(padding+dest,padding+local,size);memcpy(pointer_fields+dest,pointer_fields+local,size);}}
static void snapshot(void){unsigned prefix[16],i,j,v;unsigned char copy[sizeof(arena)];tSmoke_column columns[10];
    prefix[0]=norm((unsigned)(uintptr_t)gPipe_buffer_start);prefix[1]=norm((unsigned)(uintptr_t)gLocal_buffer);prefix[2]=norm((unsigned)(uintptr_t)gMr_chunky);prefix[3]=norm((unsigned)(uintptr_t)gPipe_callbacks);prefix[4]=norm((unsigned)(uintptr_t)gPipe_buffer_phys_end);prefix[5]=norm((unsigned)(uintptr_t)gPipe_buffer_working_end);prefix[6]=norm((unsigned)(uintptr_t)gPipe_buffer_oldest);prefix[7]=norm((unsigned)(uintptr_t)gPipe_record_ptr);prefix[8]=norm((unsigned)(uintptr_t)gNon_track_actor);prefix[9]=gColumn_flags;prefix[10]=gColumn_next;prefix[11]=gSmoke_on;prefix[12]=gUNK_0069410c;prefix[13]=gLocal_buffer_size;prefix[14]=gReentrancy_count;prefix[15]=gAction_replay_mode;
    memcpy(columns,gSmoke_column,sizeof(columns));for(i=0;i<10;i++){columns[i].core.field_0x0=norm(columns[i].core.field_0x0);columns[i].core.car=(tCar_spec*)(uintptr_t)norm((unsigned)(uintptr_t)columns[i].core.car);columns[i].core.collision_info=(tPhysics_object*)(uintptr_t)norm((unsigned)(uintptr_t)columns[i].core.collision_info);columns[i].core.car_actor=(br_actor*)(uintptr_t)norm((unsigned)(uintptr_t)columns[i].core.car_actor);columns[i].flame_actor=(br_actor*)(uintptr_t)norm((unsigned)(uintptr_t)columns[i].flame_actor);}
    memcpy(copy,arena,sizeof(copy));for(i=0;i<45;i++)for(j=0;j<4;j++){unsigned off=i*0x100+4*j;v=norm(rd(off));memcpy(copy+off,&v,4);}for(j=1;j<4;j++){v=norm(rd(0xc000+4*j));memcpy(copy+0xc000+4*j,&v,4);}
    {unsigned offsets[3]={0x14008,0x14010,0x16258};for(i=0;i<3;i++){v=norm(rd(offsets[i]));memcpy(copy+offsets[i],&v,4);}}
    for(i=0;i<71;i++){v=norm(rd(0x13000+24*i+8));memcpy(copy+0x13000+24*i+8,&v,4);}
    for(i=0;i<sizeof(copy);i++){if(pointer_fields[i]){v=norm(rd(i));memcpy(copy+i,&v,4);}}
    for(i=0;i<sizeof(copy);i++){if(padding[i])copy[i]=0;}
    hex(prefix,sizeof(prefix));hex(gReentrancy_array,20);hex(columns,sizeof(columns));hex(copy,sizeof(copy));hex(padding,sizeof(padding));hex(pointer_fields,sizeof(pointer_fields));hex(&rng_count,4);
}
static int C2_HOOK_FASTCALL test_length(tPipe_chunk*packet){unsigned kind=rd((unsigned)(gLocal_buffer-(tU8*)arena)),off=kind==17?22:6,i,v;unsigned char copy[24];memcpy(copy,packet,24);memset(copy+off,0,2);if(kind==17){for(i=8;i<=16;i+=4){memcpy(&v,copy+i,4);v=norm(v);memcpy(copy+i,&v,4);}}printf("%s[\"c\",%u,\"",events++?",":"",kind);hex(copy,24);printf("\",\"");snapshot();printf("\"]");if(callback_count++==0&&h[22])gColumn_next=h[22]-1;return 0;}
br_actor*C2_HOOK_CDECL BrActorAdd(br_actor*parent,br_actor*actor){printf("%s[\"a\",%u,%u,\"",events++?",":"",norm((unsigned)(uintptr_t)parent),norm((unsigned)(uintptr_t)actor));snapshot();printf("\"]");actor->parent=parent;if(h[19])gColumn_next=h[19]-1;return actor;}
br_actor*C2_HOOK_CDECL BrActorRemove(br_actor*actor){printf("%s[\"d\",%u,\"",events++?",":"",norm((unsigned)(uintptr_t)actor));snapshot();printf("\"]");actor->parent=NULL;if(h[19])gColumn_next=h[19]-1;return actor;}
void C2_NORETURN C2_HOOK_CDECL FatalError(int index,...){(void)index;abort();}
'''
    driver+=r'''
static int C2_HOOK_CDECL test_rand(void){rng_count++;printf("%s[\"r\",%u,\"",events++?",":"",h[10]);snapshot();printf("\"]");return h[10];}
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
    placement={'CreateSmokeColumn':0,'ConditionalSmokeColumn':256,'SmudgeCar':512,'IRandomBetween':768}
    sizes={'CreateSmokeColumn':120,'ConditionalSmokeColumn':244,'SmudgeCar':1,'IRandomBetween':34}
    target={0x4fb630:'(unsigned)(uintptr_t)CreateSmokeColumn2',0x4fc9c0:'(unsigned)(uintptr_t)(code+512)',0x513520:'(unsigned)(uintptr_t)(code+768)',0x576970:'(unsigned)(uintptr_t)test_rand'}
    data_target={0x6aa59c:'(unsigned)(uintptr_t)&gColumn_flags',0x6a96a8:'(unsigned)(uintptr_t)gSmoke_column',0x6a96ac:'(unsigned)(uintptr_t)gSmoke_column+4',0x6a9b84:'(unsigned)(uintptr_t)gSmoke_column+1244',0x6a96bc:'(unsigned)(uintptr_t)gSmoke_column+20',0x6a9b94:'(unsigned)(uintptr_t)gSmoke_column+1260',0x58afdc:'(unsigned)(uintptr_t)&physical_one'}
    driver+='\n#ifdef PHYSICAL_ORIGINAL\n'
    for name,size in sizes.items():
        raw=raw_at(ADDR[name],size);physical_code.append(dict(name=name,address=ADDR[name],body_bytes=size,sha256=hashlib.sha256(raw).hexdigest(),hex=raw.hex()))
        driver+='\nstatic const unsigned char physical_'+name+'[]={'+','.join(str(v) for v in raw)+'};\n'
        for ins in md.disasm(raw,ADDR[name]):
            off=placement[name]+ins.address-ADDR[name]
            if ins.mnemonic=='call':
                address=ins.operands[0].imm;assert address in target
                patches.append(f'physical_patch(code+{off+ins.imm_offset}, {target[address]}-(unsigned)(uintptr_t)(code+{off+ins.size}));')
            else:
                for operand in ins.operands:
                    if operand.type==X86_OP_MEM and not operand.mem.base and not operand.mem.index and operand.mem.disp>=0x400000:
                        address=operand.mem.disp;assert address in data_target
                        patches.append(f'physical_patch(code+{off+ins.disp_offset}, {data_target[address]});')
                    elif operand.type!=X86_OP_MEM and ins.imm_size==4 and ins.mnemonic in ['mov','cmp'] and 0x600000<=operand.imm<0x800000:
                        address=operand.imm;assert address in data_target
                        patches.append(f'physical_patch(code+{off+ins.imm_offset}, {data_target[address]});')
    driver+=r'''
__declspec(dllimport) void* __stdcall VirtualAlloc(void*,unsigned,unsigned,unsigned);
static const float physical_one=1.0f;
static unsigned char*physical_base;
static void physical_patch(unsigned char*p,unsigned value){memcpy(p,&value,4);}
static void physical_init(void){unsigned char*code=(unsigned char*)VirtualAlloc(NULL,4096,0x3000,0x40);if(!code)abort();physical_base=code;
'''
    for name,off in placement.items():driver+=f'memcpy(code+{off},physical_{name},sizeof(physical_{name}));\n'
    driver+='\n'.join(patches)+'\n}\n'
    driver+=r'''
#define CreateSmokeColumn ((void(C2_HOOK_FASTCALL*)(tCar_spec*,int,int,tU32))(physical_base))
#define ConditionalSmokeColumn ((void(C2_HOOK_FASTCALL*)(tCar_spec*,int,int))(physical_base+256))
#define SmudgeCar ((void(C2_HOOK_FASTCALL*)(tCar_spec*,int))(physical_base+512))
#endif
'''

    driver+=r'''
static tU8*ptr(unsigned off){return off==0xffffffff?NULL:(tU8*)arena+off;}
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned i,j,v,value=0;if(argc!=2)return 1;f=fopen(argv[1],"rb");if(!f)return 2;if(fread(h,sizeof(h),1,f)!=1||fread(arena,sizeof(arena),1,f)!=1||fread(gSmoke_column,1240,1,f)!=1)return 3;fclose(f);_controlfp(_PC_53,_MCW_PC);
#ifdef PHYSICAL_ORIGINAL
    physical_init();
#endif
    gColumn_flags=h[2];gColumn_next=h[3];gSmoke_on=h[4];gUNK_0069410c=0x12345678;gAction_replay_mode=h[6];gProgram_state.racing=h[7];gNon_track_actor=(br_actor*)ptr(0x200);gLocal_buffer=ptr(0x7000);gMr_chunky=ptr(0x7008);gPipe_buffer_start=h[5]?ptr(0xb000):NULL;gPipe_callbacks=(tReplay_callback*)ptr(0x13000);gPipe_buffer_phys_end=ptr(0x13000);gPipe_buffer_working_end=gPipe_buffer_phys_end;gPipe_buffer_oldest=NULL;gPipe_record_ptr=ptr(0xb000);gLocal_buffer_size=8;gReentrancy_count=0;for(i=0;i<5;i++)gReentrancy_array[i]=(tPipe_chunk_type)(20+i);
    gPipe_record_ptr=ptr(h[21]?h[21]:0xb000);
    gReentrancy_count=h[23];
    for(i=0;i<71;i++){unsigned off=0x13000+24*i+8;if(rd(off)==0x1082000)wr(off,(unsigned)(uintptr_t)test_length);}
    for(i=0;i<10;i++){unsigned*core=(unsigned*)&gSmoke_column[i].core;for(j=1;j<4;j++)if(core[j])core[j]=(unsigned)(uintptr_t)arena+core[j]-0x1000000;gSmoke_column[i].flame_actor=(br_actor*)((char*)arena+(unsigned)(uintptr_t)gSmoke_column[i].flame_actor-0x1000000);}
    for(i=0;i<45;i++){for(j=0;j<4;j++){unsigned off=i*0x100+4*j;v=rd(off);if(v>=0x1000000&&v<0x1014000)wr(off,(unsigned)(uintptr_t)arena+v-0x1000000);}}
    for(j=1;j<4;j++){unsigned off=0xc000+4*j;v=rd(off);if(v)wr(off,(unsigned)(uintptr_t)arena+v-0x1000000);}
    {unsigned offsets[3]={0x14008,0x14010,0x16258};for(i=0;i<3;i++){v=rd(offsets[i]);if(v>=0x1000000&&v<0x1020000)wr(offsets[i],(unsigned)(uintptr_t)arena+v-0x1000000);}}
    printf("{\"calls\":[");
    if(h[0]==0)CreateSmokeColumn((tCar_spec*)ptr(h[18]),h[9],h[8],h[13]);
    else if(h[0]==1)ConditionalSmokeColumn((tCar_spec*)ptr(h[18]?h[18]:0xffffffff),h[16],h[9]);
    else if(h[0]==2)SmudgeCar((tCar_spec*)(uintptr_t)h[18],h[8]);
    printf("],\"state\":\"");snapshot();printf("\",\"value\":");(void)value;printf("null");printf("}\n");return 0;
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
        for gravity in [0,0x80000000,0x3f7fffff,0x3f800000,0x3f800001,0x7f800000,0xff800000,0x7fc12345,0xffc12345,0x7f800001,0xff801234]:
            cases.append(dict(mode=mode,gravity=gravity,flags=0))
        for smoke_on in [0,1]:
            for knackered in [0,1,0x80000000]:
                for rand in [0,1,16383,16384,32766,32767]:cases.append(dict(mode=mode,smoke_on=smoke_on,knackered=knackered,rand=rand,flags=0,special=False))
        for slot in [0,9]:
            for colour in [0,1,0x80000000,0x7fffffff]:cases.append(dict(mode=mode,index=slot,colour=colour,flags=0))
        for gate in range(8):cases.append(dict(mode=mode,gate=gate,flags=0,knackered=1))
        for mutate in [1,10]:cases.append(dict(mode=mode,mutate=mutate,flags=1,gate=5,types=0x111))
        for mutation in [0,1,10]:cases.append(dict(mode=mode,callback=True,callback_mutate=mutation,gate=5,flags=1,types=0x111))
        for vertex in [0,65535,65536,0xffffffff]:cases.append(dict(mode=mode,vertex=vertex,flags=0))
    for driver in [0,6,7,8,0x80000000,0x7fffffff]:cases.append(dict(mode=1,driver=driver,colour=0,flags=0))
    for fire in range(12):cases.append(dict(mode=1,fire=fire,flags=0))
    for lifetime in [0,1,1999,2000,2001,0x80000000,0xffffffff]:
        for flags in [0,1,4,5,0x3ff,0x80000000]:cases.append(dict(mode=1,lifetime=lifetime,flags=flags))
    for colour in [0,1,0x80000000,0x7fffffff]:
        for oldcolour in [0,1,0x80000000,0x7fffffff]:cases.append(dict(mode=1,colour=colour,oldcolour=oldcolour,flags=5))
    for pattern in [1,2,3,4]:
        for gravity in [0x3f000000,0x3f800000]:cases.append(dict(mode=1,pattern=pattern,gravity=gravity,flags=5,colour=1))
    for lifetime in [0,1,2000,0x80000000,0xffffffff]:cases.append(dict(mode=0,lifetime=lifetime,flags=0))
    cases.append(dict(mode=1,null_car=True,flags=1,colour=0,oldcolour=0,lifetime=1))
    for pointer in [0,0xffffffff,0x14000]:cases.append(dict(mode=2,pointer=pointer))
    results=[]
    for index,case in enumerate(cases):
        h=[case['mode'],case.get('index',0),case.get('flags',1),case.get('index',0),case.get('smoke_on',1),0,0,1,17,case.get('colour',0),case.get('rand',32767),case.get('driver',7),case.get('gravity',0x3f800000),case.get('lifetime',10000),int(case.get('special',True)),case.get('knackered',0),case.get('fire',0),0,case.get('pointer',0x14000),case.get('mutate',0),0x80000000+case['alias']+32 if 'alias' in case else 0,0,0,0]
        gate=case.get('gate',0);h[5]=gate&1;h[6]=(gate>>1)&1;h[7]=(gate>>2)&1;h[21]=case.get('record',0)
        h[22]=case.get('callback_mutate',0);h[23]=case.get('reentrancy',0);h[8]=case.get('vertex',17)
        if case.get('null_car'):h[18]=0
        arena=bytearray(b'\xa5'*SIZE);columns=bytearray(b'\xa5'*1240)
        def put(buf,off,v):struct.pack_into('<I',buf,off,v&0xffffffff)
        for i in range(45):arena[256*i:256*i+100]=bytes(100)
        put(arena,0x408,HEAP+0x500);struct.pack_into('<4I',arena,0xc000,2,HEAP+0x400,HEAP+0x500,HEAP+0x300)
        arena[0x14000:0x16000]=bytes(0x2000);arena[0x16000:0x17000]=bytes(0x1000)
        put(arena,0x14008,HEAP+0x16000);put(arena,0x1400c,h[11]);put(arena,0x14010,HEAP+0x300);put(arena,0x141d4,h[15])
        for i in range(12):struct.pack_into('<H',arena,0x140ac+2*i,(case.get('vertex',0x8000+73*i))&0xffff)
        put(arena,0x16258,HEAP+0x16800 if h[14] else 0);put(arena,0x16888,h[12])
        for i in range(10):
            struct.pack_into('<4I',columns,124*i,1,HEAP+0x14000 if i%2==0 else 0,HEAP+0x16000 if i%2==0 else 0,HEAP+0x300)
            put(columns,124*i+20,case.get('lifetime',10000));put(columns,124*i+28,case.get('oldcolour',0));put(columns,124*i+36,HEAP+0x600+0x400*i)
            put(arena,0x600+0x400*i+8,HEAP+0x700+0x400*i)
            struct.pack_into('<H',arena,0x600+0x400*i+16,case.get('depth',0))
            for j in range(3):put(arena,0x700+0x400*i+256*j,HEAP+0x800+0x400*i+256*j if j<2 else 0)
            for j in range(3):arena[0x700+0x400*i+256*j+18]=(case.get('types',0)>>(4*j))&15
            for j in range(3):
                put(columns,124*i+40+4*j,1000+i*10+j)
                for off in [64,76,88,100]:put(columns,124*i+off+4*j,case.get('column_scalar',0x3f800000+0x1000*i+0x100*j+off))
        if case.get('null_car'):put(columns,4,0)
        pattern=case.get('pattern',0)
        if pattern:
            put(columns,28,2);put(columns,124*2+28,0 if pattern in [1,2] else 2)
            put(columns,124*2+20,0 if pattern in [2,4] else 1)
        put(arena,0x7000,17);put(arena,0x7004,0)
        for off in range(0xb000,0x13000-34,34):put(arena,off,17);put(arena,off+4,1);struct.pack_into('<H',arena,off+32,32)
        for i in range(71):arena[0x13000+24*i:0x13000+24*i+24]=bytes(24);put(arena,0x13000+24*i+4,20)
        if case.get('callback'):
            for kind in [17,18]:put(arena,0x13000+24*kind+8,0x1082000)
        fixture=out/'fixture.bin';fixture.write_bytes(struct.pack('<24I',*h)+arena+columns)
        expected=execute(*images[0],h,arena,columns);checks=[]
        if len(images)>1:checks.append(('recompiled-x86',execute(*images[1],h,arena,columns)))
        for compiler in commands:checks.append((compiler,json.loads(subprocess.check_output([str(out/(compiler+'.exe')),str(fixture)],text=True))))
        for compiler,actual in checks:
            if actual!=expected:
                failure=out/f'failure-{compiler}-{index}.json';failure.write_text(json.dumps(dict(case=case,original=expected,actual=actual),indent=2))
                raise AssertionError(f'{compiler} column case{index} differs: {failure}')
            results.append(dict(case=case,compiler=compiler,passed=True,state_sha256=hashlib.sha256(bytes.fromhex(actual['state'])).hexdigest()))
    physical_evidence=dict(original_sha256=ORIGINAL_HASH,precision_control=53,scope='Exact original CreateSmokeColumn120B, ConditionalSmokeColumn244B, SmudgeCar1B and IRandomBetween34B execute physically. Only original absolute global/constant operands and relative dependency calls are relocated. Original internal branches unchanged; constructor and replay execute actual production C bodies. CRT rand/BRender callbacks controlled.',bodies=physical_code,patches=patches,compiler_command=commands['physical-original'],executable_sha256=hashlib.sha256((out/'physical-original.exe').read_bytes()).hexdigest())
    (out/'physical-original.json').write_text(json.dumps(physical_evidence,indent=2)+'\n')
    verification=dict(scenario_count=len(cases),comparisons=len(results),original_sha256=ORIGINAL_HASH,source_sha256=hashlib.sha256(SOURCE.read_bytes()).hexdigest(),piping_source_sha256=hashlib.sha256(PIPE.read_bytes()).hexdigest(),utility_source_sha256=hashlib.sha256(UTILITY.read_bytes()).hexdigest(),bodies_sha256=[hashlib.sha256(b.encode()).hexdigest() for b in bodies],compiler_commands=commands,cases=results,physical_original=physical_evidence,padding_exclusions={'type17':[18,19],'type18':[2,3]},oracle_correction=None,limitations=['Valid accessible car/physics objects and fire indices0..11 on dereferencing paths; null-car case is the original early-return path only.','FPU exception-status flags are outside the observed game state; PC53 control and raw input/output bytes are verified.','Replay callback fixtures return fixed20-byte payload lengths; exclusions track only original unwritten padding and actual propagated copies.'],new_original_entries=['CreateSmokeColumn4fb890','ConditionalSmokeColumn4fcab0','SmudgeCar4fc9c0'],global_evidence={'gBlack_material':{'original_address':'0x006b7880','type':'br_material*','initial_value':None,'ownership':'Original spark module; original .data virtual tail beyond raw size, zero-initialized by PE loader.'}})
    if args.symbols:verification.update(recompiled_sha256=hashlib.sha256(args.recompiled.read_bytes()).hexdigest(),symbols_sha256=hashlib.sha256(args.symbols.read_bytes()).hexdigest())
    (out/'verification.json').write_text(json.dumps(verification,indent=2)+'\n')
    print(f'PASS: {len(cases)} scenarios, {len(results)} comparisons.')


if __name__=='__main__':main()
