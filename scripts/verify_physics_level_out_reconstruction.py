"""Complete LevelOut/matrix physical-original/native3 retained physical-original/native3/actual-linked proof."""
import argparse,hashlib,json,re,runpy,struct,subprocess,sys
from pathlib import Path
ROOT=next(p for p in Path(__file__).resolve().parents if (p/"src/carma2").is_dir());sys.path.insert(0,str(ROOT/'scripts'))
from verify_opponent_route_reconstruction import raw,HASH,words,sha,pe
from verify_tinted_native_reconstruction import extract
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM,CS_GRP_JUMP
import verify_phil_registry_reconstruction as registry
HARNESS='#include <windows.h>\n#include <stdio.h>\n#include <stdlib.h>\n#include <string.h>\n#include <stdint.h>\n#include <float.h>\n#include <math.h>\n#include <io.h>\n#include <fcntl.h>\n#include "49-physics.h"\n\n#define ARENA 65536\nstatic unsigned char* arena;\nstatic unsigned addresses[2];\nextern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison,builder_cdecl;\nextern unsigned __cdecl BuilderInvoke(void);\ntypedef char level_layout[sizeof(void*)==4&&sizeof(tPhysics_object)==1240&&sizeof(tPHIL_queued_header)==24&&offsetof(tPhysics_object,child)==0x224&&offsetof(tPhysics_object,next)==0x220&&offsetof(tPhysics_object,water_depth_factor)==0x254?1:-1];\nstatic void snapshot(unsigned event,unsigned value,unsigned address){unsigned h[3];h[0]=event;h[1]=value;h[2]=address;fwrite(h,12,1,stdout);fwrite(arena,ARENA,1,stdout);fflush(stdout);}\nstatic LONG WINAPI observe_fault(EXCEPTION_POINTERS* e){if(e->ExceptionRecord->ExceptionCode!=EXCEPTION_ACCESS_VIOLATION)return EXCEPTION_CONTINUE_SEARCH;snapshot(3,e->ExceptionRecord->ExceptionCode,(unsigned)e->ExceptionRecord->ExceptionInformation[1]);ExitProcess(0);return EXCEPTION_CONTINUE_SEARCH;}\nstatic int run(const char* path){FILE*f;unsigned h[8];typedef void*(WINAPI *add_handler)(ULONG,LONG(WINAPI*)(EXCEPTION_POINTERS*));FARPROC proc;add_handler handler;\nSetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);f=fopen(path,"rb");if(!f)return 10;arena=VirtualAlloc((void*)0x22000000,ARENA,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(arena!=(void*)0x22000000)return 11;\nif(fread(h,sizeof(h),1,f)!=1||fread(arena,ARENA,1,f)!=1)return 12;fclose(f);_setmode(_fileno(stdout),_O_BINARY);\nproc=GetProcAddress(GetModuleHandleA("kernel32.dll"),"AddVectoredExceptionHandler");memcpy(&handler,&proc,4);if(!handler||!handler(1,observe_fault))return 13;if(h[0]>1)return 15;\nbuilder_address=addresses[h[0]];builder_mode=0;builder_poison=h[7];\nif(h[0]==0){builder_x=h[1]?0x22000000+h[1]:0;builder_y=h[7];\n#if defined(REC2_MATCHING)\nbuilder_cdecl=0;builder_stack_count=0;\n#else\nbuilder_cdecl=1;builder_stack_count=1;builder_stack[0]=builder_x;\n#endif\n}else{builder_x=0;builder_y=0;builder_cdecl=1;builder_stack_count=3;builder_stack[0]=h[2]?0x22000000+h[2]:0;builder_stack[1]=h[3]?0x22000000+h[3]:0;builder_stack[2]=h[4]?0x22000000+h[4]:0;}\n_controlfp(_PC_53,_MCW_PC);_clearfp();if(BuilderInvoke())return 14;snapshot(0,0,0);return 0;}\n'
FIXTURE_PROGRAM="import random,struct\nBASE=0x22000000;ARENA=65536\n\ndef fixtures():\n cases=[];identity=[0x3f800000,0,0,0,0x3f800000,0,0,0,0x3f800000,0,0,0]\n special=[0,0x80000000,1,0x80000001,0x00800000,0x3f800000,0xbf800000,0x7f7fffff,0x7f800000,0xff800000,0x7f800001,0xff800001,0x7fc12345,0xffc12345]\n def bits(x):return struct.unpack('<I',struct.pack('<f',x))[0]\n def add(mode=0,object_=0x1000,output=0x7000,input_=0x7200,matrix=0x7400,actor=0x5000,inertia=(3.,2.,1.),normal=(.25,.75,.5),matrix_kind=0,patches=(),seed=None,label='finite',edx=0xcccccccc):\n  seed=len(cases)+1 if seed is None else seed;r=random.Random(seed);arena=bytearray(r.randbytes(ARENA))\n  def put(a,*v):struct.pack_into('<'+'I'*len(v),arena,a,*[x&0xffffffff for x in v])\n  put(0x1000,BASE+actor if actor else 0);put(0x1008,*map(bits,inertia));put(0x1074,*[bits(r.uniform(-3,3))for _ in range(3)]);put(0x1244,*map(bits,normal))\n  mat=list(identity)\n  if matrix_kind==1:mat=[bits(r.uniform(-4,4))for _ in range(12)]\n  if matrix_kind==2:mat=[r.choice(special)for _ in range(12)]\n  if actor and actor+0x2c+48<=ARENA:put(actor+0x2c,*mat)\n  put(0x7400,*mat);put(0x7200,*[bits(r.uniform(-4,4))for _ in range(3)])\n  for a,v in patches:put(a,v)\n  c=dict(mode=mode,object=object_,output=output,input=input_,matrix=matrix,actor=actor,inertia=inertia,normal=normal,matrix_kind=matrix_kind,patches=patches,seed=seed,label=label,unused_edx=edx)\n  cases.append((c,struct.pack('<8I',mode,object_,output,input_,matrix,0,0,edx)+arena))\n for mode in [0,1]:\n  for seed in range(1,65):add(mode=mode,seed=seed,matrix_kind=seed%2,normal=(.25,-.5,.75))\n for inertia in [(3,2,1),(1,3,2),(2,1,3),(1,1,1),(2,2,1),(1,2,2),(2,1,2),(-1,-2,-3)]:\n  for normal in [(0,0,1),(0,0,-1),(1,0,0),(0,1,0),(0,0,0),(.3,.4,.5),(-.25,.5,.75)]:add(inertia=inertia,normal=normal,label='axis-choice-and-normal')\n for field in [0x1008,0x100c,0x1010,0x1244,0x1248,0x124c,0x1074,0x1078,0x107c]:\n  for value in special:add(patches=[(field,value)],label='parent-IEEE')\n for lane in range(9):\n  for value in special:add(patches=[(0x502c+4*lane,value)],label='parent-matrix-IEEE')\n for value in special:\n  for lane in range(3):add(mode=1,patches=[(0x7200+4*lane,value)],label='lower-input-IEEE')\n  for lane in range(9):add(mode=1,patches=[(0x7400+4*lane,value)],label='lower-matrix-IEEE')\n pairs=[(0x7f812345,0xff812345),(0x7fc12345,0xffc12345),(0x7f812345,0xffc12345),(0xff812345,0x7fc12345),(0x7f800001,0xff800002),(0xff800001,0x7f800002)]\n for a,b in pairs:\n  for lane in range(3):\n   for row in range(3):add(mode=1,patches=[(0x7200+4*lane,a),(0x7400+12*row+4*lane,b)],label='lower-paired-input-matrix')\n  for row in [1,2]:\n   for lane in range(3):add(patches=[(0x502c+12*row+4*lane,a),(0x1074+4*row,b)],label='parent-other-matrix-row-and-omega-pair')\n for delta in range(-12,13):add(mode=1,output=0x7200+delta,label='lower-output-input-partial-alias')\n for delta in range(-12,48):add(mode=1,output=0x7400+delta,label='lower-output-matrix-partial-alias')\n for delta in range(-12,13):add(mode=1,input_=0x7400+delta,label='lower-input-matrix-partial-alias')\n for base in [0x1000,0x1008,0x1074,0x1244]:\n  for delta in [-7,-4,-1,0,1,4,7]:\n   mat=base+delta;patches=[(0x1000,BASE+mat-0x2c)]\n   add(actor=0x5000,patches=patches,label='parent-captured-matrix-public-object-alias')\n # Gate is wide even though the same dot has been stored32.\n # SelectedX axis=-X; exactly \u2212.99 and its neighboring scalar coordinates.\n boundary=bits(.99)\n for n in range(boundary-3,boundary+4):\n  for sign in [0,0x80000000]:add(normal=(.99,.125,.25),patches=[(0x1244,n|sign)],label='wide-dot-threshold-neighbors')\n for seed in range(128):\n  r=random.Random(0x4b6fb000+seed);patches=[]\n  for field in [0x1244,0x1248,0x124c,0x502c,0x5030,0x5034]:patches.append((field,(r.getrandbits(1)<<31)|(r.randint(1,253)<<23)|r.getrandbits(23)))\n  add(patches=patches,seed=seed+1001,label='finite-exponent-asymmetric-norms')\n for edx in [0,1,0x7f812345,0xdeadbeef,BASE+0x7400]:add(edx=edx,label='unused-EDX')\n add(object_=0,label='parent-authentic-NULL-object-fault');add(actor=0,label='parent-authentic-NULL-actor-fault')\n for distance in [8,12,16,0x244,0x248,0x24c,0x74,0x78,0x7c]:\n  obj=ARENA-distance;patches=[]\n  for offset,value in [(0,BASE+0x5000),(8,bits(3)),(12,bits(2)),(16,bits(1)),(0x244,bits(.25)),(0x248,bits(.75)),(0x24c,bits(.5)),(0x74,bits(1)),(0x78,bits(1)),(0x7c,bits(1))]:\n   if obj+offset+4<=ARENA:patches.append((obj+offset,value))\n  add(object_=obj,patches=patches,label='parent-phase-page-fault')\n for output,input_,matrix in [(0,0x7200,0x7400),(0x7000,0,0x7400),(0x7000,0x7200,0),(0x7000,ARENA-8,0x7400),(0x7000,0x7200,ARENA-4),(ARENA-8,0x7200,0x7400),(ARENA-4,0x7200,0x7400),(0x7000,ARENA-4,ARENA-4)]:\n  add(mode=1,output=output,input_=input_,matrix=matrix,label='lower-authentic-fault')\n # Start-of-arena straddles prove original load order when an earlier\n # coefficient is inaccessible while a later coefficient remains mapped.\n for output,input_,matrix in [(0x7000,0x7200,0xfffffff8),(0x7000,0,0xfffffff8),(0x7000,0,0xfffffffc),(0x7000,0xfffffff8,0x7400),(0x7000,0xfffffffc,0x7400),(0x7000,0xfffffff8,0xfffffff8),(0x7000,0xfffffffc,0xfffffffc),(0x7000,0x7200,0xfffffff0)]:\n  add(mode=1,output=output,input_=input_,matrix=matrix,label='lower-original-load-order-start-page-fault')\n return cases\n"
ENTRIES=[('LevelOutOnSurface',0x4b6fb0,503),('BrMatrix34TApplyV',0x533690,107)]
PHYSICAL=r'''
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned h[5],i,offset,value;unsigned char*image;
if(argc!=3)return 1;f=fopen(argv[2],"rb");if(!f)return 2;if(fread(h,sizeof(h),1,f)!=1||h[1]>=h[0]||h[2]>=h[0])return 3;
image=VirtualAlloc(NULL,h[0],MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image||fread(image,h[0],1,f)!=1)return 4;
for(i=0;i<h[3];i++){if(fread(&offset,4,1,f)!=1||offset>h[0]-4)return 5;memcpy(&value,image+offset,4);value=(unsigned)image+value-h[4];memcpy(image+offset,&value,4);}fclose(f);
FlushInstructionCache(GetCurrentProcess(),image,h[0]);addresses[0]=(unsigned)image+h[1];addresses[1]=(unsigned)image+h[2];return run(argv[1]);}
'''

def original_image(binary):
 cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;roots={a for _,a,n in ENTRIES};constants={};refs=[];bodies=[]
 for name,a,n in ENTRIES:
  code=raw(binary,a,n);ins=list(cs.disasm(code,a));assert sum(i.size for i in ins)==n;calls=[];starts={i.address for i in ins}
  for i in ins:
   if i.mnemonic=='call':assert i.operands[0].type==CS_OP_IMM and i.operands[0].imm in roots;calls.append(dict(site=i.address,target=i.operands[0].imm))
   if i.mnemonic.startswith('j'):assert i.operands[0].type==CS_OP_IMM and i.operands[0].imm in starts
   if i.mnemonic=='ret':assert ([o.imm for o in i.operands]or[0])==[0]
   for op in i.operands:
    if op.type==CS_OP_MEM and not op.mem.base and not op.mem.index:
     assert 0x580000<=op.mem.disp<0x590000 and op.size in [4,8]and i.disp_size==4
     refs.append(i.address+i.disp_offset);constants[op.mem.disp,op.size]=raw(binary,op.mem.disp,op.size)
  bodies.append(dict(name=name,address=a,bytes=n,sha256=sha(code),calls=calls,instructions=[[i.address,i.bytes.hex()]for i in ins]))
 low=min(roots);high=max([a+n for _,a,n in ENTRIES]+[a+n for a,n in constants]);image=bytearray(b'\xcc'*(high-low))
 for _,a,n in ENTRIES:image[a-low:a-low+n]=raw(binary,a,n)
 for (a,n),b in constants.items():image[a-low:a-low+n]=b
 return words([len(image),0x4b6fb0-low,0x533690-low,len(refs),low])+image+words([a-low for a in refs]),dict(bodies=bodies,bytes=610,constants=[dict(address=a,bytes=n,hex=b.hex())for(a,n),b in constants.items()],absolute_relocations=refs,low=low,high=high)

def matching_image(exe,symbols_path):
 data=exe.read_bytes();mapping=json.loads(symbols_path.read_text())
 assert mapping['format']==1 and mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(data)
 mapped={}
 for row in mapping['symbols']:
  if row['orig'] in mapped:assert mapped[row['orig']]==row['recomp'],row
  mapped[row['orig']]=row['recomp']
 roots=[mapped[0x4b6fb0],mapped[0x533690]]
 public={roots[0]:('LevelOutOnSurface','60-phil.c.obj'),roots[1]:('BrMatrix34TApplyV','libbrender:matrix34.c.obj')}
 mapbytes=exe.with_suffix('.map').read_bytes();functions={}
 for name,a,owner in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',mapbytes.decode('latin1'),re.M):
  functions.setdefault(int(a,16),[]).append((name,owner.strip()))
 assert set(public)<=set(functions)
 nt,base,size,tab=pe(data);regions=[]
 for k in range(struct.unpack_from('<H',data,nt+6)[0]):
  vs,rva,n,off=struct.unpack_from('<IIII',data,tab+40*k+8);flags=struct.unpack_from('<I',data,tab+40*k+36)[0]
  regions.append((base+rva,max(vs,n),flags))
 helpers={'FrictionRead','FrictionWord','FrictionPointer','FrictionScalar','FrictionWrite','FrictionAt','FrictionAddScalar','FrictionSubtractScalar','PartnerSubtractWide','PartnerMultiplyWide','PartnerMultiplyRawScalar','PartnerMultiplyScalarWide','PhysicsJointSum','FrictionGreater','FrictionStatus'}
 matrix_helpers={'Matrix34ApplyProductLoaded','Matrix34ApplyProductMemory','Matrix34ApplyAdd'}
 cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;todo=list(roots);seen=set();instructions={};constants={};refs=set();graph=[];edges=[]
 def undecorate(name):return name.lstrip('_@').split('@')[0]
 while todo:
  start=todo.pop()
  if start in seen:continue
  seen.add(start);aliases=functions[start]
  if start in public:
   name,owner=public[start];assert any(o==owner and undecorate(n)==name for n,o in aliases),(name,aliases)
  else:
   accepted=[(n,o)for n,o in aliases if undecorate(n)in helpers and o=='60-phil.c.obj' or undecorate(n)in matrix_helpers and o=='libbrender:matrix34.c.obj' or o.startswith('LIBC:')and undecorate(n)in {'sqrt','CIsqrt'}]
   assert accepted,('Unexpected actual callee',hex(start),aliases);name=accepted[0][0]
  end=min(a for a in functions if a>start);pending=[start];local={}
  while pending:
   at=pending.pop()
   while at not in local:
    assert start<=at<end,(name,hex(at));i=next(cs.disasm(raw(data,at,15),at));assert at+i.size<=end
    assert not any(at<old<at+i.size or old<at<old+len(b)for old,b in local.items()),('Overlapping decoding',name,hex(at))
    local[at]=bytes(i.bytes);instructions[at]=bytes(i.bytes)
    if i.mnemonic=='call'or i.group(CS_GRP_JUMP):
     assert i.operands[0].type==CS_OP_IMM,('Unproved indirect control transfer',name,hex(at),i.op_str)
     target=i.operands[0].imm
     if start<=target<end:
      if i.mnemonic=='call':assert target==start;todo.append(target)
      else:pending.append(target)
     else:
      assert target in functions,(name,i.op_str);todo.append(target);edges.append(dict(site=at,target=target,kind=i.mnemonic))
    for operand_index,op in enumerate(i.operands):
     if op.type==CS_OP_MEM and base<=op.mem.disp<base+size:
      assert not op.mem.base and not op.mem.index and i.disp_size==4
      value=op.mem.disp;width=op.size;site=at+i.disp_offset
      region=next(r for r in regions if r[0]<=value and value+width<=r[0]+r[1])
      assert not region[2]&0xa0000020 and width in [4,8,10],('Unexpected mutable/code operand',name,i.op_str)
      assert not (operand_index==0 and i.mnemonic in ['mov','fst','fstp']),('Unexpected static store',name,i.op_str)
      constants[value,width]=raw(data,value,width);refs.add(site)
     elif op.type==CS_OP_IMM and i.mnemonic in ['mov','push']and base<=op.imm<base+size:
      raise AssertionError(('Unclassified absolute immediate',name,hex(at),i.op_str))
    if i.mnemonic.startswith('ret'):
     if start in public:assert ([o.imm for o in i.operands]or[0])==[0]
     break
    assert i.mnemonic not in ['int','int3'],('Terminal instruction outside retained fault contract',name,hex(at))
    if i.mnemonic=='jmp':break
    at+=i.size
  graph.append(dict(name=name,address=start,end=end,map_aliases=aliases,instructions=[[a,b.hex()]for a,b in sorted(local.items())],instruction_count=len(local),instruction_sha256=sha(b''.join(b for a,b in sorted(local.items())))))
 active={a+k for a,b in instructions.items()for k in range(len(b))};rr,rz=struct.unpack_from('<II',data,nt+24+96+5*8);p=base+rr;checked=[]
 while p<base+rr+rz:
  page,n=struct.unpack('<II',raw(data,p,8));assert n>=8 and n%2==0 and p+n<=base+rr+rz
  for w in struct.unpack('<'+'H'*((n-8)//2),raw(data,p+8,n-8)):
   site=base+page+(w&4095);kind=w>>12
   if any(site+k in active for k in range(4))and kind:
    assert kind==3 and site in refs and all(site+k in active for k in range(4)),('Unclassified active relocation',hex(site),kind);checked.append(site)
  p+=n
 assert set(checked)==refs,('Missing/extra readonly operand relocations',sorted(refs-set(checked)))
 low=min([*instructions,*[a for a,n in constants]]);high=max([a+len(b)for a,b in instructions.items()]+[a+n for a,n in constants]);image=bytearray(b'\xcc'*(high-low))
 for a,b in instructions.items():image[a-low:a-low+len(b)]=b
 for(a,n),b in constants.items():assert not any(a+k in active for k in range(n));image[a-low:a-low+n]=b
 refs=sorted(refs);blob=words([len(image),roots[0]-low,roots[1]-low,len(refs),low])+image+words([a-low for a in refs])
 return blob,dict(recompiled_sha256=sha(data),symbols_sha256=sha(symbols_path.read_bytes()),map_sha256=sha(mapbytes),original_roots=[0x4b6fb0,0x533690],roots=roots,graph=graph,edges=edges,constants=[dict(address=a,bytes=n,hex=b.hex())for(a,n),b in sorted(constants.items())],absolute_relocations=refs,active_relocations_checked=checked,mutable_globals=[],indirect_control_transfers=[],normalization=[],low=low,high=high)

def main():
 parser=argparse.ArgumentParser();parser.add_argument('--output',required=True,type=Path);parser.add_argument('--lower-source',type=Path,default=ROOT/'src/brender/core/math/matrix34.c');parser.add_argument('--candidate-source',type=Path);parser.add_argument('--recompiled',type=Path);parser.add_argument('--symbols',type=Path);parser.add_argument('--limit',type=int);parser.add_argument('--case-index',type=int,action='append');args=parser.parse_args();assert bool(args.recompiled)==bool(args.symbols)
 out=args.output.resolve();assert not out.exists();out.mkdir(parents=True)
 physics=ROOT/'src/carma2/newcommon/49-physics.c';candidate=args.candidate_source.resolve()if args.candidate_source else ROOT/'src/carma2/newcommon/60-phil.c';lower=args.lower_source.resolve()
 paths=[Path(__file__),candidate,lower,physics,ROOT/'src/brender/core/math/matrix34.c',ROOT/'build-reference/originals/CARMA2_HW.EXE']+list((ROOT/'src').rglob('*.h'))+[ROOT/'scripts'/n for n in ['verify_opponent_route_reconstruction.py','verify_tinted_native_reconstruction.py','verify_phil_registry_reconstruction.py','verify_opponent_navigation_reconstruction.py']]
 if args.recompiled:paths += [args.recompiled,args.symbols,args.recompiled.with_suffix('.map')]
 before={str(p.resolve()):sha(p.read_bytes())for p in paths};(out/'source-before.json').write_text(json.dumps(before,indent=2)+'\n');snapshot=out/'sources';snapshot.mkdir()
 snapshots={}
 for j,p in enumerate(paths):
  dest=snapshot/(str(j)+'-'+p.name);dest.write_bytes(p.read_bytes());snapshots[str(p.resolve())]=dict(path=str(dest),sha256=sha(dest.read_bytes()))
 binary=(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes();assert sha(binary)==HASH;blob,graph=original_image(binary);(out/'original-code.bin').write_bytes(blob)
 source=physics.read_text();names=['FrictionRead','FrictionWord','FrictionPointer','FrictionScalar','FrictionWrite','FrictionAt','FrictionAddScalar','FrictionSubtractScalar','PartnerSubtractWide','PartnerMultiplyWide','PartnerMultiplyRawScalar','PartnerMultiplyScalarWide','PhysicsJointSum','FrictionGreater']
 # The published raw pointer helper has no explicit hook annotation.
 def helper(name):
  if name not in ['PartnerScalarPtr','ForceBytes']:return extract(source,name)
  found=re.findall(r'^static [^\n]+\b'+name+r'\([^\n]+\) \{[^}]*\}',source,re.M);assert len(found)==1
  return found[0]
 # Preserve only the same legacy status predicate used by the actual helpers.
 body='\n'.join(helper(n)for n in names[:-1])+'\n#if defined(_MSC_VER) && _MSC_VER < 1300\n'+extract(source,'FrictionStatus')+'\n#endif\n'+extract(source,'FrictionGreater')+'\n'
 if lower.resolve()==(ROOT/'src/brender/core/math/matrix34.c').resolve():
  lower_text=lower.read_text();lower_capsule='\n'.join(extract(lower_text,n)for n in ['Matrix34ApplyProductLoaded','Matrix34ApplyProductMemory','Matrix34ApplyAdd','BrMatrix34TApplyV'])+'\n'
 else:lower_capsule=lower.read_text()
 if args.candidate_source:capsule=candidate.read_text()
 else:
  parent=candidate.read_text()
  for name in names[:-1]+['FrictionStatus','FrictionGreater']:assert extract(parent,name)==helper(name),(name,'Parent helper differs from genuine published kernel')
  capsule=extract(parent,'LevelOutOnSurface')+'\n'

 common=HARNESS;native=common+'\n'+body+'\n'+lower_capsule+'\n'+capsule+'\nint C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;addresses[0]=(unsigned)LevelOutOnSurface;addresses[1]=(unsigned)BrMatrix34TApplyV;return run(argv[1]);}\n'
 caller,_,_=registry.literals();caller=caller.replace('builder_poison;','builder_poison,builder_cdecl;');old=' mov builder_result,eax\n mov ecx,esp';assert old in caller;caller=caller.replace(old,' xor eax,eax\n mov builder_result,eax\n cmp builder_cdecl,0\n je level_stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n level_stackready:\n mov ecx,esp')
 registry.COMMON=common;commands=registry.compile_sources(out,native,PHYSICAL,caller)
 namespace={};exec(compile(FIXTURE_PROGRAM,'<retained-level-out-fixtures>','exec'),namespace);cases=namespace['fixtures']();indices=args.case_index or list(range(len(cases)));indices=indices[:args.limit]if args.limit else indices;rows=[];failures=[];events={};matching=None
 if args.recompiled:
  linked,matching=matching_image(args.recompiled,args.symbols);(out/'matching-code.bin').write_bytes(linked)
 for j,index in enumerate(indices):
  case,payload=cases[index];p=out/'case.bin';p.write_bytes(payload);r=subprocess.run([str(out/'physical.exe'),str(p),str(out/'original-code.bin')],capture_output=True,timeout=30)
  assert r.returncode==0 and len(r.stdout)==65548,('original',index,r.returncode,len(r.stdout),r.stderr)
  expected=r.stdout;event,value,address=struct.unpack_from('<3I',expected);events[index]=dict(event=event,value=value,address=address)
  for compiler in ['msvc5','msvc','mingw']+(['matching']if matching else []):
   command=[str(out/('physical.exe'if compiler=='matching'else compiler+'.exe')),str(p)];command += [str(out/'matching-code.bin')]if compiler=='matching'else []
   r=subprocess.run(command,capture_output=True,timeout=30);passed=r.returncode==0 and r.stdout==expected
   rows.append(dict(index=index,compiler=compiler,passed=passed,input_sha256=sha(payload),original_output_sha256=sha(expected),actual_output_sha256=sha(r.stdout),original_bytes=len(expected),actual_bytes=len(r.stdout),event=event,normal_return_abi_checked=event==0,exit=r.returncode))
   if not passed:
    key=compiler+'-'+str(index);(out/('failure-'+key+'-input.bin')).write_bytes(payload);(out/('failure-'+key+'-original.bin')).write_bytes(expected);(out/('failure-'+key+'-actual.bin')).write_bytes(r.stdout);(out/('failure-'+key+'-stderr.bin')).write_bytes(r.stderr)
    first=next((k for k,(a,b)in enumerate(zip(expected,r.stdout))if a!=b),None);failures.append(dict(index=index,compiler=compiler,case=case,exit=r.returncode,first_difference=first))
  if j%25==0:print('cases',j+1,'/',len(indices),'failures',len(failures),flush=True)
 after={str(p.resolve()):sha(p.read_bytes())for p in paths};assert before==after
 result=dict(passed=not failures,scenes=len(indices),comparisons=len(rows),complete_suite=indices==list(range(len(cases))),selected_indices=indices,total_scenes=len(cases),original_sha256=HASH,graph=graph,source_before=before,source_after=after,source_stable=True,source_snapshots=snapshots,fixture_sha256=[sha(payload)for _,payload in cases],fixtures_sha256=sha(b''.join(payload for _,payload in cases)),classification=events,results=rows,failures=failures,commands=commands,generated_sha256={str(p):sha(p.read_bytes())for p in out.iterdir()if p.is_file()},live_handles=[],limits=['Full physical2-body610B original graph and genuine complete matrix lower; no math/movement/control substitute.','Raw64KiB arena+terminalfaultstate; voidEAX excluded, normal ESP/nonvolatiles/x87TOP checked. PC53/masked, not sticky/unmasked/private-stack/nonx86/exhaustive or whole-game proof.'])
 if matching:result.update(matching=matching,recompiled_sha256=sha(args.recompiled.read_bytes()),symbols_sha256=sha(args.symbols.read_bytes()))
 (out/'verification.json').write_text(json.dumps(result,indent=2)+'\n');print('CLOSED',result['passed'],len(indices),len(rows),len(failures),flush=True)
 if failures:raise SystemExit(1)
if __name__=='__main__':main()
