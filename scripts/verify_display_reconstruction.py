#!/usr/bin/env python3
"""Verify five display entries with actual production bodies and repository types.

BRender registry, pixelmap/GPU, loader and conversion are explicit controlled
boundaries. Real palette conversion, signed rectangle wrapper and byte-order
leaf execute. Full arena/global bytes are captured at every ordered callback.
DistanceFromPlane also executes unmodified original leaf bytes on physical
x87 PC53: this catches signaling-NaN quieting omitted by Unicorn. Both its
scalar store and unrounded double return are compared. ProcessShadow's sole
direct call (0x4e9469) stores a scalar copy without popping ST0, then uses the
unrounded value for its first component; a relocated original arithmetic
fragment and native component expression are compared independently.
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
SOURCE = ROOT / 'src/carma2/newcommon/18-graphics2.c'
UTILITY = ROOT / 'src/carma2/newcommon/41-utility.c'
HASH = '9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
FUNCTIONS = dict(DRDrawLine=0x47b450, ClearEntireScreen=0x47b880, DistanceFromPlane=0x47b910, KillSplashScreen=0x47b960, SplashScreenWith=0x47b990)
GLOBALS = dict(gScreen=0x74d3e0, gBack_screen=0x74d360, gDepth_buffer=0x68b8a4, gRender_palette=0x74a674, gCurrent_splash=0x68be20, gPixelFlags=0x74ca18, gGraf_spec_index=0x762324, gGraf_specs=0x662208)
BOUNDARIES = dict(BrPixelmapLine=0x538990, BrPixelmapFill=0x5382f0, PDScreenBufferSwap=0x51c520, BrMapFind=0x51eff0, BrMapRemove=0x51efd0, BrMapAdd=0x51efa0, BrPixelmapFree=0x537dc0, LoadPixelmap=0x48ec00, BRPM_convert=0x518700, BrPixelmapRectangleCopy=0x538590, BrFailure=0x527c80)
ADDRESSES = dict(FUNCTIONS, **GLOBALS, **BOUNDARIES)
HEAP, STACK, STOP, SIZE = 0x20000000, 0x1080000, 0x1081000, 0x5000
FIELDS = 'mode backtype dest screen current found loaded flags mutate width height x1 y1 x2 y2 colour seed repeat px py pz a b c d highest'.split()


def words(values):
    return struct.pack('<' + 'I' * len(values), *[v & 0xffffffff for v in values])


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--original', type=Path, default=ROOT / 'build-reference/originals/CARMA2_HW.EXE')
    p.add_argument('--dependency-dir', type=Path, default=ROOT / 'build-audit/joystick/python-deps')
    p.add_argument('--gcc', type=Path, default=ROOT / 'build-tools/winlibs-i686/mingw32/bin/gcc.exe')
    p.add_argument('--msvc-root', type=Path, default=ROOT / 'build-tools/msvc-modern')
    p.add_argument('--msvc5-root', type=Path, default=ROOT / 'build-tools/msvc500sp3')
    p.add_argument('--output', type=Path, default=ROOT / 'build-audit/display-tests')
    p.add_argument('--symbols', type=Path)
    p.add_argument('--recompiled', type=Path, default=ROOT / 'build-matching/rec2.exe')
    p.add_argument('--case-index', action='append', type=int)
    args = p.parse_args()
    sys.path.insert(0, str(args.dependency_dir.resolve()))
    from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
    from unicorn.x86_const import UC_X86_REG_EIP, UC_X86_REG_ESP, UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX, UC_X86_REG_FPCW
    binary = args.original.read_bytes()
    if hashlib.sha256(binary).hexdigest() != HASH:
        raise SystemExit('Original executable hash differs')
    images = [(binary, ADDRESSES)]
    if args.symbols:
        mapping = json.loads(args.symbols.read_text()); rebuilt = args.recompiled.read_bytes()
        if mapping['format'] != 1 or mapping['original_sha256'] != HASH or mapping['recompiled_sha256'] != hashlib.sha256(rebuilt).hexdigest():
            raise SystemExit('Stale or incompatible display symbol map')
        mapped = {s['orig']: s['recomp'] for s in mapping['symbols']}
        images.append((rebuilt, {name: mapped[address] for name, address in ADDRESSES.items()}))
    out = args.output.resolve(); out.mkdir(parents=True, exist_ok=True)
    driver = r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "18-graphics2.h"
#include "41-utility.h"
#include "63-loading3.h"
#include "globvars.h"
#include "platform.h"
typedef char layouts[sizeof(void*)==4&&sizeof(br_pixelmap)==68&&sizeof(tGraf_spec)==52?1:-1];
enum {MODE,BACKTYPE,DEST,SCREEN,CURRENT,FOUND,LOADED,FLAGS,MUTATE,WIDTH,HEIGHT,X1,Y1,X2,Y2,COLOUR,SEED,REPEAT,PX,PY,PZ,A,B,C,D,HIGHEST};
static unsigned a[26],calls;
static unsigned char* heap;
br_pixelmap *gScreen,*gBack_screen,*gDepth_buffer,*gRender_palette,*gCurrent_splash;
tPixelFlags gPixelFlags;
tGraf_spec gGraf_specs[2];int gGraf_spec_index;
static br_pixelmap* C2_HOOK_CDECL map(unsigned n) {return n?(br_pixelmap*)(heap+0x100+(n-1)*68):NULL;}
static void C2_HOOK_CDECL snapshot(void) {
 unsigned globals[7];globals[0]=(unsigned)gScreen;globals[1]=(unsigned)gBack_screen;globals[2]=(unsigned)gDepth_buffer;globals[3]=(unsigned)gRender_palette;globals[4]=(unsigned)gCurrent_splash;globals[5]=(unsigned)gPixelFlags;globals[6]=(unsigned)gGraf_spec_index;
 fwrite(globals,sizeof(globals),1,stdout);fwrite(gGraf_specs,sizeof(gGraf_specs),1,stdout);fwrite(heap,0x5000,1,stdout);
}
static void C2_HOOK_CDECL event(unsigned code,unsigned n,const unsigned* values) {unsigned v[12],i;memset(v,0,sizeof(v));v[0]=code;for(i=0;i<n;i++)v[i+1]=values[i];fwrite(v,sizeof(v),1,stdout);snapshot();calls++;if(a[MUTATE]&1)gPixelFlags=(tPixelFlags)((unsigned)gPixelFlags+17);}
void C2_HOOK_CDECL BrPixelmapLine(br_pixelmap* dst,br_int_32 x1,br_int_32 y1,br_int_32 x2,br_int_32 y2,br_uint_32 colour) {unsigned v[6];v[0]=(unsigned)dst;v[1]=x1;v[2]=y1;v[3]=x2;v[4]=y2;v[5]=colour;event(1,6,v);memcpy(dst->pixels,&colour,4);}
void C2_HOOK_CDECL BrPixelmapFill(br_pixelmap* dst,br_uint_32 colour) {unsigned v[2];v[0]=(unsigned)dst;v[1]=colour;event(2,2,v);memcpy(dst->pixels,&colour,4);if(a[MUTATE]&4){gBack_screen=map(3);gDepth_buffer=map(4);gGraf_spec_index=1;}}
void C2_HOOK_FASTCALL PDScreenBufferSwap(int value) {unsigned v=(unsigned)value;event(3,1,&v);if(a[MUTATE]&4){gBack_screen=map(3);gGraf_spec_index=1;}if(a[MUTATE]&32)gCurrent_splash=map(7);}
br_pixelmap* C2_HOOK_CDECL BrMapFind(const char* name) {unsigned v[2];v[0]=strcmp(name,"test.pix")==0;v[1]=(unsigned)map(a[FOUND]);event(4,2,v);if(a[MUTATE]&16)gCurrent_splash=map(a[FOUND]);return map(a[FOUND]);}
br_pixelmap* C2_HOOK_CDECL BrMapRemove(br_pixelmap* item) {unsigned v=(unsigned)item;event(5,1,&v);if(a[MUTATE]&2)gCurrent_splash=map(7);return item;}
br_pixelmap* C2_HOOK_CDECL BrMapAdd(br_pixelmap* item) {unsigned v=(unsigned)item;event(6,1,&v);return item;}
void C2_HOOK_CDECL BrPixelmapFree(br_pixelmap* item) {unsigned v=(unsigned)item;event(7,1,&v);memset(item,0xdd,sizeof(*item));}
br_pixelmap* C2_HOOK_FASTCALL LoadPixelmap(const char* name) {unsigned v[2];v[0]=strcmp(name,"test.pix")==0;v[1]=(unsigned)map(a[LOADED]);event(8,2,v);return map(a[LOADED]);}
void C2_HOOK_FASTCALL BRPM_convert(br_pixelmap* item,int type) {unsigned v[2];v[0]=(unsigned)item;v[1]=type;event(9,2,v);if(a[MUTATE]&8)gCurrent_splash=map(7);}
void C2_HOOK_CDECL BrPixelmapRectangleCopy(br_pixelmap* dst,br_int_32 dx,br_int_32 dy,br_pixelmap* src,br_int_32 sx,br_int_32 sy,br_int_32 w,br_int_32 h) {unsigned v[8];v[0]=(unsigned)dst;v[1]=dx;v[2]=dy;v[3]=(unsigned)src;v[4]=sx;v[5]=sy;v[6]=w;v[7]=h;event(10,8,v);memcpy(dst->pixels,&calls,4);}
void C2_HOOK_CDECL BrFailure(const char* message,...) {unsigned v=strcmp(message,"Unsupported back buffer type.")==0;event(11,1,&v);event(0,0,NULL);exit(0);}
'''
    driver += '\n'.join(extract(UTILITY.read_text(), name) for name in ('PaletteEntry16Bit', 'DrPixelmapRectangleCopyPossibleLock', 'PixelmapSwapByteOrder'))
    driver += '\n'.join(extract(SOURCE.read_text(), name) for name in FUNCTIONS)
    driver += r'''
/* Capture the physical ST0 return without imposing a float prototype. */
static double C2_HOOK_CDECL raw_distance(void* position,unsigned arg_a,unsigned arg_b,unsigned arg_c,unsigned arg_d) {
#ifdef _MSC_VER
 double value;
#ifdef REC2_MATCHING
 __asm {
  push arg_d
  push arg_c
  push arg_b
  push arg_a
  mov ecx,position
  call DistanceFromPlane
  fstp qword ptr value
 }
#else
 __asm {
  push arg_d
  push arg_c
  push arg_b
  push arg_a
  push position
  call DistanceFromPlane
  add esp,20
  fstp qword ptr value
 }
#endif
 return value;
#else
 typedef double (*raw_plane_fn)(void*,unsigned,unsigned,unsigned,unsigned);
 typedef double (C2_HOOK_FASTCALL *typed_plane_fn)(const br_vector3*,br_scalar,br_scalar,br_scalar,br_scalar);
 volatile union {typed_plane_fn typed;raw_plane_fn raw;} fn;
 fn.typed=DistanceFromPlane;return fn.raw(position,arg_a,arg_b,arg_c,arg_d);
#endif
}
int C2_HOOK_CDECL main(int argc,char**argv) {
 FILE* f;unsigned i,j,v=0;float scalars[7],result,highest,first;double wide,scale;
 if(argc!=2&&argc!=3)return 90;
 if(!strcmp(argv[1],"schema")){printf("[%u,%u,%u]\n",(unsigned)sizeof(br_pixelmap),(unsigned)sizeof(tGraf_spec),(unsigned)offsetof(br_pixelmap,type));return 0;}
 f=fopen(argv[1],"rb");if(!f)return 91;if(fread(a,sizeof(a),1,f)!=1)return 92;fclose(f);
 heap=(unsigned char*)VirtualAlloc((void*)0x20000000,0x5000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(heap!=(void*)0x20000000)return 93;
 _setmode(_fileno(stdout),_O_BINARY);_controlfp(_PC_53,_MCW_PC);memset(heap,(int)a[SEED],0x5000);memset(gGraf_specs,(int)a[SEED],sizeof(gGraf_specs));
 for(i=1;i<=8;i++){map(i)->pixels=heap+0x3000+32*i;map(i)->width=(br_uint_16)a[WIDTH];map(i)->height=(br_uint_16)a[HEIGHT];map(i)->type=(br_uint_8)a[BACKTYPE];}
 map(3)->type=(br_uint_8)(a[BACKTYPE]^7);
 map(8)->pixels=heap+0x2000;for(i=0;i<256;i++){v=0xa5000000|(i*0x10307&0xffffff);memcpy(heap+0x2000+4*i,&v,4);}
 memcpy(heap+0x40,a+PX,12);strcpy((char*)heap+0x80,"test.pix");memcpy(scalars,a+PX,sizeof(scalars));
 gScreen=map(a[SCREEN]);gBack_screen=map(1);gDepth_buffer=map(2);gRender_palette=map(8);gCurrent_splash=map(a[CURRENT]);gPixelFlags=(tPixelFlags)a[FLAGS];gGraf_spec_index=0;gGraf_specs[0].black_value=0x12345678;gGraf_specs[1].black_value=0x87654321;
 if(argc==3){
  wide=raw_distance(heap+0x40,a[A],a[B],a[C],a[D]);fwrite(&wide,8,1,stdout);
  memcpy(&highest,a+HIGHEST,4);scale=2.2-(double)highest;scale*=0.52;scale*=-0.45454545454545453;scale=0.4-scale;scale=1.0-scale;
  first=(float)((wide*scale)*(double)scalars[3]);fwrite(&first,4,1,stdout);return 0;
 }
 v=0;for(j=0;j<a[REPEAT];j++)switch(a[MODE]){
 case 0:DRDrawLine(map(a[DEST]),(int)a[X1],(int)a[Y1],(int)a[X2],(int)a[Y2],(int)a[COLOUR]);break;
 case 1:ClearEntireScreen();break;
 case 2:result=(float)DistanceFromPlane((br_vector3*)(heap+0x40),scalars[3],scalars[4],scalars[5],scalars[6]);memcpy(&v,&result,4);break;
 case 3:KillSplashScreen();break;
 case 4:SplashScreenWith((char*)heap+0x80);break;
 default:return 94;}
 event(0,1,&v);return 0;
}
'''
    harness = out / 'harness.c'; harness.write_text(driver)
    includes = [ROOT / path for path in ('src/carma2/newcommon', 'src/carma2/include', 'src/carma2/hooks', 'src/carma2/platform', 'src/carma2/s3/include', 'src/brender/include')]
    msvc = sorted((args.msvc_root / 'VC/Tools/MSVC').iterdir())[-1]; sdk = args.msvc_root / 'Windows Kits/10'; version = sorted((sdk / 'Include').iterdir())[-1].name
    env = dict(os.environ); env['PATH'] = str(msvc / 'bin/Hostx64/x86') + os.pathsep + env['PATH']
    env['INCLUDE'] = os.pathsep.join(map(str, [msvc / 'include', *[sdk / f'Include/{version}/{part}' for part in ('ucrt', 'shared', 'um')]])); env['LIB'] = os.pathsep.join(map(str, [msvc / 'lib/x86', *[sdk / f'Lib/{version}/{part}/x86' for part in ('ucrt', 'um')]]))
    msvc5 = args.msvc5_root.resolve(); env5 = dict(os.environ); env5['PATH'] = str(msvc5 / 'bin') + os.pathsep + str(msvc5 / 'redist') + os.pathsep + env5['PATH']; env5['INCLUDE'] = str(msvc5 / 'include'); env5['LIB'] = str(msvc5 / 'lib')
    commands = {
        'msvc': [str(msvc / 'bin/Hostx64/x86/cl.exe'), '/nologo', '/O2', '/W3', '/WX', '/D_CRT_SECURE_NO_WARNINGS', *['/I' + str(p) for p in includes], str(harness), '/Fo' + str(out / 'msvc.obj'), '/Fe' + str(out / 'msvc.exe')],
        'msvc5': [str(msvc5 / 'bin/cl.exe'), '/nologo', '/O2', '/Ob2', '/G5', '/Gr', '/ML', '/W3', '/WX', '/DREC2_MATCHING', '/I' + str(ROOT / 'src/msvc500/include'), *['/I' + str(p) for p in includes], str(harness), '/Fo' + str(out / 'msvc5.obj'), '/Fe' + str(out / 'msvc5.exe')],
        'mingw': [str(args.gcc), '-std=gnu89', '-O2', '-Wall', '-Wextra', '-Werror', '-Wno-unused-parameter', *['-I' + str(p) for p in includes], str(harness), '-o', str(out / 'mingw.exe')],
    }
    schemas = []
    for compiler, command in commands.items():
        result = subprocess.run(command, cwd=out, env=env if compiler == 'msvc' else env5 if compiler == 'msvc5' else None, text=True, capture_output=True)
        (out / f'{compiler}-build.txt').write_text(result.stdout + result.stderr)
        if result.returncode: raise RuntimeError(result.stdout + result.stderr)
        schemas.append(json.loads(subprocess.check_output([str(out / f'{compiler}.exe'), 'schema'], text=True)))
    assert schemas[0] == schemas[1] == schemas[2] == [68, 52, 44]
    probe = ROOT / 'scripts/verify_display_original_x87_probe.c'
    probe_exe = out / 'original-x87.exe'
    probe_command = [str(args.gcc), '-std=gnu89', '-O2', '-Wall', '-Wextra', '-Werror', str(probe), '-o', str(probe_exe)]
    subprocess.run(probe_command, check=True)

    def original(image, addresses, c):
        u = Uc(UC_ARCH_X86, UC_MODE_32)
        pe = struct.unpack_from('<I', image, 60)[0]; base = struct.unpack_from('<I', image, pe + 52)[0]; size = struct.unpack_from('<I', image, pe + 80)[0]
        u.mem_map(base, (size + 4095) & ~4095); u.mem_map(HEAP, 0x100000); u.mem_map(0x1070000, 0x30000)
        table = pe + 24 + struct.unpack_from('<H', image, pe + 20)[0]
        for i in range(struct.unpack_from('<H', image, pe + 6)[0]):
            _, rva, size, offset = struct.unpack_from('<IIII', image, table + 40*i + 8); u.mem_write(base+rva, image[offset:offset+size])
        def wr(address, value): u.mem_write(address, words([value]))
        def rd(address): return struct.unpack('<I', u.mem_read(address, 4))[0]
        def map_addr(n): return HEAP+0x100+(n-1)*68 if n else 0
        u.mem_write(HEAP, bytes([c['seed']])*SIZE)
        for i in range(1,9):
            ptr = map_addr(i); wr(ptr+8, HEAP+0x3000+32*i)
            u.mem_write(ptr+52, struct.pack('<HH', c['width']&65535,c['height']&65535)); u.mem_write(ptr+schemas[0][2], bytes([c['backtype']&255]))
        u.mem_write(map_addr(3)+schemas[0][2], bytes([(c['backtype']^7)&255]))
        wr(map_addr(8)+8, HEAP+0x2000)
        u.mem_write(HEAP+0x2000, words([0xa5000000|(i*0x10307&0xffffff) for i in range(256)]))
        u.mem_write(HEAP+0x40, words([c[f] for f in ('px','py','pz')])); u.mem_write(HEAP+0x80, b'test.pix\0')
        values = [map_addr(c['screen']),map_addr(1),map_addr(2),map_addr(8),map_addr(c['current']),c['flags'],0]
        for name,value in zip(GLOBALS,values): wr(addresses[name],value)
        u.mem_write(addresses['gGraf_specs'], bytes([c['seed']])*104); wr(addresses['gGraf_specs']+20,0x12345678);wr(addresses['gGraf_specs']+72,0x87654321)
        output=bytearray(); calls=0; failed=False
        def event(code,values):
            nonlocal calls
            output.extend(words([code,*values,*([0]*(11-len(values)))]))
            output.extend(words([rd(addresses[name]) for name in list(GLOBALS)[:7]]));output.extend(u.mem_read(addresses['gGraf_specs'],104));output.extend(u.mem_read(HEAP,SIZE))
            calls+=1
            if c['mutate']&1:wr(addresses['gPixelFlags'],rd(addresses['gPixelFlags'])+17)
        reverse={addresses[name]:i+1 for i,name in enumerate(BOUNDARIES)}
        def hook(uc,address,size,context):
            nonlocal failed
            code=reverse[address];sp=uc.reg_read(UC_X86_REG_ESP);ecx=uc.reg_read(UC_X86_REG_ECX);edx=uc.reg_read(UC_X86_REG_EDX);result=0
            vals=[]
            if code==1:
                vals=[rd(sp+4+4*i) for i in range(6)];event(code,vals);wr(rd(vals[0]+8),vals[5])
            elif code==2:
                vals=[rd(sp+4),rd(sp+8)];event(code,vals);wr(rd(vals[0]+8),vals[1])
                if c['mutate']&4:
                    wr(addresses['gBack_screen'],map_addr(3));wr(addresses['gDepth_buffer'],map_addr(4));wr(addresses['gGraf_spec_index'],1)
            elif code==3:
                event(code,[ecx])
                if c['mutate']&4:wr(addresses['gBack_screen'],map_addr(3));wr(addresses['gGraf_spec_index'],1)
                if c['mutate']&32:wr(addresses['gCurrent_splash'],map_addr(7))
            elif code in (4,8):
                pointer=rd(sp+4) if code==4 else ecx;valid=bytes(uc.mem_read(pointer,9))==b'test.pix\0';result=map_addr(c['found' if code==4 else 'loaded']);event(code,[int(valid),result])
                if code==4 and c['mutate']&16:wr(addresses['gCurrent_splash'],result)
            elif code in (5,6,7):
                pointer=rd(sp+4);event(code,[pointer]);result=pointer
                if code==5 and c['mutate']&2:wr(addresses['gCurrent_splash'],map_addr(7))
                if code==7:uc.mem_write(pointer,b'\xdd'*68)
            elif code==9:
                event(code,[ecx,edx&255])
                if c['mutate']&8:wr(addresses['gCurrent_splash'],map_addr(7))
            elif code==10:
                vals=[rd(sp+4+4*i) for i in range(8)];event(code,vals);wr(rd(vals[0]+8),calls)
            else:
                pointer=rd(sp+4);valid=bytes(uc.mem_read(pointer,30))==b'Unsupported back buffer type.\0';event(code,[int(valid)]);event(0,[]);failed=True;uc.emu_stop();return
            uc.reg_write(UC_X86_REG_EAX,result);uc.reg_write(UC_X86_REG_EIP,rd(sp));uc.reg_write(UC_X86_REG_ESP,sp+4)
        for target in reverse:u.hook_add(UC_HOOK_CODE,hook,begin=target,end=target)
        result=0; numeric=None
        for _ in range(c['repeat']):
            mode=c['mode'];values=[];ecx=0;edx=0
            if mode==0:ecx=map_addr(c['dest']);edx=c['x1'];values=[c[f] for f in ('y1','x2','y2','colour')]
            elif mode==2:ecx=HEAP+0x40;values=[c[f] for f in ('a','b','c','d')]
            elif mode==4:ecx=HEAP+0x80
            u.mem_write(STACK,words([STOP,*values]));u.reg_write(UC_X86_REG_ESP,STACK);u.reg_write(UC_X86_REG_ECX,ecx&0xffffffff);u.reg_write(UC_X86_REG_EDX,edx&0xffffffff);u.reg_write(UC_X86_REG_FPCW,0x27f)
            u.emu_start(addresses[list(FUNCTIONS)[mode]],STOP,count=100000)
            if failed:return bytes(output),None
            assert u.reg_read(UC_X86_REG_EIP)==STOP,'Display instruction budget exhausted'
            assert u.reg_read(UC_X86_REG_ESP)==STACK+4+len(values)*4,'Display ABI stack cleanup differs'
            if mode==2:
                # FST64 preserves ST0; the following FSTP32 captures scalar API use.
                u.mem_write(STOP,b'\xdd\x15'+words([HEAP+0x4810])+b'\xd9\x1d'+words([HEAP+0x4800]));u.emu_start(STOP,STOP+12,count=2)
                result=rd(HEAP+0x4800);wide_bits=f'{struct.unpack("<Q",u.mem_read(HEAP+0x4810,8))[0]:016x}'
                numeric=dict(scalar=result,double_bits=wide_bits)
                u.mem_write(HEAP+0x4800,bytes([c['seed']])*4);u.mem_write(HEAP+0x4810,bytes([c['seed']])*8)
        event(0,[result]);return bytes(output),numeric

    def case(mode,**kw):
        c={field:0 for field in FIELDS};c.update(mode=mode,backtype=3,dest=3,screen=3,current=4,found=5,loaded=6,flags=0x76543210,width=640,height=480,x1=1,y1=2,x2=3,y2=4,colour=17,seed=165,repeat=1,px=0x3f800000,py=0x40000000,pz=0x40400000,a=0x3f800000,b=0x40000000,c=0x40400000,d=0x3f800000,highest=0x3f800000);c.update(kw);return c
    cases=[case(0,backtype=t,colour=col,x1=x,y1=-x,x2=0x7fffffff,y2=0x80000000) for t in (3,4,5,0,1,6,255) for col in (0,1,17,127,255) for x in (0,-17)]
    cases += [case(0,backtype=3,colour=colour,dest=dest) for colour in (-1,0x80000000,0x7fffffff) for dest in (1,3)]
    cases += [case(1,screen=s,mutate=m,repeat=r) for s in (0,1,2,3) for m in (0,1,4,5) for r in (1,2)]
    cases += [case(3,current=cur,mutate=m,repeat=2) for cur in (0,4) for m in (0,1,2,3)]
    cases += [case(4,current=cur,found=f,loaded=l,mutate=m,width=w,height=h) for cur in (0,4) for f in (0,4,5) for l in (0,6) for m in (0,1,2,8,16,32,9) for w,h in ((640,480),(0x8001,0xfffe),(65535,0))]
    # Every scalar position, signs/payloads, infinities and subnormal boundaries.
    special=(0,0x80000000,1,0x80000001,0x007fffff,0x00800000,0x7f7fffff,0x7f800000,0xff800000,0x7fc00001,0xffc12345,0x7f800001,0xff800001,0x7fbfffff)
    for field in ('px','py','pz','a','b','c','d'):
        for value in special:cases.append(case(2,**{field:value}))
    cases += [case(2,a=0,b=0,c=0,d=d) for d in (0,0x3f800000,0x7fc00001,0x7f800001)]
    rng=random.Random(0x47b910)
    for _ in range(100):
        cases.append(case(2,**{field:rng.randrange(0x00800000,0x7f000000)|(rng.randrange(2)<<31) for field in ('px','py','pz','a','b','c','d')}))
    cases += [case(2,highest=h) for h in (0,0x3f800000,0x400ccccd)]
    if args.case_index is not None and any(i<0 or i>=len(cases) for i in args.case_index):p.error(f'--case-index must be between 0 and {len(cases)-1}')
    results=[];physical=[]
    for index,c in enumerate(cases):
        if args.case_index is not None and index not in args.case_index:continue
        expected,emulated=original(*images[0],c)
        if c['mode']==2:
            observed=json.loads(subprocess.check_output([str(probe_exe),str(args.original),*[hex(c[f]&0xffffffff) for f in ('px','py','pz','a','b','c','d','highest')]],text=True));physical.append(dict(case_index=index,scalar_bits=observed[0],double_bits=observed[1],caller_scalar_copy=observed[2],caller_first_component=observed[3]))
            assert observed[0]==observed[2],'Original caller scalar store differs from API scalar store'
            if emulated['scalar']!=observed[0] or emulated['double_bits']!=observed[1]:
                (out/f'physical-divergence-{index}.json').write_text(json.dumps(dict(case=c,unicorn=emulated,physical=observed),indent=2))
                raise AssertionError(f'Original Unicorn/physical x87 scenario {index} differs; no return normalization is applied')
        fixture=out/'case.bin';fixture.write_bytes(words([c[f] for f in FIELDS]))
        checks=[(compiler,subprocess.check_output([str(out/f'{compiler}.exe'),str(fixture)])) for compiler in commands]
        if c['mode']==2:
            for compiler in commands:
                raw=subprocess.check_output([str(out/f'{compiler}.exe'),str(fixture),'raw-return'])
                raw_bits=f'{struct.unpack_from("<Q",raw)[0]:016x}'; first=struct.unpack_from('<I',raw,8)[0]
                if raw_bits!=observed[1] or first!=observed[3]:
                    (out/f'raw-return-divergence-{compiler}-{index}.json').write_text(json.dumps(dict(case=c,compiler=compiler,native_double_bits=raw_bits,original_double_bits=observed[1],native_first_component=first,original_first_component=observed[3]),indent=2))
                    raise AssertionError(f'Distance raw/caller {compiler} return scenario {index} differs; see {out}')
        if len(images)>1:
            actual,numeric=original(*images[1],c)
            if c['mode']==2 and numeric['double_bits']!=observed[1]:
                (out/f'raw-return-divergence-matching-{index}.json').write_text(json.dumps(dict(case=c,matching=numeric,physical=observed),indent=2))
                raise AssertionError(f'Distance raw matching PE return scenario {index} differs; see {out}')
            checks.append(('matching PE',actual))
        for compiler,actual in checks:
            if actual!=expected:
                prefix=out/f'failure-{compiler.replace(" ","-")}-{index}'
                prefix.with_suffix('.expected.bin').write_bytes(expected);prefix.with_suffix('.actual.bin').write_bytes(actual)
                difference=next((i for i,(a,b) in enumerate(zip(actual,expected)) if a!=b),None)
                prefix.with_suffix('.json').write_text(json.dumps(dict(case=c,compiler=compiler,first_difference=difference,expected_size=len(expected),actual_size=len(actual)),indent=2))
                raise AssertionError(f'Display {compiler} scenario {index} differs at {difference}; see {out}')
            results.append(dict(case_index=index,compiler=compiler,state_and_calls_sha256=hashlib.sha256(actual).hexdigest(),all_bytes_match=True))
        if (index+1)%100==0:print(f'Verified {index+1}/{len(cases)} display scenarios',flush=True)
    (out/'results.json').write_text(json.dumps(results,indent=2)+'\n');(out/'physical-x87.json').write_text(json.dumps(dict(cases=physical,all_scalar_bits_match_unicorn=True,return_normalization_applied=False),indent=2)+'\n')
    manifest=dict(original_sha256=HASH,comparisons=len(results),scenario_count=len(cases),complete_suite=args.case_index is None,all_cases_passed=True,functions=FUNCTIONS,type_schema=schemas[0],compiler_commands=commands,physical_probe_command=probe_command,physical_probe_sha256=hashlib.sha256(probe.read_bytes()).hexdigest(),harness_sha256=hashlib.sha256(driver.encode()).hexdigest(),source_sha256={str(path.relative_to(ROOT)):hashlib.sha256(path.read_bytes()).hexdigest() for path in (SOURCE,UTILITY,ROOT/'src/carma2/newcommon/18-graphics2.h',ROOT/'src/carma2/newcommon/41-utility.h',ROOT/'src/carma2/include/rec2_types.h')},distance_contract=dict(precision='normal runtime x87 PC53',original_direct_caller=0x4e9469,caller_function='ProcessShadow',scalar_copy=0x4e9471,unrounded_first_component=[0x4e9475,0x4e9483,0x4e9485],physical_cases=len(physical),native_raw_double_and_first_component_checks=len(physical)*len(commands),matching_raw_double_checked=len(images)>1,return_normalization_applied=False),limitations='Registry/loader/conversion/pixelmap/GPU callbacks are controlled; free poisons storage without allocator release. Unsupported colour format uses the public nonreturning BrFailure contract. Scalar store, raw double return and caller first-component arithmetic are checked against physical original PC53 without normalization. ProcessShadow itself is not reconstructed by this fixture; later components reload its scalar copy. Ordered snapshots cover all globals and full arena.')
    if len(images)>1:manifest.update(recompiled_sha256=hashlib.sha256(images[1][0]).hexdigest(),symbols_sha256=hashlib.sha256(args.symbols.read_bytes()).hexdigest())
    (out/'verification.json').write_text(json.dumps(manifest,indent=2)+'\n');print(f'Passed {len(results)} original/native display comparisons')


if __name__=='__main__':
    main()
