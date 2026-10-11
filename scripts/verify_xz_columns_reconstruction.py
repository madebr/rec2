#!/usr/bin/env python3
"""Verify XZToColumnXZ with original x86, physical x87, and actual native body.

Full repository tTrack_spec/guard/output arena is compared. Original X is
explicitly rounded to scalar before upper clamp and integer conversion; Z
stays at runtime PC53. Every case also runs unmodified original arithmetic
and original _ftol on physical x87, relocating only five constant references.
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
from verify_tinted_native_reconstruction import extract

ROOT=Path(__file__).resolve().parents[1]
SOURCE=ROOT/'src/carma2/newcommon/37-brucetrk.c'
HASH='9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
FUNCTION=0x40cb90
HEAP,STACK,STOP=0x20000000,0x1080000,0x1081000
def words(values):return struct.pack('<'+'I'*len(values),*[v&0xffffffff for v in values])

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE')
    p.add_argument('--dependency-dir',type=Path,default=ROOT/'build-audit/joystick/python-deps')
    p.add_argument('--gcc',type=Path,default=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe')
    p.add_argument('--msvc-root',type=Path,default=ROOT/'build-tools/msvc-modern')
    p.add_argument('--msvc5-root',type=Path,default=ROOT/'build-tools/msvc500sp3')
    p.add_argument('--output',type=Path,default=ROOT/'build-audit/xz-columns-tests')
    p.add_argument('--symbols',type=Path)
    p.add_argument('--recompiled',type=Path,default=ROOT/'build-matching/rec2.exe')
    p.add_argument('--case-index',type=int,action='append')
    args=p.parse_args();sys.path.insert(0,str(args.dependency_dir.resolve()))
    from unicorn import Uc,UC_ARCH_X86,UC_MODE_32
    from unicorn.x86_const import UC_X86_REG_EIP,UC_X86_REG_ESP,UC_X86_REG_ECX,UC_X86_REG_EDX,UC_X86_REG_FPCW
    binary=args.original.read_bytes()
    if hashlib.sha256(binary).hexdigest()!=HASH:raise SystemExit('Original executable hash differs')
    images=[(binary,FUNCTION)]
    if args.symbols:
        m=json.loads(args.symbols.read_text());b=args.recompiled.read_bytes()
        if m['format']!=1 or m['original_sha256']!=HASH or m['recompiled_sha256']!=hashlib.sha256(b).hexdigest():raise SystemExit('Stale or incompatible column symbol map')
        mapped={s['orig']:s['recomp'] for s in m['symbols']};images.append((b,mapped[FUNCTION]))
    out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)
    driver=r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "37-brucetrk.h"
typedef char layouts[sizeof(void*)==4&&sizeof(tTrack_spec)==36&&offsetof(tTrack_spec,column_size_x)==4&&offsetof(tTrack_spec,column_size_z)==8&&offsetof(tTrack_spec,origin_x)==12&&offsetof(tTrack_spec,origin_z)==16?1:-1];
static unsigned header[10];static unsigned char arena[128];
'''+extract(SOURCE.read_text(),'XZToColumnXZ')+r'''
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;float x,z;
 if(argc!=2)return 90;
 f=fopen(argv[1],"rb");if(!f)return 91;if(fread(header,sizeof(header),1,f)!=1||fread(arena,sizeof(arena),1,f)!=1)return 92;fclose(f);
 memcpy(&x,header,4);memcpy(&z,header+1,4);_controlfp(_PC_53,_MCW_PC);
 XZToColumnXZ(arena+header[6],arena+header[7],x,z,(tTrack_spec*)(arena+32));
 _setmode(_fileno(stdout),_O_BINARY);fwrite(arena,sizeof(arena),1,stdout);return 0;
}
'''
    harness=out/'harness.c';harness.write_text(driver)
    includes=[ROOT/path for path in ('src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/carma2/platform','src/carma2/s3/include','src/brender/include')]
    msvc=sorted((args.msvc_root/'VC/Tools/MSVC').iterdir())[-1];sdk=args.msvc_root/'Windows Kits/10';version=sorted((sdk/'Include').iterdir())[-1].name
    env=dict(os.environ);env['PATH']=str(msvc/'bin/Hostx64/x86')+os.pathsep+env['PATH'];env['INCLUDE']=os.pathsep.join(map(str,[msvc/'include',*[sdk/f'Include/{version}/{part}' for part in ('ucrt','shared','um')]]));env['LIB']=os.pathsep.join(map(str,[msvc/'lib/x86',*[sdk/f'Lib/{version}/{part}/x86' for part in ('ucrt','um')]]))
    msvc5=args.msvc5_root.resolve();env5=dict(os.environ);env5['PATH']=str(msvc5/'bin')+os.pathsep+str(msvc5/'redist')+os.pathsep+env5['PATH'];env5['INCLUDE']=str(msvc5/'include');env5['LIB']=str(msvc5/'lib')
    commands={
        'msvc':[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(p) for p in includes],str(harness),'/Fo'+str(out/'msvc.obj'),'/Fe'+str(out/'msvc.exe')],
        'msvc5':[str(msvc5/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(p) for p in includes],str(harness),'/Fo'+str(out/'msvc5.obj'),'/Fe'+str(out/'msvc5.exe')],
        'mingw':[str(args.gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-fno-strict-aliasing',*['-I'+str(p) for p in includes],str(harness),'-o',str(out/'mingw.exe')],
    }
    for compiler,command in commands.items():
        r=subprocess.run(command,cwd=out,env=env if compiler=='msvc' else env5 if compiler=='msvc5' else None,text=True,capture_output=True);(out/f'{compiler}-build.txt').write_text(r.stdout+r.stderr)
        if r.returncode:raise RuntimeError(r.stdout+r.stderr)
    probe=ROOT/'scripts/verify_xz_columns_original_x87_probe.c';probe_exe=out/'original-x87.exe';probe_command=[str(args.gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror',str(probe),'-o',str(probe_exe)];subprocess.run(probe_command,check=True)
    def execute(image,function,header,arena):
        u=Uc(UC_ARCH_X86,UC_MODE_32);pe=struct.unpack_from('<I',image,60)[0];base=struct.unpack_from('<I',image,pe+52)[0];size=struct.unpack_from('<I',image,pe+80)[0];u.mem_map(base,(size+4095)&~4095);u.mem_map(HEAP,0x1000);u.mem_map(STACK-0x8000,0x10000)
        table=pe+24+struct.unpack_from('<H',image,pe+20)[0]
        for i in range(struct.unpack_from('<H',image,pe+6)[0]):
            _,rva,size,offset=struct.unpack_from('<IIII',image,table+40*i+8);u.mem_write(base+rva,image[offset:offset+size])
        u.mem_write(HEAP,arena);u.mem_write(STACK,words([STOP,header[0],header[1],HEAP+32]));u.reg_write(UC_X86_REG_ESP,STACK);u.reg_write(UC_X86_REG_ECX,HEAP+header[6]);u.reg_write(UC_X86_REG_EDX,HEAP+header[7]);u.reg_write(UC_X86_REG_FPCW,0x27f);u.emu_start(function,STOP,count=10000)
        assert u.reg_read(UC_X86_REG_EIP)==STOP and u.reg_read(UC_X86_REG_ESP)==STACK+16,'Column RET12 ABI differs'
        return bytes(u.mem_read(HEAP,128))
    def fb(x):return struct.unpack('<I',struct.pack('<f',x))[0]
    def case(**kw):
        c=dict(x=fb(2.5),z=fb(3.5),nx=8,nz=9,sx=fb(1),sz=fb(1),ox=0,oz=0,outx=0,outz=1);c.update(kw);return c
    cases=[]
    for n in (0,1,2,8,255):
        for value in (-10,-0.0,0,0.5,0.99999994,1,1.000000119,7.9999995,8,8.000001,254.99998,255,256):cases.append(case(nx=n,nz=n,x=fb(value),z=fb(value)))
    specials=(0,0x80000000,1,0x80000001,0x007fffff,0x00800000,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc00001,0xffc12345,0x7f800001,0xff800001,0x7fbfffff)
    for field in ('x','z','sx','sz','ox','oz'):
        for value in specials:
            for n in (0,8):cases.append(case(nx=n,nz=n,**{field:value}))
    # (1 - 2^-26)/1 is below 1 in PC53 but rounds to 1 in the original X spill.
    for ox in (fb(-2**-25),fb(-2**-26),fb(2**-25),fb(2**-26)):
        for sx in (fb(1+2**-23),fb(1),fb(1-2**-24)):
            cases.append(case(x=fb(1),z=fb(1),ox=ox,oz=ox,sx=sx,sz=sx))
    rng=random.Random(0x40cb90)
    for _ in range(75):cases.append(case(**{f:rng.getrandbits(32) for f in ('x','z','sx','sz','ox','oz')},nx=rng.randrange(256),nz=rng.randrange(256)))
    cases += [case(outx=x,outz=z) for x,z in ((0,0),(33,32),(48,44),(40,36),(32,33))]
    if args.case_index is not None and any(i<0 or i>=len(cases) for i in args.case_index):p.error(f'--case-index must be between 0 and {len(cases)-1}')
    records=[]
    for index,c in enumerate(cases):
        if args.case_index is not None and index not in args.case_index:continue
        h=[c[f] for f in ('x','z','nx','nz','sx','sz','outx','outz','ox','oz')];arena=bytearray((i*37+165)&255 for i in range(128));arena[32:34]=bytes([c['nx'],c['nz']]);arena[36:52]=words([c[f] for f in ('sx','sz','ox','oz')]);arena=bytes(arena)
        fixture=out/'case.bin';fixture.write_bytes(words(h)+arena);expected=execute(*images[0],h,arena)
        physical=subprocess.check_output([str(probe_exe),str(args.original),str(fixture)])
        if physical!=expected:
            (out/f'physical-divergence-{index}.json').write_text(json.dumps(dict(case=c,original=expected.hex(),physical=physical.hex()),indent=2));raise AssertionError(f'Original physical/Unicorn column case {index} differs; no normalization applied')
        checks=[(compiler,subprocess.check_output([str(out/f'{compiler}.exe'),str(fixture)])) for compiler in commands]
        if len(images)>1:checks.append(('matching PE',execute(*images[1],h,arena)))
        for compiler,actual in checks:
            if actual!=expected:
                (out/f'failure-{compiler.replace(" ","-")}-{index}.json').write_text(json.dumps(dict(case=c,original=expected.hex(),actual=actual.hex()),indent=2));raise AssertionError(f'Column {compiler} case {index} differs; see {out}')
            records.append(dict(compiler=compiler,case_index=index,case=c,arena_sha256=hashlib.sha256(actual).hexdigest(),all_bytes_match=True))
        if (index+1)%100==0:print(f'Verified {index+1}/{len(cases)} column cases',flush=True)
    manifest=dict(original_sha256=HASH,functions=dict(XZToColumnXZ=FUNCTION),scenario_count=len(cases),comparisons=len(records),complete_suite=args.case_index is None,all_cases_passed=True,physical_original_checked=True,return_normalization_applied=False,compiler_commands=commands,probe_command=probe_command,probe_sha256=hashlib.sha256(probe.read_bytes()).hexdigest(),harness_sha256=hashlib.sha256(driver.encode()).hexdigest(),source_sha256={str(path.relative_to(ROOT)):hashlib.sha256(path.read_bytes()).hexdigest() for path in (SOURCE,ROOT/'src/carma2/newcommon/37-brucetrk.h',ROOT/'src/carma2/include/rec2_types.h')},cases=records,limitations='Normal runtime PC53. Complete finite/nonfinite grid arithmetic and bounded alias/guard arena; helper has no controlled external callbacks, and original _ftol executes in both emulation and physical probe.')
    if len(images)>1:manifest.update(recompiled_sha256=hashlib.sha256(images[1][0]).hexdigest(),symbols_sha256=hashlib.sha256(args.symbols.read_bytes()).hexdigest())
    (out/'verification.json').write_text(json.dumps(manifest,indent=2)+'\n');print(f'Passed {len(records)} original/native column comparisons plus physical original cases')

if __name__=='__main__':main()
