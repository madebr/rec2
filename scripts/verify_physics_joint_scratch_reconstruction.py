#!/usr/bin/env python3
"""Physical original/native joint counting and physics scratch cursor proof.

All four production entries plus actual PhysicsError execute. Only external
CRT exit is controlled: it records the code/full state, then terminates the
fixture process. Error callbacks are real ABI callbacks that may mutate state.
No joint counting or allocation decision is substituted. Normal returns check
ESP, preserved registers and x87 TOP; terminal paths have no return-ABI claim.
"""
import argparse,json,re,struct,subprocess,random
from pathlib import Path
from verify_tinted_native_reconstruction import extract
from verify_opponent_route_reconstruction import ROOT,HASH,pe,raw,sha,words
from verify_phil_registry_reconstruction import literals,compile_sources
import verify_phil_registry_reconstruction as registry

SOURCE=ROOT/'src/carma2/newcommon/49-physics.c'
ENTRIES=[('CountJointForces',0x4b9650,222),('GetPhysicsScratchPointer',0x4c5c70,58),('IncreasePhysicsScratchPointer',0x4c5cb0,70),('ClearPhysicsScratchSpace',0x4c5d00,11),('PhysicsError',0x4c61c0,25)]
GLOBALS={'gPhysics_scratch_ptr':(0x6940bc,4),'gPhysics_error_cb':(0x6940c0,4),'gPhysics_other_buffer':(0x6940c4,4),'gPhysics_other_buffer_capacity':(0x6940c8,4),'gPhysics_empty_message':(0x676920,1)}
FIELDS='mode size cursor buffer capacity callback mutation newcursor newbuffer newcapacity primary secondary flags childtype childsecondary childflags tree rootstate root seed'.split()
RECORD_BYTES=24+16+1+0x10000
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
static unsigned addresses[4],h[20],case_index,event_count;static unsigned char*heap;static void*selected_callback;
#define OBJ(type,off) ((type*)(heap+0x200000+(off)))
#define OBJECT OBJ(tPhysics_object,0x100)
#define gPhysics_scratch_ptr (*(void**)(heap+0x1040bc))
#define gPhysics_error_cb (*(tPhysicsError_cbfn**)(heap+0x1040c0))
#define gPhysics_other_buffer (*(void**)(heap+0x1040c4))
#define gPhysics_other_buffer_capacity (*(int*)(heap+0x1040c8))
#define gPhysics_empty_message (*(char*)(heap+0xe6920))
enum {MODE,SIZE,CURSOR,BUFFER,CAPACITY,CALLBACK,MUTATION,NEWCURSOR,NEWBUFFER,NEWCAPACITY,PRIMARY,SECONDARY,FLAGS,CHILDTYPE,CHILDSECONDARY,CHILDFLAGS,TREE,ROOTSTATE,ROOT_INDEX,SEED};
typedef char layouts[sizeof(void*)==4&&sizeof(tPhysics_object)==1240&&offsetof(tPhysics_object,field_0x230)==0x230&&offsetof(tPhysics_object,physics_joint1)==0x188?1:-1];
static void event(unsigned code,unsigned value,unsigned message){unsigned head[6],globals[4];head[0]=case_index;head[1]=code;head[2]=value;head[3]=message;head[4]=event_count++;head[5]=0;globals[0]=(unsigned)gPhysics_scratch_ptr;globals[1]=gPhysics_error_cb?1:0;if(gPhysics_error_cb&&(void*)gPhysics_error_cb!=selected_callback)exit(35);globals[2]=(unsigned)gPhysics_other_buffer;globals[3]=(unsigned)gPhysics_other_buffer_capacity;fwrite(head,sizeof(head),1,stdout);fwrite(globals,sizeof(globals),1,stdout);fwrite(&gPhysics_empty_message,1,1,stdout);fwrite(heap+0x200000,0x10000,1,stdout);}
static void __fastcall ErrorBoundary(tPhysicsError code,const char*message){if(message&&message!=&gPhysics_empty_message)exit(36);event(1,(unsigned)code,(unsigned)message);if(h[MUTATION]&1)gPhysics_scratch_ptr=(void*)h[NEWCURSOR];if(h[MUTATION]&2)gPhysics_other_buffer=(void*)h[NEWBUFFER];if(h[MUTATION]&4)gPhysics_other_buffer_capacity=(int)h[NEWCAPACITY];if(h[MUTATION]&8)gPhysics_error_cb=NULL;}
void C2_HOOK_FASTCALL NativeErrorCallback(tPhysicsError code,const char*message){ErrorBoundary(code,message);}
static void C2_NORETURN C2_HOOK_CDECL ControlledExit(int code){event(2,(unsigned)code,0);exit(0);}
static unsigned invoke(unsigned mode,unsigned x,unsigned y){builder_mode=mode;builder_address=addresses[mode];builder_x=x;builder_y=y;builder_stack_count=0;if(BuilderInvoke())exit(14);event(0,builder_result,0);return builder_result;}
static void setup(void){unsigned i; tPhysics_object*object;tPhysics_joint*j1,*j2;event_count=0;for(i=0;i<5;i++){object=OBJ(tPhysics_object,0x100+i*0x800);j1=OBJ(tPhysics_joint,0x5000+i*0x100);j2=OBJ(tPhysics_joint,0x6000+i*0x100);object->physics_joint1=j1;object->physics_joint2=j2;object->child=object->next=NULL;object->flags=(int)(i?h[CHILDFLAGS]:h[FLAGS]);j1->type=(tPhysics_joint_type)(i?h[CHILDTYPE]:h[PRIMARY]);j2->type=(tPhysics_joint_type)(i?h[CHILDSECONDARY]:h[SECONDARY]);memcpy((char*)object+0x1e4,&h[ROOTSTATE],4);}
if(h[PRIMARY]==0x100)OBJECT->physics_joint1=NULL;if(h[SECONDARY]==0x100)OBJECT->physics_joint2=NULL;if(h[CHILDTYPE]==0x100)for(i=1;i<5;i++)OBJ(tPhysics_object,0x100+i*0x800)->physics_joint1=NULL;if(h[CHILDSECONDARY]==0x100)for(i=1;i<5;i++)OBJ(tPhysics_object,0x100+i*0x800)->physics_joint2=NULL;
if(h[TREE])OBJECT->child=OBJ(tPhysics_object,0x900);if(h[TREE]>=2)OBJ(tPhysics_object,0x900)->next=OBJ(tPhysics_object,0x1100);if(h[TREE]>=3)OBJ(tPhysics_object,0x900)->child=OBJ(tPhysics_object,0x1900);gPhysics_scratch_ptr=(void*)h[CURSOR];gPhysics_other_buffer=(void*)h[BUFFER];gPhysics_other_buffer_capacity=(int)h[CAPACITY];gPhysics_error_cb=h[CALLBACK]?(tPhysicsError_cbfn*)selected_callback:NULL;gPhysics_empty_message=(char)(h[SEED]&0x7f);builder_poison=0xcccccccc;}
static int run(const char*path){FILE*f;unsigned count,bytes,root;SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);f=fopen(path,"rb");if(!f)return 10;heap=(unsigned char*)VirtualAlloc((void*)0x20000000,0x300000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(heap!=(void*)0x20000000)return 11;_setmode(_fileno(stdout),_O_BINARY);for(case_index=0;fread(h,sizeof(h),1,f)==1;case_index++){if(fread(heap+0x200000,0x10000,1,f)!=1)return 12;setup();_controlfp(_PC_53,_MCW_PC);root=h[ROOT_INDEX]==5?0:(unsigned)OBJ(tPhysics_object,0x100+h[ROOT_INDEX]*0x800);if(h[MODE]==0)invoke(0,(unsigned)OBJECT,root);else if(h[MODE]<4)invoke(h[MODE],h[SIZE],0);else{count=invoke(0,(unsigned)OBJECT,root);bytes=(count+12)*count*8+28;invoke(3,0,0);invoke(1,bytes,0);invoke(2,bytes,0);invoke(1,h[SIZE],0);}}fclose(f);return 0;}
'''
COMMON=re.sub(r'\bSIZE\b','SIZE_BYTES',COMMON)
COMMON=re.sub(r'\bCALLBACK\b','HAS_CALLBACK',COMMON)

def fixtures():
 cases=[]
 def add(**v):
  c=dict(mode=0,size=16,cursor=0x2020a000,buffer=0x2020a000,capacity=0x4000,callback=1,mutation=0,newcursor=0xfffffff0,newbuffer=0xfffffff0,newcapacity=32,primary=0,secondary=0,flags=0,childtype=4,childsecondary=2,childflags=0,tree=3,rootstate=0,root=4,seed=len(cases)+1);c.update(v);cases.append(c)
 for primary in (0,1,2,3,4,5,0xffffffff,0x100):
  for secondary in (0,1,2,3,4,5,0xffffffff,0x100):
   for flags in (0,0x400):
    for rootstate in (0,1,0xffffffff):add(primary=primary,secondary=secondary,flags=flags,rootstate=rootstate)
 for root in (0,1,4):
  for tree in range(4):
   for childflags in (0,0x400):
    for childtype in (0,1,2,3,4,5):add(tree=tree,root=root,childflags=childflags,childtype=childtype)
 for root in (0,4,5):
  for flags in (0,0x400):add(primary=0,secondary=0,childtype=0,tree=0,root=root,flags=flags)
 for mode in (1,2,3):
  for cursor,buffer,capacity in ((0x1000,0x1000,16),(0x1010,0x1000,16),(0x100f,0x1000,16),(0xffffffff,0xfffffff0,32),(0,0xfffffff0,32),(0x80000000,0,0x7fffffff),(0xffffffff,0,0xffffffff),(0,0,0),(0x1234,0,0)):
   for size in (0,1,15,16,17,0xffffffff,0x80000000,0x7fffffff):
    for cb in (0,1):add(mode=mode,cursor=cursor,buffer=buffer,capacity=capacity,size=size,callback=cb)
 for mode in (0,1,2):
  for mutation in range(16):add(mode=mode,primary=5,size=0x8000,mutation=mutation)
 for tree in range(4):
  for primary in (0,1,2,3,4):
   for flags in (0,0x400):add(mode=4,tree=tree,primary=primary,flags=flags)
 return cases

def make_case(c):
 rng=random.Random(c['seed']);return words([c[x] for x in FIELDS])+words([rng.getrandbits(32) for _ in range(0x10000//4)])

def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,default=ROOT/'build-audit/physics-joint-scratch-tests');p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE');p.add_argument('--symbols',type=Path);p.add_argument('--recompiled',type=Path);p.add_argument('--case-index',type=int,action='append');a=p.parse_args();assert bool(a.symbols)==bool(a.recompiled);b=a.original.read_bytes();assert sha(b)==HASH;out=a.output.resolve();out.mkdir(parents=True,exist_ok=True)
 from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM,CS_OP_REG
 cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
 def relocation(data,entries,globals_,exit_,table):
  nt,base,size,tab=pe(data);refs={};closure=[];allowed={exit_,*[e[1] for e in entries]}
  def translated(value):
   for name,(start,n) in globals_.items():
    if start<=value<start+n:return 0x20000000+GLOBALS[name][0]-0x590000+value-start,1
   assert base<=value<base+size;return value-base,0
  for name,start,n in entries:
   instructions=list(cs.disasm(raw(data,start,n),start));assert sum(i.size for i in instructions)==n,(name,n)
   for ii,i in enumerate(instructions):
    if i.mnemonic=='call' or i.mnemonic.startswith('j'):
     op=i.operands[0]
     if op.type==CS_OP_IMM:assert op.imm in allowed or (i.mnemonic.startswith('j') and start<=op.imm<start+n),(name,i.op_str)
     elif op.type==CS_OP_REG:
      assert i.mnemonic=='call' and name in ('CountJointForces','GetPhysicsScratchPointer','IncreasePhysicsScratchPointer','PhysicsError'),(name,i.op_str)
      writer=next(x for x in reversed(instructions[:ii]) if op.reg in x.regs_access()[1]);assert writer.mnemonic=='mov' and writer.operands[0].type==CS_OP_REG and writer.operands[0].reg==op.reg and writer.operands[1].type==CS_OP_MEM and writer.operands[1].mem.disp==globals_['gPhysics_error_cb'][0],(name,writer.op_str)
     else:assert i.mnemonic=='jmp' and name=='CountJointForces' and op.mem.disp==table
    for op in i.operands:
     if op.type==CS_OP_MEM and base<=op.mem.disp<base+size:
      assert i.disp_size==4;refs[i.address+i.disp_offset-base]=(op.mem.disp,*translated(op.mem.disp))
     if op.type==CS_OP_IMM and base<=op.imm<base+size and not(i.mnemonic=='call' or i.mnemonic.startswith('j')):
      assert i.imm_size==4;refs[i.address+i.imm_offset-base]=(op.imm,*translated(op.imm))
   closure.append(dict(name=name,address=start,length=n,sha256=sha(raw(data,start,n))))
  if table is not None:
   for i in range(5):
    value=struct.unpack('<I',raw(data,table+4*i,4))[0];assert entries[0][1]<=value<entries[0][1]+entries[0][2];refs[table+4*i-base]=(value,*translated(value))
  return dict(base=base,size=size,refs=refs,closure=closure,entries=[e[1]-base for e in entries[:4]],exit=exit_-base)
 def save(meta,path):path.write_bytes(words([*meta['entries'],meta['exit'],len(meta['refs'])])+b''.join(words([k,*v]) for k,v in sorted(meta['refs'].items())))
 original=relocation(b,ENTRIES,GLOBALS,0x5784e0,0x4b9730);save(original,out/'original-addresses.bin');caller,physical,template=literals()
 old=' cmp builder_mode,8\n jne notwide\n fstp qword ptr builder_wide\n xor eax,eax\n notwide:\n cmp builder_mode,1\n je zeroreturn\n cmp builder_mode,10\n jne resultready\n zeroreturn:\n xor eax,eax\n resultready:';assert old in caller;caller=caller.replace(old,' cmp builder_mode,2\n je zeroreturn\n cmp builder_mode,3\n jne resultready\n zeroreturn:\n xor eax,eax\n resultready:')
 physical=physical.replace('hdr[14]','hdr[6]').replace('n=hdr[13]','n=hdr[5]').replace('i<12;i++)addresses','i<4;i++)addresses').replace('image[hdr[12]]','image[hdr[4]]').replace('image+hdr[12]','image+hdr[4]').replace('RayBoundary','ControlledExit').replace('return run(argv[1]);','selected_callback=(void*)ErrorBoundary;return run(argv[1]);')
 source=SOURCE.read_text();body='\n'.join(extract(source,n) for n in ('PhysicsJointWorkState','PhysicsSetJointWorkState',*[e[0] for e in ENTRIES]))
 native=COMMON+'\n#define exit ControlledExit\n'+body+'\n#undef exit\n'+r'''
static volatile unsigned native_bodies[4];
static unsigned __fastcall adapter0(tPhysics_object*x,tPhysics_object*y){typedef int(C2_HOOK_FASTCALL*fn)(tPhysics_object*,tPhysics_object*);return(unsigned)((fn)(size_t)native_bodies[0])(x,y);}
static unsigned __fastcall adapter1(int x,unsigned y){typedef void*(C2_HOOK_FASTCALL*fn)(int);(void)y;return(unsigned)((fn)(size_t)native_bodies[1])(x);}
static unsigned __fastcall adapter2(int x,unsigned y){typedef void(C2_HOOK_FASTCALL*fn)(int);(void)y;((fn)(size_t)native_bodies[2])(x);return 0;}
static unsigned __fastcall adapter3(unsigned x,unsigned y){typedef void(C2_HOOK_FASTCALL*fn)(void);(void)x;(void)y;((fn)(size_t)native_bodies[3])();return 0;}
int C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;selected_callback=(void*)NativeErrorCallback;INIT;return run(argv[1]);}
'''
 native=native.replace('INIT;', ''.join(f'native_bodies[{i}]=(unsigned){e[0]};addresses[{i}]=(unsigned)adapter{i};' for i,e in enumerate(ENTRIES[:4])));registry.COMMON=COMMON;commands=compile_sources(out,native,physical,caller);all_cases=fixtures();cases=[all_cases[i] for i in a.case_index] if a.case_index else all_cases
 linked=None
 if a.symbols:
  data=a.recompiled.read_bytes();mapping=json.loads(a.symbols.read_text());assert mapping['format']==1 and mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(data);symbols={s['orig']:s['recomp'] for s in mapping['symbols']};map_bytes=a.recompiled.with_suffix('.map').read_bytes();functions={int(addr,16):(name,owner.strip()) for name,addr,owner in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',map_bytes.decode('latin1'),re.M)};exit_=symbols.get(0x5784e0)
  if exit_ is None:
   candidates=[addr for addr,(name,owner) in functions.items() if name=='_exit' and owner.startswith('LIBC:')];assert len(candidates)==1;exit_=candidates[0]
  # The marker exporter omits the one-byte BSS declaration. Resolve only that
  # canonical public symbol from the same hash-bound PE's companion map.
  if GLOBALS['gPhysics_empty_message'][0] not in symbols:
   candidates=re.findall(r'^\s*[0-9a-fA-F]{4}:[0-9a-fA-F]{8}\s+_gPhysics_empty_message\s+([0-9a-fA-F]{8})\s+<common>\s*$',map_bytes.decode('latin1'),re.M);assert len(candidates)==1;symbols[GLOBALS['gPhysics_empty_message'][0]]=int(candidates[0],16)
  initial=[symbols[e[1]] for e in ENTRIES];names={symbols[e[1]]:e[0] for e in ENTRIES};pending=list(initial);seen=set();entries=[];table=None
  while pending:
   start=pending.pop(0)
   if start in seen:continue
   seen.add(start);end=min(x for x in functions if x>start);name=names.get(start,functions[start][0]);instructions=list(cs.disasm(raw(data,start,end-start),start))
   if name=='CountJointForces':
    for ins in instructions:
     if ins.mnemonic=='jmp' and ins.operands[0].type==CS_OP_MEM:table=ins.operands[0].mem.disp;assert start<table and table+20<=end;end=table;break
   entries.append((name,start,end-start))
   for ins in cs.disasm(raw(data,start,end-start),start):
    if not(ins.mnemonic=='call' or ins.mnemonic.startswith('j')):continue
    op=ins.operands[0]
    if op.type!=CS_OP_IMM:continue
    if start<=op.imm<end or op.imm==exit_:continue
    assert op.imm in functions,(name,ins.op_str)
    if op.imm not in initial:assert functions[op.imm] in (('_PhysicsJointWorkState','49-physics.c.obj'),('_PhysicsSetJointWorkState','49-physics.c.obj')),(name,functions[op.imm])
    pending.append(op.imm)
  by_address={e[1]:e for e in entries};entries=[by_address[x] for x in initial]+[e for e in entries if e[1] not in initial];linked=relocation(data,entries,{n:(symbols[old],size) for n,(old,size) in GLOBALS.items()},exit_,table);linked.update(recompiled_sha256=sha(data),symbols_sha256=sha(a.symbols.read_bytes()),map_sha256=sha(map_bytes));save(linked,out/'linked-addresses.bin')
 results=[];failures=[]
 def execute(name,command,path):
  result=subprocess.run(command,capture_output=True,timeout=30);assert not result.returncode,(name,result.returncode,result.stderr);path.write_bytes(result.stdout);assert len(result.stdout)%RECORD_BYTES==0;return result.stdout
 for ci,c in enumerate(cases):
  case_path=out/'case.bin';case_path.write_bytes(make_case(c));expected=execute('original',[str(out/'physical.exe'),str(case_path),str(a.original.resolve()),str(out/'original-addresses.bin')],out/'original.bin')
  commands_run={n:[str(out/f'{n}.exe'),str(case_path)] for n in ('msvc5','msvc','mingw')}
  if linked:commands_run['physical-matching']=[str(out/'physical.exe'),str(case_path),str(a.recompiled.resolve()),str(out/'linked-addresses.bin')]
  for compiler,command in commands_run.items():
   actual=execute(compiler,command,out/f'{compiler}.bin')
   if actual!=expected:
    first=next((i for i,(x,y) in enumerate(zip(actual,expected)) if x!=y),-1);failure=dict(case_index=ci,case=c,compiler=compiler,first_byte=first,original=expected[max(0,first-8):first+24].hex(),actual=actual[max(0,first-8):first+24].hex());failures.append(failure);(out/'failure.json').write_text(json.dumps(failure,indent=2));raise AssertionError(failure)
   results.append(dict(case=ci,compiler=compiler,records=len(actual)//RECORD_BYTES,output_sha256=sha(actual)))
  if (ci+1)%100==0:print(f'Verified{ci+1}/{len(cases)} joint/scratch cases',flush=True)
 files=[SOURCE,SOURCE.with_suffix('.h'),ROOT/'src/carma2/include/rec2_types.h',template,Path(registry.__file__),Path(__file__).resolve()]
 manifest=dict(scenes=len(cases),comparisons=len(results),entries=ENTRIES[:4],original=original,linked=linked,original_sha256=sha(b),fixture_input_sha256=sha(b''.join(make_case(c) for c in cases)),cases=cases,results=results,failures=failures,commands=commands,selected_case_indices=a.case_index,sources={str(x.relative_to(ROOT)):sha(x.read_bytes()) for x in files},generated_files_sha256={n:sha((out/n).read_bytes()) for n in ('caller.c','caller.obj','native.c','physical.c','physical.exe','msvc5.exe','msvc.exe','mingw.exe')},limits=__doc__,extent_notes=['GetPointer58/Increase70 contiguous bytes include unreachable ADD ESP3 after CRT exit; Ghidra body sums55/67 omit those gaps'])
 (out/'verification.json').write_text(json.dumps(manifest,indent=2)+'\n');print(f'PASS {len(cases)} scenes/{len(results)} comparisons')

if __name__=='__main__':main()
