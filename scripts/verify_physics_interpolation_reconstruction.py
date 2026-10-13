"""Complete interpolation3 physical original/native3 scratch proof, zero credit.

Actual full original and reconstructed BRender/math closure, raw64KiB arenas,
normal PC53 and direct ESP/nonvolatile/x87TOP checks. Void EAX is excluded.
Terminal access-fault public states remain strict; normal-return ABI not claimed
for those paths. No game boundaries or output normalization.
"""
import argparse, hashlib, json, random, re, struct, subprocess, sys
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_OP_IMM, CS_OP_MEM, CS_OP_REG, CS_GRP_JUMP

HARNESS='#include <windows.h>\n#include <stdio.h>\n#include <stdlib.h>\n#include <string.h>\n#include <float.h>\n#include <io.h>\n#include <fcntl.h>\n#include <stdint.h>\n#include "49-physics.h"\n#include <math.h>\n\n#define M(x,y) mat->m[x][y]\n#define A(x,y) A->m[x][y]\n#define B(x,y) B->m[x][y]\n#define C(x,y) C->m[x][y]\nbr_matrix34 mattmp1, mattmp2;\nextern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison,builder_cdecl;\nextern unsigned __cdecl BuilderInvoke(void);\nstatic unsigned addresses[3], h[7], case_index;\nstatic unsigned char* heap;\ntypedef char layout_assert[sizeof(void*)==4&&sizeof(tPhysics_object)==1240&&offsetof(tPhysics_object,disable_move_rotate)==0xec&&offsetof(br_actor,t)==0x28?1:-1];\n\nstatic void snapshot(unsigned event, unsigned fault_address) {\n    unsigned header[3];\n#ifndef PHYSICAL_IMAGE\n    memcpy(heap+0x3b00,&mattmp1,48);\n    memcpy(heap+0x3b30,&mattmp2,48);\n#endif\n    header[0]=event;header[1]=h[0];header[2]=fault_address;\n    fwrite(header,sizeof(header),1,stdout);\n    fwrite(heap,65536,1,stdout);\n}\nstatic LONG WINAPI fault_observer(EXCEPTION_POINTERS* exception) {\n    if(exception->ExceptionRecord->ExceptionCode!=EXCEPTION_ACCESS_VIOLATION)return EXCEPTION_CONTINUE_SEARCH;\n    snapshot(3,(unsigned)exception->ExceptionRecord->ExceptionInformation[1]);\n    fflush(stdout);ExitProcess(0);return EXCEPTION_CONTINUE_SEARCH;\n}\nstatic void install_observer(void) {\n    typedef void* (WINAPI *add_handler)(ULONG,LONG (WINAPI*)(EXCEPTION_POINTERS*));\n    FARPROC address=GetProcAddress(GetModuleHandleA("kernel32.dll"),"AddVectoredExceptionHandler");\n    add_handler handler;\n    memcpy(&handler,&address,sizeof(handler));\n    if(handler==NULL||handler(1,fault_observer)==NULL)exit(38);\n}\nstatic int run(const char* path) {\n    FILE* file;\n    SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);\n    file=fopen(path,"rb");if(file==NULL)return 10;\n    heap=(unsigned char*)VirtualAlloc((void*)0x22000000,65536,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);\n    if(heap!=(void*)0x22000000)return 11;\n    _setmode(_fileno(stdout),_O_BINARY);install_observer();\n    for(case_index=0;fread(h,sizeof(h),1,file)==1;case_index++){\n        if(fread(heap,65536,1,file)!=1)return 12;\n        memcpy(&mattmp1,heap+0x3b00,48);memcpy(&mattmp2,heap+0x3b30,48);\n        _controlfp(_PC_53,_MCW_PC);_clearfp();\n        if(h[0]>=3)return 37;\n        builder_address=addresses[h[0]];builder_mode=h[0];builder_x=h[1];builder_y=h[4];\n        builder_poison=h[5];builder_stack[0]=h[2];\n#if defined(REC2_MATCHING)\n        builder_cdecl=0;builder_stack_count=1;\n#else\n        builder_cdecl=1;builder_stack_count=3;\n        builder_stack[2]=h[2];builder_stack[0]=h[1];builder_stack[1]=h[4];\n#endif\n        if(BuilderInvoke())return 14;\n        snapshot(0,0);\n    }\n    fclose(file);return 0;\n}\n'
FIXTURE_PROGRAM="BASE=0x22000000\nOBJECTS=(0x100,0x900,0x1100,0x1900)\nACTORS=(0x5000,0x5100,0x5200,0x5300)\ncases=[]\ndef add(**items):\n    case=dict(mode=0,time=0x3d23d70a,seed=len(cases)+1,disabled=0,tree=0,root_list=False,omega=(.125,-.25,.375),patches=[],actor_matrix_at=None,subject=0,unused_edx=0xcccccccc,poison=0xcccccccc)\n    case.update(items);cases.append(case)\n\nfor mode in range(3):\n    for seed in range(32):add(mode=mode,seed=seed+1)\nfor mode in range(3):\n    for disabled in (0,1,2,128,255):\n        for tree in range(4):add(mode=mode,disabled=disabled,tree=tree,root_list=bool(tree&1))\nfor mode in range(3):\n    for time in (0,0x80000000,1,0x80000001,0x38d1b716,0x38d1b717,0x38d1b718,0xb8d1b717,0x3f800000,0xbf800000,0x4f000000,0x5a000000,0x5b000000,0x5f000000,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7f812345,0xff812345,0x7fc12345,0xffc12345):\n        add(mode=mode,time=time,omega=(0.,0.,1.))\nfor mode in range(3):\n    for omega in ((0.,0.,0.),(1.,0.,0.),(-1.,0.,0.),(1.,2.,3.),(1e-20,2e-20,3e-20),(1e20,2e20,3e20)):\n        for time in (0x3d23d70a,0xbd23d70a,0x38d1b717):add(mode=mode,time=time,omega=omega)\nfor mode in range(3):\n    for offset in (0x1c0,0x1c4,0x1c8,0x68,0x6c,0x70,0x14,0x18,0x1c):\n        for bits in (0,0x80000000,1,0x80000001,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7f812345,0xff812345,0x7fc12345,0xffc12345):\n            add(mode=mode,patches=[(OBJECTS[0]+offset,bits)])\nfor mode in range(3):\n    for offsets in ((0x2c0,0x2c4),(0x2c4,0x2c8),(0x168,0x5050),(0x16c,0x5054),(0x170,0x5058),(0x5050,0x114),(0x5054,0x118),(0x5058,0x11c)):\n        for a,b in ((0x7f812345,0xff812345),(0xff812345,0x7f812345),(0x7fc12345,0xffc12345),(0xffc12345,0x7fc12345),(0x7f812345,0xffc12345),(0xffc12345,0x7f812345)):\n            add(mode=mode,patches=[(offsets[0],a),(offsets[1],b)])\nfor mode in range(3):\n    for tree in (0,1,2,3):\n        for offset in (0x168,0x174,0x2c0,0x2e0,0x400):\n            add(mode=mode,tree=tree,actor_matrix_at=offset)\nfor mode in range(3):\n    for unused in (0,1,0x7f812345,0xdeadbeef,BASE+0x1900):add(mode=mode,unused_edx=unused)\n# NULL list is valid only for list consumer; other two preserve original faults.\nfor mode in range(3):add(mode=mode,object_override=0)\n\ndef make_case(case):\n    rng=random.Random(case['seed'])\n    arena=bytearray(words([rng.getrandbits(32)for _ in range(16384)]))\n    def put(at,*values):struct.pack_into('<'+'I'*len(values),arena,at,*[v&0xffffffff for v in values])\n    def vec(at,*values):struct.pack_into('<'+'f'*len(values),arena,at,*values)\n    for i,obj in enumerate(OBJECTS):\n        actor=ACTORS[i]if i!=0 or case['actor_matrix_at']is None else case['actor_matrix_at']-0x2c\n        put(obj,BASE+actor)\n        arena[obj+0xec]=case['disabled']\n        vec(obj+0x14,.125,-.25,.375)\n        vec(obj+0x68,*[rng.uniform(-2,2)for _ in range(3)])\n        vec(obj+0x1c0,*case['omega'])\n        put(obj+0x220,0);put(obj+0x224,0)\n        # Distinct valid actor matrices; full original math path remains real.\n        mat=(1.,0.,0.,0.,1.,0.,0.,0.,1.,4.+i,5.+i,6.+i)\n        vec(actor+0x2c,*mat)\n    if case['tree']:\n        put(OBJECTS[0]+0x224,BASE+OBJECTS[1])\n        put(OBJECTS[1]+0x220,BASE+OBJECTS[2])\n    if case['tree']>=2:put(OBJECTS[1]+0x224,BASE+OBJECTS[3])\n    if case['root_list']:put(OBJECTS[0]+0x220,BASE+OBJECTS[3]);put(OBJECTS[1]+0x224,0)\n    for at,value in case['patches']:put(at,value)\n    if case.get('preserve_actor_pointer'):\n        put(OBJECTS[0],BASE+case['actor_matrix_at']-0x2c)\n    return words([case['mode'],case.get('object_override',BASE+OBJECTS[case['subject']]),case['time'],0,case['unused_edx'],case['poison'],0])+arena\n\n# Paired raw incoming-time and loaded velocity NaNs, including real child calls.\n# These are sensitive to whether a typed recursive argument quiets the raw time.\nfor mode in (1,2):\n    for time,value in ((0x7f812345,0xff812345),(0xff812345,0x7f812345),(0x7f812345,0xffc12345),(0xff812345,0x7fc12345),(0x7fc12345,0xff812345),(0xffc12345,0x7f812345)):\n        for lane in range(3):\n            for tree in (0,2):\n                add(mode=mode,time=time,tree=tree,patches=[(obj+0x68+4*lane,value)for obj in OBJECTS])\n# Actor pointer is itself a real translation output. Preserve the initial\n# pointer after constructing the overlapping matrix, then observe live reloads.\nfor mode in range(3):\n    for time in (0,0x3f800000,0xbf800000):\n        add(mode=mode,time=time,actor_matrix_at=0xdc,preserve_actor_pointer=True)\ndef scalar(bits):return struct.unpack('<f',struct.pack('<I',bits))[0]\ndef float_bits(value):return struct.unpack('<I',struct.pack('<f',value))[0]\nfor mode in (1,2):\n    for target in (BASE+0x5100,BASE+0x5200):\n        delta=float_bits(scalar(target)-scalar(BASE+0xb0))\n        add(mode=mode,time=0x3f800000,omega=(0.,0.,0.),actor_matrix_at=0xdc,preserve_actor_pointer=True,\n            patches=[(0x168,delta),(0x16c,0),(0x170,0)])\n# Genuine live child/next redirection caused by an actor-position store.\nfor mode,field,mat in ((1,0x324,0x300),(2,0x324,0x300),(2,0x320,0x2fc)):\n    delta=float_bits(scalar(BASE+0x1900)-scalar(BASE+0x900))\n    add(mode=mode,time=0x3f800000,omega=(0.,0.,0.),actor_matrix_at=mat,\n        patches=[(0x168,delta),(0x16c,0),(0x170,0),(0x320,0),(0x324,0),(field,BASE+0x900)])\n# Retain velocityY/Z captured before preceding overlapping position stores.\nfor mode in range(3):\n    for mat in (0x144,0x148,0x14c,0x1b8,0x1bc,0x1c0):\n        add(mode=mode,actor_matrix_at=mat)\n# Signed qword conversion boundary forwarded to actual low16 BRender angle.\nboundary=float_bits(9223372036854775808.0/10430.378350470453)\nfor mode in range(3):\n    for bits in range(boundary-3,boundary+4):\n        for sign in (0,0x80000000):add(mode=mode,time=bits|sign,omega=(0.,0.,1.))\n# Finite exponent spread and threshold neighboring scalar time words.\nfor seed in range(72):\n    rng=random.Random(0x4c267000+seed)\n    omega_bits=[(rng.getrandbits(1)<<31)|(rng.randint(1,253)<<23)|rng.getrandbits(23)for _ in range(3)]\n    time=(rng.getrandbits(1)<<31)|(rng.randint(1,253)<<23)|rng.getrandbits(23)\n    add(mode=seed%3,time=time,patches=list(zip((0x2c0,0x2c4,0x2c8),omega_bits)))\nfor seed in range(24):\n    rng=random.Random(0x2670+seed)\n    omega=tuple(scalar(float_bits(rng.uniform(-8,8)))for _ in range(3))\n    magnitude=scalar(float_bits((sum(value*value for value in omega))**.5))\n    nearest=float_bits(.0001/magnitude)\n    for delta in (-1,0,1):add(mode=0,time=nearest+delta,omega=omega)\n"
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'src/carma2').is_dir())
sys.path.insert(0,str(ROOT/'scripts'))
from verify_tinted_native_reconstruction import extract
from verify_opponent_route_reconstruction import HASH,pe,raw,sha,words
import verify_phil_registry_reconstruction as registry
from inventory_reconstruction_backlog import mask_c

parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--output',type=Path,required=True)
parser.add_argument('--limit',type=int)
parser.add_argument('--case-index',type=int,action='append')
parser.add_argument('--candidate-source',type=Path)
parser.add_argument('--recompiled',type=Path)
parser.add_argument('--symbols',type=Path)
args=parser.parse_args()
assert bool(args.recompiled)==bool(args.symbols),'Supply both immutable PE and hash-bound symbols'
OUT=args.output.resolve();OUT.mkdir(exist_ok=True,parents=True)
physics=ROOT/'src/carma2/newcommon/49-physics.c';matrix=ROOT/'src/brender/core/math/matrix34.c'
candidate=args.candidate_source.resolve()if args.candidate_source else physics
paths=[candidate,Path(__file__),physics,matrix,
       ROOT/'src/carma2/newcommon/49-physics.h',ROOT/'src/carma2/include/rec2_types.h',
       ROOT/'src/carma2/hooks/c2_hooks.h',ROOT/'src/carma2/hooks/c2_math.h',
       ROOT/'src/brender/core/math/matrix34.h',ROOT/'src/brender/include/brender/brender.h']
paths += [ROOT/'scripts'/name for name in ('verify_tinted_native_reconstruction.py','verify_opponent_route_reconstruction.py','verify_phil_registry_reconstruction.py','verify_opponent_navigation_reconstruction.py','inventory_reconstruction_backlog.py')]
if args.recompiled:paths.extend([args.recompiled,args.symbols,args.recompiled.with_suffix('.map')])
before={str(p):p.read_bytes()for p in paths}
(OUT/'source-before.json').write_text(json.dumps({p:sha(b)for p,b in before.items()},indent=2)+'\n',encoding='utf-8',newline='\n')
snapshot=OUT/'source-snapshot';snapshot.mkdir(exist_ok=True)
snapshots={}
for index,(path,bits)in enumerate(before.items()):
    p=snapshot/(str(index)+'-'+Path(path).name);p.write_bytes(bits);snapshots[path]=dict(path=str(p),sha256=sha(bits))
data=(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes();assert sha(data)==HASH
nt,base,size,tab=pe(data);cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
roots=[('FUN_004c2670',0x4c2670,334),('InterpolateSingleObject',0x4c2830,106),('FUN_004c28a0',0x4c28a0,110)]
entries=roots+[('BrMatrix34ApplyV',0x533520,107),('BrMatrix34PreRotate',0x533d40,63),('__ftol',0x575b80,39),('BrMatrix34Rotate',0x532940,333),('BrMatrix34Mul',0x532620,404),('BrMatrix34Copy',0x5325d0,79)]
catalog={a:(name,n)for name,a,n in entries};todo=[a for _,a,_ in roots];seen=set();spec=[];patches=[];constants={}
while todo:
    start=todo.pop(0)
    if start in seen:continue
    seen.add(start);name,n=catalog[start];instructions=list(cs.disasm(raw(data,start,n),start));assert sum(i.size for i in instructions)==n
    for ins in instructions:
        for op in ins.operands:
            at=site=None;width=4
            if op.type==CS_OP_MEM and base<=op.mem.disp<base+size:
                at=op.mem.disp;site=ins.address+ins.disp_offset-base;width=op.size;assert ins.disp_size==4
            elif op.type==CS_OP_IMM and ins.mnemonic in('mov','push')and base<=op.imm<base+size:
                at=op.imm;site=ins.address+ins.imm_offset-base;assert ins.imm_size==4
            if at is not None:
                absolute=0
                if 0x6b09c0<=at<0x6b0a20:
                    target=0x22003b00+(at-0x6b09c0);absolute=1
                else:
                    section=next((struct.unpack_from('<I',data,tab+40*k+36)[0]for k in range(struct.unpack_from('<H',data,nt+6)[0])if base+struct.unpack_from('<I',data,tab+40*k+12)[0]<=at<base+struct.unpack_from('<I',data,tab+40*k+12)[0]+max(struct.unpack_from('<I',data,tab+40*k+8)[0],struct.unpack_from('<I',data,tab+40*k+16)[0])),None)
                    assert section is not None and not section&0x80000000,(name,hex(at),ins.op_str)
                    constants[hex(at)]=raw(data,at,width).hex();target=at-base
                patches.append((site,at,target,absolute))
        if ins.mnemonic=='call'or ins.mnemonic.startswith('j'):
            assert ins.operands[0].type==CS_OP_IMM
            target=ins.operands[0].imm
            if not start<=target<start+n:
                assert target in catalog,(name,ins.op_str);todo.append(target)
    spec.append(dict(name=name,address=start,bytes=n,sha256=sha(raw(data,start,n))))
(OUT/'original-addresses.bin').write_bytes(words([a-base for _,a,_ in roots]+[len(patches)])+b''.join(words(p)for p in patches))
COMMON=HARNESS
caller,physical,_=registry.literals()
old=' cmp builder_mode,8\n jne notwide\n fstp qword ptr builder_wide\n xor eax,eax\n notwide:\n cmp builder_mode,1\n je zeroreturn\n cmp builder_mode,10\n jne resultready\n zeroreturn:\n xor eax,eax\n resultready:'
assert old in caller;caller=caller.replace(old,' xor eax,eax')
caller=caller.replace('builder_poison;','builder_poison,builder_cdecl;').replace(' mov builder_result,eax\n mov ecx,esp',' mov builder_result,eax\n cmp builder_cdecl,0\n je interpolation_stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n interpolation_stackready:\n mov ecx,esp')
old='image[hdr[12]]=0xe9;relative=(unsigned)RayBoundary-((unsigned)image+hdr[12]+5);memcpy(image+hdr[12]+1,&relative,4);'
assert old in physical;physical=physical.replace(old,'').replace('relative,','').replace('hdr[14]','hdr[4]').replace('n=hdr[13]','n=hdr[3]').replace('i<12;i++)addresses','i<3;i++)addresses')
helpers=('PhysicsJointScalar','PhysicsJointSum','FrictionRead','FrictionDivideScalars','FrictionAddScalar','FrictionSubtractScalar','PartnerMultiplyRawScalar','FrictionLessOrUnordered','FrictionStatus','WakePointer','AngularForceAngle')
body='\n'.join(extract(before[str(physics)].decode(),name)for name in helpers if name!='FrictionStatus')+'\n'
body='#if defined(_MSC_VER) && _MSC_VER < 1300\n'+extract(before[str(physics)].decode(),'FrictionStatus')+'\n#endif\n'+body
text=before[str(matrix)].decode()
rotate=extract(text,'BrMatrix34Rotate');start=text.index(extract(text,'MatrixRotateScalar'));end=text.index(rotate)+len(rotate)
body+=text[start:end]+'\n#undef MR_PRODUCT\n#undef MR_SUM\n#undef MR_DIFFERENCE\n'
for name in('BrMatrix34Mul','BrMatrix34Copy','BrMatrix34ApplyV','BrMatrix34PreRotate'):
    body+=extract(text,name)+'\n'
candidate_text=before[str(candidate)].decode()
if candidate==physics:
    first=candidate_text.index('typedef void (C2_HOOK_FAKE_THISCALL *tInterpolation_scalar_call)')
    last=extract(candidate_text,'FUN_004c28a0');candidate_text=candidate_text[first:candidate_text.index(last)+len(last)]
body+=candidate_text+'\n'
body+='\nint C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;addresses[0]=(unsigned)FUN_004c2670;addresses[1]=(unsigned)InterpolateSingleObject;addresses[2]=(unsigned)FUN_004c28a0;return run(argv[1]);}\n'
registry.COMMON='#define PHYSICAL_IMAGE\n'+COMMON
commands=registry.compile_sources(OUT,COMMON+'\n'+body,physical,caller)
matching=None
if args.recompiled:
    linked_data=args.recompiled.read_bytes();mapping=json.loads(args.symbols.read_text())
    assert mapping['format']==1 and mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(linked_data)
    symbols={r['orig']:r['recomp']for r in mapping['symbols']};map_bytes=args.recompiled.with_suffix('.map').read_bytes()
    functions={int(address,16):(name,owner.strip())for name,address,owner in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',map_bytes.decode('latin1'),re.M)}
    nt2,base2,size2,tab2=pe(linked_data);sections=[]
    for k in range(struct.unpack_from('<H',linked_data,nt2+6)[0]):
        virtual,rva,n,off=struct.unpack_from('<IIII',linked_data,tab2+40*k+8);flags=struct.unpack_from('<I',linked_data,tab2+40*k+36)[0];sections.append((base2+rva,max(virtual,n),flags))
    initial={symbols[a]:name for name,a,_ in entries if a in symbols}
    linked_roots=[symbols[a]for _,a,_ in roots]
    mutable={symbols[0x6b09c0]:(0x22003b00,48),symbols[0x6b09f0]:(0x22003b30,48)}
    todo=list(linked_roots);seen=set();graph=[];linked_patches={};linked_constants={};indirect=[];code_pointers=[]
    def target_register_family(register):
        name=cs.reg_name(register)
        for family,aliases in (('eax',('eax','ax','al','ah')),('ebx',('ebx','bx','bl','bh')),('ecx',('ecx','cx','cl','ch')),('edx',('edx','dx','dl','dh')),('esi',('esi','si')),('edi',('edi','di')),('ebp',('ebp','bp')),('esp',('esp','sp'))):
            if name in aliases:return family
        return name
    for start in todo:
        if start in seen:continue
        seen.add(start);assert start in functions,hex(start)
        end=min(a for a in functions if a>start);name=initial.get(start,functions[start][0]);owner=functions[start][1]
        plain=name.lstrip('_@').split('@')[0]
        if start not in initial:
            assert owner in('49-physics.c.obj','matrix34.c.obj','libbrender:matrix34.c.obj')and plain.startswith(('Interpolation','PhysicsJointScalar','PhysicsJointSum','Friction','PartnerMultiplyRawScalar','WakePointer','AngularForceAngle','MatrixRotate'))or owner.startswith('LIBC:')and plain in('ftol','_ftol','sqrt','_sqrt','CIsqrt','_CIsqrt','memcpy'),(name,owner)
        pending=[start];instructions={}
        while pending:
            location=pending.pop();known_targets={}
            while location not in instructions:
                assert start<=location<end,(name,hex(location))
                ins=next(cs.disasm(raw(linked_data,location,15),location));instructions[location]=ins.bytes.hex()
                if ins.mnemonic=='mov'and ins.operands[0].type==CS_OP_REG and ins.operands[0].size==4 and ins.operands[1].type==CS_OP_IMM and ins.operands[1].imm in linked_roots:
                    known_targets[ins.operands[0].reg]=(ins.operands[1].imm,ins.address)
                else:
                    written={target_register_family(register)for register in ins.regs_access()[1]}
                    for tracked in list(known_targets):
                        if target_register_family(tracked)in written:known_targets.pop(tracked,None)
                if ins.mnemonic=='call'or ins.group(CS_GRP_JUMP):
                    if ins.operands[0].type!=CS_OP_IMM:
                        assert ins.mnemonic=='call'and ins.operands[0].type==CS_OP_REG and ins.operands[0].reg in known_targets,('Unproved rawForward target dataflow',name,hex(ins.address),ins.op_str)
                        target,load=known_targets[ins.operands[0].reg];assert target in linked_roots
                        indirect.append(dict(function=name,address=ins.address,load=load,register=cs.reg_name(ins.operands[0].reg),target=target,authority='Same-block exact original interpolation-root function pointer literal; no intervening register write.'))
                        todo.append(target)
                    else:
                        target=ins.operands[0].imm
                        if start<=target<end:
                            if ins.mnemonic!='call':pending.append(target)
                            else:assert start in linked_roots and target==start,('Unexpected local call',name,ins.op_str)
                        else:
                            assert target in functions,(name,ins.op_str);todo.append(target)
                for op in ins.operands:
                    value=site=None;width=4
                    if op.type==CS_OP_MEM and base2<=op.mem.disp<base2+size2:
                        value=op.mem.disp;site=ins.address+ins.disp_offset-base2;width=op.size;assert ins.disp_size==4
                    elif op.type==CS_OP_IMM and ins.mnemonic in('mov','push')and base2<=op.imm<base2+size2:
                        value=op.imm;site=ins.address+ins.imm_offset-base2;assert ins.imm_size==4
                    if value is not None:
                        region=next(s for s in sections if s[0]<=value<s[0]+s[1]);absolute=0
                        head=next((a for a,(destination,length)in mutable.items()if a<=value<a+length),None)
                        if head is not None:target=mutable[head][0]+value-head;absolute=1
                        elif region[2]&0x20000000:
                            assert value in linked_roots and op.type==CS_OP_IMM and ins.mnemonic in('push','mov'),('Unproved executable pointer literal',name,hex(value),ins.op_str)
                            target=value-base2;code_pointers.append(dict(function=name,instruction=ins.address,site=site,target=value,root_name=initial[value]))
                        else:
                            assert not region[2]&0x80000020,(name,hex(value),ins.op_str)
                            linked_constants[hex(value)]=raw(linked_data,value,width).hex();target=value-base2
                        patch=(site,value,target,absolute);assert site not in linked_patches or linked_patches[site]==patch;linked_patches[site]=patch
                if ins.mnemonic=='call':known_targets.clear()
                if ins.mnemonic.startswith('ret')or ins.mnemonic in('jmp','int3','int'):break
                location+=ins.size
        graph.append(dict(name=name,address=start,extent=end-start,instructions=sorted(instructions.items()),instruction_count=len(instructions),instruction_sha256=sha(b''.join(bytes.fromhex(bits)for _,bits in sorted(instructions.items())))))
    # A previously visited CALL must not hide an alternate edge entering after
    # its literal load. Reject every direct incoming branch that bypasses it.
    for call in indirect:
        row=next(row for row in graph if row['name']==call['function']and row['address']<=call['load']<row['address']+row['extent'])
        for address,bits in row['instructions']:
            ins=next(cs.disasm(bytes.fromhex(bits),address))
            if ins.group(CS_GRP_JUMP)and ins.operands[0].type==CS_OP_IMM:
                target=ins.operands[0].imm
                if call['load']<target<=call['address']:
                    assert call['load']<=address<=call['address'],('Branch bypasses rawForward literal load',call,hex(address),hex(target))
    rr,rz=struct.unpack_from('<II',linked_data,nt2+24+96+5*8);cursor=base2+rr;checked=[]
    active={a+k for row in graph for a,bits in row['instructions']for k in range(len(bits)//2)}
    while cursor<base2+rr+rz:
        page,n=struct.unpack('<II',raw(linked_data,cursor,8));assert n>=8
        for word in struct.unpack('<'+'H'*((n-8)//2),raw(linked_data,cursor+8,n-8)):
            if word>>12==3:
                site=page+(word&4095)
                if base2+site in active:assert site in linked_patches,hex(site);checked.append(site)
        cursor+=n
    packet=OUT/'matching-addresses.bin'
    packet.write_bytes(words([r-base2 for r in linked_roots]+[len(linked_patches)])+b''.join(words(row)for _,row in sorted(linked_patches.items())))
    matching=dict(recompiled_sha256=sha(linked_data),symbols_sha256=sha(args.symbols.read_bytes()),map_sha256=sha(map_bytes),graph=graph,constants=linked_constants,patches=list(linked_patches.values()),mutable_globals=mutable,relocations_checked=checked,indirect_root_calls=indirect,code_pointer_sites=code_pointers,patch_file_sha256=sha(packet.read_bytes()))
namespace=dict(random=random,struct=struct,words=words)
exec(compile(FIXTURE_PROGRAM,'<retained-interpolation-fixtures>','exec'),namespace)
cases=namespace['cases'];make_case=namespace['make_case']
indices=args.case_index or list(range(len(cases)))
if args.limit:indices=indices[:args.limit]
results=[];failures=[]
for index in indices:
    payload=make_case(cases[index]);path=OUT/'case.bin';path.write_bytes(payload)
    original_command=[str(OUT/'physical.exe'),str(path),str(ROOT/'build-reference/originals/CARMA2_HW.EXE'),str(OUT/'original-addresses.bin')]
    run=subprocess.run(original_command,capture_output=True,timeout=30);assert run.returncode==0,('original',index,run.returncode)
    expected=run.stdout;assert expected and len(expected)%65548==0
    events=[dict(event=e,mode=m,fault_address=a)for offset in range(0,len(expected),65548)for e,m,a in [struct.unpack_from('<III',expected,offset)]]
    for compiler in('msvc5','msvc','mingw'):
        actual=subprocess.run([str(OUT/(compiler+'.exe')),str(path)],capture_output=True,timeout=30)
        row=dict(index=index,compiler=compiler,fixture_sha256=sha(payload),original_output_sha256=sha(expected),original_output_bytes=len(expected),original_events=events,normal_return_abi_checked=events[-1]['event']==0,actual_output_sha256=sha(actual.stdout),actual_output_bytes=len(actual.stdout),exit=actual.returncode,passed=actual.returncode==0 and actual.stdout==expected)
        results.append(row)
        if not row['passed']:
            row['differences']=[dict(offset=j,expected=expected[j:j+4].hex(),actual=actual.stdout[j:j+4].hex())for j in range(0,min(len(expected),len(actual.stdout)),4)if expected[j:j+4]!=actual.stdout[j:j+4]]
            failures.append(row);(OUT/f'failure-{index}-{compiler}.json').write_text(json.dumps(row,indent=2)+'\n')
            (OUT/f'failure-{index}.bin').write_bytes(payload);(OUT/f'failure-{index}.original-output.bin').write_bytes(expected);(OUT/f'failure-{index}.{compiler}-output.bin').write_bytes(actual.stdout)
    if matching:
        actual=subprocess.run([str(OUT/'physical.exe'),str(path),str(args.recompiled.resolve()),str(OUT/'matching-addresses.bin')],capture_output=True,timeout=30)
        row=dict(index=index,compiler='matching',fixture_sha256=sha(payload),original_output_sha256=sha(expected),original_output_bytes=len(expected),original_events=events,normal_return_abi_checked=events[-1]['event']==0,actual_output_sha256=sha(actual.stdout),actual_output_bytes=len(actual.stdout),exit=actual.returncode,passed=actual.returncode==0 and actual.stdout==expected)
        results.append(row)
        if not row['passed']:
            row['differences']=[dict(offset=j,expected=expected[j:j+4].hex(),actual=actual.stdout[j:j+4].hex())for j in range(0,min(len(expected),len(actual.stdout)),4)if expected[j:j+4]!=actual.stdout[j:j+4]]
            failures.append(row);(OUT/f'failure-{index}-matching.json').write_text(json.dumps(row,indent=2)+'\n')
            (OUT/f'failure-{index}.bin').write_bytes(payload);(OUT/f'failure-{index}.original-output.bin').write_bytes(expected);(OUT/f'failure-{index}.matching-output.bin').write_bytes(actual.stdout)
    if index%10==0:print(index,len(failures),flush=True)
    if len(failures)>=12:break
after={str(p):p.read_bytes()for p in paths};assert before==after,'Source changed during proof'
manifest=dict(scope=__doc__,matching=matching,original_sha256=HASH,total_scenes=len(cases),selected_scenes=len(indices),scenes=indices.index(index)+1,complete_suite=len(indices)==len(cases)and indices.index(index)+1==len(cases),comparisons=len(results),source_stable=True,sources={p:sha(b)for p,b in before.items()},source_snapshots=snapshots,spec=spec,constants=constants,patches=patches,input_sha256=sha(b''.join(make_case(cases[i])for i in indices)),results=results,failures=failures,commands=commands,generated_sha256={str(p):sha(p.read_bytes())for p in OUT.iterdir()if p.suffix in('.c','.obj','.exe','.bin','.txt')})
(OUT/'verification.json').write_text(json.dumps(manifest,indent=2)+'\n',encoding='utf-8',newline='\n')
print(manifest['scenes'],manifest['comparisons'],len(failures));assert not failures
