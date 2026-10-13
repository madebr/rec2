#!/usr/bin/env python3
"""Differential skid/oil startup and visibility tests against original x86.

Compiles extracted production bodies with actual 32-bit repository types.
Controlled BRender/loading fixtures compare call order/arguments and snapshots
at material/model registration, complete allocated objects and untouched bytes,
name aliasing/mutation, failure paths, visibility and oil reset fields. The
external loaders/renderer and their allocation implementations are not tested.
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
SKID = ROOT / "src/carma2/newcommon/20-skidmark.c"
OIL = ROOT / "src/carma2/newcommon/39-oil.c"


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
    parser.add_argument("--output", type=Path, default=ROOT / "build-audit/skid-oil-tests")
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
#include "20-skidmark.h"
#include "39-oil.h"
#include "41-utility.h"
#include "52-errors.h"
#include "63-loading3.h"
#include "globvars.h"
#include "rec2_macros.h"
typedef char check_types[sizeof(void*)==4&&sizeof(tSkid)==28&&sizeof(tOil_spill_info)==84&&sizeof(br_actor)==100&&sizeof(br_vertex)==40&&sizeof(br_face)==40?1:-1];
tProgram_state gProgram_state;
br_actor* gOther_selfs[4];
br_material* gMaterial[2];
static unsigned arena[65536],used;
static int mode,find_mask,fail_phase,fail_index,find_count,last_index,seed;
static br_material* existing[2];
static br_pixelmap* pixies[2];
static br_pixelmap* shade;
static jmp_buf escape;
typedef struct {int code;unsigned a,b;char text[128];unsigned char bytes[256];unsigned length;} event;
static event events[1200];static unsigned event_count;
static unsigned norm(unsigned value) {
    unsigned base=(unsigned)(uintptr_t)arena;
    return value>=base&&value<base+sizeof(arena)?value-base+0x1000000:value;
}
static void* allocate(unsigned size) {
    void* pointer;used=(used+3)&~3u;if(used+size>sizeof(arena))abort();
    pointer=(char*)arena+used;memset(pointer,seed,size);used+=size;return pointer;
}
static void record(int code,unsigned a,unsigned b,const char* text,const void* bytes,unsigned length) {
    event* e=&events[event_count++];if(event_count>1200||length>sizeof(e->bytes))abort();
    e->code=code;e->a=norm(a);e->b=norm(b);if(text)strcpy(e->text,text);
    if(bytes)memcpy(e->bytes,bytes,length);
    e->length=length;
}
br_material* C2_HOOK_CDECL BrMaterialFind(const char* name) {
    br_material* result;last_index=find_count++%2;result=find_mask&(1<<last_index)?existing[last_index]:NULL;
    record(1,(unsigned)(uintptr_t)result,0,name,NULL,0);return result;
}
br_pixelmap* C2_HOOK_FASTCALL LoadPixelmap(const char* name) {
    br_pixelmap* result=fail_phase==1&&(mode==1||last_index==fail_index)?NULL:pixies[mode==1?0:last_index];
    record(2,(unsigned)(uintptr_t)result,0,name,NULL,0);return result;
}
br_material* C2_HOOK_FASTCALL LoadMaterial(const char* name) {
    br_material* result=fail_phase==2&&last_index==fail_index?NULL:existing[last_index];
    record(3,(unsigned)(uintptr_t)result,0,name,NULL,0);return result;
}
br_pixelmap* C2_HOOK_CDECL BrMapAdd(br_pixelmap* pm) {record(4,(unsigned)(uintptr_t)pm,0,NULL,NULL,0);return pm;}
br_material* C2_HOOK_CDECL BrMaterialAllocate(const char* name) {
    br_material* m=allocate(sizeof(*m));record(5,(unsigned)(uintptr_t)m,0,name,NULL,0);return m;
}
br_material* C2_HOOK_CDECL BrMaterialAdd(br_material* m) {record(6,(unsigned)(uintptr_t)m,0,NULL,m,sizeof(*m));return m;}
void C2_HOOK_CDECL BrMaterialUpdate(br_material* m,br_uint_16 flags) {record(7,(unsigned)(uintptr_t)m,flags,NULL,m,sizeof(*m));}
br_pixelmap* C2_HOOK_CDECL BrTableFind(const char* name) {record(8,(unsigned)(uintptr_t)shade,0,name,NULL,0);return shade;}
void C2_HOOK_FASTCALL GlorifyMaterial(br_material** materials,int count,tRendererShadingType shading) {
    if(count!=1)abort();
    record(9,(unsigned)(uintptr_t)*materials,shading,NULL,*materials,sizeof(**materials));
    (*materials)->flags^=0x80000000u;
}
br_model* C2_HOOK_CDECL BrModelAllocate(const char* name,int nv,int nf) {
    br_model* m=allocate(sizeof(*m));m->vertices=allocate(nv*sizeof(br_vertex));m->faces=allocate(nf*sizeof(br_face));
    record(10,(unsigned)(uintptr_t)m,(nv<<16)|nf,name,NULL,0);return m;
}
br_model* C2_HOOK_CDECL BrModelAdd(br_model* m) {record(11,(unsigned)(uintptr_t)m,0,NULL,m,sizeof(*m));return m;}
br_actor* C2_HOOK_CDECL BrActorAllocate(br_uint_8 type,void* data) {
    br_actor* a=allocate(sizeof(*a));record(12,(unsigned)(uintptr_t)a,type,NULL,NULL,0);if(data)abort();return a;
}
br_actor* C2_HOOK_CDECL BrActorAdd(br_actor* parent,br_actor* a) {
    record(13,(unsigned)(uintptr_t)parent,(unsigned)(uintptr_t)a,NULL,a,sizeof(*a));return a;
}
void C2_HOOK_CDECL BrMatrix23Identity(br_matrix23* m) {
    record(14,(unsigned)(uintptr_t)m,0,NULL,NULL,0);memset(m,0,sizeof(*m));m->m[0][0]=1.f;m->m[1][1]=1.f;
}
void C2_NORETURN C2_HOOK_CDECL FatalError(int code,...) {
    va_list ap;va_start(ap,code);record(15,code,0,va_arg(ap,const char*),NULL,0);va_end(ap);longjmp(escape,1);
}
void C2_HOOK_CDECL BrFatal(const char* path,int line,const char* format,...) {
    va_list ap;if(strcmp(path,"C:\\Carma2\\Source\\Common\\Skidmark.c")||strcmp(format,"Couldn't find %s"))abort();
    va_start(ap,format);record(16,line,0,va_arg(ap,const char*),NULL,0);va_end(ap);longjmp(escape,1);
}
'''
    for path, marker, terminator in ((SKID, "/* The original names", "// StretchMark"), (OIL, "// GLOBAL:", "// FUNCTION:")):
        source = path.read_text()
        driver += source[source.index(marker):source.index(terminator, source.index(marker))]
    for path, names in ((SKID, ("InitSkids", "HideSkid", "HideSkids")), (OIL, ("InitOilSpills", "ResetOilSpills"))):
        driver += "\n".join(extract(path, name) for name in names)
    driver += r'''
static void dump(const void* pointer,unsigned length) {
    unsigned i,j,value;const unsigned char* bytes=pointer;
    for(i=0;i+4<=length;i+=4){memcpy(&value,bytes+i,4);value=norm(value);for(j=0;j<4;j++)printf("%02x",(value>>(j*8))&255);}
    for(;i<length;i++)printf("%02x",bytes[i]);
}
int main(int argc,char**argv) {
    unsigned i;
    if(argc==2){printf("[%u,%u,%u,%u,%u,%u,%u,%u,%u,%u]\n",(unsigned)sizeof(br_material),(unsigned)sizeof(br_pixelmap),(unsigned)sizeof(br_model),(unsigned)offsetof(br_model,vertices),(unsigned)offsetof(br_model,faces),(unsigned)offsetof(br_material,flags),(unsigned)offsetof(br_material,map_transform),(unsigned)offsetof(tProgram_state,sausage_eater_mode),(unsigned)sizeof(tProgram_state),(unsigned)offsetof(br_model,flags));return 0;}
    mode=atoi(argv[1]);gProgram_state.sausage_eater_mode=atoi(argv[2]);find_mask=atoi(argv[3]);fail_phase=atoi(argv[4]);fail_index=atoi(argv[5]);seed=atoi(argv[6]);
    for(i=0;i<2;i++)existing[i]=allocate(sizeof(br_material));
    for(i=0;i<2;i++)pixies[i]=allocate(sizeof(br_pixelmap));
    shade=allocate(sizeof(br_pixelmap));
    for(i=0;i<4;i++)gOther_selfs[i]=allocate(sizeof(br_actor));
    memset(gSkids,seed,sizeof(gSkids));memset(gOily_spills,seed,sizeof(gOily_spills));gCurrent_skid=123;gNext_oil_pixie=456;
    if(mode==2)for(i=0;i<32;i++)gOily_spills[i].actor=allocate(sizeof(br_actor));
    if(mode>=3)for(i=0;i<100;i++)gSkids[i].actor=allocate(sizeof(br_actor));
    if(!setjmp(escape))switch(mode) {
    case 0:InitSkids();break;case 1:InitOilSpills();break;case 2:ResetOilSpills();break;
    case 3:HideSkid(fail_index);break;case 4:HideSkids();break;
    }
    printf("{\"events\":[");for(i=0;i<event_count;i++){event* e=&events[i];printf("%s[%d,%u,%u,\"%s\",\"",i?",":"",e->code,e->a,e->b,e->text);dump(e->bytes,e->length);printf("\"]");}
    printf("],\"arena\":\"");dump(arena,used);printf("\",\"skids\":\"");dump(gSkids,sizeof(gSkids));
    printf("\",\"spills\":\"");dump(gOily_spills,sizeof(gOily_spills));
    printf("\",\"material_ptrs\":\"");dump(gMaterial,sizeof(gMaterial));printf("\",\"oil_ptrs\":\"");dump(gOil_pixies,sizeof(gOil_pixies));dump(&oily_material,sizeof(oily_material));
    printf("\",\"names\":[\"%s\",\"%s\",\"%s\"],\"indices\":[%d,%d]}\n",gOil_smear_material_name,gRob_smear_material_name,gGib_smear_material_name,gCurrent_skid,gNext_oil_pixie);return 0;
}
'''
    harness = out / "harness.c"
    harness.write_text(driver)
    includes = ["src/carma2/newcommon", "src/carma2/include", "src/carma2/hooks", "src/carma2/s3/include", "src/brender/include"]
    command = [args.cc, "-std=gnu89", "-O2", "-Wall", "-Wextra", "-Werror", *["-I" + str(ROOT / path) for path in includes], str(harness), "-o", str(out / "harness.exe")]
    subprocess.run(command, check=True)
    schema = json.loads(subprocess.check_output([str(out / "harness.exe"), "schema"], text=True))
    material_size, pixmap_size, model_size, vertices_offset, faces_offset, flags_offset, transform_offset, sausage_offset, _, _ = schema
    pe = struct.unpack_from("<I", binary, 0x3c)[0]
    section_count = struct.unpack_from("<H", binary, pe + 6)[0]
    section_table = pe + 24 + struct.unpack_from("<H", binary, pe + 20)[0]

    def original(mode, sausage, find_mask, fail_phase, fail_index, seed):
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
        def text(address):
            data = bytearray()
            while address and u.mem_read(address + len(data), 1)[0]:
                data += u.mem_read(address + len(data), 1)
            return data.decode("ascii")
        used = 0
        def allocate(size):
            nonlocal used
            used = (used + 3) & ~3
            pointer = 0x1000000 + used
            assert used + size <= 0x40000
            u.mem_write(pointer, bytes([seed])*size)
            used += size
            return pointer
        existing = [allocate(material_size) for _ in range(2)]
        pixies = [allocate(pixmap_size) for _ in range(2)]
        shade = allocate(pixmap_size)
        parents = [allocate(100) for _ in range(4)]
        for i, parent in enumerate(parents):
            wr(0x74d644 + i*4, parent)
        u.mem_write(0x6a27f0, bytes([seed])*2800)
        u.mem_write(0x690c90, bytes([seed])*2688)
        wr(0x6a27e8, 123)
        wr(0x691710, 456)
        # Proved original sausage field address; verify current layout separately.
        assert sausage_offset == 0x10
        wr(0x75bb90, sausage)
        if mode == 2:
            for i in range(32):
                wr(0x690c90 + i*84, allocate(100))
        if mode >= 3:
            for i in range(100):
                wr(0x6a27f0 + i*28, allocate(100))
        events, find_count, last_index, stopped = [], 0, 0, False
        def record(code, a=0, b=0, name="", pointer=0, length=0):
            events.append([code, a, b, name, bytes(u.mem_read(pointer, length)) if length else b""])
        def hook(uc, address, size, user):
            nonlocal find_count, last_index, stopped
            external = {0x51eea0,0x48ec00,0x48ef80,0x51efa0,0x51f240,0x51ee50,0x520e70,0x51f140,0x5182f0,0x5217f0,0x51ed00,0x51e1a0,0x51dfe0,0x534ba0,0x44c230,0x527d20,0x575d00}
            if address not in external:
                return
            sp = uc.reg_read(UC_X86_REG_ESP)
            ecx, edx = uc.reg_read(UC_X86_REG_ECX), uc.reg_read(UC_X86_REG_EDX)
            value, extra = 0, 0
            if address == 0x51eea0:
                last_index = find_count % 2
                find_count += 1
                value = existing[last_index] if find_mask & (1<<last_index) else 0
                record(1, value, name=text(rd(sp+4)))
            elif address == 0x48ec00:
                value = 0 if fail_phase == 1 and (mode == 1 or last_index == fail_index) else pixies[0 if mode == 1 else last_index]
                record(2, value, name=text(ecx))
            elif address == 0x48ef80:
                value = 0 if fail_phase == 2 and last_index == fail_index else existing[last_index]
                record(3, value, name=text(ecx))
            elif address == 0x51efa0:
                value = rd(sp+4)
                record(4, value)
            elif address == 0x51f240:
                value = allocate(material_size)
                record(5, value, name=text(rd(sp+4)))
            elif address in (0x51ee50, 0x520e70):
                value = rd(sp+4)
                record(6 if address == 0x51ee50 else 7, value, rd(sp+8) if address == 0x520e70 else 0, pointer=value, length=material_size)
            elif address == 0x51f140:
                value = shade
                record(8, value, name=text(rd(sp+4)))
            elif address == 0x5182f0:
                assert edx == 1
                value = rd(ecx)
                record(9, value, rd(sp+4), pointer=value, length=material_size)
                wr(value + flags_offset, rd(value + flags_offset)^0x80000000)
                extra = 4
            elif address == 0x5217f0:
                nv, nf = rd(sp+8), rd(sp+12)
                value = allocate(model_size)
                wr(value+vertices_offset, allocate(nv*40))
                wr(value+faces_offset, allocate(nf*40))
                record(10, value, (nv<<16)|nf, name=text(rd(sp+4)))
            elif address == 0x51ed00:
                value = rd(sp+4)
                record(11, value, pointer=value, length=model_size)
            elif address == 0x51e1a0:
                assert rd(sp+8) == 0
                value = allocate(100)
                record(12, value, rd(sp+4))
            elif address == 0x51dfe0:
                value = rd(sp+8)
                record(13, rd(sp+4), value, pointer=value, length=100)
            elif address == 0x534ba0:
                pointer = rd(sp+4)
                record(14, pointer)
                uc.mem_write(pointer, struct.pack("<ffffff", 1,0,0,1,0,0))
            elif address == 0x44c230:
                record(15, rd(sp+4), name=text(rd(sp+8)))
                stopped = True
                uc.emu_stop()
                return
            elif address == 0x527d20:
                assert text(rd(sp+4)) == "C:\\Carma2\\Source\\Common\\Skidmark.c" and text(rd(sp+12)) == "Couldn't find %s"
                record(16, rd(sp+8), name=text(rd(sp+16)))
                stopped = True
                uc.emu_stop()
                return
            elif address == 0x575d00:
                value = rd(sp+4)
                assert text(rd(sp+8)) == "."
                data = text(value)
                token = data.split(".")[0]
                uc.mem_write(value+len(token), b"\0")
            uc.reg_write(UC_X86_REG_EAX, value)
            uc.reg_write(UC_X86_REG_EIP, rd(sp))
            uc.reg_write(UC_X86_REG_ESP, sp+4+extra)
        u.hook_add(UC_HOOK_CODE, hook)
        stack, stop = 0x201e000, 0x201f000
        wr(stack, stop)
        u.reg_write(UC_X86_REG_ESP, stack)
        u.reg_write(UC_X86_REG_ECX, fail_index)
        u.emu_start([0x4e9c40,0x4a6a10,0x4a6c50,0x4e9ee0,0x4e9f00][mode], stop, count=500000)
        assert stopped or u.reg_read(UC_X86_REG_EIP) == stop
        def normalize(data):
            data = bytearray(data)
            # Original arena has the canonical base, so only current pointers
            # need transformation. Byte-preservation comparisons remain exact.
            return data.hex()
        return {"events": [[*event[:4], normalize(event[4])] for event in events],
                "arena": normalize(u.mem_read(0x1000000, used)), "skids": normalize(u.mem_read(0x6a27f0,2800)),
                "spills": normalize(u.mem_read(0x690c90,2688)), "material_ptrs": normalize(u.mem_read(0x74cee8,8)),
                "oil_ptrs": normalize(u.mem_read(0x690c88,4))+normalize(u.mem_read(0x74a68c,4)),
                "names": [text(address) for address in (0x65fe38,0x65fe28,0x65fe18)], "indices": [rd(0x6a27e8),rd(0x691710)]}

    cases = [(0, sausage, mask, 0, 0, seed) for sausage in (0,1) for mask in (0,1,2,3) for seed in (0x35,0xa5)]
    cases += [(0, sausage, 0, failure, index, 0xa5) for sausage in (0,1) for failure in (1,2) for index in (0,1)]
    cases += [(1,0,0,failure,0,seed) for failure in (0,1) for seed in (0x35,0xa5)]
    cases += [(2,0,0,0,0,seed) for seed in (0x35,0xa5,0xff)]
    cases += [(3,0,0,0,index,0xa5) for index in (0,33,99)] + [(4,0,0,0,0,seed) for seed in (0x35,0xa5)]
    results = []
    for case in cases:
        native = json.loads(subprocess.check_output([str(out / "harness.exe"), *map(str,case)], text=True))
        expected = original(*case)
        if native != expected:
            (out / "failure.json").write_text(json.dumps({"case": case, "current": native, "original": expected}, indent=2))
            raise AssertionError(f"Differential mismatch {case}; see failure.json")
        results.append({"case": case, "event_count": len(native["events"]), "all_state_bytes_match": True})
    (out / "results.json").write_text(json.dumps(results, indent=2)+"\n")
    manifest = {"original_sha256": ORIGINAL_HASH, "all_cases_passed": True, "case_count": len(results), "type_schema": schema,
                "compiler_command": command, "compiler_version": subprocess.check_output([args.cc,"--version"],text=True).splitlines()[0],
                "source_sha256": {str(path.relative_to(ROOT)):hashlib.sha256(path.read_bytes()).hexdigest() for path in (SKID,OIL,ROOT/"src/carma2/include/rec2_types.h")},
                "harness_sha256": hashlib.sha256(driver.encode()).hexdigest(),
                "scope": "Current bodies and repository types vs original: full arena/global bytes, loading branches and name mutation, ordered dependency calls with material/model/actor snapshots, reset and visibility."}
    (out / "verification.json").write_text(json.dumps(manifest, indent=2)+"\n")
    print(f"Passed {len(results)} original-vs-current skid/oil cases")


if __name__ == "__main__":
    main()
