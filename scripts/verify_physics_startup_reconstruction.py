#!/usr/bin/env python3
"""Compare physics/crush startup with the hash-verified original x86 executable.

Builds current production bodies against real repository 32-bit types. Checks
every allocation byte, allocation order/category, globals and preserved fields;
workspace success/error boundaries, callback installation/order, and exact error
messages including the original hinge-case fallthrough. Allocator, FatalError,
exit and sprintf are controlled external fixtures, not implementations tested
here. Requires i686 GCC and Unicorn; generated files stay under build-audit.
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
PHYSICS = ROOT / "src/carma2/newcommon/49-physics.c"
CRUSH = ROOT / "src/carma2/newcommon/13-crush1.c"


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
    parser.add_argument("--output", type=Path, default=ROOT / "build-audit/physics-startup-tests")
    args = parser.parse_args()
    sys.path.insert(0, str(args.dependency_dir.resolve()))
    from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
    from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EIP, UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX

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
#include <stdarg.h>
#include <setjmp.h>
#include "13-crush1.h"
#include "49-physics.h"
#include "52-errors.h"
#include "rec2_macros.h"
typedef char check_layout[sizeof(void*)==4 && sizeof(tCrush_info)==260 && sizeof(tPhysics_object)==1240 && sizeof(tCrush_info_buffer)==12 && offsetof(tPhysics_object,shape)==32?1:-1];
#define PHYSICS_BUFFER_PART_SIZE 40000
#define PHYSICS_BUFFER_OTHER_SIZE 50000
unsigned char gTemporary_physics_render_buffer[300000];
static unsigned int arena[65536];
static unsigned used, sizes[150], categories[150], offsets[150], requests;
static jmp_buf escape;
static int exit_code=999999,fatal_code=999999,cb_code=999999,calls[4],call_count;
static char fatal_message[256],cb_message[256];
static void C2_HOOK_FASTCALL callback(tPhysicsError code,const char* message) {
    cb_code=code;strcpy(cb_message,message);
}
static void C2_NORETURN fixture_exit(int code) {exit_code=code;longjmp(escape,1);}
void C2_NORETURN C2_HOOK_CDECL FatalError(int code,...) {
    va_list ap;va_start(ap,code);fatal_code=code;strcpy(fatal_message,va_arg(ap,const char*));va_end(ap);longjmp(escape,1);
}
void* C2_HOOK_CDECL BrMemAllocate(br_size_t size,br_uint_8 category) {
    unsigned offset=(used+3)&~3u;
    if(offset+size>sizeof(arena)||requests>=150)abort();
    sizes[requests]=size;categories[requests]=category;offsets[requests++]=offset;
    memset((char*)arena+offset,0xa5,size);used=offset+size;return (char*)arena+offset;
}
#define exit fixture_exit
'''
    physics_source = PHYSICS.read_text()
    crush_source = CRUSH.read_text()
    driver += physics_source[physics_source.index("// GLOBAL:"):physics_source.index("// count")]
    driver += crush_source[crush_source.index("// GLOBAL: CARMA2_HW 0x0067a190"):crush_source.index("// FUNCTION: CARMA2_HW 0x00429bb0")]
    for name in ("AllocateShapeSphere", "AllocateShapePolyhedron", "AllocatePhysicsJoint", "PhysicsError", "PhysicsSetErrorHandler", "InitPhysicsWorkspace", "DoPhysicsError", "InitPhysics"):
        body = extract(PHYSICS, name)
        if name in ("PhysicsSetErrorHandler", "InitPhysicsWorkspace"):
            body = body.replace("{", "{\n    calls[call_count++]=" + ("1;" if name == "PhysicsSetErrorHandler" else "2;"), 1)
        driver += body + "\n"
    driver += extract(CRUSH, "InitCrushSystems")
    driver += r'''
static unsigned norm(unsigned value,int heap) {
    unsigned base=(unsigned)(uintptr_t)(heap?(void*)arena:(void*)gTemporary_physics_render_buffer);
    unsigned extent=heap?sizeof(arena):sizeof(gTemporary_physics_render_buffer);
    if(value>=base&&value<base+extent)return value-base+0x1000000;
    return value;
}
static void print_bytes(const void* ptr,unsigned size) {
    unsigned i,value;const unsigned char* bytes=ptr;
    for(i=0;i<size;i+=4){memcpy(&value,bytes+i,4);value=norm(value,1);printf("%02x%02x%02x%02x",value&255,(value>>8)&255,(value>>16)&255,value>>24);}
}
int main(int argc,char**argv) {
    int mode=atoi(argv[1]),n=atoi(argv[2]),has_cb=atoi(argv[3]),i;
    unsigned state[64],count=0;
    (void)argc;
    memset(gCrush_array_0067a190,0x5a,sizeof(gCrush_array_0067a190));gDAT_00679440=0x5a5a5a5a;
    memset(&gDetached_bit_crush_info_buffer,0x5a,12);memset(&gSplit_car_crush_info_buffer,0x5a,12);
    gPhysics_buffer1_ptr=(void*)0x11111111;gPhysics_buffer2_ptr=(void*)0x22222222;gPhysics_buffer3_ptr=(void*)0x33333333;
    gPhysics_other_buffer=(void*)0x44444444;gPhysics_other_buffer_capacity=0x55555555;
    gPhysics_error_cb=has_cb?callback:NULL;
    if(!setjmp(escape))switch(mode) {
    case 0:for(i=0;i<n;i++)InitCrushSystems();break;
    case 1:InitPhysicsWorkspace(gTemporary_physics_render_buffer,n);break;
    case 2:PhysicsError((tPhysicsError)n,"caller detail");break;
    case 3:DoPhysicsError((tPhysicsError)n,"ignored caller detail");break;
    case 4:InitPhysics();break;
    case 5:PhysicsSetErrorHandler(has_cb?NULL:callback);break;
    }
    if(mode==0) {
        memcpy(state,gCrush_array_0067a190,40);count=10;state[count++]=gDAT_00679440;
        memcpy(state+count,&gDetached_bit_crush_info_buffer,12);count+=3;
        memcpy(state+count,&gSplit_car_crush_info_buffer,12);count+=3;
        state[count++]=(unsigned)(uintptr_t)gDetached_bit_collision_infos;state[count++]=(unsigned)(uintptr_t)gSplit_car_collision_infos;
        state[count++]=gDetached_bit_driver;state[count++]=gSplit_car_driver;
        memcpy(state+count,gTrack_crush_joints,128);count+=32;state[count++]=(unsigned)(uintptr_t)gGonad_sphere_collision_shape;
    } else {
        state[count++]=norm((unsigned)(uintptr_t)gPhysics_buffer1_ptr,0);state[count++]=norm((unsigned)(uintptr_t)gPhysics_buffer2_ptr,0);
        state[count++]=norm((unsigned)(uintptr_t)gPhysics_buffer3_ptr,0);state[count++]=norm((unsigned)(uintptr_t)gPhysics_other_buffer,0);
        state[count++]=gPhysics_other_buffer_capacity;state[count++]=gPhysics_error_cb==NULL?0:gPhysics_error_cb==callback?1:gPhysics_error_cb==DoPhysicsError?2:99;
    }
    printf("{\"state\":[");for(i=0;i<(int)count;i++)printf("%s%u",i?",":"",mode==0?norm(state[i],1):state[i]);
    printf("],\"allocations\":[");for(i=0;i<(int)requests;i++){printf("%s[%u,%u,\"",i?",":"",sizes[i],categories[i]);print_bytes((char*)arena+offsets[i],sizes[i]);printf("\"]");}
    printf("],\"calls\":[");for(i=0;i<call_count;i++)printf("%s%d",i?",":"",calls[i]);
    printf("],\"exit\":%d,\"fatal\":[%d,\"%s\"],\"callback\":[%d,\"%s\"]}\n",exit_code,fatal_code,fatal_message,cb_code,cb_message);return 0;
}
'''
    (out / "harness.c").write_text(driver)
    includes = ["src/carma2/newcommon", "src/carma2/include", "src/carma2/hooks", "src/carma2/s3/include", "src/brender/include"]
    command = [args.cc, "-std=gnu89", "-O2", "-Wall", "-Wextra", "-Werror", *["-I" + str(ROOT / p) for p in includes], str(out / "harness.c"), "-o", str(out / "harness.exe")]
    subprocess.run(command, check=True)
    pe = struct.unpack_from("<I", binary, 0x3c)[0]
    section_count = struct.unpack_from("<H", binary, pe + 6)[0]
    section_table = pe + 24 + struct.unpack_from("<H", binary, pe + 20)[0]
    entries = [0x429fa0, 0x4c6510, 0x4c61c0, 0x4b5990, 0x4b5ca0, 0x4c61f0]

    def original(mode, n, has_cb):
        u = Uc(UC_ARCH_X86, UC_MODE_32)
        u.mem_map(0x400000, 0x400000)
        u.mem_map(0x1000000, 0x40000)
        u.mem_map(0x2000000, 0x20000)
        for i in range(section_count):
            _, rva, size, offset = struct.unpack_from("<IIII", binary, section_table + 40 * i + 8)
            u.mem_write(0x400000 + rva, binary[offset:offset + size])
        def wr(address, value):
            u.mem_write(address, struct.pack("<I", value & 0xffffffff))
        def rd(address):
            return struct.unpack("<I", u.mem_read(address, 4))[0]
        def signed(value):
            return value if value < 0x80000000 else value - 0x100000000
        def string(address):
            text = bytearray()
            while True:
                byte = u.mem_read(address + len(text), 1)[0]
                if byte == 0:
                    return text.decode("ascii")
                text.append(byte)
        callback, message, end = 0x2001000, 0x2002000, 0x201f000
        u.mem_write(message, b"caller detail\0")
        for address, size in ((0x67a190, 40), (0x679440, 4), (0x6796a0, 12), (0x67b7b8, 12)):
            u.mem_write(address, b"\x5a" * size)
        for address, value in ((0x744818, 0x11111111), (0x744814, 0x22222222), (0x74a5e0, 0x33333333), (0x6940c4, 0x44444444), (0x6940c8, 0x55555555), (0x6940c0, callback if has_cb else 0)):
            wr(address, value)
        requests, calls = [], []
        outcome = {"exit": 999999, "fatal": [999999, ""], "callback": [999999, ""]}
        used = 0
        def hook(uc, address, size, user):
            nonlocal used
            if address in (0x4c61f0, 0x4c6510):
                calls.append(1 if address == 0x4c61f0 else 2)
            if address not in (0x5275c0, callback, 0x5784e0, 0x44c230, 0x575de0):
                return
            sp = uc.reg_read(UC_X86_REG_ESP)
            value = 0
            if address == 0x5275c0:
                size_requested, category = rd(sp + 4), rd(sp + 8) & 255
                used = (used + 3) & ~3
                pointer = 0x1000000 + used
                assert used + size_requested <= 0x40000
                requests.append((pointer, size_requested, category))
                uc.mem_write(pointer, b"\xa5" * size_requested)
                used += size_requested
                value = pointer
            elif address == callback:
                outcome["callback"] = [signed(uc.reg_read(UC_X86_REG_ECX)), string(uc.reg_read(UC_X86_REG_EDX))]
            elif address == 0x5784e0:
                outcome["exit"] = signed(rd(sp + 4))
                uc.emu_stop()
                return
            elif address == 0x44c230:
                outcome["fatal"] = [signed(rd(sp + 4)), string(rd(sp + 8))]
                uc.emu_stop()
                return
            elif address == 0x575de0:
                assert string(rd(sp + 8)) == "%d "
                text = str(signed(rd(sp + 12))) + " "
                uc.mem_write(rd(sp + 4), text.encode() + b"\0")
                value = len(text)
            uc.reg_write(UC_X86_REG_EAX, value)
            uc.reg_write(UC_X86_REG_EIP, rd(sp))
            uc.reg_write(UC_X86_REG_ESP, sp + 4)
        u.hook_add(UC_HOOK_CODE, hook)
        for repeat in range(n if mode == 0 else 1):
            sp = 0x201e000
            wr(sp, end)
            u.reg_write(UC_X86_REG_ESP, sp)
            if mode == 1:
                u.reg_write(UC_X86_REG_ECX, 0x6baa40)
                u.reg_write(UC_X86_REG_EDX, n)
            elif mode in (2, 3):
                u.reg_write(UC_X86_REG_ECX, n & 0xffffffff)
                u.reg_write(UC_X86_REG_EDX, message)
            elif mode == 5:
                u.reg_write(UC_X86_REG_ECX, 0 if has_cb else callback)
            u.emu_start(entries[mode], end, count=300000)
            assert u.reg_read(UC_X86_REG_EIP) == end or outcome["exit"] != 999999 or outcome["fatal"][0] != 999999
        def norm(value, heap=True):
            base, extent = (0x1000000, 0x40000) if heap else (0x6baa40, 300000)
            return value - base + 0x1000000 if base <= value < base + extent else value
        if mode == 0:
            state = [rd(0x67a190 + 4*i) for i in range(10)] + [rd(0x679440)]
            state += [rd(address + 4*i) for address in (0x6796a0, 0x67b7b8) for i in range(3)]
            state += [rd(address) for address in (0x679448, 0x67a188, 0x6796ac, 0x67b7c4)]
            state += [rd(0x67bd68 + 4*i) for i in range(32)] + [rd(0x6796b4)]
            state = [norm(value) for value in state]
        else:
            state = [norm(rd(address), False) for address in (0x744818, 0x744814, 0x74a5e0, 0x6940c4)]
            state += [rd(0x6940c8), {0: 0, callback: 1, 0x4b5990: 2}.get(rd(0x6940c0), 99)]
        allocations = []
        for pointer, size, category in requests:
            data = bytearray(u.mem_read(pointer, size))
            for offset in range(0, size, 4):
                struct.pack_into("<I", data, offset, norm(struct.unpack_from("<I", data, offset)[0]))
            allocations.append([size, category, data.hex()])
        return {"state": state, "allocations": allocations, "calls": calls, **outcome}

    cases = [(0, repeat, 0) for repeat in (1, 2)]
    cases += [(1, size, cb) for size in (0, 119999, 120000, 169999, 170000, 170001, 300000) for cb in (0, 1)]
    cases += [(2, code, cb) for code in (-1, 0, 1, 5, 12) for cb in (0, 1)]
    cases += [(3, code, 0) for code in (-1, 0, 1, 2, 3, 4, 5, 6, 12, 99)]
    cases += [(4, 0, cb) for cb in (0, 1)] + [(5, 0, cb) for cb in (0, 1)]
    results = []
    for mode, n, cb in cases:
        native = json.loads(subprocess.check_output([str(out / "harness.exe"), str(mode), str(n), str(cb)], text=True))
        expected = original(mode, n, cb)
        if native != expected:
            (out / "failure.json").write_text(json.dumps({"case": [mode, n, cb], "current": native, "original": expected}, indent=2))
            raise AssertionError(f"Differential mismatch mode={mode}, value={n}, callback={cb}; see failure.json")
        results.append({"function": ["InitCrushSystems", "InitPhysicsWorkspace", "PhysicsError", "DoPhysicsError", "InitPhysics", "PhysicsSetErrorHandler"][mode], "value": n, "callback": cb, "result": native})
    (out / "results.json").write_text(json.dumps(results, indent=2) + "\n")
    source_files = [PHYSICS, CRUSH, ROOT / "src/carma2/include/rec2_types.h"]
    manifest = {"original_sha256": ORIGINAL_HASH, "case_count": len(results), "all_cases_passed": True,
                "compiler_command": command, "compiler_version": subprocess.check_output([args.cc, "--version"], text=True).splitlines()[0],
                "source_sha256": {str(path.relative_to(ROOT)): hashlib.sha256(path.read_bytes()).hexdigest() for path in source_files},
                "harness_sha256": hashlib.sha256(driver.encode()).hexdigest(),
                "scope": "Actual production bodies and repository 32-bit types vs original x86: full initialized/preserved allocation bytes, repeated startup, global state, workspace boundaries, callbacks, call order and all known/unknown error branches."}
    (out / "verification.json").write_text(json.dumps(manifest, indent=2) + "\n")
    print(f"Passed {len(results)} differential physics/crush startup cases")


if __name__ == "__main__":
    main()
