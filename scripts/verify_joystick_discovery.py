"""Original/native COM tests for joystick object discovery and enumeration.

Requires Python 3.10+, Unicorn and runnable i686 GCC. Compiles actual repository
C bodies with repository types and SDK COM signatures. Fake COM objects remain
addressable after Release so original release/use and double-release paths can
be recorded; this does not make those original paths safe on real hardware.
Run with --dependency-dir build-audit/joystick/python-deps when necessary.
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
parser.add_argument('--output-dir', type=pathlib.Path, default=ROOT / 'build-audit/joystick-discovery')
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


def extract(name, signature=' C2_HOOK_FASTCALL '):
    start = source.index(signature + name + '(')
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
#include "rec2_macros.h"
#define MAX_COUNT_JOYSTICKS 16
#define MAX_COUNT_EFFECTS 30
typedef char check_layout[sizeof(void*)==4&&sizeof(tDirectInputJoystickInfo)==104&&offsetof(tDirectInputJoystickInfo,productName)==12&&offsetof(tDirectInputJoystickInfo,count_buttons)==92&&offsetof(tDirectInputJoystickInfo,sizeData)==96&&offsetof(tDirectInputJoystickInfo,data)==100&&sizeof(DIDEVCAPS)==44&&sizeof(DIDEVICEOBJECTINSTANCEA)==316&&offsetof(DIDEVICEINSTANCEA,dwDevType)==36&&offsetof(DIDEVICEINSTANCEA,tszProductName)==300?1:-1];
IDirectInputA *gDirectInputJoystickHandle,handle;
IDirectInputDeviceA base_device;
IDirectInputDevice2A *gDirectInputJoystickDevices[16], device[3];
IDirectInputEffect *gDirectInputEffects[30];
tDirectInputJoystickInfo gDirectInputJoystickInfos[16];
int gJoystickFFB,gCountEnumeratedJoystickDinputDevices,gDirectInputJoystickEnumerated,gJoystick_index;
void *gHWnd=(void*)0x123456;
int mode,cfg[15],axis_query,property_calls;
char trace[32768];
void note(const char*f,...){va_list a;va_start(a,f);vsprintf(trace+strlen(trace),f,a);va_end(a);strcat(trace,",");}
void dr_dprintf(const char*f,...){const unsigned char*p=(const unsigned char*)f;va_list a;strcat(trace,"log:");for(;*p;p++)sprintf(trace+strlen(trace),"%02x",*p);if(strstr(f,"%d")){va_start(a,f);note(":%d",va_arg(a,int));va_end(a);}else strcat(trace,",");}
HINSTANCE WINAPI fake_module(LPCSTR name){if(name)exit(80);note("module");return (HINSTANCE)0x1234;}
HRESULT WINAPI fake_create(HINSTANCE module,DWORD version,LPDIRECTINPUTA*out,LPUNKNOWN outer){if(module!=(HINSTANCE)0x1234||version!=0x500||out!=&gDirectInputJoystickHandle||outer)exit(81);note("init-create");if(cfg[13])gDirectInputJoystickHandle=&handle;return DIERR_GENERIC;}
HRESULT WINAPI acquire(IDirectInputDevice2A*self){int id=self-device;if(id<0||id>=3)exit(82);note("acquire:%d",id);return cfg[9];}
HRESULT WINAPI caps(IDirectInputDevice2A*self,LPDIDEVCAPS caps){note("caps:%d",self-device);if(caps->dwSize!=44)exit(83);caps->dwButtons=cfg[5];caps->dwAxes=cfg[6];caps->dwFlags=cfg[7];return cfg[8];}
HRESULT WINAPI object(IDirectInputDevice2A*self,LPDIDEVICEOBJECTINSTANCEA desc,DWORD offset,DWORD how){int success;if(desc->dwSize!=316||how!=1)exit(84);note("object:%d:%lu",self-device,(unsigned long)offset);if(offset>=48&&offset<80)success=((unsigned)cfg[10]>>(offset-48))&1;else success=((unsigned)cfg[11]>>axis_query++)&1;return success?DI_OK:DIERR_OBJECTNOTFOUND;}
ULONG WINAPI release(IDirectInputDeviceA*self){if(self!=&base_device)exit(85);note("release:base");return 0;}
HRESULT WINAPI format(IDirectInputDeviceA*self,LPCDIDATAFORMAT data){if(self!=&base_device||data!=&c_dfDIJoystick)exit(86);note("format");return cfg[0]==2?DIERR_GENERIC:DI_OK;}
HRESULT WINAPI query(IDirectInputDeviceA*self,REFIID iid,void**output){if(self!=&base_device||memcmp(iid,&IID_IDirectInputDevice2A,16)||output!= (void**)&gDirectInputJoystickDevices[gCountEnumeratedJoystickDinputDevices])exit(87);note("query");if(cfg[0]!=4)*output=&device[0];return cfg[0]==3?DIERR_GENERIC:DI_OK;}
HRESULT WINAPI coop(IDirectInputDevice2A*self,HWND window,DWORD flags){if(self!=&device[0]||window!=(HWND)gHWnd||flags!=9)exit(88);note("coop");return cfg[0]==5?DIERR_GENERIC:DI_OK;}
HRESULT WINAPI property(IDirectInputDeviceA*self,REFGUID prop,LPCDIPROPHEADER header){const DWORD*p=(const DWORD*)header;int stage;if(self!=&base_device)exit(89);if((uintptr_t)prop==4){if(header->dwSize!=24)exit(90);note("property:4:%lu:%lu:%lu:%lu:%lu:%lu",(unsigned long)p[0],(unsigned long)p[1],(unsigned long)p[2],(unsigned long)p[3],(unsigned long)p[4],(unsigned long)p[5]);stage=header->dwObj==0?6:7;}else{if((uintptr_t)prop!=5||header->dwSize!=20)exit(91);note("property:5:%lu:%lu:%lu:%lu:%lu",(unsigned long)p[0],(unsigned long)p[1],(unsigned long)p[2],(unsigned long)p[3],(unsigned long)p[4]);stage=header->dwObj==0?8:9;}property_calls++;return cfg[0]==stage||cfg[0]==10?DIERR_GENERIC:DI_OK;}
HRESULT WINAPI create_device(IDirectInputA*self,REFGUID guid,LPDIRECTINPUTDEVICEA*output,LPUNKNOWN outer){int i;if(self!=&handle||outer)exit(92);for(i=0;i<16;i++)if(((const unsigned char*)guid)[i]!=i)exit(93);note("device-create");*output=&base_device;return cfg[0]==1?DIERR_GENERIC:DI_OK;}
IDirectInputAVtbl handle_vtable={.CreateDevice=create_device};
IDirectInputDeviceAVtbl base_vtable={.QueryInterface=query,.Release=release,.SetDataFormat=format,.SetProperty=property};
IDirectInputDevice2AVtbl device_vtable={.Acquire=acquire,.GetCapabilities=caps,.GetObjectInfo=object,.SetCooperativeLevel=coop};
#define GetModuleHandleA fake_module
#define DirectInputCreateA fake_create
'''
driver += '\n'.join(extract(name) for name in ['InitDirectInputForInfo', 'AcquireDInputJoystickDevice', 'JoystickDInputGetInfo'])
driver += '\n' + extract('Win32DInputJoystickEnum', ' CALLBACK ')
driver += r'''
int main(int argc,char**argv){int i,result,present=0;unsigned char*p;tU32 buttons=0xbabababa,axes=0xcacacaca;DIDEVICEINSTANCEA instance;
if(argc!=17){return 94;}mode=atoi(argv[1]);for(i=0;i<15;i++)cfg[i]=strtoul(argv[i+2],NULL,10);
handle.lpVtbl=&handle_vtable;base_device.lpVtbl=&base_vtable;for(i=0;i<3;i++)device[i].lpVtbl=&device_vtable;
memset(gDirectInputJoystickInfos,0xa5,sizeof(gDirectInputJoystickInfos));memset(gDirectInputEffects,0xa5,sizeof(gDirectInputEffects));
gDirectInputJoystickHandle=mode||cfg[0]?&handle:NULL;gCountEnumeratedJoystickDinputDevices=mode?cfg[2]:6;gDirectInputJoystickEnumerated=mode?7:cfg[14];gJoystick_index=cfg[2];gJoystickFFB=0x4000;
if(mode==0){gDirectInputJoystickDevices[cfg[2]]=cfg[1]?&device[0]:NULL;gDirectInputJoystickInfos[cfg[2]].buttonMask=cfg[3];gDirectInputJoystickInfos[cfg[2]].axisMask=cfg[4];result=JoystickDInputGetInfo(cfg[2],cfg[12]&1?&buttons:NULL,cfg[12]&2?&axes:NULL)!=NULL;}
else{memset(&instance,0,sizeof(instance));instance.dwSize=sizeof(instance);instance.dwDevType=cfg[1];for(i=0;i<16;i++)((unsigned char*)&instance.guidInstance)[i]=i;for(i=0;i<cfg[3];i++)instance.tszProductName[i]='A'+i%26;result=Win32DInputJoystickEnum(&instance,&handle);}
for(i=0;i<16;i++)if(gDirectInputJoystickDevices[i])present|=1<<i;
printf("%d %d %d %d %d %u %u %d\n%s\n",result,gDirectInputJoystickHandle!=NULL,gCountEnumeratedJoystickDinputDevices,gDirectInputJoystickEnumerated,gJoystickFFB,buttons,axes,present,trace);
p=(unsigned char*)gDirectInputJoystickInfos;for(i=0;i<sizeof(gDirectInputJoystickInfos);i++)printf("%02x",p[i]);puts("");return 0;}
'''
(out / 'discovery.c').write_text(driver)
includes = ['src/carma2/platform/win32', 'src/carma2/platform', 'src/carma2/newcommon',
            'src/carma2/include', 'src/carma2/hooks', 'src/brender/include', 'src/carma2/s3/include']
executables = []
for abi in ('native', 'fastcall'):
    executable = out / ('discovery-' + abi + '.exe')
    subprocess.run([args.cc, '-std=c99', '-O2', '-Wall', '-Werror', '-DDIRECTINPUT_VERSION=0x0500',
                    *(['-DREC2_MATCHING'] if abi == 'fastcall' else []),
                    *[option for path in includes for option in ['-I', str(ROOT / path)]],
                    str(out / 'discovery.c'), '-ldxguid', '-ldinput', '-o', str(executable)], check=True)
    executables.append((abi, executable))


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


def original(mode, cfg):
    u = Uc(UC_ARCH_X86, UC_MODE_32)
    u.mem_map(0x400000, 0x400000)
    u.mem_map(0x1000000, 0x100000)
    for i in range(nsects):
        _, va, rs, rp = struct.unpack_from('<IIII', binary, sect + 40*i + 8)
        u.mem_write(0x400000 + va, binary[rp:rp + rs])
    handle, base, device, methods, stop = 0x1010000, 0x1020000, 0x1021000, 0x1060000, 0x1080000
    wr(u, handle, handle + 0x100)
    wr(u, handle + 0x10c, methods + 0x10)
    wr(u, base, base + 0x100)
    for offset, method in [(0, 0x20), (8, 0x30), (0x2c, 0x40), (0x18, 0x50)]:
        wr(u, base + 0x100 + offset, methods + method)
    wr(u, device, device + 0x100)
    for offset, method in [(0x34, 0x60), (0x1c, 0x70), (0xc, 0x80), (0x38, 0x90)]:
        wr(u, device + 0x100 + offset, methods + method)
    wr(u, 0x589098, methods)
    wr(u, 0x686158, handle if mode or cfg[0] else 0)
    wr(u, 0x686160, cfg[2] if mode else 6)
    wr(u, 0x686170, 7 if mode else cfg[14])
    wr(u, 0x595f88, cfg[2])
    wr(u, 0x68615c, 0x4000)
    wr(u, 0x6ad4c8, 0x123456)
    u.mem_write(0x79d9c0, b'\xa5' * (104*16))
    u.mem_write(0x79e0a0, b'\xa5' * (30*4))
    if not mode:
        wr(u, 0x79e060 + cfg[2]*4, device if cfg[1] else 0)
        wr(u, 0x79d9c0 + cfg[2]*104, cfg[3])
        wr(u, 0x79d9c4 + cfg[2]*104, cfg[4])
    instance, buttons, axes = 0x1040000, 0x1041000, 0x1041010
    wr(u, buttons, 0xbabababa)
    wr(u, axes, 0xcacacaca)
    if mode:
        wr(u, instance, 580)
        u.mem_write(instance + 4, bytes(range(16)))
        wr(u, instance + 36, cfg[1])
        u.mem_write(instance + 300, bytes(65 + i % 26 for i in range(cfg[3])) + b'\0')
    trace = []
    axis_query = 0

    def hook(uc, ip, size, data):
        nonlocal axis_query
        sp = uc.reg_read(UC_X86_REG_ESP)
        if ip == stop:
            uc.emu_stop()
        elif ip == methods:
            assert rd(uc, sp + 4) == 0
            trace.append('module')
            ret(uc, 0x1234, 4)
        elif ip == 0x54d03c:
            assert [rd(uc, sp + n) for n in (4, 8, 12, 16)] == [0x1234, 0x500, 0x686158, 0]
            trace.append('init-create')
            if cfg[13]:
                wr(uc, 0x686158, handle)
            ret(uc, 0x80004005, 16)
        elif ip == 0x5766d0:
            dst, src, n = [rd(uc, sp + x) for x in (4, 8, 12)]
            assert n == 80
            data = cs(uc, src)
            uc.mem_write(dst, data[:n].ljust(n, b'\0'))
            ret(uc, dst)
        elif ip == methods + 0x10:
            assert rd(uc, sp + 4) == handle and rd(uc, sp + 16) == 0
            assert bytes(uc.mem_read(rd(uc, sp + 8), 16)) == bytes(range(16))
            trace.append('device-create')
            wr(uc, rd(uc, sp + 12), base)
            ret(uc, 0x80004005 if cfg[0] == 1 else 0, 16)
        elif ip == methods + 0x20:
            assert rd(uc, sp + 4) == base
            assert bytes(uc.mem_read(rd(uc, sp + 8), 16)) == bytes(uc.mem_read(0x58b398, 16))
            output = rd(uc, sp + 12)
            assert output == 0x79e060 + rd(uc, 0x686160)*4
            trace.append('query')
            if cfg[0] != 4:
                wr(uc, output, device)
            ret(uc, 0x80004005 if cfg[0] == 3 else 0, 12)
        elif ip == methods + 0x30:
            assert rd(uc, sp + 4) == base
            trace.append('release:base')
            ret(uc, 0, 4)
        elif ip == methods + 0x40:
            assert [rd(uc, sp + x) for x in (4, 8)] == [base, 0x54d310]
            trace.append('format')
            ret(uc, 0x80004005 if cfg[0] == 2 else 0, 8)
        elif ip == methods + 0x50:
            assert rd(uc, sp + 4) == base
            prop, ptr = rd(uc, sp + 8), rd(uc, sp + 12)
            n = 6 if prop == 4 else 5
            values = [rd(uc, ptr + i*4) for i in range(n)]
            assert prop in (4, 5) and values[0] == n*4
            trace.append('property:' + str(prop) + ':' + ':'.join(map(str, values)))
            stage = (6 if values[2] == 0 else 7) if prop == 4 else (8 if values[2] == 0 else 9)
            ret(uc, 0x80004005 if cfg[0] in (stage, 10) else 0, 12)
        elif ip == methods + 0x60:
            assert [rd(uc, sp + x) for x in (4, 8, 12)] == [device, 0x123456, 9]
            trace.append('coop')
            ret(uc, 0x80004005 if cfg[0] == 5 else 0, 12)
        elif ip == methods + 0x70:
            assert rd(uc, sp + 4) == device
            trace.append('acquire:0')
            ret(uc, cfg[9], 4)
        elif ip == methods + 0x80:
            assert rd(uc, sp + 4) == device
            ptr = rd(uc, sp + 8)
            assert rd(uc, ptr) == 44
            trace.append('caps:0')
            for offset, value in [(4, cfg[7]), (12, cfg[6]), (16, cfg[5])]:
                wr(uc, ptr + offset, value)
            ret(uc, cfg[8], 8)
        elif ip == methods + 0x90:
            assert rd(uc, sp + 4) == device
            ptr, offset, how = [rd(uc, sp + x) for x in (8, 12, 16)]
            assert rd(uc, ptr) == 316 and how == 1
            trace.append('object:0:' + str(offset))
            if 48 <= offset < 80:
                success = (cfg[10] >> (offset - 48)) & 1
            else:
                success = (cfg[11] >> axis_query) & 1
                axis_query += 1
            ret(uc, 0 if success else 0x80070002, 16)
        elif ip == 0x44c590:
            f = cs(uc, rd(uc, sp + 4))
            trace.append('log:' + f.hex() + (':' + str(rd(uc, sp + 8)) if b'%d' in f else ''))
            ret(uc)

    u.hook_add(UC_HOOK_CODE, hook)
    wr(u, 0x103ff00, stop)
    u.reg_write(UC_X86_REG_ESP, 0x103ff00)
    if mode:
        wr(u, 0x103ff04, instance)
        wr(u, 0x103ff08, handle)
    else:
        u.reg_write(UC_X86_REG_ECX, cfg[2])
        u.reg_write(UC_X86_REG_EDX, buttons if cfg[12] & 1 else 0)
        wr(u, 0x103ff04, axes if cfg[12] & 2 else 0)
    u.emu_start(0x459c70 if mode else 0x4589d0, stop, count=100000)
    result = u.reg_read(UC_X86_REG_EAX)
    if not mode:
        assert result in (0, 0x79d9c0 + cfg[2]*104)
        result = int(result != 0)
    present = sum(1 << i for i in range(16) if rd(u, 0x79e060 + i*4))
    values = [result, int(rd(u, 0x686158) != 0), rd(u, 0x686160), rd(u, 0x686170),
              rd(u, 0x68615c), rd(u, buttons), rd(u, axes), present]
    return ' '.join(map(str, values)) + '\n' + ','.join(trace) + (',' if trace else '') + '\n' + bytes(u.mem_read(0x79d9c0, 104*16)).hex() + '\n'


get_base = [1, 1, 0, 0, 0, 3, 3, 0x100, 0, 0, 0xffffffff, 0xff, 3, 1, 9]
cases = []


def get(name, **changes):
    keys = ['handle', 'device', 'index', 'buttons', 'axes', 'caps_buttons', 'caps_axes',
            'caps_flags', 'caps_hr', 'acquire_hr', 'button_success', 'axis_success',
            'outputs', 'create', 'enumerated']
    cfg = get_base.copy()
    for key, value in changes.items():
        cfg[keys.index(key)] = value
    cases.append((name, 0, cfg))


get('ordinary')
get('missing-device', device=0)
get('create-fails', handle=0, create=0)
get('create-without-device', handle=0)
get('caps-fails', caps_hr=0x80004005)
get('caps-noeffect', caps_hr=1)
get('no-ff', caps_flags=0)
get('cached-buttons', buttons=0x80000001)
get('cached-axes', axes=0x81)
get('cached-both', buttons=0x1234, axes=0x36)
get('sparse-buttons', caps_buttons=2, button_success=0x80000040)
get('sparse-axes', caps_axes=2, axis_success=0xc0)
get('none-found', button_success=0, axis_success=0)
get('all-bounds', caps_buttons=99, caps_axes=99)
get('empty', caps_buttons=0, caps_axes=0)
get('only-buttons', caps_axes=0)
get('only-axes', caps_buttons=0)
get('slot-fifteen', index=15, button_success=0xaaaaaaaa, axis_success=0xaa)
for outputs in range(4):
    get('output-presence-' + str(outputs), outputs=outputs, button_success=0x92, axis_success=0xa4)
for hresult in (0, 1, 0x80070057, 0x80070005, 0x80004005):
    get('acquire-result-' + str(hresult), acquire_hr=hresult)
for failure in range(11):
    cases.append(('enum-stage-' + str(failure), 1, [failure, 0x404, 0, 90] + [0]*11))
for subtype in (0, 4, 6, 255):
    for index in (0, 15):
        cases.append((f'enum-subtype-{subtype}-slot-{index}', 1, [0, (subtype << 8) | 4, index, 260] + [0]*11))
for length in (0, 3, 79, 80):
    cases.append(('enum-name-length-' + str(length), 1, [0, 0x604, 7, length] + [0]*11))
results = []
for name, mode, cfg in cases:
    expected = original(mode, cfg)
    for abi, executable in executables:
        native = subprocess.check_output([str(executable), str(mode), *map(str, cfg)], text=True)
        assert native == expected, (name, abi, native, expected)
    results.append({'name': name, 'mode': mode, 'config': cfg, 'state': expected.splitlines()[0], 'trace': expected.splitlines()[1]})
(out / 'results.json').write_text(json.dumps(results, indent=2))
print(f'PASS: {sum(mode == 0 for _, mode, _ in cases)} GetInfo and {sum(mode == 1 for _, mode, _ in cases)} enumeration cases against both normal and matching-fastcall C builds; exact records, masks, query offsets/counting, outputs, property arguments, diagnostics and release order.')
