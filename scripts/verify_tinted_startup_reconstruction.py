#!/usr/bin/env python3
"""Execute original/reconstructed tinted overlay routines with identical fixtures.

Requires Unicorn and a fresh symbol map exported by verify_input_reconstruction.py
export-map. Both images' hashes are checked. Executes mesh and color calculations
as actual x86, while BRender allocation/registration and image loading are controlled
fixtures. Compares the complete overlay pool, allocation bytes, material snapshots,
dependency order, shared opacity tokens, return values and observed fault classes.
This is not a rendering or BRender allocator integration test.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import sys

ROOT = Path(__file__).resolve().parents[1]
ORIGINAL_HASH = "9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d"
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--original", type=Path, default=ROOT / "build-reference/originals/CARMA2_HW.EXE")
parser.add_argument("--recompiled", type=Path, default=ROOT / "build-matching/rec2.exe")
parser.add_argument("--symbols", type=Path, required=True)
parser.add_argument("--dependency-dir", type=Path, default=ROOT / "build-audit/joystick/python-deps")
parser.add_argument("--output", type=Path, default=ROOT / "build-audit/tinted-startup/results.json")
def configure(argv=None):
    """Load and validate fixture images; importing this module does not run cases."""
    global args, original, recompiled, mapping, symbols
    global Uc, UcError, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
    global UC_X86_REG_ESP, UC_X86_REG_EIP, UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX, UC_X86_REG_FPCW
    args = parser.parse_args(argv)
    sys.path.insert(0, str(args.dependency_dir.resolve()))
    from unicorn import Uc, UcError, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
    from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EIP, UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX, UC_X86_REG_FPCW

    original, recompiled = args.original.read_bytes(), args.recompiled.read_bytes()
    mapping = json.loads(args.symbols.read_text())
    if hashlib.sha256(original).hexdigest() != ORIGINAL_HASH or mapping.get("original_sha256") != ORIGINAL_HASH:
        parser.error("Incorrect original executable or symbol map")
    if mapping.get("format") != 1 or mapping.get("recompiled_sha256") != hashlib.sha256(recompiled).hexdigest():
        parser.error("Stale symbol map: export a fresh map after rebuilding")
    symbols = {s["name"]: s for s in mapping["symbols"]}
    return args


TOKENS = ["gTinted_poly_material_tokens", "gTinted_poly_zero_opacity_tokens", "gTinted_poly_custom_opacity_tokens"]
POOL_SIZE, STRIDE = 10 * 0x6450, 0x6450
HEAP, STACK, STOP = 0x1000000, 0x1200000, 0x1210000


class Fixture:
    def __init__(self, data, side, case):
        self.side, self.case = side, case
        self.u = Uc(UC_ARCH_X86, UC_MODE_32)
        self.u.mem_map(0x400000, 0x500000)
        self.u.mem_map(HEAP, 0x220000)
        pe = struct.unpack_from("<I", data, 0x3c)[0]
        count = struct.unpack_from("<H", data, pe + 6)[0]
        table = pe + 24 + struct.unpack_from("<H", data, pe + 20)[0]
        for i in range(count):
            _, rva, size, offset = struct.unpack_from("<IIII", data, table + i * 40 + 8)
            self.u.mem_write(0x400000 + rva, data[offset:offset + size])
        self.next = HEAP
        self.allocations, self.calls = [], []
        self.status = "returned"
        self.pool = self.addr("gTintedPolys")
        self.u.mem_write(self.pool, bytes((i * 37 + 165) & 255 for i in range(POOL_SIZE)))
        for i in range(10):
            self.write(self.pool + i * STRIDE + 0x30, 1)
            self.write(self.pool + i * STRIDE, 0)
        slot = case.get("slot", 0)
        if slot >= 0:
            self.write(self.pool + slot * STRIDE + 0x30, 0)
            self.write(self.pool + slot * STRIDE, 0x12345678)  # Free despite nonnull old actor.
        for i in range(len(case["sequence"]) + 1 if "sequence" in case else 0):
            self.write(self.pool + i * STRIDE + 0x30, 0)
        self.screen = self.allocate("screen", 0x44)
        self.u.mem_write(self.screen + 0x34, struct.pack("<HH", *case.get("screen", (640, 480))))
        self.write(self.addr("gScreen"), self.screen)
        self.camera = self.allocate_actor(3)
        self.write(self.addr("gTintedPolyCamera"), 0 if case.get("new_camera") else self.camera)
        self.write(self.addr("gDefaultOpacity_TintedPoly"), case.get("opacity", 128))
        self.maps = {}
        for name, width, height in (("testfire", 32, 64), ("firepal1", 1, 256), ("coolmapx", 32, 64)):
            address = self.allocate("map:" + name, 0x44)
            self.u.mem_write(address + 0x34, struct.pack("<HH", width, height))
            self.maps[name] = address
        self.fail = case.get("fail")
        self.calls = []
        dependencies = ["BrActorAllocate", "BrMaterialAllocate", "BrModelAllocate", "BrMaterialAdd",
                        "BrMaterialUpdate", "BrModelAdd", "BrActorAdd", "BrMapFind", "BrPixelmapPixelGet",
                        "PathCat", "LoadAllImagesInDirectory", "PDFatalError"]
        self.hooks = {self.addr(name): name for name in dependencies}
        self.u.hook_add(UC_HOOK_CODE, self.hook)
        self.u.reg_write(UC_X86_REG_FPCW, 0x37f)

    def addr(self, name):
        if name in TOKENS:
            return self.addr("gTinted_poly_tokens") + TOKENS.index(name) * 16
        if name == "gDefaultOpacity_TintedPoly":
            return self.addr("gTinted_poly_tokens") + 52
        return symbols[name][self.side]

    def write(self, address, value):
        self.u.mem_write(address, struct.pack("<I", value & 0xffffffff))

    def read(self, address):
        return struct.unpack("<I", self.u.mem_read(address, 4))[0]

    def string(self, address):
        result = bytearray()
        while len(result) < 1024:
            value = self.u.mem_read(address + len(result), 1)[0]
            if not value:
                return result.decode("latin1")
            result.append(value)
        raise AssertionError("Unterminated fixture string")

    def allocate(self, kind, size):
        pointer = self.next
        self.next += (size + 15) & ~15
        assert self.next < HEAP + 0x100000
        self.u.mem_write(pointer, b"\xa5" * size)
        self.allocations.append((kind, pointer, size))
        return pointer

    def allocate_actor(self, kind):
        actor = self.allocate("actor", 100)
        if kind == 3:
            self.write(actor + 0x5c, self.allocate("camera", 36))
        return actor

    def material_state(self, pointer):
        data = bytearray(self.u.mem_read(pointer, 0x9c))
        tokens = []
        extra = self.read(pointer + 0x58)
        for i, name in enumerate(TOKENS):
            if extra == self.addr(name):
                tokens = list(struct.unpack("<4I", self.u.mem_read(extra, 16)))
                struct.pack_into("<I", data, 0x58, 0x2000000 + i * 16)
        return [data.hex(), tokens]

    def hook(self, uc, address, size, context):
        name = self.hooks.get(address)
        if name is None:
            return
        sp = uc.reg_read(UC_X86_REG_ESP)
        a = [self.read(sp + 4 + i * 4) for i in range(3)]
        ecx, edx = uc.reg_read(UC_X86_REG_ECX), uc.reg_read(UC_X86_REG_EDX)
        result, cleanup = 0, 0
        if name == "BrMaterialAllocate":
            self.calls.append([name, self.string(a[0])])
            result = 0 if self.fail == "material" else self.allocate("material", 0x9c)
        elif name == "BrModelAllocate":
            nv, nf = a[1] & 65535, a[2] & 65535
            self.calls.append([name, self.string(a[0]), nv, nf])
            if self.fail != "model":
                result = self.allocate("model", 0x5c)
                self.write(result + 8, self.allocate("vertices", nv * 40))
                self.write(result + 12, self.allocate("faces", nf * 40))
                self.u.mem_write(result + 0x10, struct.pack("<HH", nv, nf))
        elif name == "BrActorAllocate":
            self.calls.append([name, a[0], a[1]])
            result = 0 if self.fail == "actor" else self.allocate_actor(a[0])
        elif name in ("BrMaterialAdd", "BrMaterialUpdate"):
            self.calls.append([name, a[0], a[1] if name.endswith("Update") else None, self.material_state(a[0])])
            result = a[0]
        elif name in ("BrModelAdd", "BrActorAdd"):
            self.calls.append([name, a[:2] if name == "BrActorAdd" else a[:1]])
            result = a[1] if name == "BrActorAdd" else a[0]
        elif name == "BrMapFind":
            key = self.string(a[0])
            self.calls.append([name, key])
            result = self.maps[key]
        elif name == "BrPixelmapPixelGet":
            key = next(k for k, v in self.maps.items() if v == a[0])
            self.calls.append([name, key, a[1], a[2]])
            result = ((a[2] * 0x10203) | 0xff000000) if key == "firepal1" else (7 if self.case.get("flat_map") else (a[1] * 3 + a[2] * 7) & 31)
        elif name == "PathCat":
            self.calls.append([name, self.string(a[0])])
            self.u.mem_write(ecx, b"C:/DATA/" + self.string(a[0]).encode() + b"\0")
            cleanup = 4
        elif name == "LoadAllImagesInDirectory":
            assert ecx == self.addr("gMisc_storage_space")
            self.calls.append([name, self.string(edx)])
        elif name == "PDFatalError":
            self.calls.append([name, self.string(ecx)])
            self.status = "fatal"
            uc.emu_stop()
            return
        uc.reg_write(UC_X86_REG_EAX, result)
        uc.reg_write(UC_X86_REG_EIP, self.read(sp))
        uc.reg_write(UC_X86_REG_ESP, sp + 4 + cleanup)

    def invoke(self, name, values):
        self.write(STACK, STOP)
        for i, value in enumerate(values[2:]):
            self.write(STACK + 4 + i * 4, value)
        self.u.reg_write(UC_X86_REG_ESP, STACK)
        self.u.reg_write(UC_X86_REG_ECX, values[0] & 0xffffffff if values else 0)
        self.u.reg_write(UC_X86_REG_EDX, values[1] & 0xffffffff if len(values) > 1 else 0)
        try:
            self.u.emu_start(self.addr(name), STOP, count=2000000)
            if self.status == "returned" and self.u.reg_read(UC_X86_REG_EIP) != STOP:
                raise AssertionError(name + " instruction budget exhausted")
        except UcError as error:
            self.status = "fault:" + str(error.errno)
        return self.u.reg_read(UC_X86_REG_EAX)

    def run(self):
        mode = self.case["mode"]
        if mode == "init":
            value = self.invoke("InitTintedPolyStuff", [])
            if self.case.get("repeat") and self.status == "returned":
                self.invoke("InitTintedPolyStuff", [])
            value = None
        elif mode == "model":
            value = self.invoke("BuildTintedPolyModel", self.case["args"])
        elif mode == "color":
            slot = self.case.get("slot", 0)
            material = self.allocate("material", 0x9c)
            self.write(self.pool + slot * STRIDE + 4, material)
            for i, color in enumerate(self.case["before"]):
                self.write(self.pool + slot * STRIDE + 12 + i * 4, color)
            self.invoke("SetTintedPolyColour", [slot, *self.case["after"]])
            value = None
        else:
            value = self.invoke("CreateTintedPoly", self.case["args"])
            for values in self.case.get("sequence", []):
                if self.status != "returned":
                    break
                value = self.invoke("CreateTintedPoly", values)
        allocations = []
        for kind, pointer, size in self.allocations:
            if kind == "material":
                data = self.material_state(pointer)
            else:
                data = bytearray(self.u.mem_read(pointer, size))
                identifier = self.read(pointer + 0x14) if kind == "actor" else 0
                if kind == "actor" and 0x400000 <= identifier < 0x900000 and self.string(identifier) == "tinted_poly_camera":
                    struct.pack_into("<I", data, 0x14, 0x2000100)
                data = data.hex()
            allocations.append([kind, pointer, size, data])
        return {"status": self.status, "value": value if self.status == "returned" else None,
                "pool": bytes(self.u.mem_read(self.pool, POOL_SIZE)).hex(), "allocations": allocations,
                "calls": self.calls, "camera": self.read(self.addr("gTintedPolyCamera")),
                "tokens": [bytes(self.u.mem_read(self.addr(name), 16)).hex() for name in TOKENS]}


def main():
    configure()
    cases = []
    for new in (False, True):
        for screen in ((640, 480), (0, 0), (65535, 32768)):
            cases.append(dict(mode="init", new_camera=new, screen=screen, repeat=True))
    for dims in ((0, 0, 640, 480, 1, 1), (-7, 13, 641, 479, 4, 4), (3, -5, -123, 79, 3, 2),
                 (0, 0, 0, 480, 1, 1), (0, 0, 640, 0, 1, 1), (0, 0, 640, 480, 0, 1),
                 (0, 0, 640, 480, 1, -1), (0, 0, 640, 480, 1, 0)):
        cases.append(dict(mode="model", args=dims))
    cases.append(dict(mode="model", args=(0, 0, 640, 480, 4, 4), fail="model"))
    for before, after in (((0, 0, 0), (0, 0, 0)), ((10, 20, 30), (1, 20, 30)),
                          ((10, 20, 30), (10, 2, 30)), ((10, 20, 30), (10, 20, 3)),
                          ((1, 2, 3), (-1, 256, 0x12345678))):
        for slot in (0, 9):
            cases.append(dict(mode="color", before=before, after=after, slot=slot))
    for cls, arg1 in ((0, 0x123456), (1, -1), (2, 0), (3, 2), (4, 27), (5, 17), (6, 0), (6, 1), (6, 2)):
        for opacity in (0, 128):
            cases.append(dict(mode="create", args=(3, 7, 641, 479, cls, arg1, 0x123456, 81), opacity=opacity))
    for slot in (9, -1):
        cases.append(dict(mode="create", args=(0, 0, 640, 480, 2, 0, 0, 0), slot=slot))
    for fail in ("material", "model", "actor"):
        for cls in (2, 3):
            cases.append(dict(mode="create", args=(0, 0, 640, 480, cls, 2, 0, 0), fail=fail))
    cases += [dict(mode="create", args=(0, 0, 32, 64, 4, n, 0, 0), flat_map=flat)
              for n, flat in ((0, False), (-1, False), (128, True))]
    for classes in ((2, 5, 0), (0, 2, 5), (5, 0, 3)):
        sequence = [(0, 0, 32, 64, cls, 17, 64, 0) for cls in classes]
        cases.append(dict(mode="create", args=sequence[0], sequence=sequence[1:]))
    results = []
    args.output.parent.mkdir(parents=True, exist_ok=True)
    for index, case in enumerate(cases):
        expected = Fixture(original, "orig", case).run()
        actual = Fixture(recompiled, "recomp", case).run()
        if expected != actual:
            failure = args.output.with_name("failure.json")
            failure.write_text(json.dumps({"case": case, "original": expected, "recompiled": actual}, indent=2))
            raise AssertionError(f"Tinted startup case {index} differs; see {failure}")
        results.append({"case": case, "status": actual["status"], "match": True})
    args.output.write_text(json.dumps(results, indent=2) + "\n")
    print(f"PASS: {len(results)} original/recompiled tinted startup, mesh, color and failure scenarios")


if __name__ == "__main__":
    main()
