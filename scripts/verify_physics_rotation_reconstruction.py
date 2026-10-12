"""Complete rotation pair with actual original and production lower graph.

Physical original/native3 at normalPC53; full64KiB arenas and x86ABI compare.
Mutable BRender mattmp1/2 are initialized/serialized explicitly. No math or
orientation operation is mocked. Masked FPstatus/unmaskedtraps outside proof.
Only the documented default cyclic index table1,2,0 is initialized. Object/
actor arena aliases are exercised; aliases to independently transported math
globals or private callee frames are outside this bounded fixture.
"""
import argparse,hashlib,json,random,re,struct,subprocess,sys
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'src/carma2').is_dir());sys.path.insert(0,str(ROOT/'scripts'))
from verify_tinted_native_reconstruction import extract
from verify_opponent_route_reconstruction import HASH,pe,raw,sha,words
import verify_phil_registry_reconstruction as registry
import verify_physics_joint_translation_reconstruction as translation
from inventory_reconstruction_backlog import mask_c

p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',required=True,type=Path);p.add_argument('--limit',type=int);p.add_argument('--case-index',type=int,action='append');p.add_argument('--candidate-source',type=Path);p.add_argument('--matrix-rotate-candidate',type=Path);p.add_argument('--matrix-post-candidate',type=Path);p.add_argument('--expanded',action='store_true');p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);args=p.parse_args();OUT=args.output.resolve();OUT.mkdir(exist_ok=True,parents=True)
assert bool(args.recompiled)==bool(args.symbols),'Supply both immutable PE and hash-bound symbol map'
assert args.limit is None or args.limit>0
physics=ROOT/'src/carma2/newcommon/49-physics.c';candidate=args.candidate_source.resolve()if args.candidate_source else physics;matrix=ROOT/'src/brender/core/math/matrix34.c';phil=ROOT/'src/carma2/newcommon/60-phil.c';quat=ROOT/'src/brender/core/math/quat.c'
paths=[candidate,physics,matrix,phil,quat,Path(__file__),ROOT/'scripts/verify_physics_joint_translation_reconstruction.py',ROOT/'scripts/verify_phil_registry_reconstruction.py',ROOT/'scripts/verify_opponent_navigation_reconstruction.py',ROOT/'src/carma2/newcommon/49-physics.h',ROOT/'src/carma2/newcommon/60-phil.h',ROOT/'src/carma2/include/rec2_types.h',ROOT/'src/carma2/hooks/c2_hooks.h',ROOT/'src/carma2/hooks/c2_math.h',ROOT/'src/brender/core/math/matrix34.h',ROOT/'src/brender/include/brender/brender.h',ROOT/'src/brender/include/brender/br_defs.h',ROOT/'src/brender/include/brender/br_types.h']
if args.matrix_rotate_candidate:paths.append(args.matrix_rotate_candidate.resolve())
if args.matrix_post_candidate:paths.append(args.matrix_post_candidate.resolve())
paths.extend([ROOT/'scripts/inventory_reconstruction_backlog.py',ROOT/'scripts/verify_tinted_native_reconstruction.py',ROOT/'src/carma2/newcommon/globvrpb.h'])
if args.recompiled:paths.extend([args.recompiled,args.symbols,args.recompiled.with_suffix('.map')])
before={str(f):f.read_bytes()for f in paths};(OUT/'source-before.json').write_text(json.dumps({f:sha(b)for f,b in before.items()},indent=2))
data=(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes();assert sha(data)==HASH;nt,base,size,tab=pe(data);cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
ORIGINAL_ENTRIES=[('RotateObjectFirstOrder',0x4c1f10,330),('RotateObject',0x4c1be0,807),('__ftol',0x575b80,39),('BrMatrix34Rotate',0x532940,333),('BrMatrix34TApplyV',0x533690,107),('BrMatrix34PreTranslate',0x533dc0,159),('BrMatrix34PostTranslate',0x533e60,45),('BrMatrix34Pre',0x533700,40),('GetOrientationFromMatrix',0x49be90,356),('GetMatrixFromOrientation',0x49c000,200),('BrMatrix34Mul',0x532620,404),('BrMatrix34Copy',0x5325d0,79),('BrMatrix34ToQuat',0x535ae0,385),('BrQuatNormalise',0x535710,114),('BrQuatToMatrix34',0x5359a0,308)]
catalog={a:dict(name=name,body_bytes=n)for name,a,n in ORIGINAL_ENTRIES}
todo=[0x4c1f10,0x4c1be0];seen=set();spec=[];patches=[];constants={}
while todo:
 start=todo.pop(0)
 if start in seen:continue
 seen.add(start);row=catalog[start];name={0x49be90:'GetOrientationFromMatrix',0x49c000:'GetMatrixFromOrientation',0x4c1f10:'RotateObjectFirstOrder',0x4c1be0:'RotateObject'}.get(start,row['name']);n=row['body_bytes'];ins=list(cs.disasm(raw(data,start,n),start));assert sum(i.size for i in ins)==n
 for i in ins:
  for op in i.operands:
   addr=None;width=None
   if op.type==CS_OP_MEM and base<=op.mem.disp<base+size:
    addr=op.mem.disp;width=12 if op.mem.index or op.mem.base else op.size;site=i.address+i.disp_offset-base;assert i.disp_size==4
   elif op.type==CS_OP_IMM and i.mnemonic in('mov','push')and base<=op.imm<base+size:
    addr=op.imm;width=4;site=i.address+i.imm_offset-base;assert i.imm_size==4
   if addr is not None:
    absolute=False
    if 0x6b09c0<=addr<0x6b0a20:target=0x22003b00+(addr-0x6b09c0);absolute=True
    elif addr==0x68b918:target=0x2200f000;absolute=True
    else:
     section=next((struct.unpack_from('<I',data,tab+k*40+36)[0]for k in range(struct.unpack_from('<H',data,nt+6)[0])if base+struct.unpack_from('<I',data,tab+k*40+12)[0]<=addr<base+struct.unpack_from('<I',data,tab+k*40+12)[0]+max(struct.unpack_from('<I',data,tab+k*40+8)[0],struct.unpack_from('<I',data,tab+k*40+16)[0])),None)
     assert section is not None and (not section&0x80000000 or addr==0x66cc20 and width==12 and raw(data,addr,width)==words([1,2,0])),(hex(start),hex(addr),i.op_str)
     bits=raw(data,addr,width);constants[hex(addr)]=bits.hex();target=addr-base
    patches.append((site,addr,target,int(absolute)))
  if i.mnemonic=='call'or i.mnemonic.startswith('j'):
   assert i.operands[0].type==CS_OP_IMM
   target=i.operands[0].imm
   if start<=target<start+n:continue
   assert target in catalog and (target in(0x4c1f10,0x49be90,0x49c000,0x575b80)or catalog[target]['name'].startswith('BrMatrix34')or catalog[target]['name'].startswith('BrQuat')),(name,i.op_str)
   todo.append(target)
 spec.append({'name':name,'address':start,'bytes':n,'sha256':sha(raw(data,start,n))})
(OUT/'original-addresses.bin').write_bytes(words([0x4c1f10-base,0x4c1be0-base,len(patches)])+b''.join(words(v)for v in patches))
COMMON=translation.COMMON.replace('#include "49-physics.h"','#include "49-physics.h"\n#include "60-phil.h"\n#include "globvrpb.h"\n#include "c2_math.h"\n#include <math.h>\nextern br_quat* C2_HOOK_CDECL BrQuatNormalise(br_quat*,const br_quat*);\nextern br_matrix34* C2_HOOK_CDECL BrQuatToMatrix34(br_matrix34*,const br_quat*);\ntNet_mode gNet_mode;\nbr_matrix34 mattmp1,mattmp2;\n#define M_DIAG(x) (((br_scalar*)mat->m)[4*(x)])\n#define Q_EL(e) (((br_scalar*)&(q->x))[(e)])')
COMMON=COMMON.replace('builder_stack_count=mode?0:1','builder_stack_count=1')
COMMON=COMMON.replace('fwrite(head,sizeof(head),1,stdout);fwrite(heap,65536,1,stdout);','fwrite(head,sizeof(head),1,stdout);\n#ifndef PHYSICAL_IMAGE\nmemcpy(heap+0x3b00,&mattmp1,48);memcpy(heap+0x3b30,&mattmp2,48);\n#endif\nfwrite(heap,65536,1,stdout);')
COMMON=COMMON.replace('_setmode(_fileno(stdout),_O_BINARY);for(', '_setmode(_fileno(stdout),_O_BINARY);for(')
COMMON=COMMON.replace('_controlfp(_PC_53,_MCW_PC);_clearfp();if(h[0]', 'gNet_mode=(tNet_mode)h[3];memcpy(heap+0xf000,&h[3],4);memcpy(&mattmp1,heap+0x3b00,48);memcpy(&mattmp2,heap+0x3b30,48);_controlfp(_PC_53,_MCW_PC);_clearfp();if(h[0]')
caller,physical,template=registry.literals();old=' cmp builder_mode,8\n jne notwide\n fstp qword ptr builder_wide\n xor eax,eax\n notwide:\n cmp builder_mode,1\n je zeroreturn\n cmp builder_mode,10\n jne resultready\n zeroreturn:\n xor eax,eax\n resultready:';assert old in caller;caller=caller.replace(old,' xor eax,eax')
old='image[hdr[12]]=0xe9;relative=(unsigned)RayBoundary-((unsigned)image+hdr[12]+5);memcpy(image+hdr[12]+1,&relative,4);';assert old in physical;physical=physical.replace(old,'').replace('relative,','').replace('hdr[14]','hdr[3]').replace('n=hdr[13]','n=hdr[2]').replace('i<12;i++)addresses','i<2;i++)addresses')
body='\n'.join(extract(before[str(physics)].decode(),name)for name in ('PhysicsJointScalar','PhysicsWallNaN'))+'\n'
body+=extract(before[str(matrix)].decode(),'BrMatrix34Translate')+'\n'

def lower_capsule(text,name,helper):
 clean=mask_c(text);body=extract(text,name);start=text.index(body)
 hits=list(re.finditer(r'^static [^\n;{}]*\b'+re.escape(helper)+r'\([^;{}\n]*\)\s*\{',clean,re.M))
 assert len(hits)==1,(helper,len(hits));assert hits[0].start()<start
 return text[hits[0].start():start]+body+('\n#undef MR_PRODUCT\n#undef MR_SUM\n#undef MR_DIFFERENCE\n'if name=='BrMatrix34Rotate'else'\n')
for row in spec:
 name=row['name']
 if row['address']in(0x4c1f10,0x4c1be0,0x575b80):continue
 if name=='BrMatrix34Rotate'and args.matrix_rotate_candidate:
  body+=before[str(args.matrix_rotate_candidate.resolve())].decode()+'\n';continue
 if name=='BrMatrix34PostTranslate'and args.matrix_post_candidate:
  body+=before[str(args.matrix_post_candidate.resolve())].decode()+'\n';continue
 source=matrix if name.startswith('BrMatrix')and name!='BrMatrix34ToQuat' else quat if name.startswith('BrQuat')or name=='BrMatrix34ToQuat' else phil
 if name in('BrMatrix34Rotate','BrMatrix34PostTranslate'):
  body+=lower_capsule(before[str(source)].decode(),name,'MatrixRotateScalar'if name=='BrMatrix34Rotate'else'MatrixPostTranslateAdd')
 else:body+=extract(before[str(source)].decode(),name)+'\n'
for name in('PhilOrientationScalar',):body=extract(before[str(phil)].decode(),name)+'\n'+body
for name in('QuaternionSum','QuaternionScaleRaw','QuaternionScaleLoaded','QuaternionMultiply','QuaternionAddScalar','QuaternionSubtractScalar','QuaternionScalar'):body=extract(before[str(quat)].decode(),name)+'\n'+body
rotation_text=before[str(candidate)].decode()
body+='\n'+'\n'.join(extract(rotation_text,name)for name in('RotationArgument','RotateObjectFirstOrder','RotateObject'))+'\n'
native=COMMON+'\n'+body+r'''
static volatile unsigned native_bodies[2];
static unsigned __fastcall adapter0(tPhysics_object*x,unsigned y,br_scalar time){typedef void(C2_HOOK_FAKE_THISCALL*fn)(tPhysics_object*,unsigned,br_scalar);((fn)(size_t)native_bodies[0])(x,y,time);return 0;}
static unsigned __fastcall adapter1(tPhysics_object*x,unsigned y,br_scalar time){typedef void(C2_HOOK_FAKE_THISCALL*fn)(tPhysics_object*,unsigned,br_scalar);((fn)(size_t)native_bodies[1])(x,y,time);return 0;}
int C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;native_bodies[0]=(unsigned)RotateObjectFirstOrder;native_bodies[1]=(unsigned)RotateObject;addresses[0]=(unsigned)adapter0;addresses[1]=(unsigned)adapter1;return run(argv[1]);}
'''
registry.COMMON='#define PHYSICAL_IMAGE\n'+COMMON;commands=registry.compile_sources(OUT,native,physical,caller)
matching=None
if args.recompiled:
 linked_data=args.recompiled.read_bytes();mapping=json.loads(args.symbols.read_text());assert mapping['format']==1 and mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(linked_data)
 symbols={r['orig']:r['recomp']for r in mapping['symbols']};map_bytes=args.recompiled.with_suffix('.map').read_bytes()
 functions={int(address,16):(name,owner.strip())for name,address,owner in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',map_bytes.decode('latin1'),re.M)}
 nt2,base2,size2,tab2=pe(linked_data);sections=[]
 for k in range(struct.unpack_from('<H',linked_data,nt2+6)[0]):
  vs,rva,n,off=struct.unpack_from('<IIII',linked_data,tab2+40*k+8);flags=struct.unpack_from('<I',linked_data,tab2+40*k+36)[0];sections.append((base2+rva,max(vs,n),flags))
 initial={symbols[a]:name for name,a,_ in ORIGINAL_ENTRIES if a in symbols};roots=[symbols[0x4c1f10],symbols[0x4c1be0]]
 mutable={symbols[0x6b09c0]:0x22003b00,symbols[0x6b09f0]:0x22003b30,symbols[0x68b918]:0x2200f000}
 todo=list(roots);seen=set();linked_graph=[];linked_patches={};linked_constants={}
 for start in todo:
  if start in seen:continue
  seen.add(start);assert start in functions,hex(start)
  end=min(a for a in functions if a>start);name=initial.get(start,functions[start][0]);owner=functions[start][1]
  if start not in initial:
   plain=name.lstrip('_@').split('@')[0]
   assert owner in('49-physics.c.obj','60-phil.c.obj','matrix34.c.obj','quat.c.obj')and plain.startswith(('PhysicsJointScalar','PhysicsWallNaN','RotationArgument','MatrixRotate','MatrixPostTranslate','PhilOrientation','Quaternion'))or plain in('ftol','_ftol'),(name,owner)
  pending=[start];instructions={}
  while pending:
   location=pending.pop()
   while location not in instructions:
    assert start<=location<end,(name,hex(location))
    ins=next(cs.disasm(raw(linked_data,location,15),location));instructions[location]=ins.bytes.hex()
    if ins.mnemonic=='call'or ins.mnemonic.startswith('j'):
     assert ins.operands[0].type==CS_OP_IMM,(name,ins.op_str)
     target=ins.operands[0].imm
     if start<=target<end:
      if ins.mnemonic!='call':pending.append(target)
      else:raise AssertionError(('Unexpected local call',name,ins.op_str))
     else:
      assert target in functions,(name,ins.op_str);todo.append(target)
    for op in ins.operands:
     value=None;site=None;width=4
     if op.type==CS_OP_MEM and base2<=op.mem.disp<base2+size2:
      value=op.mem.disp;site=ins.address+ins.disp_offset-base2;width=12 if op.mem.base or op.mem.index else op.size;assert ins.disp_size==4
     elif op.type==CS_OP_IMM and ins.mnemonic in('mov','push')and base2<=op.imm<base2+size2:
      value=op.imm;site=ins.address+ins.imm_offset-base2;assert ins.imm_size==4
     if value is not None:
      region=next(s for s in sections if s[0]<=value<s[0]+s[1]);absolute=False
      head=next((a for a in mutable if a<=value<a+(48 if a!=symbols[0x68b918]else 4)),None)
      if head is not None:target=mutable[head]+value-head;absolute=True
      else:
       assert not region[2]&0x80000020,(name,hex(value),ins.op_str)
       bits=raw(linked_data,value,width);linked_constants[hex(value)]=bits.hex();target=value-base2
       if op.type==CS_OP_MEM and(op.mem.base or op.mem.index):assert bits==words([1,2,0]),(name,ins.op_str,bits.hex())
      patch=(site,value,target,int(absolute));assert site not in linked_patches or linked_patches[site]==patch;linked_patches[site]=patch
    if ins.mnemonic.startswith('ret')or ins.mnemonic in('jmp','int3','int'):break
    location+=ins.size
  linked_graph.append(dict(name=name,address=start,extent=end-start,instructions=sorted(instructions.items()),instruction_count=len(instructions),instruction_sha256=sha(b''.join(bytes.fromhex(v)for _,v in sorted(instructions.items())))))
 # Every actual code relocation in the reachable closure must be transported.
 rr,rz=struct.unpack_from('<II',linked_data,nt2+24+96+5*8);cursor=base2+rr;checked=[];active={address+k for row in linked_graph for address,code in row['instructions']for k in range(len(code)//2)}
 while cursor<base2+rr+rz:
  page,n=struct.unpack('<II',raw(linked_data,cursor,8));assert n>=8
  for word in struct.unpack('<'+'H'*((n-8)//2),raw(linked_data,cursor+8,n-8)):
   if word>>12==3:
    site=page+(word&4095)
    if base2+site in active:assert site in linked_patches,hex(site);checked.append(site)
  cursor+=n
 patch_path=OUT/'matching-addresses.bin';patch_path.write_bytes(words([r-base2 for r in roots]+[len(linked_patches)])+b''.join(words(v)for _,v in sorted(linked_patches.items())))
 matching=dict(recompiled_sha256=sha(linked_data),symbols_sha256=sha(args.symbols.read_bytes()),map_sha256=sha(map_bytes),graph=linked_graph,constants=linked_constants,patches=list(linked_patches.values()),relocations_checked=checked,patch_file_sha256=sha(patch_path.read_bytes()))
cases=[]
for mode in(0,1,2):
 for network in(0,1):
  for seed in range(30):cases.append({'mode':mode,'network':network,'seed':seed,'time':0x3dcccccd if seed%3 else 0x3f000000})

if args.expanded:
 def float_word(value):return struct.unpack('<I',struct.pack('<f',value))[0]
 def scene(mode=1,network=0,seed=7,time=0x3dcccccd,patches=(),actor_matrix_at=None):
  cases.append(dict(mode=mode,network=network,seed=seed,time=time,patches=list(patches),actor_matrix_at=actor_matrix_at))
 # Exact branch neighbors use one-axis omega so the original wide thresholds
 # are isolated from unrelated squared-norm rounding.
 threshold_words=[]
 for value in(1e-4,1e-7**.5,.00800000037997961**.5):
  word=float_word(value);threshold_words.extend((word-1,word,word+1))
 times=(0,0x80000000,1,0x80000001,0x3f800000,0xbf800000,0x7f7fffff,
        0xff7fffff,0x7f800000,0xff800000,0x7fc12345,0xffc12345,0x7f812345,0xff812345)
 omega_one=[(0x174,0x3f800000),(0x178,0),(0x17c,0)]
 for mode in(0,1,2):
  for network in(0,1):
   for time in tuple(threshold_words)+times:scene(mode,network,time=time,patches=omega_one)
 pool=(0,0x80000000,1,0x80000001,0x7f7fffff,0xff7fffff,0x7f800000,
       0xff800000,0x7fc12345,0xffc12345,0x7f812345,0xff812345)
 for mode in(0,1):
  for network in(0,1):
   for offsets in((0x174,0x178,0x17c),(0x108,0x10c,0x110),(0x114,0x118,0x11c)):
    for offset in offsets:
     for value in pool:scene(mode,network,patches=[(offset,value)])
   for offsets in((0x174,0x178),(0x108,0x10c),(0x114,0x118)):
    for values in((0x7fc12345,0xffc12345),(0xffc12345,0x7fc12345),
                  (0x7f812345,0xff812345),(0xff812345,0x7f812345),
                  (0x7f812345,0xffc12345),(0xffc12345,0x7f812345)):
     scene(mode,network,patches=list(zip(offsets,values)))
 # These valid arena aliases place actor matrix lanes on mutable object fields;
 # none overwrites the actor pointer or creates an arbitrary invalid pointer.
 for mode in(0,1,2):
  for network in(0,1):
   for offset in(0x108,0x114,0x174,0x2c0):
    for seed in(0,7,19):scene(mode,network,seed=seed,actor_matrix_at=offset)
 # Subdivision directly straddles/crosses the genuine400-iteration cap.
 for time in(71.4,71.6,80.0,400.0):
  for network in(0,1):scene(1,network,time=float_word(time),patches=omega_one)
def make_case(c):
 rng=random.Random(c['seed']);b=bytearray(words([rng.getrandbits(32)for _ in range(16384)]));obj=0x100;actor=c['actor_matrix_at']-0x2c if c.get('actor_matrix_at')is not None else 0x5000
 struct.pack_into('<I',b,obj,0x22000000+actor)
 for at,values in((obj+8,(1.0,2.0,3.0)),(obj+0x14,(1.0,2.0,3.0)),(obj+0x74,tuple(rng.uniform(-2,2)for _ in range(3))), (actor+0x2c,(1,0,0,0,1,0,0,0,1,4,5,6))):struct.pack_into('<'+'f'*len(values),b,at,*values)
 for at,value in c.get('patches',()):struct.pack_into('<I',b,at,value)
 return words([c['mode'],0x22000100,c['time'],c['network'],0,0,c['seed']])+b
indices=args.case_index or list(range(len(cases)));indices=indices[:args.limit]if args.limit else indices;results=[];failures=[]
for index in indices:
 payload=make_case(cases[index]);path=OUT/'case.bin';path.write_bytes(payload);r=subprocess.run([str(OUT/'physical.exe'),str(path),str(ROOT/'build-reference/originals/CARMA2_HW.EXE'),str(OUT/'original-addresses.bin')],capture_output=True,timeout=30);assert r.returncode==0,('original',index,r.returncode);expected=r.stdout
 for compiler in('msvc5','msvc','mingw'):
  r=subprocess.run([str(OUT/(compiler+'.exe')),str(path)],capture_output=True,timeout=30)
  result={'index':index,'compiler':compiler,'exit':r.returncode,'passed':r.returncode==0 and r.stdout==expected};results.append(result)
  if not result['passed']:
   result['differences']=[{'offset':j,'expected':expected[j:j+4].hex(),'actual':r.stdout[j:j+4].hex()}for j in range(0,min(len(expected),len(r.stdout)),4)if expected[j:j+4]!=r.stdout[j:j+4]];failures.append(result);(OUT/f'failure-{index}-{compiler}.json').write_text(json.dumps(result,indent=2));(OUT/f'failure-{index}.bin').write_bytes(payload);(OUT/f'failure-{index}.original-output.bin').write_bytes(expected);(OUT/f'failure-{index}.{compiler}-output.bin').write_bytes(r.stdout)
 if matching:
  r=subprocess.run([str(OUT/'physical.exe'),str(path),str(args.recompiled.resolve()),str(OUT/'matching-addresses.bin')],capture_output=True,timeout=30)
  result=dict(index=index,compiler='matching',exit=r.returncode,passed=r.returncode==0 and r.stdout==expected);results.append(result)
  if not result['passed']:
   result['differences']=[dict(offset=j,expected=expected[j:j+4].hex(),actual=r.stdout[j:j+4].hex())for j in range(0,min(len(expected),len(r.stdout)),4)if expected[j:j+4]!=r.stdout[j:j+4]];failures.append(result);(OUT/f'failure-{index}-matching.json').write_text(json.dumps(result,indent=2));(OUT/f'failure-{index}.bin').write_bytes(payload);(OUT/f'failure-{index}.original-output.bin').write_bytes(expected);(OUT/f'failure-{index}.matching-output.bin').write_bytes(r.stdout)
 if index%10==0:print(index,len(failures),flush=True)
 if len(failures)>=12:break
after={str(f):f.read_bytes()for f in paths};assert before==after,'Sourcechanged during proof'
manifest={'scope':__doc__,'matching':matching,'input_sha256':sha(b''.join(make_case(cases[i])for i in indices)),'expanded':args.expanded,'total_scenes':len(cases),'selected_scenes':len(indices),'complete_suite':len(indices)==len(cases)and indices.index(index)+1==len(cases),'spec':spec,'constants':constants,'patches':patches,'source_stable':True,'sources':{f:sha(b)for f,b in before.items()},'original_sha256':HASH,'scenes':indices.index(index)+1,'comparisons':len(results),'failures':failures,'results':results,'commands':commands,'generated_sha256':{str(f):sha(f.read_bytes())for f in OUT.iterdir()if f.suffix in('.c','.obj','.exe','.bin','.txt')}}
(OUT/'verification.json').write_text(json.dumps(manifest,indent=2)+'\n');print(manifest['scenes'],manifest['comparisons'],len(failures));assert not failures
