"""Complete edge/corner wall prerequisites, physical original/native3/optional linked.

Requires scoped x86 MSVC5/modern MSVC/MinGW and Capstone5. Actual rays,
CheckSingleFace, matrix and contact insertion bodies execute at PC53 masked FP
exceptions. All65536 arena bytes, int return, stack/nonvolatile registers and
x87 TOP compare without normalization.510 preserved bounded fixtures cover
finite/nonaxis geometry, capacity/flags, opposing faces, real output aliases and
paired IEEE values. No arbitrary pointer, unmasked trap, sticky flag or whole-game
claim. Full0x4228b0 consumer remains incomplete. Semantic name of0x4236a0 unproved.
"""
import argparse,ast,hashlib,inspect,json,math,os,random,re,struct,subprocess,sys
from pathlib import Path
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'src/carma2').is_dir())
sys.path.insert(0,str(ROOT/'scripts'))
import verify_wall_sphere_edges_reconstruction as prior
import verify_wall_contact_reconstruction as loader
from verify_tinted_native_reconstruction import extract
HASH=prior.HASH;PHYSICS=prior.PHYSICS;GEOMETRY=prior.GEOMETRY;MATRIX=prior.MATRIX
ENTRIES=[('GetCornerToFacePoints',0x423ed0,614),('FUN_004236a0',0x4236a0,1375)]
PREFIX=prior.PREFIX+'#define gPhysics_contact_679434 (*(tU32*)(arena+0x7ffc))\n'
DECLS=prior.DECLS
compile_caller=prior.compile_caller
DRIVER='\nint C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned h[12],values[8],i;\n if(argc!=ARGS)return 2;SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);arena=VirtualAlloc((void*)0x20000000,65536,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(!arena)return 3;\n f=fopen(argv[1],"rb");if(!f)return 4;if(fread(h,sizeof(h),1,f)!=1||fread(arena,65536,1,f)!=1)return 5;fclose(f);INSTALL\n _controlfp(_PC_53,_MCW_PC);values[0]=(unsigned)(arena+h[0]);values[1]=(unsigned)(arena+h[1]);values[2]=(unsigned)(arena+h[2]);values[3]=h[3];values[4]=h[4];values[5]=(unsigned)(arena+h[5]);values[6]=(unsigned)(arena+h[6]);values[7]=(unsigned)(arena+h[7]);\n builder_address=(unsigned)entries[h[8]];builder_mode=0;builder_x=values[0];builder_y=values[1];builder_poison=0xcccccccc;\n #ifdef REC2_MATCHING\n builder_cdecl=0;builder_stack_count=h[8]?6:5;for(i=2;i<(h[8]?8u:7u);i++)builder_stack[i-2]=values[i];\n #else\n builder_cdecl=1;builder_stack_count=h[8]?8:7;for(i=0;i<(h[8]?8u:7u);i++)builder_stack[i]=values[i];\n #endif\n if(BuilderInvoke())return 14;\n _setmode(_fileno(stdout),_O_BINARY);fwrite(&builder_result,4,1,stdout);fwrite(arena,65536,1,stdout);return 0;}\n'

def root_driver():return DRIVER

def physical_harness(binary,addresses):
    # Extend the retained actual-closure walker to two roots and the genuine
    #300-face table. Indexed face field operands retain their addend.
    source=inspect.getsource(loader.physical_harness)
    injection="""            for op in i.operands:
                if op.type==CS_OP_MEM and op.mem.disp==addresses[0x679434]:refs[a-base+i.disp_offset]=3
                face=addresses[0x744820]
                if op.type==CS_OP_IMM and face<=op.imm<face+300*80:
                    assert i.imm_size==4;refs[a-base+i.imm_offset]=2
                if op.type==CS_OP_MEM and face<=op.mem.disp<face+300*80:
                    assert i.disp_size==4;refs[a-base+i.disp_offset]=2
"""
    assert source.count("            if i.mnemonic=='call':")==1
    source=source.replace("            if i.mnemonic=='call':",injection+"            if i.mnemonic=='call':")
    source=source.replace("refs[i][1]?(unsigned)&gPling_materials:","refs[i][1]==3?(unsigned)&gPhysics_contact_679434:refs[i][1]==2?(unsigned)(arena+0x2000)+value-FACE_ADDRESS:refs[i][1]?(unsigned)&gPling_materials:")
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

def native_harness(candidate_physics,candidate_geometry=None):
    assert candidate_geometry is None
    geo=GEOMETRY.read_text();body=PHYSICS.read_text();matrix=MATRIX.read_text()
    candidate=candidate_physics.read_text()if candidate_physics else body
    code=PREFIX+DECLS
    end=geo.index('} tFinteray_projection;')+len('} tFinteray_projection;');start=geo.rfind('typedef struct {',0,end)
    code+=geo[start:end]+'\n'
    for n in ['FinterayScalar','FinterayFaceProjection','FinterayFacePoint','CheckSingleFace']:code+=extract(geo,n)+'\n'
    for n in ['PhysicsCopyWord','PhysicsSubtractScalar','PhysicsScalar','PhysicsScalarStore','PhysicsWallNaN','AddObjectWallForce','RayCastThruFaceList']:code+=extract(body,n)+'\n'
    for n in ['Matrix34ApplyProductLoaded','Matrix34ApplyProductMemory','Matrix34ApplyAdd','BrMatrix34ApplyP','BrMatrix34TApplyV']:code+=extract(matrix,n)+'\n'
    if candidate_physics:
        end=candidate.index('} tPhysics_wall_workspace;')+len('} tPhysics_wall_workspace;');start=candidate.rfind('typedef struct {',0,end)
        code+=candidate[start:end]+'\n'
    code+='typedef char workspace_layout[sizeof(tPhysics_wall_workspace)==0x708 && offsetof(tPhysics_wall_workspace,vertex_flags)==0x600 && offsetof(tPhysics_wall_workspace,count_points)==0x700 && offsetof(tPhysics_wall_workspace,count_edges)==0x702 && offsetof(tPhysics_wall_workspace,edges)==0x704 ? 1:-1];\n'
    for n in ['WallEdgeAlignment','GetCornerToFacePoints','FUN_004236a0']:code+=extract(candidate,n)+'\n'
    return code+'static void* entries[2];\n'+DRIVER.replace('ARGS','2').replace('INSTALL','entries[0]=(void*)GetCornerToFacePoints;entries[1]=(void*)FUN_004236a0;')

def options_0():
    cases=[]
    for capacity in [0,3,4,40]:
     for points in [0,1,2,64]:cases.append(dict(capacity=capacity,points=points))
    for count in [0,3,4,5]:
     for flags in [0,1]:cases.append(dict(points=2,count=count,flags=flags))
    for z in [0.,.0001,3.,-3.]:cases.append(dict(points=1,z=z,end_z=-z))
    for matrix in [[1,0,0,0,1,0,0,0,1,.1,-.2,.3],[1,.3,.2,-.4,1,.5,.7,-.2,1,.25,-.5,.75]]:cases.append(dict(points=2,matrix=matrix))
    for motion in [(0,0,0),(1.,.3,-3.),(-1.,.8,-3.),(1e-7,0,3.),(2.384185791015625e-7,0,3.)]:cases.append(dict(points=2,motion=motion))
    for xy in [(.3,.4),(-.8,.2),(1.5,1.5),(2.,2.)]:cases.append(dict(points=2,xy=xy))
    for capacity in [-2147483648,-1,4,2147483647]:cases.append(dict(points=2,capacity=capacity))
    for alias in [0x3600,0x3300,0x8c,0x7ffc]:cases.append(dict(points=2,contacts=alias))
    for offset in [0x3000,0x3004,0x3008,0x3300,0x3304,0x3308,0x8c,0x90,0x94,0xb0,0xb4,0xb8]:
     for word in [0x80000000,1,0x7f7fffff,0x7f800000,0xff800000,0x7fc12345,0x7f812345]:cases.append(dict(points=2,patch=[(offset,word)]))
    for offsets in [(0x3000,0x3300),(0x3004,0x3304),(0x3008,0x3308),(0x8c,0x90),(0xb0,0xb4)]:
     for pair in [(0x7fc12345,0xffc54321),(0x7f812345,0xffc54321),(0xff812345,0x7f854321)]:cases.append(dict(points=2,patch=list(zip(offsets,pair))))
    import random,math
    rng=random.Random(0x4236a0)
    for _ in range(96):
     a,b=rng.uniform(-.8,.8),rng.uniform(-.8,.8)
     xy=(rng.uniform(-.8,.8),rng.uniform(-.8,.8))
     endxy=(rng.uniform(-.8,.8),rng.uniform(-.8,.8))
     cases.append(dict(points=2,edges=2,xy=xy,xy_end=endxy,motion=(*endxy,-3.),centre=(rng.uniform(-2,2),rng.uniform(-2,2),rng.uniform(-2,2)),tilt=(a,b),matrix=[1,.13,-.07,-.11,1,.17,.19,-.23,1,.1,-.2,.3]))
    return cases

def fixture_0(opt):
    arena=bytearray(65536)
    def w(at,value):struct.pack_into('<I',arena,at,value)
    def f(at,*value):struct.pack_into('<'+'f'*len(value),arena,at,*value)
    w(0,0x20001000);f(0x14,0,0,0);struct.pack_into('<HH',arena,0x174,0,1);f(0x8c,*opt.get('matrix',[1,0,0,0,1,0,0,0,1,0,0,0]));w(0x2000,0x20000880);f(0x2004,-2,-2,0,2,-2,0,0,2,0);f(0x2034,0,0,1);w(0x884,0x20000900);arena[0x900:0x909]=b'ordinary\0';w(0x7ffc,0x31415926)
    for j in range(64):
     f(0x3000+j*12,*opt.get('xy',((j%3-1)*.25,0)),opt.get('z',3.));f(0x3300+j*12,*opt.get('motion',(*opt.get('xy',((j%3-1)*.25,0)),opt.get('end_z',-3.))));w(0x3600+j*4,opt.get('flags',0))
    struct.pack_into('<HHI',arena,0x3700,opt['points'],0,0)
    if 'tilt' in opt:
     a,b=opt['tilt'];length=math.sqrt(a*a+b*b+1)
     f(0x2004,-2,-2,2*a+2*b,2,-2,-2*a+2*b,0,2,-2*b);f(0x2034,a/length,b/length,1/length)
    for at,word in opt.get('patch',[]):w(at,word)
    for j in range(opt.get('count',0)):
     f(0x8100+j*96,2);f(0x8100+j*96+0x14,10+j,10+j,10+j);f(0x8100+j*96+0x2c,0,0,1)
    return struct.pack('<12I',0,0x500,opt.get('contacts',0x8100),opt.get('count',0),opt.get('capacity',40)&0xffffffff,0x3000,0x800,0,0,0,0,0)+arena


def options_1():
    cases=[]
    for shape in [0,4]:
     for edges in [0,1,2,32]:cases.append(dict(shape=shape,edges=edges))
    for count in [0,3,4,5]:cases.append(dict(edges=1,count=count))
    for first_z,second_z in [(3.,-3.),(-3.,3.),(3.,4.),(-3.,-4.),(0.,0.)]:
     for centre_z in [-2.,0.,2.]:cases.append(dict(edges=1,z=first_z,end_z=second_z,centre_z=centre_z))
    for matrix in [[1,0,0,0,1,0,0,0,1,.1,-.2,.3],[1,.3,.2,-.4,1,.5,.7,-.2,1,.25,-.5,.75]]:cases.append(dict(edges=2,matrix=matrix))
    # Bounded geometry, selected-hit branches and real output aliases.
    for centre_z in [-2.,-.9,-.5,-.1,0.,2.]:
     for swap in [False,True]:cases.append(dict(edges=1,faces=2,centre_z=centre_z,swap_faces=swap))
    for xy in [(.3,.4),(-.8,.2),(1.5,1.5),(2.,2.)]:
     for centre in [(0.,0.,2.),(.7,-.3,0.)]:cases.append(dict(edges=1,xy=xy,centre=centre,faces=2))
    for xy_end in [(0.,0.),(.5,.2),(-.7,.3)]:cases.append(dict(edges=2,xy=(.1,-.2),xy_end=xy_end,faces=2,centre=(.5,.7,2.)))
    for alias in [0x3600,0x3300,0x102c,0x7ff8]:cases.append(dict(edges=2,contacts=alias,faces=2))
    for centre_z in [.009999999776482582,.05,2.384185791015625e-7]:
     for bitsdelta in [-1,0,1]:cases.append(dict(edges=1,centre_z_bitsdelta=bitsdelta,centre_z=centre_z,xy=(1.,0.)))
    for offset in [0x14,0x18,0x1c,0x8c,0x90,0x94,0xb0,0xb4,0xb8,0x102c,0x1030,0x1034]:
     for word in [0x80000000,1,0x7f7fffff,0x7f800000,0xff800000,0x7fc12345,0x7f812345]:cases.append(dict(edges=1,patch=[(offset,word)],faces=2))
    for offsets in [(0x14,0x18),(0x1c,0x8c),(0x8c,0x90),(0xb0,0xb4),(0x102c,0x1030)]:
     for pair in [(0x7fc12345,0xffc54321),(0x7f812345,0xffc54321),(0xff812345,0x7f854321)]:cases.append(dict(edges=2,faces=2,patch=list(zip(offsets,pair))))
    import random,math
    rng=random.Random(0x4236a0)
    for _ in range(96):
     a,b=rng.uniform(-.8,.8),rng.uniform(-.8,.8)
     xy=(rng.uniform(-.8,.8),rng.uniform(-.8,.8))
     endxy=(rng.uniform(-.8,.8),rng.uniform(-.8,.8))
     cases.append(dict(points=2,edges=2,xy=xy,xy_end=endxy,motion=(*endxy,-3.),centre=(rng.uniform(-2,2),rng.uniform(-2,2),rng.uniform(-2,2)),tilt=(a,b),matrix=[1,.13,-.07,-.11,1,.17,.19,-.23,1,.1,-.2,.3]))
    for centre_z in [-2.,2.]:
     for previous_z in [-.5,.5]:
      for swap in [False,True]:cases.append(dict(edges=1,faces=2,centre_z=centre_z,previous_z=previous_z,swap_faces=swap,assert_selected_face=True))
    return cases

def fixture_1(opt):
    arena=bytearray(65536)
    def w(at,value):struct.pack_into('<I',arena,at,value)
    def f(at,*value):struct.pack_into('<'+'f'*len(value),arena,at,*value)
    w(0,0x20001000);w(0x500,opt.get('shape',0));f(0x14,*opt.get('centre',(0,0,opt.get('centre_z',2.))));f(0x102c,*[1,0,0,0,1,0,0,0,1,0,0,opt.get('previous_z',-.5)]);struct.pack_into('<HH',arena,0x174,0,opt.get('faces',1));f(0x8c,*opt.get('matrix',[1,0,0,0,1,0,0,0,1,0,0,0]));w(0x2000,0x20000880);f(0x2004,-2,-2,0,2,-2,0,0,2,0);f(0x2034,0,0,1);w(0x884,0x20000900);arena[0x900:0x909]=b'ordinary\0';w(0x7ffc,0x31415926)
    for j in range(64):
     f(0x3000+j*12,*opt.get('xy'if j%2==0 else 'xy_end',opt.get('xy',(0,0))),opt.get('z',3.)if j%2==0 else opt.get('end_z',-3.));f(0x3300+j*12,(j%3-1)*.25,0,opt.get('end_z',-3.));w(0x3600+j*4,opt.get('flags',0))
    struct.pack_into('<HHI',arena,0x3700,64,opt.get('edges',0),0x20003800)
    for j in range(32):arena[0x3800+j*2:0x3802+j*2]=bytes([j*2,j*2+1])
    w(0x7ff8,0x31415926)
    if opt.get('faces',1)==2:
     w(0x2050,0x20000880);f(0x2054,-2,-2,-1,0,2,-1,2,-2,-1);f(0x2084,0,0,-1)
     if opt.get('swap_faces'):arena[0x2000:0x20a0]=arena[0x2050:0x20a0]+arena[0x2000:0x2050]
    if 'centre_z_bitsdelta' in opt:w(0x1c,struct.unpack_from('<I',arena,0x1c)[0]+opt['centre_z_bitsdelta'])
    if 'tilt' in opt:
     a,b=opt['tilt'];length=math.sqrt(a*a+b*b+1)
     f(0x2004,-2,-2,2*a+2*b,2,-2,-2*a+2*b,0,2,-2*b);f(0x2034,a/length,b/length,1/length)
    for at,word in opt.get('patch',[]):w(at,word)
    for j in range(opt.get('count',0)):
     f(0x8100+j*96,2);f(0x8100+j*96+0x14,10+j,10+j,10+j);f(0x8100+j*96+0x2c,0,0,1)
    return struct.pack('<12I',0,0x500,opt.get('contacts',0x8100),opt.get('count',0),opt.get('capacity',40),0x3000,0x800,0x7ff8,1,0,0,0)+arena


def scenarios():return [dict(entry=0,**o)for o in options_0()]+[dict(entry=1,**o)for o in options_1()]
def make_fixture(opt):return (fixture_0 if opt['entry']==0 else fixture_1)(opt)
FIXTURE_SHA256='236d58f242c0fcb76bf6bfd911698af1c805de739d99d8fa5c82ec30db8a9d3a'
def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE');p.add_argument('--output',type=Path,required=True);p.add_argument('--candidate-physics',type=Path);p.add_argument('--case-index',type=int,action='append');p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);args=p.parse_args();args.candidate_geometry=None
    if bool(args.recompiled)!=bool(args.symbols):p.error('Supply both immutable rebuilt PE and hash-bound symbol map')
    out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)
    paths=[PHYSICS,PHYSICS.with_suffix('.h'),GEOMETRY,GEOMETRY.with_suffix('.h'),MATRIX,Path(__file__),Path(loader.__file__),Path(prior.__file__),ROOT/'scripts/verify_tinted_native_reconstruction.py',ROOT/'scripts/verify_opponent_navigation_reconstruction.py',ROOT/'src/carma2/include/rec2_types.h',ROOT/'src/carma2/hooks/c2_hooks.h']+[path for path in [args.candidate_physics,args.candidate_geometry]if path]
    before={str(path):hashlib.sha256(path.read_bytes()).hexdigest()for path in paths}
    binary=args.original.read_bytes();assert hashlib.sha256(binary).hexdigest()==HASH
    addresses={a:a for _,a,_ in ENTRIES};addresses.update({0x5964c0:0x5964c0,0x744820:0x744820,0x679434:0x679434})
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
    options=scenarios();fixtures=[make_fixture(o)for o in options];assert len(fixtures)==510 and hashlib.sha256(b''.join(fixtures)).hexdigest()==FIXTURE_SHA256,'Fixture preservation failed'
    if args.case_index is not None:assert all(0<=i<len(fixtures)for i in args.case_index)
    results=[]
    for i,(options_,fixture)in enumerate(zip(options,fixtures)):
        if args.case_index is not None and i not in args.case_index:continue
        path=out/f'case-{i}.bin';path.write_bytes(fixture);original=subprocess.run([str(out/'original.exe'),str(path),str(args.original.resolve())],capture_output=True);assert original.returncode==0 and len(original.stdout)==65540,(i,original.returncode)
        if options_.get('assert_selected_face'):
            count=struct.unpack_from('<I',original.stdout)[0];touched=struct.unpack_from('<I',original.stdout,4+0x7ff8)[0];assert touched==1
            if count:assert struct.unpack_from('<I',original.stdout,4+0x8100+12)[0]==(0x20002050 if options_.get('swap_faces') else 0x20002000),'Original retained forward face changed'
        for tag in ['msvc5','msvc','mingw']+(['matching']if linked else []):
            run=subprocess.run([str(out/(tag+'.exe')),str(path)]+([str(args.recompiled.resolve())]if tag=='matching'else []),capture_output=True)
            if run.returncode or run.stdout!=original.stdout:
                (out/f'failure-{i}-{tag}.json').write_text(json.dumps(dict(case=options_,exit=run.returncode,first_difference=next((j for j,(a,b)in enumerate(zip(original.stdout,run.stdout))if a!=b),None)),indent=2));(out/f'failure-{i}-{tag}.expected.bin').write_bytes(original.stdout);(out/f'failure-{i}-{tag}.actual.bin').write_bytes(run.stdout);raise AssertionError((i,tag))
            results.append(dict(case_index=i,compiler=tag,all_bytes_match=True,input_sha256=hashlib.sha256(fixture).hexdigest(),output_sha256=hashlib.sha256(run.stdout).hexdigest()))
        if(i+1)%30==0:print('Passed',i+1,'/',len(fixtures),flush=True)
    # If another lane adds unrelated49 functions, retain honest whole-file hash
    # changes and require exact extracted native-body identity instead.
    after={str(path):hashlib.sha256(path.read_bytes()).hexdigest()for path in paths};assert native_harness(args.candidate_physics,args.candidate_geometry)==native,'Used native body changed during proof'
    report=dict(original_sha256=HASH,entries=ENTRIES,consumer='0x4228b0 full consumer still incomplete',scope_count=2,return_scope='Defined int EAX/raw ABI/arena compared',scenes=len(fixtures),comparisons=len(results),complete_suite=args.case_index is None,fixture_input_sha256=hashlib.sha256(b''.join(fixtures)).hexdigest(),source_before_sha256=before,source_after_sha256=after,whole_sources_stable=before==after,used_native_source_stable=True,native_harness_sha256=hashlib.sha256(native.encode()).hexdigest(),physical_original=original_meta,matching_physical=linked,commands=dict(caller=caller_command,**commands),generated_sha256={path.name:hashlib.sha256(path.read_bytes()).hexdigest()for path in out.iterdir()if path.suffix in('.c','.obj','.exe')},results=results,limitations=__doc__)
    (out/'verification.json').write_text(json.dumps(report,indent=2)+'\n');(out/'verifier.py').write_bytes(Path(__file__).read_bytes());print('PASS',len(results)//(4 if linked else 3),len(results))

if __name__=='__main__':main()
