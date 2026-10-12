"""Physical original/native3/linked proof of three complete wall-contact prerequisites.

Requires Capstone5 and scoped32-bit MSVC5SP3/modern MSVC/MinGW. Actual lower
geometry/matrix/contact functions execute; no lower call is replaced.419 fixed
fixtures preserve preceding native proofs, with12-word driver protocol for all
three roots. Original PE and optional rebuilt PE/map hashes are verified. Root
ABI/nonvolatile/x87TOP and all65536 arena bytes/int return are compared raw at
PC53/masked exceptions. Null-material accepted-plane fault is a separate precise
exit-only boundary; unsupported architectures/fenv/arbitrary-pointer equivalence
are outside this proof. Full wall-contact consumer0x4228b0 remains incomplete.
"""
import argparse,ast,hashlib,inspect,json,os,random,re,struct,subprocess
from pathlib import Path
from verify_tinted_native_reconstruction import extract
import verify_wall_contact_reconstruction as loader
ROOT=Path(__file__).resolve().parents[1]
HASH='9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
ENTRIES=[('EliminateFaces',0x423320,891),('CheckSingleEdgeWithSphere',0x423ce0,491),('GetSphereToEdgePoints',0x423c00,217)]
PHYSICS=ROOT/'src/carma2/newcommon/49-physics.c'
GEOMETRY=ROOT/'src/carma2/newcommon/34-finteray.c'
MATRIX=ROOT/'src/brender/core/math/matrix34.c'
FIXTURE_SHA256='d9bea4d624425c31b124139485542e22468160f233f2126d57d1839511f8dbb4'
BOUNDS_MODE=False
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
#include <brender/brender.h>
#include "c2_math.h"
static unsigned char* arena;
int gPling_materials;
#define gFace_list__car ((tFace_ref*)(arena+0x2000))
'''
DECLS='''
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[9],builder_stack_count,builder_result,builder_poison,builder_cdecl;
extern unsigned __cdecl BuilderInvoke(void);
'''

def driver(combined):
 setup='''if(h[0]==0){n=3;v[0]=(unsigned)(arena+h[1]);v[1]=(unsigned)(arena+h[2]);v[2]=(unsigned)(arena+h[3]);}
 else if(h[0]==1){n=9;v[0]=(unsigned)(arena+h[9]);v[1]=(unsigned)(arena+h[10]);v[2]=(unsigned)(arena+h[1]);v[3]=(unsigned)(arena+h[2]);v[4]=(unsigned)(arena+h[7]);v[5]=h[4];v[6]=(unsigned)(arena+h[5]);v[7]=(unsigned)(arena+h[6]);v[8]=(unsigned)(arena+h[11]);}
 else {n=8;v[0]=(unsigned)(arena+h[1]);v[1]=(unsigned)(arena+h[2]);v[2]=(unsigned)(arena+h[7]);v[3]=h[4];v[4]=40;v[5]=(unsigned)(arena+h[5]);v[6]=(unsigned)(arena+h[6]);v[7]=(unsigned)(arena+h[3]);}
 builder_address=(unsigned)entries[h[0]];''' if combined else '''n=3;v[0]=(unsigned)(arena+h[0]);v[1]=(unsigned)(arena+h[1]);v[2]=(unsigned)(arena+h[2]);builder_address=(unsigned)entry;'''
 return '''int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned h[HEADER],v[9],n,i;LOCAL
 if(argc!=ARGS)return 2;SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);arena=VirtualAlloc((void*)0x20000000,65536,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(!arena)return 3;
 f=fopen(argv[1],"rb");if(!f)return 4;if(fread(h,sizeof(h),1,f)!=1||fread(arena,65536,1,f)!=1)return 5;fclose(f);INSTALL
 _controlfp(_PC_53,_MCW_PC);SETUP
 builder_mode=0;builder_x=v[0];builder_y=v[1];builder_poison=0xcccccccc;
 #if defined(REC2_MATCHING)
 builder_cdecl=0;builder_stack_count=n-2;for(i=2;i<n;i++)builder_stack[i-2]=v[i];
 #else
 builder_cdecl=1;builder_stack_count=n;for(i=0;i<n;i++)builder_stack[i]=v[i];
 #endif
 if(BuilderInvoke())return 14;
 _setmode(_fileno(stdout),_O_BINARY);fwrite(&builder_result,4,1,stdout);fwrite(arena,65536,1,stdout);return 0;}
 '''.replace('HEADER','12'if combined else'4').replace('LOCAL',''if combined else'void* entry;').replace('SETUP',setup)

def compile_caller(out,env5):
 tree=ast.parse((ROOT/'scripts/verify_opponent_navigation_reconstruction.py').read_text())
 strings=[n.value.value for n in ast.walk(tree)if isinstance(n,ast.Assign)and isinstance(n.value,ast.Constant)and any(isinstance(t,ast.Name)and t.id=='caller'for t in n.targets)]
 assert len(strings)==1
 caller=strings[0].replace('builder_stack[5]','builder_stack[9]').replace('builder_poison;','builder_poison,builder_cdecl;')
 # Nine cdecl arguments are used for the longest root; FASTCALL consumes seven.
 caller=caller.replace(' mov builder_result,eax\n mov ecx,esp',' mov builder_result,eax\n cmp builder_cdecl,0\n je stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n stackready:\n mov ecx,esp')
 (out/'caller.c').write_text(caller)
 cmd=[str((ROOT/'build-tools/msvc500sp3/bin/cl.exe').resolve()),'/nologo','/O2','/Zl','/c',str(out/'caller.c'),'/Fo'+str(out/'caller.obj')]
 run=subprocess.run(cmd,cwd=out,env=env5,text=True,capture_output=True);(out/'caller-build.txt').write_text(run.stdout+run.stderr);assert not run.returncode,run.stdout+run.stderr
 return cmd


def physical_harness(binary,addresses):
    # Extend the retained actual-closure walker to three roots and the genuine
    #300-face table. Indexed face field operands retain their addend.
    source=inspect.getsource(loader.physical_harness)
    injection="""            for op in i.operands:
                face=addresses[0x744820]
                if op.type==CS_OP_IMM and face<=op.imm<face+300*80:
                    assert i.imm_size==4;refs[a-base+i.imm_offset]=2
                if op.type==CS_OP_MEM and face<=op.mem.disp<face+300*80:
                    assert i.disp_size==4;refs[a-base+i.disp_offset]=2
"""
    assert source.count("            if i.mnemonic=='call':")==1
    source=source.replace("            if i.mnemonic=='call':",injection+"            if i.mnemonic=='call':")
    source=source.replace('entries[2]','entries[3]').replace('offsets[2]','offsets[3]').replace('starts[:2]','starts[:3]').replace('i<2','i<3')
    source=source.replace("refs[i][1]?(unsigned)&gPling_materials:","refs[i][1]==2?(unsigned)(arena+0x2000)+value-FACE_ADDRESS:refs[i][1]?(unsigned)&gPling_materials:")
    source=source.replace("    return s,dict(","    s=s.replace('FACE_ADDRESS',str(addresses[0x744820])).replace('contact=(raw_contact_fn)entries[0];ray=(ray_fn)entries[1];',';')\n    return s,dict(instructions=sorted(instructions.items()),")
    context=dict(PREFIX=PREFIX+DECLS,DRIVER=root_driver(),ENTRIES=ENTRIES,struct=struct,hashlib=hashlib,json=json)
    exec(source,context);code,meta=context['physical_harness'](binary,addresses)
    # All address-bearing relocations in reachable code must be explicitly
    # transported. Imported code and indirect branches were already rejected.
    nt=struct.unpack_from('<I',binary,60)[0];base=struct.unpack_from('<I',binary,nt+52)[0];tab=nt+24+struct.unpack_from('<H',binary,nt+20)[0]
    sections=[struct.unpack_from('<IIII',binary,tab+40*i+8)for i in range(struct.unpack_from('<H',binary,nt+6)[0])]
    def raw(rva,n):
        for _,va,size,off in sections:
            if va<=rva<va+size:return binary[off+rva-va:off+rva-va+n]
        raise AssertionError(('Unmapped relocation',hex(rva)))
    rel,size=struct.unpack_from('<II',binary,nt+24+96+5*8);cursor=rel;covered={site for site,_ in meta['references']};active={a-base+k for a,b in meta['instructions']for k in range(len(b)//2)};sites=[]
    while cursor<rel+size:
        page,n=struct.unpack('<II',raw(cursor,8));assert n>=8
        for word in struct.unpack('<'+'H'*((n-8)//2),raw(cursor+8,n-8)):
            if word>>12==3:
                at=page+(word&4095)
                if at in active:assert at in covered,hex(at);sites.append(at)
        cursor+=n
    meta['relocations_checked']=sites
    return code,meta

def native_harness(candidate_physics,candidate_geometry):
    geo=GEOMETRY.read_text();physics=PHYSICS.read_text();matrix=MATRIX.read_text()
    chosen_physics=candidate_physics.read_text()if candidate_physics else physics
    chosen_geometry=candidate_geometry.read_text()if candidate_geometry else geo
    code=PREFIX+DECLS
    if BOUNDS_MODE:
        for n in ['FinterayScalar','FinterayBoundsAdd','GetNewBoundingBox']:code+=extract(chosen_geometry,n)+'\n'
        code+=extract(matrix,'BrMatrix34ApplyP')+'\n'
        return code+'static void* entries[3];\n'+root_driver().replace('ARGS','2').replace('INSTALL','entries[0]=(void*)GetNewBoundingBox;entries[1]=entries[2]=entries[0];')
    code+=extract(geo,'FinterayScalar')+'\n'
    for n in ['FinterayBoundsAdd','GetNewBoundingBox']:code+=extract(chosen_geometry,n)+'\n'
    for n in ['PhysicsCopyWord','PhysicsScalar','PhysicsScalarNaN','PhysicsSubtractScalar','PhysicsScalarStore','AddObjectWallForce']:code+=extract(physics,n)+'\n'
    for n in ['Matrix34ApplyProductLoaded','Matrix34ApplyProductMemory','Matrix34ApplyAdd','BrMatrix34ApplyP','BrMatrix34ApplyV','BrMatrix34TApplyV']:code+=extract(matrix,n)+'\n'
    first=geo.index('/* VC5 crashes');last=geo.index('// FUNCTION:',first);code+=geo[first:last]+'\n'
    for n in ['FinterayBoxNaN','FinterayBoxCopyScalar','FinterayShapeLoadScalar','StationarySphereEdgeCollide','PointSphereCollision','SphereEdgeCollide']:code+=extract(geo,n)+'\n'
    for n in ['PhysicsWallNaN','PhysicsWallAddScalar','EliminateFaces','CheckSingleEdgeWithSphere','GetSphereToEdgePoints']:code+=extract(chosen_physics,n)+'\n'
    return code+'static void* entries[3];\n'+driver(True).replace('ARGS','2').replace('INSTALL','entries[0]=(void*)EliminateFaces;entries[1]=(void*)CheckSingleEdgeWithSphere;entries[2]=(void*)GetSphereToEdgePoints;')

def root_driver():
    code=driver(True)
    return code.replace('builder_mode=0','builder_mode=2')if BOUNDS_MODE else code

def bounds_fixtures():
    # Same25 focused leaf fixtures as the prior physical scratch proof. Only
    # driver header changes from4 words to12; every arena byte is preserved.
    base=make_fixture(scenarios()[93])[48:]
    cases=[dict(name='retained-paired-min-max',arena=base,out=0x51c)]
    for out in (0x51c,0x504,0x508,0x80c):
        for matrix in ([1,0,0,0,1,0,0,0,1,0,0,0],[1,.3,.2,-.4,1,.5,.7,-.2,1,.25,-.5,.75]):
            arena=bytearray(base);struct.pack_into('<6f',arena,0x504,-1,-1,-1,1,1,1);struct.pack_into('<12f',arena,0x800,*matrix)
            cases.append(dict(name='finite-valid-alias',arena=bytes(arena),out=out))
    for a,b in [(0x7fc12345,0xffc54321),(0x7fc12345,0x7f812345),(0xff812345,0x7fc54321),(0x7f800000,0xff800000)]:
        for left,right in [(0x504,0x510),(0x504,0x800),(0x510,0x800),(0x504,0x804)]:
            arena=bytearray(base);struct.pack_into('<6f',arena,0x504,-1,-1,-1,1,1,1);struct.pack_into('<12f',arena,0x800,1,0,0,0,1,0,0,0,1,0,0,0)
            struct.pack_into('<I',arena,left,a);struct.pack_into('<I',arena,right,b);cases.append(dict(name='paired-fields',arena=bytes(arena),out=0x51c,left=left,right=right,bits=[a,b]))
    fixtures=[struct.pack('<12I',0,c['out'],0x504,0x800,0,0,0,0,0,0,0,0)+c['arena']for c in cases]
    return [{k:v for k,v in c.items()if k!='arena'}for c in cases],fixtures

def scenarios():
 cases=[dict(z=z,name=n)for z in [-2.,-.01,0.,.01,2.]for n in ['ordinary','!','>','?','#','']]
 for off in [0x504,0x508,0x510,0x514,0x102c,0x1030,0x800,0x2004,0x2034,0x2038]:
  for value in [0x80000000,1,0x7f800000,0x7fc12345,0x7f812345]:cases.append(dict(z=0.,name='ordinary',patch=[(off,value)]))
 for matrix in [[0,1,0,-1,0,0,0,0,1,1,2,3],[1,.5,.125,-.25,1,.5,.5,.125,1,.25,-.5,.75]]:
  cases += [dict(z=z,name='ordinary',actor_matrix=matrix,shape_matrix=matrix)for z in [-2.,0.,2.]]
 # Exact float tolerance neighbors and paired nonfinite operands, bounded contracts.
 for sign in (1,-1):
  value=struct.unpack('<I',struct.pack('<f',sign*1.005))[0]
  for raw in (value-1,value,value+1):cases.append(dict(z=struct.unpack('<f',struct.pack('<I',raw))[0],name='ordinary',label='plane-tolerance-neighbor'))
 for a,b in [(0x7fc12345,0xffc54321),(0x7fc12345,0x7f812345),(0xff812345,0x7fc54321),(0x7f800000,0xff800000)]:
  for left,right in [(0x2004,0x2034),(0x504,0x510),(0x102c,0x2034),(0x200c,0x203c)]:
   cases.append(dict(z=0.,name='ordinary',patch=[(left,a),(right,b)],label='paired-operands'))
 cull=[dict(mode=0,**o)for o in cases]
 cases=[dict(mode=mode,z=z,name='ordinary',flags=flags)for mode in [1,2]for z in [0.,.01,2.]for flags in [0,1,2,4,7,0x80]]
 for mode in [1,2]:
  for a in [0.,.1,.3,.7]:
   cases.append(dict(mode=mode,z=a,name='ordinary',flags=0,actor_matrix=[1,a,0,-a,1,0,0,0,1,0,0,0],object_matrix=[1,-a,0,a,1,0,0,0,1,0,0,0]))
 for mode in [1,2]:
  for offset in [0x8c,0x90,0x9c,0x102c,0x1030,0x600,0x604,0x610,0x700,0x704]:
   for value in [0x80000000,1,0x7f800000,0x7fc12345,0x7f812345]:cases.append(dict(mode=mode,z=0.,name='ordinary',flags=0,patch=[(offset,value)]))
 rng=__import__('random').Random(0x423ce0)
 for mode in [1,2]:
  for j in range(20):
   e=[rng.uniform(-3,3)for _ in range(3)];d=[rng.uniform(1,3)for _ in range(3)];q=[e[k]+d[k]for k in range(3)];n=[d[1],-d[0],0];length=sum(x*x for x in n)**.5;n=[x/length for x in n];c=[(e[k]+q[k])*.5 for k in range(3)];p0=[c[k]+2*n[k]for k in range(3)];p1=[c[k]-2*n[k]for k in range(3)];cases.append(dict(mode=mode,z=0.,name='ordinary',flags=0,vertices=[*e,*q,*[q[k]+n[k]for k in range(3)]],sphere_start=p0,sphere_end=p1,sphere_data=[1.3,.3,.7,-.2,1.69]))
  for contact in [0x1fa0,0x2000,0x2014,0x600,0x700,0x720]:cases.append(dict(mode=mode,z=0.,name='ordinary',flags=0,contacts=contact))
 for mode in [1,2]:
  for count in [-1,0,1,3,4,5]:
   for flags in [0,1,2,4,7,0x80]:cases.append(dict(mode=mode,z=0.,name='ordinary',flags=flags,count=count,label='contact-count'))
  for pair in [(0x7fc12345,0xffc54321),(0x7fc12345,0x7f812345),(0xff812345,0x7fc54321),(0x7f800000,0xff800000)]:
   for left,right in [(0x600,0x604),(0x8c,0x102c),(0x8c,0x90),(0x700,0x720)]:cases.append(dict(mode=mode,z=0.,name='ordinary',flags=0,patch=[(left,pair[0]),(right,pair[1])],label='paired-fields'))
  for t in [0.,1.,.5]:cases.append(dict(mode=mode,z=0.,name='ordinary',flags=0,sphere_start=[0,1+6*t,0],sphere_end=[0,-5+6*t,0],label='time-boundary'))
 for start,end in [(0,0),(1,1),(2,1),(1,2),(0,2)]:cases.append(dict(mode=2,z=0.,name='ordinary',flags=0,face_start=start,face_end=end,label='face-range'))
 return cull+cases

def make_fixture(opt):
 if opt['mode']==0:
  z=opt['z'];name=opt['name']
  arena=bytearray(65536)
  def w(at,*v):struct.pack_into('<'+'I'*len(v),arena,at,*v)
  def f(at,*v):struct.pack_into('<'+'f'*len(v),arena,at,*v)
  w(0,0x20001000);struct.pack_into('<HH',arena,0x174,0,1);w(0x500,0);f(0x504,-1,-1,-1,1,1,1);f(0x102c,1,0,0,0,1,0,0,0,1,0,0,0);f(0x800,1,0,0,0,1,0,0,0,1,0,0,0);w(0x2000,0x20000880);f(0x2004,-2,-2,z,2,-2,z,0,2,z);f(0x2034,0,0,1);w(0x2040,0x75);w(0x884,0x20000900);arena[0x900:0x901+len(name)]=name.encode()+b'\0';
  for offset,value in opt.get('patch',[]):w(offset,value)
  if 'actor_matrix'in opt:f(0x102c,*opt['actor_matrix'])
  if 'shape_matrix'in opt:f(0x800,*opt['shape_matrix'])
  return struct.pack('<12I',0,0,0x500,0x800,0,0,0,0,0,0,0,0)+arena
 else:
  z=opt['z'];name=opt['name']
  arena=bytearray(65536)
  def w(at,*v):struct.pack_into('<'+'I'*len(v),arena,at,*v)
  def f(at,*v):struct.pack_into('<'+'f'*len(v),arena,at,*v)
  w(0,0x20001000);struct.pack_into('<HH',arena,0x174,opt.get('face_start',0),opt.get('face_end',1));w(0x500,0);f(0x504,-1,-1,-1,1,1,1);f(0x102c,1,0,0,0,1,0,0,0,1,0,0,0);f(0x800,1,0,0,0,1,0,0,0,1,0,0,0);w(0x2000,0x20000880);f(0x2004,-2,-2,z,2,-2,z,0,2,z);f(0x2034,0,0,1);w(0x2040,0x75);w(0x884,0x20000900);arena[0x900:0x901+len(name)]=name.encode()+b'\0';
  w(0x2040,opt.get('flags',0));f(0x8c,*opt.get('object_matrix',[1,0,0,0,1,0,0,0,1,0,0,0]))
  f(0x600,*opt.get('sphere_data',[1,0,0,0,1]));f(0x700,0,3,0);f(0x720,0,-3,0)
  if 'vertices'in opt:f(0x2004,*opt['vertices'])
  if 'sphere_start'in opt:f(0x700,*opt['sphere_start'])
  if 'sphere_end'in opt:f(0x720,*opt['sphere_end'])
  for offset,value in opt.get('patch',[]):w(offset,value)
  if 'actor_matrix'in opt:f(0x102c,*opt['actor_matrix'])
  if 'shape_matrix'in opt:f(0x800,*opt['shape_matrix'])
  arena[0x2050:0x20a0]=arena[0x2000:0x2050]
  for i in range(max(0,opt.get('count',0))):
   f(opt.get('contacts',0x8100)+96*i,2);f(opt.get('contacts',0x8100)+96*i+0x14,10+i,10+i,10+i);f(opt.get('contacts',0x8100)+96*i+0x2c,0,0,1)
  return struct.pack('<12I',opt['mode'],0,0x600,0x800,opt.get('count',0)&0xffffffff,0x700,0x720,opt.get('contacts',0x8100),0,0x2004,0x2010,0x2000)+arena

def main():
    global BOUNDS_MODE,ENTRIES
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE');p.add_argument('--output',type=Path,required=True);p.add_argument('--candidate-physics',type=Path);p.add_argument('--candidate-geometry',type=Path);p.add_argument('--case-index',type=int,action='append');p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);p.add_argument('--bounds-only',action='store_true',help='Focused25 original/native3/linked GetNewBoundingBox regression; void EAX excluded');args=p.parse_args()
    BOUNDS_MODE=args.bounds_only
    if BOUNDS_MODE:ENTRIES=[('GetNewBoundingBox',0x45f5b0,213)]*3
    if bool(args.recompiled)!=bool(args.symbols):p.error('Supply both immutable rebuilt PE and hash-bound symbol map')
    out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)
    paths=[PHYSICS,PHYSICS.with_suffix('.h'),GEOMETRY,GEOMETRY.with_suffix('.h'),MATRIX,Path(__file__),Path(loader.__file__),ROOT/'scripts/verify_tinted_native_reconstruction.py',ROOT/'scripts/verify_opponent_navigation_reconstruction.py',ROOT/'src/carma2/include/rec2_types.h',ROOT/'src/carma2/hooks/c2_hooks.h']+[path for path in [args.candidate_physics,args.candidate_geometry]if path]
    before={str(path):hashlib.sha256(path.read_bytes()).hexdigest()for path in paths}
    binary=args.original.read_bytes();assert hashlib.sha256(binary).hexdigest()==HASH
    addresses={a:a for _,a,_ in ENTRIES};addresses.update({0x5964c0:0x5964c0,0x744820:0x744820})
    code,original_meta=physical_harness(binary,addresses);(out/'original.c').write_text(code)
    native=native_harness(args.candidate_physics,args.candidate_geometry);(out/'harness.c').write_text(native);linked=None
    if args.recompiled:
        rebuilt=args.recompiled.read_bytes();mapping=json.loads(args.symbols.read_text());assert mapping['format']==1 and mapping['original_sha256']==HASH and mapping['recompiled_sha256']==hashlib.sha256(rebuilt).hexdigest()
        addresses={v['orig']:v['recomp']for v in mapping['symbols']};code,linked=physical_harness(rebuilt,addresses);(out/'matching.c').write_text(code)
        mpath=args.recompiled.with_suffix('.map');maptext=mpath.read_text();functions={int(a,16):name for name,a in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+',maptext,re.M)}
        linked.update(recompiled_sha256=hashlib.sha256(rebuilt).hexdigest(),symbols_sha256=hashlib.sha256(args.symbols.read_bytes()).hexdigest(),map_sha256=hashlib.sha256(mpath.read_bytes()).hexdigest(),function_names={a:functions.get(int(a,16),'unnamed genuine executable entry')for a in linked['entries']})
    inc=[ROOT/v for v in ['src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/carma2/platform','src/carma2/s3/include','src/brender/include']]
    m5=ROOT/'build-tools/msvc500sp3';e5=dict(os.environ);e5['PATH']=str(m5/'bin')+os.pathsep+str(m5/'redist')+os.pathsep+e5['PATH'];e5['INCLUDE']=str(m5/'include');e5['LIB']=str(m5/'lib')
    r=ROOT/'build-tools/msvc-modern';ms=sorted((r/'VC/Tools/MSVC').iterdir())[-1];sdk=r/'Windows Kits/10';v=sorted((sdk/'Include').iterdir())[-1].name;e=dict(os.environ);e['PATH']=str(ms/'bin/Hostx64/x86')+os.pathsep+e['PATH'];e['INCLUDE']=os.pathsep.join(map(str,[ms/'include',*[sdk/f'Include/{v}/{x}'for x in ['ucrt','shared','um']]]));e['LIB']=os.pathsep.join(map(str,[ms/'lib/x86',*[sdk/f'Lib/{v}/{x}/x86'for x in ['ucrt','um']]]))
    caller_command=compile_caller(out,e5);gcc=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'
    commands={
     'msvc5':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(x)for x in inc],str(out/'harness.c'),'/Fo'+str(out/'msvc5.obj'),'/Fe'+str(out/'msvc5.exe'),str(out/'caller.obj')],
     'msvc':[str(ms/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(x)for x in inc],str(out/'harness.c'),'/Fo'+str(out/'msvc.obj'),'/Fe'+str(out/'msvc.exe'),str(out/'caller.obj')],
     'mingw':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation',*['-I'+str(x)for x in inc],str(out/'harness.c'),'-o',str(out/'mingw.exe'),str(out/'caller.obj')],
     'original':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-DREC2_MATCHING',*['-I'+str(x)for x in inc],str(out/'original.c'),'-o',str(out/'original.exe'),str(out/'caller.obj')]}
    if linked:commands['matching']=[*commands['original'][:-4],str(out/'matching.c'),'-o',str(out/'matching.exe'),str(out/'caller.obj')]
    for tag,command in commands.items():
        run=subprocess.run(command,cwd=out,env=e5 if tag=='msvc5'else e if tag=='msvc'else None,text=True,capture_output=True);(out/(tag+'-build.txt')).write_text(run.stdout+run.stderr);assert run.returncode==0,run.stdout+run.stderr
    if BOUNDS_MODE:
        options,fixtures=bounds_fixtures();assert len(fixtures)==25
    else:
        options=scenarios();fixtures=[make_fixture(o)for o in options];assert len(fixtures)==419 and hashlib.sha256(b''.join(fixtures)).hexdigest()==FIXTURE_SHA256,'Fixture preservation failed'
    if args.case_index is not None:assert all(0<=i<len(fixtures)for i in args.case_index)
    results=[]
    for i,(options_,fixture)in enumerate(zip(options,fixtures)):
        if args.case_index is not None and i not in args.case_index:continue
        path=out/f'case-{i}.bin';path.write_bytes(fixture);original=subprocess.run([str(out/'original.exe'),str(path),str(args.original.resolve())],capture_output=True);assert original.returncode==0 and len(original.stdout)==65540,(i,original.returncode)
        for tag in ['msvc5','msvc','mingw']+(['matching']if linked else []):
            run=subprocess.run([str(out/(tag+'.exe')),str(path)]+([str(args.recompiled.resolve())]if tag=='matching'else []),capture_output=True)
            if run.returncode or run.stdout!=original.stdout:
                (out/f'failure-{i}-{tag}.json').write_text(json.dumps(dict(case=options_,exit=run.returncode,first_difference=next((j for j,(a,b)in enumerate(zip(original.stdout,run.stdout))if a!=b),None)),indent=2));(out/f'failure-{i}-{tag}.expected.bin').write_bytes(original.stdout);(out/f'failure-{i}-{tag}.actual.bin').write_bytes(run.stdout);raise AssertionError((i,tag))
            results.append(dict(case_index=i,compiler=tag,all_bytes_match=True,input_sha256=hashlib.sha256(fixture).hexdigest(),output_sha256=hashlib.sha256(run.stdout).hexdigest()))
        if(i+1)%30==0:print('Passed',i+1,'/',len(fixtures),flush=True)
    # If another lane adds unrelated49 functions, retain honest whole-file hash
    # changes and require exact extracted native-body identity instead.
    after={str(path):hashlib.sha256(path.read_bytes()).hexdigest()for path in paths};assert native_harness(args.candidate_physics,args.candidate_geometry)==native,'Used native body changed during proof'
    report=dict(original_sha256=HASH,entries=ENTRIES,consumer='0x4228b0 full consumer still incomplete',scope_count=1 if BOUNDS_MODE else 3,bounds_only=BOUNDS_MODE,return_scope='Void EAX excluded; raw ABI/arena compared'if BOUNDS_MODE else'Defined int EAX/raw ABI/arena compared',scenes=len(fixtures),comparisons=len(results),complete_suite=args.case_index is None,fixture_input_sha256=hashlib.sha256(b''.join(fixtures)).hexdigest(),source_before_sha256=before,source_after_sha256=after,whole_sources_stable=before==after,used_native_source_stable=True,native_harness_sha256=hashlib.sha256(native.encode()).hexdigest(),physical_original=original_meta,matching_physical=linked,commands=dict(caller=caller_command,**commands),generated_sha256={path.name:hashlib.sha256(path.read_bytes()).hexdigest()for path in out.iterdir()if path.suffix in('.c','.obj','.exe')},results=results,limitations=__doc__)
    (out/'verification.json').write_text(json.dumps(report,indent=2)+'\n');(out/'verifier.py').write_bytes(Path(__file__).read_bytes());print('PASS',len(results)//(4 if linked else 3),len(results))

if __name__=='__main__':main()
