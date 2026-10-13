#!/usr/bin/env python3
"""Compare real replay-recording bodies with original x86 and three native builds.

No internal recording functions are mocked. Fixtures compare complete local/ring
memory, pointer/count globals, callback packet bytes, and callback snapshots.
FatalError and variable-length callbacks are the only controlled boundaries.
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
SOURCE=ROOT/'src/carma2/newcommon/23-piping.c'
ORIGINAL_HASH='9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
ADDRESSES=dict(ARStartPipingSession=0x4025e0,EndPipingSession2=0x402660,
    AREndPipingSession=0x402890,ARAddDataToSession=0x4028a0,
    ARAddVariedDataToSession=0x402930,LengthOfSession=0x403750,
    AddSmokeToPipingSession=0x4c7010,gPipe_buffer_start=0x676908,
    gReentrancy_count=0x6768e4,gReentrancy_array=0x6768d0,
    gLocal_buffer=0x6768b4,gLocal_buffer_size=0x6768f4,gMr_chunky=0x6768a0,
    gPipe_callbacks=0x6768e8,gPipe_buffer_phys_end=0x6768f0,
    gPipe_buffer_working_end=0x6768c4,gPipe_buffer_oldest=0x6768ac,
    gPipe_record_ptr=0x6768bc,gAction_replay_mode=0x676914,
    gProgram_state=0x75bb80,FatalError=0x44c230)
POINTERS=['gPipe_buffer_start','gLocal_buffer','gMr_chunky','gPipe_callbacks',
    'gPipe_buffer_phys_end','gPipe_buffer_working_end','gPipe_buffer_oldest','gPipe_record_ptr']
HEAP,STACK,STOP,CB1,CB2,SIZE=0x1000000,0x1080000,0x1081000,0x1082000,0x1082010,0x11000


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE')
    p.add_argument('--recompiled',type=Path,default=ROOT/'build-matching/rec2.exe')
    p.add_argument('--symbols',type=Path)
    p.add_argument('--output',type=Path,default=ROOT/'build-audit/replay-record-tests')
    p.add_argument('--dependency-dir',type=Path,default=ROOT/'build-audit/joystick/python-deps')
    p.add_argument('--gcc',type=Path,default=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe')
    p.add_argument('--msvc-root',type=Path,default=ROOT/'build-tools/msvc-modern')
    p.add_argument('--msvc5-root',type=Path,default=ROOT/'build-tools/msvc500sp3')
    args=p.parse_args();sys.path.insert(0,str(args.dependency_dir.resolve()))
    from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
    from unicorn.x86_const import UC_X86_REG_EIP,UC_X86_REG_ESP,UC_X86_REG_EAX,UC_X86_REG_ECX,UC_X86_REG_EDX,UC_X86_REG_FPCW
    from verify_tinted_native_reconstruction import extract
    original=args.original.read_bytes();assert hashlib.sha256(original).hexdigest()==ORIGINAL_HASH
    images=[(original,ADDRESSES)]
    if args.symbols:
        mapping=json.loads(args.symbols.read_text());rebuilt=args.recompiled.read_bytes()
        assert mapping['format']==1 and mapping['original_sha256']==ORIGINAL_HASH
        assert mapping['recompiled_sha256']==hashlib.sha256(rebuilt).hexdigest()
        syms={s['orig']:s for s in mapping['symbols']}
        images.append((rebuilt,{n:syms[a]['recomp'] for n,a in ADDRESSES.items()}))
    out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)

    def execute(data,a,h,arena):
        u=Uc(UC_ARCH_X86,UC_MODE_32);u.mem_map(0x400000,0x500000);u.mem_map(HEAP,0x100000)
        pe=struct.unpack_from('<I',data,0x3c)[0];table=pe+24+struct.unpack_from('<H',data,pe+20)[0]
        for i in range(struct.unpack_from('<H',data,pe+6)[0]):
            _,rva,size,raw=struct.unpack_from('<IIII',data,table+40*i+8);u.mem_write(0x400000+rva,data[raw:raw+size])
        def wr(p,v):u.mem_write(p,struct.pack('<I',v&0xffffffff))
        def rd(p):return struct.unpack('<I',u.mem_read(p,4))[0]
        def ptr(off):return 0 if off==0xffffffff else HEAP+off
        u.mem_write(HEAP,bytes(arena))
        for n,off in [('gLocal_buffer',0),('gMr_chunky',h[3]),('gPipe_callbacks',0xe000),
                ('gPipe_buffer_phys_end',h[8]),('gPipe_buffer_working_end',h[7]),
                ('gPipe_buffer_oldest',h[6]),('gPipe_record_ptr',h[5])]:wr(a[n],ptr(off))
        wr(a['gPipe_buffer_start'],HEAP+0x4000 if h[1]&1 else 0)
        wr(a['gAction_replay_mode'],(h[1]>>1)&1);wr(a['gProgram_state']+0x28,(h[1]>>2)&1)
        wr(a['gProgram_state']+0xbc,HEAP+0xc000)
        wr(a['gLocal_buffer_size'],h[3]);wr(a['gReentrancy_count'],h[2])
        for i in range(5):wr(a['gReentrancy_array']+4*i,20+i)
        def snapshot():
            prefix=b''.join(struct.pack('<I',rd(a[n])) for n in POINTERS)
            prefix+=struct.pack('<2I',rd(a['gLocal_buffer_size']),rd(a['gReentrancy_count']))
            prefix+=bytes(u.mem_read(a['gReentrancy_array'],20))
            prefix+=struct.pack('<2I',rd(a['gAction_replay_mode']),rd(a['gProgram_state']+0x28))
            return (prefix+bytes(u.mem_read(HEAP,SIZE))).hex()
        calls=[];counter=[0];fatal=[None]
        def hook(uc,at,size,context):
            if at==a['FatalError']:
                fatal[0]=rd(uc.reg_read(UC_X86_REG_ESP)+4)
                calls.append(['F',fatal[0],snapshot()]);uc.reg_write(UC_X86_REG_EIP,STOP);return
            if at not in (CB1,CB2):return
            which=int(at==CB2);packet=uc.reg_read(UC_X86_REG_ECX)
            calls.append(['C',which,HEAP+0x90000 if h[0] in (4,6) else packet,bytes(u.mem_read(packet,48)).hex(),snapshot()])
            if counter[0]==0:
                if h[15]&1:wr(a['gPipe_callbacks'],HEAP+0xf000)
                if h[15]&2:
                    wr(HEAP+h[23],h[9]+1);wr(HEAP+h[23]+4,1)
                if h[15]&4:
                    wr(a['gPipe_buffer_start'],0);wr(a['gAction_replay_mode'],1);wr(a['gProgram_state']+0x28,0)
                if h[15]&8:wr(a['gPipe_buffer_oldest'],HEAP+0x4800)
                if h[15]&16:
                    wr(a['gLocal_buffer'],HEAP+0x2000);wr(a['gMr_chunky'],HEAP+0x200c);wr(a['gLocal_buffer_size'],12)
                if h[15]&32:wr(a['gPipe_record_ptr'],HEAP+0x4080)
            counter[0]+=1
            sp=uc.reg_read(UC_X86_REG_ESP);uc.reg_write(UC_X86_REG_EAX,h[16]+2*which)
            uc.reg_write(UC_X86_REG_EIP,rd(sp));uc.reg_write(UC_X86_REG_ESP,sp+4)
        u.hook_add(UC_HOOK_CODE,hook);u.reg_write(UC_X86_REG_FPCW,0x27f)
        def invoke(name,ecx=0,edx=0,stackargs=()):
            wr(STACK,STOP)
            for i,v in enumerate(stackargs):wr(STACK+4+4*i,v)
            u.reg_write(UC_X86_REG_ESP,STACK);u.reg_write(UC_X86_REG_ECX,ecx);u.reg_write(UC_X86_REG_EDX,edx)
            u.emu_start(a[name],STOP,count=2000000);assert u.reg_read(UC_X86_REG_EIP)==STOP
            return u.reg_read(UC_X86_REG_EAX)
        value=None;mode=h[0]
        if mode==0:invoke('ARStartPipingSession',h[9])
        elif mode==1:invoke('EndPipingSession2',h[21])
        elif mode==2:invoke('AREndPipingSession')
        elif mode==3:invoke('ARAddDataToSession',h[9],h[10],[HEAP+h[20],h[11]])
        elif mode==4:
            triples=[64,0,HEAP+h[20],1,1,0x12345678,2,3,0x11223344,4,5,0x56789abc,
                0xfffffffc,16,0xa1b2c3d4,7,20,HEAP+0xd040,0,0,0]
            invoke('ARAddVariedDataToSession',stackargs=[h[9],h[10],h[22],*triples])
        elif mode==5:value=invoke('LengthOfSession',HEAP+h[23])
        elif mode==6:invoke('AddSmokeToPipingSession',h[10],h[19],[HEAP+0x10000,h[17],h[18]])
        elif mode==7:
            for name,ecx,edx,stackargs in [('ARStartPipingSession',h[9],0,()),
                    ('ARAddDataToSession',h[9],h[10],[HEAP+h[20],h[11]]),
                    ('ARStartPipingSession',h[9]+1,0,()),
                    ('ARAddDataToSession',h[9]+1,h[10]+1,[HEAP+h[20],h[11]]),
                    ('AREndPipingSession',0,0,()),('AREndPipingSession',0,0,())]:
                invoke(name,ecx,edx,stackargs)
                if fatal[0] is not None:break
        if fatal[0] is not None:value=None
        return dict(calls=calls,state=snapshot(),value=value,fatal=fatal[0])

    source=SOURCE.read_text();names=['ReplayCopyForward','LengthOfSession','ARStartPipingSession','EndPipingSession2',
        'AREndPipingSession','ARAddDataToSession','ARAddVariedDataToSession','ReplayScalarRound','AddSmokeToPipingSession']
    bodies=[extract(source,n) for n in names]
    driver=r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <float.h>
#include "23-piping.h"
#include "52-errors.h"
#include "globvars.h"
typedef char layouts[sizeof(void*)==4&&sizeof(tReplay_callback)==24&&sizeof(tPipe_chunk)==8&&sizeof(tReduced_pos)==6&&sizeof(tPipe_chunk_smoke)==10&&offsetof(tProgram_state,racing)==0x28&&offsetof(tProgram_state,current_car)==0xac?1:-1];
tU8*gPipe_buffer_start,*gLocal_buffer,*gMr_chunky,*gPipe_buffer_phys_end,*gPipe_buffer_working_end,*gPipe_buffer_oldest,*gPipe_record_ptr;
const tReplay_callback*gPipe_callbacks;tU32 gLocal_buffer_size;int gReentrancy_count;tPipe_chunk_type gReentrancy_array[5];
int gAction_replay_mode;tProgram_state gProgram_state;
static unsigned arena[0x11000/4],h[24];static int events,callback_calls;
static int C2_HOOK_FASTCALL callback1(tPipe_chunk*packet);
static int C2_HOOK_FASTCALL callback2(tPipe_chunk*packet);
static unsigned norm(unsigned v){unsigned b=(unsigned)(uintptr_t)arena;if(v>=b&&v<=b+sizeof(arena))return v-b+0x1000000;if(v==(unsigned)(uintptr_t)callback1)return 0x1082000;if(v==(unsigned)(uintptr_t)callback2)return 0x1082010;return v;}
static unsigned rd(unsigned off){unsigned v;memcpy(&v,(char*)arena+off,4);return v;}
static void wr(unsigned off,unsigned v){memcpy((char*)arena+off,&v,4);}
static void hex(const void*p,unsigned n){unsigned i;for(i=0;i<n;i++)printf("%02x",((const unsigned char*)p)[i]);}
static void snapshot(void){unsigned char copy[sizeof(arena)];unsigned prefix[17],i,j,v;
    prefix[0]=norm((unsigned)(uintptr_t)gPipe_buffer_start);prefix[1]=norm((unsigned)(uintptr_t)gLocal_buffer);prefix[2]=norm((unsigned)(uintptr_t)gMr_chunky);prefix[3]=norm((unsigned)(uintptr_t)gPipe_callbacks);
    prefix[4]=norm((unsigned)(uintptr_t)gPipe_buffer_phys_end);prefix[5]=norm((unsigned)(uintptr_t)gPipe_buffer_working_end);prefix[6]=norm((unsigned)(uintptr_t)gPipe_buffer_oldest);prefix[7]=norm((unsigned)(uintptr_t)gPipe_record_ptr);prefix[8]=gLocal_buffer_size;prefix[9]=gReentrancy_count;
    for(i=0;i<5;i++){prefix[10+i]=gReentrancy_array[i];}prefix[15]=gAction_replay_mode;prefix[16]=gProgram_state.racing;
    memcpy(copy,arena,sizeof(copy));for(j=0;j<2;j++)for(i=0;i<71;i++){v=norm(rd(0xe000+0x1000*j+24*i+8));memcpy(copy+0xe000+0x1000*j+24*i+8,&v,4);}hex(prefix,sizeof(prefix));hex(copy,sizeof(copy));
}
static int callback(tPipe_chunk*packet,int which){printf("%s[\"C\",%d,%u,\"",events++?",":"",which,(h[0]==4||h[0]==6)?0x1090000:norm((unsigned)(uintptr_t)packet));hex(packet,48);printf("\",\"");snapshot();printf("\"]");
    if(callback_calls++==0){if(h[15]&1)gPipe_callbacks=(tReplay_callback*)((char*)arena+0xf000);if(h[15]&2){wr(h[23],h[9]+1);wr(h[23]+4,1);}if(h[15]&4){gPipe_buffer_start=NULL;gAction_replay_mode=1;gProgram_state.racing=0;}if(h[15]&8)gPipe_buffer_oldest=(tU8*)arena+0x4800;if(h[15]&16){gLocal_buffer=(tU8*)arena+0x2000;gMr_chunky=(tU8*)arena+0x200c;gLocal_buffer_size=12;}if(h[15]&32)gPipe_record_ptr=(tU8*)arena+0x4080;}
    return h[16]+2*which;
}
static int C2_HOOK_FASTCALL callback1(tPipe_chunk*packet){return callback(packet,0);}
static int C2_HOOK_FASTCALL callback2(tPipe_chunk*packet){return callback(packet,1);}
void C2_NORETURN C2_HOOK_CDECL FatalError(int index,...){printf("%s[\"F\",%d,\"",events++?",":"",index);snapshot();printf("\"]],\"state\":\"");snapshot();printf("\",\"value\":null,\"fatal\":%d}\n",index);exit(0);}
'''
    driver+='\n\n'.join(bodies)
    driver+=r'''
static tU8* pointer(unsigned off){return off==0xffffffff?NULL:(tU8*)arena+off;}
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned i,j,value=0;if(argc!=2)return 1;f=fopen(argv[1],"rb");if(!f)return 2;if(fread(h,sizeof(h),1,f)!=1||fread(arena,sizeof(arena),1,f)!=1)return 3;fclose(f);_controlfp(_PC_53,_MCW_PC);
    gPipe_buffer_start=(h[1]&1)?(tU8*)arena+0x4000:NULL;gAction_replay_mode=(h[1]>>1)&1;gProgram_state.racing=(h[1]>>2)&1;
    gProgram_state.current_car.car_master_actor=(br_actor*)((char*)arena+0xc000);gLocal_buffer=(tU8*)arena;gMr_chunky=(tU8*)arena+h[3];gPipe_callbacks=(tReplay_callback*)((char*)arena+0xe000);
    gPipe_buffer_phys_end=pointer(h[8]);gPipe_buffer_working_end=pointer(h[7]);gPipe_buffer_oldest=pointer(h[6]);gPipe_record_ptr=pointer(h[5]);gLocal_buffer_size=h[3];gReentrancy_count=h[2];for(i=0;i<5;i++)gReentrancy_array[i]=(tPipe_chunk_type)(20+i);
    for(j=0;j<2;j++)for(i=0;i<71;i++){unsigned off=0xe000+0x1000*j+24*i+8,v=rd(off);if(v==0x1082000)wr(off,(unsigned)(uintptr_t)callback1);else if(v==0x1082010)wr(off,(unsigned)(uintptr_t)callback2);}
    printf("{\"calls\":[");
    if(h[0]==0)ARStartPipingSession((tPipe_chunk_type)h[9]);
    else if(h[0]==1)EndPipingSession2(h[21]);
    else if(h[0]==2)AREndPipingSession();
    else if(h[0]==3)ARAddDataToSession(h[9],h[10],(char*)arena+h[20],h[11]);
    else if(h[0]==4)ARAddVariedDataToSession(h[9],h[10],h[22],64,0,(char*)arena+h[20],1,1,0x12345678,2,3,0x11223344,4,5,0x56789abc,-4,16,0xa1b2c3d4,7,20,(char*)arena+0xd040,0,0,0);
    else if(h[0]==5)value=LengthOfSession((tPipe_chunk*)((char*)arena+h[23]));
    else if(h[0]==6)AddSmokeToPipingSession(h[10],(tU8)h[19],(br_vector3*)((char*)arena+0x10000),*(float*)&h[17],*(float*)&h[18]);
    else if(h[0]==7){ARStartPipingSession((tPipe_chunk_type)h[9]);ARAddDataToSession(h[9],h[10],(char*)arena+h[20],h[11]);ARStartPipingSession((tPipe_chunk_type)(h[9]+1));ARAddDataToSession(h[9]+1,h[10]+1,(char*)arena+h[20],h[11]);AREndPipingSession();AREndPipingSession();}
    printf("],\"state\":\"");snapshot();printf("\",\"value\":");if(h[0]==5)printf("%u",value);else printf("null");printf(",\"fatal\":null}\n");return 0;
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
    def bits(v):return struct.unpack('<I',struct.pack('<f',v))[0]
    cases=[]
    for mode in range(8):
        for gate in [0,1,3,4,5,7]:cases.append(dict(mode=mode,gate=gate))
    for reentrancy in [0,1,2,5]:
        for mode in [0,1,2]:cases.append(dict(mode=mode,reentrancy=reentrancy))
    for size in [0,1,3,4,5,6,10,24,63]:cases.append(dict(mode=3,size=size))
    for data,size in [(0,10),(4,10),(8,10),(12,10),(0,24),(4,24)]:cases.append(dict(mode=3,data=data,size=size))
    for local in [14985,14986,14987,14994,14995,14996,14997,14998,15000]:
        for mode in [1,2,3,4]:cases.append(dict(mode=mode,local=local))
    for count in [0,1,2,7]:cases.append(dict(mode=4,varied_count=count,length=0 if count==0 else 24))
    for count in [0,1,2,3,5]:
        for length in [0,1,5,10,24]:cases.append(dict(mode=5,session_count=count,length=length))
    for mode in [4,5,1]:
        for mutate in [0,1,2,4,8,16,32,3,5,17]:
            cases.append(dict(mode=mode,callback=True,mutate=mutate,local=8,length=10 if mode==1 else 24,oldest=0x4000 if mode==1 else 0xffffffff))
    for record in [0x4000,0x4078,0x9ffe]:
        for oldest in [0xffffffff,0x4000,0x4078,0x9000]:cases.append(dict(mode=1,record=record,oldest=oldest,local=70))
    for radius in [0.,-.5,.05,.999999,64.,2097152.,-2097152.]:cases.append(dict(mode=6,radius=bits(radius)))
    for strength in [0,0x80000000,bits(.5),bits(-.5),0x7f800000,0xff800000,0x7fc12345,0x7f800001]:cases.append(dict(mode=6,strength=strength))
    for actor,position in [(-1.673764944076538,4.463735103607178),(.7659949064254761,5.930994987487793),(-1.7662395238876343,-12.819989204406738),(.43791353702545166,7.355413436889648),(-1.9342095851898193,5.3520402908325195),(-1.9078247547149658,-7.095324516296387)]:
        cases.append(dict(mode=6,actor=[actor,actor,actor],position=[position,position,position]))
    results=[]
    for index,case in enumerate(cases):
        arena=bytearray(b'\xa5'*SIZE)
        def put(off,v):struct.pack_into('<I',arena,off,v&0xffffffff)
        def vec(off,v):struct.pack_into('<3f',arena,off,*v)
        h=[case['mode'],case.get('gate',5),case.get('reentrancy',0),case.get('local',8),case.get('local_count',1),case.get('record',0x4000),case.get('oldest',0xffffffff),case.get('working',0xa000),case.get('physical',0xa000),15,0x12345678,case.get('size',10),case.get('session_count',2),int(case.get('callback',False)),case.get('length',10),case.get('mutate',0),case.get('extra',0),case.get('radius',bits(.05)),case.get('strength',bits(.65)),0x17,case.get('data',0xd000),1,case.get('varied_count',7),case.get('session',0xb000)]
        put(0,h[9]);put(4,h[4]);put(0x2000,h[9]);put(0x2004,1)
        for off in range(0x4000,0xa000-24,24):put(off,h[9]);put(off+4,1)
        if h[6]!=0xffffffff:put(h[6],h[9]);put(h[6]+4,1)
        put(h[23],h[9]);put(h[23]+4,h[12])
        for off in [0xd000,0xd040]:
            for j in range(64):arena[off+j]=(j*13+7)&255
        for table in range(2):
            for i in range(71):
                off=0xe000+0x1000*table+24*i;arena[off:off+24]=bytes(24)
                put(off+4,h[14]+2*table+(4 if i==h[9]+1 else 0));put(off+8,(CB1 if table==0 else CB2) if h[13] else 0)
        vec(0xc050,[.1,-.2,.3]);vec(0x10000,[3.125,-4.25,5.375])
        if 'actor' in case:vec(0xc050,case['actor'])
        if 'position' in case:vec(0x10000,case['position'])
        fixture=out/'fixture.bin';fixture.write_bytes(struct.pack('<24I',*h)+arena)
        expected=execute(*images[0],h,arena)
        checks=[]
        if len(images)>1:checks.append(('recompiled-x86',execute(*images[1],h,arena)))
        for compiler in commands:checks.append((compiler,json.loads(subprocess.check_output([str(out/(compiler+'.exe')),str(fixture)],text=True))))
        for compiler,actual in checks:
            if actual!=expected:
                failure=out/f'failure-{compiler}-{index}.json';failure.write_text(json.dumps(dict(case=case,original=expected,actual=actual),indent=2))
                raise AssertionError(f'{compiler} replay record case {index} differs: {failure}')
            results.append(dict(case=case,compiler=compiler,passed=True,calls=len(actual['calls']),fatal=actual['fatal'],state_sha256=hashlib.sha256(bytes.fromhex(actual['state'])).hexdigest()))
    (out/'verification.json').write_text(json.dumps(dict(original_sha256=ORIGINAL_HASH,source_sha256=hashlib.sha256(SOURCE.read_bytes()).hexdigest(),bodies_sha256=[hashlib.sha256(b.encode()).hexdigest() for b in bodies],compiler_commands=commands,cases=results),indent=2)+'\n')
    print(f'PASS: {len(cases)} scenarios, {len(results)} complete replay-record comparisons.')


if __name__=='__main__':main()
