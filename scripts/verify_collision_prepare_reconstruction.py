"""Prove collision-face preparation against original physical x87/native3.
Original0x4c2060 is a genuine1342byte entry; PrepareForCollisions identity is
inferred from its established49placeholder and original callsite/purpose proof.
Requires Capstone5, scoped MSVC5/modernMSVC/i686MinGW. Real geometry, matrix,
column and box dependencies execute; public world callbacks are controlled.
"""
import argparse,hashlib,json,os,struct,subprocess
from pathlib import Path
import verify_find_faces_box_reconstruction as boxes
from verify_box_pick_reconstruction import ROOT,HASH,FUNCTIONS,words
from verify_tinted_native_reconstruction import extract
PHYSICS=boxes.PHYSICS
GEOMETRY=boxes.GEOMETRY
MATRIX=boxes.MATRIX
PREP_ENTRIES=boxes.ENTRIES+[('BrMatrix34Identity',0x5327c0,47),('BrMatrix34Copy',0x5325d0,79),('PrepareForCollisions',0x4c2060,1342)]

def physical_harness(binary,addresses,wrappers):
    """Relocate only decoded reachable instructions in an approved closed graph."""
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_MEM,CS_OP_IMM
    nt=struct.unpack_from('<I',binary,60)[0];base=struct.unpack_from('<I',binary,nt+52)[0];image_size=struct.unpack_from('<I',binary,nt+80)[0];tab=nt+24+struct.unpack_from('<H',binary,nt+20)[0]
    segments=[]
    for i in range(struct.unpack_from('<H',binary,nt+6)[0]):
        _,va,size,offset=struct.unpack_from('<IIII',binary,tab+40*i+8);flags=struct.unpack_from('<I',binary,tab+40*i+36)[0];segments.append((va,size,offset,flags))
    def raw(a):
        for va,n,off,flags in segments:
            if flags&0x20 and va<=a-base<va+n:return binary[off+a-base-va:off+a-base-va+15]
        raise ValueError(f'Closure outside code {a:x}')
    globals_={addresses[0x5964c4]:(0,0),addresses[0x79d858]:(1,0),addresses[0x6b75c0]:(2,0),addresses[0x6b7880]:(3,0)}
    globals_.update({addresses[0x65d010]:(6,0),addresses[0x6940b0]:(7,0),addresses[0x744820]:(8,0)})
    track=addresses[0x75bb80]+0x1ba0
    for offset in (0,24):globals_[track+offset]=(4,offset)
    starts=[addresses[a] for name,a,n in PREP_ENTRIES];matrix={starts[i]:i-4 for i in range(4,9)};matrix.update({starts[12]:5,starts[13]:6})
    cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;pending=list(starts);entries=set(starts);instructions={};references={};calls={}
    while pending:
        function=pending.pop();todo=[function];visited=set()
        while todo:
            a=todo.pop()
            if a in visited:continue
            visited.add(a);ins=next(cs.disasm(raw(a),a,count=1),None)
            if ins is None:raise ValueError(f'Invalid instruction {a:x}')
            instructions[a]=ins.bytes.hex()
            for op in ins.operands:
                if op.type==CS_OP_MEM and not op.mem.base and not op.mem.index:
                    value=op.mem.disp
                    if not(base<=value<base+image_size) or ins.disp_size!=4:raise ValueError(f'Unexpected absolute memory {a:x}/{value:x}')
                    references[a-base+ins.disp_offset]=globals_.get(value,(5,0))
            # The original pushes track and forms face-list addresses with MOV/ADD.
            for op in ins.operands:
                if op.type==CS_OP_IMM and ins.mnemonic in ('push','mov','add') and op.imm in globals_:
                    assert ins.imm_size==4;references[a-base+ins.imm_offset]=globals_[op.imm]
            if ins.mnemonic=='call':
                if ins.operands[0].type==CS_OP_IMM:
                    target=ins.operands[0].imm;raw(target)
                    if target not in entries:entries.add(target);pending.append(target)
                    if target in matrix:calls[a-base+ins.imm_offset]=matrix[target]
                elif function not in (starts[0],starts[-1]) or (ins.operands[0].type==CS_OP_MEM and not ins.operands[0].mem.base):raise ValueError(f'Unapproved indirect call {a:x}')
            if ins.mnemonic=='ret':continue
            if ins.mnemonic.startswith('j'):
                if ins.operands[0].type!=CS_OP_IMM:raise ValueError('Indirect jump in closure')
                todo.append(ins.operands[0].imm)
                if ins.mnemonic=='jmp':continue
            todo.append(a+ins.size)
            if len(instructions)>12000:raise ValueError('Closure instruction budget exceeded')
    s=scaffold()+'\nstatic void* matrix_code[7];\nstatic void* wrapper_code;\nstatic void* query_code;\n'
    cases=''
    for i,((sig,body,call),(name,a,n)) in enumerate(zip(wrappers,FUNCTIONS[4:]+PREP_ENTRIES[12:14])):
        s+=f'typedef void (C2_HOOK_CDECL *matrix_fn_{i})({sig});\n'+body.replace('CALL',f'((matrix_fn_{i})matrix_code[{i}])({call})')
        cases+=f'case {i}:target=(unsigned){name};break;'
    s+='static const unsigned refs[][3]={'+','.join('{'+','.join(map(str,[site,*kind]))+'}' for site,kind in sorted(references.items()))+'};\n'
    s+='static const unsigned calls[][2]={'+','.join('{'+str(site)+','+str(kind)+'}' for site,kind in sorted(calls.items()))+'};\n'
    s+='static const unsigned matrix_offsets[7]={'+','.join(hex(starts[i]-base) for i in (4,5,6,7,8,12,13))+'};\n'
    s+=r'''
static int install_original(const char*name){
 FILE*f;unsigned char*disk,*image;unsigned size,i,site,target,value;IMAGE_DOS_HEADER*dos;IMAGE_NT_HEADERS32*nt;IMAGE_SECTION_HEADER*sec;
 f=fopen(name,"rb");if(!f)return 91;fseek(f,0,SEEK_END);size=(unsigned)ftell(f);rewind(f);disk=(unsigned char*)malloc(size);if(!disk||fread(disk,size,1,f)!=1)return 92;fclose(f);
 dos=(IMAGE_DOS_HEADER*)disk;nt=(IMAGE_NT_HEADERS32*)(disk+dos->e_lfanew);sec=IMAGE_FIRST_SECTION(nt);
 image=(unsigned char*)VirtualAlloc(NULL,nt->OptionalHeader.SizeOfImage,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image)return 93;
 memcpy(image,disk,nt->OptionalHeader.SizeOfHeaders);for(i=0;i<nt->FileHeader.NumberOfSections;i++)memcpy(image+sec[i].VirtualAddress,disk+sec[i].PointerToRawData,sec[i].SizeOfRawData);
 for(i=0;i<sizeof(refs)/sizeof(refs[0]);i++){
  site=(unsigned)image+refs[i][0];memcpy(&value,(void*)site,4);target=refs[i][1];
  value=target==0?(unsigned)&gActorBoxPick_StopGroovidelics:target==1?(unsigned)&gPling_face:target==2?(unsigned)&gAdditional_actors:target==3?(unsigned)&gBlack_material:target==4?(unsigned)&gProgram_state.track_spec+refs[i][2]:target==6?(unsigned)&gFace_num__car:target==7?(unsigned)&gFace_count:target==8?(unsigned)gFace_list__car:(unsigned)image+value-nt->OptionalHeader.ImageBase;memcpy((void*)site,&value,4);
 }
 for(i=0;i<sizeof(calls)/sizeof(calls[0]);i++){site=(unsigned)image+calls[i][0];target=calls[i][1];switch(target){WRAPPERS}value=target-(site+4);memcpy((void*)site,&value,4);}
 for(i=0;i<7;i++)matrix_code[i]=image+matrix_offsets[i];wrapper_code=image+WRAPPER_OFFSET;query_code=image+QUERY_OFFSET;free(disk);return 0;
}
typedef void (C2_HOOK_FASTCALL *wrapper_fn)(tPhysics_object*,tWorld_callbacks*);
'''.replace('WRAPPERS',cases).replace('WRAPPER_OFFSET',hex(starts[-1]-base)).replace('QUERY_OFFSET',hex(starts[11]-base))
    s+=QUERY_BODY.replace('QUERY_CALL','((query_fn)query_code)(b,f,n,w)')
    s+=DRIVER.replace('ARGS','3').replace('INSTALL','result=install_original(argv[2]);if(result)return result;').replace('CALL','((wrapper_fn)wrapper_code)((tPhysics_object*)(arena+0x9000),callbacks)')
    return s,dict(closure_entries=[hex(v) for v in sorted(entries)],instruction_count=len(instructions),instruction_sha256=hashlib.sha256(json.dumps(sorted(instructions.items())).encode()).hexdigest(),operand_relocations=sorted(references.items()),matrix_call_sites=sorted(calls.items()))

QUERY_BODY=r'''
typedef int(C2_HOOK_FASTCALL*query_fn)(tBounds*,tFace_ref*,int,tWorld_callbacks*);
static int C2_HOOK_FASTCALL find(tBounds*b,tFace_ref*f,int n,tWorld_callbacks*w){
 int r;event(52,w,f,NULL,0,0,n);event(50,&b->original_bounds,NULL,b->mat,24,0,0);r=QUERY_CALL;query_count++;
 if(query_count==1){
  if(header[12]&1)gFace_count=300;
  if(header[12]&2)gFace_count=0;
  if(header[12]&4)gFace_num__car=(int)((unsigned)gFace_num__car+1u);
  if(header[12]&8)world()->find_faces_in_box=NULL;
  if(header[12]&16)world()->new_face_list=NULL;
  if(header[12]&32)b->original_bounds.min.v[0]=.25f;
  if(header[12]&64)gPling_face=gFace_list__car+17;
  if(header[12]&128)gActorBoxPick_StopGroovidelics=0;
 }
 event(50,&b->original_bounds,NULL,b->mat,24,0,1);return r;
}
static void C2_HOOK_FASTCALL new_faces(tPhysics_object*p,undefined4*pling){
 event(51,p,pling,NULL,0,0,0);
 if(header[13]&1)p->flags|=0x80;
 if(header[13]&2)p->flags&=~0x80;
 if(header[13]&4)p->box_face_start=17;
 if(header[13]&8)gFace_count=77;
 if(header[13]&16)gFace_num__car=0x10001;
 event(51,p,pling,NULL,0,0,1);
}
'''
DRIVER=boxes.DRIVER.replace('result=CALL;','CALL;result=0;')
DRIVER=DRIVER.replace('memcpy(&gProgram_state.track_spec,arena+0x3800,36);','gFace_count=(int)header[10];gFace_num__car=(int)header[11];world()->find_faces_in_box=header[14]?find:NULL;world()->new_face_list=header[15]?new_faces:NULL;memcpy(&gProgram_state.track_spec,arena+0x3800,36);')

def scaffold():
    s=boxes.scaffold().replace('header[10]','header[20]')
    s=s.replace('static int mutation_done;',r'''
static int mutation_done;
static unsigned query_count;
int gFace_count,gFace_num__car;
#define gFace_list__car (*(tFace_ref(*)[300])(arena+0xa000))
static int C2_HOOK_FASTCALL find(tBounds*,tFace_ref*,int,tWorld_callbacks*);
static void C2_HOOK_FASTCALL new_faces(tPhysics_object*,undefined4*);
''')
    s=s.replace('memcpy(copy,arena,sizeof(copy));','memcpy(copy,arena,sizeof(copy));memcpy(&x,copy+0x2608,4);if(x==(unsigned)new_faces){x=0x108f020;memcpy(copy+0x2608,&x,4);}memcpy(&x,copy+0x260c,4);if(x==(unsigned)find){x=0x108f030;memcpy(copy+0x260c,&x,4);}')
    s=s.replace('fwrite(owners,sizeof(owners),1,stdout);','fwrite(owners,sizeof(owners),1,stdout);}{unsigned counters[3];counters[0]=(unsigned)gFace_count;counters[1]=(unsigned)gFace_num__car;counters[2]=query_count;fwrite(counters,sizeof(counters),1,stdout);')
    return s+'\n#define M(x,y) mat->m[x][y]\n'

def native_harness():
    s,wrappers=boxes.native_harness();s=s[:s.index('int C2_HOOK_CDECL main(int argc')]
    prefix_end=s.index('void C2_HOOK_CDECL NativeBrMatrix34Mul(')
    # Reuse the exact verified box bodies and matrices, replacing only fixture prefix.
    s=scaffold()+s[prefix_end:]
    for i,name in enumerate(('BrMatrix34Identity','BrMatrix34Copy')):
        s+='\n'+extract(MATRIX.read_text(),name).replace(name,'Native'+name)
        sig='br_matrix34*A' if i==0 else 'br_matrix34*A,const br_matrix34*B';call='A' if i==0 else 'A,B';input_='NULL' if i==0 else '(void*)B';input_size=0 if i==0 else 48
        body=f'void C2_HOOK_CDECL {name}({sig}){{event({i+6},A,{input_},NULL,48,{input_size},0);CALL;event({i+6},A,{input_},NULL,48,{input_size},1);}}\n'
        s+=body.replace('CALL','Native'+name+'('+call+')');wrappers.append((sig,body,call))
    for name in ('PhysicsScalarNaN','PhysicsLessOrUnordered','PhysicsGreaterOrdered','PhysicsScalar','PhysicsScalarStore','PrepareForCollisions'):s+='\n'+extract(PHYSICS.read_text(),name)
    s+=QUERY_BODY.replace('QUERY_CALL','FindFacesInBox(b,f,n,w)')
    s+=DRIVER.replace('ARGS','2').replace('INSTALL','').replace('CALL','PrepareForCollisions((tPhysics_object*)(arena+0x9000),callbacks)')
    return s,wrappers

def scene(**kw):
    o=dict(flags=1,count=0,epoch=1,ref=0,start=0,end=3,find_present=1,new_present=1,query_mutate=0,new_mutate=0,parent=0,v=[0,0,0],bb=[-.5,-.5,-.5,.5,.5,.5],cache=[-10,-10,-10,10,10,10],object_matrix=[1,0,0,0,1,0,0,0,1,0,0,0],actor_matrix=[1,0,0,0,1,0,0,0,1,0,0,0],inverse=[1,0,0,0,1,0,0,0,1,0,0,0],patch=[])
    o.update(kw);c=boxes.scene(actor_matrix=o['actor_matrix']);a=bytearray(c['data'])
    def wr(off,value):a[off:off+4]=words([value])
    def fl(off,values):a[off:off+4*len(values)]=struct.pack('<'+'f'*len(values),*values)
    for off in (0x9000,0x9500):a[off:off+0x4d8]=bytes(0x4d8)
    wr(0x9000,0x20000180);fl(0x9024,o['bb']);fl(0x903c,o['bb']);fl(0x9068,o['v']);fl(0x908c,o['object_matrix']);fl(0x910c,o['bb']);fl(0x9124,o['cache']);fl(0x9144,o['inverse']);struct.pack_into('<3H',a,0x9174,o['start']&65535,o['end']&65535,o['ref']&65535);wr(0x919c,o['flags']);wr(0x9228,0x20009500 if o['parent'] else 0);struct.pack_into('<3H',a,0x9674,17,23,29)
    a[0xa000:0xfdc0]=b'\xcc'*24000
    for off,value in o['patch']:wr(off,value)
    c['header'] += [o['count'],o['epoch'],o['query_mutate'],o['new_mutate'],o['find_present'],o['new_present'],0,0,0,0];c['data']=bytes(a);c['options']=o;return c

def scenarios():
    c=[scene(flags=f,count=n,ref=r) for f in (0,1,0x40,2,0x80,0x81) for n in (0,298,299,300) for r in (0,1)]
    c += [scene(flags=f,parent=p) for f in (2,3,0x42,0x82) for p in (0,1)]
    c += [scene(flags=f,count=n,epoch=e,ref=r,start=s) for f in (0,1) for n,e,r,s in ((5,1,1,0),(5,2,1,4),(5,2,1,6),(5,0x10001,1,6),(5,0,65535,6),(5,1,1,0))]
    c += [scene(flags=f,cache=cache) for f in (0,1) for cache in ([-.5,-.5,-.5,.5,.5,.5],[-.4,-.4,-.4,.4,.4,.4],[-20,-20,-20,20,20,20])]
    c += [scene(flags=f,v=v) for f in (0,1) for v in ([1,-1,0],[.333333,-.234567,.987654],[0,0,0])]
    c += [scene(flags=f,query_mutate=m,count=n) for f in (0,1) for m in (1,2,4,16,32,64,128) for n in (0,299)]
    c += [scene(flags=f,new_mutate=m) for f in (0,1,0x80,0x81) for m in (1,2,4,8,16)]
    c += [scene(flags=f,find_present=fp,new_present=np) for f in (0,1) for fp,np in ((0,0),(0,1),(1,0))]
    c += [scene(flags=1,ref=1,patch=[(0x910c+off,value)]) for off in (0,12) for value in (0x7fc12345,0x7f800001)]
    c += [scene(flags=1,patch=[(0x9068+off,value)]) for off in (0,4,8) for value in (0x7fc12345,0x7f800001,0x7f800000)]
    c += [scene(flags=0,patch=[(0x180+0x2c+off,value)]) for off in (0,16) for value in (0x7fc12345,0x7f800001)]
    c += [scene(flags=0,object_matrix=m,actor_matrix=a) for m,a in (
        ([0,1,0,-1,0,0,0,0,1,.1,-.2,.3],[1,0,0,0,1,0,0,0,1,.3,.2,-.1]),
        ([.8,.6,0,-.6,.8,0,0,0,1,.123456,-.345678,.56789],[.6,-.8,0,.8,.6,0,0,0,1,-.271828,.314159,.141421]),
        ([1,0,0,0,1,0,0,0,1,.2,-.3,.4],[0,0,-1,0,1,0,1,0,0,-.3,.2,.1]))]
    c += [scene(flags=1,count=299,epoch=e) for e in (0xffffffff,0x7fffffff,0x10000)]
    c += [scene(flags=1,count=299,query_mutate=8,expect_fault=True)]
    return c

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE');p.add_argument('--output',type=Path,default=ROOT/'build-audit/physics-collision-next/tests');p.add_argument('--case-index',type=int,action='append');p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);args=p.parse_args()
    if bool(args.recompiled)!=bool(args.symbols):p.error('Supply both immutable PE and symbol map')
    out=args.output.resolve();out.mkdir(parents=True,exist_ok=True);original=args.original.read_bytes();assert hashlib.sha256(original).hexdigest()==HASH
    a={v:v for name,v,n in PREP_ENTRIES};a.update({v:v for v in (0x5964c4,0x79d858,0x6b75c0,0x6b7880,0x75bb80,0x65d010,0x6940b0,0x744820)})
    native,wrappers=native_harness();probe,original_meta=physical_harness(original,a,wrappers);(out/'harness.c').write_text(native);(out/'physical.c').write_text(probe)
    includes=[ROOT/v for v in ('src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/carma2/platform','src/carma2/s3/include','src/brender/core/math','src/brender/include')]
    root=ROOT/'build-tools/msvc-modern';ms=sorted((root/'VC/Tools/MSVC').iterdir())[-1];sdk=root/'Windows Kits/10';version=sorted((sdk/'Include').iterdir())[-1].name;env=dict(os.environ);env['PATH']=str(ms/'bin/Hostx64/x86')+os.pathsep+env['PATH'];env['INCLUDE']=os.pathsep.join(map(str,[ms/'include',*[sdk/f'Include/{version}/{part}' for part in ('ucrt','shared','um')]]));env['LIB']=os.pathsep.join(map(str,[ms/'lib/x86',*[sdk/f'Lib/{version}/{part}/x86' for part in ('ucrt','um')]]))
    m5=ROOT/'build-tools/msvc500sp3';env5=dict(os.environ);env5['PATH']=str(m5/'bin')+os.pathsep+str(m5/'redist')+os.pathsep+env5['PATH'];env5['INCLUDE']=str(m5/'include');env5['LIB']=str(m5/'lib');gcc=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'
    commands={
      'msvc':[str(ms/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(v) for v in includes],str(out/'harness.c'),'/Fo'+str(out/'msvc.obj'),'/Fe'+str(out/'msvc.exe')],
      'msvc5':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(v) for v in includes],str(out/'harness.c'),'/Fo'+str(out/'msvc5.obj'),'/Fe'+str(out/'msvc5.exe')],
      'mingw':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation',*['-I'+str(v) for v in includes],str(out/'harness.c'),'-o',str(out/'mingw.exe')],
      'physical':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-DREC2_MATCHING',*['-I'+str(v) for v in includes],str(out/'physical.c'),'-o',str(out/'physical.exe')]}
    linked=None
    if args.recompiled:
        rebuilt=args.recompiled.read_bytes();m=json.loads(args.symbols.read_text());assert m['format']==1 and m['original_sha256']==HASH and m['recompiled_sha256']==hashlib.sha256(rebuilt).hexdigest();a={v['orig']:v['recomp'] for v in m['symbols']};matching,linked=physical_harness(rebuilt,a,wrappers);(out/'physical-matching.c').write_text(matching);commands['matching']=[*commands['physical'][:-3],str(out/'physical-matching.c'),'-o',str(out/'matching.exe')];linked.update(recompiled_sha256=hashlib.sha256(rebuilt).hexdigest(),symbols_sha256=hashlib.sha256(args.symbols.read_bytes()).hexdigest())
    for compiler,command in commands.items():
        r=subprocess.run(command,cwd=out,env=env if compiler=='msvc' else env5 if compiler=='msvc5' else None,text=True,capture_output=True);(out/f'{compiler}-build.txt').write_text(r.stdout+r.stderr)
        if r.returncode:raise RuntimeError(r.stdout+r.stderr)
    cases=scenarios();results=[]
    for i,c in enumerate(cases):
        if args.case_index is not None and i not in args.case_index:continue
        fixture=out/f'case-{i:03d}.bin';fixture.write_bytes(words(c['header'])+c['data']);r=subprocess.run([str(out/'physical.exe'),str(fixture),str(args.original.resolve())],capture_output=True);expected=r.stdout;exit_code=r.returncode&0xffffffff
        assert exit_code==(0xc0000005 if c['options'].get('expect_fault') else 0),f'Original case{i} exits {exit_code:x}'
        (out/f'case-{i:03d}.physical.bin').write_bytes(expected)
        for compiler in ('msvc5','msvc','mingw',*(['matching'] if linked else [])):
            run=subprocess.run([str(out/f'{compiler}.exe'),str(fixture),*([str(args.recompiled.resolve())] if compiler=='matching' else [])],capture_output=True);actual=run.stdout
            if actual!=expected or (run.returncode&0xffffffff)!=exit_code:
                prefix=out/f'failure-{compiler}-{i}';prefix.with_suffix('.expected.bin').write_bytes(expected);prefix.with_suffix('.actual.bin').write_bytes(actual);prefix.with_suffix('.json').write_text(json.dumps(dict(options=c['options'],first_difference=next((j for j,(x,y) in enumerate(zip(actual,expected)) if x!=y),None),expected_size=len(expected),actual_size=len(actual),original_exit=hex(exit_code),native_exit=hex(run.returncode&0xffffffff)),indent=2));raise AssertionError(f'{compiler} preparation case{i} differs: {prefix}')
            results.append(dict(case_index=i,compiler=compiler,all_state_calls_match=True,state_calls_sha256=hashlib.sha256(actual).hexdigest(),exit_code=hex(exit_code)))
        if (i+1)%20==0:print(f'Passed {i+1}/{len(cases)} scenes',flush=True)
    sources=[PHYSICS,MATRIX,GEOMETRY,boxes.COLUMNS,ROOT/'src/carma2/newcommon/00-car.c',ROOT/'src/carma2/newcommon/00-car.h',ROOT/'src/carma2/newcommon/49-physics.h']
    manifest=dict(original_sha256=HASH,scenes=len(cases),comparisons=len(results),all_cases_passed=True,complete_suite=args.case_index is None,functions=PREP_ENTRIES,source_sha256={str(v.relative_to(ROOT)):hashlib.sha256(v.read_bytes()).hexdigest() for v in sources},harness_sha256=hashlib.sha256(native.encode()).hexdigest(),physical_probe_sha256=hashlib.sha256(probe.encode()).hexdigest(),physical_original=original_meta,commands=commands,results=results,identity_caveat='Original metadata is FUN_004c2060. Name maps existing PrepareForCollisions placeholder by collision-face preparation semantics and four original object/world callback callsites.',limitations='Real complete dependencies execute; world callbacks controlled. Only callback slots/private pointer identities translated. Unwritten private tBounds members before FindFacesInBox are not read by fixture; defined bounds and matrix inputs, all public scalar/object/face/global outputs remain exact.')
    if linked:manifest['matching_physical']=linked
    (out/'verification.json').write_text(json.dumps(manifest,indent=2)+'\n');print(f'PASS: {len(cases)} scenes/{len(results)} comparisons')

if __name__=='__main__':main()
