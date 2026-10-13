#!/usr/bin/env python3
"""Compare actual tinted runtime C with original x86 at explicit API boundaries.

Compiles production bodies and repository 32-bit types under modern MSVC and
MinGW. Time, random integers, keys, pixel I/O, GPU updates and file probes are
controlled fixtures. Complete overlay/object/image/palette bytes and ordered
dependency arguments are compared. No renderer/raycast/RNG implementation is
emulated by a replacement production body.

Normal floating precision is proved by executing original CRT initializer
0x57a2d0: _controlfp(0x10000,0x30000) selects PC53. Direct original call sites to
_controlfp/_control87 are scanned; the only call is this initializer plus the
controlfp wrapper. This verifier uses PC53, not the reset-state PC64 used by
some instruction fixtures. Original _ftol remains real executable x86.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import struct
import subprocess
import sys

from verify_tinted_native_reconstruction import extract

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "src/carma2/newcommon/66-tintedpoly.c"
ORIGINAL_HASH = "9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d"
NAMES = ["UpdateTintedPolyByMaterial", "UpdatePolyVertexColours", "UpdatePulsePoly", "UpdateFirePoly", "ProcessTintedPoly", "UpdateTintedPolys"]
ENTRIES = [0x4d7d80,0x4d8050,0x4d8630,0x4d86e0,0x4d84a0,0x4d8a20]
FIELDS = "mode index visible used cls subclass shift time last red grn blu target_red target_grn target_blu frame phase cooling save keys rng occupied map volume replay vertices palette_null repeat sequence seed".split()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--original",type=Path,default=ROOT/"build-reference/originals/CARMA2_HW.EXE")
    parser.add_argument("--dependency-dir",type=Path,default=ROOT/"build-audit/joystick/python-deps")
    parser.add_argument("--gcc",type=Path,default=ROOT/"build-tools/winlibs-i686/mingw32/bin/gcc.exe")
    parser.add_argument("--msvc-root",type=Path,default=ROOT/"build-tools/msvc-modern")
    parser.add_argument("--objdump",type=Path,default=ROOT/"build-tools/winlibs-i686/mingw32/bin/objdump.exe")
    parser.add_argument("--output",type=Path,default=ROOT/"build-audit/tinted-runtime-tests")
    parser.add_argument("--case-index",type=int,action="append",help="Run only these zero-based cases for diagnosis")
    args=parser.parse_args()
    sys.path.insert(0,str(args.dependency_dir.resolve()))
    from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
    from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_EAX,UC_X86_REG_ECX,UC_X86_REG_EDX,UC_X86_REG_FPCW
    binary=args.original.read_bytes()
    if hashlib.sha256(binary).hexdigest()!=ORIGINAL_HASH:raise SystemExit("Original executable hash differs")
    out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)
    disassembly=subprocess.check_output([str(args.objdump),"-d","-M","intel","--start-address=0x401000","--stop-address=0x589000",str(args.original)],text=True)
    control_calls=[(int(a,16),int(b,16)) for a,b in re.findall(r"^\s*([0-9a-f]+):[^\n]*\bcall\s+0x(583720|5836e0)\b",disassembly,re.M)]
    assert control_calls==[(0x57a2da,0x583720),(0x58372f,0x5836e0)],control_calls
    source=SOURCE.read_text()
    driver=r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <float.h>
#include "66-tintedpoly.h"
#include "08-loading1.h"
#include "18-graphics2.h"
#include "41-utility.h"
#include "42-input.h"
#include "globvars.h"
#include "globvrkm.h"
#include "rec2_macros.h"
typedef char check_layout[sizeof(void*)==4&&sizeof(tTintedPoly)==0x6450&&sizeof(br_actor)==100&&sizeof(br_model)==92&&sizeof(br_material)==156&&sizeof(br_vertex)==40&&sizeof(br_pixelmap)==68&&offsetof(tCar_spec,collision_info)==8&&offsetof(tPhysics_object,last_special_volume)==0x258&&offsetof(tSpecial_volume,gravity_multiplier)==0x88?1:-1];
enum {MODE,INDEX,VISIBLE,USED,CLS,SUBCLASS,SHIFT,TIME,LAST,RED,GRN,BLU,TARGET_RED,TARGET_GRN,TARGET_BLU,FRAME,PHASE,COOLING,SAVE,KEYS,RNG,OCCUPIED,MAP,VOLUME,REPLAY,VERTICES,PALETTE_NULL,REPEAT,SEQUENCE,SEED};
static unsigned a[30];
typedef struct {tCar_spec car;tPhysics_object collision;tSpecial_volume volume;br_actor actors[10];br_material materials[10],refs[10];br_model models[10];br_vertex vertices[10][32];br_pixelmap maps[10],palette;br_colour original_palette[256],output_palette[256],fire_pixels[2048];} fixture_heap;
static fixture_heap heap;
br_pixelmap* gRender_palette;
tCar_spec* gCar_to_view;
int gAction_replay_mode;
tPath_name gApplication_path="C:/DATA/";
static unsigned events[32768][9],event_count,time_calls,rng_calls,open_calls;
static char texts[32768][80];
static unsigned norm(unsigned value) {unsigned base=(unsigned)(uintptr_t)&heap;return value>=base&&value<base+sizeof(heap)?value-base+0x1000000:value;}
static void record(unsigned code,unsigned p,unsigned q,unsigned r,unsigned s,const char* text) {
    unsigned* e=events[event_count];if(event_count>=32768)abort();
    e[0]=code;e[1]=p;e[2]=q;e[3]=r;e[4]=s;e[5]=gTinted_fire_frame;e[6]=gTinted_fire_phase;e[7]=gTinted_fire_cooling;e[8]=gTinted_fire_save;
    if(text)strcpy(texts[event_count],text);
    event_count++;
}
void C2_HOOK_CDECL BrMaterialUpdate(br_material* p,br_uint_16 flags) {record(1,norm((unsigned)(uintptr_t)p),flags,p->colour,0,NULL);}
void C2_HOOK_CDECL BrModelUpdate(br_model* p,br_uint_16 flags) {record(2,norm((unsigned)(uintptr_t)p),flags,p->nvertices,0,NULL);}
tU32 C2_HOOK_FASTCALL GetTotalTime(void) {unsigned value=a[TIME]+4096*time_calls;record(3,value,time_calls++,0,0,NULL);return value;}
br_colour C2_HOOK_CDECL BrPixelmapPixelGet(br_pixelmap* p,br_int_32 x,br_int_32 y) {
    unsigned i=(unsigned)(p-heap.maps);br_colour colour=0xff000000u|(((unsigned)x*17+i*7)&255)<<16|(((unsigned)y*19+i*11)&255)<<8|(((unsigned)x*3+(unsigned)y*5+i*13)&255);
    record(4,norm((unsigned)(uintptr_t)p),x,y,colour,NULL);return colour;
}
int C2_HOOK_FASTCALL PDKeyDown(int key) {int value=key==32?(a[KEYS]&1)!=0:(a[KEYS]&2)!=0;record(5,key,value,0,0,NULL);return value;}
int C2_HOOK_FASTCALL IRandomBetween(int low,int high) {
    int result=a[RNG]==0?low:a[RNG]==1?high:low+(int)((rng_calls*17+3)%(unsigned)(high-low+1));
    record(6,low,high,result,rng_calls++,NULL);return result;
}
void C2_HOOK_CDECL BrPixelmapPixelSet(br_pixelmap* p,br_int_32 x,br_int_32 y,br_uint_32 colour) {
    if(x<0||x>=32||y<0||y>=64)abort();
    heap.fire_pixels[y*32+x]=colour;record(7,norm((unsigned)(uintptr_t)p),x,y,colour,NULL);
}
void C2_HOOK_CDECL BrMapUpdate(br_pixelmap* p,br_uint_16 flags) {record(8,norm((unsigned)(uintptr_t)p),flags,0,0,NULL);}
br_uint_32 C2_HOOK_CDECL BrPixelmapSave(const char* path,br_pixelmap* p) {record(9,norm((unsigned)(uintptr_t)p),0,0,0,path);return 1;}
FILE* C2_HOOK_FASTCALL DRfopen(const char* path,const char* mode) {
    unsigned pointer;if(strcmp(mode,"rt"))abort();pointer=open_calls<a[OCCUPIED]?0x2000000+4*open_calls:0;open_calls++;record(10,pointer,0,0,0,path);return (FILE*)(uintptr_t)pointer;
}
static int C2_HOOK_CDECL fixture_close(FILE* p) {record(11,(unsigned)(uintptr_t)p,0,0,0,NULL);return 0;}
void C2_HOOK_FASTCALL PathCat(char* dest,const char* base,const char* leaf) {
    if(base!=gApplication_path||strcmp(leaf,"FLM"))abort();
    strcpy(dest,"C:/DATA/FLM/");record(12,0,0,0,0,dest);
}
#define fclose fixture_close
'''
    driver+=source[source.index("// GLOBAL:"):source.index("// FUNCTION:")]
    driver+="\n".join(extract(source,name) for name in NAMES)
    driver+=r'''
#undef fclose
static void normalize(void* p,unsigned offset) {unsigned value;memcpy(&value,(char*)p+offset,4);value=norm(value);memcpy((char*)p+offset,&value,4);}
static void dump(const void* p,unsigned size) {unsigned i;for(i=0;i<size;i++)printf("%02x",((const unsigned char*)p)[i]);}
int main(int argc,char**argv) {
    unsigned i,j,cls,sub,gravity;
    static const unsigned classes[10]={6,6,3,6,2,6,0,6,4,6};
    static const unsigned subclasses[10]={1,0,1,2,0,1,0,0,0,2};
    static const unsigned gravities[7]={0x3f800000,0x3f800000,0x3f000000,0x7fc00000,0xffc00000,0x7f800001,0xff800001};
    if(argc==2){printf("[%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u]\n",(unsigned)offsetof(fixture_heap,car),(unsigned)offsetof(fixture_heap,collision),(unsigned)offsetof(fixture_heap,volume),(unsigned)offsetof(fixture_heap,actors),(unsigned)offsetof(fixture_heap,materials),(unsigned)offsetof(fixture_heap,refs),(unsigned)offsetof(fixture_heap,models),(unsigned)offsetof(fixture_heap,vertices),(unsigned)offsetof(fixture_heap,maps),(unsigned)offsetof(fixture_heap,palette),(unsigned)offsetof(fixture_heap,original_palette),(unsigned)offsetof(fixture_heap,output_palette),(unsigned)sizeof(heap));return 0;}
    for(i=0;i<30;i++)a[i]=(unsigned)strtoul(argv[i+1],NULL,0);
    _controlfp(_PC_53,_MCW_PC);
    memset(&heap,(int)a[SEED],sizeof(heap));memset(gTintedPolys,(int)a[SEED],sizeof(gTintedPolys));
    heap.car.collision_info=&heap.collision;heap.collision.last_special_volume=a[VOLUME]?&heap.volume:NULL;
    gravity=gravities[a[VOLUME]];memcpy(&heap.volume.gravity_multiplier,&gravity,4);
    gCar_to_view=&heap.car;gAction_replay_mode=(int)a[REPLAY];
    gTinted_poly_palette=a[PALETTE_NULL]?NULL:heap.original_palette;heap.palette.pixels=heap.output_palette;gRender_palette=&heap.palette;
    gTintedColourMap_red=a[TARGET_RED];gTintedColourMap_grn=a[TARGET_GRN];gTintedColourMap_blu=a[TARGET_BLU];
    gTinted_fire_frame=(int)a[FRAME];gTinted_fire_phase=(int)a[PHASE];gTinted_fire_cooling=(int)a[COOLING];gTinted_fire_save=(int)a[SAVE];
    for(i=0;i<256;i++)heap.original_palette[i]=i%17==0?0:BR_COLOUR_RGB((i*37)&255,(i*29)&255,i);
    for(i=0;i<10;i++){
        cls=a[SEQUENCE]==1?classes[i]:a[SEQUENCE]==2?2:a[SEQUENCE]==3?6:a[CLS];
        sub=a[SEQUENCE]==1?subclasses[i]:a[SEQUENCE]==3?1:a[SUBCLASS];
        gTintedPolys[i].used=a[SEQUENCE]?(int)a[USED]:i==a[INDEX]?(int)a[USED]:0;gTintedPolys[i].visible=(int)a[VISIBLE];gTintedPolys[i].class=cls;gTintedPolys[i].subClass=sub;gTintedPolys[i].colour=a[SHIFT];
        gTintedPolys[i].actor=&heap.actors[i];gTintedPolys[i].material=&heap.materials[i];gTintedPolys[i].model=&heap.models[i];gTintedPolys[i].material2=a[MAP]==3?NULL:&heap.refs[i];gTintedPolys[i].pixelmap=&heap.maps[i];
        gTintedPolys[i].color_red=a[RED];gTintedPolys[i].color_grn=a[GRN];gTintedPolys[i].color_blu=a[BLU];gTintedPolys[i].lastTime=(int)a[LAST];
        gTintedPolys[i].color2_red=(i+1)*33;gTintedPolys[i].color2_grn=(i+1)*71;gTintedPolys[i].color2_blu=(i+1)*119;
        heap.materials[i].colour_map=&heap.maps[(i+1)%10];heap.refs[i].colour_map=a[MAP]==1?NULL:&heap.maps[i];heap.maps[i].pixels=a[MAP]==2?NULL:heap.fire_pixels;heap.maps[i].width=16;heap.maps[i].height=12;
        heap.models[i].vertices=heap.vertices[i];heap.models[i].nvertices=(br_uint_16)a[VERTICES];heap.actors[i].render_style=(br_uint_8)(a[SEED]==53?BR_RSTYLE_NONE:BR_RSTYLE_FACES);
        for(j=0;j<2048;j++){gTintedPolys[i].tints1[j]=(j*3)%9;gTintedPolys[i].tints2[j]=(j*5)%100;gTintedPolys[i].tints3[j]=(j*7)%100;}
        for(j=0;j<256;j++)gTintedPolys[i].colours[j]=0xff000000u|(j*0x10203u);
    }
    for(j=0;j<a[REPEAT];j++)switch(a[MODE]){
    case 0:UpdateTintedPolyByMaterial((int)a[INDEX]);break;case 1:UpdatePolyVertexColours((int)a[INDEX]);break;
    case 2:UpdatePulsePoly((int)a[INDEX]);break;case 3:UpdateFirePoly((int)a[INDEX]);break;
    case 4:ProcessTintedPoly((int)a[INDEX]);break;case 5:UpdateTintedPolys();break;
    }
    for(i=0;i<10;i++){
        for(j=0;j<3;j++)normalize(&gTintedPolys[i],j*4);
        normalize(&gTintedPolys[i],0x2c);normalize(&gTintedPolys[i],0x44);
        normalize(&heap.materials[i],0x40);normalize(&heap.refs[i],0x40);normalize(&heap.models[i],8);normalize(&heap.maps[i],8);
    }
    normalize(&heap.car,8);normalize(&heap.collision,0x258);normalize(&heap.palette,8);
    printf("{\"pool\":\"");dump(gTintedPolys,sizeof(gTintedPolys));printf("\",\"heap\":\"");dump(&heap,sizeof(heap));printf("\",\"globals\":[%u,%u,%u,%u,%u,%u,%u],\"events\":[",gTintedColourMap_red,gTintedColourMap_grn,gTintedColourMap_blu,(unsigned)gTinted_fire_frame,(unsigned)gTinted_fire_phase,(unsigned)gTinted_fire_cooling,(unsigned)gTinted_fire_save);
    for(i=0;i<event_count;i++){printf("%s[[",i?",":"");for(j=0;j<9;j++)printf("%s%u",j?",":"",events[i][j]);printf("],\"%s\"]",texts[i]);}
    printf("]}\n");return 0;
}
'''
    harness=out/"harness.c";harness.write_text(driver)
    includes=[ROOT/path for path in ("src/carma2/newcommon","src/carma2/include","src/carma2/hooks","src/carma2/platform","src/carma2/s3/include","src/brender/include")]
    msvc=sorted((args.msvc_root/"VC/Tools/MSVC").iterdir())[-1];sdk=args.msvc_root/"Windows Kits/10";version=sorted((sdk/"Include").iterdir())[-1].name
    env=dict(os.environ);env["PATH"]=str(msvc/"bin/Hostx64/x86")+os.pathsep+str(sdk/f"bin/{version}/x64")+os.pathsep+env["PATH"]
    env["INCLUDE"]=os.pathsep.join(map(str,[msvc/"include",*[sdk/f"Include/{version}/{part}" for part in ("ucrt","shared","um")]]));env["LIB"]=os.pathsep.join(map(str,[msvc/"lib/x86",*[sdk/f"Lib/{version}/{part}/x86" for part in ("ucrt","um")]]))
    commands={"msvc":[str(msvc/"bin/Hostx64/x86/cl.exe"),"/nologo","/O2","/W3","/WX","/D_CRT_SECURE_NO_WARNINGS",*["/I"+str(p) for p in includes],str(harness),"/Fo"+str(out/"msvc.obj"),"/Fe"+str(out/"msvc.exe")],
              "mingw":[str(args.gcc),"-std=gnu89","-O2","-Wall","-Wextra","-Werror",*["-I"+str(p) for p in includes],str(harness),"-o",str(out/"mingw.exe")]}
    schemas=[]
    for compiler,command in commands.items():
        result=subprocess.run(command,env=env if compiler=="msvc" else None,text=True,capture_output=True);(out/f"{compiler}-build.txt").write_text(result.stdout+result.stderr)
        if result.returncode:raise RuntimeError(result.stdout+result.stderr)
        schemas.append(json.loads(subprocess.check_output([str(out/f"{compiler}.exe"),"schema"],text=True)))
    assert schemas[0]==schemas[1]
    car,collision,volume,actors,materials,refs,models,vertices,maps,palette,original_palette,output_palette,heap_size=schemas[0]
    fire_pixels=output_palette+1024
    pe=struct.unpack_from("<I",binary,60)[0];sections=struct.unpack_from("<H",binary,pe+6)[0];table=pe+24+struct.unpack_from("<H",binary,pe+20)[0]
    def image():
        u=Uc(UC_ARCH_X86,UC_MODE_32);u.mem_map(0x400000,0x400000);u.mem_map(0x1000000,0x80000)
        for i in range(sections):
            _,rva,size,offset=struct.unpack_from("<IIII",binary,table+i*40+8);u.mem_write(0x400000+rva,binary[offset:offset+size])
        return u
    test=image();test.mem_write(0x107e000,struct.pack("<I",0x107f000));test.reg_write(UC_X86_REG_ESP,0x107e000);test.reg_write(UC_X86_REG_FPCW,0x37f);test.emu_start(0x57a2d0,0x107f000,count=10000)
    control_word=test.reg_read(UC_X86_REG_FPCW);assert control_word&0x300==0x200
    def original(c):
        u=image();base=0x1000000;pool=0x705c80;index=c["index"]
        def wr(address,value):u.mem_write(address,struct.pack("<I",value&0xffffffff))
        def rd(address):return struct.unpack("<I",u.mem_read(address,4))[0]
        def signed(value):return value if value<0x80000000 else value-0x100000000
        def text(address):
            b=bytearray()
            while u.mem_read(address+len(b),1)[0]:b+=u.mem_read(address+len(b),1)
            return b.decode("ascii")
        u.mem_write(base,bytes([c["seed"]])*heap_size);u.mem_write(pool,bytes([c["seed"]])*(10*0x6450));u.mem_write(0x75ba60,b"C:/DATA/\0")
        wr(base+car+8,base+collision);wr(base+collision+0x258,base+volume if c["volume"] else 0);wr(base+volume+0x88,(0x3f800000,0x3f800000,0x3f000000,0x7fc00000,0xffc00000,0x7f800001,0xff800001)[c["volume"]])
        wr(0x655f54,base+car);wr(0x676914,c["replay"]);wr(0x6a0434,0 if c["palette_null"] else base+original_palette);wr(base+palette+8,base+output_palette);wr(0x74a674,base+palette)
        for addr,key in ((0x65e870,"target_red"),(0x6a0438,"target_grn"),(0x6a043c,"target_blu"),(0x6a0444,"frame"),(0x6a0440,"phase"),(0x6a044c,"cooling"),(0x6a0448,"save")):wr(addr,c[key])
        for i in range(256):wr(base+original_palette+i*4,0 if i%17==0 else ((i*37)&255)<<16|((i*29)&255)<<8|i)
        classes=[6,6,3,6,2,6,0,6,4,6];subclasses=[1,0,1,2,0,1,0,0,0,2]
        for i in range(10):
            p=pool+i*0x6450;cls=classes[i] if c["sequence"]==1 else 2 if c["sequence"]==2 else 6 if c["sequence"]==3 else c["cls"];sub=subclasses[i] if c["sequence"]==1 else 1 if c["sequence"]==3 else c["subclass"]
            for offset,value in ((0x30,c["used"] if c["sequence"] or i==index else 0),(0x34,c["visible"]),(0x38,cls),(0x24,sub),(0x40,c["shift"]),(0,base+actors+i*100),(4,base+materials+i*156),(8,base+models+i*92),(0x2c,0 if c["map"]==3 else base+refs+i*156),(0x44,base+maps+i*68),(0xc,c["red"]),(0x10,c["grn"]),(0x14,c["blu"]),(0x3c,c["last"]),(0x18,(i+1)*33),(0x1c,(i+1)*71),(0x20,(i+1)*119)):wr(p+offset,value)
            wr(base+materials+i*156+0x40,base+maps+((i+1)%10)*68);wr(base+refs+i*156+0x40,0 if c["map"]==1 else base+maps+i*68);wr(base+maps+i*68+8,0 if c["map"]==2 else base+fire_pixels);u.mem_write(base+maps+i*68+0x34,struct.pack("<HH",16,12));wr(base+models+i*92+8,base+vertices+i*32*40);u.mem_write(base+models+i*92+0x10,struct.pack("<H",c["vertices"]));u.mem_write(base+actors+i*100+0x20,bytes([1 if c["seed"]==53 else 4]))
            for j in range(2048):wr(p+0x448+j*4,(j*3)%9);wr(p+0x2448+j*4,(j*5)%100);wr(p+0x4448+j*4,(j*7)%100)
            for j in range(256):wr(p+0x48+j*4,0xff000000|j*0x10203)
        events=[];counters={"time":0,"rng":0,"open":0}
        def record(code,p=0,q=0,r=0,s=0,message=""):
            events.append([[code,p&0xffffffff,q&0xffffffff,r&0xffffffff,s&0xffffffff,*[rd(addr) for addr in (0x6a0444,0x6a0440,0x6a044c,0x6a0448)]],message])
        def hook(uc,address,size,ctx):
            sp=uc.reg_read(UC_X86_REG_ESP);ecx=uc.reg_read(UC_X86_REG_ECX);edx=uc.reg_read(UC_X86_REG_EDX);result=0;cleanup=0
            if address==0x520e70:record(1,rd(sp+4),rd(sp+8),rd(rd(sp+4)+8))
            elif address==0x51f950:record(2,rd(sp+4),rd(sp+8),struct.unpack("<H",uc.mem_read(rd(sp+4)+0x10,2))[0])
            elif address==0x514c30:result=(c["time"]+4096*counters["time"])&0xffffffff;record(3,result,counters["time"]);counters["time"]+=1
            elif address==0x538840:
                pointer,x,y=[rd(sp+4+i*4) for i in range(3)];i=(pointer-base-maps)//68;result=0xff000000|((x*17+i*7)&255)<<16|((y*19+i*11)&255)<<8|((x*3+y*5+i*13)&255);record(4,pointer,x,y,result)
            elif address==0x482550:result=int(bool(c["keys"]&(1 if ecx==32 else 2)));record(5,ecx,result)
            elif address==0x513520:
                low,high=signed(ecx),signed(edx);result=low if c["rng"]==0 else high if c["rng"]==1 else low+(counters["rng"]*17+3)%(high-low+1);record(6,low,high,result,counters["rng"]);counters["rng"]+=1
            elif address==0x538800:
                pointer,x,y,colour=[rd(sp+4+i*4) for i in range(4)];assert x<32 and y<64;wr(base+fire_pixels+4*(y*32+x),colour);record(7,pointer,x,y,colour)
            elif address==0x523160:record(8,rd(sp+4),rd(sp+8))
            elif address==0x5393a0:record(9,rd(sp+8),message=text(rd(sp+4)));result=1
            elif address==0x491170:
                assert text(edx)=="rt";result=0x2000000+4*counters["open"] if counters["open"]<c["occupied"] else 0;counters["open"]+=1;record(10,result,message=text(ecx))
            elif address==0x577640:record(11,rd(sp+4))
            elif address==0x513690:
                assert edx==0x75ba60 and text(rd(sp+4))=="FLM";uc.mem_write(ecx,b"C:/DATA/FLM/\0");record(12,message="C:/DATA/FLM/");cleanup=4
            elif address==0x575de0:
                dest,fmt=rd(sp+4),text(rd(sp+8))
                value=(f"{signed(rd(sp+12)):02d}."+text(rd(sp+16))) if fmt=="%02d.%s" else text(rd(sp+12)) if fmt=="%s" else None
                assert value is not None;uc.mem_write(dest,value.encode()+b"\0");result=len(value)
            else:return
            uc.reg_write(UC_X86_REG_EAX,result&0xffffffff);uc.reg_write(UC_X86_REG_EIP,rd(sp));uc.reg_write(UC_X86_REG_ESP,sp+4+cleanup)
        for entry in (0x520e70,0x51f950,0x514c30,0x538840,0x482550,0x513520,0x538800,0x523160,0x5393a0,0x491170,0x577640,0x513690,0x575de0):
            u.hook_add(UC_HOOK_CODE,hook,begin=entry,end=entry)
        for repeat in range(c["repeat"]):
            wr(0x107e000,0x107f000);u.reg_write(UC_X86_REG_ESP,0x107e000);u.reg_write(UC_X86_REG_ECX,index&0xffffffff);u.reg_write(UC_X86_REG_FPCW,control_word);u.emu_start(ENTRIES[c["mode"]],0x107f000,count=2000000);assert u.reg_read(UC_X86_REG_EIP)==0x107f000
        return {"pool":bytes(u.mem_read(pool,10*0x6450)).hex(),"heap":bytes(u.mem_read(base,heap_size)).hex(),"globals":[rd(addr) for addr in (0x65e870,0x6a0438,0x6a043c,0x6a0444,0x6a0440,0x6a044c,0x6a0448)],"events":events}
    def case(mode,**kw):
        c=dict(zip(FIELDS,[0]*len(FIELDS)));c.update(mode=mode,index=0,visible=1,used=1,cls=6,time=9000,last=0,target_red=128,target_grn=73,target_blu=29,volume=2,vertices=25,repeat=1,seed=165);c.update(kw);return c
    cases=[]
    for mode_map in (0,1,2,3):
        for rgb in ((0,0,0),(10,90,200),(128,73,29),(0xffffffff,1,0)):
            for time,last in ((4095,0),(4096,0),(0,8192),(0xffffffff,0x80000000)):
                cases.append(case(0,map=mode_map,red=rgb[0],grn=rgb[1],blu=rgb[2],time=time,last=last,repeat=2))
    cases += [case(0,visible=0),case(0,index=9,map=1,red=127,grn=74,blu=29)]
    for rgb in ((0,0,0),(255,0,0),(1,2,3),(128,64,32),(255,255,255),(0xffffffff,0,1)):
        cases.append(case(1,red=rgb[0],grn=rgb[1],blu=rgb[2]))
    cases += [case(1,index=-1),case(1,palette_null=1)]
    for shift in (0,1,31,32,63,0xffffffff):
        for count in (0,1,25,32):cases.append(case(2,shift=shift,vertices=count,time=0xffffffff))
    cases += [case(2,seed=53)]
    for frame in (0,1,10,20,149,150,151,199,200):
        for rng in (0,1,2):cases.append(case(3,frame=frame,rng=rng,phase=63,cooling=7,save=1,occupied=2,repeat=2))
    for cls in (0,2,3,4,5,6,7):
        for used in (0,1):cases.append(case(4,cls=cls,used=used,keys=0,frame=0))
    for sub in (0,1,2):
        for time in (0,254,255,256,511,512,0xffffffff):cases.append(case(4,subclass=sub,time=time,shift=32,index=9))
    for volume_case in range(7):
        for replay in (0,1):cases.append(case(4,cls=2,volume=volume_case,replay=replay,map=1))
    for keys in (0,1,2,3):
        for frame in (0,199,200):cases.append(case(4,cls=4,keys=keys,frame=frame,rng=1))
    cases += [case(5,sequence=seq,keys=0) for seq in (1,2,3)]
    cases += [case(5,sequence=2,volume=volume_case,replay=replay,map=1) for volume_case in range(7) for replay in (0,1)]
    if args.case_index is not None and any(index < 0 or index >= len(cases) for index in args.case_index):
        parser.error(f"--case-index must be between 0 and {len(cases)-1}")
    results=[]
    for index,c in enumerate(cases):
        if args.case_index is not None and index not in args.case_index:
            continue
        expected=original(c)
        for compiler in commands:
            actual=json.loads(subprocess.check_output([str(out/f"{compiler}.exe"),*[str(c[key]&0xffffffff) for key in FIELDS]],text=True))
            if actual!=expected:
                failure=out/f"failure-{compiler}-{index}.json";failure.write_text(json.dumps({"case":c,"original":expected,"native":actual},indent=2));raise AssertionError(f"Runtime {compiler} case {index} differs: {failure}")
            results.append({"compiler":compiler,"case_index":index,"case":c,"all_bytes_calls_match":True})
        if (index+1)%20==0:
            print(f"Verified {index+1}/{len(cases)} scenarios",flush=True)
    (out/"results.json").write_text(json.dumps(results,indent=2)+"\n")
    manifest={"original_sha256":ORIGINAL_HASH,"all_cases_passed":True,"case_count":len(results),"functions":dict(zip(NAMES,map(hex,ENTRIES))),"type_schema":schemas[0],"compiler_commands":commands,"runtime_precision":{"significand_bits":53,"original_initializer":"0x57a2d0","executed_control_word":hex(control_word),"direct_control_call_sites":[[hex(a),hex(b)] for a,b in control_calls]},"source_sha256":{str(path.relative_to(ROOT)):hashlib.sha256(path.read_bytes()).hexdigest() for path in (SOURCE,ROOT/"src/carma2/newcommon/66-tintedpoly.h",ROOT/"src/carma2/include/rec2_types.h")},"harness_sha256":hashlib.sha256(driver.encode()).hexdigest(),"limitations":"Controlled dependency boundaries; no raycast or renderer integration. Exhaustion of all9999 filename probes leaves original stack bytes unspecified and is not modeled as a successful filename."}
    (out/"verification.json").write_text(json.dumps(manifest,indent=2)+"\n");print(f"Passed {len(results)} native/original tinted runtime cases")


if __name__=="__main__":main()
