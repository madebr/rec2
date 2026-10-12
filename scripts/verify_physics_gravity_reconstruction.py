"""Full374B gravity original/native3 physical proof. Three genuine whole bodies, no numeric/control substitutes.
Raw64KiB arena and canonical mutableglobal, direct FASTCALL/CDECL scalar32 transport, normalESP/nonvolatiles/x87TOP, voidEAX excluded.
MaskedPC53 boundedIEEE/aliases/trees; realAV snapshots exclude normalABI. No private-stack/return/concurrent/unmasked/sticky/non-x86/exhaustive or whole-game claim.
"""
import argparse,hashlib,json,random,re,struct,subprocess,sys
from pathlib import Path
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'src/carma2').is_dir());sys.path.insert(0,str(ROOT/'scripts'))
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM,CS_GRP_JUMP,CS_GRP_CALL
from verify_opponent_route_reconstruction import raw,HASH,sha,words,pe
from verify_tinted_native_reconstruction import extract
import verify_phil_registry_reconstruction as registry
BASE=0x22000000;ARENA=65536;GLOBAL=0x3058
ENTRIES=[('ProcessGravity',0x4b6ec0,236),('FUN_004c29d0',0x4c29d0,55),('FUN_004c2910',0x4c2910,83)]
COMMON='\n#include <windows.h>\n#include <stdio.h>\n#include <stdlib.h>\n#include <string.h>\n#include <stdint.h>\n#include <float.h>\n#include <io.h>\n#include <fcntl.h>\n#include "49-physics.h"\n#include "60-phil.h"\n#include "globvars.h"\n#include "brender/br_defs.h"\n#define A(x,y) A->m[x][y]\n#define B(x,y) B->m[x][y]\n#define C(x,y) C->m[x][y]\nstatic unsigned char*arena;static unsigned addresses[3];\nextern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison,builder_cdecl;\nextern unsigned __cdecl BuilderInvoke(void);\ntypedef char gravity_layout[sizeof(void*)==4&&sizeof(tPhysics_object)==1240&&sizeof(tPHIL_queued_header)==24&&offsetof(tPHIL_queued_header,field_0x4)==4&&offsetof(tPHIL_queued_header,field_0x14)==0x14&&offsetof(tPhysics_object,field_0x54)==0x54&&offsetof(tPhysics_object,v)==0x68&&offsetof(tPhysics_object,disable_move_rotate)==0xec&&offsetof(tPhysics_object,water_depth_factor)==0x254&&offsetof(tPhysics_object,last_special_volume)==0x258&&offsetof(tPhysics_object,child)==0x224&&offsetof(tPhysics_object,next)==0x220&&offsetof(tSpecial_volume,gravity_multiplier)==0x88&&offsetof(tSpecial_volume,viscosity_multiplier)==0x8c?1:-1];\n#define gGravity_multiplier (*(volatile br_scalar*)(arena+0x3058))\nstatic void snapshot(unsigned event,unsigned value,unsigned address){unsigned h[3];h[0]=event;h[1]=value;h[2]=address;fwrite(h,12,1,stdout);fwrite(arena,65536,1,stdout);fflush(stdout);}\nstatic LONG WINAPI observe_fault(EXCEPTION_POINTERS*e){if(e->ExceptionRecord->ExceptionCode!=EXCEPTION_ACCESS_VIOLATION)return EXCEPTION_CONTINUE_SEARCH;snapshot(3,e->ExceptionRecord->ExceptionCode,(unsigned)e->ExceptionRecord->ExceptionInformation[1]);ExitProcess(0);return EXCEPTION_CONTINUE_SEARCH;}\nstatic int run(const char*path){FILE*f;unsigned h[5];typedef void*(WINAPI *add_handler)(ULONG,LONG(WINAPI*)(EXCEPTION_POINTERS*));FARPROC proc;add_handler handler;\nSetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);f=fopen(path,"rb");if(!f)return 10;arena=VirtualAlloc((void*)0x22000000,65536,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(arena!=(void*)0x22000000)return 11;\nif(fread(h,sizeof(h),1,f)!=1||fread(arena,65536,1,f)!=1)return 12;fclose(f);_setmode(_fileno(stdout),_O_BINARY);\nproc=GetProcAddress(GetModuleHandleA("kernel32.dll"),"AddVectoredExceptionHandler");memcpy(&handler,&proc,4);if(!handler||!handler(1,observe_fault))return 13;if(h[0]>2)return 15;\nbuilder_address=addresses[h[0]];builder_mode=0;builder_x=h[1]?0x22000000+h[1]:0;builder_y=h[2]?0x22000000+h[2]:0;builder_poison=h[4];\n#if defined(REC2_MATCHING)\nbuilder_cdecl=0;builder_stack_count=h[0]==0?1:0;builder_stack[0]=h[3];\n#else\nbuilder_cdecl=1;builder_stack_count=h[0]==0?3:h[0]==1?2:1;builder_stack[0]=builder_x;builder_stack[1]=builder_y;builder_stack[2]=h[3];\n#endif\n_controlfp(_PC_53,_MCW_PC);_clearfp();if(BuilderInvoke())return 14;snapshot(0,0,0);return 0;}\n'
PHYSICAL='\nint C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned h[6],i,offset,value;unsigned char*image;\nif(argc!=3)return 1;f=fopen(argv[2],"rb");if(!f)return 2;if(fread(h,sizeof(h),1,f)!=1||h[1]>=h[0]||h[2]>=h[0]||h[3]>=h[0])return 3;\nimage=VirtualAlloc(NULL,h[0],MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image||fread(image,h[0],1,f)!=1)return 4;\nfor(i=0;i<h[4];i++){if(fread(&offset,4,1,f)!=1||offset>h[0]-4)return 5;memcpy(&value,image+offset,4);value=(unsigned)image+value-h[5];memcpy(image+offset,&value,4);}fclose(f);\nFlushInstructionCache(GetCurrentProcess(),image,h[0]);addresses[0]=(unsigned)image+h[1];addresses[1]=(unsigned)image+h[2];addresses[2]=(unsigned)image+h[3];return run(argv[1]);}\n'

def original_image(binary):
 cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;roots={a:(name,n)for name,a,n in ENTRIES};refs=[];globals=[];constants={};bodies=[]
 for name,a,n in ENTRIES:
  code=raw(binary,a,n);instructions=list(cs.disasm(code,a));assert sum(i.size for i in instructions)==n;calls=[]
  for i in instructions:
   if i.mnemonic=='call':assert i.operands[0].type==CS_OP_IMM and i.operands[0].imm in roots;calls.append(dict(site=i.address,target=i.operands[0].imm))
   if CS_GRP_JUMP in i.groups:assert i.operands[0].type==CS_OP_IMM and a<=i.operands[0].imm<a+n
   if i.mnemonic=='ret':assert (i.operands[0].imm if i.operands else 0)==(4 if name=='ProcessGravity'else 0)
   for op in i.operands:
    if op.type==CS_OP_MEM and not op.mem.base and not op.mem.index:
     assert i.disp_size==4
     if op.mem.disp==0x74d1a8:assert op.size==4;globals.append(i.address+i.disp_offset)
     else:
      assert (op.mem.disp,op.size)in[(0x58a700,4),(0x58a720,8),(0x58a740,4),(0x58a748,8),(0x58a710,8),(0x58a750,8)]
      refs.append(i.address+i.disp_offset);constants[(op.mem.disp,op.size)]=raw(binary,op.mem.disp,op.size)
  bodies.append(dict(name=name,address=a,bytes=n,sha256=sha(code),instructions=[[i.address,i.bytes.hex()]for i in instructions],calls=calls))
 low=min(roots);high=max([a+n for name,a,n in ENTRIES]+[a+len(v)for(a,n),v in constants.items()]);image=bytearray(b'\xcc'*(high-low))
 for name,a,n in ENTRIES:image[a-low:a-low+n]=raw(binary,a,n)
 for(a,n),v in constants.items():image[a-low:a-low+n]=v
 for site in globals:struct.pack_into('<I',image,site-low,BASE+GLOBAL)
 blob=words([len(image),0x4b6ec0-low,0x4c29d0-low,0x4c2910-low,len(refs),low])+image+words([a-low for a in refs])
 assert len(globals)==3
 return blob,dict(bodies=bodies,total_original_bytes=374,absolute_relocations=refs,canonical_mutable_global=dict(original=0x74d1a8,name='gGravity_multiplier',arena_address=BASE+GLOBAL,operand_sites=globals),constants=[dict(address=a,bytes=n,hex=v.hex())for(a,n),v in constants.items()],low=low,high=high)

def fixtures():
 cases=[];objects=[0x100,0x1000,0x2000,0x3000]
 special=[0,0x80000000,1,0x80000001,0x00800000,0x3f800000,0xbf800000,0x40000000,0x40000001,0xc0000000,0x7f7fffff,0x7f800000,0xff800000,0x7f800001,0xff800001,0x7fc12345,0xffc12345]
 def add(mode=0,tree=0,flags=8,volume=0x9000,header=0x8000,object=0x100,vector=0xa000,gravity=0x3f800000,multiplier=0x3f800000,magnitude=0x3f800000,fluid=0x3f000000,viscosity=0x40400000,depth=0x3f800000,disabled=(0,0,0,0),deltas=None,velocities=None,patches=(),label='normal'):
  seed=len(cases)+1;r=random.Random(seed);arena=bytearray(r.randbytes(ARENA))
  def put(at,*v):struct.pack_into('<'+'I'*len(v),arena,at,*[x&0xffffffff for x in v])
  for j,o in enumerate(objects):
   put(o+0x220,0,0,0);put(o+0x254,depth,BASE+volume if volume else 0)
   put(o+0x54,*(deltas if deltas is not None else [0x3f800000,0xbf800000,0x3f000000]));put(o+0x68,*(velocities if velocities is not None else [0x40000000,0x3f000000,0xbf800000]));arena[o+0xec]=disabled[j]
  if header:put(header+4,flags);put(header+0x14,magnitude)
  if volume:put(volume+0x88,fluid,viscosity)
  put(0xa000,0x3f800000,0xbf800000,0x3f000000)
  if tree==1:put(objects[0]+0x224,BASE+objects[1])
  if tree==2:put(objects[0]+0x224,BASE+objects[1]);put(objects[1]+0x220,BASE+objects[2])
  if tree==3:put(objects[0]+0x224,BASE+objects[1]);put(objects[1]+0x224,BASE+objects[2]);put(objects[1]+0x220,BASE+objects[3])
  if tree==4:put(objects[0]+0x224,BASE+objects[3])
  put(GLOBAL,multiplier)
  for at,v in patches:put(at,v)
  x=header if mode==0 else object;y=object if mode==0 else vector
  case=dict(mode=mode,tree=tree,flags=flags,volume=volume,header=header,object=object,vector=vector,gravity=gravity,multiplier=multiplier,magnitude=magnitude,fluid=fluid,viscosity=viscosity,depth=depth,disabled=disabled,deltas=deltas,velocities=velocities,patches=patches,label=label,seed=seed)
  cases.append((case,words([mode,x,y,gravity,(0,0xcccccccc,0x7f800001)[seed%3]])+arena))
 for mode in range(3):
  for tree in range(5):
   for disabled in [(0,0,0,0),(1,1,1,1),(0,1,255,0),(1,0,0,1)]:
    for flags in [0,8,0xffff]:add(mode,tree,flags,disabled=disabled)
 for field in ['gravity','multiplier','magnitude','fluid','viscosity','depth']:
  for bits in special:
   for flags,volume in [(0,0x9000),(8,0),(8,0x9000)]:add(**{field:bits},flags=flags,volume=volume,tree=3,label='singleIEEE-'+field)
 for magnitude in [0,0x80000000,0xbf800000,0x3f800000,0x7f800001,0x7fc12345]:
  for viscosity in [0x3fffffff,0x40000000,0x40000001,0x7f800001,0x7fc12345]:
   for fluid in [0,0x3f800000,0x3f800001,0x7f800001]:add(magnitude=magnitude,viscosity=viscosity,fluid=fluid,tree=2,label='ordered-branch-boundaries')
 for a,b in [(0x7f812345,0xff812345),(0x7fc12345,0xffc12345),(0x7f812345,0xffc12345),(0xff812345,0x7fc12345)]:
  for left,right in [('gravity','multiplier'),('fluid','depth'),('depth','magnitude'),('magnitude','viscosity')]:add(**{left:a,right:b},tree=3,label='pairedIEEE-'+left+'-'+right)
 for mode in [0,2]:
  for word in range(3):
   for a in special:
    deltas=[0,0,0];velocities=[0,0,0];deltas[word]=a;velocities[word]=a^0x80000000
    add(mode,tree=3,deltas=deltas,velocities=velocities,label='delta-velocityIEEE')
 for mode in [1,2]:
  for a,b in [(0x7f812345,0xff812345),(0x7fc12345,0xffc12345),(0x7f812345,0xffc12345),(0xff812345,0x7fc12345)]:
   for word in range(3):
    d=[0,0,0];v=[0,0,0];d[word]=a;v[word]=b;add(mode,tree=2,deltas=d,velocities=v,label='paired-delta-velocity')
 for delta in range(-15,16):
  for tree in [0,3]:add(1,tree=tree,vector=0x154+delta,label='forward-DWORD-partial-source')
 for vector in [0x1054,0x1068,0x1220,0x1224,GLOBAL,0x9000+0x88]:add(1,tree=3,vector=vector,label='source-child-link-global-volume')
 for header in [0x154,0x168,0x248,GLOBAL-0x14]:add(0,tree=3,header=header,label='header-object-global-alias')
 for volume in [0x100-0x88,0x154-0x88,0x168-0x88,GLOBAL-0x88]:add(0,tree=3,volume=volume,label='volume-object-global-alias')
 # Genuine overlapping nodes: rootcopy writes a child's next before recursively processing it.
 add(1,object=0x2000,patches=((0x2224,BASE+0x1e34),(0x2054,0),(0x2058,0),(0x205c,0),(0x2054+0x1000,0),(0x1e34+0x224,0),(0xa000,BASE+0x3000)),label='rawcopy-live-next-overlap')
 # A child's delta updates physically overwrite a preceding node's live sibling pointer.
 add(2,object=0x2000,patches=((0x2224,BASE+0x2100),(0x2324,BASE+0x22b8),(0x2320,0),(0x22b8+0x220,0),(0x22b8+0x224,0),(0x22b8+0xec,0),(0x22b8+0x54,0),(0x2320,BASE+0x3000)),label='increment-live-next-overlap')
 add(0,header=0,label='gravity-authentic-null-header');add(0,object=0,flags=0,label='gravity-authentic-null-object-without-volume');add(0,object=0,flags=8,label='gravity-authentic-null-object-volume')
 add(1,object=0,label='copy-authentic-null-object');add(1,vector=0,label='copy-authentic-null-vector');add(2,object=0,label='increment-authentic-null-object')
 add(1,tree=1,patches=((0x1220,3),),label='copy-authentic-invalid-next');add(2,tree=1,patches=((0x1220,3),),label='increment-authentic-invalid-next')
 # Additional cross-branch paired payloads distinguish register/register subtract
 # from memory float multiplies and comparisons without relaxing earlier cases.
 fields=['gravity','multiplier','magnitude','fluid','viscosity','depth']
 pairs=[(0x7f812345,0xff812345),(0x7fc12345,0xffc12345),(0x7f812345,0xffc12345),(0xff812345,0x7fc12345),(0x7f812345,0xff854321),(0xff854321,0x7f812345)]
 for i,left in enumerate(fields):
  for right in fields[i+1:]:
   for a,b in pairs:
    for branch in [0,1,2]:
     settings=dict(flags=8,volume=0x9000,viscosity=0x40400000)
     if branch==1:settings['viscosity']=0x40000000
     if branch==2:settings['flags']=0
     settings.update({left:a,right:b});add(**settings,tree=3,label='cross-branch-pairedIEEE-'+left+'-'+right)
 for seed in range(160):
  r=random.Random(0x374000+seed)
  values={name:(r.getrandbits(32)&0xff7fffff)for name in fields}
  add(**values,tree=seed%5,flags=[0,8][seed%2],label='finite-exponent-rounding-cancellation')
 add(1,object=0x2000,patches=((0x2224,BASE+0x1e34),(0x2054,0),(0x2058,0),(0x205c,0),(0x1e34+0x224,0),(0xa000,BASE+0x3000),(0xa004,0),(0xa008,0)),label='rawcopy-valid-live-next-overlap')
 return cases
"""Production two-owner extraction and fail-closed actual whole gravity CFG transport."""
OWNER_NAMES={
 '49-physics.c':['GravityWord','GravityStoreWord','GravityPointer','GravityRead','GravityRound','GravityAddScalar','GravityScalarPointer','FUN_004c29d0','FUN_004c2910'],
 '60-phil.c':['GravityWord','GravityRead','GravityRound','GravityNaN','GravityMultiplyScalar','PhysicsJointSum','GravitySubtractPopWide','GravityScalarPointer','ProcessGravity'],
}
def production_capsule(source=None):
 if source is not None:return source.read_text()
 bodies={};order=[]
 for owner,names in OWNER_NAMES.items():
  text=(ROOT/'src/carma2/newcommon'/owner).read_text()
  for name in names:
   body=extract(text,name)
   if name in bodies:assert bodies[name]==body,('Cross-owner helper differs',name)
   else:bodies[name]=body;order.append(name)
 # Define all helper dependencies before the three entry roots. Children precede parent.
 roots={'ProcessGravity','FUN_004c29d0','FUN_004c2910'}
 return '\n'.join(bodies[name]for name in order if name not in roots)+'\n'+'\n'.join(bodies[name]for name in ['FUN_004c29d0','FUN_004c2910','ProcessGravity'])+'\n'

def matching_image(exe,symbols_path):
 b=exe.read_bytes();mapping=json.loads(symbols_path.read_text());assert mapping['format']==1 and mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(b)
 mapped={r['orig']:r['recomp']for r in mapping['symbols']};roots=[mapped[a]for name,a,n in ENTRIES]
 map_bytes=exe.with_suffix('.map').read_bytes();functions={};global_rows=[]
 for name,address,owner in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',map_bytes.decode('latin1'),re.M):functions.setdefault(int(address,16),[]).append((name,owner.strip()))
 for name,address,owner in re.findall(r'^\s*0003:[0-9a-fA-F]{8}\s+(_gGravity_multiplier)\s+([0-9a-fA-F]{8})\s+(.+)$',map_bytes.decode('latin1'),re.M):global_rows.append((int(address,16),owner.strip()))
 assert len(global_rows)==1 and global_rows[0][1]in('<common>','globvars.c.obj')
 global_address=global_rows[0][0];assert mapped[0x74d1a8]==global_address
 public={mapped[a]:(name,'60-phil.c.obj'if name=='ProcessGravity'else'49-physics.c.obj')for name,a,n in ENTRIES};assert set(public)<=set(functions)
 nt,base,size,tab=pe(b);regions=[]
 for k in range(struct.unpack_from('<H',b,nt+6)[0]):
  vs,rva,n,off=struct.unpack_from('<IIII',b,tab+k*40+8);flags=struct.unpack_from('<I',b,tab+k*40+36)[0];regions.append((base+rva,max(vs,n),flags))
 global_region=next(r for r in regions if r[0]<=global_address<r[0]+r[1]);assert global_region[2]&0x80000000
 allowed={}
 for owner,names in OWNER_NAMES.items():
  for name in names:allowed.setdefault(name,set()).add(owner+'.obj')
 allowed['PhysicsJointSum'].add('49-physics.c.obj') # Existing identical helper may be folded with its copied60 body.
 cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;todo=list(roots);seen=set();instructions={};constants={};refs=set();global_refs=set();graph=[];edges=[]
 while todo:
  start=todo.pop()
  if start in seen:continue
  seen.add(start);aliases=functions[start]
  if start in public:
   name,owner=public[start];assert any(n.lstrip('_@').split('@')[0]==name and o==owner for n,o in aliases)
  else:
   accepted=[(n,o)for n,o in aliases if n.lstrip('_@').split('@')[0]in allowed and o in allowed[n.lstrip('_@').split('@')[0]]]
   names={n.lstrip('_@').split('@')[0]for n,o in accepted};assert len(names)==1,(hex(start),aliases);name=next(iter(names))
  end=min(v for v in functions if v>start);pending=[start];local={};branch_targets=set()
  while pending:
   at=pending.pop()
   while at not in local:
    assert start<=at<end,(name,hex(at));i=next(cs.disasm(raw(b,at,15),at));assert at+i.size<=end
    assert not any(at<prior<at+i.size or prior<at<prior+len(code)for prior,code in local.items()),('Overlapping decoded instruction',name,hex(at))
    local[at]=bytes(i.bytes);instructions[at]=bytes(i.bytes)
    if CS_GRP_CALL in i.groups:assert i.mnemonic=='call',('Unexpected call-group form',name,i.mnemonic)
    if i.mnemonic=='call'or CS_GRP_JUMP in i.groups:
     assert i.operands[0].type==CS_OP_IMM,('Unexpected indirect control',name,hex(at),i.op_str);target=i.operands[0].imm
     if start<=target<end:
      branch_targets.add(target)
      if i.mnemonic=='call':assert target==start and start in {mapped[0x4c29d0],mapped[0x4c2910]}
      else:pending.append(target)
     else:assert target in functions,(name,i.op_str);todo.append(target);edges.append(dict(site=at,target=target,kind=i.mnemonic))
    for op in i.operands:
     if op.type==CS_OP_MEM and base<=op.mem.disp<base+size:
      assert not op.mem.base and not op.mem.index and i.disp_size==4
      if op.mem.disp==global_address:assert op.size==4 and i.mnemonic in('fld','fmul','mov');global_refs.add(at+i.disp_offset)
      else:
       assert i.mnemonic.startswith('f')and op.size in(4,8),('Unexpected static data',name,i.op_str)
       region=next(r for r in regions if r[0]<=op.mem.disp<r[0]+r[1]);assert not region[2]&0xa0000000
       constants[(op.mem.disp,op.size)]=raw(b,op.mem.disp,op.size);refs.add(at+i.disp_offset)
     elif op.type==CS_OP_IMM and i.mnemonic in('mov','push')and base<=op.imm<base+size:
      assert op.imm==global_address and i.imm_size==4,('Unexpected static pointer argument',name,i.op_str);global_refs.add(at+i.imm_offset)
    if i.mnemonic.startswith('ret'):
     pop=i.operands[0].imm if i.operands else 0;assert pop==(4 if name=='ProcessGravity'else 0),(name,i.op_str);break
    if i.mnemonic in('jmp','int3','int'):break
    at+=i.size
  assert branch_targets<=set(local)
  graph.append(dict(name=name,address=start,end=end,extent=end-start,map_aliases=aliases,instructions=[[a,v.hex()]for a,v in sorted(local.items())],instruction_count=len(local),instruction_sha256=sha(b''.join(v for a,v in sorted(local.items())))))
 active={a+k for a,v in instructions.items()for k in range(len(v))};rr,rz=struct.unpack_from('<II',b,nt+24+96+5*8);cursor=base+rr;relocations=[]
 while cursor<base+rr+rz:
  page,n=struct.unpack('<II',raw(b,cursor,8));assert n>=8
  for w in struct.unpack('<'+'H'*((n-8)//2),raw(b,cursor+8,n-8)):
   if w>>12==3 and base+page+(w&4095)in active:
    site=base+page+(w&4095);assert site in refs|global_refs,('Unclassified active absolute relocation',hex(site));relocations.append(site)
  cursor+=n
 low=min([*instructions,*[a for a,n in constants]]);high=max([a+len(v)for a,v in instructions.items()]+[a+n for a,n in constants]);image=bytearray(b'\xcc'*(high-low))
 for a,v in instructions.items():image[a-low:a-low+len(v)]=v
 for(a,n),v in constants.items():assert all(a+k not in active for k in range(n));image[a-low:a-low+n]=v
 assert global_refs
 for site in global_refs:assert struct.unpack('<I',image[site-low:site-low+4])[0]==global_address;struct.pack_into('<I',image,site-low,BASE+GLOBAL)
 refs=sorted(refs);blob=words([len(image),roots[0]-low,roots[1]-low,roots[2]-low,len(refs),low])+image+words([a-low for a in refs])
 return blob,dict(recompiled_sha256=sha(b),symbols_sha256=sha(symbols_path.read_bytes()),map_sha256=sha(map_bytes),graph=graph,edges=edges,roots=roots,low=low,high=high,absolute_relocations=refs,active_relocations_checked=relocations,canonical_mutable_global=dict(original=0x74d1a8,name='gGravity_multiplier',actual=global_address,map_owner=global_rows[0][1],arena_address=BASE+GLOBAL,operand_sites=sorted(global_refs)),constants=[dict(address=a,bytes=n,hex=v.hex())for(a,n),v in sorted(constants.items())],unexpected_control_sites=[],normalization=[],unresolved_helpers=[])


def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',required=True,type=Path);p.add_argument('--candidate-source',type=Path);p.add_argument('--limit',type=int);p.add_argument('--case-index',type=int,action='append');p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);a=p.parse_args();assert bool(a.recompiled)==bool(a.symbols)
 out=a.output.resolve();assert not out.exists(),'Use a fresh uniquely owned output';out.mkdir(parents=True)
 source=a.candidate_source.resolve()if a.candidate_source else None;physics=ROOT/'src/carma2/newcommon/49-physics.c';phil=ROOT/'src/carma2/newcommon/60-phil.c';matrix=ROOT/'src/brender/core/math/matrix34.c';script=Path(__file__).resolve()
 paths=[physics,phil,matrix,ROOT/'src/carma2/newcommon/globvars.c',script,ROOT/'scripts/verify_phil_registry_reconstruction.py',ROOT/'scripts/verify_opponent_route_reconstruction.py',ROOT/'scripts/verify_opponent_navigation_reconstruction.py',ROOT/'scripts/verify_tinted_native_reconstruction.py']+list((ROOT/'src').rglob('*.h'))
 if source:paths.append(source)
 if a.recompiled:paths.extend([a.recompiled,a.symbols,a.recompiled.with_suffix('.map')])
 before={str(f):sha(f.read_bytes())for f in paths};(out/'source-before.json').write_text(json.dumps(before,indent=2)+'\n');(out/'candidate-snapshot.c').write_text(production_capsule(source));(out/'verifier-snapshot.py').write_bytes(script.read_bytes())
 binary=(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes();assert sha(binary)==HASH;image,graph=original_image(binary);(out/'original-code.bin').write_bytes(image)
 linked=None
 if a.recompiled:
  linked_blob,linked=matching_image(a.recompiled,a.symbols);(out/'matching-code.bin').write_bytes(linked_blob)
 native=COMMON+'\n'+production_capsule(source)+'\nint C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;addresses[0]=(unsigned)ProcessGravity;addresses[1]=(unsigned)FUN_004c29d0;addresses[2]=(unsigned)FUN_004c2910;return run(argv[1]);}\n'
 caller,_,_=registry.literals();caller=caller.replace('builder_poison;','builder_poison,builder_cdecl;');old=' mov builder_result,eax\n mov ecx,esp';assert old in caller;caller=caller.replace(old,' xor eax,eax\n mov builder_result,eax\n cmp builder_cdecl,0\n je gravity_stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n gravity_stackready:\n mov ecx,esp')
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
 report['production_default']=source is None
 (out/'verification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS'if not failures else'FAIL',len(indices),len(results),len(failures),flush=True);assert not failures,'Failures retained'
if __name__=='__main__':main()
