"""Differential tests for joystick preference save and backup/restore.

Requires Unicorn, Python 3.10+, and a runnable i686 GCC-compatible compiler.
Compiles actual current C bodies with controlled file/device/allocation helpers;
uses repository headers for record/path types and the source's private node type;
executes the verified original x86 with equivalent helpers. Compares return
values, device index, call order, all record bytes, handle leaks and heap leaks.
Expected unchecked-NULL failures are observed as faults, not fixed or hidden.
The separately verified serializer/parser are mocked here to isolate save flow.
Example: python scripts/verify_joystick_save.py
    --dependency-dir build-audit/joystick/python-deps
"""
import argparse
import hashlib
import json
import pathlib
import struct
import subprocess
import sys

ROOT=pathlib.Path(__file__).resolve().parents[1]
parser=argparse.ArgumentParser(description=__doc__,formatter_class=argparse.RawDescriptionHelpFormatter)
parser.add_argument('--original',type=pathlib.Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE')
parser.add_argument('--cc',default=str(ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'))
parser.add_argument('--dependency-dir',type=pathlib.Path)
parser.add_argument('--output-dir',type=pathlib.Path,default=ROOT/'build-audit/joystick-save')
args=parser.parse_args()
if args.dependency_dir:sys.path.insert(0,str(args.dependency_dir.resolve()))
from unicorn import Uc, UcError, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_ECX,UC_X86_REG_EDX,UC_X86_REG_ESP,UC_X86_REG_EIP

binary=args.original.read_bytes()
assert hashlib.sha256(binary).hexdigest()=='9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
pe=struct.unpack_from('<I',binary,0x3c)[0];nsects=struct.unpack_from('<H',binary,pe+6)[0];sect=pe+24+struct.unpack_from('<H',binary,pe+20)[0]
source=(ROOT/'src/carma2/newcommon/joystick.c').read_text()
out=args.output_dir.resolve();out.mkdir(parents=True,exist_ok=True)

def extract(name):
    start=source.index(' C2_HOOK_FASTCALL '+name+'(');start=source.rfind('\n',0,start)+1
    end=source.index('{',start)+1;depth=1
    while depth:depth+=(source[end]=='{')-(source[end]=='}');end+=1
    return source[start:end]

driver=r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <windows.h>
#include <stddef.h>
#include "joystick.h"
PRIVATE_NODE_TYPE
typedef char check_layout[sizeof(void*)==4&&sizeof(tPath_name)==256&&sizeof(tButtonJoystickInfo)==236&&offsetof(tButtonJoystickInfo,count_buttons)==0x50&&offsetof(tButtonJoystickInfo,field_0xd4)==0xd4&&offsetof(tButtonJoystickInfo,field_0xe8)==0xe8&&sizeof(tJoystick_preference_node)==8&&offsetof(tJoystick_preference_node,info)==0&&offsetof(tJoystick_preference_node,next)==4?1:-1];
int gJoystick_index=7,gJoystick_preferences_backup_valid;
tButtonJoystickInfo gJoystick_preferences_backup,records[16];
char gApplication_path[256]="X:/DATA/",trace[32768];
union Block {unsigned long long aligned;unsigned char bytes[512];} blocks[64];
int active[64],allocated,scenario,read_open,write_open,read_cursor,position,eof_flag,size,sections,mask,fail_at;
const char* name(int index){return scenario==5?"Wheel":index==0?"Wheel":"Pad";}
void note(const char*format,...){va_list a;va_start(a,format);vsprintf(trace+strlen(trace),format,a);va_end(a);strcat(trace,",");}
void fill(tButtonJoystickInfo*p,const char*n,int tag,int preserve_count){unsigned old=p->count_buttons;int i;memset(p,0,sizeof(*p));strcpy(p->productName,n);p->count_buttons=preserve_count?old:4;for(i=0;i<32;i++)p->buttons[i]=tag+i;p->field_0xd4=1;p->field_0xd8=1.25f;p->field_0xdc=.375f;p->field_0xe0=0;p->field_0xe4=tag;p->field_0xe8=1;}
void hex(const void*p,char*s){int i;for(i=0;i<236;i++)sprintf(s+2*i,"%02x",((const unsigned char*)p)[i]);}
void dump(const char*status,int result){int i,live=0;char a[473],b[473],c[473];for(i=0;i<allocated;i++)live+=active[i];hex(&records[0],a);hex(&records[2],b);hex(&gJoystick_preferences_backup,c);printf("%s %d %d %d %d %d %d %d\n%s\n%s\n%s\n%s\n",status,result,gJoystick_index,live,read_open,write_open,position,gJoystick_preferences_backup_valid,trace,a,b,c);}
LONG WINAPI fault(EXCEPTION_POINTERS*p){dump("fault",0);fflush(stdout);ExitProcess(0);return EXCEPTION_EXECUTE_HANDLER;}
void PathCat(char*d,const char*a,const char*b){note("path");strcpy(d,a);strcat(d,b);}
FILE* fake_fopen(const char*p,const char*m){if(strcmp(p,"X:/DATA/JOYSTICK.TXT"))exit(80);note("open:%s",m);if(!strcmp(m,"rt")){if(scenario==0||scenario==1||scenario==14)return NULL;read_open=1;return (FILE*)1;}if(scenario==1||scenario==12)return NULL;write_open=1;return (FILE*)2;}
int fake_fclose(FILE*f){note("close:%d",(int)(uintptr_t)f);if(f==(FILE*)1)read_open=0;else write_open=0;return f==(FILE*)2&&(scenario==13||scenario==14)?-1:0;}
void fake_rewind(FILE*f){note("rewind");position=0;eof_flag=0;read_cursor=0;}
long fake_ftell(FILE*f){note("tell");return position;}
int fake_fseek(FILE*f,long offset,int whence){note("seek");if(offset||whence!=SEEK_END)exit(81);if(scenario==15)return -1;position=size;return 0;}
int fake_feof(FILE*f){return eof_flag;}
void GetAString(FILE*f,char*s){note("token");if(sections==0)strcpy(s,"none");else sprintf(s,"&%s",read_cursor==0?"Wheel":scenario==5?"Wheel":"Old");read_cursor++;if(read_cursor>=sections)eof_flag=1;}
int ReadJoystickPreferences(FILE*f,tButtonJoystickInfo*p){const char*n=read_cursor==0?"Wheel":scenario==5?"Wheel":"Old";note("read:%d",read_cursor);fill(p,n,100+read_cursor,1);read_cursor++;return 1;}
int WriteJoystickPreferences(FILE*f,const tButtonJoystickInfo*p){char s[473];hex(p,s);note("write:%s",s);return 0;}
int PDIsJoystickAvailable(int index){note("available:%d",index);return (mask&(1<<index))!=0;}
tButtonJoystickInfo* PDGetCurrentJoystickData(void){note("get:%d",gJoystick_index);return gJoystick_index>=0&&gJoystick_index<16&&(mask&(1<<gJoystick_index))&&!(scenario==0&&gJoystick_index==2)?&records[gJoystick_index]:NULL;}
const char* GetCurrentJoystickName(void){note("name:%d",gJoystick_index);return name(gJoystick_index);}
void* fake_malloc(size_t n){int i=allocated++;note("alloc:%d:%d:%s",i+1,(int)n,i+1==fail_at?"fail":"ok");if(i+1==fail_at)return NULL;memset(blocks[i].bytes,0xa5,sizeof(blocks[i].bytes));active[i]=1;return blocks[i].bytes;}
void fake_free(void*p){int i;for(i=0;i<allocated;i++)if(p==blocks[i].bytes){if(!active[i])exit(82);note("free:%d",i+1);active[i]=0;return;}exit(83);}
int fake_sprintf(char*d,const char*f,...){va_list a;int result;note("error-format");va_start(a,f);result=vsprintf(d,f,a);va_end(a);return result;}
#define fopen fake_fopen
#define fclose fake_fclose
#define rewind fake_rewind
#define ftell fake_ftell
#define fseek fake_fseek
#define feof fake_feof
#define malloc fake_malloc
#define free fake_free
#define sprintf fake_sprintf
'''
driver+='\n'.join(extract(x) for x in ['CountJoystickPreferences','SaveJoystickPreferences','BackupJoystickPreferences','RestoreJoystickPreferences'])
driver+=r'''
int main(int argc,char**argv){int result,i;SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);SetUnhandledExceptionFilter(fault);scenario=atoi(argv[1]);size=scenario==2?20:64;sections=scenario==2||scenario==3?0:scenario==4||scenario==5||scenario==9?2:1;mask=scenario==6||scenario==16?0:5;fail_at=scenario==7?1:scenario==8||scenario==16?2:scenario==9||scenario==10?3:scenario==11?4:0;if(scenario==10||scenario==11)scenario=atoi(argv[1]);for(i=0;i<16;i++)fill(&records[i],name(i),10+i,0);if(scenario==10||scenario==11)strcpy(records[0].productName,"Pad");
if(argc>2){int step;gJoystick_index=atoi(argv[2])?0:7;for(step=0;argv[3][step];step++){switch(argv[3][step]){case 'B':BackupJoystickPreferences();break;case 'R':RestoreJoystickPreferences();break;case 'M':memset(&records[0],0x5a,sizeof(records[0]));break;case 'S':gJoystick_index=2;break;case 'N':gJoystick_index=7;break;case 'Z':gJoystick_preferences_backup_valid=0;break;}}dump("ok",0);return 0;}
if(scenario==10||scenario==11){/* Driver identity, not record name, controls matching. */}
result=SaveJoystickPreferences();dump("ok",result);return 0;}
'''
# Scenarios10/11 need an unmatched physical driver name, too.
driver=driver.replace('return scenario==5?"Wheel":index==0?"Wheel":"Pad";','return scenario==5?"Wheel":scenario==10||scenario==11?"Pad":index==0?"Wheel":"Pad";')
node_start=source.index('typedef struct tJoystick_preference_node')
node_end=source.index('} tJoystick_preference_node;',node_start)+len('} tJoystick_preference_node;')
driver=driver.replace('PRIVATE_NODE_TYPE',source[node_start:node_end])
(out/'save.c').write_text(driver)
includes=['src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/brender/include','src/carma2/s3/include']
subprocess.run([args.cc,'-std=c99','-O2',*[option for path in includes for option in ['-I',str(ROOT/path)]],str(out/'save.c'),'-o',str(out/'save.exe')],check=True)

def cpu():
    u=Uc(UC_ARCH_X86,UC_MODE_32);u.mem_map(0x400000,0x400000);u.mem_map(0x1000000,0x100000)
    for i in range(nsects):
        _,va,rs,rp=struct.unpack_from('<IIII',binary,sect+40*i+8);u.mem_write(0x400000+va,binary[rp:rp+rs])
    return u
def wr(u,a,v):u.mem_write(a,struct.pack('<I',v&0xffffffff))
def rd(u,a):return struct.unpack('<I',u.mem_read(a,4))[0]
def ret(u,value=0,extra=0):
    sp=u.reg_read(UC_X86_REG_ESP);target=rd(u,sp);u.reg_write(UC_X86_REG_EAX,value&0xffffffff);u.reg_write(UC_X86_REG_ESP,sp+4+extra);u.reg_write(UC_X86_REG_EIP,target)
def cs(u,a):
    s=bytearray()
    while True:
        b=u.mem_read(a+len(s),1)[0]
        if not b:return bytes(s)
        s.append(b)
def packed_record(name,tag,count=4):
    return struct.pack('<80sI32iiffiii',name.encode(),count,*[tag+i for i in range(32)],1,1.25,.375,0,tag,1)

def original(scenario,sequence=None,present=1):
    u=cpu();trace=[];state={'read_open':0,'write_open':0,'position':0,'read_cursor':0,'alloc':0};live=set();allocated={}
    size=20 if scenario==2 else 64
    sections=0 if scenario in (2,3) else 2 if scenario in (4,5,9) else 1
    mask=0 if scenario in (6,16) else 5
    fail_at=1 if scenario==7 else 2 if scenario in (8,16) else 3 if scenario in (9,10) else 4 if scenario==11 else 0
    def name(index):return 'Wheel' if scenario==5 else 'Pad' if scenario in (10,11) else 'Wheel' if index==0 else 'Pad'
    wr(u,0x595f88,7);u.mem_write(0x75ba60,b'X:/DATA/\0')
    for i in range(16):u.mem_write(0x1010000+0x100*i,packed_record(name(i),10+i))
    def hook(uc,ip,size_,data):
        if ip==0x1080000:uc.emu_stop();return
        sp=uc.reg_read(UC_X86_REG_ESP);index=rd(uc,0x595f88);index=index-0x100000000 if index>=0x80000000 else index
        if ip==0x513690:
            trace.append('path');uc.mem_write(uc.reg_read(UC_X86_REG_ECX),b'X:/DATA/JOYSTICK.TXT\0');ret(uc,extra=4)
        elif ip==0x577170:
            assert cs(uc,rd(uc,sp+4))==b'X:/DATA/JOYSTICK.TXT'
            mode=cs(uc,rd(uc,sp+8)).decode();trace.append('open:'+mode)
            if mode=='rt':
                if scenario in (0,1,14):ret(uc);return
                state['read_open']=1;ret(uc,0x1040000)
            else:
                if scenario in (1,12):ret(uc);return
                state['write_open']=1;ret(uc,0x1041000)
        elif ip==0x577640:
            reading=rd(uc,sp+4)==0x1040000;trace.append('close:'+('1' if reading else '2'));state['read_open' if reading else 'write_open']=0;ret(uc,-1 if not reading and scenario in (13,14) else 0)
        elif ip==0x5770e0:
            trace.append('rewind');state['position']=0;state['read_cursor']=0;wr(uc,0x104000c,0);ret(uc)
        elif ip==0x5772f0:trace.append('tell');ret(uc,state['position'])
        elif ip==0x576ec0:
            assert rd(uc,sp+8)==0 and rd(uc,sp+12)==2;trace.append('seek')
            if scenario!=15:state['position']=size
            ret(uc,-1 if scenario==15 else 0)
        elif ip==0x490630:
            trace.append('token');n=state['read_cursor'];token='none' if sections==0 else '&'+('Wheel' if n==0 or scenario==5 else 'Old');uc.mem_write(uc.reg_read(UC_X86_REG_EDX),token.encode()+b'\0');state['read_cursor']+=1
            if state['read_cursor']>=sections:wr(uc,0x104000c,0x10)
            ret(uc)
        elif ip==0x45bbc0:
            n=state['read_cursor'];trace.append('read:'+str(n));ptr=uc.reg_read(UC_X86_REG_EDX);old=rd(uc,ptr+80);uc.mem_write(ptr,packed_record('Wheel' if n==0 or scenario==5 else 'Old',100+n,old));state['read_cursor']+=1;ret(uc,1)
        elif ip==0x45b920:
            ptr=uc.reg_read(UC_X86_REG_EDX);trace.append('write:'+bytes(uc.mem_read(ptr,236)).hex());ret(uc)
        elif ip==0x459fd0:
            idx=uc.reg_read(UC_X86_REG_ECX);trace.append('available:'+str(idx));ret(uc,1 if mask&(1<<idx) else 0)
        elif ip==0x459f80:
            trace.append('get:'+str(index));ret(uc,0x1010000+0x100*index if 0<=index<16 and mask&(1<<index) and not(scenario==0 and index==2) else 0)
        elif ip==0x4599d0:
            trace.append('name:'+str(index));uc.mem_write(0x1042000,name(index).encode()+b'\0');ret(uc,0x1042000)
        elif ip==0x576d40:
            state['alloc']+=1;ident=state['alloc'];n=rd(uc,sp+4);trace.append('alloc:'+str(ident)+':'+str(n)+':'+('fail' if ident==fail_at else 'ok'))
            if ident==fail_at:ret(uc);return
            ptr=0x1050000+ident*0x1000;allocated[ptr]=ident;live.add(ident);uc.mem_write(ptr,b'\xa5'*512);ret(uc,ptr)
        elif ip==0x5775f0:
            ident=allocated[rd(uc,sp+4)];assert ident in live;trace.append('free:'+str(ident));live.remove(ident);ret(uc)
        elif ip==0x575de0:
            trace.append('error-format');assert cs(uc,rd(uc,sp+8))==b'Cant open %s';message=b'Cant open '+cs(uc,rd(uc,sp+12));uc.mem_write(rd(uc,sp+4),message+b'\0');ret(uc,len(message))
    u.hook_add(UC_HOOK_CODE,hook)
    def run(addr):
        wr(u,0x103ff00,0x1080000);u.reg_write(UC_X86_REG_ESP,0x103ff00);u.emu_start(addr,0x1080000,count=100000)
    status='ok';result=0
    try:
        if sequence is None:run(0x45bd90);result=u.reg_read(UC_X86_REG_EAX);result=result-0x100000000 if result>=0x80000000 else result
        else:
            wr(u,0x595f88,0 if present else 7)
            for step in sequence:
                if step=='B':run(0x45c1e0)
                elif step=='R':run(0x45c210)
                elif step=='M':u.mem_write(0x1010000,b'\x5a'*236)
                elif step=='S':wr(u,0x595f88,2)
                elif step=='N':wr(u,0x595f88,7)
                elif step=='Z':wr(u,0x686184,0)
    except UcError:
        status='fault'
    index=rd(u,0x595f88);index=index-0x100000000 if index>=0x80000000 else index
    head=[status,result,index,len(live),state['read_open'],state['write_open'],state['position'],rd(u,0x686184)]
    lines=[' '.join(str(x) for x in head),','.join(trace)+(',' if trace else '')]
    lines += [bytes(u.mem_read(a,236)).hex() for a in (0x1010000,0x1010200,0x79d8c0)]
    return '\n'.join(lines)+'\n'

results=[]
for scenario in range(17):
    native=subprocess.check_output([str(out/'save.exe'),str(scenario)],text=True)
    expected=original(scenario)
    assert native==expected,(scenario,native,expected)
    results.append({'scenario':scenario,'result':expected.splitlines()[0],'trace':expected.splitlines()[1]})
for present in (0,1):
    for sequence in ['R','B','BMR','BMSR','BNR','BRR','BZR','NBR','BMBR']:
        native=subprocess.check_output([str(out/'save.exe'),'4',str(present),sequence],text=True)
        expected=original(4,sequence,present)
        assert native==expected,(present,sequence,native,expected)
        results.append({'record_present':present,'sequence':sequence,'result':expected.splitlines()[0],'trace':expected.splitlines()[1]})
(out/'results.json').write_text(json.dumps(results,indent=2))
print('PASS: 17 original/C save cases including matching/append order, duplicate names, open/seek/allocation/close failures, leaks and expected unchecked-NULL faults.')
print('PASS: 18 original/C backup/restore sequences, including missing records, invalid backup, cross-device full record copies and repeated restores.')
