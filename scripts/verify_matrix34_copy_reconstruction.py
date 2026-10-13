"""Prove actual BrMatrix34Copy raw bits and sequential overlap against x86.

Hash-checks the original79byte MOV-only leaf, executes it physically at PC53,
and compares nativeMSVC5/modernMSVC/MinGW complete256byte arenas. The ABI call
is kept out of line through a volatile function pointer. No public normalization.
Requires scoped compilers and Python3; optional immutable PE/map proves linked
actual leaf instructions. No game entry/import/TLS executes.
"""
import argparse,hashlib,json,os,struct,subprocess
from pathlib import Path
from verify_box_pick_reconstruction import ROOT,HASH,words
from verify_tinted_native_reconstruction import extract
SOURCE=ROOT/'src/brender/core/math/matrix34.c'

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE');p.add_argument('--output',type=Path,default=ROOT/'build-audit/physics-collision-next/copy-tests');p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);a=p.parse_args()
    if bool(a.recompiled)!=bool(a.symbols):p.error('Supply both immutable PE and symbols')
    out=a.output.resolve();out.mkdir(parents=True,exist_ok=True);original=a.original.read_bytes();assert hashlib.sha256(original).hexdigest()==HASH
    def raw(b,address):
        nt=struct.unpack_from('<I',b,60)[0];base=struct.unpack_from('<I',b,nt+52)[0];t=nt+24+struct.unpack_from('<H',b,nt+20)[0]
        for i in range(struct.unpack_from('<H',b,nt+6)[0]):
            _,v,n,o=struct.unpack_from('<IIII',b,t+40*i+8)
            if v<=address-base<v+n:return b[o+address-base-v:o+address-base-v+256]
        raise ValueError('Unmapped leaf')
    images=[('original',original,0x5325d0)]
    if a.recompiled:
        b=a.recompiled.read_bytes();m=json.loads(a.symbols.read_text());assert m['format']==1 and m['original_sha256']==HASH and m['recompiled_sha256']==hashlib.sha256(b).hexdigest();address=next(v['recomp']for v in m['symbols']if v['orig']==0x5325d0);images.append(('matching',b,address))
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
typedef void(C2_HOOK_CDECL*copy_fn)(br_matrix34*,const br_matrix34*);
'''
    driver=r'''
int C2_HOOK_CDECL main(int argc,char**argv){
 FILE*f;unsigned h[2];unsigned char arena[256];copy_fn volatile call;
 if(argc!=2)return 2;f=fopen(argv[1],"rb");if(!f)return 3;if(fread(h,sizeof(h),1,f)!=1||fread(arena,sizeof(arena),1,f)!=1)return 4;fclose(f);
 INSTALL
 _controlfp(_PC_53,_MCW_PC);call((br_matrix34*)(arena+h[0]),(const br_matrix34*)(arena+h[1]));_setmode(_fileno(stdout),_O_BINARY);fwrite(arena,sizeof(arena),1,stdout);return 0;
}
'''
    native=prefix+extract(SOURCE.read_text(),'BrMatrix34Copy')+driver.replace('INSTALL','call=BrMatrix34Copy;');(out/'harness.c').write_text(native)
    for label,code in codes:
        probe=prefix+'static const unsigned char original_code[]={'+','.join(map(str,code))+'};\n'+driver.replace('INSTALL','{void*code=VirtualAlloc(NULL,4096,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!code)return 5;memcpy(code,original_code,sizeof(original_code));call=(copy_fn)code;}');(out/(label+'.c')).write_text(probe)
    inc=[ROOT/v for v in ('src/carma2/hooks','src/brender/include','src/brender/core/math')];r=ROOT/'build-tools/msvc-modern';ms=sorted((r/'VC/Tools/MSVC').iterdir())[-1];sdk=r/'Windows Kits/10';v=sorted((sdk/'Include').iterdir())[-1].name;env=dict(os.environ);env['PATH']=str(ms/'bin/Hostx64/x86')+os.pathsep+env['PATH'];env['INCLUDE']=os.pathsep.join(map(str,[ms/'include',*[sdk/f'Include/{v}/{part}' for part in ('ucrt','shared','um')]]));env['LIB']=os.pathsep.join(map(str,[ms/'lib/x86',*[sdk/f'Lib/{v}/{part}/x86' for part in ('ucrt','um')]]));m5=ROOT/'build-tools/msvc500sp3';e5=dict(os.environ);e5['PATH']=str(m5/'bin')+os.pathsep+str(m5/'redist')+os.pathsep+e5['PATH'];e5['INCLUDE']=str(m5/'include');e5['LIB']=str(m5/'lib');gcc=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'
    commands={
      'msvc':[str(ms/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(x)for x in inc],str(out/'harness.c'),'/Fo'+str(out/'msvc.obj'),'/Fe'+str(out/'msvc.exe')],
      'msvc5':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(x)for x in inc],str(out/'harness.c'),'/Fo'+str(out/'msvc5.obj'),'/Fe'+str(out/'msvc5.exe')],
      'mingw':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation',*['-I'+str(x)for x in inc],str(out/'harness.c'),'-o',str(out/'mingw.exe')]}
    for label,code in codes:commands[label]=[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-DREC2_MATCHING',*['-I'+str(x)for x in inc],str(out/(label+'.c')),'-o',str(out/(label+'.exe'))]
    for label,cmd in commands.items():
        build=subprocess.run(cmd,cwd=out,env=env if label=='msvc' else e5 if label=='msvc5' else None,text=True,capture_output=True);(out/(label+'-build.txt')).write_text(build.stdout+build.stderr);assert not build.returncode,build.stdout+build.stderr
    patterns=[0,0x80000000,1,0x80000001,0x3f800000,0xbf800000,0x7f7fffff,0x7f800000,0xff800000,0x7fc12345,0xffc12345,0x7f800001,0xff800001]
    results=[];index=0
    for offset in (-49,-48,-47,-16,-4,-1,0,1,4,16,47,48,49):
        for seed in (0,1,2):
            arena=words([patterns[(i+seed)%len(patterns)] for i in range(64)]);fixture=out/f'case-{index:03d}.bin';fixture.write_bytes(words([96+offset,96])+arena);expected=subprocess.check_output([str(out/'original.exe'),str(fixture)])
            for label in ('msvc5','msvc','mingw',*(['matching'] if a.recompiled else [])):
                actual=subprocess.check_output([str(out/(label+'.exe')),str(fixture)])
                if actual!=expected:
                    (out/f'failure-{label}-{index}.json').write_text(json.dumps(dict(offset=offset,seed=seed,expected=expected.hex(),actual=actual.hex()),indent=2));raise AssertionError(f'{label} copy{index} differs')
                results.append(dict(case_index=index,compiler=label,offset=offset,seed=seed,all_bytes_match=True,arena_sha256=hashlib.sha256(actual).hexdigest()))
            index+=1
    report=dict(original_sha256=HASH,scenes=index,comparisons=len(results),all_cases_passed=True,source_sha256=hashlib.sha256(SOURCE.read_bytes()).hexdigest(),harness_sha256=hashlib.sha256(native.encode()).hexdigest(),original_code_sha256=hashlib.sha256(codes[0][1]).hexdigest(),commands=commands,results=results,limitations=__doc__)
    if a.recompiled:report.update(recompiled_sha256=hashlib.sha256(a.recompiled.read_bytes()).hexdigest(),symbols_sha256=hashlib.sha256(a.symbols.read_bytes()).hexdigest(),linked_code_sha256=hashlib.sha256(codes[1][1]).hexdigest())
    (out/'verification.json').write_text(json.dumps(report,indent=2)+'\n');print(f'PASS: {index} scenes/{len(results)} physical-original/native copy comparisons')

if __name__=='__main__':main()
