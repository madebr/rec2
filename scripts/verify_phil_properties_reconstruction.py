#!/usr/bin/env python3
"""Physical original/native PHIL properties and complete child-position closure.

Actual PositionChildren/InternalPositionChildren and BRender Mul/ApplyP bodies
execute; there are no callbacks or substituted math operations. Full arenas,
aliased global/flag neighborhoods, raw IEEE stores, ABI and returned status are
compared at PC53. Floating exception flags/non-x86 builds are outside the proof.
"""
import argparse,json,os,re,struct,subprocess,random
from pathlib import Path
from verify_tinted_native_reconstruction import extract
from verify_opponent_route_reconstruction import ROOT,HASH,pe,raw,sha,words
from verify_phil_registry_reconstruction import literals,compile_sources
import verify_phil_registry_reconstruction as registry

PHIL=ROOT/'src/carma2/newcommon/60-phil.c';PHYSICS=PHIL.parent/'49-physics.c';MATRIX=ROOT/'src/brender/core/math/matrix34.c'
ENTRIES=[('PositionChildren',0x4c2b10,7),('InternalPositionChildren',0x4c2b20,260),('PHILSetObjectProperty',0x4b63b0,339),('BrMatrix34Mul',0x532620,404),('BrMatrix34ApplyP',0x5334a0,116)]
GLOBALS={'gPHIL_disabled':(0x65d004,4),'gFace_num__car':(0x65d010,4)}
FIELDS='mode disabled property low high flags child_flags grand_flags tree root info face kind seed'.split()
RECORD_BYTES=16+0x1000+0x10000
COMMON=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdarg.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "60-phil.h"
#include "49-physics.h"
#include "00-car.h"
#include <brender/brender.h>
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison;
extern unsigned __cdecl BuilderInvoke(void);
static unsigned addresses[3],h[14],case_index;static unsigned char*heap,*guard;
#define OBJ(type,off) ((type*)(heap+0x200000+(off)))
#define OBJECT OBJ(tPhysics_object,0x100)
#define gPHIL_disabled (*(int*)(heap+0xcd004))
#define gFace_num__car (*(int*)(heap+0xcd010))
enum {MODE,DISABLED,PROPERTY,LOW,HIGH,FLAGS,CHILD_FLAGS,GRAND_FLAGS,TREE,ROOT_INDEX,INFO,FACE,KIND,SEED};
typedef char layouts[sizeof(void*)==4&&sizeof(tPhysics_object)==1240&&offsetof(tPhysics_object,physics_joint1)==0x188&&offsetof(tPhysics_object,field_0x1e8)==0x1e8&&offsetof(tPhysics_joint,field_0x14)==0x14?1:-1];
static tPhysics_object* setup(void){unsigned i;tPhysics_object*o;memset(heap+0xcc800,0xa5,0x1000);for(i=0;i<5;i++){o=OBJ(tPhysics_object,0x100+i*0x800);o->child=o->next=NULL;o->physics_joint1=OBJ(tPhysics_joint,0x5000+i*0x100);o->field_0x240=OBJ(tPHIL_queued_header,0x7000+i*0x100);o->field_0x239=1;o->flags=(int)(i==0?h[FLAGS]:i==3?h[GRAND_FLAGS]:h[CHILD_FLAGS]);}
if(h[TREE])OBJECT->child=OBJ(tPhysics_object,0x900);if(h[TREE]>=2)OBJ(tPhysics_object,0x900)->next=OBJ(tPhysics_object,0x1100);if(h[TREE]>=3)OBJ(tPhysics_object,0x900)->child=OBJ(tPhysics_object,0x1900);
o=OBJECT;if(h[INFO]==1)o->field_0x240=NULL;if(h[INFO]==2)o->field_0x240=(char*)&o->flags-4;if(h[INFO]==3)o->field_0x240=(char*)&o->field_0x240-4;if(h[INFO]==4)o->field_0x240=(char*)&gFace_num__car-4;if(h[INFO]==5){o=(tPhysics_object*)((char*)&gFace_num__car-0x19c);o->child=NULL;o->field_0x240=OBJ(tPHIL_queued_header,0x7000);o->flags=(int)h[FLAGS];}
if(h[INFO]==6){if(!guard){DWORD old_protect;guard=(unsigned char*)VirtualAlloc((void*)0x20500000,8192,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);if(guard!=(void*)0x20500000)exit(31);if(!VirtualProtect(guard+4096,4096,PAGE_NOACCESS,&old_protect))exit(32);}guard[4095]=0xa0;o->field_0x240=guard+4091;o->child=NULL;}
gPHIL_disabled=(int)h[DISABLED];gFace_num__car=(int)h[FACE];builder_poison=0xcccccccc;return o;}
static int run(const char*path){FILE*f;tPhysics_object*object;unsigned result[4];SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);f=fopen(path,"rb");if(!f)return 10;heap=(unsigned char*)VirtualAlloc((void*)0x20000000,0x300000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(heap!=(void*)0x20000000)return 11;_setmode(_fileno(stdout),_O_BINARY);for(case_index=0;fread(h,sizeof(h),1,f)==1;case_index++){if(fread(heap+0x200000,0x10000,1,f)!=1)return 12;object=setup();_controlfp(_PC_53,_MCW_PC);_clearfp();builder_mode=h[MODE];builder_address=addresses[h[MODE]];builder_x=(unsigned)object;builder_y=(unsigned)OBJ(tPhysics_object,0x100+h[ROOT_INDEX]*0x800);builder_stack_count=h[MODE]==2?4:0;builder_stack[0]=(unsigned)object;builder_stack[1]=h[PROPERTY];builder_stack[2]=h[LOW];builder_stack[3]=h[HIGH];if(BuilderInvoke())return 14;result[0]=case_index;result[1]=h[MODE];result[2]=builder_result;result[3]=h[INFO]==6?guard[4095]:0;fwrite(result,sizeof(result),1,stdout);fwrite(heap+0xcc800,0x1000,1,stdout);fwrite(heap+0x200000,0x10000,1,stdout);}fclose(f);return 0;}
'''

def fixtures():
 cases=[]
 def add(**v):
  c=dict(mode=2,disabled=0,property=0,low=0,high=0x3ff01234,flags=0xa5a5,child_flags=0,grand_flags=0,tree=3,root=4,info=0,face=0x12345678,kind=0,seed=len(cases)+1);c.update(v);cases.append(c)
 for mode in (0,1):
  for tree in range(4):
   for flags in (0,0x400,0xffffffff):
    for cf,gf in ((0,0),(0x400,0),(0,0x400),(0x400,0x400)):
     for root in (0,1,4):add(mode=mode,tree=tree,flags=flags,child_flags=cf,grand_flags=gf,root=root)
 for prop in (0xffffffff,*range(10),0x80000000,0x7fffffff):
  for disabled in (0,1,0xffffffff):
   for info in range(6):
    for value in (0,1,0xffffffff):add(property=prop,disabled=disabled,info=info,low=value)
 for prop in (0,2,8):
  for bits in (0,0x8000000000000000,1,0x8000000000000001,0x3ff0000010000000,0x3ff0000010000001,0x7fefffffffffffff,0xffefffffffffffff,0x7ff0000000000000,0xfff0000000000000,0x7ff8000012345678,0xfff8000012345678,0x7ff0000000000001,0xfff0000000000001,0x7ff7ffffffffffff):
   for info in (0,2,4):add(property=prop,low=bits&0xffffffff,high=bits>>32,info=info)
 for mode in (0,1,2):
  for kind in (1,2,3):
   for k in range(80):add(mode=mode,property=1,low=1,kind=kind,flags=(0,0x400)[k%2],child_flags=(0,0x400)[k%3==0],grand_flags=(0,0x400)[k%4==0],root=(0,1,4)[k%3])
 for flags in (0,1,4,8,0x400,0xffff,0xffff0000,0xffffffff):
  for property_ in (1,3,4,5,6,7):
   for value in (0,1,0x80000000):add(property=property_,low=value,flags=flags,face=flags,info=2)
 for prop in (1,6,7):
  for value in (1,0xffffffff):add(property=prop,low=value,info=6)
 return cases

def make_case(c):
 rng=random.Random(c['seed']);arena=bytearray(words([rng.getrandbits(32) for _ in range(0x10000//4)]));pool=(0,0x80000000,1,0x80000001,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc12345,0xffc56789,0x7f800001,0xff800001)
 def value():
  if c['kind']==1:return rng.choice(pool)
  if c['kind']==2:return (rng.randrange(255)<<23)|rng.getrandbits(23)|(rng.randrange(2)<<31)
  if c['kind']==3:return rng.choice((0x3f800000,0x5b000000,0xdb000000,0x3f800001,0xbf800000))
  return struct.unpack('<I',struct.pack('<f',rng.uniform(-3,3)))[0]
 for i in range(5):
  for off,n in ((0x100+i*0x800+0x8c,12),(0x100+i*0x800+0x1e8,12),(0x5000+i*0x100+8,6)):
   arena[off:off+n*4]=words([value() for _ in range(n)])
 return words([c[x] for x in FIELDS])+arena

def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,default=ROOT/'build-audit/phil-properties-tests');p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE');p.add_argument('--symbols',type=Path);p.add_argument('--recompiled',type=Path);p.add_argument('--case-index',type=int,action='append');a=p.parse_args();assert bool(a.symbols)==bool(a.recompiled);b=a.original.read_bytes();assert sha(b)==HASH;out=a.output.resolve();out.mkdir(parents=True,exist_ok=True)
 from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM
 cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
 def relocation(data,entries,globals_,table):
  nt,base,size,tab=pe(data);refs={};closure=[];allowed={e[1] for e in entries}
  def translated(value):
   for name,(start,n) in globals_.items():
    if start<=value<start+n:return 0x20000000+GLOBALS[name][0]-0x590000+value-start,1
   assert base<=value<base+size;return value-base,0
  for name,start,n in entries:
   instructions=list(cs.disasm(raw(data,start,n),start));assert sum(i.size for i in instructions)==n,(name,n)
   for i in instructions:
    if i.mnemonic=='call' or i.mnemonic.startswith('j'):
     op=i.operands[0]
     if op.type==CS_OP_IMM:assert op.imm in allowed or (i.mnemonic.startswith('j') and start<=op.imm<start+n),(name,hex(i.address),i.op_str)
     else:assert name=='PHILSetObjectProperty' and i.mnemonic=='jmp' and op.type==CS_OP_MEM and op.mem.disp==table
    for op in i.operands:
     if op.type==CS_OP_MEM and base<=op.mem.disp<base+size:
      assert i.disp_size==4;refs[i.address+i.disp_offset-base]=(op.mem.disp,*translated(op.mem.disp))
     if op.type==CS_OP_IMM and base<=op.imm<base+size and not(i.mnemonic=='call' or i.mnemonic.startswith('j')):
      assert i.imm_size==4;refs[i.address+i.imm_offset-base]=(op.imm,*translated(op.imm))
   closure.append(dict(name=name,address=start,length=n,sha256=sha(raw(data,start,n))))
  if table is not None:
   for i in range(9):
    value=struct.unpack('<I',raw(data,table+4*i,4))[0];assert entries[2][1]<=value<entries[2][1]+entries[2][2];refs[table+4*i-base]=(value,*translated(value))
  return dict(base=base,size=size,refs=refs,closure=closure,entries=[e[1]-base for e in entries[:3]])
 def save(meta,path):path.write_bytes(words([*meta['entries'],len(meta['refs'])])+b''.join(words([k,*v]) for k,v in sorted(meta['refs'].items())))
 original=relocation(b,ENTRIES,GLOBALS,0x4b6504);save(original,out/'original-addresses.bin')
 caller,physical,template=literals()
 # CDECL property setter returns status and leaves all argument words to caller.
 caller=caller.replace(' cmp builder_mode,8\n jne notwide\n fstp qword ptr builder_wide\n xor eax,eax\n notwide:\n cmp builder_mode,1\n je zeroreturn\n cmp builder_mode,10\n jne resultready\n zeroreturn:\n xor eax,eax\n resultready:', ' cmp builder_mode,2\n je resultready\n xor eax,eax\n resultready:')
 caller=caller.replace(' mov builder_result,eax\n mov ecx,esp',' mov builder_result,eax\n cmp builder_mode,2\n jne stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n stackready:\n mov ecx,esp')
 physical=physical.replace('hdr[14]','hdr[4]').replace('n=hdr[13]','n=hdr[3]').replace('i<12;i++)addresses','i<3;i++)addresses');start=physical.index('image[hdr[12]]=0xe9;');end=physical.index('free(patch);',start);physical=physical[:start]+physical[end:];physical=physical.replace('value,relative,hdr','value,hdr')
 body='\n'.join(extract(PHYSICS.read_text(),n) for n in ('PhysicsPositionChildrenAdjust','InternalPositionChildren','PositionChildren'))+'\n'+extract(PHIL.read_text(),'PHILSetObjectProperty')+'\n'+'\n'.join(extract(MATRIX.read_text(),n) for n in ('BrMatrix34Mul','BrMatrix34ApplyP'))
 native=COMMON+body+r'''
static volatile unsigned native_bodies[3];
static unsigned __fastcall adapter0(tPhysics_object*x,tPhysics_object*y){typedef void (C2_HOOK_FASTCALL *body_fn)(tPhysics_object*);(void)y;((body_fn)(size_t)native_bodies[0])(x);return 0;}
static unsigned __fastcall adapter1(tPhysics_object*x,tPhysics_object*y){typedef void (C2_HOOK_FASTCALL *body_fn)(tPhysics_object*,tPhysics_object*);((body_fn)(size_t)native_bodies[1])(x,y);return 0;}
static unsigned __cdecl adapter2(tPhysics_object*x,int property,unsigned low,unsigned high){typedef int (C2_HOOK_CDECL *body_fn)(tPhysics_object*,int,...);unsigned raw_value[2];double value;raw_value[0]=low;raw_value[1]=high;memcpy(&value,raw_value,8);if(property==0||property==2||property==8)return ((body_fn)(size_t)native_bodies[2])(x,property,value);return ((body_fn)(size_t)native_bodies[2])(x,property,(int)low);}
int C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;native_bodies[0]=(unsigned)PositionChildren;native_bodies[1]=(unsigned)InternalPositionChildren;native_bodies[2]=(unsigned)PHILSetObjectProperty;addresses[0]=(unsigned)adapter0;addresses[1]=(unsigned)adapter1;addresses[2]=(unsigned)adapter2;return run(argv[1]);}
'''
 # Compiler orchestration is shared; each suite supplies its complete own driver.
 registry.COMMON=COMMON
 commands=compile_sources(out,native,physical,caller);all_cases=fixtures();cases=[all_cases[i] for i in a.case_index] if a.case_index else all_cases;fixture=out/'cases.bin';fixture.write_bytes(b''.join(make_case(c) for c in cases));results=[];failures=[]
 def execute(name,cmd):
  with (out/f'{name}.bin').open('wb') as f:r=subprocess.run(cmd,stdout=f,stderr=subprocess.PIPE,timeout=120)
  assert not r.returncode,(name,r.returncode,r.stderr)
 def compare(name):
  e=(out/'original.bin').read_bytes();v=(out/f'{name}.bin').read_bytes();assert len(e)==len(v)==RECORD_BYTES*len(cases)
  if e!=v:
   first=next(i for i,(x,y) in enumerate(zip(e,v)) if x!=y);ci=first//RECORD_BYTES;failure=dict(compiler=name,case_index=ci,case=cases[ci],record_byte=first%RECORD_BYTES,original=e[max(0,first-12):first+20].hex(),actual=v[max(0,first-12):first+20].hex());failures.append(failure);(out/f'{name}-failure.json').write_text(json.dumps(failure,indent=2))
  results.append(dict(compiler=name,scenes=len(cases),match=e==v,output_sha256=sha(v)))
 original_command=[str(out/'physical.exe'),str(fixture),str(a.original.resolve()),str(out/'original-addresses.bin')]
 execute('original',original_command)
 for name in ('msvc5','msvc','mingw'):execute(name,[str(out/f'{name}.exe'),str(fixture)]);compare(name)
 linked=None;linked_command=None
 if a.symbols:
  data=a.recompiled.read_bytes();mapping=json.loads(a.symbols.read_text());assert mapping['format']==1 and mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(data);symbols={s['orig']:s['recomp'] for s in mapping['symbols']};map_bytes=a.recompiled.with_suffix('.map').read_bytes();functions={int(addr,16):(name,owner.strip()) for name,addr,owner in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',map_bytes.decode('latin1'),re.M)};initial=[symbols[e[1]] for e in ENTRIES];names={symbols[e[1]]:e[0] for e in ENTRIES};pending=list(initial);seen=set();entries=[];table=None
  while pending:
   start=pending.pop(0)
   if start in seen:continue
   seen.add(start);end=min(x for x in functions if x>start);name=names.get(start,functions[start][0]);instructions=list(cs.disasm(raw(data,start,end-start),start))
   if name=='PHILSetObjectProperty':
    for i in instructions:
     if i.mnemonic=='jmp' and i.operands[0].type==CS_OP_MEM:table=i.operands[0].mem.disp;assert start<table and table+36<=end;end=table;break
   entries.append((name,start,end-start))
   for i in cs.disasm(raw(data,start,end-start),start):
    if not(i.mnemonic=='call' or i.mnemonic.startswith('j')):continue
    op=i.operands[0]
    if op.type!=CS_OP_IMM:assert name=='PHILSetObjectProperty' and i.mnemonic=='jmp' and op.type==CS_OP_MEM;continue
    if start<=op.imm<end:continue
    assert op.imm in functions,(name,i.op_str)
    if op.imm not in initial:assert functions[op.imm]==('_PhysicsPositionChildrenAdjust','49-physics.c.obj'),(name,functions[op.imm])
    pending.append(op.imm)
  by_address={e[1]:e for e in entries};entries=[by_address[x] for x in initial]+[e for e in entries if e[1] not in initial];linked=relocation(data,entries,{n:(symbols[old],size) for n,(old,size) in GLOBALS.items()},table);linked.update(recompiled_sha256=sha(data),symbols_sha256=sha(a.symbols.read_bytes()),map_sha256=sha(map_bytes));save(linked,out/'linked-addresses.bin');linked_command=[str(out/'physical.exe'),str(fixture),str(a.recompiled.resolve()),str(out/'linked-addresses.bin')];execute('physical-matching',linked_command);compare('physical-matching')
 faults=[]
 for prop in (1,6,7):
  c=dict(all_cases[0],mode=2,property=prop,low=0,info=6);path=out/f'word-and-fault-{prop}.bin';path.write_bytes(make_case(c));cmds={'original':[original_command[0],str(path),*original_command[2:]],**{n:[str(out/f'{n}.exe'),str(path)] for n in ('msvc5','msvc','mingw')}}
  if linked_command:cmds['physical-matching']=[linked_command[0],str(path),*linked_command[2:]]
  codes={}
  for name,command in cmds.items():
   result=subprocess.run(command,stdout=subprocess.PIPE,stderr=subprocess.PIPE,timeout=30);codes[name]=result.returncode&0xffffffff
  assert set(codes.values())=={0xc0000005},(prop,codes)
  faults.append(dict(property=prop,input_sha256=sha(path.read_bytes()),process_returncodes=codes))
 paths=[PHIL,PHIL.with_suffix('.h'),PHYSICS,PHYSICS.with_suffix('.h'),MATRIX,MATRIX.with_suffix('.h'),ROOT/'src/carma2/include/rec2_types.h',template,Path(registry.__file__),Path(__file__).resolve()]
 manifest=dict(scenes=len(cases),comparisons=len(cases)*len(results),fault_probes=faults,fault_comparisons=len(faults)*len(results),entries=ENTRIES[:3],original=original,linked=linked,original_sha256=sha(b),input_sha256=sha(fixture.read_bytes()),results=results,failures=failures,commands=commands,selected_case_indices=a.case_index,sources={str(p.relative_to(ROOT)):sha(p.read_bytes()) for p in paths},generated_files_sha256={n:sha((out/n).read_bytes()) for n in ('caller.c','caller.obj','native.c','physical.c','physical.exe','msvc5.exe','msvc.exe','mingw.exe')},limits=__doc__+' Guard faults compare access-violation class only; no post-fault state claim.')
 (out/'verification.json').write_text(json.dumps(manifest,indent=2)+'\n')
 if failures:raise SystemExit(json.dumps(failures,indent=2))
 print(f'PASS {len(cases)} scenes /{manifest["comparisons"]} comparisons')

if __name__=='__main__':main()
