#!/usr/bin/env python3
"""Exercise replay searches against physical original x86 bodies.

Original and linked bodies execute in relocated private images, after a complete
direct-call/data-reference audit. Only FatalError and caller-provided callbacks
are controlled boundaries. Compare complete arenas, globals, callback order and
arguments, returned pointers, ESP, callee-saved registers and x87 TOP. Fixtures
use masked x87 exceptions and PC53; exception flags are outside this comparison.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import struct
import subprocess
import sys

from verify_replay_session_reconstruction import raw, PHYSICAL_LOADER, ORIGINAL_HASH
from verify_tinted_native_reconstruction import extract

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / 'src/carma2/newcommon/23-piping.c'
FUNCTIONS = dict(LengthOfChunk=(0x403720,48), MoveSessionPointerBackOne=(0x4037c0,60),
    FindNextChunk=(0x4030f0,279), FindPreviousChunk=(0x403800,388),
    ARScanBuffer=(0x402e10,524), ARReplayForwards=(0x402300,93))
GLOBALS = dict(gPipe_buffer_start=(0x676908,'tU8*'),
    gPipe_buffer_working_end=(0x6768c4,'tU8*'),gPipe_buffer_oldest=(0x6768ac,'tU8*'),
    gPipe_record_ptr=(0x6768bc,'tU8*'),gPipe_play_ptr=(0x6768b8,'tU8*'),
    gPipe_chunk_end=(0x6768c8,'tU8*'),gPipe_callbacks=(0x6768e8,'const tReplay_callback*'),
    gReplay_rate=(0x676900,'float'),gPlay_direction=(0x6768c0,'int'),
    gDisable_advance=(0x67690c,'int'),gMax_previous_pipe_sessions=(0x58f1e8,'int'))
ADDRESSES = {**{n:a for n,(a,_) in FUNCTIONS.items()},**{n:a for n,(a,_) in GLOBALS.items()},'FatalError':0x44c230}


def audit(path, data, addresses, spans):
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM,CS_OP_REG
    cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
    patches={};closure=[];code={a for a,_ in spans}|{addresses['FatalError']}
    for address,length in spans:
        body=raw(data,address,length);decoded=list(cs.disasm(body,address))
        assert sum(i.size for i in decoded)==length
        closure.append(dict(address=address,length=length,sha256=hashlib.sha256(body).hexdigest()))
        for ins in decoded:
            if ins.mnemonic=='call' or ins.mnemonic.startswith('j'):
                op=ins.operands[0]
                if op.type==CS_OP_IMM:
                    assert op.imm in code or (ins.mnemonic.startswith('j') and address<=op.imm<address+length),(hex(ins.address),ins.op_str)
                else:
                    assert ins.mnemonic=='call' and (op.type==CS_OP_REG or (op.type==CS_OP_MEM and op.mem.base)),(hex(ins.address),ins.op_str)
            for op in ins.operands:
                if op.type==CS_OP_MEM and not op.mem.base and op.mem.disp>=0x400000:
                    allowed=any(addresses[n]<=op.mem.disp<addresses[n]+4 for n in GLOBALS)
                    if not allowed:
                        assert ins.mnemonic in ('fld','fcomp','fcom') and raw(data,op.mem.disp,op.size)==bytes(op.size),(hex(ins.address),ins.op_str)
                    assert ins.disp_size==4
                    patches[ins.address+ins.disp_offset-0x400000]=op.mem.disp
    patches=sorted(patches.items())
    path.write_bytes(struct.pack('<'+str(len(ADDRESSES)+1)+'I',*[addresses[n] for n in ADDRESSES],len(patches))+b''.join(struct.pack('<2I',*v) for v in patches))
    return dict(closure=closure,absolute_data_relocations=patches)


def definitions():
    ix={n:i for i,n in enumerate(ADDRESSES)}
    lines=[f'static unsigned addresses[{len(ix)}];','static unsigned __cdecl checked(unsigned,unsigned,unsigned,unsigned*,unsigned,unsigned);']
    for n,(_,t) in GLOBALS.items():lines.append(f'#define {n} (*({t}*)(uintptr_t)addresses[{ix[n]}])')
    lines += [
        f'int C2_HOOK_FASTCALL LengthOfChunk(void*p,int t){{return checked(addresses[{ix["LengthOfChunk"]}],(unsigned)(uintptr_t)p,t,NULL,0,0);}}',
        f'int C2_HOOK_FASTCALL MoveSessionPointerBackOne(tU8**p){{return checked(addresses[{ix["MoveSessionPointerBackOne"]}],(unsigned)(uintptr_t)p,0,NULL,0,0);}}',
        f'tPipe_chunk_data* C2_HOOK_FASTCALL FindNextChunk(tPipe_chunk_type t,uintptr_t o){{return (tPipe_chunk_data*)(uintptr_t)checked(addresses[{ix["FindNextChunk"]}],t,o,NULL,0,0);}}',
        f'tPipe_chunk_data* C2_HOOK_FASTCALL FindPreviousChunk(tU8*p,int t,uintptr_t o){{unsigned w=o;return (tPipe_chunk_data*)(uintptr_t)checked(addresses[{ix["FindPreviousChunk"]}],(unsigned)(uintptr_t)p,t,&w,1,0);}}',
        f'void C2_HOOK_FASTCALL ARScanBuffer(tU8**p,tPipe_chunk_type t,tU32 time,tARScanBuffer_callback*cb,tARScanBuffer_time_check*tc){{unsigned w[3];w[0]=time;w[1]=(unsigned)(uintptr_t)cb;w[2]=(unsigned)(uintptr_t)tc;checked(addresses[{ix["ARScanBuffer"]}],(unsigned)(uintptr_t)p,t,w,3,0);}}']
    return '\n'.join(lines)


DRIVER=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "23-piping.h"
#include "52-errors.h"
#ifndef PHYSICAL
GLOBAL_DEFINITIONS
#else
PHYSICAL_DEFINITIONS
#endif
static unsigned arena[0x8000/4],h[32];static tU8*cursor;static int events,calls,time_calls;
static int C2_HOOK_FASTCALL length1(tPipe_chunk*p);
static int C2_HOOK_FASTCALL length2(tPipe_chunk*p);
static unsigned norm(unsigned v){unsigned b=(unsigned)(uintptr_t)arena;if(v>=b&&v<=b+sizeof(arena))return v-b+0x1000000;if(v==(unsigned)(uintptr_t)length1)return 0x2000010;if(v==(unsigned)(uintptr_t)length2)return 0x2000020;return v;}
static unsigned rd(unsigned off){unsigned v;memcpy(&v,(char*)arena+off,4);return v;}
static void wr(unsigned off,unsigned v){memcpy((char*)arena+off,&v,4);}
static tU8*ptr(unsigned off){return off==0xffffffff?NULL:(tU8*)arena+off;}
static void hex(const void*p,unsigned n){unsigned i;for(i=0;i<n;i++)printf("%02x",((const unsigned char*)p)[i]);}
static void snapshot(void){unsigned char copy[sizeof(arena)];unsigned prefix[12],i,j,v;
    prefix[0]=norm((unsigned)(uintptr_t)gPipe_buffer_start);prefix[1]=norm((unsigned)(uintptr_t)gPipe_buffer_working_end);prefix[2]=norm((unsigned)(uintptr_t)gPipe_buffer_oldest);prefix[3]=norm((unsigned)(uintptr_t)gPipe_record_ptr);prefix[4]=norm((unsigned)(uintptr_t)gPipe_play_ptr);prefix[5]=norm((unsigned)(uintptr_t)gPipe_chunk_end);prefix[6]=norm((unsigned)(uintptr_t)gPipe_callbacks);memcpy(prefix+7,&gReplay_rate,4);prefix[8]=gPlay_direction;prefix[9]=gDisable_advance;prefix[10]=gMax_previous_pipe_sessions;prefix[11]=norm((unsigned)(uintptr_t)cursor);
    memcpy(copy,arena,sizeof(copy));for(j=0;j<2;j++)for(i=0;i<71;i++){v=norm(rd(0x6000+j*0x1000+24*i+8));memcpy(copy+0x6000+j*0x1000+24*i+8,&v,4);}hex(prefix,sizeof(prefix));hex(copy,sizeof(copy));
}
static void mutate(void){if((unsigned)calls++!=h[15])return;
    if(h[14]&1)gPipe_callbacks=(tReplay_callback*)((char*)arena+0x7000);
    if(h[14]&2){wr(h[21]+4,1);gPipe_record_ptr=ptr(h[21]+22);}
    if(h[14]&4)wr(h[21],2);
    if(h[14]&8){if(h[19]==1)gPipe_play_ptr=ptr(h[24]);else if(h[19]==2)gPipe_chunk_end=ptr(h[24]);else cursor=ptr(h[24]);}
    if(h[14]&16)gPipe_chunk_end=h[27]?(tU8*)(uintptr_t)h[25]:ptr(h[25]);
    if(h[14]&32)gDisable_advance=1;
    if(h[14]&64){gReplay_rate=-1;gPlay_direction=-1;}
    if(h[14]&128)gMax_previous_pipe_sessions=1;
    if(h[14]&256)gPipe_record_ptr=ptr(h[24]);
}
static void event(char kind,unsigned p,unsigned n,unsigned time){if(events>200)exit(91);printf("%s[\"%c\",%u,%u,%u,\"",events++?",":"",kind,p,n,time);snapshot();printf("\"]");}
static int length(tPipe_chunk*p,int which){event('L',norm((unsigned)(uintptr_t)p),which,0);mutate();return h[23];}
static int C2_HOOK_FASTCALL length1(tPipe_chunk*p){return length(p,1);}
static int C2_HOOK_FASTCALL length2(tPipe_chunk*p){return length(p,2);}
static int C2_HOOK_FASTCALL scan(tPipe_chunk*p,int n,tU32 time){event('S',norm((unsigned)(uintptr_t)p),n,time);mutate();return h[16];}
static int C2_HOOK_FASTCALL time_check(tU32 time){event('T',0,0,time);mutate();return ++time_calls<(int)h[17];}
void C2_NORETURN C2_HOOK_CDECL FatalError(int index,...){event('F',index,0,0);printf("],\"state\":\"");snapshot();printf("\",\"value\":null,\"fatal\":%d}\n",index);exit(0);}
#ifndef PHYSICAL
BODIES
#endif
static int run(int argc,char**argv){FILE*f;unsigned i,j,value=0;tU8**cursor_ptr;if(argc!=2)return 1;f=fopen(argv[1],"rb");if(!f)return 2;if(fread(h,sizeof(h),1,f)!=1||fread(arena,sizeof(arena),1,f)!=1)return 3;fclose(f);_controlfp(_PC_53,_MCW_PC);
    cursor=ptr(h[3]);gPipe_play_ptr=ptr(h[4]);gPipe_record_ptr=ptr(h[5]);gPipe_buffer_oldest=ptr(h[6]);gPipe_buffer_working_end=ptr(h[7]);gPipe_buffer_start=ptr(h[8]);gMax_previous_pipe_sessions=(int)h[9];memcpy(&gReplay_rate,h+10,4);gPlay_direction=(int)h[11];gDisable_advance=h[12];gPipe_chunk_end=ptr(h[22]);gPipe_callbacks=(tReplay_callback*)((char*)arena+0x6000);
    for(j=0;j<2;j++)for(i=0;i<71;i++){unsigned off=0x6000+0x1000*j+24*i+8,v=rd(off);if(v==0x2000010)wr(off,(unsigned)(uintptr_t)length1);else if(v==0x2000020)wr(off,(unsigned)(uintptr_t)length2);}
    cursor_ptr=h[19]==1?&gPipe_play_ptr:h[19]==2?&gPipe_chunk_end:&cursor;
    printf("{\"calls\":[");
    if(h[0]==0)value=LengthOfChunk(ptr(h[20]),h[1]);
    else if(h[0]==1)value=MoveSessionPointerBackOne(cursor_ptr);
    else if(h[0]==2)value=norm((unsigned)(uintptr_t)FindNextChunk((tPipe_chunk_type)h[1],h[2]));
    else if(h[0]==3)value=norm((unsigned)(uintptr_t)FindPreviousChunk(cursor,h[1],h[2]));
    else if(h[0]==4)ARScanBuffer(cursor_ptr,(tPipe_chunk_type)h[1],h[18],scan,h[17]?time_check:NULL);
    printf("],\"state\":\"");snapshot();printf("\",\"value\":%u,\"fatal\":null}\n",value);return 0;
}
'''


def fixtures():
    cases=[]
    for mode in range(5):
        for cb in (0,1):
            for count in (0,1,2,3):
                for owner in (0x12345001,0x12345002,0xabcd5001,0xffffffff):
                    cases.append(dict(mode=mode,callback=cb,count=count,owner=owner))
    for mode in (1,2,3,4):
        for layout in ('wrap','record-at-end','oldest','oldest-record','single'):
            for cb in (0,1):cases.append(dict(mode=mode,callback=cb,layout=layout))
    for mode in (0,2,3,4):
        for mutate in (1,2,4,3,5,16,32,64,128,256):
            for at in (0,1,3):cases.append(dict(mode=mode,callback=1,mutate=mutate,at=at,owner=0xffffffff))
    for rate in (0,0x80000000,1,0x80000001,0x3f800000,0xbf800000,0x7fc12345,0x7f800001,0x7f800000,0xff800000):
        for direction in (-1,0,1):cases.append(dict(mode=4,rate=rate,direction=direction))
    for limit in (-1,0,1,2,3,1000):
        for disabled in (0,1):cases.append(dict(mode=3,limit=limit,disabled=disabled,owner=0xffffffff))
    for mode in (1,4):
        for alias in (1,2):
            for cb in (0,1):cases.append(dict(mode=mode,alias=alias,callback=cb,timecheck=1 if alias==2 else 10))
    for mode in (0,2,3,4):cases.append(dict(mode=mode,odd=True,callback=1,count=1,target=3,owner=0xffffffff))
    for mode in (2,3,4):
        for target in (0,1,2,3):cases.append(dict(mode=mode,target=target))
    for stop in (0,1):
        for timecheck in (0,1,2,10):cases.append(dict(mode=4,stop=stop,timecheck=timecheck))
    for at in (0,1,3):cases.append(dict(mode=4,callback=1,mutate=8,at=at))
    for mode in (0,2,3,4):cases.append(dict(mode=mode,callback=1,mutate=1,table_delta=2,owner=0xffffffff))
    cases.append(dict(mode=3,callback=1,mutate=16,at=2,target=2,limit=1,high_end=True,owner=0xffffffff))
    return cases


def fixture(case):
    arena=bytearray(b'\xa5'*0x8000)
    def put(off,v):struct.pack_into('<I',arena,off,v&0xffffffff)
    count=case.get('count',2);cb=case.get('callback',0);extra=4 if cb else 0
    positions=[];at=0x1000;stride=12+case.get('table_delta',0)
    for session_type in (1,0,1,2,1):
        n=1 if session_type==0 else count
        positions.append(at);put(at,session_type);put(at+4,n)
        for j in range(n):put(at+8+stride*j,0x12345001+j);put(at+12+stride*j,0xfedc0000+len(positions));put(at+16+stride*j,0x12345678)
        size=10+stride*n;struct.pack_into('<H',arena,at+size-2,size-2);at+=size
    positions.append(at)
    mode=case['mode'];cursor=positions[0] if mode==4 else positions[4]
    play=positions[2] if mode==4 else positions[0]
    oldest=positions[0];record=positions[5];start=positions[0];working=record
    layout=case.get('layout','linear')
    if layout=='wrap':cursor=positions[0];play=positions[4];oldest=positions[3];record=positions[2]
    elif layout=='record-at-end':cursor=record;play=record if mode==4 else positions[4]
    elif layout=='oldest':cursor=oldest;play=oldest
    elif layout=='oldest-record':cursor=oldest;record=oldest;play=oldest
    elif layout=='single':cursor=positions[1];record=positions[1];play=positions[0]
    for table in range(2):
        for typ in range(71):
            off=0x6000+0x1000*table+24*typ;arena[off:off+24]=bytes(24);put(off+4,8-extra+table*case.get('table_delta',0));put(off+8,(0x2000010+16*table) if cb else 0)
    # Callback results are real additions to the entry's live fixed length.
    if case.get('odd'):extra+=1
    h=[mode,case.get('target',1),case.get('owner',0x12345002),cursor,play,record,oldest,working,start,case.get('limit',1000),case.get('rate',0x3f800000),case.get('direction',1),case.get('disabled',0),cb,case.get('mutate',0),case.get('at',0),case.get('stop',0),case.get('timecheck',10),0x87654321,case.get('alias',0),positions[2]+8,positions[2],positions[0],extra,positions[2],positions[2]+8]+[0]*6
    # Alias the scan cursor to initialized public globals intentionally.
    if h[19]==1:h[4]=h[3]
    if h[19]==2:h[22]=h[3]
    if case.get('high_end'):h[25]=0x80001000;h[27]=1
    return struct.pack('<32I',*[v&0xffffffff for v in h])+arena


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE')
    p.add_argument('--recompiled',type=Path,default=ROOT/'build-matching/rec2.exe')
    p.add_argument('--symbols',type=Path)
    p.add_argument('--output',type=Path,default=ROOT/'build-audit/replay-search-tests')
    p.add_argument('--case-limit',type=int)
    p.add_argument('--case-start',type=int,default=0)
    args=p.parse_args();out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)
    original=args.original.read_bytes();assert hashlib.sha256(original).hexdigest()==ORIGINAL_HASH
    original_meta=audit(out/'original-addresses.bin',original,ADDRESSES,list(FUNCTIONS.values()))
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
            a,length=pending.pop()
            for ins in cs.disasm(raw(rebuilt,a,length),a):
                if ins.mnemonic=='call' and ins.operands[0].type==CS_OP_IMM:
                    target=ins.operands[0].imm
                    if target in seen or target==linked['FatalError']:continue
                    assert target in known,(hex(ins.address),ins.op_str)
                    extra=(target,min(a for a in known if a>target)-target);seen.add(target);spans.append(extra);pending.append(extra)
        linked_meta=audit(out/'linked-addresses.bin',rebuilt,linked,spans)
    source=SOURCE.read_text();names=['ReplaySearchSessionLength',*FUNCTIONS];bodies=[extract(source,n) for n in names]
    declarations='\n'.join(t+' '+n+';' for n,(_,t) in GLOBALS.items())
    driver=DRIVER.replace('GLOBAL_DEFINITIONS',declarations).replace('PHYSICAL_DEFINITIONS',definitions()).replace('BODIES','\n\n'.join(bodies))
    (out/'harness.c').write_text(driver+'\nint C2_HOOK_CDECL main(int argc,char**argv){return run(argc,argv);}\n')
    (out/'physical.c').write_text('#define PHYSICAL\n'+driver+PHYSICAL_LOADER.replace('FATAL_INDEX',str(list(ADDRESSES).index('FatalError'))))
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
        inp=out/'fixture.bin';inp.write_bytes(fixture(case))
        (out/'current-case.json').write_text(json.dumps(dict(index=index,case=case)))
        expected=json.loads(subprocess.check_output([str(out/'physical.exe'),str(inp),str(args.original.resolve()),str(out/'original-addresses.bin')],text=True,timeout=15))
        checks=[]
        if args.symbols:checks.append(('matching-physical',json.loads(subprocess.check_output([str(out/'physical.exe'),str(inp),str(args.recompiled.resolve()),str(out/'linked-addresses.bin')],text=True,timeout=15))))
        for compiler in ('msvc','msvc5','mingw'):checks.append((compiler,json.loads(subprocess.check_output([str(out/(compiler+'.exe')),str(inp)],text=True,timeout=15))))
        for compiler,actual in checks:
            if actual!=expected:
                path=out/f'failure-{compiler}-{index}.json';path.write_text(json.dumps(dict(case=case,original=expected,actual=actual),indent=2));raise AssertionError(f'{compiler} case {index} differs: {path}')
            results.append(dict(case=case,compiler=compiler,passed=True,calls=len(actual['calls']),fatal=actual['fatal'],state_sha256=hashlib.sha256(bytes.fromhex(actual['state'])).hexdigest()))
    (out/'verification.json').write_text(json.dumps(dict(scenario_count=len(cases),comparisons=len(results),original_sha256=ORIGINAL_HASH,source_sha256=hashlib.sha256(SOURCE.read_bytes()).hexdigest(),header_sha256=hashlib.sha256(SOURCE.with_suffix('.h').read_bytes()).hexdigest(),verifier_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),bodies_sha256=[hashlib.sha256(b.encode()).hexdigest() for b in bodies],compiler_commands=commands,original=original_meta,linked=linked_meta,recompiled_sha256=hashlib.sha256(args.recompiled.read_bytes()).hexdigest() if args.symbols else None,cases=results,limitations=__doc__),indent=2)+'\n')
    print(f'PASS: {len(cases)} scenarios, {len(results)} complete replay-search comparisons.')


if __name__=='__main__':main()
