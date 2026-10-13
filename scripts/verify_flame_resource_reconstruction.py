#!/usr/bin/env python3
"""Verify flame resources with actual texture-search and material helpers.

Only platform enumeration/image decoding, BRender allocation/registry/free APIs,
ASCII CRT case conversion and FatalError are controlled boundaries. Full external
state and lossless deltas at each callback are compared. Private unwritten loader
list tails are not read; all paths/names consumed by the decoder are checked.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import struct
import subprocess
import sys

ROOT=Path(__file__).resolve().parents[1]
HASH='9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
SOURCE=ROOT/'src/carma2/newcommon/32-spark.c'
UTILITY=ROOT/'src/carma2/newcommon/41-utility.c'
WORLD=ROOT/'src/carma2/newcommon/28-world3.c'
VECTOR=ROOT/'src/brender/core/math/vector.c'
ADDR=dict(InitFlame=0x4fc3a0,DisposeFlame=0x4fc2e0,gLollipop_model=0x6aa380,gFlame_map=0x6a8638,
    gSmoke_column=0x6a96a8,gColumn_flags=0x6aa59c,gColumn_next=0x6aa5a0,
    LoadTextureTryAllLocations=0x513a30,GlorifyMaterial=0x5182f0,DRStricmp=0x515870,
    GetFileName=0x5026b0,GetAdditionalFileName=0x502780,PFForEveryFile2=0x4b4d30,DRLdImg=0x48eb80,
    gApplication_path=0x75ba60,gCurrent_load_directory=0x762180,gCurrent_load_name=0x761a80,
    gDir_separator=0x761a5c,gGraf_specs=0x662208,gGraf_spec_index=0x762324,
    gPixelFlags=0x74ca18,gRender_palette=0x74a674,gMaterial_exceptions=0x74ca04,
    gEnable_texture_interpolation=0x74cf38,gEnable_texture_antialiasing=0x74cf20,gEnable_perspective_maps=0x74ca54,
    toupper=0x5771a0,tolower=0x576860,BrModelAllocate=0x5217f0,BrActorAllocate=0x51e1a0,
    BrMaterialAllocate=0x51f240,BrActorAdd=0x51dfe0,BrActorRemove=0x51e070,BrActorFree=0x51e2c0,
    BrMaterialAdd=0x51ee50,BrMaterialRemove=0x51ee80,BrMaterialFree=0x51f2a0,
    BrModelAdd=0x51ed00,BrModelRemove=0x51ed30,BrModelFree=0x5217e0,
    BrMapAddMany=0x51f010,BrMapRemove=0x51efd0,BrPixelmapFree=0x537dc0,FatalError=0x44c230)
HEAP,STACK,STOP,RETURN,SCRATCH,SIZE=0x1000000,0x1080000,0x1090000,0x1091000,0x10a0000,0x20000
GLOBALS=['gLollipop_model','gColumn_flags','gColumn_next','gDir_separator','gGraf_spec_index','gPixelFlags','gRender_palette',
    'gMaterial_exceptions','gEnable_texture_interpolation','gEnable_texture_antialiasing','gEnable_perspective_maps']
PATHS=['gApplication_path','gCurrent_load_directory','gCurrent_load_name']
POINTER_FIELDS={1:[4,8,12],3:[0,4,8,12,20,24,28],4:[4,64],5:[4]}


def main():
    p=argparse.ArgumentParser(description=__doc__)
    for n,d in [('original',ROOT/'build-reference/originals/CARMA2_HW.EXE'),('recompiled',ROOT/'build-matching/rec2.exe'),
        ('output',ROOT/'build-audit/flame-resource-tests'),('dependency-dir',ROOT/'build-audit/joystick/python-deps'),
        ('gcc',ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'),('msvc-root',ROOT/'build-tools/msvc-modern'),
        ('msvc5-root',ROOT/'build-tools/msvc500sp3')]:p.add_argument('--'+n,type=Path,default=d)
    p.add_argument('--symbols',type=Path);args=p.parse_args();sys.path.insert(0,str(args.dependency_dir.resolve()))
    from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
    from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_EAX,UC_X86_REG_ECX,UC_X86_REG_EDX,UC_X86_REG_FPCW
    from verify_tinted_native_reconstruction import extract
    original=args.original.read_bytes();assert hashlib.sha256(original).hexdigest()==HASH;images=[(original,ADDR)]
    if args.symbols:
        mapping=json.loads(args.symbols.read_text());b=args.recompiled.read_bytes()
        assert mapping['original_sha256']==HASH and mapping['recompiled_sha256']==hashlib.sha256(b).hexdigest()
        syms={s['orig']:s for s in mapping['symbols']};images.append((b,{n:syms[a]['recomp'] for n,a in ADDR.items()}))
    out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)
    def execute(data,a,h,arena):
        u=Uc(UC_ARCH_X86,UC_MODE_32);u.mem_map(0x400000,0x500000);u.mem_map(HEAP,0x100000)
        pe=struct.unpack_from('<I',data,0x3c)[0];table=pe+24+struct.unpack_from('<H',data,pe+20)[0]
        for i in range(struct.unpack_from('<H',data,pe+6)[0]):
            _,rva,n,raw=struct.unpack_from('<IIII',data,table+40*i+8);u.mem_write(0x400000+rva,data[raw:raw+n])
        def wr(at,v):u.mem_write(at,struct.pack('<I',v&0xffffffff))
        def rd(at):return struct.unpack('<I',u.mem_read(at,4))[0]
        def string(at):
            raw=bytearray()
            while u.mem_read(at+len(raw),1)!=b'\0':raw.extend(u.mem_read(at+len(raw),1));assert len(raw)<256
            return raw.decode('ascii')
        u.mem_write(HEAP,bytes(arena));u.mem_write(a['gSmoke_column'],b'\xa5'*1240);u.mem_write(a['gFlame_map'],struct.pack('<20I',*[0x12345678]*20))
        for n,v in zip(GLOBALS,[0xdeadbeef,h[9],9,HEAP+0x1f000,0,0x12345678,HEAP+0x11000,HEAP+0x1f040 if h[5]!=0xffffffff else 0,h[3]&1,(h[3]>>1)&1,(h[3]>>2)&1]):wr(a[n],v)
        for n,s in zip(PATHS,['DATA','START','CAR']):u.mem_write(a[n],s.encode()+b'\0'+b'\xa5'*(255-len(s)))
        specs=bytearray(b'\xa5'*104);struct.pack_into('<I',specs,28,HEAP+0x1f020);struct.pack_into('<I',specs,80,HEAP+0x1f020);u.mem_write(a['gGraf_specs'],bytes(specs))
        cursor=[0];objects=[(1,0x10000,92),(2,0x10200,160),(2,0x10300,80),(5,0x11000,68),(4,0x12000,156),
            (3,0x13000,100),(3,0x13100,100),(3,0x13200,100),(3,0x13300,100)]
        calls=[];previous=[b''];counts={};state=dict(enums=0,loads=0,frame=None,status='returned')
        def full():
            return (b''.join(struct.pack('<I',rd(a[n])) for n in GLOBALS)+bytes(u.mem_read(a['gFlame_map'],80))+bytes(u.mem_read(a['gSmoke_column'],1240))
                +b''.join(bytes(u.mem_read(a[n],256)) for n in PATHS)+bytes(u.mem_read(a['gGraf_specs'],104))+bytes(u.mem_read(HEAP,SIZE)))
        def delta():
            now=full();before=previous[0];i=0;parts=[]
            while i<len(now):
                if now[i:i+1024]==before[i:i+1024]:i+=min(1024,len(now)-i);continue
                if now[i]==before[i]:i+=1;continue
                first=i
                while i<len(now) and now[i]!=before[i]:i+=1
                parts.append(f'{first:08x}:'+now[first:i].hex())
            previous[0]=now;return ';'.join(parts)
        baseline=full().hex();previous[0]=bytes.fromhex(baseline)
        def allocate(kind,n):
            off=cursor[0];cursor[0]+=(n+15)&~15;assert cursor[0]<0x10000
            u.mem_write(HEAP+off,b'\xa5'*n);objects.append((kind,off,n));return HEAP+off
        def ret(value=0,pop=0):
            sp=u.reg_read(UC_X86_REG_ESP);u.reg_write(UC_X86_REG_EAX,value&0xffffffff);u.reg_write(UC_X86_REG_EIP,rd(sp));u.reg_write(UC_X86_REG_ESP,sp+4+pop)
        def advance():
            f=state['frame']
            if f['j']<(h[1] if f['index']==h[2] else 0):
                name=f'f{f["j"]:02d}.tif';path=f['path']+string(rd(a['gDir_separator']))+name;u.mem_write(SCRATCH,path.encode()+b'\0')
                calls.append(['file',path,delta()]);wr(f['sp']-4,RETURN);u.reg_write(UC_X86_REG_ESP,f['sp']-4);u.reg_write(UC_X86_REG_ECX,SCRATCH);u.reg_write(UC_X86_REG_EDX,f['list']);u.reg_write(UC_X86_REG_EIP,f['callback'])
            else:
                calls.append(['enum-end',f['index'],delta()]);u.reg_write(UC_X86_REG_ESP,f['sp']);ret(pop=4);state['frame']=None
        apis={a[n]:n for n in ADDR if n.startswith('Br') or n=='FatalError'}
        def hook(uc,ip,size,context):
            sp=u.reg_read(UC_X86_REG_ESP)
            if ip in (a['toupper'],a['tolower']):
                c=rd(sp+4);ret(c-32 if ip==a['toupper'] and 97<=c<=122 else c+32 if ip==a['tolower'] and 65<=c<=90 else c);return
            if ip==a['PFForEveryFile2']:
                index=state['enums'];state['enums']+=1;assert index<8;path=string(uc.reg_read(UC_X86_REG_ECX));callback=uc.reg_read(UC_X86_REG_EDX)
                assert callback in (a['GetFileName'],a['GetAdditionalFileName']);calls.append(['enum',index,path,int(callback==a['GetAdditionalFileName']),delta()])
                state['frame']=dict(index=index,j=0,sp=sp,path=path,callback=callback,list=rd(sp+4));advance();return
            if ip==RETURN:
                f=state['frame'];assert f and sp==f['sp'];calls.append(['file-return',uc.reg_read(UC_X86_REG_EAX),delta()]);f['j']+=1;advance();return
            if ip==a['DRLdImg']:
                index=state['loads'];state['loads']+=1;name=string(uc.reg_read(UC_X86_REG_EDX));path=string(uc.reg_read(UC_X86_REG_ECX))
                calls.append(['load',index,path,name,rd(sp+4),rd(sp+8),delta()]);wr(rd(sp+12),0)
                value=0
                if not (h[8]==1 or h[8]==2 and index%2):
                    value=allocate(5,68);u.mem_write(HEAP+0x1f100+16*index,name.encode()+b'\0');wr(value+4,HEAP+0x1f100+16*index)
                calls.append(['loaded',value,delta()]);ret(value,12);return
            if ip not in apis:return
            name=apis[ip];number=counts.get(name,0);counts[name]=number+1;argsv=[rd(sp+4+4*i) for i in range(3)];value=0
            if name=='FatalError':calls.append(['fatal',argsv[0],string(argsv[1]),delta()]);state['status']='fatal';uc.reg_write(UC_X86_REG_EIP,STOP);return
            if name=='BrModelAllocate':
                calls.append(['model-allocate',string(argsv[0]),argsv[1],argsv[2],delta()]);value=allocate(1,92);wr(value+4,HEAP+0x1f0b0);wr(value+8,allocate(2,argsv[1]*40));wr(value+12,allocate(2,argsv[2]*40));u.mem_write(value+16,struct.pack('<HH',argsv[1],argsv[2]))
            elif name=='BrActorAllocate':
                calls.append(['actor-allocate',argsv[0],argsv[1],delta()]);value=allocate(3,100)
                for off in [0,4,8,12,20,24,28]:wr(value+off,0)
                u.mem_write(value+16,struct.pack('<HB',0,argsv[0]))
            elif name=='BrMaterialAllocate':
                assert argsv[0]==0;calls.append(['material-allocate',delta()]);value=allocate(4,156);wr(value+4,0);wr(value+0x20,h[4])
            elif name=='BrActorAdd':
                parent,child=argsv[:2];calls.append(['actor-add',parent,child,delta()]);old=rd(parent+8);wr(child,old);wr(child+4,parent+8);wr(child+12,parent);wr(parent+8,child)
                if old:wr(old+4,child)
                if h[6]&1 and number==h[7]:wr(a['gLollipop_model'],HEAP+0x10000)
                value=child
            elif name=='BrMapAddMany':
                calls.append(['map-add-many',argsv[1],bytes(u.mem_read(argsv[0],4*argsv[1])).hex(),delta()]);value=argsv[1]
                if h[6]&2:wr(a['gFlame_map'],HEAP+0x11000)
            else:
                tag={'BrMaterialAdd':'material-add','BrModelAdd':'model-add','BrMapRemove':'map-remove','BrPixelmapFree':'map-free',
                    'BrActorRemove':'actor-remove','BrMaterialRemove':'material-remove','BrMaterialFree':'material-free','BrActorFree':'actor-free','BrModelRemove':'model-remove','BrModelFree':'model-free'}[name]
                calls.append([tag,argsv[0],delta()]);value=argsv[0]
                if name=='BrMapRemove' and h[6]&4 and number==h[7]:wr(a['gFlame_map']+4*number,HEAP+0x11000)
                if name=='BrMaterialRemove' and h[6]&8 and number==h[7]:
                    for kind,off,n in objects:
                        if kind==3 and rd(HEAP+off+28)==argsv[0]:wr(HEAP+off+28,HEAP+0x12000)
                if name=='BrModelRemove' and h[6]&16:wr(a['gLollipop_model'],HEAP+0x10000)
                if name=='BrActorRemove':
                    wr(argsv[0]+12,0)
                    if h[6]&32 and number==h[7]:
                        for i in range(10):
                            if rd(a['gSmoke_column']+124*i+36)==argsv[0]:wr(a['gSmoke_column']+124*i+36,HEAP+0x13000)
                if name=='BrMaterialAdd' and h[6]&64 and number==h[7]:wr(argsv[0]+32,0x12345678)
                if name=='BrModelAdd' and h[6]&128:wr(a['gSmoke_column']+9*124+36,HEAP+0x13000)
            ret(value)
        u.hook_add(UC_HOOK_CODE,hook);u.reg_write(UC_X86_REG_FPCW,0x27f)
        def invoke(n):wr(STACK,STOP);u.reg_write(UC_X86_REG_ESP,STACK);u.emu_start(a[n],STOP,count=3000000);assert u.reg_read(UC_X86_REG_EIP)==STOP
        invoke('InitFlame')
        if h[0] and state['status']=='returned':
            wr(a['gColumn_flags'],h[9])
            for i in range(10):wr(a['gSmoke_column']+124*i+28,h[10] if h[10]!=0xffffffff else i%3)
            calls.append(['dispose-phase',delta()]);invoke('DisposeFlame')
        return dict(baseline=baseline,calls=calls,state=full().hex(),status=state['status'])
    bodies=[extract(SOURCE.read_text(),n) for n in ['InitFlame','DisposeFlame']]
    bodies+=[extract(UTILITY.read_text(),n) for n in ['Uppercaseificate','PathCat','SepDirAndFilename','LoadTextureTryAllLocations','DRStricmp','FindExceptionInList','GlorifyMaterial']]
    bodies+=[extract(WORLD.read_text(),n) for n in ['GetFileName','GetAdditionalFileName']]+[extract(VECTOR.read_text(),'BrVector3SetFloat')]
    # Definitions must precede uses for static FindExceptionInList.
    bodies=bodies[2:]+bodies[:2]
    driver=r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <ctype.h>
#include <setjmp.h>
#include <float.h>
#include "32-spark.h"
#include "41-utility.h"
#include "28-world3.h"
#include "63-loading3.h"
#include "70-packfile.h"
#include "52-errors.h"
#include "globvars.h"
#include "platform.h"
#include "rec2_macros.h"
typedef char layouts[sizeof(void*)==4&&sizeof(br_model)==92&&sizeof(br_material)==156&&sizeof(br_actor)==100&&sizeof(br_pixelmap)==68&&sizeof(tSmoke_column)==124&&sizeof(tGraf_spec)==52?1:-1];
br_model*gLollipop_model;br_pixelmap*gFlame_map[20];tSmoke_column gSmoke_column[10];int gColumn_flags,gColumn_next;
tPath_name gApplication_path;char gCurrent_load_directory[256],gCurrent_load_name[256];const char*gDir_separator;tGraf_spec gGraf_specs[2];int gGraf_spec_index;br_pixelmap*gRender_palette;tPixelFlags gPixelFlags;
tMaterial_exception*gMaterial_exceptions;int gEnable_texture_interpolation,gEnable_texture_antialiasing,gEnable_perspective_maps;
static unsigned arena[0x20000/4],h[16],cursor,nobjects,enums,loads,events,counts[16];static unsigned objects[256][3];static unsigned char previous[140000];static jmp_buf escape;static int fatal;
static unsigned norm(unsigned v){unsigned b=(unsigned)(uintptr_t)arena;if(v>=b&&v<b+sizeof(arena))return v-b+0x1000000;return v;}
static unsigned rd(unsigned off){unsigned v;memcpy(&v,(char*)arena+off,4);return v;}
static void wr(unsigned off,unsigned v){memcpy((char*)arena+off,&v,4);}
static unsigned host(unsigned v){return v>=0x1000000&&v<0x1020000?v-0x1000000+(unsigned)(uintptr_t)arena:v;}
static void hex(const void*p,unsigned n){unsigned i;for(i=0;i<n;i++)printf("%02x",((const unsigned char*)p)[i]);}
static void quoted(const char*s){putchar('"');for(;*s;s++){if(*s=='\\'||*s=='"')putchar('\\');putchar(*s);}putchar('"');}
static unsigned full(unsigned char*dest){unsigned prefix[11],i,j,v,off=0;unsigned char copy[sizeof(arena)],specs[104];unsigned maps[20];tSmoke_column columns[10];
    prefix[0]=norm((unsigned)(uintptr_t)gLollipop_model);prefix[1]=gColumn_flags;prefix[2]=gColumn_next;prefix[3]=norm((unsigned)(uintptr_t)gDir_separator);prefix[4]=gGraf_spec_index;prefix[5]=gPixelFlags;prefix[6]=norm((unsigned)(uintptr_t)gRender_palette);prefix[7]=norm((unsigned)(uintptr_t)gMaterial_exceptions);prefix[8]=gEnable_texture_interpolation;prefix[9]=gEnable_texture_antialiasing;prefix[10]=gEnable_perspective_maps;
    for(i=0;i<20;i++){maps[i]=norm((unsigned)(uintptr_t)gFlame_map[i]);}
    memcpy(columns,gSmoke_column,sizeof(columns));for(i=0;i<10;i++){columns[i].flame_actor=(br_actor*)(uintptr_t)norm((unsigned)(uintptr_t)columns[i].flame_actor);}
    memcpy(specs,gGraf_specs,sizeof(specs));for(i=0;i<2;i++){v=norm((unsigned)(uintptr_t)gGraf_specs[i].data_dir_name);memcpy(specs+52*i+28,&v,4);}
    memcpy(copy,arena,sizeof(copy));for(i=0;i<nobjects;i++){unsigned kind=objects[i][0],base=objects[i][1];unsigned fields[7],n=0;if(kind==1){fields[n++]=4;fields[n++]=8;fields[n++]=12;}else if(kind==3){fields[n++]=0;fields[n++]=4;fields[n++]=8;fields[n++]=12;fields[n++]=20;fields[n++]=24;fields[n++]=28;}else if(kind==4){fields[n++]=4;fields[n++]=64;}else if(kind==5)fields[n++]=4;for(j=0;j<n;j++){v=norm(rd(base+fields[j]));memcpy(copy+base+fields[j],&v,4);}}
    for(i=0;i<2;i++){v=norm(rd(0x1f040+4*i));memcpy(copy+0x1f040+4*i,&v,4);}
#define APPEND(p,n) do{memcpy(dest+off,p,n);off+=n;}while(0)
    APPEND(prefix,sizeof(prefix));APPEND(maps,80);APPEND(columns,1240);APPEND(gApplication_path,256);APPEND(gCurrent_load_directory,256);APPEND(gCurrent_load_name,256);APPEND(specs,104);APPEND(copy,sizeof(copy));return off;
}
static void delta(void){unsigned char now[140000];unsigned n=full(now),i=0,first;int pieces=0;while(i<n){if(now[i]==previous[i]){i++;continue;}first=i;while(i<n&&now[i]!=previous[i])i++;printf("%s%08x:",pieces++?";":"",first);hex(now+first,i-first);}memcpy(previous,now,n);}
static void event(const char*name){printf("%s[\"%s\"",events++?",":"",name);}
static void end(void){printf(",\"");delta();printf("\"]");}
static void object(unsigned kind,unsigned off,unsigned size){objects[nobjects][0]=kind;objects[nobjects][1]=off;objects[nobjects++][2]=size;}
static void*allocate(unsigned kind,unsigned size){unsigned off=cursor;cursor+=(size+15)&~15;if(cursor>=0x10000)abort();memset((char*)arena+off,0xa5,size);object(kind,off,size);return(char*)arena+off;}
void C2_HOOK_FASTCALL PFForEveryFile2(const char*path,tEnumPathCallback callback,void*data){unsigned index=enums++,j;char fullpath[256];int result;event("enum");printf(",%u,",index);quoted(path);printf(",%d",callback==(tEnumPathCallback)GetAdditionalFileName);end();for(j=0;j<(index==h[2]?h[1]:0);j++){sprintf(fullpath,"%s%sf%02u.tif",path,gDir_separator,j);event("file");putchar(',');quoted(fullpath);end();result=callback(fullpath,data);event("file-return");printf(",%u",(unsigned)result);end();}event("enum-end");printf(",%u",index);end();}
br_pixelmap*C2_HOOK_FASTCALL DRLdImg(const char*path,const char*name,br_pixelmap*palette,int flags,int*error){unsigned index=loads++;br_pixelmap*map=NULL;event("load");printf(",%u,",index);quoted(path);putchar(',');quoted(name);printf(",%u,%u",norm((unsigned)(uintptr_t)palette),(unsigned)flags);end();*error=0;if(!(h[8]==1||(h[8]==2&&(index&1)))){map=allocate(5,68);strcpy((char*)arena+0x1f100+16*index,name);map->identifier=(char*)arena+0x1f100+16*index;}event("loaded");printf(",%u",norm((unsigned)(uintptr_t)map));end();return map;}
br_model*C2_HOOK_CDECL BrModelAllocate(const char*name,int nv,int nf){br_model*model;event("model-allocate");printf(",\"%s\",%d,%d",name,nv,nf);end();model=allocate(1,92);model->identifier=(char*)arena+0x1f0b0;model->vertices=allocate(2,nv*40);model->faces=allocate(2,nf*40);model->nvertices=nv;model->nfaces=nf;return model;}
br_actor*C2_HOOK_CDECL BrActorAllocate(br_uint_8 type,void*data){br_actor*actor;event("actor-allocate");printf(",%u,%u",type,(unsigned)(uintptr_t)data);end();actor=allocate(3,100);actor->next=NULL;actor->prev=NULL;actor->children=NULL;actor->parent=NULL;actor->identifier=NULL;actor->model=NULL;actor->material=NULL;actor->depth=0;actor->type=type;return actor;}
br_material*C2_HOOK_CDECL BrMaterialAllocate(const char*name){br_material*material;if(name)abort();event("material-allocate");end();material=allocate(4,156);material->identifier=NULL;material->flags=h[4];return material;}
br_actor*C2_HOOK_CDECL BrActorAdd(br_actor*parent,br_actor*child){unsigned number=counts[0]++;event("actor-add");printf(",%u,%u",norm((unsigned)(uintptr_t)parent),norm((unsigned)(uintptr_t)child));end();child->next=parent->children;child->prev=&parent->children;child->parent=parent;parent->children=child;if(child->next)child->next->prev=&child->next;if((h[6]&1)&&number==h[7])gLollipop_model=(br_model*)((char*)arena+0x10000);return child;}
br_uint_32 C2_HOOK_CDECL BrMapAddMany(br_pixelmap**items,int count){unsigned i;event("map-add-many");printf(",%u,\"",count);for(i=0;i<(unsigned)count;i++){unsigned v=norm((unsigned)(uintptr_t)items[i]);hex(&v,4);}printf("\"");end();if(h[6]&2)gFlame_map[0]=(br_pixelmap*)((char*)arena+0x11000);return count;}
br_material*C2_HOOK_CDECL BrMaterialAdd(br_material*material){unsigned number=counts[1]++;event("material-add");printf(",%u",norm((unsigned)(uintptr_t)material));end();if((h[6]&64)&&number==h[7])material->flags=0x12345678;return material;}
br_model*C2_HOOK_CDECL BrModelAdd(br_model*model){event("model-add");printf(",%u",norm((unsigned)(uintptr_t)model));end();if(h[6]&128)gSmoke_column[9].flame_actor=(br_actor*)((char*)arena+0x13000);return model;}
br_pixelmap*C2_HOOK_CDECL BrMapRemove(br_pixelmap*map){unsigned number=counts[2]++;event("map-remove");printf(",%u",norm((unsigned)(uintptr_t)map));end();if((h[6]&4)&&number==h[7])gFlame_map[number]=(br_pixelmap*)((char*)arena+0x11000);return map;}
void C2_HOOK_CDECL BrPixelmapFree(br_pixelmap*map){event("map-free");printf(",%u",norm((unsigned)(uintptr_t)map));end();}
br_actor*C2_HOOK_CDECL BrActorRemove(br_actor*actor){unsigned number=counts[3]++,i;event("actor-remove");printf(",%u",norm((unsigned)(uintptr_t)actor));end();actor->parent=NULL;if((h[6]&32)&&number==h[7])for(i=0;i<10;i++)if(gSmoke_column[i].flame_actor==actor)gSmoke_column[i].flame_actor=(br_actor*)((char*)arena+0x13000);return actor;}
br_material*C2_HOOK_CDECL BrMaterialRemove(br_material*material){unsigned number=counts[4]++,i;event("material-remove");printf(",%u",norm((unsigned)(uintptr_t)material));end();if((h[6]&8)&&number==h[7])for(i=0;i<nobjects;i++)if(objects[i][0]==3){br_actor*actor=(br_actor*)((char*)arena+objects[i][1]);if(actor->material==material)actor->material=(br_material*)((char*)arena+0x12000);}return material;}
void C2_HOOK_CDECL BrMaterialFree(br_material*material){event("material-free");printf(",%u",norm((unsigned)(uintptr_t)material));end();}
void C2_HOOK_CDECL BrActorFree(br_actor*actor){event("actor-free");printf(",%u",norm((unsigned)(uintptr_t)actor));end();}
br_model*C2_HOOK_CDECL BrModelRemove(br_model*model){event("model-remove");printf(",%u",norm((unsigned)(uintptr_t)model));end();if(h[6]&16)gLollipop_model=(br_model*)((char*)arena+0x10000);return model;}
void C2_HOOK_CDECL BrModelFree(br_model*model){event("model-free");printf(",%u",norm((unsigned)(uintptr_t)model));end();}
void C2_NORETURN C2_HOOK_CDECL FatalError(int code,...){va_list ap;char*name;va_start(ap,code);name=va_arg(ap,char*);event("fatal");printf(",%u,\"%s\"",code,name);end();va_end(ap);fatal=1;longjmp(escape,1);}
'''
    driver+='\n\n'.join(bodies)
    driver+=r'''
int C2_HOOK_CDECL main(int argc,char**argv){FILE*file;unsigned i,j,n,v;unsigned char baseline[140000];if(argc!=2)return 1;file=fopen(argv[1],"rb");if(!file)return 2;if(fread(h,sizeof(h),1,file)!=1||fread(arena,sizeof(arena),1,file)!=1)return 3;fclose(file);_controlfp(_PC_53,_MCW_PC);
    object(1,0x10000,92);object(2,0x10200,160);object(2,0x10300,80);object(5,0x11000,68);object(4,0x12000,156);for(i=0;i<4;i++)object(3,0x13000+256*i,100);
    for(i=0;i<nobjects;i++){unsigned kind=objects[i][0],base=objects[i][1],fields[7],count=0;if(kind==1){fields[count++]=4;fields[count++]=8;fields[count++]=12;}else if(kind==3){fields[count++]=0;fields[count++]=4;fields[count++]=8;fields[count++]=12;fields[count++]=20;fields[count++]=24;fields[count++]=28;}else if(kind==4){fields[count++]=4;fields[count++]=64;}else if(kind==5)fields[count++]=4;for(j=0;j<count;j++){v=rd(base+fields[j]);wr(base+fields[j],host(v));}}
    for(i=0;i<2;i++)wr(0x1f040+4*i,host(rd(0x1f040+4*i)));
    memset(gSmoke_column,0xa5,1240);for(i=0;i<20;i++)gFlame_map[i]=(br_pixelmap*)(uintptr_t)0x12345678;gLollipop_model=(br_model*)(uintptr_t)0xdeadbeef;gColumn_flags=h[9];gColumn_next=9;gDir_separator=(char*)arena+0x1f000;gGraf_spec_index=0;gPixelFlags=(tPixelFlags)0x12345678;gRender_palette=(br_pixelmap*)((char*)arena+0x11000);gMaterial_exceptions=h[5]==0xffffffff?NULL:(tMaterial_exception*)((char*)arena+0x1f040);gEnable_texture_interpolation=h[3]&1;gEnable_texture_antialiasing=(h[3]>>1)&1;gEnable_perspective_maps=(h[3]>>2)&1;
    memset(gApplication_path,0xa5,256);strcpy(gApplication_path,"DATA");memset(gCurrent_load_directory,0xa5,256);strcpy(gCurrent_load_directory,"START");memset(gCurrent_load_name,0xa5,256);strcpy(gCurrent_load_name,"CAR");memset(gGraf_specs,0xa5,104);gGraf_specs[0].data_dir_name=gGraf_specs[1].data_dir_name=(char*)arena+0x1f020;
    n=full(baseline);memcpy(previous,baseline,n);printf("{\"baseline\":\"");hex(baseline,n);printf("\",\"calls\":[");if(setjmp(escape)==0){InitFlame();if(h[0]){gColumn_flags=h[9];for(i=0;i<10;i++)gSmoke_column[i].colour=h[10]==0xffffffff?i%3:h[10];event("dispose-phase");end();DisposeFlame();}}
    printf("],\"state\":\"");n=full(baseline);hex(baseline,n);printf("\",\"status\":\"%s\"}\n",fatal?"fatal":"returned");return 0;
}
'''
    harness=out/'harness.c';harness.write_text(driver)
    includes=[ROOT/r for r in ['src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/carma2/platform','src/carma2/s3/include','src/brender/include']]
    msvc=sorted((args.msvc_root.resolve()/'VC/Tools/MSVC').iterdir())[-1];sdk=args.msvc_root.resolve()/'Windows Kits/10';version=sorted(p.name for p in (sdk/'Include').iterdir())[-1]
    env=dict(os.environ);env['PATH']=str(msvc/'bin/Hostx64/x86')+os.pathsep+env['PATH'];env['INCLUDE']=os.pathsep.join(map(str,[msvc/'include',*[sdk/f'Include/{version}/{r}' for r in ['ucrt','shared','um']]]));env['LIB']=os.pathsep.join(map(str,[msvc/'lib/x86',*[sdk/f'Lib/{version}/{r}/x86' for r in ['ucrt','um']]]))
    old=args.msvc5_root.resolve();oldenv=dict(os.environ);oldenv['PATH']=str(old/'bin')+os.pathsep+str(old/'redist')+os.pathsep+oldenv['PATH'];oldenv['INCLUDE']=str(old/'include');oldenv['LIB']=str(old/'lib')
    commands={'msvc':[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(r) for r in includes],str(harness),'/Fo'+str(out/'msvc.obj'),'/Fe'+str(out/'msvc.exe')],
        'msvc5':[str(old/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(r) for r in includes],str(harness),'/Fo'+str(out/'msvc5.obj'),'/Fe'+str(out/'msvc5.exe')],
        'mingw':[str(args.gcc.resolve()),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-fno-strict-aliasing',*['-I'+str(r) for r in includes],str(harness),'-o',str(out/'mingw.exe')]}
    for compiler,cmd in commands.items():
        build=subprocess.run(cmd,cwd=out,env=env if compiler=='msvc' else oldenv if compiler=='msvc5' else None,text=True,capture_output=True)
        (out/(compiler+'-build.log')).write_text(build.stdout+build.stderr);assert build.returncode==0,build.stdout+build.stderr
    cases=[dict(mode=mode,renderer=flags) for mode in [0,1] for flags in range(8)]
    cases+=[dict(count=count,slot=slot) for count in [0,19,20] for slot in [0,4,7]]
    cases+=[dict(exception=flags,renderer=7) for flags in [0,1,8,9]]
    cases+=[dict(mode=1,active=flags,colour=colour) for flags in [0,1,0x200,0x3ff,0x80000000] for colour in [0,1,0xffffffff]]
    cases+=[dict(mode=1,mutate=mask,at=at) for mask in [1,4,8,32,64] for at in [0,19 if mask==4 else 9 if mask==32 else 29]]
    cases+=[dict(mode=1,mutate=mask) for mask in [2,16,128,30]]+[dict(nullable=n) for n in [1,2]]
    results=[]
    for index,case in enumerate(cases):
        h=[case.get('mode',0),case.get('count',20),case.get('slot',0),case.get('renderer',7),case.get('material_flags',0xa5a5a5a5),case.get('exception',0xffffffff),case.get('mutate',0),case.get('at',0),case.get('nullable',0),case.get('active',0x3ff),case.get('colour',0),0,0,0,0,0]
        arena=bytearray(b'\xa5'*SIZE)
        def put(off,v):struct.pack_into('<I',arena,off,v&0xffffffff)
        for off,s in [(0x1f000,'\\'),(0x1f020,'GFX'),(0x1f080,'f00'),(0x1f0a0,'ALT'),(0x1f0b0,'Lollipop')]:arena[off:off+len(s)+1]=s.encode()+b'\0'
        put(0x10004,HEAP+0x1f0b0);put(0x10008,HEAP+0x10200);put(0x1000c,HEAP+0x10300);struct.pack_into('<HH',arena,0x10010,4,2)
        put(0x11004,HEAP+0x1f0a0);put(0x12004,0);put(0x12040,HEAP+0x11000)
        for i in range(4):
            for off in [0,4,8,12,20,24,28]:put(0x13000+256*i+off,0)
            if i:put(0x13000+256*i,HEAP+0x13000+256*(i+1) if i<3 else 0);put(0x13000+256*i+28,HEAP+0x12000)
        put(0x13008,HEAP+0x13100);put(0x1f040,0);put(0x1f044,HEAP+0x1f080);put(0x1f048,h[5])
        fixture=out/'fixture.bin';fixture.write_bytes(struct.pack('<16I',*h)+arena)
        expected=execute(*images[0],h,arena);checks=[]
        if len(images)>1:checks.append(('recompiled-x86',execute(*images[1],h,arena)))
        for compiler in commands:checks.append((compiler,json.loads(subprocess.check_output([str(out/(compiler+'.exe')),str(fixture)],text=True))))
        for compiler,actual in checks:
            if actual!=expected:
                failure=out/f'failure-{compiler}-{index}.json';failure.write_text(json.dumps(dict(case=case,original=expected,actual=actual),indent=2))
                raise AssertionError(f'{compiler} flame resource case{index} differs: {failure}')
            results.append(dict(case=case,compiler=compiler,passed=True,calls=len(actual['calls']),state_sha256=hashlib.sha256(bytes.fromhex(actual['state'])).hexdigest()))
    verification=dict(scenario_count=len(cases),comparisons=len(results),original_sha256=HASH,source_sha256=hashlib.sha256(SOURCE.read_bytes()).hexdigest(),utility_sha256=hashlib.sha256(UTILITY.read_bytes()).hexdigest(),world_sha256=hashlib.sha256(WORLD.read_bytes()).hexdigest(),vector_sha256=hashlib.sha256(VECTOR.read_bytes()).hexdigest(),bodies_sha256=[hashlib.sha256(b.encode()).hexdigest() for b in bodies],compiler_commands=commands,cases=results,oracle_normalizations=[])
    if args.symbols:verification.update(recompiled_sha256=hashlib.sha256(args.recompiled.read_bytes()).hexdigest(),symbols_sha256=hashlib.sha256(args.symbols.read_bytes()).hexdigest())
    (out/'verification.json').write_text(json.dumps(verification,indent=2)+'\n');print(f'PASS: {len(cases)} scenarios, {len(results)} flame resource comparisons.')


if __name__=='__main__':main()
