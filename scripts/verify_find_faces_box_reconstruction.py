"""Verify FindFacesInBox using actual bounds, columns and box geometry.

Requires Capstone5 and the scoped MSVC5/modern MSVC/i686 MinGW toolchains.
The hash-checked original closure executes on physical x87 at PC53. Native
harnesses extract actual production bodies/types. Only world callbacks are
controlled; no game entry/import/TLS is run or public scalar normalized.
"""
import argparse, hashlib, json, os, struct, subprocess
from pathlib import Path
from verify_box_pick_reconstruction import ROOT, HASH, FUNCTIONS, SCAFFOLD, make_case as box_case, words
from verify_tinted_native_reconstruction import extract

PHYSICS=ROOT/'src/carma2/newcommon/49-physics.c'
GEOMETRY=ROOT/'src/carma2/newcommon/34-finteray.c'
MATRIX=ROOT/'src/brender/core/math/matrix34.c'
COLUMNS=ROOT/'src/carma2/newcommon/37-brucetrk.c'
ENTRIES=FUNCTIONS+[('FillInBounds',0x45f690,552),('XZToColumnXZ',0x40cb90,184),('FindFacesInBox',0x4b57d0,404)]
SIZE=0x10000
EXTRA=r'''
tProgram_state gProgram_state;
br_actor* gAdditional_actors;
br_material* gBlack_material;
static int mutation_done;
'''
DRIVER=r'''
int C2_HOOK_CDECL main(int argc,char**argv){
 FILE*f;int result;unsigned v[8];tWorld_callbacks*callbacks;
 SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX|SEM_NOOPENFILEERRORBOX);
 if(argc!=ARGS)return 2;f=fopen(argv[1],"rb");if(!f)return 3;if(fread(header,sizeof(header),1,f)!=1)return 4;
 arena=(unsigned char*)VirtualAlloc((void*)0x20000000,0x10000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(arena!=(void*)0x20000000)return 5;if(fread(arena,0x10000,1,f)!=1)return 6;fclose(f);
 if(sizeof(br_actor)!=100||sizeof(br_model)!=92||sizeof(br_material)!=156||sizeof(tBounds)!=68||sizeof(tFace_ref)!=80||sizeof(tTrack_square)!=12||sizeof(tTrack_spec)!=36||offsetof(tProgram_state,track_spec)!=0x1ba0)return 7;
 _controlfp(_PC_53,_MCW_PC);_setmode(_fileno(stdout),_O_BINARY);world()->pull_actor_from_world=pull;world()->stop_groovidelic=stop;
 gActorBoxPick_StopGroovidelics=(int)header[7];gPling_face=(tFace_ref*)(arena+0x2000+400);
 memcpy(&gProgram_state.track_spec,arena+0x3800,36);memcpy(&gAdditional_actors,arena+0x3824,4);memcpy(&gBlack_material,arena+0x3828,4);
 callbacks=header[4]?world():NULL;
 INSTALL
 result=CALL;
 memset(v,0,sizeof(v));v[1]=(unsigned)result;v[2]=events;fwrite(v,sizeof(v),1,stdout);{unsigned char zero[144];memset(zero,0,sizeof(zero));fwrite(zero,sizeof(zero),1,stdout);}snapshot();return 0;
}
'''

def scaffold():
    s=SCAFFOLD.replace('#include "34-finteray.h"','#include "34-finteray.h"\n#include "37-brucetrk.h"\n#include "49-physics.h"\n#include "globvars.h"\n#include "28-world3.h"\n#include "32-spark.h"\n#include <stddef.h>')
    s=s.replace('static unsigned header[10],events;',EXTRA+'\nstatic unsigned header[10],events;')
    s=s.replace('copy[0x4000]','copy[0x10000]').replace('0x20004000','0x20010000')
    s=s.replace('fwrite(copy,sizeof(copy),1,stdout);','fwrite(copy,sizeof(copy),1,stdout);fwrite(&gProgram_state.track_spec,36,1,stdout);{unsigned owners[2];owners[0]=(unsigned)gAdditional_actors;owners[1]=(unsigned)gBlack_material;fwrite(owners,sizeof(owners),1,stdout);}')
    s=s.replace('static void C2_HOOK_CDECL mutate(br_actor* actor){',r'''static void C2_HOOK_CDECL mutate(br_actor* actor){
 if(!mutation_done){
  if(header[6]&1024)gProgram_state.track_spec.columns=(tTrack_square**)(arena+0x5000);
  if(header[6]&2048)gBlack_material=(br_material*)(arena+0x1a00+312);
  if(header[6]&4096)gAdditional_actors=NULL;
  if(header[6]&8192)gAdditional_actors=(br_actor*)(arena+0x180+200);
  if(header[6]&16384){gProgram_state.track_spec.ncolumns_x=0;gProgram_state.track_spec.ncolumns_z=0;}
  if(header[6]&32768){tTrack_square*row=gProgram_state.track_spec.columns[1];row[1].actor_0x0=NULL;}
  mutation_done=1;
 }
''')
    return s

def native_harness():
    s=scaffold()
    for name,a,n in FUNCTIONS[4:]:s+='\n'+extract(MATRIX.read_text(),name).replace(name,'Native'+name)
    wrappers=[]
    for i,(name,a,n) in enumerate(FUNCTIONS[4:]):
        if i==1:sig='br_matrix34* A, const br_matrix34* B';call='A,B';size=48;matrix='NULL'
        elif i==0:sig='br_matrix34* A, const br_matrix34* B, const br_matrix34* C';call='A,B,C';size=48;matrix='(void*)C'
        else:sig='br_vector3* A, const br_vector3* B, const br_matrix34* C';call='A,B,C';size=12;matrix='(void*)C'
        body=f'void C2_HOOK_CDECL {name}({sig}){{event({i+1},A,(void*)B,{matrix},{size},{size},0);CALL;event({i+1},A,(void*)B,{matrix},{size},{size},1);}}\n'
        s+=body.replace('CALL','Native'+name+'('+call+')');wrappers.append((sig,body,call))
    for name in ['FinterayScalar','FinterayBoxNaN','FinterayBoxCopyScalar','FinterayBoxStoreScalar','FinterayBoxClip','ModelPickBox','FinterayBoxBoundsTransform','ActorBoxPick','GetNewBoundingBox','BoundsOverlapTest__finteray','FillInBounds']:s+='\n'+extract(GEOMETRY.read_text(),name)
    s+='\n'+extract(COLUMNS.read_text(),'XZToColumnXZ')+'\n'+extract(PHYSICS.read_text(),'FindFacesInBox')
    s+=DRIVER.replace('ARGS','2').replace('INSTALL','').replace('CALL','FindFacesInBox((tBounds*)arena,(tFace_ref*)(arena+header[2]),(int)header[1],callbacks)')
    return s,wrappers

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
    track=addresses[0x75bb80]+0x1ba0
    for offset in (0,24):globals_[track+offset]=(4,offset)
    starts=[addresses[a] for name,a,n in ENTRIES];matrix={starts[i]:i-4 for i in range(4,9)}
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
            # Track pointer is pushed as an immediate by original wrapper.
            for op in ins.operands:
                if op.type==CS_OP_IMM and ins.mnemonic=='push' and op.imm==track:
                    assert ins.imm_size==4;references[a-base+ins.imm_offset]=(4,0)
            if ins.mnemonic=='call':
                if ins.operands[0].type==CS_OP_IMM:
                    target=ins.operands[0].imm;raw(target)
                    if target not in entries:entries.add(target);pending.append(target)
                    if target in matrix:calls[a-base+ins.imm_offset]=matrix[target]
                elif function!=starts[0] or (ins.operands[0].type==CS_OP_MEM and not ins.operands[0].mem.base):raise ValueError(f'Unapproved indirect call {a:x}')
            if ins.mnemonic=='ret':continue
            if ins.mnemonic.startswith('j'):
                if ins.operands[0].type!=CS_OP_IMM:raise ValueError('Indirect jump in closure')
                todo.append(ins.operands[0].imm)
                if ins.mnemonic=='jmp':continue
            todo.append(a+ins.size)
            if len(instructions)>12000:raise ValueError('Closure instruction budget exceeded')
    s=scaffold()+'\nstatic void* matrix_code[5];\nstatic void* wrapper_code;\n'
    cases=''
    for i,((sig,body,call),(name,a,n)) in enumerate(zip(wrappers,FUNCTIONS[4:])):
        s+=f'typedef void (C2_HOOK_CDECL *matrix_fn_{i})({sig});\n'+body.replace('CALL',f'((matrix_fn_{i})matrix_code[{i}])({call})')
        cases+=f'case {i}:target=(unsigned){name};break;'
    s+='static const unsigned refs[][3]={'+','.join('{'+','.join(map(str,[site,*kind]))+'}' for site,kind in sorted(references.items()))+'};\n'
    s+='static const unsigned calls[][2]={'+','.join('{'+str(site)+','+str(kind)+'}' for site,kind in sorted(calls.items()))+'};\n'
    s+='static const unsigned matrix_offsets[5]={'+','.join(hex(starts[i]-base) for i in range(4,9))+'};\n'
    s+=r'''
static int install_original(const char*name){
 FILE*f;unsigned char*disk,*image;unsigned size,i,site,target,value;IMAGE_DOS_HEADER*dos;IMAGE_NT_HEADERS32*nt;IMAGE_SECTION_HEADER*sec;
 f=fopen(name,"rb");if(!f)return 91;fseek(f,0,SEEK_END);size=(unsigned)ftell(f);rewind(f);disk=(unsigned char*)malloc(size);if(!disk||fread(disk,size,1,f)!=1)return 92;fclose(f);
 dos=(IMAGE_DOS_HEADER*)disk;nt=(IMAGE_NT_HEADERS32*)(disk+dos->e_lfanew);sec=IMAGE_FIRST_SECTION(nt);
 image=(unsigned char*)VirtualAlloc(NULL,nt->OptionalHeader.SizeOfImage,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image)return 93;
 memcpy(image,disk,nt->OptionalHeader.SizeOfHeaders);for(i=0;i<nt->FileHeader.NumberOfSections;i++)memcpy(image+sec[i].VirtualAddress,disk+sec[i].PointerToRawData,sec[i].SizeOfRawData);
 for(i=0;i<sizeof(refs)/sizeof(refs[0]);i++){
  site=(unsigned)image+refs[i][0];memcpy(&value,(void*)site,4);target=refs[i][1];
  value=target==0?(unsigned)&gActorBoxPick_StopGroovidelics:target==1?(unsigned)&gPling_face:target==2?(unsigned)&gAdditional_actors:target==3?(unsigned)&gBlack_material:target==4?(unsigned)&gProgram_state.track_spec+refs[i][2]:(unsigned)image+value-nt->OptionalHeader.ImageBase;memcpy((void*)site,&value,4);
 }
 for(i=0;i<sizeof(calls)/sizeof(calls[0]);i++){site=(unsigned)image+calls[i][0];target=calls[i][1];switch(target){WRAPPERS}value=target-(site+4);memcpy((void*)site,&value,4);}
 for(i=0;i<5;i++)matrix_code[i]=image+matrix_offsets[i];wrapper_code=image+WRAPPER_OFFSET;free(disk);return 0;
}
typedef int (C2_HOOK_FASTCALL *wrapper_fn)(tBounds*,tFace_ref*,int,tWorld_callbacks*);
'''.replace('WRAPPERS',cases).replace('WRAPPER_OFFSET',hex(starts[-1]-base))
    s+=DRIVER.replace('ARGS','3').replace('INSTALL','result=install_original(argv[2]);if(result)return result;').replace('CALL','((wrapper_fn)wrapper_code)((tBounds*)arena,(tFace_ref*)(arena+header[2]),(int)header[1],callbacks)')
    return s,dict(closure_entries=[hex(v) for v in sorted(entries)],instruction_count=len(instructions),instruction_sha256=hashlib.sha256(json.dumps(sorted(instructions.items())).encode()).hexdigest(),operand_relocations=sorted(references.items()),matrix_call_sites=sorted(calls.items()))

def scene(**kw):
    defaults=dict(ncolumns=(4,4),origin=(-2,-2),size=(1,1),grid=[(0,0,0),(1,1,1),(2,2,2),(3,3,3)],additional=4,black=0,callbacks=1,identifier='&1',children=0,capacity=12,material=0,model_bounds=[-20,-20,-20,20,20,20],box_bounds=[-.5,-.5,-.5,.5,.5,.5])
    defaults.update(kw);o=defaults;c=box_case(**o);a=bytearray(c['data'])+bytearray(SIZE-len(c['data']))
    def wr(off,value):a[off:off+4]=words([value])
    def ptr(off,target):wr(off,0x20000000+target if target is not None else 0)
    a[0x3800:0x3824]=bytes(36);a[0x3800],a[0x3801]=o['ncolumns'];struct.pack_into('<ffff',a,0x3804,*o['size'],*o['origin']);ptr(0x3818,0x4000);ptr(0x3824,None if o['additional'] is None else 0x180+o['additional']*100);ptr(0x3828,None if o['black'] is None else 0x1a00+o['black']*156)
    for z in range(256):ptr(0x4000+z*4,0x6000+z*0xc00 if z<4 else 0x4400);ptr(0x5000+z*4,0x5400)
    for x,z,actor in o['grid']:ptr(0x6000+z*0xc00+x*12,0x180+actor*100)
    # Zero-count 254..255 scans use a valid shared sparse row rather than fake guards.
    if o.get('zero_cells'):
        for x in (254,255):ptr(0x4400+x*12,0x180+((x-254)%5)*100)
    ptr(0x5400+1*12,0x180+300);ptr(0x5400+2*12,0x180+200)
    for i in range(5):
        name=('&1node'+str(i)).encode()+b'\0';a[0x1c80+i*24:0x1c80+i*24+len(name)]=name
        if o.get('identifiers') and i<len(o['identifiers']):
            name=o['identifiers'][i].encode()+b'\0';a[0x1c80+i*24:0x1c80+i*24+len(name)]=name
        if o.get('null_actor_material'):ptr(0x180+i*100+0x1c,None)
    if o.get('null_columns'):ptr(0x3818,None)
    for off,value in o.get('wrapper_patch',[]):wr(off,value)
    c['data']=bytes(a);c['options']=o;return c

def scenarios():
    c=[scene(capacity=n,callbacks=cb,black=black,additional=additional) for n in (0,1,2,12,-1) for cb in (0,1) for black in (0,None) for additional in (None,4)]
    c += [scene(box_bounds=b,origin=o,size=s) for b,o,s in [([-3,-1,-3,-2,-.5,-2],(-2,-2),(1,1)),([1,-1,1,2,1,2],(-2,-2),(1,1)),([-20,-1,-20,20,1,20],(-2,-2),(1,1)),([0,-1,0,.0001,1,.0001],(0,0),(.333333,.2)),([.99999994,-1,.99999994,1.00000012,1,1.00000012],(0,0),(1,1))]]
    c += [scene(grid=g,additional=add) for g,add in [([],None),([],4),([(x,z,(x+z)%5) for x in range(4) for z in range(4)],None),([(0,3,0),(3,0,1),(1,2,2)],4)]]
    c += [scene(mutate=m,pull_return=r,capacity=n) for m in (1024,2048,4096,8192,16384,32768,1024|2048|8192,1,2,4,8,16,32,64,128,256,512) for r in (0,1) for n in (1,12)]
    c += [scene(ncolumns=dims,zero_cells=True,grid=[],additional=None) for dims in ((0,0),(0,4),(4,0),(1,1))]
    c += [scene(box_matrix=m,box_bounds=[-.75,-.5,-.25,.75,.5,.25]) for m in ([0,1,0,-1,0,0,0,0,1,0,0,0],[.75,.1,-.2,-.3,1.1,.25,.15,-.05,.9,.1,-.2,.3])]
    c += [scene(wrapper_patch=[(offset,value)]) for offset in (0x3804,0x3808,0x380c,0x3810) for value in (0x7fc12345,0x7f800001,0,0x7f800000)]
    c += [scene(null_actor_material=True,black=black) for black in (0,1,2,None)]
    # Missing prepared/actor materials make live fallback reload observable.
    c += [scene(null_actor_material=True,mutate=m,black=black,capacity=12)
          for m,black in ((2048,0),(2048,None),(1024|2048,0),(8192|2048,1))]
    # Repeated empty cells and row mutations retain endpoint bytes and scan order.
    c += [scene(grid=[(x,z,(x+z)%5) for x in range(4) for z in range(4)],
                mutate=m,capacity=1,null_actor_material=True)
          for m in (16384,32768,1024|16384)]
    c += [scene(identifiers=['ordinary','&1','&0','&9','&'])]
    c += [scene(null_columns=True,expect_fault=True),scene(wrapper_patch=[(0,0)],expect_fault=True)]
    return c

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE');p.add_argument('--output',type=Path,default=ROOT/'build-audit/find-faces-box/tests');p.add_argument('--gcc',type=Path,default=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe');p.add_argument('--msvc-root',type=Path,default=ROOT/'build-tools/msvc-modern');p.add_argument('--msvc5-root',type=Path,default=ROOT/'build-tools/msvc500sp3');p.add_argument('--case-index',type=int,action='append');p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);args=p.parse_args()
    if bool(args.recompiled)!=bool(args.symbols):p.error('Supply both rebuilt binary and matching symbol map')
    out=args.output.resolve();out.mkdir(parents=True,exist_ok=True);original=args.original.read_bytes();assert hashlib.sha256(original).hexdigest()==HASH
    addresses={a:a for name,a,n in ENTRIES};addresses.update({a:a for a in (0x5964c4,0x79d858,0x6b75c0,0x6b7880,0x75bb80)})
    native,wrappers=native_harness();probe,provenance=physical_harness(original,addresses,wrappers);(out/'harness.c').write_text(native);(out/'physical.c').write_text(probe)
    includes=[ROOT/v for v in ('src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/carma2/platform','src/carma2/s3/include','src/brender/core/math','src/brender/include')]
    ms=sorted((args.msvc_root/'VC/Tools/MSVC').iterdir())[-1];sdk=args.msvc_root/'Windows Kits/10';version=sorted((sdk/'Include').iterdir())[-1].name;env=dict(os.environ);env['PATH']=str(ms/'bin/Hostx64/x86')+os.pathsep+env['PATH'];env['INCLUDE']=os.pathsep.join(map(str,[ms/'include',*[sdk/f'Include/{version}/{part}' for part in ('ucrt','shared','um')]]));env['LIB']=os.pathsep.join(map(str,[ms/'lib/x86',*[sdk/f'Lib/{version}/{part}/x86' for part in ('ucrt','um')]]))
    m5=args.msvc5_root;env5=dict(os.environ);env5['PATH']=str(m5/'bin')+os.pathsep+str(m5/'redist')+os.pathsep+env5['PATH'];env5['INCLUDE']=str(m5/'include');env5['LIB']=str(m5/'lib')
    commands={
      'msvc':[str(ms/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(v) for v in includes],str(out/'harness.c'),'/Fo'+str(out/'msvc.obj'),'/Fe'+str(out/'msvc.exe')],
      'msvc5':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(v) for v in includes],str(out/'harness.c'),'/Fo'+str(out/'msvc5.obj'),'/Fe'+str(out/'msvc5.exe')],
      'mingw':[str(args.gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation',*['-I'+str(v) for v in includes],str(out/'harness.c'),'-o',str(out/'mingw.exe')],
      'physical':[str(args.gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-DREC2_MATCHING',*['-I'+str(v) for v in includes],str(out/'physical.c'),'-o',str(out/'physical.exe')]}
    linked=None
    if args.recompiled:
        rebuilt=args.recompiled.read_bytes();m=json.loads(args.symbols.read_text());assert m['format']==1 and m['original_sha256']==HASH and m['recompiled_sha256']==hashlib.sha256(rebuilt).hexdigest();a={v['orig']:v['recomp'] for v in m['symbols']};matching,linked=physical_harness(rebuilt,a,wrappers);(out/'physical-matching.c').write_text(matching);commands['matching']=[*commands['physical'][:-3],str(out/'physical-matching.c'),'-o',str(out/'matching.exe')];linked.update(recompiled_sha256=hashlib.sha256(rebuilt).hexdigest(),symbols_sha256=hashlib.sha256(args.symbols.read_bytes()).hexdigest(),probe_sha256=hashlib.sha256(matching.encode()).hexdigest())
    for compiler,command in commands.items():
        r=subprocess.run(command,cwd=out,env=env if compiler=='msvc' else env5 if compiler=='msvc5' else None,text=True,capture_output=True);(out/f'{compiler}-build.txt').write_text(r.stdout+r.stderr)
        if r.returncode:raise RuntimeError(r.stdout+r.stderr)
    cases=scenarios();results=[]
    if args.case_index is not None and any(i<0 or i>=len(cases) for i in args.case_index):p.error('Case index outside suite')
    for i,c in enumerate(cases):
        if args.case_index is not None and i not in args.case_index:continue
        fixture=out/f'case-{i:03d}.bin';fixture.write_bytes(words(c['header'])+c['data']);reference=subprocess.run([str(out/'physical.exe'),str(fixture),str(args.original.resolve())],capture_output=True);expected=reference.stdout;exit_code=reference.returncode&0xffffffff
        assert exit_code==(0xc0000005 if c['options'].get('expect_fault') else 0),f'Original case{i} unexpectedly exits {exit_code:x}'
        (out/f'case-{i:03d}.physical.bin').write_bytes(expected)
        for compiler in ('msvc5','msvc','mingw',*(['matching'] if linked else [])):
            run=subprocess.run([str(out/f'{compiler}.exe'),str(fixture),*([str(args.recompiled.resolve())] if compiler=='matching' else [])],capture_output=True);actual=run.stdout;actual_exit=run.returncode&0xffffffff
            if actual!=expected or actual_exit!=exit_code:
                prefix=out/f'failure-{compiler}-{i}';prefix.with_suffix('.expected.bin').write_bytes(expected);prefix.with_suffix('.actual.bin').write_bytes(actual);prefix.with_suffix('.json').write_text(json.dumps(dict(options=c['options'],first_difference=next((j for j,(x,y) in enumerate(zip(actual,expected)) if x!=y),None),expected_size=len(expected),actual_size=len(actual),original_exit=hex(exit_code),native_exit=hex(actual_exit)),indent=2));raise AssertionError(f'Wrapper {compiler} case{i} differs: {prefix}')
            results.append(dict(case_index=i,compiler=compiler,all_state_calls_match=True,state_calls_sha256=hashlib.sha256(actual).hexdigest(),exit_code=hex(actual_exit)))
        if (i+1)%20==0:print(f'Passed {i+1}/{len(cases)} scenes',flush=True)
    sources=[PHYSICS,GEOMETRY,MATRIX,COLUMNS,ROOT/'src/carma2/newcommon/49-physics.h',ROOT/'src/carma2/newcommon/32-spark.h',ROOT/'src/carma2/include/rec2_types.h']
    manifest=dict(original_sha256=HASH,scenes=len(cases),comparisons=len(results),complete_suite=args.case_index is None,all_cases_passed=True,results=results,functions=ENTRIES,source_sha256={str(v.relative_to(ROOT)):hashlib.sha256(v.read_bytes()).hexdigest() for v in sources},harness_sha256=hashlib.sha256(native.encode()).hexdigest(),physical_probe_sha256=hashlib.sha256(probe.encode()).hexdigest(),physical_original=provenance,commands=commands,limitations='Controlled world callbacks only. Actual FillInBounds/XZToColumnXZ/ActorBoxPick and matrix bodies execute; original _ftol executes physically. Callback slots/private temporary identities translated; public scalar and grid/global/output bytes exact.')
    if linked:manifest['matching_physical']=linked
    (out/'verification.json').write_text(json.dumps(manifest,indent=2)+'\n');print(f'PASS: {len(cases)} scenes/{len(results)} full-state comparisons')

if __name__=='__main__':main()
