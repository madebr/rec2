"""Complete reset-list/matrix propagation original/native3 physical comparison.

Seven authentic original bodies, no numeric/control substitutes. Full64KiB
arenas and real fault addresses remain raw; direct FASTCALL/CDECL transport,
ESP/nonvolatiles/x87TOP on normal returns, void EAX deliberately excluded.
"""
import argparse,hashlib,json,random,re,struct,subprocess,sys
from pathlib import Path
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'src/carma2').is_dir());sys.path.insert(0,str(ROOT/'scripts'))
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM
from verify_opponent_route_reconstruction import raw,HASH,sha,words,pe
from verify_tinted_native_reconstruction import extract
import verify_phil_registry_reconstruction as registry
BASE=0x22000000;ARENA=65536
ENTRIES=[('ResetObjectList',0x4c2600,104),('FUN_004c62e0',0x4c62e0,195),('FUN_004c25a0',0x4c25a0,82),('BrMatrix34Mul',0x532620,404),('BrMatrix34LPInverse',0x532eb0,175),('BrMatrix34LPNormalise',0x532f60,411),('BrMatrix34Copy',0x5325d0,79)]
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
#include "c2_math.h"
#include "brender/br_defs.h"
#define A(x,y) A->m[x][y]
#define B(x,y) B->m[x][y]
#define C(x,y) C->m[x][y]
static unsigned char*arena;static unsigned addresses[2];
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison,builder_cdecl;
extern unsigned __cdecl BuilderInvoke(void);
typedef char listmatrix_layout[sizeof(void*)==4&&sizeof(tPhysics_object)==1240&&offsetof(tPhysics_object,actor)==0&&offsetof(tPhysics_object,field_0xf0)==0xf0&&offsetof(tPhysics_object,child)==0x224&&offsetof(tPhysics_object,next)==0x220&&offsetof(br_actor,t)==0x28&&offsetof(br_transform,t)==4&&sizeof(br_matrix34)==48?1:-1];
static void snapshot(unsigned event,unsigned value,unsigned address){unsigned h[3];h[0]=event;h[1]=value;h[2]=address;fwrite(h,12,1,stdout);fwrite(arena,65536,1,stdout);fflush(stdout);}
static LONG WINAPI observe_fault(EXCEPTION_POINTERS*e){if(e->ExceptionRecord->ExceptionCode!=EXCEPTION_ACCESS_VIOLATION)return EXCEPTION_CONTINUE_SEARCH;snapshot(3,e->ExceptionRecord->ExceptionCode,(unsigned)e->ExceptionRecord->ExceptionInformation[1]);ExitProcess(0);return EXCEPTION_CONTINUE_SEARCH;}
static int run(const char*path){FILE*f;unsigned h[4];typedef void*(WINAPI *add_handler)(ULONG,LONG(WINAPI*)(EXCEPTION_POINTERS*));FARPROC proc;add_handler handler;
SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);f=fopen(path,"rb");if(!f)return 10;arena=VirtualAlloc((void*)0x22000000,65536,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(arena!=(void*)0x22000000)return 11;
if(fread(h,sizeof(h),1,f)!=1||fread(arena,65536,1,f)!=1)return 12;fclose(f);_setmode(_fileno(stdout),_O_BINARY);
proc=GetProcAddress(GetModuleHandleA("kernel32.dll"),"AddVectoredExceptionHandler");memcpy(&handler,&proc,4);if(!handler||!handler(1,observe_fault))return 13;if(h[0]>1)return 15;
builder_address=addresses[h[0]];builder_mode=0;builder_x=h[1]?0x22000000+h[1]:0;builder_y=h[2]?0x22000000+h[2]:0;builder_poison=h[3];
#if defined(REC2_MATCHING)
builder_cdecl=0;builder_stack_count=0;
#else
builder_cdecl=1;builder_stack_count=h[0]?2:1;builder_stack[0]=builder_x;builder_stack[1]=builder_y;
#endif
_controlfp(_PC_53,_MCW_PC);_clearfp();if(BuilderInvoke())return 14;snapshot(0,0,0);return 0;}
'''
PHYSICAL=r'''
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned h[5],i,offset,value;unsigned char*image;
if(argc!=3)return 1;f=fopen(argv[2],"rb");if(!f)return 2;if(fread(h,sizeof(h),1,f)!=1||h[1]>=h[0]||h[2]>=h[0])return 3;
image=VirtualAlloc(NULL,h[0],MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image||fread(image,h[0],1,f)!=1)return 4;
for(i=0;i<h[3];i++){if(fread(&offset,4,1,f)!=1||offset>h[0]-4)return 5;memcpy(&value,image+offset,4);value=(unsigned)image+value-h[4];memcpy(image+offset,&value,4);}fclose(f);
FlushInstructionCache(GetCurrentProcess(),image,h[0]);addresses[0]=(unsigned)image+h[1];addresses[1]=(unsigned)image+h[2];return run(argv[1]);}
'''
def original_image(binary):
 cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;roots={a:(name,n)for name,a,n in ENTRIES};refs=[];constants={};bodies=[]
 for name,a,n in ENTRIES:
  code=raw(binary,a,n);instructions=list(cs.disasm(code,a));assert sum(i.size for i in instructions)==n;calls=[]
  for i in instructions:
   if i.mnemonic=='call':assert i.operands[0].type==CS_OP_IMM and i.operands[0].imm in roots;calls.append(dict(site=i.address,target=i.operands[0].imm))
   if i.mnemonic.startswith('j'):assert i.operands[0].type==CS_OP_IMM and a<=i.operands[0].imm<a+n
   if i.mnemonic=='ret':assert not i.operands
   for op in i.operands:
    if op.type==CS_OP_MEM and not op.mem.base and not op.mem.index:
     assert op.mem.disp in(0x58b8c0,0x58b8c8) and op.size in(4,8) and i.disp_size==4
     refs.append(i.address+i.disp_offset);constants[(op.mem.disp,op.size)]=raw(binary,op.mem.disp,op.size)
  bodies.append(dict(name=name,address=a,bytes=n,sha256=sha(code),instructions=[[i.address,i.bytes.hex()]for i in instructions],calls=calls))
 low=min(roots);high=max([a+n for name,a,n in ENTRIES]+[a+len(v)for (a,n),v in constants.items()]);image=bytearray(b'\xcc'*(high-low))
 for name,a,n in ENTRIES:image[a-low:a-low+n]=raw(binary,a,n)
 for (a,n),v in constants.items():image[a-low:a-low+n]=v
 blob=words([len(image),0x4c2600-low,0x4c62e0-low,len(refs),low])+image+words([a-low for a in refs])
 return blob,dict(bodies=bodies,total_original_bytes=1450,absolute_relocations=refs,constants=[dict(address=a,bytes=n,hex=v.hex())for (a,n),v in constants.items()],low=low,high=high)

def production_capsule(source):
 if source.resolve()!=(ROOT/'src/carma2/newcommon/49-physics.c').resolve():return source.read_text()
 text=source.read_text()
 return '\n'.join(extract(text,name)for name in ['ListMatrixPointer','ListMatrixActor','ResetObjectList','FUN_004c62e0'])+'\n'

def matching_image(exe,symbols_path):
 """Full actual root/helper CFG; no control/math substitution or data mocks."""
 b=exe.read_bytes();mapping=json.loads(symbols_path.read_text());assert mapping['format']==1 and mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(b)
 mapped={r['orig']:r['recomp']for r in mapping['symbols']};roots=[mapped[0x4c2600],mapped[0x4c62e0]]
 map_bytes=exe.with_suffix('.map').read_bytes();functions={}
 for name,address,owner in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',map_bytes.decode('latin1'),re.M):functions.setdefault(int(address,16),[]).append((name,owner.strip()))
 public={mapped[a]:name for name,a,n in ENTRIES};assert set(public)<=set(functions)
 nt,base,size,tab=pe(b);regions=[]
 for k in range(struct.unpack_from('<H',b,nt+6)[0]):
  vs,rva,n,off=struct.unpack_from('<IIII',b,tab+k*40+8);flags=struct.unpack_from('<I',b,tab+k*40+36)[0];regions.append((base+rva,max(vs,n),flags))
 cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;todo=list(roots);seen=set();instructions={};constants={};refs=set();graph=[];edges=[]
 helpers={'ListMatrixPointer','ListMatrixActor','LPRead','LPWrite','LPScalar','LPNaN','LPCopyWord'};owners={'49-physics.c.obj','matrix34.c.obj','libbrender:matrix34.c.obj'}
 while todo:
  start=todo.pop()
  if start in seen:continue
  seen.add(start);aliases=functions[start]
  if start in public:name=public[start];assert any(owner in owners for n,owner in aliases)
  else:
   accepted=[(name,owner)for name,owner in aliases if name.lstrip('_@').split('@')[0]in helpers and owner in owners];assert len(accepted)==1,(hex(start),aliases);name=accepted[0][0]
  end=min(v for v in functions if v>start);pending=[start];local={}
  while pending:
   at=pending.pop()
   while at not in local:
    assert start<=at<end,(name,hex(at));i=next(cs.disasm(raw(b,at,15),at));assert at+i.size<=end;local[at]=bytes(i.bytes);instructions[at]=bytes(i.bytes)
    if i.mnemonic=='call' or i.mnemonic.startswith('j'):
     assert i.operands[0].type==CS_OP_IMM,('Unexpected indirect control',name,hex(at),i.op_str)
     target=i.operands[0].imm
     if start<=target<end:
      if i.mnemonic=='call':assert target==start and start in {mapped[0x4c62e0],mapped[0x4c25a0]},(name,i.op_str)
      else:pending.append(target)
     else:assert target in functions,(name,i.op_str);todo.append(target);edges.append(dict(site=at,target=target,kind=i.mnemonic))
    for op in i.operands:
     if op.type==CS_OP_MEM and base<=op.mem.disp<base+size:
      assert not op.mem.base and not op.mem.index and i.mnemonic.startswith('f')and op.size in(4,8),('Unexpected nonconstant static data',name,i.op_str)
      assert i.disp_size==4;region=next(r for r in regions if r[0]<=op.mem.disp<r[0]+r[1]);assert not region[2]&0xa0000000
      constants[(op.mem.disp,op.size)]=raw(b,op.mem.disp,op.size);refs.add(at+i.disp_offset)
     elif op.type==CS_OP_IMM and i.mnemonic in('mov','push')and base<=op.imm<base+size:raise AssertionError(('Unexpected static pointer argument',name,i.op_str))
    if i.mnemonic.startswith('ret'):assert not i.operands,(name,i.op_str);break
    if i.mnemonic in('jmp','int3','int'):break
    at+=i.size
  graph.append(dict(name=name,address=start,end=end,extent=end-start,map_aliases=aliases,instructions=[[a,v.hex()]for a,v in sorted(local.items())],instruction_count=len(local),instruction_sha256=sha(b''.join(v for a,v in sorted(local.items())))))
 active={a+k for a,v in instructions.items()for k in range(len(v))};rr,rz=struct.unpack_from('<II',b,nt+24+96+5*8);cursor=base+rr;relocations=[]
 while cursor<base+rr+rz:
  page,n=struct.unpack('<II',raw(b,cursor,8));assert n>=8
  for w in struct.unpack('<'+'H'*((n-8)//2),raw(b,cursor+8,n-8)):
   if w>>12==3 and base+page+(w&4095)in active:
    site=base+page+(w&4095);assert site in refs,hex(site);relocations.append(site)
  cursor+=n
 low=min([*instructions,*[a for a,n in constants]]);high=max([a+len(v)for a,v in instructions.items()]+[a+n for a,n in constants]);image=bytearray(b'\xcc'*(high-low))
 for a,v in instructions.items():image[a-low:a-low+len(v)]=v
 for (a,n),v in constants.items():
  assert all(a+k not in active for k in range(n));image[a-low:a-low+n]=v
 refs=sorted(refs);blob=words([len(image),roots[0]-low,roots[1]-low,len(refs),low])+image+words([a-low for a in refs])
 meta=dict(recompiled_sha256=sha(b),symbols_sha256=sha(symbols_path.read_bytes()),map_sha256=sha(map_bytes),graph=graph,edges=edges,roots=roots,low=low,high=high,absolute_relocations=refs,active_relocations_checked=relocations,constants=[dict(address=a,bytes=n,hex=v.hex())for (a,n),v in sorted(constants.items())],unexpected_control_sites=[],mutable_globals=[],normalization=[])
 return blob,meta

def fixtures():
 cases=[];objects=[0x100,0x1000,0x2000,0x3000];actors=[0x8000,0x8200,0x8400,0x8600]
 identity=[0x3f800000,0,0,0,0x3f800000,0,0,0,0x3f800000,0,0,0]
 special=[0,0x80000000,1,0x80000001,0x00800000,0x3f800000,0xbf800000,0x7f7fffff,0x7f800000,0xff800000,0x7f800001,0xff800001,0x7fc12345,0xffc12345]
 def add(mode=0,tree=0,kinds=(0,1,0,1),external=0x9000,object_offset=0x100,patches=(),actor_matrix=None,label='normal',seed=None,matrix_kind=0):
  seed=len(cases)+1 if seed is None else seed;r=random.Random(seed);arena=bytearray(r.randbytes(ARENA))
  def put(at,*v):struct.pack_into('<'+'I'*len(v),arena,at,*[x&0xffffffff for x in v])
  for j,o in enumerate(objects):
   put(o,BASE+actors[j]);put(o+0xf0,kinds[j]);put(o+0x220,0,0,0)
   mat=list(identity)
   if matrix_kind==1:mat=[struct.unpack('<I',struct.pack('<f',r.uniform(-3,3)))[0]for _ in range(12)]
   if matrix_kind==2:mat=[r.choice(special)for _ in range(12)]
   put(o+0x8c,*mat);put(o+0xbc,*list(reversed(mat)));put(actors[j]+0x2c,*identity)
  put(0x9000,*identity)
  if tree==1:put(objects[0]+0x224,BASE+objects[1])
  if tree==2:put(objects[0]+0x224,BASE+objects[1]);put(objects[1]+0x220,BASE+objects[2])
  if tree==3:put(objects[0]+0x224,BASE+objects[1]);put(objects[1]+0x224,BASE+objects[2]);put(objects[1]+0x220,BASE+objects[3])
  if tree==4:put(objects[0]+0x220,BASE+objects[1]);put(objects[1]+0x220,BASE+objects[2])
  if tree==5:put(objects[0]+0x220,BASE+objects[3]);put(objects[0]+0x224,BASE+objects[1]);put(objects[1]+0x220,BASE+objects[2])
  if actor_matrix is not None:put(objects[0],BASE+actor_matrix-0x2c)
  for at,v in patches:put(at,v)
  case=dict(mode=mode,tree=tree,kinds=kinds,external=external,object_offset=object_offset,patches=patches,actor_matrix=actor_matrix,label=label,seed=seed,matrix_kind=matrix_kind)
  cases.append((case,words([mode,object_offset,external,(0,0xcccccccc,0x7f800001)[seed%3]])+arena))
 for mode in(0,1):
  for tree in range(6):
   for kinds in[(0,0,0,0),(1,1,1,1),(0,1,2,0xffffffff),(1,0,1,0)]:
    for matrix_kind in(0,1,2):
     for external in(0,0x9000):add(mode,tree,kinds,external,matrix_kind=matrix_kind)
 for mode in(0,1):
  for word in range(12):
   for bits in special:
    add(mode,tree=0,patches=((0x18c+word*4,bits),),label='object-transform-IEEE')
    add(mode,tree=1,patches=((0x802c+word*4,bits),),external=0,label='actor-IEEE-child')
    if mode==1:add(mode,tree=1,patches=((0x9000+word*4,bits),),label='external-matrix-IEEE')
 for mode in(0,1):
  for a,b in[(0x7f812345,0xff812345),(0x7fc12345,0xffc12345),(0x7f812345,0xffc12345),(0xff812345,0x7fc12345)]:
   for word in range(9):add(mode,tree=2,patches=((0x18c+word*4,a),(0x9000+word*4,b),(0x802c+word*4,b)),label='paired-IEEE')
 for mode in(0,1):
  for delta in range(-47,48):
   for root_kind in(0,1):add(mode,kinds=(root_kind,1,0,1),actor_matrix=0x18c+delta,label='partial-actor-object')
 for delta in range(-47,48):add(1,external=0x802c+delta,label='external-actor-partial')
 # Redirects use real accessible nodes/matrices; no mathematical substitution.
 add(1,actor_matrix=0x100,patches=((0x18c,BASE+0x8200),),label='fresh-actor-pointer-after-Mul')
 add(1,tree=1,actor_matrix=0x100,patches=((0x18c,BASE+0x8200),),label='fresh-actor-pointer-before-unused-Mul')
 add(1,tree=1,actor_matrix=0x300,patches=((0x1b0,BASE+0x3000),(0x1ac,0)),label='captured-child-before-actor-write')
 add(1,tree=2,patches=((0x1000,BASE+0x1200-0x2c),(0x108c+32,BASE+0x3000),(0x108c+36,0)),label='live-sibling-after-recursion')
 add(0,tree=4,kinds=(1,1,1,1),actor_matrix=0x300,patches=((0x1bc+32,BASE+0x3000),(0x1bc+36,0)),label='live-list-next-after-rawcopy')
 add(0,object_offset=0,label='reset-NULL-list');add(1,object_offset=0,label='propagation-authentic-NULL-fault')
 return cases

def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',required=True,type=Path);p.add_argument('--candidate-source',type=Path,default=ROOT/'src/carma2/newcommon/49-physics.c');p.add_argument('--limit',type=int);p.add_argument('--case-index',type=int,action='append');p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);a=p.parse_args();assert bool(a.recompiled)==bool(a.symbols)
 out=a.output.resolve();assert not out.exists(),'Use a fresh uniquely owned output';out.mkdir(parents=True)
 source=a.candidate_source.resolve();physics=ROOT/'src/carma2/newcommon/49-physics.c';matrix=ROOT/'src/brender/core/math/matrix34.c';script=Path(__file__).resolve()
 paths=[source,physics,matrix,script,ROOT/'scripts/verify_phil_registry_reconstruction.py',ROOT/'scripts/verify_opponent_route_reconstruction.py',ROOT/'scripts/verify_opponent_navigation_reconstruction.py',ROOT/'scripts/verify_tinted_native_reconstruction.py']+list((ROOT/'src').rglob('*.h'))
 if a.recompiled:paths.extend([a.recompiled,a.symbols,a.recompiled.with_suffix('.map')])
 before={str(f):sha(f.read_bytes())for f in paths};(out/'source-before.json').write_text(json.dumps(before,indent=2)+'\n');(out/'candidate-snapshot.c').write_bytes(source.read_bytes());(out/'verifier-snapshot.py').write_bytes(script.read_bytes())
 binary=(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes();assert sha(binary)==HASH;image,graph=original_image(binary);(out/'original-code.bin').write_bytes(image)
 linked=None
 if a.recompiled:
  linked_blob,linked=matching_image(a.recompiled,a.symbols);(out/'matching-code.bin').write_bytes(linked_blob)
 body=extract(physics.read_text(),'FUN_004c25a0')+'\n'
 for name in['LPRead','LPWrite','LPScalar','LPNaN','LPCopyWord','BrMatrix34Mul','BrMatrix34LPInverse','BrMatrix34LPNormalise','BrMatrix34Copy']:body+=extract(matrix.read_text(),name)+'\n'
 native=COMMON+'\n'+body+production_capsule(source)+'\nint C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;addresses[0]=(unsigned)ResetObjectList;addresses[1]=(unsigned)FUN_004c62e0;return run(argv[1]);}\n'
 caller,_,_=registry.literals();caller=caller.replace('builder_poison;','builder_poison,builder_cdecl;');old=' mov builder_result,eax\n mov ecx,esp';assert old in caller;caller=caller.replace(old,' xor eax,eax\n mov builder_result,eax\n cmp builder_cdecl,0\n je listmatrix_stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n listmatrix_stackready:\n mov ecx,esp')
 registry.COMMON=COMMON;commands=registry.compile_sources(out,native,PHYSICAL,caller)
 cases=fixtures();indices=a.case_index or list(range(len(cases)));indices=indices[:a.limit]if a.limit else indices;results=[];failures=[];classification={}
 for index in indices:
  case,payload=cases[index];f=out/'case.bin';f.write_bytes(payload);r=subprocess.run([str(out/'physical.exe'),str(f),str(out/'original-code.bin')],capture_output=True,timeout=30)
  assert r.returncode==0 and len(r.stdout)==65548,('original',index,r.returncode,len(r.stdout),r.stderr)
  expected=r.stdout;event,value,address=struct.unpack_from('<3I',expected);classification[index]=dict(event=event,value=value,address=address)
  for compiler in('msvc5','msvc','mingw',*(['matching']if linked else [])):
   command=[str(out/('physical.exe'if compiler=='matching'else compiler+'.exe')),str(f)]
   if compiler=='matching':command.append(str(out/'matching-code.bin'))
   r=subprocess.run(command,capture_output=True,timeout=30);passed=r.returncode==0 and r.stdout==expected
   results.append(dict(index=index,compiler=compiler,passed=passed,input_sha256=sha(payload),original_output_sha256=sha(expected),actual_output_sha256=sha(r.stdout),original_output_bytes=len(expected),actual_output_bytes=len(r.stdout),event=event,normal_return_abi_checked=event==0,exit=r.returncode))
   if not passed:
    failure=out/('failure-%s-%d.json'%(compiler,index));failure.write_text(json.dumps(dict(case=case,expected=expected.hex(),actual=r.stdout.hex(),stderr=r.stderr.hex()),indent=2)+'\n');failures.append(dict(index=index,compiler=compiler,case=case,path=str(failure)))
  if index%100==0:print('checked',index,len(results),'failures',len(failures),flush=True)
 after={f:sha(Path(f).read_bytes())for f in before};assert before==after,'source guard changed'
 report=dict(passed=not failures,scenes=len(indices),comparisons=len(results),total_scenes=len(cases),complete_suite=indices==list(range(len(cases))),selected_indices=indices,source_stable=True,source_before=before,source_after=after,original_sha256=HASH,graph=graph,fixtures_sha256=sha(b''.join(cases[i][1]for i in indices)),fixture_sha256=[sha(cases[i][1])for i in indices],classification=classification,results=results,failures=failures,commands=commands,generated_sha256={str(f):sha(f.read_bytes())for f in out.iterdir()if f.is_file()},live_handles=[],limits=__doc__)
 if linked:report.update(matching=linked,recompiled_sha256=linked['recompiled_sha256'],symbols_sha256=linked['symbols_sha256'])
 (out/'verification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS'if not failures else'FAIL',len(indices),len(results),len(failures),flush=True);assert not failures,'Failures retained'
if __name__=='__main__':main()
