#!/usr/bin/env python3
"""Compare graphics startup state with the verified original x86 executable.

Compiles the current three production functions with real 32-bit repository
types. Compares complete transient bitmap, actor, camera and palette state,
including untouched bytes, for deterministic nonzero fixtures. BrActorAllocate
is a controlled fixture on both sides; this does not test BRender allocation.
Requires an i686 GCC compiler and Unicorn (paths may be supplied explicitly).
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import subprocess
import sys

from inventory_reconstruction_backlog import mask_c

ROOT = Path(__file__).resolve().parents[1]
ORIGINAL_HASH = "9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d"


def extract(path, name):
    source = path.read_text()
    clean = mask_c(source)
    marker = clean.index(" " + name + "(")
    start = clean.rfind("\n", 0, marker) + 1
    end = clean.index("{", marker) + 1
    depth = 1
    while depth:
        depth += (clean[end] == "{") - (clean[end] == "}")
        end += 1
    return source[start:end]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cc", default=str(ROOT / "build-tools/winlibs-i686/mingw32/bin/gcc.exe"))
    parser.add_argument("--dependency-dir", type=Path, default=ROOT / "build-audit/joystick/python-deps")
    parser.add_argument("--original", type=Path, default=ROOT / "build-reference/originals/CARMA2_HW.EXE")
    parser.add_argument("--output", type=Path, default=ROOT / "build-audit/graphics-startup-tests")
    args = parser.parse_args()
    sys.path.insert(0, str(args.dependency_dir.resolve()))
    from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
    from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EIP, UC_X86_REG_EAX

    binary = args.original.read_bytes()
    if hashlib.sha256(binary).hexdigest() != ORIGINAL_HASH:
        raise SystemExit("Original PE SHA-256 differs from verified binary")
    out = args.output.resolve()
    out.mkdir(parents=True, exist_ok=True)
    driver = r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "18-graphics2.h"
#include "29-raycast.h"
#include "rec2_macros.h"
typedef char check_layout[sizeof(void*)==4 && sizeof(tTransient_bm)==24 && sizeof(br_actor)==100 && sizeof(br_camera)==36 && offsetof(br_actor,type_data)==92 ? 1 : -1];
tTransient_bm gTransient_bitmaps[50];
tU32 gLast_palette_change;
int gPalette_index;
br_actor* gY_picking_camera;
static br_actor actor;
static br_camera camera;
static unsigned allocation_calls;
br_actor* C2_HOOK_CDECL BrActorAllocate(br_uint_8 type,void* data) {
    if(type!=BR_ACTOR_CAMERA || data!=NULL)exit(90);
    allocation_calls++;
    return &actor;
}
static void fill(void* data,size_t length,unsigned seed) {
    size_t i;
    for(i=0;i<length;i++)((unsigned char*)data)[i]=(unsigned char)(i*37+seed);
}
static void dump(const void* data,size_t length) {
    size_t i;
    for(i=0;i<length;i++)printf("%02x",((const unsigned char*)data)[i]);
}
'''
    entries = [("18-graphics2.c", "InitPaletteAnimate", 0x4b52a0),
               ("18-graphics2.c", "InitTransientBitmaps", 0x43dff0),
               ("29-raycast.c", "InitRayCasting", 0x4e3660)]
    driver += "\n".join(extract(ROOT / "src/carma2/newcommon" / path, name) for path, name, _ in entries)
    driver += r'''
int main(int argc,char** argv) {
    unsigned seed=(unsigned)atoi(argv[1]);
    (void)argc;
    fill(gTransient_bitmaps,sizeof(gTransient_bitmaps),seed);
    fill(&actor,sizeof(actor),seed);
    fill(&camera,sizeof(camera),seed);
    actor.type_data=&camera;
    gLast_palette_change=seed+1;
    gPalette_index=-(int)(seed+1);
    InitPaletteAnimate();
    InitTransientBitmaps();
    InitRayCasting();
    if(gY_picking_camera!=&actor || actor.type_data!=&camera) return 91;
    actor.type_data=NULL; /* Normalize the one fixture pointer. */
    printf("%u %u %d\n",allocation_calls,(unsigned)gLast_palette_change,gPalette_index);
    dump(gTransient_bitmaps,sizeof(gTransient_bitmaps));puts("");
    dump(&actor,sizeof(actor));puts("");
    dump(&camera,sizeof(camera));puts("");
    return 0;
}
'''
    harness = out / "harness.c"
    harness.write_text(driver)
    includes = ["src/carma2/newcommon", "src/carma2/include", "src/carma2/hooks",
                "src/carma2/s3/include", "src/brender/include"]
    subprocess.run([args.cc, "-std=gnu89", "-O2", "-Wall", "-Wextra", "-Werror",
                    *["-I" + str(ROOT / p) for p in includes], str(harness),
                    "-o", str(out / "harness.exe")], check=True)
    pe = struct.unpack_from("<I", binary, 0x3c)[0]
    count = struct.unpack_from("<H", binary, pe + 6)[0]
    table = pe + 24 + struct.unpack_from("<H", binary, pe + 20)[0]

    def original(seed):
        u = Uc(UC_ARCH_X86, UC_MODE_32)
        u.mem_map(0x400000, 0x400000)
        u.mem_map(0x1000000, 0x20000)
        for i in range(count):
            _, rva, size, offset = struct.unpack_from("<IIII", binary, table + 40 * i + 8)
            u.mem_write(0x400000 + rva, binary[offset:offset + size])

        def write(address, value):
            u.mem_write(address, struct.pack("<I", value & 0xffffffff))

        def read(address):
            return struct.unpack("<I", u.mem_read(address, 4))[0]

        actor, camera, stop, stack = 0x1001000, 0x1002000, 0x1010000, 0x101f000
        for address, size in [(0x67be98, 1200), (actor, 100), (camera, 36)]:
            u.mem_write(address, bytes((i * 37 + seed) % 256 for i in range(size)))
        write(actor + 92, camera)
        write(0x6923a4, seed + 1)
        write(0x6923b4, -(seed + 1))
        calls = []

        def hook(uc, address, size, user):
            if address != 0x51e1a0:
                return
            sp = uc.reg_read(UC_X86_REG_ESP)
            calls.append([read(sp + 4), read(sp + 8)])
            uc.reg_write(UC_X86_REG_EAX, actor)
            uc.reg_write(UC_X86_REG_EIP, read(sp))
            uc.reg_write(UC_X86_REG_ESP, sp + 4)

        u.hook_add(UC_HOOK_CODE, hook)
        for _, name, entry in entries:
            write(stack, stop)
            u.reg_write(UC_X86_REG_ESP, stack)
            u.emu_start(entry, stop, count=10000)
            if u.reg_read(UC_X86_REG_EIP) != stop:
                raise AssertionError(name + " exceeded instruction budget")
        assert calls == [[3, 0]]
        assert read(0x6a20d0) == actor and read(actor + 92) == camera
        write(actor + 92, 0)
        return [f"{len(calls)} {read(0x6923a4)} {read(0x6923b4)}",
                bytes(u.mem_read(0x67be98, 1200)).hex(),
                bytes(u.mem_read(actor, 100)).hex(), bytes(u.mem_read(camera, 36)).hex()]

    results = []
    for seed in (0, 1, 37, 127, 165, 254, 255, 1024):
        native = subprocess.check_output([str(out / "harness.exe"), str(seed)], text=True).splitlines()
        expected = original(seed)
        if native != expected:
            (out / f"failure-{seed}.json").write_text(json.dumps({"native": native, "original": expected}, indent=2))
            raise AssertionError(f"Graphics startup state differs for seed {seed}")
        results.append({"seed": seed, "functions": 3, "match": True})
    (out / "results.json").write_text(json.dumps(results, indent=2) + "\n")
    print(f"PASS: {len(results)} complete-state fixtures across three graphics startup routines")


if __name__ == "__main__":
    main()
