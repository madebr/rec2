#!/usr/bin/env python3
"""Verify actual bounds geometry at PC53 against original and native bodies.

Requires Unicorn2.1.4, reccmp0.1.7 for export-map, scoped MSVC5SP3/modern x86
MSVC/i686 MinGW. Hash-checked original machine code also executes on physical
x87 in an isolated probe. It only loads the reference image as data and calls
the three allocation-free geometry entries plus their matrix apply dependency.
No game entry point, imports, installers or system installation are executed.
All data and matrix-call boundaries are compared without output normalization
other than translating fixture pointers to their fixed virtual addresses.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import random
import struct
import subprocess
import sys

from verify_finteray_reconstruction import ROOT, ORIGINAL_SHA256, words, bits

ADDRESSES=dict(GetNewBoundingBox=0x45f5b0,FillInBounds=0x45f690,
    BoundsOverlapTest__finteray=0x425ba0,BrMatrix34ApplyP=0x5334a0)
HEAP,STACK,STOP,SIZE=0x1000000,0x1080000,0x1081000,0x200

def make_case(options):
    data=bytearray(b'\xa5'*SIZE)
    def floats(off,values):data[off:off+4*len(values)]=words([bits(v) for v in values])
    floats(0,options.get('bounds',[-1,-2,-3,4,5,6]))
    floats(0x20,options.get('other',[-1,-2,-3,4,5,6]))
    floats(0x80,options.get('matrix',[1,0,0,0,1,0,0,0,1,2,-4,8]))
    data[0xc0:0xc4]=words([HEAP+options.get('matrix_offset',0x80)])
    data[0xc4:0xdc]=data[:24]
    for off,value in options.get('patch',[]):data[off:off+4]=words([value])
    return dict(options=options,data=bytes(data),header=[options.get('mode',0),options.get('out',0x40),options.get('input',0),options.get('matrix_offset',0x80)])

class Fixture:
    def __init__(self,image,addresses,case):
        from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
        from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_EAX,UC_X86_REG_ECX,UC_X86_REG_EDX,UC_X86_REG_FPCW
        self.regs=UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_EAX,UC_X86_REG_ECX,UC_X86_REG_EDX
        self.u=Uc(UC_ARCH_X86,UC_MODE_32);self.addresses=addresses;self.case=case;self.events=bytearray();self.pending=[]
        pe=struct.unpack_from('<I',image,0x3c)[0];table=pe+24+struct.unpack_from('<H',image,pe+20)[0]
        base=struct.unpack_from('<I',image,pe+52)[0];size=struct.unpack_from('<I',image,pe+80)[0]
        self.u.mem_map(base,(size+4095)&~4095);self.u.mem_map(HEAP,0x100000)
        for i in range(struct.unpack_from('<H',image,pe+6)[0]):
            _,va,n,raw=struct.unpack_from('<IIII',image,table+40*i+8)
            if n:self.u.mem_write(base+va,image[raw:raw+n])
        self.u.mem_write(HEAP,case['data']);self.u.reg_write(UC_X86_REG_FPCW,0x27f)
        self.u.hook_add(UC_HOOK_CODE,self.hook)
    def read(self,p):return struct.unpack('<I',self.u.mem_read(p,4))[0]
    def record(self,phase,args):
        a,b,c=args
        self.events+=words([phase,a if HEAP<=a<HEAP+SIZE else 0])+bytes(self.u.mem_read(b,12))+bytes(self.u.mem_read(c,48))+bytes(self.u.mem_read(a,12))+bytes(self.u.mem_read(HEAP,SIZE))
    def hook(self,u,address,size,context):
        esp=self.regs[0]
        if self.pending and address==self.pending[-1][0]:
            _,args=self.pending.pop();self.record(2,args)
        if address==self.addresses['BrMatrix34ApplyP']:
            sp=u.reg_read(esp);args=[self.read(sp+4+4*i) for i in range(3)]
            self.record(1,args);self.pending.append((self.read(sp),args))
    def execute(self):
        esp,eip,eax,ecx,edx=self.regs;m,out,inp,mat=self.case['header']
        self.u.mem_write(STACK,words([STOP,HEAP+mat]));self.u.reg_write(esp,STACK)
        if m==0:name='GetNewBoundingBox';first=[HEAP+out,HEAP+inp]
        elif m==1:name='FillInBounds';first=[HEAP+0xc0,0]
        elif m==2:name='BoundsOverlapTest__finteray';first=[HEAP+inp,HEAP+0x20]
        else:
            name='BrMatrix34ApplyP';first=[0,0]
            self.u.mem_write(STACK,words([STOP,HEAP+out,HEAP+inp,HEAP+mat]))
        self.u.reg_write(ecx,first[0]);self.u.reg_write(edx,first[1]);self.u.emu_start(self.addresses[name],STOP,count=100000)
        if self.pending and self.pending[-1][0]==STOP:
            _,args=self.pending.pop();self.record(2,args)
        if self.u.reg_read(eip)!=STOP or self.pending:raise AssertionError('Incomplete execution')
        self.events+=words([0,self.u.reg_read(eax) if m==2 else 0])+bytes(self.u.mem_read(HEAP,SIZE))
        return bytes(self.events)

def original_relocations(image):
    """Prove exact operand relocations and call boundaries in the scoped probe."""
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    from capstone.x86 import X86_OP_MEM
    pe=struct.unpack_from('<I',image,0x3c)[0];table=pe+24+struct.unpack_from('<H',image,pe+20)[0]
    def read(address,size):
        for i in range(struct.unpack_from('<H',image,pe+6)[0]):
            _,va,n,raw=struct.unpack_from('<IIII',image,table+40*i+8)
            if va<=address-0x400000<va+n:return image[raw+address-0x400000-va:raw+address-0x400000-va+size]
        raise AssertionError('Unmapped original code')
    sizes=dict(GetNewBoundingBox=0xd5,FillInBounds=0x228,BoundsOverlapTest__finteray=0xfa,BrMatrix34ApplyP=0x80)
    md=Cs(CS_ARCH_X86,CS_MODE_32);md.detail=True;relocations=[];calls=[];hashes={}
    for name,address in ADDRESSES.items():
        code=read(address,sizes[name]);hashes[name]=hashlib.sha256(code).hexdigest()
        for inst in md.disasm(code,address):
            for op in inst.operands:
                if op.type==X86_OP_MEM and not op.mem.base and not op.mem.index and op.mem.disp:
                    if op.mem.disp not in [0x589fac,0x589fd4]:raise AssertionError(f'Unexpected absolute operand: {inst}')
                    if inst.disp_size!=4:raise AssertionError('Unexpected displacement size')
                    relocations.append(inst.address-0x400000+inst.disp_offset)
            if inst.mnemonic=='call':
                if inst.op_str!='0x5334a0':raise AssertionError('Unapproved original call')
                calls.append(inst.address-0x400000)
    if len(calls)!=3:raise AssertionError('Wrong matrix call count')
    return dict(relocations=relocations,calls=calls,code_sha256=hashes)

def compile_native(output,args,original):
    from verify_tinted_native_reconstruction import extract
    output=output.resolve();output.mkdir(parents=True,exist_ok=True)
    source=(ROOT/'src/carma2/newcommon/34-finteray.c').read_text()
    body=[extract(source,'FinterayScalar')]+[extract(source,n) for n in list(ADDRESSES)[:3]]
    matrix=extract((ROOT/'src/brender/core/math/matrix34.c').read_text(),'BrMatrix34ApplyP').replace('BrMatrix34ApplyP','BoundsApplyBody')
    provenance=original_relocations(original)
    prefix=r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <fcntl.h>
#include <io.h>
#include "34-finteray.h"
#include <brender/brender.h>
#include "c2_math.h"
#include "c2_string.h"
typedef char layout[sizeof(void*)==4 && sizeof(tBounds)==68 && sizeof(br_matrix34)==48 && sizeof(br_bounds)==24?1:-1];
static unsigned arena[0x200/4],header[4];
static unsigned C2_HOOK_CDECL norm(unsigned p){unsigned a=(unsigned)(uintptr_t)arena;return p>=a&&p<a+sizeof(arena)?p-a+0x1000000:p;}
static void C2_HOOK_CDECL emit(unsigned v){fwrite(&v,4,1,stdout);}
static void C2_HOOK_CDECL snapshot(void){unsigned char copy[sizeof(arena)];unsigned value;memcpy(copy,arena,sizeof(copy));memcpy(&value,copy+0xc0,4);value=norm(value);memcpy(copy+0xc0,&value,4);fwrite(copy,sizeof(copy),1,stdout);}
static void C2_HOOK_CDECL record(unsigned phase,br_vector3* a,const br_vector3* b,const br_matrix34* c){unsigned p=norm((unsigned)(uintptr_t)a);emit(phase);emit(p>=0x1000000&&p<0x1000200?p:0);fwrite(b,12,1,stdout);fwrite(c,48,1,stdout);fwrite(a,12,1,stdout);snapshot();}
#ifdef ORIGINAL_PROBE
#include <windows.h>
static unsigned char* image;
static void (__cdecl* BoundsApplyBody)(br_vector3*,const br_vector3*,const br_matrix34*);
static void (__fastcall* OriginalNewBox)(br_bounds*,br_bounds*,br_matrix34*);
static void (__fastcall* OriginalFill)(tBounds*);
static int (__fastcall* OriginalOverlap)(br_bounds*,br_bounds*);
#define GetNewBoundingBox OriginalNewBox
#define FillInBounds OriginalFill
#define BoundsOverlapTest__finteray OriginalOverlap
#else
#define C(x,y) C->m[x][y]
#undef BR_MAC3
#undef BR_MAC4
#define BR_MAC3(a,b,c,d,e,f) ((a)*(b)+(c)*(d)+(e)*(f))
#define BR_MAC4(a,b,c,d,e,f,g,h) ((a)*(b)+(c)*(d)+(e)*(f)+(g)*(h))
'''
    wrapper='''#endif
void C2_HOOK_CDECL BrMatrix34ApplyP(br_vector3* a,const br_vector3* b,const br_matrix34* c) {
    record(1,a,b,c);BoundsApplyBody(a,b,c);record(2,a,b,c);
}
'''
    probe=r'''
#ifdef ORIGINAL_PROBE
static int C2_HOOK_CDECL load_original(const char* filename){FILE* f;unsigned char* disk;unsigned size,i,value,delta;IMAGE_NT_HEADERS32* nt;IMAGE_SECTION_HEADER* sections;
    f=fopen(filename,"rb");if(!f)return 3;fseek(f,0,SEEK_END);size=(unsigned)ftell(f);rewind(f);disk=(unsigned char*)malloc(size);
    if(!disk || fread(disk,1,size,f)!=size)return 4;fclose(f);nt=(IMAGE_NT_HEADERS32*)(disk+((IMAGE_DOS_HEADER*)disk)->e_lfanew);
    image=(unsigned char*)VirtualAlloc(NULL,nt->OptionalHeader.SizeOfImage,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image)return 5;
    sections=IMAGE_FIRST_SECTION(nt);for(i=0;i<nt->FileHeader.NumberOfSections;i++)memcpy(image+sections[i].VirtualAddress,disk+sections[i].PointerToRawData,sections[i].SizeOfRawData);
    delta=(unsigned)(uintptr_t)image-0x400000;
    for(i=0;i<sizeof(relocations)/sizeof(relocations[0]);i++){memcpy(&value,image+relocations[i],4);value+=delta;memcpy(image+relocations[i],&value,4);}
    for(i=0;i<sizeof(calls)/sizeof(calls[0]);i++){value=(unsigned)(uintptr_t)BrMatrix34ApplyP-(unsigned)(uintptr_t)(image+calls[i]+5);memcpy(image+calls[i]+1,&value,4);}
    BoundsApplyBody=(void(__cdecl*)(br_vector3*,const br_vector3*,const br_matrix34*))(image+0x1334a0);
    OriginalNewBox=(void(__fastcall*)(br_bounds*,br_bounds*,br_matrix34*))(image+0x5f5b0);
    OriginalFill=(void(__fastcall*)(tBounds*))(image+0x5f690);OriginalOverlap=(int(__fastcall*)(br_bounds*,br_bounds*))(image+0x25ba0);
    free(disk);return 0;
}
#endif
'''
    driver=r'''
int C2_HOOK_CDECL main(int argc,char** argv){FILE* f;unsigned pointer_;int r=0;char* a=(char*)arena;
#ifdef ORIGINAL_PROBE
    if(argc!=3)return 1;
    r=load_original(argv[2]);if(r)return r;
#else
    if(argc!=2)return 1;
#endif
    f=fopen(argv[1],"rb");if(!f)return 2;
    if(fread(header,sizeof(header),1,f)!=1 || fread(arena,sizeof(arena),1,f)!=1)return 3;
    fclose(f);_setmode(_fileno(stdout),_O_BINARY);_controlfp(_PC_53,_MCW_PC);
    memcpy(&pointer_,a+0xc0,4);pointer_=pointer_-0x1000000+(unsigned)(uintptr_t)arena;memcpy(a+0xc0,&pointer_,4);
    if(header[0]==0)GetNewBoundingBox((br_bounds*)(a+header[1]),(br_bounds*)(a+header[2]),(br_matrix34*)(a+header[3]));
    else if(header[0]==1)FillInBounds((tBounds*)(a+0xc0));
    else if(header[0]==2)r=BoundsOverlapTest__finteray((br_bounds*)(a+header[2]),(br_bounds*)(a+0x20));
    else BrMatrix34ApplyP((br_vector3*)(a+header[1]),(br_vector3*)(a+header[2]),(br_matrix34*)(a+header[3]));
    emit(0);emit((unsigned)r);snapshot();
#ifdef ORIGINAL_PROBE
    VirtualFree(image,0,MEM_RELEASE);
#endif
    return 0;
}
'''
    declarations='static unsigned relocations[]={'+','.join(map(str,provenance['relocations']))+'};\nstatic unsigned calls[]={'+','.join(map(str,provenance['calls']))+'};\n'
    harness=output/'harness.c';harness.write_text(prefix+matrix+'\n'+'\n'.join(body)+'\n'+wrapper+'#ifdef ORIGINAL_PROBE\n'+declarations+'#endif\n'+probe+driver)
    includes=[ROOT/p for p in ['src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/carma2/platform','src/carma2/s3/include','src/brender/include']]
    msvc_root=args.msvc_root.resolve();msvc=sorted((msvc_root/'VC/Tools/MSVC').iterdir())[-1];sdk=msvc_root/'Windows Kits/10';version=sorted(p.name for p in (sdk/'Include').iterdir())[-1]
    env=dict(os.environ);env['PATH']=str(msvc/'bin/Hostx64/x86')+os.pathsep+env['PATH'];env['INCLUDE']=os.pathsep.join(map(str,[msvc/'include',*[sdk/f'Include/{version}/{p}' for p in ['ucrt','shared','um']]]));env['LIB']=os.pathsep.join(map(str,[msvc/'lib/x86',*[sdk/f'Lib/{version}/{p}/x86' for p in ['ucrt','um']]]))
    msvc5=args.msvc5_root.resolve();old_env=dict(os.environ);old_env['PATH']=str(msvc5/'bin')+os.pathsep+str(msvc5/'redist')+os.pathsep+old_env['PATH'];old_env['INCLUDE']=str(msvc5/'include');old_env['LIB']=str(msvc5/'lib')
    commands=dict(msvc=[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(p) for p in includes],str(harness),'/Fo'+str(output/'msvc.obj'),'/Fe'+str(output/'msvc.exe')],
        msvc5=[str(msvc5/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(p) for p in includes],str(harness),'/Fo'+str(output/'msvc5.obj'),'/Fe'+str(output/'msvc5.exe')],
        mingw=[str(args.gcc.resolve()),'-std=gnu89','-O2','-Wall','-Wextra','-Werror',*['-I'+str(p) for p in includes],str(harness),'-o',str(output/'mingw.exe')])
    commands['original-x87']=[*commands['msvc'][:-2],'/DORIGINAL_PROBE','/Fo'+str(output/'original-x87.obj'),'/Fe'+str(output/'original-x87.exe'),'/link','kernel32.lib']
    for compiler,command in commands.items():
        result=subprocess.run(command,cwd=output,env=old_env if compiler=='msvc5' else env if compiler in ['msvc','original-x87'] else None,capture_output=True,text=True)
        (output/(compiler+'-build.log')).write_text(result.stdout+result.stderr)
        if result.returncode:raise AssertionError(result.stdout+result.stderr)
    return commands,provenance

def case_options():
    result=[]
    for mode in range(3):
        for b in [[-1,-2,-3,4,5,6],[0]*6,[1,2,3,-4,-5,-6],[-0.0]*6,[1e20,-1e20,1e-20,2e20,1e20,2e-20]]:result.append(dict(mode=mode,bounds=b))
    matrices=[[1,0,0,0,1,0,0,0,1,0,0,0],[-1,0,0,0,-1,0,0,0,-1,1,2,3],[0,1,0,-1,0,0,0,0,1,-4,8,16],[1,.5,-.25,.125,2,.5,-.5,.25,.5,1,-2,3],[0]*12]
    for mode in [0,1]:
        for matrix in matrices:result.append(dict(mode=mode,matrix=matrix))
    for out in [0,4,12,0x20]:result.append(dict(mode=0,out=out))
    for axis in range(3):
        for v in [-1,-1.000001,4,4.000001]:
            b=[-1,-2,-3,4,5,6];b[axis]=v;b[axis+3]=v;result.append(dict(mode=2,other=b))
    rng=random.Random(0x45f690)
    for i in range(120):
        b=[rng.uniform(-1e3,1e3) for j in range(6)]
        for mode in [0,1,2]:result.append(dict(mode=mode,bounds=b,matrix=matrices[i%len(matrices)]))
    specials=[0,0x80000000,1,0x80000001,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc12345,0xffc23456,0x7f800001,0xff801234]
    for mode in range(3):
        offsets=[4*i for i in range(6)] if mode==0 else [0xc4+4*i for i in range(6)] if mode==1 else [4*i for i in range(6)]+[0x20+4*i for i in range(6)]
        for offset in offsets:
            for value in specials:result.append(dict(mode=mode,patch=[(offset,value)]))
    for matrix in matrices:
        for out in [0,4,8,12,0x80,0x84,0x88,0x8c]:result.append(dict(mode=3,out=out,matrix=matrix))
    for i in range(80):result.append(dict(mode=3,bounds=[rng.uniform(-1e5,1e5)for j in range(6)],matrix=[rng.uniform(-16,16)for j in range(12)]))
    return result

def main():
    parser=argparse.ArgumentParser(description=__doc__);sub=parser.add_subparsers(dest='command',required=True)
    export=sub.add_parser('export-map');export.add_argument('--build-directory',type=Path,required=True);export.add_argument('--output',type=Path,required=True)
    verify=sub.add_parser('verify')
    for name in ['original','recompiled','symbols','output']:verify.add_argument('--'+name,type=Path,required=True)
    verify.add_argument('--gcc',type=Path,default=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe');verify.add_argument('--msvc-root',type=Path,default=ROOT/'build-tools/msvc-modern');verify.add_argument('--msvc5-root',type=Path,default=ROOT/'build-tools/msvc500sp3')
    args=parser.parse_args()
    if args.command=='export-map':
        from reccmp.project.detect import argparse_parse_project_target
        from reccmp.compare import Compare
        output=args.output.resolve();os.chdir(args.build_directory);target=argparse_parse_project_target(argparse.Namespace(target='CARMA2_HW'));original=target.original_path.read_bytes()
        if hashlib.sha256(original).hexdigest()!=ORIGINAL_SHA256:parser.error('Incorrect original hash')
        compare=Compare.from_target(target);mapping=dict(format=1,original_sha256=ORIGINAL_SHA256,recompiled_sha256=hashlib.sha256(target.recompiled_path.read_bytes()).hexdigest(),symbols=[dict(name=e.name,orig=e.orig_addr,recomp=e.recomp_addr)for e in compare.get_all()if e.orig_addr and e.recomp_addr]);output.parent.mkdir(parents=True,exist_ok=True);output.write_text(json.dumps(mapping,indent=2));print('Exported symbol map');return
    original=args.original.read_bytes();rebuilt=args.recompiled.read_bytes();mapping=json.loads(args.symbols.read_text())
    if hashlib.sha256(original).hexdigest()!=ORIGINAL_SHA256:parser.error('Incorrect original hash')
    if mapping.get('format')!=1 or mapping.get('original_sha256')!=ORIGINAL_SHA256 or mapping.get('recompiled_sha256')!=hashlib.sha256(rebuilt).hexdigest():parser.error('Stale/invalid symbol map')
    symbols={s['name']:s for s in mapping['symbols']}
    for name,address in ADDRESSES.items():
        if symbols[name]['orig']!=address:parser.error('Unexpected original address:'+name)
    addresses={n:symbols[n]['recomp'] for n in ADDRESSES};output=args.output.resolve();output.mkdir(parents=True,exist_ok=True)
    commands,provenance=compile_native(output,args,original);cases=case_options();results=[];emulation_gaps=[]
    for index,options in enumerate(cases):
        case=make_case(options);path=output/'case.bin';path.write_bytes(words(case['header'])+case['data'])
        expected=subprocess.check_output([str(output/'original-x87.exe'),str(path),str(args.original.resolve())])
        emulated=Fixture(original,ADDRESSES,case).execute()
        if emulated!=expected:emulation_gaps.append(dict(case=index,options=options,physical_sha256=hashlib.sha256(expected).hexdigest(),unicorn_sha256=hashlib.sha256(emulated).hexdigest()))
        checks=[('recompiled-x86',Fixture(rebuilt,addresses,case).execute())]
        for compiler in ['msvc5','msvc','mingw']:checks.append((compiler,subprocess.check_output([str(output/(compiler+'.exe')),str(path)])))
        for compiler,actual in checks:
            if actual!=expected:
                (output/f'failure-{index}-{compiler}.json').write_text(json.dumps(dict(options=options,original=expected.hex(),actual=actual.hex(),unicorn=emulated.hex()),indent=2));raise AssertionError(f'Case{index} {options} {compiler} differs; see {output}')
            results.append(dict(case=options,compiler=compiler,events_sha256=hashlib.sha256(actual).hexdigest()))
    (output/'verification.json').write_text(json.dumps(dict(original_sha256=ORIGINAL_SHA256,recompiled_sha256=hashlib.sha256(rebuilt).hexdigest(),source_sha256=hashlib.sha256((ROOT/'src/carma2/newcommon/34-finteray.c').read_bytes()).hexdigest(),compiler_commands=commands,physical_original_provenance=provenance,unicorn_gaps=emulation_gaps,cases=results),indent=2))
    print(f'PASS: {len(cases)} bounds scenarios, {len(results)} original physical/native/rebuilt comparisons; {len(emulation_gaps)} documented Unicorn differences')

if __name__=='__main__':main()
