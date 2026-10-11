#!/usr/bin/env python3
"""Compare recovered car callbacks with original x86 on ownership/branch cases.

Compiles current function bodies against the real 32-bit repository types.
Executes the verified original PE in Unicorn with BrMemFree recorded rather
than deallocated, then compares exact free order and retained/cleared pointers.
Requires an i686 C compiler, runnable output, and the Unicorn Python package.
The memory recorder does not test the allocator or malformed out-of-range LODs.
On this workspace: build-tools/reccmp-venv/Scripts/python.exe
scripts/verify_car_cleanup_reconstruction.py. Override --cc, --dependency-dir,
--original and --output to use another compatible runtime/toolchain.
"""

import argparse
import hashlib
import json
from pathlib import Path
import struct
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--cc", default=str(ROOT / "build-tools/winlibs-i686/mingw32/bin/gcc.exe"))
parser.add_argument("--dependency-dir", type=Path, default=ROOT / "build-audit/joystick/python-deps")
parser.add_argument("--output", type=Path, default=ROOT / "build-audit/car-cleanup-tests")
parser.add_argument("--original", type=Path, default=ROOT / "build-reference/originals/CARMA2_HW.EXE")
args = parser.parse_args()
sys.path.insert(0, str(args.dependency_dir.resolve()))
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EIP, UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX
from inventory_reconstruction_backlog import mask_c

out = args.output.resolve()
out.mkdir(parents=True, exist_ok=True)
binary = args.original.read_bytes()
if hashlib.sha256(binary).hexdigest() != "9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d":
    raise SystemExit("Original PE hash differs from verified CARMA2_HW.EXE")


def extract(file, name):
    source = (ROOT / "src/carma2/newcommon" / file).read_text()
    clean = mask_c(source)
    marker = clean.index(" " + name + "(")
    start = source.rfind("\n", 0, marker) + 1
    end = clean.index("{", marker) + 1
    depth = 1
    while depth:
        depth += (clean[end] == "{") - (clean[end] == "}")
        end += 1
    return source[start:end]


driver = r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "rec2_types.h"
#include "00-car.h"
#include "08-loading1.h"
#include "13-crush1.h"
#include "51-smash.h"
#include "41-utility.h"
typedef char check_pointer_size[sizeof(void*) == 4 ? 1 : -1];
typedef char check_actor_offsets[offsetof(br_actor,user)==0x60 && offsetof(br_actor,model)==0x18 && offsetof(br_actor,type)==0x12 ? 1 : -1];
typedef char check_model_offsets[offsetof(br_model,user)==0x28 ? 1 : -1];
typedef char check_car_offsets[offsetof(tCar_spec,car_model_actor)==0xe0c && offsetof(tCar_spec,count_detail_levels)==0xe14 && offsetof(tCar_spec,field_0xe18)==0xe18 ? 1 : -1];
typedef char check_crush_offsets[offsetof(tCar_crush_buffer_entry,flap_data)==0x30 && offsetof(tCar_crush_buffer_entry,detach_data)==0x34 && offsetof(tCar_crush_buffer_entry,count_smashables)==0x38 && offsetof(tCar_crush_buffer_entry,smashables)==0x3c ? 1 : -1];
typedef char check_smash_offsets[sizeof(tCar_crush_smashable_part)==0x7c && offsetof(tCar_crush_smashable_part,levels)==0x50 && offsetof(tCar_crush_smashable_part,field_0x70)==0x70 && offsetof(tCar_crush_smashable_part,field_0x78)==0x78 ? 1 : -1];
static br_actor actors[3];
static br_model models[2];
static tUser_crush_data users[3];
static tUser_detail_level_model details[2];
static tModel_detail_vertex_data vertices[2];
static tCar_spec car;
static tCar_crush_buffer_entry crush;
static tCar_crush_flap_data flap;
static tCar_crush_detach_data detach;
static tCar_crush_smashable_part smashes[2];
static unsigned char buffers[6][128];
static void* registry[100];
static int trace[100], trace_count;
static int id(void* pointer) {
    int i;
    if (pointer == NULL) return 0;
    for (i=0;i<100;i++) if (registry[i]==pointer) return i;
    fprintf(stderr,"Unregistered pointer %p\n",pointer);exit(91);
    return -1;
}
void C2_HOOK_CDECL BrMemFree(void* pointer) {
    trace[trace_count++] = id(pointer);
}
'''
for file, name in [("41-utility.c", "DRActorEnumRecurse"), ("00-car.c", "SwitchCarModel"),
                   ("00-car.c", "SwitchCarModels"), ("51-smash.c", "DisposeSmashData"),
                   ("13-crush1.c", "DisposeCrushData"), ("13-crush1.c", "DisposeVertexData"),
                   ("08-loading1.c", "DisposeCarActModDataCB")]:
    driver += "\n" + extract(file, name) + "\n"
driver += r'''
int main(int argc,char**argv) {
    int mode=atoi(argv[1]), scenario=atoi(argv[2]), i, result=0, index=scenario==2 || scenario==4 || scenario==5;
    (void)argc;
    registry[10]=&models[0];registry[11]=&models[1];
    for(i=0;i<3;i++){registry[20+i]=&users[i];users[i].models[0]=&models[0];users[i].models[1]=&models[1];actors[i].user=&users[i];actors[i].model=&models[0];actors[i].type=9;}
    for(i=0;i<2;i++){registry[30+i]=&details[i];registry[40+i]=&vertices[i];models[i].user=&details[i];details[i].field_0x4=&vertices[i];}
    registry[50]=&crush;registry[51]=&flap;registry[52]=&detach;registry[60]=smashes;
    for(i=0;i<6;i++)registry[70+i]=buffers[i];
    car.car_model_actor=&actors[0];car.count_detail_levels=2;car.field_0xe18=99;
    if(mode==0) {
        actors[0].children=&actors[1];actors[1].children=&actors[2];
        if(scenario==0)for(i=0;i<3;i++)actors[i].user=NULL;
        if(scenario==1)actors[0].user=NULL;
        if(scenario==2)users[1].models[1]=NULL;
        if(scenario==3)users[0].models[0]=NULL;
        if(scenario==5){actors[1].children=NULL;actors[1].next=&actors[2];actors[2].user=NULL;}
        if(scenario==6){actors[0].user=NULL;result=(int)SwitchCarModel(&actors[0],NULL);}
        else SwitchCarModels(&car,index);
        printf("{\"result\":%d,\"state\":[%d",result,car.field_0xe18);
        for(i=0;i<3;i++)printf(",%d,%d",actors[i].type,id(actors[i].model));
    } else {
        users[0].crush_data=&crush;crush.flap_data=&flap;crush.detach_data=&detach;crush.smashables=smashes;crush.count_smashables=2;
        for(i=0;i<2;i++){smashes[i].field_0x70=(tU16*)buffers[3*i];smashes[i].field_0x78=(tCar_crush_vertex_data*)buffers[3*i+1];smashes[i].levels=(tSmashable_level*)buffers[3*i+2];}
        if(scenario==0)actors[0].user=NULL;
        if(scenario==1){car.count_detail_levels=0;users[0].crush_data=NULL;}
        if(scenario==2){users[0].models[0]=NULL;models[1].user=NULL;users[0].crush_data=NULL;}
        if(scenario==4){car.count_detail_levels=1;details[0].field_0x4=NULL;users[0].crush_data=NULL;}
        if(scenario==5)car.count_detail_levels=-1;
        if(scenario==6){car.count_detail_levels=0;crush.count_smashables=0;}
        if(scenario==7){car.count_detail_levels=0;crush.smashables=NULL;}
        if(scenario==8)users[0].models[1]=&models[0];
        result=(int)DisposeCarActModDataCB(&actors[0],&car.index);
        printf("{\"result\":%d,\"state\":[%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",result,id(actors[0].user),id(actors[0].model),id(models[0].user),id(models[1].user),id(details[0].field_0x4),id(details[1].field_0x4),id(users[0].crush_data),id(crush.flap_data),id(crush.detach_data),id(crush.smashables),id(users[0].models[0]),id(users[0].models[1]));
    }
    printf("],\"frees\":[");
    for(i=0;i<trace_count;i++)printf("%s%d",i?",":"",trace[i]);
    printf("]}\n");return 0;
}
'''
(out / "harness.c").write_text(driver)
includes = ["src/carma2/newcommon", "src/carma2/include", "src/carma2/hooks", "src/carma2/s3/include", "src/brender/include"]
compile_command = [args.cc, "-std=gnu89", "-O2", "-Wall", "-Wextra", *["-I" + str(ROOT / x) for x in includes],
                   str(out / "harness.c"), "-o", str(out / "harness.exe")]
subprocess.run(compile_command, check=True)

pe = struct.unpack_from("<I", binary, 0x3c)[0]
sections = struct.unpack_from("<H", binary, pe + 6)[0]
section_table = pe + 24 + struct.unpack_from("<H", binary, pe + 20)[0]
addresses = {10: 0x1005000, 11: 0x1005100, 20: 0x1006000, 21: 0x1006100, 22: 0x1006200,
             30: 0x1007000, 31: 0x1007100, 40: 0x1007200, 41: 0x1007300,
             50: 0x1008000, 51: 0x1008100, 52: 0x1008200, 60: 0x1009000,
             **{70 + i: 0x100a000 + i * 0x100 for i in range(6)}}
ids = {value: key for key, value in addresses.items()}
ids[0] = 0


def original(mode, scenario):
    u = Uc(UC_ARCH_X86, UC_MODE_32)
    u.mem_map(0x400000, 0x400000)
    u.mem_map(0x1000000, 0x20000)
    for i in range(sections):
        _, rva, size, offset = struct.unpack_from("<IIII", binary, section_table + i * 40 + 8)
        u.mem_write(0x400000 + rva, binary[offset:offset + size])
    def wr(address, value):
        u.mem_write(address, struct.pack("<I", value & 0xffffffff))
    def rd(address):
        return struct.unpack("<I", u.mem_read(address, 4))[0]
    def pid(address):
        return ids[rd(address)]
    car, actors = 0x1001000, [0x1004000 + i * 0x100 for i in range(3)]
    index = int(scenario in (2, 4, 5))
    for i, actor in enumerate(actors):
        wr(actor + 0x60, addresses[20 + i]); wr(actor + 0x18, addresses[10])
        u.mem_write(actor + 0x12, b"\x09")
        wr(addresses[20 + i], addresses[10]); wr(addresses[20 + i] + 4, addresses[11])
    for i in range(2):
        wr(addresses[10 + i] + 0x28, addresses[30 + i]); wr(addresses[30 + i] + 4, addresses[40 + i])
    wr(car + 0xe0c, actors[0]); wr(car + 0xe14, 2); wr(car + 0xe18, 99)
    if mode == 0:
        wr(actors[0] + 8, actors[1]); wr(actors[1] + 8, actors[2])
        if scenario == 0:
            for actor in actors: wr(actor + 0x60, 0)
        if scenario == 1: wr(actors[0] + 0x60, 0)
        if scenario == 2: wr(addresses[21] + 4, 0)
        if scenario == 3: wr(addresses[20], 0)
        if scenario == 5:
            wr(actors[1] + 8, 0); wr(actors[1], actors[2]); wr(actors[2] + 0x60, 0)
        if scenario == 6: wr(actors[0] + 0x60, 0)
    else:
        wr(addresses[20] + 8, addresses[50]); wr(addresses[50] + 0x30, addresses[51]); wr(addresses[50] + 0x34, addresses[52])
        wr(addresses[50] + 0x3c, addresses[60]); u.mem_write(addresses[50] + 0x38, struct.pack("<H", 2))
        for i in range(2):
            for offset, value in [(0x70, 70), (0x78, 71), (0x50, 72)]:
                wr(addresses[60] + i * 0x7c + offset, addresses[value + 3 * i])
        if scenario == 0: wr(actors[0] + 0x60, 0)
        if scenario == 1: wr(car + 0xe14, 0); wr(addresses[20] + 8, 0)
        if scenario == 2: wr(addresses[20], 0); wr(addresses[11] + 0x28, 0); wr(addresses[20] + 8, 0)
        if scenario == 4: wr(car + 0xe14, 1); wr(addresses[30] + 4, 0); wr(addresses[20] + 8, 0)
        if scenario == 5: wr(car + 0xe14, -1)
        if scenario == 6: wr(car + 0xe14, 0); u.mem_write(addresses[50] + 0x38, b"\0\0")
        if scenario == 7: wr(car + 0xe14, 0); wr(addresses[50] + 0x3c, 0)
        if scenario == 8: wr(addresses[20] + 4, addresses[10])
    frees = []
    def hook(uc, address, size, context):
        if address == 0x5275f0:
            sp = uc.reg_read(UC_X86_REG_ESP)
            frees.append(ids[rd(sp + 4)])
            uc.reg_write(UC_X86_REG_EAX, 0)
            uc.reg_write(UC_X86_REG_EIP, rd(sp))
            uc.reg_write(UC_X86_REG_ESP, sp + 4)
    u.hook_add(UC_HOOK_CODE, hook)
    sp = 0x101f000
    wr(sp, 0x1020000)
    u.reg_write(UC_X86_REG_ESP, sp)
    if mode == 0 and scenario != 6:
        u.reg_write(UC_X86_REG_ECX, car); u.reg_write(UC_X86_REG_EDX, index)
        entry = 0x413f40
    else:
        wr(sp + 4, actors[0]); wr(sp + 8, car if mode else 0)
        entry = 0x44be80 if mode else 0x413f10
    u.emu_start(entry, 0x1020000, count=10000)
    result = u.reg_read(UC_X86_REG_EAX) if mode or scenario == 6 else 0
    if mode == 0:
        state = [rd(car + 0xe18)]
        for actor in actors: state.extend([u.mem_read(actor + 0x12, 1)[0], pid(actor + 0x18)])
    else:
        state = [pid(actors[0] + 0x60), pid(actors[0] + 0x18), pid(addresses[10] + 0x28), pid(addresses[11] + 0x28),
                 pid(addresses[30] + 4), pid(addresses[31] + 4), pid(addresses[20] + 8), pid(addresses[50] + 0x30),
                 pid(addresses[50] + 0x34), pid(addresses[50] + 0x3c), pid(addresses[20]), pid(addresses[20] + 4)]
    return {"result": result, "state": state, "frees": frees}


results = []
for mode, count in [(0, 7), (1, 9)]:
    for scenario in range(count):
        native = json.loads(subprocess.check_output([str(out / "harness.exe"), str(mode), str(scenario)], text=True))
        expected = original(mode, scenario)
        if native != expected:
            raise AssertionError(f"mode={mode} case={scenario}: C={native}, original={expected}")
        results.append({"mode": "model_switch" if mode == 0 else "cleanup", "case": scenario, "result": native})
(out / "results.json").write_text(json.dumps(results, indent=2) + "\n")
inputs = ["src/carma2/newcommon/" + file + suffix for file in ("00-car", "08-loading1", "13-crush1", "51-smash", "41-utility") for suffix in (".c", ".h")]
inputs.extend(["src/carma2/include/rec2_types.h", "src/brender/include/brender/br_types.h", "src/carma2/hooks/c2_hooks.h"])
manifest = {
    "original_sha256": hashlib.sha256(binary).hexdigest(),
    "original_path": str(args.original.resolve()), "compiler_command": compile_command,
    "compiler_version": subprocess.check_output([args.cc, "--version"], text=True).splitlines()[0],
    "harness_sha256": hashlib.sha256(driver.encode()).hexdigest(),
    "source_sha256": {path: hashlib.sha256((ROOT / path).read_bytes()).hexdigest() for path in inputs},
    "case_count": len(results), "all_cases_passed": True,
    "scope": "Actual current function bodies and actual 32-bit repository types against original x86; BrMemFree records addresses without releasing memory.",
}
(out / "verification.json").write_text(json.dumps(manifest, indent=2) + "\n")
print(f"Passed {len(results)} differential cases: null/missing LODs, nested/sibling actor traversal, signed detail counts, aliased models, exact free order and pointer retention")
