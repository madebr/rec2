#!/usr/bin/env python3
"""Compare quaternion arithmetic with physical original x86 at PC53.

Run actual production bodies with MSVC5, modern MSVC and i686 MinGW, plus
optional linked bodies. Compare complete raw arenas and returned pointers for
finite/IEEE inputs and bounded partial/in-place aliases. ESP, preserved
registers and x87 TOP are checked. Only audited constants and the original
three-index table may be relocated; there are no callbacks or replacement
math operations. Floating exception flags are outside these fixtures.
"""
import argparse,hashlib,json,math,os,random,struct,subprocess,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];sys.path.insert(0,str(ROOT/'scripts'))
from verify_replay_session_reconstruction import raw,PHYSICAL_LOADER,ORIGINAL_HASH
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM
parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--output',type=Path,default=ROOT/'build-audit/quaternion-tests');parser.add_argument('--recompiled',type=Path);parser.add_argument('--symbols',type=Path)
args=parser.parse_args();assert bool(args.recompiled)==bool(args.symbols);OUT=args.output.resolve();OUT.mkdir(parents=True,exist_ok=True)
ENTRIES={0x5355e0:290,0x535790:37,0x535ae0:385}
driver=r'''
#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#define C2_HOOK_CDECL __cdecl
static unsigned addresses[3],arena[128],h[4];
static unsigned __cdecl checked(unsigned,unsigned,unsigned,unsigned*,unsigned,unsigned);
static int run(int argc,char**argv){FILE*f;unsigned words[3],i;
 if(argc!=2)return 1;SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);f=fopen(argv[1],"rb");if(!f)return 2;
 _controlfp(_PC_53,_MCW_PC);
 while(fread(h,sizeof(h),1,f)==1){if(fread(arena,sizeof(arena),1,f)!=1)return 3;
  for(i=0;i<3;i++)words[i]=(unsigned)(uintptr_t)((unsigned char*)arena+h[i+1]);
  if(checked(addresses[h[0]],0,0,words,h[0]?2:3,h[0]?8:12)!=words[0])return 91;
  for(i=0;i<sizeof(arena);i++)printf("%02x",((unsigned char*)arena)[i]);printf("\n");
 }fclose(f);return 0;
}
'''
old='{unsigned char*site=(unsigned char*)(uintptr_t)addresses[FATAL_INDEX];site[0]=0xe9;value=(unsigned)(uintptr_t)FatalError-((unsigned)(uintptr_t)site+5);memcpy(site+1,&value,4);}'
assert old in PHYSICAL_LOADER
(OUT/'physical.c').write_text(driver+PHYSICAL_LOADER.replace(old,'').replace('Only FatalError is replaced; all internal replay bodies remain intact.','All three quaternion bodies execute unchanged.'))
msvc=sorted((ROOT/'build-tools/msvc-modern/VC/Tools/MSVC').iterdir())[-1];sdk=ROOT/'build-tools/msvc-modern/Windows Kits/10';version=sorted(p.name for p in (sdk/'Include').iterdir())[-1]
env=dict(os.environ);env['PATH']=str(msvc/'bin/Hostx64/x86')+os.pathsep+env['PATH'];env['INCLUDE']=os.pathsep.join(map(str,[msvc/'include',*[sdk/f'Include/{version}/{r}' for r in ['ucrt','shared','um']]]));env['LIB']=os.pathsep.join(map(str,[msvc/'lib/x86',*[sdk/f'Lib/{version}/{r}/x86' for r in ['ucrt','um']]]))
cmd=[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',str(OUT/'physical.c'),'/Fo'+str(OUT/'physical.obj'),'/Fe'+str(OUT/'physical.exe')]
r=subprocess.run(cmd,cwd=OUT,env=env,text=True,capture_output=True);(OUT/'build.log').write_text(r.stdout+r.stderr);assert not r.returncode,r.stdout+r.stderr
targets=[('original',ROOT/'build-reference/originals/CARMA2_HW.EXE',None)]
if args.symbols:targets.append(('matching',args.recompiled.resolve(),args.symbols.resolve()))
evidence={};cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
for name,pe,mp in targets:
 data=pe.read_bytes();sha=hashlib.sha256(data).hexdigest()
 if mp:
  mapping=json.loads(mp.read_text());assert mapping['recompiled_sha256']==sha;syms={v['orig']:v for v in mapping['symbols']};starts=sorted(v['recomp'] for v in mapping['symbols'])
  spans=[(syms[a]['recomp'],min(s for s in starts if s>syms[a]['recomp'])-syms[a]['recomp']) for a in ENTRIES]
 else:
  assert sha==ORIGINAL_HASH;spans=list(ENTRIES.items())
 metadata=[];patches={};constants={}
 for a,n in spans:
  code=raw(data,a,n);ins=list(cs.disasm(code,a));assert sum(i.size for i in ins)==n
  for i in ins:
   if i.mnemonic=='call':assert i.operands[0].type==CS_OP_IMM and i.operands[0].imm in {s for s,_ in spans},(hex(i.address),i.op_str)
   if i.mnemonic.startswith('j'):assert i.operands[0].type==CS_OP_IMM and a<=i.operands[0].imm<a+n,(hex(i.address),i.op_str)
   for v in i.operands:
    if v.type==CS_OP_MEM and v.mem.disp>=0x400000:
     if v.mem.base or v.mem.index:
      assert i.mnemonic=='mov' and raw(data,v.mem.disp,12)==struct.pack('<3I',1,2,0),(hex(i.address),i.op_str)
      value=raw(data,v.mem.disp,12)
     else:
      value=raw(data,v.mem.disp,v.size)
      assert i.mnemonic.startswith('f') and value in [struct.pack('<f',x) for x in (0.,.5,1.,-1.)],(hex(i.address),i.op_str,value.hex())
     constants[v.mem.disp]=value.hex();assert i.disp_size==4;patches[i.address+i.disp_offset-0x400000]=v.mem.disp
  metadata.append(dict(address=a,length=n,sha256=hashlib.sha256(code).hexdigest()))
 patches=sorted(patches.items())
 (OUT/(name+'.bin')).write_bytes(struct.pack('<4I',*[a for a,_ in spans],len(patches))+b''.join(struct.pack('<2I',*v) for v in patches));evidence[name]=dict(pe=str(pe),sha256=sha,closure=metadata,absolute_data_relocations=patches,constants=constants)
cases=[];rng=random.Random(5371214);pool=[0,0x80000000,1,0x80000001,0x007fffff,0x00800000,0x3f800000,0xbf800000,0x3f800001,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc00000,0xffc00000,0x7fc01234,0xffc01234,0x7fa01234,0xffa01234]
for mode in range(3):
 for kind in ('finite','raw','nanpairs'):
  for k in range(180):
   if kind=='finite':words=[struct.unpack('<I',struct.pack('<f',rng.uniform(-100,100)))[0] for _ in range(128)]
   else:words=[rng.choice(pool) for _ in range(128)]
   if kind=='nanpairs':
    words=[0x3f800000]*128
    for _ in range(2):words[rng.randrange(64,96)]=rng.choice(pool[13:])
   offsets=[0x80,0x100,0x140]
   if k%9 in (1,2,3,4):offsets[0]=[0x100,0x104,0x140,0x144][k%9-1]
   if k%9==5:offsets[2]=0x100
   if k%9==6:offsets[2]=0x104
   if k%9==7:offsets[0]=0x108;offsets[2]=0x104
   if k%9==8:offsets[0]=0x120
   cases.append(dict(mode=mode,kind=kind,offsets=offsets,words=words))
# Deterministic ties, trace thresholds, cancellation, and real rotation matrices.
for diag in ((-1.,-1.,-1.),(-2.,-2.,-3.),(-2.,-3.,-2.),(-3.,-2.,-2.),(1.,1.,1.),(0.,0.,0.),(1e20,-1e20,1.),(1e20,1.,-1e20),(-.5,-.5,1.),(-.5,-.5,.99999994)):
 for off in (0x80,0x100,0x104,0x108,0x10c,0x110,0x114,0x118,0x11c,0x120,0x124,0x128,0x12c):
  words=[0x3f000000]*128
  for j,x in enumerate(diag):words[64+4*j]=struct.unpack('<I',struct.pack('<f',x))[0]
  cases.append(dict(mode=2,kind='diagonal-boundary',offsets=[off,0x100,0x140],words=words))
# Proper axis-angle rotations include the half-turn diagonal-selection boundary.
for axis in ((1.,0.,0.),(0.,1.,0.),(0.,0.,1.),(1.,1.,0.),(1.,1.,1.),(1.,2.,3.)):
 norm=math.sqrt(sum(v*v for v in axis));x,y,z=[v/norm for v in axis]
 for angle in (0.,1e-12,-1e-12,1e-6,-1e-6,math.pi/6,math.pi/2,math.pi-1e-7,math.pi,math.pi+1e-7,2*math.pi):
  c=math.cos(angle);s=math.sin(angle);d=1-c
  rotation=[c+x*x*d,x*y*d-z*s,x*z*d+y*s,x*y*d+z*s,c+y*y*d,y*z*d-x*s,x*z*d-y*s,y*z*d+x*s,c+z*z*d]
  for off in (0x80,0x100,0x104,0x110,0x11c,0x120):
   words=[0x3f000000]*128
   for j,v in enumerate(rotation):words[64+j]=struct.unpack('<I',struct.pack('<f',v))[0]
   cases.append(dict(mode=2,kind='proper-rotation',offsets=[off,0x100,0x140],words=words))
# Finite exponent spread catches spills, underflow and cancellation without NaNs.
for mode in (0,2):
 for k in range(256):
  words=[rng.choice((0,0x80000000))|rng.randrange(255)<<23|rng.getrandbits(23) for _ in range(128)]
  cases.append(dict(mode=mode,kind='finite-exponent-spread',offsets=[[0x80,0x100,0x104,0x108,0x140,0x144][k%6],0x100,0x140],words=words))
# Every inversion component must quiet an sNaN, including the copied w word.
for off in (0x80,0x100,0x104,0x108,0x10c,0xfc):
 for component in range(4):
  for bits in (0x7f800001,0xff800001,0x7fbfffff,0xffbfffff,0x7fc01234,0xffc01234):
   words=[0x3f800000]*128;words[64+component]=bits
   cases.append(dict(mode=1,kind='inversion-nan',offsets=[off,0x100,0x140],words=words))
inp=OUT/'cases.bin';inp.write_bytes(b''.join(struct.pack('<132I',c['mode'],*c['offsets'],*c['words']) for c in cases))
results={}
for name,pe,_ in targets:
 output=subprocess.check_output([str(OUT/'physical.exe'),str(inp),str(pe),str(OUT/(name+'.bin'))],timeout=40)
 (OUT/(name+'-outputs.txt')).write_bytes(output);results[name]=output.decode().splitlines();assert len(results[name])==len(cases)
failures={}
for name in results:
 if name=='original':continue
 bad=[dict(index=i,case=cases[i],original=results['original'][i],actual=results[name][i]) for i in range(len(cases)) if results[name][i]!=results['original'][i]]
 if bad:
  (OUT/(name+'-differences.json')).write_text(json.dumps(bad,indent=2)+'\n');failures[name]=len(bad)

from verify_tinted_native_reconstruction import extract
names=['BrQuatMul','BrQuatInvert','BrMatrix34ToQuat']
source=ROOT/'src/brender/core/math/quat.c';bodies='\n\n'.join(extract(source.read_text(),n) for n in names)
driver=driver.replace('#define C2_HOOK_CDECL __cdecl','').replace('if(argc!=2)return 1;','if(argc!=2){return 1;}').replace('for(i=0;i<sizeof(arena);i++)printf("%02x",((unsigned char*)arena)[i]);printf("\\n");','for(i=0;i<sizeof(arena);i++){printf("%02x",((unsigned char*)arena)[i]);}printf("\\n");')
driver='#include "quat.h"\n#include <math.h>\n#define M(x,y) mat->m[x][y]\n#define M_DIAG(x) (((br_scalar*)mat->m)[4*(x)])\n#define Q_EL(e) (((br_scalar*)&(q->x))[(e)])\n'+driver+bodies+r'''
static unsigned __cdecl checked(unsigned fn,unsigned cx,unsigned dx,unsigned*w,unsigned n,unsigned cleanup){
 (void)cx;(void)dx;(void)cleanup;
 if(n==2){return (unsigned)(uintptr_t)((void* (__cdecl*)(void*,const void*))(uintptr_t)fn)((void*)(uintptr_t)w[0],(void*)(uintptr_t)w[1]);}
 return (unsigned)(uintptr_t)((void* (__cdecl*)(void*,const void*,const void*))(uintptr_t)fn)((void*)(uintptr_t)w[0],(void*)(uintptr_t)w[1],(void*)(uintptr_t)w[2]);
}
int __cdecl main(int argc,char**argv){
'''+''.join(f'addresses[{i}]=(unsigned)(uintptr_t){n};' for i,n in enumerate(names))+'return run(argc,argv);}\n'
(OUT/'native.c').write_text(driver)
includes=[ROOT/r for r in ['src/carma2/include','src/carma2/hooks','src/brender/include','src/brender/core/math']]
old=ROOT/'build-tools/msvc500sp3';oldenv=dict(os.environ);oldenv['PATH']=str(old/'bin')+os.pathsep+str(old/'redist')+os.pathsep+oldenv['PATH'];oldenv['INCLUDE']=str(old/'include');oldenv['LIB']=str(old/'lib')
commands={
 'msvc':[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(r) for r in includes],str(OUT/'native.c'),'/Fo'+str(OUT/'msvc.obj'),'/Fe'+str(OUT/'msvc.exe')],
 'msvc5':[str(old/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(r) for r in includes],str(OUT/'native.c'),'/Fo'+str(OUT/'msvc5.obj'),'/Fe'+str(OUT/'msvc5.exe')],
 'mingw':[str(ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-fno-strict-aliasing',*['-I'+str(r) for r in includes],str(OUT/'native.c'),'-o',str(OUT/'mingw.exe')]}
expected=(OUT/'original-outputs.txt').read_bytes();results={}
for name,cmd in commands.items():
 r=subprocess.run(cmd,cwd=OUT,env=env if name=='msvc' else oldenv if name=='msvc5' else None,text=True,capture_output=True);(OUT/(name+'-build.log')).write_text(r.stdout+r.stderr);assert not r.returncode,r.stdout+r.stderr
 actual=subprocess.check_output([str(OUT/(name+'.exe')),str(OUT/'cases.bin')]);(OUT/(name+'-outputs.txt')).write_bytes(actual)
 if actual!=expected:
  a=actual.decode().splitlines();e=expected.decode().splitlines();assert len(a)==len(cases);bad=[dict(index=i,case=cases[i],original=e[i],actual=a[i]) for i in range(len(a)) if a[i]!=e[i]];failures[name]=len(bad);(OUT/(name+'-differences.json')).write_text(json.dumps(bad,indent=2)+'\n')
 results[name]=dict(passed=name not in failures,output_sha256=hashlib.sha256(actual).hexdigest())

summary=dict(failures=failures,scenario_count=len(cases),comparisons=len(cases)*(3+bool(args.symbols)),source_sha256=hashlib.sha256(source.read_bytes()).hexdigest(),bodies_sha256=hashlib.sha256(bodies.encode()).hexdigest(),harness_sha256=hashlib.sha256(driver.encode()).hexdigest(),verifier_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),compiler_commands=commands,native_results=results,original_sha256=ORIGINAL_HASH,physical=evidence,recompiled_sha256=hashlib.sha256(args.recompiled.read_bytes()).hexdigest() if args.symbols else None,symbols_sha256=hashlib.sha256(args.symbols.read_bytes()).hexdigest() if args.symbols else None,limitations=__doc__)
(OUT/'verification.json').write_text(json.dumps(summary,indent=2)+'\n');print(f"{'FAIL' if failures else 'PASS'}: {len(cases)} scenarios, {summary['comparisons']} quaternion comparisons.")

if failures:raise AssertionError(failures)
