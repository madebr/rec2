"""Complete282-byte UpdateCollisionObject physical-original/native3 proof.
Raw65536-byte public arena, FASTCALL/void ESP/nonvolatile/x87TOP on normal
return; exact actual access-fault states. PC53/masked only, no normalization,
no math/control substitutes, no exhaustive/whole-game/private-frame alias claim.
Optional final PE executes its actual complete linked root/helper graph.
"""
import argparse,hashlib,json,random,re,struct,subprocess,sys
from pathlib import Path
sys.path.insert(0,str(Path.cwd()/'scripts'))
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM
from verify_opponent_route_reconstruction import ROOT,HASH,raw,sha,words,pe
from verify_tinted_native_reconstruction import extract
import verify_phil_registry_reconstruction as registry
BASE=0x22000000;ARENA=65536;START=0x4c60a0;END=0x4c61ba
COMMON=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "49-physics.h"
static unsigned char*arena;

extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[6],builder_stack_count,builder_result,builder_poison,builder_cdecl;
extern unsigned __cdecl BuilderInvoke(void);
typedef char update_layout[sizeof(void*)==4&&offsetof(tPhysics_object,shape)==0x20&&offsetof(tPhysics_object,bb1)==0x24&&offsetof(tPhysics_object,bb2)==0x3c&&offsetof(tPhysics_object,radius_squared)==0x13c&&offsetof(tPhysics_object,radius)==0x140&&offsetof(tCollision_shape_common,next)==0x34?1:-1];
static void snapshot(unsigned event,unsigned value,unsigned address){unsigned h[3];h[0]=event;h[1]=value;h[2]=address;fwrite(h,12,1,stdout);fwrite(arena,65536,1,stdout);fflush(stdout);}
static LONG WINAPI observe_fault(EXCEPTION_POINTERS*e){if(e->ExceptionRecord->ExceptionCode!=EXCEPTION_ACCESS_VIOLATION)return EXCEPTION_CONTINUE_SEARCH;snapshot(3,e->ExceptionRecord->ExceptionCode,(unsigned)e->ExceptionRecord->ExceptionInformation[1]);ExitProcess(0);return EXCEPTION_CONTINUE_SEARCH;}
static int run(const char*path){FILE*f;unsigned h[8];typedef void*(WINAPI *add_handler)(ULONG,LONG(WINAPI*)(EXCEPTION_POINTERS*));FARPROC proc;add_handler handler;
SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);f=fopen(path,"rb");if(!f)return 10;arena=VirtualAlloc((void*)0x22000000,65536,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(arena!=(void*)0x22000000)return 11;
if(fread(h,sizeof(h),1,f)!=1||fread(arena,65536,1,f)!=1)return 12;fclose(f);_setmode(_fileno(stdout),_O_BINARY);
proc=GetProcAddress(GetModuleHandleA("kernel32.dll"),"AddVectoredExceptionHandler");memcpy(&handler,&proc,4);if(!handler||!handler(1,observe_fault))return 13;
builder_mode=0;builder_x=h[0];builder_y=h[1];builder_poison=h[7];
#if defined(REC2_MATCHING)
builder_cdecl=0;builder_stack_count=0;
#else
builder_cdecl=1;builder_stack_count=1;builder_stack[0]=builder_x;
#endif
_controlfp(_PC_53,_MCW_PC);_clearfp();if(BuilderInvoke())return 14;snapshot(0,0,0);return 0;}
'''
PHYSICAL=r'''
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned h[3],i,*patch;unsigned char*image;unsigned value;
if(argc!=3)return 1;f=fopen(argv[2],"rb");if(!f)return 2;if(fread(h,sizeof(h),1,f)!=1||h[1]>=h[0])return 3;
image=VirtualAlloc(NULL,h[0],MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image||fread(image,h[0],1,f)!=1)return 4;
patch=NULL;if(h[2]){patch=malloc(h[2]*12);if(!patch||fread(patch,h[2]*12,1,f)!=1)return 5;}fclose(f);
for(i=0;i<h[2];i++){if(patch[3*i]+4>h[0])return 6;value=patch[3*i+2]?patch[3*i+1]:(unsigned)image+patch[3*i+1];memcpy(image+patch[3*i],&value,4);}free(patch);
FlushInstructionCache(GetCurrentProcess(),image,h[0]);builder_address=(unsigned)image+h[1];return run(argv[1]);}
'''

def original_image(binary):
 cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;body=raw(binary,START,END-START);instructions=list(cs.disasm(body,START))
 assert sum(i.size for i in instructions)==282
 for i in instructions:
  assert i.mnemonic!='call'
  if i.mnemonic.startswith('j'):assert i.operands[0].type==CS_OP_IMM and START<=i.operands[0].imm<END
  if i.mnemonic=='ret':assert not i.operands
  for op in i.operands:
   if op.type==CS_OP_MEM:assert op.mem.base or op.mem.index
 return words([len(body),0,0])+body,dict(address=hex(START),end=hex(END),bytes=len(body),body_sha256=sha(body),instructions=[[i.address,i.bytes.hex()]for i in instructions],patches=[],original_calls=[])

def production_capsule(source):
 if source.resolve()!=(ROOT/'src/carma2/newcommon/49-physics.c').resolve():return source.read_text()
 text=source.read_text()
 return '\n'.join(extract(text,name)for name in ['CollisionBoundsReadWord','CollisionBoundsCopy','CollisionBoundsSquared','UpdateCollisionObject'])+'\n'

def linked_image(exe,symbols_path,physics_text):
 """Transport complete actual control graph; reject unknown calls/data."""
 b=exe.read_bytes();mapping=json.loads(symbols_path.read_text());assert mapping['format']==1 and mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(b)
 mapped={r['orig']:r['recomp']for r in mapping['symbols']};start=mapped[START]
 map_bytes=exe.with_suffix('.map').read_bytes();map_text=map_bytes.decode('latin1')
 functions={int(a,16):(n,o.strip())for n,a,o in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',map_text,re.M)}
 assert start in functions and functions[start][1]=='49-physics.c.obj'
 nt,base,size,tab=pe(b);regions=[]
 for k in range(struct.unpack_from('<H',b,nt+6)[0]):
  vs,rva,n,off=struct.unpack_from('<IIII',b,tab+40*k+8);flags=struct.unpack_from('<I',b,tab+40*k+36)[0];regions.append((base+rva,max(vs,n),flags))
 cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;todo=[start];seen=set();instructions={};graph=[];edges=[];constants={};patches={}
 allowed={'UpdateCollisionObject','CollisionBoundsReadWord','CollisionBoundsCopy','CollisionBoundsSquared','PhysicsWallNaN','PhysicsScalarNaN','PhysicsLessOrUnordered','PhysicsGreaterOrdered','PhysicsScalarStore','FrictionRead','PhysicsJointScalar','PartnerMultiplyRawScalar','PhysicsJointSum'}
 while todo:
  root=todo.pop()
  if root in seen:continue
  seen.add(root);name,owner=functions[root];plain=name.lstrip('_@').split('@')[0]
  assert plain in allowed and owner=='49-physics.c.obj',(name,owner)
  end=min(a for a in functions if a>root);pending=[root];local={}
  while pending:
   at=pending.pop()
   while at not in local:
    assert root<=at<end,(name,hex(at));i=next(cs.disasm(raw(b,at,15),at));assert i.address+i.size<=end
    local[at]=bytes(i.bytes);instructions[at]=bytes(i.bytes)
    if i.mnemonic=='call' or i.mnemonic.startswith('j'):
     assert i.operands[0].type==CS_OP_IMM,('No indirect control boundary exists',name,hex(at),i.op_str)
     target=i.operands[0].imm
     if root<=target<end:
      assert i.mnemonic!='call',('Unexpected local call',name,hex(at));pending.append(target)
     else:
      assert target in functions,(name,i.mnemonic,i.op_str);todo.append(target);edges.append(dict(site=at,target=target,kind=i.mnemonic))
    for op in i.operands:
     value=None;site=None;width=4
     if op.type==CS_OP_MEM and base<=op.mem.disp<base+size:
      value=op.mem.disp;site=at+i.disp_offset;width=op.size;assert i.disp_size==4
      assert not op.mem.base and not op.mem.index,('No indexed static table exists',name,i.op_str)
     elif op.type==CS_OP_IMM and i.mnemonic in('mov','push')and base<=op.imm<base+size:
      value=op.imm;site=at+i.imm_offset;assert i.imm_size==4
     if value is not None:
      assert struct.unpack('<I',raw(b,site,4))[0]==value
      region=next(r for r in regions if r[0]<=value<r[0]+r[1]);assert not region[2]&0xa0000000,(name,hex(value),i.op_str)
      assert width in(1,2,4,8,10);constant=raw(b,value,width);constants[(value,width)]=constant;target=value;absolute=0
      patch=(site,target,absolute);assert site not in patches or patches[site]==patch;patches[site]=patch
    if i.mnemonic.startswith('ret'):
     assert (i.operands[0].imm if i.operands else 0)==(8 if plain=='PhysicsWallNaN'else 0),(name,i.op_str)
     break
    if i.mnemonic in('jmp','int','int3'):break
    at+=i.size
  graph.append(dict(name=name,owner=owner,address=root,end=end,extent=end-root,instructions=[[a,v.hex()]for a,v in sorted(local.items())],instruction_count=len(local),instruction_sha256=sha(b''.join(v for _,v in sorted(local.items())))))
 # Authenticated absolute relocation completeness in every reachable byte.
 active={a+k for a,v in instructions.items()for k in range(len(v))};rr,rz=struct.unpack_from('<II',b,nt+24+96+5*8);cursor=base+rr;checked=[]
 while cursor<base+rr+rz:
  page,n=struct.unpack('<II',raw(b,cursor,8));assert n>=8
  for w in struct.unpack('<'+'H'*((n-8)//2),raw(b,cursor+8,n-8)):
   if w>>12==3 and base+page+(w&4095)in active:
    site=base+page+(w&4095);assert site in patches,hex(site);checked.append(site)
  cursor+=n
 low=min([*instructions,*[a for a,n in constants]]);high=max([a+len(v)for a,v in instructions.items()]+[a+len(v)for (a,n),v in constants.items()]);image=bytearray(b'\xcc'*(high-low))
 for a,v in instructions.items():image[a-low:a-low+len(v)]=v
 for (a,n),v in constants.items():
  for k in range(n):assert a+k not in active,'constant/code overlap'
  image[a-low:a-low+n]=v
 transported=[(site-low,target if absolute else target-low,absolute)for site,target,absolute in sorted(patches.values())]
 blob=words([len(image),start-low,len(transported)])+image+b''.join(words(r)for r in transported)
 meta=dict(recompiled_sha256=sha(b),symbols_sha256=sha(symbols_path.read_bytes()),map_sha256=sha(map_bytes),start=start,low=low,high=high,graph=graph,edges=edges,global_fallbacks=[],mutable_globals={},constants=[dict(address=a,bytes=n,hex=v.hex())for (a,n),v in sorted(constants.items())],patches=[dict(site=s,old=struct.unpack('<I',raw(b,s,4))[0],target=t,absolute=x)for s,t,x in sorted(patches.values())],transport_patches=transported,active_relocations_checked=checked,indirect_control_sites=[],normalization=[])
 return blob,meta

def fixtures():
 result=[]
 bits=lambda f:struct.unpack('<I',struct.pack('<f',f))[0]
 special=[0,0x80000000,1,0x80000001,0x00800000,0x80800000,0x3f800000,0xbf800000,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7f800001,0xff800001,0x7fc12345,0xffc54321]
 def add(shapes=(0x2000,),bounds=None,patches=(),label='normal',root_pointer=None,seed=None,head=None):
  seed=len(result)+1597 if seed is None else seed;r=random.Random(seed);arena=bytearray(r.randbytes(ARENA))
  def put(at,value):struct.pack_into('<I',arena,at,value&0xffffffff)
  for j,shape in enumerate(shapes):
   vals=bounds[j]if bounds is not None else [bits(r.uniform(-8,8))for _ in range(6)]
   put(shape,j%5)
   for k,v in enumerate(vals):put(shape+4+4*k,v)
   put(shape+0x34,BASE+shapes[j+1]if j+1<len(shapes)else 0)
  put(0x1020,head if head is not None else BASE+shapes[0]if shapes else 0)
  for at,value in patches:put(at,value)
  root_pointer=BASE+0x1000 if root_pointer is None else root_pointer
  h=words([root_pointer,r.choice([0,1,0xdeadbeef,BASE+0x1000]),0,0,0,0,0,r.choice([0,0xcccccccc,0x7f800001])])
  result.append((dict(label=label,shapes=shapes,bounds=bounds,patches=patches,root_pointer=root_pointer,head=head,seed=seed),h+arena))
 add(shapes=(),label='no-shape')
 add(root_pointer=0,label='null-object-fault')
 add(head=1,label='invalid-head-fault')
 add(patches=((0x2034,1),),label='invalid-next-fault')
 for count in [1,2,3,8]:
  for i in range(24):add(shapes=tuple(0x2000+j*0x100 for j in range(count)),label='finite-chain')
 # The complete asymmetric squared-bound calculation, including overflow
 # of only the maximum square's scalar32 spill and directed paired NaNs.
 for axis in range(3):
  for low in special:
   for high in special:
    vals=[bits(-2),bits(-3),bits(-4),bits(2),bits(3),bits(4)];vals[axis]=low;vals[3+axis]=high
    add(bounds=[vals],label='squared-paired-special')
 # Every min and max branch has ordered/equal/unordered pairs; raw initial
 # copying differs from the FLD/FSTP32 store that every merge executes.
 for axis in range(3):
  for left in special:
   for right in special:
    a=[bits(-1)]*3+[bits(1)]*3;b=a.copy();a[axis]=left;a[axis+3]=right;b[axis]=right;b[axis+3]=left
    add(shapes=(0x2000,0x2100),bounds=[a,b],label='merge-paired-special')
 for i in range(120):
  r=random.Random(6700+i);vals=[r.choice(special)for _ in range(6)];add(bounds=[vals],label='three-axis-payload-order')
 for d in range(-16,49):
  add(shapes=(0x1020+d,),label='object-shape-forward-overlap')
 for d in range(-7,8):
  add(shapes=(0x2000,0x1020+d),label='merged-object-shape-overlap')
 # Captured head and live next reads: merging can replace shape.next with
 # another mapped shape pointer; no next pointer is pre-cached by the C.
 for d in [0,4,8]:
  add(shapes=(0x2000,0x1000+d),patches=((0x2004,BASE+0x3000),(0x3034,0)),label='live-next-redirect')
 for i in range(120):
  r=random.Random(17000+i);vals=[bits(r.uniform(-1,1)*2.**r.randrange(-120,120))for _ in range(6)]
  add(bounds=[vals],label='finite-exponents')
 return result
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',required=True,type=Path);p.add_argument('--candidate-source',type=Path,default=ROOT/'src/carma2/newcommon/49-physics.c');p.add_argument('--limit',type=int);p.add_argument('--case-index',type=int,action='append');p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);a=p.parse_args();assert bool(a.recompiled)==bool(a.symbols)
 out=a.output.resolve();assert not out.exists(),'Use a new uniquely owned output directory';out.mkdir(parents=True)
 source=a.candidate_source.resolve();script=Path(__file__).resolve();paths=[source,script,ROOT/'scripts/verify_phil_registry_reconstruction.py',ROOT/'scripts/verify_opponent_route_reconstruction.py',ROOT/'scripts/verify_opponent_navigation_reconstruction.py']+list((ROOT/'src').rglob('*.h'))
 paths.extend([ROOT/'src/carma2/newcommon/49-physics.c',ROOT/'scripts/verify_tinted_native_reconstruction.py'])
 if a.recompiled:paths.extend([a.recompiled,a.symbols,a.recompiled.with_suffix('.map')])
 before={str(f):sha(f.read_bytes())for f in paths};(out/'source-before.json').write_text(json.dumps(before,indent=2)+'\n')
 (out/'candidate-snapshot.c').write_bytes(source.read_bytes());(out/'verifier-snapshot.py').write_bytes(script.read_bytes())
 original_path=ROOT/'build-reference/originals/CARMA2_HW.EXE';binary=original_path.read_bytes();assert sha(binary)==HASH
 image,graph=original_image(binary);(out/'original-code.bin').write_bytes(image)
 linked=None
 if a.recompiled:
  linked_blob,linked=linked_image(a.recompiled,a.symbols,(ROOT/'src/carma2/newcommon/49-physics.c').read_text());(out/'matching-code.bin').write_bytes(linked_blob)
 native=COMMON+'\n'+'\n'.join(extract((ROOT/'src/carma2/newcommon/49-physics.c').read_text(),name)for name in ['PhysicsWallNaN','PhysicsScalarNaN','PhysicsLessOrUnordered','PhysicsGreaterOrdered','PhysicsScalarStore','FrictionRead','PhysicsJointScalar','PartnerMultiplyRawScalar','PhysicsJointSum'])+'\n'+production_capsule(source)+'\nint C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;builder_address=(unsigned)UpdateCollisionObject;return run(argv[1]);}\n'
 caller,_,_=registry.literals();caller=caller.replace('builder_stack[5]','builder_stack[6]').replace('builder_poison;','builder_poison,builder_cdecl;')
 old=' mov builder_result,eax\n mov ecx,esp';assert old in caller;caller=caller.replace(old,' mov builder_result,eax\n cmp builder_cdecl,0\n je update_stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n update_stackready:\n mov ecx,esp')
 registry.COMMON=COMMON;commands=registry.compile_sources(out,native,PHYSICAL,caller)
 cases=fixtures();indices=a.case_index or list(range(len(cases)));indices=indices[:a.limit]if a.limit else indices
 results=[];failures=[];classification={};fixture_hashes=[]
 for index in indices:
  case,payload=cases[index];fixture_hashes.append(sha(payload));f=out/'case.bin';f.write_bytes(payload)
  run=subprocess.run([str(out/'physical.exe'),str(f),str(out/'original-code.bin')],capture_output=True,timeout=30)
  assert run.returncode==0 and len(run.stdout)==ARENA+12,('original',index,run.returncode,len(run.stdout),run.stderr)
  expected=run.stdout;event,value,address=struct.unpack_from('<3I',expected);classification[index]=dict(event=event,value=value,address=address)
  for compiler in('msvc5','msvc','mingw',*(['matching']if linked else [])):
   command=[str(out/('physical.exe'if compiler=='matching'else compiler+'.exe')),str(f)]
   if compiler=='matching':command.append(str(out/'matching-code.bin'))
   proc=subprocess.run(command,capture_output=True,timeout=30);passed=proc.returncode==0 and proc.stdout==expected
   row=dict(index=index,compiler=compiler,passed=passed,input_sha256=sha(payload),original_output_sha256=sha(expected),actual_output_sha256=sha(proc.stdout),original_output_bytes=len(expected),actual_output_bytes=len(proc.stdout),event=event,normal_return_abi_checked=event==0,exit=proc.returncode);results.append(row)
   if not passed:
    failure=out/('failure-%s-%d.json'%(compiler,index));failure.write_text(json.dumps(dict(case=case,expected=expected.hex(),actual=proc.stdout.hex(),stderr=proc.stderr.hex()),indent=2)+'\n');failures.append(dict(index=index,compiler=compiler,case=case,path=str(failure)))
  if index%100==0:print('checked',index,len(results),'failures',len(failures),flush=True)
 after={f:sha(Path(f).read_bytes())for f in before};assert before==after,'source guard changed'
 report=dict(passed=not failures,original_sha256=HASH,graph=graph,scenes=len(indices),comparisons=len(results),complete_suite=indices==list(range(len(cases))),selected_indices=indices,total_scenes=len(cases),source_stable=True,source_before=before,source_after=after,fixtures_sha256=sha(b''.join(cases[i][1]for i in indices)),fixture_sha256=fixture_hashes,results=results,failures=failures,classification=classification,commands=commands,generated_sha256={str(f):sha(f.read_bytes())for f in out.iterdir()if f.is_file()},limitations=__doc__,live_handles=[])
 if linked:report.update(linked=linked,recompiled_sha256=linked['recompiled_sha256'],symbols_sha256=linked['symbols_sha256'])
 (out/'verification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS'if not failures else'FAIL',len(indices),len(results),len(failures),flush=True)
 assert not failures,'Full failure evidence preserved'
if __name__=='__main__':main()
