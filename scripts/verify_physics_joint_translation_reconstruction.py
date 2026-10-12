"""Complete translation/actor-position physical original/native/linked proof.

Actual two entries and actual published BRender transforms execute at PC53.
All 65536 fixture bytes compare; no output normalization or game boundaries.
Default extraction requires both actual production entries; --candidate-source
is a complete scratch override. Optional immutable PE/map binds linked bodies.
ESP, preserved registers and x87 TOP checked; void EAX excluded. Floating
status/traps, invalid pointer storage and non-x86 builds are outside this proof.
"""
import argparse,json,random,re,struct,subprocess,sys
from pathlib import Path
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'src/carma2').is_dir())
sys.path.insert(0,str(ROOT/'scripts'))
from verify_tinted_native_reconstruction import extract
from verify_opponent_route_reconstruction import HASH,pe,raw,sha,words
import verify_phil_registry_reconstruction as registry
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM

HEAP=0x22000000
ARENA=65536
SPEC=[('TranslateObject',0x4b8fe0,585),('WriteNewPositions',0x4c2a10,246),('BrMatrix34ApplyV',0x533520,107),('BrMatrix34ApplyP',0x5334a0,116),('BrMatrix34Mul',0x532620,416)]
COMMON=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "49-physics.h"
#define M(x,y) mat->m[x][y]
#define A(x,y) A->m[x][y]
#define B(x,y) B->m[x][y]
#define C(x,y) C->m[x][y]
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison;
extern unsigned __cdecl BuilderInvoke(void);
static unsigned addresses[2],h[7],case_index;
static unsigned char*heap;
typedef char layout_assert[sizeof(void*)==4&&sizeof(tPhysics_object)==1240&&offsetof(tPhysics_object,field_0x17c)==0x17c&&offsetof(br_actor,t)==0x28&&offsetof(br_transform,t)==4?1:-1];
static void invoke(unsigned mode,unsigned object,unsigned time){unsigned head[3];builder_mode=mode;builder_address=addresses[mode];builder_x=object;builder_y=0xcccccccc;builder_stack[0]=time;builder_stack_count=mode?0:1;builder_poison=0xcccccccc;if(BuilderInvoke())exit(14);head[0]=case_index;head[1]=mode;head[2]=builder_result;fwrite(head,sizeof(head),1,stdout);fwrite(heap,65536,1,stdout);}
static int run(const char*path){FILE*f;SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);f=fopen(path,"rb");if(!f)return 10;heap=(unsigned char*)VirtualAlloc((void*)0x22000000,65536,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(heap!=(void*)0x22000000)return 11;_setmode(_fileno(stdout),_O_BINARY);for(case_index=0;fread(h,sizeof(h),1,f)==1;case_index++){if(fread(heap,65536,1,f)!=1)return 12;_controlfp(_PC_53,_MCW_PC);_clearfp();if(h[0]<2)invoke(h[0],h[1],h[2]);else{invoke(0,h[1],h[2]);invoke(1,h[1],h[2]);}}fclose(f);return 0;}
'''

def fixtures():
 cases=[]
 def add(**v):
  c=dict(mode=0,type=5,flags=0,tree=0,time=0x3dcccccd,seed=len(cases)+1,alias=None,patch=[]);c.update(v);cases.append(c)
 for seed in range(180):add(seed=seed+1,type=(0,1,4,5,10,11,-1)[seed%7])
 for mode in (1,2):
  for tree in range(5):
   for flags in (0,0x400,0x401,0xffffffff):
    for seed in range(6):add(mode=mode,flags=flags,tree=tree,seed=seed+10)
 for typ in (0,5):
  for time in (0,0x80000000,1,0x80000001,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc12345,0xffc54321,0x7f812345,0xff812345):add(type=typ,time=time)
 for mode in (0,1,2):
  for alias in ('shared_actor','actor_v','actor_anchor','actor_matrix','root_actor','actor_flags','child_actor_flags'):
   for flags in (0,0x400):add(mode=mode,flags=flags,tree=4,alias=alias)
 for typ in (0,5):
  for slot in (0x17c,0x180,0x184,0x68,0x6c,0x70,0x8c,0x90,0x94,0x4008,0x400c,0x4010,0x4038,0x403c,0x4040,0x5050,0x5054,0x5058):
   for bits in (0,0x80000000,1,0x80000001,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc12345,0xffc54321,0x7f812345,0xff812345):add(type=typ,patch=[(slot+0x100 if slot<0x300 else slot,bits)])
 for mode in (0,1,2):
  for slots in ((0x5050,0x4038),(0x5054,0x403c),(0x278,0x4008),(0x5050,0x280),(0x168,0x4038),(0x1698,0x5058),(0x900+0x17c,0x5150),(0x5050,0x5150)):
   for a,b in ((0x7f812345,0xff812346),(0xff812346,0x7f812345),(0x7fc12345,0xffc54321),(0xffc54321,0x7fc12345),(0x7f800000,0xff800000),(0x7f7fffff,1)):
    add(mode=mode,tree=4,patch=list(zip(slots,(a,b))))
 for mode in (0,1,2):
  for typ in (0,5):
   for bits in (0x00800000,0x80800000,0x2b800000,0x58800000,0x7f7fffff,0xff7fffff):
    for seed in range(8):
     add(mode=mode,type=typ,tree=4,seed=seed+200,patch=[(slot,bits) for slot in (0x168,0x16c,0x170,0x27c,0x280,0x284,0x4038,0x403c,0x4040,0x5050,0x5054,0x5058)])
 # Equal payloads with opposite signs expose tied-NaN FADDP operand selection.
 for mode in (0,2):
  for slots in ((0x5054,0x5058),(0x5050,0x5058),(0x168,0x16c),(0x168,0x170),(0x16c,0x170)):
   for a,b in ((0x7fc12345,0xffc12345),(0xffc12345,0x7fc12345),(0x7f812345,0xff812345),(0xff812345,0x7f812345),(0x7fc12345,0xff812345),(0xff812345,0x7fc12345)):
    for seed in (1041,2042,3043,4044):add(mode=mode,type=5,tree=4,seed=seed,patch=list(zip(slots,(a,b))))
 return cases

def make_case(c):
 rng=random.Random(c['seed']);b=bytearray(words([rng.getrandbits(32) for _ in range(ARENA//4)]))
 def put(off,*values):struct.pack_into('<'+'I'*len(values),b,off,*[v&0xffffffff for v in values])
 def scalar(off,n=3):
  for i in range(n):struct.pack_into('<f',b,off+4*i,rng.uniform(-8,8))
 objects=(0x100,0x900,0x1100,0x1900,0x2100)
 actors=[0x5000+i*0x100 for i in range(5)]
 if c['alias']=='shared_actor':actors[1]=actors[0]
 if c['alias']=='actor_v':actors[0]=0x100+0x68-0x50
 if c['alias']=='actor_anchor':actors[0]=0x100+0x17c-0x50
 if c['alias']=='actor_matrix':actors[0]=0x100+0x8c-0x2c
 if c['alias']=='root_actor':actors[1]=actors[4]
 if c['alias']=='actor_flags':actors[0]=0x100+0x19c-0x50
 if c['alias']=='child_actor_flags':actors[1]=0x900+0x19c-0x50
 for i,obj in enumerate(objects):
  scalar(obj+0x68);scalar(obj+0x17c);scalar(obj+0x8c,12);scalar(obj+0x1e8,12)
  scalar(actors[i]+0x2c,12);joint=0x4000+i*0x100
  scalar(joint+8);scalar(joint+0x14);scalar(joint+0x38);put(joint,c['type'])
 # Restore control/pointer words after any deliberate scalar-storage aliases.
 for i,obj in enumerate(objects):
  put(obj,HEAP+actors[i]);put(obj+0x188,HEAP+0x4000+i*0x100,HEAP+0x4000+i*0x100)
  put(obj+0x19c,c['flags'] if i==0 else 0);put(obj+0x220,0,0,0,0,HEAP+objects[4])
 tree=c['tree']
 if tree>=1:put(objects[0]+0x224,HEAP+objects[1])
 if tree>=2:put(objects[1]+0x220,HEAP+objects[2])
 if tree>=3:put(objects[1]+0x224,HEAP+objects[3])
 if tree>=4:put(objects[2]+0x19c,0x400)
 for off,bits in c['patch']:put(off,bits)
 return words([c['mode'],HEAP+0x100,c['time'],0,0,0,c['seed']])+b

def main():
 p=argparse.ArgumentParser();p.add_argument('--output',required=True,type=Path);p.add_argument('--case-index',type=int,action='append');p.add_argument('--limit',type=int);p.add_argument('--candidate-source',type=Path);p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);args=p.parse_args();assert bool(args.recompiled)==bool(args.symbols);out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)
 source=ROOT/'src/carma2/newcommon/49-physics.c';matrix=ROOT/'src/brender/core/math/matrix34.c';candidate=args.candidate_source.resolve() if args.candidate_source else None;caller,physical,template=registry.literals()
 paths=[source,source.with_suffix('.h'),matrix,Path(__file__),template,Path(registry.__file__),ROOT/'src/carma2/include/rec2_types.h',ROOT/'src/carma2/hooks/c2_hooks.h']
 
 if candidate:paths.append(candidate)
 if args.symbols:paths.extend([args.recompiled,args.recompiled.with_suffix('.map'),args.symbols])
 before={str(x):x.read_bytes() for x in paths};original=ROOT/'build-reference/originals/CARMA2_HW.EXE';data=original.read_bytes();assert sha(data)==HASH;nt,base,size,tab=pe(data);cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;closure=[]
 for name,start,n in SPEC:
  ins=list(cs.disasm(raw(data,start,n),start));assert sum(i.size for i in ins)==n
  for i in ins:
   for op in i.operands:
    if op.type==CS_OP_MEM:assert not base<=op.mem.disp<base+size,(name,i.op_str)
   if i.mnemonic=='call' or i.mnemonic.startswith('j'):assert i.operands[0].type==CS_OP_IMM and (start<=i.operands[0].imm<start+n or i.operands[0].imm in {a for _,a,_ in SPEC}),(name,i.op_str)
  closure.append(dict(name=name,address=start,bytes=n,sha256=sha(raw(data,start,n))))
 (out/'original-addresses.bin').write_bytes(words([SPEC[0][1]-base,SPEC[1][1]-base,0]))
 old=' cmp builder_mode,8\n jne notwide\n fstp qword ptr builder_wide\n xor eax,eax\n notwide:\n cmp builder_mode,1\n je zeroreturn\n cmp builder_mode,10\n jne resultready\n zeroreturn:\n xor eax,eax\n resultready:';assert old in caller;caller=caller.replace(old,' xor eax,eax')
 patch='image[hdr[12]]=0xe9;relative=(unsigned)RayBoundary-((unsigned)image+hdr[12]+5);memcpy(image+hdr[12]+1,&relative,4);';assert patch in physical;physical=physical.replace(patch,'').replace('relative,','').replace('hdr[14]','hdr[3]').replace('n=hdr[13]','n=hdr[2]').replace('i<12;i++)addresses','i<2;i++)addresses')
 src=before[str(source)].decode();mat=before[str(matrix)].decode();body='\n'.join(extract(src,n) for n in ('PhysicsJointScalar','PhysicsJointSum','PhysicsJointProductMemory','PartnerReverseSubtractScalar','PartnerMultiplyRawScalar','PhysicsJointAddScalar','PartnerDifferenceScalars'))+'\n'+(before[str(candidate)].decode() if candidate else '\n'.join(extract(src,n) for n in ('TranslateObject','WriteNewPositions'))) +'\n'+'\n'.join(extract(mat,name) for name in ('BrMatrix34ApplyV','BrMatrix34ApplyP','BrMatrix34Mul'))
 native=COMMON+'\nvoid C2_HOOK_FASTCALL WriteNewPositions(tPhysics_object* object);\n'+body+r'''
static volatile unsigned native_bodies[2];
static unsigned __fastcall adapter0(tPhysics_object*object,undefined4 unused,br_scalar time){typedef void(C2_HOOK_FAKE_THISCALL*fn)(tPhysics_object*,undefined4,br_scalar);((fn)(size_t)native_bodies[0])(object,unused,time);return 0;}
static unsigned __fastcall adapter1(tPhysics_object*object){typedef void(C2_HOOK_FASTCALL*fn)(tPhysics_object*);((fn)(size_t)native_bodies[1])(object);return 0;}
int C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;native_bodies[0]=(unsigned)TranslateObject;native_bodies[1]=(unsigned)WriteNewPositions;addresses[0]=(unsigned)adapter0;addresses[1]=(unsigned)adapter1;return run(argv[1]);}
'''
 registry.COMMON=COMMON;commands=registry.compile_sources(out,native,physical,caller);cases=fixtures();indices=args.case_index or list(range(len(cases)));indices=indices[:args.limit] if args.limit else indices;results=[];failures=[]
 linked=None
 if args.symbols:
  data=args.recompiled.read_bytes();mapping=json.loads(args.symbols.read_text());assert mapping['format']==1 and mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(data)
  symbols={s['orig']:s['recomp'] for s in mapping['symbols']};map_bytes=args.recompiled.with_suffix('.map').read_bytes();functions={int(addr,16):(name,owner.strip()) for name,addr,owner in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',map_bytes.decode('latin1'),re.M)}
  nt,lb,ls,tab=pe(data);initial=[symbols[e[1]] for e in SPEC];names_={symbols[e[1]]:e[0] for e in SPEC};todo=list(initial);seen=set();linked_closure=[]
  helpers={'_'+x for x in ('PhysicsJointScalar','PhysicsJointSum','PhysicsJointProductMemory','PartnerReverseSubtractScalar','PartnerMultiplyRawScalar','PhysicsJointAddScalar','PartnerDifferenceScalars')}
  while todo:
   start=todo.pop(0)
   if start in seen:continue
   seen.add(start);end=min(x for x in functions if x>start);name=names_.get(start,functions[start][0]);n=end-start;ins=list(cs.disasm(raw(data,start,n),start));assert sum(i.size for i in ins)==n
   for i in ins:
    for op in i.operands:
     if op.type==CS_OP_MEM:assert not lb<=op.mem.disp<lb+ls,(name,i.op_str)
    if i.mnemonic=='call' or i.mnemonic.startswith('j'):
     assert i.operands[0].type==CS_OP_IMM,(name,i.op_str);target=i.operands[0].imm
     if start<=target<end:continue
     assert target in functions,(name,i.op_str)
     if target not in initial:assert functions[target][0] in helpers and functions[target][1]=='49-physics.c.obj',(name,functions[target])
     todo.append(target)
   linked_closure.append(dict(name=name,address=start,length=n,sha256=sha(raw(data,start,n))))
  (out/'linked-addresses.bin').write_bytes(words([symbols[SPEC[0][1]]-lb,symbols[SPEC[1][1]]-lb,0]))
  linked=dict(recompiled_sha256=sha(data),symbols_sha256=sha(args.symbols.read_bytes()),map_sha256=sha(map_bytes),closure=linked_closure)
 for index in indices:
  c=cases[index];path=out/'case.bin';path.write_bytes(make_case(c));r=subprocess.run([str(out/'physical.exe'),str(path),str(original),str(out/'original-addresses.bin')],capture_output=True,timeout=20);assert r.returncode==0,('original',index,r.returncode,r.stderr);expected=r.stdout;assert len(expected)==(2 if c['mode']==2 else 1)*(ARENA+12)
  runs={n:[str(out/f'{n}.exe'),str(path)] for n in ('msvc5','msvc','mingw')}
  if linked:runs['physical-matching']=[str(out/'physical.exe'),str(path),str(args.recompiled),str(out/'linked-addresses.bin')]
  for compiler,command in runs.items():
   r=subprocess.run(command,capture_output=True,timeout=20);assert not r.returncode,(compiler,index,r.returncode,r.stderr)
   if r.stdout!=expected:
    first=next(i for i,(a,b) in enumerate(zip(expected,r.stdout)) if a!=b);first-=first%4;failure=dict(index=index,case=c,compiler=compiler,offset=first,expected=expected[first:first+4].hex(),actual=r.stdout[first:first+4].hex());failures.append(failure);(out/'failure.json').write_text(json.dumps(failure,indent=2));(out/'failure-case.bin').write_bytes(path.read_bytes());(out/'original.bin').write_bytes(expected);(out/f'{compiler}.bin').write_bytes(r.stdout);break
   results.append(dict(index=index,compiler=compiler,sha256=sha(r.stdout)))
  if failures:break
  if (indices.index(index)+1)%50==0:print(f'Verified {indices.index(index)+1}/{len(indices)} scenes',flush=True)
 after={str(x):x.read_bytes() for x in paths};assert before==after,'Input changed during verification'
 manifest=dict(scenes=indices.index(index)+1,comparisons=len(results),complete=len(results)==len(indices)*(4 if linked else 3),failures=failures,results=results,cases=[cases[i] for i in indices],closure=closure,linked=linked,fixture_input_sha256=sha(b''.join(make_case(cases[i]) for i in indices)),original_sha256=HASH,sources={x:sha(v) for x,v in before.items()},source_before_equals_after=True,commands=commands,generated_files_sha256={x.name:sha(x.read_bytes()) for x in out.iterdir() if x.suffix in ('.c','.exe','.obj')},limits=__doc__)
 (out/'verification.json').write_text(json.dumps(manifest,indent=2)+'\n');print('PASS' if not failures else 'FAIL',manifest['scenes'],manifest['comparisons']);return bool(failures)
if __name__=='__main__':raise SystemExit(main())
