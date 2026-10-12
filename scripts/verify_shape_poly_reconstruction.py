"""Physical original/native3 polygon and shape-dispatch wall-contact proof.

Hash-checked original code executes without game startup, imports or TLS.
Actual geometry, BrVector3SetFloat and PhysicsError bodies run at PC53;
only terminal exit is captured as an explicit process boundary. Full arenas,
error callback state and raw outputs compare without scalar normalization.
Private uninitialized candidate slots use explicit uniform bounded stack
history. This four-entry stage does not complete the full 0x4228b0 consumer.
Requires Capstone5, scoped MSVC5/modern x86 MSVC/i686 MinGW. Optional immutable
rebuilt PE requires its hash-bound symbol map.
"""
import argparse,hashlib,json,os,struct,subprocess,random,re
from pathlib import Path
from verify_box_pick_reconstruction import ROOT,HASH,words,fb
from verify_tinted_native_reconstruction import extract
SOURCE=ROOT/'src/carma2/newcommon/34-finteray.c'
GEOMETRY=SOURCE
PHYSICS=ROOT/'src/carma2/newcommon/49-physics.c'
VECTOR=ROOT/'src/brender/core/math/vector.c'
ENTRIES=[('LinePolyColl',0x461310,518),('LinePolyCollWithSphere',0x461520,530),('ShapeRayCast',0x461740,247),('ShapeRayCastWithSphere',0x461850,121)]
PREFIX=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "34-finteray.h"
#include "49-physics.h"
#include "c2_math.h"
int gPling_materials;
char gPhysics_empty_message;
tPhysicsError_cbfn* gPhysics_error_cb;
static unsigned char* arena;
static unsigned callback_mode;
static tPhysicsError_cbfn** callback_slot;
static char* message_slot;
static void* entries[4];
static void snapshot(unsigned tag,unsigned value){
 unsigned state[3];state[0]=tag;state[1]=value;state[2]=(*callback_slot!=NULL)|((unsigned char)*message_slot<<8);
 fwrite(state,sizeof(state),1,stdout);fwrite(arena,0x4000,1,stdout);
}
static void C2_HOOK_FASTCALL ErrorCallback(tPhysicsError error,const char*message){
 unsigned state[3];state[0]=1;state[1]=(unsigned)error;state[2]=(message==message_slot)|((unsigned char)*message<<8);
 fwrite(state,sizeof(state),1,stdout);fwrite(arena,0x4000,1,stdout);
 if(callback_mode&1)*message_slot=(char)0xe7;
 if(callback_mode&2)*callback_slot=NULL;
 if(callback_mode&4)arena[0x1f00]=(unsigned char)error;
}
static void C2_NORETURN C2_HOOK_CDECL ProbeExit(int code){snapshot(2,(unsigned)code);fflush(stdout);ExitProcess(0);}
static LONG WINAPI ProbeFault(EXCEPTION_POINTERS*e){
 unsigned code=e->ExceptionRecord->ExceptionCode;
 if(code!=0xc0000005u&&code!=0xc00000fdu)return EXCEPTION_CONTINUE_SEARCH;
 snapshot(3,code);fflush(stdout);ExitProcess(0);
}
static void InstallFaultBoundary(void){
 typedef void*(WINAPI*install_fn)(unsigned,LONG(WINAPI*)(EXCEPTION_POINTERS*));
 install_fn fn;FARPROC address=GetProcAddress(GetModuleHandleA("kernel32.dll"),"AddVectoredExceptionHandler");memcpy(&fn,&address,4);
 if(fn)fn(1,ProbeFault);
 SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);
}
#define exit ProbeExit

/* Uniform stack history is seeded below the live frame before the real
   FASTCALL invocation. Each tested root owns exactly four stack arguments. */
#if defined(_MSC_VER)
#pragma warning(disable:4035)
__declspec(naked) static unsigned C2_HOOK_CDECL ProbeCall(void*fn,void*start,void*end,unsigned*args,unsigned seed){
 __asm{
  push ebp
  push ebx
  push esi
  push edi
  mov ebx,esp
  mov eax,[ebx+36]
  lea edi,[ebx-4096]
  mov ecx,1024
  rep stosd
  mov esi,[ebx+32]
  push [esi+12]
  push [esi+8]
  push [esi+4]
  push [esi]
  mov ecx,[ebx+24]
  mov edx,[ebx+28]
  call [ebx+20]
  cmp esp,ebx
  je good_stack
  push 90
  call ExitProcess
 good_stack:
  pop edi
  pop esi
  pop ebx
  pop ebp
  ret
 }
}
#pragma warning(default:4035)
#else
__attribute__((naked)) static unsigned C2_HOOK_CDECL ProbeCall(void*fn __attribute__((unused)),void*start __attribute__((unused)),void*end __attribute__((unused)),unsigned*args __attribute__((unused)),unsigned seed __attribute__((unused))){
 __asm__ __volatile__("push %ebp;push %ebx;push %esi;push %edi;mov %esp,%ebx;mov 36(%ebx),%eax;lea -4096(%ebx),%edi;mov $1024,%ecx;rep stosl;mov 32(%ebx),%esi;push 12(%esi);push 8(%esi);push 4(%esi);push (%esi);mov 24(%ebx),%ecx;mov 28(%ebx),%edx;call *20(%ebx);cmp %ebx,%esp;je 1f;push $90;call _ExitProcess@4;1:pop %edi;pop %esi;pop %ebx;pop %ebp;ret");
}
#endif
'''
DRIVER=r'''
int C2_HOOK_CDECL main(int argc,char**argv){
 FILE*f;unsigned h[11],args[4],result;if(argc!=ARGS)return 2;f=fopen(argv[1],"rb");if(!f)return 3;
 arena=(unsigned char*)VirtualAlloc((void*)0x20000000,0x4000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(!arena)return 4;
 if(fread(h,sizeof(h),1,f)!=1||fread(arena,0x4000,1,f)!=1)return 5;fclose(f);INSTALL
 callback_mode=h[9];*callback_slot=(h[9]&8)?NULL:ErrorCallback;*message_slot=(char)h[10];
 _setmode(_fileno(stdout),_O_BINARY);_controlfp(_PC_53,_MCW_PC);InstallFaultBoundary();
 args[0]=(unsigned)arena+h[3];args[1]=(unsigned)arena+h[4];
 if(h[0]==0){args[2]=h[7];args[3]=(unsigned)arena+h[6];}
 else if(h[0]==2){args[2]=(unsigned)arena+h[6];args[3]=h[5]==0xffffffffu?0:(unsigned)arena+h[5];}
 else{args[2]=(unsigned)arena+h[5];args[3]=(unsigned)arena+h[6];}
 result=ProbeCall(entries[h[0]],arena+h[1],arena+h[2],args,h[8]);snapshot(0,result);return 0;
}
'''

def native_harness():
    body=SOURCE.read_text();s=PREFIX
    for name in ('FinterayScalar','FinterayBoxNaN','FinterayBoxCopyScalar','FinterayBoxStoreScalar','FinterayShapeLoadScalar','FinterayShapePlanes','FinterayShapePlaneCount','LineBoxColl','SphereBoxIntersection','LineBoxCollWithSphere','LineSphereColl','LineSphereCollWithSphere','LinePolyColl','LinePolyCollWithSphere','ShapeRayCast','ShapeRayCastWithSphere'):
        s+=extract(body,name)+'\n'
    s+=extract(VECTOR.read_text(),'BrVector3SetFloat')+'\n'+extract(PHYSICS.read_text(),'PhysicsError')+'\n'
    install='callback_slot=&gPhysics_error_cb;message_slot=&gPhysics_empty_message;'+''.join(f'entries[{i}]=(void*){n};'for i,(n,_,_)in enumerate(ENTRIES))
    return s+DRIVER.replace('ARGS','2').replace('INSTALL',install)

def physical_harness(binary,addresses):
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_MEM,CS_OP_IMM
    nt=struct.unpack_from('<I',binary,60)[0];base=struct.unpack_from('<I',binary,nt+52)[0];tab=nt+24+struct.unpack_from('<H',binary,nt+20)[0]
    segments=[(*struct.unpack_from('<IIII',binary,tab+40*i+8),struct.unpack_from('<I',binary,tab+40*i+36)[0])for i in range(struct.unpack_from('<H',binary,nt+6)[0])]
    def raw(a,size=15,code=True):
        for vs,va,n,off,flags in segments:
            if (not code or flags&0x20) and va<=a-base<va+n:return binary[off+a-base-va:off+a-base-va+size]
        raise ValueError(f'Closure outside mapped {"code"if code else "data"}: {a:x}')
    cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;starts=[addresses[a]for _,a,_ in ENTRIES];pending=list(starts);roots=set(starts);instructions={};refs={};tables={};exit_address=addresses[0x5784e0];error_address=addresses.get(0x4c61c0)
    while pending:
        function=pending.pop();todo=[function];visited=set()
        while todo:
            a=todo.pop()
            if a in visited:continue
            visited.add(a);i=next(cs.disasm(raw(a),a,count=1));instructions[a]=i.bytes.hex()
            for op in i.operands:
                if op.type==CS_OP_MEM and not op.mem.base and op.mem.disp>=base:
                    assert i.disp_size==4;refs[a-base+i.disp_offset]=int(op.mem.disp==addresses[0x5964c0])
                if op.type==CS_OP_IMM and i.mnemonic not in ('call','jmp') and not i.mnemonic.startswith('j') and op.imm>=base:
                    if not any(va<=op.imm-base<va+max(vs,n)for vs,va,n,_,_ in segments):continue
                    assert i.imm_size==4;refs[a-base+i.imm_offset]=0
            if i.mnemonic=='call':
                if i.operands[0].type!=CS_OP_IMM:
                    assert function==error_address,'Only the authentic error callback may be indirect'
                else:
                    target=i.operands[0].imm
                    if target==exit_address:continue
                    raw(target)
                    if target not in roots:roots.add(target);pending.append(target)
            if i.mnemonic=='ret':continue
            if i.mnemonic.startswith('j'):
                op=i.operands[0]
                if op.type==CS_OP_IMM:
                    todo.append(op.imm)
                else:
                    assert i.mnemonic=='jmp' and op.type==CS_OP_MEM and not op.mem.base and op.mem.index and op.mem.scale==4,'Unproven indirect branch'
                    table=op.mem.disp;targets=struct.unpack('<5I',raw(table,20,False));tables[table]=targets
                    for j,t in enumerate(targets):raw(t);todo.append(t);refs[table-base+4*j]=0
                if i.mnemonic=='jmp':continue
            todo.append(a+i.size);assert len(instructions)<6000
    s=PREFIX+'static const unsigned refs[][2]={'+','.join('{'+str(a)+','+str(k)+'}'for a,k in sorted(refs.items()))+'};\n'
    s+='static const unsigned offsets[4]={'+','.join(str(a-base)for a in starts)+'};\n'
    s+=r'''
static int install(const char*path){
 FILE*f;unsigned char*disk,*image;unsigned size,i,site,value;IMAGE_DOS_HEADER*dos;IMAGE_NT_HEADERS32*nt;IMAGE_SECTION_HEADER*sec;
 f=fopen(path,"rb");if(!f)return 91;fseek(f,0,SEEK_END);size=(unsigned)ftell(f);rewind(f);disk=(unsigned char*)malloc(size);if(!disk||fread(disk,size,1,f)!=1)return 92;fclose(f);
 dos=(IMAGE_DOS_HEADER*)disk;nt=(IMAGE_NT_HEADERS32*)(disk+dos->e_lfanew);sec=IMAGE_FIRST_SECTION(nt);image=(unsigned char*)VirtualAlloc(NULL,nt->OptionalHeader.SizeOfImage,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image)return 93;
 memcpy(image,disk,nt->OptionalHeader.SizeOfHeaders);for(i=0;i<nt->FileHeader.NumberOfSections;i++)memcpy(image+sec[i].VirtualAddress,disk+sec[i].PointerToRawData,sec[i].SizeOfRawData);
 for(i=0;i<sizeof(refs)/sizeof(refs[0]);i++){site=(unsigned)image+refs[i][0];memcpy(&value,(void*)site,4);value=refs[i][1]?(unsigned)&gPling_materials:(unsigned)image+value-nt->OptionalHeader.ImageBase;memcpy((void*)site,&value,4);}
 callback_slot=(tPhysicsError_cbfn**)(image+CALLBACK_OFFSET);message_slot=(char*)(image+MESSAGE_OFFSET);
 image[EXIT_OFFSET]=0xe9;value=(unsigned)ProbeExit-((unsigned)image+EXIT_OFFSET+5);memcpy(image+EXIT_OFFSET+1,&value,4);
 for(i=0;i<4;i++)entries[i]=image+offsets[i];free(disk);return 0;
}
'''
    s=s.replace('CALLBACK_OFFSET',str(addresses[0x6940c0]-base)).replace('MESSAGE_OFFSET',str(addresses[0x676920]-base)).replace('EXIT_OFFSET',str(exit_address-base))
    s+=DRIVER.replace('ARGS','3').replace('INSTALL','{int r=install(argv[2]);if(r)return r;}')
    return s,dict(entries=[hex(a)for a in sorted(roots)],instruction_count=len(instructions),closure_sha256=hashlib.sha256(json.dumps(sorted(instructions.items())).encode()).hexdigest(),references=sorted(refs.items()),tables=tables,terminal_exit=hex(exit_address),private_history_bytes=4096)

def scene(mode=0,start=(2,0,0),end=(0,0,0),kind=1,planes=None,count=None,patch=(),**kw):
    a=bytearray(b'\xa5'*0x4000)
    a[0:12]=struct.pack('<3f',*start);a[0x20:0x2c]=struct.pack('<3f',*end)
    a[0x100:0x158]=bytes(0x58);a[0x100:0x104]=words([kind]);a[0x104:0x11c]=struct.pack('<6f',-1,-1,-1,1,1,1)
    planes=planes or [(1,0,0,1),(-1,0,0,1),(0,1,0,1),(0,-1,0,1),(0,0,1,1),(0,0,-1,1)]
    if kind==4:a[0x138:0x14c]=struct.pack('<5f',1,0,0,0,1)
    else:
        a[0x140:0x144]=words([len(planes)if count is None else count]);a[0x14c:0x150]=words([0x20000400])
        for j,p in enumerate(planes):a[0x400+j*16:0x410+j*16]=struct.pack('<4f',*p)
        a[0x3f0:0x400]=words([0x3f012345,0x3f456789,0xbf234567,0x3f800000]);a[0x1000:0x1400]=a[0x400:0x800]
    for off,v in patch:a[off:off+4]=words([v])
    h=[mode,kw.get('start_offset',0),kw.get('end_offset',0x20),kw.get('shape_offset',0x138 if mode==0 else 0x100),kw.get('point',0x80),kw.get('normal',0xa0),kw.get('t',0xc0),kw.get('radius',0),kw.get('seed',fb(4)),kw.get('callback',0),kw.get('message',0)]
    return dict(header=h,data=bytes(a),options=dict(mode=mode,start=start,end=end,kind=kind,planes=planes,count=count,patch=patch,**kw))

def scenarios():
    cases=[]
    for mode in range(4):
        for axis in range(3):
            for sign in (-1,1):
                for start,end in ((0,0),(2,0),(2,3),(0,2),(1,1),(2,1),(2,-2),(1.004,1.004),(1.006,1.006)):
                    a=[0]*3;b=[0]*3;a[axis]=sign*start;b[axis]=sign*end;cases.append(scene(mode,start=a,end=b))
        for a,b in (((2,2,2),(0,0,0)),((2,2,2),(1.004,)*3),((1.004,)*3,(1.004,)*3),((1.006,)*3,(1.006,)*3)):
            cases.append(scene(mode,start=a,end=b))
        for count in (-1,0,1,63,64):
            planes=[(0,1,0,1)]*max(count,1);planes[-1]=(-1,0,0,1);cases.append(scene(mode,planes=planes,count=count))
        for point in (0,4,0x20,0x24,0x400,0x404,0xa0,0xc0):cases.append(scene(mode,point=point))
        for t in (0,0x20,0x80,0xa0,0x400,0x40c):cases.append(scene(mode,t=t))
        if mode:
            for normal in (0,0x20,0x80,0xc0,0x400,0x404,0x140):cases.append(scene(mode,normal=normal))
            cases.append(scene(mode,start=(-2,0,0),normal=0x14c,patch=[(0x400,0x20001000),(0x40c,fb(1e-20))]))
            cases.append(scene(mode,start=(1.004,)*3,end=(1.004,)*3,normal=0x140))
    for mode in (2,3):
        for kind in (0,2,3,4,5,0xffffffff):
            for callback in ((0,1,2,4,7,8)if kind in(5,0xffffffff)else(0,)):
                cases.append(scene(mode,kind=kind,callback=callback,message=0x83))
        if mode==2:
            for kind in (0,1,3,4):cases.append(scene(mode,kind=kind,normal=0xffffffff))
    rng=random.Random(0x461310)
    for j in range(35):
        start=[rng.uniform(-3,3)for _ in range(3)];end=[rng.uniform(-3,3)for _ in range(3)]
        for mode in range(4):cases.append(scene(mode,start=start,end=end))
    # Bounded explicit stack histories expose the original untouched ratio slot.
    for mode in range(4):
        for seed in (fb(-1),fb(4),0x7fc12345):
            cases.append(scene(mode,start=(0,0,0),end=(float('inf'),)*3,planes=[(1,1,-1,1),(1,1,1,-1)],seed=seed))
    specials=(0x80000000,1,0x80000001,0x7f800000,0xff800000,0x7fc12345,0xffc54321,0x7f812345,0xff854321)
    for mode in range(4):
        for offset in (0,0x20,0x400,0x404,0x408,0x40c):
            for value in specials:cases.append(scene(mode,patch=[(offset,value)]))
        for left,right in ((0x7fc12345,0xffc54321),(0x7f812345,0xff854321),(0xffc54321,0x7f812345)):
            cases.append(scene(mode,patch=[(0,left),(0x20,right)]))
            cases.append(scene(mode,patch=[(0x400,left),(0x404,right)]))
    for value in specials:cases.append(scene(0,radius=value))
    for mode in (1,2,3):cases.append(scene(mode,normal=0x14c,expect_fault=True))
    return cases

def assert_oracle_contract(case,result):
    """Reject accidental loader faults or inert boundary fixtures."""
    options=case['options'];tag,value,state=struct.unpack_from('<III',result);tail=struct.unpack_from('<III',result,len(result)-0x400c)
    if options.get('expect_fault'):
        assert (tag,value)==(3,0xc0000005),'Expected only the explicitly owned invalid-pointer fault'
        return
    assert tag!=3,'Unexpected physical oracle fault'
    if options['mode']>=2 and options['kind'] in (5,0xffffffff):
        assert tail[:2]==(2,1),'Authentic PhysicsError must reach the terminal exit boundary'
        if not options.get('callback',0)&8:
            assert (tag,value)==(1,1) and state&1,'Callback must receive the genuine writable empty-message pointer'
        return
    assert tag==0 and len(result)==0x400c,'Normal return must retain complete arena'
    if options['mode']>=2 and options['kind']==2:
        assert value==0 and result[12:]==case['data'],'Wireframe no-hit must leave all caller state untouched'
    if options['count']==64:
        assert value==64
        if options['mode']:
            assert result[12+0xa0:12+0xac]==case['data'][0x3f0:0x3fc],'Plane64 must select the original masked plane[-1] normal'
    if options.get('seed') is not None and options['planes']==[(1,1,-1,1),(1,1,1,-1)]:
        assert value==(0 if options['seed']==fb(-1) else 2),'Untouched ratio slot must expose the declared stack history'

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE');p.add_argument('--output',type=Path,default=ROOT/'build-audit/wall-contact-stage3');p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);p.add_argument('--case-index',type=int,action='append');args=p.parse_args()
    if bool(args.recompiled)!=bool(args.symbols):p.error('Supply both immutable PE/map')
    out=args.output.resolve();out.mkdir(parents=True,exist_ok=True);binary=args.original.read_bytes();assert hashlib.sha256(binary).hexdigest()==HASH
    a={x:x for _,x,_ in ENTRIES};a[0x5964c0]=0x5964c0;a[0x5784e0]=0x5784e0;a[0x6940c0]=0x6940c0;a[0x676920]=0x676920;a[0x4c61c0]=0x4c61c0;native=native_harness();physical,meta=physical_harness(binary,a);(out/'harness.c').write_text(native);(out/'original.c').write_text(physical)
    inc=[ROOT/v for v in ('src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/carma2/platform','src/carma2/s3/include','src/brender/include')];r=ROOT/'build-tools/msvc-modern';ms=sorted((r/'VC/Tools/MSVC').iterdir())[-1];sdk=r/'Windows Kits/10';v=sorted((sdk/'Include').iterdir())[-1].name;env=dict(os.environ);env['PATH']=str(ms/'bin/Hostx64/x86')+os.pathsep+env['PATH'];env['INCLUDE']=os.pathsep.join(map(str,[ms/'include',*[sdk/f'Include/{v}/{part}'for part in ('ucrt','shared','um')]]));env['LIB']=os.pathsep.join(map(str,[ms/'lib/x86',*[sdk/f'Lib/{v}/{part}/x86'for part in ('ucrt','um')]]));m5=ROOT/'build-tools/msvc500sp3';e5=dict(os.environ);e5['PATH']=str(m5/'bin')+os.pathsep+str(m5/'redist')+os.pathsep+e5['PATH'];e5['INCLUDE']=str(m5/'include');e5['LIB']=str(m5/'lib');gcc=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'
    commands={
      'msvc':[str(ms/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS','/DREC2_MATCHING',*['/I'+str(x)for x in inc],str(out/'harness.c'),'/Fo'+str(out/'msvc.obj'),'/Fe'+str(out/'msvc.exe')],
      'msvc5':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(x)for x in inc],str(out/'harness.c'),'/Fo'+str(out/'msvc5.obj'),'/Fe'+str(out/'msvc5.exe')],
      'mingw':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-DREC2_MATCHING',*['-I'+str(x)for x in inc],str(out/'harness.c'),'-o',str(out/'mingw.exe')],
      'original':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-DREC2_MATCHING',*['-I'+str(x)for x in inc],str(out/'original.c'),'-o',str(out/'original.exe')]}
    linked=None
    if args.recompiled:
        rebuilt=args.recompiled.read_bytes();m=json.loads(args.symbols.read_text());assert m['format']==1 and m['original_sha256']==HASH and m['recompiled_sha256']==hashlib.sha256(rebuilt).hexdigest();a={v['orig']:v['recomp']for v in m['symbols']}
        supplement=None
        if 0x676920 not in a:
            map_path=args.recompiled.with_suffix('.map');data=map_path.read_bytes();matches=re.findall(r'\s+[0-9a-fA-F]{4}:[0-9a-fA-F]{8}\s+_gPhysics_empty_message\s+([0-9a-fA-F]{8})\s',data.decode('latin1'))
            assert len(matches)==1,'Unique actual empty-message MAP symbol required';a[0x676920]=int(matches[0],16);supplement=dict(path=str(map_path),sha256=hashlib.sha256(data).hexdigest(),name='gPhysics_empty_message',original='0x676920',recompiled=hex(a[0x676920]))
        s,linked=physical_harness(rebuilt,a);(out/'matching.c').write_text(s);commands['matching']=[*commands['original'][:-3],str(out/'matching.c'),'-o',str(out/'matching.exe')];linked.update(recompiled_sha256=hashlib.sha256(rebuilt).hexdigest(),symbols_sha256=hashlib.sha256(args.symbols.read_bytes()).hexdigest(),map_supplement=supplement)
    for label,cmd in commands.items():
        build=subprocess.run(cmd,cwd=out,env=env if label=='msvc'else e5 if label=='msvc5'else None,text=True,capture_output=True);(out/(label+'-build.txt')).write_text(build.stdout+build.stderr);assert not build.returncode,build.stdout+build.stderr
    cases=scenarios();results=[]
    for i,c in enumerate(cases):
        if args.case_index is not None and i not in args.case_index:continue
        fixture=out/f'case-{i:03d}.bin';fixture.write_bytes(words(c['header'])+c['data']);run=subprocess.run([str(out/'original.exe'),str(fixture),str(args.original.resolve())],capture_output=True);expected=run.stdout;assert run.returncode==0,f'Original case{i} exits {run.returncode:x}';assert_oracle_contract(c,expected)
        for label in ('msvc5','msvc','mingw',*(['matching']if linked else [])):
            run=subprocess.run([str(out/(label+'.exe')),str(fixture),*([str(args.recompiled.resolve())]if label=='matching'else [])],capture_output=True);actual=run.stdout
            if actual!=expected or run.returncode:
                prefix=out/f'failure-{label}-{i}';prefix.with_suffix('.expected.bin').write_bytes(expected);prefix.with_suffix('.actual.bin').write_bytes(actual);prefix.with_suffix('.json').write_text(json.dumps(dict(options=c['options'],first_difference=next((j for j,(x,y)in enumerate(zip(expected,actual))if x!=y),None),exit=run.returncode,expected_size=len(expected),actual_size=len(actual)),indent=2));raise AssertionError(f'{label} shape{i} differs:{prefix}')
            results.append(dict(case_index=i,compiler=label,all_bytes_match=True,output_sha256=hashlib.sha256(actual).hexdigest()))
        if (i+1)%30==0:print(f'Passed {i+1}/{len(cases)}',flush=True)
    report=dict(original_sha256=HASH,scenes=len(set(r['case_index']for r in results)),total_scenarios=len(cases),comparisons=len(results),complete_suite=args.case_index is None,stage=3,full_consumer_complete=False,intended_consumer='0x4228b0',entries=ENTRIES,physical_original=meta,verifier_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),native_harness_sha256=hashlib.sha256(native.encode()).hexdigest(),source_sha256={str(s.relative_to(ROOT)):hashlib.sha256(s.read_bytes()).hexdigest()for s in (SOURCE,SOURCE.with_suffix('.h'),PHYSICS,PHYSICS.with_suffix('.h'),VECTOR)},commands=commands,results=results,limitations=__doc__)
    if linked:report['matching_physical']=linked
    (out/'verification.json').write_text(json.dumps(report,indent=2)+'\n');print(f"PASS: {report['scenes']} scenes/{len(results)} comparisons")

if __name__=='__main__':main()
