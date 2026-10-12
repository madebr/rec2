#!/usr/bin/env python3
"""Physical original/native three-entry joint application and workspace proof.

The actual ApplySingleForce, AllocateObjectWorkspace and RedoQuickHingeVelocities
graph executes with real record constructors, matrix/partner helpers and BRender
transforms. Default source is production; --candidate-source permits an isolated
complete draft. Missing required bodies fail extraction. No game dependency is
substituted. Only CRT exit and supplied error callback are controlled boundaries.
Full arenas, scratch globals and ordered callback/exit snapshots compare at PC53.
Normal calls check ESP, preserved registers and x87 TOP. No FP-status/unmasked
traps or nonx86 equivalence claim. Parent preparation/follower remains unfinished.
"""
import argparse,ast,hashlib,json,os,random,re,struct,subprocess,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'scripts'))
from verify_tinted_native_reconstruction import extract
from verify_opponent_route_reconstruction import HASH,sha,pe,raw,words
import verify_phil_registry_reconstruction as registry
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM,CS_OP_REG

def workspace_block(source):
    start=source.index('/* Hinge response workspace. */')
    marker='static void C2_HOOK_CDECL ApplicationScaleAdd('
    end=source.index(marker,start) if marker in source[start:] else source.index('// RestoreListOmega',start)
    return source[start:end]

def compile_linked(output,recompiled,symbols_path,spec,prefix,driver):
    data=recompiled.read_bytes();mapping=json.loads(symbols_path.read_text())
    assert mapping['format']==1 and mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(data)
    symbols={r['orig']:r['recomp'] for r in mapping['symbols']}
    map_path=recompiled.with_suffix('.map');map_bytes=map_path.read_bytes()
    functions={int(a,16):(n,o.strip()) for n,a,o in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',map_bytes.decode('latin1'),re.M)}
    nt,base,size,table=pe(data);flat=bytearray(max(size,0x2940d0));sections=[]
    for i in range(struct.unpack_from('<H',data,nt+6)[0]):
        virtual,rva,n,offset=struct.unpack_from('<IIII',data,table+40*i+8);flags=struct.unpack_from('<I',data,table+40*i+36)[0]
        flat[rva:rva+n]=data[offset:offset+n];sections.append((base+rva,max(virtual,n),flags))
    globals_={symbols[original]:0x50000000+original-0x400000 for original in (0x6940bc,0x6940c0,0x6940c4,0x6940c8)}
    exit_address=symbols.get(0x5784e0)
    if exit_address is None:
        candidates=[a for a,(n,o) in functions.items() if n=='_exit' and o.startswith('LIBC:')];assert len(candidates)==1;exit_address=candidates[0]
    initial={symbols[a]:name for name,a,_ in spec};pending=list(initial);seen=set();closures=[];patched=set();relocations=[];tables=[]
    cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
    def patch(site,value,kind,width=4):
        if site in patched:return
        old=struct.unpack_from('<I',flat,site)[0];relocations.append(dict(site=site,original=old,mapped=value,kind=kind,width=width));struct.pack_into('<I',flat,site,value);patched.add(site)
    while pending:
        start=pending.pop(0)
        if start in seen:continue
        seen.add(start);end=min(a for a in functions if a>start);name=initial.get(start,functions[start][0]);todo=[start];instructions={};callback_loads={}
        while todo:
            at=todo.pop()
            while at not in instructions:
                assert start<=at<end,(name,hex(at),'outside linked function')
                ins=next(cs.disasm(raw(data,at,15),at,count=1));instructions[at]=ins.bytes.hex()
                if ins.mnemonic=='mov' and ins.operands[0].type==CS_OP_REG and ins.operands[1].type==CS_OP_MEM and ins.operands[1].mem.disp==symbols[0x6940c0]:callback_loads[ins.operands[0].reg]=ins.address
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
                                assert owner=='49-physics.c.obj' and short.startswith(('Application','PhysicsJoint','PhysicsScalarNaN','PhysicsSubtractScalar','PhysicsCopyWord','Force','Partner')),(name,functions[target])
                            pending.append(target)
                    elif ins.mnemonic=='call':
                        assert name in ('GetPhysicsScratchPointer','IncreasePhysicsScratchPointer','PhysicsError','AllocateObjectWorkspace') and operand.type==CS_OP_REG and operand.reg in callback_loads and 0<ins.address-callback_loads[operand.reg]<=64,(name,ins.op_str,'unproven callback register load')
                    else:assert is_table,(name,ins.op_str)
                if ins.mnemonic.startswith('ret') or ins.mnemonic in ('jmp','int3','int'):break
                at+=ins.size
        closures.append(dict(name=name,address=start,extent=end-start,instruction_count=len(instructions),instruction_sha256=sha(json.dumps(sorted(instructions.items())).encode())))
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
    matching_driver=driver.replace(old_size,str(len(flat))).replace('0xb7960,0xb8cf0,0xb8350',','.join(hex(symbols[a]-base) for _,a,_ in spec[:3])).replace('0x1784e0',hex(exit_address-base))
    (output/'matching.c').write_text(prefix+'\n#define PHYSICAL_ORIGINAL\n'+matching_driver)
    gcc=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe';inc=[ROOT/p for p in ('src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/brender/include','src/carma2/s3/include','src/carma2/platform')]
    command=[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-DREC2_MATCHING',*['-I'+str(p) for p in inc],str(output/'matching.c'),str(output/'caller.obj'),'-o',str(output/'matching.exe')]
    result=subprocess.run(command,cwd=output,capture_output=True,text=True);(output/'matching-build.txt').write_text(result.stdout+result.stderr);assert result.returncode==0,result.stdout+result.stderr
    return dict(recompiled_sha256=sha(data),symbols_sha256=sha(symbols_path.read_bytes()),map_sha256=sha(map_bytes),graph=closures,globals=[dict(actual=a,transport=t) for a,t in globals_.items()],allocator_tables=tables,relocations=relocations,crt_exit=exit_address,command=command)


def compile_harness(output,candidate_source=None,recompiled=None,symbols_path=None):
    OUT=output.resolve()
    OUT.mkdir(parents=True,exist_ok=True)
    candidate=candidate_source.resolve() if candidate_source else ROOT/'src/carma2/newcommon/49-physics.c'
    production=ROOT/'src/carma2/newcommon/49-physics.c'
    matrix=ROOT/'src/brender/core/math/matrix34.c'
    inputs=[production,production.with_suffix('.h'),matrix,ROOT/'src/brender/core/math/vector.c',ROOT/'src/carma2/include/rec2_types.h',ROOT/'src/carma2/hooks/c2_hooks.h',candidate,Path(__file__),ROOT/'scripts/verify_tinted_native_reconstruction.py',ROOT/'scripts/verify_opponent_route_reconstruction.py',ROOT/'scripts/verify_opponent_navigation_reconstruction.py',Path(registry.__file__)]
    source_before={str(p):p.read_bytes() for p in inputs}
    src=source_before[str(production)].decode().replace('\r\n','\n');mat=source_before[str(matrix)].decode().replace('\r\n','\n');draft=source_before[str(candidate)].decode().replace('\r\n','\n')
    if not candidate_source:draft='\n'.join(extract(src,n) for n in ['ApplicationScaleAdd','ApplicationWorldAdd','ApplicationDirect','ApplicationHinged','ApplySingleForce','RedoQuickHingeVelocities','ApplicationJoint','ApplicationInlineTorque','ApplicationSecondary','AllocateObjectWorkspace'])
    spec=[('ApplySingleForce',0x4b7960,2534),('RedoQuickHingeVelocities',0x4b8cf0,287),('AllocateObjectWorkspace',0x4b8350,2237),('DRMatrix33Inverse',0x4be350,352),('SetUpForce',0x4be4b0,159),('SetUpTorqueHingePartners',0x4be550,1943),('SetUpHingePartners',0x4becf0,1942),('MakeMatrix',0x4bf490,3922),('BrMatrix34ApplyV',0x533520,107),('BrMatrix34TApplyV',0x533690,107),('BrVector3SetFloat',0x534590,25),('MakeJointVelocitiesVector',0x4b77b0,425),('CreateJointForce',0x4b8c20,201),('CreateJointTorque',0x4b8e10,198),('GetPhysicsScratchPointer',0x4c5c70,58),('IncreasePhysicsScratchPointer',0x4c5cb0,70),('PhysicsError',0x4c61c0,25)]
    spec += [('BrMatrix34LPInverse',0x532eb0,175),('BrMatrix34Mul',0x532620,416)]
    original=(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes();assert sha(original)==HASH
    nt,base,size,tab=pe(original);image=bytearray(size)
    for si in range(struct.unpack_from('<H',original,nt+6)[0]):
     _,rva,n,off=struct.unpack_from('<IIII',original,tab+40*si+8);image[rva:rva+n]=original[off:off+n]
    cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
    closures=[];patches=[]
    for index in range(4):
     at=0x4b8c10+4*index;target=struct.unpack('<I',raw(original,at,4))[0]
     assert 0x4b8350<=target<0x4b8c0d
     struct.pack_into('<I',image,at-base,target-base+0x50000000)
     patches.append(dict(site=at,address=target,width=4,kind='allocator-switch-target'))
    for name,start,n in spec:
     ins=list(cs.disasm(raw(original,start,n),start));assert sum(x.size for x in ins)==n,(name,n)
     (OUT/(f'{start:08x}.asm')).write_text('\n'.join(f'{x.address:08x}: {x.bytes.hex():24s} {x.mnemonic} {x.op_str}' for x in ins)+'\n')
     for x in ins:
      if x.mnemonic=='call' or x.mnemonic.startswith('j'):
       operand=x.operands[0]
       if operand.type==CS_OP_IMM:assert start<=operand.imm<start+n or operand.imm in {a for _,a,_ in spec} or (name in ('PhysicsError','GetPhysicsScratchPointer','IncreasePhysicsScratchPointer') and operand.imm==0x5784e0),(name,x.op_str)
       else:assert (name in ('PhysicsError','GetPhysicsScratchPointer','IncreasePhysicsScratchPointer') and x.mnemonic=='call') or (name=='AllocateObjectWorkspace' and x.mnemonic=='jmp' and operand.type==CS_OP_MEM and operand.mem.disp==0x4b8c10),(name,x.op_str)
      for o in x.operands:
       if o.type==CS_OP_MEM and base<=o.mem.disp<base+size:
        assert not o.mem.base and x.disp_size==4,(name,x.op_str)
        old=o.mem.disp;assert old in (0x4b8c10,0x58a768,0x58a780,0x6940bc,0x6940c0,0x6940c4,0x6940c8),(name,x.op_str)
        if old==0x58a768:assert o.size==4 and raw(original,old,4)==bytes(4)
        if old==0x58a780:assert o.size==8 and raw(original,old,8)==struct.pack('<d',1.)
        struct.pack_into('<I',image,x.address-base+x.disp_offset,old-base+0x50000000)
        patches.append(dict(site=x.address+x.disp_offset,address=old,width=o.size,bits=bytes(image[old-base:old-base+max(o.size,1)]).hex()))
     closures.append(dict(name=name,address=start,bytes=n,sha256=sha(raw(original,start,n))))
    (OUT/'physical-image.bin').write_bytes(image)

    prefix=r'''
    #include <windows.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <float.h>
    #include <io.h>
    #include <fcntl.h>
    #include "49-physics.h"
    #define M(x,y) mat->m[x][y]
    #define A(x,y) A->m[x][y]
    #define B(x,y) B->m[x][y]
    #define C(x,y) C->m[x][y]
    extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[6],builder_stack_count,builder_result,builder_poison,builder_cdecl;
    extern unsigned __cdecl BuilderInvoke(void);
    static unsigned char*code,*arena;
    static unsigned settings[16];
    typedef char application_layout[sizeof(void*)==4&&sizeof(tPhysics_object)==1240&&sizeof(tPhysics_joint_force)==96&&offsetof(tPhysics_joint_force,type)==0x5c&&offsetof(tPhysics_object,field_0x230)==0x230?1:-1];
    #if defined(__GNUC__)
    static LONG WINAPI ApplicationException(EXCEPTION_POINTERS* e){fprintf(stderr,"fault %08lx eip %08lx access %08lx\\n",e->ExceptionRecord->ExceptionCode,e->ContextRecord->Eip,(unsigned long)e->ExceptionRecord->ExceptionInformation[1]);ExitProcess(99);return EXCEPTION_CONTINUE_SEARCH;}
    #endif
    #define gPhysics_scratch_ptr (*(void**)(code+0x2940bc))
    #define gPhysics_error_cb (*(tPhysicsError_cbfn**)(code+0x2940c0))
    #define gPhysics_other_buffer (*(void**)(code+0x2940c4))
    #define gPhysics_other_buffer_capacity (*(int*)(code+0x2940c8))
    static void ApplicationSnapshot(unsigned event,unsigned value,unsigned message){unsigned h[7];h[0]=event;h[1]=value;h[2]=message;h[3]=(unsigned)gPhysics_scratch_ptr;h[4]=gPhysics_error_cb?1:0;h[5]=(unsigned)gPhysics_other_buffer;h[6]=(unsigned)gPhysics_other_buffer_capacity;fwrite(h,sizeof(h),1,stdout);fwrite(arena,65536,1,stdout);}
    static void ApplicationErrorAction(tPhysicsError error,const char*message){ApplicationSnapshot(1,(unsigned)error,(unsigned)message);if(settings[7]&1)gPhysics_scratch_ptr=arena+0xb000;if(settings[7]&2)gPhysics_other_buffer_capacity=0x100;if(settings[7]&4)gPhysics_error_cb=NULL;}
    static void __fastcall ApplicationOriginalCallback(tPhysicsError error,const char*message){ApplicationErrorAction(error,message);}
    static void C2_HOOK_FASTCALL ApplicationNativeCallback(tPhysicsError error,const char*message){ApplicationErrorAction(error,message);}
    static void C2_NORETURN C2_HOOK_CDECL ControlledExit(int exit_code){ApplicationSnapshot(2,(unsigned)exit_code,0);exit(0);}
    '''
    foundation=['PhysicsCopyWord','PhysicsSubtractScalar','PhysicsJointScalar','PhysicsJointSum','PhysicsJointProduct','PhysicsJointProductMemory','PhysicsJointAddScalar','PhysicsJointAddVelocity','PhysicsJointVelocityA','PhysicsJointVelocityB','MakeJointVelocitiesVector','CreateJointForce','CreateJointTorque','PhysicsScalarNaN']
    math='\n'.join(extract(mat,n) for n in ['BrMatrix34ApplyV','BrMatrix34TApplyV','BrMatrix34LPInverse','BrMatrix34Mul'])+'\n'+extract(source_before[str(ROOT/'src/brender/core/math/vector.c')].decode().replace('\r\n','\n'),'BrVector3SetFloat')
    body='\n'.join(extract(src,n) for n in foundation)+'\n'+math+'\n'+'\n'.join(extract(src,n) for n in ['DRMatrix33Inverse','SetUpForce'])+'\n'+workspace_block(src)+'\n#define exit ControlledExit\n'+'\n'.join(extract(src,n) for n in ['GetPhysicsScratchPointer','IncreasePhysicsScratchPointer','PhysicsError'])+'\n#undef exit\n'+draft
    driver=r'''
    int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned h[16],result,i,n=0,entry[3]={0xb7960,0xb8cf0,0xb8350};unsigned relative;void*function;
    if(argc!=3)return 2;SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);
    #if defined(__GNUC__)
    AddVectoredExceptionHandler(1,ApplicationException);
    #endif
    code=VirtualAlloc((void*)0x50000000,IMAGE_SIZE,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);arena=VirtualAlloc((void*)0x20000000,65536,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(code!=(void*)0x50000000||arena!=(void*)0x20000000)return 3;
    f=fopen(argv[2],"rb");if(!f)return 4;if(fread(code,IMAGE_SIZE,1,f)!=1)return 5;fclose(f);f=fopen(argv[1],"rb");if(!f)return 6;if(fread(h,sizeof(h),1,f)!=1||fread(arena,65536,1,f)!=1)return 7;fclose(f);
    memcpy(settings,h,sizeof(h));gPhysics_scratch_ptr=arena+0xa000+h[5];gPhysics_other_buffer=arena+0xa000;gPhysics_other_buffer_capacity=h[4];
    #ifdef PHYSICAL_ORIGINAL
    gPhysics_error_cb=h[6]?(tPhysicsError_cbfn*)ApplicationOriginalCallback:NULL;(void)ApplicationNativeCallback;
    #else
    gPhysics_error_cb=h[6]?ApplicationNativeCallback:NULL;(void)ApplicationOriginalCallback;
    #endif
    _controlfp(_PC_53,_MCW_PC);_setmode(_fileno(stdout),_O_BINARY);
    code[0x1784e0]=0xe9;relative=(unsigned)ControlledExit-((unsigned)code+0x1784e0+5);memcpy(code+0x1784e0+1,&relative,4);
    #ifdef PHYSICAL_ORIGINAL
    function=code+entry[h[0]];
    #else
    (void)entry;
    function=h[0]==0?(void*)ApplySingleForce:h[0]==1?(void*)RedoQuickHingeVelocities:(void*)AllocateObjectWorkspace;
    #endif
    builder_address=(unsigned)function;builder_mode=h[0]==2?0:2;builder_x=(unsigned)(arena+h[1]);builder_y=h[0]==2?(unsigned)(arena+h[2]):0xcccccccc;builder_poison=0xcccccccc;
    #if defined(REC2_MATCHING) || defined(PHYSICAL_ORIGINAL)
    builder_cdecl=0;n=h[0]==0?1:0;builder_stack[0]=h[3];
    #else
    builder_cdecl=1;builder_stack[0]=builder_x;n=h[0]==0?3:h[0]==1?1:2;builder_stack[1]=h[0]==2?builder_y:0xcccccccc;builder_stack[2]=h[3];
    #endif
    builder_stack_count=n;for(i=n;i<6;i++)builder_stack[i]=0;builder_cdecl=builder_cdecl;
    if(BuilderInvoke())return 14;result=h[0]==2?builder_result:0;ApplicationSnapshot(0,result,0);return 0;}
    '''.replace('IMAGE_SIZE',str(size))
    native=prefix+body+driver;physical=prefix+'\n#define PHYSICAL_ORIGINAL\n'+driver
    tree=ast.parse((ROOT/'scripts/verify_opponent_navigation_reconstruction.py').read_text());caller=next(n.value.value for n in ast.walk(tree) if isinstance(n,ast.Assign) and isinstance(n.value,ast.Constant) and any(isinstance(t,ast.Name) and t.id=='caller' for t in n.targets))
    caller=caller.replace('builder_stack[5]','builder_stack[6]').replace('builder_poison;','builder_poison,builder_cdecl;').replace(' mov builder_result,eax\n mov ecx,esp',' mov builder_result,eax\n cmp builder_cdecl,0\n je stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n stackready:\n mov ecx,esp')
    registry.COMMON='';commands=registry.compile_sources(OUT,native,physical,caller)
    assert source_before=={str(p):p.read_bytes() for p in inputs},'Source/helper changed while compiling'
    (OUT/'verified-bodies.c').write_text(body)
    dependency_functions={str(production.relative_to(ROOT)):{n:sha(extract(src,n).encode()) for n in foundation+['DRMatrix33Inverse','SetUpForce','GetPhysicsScratchPointer','IncreasePhysicsScratchPointer','PhysicsError']},str(matrix.relative_to(ROOT)):{n:sha(extract(mat,n).encode()) for n in ['BrMatrix34ApplyV','BrMatrix34TApplyV','BrMatrix34LPInverse','BrMatrix34Mul']},'src/brender/core/math/vector.c':{'BrVector3SetFloat':sha(extract(source_before[str(ROOT/'src/brender/core/math/vector.c')].decode().replace('\r\n','\n'),'BrVector3SetFloat').encode())}}
    linked=compile_linked(OUT,recompiled,symbols_path,spec,prefix,driver) if recompiled else None
    (OUT/'compiled.json').write_text(json.dumps(dict(commands=commands,closures=closures,patches=patches,candidate_sha256=sha(source_before[str(candidate)]),sources={str(Path(p).relative_to(ROOT)):sha(b) for p,b in source_before.items()},dependency_functions=dependency_functions,workspace_block_sha256=sha(workspace_block(src).encode()),source_unchanged_through_compile=True,verified_bodies_sha256=sha(body.encode()),linked=linked,generated_files_sha256={p.name:sha(p.read_bytes()) for p in OUT.iterdir() if p.suffix in ('.c','.obj','.exe','.bin')}),indent=2))
    print('Compiled actual three-entry application graph',flush=True)

def run_cases(output,candidate_source=None,case_indices=None):
    HEAP=0x20000000
    OUT=output.resolve()
    verifier_before=Path(__file__).read_bytes()
    compiled_path=output/'compiled.json';compiled=json.loads(compiled_path.read_text())
    artifact_before={n:hashlib.sha256((OUT/n).read_bytes()).hexdigest() for n in ['compiled.json','native.c','physical.c','caller.c','verified-bodies.c','caller.obj','physical-image.bin','physical.exe','msvc5.exe','msvc.exe','mingw.exe']}
    assert all(artifact_before[n]==compiled['generated_files_sha256'][n] for n in artifact_before if n not in ('compiled.json','verified-bodies.c'))
    source_before={p:hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in compiled['sources']}
    def extracted_snapshot():
     out={}
     for path,functions in compiled['dependency_functions'].items():
      text=(ROOT/path).read_text();out[path]={name:hashlib.sha256(extract(text,name).encode()).hexdigest() for name in functions}
     text=(ROOT/'src/carma2/newcommon/49-physics.c').read_text()
     out['workspace_block_sha256']=hashlib.sha256(workspace_block(text).encode()).hexdigest()
     out['candidate_sha256']=hashlib.sha256((candidate_source.resolve() if candidate_source else ROOT/'src/carma2/newcommon/49-physics.c').read_bytes()).hexdigest()
     return out
    extracted_before=extracted_snapshot()
    assert all(extracted_before[p]==v for p,v in compiled['dependency_functions'].items())
    assert extracted_before['workspace_block_sha256']==compiled['workspace_block_sha256'] and extracted_before['candidate_sha256']==compiled['candidate_sha256']
    HEAP=0x20000000
    cases=[]
    def add(**kw):
     c=dict(mode=0,type=10,hinge=0,partner=True,flags=0,seed=len(cases)+1,impulse=0x3f812345,primary=1,secondary=None,patch=[],record=0x3800,capacity=0x4000,cursor=0,callback=0,mutation=0,tree=0,geometry=False,output=0x4000,reload=None,swap=False,self_partner=False)
     c.update(kw);cases.append(c)
    for typ in (9,10,11,14,0,-1):
     for hinge in (0,1,2):
      for partner in (False,True):
       for seed in range(1,5):add(type=typ,hinge=hinge,partner=partner,seed=seed)
    for typ in (10,11):
     for flags in (0x800,0xffffffff):
      for hinge in (0,1,2):add(type=typ,hinge=hinge,flags=flags)
     for impulse in (0,0x80000000,0x7f800001,0xff800001,0x7fc12345,0xffc12345):add(type=typ,impulse=impulse)
    for mode in (1,2):
     for seed in range(1,11):add(mode=mode,seed=seed,primary=4 if mode==1 else seed%3+1)
    for primary in (None,0,1,2,3,4):
     for secondary in (None,1,2,3,5):add(mode=2,primary=primary,secondary=secondary)
    for flags in (0,0x400):
     for primary in (None,0,1,2,3,4):add(mode=2,primary=primary,flags=flags)
    for typ in (10,11,14):
     for hinge in (0,1,2):
      for at in (0x1004,0x1008,0x1014,0x1074,0x11a8,0x1804,0x1808,0x1874,0x19a8,0x3814,0x382c,0x3838):
       for bits in (0x7f800001,0xff800002,0x7fc12345,0xffc54321,0x7f800000,0xff800000,1,0x80000000):add(type=typ,hinge=hinge,patch=[(at,bits)])
    for mode in (1,2):
     for primary in ((4,) if mode==1 else (1,4)):
      for at in (0x1004,0x1008,0x1014,0x1074,0x11a8,0x1804,0x1808,0x1874,0x19a8,0x2808,0x2820,0x2838):
       for bits in (0x7f800001,0xff800002,0x7fc12345,0xffc54321,0x7f800000,0xff800000):add(mode=mode,primary=primary,patch=[(at,bits)])
    for typ in (10,11,14):
     for hinge in (0,1,2):
      for slots in ((0x382c,0x1004),(0x3814,0x382c),(0x382c,0x1074),(0x2838,0x2844),(0x2408,0x2838),(0x19a8,0x3838)):
       for pair in ((0x7f800001,0xff800002),(0xff800002,0x7f800001),(0x7fc12345,0xffc54321),(0xffc54321,0x7fc12345),(0x7f800000,0xff800000)):add(type=typ,hinge=hinge,patch=list(zip(slots,pair)))
      for record in (0x1018,0x1070,0x1818,0x1870):add(type=typ,hinge=hinge,record=record)
    for primary in (1,2,3,4):
     for seed in range(1,21):add(mode=2,primary=primary,seed=seed,geometry=True)
     for tree in range(1,4):add(mode=2,primary=primary,tree=tree)
    for primary,secondary in ((5,None),(-1,None),(None,0),(None,4),(4,None)):
     for callback in (0,1):
      for mutation in (0,1,2,4,7):
       add(mode=2,primary=primary,secondary=secondary,callback=callback,mutation=mutation,capacity=100 if primary==4 else 0x4000)
    for slots in ((0x2808,0x1014),(0x2814,0x1814),(0x2838,0x1074),(0x2844,0x1874),(0x1074,0x1078),(0x2838,0x283c),(0x11a8,0x11ac)):
     for pair in ((0x7f800001,0xff800002),(0xff800002,0x7f800001),(0x7fc12345,0xffc54321),(0xffc54321,0x7fc12345),(0x7fc12345,0xffc12345),(0xffc12345,0x7fc12345)):
      for primary in (1,2,4):add(mode=2,primary=primary,patch=list(zip(slots,pair)),geometry=True)
    for mode in (0,1,2):
     for at in (0x1074,0x11a8,0x1874,0x19a8,0x2838,0x2844):
      for bits in (0x00800000,0x007fffff,0x7f7fffff,0x60000001,0x3f000001):add(mode=mode,primary=4,patch=[(at,bits)],geometry=True,hinge=1)
    add(mode=2,primary=4,output=0x115c,reload='primary')
    add(mode=2,primary=None,secondary=5,output=0x1160,reload='secondary')
    for seed in range(1,13):add(mode=1,swap=True,seed=seed,geometry=True)
    add(mode=1,swap=True,self_partner=True)

    def fixture(c):
     rng=random.Random(c['seed']);b=bytearray(b'\xa5'*65536)
     def u(at,*v):struct.pack_into('<'+'I'*len(v),b,at,*[x&0xffffffff for x in v])
     def f(at,*v):struct.pack_into('<'+'f'*len(v),b,at,*v)
     def vector(at):f(at,*[rng.uniform(-2,2) for _ in range(3)])
     for i,o in enumerate((0x1000,0x1800,0x2000,0x3000)):
      jo=(0x2800,0x2880,0x2b00,0x2c00)[i];work=(0x6000,0x6080,0x6100,0x6180)[i]
      b[o:o+1240]=bytes(1240)
      f(o+4,2.375,1.25,4.75,8.0625)
      for off in (0x14,0x68,0x74,0x1a8):vector(o+off)
      f(o+0x8c,1,0,0,0,1,0,0,0,1,0,0,0)
      u(o+0x188,HEAP+jo);u(o+0x230,HEAP+(0x1000 if i==1 else 0x1800))
      u(o+0x19c,c['flags']);u(o+0x1e4,HEAP+work if c['hinge'] else 0)
      b[jo:jo+80]=bytes(80)
      u(jo,4 if i<2 else 2);vector(jo+8);vector(jo+0x14)
      f(jo+0x20,1,0,0,0,1,0,0,0,1,0,0,1)
      u(work,1 if c['hinge']==1 else 0,HEAP+(0x1000 if i==1 else 0x1800))
      f(work+8,1,0,0,0,1,0,0,0,1)
      f(work+0x38,1,0,0,0,1,0,0,0,1,0,0,0)
      if c['geometry']:
       for j in range(9):f(o+0x8c+4*j,(1 if j%4==0 else 0)+rng.uniform(-.5,.5))
       for at in (jo+0x20,jo+0x2c,jo+0x38,jo+0x44):vector(at)
     r=c['record'];u(r+4,HEAP+0x1000,HEAP+0x1800 if c['partner'] else 0);u(r+0x5c,c['type'])
     for at in (r+0x14,r+0x20,r+0x2c,r+0x38):vector(at)
     if c['mode']==1:
      u(0x11e4,HEAP+0x6000);u(0x19e4,HEAP+0x6080)
      if c['swap']:u(0x1230,0)
      if c['self_partner']:u(0x6004,HEAP+0x1000)
     if c['mode']==2:
      if c['primary'] is None:u(0x1188,0)
      else:u(0x2800,c['primary'])
      if c['secondary'] is not None:
       u(0x118c,HEAP+0x2900);b[0x2900:0x2950]=bytes(80);u(0x2900,c['secondary']);vector(0x2908);f(0x2920,1,0,0,0,1,0)
      if c['tree']:u(0x1224,HEAP+0x2000)
      if c['tree']>=2:u(0x2220,HEAP+0x3000)
      if c['tree']>=3:u(0x2224,HEAP+0x3000);u(0x2220,0)
     for at,v in c['patch']:u(at+0x3c00 if 0x2400<=at<0x2600 else at,v)
     if c['reload']:
      b[0x2d00:0x2d50]=bytes(80);u(0x2d00,4 if c['reload']=='primary' else 5);vector(0x2d08);vector(0x2d14);f(0x2d20,1,0,0,0,1,0,0,0,1,0,0,1)
      u(0x2820 if c['reload']=='primary' else 0x2920,HEAP+0x2d00)
     return struct.pack('<16I',c['mode'],c['record'] if c['mode']==0 else 0x1000,c['output'],c['impulse'],c['capacity'],c['cursor'],c['callback'],c['mutation'],*[0]*8)+b

    indices=case_indices if case_indices else list(range(len(cases)))
    results=[]
    if compiled.get('linked'):
     for name in ('matching.c','matching.exe','matching-image.bin'):artifact_before[name]=hashlib.sha256((OUT/name).read_bytes()).hexdigest()
    for index in indices:
     c=cases[index];inp=OUT/'case.bin';inp.write_bytes(fixture(c));expected=None
     for name in ('physical','msvc5','msvc','mingw')+(('matching',) if compiled.get('linked') else ()):
      result=subprocess.run([str(OUT/f'{name}.exe'),str(inp),str(OUT/('matching-image.bin' if name=='matching' else 'physical-image.bin'))],capture_output=True,timeout=10)
      (OUT/f'{name}.bin').write_bytes(result.stdout)
      assert result.returncode==0,(index,name,result.returncode,result.stderr)
      assert len(result.stdout)%(65536+28)==0,(index,name,len(result.stdout))
      if name=='physical':expected=result.stdout
      elif expected!=result.stdout:
       diff=[i for i,(a,b) in enumerate(zip(expected,result.stdout)) if a!=b]
       failure=dict(index=index,case=c,compiler=name,offsets=diff[:50],original=expected[diff[0]&~3:(diff[0]&~3)+4].hex(),native=result.stdout[diff[0]&~3:(diff[0]&~3)+4].hex())
       (OUT/'failure.json').write_text(json.dumps(failure,indent=2));raise AssertionError(failure)
      results.append(dict(index=index,compiler=name,output_sha256=hashlib.sha256(result.stdout).hexdigest()))
     if (index+1)%20==0:print(f'{index+1}/{len(cases)}',flush=True)
    artifact_after={n:hashlib.sha256((OUT/n).read_bytes()).hexdigest() for n in artifact_before};assert artifact_before==artifact_after,'Generated verification artifact changed during proof'
    source_after={p:hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in compiled['sources']};extracted_after=extracted_snapshot();assert extracted_before==extracted_after,'Verified dependency/body changed during proof'
    for p in ('src/carma2/include/rec2_types.h','src/carma2/hooks/c2_hooks.h'):assert source_before[str(Path(p))]==source_after[str(Path(p))]
    assert verifier_before==Path(__file__).read_bytes(),'Verifier changed while running'
    comparisons=len(indices)*(4 if compiled.get('linked') else 3)
    (OUT/'verification.json').write_text(json.dumps(dict(scenes=len(indices),comparisons=comparisons,case_indices=indices,cases=[cases[i] for i in indices],fixture_input_sha256=hashlib.sha256(b''.join(fixture(cases[i]) for i in indices)).hexdigest(),results=results,compiled=compiled,artifact_sha256=artifact_before,artifacts_unchanged=True,source_before=source_before,source_after=source_after,whole_sources_unchanged=source_before==source_after,verified_extractions_unchanged=True,extractions=extracted_before,verifier_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),limits='Physical x86 original at PC53 and actual snapshot native3 plus optional hash-bound linked PE closure. Complete arena/scratch cursor-capacity state and callback/exit order compare. Original/linked allocator switch targets relocated within exact four-index domain. Only CRT exit and supplied PhysicsError callback are controlled boundaries. No public IEEE normalization; callback function pointer is serialized as boolean because code addresses differ. Normal outer calls check ESP, preserved registers and x87 TOP. No FP status/unmasked traps or nonx86 behavior claim. Does not implement PrepareObject/AddObject/follower.'),indent=2))
    print('PASS',len(indices),comparisons)

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output',type=Path,default=ROOT/'build-audit/physics-joint-application-tests')
    parser.add_argument('--candidate-source',type=Path)
    parser.add_argument('--recompiled',type=Path)
    parser.add_argument('--symbols',type=Path)
    parser.add_argument('--case-index',type=int,action='append')
    args=parser.parse_args()
    assert bool(args.recompiled)==bool(args.symbols),'Supply both hash-bound PE and symbol map'
    compile_harness(args.output,args.candidate_source,args.recompiled,args.symbols)
    run_cases(args.output,args.candidate_source,args.case_index)

if __name__=='__main__':main()
