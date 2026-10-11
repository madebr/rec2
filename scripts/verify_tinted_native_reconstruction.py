#!/usr/bin/env python3
"""Check production tinted arithmetic under modern MSVC and i686 MinGW.

Includes the actual production C file and actual repository types. Controlled
allocation/image fixtures match the shared original-x86 verifier. Compares full
overlay state and all allocation bytes for class 4 and meshes, including float
rounding counterexamples, uint32 scale inputs and zero-range NaNs. Renderer and
image-loader implementations are outside this arithmetic test's scope.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import struct
import subprocess

from inventory_reconstruction_backlog import mask_c
import verify_tinted_startup_reconstruction as x86

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "src/carma2/newcommon/66-tintedpoly.c"


def extract(source, name):
    """Require exactly one definition; ignore calls, comments and string braces."""
    clean = mask_c(source)
    definitions = list(re.finditer(r"^[^\n;{}]*\bC2_HOOK_\w+\s+" + re.escape(name) + r"\([^;{}\n]*\)\s*\{", clean, re.M))
    if len(definitions) != 1:
        raise ValueError(f"Expected one production definition of {name}, found {len(definitions)}")
    start, end = definitions[0].start(), definitions[0].end()
    depth = 1
    while depth:
        depth += (clean[end] == "{") - (clean[end] == "}")
        end += 1
    return source[start:end]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--original", type=Path, default=ROOT / "build-reference/originals/CARMA2_HW.EXE")
    parser.add_argument("--recompiled", type=Path, default=ROOT / "build-matching/rec2.exe")
    parser.add_argument("--symbols", type=Path, required=True)
    parser.add_argument("--dependency-dir", type=Path, default=ROOT / "build-audit/joystick/python-deps")
    parser.add_argument("--gcc", type=Path, default=ROOT / "build-tools/winlibs-i686/mingw32/bin/gcc.exe")
    parser.add_argument("--msvc-root", type=Path, default=ROOT / "build-tools/msvc-modern")
    parser.add_argument("--output", type=Path, default=ROOT / "build-audit/tinted-native-tests")
    args = parser.parse_args()
    x86.configure(["--original", str(args.original), "--recompiled", str(args.recompiled), "--symbols", str(args.symbols), "--dependency-dir", str(args.dependency_dir)])
    out = args.output.resolve()
    out.mkdir(parents=True, exist_ok=True)
    driver = r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "66-tintedpoly.h"
#include "28-world3.h"
#include "41-utility.h"
#include "globvars.h"
#include "platform.h"
#include "rec2_macros.h"
typedef char check_layout[sizeof(void*)==4&&sizeof(tTintedPoly)==0x6450&&sizeof(br_material)==0x9c&&sizeof(br_model)==0x5c&&sizeof(br_vertex)==40&&sizeof(br_face)==40&&sizeof(br_actor)==100&&sizeof(br_camera)==36?1:-1];
br_pixelmap* gScreen;
tPath_name gApplication_path="C:/DATA/";
tBrender_storage gMisc_storage_space;
static unsigned arena[262144],used,count;
static void* pointers[30];static unsigned sizes[30];static const char* kinds[30];
static br_pixelmap* maps[3];
static unsigned range,lowest,pixels;
static void* allocate(const char* kind,unsigned size) {
    void* pointer=(char*)arena+used;
    if(count>=30||used+size>sizeof(arena))abort();
    pointers[count]=pointer;sizes[count]=size;kinds[count++]=kind;
    memset(pointer,0xa5,size);used+=(size+15)&~15u;return pointer;
}
br_actor* C2_HOOK_CDECL BrActorAllocate(br_uint_8 type,void* data) {
    br_actor* actor=allocate("actor",sizeof(br_actor));
    if(data)abort();
    if(type==BR_ACTOR_CAMERA)actor->type_data=allocate("camera",sizeof(br_camera));
    return actor;
}
br_material* C2_HOOK_CDECL BrMaterialAllocate(const char* name) {
    (void)name;return allocate("material",sizeof(br_material));
}
br_model* C2_HOOK_CDECL BrModelAllocate(const char* name,int nv,int nf) {
    br_model* model=allocate("model",sizeof(br_model));
    (void)name;nv&=65535;nf&=65535;
    model->vertices=allocate("vertices",nv*sizeof(br_vertex));model->faces=allocate("faces",nf*sizeof(br_face));
    model->nvertices=(br_uint_16)nv;model->nfaces=(br_uint_16)nf;return model;
}
br_material* C2_HOOK_CDECL BrMaterialAdd(br_material* material) {return material;}
void C2_HOOK_CDECL BrMaterialUpdate(br_material* material,br_uint_16 flags) {(void)material;(void)flags;}
br_model* C2_HOOK_CDECL BrModelAdd(br_model* model) {return model;}
br_actor* C2_HOOK_CDECL BrActorAdd(br_actor* parent,br_actor* actor) {(void)parent;return actor;}
br_pixelmap* C2_HOOK_CDECL BrMapFind(const char* name) {
    if(!strcmp(name,"testfire"))return maps[0];
    if(!strcmp(name,"firepal1"))return maps[1];
    if(!strcmp(name,"coolmapx"))return maps[2];
    abort();return NULL;
}
br_colour C2_HOOK_CDECL BrPixelmapPixelGet(br_pixelmap* map,br_int_32 x,br_int_32 y) {
    pixels++;
    if(map==maps[1])return ((unsigned)y*0x10203u)|0xff000000u;
    if(map!=maps[2])abort();
    return lowest+((unsigned)x*3u+(unsigned)y*7u)%(range+1);
}
void C2_HOOK_FASTCALL PathCat(char* dest,const char* first,const char* second) {
    if(first!=gApplication_path)abort();
    strcpy(dest,"C:/DATA/");strcat(dest,second);
}
void C2_HOOK_FASTCALL LoadAllImagesInDirectory(tBrender_storage* storage,const char* path) {
    if(storage!=&gMisc_storage_space||strcmp(path,"C:/DATA/fire"))abort();
}
void C2_NORETURN C2_HOOK_FASTCALL PDFatalError(const char* message) {(void)message;abort();}
'''
    source = SOURCE.read_text()
    driver += source[source.index("// GLOBAL:"):source.index("// FUNCTION:")]
    driver += "\n".join(extract(source, name) for name in ("InitTintedPolyStuff", "BuildTintedPolyMaterial", "GetRangeOfValuesFromPixelmap", "CreateTintedPoly", "BuildTintedPolyModel", "SetTintedPolyColour"))
    driver += r'''
static unsigned norm(unsigned value) {
    unsigned base=(unsigned)(uintptr_t)arena;
    return value>=base&&value<base+sizeof(arena)?value-base+0x1000000:value;
}
static void dump(const void* pointer,unsigned size) {
    unsigned i;for(i=0;i<size;i++)printf("%02x",((const unsigned char*)pointer)[i]);
}
static void normalize(void* pointer,unsigned offset) {
    unsigned value;memcpy(&value,(char*)pointer+offset,4);value=norm(value);memcpy((char*)pointer+offset,&value,4);
}
int main(int argc,char**argv) {
    int mode=atoi(argv[1]),a[6],value,i,j;
    unsigned camera;
    (void)argc;
    for(i=0;i<6;i++)a[i]=(int)strtoul(argv[i+2],NULL,0);
    range=(unsigned)atoi(argv[8]);lowest=(unsigned)atoi(argv[9]);
    for(i=0;i<(int)sizeof(gTintedPolys);i++)((unsigned char*)gTintedPolys)[i]=(unsigned char)(i*37+165);
    for(i=0;i<10;i++){gTintedPolys[i].used=1;gTintedPolys[i].actor=NULL;}
    gTintedPolys[0].used=0;gTintedPolys[0].actor=(br_actor*)0x12345678;
    gScreen=allocate("screen",sizeof(br_pixelmap));gScreen->width=640;gScreen->height=480;
    gTintedPolyCamera=BrActorAllocate(BR_ACTOR_CAMERA,NULL);
    maps[0]=allocate("map:testfire",sizeof(br_pixelmap));maps[0]->width=32;maps[0]->height=64;
    maps[1]=allocate("map:firepal1",sizeof(br_pixelmap));maps[1]->width=1;maps[1]->height=256;
    maps[2]=allocate("map:coolmapx",sizeof(br_pixelmap));maps[2]->width=(br_uint_16)atoi(argv[10]);maps[2]->height=(br_uint_16)atoi(argv[11]);
    gDefaultOpacity_TintedPoly=128;
    value=mode? (int)norm((unsigned)(uintptr_t)BuildTintedPolyModel(a[0],a[1],a[2],a[3],a[4],a[5])):CreateTintedPoly(a[0],a[1],a[2],a[3],4,a[4],0,0);
    camera=norm((unsigned)(uintptr_t)gTintedPolyCamera);
    for(i=0;i<10;i++){
        normalize(&gTintedPolys[i],offsetof(tTintedPoly,actor));normalize(&gTintedPolys[i],offsetof(tTintedPoly,material));
        normalize(&gTintedPolys[i],offsetof(tTintedPoly,model));normalize(&gTintedPolys[i],offsetof(tTintedPoly,material2));
        normalize(&gTintedPolys[i],offsetof(tTintedPoly,pixelmap));
    }
    printf("{\"value\":%d,\"camera\":%u,\"pixels\":%u,\"pool\":\"",value,camera,pixels);dump(gTintedPolys,sizeof(gTintedPolys));
    printf("\",\"allocations\":[");
    for(i=0;i<(int)count;i++){
        if(!strcmp(kinds[i],"actor")){normalize(pointers[i],0x18);normalize(pointers[i],0x1c);normalize(pointers[i],0x5c);}
        if(!strcmp(kinds[i],"model")){normalize(pointers[i],8);normalize(pointers[i],12);}
        if(!strcmp(kinds[i],"material"))normalize(pointers[i],0x40);
        if(!strcmp(kinds[i],"faces"))for(j=0;j<(int)sizes[i];j+=40)normalize(pointers[i],j+8);
        printf("%s[\"%s\",%u,%u,\"",i?",":"",kinds[i],norm((unsigned)(uintptr_t)pointers[i]),sizes[i]);dump(pointers[i],sizes[i]);printf("\"]");
    }
    printf("]}\n");return 0;
}
'''
    harness = out / "harness.c"
    harness.write_text(driver)
    includes = [ROOT / path for path in ("src/carma2/newcommon", "src/carma2/include", "src/carma2/hooks", "src/carma2/platform", "src/carma2/s3/include", "src/brender/include")]
    tools = args.msvc_root / "VC/Tools/MSVC"
    versions = sorted(path for path in tools.iterdir() if path.is_dir())
    msvc = versions[-1]
    sdk = args.msvc_root / "Windows Kits/10"
    sdk_version = sorted(path.name for path in (sdk / "Include").iterdir() if path.is_dir())[-1]
    env = dict(os.environ)
    env["PATH"] = str(msvc / "bin/Hostx64/x86") + os.pathsep + str(sdk / f"bin/{sdk_version}/x64") + os.pathsep + env["PATH"]
    env["INCLUDE"] = os.pathsep.join(map(str,[msvc / "include", *[sdk / f"Include/{sdk_version}/{part}" for part in ("ucrt","shared","um")]]))
    env["LIB"] = os.pathsep.join(map(str,[msvc / "lib/x86", *[sdk / f"Lib/{sdk_version}/{part}/x86" for part in ("ucrt","um")]]))
    commands = {
        "msvc": [str(msvc / "bin/Hostx64/x86/cl.exe"), "/nologo", "/O2", "/W3", "/WX", "/D_CRT_SECURE_NO_WARNINGS", *["/I"+str(path) for path in includes], str(harness), "/Fo"+str(out/"msvc.obj"), "/Fe"+str(out/"msvc.exe")],
        "mingw": [str(args.gcc), "-std=gnu89", "-O2", "-Wall", "-Wextra", "-Werror", *["-I"+str(path) for path in includes], str(harness), "-o", str(out/"mingw.exe")],
    }
    for compiler, command in commands.items():
        result = subprocess.run(command, env=env if compiler == "msvc" else None, text=True, capture_output=True)
        (out / f"{compiler}-build.txt").write_text(result.stdout+result.stderr)
        if result.returncode:
            raise RuntimeError(result.stdout+result.stderr)

    class ArithmeticFixture(x86.Fixture):
        def __init__(self, case):
            super().__init__(x86.original, "orig", case)
            self.u.mem_write(self.maps["coolmapx"]+0x34, struct.pack("<HH", *case["map_size"]))
        def hook(self, uc, address, size, context):
            if self.hooks.get(address) != "BrPixelmapPixelGet":
                return super().hook(uc,address,size,context)
            sp=uc.reg_read(x86.UC_X86_REG_ESP)
            pointer,x,y=[self.read(sp+4+i*4) for i in range(3)]
            key=next(k for k,v in self.maps.items() if v==pointer)
            self.calls.append(["BrPixelmapPixelGet",key,x,y])
            value=((y*0x10203)|0xff000000) if key=="firepal1" else self.case["lowest"]+(x*3+y*7)%(self.case["range"]+1)
            uc.reg_write(x86.UC_X86_REG_EAX,value)
            uc.reg_write(x86.UC_X86_REG_EIP,self.read(sp))
            uc.reg_write(x86.UC_X86_REG_ESP,sp+4)

    cases = []
    for n in (0,1,9,27,128,16777217,16777219,33554433,0x7fffffff,0xfffffffe,0xffffffff):
        for low,range_value in ((0,31),(7,5),(7,0)):
            cases.append(dict(mode="create",args=(3,7,641,479,4,n,0,0),lowest=low,range=range_value,map_size=(32,64)))
    for size in ((0,64),(32,0),(0,0)):
        cases.append(dict(mode="create",args=(0,0,32,64,4,9,0,0),lowest=7,range=5,map_size=size))
    for dims in ((0,0,640,480,1,1),(-7,13,641,479,4,4),(3,-5,-123,79,3,2),(0,0,0,480,1,1),(0,0,640,0,1,1),(0,0,0,0,1,1),(0,0,640,480,0,1),(0,0,640,480,1,-1),
                 (3,-5,16777217,16777219,4,4),(-7,13,16777219,33554433,3,2)):
        cases.append(dict(mode="model",args=dims,lowest=0,range=31,map_size=(32,64)))
    results = []
    for index, case in enumerate(cases):
        original = ArithmeticFixture(case).run()
        assert original["status"] == "returned"
        expected = {key:original[key] for key in ("value","camera","pool")}
        expected["pixels"] = sum(call[0]=="BrPixelmapPixelGet" for call in original["calls"])
        expected["allocations"] = [[kind,pointer,size,data[0] if kind=="material" else data] for kind,pointer,size,data in original["allocations"]]
        values = case["args"] if case["mode"]=="model" else [*case["args"][:4],case["args"][5],0]
        cli = ["1" if case["mode"]=="model" else "0", *map(str,values), str(case["range"]),str(case["lowest"]),*map(str,case["map_size"])]
        for compiler in commands:
            actual = json.loads(subprocess.check_output([str(out/f"{compiler}.exe"),*cli],text=True))
            if actual != expected:
                failure = out / f"failure-{compiler}-{index}.json"
                failure.write_text(json.dumps({"case":case,"original":expected,"native":actual},indent=2))
                raise AssertionError(f"Native {compiler} arithmetic case {index} differs: {failure}")
            results.append({"compiler":compiler,"case":case,"all_bytes_match":True})
    (out/"results.json").write_text(json.dumps(results,indent=2)+"\n")
    manifest = {"original_sha256":x86.ORIGINAL_HASH,"all_cases_passed":True,"case_count":len(results),"compiler_commands":commands,
                "source_sha256":{str(path.relative_to(ROOT)):hashlib.sha256(path.read_bytes()).hexdigest() for path in (SOURCE,ROOT/"src/carma2/newcommon/66-tintedpoly.h",ROOT/"src/carma2/include/rec2_types.h")},
                "harness_sha256":hashlib.sha256(driver.encode()).hexdigest(),"shared_fixture_sha256":hashlib.sha256(Path(x86.__file__).read_bytes()).hexdigest(),
                "scope":"Actual production C under modern MSVC/MinGW vs original x86; complete class4/mesh pool and allocation bytes, precision, unsigned input, zero range/NaN, empty maps."}
    (out/"verification.json").write_text(json.dumps(manifest,indent=2)+"\n")
    print(f"Passed {len(results)} native compiler/original tinted arithmetic cases")


if __name__ == "__main__":
    main()
