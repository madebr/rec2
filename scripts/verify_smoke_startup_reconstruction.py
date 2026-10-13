#!/usr/bin/env python3
"""Check smoke startup against original x86 and modern MSVC/MinGW production C.

Compares complete smoke records, allocated objects and mesh data, shared tokens,
registration snapshots and ordering. Allocator, loader, registry and fatal-error
boundaries are controlled fixtures. Optional --symbols checks the reconstructed
executable too, including the original unchecked second-model allocation fault.
This does not exercise the BRender allocator, renderer or image-loader internals.
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

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "src/carma2/newcommon/32-spark.c"
ORIGINAL_HASH = "9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d"
ADDRESSES = dict(InitSmokeStuff=0x4f9fc0, gBlend_model=0x74cf30, gBlend_model2=0x74cf94,
    gBlend_actor=0x74caac, gBR_smoke_structs=0x6a87f0, gSmoke_material_tokens=0x660148,
    gRender_palette=0x74a674, gDont_render_actor=0x761ce4, BrModelAllocate=0x5217f0,
    BrActorAllocate=0x51e1a0, BrMaterialAllocate=0x51f240, LoadPixelmap=0x48ec00,
    FatalError=0x44c230, BrMapAdd=0x51efa0, BrMaterialAdd=0x51ee50,
    BrModelAdd=0x51ed00, BrActorAdd=0x51dfe0)
HEAP, STACK, STOP, TOKEN, IDENTIFIER = 0x1000000, 0x1080000, 0x1081000, 0x2000100, 0x2000200


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--original", type=Path, default=ROOT / "build-reference/originals/CARMA2_HW.EXE")
    parser.add_argument("--recompiled", type=Path, default=ROOT / "build-matching/rec2.exe")
    parser.add_argument("--symbols", type=Path)
    parser.add_argument("--dependency-dir", type=Path, default=ROOT / "build-audit/joystick/python-deps")
    parser.add_argument("--gcc", type=Path, default=ROOT / "build-tools/winlibs-i686/mingw32/bin/gcc.exe")
    parser.add_argument("--msvc-root", type=Path, default=ROOT / "build-tools/msvc-modern")
    parser.add_argument("--output", type=Path, default=ROOT / "build-audit/smoke-startup-tests")
    args = parser.parse_args()
    sys.path.insert(0, str(args.dependency_dir.resolve()))
    from unicorn import Uc, UcError, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
    from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EIP, UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_FPCW
    from verify_tinted_native_reconstruction import extract

    binary = args.original.read_bytes()
    assert hashlib.sha256(binary).hexdigest() == ORIGINAL_HASH
    images = [(binary, ADDRESSES)]
    if args.symbols:
        mapping = json.loads(args.symbols.read_text())
        rebuilt = args.recompiled.read_bytes()
        assert mapping["original_sha256"] == ORIGINAL_HASH and mapping["recompiled_sha256"] == hashlib.sha256(rebuilt).hexdigest()
        assert mapping["format"] == 1
        mapped = {s["name"]: s["recomp"] for s in mapping["symbols"]}
        originals = {s["name"]: s["orig"] for s in mapping["symbols"]}
        assert all(originals[name] == address for name, address in ADDRESSES.items())
        images.append((rebuilt, {name: mapped[name] for name in ADDRESSES}))
    out = args.output.resolve()
    out.mkdir(parents=True, exist_ok=True)
    source = SOURCE.read_text()
    tokens = re.search(r"static br_token_value gSmoke_material_tokens\[3\] = \{.*?\n\};", source, re.S).group(0)
    driver = r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <setjmp.h>
#include <stddef.h>
#include "32-spark.h"
#include "52-errors.h"
#include "63-loading3.h"
#include "18-graphics2.h"
#include "globvars.h"
#include "rec2_macros.h"
typedef char layout_check[sizeof(void*)==4&&sizeof(tBRender_smoke)==36&&sizeof(br_model)==92&&sizeof(br_actor)==100&&sizeof(br_material)==156&&sizeof(br_vertex)==40&&sizeof(br_face)==40?1:-1];
br_model *gBlend_model,*gBlend_model2;
br_actor *gBlend_actor,*gDont_render_actor;
br_pixelmap* gRender_palette;
tBRender_smoke gBR_smoke_structs[35];
static unsigned arena[16384],used,nobjects,nevents,nmodels,nmaterials;
static void* pointers[100];static unsigned sizes[100];static const char* kinds[100];
static int fail_model,fail_actor,fail_material,fail_load,fatal=-1;
static unsigned width,height;
static jmp_buf escape;
'''
    driver += tokens + r'''
static unsigned norm(unsigned v) {
    unsigned base=(unsigned)(uintptr_t)arena;
    if(v==(unsigned)(uintptr_t)gSmoke_material_tokens)return 0x2000100;
    if(v>=base&&v<base+sizeof(arena))return v-base+0x1000000;
    return v;
}
static void dump(const void* p,unsigned n,const char* kind) {
    unsigned i,v;unsigned char data[1300];if(n>sizeof(data))abort();memcpy(data,p,n);
    for(i=0;i<n;i+=4){
        int pointer=(!strcmp(kind,"model")&&(i==8||i==12))||(!strcmp(kind,"material")&&(i==0x40||i==0x58))||(!strcmp(kind,"map")&&i==0x10)||(!strcmp(kind,"actor")&&(i==0x18||i==0x1c))||(!strcmp(kind,"pool")&&(i%36==28||i%36==32));
        if(pointer){memcpy(&v,data+i,4);v=norm(v);memcpy(data+i,&v,4);}
    }
    if(!strcmp(kind,"actor")){memcpy(&v,data+0x14,4);if(v!=0xa5a5a5a5u){if(strcmp((char*)(uintptr_t)v,"gBlend_actor"))abort();v=0x2000200;memcpy(data+0x14,&v,4);}}
    for(i=0;i<n;i++)printf("%02x",data[i]);
}
static void prefix(const char* name){printf("%s[\"%s\"",nevents++?",":"",name);}
static void* allocate(const char* kind,unsigned size) {
    void* p=(char*)arena+used;if(nobjects>=100||used+size>sizeof(arena))abort();
    pointers[nobjects]=p;sizes[nobjects]=size;kinds[nobjects++]=kind;memset(p,0xa5,size);used+=(size+15)&~15u;return p;
}
br_model* C2_HOOK_CDECL BrModelAllocate(const char* name,int nv,int nf) {
    br_model* m=NULL;int index=(int)nmodels++;
    if(index!=fail_model){m=allocate("model",sizeof(br_model));m->vertices=allocate("vertices",nv*sizeof(br_vertex));m->faces=allocate("faces",nf*sizeof(br_face));m->nvertices=(br_uint_16)nv;m->nfaces=(br_uint_16)nf;}
    prefix("BrModelAllocate");printf(",\"%s\",%d,%d,%u]",name,nv,nf,norm((unsigned)(uintptr_t)m));return m;
}
br_actor* C2_HOOK_CDECL BrActorAllocate(br_uint_8 type,void* data) {
    br_actor* a=fail_actor?NULL:allocate("actor",sizeof(br_actor));if(data)abort();prefix("BrActorAllocate");printf(",%u,%u]",type,norm((unsigned)(uintptr_t)a));return a;
}
br_material* C2_HOOK_CDECL BrMaterialAllocate(const char* name) {
    br_material* m=(int)nmaterials++==fail_material?NULL:allocate("material",sizeof(br_material));prefix("BrMaterialAllocate");printf(",\"%s\",%u]",name,norm((unsigned)(uintptr_t)m));return m;
}
br_pixelmap* C2_HOOK_FASTCALL LoadPixelmap(const char* name) {
    br_pixelmap* m=NULL;if(!fail_load){m=allocate("map",sizeof(br_pixelmap));m->width=(br_uint_16)width;m->height=(br_uint_16)height;}prefix("LoadPixelmap");printf(",\"%s\",%u]",name,norm((unsigned)(uintptr_t)m));return m;
}
br_pixelmap* C2_HOOK_CDECL BrMapAdd(br_pixelmap* m) {prefix("BrMapAdd");printf(",%u,\"",norm((unsigned)(uintptr_t)m));dump(m,sizeof(*m),"map");printf("\"]");return m;}
br_material* C2_HOOK_CDECL BrMaterialAdd(br_material* m) {prefix("BrMaterialAdd");printf(",%u,\"",norm((unsigned)(uintptr_t)m));dump(m,sizeof(*m),"material");printf("\"]");return m;}
br_model* C2_HOOK_CDECL BrModelAdd(br_model* m) {prefix("BrModelAdd");printf(",%u,\"",norm((unsigned)(uintptr_t)m));dump(m,sizeof(*m),"model");printf("\"]");return m;}
br_actor* C2_HOOK_CDECL BrActorAdd(br_actor* parent,br_actor* a) {prefix("BrActorAdd");printf(",%u,%u]",norm((unsigned)(uintptr_t)parent),norm((unsigned)(uintptr_t)a));return a;}
void C2_NORETURN C2_HOOK_CDECL FatalError(int code,...) {
    prefix("FatalError");printf(",%d",code);if(code==79){va_list ap;va_start(ap,code);printf(",\"%s\"",va_arg(ap,char*));va_end(ap);}printf("]");fatal=code;longjmp(escape,1);
}
'''
    driver += extract(source, "InitSmokeStuff") + r'''
int main(int argc,char**argv) {
    unsigned i;int repeat,j;(void)argc;
    width=(unsigned)atoi(argv[1]);height=(unsigned)atoi(argv[2]);fail_model=atoi(argv[3]);fail_actor=atoi(argv[4]);fail_material=atoi(argv[5]);fail_load=atoi(argv[6]);repeat=atoi(argv[7]);
    memset(gBR_smoke_structs,0xa5,sizeof(gBR_smoke_structs));gBlend_model=(br_model*)0xdeadbeef;gBlend_model2=(br_model*)0xdeadbeef;gBlend_actor=(br_actor*)0xdeadbeef;
    gRender_palette=(br_pixelmap*)0x12345000;gDont_render_actor=(br_actor*)0x12346000;gSmoke_material_tokens[1].v.x=atoi(argv[8]);
    printf("{\"calls\":[");if(!setjmp(escape))for(j=0;j<repeat;j++)InitSmokeStuff();
    printf("],\"status\":\"%s\",\"globals\":[%u,%u,%u],\"pool\":\"",fatal<0?"returned":"fatal",norm((unsigned)(uintptr_t)gBlend_model),norm((unsigned)(uintptr_t)gBlend_model2),norm((unsigned)(uintptr_t)gBlend_actor));dump(gBR_smoke_structs,sizeof(gBR_smoke_structs),"pool");
    printf("\",\"tokens\":\"");dump(gSmoke_material_tokens,sizeof(gSmoke_material_tokens),"tokens");printf("\",\"objects\":[");
    for(i=0;i<nobjects;i++){printf("%s[\"%s\",%u,%u,\"",i?",":"",kinds[i],norm((unsigned)(uintptr_t)pointers[i]),sizes[i]);dump(pointers[i],sizes[i],kinds[i]);printf("\"]");}printf("]}\n");return 0;
}
'''
    harness = out / "harness.c"
    harness.write_text(driver)
    includes = [ROOT / p for p in ("src/carma2/newcommon", "src/carma2/include", "src/carma2/hooks", "src/carma2/s3/include", "src/brender/include")]
    msvc = sorted((args.msvc_root / "VC/Tools/MSVC").iterdir())[-1]
    sdk = args.msvc_root / "Windows Kits/10"
    version = sorted(p.name for p in (sdk / "Include").iterdir())[-1]
    env = dict(os.environ)
    env["PATH"] = str(msvc / "bin/Hostx64/x86") + os.pathsep + env["PATH"]
    env["INCLUDE"] = os.pathsep.join(map(str, [msvc / "include", *[sdk / f"Include/{version}/{p}" for p in ("ucrt", "shared", "um")]]))
    env["LIB"] = os.pathsep.join(map(str, [msvc / "lib/x86", *[sdk / f"Lib/{version}/{p}/x86" for p in ("ucrt", "um")]]))
    commands = {
        "msvc": [str(msvc / "bin/Hostx64/x86/cl.exe"), "/nologo", "/O2", "/W3", "/WX", "/D_CRT_SECURE_NO_WARNINGS", *["/I"+str(p) for p in includes], str(harness), "/Fo"+str(out / "harness.obj"), "/Fe"+str(out / "msvc.exe")],
        "mingw": [str(args.gcc), "-std=gnu89", "-O2", "-Wall", "-Wextra", "-Werror", *["-I"+str(p) for p in includes], str(harness), "-o", str(out / "mingw.exe")]
    }
    for compiler, command in commands.items():
        result = subprocess.run(command, env=env if compiler == "msvc" else None, text=True, capture_output=True)
        (out / f"{compiler}-build.log").write_text(result.stdout + result.stderr)
        if result.returncode: raise AssertionError(result.stdout + result.stderr)

    def execute(data, addresses, case):
        width, height, fail_model, fail_actor, fail_material, fail_load, repeat, opacity = case
        u = Uc(UC_ARCH_X86, UC_MODE_32)
        u.mem_map(0x400000, 0x500000)
        u.mem_map(HEAP, 0x90000)
        pe = struct.unpack_from("<I", data, 0x3c)[0]
        table = pe + 24 + struct.unpack_from("<H", data, pe+20)[0]
        for i in range(struct.unpack_from("<H", data, pe+6)[0]):
            _, rva, size, offset = struct.unpack_from("<IIII", data, table+40*i+8)
            u.mem_write(0x400000+rva, data[offset:offset+size])
        def wr(p, v): u.mem_write(p, struct.pack("<I", v & 0xffffffff))
        def rd(p): return struct.unpack("<I", u.mem_read(p, 4))[0]
        def string(p):
            raw = bytearray()
            while len(raw) < 256:
                ch = u.mem_read(p+len(raw), 1)[0]
                if not ch: return raw.decode("ascii")
                raw.append(ch)
            raise AssertionError("Unterminated string")
        def snapshot(p, n, kind):
            raw = bytearray(u.mem_read(p, n))
            if kind == "material" and struct.unpack_from("<I", raw, 0x58)[0] == addresses["gSmoke_material_tokens"]:
                struct.pack_into("<I", raw, 0x58, TOKEN)
            if kind == "actor":
                name = struct.unpack_from("<I", raw, 0x14)[0]
                if name != 0xa5a5a5a5:
                    assert string(name) == "gBlend_actor"
                    struct.pack_into("<I", raw, 0x14, IDENTIFIER)
            return raw.hex()
        for name in ("gBlend_model", "gBlend_model2", "gBlend_actor"): wr(addresses[name], 0xdeadbeef)
        wr(addresses["gRender_palette"], 0x12345000)
        wr(addresses["gDont_render_actor"], 0x12346000)
        wr(addresses["gSmoke_material_tokens"]+12, opacity)
        u.mem_write(addresses["gBR_smoke_structs"], b"\xa5"*(35*36))
        cursor, models, materials = HEAP, 0, 0
        objects, calls, status = [], [], "returned"
        def allocate(kind, n):
            nonlocal cursor
            p = cursor
            cursor += (n+15)&~15
            assert cursor < HEAP+0x10000
            u.mem_write(p, b"\xa5"*n)
            objects.append([kind, p, n])
            return p
        hooks = {addresses[name]: name for name in ADDRESSES if name.startswith("Br") or name in ("LoadPixelmap", "FatalError")}
        def hook(uc, at, size, context):
            nonlocal models, materials, status
            if at not in hooks: return
            name = hooks[at]
            sp = uc.reg_read(UC_X86_REG_ESP)
            a = [rd(sp+4+4*i) for i in range(3)]
            value = 0
            if name == "BrModelAllocate":
                if models != fail_model:
                    value = allocate("model", 92)
                    wr(value+8, allocate("vertices", a[1]*40))
                    wr(value+12, allocate("faces", a[2]*40))
                    u.mem_write(value+16, struct.pack("<HH", a[1], a[2]))
                models += 1
                calls.append([name, string(a[0]), a[1], a[2], value])
            elif name == "BrActorAllocate":
                assert a[1] == 0
                value = 0 if fail_actor else allocate("actor", 100)
                calls.append([name, a[0], value])
            elif name == "BrMaterialAllocate":
                value = 0 if materials == fail_material else allocate("material", 156)
                materials += 1
                calls.append([name, string(a[0]), value])
            elif name == "LoadPixelmap":
                value = 0 if fail_load else allocate("map", 68)
                if value: u.mem_write(value+0x34, struct.pack("<HH", width, height))
                calls.append([name, string(uc.reg_read(UC_X86_REG_ECX)), value])
            elif name == "FatalError":
                calls.append([name, a[0]] + ([string(a[1])] if a[0] == 79 else []))
                status = "fatal"
                uc.emu_stop()
                return
            elif name == "BrActorAdd":
                calls.append([name, a[0], a[1]])
                value = a[1]
            else:
                kind, n = {"BrMapAdd": ("map", 68), "BrMaterialAdd": ("material", 156), "BrModelAdd": ("model", 92)}[name]
                calls.append([name, a[0], snapshot(a[0], n, kind)])
                value = a[0]
            uc.reg_write(UC_X86_REG_EAX, value)
            uc.reg_write(UC_X86_REG_EIP, rd(sp))
            uc.reg_write(UC_X86_REG_ESP, sp+4)
        u.hook_add(UC_HOOK_CODE, hook)
        u.reg_write(UC_X86_REG_FPCW, 0x37f)
        for _ in range(repeat):
            wr(STACK, STOP)
            u.reg_write(UC_X86_REG_ESP, STACK)
            try: u.emu_start(addresses["InitSmokeStuff"], STOP, count=100000)
            except UcError as error: status = f"fault:{error.errno}"
            if status != "returned": break
            assert u.reg_read(UC_X86_REG_EIP) == STOP
        return dict(status=status, calls=calls,
            globals=[rd(addresses[name]) for name in ("gBlend_model", "gBlend_model2", "gBlend_actor")],
            pool=snapshot(addresses["gBR_smoke_structs"], 35*36, "pool"),
            tokens=snapshot(addresses["gSmoke_material_tokens"], 24, "tokens"),
            objects=[[kind, p, n, snapshot(p, n, kind)] for kind, p, n in objects])

    cases = [(w, h, -1, 0, -1, 0, 1, 75<<16) for w, h in ((256, 256), (1, 1), (3, 7), (7, 3), (0, 256), (256, 0), (0, 0), (65535, 32768))]
    cases += [(3, 7, -1, 0, -1, 0, 2, -12345), (3, 7, -1, 0, -1, 0, 1, 0)]
    cases += [(256, 256, m, a, -1, l, 1, 75<<16) for m, a, l in ((0, 0, 0), (1, 0, 0), (-1, 1, 0), (-1, 0, 1))]
    cases += [(256, 256, -1, 0, i, 0, 1, 75<<16) for i in range(35)]
    results = []
    for index, case in enumerate(cases):
        expected = execute(*images[0], case)
        if len(images) > 1:
            actual = execute(*images[1], case)
            if actual != expected:
                (out / f"failure-x86-{index}.json").write_text(json.dumps(dict(case=case, original=expected, rebuilt=actual), indent=2))
                raise AssertionError(f"Recompiled x86 case {index} differs")
            results.append(dict(case=case, compiler="recompiled-x86", match=True))
        # Null second-model dereference is verified only in the x86 emulators.
        if expected["status"].startswith("fault:"):
            assert case[2] == 1
            continue
        for compiler in commands:
            actual = json.loads(subprocess.check_output([str(out / f"{compiler}.exe"), *map(str, case)], text=True))
            if actual != expected:
                (out / f"failure-{compiler}-{index}.json").write_text(json.dumps(dict(case=case, original=expected, native=actual), indent=2))
                raise AssertionError(f"Native {compiler} case {index} differs")
            results.append(dict(case=case, compiler=compiler, match=True))
    (out / "verification.json").write_text(json.dumps(dict(original_sha256=ORIGINAL_HASH,
        source_sha256=hashlib.sha256(SOURCE.read_bytes()).hexdigest(), compiler_commands=commands,
        reconstructed_x86_checked=bool(args.symbols), cases=results), indent=2)+"\n")
    print(f"PASS: {len(results)} smoke startup comparisons, complete objects/meshes/materials and failure ordering")


if __name__ == "__main__":
    main()
