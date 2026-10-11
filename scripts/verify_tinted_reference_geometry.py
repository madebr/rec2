#!/usr/bin/env python3
"""Verify SetTintedPolyRefMaterial through the actual finite raycast closure.

Imports the retained geometry fixture without replacing FindFace or any of
the seven reconstructed geometry bodies. Native builds also compile actual
BRender inverse/apply arithmetic; transform dispatch uses the geometry
verifier's declared matrix/translation/identity boundary. Complete geometry,
overlay and reference-owner state is compared against original x86. Only
unused inverse-matrix signed zeros receive the geometry verifier's documented
canonicalization. The separate boundary suite covers arbitrary ray outputs.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import subprocess
import sys

from verify_tinted_native_reconstruction import extract
import verify_finteray_reconstruction as geometry

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "src/carma2/newcommon/66-tintedpoly.c"
ADDRESSES = dict(geometry.ADDRESSES, SetTintedPolyRefMaterial=0x4d8350,
                gTintedPolys=0x705c80, gCar_to_view=0x655f54, gAction_replay_mode=0x676914)
REFERENCE, REFERENCE_SIZE, COLLISION, VOLUME = geometry.HEAP + 0x6000, 7960, 6500, 7740
GRAVITIES = [0x3f800000, 0x3f800000, 0x3f000000, 0x7fc00000]


class ReferenceFixture(geometry.Fixture):
    def __init__(self, image, addresses, case):
        super().__init__(image, addresses, case)
        options = case["options"]
        self.u.mem_write(REFERENCE, b"\xa5" * REFERENCE_SIZE)
        self.write(REFERENCE + 8, REFERENCE + COLLISION)
        self.write(REFERENCE + COLLISION + 0x258, REFERENCE + VOLUME if options["volume"] else 0)
        self.write(REFERENCE + VOLUME + 0x88, GRAVITIES[options["volume"]])
        self.write(addresses["gCar_to_view"], REFERENCE)
        self.write(addresses["gAction_replay_mode"], options["replay"])
        pool = addresses["gTintedPolys"]
        self.u.mem_write(pool, b"\xa5" * (10 * 0x6450))
        for i in range(10):
            self.write(pool + i * 0x6450 + 0x2c, geometry.HEAP + geometry.MATERIALS + 3 * 256)

    def execute(self):
        from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EIP, UC_X86_REG_ECX, UC_X86_REG_EDX
        options = self.case["options"]
        for repeat in range(options["repeat"]):
            self.write(geometry.STACK, geometry.STOP)
            self.u.reg_write(UC_X86_REG_ESP, geometry.STACK)
            self.u.reg_write(UC_X86_REG_ECX, options["index"])
            self.u.reg_write(UC_X86_REG_EDX, geometry.HEAP + 0x40)
            self.u.emu_start(self.addresses["SetTintedPolyRefMaterial"], geometry.STOP, count=1000000)
            if self.u.reg_read(UC_X86_REG_EIP) != geometry.STOP:
                raise AssertionError("Integrated tinted instruction budget exhausted")
        pool = bytes(self.u.mem_read(self.addresses["gTintedPolys"], 10 * 0x6450))
        owner = bytes(self.u.mem_read(REFERENCE, REFERENCE_SIZE))
        state = geometry.words([self.read(self.addresses["gAction_replay_mode"]), self.read(self.addresses["gCar_to_view"])]) + owner + pool
        result = geometry.words([0, 0]) + self.snapshot() + state
        return result


def transform_driver(source):
    replacements = {
        "unsigned header[11]": "unsigned header[15]",
        "    switch(header[0]){": "    reference_init(header[12],header[13]);\n    switch(header[0]){",
        "    default:return 90;}": "    case 7:for(i=0;i<header[14];i++)SetTintedPolyRefMaterial((int)header[11],(br_vector3*)(a+0x40));break;\n    default:return 90;}",
        "    emit(0);emit((unsigned)r);snapshot();return 0;": "    emit(0);emit((unsigned)r);snapshot();reference_emit();return 0;",
    }
    for before, after in replacements.items():
        if source.count(before) != 1:
            raise AssertionError(f"Geometry driver extraction changed: {before}")
        source = source.replace(before, after)
    return source


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--original", type=Path, default=ROOT / "build-reference/originals/CARMA2_HW.EXE")
    parser.add_argument("--dependency-dir", type=Path, default=ROOT / "build-audit/joystick/python-deps")
    parser.add_argument("--gcc", type=Path, default=ROOT / "build-tools/winlibs-i686/mingw32/bin/gcc.exe")
    parser.add_argument("--msvc-root", type=Path, default=ROOT / "build-tools/msvc-modern")
    parser.add_argument("--output", type=Path, default=ROOT / "build-audit/tinted-reference-geometry")
    parser.add_argument("--symbols", type=Path)
    parser.add_argument("--recompiled", type=Path, default=ROOT / "build-matching/rec2.exe")
    args = parser.parse_args()
    sys.path.insert(0, str(args.dependency_dir.resolve()))
    original = args.original.read_bytes()
    if hashlib.sha256(original).hexdigest() != geometry.ORIGINAL_SHA256:
        raise SystemExit("Original executable hash differs")
    rebuilt = None
    rebuilt_addresses = {}
    if args.symbols:
        mapping = json.loads(args.symbols.read_text())
        rebuilt = args.recompiled.read_bytes()
        if mapping["format"] != 1 or mapping["original_sha256"] != geometry.ORIGINAL_SHA256 or mapping["recompiled_sha256"] != hashlib.sha256(rebuilt).hexdigest():
            raise SystemExit("Stale or incompatible integrated geometry symbol map")
        symbols = {s["orig"]: s["recomp"] for s in mapping["symbols"]}
        rebuilt_addresses = {name: symbols[address] for name, address in ADDRESSES.items()}
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=True)
    extra_bodies = r'''
#include "66-tintedpoly.h"
typedef char reference_layout[sizeof(tCar_spec)==6500&&sizeof(tPhysics_object)==1240&&sizeof(tSpecial_volume)==220&&sizeof(tTintedPoly)==0x6450?1:-1];
tTintedPoly gTintedPolys[10];
tCar_spec* gCar_to_view;
int gAction_replay_mode;
typedef struct {tCar_spec car;tPhysics_object collision;tSpecial_volume volume;} reference_heap;
static reference_heap reference_owner;
static void reference_init(unsigned volume,unsigned replay);
static void reference_emit(void);
''' + extract(SOURCE.read_text(), "SetTintedPolyRefMaterial")
    extra_driver = r'''
static unsigned reference_norm(unsigned value) {
    unsigned base=(unsigned)(uintptr_t)&reference_owner;
    return value>=base&&value<base+sizeof(reference_owner)?value-base+0x1006000:value;
}
static void reference_init(unsigned volume,unsigned replay) {
    static const unsigned gravities[4]={0x3f800000,0x3f800000,0x3f000000,0x7fc00000};
    unsigned i;
    memset(&reference_owner,0xa5,sizeof(reference_owner));memset(gTintedPolys,0xa5,sizeof(gTintedPolys));
    reference_owner.car.collision_info=&reference_owner.collision;
    reference_owner.collision.last_special_volume=volume?&reference_owner.volume:NULL;
    memcpy(&reference_owner.volume.gravity_multiplier,&gravities[volume],4);
    gCar_to_view=&reference_owner.car;gAction_replay_mode=(int)replay;
    for(i=0;i<10;i++)gTintedPolys[i].material2=(br_material*)((char*)arena+0x3500);
}
static void reference_emit(void) {
    unsigned i,v;unsigned char copy[sizeof(reference_owner)],pool[sizeof(gTintedPolys)];
    emit((unsigned)gAction_replay_mode);emit(reference_norm((unsigned)(uintptr_t)gCar_to_view));
    memcpy(copy,&reference_owner,sizeof(copy));
    v=reference_norm((unsigned)(uintptr_t)reference_owner.car.collision_info);memcpy(copy+8,&v,4);
    v=reference_norm((unsigned)(uintptr_t)reference_owner.collision.last_special_volume);memcpy(copy+6500+0x258,&v,4);
    fwrite(copy,sizeof(copy),1,stdout);
    memcpy(pool,gTintedPolys,sizeof(pool));
    for(i=0;i<10;i++){v=norm((unsigned)(uintptr_t)gTintedPolys[i].material2);memcpy(pool+i*0x6450+0x2c,&v,4);}
    fwrite(pool,sizeof(pool),1,stdout);
}
'''
    commands = geometry.compile_native(output, args.gcc, args.msvc_root, extra_bodies, extra_driver, transform_driver)
    cases = []
    for identifier in ("!", "#", "@", "normal"):
        for position in ([0.25, -2, 0.25], [2, -2, 2], [0.25, 2, 0.25]):
            for index in (0, 9):
                for volume, replay in ((0, 0), (0, 1), (1, 0), (2, 0), (3, 0)):
                    cases.append(dict(mode=7, identifier=identifier, rp=position, rd=[0, 200, 0], axis=1, sign=-1, index=index, volume=volume, replay=replay, repeat=2))
    results = []
    for index, options in enumerate(cases):
        case = geometry.make_case(options)
        expected = ReferenceFixture(original, ADDRESSES, case).execute()
        path = output / "case.bin"
        path.write_bytes(geometry.words([7, case["callback_return"], case["mutate"], *case["globals"], 0, options["index"], options["volume"], options["replay"], options["repeat"]]) + case["data"])
        checks = [(compiler, subprocess.check_output([str(output / (compiler + ".exe")), str(path)])) for compiler in commands]
        if rebuilt is not None:
            checks.append(("matching PE", ReferenceFixture(rebuilt, rebuilt_addresses, case).execute()))
        for compiler, actual in checks:
            if actual != expected:
                (output / f"failure-{index}-{compiler.replace(' ', '-')}.json").write_text(json.dumps({"options": options, "original": expected.hex(), "actual": actual.hex()}, indent=2))
                raise AssertionError(f"Integrated tinted case {index}, {compiler} differs")
            results.append(dict(options=options, compiler=compiler, all_state_bytes_match=True, state_sha256=hashlib.sha256(actual).hexdigest()))
        # Prove the finite inside-triangle scenarios actually retain a material.
        if options["rp"] == [0.25, -2, 0.25] and (options["volume"] in (2, 3) or options["replay"]) and options["identifier"] in ("!", "#", "@"):
            selected = len(expected) - 10 * 0x6450 + options["index"] * 0x6450
            tag = expected[selected + 0x28]
            material = struct.unpack_from("<I", expected, selected + 0x2c)[0]
            assert tag == ord("@" if options["identifier"] == "@" else "#") and material != 0
        if (index + 1) % 30 == 0:
            print(f"Verified {index + 1}/{len(cases)} integrated tinted geometry scenarios", flush=True)
    (output / "results.json").write_text(json.dumps(results, indent=2) + "\n")
    manifest = dict(original_sha256=geometry.ORIGINAL_SHA256, comparisons=len(results), all_cases_passed=True, compiler_commands=commands,
                    source_sha256={str(path.relative_to(ROOT)): hashlib.sha256(path.read_bytes()).hexdigest() for path in (SOURCE, ROOT / "src/carma2/newcommon/00-car.c", ROOT / "src/carma2/newcommon/34-finteray.c", ROOT / "scripts/verify_finteray_reconstruction.py")},
                    harness_sha256=hashlib.sha256((output / "harness.c").read_bytes()).hexdigest(), limitations="Finite real geometry only; inherited transform-dispatch boundary and unused inverse-matrix zero-sign canonicalization are documented in verify_finteray_reconstruction.py.")
    if rebuilt is not None:
        manifest["recompiled_sha256"] = hashlib.sha256(rebuilt).hexdigest()
        manifest["symbols_sha256"] = hashlib.sha256(args.symbols.read_bytes()).hexdigest()
    (output / "verification.json").write_text(json.dumps(manifest, indent=2) + "\n")
    print(f"Passed {len(results)} integrated tinted reference geometry comparisons")


if __name__ == "__main__":
    main()
