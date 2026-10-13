"""Physical original/native/linked proof of the complete force-group foundation.

Five genuine entries: 4bc8e0/292B,4bd720/203B,4bd7f0/51B,4bd830/374B,
4bd9b0/42B. Numeric names remain neutral pending lexical evidence. Default
extraction uses production49,34 and actual BRender/application/force/QR bodies;
--candidate is explicitly a development source. No missing function substitute.
Raw arena, bounds globals, callback event/state snapshots and semantic full-EAX
returns compare physically at PC53. Every normal call checks ESP/nonvolatiles/
x87TOP. Void EAX, sticky flags/unmasked traps and nonx86 are outside the claim.
Retains871 bounded finite/IEEE/partner/container/alias/threshold/recursive scenes.
The higher4baec0 consumer and missing contact/generation paths remain incomplete.
"""
import argparse, ast, hashlib, json, os, random, re, struct, subprocess, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parent
while not (ROOT / "src").is_dir(): ROOT = ROOT.parent
sys.path.insert(0, str(ROOT / "scripts"))
sys.path.insert(0, str(ROOT / "build-audit/joystick/python-deps"))
from capstone import *
from verify_tinted_native_reconstruction import extract
from verify_opponent_route_reconstruction import HASH, sha, pe, raw
from verify_physics_joint_application_reconstruction import workspace_block
import verify_phil_registry_reconstruction as registry

def compile_group_linked(output,recompiled,symbols_path,spec,prefix,driver):
    data=recompiled.read_bytes();mapping=json.loads(symbols_path.read_text())
    assert mapping['format']==1 and mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(data)
    symbols={r['orig']:r['recomp'] for r in mapping['symbols']}
    map_path=recompiled.with_suffix('.map');map_bytes=map_path.read_bytes()
    functions={int(a,16):(n,o.strip()) for n,a,o in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',map_bytes.decode('latin1'),re.M)}
    nt,base,size,table=pe(data);flat=bytearray(max(size,0x2940d0));sections=[]
    for i in range(struct.unpack_from('<H',data,nt+6)[0]):
        virtual,rva,n,offset=struct.unpack_from('<IIII',data,table+40*i+8);flags=struct.unpack_from('<I',data,table+40*i+36)[0]
        flat[rva:rva+n]=data[offset:offset+n];sections.append((base+rva,max(virtual,n),flags))
    globals_={symbols[original]:0x50000000+original-0x400000 for original in (0x692dd0,0x692dd4,0x6940bc,0x6940c0,0x6940c4,0x6940c8)}
    exit_address=symbols.get(0x5784e0)
    if exit_address is None:
        candidates=[a for a,(n,o) in functions.items() if n=='_exit' and o.startswith('LIBC:')];assert len(candidates)==1;exit_address=candidates[0]
    initial={symbols[a]:name for name,a in spec};pending=list(initial);seen=set();closures=[];patched=set();relocations=[];tables=[]
    cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
    def patch(site,value,kind,width=4):
        if site in patched:return
        old=struct.unpack_from('<I',flat,site)[0];relocations.append(dict(site=site,original=old,mapped=value,kind=kind,width=width));struct.pack_into('<I',flat,site,value);patched.add(site)
    while pending:
        start=pending.pop(0)
        if start in seen:continue
        seen.add(start);end=min(a for a in functions if a>start);name=initial.get(start,functions[start][0].lstrip('_@').split('@')[0]);todo=[start];instructions={};callback_loads={};group_callback_loads={}
        while todo:
            at=todo.pop()
            while at not in instructions:
                assert start<=at<end,(name,hex(at),'outside linked function')
                ins=next(cs.disasm(raw(data,at,15),at,count=1));instructions[at]=ins.bytes.hex()
                if ins.mnemonic=='mov' and ins.operands[0].type==CS_OP_REG and ins.operands[1].type==CS_OP_MEM and ins.operands[1].mem.disp==symbols[0x6940c0]:callback_loads[ins.operands[0].reg]=ins.address
                if name=='FUN_004bd720' and ins.mnemonic=='mov' and ins.operands[0].type==CS_OP_REG and ins.operands[1].type==CS_OP_MEM and ins.operands[1].mem.disp==0x198:group_callback_loads[ins.operands[0].reg]=ins.address
                is_table=ins.mnemonic=='jmp' and ins.operands[0].type==CS_OP_MEM and bool(ins.operands[0].mem.index)
                for operand in ins.operands:
                    if operand.type==CS_OP_MEM and base<=operand.mem.disp<base+size:
                        assert not operand.mem.base and ins.disp_size==4,(name,ins.op_str)
                        value=operand.mem.disp;site=ins.address-base+ins.disp_offset
                        if value in globals_:patch(site,globals_[value],'canonical scratch global',operand.size)
                        elif is_table:
                            assert name=='AllocateObjectWorkspace' and operand.mem.scale==4,(name,ins.op_str)
                            targets=[]
                            for index in range(4):
                                target=struct.unpack('<I',raw(data,value+4*index,4))[0];assert start<=target<end,(name,index,hex(target))
                                targets.append(target-start);todo.append(target);patch(value-base+4*index,target-base+0x50000000,'allocator table target')
                            tables.append(dict(function=name,table=value,index_domain=[0,1,2,3],target_offsets=targets,authority='Original and production primary-kind switch1..4; selector minus1 bounded to0..3 before indirect jump'))
                            patch(site,value-base+0x50000000,'allocator table base')
                        else:
                            section=next(s for s in sections if s[0]<=value<s[0]+s[1]);assert not(section[2]&(0x80000000|0x20)),(name,ins.op_str,'unreviewed writable/code operand')
                            bits=raw(data,value,operand.size).hex();patch(site,value-base+0x50000000,'readonly numeric operand',operand.size);relocations[-1]['bits']=bits
                if ins.mnemonic=='call' or ins.mnemonic.startswith('j'):
                    operand=ins.operands[0]
                    if operand.type==CS_OP_IMM:
                        target=operand.imm
                        if start<=target<end:todo.append(target)
                        elif target!=exit_address:
                            assert target in functions,(name,ins.op_str)
                            if target not in initial:
                                callee,owner=functions[target];short=callee.lstrip('_@')
                                allowed=(owner=='49-physics.c.obj' and short.startswith(('Application','PhysicsJoint','PhysicsScalarNaN','PhysicsSubtractScalar','PhysicsCopyWord','Force','Partner','RQ','Group','FrictionProductScalars','MakeJointVelocitiesVector','CreateJointForce','CreateJointTorque','DRMatrix33Inverse','SetUpForce','SetUpTorqueHingePartners','SetUpHingePartners','MakeMatrix','MatrixProcess','ApplySingleForce','RedoQuickHingeVelocities','GetPhysicsScratchPointer','IncreasePhysicsScratchPointer','PhysicsError')))
                                allowed=allowed or (owner=='34-finteray.c.obj' and short.startswith(('BoundsOverlapTest','Finteray')))
                                allowed=allowed or (owner.startswith(('brender:','libbrender:')) and short.startswith(('BrMatrix34ApplyV','BrMatrix34TApplyV','BrMatrix34LPInverse','BrMatrix34Mul','BrVector3SetFloat')))
                                assert allowed,(name,functions[target],'not an approved actual lower body')
                            pending.append(target)
                    elif ins.mnemonic=='call':
                        if name=='FUN_004bd720':
                            assert operand.type==CS_OP_REG and operand.reg in group_callback_loads and 0<ins.address-group_callback_loads[operand.reg]<=32,(name,ins.op_str,'unproven group callback reload')
                        else:
                            assert name in ('GetPhysicsScratchPointer','IncreasePhysicsScratchPointer','PhysicsError','AllocateObjectWorkspace') and operand.type==CS_OP_REG and operand.reg in callback_loads and 0<ins.address-callback_loads[operand.reg]<=64,(name,ins.op_str,'unproven error callback reload')
                    else:assert is_table,(name,ins.op_str)
                if ins.mnemonic.startswith('ret') or ins.mnemonic in ('jmp','int3','int'):break
                at+=ins.size
        closures.append(dict(name=name,address=start,extent=end-start,instruction_count=len(instructions),instruction_sha256=sha(json.dumps(sorted(instructions.items())).encode())))
    assert not any(row['address']-base<=0x3f00<row['address']-base+row['extent'] for row in closures),'callback trampoline overlaps matching closure'
    # Every actual HIGHLOW site reachable in an instruction must be accounted
    # for as a proven global, numeric operand, or allocator table reference.
    rel_rva,rel_size=struct.unpack_from('<II',data,nt+24+96+5*8);cursor=rel_rva
    while cursor<rel_rva+rel_size:
        page,n=struct.unpack('<II',raw(data,base+cursor,8));assert n>=8
        for item in struct.unpack('<'+'H'*((n-8)//2),raw(data,base+cursor+8,n-8)):
            if item>>12==3:
                site=page+(item&4095)
                if any(r['address']-base<=site<r['address']-base+r['extent'] for r in closures):assert site in patched,('Unaccounted linked relocation',hex(site))
        cursor+=n
    (output/'matching-image.bin').write_bytes(flat)
    old_size=str(struct.unpack_from('<I',(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes(),struct.unpack_from('<I',(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes(),60)[0]+80)[0])
    matching_driver=driver.replace(old_size,str(len(flat))).replace('0xbc8e0,0xbd720,0xbd7f0,0xbd830,0xbd9b0',','.join(hex(symbols[a]-base) for _,a in spec)).replace('0x1784e0',hex(exit_address-base))
    (output/'matching.c').write_text(prefix+'\n#define PHYSICAL_ORIGINAL\n'+matching_driver)
    gcc=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe';inc=[ROOT/p for p in ('src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/brender/include','src/carma2/s3/include','src/carma2/platform')]
    command=[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-DREC2_MATCHING',*['-I'+str(p) for p in inc],str(output/'matching.c'),str(output/'caller.obj'),'-o',str(output/'matching.exe')]
    result=subprocess.run(command,cwd=output,capture_output=True,text=True);(output/'matching-build.txt').write_text(result.stdout+result.stderr);assert result.returncode==0,result.stdout+result.stderr
    return dict(recompiled_sha256=sha(data),symbols_sha256=sha(symbols_path.read_bytes()),map_sha256=sha(map_bytes),graph=closures,globals=[dict(actual=a,transport=t) for a,t in globals_.items()],allocator_tables=tables,relocations=relocations,crt_exit=exit_address,command=command)

p = argparse.ArgumentParser(description=__doc__)
p.add_argument("--output", type=Path, default=ROOT/"build-audit/physics-force-group-tests")
p.add_argument("--candidate", type=Path)
p.add_argument("--recompiled", type=Path)
p.add_argument("--symbols", type=Path)
p.add_argument("--build-only", action="store_true")
p.add_argument("--case-limit", type=int)
p.add_argument("--max-failures", type=int, default=20)
args = p.parse_args()
assert bool(args.recompiled)==bool(args.symbols),"Supply both immutable PE and symbols"
OUT = args.output.resolve(); OUT.mkdir(parents=True, exist_ok=True)
production = ROOT / "src/carma2/newcommon/49-physics.c"
candidate = args.candidate.resolve() if args.candidate else production
paths = [candidate, Path(__file__), production, production.with_suffix(".h"),
    ROOT / "src/carma2/newcommon/34-finteray.c", ROOT / "src/carma2/newcommon/34-finteray.h",
    ROOT / "src/brender/core/math/matrix34.c", ROOT / "src/brender/core/math/vector.c",
    ROOT / "src/carma2/include/rec2_types.h", ROOT / "src/carma2/hooks/c2_hooks.h",
    ROOT / "src/brender/include/brender/brender.h", ROOT / "src/brender/include/brender/br_types.h",
    ROOT / "src/brender/core/math/matrix34.h",
    ROOT / "scripts/verify_physics_joint_application_reconstruction.py",
    ROOT / "scripts/verify_phil_registry_reconstruction.py",
    ROOT / "scripts/verify_opponent_navigation_reconstruction.py"]
if args.recompiled:paths.extend([args.recompiled,args.recompiled.with_suffix('.map'),args.symbols])
before = {str(q): sha(q.read_bytes()) for q in paths}
(OUT / "verifier-snapshot.py").write_bytes(Path(__file__).read_bytes())
(OUT / "candidate-snapshot.c").write_bytes(candidate.read_bytes())
src = production.read_text()
mat = (ROOT / "src/brender/core/math/matrix34.c").read_text()
vec = (ROOT / "src/brender/core/math/vector.c").read_text()
foundation = ["PhysicsCopyWord", "PhysicsSubtractScalar", "PhysicsJointScalar", "PhysicsJointSum", "PhysicsJointProduct", "PhysicsJointProductMemory", "PhysicsJointAddScalar", "PhysicsJointAddVelocity", "PhysicsJointVelocityA", "PhysicsJointVelocityB", "MakeJointVelocitiesVector", "CreateJointForce", "CreateJointTorque", "PhysicsScalarNaN"]
applications = ["ApplicationScaleAdd", "ApplicationWorldAdd", "ApplicationDirect", "ApplicationHinged", "ApplySingleForce", "RedoQuickHingeVelocities", "ApplicationJoint", "ApplicationInlineTorque", "ApplicationSecondary", "AllocateObjectWorkspace"]
rq = ["RQScalar", "RQNaN", "RQAccumulate", "RQSubtractProduct", "RQDivideScalars", "RQDcmpV", "MatrixProcess"]
body = "\n".join(extract(src, n) for n in foundation)
body += "\n" + "\n".join(extract(mat, n) for n in ['Matrix34ApplyProductLoaded','Matrix34ApplyProductMemory','Matrix34ApplyAdd',"BrMatrix34ApplyV", "BrMatrix34TApplyV", "BrMatrix34LPInverse", "BrMatrix34Mul"])
body += "\n" + extract(vec, "BrVector3SetFloat")
body += "\n" + "\n".join(extract(src, n) for n in ["DRMatrix33Inverse", "SetUpForce"])
body += "\n" + workspace_block(src)
body += "\n#define exit ControlledExit\n" + "\n".join(extract(src, n) for n in ["GetPhysicsScratchPointer", "IncreasePhysicsScratchPointer", "PhysicsError"]) + "\n#undef exit\n"
body += "\n" + "\n".join(extract(src, n) for n in applications + rq)
body += "\n" + extract((ROOT / "src/carma2/newcommon/34-finteray.c").read_text(), "BoundsOverlapTest__finteray")
body += "\n" + extract(src, "FrictionProductScalars")
if args.candidate:
    group_body=candidate.read_text()
else:
    group_names=["GroupScalar","GroupPointer","GroupReducedSlot","GroupTemporaryHead","FUN_004bc8e0","FUN_004bd720","FUN_004bd7f0","FUN_004bd830","FUN_004bd9b0"]
    declarations=src[src.index("typedef struct tForce_group_container {"):src.index("static br_scalar C2_HOOK_CDECL GroupScalar(")]
    pieces=[]
    for name in group_names:
        if name=="GroupTemporaryHead":pieces.append("#if defined(__GNUC__)\n__attribute__((noinline))\n#endif")
        pieces.append(extract(src,name))
    group_body=declarations+'\n'+'\n'.join(pieces)
body += "\n" + group_body

spec = [("FUN_004bc8e0", 0x4bc8e0), ("FUN_004bd720", 0x4bd720), ("FUN_004bd7f0", 0x4bd7f0), ("FUN_004bd830", 0x4bd830), ("FUN_004bd9b0", 0x4bd9b0)]
catalog={a:dict(name=n,body_bytes=z)for n,a,z in [
 ("FUN_004bc8e0",0x4bc8e0,292),("FUN_004bd720",0x4bd720,203),("FUN_004bd7f0",0x4bd7f0,51),("FUN_004bd830",0x4bd830,374),("FUN_004bd9b0",0x4bd9b0,42),
 ("RedoQuickHingeVelocities",0x4b8cf0,287),("MakeMatrix",0x4bf490,3922),("MatrixProcess",0x4c5b50,13),("BoundsOverlapTest__finteray",0x425ba0,250),
 ("CreateJointForce",0x4b8c20,201),("CreateJointTorque",0x4b8e10,198),("DRMatrix33Inverse",0x4be350,352),("MakeJointVelocitiesVector",0x4b77b0,425),
 ("BrMatrix34ApplyV",0x533520,107),("ApplySingleForce",0x4b7960,2534),("SetUpTorqueHingePartners",0x4be550,1943),("SetUpForce",0x4be4b0,159),
 ("SetUpHingePartners",0x4becf0,1942),("RQDcmpV",0x4c5830,792),("BrMatrix34TApplyV",0x533690,107),("BrVector3SetFloat",0x534590,25)]}
original = (ROOT / "build-reference/originals/CARMA2_HW.EXE").read_bytes(); assert sha(original) == HASH
nt, base, size, table = pe(original); image = bytearray(size); sections = []
for i in range(struct.unpack_from("<H", original, nt + 6)[0]):
    virtual, rva, n, offset = struct.unpack_from("<IIII", original, table + 40 * i + 8)
    flags = struct.unpack_from("<I", original, table + 40 * i + 36)[0]
    image[rva:rva+n] = original[offset:offset+n]; sections.append((base+rva, max(virtual,n),flags))
cs = Cs(CS_ARCH_X86, CS_MODE_32); cs.detail = True
pending = [a for _,a in spec]; seen = set(); closures = []; patches = []
globals_ = [0x692dd0,0x692dd4,0x6940bc,0x6940c0,0x6940c4,0x6940c8]
while pending:
    start = pending.pop(0)
    if start in seen: continue
    seen.add(start); n = catalog[start]["body_bytes"]; name = catalog[start]["name"]
    ins = list(cs.disasm(raw(original,start,n),start)); assert sum(x.size for x in ins) == n
    for x in ins:
        if x.mnemonic == "call" or x.mnemonic.startswith("j"):
            operand = x.operands[0]
            if operand.type == CS_OP_IMM:
                target = operand.imm
                if not start <= target < start+n and target != 0x5784e0:
                    assert target in catalog, (hex(start), x.op_str)
                    pending.append(target)
            else:
                assert x.mnemonic == "call" and start in [0x4bd720,0x4c61c0,0x4c5c70,0x4c5cb0], (hex(start),x.op_str)
        for operand in x.operands:
            if operand.type == CS_OP_MEM and base <= operand.mem.disp < base+size:
                at = operand.mem.disp
                assert not operand.mem.base and not operand.mem.index and x.disp_size == 4, (hex(start),x.op_str)
                if at not in globals_:
                    section = next(s for s in sections if s[0] <= at < s[0]+s[1])
                    assert not section[2] & 0x80000020, (hex(start), x.op_str)
                struct.pack_into("<I",image,x.address-base+x.disp_offset,at-base+0x50000000)
                patches.append(dict(site=x.address+x.disp_offset,address=at,width=operand.size))
    closures.append(dict(name=name,address=start,bytes=n,sha256=sha(raw(original,start,n))))
(OUT / "physical-image.bin").write_bytes(image)

prefix = r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "49-physics.h"
#include "34-finteray.h"
#define M(x,y) mat->m[x][y]
#define A(x,y) A->m[x][y]
#define B(x,y) B->m[x][y]
#define C(x,y) C->m[x][y]
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[6],builder_stack_count,builder_result,builder_poison,builder_cdecl;
extern unsigned __cdecl BuilderInvoke(void);
static unsigned char* code;
static unsigned char* arena;
static unsigned settings[16];
#define gPhysics_bounds_max_x (*(br_scalar*)(code+0x292dd0))
#define gPhysics_bounds_min_x (*(br_scalar*)(code+0x292dd4))
#define gPhysics_scratch_ptr (*(void**)(code+0x2940bc))
#define gPhysics_error_cb (*(tPhysicsError_cbfn**)(code+0x2940c0))
#define gPhysics_other_buffer (*(void**)(code+0x2940c4))
#define gPhysics_other_buffer_capacity (*(int*)(code+0x2940c8))
static void C2_NORETURN C2_HOOK_CDECL ControlledExit(int value){fprintf(stderr,"unexpected exit %d\n",value);exit(90);}
static void C2_HOOK_CDECL GroupCallbackAction(tPhysics_object* object){unsigned event[2];unsigned count;memcpy(&count,arena+0xf000,4);event[0]=++count;event[1]=(unsigned)object;memcpy(arena+0xf000,&count,4);fwrite(event,8,1,stdout);fwrite(arena,65536,1,stdout);
if(object==(tPhysics_object*)(arena+0x1000)){
if(settings[5]&1)object->child=(tPhysics_object*)(arena+0x4000);
if(settings[5]&2)object->next=(tPhysics_object*)(arena+0x2000);
if(settings[5]&4)memset((unsigned char*)object+0x198,0,4);
if(settings[5]&8)object->omega.v[0]=-17.0f;
if(settings[5]&16)gPhysics_bounds_max_x=-3.0f;
}}
'''
driver = r'''
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned h[16],result,mode,relative;void*function;
if(argc!=3)return 2;code=VirtualAlloc((void*)0x50000000,IMAGE_SIZE,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);arena=VirtualAlloc((void*)0x20000000,65536,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(code!=(void*)0x50000000||arena!=(void*)0x20000000)return 3;
f=fopen(argv[2],"rb");if(!f)return 4;if(fread(code,IMAGE_SIZE,1,f)!=1)return 5;fclose(f);f=fopen(argv[1],"rb");if(!f)return 6;if(fread(h,sizeof(h),1,f)!=1||fread(arena,65536,1,f)!=1)return 7;fclose(f);
memcpy(code+0x292dd0,&h[6],4);memcpy(code+0x292dd4,&h[7],4);gPhysics_scratch_ptr=arena+0xa000;gPhysics_other_buffer=arena+0xa000;gPhysics_other_buffer_capacity=0x5000;gPhysics_error_cb=NULL;
memcpy(settings,h,sizeof(h));
#if defined(REC2_MATCHING) || defined(PHYSICAL_ORIGINAL)
code[0x3f00]=0x51;code[0x3f01]=0xe8;relative=(unsigned)GroupCallbackAction-((unsigned)code+0x3f06);memcpy(code+0x3f02,&relative,4);memcpy(code+0x3f06,"\x83\xc4\x04\xc3",4);
#else
code[0x3f00]=0xe9;relative=(unsigned)GroupCallbackAction-((unsigned)code+0x3f05);memcpy(code+0x3f01,&relative,4);
#endif
code[0x1784e0]=0xe9;relative=(unsigned)ControlledExit-((unsigned)code+0x1784e0+5);memcpy(code+0x1784e0+1,&relative,4);_controlfp(_PC_53,_MCW_PC);_setmode(_fileno(stdout),_O_BINARY);mode=h[0];
#ifdef PHYSICAL_ORIGINAL
{unsigned entries[5]={0xbc8e0,0xbd720,0xbd7f0,0xbd830,0xbd9b0};function=code+entries[mode];}
#else
function=mode==0?(void*)FUN_004bc8e0:mode==1?(void*)FUN_004bd720:mode==2?(void*)FUN_004bd7f0:mode==3?(void*)FUN_004bd830:(void*)FUN_004bd9b0;
#endif
builder_address=(unsigned)function;builder_mode=0;builder_poison=0xcccccccc;builder_x=h[1]?(unsigned)(arena+h[1]):0;builder_y=mode==3?(unsigned)(arena+h[2]):mode==4?h[3]:0xcccccccc;
#if defined(REC2_MATCHING) || defined(PHYSICAL_ORIGINAL)
builder_cdecl=0;builder_stack_count=mode==0||mode==3?1:0;builder_stack[0]=mode==0?h[3]:(unsigned)(arena+h[4]);
#else
builder_cdecl=1;builder_stack_count=mode==0||mode==3?3:mode==4?2:1;builder_stack[0]=builder_x;builder_stack[1]=builder_y;builder_stack[2]=mode==0?h[3]:(unsigned)(arena+h[4]);
#endif
if(BuilderInvoke())return 14;result=mode==0||mode==4?0:builder_result;fwrite(&result,4,1,stdout);fwrite(code+0x292dd0,8,1,stdout);fwrite(arena,65536,1,stdout);return 0;}
'''.replace("IMAGE_SIZE",str(size))
tree = ast.parse((ROOT / "scripts/verify_opponent_navigation_reconstruction.py").read_text())
caller = next(n.value.value for n in ast.walk(tree) if isinstance(n,ast.Assign) and isinstance(n.value,ast.Constant) and any(isinstance(t,ast.Name) and t.id == "caller" for t in n.targets))
caller = caller.replace("builder_stack[5]","builder_stack[6]").replace("builder_poison;","builder_poison,builder_cdecl;").replace(" mov builder_result,eax\n mov ecx,esp"," mov builder_result,eax\n cmp builder_cdecl,0\n je stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n stackready:\n mov ecx,esp")
registry.COMMON = ""
commands = registry.compile_sources(OUT,prefix+body+driver,prefix+"\n#define PHYSICAL_ORIGINAL\n"+driver,caller)
linked_meta=None
if args.recompiled:
    linked_meta=compile_group_linked(OUT,args.recompiled,args.symbols,spec,prefix,driver)
    commands['matching']=linked_meta['command']
assert before == {str(q):sha(q.read_bytes()) for q in paths}
if args.build_only:raise SystemExit(0)

HEAP = 0x20000000
cases = []
def word(value): return struct.unpack("<I",struct.pack("<f",value))[0]
def scene(mode,label,patches=(),factor=1.0):
    data=bytearray(65536)
    for offset in [0x1000,0x2000,0x3000,0x4000]:
        struct.pack_into("<f",data,offset+4,2.5)
        for i in range(3):
            struct.pack_into("<f",data,offset+8+4*i,1.0+i)
            struct.pack_into("<f",data,offset+0x74+4*i,float(i+1))
            struct.pack_into("<f",data,offset+0x8c+16*i,1.0)
            struct.pack_into("<f",data,offset+0x10c+4*i,-2.0)
            struct.pack_into("<f",data,offset+0x118+4*i,2.0)
        struct.pack_into("<f",data,offset+0x21c,1.0)
    header=[mode,0x1000,0x1000,word(factor),0x8000,0,word(5.0),word(-5.0)]+[0]*8
    for offset,bits in patches:struct.pack_into("<I",data,offset,bits)
    cases.append(dict(label=label,header=header,data=data))
for mode in range(5):scene(mode,"single-%d"%mode)
for factor in [0.0,-0.0,0.001,0.1,0.33333334,1.001,2.0,100.0,-2.0]:scene(0,"scale-%r"%factor,factor=factor)
for speed in [0.0,1.0,30.0,39.25,40.0,100.0,1.e10,-1.e10]:
    scene(1,"cap-%r"%speed,[(0x1074,word(speed)),(0x1078,0),(0x107c,0)])
    scene(2,"treecap-%r"%speed,[(0x1074,word(speed)),(0x1220,HEAP+0x2000),(0x1224,HEAP+0x3000),(0x3274,word(speed*2))])
for state in [0,1,2,0xffffffff,0x80000000]:
    scene(4,"state-%x"%state,[(0x1224,HEAP+0x2000),(0x2220,HEAP+0x3000),(0x3224,HEAP+0x4000)])
    cases[-1]["header"][3]=state
for flag in [0,8]:
    scene(3,"list-%x"%flag,[(0x119c,flag),(0x1220,HEAP+0x2000),(0x122c,HEAP+0x3000),(0x2224,HEAP+0x4000),(0x4228,HEAP+0x2000)])
for seed in range(24):
    rng=random.Random(seed+904)
    case_patches=[(0x1000+offset,word(rng.uniform(-1.e4,1.e4))) for offset in [8,12,16,0x74,0x78,0x7c,0x21c]]
    scene(seed%3,"random-%d"%seed,case_patches,factor=rng.uniform(-5,5))
specials = [0,0x80000000,1,0x80000001,0x007fffff,0x00800000,0x60000001,0x7f7fffff,0x7f800000,0xff800000,0x7f800001,0xff800002,0x7fc12345,0xffc54321]
for bits in specials:
    scene(0,"raw-factor-%x"%bits); cases[-1]["header"][3]=bits
    for offset in [8,12,16,0x21c]:
        scene(0,"raw-inertia-%x-%x"%(offset,bits),[(0x1000+offset,bits)],factor=1.375)
    for offset in [0x74,0x78,0x7c]:
        scene(1,"raw-omega-%x-%x"%(offset,bits),[(0x1000+offset,bits)])
    for offset in [0x10c,0x118,0xb0]:
        scene(3,"raw-cutoff-%x-%x"%(offset,bits),[(0x1000+offset,bits),(0x1220,HEAP+0x2000),(0x122c,HEAP+0x3000)])
    for offset in [0x210c,0x2118,0x20b0,0x30b0]:
        scene(3,"raw-list-%x-%x"%(offset,bits),[(offset,bits),(0x1220,HEAP+0x2000),(0x122c,HEAP+0x3000)])
for pair in [(0x7f800001,0xff800002),(0xff800002,0x7f800001),(0x7fc12345,0xff800002),(0xff800002,0x7fc12345),(0x7fc12345,0xffc54321),(0xffc54321,0x7fc12345)]:
    for offset in [8,12,16,0x21c]:
        scene(0,"paired-factor-%x-%x-%x"%(offset,*pair),[(0x1000+offset,pair[0])]); cases[-1]["header"][3]=pair[1]
for speed_bits in range(word(39.25)-8,word(39.25)+9):
    scene(1,"cap-boundary-%x"%speed_bits,[(0x1074,speed_bits),(0x1078,0),(0x107c,0)])
for mode in [1,2]:
    for mutation in [0,1,2,3,4,8,16,31]:
        scene(mode,"callback-%d-%d"%(mode,mutation),[(0x1074,word(100.0)),(0x1198,0x50003f00),(0x1220,HEAP+0x3000),(0x1224,HEAP+0x2000),(0x2074,word(100.0)),(0x3074,word(100.0)),(0x4074,word(100.0))])
        cases[-1]["header"][5]=mutation
for mode in [0,1]:
    for geometry in range(8):
        scene(mode,"partner-%d-%d"%(mode,geometry),[(0x1074,word(100.0)),(0x11e4,HEAP+0x6000),(0x21e4,HEAP+0x6080),(0x1230,HEAP+0x2000),(0x2230,HEAP+0x1000),(0x1188,HEAP+0x5000)],factor=1.375)
        b=cases[-1]["data"];rng=random.Random(geometry+827)
        struct.pack_into("<I",b,0x5000,4)
        struct.pack_into("<3f",b,0x5008,.2,.3,.4)
        struct.pack_into("<3f",b,0x5014,-.2,.1,.25)
        struct.pack_into("<12f",b,0x5020,1,0,0,0,1,0,0,0,1,.125,.25,.375)
        for workspace,partner in [(0x6000,0x2000),(0x6080,0x1000)]:
            struct.pack_into("<II",b,workspace,1,HEAP+partner)
            struct.pack_into("<9f",b,workspace+8,1,0,0,0,1,0,0,0,1)
            struct.pack_into("<12f",b,workspace+0x38,1,0,0,0,1,0,0,0,1,0,0,0)
        if geometry:
            for at in [0x1008,0x2008,0x1074,0x2074,0x1014,0x2014,0x11a8,0x21a8]:
                struct.pack_into("<3f",b,at,*[rng.uniform(.2,3) for _ in range(3)])
            if mode==1:struct.pack_into("<f",b,0x1074,100)
for mode in [0,1]:
    for owner in [0x1000,0x2000,0x3000]:
        for count in [0,1,2,3,8,40]:
            scene(mode,"container-%d-%x-%d"%(mode,owner,count),[(0x1074,word(100.0)),(0x1218 if owner==0x1000 else owner+0x218,HEAP+0x7000),(0x1228,0 if owner==0x1000 else HEAP+0x2000),(0x2228,HEAP+0x3000 if owner==0x3000 else 0)],factor=1.375)
            b=cases[-1]["data"];rng=random.Random(count+owner)
            struct.pack_into("<II",b,0x7000,count,HEAP+0x6000)
            struct.pack_into("<II",b,0x7014,HEAP+0xb000,HEAP+0x8000)
            for i in range(count):
                at=0x6000+96*i
                struct.pack_into("<II",b,at+4,HEAP+0x1000,HEAP+0x2000 if i%2 else 0)
                struct.pack_into("<I",b,at+0x5c,[10,11,14][i%3])
                for off in [0x14,0x20,0x2c,0x38]:struct.pack_into("<3f",b,at+off,*[rng.uniform(-2,2) for _ in range(3)])
scene(2,"empty-cap-pass");cases[-1]["header"][1]=0
for state in [0,HEAP+0x4000]:
    scene(4,"pending-next-alias-%x"%state,[(0x1224,HEAP+0xed0)])
    cases[-1]["header"][3]=state
for flags in [0,8,0xffffffff]:
    scene(3,"nonroot-self-%x"%flags,[(0x119c,flags),(0x1228,HEAP+0x2000),(0x1220,HEAP+0x3000)])
for slot in [0x8234,0x2234,0x3234]:
    scene(3,"reduced-output-alias-%x"%slot,[(0x1220,HEAP+0x2000),(0x122c,HEAP+0x3000)])
    cases[-1]["header"][4]=slot
for at in [0x10c,0x118]:
    for bits in specials:
        scene(3,"raw-global-%x-%x"%(at,bits),[(0x1220,HEAP+0x2000),(0x122c,HEAP+0x3000)])
        cases[-1]["header"][6 if at==0x10c else 7]=bits
for seed in range(160):
    rng=random.Random(98234+seed)
    scale=2.0**rng.randint(-60,60)
    case_patches=[(0x1000+offset,word(rng.uniform(-2,2)*scale)) for offset in [8,12,16,0x74,0x78,0x7c,0x21c]]
    scene(seed%3,"exponent-%d"%seed,case_patches,factor=rng.uniform(-2,2)*2.0**rng.randint(-60,60))
for x_bits in range(word(39.25)-3,word(39.25)+4):
    for yz in [(0,0),(.0001,.0002),(.1,.2),(2,3)]:
        scene(1,"vector-boundary-%x-%r"%(x_bits,yz),[(0x1074,x_bits),(0x1078,word(yz[0])),(0x107c,word(yz[1]))])
partner_base=[dict(header=c["header"].copy(),data=bytearray(c["data"]),label=c["label"]) for c in cases if c["label"]=="partner-0-0"]
assert len(partner_base)==1
for mode in [0,1]:
    for bits in specials:
        for at in [0x1008,0x2008,0x121c,0x221c,0x2074]:
            c=dict(header=partner_base[0]["header"].copy(),data=bytearray(partner_base[0]["data"]),label="partner-raw-%d-%x-%x"%(mode,at,bits))
            c["header"][0]=mode;struct.pack_into("<I",c["data"],at,bits);cases.append(c)
    c=dict(header=partner_base[0]["header"].copy(),data=bytearray(partner_base[0]["data"]),label="partner-self-%d"%mode)
    c["header"][0]=mode;struct.pack_into("<I",c["data"],0x6004,HEAP+0x1000);cases.append(c)
container_base=next(c for c in cases if c["label"]=="container-0-1000-1")
for target in [0x7000,0x1008,0x21a8,0x6014,0x602c,0xb000]:
    c=dict(header=container_base["header"].copy(),data=bytearray(container_base["data"]),label="container-output-alias-%x"%target)
    struct.pack_into("<I",c["data"],0x7018,HEAP+target)
    if target==0x7000:
        for at in [0x6014,0x602c]:struct.pack_into("<3f",c["data"],at,0,0,0)
    cases.append(c)
for bits in specials:
    for at in [0x1008,0x2008,0x602c,0x6014,0x121c]:
        c=dict(header=container_base["header"].copy(),data=bytearray(container_base["data"]),label="container-raw-%x-%x"%(at,bits))
        struct.pack_into("<I",c["data"],at,bits);cases.append(c)
for pair in [(0x7f800001,0xff800002),(0xff800002,0x7f800001),(0x7fc12345,0xff800002),(0xff800002,0x7fc12345),(0x7fc12345,0xffc54321),(0xffc54321,0x7fc12345)]:
    for at in [0x2008,0x200c,0x2010,0x221c]:
        c=dict(header=partner_base[0]["header"].copy(),data=bytearray(partner_base[0]["data"]),label="partner-factor-pair-%x-%x-%x"%(at,*pair))
        c["header"][3]=pair[1];struct.pack_into("<I",c["data"],at,pair[0]);cases.append(c)
    for slots in [(0x1008,0x602c),(0x1010,0x6034),(0x6014,0x602c),(0x1004,0x602c)]:
        c=dict(header=container_base["header"].copy(),data=bytearray(container_base["data"]),label="container-pair-%x-%x-%x-%x"%(*slots,*pair))
        for at,bits in zip(slots,pair):struct.pack_into("<I",c["data"],at,bits)
        cases.append(c)
selected=cases[:args.case_limit] if args.case_limit is not None else cases
results=[];failures=[]
for index,c in enumerate(selected):
    path=OUT/"input.bin";path.write_bytes(struct.pack("<16I",*c["header"])+c["data"])
    expected=subprocess.run([str(OUT/"physical.exe"),str(path),str(OUT/"physical-image.bin")],capture_output=True,timeout=15)
    assert expected.returncode==0,(index,c["label"],expected.returncode,expected.stderr)
    assert len(expected.stdout)>=65548 and (len(expected.stdout)-65548)%65544==0,(index,len(expected.stdout))
    for compiler in ["msvc5","msvc","mingw",*(["matching"]if args.recompiled else [])]:
        actual=subprocess.run([str(OUT/(compiler+".exe")),str(path),str(OUT/("matching-image.bin"if compiler=="matching"else "physical-image.bin"))],capture_output=True,timeout=15)
        equal=actual.returncode==0 and actual.stdout==expected.stdout
        results.append(dict(index=index,label=c["label"],compiler=compiler,passed=equal,original_output_sha256=sha(expected.stdout),output_sha256=sha(actual.stdout)))
        if not equal:
            offsets=[i for i,(a,b) in enumerate(zip(expected.stdout,actual.stdout)) if a!=b]
            failures.append(dict(index=index,label=c["label"],compiler=compiler,returncode=actual.returncode,stderr=actual.stderr.decode(errors="replace"),offsets=offsets[:30]))
            (OUT/("failure-%d-%s-original.bin"%(index,compiler))).write_bytes(expected.stdout)
            (OUT/("failure-%d-%s-native.bin"%(index,compiler))).write_bytes(actual.stdout)
            (OUT/("failure-%d-input.bin"%index)).write_bytes(path.read_bytes())
    if len(failures)>=args.max_failures:break
after={str(q):sha(q.read_bytes()) for q in paths}; assert before==after
manifest=dict(scenario_count=len(cases),selected_scenes=len(selected),executed_scenes=index+1,comparisons=len(results),failures=failures,results=results,source_before=before,source_after=after,whole_sources_unchanged=True,commands=commands,original_closure=closures,patches=patches,fixture_sha256=sha(b"".join(struct.pack("<16I",*c["header"])+c["data"] for c in cases)),matching=linked_meta,generated_sha256={q.name:sha(q.read_bytes())for q in OUT.iterdir()if q.suffix in (".c",".obj",".exe",".bin",".py") and not q.name.startswith(("failure-","input"))},candidate_sha256=sha(candidate.read_bytes()),verifier_sha256=sha(Path(__file__).read_bytes()),scope="Five complete force/group entries with full actual original21-entry and production lower graph at PC53. Raw65536-byte arena,8-byte bounds globals,full ordered callback snapshots and semantic EAX compare without masks/normalization. ESP/nonvolatile registers/x87TOP checked on every call. Neutral numeric names; higher4baec0 remains incomplete. Sticky flags/unmasked traps,nonx86 and invalid/unbounded graph faults are outside claim.")
(OUT/"verification.json").write_text(json.dumps(manifest,indent=2)+"\n")
print("%d scenes/%d comparisons/%d failures"%(index+1,len(results),len(failures)),flush=True)
raise SystemExit(bool(failures))
