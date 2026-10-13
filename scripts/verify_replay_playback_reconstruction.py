#!/usr/bin/env python3
"""Compare replay playback and sound timing with audited physical original x86.

Replay traversal, reverse dispatch, sound duration lookup and game clock are real
code. Caller callbacks, KeyIsDown/WaitForNoKeys and FatalError are the controlled
boundaries. Complete arenas, public globals, callback order/arguments and return
values are compared. Physical entry calls check ESP, preserved registers and x87
TOP. Runs use masked exceptions/PC53; exception status flags are outside scope.
Fixtures use nonzero, nonoverflowing signed divisors; divide-fault handling and
cyclic descriptor alternatives are outside scope.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import struct
import subprocess
import sys

from verify_replay_session_reconstruction import raw,PHYSICAL_LOADER,ORIGINAL_HASH
from verify_tinted_native_reconstruction import extract
from verify_replay_search_reconstruction import GLOBALS as SEARCH_GLOBALS

ROOT=Path(__file__).resolve().parents[1]
SOURCE=ROOT/'src/carma2/newcommon/23-piping.c'
SAMPLE=ROOT/'src/carma2/s3/src/sample.c'
FUNCTIONS=dict(MoveReplayBuffer=(0x403260,1190),ScanAndPlaySoundsToBe=(0x403990,692),
    SoundTimeout=(0x403c50,30),ARMoveToEndOfReplay=(0x403210,74),
    ARMoveToStartOfReplay=(0x403c70,74),ARDoActionReplay=(0x403cc0,56),
    S3GetSampleDuration=(0x564e5e,61),S3GetDescriptorByID=(0x565a3d,77),
    GetTotalTime=(0x514c30,60),ARReplayForwards=(0x402300,93),
    ARReplayIsReallyPaused=(0x402360,27),ARSomeReplayLeft=(0x4025b0,47),
    LengthOfChunk=(0x403720,48),LengthOfSession=(0x403750,103),
    MoveSessionPointerBackOne=(0x4037c0,60),FindPreviousChunk=(0x403800,388),
    ARAdvanceChunkPtr=(0x4024e0,87),ftol=(0x575b80,39))
GLOBALS={**SEARCH_GLOBALS,
    'gReplay_pause_state':(0x6768fc,'int'),'gPipe_count_callbacks':(0x676910,'int'),
    'gDisable_piped_sounds':(0x676918,'int'),'gPipe_sound_time':(0x6768b0,'tU32'),
    'gPipe_sound_start_time':(0x676904,'tU32'),'gPipe_sound_time_window':(0x6768a8,'tU32'),
    'gFrame_period':(0x74d3e4,'tU32'),'gLast_replay_frame_time':(0x79efb4,'tU32'),
    'gAction_replay_mode':(0x676914,'int'),'gNet_mode':(0x68b918,'tNet_mode'),
    'gLast_tick_count':(0x79efb0,'tU32'),'gLost_time':(0x6abef4,'tU32'),
    'gS3_state':(0x7a0580,'tS3_state')}
EXTERNALS=dict(FatalError=0x44c230,KeyIsDown=0x483040,WaitForNoKeys=0x4833b0)
ADDRESSES={**{n:a for n,(a,_) in FUNCTIONS.items()},**{n:a for n,(a,_) in GLOBALS.items()},**EXTERNALS}


def decode_body(cs,data,address,length):
    from capstone import CS_OP_MEM
    decoded=list(cs.disasm(raw(data,address,length),address));tables=[]
    for ins in decoded:
        if ins.mnemonic!='jmp' or ins.operands[0].type!=CS_OP_MEM:continue
        op=ins.operands[0]
        assert not op.mem.base and op.mem.index and op.mem.scale==4
        targets=[]
        for j in range(64):
            value=struct.unpack('<I',raw(data,op.mem.disp+4*j,4))[0]
            if not address<=value<address+length:break
            targets.append(value)
        assert len(targets)==5,(hex(ins.address),targets)
        tables.append(dict(address=op.mem.disp,targets=targets))
    # MSVC5 puts the switch table after RET in the function's symbol extent.
    # Audit its five pointers as data, rather than decoding them as instructions.
    ranges=sorted((t['address'],t['address']+4*len(t['targets'])) for t in tables if address<=t['address']<address+length)
    decoded=[];cursor=address
    for lo,hi in ranges+[(address+length,address+length)]:
        assert cursor<=lo<=hi<=address+length
        part=list(cs.disasm(raw(data,cursor,lo-cursor),cursor))
        assert sum(i.size for i in part)==lo-cursor,(hex(cursor),lo-cursor)
        decoded.extend(part);cursor=hi
    starts={i.address for i in decoded}
    assert all(target in starts for t in tables for target in t['targets'])
    return decoded,tables


def audit(path,data,addresses,spans):
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM,CS_OP_REG
    cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
    patches={};closure=[];tables=[];code={a for a,_ in spans}|{addresses[n] for n in EXTERNALS}
    def in_code(a):return any(lo<=a<lo+n for lo,n in spans)
    image_end=0x400000+struct.unpack_from('<I',data,struct.unpack_from('<I',data,60)[0]+24+56)[0]
    for address,length in spans:
        body=raw(data,address,length);decoded,body_tables=decode_body(cs,data,address,length)
        tables.extend(body_tables)
        for table in body_tables:
            for j,value in enumerate(table['targets']):patches[table['address']+4*j-0x400000]=value
        closure.append(dict(address=address,length=length,sha256=hashlib.sha256(body).hexdigest()))
        for ins in decoded:
            if ins.mnemonic=='call' or ins.mnemonic.startswith('j'):
                op=ins.operands[0]
                if op.type==CS_OP_IMM:
                    assert op.imm in code or (ins.mnemonic.startswith('j') and address<=op.imm<address+length),(hex(ins.address),ins.op_str)
                elif ins.mnemonic=='jmp':
                    assert op.type==CS_OP_MEM and not op.mem.base and op.mem.index and op.mem.scale==4
                    assert any(t['address']==op.mem.disp for t in body_tables)
                else:assert op.type==CS_OP_REG or (op.type==CS_OP_MEM and op.mem.base),(hex(ins.address),ins.op_str)
            for op in ins.operands:
                if op.type==CS_OP_MEM and not op.mem.base and op.mem.disp>=0x400000:
                    allowed=any(addresses[n]<=op.mem.disp<addresses[n]+(28 if n=='gS3_state' else 4) for n in GLOBALS)
                    allowed=allowed or any(t['address']==op.mem.disp for t in tables)
                    if not allowed:
                        assert ins.mnemonic in ('fld','fcomp','fcom','fmul','fdiv'),(hex(ins.address),ins.op_str)
                        assert op.size in (4,8)
                        value=struct.unpack('<f' if op.size==4 else '<d',raw(data,op.mem.disp,op.size))[0]
                        assert value in (0.,1500.,1./65536.),(hex(ins.address),value)
                    assert ins.disp_size==4;patches[ins.address+ins.disp_offset-0x400000]=op.mem.disp
                elif op.type==CS_OP_IMM and ins.mnemonic not in ('call',) and not ins.mnemonic.startswith('j') and 0x400000<=op.imm<image_end:
                    assert op.imm in code or in_code(op.imm),(hex(ins.address),ins.op_str)
                    assert ins.imm_size==4;patches[ins.address+ins.imm_offset-0x400000]=op.imm
    patches=sorted(patches.items())
    path.write_bytes(struct.pack('<'+str(len(ADDRESSES)+1)+'I',*[addresses[n] for n in ADDRESSES],len(patches))+b''.join(struct.pack('<2I',*v) for v in patches))
    return dict(closure=closure,jump_tables=tables,absolute_relocations=patches)


def definitions():
    ix={n:i for i,n in enumerate(ADDRESSES)}
    lines=[f'static unsigned addresses[{len(ix)}];','static unsigned __cdecl checked(unsigned,unsigned,unsigned,unsigned*,unsigned,unsigned);']
    for n,(_,t) in GLOBALS.items():lines.append(f'#define {n} (*({t}*)(uintptr_t)addresses[{ix[n]}])')
    for n in ['SoundTimeout','S3GetSampleDuration']:
        arg='tU32' if n=='SoundTimeout' else 'int'
        lines.append(f'int C2_HOOK_FASTCALL {n}({arg} v){{return checked(addresses[{ix[n]}],v,0,NULL,0,0);}}')
    lines.append(f'void C2_HOOK_FASTCALL ScanAndPlaySoundsToBe(tU8*p,tU32 t,tU32 start){{unsigned w=start;checked(addresses[{ix["ScanAndPlaySoundsToBe"]}],(unsigned)(uintptr_t)p,t,&w,1,0);}}')
    for n in ['MoveReplayBuffer','ARDoActionReplay']:
        lines.append(f'void C2_HOOK_FASTCALL {n}(int frames,tReplay_session_callback*middle,tPreprocess_AR_callback*before,tPostProcess_AR_callback*after){{unsigned w[2];w[0]=(unsigned)(uintptr_t)before;w[1]=(unsigned)(uintptr_t)after;checked(addresses[{ix[n]}],frames,(unsigned)(uintptr_t)middle,w,2,0);}}')
    for n in ['ARMoveToEndOfReplay','ARMoveToStartOfReplay']:
        lines.append(f'void C2_HOOK_FASTCALL {n}(tReplay_session_callback*middle,tPreprocess_AR_callback*before,tPostProcess_AR_callback*after){{unsigned w=(unsigned)(uintptr_t)after;checked(addresses[{ix[n]}],(unsigned)(uintptr_t)middle,(unsigned)(uintptr_t)before,&w,1,0);}}')
    return '\n'.join(lines)


DRIVER=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "23-piping.h"
#include "41-utility.h"
#include "42-input.h"
#include "52-errors.h"
#include "globvars.h"
#include "globvrpb.h"
#include <s3/internal/sample.h>
#include <s3/internal/audio.h>
#ifndef PHYSICAL
GLOBAL_DEFINITIONS
#else
PHYSICAL_DEFINITIONS
#endif
static unsigned arena[0x9000/4],h[48];static int events,callback_calls,key_calls;
static tPipe_chunk**held;
static int C2_HOOK_FASTCALL length1(tPipe_chunk*p);
static int C2_HOOK_FASTCALL length2(tPipe_chunk*p);
static void C2_HOOK_FASTCALL apply1(tPipe_chunk**p);
static void C2_HOOK_FASTCALL apply2(tPipe_chunk**p);
static void C2_HOOK_FASTCALL undo1(tPipe_chunk**p,tPipe_chunk*previous);
static void C2_HOOK_FASTCALL reset1(void);
static unsigned rd(unsigned off){unsigned v;memcpy(&v,(char*)arena+off,4);return v;}
static void wr(unsigned off,unsigned v){memcpy((char*)arena+off,&v,4);}
static tU8*ptr(unsigned off){return off==0xffffffff?NULL:(tU8*)arena+off;}
static unsigned norm(unsigned v){unsigned b=(unsigned)(uintptr_t)arena;if(v>=b&&v<=b+sizeof(arena))return v-b+0x1000000;
    if(v==(unsigned)(uintptr_t)length1)return 0x2000010;
    if(v==(unsigned)(uintptr_t)length2)return 0x2000020;
    if(v==(unsigned)(uintptr_t)apply1)return 0x2000030;
    if(v==(unsigned)(uintptr_t)apply2)return 0x2000040;
    if(v==(unsigned)(uintptr_t)undo1)return 0x2000050;
    if(v==(unsigned)(uintptr_t)reset1)return 0x2000060;
    return v;}
static void hex(const void*p,unsigned n){unsigned i;for(i=0;i<n;i++)printf("%02x",((const unsigned char*)p)[i]);}
static void snapshot(void){unsigned prefix[32],i,j,k,v;unsigned char copy[sizeof(arena)];
    prefix[0]=norm((unsigned)(uintptr_t)gPipe_buffer_start);prefix[1]=norm((unsigned)(uintptr_t)gPipe_buffer_working_end);prefix[2]=norm((unsigned)(uintptr_t)gPipe_buffer_oldest);prefix[3]=norm((unsigned)(uintptr_t)gPipe_record_ptr);prefix[4]=norm((unsigned)(uintptr_t)gPipe_play_ptr);prefix[5]=norm((unsigned)(uintptr_t)gPipe_chunk_end);prefix[6]=norm((unsigned)(uintptr_t)gPipe_callbacks);memcpy(prefix+7,&gReplay_rate,4);prefix[8]=gPlay_direction;prefix[9]=gDisable_advance;prefix[10]=gMax_previous_pipe_sessions;prefix[11]=gReplay_pause_state;prefix[12]=gPipe_count_callbacks;prefix[13]=gDisable_piped_sounds;prefix[14]=gPipe_sound_time;prefix[15]=gPipe_sound_start_time;prefix[16]=gPipe_sound_time_window;prefix[17]=gFrame_period;prefix[18]=gLast_replay_frame_time;prefix[19]=gAction_replay_mode;prefix[20]=gNet_mode;prefix[21]=gLast_tick_count;prefix[22]=gLost_time;prefix[23]=norm((unsigned)(uintptr_t)gS3_state.descriptors);prefix[24]=key_calls;
    memcpy(prefix+25,&gS3_state,sizeof(gS3_state));prefix[30]=norm(prefix[30]);
    memcpy(copy,arena,sizeof(copy));for(j=0;j<2;j++)for(i=0;i<71;i++)for(k=0;k<6;k++)if(k!=1&&k!=4){unsigned off=0x6000+j*0x1000+24*i+4*k;v=norm(rd(off));memcpy(copy+off,&v,4);}
    for(i=0;i<3;i++){v=norm(rd(0x5000+i*0x80+0x2c));memcpy(copy+0x5000+i*0x80+0x2c,&v,4);v=norm(rd(0x5000+i*0x80+0x40));memcpy(copy+0x5000+i*0x80+0x40,&v,4);}hex(prefix,sizeof(prefix));hex(copy,sizeof(copy));}
static void event(char kind,unsigned a,unsigned b,unsigned c){if(events>1500)exit(91);printf("%s[\"%c\",%u,%u,%u,\"",events++?",":"",kind,a,b,c);snapshot();printf("\"]");}
static void mutate(void){if((unsigned)callback_calls++!=h[16])return;
    if(h[15]&1)gPipe_callbacks=(tReplay_callback*)((char*)arena+0x7000);
    if(h[15]&2)gPipe_count_callbacks=h[32];
    if(h[15]&4)gFrame_period=h[33];
    if(h[15]&8)gReplay_rate=-1.f;
    if(h[15]&16)gPipe_play_ptr=ptr(h[30]);
    if(h[15]&32&&held!=NULL)*held=(tPipe_chunk*)ptr(h[31]);
    if(h[15]&64)gDisable_advance=1;
    if(h[15]&128)gPipe_chunk_end=ptr(h[31]);
    if(h[15]&256)gPipe_record_ptr=ptr(h[30]);
    if(h[15]&512)wr(h[34],h[35]);
    if(h[15]&1024)gDisable_piped_sounds=h[36];
}
static int length(tPipe_chunk*p,int which){event('L',norm((unsigned)(uintptr_t)p),which,0);mutate();return h[14];}
static int C2_HOOK_FASTCALL length1(tPipe_chunk*p){return length(p,1);}
static int C2_HOOK_FASTCALL length2(tPipe_chunk*p){return length(p,2);}
static void apply(tPipe_chunk**p,int which){held=p;event('A',norm((unsigned)(uintptr_t)*p),which,gDisable_advance);mutate();if(h[17]&&!gDisable_advance)*p=(tPipe_chunk*)((tU8*)*p+h[17]);}
static void C2_HOOK_FASTCALL apply1(tPipe_chunk**p){apply(p,1);}
static void C2_HOOK_FASTCALL apply2(tPipe_chunk**p){apply(p,2);}
static void C2_HOOK_FASTCALL undo1(tPipe_chunk**p,tPipe_chunk*previous){held=p;event('U',norm((unsigned)(uintptr_t)*p),norm((unsigned)(uintptr_t)previous),gDisable_advance);mutate();if(h[17])*p=(tPipe_chunk*)((tU8*)*p+h[17]);}
static void C2_HOOK_FASTCALL reset1(void){event('R',0,0,0);mutate();}
static void C2_HOOK_FASTCALL before(int period){event('B',period,0,0);mutate();}
static void C2_HOOK_FASTCALL after(int period){event('E',period,0,0);mutate();}
static void C2_HOOK_FASTCALL middle(int frames){event('M',frames,0,0);mutate();}
int C2_HOOK_FASTCALL KeyIsDown(int index){int result=key_calls++>=(int)h[18];event('K',index,result,0);return result;}
void C2_HOOK_FASTCALL WaitForNoKeys(void){event('W',0,0,0);}
void C2_NORETURN C2_HOOK_CDECL FatalError(int index,...){event('F',index,0,0);printf("],\"state\":\"");snapshot();printf("\",\"value\":null,\"fatal\":%d}\n",index);exit(0);}
#ifndef PHYSICAL
BODIES
#endif
static int run(int argc,char**argv){FILE*f;unsigned i,j,k,value=0;if(argc!=2)return 1;SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);f=fopen(argv[1],"rb");if(!f)return 2;if(fread(h,sizeof(h),1,f)!=1||fread(arena,sizeof(arena),1,f)!=1)return 3;fclose(f);_controlfp(_PC_53,_MCW_PC);
    gPipe_play_ptr=ptr(h[2]);gPipe_record_ptr=ptr(h[3]);gPipe_buffer_oldest=ptr(h[4]);gPipe_buffer_working_end=ptr(h[5]);gPipe_buffer_start=ptr(h[6]);memcpy(&gReplay_rate,h+7,4);gPlay_direction=h[8];gDisable_piped_sounds=h[9];gPipe_count_callbacks=h[10];gReplay_pause_state=0xa5a5;gDisable_advance=h[11];gMax_previous_pipe_sessions=h[12];gPipe_chunk_end=ptr(0x8000);gPipe_callbacks=(tReplay_callback*)((char*)arena+0x6000);gPipe_sound_time=h[23];gPipe_sound_start_time=h[24];gPipe_sound_time_window=h[25];gFrame_period=h[19];gLast_replay_frame_time=h[20];gAction_replay_mode=h[21];gNet_mode=(tNet_mode)h[22];gLast_tick_count=h[26];gLost_time=h[27];memset(&gS3_state,0,sizeof(gS3_state));gS3_state.descriptors=(tS3_descriptor*)ptr(0x5000);
    for(j=0;j<2;j++)for(i=0;i<71;i++)for(k=0;k<6;k++){unsigned off=0x6000+0x1000*j+24*i+4*k,v=rd(off);if(v==0x2000010)wr(off,(unsigned)(uintptr_t)length1);else if(v==0x2000020)wr(off,(unsigned)(uintptr_t)length2);else if(v==0x2000030)wr(off,(unsigned)(uintptr_t)apply1);else if(v==0x2000040)wr(off,(unsigned)(uintptr_t)apply2);else if(v==0x2000050)wr(off,(unsigned)(uintptr_t)undo1);else if(v==0x2000060)wr(off,(unsigned)(uintptr_t)reset1);}
    for(i=0;i<3;i++){wr(0x5000+i*0x80+0x2c,i<2?(unsigned)(uintptr_t)ptr(0x5080+i*0x80):0);wr(0x5000+i*0x80+0x40,(unsigned)(uintptr_t)ptr(0x5300+i*32));}
    if(h[37])gS3_state.descriptors=NULL;
    printf("{\"calls\":[");
    if(h[0]==0)MoveReplayBuffer((int)h[1],middle,before,after);
    else if(h[0]==1)ScanAndPlaySoundsToBe(ptr(h[2]),h[23],h[24]);
    else if(h[0]==2)value=SoundTimeout(h[28]);
    else if(h[0]==3)ARMoveToEndOfReplay(middle,before,after);
    else if(h[0]==4)ARMoveToStartOfReplay(middle,before,after);
    else if(h[0]==5)ARDoActionReplay(0x12345678,middle,before,after);
    else if(h[0]==6)value=S3GetSampleDuration(h[28]);
    printf("],\"state\":\"");snapshot();printf("\",\"value\":%u,\"fatal\":null}\n",value);return 0;
}
'''


def fixtures():
    cases=[]
    for frames in (-3,-2,-1,0,1,2,3,-0x80000000):
        for flag in (-1,0,1,2,3,4,5,6):
            for cb in (0,1):cases.append(dict(mode=0,frames=frames,flag=flag,callback=cb))
    for mode in (0,1,3,4,5):
        for play in (0,1,3,4,7,10):cases.append(dict(mode=mode,play=play,frames=-1 if mode in (1,4) else 1))
    for flag in (1,2,3,4,5):
        for apply in (0,1):
            for undo in (0,1):cases.append(dict(mode=0,frames=-1,flag=flag,apply=apply,undo=undo))
    for mode in (0,3,4,5):
        for key in (0,1,2,4):cases.append(dict(mode=mode,key=key,frames=3))
    for rate in (0,0x80000000,1,0x80000001,0x3f000000,0xbf000000,0x3f800000,0xbf800000,0x40000000,0xc0000000,0x7f800001,0x7fc12345,0x7f800000,0xff800000,0x5f000000,0xdf000000):
        for mode in (1,3,4,5):cases.append(dict(mode=mode,rate=rate,frames=-1,key=3))
    for mode in (0,1):
        for mutation in (1,2,4,8,16,32,64,128,256,1024):
            for at in (0,2,5):cases.append(dict(mode=mode,frames=-1,callback=1,mutate=mutation,at=at))
    for window in (0,1,1500,0x80000000,0xffffffff):
        for now in (0,1,1000,2000,0x7fffffff,0xffffffff):cases.append(dict(mode=2,window=window,time=2000,test_time=now))
    for sample in (-1,100,101,102,103):
        for size,rate in [(0,48000),(48000,48000),(0x7fffffff,3),(-1,-3),(1,0x7fffffff),(0x80000000,7)]:cases.append(dict(mode=6,sample=sample,sample_size=size,sample_rate=rate))
    for pitch in (-1,0,1,2,32768,65536,0x7fffffff):cases.append(dict(mode=1,frames=-1,pitch=pitch))
    for net in (0,1,2):cases.append(dict(mode=0,frames=-1,replay=0,net=net,ticks=0xfffffff0,period=32,lost=128))
    for mode in (0,1,3,4,5):
        for frames in (-2,2):
            for flag in (1,2,3,4,5):cases.append(dict(mode=mode,frames=frames,flag=flag,wrap=1))
    for mode in (0,1):
        for frames in (-1,1):
            for advance in (4,12):cases.append(dict(mode=mode,frames=frames,advance=advance))
    for alternatives in ((101,-1,-1),(101,102,-1),(105,-1,-1)):
        cases.append(dict(mode=6,sample=100,alternatives=alternatives))
    cases.append(dict(mode=6,sample=100,empty_descriptors=1))
    for mode in (0,1,3,4,5):cases.append(dict(mode=mode,frames=-1,sound_disabled=7))
    for now in (0,1,2999,3000,4000,0xffffffff):cases.append(dict(mode=1,frames=-1,sound_time=now))
    for pitch in (1,2):
        for size,rate in ((0x7fffffff,3),(-1,-3),(0x80000000,7)):
            cases.append(dict(mode=1,frames=-1,pitch=pitch,sample_size=size,sample_rate=rate))
    return cases


def fixture(case):
    arena=bytearray(b'\xa5'*0x9000)
    def put(off,v):struct.pack_into('<I',arena,off,v&0xffffffff)
    positions=[];at=0x1000;callback=case.get('callback',0);extra=2 if callback else 0
    for index,typ in enumerate([0,1,2,0,1,2,0,1,2,0]):
        positions.append(at);count=1 if typ==0 else 2;put(at,typ);put(at+4,count);stride=8 if typ==0 else 12
        for j in range(count):put(at+8+j*stride,100+j);put(at+12+j*stride,(index//3+1)*1000 if typ==0 else case.get('pitch',65536));
        size=10+stride*count;struct.pack_into('<H',arena,at+size-2,size-2);at+=size
    positions.append(at)
    for table in range(2):
        for typ in range(71):
            off=0x6000+0x1000*table+24*typ;arena[off:off+24]=bytes(24)
            put(off,0x2000060 if typ<3 else 0);put(off+4,(4 if typ==0 else 8)-extra);put(off+8,(0x2000010+16*table) if callback else 0)
            put(off+12,(0x2000030+16*table) if case.get('apply',1) else 0);put(off+16,case.get('flag',4));put(off+20,0x2000050 if case.get('undo',1) else 0)
    for i in range(3):
        off=0x5000+i*0x80;arena[off:off+76]=bytes(76);put(off,100+i);put(off+0x3c,case.get('alternatives',(-1,-1,-1))[i])
        put(0x5300+i*32,case.get('sample_size',48000));put(0x5304+i*32,case.get('sample_rate',48000))
    mode=case['mode'];frames=case.get('frames',1);play=case.get('play',7 if frames<0 or mode==4 else 1)
    rate=case.get('rate',0xbf800000 if frames<0 or mode==4 else 0x3f800000)
    h=[mode,frames,positions[play],positions[10],positions[0],positions[10],positions[0],rate,-1 if frames<0 or mode==4 else 1,case.get('sound_disabled',0),3,case.get('disabled',0),1000,callback,extra,case.get('mutate',0),case.get('at',0),case.get('advance',0),case.get('key',100),case.get('period',16),5000,case.get('replay',1),case.get('net',0),5000,case.get('time',2000),case.get('window',1500),case.get('ticks',9000),case.get('lost',0),case.get('sample',case.get('test_time',0)),0,positions[4],positions[5]+8,1,33,positions[7],1,1]+[0]*11
    h[23]=case.get('sound_time',h[23]);h[37]=case.get('empty_descriptors',0)
    if case.get('wrap'):
        h[2]=positions[1 if frames<0 or mode==4 else 8];h[3]=positions[6];h[4]=positions[8]
    return struct.pack('<48I',*[v&0xffffffff for v in h])+arena


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,default=ROOT/'build-audit/replay-playback-tests');p.add_argument('--case-start',type=int,default=0);p.add_argument('--case-limit',type=int)
    p.add_argument('--recompiled',type=Path,default=ROOT/'build-matching/rec2.exe')
    p.add_argument('--symbols',type=Path)
    args=p.parse_args();out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)
    original=ROOT/'build-reference/originals/CARMA2_HW.EXE';data=original.read_bytes();assert hashlib.sha256(data).hexdigest()==ORIGINAL_HASH
    metadata=audit(out/'original-addresses.bin',data,ADDRESSES,list(FUNCTIONS.values()))
    linked_meta=None
    if args.symbols:
        mapping=json.loads(args.symbols.read_text());rebuilt=args.recompiled.read_bytes()
        assert mapping['format']==1 and mapping['original_sha256']==ORIGINAL_HASH
        assert mapping['recompiled_sha256']==hashlib.sha256(rebuilt).hexdigest()
        syms={s['orig']:s for s in mapping['symbols']};linked={n:syms[a]['recomp'] for n,a in ADDRESSES.items()}
        known={s['recomp'] for s in mapping['symbols']}
        spans=[(linked[n],min(a for a in known if a>linked[n])-linked[n]) for n in FUNCTIONS]
        from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM
        cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;pending=list(spans);seen={a for a,_ in spans}
        while pending:
            address,length=pending.pop()
            for ins in decode_body(cs,rebuilt,address,length)[0]:
                if ins.mnemonic=='call' and ins.operands[0].type==CS_OP_IMM:
                    target=ins.operands[0].imm
                    if target in seen or target in {linked[n] for n in EXTERNALS}:continue
                    assert target in known,(hex(ins.address),ins.op_str)
                    extra=(target,min(a for a in known if a>target)-target);seen.add(target);spans.append(extra);pending.append(extra)
        linked_meta=audit(out/'linked-addresses.bin',rebuilt,linked,spans)
    source=SOURCE.read_text();names=['ReplaySearchSessionLength','ReplayFtolLow',*[n for n in FUNCTIONS if n not in ('ftol','S3GetSampleDuration','S3GetDescriptorByID','GetTotalTime')]]
    bodies=[extract(source,n) for n in names]+[extract(SAMPLE.read_text(),n) for n in ['S3GetDescriptorByID','S3GetSampleDuration']]+[extract((ROOT/'src/carma2/newcommon/41-utility.c').read_text(),'GetTotalTime')]
    declarations='\n'.join(t+' '+n+';' for n,(_,t) in GLOBALS.items())
    driver=DRIVER.replace('GLOBAL_DEFINITIONS',declarations).replace('PHYSICAL_DEFINITIONS',definitions()).replace('BODIES','\n\n'.join(bodies))
    (out/'harness.c').write_text(driver+'\nint C2_HOOK_CDECL main(int argc,char**argv){return run(argc,argv);}\n')
    loader=PHYSICAL_LOADER
    patches=[]
    for name in EXTERNALS:
        patches.append('{unsigned char*site=(unsigned char*)(uintptr_t)addresses[%d];site[0]=0xe9;value=(unsigned)(uintptr_t)%s-((unsigned)(uintptr_t)site+5);memcpy(site+1,&value,4);}'%(list(ADDRESSES).index(name),name))
    old='{unsigned char*site=(unsigned char*)(uintptr_t)addresses[FATAL_INDEX];site[0]=0xe9;value=(unsigned)(uintptr_t)FatalError-((unsigned)(uintptr_t)site+5);memcpy(site+1,&value,4);}'
    assert old in loader;loader=loader.replace(old,'\n'.join(patches))
    (out/'physical.c').write_text('#define PHYSICAL\n'+driver+loader)
    includes=[ROOT/r for r in ['src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/carma2/platform','src/carma2/s3/include','src/brender/include']]
    msvc=sorted((ROOT/'build-tools/msvc-modern/VC/Tools/MSVC').iterdir())[-1];sdk=ROOT/'build-tools/msvc-modern/Windows Kits/10';version=sorted(p.name for p in (sdk/'Include').iterdir())[-1]
    env=dict(os.environ);env['PATH']=str(msvc/'bin/Hostx64/x86')+os.pathsep+env['PATH'];env['INCLUDE']=os.pathsep.join(map(str,[msvc/'include',*[sdk/f'Include/{version}/{r}' for r in ['ucrt','shared','um']]]));env['LIB']=os.pathsep.join(map(str,[msvc/'lib/x86',*[sdk/f'Lib/{version}/{r}/x86' for r in ['ucrt','um']]]))
    old=ROOT/'build-tools/msvc500sp3';oldenv=dict(os.environ);oldenv['PATH']=str(old/'bin')+os.pathsep+str(old/'redist')+os.pathsep+oldenv['PATH'];oldenv['INCLUDE']=str(old/'include');oldenv['LIB']=str(old/'lib')
    commands={
        'msvc':[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(r) for r in includes],str(out/'harness.c'),'/Fo'+str(out/'msvc.obj'),'/Fe'+str(out/'msvc.exe')],
        'msvc5':[str(old/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(r) for r in includes],str(out/'harness.c'),'/Fo'+str(out/'msvc5.obj'),'/Fe'+str(out/'msvc5.exe')],
        'mingw':[str(ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-fno-strict-aliasing',*['-I'+str(r) for r in includes],str(out/'harness.c'),'-o',str(out/'mingw.exe')],
        'physical':[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS','/DREC2_MATCHING',*['/I'+str(r) for r in includes],str(out/'physical.c'),'/Fo'+str(out/'physical.obj'),'/Fe'+str(out/'physical.exe')]}
    for compiler,cmd in commands.items():
        result=subprocess.run(cmd,cwd=out,env=env if compiler in ('msvc','physical') else oldenv if compiler=='msvc5' else None,text=True,capture_output=True)
        (out/(compiler+'-build.log')).write_text(result.stdout+result.stderr);assert result.returncode==0,result.stdout+result.stderr
    cases=fixtures()[args.case_start:];results=[]
    if args.case_limit:cases=cases[:args.case_limit]
    for index,case in enumerate(cases,args.case_start):
        inp=out/'fixture.bin';inp.write_bytes(fixture(case));(out/'current-case.json').write_text(json.dumps(dict(index=index,case=case)))
        expected=json.loads(subprocess.check_output([str(out/'physical.exe'),str(inp),str(original),str(out/'original-addresses.bin')],text=True,timeout=15))
        checks=[]
        if args.symbols:checks.append(('matching-physical',json.loads(subprocess.check_output([str(out/'physical.exe'),str(inp),str(args.recompiled.resolve()),str(out/'linked-addresses.bin')],text=True,timeout=15))))
        for compiler in ('msvc','msvc5','mingw'):checks.append((compiler,json.loads(subprocess.check_output([str(out/(compiler+'.exe')),str(inp)],text=True,timeout=15))))
        for compiler,actual in checks:
            if actual!=expected:
                path=out/f'failure-{compiler}-{index}.json';path.write_text(json.dumps(dict(case=case,original=expected,actual=actual),indent=2));raise AssertionError(f'{compiler} case {index} differs: {path}')
            results.append(dict(case=case,compiler=compiler,passed=True,calls=len(actual['calls']),fatal=actual['fatal'],state_sha256=hashlib.sha256(bytes.fromhex(actual['state'])).hexdigest()))
    (out/'verification.json').write_text(json.dumps(dict(scenario_count=len(cases),comparisons=len(results),original_sha256=ORIGINAL_HASH,source_sha256=hashlib.sha256(SOURCE.read_bytes()).hexdigest(),sample_source_sha256=hashlib.sha256(SAMPLE.read_bytes()).hexdigest(),header_sha256=hashlib.sha256(SOURCE.with_suffix('.h').read_bytes()).hexdigest(),verifier_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),bodies_sha256=[hashlib.sha256(b.encode()).hexdigest() for b in bodies],compiler_commands=commands,original=metadata,linked=linked_meta,recompiled_sha256=hashlib.sha256(args.recompiled.read_bytes()).hexdigest() if args.symbols else None,symbols_sha256=hashlib.sha256(args.symbols.read_bytes()).hexdigest() if args.symbols else None,cases=results,limitations=__doc__),indent=2)+'\n')
    print(f'PASS: {len(cases)} scenarios, {len(results)} complete replay-playback comparisons.')


if __name__=='__main__':main()
