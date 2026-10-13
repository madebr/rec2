#!/usr/bin/env python3
"""Compare tinted lifecycle/visibility/query/resize bodies with original x86.

Compiles actual current bodies with repository 32-bit types under modern MSVC
and MinGW. Compares complete pool/object bytes, disposal order and state at
each dependency call, renderer arguments, raw query values and resize geometry.
Free fixtures poison objects without releasing memory; renderer/model update
fixtures record calls. This verifies ownership decisions, not BRender internals.
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
SOURCE = ROOT / "src/carma2/newcommon/66-tintedpoly.c"
ORIGINAL_HASH = "9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d"
NAMES = ["FreeTintedPolyActor", "FreeAllTintedPolyActor", "TurnTintedPolyOn", "TurnTintedPolyOff", "InWater", "TintedPolyIsOn", "RenderTintedPolys", "SetTintedPolySize"]
ENTRIES = [0x4d7c80, 0x4d7d10, 0x4d8220, 0x4d8250, 0x4d8470, 0x4d8cf0, 0x4d8290, 0x4d8bb0]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--original", type=Path, default=ROOT / "build-reference/originals/CARMA2_HW.EXE")
    parser.add_argument("--dependency-dir", type=Path, default=ROOT / "build-audit/joystick/python-deps")
    parser.add_argument("--gcc", type=Path, default=ROOT / "build-tools/winlibs-i686/mingw32/bin/gcc.exe")
    parser.add_argument("--msvc-root", type=Path, default=ROOT / "build-tools/msvc-modern")
    parser.add_argument("--output", type=Path, default=ROOT / "build-audit/tinted-lifecycle-tests")
    args = parser.parse_args()
    sys.path.insert(0, str(args.dependency_dir.resolve()))
    from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
    from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EIP, UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX, UC_X86_REG_FPCW
    binary = args.original.read_bytes()
    if hashlib.sha256(binary).hexdigest() != ORIGINAL_HASH:
        raise SystemExit("Original executable hash differs from verified binary")
    out = args.output.resolve()
    out.mkdir(parents=True, exist_ok=True)
    driver = r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "66-tintedpoly.h"
#include "globvars.h"
#include "rec2_macros.h"
typedef char check_layout[sizeof(void*)==4&&sizeof(tTintedPoly)==0x6450&&sizeof(br_actor)==100&&sizeof(br_model)==92&&sizeof(br_material)==156&&sizeof(br_vertex)==40&&sizeof(br_pixelmap)==68&&offsetof(tTintedPoly,used)==0x30&&offsetof(tTintedPoly,visible)==0x34&&offsetof(tTintedPoly,material2)==0x2c?1:-1];
tTintedPoly gTintedPolys[10];
br_actor* gTintedPolyCamera;
br_pixelmap* gBack_screen;
br_pixelmap* gDepth_buffer;
typedef struct {br_actor actors[11];br_material materials[10];br_model models[10];br_vertex vertices[10][25];br_material refs[10];br_pixelmap back,depth;br_actor camera;} fixture_heap;
static fixture_heap heap;
static unsigned events[64][10],event_count;
static int mutate_remove;
static unsigned norm(unsigned value) {
    unsigned base=(unsigned)(uintptr_t)&heap;
    return value>=base&&value<base+sizeof(heap)?value-base+0x1000000:value;
}
static int locate(void* pointer) {
    int i;for(i=0;i<10;i++)if(gTintedPolys[i].actor==pointer||gTintedPolys[i].material==pointer||gTintedPolys[i].model==pointer)return i;
    abort();return -1;
}
static int record(unsigned code,void* pointer) {
    int i=locate(pointer);tTintedPoly* p=&gTintedPolys[i];unsigned* e=events[event_count++];
    if(event_count>64)abort();
    e[0]=code;e[1]=(unsigned)(uintptr_t)pointer;e[2]=i;e[3]=(unsigned)(uintptr_t)p->actor;e[4]=(unsigned)(uintptr_t)p->material;e[5]=(unsigned)(uintptr_t)p->model;
    e[6]=p->used;e[7]=p->class;e[8]=p->visible;e[9]=(unsigned)(uintptr_t)p->material2;return i;
}
void C2_HOOK_CDECL BrMaterialFree(br_material* p) {record(1,p);memset(p,0xcc,sizeof(*p));}
void C2_HOOK_CDECL BrModelFree(br_model* p) {record(2,p);memset(p,0xbb,sizeof(*p));}
br_actor* C2_HOOK_CDECL BrActorRemove(br_actor* p) {
    int i=record(3,p);if(mutate_remove)gTintedPolys[i].actor=&heap.actors[10];return p;
}
void C2_HOOK_CDECL BrActorFree(br_actor* p) {record(4,p);memset(p,0xdd,sizeof(*p));}
void C2_HOOK_CDECL BrModelUpdate(br_model* p,br_uint_16 flags) {if(flags!=BR_MODU_VERTICES)abort();record(5,p);}
void C2_HOOK_CDECL BrZbsSceneRender(br_actor* world,br_actor* camera,br_pixelmap* colour,br_pixelmap* depth) {
    unsigned* e=events[event_count++];e[0]=6;e[1]=(unsigned)(uintptr_t)world;e[2]=(unsigned)(uintptr_t)camera;e[3]=(unsigned)(uintptr_t)colour;e[4]=(unsigned)(uintptr_t)depth;
}
'''
    source = SOURCE.read_text()
    driver += "\n".join(extract(source, name) for name in NAMES)
    driver += r'''
static void dump(void* pointer,unsigned size) {
    unsigned i,j,value;unsigned char* bytes=pointer;
    for(i=0;i<size;i+=4){memcpy(&value,bytes+i,4);value=norm(value);for(j=0;j<4;j++)printf("%02x",(value>>(j*8))&255);}
}
int main(int argc,char**argv) {
    int mode,index,used,mask,tag,visible,x0,y0,width,height,seed,i,m;
    unsigned value=0;
    if(argc==2){printf("[%u,%u,%u,%u,%u,%u,%u,%u,%u]\n",(unsigned)offsetof(fixture_heap,actors),(unsigned)offsetof(fixture_heap,materials),(unsigned)offsetof(fixture_heap,models),(unsigned)offsetof(fixture_heap,vertices),(unsigned)offsetof(fixture_heap,refs),(unsigned)offsetof(fixture_heap,back),(unsigned)offsetof(fixture_heap,depth),(unsigned)offsetof(fixture_heap,camera),(unsigned)sizeof(heap));return 0;}
    mode=atoi(argv[1]);index=atoi(argv[2]);used=atoi(argv[3]);mask=atoi(argv[4]);tag=atoi(argv[5]);visible=atoi(argv[6]);mutate_remove=atoi(argv[7]);
    x0=atoi(argv[8]);y0=atoi(argv[9]);width=atoi(argv[10]);height=atoi(argv[11]);seed=atoi(argv[12]);
    memset(&heap,seed,sizeof(heap));memset(gTintedPolys,seed,sizeof(gTintedPolys));
    for(i=0;i<10;i++){
        m=mode==1?(i&7)|8:15;
        if(i==index&&mode!=1)m=mask;
        gTintedPolys[i].actor=m&4?&heap.actors[i]:NULL;gTintedPolys[i].material=m&1?&heap.materials[i]:NULL;
        gTintedPolys[i].model=m&2?&heap.models[i]:NULL;gTintedPolys[i].material2=m&8?&heap.refs[i]:NULL;
        gTintedPolys[i].used=mode==1?(i%3==0?0:i%3==1?1:-1):1;gTintedPolys[i].class=i%5+1;gTintedPolys[i].visible=i%3-1;
        heap.models[i].vertices=heap.vertices[i];
    }
    if(index>=0){gTintedPolys[index].used=used;gTintedPolys[index].class=tag;gTintedPolys[index].field_0x28=(br_uint_8)tag;gTintedPolys[index].visible=visible;}
    gTintedPolyCamera=mode==6&&!(mask&1)?NULL:&heap.camera;gBack_screen=mode==6&&!(mask&2)?NULL:&heap.back;gDepth_buffer=mode==6&&!(mask&4)?NULL:&heap.depth;
    switch(mode){
    case 0:FreeTintedPolyActor(index);FreeTintedPolyActor(index);break;
    case 1:FreeAllTintedPolyActor();FreeAllTintedPolyActor();break;
    case 2:TurnTintedPolyOn(index);break;case 3:TurnTintedPolyOff(index);break;
    case 4:value=norm((unsigned)(uintptr_t)InWater(index));break;case 5:value=(unsigned)TintedPolyIsOn(index);break;
    case 6:RenderTintedPolys();break;case 7:SetTintedPolySize(index,x0,y0,width,height);break;
    }
    printf("{\"value\":%u,\"events\":[",value);for(i=0;i<(int)event_count;i++){int j;printf("%s[",i?",":"");for(j=0;j<10;j++)printf("%s%u",j?",":"",norm(events[i][j]));printf("]");}
    printf("],\"pool\":\"");dump(gTintedPolys,sizeof(gTintedPolys));printf("\",\"heap\":\"");dump(&heap,sizeof(heap));printf("\"}\n");return 0;
}
'''
    harness = out / "harness.c"
    harness.write_text(driver)
    includes = [ROOT/path for path in ("src/carma2/newcommon","src/carma2/include","src/carma2/hooks","src/carma2/platform","src/carma2/s3/include","src/brender/include")]
    msvc = sorted((args.msvc_root/"VC/Tools/MSVC").iterdir())[-1]
    sdk = args.msvc_root/"Windows Kits/10"
    version = sorted((sdk/"Include").iterdir())[-1].name
    env = dict(os.environ)
    env["PATH"] = str(msvc/"bin/Hostx64/x86")+os.pathsep+str(sdk/f"bin/{version}/x64")+os.pathsep+env["PATH"]
    env["INCLUDE"] = os.pathsep.join(map(str,[msvc/"include",*[sdk/f"Include/{version}/{part}" for part in ("ucrt","shared","um")]]))
    env["LIB"] = os.pathsep.join(map(str,[msvc/"lib/x86",*[sdk/f"Lib/{version}/{part}/x86" for part in ("ucrt","um")]]))
    commands = {
        "msvc":[str(msvc/"bin/Hostx64/x86/cl.exe"),"/nologo","/O2","/W3","/WX","/D_CRT_SECURE_NO_WARNINGS",*["/I"+str(path) for path in includes],str(harness),"/Fo"+str(out/"msvc.obj"),"/Fe"+str(out/"msvc.exe")],
        "mingw":[str(args.gcc),"-std=gnu89","-O2","-Wall","-Wextra","-Werror",*["-I"+str(path) for path in includes],str(harness),"-o",str(out/"mingw.exe")],
    }
    schemas = []
    for compiler, command in commands.items():
        result = subprocess.run(command,env=env if compiler=="msvc" else None,capture_output=True,text=True)
        (out/f"{compiler}-build.txt").write_text(result.stdout+result.stderr)
        if result.returncode:
            raise RuntimeError(result.stdout+result.stderr)
        schemas.append(json.loads(subprocess.check_output([str(out/f"{compiler}.exe"),"schema"],text=True)))
    assert schemas[0] == schemas[1]
    actors,materials,models,vertices,refs,back,depth,camera,heap_size = schemas[0]
    pe=struct.unpack_from("<I",binary,60)[0]
    sections=struct.unpack_from("<H",binary,pe+6)[0]
    table=pe+24+struct.unpack_from("<H",binary,pe+20)[0]

    def original(case):
        mode,index,used,mask,tag,visible,mutate,x0,y0,width,height,seed=case
        u=Uc(UC_ARCH_X86,UC_MODE_32)
        u.mem_map(0x400000,0x400000);u.mem_map(0x1000000,0x10000)
        for i in range(sections):
            _,rva,size,offset=struct.unpack_from("<IIII",binary,table+i*40+8)
            u.mem_write(0x400000+rva,binary[offset:offset+size])
        def wr(address,value):u.mem_write(address,struct.pack("<I",value&0xffffffff))
        def rd(address):return struct.unpack("<I",u.mem_read(address,4))[0]
        base,pool=0x1000000,0x705c80
        u.mem_write(base,bytes([seed])*heap_size);u.mem_write(pool,bytes([seed])*(10*0x6450))
        for i in range(10):
            p=pool+i*0x6450;m=(i&7)|8 if mode==1 else mask if i==index else 15
            for offset,value in ((0,base+actors+i*100 if m&4 else 0),(4,base+materials+i*156 if m&1 else 0),(8,base+models+i*92 if m&2 else 0),(0x2c,base+refs+i*156 if m&8 else 0),(0x30,(0 if i%3==0 else 1 if i%3==1 else -1) if mode==1 else 1),(0x38,i%5+1),(0x34,i%3-1)):
                wr(p+offset,value)
            wr(base+models+i*92+8,base+vertices+i*25*40)
        if index>=0:
            p=pool+index*0x6450
            for offset,value in ((0x30,used),(0x38,tag),(0x34,visible)):wr(p+offset,value)
            u.mem_write(p+0x28,bytes([tag&255]))
        wr(0x6a0430,0 if mode==6 and not mask&1 else base+camera)
        wr(0x74d360,0 if mode==6 and not mask&2 else base+back)
        wr(0x68b8a4,0 if mode==6 and not mask&4 else base+depth)
        events=[]
        def record(code,pointer):
            i=next(i for i in range(10) if pointer in (rd(pool+i*0x6450),rd(pool+i*0x6450+4),rd(pool+i*0x6450+8)))
            p=pool+i*0x6450
            events.append([code,pointer,i,*[rd(p+offset) for offset in (0,4,8,0x30,0x38,0x34,0x2c)]])
            return i
        def hook(uc,address,size,context):
            codes={0x51f2a0:1,0x5217e0:2,0x51e070:3,0x51e2c0:4,0x51f950:5,0x522f30:6}
            if address not in codes:return
            sp=uc.reg_read(UC_X86_REG_ESP);pointer=rd(sp+4);code=codes[address]
            if code==6:
                events.append([6,*[rd(sp+4+i*4) for i in range(4)],0,0,0,0,0])
            else:
                i=record(code,pointer)
                if code in (1,2,4):u.mem_write(pointer,bytes([{1:0xcc,2:0xbb,4:0xdd}[code]])*{1:156,2:92,4:100}[code])
                if code==3 and mutate:wr(pool+i*0x6450,base+actors+1000)
                if code==5:assert rd(sp+8)==0x40
            uc.reg_write(UC_X86_REG_EAX,pointer if code==3 else 0);uc.reg_write(UC_X86_REG_EIP,rd(sp));uc.reg_write(UC_X86_REG_ESP,sp+4)
        u.hook_add(UC_HOOK_CODE,hook)
        for repeat in range(2 if mode in (0,1) else 1):
            stack,stop=0x100e000,0x100f000;wr(stack,stop)
            u.reg_write(UC_X86_REG_ESP,stack);u.reg_write(UC_X86_REG_ECX,index&0xffffffff);u.reg_write(UC_X86_REG_EDX,x0&0xffffffff);u.reg_write(UC_X86_REG_FPCW,0x37f)
            for i,value in enumerate((y0,width,height)):wr(stack+4+i*4,value)
            u.emu_start(ENTRIES[mode],stop,count=100000)
            assert u.reg_read(UC_X86_REG_EIP)==stop
        return {"value":u.reg_read(UC_X86_REG_EAX) if mode in (4,5) else 0,"events":events,"pool":bytes(u.mem_read(pool,10*0x6450)).hex(),"heap":bytes(u.mem_read(base,heap_size)).hex()}

    def case(mode,index=0,used=1,mask=15,tag=3,visible=99,mutate=0,dims=(3,7,641,479),seed=165):
        return (mode,index,used,mask,tag,visible,mutate,*dims,seed)
    cases=[case(0,index,used,mask) for index in (0,9) for used in (0,1,-1) for mask in range(8)]
    cases += [case(0,9,mutate=1),case(1,index=-1),case(1,index=-1,seed=53)]
    cases += [case(mode,index,used,mask=0 if used==0 else 15) for mode in (2,3) for index in (0,9) for used in (0,1,-1)]
    cases += [case(4,index,used,mask,tag) for index in (0,9) for used in (0,1) for mask in (7,15) for tag in (0,35,64,255)]
    cases += [case(5,index,used,visible=visible) for index in (0,9) for used in (0,1) for visible in (0,1,-1,99)]
    cases += [case(6,mask=mask) for mask in range(8)]
    cases += [case(7,index=-1),case(7,mask=13)]
    for index in (0,9):
        for tag in (3,2,7):
            for dims in ((3,7,641,479),(-9,13,-123,79),(0,0,0,480),(0,0,640,0),(0,0,0,0),(3,-5,16777217,16777219),(-7,13,16777219,33554433)):
                cases.append(case(7,index,used=0,tag=tag,dims=dims))
    results=[]
    for index,c in enumerate(cases):
        expected=original(c)
        for compiler in commands:
            native=json.loads(subprocess.check_output([str(out/f"{compiler}.exe"),*map(str,c)],text=True))
            if native!=expected:
                failure=out/f"failure-{compiler}-{index}.json"
                failure.write_text(json.dumps({"case":c,"original":expected,"native":native},indent=2))
                raise AssertionError(f"Lifecycle mismatch {compiler} case {index}: {failure}")
            results.append({"compiler":compiler,"function":NAMES[c[0]],"case":c,"all_bytes_and_calls_match":True})
    (out/"results.json").write_text(json.dumps(results,indent=2)+"\n")
    manifest={"original_sha256":ORIGINAL_HASH,"case_count":len(results),"all_cases_passed":True,"functions":dict(zip(NAMES,map(hex,ENTRIES))),"type_schema":schemas[0],"compiler_commands":commands,
              "source_sha256":{str(path.relative_to(ROOT)):hashlib.sha256(path.read_bytes()).hexdigest() for path in (SOURCE,ROOT/"src/carma2/newcommon/66-tintedpoly.h",ROOT/"src/carma2/include/rec2_types.h")},"harness_sha256":hashlib.sha256(driver.encode()).hexdigest(),
              "scope":"Actual bodies/types under modern MSVC and MinGW vs original x86; full pool/object bytes, repeat free, null/used gates, free order/state, actor pointer reload, raw queries, render args and integer-spaced resize/UV arithmetic."}
    (out/"verification.json").write_text(json.dumps(manifest,indent=2)+"\n")
    print(f"Passed {len(results)} native/original tinted lifecycle cases")


if __name__=="__main__":
    main()
