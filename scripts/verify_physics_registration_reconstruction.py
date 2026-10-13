"""Full four-entry registration scratch oracle, with real preparation/raycast.

No runtime is authorized until root confirms the production source freeze.
The physical original packet is fail-closed over direct control flow, genuine
callback bodies, indexed tables, absolute operands and PE relocations. The
only controlled call is the external physics-error callback; CRT exit and AV
are terminal full-public-state observations, never successful game returns.
Native lower bodies are extracted from current production at invocation time.
"""
import argparse,ast,json,random,re,struct,subprocess,sys
from pathlib import Path
from types import SimpleNamespace
from capstone import *
from capstone.x86_const import X86_REG_ESP

ROOT=next(p for p in Path(__file__).resolve().parents if (p/'src/carma2').is_dir())
D=Path(__file__).resolve().parent
SOURCE=ROOT/"src/carma2/newcommon/60-phil.c"
COMMON=r'''/* Scratch registration proof. No game/math/geometry callee is substituted. */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
/* The legacy public header predates this const-correct real body declaration.
 * Hide that unused declaration in this standalone merged translation unit. */
#define BrTransformToMatrix34 RegistrationLegacyTransformDeclaration
#include "49-physics.h"
#include "60-phil.h"
#include "55-volume.h"
#include "34-finteray.h"
#include "00-car.h"
#include "globvrpb.h"
#include "c2_math.h"
#include "rec2_macros.h"
#undef BrTransformToMatrix34
void C2_HOOK_CDECL BrTransformToMatrix34(br_matrix34*,const br_transform*);
#include "C:/Users/user/Documents/ChatGPT/rec2/src/brender/core/math/angles.h"
#include "C:/Users/user/Documents/ChatGPT/rec2/src/brender/core/math/quat.h"
#include <math.h>
#define M_DIAG(x) (((br_scalar*)mat->m)[4*(x)])
#define Q_EL(e) (((br_scalar*)&(q->x))[(e)])
#define M(x,y) mat->m[x][y]
#define A(x,y) A->m[x][y]
#define B(x,y) B->m[x][y]
#define C(x,y) C->m[x][y]
#define M34 BR_TRANSFORM_MATRIX34
#define MLP BR_TRANSFORM_MATRIX34_LP
#define QUT BR_TRANSFORM_QUAT
#define EUL BR_TRANSFORM_EULER
#define LUP BR_TRANSFORM_LOOK_UP
#define TRA BR_TRANSFORM_TRANSLATION
#define IDT BR_TRANSFORM_IDENTITY
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5];
extern unsigned builder_stack_count,builder_result,builder_poison,builder_cdecl;
extern unsigned __cdecl BuilderInvoke(void);
static unsigned addresses[4],h[8],case_index;
static unsigned char* heap;
enum {ARENA_BYTES=0x80000};
#define gNet_mode (*(tNet_mode*)(heap+0xf000))
#define gPhysics_error_cb (*(tPhysicsError_cbfn**)(heap+0xf010))
#define gPhysics_empty_message (*(char*)(heap+0xf020))
#define gPhysics_scratch_ptr (*(void**)(heap+0xf024))
#define gPhysics_other_buffer (*(void**)(heap+0xf028))
#define gPhysics_other_buffer_capacity (*(int*)(heap+0xf02c))
#define gPhysics_joints_changed_692dc8 (*(tU32*)(heap+0xf030))
#define gPhysics_group_counter_6940a0 (*(tU32*)(heap+0xf034))
#define gFace_num__car (*(int*)(heap+0xf038))
#define gPhysics_prepared_objects_692dcc (*(volatile tU32*)(heap+0xf060))
#define gReduced_object_list (*(tPhysics_object** volatile*)(heap+0xf064))
#define gPhysics_stationary_filter_6940ac (*(volatile tU32*)(heap+0xf068))
#define gPhysics_bounds_max_x (*(br_scalar*)(heap+0xf06c))
#define gPhysics_bounds_min_x (*(br_scalar*)(heap+0xf070))
#define mattmp1 (*(br_matrix34*)(heap+0x3b00))
#define mattmp2 (*(br_matrix34*)(heap+0x3b30))
#define gPHIL_registry (*(tPHIL_registry_storage*)(heap+0x40000))
#define gPHIL_disabled (*(int*)(heap+0xf080))
#define gPHIL_munging_objects (*(int*)(heap+0xf084))
#define gPHIL_doing_physics (*(int*)(heap+0xf088))
#define gPHIL_list_collision_infos (*(tPhysics_object**)(heap+0xf08c))
#define gDefault_water_spec_vol_real (*(tSpecial_volume**)(heap+0xf090))
#define gPling_materials (*(int*)(heap+0xf094))
#define gTemp_group (*(int*)(heap+0xf098))
#define gNearest_face (*(int*)(heap+0xf09c))
#define gNearest_model (*(br_model**)(heap+0xf0a0))
#define gNearest_actor (*(br_actor**)(heap+0xf0a4))
#define gNearest_T (*(br_scalar*)(heap+0xf0a8))
#define gNearest_face_group (*(int*)(heap+0xf0ac))
#define gTrack_actor (*(br_actor**)(heap+0xf0b0))
#define gPick_model_to_view__finteray (*(br_matrix34*)(heap+0xf100))
#define OrderAxes ((order_info*)(heap+0xf180))
typedef char RegistrationLayouts[sizeof(void*)==4 && sizeof(tPhysics_object)==1240 && sizeof(tPHIL_registry_storage)==2532 && sizeof(tPHIL_queued_object_info)==100 && sizeof(tPHIL_queued_header)==24 && offsetof(tPHIL_registry_storage,queued)==16 && offsetof(tPHIL_queued_objects,headers)==104 && sizeof(order_info)==4 ? 1:-1];
typedef char RegistrationGeometryLayouts[sizeof(br_actor)==100 && offsetof(br_actor,children)==8 && offsetof(br_actor,model)==24 && offsetof(br_model,prepared)==76 && sizeof(v11group)==36 && sizeof(DR_VERTEX)==32 && sizeof(DR_FACE)==28 ? 1:-1];
int C2_HOOK_FASTCALL PHILAddObject(tPhysics_object*);
int C2_HOOK_FASTCALL PHILAddActiveObject(tPhysics_object*,const br_matrix34*,const br_vector3*,const br_vector3*);
int C2_HOOK_FASTCALL FUN_004b62b0(tPhysics_object*);
int C2_HOOK_FASTCALL FUN_004b62e0(tPhysics_object*,const br_matrix34*,const br_vector3*,const br_vector3*);
/* Snapshot only identifies the external error callback by presence. It does
 * not normalize numerical outputs or registry pointers/bytes. */
static void RegistrationSnapshot(unsigned event,unsigned value,unsigned detail) {
    unsigned head[4],saved=(unsigned)gPhysics_error_cb;
    head[0]=case_index;head[1]=event;head[2]=value;head[3]=detail;
    *(unsigned*)(heap+0xf010)=!!saved;
    fwrite(head,sizeof(head),1,stdout);fwrite(heap,ARENA_BYTES,1,stdout);
    *(unsigned*)(heap+0xf010)=saved;
}
static void RegistrationErrorAction(tPhysicsError value,const char*message) {
    RegistrationSnapshot(1,(unsigned)value,message?0x100+*(const unsigned char*)message:0);
    /* The mutation is the boundary's real effect, visible to the continuing
     * original body. It does not decide a numerical/geometry result. */
    if(h[5]&1)gPhysics_error_cb=NULL;
    if(h[5]&2)gPHIL_disabled=1;
    if(h[5]&8)gPHIL_list_collision_infos=(tPhysics_object*)(heap+0x1100);
    if(h[5]&16)gPhysics_group_counter_6940a0=*(unsigned*)(heap+0xf040);
    if(h[5]&32)memcpy(heap+*(unsigned*)(heap+0xf048),heap+0xf04c,4);
}
static void __fastcall RegistrationOriginalError(tPhysicsError value,const char*message) {RegistrationErrorAction(value,message);}
static void C2_HOOK_FASTCALL RegistrationNativeError(tPhysicsError value,const char*message) {RegistrationErrorAction(value,message);}
/* The CRT exit graph is deliberately terminal; never return success to game. */
static void C2_NORETURN C2_HOOK_CDECL HierarchyExit(int value) {RegistrationSnapshot(2,(unsigned)value,0);fflush(stdout);ExitProcess(0);}
static LONG WINAPI RegistrationFault(EXCEPTION_POINTERS* x) {
    if(x->ExceptionRecord->ExceptionCode!=EXCEPTION_ACCESS_VIOLATION)return EXCEPTION_CONTINUE_SEARCH;
    RegistrationSnapshot(3,(unsigned)x->ExceptionRecord->ExceptionCode,(unsigned)x->ExceptionRecord->ExceptionInformation[1]);
    fflush(stdout);ExitProcess(0);return EXCEPTION_CONTINUE_SEARCH;
}
static void RegistrationInstallObserver(void) {
    typedef void*(WINAPI *install_fn)(ULONG,LONG(WINAPI*)(EXCEPTION_POINTERS*));
    FARPROC address=GetProcAddress(GetModuleHandleA("kernel32.dll"),"AddVectoredExceptionHandler");install_fn install;
    memcpy(&install,&address,4);if(!install||!install(1,RegistrationFault))exit(38);
}
static int run(const char* path) {
    FILE* f;unsigned mode;
    SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);
    f=fopen(path,"rb");if(!f)return 10;
    heap=(unsigned char*)VirtualAlloc((void*)0x22000000,ARENA_BYTES,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
    if(heap!=(void*)0x22000000)return 11;
    _setmode(_fileno(stdout),_O_BINARY);RegistrationInstallObserver();
    for(case_index=0;fread(h,sizeof(h),1,f)==1;case_index++) {
        if(fread(heap,ARENA_BYTES,1,f)!=1)return 12;
        mode=h[0];if(mode>=4)return 37;
#ifdef PHYSICAL_IMAGE
        gPhysics_error_cb=h[5]&4?RegistrationOriginalError:NULL;(void)RegistrationNativeError;
#else
        gPhysics_error_cb=h[5]&4?RegistrationNativeError:NULL;(void)RegistrationOriginalError;
#endif
        _controlfp(_PC_53,_MCW_PC);_clearfp();
        builder_mode=0;builder_address=addresses[mode];builder_x=h[1];builder_y=h[2];
        builder_stack[0]=h[3];builder_stack[1]=h[4];
#if defined(REC2_MATCHING)
        builder_cdecl=0;builder_stack_count=(mode==1||mode==3)?2:0;
#else
        builder_cdecl=1;builder_stack_count=(mode==1||mode==3)?4:1;
        builder_stack[2]=h[3];builder_stack[3]=h[4];builder_stack[0]=h[1];builder_stack[1]=h[2];
#endif
        builder_poison=h[6];if(BuilderInvoke())exit(14);
        RegistrationSnapshot(0,builder_result,0);
    }
    fclose(f);return 0;
}
'''

sys.path.insert(0,str(ROOT/'scripts'))
from verify_opponent_route_reconstruction import HASH,pe,raw,sha,words
from verify_tinted_native_reconstruction import extract
from inventory_reconstruction_backlog import mask_c
from verify_physics_joint_application_reconstruction import workspace_block
import verify_phil_registry_reconstruction as registry

BASE=0x22000000
SIZE=0x80000
ROOTS=[('PHILAddObject',0x4b5d40,343),('PHILAddActiveObject',0x4b6210,149),
       ('FUN_004b62b0',0x4b62b0,47),('FUN_004b62e0',0x4b62e0,198)]
EXTRA=[('PHILMakeObjectActive',0x4b6090,259),('ResetCarSpecialVolume',0x4ff530,152),
 ('FindFace',0x41e340,31),('ActorFindFace',0x45ca60,262),('ActorRayPick2D',0x45cb70,531),
 ('PickBoundsTestRay__finteray',0x45cd90,391),('FindHighestCallBack__finteray',0x45cf20,54),
 # Include the genuine detached FSTP/JMP block45d568..45d56b. The
 # catalogue's summed1537-byte chunks are not a contiguous extent.
 ('DRModelPick2D__finteray',0x45cf60,1548),('FindHighestPolyCallBack__finteray',0x45d570,74),
 ('EnablePlingMaterials',0x45d5c0,11),('DisablePlingMaterials',0x45d5d0,11),
 ('BrTransformToMatrix34',0x531870,722),('BrEulerToMatrix34',0x531de0,751),
 ('BrMatrix34Identity',0x5327c0,47),('BrMatrix34Translate',0x532a90,59),
 ('BrMatrix34Inverse',0x532bd0,725)]
MUTABLE={
 0x68b918:('gNet_mode',0xf000,4),0x6940c0:('gPhysics_error_cb',0xf010,4),
 0x676920:('gPhysics_empty_message',0xf020,1),0x6940bc:('gPhysics_scratch_ptr',0xf024,4),
 0x6940c4:('gPhysics_other_buffer',0xf028,4),0x6940c8:('gPhysics_other_buffer_capacity',0xf02c,4),
 0x692dc8:('gPhysics_joints_changed_692dc8',0xf030,4),0x6940a0:('gPhysics_group_counter_6940a0',0xf034,4),
 0x65d010:('gFace_num__car',0xf038,4),0x692dcc:('gPhysics_prepared_objects_692dcc',0xf060,4),
 0x6940a8:('gReduced_object_list',0xf064,4),0x6940ac:('gPhysics_stationary_filter_6940ac',0xf068,4),
 0x692dd0:('gPhysics_bounds_max_x',0xf06c,4),0x692dd4:('gPhysics_bounds_min_x',0xf070,4),
 0x6b09c0:('mattmp1',0x3b00,48),0x6b09f0:('mattmp2',0x3b30,48),
 0x6923e0:('gPHIL_registry',0x40000,2532),0x65d004:('gPHIL_disabled',0xf080,4),
 0x74a5e8:('gPHIL_munging_objects',0xf084,4),0x74a5f8:('gPHIL_doing_physics',0xf088,4),
 0x74a5f0:('gPHIL_list_collision_infos',0xf08c,4),0x761f44:('gDefault_water_spec_vol_real',0xf090,4),
 0x5964c0:('gPling_materials',0xf094,4),0x6861c8:('gTemp_group',0xf098,4),
 0x686188:('gNearest_face',0xf09c,4),0x68618c:('gNearest_model',0xf0a0,4),
 0x6861cc:('gNearest_actor',0xf0a4,4),0x686190:('gNearest_T',0xf0a8,4),
 0x686194:('gNearest_face_group',0xf0ac,4),0x75bb60:('gTrack_actor',0xf0b0,4),
 0x686198:('gPick_model_to_view__finteray',0xf100,48),0x66cba0:('OrderAxes',0xf180,128)}
TABLES={0x4b9230:(0x4b9638,4),0x4b8350:(0x4b8c10,4),0x4b9650:(0x4b9730,5),
        0x531870:(0x531b44,7)}
INDIRECT={0x4c61c0,0x4c5c70,0x4c5cb0,0x45cb70,0x45cf60}
EXIT=0x5784e0
PREPARER=ROOT/'scripts/verify_physics_object_preparation_reconstruction.py'

def preparation_catalog(text):
    """Read only literal original identities; importing producer would run it."""
    env={}
    for node in ast.parse(text).body:
        names={t.id for t in getattr(node,'targets',[]) if isinstance(t,ast.Name)}
        if isinstance(node,ast.Assign) and names&{'ORIGINAL_ENTRIES','WAKE_ENTRIES','PREP_ENTRIES'}:
            exec(compile(ast.Module(body=[node],type_ignores=[]),str(PREPARER),'exec'),env)
        elif isinstance(node,ast.AugAssign) and isinstance(node.target,ast.Name) and node.target.id=='ORIGINAL_ENTRIES':
            exec(compile(ast.Module(body=[node],type_ignores=[]),str(PREPARER),'exec'),env)
    return env['ORIGINAL_ENTRIES']

def original_packet(data,catalog,out):
    nt,base,size,tab=pe(data);cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
    todo=[a for _,a,_ in ROOTS];seen=set();rows=[];patches={};constants={};tables=[];edges=[];callbacks=[]
    sections=[]
    for k in range(struct.unpack_from('<H',data,nt+6)[0]):
        vs,rva,n,off=struct.unpack_from('<IIII',data,tab+40*k+8)
        sections.append((base+rva,max(vs,n),struct.unpack_from('<I',data,tab+40*k+36)[0]))
    # Validate every present HIGHLOW site. This original image is stripped,
    # so absent relocations cannot establish pointer-immediate completeness;
    # MOV/PUSH pointer spellings and canonical-data CMP end pointers below
    # also have explicit instruction/data authority.
    rr,rz=struct.unpack_from('<II',data,nt+24+96+5*8);cursor=base+rr;highlow=set()
    while cursor<base+rr+rz:
        page,n=struct.unpack('<II',raw(data,cursor,8));assert n>=8
        for value in struct.unpack('<'+'H'*((n-8)//2),raw(data,cursor+8,n-8)):
            if value>>12==3:highlow.add(page+(value&4095))
        cursor+=n
    def patch(site,value,target,absolute):
        record=(site-base,value,target,int(absolute))
        assert site-base not in patches or patches[site-base]==record
        patches[site-base]=record
    def transport(value,width):
        for address,(name,offset,length) in MUTABLE.items():
            if address<=value<address+length:return BASE+offset+value-address,True
        region=next(s for s in sections if s[0]<=value<s[0]+s[1])
        assert not region[2]&0x80000000 or value==0x66cc20 and raw(data,value,12)==words([1,2,0]) or value==0x65d01c and raw(data,value,22)==b'Stupid trial position\0',hex(value)
        if value==0x65d01c:width=22
        if value==0x66cc20:width=12
        constants[hex(value)]=raw(data,value,width).hex()
        return value-base,False
    while todo:
        start=todo.pop(0)
        if start in seen or start==EXIT:continue
        assert start in catalog,('unknown lower',hex(start))
        seen.add(start);name,n=catalog[start];instructions=list(cs.disasm(raw(data,start,n),start))
        assert sum(i.size for i in instructions)==n,(name,n)
        for ins in instructions:
            control=CS_GRP_CALL in ins.groups or CS_GRP_JUMP in ins.groups
            if control:
                operand=ins.operands[0]
                if operand.type==CS_OP_IMM:
                    target=operand.imm
                    if not(start<=target<start+n):
                        assert target==EXIT or target in catalog,(name,hex(ins.address),ins.op_str)
                        edges.append(dict(site=ins.address,target=target,terminal=target==EXIT));todo.append(target)
                elif CS_GRP_JUMP in ins.groups:
                    assert start in TABLES and operand.type==CS_OP_MEM
                    at,count=TABLES[start]
                    assert operand.mem.disp==at and operand.mem.scale==4
                    targets=struct.unpack('<'+'I'*count,raw(data,at,4*count))
                    assert all(start<=t<start+n for t in targets)
                    for j,target in enumerate(targets):patch(at+4*j,target,target-base,False)
                    tables.append(dict(owner=start,site=ins.address,address=at,count=count,targets=targets,sha256=sha(raw(data,at,4*count))))
                else:
                    assert start in INDIRECT,('unknown indirect',name,hex(ins.address),ins.op_str)
                    callbacks.append(dict(owner=start,site=ins.address,operand=ins.op_str))
            for op in ins.operands:
                if op.type==CS_OP_MEM and base<=op.mem.disp<base+size:
                    assert ins.disp_size==4
                    value=op.mem.disp;width=op.size
                    if start in TABLES and value==TABLES[start][0]:target=value-base;absolute=False
                    else:target,absolute=transport(value,12 if op.mem.base or op.mem.index else width)
                    patch(ins.address+ins.disp_offset,value,target,absolute)
                elif op.type==CS_OP_IMM and not control and (ins.address+ins.imm_offset-base in highlow or
                     ins.mnemonic in('mov','push')and base<=op.imm<base+size or
                     ins.mnemonic=='cmp'and any(at<=op.imm<at+length for at,(symbol,offset,length)in MUTABLE.items())):
                    assert ins.imm_size==4;value=op.imm
                    if value in catalog:
                        todo.append(value);target=value-base;absolute=False
                        edges.append(dict(site=ins.address,target=value,callback_pointer=True))
                    else:target,absolute=transport(value,4)
                    patch(ins.address+ins.imm_offset,value,target,absolute)
        rows.append(dict(name=name,address=start,bytes=n,sha256=sha(raw(data,start,n)),instruction_count=len(instructions)))
    # Authenticate every active HIGHLOW relocation, including code table words.
    active=set()
    for row in rows:active.update(range(row['address'],row['address']+row['bytes']))
    for table in tables:active.update(range(table['address'],table['address']+table['count']*4))
    checked=[]
    for site in sorted(highlow):
        if base+site in active:assert site in patches,hex(site);checked.append(site)
    packet=words([a-base for _,a,_ in ROOTS]+[len(patches),EXIT-base])+b''.join(words(v)for _,v in sorted(patches.items()))
    (out/'original-addresses.bin').write_bytes(packet)
    meta=dict(graph=rows,edges=edges,indirect_sites=callbacks,tables=tables,constants=constants,patches=list(patches.values()),relocations_checked=checked,packet_sha256=sha(packet),terminal_exit=EXIT,mutable_globals=MUTABLE)
    (out/'original-packet.json').write_text(json.dumps(meta,indent=2)+'\n');return meta

def preparation_body(before,records):
    """Execute only the reviewed extraction statements, not producer/runtime.

    This dependency carries no fixtures, oracle execution or output rewrite.
    Exact selected source segments are saved with the generated proof inputs.
    """
    physics=ROOT/'src/carma2/newcommon/49-physics.c';matrix=ROOT/'src/brender/core/math/matrix34.c'
    geometry=ROOT/'src/carma2/newcommon/34-finteray.c';phil=ROOT/'src/carma2/newcommon/60-phil.c';quat=ROOT/'src/brender/core/math/quat.c'
    text=before[str(PREPARER)].decode();a=text.index('foundation=');b=text.index('native=COMMON',a)
    capsule=text[a:b]
    env=dict(ROOT=ROOT,extract=extract,mask_c=mask_c,re=re,workspace_block=workspace_block,before=before,
             physics=physics,matrix=matrix,geometry=geometry,phil=phil,quat=quat,candidate=physics,overlap=physics,
             bounds_candidate=physics,args=SimpleNamespace(matrix_rotate_candidate=None,matrix_post_candidate=None),
             WAKE_ENTRIES=[r for r in records if r[1]in(0x4bae80,0x4bddd0,0x4ba190,0x4be0e0)],
             PREP_ENTRIES=[r for r in records if r[1]in(0x4b99e0,0x4b9770)],
             spec=[dict(name=n,address=a,bytes=s)for n,a,s in records])
    exec(compile(capsule,str(PREPARER)+'[lower-extraction]','exec'),env)
    return env['body'],capsule

def native_source(before,records,candidate):
    body,capsule=preparation_body(before,records)
    phil=before[str(ROOT/'src/carma2/newcommon/60-phil.c')].decode()
    volume=before[str(ROOT/'src/carma2/newcommon/55-volume.c')].decode()
    geometry=before[str(ROOT/'src/carma2/newcommon/34-finteray.c')].decode()
    matrix=before[str(ROOT/'src/brender/core/math/matrix34.c')].decode()
    trans=before[str(ROOT/'src/brender/core/math/transfrm.c')].decode().replace('\r\n','\n')
    angles=before[str(ROOT/'src/brender/core/math/angles.c')].decode()
    body+='\n'+extract(phil,'PhilCopyWords')+'\n'+extract(phil,'PHILMakeObjectActive')
    body+='\n'+'\n'.join(extract(volume,n)for n in('VolumeIdentifierHasLength','VolumeBelowSurfaceLimit','ResetCarSpecialVolume'))
    geometry_names=[n for n,a,s in EXTRA if a in range(0x45ca60,0x45d5e0)]
    body+='\n'+'\n'.join(extract(geometry,n)for n in geometry_names)
    body+='\n'+extract(before[str(ROOT/'src/carma2/newcommon/00-car.c')].decode(),'FindFace')
    body+='\n'+'\n'.join(extract(matrix,n)for n in('ActivationInverseNaN','ActivationInverseAdd','ActivationInverseRead','ActivationInverseScalar','ActivationInverseWrite','BrMatrix34Inverse','BrMatrix34Identity'))
    # Complete original Euler and transform dispatch. No old fixture dispatcher.
    start=trans.index('#if defined(__GNUC__)\ntypedef br_uint_32 tActivation_transform_word')
    last=extract(trans,'BrTransformToMatrix34');body+='\n'+trans[start:trans.index(last)+len(last)]
    body+='\n'+'\n'.join(extract(angles,n)for n in('ActivationEulerScalar','ActivationEulerCosine','ActivationEulerSine','BrEulerToMatrix34'))
    text=before[str(candidate)].decode()
    if candidate==SOURCE:
        match=re.search(r'#if defined\(__GNUC__\)\s+typedef tU32 tRegistrationWord[^\n]*;\s*#else\s*typedef tU32 tRegistrationWord;\s*#endif',text)
        assert match is not None,'Missing genuine RegistrationWord capsule'
        # Canonical roots are separated by existing published PHIL bodies.
        # Capture only the exact private typedef/helpers and four real roots,
        # never the unrelated code between their production positions.
        names=('RegistrationAddress','RegistrationRead','RegistrationWrite','RegistrationByte','RegistrationWriteByte','RegistrationPointer','RegistrationCopyWords')+tuple(name for name,address,size in ROOTS)
        text=match.group(0)+'\n'+'\n'.join(extract(text,name)for name in names)
    body+='\n'+text
    common=COMMON
    driver='\nint C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;'+''.join('addresses[%d]=(unsigned)%s;'%(i,n)for i,(n,a,s)in enumerate(ROOTS))+'return run(argv[1]);}\n'
    return common+'\n'+body+driver,capsule

def bridge():
    caller,physical,_=registry.literals()
    old=' cmp builder_mode,8\n jne notwide\n fstp qword ptr builder_wide\n xor eax,eax\n notwide:\n cmp builder_mode,1\n je zeroreturn\n cmp builder_mode,10\n jne resultready\n zeroreturn:\n xor eax,eax\n resultready:'
    assert old in caller;caller=caller.replace(old,' resultready:')
    caller=caller.replace('builder_poison;','builder_poison,builder_cdecl;')
    old=' mov builder_result,eax\n mov ecx,esp';assert old in caller
    caller=caller.replace(old,' mov builder_result,eax\n cmp builder_cdecl,0\n je registration_stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n registration_stackready:\n mov ecx,esp')
    old='image[hdr[12]]=0xe9;relative=(unsigned)RayBoundary-((unsigned)image+hdr[12]+5);memcpy(image+hdr[12]+1,&relative,4);'
    assert old in physical
    physical=physical.replace(old,'image[hdr[5]]=0xe9;relative=(unsigned)HierarchyExit-((unsigned)image+hdr[5]+5);memcpy(image+hdr[5]+1,&relative,4);')
    physical=physical.replace('hdr[14]','hdr[6]').replace('n=hdr[13]','n=hdr[4]').replace('i<12;i++)addresses','i<4;i++)addresses')
    return caller,physical

def preparation_fixture():
    text=PREPARER.read_text();tree=ast.parse(text);node=next(n for n in tree.body if isinstance(n,ast.FunctionDef)and n.name=='make_case')
    env=dict(random=random,words=words,struct=struct,BASE=BASE,OBJECTS=(0x100,0x900,0x1100,0x1900),ACTORS=(0x5000,0x5100,0x5200,0x5300))
    exec(compile(ast.Module(body=[node],type_ignores=[]),str(PREPARER)+'[fixture]','exec'),env)
    return env['make_case']

def fixtures():
    result=[]
    def add(**v):
        c=dict(mode=0,disabled=0,munging=0,added=0,doing=0,count=0,filled=0,duplicate=-1,head=0x900,
          matrix=0,velocity=0x6200,omega=0x6300,tree=0,kind=1,network=0,callback=0,capacity=0x6000,
          patches=[],bytes=[],object=0x100,seed=len(result)+1,geometry=0,transform=0,water=1)
        c.update(v);result.append(c)
    for mode in range(4):
        for disabled in(0,1,0xffffffff,0x80000000):
            for munging,added in((0,0),(1,0),(1,1),(1,0xffffffff)):
                add(mode=mode,disabled=disabled,munging=munging,added=added,matrix=0x6000 if mode&1 else 0)
        for filled,duplicate in((0,-1),(100,-1),(0,0),(0,99),(100,50),(20,90)):
            add(mode=mode,filled=filled,duplicate=duplicate)
        for head in(0,0x900,0x100,0x40000-0x220,0xf08c-0x220):add(mode=mode,head=head)
        for count in(0,1,24,25,99,100,628,0xffffffff,0x80000000):add(mode=mode,munging=1,count=count)
    ieee=(0,0x80000000,1,0x80000001,0x3f800001,0x4b800001,0x7f7fffff,0x7f800000,0xff800000,0x7f812345,0xff812345,0x7fc12345,0xffc12345)
    for mode in(1,3):
        for lane in range(12):
            for value in ieee:add(mode=mode,matrix=0x6000,patches=[(0x6000+4*lane,value)])
        for matrix in(0,0x502c-8,0x502c-4,0x502c,0x502c+4,0x502c+8,0x40040-4,0x40040,0x40040+4):
            for munging in(0,1):add(mode=mode,munging=munging,matrix=matrix)
        for v in(0,0x168-4,0x168,0x168+4,0x6200,0x40028):
            for w in(0,v,0x174-4,0x174+4,0x6300):add(mode=mode,velocity=v,omega=w)
        for transform in range(7):
            for geometry in(0,1):add(mode=mode,geometry=geometry,transform=transform,matrix=0x6000)
    for mode in range(4):
        for tree in range(4):
            for kind in range(6):
                for network in(0,1,2):add(mode=mode,doing=1,tree=tree,kind=kind,network=network)
        for capacity in(0,1,252,708,0x6000):
            for callback in(0,4,4|2|8|16|32):add(mode=mode,doing=1,tree=2,capacity=capacity,callback=callback,patches=[(0xf040,0xffffffff),(0xf048,0x324),(0xf04c,0)])
        for object_ in(0,0x7fffc,0x7fe00,0x40000,0xf08c):add(mode=mode,object=object_)
    return result

def make_case(c,data,make_preparation):
    options=dict(mode=1,object_mode=0,disabled=0,mass=0x3f800000,scale=False,tree=c['tree'],network=c['network'],seed=c['seed'],callback=0,capacity=c['capacity'],subject=0,list_nodes=(1,),positions=(1.,2.,3.,4.),stationary=0,reduced=0,parent=False,patches=[],child_type=c['kind'],actor_matrix_at=None)
    initial=make_preparation(options);arena=bytearray(initial[28:])+bytearray(SIZE-65536)
    def put(at,*values):arena[at:at+len(values)*4]=words(values)
    def matrix(at,tx=0.,ty=0.,tz=0.):arena[at:at+48]=struct.pack('<12f',1,0,0,0,1,0,0,0,1,tx,ty,tz)
    if c['doing']:
        for object_ in(0x100,0x900,0x1100,0x1900):put(object_+0x19c,0)
    head=0x2900 if c['doing']and c['tree']and c['head']==0x900 else c['head']
    if head==0x2900:
        # A list head distinct from every child avoids insertion producing
        # child.next=root. The original-only c00000fd case598 is retained,
        # not reclassified as a normal return or numerical success.
        arena[head:head+1240]=bytes(1240);put(head,BASE+0x5400);put(head+4,0x3f800000)
        put(head+0x19c,0x400);put(head+0x1d8,BASE+head);matrix(0x5400+0x2c,5.,0.,0.)
        matrix(head+0x8c,5.,0.,0.);matrix(head+0xbc,5.,0.,0.);matrix(head+0x1e8,5.,0.,0.)
        arena[head+0x24:head+0x54]=struct.pack('<12f',-.5,-.75,-1.,.5,.75,1.,-.5,-.75,-1.,.5,.75,1.)
    put(0xf000,c['network']);put(0xf080,c['disabled'],c['munging'],c['doing'],BASE+head if head else 0,BASE+0x20000 if c['water']else 0,1)
    arena[0x40000:0x40000+2532]=b'\xa5'*2532
    for j in range(100):put(0x40078+j*24,BASE+0x20000+j*4 if j<c['filled']else 0)
    if c['duplicate']>=0:put(0x40078+24*c['duplicate'],BASE+c['object']if c['object']else 0)
    put(0x40000,7,c['added']);put(0x409e0,c['count'])
    matrix(0x6000,0.25,-2.,0.25);put(0x6200,0x3e000000,0xbe800000,0x3ec00000);put(0x6300,0x3dcccccd,0x3e4ccccd,0x3e99999a)
    # Actual geometry root and a triangle prepared group, independent of the
    # old controlled transform dispatcher. All seven transform tags execute
    # complete BrTransformToMatrix34 and genuine Euler/quaternion bodies.
    track=0x22000;child=0x22100;model=0x22400;prepared=0x22600;group=0x22800
    arena[track:track+100]=bytes(100);arena[child:child+100]=bytes(100)
    put(0xf0b0,BASE+track);matrix(track+0x2c);put(track+8,BASE+child if c['geometry']else 0)
    struct.pack_into('<H',arena,track+0x28,c['transform'])
    matrix(child+0x2c);put(child+24,BASE+model);put(child+28,BASE+0x23600)
    arena[model:model+92]=bytes(92);put(model+76,BASE+prepared)
    arena[model+52:model+76]=struct.pack('<6f',-10,-10,-10,10,10,10)
    arena[prepared:prepared+28]=bytes(28);struct.pack_into('<H',arena,prepared+8,1);put(prepared+24,BASE+group)
    arena[group:group+36]=bytes(36);put(group+4,BASE+0x23100);put(group+8,BASE+0x23200);put(group+16,BASE+0x23000)
    struct.pack_into('<2H',arena,group+28,1,3);put(0x23200,BASE+0x23600)
    for j,position in enumerate(((0.,0.,0.),(1.,0.,0.),(0.,0.,1.))):
        off=0x23000+32*j;arena[off:off+32]=bytes(32);arena[off:off+12]=struct.pack('<3f',*position)
    arena[0x23100:0x23100+28]=bytes(28);struct.pack_into('<3H',arena,0x23100,0,1,2)
    arena[0x2310c:0x2311c]=struct.pack('<4f',0.,-1.,0.,0.)
    arena[0x23600:0x23600+156]=bytes(156);put(0x23604,BASE+0x23700);arena[0x23700:0x23707]=b'!water\0'
    put(0xf180,*struct.unpack('<32I',raw(data,0x66cba0,128)))
    for at,value in c['patches']:put(at,value)
    for at,value in c['bytes']:arena[at]=value&255
    return words([c['mode'],BASE+c['object']if c['object']else 0,BASE+c['matrix']if c['matrix']else 0,BASE+c['velocity']if c['velocity']else 0,BASE+c['omega']if c['omega']else 0,c['callback'],0xcccccccc,0])+arena

def matching_packet(image_path,symbol_path,original,before,out):
    data=image_path.read_bytes();mapping=json.loads(symbol_path.read_text())
    assert mapping['format']==1 and mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(data)
    symbols={r['orig']:r['recomp']for r in mapping['symbols']}
    map_bytes=image_path.with_suffix('.map').read_bytes();map_text=map_bytes.decode('latin1')
    functions={int(a,16):(name,owner.strip())for name,a,owner in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',map_text,re.M)}
    nt,base,size,tab=pe(data);cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
    sections=[]
    for k in range(struct.unpack_from('<H',data,nt+6)[0]):
        vs,rva,n,off=struct.unpack_from('<IIII',data,tab+40*k+8)
        sections.append((base+rva,max(vs,n),struct.unpack_from('<I',data,tab+40*k+36)[0]))
    fallbacks=[];mutable={}
    all_sources='\n'.join(bits.decode('latin1')for path,bits in before.items()if path.endswith('.c'))
    for identity,(name,offset,length)in MUTABLE.items():
        exact=re.findall(r'^\s*000[23]:[0-9a-fA-F]{8}\s+_'+re.escape(name)+r'\s+([0-9a-fA-F]{8})\s+(.+)$',map_text,re.M)
        assert len(exact)==1,(name,exact)
        address=int(exact[0][0],16);owner=exact[0][1].strip();region=next(s for s in sections if s[0]<=address<s[0]+s[1])
        assert region[2]&0x80000000 and not region[2]&0x20
        if identity in symbols:assert symbols[identity]==address,(name,'identity/map disagreement')
        else:
            # Canonical MAP name plus actual declaration, never nearest-symbol.
            if name=='OrderAxes':assert re.search(r'order_info\s+OrderAxes\[32\]',all_sources)
            elif name in('mattmp1','mattmp2'):assert re.search(r'br_matrix34\s+mattmp1\s*,\s*mattmp2',all_sources)
            else:assert re.search(r'// GLOBAL: CARMA2_HW 0x00'+format(identity,'06x')+r'\s+[^;{}]*\b'+re.escape(name)+r'\b',all_sources)
            symbols[identity]=address;fallbacks.append(dict(original=identity,recompiled=address,name=name,owner=owner,bytes=length))
        mutable[address]=(BASE+offset,length)
    initial={symbols[r['address']]:r['name']for r in original['graph']if r['address']in symbols}
    roots=[symbols[address]for name,address,n in ROOTS]
    exit_address=symbols.get(EXIT)
    if exit_address is None:
        exits=[a for a,(name,owner)in functions.items()if name=='_exit'and owner.startswith('LIBC:')]
        assert len(exits)==1;exit_address=exits[0]
    allowed_owners={'49-physics.c.obj','60-phil.c.obj','55-volume.c.obj','00-car.c.obj','34-finteray.c.obj',
                    'libbrender:matrix34.c.obj','libbrender:quat.c.obj','libbrender:angles.c.obj','libbrender:transfrm.c.obj','libbrender:vector.c.obj'}
    private_prefix=('Registration','Physics','Rotation','Matrix','PhilOrientation','Quaternion','Partner','Velocity','Application','Force','RQ','JointedMotion','Wake','Friction','Prepare','LP','Finteray','Activation','Volume')
    todo=list(roots);seen=set();graph=[];patches={};constants={};tables=[];callbacks=[];edges=[]
    def patch(site,value,target,absolute):
        row=(site-base,value,target,int(absolute));assert site-base not in patches or patches[site-base]==row
        patches[site-base]=row
    while todo:
        start=todo.pop(0)
        if start in seen:continue
        seen.add(start);assert start in functions,hex(start)
        name,owner=functions[start];plain=name.lstrip('_@').split('@')[0]
        if start not in initial:
            assert owner in allowed_owners and plain.startswith(private_prefix)or plain in('ftol','_ftol')and owner=='LIBC:ftol.obj',(name,owner)
        end=min(a for a in functions if a>start);pending=[start];instructions={}
        occupied={};decoded={};successors={};register_calls={};frame_calls={}
        while pending:
            address=pending.pop()
            while address not in instructions:
                assert start<=address<end,(name,hex(address),hex(end))
                ins=next(cs.disasm(raw(data,address,15),address))
                assert ins.size>0 and address+ins.size<=end,('Instruction crosses function extent',name,hex(address),ins.size,hex(end))
                assert all(at not in occupied for at in range(address,address+ins.size)),('Overlapping decoded instruction',name,hex(address))
                for at in range(address,address+ins.size):occupied[at]=address
                instructions[address]=ins.bytes.hex();decoded[address]=ins
                successors[address]=[] if CS_GRP_RET in ins.groups or ins.mnemonic in('int3','int') else [address+ins.size]
                control=CS_GRP_CALL in ins.groups or CS_GRP_JUMP in ins.groups;is_table=False
                if control:
                    op=ins.operands[0]
                    if op.type==CS_OP_IMM:
                        target=op.imm
                        if start<=target<end:
                            if CS_GRP_CALL in ins.groups:assert target==start,('Unexpected interior call',name,ins.op_str)
                            else:pending.append(target);successors[address].append(target)
                        elif target!=exit_address:
                            assert target in functions,(name,ins.op_str);todo.append(target)
                        edges.append(dict(site=address,target=target,terminal=target==exit_address))
                        if ins.mnemonic=='jmp':successors[address]=[target] if start<=target<end else []
                    elif CS_GRP_JUMP in ins.groups:
                        assert op.type==CS_OP_MEM and not op.mem.base and op.mem.index and op.mem.scale==4
                        identities=[identity for identity in TABLES if symbols.get(identity)==start]
                        assert len(identities)==1,('Unproved table domain',name,ins.op_str)
                        count=TABLES[identities[0]][1];at=op.mem.disp;targets=struct.unpack('<'+'I'*count,raw(data,at,count*4))
                        assert all(start<=target<end for target in targets);pending.extend(targets);is_table=True
                        successors[address]=list(targets)
                        for k,target in enumerate(targets):patch(at+4*k,target,target-base,False)
                        tables.append(dict(owner=start,address=at,count=count,targets=list(targets),domain_source_original=identities[0],sha256=sha(raw(data,at,count*4))))
                    elif op.type==CS_OP_MEM:
                        assert start in tuple(symbols[a]for a in(0x45cb70,0x45cf60)) and not op.mem.index
                        if op.mem.base==X86_REG_ESP:
                            callbacks.append(dict(site=address,owner=start,kind='real-geometry-stack-callback',operand=ins.op_str))
                        else:
                            # DRModel's double-aligned local frame changes the
                            # incoming callback spelling, not its identity:
                            # entryESP+18h == (entryESP-4)+1ch after55/8bec.
                            assert start==symbols[0x45cf60] and cs.reg_name(op.mem.base)=='ebp' and op.mem.disp==0x1c and op.size==4
                            assert raw(data,start,3)==b'\x55\x8b\xec'
                            frame_calls[address]=op
                    else:
                        assert op.type==CS_OP_REG
                        register_calls[address]=op.reg
                for op in ins.operands:
                    value=None;width=4
                    if op.type==CS_OP_MEM and base<=op.mem.disp<base+size:
                        value=op.mem.disp;site=address+ins.disp_offset;assert ins.disp_size==4
                        width=4*count if is_table else 12 if op.mem.base or op.mem.index else op.size
                    elif op.type==CS_OP_IMM and not control and base<=op.imm<base+size and(ins.mnemonic in('mov','push')or ins.mnemonic=='cmp'and any(at<=op.imm<at+n for at,(dest,n)in mutable.items())):
                        value=op.imm;site=address+ins.imm_offset;assert ins.imm_size==4
                    if value is None:continue
                    head=next((at for at,(dest,n)in mutable.items()if at<=value<at+n),None)
                    if head is not None:target=mutable[head][0]+value-head;absolute=True
                    elif value in functions:
                        assert op.type==CS_OP_IMM,('Code-data ambiguity',name,ins.op_str)
                        todo.append(value);target=value-base;absolute=False;edges.append(dict(site=address,target=value,callback_pointer=True))
                    elif is_table:target=value-base;absolute=False
                    else:
                        region=next(s for s in sections if s[0]<=value<s[0]+s[1])
                        assert not region[2]&0x20
                        if region[2]&0x80000000:
                            assert raw(data,value,22)==b'Stupid trial position\0',(name,ins.op_str);width=22
                        bits=raw(data,value,width);constants[hex(value)]=bits.hex();target=value-base;absolute=False
                        if op.type==CS_OP_MEM and(op.mem.base or op.mem.index):assert bits==words([1,2,0]),('Unknown indexed constants',name,ins.op_str)
                    patch(site,value,target,absolute)
                if CS_GRP_RET in ins.groups or ins.mnemonic in('jmp','int3','int'):break
                address+=ins.size
        # All successors must be genuine decoded starts. This independently
        # rejects a branch/table target inside an already decoded instruction,
        # even if the target was discovered before that containing stream.
        assert all(target in decoded for targets in successors.values()for target in targets),('Noninstruction CFG successor',name)
        if frame_calls:
            # No back/alternate edge may establish the frame by reentering
            # MOV EBP,ESP without the unique incoming PUSH EBP.
            assert all(start not in targets for targets in successors.values()),('Incoming frame prologue reentry',name)
            assert {at for at,targets in successors.items()if start+1 in targets}=={start}
        families={alias:family for family,aliases in {
            'eax':('eax','ax','al','ah'),'ebx':('ebx','bx','bl','bh'),
            'ecx':('ecx','cx','cl','ch'),'edx':('edx','dx','dl','dh'),
            'esi':('esi','si'),'edi':('edi','di'),'ebp':('ebp','bp'),'esp':('esp','sp')}.items()for alias in aliases}
        # Forward must-dataflow over the actual CFG: a retained definition
        # must dominate the call on every path, including loop/back edges.
        # Instruction order alone cannot prove callback provenance.
        inputs={start:{}};work=[start]
        while work:
            at=work.pop(0);ins=decoded[at];state=dict(inputs[at]);read,written=ins.regs_access()
            for reg in written:
                family=families.get(cs.reg_name(reg))
                if family is not None:state.pop(family,None)
                if family=='ebp':state.pop('incoming-frame-ebp',None)
            if CS_GRP_CALL in ins.groups:
                # Normal x86 ABI callees may clobber these full families even
                # when Capstone reports only the architectural ESP write.
                for family in('eax','ecx','edx'):state.pop(family,None)
            if ins.mnemonic=='mov'and len(ins.operands)==2:
                dest,source=ins.operands;family=families.get(cs.reg_name(dest.reg))if dest.type==CS_OP_REG else None
                if family is not None and dest.size==4 and cs.reg_name(dest.reg)==family and source.type==CS_OP_MEM and not source.mem.base and not source.mem.index and source.mem.disp==symbols[0x6940c0]:
                    state[family]=at
            if frame_calls and at==start+1:
                assert ins.bytes==b'\x8b\xec'
                state['incoming-frame-ebp']=at
            for target in successors[at]:
                previous=inputs.get(target)
                merged=dict(state)if previous is None else {family:site for family,site in previous.items()if state.get(family)==site}
                if previous is None or merged!=previous:inputs[target]=merged;work.append(target)
        for at,reg in sorted(register_calls.items()):
            family=families.get(cs.reg_name(reg));load=inputs.get(at,{}).get(family)
            assert family==cs.reg_name(reg)and load is not None,('No dominating live full-GPR error-callback load',name,hex(at),cs.reg_name(reg))
            callbacks.append(dict(site=at,owner=start,kind='external-error',load=load,register=family,authority='Actual CFG must-dataflow; full/partial-family write and call-volatile kills; identical definition on all predecessors'))
        for at,operand in sorted(frame_calls.items()):
            assert inputs.get(at,{}).get('incoming-frame-ebp')==start+1,('Incoming frame does not dominate callback',name,hex(at))
            callbacks.append(dict(site=at,owner=start,kind='real-geometry-incoming-frame-callback',operand=decoded[at].op_str,frame_load=start+1,ebp_entry_delta=-4,entry_stack_offset=0x18,authority='Mapped DRModel only; literal55/8bec unique predecessor; full CFG must-dataflow with every EBP-family write kill; normal ABI preserves EBP across calls'))
        graph.append(dict(name=initial.get(start,name),map_name=name,owner=owner,address=start,extent=end-start,instructions=sorted(instructions.items()),instruction_sha256=sha(b''.join(bytes.fromhex(code)for at,code in sorted(instructions.items())))))
    active={at+k for row in graph for at,code in row['instructions']for k in range(len(code)//2)}
    for table in tables:active.update(range(table['address'],table['address']+table['count']*4))
    rr,rz=struct.unpack_from('<II',data,nt+24+96+5*8);cursor=base+rr;checked=[]
    while cursor<base+rr+rz:
        page,n=struct.unpack('<II',raw(data,cursor,8));assert n>=8
        for word in struct.unpack('<'+'H'*((n-8)//2),raw(data,cursor+8,n-8)):
            if word>>12==3:
                site=page+(word&4095)
                if base+site in active:assert site in patches,hex(site);checked.append(site)
        cursor+=n
    packet=words([a-base for a in roots]+[len(patches),exit_address-base])+b''.join(words(v)for _,v in sorted(patches.items()))
    (out/'matching-addresses.bin').write_bytes(packet)
    return dict(recompiled_sha256=sha(data),symbols_sha256=sha(symbol_path.read_bytes()),map_sha256=sha(map_bytes),graph=graph,edges=edges,callbacks=callbacks,tables=tables,constants=constants,patches=list(patches.values()),relocations_checked=checked,global_fallbacks=fallbacks,mutable_globals=mutable,packet_sha256=sha(packet),crt_exit=exit_address)

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',required=True,type=Path);p.add_argument('--candidate-source',type=Path,default=SOURCE);p.add_argument('--case-index',type=int,action='append');p.add_argument('--limit',type=int);p.add_argument('--package-only',action='store_true');p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);a=p.parse_args();assert bool(a.recompiled)==bool(a.symbols)
    out=a.output.resolve();assert not out.exists(),'Fresh output directory required';out.mkdir(parents=True)
    paths=[Path(__file__),a.candidate_source,PREPARER,ROOT/'scripts/verify_phil_registry_reconstruction.py',ROOT/'scripts/verify_physics_joint_application_reconstruction.py',ROOT/'scripts/verify_tinted_native_reconstruction.py',ROOT/'scripts/verify_opponent_route_reconstruction.py',ROOT/'scripts/verify_opponent_navigation_reconstruction.py',ROOT/'scripts/inventory_reconstruction_backlog.py']
    paths += [ROOT/'src/carma2/newcommon'/name for name in('49-physics.c','60-phil.c','55-volume.c','34-finteray.c','00-car.c')]
    paths += [ROOT/'src/brender/core/math'/name for name in('matrix34.c','quat.c','vector.c','transfrm.c','angles.c')]
    paths += list((ROOT/'src').rglob('*.h'));paths=list(dict.fromkeys(f.resolve()for f in paths))
    if a.recompiled:paths.extend([a.recompiled.resolve(),a.symbols.resolve(),a.recompiled.with_suffix('.map').resolve()])
    before={str(f):f.read_bytes()for f in paths};snap=out/'sources';snap.mkdir();snapshots={}
    for j,(path,bits)in enumerate(before.items()):
        target=snap/(str(j)+'-'+Path(path).name);target.write_bytes(bits);snapshots[path]=dict(path=str(target),sha256=sha(bits))
    data=(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes();assert sha(data)==HASH
    records=preparation_catalog(before[str(PREPARER)].decode());catalog={addr:(name,n)for name,addr,n in records+EXTRA+ROOTS}
    original=original_packet(data,catalog,out);native,capsule=native_source(before,records,a.candidate_source.resolve());caller,physical=bridge()
    matching=matching_packet(a.recompiled.resolve(),a.symbols.resolve(),original,before,out)if a.recompiled else None
    common=COMMON;(out/'native.c').write_text(native);(out/'caller.c').write_text(caller);(out/'physical.c').write_text('#define PHYSICAL_IMAGE\n'+common+physical);(out/'preparation-extraction.py').write_text(capsule)
    cases=fixtures();maker=preparation_fixture();indices=a.case_index or list(range(len(cases)));indices=indices[:a.limit]if a.limit else indices
    inputs=[make_case(cases[i],data,maker)for i in indices]
    package=dict(stage='Packaged only; no runtime/acceptance',original_sha256=HASH,original=original,matching=matching,total_scenes=len(cases),selected_scenes=len(indices),fixture_sha256=[sha(b)for b in inputs],sources={k:sha(v)for k,v in before.items()},source_snapshots=snapshots,limitations=__doc__)
    (out/'package.json').write_text(json.dumps(package,indent=2)+'\n')
    if a.package_only:
        assert all(Path(path).read_bytes()==bits for path,bits in before.items()),'Packaging source changed'
        return
    registry.COMMON='#define PHYSICAL_IMAGE\n'+common
    try:commands=registry.compile_sources(out,native,physical,caller)
    except Exception:
        stable=all(Path(path).read_bytes()==bits for path,bits in before.items())
        (out/'compile-guard-closure.json').write_text(json.dumps(dict(source_stable=stable,sources_before={k:sha(v)for k,v in before.items()},sources_after={k:sha(Path(k).read_bytes())for k in before}),indent=2)+'\n')
        raise
    results=[];failures=[]
    for index,payload in zip(indices,inputs):
        case=out/'case.bin';case.write_bytes(payload)
        runs=[('original',[str(out/'physical.exe'),str(case),str(ROOT/'build-reference/originals/CARMA2_HW.EXE'),str(out/'original-addresses.bin')])]+[(label,[str(out/(label+'.exe')),str(case)])for label in('msvc5','msvc','mingw')]
        if matching:runs.append(('matching',[str(out/'physical.exe'),str(case),str(a.recompiled.resolve()),str(out/'matching-addresses.bin')]))
        expected=None
        for label,command in runs:
            r=subprocess.run(command,capture_output=True,timeout=30);target=out/('%d-%s.bin'%(index,label));target.write_bytes(r.stdout)
            if label=='original':assert r.returncode==0;expected=r.stdout;assert expected and len(expected)%(16+SIZE)==0;continue
            row=dict(index=index,compiler=label,fixture_sha256=sha(payload),original_output_sha256=sha(expected),original_output_bytes=len(expected),actual_output_sha256=sha(r.stdout),actual_output_bytes=len(r.stdout),exit=r.returncode,passed=r.returncode==0 and r.stdout==expected)
            results.append(row)
            if not row['passed']:failures.append(row);(out/('failure-%d-%s.json'%(index,label))).write_text(json.dumps(row,indent=2))
        print(index,len(results),len(failures),flush=True)
    stable=all(Path(path).read_bytes()==bits for path,bits in before.items());assert stable,'Source changed while proof active'
    package.update(stage='Complete original/native3 run',source_stable=stable,scenes=len(indices),comparisons=len(results),results=results,failures=failures,commands=commands,generated_sha256={str(f):sha(f.read_bytes())for f in out.iterdir()if f.suffix in('.c','.obj','.exe','.bin','.txt')})
    (out/'verification.json').write_text(json.dumps(package,indent=2)+'\n');assert not failures

if __name__=='__main__':main()
