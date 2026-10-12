"""Unaccepted complete hierarchy motion pair with real original/production lower graph.

Physical original/native3 at normalPC53; full64KiB arenas and x86ABI compare.
Mutable BRender mattmp1/2 are initialized/serialized explicitly. No math or
orientation operation is mocked. Only the supplied PhysicsError callback and
CRT exit endpoint are controlled; actual PhysicsError body executes. Callback
code pointers are serialized as boolean because transport code addresses differ.
Native calls bridge raw scalar DWORDs directly with actual CDECL/fastcall ABI. Masked FPstatus/unmaskedtraps outside proof.
Only the documented default cyclic index table1,2,0 is initialized. Object/
actor arena aliases are exercised; aliases to independently transported math
globals or private callee frames are outside this bounded fixture.
"""
import argparse,hashlib,json,random,re,struct,subprocess,sys
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM,CS_OP_REG
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
paths.extend([ROOT/'scripts/verify_physics_rotation_reconstruction.py',ROOT/'scripts/inventory_reconstruction_backlog.py',ROOT/'scripts/verify_tinted_native_reconstruction.py',ROOT/'src/carma2/newcommon/globvrpb.h'])
if args.recompiled:paths.extend([args.recompiled,args.symbols,args.recompiled.with_suffix('.map')])
before={str(f):f.read_bytes()for f in paths};(OUT/'source-before.json').write_text(json.dumps({f:sha(b)for f,b in before.items()},indent=2))
data=(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes();assert sha(data)==HASH;nt,base,size,tab=pe(data);cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
ORIGINAL_ENTRIES=[('FUN_004b9230',0x4b9230,1030),('FUN_004b8ee0',0x4b8ee0,254),('TranslateObject',0x4b8fe0,585),('WriteNewPositions',0x4c2a10,246),('DrMatrix34ApplyLPInverse',0x4225b0,57),('PhysicsError',0x4c61c0,20),('BrMatrix34ApplyP',0x5334a0,116),('BrMatrix34ApplyV',0x533520,107),('BrMatrix34LPInverse',0x532eb0,175),('BrMatrix34PreRotate',0x533d40,63),('RotateObjectFirstOrder',0x4c1f10,330),('RotateObject',0x4c1be0,807),('__ftol',0x575b80,39),('BrMatrix34Rotate',0x532940,333),('BrMatrix34TApplyV',0x533690,107),('BrMatrix34PreTranslate',0x533dc0,159),('BrMatrix34PostTranslate',0x533e60,45),('BrMatrix34Pre',0x533700,40),('GetOrientationFromMatrix',0x49be90,356),('GetMatrixFromOrientation',0x49c000,200),('BrMatrix34Mul',0x532620,404),('BrMatrix34Copy',0x5325d0,79),('BrMatrix34ToQuat',0x535ae0,385),('BrQuatNormalise',0x535710,114),('BrQuatToMatrix34',0x5359a0,308)]
catalog={a:dict(name=name,body_bytes=n)for name,a,n in ORIGINAL_ENTRIES}
todo=[0x4b9230,0x4b8ee0];seen=set();spec=[];patches=[];constants={}
while todo:
 start=todo.pop(0)
 if start in seen:continue
 seen.add(start);row=catalog[start];name={0x49be90:'GetOrientationFromMatrix',0x49c000:'GetMatrixFromOrientation',0x4c1f10:'RotateObjectFirstOrder',0x4c1be0:'RotateObject'}.get(start,row['name']);n=row['body_bytes'];ins=list(cs.disasm(raw(data,start,n),start));assert sum(i.size for i in ins)==n
 for i in ins:
  for op in i.operands:
   addr=None;width=None
   if op.type==CS_OP_MEM and base<=op.mem.disp<base+size:
    addr=op.mem.disp;width=16 if op.mem.disp==0x4b9638 else 12 if op.mem.index or op.mem.base else op.size;site=i.address+i.disp_offset-base;assert i.disp_size==4
   elif op.type==CS_OP_IMM and i.mnemonic in('mov','push')and base<=op.imm<base+size:
    addr=op.imm;width=4;site=i.address+i.imm_offset-base;assert i.imm_size==4
   if addr is not None:
    absolute=False
    if 0x6b09c0<=addr<0x6b0a20:target=0x22003b00+(addr-0x6b09c0);absolute=True
    elif addr==0x68b918:target=0x2200f000;absolute=True
    elif addr==0x6940c0:target=0x2200f010;absolute=True
    elif addr==0x676920:target=0x2200f020;absolute=True
    else:
     section=next((struct.unpack_from('<I',data,tab+k*40+36)[0]for k in range(struct.unpack_from('<H',data,nt+6)[0])if base+struct.unpack_from('<I',data,tab+k*40+12)[0]<=addr<base+struct.unpack_from('<I',data,tab+k*40+12)[0]+max(struct.unpack_from('<I',data,tab+k*40+8)[0],struct.unpack_from('<I',data,tab+k*40+16)[0])),None)
     assert section is not None and (not section&0x80000000 or addr==0x66cc20 and width==12 and raw(data,addr,width)==words([1,2,0])),(hex(start),hex(addr),i.op_str)
     bits=raw(data,addr,width);constants[hex(addr)]=bits.hex();target=addr-base
    patches.append((site,addr,target,int(absolute)))
  if i.mnemonic=='jmp'and i.operands[0].type==CS_OP_MEM:
   assert start==0x4b9230 and i.operands[0].mem.disp==0x4b9638 and i.operands[0].mem.scale==4 and cs.reg_name(i.operands[0].mem.index)=='eax'
   table=struct.unpack('<4I',raw(data,0x4b9638,16));assert table==(0x4b93b6,0x4b94cb,0x4b94dc,0x4b93b6)
   for k,target in enumerate(table):patches.append((0x4b9638-base+4*k,target,target-base,0))
   continue
  if i.mnemonic=='call'or i.mnemonic.startswith('j'):
   if i.operands[0].type!=CS_OP_IMM:
    assert start==0x4c61c0 and i.mnemonic=='call'and cs.reg_name(i.operands[0].reg)=='eax';continue
   target=i.operands[0].imm
   if start<=target<start+n or target==0x5784e0:continue
   assert target in catalog,(name,i.op_str)
   todo.append(target)
 spec.append({'name':name,'address':start,'bytes':n,'sha256':sha(raw(data,start,n))})
(OUT/'original-addresses.bin').write_bytes(words([0x4b9230-base,0x4b8ee0-base,len(patches),0x1784e0])+b''.join(words(v)for v in patches))
COMMON='\n#include <windows.h>\n#include <stdio.h>\n#include <stdlib.h>\n#include <string.h>\n#include <float.h>\n#include <io.h>\n#include <fcntl.h>\n#include "49-physics.h"\n#include "60-phil.h"\n#include "globvrpb.h"\n#include "c2_math.h"\n#include <math.h>\nextern br_quat* C2_HOOK_CDECL BrQuatNormalise(br_quat*,const br_quat*);\nextern br_matrix34* C2_HOOK_CDECL BrQuatToMatrix34(br_matrix34*,const br_quat*);\ntNet_mode gNet_mode;\nbr_matrix34 mattmp1,mattmp2;\n#define M_DIAG(x) (((br_scalar*)mat->m)[4*(x)])\n#define Q_EL(e) (((br_scalar*)&(q->x))[(e)])\n#define M(x,y) mat->m[x][y]\n#define A(x,y) A->m[x][y]\n#define B(x,y) B->m[x][y]\n#define C(x,y) C->m[x][y]\nextern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison,builder_cdecl;\nextern unsigned __cdecl BuilderInvoke(void);\ntPhysicsError_cbfn* gPhysics_error_cb;char gPhysics_empty_message;\nstatic unsigned addresses[2],h[7],case_index;\nstatic unsigned char*heap;\ntypedef char layout_assert[sizeof(void*)==4&&sizeof(tPhysics_object)==1240&&offsetof(tPhysics_object,field_0x17c)==0x17c&&offsetof(br_actor,t)==0x28&&offsetof(br_transform,t)==4?1:-1];\nstatic void HierarchySnapshot(unsigned event,unsigned value,unsigned message){unsigned head[3],saved;head[0]=event;head[1]=value;head[2]=message;\n#ifndef PHYSICAL_IMAGE\nmemcpy(heap+0x3b00,&mattmp1,48);memcpy(heap+0x3b30,&mattmp2,48);memcpy(heap+0xf020,&gPhysics_empty_message,1);saved=(unsigned)gPhysics_error_cb;\n#else\nsaved=*(unsigned*)(heap+0xf010);\n#endif\n*(unsigned*)(heap+0xf010)=!!saved;fwrite(head,sizeof(head),1,stdout);fwrite(heap,65536,1,stdout);*(unsigned*)(heap+0xf010)=saved;}\nstatic void HierarchyErrorAction(tPhysicsError value,const char*message){HierarchySnapshot(1,(unsigned)value,message?0x100+*(const unsigned char*)message:0);if(h[4]&1){\n#ifndef PHYSICAL_IMAGE\ngPhysics_error_cb=NULL;\n#else\n*(unsigned*)(heap+0xf010)=0;\n#endif\n}if(h[4]&2)*(unsigned*)(heap+0x3c00)=0x22001900;}\nstatic void __fastcall HierarchyOriginalError(tPhysicsError value,const char*message){HierarchyErrorAction(value,message);}\nstatic void C2_HOOK_FASTCALL HierarchyNativeError(tPhysicsError value,const char*message){HierarchyErrorAction(value,message);}\nstatic void C2_NORETURN C2_HOOK_CDECL HierarchyExit(int value){HierarchySnapshot(2,(unsigned)value,0);exit(0);}\nstatic void invoke(unsigned mode,unsigned object,unsigned time){builder_mode=mode;builder_address=addresses[mode];builder_x=object;builder_y=mode?0xcccccccc:(unsigned)(heap+h[5]);builder_stack[0]=time;\n#if defined(REC2_MATCHING)\nbuilder_cdecl=0;builder_stack_count=1;\n#else\nbuilder_cdecl=1;builder_stack_count=3;builder_stack[0]=object;builder_stack[1]=builder_y;builder_stack[2]=time;\n#endif\nbuilder_poison=0xcccccccc;if(BuilderInvoke())exit(14);HierarchySnapshot(0,mode,0);}\nstatic int run(const char*path){FILE*f;SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);f=fopen(path,"rb");if(!f)return 10;heap=(unsigned char*)VirtualAlloc((void*)0x22000000,65536,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(heap!=(void*)0x22000000)return 11;_setmode(_fileno(stdout),_O_BINARY);for(case_index=0;fread(h,sizeof(h),1,f)==1;case_index++){if(fread(heap,65536,1,f)!=1)return 12;gNet_mode=(tNet_mode)h[3];memcpy(heap+0xf000,&h[3],4);memcpy(&mattmp1,heap+0x3b00,48);memcpy(&mattmp2,heap+0x3b30,48);gPhysics_empty_message=heap[0xf020];\n#ifndef PHYSICAL_IMAGE\ngPhysics_error_cb=h[4]&4?HierarchyNativeError:NULL;(void)HierarchyOriginalError;\n#else\n*(unsigned*)(heap+0xf010)=h[4]&4?(unsigned)HierarchyOriginalError:0;(void)HierarchyNativeError;\n#endif\n_controlfp(_PC_53,_MCW_PC);_clearfp();if(h[0]<2)invoke(h[0],h[1],h[2]);else{invoke(0,h[1],h[2]);invoke(1,h[1],h[2]);}}fclose(f);return 0;}\n\n'
caller,physical,template=registry.literals();old=' cmp builder_mode,8\n jne notwide\n fstp qword ptr builder_wide\n xor eax,eax\n notwide:\n cmp builder_mode,1\n je zeroreturn\n cmp builder_mode,10\n jne resultready\n zeroreturn:\n xor eax,eax\n resultready:';assert old in caller;caller=caller.replace(old,' xor eax,eax')
old='image[hdr[12]]=0xe9;relative=(unsigned)RayBoundary-((unsigned)image+hdr[12]+5);memcpy(image+hdr[12]+1,&relative,4);';assert old in physical;physical=physical.replace(old,'image[hdr[3]]=0xe9;relative=(unsigned)HierarchyExit-((unsigned)image+hdr[3]+5);memcpy(image+hdr[3]+1,&relative,4);').replace('hdr[14]','hdr[4]').replace('n=hdr[13]','n=hdr[2]').replace('i<12;i++)addresses','i<2;i++)addresses')
caller=caller.replace('builder_poison;','builder_poison,builder_cdecl;').replace(' mov builder_result,eax\n mov ecx,esp',' mov builder_result,eax\n cmp builder_cdecl,0\n je hierarchy_stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n hierarchy_stackready:\n mov ecx,esp')
body='\n'.join(extract(before[str(physics)].decode(),name)for name in ('PhysicsJointScalar','PhysicsWallNaN','PhysicsJointSum','PhysicsJointProductMemory','PartnerReverseSubtractScalar','PartnerMultiplyRawScalar','PhysicsJointAddScalar','PartnerDifferenceScalars','VelocityScalar','VelocitySubtractScalar','RotationArgument'))+'\n'
body+=extract(before[str(matrix)].decode(),'BrMatrix34Translate')+'\n'

def lower_capsule(text,name,helper):
 clean=mask_c(text);body=extract(text,name);start=text.index(body)
 hits=list(re.finditer(r'^static [^\n;{}]*\b'+re.escape(helper)+r'\([^;{}\n]*\)\s*\{',clean,re.M))
 assert len(hits)==1,(helper,len(hits));assert hits[0].start()<start
 return text[hits[0].start():start]+body+('\n#undef MR_PRODUCT\n#undef MR_SUM\n#undef MR_DIFFERENCE\n'if name=='BrMatrix34Rotate'else'\n')
for row in spec:
 name=row['name']
 if row['address']in(0x4b9230,0x4b8ee0,0x575b80):continue
 if name=='BrMatrix34Rotate'and args.matrix_rotate_candidate:
  body+=before[str(args.matrix_rotate_candidate.resolve())].decode()+'\n';continue
 if name=='BrMatrix34PostTranslate'and args.matrix_post_candidate:
  body+=before[str(args.matrix_post_candidate.resolve())].decode()+'\n';continue
 source=physics if name in('TranslateObject','WriteNewPositions','DrMatrix34ApplyLPInverse','RotateObject','RotateObjectFirstOrder','PhysicsError')else matrix if name.startswith('BrMatrix')and name!='BrMatrix34ToQuat' else quat if name.startswith('BrQuat')or name=='BrMatrix34ToQuat' else phil
 if name in('BrMatrix34Rotate','BrMatrix34PostTranslate'):
  body+=lower_capsule(before[str(source)].decode(),name,'MatrixRotateScalar'if name=='BrMatrix34Rotate'else'MatrixPostTranslateAdd')
 elif name=='PhysicsError':body+='\n#define exit HierarchyExit\n'+extract(before[str(source)].decode(),name)+'\n#undef exit\n'
 else:body+=extract(before[str(source)].decode(),name)+'\n'
for name in('PhilOrientationScalar',):body=extract(before[str(phil)].decode(),name)+'\n'+body
for name in('QuaternionSum','QuaternionScaleRaw','QuaternionScaleLoaded','QuaternionMultiply','QuaternionAddScalar','QuaternionSubtractScalar','QuaternionScalar'):body=extract(before[str(quat)].decode(),name)+'\n'+body
rotation_text=before[str(candidate)].decode().replace('\r\n','\n')
hierarchy_first=extract(rotation_text,'FUN_004b9230');hierarchy_last=extract(rotation_text,'FUN_004b8ee0')
marker='/* Original calls push the time DWORD with MOV/PUSH.'
assert rotation_text.count(marker)==1
hierarchy_start=rotation_text.index(marker);hierarchy_end=rotation_text.index(hierarchy_last)+len(hierarchy_last)
assert hierarchy_start<rotation_text.index(hierarchy_first)<rotation_text.index(hierarchy_last)
body+='\n'+rotation_text[hierarchy_start:hierarchy_end]+'\n#undef HIERARCHY_RECURSE\n#undef HIERARCHY_MOTION\n'
native=COMMON+'\n'+body+r'''
int C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;addresses[0]=(unsigned)FUN_004b9230;addresses[1]=(unsigned)FUN_004b8ee0;return run(argv[1]);}
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
 global_fallbacks=[]
 if 0x676920 not in symbols:
  # This one-byte common/BSS symbol can be absent from the report-derived map.
  # Resolve its exact canonical linker symbol, retaining the original identity.
  assert re.search(r'// GLOBAL: CARMA2_HW 0x00676920\s+char gPhysics_empty_message;',before[str(physics)].decode())
  rows=re.findall(r'^\s*0003:[0-9a-fA-F]{8}\s+_gPhysics_empty_message\s+([0-9a-fA-F]{8})\s+<common>\s*$',map_bytes.decode('latin1'),re.M)
  assert len(rows)==1,rows
  address=int(rows[0],16);region=next(s for s in sections if s[0]<=address<s[0]+s[1])
  assert region[2]&0x80000000 and not region[2]&0x20000000
  symbols[0x676920]=address
  global_fallbacks.append(dict(original=0x676920,recompiled=address,name='_gPhysics_empty_message',owner='<common>',bytes=1,authority='Exact canonical linker common symbol and original-address annotated char BSS declaration.'))
 initial={symbols[a]:name for name,a,_ in ORIGINAL_ENTRIES if a in symbols};roots=[symbols[0x4b9230],symbols[0x4b8ee0]]
 mutable={symbols[0x6b09c0]:(0x22003b00,48),symbols[0x6b09f0]:(0x22003b30,48),symbols[0x68b918]:(0x2200f000,4),symbols[0x6940c0]:(0x2200f010,4),symbols[0x676920]:(0x2200f020,1)}
 exit_address=symbols.get(0x5784e0)
 if exit_address is None:
  exits=[a for a,(n,o)in functions.items()if n=='_exit'and o.startswith('LIBC:')];assert len(exits)==1;exit_address=exits[0]
 linked_tables=[];linked_callbacks=[]
 todo=list(roots);seen=set();linked_graph=[];linked_patches={};linked_constants={}
 for start in todo:
  if start in seen:continue
  seen.add(start);assert start in functions,hex(start)
  end=min(a for a in functions if a>start);name=initial.get(start,functions[start][0]);owner=functions[start][1]
  if start not in initial:
   plain=name.lstrip('_@').split('@')[0]
   assert owner in('49-physics.c.obj','60-phil.c.obj','matrix34.c.obj','quat.c.obj','libbrender:matrix34.c.obj','libbrender:quat.c.obj')and plain.startswith(('PhysicsJoint','PhysicsWallNaN','RotationArgument','MatrixRotate','MatrixPostTranslate','PhilOrientation','Quaternion','Partner','Velocity'))or plain in('ftol','_ftol')and owner=='LIBC:ftol.obj',(name,owner)
  pending=[start];instructions={};callback_loads={}
  while pending:
   location=pending.pop()
   while location not in instructions:
    assert start<=location<end,(name,hex(location))
    ins=next(cs.disasm(raw(linked_data,location,15),location));instructions[location]=ins.bytes.hex()
    if ins.mnemonic=='mov'and ins.operands[0].type==CS_OP_REG and ins.operands[1].type==CS_OP_MEM and ins.operands[1].mem.disp==symbols[0x6940c0]:callback_loads[ins.operands[0].reg]=ins.address
    is_table=ins.mnemonic=='jmp'and ins.operands[0].type==CS_OP_MEM and bool(ins.operands[0].mem.index)
    if is_table:
     operand=ins.operands[0];assert start==symbols[0x4b9230]and operand.mem.scale==4 and not operand.mem.base
     targets=struct.unpack('<4I',raw(linked_data,operand.mem.disp,16));assert all(start<=t<end for t in targets)
     pending.extend(targets)
     for k,target in enumerate(targets):
      site=operand.mem.disp-base2+4*k;linked_patches[site]=(site,target,target-base2,0)
     linked_tables.append(dict(function=name,address=operand.mem.disp,index_domain=[0,1,2,3],target_offsets=[t-start for t in targets],authority='Original and source unsigned(type-1)<=3 bounds four declared joint kinds before indexed dispatch.'))
    elif ins.mnemonic=='call'or ins.mnemonic.startswith('j'):
     if ins.operands[0].type!=CS_OP_IMM:
      # MSVC5 may inline the complete PhysicsError callback/exit sequence in
      # the recursive entry. The controlled callback is still the fresh load
      # of the same canonical mutable callback global, not an arbitrary call.
      assert start in(symbols[0x4c61c0],symbols[0x4b9230])and ins.mnemonic=='call'and ins.operands[0].type==CS_OP_REG and ins.operands[0].reg in callback_loads and ins.address-callback_loads[ins.operands[0].reg]<=64,(name,hex(ins.address),ins.mnemonic,ins.op_str,callback_loads)
      linked_callbacks.append(dict(function=name,address=ins.address,load=callback_loads[ins.operands[0].reg],global_address=symbols[0x6940c0],register=cs.reg_name(ins.operands[0].reg)))
     else:
      target=ins.operands[0].imm
      if start<=target<end:
       if ins.mnemonic!='call':pending.append(target)
       else:assert start in(symbols[0x4b9230],symbols[0x4c2a10])and target==start,('Unexpected local call',name,ins.op_str)
      elif target!=exit_address:
       assert target in functions,(name,ins.op_str);todo.append(target)
    for op in ins.operands:
     value=None;site=None;width=4
     if op.type==CS_OP_MEM and base2<=op.mem.disp<base2+size2:
      value=op.mem.disp;site=ins.address+ins.disp_offset-base2;width=16 if is_table else 12 if op.mem.base or op.mem.index else op.size;assert ins.disp_size==4
     elif op.type==CS_OP_IMM and ins.mnemonic in('mov','push')and base2<=op.imm<base2+size2:
      value=op.imm;site=ins.address+ins.imm_offset-base2;assert ins.imm_size==4
     if value is not None:
      region=next(s for s in sections if s[0]<=value<s[0]+s[1]);absolute=False
      head=next((a for a,(target,length)in mutable.items()if a<=value<a+length),None)
      if head is not None:target=mutable[head][0]+value-head;absolute=True
      elif is_table:
       target=value-base2
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
 patch_path=OUT/'matching-addresses.bin';patch_path.write_bytes(words([r-base2 for r in roots]+[len(linked_patches),exit_address-base2])+b''.join(words(v)for _,v in sorted(linked_patches.items())))
 matching=dict(recompiled_sha256=sha(linked_data),symbols_sha256=sha(args.symbols.read_bytes()),map_sha256=sha(map_bytes),global_fallbacks=global_fallbacks,callback_sites=linked_callbacks,graph=linked_graph,constants=linked_constants,patches=list(linked_patches.values()),relocations_checked=checked,allocator_tables=linked_tables,crt_exit=exit_address,mutable_globals=mutable,patch_file_sha256=sha(patch_path.read_bytes()))
cases=[]
def add(**items):
 c=dict(mode=0,network=0,seed=len(cases)+1,time=0x3d23d70a,tree=0,first=0,second=None,fixed=False,patches=[],selected=0x3c00,callback=0,actor_matrix_at=None);c.update(items);cases.append(c)
for mode in(0,1):
 for first in(0,1,2,3,4):
  for second in(None,0,5):
   for seed in range(4):add(mode=mode,first=first,second=second,seed=seed+1,tree=seed)
for mode in(0,1):
 for first in(1,2,3,4):
  for tree in range(4):add(mode=mode,first=first,fixed=True,tree=tree,second=5)
if args.expanded:
 pool=(0,0x80000000,1,0x80000001,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc12345,0xffc12345,0x7f812345,0xff812345)
 for mode in(0,1):
  for first in(0,1,2,3,4):
   for time in pool:add(mode=mode,first=first,time=time,tree=1)
 # Field sets exercise the no-joint, hinge and fixed-joint arithmetic paths.
 for mode in(0,1):
  for first,fixed,fields in[(0,False,(0x168,0x16c,0x170,0x174,0x178,0x17c)),(1,False,(0x174,0x178,0x17c,0x4038,0x403c,0x4040)),(1,True,(0x114,0x118,0x11c,0x174,0x178,0x17c))]:
   for field in fields:
    for bits in pool:add(mode=mode,first=first,fixed=fixed,patches=[(field,bits)])
 tied=((0x7fc12345,0xffc12345),(0xffc12345,0x7fc12345),(0x7f812345,0xff812345),(0xff812345,0x7f812345),(0x7f812345,0xffc12345),(0xffc12345,0x7f812345))
 for mode in(0,1):
  for first,fixed in[(0,False),(1,False),(1,True)]:
   for lanes in[(0x168,0x16c),(0x174,0x178),(0x174,0x17c),(0x178,0x17c),(0x174,0x4038),(0x178,0x403c)]:
    for pair in tied:add(mode=mode,first=first,fixed=fixed,tree=1,patches=list(zip(lanes,pair)))
 for mode in(0,1):
  for first,fixed in[(0,False),(1,False),(2,False),(1,True)]:
   for at in(0x114,0x168,0x18c,0x2e8):
    for seed in(0,7):add(mode=mode,first=first,fixed=fixed,actor_matrix_at=at,seed=seed,second=0)
 # Selected child/sibling vs ancestor, including type0 secondary joints.
 for mode in(0,1):
  for chosen in(0x900,0x1100):
   for second in(0,5):
    secondary=0x4000+(chosen-0x100)//0x800*0x100+0x80
    add(mode=mode,tree=2,first=1,patches=[(chosen+0x18c,0x22000000+secondary),(secondary,second)])
 for at in(0x288,0x28c,0x324,0x334):add(mode=0,tree=2,first=1,second=0,selected=at)
 for mode in(0,1):
  add(mode=mode,first=0,patches=[(0x288,0)])
 for secondary in(0,):add(mode=0,first=0,second=secondary,patches=[(0x288,0)])
 # Isolate two-NaN multiplication and memory-add selection in the linear path.
 for mode in(0,1):
  for lane in range(3):
   for pair in tied:
    zero_omega=[(0x174,0),(0x178,0),(0x17c,0)]
    add(mode=mode,first=0,time=pair[1],patches=zero_omega+[(0x168+lane*4,pair[0])])
    add(mode=mode,first=0,time=0x3f800000,patches=zero_omega+[(0x168+lane*4,pair[0]),(0x5050+lane*4,pair[1])])
 # Execute the actual PhysicsError body, with supplied callback+CRTexit only.
 for mode in(0,1):
  for callback in(0,4,5,6,7):
   for bits in(0,0x7f):
    add(mode=mode,first=1,callback=callback,patches=[(0x330,0),(0xf020,bits)])
    add(mode=mode,first=5,callback=callback,patches=[(0xf020,bits)])
def make_case(c):
 rng=random.Random(c['seed']);b=bytearray(words([rng.getrandbits(32)for _ in range(16384)]));objects=(0x100,0x900,0x1100,0x1900);actors=(0x5000,0x5100,0x5200,0x5300)
 def put(offset,*values):struct.pack_into('<'+'I'*len(values),b,offset,*[v&0xffffffff for v in values])
 def vec(offset,values):struct.pack_into('<'+'f'*len(values),b,offset,*values)
 for i,obj in enumerate(objects):
  actor=(c['actor_matrix_at']-0x2c)if i==0 and c['actor_matrix_at']is not None else actors[i];joint=0x4000+0x100*i
  vec(obj+8,(1,2,3));vec(obj+0x14,(1,2,3));vec(obj+0x68,tuple(rng.uniform(-.2,.2)for _ in range(3)));vec(obj+0x74,tuple(rng.uniform(-.2,.2)for _ in range(3)));vec(obj+0x17c,(1,2,3));vec(obj+0x1c0,(.1,.2,.3))
  identity=(1,0,0,0,1,0,0,0,1,1+i,2+i,3+i)
  vec(obj+0x8c,identity);vec(obj+0x1e8,identity);vec(actor+0x2c,identity)
  vec(joint+8,(.1,.2,.3));vec(joint+0x14,(.2,.3,.4));vec(joint+0x38,(0,1,0));put(joint,c['first']if i==0 else 1)
  vec(joint+0x80,(.1,.2,.3));vec(joint+0x80+0x38,(0,1,0));put(joint+0x80,c['second']or 0)
  put(obj,0x22000000+actor);put(obj+0x188,0x22000000+joint,0 if c['second']is None else 0x22000000+joint+0x80);put(obj+0x19c,0x400 if c['fixed']and i==0 else 0);put(obj+0x220,0,0,0x22000000+objects[3],0,0 if i==3 else 0x22000000+objects[3])
 if c['tree']>=1:put(objects[0]+0x224,0x22000000+objects[1])
 if c['tree']>=2:put(objects[1]+0x220,0x22000000+objects[2])
 if c['tree']>=3:put(objects[1]+0x224,0x22000000+objects[3]);put(0x4300,0)
 for obj in objects[1:3]:put(obj+0x230,0x22000000+objects[0]);put(obj+0x228,0x22000000+objects[0])
 put(objects[3]+0x18c,0);put(0x3c00,0);put(0xf010,0);b[0xf020]=0
 for offset,value in c['patches']:put(offset,value)
 return words([c['mode'],0x22000100,c['time'],c['network'],c['callback'],c['selected'],c['seed']])+b
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
