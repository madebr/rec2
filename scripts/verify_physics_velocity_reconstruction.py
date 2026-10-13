"""Physical original/native/linked proof of the complete velocity-vector closure.

New entries: RestoreListOmega4c03f0, DrMatrix34ApplyLPInverse4225b0,
MakeVelocitiesVector2 4c0430. Reuse the published MakeJointVelocitiesVector
4b77b0 and genuine matrix34 ApplyP/ApplyV/TApplyV bodies. No fake helpers or
inlined RestoreGroupOmegas marker. Input16DWORD header +65536-byte arena;
output exact consumerEAX0/1 (voidEAX excluded) +complete unmasked raw arena.
Original instructions execute physically atPC53; only absolute constants are
relocated. Optional matching image executes its actual closed call graph,
including private helper bodies, physically with hash-bound symbol/MAP inputs.
Every outer call checks ESP, EBX/ESI/EDI/EBP and x87 TOP. Earlier scratch ABI
runs used the bridge's void mode for the consumer and overstated EAX coverage;
this retained verifier selects real-return mode0 for the consumer.
"""
import ast,argparse,hashlib,json,os,random,re,struct,subprocess,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parent
while not (ROOT/'src').is_dir():ROOT=ROOT.parent
sys.path.insert(0,str(ROOT/'build-audit/joystick/python-deps'))
sys.path.insert(0,str(ROOT/'scripts'))
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_MEM,CS_OP_IMM
from verify_tinted_native_reconstruction import extract
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',default='build-audit/physics-velocity-tests');p.add_argument('--candidate',type=Path);p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);p.add_argument('--case-limit',type=int);p.add_argument('--case-start',type=int,default=0);p.add_argument('--max-failures',type=int,default=20);args=p.parse_args()
OUT=ROOT/args.output;OUT.mkdir(parents=True,exist_ok=True)
assert bool(args.recompiled)==bool(args.symbols),'Supply both immutable PE and symbol map'
leaves=ROOT/'src/carma2/newcommon/49-physics.c'
source=args.candidate.resolve() if args.candidate else leaves
candidate_names=['VelocityField','VelocityRead','VelocityPointer','VelocityScalar','VelocityBelowOrUnordered','VelocityGreater','VelocitySubtractScalar','RestoreListOmega','DrMatrix34ApplyLPInverse','MakeVelocitiesVector2']
leaf_names=['PhysicsJointScalar','PhysicsJointSum','PhysicsJointProduct','PhysicsJointProductMemory','PhysicsJointAddScalar','PhysicsJointVelocityA','PhysicsJointAddVelocity','PhysicsJointVelocityB','MakeJointVelocitiesVector']
def extract_candidate(text):
 constants=re.findall(r'static const (?:br_scalar|double) VelocityConstant[0-9a-f]+ = [^;]+;',text)
 assert len(constants)==11
 return '\n'.join(constants+[extract(text,n) for n in candidate_names])

source_paths=[leaves,leaves.with_suffix('.h'),ROOT/'src/brender/core/math/matrix34.c',ROOT/'src/carma2/include/rec2_types.h',ROOT/'src/carma2/hooks/c2_hooks.h',Path(__file__),ROOT/'scripts/verify_opponent_navigation_reconstruction.py',ROOT/'scripts/verify_tinted_native_reconstruction.py']
if args.candidate:source_paths.append(source)
source_before={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in source_paths}
leaf_file_text=leaves.read_text()
original=(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes();original_hash=hashlib.sha256(original).hexdigest();assert original_hash=='9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
nt=struct.unpack_from('<I',original,60)[0];table=nt+24+struct.unpack_from('<H',original,nt+20)[0];sz=struct.unpack_from('<I',original,nt+80)[0];image=bytearray(sz)
for i in range(struct.unpack_from('<H',original,nt+6)[0]):
 _,rva,n,off=struct.unpack_from('<IIII',original,table+40*i+8);image[rva:rva+n]=original[off:off+n]
spec=[('RestoreListOmega',0x4c03f0,61),('MakeVelocitiesVector2',0x4c0430,1675),('DrMatrix34ApplyLPInverse',0x4225b0,57),('MakeJointVelocitiesVector',0x4b77b0,425),('BrMatrix34ApplyP',0x5334a0,116),('BrMatrix34ApplyV',0x533520,107),('BrMatrix34TApplyV',0x533690,107)]
cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;patches=[];entries=[]
for name,address,n in spec:
 body=bytes(image[address-0x400000:address-0x400000+n]);instructions=list(cs.disasm(body,address));assert sum(i.size for i in instructions)==n
 entries.append(dict(name=name,address=address,bytes=n,sha256=hashlib.sha256(body).hexdigest()))
 for ins in instructions:
  if ins.mnemonic=='call':assert ins.operands[0].type==CS_OP_IMM and ins.operands[0].imm in {a for _,a,_ in spec}
  if ins.mnemonic.startswith('j'):assert address<=ins.operands[0].imm<address+n
  for op in ins.operands:
   if op.type==CS_OP_MEM and 0x400000<=op.mem.disp<0x400000+sz:
    assert not op.mem.base and not op.mem.index and ins.disp_size==4
    assert 0x580000<=op.mem.disp<0x600000 and op.size in [4,8]
    at=ins.address-0x400000+ins.disp_offset;struct.pack_into('<I',image,at,op.mem.disp-0x400000+0x50000000);patches.append(dict(site=ins.address+ins.disp_offset,original=op.mem.disp,mapped=op.mem.disp-0x400000+0x50000000))
(OUT/'physical-image.bin').write_bytes(image)
math_source=(ROOT/'src/brender/core/math/matrix34.c').read_text();vector_source=(ROOT/'src/brender/core/math/vector.c').read_text()
math_bodies='\n'.join(extract(math_source,n) for n in ['Matrix34ApplyProductLoaded','Matrix34ApplyProductMemory','Matrix34ApplyAdd','BrMatrix34ApplyP','BrMatrix34ApplyV','BrMatrix34TApplyV'])
prefix=r'''
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "rec2_types.h"
#include "49-physics.h"
#include "c2_hooks.h"
#define M(row,col) mat->m[row][col]
static unsigned char*code,*arena;
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[6],builder_stack_count,builder_result,builder_poison,builder_cdecl;
extern unsigned __cdecl BuilderInvoke(void);
'''
candidate_file_text=source.read_text();candidate_text=extract_candidate(candidate_file_text);leaf_text='\n'.join(extract(leaf_file_text,name) for name in leaf_names)
(OUT/'candidate.c').write_text(candidate_text);(OUT/'leaf-candidate.c').write_text(leaf_text);(OUT/'verifier.py').write_bytes(Path(__file__).read_bytes())
native=math_bodies+'\n'+leaf_text+'\n'+candidate_text

driver=prefix+'\n#ifndef PHYSICAL_ORIGINAL\n'+native+'\n#endif\n'+r'''
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned h[16],result=0,values[4],n=0,i;unsigned entry_offsets[4]={0xc03f0,0x225b0,0xb77b0,0xc0430};if(argc!=3)return 2;
 SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);
 code=VirtualAlloc((void*)0x50000000,IMAGE_SIZE,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);arena=VirtualAlloc((void*)0x20000000,65536,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(code!=(void*)0x50000000||arena!=(void*)0x20000000)return 3;
 f=fopen(argv[2],"rb");if(!f)return 4;if(fread(code,IMAGE_SIZE,1,f)!=1)return 5;fclose(f);f=fopen(argv[1],"rb");if(!f)return 6;if(fread(h,sizeof(h),1,f)!=1||fread(arena,65536,1,f)!=1)return 7;fclose(f);
 _controlfp(_PC_53,_MCW_PC);_setmode(_fileno(stdout),_O_BINARY);
 values[0]=(unsigned)(uintptr_t)(arena+h[1]);values[1]=(unsigned)(uintptr_t)(arena+h[2]);
 if(h[0]==0){values[0]=(unsigned)(uintptr_t)(arena+h[6]);values[1]=0;n=1;}
 else if(h[0]==1){values[1]=(unsigned)(uintptr_t)(arena+h[5]);values[2]=(unsigned)(uintptr_t)(arena+h[8]);n=3;}
 else if(h[0]==2){values[2]=h[10];n=3;}
 else if(h[0]==3){values[2]=h[7];values[3]=h[10];n=4;}
 else return 8;
#ifdef PHYSICAL_ORIGINAL
 builder_address=(unsigned)(uintptr_t)(code+entry_offsets[h[0]]);
#else
 (void)entry_offsets;
 if(h[0]==0)builder_address=(unsigned)(uintptr_t)RestoreListOmega;
 else if(h[0]==1)builder_address=(unsigned)(uintptr_t)DrMatrix34ApplyLPInverse;
 else if(h[0]==2)builder_address=(unsigned)(uintptr_t)MakeJointVelocitiesVector;
 else builder_address=(unsigned)(uintptr_t)MakeVelocitiesVector2;
#endif
 builder_mode=h[0]==3?0:2;builder_x=values[0];builder_y=values[1];builder_poison=0xcccccccc;
#if defined(REC2_MATCHING) || defined(PHYSICAL_ORIGINAL)
 builder_cdecl=0;builder_stack_count=n>2?n-2:0;for(i=2;i<n;i++)builder_stack[i-2]=values[i];
#else
 builder_cdecl=1;builder_stack_count=n;for(i=0;i<n;i++)builder_stack[i]=values[i];
#endif
 if(BuilderInvoke()){return 14;}if(h[0]==3)result=builder_result;
 fwrite(&result,4,1,stdout);fwrite(arena,65536,1,stdout);return 0;}
'''
driver=driver.replace('IMAGE_SIZE',str(sz))
(OUT/'harness.c').write_text(driver);(OUT/'original.c').write_text('#define PHYSICAL_ORIGINAL\n'+driver)
linked_meta=None
if args.recompiled:
 data=args.recompiled.read_bytes();mapping=json.loads(args.symbols.read_text());assert mapping['original_sha256']==original_hash and mapping['recompiled_sha256']==hashlib.sha256(data).hexdigest()
 symbols={x['orig']:x['recomp'] for x in mapping['symbols']};map_path=args.recompiled.with_suffix('.map');map_bytes=map_path.read_bytes()
 functions={int(address,16):(name,owner.strip()) for name,address,owner in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',map_bytes.decode('latin1'),re.M)}
 nt2=struct.unpack_from('<I',data,60)[0];base2=struct.unpack_from('<I',data,nt2+52)[0];size2=struct.unpack_from('<I',data,nt2+80)[0];table2=nt2+24+struct.unpack_from('<H',data,nt2+20)[0]
 flat=bytearray(size2);sections=[]
 for si in range(struct.unpack_from('<H',data,nt2+6)[0]):
  virtual,rva,n,offset=struct.unpack_from('<IIII',data,table2+40*si+8);flags=struct.unpack_from('<I',data,table2+40*si+36)[0];flat[rva:rva+n]=data[offset:offset+n];sections.append((base2+rva,max(virtual,n),flags))
 initial={symbols[address]:name for name,address,_ in spec};todo=list(initial);seen=set();linked_closure=[];patched=set();relocations=[]
 while todo:
  start=todo.pop(0)
  if start in seen:continue
  seen.add(start);end=min(address for address in functions if address>start);body=bytes(flat[start-base2:end-base2]);instructions=list(cs.disasm(body,start));assert sum(i.size for i in instructions)==len(body)
  name=initial.get(start,functions[start][0])
  for ins in instructions:
   if ins.mnemonic=='call' or ins.mnemonic.startswith('j'):
    assert ins.operands[0].type==CS_OP_IMM,(name,ins.op_str)
    target=ins.operands[0].imm
    if not start<=target<end:
     assert target in functions,(name,ins.op_str)
     if target not in initial:assert functions[target][1]=='49-physics.c.obj' and functions[target][0].lstrip('_@').startswith(('Force','Partner')),(name,functions[target])
     todo.append(target)
   for operand in ins.operands:
    if operand.type==CS_OP_MEM and base2<=operand.mem.disp<base2+size2:
     assert not operand.mem.base and not operand.mem.index and ins.disp_size==4,(name,ins.op_str)
     section=next(sec for sec in sections if sec[0]<=operand.mem.disp<sec[0]+sec[1]);assert not(section[2]&0x80000020),(name,ins.op_str)
     site=ins.address-base2+ins.disp_offset
     if site not in patched:
      relocations.append(dict(site=site,original=operand.mem.disp,width=operand.size,bits=bytes(flat[operand.mem.disp-base2:operand.mem.disp-base2+operand.size]).hex()));struct.pack_into('<I',flat,site,operand.mem.disp-base2+0x50000000);patched.add(site)
  linked_closure.append(dict(name=name,address=start,length=end-start,sha256=hashlib.sha256(body).hexdigest()))
 # Reject any unreviewed address-bearing immediate or other relocation in closure.
 rel_rva,rel_size=struct.unpack_from('<II',data,nt2+24+96+5*8);cursor=rel_rva
 while cursor<rel_rva+rel_size:
  page,n=struct.unpack_from('<II',flat,cursor);assert n>=8
  for word in struct.unpack_from('<'+'H'*((n-8)//2),flat,cursor+8):
   if word>>12==3:
    site=page+(word&4095)
    if any(row['address']-base2<=site<row['address']-base2+row['length'] for row in linked_closure):assert site in patched,hex(site)
  cursor+=n
 (OUT/'matching-image.bin').write_bytes(flat)
 matching_driver=driver.replace(str(sz),str(size2)).replace('{0xc03f0,0x225b0,0xb77b0,0xc0430}','{'+','.join(hex(symbols[address]-base2) for address in [0x4c03f0,0x4225b0,0x4b77b0,0x4c0430])+'}')
 (OUT/'matching.c').write_text('#define PHYSICAL_ORIGINAL\n'+matching_driver)
 linked_meta=dict(recompiled_sha256=hashlib.sha256(data).hexdigest(),symbols_sha256=hashlib.sha256(args.symbols.read_bytes()).hexdigest(),native_map_sha256=hashlib.sha256(map_bytes).hexdigest(),closure=linked_closure,constant_relocations=relocations)
compile_source=(ROOT/'build-audit/force-matrix-next/verify-leaves.py').read_text().replace("'src/carma2/include'","'src/carma2/newcommon','src/carma2/platform','src/carma2/include'")
config=compile_source[compile_source.index('includes='):compile_source.index('for name,cmd in commands.items():')];exec(config)
caller_tree=ast.parse((ROOT/'scripts/verify_opponent_navigation_reconstruction.py').read_text())
caller_nodes=[node.value.value for node in ast.walk(caller_tree) if isinstance(node,ast.Assign) and isinstance(node.value,ast.Constant) and any(isinstance(target,ast.Name) and target.id=='caller' for target in node.targets)]
assert len(caller_nodes)==1
caller=caller_nodes[0].replace('builder_stack[5]','builder_stack[6]').replace('builder_poison;','builder_poison,builder_cdecl;')
caller=caller.replace(' mov builder_result,eax\n mov ecx,esp',' mov builder_result,eax\n cmp builder_cdecl,0\n je stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n stackready:\n mov ecx,esp')
(OUT/'caller.c').write_text(caller)
if args.recompiled:commands['matching']=[*commands['original'][:-3],str(OUT/'matching.c'),'-o',str(OUT/'matching.exe')]
for command in commands.values():command.append(str(OUT/'caller.obj'))
commands={'caller':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Zl','/c',str(OUT/'caller.c'),'/Fo'+str(OUT/'caller.obj')],**commands}
for name,cmd in commands.items():
 r=subprocess.run(cmd,cwd=OUT,env=env if name=='msvc'else env5 if name in ('msvc5','caller')else None,capture_output=True,text=True);(OUT/(name+'-build.txt')).write_text(r.stdout+r.stderr)
 if r.returncode:raise RuntimeError(r.stdout+r.stderr)
rng=random.Random(0x4c0430);cases=[]
def scene(mode,kind=0,count=1,capacity=3,second=True,seed=0,patch=None,alias=None,graph=0):
 h=[mode,0x8000,0x3000,0,0,0x5000,0x1000,capacity&0xffffffff,0x5100,0,count&0xffffffff]+[0]*5;arena=bytearray(b'\xa5'*65536)
 def words(at,*v):struct.pack_into('<'+'I'*len(v),arena,at,*[x&0xffffffff for x in v])
 def floats(at,*v):struct.pack_into('<'+'f'*len(v),arena,at,*v)
 for i,off in enumerate([0x1000,0x1800,0x2000]):
  arena[off:off+0x500]=bytes(0x500);words(off,0x20006000+0x100*i);floats(off+0x14,.2,-.1,.3);floats(off+0x74,.25,-.5,.75);floats(off+0x1a8,.1,-.2,.3);floats(off+0x1c0,1,2,-3);floats(off+0x8c,1,0,0,0,1,0,0,0,1,.1,.2,.3);floats(0x602c+0x100*i,1,0,0,0,1,0,0,0,1,.2,-.1,.4)
 if graph==1:words(0x1220,0x20001800);words(0x1224,0x20002000)
 elif graph==2:words(0x1224,0x20001800);words(0x1a24,0x20002000)
 for i in range(max(count,1)):
  at=0x4000+128*i;arena[at:at+96]=bytes(96);words(0x3000+4*i,0x20000000+at);floats(at,.02);words(at+4,0x20001000,0x20001800 if second else 0);floats(at+0x10,.03);floats(at+0x14,.5,-.4,.3,-.2,.1,-.7,1,0,0,-1,0,0);words(at+0x5c,kind)
 floats(0x5000,.25,-.5,.75);floats(0x5100,1,0,0,0,1,0,0,0,1,.1,.2,.3)
 if seed:
  for at in [0x1074,0x1874,0x10a8,0x18a8,0x11a8,0x19a8,0x4014,0x4020,0x402c,0x4038,0x5000]:floats(at,*[rng.uniform(-2,2) for _ in range(3)])
  for at in [0x108c,0x188c,0x602c,0x612c,0x5100]:floats(at,*[rng.uniform(-1,1) for _ in range(12)])
 for at,v in patch or []:words(at,v)
 for key,v in (alias or {}).items():h[key]=v
 cases.append(dict(mode=mode,kind=kind,count=count,capacity=capacity,second=second,seed=seed,patch=patch,alias=alias,graph=graph,header=h,data=bytes(arena)))
for mode in [0,1,2,3]:
 if mode==0:
  for graph in [0,1,2]:scene(mode,graph=graph)
  continue
 for kind in ([0] if mode==1 else [0,5,11,14]):
  for second in [False,True]:
   scene(mode,kind,second=second)
   for seed in range(1,13):scene(mode,kind,second=second,seed=seed)
   for at in ([0x5000,0x5100,0x5124] if mode==1 else [0x1074,0x11a8,0x1874,0x1078,0x402c,0x4014,0x4000,0x4010]):
    for word in [0,0x80000000,1,0x7f800000,0xff800000,0x7fc12345,0x7f812345,0xffc12345]:scene(mode,kind,second=second,patch=[(at,word)])
   for offset in ([0x5000,0x5004,0x5100,0x5104] if mode==1 else [0x4000,0x4014,0x402c,0x1074,0x108c]):scene(mode,kind,second=second,capacity=1 if mode==3 and offset==0x4000 else 3,alias={1:offset})
 for count,cap in [(-2,0),(0,0),(0,4),(1,0),(1,1),(2,4),(4,2)]:
  if mode in [2,3]:scene(mode,count=count,capacity=cap)
# Explicit phase-two, paired operands, thresholds, raw restore and live hierarchy cases.
zero_velocity=[(at,0) for base in [0x1000,0x1800] for off in [0x74,0x78,0x7c,0x1a8,0x1ac,0x1b0] for at in [base+off]]
for kind in [0,5,11,14]:
 for second in [False,True]:
  for seed in range(1,17):scene(3,kind,second=second,seed=seed,patch=zero_velocity)
  for word in [0x3a83126d,0x3a83126e,0x3a83126f,0x3a831270,0x3c23d709,0x3c23d70a,0x3c23d70b,0x3dcccccc,0x3dcccccd,0x3dccccce,0x7fc12345,0x7f812345]:
   scene(3,kind,second=second,patch=zero_velocity+[(0x4000,word)])
  for flags in [0,1,0x80,0xff]:scene(3,kind,second=second,patch=[(0x11df,flags),(0x19df,flags)])
  for pair in [(0x4014,0x107c),(0x401c,0x1078),(0x402c,0x1074),(0x4038,0x1874),(0x4014,0x1014),(0x4014,0x108c),(0x4020,0x188c),(0x4000,0x4014)]:
   for a,b in [(0x7f801234,0xff805678),(0xff801234,0x7fc05678),(0x7fc01234,0xff805678),(0x7fc01234,0xffc05678)]:scene(3,kind,second=second,patch=[(pair[0],a),(pair[1],b)])
for component in range(3):
 for a,b in [(0x7f801234,0xff805678),(0xff801234,0x7fc05678),(0x7fc01234,0xff805678),(0x7fc01234,0xffc05678)]:scene(1,patch=[(0x5000+4*component,a),(0x5124+4*component,b)])
for graph in [0,1,2]:
 for word in [0,0x80000000,0x7f801234,0xff805678,0x7fc12345]:scene(0,graph=graph,patch=[(0x11c0,word),(0x19c4,word),(0x21c8,word)])
scene(0,patch=[(0x1224,0x200011ac),(0x136c,0x20001800),(0x1370,0),(0x13cc,0),(0x13d0,0)])
# Valid output aliases include writes that change later numeric fields, type
# tags and object flags, but never erase an object pointer subsequently read.
for mode in [2,3]:
 for kind in [0,5,11,14]:
  for second in [False,True]:
   for offset in [0x4010,0x4014,0x4015,0x4020,0x402c,0x4038,0x405c,0x1075,0x1078,0x107c,0x11a8,0x11ac,0x11b0,0x11dc,0x19dc,0x1014,0x602c,0x6030]:scene(mode,kind,second=second,capacity=1,alias={1:offset})
for offset in [0x5001,0x5008,0x5101,0x5110,0x5124,0x5128]:scene(1,alias={1:offset})
# First torque result is exactly the address of a second valid record. The
# original reloads the pointer array before its optional-partner computation.
new_record=[(0x4100+4*i,0) for i in range(24)]+[(0x4104,0x20001000),(0x4108,0x20001800),(0x4140,0x3f800000),(0x415c,11)]
scene(3,11,second=False,capacity=1,alias={1:0x3000},patch=new_record+[(0x1074,0xa0004100),(0x1078,0),(0x107c,0)])
for kind in [0,5,14]:
 for second in [False,True]:
  for prefix in [.1000001,.33333334,.7,.95,1.,1.000001,1.0999999,1.1,1.1000001]:
   word=struct.unpack('<I',struct.pack('<f',prefix))[0]
   for seed in [2,7]:scene(3,kind,second=second,seed=seed,patch=[(0x4000,word)])
   scene(3,kind,second=second,seed=3,patch=zero_velocity+[(0x4000,word)])
for mode in [1,3]:
 for word in [0x00800000,0x007fffff,0x01000000,0x60000001,0x7f7fffff,0xff7fffff]:
  scene(mode,seed=3,patch=[(0x5000 if mode==1 else 0x1074,word),(0x5124 if mode==1 else 0x401c,0x3f000001)])
scene(3,11,second=False,patch=[(0x1074,0xbf800000),(0x1078,0xdb000000),(0x107c,0x5b000000),(0x402c,0x3f800000),(0x4030,0x3f800000),(0x4034,0x3f800000)])
scene(3,11,second=True,patch=[(0x1074,0),(0x1078,0),(0x107c,0),(0x1874,0x5b000000),(0x1878,0xdb000000),(0x187c,0x3f800000),(0x4038,0x3f800000),(0x403c,0x3f800000),(0x4040,0x3f800000)])
results=[];failures=[]
for index,c in enumerate(cases[:args.case_limit]):
 if index<args.case_start:continue
 f=OUT/'input.bin';f.write_bytes(struct.pack('<16I',*c['header'])+c['data']);oracle=subprocess.run([str(OUT/'original.exe'),str(f),str(OUT/'physical-image.bin')],capture_output=True);assert oracle.returncode==0 and len(oracle.stdout)==65540,(index,oracle.returncode)
 for compiler in ['msvc5','msvc','mingw']+(['matching'] if args.recompiled else []):
  actual=subprocess.run([str(OUT/(compiler+'.exe')),str(f),str(OUT/('matching-image.bin' if compiler=='matching' else 'physical-image.bin'))],capture_output=True);ok=actual.returncode==0 and actual.stdout==oracle.stdout
  row=dict(index=index,case={k:v for k,v in c.items() if k!='data'},compiler=compiler,passed=ok)
  if not ok:
   row.update(returncode=actual.returncode,differences=[dict(offset=i,original=oracle.stdout[i:i+4].hex(),actual=actual.stdout[i:i+4].hex()) for i in range(0,min(len(actual.stdout),65540),4) if oracle.stdout[i:i+4]!=actual.stdout[i:i+4]])
   (OUT/('failure-%d-%s.bin'%(index,compiler))).write_bytes(f.read_bytes());failures.append(row)
  results.append(row)
 if index%100==0:print('scenes',index+1,'failures',len(failures),flush=True)
 if len(failures)>=args.max_failures:break
source_after={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in source_paths}
assert leaf_text=='\n'.join(extract(leaves.read_text(),n) for n in leaf_names),'Verified lower49 bodies changed during proof'
assert candidate_text==extract_candidate(source.read_text()),'Verified candidate bodies changed during proof'
assert math_bodies=='\n'.join(extract((ROOT/'src/brender/core/math/matrix34.c').read_text(),n) for n in ['Matrix34ApplyProductLoaded','Matrix34ApplyProductMemory','Matrix34ApplyAdd','BrMatrix34ApplyP','BrMatrix34ApplyV','BrMatrix34TApplyV']),'Verified math bodies changed during proof'
v=dict(matching=linked_meta,generated_files_sha256={p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in OUT.iterdir() if p.suffix in ('.c','.obj','.exe')},fixture_sha256=hashlib.sha256(b''.join(struct.pack('<16I',*c['header'])+c['data'] for c in cases[args.case_start:args.case_limit])).hexdigest(),case_start=args.case_start,source_before=source_before,source_after=source_after,whole_sources_unchanged=source_before==source_after,verified_extractions_unchanged=True,source_sha256=hashlib.sha256(candidate_file_text.encode()).hexdigest(),candidate_sha256=hashlib.sha256(candidate_text.encode()).hexdigest(),leaf_source_sha256=hashlib.sha256(leaf_text.encode()).hexdigest(),original_sha256=original_hash,entries=entries,patches=patches,scenario_count=len(cases),executed_scenarios=len(results)//(4 if args.recompiled else 3),comparisons=len(results),results=results,failures=failures,compiler_commands=commands,scope='Complete structured C proof of3 missing velocity closure entries plus actual published MakeJointVelocitiesVector. Original full instructions and matrix dependencies execute physically atPC53; strict EAX0/1 result and65536byte arena. No masks/normalization; direct caller checks ESP, EBX/ESI/EDI/EBP and x87 TOP for every scene. Void EAX is excluded; consumer EAX is exact0/1.')
(OUT/'verification.json').write_text(json.dumps(v,indent=2)+'\n');print('complete',index+1,len(results),'failures',len(failures))
