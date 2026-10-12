"""Complete original/native/linked PHIL orientation packing closure at PC53.

Actual published quaternion functions execute. The original FTOL and all
audited constant references execute physically. No quaternion operation is
mocked. Full arena/ABI compares; masked FP flags/traps are outside this proof.
"""
import argparse,json,random,re,struct,subprocess,sys
from pathlib import Path
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'src/carma2').is_dir());sys.path.insert(0,str(ROOT/'scripts'))
from verify_tinted_native_reconstruction import extract
from verify_opponent_route_reconstruction import HASH,pe,raw,sha,words
import verify_phil_registry_reconstruction as registry
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM

SPEC=[('GetOrientationFromMatrix',0x49be90,356),('GetMatrixFromOrientation',0x49c000,200),('BrMatrix34ToQuat',0x535ae0,385),('BrQuatNormalise',0x535710,114),('BrQuatToMatrix34',0x5359a0,308),('__ftol',0x575b80,39)]
COMMON=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "60-phil.h"
extern br_quat* C2_HOOK_CDECL BrQuatNormalise(br_quat* q, const br_quat* qq);
extern br_matrix34* C2_HOOK_CDECL BrQuatToMatrix34(br_matrix34* mat, const br_quat* q);
extern br_quat* C2_HOOK_CDECL BrMatrix34ToQuat(br_quat* q, const br_matrix34* mat);
#define M(x,y) mat->m[x][y]
#define M_DIAG(x) (((br_scalar*)mat->m)[4*(x)])
#define Q_EL(e) (((br_scalar*)&(q->x))[(e)])
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison;
extern unsigned __cdecl BuilderInvoke(void);
static unsigned addresses[2],h[7],case_index;
static unsigned char*heap;
static void invoke(unsigned mode,unsigned x,unsigned y){unsigned head[3];builder_mode=mode;builder_address=addresses[mode];builder_x=x;builder_y=y;builder_stack_count=0;builder_poison=0xcccccccc;if(BuilderInvoke())exit(14);head[0]=case_index;head[1]=mode;head[2]=builder_result;fwrite(head,sizeof(head),1,stdout);fwrite(heap,65536,1,stdout);}
static int run(const char*path){FILE*f;SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);f=fopen(path,"rb");if(!f)return 10;heap=(unsigned char*)VirtualAlloc((void*)0x22000000,65536,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(heap!=(void*)0x22000000)return 11;_setmode(_fileno(stdout),_O_BINARY);for(case_index=0;fread(h,sizeof(h),1,f)==1;case_index++){if(fread(heap,65536,1,f)!=1)return 12;_controlfp(_PC_53,_MCW_PC);_clearfp();if(h[0]<2)invoke(h[0],h[1],h[2]);else{invoke(0,h[1],h[2]);invoke(1,h[3],h[1]);}}fclose(f);return 0;}
'''

def fixtures():
 cases=[]
 def add(**v):
  c=dict(mode=0,matrix=0x100,orientation=0x1000,result=0x2000,parts=None,patch=[],seed=len(cases)+1);c.update(v);cases.append(c)
 for mode in (0,1,2):
  for seed in range(100):add(mode=mode,seed=seed+1)
 for mode in (0,1,2):
  for delta in range(-10,51,2):add(mode=mode,orientation=0x100+delta)
 for parts in ((0,0,0,0),(65535,65535,65535,65535),(32767,32767,32767,32767),(32768,32768,32768,32768),(0,32767,65534,65535)):
  add(mode=1,parts=parts)
 for slot in range(12):
  for bits in (0,0x80000000,1,0x80000001,0x00800000,0x3f800001,0xbf800001,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc12345,0xffc54321,0x7f812345,0xff812345):
   add(patch=[(0x100+4*slot,bits)])
 for i in range(4):
  for value in (0,1,2,32765,32766,32767,32768,32769,65532,65533,65534,65535):
   parts=[12345,23456,34567,45678];parts[i]=value;add(mode=1,parts=parts)
 return cases

def fixture(c):
 rng=random.Random(c['seed']);b=bytearray(words([rng.getrandbits(32) for _ in range(16384)]))
 for i in range(12):struct.pack_into('<f',b,c['matrix']+4*i,rng.uniform(-4,4))
 if c['parts'] is not None:struct.pack_into('<4H',b,c['orientation'],*c['parts'])
 for off,bits in c['patch']:struct.pack_into('<I',b,off,bits)
 x,y=(c['matrix'],c['orientation']) if c['mode']==1 else (c['orientation'],c['matrix'])
 return words([c['mode'],0x22000000+x,0x22000000+y,0x22000000+c['result'],0,0,c['seed']])+b

def main():
 p=argparse.ArgumentParser();p.add_argument('--output',required=True,type=Path);p.add_argument('--case-index',type=int,action='append');p.add_argument('--limit',type=int);p.add_argument('--candidate-source',type=Path);p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);p.add_argument('--quat-candidate',type=Path);args=p.parse_args();assert bool(args.recompiled)==bool(args.symbols);out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)
 source=ROOT/'src/carma2/newcommon/60-phil.c';quaternion=ROOT/'src/brender/core/math/quat.c';candidate=args.candidate_source.resolve() if args.candidate_source else None;caller,physical,template=registry.literals();paths=[source,source.with_suffix('.h'),quaternion,quaternion.with_suffix('.h'),Path(__file__),template,Path(registry.__file__),ROOT/'src/carma2/include/rec2_types.h',ROOT/'src/carma2/hooks/c2_hooks.h',ROOT/'src/brender/include/brender/br_types.h',ROOT/'scripts/verify_opponent_route_reconstruction.py',ROOT/'scripts/verify_tinted_native_reconstruction.py'];
 if candidate:paths.append(candidate)
 if args.recompiled:paths.extend((args.recompiled,args.recompiled.with_suffix('.map'),args.symbols))
 if args.quat_candidate:paths.append(args.quat_candidate.resolve())
 before={str(x):x.read_bytes() for x in paths}
 original=ROOT/'build-reference/originals/CARMA2_HW.EXE';data=original.read_bytes();assert sha(data)==HASH;nt,base,size,tab=pe(data);cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;closure=[];patches=[];constants={}
 for name,start,n in SPEC:
  ins=list(cs.disasm(raw(data,start,n),start));assert sum(i.size for i in ins)==n,(name,n)
  for i in ins:
   for op in i.operands:
    if op.type==CS_OP_MEM and base<=op.mem.disp<base+size:
     assert i.disp_size==4 and (not op.mem.base or name=='BrMatrix34ToQuat'),(name,i.op_str)
     width=12 if op.mem.index or op.mem.base else op.size;value=raw(data,op.mem.disp,width)
     if name=='BrMatrix34ToQuat':assert value in (struct.pack('<3I',1,2,0),*[struct.pack('<f',x) for x in (0.,.5,1.,-1.)])
     elif name in ('GetOrientationFromMatrix','GetMatrixFromOrientation'):assert op.mem.disp in range(0x58a3f8,0x58a410,4) and width==4
     else:assert value==struct.pack('<f',1.),(name,i.op_str,value.hex())
     constants[op.mem.disp]=value.hex();patches.append((i.address+i.disp_offset-base,op.mem.disp,op.mem.disp-base,0))
   if i.mnemonic=='call' or i.mnemonic.startswith('j'):assert i.operands[0].type==CS_OP_IMM and (start<=i.operands[0].imm<start+n or i.operands[0].imm in {a for _,a,_ in SPEC}),(name,i.op_str)
  closure.append(dict(name=name,address=start,bytes=n,sha256=sha(raw(data,start,n))))
 (out/'original-addresses.bin').write_bytes(words([SPEC[0][1]-base,SPEC[1][1]-base,len(patches)])+b''.join(words(p) for p in patches))
 old=' cmp builder_mode,8\n jne notwide\n fstp qword ptr builder_wide\n xor eax,eax\n notwide:\n cmp builder_mode,1\n je zeroreturn\n cmp builder_mode,10\n jne resultready\n zeroreturn:\n xor eax,eax\n resultready:';assert old in caller;caller=caller.replace(old,' xor eax,eax')
 patch='image[hdr[12]]=0xe9;relative=(unsigned)RayBoundary-((unsigned)image+hdr[12]+5);memcpy(image+hdr[12]+1,&relative,4);';assert patch in physical;physical=physical.replace(patch,'').replace('relative,','').replace('hdr[14]','hdr[3]').replace('n=hdr[13]','n=hdr[2]').replace('i<12;i++)addresses','i<2;i++)addresses')
 src=before[str(source)].decode();quat=before[str(quaternion)].decode();body=(before[str(candidate)].decode() if candidate else '\n'.join(extract(src,n) for n in ('PhilOrientationScalar','GetOrientationFromMatrix','GetMatrixFromOrientation')))+'\n'+(before[str(args.quat_candidate.resolve())].decode()+'\n'+extract(quat,'BrMatrix34ToQuat') if args.quat_candidate else '\n'.join(extract(quat,name) for name in ('QuaternionSum','QuaternionScaleRaw','QuaternionScaleLoaded','QuaternionMultiply','QuaternionAddScalar','QuaternionSubtractScalar','QuaternionScalar','BrQuatNormalise','BrQuatToMatrix34','BrMatrix34ToQuat')))
 native=COMMON+'\n'+body+r'''
static volatile unsigned native_bodies[2];
static unsigned __fastcall adapter0(tU16*x,const br_matrix34*y){typedef void(C2_HOOK_FASTCALL*fn)(tU16*,const br_matrix34*);((fn)(size_t)native_bodies[0])(x,y);return 0;}
static unsigned __fastcall adapter1(br_matrix34*x,const tU16*y){typedef void(C2_HOOK_FASTCALL*fn)(br_matrix34*,const tU16*);((fn)(size_t)native_bodies[1])(x,y);return 0;}
int C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;native_bodies[0]=(unsigned)GetOrientationFromMatrix;native_bodies[1]=(unsigned)GetMatrixFromOrientation;addresses[0]=(unsigned)adapter0;addresses[1]=(unsigned)adapter1;return run(argv[1]);}
'''
 registry.COMMON=COMMON;commands=registry.compile_sources(out,native,physical,caller);cases=fixtures();indices=args.case_index or list(range(len(cases)));indices=indices[:args.limit] if args.limit else indices;results=[];failures=[]
 linked=None
 if args.recompiled:
  linked_data=before[str(args.recompiled)];mapping=json.loads(before[str(args.symbols)]);assert mapping['format']==1 and mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(linked_data)
  syms={v['orig']:v['recomp']for v in mapping['symbols']};map_bytes=before[str(args.recompiled.with_suffix('.map'))]
  functions={int(addr,16):(name,owner.strip())for name,addr,owner in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',map_bytes.decode('latin1'),re.M)}
  nt,lb,ls,tab=pe(linked_data);initial=[syms[address]for _,address,_ in SPEC];todo=list(initial);seen=set();linked_closure=[];relocations={}
  helpers={'_PhilOrientationScalar':('60-phil.c.obj',),'_QuaternionSum':('quat.c.obj','libbrender:quat.c.obj'),'_QuaternionScaleRaw':('quat.c.obj','libbrender:quat.c.obj'),'_QuaternionScaleLoaded':('quat.c.obj','libbrender:quat.c.obj'),'_QuaternionMultiply':('quat.c.obj','libbrender:quat.c.obj'),'_QuaternionAddScalar':('quat.c.obj','libbrender:quat.c.obj'),'_QuaternionSubtractScalar':('quat.c.obj','libbrender:quat.c.obj'),'_QuaternionScalar':('quat.c.obj','libbrender:quat.c.obj')}
  allowed_constants=[struct.pack(fmt,value)for fmt in('<f','<d')for value in(0.,.5,-.5,1.,-1.,65534.,1.5259254723787308e-05)]
  while todo:
   start=todo.pop(0)
   if start in seen:continue
   seen.add(start);end=min(v for v in functions if v>start);name,owner=functions[start];ins=list(cs.disasm(raw(linked_data,start,end-start),start));assert sum(i.size for i in ins)==end-start
   for i in ins:
    for op in i.operands:
     if op.type==CS_OP_MEM and lb<=op.mem.disp<lb+ls:
      assert i.disp_size==4
      width=12 if op.mem.base or op.mem.index else op.size;bits=raw(linked_data,op.mem.disp,width)
      if width==12:assert name=='_BrMatrix34ToQuat'and bits==struct.pack('<3I',1,2,0)
      else:assert bits in allowed_constants,(name,i.op_str,bits.hex())
      relocations[i.address+i.disp_offset-lb]=(op.mem.disp,op.mem.disp-lb,0)
    if i.mnemonic=='call' or i.mnemonic.startswith('j'):
     assert i.operands[0].type==CS_OP_IMM,(name,i.op_str);target=i.operands[0].imm
     if start<=target<end:continue
     assert target in functions,(name,i.op_str)
     if target not in initial:assert functions[target][0]in helpers and functions[target][1]in helpers[functions[target][0]],(name,functions[target])
     todo.append(target)
   linked_closure.append(dict(name=name,owner=owner,address=start,bytes=end-start,sha256=sha(raw(linked_data,start,end-start))))
  (out/'linked-addresses.bin').write_bytes(words([initial[0]-lb,initial[1]-lb,len(relocations)])+b''.join(words((site,*value))for site,value in sorted(relocations.items())))
  linked=dict(recompiled_sha256=sha(linked_data),symbols_sha256=sha(before[str(args.symbols)]),map_sha256=sha(map_bytes),closure=linked_closure,relocations=relocations)
 artifact_paths=[out/n for n in ('physical.exe','msvc5.exe','msvc.exe','mingw.exe','caller.obj','caller.c','native.c','physical.c','original-addresses.bin')]
 if linked:artifact_paths.append(out/'linked-addresses.bin')
 artifact_before={str(p):sha(p.read_bytes())for p in artifact_paths}
 for index in indices:
  c=cases[index];path=out/'case.bin';path.write_bytes(fixture(c));r=subprocess.run([str(out/'physical.exe'),str(path),str(original),str(out/'original-addresses.bin')],capture_output=True,timeout=20);assert not r.returncode,('original',index,r.returncode,r.stderr);expected=r.stdout;assert len(expected)==(2 if c['mode']==2 else 1)*65548
  runs={name:[str(out/f'{name}.exe'),str(path)]for name in ('msvc5','msvc','mingw')}
  if linked:runs['physical-matching']=[str(out/'physical.exe'),str(path),str(args.recompiled),str(out/'linked-addresses.bin')]
  for compiler,command in runs.items():
   r=subprocess.run(command,capture_output=True,timeout=20);assert not r.returncode,(compiler,index,r.returncode,r.stderr)
   if r.stdout!=expected:
    first=next(i for i,(a,b) in enumerate(zip(expected,r.stdout)) if a!=b);first-=first%4;failure=dict(index=index,case=c,compiler=compiler,offset=first,expected=expected[first:first+4].hex(),actual=r.stdout[first:first+4].hex());failures.append(failure);(out/'failure.json').write_text(json.dumps(failure,indent=2));(out/'failure-case.bin').write_bytes(path.read_bytes());(out/'original.bin').write_bytes(expected);(out/f'{compiler}.bin').write_bytes(r.stdout);break
   results.append(dict(index=index,compiler=compiler,sha256=sha(r.stdout)))
  if failures:break
  if (indices.index(index)+1)%50==0:print(f'Verified {indices.index(index)+1}/{len(indices)} scenes',flush=True)
 after={str(x):x.read_bytes() for x in paths};assert before==after,'Input changed during verification'
 artifact_after={str(p):sha(p.read_bytes())for p in artifact_paths};assert artifact_before==artifact_after,'Generated artifact changed during proof'
 manifest=dict(scenes=indices.index(index)+1,comparisons=len(results),complete=len(results)==len(indices)*(4 if linked else 3),failures=failures,results=results,cases=[cases[i] for i in indices],closure=closure,linked=linked,fixture_input_sha256=sha(b''.join(fixture(cases[i])for i in indices)),artifact_before=artifact_before,artifacts_unchanged=True,constants=constants,absolute_relocation_patches=patches,original_sha256=HASH,sources={x:sha(v) for x,v in before.items()},source_before_equals_after=True,commands=commands,generated_files_sha256={x.name:sha(x.read_bytes()) for x in out.iterdir() if x.suffix in ('.c','.exe','.obj')},limits=__doc__)
 (out/'verification.json').write_text(json.dumps(manifest,indent=2)+'\n');print('PASS' if not failures else 'FAIL',manifest['scenes'],manifest['comparisons']);return bool(failures)
if __name__=='__main__':raise SystemExit(main())
