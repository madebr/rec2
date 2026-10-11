"""Differentially verify joystick reconstruction against the original x86 binary.

Requires Python 3.10+, Unicorn (python -m pip install unicorn), and a C compiler
that produces executables runnable on this host. The script extracts the current
C function bodies, compiles them with controlled dependency mocks, then compares
returns, globals, record writes, and exact call order with original x86 executed
by Unicorn. The original PE hash is checked before executing any instructions.

Examples:
    python scripts/verify_joystick_reconstruction.py --cc gcc
    python scripts/verify_joystick_reconstruction.py --cc path/to/gcc.exe \
        --dependency-dir build-audit/joystick/python-deps

Dependencies are deliberately mocked: these tests verify the recovered caller
and Windows helper logic, not physical joystick hardware or the file parsers.
The file parser helpers also have independent 100% matching compiler reports.
Artifacts go to build-audit/joystick by default; no repository source is changed.
"""
import argparse, hashlib, json, pathlib, shutil, struct, subprocess, sys
ROOT = pathlib.Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
parser.add_argument('--original', type=pathlib.Path, default=ROOT/'build-reference/originals/CARMA2_HW.EXE')
parser.add_argument('--cc', default=shutil.which('gcc') or str(ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'))
parser.add_argument('--dependency-dir', type=pathlib.Path)
parser.add_argument('--output-dir', type=pathlib.Path, default=ROOT/'build-audit/joystick')
args = parser.parse_args()
if args.dependency_dir:
    sys.path.insert(0, str(args.dependency_dir.resolve()))
out = args.output_dir.resolve()
out.mkdir(parents=True, exist_ok=True)
binary = args.original.read_bytes()
expected_hash = '9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
if hashlib.sha256(binary).hexdigest() != expected_hash:
    raise SystemExit('Original PE SHA-256 does not match the verified CARMA2_HW.EXE.')
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EIP, UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX
source = (ROOT/'src/carma2/newcommon/joystick.c').read_text()
def extract(name, text=source):
    start = text.index(' C2_HOOK_FASTCALL '+name+'(')
    start = text.rfind('\n', 0, start)+1
    end = text.index('{', start); depth=1; end+=1
    while depth:
        depth += (text[end]=='{')-(text[end]=='}'); end+=1
    return text[start:end]
driver = r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#define C2_HOOK_FASTCALL
typedef char tPath_name[256];
typedef struct {char name[80]; unsigned count; int buttons[32]; int field_0xd4; float field_0xd8,field_0xdc; int field_0xe0,field_0xe4,field_0xe8;} tButtonJoystickInfo;
typedef char check_info_size[sizeof(tButtonJoystickInfo)==0xec?1:-1];
typedef char check_info_fields[offsetof(tButtonJoystickInfo,field_0xd4)==0xd4 && offsetof(tButtonJoystickInfo,field_0xe8)==0xe8?1:-1];
int gINT_00595f84=7500,gINT_00596310=50,gINT_00595f9c=1, scenario,current=1;
float gJoystick_x_steering=1,gJoystick_y_throttle=1;
char gApplication_path[256]="X:/DATA/", trace[1024]; tButtonJoystickInfo info;
void note(const char*s){strcat(trace,s);strcat(trace,",");}
tButtonJoystickInfo* PDGetCurrentJoystickData(void){note("get");return current?&info:NULL;}
void PathCat(char*d,const char*a,const char*b){note("path");strcpy(d,a);strcat(d,b);}
FILE* fake_fopen(const char*p,const char*m){note("open");if(strcmp(p,"X:/DATA/JOYSTICK.TXT")||strcmp(m,"rt"))exit(90);return scenario==1?NULL:(FILE*)1;}
int fake_fclose(FILE*f){note("close");return 0;}
int fake_remove(const char*p){note("remove");return 0;}
const char* GetCurrentJoystickName(void){note("name");return "Wheel";}
int FindJoystickPreferences(FILE*f,const char*n){note("find");return scenario==2?-1:scenario==8?0:1;}
int ReadJoystickPreferences(FILE*f,tButtonJoystickInfo*i){note("read");i->field_0xd8=2.5f;i->field_0xdc=-0.25f;i->field_0xd4=7;i->field_0xe4=99;i->field_0xe8=scenario!=3;return 1;}
void PDSetJoystickFFBGain(int g){note("gain");if(g!=gINT_00596310)exit(91);}
int PDEnableJoysticks(void){note("enable");if(scenario==4)current=0;return 2;}
int PDDisableJoysticks(void){note("disable");current=0;return 2;}
#define fopen fake_fopen
#define fclose fake_fclose
#define remove fake_remove
'''
driver += '\n'.join(extract(x) for x in ['EnableJoysticks','DisableJoysticks','LoadJoystickPreferences'])
driver += r'''
int main(int argc,char**argv){int result;scenario=atoi(argv[1]);current=scenario!=0;info.field_0xd4=3;info.field_0xd8=1.25f;info.field_0xdc=0.75f;info.field_0xe4=61;info.field_0xe8=1;gINT_00596310=scenario==5?100:scenario==6?-50:scenario==7?75:50;result=LoadJoystickPreferences();printf("%d %.8g %.8g %d %d %d %d %s",result,gJoystick_x_steering,gJoystick_y_throttle,gINT_00595f9c,gINT_00595f84,gINT_00596310,info.field_0xe8,trace);return 0;}
'''
(out/'load_harness.c').write_text(driver)
subprocess.run([args.cc,'-std=c89','-O2',str(out/'load_harness.c'),'-o',str(out/'load_harness.exe')],check=True)
pe=struct.unpack_from('<I',binary,0x3c)[0];nsects=struct.unpack_from('<H',binary,pe+6)[0];optsize=struct.unpack_from('<H',binary,pe+20)[0];sect=pe+24+optsize
def original(scenario):
    u=Uc(UC_ARCH_X86,UC_MODE_32);u.mem_map(0x400000,0x400000);u.mem_map(0x1000000,0x20000)
    for i in range(nsects):
        vs,va,rs,rp=struct.unpack_from('<IIII',binary,sect+40*i+8)
        u.mem_write(0x400000+va,binary[rp:rp+rs])
    info=0x1010000; current=[scenario!=0]; trace=[]
    def wr(addr,v):u.mem_write(addr,struct.pack('<I',v&0xffffffff))
    def rd(addr):return struct.unpack('<I',u.mem_read(addr,4))[0]
    def fl(addr):return struct.unpack('<f',u.mem_read(addr,4))[0]
    wr(info+0xd4,3);u.mem_write(info+0xd8,struct.pack('<ff',1.25,0.75));wr(info+0xe4,61);wr(info+0xe8,1)
    wr(0x596310,100 if scenario==5 else -50 if scenario==6 else 75 if scenario==7 else 50)
    u.mem_write(0x75ba60,b'X:/DATA/\0')
    hooks={0x459f80:'get',0x513690:'path',0x577170:'open',0x4599d0:'name',0x45b7e0:'find',0x45bbc0:'read',0x577640:'close',0x5776c0:'remove',0x457b70:'gain',0x45a070:'enable',0x45a0c0:'disable'}
    def hook(uc,addr,size,data):
        if addr==0x1020000: uc.emu_stop();return
        if addr not in hooks:return
        event=hooks[addr];trace.append(event);sp=uc.reg_read(UC_X86_REG_ESP);ret=rd(sp);extra=4 if event=='path' else 0;value=0
        if event=='get':value=info if current[0] else 0
        elif event=='path':u.mem_write(uc.reg_read(UC_X86_REG_ECX),b'X:/DATA/JOYSTICK.TXT\0')
        elif event=='open':value=0 if scenario==1 else 0x1011000
        elif event=='name':value=0x1011200;u.mem_write(value,b'Wheel\0')
        elif event=='find':value=-1 if scenario==2 else 0 if scenario==8 else 1
        elif event=='read':u.mem_write(info+0xd8,struct.pack('<ff',2.5,-0.25));wr(info+0xd4,7);wr(info+0xe4,99);wr(info+0xe8,scenario!=3);value=1
        elif event=='enable' and scenario==4:current[0]=False
        elif event=='disable':current[0]=False
        uc.reg_write(UC_X86_REG_EAX,value&0xffffffff);uc.reg_write(UC_X86_REG_ESP,sp+4+extra);uc.reg_write(UC_X86_REG_EIP,ret)
    u.hook_add(UC_HOOK_CODE,hook)
    wr(0x100fffc,0x1020000);u.reg_write(UC_X86_REG_ESP,0x100fffc);u.emu_start(0x45c0b0,0x1020000,count=5000)
    result=[u.reg_read(UC_X86_REG_EAX),fl(0x595f90),fl(0x595f94),rd(0x595f9c),rd(0x595f84),rd(0x596310),rd(info+0xe8)]
    result=[x-0x100000000 if isinstance(x,int) and x>=0x80000000 else x for x in result]
    return result,','.join(trace)+','
records=[]
for s in range(9):
    native=subprocess.check_output([str(out/'load_harness.exe'),str(s)],text=True).split(); values=[float(x) for x in native[:7]]
    expected,trace=original(s)
    assert values==expected,(s,values,expected)
    assert native[7]==trace,(s,native[7],trace)
    records.append({'scenario':s,'values':expected,'trace':trace})
(out/'load-differential-results.json').write_text(json.dumps(records,indent=2))
print('PASS: 9 original-x86 versus actual-C differential scenarios; globals, returns, record mutation, and exact call order agree.')

platform_source=(ROOT/'src/carma2/platform/win32/win32_dinput.c').read_text()
platform_driver=r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define C2_HOOK_FASTCALL
typedef unsigned int DWORD;
typedef struct {DWORD dwSize,dwHeaderSize,dwObj,dwHow;} DIPROPHEADER;
typedef struct {DIPROPHEADER diph;DWORD dwData;} DIPROPDWORD;
typedef struct {int index;} IDirectInputEffect;
typedef struct {int index;} IDirectInputDevice2A;
#define DIPH_DEVICE 0
#define DIPROP_FFGAIN 7
int gForceFeedbackAvailable,gJoystick_index,gCount_joystick_effects,gCountEnumeratedJoystickDinputDevices,gLast_joystick_index,gJoystickFFB;
IDirectInputDevice2A* gDirectInputJoystickDevices[16];
IDirectInputEffect* gDirectInputEffects[30];
char trace[1024];
void fake_unload(IDirectInputEffect*e){sprintf(trace+strlen(trace),"unload:%d,",e->index);}
void fake_download(IDirectInputEffect*e){sprintf(trace+strlen(trace),"download:%d,",e->index);}
void fake_property(IDirectInputDevice2A*d,int key,DIPROPHEADER*h){DIPROPDWORD*p=(DIPROPDWORD*)h;sprintf(trace+strlen(trace),"property:%d:%d:%u:%u:%u:%u:%u,",d->index,key,h->dwSize,h->dwHeaderSize,h->dwObj,h->dwHow,p->dwData);}
#define IDirectInputEffect_Unload(e) fake_unload(e)
#define IDirectInputEffect_Download(e) fake_download(e)
#define IDirectInputDevice_SetProperty(d,k,h) fake_property(d,k,h)
void UnloadDinputFFBEffectAtIndex(int index);
void DownloadDinputFFBEffectAtIndex(int index);
'''
platform_driver+='\n'.join(extract(x,platform_source) for x in ['PDSetJoystickFFBGain','DownloadDinputFFBEffectAtIndex','UnloadDinputFFBEffectAtIndex','PDEnableJoysticks','PDDisableJoysticks'])
platform_driver+=r'''
int main(int argc,char**argv){IDirectInputEffect effects[3];IDirectInputDevice2A devices[16];int mode=atoi(argv[1]),scenario=atoi(argv[2]),result=0,i;
for(i=0;i<16;i++){devices[i].index=i;gDirectInputJoystickDevices[i]=&devices[i];}
for(i=0;i<3;i++){effects[i].index=i;gDirectInputEffects[i]=&effects[i];}
gForceFeedbackAvailable=1;gJoystick_index=2;gCount_joystick_effects=3;gCountEnumeratedJoystickDinputDevices=2;gLast_joystick_index=3;gJoystickFFB=8;
if(mode==0){if(scenario==0)gForceFeedbackAvailable=0;if(scenario==1)gJoystick_index=-1;if(scenario==2)gCount_joystick_effects=0;if(scenario==3)gDirectInputEffects[1]=NULL;if(scenario==4)gCount_joystick_effects=-1;PDSetJoystickFFBGain(atoi(argv[3]));}
if(mode==1){gForceFeedbackAvailable=0;if(scenario==0)gCountEnumeratedJoystickDinputDevices=0;if(scenario==1)gLast_joystick_index=-1;if(scenario==2)gDirectInputJoystickDevices[3]=NULL;if(scenario==3)gJoystickFFB=0;if(scenario==4)gJoystickFFB=4;result=PDEnableJoysticks();}
if(mode==2){if(scenario==0)gCountEnumeratedJoystickDinputDevices=0;if(scenario==1)gJoystick_index=-1;result=PDDisableJoysticks();}
printf("%d %d %d %d %s",result,gJoystick_index,gLast_joystick_index,gForceFeedbackAvailable,trace);return 0;}
'''
(out/'platform_harness.c').write_text(platform_driver)
subprocess.run([args.cc,'-std=c89','-O2',str(out/'platform_harness.c'),'-o',str(out/'platform_harness.exe')],check=True)
def original_platform(mode,scenario,gain):
    u=Uc(UC_ARCH_X86,UC_MODE_32);u.mem_map(0x400000,0x400000);u.mem_map(0x1000000,0x30000)
    for i in range(nsects):
        vs,va,rs,rp=struct.unpack_from('<IIII',binary,sect+40*i+8);u.mem_write(0x400000+va,binary[rp:rp+rs])
    def wr(addr,v):u.mem_write(addr,struct.pack('<I',v&0xffffffff))
    def rd(addr):return struct.unpack('<I',u.mem_read(addr,4))[0]
    trace=[]
    wr(0x686168,1 if mode!=1 else 0);wr(0x595f88,2);wr(0x686164,3);wr(0x686160,2);wr(0x595f8c,3);wr(0x68615c,8)
    for i in range(16):wr(0x79e060+4*i,0x1015000+8*i);wr(0x1015000+8*i,0x1016000);wr(0x1015004+8*i,i)
    wr(0x1016000+0x18,0x1018020)
    for i in range(3):wr(0x79e0a0+4*i,0x1017000+8*i);wr(0x1017000+8*i,0x1017100);wr(0x1017004+8*i,i)
    wr(0x1017100+0x2c,0x1018000);wr(0x1017100+0x28,0x1018010)
    if mode==0:
        if scenario==0:wr(0x686168,0)
        if scenario==1:wr(0x595f88,-1)
        if scenario==2:wr(0x686164,0)
        if scenario==3:wr(0x79e0a4,0)
        if scenario==4:wr(0x686164,-1)
    if mode==1:
        if scenario==0:wr(0x686160,0)
        if scenario==1:wr(0x595f8c,-1)
        if scenario==2:wr(0x79e06c,0)
        if scenario==3:wr(0x68615c,0)
        if scenario==4:wr(0x68615c,4)
    if mode==2:
        if scenario==0:wr(0x686160,0)
        if scenario==1:wr(0x595f88,-1)
    def hook(uc,addr,size,data):
        if addr==0x1020000:uc.emu_stop();return
        if addr not in (0x1018000,0x1018010,0x1018020):return
        sp=uc.reg_read(UC_X86_REG_ESP);ret=rd(sp);this=rd(sp+4)
        if addr==0x1018020:
            key=rd(sp+8);prop=rd(sp+12);fields=struct.unpack('<IIIII',u.mem_read(prop,20));trace.append('property:'+':'.join(str(x) for x in (rd(this+4),key,*fields))+',');extra=12
        else:
            trace.append(('unload:' if addr==0x1018000 else 'download:')+str(rd(this+4))+',');extra=4
        uc.reg_write(UC_X86_REG_EAX,0);uc.reg_write(UC_X86_REG_ESP,sp+4+extra);uc.reg_write(UC_X86_REG_EIP,ret)
    u.hook_add(UC_HOOK_CODE,hook);wr(0x100fffc,0x1020000);u.reg_write(UC_X86_REG_ESP,0x100fffc);u.reg_write(UC_X86_REG_ECX,gain&0xffffffff)
    u.emu_start([0x457b70,0x45a070,0x45a0c0][mode],0x1020000,count=5000)
    vals=[0 if mode==0 else u.reg_read(UC_X86_REG_EAX),rd(0x595f88),rd(0x595f8c),rd(0x686168)]
    return [x-0x100000000 if x>=0x80000000 else x for x in vals],''.join(trace)
platform_records=[]
for mode in range(3):
    for s in range(5 if mode<2 else 3):
        for gain in ([-1,0,50,100,101] if mode==0 else [0]):
            native=subprocess.check_output([str(out/'platform_harness.exe'),str(mode),str(s),str(gain)],text=True).split(maxsplit=4)
            expected,trace=original_platform(mode,s,gain)
            assert [int(x) for x in native[:4]]==expected,(mode,s,gain,native,expected)
            assert (native[4] if len(native)>4 else '')==trace,(mode,s,gain,native,trace)
            platform_records.append({'mode':mode,'scenario':s,'gain':gain,'values':expected,'trace':trace})
(out/'platform-differential-results.json').write_text(json.dumps(platform_records,indent=2))
print('PASS: 33 original-x86 versus actual-C platform scenarios; gain clamp, property layout, effect unload/download order, enabled state and return counts agree.')
