"""Compare current joystick settings/serialization C with verified original x86.

Requires Python 3.10+, Unicorn (pip install unicorn), and a native x86/x64
GCC-compatible C compiler (including support for GNU inline assembly).
Example: python scripts/verify_joystick_settings.py --cc path/to/gcc.exe
    --dependency-dir build-audit/joystick/python-deps
The original PE hash is verified; outputs are under build-audit/joystick-settings.
Dependency mocks model current record lookup, gain application, CRT formatting,
raw fwrite calls and token-stream I/O. Current production C bodies are compiled.
Includes original x87 unordered/NaN behavior, float bit patterns, gain clamps,
fallback getters, serialization bytes, ignored write failures and file positions.
Hardware and the separately reconstructed GetAString parser are not exercised.
Unicorn does not quiet signaling NaNs on x87 fld/fstp; those two getter returns
are checked separately against native x87 hardware and the compiler ABI, rather
than counted as exact Unicorn return-bit comparisons. Their raw record state
still must agree exactly. x64 SSE returns preserve signaling bits; x86 x87
returns quiet them. Production getters are unchanged for either native ABI.
"""
import argparse
import hashlib
import json
import pathlib
import shutil
import struct
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
parser.add_argument('--original', type=pathlib.Path, default=ROOT/'build-reference/originals/CARMA2_HW.EXE')
parser.add_argument('--cc', default=shutil.which('gcc') or str(ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'))
parser.add_argument('--dependency-dir', type=pathlib.Path)
parser.add_argument('--output-dir', type=pathlib.Path, default=ROOT/'build-audit/joystick-settings')
args = parser.parse_args()
if args.dependency_dir:
    sys.path.insert(0, str(args.dependency_dir.resolve()))
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX, UC_X86_REG_ESP, UC_X86_REG_EIP, UC_X86_REG_FPCW

binary = args.original.read_bytes()
if hashlib.sha256(binary).hexdigest() != '9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d':
    raise SystemExit('Original PE hash mismatch')
pe = struct.unpack_from('<I', binary, 0x3c)[0]
nsects = struct.unpack_from('<H', binary, pe+6)[0]
sect = pe+24+struct.unpack_from('<H', binary, pe+20)[0]
source = (ROOT/'src/carma2/newcommon/joystick.c').read_text()
out = args.output_dir.resolve()
out.mkdir(parents=True, exist_ok=True)

def extract(name):
    start = source.index(' C2_HOOK_FASTCALL '+name+'(')
    start = source.rfind('\n', 0, start)+1
    end = source.index('{', start)+1
    depth = 1
    while depth:
        depth += (source[end] == '{') - (source[end] == '}')
        end += 1
    return source[start:end]

def cpu():
    u = Uc(UC_ARCH_X86, UC_MODE_32)
    u.mem_map(0x400000, 0x400000)
    u.mem_map(0x1000000, 0x30000)
    for i in range(nsects):
        _, va, rs, rp = struct.unpack_from('<IIII', binary, sect+40*i+8)
        u.mem_write(0x400000+va, binary[rp:rp+rs])
    u.reg_write(UC_X86_REG_FPCW, 0x37f)
    return u

def wr(u, addr, value):
    u.mem_write(addr, struct.pack('<I', value & 0xffffffff))

def rd(u, addr):
    return struct.unpack('<I', u.mem_read(addr, 4))[0]

def ret(u, value=0, extra=0):
    sp = u.reg_read(UC_X86_REG_ESP)
    target = rd(u, sp)
    u.reg_write(UC_X86_REG_EAX, value & 0xffffffff)
    u.reg_write(UC_X86_REG_ESP, sp+4+extra)
    u.reg_write(UC_X86_REG_EIP, target)

def cstring(u, addr):
    s = bytearray()
    while True:
        b = u.mem_read(addr+len(s), 1)[0]
        if b == 0:
            return bytes(s)
        s.append(b)

header = r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#define C2_HOOK_FASTCALL
typedef struct {char productName[80];unsigned count_buttons;int buttons[32];int field_0xd4;float field_0xd8,field_0xdc;int field_0xe0,field_0xe4,field_0xe8;} tButtonJoystickInfo;
typedef char check_size[sizeof(tButtonJoystickInfo)==0xec?1:-1];
unsigned rawbits(const float*p){unsigned v;memcpy(&v,p,4);return v;}
float frombits(unsigned v){float f;memcpy(&f,&v,4);return f;}
'''

settings = [
    ('HasCurrentJoystick',0x45a0f0,'int'), ('ToggleJoystickThrottle',0x45c460,'int'),
    ('SetJoystickDPadEnabled',0x45c570,'void-int'), ('CycleJoystickFFBGain',0x45c780,'int'),
    ('SetJoystickFFBGain',0x45c7b0,'void-int'), ('GetJoystickFBBGain',0x45c7f0,'int'),
    ('SetJoystickX',0x45c810,'void-float'), ('SetJoystickY',0x45c870,'void-float'),
    ('GetJoystickX',0x45c8d0,'float'), ('GetJoystickY',0x45c8f0,'float'),
]
driver = header + r'''
int gJoystick_index,gINT_00596310,gINT_00595f9c=7,present;
float gJoystick_x_steering=1.25f,gJoystick_y_throttle=.75f;
tButtonJoystickInfo info;char trace[256];
tButtonJoystickInfo* PDGetCurrentJoystickData(void){strcat(trace,"get,");return present?&info:NULL;}
void PDSetJoystickFFBGain(int v){sprintf(trace+strlen(trace),"gain:%d,",v);}
'''
driver += '\n'.join(extract(name) for name,_,_ in settings)
driver += r'''
int main(int argc,char**argv){int mode,arg;unsigned input,result=0;float returned;
if(argc==2&&!strcmp(argv[1],"--abi")){printf("%u",(unsigned)(8*sizeof(void*)));return 0;}
if(argc==2&&!strcmp(argv[1],"--x87-probe")){unsigned input=0x7f812345,output;unsigned short status;
__asm__ __volatile__("fnclex\n\tflds %2\n\tfstps %0\n\tfnstsw %1":"=m"(output),"=m"(status):"m"(input):"st");
printf("%u %u",output,(unsigned)status);return 0;}
mode=atoi(argv[1]);arg=atoi(argv[3]);input=(unsigned)strtoul(argv[4],NULL,10);present=atoi(argv[2]);gJoystick_index=arg;gINT_00596310=arg;info.field_0xd4=arg;info.field_0xe0=5;info.field_0xe4=37;memcpy(&info.field_0xd8,&input,4);memcpy(&info.field_0xdc,&input,4);
switch(mode){
'''
for i,(name,_,kind) in enumerate(settings):
    argument = 'arg' if kind == 'void-int' else 'frombits(input)' if kind == 'void-float' else ''
    call = name+'('+argument+')'
    expression = 'result=(unsigned)'+call if kind == 'int' else 'returned='+call+';result=rawbits(&returned)' if kind == 'float' else call
    driver += 'case '+str(i)+':'+expression+';break;\n'
driver += r'''
}printf("%u %u %u %u %u %u %u %u %u %u %s",result,rawbits(&gJoystick_x_steering),rawbits(&gJoystick_y_throttle),(unsigned)gINT_00596310,(unsigned)gINT_00595f9c,(unsigned)info.field_0xd4,rawbits(&info.field_0xd8),rawbits(&info.field_0xdc),(unsigned)info.field_0xe0,(unsigned)info.field_0xe4,trace);return 0;}
'''
(out/'settings.c').write_text(driver)
subprocess.run([args.cc,'-std=c99','-O2',str(out/'settings.c'),'-o',str(out/'settings.exe')],check=True)

def original_setting(mode,present,arg,value):
    name,addr,kind = settings[mode]
    u = cpu()
    info = 0x1010000
    trace = []
    wr(u,0x595f88,arg);wr(u,0x596310,arg);wr(u,0x595f9c,7)
    u.mem_write(0x595f90,struct.pack('<ff',1.25,.75))
    wr(u,info+0xd4,arg);wr(u,info+0xd8,value);wr(u,info+0xdc,value);wr(u,info+0xe0,5);wr(u,info+0xe4,37)
    def hook(uc,ip,size,data):
        if ip == 0x1020006:
            uc.emu_stop()
        elif ip == 0x459f80:
            trace.append('get,');ret(uc,info if present else 0)
        elif ip == 0x457b70:
            gain = uc.reg_read(UC_X86_REG_ECX)
            if gain >= 0x80000000:gain -= 0x100000000
            trace.append('gain:'+str(gain)+',');ret(uc)
    u.hook_add(UC_HOOK_CODE,hook)
    # Store x87 float return to memory, with the same single-precision rounding.
    u.mem_write(0x1020000,b'\xd9\x1d'+struct.pack('<I',0x1011000)+b'\x90')
    wr(u,0x100fffc,0x1020000);wr(u,0x1010000,value) # argument stack uses a separate slot below
    sp=0x100ff00
    wr(u,sp,0x1020000);wr(u,sp+4,value)
    u.reg_write(UC_X86_REG_ESP,sp);u.reg_write(UC_X86_REG_ECX,arg&0xffffffff)
    if kind != 'float':
        u.mem_write(0x1020000,b'\x90'*7)
    u.emu_start(addr,0x1020007,count=2000)
    result=rd(u,0x1011000) if kind=='float' else u.reg_read(UC_X86_REG_EAX) if kind=='int' else 0
    values=[result]+[rd(u,x) for x in [0x595f90,0x595f94,0x596310,0x595f9c,info+0xd4,info+0xd8,info+0xdc,info+0xe0,info+0xe4]]
    return values,''.join(trace)

records=[]
abi_records=[]
native_bits=int(subprocess.check_output([str(out/'settings.exe'),'--abi'],text=True))
hardware_nan,hardware_status=[int(x) for x in subprocess.check_output([str(out/'settings.exe'),'--x87-probe'],text=True).split()]
assert hardware_nan==0x7fc12345 and hardware_status&1,(hardware_nan,hardware_status)
float_patterns=[0xff800000,0xc0400000,0x80000001,0x80000000,0,0x3f000000,0x40000000,0x40000001,0x7f800000,0x7fc12345,0x7f812345]
for mode,(_,_,kind) in enumerate(settings):
    values=float_patterns if 'float' in kind else [0x3f800000]
    ints=[-2147483648,-50,-1,0,1,2,50,75,100,101,2147483647] if 'float' not in kind else [50]
    for present in [0,1]:
        for arg in ints:
            for value in values:
                native=subprocess.check_output([str(out/'settings.exe'),str(mode),str(present),str(arg),str(value)],text=True).split(maxsplit=10)
                expected,trace=original_setting(mode,present,arg,value)
                observed=[int(x) for x in native[:10]]
                if mode in (8,9) and present and value==0x7f812345:
                    # Explicitly separate the known Unicorn x87 emulation gap.
                    assert observed[1:]==expected[1:],(mode,observed,expected)
                    assert expected[0]==value
                    assert observed[0]==(hardware_nan if native_bits==32 else value),(native_bits,observed[0])
                    abi_records.append({'function':settings[mode][0],'native_abi_bits':native_bits,'input_snan_bits':hex(value),'unicorn_return_bits':hex(expected[0]),'native_return_bits':hex(observed[0]),'hardware_x87_roundtrip_bits':hex(hardware_nan),'hardware_x87_status':hex(hardware_status),'raw_record_state_matches':True})
                    continue
                assert observed==expected,(mode,present,arg,hex(value),native,expected)
                assert (native[10] if len(native)>10 else '')==trace,(mode,present,arg,native,trace)
                records.append({'function':settings[mode][0],'record_present':present,'arg':arg,'float_bits':hex(value),'values':expected,'trace':trace})
(out/'settings-results.json').write_text(json.dumps(records,indent=2))
(out/'settings-snan-abi-results.json').write_text(json.dumps(abi_records,indent=2))
print('PASS:',len(records),'exact Unicorn/C settings cases, including qNaN/infinity/subnormal/signed-zero and NaN setter behavior.')
print('PASS: 2 separately classified signaling-NaN getter return checks; native ABI',native_bits,'bits, physical x87 quieting verified; raw record state exact. Unicorn x87 quieting limitation recorded.')

writer = header + r'''
char output[8192];size_t length;int fail,writes;
size_t fake_fwrite(const void*p,size_t size,size_t count,FILE*f){if(size!=1||f!=(FILE*)1)exit(80);writes++;if(!fail){memcpy(output+length,p,count);length+=count;}return fail?0:count;}
#define fwrite fake_fwrite
'''+extract('WriteJoystickPreferences')+r'''
int main(int argc,char**argv){tButtonJoystickInfo info;int s=atoi(argv[1]),i;memset(&info,0,sizeof(info));fail=atoi(argv[2]);strcpy(info.productName,s==0?"Force Wheel":s==1?"":" A__Z ");info.count_buttons=s==0?3:s==1?0:100;for(i=0;i<32;i++)info.buttons[i]=i%3==0?-1:i;info.field_0xd4=-3;info.field_0xd8=1.25f;info.field_0xdc=-.875f;info.field_0xe0=2;info.field_0xe4=83;info.field_0xe8=-7;i=WriteJoystickPreferences((FILE*)1,&info);printf("%d %d ",i,writes);for(i=0;i<(int)length;i++)printf("%02x",(unsigned char)output[i]);return 0;}
'''
(out/'writer.c').write_text(writer)
subprocess.run([args.cc,'-std=c99','-O2',str(out/'writer.c'),'-o',str(out/'writer.exe')],check=True)

def original_writer(scenario,fail):
    u=cpu();info=0x1010000;trace=[];output=bytearray()
    u.mem_write(info,[b'Force Wheel\0',b'\0',b' A__Z \0'][scenario]);wr(u,info+0x50,[3,0,100][scenario])
    for i in range(32):wr(u,info+0x54+4*i,-1 if i%3==0 else i)
    wr(u,info+0xd4,-3);u.mem_write(info+0xd8,struct.pack('<ff',1.25,-.875));wr(u,info+0xe0,2);wr(u,info+0xe4,83);wr(u,info+0xe8,-7)
    before=bytes(u.mem_read(info,0xec))
    def hook(uc,ip,size,data):
        if ip==0x1020000:uc.emu_stop();return
        sp=uc.reg_read(UC_X86_REG_ESP)
        if ip==0x575de0:
            dest=rd(uc,sp+4);fmt=cstring(uc,rd(uc,sp+8)).decode('ascii')
            if fmt=='&%s\n':value=cstring(uc,rd(uc,sp+12)).decode('ascii')
            elif fmt=='%d\n':value=rd(uc,sp+12);value=value-0x100000000 if value>=0x80000000 else value
            elif fmt=='%.3f, %.3f\n':value=struct.unpack('<dd',uc.mem_read(sp+12,16))
            else:assert fmt=='\n';value=()
            payload=(fmt%value).encode('ascii');uc.mem_write(dest,payload+b'\0');ret(uc,len(payload))
        elif ip==0x5774a0:
            ptr,size,count,file=struct.unpack('<IIII',uc.mem_read(sp+4,16));assert size==1 and file==0x1011000
            trace.append(count)
            if not fail:output.extend(uc.mem_read(ptr,count))
            ret(uc,0 if fail else count)
    u.hook_add(UC_HOOK_CODE,hook);wr(u,0x100ff00,0x1020000);u.reg_write(UC_X86_REG_ESP,0x100ff00);u.reg_write(UC_X86_REG_ECX,0x1011000);u.reg_write(UC_X86_REG_EDX,info)
    u.emu_start(0x45b920,0x1020000,count=50000)
    assert bytes(u.mem_read(info,0xec))==before
    return u.reg_read(UC_X86_REG_EAX),trace,bytes(output)

for scenario in range(3):
    for fail in (0,1):
        native=subprocess.check_output([str(out/'writer.exe'),str(scenario),str(fail)],text=True).split()
        result,trace,payload=original_writer(scenario,fail)
        assert int(native[0])==result==1
        assert int(native[1])==len(trace)==40
        assert (bytes.fromhex(native[2]) if len(native)>2 else b'')==payload
        if not fail:
            lines=payload.decode('ascii').splitlines();assert lines[1]=='32' and len(lines)==40
            assert lines[35]=='1.250, -0.875'
(out/'serialization-last.bin').write_bytes(payload)
print('PASS: 6 original/C serialization scenarios; names, fixed32 bindings, signed fields, three-decimal floats, blank separator, and ignored fwrite failures.')

count_inputs=['a'*31,'a'*32,'&one '+('x'*40),'&one foo\n&two '+('x'*32),
              'junk&marker '+('y'*40),'& '+('z'*32),'&one &two &three '+('z'*32),'']
counter = header + r'''
const char* input;long position;int eof_flag,seek_fail;char trace[1024];
void fake_rewind(FILE*f){strcat(trace,"rewind,");position=0;eof_flag=0;}
long fake_ftell(FILE*f){strcat(trace,"ftell,");return position;}
int fake_fseek(FILE*f,long offset,int whence){strcat(trace,"seek,");if(offset!=0||whence!=SEEK_END)exit(81);if(!seek_fail)position=(long)strlen(input);return seek_fail?-1:0;}
int fake_feof(FILE*f){return eof_flag;}
void GetAString(FILE*f,char*s){int n=0;strcat(trace,"string,");while(input[position]==' '||input[position]=='\n')position++;while(input[position]&&input[position]!=' '&&input[position]!='\n')s[n++]=input[position++];s[n]=0;if(!input[position])eof_flag=1;}
#define rewind fake_rewind
#define ftell fake_ftell
#define fseek fake_fseek
#define feof fake_feof
'''+extract('CountJoystickPreferences')+r'''
int main(int argc,char**argv){int result;input=argv[1];seek_fail=atoi(argv[2]);result=CountJoystickPreferences((FILE*)1);printf("%d %ld %d %s",result,position,eof_flag,trace);return 0;}
'''
(out/'counter.c').write_text(counter)
subprocess.run([args.cc,'-std=c99','-O2',str(out/'counter.c'),'-o',str(out/'counter.exe')],check=True)

def original_count(text,seek_fail):
    u=cpu();file=0x1010000;position=0;trace=[]
    def hook(uc,ip,size,data):
        nonlocal position
        if ip==0x1020000:uc.emu_stop();return
        if ip==0x5770e0:
            trace.append('rewind,');position=0;wr(uc,file+12,0);ret(uc)
        elif ip==0x5772f0:
            trace.append('ftell,');ret(uc,position)
        elif ip==0x576ec0:
            sp=uc.reg_read(UC_X86_REG_ESP)
            assert rd(uc,sp+8)==0 and rd(uc,sp+12)==2
            trace.append('seek,')
            if not seek_fail:position=len(text)
            ret(uc,-1 if seek_fail else 0)
        elif ip==0x490630:
            trace.append('string,')
            while position<len(text) and text[position] in ' \n':position+=1
            start=position
            while position<len(text) and text[position] not in ' \n':position+=1
            uc.mem_write(uc.reg_read(UC_X86_REG_EDX),text[start:position].encode()+b'\0')
            if position==len(text):wr(uc,file+12,0x10)
            ret(uc)
    u.hook_add(UC_HOOK_CODE,hook);wr(u,0x100ff00,0x1020000);u.reg_write(UC_X86_REG_ESP,0x100ff00);u.reg_write(UC_X86_REG_ECX,file)
    u.emu_start(0x45bcc0,0x1020000,count=10000)
    return [u.reg_read(UC_X86_REG_EAX),position,1 if rd(u,file+12)&0x10 else 0],''.join(trace)

for text in count_inputs:
    for seek_fail in (0,1):
        native=subprocess.check_output([str(out/'counter.exe'),text,str(seek_fail)],text=True).split(maxsplit=3)
        expected,trace=original_count(text,seek_fail)
        assert [int(x) for x in native[:3]]==expected,(text,seek_fail,native,expected)
        assert native[3]==trace,(text,seek_fail,native,trace)
print('PASS: 16 original/C preference-section count scenarios; 31/32-byte boundary, token prefixes, multiple sections, empty input, ignored seek failure, call order and final file position.')
