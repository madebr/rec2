#!/usr/bin/env python3
"""Physical original/native/linked quaternion orientation dependency proof.

The complete BrQuatNormalise535710 and BrQuatToMatrix345359a0 bodies and their
private arithmetic helpers execute at normal runtime PC53. Full65536 arenas,
raw pointer returns, ESP/nonvolatiles/x87TOP compare without normalization.
Finite, raw IEEE, paired NaNs, dynamic ranges and partial/in-place aliases are
covered. No callback/import boundary substitutes any quaternion operation.
Floating status/unmasked traps, alternate precision and nonx86 are outside
this proof. Default conservatively extracts production; candidate-source is
an optional complete development override. Immutable PE/symbol pairs bind
actual linked bodies and complete private-helper closure.
"""
import argparse,hashlib,json,random,re,struct,subprocess,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'scripts'))
from verify_tinted_native_reconstruction import extract
from verify_opponent_route_reconstruction import HASH,sha,raw,pe,words
import verify_phil_registry_reconstruction as registry
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_MEM,CS_OP_IMM
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--output',type=Path,default=ROOT/'build-audit/quaternion-orientation-dependencies')
parser.add_argument('--candidate-source',type=Path)
parser.add_argument('--recompiled',type=Path);parser.add_argument('--symbols',type=Path)
parser.add_argument('--case-index',type=int,action='append')
args=parser.parse_args();assert bool(args.recompiled)==bool(args.symbols)
out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)
source=ROOT/'src/brender/core/math/quat.c'
helpers=('QuaternionSum','QuaternionScaleRaw','QuaternionScaleLoaded','QuaternionMultiply','QuaternionAddScalar','QuaternionSubtractScalar','QuaternionScalar')
COMMON='\n#include <windows.h>\n#include <stdio.h>\n#include <stdlib.h>\n#include <string.h>\n#include <math.h>\n#include <float.h>\n#include <io.h>\n#include <fcntl.h>\n#include "c2_hooks.h"\n#include "brender/br_types.h"\nextern br_quat* C2_HOOK_CDECL BrQuatNormalise(br_quat* q, const br_quat* qq);\nextern br_matrix34* C2_HOOK_CDECL BrQuatToMatrix34(br_matrix34* mat, const br_quat* q);\nextern br_quat* C2_HOOK_CDECL BrMatrix34ToQuat(br_quat* q, const br_matrix34* mat);\n#define M(x,y) mat->m[x][y]\n#define M_DIAG(x) (((br_scalar*)mat->m)[4*(x)])\n#define Q_EL(e) (((br_scalar*)&(q->x))[(e)])\nextern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison;\nextern unsigned __cdecl BuilderInvoke(void);\nstatic unsigned addresses[2],h[7],case_index;\nstatic unsigned char*heap;\nstatic void invoke(unsigned mode,unsigned x,unsigned y){unsigned head[3];builder_mode=mode;builder_address=addresses[mode];builder_x=x;builder_y=y;builder_stack_count=2;builder_stack[0]=x;builder_stack[1]=y;builder_poison=0xcccccccc;if(BuilderInvoke())exit(14);head[0]=case_index;head[1]=mode;head[2]=builder_result;fwrite(head,sizeof(head),1,stdout);fwrite(heap,65536,1,stdout);}\nstatic int run(const char*path){FILE*f;SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);f=fopen(path,"rb");if(!f)return 10;heap=(unsigned char*)VirtualAlloc((void*)0x22000000,65536,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(heap!=(void*)0x22000000)return 11;_setmode(_fileno(stdout),_O_BINARY);for(case_index=0;fread(h,sizeof(h),1,f)==1;case_index++){if(fread(heap,65536,1,f)!=1)return 12;_controlfp(_PC_53,_MCW_PC);_clearfp();if(h[0]>1)return 13;invoke(h[0],h[1],h[2]);}fclose(f);return 0;}\n\ntypedef char quaternion_layout[sizeof(void*)==4&&sizeof(br_scalar)==4&&sizeof(br_quat)==16&&sizeof(br_matrix34)==48?1:-1];\n'
candidate=args.candidate_source.resolve() if args.candidate_source else None
paths=[source,source.with_suffix('.h'),ROOT/'src/brender/include/brender/br_types.h',ROOT/'src/carma2/hooks/c2_hooks.h',ROOT/'src/carma2/hooks/c2_math.h',Path(__file__),Path(registry.__file__),ROOT/'scripts/verify_tinted_native_reconstruction.py',ROOT/'scripts/verify_opponent_route_reconstruction.py',ROOT/'scripts/verify_opponent_navigation_reconstruction.py']
if candidate:paths.append(candidate)
if args.recompiled:paths.extend((args.recompiled,args.recompiled.with_suffix('.map'),args.symbols))
source_before={str(p):p.read_bytes() for p in paths};before=source_before[str(source)];candidate_before=source_before[str(candidate)] if candidate else None
caller,physical,template=registry.literals()
old=' cmp builder_mode,8\n jne notwide\n fstp qword ptr builder_wide\n xor eax,eax\n notwide:\n cmp builder_mode,1\n je zeroreturn\n cmp builder_mode,10\n jne resultready\n zeroreturn:\n xor eax,eax\n resultready:';assert old in caller;caller=caller.replace(old,' add esp,8')
patch='image[hdr[12]]=0xe9;relative=(unsigned)RayBoundary-((unsigned)image+hdr[12]+5);memcpy(image+hdr[12]+1,&relative,4);';assert patch in physical;physical=physical.replace(patch,'').replace('relative,','').replace('hdr[14]','hdr[3]').replace('n=hdr[13]','n=hdr[2]').replace('i<12;i++)addresses','i<2;i++)addresses')
entries=[('BrQuatNormalise',0x535710,114),('BrQuatToMatrix34',0x5359a0,308)];original=ROOT/'build-reference/originals/CARMA2_HW.EXE';data=original.read_bytes();assert sha(data)==HASH;nt,base,size,tab=pe(data);cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;patches=[];closure=[]
for name,start,n in entries:
 ins=list(cs.disasm(raw(data,start,n),start));assert sum(i.size for i in ins)==n
 for i in ins:
  assert i.mnemonic!='call'
  if i.mnemonic.startswith('j'):assert i.operands[0].type==CS_OP_IMM and start<=i.operands[0].imm<start+n
  for op in i.operands:
   if op.type==CS_OP_MEM and base<=op.mem.disp<base+size:
    assert op.size==4 and not op.mem.base and not op.mem.index and raw(data,op.mem.disp,4)==struct.pack('<f',1.)
    patches.append((i.address+i.disp_offset-base,op.mem.disp,op.mem.disp-base,0))
 closure.append(dict(name=name,address=start,bytes=n,sha256=sha(raw(data,start,n))))
(out/'original-addresses.bin').write_bytes(words([entries[0][1]-base,entries[1][1]-base,len(patches)])+b''.join(words(p)for p in patches))
native=COMMON+'\n'+('\n'.join(extract(candidate_before.decode() if candidate else before.decode(),name)for name in (*helpers,*[name for name,_,_ in entries])))+r'''
int C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;addresses[0]=(unsigned)BrQuatNormalise;addresses[1]=(unsigned)BrQuatToMatrix34;return run(argv[1]);}
'''
registry.COMMON=COMMON;commands=registry.compile_sources(out,native,physical,caller)
# Finite isolated consumer counterexample derived from real packed orientation
#42935,24743,1702,25060; raw input avoids any fixture-side quaternion math.
parts=(42935,24743,1702,25060)
q=[0x3e9ee13e,0xbe7ac1f6,0xbf72b3e5,0xbe70d9e2]
linked=None
if args.recompiled:
 data=source_before[str(args.recompiled)];mapping=json.loads(source_before[str(args.symbols)]);assert mapping['format']==1 and mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(data)
 syms={v['orig']:v['recomp']for v in mapping['symbols']};map_bytes=source_before[str(args.recompiled.with_suffix('.map'))]
 functions={int(addr,16):(name,owner.strip())for name,addr,owner in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',map_bytes.decode('latin1'),re.M)}
 nt,lb,ls,tab=pe(data);initial=[syms[address] for _,address,_ in entries];todo=list(initial);seen=set();linked_closure=[];relocations={}
 allowed={'_'+name for name in helpers}
 while todo:
  start=todo.pop(0)
  if start in seen:continue
  seen.add(start);end=min(v for v in functions if v>start);name,owner=functions[start];ins=list(cs.disasm(raw(data,start,end-start),start));assert sum(i.size for i in ins)==end-start
  for i in ins:
   for op in i.operands:
    if op.type==CS_OP_MEM and lb<=op.mem.disp<lb+ls:
     assert not op.mem.base and not op.mem.index and i.disp_size==4 and op.size in(4,8),(name,i.op_str)
     bits=raw(data,op.mem.disp,op.size);assert bits in(struct.pack('<f',1.),struct.pack('<d',1.)),(name,i.op_str,bits.hex())
     relocations[i.address+i.disp_offset-lb]=(op.mem.disp,op.mem.disp-lb,0)
   if i.mnemonic=='call' or i.mnemonic.startswith('j'):
    assert i.operands[0].type==CS_OP_IMM,(name,i.op_str);target=i.operands[0].imm
    if start<=target<end:continue
    assert target in functions,(name,i.op_str)
    if target not in initial:assert functions[target][0]in allowed and functions[target][1]in('quat.c.obj','libbrender:quat.c.obj'),(name,functions[target])
    todo.append(target)
  linked_closure.append(dict(name=name,owner=owner,address=start,bytes=end-start,sha256=sha(raw(data,start,end-start))))
 (out/'linked-addresses.bin').write_bytes(words([initial[0]-lb,initial[1]-lb,len(relocations)])+b''.join(words((site,*patch))for site,patch in sorted(relocations.items())))
 linked=dict(recompiled_sha256=sha(data),symbols_sha256=sha(source_before[str(args.symbols)]),map_sha256=sha(map_bytes),closure=linked_closure,relocations=relocations)
def fixture_arena(c):
 arena=bytearray(b'\xa5'*65536);struct.pack_into('<4I',arena,c['source'],*c['q']);return arena
results=[];cases=[dict(mode=mode,q=q,destination=0x1000 if mode==0 else 0x2000,source=0x1000) for mode in (0,1)]
if True:
 import random
 rng=random.Random(0x535710)
 pool=(0,0x80000000,1,0x80000001,0x00800000,0x80800000,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc12345,0xffc54321,0x7f812345,0xff812345)
 for mode in (0,1):
  for seed in range(250):
   bits=[struct.unpack('<I',struct.pack('<f',rng.uniform(-16,16)))[0] for i in range(4)]
   cases.append(dict(mode=mode,q=bits,destination=0x1000 if mode==0 else 0x2000,source=0x1000))
  for slot in range(4):
   for value in pool:
    bits=list(q);bits[slot]=value;cases.append(dict(mode=mode,q=bits,destination=0x1000 if mode==0 else 0x2000,source=0x1000))
  for slots in ((0,1),(0,2),(0,3),(1,2),(1,3),(2,3)):
   for values in ((0x7fc12345,0xffc54321),(0xffc54321,0x7fc12345),(0x7f812345,0xff812346),(0xff812346,0x7f812345),(0x7fc12345,0xff812345),(0x7fc12345,0xffc12345),(0x7f800000,0xff800000)):
    bits=list(q)
    for slot,value in zip(slots,values):bits[slot]=value
    cases.append(dict(mode=mode,q=bits,destination=0x1000 if mode==0 else 0x2000,source=0x1000))
  for destination in (0x0ff4,0x0ff8,0x0ffc,0x1000,0x1004,0x1008,0x100c):
   for bits in (q,[0x7fc12345,0xffc12345,0x7f812345,0xff812345],[0x00800000,0x7f7fffff,0x80000000,0xbf800000]):
    cases.append(dict(mode=mode,q=bits,destination=destination,source=0x1000))
if True:
 for mode in (0,1):
  for value in pool:
   cases.append(dict(mode=mode,q=[value]*4,destination=0x1000 if mode==0 else 0x2000,source=0x1000))
  for exponent in (2,5,16,32,64,96,128,160,192,224,253,254):
   for seed in range(10):
    bits=[rng.getrandbits(1)<<31 | ((exponent if i%2==0 else 255-exponent)<<23) | rng.getrandbits(23) for i in range(4)]
    cases.append(dict(mode=mode,q=bits,destination=0x1000 if mode==0 else 0x2000,source=0x1000))
  for destination in (0x0ff4,0x0ff8,0x0ffc,0x1000,0x1004,0x1008,0x100c):
   for seed in range(8):
    bits=[struct.unpack('<I',struct.pack('<f',rng.uniform(-32,32)))[0] for i in range(4)]
    cases.append(dict(mode=mode,q=bits,destination=destination,source=0x1000))
artifact_paths=[out/n for n in ('physical.exe','msvc5.exe','msvc.exe','mingw.exe','caller.obj','caller.c','native.c','physical.c','original-addresses.bin')]
if linked:artifact_paths.append(out/'linked-addresses.bin')
artifacts_before={str(p):sha(p.read_bytes())for p in artifact_paths}
failures=[];indices=args.case_index or list(range(len(cases)))
for ci in indices:
 c=cases[ci]
 mode=c['mode'];arena=fixture_arena(c);fixture=words([mode,0x22000000+c['destination'],0x22000000+c['source'],0,0,0,1])+arena;(out/'case.bin').write_bytes(fixture)
 command=[str(out/'physical.exe'),str(out/'case.bin'),str(original),str(out/'original-addresses.bin')];r=subprocess.run(command,capture_output=True,timeout=20);assert not r.returncode;expected=r.stdout
 runs={n:[str(out/f'{n}.exe'),str(out/'case.bin')] for n in ('msvc5','msvc','mingw')}
 if linked:runs['physical-matching']=[str(out/'physical.exe'),str(out/'case.bin'),str(args.recompiled),str(out/'linked-addresses.bin')]
 for compiler,command in runs.items():
  r=subprocess.run(command,capture_output=True,timeout=20);assert not r.returncode;actual=r.stdout
  differences=[dict(offset=i-12,expected=expected[i:i+4].hex(),actual=actual[i:i+4].hex())for i in range(12,len(expected),4)if expected[i:i+4]!=actual[i:i+4]]
  row=dict(case_index=ci,case=c,entry=entries[mode][0],compiler=compiler,passed=not differences,differences=differences);results.append(row)
  if differences:
   failures.append(row);(out/f'failure-{ci}.bin').write_bytes(fixture);(out/f'original-{ci}.bin').write_bytes(expected);(out/f'{compiler}-{ci}.bin').write_bytes(actual)
 if(ci+1)%50==0:print(ci+1,len(failures),flush=True)
 if len(failures)>=12:break
source_after={str(p):p.read_bytes() for p in paths};assert source_before==source_after,'Source/include/verifier/immutable input changed during proof'
artifacts_after={str(p):sha(p.read_bytes())for p in artifact_paths};assert artifacts_before==artifacts_after,'Generated proof artifact changed during proof'
manifest=dict(scenes=indices.index(ci)+1,total_scenes=len(indices),complete=indices.index(ci)+1==len(indices)and not failures,comparisons=len(results),case_indices=indices,cases=[cases[i]for i in indices],fixture_input_sha256=sha(b''.join(words([c['mode'],0x22000000+c['destination'],0x22000000+c['source'],0,0,0,1])+fixture_arena(c)for c in [cases[i]for i in indices])),original_sha256=HASH,source_before={k:sha(v)for k,v in source_before.items()},source_before_equals_after=True,artifacts_before=artifacts_before,artifacts_unchanged=True,candidate_sha256=sha(candidate_before)if candidate else None,closure=closure,linked=linked,results=results,failures=failures,commands=commands,limits=__doc__)
(out/'verification.json').write_text(json.dumps(manifest,indent=2)+'\n');print('PASS'if not failures else'FAIL',manifest['scenes'],manifest['comparisons']);raise SystemExit(bool(failures))
