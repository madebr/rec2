#!/usr/bin/env python3
"""Verify InitShadow x86/native state and allocation ordering against the original.

Requires Python 3.10+, Unicorn2.1.4 and (for export-map) reccmp0.1.7. verify also
compiles verbatim current production InitShadow/globals under modern x86 MSVC
and MinGW, with repository BRender types and verbatim matrix identity. Allocation,
actor registration/disable and registry lookup are deterministic boundaries.
Native success paths and original/rebuilt x86 allocation faults are compared;
this is not a complete renderer/allocator integration test.
"""
from pathlib import Path
import argparse,hashlib,json,os,re,struct,sys

ORIGINAL_SHA256='9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
parser=argparse.ArgumentParser(description=__doc__,formatter_class=argparse.RawDescriptionHelpFormatter)
commands=parser.add_subparsers(dest='command',required=True)
export=commands.add_parser('export-map',help='Export matched reccmp symbols and image hashes')
export.add_argument('--build-directory',type=Path,required=True)
export.add_argument('--output',type=Path,required=True)
verify=commands.add_parser('verify',help='Execute differential shadow startup cases and native checks')
for option in ('original','recompiled','symbols','output'):
    verify.add_argument('--'+option,type=Path,required=True)
verify.add_argument('--gcc',type=Path,default=Path('build-tools/winlibs-i686/mingw32/bin/gcc.exe'))
verify.add_argument('--msvc-root',type=Path,default=Path('build-tools/msvc-modern'))
verify.add_argument('--native-directory',type=Path,default=Path('build-audit/shadow-startup/native'))
cli=parser.parse_args()
def digest(path):return hashlib.sha256(path.read_bytes()).hexdigest()

if cli.command=='export-map':
    from reccmp.project.detect import argparse_parse_project_target
    from reccmp.compare import Compare
    output=cli.output.resolve()
    os.chdir(cli.build_directory)
    target=argparse_parse_project_target(argparse.Namespace(target='CARMA2_HW'))
    if digest(target.original_path)!=ORIGINAL_SHA256:parser.error('Incorrect original binary hash')
    compare=Compare.from_target(target)
    mapping={'format':1,'original_sha256':ORIGINAL_SHA256,'recompiled_sha256':digest(target.recompiled_path),
             'symbols':[dict(name=e.name,orig=e.orig_addr,recomp=e.recomp_addr) for e in compare.get_all() if e.orig_addr and e.recomp_addr]}
    output.parent.mkdir(parents=True,exist_ok=True)
    output.write_text(json.dumps(mapping,indent=2))
    print(f'Exported {len(mapping["symbols"])} matched symbols to {output}')
    sys.exit(0)

if digest(cli.original)!=ORIGINAL_SHA256:parser.error('Incorrect original binary hash')
mapping=json.loads(cli.symbols.read_text())
if mapping.get('format')!=1 or mapping.get('original_sha256')!=ORIGINAL_SHA256:parser.error('Invalid symbol map')
if mapping.get('recompiled_sha256')!=digest(cli.recompiled):parser.error('Stale symbol map: export a fresh map for this reconstructed image')
symbols={entry['name']:entry for entry in mapping['symbols']}
try:
    from unicorn import Uc,UcError,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE,UC_HOOK_MEM_INVALID,UC_MEM_READ_UNMAPPED
    from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_EAX,UC_X86_REG_ECX,UC_X86_REG_EDX
except ImportError:parser.error('Install the verifier dependency: python -m pip install unicorn==2.1.4')

STOP,STACK,HEAP=0x1000000,0x1010000,0x1040000

class Image:
    def __init__(self,side):
        self.side=side
        binary=(cli.original if side=='orig' else cli.recompiled).read_bytes()
        pe=struct.unpack_from('<I',binary,0x3c)[0]
        count=struct.unpack_from('<H',binary,pe+6)[0]
        table=pe+24+struct.unpack_from('<H',binary,pe+20)[0]
        base=struct.unpack_from('<I',binary,pe+52)[0]
        size=struct.unpack_from('<I',binary,pe+80)[0]
        self.u=Uc(UC_ARCH_X86,UC_MODE_32)
        self.u.mem_map(base,(size+4095)&~4095)
        self.u.mem_map(STOP,0x200000)
        for i in range(count):
            virtual,va,raw_size,raw=struct.unpack_from('<IIII',binary,table+40*i+8)
            if raw_size:self.u.mem_write(base+va,binary[raw:raw+raw_size])
    def address(self,name):return symbols[name][self.side]
    def write(self,address,value):self.u.mem_write(address,struct.pack('<I',value&0xffffffff))
    def read(self,address):return struct.unpack('<I',self.u.mem_read(address,4))[0]
    def global_write(self,name,value):self.write(self.address(name),value)
    def global_read(self,name):return self.read(self.address(name))
    def string(self,address):
        result=bytearray()
        while True:
            ch=self.u.mem_read(address+len(result),1)[0]
            if ch==0:return bytes(result)
            result.append(ch)
            if len(result)>4096:raise AssertionError('Unterminated fixture string')

NAMES=['gFancy_shadow','gShadow_material','gShadow_model','gShadow_actor']
VECTORS=['gShadow_light_ray','gShadow_light_z','gShadow_light_x']
fixtures=[dict(name='initialize'),dict(name='missing_shadow_material',missing_material=True),dict(name='repeat',repeat=True)]
fixtures+=[dict(name=f'null_actor_{i}',null_actor=i,expected_fault=True) for i in range(9)]
fixtures.append(dict(name='null_model',null_model=True,expected_fault=True))

def execute(side,fixture):
    image=Image(side);u=image.u
    cursor=[HEAP];actors=[0];objects=[];calls=[];fault=[]
    u.mem_write(image.address('gShadow_clip_planes'),b'\xa5'*64)
    for name in NAMES:image.global_write(name,0xdeadbeef)
    for name in VECTORS:u.mem_write(image.address(name),b'\xa5'*12)
    image.global_write('gUniverse_actor',HEAP+0x18000)
    def allocate(size):
        address=cursor[0];cursor[0]+=(size+15)&~15
        u.mem_write(address,b'\xa5'*size);objects.append([address,size]);return address
    names=['BrActorAllocate','BrActorAdd','BrClipPlaneDisable','BrMaterialFind','BrModelAllocate']
    hooks={image.address(name):name for name in names}
    def invalid(uc,access,address,size,value,data):
        fault.append(dict(access=access,address=address,size=size));return False
    def hook(uc,at,size,data):
        if at==STOP:uc.emu_stop();return
        if at not in hooks:return
        name=hooks[at];sp=uc.reg_read(UC_X86_REG_ESP)
        args=[image.read(sp+4*i) for i in (1,2,3)]
        value=0
        if name=='BrActorAllocate':
            value=0 if fixture.get('null_actor')==actors[0] else allocate(100)
            actors[0]+=1;calls.append([name,args[0],args[1],value])
        elif name=='BrModelAllocate':
            if image.string(args[0])!=b'' or args[1:3]!=[0,0]:raise AssertionError('Model arguments differ')
            value=0 if fixture.get('null_model') else allocate(92)
            calls.append([name,'',args[1],args[2],value])
        elif name=='BrMaterialFind':
            if image.string(args[0])!=b'SHADOW.MAT':raise AssertionError('Shadow material name differs')
            value=0 if fixture.get('missing_material') else HEAP+0x19000
            calls.append([name,'SHADOW.MAT',value])
        elif name=='BrActorAdd':calls.append([name,args[0],args[1]]);value=args[1]
        else:calls.append([name,args[0]])
        uc.reg_write(UC_X86_REG_EAX,value)
        uc.reg_write(UC_X86_REG_EIP,image.read(sp));uc.reg_write(UC_X86_REG_ESP,sp+4)
    u.hook_add(UC_HOOK_CODE,hook);u.hook_add(UC_HOOK_MEM_INVALID,invalid)
    def invoke():
        image.write(STACK-4,STOP);u.reg_write(UC_X86_REG_ESP,STACK-4)
        try:u.emu_start(image.address('InitShadow'),STOP,count=50000)
        except UcError:
            if not fault:raise
        if not fault and u.reg_read(UC_X86_REG_EIP)!=STOP:raise AssertionError('Instruction budget exhausted')
    invoke()
    if fixture.get('repeat'):invoke()
    state=dict(calls=calls,globals=[image.global_read(n) for n in NAMES],
               clips=bytes(u.mem_read(image.address('gShadow_clip_planes'),64)).hex(),
               vectors=[bytes(u.mem_read(image.address(n),12)).hex() for n in VECTORS],
               objects=[[a,n,bytes(u.mem_read(a,n)).hex()] for a,n in objects])
    for i in range(8):
        if bytes(u.mem_read(image.address('gShadow_clip_planes')+i*8+4,4))!=b'\xa5'*4:
            raise AssertionError('InitShadow changed clip lengths')
    return dict(state=state,fault=fault)

records=[]
for fixture in fixtures:
    original,actual=execute('orig',fixture),execute('recomp',fixture)
    if original!=actual:raise AssertionError((fixture,original,actual))
    if bool(actual['fault'])!=bool(fixture.get('expected_fault')):raise AssertionError((fixture,'Unexpected fault'))
    records.append(dict(fixture=fixture['name'],passed=True,**actual))

# Extract the current function/global definitions verbatim, avoiding unrelated
# unresolved render functions. No alternative algorithm or helper stub is used.
import os,subprocess
root=Path(__file__).resolve().parents[1]
out=cli.native_directory.resolve();out.mkdir(parents=True,exist_ok=True)
harness=out/'harness.c'
source=(root/'src/carma2/newcommon/16-graphics1.c').read_text()
shadow_body=re.search(r'void C2_HOOK_FASTCALL InitShadow\(void\) \{.*?\n\}',source,re.S).group(0)
shadow_source=out/'shadow_under_test.c'
shadow_source.write_text(source[:source.index('// MungeClipPlane')]+shadow_body+'\n')
identity=(root/'src/brender/core/math/matrix34.c').read_text()
identity_body=re.search(r'void C2_HOOK_CDECL BrMatrix34Identity\(br_matrix34\* mat\) \{.*?\n\}',identity,re.S).group(0)
identity_source=out/'identity_under_test.c'
identity_source.write_text(identity[:identity.index('// FUNCTION:')]+identity_body+'\n')
driver=r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "16-graphics1.h"
#include "globvars.h"
typedef char correct_layout[sizeof(void*)==4&&sizeof(br_actor)==100&&sizeof(br_model)==92&&sizeof(tClip_details)==8?1:-1];
br_actor* gUniverse_actor;
static unsigned arena[32768],used,count,actors;
static void* pointers[24];static unsigned sizes[24];
static int missing;
static char calls[4096];static unsigned call_length;
static unsigned norm(unsigned v){unsigned b=(unsigned)(uintptr_t)arena;return v>=b&&v<b+sizeof(arena)?v-b+0x1040000:v;}
static void* allocate(unsigned size){void*p=(char*)arena+used;memset(p,0xa5,size);pointers[count]=p;sizes[count++]=size;used+=(size+15)&~15u;return p;}
static void event(const char* name,unsigned a,unsigned b,unsigned c,int n){
    int k=sprintf(calls+call_length,"%s[\"%s\",%u",call_length?",":"",name,a);
    call_length+=(unsigned)k;
    if(n>=2)call_length+=(unsigned)sprintf(calls+call_length,",%u",b);
    if(n>=3)call_length+=(unsigned)sprintf(calls+call_length,",%u",c);
    call_length+=(unsigned)sprintf(calls+call_length,"]");
}
br_actor* C2_HOOK_CDECL BrActorAllocate(br_uint_8 type,void* data){br_actor*a=allocate(sizeof(br_actor));if(data)abort();actors++;event("BrActorAllocate",type,0,norm((unsigned)(uintptr_t)a),3);return a;}
br_actor* C2_HOOK_CDECL BrActorAdd(br_actor* parent,br_actor* actor){event("BrActorAdd",norm((unsigned)(uintptr_t)parent),norm((unsigned)(uintptr_t)actor),0,2);return actor;}
void C2_HOOK_CDECL BrClipPlaneDisable(br_actor* actor){event("BrClipPlaneDisable",norm((unsigned)(uintptr_t)actor),0,0,1);}
br_material* C2_HOOK_CDECL BrMaterialFind(const char* name){br_material*m=missing?NULL:(br_material*)((char*)arena+0x19000);if(strcmp(name,"SHADOW.MAT"))abort();call_length+=(unsigned)sprintf(calls+call_length,"%s[\"BrMaterialFind\",\"SHADOW.MAT\",%u]",call_length?",":"",norm((unsigned)(uintptr_t)m));return m;}
br_model* C2_HOOK_CDECL BrModelAllocate(const char* name,int nv,int nf){br_model*m=allocate(sizeof(br_model));if(*name||nv||nf)abort();call_length+=(unsigned)sprintf(calls+call_length,"%s[\"BrModelAllocate\",\"\",0,0,%u]",call_length?",":"",norm((unsigned)(uintptr_t)m));return m;}
'''
driver+=r'''
static void dump(const void* p,unsigned n){unsigned i;for(i=0;i<n;i++)printf("%02x",((const unsigned char*)p)[i]);}
int main(int argc,char**argv){unsigned i,j;int repeat;
    if(argc!=3)return 1;
    missing=atoi(argv[1]);repeat=atoi(argv[2]);
    gUniverse_actor=(br_actor*)((char*)arena+0x18000);
    memset(gShadow_clip_planes,0xa5,sizeof(gShadow_clip_planes));
    gFancy_shadow=(int)0xdeadbeef;gShadow_material=(br_material*)0xdeadbeef;gShadow_actor=(br_actor*)0xdeadbeef;gShadow_model=(br_model*)0xdeadbeef;
    memset(&gShadow_light_ray,0xa5,12);memset(&gShadow_light_x,0xa5,12);memset(&gShadow_light_z,0xa5,12);
    InitShadow();if(repeat)InitShadow();
    printf("{\"calls\":[%s],\"globals\":[%u,%u,%u,%u],\"clips\":\"",calls,(unsigned)gFancy_shadow,norm((unsigned)(uintptr_t)gShadow_material),norm((unsigned)(uintptr_t)gShadow_model),norm((unsigned)(uintptr_t)gShadow_actor));
    for(i=0;i<8;i++)gShadow_clip_planes[i].clip=(br_actor*)(uintptr_t)norm((unsigned)(uintptr_t)gShadow_clip_planes[i].clip);
    dump(gShadow_clip_planes,sizeof(gShadow_clip_planes));printf("\",\"vectors\":[\"");dump(&gShadow_light_ray,12);printf("\",\"");dump(&gShadow_light_z,12);printf("\",\"");dump(&gShadow_light_x,12);printf("\"],\"objects\":[");
    for(i=0;i<count;i++){
        if(sizes[i]==100){unsigned v;memcpy(&v,(char*)pointers[i]+0x18,4);v=norm(v);memcpy((char*)pointers[i]+0x18,&v,4);}
        printf("%s[%u,%u,\"",i?",":"",norm((unsigned)(uintptr_t)pointers[i]),sizes[i]);dump(pointers[i],sizes[i]);printf("\"]");
    }
    printf("]}\n");(void)j;return 0;
}
'''
harness.write_text(driver)
includes=[root/p for p in ['src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/carma2/platform','src/carma2/s3/include','src/brender/include','src/brender']]
msvc_root=cli.msvc_root.resolve();msvc=sorted((msvc_root/'VC/Tools/MSVC').iterdir())[-1]
sdk=msvc_root/'Windows Kits/10';sdk_version=sorted(p.name for p in (sdk/'Include').iterdir())[-1]
env=dict(os.environ)
env['PATH']=str(msvc/'bin/Hostx64/x86')+os.pathsep+env['PATH']
env['INCLUDE']=os.pathsep.join(map(str,[msvc/'include',*[sdk/f'Include/{sdk_version}/{p}' for p in ['ucrt','shared','um']]]))
env['LIB']=os.pathsep.join(map(str,[msvc/'lib/x86',*[sdk/f'Lib/{sdk_version}/{p}/x86' for p in ['ucrt','um']]]))
commands={
    'msvc':[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(p) for p in includes],'/I'+str(root/'src/brender/core/math'),str(harness),str(shadow_source),str(identity_source),'/Fo'+str(out)+os.sep,'/Fe'+str(out/'msvc.exe')],
    'mingw':[str(cli.gcc.resolve()),'-std=gnu89','-O2','-Wall','-Wextra','-Werror',*['-I'+str(p) for p in includes],'-I'+str(root/'src/brender/core/math'),str(harness),str(shadow_source),str(identity_source),'-lm','-o',str(out/'mingw.exe')]
}
native=[]
for compiler,command in commands.items():
    result=subprocess.run(command,cwd=out,env=env if compiler=='msvc' else None,text=True,capture_output=True)
    (out/(compiler+'-build.log')).write_text(result.stdout+result.stderr)
    if result.returncode:raise AssertionError(result.stdout+result.stderr)
    for fixture in fixtures[:3]:
        result=subprocess.run([str(out/(compiler+'.exe')),str(int(fixture.get('missing_material',False))),str(int(fixture.get('repeat',False)))],text=True,capture_output=True,check=True)
        actual=json.loads(result.stdout);expected=execute('orig',fixture)['state']
        if actual!=expected:raise AssertionError((compiler,fixture,{k:(expected[k],actual[k]) for k in expected if expected[k]!=actual[k]}))
        native.append(dict(compiler=compiler,fixture=fixture['name'],passed=True))
cli.output.parent.mkdir(parents=True,exist_ok=True)
cli.output.write_text(json.dumps(dict(original_sha256=ORIGINAL_SHA256,recompiled_sha256=mapping['recompiled_sha256'],
                                   native_shadow_body_sha256=hashlib.sha256(shadow_body.encode()).hexdigest(),
                                   native_identity_body_sha256=hashlib.sha256(identity_body.encode()).hexdigest(),
                                   x86_scenarios=records,native_scenarios=native),indent=2))
print(f'PASS: {len(records)} original/recompiled x86 scenarios and {len(native)} modern/MinGW native scenarios agree.')
