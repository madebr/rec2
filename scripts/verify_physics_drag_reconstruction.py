"""Complete drag/angular physical-original/native3 scratch proof, no credit."""
import argparse,hashlib,json,re,runpy,struct,subprocess,sys
from pathlib import Path
ROOT=next(p for p in Path(__file__).resolve().parents if (p/"src/carma2").is_dir());sys.path.insert(0,str(ROOT/'scripts'))
from verify_opponent_route_reconstruction import raw,HASH,words,sha,pe
from verify_tinted_native_reconstruction import extract
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM,CS_OP_REG,CS_GRP_JUMP
import verify_phil_registry_reconstruction as registry
FOLDER=ROOT/'build-audit/physics-drag-next'
HARNESS='#include <windows.h>\n#include <stdio.h>\n#include <stdlib.h>\n#include <string.h>\n#include <stdint.h>\n#include <float.h>\n#include <math.h>\n#include <io.h>\n#include <fcntl.h>\n#include "49-physics.h"\n#include "41-utility.h"\n#define ARENA 65536\nstatic unsigned char* arena;\nstatic unsigned addresses[2];\nextern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison,builder_cdecl;\nextern unsigned __cdecl BuilderInvoke(void);\ntypedef char drag_layout[sizeof(void*)==4&&sizeof(tPhysics_object)==1240&&sizeof(tPHIL_queued_header)==24&&offsetof(tPHIL_queued_header,field_0x14)==20&&offsetof(tPhysics_object,child)==0x224&&offsetof(tPhysics_object,next)==0x220&&offsetof(tPhysics_object,water_depth_factor)==0x254?1:-1];\nstatic void snapshot(unsigned event,unsigned value,unsigned address){unsigned h[3];h[0]=event;h[1]=value;h[2]=address;fwrite(h,12,1,stdout);fwrite(arena,ARENA,1,stdout);fflush(stdout);}\nstatic LONG WINAPI observe_fault(EXCEPTION_POINTERS* e){if(e->ExceptionRecord->ExceptionCode!=EXCEPTION_ACCESS_VIOLATION)return EXCEPTION_CONTINUE_SEARCH;snapshot(3,e->ExceptionRecord->ExceptionCode,(unsigned)e->ExceptionRecord->ExceptionInformation[1]);ExitProcess(0);return EXCEPTION_CONTINUE_SEARCH;}\nstatic int run(const char* path){FILE*f;unsigned h[8];typedef void*(WINAPI *add_handler)(ULONG,LONG(WINAPI*)(EXCEPTION_POINTERS*));FARPROC proc;add_handler handler;\nSetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);f=fopen(path,"rb");if(!f)return 10;arena=VirtualAlloc((void*)0x22000000,ARENA,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(arena!=(void*)0x22000000)return 11;\nif(fread(h,sizeof(h),1,f)!=1||fread(arena,ARENA,1,f)!=1)return 12;fclose(f);_setmode(_fileno(stdout),_O_BINARY);\nproc=GetProcAddress(GetModuleHandleA("kernel32.dll"),"AddVectoredExceptionHandler");memcpy(&handler,&proc,4);if(!handler||!handler(1,observe_fault))return 13;if(h[0]>1)return 15;\nbuilder_address=addresses[h[0]];builder_mode=0;builder_poison=h[7];\nif(h[0]==0){builder_x=h[2]?0x22000000+h[2]:0;builder_y=h[1]?0x22000000+h[1]:0;builder_stack[0]=h[3];builder_stack[1]=h[4];builder_stack[2]=h[5]?0x22000000+h[5]:0;\n#if defined(REC2_MATCHING)\nbuilder_cdecl=0;builder_stack_count=3;\n#else\nbuilder_cdecl=1;builder_stack_count=5;builder_stack[4]=builder_stack[2];builder_stack[3]=builder_stack[1];builder_stack[2]=builder_stack[0];builder_stack[0]=builder_x;builder_stack[1]=builder_y;\n#endif\n}else{builder_x=h[1]?0x22000000+h[1]:0;builder_y=h[6]?0x22000000+h[6]:0;\n#if defined(REC2_MATCHING)\nbuilder_cdecl=0;builder_stack_count=0;\n#else\nbuilder_cdecl=1;builder_stack_count=2;builder_stack[0]=builder_x;builder_stack[1]=builder_y;\n#endif\n}\n_controlfp(_PC_53,_MCW_PC);_clearfp();if(BuilderInvoke())return 14;snapshot(0,0,0);return 0;}\n'
FIXTURE_PROGRAM="import random,struct\nBASE=0x22000000;ARENA=65536\n\ndef fixtures():\n cases=[]\n def add(mode=0,tree=0,flags=0,drag=0x3f800000,volume=0x6200,header=0x6000,object_=0x1000,decrement=0x6400,patches=(),seed=None,label='finite'):\n  seed=len(cases)+1 if seed is None else seed;r=random.Random(seed);arena=bytearray(r.randbytes(ARENA))\n  def put(a,*v):struct.pack_into('<'+'I'*len(v),arena,a,*[x&0xffffffff for x in v])\n  def f(v):return struct.unpack('<I',struct.pack('<f',v))[0]\n  for obj in [0x1000,0x2000,0x3000,0x4000]:\n   put(obj+4,f(r.uniform(.01,12)),f(r.uniform(.01,10)),f(r.uniform(.01,10)),f(r.uniform(.01,10)))\n   put(obj+0x68,*[f(r.uniform(-20,20))for _ in range(6)]);put(obj+0x220,0,0);put(obj+0x254,f(r.uniform(0,3)))\n  if tree in [1,2,3]:put(0x1000+0x224,BASE+0x2000)\n  if tree in [2,3]:put(0x2000+0x220,BASE+0x3000)\n  if tree==3:put(0x2000+0x224,BASE+0x4000)\n  put(0x6000+0x14,f(1));put(0x6200+0x8c,f(3));put(0x6400,*[f(r.uniform(-20,20))for _ in range(3)])\n  for a,v in patches:put(a,v)\n  case=dict(mode=mode,tree=tree,flags=flags,drag=drag,volume=volume,header=header,object=object_,decrement=decrement,patches=patches,seed=seed,label=label)\n  cases.append((case,struct.pack('<8I',mode,object_,header,drag,flags,volume,decrement,[0,0xcccccccc,0x7f800001][seed%3])+arena))\n for mode in [0,1]:\n  for seed in range(1,49):add(mode=mode,tree=seed%4,flags=[0,8,0xffffffff,0x80000008][seed%4],seed=seed)\n for flags in [0,1,8,9,0x80000000,0xffffffff]:\n  for volume in [0,0x6200]:\n   for h in [0,0x3f800000,0xbf800000]:add(flags=flags,volume=volume,patches=[(0x6014,h)])\n special=[0,0x80000000,1,0x80000001,0x00800000,0x3f800000,0xbf800000,0x7f7fffff,0x7f800000,0xff800000,0x7f800001,0xff800001,0x7fc12345,0xffc12345]\n for bits in special:\n  add(drag=bits,tree=3,flags=8,label='recursive-raw-drag-IEEE')\n  for field in [4,8,12,16,0x68,0x6c,0x70,0x74,0x78,0x7c,0x254]:add(flags=8,patches=[(0x1000+field,bits)],label='drag-object-IEEE')\n  for field in [0x6014,0x628c]:add(flags=8,patches=[(field,bits)],label='drag-header-volume-IEEE')\n  for field in [8,12,16,0x74,0x78,0x7c]:add(mode=1,patches=[(0x1000+field,bits)],label='angular-object-IEEE')\n  for field in [0x6400,0x6404,0x6408]:add(mode=1,patches=[(field,bits)],label='angular-damping-IEEE')\n pairs=[(0x7f812345,0xff812345),(0x7fc12345,0xffc12345),(0x7f812345,0xffc12345),(0xff812345,0x7fc12345),(0x7f800001,0xff800002),(0xff800001,0x7f800002)]\n for a,b in pairs:\n  for axis in range(3):\n   add(mode=1,patches=[(0x6400+4*axis,a),(0x1008+4*axis,b)],label='paired-damping-inertia')\n   add(mode=1,patches=[(0x6400+4*axis,a),(0x1074+4*axis,b)],label='paired-damping-omega')\n  add(flags=8,patches=[(0x1254,a),(0x628c,b)],label='paired-volume-depth')\n  add(drag=a,flags=8,patches=[(0x628c,b)],label='paired-volume-drag')\n  add(patches=[(0x1068,a),(0x106c,b)],label='paired-norm-XY')\n  add(patches=[(0x1074,a),(0x107c,b)],label='paired-norm-XZ')\n for delta in range(-12,13):\n  for base in [0x1008,0x1074]:add(mode=1,decrement=base+delta,label='angular-decrement-inertia-omega-alias')\n for volume in [0x1068-0x8c,0x1074-0x8c,0x1004-0x8c]:\n  add(tree=3,flags=8,volume=volume,label='live-recursive-volume-output-alias')\n for header in [0x1068-0x14,0x1074-0x14]:add(tree=3,flags=8,header=header,label='live-recursive-header-output-alias')\n for bits in [0,0x80000000,0x80000001,0x80000030,0x3f800000,0xbf800000]:\n  for inertia in [0x3f7fffff,0x3f800000,0x3f800001]:\n   add(mode=1,patches=[(0x1078,0x80000001),(0x6404,bits),(0x100c,inertia)],label='Y-wide-sign-versus-rounded-zero')\n for viscosity in [0x3fffffff,0x40000000,0x40000001]:\n  for queued in [0,0x80000000,0xbf800000,0x3f800000,0x7f800001]:add(flags=8,patches=[(0x6014,queued),(0x628c,viscosity)],label='exact-viscosity-threshold')\n for seed in range(101,153):\n  add(mode=seed%2,tree=seed%4,flags=8,drag=[0x00800000,0x7f7fffff,0xbf800000,1][seed%4],seed=seed,label='finite-exponent-and-scale')\n child=0xe48\n aliaspatch=[(0x1224,BASE+child),(0x1068,BASE+0x3000),(0x106c,BASE+0x4000),(child+4,0x3f800000),(child+8,0x3f800000),(child+12,0x3f800000),(child+16,0x3f800000),(child+0x68,0x3f800000),(child+0x6c,0x3f800000),(child+0x70,0x3f800000),(child+0x74,0),(child+0x78,0),(child+0x7c,0)]\n add(drag=0x7f7fffff,patches=aliaspatch,label='linear-clamp-overwrites-live-child-next-and-child')\n add(mode=0,object_=0,volume=0,label='authentic-NULL-object-fault')\n add(mode=0,header=0,flags=8,label='authentic-NULL-header-fault')\n add(mode=0,header=0,flags=0,label='unused-NULL-header-valid')\n add(mode=1,object_=0,label='authentic-NULL-angular-object-fault')\n add(mode=1,decrement=0,label='authentic-NULL-angular-vector-fault')\n for distance in [0x74,0x78,0x7c]:\n  obj=ARENA-distance;patches=[(obj+4,0x3f800000),(obj+8,0x3f800000),(obj+12,0x3f800000),(obj+16,0x3f800000)]\n  patches += [(obj+off,0x3f800000)for off in [0x74,0x78,0x7c]if obj+off<ARENA]\n  add(mode=1,object_=obj,patches=patches,label='angular-original-raw-snapshot-page-fault')\n for distance in [0x70,0x74,0x224]:\n  obj=ARENA-distance;patches=[(obj+4,0x3f800000),(obj+8,0x3f800000),(obj+12,0x3f800000),(obj+16,0x3f800000)]\n  patches += [(obj+off,0x3f800000)for off in [0x68,0x6c,0x70,0x74,0x78,0x7c]if obj+off<ARENA]\n  add(object_=obj,flags=0,patches=patches,label='drag-original-phase-page-fault')\n return cases\n"
ENTRIES=[('ProcessDrag2',0x4b6ce0,418),('FUN_004c2c90',0x4c2c90,207),('DRVector3Diminish',0x516390,124)]
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
   if i.mnemonic=='ret':assert ([o.imm for o in i.operands]or[0])==([12]if name=='ProcessDrag2'else[0])
   for op in i.operands:
    if op.type==CS_OP_MEM and not op.mem.base and not op.mem.index:
     assert 0x580000<=op.mem.disp<0x590000 and op.size in [4,8]and i.disp_size==4
     refs.append(i.address+i.disp_offset);constants[op.mem.disp,op.size]=raw(binary,op.mem.disp,op.size)
  bodies.append(dict(name=name,address=a,bytes=n,sha256=sha(code),calls=calls,instructions=[[i.address,i.bytes.hex()]for i in ins]))
 low=min(roots);high=max([a+n for _,a,n in ENTRIES]+[a+n for a,n in constants]);image=bytearray(b'\xcc'*(high-low))
 for _,a,n in ENTRIES:image[a-low:a-low+n]=raw(binary,a,n)
 for (a,n),b in constants.items():image[a-low:a-low+n]=b
 return words([len(image),0x4b6ce0-low,0x4c2c90-low,len(refs),low])+image+words([a-low for a in refs]),dict(bodies=bodies,bytes=749,constants=[dict(address=a,bytes=n,hex=b.hex())for(a,n),b in constants.items()],absolute_relocations=refs,low=low,high=high)

def matching_image(exe,symbols_path):
 data=exe.read_bytes();mapping=json.loads(symbols_path.read_text());assert mapping['format']==1 and mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(data)
 mapped={r['orig']:r['recomp']for r in mapping['symbols']};roots=[mapped[0x4b6ce0],mapped[0x4c2c90]]
 public={mapped[0x4b6ce0]:('ProcessDrag2','60-phil.c.obj'),mapped[0x4c2c90]:('FUN_004c2c90','49-physics.c.obj'),mapped[0x516390]:('DRVector3Diminish','41-utility.c.obj')}
 mapbytes=exe.with_suffix('.map').read_bytes();functions={}
 for name,a,owner in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',mapbytes.decode('latin1'),re.M):functions.setdefault(int(a,16),[]).append((name,owner.strip()))
 assert set(public)<=set(functions)
 nt,base,size,tab=pe(data);regions=[]
 for k in range(struct.unpack_from('<H',data,nt+6)[0]):
  vs,rva,n,off=struct.unpack_from('<IIII',data,tab+40*k+8);flags=struct.unpack_from('<I',data,tab+40*k+36)[0];regions.append((base+rva,max(vs,n),flags))
 helpers={'DragForward','DragLength','ForceBytes','FrictionRead','FrictionWord','FrictionPointer','FrictionScalar','FrictionWrite','FrictionAt','FrictionProductScalars','FrictionReverseDivideScalar','FrictionReverseSubtractScalar','PartnerMultiplyWide','PartnerMultiplyRawScalar','PartnerMultiplyScalarWide','PartnerScalarPtr','PhysicsJointSum','FrictionGreater','FrictionStatus','DiminishSubtractScalar'}
 owners={'49-physics.c.obj','60-phil.c.obj','41-utility.c.obj'};cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;todo=list(roots);seen=set();instructions={};constants={};refs=set();graph=[];edges=[];indirect=[];pointers=[]
 def family(register):
  n=cs.reg_name(register)
  for key,aliases in [('eax',['eax','ax','al','ah']),('ebx',['ebx','bx','bl','bh']),('ecx',['ecx','cx','cl','ch']),('edx',['edx','dx','dl','dh']),('esi',['esi','si']),('edi',['edi','di']),('ebp',['ebp','bp']),('esp',['esp','sp'])]:
   if n in aliases:return key
  return n
 while todo:
  start=todo.pop()
  if start in seen:continue
  seen.add(start);aliases=functions[start]
  if start in public:
   name,owner=public[start];assert any(o==owner for n,o in aliases),(name,aliases)
  else:
   accepted=[(n,o)for n,o in aliases if n.lstrip('_@').split('@')[0]in helpers and o in owners or o.startswith('LIBC:')and n.lstrip('_@').split('@')[0]in {'sqrt','CIsqrt','memcpy'}]
   assert accepted,('Unexpected actual callee',hex(start),aliases);name=accepted[0][0]
  end=min(a for a in functions if a>start);pending=[start];local={}
  while pending:
   at=pending.pop();known={}
   while at not in local:
    assert start<=at<end,(name,hex(at));i=next(cs.disasm(raw(data,at,15),at));assert at+i.size<=end
    local[at]=bytes(i.bytes);instructions[at]=bytes(i.bytes)
    if i.mnemonic=='mov'and i.operands[0].type==CS_OP_REG and i.operands[0].size==4 and i.operands[1].type==CS_OP_IMM and i.operands[1].imm==roots[0]:known[i.operands[0].reg]=(roots[0],at)
    else:
     writes={family(reg)for reg in i.regs_access()[1]}
     for reg in list(known):
      if family(reg)in writes:known.pop(reg)
    if i.mnemonic=='call'or i.group(CS_GRP_JUMP):
     if i.operands[0].type!=CS_OP_IMM:
      assert i.mnemonic=='call'and i.operands[0].type==CS_OP_REG and i.operands[0].reg in known,('Unproved raw recursive target',name,hex(at),i.op_str)
      target,load=known[i.operands[0].reg];assert target==roots[0]
      indirect.append(dict(function=start,address=at,load=load,target=target,register=cs.reg_name(i.operands[0].reg)));todo.append(target)
     else:
      target=i.operands[0].imm
      if start<=target<end:
       if i.mnemonic=='call':assert start==roots[0]and target==start
       else:pending.append(target)
      else:assert target in functions,(name,i.op_str);todo.append(target);edges.append(dict(site=at,target=target,kind=i.mnemonic))
    for op in i.operands:
     value=None
     if op.type==CS_OP_MEM and base<=op.mem.disp<base+size:
      assert not op.mem.base and not op.mem.index and i.disp_size==4;value=op.mem.disp;site=at+i.disp_offset;width=op.size
     elif op.type==CS_OP_IMM and i.mnemonic in ['mov','push']and base<=op.imm<base+size:
      assert i.imm_size==4;value=op.imm;site=at+i.imm_offset;width=4
     if value is not None:
      region=next(r for r in regions if r[0]<=value<r[0]+r[1])
      if region[2]&0x20000000:
       assert value==roots[0]and op.type==CS_OP_IMM and i.mnemonic in ['mov','push'],('Unexpected executable pointer',name,i.op_str)
       pointers.append(dict(function=start,site=site,target=value))
      else:
       assert not region[2]&0x80000020 and width in [4,8,10],('Unexpected mutable/static operand',name,i.op_str)
       constants[value,width]=raw(data,value,width)
      refs.add(site)
    if i.mnemonic=='call':known.clear()
    if i.mnemonic.startswith('ret'):
     if start in public:assert ([o.imm for o in i.operands]or[0])==([12]if start==roots[0]else[0])
     break
    if i.mnemonic in ['jmp','int','int3']:break
    at+=i.size
  graph.append(dict(name=name,address=start,end=end,map_aliases=aliases,instructions=[[a,b.hex()]for a,b in sorted(local.items())],instruction_count=len(local),instruction_sha256=sha(b''.join(b for a,b in sorted(local.items())))))
 for call in indirect:
  row=next(r for r in graph if r['address']==call['function'])
  for a,b in row['instructions']:
   i=next(cs.disasm(bytes.fromhex(b),a))
   if i.group(CS_GRP_JUMP)and i.operands[0].type==CS_OP_IMM and call['load']<i.operands[0].imm<=call['address']:assert call['load']<=a<=call['address'],('Branch bypasses immutable raw target load',call,hex(a),i.op_str)
 active={a+k for a,b in instructions.items()for k in range(len(b))};rr,rz=struct.unpack_from('<II',data,nt+24+96+5*8);p=base+rr;checked=[]
 while p<base+rr+rz:
  page,n=struct.unpack('<II',raw(data,p,8));assert n>=8
  for w in struct.unpack('<'+'H'*((n-8)//2),raw(data,p+8,n-8)):
   site=base+page+(w&4095)
   if w>>12==3 and site in active:assert site in refs,hex(site);checked.append(site)
  p+=n
 low=min([*instructions,*[a for a,n in constants]]);high=max([a+len(b)for a,b in instructions.items()]+[a+n for a,n in constants]);image=bytearray(b'\xcc'*(high-low))
 for a,b in instructions.items():image[a-low:a-low+len(b)]=b
 for(a,n),b in constants.items():assert not any(a+k in active for k in range(n));image[a-low:a-low+n]=b
 refs=sorted(refs);blob=words([len(image),roots[0]-low,roots[1]-low,len(refs),low])+image+words([a-low for a in refs])
 return blob,dict(recompiled_sha256=sha(data),symbols_sha256=sha(symbols_path.read_bytes()),map_sha256=sha(mapbytes),roots=roots,graph=graph,edges=edges,indirect_root_calls=indirect,code_pointer_sites=pointers,constants=[dict(address=a,bytes=n,hex=b.hex())for(a,n),b in sorted(constants.items())],absolute_relocations=refs,active_relocations_checked=checked,mutable_globals=[],normalization=[],low=low,high=high)

def main():
 parser=argparse.ArgumentParser();parser.add_argument('--output',required=True,type=Path);parser.add_argument('--lower-source',type=Path,default=ROOT/'src/carma2/newcommon/41-utility.c');parser.add_argument('--candidate-source',type=Path);parser.add_argument('--recompiled',type=Path);parser.add_argument('--symbols',type=Path);parser.add_argument('--limit',type=int);parser.add_argument('--case-index',type=int,action='append');args=parser.parse_args();assert bool(args.recompiled)==bool(args.symbols)
 out=args.output.resolve();assert not out.exists();out.mkdir(parents=True)
 physics=ROOT/'src/carma2/newcommon/49-physics.c';candidate=args.candidate_source.resolve()if args.candidate_source else ROOT/'src/carma2/newcommon/60-phil.c';lower=args.lower_source.resolve()
 paths=[Path(__file__),candidate,lower,physics,ROOT/'src/carma2/newcommon/41-utility.c',ROOT/'build-reference/originals/CARMA2_HW.EXE']+list((ROOT/'src').rglob('*.h'))+[ROOT/'scripts'/n for n in ['verify_opponent_route_reconstruction.py','verify_tinted_native_reconstruction.py','verify_phil_registry_reconstruction.py','verify_opponent_navigation_reconstruction.py']]
 if args.recompiled:paths += [args.recompiled,args.symbols,args.recompiled.with_suffix('.map')]
 before={str(p.resolve()):sha(p.read_bytes())for p in paths};(out/'source-before.json').write_text(json.dumps(before,indent=2)+'\n');snapshot=out/'sources';snapshot.mkdir()
 snapshots={}
 for j,p in enumerate(paths):
  dest=snapshot/(str(j)+'-'+p.name);dest.write_bytes(p.read_bytes());snapshots[str(p.resolve())]=dict(path=str(dest),sha256=sha(dest.read_bytes()))
 binary=(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes();assert sha(binary)==HASH;blob,graph=original_image(binary);(out/'original-code.bin').write_bytes(blob)
 source=physics.read_text();names=['ForceBytes','FrictionRead','FrictionWord','FrictionPointer','FrictionScalar','FrictionWrite','FrictionAt','FrictionProductScalars','FrictionReverseDivideScalar','FrictionReverseSubtractScalar','FrictionProductWide','PartnerMultiplyWide','PartnerMultiplyRawScalar','PartnerMultiplyScalarWide','PartnerScalarPtr','PhysicsJointSum','FrictionGreater']
 # The published raw pointer helper has no explicit hook annotation.
 def helper(name):
  if name not in ['PartnerScalarPtr','ForceBytes']:return extract(source,name)
  found=re.findall(r'^static [^\n]+\b'+name+r'\([^\n]+\) \{[^}]*\}',source,re.M);assert len(found)==1
  return found[0]
 # Preserve only the same legacy status predicate used by the actual helpers.
 body='\n'.join(helper(n)for n in names[:-1])+'\n#if defined(_MSC_VER) && _MSC_VER < 1300\n'+extract(source,'FrictionStatus')+'\n#endif\n'+extract(source,'FrictionGreater')+'\n'
 body=body.replace(extract(source,'FrictionProductWide'),'')
 names.remove('FrictionProductWide')
 common=HARNESS
 if args.lower_source.resolve()==(ROOT/'src/carma2/newcommon/41-utility.c').resolve():
  lower_text=lower.read_text();found=re.findall(r'#if defined\(__GNUC__\)\n(?:[^\n]*\n){1,5}#endif',lower_text)
  alias=[b for b in found if 'tDiminishRawWord'in b];assert len(alias)==1
  lower_capsule=alias[0]+'\n'+extract(lower_text,'DiminishSubtractScalar')+'\n'+extract(lower_text,'DRVector3Diminish')+'\n'
 else:lower_capsule=lower.read_text()
 if args.candidate_source:capsule=candidate.read_text()
 else:
  parent=candidate.read_text();angular=physics.read_text()
  types=re.findall(r'^typedef void \(C2_HOOK_FASTCALL\* tDrag(?:Words)?Function\)[^\n]*;',parent,re.M);assert len(types)==2
  # Every parent helper copy must be the exact published mathematical body
  # selected above; differently named gravity helpers are not substituted.
  for name in ['ForceBytes','FrictionRead','FrictionWord','FrictionPointer','FrictionScalar','FrictionAt','FrictionProductScalars','PartnerMultiplyWide','PartnerMultiplyRawScalar','PartnerMultiplyScalarWide','PartnerScalarPtr','PhysicsJointSum','FrictionStatus','FrictionGreater']:
   if name in ['ForceBytes','PartnerScalarPtr']:
    found=re.findall(r'^static [^\n]+\b'+name+r'\([^\n]+\) \{[^}]*\}',parent,re.M);assert len(found)==1;copied=found[0]
   else:copied=extract(parent,name)
   assert copied==helper(name),(name,'Parent helper copy differs from published body')
  capsule='void C2_HOOK_FASTCALL FUN_004c2c90(tPhysics_object*,const br_vector3*);\nvoid C2_HOOK_FASTCALL ProcessDrag2(tPHIL_queued_header*,tPhysics_object*,br_scalar,int,tSpecial_volume*);\n'+'\n'.join(types)+'\n'+'\n'.join(extract(parent,n)for n in ['DragForward','DragLength','ProcessDrag2'])+'\n'+extract(angular,'FUN_004c2c90')+'\n'
 native=common+'\n'+body+'\n'+lower_capsule+'\n'+capsule+'\nint C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;addresses[0]=(unsigned)ProcessDrag2;addresses[1]=(unsigned)FUN_004c2c90;return run(argv[1]);}\n'
 caller,_,_=registry.literals();caller=caller.replace('builder_poison;','builder_poison,builder_cdecl;');old=' mov builder_result,eax\n mov ecx,esp';assert old in caller;caller=caller.replace(old,' xor eax,eax\n mov builder_result,eax\n cmp builder_cdecl,0\n je drag_stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n drag_stackready:\n mov ecx,esp')
 registry.COMMON=common;commands=registry.compile_sources(out,native,PHYSICAL,caller)
 namespace={};exec(compile(FIXTURE_PROGRAM,'<retained-drag-fixtures>','exec'),namespace);cases=namespace['fixtures']();indices=args.case_index or list(range(len(cases)));indices=indices[:args.limit]if args.limit else indices;rows=[];failures=[];events={};matching=None
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
 result=dict(passed=not failures,scenes=len(indices),comparisons=len(rows),complete_suite=indices==list(range(len(cases))),selected_indices=indices,total_scenes=len(cases),original_sha256=HASH,graph=graph,source_before=before,source_after=after,source_stable=True,source_snapshots=snapshots,fixture_sha256=[sha(payload)for _,payload in cases],fixtures_sha256=sha(b''.join(payload for _,payload in cases)),classification=events,results=rows,failures=failures,commands=commands,generated_sha256={str(p):sha(p.read_bytes())for p in out.iterdir()if p.is_file()},live_handles=[],limits=['Full physical3-body749B original graph and genuine corrected vector lower; no math/movement/control substitute.','Raw64KiB arena+terminalfaultstate; voidEAX excluded, normal ESP/nonvolatiles/x87TOP checked. PC53/masked, not sticky/unmasked/private-stack/nonx86/exhaustive or whole-game proof.'])
 if matching:result.update(matching=matching,recompiled_sha256=sha(args.recompiled.read_bytes()),symbols_sha256=sha(args.symbols.read_bytes()))
 (out/'verification.json').write_text(json.dumps(result,indent=2)+'\n');print('CLOSED',result['passed'],len(indices),len(rows),len(failures),flush=True)
 if failures:raise SystemExit(1)
if __name__=='__main__':main()
