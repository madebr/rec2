#!/usr/bin/env python3
"""Compare actual palette runtime C with the original executable.

Compiles eight production entries and their existing real palette helpers
under modern MSVC and MinGW, using repository 32-bit types. Platform time and
palette upload are explicit boundaries. All palette/object bytes, global
state, and ordered time/upload snapshots are compared with original x86.
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
HASH = "9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d"
NAMES = ["RevertPalette", "MungePalette", "ResetPalette", "SetFadedPalette", "FadePaletteDown", "FadePaletteUp", "EnsurePaletteUp", "EnsureRenderPalette"]
ENTRIES = [0x4b52b0, 0x4b5320, 0x4b5330, 0x4b5390, 0x4b5460, 0x4b5470, 0x4b55f0, 0x4b5770]
TIMES = [[0, 0, 250, 499, 500], [0xffffffff, 0xffffffff, 0, 249, 498, 499], [0, 500], [1000, 900, 1000, 1500], [0, 0x80000000, 0x80000001, 500], [0x80000000, 0, 0x800001f4], [0xffffff00, 0xf4]]
FIELDS = "mode degree faded munged alias time_pattern mutate repeat seed".split()
SCALAR_GLOBALS = [0x6923a4, 0x6923b4, 0x6923c8, 0x6923c0, 0x74cf04]
POINTER_GLOBALS = [0x74a674, 0x74a678, 0x6923ac, 0x6923a8, 0x6923b8, 0x74a680]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--original", type=Path, default=ROOT / "build-reference/originals/CARMA2_HW.EXE")
    parser.add_argument("--dependency-dir", type=Path, default=ROOT / "build-audit/joystick/python-deps")
    parser.add_argument("--gcc", type=Path, default=ROOT / "build-tools/winlibs-i686/mingw32/bin/gcc.exe")
    parser.add_argument("--msvc-root", type=Path, default=ROOT / "build-tools/msvc-modern")
    parser.add_argument("--output", type=Path, default=ROOT / "build-audit/palette-runtime-tests")
    parser.add_argument("--symbols", type=Path, help="Also compare a rebuilt matching PE using its hash-checked map")
    parser.add_argument("--recompiled", type=Path, default=ROOT / "build-matching/rec2.exe")
    parser.add_argument("--case-index", type=int, action="append")
    args = parser.parse_args()
    sys.path.insert(0, str(args.dependency_dir.resolve()))
    from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
    from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EIP, UC_X86_REG_ECX, UC_X86_REG_EAX
    binary = args.original.read_bytes()
    if hashlib.sha256(binary).hexdigest() != HASH:
        raise SystemExit("Original executable hash differs")
    rebuilt = None
    addresses = {}
    if args.symbols is not None:
        mapping = json.loads(args.symbols.read_text())
        rebuilt = args.recompiled.read_bytes()
        if mapping["format"] != 1 or mapping["original_sha256"] != HASH or mapping["recompiled_sha256"] != hashlib.sha256(rebuilt).hexdigest():
            raise SystemExit("Stale or incompatible matching symbol map")
        addresses = {s["orig"]: s["recomp"] for s in mapping["symbols"]}
        required = ENTRIES + SCALAR_GLOBALS + POINTER_GLOBALS + [0x51d410, 0x51c840]
        if any(address not in addresses for address in required):
            raise SystemExit("Matching symbol map is missing palette entries, globals or platform boundaries")
    out = args.output.resolve()
    out.mkdir(parents=True, exist_ok=True)
    source = SOURCE.read_text()
    driver = r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "18-graphics2.h"
#include "platform.h"
typedef char check_layout[sizeof(void*)==4&&sizeof(br_pixelmap)==68&&offsetof(br_pixelmap,pixels)==8?1:-1];
enum {MODE,DEGREE,FADED,MUNGED,ALIAS,TIME_PATTERN,MUTATE,REPEAT,SEED};
static unsigned a[9],time_calls,event_count;
static const unsigned times[7][6]={{0,0,250,499,500,500},{0xffffffff,0xffffffff,0,249,498,499},{0,500,500,500,500,500},{1000,900,1000,1500,1500,1500},{0,0x80000000,0x80000001,500,500,500},{0x80000000,0,0x800001f4,0x800001f4,0x800001f4,0x800001f4},{0xffffff00,0xf4,0xf4,0xf4,0xf4,0xf4}};
typedef struct {br_pixelmap maps[4];br_colour pixels[4][256],current_pixels[256];} fixture_heap;
static fixture_heap heap;
static unsigned events[64][9];
static unsigned char images[64][1024];
'''
    driver += source[source.index("// GLOBAL:"):source.index("// FUNCTION:")]
    driver += r'''
static unsigned norm(unsigned value) {
    unsigned base=(unsigned)(uintptr_t)&heap;
    return value>=base&&value<base+sizeof(heap)?value-base+0x1000000:value;
}
static void record(unsigned code,br_pixelmap* palette,unsigned value) {
    unsigned* e;
    if(event_count>=64)abort();
    e=events[event_count];e[0]=code;e[1]=norm((unsigned)(uintptr_t)palette);e[2]=value;e[3]=time_calls;
    e[4]=gLast_palette_change;e[5]=(unsigned)gPalette_index;e[6]=(unsigned)gFaded_palette;e[7]=(unsigned)gPalette_munged;e[8]=(unsigned)gPalette_changed;
    if(palette)memcpy(images[event_count],palette->pixels,1024);
    event_count++;
}
tU32 C2_HOOK_FASTCALL PDGetTotalTime(void) {
    unsigned value=times[a[TIME_PATTERN]][time_calls<6?time_calls:5];
    record(1,NULL,value);time_calls++;return value;
}
void C2_HOOK_FASTCALL PDSetPalette(br_pixelmap* palette) {
    record(2,palette,0);
    if(a[MUTATE]){gFaded_palette=3;gPalette_munged=0x80;gPalette_changed=77;}
}
'''
    driver += "\n".join(extract(source, name) for name in ["DRSetPalette3", "DRSetPalette2", "DRSetPalette", "InitPaletteAnimate", *NAMES])
    driver += r'''
static void dump(const void* p,unsigned size) {
    unsigned i;for(i=0;i<size;i++)printf("%02x",((const unsigned char*)p)[i]);
}
int main(int argc,char**argv) {
    unsigned i,j,render,current,original,scratch,pixel;
    if(argc==2){printf("[%u,%u,%u]\n",(unsigned)offsetof(fixture_heap,pixels),(unsigned)offsetof(fixture_heap,current_pixels),(unsigned)sizeof(heap));return 0;}
    if(argc!=10)abort();
    for(i=0;i<9;i++)a[i]=(unsigned)strtoul(argv[i+1],NULL,0);
    memset(&heap,(int)a[SEED],sizeof(heap));
    for(i=0;i<4;i++){
        heap.maps[i].pixels=heap.pixels[i];
        for(j=0;j<256;j++)heap.pixels[i][j]=((i+1)<<24)|((j*37+i*17)&255)<<16|((j*53+i*13)&255)<<8|((j*91+i*31)&255);
    }
    render=0;current=a[ALIAS]==1?0:1;original=a[ALIAS]==3?0:2;scratch=a[ALIAS]==2?0:a[ALIAS]==5?current:3;
    gRender_palette=&heap.maps[render];gCurrent_palette=&heap.maps[current];gOrig_render_palette=&heap.maps[original];gScratch_palette=&heap.maps[scratch];
    gScratch_pixels=heap.pixels[scratch];gCurrent_palette_pixels=(char*)(a[ALIAS]==4?heap.pixels[0]:heap.current_pixels);
    gLast_palette_change=0xffffffff;gPalette_index=-7;gFaded_palette=(int)a[FADED];gPalette_munged=(int)a[MUNGED];gPalette_changed=123;
    for(j=0;j<a[REPEAT];j++)switch(a[MODE]){
    case 0:RevertPalette();break;case 1:MungePalette();break;case 2:ResetPalette();break;case 3:SetFadedPalette((int)a[DEGREE]);break;
    case 4:FadePaletteDown();break;case 5:FadePaletteUp();break;case 6:EnsurePaletteUp();break;case 7:EnsureRenderPalette();break;
    }
    for(i=0;i<4;i++){pixel=norm((unsigned)(uintptr_t)heap.maps[i].pixels);memcpy(&heap.maps[i].pixels,&pixel,4);}
    printf("{\"heap\":\"");dump(&heap,sizeof(heap));
    printf("\",\"globals\":[%u,%u,%u,%u,%u",gLast_palette_change,(unsigned)gPalette_index,(unsigned)gFaded_palette,(unsigned)gPalette_munged,(unsigned)gPalette_changed);
    printf(",%u,%u,%u,%u,%u,%u],\"events\":[",norm((unsigned)(uintptr_t)gRender_palette),norm((unsigned)(uintptr_t)gCurrent_palette),norm((unsigned)(uintptr_t)gOrig_render_palette),norm((unsigned)(uintptr_t)gScratch_palette),norm((unsigned)(uintptr_t)gScratch_pixels),norm((unsigned)(uintptr_t)gCurrent_palette_pixels));
    for(i=0;i<event_count;i++){printf("%s[[",i?",":"");for(j=0;j<9;j++)printf("%s%u",j?",":"",events[i][j]);printf("],\"");dump(images[i],1024);printf("\"]");}
    printf("]}\n");return 0;
}
'''
    harness = out / "harness.c"
    harness.write_text(driver)
    includes = [ROOT / path for path in ("src/carma2/newcommon", "src/carma2/include", "src/carma2/hooks", "src/carma2/platform", "src/carma2/s3/include", "src/brender/include")]
    msvc = sorted((args.msvc_root / "VC/Tools/MSVC").iterdir())[-1]
    sdk = args.msvc_root / "Windows Kits/10"
    version = sorted((sdk / "Include").iterdir())[-1].name
    env = dict(os.environ)
    env["PATH"] = str(msvc / "bin/Hostx64/x86") + os.pathsep + str(sdk / f"bin/{version}/x64") + os.pathsep + env["PATH"]
    env["INCLUDE"] = os.pathsep.join(map(str, [msvc / "include", *[sdk / f"Include/{version}/{part}" for part in ("ucrt", "shared", "um")]]))
    env["LIB"] = os.pathsep.join(map(str, [msvc / "lib/x86", *[sdk / f"Lib/{version}/{part}/x86" for part in ("ucrt", "um")]]))
    commands = {
        "msvc": [str(msvc / "bin/Hostx64/x86/cl.exe"), "/nologo", "/O2", "/W3", "/WX", "/D_CRT_SECURE_NO_WARNINGS", *["/I" + str(path) for path in includes], str(harness), "/Fo" + str(out / "msvc.obj"), "/Fe" + str(out / "msvc.exe")],
        "mingw": [str(args.gcc), "-std=gnu89", "-O2", "-Wall", "-Wextra", "-Werror", *["-I" + str(path) for path in includes], str(harness), "-o", str(out / "mingw.exe")],
    }
    schemas = []
    for compiler, command in commands.items():
        result = subprocess.run(command, env=env if compiler == "msvc" else None, text=True, capture_output=True)
        (out / f"{compiler}-build.txt").write_text(result.stdout + result.stderr)
        if result.returncode:
            raise RuntimeError(result.stdout + result.stderr)
        schemas.append(json.loads(subprocess.check_output([str(out / f"{compiler}.exe"), "schema"], text=True)))
    assert schemas[0] == schemas[1]
    pixels, current_pixels, heap_size = schemas[0]
    def original(c, image=binary, address_map=None):
        address_map = address_map or {}
        reverse = {value: key for key, value in address_map.items()}
        pe = struct.unpack_from("<I", image, 60)[0]
        sections = struct.unpack_from("<H", image, pe + 6)[0]
        table = pe + 24 + struct.unpack_from("<H", image, pe + 20)[0]
        image_base = struct.unpack_from("<I", image, pe + 52)[0]
        image_size = struct.unpack_from("<I", image, pe + 80)[0]
        u = Uc(UC_ARCH_X86, UC_MODE_32)
        u.mem_map(image_base, (image_size + 4095) & ~4095);u.mem_map(0x1000000, 0x10000)
        for i in range(sections):
            _, rva, size, offset = struct.unpack_from("<IIII", image, table + i * 40 + 8)
            u.mem_write(image_base + rva, image[offset:offset + size])
        base = 0x1000000

        def wr(address, value):
            u.mem_write(address_map.get(address, address), struct.pack("<I", value & 0xffffffff))

        def rd(address):
            return struct.unpack("<I", u.mem_read(address_map.get(address, address), 4))[0]

        u.mem_write(base, bytes([c["seed"]]) * heap_size)
        for i in range(4):
            wr(base + i * 68 + 8, base + pixels + i * 1024)
            for j in range(256):
                wr(base + pixels + i * 1024 + j * 4, ((i + 1) << 24) | ((j * 37 + i * 17) & 255) << 16 | ((j * 53 + i * 13) & 255) << 8 | ((j * 91 + i * 31) & 255))
        render, current, orig, scratch = 0, 0 if c["alias"] == 1 else 1, 0 if c["alias"] == 3 else 2, 0 if c["alias"] == 2 else (0 if c["alias"] == 1 else 1) if c["alias"] == 5 else 3
        for address, value in zip(POINTER_GLOBALS, [base + render * 68, base + current * 68, base + orig * 68, base + scratch * 68, base + pixels + scratch * 1024, base + (pixels if c["alias"] == 4 else current_pixels)]):
            wr(address, value)
        for address, value in zip(SCALAR_GLOBALS, [0xffffffff, -7, c["faded"], c["munged"], 123]):
            wr(address, value)
        events = []
        time_calls = 0

        def hook(uc, address, size, context):
            nonlocal time_calls
            address = reverse.get(address, address)
            sp = uc.reg_read(UC_X86_REG_ESP)
            palette = 0
            value = 0
            if address == 0x51d410:
                times = TIMES[c["time_pattern"]]
                value = times[min(time_calls, len(times) - 1)]
                code = 1
            else:
                palette = uc.reg_read(UC_X86_REG_ECX)
                code = 2
            event = [[code, palette, value, time_calls, *[rd(addr) for addr in SCALAR_GLOBALS]], bytes(uc.mem_read(rd(palette + 8), 1024)).hex() if palette else "00" * 1024]
            events.append(event)
            if code == 1:
                time_calls += 1
            elif c["mutate"]:
                wr(0x6923c8, 3);wr(0x6923c0, 0x80);wr(0x74cf04, 77)
            uc.reg_write(UC_X86_REG_EAX, value)
            uc.reg_write(UC_X86_REG_EIP, rd(sp))
            uc.reg_write(UC_X86_REG_ESP, sp + 4)

        for entry in (0x51d410, 0x51c840):
            target = address_map.get(entry, entry)
            u.hook_add(UC_HOOK_CODE, hook, begin=target, end=target)
        for repeat in range(c["repeat"]):
            wr(0x100e000, 0x100f000)
            u.reg_write(UC_X86_REG_ESP, 0x100e000)
            u.reg_write(UC_X86_REG_ECX, c["degree"] & 0xffffffff)
            u.emu_start(address_map.get(ENTRIES[c["mode"]], ENTRIES[c["mode"]]), 0x100f000, count=1000000)
            assert u.reg_read(UC_X86_REG_EIP) == 0x100f000
        return {"heap": bytes(u.mem_read(base, heap_size)).hex(), "globals": [rd(addr) for addr in SCALAR_GLOBALS + POINTER_GLOBALS], "events": events}

    def case(mode, **kw):
        c = dict(mode=mode, degree=128, faded=0, munged=0, alias=0, time_pattern=0, mutate=0, repeat=1, seed=165)
        c.update(kw)
        return c

    cases = [case(mode, faded=faded, munged=munged, alias=alias, repeat=2) for mode in (0, 1, 2, 4, 7) for faded in (0, 1, -7) for munged in (0, 1, 0x80) for alias in range(6)]
    cases += [case(3, degree=degree, faded=faded, alias=alias, repeat=2) for degree in (0, 1, 128, 255, 256, 257, -1, 0x7fffffff, 0x80000000) for faded in (0, 1) for alias in range(6)]
    cases += [case(mode, faded=faded, munged=0x80, alias=alias, time_pattern=time_pattern, mutate=mutate) for mode in (5, 6) for faded in (0, 1, -7) for alias in range(6) for time_pattern in range(7) for mutate in (0, 1)]
    if args.case_index is not None and any(index < 0 or index >= len(cases) for index in args.case_index):
        parser.error(f"--case-index must be between 0 and {len(cases) - 1}")
    results = []
    for index, c in enumerate(cases):
        if args.case_index is not None and index not in args.case_index:
            continue
        expected = original(c)
        if rebuilt is not None:
            actual = original(c, rebuilt, addresses)
            if actual != expected:
                failure = out / f"failure-matching-{index}.json"
                failure.write_text(json.dumps({"case": c, "original": expected, "matching": actual}, indent=2))
                raise AssertionError(f"Palette matching case {index} differs: {failure}")
            results.append({"compiler": "matching PE", "case_index": index, "case": c, "all_bytes_calls_match": True})
        for compiler in commands:
            actual = json.loads(subprocess.check_output([str(out / f"{compiler}.exe"), *[str(c[key] & 0xffffffff) for key in FIELDS]], text=True))
            if actual != expected:
                failure = out / f"failure-{compiler}-{index}.json"
                failure.write_text(json.dumps({"case": c, "original": expected, "native": actual}, indent=2))
                raise AssertionError(f"Palette {compiler} case {index} differs: {failure}")
            results.append({"compiler": compiler, "case_index": index, "case": c, "all_bytes_calls_match": True})
        if (index + 1) % 100 == 0:
            print(f"Verified {index + 1}/{len(cases)} palette scenarios", flush=True)
    (out / "results.json").write_text(json.dumps(results, indent=2) + "\n")
    manifest = {"original_sha256": HASH, "all_cases_passed": True, "case_count": len(results), "complete_suite": args.case_index is None,
                "functions": dict(zip(NAMES, map(hex, ENTRIES))), "type_schema": schemas[0], "compiler_commands": commands,
                "source_sha256": {str(path.relative_to(ROOT)): hashlib.sha256(path.read_bytes()).hexdigest() for path in (SOURCE, ROOT / "src/carma2/newcommon/18-graphics2.h", ROOT / "src/carma2/include/rec2_types.h")},
                "harness_sha256": hashlib.sha256(driver.encode()).hexdigest(), "limitations": "Controlled platform time/upload boundaries; no physical display integration. The two single-RET entries are verified as byte/state-preserving no-ops."}
    if rebuilt is not None:
        manifest["recompiled_sha256"] = hashlib.sha256(rebuilt).hexdigest()
        manifest["symbols_sha256"] = hashlib.sha256(args.symbols.read_bytes()).hexdigest()
    (out / "verification.json").write_text(json.dumps(manifest, indent=2) + "\n")
    print(f"Passed {len(results)} native/original palette comparisons")


if __name__ == "__main__":
    main()
