#!/usr/bin/env python3
"""Compare the production inverse/force leaves with the original x86 bodies.

Original and optional matching code execute physically. All 4096 arena bytes
must agree with each native compiler at PC53; no normalization. Only verified
1.0 constants are relocated. Includes finite, singular, raw NaN and partial
alias scenes. This is a leaf proof; the complete force-solver goal continues.
"""
import argparse, ast, hashlib, json, os, random, struct, subprocess
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_MEM,CS_OP_IMM
from verify_tinted_native_reconstruction import extract

ROOT=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',default='build-audit/physics-force-leaves');p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);args=p.parse_args()
if bool(args.recompiled)!=bool(args.symbols):p.error('Supply both immutable PE and symbol map')
OUT=ROOT/args.output;OUT.mkdir(parents=True,exist_ok=True)
src=ROOT/'src/carma2/newcommon/49-physics.c'
source_bytes=src.read_bytes();source_text=source_bytes.decode('utf-8').replace('\r\n','\n')
verifier_bytes=Path(__file__).read_bytes()
original=ROOT/'build-reference/originals/CARMA2_HW.EXE'
image=original.read_bytes();original_hash=hashlib.sha256(image).hexdigest();assert original_hash=='9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
nt=struct.unpack_from('<I',image,60)[0];tab=nt+24+struct.unpack_from('<H',image,nt+20)[0]
def raw(a,n):
    for i in range(struct.unpack_from('<H',image,nt+6)[0]):
        _,va,sz,off=struct.unpack_from('<IIII',image,tab+40*i+8)
        if va<=a-0x400000<va+sz:return image[off+a-0x400000-va:off+a-0x400000-va+n]
spec=[('DRMatrix33Inverse',0x4be350,352),('SetUpForce',0x4be4b0,159)]
cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;entries=[];arrays=[]
for mode,(name,addr,size) in enumerate(spec):
    code=raw(addr,size);ins=list(cs.disasm(code,addr));assert sum(i.size for i in ins)==size
    absolute=[]
    for i in ins:
        assert i.mnemonic!='call'
        if i.mnemonic.startswith('j'):assert addr<=i.operands[0].imm<addr+size
        for op in i.operands:
            if op.type==CS_OP_MEM and not op.mem.base and not op.mem.index:
                assert op.mem.disp==0x58a780 and op.size==8
                absolute.append(i.address-addr+i.disp_offset)
    assert len(absolute)==1 and raw(0x58a780,8)==struct.pack('<d',1.)
    arrays.append('static unsigned char code%d[]={%s};'%(mode,','.join(hex(v) for v in code)))
    entries.append(dict(name=name,address=hex(addr),bytes=size,sha256=hashlib.sha256(code).hexdigest(),constant_operand_offset=absolute[0]))
common=r'''
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "rec2_types.h"
#include "c2_hooks.h"
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison,builder_cdecl;
extern unsigned __cdecl BuilderInvoke(void);
static unsigned h[6];
static unsigned char arena[4096];
typedef void (C2_HOOK_FASTCALL *inverse_fn)(br_scalar*,const br_scalar*);
typedef void (C2_HOOK_FASTCALL *force_fn)(br_vector3*,br_vector3*,const br_vector3*,const br_vector3*,const tPhysics_object*);
'''
call=r'''
static int C2_HOOK_CDECL run(const char* path,inverse_fn inverse,force_fn force){
 FILE*f;f=fopen(path,"rb");if(!f)return 3;
 if(fread(h,sizeof(h),1,f)!=1||fread(arena,sizeof(arena),1,f)!=1)return 4;fclose(f);
 _controlfp(_PC_53,_MCW_PC);_setmode(_fileno(stdout),_O_BINARY);
 builder_mode=2;builder_poison=0xcccccccc;
 builder_address=(unsigned)(uintptr_t)(h[0]==0?(void*)inverse:(void*)force);
 builder_x=(unsigned)(uintptr_t)(arena+h[1]);builder_y=(unsigned)(uintptr_t)(arena+(h[0]==0?h[3]:h[2]));
#if defined(REC2_MATCHING) || defined(PROBE_FASTCALL)
 builder_cdecl=0;builder_stack_count=h[0]==0?0:3;
 builder_stack[0]=(unsigned)(uintptr_t)(arena+h[3]);builder_stack[1]=(unsigned)(uintptr_t)(arena+h[4]);builder_stack[2]=(unsigned)(uintptr_t)(arena+h[5]);
#else
 builder_cdecl=1;builder_stack_count=h[0]==0?2:5;
 builder_stack[0]=builder_x;builder_stack[1]=builder_y;builder_stack[2]=(unsigned)(uintptr_t)(arena+h[3]);builder_stack[3]=(unsigned)(uintptr_t)(arena+h[4]);builder_stack[4]=(unsigned)(uintptr_t)(arena+h[5]);
#endif
 if(BuilderInvoke())return 14;
 fwrite(arena,sizeof(arena),1,stdout);return 0;
}
'''
native='\n'.join(extract(source_text,name) for name,_,_ in spec)
harness=common+native+call+r'''
int C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 2;return run(argv[1],DRMatrix33Inverse,SetUpForce);}
'''
probe='#define PROBE_FASTCALL\n'+common.replace('C2_HOOK_FASTCALL *','__fastcall *')+'\n'.join(arrays)+call+r'''
int C2_HOOK_CDECL main(int argc,char**argv){unsigned char*p;unsigned v;double one=1.;
 if(argc!=2)return 2;p=VirtualAlloc(NULL,4096,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!p)return 3;
 memcpy(p,code0,sizeof(code0));memcpy(p+1024,code1,sizeof(code1));memcpy(p+2048,&one,8);v=(unsigned)(uintptr_t)(p+2048);
'''+f'memcpy(p+{entries[0]["constant_operand_offset"]},&v,4);memcpy(p+1024+{entries[1]["constant_operand_offset"]},&v,4);'+r'''
 return run(argv[1],(inverse_fn)p,(force_fn)(p+1024));}
'''
(OUT/'harness.c').write_text(harness);(OUT/'original.c').write_text(probe)

linked_meta=None
if args.recompiled:
    rebuilt=args.recompiled.read_bytes();mapping=json.loads(args.symbols.read_text())
    assert mapping['format']==1 and mapping['original_sha256']==original_hash and mapping['recompiled_sha256']==hashlib.sha256(rebuilt).hexdigest()
    addresses={x['name']:x['recomp'] for x in mapping['symbols']}
    nt2=struct.unpack_from('<I',rebuilt,60)[0];table2=nt2+24+struct.unpack_from('<H',rebuilt,nt2+20)[0]
    def linked_raw(address,n):
        for si in range(struct.unpack_from('<H',rebuilt,nt2+6)[0]):
            _,rva,length,offset=struct.unpack_from('<IIII',rebuilt,table2+40*si+8)
            if rva<=address-0x400000 and address-0x400000+n<=rva+length:
                return rebuilt[offset+address-0x400000-rva:offset+address-0x400000-rva+n]
        raise ValueError(('unmapped linked bytes',hex(address),n))
    linked_arrays=[];linked_entries=[];patch_code=[]
    for mode,(name,_,_) in enumerate(spec):
        address=addresses[name];queue=[address];instructions={}
        while queue:
            location=queue.pop()
            while location not in instructions:
                assert address<=location<address+4096
                instruction=next(cs.disasm(linked_raw(location,15),location));instructions[location]=instruction
                assert not instruction.mnemonic.startswith('call')
                if instruction.mnemonic.startswith('ret'):break
                if instruction.mnemonic.startswith('j'):
                    assert instruction.operands[0].type==CS_OP_IMM
                    queue.append(instruction.operands[0].imm)
                    if instruction.mnemonic=='jmp':break
                location+=instruction.size
        length=max(i.address+i.size for i in instructions.values())-address
        body=linked_raw(address,length);relocations=[]
        for instruction in instructions.values():
            for operand in instruction.operands:
                if operand.type==CS_OP_MEM and not operand.mem.base and not operand.mem.index:
                    assert operand.size==8 and linked_raw(operand.mem.disp,8)==struct.pack('<d',1.)
                    relocations.append(instruction.address-address+instruction.disp_offset)
        linked_entries.append(dict(name=name,address=address,length=length,sha256=hashlib.sha256(body).hexdigest(),constant_relocations=relocations))
        linked_arrays.append('static unsigned char code%d[]={%s};'%(mode,','.join(hex(v) for v in body)))
        patch_code += ['memcpy(p+%d,&v,4);'%(4096*mode+offset) for offset in relocations]
    linked_main=r"""
int C2_HOOK_CDECL main(int argc,char**argv){unsigned char*p;unsigned v;double one=1.;
 if(argc!=2)return 2;p=VirtualAlloc(NULL,12288,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!p)return 3;
 memcpy(p,code0,sizeof(code0));memcpy(p+4096,code1,sizeof(code1));memcpy(p+8192,&one,8);v=(unsigned)(uintptr_t)(p+8192);(void)v;
"""+''.join(patch_code)+r"""
 return run(argv[1],(inverse_fn)p,(force_fn)(p+4096));}
"""
    (OUT/'matching.c').write_text('#define PROBE_FASTCALL\n'+common.replace('C2_HOOK_FASTCALL *','__fastcall *')+'\n'.join(linked_arrays)+call+linked_main)
    linked_meta=dict(recompiled_sha256=hashlib.sha256(rebuilt).hexdigest(),symbols_sha256=hashlib.sha256(args.symbols.read_bytes()).hexdigest(),entries=linked_entries)

includes=[ROOT/v for v in ('src/brender/include','src/carma2/include','src/carma2/hooks','src/carma2/s3/include')]
mroot=ROOT/'build-tools/msvc-modern';msvc=sorted((mroot/'VC/Tools/MSVC').iterdir())[-1];sdk=mroot/'Windows Kits/10';version=sorted((sdk/'Include').iterdir())[-1].name
env=dict(os.environ);env['PATH']=str(msvc/'bin/Hostx64/x86')+os.pathsep+env['PATH'];env['INCLUDE']=os.pathsep.join(map(str,[msvc/'include',*[sdk/f'Include/{version}/{part}' for part in ('ucrt','shared','um')]]));env['LIB']=os.pathsep.join(map(str,[msvc/'lib/x86',*[sdk/f'Lib/{version}/{part}/x86' for part in ('ucrt','um')]]))
m5=ROOT/'build-tools/msvc500sp3';env5=dict(os.environ);env5['PATH']=str(m5/'bin')+os.pathsep+str(m5/'redist')+os.pathsep+env5['PATH'];env5['INCLUDE']=str(m5/'include');env5['LIB']=str(m5/'lib')
gcc=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'
commands={
 'msvc':[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(q)for q in includes],str(OUT/'harness.c'),'/Fo'+str(OUT/'msvc.obj'),'/Fe'+str(OUT/'msvc.exe')],
 'msvc5':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(q)for q in includes],str(OUT/'harness.c'),'/Fo'+str(OUT/'msvc5.obj'),'/Fe'+str(OUT/'msvc5.exe')],
 'mingw':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation',*['-I'+str(q)for q in includes],str(OUT/'harness.c'),'-o',str(OUT/'mingw.exe')],
 'original':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation',*['-I'+str(q)for q in includes],str(OUT/'original.c'),'-o',str(OUT/'original.exe')]}
if args.recompiled:commands['matching']=[*commands['original'][:-3],str(OUT/'matching.c'),'-o',str(OUT/'matching.exe')]

# Use the retained ABI checker template without substituting leaf semantics.
caller_tree=ast.parse((ROOT/'scripts/verify_opponent_navigation_reconstruction.py').read_text())
caller_nodes=[node.value.value for node in ast.walk(caller_tree) if isinstance(node,ast.Assign) and isinstance(node.value,ast.Constant) and any(isinstance(target,ast.Name) and target.id=='caller' for target in node.targets)]
assert len(caller_nodes)==1
caller=caller_nodes[0].replace('builder_poison;','builder_poison,builder_cdecl;')
caller=caller.replace(' mov builder_result,eax\n mov ecx,esp',' mov builder_result,eax\n cmp builder_cdecl,0\n je stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n stackready:\n mov ecx,esp')
(OUT/'caller.c').write_text(caller)
for command in commands.values():command.append(str(OUT/'caller.obj'))
commands={'caller':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Zl','/c',str(OUT/'caller.c'),'/Fo'+str(OUT/'caller.obj')],**commands}

for name,cmd in commands.items():
    r=subprocess.run(cmd,cwd=OUT,env=env if name=='msvc'else env5 if name in ('msvc5','caller')else None,capture_output=True,text=True);(OUT/(name+'-build.txt')).write_text(r.stdout+r.stderr)
    if r.returncode:raise RuntimeError(r.stdout+r.stderr)
rng=random.Random(0x4be350);cases=[]
def case(mode,label,offsets=None,patch=None,flag=0):
    h=[mode,0,64,256,320,1024]
    if offsets:
        for k,v in offsets.items():h[k]=v
    arena=bytearray(b'\xa5'*4096)
    def flt(at,values):arena[at:at+len(values)*4]=struct.pack('<'+'f'*len(values),*values)
    flt(256,[1.125,-.375,.0625,.4375,1.75,-.25,-.625,.15625,.75])
    if mode:flt(256,[1.25,-2.5,3.125])
    flt(320,[2.375,-4.125,8.0625]);flt(1028,[2.375,1.25,4.75,8.0625]);arena[1024+0x19c:1024+0x1a0]=struct.pack('<I',flag)
    for at,word in patch or []:arena[at:at+4]=struct.pack('<I',word&0xffffffff)
    cases.append(dict(label=label,header=h,data=bytes(arena)))
special=[0,0x80000000,1,0x80000001,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7f800001,0xff800001,0x7fc00011,0xffc00011,0x7fc12345,0xffc12345]
for mode in (0,1):
    case(mode,'finite-general')
    slots=[256+4*i for i in range(9)] if mode==0 else [256+4*i for i in range(3)]+[320+4*i for i in range(3)]+[1028+4*i for i in range(4)]
    for i in range(64):case(mode,f'finite-{i}',patch=[(s,struct.unpack('<I',struct.pack('<f',rng.uniform(-16,16)))[0]) for s in slots])
    for at in (252,256,260,255,257):case(mode,f'alias-out-input-{at}',offsets={1:at})
    if mode:
        for at in (0,4,252,256,260,316,320,324,1028,1032):
            case(mode,f'alias-second-{at}',offsets={2:at})
            case(mode,f'alias-first-{at}',offsets={1:at})
        for flag in (0x800,0xffffffff,0x7ff):case(mode,f'flags-{flag}',flag=flag)
    for i,s in enumerate(slots):
        for word in special:case(mode,f'special-{i}-{word:08x}',patch=[(s,word)])
    for left in range(len(slots)):
        for right in range(left+1,len(slots)):
            case(mode,f'NaN-pair-{left}-{right}',patch=[(slots[left],0x7fc00011),(slots[right],0xffc00011)])
    for i in range(32):case(mode,f'raw-mix-{i}',patch=[(s,rng.choice(special)) for s in slots])
results=[];fails=[]
for index,c in enumerate(cases):
    f=OUT/'input.bin';f.write_bytes(struct.pack('<6I',*c['header'])+c['data'])
    oracle=subprocess.run([str(OUT/'original.exe'),str(f)],capture_output=True);assert oracle.returncode==0 and len(oracle.stdout)==4096
    for compiler in ['msvc5','msvc','mingw']+(['matching'] if args.recompiled else []):
        r=subprocess.run([str(OUT/(compiler+'.exe')),str(f)],capture_output=True)
        ok=r.returncode==0 and r.stdout==oracle.stdout
        row=dict(index=index,mode=c['header'][0],label=c['label'],compiler=compiler,passed=ok,returncode=r.returncode)
        if not ok:
            row['differences']=[dict(offset=i,original=oracle.stdout[i:i+4].hex(),native=r.stdout[i:i+4].hex())for i in range(0,min(len(r.stdout),4096),4)if r.stdout[i:i+4]!=oracle.stdout[i:i+4]]
            (OUT/f'failure-{index}-{compiler}.bin').write_bytes(f.read_bytes());fails.append(row)
        results.append(row)
    if index%100==0:print('cases',index+1,'failures',len(fails),flush=True)
    if len(fails)>=20:break
assert all(extract(src.read_text(),name)==extract(source_text,name) for name,_,_ in spec),'Verified production bodies changed during proof'
result=dict(source_sha256=hashlib.sha256(source_bytes).hexdigest(),current_source_sha256=hashlib.sha256(src.read_bytes()).hexdigest(),verified_bodies_unchanged=True,extracted_bodies_sha256=hashlib.sha256(native.encode()).hexdigest(),verifier_sha256=hashlib.sha256(verifier_bytes).hexdigest(),fixture_input_sha256=hashlib.sha256(b''.join(struct.pack('<6I',*c['header'])+c['data'] for c in cases)).hexdigest(),original_sha256=original_hash,entries=entries,total_scenarios=len(cases),executed_scenarios=index+1,comparisons=len(results),failures=fails,results=results,compiler_commands=commands,scope='Physical original instruction bytes with only original1.0 constant relocated; native MSVC5/modern/MinGW full4096-byte arena at original CRTPC53. Masked FP exceptions/default rounding. Every normal return checks ESP, EBX/ESI/EDI/EBP and x87 TOP through the retained ABI caller; full integration remains separate.')
result['matching_physical']=linked_meta
result['generated_files_sha256']={p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in OUT.iterdir() if p.suffix in ('.c','.obj','.exe')}
(OUT/'verification.json').write_text(json.dumps(result,indent=2)+'\n');print('complete',index+1,len(results),'failures',len(fails))
if fails:raise SystemExit(1)
