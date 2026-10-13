"""Prove actual DistanceFromFaceND PC53 raw return/NaN behavior against physical x86.

Hash-checks the original39byte x87 leaf, executes it physically at PC53,
and compares nativeMSVC5/modernMSVC/MinGW raw double return, scalar copy and input arena. Both original callers consume raw ST0 before float32 rounding: ProcessForcesCallback
0x41c31e and FindSpecialVolume0x4ff17f. Double return preserves that contract.
The raw plane argument and call stay out of line. No public normalization.
Requires scoped compilers and Python3; optional immutable PE/map proves linked
actual leaf instructions. Other-architecture fallback NaN payload choices are outside this x86 proof.
No game entry/import/TLS executes.
"""
import argparse,hashlib,json,os,struct,subprocess
from pathlib import Path
from verify_box_pick_reconstruction import ROOT,HASH,words,fb
from verify_tinted_native_reconstruction import extract
SOURCE=ROOT/'src/carma2/newcommon/41-utility.c'

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE');p.add_argument('--output',type=Path,default=ROOT/'build-audit/distance-face-tests');p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);a=p.parse_args()
    if bool(a.recompiled)!=bool(a.symbols):p.error('Supply both immutable PE and symbols')
    out=a.output.resolve();out.mkdir(parents=True,exist_ok=True);original=a.original.read_bytes();assert hashlib.sha256(original).hexdigest()==HASH
    def raw(b,address):
        nt=struct.unpack_from('<I',b,60)[0];base=struct.unpack_from('<I',b,nt+52)[0];t=nt+24+struct.unpack_from('<H',b,nt+20)[0]
        for i in range(struct.unpack_from('<H',b,nt+6)[0]):
            _,v,n,o=struct.unpack_from('<IIII',b,t+40*i+8)
            if v<=address-base<v+n:return b[o+address-base-v:o+address-base-v+256]
        raise ValueError('Unmapped leaf')
    images=[('original',original,0x5162a0)]
    if a.recompiled:
        b=a.recompiled.read_bytes();m=json.loads(a.symbols.read_text());assert m['format']==1 and m['original_sha256']==HASH and m['recompiled_sha256']==hashlib.sha256(b).hexdigest();address=next(v['recomp']for v in m['symbols']if v['orig']==0x5162a0);images.append(('matching',b,address))
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_MEM
    cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;codes=[]
    for label,b,address in images:
        code=raw(b,address);n=0
        for ins in cs.disasm(code,address):
            assert ins.mnemonic!='call' and not ins.mnemonic.startswith('j')
            for op in ins.operands:
                if op.type==CS_OP_MEM:assert op.mem.base or op.mem.index
            n+=ins.size
            if ins.mnemonic=='ret':break
        else:raise ValueError('Leaf lacks RET')
        codes.append((label,code[:n]));(out/(label+'-leaf.bin')).write_bytes(code[:n])
    prefix=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "matrix34.h"
#define A(x,y) A->m[x][y]
#define B(x,y) B->m[x][y]
typedef double(C2_HOOK_FASTCALL*distance_fn)(const br_vector3*,const br_vector3*,unsigned);
typedef double(C2_HOOK_FASTCALL*typed_fn)(const br_vector3*,const br_vector3*,br_scalar);
'''
    driver=r'''
int C2_HOOK_CDECL main(int argc,char**argv){
 FILE*f;unsigned h[3],raw[2],scalar_bits;unsigned char arena[256];distance_fn volatile call;double result;volatile float scalar;
 if(argc!=2)return 2;f=fopen(argv[1],"rb");if(!f)return 3;if(fread(h,sizeof(h),1,f)!=1||fread(arena,sizeof(arena),1,f)!=1)return 4;fclose(f);
 INSTALL
 _controlfp(_PC_53,_MCW_PC);result=call((const br_vector3*)(arena+h[0]),(const br_vector3*)(arena+h[1]),h[2]);scalar=(float)result;memcpy(raw,&result,8);memcpy(&scalar_bits,(const void*)&scalar,4);_setmode(_fileno(stdout),_O_BINARY);fwrite(raw,8,1,stdout);fwrite(&scalar_bits,4,1,stdout);fwrite(arena,sizeof(arena),1,stdout);return 0;
}
'''
    body=extract(SOURCE.read_text(),'DistanceFromFaceND')
    native=prefix+body+driver.replace('INSTALL','{union{typed_fn typed;distance_fn raw;}cast;cast.typed=DistanceFromFaceND;call=cast.raw;}');(out/'harness.c').write_text(native)
    for label,code in codes:
        probe=prefix+'static const unsigned char original_code[]={'+','.join(map(str,code))+'};\n'+driver.replace('INSTALL','{void*code=VirtualAlloc(NULL,4096,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!code)return 5;memcpy(code,original_code,sizeof(original_code));call=(distance_fn)code;}');(out/(label+'.c')).write_text(probe)
    inc=[ROOT/v for v in ('src/carma2/hooks','src/brender/include','src/brender/core/math')];r=ROOT/'build-tools/msvc-modern';ms=sorted((r/'VC/Tools/MSVC').iterdir())[-1];sdk=r/'Windows Kits/10';v=sorted((sdk/'Include').iterdir())[-1].name;env=dict(os.environ);env['PATH']=str(ms/'bin/Hostx64/x86')+os.pathsep+env['PATH'];env['INCLUDE']=os.pathsep.join(map(str,[ms/'include',*[sdk/f'Include/{v}/{part}' for part in ('ucrt','shared','um')]]));env['LIB']=os.pathsep.join(map(str,[ms/'lib/x86',*[sdk/f'Lib/{v}/{part}/x86' for part in ('ucrt','um')]]));m5=ROOT/'build-tools/msvc500sp3';e5=dict(os.environ);e5['PATH']=str(m5/'bin')+os.pathsep+str(m5/'redist')+os.pathsep+e5['PATH'];e5['INCLUDE']=str(m5/'include');e5['LIB']=str(m5/'lib');gcc=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'
    commands={
      'msvc':[str(ms/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(x)for x in inc],str(out/'harness.c'),'/Fo'+str(out/'msvc.obj'),'/Fe'+str(out/'msvc.exe')],
      'msvc5':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(x)for x in inc],str(out/'harness.c'),'/Fo'+str(out/'msvc5.obj'),'/Fe'+str(out/'msvc5.exe')],
      'mingw':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation',*['-I'+str(x)for x in inc],str(out/'harness.c'),'-o',str(out/'mingw.exe')]}
    for label,code in codes:commands[label]=[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-DREC2_MATCHING',*['-I'+str(x)for x in inc],str(out/(label+'.c')),'-o',str(out/(label+'.exe'))]
    for label,cmd in commands.items():
        build=subprocess.run(cmd,cwd=out,env=env if label in ('msvc','fallback-msvc') else e5 if label in ('msvc5','fallback-msvc5') else None,text=True,capture_output=True);(out/(label+'-build.txt')).write_text(build.stdout+build.stderr);assert not build.returncode,build.stdout+build.stderr
    cases=scenarios();results=[]
    for index,(header,arena,finite) in enumerate(cases):
        fixture=out/f'case-{index:03d}.bin';fixture.write_bytes(words(header)+arena);expected=subprocess.check_output([str(out/'original.exe'),str(fixture)])
        labels=['msvc5','msvc','mingw',*(['matching'] if a.recompiled else [])]

        for label in labels:
            actual=subprocess.check_output([str(out/(label+'.exe')),str(fixture)])
            if actual!=expected:
                (out/f'failure-{label}-{index}.json').write_text(json.dumps(dict(header=header,finite=finite,expected=expected.hex(),actual=actual.hex()),indent=2));raise AssertionError(f'{label} DistanceFromFaceND{index} differs')
            results.append(dict(case_index=index,compiler=label,finite=finite,all_bytes_match=True,arena_sha256=hashlib.sha256(actual).hexdigest()))
    index=len(cases)
    report=dict(original_sha256=HASH,scenes=index,comparisons=len(results),all_cases_passed=True,source_sha256=hashlib.sha256(SOURCE.read_bytes()).hexdigest(),harness_sha256=hashlib.sha256(native.encode()).hexdigest(),original_code_sha256=hashlib.sha256(codes[0][1]).hexdigest(),commands=commands,results=results,limitations=__doc__)
    if a.recompiled:report.update(recompiled_sha256=hashlib.sha256(a.recompiled.read_bytes()).hexdigest(),symbols_sha256=hashlib.sha256(a.symbols.read_bytes()).hexdigest(),linked_code_sha256=hashlib.sha256(codes[1][1]).hexdigest())
    (out/'verification.json').write_text(json.dumps(report,indent=2)+'\n');print(f'PASS: {index} scenes/{len(results)} physical-original/native DistanceFromFaceND comparisons')

def scenarios():
    cases=[]
    def case(point,normal,d=0,finite=True):
        data=bytearray(b'\xa5'*256);data[128:140]=words(point);data[64:76]=words(normal);cases.append(([128,64,d],bytes(data),finite))
    nan=[0x7fc12345,0xffc12345,0x7fc54321,0xffc54321,0x7f800001,0xff800001,0x7f812345,0xff812345]
    for axis in range(3):
        for left in nan:
            for right in nan:
                point=[0x3f800000,0,0];normal=point[:];point[axis]=left;normal[axis]=right;case(point,normal,finite=False)
    for p,n,d in [([.314159,-.271828,1.41421],[.8,.6,.25],.000001),([1,2**55,-2**55],[1,1,1],0),([2**55,1,-2**55],[1,1,1],0),([2**55,-2**55,1],[1,1,1],0),([1e30,1e30,1e30],[1e30]*3,0),([1e-30]*3,[1e-30]*3,0),([.1,.2,.3],[.4,.5,.6],.31)]:case([fb(v)for v in p],[fb(v)for v in n],fb(d))
    for value in (0,0x80000000,1,0x80000001,0x7f800000,0xff800000,*nan):
        case([value]*3,[0x3f800000]*3,finite=False);case([0x3f800000]*3,[0]*3,value,finite=False)
    for left,right in ((nan[0],nan[2]),(nan[4],nan[2]),(nan[2],nan[4])):case([left,0,0],[0x3f800000,0,0],right,finite=False)
    return cases

if __name__=='__main__':main()
