#!/usr/bin/env python3
"""Compare replay-clock startup and the real timer with physical original x86.

Only the operating-system QueryPerformanceCounter import is controlled. The
original integer division helper executes unchanged. Compare complete timer
globals, import observations and raw returns; callbacks may change timer/replay
state. Nonzero divisors are used. This does not validate a hardware clock.
"""
import argparse, hashlib, json, os, random, re, struct, subprocess
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_OP_IMM, CS_OP_MEM
from verify_replay_session_reconstruction import raw, PHYSICAL_LOADER, ORIGINAL_HASH
from verify_tinted_native_reconstruction import extract

ROOT = Path(__file__).resolve().parents[1]
PIPE = ROOT/'src/carma2/newcommon/23-piping.c'
TIMER = ROOT/'src/carma2/platform/win32/win32.c'
ADDRESSES = [0x403d40, 0x51d410, 0x676914, 0x79efb4, 0x6acc70, 0x6acc80, 0x6ad1f0, 0x589040]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--output', type=Path, default=ROOT/'build-audit/replay-clock-tests')
parser.add_argument('--symbols', type=Path)
parser.add_argument('--recompiled', type=Path)
args = parser.parse_args()
assert bool(args.symbols) == bool(args.recompiled)
out = args.output.resolve(); out.mkdir(parents=True, exist_ok=True)
driver = r'''
#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "23-piping.h"
#include "platform.h"
static unsigned addresses[8],h[18],calls,observed[7];
static unsigned __cdecl checked(unsigned,unsigned,unsigned,unsigned*,unsigned,unsigned);
#ifdef PHYSICAL
#define gAction_replay_mode (*(int*)(uintptr_t)addresses[2])
#define gLast_replay_frame_time (*(tU32*)(uintptr_t)addresses[3])
#define gPerformanceCounterInitialized (*(int*)(uintptr_t)addresses[4])
#define gPerformanceCounterStart (*(LARGE_INTEGER*)(uintptr_t)addresses[5])
#define gPerformanceCounterFrequency_ms (*(LARGE_INTEGER*)(uintptr_t)addresses[6])
#else
int gAction_replay_mode,gPerformanceCounterInitialized;
tU32 gLast_replay_frame_time;
LARGE_INTEGER gPerformanceCounterStart,gPerformanceCounterFrequency_ms;
#endif
static void snapshot(unsigned* s){
 s[0]=gAction_replay_mode;s[1]=gLast_replay_frame_time;s[2]=gPerformanceCounterInitialized;
 memcpy(s+3,&gPerformanceCounterStart,8);memcpy(s+5,&gPerformanceCounterFrequency_ms,8);
}
static BOOL WINAPI ClockQuery(LARGE_INTEGER* value){
 calls++;snapshot(observed);memcpy(value,h+8,8);
 if(h[11]&1)gAction_replay_mode=h[12];
 if(h[11]&2)gLast_replay_frame_time=h[13];
 if(h[11]&4)memcpy(&gPerformanceCounterStart,h+14,8);
 if(h[11]&8)memcpy(&gPerformanceCounterFrequency_ms,h+16,8);
 if(h[11]&16)gPerformanceCounterInitialized=0;
 return h[10];
}
#define QueryPerformanceCounter ClockQuery
BODIES
static int run(int argc,char**argv){FILE*f;unsigned result[16];
 if(argc!=2)return 1;
 SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);
 _setmode(_fileno(stdout),_O_BINARY);f=fopen(argv[1],"rb");if(!f)return 2;
 _controlfp(_PC_53,_MCW_PC);
 while(fread(h,sizeof(h),1,f)==1){
  if(h[0]>1)return 3;
  gAction_replay_mode=h[1];gLast_replay_frame_time=h[2];gPerformanceCounterInitialized=h[3];
  memcpy(&gPerformanceCounterStart,h+4,8);memcpy(&gPerformanceCounterFrequency_ms,h+6,8);
  calls=0;memset(observed,0,sizeof(observed));memset(result,0,sizeof(result));
  result[0]=checked(addresses[h[0]],0,0,NULL,0,0);
  if(!h[0])result[0]=0;
  result[1]=calls;memcpy(result+2,observed,sizeof(observed));snapshot(result+9);
  fwrite(result,sizeof(result),1,stdout);
 }fclose(f);return 0;
}
'''
bodies = extract(TIMER.read_text(), 'PDGetTotalTime')+'\n'+extract(PIPE.read_text(), 'ARMainLoopStart')
native = driver.replace('BODIES', bodies)+r'''
static unsigned __cdecl checked(unsigned fn,unsigned cx,unsigned dx,unsigned*w,unsigned n,unsigned cleanup){
 (void)cx;(void)dx;(void)w;(void)n;(void)cleanup;
 if(fn==addresses[0]){ARMainLoopStart();return 0;}
 return PDGetTotalTime();
}
int C2_HOOK_CDECL main(int argc,char**argv){
 addresses[0]=(unsigned)(uintptr_t)ARMainLoopStart;addresses[1]=(unsigned)(uintptr_t)PDGetTotalTime;
 return run(argc,argv);
}
'''
old = '{unsigned char*site=(unsigned char*)(uintptr_t)addresses[FATAL_INDEX];site[0]=0xe9;value=(unsigned)(uintptr_t)FatalError-((unsigned)(uintptr_t)site+5);memcpy(site+1,&value,4);}'
assert old in PHYSICAL_LOADER
physical = '#define PHYSICAL\n'+driver.replace('BODIES', '')+PHYSICAL_LOADER.replace(old, '*(unsigned*)(uintptr_t)addresses[7]=(unsigned)(uintptr_t)ClockQuery;').replace('Only FatalError is replaced; all internal replay bodies remain intact.', 'Only the QueryPerformanceCounter import is controlled.')
(out/'native.c').write_text(native); (out/'physical.c').write_text(physical)
msvc = sorted((ROOT/'build-tools/msvc-modern/VC/Tools/MSVC').iterdir())[-1]
sdk = ROOT/'build-tools/msvc-modern/Windows Kits/10'; version = sorted(p.name for p in (sdk/'Include').iterdir())[-1]
env = dict(os.environ); env['PATH'] = str(msvc/'bin/Hostx64/x86')+os.pathsep+env['PATH']
env['INCLUDE'] = os.pathsep.join(map(str,[msvc/'include',*[sdk/f'Include/{version}/{r}' for r in ['ucrt','shared','um']]]))
env['LIB'] = os.pathsep.join(map(str,[msvc/'lib/x86',*[sdk/f'Lib/{version}/{r}/x86' for r in ['ucrt','um']]]))
old = ROOT/'build-tools/msvc500sp3'; oldenv = dict(os.environ)
oldenv['PATH'] = str(old/'bin')+os.pathsep+str(old/'redist')+os.pathsep+oldenv['PATH']
oldenv['INCLUDE'] = str(old/'include'); oldenv['LIB'] = str(old/'lib')
includes = [ROOT/r for r in ['src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/carma2/platform','src/carma2/s3/include','src/brender/include']]
commands = {}
for name in ('msvc','msvc5','mingw','physical'):
 source = out/('physical.c' if name=='physical' else 'native.c')
 if name=='mingw':
  cmd = [str(ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'),'-std=gnu89','-O2','-Wall','-Wextra','-Werror',*['-I'+str(r) for r in includes],str(source),'-o',str(out/(name+'.exe'))]
 else:
  cmd = [str(old/'bin/cl.exe') if name=='msvc5' else str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS']
  if name=='msvc5':cmd += ['/Ob2','/G5','/Gr','/ML','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include')]
  cmd += [*['/I'+str(r) for r in includes],str(source),'/Fo'+str(out/(name+'.obj')),'/Fe'+str(out/(name+'.exe'))]
 commands[name] = cmd
 result = subprocess.run(cmd,cwd=out,env=oldenv if name=='msvc5' else None if name=='mingw' else env,text=True,capture_output=True)
 (out/(name+'-build.log')).write_text(result.stdout+result.stderr)
 assert result.returncode==0, result.stdout+result.stderr

targets = [('original', ROOT/'build-reference/originals/CARMA2_HW.EXE', None)]
if args.symbols:targets.append(('matching',args.recompiled.resolve(),args.symbols.resolve()))
metadata = {}; cs = Cs(CS_ARCH_X86,CS_MODE_32); cs.detail = True
for name,pe,mp in targets:
 data = pe.read_bytes()
 if mp:
  mapping = json.loads(mp.read_text()); assert mapping['original_sha256']==ORIGINAL_HASH and mapping['recompiled_sha256']==hashlib.sha256(data).hexdigest()
  syms = {v['orig']:v['recomp'] for v in mapping['symbols']}
  public = {n:int(a,16) for n,a in re.findall(r'^\s*[0-9a-fA-F]{4}:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s',pe.with_suffix('.map').read_text(),re.M)}
  addresses = [syms[a] for a in ADDRESSES[:-1]]+[public['__imp__QueryPerformanceCounter@4']]
  starts = set(syms.values())|set(public.values())
  spans = [(a,min(s for s in starts if s>a)-a) for a in addresses[:2]]
  for a,n in list(spans):
   for ins in cs.disasm(raw(data,a,n),a):
    if ins.mnemonic=='call' and ins.operands[0].type==CS_OP_IMM and ins.operands[0].imm not in addresses[:2]:
     callee = ins.operands[0].imm
     assert callee in {public.get('__alldiv'),public.get('__aulldiv')},(hex(ins.address),ins.op_str)
     spans.append((callee,min(s for s in starts if s>callee)-callee))
 else:
  assert hashlib.sha256(data).hexdigest()==ORIGINAL_HASH
  addresses = ADDRESSES
  spans = [(0x403d40,21),(0x51d410,139),(0x578830,104)]
 patches = {}; closure = []
 allowed = {addresses[2],addresses[3],addresses[4],addresses[5],addresses[5]+4,addresses[6],addresses[6]+4,addresses[7]}
 for a,n in spans:
  code = raw(data,a,n); instructions = list(cs.disasm(code,a)); assert sum(i.size for i in instructions)==n
  for ins in instructions:
   if ins.mnemonic=='call' or ins.mnemonic.startswith('j'):
    operand = ins.operands[0]
    if operand.type==CS_OP_IMM:
     assert operand.imm in {s for s,_ in spans} or (ins.mnemonic.startswith('j') and a<=operand.imm<a+n),(hex(ins.address),ins.op_str)
    else:assert ins.mnemonic=='call' and operand.type==CS_OP_MEM and operand.mem.disp==addresses[7]
   for operand in ins.operands:
    if operand.type==CS_OP_MEM and not operand.mem.base and operand.mem.disp>=0x400000:
     assert operand.mem.disp in allowed and ins.disp_size==4
     patches[ins.address+ins.disp_offset-0x400000] = operand.mem.disp
  closure.append(dict(address=a,length=n,sha256=hashlib.sha256(code).hexdigest()))
 patches = sorted(patches.items())
 (out/(name+'-addresses.bin')).write_bytes(struct.pack('<9I',*addresses,len(patches))+b''.join(struct.pack('<2I',*v) for v in patches))
 metadata[name] = dict(sha256=hashlib.sha256(data).hexdigest(),closure=closure,addresses=addresses,relocations=patches)

cases = []; rng = random.Random(0x403d40)
def add(mode,init,start,counter,frequency,mutation=0,status=1):
 vals = [mode,0x89abcdef,0x12345678,init,start&0xffffffff,start>>32,frequency&0xffffffff,frequency>>32,counter&0xffffffff,counter>>32,status,mutation,0x87654321,0xdeadbeef,7,0,3,0]
 cases.append([v&0xffffffff for v in vals])
for mode in (0,1):
 for init in (0,1,2,0xffffffff):
  for start,counter in ((0,0),(0,1),(0,0xffffffff),(0xffffffff,0x100000000),(1,0),(0xffffffffffffffff,0),(0,0x8000000000000000),(0x8000000000000000,0),(0x7fffffffffffffff,0xffffffffffffffff)):
   for frequency in (1,2,3,1000,10000000,0xffffffff,0x100000000,0x8000000000000000,0xffffffffffffffff):
    add(mode,init,start,counter,frequency)
 for mutation in range(32):
  for status in (0,1):add(mode,1,13,123456789,1000,mutation,status)
 for _ in range(256):add(mode,1,rng.getrandbits(64),rng.getrandbits(64),rng.getrandbits(64) or 1)
packed = b''.join(struct.pack('<18I',*c) for c in cases); (out/'cases.bin').write_bytes(packed)
outputs = {}
for name,pe,_ in targets:
 outputs[name] = subprocess.check_output([str(out/'physical.exe'),str(out/'cases.bin'),str(pe),str(out/(name+'-addresses.bin'))],timeout=40)
for name in ('msvc','msvc5','mingw'):outputs[name] = subprocess.check_output([str(out/(name+'.exe')),str(out/'cases.bin')],timeout=40)
failures = {}
for name,data in outputs.items():
 assert len(data)==64*len(cases)
 (out/(name+'-outputs.bin')).write_bytes(data)
 differences = [dict(index=i,inputs=c,original=outputs['original'][64*i:64*(i+1)].hex(),actual=data[64*i:64*(i+1)].hex()) for i,c in enumerate(cases) if data[64*i:64*(i+1)]!=outputs['original'][64*i:64*(i+1)]]
 if differences:
  failures[name] = len(differences); (out/(name+'-differences.json')).write_text(json.dumps(differences,indent=2)+'\n')
report = dict(scenario_count=len(cases),comparisons=len(cases)*(len(outputs)-1),failures=failures,physical=metadata,compiler_commands=commands,fixtures_sha256=hashlib.sha256(packed).hexdigest(),source_hashes={str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in [PIPE,TIMER,Path(__file__)]},output_hashes={n:hashlib.sha256(d).hexdigest() for n,d in outputs.items()},limitations=__doc__)
(out/'verification.json').write_text(json.dumps(report,indent=2)+'\n')
print(('FAIL' if failures else 'PASS')+f": {len(cases)} scenarios, {report['comparisons']} comparisons.")
if failures:raise AssertionError(failures)
