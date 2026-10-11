#!/usr/bin/env python3
"""Compare the actual tinted reference-material body with original x86.

Uses repository 32-bit types and compiles production SetTintedPolyRefMaterial
plus the real Pling switch bodies under modern MSVC and MinGW. FindFace is an
explicit dependency boundary: its arguments, call order and output branches
are controlled. Complete overlay pool and caller/object bytes are compared.
This boundary suite does not claim to verify the separate raycast algorithm.
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
PLING_SOURCE = ROOT / "src/carma2/newcommon/34-finteray.c"
ORIGINAL_HASH = "9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d"
GRAVITIES = [0x3f800000, 0x3f800000, 0x3f000000, 0x40000000, 0xbf800000,
             0x7f800000, 0xff800000, 0x7fc00000, 0xffc00000, 0x7f800001, 0xff800001]
IDENTIFIERS = ["ignored", "ignored", "", "!", "#", "@", "road", "!water"]
FIELDS = "index volume replay distance material pling repeat mutate seed".split()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--original", type=Path, default=ROOT / "build-reference/originals/CARMA2_HW.EXE")
    parser.add_argument("--dependency-dir", type=Path, default=ROOT / "build-audit/joystick/python-deps")
    parser.add_argument("--gcc", type=Path, default=ROOT / "build-tools/winlibs-i686/mingw32/bin/gcc.exe")
    parser.add_argument("--msvc-root", type=Path, default=ROOT / "build-tools/msvc-modern")
    parser.add_argument("--output", type=Path, default=ROOT / "build-audit/tinted-reference-tests")
    parser.add_argument("--case-index", type=int, action="append", help="Run only these zero-based diagnostic scenarios")
    args = parser.parse_args()
    sys.path.insert(0, str(args.dependency_dir.resolve()))
    from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
    from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EIP, UC_X86_REG_ECX, UC_X86_REG_EDX, UC_X86_REG_FPCW

    binary = args.original.read_bytes()
    if hashlib.sha256(binary).hexdigest() != ORIGINAL_HASH:
        raise SystemExit("Original executable hash differs from verified binary")
    out = args.output.resolve()
    out.mkdir(parents=True, exist_ok=True)
    source = SOURCE.read_text()
    pling_source = PLING_SOURCE.read_text()
    driver = r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <float.h>
#include "66-tintedpoly.h"
#include "34-finteray.h"
#include "globvars.h"
#include "rec2_macros.h"
typedef char check_layout[sizeof(void*)==4&&sizeof(tTintedPoly)==0x6450&&sizeof(br_material)==156&&offsetof(tTintedPoly,field_0x28)==0x28&&offsetof(tTintedPoly,material2)==0x2c&&offsetof(tCar_spec,collision_info)==8&&offsetof(tPhysics_object,last_special_volume)==0x258&&offsetof(tSpecial_volume,gravity_multiplier)==0x88?1:-1];
enum {INDEX,VOLUME,REPLAY,DISTANCE,MATERIAL,PLING,REPEAT,MUTATE,SEED};
static unsigned a[9],step;
static const unsigned gravities[11]={0x3f800000,0x3f800000,0x3f000000,0x40000000,0xbf800000,0x7f800000,0xff800000,0x7fc00000,0xffc00000,0x7f800001,0xff800001};
static const char* identifiers[8]={"ignored","ignored","","!","#","@","road","!water"};
typedef struct {tCar_spec car;tPhysics_object collision;tSpecial_volume volume;br_material materials[2];br_vector3 position;char identifier[80];} fixture_heap;
static fixture_heap heap;
tTintedPoly gTintedPolys[10];
tCar_spec* gCar_to_view;
int gAction_replay_mode;
int gPling_materials;
static unsigned events[32][11],event_count;
static unsigned norm(unsigned value) {
    unsigned base=(unsigned)(uintptr_t)&heap;
    return value>=base&&value<base+sizeof(heap)?value-base+0x1000000:value;
}
static void record(unsigned code,br_vector3* position,br_vector3* dir) {
    unsigned* e;
    if(event_count>=32)abort();
    e=events[event_count++];
    e[0]=code;e[1]=(unsigned)gPling_materials;e[2]=gTintedPolys[a[INDEX]].field_0x28;
    e[3]=norm((unsigned)(uintptr_t)gTintedPolys[a[INDEX]].material2);
    if(position){e[4]=norm((unsigned)(uintptr_t)position);memcpy(e+5,position,12);memcpy(e+8,dir,12);}
}
'''
    # Rename only the switch definitions so boundary wrappers can observe order;
    # the compiled switch bodies themselves are exactly the production text.
    for name in ("DisablePlingMaterials", "EnablePlingMaterials"):
        body = extract(pling_source, name)
        driver += body.replace(name + "(", "real_" + name + "(", 1)
    driver += r'''
void C2_HOOK_FASTCALL DisablePlingMaterials(void) {record(1,NULL,NULL);real_DisablePlingMaterials();}
void C2_HOOK_FASTCALL EnablePlingMaterials(void) {record(3,NULL,NULL);real_EnablePlingMaterials();}
void C2_HOOK_FASTCALL FindFace(br_vector3* position,br_vector3* dir,br_vector3* nor,br_scalar* t,br_material** mat) {
    unsigned material=(a[MATERIAL]+step)%8;
    record(2,position,dir);
    memcpy(t,&a[DISTANCE],4);
    BrVector3Set(nor,2.0f,3.0f,4.0f);
    *mat=material==0?NULL:&heap.materials[0];
    if(a[MUTATE]){
        position->v[0]+=1.0f;gPling_materials=-17;
        gTintedPolys[a[INDEX]].field_0x28=0x12;
        gTintedPolys[a[INDEX]].material2=&heap.materials[1];
    }
}
'''
    driver += extract(source, "SetTintedPolyRefMaterial")
    driver += r'''
static void normalize(void* p,unsigned offset) {
    unsigned value;memcpy(&value,(char*)p+offset,4);value=norm(value);memcpy((char*)p+offset,&value,4);
}
static void dump(const void* p,unsigned size) {
    unsigned i;for(i=0;i<size;i++)printf("%02x",((const unsigned char*)p)[i]);
}
int main(int argc,char**argv) {
    unsigned i,j,material;
    if(argc==2){printf("[%u,%u,%u,%u,%u,%u,%u]\n",(unsigned)offsetof(fixture_heap,car),(unsigned)offsetof(fixture_heap,collision),(unsigned)offsetof(fixture_heap,volume),(unsigned)offsetof(fixture_heap,materials),(unsigned)offsetof(fixture_heap,position),(unsigned)offsetof(fixture_heap,identifier),(unsigned)sizeof(heap));return 0;}
    if(argc!=10)abort();
    for(i=0;i<9;i++)a[i]=(unsigned)strtoul(argv[i+1],NULL,0);
    _controlfp(_PC_53,_MCW_PC);
    memset(&heap,(int)a[SEED],sizeof(heap));memset(gTintedPolys,(int)a[SEED],sizeof(gTintedPolys));
    heap.car.collision_info=&heap.collision;heap.collision.last_special_volume=a[VOLUME]?&heap.volume:NULL;
    memcpy(&heap.volume.gravity_multiplier,&gravities[a[VOLUME]],4);
    BrVector3Set(&heap.position,1.25f,-3.0f,7.5f);
    gCar_to_view=&heap.car;gAction_replay_mode=(int)a[REPLAY];gPling_materials=(int)a[PLING];
    for(i=0;i<10;i++)gTintedPolys[i].material2=&heap.materials[1];
    for(step=0;step<a[REPEAT];step++){
        material=(a[MATERIAL]+step)%8;
        strcpy(heap.identifier,identifiers[material]);
        heap.materials[0].identifier=material==1?NULL:heap.identifier;
        SetTintedPolyRefMaterial((int)a[INDEX],&heap.position);
    }
    for(i=0;i<10;i++)normalize(&gTintedPolys[i],0x2c);
    normalize(&heap.car,8);normalize(&heap.collision,0x258);normalize(&heap.materials[0],4);
    printf("{\"pool\":\"");dump(gTintedPolys,sizeof(gTintedPolys));
    printf("\",\"heap\":\"");dump(&heap,sizeof(heap));printf("\",\"pling\":%u,\"events\":[",(unsigned)gPling_materials);
    for(i=0;i<event_count;i++){printf("%s[",i?",":"");for(j=0;j<11;j++)printf("%s%u",j?",":"",events[i][j]);printf("]");}
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
    car, collision, volume, materials, position, identifier, heap_size = schemas[0]
    pe = struct.unpack_from("<I", binary, 60)[0]
    sections = struct.unpack_from("<H", binary, pe + 6)[0]
    table = pe + 24 + struct.unpack_from("<H", binary, pe + 20)[0]

    def original(c):
        u = Uc(UC_ARCH_X86, UC_MODE_32)
        u.mem_map(0x400000, 0x400000)
        u.mem_map(0x1000000, 0x10000)
        for i in range(sections):
            _, rva, size, offset = struct.unpack_from("<IIII", binary, table + i * 40 + 8)
            u.mem_write(0x400000 + rva, binary[offset:offset + size])
        base, pool = 0x1000000, 0x705c80

        def wr(address, value):
            u.mem_write(address, struct.pack("<I", value & 0xffffffff))

        def rd(address):
            return struct.unpack("<I", u.mem_read(address, 4))[0]

        u.mem_write(base, bytes([c["seed"]]) * heap_size)
        u.mem_write(pool, bytes([c["seed"]]) * (10 * 0x6450))
        wr(base + car + 8, base + collision)
        wr(base + collision + 0x258, base + volume if c["volume"] else 0)
        wr(base + volume + 0x88, GRAVITIES[c["volume"]])
        u.mem_write(base + position, struct.pack("<fff", 1.25, -3.0, 7.5))
        wr(0x655f54, base + car)
        wr(0x676914, c["replay"])
        wr(0x5964c0, c["pling"])
        for i in range(10):
            wr(pool + i * 0x6450 + 0x2c, base + materials + 156)
        events = []
        selected = pool + c["index"] * 0x6450
        step = 0

        def hook(uc, address, size, context):
            code = {0x45d5d0: 1, 0x41e340: 2, 0x45d5c0: 3}[address]
            event = [code, rd(0x5964c0), uc.mem_read(selected + 0x28, 1)[0], rd(selected + 0x2c), *([0] * 7)]
            if code != 2:
                events.append(event)
                return  # Execute the original switch body.
            sp = uc.reg_read(UC_X86_REG_ESP)
            pos = uc.reg_read(UC_X86_REG_ECX)
            direction = uc.reg_read(UC_X86_REG_EDX)
            assert pos == base + position
            event[4] = pos
            event[5:8] = struct.unpack("<III", uc.mem_read(pos, 12))
            event[8:11] = struct.unpack("<III", uc.mem_read(direction, 12))
            events.append(event)
            nor, t, mat = [rd(sp + 4 + i * 4) for i in range(3)]
            uc.mem_write(nor, struct.pack("<fff", 2.0, 3.0, 4.0))
            wr(t, c["distance"])
            wr(mat, 0 if (c["material"] + step) % 8 == 0 else base + materials)
            if c["mutate"]:
                x = struct.unpack("<f", uc.mem_read(pos, 4))[0]
                uc.mem_write(pos, struct.pack("<f", x + 1.0))
                wr(0x5964c0, -17)
                uc.mem_write(selected + 0x28, b"\x12")
                wr(selected + 0x2c, base + materials + 156)
            uc.reg_write(UC_X86_REG_EIP, rd(sp))
            uc.reg_write(UC_X86_REG_ESP, sp + 16)

        for entry in (0x45d5d0, 0x41e340, 0x45d5c0):
            u.hook_add(UC_HOOK_CODE, hook, begin=entry, end=entry)
        for step in range(c["repeat"]):
            material = (c["material"] + step) % 8
            u.mem_write(base + identifier, IDENTIFIERS[material].encode() + b"\0")
            wr(base + materials + 4, 0 if material == 1 else base + identifier)
            wr(0x100e000, 0x100f000)
            u.reg_write(UC_X86_REG_ESP, 0x100e000)
            u.reg_write(UC_X86_REG_ECX, c["index"])
            u.reg_write(UC_X86_REG_EDX, base + position)
            u.reg_write(UC_X86_REG_FPCW, 0x23f)
            u.emu_start(0x4d8350, 0x100f000, count=100000)
            assert u.reg_read(UC_X86_REG_EIP) == 0x100f000
        return {"pool": bytes(u.mem_read(pool, 10 * 0x6450)).hex(), "heap": bytes(u.mem_read(base, heap_size)).hex(), "pling": rd(0x5964c0), "events": events}

    def case(**kw):
        c = dict(index=0, volume=2, replay=0, distance=0, material=4, pling=7, repeat=1, mutate=0, seed=165)
        c.update(kw)
        return c

    cases = [case(index=index, volume=gravity, replay=replay) for index in (0, 9) for gravity in range(len(GRAVITIES)) for replay in (0, 1)]
    distances = [0xbf800000, 0, 0x42c7ffff, 0x42c80000, 0x42c80001, 0x7f800000, 0xff800000, 0x7fc00000, 0xffc00000, 0x7f800001, 0xff800001]
    cases += [case(index=index, distance=distance, material=material) for index in (0, 9) for distance in distances for material in range(8)]
    cases += [case(index=index, volume=gravity, repeat=4, mutate=1) for index in (0, 9) for gravity in (0, 2, 7)]
    if args.case_index is not None and any(index < 0 or index >= len(cases) for index in args.case_index):
        parser.error(f"--case-index must be between 0 and {len(cases) - 1}")
    results = []
    for index, c in enumerate(cases):
        if args.case_index is not None and index not in args.case_index:
            continue
        expected = original(c)
        for compiler in commands:
            actual = json.loads(subprocess.check_output([str(out / f"{compiler}.exe"), *[str(c[key] & 0xffffffff) for key in FIELDS]], text=True))
            if actual != expected:
                failure = out / f"failure-{compiler}-{index}.json"
                failure.write_text(json.dumps({"case": c, "original": expected, "native": actual}, indent=2))
                raise AssertionError(f"Reference {compiler} case {index} differs: {failure}")
            results.append({"compiler": compiler, "case_index": index, "case": c, "all_bytes_calls_match": True})
        if (index + 1) % 40 == 0:
            print(f"Verified {index + 1}/{len(cases)} scenarios", flush=True)
    (out / "results.json").write_text(json.dumps(results, indent=2) + "\n")
    manifest = {
        "original_sha256": ORIGINAL_HASH, "all_cases_passed": True, "case_count": len(results), "complete_suite": args.case_index is None,
        "functions": {"SetTintedPolyRefMaterial": "0x4d8350", "EnablePlingMaterials": "0x45d5c0", "DisablePlingMaterials": "0x45d5d0"},
        "type_schema": schemas[0], "compiler_commands": commands, "source_sha256": {str(path.relative_to(ROOT)): hashlib.sha256(path.read_bytes()).hexdigest() for path in (SOURCE, PLING_SOURCE, ROOT / "src/carma2/newcommon/66-tintedpoly.h", ROOT / "src/carma2/include/rec2_types.h")},
        "harness_sha256": hashlib.sha256(driver.encode()).hexdigest(), "limitations": "FindFace is a controlled boundary; verify the actual raycast closure separately. Original normal CRT precision PC53 is used.",
    }
    (out / "verification.json").write_text(json.dumps(manifest, indent=2) + "\n")
    print(f"Passed {len(results)} native/original tinted reference comparisons")


if __name__ == "__main__":
    main()
