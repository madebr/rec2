#!/usr/bin/env python3
"""Physical original/native force-record creation and velocity-vector proof.

The three complete production bodies and both actual BRender vector transforms
execute at x86 PC53. Full fixture arenas compare without NaN normalization.
There are no substituted game dependencies or external callbacks in this graph.
Normal calls check ESP, preserved registers and x87 TOP. FP status/traps and
nonx86 builds are outside this proof. Invalid pointer faults are separate from
successful full-state cases; no initialized values are invented for untouched
record words. Requires the scoped MSVC5/modern MSVC/i686 MinGW and Capstone.
"""
import argparse,json,random,re,struct,subprocess
from pathlib import Path
from verify_tinted_native_reconstruction import extract
from verify_opponent_route_reconstruction import ROOT,HASH,pe,raw,sha,words
import verify_phil_registry_reconstruction as registry
import verify_tinted_native_reconstruction as extraction
import verify_opponent_route_reconstruction as reference

SOURCE=ROOT/'src/carma2/newcommon/49-physics.c'
MATRIX=ROOT/'src/brender/core/math/matrix34.c'
ENTRIES=[('MakeJointVelocitiesVector',0x4b77b0,425),('CreateJointForce',0x4b8c20,201),('CreateJointTorque',0x4b8e10,198),('BrMatrix34ApplyV',0x533520,107),('BrMatrix34TApplyV',0x533690,107)]
HEAP=0x22000000
ARENA=0x10000
COMMON=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "49-physics.h"
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison;
extern unsigned __cdecl BuilderInvoke(void);
static unsigned addresses[3],h[7],case_index;
static unsigned char*heap;
typedef char layouts[sizeof(void*)==4&&sizeof(tPhysics_object)==1240&&sizeof(tPhysics_joint_force)==96&&offsetof(tPhysics_joint_force,type)==0x5c&&offsetof(tPhysics_joint_force,direction_b)==0x38?1:-1];
static void invoke(unsigned mode,unsigned x,unsigned y,unsigned a,unsigned b){unsigned head[3];builder_mode=mode;builder_address=addresses[mode];builder_x=x;builder_y=y;builder_stack[0]=a;builder_stack[1]=b;builder_stack_count=mode?2:1;builder_poison=0xcccccccc;if(BuilderInvoke())exit(14);head[0]=case_index;head[1]=mode;head[2]=builder_result;fwrite(head,sizeof(head),1,stdout);fwrite(heap,0x10000,1,stdout);}
static int run(const char*path){FILE*f;SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);f=fopen(path,"rb");if(!f)return 10;heap=(unsigned char*)VirtualAlloc((void*)0x22000000,0x10000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(heap!=(void*)0x22000000)return 11;_setmode(_fileno(stdout),_O_BINARY);for(case_index=0;fread(h,sizeof(h),1,f)==1;case_index++){if(fread(heap,0x10000,1,f)!=1)return 12;_controlfp(_PC_53,_MCW_PC);if(h[0]<3)invoke(h[0],h[1],h[2],h[3],h[4]);else{invoke(1,0x22000100,0x22000900,0x22005000,0x22006000);invoke(2,0x22000900,0x22000100,0x22005100,0x22006010);invoke(0,0x22008000,0x22007000,2,0);}}fclose(f);return 0;}
'''

def fixtures():
 cases=[]
 def add(**v):
  c=dict(mode=0,count=2,type=10,second_type=11,partner=True,record=0x5000,axis=0x6000,output=0x8000,ieee=None,seed=len(cases)+1);c.update(v);cases.append(c)
 for seed in range(160):add(seed=seed+1,type=(10,11,0,-1)[seed%4],second_type=(11,10)[seed%2],partner=bool(seed%3))
 for count in (0,-1,-2147483648,1,2,3):
  for typ in (0,10,11,-1):add(count=count,type=typ)
 for mode in (1,2,3):
  for record in (0x5000,0x0170,0x0870,0x4300):
   for axis in (0x6000,record+0x2c,record+0x28,record+0x30,0x018c):add(mode=mode,record=record,axis=axis)
 for mode in (0,1,2,3):
  for typ in (10,11):
   for bits in (0,0x80000000,1,0x80000001,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc00001,0xffc00002,0x7f800003,0xff800004):add(mode=mode,type=typ,ieee=bits)
 for mode,slots in ((0,(0x174,0x178,0x17c,0x2a8,0x2ac,0x2b0,0x502c,0x5030,0x5034,0x5040)),(1,(0x188,0x18c,0x194,0x98c,0x6000,0x6004,0x4008)),(2,(0x18c,0x198,0x98c,0x6000,0x6008,0x4018)),(3,(0x18c,0x98c,0x6000,0x6014))):
  for typ in (10,11):
   for slot in slots:
    for bits in (0x7f800001,0xff800002,0x7fc12345,0xffc54321,0x7f800000,0xff800000):
     # +188 contains the actual joint pointer, not a scalar matrix lane.
     if slot==0x188:continue
     add(mode=mode,type=typ,overrides=[(slot,bits)])
 for mode,slots in ((0,(0x174,0x502c)),(0,(0x178,0x5030)),(0,(0x974,0x5038)),(0,(0x2ac,0x17c)),(1,(0x18c,0x6000)),(2,(0x98c,0x5030)),(3,(0x98c,0x6014))):
  for typ in (10,11):
   for a,b in ((0x7f800001,0xff800002),(0xff800002,0x7f800001),(0x7fc12345,0xffc54321),(0xffc54321,0x7fc12345),(0x7f800000,0xff800000)):
    add(mode=mode,type=typ,overrides=list(zip(slots,(a,b))))
 for mode in (0,1,2,3):
  for bits in (0x00800000,0x80800000,0x7f7fffff,0xff7fffff,0x2b800000,0x58800000):
   add(mode=mode,overrides=[(s,bits) for s in (0x174,0x178,0x17c,0x2a8,0x2ac,0x2b0,0x502c,0x5030,0x5034,0x6000,0x6004,0x6008)])
 for output in (0x5014,0x502c,0x5038,0x174,0x178,0x974,0x98c):
  for typ in (10,11):add(output=output,type=typ,count=3)
 for slots in ((0x5018,0x17c),(0x501c,0x178),(0x5014,0x17c),(0x501c,0x174),(0x5018,0x174),(0x5014,0x178),(0x5024,0x97c),(0x5028,0x978),(0x5020,0x97c),(0x5028,0x974)):
  for a,b in ((0x7f800001,0xff800002),(0xff800002,0x7f800001),(0x7fc12345,0xffc54321),(0x7fc12345,0xffc12345),(0xffc12345,0x7fc12345)):
   add(overrides=list(zip(slots,(a,b))))
 for reload in ('array','partner','type'):
  add(type=11,count=1,reload=reload)
 add(count=0,null_arguments=True)
 add(count=-1,null_arguments=True)
 return cases

def make_case(c):
 rng=random.Random(c['seed']);b=bytearray(words([rng.getrandbits(32) for _ in range(ARENA//4)]))
 def put(off,values):struct.pack_into('<'+'I'*len(values),b,off,*[v&0xffffffff for v in values])
 def scalar(off):
  for i in range(3):
   bits=c['ieee'] if c['ieee'] is not None and i==c['seed']%3 else struct.unpack('<I',struct.pack('<f',rng.uniform(-16,16)))[0]
   put(off+4*i,[bits])
 for obj,joint in ((0x100,0x4000),(0x900,0x4200),(0x1100,0x4400)):
  put(obj+0x188,[HEAP+joint]);scalar(obj+0x14);scalar(obj+0x74);scalar(obj+0x1a8)
  for i in range(3):scalar(obj+0x8c+12*i)
  scalar(joint+8);scalar(joint+0x14)
 for i in range(3):
  rec=0x5000+i*0x100;put(rec+4,[HEAP+0x100,HEAP+0x900 if c['partner'] else 0]);put(rec+0x5c,[c['type'] if i==0 else c['second_type']]);
  for off in (0x14,0x20,0x2c,0x38):scalar(rec+off)
 put(0x7000,[HEAP+0x5000,HEAP+0x5100,HEAP+0x5200]);scalar(0x6000);scalar(0x6010)
 for off,bits in c.get('overrides',[]):put(off,[bits])
 if c['mode']==0:h=[0,HEAP+c['output'],HEAP+0x7000,c['count'],0,0,c['seed']]
 else:h=[c['mode'],HEAP+0x100,HEAP+0x900,HEAP+c['record'],HEAP+c['axis'],0,c['seed']]
 if c.get('null_arguments'):h[1]=h[2]=0
 if 'reload' in c:
  target={'array':HEAP+0x5300,'partner':HEAP+0x1100,'type':0x47000000}[c['reload']];put(0x174,[target^0x80000000,0,0]);put(0x502c,[0x3f800000,0,0]);h[1]=HEAP+{'array':0x7000,'partner':0x5008,'type':0x505c}[c['reload']]
  put(0x5304,[HEAP+0x900,HEAP+0x1100]);put(0x535c,[10]);scalar(0x5338)
 return words(h)+b

def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,default=ROOT/'build-audit/physics-joint-foundations-tests');p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE');p.add_argument('--case-index',type=int,action='append');p.add_argument('--symbols',type=Path);p.add_argument('--recompiled',type=Path);a=p.parse_args();assert bool(a.symbols)==bool(a.recompiled);b=a.original.read_bytes();assert sha(b)==HASH;out=a.output.resolve();out.mkdir(parents=True,exist_ok=True)
 caller,physical,template=registry.literals()
 files=[SOURCE,SOURCE.with_suffix('.h'),MATRIX,ROOT/'src/carma2/include/rec2_types.h',ROOT/'src/carma2/hooks/c2_hooks.h',Path(__file__).resolve(),Path(registry.__file__),Path(extraction.__file__),Path(reference.__file__),template]
 source_before={x:x.read_bytes() for x in files}
 from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM
 cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;nt,base,size,tab=pe(b);allowed={e[1] for e in ENTRIES};closure=[]
 for name,start,n in ENTRIES:
  ins=list(cs.disasm(raw(b,start,n),start));assert sum(i.size for i in ins)==n
  for i in ins:
   for op in i.operands:
    if op.type==CS_OP_MEM:assert not(base<=op.mem.disp<base+size),(name,i.op_str)
   if i.mnemonic=='call' or i.mnemonic.startswith('j'):assert i.operands[0].type==CS_OP_IMM and (i.operands[0].imm in allowed or start<=i.operands[0].imm<start+n),(name,i.op_str)
  closure.append(dict(name=name,address=start,length=n,sha256=sha(raw(b,start,n))))
 (out/'original-addresses.bin').write_bytes(words([*[e[1]-base for e in ENTRIES[:3]],0]))
 old=' cmp builder_mode,8\n jne notwide\n fstp qword ptr builder_wide\n xor eax,eax\n notwide:\n cmp builder_mode,1\n je zeroreturn\n cmp builder_mode,10\n jne resultready\n zeroreturn:\n xor eax,eax\n resultready:';assert old in caller;caller=caller.replace(old,' xor eax,eax')
 patch='image[hdr[12]]=0xe9;relative=(unsigned)RayBoundary-((unsigned)image+hdr[12]+5);memcpy(image+hdr[12]+1,&relative,4);';assert patch in physical;physical=physical.replace(patch,'').replace('relative,','').replace('hdr[14]','hdr[4]').replace('n=hdr[13]','n=hdr[3]').replace('i<12;i++)addresses','i<3;i++)addresses')
 source=source_before[SOURCE].decode('utf-8');matrix=source_before[MATRIX].decode('utf-8');names=['PhysicsCopyWord','PhysicsSubtractScalar','PhysicsJointScalar','PhysicsJointSum','PhysicsJointProduct','PhysicsJointProductMemory','PhysicsJointAddScalar','PhysicsJointAddVelocity','PhysicsJointVelocityA','PhysicsJointVelocityB',*[e[0] for e in ENTRIES[:3]]];body='\n'.join(extract(source,n) for n in names)+'\n'+'\n'.join(extract(matrix,n) for n in ('Matrix34ApplyProductLoaded','Matrix34ApplyProductMemory','Matrix34ApplyAdd','BrMatrix34ApplyV','BrMatrix34TApplyV'))
 native=COMMON+'\n'+body+r'''
static volatile unsigned native_bodies[3];
static unsigned __fastcall adapter0(br_scalar*x,tPhysics_joint_force*const*y,int n){typedef void(C2_HOOK_FASTCALL*fn)(br_scalar*,tPhysics_joint_force*const*,int);((fn)(size_t)native_bodies[0])(x,y,n);return 0;}
static unsigned __fastcall adapter1(tPhysics_object*x,tPhysics_object*y,tPhysics_joint_force*r,const br_vector3*v){typedef void(C2_HOOK_FASTCALL*fn)(tPhysics_object*,tPhysics_object*,tPhysics_joint_force*,const br_vector3*);((fn)(size_t)native_bodies[1])(x,y,r,v);return 0;}
static unsigned __fastcall adapter2(tPhysics_object*x,tPhysics_object*y,tPhysics_joint_force*r,const br_vector3*v){typedef void(C2_HOOK_FASTCALL*fn)(tPhysics_object*,tPhysics_object*,tPhysics_joint_force*,const br_vector3*);((fn)(size_t)native_bodies[2])(x,y,r,v);return 0;}
int C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;INIT;return run(argv[1]);}
''';native=native.replace('INIT;',''.join(f'native_bodies[{i}]=(unsigned){e[0]};addresses[{i}]=(unsigned)adapter{i};' for i,e in enumerate(ENTRIES[:3])));registry.COMMON=COMMON;commands=registry.compile_sources(out,native,physical,caller);all_cases=fixtures();indices=a.case_index if a.case_index else list(range(len(all_cases)));results=[];failures=[]
 linked=None
 if a.symbols:
  data=a.recompiled.read_bytes();mapping=json.loads(a.symbols.read_text());assert mapping['format']==1 and mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(data);symbols={s['orig']:s['recomp'] for s in mapping['symbols']};map_bytes=a.recompiled.with_suffix('.map').read_bytes();functions={int(addr,16):(name,owner.strip()) for name,addr,owner in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',map_bytes.decode('latin1'),re.M)};nt,lb,ls,tab=pe(data);initial=[symbols[e[1]] for e in ENTRIES];names_={symbols[e[1]]:e[0] for e in ENTRIES};todo=list(initial);seen=set();linked_closure=[]
  while todo:
   start=todo.pop(0)
   if start in seen:continue
   seen.add(start);end=min(x for x in functions if x>start);name=names_.get(start,functions[start][0]);n=end-start;ins=list(cs.disasm(raw(data,start,n),start));assert sum(i.size for i in ins)==n
   for i in ins:
    for op in i.operands:
     if op.type==CS_OP_MEM:assert not(lb<=op.mem.disp<lb+ls),(name,i.op_str)
    if i.mnemonic=='call' or i.mnemonic.startswith('j'):
     assert i.operands[0].type==CS_OP_IMM,(name,i.op_str);target=i.operands[0].imm
     if start<=target<end:continue
     assert target in functions,(name,i.op_str)
     if target not in initial:assert functions[target][0] in {'_'+x for x in ('PhysicsCopyWord','PhysicsSubtractScalar','PhysicsJointScalar','PhysicsJointSum','PhysicsJointProduct','PhysicsJointProductMemory','PhysicsJointAddScalar','PhysicsJointAddVelocity','PhysicsJointVelocityA','PhysicsJointVelocityB')} and functions[target][1]=='49-physics.c.obj',(name,functions[target])
     todo.append(target)
   linked_closure.append(dict(name=name,address=start,length=n,sha256=sha(raw(data,start,n))))
  (out/'linked-addresses.bin').write_bytes(words([*[symbols[e[1]]-lb for e in ENTRIES[:3]],0]));linked=dict(recompiled_sha256=sha(data),symbols_sha256=sha(a.symbols.read_bytes()),map_sha256=sha(map_bytes),closure=linked_closure)
 for ci in indices:
  c=all_cases[ci];case=out/'case.bin';case.write_bytes(make_case(c));r=subprocess.run([str(out/'physical.exe'),str(case),str(a.original.resolve()),str(out/'original-addresses.bin')],capture_output=True,timeout=20);assert not r.returncode,('original',ci,r.returncode,r.stderr);expected=r.stdout;(out/'original.bin').write_bytes(expected);assert len(expected)%(ARENA+12)==0
  run_commands={n:[str(out/f'{n}.exe'),str(case)] for n in ('msvc5','msvc','mingw')}
  if linked:run_commands['physical-matching']=[str(out/'physical.exe'),str(case),str(a.recompiled.resolve()),str(out/'linked-addresses.bin')]
  for name,command in run_commands.items():
   r=subprocess.run(command,capture_output=True,timeout=20);assert not r.returncode,(name,ci,r.returncode,r.stderr);actual=r.stdout;(out/f'{name}.bin').write_bytes(actual)
   if actual!=expected:
    first=next((i for i,(x,y) in enumerate(zip(actual,expected)) if x!=y),-1);failure=dict(case_index=ci,case=c,compiler=name,first_byte=first,expected=expected[max(first-8,0):first+24].hex(),actual=actual[max(first-8,0):first+24].hex());failures.append(failure);(out/'failure.json').write_text(json.dumps(failure,indent=2));raise AssertionError(failure)
   results.append(dict(case_index=ci,compiler=name,output_sha256=sha(actual),records=len(actual)//(ARENA+12)))
  if (indices.index(ci)+1)%50==0:print(f'Verified{indices.index(ci)+1}/{len(indices)} joint foundation cases',flush=True)
 source_after={x:x.read_bytes() for x in files};assert source_before==source_after,'Source/verifier/helper changed during proof'
 manifest=dict(scenes=len(indices),comparisons=len(results),original_sha256=sha(b),entries=ENTRIES[:3],closure=closure,linked=linked,fixture_input_sha256=sha(b''.join(make_case(all_cases[i]) for i in indices)),case_indices=indices,cases=[all_cases[i] for i in indices],results=results,failures=failures,commands=commands,sources={str(x.relative_to(ROOT)):sha(source_before[x]) for x in files},source_before_equals_after=True,generated_files_sha256={n:sha((out/n).read_bytes()) for n in ('native.c','physical.c','caller.c','caller.obj','physical.exe','msvc5.exe','msvc.exe','mingw.exe')},limits=__doc__)
 (out/'verification.json').write_text(json.dumps(manifest,indent=2)+'\n');print(f'PASS {len(indices)} scenes/{len(results)} comparisons')

if __name__=='__main__':main()
