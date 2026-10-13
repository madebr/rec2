#!/usr/bin/env python3
"""Verify five cursor entries against original x86 with repository types.

Native MSVC5/modern MSVC/MinGW compile actual cursor bodies and actual tinted
resize/visibility/process/render bodies. Class 1 overlays are exercised;
other tinted update classes abort at their declared unexercised boundaries.
Map lookup/overlay creation, input/time, material/rotation/pixelmap/GPU APIs
are controlled boundaries. Complete final objects/overlay pool and ordered
callback snapshots are compared. Free callbacks poison local objects without
releasing storage, so this tests disposal decisions rather than allocator bugs.
Cursor bodies contain no variable scalar load/store conversion; their only
scalar assignment is the finite material ambient constant 1.0f. The inherited
tinted resize body generates UVs from integer operands at normal x87 PC53.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import struct
import subprocess
import sys

from verify_tinted_native_reconstruction import extract

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "src/carma2/newcommon/18-graphics2.c"
TINTED = ROOT / "src/carma2/newcommon/66-tintedpoly.c"
HASH = "9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d"
FUNCTIONS = dict(RemoveTransientBitmaps=0x43e010, DoMouseCursor=0x43e0a0, AllocateCursorActors=0x43e3f0, StartMouseCursor=0x43e6c0, EndMouseCursor=0x43e710)
SCALARS = dict(gCursor_tinted_top=0x58fdc8, gCursor_tinted_left=0x58fdd0, gCursor_tinted_bottom=0x58fdcc, gCursor_tinted_right=0x58fdd4, gCursor_tinted_center=0x58fdd8,
               gCursor_line_width=0x58fde0, gTransient_bitmap_index=0x67be94, gMouse_last_pos=0x79ecb0, mouse_y=0x79ecb4, gMouse_in_use=0x79ecb8,
               gCurrent_cursor_index=0x67be90, gMouse_started=0x67c390, gNext_transient=0x67c348, gCursor_last_call_time=0x67c398,
               gCursor_last_required_change=0x67c39c, gCursor_delta_x=0x67c3a0, gCursor_required_cursor=0x67c3a4, gCursor_zero_count=0x67c3a8, gCursor_button_was_down=0x67c3ac,
               gDraw_cursor=0x58fddc, gNoTransients=0x74ca28)
POINTERS = dict(gBack_screen=0x74d360, gCurrent_graf_data=0x74b774, gTintedPolyCamera=0x6a0430, gDepth_buffer=0x68b8a4)
BOUNDARIES = dict(PDGetTotalTime=0x51d410, GetMousePosition=0x483c10, EitherMouseButtonDown=0x4824c0, BrMapFind=0x51eff0, CreateTintedPoly=0x4d70c0,
                  BrMaterialUpdate=0x520e70, BrMatrix23Rotate=0x534bc0, BrPixelmapRectangleCopy=0x538590, BrPixelmapFree=0x537dc0, PossibleUnlock=0x516c30,
                  SetTintedPolySize=0x4d8bb0, TurnTintedPolyOn=0x4d8220, ProcessTintedPoly=0x4d84a0, RenderTintedPolys=0x4d8290, TurnTintedPolyOff=0x4d8250,
                  BrModelUpdate=0x51f950, BrZbsSceneRender=0x522f30)
ADDRESSES = dict(FUNCTIONS, **SCALARS, **POINTERS, **BOUNDARIES, gTransient_bitmaps=0x67be98, gTintedPolys=0x705c80)
FIELDS = "mode graphical no_transients next_transient transient_index mouse_in_use mouse_started current_cursor last_x last_y last_time last_change delta_x required_cursor zero_count button_was draw_cursor top left bottom right center line_width width height mouse_x mouse_y button time_base time_step maps allocate_alias copy_mode mutate repeat seed wide tint_used".split()
HEAP, STACK, STOP = 0x1000000, 0x1080000, 0x1081000


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--original", type=Path, default=ROOT / "build-reference/originals/CARMA2_HW.EXE")
    parser.add_argument("--dependency-dir", type=Path, default=ROOT / "build-audit/joystick/python-deps")
    parser.add_argument("--gcc", type=Path, default=ROOT / "build-tools/winlibs-i686/mingw32/bin/gcc.exe")
    parser.add_argument("--msvc-root", type=Path, default=ROOT / "build-tools/msvc-modern")
    parser.add_argument("--msvc5-root", type=Path, default=ROOT / "build-tools/msvc500sp3")
    parser.add_argument("--output", type=Path, default=ROOT / "build-audit/cursor-tests")
    parser.add_argument("--symbols", type=Path)
    parser.add_argument("--recompiled", type=Path, default=ROOT / "build-matching/rec2.exe")
    parser.add_argument("--case-index", type=int, action="append")
    args = parser.parse_args()
    sys.path.insert(0, str(args.dependency_dir.resolve()))
    from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
    from unicorn.x86_const import UC_X86_REG_EIP, UC_X86_REG_ESP, UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX, UC_X86_REG_FPCW
    binary = args.original.read_bytes()
    if hashlib.sha256(binary).hexdigest() != HASH:
        raise SystemExit("Original executable hash differs")
    images = [(binary, ADDRESSES)]
    if args.symbols:
        mapping = json.loads(args.symbols.read_text());rebuilt = args.recompiled.read_bytes()
        if mapping["format"] != 1 or mapping["original_sha256"] != HASH or mapping["recompiled_sha256"] != hashlib.sha256(rebuilt).hexdigest():
            raise SystemExit("Stale or incompatible cursor symbol map")
        mapped = {item["orig"]: item["recomp"] for item in mapping["symbols"]}
        mapped[SCALARS["mouse_y"]] = mapped[SCALARS["gMouse_last_pos"]] + 4
        images.append((rebuilt, {name: mapped[address] for name, address in ADDRESSES.items()}))
    out = args.output.resolve();out.mkdir(parents=True, exist_ok=True)
    source = SOURCE.read_text();tinted = TINTED.read_text()
    driver = r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "18-graphics2.h"
#include "25-grafdata.h"
#include "42-input.h"
#include "41-utility.h"
#include "66-tintedpoly.h"
#include "globvars.h"
#include "globvrkm.h"
#include "platform.h"
#include "rec2_macros.h"
typedef char layout[sizeof(void*)==4&&sizeof(tTintedPoly)==0x6450&&sizeof(tTransient_bm)==24&&sizeof(br_material)==156&&sizeof(br_model)==92&&sizeof(br_actor)==100&&sizeof(br_pixelmap)==68&&sizeof(br_vertex)==40&&offsetof(tGraf_data,width)==0&&offsetof(tGraf_data,height)==4?1:-1];
enum {MODE,GRAPHICAL,NO_TRANSIENTS,NEXT_TRANSIENT,TRANSIENT_INDEX,MOUSE_IN_USE,MOUSE_STARTED,CURRENT_CURSOR,LAST_X,LAST_Y,LAST_TIME,LAST_CHANGE,DELTA_X,REQUIRED_CURSOR,ZERO_COUNT,BUTTON_WAS,DRAW_CURSOR,TOP,LEFT,BOTTOM,RIGHT,CENTER,LINE_WIDTH,WIDTH,HEIGHT,MOUSE_X,MOUSE_Y,BUTTON,TIME_BASE,TIME_STEP,MAPS,ALLOCATE_ALIAS,COPY_MODE,MUTATE,REPEAT,SEED,WIDE,TINT_USED};
static unsigned a[38],time_calls,mouse_calls,lookup_calls,create_calls,copy_calls,material_calls;
typedef struct {br_pixelmap maps[53];br_material materials[11];br_actor actors[10];br_model models[10];br_vertex vertices[10][4];tGraf_data graf;br_actor camera;br_pixelmap back,depth;unsigned pixels[53][8];} fixture_heap;
static fixture_heap heap;
tTintedPoly gTintedPolys[10];br_actor* gTintedPolyCamera;br_pixelmap* gBack_screen;br_pixelmap* gDepth_buffer;tGraf_data* gCurrent_graf_data;
int gTinted_fire_frame,gTinted_fire_save,gMouse_in_use,gAction_replay_mode;tCar_spec* gCar_to_view;
'''
    driver += source[source.index("// GLOBAL:"):source.index("// FUNCTION:")]
    driver += r'''
static unsigned C2_HOOK_CDECL norm(unsigned value) {unsigned base=(unsigned)(uintptr_t)&heap;return value>=base&&value<base+sizeof(heap)?value-base+0x1000000:value;}
static void C2_HOOK_CDECL snapshot(void) {
    unsigned scalars[25],i,j,v;unsigned char table[sizeof(gTransient_bitmaps)],copy[sizeof(heap)],headers[10][72];
    scalars[0]=gCursor_tinted_top;scalars[1]=gCursor_tinted_left;scalars[2]=gCursor_tinted_bottom;scalars[3]=gCursor_tinted_right;scalars[4]=gCursor_tinted_center;
    scalars[5]=gCursor_line_width;scalars[6]=gTransient_bitmap_index;scalars[7]=gMouse_last_pos.x;scalars[8]=gMouse_last_pos.y;scalars[9]=gMouse_in_use;
    scalars[10]=gCurrent_cursor_index;scalars[11]=gMouse_started;scalars[12]=gNext_transient;scalars[13]=gCursor_last_call_time;scalars[14]=gCursor_last_required_change;
    scalars[15]=gCursor_delta_x;scalars[16]=gCursor_required_cursor;scalars[17]=gCursor_zero_count;scalars[18]=gCursor_button_was_down;scalars[19]=gDraw_cursor;scalars[20]=gNoTransients;
    scalars[21]=norm((unsigned)(uintptr_t)gBack_screen);scalars[22]=norm((unsigned)(uintptr_t)gCurrent_graf_data);scalars[23]=norm((unsigned)(uintptr_t)gTintedPolyCamera);scalars[24]=norm((unsigned)(uintptr_t)gDepth_buffer);
    fwrite(scalars,sizeof(scalars),1,stdout);memcpy(table,gTransient_bitmaps,sizeof(table));
    for(i=0;i<50;i++){v=norm((unsigned)(uintptr_t)gTransient_bitmaps[i].pixmap);memcpy(table+i*24,&v,4);}
    fwrite(table,sizeof(table),1,stdout);memcpy(copy,&heap,sizeof(copy));
    for(i=0;i<53;i++){v=norm((unsigned)(uintptr_t)heap.maps[i].pixels);memcpy(copy+offsetof(fixture_heap,maps)+68*i+8,&v,4);}
    v=norm((unsigned)(uintptr_t)heap.back.pixels);memcpy(copy+offsetof(fixture_heap,back)+8,&v,4);
    v=norm((unsigned)(uintptr_t)heap.depth.pixels);memcpy(copy+offsetof(fixture_heap,depth)+8,&v,4);
    for(i=0;i<11;i++){v=norm((unsigned)(uintptr_t)heap.materials[i].colour_map);memcpy(copy+offsetof(fixture_heap,materials)+156*i+64,&v,4);}
    for(i=0;i<10;i++){
        v=norm((unsigned)(uintptr_t)heap.actors[i].model);memcpy(copy+offsetof(fixture_heap,actors)+100*i+24,&v,4);
        v=norm((unsigned)(uintptr_t)heap.actors[i].material);memcpy(copy+offsetof(fixture_heap,actors)+100*i+28,&v,4);
        v=norm((unsigned)(uintptr_t)heap.models[i].vertices);memcpy(copy+offsetof(fixture_heap,models)+92*i+8,&v,4);
        memcpy(headers[i],&gTintedPolys[i],72);for(j=0;j<3;j++){memcpy(&v,headers[i]+4*j,4);v=norm(v);memcpy(headers[i]+4*j,&v,4);}
        memcpy(&v,headers[i]+0x2c,4);v=norm(v);memcpy(headers[i]+0x2c,&v,4);memcpy(&v,headers[i]+0x44,4);v=norm(v);memcpy(headers[i]+0x44,&v,4);
    }
    fwrite(copy,sizeof(copy),1,stdout);fwrite(headers,sizeof(headers),1,stdout);
}
static void C2_HOOK_CDECL event(unsigned code,unsigned n,const unsigned* values) {unsigned data[12],i;memset(data,0,sizeof(data));data[0]=code;for(i=0;i<n;i++)data[i+1]=values[i];fwrite(data,sizeof(data),1,stdout);snapshot();}
tU32 C2_HOOK_FASTCALL PDGetTotalTime(void) {unsigned values[2];values[0]=a[TIME_BASE]+a[TIME_STEP]*time_calls;values[1]=time_calls++;event(1,2,values);return values[0];}
void C2_HOOK_FASTCALL GetMousePosition(int* x,int* y) {unsigned values[4];values[0]=x==&gMouse_last_pos.x&&y==&gMouse_last_pos.y;values[1]=a[MOUSE_X];values[2]=a[MOUSE_Y];values[3]=mouse_calls++;event(2,4,values);*x=(int)values[1];*y=(int)values[2];}
int C2_HOOK_FASTCALL EitherMouseButtonDown(void) {unsigned value=a[BUTTON];event(3,1,&value);return (int)value;}
br_pixelmap* C2_HOOK_CDECL BrMapFind(const char* pattern) {unsigned values[2],index;if(!strcmp(pattern,"mse_corn"))index=0;else if(!strcmp(pattern,"mse_cros"))index=1;else if(!strcmp(pattern,"mse_line"))index=2;else abort();values[0]=index;values[1]=a[MAPS]&(1u<<index)?0x1000000+offsetof(fixture_heap,maps)+(50+index)*68:0;event(4,2,values);lookup_calls++;return values[1]?&heap.maps[50+index]:NULL;}
int C2_HOOK_FASTCALL CreateTintedPoly(int x,int y,int width,int height,int cls,int arg1,int arg2,int arg3) {unsigned values[9],index;static const unsigned aliases[5]={1,1,2,2,3};index=a[ALLOCATE_ALIAS]?aliases[create_calls%5]:create_calls%5;values[0]=x;values[1]=y;values[2]=width;values[3]=height;values[4]=cls;values[5]=arg1;values[6]=arg2;values[7]=arg3;values[8]=index;event(5,9,values);create_calls++;gTintedPolys[index].used=1;gTintedPolys[index].visible=0;gTintedPolys[index].class=cls;gTintedPolys[index].color_red=arg1;gTintedPolys[index].color_grn=arg2;gTintedPolys[index].color_blu=arg3;return (int)index;}
void C2_HOOK_CDECL BrMaterialUpdate(br_material* material,br_uint_16 flags) {unsigned values[2];values[0]=norm((unsigned)(uintptr_t)material);values[1]=flags;event(6,2,values);if((a[MUTATE]&2)&&material_calls++==0)gTintedPolys[gCursor_tinted_top].material=&heap.materials[10];}
void C2_HOOK_CDECL BrMatrix23Rotate(br_matrix23* matrix,br_angle angle) {unsigned values[2],i,bits;values[0]=norm((unsigned)(uintptr_t)matrix);values[1]=angle;event(7,2,values);for(i=0;i<6;i++){bits=0x3f000000+i;memcpy((char*)matrix+4*i,&bits,4);}}
void C2_HOOK_CDECL BrPixelmapRectangleCopy(br_pixelmap* dst,br_int_32 dx,br_int_32 dy,br_pixelmap* src,br_int_32 sx,br_int_32 sy,br_int_32 width,br_int_32 height) {unsigned values[8];values[0]=norm((unsigned)(uintptr_t)dst);values[1]=dx;values[2]=dy;values[3]=norm((unsigned)(uintptr_t)src);values[4]=sx;values[5]=sy;values[6]=width;values[7]=height;event(8,8,values);if(dst->pixels){unsigned value=0xabcdef00+copy_calls;memcpy(dst->pixels,&value,4);}copy_calls++;}
void C2_HOOK_CDECL BrPixelmapFree(br_pixelmap* map) {unsigned value=norm((unsigned)(uintptr_t)map);event(9,1,&value);memset(map,0xdd,sizeof(*map));}
int C2_HOOK_FASTCALL PossibleUnlock(int value) {unsigned bits=(unsigned)value;event(10,1,&bits);if(a[MUTATE]&1)gCursor_line_width++;return 1;}
void C2_HOOK_FASTCALL UpdateTintedPolyByMaterial(int index) {(void)index;abort();}
void C2_HOOK_FASTCALL UpdatePulsePoly(int index) {(void)index;abort();}
void C2_HOOK_FASTCALL UpdateFirePoly(int index) {(void)index;abort();}
int C2_HOOK_FASTCALL PDKeyDown(int key) {(void)key;abort();return 0;}
tU32 C2_HOOK_FASTCALL GetTotalTime(void) {abort();return 0;}
void C2_HOOK_CDECL BrModelUpdate(br_model* model,br_uint_16 flags) {unsigned values[2];values[0]=norm((unsigned)(uintptr_t)model);values[1]=flags;event(16,2,values);}
void C2_HOOK_CDECL BrZbsSceneRender(br_actor* world,br_actor* camera,br_pixelmap* colour,br_pixelmap* depth) {unsigned values[4];values[0]=norm((unsigned)(uintptr_t)world);values[1]=norm((unsigned)(uintptr_t)camera);values[2]=norm((unsigned)(uintptr_t)colour);values[3]=norm((unsigned)(uintptr_t)depth);event(17,4,values);}
'''
    for name in ("SetTintedPolySize", "TurnTintedPolyOn", "ProcessTintedPoly", "RenderTintedPolys", "TurnTintedPolyOff"):
        driver += extract(tinted, name).replace(name + "(", "real_" + name + "(", 1)
    driver += r'''
void C2_HOOK_FASTCALL SetTintedPolySize(int index,int x,int y,int width,int height) {unsigned values[5];values[0]=index;values[1]=x;values[2]=y;values[3]=width;values[4]=height;event(11,5,values);real_SetTintedPolySize(index,x,y,width,height);}
void C2_HOOK_FASTCALL TurnTintedPolyOn(int index) {unsigned value=(unsigned)index;event(12,1,&value);real_TurnTintedPolyOn(index);}
void C2_HOOK_FASTCALL ProcessTintedPoly(int index) {unsigned value=(unsigned)index;event(13,1,&value);real_ProcessTintedPoly(index);}
void C2_HOOK_FASTCALL RenderTintedPolys(void) {event(14,0,NULL);real_RenderTintedPolys();}
void C2_HOOK_FASTCALL TurnTintedPolyOff(int index) {unsigned value=(unsigned)index;event(15,1,&value);real_TurnTintedPolyOff(index);}
'''
    driver += "\n".join(extract(source, name) for name in FUNCTIONS)
    driver += r'''
int C2_HOOK_CDECL main(int argc,char**argv) {
    FILE* file;unsigned i,j,v;int result=0;unsigned char pool[sizeof(gTintedPolys)];
    if(argc!=2)return 90;
    if(!strcmp(argv[1],"schema")){printf("[%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u]\n",(unsigned)offsetof(fixture_heap,maps),(unsigned)offsetof(fixture_heap,materials),(unsigned)offsetof(fixture_heap,actors),(unsigned)offsetof(fixture_heap,models),(unsigned)offsetof(fixture_heap,vertices),(unsigned)offsetof(fixture_heap,graf),(unsigned)offsetof(fixture_heap,camera),(unsigned)offsetof(fixture_heap,back),(unsigned)offsetof(fixture_heap,depth),(unsigned)offsetof(fixture_heap,pixels),(unsigned)sizeof(heap));return 0;}
    file=fopen(argv[1],"rb");if(!file)return 91;if(fread(a,sizeof(a),1,file)!=1)return 92;fclose(file);
    _setmode(_fileno(stdout),_O_BINARY);_controlfp(_PC_53,_MCW_PC);
    memset(&heap,(int)a[SEED],sizeof(heap));memset(gTintedPolys,(int)a[SEED],sizeof(gTintedPolys));memset(gTransient_bitmaps,(int)a[SEED],sizeof(gTransient_bitmaps));
    for(i=0;i<53;i++){heap.maps[i].pixels=heap.pixels[i];heap.maps[i].width=(br_uint_16)(a[WIDE]?0x8001:16+i%2);heap.maps[i].height=(br_uint_16)(a[WIDE]?0xfffe:8+i%2);}
    for(i=0;i<11;i++)heap.materials[i].colour_map=NULL;
    for(i=0;i<10;i++){
        heap.actors[i].model=&heap.models[i];heap.actors[i].material=&heap.materials[i];heap.actors[i].render_style=BR_RSTYLE_NONE;
        heap.models[i].vertices=heap.vertices[i];heap.models[i].nvertices=4;
        gTintedPolys[i].actor=&heap.actors[i];gTintedPolys[i].material=&heap.materials[i];gTintedPolys[i].model=&heap.models[i];gTintedPolys[i].material2=NULL;gTintedPolys[i].pixelmap=NULL;
        gTintedPolys[i].used=(int)a[TINT_USED];gTintedPolys[i].visible=0;gTintedPolys[i].class=1;gTintedPolys[i].color_red=17;gTintedPolys[i].color_grn=29;gTintedPolys[i].color_blu=37;
    }
    for(i=0;i<50;i++){
        gTransient_bitmaps[i].pixmap=a[COPY_MODE]==2|| (a[COPY_MODE]==1&&i>=5)?NULL:&heap.maps[a[COPY_MODE]==3&&i==1?0:i];
        gTransient_bitmaps[i].in_use=a[COPY_MODE]==4?0:a[COPY_MODE]==1?i!=0:1;
        gTransient_bitmaps[i].order_number=a[COPY_MODE]==1?(i==0?1:i==1?1:i==2?0:i==3?3:2):(int)i;
        gTransient_bitmaps[i].x_coord=100+(int)i;gTransient_bitmaps[i].y_coord=200-(int)i;
    }
    heap.graf.width=(int)a[WIDTH];heap.graf.height=(int)a[HEIGHT];gCurrent_graf_data=&heap.graf;gBack_screen=&heap.back;gDepth_buffer=&heap.depth;gTintedPolyCamera=&heap.camera;
    heap.back.pixels=heap.pixels[50];heap.depth.pixels=heap.pixels[51];
    gCursor_tinted_top=(int)a[TOP];gCursor_tinted_left=(int)a[LEFT];gCursor_tinted_bottom=(int)a[BOTTOM];gCursor_tinted_right=(int)a[RIGHT];gCursor_tinted_center=(int)a[CENTER];gCursor_line_width=(int)a[LINE_WIDTH];
    gNoTransients=(int)a[NO_TRANSIENTS];gNext_transient=(int)a[NEXT_TRANSIENT];gTransient_bitmap_index=(int)a[TRANSIENT_INDEX];gMouse_in_use=(int)a[MOUSE_IN_USE];gMouse_started=(int)a[MOUSE_STARTED];gCurrent_cursor_index=(int)a[CURRENT_CURSOR];gMouse_last_pos.x=(int)a[LAST_X];gMouse_last_pos.y=(int)a[LAST_Y];
    gCursor_last_call_time=a[LAST_TIME];gCursor_last_required_change=a[LAST_CHANGE];gCursor_delta_x=(int)a[DELTA_X];gCursor_required_cursor=(int)a[REQUIRED_CURSOR];gCursor_zero_count=(int)a[ZERO_COUNT];gCursor_button_was_down=(int)a[BUTTON_WAS];gDraw_cursor=(int)a[DRAW_CURSOR];
    for(j=0;j<a[REPEAT];j++)switch(a[MODE]){
    case 0:RemoveTransientBitmaps((int)a[GRAPHICAL]);break;case 1:result=DoMouseCursor();break;case 2:AllocateCursorActors();break;case 3:StartMouseCursor();break;case 4:EndMouseCursor();break;
    case 5:StartMouseCursor();result=DoMouseCursor();EndMouseCursor();break;default:return 93;
    }
    v=(unsigned)result;event(0,1,&v);memcpy(pool,gTintedPolys,sizeof(pool));
    for(i=0;i<10;i++){for(j=0;j<3;j++){memcpy(&v,pool+i*0x6450+4*j,4);v=norm(v);memcpy(pool+i*0x6450+4*j,&v,4);}memcpy(&v,pool+i*0x6450+0x2c,4);v=norm(v);memcpy(pool+i*0x6450+0x2c,&v,4);memcpy(&v,pool+i*0x6450+0x44,4);v=norm(v);memcpy(pool+i*0x6450+0x44,&v,4);}
    fwrite(pool,sizeof(pool),1,stdout);return 0;
}
'''
    harness = out / "harness.c";harness.write_text(driver)
    includes = [ROOT / path for path in ("src/carma2/newcommon", "src/carma2/include", "src/carma2/hooks", "src/carma2/platform", "src/carma2/s3/include", "src/brender/include")]
    msvc = sorted((args.msvc_root / "VC/Tools/MSVC").iterdir())[-1];sdk = args.msvc_root / "Windows Kits/10";version = sorted((sdk / "Include").iterdir())[-1].name
    env = dict(os.environ);env["PATH"] = str(msvc / "bin/Hostx64/x86") + os.pathsep + env["PATH"]
    env["INCLUDE"] = os.pathsep.join(map(str, [msvc / "include", *[sdk / f"Include/{version}/{part}" for part in ("ucrt", "shared", "um")]]));env["LIB"] = os.pathsep.join(map(str, [msvc / "lib/x86", *[sdk / f"Lib/{version}/{part}/x86" for part in ("ucrt", "um")]]))
    msvc5 = args.msvc5_root.resolve();env5 = dict(os.environ);env5["PATH"] = str(msvc5 / "bin") + os.pathsep + str(msvc5 / "redist") + os.pathsep + env5["PATH"];env5["INCLUDE"] = str(msvc5 / "include");env5["LIB"] = str(msvc5 / "lib")
    commands = {
        "msvc": [str(msvc / "bin/Hostx64/x86/cl.exe"), "/nologo", "/O2", "/W3", "/WX", "/D_CRT_SECURE_NO_WARNINGS", *["/I" + str(p) for p in includes], str(harness), "/Fo" + str(out / "msvc.obj"), "/Fe" + str(out / "msvc.exe")],
        "msvc5": [str(msvc5 / "bin/cl.exe"), "/nologo", "/O2", "/Ob2", "/G5", "/Gr", "/ML", "/W3", "/WX", "/DREC2_MATCHING", "/I" + str(ROOT / "src/msvc500/include"), *["/I" + str(p) for p in includes], str(harness), "/Fo" + str(out / "msvc5.obj"), "/Fe" + str(out / "msvc5.exe")],
        "mingw": [str(args.gcc), "-std=gnu89", "-O2", "-Wall", "-Wextra", "-Werror", *["-I" + str(p) for p in includes], str(harness), "-o", str(out / "mingw.exe")],
    }
    schemas = []
    for compiler, command in commands.items():
        result = subprocess.run(command, cwd=out, env=env if compiler == "msvc" else env5 if compiler == "msvc5" else None, text=True, capture_output=True)
        (out / f"{compiler}-build.txt").write_text(result.stdout + result.stderr)
        if result.returncode:raise RuntimeError(result.stdout + result.stderr)
        schemas.append(json.loads(subprocess.check_output([str(out / f"{compiler}.exe"), "schema"], text=True)))
    assert schemas[0] == schemas[1] == schemas[2]
    maps, materials, actors, models, vertices, graf, camera, back, depth, pixels, heap_size = schemas[0]
    def words(values):
        return struct.pack("<" + "I" * len(values), *[value & 0xffffffff for value in values])

    def original(image, addresses, c):
        u = Uc(UC_ARCH_X86, UC_MODE_32)
        pe = struct.unpack_from("<I", image, 60)[0]
        image_base = struct.unpack_from("<I", image, pe + 52)[0]
        image_size = struct.unpack_from("<I", image, pe + 80)[0]
        u.mem_map(image_base, (image_size + 4095) & ~4095);u.mem_map(HEAP, 0x100000)
        table = pe + 24 + struct.unpack_from("<H", image, pe + 20)[0]
        for i in range(struct.unpack_from("<H", image, pe + 6)[0]):
            _, rva, size, offset = struct.unpack_from("<IIII", image, table + i * 40 + 8)
            u.mem_write(image_base + rva, image[offset:offset + size])

        def wr(address, value):u.mem_write(address, words([value]))
        def rd(address):return struct.unpack("<I", u.mem_read(address, 4))[0]
        def half(address, value):u.mem_write(address, struct.pack("<H", value & 65535))
        pool, transients = addresses["gTintedPolys"], addresses["gTransient_bitmaps"]
        u.mem_write(HEAP, bytes([c["seed"]]) * heap_size);u.mem_write(pool, bytes([c["seed"]]) * (10 * 0x6450));u.mem_write(transients, bytes([c["seed"]]) * 1200)
        for i in range(53):
            wr(HEAP + maps + i * 68 + 8, HEAP + pixels + i * 32)
            half(HEAP + maps + i * 68 + 52, 0x8001 if c["wide"] else 16 + i % 2)
            half(HEAP + maps + i * 68 + 54, 0xfffe if c["wide"] else 8 + i % 2)
        for i in range(11):wr(HEAP + materials + i * 156 + 64, 0)
        for i in range(10):
            wr(HEAP + actors + i * 100 + 24, HEAP + models + i * 92);wr(HEAP + actors + i * 100 + 28, HEAP + materials + i * 156)
            u.mem_write(HEAP + actors + i * 100 + 32, b"\x01")
            wr(HEAP + models + i * 92 + 8, HEAP + vertices + i * 160);half(HEAP + models + i * 92 + 16, 4)
            p = pool + i * 0x6450
            for offset, value in ((0, HEAP + actors + i * 100), (4, HEAP + materials + i * 156), (8, HEAP + models + i * 92), (0x2c, 0), (0x44, 0), (0x30, c["tint_used"]), (0x34, 0), (0x38, 1), (0xc, 17), (0x10, 29), (0x14, 37)):
                wr(p + offset, value)
        for i in range(50):
            pointer = 0 if c["copy_mode"] == 2 or c["copy_mode"] == 1 and i >= 5 else HEAP + maps + (0 if c["copy_mode"] == 3 and i == 1 else i) * 68
            in_use = 0 if c["copy_mode"] == 4 else int(i != 0) if c["copy_mode"] == 1 else 1
            order = (1 if i <= 1 else 0 if i == 2 else 3 if i == 3 else 2) if c["copy_mode"] == 1 else i
            for offset, value in ((0, pointer), (4, in_use), (8, 100 + i), (12, 200 - i), (16, order)):
                wr(transients + i * 24 + offset, value)
        wr(HEAP + graf, c["width"]);wr(HEAP + graf + 4, c["height"])
        wr(HEAP + back + 8, HEAP + pixels + 50 * 32);wr(HEAP + depth + 8, HEAP + pixels + 51 * 32)
        scalar_fields = "top left bottom right center line_width transient_index last_x last_y mouse_in_use current_cursor mouse_started next_transient last_time last_change delta_x required_cursor zero_count button_was draw_cursor no_transients".split()
        for name, field in zip(SCALARS, scalar_fields):wr(addresses[name], c[field])
        for name, offset in zip(POINTERS, (back, graf, camera, depth)):wr(addresses[name], HEAP + offset)
        output = bytearray();counts = dict(time=0, mouse=0, lookup=0, create=0, copy=0, material=0)

        def snapshot():
            return (words([rd(addresses[name]) for name in SCALARS] + [rd(addresses[name]) for name in POINTERS])
                    + bytes(u.mem_read(transients, 1200)) + bytes(u.mem_read(HEAP, heap_size))
                    + b"".join(bytes(u.mem_read(pool + i * 0x6450, 72)) for i in range(10)))

        def event(code, values):
            assert len(values) <= 11
            output.extend(words([code, *values, *([0] * (11 - len(values)))]));output.extend(snapshot())

        reverse = {addresses[name]: index + 1 for index, name in enumerate(BOUNDARIES)}

        def hook(uc, address, size, context):
            code = reverse[address];sp = uc.reg_read(UC_X86_REG_ESP);ecx = uc.reg_read(UC_X86_REG_ECX);edx = uc.reg_read(UC_X86_REG_EDX)
            result = 0;cleanup = 0
            if code == 1:
                result = (c["time_base"] + c["time_step"] * counts["time"]) & 0xffffffff
                event(code, [result, counts["time"]]);counts["time"] += 1
            elif code == 2:
                event(code, [int(ecx == addresses["gMouse_last_pos"] and edx == addresses["mouse_y"]), c["mouse_x"], c["mouse_y"], counts["mouse"]])
                counts["mouse"] += 1;wr(ecx, c["mouse_x"]);wr(edx, c["mouse_y"])
            elif code == 3:
                result = c["button"] & 0xffffffff;event(code, [result])
            elif code == 4:
                name = bytearray();pointer = rd(sp + 4)
                while uc.mem_read(pointer + len(name), 1)[0]:name += uc.mem_read(pointer + len(name), 1)
                index = [b"mse_corn", b"mse_cros", b"mse_line"].index(bytes(name))
                result = HEAP + maps + (50 + index) * 68 if c["maps"] & (1 << index) else 0
                event(code, [index, result]);counts["lookup"] += 1
            elif code == 5:
                values = [ecx, edx, *[rd(sp + 4 + i * 4) for i in range(6)]]
                result = [1, 1, 2, 2, 3][counts["create"] % 5] if c["allocate_alias"] else counts["create"] % 5
                event(code, [*values, result]);counts["create"] += 1
                for offset, value in ((0x30, 1), (0x34, 0), (0x38, values[4]), (0xc, values[5]), (0x10, values[6]), (0x14, values[7])):wr(pool + result * 0x6450 + offset, value)
                cleanup = 24
            elif code == 6:
                event(code, [rd(sp + 4), rd(sp + 8) & 65535])
                if c["mutate"] & 2:
                    if counts["material"] == 0:wr(pool + rd(addresses["gCursor_tinted_top"]) * 0x6450 + 4, HEAP + materials + 10 * 156)
                    counts["material"] += 1
            elif code == 7:
                pointer, angle = rd(sp + 4), rd(sp + 8) & 65535;event(code, [pointer, angle])
                uc.mem_write(pointer, words([0x3f000000 + i for i in range(6)]))
            elif code == 8:
                values = [rd(sp + 4 + i * 4) for i in range(8)];event(code, values)
                if rd(values[0] + 8):wr(rd(values[0] + 8), 0xabcdef00 + counts["copy"])
                counts["copy"] += 1
            elif code == 9:
                pointer = rd(sp + 4);event(code, [pointer]);uc.mem_write(pointer, b"\xdd" * 68)
            elif code == 10:
                event(code, [ecx]);result = 1
                if c["mutate"] & 1:wr(addresses["gCursor_line_width"], rd(addresses["gCursor_line_width"]) + 1)
            elif code == 11:
                event(code, [ecx, edx, *[rd(sp + 4 + i * 4) for i in range(3)]]);return
            elif code in (12, 13, 15):
                event(code, [ecx]);return
            elif code == 14:
                event(code, []);return
            elif code == 16:
                event(code, [rd(sp + 4), rd(sp + 8) & 65535])
            else:
                event(code, [rd(sp + 4 + i * 4) for i in range(4)])
            uc.reg_write(UC_X86_REG_EAX, result & 0xffffffff);uc.reg_write(UC_X86_REG_EIP, rd(sp));uc.reg_write(UC_X86_REG_ESP, sp + 4 + cleanup)

        for target in reverse:u.hook_add(UC_HOOK_CODE, hook, begin=target, end=target)
        result = 0
        names = list(FUNCTIONS)
        for repeat in range(c["repeat"]):
            calls = ["StartMouseCursor", "DoMouseCursor", "EndMouseCursor"] if c["mode"] == 5 else [names[c["mode"]]]
            for name in calls:
                wr(STACK, STOP);u.reg_write(UC_X86_REG_ESP, STACK);u.reg_write(UC_X86_REG_ECX, c["graphical"] & 0xffffffff);u.reg_write(UC_X86_REG_FPCW, 0x27f)
                u.emu_start(addresses[name], STOP, count=1000000)
                assert u.reg_read(UC_X86_REG_EIP) == STOP, "Original cursor instruction budget exhausted"
                if name == "DoMouseCursor":result = u.reg_read(UC_X86_REG_EAX)
        event(0, [result]);output.extend(bytes(u.mem_read(pool, 10 * 0x6450)))
        return bytes(output)

    def case(mode, **kw):
        c = dict(zip(FIELDS, [0] * len(FIELDS)))
        c.update(mode=mode, graphical=1, next_transient=5, mouse_started=7, current_cursor=3, last_x=20, last_y=30,
                 delta_x=17, required_cursor=1, button_was=7, draw_cursor=1, top=0, left=1, bottom=2, right=3, center=4,
                 line_width=5, width=640, height=480, mouse_x=40, mouse_y=50, button=1, time_base=1000, time_step=30,
                 maps=7, repeat=1, seed=165, tint_used=1)
        c.update(kw)
        return c

    cases = [case(mode, graphical=graphical, no_transients=no_transients, next_transient=count, copy_mode=copy_mode, repeat=2)
             for mode in (0, 4) for graphical in (0, 1) for no_transients in (0, 1) for count in (-1, 0, 5) for copy_mode in (0, 1, 2, 4)]
    cases += [case(mode, maps=maps_available, allocate_alias=alias, top=top, mutate=mutate, repeat=2)
              for mode in (2, 3) for maps_available in (0, 1, 2, 4, 7) for alias in (0, 1) for top in (-1, 0) for mutate in (0, 2)]
    cases += [case(1, mouse_x=x, mouse_y=y, mouse_in_use=in_use, no_transients=no_transients, draw_cursor=draw, repeat=7)
              for x, y in ((20, 30), (20, 50), (40, 50), (-20, 50)) for in_use in (0, 1) for no_transients in (0, 1) for draw in (0, 1, 2)]
    cases += [case(1, last_time=last, time_base=time_base, time_step=step, last_change=last_change, zero_count=zero, transient_index=49, repeat=2)
              for last, time_base, step in ((1000, 1000, 10), (0xffffffff, 0, 10), (1000, 995, 5)) for last_change in (801, 800, 0xffffffff) for zero in (0, 4, 5)]
    cases += [case(1, draw_cursor=0, last_x=x, mouse_x=y, mouse_y=30, last_time=1000, time_base=2000) for x, y in ((0x7fffffff, -1), (-1, 0x7fffffff), (0x7fffffff, 0x80000000))]
    cases += [case(1, wide=1), case(0, wide=1), case(1, tint_used=0), case(1, mouse_y=9), case(1, mutate=1), case(4, copy_mode=3), case(5, top=-1), case(5, no_transients=1)]
    if args.case_index is not None and any(index < 0 or index >= len(cases) for index in args.case_index):parser.error(f"--case-index must be between 0 and {len(cases)-1}")
    results = []
    for index, c in enumerate(cases):
        if args.case_index is not None and index not in args.case_index:continue
        expected = original(*images[0], c)
        fixture = out / "case.bin";fixture.write_bytes(words([c[field] for field in FIELDS]))
        checks = [(compiler, subprocess.check_output([str(out / (compiler + ".exe")), str(fixture)])) for compiler in commands]
        if len(images) > 1:checks.append(("matching PE", original(*images[1], c)))
        for compiler, actual in checks:
            if actual != expected:
                (out / f"failure-{compiler.replace(' ', '-')}-{index}-expected.bin").write_bytes(expected)
                (out / f"failure-{compiler.replace(' ', '-')}-{index}-actual.bin").write_bytes(actual)
                difference = next(i for i, values in enumerate(zip(actual, expected)) if values[0] != values[1]) if len(actual) == len(expected) else None
                (out / f"failure-{compiler.replace(' ', '-')}-{index}.json").write_text(json.dumps(dict(case=c, compiler=compiler, first_difference=difference, expected_size=len(expected), actual_size=len(actual)), indent=2))
                raise AssertionError(f"Cursor {compiler} scenario {index} differs; see {out}")
            results.append(dict(compiler=compiler, case_index=index, case=c, state_and_calls_sha256=hashlib.sha256(actual).hexdigest(), all_bytes_match=True))
        if (index + 1) % 25 == 0:print(f"Verified {index+1}/{len(cases)} cursor scenarios", flush=True)
    (out / "results.json").write_text(json.dumps(results, indent=2) + "\n")
    manifest = dict(original_sha256=HASH, comparisons=len(results), complete_suite=args.case_index is None, all_cases_passed=True,
                    functions=FUNCTIONS, type_schema=schemas[0], compiler_commands=commands, harness_sha256=hashlib.sha256(driver.encode()).hexdigest(),
                    source_sha256={str(path.relative_to(ROOT)): hashlib.sha256(path.read_bytes()).hexdigest() for path in (SOURCE, TINTED, ROOT / "src/carma2/newcommon/18-graphics2.h", ROOT / "src/carma2/include/rec2_types.h")},
                    limitations="Map lookup/create, time/input and BRender boundaries are controlled. Real tinted class1 resize/on/off/process/render bodies execute. Callback snapshots cover full objects/transients/scalars and overlay headers; final bytes cover the entire overlay pool. Free poisons local objects without allocator release.")
    if len(images) > 1:manifest["recompiled_sha256"] = hashlib.sha256(images[1][0]).hexdigest();manifest["symbols_sha256"] = hashlib.sha256(args.symbols.read_bytes()).hexdigest()
    (out / "verification.json").write_text(json.dumps(manifest, indent=2) + "\n")
    print(f"Passed {len(results)} original/native cursor comparisons")


if __name__ == "__main__":
    main()
