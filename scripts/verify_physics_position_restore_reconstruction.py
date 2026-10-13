"""Complete original3body restore graph vs complete C-first LPNormalise draft, PC53.

No numeric mocks or altered game dependencies. This is a scratch proof and
must retain all failures before any attempted lower schedule correction.
"""
import argparse,hashlib,json,random,re,struct,subprocess,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'scripts'));sys.path.insert(0,str(ROOT/'build-audit/joystick/python-deps'))
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_MEM,CS_OP_IMM
from verify_opponent_route_reconstruction import raw,HASH,sha,words
from verify_tinted_native_reconstruction import extract
import verify_physics_object_matrix_reconstruction as parent
import verify_phil_registry_reconstruction as registry
OUT=ROOT/'build-audit/matrix-lpnormalise-next'
SOURCE=ROOT/'src/carma2/newcommon/49-physics.c'
LP=ROOT/'src/brender/core/math/matrix34.c'
MATRIX=ROOT/'src/brender/core/math/matrix34.c'
VECTOR=ROOT/'src/brender/core/math/vector.c'
ENTRIES=[(0x4c25a0,82),(0x532f60,411),(0x5325d0,79)]
BASE=0x22000000
COMMON=parent.COMMON.replace('#define A(x,y)','#include "c2_math.h"\n#include "brender/br_defs.h"\n#define A(x,y)')
COMMON=COMMON.replace('builder_x=0x22000000+h[0];builder_y=0x22000000+h[1];','builder_x=0x22000000+h[0];builder_y=h[1];')
COMMON=COMMON.replace('builder_stack_count=2;builder_cdecl=1;builder_stack[0]=builder_x;builder_stack[1]=builder_y;','builder_stack_count=1;builder_cdecl=1;builder_stack[0]=builder_x;')
COMMON+='\n#undef BrVector3Normalise\n#undef BrVector3Cross\n'
COMMON+='\ntypedef char restore_layout_assert[offsetof(tPhysics_object,field_0xbc)==0xbc && offsetof(tPhysics_object,field_0xf0)==0xf0 && offsetof(tPhysics_object,next)==0x220 && offsetof(tPhysics_object,child)==0x224 && sizeof(br_matrix34)==48 ? 1 : -1];\n'
PHYSICAL=r'''
int C2_HOOK_CDECL main(int argc,char**argv){
 FILE*f;unsigned h[4],i,offset,value;unsigned char*image;
 if(argc!=3)return 1;f=fopen(argv[2],"rb");if(!f)return 2;
 if(fread(h,sizeof(h),1,f)!=1||!h[0]||h[1]>=h[0])return 3;
 image=(unsigned char*)VirtualAlloc(NULL,h[0],MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image)return 4;
 if(fread(image,h[0],1,f)!=1)return 5;
 for(i=0;i<h[2];i++){
  if(fread(&offset,4,1,f)!=1||offset>h[0]-4)return 6;
  memcpy(&value,image+offset,4);value=(unsigned)image+value-h[3];memcpy(image+offset,&value,4);
 }
 fclose(f);FlushInstructionCache(GetCurrentProcess(),image,h[0]);builder_address=(unsigned)(image+h[1]);return run(argv[1]);
}
'''
def original_graph(binary):
 cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
 low=min(a for a,n in ENTRIES);high=0x58b8d0
 image=bytearray(b'\xcc'*(high-low));refs=[];graphs={}
 for a,n in ENTRIES:
  b=raw(binary,a,n);ins=list(cs.disasm(b,a));assert sum(i.size for i in ins)==n
  calls=[]
  for i in ins:
   if i.mnemonic=='call':
    assert i.operands[0].type==CS_OP_IMM and i.operands[0].imm in [a for a,n in ENTRIES]
    calls.append(dict(site=hex(i.address),target=hex(i.operands[0].imm)))
   if i.mnemonic.startswith('j'):
    assert i.operands[0].type==CS_OP_IMM and a<=i.operands[0].imm<a+n
   for op in i.operands:
    if op.type==CS_OP_MEM and not op.mem.base and not op.mem.index:
     assert op.mem.disp in [0x58b8c0,0x58b8c8] and op.size in [4,8]
     refs.append(i.address-low+i.disp_offset)
  image[a-low:a-low+n]=b
  graphs[hex(a)]=dict(bytes=n,end=hex(a+n),sha256=sha(b),calls=calls,instructions={hex(i.address):bytes(i.bytes).hex()for i in ins})
 assert len(refs)==4
 image[0x58b8c0-low:0x58b8d0-low]=raw(binary,0x58b8c0,16)
 return words([len(image),ENTRIES[0][0]-low,len(refs),low])+image+words(refs),dict(graphs=graphs,constants=raw(binary,0x58b8c0,16).hex(),absolute_relocations=refs,low=hex(low))

def matching_graph(binary,mapping,map_text,entry):
 # Only genuine public entries plus the five exact private C helpers are
 # transported. No unrelated MAP boundary or CRT endpoint is accepted.
 assert mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(binary)
 symbols={row['orig']:row['recomp']for row in mapping['symbols']}
 public={symbols[a]for a,n in ENTRIES};root=symbols[0x4c25a0 if entry=='parent'else 0x532f60]
 rows={}
 for m in re.finditer(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(\S+)',map_text,re.M):
  rows.setdefault(int(m[2],16),[]).append((m[1],m[3]))
 starts=sorted(rows);assert public<=set(starts)
 helper_names={'_LPRead','_LPWrite','_LPScalar','_LPNaN','_LPCopyWord'}
 owners={'49-physics.c.obj','matrix34.c.obj','libbrender:matrix34.c.obj'}
 cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
 todo=[root];functions={};code={};data={};refs=[];edges=[]
 while todo:
  start=todo.pop()
  if start in functions:continue
  if start not in public:
   assert start in rows and any(name in helper_names and owner in owners for name,owner in rows[start]),('unapproved callee',hex(start),rows.get(start))
  end=next(v for v in starts if v>start);work=[start];seen={}
  while work:
   address=work.pop()
   while address not in seen:
    assert start<=address<end,(hex(start),hex(address),hex(end))
    i=next(cs.disasm(raw(binary,address,15),address));assert i.address+i.size<=end
    seen[address]=bytes(i.bytes);code[address]=bytes(i.bytes)
    for op in i.operands:
     if op.type==CS_OP_MEM and not op.mem.base and not op.mem.index:
      # Absolute operands in this graph are floating constants only. The
      # matrix/arena arguments are runtime data and are never relocated.
      assert i.mnemonic.startswith('f') and op.size in [4,8],('nonconstant absolute data',hex(address),i.mnemonic,i.op_str)
      data[op.mem.disp]=raw(binary,op.mem.disp,op.size);refs.append(address+i.disp_offset)
    if i.mnemonic=='call':
     assert i.operands[0].type==CS_OP_IMM
     target=i.operands[0].imm;todo.append(target);edges.append(dict(site=hex(address),target=hex(target),kind='call'))
    if i.mnemonic.startswith('j'):
     assert i.operands[0].type==CS_OP_IMM
     target=i.operands[0].imm
     if start<=target<end:work.append(target)
     else:
      assert i.mnemonic=='jmp';todo.append(target);edges.append(dict(site=hex(address),target=hex(target),kind='tail'))
     if i.mnemonic=='jmp':break
    if i.mnemonic=='ret':assert not i.operands;break
    address+=i.size
  functions[start]=seen
 # A direct lower test intentionally transports only its reachable leaf;
 # the parent test must contain both authentic distinct lower branches.
 assert (public<=set(functions))if entry=='parent'else symbols[0x532f60]in functions
 low=min([*code,*data]);high=max([a+len(b)for a,b in list(code.items())+list(data.items())])
 image=bytearray(b'\xcc'*(high-low))
 for a,b in list(code.items())+list(data.items()):image[a-low:a-low+len(b)]=b
 relocations=[a-low for a in refs]
 graph=dict(root=hex(root),low=hex(low),high=hex(high),functions={hex(a):dict(names=rows[a],instructions={hex(i):b.hex()for i,b in sorted(ins.items())})for a,ins in functions.items()},absolute_data={hex(a):b.hex()for a,b in data.items()},absolute_relocations=relocations,calls=edges)
 return words([len(image),root-low,len(relocations),low])+image+words(relocations),graph
def fixtures():
 cases=[]
 def add(kind='identity',state=0,seed=0,alias=0x900,nodes=1,offset=0,private=0,payload_words=None,in_place=False,output_offset=None,third_offset=0x500):
  r=random.Random(seed);arena=bytearray(r.randbytes(4096));objects=[0x100,0x300,third_offset]
  for index,obj in enumerate(objects):
   actor=alias+index*0x100
   struct.pack_into('<I',arena,obj,BASE+actor-0x2c)
   struct.pack_into('<I',arena,obj+0xf0,state)
   struct.pack_into('<I',arena,obj+0x224,BASE+objects[1]if index==0 and nodes>1 else 0)
   struct.pack_into('<I',arena,obj+0x220,BASE+objects[2]if index==1 and nodes>2 else 0)
   m=[1.,0,0,0,1.,0,0,0,1.,3.,-2.,1.]
   if kind=='oblique':m=[1.25,.125,-.25,-.375,.75,.25,.125,-.5,1.5,.75,-1.25,2.]
   if kind=='zero':m=[0.]*12
   if kind=='random':m=[r.uniform(-10.,10.)for _ in range(12)]
   if kind=='small':m=[1e-8,-1e-8,1e-8,0,1e-8,0,1e-8,1e-8,-1e-8,0,0,0]
   payload=struct.pack('<12f',*m)
   if kind=='ieee':
    payload=words([0,0x80000000,1,0x80000001,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7f800001,0xff800002,0x7fc12345,0xffc54321])
    payload=payload[(seed%12)*4:]+payload[:(seed%12)*4]
   if payload_words is not None:payload=words(payload_words)
   arena[obj+0x8c:obj+0xbc]=payload
   arena[obj+0xbc:obj+0xec]=payload
  # Safe matrix-output aliases alter matrices but do not overwrite live actor,
  # child or next pointers. Recursive graph remains bounded and accessible.
  if offset or in_place:
   struct.pack_into('<I',arena,0x100,BASE+0x18c+offset-0x2c)
  if output_offset is not None:struct.pack_into('<I',arena,0x100,BASE+output_offset-0x2c)
  case=dict(kind=kind,state=state,seed=seed,alias=alias,nodes=nodes,offset=offset,private=private)
  if payload_words is not None:case['payload_words']=payload_words
  if in_place:case['in_place']=True
  if output_offset is not None:case['output_offset']=output_offset
  if third_offset!=0x500:case['third_offset']=third_offset
  cases.append((case,words([0x100,0xcccccccc,private])+arena))
 for k in ['identity','oblique','zero','random','small']:
  for state in [0,1,2,0xffffffff]:
   for nodes in [1,2,3]:
    for private in [0,0xcccccccc]:add(k,state,11,0x900,nodes,0,private)
 for state in [0,1,2]:
  for seed in range(12):add('ieee',state,seed)
 for state in [0,1,2]:
  for offset in [-12,-8,-4,-1,1,4,8,12,44,48]:add('oblique',state,2,0x900,1,offset)
 for seed in range(64):add('random',0,seed,0x900,1)
 # Keep the first 250 exact fixtures as the preserved baseline. Directed
 # additions exercise raw paired payload/sign selection and live source reads
 # under partial overlap, including the formerly absent exact in-place case.
 identity=[0x3f800000,0,0,0,0x3f800000,0,0,0,0x3f800000,0x7f800001,0xff800002,0x80000000]
 ieee=[0x7fc12345,0xffc12345,0x7f812345,0xff812345,0x7fc54321,0xffc54321,0x7f800000,0xff800000,0,0x80000000]
 for lane in [3,4,5,6,7,8]:
  for value in ieee:
   matrix=identity.copy();matrix[lane]=value
   for offset in [None,0,-4,4,12]:
    add('single-ieee',payload_words=matrix,offset=offset or 0,in_place=offset is not None)
 for lanes in [(3,4),(4,5),(3,5),(6,7),(7,8),(6,8)]:
  for left,right in [(0x7fc12345,0xffc12345),(0xffc12345,0x7fc12345),(0x7f812345,0xff812345),(0xff812345,0x7f812345),(0x7f800000,0xff800000),(0xff800000,0x7f800000)]:
   matrix=identity.copy();matrix[lanes[0]]=left;matrix[lanes[1]]=right
   for offset in [None,0,-4,4,12]:
    add('paired-ieee',payload_words=matrix,offset=offset or 0,in_place=offset is not None)
 # Exact epsilon-adjacent and exponent patterns reach both ordered threshold
 # branches without replacing the genuine sqrt/divide implementation.
 for lane in [4,6,7,8]:
  for value in [0,1,0x007fffff,0x00800000,0x347fffff,0x34800000,0x34800001,0x3f7fffff,0x3f800001,0x5f000000,0x7f7fffff]:
   matrix=identity.copy()
   if lane in [6,7,8]:matrix[6:9]=[0,0,0]
   matrix[lane]=value
   for offset in [None,0,4]:add('threshold-exponent',payload_words=matrix,offset=offset or 0,in_place=offset is not None)
 for state in [0,1,2,0xffffffff]:
  for offset in range(-32,65,4):add('oblique',state,79,0x900,3,offset,0,in_place=True)
 for seed in range(128):
  rng=random.Random(0x532f6000+seed)
  matrix=[rng.getrandbits(32)for _ in range(12)]
  for offset in [None,0,4]:add('raw-word-stress',seed=seed,payload_words=matrix,offset=offset or 0,in_place=offset is not None)
 # Root output can change a later child's matrix or branch before that real
 # recursive call. The next/child case uses authentic accessible pointer bits
 # and prevents loops/faults; no pointer or numerical output is normalized.
 for state in [0,1,2]:
  for output in [0x38c,0x3cc,0x4fc]:
   for next_ in [0,BASE+0x600]:
    matrix=identity.copy();matrix[9:12]=[next_,0,0]
    add('recursive-live-alias',state,23,nodes=3,payload_words=matrix,output_offset=output,third_offset=0x600)
 return cases
def main():
 p=argparse.ArgumentParser();p.add_argument('--output',required=True,type=Path);p.add_argument('--candidate-source',type=Path);p.add_argument('--lp-candidate-source',type=Path);p.add_argument('--entry',choices=['parent','lp'],default='parent');p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);a=p.parse_args();assert bool(a.recompiled)==bool(a.symbols);out=a.output.resolve();out.mkdir(parents=True,exist_ok=True)
 global SOURCE,LP,COMMON
 SOURCE=a.candidate_source.resolve()if a.candidate_source else SOURCE
 LP=a.lp_candidate_source.resolve()if a.lp_candidate_source else LP
 if a.entry=='lp':
  first=COMMON.index('#ifdef REC2_MATCHING');last=COMMON.index('#endif',first)+len('#endif')
  COMMON=COMMON[:first]+'builder_stack_count=2;builder_cdecl=1;builder_stack[0]=builder_x;builder_stack[1]=0x22000000+h[1];'+COMMON[last:]

 paths=[SOURCE,LP,MATRIX,VECTOR,Path(__file__).resolve(),ROOT/'scripts/verify_physics_object_matrix_reconstruction.py',ROOT/'scripts/verify_phil_registry_reconstruction.py',ROOT/'scripts/verify_opponent_navigation_reconstruction.py',ROOT/'scripts/verify_opponent_route_reconstruction.py',ROOT/'scripts/verify_tinted_native_reconstruction.py']
 paths+=list((ROOT/'src').rglob('*.h'))
 if a.recompiled:paths.extend([a.recompiled,a.symbols,a.recompiled.with_suffix('.map')])
 before={str(f):sha(f.read_bytes())for f in paths};(out/'source-before.json').write_text(json.dumps(before,indent=2))
 (out/'verifier-snapshot.py').write_bytes(Path(__file__).read_bytes())
 (out/'candidate-snapshot.c').write_bytes(LP.read_bytes())
 (out/'parent-snapshot.c').write_bytes(SOURCE.read_bytes())
 original=(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes();assert sha(original)==HASH
 code,graph=original_graph(original)
 if a.entry=='lp':code=code[:4]+words([0x532f60-int(graph['low'],16)])+code[8:]
 (out/'original-code.bin').write_bytes(code)
 images={'original':code};graphs={'original':graph}
 if a.recompiled:
  images['matching'],graphs['matching']=matching_graph(a.recompiled.read_bytes(),json.loads(a.symbols.read_text()),a.recompiled.with_suffix('.map').read_text(encoding='latin1'),a.entry)
  (out/'matching-code.bin').write_bytes(images['matching'])
 lp_text=LP.read_text();lp_names=['LPRead','LPWrite','LPScalar','LPNaN','LPCopyWord','BrMatrix34LPNormalise']
 lp_capsule='\n'.join(extract(lp_text,n)for n in lp_names)
 parent_body=extract(SOURCE.read_text(),'FUN_004c25a0');copy_body=extract(MATRIX.read_text(),'BrMatrix34Copy')
 entry_name='FUN_004c25a0'if a.entry=='parent'else 'BrMatrix34LPNormalise'
 native=COMMON+'\n'+lp_capsule+'\n'+copy_body+'\n'+parent_body+'\nint C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;builder_address=(unsigned)'+entry_name+';return run(argv[1]);}\n'
 (out/'verified-capsules.c').write_text(lp_capsule+'\n'+copy_body+'\n'+parent_body)

 caller,_,_=registry.literals();caller=caller.replace('builder_poison;','builder_poison,builder_cdecl;')
 old=' mov builder_result,eax\n mov ecx,esp';assert old in caller
 caller=caller.replace(old,' mov builder_result,eax\n cmp builder_cdecl,0\n je restore_stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n restore_stackready:\n mov ecx,esp')
 registry.COMMON=COMMON;commands=registry.compile_sources(out,native,PHYSICAL,caller)
 cases=fixtures()
 if a.entry=='lp':
  direct=[]
  for case,blob in cases:
   arena=blob[12:];actor=struct.unpack_from('<I',arena,0x100)[0];dest=actor+0x2c-BASE
   direct.append((dict(case,entry='lp',output_offset=dest,source_offset=0x18c),words([dest,0x18c,struct.unpack_from('<I',blob,8)[0]])+arena))
  cases=direct
 fixture=out/'fixtures.bin';fixture.write_bytes(b''.join(b for c,b in cases))
 runs={};results=[];failures=[]
 for label in ['original','msvc5','msvc','mingw',*(['matching']if a.recompiled else [])]:
  command=[str(out/('physical.exe'if label in images else label+'.exe')),str(fixture)]
  if label in images:command.append(str(out/(label+'-code.bin')))
  proc=subprocess.run(command,capture_output=True,timeout=60);(out/(label+'-output.bin')).write_bytes(proc.stdout);(out/(label+'-stderr.bin')).write_bytes(proc.stderr)
  assert proc.returncode==0,(label,hex(proc.returncode&0xffffffff),proc.stderr)
  assert len(proc.stdout)==len(cases)*4100,(label,len(proc.stdout),len(cases)*4100)
  runs[label]=proc.stdout
 for index,(case,blob)in enumerate(cases):
  expected=runs['original'][index*4100:(index+1)*4100];assert struct.unpack_from('<I',expected)[0]==index
  for label in ['msvc5','msvc','mingw',*(['matching']if a.recompiled else [])]:
   actual=runs[label][index*4100:(index+1)*4100];passed=actual==expected
   result=dict(index=index,compiler=label,passed=passed,case=case,input_sha256=sha(blob),input_length=len(blob),original_sha256=sha(expected),original_length=len(expected),actual_sha256=sha(actual),actual_length=len(actual))
   results.append(result)
   if not passed:
    result['first_mismatch_offset']=next(i for i,(a,b)in enumerate(zip(expected,actual))if a!=b)-4
    failures.append(result)
 after={str(f):sha(f.read_bytes())for f in paths};assert before==after
 report=dict(passed=not failures,status='bounded complete LPNormalise and position restore proof; no whole-game equivalence',entry=a.entry,script_sha256=sha(Path(__file__).read_bytes()),scenes=len(cases),comparisons=len(results),failures=failures,results=results,original_sha256=HASH,original_graph=graph,graphs=graphs,recompiled_sha256=sha(a.recompiled.read_bytes())if a.recompiled else None,symbols_sha256=sha(a.symbols.read_bytes())if a.symbols else None,commands=commands,sources=before,source_stable=before==after,abi=dict(parent='ECX object, RET0, void EAX excluded',lower='cdecl two pointers, caller cleanup, void EAX excluded',checks=['ESP','EBX','ESI','EDI','EBP','x87 TOP'],normal_return_cases=len(cases),terminal_cases=0),artifacts={str(f):sha(f.read_bytes())for f in out.iterdir()if f.is_file()},limitations=__doc__+' Masked exceptions and runtime PC53 only; sticky FP status/unmasked traps, invalid pointers/graphs, cycles and non-x86 are not proven. The original full 3-entry graph physically executes; no Unicorn arithmetic oracle is used.')
 (out/'verification.json').write_text(json.dumps(report,indent=2)+'\n')
 print(json.dumps(dict(passed=report['passed'],scenes=len(cases),comparisons=len(results),failures=len(failures),first_failures=failures[:6]),indent=2))
if __name__=='__main__':main()
