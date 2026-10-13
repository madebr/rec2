"""Complete original object preparation and ordered-list/wake consumers, scratch zero credit.

Actual original game bytes execute physically at PC53, compared with native
MSVC5, modern MSVC and MinGW. Every movement/math/allocation/error lower body
is authentic. Full arenas, mutable globals, callback snapshots and x86 ABI
are checked raw except the documented callback-pointer presence and message
first-byte serialization. Original terminal access faults
have full-state observation but no normal-return ABI claim.
"""
import argparse,hashlib,json,random,re,struct,subprocess,sys
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM,CS_OP_REG
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'src/carma2').is_dir());sys.path.insert(0,str(ROOT/'scripts'))
from verify_tinted_native_reconstruction import extract
from verify_opponent_route_reconstruction import HASH,pe,raw,sha,words
import verify_phil_registry_reconstruction as registry
from verify_physics_joint_application_reconstruction import workspace_block
from inventory_reconstruction_backlog import mask_c

p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',required=True,type=Path);p.add_argument('--limit',type=int);p.add_argument('--case-index',type=int,action='append');p.add_argument('--candidate-source',type=Path);p.add_argument('--overlap-source',type=Path);p.add_argument('--bounds-source',type=Path);p.add_argument('--matrix-rotate-candidate',type=Path);p.add_argument('--matrix-post-candidate',type=Path);p.add_argument('--expanded',action='store_true');p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);args=p.parse_args();OUT=args.output.resolve();OUT.mkdir(exist_ok=True,parents=True)
assert bool(args.recompiled)==bool(args.symbols),'Supply both immutable PE and hash-bound symbol map'
assert args.limit is None or args.limit>0
physics=ROOT/'src/carma2/newcommon/49-physics.c';candidate=args.candidate_source.resolve()if args.candidate_source else physics;overlap=args.overlap_source.resolve()if args.overlap_source else physics;geometry=ROOT/'src/carma2/newcommon/34-finteray.c';matrix=ROOT/'src/brender/core/math/matrix34.c';phil=ROOT/'src/carma2/newcommon/60-phil.c';quat=ROOT/'src/brender/core/math/quat.c'
paths=[candidate,overlap,geometry,physics,matrix,phil,quat,Path(__file__),ROOT/'scripts/verify_physics_joint_application_reconstruction.py',ROOT/'scripts/verify_physics_joint_translation_reconstruction.py',ROOT/'scripts/verify_phil_registry_reconstruction.py',ROOT/'scripts/verify_opponent_navigation_reconstruction.py',ROOT/'src/carma2/newcommon/49-physics.h',ROOT/'src/carma2/newcommon/60-phil.h',ROOT/'src/carma2/include/rec2_types.h',ROOT/'src/carma2/hooks/c2_hooks.h',ROOT/'src/carma2/hooks/c2_math.h',ROOT/'src/brender/core/math/matrix34.h',ROOT/'src/brender/include/brender/brender.h',ROOT/'src/brender/include/brender/br_defs.h',ROOT/'src/brender/include/brender/br_types.h']
if args.matrix_rotate_candidate:paths.append(args.matrix_rotate_candidate.resolve())
if args.matrix_post_candidate:paths.append(args.matrix_post_candidate.resolve())
paths.extend([ROOT/'scripts/verify_physics_rotation_reconstruction.py',ROOT/'scripts/inventory_reconstruction_backlog.py',ROOT/'scripts/verify_tinted_native_reconstruction.py',ROOT/'scripts/verify_opponent_route_reconstruction.py',ROOT/'scripts/verify_physics_jointed_object_reconstruction.py',ROOT/'src/carma2/newcommon/globvrpb.h',ROOT/'src/brender/core/math/vector.c'])
if args.recompiled:paths.extend([args.recompiled,args.symbols,args.recompiled.with_suffix('.map')])
bounds_candidate=args.bounds_source.resolve()if args.bounds_source else physics;paths.append(bounds_candidate)
before={str(f):f.read_bytes()for f in paths};(OUT/'source-before.json').write_text(json.dumps({f:sha(b)for f,b in before.items()},indent=2))
snapshot_dir=OUT/'source-snapshot';snapshot_dir.mkdir(exist_ok=True)
source_snapshots={}
for index,(path,bits)in enumerate(before.items()):
 snapshot=snapshot_dir/(str(index)+'-'+Path(path).name);snapshot.write_bytes(bits);source_snapshots[path]=dict(path=str(snapshot),sha256=sha(bits))
data=(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes();assert sha(data)==HASH;nt,base,size,tab=pe(data);cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
ORIGINAL_ENTRIES=[('MoveJointedObject', 4945168, 662), ('FUN_004b9750', 4953936, 28), ('TranslateObject', 4952032, 585), ('WriteNewPositions', 4991504, 246), ('DrMatrix34ApplyLPInverse', 4335024, 57), ('PhysicsError', 5005760, 20), ('BrMatrix34ApplyP', 5452960, 116), ('BrMatrix34ApplyV', 5453088, 107), ('BrMatrix34LPInverse', 5451440, 175), ('BrMatrix34PreRotate', 5455168, 63), ('RotateObjectFirstOrder', 4988688, 330), ('RotateObject', 4987872, 807), ('__ftol', 5725056, 39), ('BrMatrix34Rotate', 5450048, 333), ('BrMatrix34TApplyV', 5453456, 107), ('BrMatrix34PreTranslate', 5455296, 159), ('BrMatrix34PostTranslate', 5455456, 45), ('BrMatrix34Pre', 5453568, 40), ('GetOrientationFromMatrix', 4832912, 356), ('GetMatrixFromOrientation', 4833280, 200), ('BrMatrix34Mul', 5449248, 404), ('BrMatrix34Copy', 5449168, 79), ('BrMatrix34ToQuat', 5462752, 385), ('BrQuatNormalise', 5461776, 114), ('BrQuatToMatrix34', 5462432, 308), ('FUN_004b9230', 4952624, 1030), ('FUN_004b8ee0', 4951776, 254), ('ApplySingleForce', 4946272, 2534), ('RedoQuickHingeVelocities', 4951280, 287), ('AllocateObjectWorkspace', 4948816, 2237), ('DRMatrix33Inverse', 4973392, 352), ('SetUpForce', 4973744, 159), ('SetUpTorqueHingePartners', 4973904, 1943), ('SetUpHingePartners', 4975856, 1942), ('MakeMatrix', 4977808, 3922), ('BrVector3SetFloat', 5457296, 25), ('MakeJointVelocitiesVector', 4945840, 425), ('CreateJointForce', 4951072, 201), ('CreateJointTorque', 4951568, 198), ('GetPhysicsScratchPointer', 5004400, 58), ('IncreasePhysicsScratchPointer', 5004464, 70), ('CountJointForces', 4953680, 222), ('HierarchyCalcEnergy', 4944528, 127), ('RQDcmpV', 5003312, 792), ('SparseRQDcmpV', 5002160, 1141), ('MatrixProcess', 5004112, 13), ('RqBackSub', 5004128, 258)]
WAKE_ENTRIES=[('FUN_004bae80',0x4bae80,57),('FUN_004bddd0',0x4bddd0,784),('CheckForObjectHierachyTouchingObjectList',0x4ba190,850),('FUN_004be0e0',0x4be0e0,616)]
ORIGINAL_ENTRIES+=WAKE_ENTRIES+[('MakeObjectListDoSomething',0x4b9f40,67)]
PREP_ENTRIES=[('InternalPrepareObject',0x4b99e0,760),('PrepareObject',0x4b9770,624)]
ORIGINAL_ENTRIES+=PREP_ENTRIES+[('CheckForObjectHierachyTouchingAnotherObject',0x4b9f90,509),('GetWorldBoundingBoxForObject',0x4b9ce0,339),('GetNewBoundingBox',0x45f5b0,213),('BrMatrix34LPNormalise',0x532f60,411),('FUN_004c61e0',0x4c61e0,1)]
catalog={a:dict(name=name,body_bytes=n)for name,a,n in ORIGINAL_ENTRIES}
todo=[a for _,a,_ in PREP_ENTRIES];seen=set();spec=[];patches=[];constants={}
while todo:
 start=todo.pop(0)
 if start in seen:continue
 seen.add(start);row=catalog[start];name={0x49be90:'GetOrientationFromMatrix',0x49c000:'GetMatrixFromOrientation',0x4c1f10:'RotateObjectFirstOrder',0x4c1be0:'RotateObject'}.get(start,row['name']);n=row['body_bytes'];ins=list(cs.disasm(raw(data,start,n),start));assert sum(i.size for i in ins)==n
 for i in ins:
  for op in i.operands:
   addr=None;width=None
   if op.type==CS_OP_MEM and base<=op.mem.disp<base+size:
    addr=op.mem.disp;width=20 if op.mem.disp==0x4b9730 else 16 if op.mem.disp in(0x4b9638,0x4b8c10) else 12 if op.mem.index or op.mem.base else op.size;site=i.address+i.disp_offset-base;assert i.disp_size==4
   elif op.type==CS_OP_IMM and i.mnemonic in('mov','push')and base<=op.imm<base+size:
    addr=op.imm;width=4;site=i.address+i.imm_offset-base;assert i.imm_size==4
   if addr is not None:
    absolute=False
    if 0x6b09c0<=addr<0x6b0a20:target=0x22003b00+(addr-0x6b09c0);absolute=True
    elif addr in(0x692dcc,0x6940a8,0x6940ac,0x692dd0,0x692dd4):target={0x692dcc:0x2200f060,0x6940a8:0x2200f064,0x6940ac:0x2200f068,0x692dd0:0x2200f06c,0x692dd4:0x2200f070}[addr];absolute=True
    elif addr in(0x6940a0,0x65d010):target={0x6940a0:0x2200f034,0x65d010:0x2200f038}[addr];absolute=True
    elif addr==0x68b918:target=0x2200f000;absolute=True
    elif addr==0x6940c0:target=0x2200f010;absolute=True
    elif addr==0x676920:target=0x2200f020;absolute=True
    elif addr in(0x6940bc,0x6940c4,0x6940c8,0x692dc8):target={0x6940bc:0x2200f024,0x6940c4:0x2200f028,0x6940c8:0x2200f02c,0x692dc8:0x2200f030}[addr];absolute=True
    else:
     section=next((struct.unpack_from('<I',data,tab+k*40+36)[0]for k in range(struct.unpack_from('<H',data,nt+6)[0])if base+struct.unpack_from('<I',data,tab+k*40+12)[0]<=addr<base+struct.unpack_from('<I',data,tab+k*40+12)[0]+max(struct.unpack_from('<I',data,tab+k*40+8)[0],struct.unpack_from('<I',data,tab+k*40+16)[0])),None)
     assert section is not None and (not section&0x80000000 or addr==0x66cc20 and width==12 and raw(data,addr,width)==words([1,2,0]) or addr==0x65d01c and raw(data,addr,22)==b'Stupid trial position\0'),(hex(start),hex(addr),i.op_str)
     bits=raw(data,addr,width);constants[hex(addr)]=bits.hex();target=addr-base
    patches.append((site,addr,target,int(absolute)))
  if i.mnemonic=='jmp'and i.operands[0].type==CS_OP_MEM:
   assert start in(0x4b9230,0x4b8350,0x4b9650) and i.operands[0].mem.disp in(0x4b9638,0x4b8c10,0x4b9730) and i.operands[0].mem.scale==4
   at=i.operands[0].mem.disp;count=5 if at==0x4b9730 else 4;table=struct.unpack('<'+str(count)+'I',raw(data,at,4*count));assert all(start<=t<start+n for t in table)
   for k,target in enumerate(table):patches.append((at-base+4*k,target,target-base,0))
   continue
  if i.mnemonic=='call'or i.mnemonic.startswith('j'):
   if i.operands[0].type!=CS_OP_IMM:
    assert start in(0x4c61c0,0x4c5c70,0x4c5cb0) and i.mnemonic=='call'and cs.reg_name(i.operands[0].reg)=='eax';continue
   target=i.operands[0].imm
   if start<=target<start+n or target==0x5784e0:continue
   assert target in catalog,(name,i.op_str)
   todo.append(target)
 spec.append({'name':name,'address':start,'bytes':n,'sha256':sha(raw(data,start,n))})
(OUT/'original-addresses.bin').write_bytes(words([a-base for _,a,_ in PREP_ENTRIES]+[len(patches),0x1784e0])+b''.join(words(v)for v in patches))
COMMON='\n#include <windows.h>\n#include <stdio.h>\n#include <stdlib.h>\n#include <string.h>\n#include <float.h>\n#include <io.h>\n#include <fcntl.h>\n#include "49-physics.h"\n#include "60-phil.h"\n#include "globvrpb.h"\n#include "c2_math.h"\n#include <math.h>\nextern br_quat* C2_HOOK_CDECL BrQuatNormalise(br_quat*,const br_quat*);\nextern br_matrix34* C2_HOOK_CDECL BrQuatToMatrix34(br_matrix34*,const br_quat*);\ntNet_mode gNet_mode;\nbr_matrix34 mattmp1,mattmp2;\n#define M_DIAG(x) (((br_scalar*)mat->m)[4*(x)])\n#define Q_EL(e) (((br_scalar*)&(q->x))[(e)])\n#define M(x,y) mat->m[x][y]\n#define A(x,y) A->m[x][y]\n#define B(x,y) B->m[x][y]\n#define C(x,y) C->m[x][y]\nextern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison,builder_cdecl;\nextern unsigned __cdecl BuilderInvoke(void);\ntPhysicsError_cbfn* gPhysics_error_cb;char gPhysics_empty_message;void* gPhysics_scratch_ptr;void* gPhysics_other_buffer;int gPhysics_other_buffer_capacity;tU32 gPhysics_joints_changed_692dc8;\nint gFace_num__car; tU32 gPhysics_group_counter_6940a0;\nstatic unsigned addresses[4],h[7],case_index;\nstatic unsigned char*heap;\ntypedef char layout_assert[sizeof(void*)==4&&sizeof(tPhysics_object)==1240&&offsetof(tPhysics_object,field_0x17c)==0x17c&&offsetof(br_actor,t)==0x28&&offsetof(br_transform,t)==4?1:-1];\nstatic void HierarchySnapshot(unsigned event,unsigned value,unsigned message){unsigned head[3],saved;head[0]=event;head[1]=value;head[2]=message;\n#ifndef PHYSICAL_IMAGE\nmemcpy(heap+0x3b00,&mattmp1,48);memcpy(heap+0x3b30,&mattmp2,48);memcpy(heap+0xf020,&gPhysics_empty_message,1);memcpy(heap+0xf024,&gPhysics_scratch_ptr,4);memcpy(heap+0xf028,&gPhysics_other_buffer,4);memcpy(heap+0xf02c,&gPhysics_other_buffer_capacity,4);memcpy(heap+0xf030,&gPhysics_joints_changed_692dc8,4);memcpy(heap+0xf034,&gPhysics_group_counter_6940a0,4);memcpy(heap+0xf038,&gFace_num__car,4);saved=(unsigned)gPhysics_error_cb;\n#else\nsaved=*(unsigned*)(heap+0xf010);\n#endif\n*(unsigned*)(heap+0xf010)=!!saved;fwrite(head,sizeof(head),1,stdout);fwrite(heap,65536,1,stdout);*(unsigned*)(heap+0xf010)=saved;}\nstatic void HierarchyErrorAction(tPhysicsError value,const char*message){HierarchySnapshot(1,(unsigned)value,message?0x100+*(const unsigned char*)message:0);if(h[4]&1){\n#ifndef PHYSICAL_IMAGE\ngPhysics_error_cb=NULL;\n#else\n*(unsigned*)(heap+0xf010)=0;\n#endif\n}if(h[4]&2)*(unsigned*)(heap+0x3c00)=0x22001900;\nif(h[4]&8){\n#ifndef PHYSICAL_IMAGE\ngPhysics_group_counter_6940a0=*(unsigned*)(heap+0xf040);\n#else\n*(unsigned*)(heap+0xf034)=*(unsigned*)(heap+0xf040);\n#endif\n}\nif(h[4]&16){\n#ifndef PHYSICAL_IMAGE\nmemcpy(&gFace_num__car,heap+0xf044,4);\n#else\nmemcpy(heap+0xf038,heap+0xf044,4);\n#endif\n}\nif(h[4]&32)memcpy(heap+*(unsigned*)(heap+0xf048),heap+0xf04c,4);\n}\nstatic void __fastcall HierarchyOriginalError(tPhysicsError value,const char*message){HierarchyErrorAction(value,message);}\nstatic void C2_HOOK_FASTCALL HierarchyNativeError(tPhysicsError value,const char*message){HierarchyErrorAction(value,message);}\nstatic void C2_NORETURN C2_HOOK_CDECL HierarchyExit(int value){HierarchySnapshot(2,(unsigned)value,0);exit(0);}\nstatic void invoke(unsigned mode,unsigned object,unsigned label){\nbuilder_mode=mode;builder_address=addresses[mode];builder_x=object;\nbuilder_y=mode==0?label:h[5]?(unsigned)(heap+h[5]):0;\nbuilder_stack[0]=h[6]?(unsigned)(heap+h[6]):0;\n#if defined(REC2_MATCHING)\nbuilder_cdecl=0;builder_stack_count=mode==2?1:0;\n#else\nbuilder_cdecl=1;builder_stack_count=mode==2?3:2;\nbuilder_stack[2]=builder_stack[0];builder_stack[0]=object;builder_stack[1]=builder_y;\n#endif\nbuilder_poison=0xcccccccc;if(BuilderInvoke())exit(14);HierarchySnapshot(0,mode,0);}\n/* Observe actual access-fault public state; terminal paths have no return-ABI claim. */\nstatic LONG WINAPI JointedFault(EXCEPTION_POINTERS* exception) {\n    if (exception->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION) return EXCEPTION_CONTINUE_SEARCH;\n    HierarchySnapshot(3, (unsigned)exception->ExceptionRecord->ExceptionCode, (unsigned)exception->ExceptionRecord->ExceptionInformation[1]);\n    fflush(stdout);\n    ExitProcess(0);\n    return EXCEPTION_CONTINUE_SEARCH;\n}\nstatic void InstallJointedFaultObserver(void) {\n    typedef void* (WINAPI *add_handler)(ULONG, LONG (WINAPI*)(EXCEPTION_POINTERS*));\n    FARPROC address = GetProcAddress(GetModuleHandleA("kernel32.dll"), "AddVectoredExceptionHandler");\n    add_handler function;\n    memcpy(&function, &address, sizeof(function));\n    if (function == NULL || function(1, JointedFault) == NULL) exit(38);\n}\nstatic int run(const char*path){FILE*f;SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);f=fopen(path,"rb");if(!f)return 10;heap=(unsigned char*)VirtualAlloc((void*)0x22000000,65536,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(heap!=(void*)0x22000000)return 11;_setmode(_fileno(stdout),_O_BINARY);InstallJointedFaultObserver();for(case_index=0;fread(h,sizeof(h),1,f)==1;case_index++){if(fread(heap,65536,1,f)!=1)return 12;gNet_mode=(tNet_mode)h[3];memcpy(heap+0xf000,&h[3],4);memcpy(&mattmp1,heap+0x3b00,48);memcpy(&mattmp2,heap+0x3b30,48);gPhysics_empty_message=heap[0xf020];memcpy(&gPhysics_scratch_ptr,heap+0xf024,4);memcpy(&gPhysics_other_buffer,heap+0xf028,4);memcpy(&gPhysics_other_buffer_capacity,heap+0xf02c,4);memcpy(&gPhysics_joints_changed_692dc8,heap+0xf030,4);memcpy(&gPhysics_group_counter_6940a0,heap+0xf034,4);memcpy(&gFace_num__car,heap+0xf038,4);\n#ifndef PHYSICAL_IMAGE\ngPhysics_error_cb=h[4]&4?HierarchyNativeError:NULL;(void)HierarchyOriginalError;\n#else\n*(unsigned*)(heap+0xf010)=h[4]&4?(unsigned)HierarchyOriginalError:0;(void)HierarchyNativeError;\n#endif\n_controlfp(_PC_53,_MCW_PC);_clearfp();if(h[0]>=4)return 37;invoke(h[0],h[1],h[2]);}fclose(f);return 0;}\n\n'
COMMON=COMMON.replace('static unsigned addresses[4]', 'static unsigned addresses[2]')
COMMON=COMMON.replace('int gFace_num__car;', 'tU32 gPhysics_prepared_objects_692dcc,gPhysics_stationary_filter_6940ac; tPhysics_object** gReduced_object_list; br_scalar gPhysics_bounds_max_x,gPhysics_bounds_min_x; int gFace_num__car;')
COMMON=COMMON.replace('saved=(unsigned)gPhysics_error_cb;', 'memcpy(heap+0xf060,&gPhysics_prepared_objects_692dcc,4);memcpy(heap+0xf064,&gReduced_object_list,4);memcpy(heap+0xf068,&gPhysics_stationary_filter_6940ac,4);memcpy(heap+0xf06c,&gPhysics_bounds_max_x,4);memcpy(heap+0xf070,&gPhysics_bounds_min_x,4);saved=(unsigned)gPhysics_error_cb;')
COMMON=COMMON.replace('builder_y=mode==0?label:h[5]?(unsigned)(heap+h[5]):0;', 'builder_y=h[5]?(unsigned)(heap+h[5]):0;(void)label;')
COMMON=COMMON.replace('builder_cdecl=0;builder_stack_count=mode==2?1:0;', 'builder_cdecl=0;builder_stack_count=0;')
COMMON=COMMON.replace('builder_cdecl=1;builder_stack_count=mode==2?3:2;', 'builder_cdecl=1;builder_stack_count=mode==0?1:2;')
COMMON=COMMON.replace('if(h[0]>=4)', 'memcpy(&gPhysics_prepared_objects_692dcc,heap+0xf060,4);memcpy(&gReduced_object_list,heap+0xf064,4);memcpy(&gPhysics_stationary_filter_6940ac,heap+0xf068,4);memcpy(&gPhysics_bounds_max_x,heap+0xf06c,4);memcpy(&gPhysics_bounds_min_x,heap+0xf070,4);if(h[0]>=2)')
COMMON+='\n#include \"34-finteray.h\"\nvoid C2_HOOK_FASTCALL InternalPrepareObject(tPhysics_object*);void C2_HOOK_FASTCALL PrepareObject(tPhysics_object*,tPhysics_object**);\n'
COMMON=COMMON.replace('tU32 gPhysics_prepared_objects_692dcc,gPhysics_stationary_filter_6940ac; tPhysics_object** gReduced_object_list; ', '')
COMMON=COMMON.replace('static unsigned char*heap;', 'static unsigned char*heap;\n#define gPhysics_prepared_objects_692dcc (*(volatile tU32*)(heap+0xf060))\n#define gReduced_object_list (*(tPhysics_object** volatile*)(heap+0xf064))\n#define gPhysics_stationary_filter_6940ac (*(volatile tU32*)(heap+0xf068))\n')
for variable,offset in (('gPhysics_prepared_objects_692dcc','f060'),('gReduced_object_list','f064'),('gPhysics_stationary_filter_6940ac','f068')):
 COMMON=COMMON.replace('memcpy(heap+0x'+offset+',&'+variable+',4);','').replace('memcpy(&'+variable+',heap+0x'+offset+',4);','')
caller,physical,template=registry.literals();old=' cmp builder_mode,8\n jne notwide\n fstp qword ptr builder_wide\n xor eax,eax\n notwide:\n cmp builder_mode,1\n je zeroreturn\n cmp builder_mode,10\n jne resultready\n zeroreturn:\n xor eax,eax\n resultready:';assert old in caller;caller=caller.replace(old,' xor eax,eax')
old='image[hdr[12]]=0xe9;relative=(unsigned)RayBoundary-((unsigned)image+hdr[12]+5);memcpy(image+hdr[12]+1,&relative,4);';assert old in physical;physical=physical.replace(old,'image[hdr[5]]=0xe9;relative=(unsigned)HierarchyExit-((unsigned)image+hdr[5]+5);memcpy(image+hdr[5]+1,&relative,4);').replace('hdr[14]','hdr[6]').replace('n=hdr[13]','n=hdr[4]').replace('i<12;i++)addresses','i<4;i++)addresses')
physical=physical.replace('hdr[6]','hdr[4]').replace('n=hdr[4]','n=hdr[2]').replace('image[hdr[5]]','image[hdr[3]]').replace('image+hdr[5]','image+hdr[3]').replace('i<4;i++)addresses','i<2;i++)addresses')
caller=caller.replace('builder_poison;','builder_poison,builder_cdecl;').replace(' mov builder_result,eax\n mov ecx,esp',' mov builder_result,eax\n cmp builder_cdecl,0\n je hierarchy_stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n hierarchy_stackready:\n mov ecx,esp')
foundation=['PhysicsScalarNaN','PhysicsLessOrUnordered','PhysicsGreaterOrdered','FrictionRead','FrictionSubtractScalar','PhysicsCopyWord','PhysicsSubtractScalar','PhysicsJointScalar','PhysicsJointSum','PhysicsJointProduct','PhysicsJointProductMemory','PhysicsJointAddScalar','PhysicsJointAddVelocity','PhysicsJointVelocityA','PhysicsJointVelocityB','PhysicsJointWorkState','PhysicsSetJointWorkState','PhysicsObjectEnergy','PhysicsAccumulateEnergy','PhysicsWallNaN','VelocityScalar','VelocitySubtractScalar','RotationArgument']
body='\n'.join(extract(before[str(physics)].decode(),name)for name in foundation)+'\n'
body+=workspace_block(before[str(physics)].decode())+'\n'
applications=['ApplicationScaleAdd','ApplicationWorldAdd','ApplicationDirect','ApplicationHinged','ApplicationJoint','ApplicationInlineTorque','ApplicationSecondary']
rq=['RQScalar','RQNaN','RQAccumulate','RQSubtractProduct','RQDivideScalars','RQMultiplyScalars']
body+='\n'.join(extract(before[str(physics)].decode(),name)for name in applications+rq)+'\n'
body+='\n'.join(extract(before[str(matrix)].decode(),n) for n in ['Matrix34ApplyProductLoaded', 'Matrix34ApplyProductMemory', 'Matrix34ApplyAdd', 'BrMatrix34Translate'])+ '\n'
body+='\n'.join(extract(before[str(matrix)].decode(),name)for name in ('LPRead','LPWrite','LPScalar','LPNaN','LPCopyWord'))+'\n'
body+='\n'.join(extract(before[str(geometry)].decode(),name)for name in ('FinterayScalar','FinterayBoundsAdd'))+'\n'
body+=extract((ROOT/'src/brender/core/math/vector.c').read_text(),'BrVector3SetFloat')+'\n'

def lower_capsule(text,name,helper):
 clean=mask_c(text);body=extract(text,name);start=text.index(body)
 hits=list(re.finditer(r'^static [^\n;{}]*\b'+re.escape(helper)+r'\([^;{}\n]*\)\s*\{',clean,re.M))
 assert len(hits)==1,(helper,len(hits));assert hits[0].start()<start
 return text[hits[0].start():start]+body+('\n#undef MR_PRODUCT\n#undef MR_SUM\n#undef MR_DIFFERENCE\n'if name=='BrMatrix34Rotate'else'\n')
for row in spec:
 name=row['name']
 if row['address']in(tuple(a for _,a,_ in WAKE_ENTRIES+PREP_ENTRIES)+(0x4b9f90,)+(0x4b7510,0x4b9750,0x4b9230,0x4b8ee0,0x575b80,0x534590,0x4be550,0x4becf0,0x4bf490)):continue
 if name=='BrMatrix34Rotate'and args.matrix_rotate_candidate:
  body+=before[str(args.matrix_rotate_candidate.resolve())].decode()+'\n';continue
 if name=='BrMatrix34PostTranslate'and args.matrix_post_candidate:
  body+=before[str(args.matrix_post_candidate.resolve())].decode()+'\n';continue
 source=geometry if name=='GetNewBoundingBox'else physics if not name.startswith(('BrMatrix','BrQuat','GetOrientation','GetMatrixFrom'))else matrix if name.startswith('BrMatrix')and name!='BrMatrix34ToQuat' else quat if name.startswith('BrQuat')or name=='BrMatrix34ToQuat' else phil
 if name in('BrMatrix34Rotate','BrMatrix34PostTranslate'):
  body+=lower_capsule(before[str(source)].decode(),name,'MatrixRotateScalar'if name=='BrMatrix34Rotate'else'MatrixPostTranslateAdd')
 elif name=='GetWorldBoundingBoxForObject':body+=extract(before[str(bounds_candidate)].decode(),name)+'\n'
 elif name in('PhysicsError','GetPhysicsScratchPointer','IncreasePhysicsScratchPointer'):body+='\n#define exit HierarchyExit\n'+extract(before[str(source)].decode(),name)+'\n#undef exit\n'
 else:body+=extract(before[str(source)].decode(),name)+'\n'
for name in('PhilOrientationScalar',):body=extract(before[str(phil)].decode(),name)+'\n'+body
for name in('QuaternionSum','QuaternionScaleRaw','QuaternionScaleLoaded','QuaternionMultiply','QuaternionAddScalar','QuaternionSubtractScalar','QuaternionScalar'):body=extract(before[str(quat)].decode(),name)+'\n'+body
rotation_text=before[str(physics)].decode().replace('\r\n','\n')
hierarchy_first=extract(rotation_text,'FUN_004b9230');hierarchy_last=extract(rotation_text,'FUN_004b8ee0')
marker='/* Original calls push the time DWORD with MOV/PUSH.'
assert rotation_text.count(marker)==1
hierarchy_start=rotation_text.index(marker);hierarchy_end=rotation_text.index(hierarchy_last)+len(hierarchy_last)
assert hierarchy_start<rotation_text.index(hierarchy_first)<rotation_text.index(hierarchy_last)
body+='\n'+rotation_text[hierarchy_start:hierarchy_end]+'\n#undef HIERARCHY_RECURSE\n#undef HIERARCHY_MOTION\n'
container=rotation_text[rotation_text.index('typedef struct tForce_group_container {'):rotation_text.index('typedef void (C2_HOOK_FASTCALL *tForce_group_callback)')]
candidate_text=before[str(physics)].decode().replace('\r\n','\n')
candidate_marker='/* Both original inline passes execute'
assert candidate_text.count(candidate_marker)==1
candidate_first=extract(candidate_text,'JointedMotionEnergyPass')
candidate_parent=extract(candidate_text,'MoveJointedObject')
candidate_wrapper=extract(candidate_text,'FUN_004b9750')
candidate_start=candidate_text.index(candidate_marker)
candidate_end=candidate_text.index(candidate_wrapper)+len(candidate_wrapper)
assert candidate_start<candidate_text.index(candidate_first)<candidate_text.index(candidate_parent)<candidate_text.index(candidate_wrapper)
candidate_capsule=candidate_text[candidate_start:candidate_end]
wake_text=before[str(physics)].decode().replace('\r\n','\n')
wake_first=extract(wake_text,'WakePointer');wake_last=extract(wake_text,'FUN_004be0e0')
wake_text=wake_text[wake_text.index(wake_first):wake_text.index(wake_last)+len(wake_last)]
prepare_text=before[str(candidate)].decode()
if candidate==physics:
 first_prepare=extract(prepare_text,'PrepareWord');last_prepare=extract(prepare_text,'PrepareObject')
 prepare_text=prepare_text[prepare_text.index(first_prepare):prepare_text.index(last_prepare)+len(last_prepare)]
for declaration in ('extern tU32 gPhysics_prepared_objects_692dcc;', 'extern tPhysics_object** gReduced_object_list;', 'extern tU32 gPhysics_stationary_filter_6940ac;'):
 assert prepare_text.count(declaration)in(0,1)
 prepare_text=prepare_text.replace(declaration,'')
body+=container+'\n'+candidate_capsule+'\n'+wake_text+'\n'+extract(before[str(overlap)].decode(),'CheckForObjectHierachyTouchingAnotherObject')+'\n'+prepare_text+'\n'
native=COMMON+'\n'+body+r'''
int C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;addresses[0]=(unsigned)InternalPrepareObject;addresses[1]=(unsigned)PrepareObject;return run(argv[1]);}
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
 if 0x6940a0 not in symbols:
  # Zero-initialized common symbols can be absent from reccmp's map. Resolve
  # only the declared exact canonical U32 symbol, preserving numeric identity.
  assert re.search(r'// GLOBAL: CARMA2_HW 0x006940a0\s+tU32 gPhysics_group_counter_6940a0\s*(?:=\s*0\s*)?;',before[str(physics)].decode())
  rows=re.findall(r'^\s*0003:[0-9a-fA-F]{8}\s+_gPhysics_group_counter_6940a0\s+([0-9a-fA-F]{8})\s+<common>\s*$',map_bytes.decode('latin1'),re.M)
  assert len(rows)==1,rows
  address=int(rows[0],16);region=next(s for s in sections if s[0]<=address<s[0]+s[1]);assert region[2]&0x80000000 and not region[2]&0x20000000
  symbols[0x6940a0]=address
  global_fallbacks.append(dict(original=0x6940a0,recompiled=address,name='_gPhysics_group_counter_6940a0',owner='<common>',bytes=4,authority='Exact canonical U32 linker common symbol and original-address annotated zero-initialized declaration.'))
 for identity,canonical,declaration in ((0x692dcc,'gPhysics_prepared_objects_692dcc',r'tU32\s+gPhysics_prepared_objects_692dcc'),(0x6940a8,'gReduced_object_list',r'tPhysics_object\s*\*\s*\*\s*gReduced_object_list'),(0x6940ac,'gPhysics_stationary_filter_6940ac',r'tU32\s+gPhysics_stationary_filter_6940ac'),(0x692dd0,'gPhysics_bounds_max_x',r'br_scalar\s+gPhysics_bounds_max_x'),(0x692dd4,'gPhysics_bounds_min_x',r'br_scalar\s+gPhysics_bounds_min_x')):
  if identity in symbols:continue
  assert re.search(r'// GLOBAL: CARMA2_HW 0x00'+format(identity,'06x')+r'\s+'+declaration+r'\s*(?:=\s*(?:0|NULL)\s*)?;',before[str(physics)].decode())
  rows=re.findall(r'^\s*0003:[0-9a-fA-F]{8}\s+_'+canonical+r'\s+([0-9a-fA-F]{8})\s+<common>\s*$',map_bytes.decode('latin1'),re.M)
  assert len(rows)==1,(canonical,rows)
  address=int(rows[0],16);region=next(s for s in sections if s[0]<=address<s[0]+s[1]);assert region[2]&0x80000000 and not region[2]&0x20000000
  symbols[identity]=address
  global_fallbacks.append(dict(original=identity,recompiled=address,name='_'+canonical,owner='<common>',bytes=4,authority='Exact annotated canonical zero-initialized declaration and exact common linker symbol; no nearest-symbol inference.'))
 initial={symbols[a]:name for name,a,_ in ORIGINAL_ENTRIES if a in symbols};roots=[symbols[a]for _,a,_ in PREP_ENTRIES]
 mutable={symbols[0x6940a0]:(0x2200f034,4),symbols[0x65d010]:(0x2200f038,4),symbols[0x6b09c0]:(0x22003b00,48),symbols[0x6b09f0]:(0x22003b30,48),symbols[0x68b918]:(0x2200f000,4),symbols[0x6940c0]:(0x2200f010,4),symbols[0x676920]:(0x2200f020,1),symbols[0x6940bc]:(0x2200f024,4),symbols[0x6940c4]:(0x2200f028,4),symbols[0x6940c8]:(0x2200f02c,4),symbols[0x692dc8]:(0x2200f030,4),symbols[0x692dcc]:(0x2200f060,4),symbols[0x6940a8]:(0x2200f064,4),symbols[0x6940ac]:(0x2200f068,4),symbols[0x692dd0]:(0x2200f06c,4),symbols[0x692dd4]:(0x2200f070,4)}
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
   assert owner in('49-physics.c.obj','60-phil.c.obj','matrix34.c.obj','quat.c.obj','libbrender:matrix34.c.obj','libbrender:quat.c.obj','34-finteray.c.obj')and plain.startswith(('PhysicsJoint','PhysicsWallNaN','RotationArgument','MatrixRotate','MatrixPostTranslate','PhilOrientation','Quaternion','Partner','Velocity','Application','Force','RQ','PhysicsScalarNaN','PhysicsLessOrUnordered','PhysicsGreaterOrdered','PhysicsCopyWord','PhysicsSubtractScalar','PhysicsObjectEnergy','PhysicsAccumulateEnergy','PhysicsSetJointWorkState','JointedMotionEnergyPass','Wake','Friction','Prepare','LP','FinterayBoundsAdd','FinterayScalar','Matrix34Apply'))or plain in('ftol','_ftol')and owner=='LIBC:ftol.obj',(name,owner)
  pending=[start];instructions={};callback_loads={}
  while pending:
   location=pending.pop()
   while location not in instructions:
    assert start<=location<end,(name,hex(location))
    ins=next(cs.disasm(raw(linked_data,location,15),location));instructions[location]=ins.bytes.hex()
    if ins.mnemonic=='mov'and ins.operands[0].type==CS_OP_REG and ins.operands[1].type==CS_OP_MEM and ins.operands[1].mem.disp==symbols[0x6940c0]:callback_loads[ins.operands[0].reg]=ins.address
    is_table=ins.mnemonic=='jmp'and ins.operands[0].type==CS_OP_MEM and bool(ins.operands[0].mem.index)
    if is_table:
     operand=ins.operands[0];assert start in(symbols[0x4b9230],symbols[0x4b8350],symbols[0x4b9650])and operand.mem.scale==4 and not operand.mem.base
     count=5 if start==symbols[0x4b9650]else 4;targets=struct.unpack('<'+str(count)+'I',raw(linked_data,operand.mem.disp,4*count));assert all(start<=t<end for t in targets)
     pending.extend(targets)
     for k,target in enumerate(targets):
      site=operand.mem.disp-base2+4*k;linked_patches[site]=(site,target,target-base2,0)
     linked_tables.append(dict(function=name,address=operand.mem.disp,index_domain=list(range(count)),target_offsets=[t-start for t in targets],authority='Actual original/source bounded primary joint-kind switch: Count0..4; allocation and hierarchy1..4 after selector-minus1.'))
    elif ins.mnemonic=='call'or ins.mnemonic.startswith('j'):
     if ins.operands[0].type!=CS_OP_IMM:
      # MSVC5 may inline the complete PhysicsError callback/exit sequence in
      # the recursive entry. The controlled callback is still the fresh load
      # of the same canonical mutable callback global, not an arbitrary call.
      assert start in(symbols[0x4c61c0],symbols[0x4b9230],symbols[0x4b9650],symbols[0x4c5c70],symbols[0x4c5cb0],symbols[0x4b8350],symbols[0x4b7510],symbols[0x4b9750])and ins.mnemonic=='call'and ins.operands[0].type==CS_OP_REG and ins.operands[0].reg in callback_loads and ins.address-callback_loads[ins.operands[0].reg]<=64,(name,hex(ins.address),ins.mnemonic,ins.op_str,callback_loads)
      linked_callbacks.append(dict(function=name,address=ins.address,load=callback_loads[ins.operands[0].reg],global_address=symbols[0x6940c0],register=cs.reg_name(ins.operands[0].reg)))
     else:
      target=ins.operands[0].imm
      if start<=target<end:
       if ins.mnemonic!='call':pending.append(target)
       else:assert start in(tuple(symbols[a]for _,a,_ in WAKE_ENTRIES+PREP_ENTRIES)+(symbols[0x4b9f90],symbols[0x4b9ce0],symbols[0x4b9f40],symbols[0x4b9230],symbols[0x4c2a10],symbols[0x4b9650],symbols[0x4b7290],symbols[0x4b8350]))and target==start,('Unexpected local call',name,ins.op_str)
      elif target!=exit_address:
       assert target in functions,(name,ins.op_str);todo.append(target)
    for op in ins.operands:
     value=None;site=None;width=4
     if op.type==CS_OP_MEM and base2<=op.mem.disp<base2+size2:
      value=op.mem.disp;site=ins.address+ins.disp_offset-base2;width=4*count if is_table else 12 if op.mem.base or op.mem.index else op.size;assert ins.disp_size==4
     elif op.type==CS_OP_IMM and ins.mnemonic in('mov','push')and base2<=op.imm<base2+size2:
      value=op.imm;site=ins.address+ins.imm_offset-base2;assert ins.imm_size==4
     if value is not None:
      region=next(s for s in sections if s[0]<=value<s[0]+s[1]);absolute=False
      head=next((a for a,(target,length)in mutable.items()if a<=value<a+length),None)
      if head is not None:target=mutable[head][0]+value-head;absolute=True
      elif is_table:
       target=value-base2
      else:
       if region[2]&0x80000000:
        assert raw(linked_data,value,22)==b'Stupid trial position\0' and ins.mnemonic in('mov','push') and op.type==CS_OP_IMM,(name,hex(value),ins.op_str)
       else:assert not region[2]&0x20,(name,hex(value),ins.op_str)
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

BASE=0x22000000
OBJECTS=(0x100,0x900,0x1100,0x1900)
ACTORS=(0x5000,0x5100,0x5200,0x5300)
cases=[]
def add(**items):
 c=dict(mode=0,object_mode=2,disabled=0,mass=0x3f800000,scale=False,tree=0,network=0,seed=len(cases)+1,callback=0,capacity=0x6000,subject=0,list_nodes=(0,),positions=(1.,2.,3.,4.),stationary=0,reduced=0,parent=False,patches=[],child_type=1,actor_matrix_at=None)
 c.update(items);cases.append(c)

# Entire branch matrix, including TEST CL,DL low-bit behavior and parent inheritance.
for mode in (0,1):
 for object_mode in (0,1,2,3,0xffffffff):
  for disabled in (0,1,2,128,255):
   for scale in (False,True):add(mode=mode,object_mode=object_mode,disabled=disabled,scale=scale)
for tree in (1,2,3):
 for mode in (0,1):
  for object_mode in (0,1,2):
   for disabled in (0,1,2):add(mode=mode,object_mode=object_mode,disabled=disabled,tree=tree)
for mass in (0,0x80000000,1,0x80000001,0xbf800000,0x7f800000,0xff800000,0x7f800001,0xff800001,0x7fc12345,0xffc12345):
 for mode in (0,1):
  for disabled in (0,1):add(mode=mode,mass=mass,disabled=disabled,scale=True)
for object_mode in (0,1,2):
 for disabled in (0,1,2,128,255):
  for inherited in (0,1,2,255):add(object_mode=object_mode,disabled=disabled,parent=True,patches=[(0x900+0xec,inherited)])
# Backward/head and forward swaps, equality and the original asymmetric unordered gates.
for positions in ((0.,1.,2.,3.),(2.,1.,0.,3.),(3.,1.,2.,0.),(1.,1.,1.,1.),(-5.,4.,3.,2.),(0.,5.,10.,15.)):
 for subject in (0,1,2,3):
  for disabled in (0,1):add(mode=1,object_mode=1,disabled=disabled,subject=subject,list_nodes=(0,1,2,3),positions=positions)
for subject in (0,1,2,3):
 for bits in (0x7f800001,0xff800001,0x7fc12345,0xffc12345,0x7f800000,0xff800000):
  add(mode=1,object_mode=2,subject=subject,list_nodes=(0,1,2,3),patches=[(OBJECTS[subject]+0xb0,bits),(ACTORS[subject]+0x50,bits)])
# Wake with the actual overlap consumer and zero-time full movement graph.
for stationary in (1,0xffffffff):
 for tree in (0,1,2,3):
  for disabled in (0,1,2):
   for child_type in (0,1,2,3,4):add(mode=1,object_mode=2,disabled=disabled,stationary=stationary,tree=tree,child_type=child_type,list_nodes=(0,2))
for reduced in (0x3c04,0x334,0xf064):
 for mode in (0,1):add(mode=mode,reduced=reduced)

def make_case(c):
 rng=random.Random(c['seed']);b=bytearray(words([rng.getrandbits(32)for _ in range(16384)]))
 def put(offset,*values):struct.pack_into('<'+'I'*len(values),b,offset,*[v&0xffffffff for v in values])
 def vec(offset,*values):struct.pack_into('<'+'f'*len(values),b,offset,*values)
 for i,obj in enumerate(OBJECTS):
  actor=ACTORS[i]if not(i==c['subject']and c['actor_matrix_at']is not None)else c['actor_matrix_at']-0x2c
  b[obj:obj+0x4d8]=bytes(0x4d8)
  put(obj,BASE+actor);put(obj+4,c['mass']if i==c['subject']else 0x3f800000)
  vec(obj+8,1.,2.,3.);vec(obj+0x14,1.,2.,3.)
  vec(obj+0x24,-.5,-.75,-1.);vec(obj+0x30,.5,.75,1.)
  vec(obj+0x3c,-.5,-.75,-1.);vec(obj+0x48,.5,.75,1.)
  vec(obj+0x68,.125,-.25,.375);vec(obj+0x74,.1,.2,.3)
  matrix=(1.,0.,0.,0.,1.,0.,0.,0.,1.,c['positions'][i],2.+i,3.+i)
  vec(obj+0x8c,*matrix);vec(obj+0xbc,*matrix);vec(obj+0x1e8,*matrix);vec(actor+0x2c,*matrix)
  put(obj+0x19c,0xffffffff);put(obj+0x1d8,BASE+obj)
  put(obj+0x218,0);put(obj+0x228,0)
  b[obj+0xec]=c['disabled'];b[obj+0x1df]=int(c['scale'])
  put(obj+0xf0,c['object_mode']);put(obj+0x21c,0xdeadbeef)
  joint=0x4000+i*0x100
  b[joint:joint+0x100]=bytes(0x100)
  put(joint,c['child_type']);vec(joint+8,.1,.2,.3);vec(joint+0x14,.2,.3,.4);vec(joint+0x20,1.,0.,0.);vec(joint+0x2c,0.,0.,1.);vec(joint+0x38,0.,1.,0.)
 if c['tree']:
  put(0x324,BASE+0x900);put(0xb28,BASE+0x100);put(0xb30,BASE+0x100);put(0xa88,BASE+0x4100)
 if c['tree']>=2:put(0xb20,BASE+0x1100);put(0x1328,BASE+0x100);put(0x1330,BASE+0x100);put(0x1288,BASE+0x4200)
 if c['tree']>=3:put(0xb24,BASE+0x1900);put(0x1b28,BASE+0x900);put(0x1b30,BASE+0x900);put(0x1a88,BASE+0x4300)
 if c['parent']:put(OBJECTS[c['subject']]+0x228,BASE+0x900)
 if c['mode']==1:
  for j,i in enumerate(c['list_nodes']):
   put(OBJECTS[i]+0x22c,0 if j==0 else BASE+OBJECTS[c['list_nodes'][j-1]])
   put(OBJECTS[i]+0x220,0 if j+1==len(c['list_nodes'])else BASE+OBJECTS[c['list_nodes'][j+1]])
 put(0x3c00,0 if not c['list_nodes']else BASE+OBJECTS[c['list_nodes'][0]]);put(0x3c04,BASE+0x1100)
 put(0xf010,0);b[0xf020]=0;put(0xf024,BASE+0x8000);put(0xf028,BASE+0x8000);put(0xf02c,c['capacity']);put(0xf030,0x12345678);put(0xf034,17);put(0xf038,123)
 put(0xf060,0xabcdef00);put(0xf064,0 if not c['reduced']else BASE+c['reduced']);put(0xf068,c['stationary']);put(0xf06c,0x447a0000);put(0xf070,0xc47a0000)
 for offset,value in c['patches']:
  if offset%4: b[offset]=value&255
  else:put(offset,value)
 return words([c['mode'],BASE+OBJECTS[c['subject']],0,c['network'],c['callback'],c.get('head_offset',0x3c00),0])+b

# Diagnostic norm: exact threshold and original asymmetric y32/z32 spills.
for axis in range(3):
 for bits in (0,0x80000000,1,0x80000001,0x3f7fffff,0x3f800000,0x3f800001,0xbf800001,0x4b800001,0x7f800000,0xff800000,0x7f800001,0xff800001,0x7fc12345,0xffc12345):
  for disabled in (0,1):
   add(disabled=disabled,positions=(0.,2.,3.,4.),patches=[(ACTORS[0]+0x50,0),(ACTORS[0]+0x54,0),(ACTORS[0]+0x58,0),(0x100+0xb0,0),(0x100+0xb4,0),(0x100+0xb8,0),(ACTORS[0]+0x50+4*axis,bits)])
for seed in range(64):
 rng=random.Random(0x4b99e000+seed)
 fields=[]
 for axis in range(3):
  fields.extend(((0x100+0xb0+4*axis,rng.choice((0x3f800001,0x4b800001,0x3effffff,0xbeffffff,0x35800001))), (ACTORS[0]+0x50+4*axis,rng.choice((0x3f800000,0x4b800000,0x3f000000,0xbf000000,0x35800000)))))
 add(disabled=1,patches=fields)
# Full equality-or-unordered matrix scan; individual lanes may retain disabled.
for lane in range(12):
 for bits in (0x80000000,0x3f000001,0x7f800001,0xff800001,0x7fc12345,0xffc12345):
  add(disabled=1,patches=[(ACTORS[0]+0x2c+4*lane,bits)])
# Authenticated full lower bounding pipeline also consumes the raw matrices,
# so these deliberately preserve any real lower counterexample.
for lane in (0,1,4,8,9,10,11):
 for a,b in ((0x7f812345,0xff812345),(0xff812345,0x7f812345),(0x7fc12345,0xffc12345),(0xffc12345,0x7fc12345)):
  add(disabled=1,patches=[(0x100+0x8c+4*lane,a),(ACTORS[0]+0x2c+4*lane,b)])
# Parent inheritance occurs after all raw snapshots, including aliases into
# fields just cleared by this entry; no cached initial parent-byte substitute.
for parent in (0x100,0x100+0xf0,0x100+0xe0):
 for object_mode in (0,1,2):add(object_mode=object_mode,disabled=1,patches=[(0x328,BASE+parent)])
# Sequential partial matrix overlaps use actual Copy/LPNormalise/bounds bodies.
for object_mode in (0,1,2):
 for delta in (-8,-4,0,4,8,12,24,48):
  for disabled in (0,1):add(object_mode=object_mode,disabled=disabled,actor_matrix_at=0x100+0x8c+delta)
# Root real Move uses every primary joint-kind branch, real hierarchy and
# allocation/physics-error exits, including callbacks mutating reachable state.
for child_type in (0,1,2,3,4,5):
 for capacity in (0,1,252,708,0x6000):
  for callback in (0,4,4|8|16|32):
   add(object_mode=0,tree=1,child_type=child_type,capacity=capacity,callback=callback,patches=[(0xf040,0xffffffff),(0xf044,0x10001),(0xf048,0x324),(0xf04c,0)])
# Pure list-head aliases and genuine terminal pointer faults. A null captured
# head is not touched without the exact stationary condition/head-update branch.
for head_offset in (0,0x328,0x330,0x334):
 for stationary in (0,1):
  for disabled in (0,1):add(mode=1,disabled=disabled,stationary=stationary,head_offset=head_offset)
# Full active (not fixed-bit400) movement/constraint consumer; preserve all
# earlier873 byte fixtures and append this gap repair without changing them.
for flags in (0,0x1200,0x400):
 for tree in (0,1,2,3):
  for kind in (0,1,4):
   add(object_mode=0,tree=tree,child_type=kind,patches=[(o+0x19c,flags)for o in OBJECTS])
for network in (0,1,2):
 for stationary in (0,1):
  for kind in (1,2,3,4):
   add(mode=1,object_mode=0,tree=2,child_type=kind,stationary=stationary,network=network,patches=[(o+0x19c,0)for o in OBJECTS])

indices=args.case_index or list(range(len(cases)));indices=indices[:args.limit]if args.limit else indices;results=[];failures=[]
for index in indices:
 payload=make_case(cases[index]);path=OUT/'case.bin';path.write_bytes(payload);r=subprocess.run([str(OUT/'physical.exe'),str(path),str(ROOT/'build-reference/originals/CARMA2_HW.EXE'),str(OUT/'original-addresses.bin')],capture_output=True,timeout=30);assert r.returncode==0,('original',index,r.returncode);expected=r.stdout
 assert expected and len(expected)%(12+65536)==0,('snapshot layout',index,len(expected))
 original_events=[dict(event=event,value=value,message_or_fault_address=message)for offset in range(0,len(expected),12+65536)for event,value,message in[struct.unpack_from('<3I',expected,offset)]]
 for compiler in('msvc5','msvc','mingw'):
  r=subprocess.run([str(OUT/(compiler+'.exe')),str(path)],capture_output=True,timeout=30)
  result={'index':index,'compiler':compiler,'fixture_sha256':sha(payload),'original_output_sha256':sha(expected),'original_output_bytes':len(expected),'original_events':original_events,'normal_return_abi_checked':original_events[-1]['event']==0,'actual_output_sha256':sha(r.stdout),'actual_output_bytes':len(r.stdout),'exit':r.returncode,'passed':r.returncode==0 and r.stdout==expected};results.append(result)
  if not result['passed']:
   result['differences']=[{'offset':j,'expected':expected[j:j+4].hex(),'actual':r.stdout[j:j+4].hex()}for j in range(0,min(len(expected),len(r.stdout)),4)if expected[j:j+4]!=r.stdout[j:j+4]];failures.append(result);(OUT/f'failure-{index}-{compiler}.json').write_text(json.dumps(result,indent=2));(OUT/f'failure-{index}.bin').write_bytes(payload);(OUT/f'failure-{index}.original-output.bin').write_bytes(expected);(OUT/f'failure-{index}.{compiler}-output.bin').write_bytes(r.stdout)
 if matching:
  r=subprocess.run([str(OUT/'physical.exe'),str(path),str(args.recompiled.resolve()),str(OUT/'matching-addresses.bin')],capture_output=True,timeout=30)
  result=dict(index=index,compiler='matching',fixture_sha256=sha(payload),original_output_sha256=sha(expected),original_output_bytes=len(expected),original_events=original_events,normal_return_abi_checked=original_events[-1]['event']==0,actual_output_sha256=sha(r.stdout),actual_output_bytes=len(r.stdout),exit=r.returncode,passed=r.returncode==0 and r.stdout==expected);results.append(result)
  if not result['passed']:
   result['differences']=[dict(offset=j,expected=expected[j:j+4].hex(),actual=r.stdout[j:j+4].hex())for j in range(0,min(len(expected),len(r.stdout)),4)if expected[j:j+4]!=r.stdout[j:j+4]];failures.append(result);(OUT/f'failure-{index}-matching.json').write_text(json.dumps(result,indent=2));(OUT/f'failure-{index}.bin').write_bytes(payload);(OUT/f'failure-{index}.original-output.bin').write_bytes(expected);(OUT/f'failure-{index}.matching-output.bin').write_bytes(r.stdout)
 if index%10==0:print(index,len(failures),flush=True)
 if len(failures)>=12:break
after={str(f):f.read_bytes()for f in paths};assert before==after,'Sourcechanged during proof'
manifest={'source_snapshots':source_snapshots,'scope':__doc__,'matching':matching,'input_sha256':sha(b''.join(make_case(cases[i])for i in indices)),'expanded':args.expanded,'total_scenes':len(cases),'selected_scenes':len(indices),'complete_suite':len(indices)==len(cases)and indices.index(index)+1==len(cases),'spec':spec,'constants':constants,'patches':patches,'source_stable':True,'sources':{f:sha(b)for f,b in before.items()},'original_sha256':HASH,'scenes':indices.index(index)+1,'comparisons':len(results),'failures':failures,'results':results,'commands':commands,'generated_sha256':{str(f):sha(f.read_bytes())for f in OUT.iterdir()if f.suffix in('.c','.obj','.exe','.bin','.txt')}}
(OUT/'verification.json').write_text(json.dumps(manifest,indent=2)+'\n');print(manifest['scenes'],manifest['comparisons'],len(failures));assert not failures
