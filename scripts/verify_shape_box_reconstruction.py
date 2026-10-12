"""Prove the real box/sphere primitive stage of the full0x4228b0 wall-contact closure.

Physical hash-checked original x86 and actual production native3 bodies compare
full fixed arenas and raw return/output bits at PC53. Optional linked PE/map
must be hash-bound. Five real entries execute with actual lower dependencies;
no geometry replacement stubs, game startup/imports/TLS or output normalization.
Requires Capstone5 and scoped MSVC5SP3/modern x86 MSVC/i686 MinGW.
Full wall consumer remains unfinished until all later contact branches pass.
"""
import argparse,hashlib,json,os,struct,subprocess,random
from pathlib import Path
from verify_box_pick_reconstruction import ROOT,HASH,words,fb
from verify_tinted_native_reconstruction import extract
SOURCE=ROOT/'src/carma2/newcommon/34-finteray.c'
GEOMETRY=SOURCE
ENTRIES=[('LineBoxColl',0x460bd0,454),('SphereBoxIntersection',0x460da0,106),('LineBoxCollWithSphere',0x460e10,336),('LineSphereColl',0x460f60,582),('LineSphereCollWithSphere',0x4611b0,347)]
PREFIX=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "34-finteray.h"
#include "c2_math.h"
int gPling_materials;
static unsigned char* arena;
typedef int(C2_HOOK_FASTCALL*sphere_raw_fn)(br_bounds*,br_vector3*,unsigned,br_vector3*);
'''
DRIVER=r'''
int C2_HOOK_CDECL main(int argc,char**argv){
 FILE*f;unsigned h[8],result;if(argc!=ARGS)return 2;f=fopen(argv[1],"rb");if(!f)return 3;
 arena=(unsigned char*)VirtualAlloc((void*)0x20000000,0x1000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(!arena)return 4;
 if(fread(h,sizeof(h),1,f)!=1||fread(arena,0x400,1,f)!=1)return 5;fclose(f);INSTALL
 _controlfp(_PC_53,_MCW_PC);
 if(h[0]==0)result=(unsigned)((int(C2_HOOK_FASTCALL*)(br_vector3*,br_vector3*,br_bounds*,br_vector3*,br_scalar*))entries[0])((br_vector3*)(arena+h[1]),(br_vector3*)(arena+h[2]),(br_bounds*)(arena+h[3]),(br_vector3*)(arena+h[4]),(br_scalar*)(arena+h[6]));
 else if(h[0]==1)result=(unsigned)((sphere_raw_fn)entries[1])((br_bounds*)(arena+h[3]),(br_vector3*)(arena+h[2]),h[7],(br_vector3*)(arena+h[4]));
 else if(h[0]==2)result=(unsigned)((int(C2_HOOK_FASTCALL*)(br_vector3*,br_vector3*,br_bounds*,br_vector3*,br_vector3*,br_scalar*))entries[2])((br_vector3*)(arena+h[1]),(br_vector3*)(arena+h[2]),(br_bounds*)(arena+h[3]),(br_vector3*)(arena+h[4]),(br_vector3*)(arena+h[5]),(br_scalar*)(arena+h[6]));
 else result=(unsigned)((int(C2_HOOK_FASTCALL*)(br_vector3*,br_vector3*,tCollision_shape_sphere_data*,br_vector3*,br_vector3*,br_scalar*))entries[h[0]])((br_vector3*)(arena+h[1]),(br_vector3*)(arena+h[2]),(tCollision_shape_sphere_data*)(arena+h[3]),(br_vector3*)(arena+h[4]),h[5]==0xffffffffu?NULL:(br_vector3*)(arena+h[5]),(br_scalar*)(arena+h[6]));
 _setmode(_fileno(stdout),_O_BINARY);fwrite(&result,4,1,stdout);fwrite(arena,0x400,1,stdout);return 0;
}
'''

def native_harness():
 body=SOURCE.read_text();s=PREFIX
 for n in ('FinterayScalar','FinterayBoxNaN','FinterayBoxCopyScalar','FinterayBoxStoreScalar','FinterayShapeLoadScalar','LineBoxColl','SphereBoxIntersection','LineBoxCollWithSphere','LineSphereColl','LineSphereCollWithSphere'):s+=extract(body,n)+'\n'
 s+='static void* entries[5];\n'
 return s+DRIVER.replace('ARGS','2').replace('INSTALL','entries[0]=(void*)LineBoxColl;entries[1]=(void*)SphereBoxIntersection;entries[2]=(void*)LineBoxCollWithSphere;entries[3]=(void*)LineSphereColl;entries[4]=(void*)LineSphereCollWithSphere;')

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
    s=PREFIX+'static void* entries[5];\nstatic const unsigned refs[][2]={'+','.join('{'+str(a)+','+str(k)+'}'for a,k in sorted(refs.items()))+'};\n'
    s+='static const unsigned offsets[5]={'+','.join(str(a-base)for a in starts[:5])+'};\n'
    s+=r'''
static int install(const char*path){
 FILE*f;unsigned char*disk,*image;unsigned size,i,site,value;IMAGE_DOS_HEADER*dos;IMAGE_NT_HEADERS32*nt;IMAGE_SECTION_HEADER*sec;
 f=fopen(path,"rb");if(!f)return 91;fseek(f,0,SEEK_END);size=(unsigned)ftell(f);rewind(f);disk=(unsigned char*)malloc(size);if(!disk||fread(disk,size,1,f)!=1)return 92;fclose(f);
 dos=(IMAGE_DOS_HEADER*)disk;nt=(IMAGE_NT_HEADERS32*)(disk+dos->e_lfanew);sec=IMAGE_FIRST_SECTION(nt);image=(unsigned char*)VirtualAlloc(NULL,nt->OptionalHeader.SizeOfImage,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image)return 93;
 memcpy(image,disk,nt->OptionalHeader.SizeOfHeaders);for(i=0;i<nt->FileHeader.NumberOfSections;i++)memcpy(image+sec[i].VirtualAddress,disk+sec[i].PointerToRawData,sec[i].SizeOfRawData);
 for(i=0;i<sizeof(refs)/sizeof(refs[0]);i++){site=(unsigned)image+refs[i][0];memcpy(&value,(void*)site,4);value=refs[i][1]?(unsigned)&gPling_materials:(unsigned)image+value-nt->OptionalHeader.ImageBase;memcpy((void*)site,&value,4);}
 for(i=0;i<5;i++)entries[i]=image+offsets[i];free(disk);return 0;
}
'''
    s+=DRIVER.replace('ARGS','3').replace('INSTALL','{int r=install(argv[2]);if(r)return r;}')
    return s,dict(entries=[hex(a)for a in sorted(entries)],instruction_count=len(instructions),closure_sha256=hashlib.sha256(json.dumps(sorted(instructions.items())).encode()).hexdigest(),references=sorted(refs.items()))

def scene(mode=0,start=(0,0,0),end=(2,0,0),bounds=(-1,-1,-1,1,1,1),patch=(),**kw):
 a=bytearray(b'\xa5'*0x400)
 for off,v in [(0,start),(0x20,end),(0x40,bounds)]:a[off:off+4*len(v)]=struct.pack('<'+'f'*len(v),*v)
 if mode in (3,4):
  values=kw.get('sphere',(1,0,0,0,1));a[0x40:0x54]=struct.pack('<5f',*values)
 for off,v in patch:a[off:off+4]=words([v])
 h=[mode,kw.get('start_offset',0),kw.get('end_offset',0x20),kw.get('bounds_offset',0x40),kw.get('point',0x80),kw.get('normal',0xa0),kw.get('t',0xc0),kw.get('radius',fb(0.25))]
 return dict(header=h,data=bytes(a),options=dict(mode=mode,start=start,end=end,bounds=bounds,patch=patch,**kw))

def scenarios():
 cases=[]
 for mode in range(3):
  for axis in range(3):
   for sign in (-1,1):
    for start,end in [(0,0),(2,0),(2,3),(0,2),(1,1),(2,1),(2,-2),(1.004,1.004),(1.006,1.006)]:
     a=[0]*3;b=[0]*3;a[axis]=sign*start;b[axis]=sign*end;cases.append(scene(mode,start=a,end=b))
  for a,b in [([2,2,2],[0,0,0]),([2,2,0],[0,0,0]),([2,2,2],[1.004]*3),([1.004]*3,[1.004]*3),([1.006]*3,[1.006]*3)]:cases.append(scene(mode,start=a,end=b))
  for point in (0,4,0x20,0x24,0x40,0x44,0x4c,0xa0,0xc0):cases.append(scene(mode,start=[2,2,2],end=[0,0,0],point=point))
  for point in (0,4,0x20,0x24):cases.append(scene(mode,start=[0,0,0],end=[0,0,0],point=point))
  for t in (0,0x20,0x40,0x80,0xa0):cases.append(scene(mode,start=[2,2,2],end=[0,0,0],t=t))
  for normal in (0,0x20,0x40,0x80,0xc0):cases.append(scene(mode,start=[1.004]*3,end=[1.004]*3,normal=normal))
 specials=(0,0x80000000,1,0x80000001,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc12345,0xffc54321,0x7f812345,0xff854321)
 for mode in range(3):
  for off in (0,4,8,0x20,0x24,0x28,0x40,0x44,0x48,0x4c,0x50,0x54):
   for value in specials:cases.append(scene(mode,start=[2]*3,end=[0]*3,patch=[(off,value)]))
 for value in specials:cases.append(scene(1,radius=value,end=[1.5,1.5,1.5]))
 for left,right in ((0x7fc12345,0xffc12345),(0xffc12345,0x7fc12345),(0x7fc12345,0xffc54321),(0x7f812345,0xffc54321)):
  for mode in range(3):
   for axis in range(3):cases.append(scene(mode,patch=[(4*axis,left),(0x20+4*axis,right)]))
 rng=random.Random(0x460bd0)
 for i in range(60):
  a=[rng.uniform(-3,3)for _ in range(3)];b=[rng.uniform(-3,3)for _ in range(3)]
  for mode in range(3):cases.append(scene(mode,start=a,end=b))
 for mode in (3,4):
  for axis in range(3):
   for sign in (-1,1):
    for start,end in [(2,0),(2,3),(0,2),(1,1),(2,1),(2,-2),(1.001,1.001),(1.003,1.003),(1.005,1.005),(.5,.5),(.999,1.001),(-2,2)]:
     a=[0]*3;b=[0]*3;a[axis]=sign*start;b[axis]=sign*end;cases.append(scene(mode,start=a,end=b))
  for normal in (0xffffffff,0,4,0x20,0x24,0x40,0x44,0x4c,0x80,0xc0):
   if mode==4 and normal==0xffffffff:continue
   cases.append(scene(mode,start=[2,2,2],end=[0,0,0],normal=normal))
  for point in (0,4,0x20,0x24,0x40,0x44,0x4c,0xa0,0xc0):cases.append(scene(mode,start=[2,2,2],end=[0,0,0],point=point))
  for t in (0,0x20,0x40,0x44,0x50,0x80,0xa0):cases.append(scene(mode,start=[2,2,2],end=[0,0,0],t=t))
  for normal in (0,4,0x20,0x24,0x40,0x44,0x4c,0x80,0xc0):cases.append(scene(mode,start=[1.001,0,0],end=[1.001,0,0],normal=normal))
  for off in (0,4,8,0x20,0x24,0x28,0x40,0x44,0x48,0x4c,0x50):
   for value in specials:cases.append(scene(mode,start=[2,2,2],end=[0,0,0],patch=[(off,value)]))
  for i in range(30):cases.append(scene(mode,start=[rng.uniform(-3,3)for _ in range(3)],end=[rng.uniform(-3,3)for _ in range(3)],sphere=[rng.uniform(.1,2),*[rng.uniform(-.5,.5)for _ in range(3)],rng.uniform(.1,4)]))
 for mode in (3,4):
  for sign in (-1,1):
   for radius in (1.0,float('inf')):
    cases.append(scene(mode,start=[sign*float('inf'),0,0],end=[sign*float('inf'),0,0],sphere=[radius,0,0,0,float('inf')]))
 return cases

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE');p.add_argument('--output',type=Path,default=ROOT/'build-audit/shape-box-contact');p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);p.add_argument('--case-index',type=int,action='append');args=p.parse_args()
    if bool(args.recompiled)!=bool(args.symbols):p.error('Supply both immutable PE/map')
    out=args.output.resolve();out.mkdir(parents=True,exist_ok=True);binary=args.original.read_bytes();assert hashlib.sha256(binary).hexdigest()==HASH
    a={x:x for _,x,_ in ENTRIES};a[0x5964c0]=0x5964c0;native=native_harness();physical,meta=physical_harness(binary,a);(out/'harness.c').write_text(native);(out/'original.c').write_text(physical)
    inc=[ROOT/v for v in ('src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/carma2/platform','src/carma2/s3/include','src/brender/include')];r=ROOT/'build-tools/msvc-modern';ms=sorted((r/'VC/Tools/MSVC').iterdir())[-1];sdk=r/'Windows Kits/10';v=sorted((sdk/'Include').iterdir())[-1].name;env=dict(os.environ);env['PATH']=str(ms/'bin/Hostx64/x86')+os.pathsep+env['PATH'];env['INCLUDE']=os.pathsep.join(map(str,[ms/'include',*[sdk/f'Include/{v}/{part}'for part in ('ucrt','shared','um')]]));env['LIB']=os.pathsep.join(map(str,[ms/'lib/x86',*[sdk/f'Lib/{v}/{part}/x86'for part in ('ucrt','um')]]));m5=ROOT/'build-tools/msvc500sp3';e5=dict(os.environ);e5['PATH']=str(m5/'bin')+os.pathsep+str(m5/'redist')+os.pathsep+e5['PATH'];e5['INCLUDE']=str(m5/'include');e5['LIB']=str(m5/'lib');gcc=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'
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
    report=dict(original_sha256=HASH,scenes=len(set(r['case_index']for r in results)),total_scenarios=len(cases),comparisons=len(results),complete_suite=args.case_index is None,stage=2,full_consumer_complete=False,intended_consumer='0x4228b0',entries=ENTRIES,physical_original=meta,source_sha256={str(s.relative_to(ROOT)):hashlib.sha256(s.read_bytes()).hexdigest()for s in (SOURCE,SOURCE.with_suffix('.h'))},commands=commands,results=results,limitations=__doc__)
    if linked:report['matching_physical']=linked
    (out/'verification.json').write_text(json.dumps(report,indent=2)+'\n');print(f"PASS: {report['scenes']} scenes/{len(results)} comparisons")

if __name__=='__main__':main()
