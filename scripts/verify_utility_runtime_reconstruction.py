#!/usr/bin/env python3
"""Compare utility entries with original x86 and three native compilers.

Time, keyboard, sound, failure, table lookup, stdio and rectangle-copy calls are controlled boundaries.
Actor traversal executes actual BRender multiply/identity bodies using exact
dyadic matrices; it tests traversal and matrix order, not general matrix accuracy.
Complete arena bytes, callback mutations and ordered arguments are compared.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import random
import struct
import subprocess
import sys

from verify_tinted_native_reconstruction import extract

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "src/carma2/newcommon/41-utility.c"
HASH = "9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d"
FUNCTIONS = dict(WaitFor=0x5146c0, Colour24BitTo16Bit=0x517050,
    DRPixelmapRectangleCopy=0x515690, DrPixelmapRectangleCopyPossibleLock=0x5191b0,
    DRActorEnumRecurseWithSnart=0x5148c0,
    FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport=0x518d60,
    GetBlendificatiousnessOfMaterial=0x518e70, sign=0x514930,
    DumpVisibleActorsCB=0x518b00, DumpVisibleActors=0x518ac0, DRImageLoad=0x513870)
ADDRESSES = dict(FUNCTIONS, PDGetTotalTime=0x51d410, AnyKeyDown=0x482d70,
    SoundService=0x455a80, BrFailure=0x527c80, BrPixelmapRectangleCopy=0x538590,
    BrMatrix34Mul=0x532620, BrMatrix34Identity=0x5327c0, gBack_screen=0x74d360,
    gScreen=0x74d3e0, BrTableFind=0x51f140, BrActorEnum=0x51ded0,
    printf=0x577930, fflush=0x577700, DRLdImg=0x48eb80,
    gPixelFlags=0x74ca18, gRender_palette=0x74a674, gDir_separator=0x761a5c)
HEAP, STACK, STOP, CALLBACK, SIZE = 0x1000000, 0x1080000, 0x1081000, 0x1082000, 4096


def words(values):
    return struct.pack("<" + "I" * len(values), *[v & 0xffffffff for v in values])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--original", type=Path, default=ROOT / "build-reference/originals/CARMA2_HW.EXE")
    parser.add_argument("--recompiled", type=Path, default=ROOT / "build-matching/rec2.exe")
    parser.add_argument("--symbols", type=Path)
    parser.add_argument("--dependency-dir", type=Path, default=ROOT / "build-audit/joystick/python-deps")
    parser.add_argument("--gcc", type=Path, default=ROOT / "build-tools/winlibs-i686/mingw32/bin/gcc.exe")
    parser.add_argument("--msvc-root", type=Path, default=ROOT / "build-tools/msvc-modern")
    parser.add_argument("--msvc5-root", type=Path, default=ROOT / "build-tools/msvc500sp3")
    parser.add_argument("--output", type=Path, default=ROOT / "build-audit/utility-runtime-tests")
    parser.add_argument("--modes", type=int, nargs="+", choices=range(len(FUNCTIONS)))
    args = parser.parse_args()
    sys.path.insert(0, str(args.dependency_dir.resolve()))
    from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
    from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EIP, UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX, UC_X86_REG_FPCW
    original = args.original.read_bytes()
    assert hashlib.sha256(original).hexdigest() == HASH
    images = [(original, ADDRESSES)]
    if args.symbols:
        mapping = json.loads(args.symbols.read_text())
        rebuilt = args.recompiled.read_bytes()
        assert mapping["format"] == 1 and mapping["original_sha256"] == HASH
        assert mapping["recompiled_sha256"] == hashlib.sha256(rebuilt).hexdigest()
        by_address = {s["orig"]: s["recomp"] for s in mapping["symbols"]}
        images.append((rebuilt, {n: by_address[a] for n, a in ADDRESSES.items()}))
    out = args.output.resolve()
    out.mkdir(parents=True, exist_ok=True)
    source = SOURCE.read_text()
    matrix = (ROOT / "src/brender/core/math/matrix34.c").read_text()
    matrix_bodies = "\n".join(extract(matrix, n).replace(n, "Native" + n)
        for n in ("BrMatrix34Mul", "BrMatrix34Identity"))
    driver = r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <stdarg.h>
#include <float.h>
#include <fcntl.h>
#include <io.h>
#include "41-utility.h"
#include "42-input.h"
#include "63-loading3.h"
#include "69-sound.h"
#include "globvars.h"
#include "platform.h"
#include "rec2_macros.h"
typedef char layouts[sizeof(void*)==4 && sizeof(br_actor)==100 &&
    offsetof(br_actor,t)==40 && offsetof(br_pixelmap,type)==44 &&
    offsetof(br_material,index_blend)==76 && offsetof(br_material,extra_prim)==88 ? 1:-1];
static unsigned h[16], ticks, keys, sounds, tables;
static unsigned char arena[4096];
static jmp_buf failure;
br_pixelmap* gBack_screen;
br_pixelmap* gScreen;
br_pixelmap* gRender_palette;
tPixelFlags gPixelFlags;
const char* gDir_separator;
static void emit(unsigned v){fwrite(&v,4,1,stdout);}
static unsigned norm(const void* p){
    unsigned v=(unsigned)(uintptr_t)p,b=(unsigned)(uintptr_t)arena;
    return v>=b && v<b+4096 ? v-b+0x1000000 : v;
}
static void snapshot(void){
    unsigned char copy[4096];unsigned i,j,v;
    memcpy(copy,arena,sizeof(copy));
    for(i=0;i<6;i++)for(j=0;j<7;j++){
        static const unsigned fields[7]={0,8,12,20,24,28,36};
        unsigned off=0x400+128*i+fields[j];
        memcpy(&v,copy+off,4);v=norm((void*)(uintptr_t)v);memcpy(copy+off,&v,4);
    }
    for(i=0;i<2;i++){
        unsigned off=i?0xb58:0xb4c;
        memcpy(&v,copy+off,4);v=norm((void*)(uintptr_t)v);memcpy(copy+off,&v,4);
    }
    for(i=0;i<2;i++){
        unsigned off=i?0xa04:0x804;
        memcpy(&v,copy+off,4);v=norm((void*)(uintptr_t)v);memcpy(copy+off,&v,4);
    }
    fwrite(copy,sizeof(copy),1,stdout);
}
br_pixelmap* C2_HOOK_CDECL BrTableFind(const char* name){
    unsigned index=tables++,value=h[1+index];
    br_material* material=(br_material*)(arena+0xb00);
    if(index>=3)exit(91);
    emit(10);emit(index);emit((unsigned)strlen(name));fwrite(name,strlen(name),1,stdout);snapshot();
    if(h[4]==index+1)material->index_blend=h[5]?(br_pixelmap*)(arena+h[5]):NULL;
    if(h[10])gScreen->type=BR_PMT_RGB_555;
    return value?(br_pixelmap*)(arena+value):NULL;
}
br_pixelmap* C2_HOOK_FASTCALL DRLdImg(const char*dir,const char*name,br_pixelmap*palette,int flags,int*error){
    emit(14);emit((unsigned)strlen(dir));fwrite(dir,strlen(dir),1,stdout);
    emit((unsigned)strlen(name));fwrite(name,strlen(name),1,stdout);emit(norm(palette));emit(flags);
    *error=(int)h[2];
    if(h[3]){memset(arena+0xe00,'?',32);gPixelFlags=(tPixelFlags)0xaabbccdd;gRender_palette=NULL;}
    return h[4]?(br_pixelmap*)(arena+h[4]):NULL;
}
tU32 C2_HOOK_FASTCALL PDGetTotalTime(void){
    unsigned result;
    if(ticks>=32)exit(90);
    memcpy(&result,arena+0x200+4*(ticks<h[2]?ticks:h[2]-1),4);
    emit(1);emit(ticks++);emit(result);return result;
}
int C2_HOOK_FASTCALL AnyKeyDown(void){
    int result=keys>=h[3]?(int)h[4]:0;
    emit(2);emit(keys++);emit((unsigned)result);return result;
}
void C2_HOOK_FASTCALL SoundService(void){emit(3);emit(sounds++);}
C2_NORETURN void C2_HOOK_CDECL BrFailure(const char* text,...){
    emit(5);emit((unsigned)strlen(text));fwrite(text,strlen(text),1,stdout);
    longjmp(failure,1);
}
void C2_HOOK_CDECL BrPixelmapRectangleCopy(br_pixelmap* dst,br_int_32 dx,br_int_32 dy,
    br_pixelmap* src,br_int_32 sx,br_int_32 sy,br_int_32 w,br_int_32 ht){
    emit(4);emit(norm(dst));emit(dx);emit(dy);emit(norm(src));
    emit(sx);emit(sy);emit(w);emit(ht);
}
#define A(x,y) A->m[x][y]
#define B(x,y) B->m[x][y]
#define C(x,y) C->m[x][y]
#define M(x,y) mat->m[x][y]
#undef BR_MAC3
#define BR_MAC3(a,b,c,d,e,f) ((a)*(b)+(c)*(d)+(e)*(f))
'''
    driver += matrix_bodies + r'''
void C2_HOOK_CDECL BrMatrix34Mul(br_matrix34*A,const br_matrix34*B,const br_matrix34*C){
    emit(6);fwrite(B,48,1,stdout);fwrite(C,48,1,stdout);NativeBrMatrix34Mul(A,B,C);
}
void C2_HOOK_CDECL BrMatrix34Identity(br_matrix34*mat){emit(7);NativeBrMatrix34Identity(mat);}
static intptr_t C2_HOOK_FASTCALL callback(br_actor*actor,br_matrix34*mat,void*arg){
    unsigned i=((unsigned char*)actor-arena-0x400)/128;
    emit(8);emit(i);emit(norm(arg));fwrite(mat,48,1,stdout);snapshot();
    if((h[7]&1)&&i==0)actor->children=(br_actor*)(arena+0x580);
    if((h[7]&2)&&i==1)actor->next=(br_actor*)(arena+0x680);
    if(h[7]&4)mat->m[3][0]+=8.f;
    if((h[7]&8)&&i==1)((br_actor*)(arena+0x500))->t.t.mat.m[0][1]=4.f;
    *(unsigned*)arg+=i+1;
    return i==h[8]?(intptr_t)h[9]:0;
}
'''
    driver += extract((ROOT / "src/brender/core/v1db/actsupt.c").read_text(), "BrActorEnum").replace("BrActorEnum", "NativeBrActorEnum")
    driver += r'''
br_uint_32 C2_HOOK_CDECL BrActorEnum(br_actor*actor,br_actor_enum_cbfn*cb,void*arg){
    emit(12);emit(norm(actor));emit(cb==DumpVisibleActorsCB);emit(norm(arg));snapshot();
    return NativeBrActorEnum(actor,cb,arg);
}
static int C2_HOOK_CDECL FixturePrintf(const char*fmt,...){
    va_list args;unsigned value;const char*text;
    emit(11);emit((unsigned)strlen(fmt));fwrite(fmt,strlen(fmt),1,stdout);
    va_start(args,fmt);
    if(strstr(fmt,"%s")){
        text=va_arg(args,const char*);emit((unsigned)strlen(text));fwrite(text,strlen(text),1,stdout);
    }else if(strstr(fmt,"%08x")){
        value=va_arg(args,unsigned);emit(norm((void*)(uintptr_t)value));
    }else if(strstr(fmt,"%i")){
        value=(unsigned)va_arg(args,int);emit(value);
    }else if(strchr(fmt,'%')){exit(92);}
    va_end(args);return 0;
}
static int C2_HOOK_CDECL FixtureFlush(FILE*f){if(f!=stdout)exit(93);emit(13);return 0;}
#define printf FixturePrintf
#define fflush FixtureFlush
'''
    driver += "\n".join(extract(source, n) for n in FUNCTIONS)
    driver += r'''
int C2_HOOK_CDECL main(int argc,char**argv){
    FILE*f;unsigned i,j,v;volatile unsigned result=0;
    if(argc!=2)return 1;
    f=fopen(argv[1],"rb");if(f==NULL)return 2;
    if(fread(h,sizeof(h),1,f)!=1||fread(arena,sizeof(arena),1,f)!=1)return 3;
    fclose(f);_setmode(_fileno(stdout),_O_BINARY);_controlfp(_PC_53,_MCW_PC);
    for(i=0;i<6;i++)for(j=0;j<7;j++){
        static const unsigned fields[7]={0,8,12,20,24,28,36};
        unsigned off=0x400+128*i+fields[j];
        memcpy(&v,arena+off,4);
        if(v>=0x1000000&&v<0x1001000){v=v-0x1000000+(unsigned)(uintptr_t)arena;memcpy(arena+off,&v,4);}
    }
    for(i=0;i<2;i++){
        unsigned off=i?0xb58:0xb4c;
        memcpy(&v,arena+off,4);
        if(v>=0x1000000&&v<0x1001000){v=v-0x1000000+(unsigned)(uintptr_t)arena;memcpy(arena+off,&v,4);}
    }
    for(i=0;i<2;i++){
        unsigned off=i?0xa04:0x804;
        memcpy(&v,arena+off,4);
        if(v>=0x1000000&&v<0x1001000){v=v-0x1000000+(unsigned)(uintptr_t)arena;memcpy(arena+off,&v,4);}
    }
    gBack_screen=(br_pixelmap*)(arena+0x80);
    gScreen=gBack_screen;
    gRender_palette=h[5]?(br_pixelmap*)(arena+h[5]):NULL;
    gDir_separator=(char*)(arena+0xd00);gPixelFlags=(tPixelFlags)h[1];
    if(!setjmp(failure)){
        switch(h[0]){
        case 0:WaitFor(h[1]);break;
        case 1:result=Colour24BitTo16Bit(h[1]);break;
        case 2:DRPixelmapRectangleCopy(gBack_screen,(br_int_16)h[1],(br_int_16)h[2],(br_pixelmap*)(arena+0x180),(br_int_16)h[3],(br_int_16)h[4],(br_uint_16)h[5],(br_uint_16)h[6]);break;
        case 3:DrPixelmapRectangleCopyPossibleLock(gBack_screen,(br_int_16)h[1],(br_int_16)h[2],(br_pixelmap*)(arena+0x180),(br_int_16)h[3],(br_int_16)h[4],(br_int_16)h[5],(br_int_16)h[6]);break;
        case 4:result=(unsigned)DRActorEnumRecurseWithSnart((br_actor*)(arena+0x400),h[6]?(br_matrix34*)(arena+0x300):NULL,callback,arena+0xf00);break;
        case 5:result=FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(gScreen,h[1],h[2],h[3],h[4]);break;
        case 6:result=(unsigned)GetBlendificatiousnessOfMaterial((br_material*)(arena+0xb00));break;
        case 7:result=(unsigned)sign((int)h[1]);break;
        case 8:result=DumpVisibleActorsCB((br_actor*)(arena+0x400),arena+0xf00);break;
        case 9:DumpVisibleActors((br_actor*)(arena+0x400),(char*)(arena+0xe80));break;
        case 10:result=norm(DRImageLoad((char*)(arena+0xe00)));break;
        }
    }
    if(h[0]==10){emit(15);emit(gPixelFlags);emit(norm(gRender_palette));}
    emit(9);emit(result);snapshot();return 0;
}
'''
    harness = out / "harness.c"
    harness.write_text(driver)
    includes = [ROOT / p for p in ["src/carma2/newcommon", "src/carma2/include", "src/carma2/hooks", "src/carma2/platform", "src/carma2/s3/include", "src/brender/include"]]
    msvc = args.msvc_root.resolve() / "VC/Tools/MSVC/14.44.35207"
    sdk = args.msvc_root.resolve() / "Windows Kits/10"
    sdk_version = "10.0.26100.0"
    env = dict(os.environ)
    env["PATH"] = str(msvc / "bin/Hostx64/x86") + os.pathsep + env["PATH"]
    env["INCLUDE"] = os.pathsep.join(map(str, [msvc / "include", *[sdk / f"Include/{sdk_version}/{p}" for p in ["ucrt", "shared", "um"]]]))
    env["LIB"] = os.pathsep.join(map(str, [msvc / "lib/x86", *[sdk / f"Lib/{sdk_version}/{p}/x86" for p in ["ucrt", "um"]]]))
    old = args.msvc5_root.resolve()
    old_env = dict(os.environ)
    old_env["PATH"] = str(old / "bin") + os.pathsep + str(old / "redist") + os.pathsep + old_env["PATH"]
    old_env["INCLUDE"], old_env["LIB"] = str(old / "include"), str(old / "lib")
    commands = {
        "msvc": [str(msvc / "bin/Hostx64/x86/cl.exe"), "/nologo", "/O2", "/W3", "/WX", "/D_CRT_SECURE_NO_WARNINGS", *["/I" + str(p) for p in includes], str(harness), "/Fo" + str(out / "msvc.obj"), "/Fe" + str(out / "msvc.exe")],
        "msvc5": [str(old / "bin/cl.exe"), "/nologo", "/O2", "/Ob2", "/G5", "/Gr", "/ML", "/W3", "/WX", "/DREC2_MATCHING", "/I" + str(ROOT / "src/msvc500/include"), *["/I" + str(p) for p in includes], str(harness), "/Fo" + str(out / "msvc5.obj"), "/Fe" + str(out / "msvc5.exe")],
        "mingw": [str(args.gcc.resolve()), "-std=gnu89", "-O2", "-Wall", "-Wextra", "-Werror", "-fno-strict-aliasing", *["-I" + str(p) for p in includes], str(harness), "-o", str(out / "mingw.exe")],
    }
    for compiler, command in commands.items():
        result = subprocess.run(command, cwd=out, env=env if compiler == "msvc" else old_env if compiler == "msvc5" else None, capture_output=True, text=True)
        (out / f"{compiler}-build.log").write_text(result.stdout + result.stderr)
        assert result.returncode == 0, result.stdout + result.stderr

    def execute(binary, addresses, h, arena):
        u = Uc(UC_ARCH_X86, UC_MODE_32)
        u.mem_map(0x400000, 0x500000)
        u.mem_map(HEAP, 0x100000)
        pe = struct.unpack_from("<I", binary, 0x3c)[0]
        table = pe + 24 + struct.unpack_from("<H", binary, pe + 20)[0]
        for i in range(struct.unpack_from("<H", binary, pe + 6)[0]):
            _, rva, size, offset = struct.unpack_from("<IIII", binary, table + 40 * i + 8)
            if size:
                u.mem_write(0x400000 + rva, binary[offset:offset + size])
        u.mem_write(HEAP, bytes(arena))
        u.reg_write(UC_X86_REG_FPCW, 0x27f)
        def rd(p):
            return struct.unpack("<I", u.mem_read(p, 4))[0]
        def wr(p, value):
            u.mem_write(p, words([value]))
        def string(p):
            s = bytearray()
            while u.mem_read(p, 1) != b"\0": s.extend(u.mem_read(p, 1)); p += 1
            return s
        wr(addresses["gBack_screen"], HEAP + 0x80)
        wr(addresses["gScreen"], HEAP + 0x80)
        wr(addresses["gPixelFlags"], h[1]); wr(addresses["gRender_palette"], HEAP + h[5] if h[5] else 0)
        wr(addresses["gDir_separator"], HEAP + 0xd00)
        events = bytearray()
        counts = [0, 0, 0, 0]
        failed = [False]
        def emit(*values):
            events.extend(words(values))
        def ret(value=0, pop=0):
            sp = u.reg_read(UC_X86_REG_ESP)
            u.reg_write(UC_X86_REG_EAX, value & 0xffffffff)
            u.reg_write(UC_X86_REG_EIP, rd(sp))
            u.reg_write(UC_X86_REG_ESP, sp + 4 + pop)
        def hook(uc, ip, size, context):
            sp = uc.reg_read(UC_X86_REG_ESP)
            if ip == addresses["PDGetTotalTime"]:
                i = counts[0]
                assert i < 32
                v = rd(HEAP + 0x200 + 4 * min(i, h[2] - 1))
                emit(1, i, v); counts[0] += 1; ret(v)
            elif ip == addresses["AnyKeyDown"]:
                i = counts[1]; v = h[4] if i >= h[3] else 0
                emit(2, i, v); counts[1] += 1; ret(v)
            elif ip == addresses["SoundService"]:
                emit(3, counts[2]); counts[2] += 1; ret()
            elif ip == addresses["BrFailure"]:
                p = rd(sp + 4); s = bytearray()
                while u.mem_read(p, 1) != b"\0":
                    s.extend(u.mem_read(p, 1)); p += 1
                emit(5, len(s)); events.extend(s); failed[0] = True; u.emu_stop()
            elif ip == addresses["BrPixelmapRectangleCopy"]:
                emit(4, *[rd(sp + 4 + 4 * i) for i in range(8)]); ret()
            elif ip == addresses["BrTableFind"]:
                i = counts[3]; counts[3] += 1
                assert i < 3
                p = rd(sp + 4); s = bytearray()
                while u.mem_read(p, 1) != b"\0":
                    s.extend(u.mem_read(p, 1)); p += 1
                emit(10, i, len(s)); events.extend(s); events.extend(u.mem_read(HEAP, SIZE))
                if h[4] == i + 1: wr(HEAP + 0xb4c, HEAP + h[5] if h[5] else 0)
                if h[10]: u.mem_write(HEAP + 0xac, b"\x04")
                ret(HEAP + h[1 + i] if h[1 + i] else 0)
            elif ip == addresses["BrMatrix34Mul"]:
                emit(6); events.extend(u.mem_read(rd(sp + 8), 48)); events.extend(u.mem_read(rd(sp + 12), 48))
            elif ip == addresses["BrMatrix34Identity"]:
                emit(7)
            elif ip == addresses["printf"]:
                fmt = string(rd(sp + 4)); emit(11, len(fmt)); events.extend(fmt)
                if b"%s" in fmt:
                    s = string(rd(sp + 8)); emit(len(s)); events.extend(s)
                elif b"%08x" in fmt or b"%i" in fmt: emit(rd(sp + 8))
                else: assert b"%" not in fmt
                ret()
            elif ip == addresses["fflush"]:
                if addresses["fflush"] == ADDRESSES["fflush"]: assert rd(sp + 4) == 0x6737f0
                emit(13); ret()
            elif ip == addresses["BrActorEnum"]:
                emit(12, rd(sp + 4), int(rd(sp + 8) == addresses["DumpVisibleActorsCB"]), rd(sp + 12))
                events.extend(u.mem_read(HEAP, SIZE))
            elif ip == addresses["DRLdImg"]:
                directory = string(uc.reg_read(UC_X86_REG_ECX)); name = string(uc.reg_read(UC_X86_REG_EDX))
                emit(14, len(directory)); events.extend(directory); emit(len(name)); events.extend(name)
                emit(rd(sp+4), rd(sp+8)); wr(rd(sp+12), h[2])
                if h[3]:
                    u.mem_write(HEAP+0xe00, b'?'*32); wr(addresses['gPixelFlags'], 0xaabbccdd); wr(addresses['gRender_palette'], 0)
                ret(HEAP+h[4] if h[4] else 0, 12)
            elif ip == CALLBACK:
                actor, mat, arg = uc.reg_read(UC_X86_REG_ECX), uc.reg_read(UC_X86_REG_EDX), rd(sp + 4)
                i = (actor - HEAP - 0x400) // 128
                emit(8, i, arg); events.extend(u.mem_read(mat, 48)); events.extend(u.mem_read(HEAP, SIZE))
                if h[7] & 1 and i == 0: wr(actor + 8, HEAP + 0x580)
                if h[7] & 2 and i == 1: wr(actor, HEAP + 0x680)
                if h[7] & 4:
                    v = struct.unpack("<f", u.mem_read(mat + 36, 4))[0]
                    u.mem_write(mat + 36, struct.pack("<f", v + 8))
                if h[7] & 8 and i == 1: wr(HEAP + 0x500 + 44 + 4, 0x40800000)
                wr(arg, rd(arg) + i + 1); ret(h[9] if i == h[8] else 0, 4)
        u.hook_add(UC_HOOK_CODE, hook)
        mode = h[0]
        ecx, edx, stackargs = h[1], 0, []
        if mode in (2, 3): ecx, edx, stackargs = HEAP + 0x80, h[1], [h[2], HEAP + 0x180, *h[3:7]]
        elif mode == 4: ecx, edx, stackargs = HEAP + 0x400, HEAP + 0x300 if h[6] else 0, [CALLBACK, HEAP + 0xf00]
        elif mode == 5: ecx, edx, stackargs = HEAP + 0x80, h[1], h[2:5]
        elif mode == 6: ecx = HEAP + 0xb00
        elif mode == 8: ecx, edx, stackargs = 0, 0, [HEAP + 0x400, HEAP + 0xf00]
        elif mode == 9: ecx, edx = HEAP + 0x400, HEAP + 0xe80
        elif mode == 10: ecx = HEAP + 0xe00
        u.mem_write(STACK, words([STOP, *stackargs]))
        u.reg_write(UC_X86_REG_ESP, STACK); u.reg_write(UC_X86_REG_ECX, ecx); u.reg_write(UC_X86_REG_EDX, edx)
        u.emu_start(addresses[list(FUNCTIONS)[mode]], STOP, count=100000)
        assert failed[0] or u.reg_read(UC_X86_REG_EIP) == STOP
        value = u.reg_read(UC_X86_REG_EAX) if mode in (1, 4, 5, 6, 7, 8, 10) and not failed[0] else 0
        if mode == 1: value &= 65535
        if mode == 10: emit(15, rd(addresses['gPixelFlags']), rd(addresses['gRender_palette']))
        emit(9, value); events.extend(u.mem_read(HEAP, SIZE))
        return bytes(events)

    rng = random.Random(0x5148c0)
    cases = []
    for times in ([0, 0, 10, 20, 30], [0xfffffffe, 0xfffffffe, 0xffffffff, 0, 20],
                  [100, 90, 100, 120], [0x7ffffffe, 0x7ffffffe, 0x80000010]):
        for delay in (0, 1, 10, 20, 0xffffffff, 0x80000000):
            for key_after in (0, 1, 3):
                cases.append(dict(mode=0, delay=delay, times=times, key_after=key_after))
    for kind in (4, 5):
        for colour in [0, 0xffffffff, 0xff0000, 0xff00, 0xff, 0x123456, *[rng.getrandbits(32) for _ in range(100)]]:
            cases.append(dict(mode=1, kind=kind, colour=colour))
    cases += [dict(mode=1, kind=kind, colour=0xaabbccdd) for kind in (0, 3, 6, 255)]
    for mode in (2, 3):
        for val in (0, 1, 0x7fff, 0x8000, 0xffff, 0x10000, 0x12348001, 0xffffffff):
            for slot in range(6):
                dims = [7, 11, 13, 17, 19, 23]; dims[slot] = val
                cases.append(dict(mode=mode, dims=dims))
    for parent in (0, 1):
        for stop in (0, 1, 2, 3, 4, 5, 99):
            for mutate in (0, 1, 2, 4, 8, 15):
                cases.append(dict(mode=4, parent=parent, stop=stop, mutate=mutate))
    channels = [[0, 0, 0, 0], [255, 255, 255, 255], [0x12, 0x34, 0x56, 0x78],
                [0xffffffff] * 4, [0x12345678, 0x87654321, 0xabcdef01, 0x80000180]]
    for slot in range(4):
        for value in (1, 7, 8, 15, 16, 127, 128, 255, 256, 0x80000000, 0xffffffff):
            values = [0x12, 0x34, 0x56, 0x78]; values[slot] = value; channels.append(values)
    channels += [[rng.getrandbits(32) for _ in range(4)] for _ in range(50)]
    for kind in (0, 3, 4, 5, 6, 7, 8, 18, 23, 30, 31, 32, 255):
        for values in channels: cases.append(dict(mode=5, kind=kind, channels=values))
    for kind in (0, 4, 5, 255):
        for value in (0, 1, 0xffff, 0x10000, 0x1000000, 0x7fffffff, 0xffffffff, 0xffff0000, 0xff800000, 0x80000000):
            cases.append(dict(mode=6, kind=kind, opacity=value))
        for token, boolean, opacity_token, null in ((0,1,0xbe,False),(0x85,0,0xbe,False),
                (0x85,2,0xbe,False),(0x85,0xffffffff,0xbe,False),(0x85,1,0,False),(0x85,1,0xbe,True)):
            cases.append(dict(mode=6, kind=kind, token=token, boolean=boolean, opacity_token=opacity_token, null=null))
    for initial in (0, 0xd00, 0xd20, 0xd40, 0xd60):
        for tables in ([0xd00,0xd20,0xd40], [0,0,0], [0xd20]*3):
            for stage in (0, 1, 2, 3):
                for new in (0, 0xd00, 0xd20, 0xd40):
                    cases.append(dict(mode=6, kind=3, initial=initial, tables=tables, stage=stage, new=new, screen_mutation=True))
    for value in (0,1,0xffffffff,0x7fffffff,0x80000000,0x80000001,65535,65536,*[rng.getrandbits(32) for _ in range(32)]):
        cases.append(dict(mode=7, value=value))
    for mode in (8, 9):
        for kind, style in [(k, 4) for k in range(10)] + [(1, s) for s in range(10)] + [(255, 255)]:
            for flags in (0, 1, 2, 6, 8, 24, 31):
                cases.append(dict(mode=mode, kind=kind, style=style, flags=flags, depth=0xffff))
    for path in ('a', '.', '\\', 'image.tif', 'file.name.tif', '.hidden', 'dir\\image.tif',
                 '\\root.tif', 'dir\\', 'a.b\\c.d', 'dir/image.TIF', 'a'*255, 'a'*250+'\\b.c', 'x\\.hidden'):
        for separator in ('\\', '/', ':'):
            for ret, palette, flags, error, mutate in ((0,0,0,0,0),(0x80,0x180,0xffffffff,1,0),(0x80,0,0x80000000,0xffffffff,1)):
                cases.append(dict(mode=10, path=path, separator=separator, ret=ret, palette=palette, flags=flags, error=error, mutate=mutate))
    if args.modes is not None: cases = [case for case in cases if case["mode"] in args.modes]
    results = []
    for index, case in enumerate(cases):
        arena = bytearray(b"\xa5" * SIZE)
        h = [0] * 16; h[0] = case["mode"]
        def wr(off, value):
            arena[off:off + 4] = words([value])
        for i in range(6):
            actor = 0x400 + 128 * i
            arena[actor:actor + 100] = bytes(100)
            vals = [1, i * .5, 0, 0, 1, .25, .5, 0, 1, i + .5, -i, 2]
            struct.pack_into("<12f", arena, actor + 44, *vals)
        for off, target in ((0x408, 0x480), (0x480, 0x500), (0x488, 0x580), (0x580, 0x600), (0x508, 0x680)):
            wr(off, HEAP + target)
        struct.pack_into("<12f", arena, 0x300, 1, 0, .5, .25, 2, 0, 0, 0, 1, -2, 3, .5)
        wr(0xf00, 0x12340000)
        wr(0xb4c, 0); wr(0xb58, 0)
        wr(0x804, 0); wr(0xa04, 0)
        mode = h[0]
        if mode == 0:
            h[1:5] = [case["delay"], len(case["times"]), case["key_after"], 0x80000001]
            arena[0x200:0x200 + 4 * h[2]] = words(case["times"])
        elif mode == 1: h[1] = case["colour"]; arena[0xac] = case["kind"]
        elif mode in (2, 3): h[1:7] = case["dims"]
        elif mode == 4: h[6:10] = [case["parent"], case["mutate"], case["stop"], 0x90000001]
        elif mode == 5: h[1:5] = case["channels"]; arena[0xac] = case["kind"]
        elif mode == 6:
            arena[0xac] = case["kind"]
            initial = case.get("initial", 0); wr(0xb4c, HEAP + initial if initial else 0)
            wr(0xb58, 0 if case.get("null") else HEAP + 0xc00)
            for off, val in ((0xc00, case.get("token", 0x85)), (0xc04, case.get("boolean", 1)),
                             (0xc08, case.get("opacity_token", 0xbe)), (0xc0c, case.get("opacity", 0x800000))): wr(off, val)
            h[1:4] = case.get("tables", [0xd00,0xd20,0xd40])
            h[4], h[5], h[10] = case.get("stage", 0), case.get("new", 0), int(case.get("screen_mutation", False))
        elif mode == 7: h[1] = case["value"]
        elif mode in (8, 9):
            flags = case["flags"]
            for off, text in ((0xe00,b'actor %name'),(0xe20,b'model'),(0xe40,b'material'),(0xe80,b'visible tree')):
                arena[off:off+len(text)+1] = text+b'\0'
            wr(0x804, HEAP+0xe20 if flags&4 else 0); wr(0xa04, HEAP+0xe40 if flags&16 else 0)
            for i in range(6):
                off = 0x400 + 128*i
                struct.pack_into('<H', arena, off+16, case['depth']-i)
                arena[off+18], arena[off+32] = case['kind'], case['style']
                wr(off+12, HEAP+0x400 if i else 0)
                wr(off+20, HEAP+0xe00 if flags&1 else 0)
                wr(off+24, HEAP+0x800 if flags&2 else 0)
                wr(off+28, HEAP+0xa00 if flags&8 else 0)
        elif mode == 10:
            path=case['path'].encode(); arena[0xe00:0xe00+len(path)+1]=path+b'\0'
            arena[0xd00:0xd02]=case['separator'].encode()+b'\0'
            h[1:6]=[case['flags'],case['error'],case['mutate'],case['ret'],case['palette']]
        expected = execute(*images[0], h, arena)
        checks = []
        if len(images) > 1: checks.append(("matching PE", execute(*images[1], h, arena)))
        fixture = out / "fixture.bin"; fixture.write_bytes(words(h) + arena)
        for compiler in commands:
            checks.append((compiler, subprocess.check_output([str(out / (compiler + ".exe")), str(fixture)])))
        for compiler, actual in checks:
            if actual != expected:
                (out / f"failure-{index}-{compiler}.json").write_text(json.dumps(case, indent=2))
                (out / "expected.bin").write_bytes(expected); (out / "actual.bin").write_bytes(actual)
                raise AssertionError(f"Case {index}, {compiler} differs: {case}")
            results.append(dict(case=index, compiler=compiler, events_sha256=hashlib.sha256(actual).hexdigest()))
    manifest = dict(original_sha256=HASH, source_sha256=hashlib.sha256(SOURCE.read_bytes()).hexdigest(),
        compiler_commands=commands, scenarios=len(cases), comparisons=len(results), cases=results)
    manifest['modes'] = args.modes
    if len(images) > 1: manifest["recompiled_sha256"] = hashlib.sha256(images[1][0]).hexdigest()
    (out / "verification.json").write_text(json.dumps(manifest, indent=2) + "\n")
    print(f"PASS: {len(cases)} utility scenarios, {len(results)} original/native comparisons.")


if __name__ == "__main__":
    main()
