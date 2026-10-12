"""Physical-original/native3 sphere-edge stage of full wall-contact closure.

Actual production bodies and authentic lower calls execute at PC53 in an
isolated x86 probe. Original is hash-verified; optional linked PE/map is bound
to its SHA. Full fixed arenas/raw output bits compare without normalization.
No original game startup, imports or TLS. Requires Capstone5 and scoped x86
MSVC5SP3/modern MSVC/i686 MinGW. Full0x4228b0 remains an unfinished objective.
"""
import argparse,hashlib,json,os,struct,subprocess,random
from pathlib import Path
from verify_box_pick_reconstruction import ROOT,HASH,words,fb
from verify_tinted_native_reconstruction import extract
SOURCE=ROOT/'src/carma2/newcommon/34-finteray.c'
GEOMETRY=SOURCE
ENTRIES=[('StationarySphereEdgeCollide',0x45dea0,380),('PointSphereCollision',0x45e020,476),('SphereFaceCollide',0x45e850,1130),('SphereEdgeCollide',0x45d920,1399)]
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
static unsigned char*arena;
int gPling_materials;
'''
DRIVER=r'''
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned h[11];if(argc!=ARGS)return 2;f=fopen(argv[1],"rb");if(!f)return 3;
arena=(unsigned char*)VirtualAlloc((void*)0x20000000,0x1000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(!arena)return 4;
if(fread(h,sizeof(h),1,f)!=1||fread(arena,0x400,1,f)!=1)return 5;fclose(f);INSTALL
_controlfp(_PC_53,_MCW_PC);
if(h[0]==0)((void(C2_HOOK_FASTCALL*)(br_vector3*,br_vector3*,br_vector3*,unsigned,br_vector3*,tCollision_shape_sphere_data*,br_vector3*,br_vector3*,br_scalar*))entries[0])((br_vector3*)(arena+h[1]),(br_vector3*)(arena+h[2]),(br_vector3*)(arena+h[3]),h[4],(br_vector3*)(arena+h[5]),(tCollision_shape_sphere_data*)(arena+h[6]),(br_vector3*)(arena+h[7]),(br_vector3*)(arena+h[8]),(br_scalar*)(arena+h[9]));
else if(h[0]==1)((void(C2_HOOK_FASTCALL*)(br_vector3*,br_vector3*,br_vector3*,br_vector3*,tCollision_shape_sphere_data*,br_vector3*,br_vector3*,br_scalar*))entries[1])((br_vector3*)(arena+h[1]),(br_vector3*)(arena+h[2]),(br_vector3*)(arena+h[3]),(br_vector3*)(arena+h[5]),(tCollision_shape_sphere_data*)(arena+h[6]),(br_vector3*)(arena+h[7]),(br_vector3*)(arena+h[8]),(br_scalar*)(arena+h[9]));
else if(h[0]==2)((void(C2_HOOK_FASTCALL*)(tFace_ref*,br_vector3*,br_vector3*,unsigned,void*,br_vector3*,br_scalar*))entries[2])((tFace_ref*)(arena+h[1]),(br_vector3*)(arena+h[2]),(br_vector3*)(arena+h[3]),h[4],(void*)(arena+h[5]),(br_vector3*)(arena+h[8]),(br_scalar*)(arena+h[9]));
else ((void(C2_HOOK_FASTCALL*)(br_vector3*,br_vector3*,br_vector3*,br_vector3*,tCollision_shape_sphere_data*,br_vector3*,br_vector3*,br_scalar*,int,int))entries[3])((br_vector3*)(arena+h[1]),(br_vector3*)(arena+h[2]),(br_vector3*)(arena+h[3]),(br_vector3*)(arena+h[5]),(tCollision_shape_sphere_data*)(arena+h[6]),(br_vector3*)(arena+h[7]),(br_vector3*)(arena+h[8]),(br_scalar*)(arena+h[9]),(int)(h[10]&0xffff),(int)(h[10]>>16));
_setmode(_fileno(stdout),_O_BINARY);fwrite(arena,0x400,1,stdout);return 0;}
'''

def native_harness():
    s=PREFIX;body=SOURCE.read_text()
    s+=extract(body,'FinterayScalar')+'\n'
    first=body.index('/* VC5 crashes');last=body.index('// FUNCTION:',first);s+=body[first:last]+'\n'
    for name in ('FinterayBoxNaN','FinterayBoxCopyScalar','FinterayShapeLoadScalar','FinteraySphereFacePoint',*[n for n,_,_ in ENTRIES]):s+=extract(body,name)+'\n'
    return s+'static void* entries[4];\n'+DRIVER.replace('ARGS','2').replace('INSTALL',''.join(f'entries[{i}]=(void*){n};'for i,(n,_,_)in enumerate(ENTRIES)))

def physical_harness(binary,addresses):
    # Reuse only the authenticated straight-branch closed graph loader; the
    # fixture/ABI and actual root bodies belong to this sphere-edge verifier.
    import verify_shape_box_reconstruction as loader
    old=(loader.ENTRIES,loader.PREFIX,loader.DRIVER)
    try:
        loader.ENTRIES=[*ENTRIES,*([ENTRIES[0]]*(5-len(ENTRIES)))];loader.PREFIX=PREFIX;loader.DRIVER=DRIVER
        return loader.physical_harness(binary,addresses)
    finally:loader.ENTRIES,loader.PREFIX,loader.DRIVER=old

def scene(mode=0,edge_start=(-2,0,0),edge_end=(2,0,0),edge_dir=(4,0,0),sphere_end=(0,1.001,0),sphere=(1,0,0,0,1),patch=(),**kw):
    a=bytearray(b'\xa5'*0x400)
    for offset,data in ((0,edge_start),(0x20,edge_end),(0x40,edge_dir),(0x60,sphere_end),(0x80,sphere)):
        a[offset:offset+4*len(data)]=struct.pack('<'+'f'*len(data),*data)
    for offset,value in patch:a[offset:offset+4]=words([value])
    h=[mode,kw.get('start_offset',0),kw.get('end_offset',0x20),kw.get('direction_offset',0x40),kw.get('length',fb(sum(x*x for x in edge_dir))),kw.get('sphere_end_offset',0x60),kw.get('sphere_offset',0x80),kw.get('point',0xa0),kw.get('normal',0xc0),kw.get('t',0xe0),0]
    return dict(header=h,data=bytes(a),options=dict(mode=mode,edge_start=edge_start,edge_end=edge_end,edge_dir=edge_dir,sphere_end=sphere_end,sphere=sphere,patch=patch,**kw))

def scenarios():
    cases=[]
    def hit(mode,**kw):
        return scene(mode,**kw) if mode==0 else scene(1,edge_start=(0,0,0),edge_end=(0,2,0),sphere_end=(0,-4,0),**kw)
    for axis in range(3):
        for coordinate in (-3,-2,-1,0,1,2,3):
            for gap in (0,0.999,1,1.001,1.003,1.004):
                start=[0]*3;end=[0]*3;direction=[0]*3;point=[0]*3;start[axis]=-2;end[axis]=2;direction[axis]=4;point[axis]=coordinate;point[(axis+1)%3]=gap
                cases.append(scene(0,edge_start=start,edge_end=end,edge_dir=direction,sphere_end=point))
    for a,b,v in (((0,0,0),(0,2,0),(0,-4,0)),((0,0,0),(0,0,0),(0,0,0)),((0,0,0),(2,0,0),(-4,0,0)),((0,0,0),(2,1,0),(-4,0,0))):
        cases.append(scene(1,edge_start=a,edge_end=b,sphere_end=v))
    for point in (0,4,0x20,0x24,0x40,0x44,0x60,0x64,0x80,0x84,0xc0,0xe0):
        for mode in range(2):cases.append(hit(mode,point=point))
    for normal in (0,4,0x20,0x24,0x40,0x44,0x60,0x64,0x80,0x84,0xa0,0xe0):
        for mode in range(2):cases.append(hit(mode,normal=normal))
    for t in (0,0x20,0x40,0x60,0x80,0xa0,0xc0):
        for mode in range(2):cases.append(hit(mode,t=t))
    rng=random.Random(0x45dea0)
    for mode in range(2):
        for j in range(50):cases.append(scene(mode,edge_start=[rng.uniform(-2,2)for _ in range(3)],edge_end=[rng.uniform(-2,2)for _ in range(3)],edge_dir=[rng.uniform(-4,4)for _ in range(3)],sphere_end=[rng.uniform(-4,4)for _ in range(3)]))
    for j in range(35):
        vertex=[rng.uniform(-10,10)for _ in range(3)];offset=[rng.uniform(2,4)for _ in range(3)]
        cases.append(scene(1,edge_start=vertex,edge_end=[v+x for v,x in zip(vertex,offset)],sphere_end=[-2*x for x in offset]))
    for mode in range(2):
        for offset in (0,0x20,0x40,0x60,0x80,0x90):
            for value in (0x80000000,1,0x80000001,0x7f800000,0xff800000,0x7fc12345,0xffc54321,0x7f812345,0xff854321):cases.append(hit(mode,patch=[(offset,value)]))
    for mode in range(2):
        for left,right in ((0x7fc12345,0xffc54321),(0x7fc12345,0xffc12345),(0x7f812345,0xff854321),(0xffc54321,0x7f812345)):
            for axis in range(3):cases.append(hit(mode,patch=[(4*axis,left),(0x60+4*axis,right)]))
    for axis in range(3):
        for side in (-1,1):
            for start,end in ((2,0),(2,2),(.5,0),(0,2),(2,.501),(2,.503),(2,-2)):
                cases.append(face_scene(axis=axis,start=side*start,end=side*end,two_sided=True))
    for x,y in ((0,0),(.5,.5),(1,1),(2,2),(-2,-2),(2,-2),(0,2),(.9999,0),(1.0001,0)):
        cases.append(face_scene(x=x,y=y))
    for name in (None,'','!','>','?','#','@','normal'):cases.append(face_scene(name=name))
    for t in (0,0x20,0x80,0xc0,0x134,0x138,0x13c,0x144):cases.append(face_scene(t=t))
    for normal in (0,4,0x20,0x24,0x80,0xe0,0x134,0x138,0x144):cases.append(face_scene(normal=normal))
    for off in (0,4,8,0x20,0x24,0x28,0x104,0x108,0x110,0x120,0x134,0x138,0x13c,0x144):
        for value in (0x80000000,1,0x7f800000,0xff800000,0x7fc12345,0x7f812345):cases.append(face_scene(patch=[(off,value)],two_sided=True))
    for radius in (0,0x80000000,1,fb(-.5),0x7f800000,0x7fc12345,0x7f812345):cases.append(face_scene(radius=radius,two_sided=True))
    for j in range(35):
        c=face_scene(two_sided=True);a=bytearray(c['data']);vertices=[[rng.uniform(-4,4)for _ in range(3)]for k in range(3)];u=[vertices[1][k]-vertices[0][k]for k in range(3)];v=[vertices[2][k]-vertices[0][k]for k in range(3)];n=[u[1]*v[2]-u[2]*v[1],u[2]*v[0]-u[0]*v[2],u[0]*v[1]-u[1]*v[0]];length=sum(x*x for x in n)**.5;n=[x/length for x in n];p=[sum(z[k]for z in vertices)/3 for k in range(3)]
        a[:12]=struct.pack('<3f',*[p[k]+2*n[k]for k in range(3)]);a[0x20:0x2c]=struct.pack('<3f',*[p[k]-2*n[k]for k in range(3)]);a[0x104:0x128]=struct.pack('<9f',*[x for z in vertices for x in z]);a[0x134:0x140]=struct.pack('<3f',*n);a[0x144:0x148]=struct.pack('<f',sum(x*y for x,y in zip(vertices[0],n)));c['data']=bytes(a);c['options'].update(random_vertices=vertices,random_normal=n);cases.append(c)
    for axis in range(3):
        for coordinate in (-3,-2,-1,0,1,2,3):
            for start,end in ((2,-2),(2,0),(2,2),(0,2),(1.001,1.001),(1.003,1.003)):
                p=[0]*3;q=[0]*3;p[axis]=q[axis]=coordinate;p[(axis+1)%3]=start;q[(axis+1)%3]=end
                e=[0]*3;f=[0]*3;e[axis]=-2;f[axis]=2
                for flags in (0,0x80,0x800000,0x800080):cases.append(moving_scene(edge_start=e,edge_end=f,sphere_start=p,sphere_end=q,flags=flags))
    for j in range(60):
        e=[rng.uniform(-10,10)for _ in range(3)];d=[rng.uniform(-4,4)for _ in range(3)];f=[e[k]+d[k]for k in range(3)];n=[d[1],-d[0],0];length=sum(x*x for x in n)**.5;n=[x/length for x in n];fraction=(-.3,.2,.5,.8,1.3)[j%5];center=[e[k]+fraction*d[k]for k in range(3)];p=[center[k]+2*n[k]for k in range(3)];q=[center[k]-2*n[k]for k in range(3)]
        cases.append(moving_scene(edge_start=e,edge_end=f,sphere_start=p,sphere_end=q))
    for field in ('point','normal','t'):
        for off in (0,4,0x20,0x24,0x40,0x44,0x60,0x64,0x80,0x84,0x90,0xa0,0xc0,0xe0):cases.append(moving_scene(**{field:off}))
    for off in (0,0x20,0x40,0x60,0x80,0x90):
        for value in (0x80000000,1,0x7f800000,0xff800000,0x7fc12345,0xffc54321,0x7f812345,0xff854321):cases.append(moving_scene(patch=[(off,value)]))
    for left,right in ((0x7fc12345,0xffc54321),(0x7f812345,0xff854321),(0xffc54321,0x7f812345)):
        for axis in range(3):cases.append(moving_scene(patch=[(4*axis,left),(0x20+4*axis,right)]))
    return cases

def moving_scene(edge_start=(-2,0,0),edge_end=(2,0,0),sphere_start=(0,2,0),sphere_end=(0,-2,0),flags=0,**kw):
    c=scene(3,edge_start=edge_start,edge_end=edge_end,edge_dir=sphere_start,sphere_end=sphere_end,**kw);c['header'][10]=flags;c['options']['flags']=flags
    return c

def face_scene(axis=2,start=2,end=0,x=0,y=0,name='normal',two_sided=False,patch=(),**kw):
    a=bytearray(b'\xa5'*0x400);a[0x100:0x150]=bytes(80);a[0x300:0x368]=bytes(104)
    a[0x100:0x104]=words([0 if name is None else 0x20000300]);a[0x304:0x308]=words([0x20000380]);a[0x320:0x324]=words([0x1000 if two_sided else 0])
    if name is not None:a[0x380:0x380+len(name)+1]=name.encode()+b'\0'
    i=1 if axis==0 else 0;j=1 if axis==2 else 2;normal=[0]*3;normal[axis]=1
    for k,(u,v)in enumerate(((-2,-2),(2,-2),(0,2))):
        p=[0]*3;p[i]=u;p[j]=v;a[0x104+12*k:0x110+12*k]=struct.pack('<3f',*p)
    a[0x134:0x140]=struct.pack('<3f',*normal);a[0x144:0x148]=words([0]);p=[0]*3;q=[0]*3;p[axis]=start;q[axis]=end;p[i]=q[i]=x;p[j]=q[j]=y;a[:12]=struct.pack('<3f',*p);a[0x20:0x2c]=struct.pack('<3f',*q)
    for off,v in patch:a[off:off+4]=words([v])
    h=[2,0x100,0,0x20,kw.get('radius',fb(.5)),kw.get('unused',0xa0),0,0xa0,kw.get('normal',0xc0),kw.get('t',0xe0),0]
    return dict(header=h,data=bytes(a),options=dict(mode=2,axis=axis,start=start,end=end,x=x,y=y,name=name,two_sided=two_sided,patch=patch,**kw))

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE');p.add_argument('--output',type=Path,default=ROOT/'build-audit/wall-contact-stage4');p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);p.add_argument('--case-index',type=int,action='append');args=p.parse_args()
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
    report=dict(original_sha256=HASH,scenes=len(set(r['case_index']for r in results)),total_scenarios=len(cases),comparisons=len(results),complete_suite=args.case_index is None,stage=4,full_consumer_complete=False,intended_consumer='0x4228b0',entries=ENTRIES,physical_original=meta,source_sha256={str(s.relative_to(ROOT)):hashlib.sha256(s.read_bytes()).hexdigest()for s in (SOURCE,SOURCE.with_suffix('.h'))},commands=commands,results=results,limitations=__doc__)
    if linked:report['matching_physical']=linked
    (out/'verification.json').write_text(json.dumps(report,indent=2)+'\n');print(f"PASS: {report['scenes']} scenes/{len(results)} comparisons")

if __name__=='__main__':main()
