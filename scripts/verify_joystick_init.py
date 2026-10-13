"""Compare PDInitJoysticks and its initialization helpers with the original x86.

Requires Python 3.10+, Unicorn, and a runnable i686 GCC compiler. Uses repository
types, extracts the actual current C bodies, and provides SDK-typed COM vtables.
The original executes its initialization, acquisition and reset instructions;
enumeration, GetInfo and force-feedback setup are controlled dependencies. This
does not claim to verify the still-incomplete bodies of those dependencies.

Example: python scripts/verify_joystick_init.py
    --dependency-dir build-audit/joystick/python-deps
"""
import argparse
import hashlib
import json
import pathlib
import struct
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
parser.add_argument('--original', type=pathlib.Path, default=ROOT / 'build-reference/originals/CARMA2_HW.EXE')
parser.add_argument('--cc', default=str(ROOT / 'build-tools/winlibs-i686/mingw32/bin/gcc.exe'))
parser.add_argument('--dependency-dir', type=pathlib.Path)
parser.add_argument('--output-dir', type=pathlib.Path, default=ROOT / 'build-audit/joystick-init')
args = parser.parse_args()
if args.dependency_dir:
    sys.path.insert(0, str(args.dependency_dir.resolve()))
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX, UC_X86_REG_ESP, UC_X86_REG_EIP

binary = args.original.read_bytes()
assert hashlib.sha256(binary).hexdigest() == '9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
pe = struct.unpack_from('<I', binary, 0x3c)[0]
nsects = struct.unpack_from('<H', binary, pe + 6)[0]
sect = pe + 24 + struct.unpack_from('<H', binary, pe + 20)[0]
source = (ROOT / 'src/carma2/platform/win32/win32_dinput.c').read_text()
out = args.output_dir.resolve()
out.mkdir(parents=True, exist_ok=True)


def extract(name):
    start = source.index(' C2_HOOK_FASTCALL ' + name + '(')
    start = source.rfind('\n', 0, start) + 1
    end = source.index('{', start) + 1
    depth = 1
    while depth:
        depth += (source[end] == '{') - (source[end] == '}')
        end += 1
    return source[start:end]


driver = r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include "win32_dinput.h"
#define MAX_COUNT_JOYSTICKS 16
#define MAX_COUNT_EFFECTS 30
typedef char check_layout[sizeof(void*)==4&&sizeof(tDirectInputJoystickInfo)==104&&offsetof(tDirectInputJoystickInfo,productName)==12&&offsetof(tDirectInputJoystickInfo,count_buttons)==92&&offsetof(tDirectInputJoystickInfo,sizeData)==96&&offsetof(tDirectInputJoystickInfo,data)==100?1:-1];
IDirectInputA *gDirectInputJoystickHandle;
IDirectInputDevice2A *gDirectInputJoystickDevices[16], devices[16];
IDirectInputEffect *gDirectInputEffects[30];
tDirectInputJoystickInfo gDirectInputJoystickInfos[16];
int gJoystickFFB,gCountEnumeratedJoystickDinputDevices,gDirectInputJoystickEnumerated,gJoystick_index;
int cfg[13];
char trace[32768];
IDirectInputA handle;
void note(const char*f,...){va_list a;va_start(a,f);vsprintf(trace+strlen(trace),f,a);va_end(a);strcat(trace,",");}
void dr_dprintf(const char*f,...){const unsigned char*p=(const unsigned char*)f;va_list a;int i;strcat(trace,"log:");for(;*p;p++)sprintf(trace+strlen(trace),"%02x",*p);va_start(a,f);i=va_arg(a,int);va_end(a);note(":%d",i);}
HINSTANCE WINAPI fake_module(LPCSTR name){if(name)exit(80);note("module");return (HINSTANCE)0x1234;}
HRESULT WINAPI fake_create(HINSTANCE module,DWORD version,LPDIRECTINPUTA*out,LPUNKNOWN outer){if(module!=(HINSTANCE)0x1234||version!=0x500||out!=&gDirectInputJoystickHandle||outer)exit(81);note("create");if(cfg[8])gDirectInputJoystickHandle=&handle;return DIERR_GENERIC;}
BOOL CALLBACK Win32DInputJoystickEnum(LPCDIDEVICEINSTANCEA instance,LPVOID context){exit(82);return 0;}
HRESULT WINAPI enum_devices(IDirectInputA*self,DWORD type,LPDIENUMDEVICESCALLBACKA callback,void*context,DWORD flags){int i;if(self!=&handle||type!=4||callback!=Win32DInputJoystickEnum||context!=&handle||flags!=1)exit(83);note("enum");gCountEnumeratedJoystickDinputDevices=cfg[6];for(i=0;i<16;i++)if(cfg[7]&(1<<i))gDirectInputJoystickDevices[i]=&devices[i];return DIERR_GENERIC;}
HRESULT WINAPI acquire(IDirectInputDevice2A*self){int i=self-devices;static const HRESULT results[]={DI_OK,DI_NOEFFECT,DIERR_INVALIDPARAM,DIERR_OTHERAPPHASPRIO,DIERR_GENERIC};if(i<0||i>=16)exit(84);note("acquire:%d",i);return results[cfg[12]==5?i%5:cfg[12]];}
HRESULT WINAPI command(IDirectInputDevice2A*self,DWORD command){int i=self-devices;if(i<0||i>=16)exit(85);note("command:%d:%lu",i,(unsigned long)command);return DIERR_GENERIC;}
IDirectInputAVtbl handle_vtable={.EnumDevices=enum_devices};
IDirectInputDevice2AVtbl device_vtable={.Acquire=acquire,.SendForceFeedbackCommand=command};
tDirectInputJoystickInfo* C2_HOOK_FASTCALL JoystickDInputGetInfo(int i,tU32*buttons,tU32*axes){if(buttons||axes)exit(86);note("info:%d",i);if(i==cfg[11])gDirectInputJoystickHandle=NULL;return &gDirectInputJoystickInfos[i];}
int C2_HOOK_FASTCALL InitForceFeedback(void){note("ff-init");if(cfg[10]>=0)gJoystickFFB=cfg[10];return cfg[9];}
void C2_HOOK_FASTCALL RegisterJoystickFFBForces(void){note("register");}
#define GetModuleHandleA fake_module
#define DirectInputCreateA fake_create
'''
driver += '\n'.join(extract(name) for name in [
    'InitDirectInput', 'AcquireDInputJoystickDevice', 'ResetForceFeedback',
    'ResetDInputJoystickFFB', 'JoystickDInputBegin', 'PDInitJoysticks'])
driver += r'''
int main(int argc,char**argv){int i,present=0,zero_effects=0;unsigned char*p;
if(argc!=14){return 87;}
for(i=0;i<13;i++)cfg[i]=atoi(argv[i+1]);
handle.lpVtbl=&handle_vtable;for(i=0;i<16;i++){devices[i].lpVtbl=&device_vtable;gDirectInputJoystickDevices[i]=cfg[5]&(1<<i)?&devices[i]:NULL;}
gDirectInputJoystickHandle=cfg[0]?&handle:NULL;gCountEnumeratedJoystickDinputDevices=cfg[1];gDirectInputJoystickEnumerated=cfg[2];gJoystick_index=cfg[3];gJoystickFFB=cfg[4];
memset(gDirectInputJoystickInfos,0xa5,sizeof(gDirectInputJoystickInfos));memset(gDirectInputEffects,0xa5,sizeof(gDirectInputEffects));
PDInitJoysticks();for(i=0;i<16;i++)if(gDirectInputJoystickDevices[i])present|=1<<i;for(i=0;i<30;i++)zero_effects+=gDirectInputEffects[i]==NULL;
printf("%d %d %d %d %d %d %d\n%s\n",gDirectInputJoystickHandle!=NULL,gCountEnumeratedJoystickDinputDevices,gDirectInputJoystickEnumerated,gJoystick_index,gJoystickFFB,present,zero_effects,trace);
p=(unsigned char*)gDirectInputJoystickInfos;for(i=0;i<sizeof(gDirectInputJoystickInfos);i++)printf("%02x",p[i]);puts("");return 0;}
'''
(out / 'init.c').write_text(driver)
includes = ['src/carma2/platform/win32', 'src/carma2/platform', 'src/carma2/newcommon',
            'src/carma2/include', 'src/carma2/hooks', 'src/brender/include', 'src/carma2/s3/include']
subprocess.run([args.cc, '-std=c99', '-O2', '-Wall', '-Werror', '-DDIRECTINPUT_VERSION=0x0500',
                *[option for path in includes for option in ['-I', str(ROOT / path)]],
                str(out / 'init.c'), '-o', str(out / 'init.exe')], check=True)


def wr(u, a, v):
    u.mem_write(a, struct.pack('<I', v & 0xffffffff))


def rd(u, a):
    return struct.unpack('<I', u.mem_read(a, 4))[0]


def ret(u, value=0, extra=0):
    sp = u.reg_read(UC_X86_REG_ESP)
    target = rd(u, sp)
    u.reg_write(UC_X86_REG_EAX, value & 0xffffffff)
    u.reg_write(UC_X86_REG_ESP, sp + 4 + extra)
    u.reg_write(UC_X86_REG_EIP, target)


def cs(u, a):
    s = bytearray()
    while True:
        b = u.mem_read(a + len(s), 1)[0]
        if not b:
            return bytes(s)
        s.append(b)


def original(cfg):
    u = Uc(UC_ARCH_X86, UC_MODE_32)
    u.mem_map(0x400000, 0x400000)
    u.mem_map(0x1000000, 0x100000)
    for i in range(nsects):
        _, va, rs, rp = struct.unpack_from('<IIII', binary, sect + 40*i + 8)
        u.mem_write(0x400000 + va, binary[rp:rp + rs])
    handle, devbase, methods, stop = 0x1010000, 0x1020000, 0x1060000, 0x1080000
    wr(u, handle, handle + 0x100)
    wr(u, handle + 0x110, methods + 0x10)
    for i in range(16):
        ptr = devbase + i * 0x100
        wr(u, ptr, devbase + 0x2000)
        wr(u, 0x79e060 + i*4, ptr if cfg[5] & (1 << i) else 0)
    wr(u, devbase + 0x201c, methods + 0x20)
    wr(u, devbase + 0x2058, methods + 0x30)
    wr(u, 0x589098, methods)
    for a, value in [(0x686158, handle if cfg[0] else 0), (0x686160, cfg[1]),
                     (0x686170, cfg[2]), (0x595f88, cfg[3]), (0x68615c, cfg[4])]:
        wr(u, a, value)
    u.mem_write(0x79d9c0, b'\xa5' * (104*16))
    u.mem_write(0x79e0a0, b'\xa5' * (30*4))
    trace = []

    def hook(uc, ip, size, data):
        sp = uc.reg_read(UC_X86_REG_ESP)
        if ip == stop:
            uc.emu_stop()
        elif ip == methods:
            assert rd(uc, sp + 4) == 0
            trace.append('module')
            ret(uc, 0x1234, 4)
        elif ip == 0x54d03c:
            assert [rd(uc, sp + n) for n in (4, 8, 12, 16)] == [0x1234, 0x500, 0x686158, 0]
            trace.append('create')
            if cfg[8]:
                wr(uc, 0x686158, handle)
            ret(uc, 0x80004005, 16)
        elif ip == methods + 0x10:
            assert [rd(uc, sp + n) for n in (4, 8, 12, 16, 20)] == [handle, 4, 0x459c70, handle, 1]
            trace.append('enum')
            wr(uc, 0x686160, cfg[6])
            for i in range(16):
                if cfg[7] & (1 << i):
                    wr(uc, 0x79e060 + i*4, devbase + i*0x100)
            ret(uc, 0x80004005, 20)
        elif ip == 0x4589d0:
            i = uc.reg_read(UC_X86_REG_ECX)
            assert uc.reg_read(UC_X86_REG_EDX) == 0 and rd(uc, sp + 4) == 0
            trace.append('info:' + str(i))
            if i == cfg[11]:
                wr(uc, 0x686158, 0)
            ret(uc, 0x79d9c0 + i*104, 4)
        elif ip == methods + 0x20:
            ptr = rd(uc, sp + 4)
            i = (ptr - devbase) // 0x100
            assert ptr == devbase + i*0x100 and 0 <= i < 16
            trace.append('acquire:' + str(i))
            ret(uc, [0, 1, 0x80070057, 0x80070005, 0x80004005][i % 5 if cfg[12] == 5 else cfg[12]], 4)
        elif ip == methods + 0x30:
            ptr, command = rd(uc, sp + 4), rd(uc, sp + 8)
            i = (ptr - devbase) // 0x100
            assert ptr == devbase + i*0x100 and 0 <= i < 16
            trace.append('command:' + str(i) + ':' + str(command))
            ret(uc, 0x80004005, 8)
        elif ip == 0x44c590:
            trace.append('log:' + cs(uc, rd(uc, sp + 4)).hex() + ':' + str(rd(uc, sp + 8)))
            ret(uc)
        elif ip == 0x457760:
            trace.append('ff-init')
            if cfg[10] >= 0:
                wr(uc, 0x68615c, cfg[10])
            ret(uc, cfg[9])
        elif ip == 0x458520:
            trace.append('register')
            ret(uc)

    u.hook_add(UC_HOOK_CODE, hook)
    wr(u, 0x103ff00, stop)
    u.reg_write(UC_X86_REG_ESP, 0x103ff00)
    u.emu_start(0x4575b0, stop, count=100000)
    present = sum((1 << i) for i in range(16) if rd(u, 0x79e060 + i*4))
    index = rd(u, 0x595f88)
    index = index - 0x100000000 if index >= 0x80000000 else index
    values = [int(rd(u, 0x686158) != 0), rd(u, 0x686160), rd(u, 0x686170),
              index, rd(u, 0x68615c), present,
              sum(rd(u, 0x79e0a0 + i*4) == 0 for i in range(30))]
    return ' '.join(map(str, values)) + '\n' + ','.join(trace) + (',' if trace else '') + '\n' + bytes(u.mem_read(0x79d9c0, 104*16)).hex() + '\n'


# handle, count, enumerated, selected index, initial FFB mask, device mask,
# enumerated count/mask, create writes handle, FF result/mask (-1 retains),
# GetInfo clears handle at this index (-1 disables), Acquire HRESULT selector.
base = [0, 0, 9, -1, 65535, 65535, 0, 0, 1, 0, -1, -1, 5]
cases = []


def add(name, **changes):
    keys = ['handle', 'count', 'enumerated', 'index', 'ffb', 'devices', 'enum_count',
            'enum_devices', 'create', 'ff_result', 'ff_mask', 'clear_handle_at', 'acquire']
    cfg = base.copy()
    for key, value in changes.items():
        cfg[keys.index(key)] = value
    cases.append((name, cfg))


add('create-fails', create=0)
add('create-fails-selected', create=0, index=7)
add('empty-new')
add('empty-selected', index=7, ff_result=1, ff_mask=65535)
add('all-new', enum_count=16, enum_devices=65535)
add('all-new-ff', enum_count=16, enum_devices=65535, ff_result=1, ff_mask=65535)
add('sparse-without-slot-zero', enum_count=2, enum_devices=(1 << 5) | (1 << 15), ff_result=1, ff_mask=65535)
add('slot-fifteen', enum_count=1, enum_devices=1 << 15, ff_result=1, ff_mask=1 << 15)
for hresult in range(5):
    add('acquire-result-' + str(hresult), enum_count=16, enum_devices=65535, acquire=hresult)
for index in (0, 7, 15):
    add('handle-lost-at-' + str(index), enum_count=16, enum_devices=65535, clear_handle_at=index, ff_result=1, ff_mask=65535)
add('existing-not-enumerated', handle=1, enumerated=0, enum_count=2, enum_devices=5, ff_result=1, ff_mask=5)
add('existing-enumerated', handle=1, enumerated=1)
add('existing-enumerated-reset', handle=1, enumerated=1, index=0)
add('existing-empty', handle=1, enumerated=0, devices=0, ffb=0)
for mask in (0, 1, 2, 32768, 65535, 0xaaaa):
    add('already-counted-reset-' + str(mask), handle=1, count=3, ffb=mask)
add('already-counted-null-handle', count=3)
add('already-counted-sparse-reset', handle=1, count=3, devices=0x8001, ffb=65535)
add('existing-negative-selected', handle=1, enumerated=0, devices=0, index=-2, ffb=0)
add('ff-false-still-final-reset', enum_count=16, enum_devices=65535, ff_mask=0xaaaa)
add('ff-true-no-capable-devices', enum_count=16, enum_devices=65535, ff_result=1, ff_mask=0)
results = []
for name, cfg in cases:
    native = subprocess.check_output([str(out / 'init.exe'), *map(str, cfg)], text=True)
    expected = original(cfg)
    assert native == expected, (name, native, expected)
    results.append({'name': name, 'config': cfg, 'state': expected.splitlines()[0], 'trace': expected.splitlines()[1]})
(out / 'results.json').write_text(json.dumps(results, indent=2))
print(f'PASS: {len(cases)} original/native initialization cases: all 16 slots, partial record clearing, all 30 effects, create/enumerate failures, acquisition HRESULTs, handle loss, selection and STOPALL/RESET order.')
