#!/usr/bin/env python3
"""Compare single/multiple face rays with physical original x87 and native code.

Requires Unicorn2.1.4, reccmp0.1.7 for export-map, scoped MSVC5SP3/modern x86
MSVC/i686 MinGW. Only the two original allocation-free face entries execute
from the reference PE loaded as data, with their verified absolute data operands
relocated. No game entry, import, loader or installer executes. Public outputs
and all fixture memory are compared bit-for-bit, including stored NaN bits.
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

ADDRESSES=dict(CheckSingleFace=0x45ecc0,MultiRayCheckSingleFace=0x45f0d0,gPling_materials=0x5964c0)
HEAP,STACK,STOP,SIZE=0x1000000,0x1080000,0x1081000,0x1000

def make_case(options):
    data=bytearray(b'\xa5'*SIZE)
    def floats(off,values):data[off:off+4*len(values)]=words([bits(v) for v in values])
    def wr(off,value):data[off:off+4]=words([value])
    data[:80]=bytes(80);wr(0,0 if options.get('null_material') else HEAP+0x100)
    axis=options.get('axis',2);other=[k for k in range(3) if k!=axis]
    for i,xy in enumerate(options.get('vertices',[[0,0],[1,0],[0,1]])):
        point=[0,0,0];point[other[0]],point[other[1]]=xy;point[axis]=options.get('plane',0)
        if 'vertices3d' in options:point=options['vertices3d'][i]
        floats(4+12*i,point)
    normal=options.get('normal',[1 if k==axis else 0 for k in range(3)]);floats(52,normal)
    data[0x100:0x19c]=bytes(156);wr(0x104,0 if options.get('null_identifier') else HEAP+0x200);wr(0x120,options.get('flags',0x1800))
    text=options.get('identifier','normal').encode()+b'\0';data[0x200:0x200+len(text)]=text
    for i in range(4):
        point=[0,0,0];point[axis]=1;point[other[0]],point[other[1]]=[.25,.25]
        floats(0x300+12*i,options.get('positions',[point]*4)[i])
    floats(0x400,options.get('direction',[-2 if k==axis else 0 for k in range(3)]))
    for off,value in options.get('patch',[]):wr(off,value)
    header=[options.get('mode',0),options.get('count',4),options.get('position_offset',0x300),options.get('direction_offset',0x400),options.get('normal_offset',0x500),options.get('t_offset',0x540),options.get('point_offset',0x580),options.get('pling',1)]
    return dict(options=options,data=bytes(data),header=header)

class Fixture:
    def __init__(self,image,addresses,case):
        from unicorn import Uc,UC_ARCH_X86,UC_MODE_32
        from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_ECX,UC_X86_REG_EDX,UC_X86_REG_FPCW
        self.regs=UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_ECX,UC_X86_REG_EDX
        self.u=Uc(UC_ARCH_X86,UC_MODE_32);self.addresses=addresses;self.case=case
        pe=struct.unpack_from('<I',image,0x3c)[0];table=pe+24+struct.unpack_from('<H',image,pe+20)[0]
        base=struct.unpack_from('<I',image,pe+52)[0];size=struct.unpack_from('<I',image,pe+80)[0]
        self.u.mem_map(base,(size+4095)&~4095);self.u.mem_map(HEAP,0x100000)
        for i in range(struct.unpack_from('<H',image,pe+6)[0]):
            _,va,n,raw=struct.unpack_from('<IIII',image,table+40*i+8)
            if n:self.u.mem_write(base+va,image[raw:raw+n])
        self.u.mem_write(HEAP,case['data']);self.u.mem_write(addresses['gPling_materials'],words([case['header'][7]]));self.u.reg_write(UC_X86_REG_FPCW,0x27f)
    def execute(self):
        esp,eip,ecx,edx=self.regs;mode,count,rp,rd,nor,t,point,_=self.case['header']
        if mode==0:name='CheckSingleFace';first=[HEAP,HEAP+rp];rest=[HEAP+rd,HEAP+nor,HEAP+t,HEAP+point]
        else:name='MultiRayCheckSingleFace';first=[count,HEAP];rest=[HEAP+rp,HEAP+rd,HEAP+nor,HEAP+t]
        self.u.mem_write(STACK,words([STOP,*rest]));self.u.reg_write(esp,STACK);self.u.reg_write(ecx,first[0]&0xffffffff);self.u.reg_write(edx,first[1])
        self.u.emu_start(self.addresses[name],STOP,count=100000)
        if self.u.reg_read(eip)!=STOP:raise AssertionError('Incomplete execution')
        return bytes(self.u.mem_read(self.addresses['gPling_materials'],4))+bytes(self.u.mem_read(HEAP,SIZE))

def original_relocations(image):
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32
    from capstone.x86 import X86_OP_MEM
    pe=struct.unpack_from('<I',image,0x3c)[0];table=pe+24+struct.unpack_from('<H',image,pe+20)[0]
    def read(address,size):
        for i in range(struct.unpack_from('<H',image,pe+6)[0]):
            _,va,n,raw=struct.unpack_from('<IIII',image,table+40*i+8)
            if va<=address-0x400000<va+n:return image[raw+address-0x400000-va:raw+address-0x400000-va+size]
        raise AssertionError('Unmapped original code')
    md=Cs(CS_ARCH_X86,CS_MODE_32);md.detail=True;relocations=[];hashes={}
    approved=[0x5964c0,0x589fac,0x589fc4,0x589fb0,0x589fbc,0x58a010,0x58a018,0x58a020,0x58a028,0x58a030]
    for name,size in [('CheckSingleFace',0x40b),('MultiRayCheckSingleFace',0x4dd)]:
        address=ADDRESSES[name];code=read(address,size);hashes[name]=hashlib.sha256(code).hexdigest()
        for inst in md.disasm(code,address):
            if inst.mnemonic=='call':raise AssertionError('Unapproved original call')
            for op in inst.operands:
                if op.type==X86_OP_MEM and not op.mem.base and not op.mem.index and op.mem.disp:
                    if op.mem.disp not in approved or inst.disp_size!=4:raise AssertionError(f'Unexpected absolute operand: {inst}')
                    relocations.append(inst.address-0x400000+inst.disp_offset)
    return dict(relocations=relocations,code_sha256=hashes)

def compile_native(output,args,original):
    from verify_tinted_native_reconstruction import extract
    output=output.resolve();output.mkdir(parents=True,exist_ok=True)
    source=(ROOT/'src/carma2/newcommon/34-finteray.c').read_text()
    end=source.index('} tFinteray_projection;')+len('} tFinteray_projection;');start=source.rfind('typedef struct {',0,end)
    projection=source[start:end]
    bodies=[extract(source,n) for n in ['FinterayScalar','FinterayFaceProjection','FinterayFacePoint','CheckSingleFace','MultiRayCheckSingleFace']]
    provenance=original_relocations(original)
    prefix=r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <fcntl.h>
#include <io.h>
#include "34-finteray.h"
#include "c2_math.h"
typedef char layout[sizeof(void*)==4 && sizeof(tFace_ref)==80 && sizeof(br_material)==156?1:-1];
static unsigned arena[0x1000/4],header[8];int gPling_materials;
static unsigned C2_HOOK_CDECL norm(unsigned p){unsigned a=(unsigned)(uintptr_t)arena;return p>=a&&p<a+sizeof(arena)?p-a+0x1000000:p;}
static void C2_HOOK_CDECL snapshot(void){unsigned char copy[sizeof(arena)];unsigned value,i;unsigned slots[]={0,0x104};fwrite(&gPling_materials,4,1,stdout);memcpy(copy,arena,sizeof(copy));for(i=0;i<2;i++){memcpy(&value,copy+slots[i],4);value=norm(value);memcpy(copy+slots[i],&value,4);}fwrite(copy,sizeof(copy),1,stdout);}
#ifdef ORIGINAL_PROBE
#include <windows.h>
static unsigned char* image;
static void (__fastcall* OriginalSingle)(tFace_ref*,br_vector3*,br_vector3*,br_vector3*,br_scalar*,br_vector3*);
static void (__fastcall* OriginalMulti)(int,tFace_ref*,br_vector3*,br_vector3*,br_vector3*,br_scalar*);
#define CheckSingleFace OriginalSingle
#define MultiRayCheckSingleFace OriginalMulti
#else
'''
    probe=r'''
#endif
#ifdef ORIGINAL_PROBE
static int C2_HOOK_CDECL load_original(const char* filename){FILE* f;unsigned char* disk;unsigned size,i,value,delta;IMAGE_NT_HEADERS32* nt;IMAGE_SECTION_HEADER* sections;
    f=fopen(filename,"rb");if(!f)return 3;fseek(f,0,SEEK_END);size=(unsigned)ftell(f);rewind(f);disk=(unsigned char*)malloc(size);
    if(!disk || fread(disk,1,size,f)!=size)return 4;
    fclose(f);nt=(IMAGE_NT_HEADERS32*)(disk+((IMAGE_DOS_HEADER*)disk)->e_lfanew);
    image=(unsigned char*)VirtualAlloc(NULL,nt->OptionalHeader.SizeOfImage,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image)return 5;
    sections=IMAGE_FIRST_SECTION(nt);for(i=0;i<nt->FileHeader.NumberOfSections;i++)memcpy(image+sections[i].VirtualAddress,disk+sections[i].PointerToRawData,sections[i].SizeOfRawData);
    delta=(unsigned)(uintptr_t)image-0x400000;
    for(i=0;i<sizeof(relocations)/sizeof(relocations[0]);i++){memcpy(&value,image+relocations[i],4);value+=delta;memcpy(image+relocations[i],&value,4);}
    OriginalSingle=(void(__fastcall*)(tFace_ref*,br_vector3*,br_vector3*,br_vector3*,br_scalar*,br_vector3*))(image+0x5ecc0);
    OriginalMulti=(void(__fastcall*)(int,tFace_ref*,br_vector3*,br_vector3*,br_vector3*,br_scalar*))(image+0x5f0d0);
    memcpy(image+0x1964c0,header+7,4);free(disk);return 0;
}
#endif
int C2_HOOK_CDECL main(int argc,char** argv){FILE* f;unsigned pointer_,i;unsigned slots[]={0,0x104};char* a=(char*)arena;
#ifdef ORIGINAL_PROBE
    if(argc!=3)return 1;
#else
    if(argc!=2)return 1;
#endif
    f=fopen(argv[1],"rb");if(!f)return 2;
    if(fread(header,sizeof(header),1,f)!=1 || fread(arena,sizeof(arena),1,f)!=1)return 3;
    fclose(f);_setmode(_fileno(stdout),_O_BINARY);_controlfp(_PC_53,_MCW_PC);gPling_materials=(int)header[7];
    for(i=0;i<2;i++){memcpy(&pointer_,a+slots[i],4);if(pointer_>=0x1000000&&pointer_<0x1001000){pointer_=pointer_-0x1000000+(unsigned)(uintptr_t)arena;memcpy(a+slots[i],&pointer_,4);}}
#ifdef ORIGINAL_PROBE
    {int r=load_original(argv[2]);if(r)return r;}
#endif
    if(header[0]==0)CheckSingleFace((tFace_ref*)a,(br_vector3*)(a+header[2]),(br_vector3*)(a+header[3]),(br_vector3*)(a+header[4]),(br_scalar*)(a+header[5]),(br_vector3*)(a+header[6]));
    else MultiRayCheckSingleFace((int)header[1],(tFace_ref*)a,(br_vector3*)(a+header[2]),(br_vector3*)(a+header[3]),(br_vector3*)(a+header[4]),(br_scalar*)(a+header[5]));
    snapshot();
#ifdef ORIGINAL_PROBE
    VirtualFree(image,0,MEM_RELEASE);
#endif
    return 0;
}
'''
    declarations='#ifdef ORIGINAL_PROBE\nstatic unsigned relocations[]={'+','.join(map(str,provenance['relocations']))+'};\n#endif\n'
    probe=probe.replace('#endif\n#ifdef ORIGINAL_PROBE','#endif\n'+declarations+'#ifdef ORIGINAL_PROBE',1)
    harness=output/'harness.c';harness.write_text(prefix+projection+'\n'+'\n'.join(bodies)+'\n'+probe)
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
    for mode in range(2):
        for axis in range(3):
            for sign in [-1,1]:
                for xy in [[.25,.25],[0,0],[1,0],[0,1],[.5,.5],[-.00005,.25],[-.0002,.25],[.75,.75]]:
                    other=[k for k in range(3)if k!=axis];p=[0,0,0];p[axis]=1;p[other[0]],p[other[1]]=xy
                    result.append(dict(mode=mode,axis=axis,normal=[sign if k==axis else 0 for k in range(3)],positions=[p]*4))
        for identifier in ['!','>','?','#','@','normal','']:
            for pling in [0,1]:result.append(dict(mode=mode,identifier=identifier,pling=pling))
        for option in [dict(null_material=True),dict(null_identifier=True),dict(flags=0),dict(vertices=[[0,0],[0,0],[0,0]]),dict(direction=[0,0,0]),dict(positions=[[.25,.25,0]]*4),dict(positions=[[.25,.25,-.0005]]*4),dict(positions=[[.25,.25,2.002]]*4),dict(positions=[[.25,.25,2.004]]*4)]:result.append(dict(mode=mode,**option))
    for count in [-1,0,1,2,3,4]:result.append(dict(mode=1,count=count,positions=[[.25,.25,1],[5,5,1],[.25,.25,-2],[.1,.1,.5]]))
    for field,offsets in [('normal_offset',[4,52,56,0x300,0x400]),('t_offset',[52,0x300,0x400,0x500]),('point_offset',[4,52,0x300,0x304,0x400,0x404])]:
        for mode in range(2):
            if mode==1 and field=='point_offset':continue
            for offset in offsets:result.append(dict(mode=mode,**{field:offset}))
    specials=[0,0x80000000,1,0x80000001,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc12345,0xffc23456,0x7f800001,0xff801234]
    for mode in range(2):
        for offset in [4,8,12,52,56,60,0x300,0x304,0x308,0x400,0x404,0x408]:
            for value in specials:result.append(dict(mode=mode,patch=[(offset,value)]))
    rng=random.Random(0x45ecc0)
    for i in range(100):
        x,y=rng.uniform(-128,128),rng.uniform(-128,128);sx,sy=rng.uniform(.125,32),rng.uniform(.125,32);hx,hy=rng.uniform(-16,16),rng.uniform(-16,16);a,b=rng.uniform(.01,.45),rng.uniform(.01,.45)
        for mode in range(2):result.append(dict(mode=mode,vertices=[[x,y],[x+sx,y+hy],[x+hx,y+sy]],positions=[[x+a*sx+b*hx,y+a*hy+b*sy,rng.uniform(.1,8)]for j in range(4)],direction=[0,0,-32]))
    for mode in range(2):
        for xy in [[-.0001,.25],[-.00010001,.25],[1.0001,0],[1.0000999,0],[.5,.5001],[.5,.5000999]]:
            result.append(dict(mode=mode,positions=[[xy[0],xy[1],1]]*4))
        for z in [-.001,-.0009999999,-.0010000001,2.003,2.0029999,2.0030001]:
            result.append(dict(mode=mode,positions=[[.25,.25,z]]*4))
        for offset in [4,52,56,0x300,0x400]:result.append(dict(mode=mode,normal=[0,0,-1],normal_offset=offset))
        result.append(dict(mode=mode,null_material=True,direction=[0,0,-1e-8]))
        result.append(dict(mode=mode,direction=[0,0,-3e-7],positions=[[.25,.25,1e-7],[.25,.25,3.4028234e38],[.25,.25,1e-7],[.25,.25,1e-7]]))
    for i in range(60):
        origin=[rng.uniform(-32,32)for j in range(3)];e1=[rng.uniform(-8,8)for j in range(3)];e2=[rng.uniform(-8,8)for j in range(3)]
        normal=[e1[1]*e2[2]-e1[2]*e2[1],e1[2]*e2[0]-e1[0]*e2[2],e1[0]*e2[1]-e1[1]*e2[0]]
        vertices=[origin,[origin[k]+e1[k]for k in range(3)],[origin[k]+e2[k]for k in range(3)]]
        positions=[[origin[k]+.2*e1[k]+.3*e2[k]+normal[k]for k in range(3)]]*4
        for mode in range(2):result.append(dict(mode=mode,vertices3d=vertices,normal=normal,positions=positions,direction=[-2*n for n in normal]))
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
        expected=subprocess.check_output([str(output/'original-x87.exe'),str(path),str(args.original.resolve())]);emulated=Fixture(original,ADDRESSES,case).execute()
        if emulated!=expected:emulation_gaps.append(dict(case=index,options=options,physical_sha256=hashlib.sha256(expected).hexdigest(),unicorn_sha256=hashlib.sha256(emulated).hexdigest()))
        checks=[('recompiled-x86',Fixture(rebuilt,addresses,case).execute())]
        for compiler in ['msvc5','msvc','mingw']:checks.append((compiler,subprocess.check_output([str(output/(compiler+'.exe')),str(path)])))
        for compiler,actual in checks:
            if actual!=expected:
                (output/f'failure-{index}-{compiler}.json').write_text(json.dumps(dict(options=options,original=expected.hex(),actual=actual.hex(),unicorn=emulated.hex()),indent=2));raise AssertionError(f'Case {index} {options} {compiler} differs; see {output}')
            results.append(dict(case=options,compiler=compiler,state_sha256=hashlib.sha256(actual).hexdigest()))
    (output/'verification.json').write_text(json.dumps(dict(original_sha256=ORIGINAL_SHA256,recompiled_sha256=hashlib.sha256(rebuilt).hexdigest(),source_sha256=hashlib.sha256((ROOT/'src/carma2/newcommon/34-finteray.c').read_bytes()).hexdigest(),compiler_commands=commands,physical_original_provenance=provenance,unicorn_gaps=emulation_gaps,cases=results),indent=2))
    print(f'PASS: {len(cases)} face scenarios, {len(results)} original physical/native/rebuilt comparisons; {len(emulation_gaps)} documented Unicorn differences')

if __name__=='__main__':main()
