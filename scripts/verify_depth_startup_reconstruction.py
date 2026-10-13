#!/usr/bin/env python3
"""Verify depth startup, table row flipping and pixelmap loading against original x86.

Requires Unicorn2.1.4; export-map additionally needs reccmp0.1.7. verify compiles
verbatim current production bodies and repository types under scoped modern
x86 MSVC and i686 MinGW. It compares all state/data and dependency ordering.
Only file/pixelmap allocation, BRender registry/registration, diagnostics and
smoke-shade dispatch use boundary fixtures; this is not a rendering/codec test.
"""
from pathlib import Path
import argparse,hashlib,json,os,re,struct,sys

ORIGINAL_SHA256='9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
parser=argparse.ArgumentParser(description=__doc__,formatter_class=argparse.RawDescriptionHelpFormatter)
commands=parser.add_subparsers(dest='command',required=True)
export=commands.add_parser('export-map',help='Export matched reccmp symbols and image hashes')
export.add_argument('--build-directory',type=Path,required=True)
export.add_argument('--output',type=Path,required=True)
verify=commands.add_parser('verify',help='Execute differential depth startup cases and native checks')
for option in ('original','recompiled','symbols','output'):
    verify.add_argument('--'+option,type=Path,required=True)
verify.add_argument('--gcc',type=Path,default=Path('build-tools/winlibs-i686/mingw32/bin/gcc.exe'))
verify.add_argument('--msvc-root',type=Path,default=Path('build-tools/msvc-modern'))
verify.add_argument('--native-directory',type=Path,default=Path('build-audit/depth-startup/native'))
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

TABLES=['gDepth_shade_table','gFog_shade_table','gAcid_shade_table','gWater_shade_table']
POWERS=[name+'_power' for name in TABLES]
GLOBALS=TABLES+POWERS+['gHorizon_material','gForward_sky_model','gForward_sky_actor']
fixtures=[]
for row in (-32768,-32767,-9,-5,-4,-3,-1,0,1,2,3,4,5,7,32764,32765,32767):
    fixtures.append(dict(name=f'wrapper_row_{row}',mode=0,width=0,height=0,row=row))
fixtures.append(dict(name='wrapper_null',mode=0,width=0,height=0,row=0,null_load=0))
for width,heights,row in [(0,(0,1,2,3,32768,65535),4),(1,(0,1,2,3,7,16,32768,65535),4),(7,(0,1,2,3,5,16),12)]:
    for height in heights:fixtures.append(dict(name=f'flip_{width}_{height}',mode=1,width=width,height=height,row=row))
for row in (-12,-9,-4,-1,0,7):fixtures.append(dict(name=f'flip_stride_{row}',mode=1,width=3,height=5,row=row))
fixtures.append(dict(name='flip_null',mode=1,width=3,height=3,row=4,null_load=0,expected_fatal=True))
for screen in (3,7,18):
    for fog in (0,1,-1):fixtures.append(dict(name=f'init_type_{screen}_fog_{fog}',mode=2,screen=screen,fog=fog))
fixtures.append(dict(name='init_repeat',mode=2,screen=3,fog=0,repeat=True))
for index in range(4):fixtures.append(dict(name=f'init_null_load_{index}',mode=2,null_load=index,expected_fatal=True))
fixtures.append(dict(name='init_null_material',mode=2,null_material=True,expected_fatal=True))
for kind in ('blend','model','actor'):
    fixtures.append(dict(name='init_null_'+kind,mode=2,screen=3,fog=0,null_object=kind,expected_fault=True))

def rounded_row(value):
    return struct.unpack('<h',struct.pack('<H',(value+3)&0xfffc))[0]

class Fixture:
    def __init__(self,side,case):
        self.image=Image(side);self.u=self.image.u;self.case=case
        self.calls=[];self.objects=[];self.cursor=HEAP;self.loads=0;self.fatal=None;self.fault=[]
        for i,name in enumerate(GLOBALS):self.image.global_write(name,0xdead0000+i)
        self.screen=HEAP+0x1a0000;self.material=self.screen+0x100
        self.u.mem_write(self.screen,b'\xa5'*68)
        self.u.mem_write(self.screen+0x2c,bytes([case.get('screen',3)]))
        self.u.mem_write(self.material,b'\xa5'*156)
        self.image.write(self.material+32,0x10203040)
        self.image.global_write('gScreen',self.screen)
        self.image.global_write('gNo_fog',case.get('fog',0))
        self.image.global_write('gUniverse_actor',self.screen+0x200)
        self.u.mem_write(self.image.address('gApplication_path'),b'C:/GAME/DATA/\0')
        self.filename=self.screen+0x300
        self.u.mem_write(self.filename,b'CUSTOM.TAB\0')
        hooks=['PathCat','BrPixelmapLoad','GenerateSmokeShades','BrMaterialFind','FatalError',
               'BrPixelmapAllocate','BrTableAdd','BrModelAllocate','BrModelAdd','BrActorAllocate','BrActorAdd']
        self.hooks={self.image.address(name):name for name in hooks}
        self.u.hook_add(UC_HOOK_CODE,self.hook);self.u.hook_add(UC_HOOK_MEM_INVALID,self.invalid)
    def allocate(self,size):
        address=self.cursor;self.cursor+=(size+15)&~15
        if self.cursor>=HEAP+0x190000:raise AssertionError('Fixture allocation overflow')
        self.u.mem_write(address,bytes((i*17+91)&255 for i in range(size)))
        self.objects.append([address,size]);return address
    def pixelmap(self,width,height,row):
        address=self.allocate(68)
        effective=rounded_row(row)
        before=max(0,-effective*(height-1)) if height else 0
        after=max(0,effective*(height-1)) if height else 0
        size=before+after+max(width,1)+32
        pixels=self.allocate(size)
        self.image.write(address+8,pixels+16+before)
        self.u.mem_write(address+0x28,struct.pack('<h',row))
        self.u.mem_write(address+0x34,struct.pack('<HH',width,height))
        self.u.mem_write(address+0x38,struct.pack('<hh',-7,9))
        return address
    def invalid(self,uc,access,address,size,value,data):
        self.fault.append(dict(access=access,address=address,size=size));return False
    def hook(self,uc,address,size,data):
        if address==STOP:uc.emu_stop();return
        name=self.hooks.get(address)
        if name is None:return
        image=self.image;sp=uc.reg_read(UC_X86_REG_ESP)
        def arg(i):return image.read(sp+4*i)
        ecx,edx=uc.reg_read(UC_X86_REG_ECX),uc.reg_read(UC_X86_REG_EDX)
        value=0;cleanup=0
        if name=='PathCat':
            first,second=image.string(edx),image.string(arg(1))
            result=first+(b'' if first.endswith(b'/') else b'/')+second
            self.u.mem_write(ecx,result+b'\0');cleanup=4
            self.calls.append([name,first.decode(),second.decode()])
        elif name=='BrPixelmapLoad':
            path=image.string(arg(1)).decode()
            if self.case.get('null_load')==self.loads:value=0
            else:
                dims=(self.case.get('width',3),self.case.get('height',5),self.case.get('row',7))
                if self.case['mode']==2:dims=[(3,3,7),(4,2,9),(1,5,-7),(0,0,0)][self.loads%4]
                value=self.pixelmap(*dims)
            self.calls.append([name,path,value]);self.loads+=1
        elif name=='GenerateSmokeShades':self.calls.append([name])
        elif name=='BrMaterialFind':
            text=image.string(arg(1)).decode()
            if text!='HORIZON.MAT':raise AssertionError('Unexpected material')
            value=0 if self.case.get('null_material') else self.material
            self.calls.append([name,text,value])
        elif name=='FatalError':
            self.fatal=arg(1);self.calls.append([name,self.fatal]);uc.emu_stop();return
        elif name=='BrPixelmapAllocate':
            parameters=[arg(i) for i in range(1,6)]
            if parameters!=[3,256,256,0,0]:raise AssertionError(('Blend allocation',parameters))
            if self.case.get('null_object')!='blend':value=self.pixelmap(256,256,256)
            self.calls.append([name,*parameters,value])
        elif name=='BrModelAllocate':
            if [arg(1),arg(2),arg(3)]!=[0,88,126]:raise AssertionError('Horizon model arguments differ')
            if self.case.get('null_object')!='model':
                value=self.allocate(92)
                self.u.mem_write(value+32,struct.pack('<H',0x1234))
                image.write(value+8,self.allocate(88*40));image.write(value+12,self.allocate(126*40))
                self.u.mem_write(value+16,struct.pack('<HH',88,126))
            self.calls.append([name,0,88,126,value])
        elif name=='BrActorAllocate':
            if [arg(1),arg(2)]!=[1,0]:raise AssertionError('Horizon actor arguments differ')
            if self.case.get('null_object')!='actor':value=self.allocate(100)
            self.calls.append([name,1,0,value])
        elif name in ('BrTableAdd','BrModelAdd'):value=arg(1);self.calls.append([name,value])
        elif name=='BrActorAdd':value=arg(2);self.calls.append([name,arg(1),arg(2)])
        else:raise AssertionError(name)
        uc.reg_write(UC_X86_REG_EAX,value);uc.reg_write(UC_X86_REG_EIP,image.read(sp))
        uc.reg_write(UC_X86_REG_ESP,sp+4+cleanup)
    def run(self):
        mode=self.case['mode'];name=['DRPixelmapLoad','LoadDepthTable','InitDepthEffects'][mode]
        def invoke():
            image=self.image;image.write(STACK-4,STOP)
            self.u.reg_write(UC_X86_REG_ESP,STACK-4)
            if mode<2:self.u.reg_write(UC_X86_REG_ECX,self.filename)
            if mode==1:
                self.u.reg_write(UC_X86_REG_EDX,image.address(TABLES[0]));image.write(STACK,image.address(POWERS[0]))
            try:self.u.emu_start(image.address(name),STOP,count=5000000)
            except UcError:
                if not self.fault:raise
            if not self.fault and self.fatal is None and self.u.reg_read(UC_X86_REG_EIP)!=STOP:
                raise AssertionError('Instruction budget exhausted')
            return self.u.reg_read(UC_X86_REG_EAX) if mode==0 else None
        value=invoke()
        if self.case.get('repeat'):invoke()
        return dict(calls=self.calls,globals=[self.image.global_read(n) for n in GLOBALS],
                    objects=[[a,n,bytes(self.u.mem_read(a,n)).hex()] for a,n in self.objects],
                    material=bytes(self.u.mem_read(self.material,156)).hex(),value=value,
                    fatal=self.fatal,fault=bool(self.fault),fault_details=self.fault)

records=[]
for case in fixtures:
    original,actual=Fixture('orig',case).run(),Fixture('recomp',case).run()
    if original!=actual:raise AssertionError((case,{k:(original[k],actual[k]) for k in original if original[k]!=actual[k]}))
    if bool(actual['fatal'])!=bool(case.get('expected_fatal')) or actual['fault']!=bool(case.get('expected_fault')):
        raise AssertionError((case,'Unexpected completion state'))
    records.append(dict(case=case,passed=True,result=actual))

# Compile verbatim current production bodies/global definitions, excluding
# unrelated functions that have independent external dependencies.
import os,subprocess
root=Path(__file__).resolve().parents[1]
out=cli.native_directory.resolve();out.mkdir(parents=True,exist_ok=True)
source=(root/'src/carma2/newcommon/33-depth.c').read_text()
def function(text,name):
    match=re.search(r'(?m)^(?:static )?[^\n]*\b'+re.escape(name)+r'\([^\n]*\) \{.*?\n\}',text,re.S)
    if not match:raise AssertionError(('Missing production function',name))
    return match.group(0)
bodies=[function(source,n) for n in ['Log2','CreateHorizonModel','LoadDepthTable','InitDepthEffects']]
depth_source=out/'depth_under_test.c'
depth_source.write_text(source[:source.index('static void C2_HOOK_FASTCALL FrobFog')]+ '\n\n'.join(bodies)+'\n')
wrapper_body=function((root/'src/carma2/newcommon/41-utility.c').read_text(),'DRPixelmapLoad')
wrapper_source=out/'wrapper_under_test.c'
wrapper_source.write_text('#include "41-utility.h"\n#include "brender/brender.h"\n'+wrapper_body+'\n')
harness=out/'harness.c'
driver=r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <setjmp.h>
#include "33-depth.h"
#include "41-utility.h"
#include "52-errors.h"
#include "32-spark.h"
#include "globvars.h"
typedef char correct_layout[sizeof(void*)==4&&sizeof(br_actor)==100&&sizeof(br_model)==92&&sizeof(br_pixelmap)==68&&sizeof(br_material)==156&&sizeof(br_vertex)==40&&sizeof(br_face)==40?1:-1];
br_pixelmap* gScreen;int gNo_fog;br_actor* gUniverse_actor;tPath_name gApplication_path="C:/GAME/DATA/";
static unsigned arena[458752],used,count,loads;
static void* pointers[40];static unsigned sizes[40];
static int mode,width,height,row,screen_type,fog,null_load,null_material,repeat,fatal;
static br_material* material;
static jmp_buf failure;
static char calls[8192];static unsigned call_length;
static unsigned norm(unsigned v){unsigned b=(unsigned)(uintptr_t)arena;return v>=b&&v<b+sizeof(arena)?v-b+0x1040000:v;}
static void* allocate(unsigned size){unsigned i;void*p=(char*)arena+used;if(count>=40||used+size>=0x190000)abort();for(i=0;i<size;i++)((unsigned char*)p)[i]=(unsigned char)(i*17+91);pointers[count]=p;sizes[count++]=size;used+=(size+15)&~15u;return p;}
static void event(const char*name,unsigned a,unsigned b,unsigned c,int n){call_length+=(unsigned)sprintf(calls+call_length,"%s[\"%s\"",call_length?",":"",name);if(n>0)call_length+=(unsigned)sprintf(calls+call_length,",%u",a);if(n>1)call_length+=(unsigned)sprintf(calls+call_length,",%u",b);if(n>2)call_length+=(unsigned)sprintf(calls+call_length,",%u",c);call_length+=(unsigned)sprintf(calls+call_length,"]");}
static br_pixelmap* pixelmap(int w,int h,int r){
    br_pixelmap* pm=allocate(sizeof(br_pixelmap));int effective=(short)((r+3)&~3);unsigned before=0,after=0,size;void*pixels;
    if(h){if(effective<0)before=(unsigned)(-effective*(h-1));else after=(unsigned)(effective*(h-1));}
    size=before+after+(w?w:1)+32;pixels=allocate(size);pm->pixels=(char*)pixels+16+before;
    pm->row_bytes=(short)r;pm->width=(br_uint_16)w;pm->height=(br_uint_16)h;pm->origin_x=-7;pm->origin_y=9;return pm;
}
void C2_HOOK_FASTCALL PathCat(char*dest,const char*first,const char*second){char f[256],s[256];strcpy(f,first);strcpy(s,second);call_length+=(unsigned)sprintf(calls+call_length,"%s[\"PathCat\",\"%s\",\"%s\"]",call_length?",":"",f,s);strcpy(dest,f);if(f[strlen(f)-1]!='/')strcat(dest,"/");strcat(dest,s);}
br_pixelmap* C2_HOOK_CDECL BrPixelmapLoad(const char*path){br_pixelmap*pm=NULL;static int dims[4][3]={{3,3,7},{4,2,9},{1,5,-7},{0,0,0}};if((int)loads!=null_load){if(mode==2)pm=pixelmap(dims[loads%4][0],dims[loads%4][1],dims[loads%4][2]);else pm=pixelmap(width,height,row);}call_length+=(unsigned)sprintf(calls+call_length,"%s[\"BrPixelmapLoad\",\"%s\",%u]",call_length?",":"",path,norm((unsigned)(uintptr_t)pm));loads++;return pm;}
void C2_HOOK_FASTCALL GenerateSmokeShades(void){event("GenerateSmokeShades",0,0,0,0);}
br_material* C2_HOOK_CDECL BrMaterialFind(const char*name){br_material*m=null_material?NULL:material;if(strcmp(name,"HORIZON.MAT"))abort();call_length+=(unsigned)sprintf(calls+call_length,"%s[\"BrMaterialFind\",\"HORIZON.MAT\",%u]",call_length?",":"",norm((unsigned)(uintptr_t)m));return m;}
void C2_HOOK_CDECL FatalError(int index,...){fatal=index;event("FatalError",(unsigned)index,0,0,1);longjmp(failure,1);}
br_pixelmap* C2_HOOK_CDECL BrPixelmapAllocate(br_uint_8 type,br_int_32 w,br_int_32 h,void*pixels,int flags){br_pixelmap*pm;if(type!=3||w!=256||h!=256||pixels||flags)abort();pm=pixelmap(w,h,256);call_length+=(unsigned)sprintf(calls+call_length,"%s[\"BrPixelmapAllocate\",3,256,256,0,0,%u]",call_length?",":"",norm((unsigned)(uintptr_t)pm));return pm;}
br_pixelmap* C2_HOOK_CDECL BrTableAdd(br_pixelmap*pm){event("BrTableAdd",norm((unsigned)(uintptr_t)pm),0,0,1);return pm;}
br_model* C2_HOOK_CDECL BrModelAllocate(const char*name,int nv,int nf){br_model*m;if(name||nv!=88||nf!=126)abort();m=allocate(92);m->flags=0x1234;m->vertices=allocate(nv*40);m->faces=allocate(nf*40);m->nvertices=(br_uint_16)nv;m->nfaces=(br_uint_16)nf;call_length+=(unsigned)sprintf(calls+call_length,"%s[\"BrModelAllocate\",0,88,126,%u]",call_length?",":"",norm((unsigned)(uintptr_t)m));return m;}
br_model* C2_HOOK_CDECL BrModelAdd(br_model*m){event("BrModelAdd",norm((unsigned)(uintptr_t)m),0,0,1);return m;}
br_actor* C2_HOOK_CDECL BrActorAllocate(br_uint_8 type,void*data){br_actor*a;if(type!=1||data)abort();a=allocate(100);event("BrActorAllocate",1,0,norm((unsigned)(uintptr_t)a),3);return a;}
br_actor* C2_HOOK_CDECL BrActorAdd(br_actor*parent,br_actor*actor){event("BrActorAdd",norm((unsigned)(uintptr_t)parent),norm((unsigned)(uintptr_t)actor),0,2);return actor;}
static void dump(const void*p,unsigned n){unsigned i;for(i=0;i<n;i++)printf("%02x",((const unsigned char*)p)[i]);}
static void normalize(void*p,unsigned offset){unsigned v;memcpy(&v,(char*)p+offset,4);v=norm(v);memcpy((char*)p+offset,&v,4);}
int main(int argc,char**argv){unsigned i;volatile unsigned value=0;unsigned globals[11];char filename[]="CUSTOM.TAB";
    if(argc!=10)return 1;
    mode=atoi(argv[1]);width=atoi(argv[2]);height=atoi(argv[3]);row=atoi(argv[4]);screen_type=atoi(argv[5]);fog=atoi(argv[6]);null_load=atoi(argv[7]);null_material=atoi(argv[8]);repeat=atoi(argv[9]);
    gScreen=(br_pixelmap*)((char*)arena+0x1a0000);material=(br_material*)((char*)arena+0x1a0100);gUniverse_actor=(br_actor*)((char*)arena+0x1a0200);gNo_fog=fog;
    memset(gScreen,0xa5,68);gScreen->type=(br_uint_8)screen_type;memset(material,0xa5,156);material->flags=0x10203040;
    gDepth_shade_table=(br_pixelmap*)0xdead0000;gFog_shade_table=(br_pixelmap*)0xdead0001;gAcid_shade_table=(br_pixelmap*)0xdead0002;gWater_shade_table=(br_pixelmap*)0xdead0003;
    gDepth_shade_table_power=(int)0xdead0004;gFog_shade_table_power=(int)0xdead0005;gAcid_shade_table_power=(int)0xdead0006;gWater_shade_table_power=(int)0xdead0007;
    gHorizon_material=(br_material*)0xdead0008;gForward_sky_model=(br_model*)0xdead0009;gForward_sky_actor=(br_actor*)0xdead000a;
    if(!setjmp(failure)){if(mode==0)value=norm((unsigned)(uintptr_t)DRPixelmapLoad(filename));else if(mode==1)LoadDepthTable(filename,&gDepth_shade_table,&gDepth_shade_table_power);else{InitDepthEffects();if(repeat)InitDepthEffects();}}
    globals[0]=norm((unsigned)(uintptr_t)gDepth_shade_table);globals[1]=norm((unsigned)(uintptr_t)gFog_shade_table);globals[2]=norm((unsigned)(uintptr_t)gAcid_shade_table);globals[3]=norm((unsigned)(uintptr_t)gWater_shade_table);
    globals[4]=(unsigned)gDepth_shade_table_power;globals[5]=(unsigned)gFog_shade_table_power;globals[6]=(unsigned)gAcid_shade_table_power;globals[7]=(unsigned)gWater_shade_table_power;globals[8]=norm((unsigned)(uintptr_t)gHorizon_material);globals[9]=norm((unsigned)(uintptr_t)gForward_sky_model);globals[10]=norm((unsigned)(uintptr_t)gForward_sky_actor);
    printf("{\"calls\":[%s],\"globals\":[",calls);for(i=0;i<11;i++)printf("%s%u",i?",":"",globals[i]);printf("],\"objects\":[");
    for(i=0;i<count;i++){if(sizes[i]==68)normalize(pointers[i],8);if(sizes[i]==92){normalize(pointers[i],8);normalize(pointers[i],12);}if(sizes[i]==100){normalize(pointers[i],0x18);normalize(pointers[i],0x1c);}printf("%s[%u,%u,\"",i?",":"",norm((unsigned)(uintptr_t)pointers[i]),sizes[i]);dump(pointers[i],sizes[i]);printf("\"]");}
    normalize(material,0x4c);printf("],\"material\":\"");dump(material,156);printf("\",\"value\":");if(mode==0)printf("%u",value);else printf("null");printf(",\"fatal\":");if(fatal)printf("%d",fatal);else printf("null");printf(",\"fault\":false,\"fault_details\":[]}\n");return 0;
}
'''
harness.write_text(driver)
includes=[root/p for p in ['src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/carma2/platform','src/carma2/s3/include','src/brender/include']]
msvc_root=cli.msvc_root.resolve();msvc=sorted((msvc_root/'VC/Tools/MSVC').iterdir())[-1]
sdk=msvc_root/'Windows Kits/10';sdk_version=sorted(p.name for p in (sdk/'Include').iterdir())[-1]
env=dict(os.environ)
env['PATH']=str(msvc/'bin/Hostx64/x86')+os.pathsep+env['PATH']
env['INCLUDE']=os.pathsep.join(map(str,[msvc/'include',*[sdk/f'Include/{sdk_version}/{p}' for p in ['ucrt','shared','um']]]))
env['LIB']=os.pathsep.join(map(str,[msvc/'lib/x86',*[sdk/f'Lib/{sdk_version}/{p}/x86' for p in ['ucrt','um']]]))
commands={
    'msvc':[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(p) for p in includes],str(harness),str(depth_source),str(wrapper_source),'/Fo'+str(out)+os.sep,'/Fe'+str(out/'msvc.exe')],
    'mingw':[str(cli.gcc.resolve()),'-std=gnu89','-O2','-Wall','-Wextra','-Werror',*['-I'+str(p) for p in includes],str(harness),str(depth_source),str(wrapper_source),'-o',str(out/'mingw.exe')]
}
native=[]
for compiler,command in commands.items():
    result=subprocess.run(command,cwd=out,env=env if compiler=='msvc' else None,text=True,capture_output=True)
    (out/(compiler+'-build.log')).write_text(result.stdout+result.stderr)
    if result.returncode:raise AssertionError(result.stdout+result.stderr)
    for record in records:
        case=record['case']
        if case.get('expected_fault'):continue
        values=[case['mode'],case.get('width',3),case.get('height',5),case.get('row',7),case.get('screen',3),case.get('fog',0),case.get('null_load',-1),int(case.get('null_material',False)),int(case.get('repeat',False))]
        result=subprocess.run([str(out/(compiler+'.exe')),*map(str,values)],text=True,capture_output=True,check=True)
        actual=json.loads(result.stdout);expected=record['result']
        if actual!=expected:raise AssertionError((compiler,case,{k:(expected[k],actual[k]) for k in expected if expected[k]!=actual[k]}))
        native.append(dict(compiler=compiler,case=case['name'],passed=True))
cli.output.parent.mkdir(parents=True,exist_ok=True)
cli.output.write_text(json.dumps(dict(original_sha256=ORIGINAL_SHA256,recompiled_sha256=mapping['recompiled_sha256'],
                                   native_body_sha256={n:hashlib.sha256(b.encode()).hexdigest() for n,b in zip(['Log2','CreateHorizonModel','LoadDepthTable','InitDepthEffects','DRPixelmapLoad'],bodies+[wrapper_body])},
                                   x86_scenarios=records,native_scenarios=native),indent=2))
print(f'PASS: {len(records)} original/recompiled x86 scenarios and {len(native)} modern/MinGW native scenarios agree.')
