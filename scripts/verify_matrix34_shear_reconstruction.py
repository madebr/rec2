"""Independent actual physical original/native3 PreShearX and dependencies proof.
Full arena and both genuine scratch matrix globals retain raw bits, PC53 and
full/partial overlap. No original game/import/TLS or public normalization.
"""
import argparse,hashlib,json,os,struct,subprocess
from pathlib import Path
from verify_box_pick_reconstruction import ROOT,HASH,words,fb
from verify_tinted_native_reconstruction import extract
SOURCE=ROOT/'src/brender/core/math/matrix34.c'
GEOMETRY=SOURCE
ENTRIES=[('BrMatrix34PreShearX',0x533fd0,159)]
PREFIX=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "matrix34.h"
#define M(x,y) mat->m[x][y]
#define A(x,y) A->m[x][y]
#define B(x,y) B->m[x][y]
#define C(x,y) C->m[x][y]
#undef BR_MAC3
#define BR_MAC3(a,b,c,d,e,f) ((a)*(b)+(c)*(d)+(e)*(f))
static unsigned char* arena;
#define mattmp1 (*(br_matrix34*)(arena+0x100))
#define mattmp2 (*(br_matrix34*)(arena+0x130))
'''
DRIVER=r'''
int C2_HOOK_CDECL main(int argc,char**argv){
 FILE*f;unsigned h[3];if(argc!=ARGS)return 2;f=fopen(argv[1],"rb");if(!f)return 3;
 arena=(unsigned char*)VirtualAlloc((void*)0x20000000,0x1000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(!arena)return 4;
 if(fread(h,sizeof(h),1,f)!=1||fread(arena,0x200,1,f)!=1)return 5;fclose(f);INSTALL
 _controlfp(_PC_53,_MCW_PC);
 ((void(C2_HOOK_CDECL*)(br_matrix34*,unsigned,unsigned))entries[0])((br_matrix34*)(arena+h[0]),h[1],h[2]);
 _setmode(_fileno(stdout),_O_BINARY);fwrite(arena,0x200,1,stdout);return 0;
}
'''

def native_harness():
 s=PREFIX
 for n in ('BrMatrix34RawShearX','BrMatrix34Copy','BrMatrix34Mul','BrMatrix34ShearX',*([ENTRIES[0][0]]if ENTRIES[0][0]!='BrMatrix34ShearX' else [])):s+=('#if !defined(_MSC_VER) || _MSC_VER >= 1300\n'+extract(SOURCE.read_text(),n)+'\n#endif\n' if n=='BrMatrix34RawShearX' else extract(SOURCE.read_text(),n)+'\n')
 return s+'static void* entries[1];\n'+DRIVER.replace('ARGS','2').replace('INSTALL','entries[0]=(void*)'+ENTRIES[0][0]+';')

def physical_harness(binary,addresses):
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_MEM,CS_OP_IMM
    nt=struct.unpack_from('<I',binary,60)[0];base=struct.unpack_from('<I',binary,nt+52)[0];tab=nt+24+struct.unpack_from('<H',binary,nt+20)[0]
    segments=[(*struct.unpack_from('<IIII',binary,tab+40*i+8),struct.unpack_from('<I',binary,tab+40*i+36)[0])for i in range(struct.unpack_from('<H',binary,nt+6)[0])]
    def raw(a):
        for vs,va,n,off,flags in segments:
            if flags&0x20 and va<=a-base<va+n:return binary[off+a-base-va:off+a-base-va+15]
        raise ValueError(f'Closure outside executable code {a:x}')
    cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;starts=[addresses[a]for _,a,_ in ENTRIES];pending=list(starts);entries=set(starts);instructions={};refs={}
    while pending:
        function=pending.pop();todo=[function];visited=set()
        while todo:
            a=todo.pop()
            if a in visited:continue
            visited.add(a);i=next(cs.disasm(raw(a),a,count=1));instructions[a]=i.bytes.hex()
            for op in i.operands:
                if op.type==CS_OP_MEM and not op.mem.base and not op.mem.index:
                    assert i.disp_size==4;refs[a-base+i.disp_offset]=int(op.mem.disp==addresses[0x5964c0])
            for op in i.operands:
                if op.type==CS_OP_IMM and not i.mnemonic.startswith(('j','call')) and base<=op.imm<base+struct.unpack_from('<I',binary,nt+80)[0]:refs[a-base+i.imm_offset]=0
            if i.mnemonic=='call':
                assert i.operands[0].type==CS_OP_IMM,'No replacement lower call boundary permitted'
                target=i.operands[0].imm;raw(target)
                if target not in entries:entries.add(target);pending.append(target)
            if i.mnemonic=='ret':continue
            if i.mnemonic.startswith('j'):
                assert i.operands[0].type==CS_OP_IMM,'Unapproved indirect branch'
                todo.append(i.operands[0].imm)
                if i.mnemonic=='jmp':continue
            todo.append(a+i.size);assert len(instructions)<4000
    s=PREFIX+'#define GLOBAL1_BASE '+str(addresses[0x6b09c0])+'u\n#define GLOBAL2_BASE '+str(addresses[0x6b09f0])+'u\n'+'static void* entries[1];\nstatic const unsigned refs[][2]={'+','.join('{'+str(a)+','+str(k)+'}'for a,k in sorted(refs.items()))+'};\n'
    s+='static const unsigned offsets[1]={'+','.join(str(a-base)for a in starts[:1])+'};\n'
    s+=r'''
static int install(const char*path){
 FILE*f;unsigned char*disk,*image;unsigned size,i,site,value,ref_count;IMAGE_DOS_HEADER*dos;IMAGE_NT_HEADERS32*nt;IMAGE_SECTION_HEADER*sec;
 f=fopen(path,"rb");if(!f)return 91;fseek(f,0,SEEK_END);size=(unsigned)ftell(f);rewind(f);disk=(unsigned char*)malloc(size);if(!disk||fread(disk,size,1,f)!=1)return 92;fclose(f);
 dos=(IMAGE_DOS_HEADER*)disk;nt=(IMAGE_NT_HEADERS32*)(disk+dos->e_lfanew);sec=IMAGE_FIRST_SECTION(nt);image=(unsigned char*)VirtualAlloc(NULL,nt->OptionalHeader.SizeOfImage,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image)return 93;
 memcpy(image,disk,nt->OptionalHeader.SizeOfHeaders);for(i=0;i<nt->FileHeader.NumberOfSections;i++)memcpy(image+sec[i].VirtualAddress,disk+sec[i].PointerToRawData,sec[i].SizeOfRawData);
 ref_count=(unsigned)(sizeof(refs)/sizeof(refs[0]));for(i=0;i<ref_count;i++){site=(unsigned)image+refs[i][0];memcpy(&value,(void*)site,4);if(value>=GLOBAL1_BASE&&value<GLOBAL1_BASE+48)value=(unsigned)arena+0x100+value-GLOBAL1_BASE;else if(value>=GLOBAL2_BASE&&value<GLOBAL2_BASE+48)value=(unsigned)arena+0x130+value-GLOBAL2_BASE;else value=(unsigned)image+value-nt->OptionalHeader.ImageBase;memcpy((void*)site,&value,4);}
 for(i=0;i<1;i++)entries[i]=image+offsets[i];free(disk);return 0;
}
'''
    s+=DRIVER.replace('ARGS','3').replace('INSTALL','{int r=install(argv[2]);if(r)return r;}')
    return s,dict(entries=[hex(a)for a in sorted(entries)],instruction_count=len(instructions),closure_sha256=hashlib.sha256(json.dumps(sorted(instructions.items())).encode()).hexdigest(),references=sorted(refs.items()))

def scenarios():
 cases=[]
 special=[0,0x80000000,1,0x80000001,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc12345,0xffc54321,0x7f812345,0xff854321]
 for offset in (0,0x100,0x104,0x108,0x12c,0x130,0x134):
  for sy,sz in [(fb(.25),fb(-.125)),(0,0),(fb(2),fb(-3))]+[(v,0)for v in special]:
   data=bytearray(b'\xa5'*0x200)
   for at in (0,0x100,0x130):data[at:at+48]=struct.pack('<12f',1,.5,-.25,.125,2,.5,-.5,.25,1,2,-4,8)
   cases.append(dict(data=bytes(data),header=[offset,sy,sz],options=dict(offset=offset,sy=sy,sz=sz)))
 return cases

def main():
    global ENTRIES
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE');p.add_argument('--output',type=Path,default=ROOT/'build-audit/matrix-shear-tests');p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);p.add_argument('--case-index',type=int,action='append');p.add_argument('--entry',choices=['pre','shear','post'],default='pre');args=p.parse_args();ENTRIES=[dict(pre=('BrMatrix34PreShearX',0x533fd0,159),shear=('BrMatrix34ShearX',0x532b10,55),post=('BrMatrix34PostShearX',0x534070,159))[args.entry]]
    if bool(args.recompiled)!=bool(args.symbols):p.error('Supply both immutable PE/map')
    out=args.output.resolve();out.mkdir(parents=True,exist_ok=True);binary=args.original.read_bytes();assert hashlib.sha256(binary).hexdigest()==HASH
    a={x:x for _,x,_ in ENTRIES};a[0x5964c0]=0x5964c0;a[0x6b09c0]=0x6b09c0;a[0x6b09f0]=0x6b09f0;native=native_harness();physical,meta=physical_harness(binary,a);(out/'harness.c').write_text(native);(out/'original.c').write_text(physical)
    inc=[SOURCE.parent]+[ROOT/v for v in ('src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/carma2/platform','src/carma2/s3/include','src/brender/include')];r=ROOT/'build-tools/msvc-modern';ms=sorted((r/'VC/Tools/MSVC').iterdir())[-1];sdk=r/'Windows Kits/10';v=sorted((sdk/'Include').iterdir())[-1].name;env=dict(os.environ);env['PATH']=str(ms/'bin/Hostx64/x86')+os.pathsep+env['PATH'];env['INCLUDE']=os.pathsep.join(map(str,[ms/'include',*[sdk/f'Include/{v}/{part}'for part in ('ucrt','shared','um')]]));env['LIB']=os.pathsep.join(map(str,[ms/'lib/x86',*[sdk/f'Lib/{v}/{part}/x86'for part in ('ucrt','um')]]));m5=ROOT/'build-tools/msvc500sp3';e5=dict(os.environ);e5['PATH']=str(m5/'bin')+os.pathsep+str(m5/'redist')+os.pathsep+e5['PATH'];e5['INCLUDE']=str(m5/'include');e5['LIB']=str(m5/'lib');gcc=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'
    commands={
      'msvc':[str(ms/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(x)for x in inc],str(out/'harness.c'),'/Fo'+str(out/'msvc.obj'),'/Fe'+str(out/'msvc.exe')],
      'msvc5':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(x)for x in inc],str(out/'harness.c'),'/Fo'+str(out/'msvc5.obj'),'/Fe'+str(out/'msvc5.exe')],
      'mingw':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation',*['-I'+str(x)for x in inc],str(out/'harness.c'),'-o',str(out/'mingw.exe')],
      'original':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-DREC2_MATCHING',*['-I'+str(x)for x in inc],str(out/'original.c'),'-o',str(out/'original.exe')]}
    linked=None
    if args.recompiled:
        rebuilt=args.recompiled.read_bytes();m=json.loads(args.symbols.read_text());assert m['format']==1 and m['original_sha256']==HASH and m['recompiled_sha256']==hashlib.sha256(rebuilt).hexdigest();a={v['orig']:v['recomp']for v in m['symbols']};s,linked=physical_harness(rebuilt,a);(out/'matching.c').write_text(s);commands['matching']=[*commands['original'][:-3],str(out/'matching.c'),'-o',str(out/'matching.exe')];linked.update(recompiled_sha256=hashlib.sha256(rebuilt).hexdigest(),symbols_sha256=hashlib.sha256(args.symbols.read_bytes()).hexdigest())
    for label,cmd in commands.items():
        build=subprocess.run(cmd,cwd=out,env=env if label=='msvc'else e5 if label=='msvc5'else None,text=True,capture_output=True);(out/(label+'-build.txt')).write_text(build.stdout+build.stderr);assert not build.returncode,build.stdout+build.stderr
    cases=scenarios();results=[]
    for i,c in enumerate(cases):
        if args.case_index is not None and i not in args.case_index:continue
        fixture=out/f'case-{i:03d}.bin';fixture.write_bytes(words(c['header'])+c['data']);run=subprocess.run([str(out/'original.exe'),str(fixture),str(args.original.resolve())],capture_output=True);expected=run.stdout;assert run.returncode==0,f'Original case{i} exits {run.returncode:x}'
        for label in ('msvc5','msvc','mingw',*(['matching']if linked else [])):
            run=subprocess.run([str(out/(label+'.exe')),str(fixture),*([str(args.recompiled.resolve())]if label=='matching'else [])],capture_output=True);actual=run.stdout
            if actual!=expected or run.returncode:
                prefix=out/f'failure-{label}-{i}';prefix.with_suffix('.expected.bin').write_bytes(expected);prefix.with_suffix('.actual.bin').write_bytes(actual);prefix.with_suffix('.json').write_text(json.dumps(dict(options=c['options'],first_difference=next((j for j,(x,y)in enumerate(zip(expected,actual))if x!=y),None),exit=run.returncode,expected_size=len(expected),actual_size=len(actual)),indent=2));raise AssertionError(f'{label} shape{i} differs:{prefix}')
            results.append(dict(case_index=i,compiler=label,all_bytes_match=True,output_sha256=hashlib.sha256(actual).hexdigest()))
        if (i+1)%30==0:print(f'Passed {i+1}/{len(cases)}',flush=True)
    report=dict(original_sha256=HASH,scenes=len(set(r['case_index']for r in results)),total_scenarios=len(cases),comparisons=len(results),complete_suite=args.case_index is None,entries=ENTRIES,physical_original=meta,verifier_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),source_sha256={str(s.relative_to(ROOT)):hashlib.sha256(s.read_bytes()).hexdigest()for s in (SOURCE,ROOT/'src/brender/core/math/matrix34.h')},commands=commands,results=results,limitations=__doc__)
    if linked:report['matching_physical']=linked
    (out/'verification.json').write_text(json.dumps(report,indent=2)+'\n');print(f"PASS: {report['scenes']} scenes/{len(results)} comparisons")

if __name__=='__main__':main()
