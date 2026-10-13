#!/usr/bin/env python3
"""Compare replay audio reversal with audited physical original x86.

Actual replay mode callbacks, S3 lookup/filter/resource wrappers, platform audio
reversal and the original CRT memmove execute together. Only BrMemAllocate,
BrMemFree, caller filter callbacks and the DirectSound COM methods are controlled.
Complete fixture memory, globals, callbacks, ordered snapshots and int returns
are compared. Physical calls check ESP, preserved registers and x87 TOP at PC53.
Fixtures provide valid finite object graphs and bounded copy/reversal ranges;
device hardware, concurrent callbacks and faulting/invalid memory are not tested.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import struct
import subprocess

from verify_replay_session_reconstruction import raw, PHYSICAL_LOADER, ORIGINAL_HASH
from verify_tinted_native_reconstruction import extract

ROOT = Path(__file__).resolve().parents[1]
SOURCES = {
    'replay': ROOT/'src/carma2/newcommon/23-piping.c',
    'audio': ROOT/'src/carma2/s3/src/audio.c',
    'resource': ROOT/'src/carma2/s3/src/resource.c',
    'platform': ROOT/'src/carma2/platform/win32/win32_sound.c',
}
FUNCTIONS = dict(ARGoBackwards=(0x403d00,15), ReverseSound=(0x403d10,5),
    ARGoForwards=(0x403d20,9), S3ReverseSample=(0x564d4a,43),
    S3SetSampleFilterCallbacks=(0x564d75,107), S3ExecuteSampleFilterFuncs=(0x564de0,126),
    S3GetChannelForTag=(0x565888,124), S3MemAllocate=(0x565af7,38),
    S3MemFree=(0x565b1d,23), PDReverseAudio=(0x569401,262), memmove=(0x5790c0,0x340))
GLOBALS = dict(gS3_enabled=(0x7a06c0,'int'), gS3_state=(0x7a0580,'tS3_state'),
    gS3_effects_enabled=(0x7a0758,'int'),gS3_sample_filter_func=(0x7a0760,'tS3_sample_filter'),
    gS3_sample_filter_disable_func=(0x7a0764,'tS3_sample_filter'))
EXTERNALS = dict(BrMemAllocate=0x5275c0,BrMemFree=0x5275f0)
ADDRESSES = {**{n:a for n,(a,_) in FUNCTIONS.items()},**{n:a for n,(a,_) in GLOBALS.items()},**EXTERNALS}


def decode_cfg(cs, data, address, length):
    """Decode reachable instructions; never interpret embedded dispatch tables as code."""
    from capstone import CS_OP_IMM, CS_OP_MEM
    pending=[address]; decoded={}; tables={}
    while pending:
        pc=pending.pop()
        while address<=pc<address+length and pc not in decoded:
            ins=next(cs.disasm(raw(data,pc,min(15,address+length-pc)),pc),None)
            assert ins is not None,(hex(pc),'undecodable')
            decoded[pc]=ins
            if ins.mnemonic.startswith('ret'):break
            assert ins.mnemonic not in ('int3','ud2','int','hlt'),(hex(pc),ins.mnemonic)
            if ins.mnemonic.startswith('j') or ins.mnemonic.startswith('loop'):
                op=ins.operands[0]
                if op.type==CS_OP_IMM:
                    if address<=op.imm<address+length:pending.append(op.imm)
                else:
                    assert ins.mnemonic=='jmp' and op.type==CS_OP_MEM and not op.mem.base and op.mem.index and op.mem.scale==4,(hex(pc),ins.op_str)
                    # The CRT's biased tables intentionally omit alignment
                    # index zero and use negative indices for two dispatches.
                    # These ranges follow the preceding AND/subtract/NEG paths.
                    hints={0x60:range(1,4),0x158:range(-4,0),0xdc:range(8),
                        0x148:range(4),0x1e8:range(1,4),0x290:range(-7,1),0x2e0:range(4)}
                    indices=hints.get(op.mem.disp-address)
                    assert indices is not None,(hex(pc),ins.op_str)
                    targets={}
                    for i in indices:
                        v=struct.unpack('<I',raw(data,op.mem.disp+4*i,4))[0]
                        assert address<=v<address+length,(hex(pc),i,hex(v))
                        targets[i]=v
                    tables[op.mem.disp]=targets;pending.extend(targets.values())
                if ins.mnemonic=='jmp':break
            pc+=ins.size
    occupied=set()
    for ins in decoded.values():
        slots=set(range(ins.address,ins.address+ins.size));assert not occupied&slots
        occupied.update(slots)
    for a,targets in tables.items():
        assert not occupied&{a+4*i+j for i in targets for j in range(4)}
        assert all(v in decoded for v in targets.values())
    return sorted(decoded.values(),key=lambda i:i.address),tables


def audit(path,data,addresses,spans):
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM,CS_OP_REG
    cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
    code={a for a,_ in spans}|{addresses[n] for n in EXTERNALS}
    patches={};closure=[];all_tables=[]
    image_end=0x400000+struct.unpack_from('<I',data,struct.unpack_from('<I',data,60)[0]+24+56)[0]
    for address,length in spans:
        decoded,tables=decode_cfg(cs,data,address,length)
        starts={ins.address for ins in decoded}
        closure.append(dict(address=address,length=length,reachable_instructions=len(decoded),
            sha256=hashlib.sha256(raw(data,address,length)).hexdigest()))
        for a,targets in tables.items():
            all_tables.append(dict(address=a,targets=targets))
            for i,v in targets.items():patches[a+4*i-0x400000]=v
        for ins in decoded:
            if ins.mnemonic=='call' or ins.mnemonic.startswith('j') or ins.mnemonic.startswith('loop'):
                op=ins.operands[0]
                if op.type==CS_OP_IMM:
                    assert op.imm in code or (ins.mnemonic!='call' and op.imm in starts),(hex(ins.address),ins.op_str)
                elif ins.mnemonic=='jmp':assert op.type==CS_OP_MEM and op.mem.disp in tables
                else:
                    assert ins.mnemonic=='call' and (op.type==CS_OP_REG or op.type==CS_OP_MEM),(hex(ins.address),ins.op_str)
                    if op.type==CS_OP_MEM and op.mem.base:
                        assert op.mem.disp in (0x2c,0x4c),(hex(ins.address),ins.op_str)
            for op in ins.operands:
                if op.type==CS_OP_MEM and not op.mem.base and 0x400000<=op.mem.disp<image_end:
                    assert any(addresses[n]<=op.mem.disp<addresses[n]+(28 if n=='gS3_state' else 4) for n in GLOBALS) or op.mem.disp in tables,(hex(ins.address),ins.op_str)
                    assert ins.disp_size==4;patches[ins.address+ins.disp_offset-0x400000]=op.mem.disp
                elif op.type==CS_OP_IMM and ins.mnemonic!='call' and not ins.mnemonic.startswith(('j','loop')) and 0x400000<=op.imm<image_end:
                    assert op.imm in code,(hex(ins.address),ins.op_str)
                    assert ins.imm_size==4;patches[ins.address+ins.imm_offset-0x400000]=op.imm
    patches=sorted(patches.items())
    path.write_bytes(struct.pack('<'+str(len(ADDRESSES)+1)+'I',*[addresses[n] for n in ADDRESSES],len(patches))+b''.join(struct.pack('<2I',*v) for v in patches))
    return dict(closure=closure,jump_tables=all_tables,absolute_relocations=patches)


def definitions():
    ix={n:i for i,n in enumerate(ADDRESSES)}
    lines=[f'static unsigned addresses[{len(ix)}];','static unsigned __cdecl checked(unsigned,unsigned,unsigned,unsigned*,unsigned,unsigned);']
    for n,(_,t) in GLOBALS.items():lines.append(f'#define {n} (*({t}*)(uintptr_t)addresses[{ix[n]}])')
    for n in ('ARGoBackwards','ARGoForwards'):
        lines.append(f'int C2_HOOK_FASTCALL {n}(void){{return checked(addresses[{ix[n]}],0,0,NULL,0,0);}}')
    for n in ('ReverseSound','S3ReverseSample'):
        lines.append(f'void C2_HOOK_FASTCALL {n}(tS3_effect_tag e,tS3_sound_tag t){{checked(addresses[{ix[n]}],e,t,NULL,0,0);}}')
    for n in ('S3ExecuteSampleFilterFuncs','PDReverseAudio'):
        lines.append(f'int C2_HOOK_FASTCALL {n}(tS3_channel*p){{return checked(addresses[{ix[n]}],(unsigned)(uintptr_t)p,0,NULL,0,0);}}')
    lines.append(f'int C2_HOOK_FASTCALL S3SetSampleFilterCallbacks(tS3_sample_filter a,tS3_sample_filter b){{return checked(addresses[{ix["S3SetSampleFilterCallbacks"]}],(unsigned)(uintptr_t)a,(unsigned)(uintptr_t)b,NULL,0,0);}}')
    lines.append(f'#define REVERSE_ADDRESS addresses[{ix["ReverseSound"]}]')
    return '\n'.join(lines)


DRIVER=r'''
#define CINTERFACE
#define COBJMACROS
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "23-piping.h"
#include "win32/win32_sound.h"
#include <s3/internal/audio.h>
#include <s3/internal/resource.h>
#ifndef PHYSICAL
GLOBAL_DEFINITIONS
#define REVERSE_ADDRESS ((unsigned)(uintptr_t)ReverseSound)
#else
PHYSICAL_DEFINITIONS
#endif
static unsigned arena[0x3000/4],h[48];static unsigned events,callback_calls;
static LPVOID*held_data;static LPDWORD held_length;
static void C2_HOOK_FASTCALL filter1(tS3_effect_tag,tS3_sound_tag);
static void C2_HOOK_FASTCALL filter2(tS3_effect_tag,tS3_sound_tag);
static HRESULT WINAPI lock1(LPDIRECTSOUNDBUFFER,DWORD,DWORD,LPVOID*,LPDWORD,LPVOID*,LPDWORD,DWORD);
static HRESULT WINAPI unlock1(LPDIRECTSOUNDBUFFER,LPVOID,DWORD,LPVOID,DWORD);
static HRESULT WINAPI unlock2(LPDIRECTSOUNDBUFFER,LPVOID,DWORD,LPVOID,DWORD);
static unsigned rd(unsigned off){unsigned v;memcpy(&v,(char*)arena+off,4);return v;}
static void wr(unsigned off,unsigned v){memcpy((char*)arena+off,&v,4);}
static void* ptr(unsigned off){return off==0xffffffff?NULL:(char*)arena+off;}
static tS3_channel* channel(unsigned n){return (tS3_channel*)ptr(0x300+n*0x80);}
static unsigned norm(unsigned v){unsigned b=(unsigned)(uintptr_t)arena;
    if(v>=b&&v<=b+sizeof(arena))return v-b+0x1000000;
    if(v==(unsigned)(uintptr_t)filter1)return 0x2000010;
    if(v==(unsigned)(uintptr_t)filter2)return 0x2000020;
    if(v==REVERSE_ADDRESS)return 0x2000030;
    if(v==(unsigned)(uintptr_t)lock1)return 0x2000040;
    if(v==(unsigned)(uintptr_t)unlock1)return 0x2000050;
    if(v==(unsigned)(uintptr_t)unlock2)return 0x2000060;
    return v;
}
static tS3_sample_filter filter(unsigned i){return i==1?filter1:i==2?filter2:i==3?(tS3_sample_filter)(uintptr_t)REVERSE_ADDRESS:NULL;}
static void hex(const void*p,unsigned n){unsigned i;for(i=0;i<n;i++)printf("%02x",((const unsigned char*)p)[i]);}
static void snapshot(void){unsigned prefix[12],i,j,v;unsigned char copy[sizeof(arena)];
    const unsigned fields[]={0x110,0x114,0x118,0x11c,0x130,0x134,0x138,0x13c,0x150,0x154,0x158,0x15c,
        0x35c,0x360,0x364,0x368,0x36c,0x370,0x374,
        0x3dc,0x3e0,0x3e4,0x3e8,0x3ec,0x3f0,0x3f4,
        0x45c,0x460,0x464,0x468,0x46c,0x470,0x474,
        0x628,0x62c,0x634,0x640,0x644,0x648,0x678,0x67c,0x684,0x690,0x694,0x698,
        0x6c8,0x6cc,0x6d4,0x6e0,0x6e4,0x6e8,0x814,0x834,0x854,0x900,0x910,0x920};
    prefix[0]=gS3_enabled;prefix[1]=gS3_effects_enabled;prefix[2]=norm((unsigned)(uintptr_t)gS3_sample_filter_func);prefix[3]=norm((unsigned)(uintptr_t)gS3_sample_filter_disable_func);
    memcpy(prefix+4,&gS3_state,sizeof(gS3_state));for(i=7;i<11;i++)prefix[i]=norm(prefix[i]);prefix[11]=callback_calls;
    memcpy(copy,arena,sizeof(copy));
    for(i=0;i<sizeof(fields)/sizeof(fields[0]);i++){v=norm(rd(fields[i]));memcpy(copy+fields[i],&v,4);}
    for(j=0;j<2;j++)for(i=0;i<20;i++){unsigned off=0xa00+0x100*j+4*i;v=norm(rd(off));memcpy(copy+off,&v,4);}
    hex(prefix,sizeof(prefix));hex(copy,sizeof(copy));
}
static void event(int kind,unsigned a,unsigned b,unsigned c,unsigned d,unsigned e,unsigned f){
    if(events>80){exit(91);}printf("%s[\"%c\",%u,%u,%u,%u,%u,%u,\"",events++?",":"",kind,a,b,c,d,e,f);snapshot();printf("\"]");
}
static void mutate(void){tS3_channel*c=channel(h[17]);if(callback_calls++!=h[22])return;
    if(h[15]&1)c->descriptor=(tS3_descriptor*)ptr(0x600+0x50*h[23]);
    if(h[15]&2)gS3_effects_enabled=h[24];
    if(h[15]&4)c->tag=h[25];
    if(h[15]&8)gS3_sample_filter_disable_func=filter2;
    if(h[15]&16)gS3_sample_filter_func=filter2;
    if(h[15]&32)wr(0x910,(unsigned)(uintptr_t)ptr(0xb00));
    if(h[15]&64)wr(0x820,h[26]);
    if(h[15]&128)gS3_state.outlets=NULL;
    if(h[15]&256)wr(h[28],h[29]);
    if((h[15]&512)&&held_data!=NULL)*held_data=ptr(h[30]);
    if((h[15]&1024)&&held_length!=NULL)*held_length=h[31];
}
static void C2_HOOK_FASTCALL filter1(tS3_effect_tag e,tS3_sound_tag t){event('E',e,t,0,0,0,0);mutate();}
static void C2_HOOK_FASTCALL filter2(tS3_effect_tag e,tS3_sound_tag t){event('D',e,t,0,0,0,0);mutate();}
static HRESULT WINAPI lock1(LPDIRECTSOUNDBUFFER self,DWORD off,DWORD count,LPVOID*data,LPDWORD length,LPVOID*data2,LPDWORD length2,DWORD flags){
    event('L',norm((unsigned)(uintptr_t)self),off,count,flags,*data==NULL,(data2!=NULL)|((length2!=NULL)<<1));
    held_data=data;held_length=length;*data=ptr(h[18]);*length=h[2];mutate();return (HRESULT)h[3];
}
static HRESULT unlock_common(LPDIRECTSOUNDBUFFER self,LPVOID data,DWORD length,LPVOID data2,DWORD length2,int which){
    event(which==1?'U':'V',norm((unsigned)(uintptr_t)self),norm((unsigned)(uintptr_t)data),length,norm((unsigned)(uintptr_t)data2),length2,0);mutate();return (HRESULT)h[4];
}
static HRESULT WINAPI unlock1(LPDIRECTSOUNDBUFFER s,LPVOID p,DWORD n,LPVOID p2,DWORD n2){return unlock_common(s,p,n,p2,n2,1);}
static HRESULT WINAPI unlock2(LPDIRECTSOUNDBUFFER s,LPVOID p,DWORD n,LPVOID p2,DWORD n2){return unlock_common(s,p,n,p2,n2,2);}
void* C2_HOOK_CDECL BrMemAllocate(br_size_t n,br_uint_8 type){event('A',n,type,0,0,0,0);mutate();return h[5]?NULL:ptr(h[19]);}
void C2_HOOK_CDECL BrMemFree(void*p){event('F',norm((unsigned)(uintptr_t)p),0,0,0,0,0);mutate();}
#ifndef PHYSICAL
BODIES
#endif
static int run(int argc,char**argv){FILE*f;unsigned i,j,value=0;const unsigned channel_fields[]={0x5c,0x60,0x64,0x68,0x6c,0x70,0x74};
    if(sizeof(tS3_channel)!=0x78||sizeof(tS3_descriptor)!=0x4c||sizeof(tS3_state)!=28||sizeof(tS3_outlet)!=32||sizeof(tS3_buffer_desc)!=24)return 92;
    if(argc!=2){return 1;}SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);f=fopen(argv[1],"rb");if(!f)return 2;
    if(fread(h,sizeof(h),1,f)!=1||fread(arena,sizeof(arena),1,f)!=1){return 3;}fclose(f);_controlfp(_PC_53,_MCW_PC);
    gS3_enabled=h[6];gS3_effects_enabled=h[7];gS3_sample_filter_func=filter(h[11]);gS3_sample_filter_disable_func=filter(h[12]);
    memset(&gS3_state,0,sizeof(gS3_state));gS3_state.count_outlets=3;gS3_state.outlets=h[21]?NULL:(tS3_outlet*)ptr(0x100);gS3_state.descriptors=(tS3_descriptor*)ptr(0x600);gS3_state.root_descriptor=(tS3_descriptor*)ptr(0x600);
    for(i=0;i<3;i++){
        unsigned o=0x100+0x20*i,c=0x300+0x80*i,d=0x600+0x50*i,b=0x800+0x20*i;
        for(j=0x10;j<0x20;j+=4)wr(o+j,0);
        wr(o+0x10,(unsigned)(uintptr_t)ptr(0x300));wr(o+0x18,i<2?(unsigned)(uintptr_t)ptr(o+0x20):0);
        for(j=0;j<sizeof(channel_fields)/sizeof(channel_fields[0]);j++)wr(c+channel_fields[j],0);
        wr(c+0x60,(unsigned)(uintptr_t)ptr(d));wr(c+0x64,i<2?(unsigned)(uintptr_t)ptr(c+0x80):0);
        wr(d+0x28,0);wr(d+0x2c,i<2?(unsigned)(uintptr_t)ptr(d+0x50):0);wr(d+0x34,0);wr(d+0x40,(unsigned)(uintptr_t)ptr(b));wr(d+0x44,0);
        wr(d+0x48,h[20]?0:(unsigned)(uintptr_t)ptr(0x900+0x10*i));wr(b+0x14,0);
        wr(0x900+0x10*i,(unsigned)(uintptr_t)ptr(0xa00));
    }
    for(i=0;i<2;i++){for(j=0;j<20;j++)wr(0xa00+0x100*i+4*j,0);wr(0xa2c+0x100*i,(unsigned)(uintptr_t)lock1);wr(0xa4c+0x100*i,(unsigned)(uintptr_t)(i?unlock2:unlock1));}
    printf("{\"calls\":[");
    if(h[0]==0)value=ARGoBackwards();
    else if(h[0]==1)value=ARGoForwards();
    else if(h[0]==2)value=S3SetSampleFilterCallbacks(filter(h[13]),filter(h[14]));
    else if(h[0]==3)S3ReverseSample(h[10],h[9]);
    else if(h[0]==4)value=PDReverseAudio(channel(h[17]));
    else if(h[0]==5)value=S3ExecuteSampleFilterFuncs(channel(h[17]));
    else if(h[0]==6){value=ARGoBackwards();event('B',value,0,0,0,0,0);value=S3ExecuteSampleFilterFuncs(channel(h[17]));event('X',value,0,0,0,0,0);value=S3ExecuteSampleFilterFuncs(channel(h[17]));event('X',value,0,0,0,0,0);value=ARGoForwards();event('G',value,0,0,0,0,0);value=S3ExecuteSampleFilterFuncs(channel(h[17]));event('X',value,0,0,0,0,0);value=S3ExecuteSampleFilterFuncs(channel(h[17]));}
    else if(h[0]==7)ReverseSound(h[10],h[9]);
    printf("],\"state\":\"");snapshot();printf("\",\"value\":%u}\n",value);return 0;
}
'''


def fixtures():
    cases=[dict(mode=i) for i in range(8)]
    for before in range(4):
        for enable in range(4):
            for disable in range(4):cases.append(dict(mode=2,enable_before=before,enable=enable,disable=disable))
    for effects in (0,1,2,3,0x80000000,0xffffffff):
        for descriptor in (0,1,2,3,0x80000000,0xffffffff):
            cases.append(dict(mode=5,effects=effects,descriptor=descriptor))
    for mode in (3,4,6,7):
        for size in (0,1,2,3,4,7,8,15,16,31,32,63,64,127,128,255,256):
            cases.append(dict(mode=mode,size=size))
        for status in (1,2,0x80004005,0xffffffff):cases.append(dict(mode=mode,lock=status))
        for size in (0,1,128,1999999,2000000,2000001,0x7fffffff,0x80000000,0xffffffff):
            cases.append(dict(mode=mode,size=size,alloc_fail=1))
        cases.extend([dict(mode=mode,null_handle=1),dict(mode=mode,unlock=0x80004005)])
    for tag in (0,1,2,3,0x101,0x102,0x103,0x202,0x80000202,0xffffff02,0xffffffff):
        cases.append(dict(mode=3,tag=tag));cases.append(dict(mode=7,tag=tag,enabled=0))
    for mode in (3,7):
        for effect in (0,1,2,0xffffffff,0x80000000):cases.append(dict(mode=mode,effect=effect))
        cases.append(dict(mode=mode,no_outlets=1))
    for mode in (4,6):
        for size in (1,4,7,16,63,128):
            for length in (0,1,size//2,size):cases.append(dict(mode=mode,size=size,length=length))
        for delta in (-17,-4,-1,0,1,4,17):
            for align in range(4):cases.append(dict(mode=mode,size=63,data=0x1100+align,temp=0x1100+align+delta))
    for mode in (4,5,6):
        for mutation in (1,2,4,8,16,32,64,128,256,1|2|4|8|16|32|64):
            for at in (0,1,2):cases.append(dict(mode=mode,mutate=mutation,at=at))
    for mode in (4,6):
        for mutation in (512,1024,512|1024):
            for at in (0,1,2):cases.append(dict(mode=mode,mutate=mutation,at=at))
    return cases


def fixture(case):
    size=case.get('size',32)
    h=[case['mode'],size,case.get('length',min(size,256)),case.get('lock',0),case.get('unlock',0),case.get('alloc_fail',0),case.get('enabled',1),case.get('effects',1),case.get('descriptor',0),case.get('tag',0x202),case.get('effect',1),case.get('enable_before',1),2,case.get('enable',1),case.get('disable',0),case.get('mutate',0),0,1,case.get('data',0x1000),case.get('temp',0x2000),case.get('null_handle',0),case.get('no_outlets',0),case.get('at',0),2,3,0x303,77,0,0x1600,0x11223344,0x1200,16]+[0]*16
    arena=bytearray((i*197+43)&255 for i in range(0x3000))
    def wr(off,v):struct.pack_into('<I',arena,off,v&0xffffffff)
    for i in range(3):
        wr(0x100+0x20*i,i+1);wr(0x31c+0x80*i,(i+1)*0x101)
        wr(0x638+0x50*i,h[8]);wr(0x800+0x20*i,size)
    return struct.pack('<48I',*[v&0xffffffff for v in h])+arena


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--output',type=Path,default=ROOT/'build-audit/replay-audio-tests')
    p.add_argument('--case-start',type=int,default=0);p.add_argument('--case-limit',type=int)
    p.add_argument('--recompiled',type=Path,default=ROOT/'build-matching/rec2.exe');p.add_argument('--symbols',type=Path)
    p.add_argument('--map-file',type=Path);p.add_argument('--pd-source',type=Path)
    args=p.parse_args();out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)
    original=ROOT/'build-reference/originals/CARMA2_HW.EXE';data=original.read_bytes()
    assert hashlib.sha256(data).hexdigest()==ORIGINAL_HASH
    original_meta=audit(out/'original-addresses.bin',data,ADDRESSES,list(FUNCTIONS.values()))
    linked_meta=None;map_file=args.map_file or args.recompiled.with_suffix('.map')
    if args.symbols:
        mapping=json.loads(args.symbols.read_text());rebuilt=args.recompiled.read_bytes()
        assert mapping['format']==1 and mapping['original_sha256']==ORIGINAL_HASH
        assert mapping['recompiled_sha256']==hashlib.sha256(rebuilt).hexdigest()
        syms={s['orig']:s for s in mapping['symbols']}
        map_symbols=re.findall(r'^\s*[0-9a-fA-F]{4}:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\b',map_file.read_text(),re.M)
        moves=[int(a,16) for n,a in map_symbols if n=='_memmove'];assert len(moves)==1
        linked={n:(moves[0] if n=='memmove' else syms[a]['recomp']) for n,a in ADDRESSES.items()}
        known={int(a,16) for _,a in map_symbols}|{s['recomp'] for s in mapping['symbols']}
        spans=[(linked[n],min(a for a in known if a>linked[n])-linked[n]) for n in FUNCTIONS]
        from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM
        cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
        pending=list(spans);seen={a for a,_ in spans}
        while pending:
            a,n=pending.pop()
            for ins in decode_cfg(cs,rebuilt,a,n)[0]:
                if ins.mnemonic=='call' and ins.operands[0].type==CS_OP_IMM:
                    target=ins.operands[0].imm
                    if target in seen or target in {linked[n] for n in EXTERNALS}:continue
                    assert target in known,(hex(ins.address),ins.op_str)
                    extra=(target,min(v for v in known if v>target)-target)
                    spans.append(extra);pending.append(extra);seen.add(target)
        linked_meta=audit(out/'linked-addresses.bin',rebuilt,linked,spans)
    body_names=dict(replay=['ARGoBackwards','ARGoForwards','ReverseSound'],audio=['S3ReverseSample','S3SetSampleFilterCallbacks','S3ExecuteSampleFilterFuncs','S3GetChannelForTag'],resource=['S3MemAllocate','S3MemFree'],platform=['PDReverseAudio'])
    bodies=[extract((args.pd_source if args.pd_source and k=='platform' else SOURCES[k]).read_text(),n) for k,names in body_names.items() for n in names]
    declarations='\n'.join(t+' '+n+';' for n,(_,t) in GLOBALS.items())
    driver=DRIVER.replace('GLOBAL_DEFINITIONS',declarations).replace('PHYSICAL_DEFINITIONS',definitions()).replace('BODIES','\n\n'.join(bodies))
    (out/'harness.c').write_text(driver+'\nint C2_HOOK_CDECL main(int argc,char**argv){return run(argc,argv);}\n')
    patches=['{unsigned char*site=(unsigned char*)(uintptr_t)addresses[%d];site[0]=0xe9;value=(unsigned)(uintptr_t)%s-((unsigned)(uintptr_t)site+5);memcpy(site+1,&value,4);}'%(list(ADDRESSES).index(n),n) for n in EXTERNALS]
    old='{unsigned char*site=(unsigned char*)(uintptr_t)addresses[FATAL_INDEX];site[0]=0xe9;value=(unsigned)(uintptr_t)FatalError-((unsigned)(uintptr_t)site+5);memcpy(site+1,&value,4);}'
    assert old in PHYSICAL_LOADER
    loader=PHYSICAL_LOADER.replace(old,'\n'.join(patches)).replace('Only FatalError is replaced; all internal replay bodies remain intact.','Only BrMemAllocate/BrMemFree are replaced; internal audio bodies remain intact.')
    (out/'physical.c').write_text('#define PHYSICAL\n'+driver+loader)
    includes=[ROOT/r for r in ['src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/carma2/platform','src/carma2/s3/include','src/brender/include']]
    msvc=sorted((ROOT/'build-tools/msvc-modern/VC/Tools/MSVC').iterdir())[-1]
    sdk=ROOT/'build-tools/msvc-modern/Windows Kits/10';version=sorted(p.name for p in (sdk/'Include').iterdir())[-1]
    env=dict(os.environ);env['PATH']=str(msvc/'bin/Hostx64/x86')+os.pathsep+env['PATH']
    env['INCLUDE']=os.pathsep.join(map(str,[msvc/'include',*[sdk/f'Include/{version}/{r}' for r in ['ucrt','shared','um']]]));env['LIB']=os.pathsep.join(map(str,[msvc/'lib/x86',*[sdk/f'Lib/{version}/{r}/x86' for r in ['ucrt','um']]]))
    old=ROOT/'build-tools/msvc500sp3';oldenv=dict(os.environ);oldenv['PATH']=str(old/'bin')+os.pathsep+str(old/'redist')+os.pathsep+oldenv['PATH'];oldenv['INCLUDE']=str(old/'include')+os.pathsep+str(ROOT/'external/dxsdk/dxsdk/include');oldenv['LIB']=str(old/'lib')
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
                path=out/f'failure-{compiler}-{index}.json';path.write_text(json.dumps(dict(case=case,original=expected,actual=actual),indent=2))
                (out/f'failure-{compiler}-{index}.bin').write_bytes(inp.read_bytes())
                raise AssertionError(f'{compiler} case {index} differs: {path}')
            results.append(dict(index=index,case=case,compiler=compiler,passed=True,calls=len(actual['calls']),state_sha256=hashlib.sha256(bytes.fromhex(actual['state'])).hexdigest()))
    (out/'verification.json').write_text(json.dumps(dict(scenario_count=len(cases),comparisons=len(results),original_sha256=ORIGINAL_HASH,sources_sha256={str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in SOURCES.values()},verifier_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),bodies_sha256=[hashlib.sha256(b.encode()).hexdigest() for b in bodies],compiler_commands=commands,original=original_meta,linked=linked_meta,recompiled_sha256=hashlib.sha256(args.recompiled.read_bytes()).hexdigest() if args.symbols else None,symbols_sha256=hashlib.sha256(args.symbols.read_bytes()).hexdigest() if args.symbols else None,map_sha256=hashlib.sha256(map_file.read_bytes()).hexdigest() if args.symbols else None,pd_source=str(args.pd_source) if args.pd_source else None,cases=results,limitations=__doc__),indent=2)+'\n')
    print(f'PASS: {len(cases)} scenarios, {len(results)} complete replay-audio comparisons.')


if __name__=='__main__':main()
