"""Compare actual C force-feedback creation, IFR setup and playback with x86.

Requires Python 3.10+, Unicorn and runnable i686 GCC. Uses actual repository
types and SDK-typed COM/IFR interfaces. IFR loader/name parser are controlled
dependencies; file-format parsing and the bootstrap path are excluded here.
All 30 persistent effect records, COM arguments, names, ignored failures and
call order are compared. Run --dependency-dir build-audit/joystick/python-deps.
"""
import argparse
import hashlib
import json
import pathlib
import struct
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parents[1]
p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
p.add_argument('--original', type=pathlib.Path, default=ROOT/'build-reference/originals/CARMA2_HW.EXE')
p.add_argument('--cc', default=str(ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'))
p.add_argument('--dependency-dir', type=pathlib.Path)
p.add_argument('--output-dir', type=pathlib.Path, default=ROOT/'build-audit/joystick-effects')
args = p.parse_args()
if args.dependency_dir:
    sys.path.insert(0, str(args.dependency_dir.resolve()))
from unicorn import Uc, UcError, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX, UC_X86_REG_ESP, UC_X86_REG_EIP

binary = args.original.read_bytes()
assert hashlib.sha256(binary).hexdigest() == '9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
pe = struct.unpack_from('<I', binary, 0x3c)[0]
sect = pe+24+struct.unpack_from('<H', binary, pe+20)[0]
nsects = struct.unpack_from('<H', binary, pe+6)[0]
source = (ROOT/'src/carma2/platform/win32/win32_dinput.c').read_text()
out = args.output_dir.resolve()
out.mkdir(parents=True, exist_ok=True)


def extract(name):
    start = source.index(' C2_HOOK_FASTCALL '+name+'(')
    start = source.rfind('\n', 0, start)+1
    end = source.index('{', start)+1
    depth = 1
    while depth:
        depth += (source[end] == '{')-(source[end] == '}')
        end += 1
    return source[start:end]


driver = r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include "win32_dinput.h"
#include "41-utility.h"
#include "rec2_macros.h"
#include <iforce2.h>
#define MAX_COUNT_JOYSTICKS 16
#define MAX_COUNT_EFFECTS 30
typedef char check_layout[sizeof(void*)==4&&sizeof(DIEFFECT)==52&&sizeof(tJoystick_force_effect)==232&&offsetof(tJoystick_force_effect,type_specific_params)==52&&offsetof(tJoystick_force_effect,field_0x60)==96&&offsetof(tJoystick_force_effect,name)==104&&sizeof(tJoystick_effect_description)==80&&sizeof(IFR_Effect)==20?1:-1];
IDirectInputA *gDirectInputJoystickHandle,handle;
IDirectInputDevice2A *gDirectInputJoystickDevices[16],devices[16];
IDirectInputEffect *gDirectInputEffects[30],effects[30];
tDirectInputJoystickInfo gDirectInputJoystickInfos[16];
tJoystick_force_effect gJoystick_effects[30];
int gCount_joystick_effects,gCountEnumeratedJoystickDinputDevices,gDirectInputJoystickEnumerated,gJoystick_index,gJoystickFFB,gForceFeedbackAvailable;
IFR_Project *gIfr_project,project;
tPath_name gApplication_path="X:/DATA/";
int mode,cfg[13],row,status;
char trace[65536];
DIEFFECT imported[2];IFR_Effect imported_wrapper[2];IFR_Effect* imported_ptrs[2];
void dump(int result);
void note(const char*f,...){va_list a;va_start(a,f);vsprintf(trace+strlen(trace),f,a);va_end(a);strcat(trace,",");}
void dr_dprintf(const char*f,...){const unsigned char*p=(const unsigned char*)f;strcat(trace,"log:");for(;*p;p++)sprintf(trace+strlen(trace),"%02x",*p);strcat(trace,",");}
unsigned norm(unsigned value){unsigned start=(unsigned)(uintptr_t)gJoystick_effects;return value>=start&&value<start+sizeof(gJoystick_effects)?value-start+0x684628:value;}
void normalized_record(const tJoystick_force_effect*record,unsigned char*bytes){DIEFFECT*di;memcpy(bytes,record,232);di=(DIEFFECT*)bytes;di->rglDirection=(LONG*)(uintptr_t)norm((unsigned)(uintptr_t)di->rglDirection);di->lpvTypeSpecificParams=(void*)(uintptr_t)norm((unsigned)(uintptr_t)di->lpvTypeSpecificParams);if(mode==0&&di->dwSize==52)di->rgdwAxes=(DWORD*)0x684620;}
void record_hex(const tJoystick_force_effect*record,char*text){unsigned char bytes[232];int i;normalized_record(record,bytes);for(i=0;i<232;i++)sprintf(text+2*i,"%02x",bytes[i]);}
HRESULT WINAPI create(IDirectInputDevice2A*self,REFGUID guid,LPCDIEFFECT di,LPDIRECTINPUTEFFECT*output,LPUNKNOWN outer){int id=(const tJoystick_force_effect*)di-gJoystick_effects,i;char hex[465];if(id<0||id>=30||outer||output!=&gDirectInputEffects[gCount_joystick_effects])exit(80);record_hex(&gJoystick_effects[id],hex);note("create:%d:%d:%d:%s",self-devices,id,gCount_joystick_effects,hex);strcat(trace,"guid:");for(i=0;i<16;i++)sprintf(trace+strlen(trace),"%02x",((const unsigned char*)guid)[i]);strcat(trace,",");if(mode==0)note("axes:%lu:%lu",(unsigned long)di->rgdwAxes[0],(unsigned long)di->rgdwAxes[1]);if(mode!=1||!cfg[9])*output=&effects[gCount_joystick_effects];if(mode==0&&cfg[12]>=0)gCount_joystick_effects=cfg[12];return mode==0?cfg[11]:cfg[8];}
HRESULT WINAPI command(IDirectInputDevice2A*self,DWORD command){note("command:%d:%lu:%d",self-devices,(unsigned long)command,gForceFeedbackAvailable);return DIERR_GENERIC;}
HRESULT WINAPI start(IDirectInputEffect*self,DWORD iterations,DWORD flags){note("start:%d:%lu:%lu",self-effects,(unsigned long)iterations,(unsigned long)flags);return DIERR_GENERIC;}
HRESULT WINAPI unload(IDirectInputEffect*self){note("unload:%d",self-effects);return DIERR_GENERIC;}
IDirectInputDevice2AVtbl device_vtable={.CreateEffect=create,.SendForceFeedbackCommand=command};
IDirectInputEffectVtbl effect_vtable={.Start=start,.Unload=unload};
const char* effect_name(int i){return cfg[10]?"same":i?"Bump":"Pulse";}
void C2_HOOK_FASTCALL PathCat(char*d,const char*a,const char*b){if(strcmp(a,"X:/DATA/")||strcmp(b,"FFB.IFR"))exit(81);note("path");strcpy(d,a);strcat(d,b);}
void C2_NORETURN C2_HOOK_FASTCALL PDFatalError(const char*s){if(strcmp(s,"Cant enable I-Force"))exit(82);note("fatal");status=1;dump(0);exit(0);}
IFR_Project* IFR_DECL fake_load(const char*path,IDirectInputDevice2A*device){if(strcmp(path,"X:/DATA/FFB.IFR"))exit(83);note("load:%d",device?device-devices:-1);return cfg[6]?NULL:&project;}
IFR_Effect** IFR_DECL fake_structs(IFR_Project*p,const char*name,int*count){if(p!=&project||strcmp(name,effect_name(row)))exit(84);note("structs:%s",name);*count=cfg[11];if(cfg[7])return NULL;return &imported_ptrs[row++];}
int IFR_DECL fake_release(IFR_Project*p){if(p!=&project)exit(85);note("release-project");return -1;}
int C2_HOOK_FASTCALL ReadIforceEffectsNames(const char*path,char**names,size_t capacity){int i;if(strcmp(path,"X:/DATA/FFB.IFR")||capacity!=20)exit(86);note("names:%d",cfg[5]);for(i=0;i<cfg[5];i++)names[i]=(char*)effect_name(i);return cfg[5];}
HINSTANCE WINAPI fake_module(LPCSTR s){exit(87);return NULL;}
HRESULT WINAPI fake_directinput(HINSTANCE a,DWORD b,LPDIRECTINPUTA*c,LPUNKNOWN d){exit(88);return 0;}
tDirectInputJoystickInfo* C2_HOOK_FASTCALL JoystickDInputGetInfo(int a,tU32*b,tU32*c){exit(89);return NULL;}
BOOL CALLBACK Win32DInputJoystickEnum(LPCDIDEVICEINSTANCEA a,LPVOID b){exit(90);return 0;}
void C2_HOOK_FASTCALL RegisterJoystickFFBForces(void){exit(91);}
#define GetModuleHandleA fake_module
#define DirectInputCreateA fake_directinput
#define IFLoadProjectFile fake_load
#define IFCreateEffectStructs fake_structs
#define IFReleaseProject fake_release
'''
guid_start = source.index('const GUID* gJoystick_effect_guids')
guid_end = source.index('};', guid_start)+2
driver += source[guid_start:guid_end]+'\n'
driver += '\n'.join(extract(n) for n in ['InitDirectInput','AcquireDInputJoystickDevice','ResetForceFeedback','ResetDInputJoystickFFB','JoystickDInputBegin','UnloadDinputFFBEffectAtIndex','UnloadDinputFFBEfectwithName','CreateDinputEffect','InitForceFeedback','PDPlayFFBEffectIndex','PDFindJoystickEffect','PDPlayFFBEffect'])
driver += r'''
void dump(int result){int i;char hex[465];DWORD*axis=NULL;printf("%d %d %d %d %d\n%s\n",status,result,gCount_joystick_effects,gForceFeedbackAvailable,gIfr_project!=NULL,trace);for(i=0;i<30;i++){record_hex(&gJoystick_effects[i],hex);printf("%s",hex);}puts("");for(i=0;i<30;i++)printf("%s%d",i?",":"",gDirectInputEffects[i]?(int)(gDirectInputEffects[i]-effects):-1);puts("");if(mode==0)for(i=0;i<30;i++)if(gJoystick_effects[i].di_effect.dwSize==52){DWORD*p=gJoystick_effects[i].di_effect.rgdwAxes;if(!axis)axis=p;printf("%d:%lu:%lu:%d,",i,(unsigned long)p[0],(unsigned long)p[1],p==axis);}puts("");}
LONG WINAPI fault(EXCEPTION_POINTERS*p){status=2;dump(0);fflush(stdout);ExitProcess(0);return EXCEPTION_EXECUTE_HANDLER;}
int main(int argc,char**argv){int i,j,result=0;tJoystick_effect_description description;
if(argc!=15){return 92;}mode=atoi(argv[1]);for(i=0;i<13;i++)cfg[i]=strtoul(argv[i+2],NULL,10);
SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);SetUnhandledExceptionFilter(fault);
gDirectInputJoystickHandle=&handle;gCountEnumeratedJoystickDinputDevices=1;gDirectInputJoystickEnumerated=1;gJoystick_index=2;
for(i=0;i<16;i++){devices[i].lpVtbl=&device_vtable;gDirectInputJoystickDevices[i]=&devices[i];}
for(i=0;i<30;i++)effects[i].lpVtbl=&effect_vtable;
memset(gJoystick_effects,0xa5,sizeof(gJoystick_effects));
if(mode==0){gCount_joystick_effects=cfg[9];gJoystickFFB=cfg[7];gDirectInputJoystickDevices[2]=cfg[8]?&devices[2]:NULL;gDirectInputJoystickInfos[2].devSubType=cfg[10];memset(&description,0x5a,sizeof(description));description.type=cfg[0];description.field_0x40=cfg[1];description.time_ms=cfg[2];description.field_0x44=cfg[3];description.field_0x3c=cfg[4];description.field_0x48=cfg[5];description.field_0x4c=cfg[6];result=CreateDinputEffect(2,&description,"Test effect");if(cfg[12]==-2){description.field_0x40=1;result=CreateDinputEffect(2,&description,"Test effect");}}
else if(mode==1){gForceFeedbackAvailable=cfg[0];gJoystickFFB=cfg[1];gJoystick_index=cfg[3];gCount_joystick_effects=cfg[4];if(cfg[12])gCountEnumeratedJoystickDinputDevices=0;for(i=0;i<16;i++)gDirectInputJoystickDevices[i]=cfg[2]&(1<<i)?&devices[i]:NULL;for(i=0;i<cfg[4];i++){sprintf(gJoystick_effects[i].name,cfg[10]?"same":"old%d",i);gDirectInputEffects[i]=&effects[i];}for(i=0;i<2;i++){for(j=0;j<13;j++)((DWORD*)&imported[i])[j]=0x11000000+i*0x100+j;for(j=0;j<16;j++)((unsigned char*)&imported_wrapper[i].guid)[j]=j+i*16;imported_wrapper[i].di_effect=&imported[i];imported_ptrs[i]=&imported_wrapper[i];}result=InitForceFeedback();}
else{gCount_joystick_effects=cfg[1];gForceFeedbackAvailable=cfg[0];for(i=0;i<cfg[1];i++){strcpy(gJoystick_effects[i].name,i==0||cfg[3]?"same":"other");gDirectInputEffects[i]=cfg[2]&(1<<i)?&effects[i]:NULL;}if(mode==2)PDPlayFFBEffectIndex(cfg[4]);else PDPlayFFBEffect(cfg[4]?"missing":"same");}
dump(result);return 0;}
'''
(out/'effects.c').write_text(driver)
includes = ['src/carma2/platform/win32','src/carma2/platform','src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/brender/include','src/carma2/s3/include','external/iforce2/include']
executables = []
for abi in ('native','fastcall'):
    executable = out/('effects-'+abi+'.exe')
    subprocess.run([args.cc,'-std=c99','-O2','-Wall','-Werror','-DDIRECTINPUT_VERSION=0x0500',
                    *(['-DREC2_MATCHING'] if abi == 'fastcall' else []),
                    *[x for path in includes for x in ['-I',str(ROOT/path)]],str(out/'effects.c'),
                    '-ldxguid','-o',str(executable)],check=True)
    executables.append((abi,executable))


def wr(u,a,v):
    u.mem_write(a,struct.pack('<I',v&0xffffffff))


def rd(u,a):
    return struct.unpack('<I',u.mem_read(a,4))[0]


def cs(u,a):
    s = bytearray()
    while True:
        b = u.mem_read(a+len(s),1)[0]
        if not b:
            return bytes(s)
        s.append(b)


def ret(u,value=0,extra=0):
    sp = u.reg_read(UC_X86_REG_ESP)
    target = rd(u,sp)
    u.reg_write(UC_X86_REG_EAX,value&0xffffffff)
    u.reg_write(UC_X86_REG_ESP,sp+4+extra)
    u.reg_write(UC_X86_REG_EIP,target)


def original(mode,cfg):
    u = Uc(UC_ARCH_X86,UC_MODE_32)
    u.mem_map(0x400000,0x400000)
    u.mem_map(0x1000000,0x100000)
    for i in range(nsects):
        _,va,rs,rp = struct.unpack_from('<IIII',binary,sect+40*i+8)
        u.mem_write(0x400000+va,binary[rp:rp+rs])
    devbase,effbase,methods,stop,project = 0x1010000,0x1020000,0x1060000,0x1080000,0x1040000
    for i in range(16):
        ptr = devbase+i*0x100
        wr(u,ptr,devbase+0x2000)
        wr(u,0x79e060+i*4,ptr)
    for offset,method in [(0x48,0x10),(0x58,0x20)]:
        wr(u,devbase+0x2000+offset,methods+method)
    for i in range(30):
        wr(u,effbase+i*0x100,effbase+0x4000)
    wr(u,effbase+0x401c,methods+0x30)
    wr(u,effbase+0x402c,methods+0x40)
    for address,method in [(0x589034,0x50),(0x589038,0x60),(0x589030,0x70)]:
        wr(u,address,methods+method)
    wr(u,0x686158,0x1047000)
    wr(u,0x686160,1)
    wr(u,0x686170,1)
    wr(u,0x595f88,2)
    u.mem_write(0x684628,b'\xa5'*(232*30))
    trace = []
    state = {'status':0,'row':0}
    description,name,imported,wrappers,array = 0x1041000,0x1042000,0x1043000,0x1044000,0x1045000
    u.mem_write(name,b'Test effect\0')
    if mode == 0:
        wr(u,0x686164,cfg[9])
        wr(u,0x68615c,cfg[7])
        wr(u,0x79e068,devbase+0x200 if cfg[8] else 0)
        wr(u,0x79d9c8+2*104,cfg[10])
        u.mem_write(description,b'\x5a'*80)
        for offset,value in [(0,cfg[0]),(0x40,cfg[1]),(0x34,cfg[2]),(0x44,cfg[3]),(0x3c,cfg[4]),(0x48,cfg[5]),(0x4c,cfg[6])]:
            wr(u,description+offset,value)
    elif mode == 1:
        wr(u,0x686168,cfg[0])
        wr(u,0x68615c,cfg[1])
        wr(u,0x595f88,cfg[3])
        wr(u,0x686164,cfg[4])
        if cfg[12]:
            wr(u,0x686160,0)
        for i in range(16):
            wr(u,0x79e060+i*4,devbase+i*0x100 if cfg[2]&(1<<i) else 0)
        for i in range(cfg[4]):
            u.mem_write(0x684690+i*232,('same' if cfg[10] else 'old'+str(i)).encode()+b'\0')
            wr(u,0x79e0a0+i*4,effbase+i*0x100)
        u.mem_write(0x75ba60,b'X:/DATA/\0')
        for i in range(2):
            u.mem_write(imported+i*0x100,struct.pack('<13I',*[0x11000000+i*0x100+j for j in range(13)]))
            u.mem_write(wrappers+i*0x100,bytes(j+i*16 for j in range(16)))
            wr(u,wrappers+i*0x100+16,imported+i*0x100)
            wr(u,array+i*4,wrappers+i*0x100)
    else:
        wr(u,0x686164,cfg[1])
        wr(u,0x686168,cfg[0])
        for i in range(cfg[1]):
            u.mem_write(0x684690+i*232,('same' if i == 0 or cfg[3] else 'other').encode()+b'\0')
            wr(u,0x79e0a0+i*4,effbase+i*0x100 if cfg[2]&(1<<i) else 0)
        u.mem_write(name,b'missing\0' if cfg[4] else b'same\0')

    def hook(uc,ip,size,data):
        sp = uc.reg_read(UC_X86_REG_ESP)
        if ip == stop:
            uc.emu_stop()
        elif ip == methods+0x10:
            dev,guid,di,output,outer = [rd(uc,sp+x) for x in (4,8,12,16,20)]
            idx = (di-0x684628)//232
            count = rd(uc,0x686164)
            assert 0 <= idx < 30 and di == 0x684628+idx*232 and output == 0x79e0a0+count*4 and outer == 0
            trace.append(f'create:{(dev-devbase)//0x100}:{idx}:{count}:'+bytes(uc.mem_read(di,232)).hex())
            trace.append('guid:'+bytes(uc.mem_read(guid,16)).hex())
            if mode == 0:
                ptr = rd(uc,di+32)
                trace.append(f'axes:{rd(uc,ptr)}:{rd(uc,ptr+4)}')
            if mode != 1 or not cfg[9]:
                wr(uc,output,effbase+count*0x100)
            if mode == 0 and cfg[12] >= 0:
                wr(uc,0x686164,cfg[12])
            ret(uc,cfg[11] if mode == 0 else cfg[8],20)
        elif ip == methods+0x20:
            dev,command = rd(uc,sp+4),rd(uc,sp+8)
            trace.append(f'command:{(dev-devbase)//0x100}:{command}:{rd(uc,0x686168)}')
            ret(uc,0x80004005,8)
        elif ip == methods+0x30:
            effect,iterations,flags = [rd(uc,sp+x) for x in (4,8,12)]
            trace.append(f'start:{(effect-effbase)//0x100}:{iterations}:{flags}')
            ret(uc,0x80004005,12)
        elif ip == methods+0x40:
            effect = rd(uc,sp+4)
            trace.append(f'unload:{(effect-effbase)//0x100}')
            ret(uc,0x80004005,4)
        elif ip == 0x513690:
            assert cs(uc,uc.reg_read(UC_X86_REG_EDX)) == b'X:/DATA/' and cs(uc,rd(uc,sp+4)) == b'FFB.IFR'
            trace.append('path')
            uc.mem_write(uc.reg_read(UC_X86_REG_ECX),b'X:/DATA/FFB.IFR\0')
            ret(uc,extra=4)
        elif ip == methods+0x50:
            assert cs(uc,rd(uc,sp+4)) == b'X:/DATA/FFB.IFR'
            dev = rd(uc,sp+8)
            trace.append('load:'+str((dev-devbase)//0x100 if dev else -1))
            ret(uc,0 if cfg[6] else project,8)
        elif ip == 0x45a390:
            assert cs(uc,uc.reg_read(UC_X86_REG_ECX)) == b'X:/DATA/FFB.IFR' and rd(uc,sp+4) == 20
            trace.append('names:'+str(cfg[5]))
            names = uc.reg_read(UC_X86_REG_EDX)
            for i in range(max(0,cfg[5])):
                ptr = 0x1046000+i*0x100
                uc.mem_write(ptr,('same' if cfg[10] else 'Pulse' if i == 0 else 'Bump').encode()+b'\0')
                wr(uc,names+i*4,ptr)
            ret(uc,cfg[5],4)
        elif ip == methods+0x60:
            assert rd(uc,sp+4) == project
            name = cs(uc,rd(uc,sp+8)).decode()
            trace.append('structs:'+name)
            wr(uc,rd(uc,sp+12),cfg[11])
            ptr = array+state['row']*4
            state['row'] += 1
            ret(uc,0 if cfg[7] else ptr,12)
        elif ip == methods+0x70:
            assert rd(uc,sp+4) == project
            trace.append('release-project')
            ret(uc,-1,4)
        elif ip == 0x51af20:
            assert cs(uc,uc.reg_read(UC_X86_REG_ECX)) == b'Cant enable I-Force'
            trace.append('fatal')
            state['status'] = 1
            uc.emu_stop()
        elif ip == 0x44c590:
            trace.append('log:'+cs(uc,rd(uc,sp+4)).hex())
            ret(uc)

    u.hook_add(UC_HOOK_CODE,hook)
    wr(u,0x103ff00,stop)
    u.reg_write(UC_X86_REG_ESP,0x103ff00)
    if mode == 0:
        u.reg_write(UC_X86_REG_ECX,2)
        u.reg_write(UC_X86_REG_EDX,description)
        wr(u,0x103ff04,name)
    elif mode == 2:
        u.reg_write(UC_X86_REG_ECX,cfg[4])
    elif mode == 3:
        u.reg_write(UC_X86_REG_ECX,name)
    try:
        u.emu_start([0x457c20,0x457760,0x457eb0,0x45c720][mode],stop,count=200000)
        if mode == 0 and cfg[12] == -2:
            wr(u,description+0x40,1)
            wr(u,0x103ff00,stop)
            wr(u,0x103ff04,name)
            u.reg_write(UC_X86_REG_ESP,0x103ff00)
            u.reg_write(UC_X86_REG_ECX,2)
            u.reg_write(UC_X86_REG_EDX,description)
            u.emu_start(0x457c20,stop,count=200000)
    except UcError:
        state['status'] = 2
    result = u.reg_read(UC_X86_REG_EAX) if mode < 2 and not state['status'] else 0
    result = result-0x100000000 if result >= 0x80000000 else result
    count = rd(u,0x686164)
    count = count-0x100000000 if count >= 0x80000000 else count
    values = [state['status'],result,count,rd(u,0x686168),int(rd(u,0x79e040) != 0)]
    pointers = [str((rd(u,0x79e0a0+i*4)-effbase)//0x100) if rd(u,0x79e0a0+i*4) else '-1' for i in range(30)]
    axes_state = []
    shared_axis = None
    if mode == 0:
        for i in range(30):
            di = 0x684628+i*232
            if rd(u,di) == 52:
                ptr = rd(u,di+32)
                if shared_axis is None:
                    shared_axis = ptr
                axes_state.append(f'{i}:{rd(u,ptr)}:{rd(u,ptr+4)}:{int(ptr == shared_axis)},')
    return ' '.join(map(str,values))+'\n'+','.join(trace)+(',' if trace else '')+'\n'+bytes(u.mem_read(0x684628,232*30)).hex()+'\n'+','.join(pointers)+'\n'+''.join(axes_state)+'\n'


cases = []
create_base = [0,0,1000,-1,5000,123,90,4,1,0,0,0,-1]
init_base = [0,4,4,2,0,2,0,0,0,0,0,1,0]


def add(name,mode,cfg,**changes):
    cfg = cfg.copy()
    for key,value in changes.items():
        cfg[int(key[1:])] = value
    cases.append((name,mode,cfg))


for typ in range(13):
    add('create-type-'+str(typ),0,create_base,c0=typ)
for duration in (0,0xffffff,0xffffffff,16777217,0x80000000):
    add('duration-'+str(duration),0,create_base,c2=duration)
for axis in (0,1,2,9):
    for subtype in (0,2):
        add(f'axis-{axis}-subtype-{subtype}',0,create_base,c1=axis,c10=subtype)
for count in (29,30,31,0x7fffffff):
    add('count-'+str(count),0,create_base,c9=count)
add('negative-count-null-device',0,create_base,c9=-1,c8=0)
add('no-device',0,create_base,c8=0)
add('no-ff-bit',0,create_base,c7=1)
for typ in (13,0xffffffff):
    add('illegal-type-'+str(typ),0,create_base,c0=typ)
add('create-fails',0,create_base,c11=0x80004005)
add('live-count-after-create',0,create_base,c12=2)
add('shared-axis-storage-sequence',0,create_base,c1=2,c12=-2)
add('trigger-and-overflow',0,create_base,c3=31,c5=0xffffffff,c6=0x7fffffff)
for count in (-1,0,1,2):
    add('init-names-'+str(count),1,init_base,c5=count)
add('init-no-capability',1,init_base,c1=0)
add('init-no-device',1,init_base,c2=0)
add('init-already-available',1,init_base,c0=2,c1=0)
add('init-reset-order',1,init_base,c1=0xffff,c2=0xffff)
add('init-project-fails',1,init_base,c6=1)
add('init-structs-null',1,init_base,c7=1)
add('init-ignored-valid-count',1,init_base,c11=0)
add('init-create-fails',1,init_base,c8=0x80004005)
add('init-create-null-effect',1,init_base,c9=1)
add('init-existing-duplicate-name',1,init_base,c4=1,c10=1)
add('init-slot-fifteen',1,init_base,c1=0x8000,c2=0x8000,c3=15)
add('init-enumeration-already-complete',1,init_base,c12=1)
for present in (0,1):
    add('index-play-'+str(present),2,[1,1,present,0,0]+[0]*8)
for available in (0,1,2):
    for absent in (0,1):
        add(f'named-play-{available}-{absent}',3,[available,3,7,0,absent]+[0]*8)
add('named-duplicate-first',3,[1,3,7,1,0]+[0]*8)
add('named-null-effect',3,[1,3,0,1,0]+[0]*8)
results = []
for name,mode,cfg in cases:
    expected = original(mode,cfg)
    for abi,executable in executables:
        native = subprocess.check_output([str(executable),str(mode),*map(str,cfg)],text=True)
        if native != expected:
            (out/'failure.json').write_text(json.dumps({'name':name,'abi':abi,'native':native,'original':expected},indent=2))
            raise AssertionError((name,abi,'see '+str(out/'failure.json')))
    results.append({'name':name,'mode':mode,'config':cfg,'state':expected.splitlines()[0],'trace':expected.splitlines()[1]})
(out/'results.json').write_text(json.dumps(results,indent=2))
print(f'PASS: {len(cases)} original/C effects cases in normal and matching-fastcall builds; complete persistent records, GUIDs, parameter pointers, HRESULT paths, names and playback flags.')
