"""Staged proof of the full original wall-contact consumer closure.

Current stage proves AddObjectWallForce0x422fe0 and RayCastThruFaceList0x4225f0
using actual CheckSingleFace. Full0x4228b0 consumer remains the objective; this
manifest never claims its unfinished branches implemented. Hash-checked original
closed x86 graph executes physically at PC53; actual production native3 bodies
and optional immutable rebuilt PE compare full arenas and raw return/global bits.
Requires Capstone5 and scoped MSVC5/modern x86 MSVC/i686 MinGW. No game entry,
imports/TLS or replacement geometry stubs execute. No public normalization.
"""
import argparse,hashlib,json,os,struct,subprocess
from pathlib import Path
from verify_box_pick_reconstruction import ROOT,HASH,words,fb
from verify_tinted_native_reconstruction import extract
SOURCE=ROOT/'src/carma2/newcommon/49-physics.c'
GEOMETRY=ROOT/'src/carma2/newcommon/34-finteray.c'
ENTRIES=[('AddObjectWallForce',0x422fe0,824),('RayCastThruFaceList',0x4225f0,209),('CheckSingleFace',0x45ecc0,1035)]
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
#include "c2_math.h"
typedef char layout[sizeof(tPhysics_contact)==96 && sizeof(tFace_ref)==80 ? 1:-1];
int gPling_materials;
static unsigned char* arena;
typedef int(C2_HOOK_FASTCALL*contact_fn)(tPhysics_contact*,const br_vector3*,br_scalar,const br_vector3*,int,int,tPhysics_object*,int,tFace_ref*,const br_vector3*);
typedef int(C2_HOOK_FASTCALL*raw_contact_fn)(tPhysics_contact*,const br_vector3*,tU32,const br_vector3*,int,int,tPhysics_object*,int,tFace_ref*,const br_vector3*);
typedef tFace_ref*(C2_HOOK_FASTCALL*ray_fn)(br_vector3*,br_vector3*,br_vector3*,br_scalar*,br_vector3*,tFace_ref*,int);
'''
DRIVER=r'''
int C2_HOOK_CDECL main(int argc,char**argv){
 FILE*f;unsigned h[16],result;raw_contact_fn volatile contact;ray_fn volatile ray;
 if(argc!=ARGS)return 2;f=fopen(argv[1],"rb");if(!f)return 3;
 arena=(unsigned char*)VirtualAlloc((void*)0x20000000,0x4000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(!arena)return 4;
 if(fread(h,sizeof(h),1,f)!=1||fread(arena,0x4000,1,f)!=1)return 5;fclose(f);gPling_materials=(int)h[15];
 INSTALL
 _controlfp(_PC_53,_MCW_PC);
 if(h[0]==0)result=(unsigned)contact((tPhysics_contact*)(arena+h[1]),(br_vector3*)(arena+h[2]),h[4],(br_vector3*)(arena+h[3]),(int)h[5],(int)h[6],h[7]?(tPhysics_object*)(arena+h[7]):NULL,(int)h[8],(tFace_ref*)(arena+h[9]),h[10]?(br_vector3*)(arena+h[10]):NULL);
 else result=(unsigned)ray((br_vector3*)(arena+h[2]),(br_vector3*)(arena+h[11]),(br_vector3*)(arena+h[12]),(br_scalar*)(arena+h[13]),(br_vector3*)(arena+h[14]),(tFace_ref*)(arena+h[9]),(int)h[6]);
 _setmode(_fileno(stdout),_O_BINARY);fwrite(&result,4,1,stdout);fwrite(&gPling_materials,4,1,stdout);fwrite(arena,0x4000,1,stdout);return 0;
}
'''
def native_harness():
    geo=GEOMETRY.read_text();body=SOURCE.read_text();end=geo.index('} tFinteray_projection;')+len('} tFinteray_projection;');start=geo.rfind('typedef struct {',0,end)
    s=PREFIX+geo[start:end]+'\n'
    for n in ('FinterayScalar','FinterayFaceProjection','FinterayFacePoint','CheckSingleFace'):s+=extract(geo,n)+'\n'
    for n in ('PhysicsCopyWord','PhysicsSubtractScalar','PhysicsScalar','PhysicsScalarStore','AddObjectWallForce','RayCastThruFaceList'):s+=extract(body,n)+'\n'
    return s+DRIVER.replace('ARGS','2').replace('INSTALL','{union{contact_fn typed;raw_contact_fn raw;}cast;cast.typed=AddObjectWallForce;contact=cast.raw;ray=RayCastThruFaceList;}')

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
    s=PREFIX+'static void* entries[2];\nstatic const unsigned refs[][2]={'+','.join('{'+str(a)+','+str(k)+'}'for a,k in sorted(refs.items()))+'};\n'
    s+='static const unsigned offsets[2]={'+','.join(str(a-base)for a in starts[:2])+'};\n'
    s+=r'''
static int install(const char*path){
 FILE*f;unsigned char*disk,*image;unsigned size,i,site,value;IMAGE_DOS_HEADER*dos;IMAGE_NT_HEADERS32*nt;IMAGE_SECTION_HEADER*sec;
 f=fopen(path,"rb");if(!f)return 91;fseek(f,0,SEEK_END);size=(unsigned)ftell(f);rewind(f);disk=(unsigned char*)malloc(size);if(!disk||fread(disk,size,1,f)!=1)return 92;fclose(f);
 dos=(IMAGE_DOS_HEADER*)disk;nt=(IMAGE_NT_HEADERS32*)(disk+dos->e_lfanew);sec=IMAGE_FIRST_SECTION(nt);image=(unsigned char*)VirtualAlloc(NULL,nt->OptionalHeader.SizeOfImage,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image)return 93;
 memcpy(image,disk,nt->OptionalHeader.SizeOfHeaders);for(i=0;i<nt->FileHeader.NumberOfSections;i++)memcpy(image+sec[i].VirtualAddress,disk+sec[i].PointerToRawData,sec[i].SizeOfRawData);
 for(i=0;i<sizeof(refs)/sizeof(refs[0]);i++){site=(unsigned)image+refs[i][0];memcpy(&value,(void*)site,4);value=refs[i][1]?(unsigned)&gPling_materials:(unsigned)image+value-nt->OptionalHeader.ImageBase;memcpy((void*)site,&value,4);}
 for(i=0;i<2;i++)entries[i]=image+offsets[i];free(disk);return 0;
}
'''
    s+=DRIVER.replace('ARGS','3').replace('INSTALL','{int r=install(argv[2]);if(r)return r;contact=(raw_contact_fn)entries[0];ray=(ray_fn)entries[1];}')
    return s,dict(entries=[hex(a)for a in sorted(entries)],instruction_count=len(instructions),closure_sha256=hashlib.sha256(json.dumps(sorted(instructions.items())).encode()).hexdigest(),references=sorted(refs.items()))

def scene(mode=0,count=0,patch=(),**kw):
    a=bytearray(b'\xa5'*0x4000)
    def wr(p,v):a[p:p+4]=words([v])
    def fl(p,v):a[p:p+4*len(v)]=struct.pack('<'+'f'*len(v),*v)
    def ptr(p,v):wr(p,0x20000000+v if v else 0)
    fl(0x100,[0,0,0]if mode==0 else [.25,.25,1]);fl(0x140,[0,1,0]);fl(0x180,[0,0,-2]);fl(0x1c0,[1,2,3])
    for i in range(-2,6):
        p=0x400+i*96;fl(p,[.1+i*.1]);fl(p+0x14,[3+i,2,1]);fl(p+0x2c,[0,1,0])
    a[0x1000:0x14d8]=bytes(0x4d8);fl(0x1000+0x14,[.1,.2,.3]);fl(0x1000+0x24,[-1,-2,-3,1,2,3]);ptr(0x1000+0x18c,0x1500 if kw.get('joint')else 0);a[0x1500:0x1560]=bytes(0x60);wr(0x1500,kw.get('joint',0))
    for i in range(5):
        p=0x800+i*80;a[p:p+80]=bytes(80);ptr(p,0x2000 if kw.get('material',1)else 0);fl(p+4,[0,0,i*.25,1,0,i*.25,0,1,i*.25]);fl(p+52,[0,0,1]);fl(p+68,[i*.25]);wr(p+64,kw.get('face_flags',0))
    a[0x2000:0x209c]=bytes(156);ptr(0x2004,0x2500);wr(0x2020,0x1800);name=kw.get('identifier','ordinary').encode()+b'\0';a[0x2500:0x2500+len(name)]=name
    for p,v in patch:wr(p,v)
    h=[mode,kw.get('contacts',0x400),kw.get('position',0x100),kw.get('normal',0x140),kw.get('t',fb(.05)),0,count,kw.get('object',0x1000),kw.get('type',3),kw.get('faces',0x800),kw.get('reference',0x1c0),kw.get('direction',0x180),kw.get('normal_out',0x200),kw.get('t_out',0x240),kw.get('collision_out',0x280),kw.get('pling',1)]
    return dict(header=h,data=bytes(a),options=dict(mode=mode,count=count,patch=patch,**kw))

def scenarios():
    cases=[]
    for count in (-2,-1,0,1,2,3,4,5):
        for t in (fb(-1),fb(.05),fb(.15),fb(.3),fb(2),fb(3)):cases.append(scene(count=count,t=t))
    for joint in (1,2,3,4):
        for pos in ((0,0,0),(1,0,0),(0,2,0)):cases.append(scene(count=1,joint=joint,patch=[(0x100+i*4,fb(v))for i,v in enumerate(pos)]))
    for offset in (0x100,0x1004,0x1014,0x414,0x42c,0x140):
        for value in (0x7f800000,0xff800000,0x7fc12345,0x7f812345,0x80000000,1):cases.append(scene(count=1,patch=[(offset,value)]))
    for p in (0x414,0x42c,0x460,0x14,0x140):cases.append(scene(count=2,position=p,normal=p,reference=p))
    for count in (-1,0,1,2,5):
        for flags in (0,0x80):cases.append(scene(1,count=count,face_flags=flags))
    for name in ('!special','>special','?special','ordinary'):
        for pling in (0,1):cases.append(scene(1,count=3,identifier=name,pling=pling))
    for out in (0x100,0x140,0x180,0x200,0x240,0x280,0x800+52):cases.append(scene(1,count=3,normal_out=out,t_out=out,collision_out=out))
    for threshold in (.0005,.0045000000000000005):
        value=fb(-.1+threshold**.5)
        for delta in (-1,0,1):cases.append(scene(count=1,patch=[(0x414,value+delta),(0x418,fb(-.2)),(0x41c,fb(-.3))]))
    for threshold in (.95,-.95):
        value=fb(threshold)
        for delta in (-1,0,1):cases.append(scene(count=1,patch=[(0x414,fb(-.1)),(0x418,fb(-.2)),(0x41c,fb(-.3)),(0x430,value+delta)]))
    for value in (0x7f800000,0xff800000,0x7fc12345,0xffc54321,0x7f812345,0xff854321):cases.append(scene(count=0,t=value))
    for left,right in ((0x7fc54321,0x7fc12345),(0x7fc12345,0x7fc54321),(0x7f854321,0xff812345)):
        cases.append(scene(count=0,patch=[(0x100,left),(0x1014,right)]))
    for contacts in (0x1000,0x1014,0x1024):cases.append(scene(count=0,contacts=contacts))
    return cases

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE');p.add_argument('--output',type=Path,default=ROOT/'build-audit/wall-contact-stage1');p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);p.add_argument('--case-index',type=int,action='append');args=p.parse_args()
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
                prefix=out/f'failure-{label}-{i}';prefix.with_suffix('.expected.bin').write_bytes(expected);prefix.with_suffix('.actual.bin').write_bytes(actual);prefix.with_suffix('.json').write_text(json.dumps(dict(options=c['options'],first_difference=next((j for j,(x,y)in enumerate(zip(expected,actual))if x!=y),None),exit=run.returncode,expected_size=len(expected),actual_size=len(actual)),indent=2));raise AssertionError(f'{label} contact{i} differs:{prefix}')
            results.append(dict(case_index=i,compiler=label,all_bytes_match=True,output_sha256=hashlib.sha256(actual).hexdigest()))
        if (i+1)%30==0:print(f'Passed {i+1}/{len(cases)}',flush=True)
    report=dict(original_sha256=HASH,scenes=len(cases),comparisons=len(results),complete_suite=args.case_index is None,stage=1,full_consumer_complete=False,intended_consumer='0x4228b0',entries=ENTRIES,physical_original=meta,source_sha256={str(s.relative_to(ROOT)):hashlib.sha256(s.read_bytes()).hexdigest()for s in (SOURCE,SOURCE.with_suffix('.h'),GEOMETRY,GEOMETRY.with_suffix('.h'))},commands=commands,results=results,limitations=__doc__)
    if linked:report['matching_physical']=linked
    (out/'verification.json').write_text(json.dumps(report,indent=2)+'\n');print(f'PASS: {len(cases)} scenes/{len(results)} comparisons')

if __name__=='__main__':main()
