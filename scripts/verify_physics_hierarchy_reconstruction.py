"""Prove six genuine hierarchy preparation entries against physical original x87.

Executes the hash-checked original closed graph at PC53, actual native production
bodies under MSVC5/modern MSVC/MinGW, and optionally immutable rebuilt PE/map.
Full fixed arena, extent/epoch globals and raw returned energy are compared.
No game entry, imports, TLS or callback substitutes execute. Requires Capstone5
and the repository's scoped x86 compilers. Public float bytes are not normalized.
"""
import argparse,hashlib,json,os,struct,subprocess
from pathlib import Path
from verify_box_pick_reconstruction import ROOT,HASH,words,fb
from verify_tinted_native_reconstruction import extract
SOURCE=ROOT/'src/carma2/newcommon/49-physics.c'
GEOMETRY=ROOT/'src/carma2/newcommon/34-finteray.c'
MATRIX=ROOT/'src/brender/core/math/matrix34.c'
ENTRIES=[('GetWorldBoundingBoxForObject',0x4b9ce0,339),('HierarchyCalcEnergy',0x4b7290,127),('SetListDoingNothingFlag',0x4b9eb0,55),('SetGroupDoingNothingFlag',0x4b9e40,106),('SetHierarchyDoingNothingFlag',0x4b9ef0,78),('MakeObjectListDoSomething',0x4b9f40,67),('GetNewBoundingBox',0x45f5b0,213),('BrMatrix34ApplyP',0x5334a0,116)]
PREFIX=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "49-physics.h"
#include "34-finteray.h"
#include "matrix34.h"
#define A(x,y) A->m[x][y]
#define B(x,y) B->m[x][y]
#define C(x,y) C->m[x][y]
int gFace_num__car;
br_scalar gPhysics_bounds_max_x,gPhysics_bounds_min_x;
static unsigned char* arena;
typedef void(C2_HOOK_FASTCALL*bounds_fn)(tPhysics_object*,br_bounds*);
typedef br_scalar(C2_HOOK_FASTCALL*energy_fn)(tPhysics_object*);
typedef void(C2_HOOK_FASTCALL*flag_fn)(tPhysics_object*,tU8);
typedef void(C2_HOOK_FASTCALL*wake_fn)(tPhysics_object*);
'''
DRIVER=r'''
int C2_HOOK_CDECL main(int argc,char**argv){
 FILE*f;unsigned h[7],result=0,globals[3];tPhysics_object*object;br_bounds*bounds;volatile br_scalar energy;
 if(argc!=ARGS)return 2;f=fopen(argv[1],"rb");if(!f)return 3;
 arena=(unsigned char*)VirtualAlloc((void*)0x20000000,0x4000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(!arena)return 4;
 if(fread(h,sizeof(h),1,f)!=1||fread(arena,0x4000,1,f)!=1)return 5;fclose(f);
 gFace_num__car=(int)h[4];memcpy(&gPhysics_bounds_max_x,h+5,4);memcpy(&gPhysics_bounds_min_x,h+6,4);
 INSTALL
 object=h[1]?(tPhysics_object*)(arena+h[1]):NULL;bounds=h[2]?(br_bounds*)(arena+h[2]):NULL;
 _controlfp(_PC_53,_MCW_PC);
 switch(h[0]){
 case 0:BOUND(object,bounds);break;
 case 1:energy=ENERGY(object);memcpy(&result,(const void*)&energy,4);break;
 case 2:LIST(object,(tU8)h[3]);break;
 case 3:GROUP(object,(tU8)h[3]);break;
 case 4:HIERARCHY(object,(tU8)h[3]);break;
 case 5:WAKE(object);break;
 default:return 6;
 }
 globals[0]=(unsigned)gFace_num__car;memcpy(globals+1,&gPhysics_bounds_max_x,4);memcpy(globals+2,&gPhysics_bounds_min_x,4);
 _setmode(_fileno(stdout),_O_BINARY);fwrite(&result,4,1,stdout);fwrite(globals,sizeof(globals),1,stdout);fwrite(arena,0x4000,1,stdout);return 0;
}
'''
def native_harness():
    s=PREFIX;body=SOURCE.read_text();geo=GEOMETRY.read_text()
    for name in ('PhysicsScalarNaN','PhysicsLessOrUnordered','PhysicsGreaterOrdered','PhysicsObjectEnergy','PhysicsAccumulateEnergy'):s+=extract(body,name)+'\n'
    s+=extract(MATRIX.read_text(),'BrMatrix34ApplyP')+'\n'+extract(geo,'FinterayScalar')+'\n'+extract(geo,'FinterayBoundsAdd')+'\n'+extract(geo,'GetNewBoundingBox')+'\n'
    for name,a,n in ENTRIES[:6]:s+=extract(body,name)+'\n'
    d=DRIVER.replace('ARGS','2').replace('INSTALL','')
    for key,(name,a,n) in zip(('BOUND','ENERGY','LIST','GROUP','HIERARCHY','WAKE'),ENTRIES):d=d.replace(key,name)
    return s+d

def physical_harness(binary,addresses):
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_MEM,CS_OP_IMM
    nt=struct.unpack_from('<I',binary,60)[0];base=struct.unpack_from('<I',binary,nt+52)[0];tab=nt+24+struct.unpack_from('<H',binary,nt+20)[0]
    segments=[(*struct.unpack_from('<IIII',binary,tab+40*i+8),struct.unpack_from('<I',binary,tab+40*i+36)[0])for i in range(struct.unpack_from('<H',binary,nt+6)[0])]
    def raw(a):
        for vs,va,n,off,flags in segments:
            if flags&0x20 and va<=a-base<va+n:return binary[off+a-base-va:off+a-base-va+15]
        raise ValueError(f'Closure outside executable code {a:x}')
    cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;starts=[addresses[a]for name,a,n in ENTRIES];pending=list(starts);entries=set(starts);instructions={};refs={}
    globals_={addresses[0x65d010]:0,addresses[0x692dd0]:1,addresses[0x692dd4]:2}
    while pending:
        function=pending.pop();todo=[function];visited=set()
        while todo:
            a=todo.pop()
            if a in visited:continue
            visited.add(a);i=next(cs.disasm(raw(a),a,count=1));instructions[a]=i.bytes.hex()
            for op in i.operands:
                if op.type==CS_OP_MEM and not op.mem.base and not op.mem.index:
                    assert i.disp_size==4;refs[a-base+i.disp_offset]=globals_.get(op.mem.disp,3)
            if i.mnemonic=='call':
                assert i.operands[0].type==CS_OP_IMM,'No controlled lower call substitutes permitted'
                target=i.operands[0].imm;raw(target)
                if target not in entries:entries.add(target);pending.append(target)
            if i.mnemonic=='ret':continue
            if i.mnemonic.startswith('j'):
                assert i.operands[0].type==CS_OP_IMM,'Unapproved indirect branch'
                todo.append(i.operands[0].imm)
                if i.mnemonic=='jmp':continue
            todo.append(a+i.size)
            assert len(instructions)<4000
    s=PREFIX+'static void* entry[6];\nstatic const unsigned refs[][2]={'+','.join('{'+str(a)+','+str(k)+'}'for a,k in sorted(refs.items()))+'};\n'
    s+='static const unsigned offsets[]={'+','.join(str(a-base)for a in starts[:6])+'};\n'
    s+=r'''
static int install(const char*path){
 FILE*f;unsigned char*disk,*image;unsigned size,i,site,value;IMAGE_DOS_HEADER*dos;IMAGE_NT_HEADERS32*nt;IMAGE_SECTION_HEADER*sec;
 f=fopen(path,"rb");if(!f)return 91;fseek(f,0,SEEK_END);size=(unsigned)ftell(f);rewind(f);disk=(unsigned char*)malloc(size);if(!disk||fread(disk,size,1,f)!=1)return 92;fclose(f);
 dos=(IMAGE_DOS_HEADER*)disk;nt=(IMAGE_NT_HEADERS32*)(disk+dos->e_lfanew);sec=IMAGE_FIRST_SECTION(nt);image=(unsigned char*)VirtualAlloc(NULL,nt->OptionalHeader.SizeOfImage,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image)return 93;
 memcpy(image,disk,nt->OptionalHeader.SizeOfHeaders);for(i=0;i<nt->FileHeader.NumberOfSections;i++)memcpy(image+sec[i].VirtualAddress,disk+sec[i].PointerToRawData,sec[i].SizeOfRawData);
 for(i=0;i<sizeof(refs)/sizeof(refs[0]);i++){site=(unsigned)image+refs[i][0];memcpy(&value,(void*)site,4);value=refs[i][1]==0?(unsigned)&gFace_num__car:refs[i][1]==1?(unsigned)&gPhysics_bounds_max_x:refs[i][1]==2?(unsigned)&gPhysics_bounds_min_x:(unsigned)image+value-nt->OptionalHeader.ImageBase;memcpy((void*)site,&value,4);}
 for(i=0;i<6;i++)entry[i]=image+offsets[i];free(disk);return 0;
}
'''
    d=DRIVER.replace('ARGS','3').replace('INSTALL','{int r=install(argv[2]);if(r)return r;}')
    for key,kind,i in (('BOUND','bounds_fn',0),('ENERGY','energy_fn',1),('LIST','flag_fn',2),('GROUP','flag_fn',3),('HIERARCHY','flag_fn',4),('WAKE','wake_fn',5)):d=d.replace(key,f'(({kind})entry[{i}])')
    return s+d,dict(entries=[hex(a)for a in sorted(entries)],instructions=len(instructions),closure_sha256=hashlib.sha256(json.dumps(sorted(instructions.items())).encode()).hexdigest(),references=sorted(refs.items()))

def make_case(mode=0,tree=1,output=0,flag=1,epoch=1,patch=(),globals_=(0,0),object_=0x1000,expect_fault=False):
    a=bytearray(b'\xa5'*0x4000)
    def wr(p,v):a[p:p+4]=words([v])
    def fl(p,v):a[p:p+4*len(v)]=struct.pack('<'+'f'*len(v),*v)
    def ptr(p,v):wr(p,0x20000000+v if v else 0)
    for i in range(6):
        p=0x1000+i*0x600;actor=0x400+i*100;a[p:p+0x4d8]=bytes(0x4d8);a[actor:actor+100]=bytes(100);ptr(p,actor)
        fl(p+4,[2+i,.25+i,.5+i,.75+i]);fl(p+0x24,[-1,-2,-3,1,2,3]);fl(p+0x68,[.25+i,-.5-i,1+i]);fl(p+0x74,[.5+i,.75+i,-.25-i])
        matrix=[1,0,0,0,1,0,0,0,1,i*3,i*.25,-i*.5];fl(p+0x8c,matrix);matrix[9]+=1;fl(actor+0x2c,matrix);a[p+0xec]=i%3;a[p+0x1df]=0x55+i;struct.pack_into('<H',a,p+0x178,17+i)
    if tree:
        ptr(0x1000+0x224,0x1600);ptr(0x1600+0x220,0x1c00);ptr(0x1600+0x224,0x2200)
    if tree==2:ptr(0x1000+0x220,0x2800);ptr(0x2800+0x224,0x2e00)
    if tree==3:
        ptr(0x1000+0x1d8,0x2800);ptr(0x2800+0x1d8,0x1000);ptr(0x2800+0x224,0x2e00)
    if tree==4:ptr(0x1000+0x1d8,0x2800)
    fl(0x3800,[-9,-9,-9,9,9,9])
    for p,v in patch:wr(p,v)
    return dict(header=[mode,object_,output,flag,epoch,*globals_],data=bytes(a),options=dict(mode=mode,tree=tree,output=output,flag=flag,epoch=epoch,patch=patch,globals=globals_,object=object_,expect_fault=expect_fault))

def scenarios():
    c=[]
    for mode in range(6):
        for tree in range(5):c.append(make_case(mode,tree))
    for mode in (1,2,5):c.append(make_case(mode,object_=0))
    for mode in (2,3,4):
        for flag in (0,2,127,128,255):c.append(make_case(mode,3,flag=flag))
    for epoch in (0,1,2,65535,65536,0x7fffffff,0x80000000,0xffffffff):c.append(make_case(5,2,epoch=epoch))
    for out in (0,0x3800,0x10f4,0x110c,0x1110,0x1024,0x1180):c.append(make_case(0,1,output=out))
    for value in (fb(.0025),fb(-.0025),fb(1e20),fb(-1e20),0x80000000,1,0x7f800000,0xff800000,0x7fc12345,0xffc54321,0x7f812345,0xff812345):
        for offset in (0x1024,0x1030,0x10b0,0x400+0x50):c.append(make_case(0,1,patch=[(offset,value)]))
        c.append(make_case(0,1,globals_=(value,value)))
        for offset in (0x1004,0x1008,0x1068,0x1074):c.append(make_case(1,1,patch=[(offset,value)]))
    for left in (0x7fc12345,0x7f812345,0xff812345):
        for right in (0x7fc54321,0x7f854321,0xff854321):
            for offsets in ((0x1004,0x1068),(0x1008,0x1074),(0x1068,0x1074)):c.append(make_case(1,1,patch=list(zip(offsets,(left,right)))))
    for velocity in ([fb(2**26),fb(1),fb(1)],[fb(1e20),fb(1e-20),fb(-1e20)]):c.append(make_case(1,2,patch=[(0x1068+i*4,v)for i,v in enumerate(velocity)]))
    for left,right in ((0x7fc12345,0x7fc54321),(0x7f812345,0xff854321),(0xffc12345,0x7f854321)):
        for offsets in ((0x1004,0x1604),(0x1604,0x1c04),(0x1004,0x2804)):c.append(make_case(1,2,patch=list(zip(offsets,(left,right)))))
    for mass in (-2.,0.,1e30):c.append(make_case(1,2,patch=[(0x1004,fb(mass)),(0x1008,fb(-1.))]))
    c.append(make_case(1,1,patch=[(0x1004,0x7fc54321),(0x1068,0x7fc12345)]))
    for mode in (0,3,4):c.append(make_case(mode,object_=0,expect_fault=True))
    c.append(make_case(0,patch=[(0x1000,0)],expect_fault=True))
    return c

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE');p.add_argument('--output',type=Path,default=ROOT/'build-audit/physics-hierarchy-next/tests');p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);p.add_argument('--case-index',type=int,action='append');args=p.parse_args()
    if bool(args.recompiled)!=bool(args.symbols):p.error('Supply both immutable PE and symbol map')
    out=args.output.resolve();out.mkdir(parents=True,exist_ok=True);binary=args.original.read_bytes();assert hashlib.sha256(binary).hexdigest()==HASH
    addresses={a:a for name,a,n in ENTRIES};addresses.update({v:v for v in (0x65d010,0x692dd0,0x692dd4)});native=native_harness();physical,meta=physical_harness(binary,addresses);(out/'harness.c').write_text(native);(out/'original.c').write_text(physical)
    inc=[ROOT/v for v in ('src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/carma2/platform','src/carma2/s3/include','src/brender/core/math','src/brender/include')];r=ROOT/'build-tools/msvc-modern';ms=sorted((r/'VC/Tools/MSVC').iterdir())[-1];sdk=r/'Windows Kits/10';v=sorted((sdk/'Include').iterdir())[-1].name;env=dict(os.environ);env['PATH']=str(ms/'bin/Hostx64/x86')+os.pathsep+env['PATH'];env['INCLUDE']=os.pathsep.join(map(str,[ms/'include',*[sdk/f'Include/{v}/{part}' for part in ('ucrt','shared','um')]]));env['LIB']=os.pathsep.join(map(str,[ms/'lib/x86',*[sdk/f'Lib/{v}/{part}/x86' for part in ('ucrt','um')]]));m5=ROOT/'build-tools/msvc500sp3';e5=dict(os.environ);e5['PATH']=str(m5/'bin')+os.pathsep+str(m5/'redist')+os.pathsep+e5['PATH'];e5['INCLUDE']=str(m5/'include');e5['LIB']=str(m5/'lib');gcc=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'
    commands={
      'msvc':[str(ms/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(x)for x in inc],str(out/'harness.c'),'/Fo'+str(out/'msvc.obj'),'/Fe'+str(out/'msvc.exe')],
      'msvc5':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(x)for x in inc],str(out/'harness.c'),'/Fo'+str(out/'msvc5.obj'),'/Fe'+str(out/'msvc5.exe')],
      'mingw':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation',*['-I'+str(x)for x in inc],str(out/'harness.c'),'-o',str(out/'mingw.exe')],
      'original':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-DREC2_MATCHING',*['-I'+str(x)for x in inc],str(out/'original.c'),'-o',str(out/'original.exe')]}
    linked=None
    if args.recompiled:
        rebuilt=args.recompiled.read_bytes();m=json.loads(args.symbols.read_text());assert m['format']==1 and m['original_sha256']==HASH and m['recompiled_sha256']==hashlib.sha256(rebuilt).hexdigest();a={v['orig']:v['recomp']for v in m['symbols']};s,linked=physical_harness(rebuilt,a);(out/'matching.c').write_text(s);commands['matching']=[*commands['original'][:-3],str(out/'matching.c'),'-o',str(out/'matching.exe')];linked.update(recompiled_sha256=hashlib.sha256(rebuilt).hexdigest(),symbols_sha256=hashlib.sha256(args.symbols.read_bytes()).hexdigest())
    for label,cmd in commands.items():
        build=subprocess.run(cmd,cwd=out,env=env if label=='msvc' else e5 if label=='msvc5' else None,text=True,capture_output=True);(out/(label+'-build.txt')).write_text(build.stdout+build.stderr);assert not build.returncode,build.stdout+build.stderr
    cases=scenarios();results=[]
    for i,c in enumerate(cases):
        if args.case_index is not None and i not in args.case_index:continue
        fixture=out/f'case-{i:03d}.bin';fixture.write_bytes(words(c['header'])+c['data']);run=subprocess.run([str(out/'original.exe'),str(fixture),str(args.original.resolve())],capture_output=True);expected=run.stdout;exit_code=run.returncode&0xffffffff;assert exit_code==(0xc0000005 if c['options']['expect_fault']else 0),f'Original case{i} exits{run.returncode:x}'
        for label in ('msvc5','msvc','mingw',*(['matching']if linked else [])):
            run=subprocess.run([str(out/(label+'.exe')),str(fixture),*([str(args.recompiled.resolve())]if label=='matching' else [])],capture_output=True);actual=run.stdout
            if actual!=expected or (run.returncode&0xffffffff)!=exit_code:
                prefix=out/f'failure-{label}-{i}';prefix.with_suffix('.expected.bin').write_bytes(expected);prefix.with_suffix('.actual.bin').write_bytes(actual);prefix.with_suffix('.json').write_text(json.dumps(dict(options=c['options'],first_difference=next((j for j,(x,y)in enumerate(zip(expected,actual))if x!=y),None),actual_exit=run.returncode,expected_size=len(expected),actual_size=len(actual)),indent=2));raise AssertionError(f'{label} hierarchy{i} differs:{prefix}')
            results.append(dict(case_index=i,compiler=label,all_bytes_match=True,exit_code=hex(exit_code),output_sha256=hashlib.sha256(actual).hexdigest()))
        if (i+1)%40==0:print(f'Passed {i+1}/{len(cases)}',flush=True)
    sources=[SOURCE,SOURCE.with_suffix('.h'),GEOMETRY,MATRIX];report=dict(original_sha256=HASH,scenes=len(cases),comparisons=len(results),all_cases_passed=True,complete_suite=args.case_index is None,entries=ENTRIES,physical_original=meta,source_sha256={str(s.relative_to(ROOT)):hashlib.sha256(s.read_bytes()).hexdigest()for s in sources},harness_sha256=hashlib.sha256(native.encode()).hexdigest(),commands=commands,results=results,limitations=__doc__,identity_caveat='Bounds/energy/group names map established49 placeholders by original purpose/callers; list/hierarchy names map known original SetCollisionInfoDoNothing/Childs names. Inlined Objects/CombineBounds helpers are not counted.')
    if linked:report['matching_physical']=linked
    (out/'verification.json').write_text(json.dumps(report,indent=2)+'\n');print(f'PASS: {len(cases)} scenes/{len(results)} comparisons')

if __name__=='__main__':main()
