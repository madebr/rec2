#!/usr/bin/env python3
"""Compare smoke shade setup with the hash-verified original x86 executable.

Compiles the five production bodies with repository 32-bit types. Compares the
complete shade arrays and globals, cache behavior, exact parser/service order
and arguments. Parser and shade-generator callees are controlled fixtures;
their implementations and rendering are outside this test's scope.
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
SOURCE = ROOT / "src/carma2/newcommon/32-spark.c"
ORIGINAL_HASH = "9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d"
NAMES = ["GenerateSmokeShades", "GenerateItFoxShadeTable", "GetSmokeShadeTables", "FreeSmokeShadeTables", "ResetSparks"]
ENTRIES = [0x4fb910, 0x4fb9c0, 0x4fdb70, 0x4fdc00, 0x4f87a0]


def extract(source, name):
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
    parser.add_argument("--output", type=Path, default=ROOT / "build-audit/smoke-shades-tests")
    args = parser.parse_args()
    sys.path.insert(0, str(args.dependency_dir.resolve()))
    from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
    from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EIP, UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX

    binary = args.original.read_bytes()
    if hashlib.sha256(binary).hexdigest() != ORIGINAL_HASH:
        raise SystemExit("Unexpected original executable SHA-256")
    out = args.output.resolve()
    out.mkdir(parents=True, exist_ok=True)
    source = SOURCE.read_text()
    driver = r'''
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "32-spark.h"
#include "08-loading1.h"
#include "18-graphics2.h"
#include "41-utility.h"
typedef char layout_check[sizeof(void*)==4&&sizeof(br_scalar)==4?1:-1];
int gShade_list[16],gNum_dust_tables,gSpark_flags;
int* gShade_table=&gShade_list[8];
br_pixelmap *gIt_shade_table,*gRender_palette;
static int separate[16],input_count,row,pattern,return_null,change_count;
static unsigned events[512],event_count;
static void event(unsigned value) {if(event_count>=512)abort();events[event_count++]=value;}
static void check_file(FILE* f) {if(f!=(FILE*)0x2000100)abort();}
static unsigned colour(int column) {
    static const unsigned unusual[8]={0,255,256,0xffffffffu,0x12345678,0x80000000u,0xabcdef,0x7fffffffu};
    return pattern?unusual[(row+column*3)%8]:(unsigned)(row*17+column*31);
}
int C2_HOOK_FASTCALL GetAnInt(FILE* f) {check_file(f);event(1);return input_count;}
void C2_HOOK_FASTCALL PossibleService(void) {event(2);if(change_count)gNum_dust_tables=1;}
void C2_HOOK_FASTCALL GetThreeInts(FILE* f,int* r,int* g,int* b) {
    check_file(f);event(3);*r=(int)colour(0);*g=(int)colour(1);*b=(int)colour(2);
}
void C2_HOOK_FASTCALL GetThreeScalars(FILE* f,br_scalar* q,br_scalar* h,br_scalar* t) {
    check_file(f);event(4);*q=.125f;*h=-123.5f;*t=9999.f;row++;
}
br_pixelmap* C2_HOOK_FASTCALL GenerateDarkenedShadeTable(int height,br_pixelmap* palette,int r,int g,int b,float q,float h,float t,br_scalar darken) {
    unsigned bits;
    event(5);event(height);event((unsigned)(uintptr_t)palette);event(r);event(g);event(b);
    memcpy(&bits,&q,4);event(bits);memcpy(&bits,&h,4);event(bits);
    memcpy(&bits,&t,4);event(bits);memcpy(&bits,&darken,4);event(bits);
    return return_null?NULL:(br_pixelmap*)0x2001000;
}
'''
    driver += "\n\n".join(extract(source, name) for name in NAMES)
    driver += r'''
int main(int argc,char**argv) {
    int i,mode=atoi(argv[1]),repeat=atoi(argv[6]);
    (void)argc;input_count=atoi(argv[2]);pattern=atoi(argv[3]);return_null=atoi(argv[5]);change_count=atoi(argv[7]);
    gNum_dust_tables=0x12345678;gSpark_flags=(int)0xfedcba98u;
    gRender_palette=(br_pixelmap*)0x2002000;gIt_shade_table=atoi(argv[4])?(br_pixelmap*)0x2003000:NULL;
    for(i=0;i<16;i++){gShade_list[i]=(int)(0xa5a50000u+i);separate[i]=(int)(0x5a5a0000u+i);}
    if(pattern==2)gShade_table=&separate[3];
    for(i=0;i<repeat;i++){
        switch(mode){
        case 0:GenerateSmokeShades();break;
        case 1:GenerateItFoxShadeTable();break;
        case 2:GetSmokeShadeTables((FILE*)0x2000100);break;
        case 3:FreeSmokeShadeTables();break;
        case 4:ResetSparks();break;
        }
    }
    printf("{\"state\":[%u,%u,%u],\"shades\":[",(unsigned)gNum_dust_tables,(unsigned)gSpark_flags,(unsigned)(uintptr_t)gIt_shade_table);
    for(i=0;i<16;i++)printf("%s%u",i?",":"",(unsigned)gShade_list[i]);
    printf("],\"separate\":[");for(i=0;i<16;i++)printf("%s%u",i?",":"",(unsigned)separate[i]);
    printf("],\"events\":[");for(i=0;i<(int)event_count;i++)printf("%s%u",i?",":"",events[i]);printf("]}\n");return 0;
}
'''
    harness = out / "harness.c"
    harness.write_text(driver)
    includes = ["src/carma2/newcommon", "src/carma2/include", "src/carma2/hooks", "src/carma2/s3/include", "src/brender/include"]
    command = [args.cc, "-std=gnu89", "-O2", "-Wall", "-Wextra", "-Werror", *["-I" + str(ROOT / p) for p in includes], str(harness), "-o", str(out / "harness.exe")]
    subprocess.run(command, check=True)
    pe = struct.unpack_from("<I", binary, 0x3c)[0]
    table = pe + 24 + struct.unpack_from("<H", binary, pe + 20)[0]
    sections = [struct.unpack_from("<IIII", binary, table + 40*i + 8) for i in range(struct.unpack_from("<H", binary, pe + 6)[0])]

    def original(case):
        mode, count, pattern, present, return_null, repeat, change_count = case
        u = Uc(UC_ARCH_X86, UC_MODE_32)
        u.mem_map(0x400000, 0x400000)
        u.mem_map(0x2000000, 0x20000)
        for _, rva, size, offset in sections:
            u.mem_write(0x400000+rva, binary[offset:offset+size])
        def wr(address, value): u.mem_write(address, struct.pack("<I", value & 0xffffffff))
        def rd(address): return struct.unpack("<I", u.mem_read(address, 4))[0]
        for i in range(16):
            wr(0x6b7840+4*i, 0xa5a50000+i)
            wr(0x2004000+4*i, 0x5a5a0000+i)
        wr(0x660140, 0x200400c if pattern == 2 else 0x6b7860)
        wr(0x6a82b4, 0x12345678)
        wr(0x6aa57c, 0xfedcba98)
        wr(0x74a674, 0x2002000)
        wr(0x6aa5ac, 0x2003000 if present else 0)
        events = []
        row = 0
        def hook(uc, address, size, user):
            nonlocal row
            if address not in (0x48fb00, 0x5155d0, 0x48fe30, 0x4901c0, 0x514eb0):
                return
            sp = uc.reg_read(UC_X86_REG_ESP)
            ecx = uc.reg_read(UC_X86_REG_ECX)
            edx = uc.reg_read(UC_X86_REG_EDX)
            pop, value = 0, 0
            if address == 0x48fb00:
                assert ecx == 0x2000100
                events.append(1)
                value = count
            elif address == 0x5155d0:
                events.append(2)
                if change_count: wr(0x6a82b4, 1)
            elif address in (0x48fe30, 0x4901c0):
                assert ecx == 0x2000100
                pointers = [edx, rd(sp+4), rd(sp+8)]
                pop = 8
                if address == 0x48fe30:
                    events.append(3)
                    unusual = [0, 255, 256, 0xffffffff, 0x12345678, 0x80000000, 0xabcdef, 0x7fffffff]
                    values = [unusual[(row+column*3)%8] if pattern else row*17+column*31 for column in range(3)]
                    for pointer, v in zip(pointers, values): wr(pointer, v)
                else:
                    events.append(4)
                    for pointer, v in zip(pointers, (.125, -123.5, 9999.0)):
                        u.mem_write(pointer, struct.pack("<f", v))
                    row += 1
            else:
                events.extend([5, ecx, edx, *[rd(sp+4+4*i) for i in range(7)]])
                value = 0 if return_null else 0x2001000
                pop = 28
            uc.reg_write(UC_X86_REG_EAX, value & 0xffffffff)
            uc.reg_write(UC_X86_REG_EIP, rd(sp))
            uc.reg_write(UC_X86_REG_ESP, sp+4+pop)
        u.hook_add(UC_HOOK_CODE, hook)
        for _ in range(repeat):
            wr(0x201e000, 0x201f000)
            u.reg_write(UC_X86_REG_ESP, 0x201e000)
            u.reg_write(UC_X86_REG_ECX, 0x2000100)
            u.emu_start(ENTRIES[mode], 0x201f000, count=100000)
            assert u.reg_read(UC_X86_REG_EIP) == 0x201f000
        return dict(state=[rd(p) for p in (0x6a82b4, 0x6aa57c, 0x6aa5ac)],
                    shades=[rd(0x6b7840+4*i) for i in range(16)],
                    separate=[rd(0x2004000+4*i) for i in range(16)], events=events)

    cases = [(0, 0, 0, 0, 0, r, 0) for r in (1, 2)]
    cases += [(1, 0, 0, present, fail, repeat, 0) for present in (0, 1) for fail in (0, 1) for repeat in (1, 2)]
    cases += [(2, n, pattern, 0, 0, 1, 0) for n in (-2147483648, -1, 0, 1, 7, 8, 9, 2147483647) for pattern in (0, 1, 2)]
    cases += [(2, 8, pattern, 0, 0, 1, 1) for pattern in (0, 1, 2)]
    cases += [(mode, 0, 0, present, 0, 2, 0) for mode in (3, 4) for present in (0, 1)]
    results = []
    for index, case in enumerate(cases):
        expected = original(case)
        actual = json.loads(subprocess.check_output([str(out / "harness.exe"), *map(str, case)], text=True))
        if expected != actual:
            failure = out / f"failure-{index}.json"
            failure.write_text(json.dumps(dict(case=case, original=expected, native=actual), indent=2))
            raise AssertionError(f"Smoke shade case {index} differs; see {failure}")
        results.append(dict(case=case, match=True))
    (out / "verification.json").write_text(json.dumps(dict(original_sha256=ORIGINAL_HASH,
        source_sha256=hashlib.sha256(SOURCE.read_bytes()).hexdigest(), compiler_command=command,
        cases=results), indent=2) + "\n")
    print(f"PASS: {len(results)} original/native smoke shade state, parser, cache and reset scenarios")


if __name__ == "__main__":
    main()
