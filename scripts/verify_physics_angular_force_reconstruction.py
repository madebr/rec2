"""Complete angular-force consumer, physical original and three C compilers.

Production-default complete extraction and optional hash-bound actual linked PE.
A candidate override remains a clearly labelled development proof.
Real BRender matrix rotation and complete force-group/QR lower graph execute.
"""
import argparse,ast,json,random,re,struct,subprocess,sys
from pathlib import Path
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'src/carma2').is_dir())
sys.path.insert(0,str(ROOT/'scripts'))
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM,CS_OP_REG
from verify_tinted_native_reconstruction import extract
from verify_opponent_route_reconstruction import HASH,sha,pe,raw
from verify_physics_joint_application_reconstruction import workspace_block
import verify_phil_registry_reconstruction as registry

def compile_angular_linked(output,recompiled,symbols_path,spec,prefix,driver):
    data=recompiled.read_bytes();mapping=json.loads(symbols_path.read_text())
    assert mapping['format']==1 and mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(data)
    symbols={r['orig']:r['recomp'] for r in mapping['symbols']}
    map_path=recompiled.with_suffix('.map');map_bytes=map_path.read_bytes()
    functions={int(a,16):(n,o.strip()) for n,a,o in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',map_bytes.decode('latin1'),re.M)}
    nt,base,size,table=pe(data);flat=bytearray(max(size,0x2940d0));sections=[]
    for i in range(struct.unpack_from('<H',data,nt+6)[0]):
        virtual,rva,n,offset=struct.unpack_from('<IIII',data,table+40*i+8);flags=struct.unpack_from('<I',data,table+40*i+36)[0]
        flat[rva:rva+n]=data[offset:offset+n];sections.append((base+rva,max(virtual,n),flags))
    all_map={n:int(a,16)for n,a in re.findall(r'^\s*000[1-3]:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})',map_bytes.decode('latin1'),re.M)}
    if 0x676920 not in symbols:
        assert '_gPhysics_empty_message'in all_map;symbols[0x676920]=all_map['_gPhysics_empty_message']
    global_widths={a:4 for a in(0x692dd0,0x692dd4,0x6940bc,0x6940c0,0x6940c4,0x6940c8)};global_widths[0x676920]=1
    globals_={symbols[a]:0x50000000+a-0x400000 for a in global_widths}
    initial_global_bytes={a:bytes(flat[symbols[a]-base:symbols[a]-base+n])for a,n in global_widths.items()}
    for a,n in global_widths.items():flat[a-0x400000:a-0x400000+n]=initial_global_bytes[a]
    assert initial_global_bytes[0x676920]==b'\0'
    exit_address=symbols.get(0x5784e0)
    if exit_address is None:
        candidates=[a for a,(n,o) in functions.items() if n=='_exit' and o.startswith('LIBC:')];assert len(candidates)==1;exit_address=candidates[0]
    ftol_candidates=[a for a,(n,o)in functions.items()if n=='__ftol'and o=='LIBC:ftol.obj'];assert len(ftol_candidates)==1
    ftol_address=ftol_candidates[0];symbols.setdefault(0x575b80,ftol_address)
    assert symbols[0x575b80]==ftol_address
    initial={symbols[a]:name for name,a in spec};pending=[symbols[0x4bc190]];seen=set();closures=[];patched=set();relocations=[];tables=[]
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
                            assert name in('AllocateObjectWorkspace','FUN_004bc190','AngularForceNextIndex')and operand.mem.scale==4,(name,ins.op_str)
                            domain=4;authority='Published allocator primary selector1..4'
                            if name!='AllocateObjectWorkspace':
                                preceding=[next(cs.disasm(bytes.fromhex(bits),address,count=1))for address,bits in sorted(instructions.items())if address<ins.address]
                                assert len(preceding)>=2
                                compare,guard=preceding[-2:]
                                assert compare.mnemonic=='cmp'and len(compare.operands)==2 and compare.operands[0].type==CS_OP_REG and compare.operands[0].reg==operand.mem.index and compare.operands[1].type==CS_OP_IMM
                                domain=compare.operands[1].imm+1;assert domain in(4,5)
                                assert guard.mnemonic=='ja'and guard.address+guard.size==ins.address and compare.address+compare.size==guard.address
                                authority='Exact immediately preceding CMP selector,%d / JA bounds guard'%compare.operands[1].imm
                            targets=[]
                            for index in range(domain):
                                target=struct.unpack('<I',raw(data,value+4*index,4))[0];assert start<=target<end,(name,index,hex(target))
                                targets.append(target-start);todo.append(target);patch(value-base+4*index,target-base+0x50000000,'allocator table target')
                            tables.append(dict(function=name,table=value,index_domain=list(range(domain)),target_offsets=targets,authority=authority,raw_sha256=sha(raw(data,value,4*domain))))
                            patch(site,value-base+0x50000000,'allocator table base')
                        else:
                            section=next(s for s in sections if s[0]<=value<s[0]+s[1]);assert not(section[2]&(0x80000000|0x20)),(name,ins.op_str,'unreviewed writable/code operand')
                            bits=raw(data,value,operand.size).hex();patch(site,value-base+0x50000000,'readonly numeric operand',operand.size);relocations[-1]['bits']=bits
                for operand in ins.operands:
                    if operand.type==CS_OP_IMM and ins.mnemonic in('mov','push')and base<=operand.imm<base+size:
                        value=operand.imm;assert value in globals_,(name,ins.op_str,'unapproved pointer immediate')
                        assert ins.imm_size==4;patch(ins.address-base+ins.imm_offset,globals_[value],'canonical global pointer',global_widths[next(a for a in global_widths if symbols[a]==value)])
                if ins.mnemonic=='call' or ins.mnemonic.startswith('j'):
                    operand=ins.operands[0]
                    if operand.type==CS_OP_IMM:
                        target=operand.imm
                        if start<=target<end:todo.append(target)
                        elif target!=exit_address:
                            assert target in functions,(name,ins.op_str)
                            if target not in initial:
                                callee,owner=functions[target];short=callee.lstrip('_@')
                                allowed=(owner=='49-physics.c.obj' and short.startswith(('Application','PhysicsJoint','PhysicsScalarNaN','PhysicsSubtractScalar','PhysicsCopyWord','Force','Partner','RQ','Group','Friction','AngularForce','MakeJointVelocitiesVector','CreateJointForce','CreateJointTorque','DRMatrix33Inverse','SetUpForce','SetUpTorqueHingePartners','SetUpHingePartners','MakeMatrix','MatrixProcess','ApplySingleForce','RedoQuickHingeVelocities','GetPhysicsScratchPointer','IncreasePhysicsScratchPointer','PhysicsError')))
                                allowed=allowed or (owner=='34-finteray.c.obj' and short.startswith(('BoundsOverlapTest','Finteray')))
                                allowed=allowed or (owner.startswith(('brender:','libbrender:')) and short.startswith(('BrMatrix34ApplyV','BrMatrix34TApplyV','BrMatrix34LPInverse','BrMatrix34Mul','BrVector3SetFloat','BrMatrix34Rotate','MatrixRotate')))
                                allowed=allowed or(owner=='LIBC:ftol.obj'and callee=='__ftol'and target==ftol_address)
                                assert allowed,(name,functions[target],'not an approved actual lower body')
                            pending.append(target)
                    elif ins.mnemonic=='call':
                        if name=='FUN_004bd720':
                            assert operand.type==CS_OP_REG and operand.reg in group_callback_loads and 0<ins.address-group_callback_loads[operand.reg]<=32,(name,ins.op_str,'unproven group callback reload')
                        else:
                            assert name in ('GetPhysicsScratchPointer','IncreasePhysicsScratchPointer','PhysicsError','AllocateObjectWorkspace','AngularForceNextIndex','FUN_004bc190') and operand.type==CS_OP_REG and operand.reg in callback_loads and 0<ins.address-callback_loads[operand.reg]<=64,(name,ins.op_str,'unproven error callback reload')
                    else:assert is_table,(name,ins.op_str)
                if ins.mnemonic.startswith('ret') or ins.mnemonic in ('jmp','int3','int'):break
                at+=ins.size
        closures.append(dict(name=name,address=start,extent=end-start,instruction_count=len(instructions),instruction_sha256=sha(json.dumps(sorted(instructions.items())).encode()),instructions={hex(a):b for a,b in sorted(instructions.items())}))
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
    for actual,transport in globals_.items():assert not any(row['address']-base<=transport-0x50000000<row['address']-base+row['extent']for row in closures),'canonical transport overlaps actual code'
    assert any(row['address']==ftol_address for row in closures),'actual __ftol was not in reachable graph'
    (output/'matching-image.bin').write_bytes(flat)
    old_size=str(struct.unpack_from('<I',(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes(),struct.unpack_from('<I',(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes(),60)[0]+80)[0])
    matching_driver=driver.replace(old_size,str(len(flat))).replace('function=code+0xbc190;', 'function=code+'+hex(symbols[0x4bc190]-base)+';').replace('0x1784e0',hex(exit_address-base))
    (output/'matching.c').write_text(prefix+'\n#define PHYSICAL_ORIGINAL\n'+matching_driver)
    gcc=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe';inc=[ROOT/p for p in ('src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/brender/include','src/carma2/s3/include','src/carma2/platform')]
    command=[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-DREC2_MATCHING',*['-I'+str(p) for p in inc],str(output/'matching.c'),str(output/'caller.obj'),'-o',str(output/'matching.exe')]
    result=subprocess.run(command,cwd=output,capture_output=True,text=True);(output/'matching-build.txt').write_text(result.stdout+result.stderr);assert result.returncode==0,result.stdout+result.stderr
    return dict(recompiled_sha256=sha(data),symbols_sha256=sha(symbols_path.read_bytes()),map_sha256=sha(map_bytes),graph=closures,globals=[dict(actual=a,transport=t,width=global_widths[t-0x50000000+0x400000],initial_bits=initial_global_bytes[t-0x50000000+0x400000].hex())for a,t in globals_.items()],allocator_tables=tables,relocations=relocations,crt_exit=exit_address,command=command)

p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--output',type=Path,required=True)
p.add_argument('--candidate',type=Path)
p.add_argument('--recompiled',type=Path)
p.add_argument('--symbols',type=Path)
p.add_argument('--limit',type=int)
p.add_argument('--case-index',type=int,action='append')
p.add_argument('--max-failures',type=int,default=10)
args=p.parse_args();OUT=args.output.resolve();OUT.mkdir(parents=True,exist_ok=True)
assert bool(args.recompiled)==bool(args.symbols),'Supply both immutable PE and symbols'
production=ROOT/'src/carma2/newcommon/49-physics.c';candidate=args.candidate.resolve()if args.candidate else production
paths=[production,production.with_suffix('.h'),candidate,Path(__file__),ROOT/'scripts/verify_physics_force_group_reconstruction.py',ROOT/'scripts/verify_tinted_native_reconstruction.py',ROOT/'scripts/verify_opponent_route_reconstruction.py',ROOT/'scripts/verify_physics_joint_application_reconstruction.py',ROOT/'scripts/verify_phil_registry_reconstruction.py',ROOT/'scripts/verify_opponent_navigation_reconstruction.py',ROOT/'src/carma2/newcommon/34-finteray.c',ROOT/'src/carma2/newcommon/34-finteray.h',ROOT/'src/brender/core/math/matrix34.c',ROOT/'src/brender/core/math/matrix34.h',ROOT/'src/brender/core/math/vector.c',ROOT/'src/carma2/include/rec2_types.h',ROOT/'src/carma2/hooks/c2_hooks.h',ROOT/'src/brender/include/brender/brender.h',ROOT/'src/brender/include/brender/br_types.h',ROOT/'src/brender/include/brender/br_defs.h',ROOT/'build-audit/physics-parent-dependencies-next/functions.jsonl']
paths+=list((ROOT/'src').rglob('*.h'))
if args.recompiled:paths.extend([args.recompiled,args.symbols,args.recompiled.with_suffix('.map')])
before={str(f):sha(f.read_bytes())for f in paths}
(OUT/'source-before.json').write_text(json.dumps(before,indent=2))
(OUT/'verifier-snapshot.py').write_bytes(Path(__file__).read_bytes())
(OUT/'candidate-snapshot.c').write_bytes(candidate.read_bytes())
src=production.read_text();mat=(ROOT/'src/brender/core/math/matrix34.c').read_text();vec=(ROOT/'src/brender/core/math/vector.c').read_text()
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
if False:
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

# Keep real private arithmetic helpers and the complete production Rotate body.
for name in ('FrictionStatus','FrictionGreater','FrictionLessOrUnordered'):
    part=extract(src,name)
    body+='\n'+('#if defined(_MSC_VER) && _MSC_VER < 1300\n'+part+'\n#endif\n'if name=='FrictionStatus'else part)
rotate=extract(mat,'BrMatrix34Rotate');start=mat.index('static double C2_HOOK_CDECL MatrixRotateScalar(')
body+='\n'+mat[start:mat.index(rotate)]+rotate+'\n#undef MR_PRODUCT\n#undef MR_SUM\n#undef MR_DIFFERENCE\n'
angular_capsule='\n'.join(extract(candidate.read_text(),name)for name in('AngularForceAngle','AngularForceNextIndex','FUN_004bc190'))
body+='\n'+angular_capsule
(OUT/'verified-capsules.c').write_text(body)

original=(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes();assert sha(original)==HASH
nt,base,size,table=pe(original);image=bytearray(size);sections=[]
for i in range(struct.unpack_from('<H',original,nt+6)[0]):
    virtual,rva,n,offset=struct.unpack_from('<IIII',original,table+40*i+8);flags=struct.unpack_from('<I',original,table+40*i+36)[0]
    image[rva:rva+n]=original[offset:offset+n];sections.append((base+rva,max(virtual,n),flags))
catalog={int(r['address'],16):r for r in map(json.loads,(ROOT/'build-audit/physics-parent-dependencies-next/functions.jsonl').read_text().splitlines())}
catalog[0x575b80]=dict(name='__ftol',body_bytes=39)
cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
pending=[0x4bc190];seen=set();closures=[];patches=[];tables=[]
table_domains={0x4bc88c:4,0x4bc89c:5,0x4bc8b0:4,0x4bc8c0:5}
mutable=[0x692dd0,0x692dd4,0x6940bc,0x6940c0,0x6940c4,0x6940c8,0x676920]
while pending:
    start=pending.pop(0)
    if start in seen:continue
    seen.add(start);row=catalog[start];n=row['body_bytes'];ins=list(cs.disasm(raw(original,start,n),start));assert sum(i.size for i in ins)==n
    for i in ins:
        is_table=i.mnemonic=='jmp'and i.operands[0].type==CS_OP_MEM
        if i.mnemonic=='call'or i.mnemonic.startswith('j'):
            op=i.operands[0]
            if op.type==CS_OP_IMM:
                target=op.imm
                if not start<=target<start+n and target!=0x5784e0:
                    assert target in catalog,(hex(start),i.op_str);pending.append(target)
            elif not is_table:assert i.mnemonic=='call'and start in(0x4c61c0,0x4c5c70,0x4c5cb0),(hex(start),i.op_str)
        for op in i.operands:
            at=None;site=None;width=op.size
            if op.type==CS_OP_MEM and base<=op.mem.disp<base+size:
                at=op.mem.disp;site=i.address-base+i.disp_offset;assert not op.mem.base and i.disp_size==4
                if op.mem.index:
                    assert is_table and start==0x4bc190 and at in table_domains and op.mem.scale==4,(hex(start),i.op_str)
                    targets=[]
                    for k in range(table_domains[at]):
                        target=struct.unpack('<I',raw(original,at+4*k,4))[0];assert start<=target<start+n
                        targets.append(target);struct.pack_into('<I',image,at-base+4*k,target-base+0x50000000)
                    tables.append(dict(address=at,entries=targets,sha256=sha(raw(original,at,4*len(targets)))))
            elif op.type==CS_OP_IMM and i.mnemonic in('mov','push')and base<=op.imm<base+size:
                at=op.imm;site=i.address-base+i.imm_offset;assert i.imm_size==4;width=1 if at==0x676920 else 4
            if at is not None:
                if at not in mutable and not is_table:
                    section=next(s for s in sections if s[0]<=at<s[0]+s[1]);assert not section[2]&0x80000020,(hex(start),i.op_str)
                struct.pack_into('<I',image,site,at-base+0x50000000)
                patches.append(dict(site=site,address=at,width=width,bits=raw(original,at,width).hex()if at not in mutable else None))
    closures.append(dict(address=start,name=row['name'],bytes=n,sha256=sha(raw(original,start,n))))
assert len(tables)==4
(OUT/'physical-image.bin').write_bytes(image)

prefix=r'''
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
#define gPhysics_bounds_max_x (*(br_scalar*)(code+0x292dd0))
#define gPhysics_bounds_min_x (*(br_scalar*)(code+0x292dd4))
#define gPhysics_scratch_ptr (*(void**)(code+0x2940bc))
#define gPhysics_error_cb (*(tPhysicsError_cbfn**)(code+0x2940c0))
#define gPhysics_other_buffer (*(void**)(code+0x2940c4))
#define gPhysics_other_buffer_capacity (*(int*)(code+0x2940c8))
#define gPhysics_empty_message (*(char*)(code+0x276920))
static void C2_HOOK_CDECL AngularSnapshot(unsigned kind,unsigned value){unsigned h[2];h[0]=kind;h[1]=value;fwrite(h,8,1,stdout);fwrite(code+0x292dd0,8,1,stdout);fwrite(code+0x2940bc,16,1,stdout);fwrite(arena,65536,1,stdout);}
static void C2_NORETURN C2_HOOK_CDECL ControlledExit(int value){AngularSnapshot(1,(unsigned)value);exit(0);}
static void C2_HOOK_FASTCALL ErrorCallback(tPhysicsError value,const char*message){unsigned h[3];h[0]=2;h[1]=(unsigned)value;h[2]=(unsigned)message;fwrite(h,12,1,stdout);fwrite(arena,65536,1,stdout);}
'''
driver=r'''
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned h[16],relative;void*function;
if(argc!=3)return 2;code=VirtualAlloc((void*)0x50000000,IMAGE_SIZE,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);arena=VirtualAlloc((void*)0x20000000,65536,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(code!=(void*)0x50000000||arena!=(void*)0x20000000)return 3;
f=fopen(argv[2],"rb");if(!f)return 4;if(fread(code,IMAGE_SIZE,1,f)!=1)return 5;fclose(f);f=fopen(argv[1],"rb");if(!f)return 6;if(fread(h,sizeof(h),1,f)!=1||fread(arena,65536,1,f)!=1)return 7;fclose(f);
gPhysics_scratch_ptr=arena+0xa000;gPhysics_other_buffer=arena+0xa000;gPhysics_other_buffer_capacity=0x5000;
code[0x3f00]=0xe9;relative=(unsigned)ErrorCallback-((unsigned)code+0x3f05);memcpy(code+0x3f01,&relative,4);gPhysics_error_cb=h[5]?(tPhysicsError_cbfn*)(code+0x3f00):NULL;
code[0x1784e0]=0xe9;relative=(unsigned)ControlledExit-((unsigned)code+0x1784e0+5);memcpy(code+0x1784e0+1,&relative,4);_controlfp(_PC_53,_MCW_PC);_setmode(_fileno(stdout),_O_BINARY);
#ifdef PHYSICAL_ORIGINAL
function=code+0xbc190;
#else
function=(void*)FUN_004bc190;
#endif
builder_address=(unsigned)function;builder_mode=1;builder_poison=0xcccccccc;builder_x=h[1]?(unsigned)(arena+h[1]):0;builder_y=h[2]?(unsigned)(arena+h[2]):0;
#if defined(REC2_MATCHING) || defined(PHYSICAL_ORIGINAL)
builder_cdecl=0;builder_stack_count=2;builder_stack[0]=h[3];builder_stack[1]=h[4];
#else
builder_cdecl=1;builder_stack_count=4;builder_stack[0]=builder_x;builder_stack[1]=builder_y;builder_stack[2]=h[3];builder_stack[3]=h[4];
#endif
if(BuilderInvoke())return 14;AngularSnapshot(0,0);return 0;}
'''.replace('IMAGE_SIZE',str(size))
tree=ast.parse((ROOT/'scripts/verify_opponent_navigation_reconstruction.py').read_text())
caller=next(n.value.value for n in ast.walk(tree)if isinstance(n,ast.Assign)and isinstance(n.value,ast.Constant)and any(isinstance(t,ast.Name)and t.id=='caller'for t in n.targets))
caller=caller.replace('builder_stack[5]','builder_stack[6]').replace('builder_poison;','builder_poison,builder_cdecl;').replace(' mov builder_result,eax\n mov ecx,esp',' mov builder_result,eax\n cmp builder_cdecl,0\n je stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n stackready:\n mov ecx,esp')
registry.COMMON='';commands=registry.compile_sources(OUT,prefix+body+driver,prefix+'\n#define PHYSICAL_ORIGINAL\n'+driver,caller)
linked=compile_angular_linked(OUT,args.recompiled,args.symbols,[(r['name'],r['address'])for r in closures],prefix,driver)if args.recompiled else None
assert before=={str(f):sha(f.read_bytes())for f in paths}

HEAP=0x20000000;cases=[]
def word(x):return struct.unpack('<I',struct.pack('<f',x))[0]
def scene(label,omega=(0,0,4),axis=(1,0,0),force=(2,1,0),primary=3,secondary=0,child_primary=3,child_secondary=0,header=(),patches=()):
    data=bytearray(65536);h=[0,0x6000,0x6800,0,0x12345678,1]+[0]*10
    def put(at,*values):struct.pack_into('<'+'I'*len(values),data,at,*[v&0xffffffff for v in values])
    def floats(at,*values):struct.pack_into('<'+'f'*len(values),data,at,*values)
    for object_,joint in ((0x1000,0x5000),(0x2000,0x5100),(0x3000,0x5200)):
        floats(object_+4,2,1,2,3);floats(object_+0x21c,1);put(object_+0x188,HEAP+joint)
        floats(object_+0x8c,1,0,0,0,1,0,0,0,1,0,0,0)
    put(0x1224,HEAP+0x2000);put(0x2228,HEAP+0x1000)
    put(0x5000,primary);put(0x5100,child_primary);put(0x5200,3)
    if primary==0:put(0x1188,0)
    if child_primary==0:put(0x2188,0)
    if secondary:put(0x118c,HEAP+0x5300);put(0x5300,secondary)
    if child_secondary:put(0x218c,HEAP+0x5400);put(0x5400,child_secondary)
    floats(0x5008,*axis);floats(0x1074,*omega)
    step=lambda a,b:({0:0,1:5,2:4,3:3,4:2}.get(a,3)+{0:0,1:5,2:4,3:3,5:2}.get(b,3))
    first=step(primary,secondary);second=step(child_primary,child_secondary);h[3]=first+second
    for k in range(max(h[3],3)+3):
        rec=0x7000+96*k;put(0x6000+4*k,HEAP+rec)
        put(rec+4,HEAP+(0x1000 if k<first else 0x2000));put(rec+8,0 if k<first else HEAP+0x1000)
        floats(rec+0x38,1 if k%3==0 else 0,1 if k%3==1 else 0,1 if k%3==2 else 0)
        floats(0x6800+4*k,force[k%3])
    # Child's translational records always use xyz directions, independent of
    # the number of constraints in the outer parent's joint.
    for k in range(3):floats(0x7000+96*(first+k)+0x38,1 if k==0 else 0,1 if k==1 else 0,1 if k==2 else 0)
    for at,val in patches:put(at,val)
    for at,val in header:h[at]=val&0xffffffff
    cases.append(dict(label=label,header=h,data=data))
for count in (0,-1,-2147483648):scene('nonpositive-%d'%count,header=[(1,0),(2,0),(3,count)])
for omega in ((0,0,4),(0,0,-4),(0,2,4),(1,-2,3),(0,0,0)):
    for force in ((2,1,0),(2,-1,0),(0,1,0),(0,0,0),(10,3,-2)):
        scene('direct-%r-%r'%(omega,force),omega=omega,force=force)
for a in range(1,5):
    for b in (0,1,2,3,5):
        for c in (1,2,3,4):scene('steps-%d-%d-%d'%(a,b,c),primary=a,secondary=b,child_primary=c)
for a,b in ((0,1),(0,2),(0,3),(0,5)):
    # A leaf needs no primary anchor. These scenes isolate the outer skips.
    scene('secondary-only-%d'%b,primary=a,secondary=b,patches=[(0x1224,0)])
for omega in ((1e-20,2e-20,3e-20),(1e20,-2e20,3e20),(3.5,2.7,-4.1)):
    for axis in ((1,2,3),(-1,0,0),(0,0,0),(1e-20,2e-20,3e-20),(1e20,2e20,-1e20)):
        scene('magnitudes-%r-%r'%(omega,axis),omega=omega,axis=axis)
rng=random.Random(0x4bc190)
for k in range(220):
    scene('finite-random-%d'%k,omega=tuple(rng.uniform(-70,70)for _ in range(3)),axis=tuple(rng.uniform(-5,5)for _ in range(3)),force=tuple(rng.uniform(-10,10)for _ in range(3)))
specials=[0,0x80000000,1,0x80000001,0x007fffff,0x00800000,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc12345,0xffc54321,0x7f800001,0xff800002]
for offset in (0x1004,0x1074,0x1078,0x107c,0x11c0,0x11c4,0x11c8,0x5008,0x500c,0x5010,0x5114,0x5118,0x511c,0x6800,0x6804,0x6808,0x680c,0x6810,0x6814,0x7158,0x715c,0x7160):
    for bits in specials:scene('raw-%x-%x'%(offset,bits),patches=[(offset,bits)])
for at in (0x5000,0x5100,0x5300,0x5400):
    for typ in (0,4,5,6,0xffffffff):
        if at in(0x5000,0x5100)and typ in(0,4):continue
        if at in(0x5300,0x5400)and typ==5:continue
        scene('invalid-kind-%x-%x'%(at,typ),secondary=3,child_secondary=3,patches=[(at,typ)])
for force in ((20,1,0),(1,.05,0),(2,.01,0)):
    scene('direct-scale-%r'%(force,),force=force)
for bits in range(word(1.)-5,word(1.)+6):
    scene('first-threshold-%x'%bits,force=(1,.05,0),patches=[(0x6800,bits)])
    scene('second-threshold-%x'%bits,force=(1,.05,0),patches=[(0x680c,bits)])
for bits in range(word(4.)-5,word(4.)+6):
    scene('old-norm-threshold-%x'%bits,force=(20,1,0),patches=[(0x11c8,bits)])
for bits in range(word(2.384185791015625e-7)-5,word(2.384185791015625e-7)+6):
    scene('normalization-threshold-%x'%bits,force=(2.384185791015625e-7,1.e-8,0),patches=[(0x1004,0),(0x6800,bits)])
for bits in range(word(4.)-10,word(4.)+11):
    scene('angle-threshold-%x'%bits,force=(20,1,0),patches=[(0x107c,bits)])
for bits in (word(1.e16),word(2.e16),word(3.e16),word(1.e17),word(1.e30),0x7f7fffff,0x7f800000,0xff800000):
    scene('angle-conversion-%x'%bits,force=(20,1,0),patches=[(0x107c,bits)])
for at in (0x1008,0x100c,0x1010,0x121c):
    for bits in specials:scene('scale-output-raw-%x-%x'%(at,bits),force=(20,1,0),patches=[(at,bits)])
for at in (0x1074,0x1008,0x11c0,0x5008):
    scene('impulse-alias-%x'%at,force=(20,1,0),header=[(2,at)])
    c=cases[-1];struct.pack_into('<6f',c['data'],at,20,1,0,20,1,0)
for count in (0,1,2,3,8,40):
    for owner in (0x1000,0x3000,0x4000):
        scene('nested-container-%x-%d'%(owner,count),force=(20,1,0),patches=[(owner+0x218,HEAP+0x8800)])
        b=cases[-1]['data'];r=random.Random(owner+count)
        if owner!=0x1000:
            struct.pack_into('<I',b,0x1228,HEAP+0x3000)
            if owner==0x4000:struct.pack_into('<I',b,0x3228,HEAP+0x4000)
        struct.pack_into('<II',b,0x8800,count,HEAP+0x9000)
        struct.pack_into('<II',b,0x8814,HEAP+0xe000,HEAP+0xb000)
        for i in range(count):
            at=0x9000+96*i;struct.pack_into('<II',b,at+4,HEAP+0x1000,HEAP+0x2000 if i%2 else 0)
            struct.pack_into('<I',b,at+0x5c,[10,11,14][i%3])
            for off in(0x14,0x20,0x2c,0x38):struct.pack_into('<3f',b,at+off,*[r.uniform(-2,2)for _ in range(3)])
for partner in (0x1000,0x2000,0x3000):
    for geometry in range(4):
        scene('nested-partner-%x-%d'%(partner,geometry),force=(20,1,0),primary=4,patches=[(0x11e4,HEAP+0x8500),(0x1230,HEAP+partner),(partner+0x1e4,HEAP+0x8580),(partner+0x230,HEAP+0x1000)])
        b=cases[-1]['data'];r=random.Random(partner+geometry)
        for workspace,other in ((0x8500,partner),(0x8580,0x1000)):
            struct.pack_into('<II',b,workspace,1,HEAP+other)
            struct.pack_into('<9f',b,workspace+8,1,0,0,0,1,0,0,0,1)
            struct.pack_into('<12f',b,workspace+0x38,1,0,0,0,1,0,0,0,1,0,0,0)
        struct.pack_into('<12f',b,0x5020,1,0,0,0,1,0,0,0,1,.125,.25,.375)
        for at in (0x1008,partner+8,0x1014,partner+0x14,0x11a8,partner+0x1a8):
            struct.pack_into('<3f',b,at,*[r.uniform(.2,3)for _ in range(3)])
for repeats in(1,2,3,5):
    scene('multiple-inner-%d'%repeats,force=(20,.001,0))
    c=cases[-1];c['header'][3]=3*(repeats+1);b=c['data']
    for i in range(3,3*(repeats+1)):
        at=0x7000+96*i;struct.pack_into('<I',b,0x6000+4*i,HEAP+at)
        struct.pack_into('<II',b,at+4,HEAP+0x2000,HEAP+0x1000)
        struct.pack_into('<3f',b,at+0x38,1 if i%3==0 else 0,1 if i%3==1 else 0,1 if i%3==2 else 0)
        struct.pack_into('<f',b,0x6800+4*i,(20,.001,0)[i%3])
selected=[(i,c)for i,c in enumerate(cases)if args.case_index is None or i in args.case_index]
if args.limit is not None:selected=selected[:args.limit]
results=[];failures=[];changed=0;terminal=0
(OUT/'fixtures.bin').write_bytes(b''.join(struct.pack('<16I',*c['header'])+c['data']for c in cases))
batch_outputs={label:bytearray()for label in('original','msvc5','msvc','mingw',*(['matching']if linked else []))}
for index,c in selected:
    path=OUT/'input.bin';path.write_bytes(struct.pack('<16I',*c['header'])+c['data'])
    expected=subprocess.run([str(OUT/'physical.exe'),str(path),str(OUT/'physical-image.bin')],capture_output=True,timeout=15)
    assert expected.returncode==0,(index,c['label'],expected.returncode,expected.stderr)
    assert len(expected.stdout)in(65568,131116),(index,c['label'],len(expected.stdout))
    tail=expected.stdout[-65568:];kind=struct.unpack_from('<I',tail)[0]
    terminal+=bool(kind);changed+=tail[-65536:]!=c['data']
    original_offset=len(batch_outputs['original']);batch_outputs['original'].extend(expected.stdout)
    for compiler in ('msvc5','msvc','mingw',*(['matching']if linked else [])):
        actual=subprocess.run([str(OUT/(compiler+'.exe')),str(path),str(OUT/('matching-image.bin'if compiler=='matching'else 'physical-image.bin'))],capture_output=True,timeout=15)
        output_offset=len(batch_outputs[compiler]);batch_outputs[compiler].extend(actual.stdout)
        equal=actual.returncode==0 and actual.stdout==expected.stdout
        results.append(dict(index=index,label=c['label'],compiler=compiler,passed=equal,fixture_sha256=sha(path.read_bytes()),fixture_length=path.stat().st_size,original_output_sha256=sha(expected.stdout),output_sha256=sha(actual.stdout),original_output_length=len(expected.stdout),output_length=len(actual.stdout),original_output_offset=original_offset,output_offset=output_offset,terminal=bool(kind)))
        if not equal:
            offsets=[j for j,(a,b)in enumerate(zip(expected.stdout,actual.stdout))if a!=b]
            failures.append(dict(index=index,label=c['label'],compiler=compiler,returncode=actual.returncode,stderr=actual.stderr.decode(errors='replace'),offsets=offsets[:32]))
            (OUT/('failure-%d-%s-original.bin'%(index,compiler))).write_bytes(expected.stdout)
            (OUT/('failure-%d-%s-native.bin'%(index,compiler))).write_bytes(actual.stdout)
            (OUT/('failure-%d-input.bin'%index)).write_bytes(path.read_bytes())
    if len(failures)>=args.max_failures:break
for label,output in batch_outputs.items():(OUT/(label+'-output.bin')).write_bytes(output)
after={str(f):sha(f.read_bytes())for f in paths};assert before==after
manifest=dict(scenario_count=len(cases),passed=not failures,executed_scenes=len(results)//(4 if linked else 3),script_sha256=sha(Path(__file__).read_bytes()),recompiled_sha256=linked['recompiled_sha256']if linked else None,symbols_sha256=linked['symbols_sha256']if linked else None,linked=linked,original_sha256=HASH,comparisons=len(results),changed_scenes=changed,terminal_scenes=terminal,failures=failures,results=results,source_before=before,source_after=after,whole_sources_unchanged=True,commands=commands,original_closure=closures,patches=patches,tables=tables,fixture_sha256=sha(b''.join(struct.pack('<16I',*c['header'])+c['data']for c in cases)),generated_sha256={f.name:sha(f.read_bytes())for f in OUT.iterdir()if f.suffix in('.c','.obj','.exe','.bin','.py')and not f.name.startswith(('failure-','input'))},candidate_sha256=sha(candidate.read_bytes()),scope='Original 1788-byte angular force consumer and complete lower closure; bounded PC53, raw arenas/globals, errors, ESP/nonvolatiles/x87TOP on normal calls. Terminal paths have state/error evidence only. No original code instrumentation or math stubs.')
(OUT/'verification.json').write_text(json.dumps(manifest,indent=2)+'\n')
print('%d scenes/%d comparisons/%d changes/%d terminal/%d failures'%(len(results)//(4 if linked else 3),len(results),changed,terminal,len(failures)),flush=True)
raise SystemExit(bool(failures))
