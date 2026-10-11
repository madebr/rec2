#!/usr/bin/env python3
"""Compare seven real geometry entries with the hash-checked original at PC53.

Requires unicorn==2.1.4, reccmp==0.1.7 for export-map, scoped modern x86
MSVC and i686 MinGW. Native harnesses contain verbatim production bodies and
repository types. Finite matrix fixtures use actual BRender inverse/apply
bodies; native transform dispatch supports the exercised matrix/translation/
identity cases and aborts if an unexercised lower dependency is reached.
Callbacks record all arguments, geometry, arena data and nearest-hit state.
Only zero signs in the unused global inverse pick matrix are canonicalized:
the existing BRender inverse returns four negative identity zeros that the
original returns as positive zeros. Geometry/output data remain bit-exact.
This is a geometry/state verifier, not a rendered-image or crash recovery test.
"""
import argparse
import hashlib
import json
import os
import random
from pathlib import Path
import struct
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
ORIGINAL_SHA256 = '9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
FUNCTIONS = dict(FindFace=0x41e340, ActorFindFace=0x45ca60, ActorRayPick2D=0x45cb70,
    PickBoundsTestRay__finteray=0x45cd90, FindHighestCallBack__finteray=0x45cf20,
    DRModelPick2D__finteray=0x45cf60, FindHighestPolyCallBack__finteray=0x45d570,
    EnablePlingMaterials=0x45d5c0, DisablePlingMaterials=0x45d5d0)
GLOBALS = dict(gPling_materials=0x5964c0, gTemp_group=0x6861c8, gNearest_face=0x686188,
    gNearest_model=0x68618c, gNearest_actor=0x6861cc, gNearest_T=0x686190,
    gNearest_face_group=0x686194, gPick_model_to_view__finteray=0x686198, gTrack_actor=0x75bb60)
ADDRESSES = dict(FUNCTIONS, **GLOBALS)
HEAP, STACK, STOP, CALLBACK, ARENA_SIZE = 0x1000000, 0x1080000, 0x1081000, 0x1082000, 0x5000
ACTORS, MODELS, PREPARED, GROUPS, VERTICES, FACES, SLOTS, MATERIALS = 0x100, 0x600, 0x900, 0xc00, 0x1800, 0x2800, 0x3000, 0x3200

def words(values):
    return struct.pack('<'+'I'*len(values), *[v & 0xffffffff for v in values])

def bits(value):
    return struct.unpack('<I', struct.pack('<f', value))[0]

def pointer_slots():
    result=[0x78,0x7c]
    for i in range(6):result += [ACTORS+128*i+j for j in [0,4,8,12,20,24,28,92,96]]
    for i in range(3):result += [MODELS+128*i+76,PREPARED+64*i+24]
    for i in range(6):result += [GROUPS+36*i+j for j in [0,4,8,12,16,20,24]]+[SLOTS+4*i]
    for i in range(4):result += [MATERIALS+256*i+4]
    return result

def make_case(options):
    """Return a fixed-address arena, initial globals and mode for Fixture/native."""
    data=bytearray(b'\xa5'*ARENA_SIZE)
    def wr(off,value):struct.pack_into('<I',data,off,value & 0xffffffff)
    def half(off,value):struct.pack_into('<H',data,off,value & 65535)
    def floats(off,values):data[off:off+4*len(values)]=words([bits(v) for v in values])
    def ptr(off,target):wr(off,HEAP+target if target is not None else 0)
    floats(0,options.get('min',[-1,-1,-1]));floats(12,options.get('max',[1,1,1]))
    if 'min_bits' in options:data[0:12]=words(options['min_bits'])
    if 'max_bits' in options:data[12:24]=words(options['max_bits'])
    floats(0x40,options.get('rp',[0.25,0.25,1]));floats(0x50,options.get('rd',[0,0,-2]))
    if 'rp_bits' in options:data[0x40:0x4c]=words(options['rp_bits'])
    if 'rd_bits' in options:data[0x50:0x5c]=words(options['rd_bits'])
    floats(0x70,[options.get('near',0),options.get('far',1)])
    wr(0x78,0xdead0078);wr(0x7c,0xdead007c)
    matrix=[1,0,0,0,1,0,0,0,1,0,0,0]
    for i in range(6):
        off=ACTORS+128*i;data[off:off+100]=bytes(100)
        data[off+18]=1 if i else 0
        floats(off+44,matrix)
    for i in range(3):
        off=MODELS+128*i;data[off:off+92]=bytes(92);ptr(off+76,PREPARED+64*i)
        floats(off+52,[-10,-10,-10,10,10,10])
        off=PREPARED+64*i;data[off:off+28]=bytes(28);half(off+8,2);ptr(off+24,GROUPS+72*i)
    axis=options.get('axis',2);sign=options.get('sign',1)
    axes=[j for j in range(3) if j!=axis]
    for i in range(6):
        off=GROUPS+36*i;data[off:off+36]=bytes(36)
        ptr(off+4,FACES+256*i);ptr(off+8,SLOTS+4*i);ptr(off+16,VERTICES+256*i)
        half(off+28,options.get('faces',2));half(off+30,3)
        ptr(SLOTS+4*i,None if options.get('null_material') else MATERIALS+256*(i%4))
        vertices=options.get('vertices',[[0,0],[1,0],[0,1]])
        for j in range(3):
            v=VERTICES+256*i+32*j;data[v:v+32]=bytes(32)
            position=[0,0,0];position[axes[0]]=vertices[j][0];position[axes[1]]=vertices[j][1]
            position[axis]=options.get('planes',[0,0])[i%2]
            if 'vertices3d' in options:position=options['vertices3d'][j]
            floats(v,position);floats(v+12,options.get('maps',[[2,3],[5,7],[11,13]])[j])
        for j in range(2):
            f=FACES+256*i+28*j;data[f:f+28]=bytes(28)
            data[f:f+6]=struct.pack('<3H',0,1,2)
            normal=options.get('normal',[sign if k==axis else 0 for k in range(3)])
            floats(f+12,normal+[options.get('plane_constant',sign*options.get('planes',[0,0])[i%2])])
    for i in range(4):
        off=MATERIALS+256*i;data[off:off+156]=bytes(156)
        ptr(off+4,0x4000+32*i);wr(off+32,options.get('flags',0x1800))
        text=(options.get('identifier','normal')+str(i)).encode()+b'\0';data[0x4000+32*i:0x4000+32*i+len(text)]=text
    ptr(ACTORS+8,ACTORS+128);ptr(ACTORS+24,MODELS);ptr(ACTORS+28,MATERIALS)
    ptr(ACTORS+128,ACTORS+256);ptr(ACTORS+256,None)
    ptr(ACTORS+256+24,MODELS+128);ptr(ACTORS+256+28,MATERIALS+256)
    if options.get('empty'):ptr(ACTORS+8,None)
    if options.get('no_model'):ptr(ACTORS+24,None);ptr(ACTORS+256+24,None)
    if options.get('none'):data[ACTORS+128+32]=1
    if options.get('bounds_actor'):
        data[ACTORS+18]=options['bounds_actor'];ptr(ACTORS+92,0)
    if options.get('nested'):
        ptr(ACTORS+128, None);ptr(ACTORS+128+8,ACTORS+256)
    if options.get('transform'):
        ptr(ACTORS+128+20,0x4100);data[0x4100:0x4103]=b'&a\0'
        floats(ACTORS+128+44,options.get('matrix',[1,0,0,0,1,0,0,0,1,0.125,-0.125,0.25]))
        half(ACTORS+128+40,options.get('transform_type',0))
    if options.get('no_groups'):half(PREPARED+8,0);half(PREPARED+64+8,0)
    if options.get('null_slot'):ptr(GROUPS+8,None)
    if 'plane_bits' in options:wr(FACES+12+4*axis,options['plane_bits'])
    init=[options.get('pling',1), 77, 88, HEAP+MODELS+256, HEAP+ACTORS+640,
          options.get('nearest_bits',bits(options.get('nearest',100))), 99]
    return dict(options=options,data=bytes(data),globals=init,mode=options.get('mode',1),
                callback_return=options.get('callback_return',0),mutate=options.get('mutate',0))

class Fixture:
    def __init__(self,image,addresses,case):
        from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE,UC_HOOK_MEM_INVALID
        from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_EAX,UC_X86_REG_ECX,UC_X86_REG_EDX,UC_X86_REG_FPCW
        self.regs=(UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_EAX,UC_X86_REG_ECX,UC_X86_REG_EDX)
        self.u=Uc(UC_ARCH_X86,UC_MODE_32);self.addresses=addresses;self.case=case;self.events=bytearray();self.calls=0;self.fault=None
        pe=struct.unpack_from('<I',image,0x3c)[0];table=pe+24+struct.unpack_from('<H',image,pe+20)[0]
        base=struct.unpack_from('<I',image,pe+52)[0];size=struct.unpack_from('<I',image,pe+80)[0]
        self.u.mem_map(base,(size+4095)&~4095);self.u.mem_map(HEAP,0x100000)
        for i in range(struct.unpack_from('<H',image,pe+6)[0]):
            _,rva,n,raw=struct.unpack_from('<IIII',image,table+40*i+8)
            if n:self.u.mem_write(base+rva,image[raw:raw+n])
        self.u.mem_write(HEAP,case['data'])
        for name,value in zip(list(GLOBALS)[:7],case['globals']):self.write(addresses[name],value)
        self.u.mem_write(addresses['gPick_model_to_view__finteray'],b'\xa5'*48)
        self.write(addresses['gTrack_actor'],HEAP+ACTORS)
        self.u.reg_write(UC_X86_REG_FPCW,0x27f)
        self.u.hook_add(UC_HOOK_CODE,self.hook)
        def invalid(u,access,address,size,value,context):self.fault=(access,address,size);return False
        self.u.hook_add(UC_HOOK_MEM_INVALID,invalid)
    def write(self,p,value):self.u.mem_write(p,words([value]))
    def read(self,p):return struct.unpack('<I',self.u.mem_read(p,4))[0]
    def snapshot(self):
        values=[self.read(self.addresses[n]) for n in list(GLOBALS)[:7]]
        matrix=struct.unpack('<12I',self.u.mem_read(self.addresses['gPick_model_to_view__finteray'],48))
        matrix=words([0 if value==0x80000000 else value for value in matrix])
        return words(values)+matrix+words([self.read(self.addresses['gTrack_actor'])])+bytes(self.u.mem_read(HEAP,ARENA_SIZE))
    def hook(self,u,address,size,context):
        if address!=CALLBACK:return
        esp,eip,eax,ecx,edx=self.regs;sp=u.reg_read(esp);a=[self.read(sp+4+4*i) for i in range(12)]
        if self.case['mode']==2:
            payload=words(a[:3])+bytes(u.mem_read(a[3],12))+bytes(u.mem_read(a[4],12))+words(a[5:8]);kind=2
        else:
            payload=words(a[:3])+bytes(u.mem_read(a[3],12))+bytes(u.mem_read(a[4],12))+words(a[5:9])+bytes(u.mem_read(a[9],12))+bytes(u.mem_read(a[10],8))+words(a[11:]);kind=1
        self.events+=words([kind])+payload+self.snapshot()
        if self.calls==0:
            if self.case['mutate'] & 1:self.u.mem_write(HEAP+GROUPS+28,struct.pack('<H',1))
            if self.case['mutate'] & 2:self.u.mem_write(HEAP+PREPARED+8,struct.pack('<H',1))
            if self.case['mutate'] & 4:self.write(self.addresses['gNearest_T'],bits(0.125))
            if self.case['mutate'] & 8:self.write(HEAP+ACTORS+128,0)
        self.calls+=1
        u.reg_write(eax,self.case['callback_return'] & 0xffffffff);u.reg_write(eip,self.read(sp));u.reg_write(esp,sp+4)
    def execute(self):
        from unicorn import UcError
        esp,eip,eax,ecx,edx=self.regs;m=self.case['mode'];self.write(STACK,STOP)
        if m==0:name='PickBoundsTestRay__finteray';first=[HEAP,HEAP+0x40];rest=[HEAP+0x50,self.read(HEAP+0x70),self.read(HEAP+0x74),HEAP+0x70,HEAP+0x74]
        elif m==1:name='DRModelPick2D__finteray';first=[HEAP+MODELS,HEAP+MATERIALS];rest=[HEAP+ACTORS+128,HEAP+0x40,HEAP+0x50,self.read(HEAP+0x70),self.read(HEAP+0x74),CALLBACK,0x12345678]
        elif m==2:name='ActorRayPick2D';first=[HEAP+ACTORS,HEAP+0x40];rest=[HEAP+0x50,0,0,CALLBACK]
        elif m==3:
            name='FindHighestPolyCallBack__finteray';first=[];rest=[HEAP+MODELS,HEAP+ACTORS+128,HEAP+MATERIALS,HEAP+0x40,HEAP+0x50,self.case['options'].get('t_bits',bits(0.5)),2,1,0,HEAP+0x40,HEAP+0x60,0]
        elif m==4:name='FindHighestCallBack__finteray';first=[];rest=[HEAP+ACTORS+128,HEAP+MODELS,HEAP+MATERIALS,HEAP+0x40,HEAP+0x50,self.read(HEAP+0x70),self.read(HEAP+0x74),0]
        elif m==5:name='ActorFindFace';first=[HEAP+0x40,HEAP+0x50];rest=[HEAP+ACTORS,HEAP+0x60,HEAP+0x70,HEAP+0x78,0 if self.case['options'].get('no_output_actor') else HEAP+0x7c]
        else:name='FindFace';first=[HEAP+0x40,HEAP+0x50];rest=[HEAP+0x60,HEAP+0x70,HEAP+0x78]
        for i,v in enumerate(rest):self.write(STACK+4+4*i,v)
        if first:self.u.reg_write(ecx,first[0]);self.u.reg_write(edx,first[1])
        self.u.reg_write(esp,STACK)
        try:self.u.emu_start(self.addresses[name],STOP,count=1000000)
        except UcError:
            if not self.fault:raise
            return dict(fault=self.fault,events=bytes(self.events),state=self.snapshot())
        if self.u.reg_read(eip)!=STOP:raise AssertionError('Instruction budget exhausted')
        value=self.u.reg_read(eax) if m<5 else 0
        self.events+=words([0,value])+self.snapshot()
        return dict(events=bytes(self.events),fault=None)

def compile_native(output,gcc,msvc_root,extra_bodies='',extra_driver='',source_transform=None):
    from verify_tinted_native_reconstruction import extract
    output=Path(output).resolve();output.mkdir(parents=True,exist_ok=True)
    source=(ROOT/'src/carma2/newcommon/34-finteray.c').read_text()
    matrix=(ROOT/'src/brender/core/math/matrix34.c').read_text()
    bodies=[extract(source,'FinterayScalar')]+[extract(source,n) for n in FUNCTIONS if n!='FindFace']
    bodies.append(extract((ROOT/'src/carma2/newcommon/00-car.c').read_text(),'FindFace'))
    matrix_bodies=[extract(matrix,n) for n in ['BrMatrix34Inverse','BrMatrix34ApplyP','BrMatrix34ApplyV']]
    prefix='''#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <fcntl.h>
#include <io.h>
#include "34-finteray.h"
#include "00-car.h"
#include "globvars.h"
#include <brender/brender.h>
#include "rec2_macros.h"
#include "c2_math.h"
typedef char layouts[sizeof(void*)==4 && sizeof(br_actor)==100 && sizeof(br_model)==92 && sizeof(v11group)==36 && sizeof(DR_FACE)==28 && sizeof(DR_VERTEX)==32 ? 1:-1];
int gPling_materials,gTemp_group,gNearest_face,gNearest_face_group;
br_model* gNearest_model;br_actor* gNearest_actor;br_actor* gTrack_actor;
br_scalar gNearest_T;br_matrix34 gPick_model_to_view__finteray;
#define A(x,y) A->m[x][y]
#define B(x,y) B->m[x][y]
#define C(x,y) C->m[x][y]
#undef BR_MAC3
#undef BR_MAC4
#define BR_MAC3(a,b,c,d,e,f) ((a)*(b)+(c)*(d)+(e)*(f))
#define BR_MAC4(a,b,c,d,e,f,g,h) ((a)*(b)+(c)*(d)+(e)*(f)+(g)*(h))
'''
    # Transform dispatch is the fixture boundary; all exercised numeric matrix
    # operations below are actual production BRender functions.
    transform='''void C2_HOOK_CDECL BrTransformToMatrix34(br_matrix34* mat, br_transform* xform) {
    if(xform->type==0 || xform->type==1) *mat=xform->t.mat;
    else if(xform->type==5 || xform->type==6) {
        memset(mat,0,sizeof(*mat));mat->m[0][0]=mat->m[1][1]=mat->m[2][2]=1.f;
        if(xform->type==5)mat->m[3]=xform->t.look_up.t;
    } else {fprintf(stderr,"Unsupported transform fixture\\n");exit(91);}
}
'''.replace('mat->m[3]=xform->t.look_up.t;', 'memcpy(mat->m[3],&xform->t.look_up.t,12);')
    driver=r'''
static unsigned arena[0x5000/4];static unsigned callback_return,mutate,calls;
static unsigned rd(unsigned o){unsigned v;memcpy(&v,(char*)arena+o,4);return v;}
static void wr(unsigned o,unsigned v){memcpy((char*)arena+o,&v,4);}
static unsigned norm(unsigned v){unsigned base=(unsigned)(uintptr_t)arena;return v>=base && v<base+sizeof(arena)?v-base+0x1000000:v;}
static void emit(unsigned v){fwrite(&v,4,1,stdout);}
static void snapshot(void){unsigned char copy[sizeof(arena)];unsigned i,v;
    emit((unsigned)gPling_materials);emit((unsigned)gTemp_group);emit((unsigned)gNearest_face);
    emit(norm((unsigned)(uintptr_t)gNearest_model));emit(norm((unsigned)(uintptr_t)gNearest_actor));
    memcpy(&v,&gNearest_T,4);emit(v);emit((unsigned)gNearest_face_group);
    for(i=0;i<12;i++){memcpy(&v,(char*)&gPick_model_to_view__finteray+4*i,4);emit(v==0x80000000?0:v);}
    emit(norm((unsigned)(uintptr_t)gTrack_actor));
    memcpy(copy,arena,sizeof(copy));
    for(i=0;i<sizeof(pointer_offsets)/sizeof(pointer_offsets[0]);i++){v=norm(rd(pointer_offsets[i]));memcpy(copy+pointer_offsets[i],&v,4);}
    fwrite(copy,sizeof(copy),1,stdout);
}
static void mutation(void){if(calls++!=0)return;
    if(mutate&1)*(unsigned short*)((char*)arena+0xc00+28)=1;
    if(mutate&2)*(unsigned short*)((char*)arena+0x900+8)=1;
    if(mutate&4)gNearest_T=0.125f;
    if(mutate&8)wr(0x180,0);
}
static int C2_HOOK_CDECL model_callback(br_model* model,br_actor* actor,br_material* material,br_vector3* rp,br_vector3* rd_,br_scalar t,int f,int e,int v,br_vector3* p,br_vector2* map,void* arg){
    unsigned tb;memcpy(&tb,&t,4);emit(1);emit(norm((unsigned)(uintptr_t)model));emit(norm((unsigned)(uintptr_t)actor));emit(norm((unsigned)(uintptr_t)material));
    fwrite(rp,12,1,stdout);fwrite(rd_,12,1,stdout);emit(tb);emit((unsigned)f);emit((unsigned)e);emit((unsigned)v);
    fwrite(p,12,1,stdout);fwrite(map,8,1,stdout);emit((unsigned)(uintptr_t)arg);snapshot();mutation();return (int)callback_return;
}
static int C2_HOOK_CDECL actor_callback(br_actor* actor,br_model* model,br_material* material,br_vector3* rp,br_vector3* rd_,br_scalar near_,br_scalar far_,void* arg){
    unsigned tb;emit(2);emit(norm((unsigned)(uintptr_t)actor));emit(norm((unsigned)(uintptr_t)model));emit(norm((unsigned)(uintptr_t)material));
    fwrite(rp,12,1,stdout);fwrite(rd_,12,1,stdout);memcpy(&tb,&near_,4);emit(tb);memcpy(&tb,&far_,4);emit(tb);emit((unsigned)(uintptr_t)arg);
    snapshot();mutation();return (int)callback_return;
}
int main(int argc,char** argv){FILE* input;unsigned header[11],i,v;int r=0;float t;char* a=(char*)arena;
    if(argc!=2)return 90;
    input=fopen(argv[1],"rb");if(!input)return 90;
    if(fread(header,sizeof(header),1,input)!=1 || fread(arena,sizeof(arena),1,input)!=1)return 90;
    fclose(input);
    _setmode(_fileno(stdout),_O_BINARY);_controlfp(_PC_53,_MCW_PC);callback_return=header[1];mutate=header[2];
    for(i=0;i<sizeof(pointer_offsets)/sizeof(pointer_offsets[0]);i++){v=rd(pointer_offsets[i]);if(v>=0x1000000 && v<0x1005000)wr(pointer_offsets[i],v-0x1000000+(unsigned)(uintptr_t)arena);}
    gPling_materials=(int)header[3];gTemp_group=(int)header[4];gNearest_face=(int)header[5];
    gNearest_model=(br_model*)(a+header[6]-0x1000000);gNearest_actor=(br_actor*)(a+header[7]-0x1000000);
    memcpy(&gNearest_T,header+8,4);gNearest_face_group=(int)header[9];memset(&gPick_model_to_view__finteray,0xa5,48);gTrack_actor=(br_actor*)(a+0x100);
    memcpy(&t,header+10,4);
    switch(header[0]){
    case 0:r=PickBoundsTestRay__finteray((br_bounds*)a,(br_vector3*)(a+0x40),(br_vector3*)(a+0x50),*(float*)(a+0x70),*(float*)(a+0x74),(float*)(a+0x70),(float*)(a+0x74));break;
    case 1:r=DRModelPick2D__finteray((br_model*)(a+0x600),(br_material*)(a+0x3200),(br_actor*)(a+0x180),(br_vector3*)(a+0x40),(br_vector3*)(a+0x50),*(float*)(a+0x70),*(float*)(a+0x74),model_callback,(void*)0x12345678);break;
    case 2:r=ActorRayPick2D((br_actor*)(a+0x100),(br_vector3*)(a+0x40),(br_vector3*)(a+0x50),NULL,NULL,actor_callback);break;
    case 3:r=FindHighestPolyCallBack__finteray((br_model*)(a+0x600),(br_actor*)(a+0x180),(br_material*)(a+0x3200),(br_vector3*)(a+0x40),(br_vector3*)(a+0x50),t,2,1,0,(br_vector3*)(a+0x40),(br_vector2*)(a+0x60),NULL);break;
    case 4:r=FindHighestCallBack__finteray((br_actor*)(a+0x180),(br_model*)(a+0x600),(br_material*)(a+0x3200),(br_vector3*)(a+0x40),(br_vector3*)(a+0x50),*(float*)(a+0x70),*(float*)(a+0x74),NULL);break;
    case 5:ActorFindFace((br_vector3*)(a+0x40),(br_vector3*)(a+0x50),(br_actor*)(a+0x100),(br_vector3*)(a+0x60),(float*)(a+0x70),(br_material**)(a+0x78),header[10]==0xffffffff?NULL:(br_actor**)(a+0x7c));break;
    case 6:FindFace((br_vector3*)(a+0x40),(br_vector3*)(a+0x50),(br_vector3*)(a+0x60),(float*)(a+0x70),(br_material**)(a+0x78));break;
    default:return 90;}
    emit(0);emit((unsigned)r);snapshot();return 0;
}
'''
    offsets='static unsigned pointer_offsets[]={'+','.join(map(str,pointer_slots()))+'};\n'
    generated=prefix+'\n'.join(matrix_bodies)+'\n'+transform+'\n'.join(bodies)+extra_bodies+offsets+driver+extra_driver
    if source_transform is not None:generated=source_transform(generated)
    harness=output/'harness.c';harness.write_text(generated)
    includes=[ROOT/p for p in ['src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/carma2/platform','src/carma2/s3/include','src/brender/include']]
    msvc_root=Path(msvc_root).resolve();msvc=sorted((msvc_root/'VC/Tools/MSVC').iterdir())[-1];sdk=msvc_root/'Windows Kits/10';version=sorted(p.name for p in (sdk/'Include').iterdir())[-1]
    env=dict(os.environ);env['PATH']=str(msvc/'bin/Hostx64/x86')+os.pathsep+env['PATH']
    env['INCLUDE']=os.pathsep.join(map(str,[msvc/'include',*[sdk/f'Include/{version}/{p}' for p in ['ucrt','shared','um']]]))
    env['LIB']=os.pathsep.join(map(str,[msvc/'lib/x86',*[sdk/f'Lib/{version}/{p}/x86' for p in ['ucrt','um']]]))
    commands={'msvc':[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(p) for p in includes],str(harness),'/Fo'+str(output/'msvc.obj'),'/Fe'+str(output/'msvc.exe')],
        'mingw':[str(Path(gcc).resolve()),'-std=gnu89','-O2','-Wall','-Wextra','-Werror',*['-I'+str(p) for p in includes],str(harness),'-o',str(output/'mingw.exe')]}
    for compiler,command in commands.items():
        result=subprocess.run(command,cwd=output,env=env if compiler=='msvc' else None,capture_output=True,text=True)
        (output/(compiler+'-build.log')).write_text(result.stdout+result.stderr)
        if result.returncode:raise AssertionError(result.stdout+result.stderr)
    return commands

def main():
    parser=argparse.ArgumentParser(description=__doc__);sub=parser.add_subparsers(dest='command',required=True)
    exp=sub.add_parser('export-map');exp.add_argument('--build-directory',type=Path,required=True);exp.add_argument('--output',type=Path,required=True)
    check=sub.add_parser('verify')
    for name in ['original','recompiled','symbols','output']:check.add_argument('--'+name,type=Path,required=True)
    check.add_argument('--gcc',type=Path,default=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe')
    check.add_argument('--msvc-root',type=Path,default=ROOT/'build-tools/msvc-modern')
    args=parser.parse_args()
    if args.command=='export-map':
        from reccmp.project.detect import argparse_parse_project_target
        from reccmp.compare import Compare
        output=args.output.resolve();os.chdir(args.build_directory)
        target=argparse_parse_project_target(argparse.Namespace(target='CARMA2_HW'));original=target.original_path.read_bytes()
        if hashlib.sha256(original).hexdigest()!=ORIGINAL_SHA256:parser.error('Incorrect original hash')
        compare=Compare.from_target(target)
        mapping=dict(format=1,original_sha256=ORIGINAL_SHA256,recompiled_sha256=hashlib.sha256(target.recompiled_path.read_bytes()).hexdigest(),symbols=[dict(name=e.name,orig=e.orig_addr,recomp=e.recomp_addr) for e in compare.get_all() if e.orig_addr and e.recomp_addr])
        output.parent.mkdir(parents=True,exist_ok=True);output.write_text(json.dumps(mapping,indent=2));print(f'Exported {len(mapping["symbols"])} symbols');return
    original=args.original.read_bytes();rebuilt=args.recompiled.read_bytes();mapping=json.loads(args.symbols.read_text())
    if hashlib.sha256(original).hexdigest()!=ORIGINAL_SHA256:parser.error('Incorrect original hash')
    if mapping.get('format')!=1 or mapping.get('original_sha256')!=ORIGINAL_SHA256 or mapping.get('recompiled_sha256')!=hashlib.sha256(rebuilt).hexdigest():parser.error('Stale/invalid symbol map')
    symbols={s['name']:s for s in mapping['symbols']}
    for name,address in ADDRESSES.items():
        if symbols[name]['orig']!=address:parser.error('Unexpected original address: '+name)
    addresses={n:symbols[n]['recomp'] for n in ADDRESSES}
    output=args.output.resolve();output.mkdir(parents=True,exist_ok=True)
    commands=compile_native(output,args.gcc,args.msvc_root)
    cases=case_options();results=[]
    for index,options in enumerate(cases):
        case=make_case(options);expected=Fixture(original,ADDRESSES,case).execute();actual=Fixture(rebuilt,addresses,case).execute()
        checks=[('recompiled-x86',actual)]
        if not expected['fault']:
            path=output/'case.bin';path.write_bytes(words([case['mode'],case['callback_return'],case['mutate'],*case['globals'],0xffffffff if options.get('no_output_actor') else options.get('t_bits',bits(0.5))])+case['data'])
            for compiler in commands:checks.append((compiler,dict(events=subprocess.check_output([str(output/(compiler+'.exe')),str(path)]),fault=None)))
        for compiler,actual in checks:
            if actual!=expected:
                (output/f'failure-{index}-{compiler}.json').write_text(json.dumps(dict(options=options,original={k:v.hex() if isinstance(v,bytes) else v for k,v in expected.items()},actual={k:v.hex() if isinstance(v,bytes) else v for k,v in actual.items()}),indent=2))
                raise AssertionError(f'Case {index}, {options}, {compiler} differs; see {output}')
            results.append(dict(case=options,compiler=compiler,events_sha256=hashlib.sha256(actual['events']).hexdigest(),fault=actual['fault']))
    (output/'verification.json').write_text(json.dumps(dict(original_sha256=ORIGINAL_SHA256,recompiled_sha256=hashlib.sha256(rebuilt).hexdigest(),compiler_commands=commands,cases=results),indent=2))
    print(f'PASS: {len(cases)} geometry scenarios, {len(results)} complete x86/native comparisons at PC53')

def case_options():
    result=[]
    for mode in range(7):result.append(dict(mode=mode))
    for axis in range(3):
        for sign in [-1,1]:
            for point in [[.25,.25],[0,0],[1,0],[0,1],[.5,.5],[-.001,.25],[.75,.75]]:
                axes=[j for j in range(3) if j!=axis];rp=[0,0,0];rp[axis]=1;rp[axes[0]],rp[axes[1]]=point
                rd=[0,0,0];rd[axis]=-2
                result.append(dict(mode=1,axis=axis,sign=sign,rp=rp,rd=rd))
    for identifier in ['!','>','?','#','@','normal','']:
        for pling in [0,1]:result.append(dict(mode=1,identifier=identifier,pling=pling))
    for flags in [0,0x800,0x1000,0x1800]:
        for sign in [-1,1]:result.append(dict(mode=1,flags=flags,sign=sign))
    for mode in [1,4,5,6]:
        for option in [dict(null_material=True),dict(faces=0),dict(no_groups=True),dict(vertices=[[0,0],[0,0],[0,0]]),dict(planes=[0,.25]),dict(planes=[.25,0]),dict(rp=[5,5,1])]:result.append(dict(mode=mode,**option))
    for mode in [1,2]:
        for value in [1,-7]:result.append(dict(mode=mode,callback_return=value))
        for mask in [1,2,4,8,15]:result.append(dict(mode=mode,mutate=mask))
    for mode in [2,5,6]:
        for option in [dict(empty=True),dict(no_model=True),dict(none=True),dict(nested=True),dict(bounds_actor=5),dict(bounds_actor=6,min=[3,3,3],max=[4,4,4]),dict(transform=True),dict(transform=True,transform_type=1),dict(transform=True,transform_type=5),dict(transform=True,transform_type=6)]:result.append(dict(mode=mode,**option))
    result.append(dict(mode=5,no_output_actor=True))
    for near in [0,.125,1,float('inf')]:result.append(dict(mode=2,nearest=near))
    result.append(dict(mode=2,nearest_bits=0x7fc12345))
    for a in [0,0x80000000,bits(-1),bits(.5),bits(1),0x7f800000,0xff800000,0x7fc12345]:
        for b in [bits(.5),0x7fc67890]:result.append(dict(mode=3,t_bits=a,nearest_bits=b))
    for direction in [0,2.3841858e-7,-2.3841858e-7,1e-8,-1e-8,3,-3,1e20,-1e20]:
        for position in [-2,0,2]:result.append(dict(mode=0,rp=[position,0,0],rd=[direction,0,0]))
    for value in [0x7f800000,0xff800000,0x7fc12345,1,0x80000001,0x7f7fffff]:
        for field in ['rp_bits','rd_bits']:result.append(dict(mode=0,**{field:[value,0,0]}))
        result.append(dict(mode=1,rd_bits=[0,0,value]))
    result.append(dict(mode=1,null_slot=True))
    for near,far in [(1,0),(-1,2),(.500005,.5),(.50002,.5),(.5,.499995),(.5,.49998)]:
        for mode in [0,1]:result.append(dict(mode=mode,near=near,far=far))
    for normal in [[1,.125,-.25],[.25,1,-.125],[.125,-.25,1]]:
        axis=max(range(3),key=lambda k:abs(normal[k]));axes=[k for k in range(3) if k!=axis]
        vertices=[]
        for xy in [[-1,-1],[2,-1],[-1,2]]:
            p=[0,0,0];p[axes[0]],p[axes[1]]=xy;p[axis]=-sum(p[k]*normal[k] for k in axes)/normal[axis];vertices.append(p)
        p=[0,0,0];rp=[2*n for n in normal];rd=[-4*n for n in normal]
        for mode in [1,4,5,6]:result.append(dict(mode=mode,normal=normal,vertices3d=vertices,rp=rp,rd=rd))
    rng=random.Random(0x45cf60)
    for i in range(120):
        x,y=rng.uniform(-32,32),rng.uniform(-32,32);sx,sy=rng.uniform(.125,16),rng.uniform(.125,16)
        a,b=rng.uniform(.01,.4),rng.uniform(.01,.4)
        options=dict(mode=1,vertices=[[x,y],[x+sx,y],[x,y+sy]],rp=[x+a*sx,y+b*sy,rng.uniform(.1,8)],rd=[0,0,-rng.uniform(8,32)],maps=[[rng.uniform(-1e6,1e6),rng.uniform(-1e6,1e6)] for j in range(3)])
        result.append(options)
    for i in range(160):
        x,y=rng.uniform(-128,128),rng.uniform(-128,128)
        sx,sy=rng.uniform(.125,32),rng.uniform(.125,32)
        shear_x,shear_y=rng.uniform(-16,16),rng.uniform(-16,16)
        a,b=rng.uniform(.01,.45),rng.uniform(.01,.45)
        result.append(dict(mode=1,vertices=[[x,y],[x+sx,y+shear_y],[x+shear_x,y+sy]],
            rp=[x+a*sx+b*shear_x,y+a*shear_y+b*sy,rng.uniform(.1,8)],rd=[0,0,-rng.uniform(8,32)],
            maps=[[rng.uniform(-1e6,1e6),rng.uniform(-1e6,1e6)] for j in range(3)]))
    for mode in [0,1]:
        for rd in [2.3841861e-7,-2.3841861e-7,2.3841856e-7,-2.3841856e-7]:
            result.append(dict(mode=mode,rp=[1e30,0,0],rd=[rd,0,0]))
    for direction in [0,3,-3]:
        for field in ['near','far']:
            result.append(dict(mode=0,rp=[0,0,0],rd=[direction,0,0],**{field:float('nan')}))
        for field in ['min_bits','max_bits']:
            result.append(dict(mode=0,rp=[0,0,0],rd=[direction,0,0],**{field:[0x7fc12345,bits(-1 if field=='min_bits' else 1),bits(-1 if field=='min_bits' else 1)]}))
    result.append(dict(mode=0,rp=[0,0,0],rd_bits=[0x7fc12345,0,0],near=float('-inf')))
    return result

if __name__=='__main__':main()
