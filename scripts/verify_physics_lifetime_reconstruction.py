#!/usr/bin/env python3
"""Verify current primitive allocation/layout and traversal against original x86.

Uses real 32-bit repository types and extracts current production function
bodies. An allocator recorder preserves uninitialized-byte patterns. Unicorn
executes the verified original PE; allocation sizes/categories, every allocation
byte (with internal addresses normalized), copied payloads and callback order /
early-return values are compared. Does not emulate the allocator implementation.
Workspace command: build-tools/reccmp-venv/Scripts/python.exe
scripts/verify_physics_lifetime_reconstruction.py.
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
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--cc", default=str(ROOT / "build-tools/winlibs-i686/mingw32/bin/gcc.exe"))
parser.add_argument("--dependency-dir", type=Path, default=ROOT / "build-audit/joystick/python-deps")
parser.add_argument("--original", type=Path, default=ROOT / "build-reference/originals/CARMA2_HW.EXE")
parser.add_argument("--output", type=Path, default=ROOT / "build-audit/physics-lifetime-tests")
args = parser.parse_args()
sys.path.insert(0, str(args.dependency_dir.resolve()))
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EIP, UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX

out = args.output.resolve(); out.mkdir(parents=True, exist_ok=True)
binary = args.original.read_bytes()
expected_hash = "9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d"
if hashlib.sha256(binary).hexdigest() != expected_hash:
    raise SystemExit("Original PE SHA-256 differs from verified binary")
source_path = ROOT / "src/carma2/newcommon/49-physics.c"
source = source_path.read_text(); clean = mask_c(source)
names = ["AllocateShapeSphere", "AllocateShapePolyhedron", "AllocateShapeWireFrame", "AllocateShapeBox",
         "AllocatePhysicsJoint", "ClonePhysicsJoint", "PhysicsObjectRecurse", "PhysicsObjectRecurseChildren"]

def extract(name):
    marker = clean.index(" " + name + "("); start = clean.rfind("\n", 0, marker) + 1
    end = clean.index("{", marker) + 1; depth = 1
    while depth:
        depth += (clean[end] == "{") - (clean[end] == "}"); end += 1
    return source[start:end]

driver = r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "49-physics.h"
typedef char check_pointer[sizeof(void*)==4?1:-1];
typedef char check_shapes[sizeof(tCollision_shape_sphere)==0x4c && sizeof(tCollision_shape_box)==0x38 && sizeof(tCollision_shape_polyhedron)==0x50 && sizeof(tCollision_shape_wireframe)==0x48?1:-1];
typedef char check_joint[offsetof(tPhysics_joint,limits)==0x54 && offsetof(tPhysics_joint,count_limits)==0x50 && sizeof(tPhysics_joint_limit)==0x20?1:-1];
typedef char check_tree[offsetof(tPhysics_object,child)==0x224 && offsetof(tPhysics_object,next)==0x220?1:-1];
static unsigned int allocation[2048], clone_source[2048];
static tPhysics_object objects[5];
static unsigned size_requested, category, visits[20], visit_count;
void* C2_HOOK_CDECL BrMemAllocate(br_size_t size, br_uint_8 type) {
    if(size>sizeof(allocation))exit(90);
    size_requested=(unsigned)size;category=type;
    memset(allocation,0xa5,sizeof(allocation));return allocation;
}
static int C2_HOOK_FASTCALL callback(tPhysics_object* object,void* context) {
    int label=(int)(uintptr_t)object->actor;
    visits[visit_count++]=label;
    return label==*(int*)context?-17:0;
}
'''
driver += "\n".join(extract(name) for name in names)
driver += r'''
int main(int argc,char**argv) {
    int mode=atoi(argv[1]), n=atoi(argv[2]), type=atoi(argv[3]), i, result=0;
    void* pointer=NULL;
    unsigned char* bytes=(unsigned char*)allocation;
    (void)argc;
    for(i=0;i<(int)sizeof(clone_source);i++)((unsigned char*)clone_source)[i]=(unsigned char)(i%251);
    ((tPhysics_joint*)clone_source)->count_limits=n;
    switch(mode) {
    case 1:pointer=AllocateShapeSphere(type);break;
    case 2:pointer=AllocateShapePolyhedron(n,type);break;
    case 3:pointer=AllocateShapeWireFrame(n,2*n+1,type);break;
    case 4:pointer=AllocateShapeBox(type);break;
    case 5:pointer=AllocatePhysicsJoint(n,type);break;
    case 6:pointer=ClonePhysicsJoint((tPhysics_joint*)clone_source,type);break;
    default:
        for(i=0;i<5;i++)objects[i].actor=(br_actor*)(uintptr_t)(i+1);
        objects[0].child=&objects[1];objects[1].next=&objects[2];objects[1].child=&objects[3];objects[3].next=&objects[4];
        if(n==-1)objects[0].child=NULL;
        result=mode==7?PhysicsObjectRecurse(&objects[0],callback,&n):PhysicsObjectRecurseChildren(&objects[0],callback,&n);
        printf("{\"result\":%d,\"visits\":[",result);
        for(i=0;i<(int)visit_count;i++)printf("%s%u",i?",":"",visits[i]);
        printf("]}\n");return 0;
    }
    if(mode==2){allocation[0x44/4]-=(unsigned)(uintptr_t)allocation;allocation[0x48/4]-=(unsigned)(uintptr_t)allocation;allocation[0x4c/4]-=(unsigned)(uintptr_t)allocation;}
    if(mode==3){allocation[0x40/4]-=(unsigned)(uintptr_t)allocation;allocation[0x44/4]-=(unsigned)(uintptr_t)allocation;}
    printf("{\"returns_allocation\":%s,\"size\":%u,\"category\":%u,\"data\":\"",pointer==allocation?"true":"false",size_requested,category);
    for(i=0;i<(int)size_requested;i++)printf("%02x",bytes[i]);
    printf("\"}\n");return 0;
}
'''
(out / "harness.c").write_text(driver)
includes = ["src/carma2/newcommon", "src/carma2/include", "src/carma2/hooks", "src/carma2/s3/include", "src/brender/include"]
command = [args.cc, "-std=gnu89", "-O2", "-Wall", "-Wextra", "-Werror", *["-I" + str(ROOT / p) for p in includes], str(out / "harness.c"), "-o", str(out / "harness.exe")]
subprocess.run(command, check=True)
pe = struct.unpack_from("<I", binary, 0x3c)[0]
section_count = struct.unpack_from("<H", binary, pe + 6)[0]
section_table = pe + 24 + struct.unpack_from("<H", binary, pe + 20)[0]
entries = [None, 0x4c5d10, 0x4c5d70, 0x4c5dc0, 0x4c5e00, 0x4c5e20, 0x4c5e40, 0x4c6470, 0x4c64b0]

def original(mode, n, category):
    u = Uc(UC_ARCH_X86, UC_MODE_32); u.mem_map(0x400000, 0x400000); u.mem_map(0x1000000, 0x20000)
    for i in range(section_count):
        _, rva, size, offset = struct.unpack_from("<IIII", binary, section_table + 40 * i + 8)
        u.mem_write(0x400000 + rva, binary[offset:offset + size])
    def wr(address, value):u.mem_write(address, struct.pack("<I", value & 0xffffffff))
    def rd(address):return struct.unpack("<I", u.mem_read(address, 4))[0]
    allocation, clone, callback, context = 0x100f000, 0x100c000, 0x100b000, 0x100b100
    objects = [0x1004000 + i * 0x500 for i in range(5)]
    u.mem_write(clone, bytes(i % 251 for i in range(8192))); wr(clone + 0x50, n)
    for i, obj in enumerate(objects):wr(obj, i + 1)
    wr(objects[0] + 0x224, objects[1] if n != -1 else 0); wr(objects[1] + 0x220, objects[2]); wr(objects[1] + 0x224, objects[3]); wr(objects[3] + 0x220, objects[4]); wr(context, n)
    request, visits = {}, []
    def hook(uc, address, size, user):
        if address not in (0x5275c0, callback):return
        sp = uc.reg_read(UC_X86_REG_ESP)
        if address == 0x5275c0:
            request.update(size=rd(sp + 4), category=rd(sp + 8) & 255)
            assert request["size"] <= 8192
            uc.mem_write(allocation, b"\xa5" * 8192); value = allocation
        else:
            assert uc.reg_read(UC_X86_REG_EDX) == context
            label = rd(uc.reg_read(UC_X86_REG_ECX)); visits.append(label)
            value = -17 if label == n else 0
        uc.reg_write(UC_X86_REG_EAX, value & 0xffffffff); uc.reg_write(UC_X86_REG_EIP, rd(sp)); uc.reg_write(UC_X86_REG_ESP, sp + 4)
    u.hook_add(UC_HOOK_CODE, hook)
    sp = 0x101f000; wr(sp, 0x1020000); u.reg_write(UC_X86_REG_ESP, sp)
    if mode in (1, 4):u.reg_write(UC_X86_REG_ECX, category & 0xffffffff)
    elif mode in (2, 5):u.reg_write(UC_X86_REG_ECX, n);u.reg_write(UC_X86_REG_EDX, category & 0xffffffff)
    elif mode == 3:u.reg_write(UC_X86_REG_ECX, n);u.reg_write(UC_X86_REG_EDX, 2 * n + 1);wr(sp + 4, category)
    elif mode == 6:u.reg_write(UC_X86_REG_ECX, clone);u.reg_write(UC_X86_REG_EDX, category & 0xffffffff)
    else:u.reg_write(UC_X86_REG_ECX, objects[0]);u.reg_write(UC_X86_REG_EDX, callback);wr(sp + 4, context)
    u.emu_start(entries[mode], 0x1020000, count=10000)
    result = u.reg_read(UC_X86_REG_EAX)
    if mode >= 7:return {"result": result if result < 0x80000000 else result - 0x100000000, "visits": visits}
    data = bytearray(u.mem_read(allocation, request["size"]))
    for offset in [0x44, 0x48, 0x4c] if mode == 2 else [0x40, 0x44] if mode == 3 else []:
        struct.pack_into("<I", data, offset, struct.unpack_from("<I", data, offset)[0] - allocation)
    return {"returns_allocation": result == allocation, **request, "data": data.hex()}

cases = [(mode, 0, cat) for mode in (1, 4) for cat in (0, 3, 255, 0x1234, -1)]
cases += [(2, n, 19) for n in (2, 3, 4, 12)] + [(3, n, 21) for n in (0, 3, 8)]
cases += [(mode, n, 255) for mode in (5, 6) for n in (0, 1, 3, 10)]
cases += [(mode, stop, 0) for mode in (7, 8) for stop in (-1, 0, 1, 2, 3, 4, 5)]
results = []
for mode, n, category in cases:
    native = json.loads(subprocess.check_output([str(out / "harness.exe"), str(mode), str(n), str(category)], text=True))
    expected = original(mode, n, category)
    if native != expected:raise AssertionError(f"mode={mode},n={n},type={category}: C={native},original={expected}")
    results.append({"name": names[mode - 1], "count_or_stop": n, "category": category, "result": native})
(out / "results.json").write_text(json.dumps(results, indent=2) + "\n")
manifest = {"original_sha256": expected_hash, "case_count": len(results), "all_cases_passed": True,
            "compiler_command": command, "compiler_version": subprocess.check_output([args.cc, "--version"], text=True).splitlines()[0],
            "source_sha256": hashlib.sha256(source_path.read_bytes()).hexdigest(), "harness_sha256": hashlib.sha256(driver.encode()).hexdigest(),
            "scope": "Actual current function bodies + actual 32-bit types vs original x86, full allocation byte pattern/layout/copy comparison and callback traversal/early-return comparison."}
(out / "verification.json").write_text(json.dumps(manifest, indent=2) + "\n")
print(f"Passed {len(results)} differential allocation/layout/clone/traversal cases")
