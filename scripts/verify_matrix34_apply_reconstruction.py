"""Actual BrMatrix34Apply0x533410 physical-original/native3/optional linked proof.

Requires CPython3.13, Capstone5 and scoped x86 MSVC5SP3/modern MSVC/MinGW.
The222 preserved fixtures include four concrete finite Wave27 regressions,
the failed C-first paired-NaN case, exact register-loaded versus scalar-memory
NaN products, exponent spread and full/partial/byte output aliases. Actual
production and genuine linked helper graph execute at masked PC53. Full512B,
ESP/nonvolatile registers/x87TOP compare raw; voidEAX is undefined/not compared.
No numerical callee substitution, scalar normalization, arbitrary private-stack,
sticky-flag/unmasked-trap or game-entry/import/TLS claim. Portable non-x86 paths
are outside this IEEE proof. Linked PE requires immutable hash-bound symbols.
"""
import argparse,hashlib,json,os,random,re,struct,subprocess,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];sys.path.insert(0,str(ROOT/'scripts'))
import verify_wall_quadratic_reconstruction as q
from verify_tinted_native_reconstruction import extract
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_MEM,CS_OP_IMM
HASH=q.HASH;SOURCE=ROOT/'src/brender/core/math/matrix34.c'
NAMES=['Matrix34ApplyProductLoaded','Matrix34ApplyProductMemory','Matrix34ApplyAdd','BrMatrix34Apply']
FIXTURE_SHA256='24f4789674903955480bcd9f21f3c60bd5cd45574b14ac33de5c611399ca72f3'
def sha(data):return hashlib.sha256(data).hexdigest()
def fixtures():
    cases=[];rng=random.Random(0x5333a0)
    def add(mode,label,output=0,vector=None,matrix=None,patch=()):
        a=bytearray(b'\xa5'*512);struct.pack_into('<4f',a,128,*(vector or[1.25,-2.5,3.125,.75]));struct.pack_into('<12f',a,256,*(matrix or[1.125,-.375,.0625,.4375,1.75,-.25,-.625,.15625,.75,2.375,-4.125,8.0625]))
        for at,bits in patch:struct.pack_into('<I',a,at,bits)
        cases.append(dict(mode=mode,label=label,data=struct.pack('<4I',mode,output,128,256)+a))
    # Preserve RNG/control flow of all four prior leaf fixtures before selecting
    # only Apply: the preceding mode consumes the established finite RNG stream.
    for mode in range(4):
        add(mode,'general');add(mode,'identity',matrix=[1,0,0,0,1,0,0,0,1,0,0,0])
        for i in range(24):add(mode,'finite-rounding',vector=[rng.uniform(-8,8)for _ in range(4)],matrix=[rng.uniform(-8,8)for _ in range(12)])
        for i in range(16):add(mode,'finite-exponent-spread',patch=[(at,(rng.getrandbits(1)<<31)|(rng.randrange(1,255)<<23)|rng.getrandbits(23))for at in [128+4*j for j in range(4)]+[256+4*j for j in range(12)]])
        for out in [128,124,132,127,129,256,252,260,255,257,264,268]:add(mode,'valid-output-alias',output=out)
        for at in [128+4*j for j in range(4)]+[256+4*j for j in range(12)]:
            for bits in [0x80000000,1,0x7f800000,0xff800000,0x7fc12345,0x7f812345]:add(mode,'single-IEEE',patch=[(at,bits)])
        for a,b in [(128,132),(132,136),(128,256),(132,260),(136,264),(128,268),(140,292),(256,260),(268,280)]:
            for x,y in [(0x7fc10001,0x7fc20002),(0x7f810001,0xffc20002),(0xffc20002,0x7f810001),(0x7fc10001,0xffc10001)]:add(mode,'distinct-paired-NaNs',patch=[(a,x),(b,y)])
        for scale in [2.**24,2.**53,2.**-70]:
            for sign in [1.,-1.]:add(mode,'finite-cancellation',vector=[1,1,1,1],matrix=[scale,-scale,sign,-scale,sign,scale,sign,scale,-scale,sign,-sign,scale])
    cases=[c for c in cases if c['mode']==1]
    for a,b in [(292,140),(280,136),(296,140),(272,132),(288,136),(300,140),(268,132),(256,128),(284,136),(260,128),(276,132),(264,128)]:
        for x,y in [(0x7fc10001,0x7f820002),(0xff820002,0x7fc10001)]:
            z=bytearray(cases[0]['data']);struct.pack_into('<I',z,16+a,x);struct.pack_into('<I',z,16+b,y);cases.append(dict(mode=1,label='exact-loaded-or-memory-product-NaNs',data=bytes(z)))
    for output in [128,132,252,256,260,268]:
        z=bytearray(cases[0]['data']);struct.pack_into('<I',z,4,output);struct.pack_into('<I',z,16+272,0x7fc10001);struct.pack_into('<I',z,16+132,0x7f820002);cases.append(dict(mode=1,label='paired-NaN-sequential-output-alias',data=bytes(z)))
    assert len(cases)==222 and sha(b''.join(c['data']for c in cases))==FIXTURE_SHA256
    return cases

COMMON='''#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "matrix34.h"
'''+q.DECLS
DRIVER='''int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned h[4],i;unsigned char arena[512];void*fn;if(argc!=2)return 2;
f=fopen(argv[1],"rb");if(!f)return 3;if(fread(h,sizeof(h),1,f)!=1||fread(arena,512,1,f)!=1)return 4;fclose(f);INSTALL
_controlfp(_PC_53,_MCW_PC);builder_address=(unsigned)fn;builder_mode=0;builder_cdecl=1;builder_stack_count=3;builder_poison=0xcccccccc;for(i=0;i<3;i++)builder_stack[i]=(unsigned)(arena+h[i+1]);if(BuilderInvoke())return 14;
_setmode(_fileno(stdout),_O_BINARY);fwrite(arena,512,1,stdout);return 0;}'''
def native(body):return COMMON+'\n'.join(extract(body,n)for n in NAMES)+DRIVER.replace('INSTALL','fn=(void*)BrMatrix34Apply;')
def physical(binary,entry):
    nt=struct.unpack_from('<I',binary,60)[0];base=struct.unpack_from('<I',binary,nt+52)[0];tab=nt+24+struct.unpack_from('<H',binary,nt+20)[0];segments=[(*struct.unpack_from('<4I',binary,tab+40*i+8),struct.unpack_from('<I',binary,tab+40*i+36)[0])for i in range(struct.unpack_from('<H',binary,nt+6)[0])]
    def raw(address,size=15):
        for _,va,n,off,flags in segments:
            if flags&0x20 and va<=address-base<va+n:return binary[off+address-base-va:off+address-base-va+size]
        raise AssertionError(('Not actual executable code',hex(address)))
    cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;todo=[entry];instructions={};roots={entry};refs={}
    while todo:
        address=todo.pop()
        if address in instructions:continue
        i=next(cs.disasm(raw(address),address,count=1));instructions[address]=i.bytes.hex()
        for op in i.operands:
            if op.type==CS_OP_MEM and not op.mem.base and not op.mem.index:
                assert any(va<=op.mem.disp-base<va+max(vs,n)for vs,va,n,_,_ in segments);assert i.disp_size==4;refs[address-base+i.disp_offset]=op.mem.disp
            if op.type==CS_OP_IMM and i.mnemonic not in ('call','jmp')and not i.mnemonic.startswith('j'):
                if any(va<=op.imm-base<va+max(vs,n)for vs,va,n,_,_ in segments):assert i.imm_size==4;refs[address-base+i.imm_offset]=op.imm
        if i.mnemonic=='call':assert i.operands[0].type==CS_OP_IMM;roots.add(i.operands[0].imm);todo.append(i.operands[0].imm)
        if i.mnemonic=='ret':assert not i.operands;continue
        if i.mnemonic.startswith('j'):
            assert i.operands[0].type==CS_OP_IMM;todo.append(i.operands[0].imm)
            if i.mnemonic=='jmp':continue
        todo.append(address+i.size);assert len(instructions)<4096
    code=COMMON+'static const unsigned refs[]={'+(','.join(str(x)for x in sorted(refs))if refs else'0')+'};\n'
    code+='static const unsigned ref_count='+str(len(refs))+';\nstatic const unsigned char disk[]={'+','.join(str(x)for x in binary)+'};\n'
    install='''{const IMAGE_DOS_HEADER*dos=(const IMAGE_DOS_HEADER*)disk;const IMAGE_NT_HEADERS32*nt=(const IMAGE_NT_HEADERS32*)(disk+dos->e_lfanew);const IMAGE_SECTION_HEADER*sec=IMAGE_FIRST_SECTION(nt);unsigned char*image;unsigned site,value,j;
image=VirtualAlloc(NULL,nt->OptionalHeader.SizeOfImage,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image)return 5;memcpy(image,disk,nt->OptionalHeader.SizeOfHeaders);
for(j=0;j<nt->FileHeader.NumberOfSections;j++)memcpy(image+sec[j].VirtualAddress,disk+sec[j].PointerToRawData,sec[j].SizeOfRawData);
for(j=0;j<ref_count;j++){site=(unsigned)image+refs[j];memcpy(&value,(void*)site,4);value=(unsigned)image+value-nt->OptionalHeader.ImageBase;memcpy((void*)site,&value,4);}fn=image+ENTRY_RVA;}'''.replace('ENTRY_RVA',str(entry-base))
    # Embedded, hash-checked immutable image data avoids rereading a mutable
    # filename between cases; only the checked closed graph is invoked.
    return code+DRIVER.replace('INSTALL',install),dict(entry=hex(entry),entries=[hex(a)for a in sorted(roots)],instructions=sorted(instructions.items()),references=sorted(refs.items()),closure_sha256=sha(json.dumps(sorted(instructions.items())).encode()))

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE');p.add_argument('--output',type=Path,required=True);p.add_argument('--candidate',type=Path);p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);args=p.parse_args()
    if bool(args.recompiled)!=bool(args.symbols):p.error('Supply immutable PE and hash-bound symbols together')
    out=args.output.resolve();out.mkdir(parents=True,exist_ok=True);paths=[SOURCE,SOURCE.with_suffix('.h'),Path(__file__),Path(q.__file__),ROOT/'src/carma2/hooks/c2_hooks.h',ROOT/'src/brender/include/brender/brender.h']+([args.candidate]if args.candidate else[]);before={str(x):sha(x.read_bytes())for x in paths}
    original=args.original.read_bytes();assert sha(original)==HASH;original_code,original_meta=physical(original,0x533410);body=(args.candidate or SOURCE).read_text();native_code=native(body);(out/'native.c').write_text(native_code);(out/'original.c').write_text(original_code);matching=None
    if args.recompiled:
        rebuilt=args.recompiled.read_bytes();symbols=json.loads(args.symbols.read_text());assert symbols['original_sha256']==HASH and symbols['recompiled_sha256']==sha(rebuilt);addresses={s['orig']:s['recomp']for s in symbols['symbols']};code,matching=physical(rebuilt,addresses[0x533410]);(out/'matching.c').write_text(code);mapping=args.recompiled.with_suffix('.map');text=mapping.read_text();names={int(a,16):n for n,a in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+',text,re.M)};matching.update(pe_sha256=sha(rebuilt),symbols_sha256=sha(args.symbols.read_bytes()),map_sha256=sha(mapping.read_bytes()),function_names={a:names.get(int(a,16),'actual private executable body')for a in matching['entries']})
    inc=[ROOT/x for x in ['src/brender/core/math','src/brender/include','src/carma2/hooks','src/carma2/include']];m5=ROOT/'build-tools/msvc500sp3';env5=dict(os.environ);env5.update(INCLUDE=str(m5/'include'),LIB=str(m5/'lib'));env5['PATH']=str(m5/'bin')+os.pathsep+str(m5/'redist')+os.pathsep+env5['PATH'];caller=q.compile_caller(out,env5)
    r=ROOT/'build-tools/msvc-modern';ms=sorted((r/'VC/Tools/MSVC').iterdir())[-1];sdk=r/'Windows Kits/10';version=sorted((sdk/'Include').iterdir())[-1].name;env=dict(os.environ);env['PATH']=str(ms/'bin/Hostx64/x86')+os.pathsep+env['PATH'];env['INCLUDE']=os.pathsep.join(map(str,[ms/'include',*[sdk/f'Include/{version}/{x}'for x in ['ucrt','shared','um']]]));env['LIB']=os.pathsep.join(map(str,[ms/'lib/x86',*[sdk/f'Lib/{version}/{x}/x86'for x in ['ucrt','um']]]));gcc=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'
    commands={'msvc5':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(x)for x in inc],str(out/'native.c'),'/Fo'+str(out/'msvc5.obj'),'/Fe'+str(out/'msvc5.exe'),str(out/'caller.obj')],'msvc':[str(ms/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(x)for x in inc],str(out/'native.c'),'/Fo'+str(out/'msvc.obj'),'/Fe'+str(out/'msvc.exe'),str(out/'caller.obj')],'mingw':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation',*['-I'+str(x)for x in inc],str(out/'native.c'),str(out/'caller.obj'),'-o',str(out/'mingw.exe')]}
    for tag in ['original']+(['matching']if matching else[]):commands[tag]=[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation',*['-I'+str(x)for x in inc],str(out/f'{tag}.c'),str(out/'caller.obj'),'-o',str(out/f'{tag}.exe')]
    for tag,cmd in commands.items():
        run=subprocess.run(cmd,cwd=out,env=env5 if tag=='msvc5'else env if tag=='msvc'else None,capture_output=True,text=True);(out/f'{tag}-build.txt').write_text(run.stdout+run.stderr);assert not run.returncode,run.stdout+run.stderr
    cases=fixtures();rows=[]
    for index,c in enumerate(cases):
        fixture=out/f'case-{index}.bin';fixture.write_bytes(c['data']);run=subprocess.run([str(out/'original.exe'),str(fixture)],capture_output=True);assert not run.returncode and len(run.stdout)==512;expected=run.stdout
        for tag in ['msvc5','msvc','mingw']+(['matching']if matching else[]):
            run=subprocess.run([str(out/f'{tag}.exe'),str(fixture)],capture_output=True)
            if run.returncode or run.stdout!=expected:
                (out/f'failure-{index}-{tag}.expected.bin').write_bytes(expected);(out/f'failure-{index}-{tag}.actual.bin').write_bytes(run.stdout);raise AssertionError((index,c['label'],tag,run.returncode))
            rows.append(dict(case=index,label=c['label'],compiler=tag,all_bytes_match=True,output_sha256=sha(run.stdout)))
    after={str(x):sha(x.read_bytes())for x in paths};assert before==after
    report=dict(original_sha256=HASH,scenarios=222,comparisons=len(rows),fixture_input_sha256=FIXTURE_SHA256,source_before_sha256=before,source_after_sha256=after,sources_stable=True,native_harness_sha256=sha(native_code.encode()),physical_original=original_meta,matching=matching,commands=dict(caller=caller,**commands),results=rows,limitations=__doc__);(out/'verification.json').write_text(json.dumps(report,indent=2)+'\n');(out/'verifier.py').write_bytes(Path(__file__).read_bytes());print('PASS',222,len(rows))
if __name__=='__main__':main()
