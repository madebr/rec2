"""Verify genuine quadratic wall-edge contact0x424990 against physical original x86.

Requires scoped x86 MSVC5SP3/modern MSVC/MinGW and Capstone5. The bounded318
fixtures cover linear/quadratic and second-root paths, fallback routing, oblique
finite geometry/exponent spread, transforms, valid output/source/frame aliases,
IEEE/paired NaN inputs and interpolation cancellation. Actual lower fallback
and BRender matrix bodies execute at PC53/masked exceptions. Raw ECX/EDX plus
8stack arguments/RET32 or native10-slot cdecl ABI, defined integer return,
ESP/nonvolatile registers/x87TOP and all65536 arena bytes compare unchanged.

--candidate-physics supports frozen development snapshots; default extracts
actual production bodies. --recompiled/--symbols require an immutable hash-bound
linked pair. No public scalar normalization, invented lexical identity, arbitrary
invalid-pointer, sticky flag/unmasked trap or whole-game claim. The full wall
consumer0x4228b0 remains incomplete; private helpers do not count as entries.
"""
import argparse,ast,hashlib,inspect,json,math,os,random,re,struct,subprocess,sys
from pathlib import Path
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'src/carma2').is_dir())
sys.path.insert(0,str(ROOT/'scripts'))
import verify_wall_sphere_edges_reconstruction as prior
import verify_wall_contact_reconstruction as loader
from verify_tinted_native_reconstruction import extract
HASH=prior.HASH;PHYSICS=prior.PHYSICS;GEOMETRY=prior.GEOMETRY;MATRIX=prior.MATRIX
ENTRIES=[('FUN_00424990',0x424990,3225)]
PREFIX=prior.PREFIX+'#define gPhysics_contact_679438 (*(tU32*)(arena+0x7ff0))\n'
DECLS=prior.DECLS.replace('builder_stack[9]','builder_stack[12]')
def compile_caller(out,env5):
    prior.compile_caller(out,env5)
    path=out/'caller.c';path.write_text(path.read_text().replace('builder_stack[9]','builder_stack[12]'))
    command=[str(ROOT/'build-tools/msvc500sp3/bin/cl.exe'),'/nologo','/O2','/Zl','/c',str(path),'/Fo'+str(out/'caller.obj')]
    run=subprocess.run(command,cwd=out,env=env5,text=True,capture_output=True);(out/'caller-build.txt').write_text(run.stdout+run.stderr);assert run.returncode==0,run.stdout+run.stderr
    return command
DRIVER='int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned h[12],values[12],i;\n if(argc!=ARGS)return 2;SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);arena=VirtualAlloc((void*)0x20000000,65536,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(!arena)return 3;\n f=fopen(argv[1],"rb");if(!f)return 4;if(fread(h,sizeof(h),1,f)!=1||fread(arena,65536,1,f)!=1)return 5;fclose(f);INSTALL\n _controlfp(_PC_53,_MCW_PC);for(i=0;i<10;i++)values[i]=h[i]?((unsigned)arena+h[i]):0;values[7]=h[7];gPhysics_contact_679438=h[10];\n builder_address=(unsigned)entries[0];builder_mode=0;builder_x=values[0];builder_y=values[1];builder_poison=0xcccccccc;\n #ifdef REC2_MATCHING\n builder_cdecl=0;builder_stack_count=8;for(i=2;i<10;i++)builder_stack[i-2]=values[i];\n #else\n builder_cdecl=1;builder_stack_count=10;for(i=0;i<10;i++)builder_stack[i]=values[i];\n #endif\n if(BuilderInvoke())return 14;\n _setmode(_fileno(stdout),_O_BINARY);fwrite(&builder_result,4,1,stdout);fwrite(arena,65536,1,stdout);return 0;}'
def root_driver():return DRIVER
def physical_harness(binary,addresses):
    # Retain the actual closed-graph walker, narrowed to one authentic root.
    # Matrix-only dependencies require no replacement world callback.
    source=inspect.getsource(loader.physical_harness)
    injection="""            for op in i.operands:

                if op.type==CS_OP_MEM and op.mem.disp==addresses[0x679438]:refs[a-base+i.disp_offset]=3
                face=addresses[0x744820]

                if op.type==CS_OP_IMM and face<=op.imm<face+300*80:

                    assert i.imm_size==4;refs[a-base+i.imm_offset]=2

                if op.type==CS_OP_MEM and face<=op.mem.disp<face+300*80:

                    assert i.disp_size==4;refs[a-base+i.disp_offset]=2

"""
    assert source.count("            if i.mnemonic=='call':")==1
    source=source.replace("            if i.mnemonic=='call':",injection+"            if i.mnemonic=='call':")
    source=source.replace('entries[2]','entries[1]').replace('offsets[2]','offsets[1]').replace('starts[:2]','starts[:1]').replace('i<2','i<1')
    source=source.replace("refs[i][1]?(unsigned)&gPling_materials:","refs[i][1]==3?(unsigned)&gPhysics_contact_679438:refs[i][1]==2?(unsigned)(arena+0x2000)+value-FACE_ADDRESS:refs[i][1]?(unsigned)&gPling_materials:")
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
    body=PHYSICS.read_text();matrix=MATRIX.read_text()
    chosen=candidate_physics.read_text()if candidate_physics else body
    code=PREFIX+DECLS
    for n in ['PhysicsCopyWord','PhysicsSubtractScalar','PhysicsScalar','PhysicsWallNaN']:code+=extract(body,n)+'\n'
    for n in ['BrMatrix34ApplyP','BrMatrix34ApplyV','BrMatrix34TApplyV']:code+=extract(matrix,n)+'\n'
    code+=extract(body,'FUN_00425630')+'\n'
    code+=extract(chosen,'FUN_00424990')+'\n'
    return code+'static void* entries[1];\n'+DRIVER.replace('ARGS','2').replace('INSTALL','entries[0]=(void*)FUN_00424990;')
def scenarios():
    cases=[]
    for other in [False,True]:
        for output_time in [0.,.25,.5,1.,2.]:cases.append(dict(other=other,t=output_time))
    for change in [(0,0,0),(0,.5,0),(0,-.5,0),(0,1,0)]:
        for velocity in [(0,2,0),(0,2,1),(0,2,-1)]:
            for flag in [0,1]:cases.append(dict(change=change,velocity=velocity,flag=flag))
    for origin in [(.5,-1,0),(.5,1,0),(-.5,-1,0),(1.5,-1,0)]:cases.append(dict(moving=origin))
    rng=random.Random(424990)
    for k in range(48):
        a=tuple(rng.uniform(-3,3)for _ in range(3));d=tuple(rng.uniform(-2,2)for _ in range(3));e=tuple(rng.uniform(-2,2)for _ in range(3));vel=tuple(rng.uniform(-3,3)for _ in range(3));w=tuple(rng.uniform(-1,1)for _ in range(3))if k>=24 else(0,0,0)
        hit_time=.5;u=.3;along=.6
        moving=tuple(a[j]+along*d[j]-u*(e[j]+hit_time*w[j])-hit_time*vel[j]for j in range(3))
        cases.append(dict(old=a,direction=d,moving=moving,edge=e,velocity=vel,change=w,other=bool(k%2),label='oblique-known-intersection',flag=0))
    matrices=[(1.25,.125,-.25, -.375,.75,.25, .125,-.5,1.5, .75,-1.25,2.),(-1,0,0,0,.5,0,0,0,2,-3,4,-5),(1,0,0,0,1,0,0,0,1,.25,.5,-.75)]
    for k in range(24):
        scene=dict(cases[38+k]);scene.update(other=True,matrix0=matrices[k%3],matrix1=matrices[(k+1)%3],actor1=matrices[(k+2)%3],label='distinct-current-previous-matrix')
        cases.append(scene)
    for source in range(6):
        for field in [0x14,0x20,0x2c]:
            for shift in [0,4]:
                cases.append(dict(cases[38+(source%4)],output=0x100+12*source-field+shift,label='valid-output-source-alias',other=bool(shift)))
    for source in range(6):
        for axis in range(3):
            for bits in [0x7f800000,0xff800000,0x7fc12345,0x7f812345]:
                cases.append(dict(cases[38+source],raw=[(0x100+12*source+4*axis,bits)],label='single-source-IEEE',other=bool(axis%2)))
    for bits in [0x7fc12345,0x7f812345,0x7f800000,0xff800000,0x80000000]:cases.append(dict(cases[39],raw=[(0x8000,bits)],label='IEEE-output-time'))
    for first,second in [(0x7fc10001,0x7fc20002),(0x7f810001,0xffc20002),(0xffc20002,0x7f810001)]:
        for offsets in [(0x104,0x11c),(0x108,0x118),(0x13c,0x140)]:cases.append(dict(cases[38],raw=[(offsets[0],first),(offsets[1],second)],label='distinct-paired-NaNs'))
    identity=(1,0,0,0,1,0,0,0,1,0,0,0)
    for scale in [-1.,-.9999999403953552,-1.0000001192092896,0.]:
        matrix=(scale,0,0,0,scale,0,0,0,scale,0,0,0)
        for seed in [0,38,39]:cases.append(dict(cases[seed],other=True,matrix1=matrix,actor1=identity,label='normal-interpolation-cancellation'))
    for a,b in [(0x7fc10001,0x7fc20002),(0x7f810001,0xffc20002),(0xffc20002,0x7f810001)]:
        for offsets in [(0x1490,0x2430),(0x14b0,0x14b4),(0x10b0,0x10b4),(0x1090,0x1094)]:cases.append(dict(cases[39],other=True,raw=[(offsets[0],a),(offsets[1],b)],label='paired-matrix-NaNs'))
    scales=[(-12,-12,-12),(-3,-3,-3),(3,3,3),(12,12,12),(20,0,-20),(-20,0,20)]
    for seed in [38,39,40,41,62,63,64,65]:
        for exponents in scales:
            scene=dict(cases[seed]);scene['label']='finite-exponent-spread'
            for name in ['old','direction','moving','edge','velocity','change']:scene[name]=tuple(math.ldexp(scene[name][j],exponents[j])for j in range(3))
            cases.append(scene)
    for output in [0x1060,0x1460,0x1064,0x1464,0x1078,0x1478,0x106c,0x146c,0x2000,0x2400,0x2004,0x2404]:
        cases.append(dict(cases[39],other=True,output=output,label='valid-output-transform-storage-alias'))
    for other in [False,True]:cases.append(dict(old=(0,0,0),direction=(1048576.,0,1048576.),moving=(524288.,-1.,524288.),edge=(0,1,2.**-34),velocity=(0,2,0),change=(0,0,0),other=other,label='orientation-binade-half-ULP'))
    return cases

def make_fixture(opt):
    arena=bytearray(b'\xa5'*65536)
    def f(at,*values):struct.pack_into('<'+'f'*len(values),arena,at,*values)
    for i,values in enumerate([opt.get('old',(0,0,0)),opt.get('direction',(1,0,0)),opt.get('moving',(.5,-1,0)),opt.get('edge',(0,0,1)),opt.get('velocity',(0,2,0)),opt.get('change',(0,0,0))]):f(0x100+12*i,*values)
    output=opt.get('output',0x8000);f(output,opt.get('t',2.))
    for object_,actor in [(0x1000,0x2000),(0x1400,0x2400)]:
        struct.pack_into('<I',arena,object_,0x20000000+actor)
        f(object_+0x8c,*opt.get('matrix0'if object_==0x1000 else'matrix1',[1,0,0,0,1,0,0,0,1,0,0,0]));f(actor+0x2c,*opt.get('actor1'if actor==0x2400 else'actor0',[1,0,0,0,1,0,0,0,1,0,0,0]))
    for at,bits in opt.get('raw',[]):struct.pack_into('<I',arena,at,bits)
    return struct.pack('<12I',*[0x100+12*i for i in range(6)],output,0,0x1000,0x1400 if opt.get('other') else 0,opt.get('flag',0),0)+arena

FIXTURE_SHA256='cf50cee81f81c71161646e6c9f1e10e279d80e3ceb484d7cecec6d7b75fc18bf'
def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE');p.add_argument('--output',type=Path,required=True);p.add_argument('--candidate-physics',type=Path);p.add_argument('--case-index',type=int,action='append');p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);args=p.parse_args();args.candidate_geometry=None
    if bool(args.recompiled)!=bool(args.symbols):p.error('Supply both immutable rebuilt PE and hash-bound symbol map')
    out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)
    paths=[PHYSICS,PHYSICS.with_suffix('.h'),GEOMETRY,GEOMETRY.with_suffix('.h'),MATRIX,Path(__file__),Path(loader.__file__),Path(prior.__file__),ROOT/'scripts/verify_tinted_native_reconstruction.py',ROOT/'scripts/verify_opponent_navigation_reconstruction.py',ROOT/'src/carma2/include/rec2_types.h',ROOT/'src/carma2/hooks/c2_hooks.h']+[path for path in [args.candidate_physics,args.candidate_geometry]if path]
    before={str(path):hashlib.sha256(path.read_bytes()).hexdigest()for path in paths}
    binary=args.original.read_bytes();assert hashlib.sha256(binary).hexdigest()==HASH
    addresses={a:a for _,a,_ in ENTRIES};addresses.update({0x5964c0:0x5964c0,0x744820:0x744820,0x679438:0x679438})
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
    options=scenarios();fixtures=[make_fixture(o)for o in options];assert len(fixtures)==318 and hashlib.sha256(b''.join(fixtures)).hexdigest()==FIXTURE_SHA256,'Fixture preservation failed'
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
    report=dict(original_sha256=HASH,entries=ENTRIES,consumer='0x4228b0 full consumer still incomplete',scope_count=1,return_scope='Defined int EAX/raw ABI/arena compared',scenes=len(fixtures),comparisons=len(results),complete_suite=args.case_index is None,fixture_input_sha256=hashlib.sha256(b''.join(fixtures)).hexdigest(),source_before_sha256=before,source_after_sha256=after,whole_sources_stable=before==after,used_native_source_stable=True,native_harness_sha256=hashlib.sha256(native.encode()).hexdigest(),physical_original=original_meta,matching_physical=linked,commands=dict(caller=caller_command,**commands),generated_sha256={path.name:hashlib.sha256(path.read_bytes()).hexdigest()for path in out.iterdir()if path.suffix in('.c','.obj','.exe')},results=results,limitations=__doc__)
    (out/'verification.json').write_text(json.dumps(report,indent=2)+'\n');(out/'verifier.py').write_bytes(Path(__file__).read_bytes());print('PASS',len(results)//(4 if linked else 3),len(results))
if __name__=='__main__':main()
