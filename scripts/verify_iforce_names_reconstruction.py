#!/usr/bin/env python3
"""Compare IFR effect-name parsing with original x86 and actual native C.

Controlled CRT streams expose call order, position, EOF and ownership. A second
native mode uses real stdio on synthetic local IFR records. Strings fit the
original 128-byte local buffer; undefined oversized/indeterminate short-header
paths and external IForce DLL acceptance are outside this verifier's claim.
Requires Unicorn and the repository's scoped 32-bit MSVC/MinGW toolchains.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import struct
import subprocess
import sys

from verify_tinted_native_reconstruction import extract

ROOT = Path(__file__).resolve().parents[1]
HASH = '9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
SOURCE = ROOT / 'src/carma2/platform/win32/win32_dinput.c'
p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--original', type=Path, default=ROOT/'build-reference/originals/CARMA2_HW.EXE')
p.add_argument('--symbols', type=Path)
p.add_argument('--recompiled', type=Path, default=ROOT/'build-matching/rec2.exe')
p.add_argument('--dependency-dir', type=Path, default=ROOT/'build-audit/joystick/python-deps')
p.add_argument('--gcc', type=Path, default=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe')
p.add_argument('--msvc-root', type=Path, default=ROOT/'build-tools/msvc-modern')
p.add_argument('--output', type=Path, default=ROOT/'build-audit/iforce-names')
args = p.parse_args()
sys.path.insert(0, str(args.dependency_dir.resolve()))
from unicorn import Uc, UcError, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX, UC_X86_REG_ESP, UC_X86_REG_EIP

original_binary = args.original.read_bytes()
assert hashlib.sha256(original_binary).hexdigest() == HASH
images = [(original_binary, {})]
if args.symbols:
    mapping = json.loads(args.symbols.read_text())
    rebuilt = args.recompiled.read_bytes()
    assert mapping['format'] == 1 and mapping['original_sha256'] == HASH
    assert mapping['recompiled_sha256'] == hashlib.sha256(rebuilt).hexdigest(), 'Stale symbol map'
    images.append((rebuilt, {s['orig']: s['recomp'] for s in mapping['symbols']}))
out = args.output.resolve()
out.mkdir(parents=True, exist_ok=True)
body = extract(SOURCE.read_text(), 'ReadIforceEffectsNames')

driver = r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "c2_hooks.h"
static unsigned char data[16384],allocated[24][256];
static char* names[24];
static int length,position,at_eof,opened,nalloc,missing,events,real_mode;
static FILE* real_file;
static void note(const char*fmt,...) {va_list a;if(real_mode)return;if(events++)printf(",");va_start(a,fmt);vprintf(fmt,a);va_end(a);}
static void hex(const unsigned char*s,size_t n) {size_t i;putchar('"');for(i=0;i<n;i++)printf("%02x",s[i]);putchar('"');}
static FILE* probe_open(const char*path,const char*mode) {
 if(real_mode){real_file=fopen(path,mode);opened=real_file!=NULL;return real_file;}
 if(strcmp(path,"test.ifr")||strcmp(mode,"rb"))exit(81);
 note("[\"open\"]");opened=!missing;return missing?NULL:(FILE*)data;
}
static void probe_rewind(FILE*f) {if(real_mode){rewind(f);return;}if(f!=(FILE*)data)exit(82);position=at_eof=0;note("[\"rewind\"]");}
static size_t probe_read(void*dst,size_t size,size_t count,FILE*f) {
 int n;if(real_mode)return fread(dst,size,count,f);if(f!=(FILE*)data||size!=1||count!=5)exit(83);
 n=length-position;if(n<0)n=0;if(n>5)n=5;memcpy(dst,data+position,n);position+=n;at_eof=n<5;
 note("[\"read\",5,%d]",n);return n;
}
static int probe_compare(const char*a,const char*b,size_t n) {
 if(!real_mode){if(n!=4||strcmp(b,"ifpr"))exit(84);note("[\"signature\",\"%02x%02x%02x%02x\"]",(unsigned char)a[0],(unsigned char)a[1],(unsigned char)a[2],(unsigned char)a[3]);}
 return strncmp(a,b,n);
}
static int probe_seek(FILE*f,long offset,int origin) {
 if(real_mode)return fseek(f,offset,origin);
 if(f!=(FILE*)data||origin!=SEEK_CUR)exit(85);
 position+=(int)offset;at_eof=0;note("[\"seek\",%ld,%d]",offset,position);return 0;
}
static int probe_get(FILE*f) {
 int value,old=position;if(real_mode)return fgetc(f);if(f!=(FILE*)data)exit(86);
 if(position>=length){at_eof=1;value=EOF;}else value=data[position++];
 note("[\"getc\",%d,%d]",old,value);return value;
}
static int probe_unget(int c,FILE*f) {
 int value;if(real_mode)return ungetc(c,f);if(f!=(FILE*)data)exit(87);
 if(c==EOF)value=EOF;else{value=(unsigned char)c;position--;at_eof=0;}
 note("[\"ungetc\",%d,%d,%d]",c,value,position);return value;
}
static int probe_eof(FILE*f) {if(real_mode)return feof(f);if(f!=(FILE*)data)exit(88);return at_eof;}
static void* probe_alloc(size_t n) {
 unsigned char*r;if(nalloc>=24||n>256)exit(89);r=allocated[nalloc++];memset(r,0xa5,256);
 note("[\"alloc\",%lu,%u]",(unsigned long)n,0x1050000+nalloc*0x1000);return r;
}
#define fopen probe_open
#define rewind probe_rewind
#define fread probe_read
#define strncmp probe_compare
#define fseek probe_seek
#define fgetc probe_get
#ifdef feof
#undef feof
#endif
#define feof probe_eof
#define ungetc probe_unget
#define malloc probe_alloc
'''+body+r'''
#undef fopen
#undef rewind
#undef fread
#undef strncmp
#undef fseek
#undef fgetc
#undef feof
#undef ungetc
#undef malloc
int main(int argc,char**argv) {
 FILE*f;int result,i;if(argc!=5)return 2;
 missing=atoi(argv[3]);real_mode=atoi(argv[4]);
 f=fopen(argv[1],"rb");if(!f)return 3;length=(int)fread(data,1,sizeof(data),f);fclose(f);
 printf("{\"trace\":[");result=ReadIforceEffectsNames(real_mode?(missing?"missing-iforce-fixture.ifr":argv[1]):"test.ifr",names,atoi(argv[2]));
 if(real_mode&&real_file){position=(int)ftell(real_file);at_eof=!!feof(real_file);}
 printf("],\"status\":\"return\",\"result\":%d,\"names\":[",result);
 for(i=0;i<nalloc;i++){if(i)putchar(',');hex(allocated[i],strlen((char*)allocated[i]));}
 printf("],\"allocation_bytes\":[");for(i=0;i<nalloc;i++){if(i)putchar(',');hex(allocated[i],256);}
 printf("],\"slots\":[");for(i=0;i<24;i++){int j,v=0;if(i)putchar(',');for(j=0;j<nalloc;j++)if(names[i]==(char*)allocated[j])v=0x1050000+(j+1)*0x1000;printf("%d",v);}
 printf("],\"state\":{\"position\":%d,\"allocations\":%d,\"open\":%s,\"eof\":%s}}\n",position,nalloc,opened?"true":"false",at_eof?"true":"false");
 if(real_file)fclose(real_file);
 return 0;
}
'''
harness = out/'harness.c'
harness.write_text(driver)
msvc = sorted((args.msvc_root/'VC/Tools/MSVC').iterdir())[-1]
sdk = args.msvc_root/'Windows Kits/10'
version = sorted(p.name for p in (sdk/'Include').iterdir())[-1]
env = dict(os.environ)
env['PATH'] = str(msvc/'bin/Hostx64/x86')+os.pathsep+env['PATH']
env['INCLUDE'] = os.pathsep.join(map(str,[msvc/'include',*[sdk/f'Include/{version}/{p}' for p in ('ucrt','shared','um')]]))
env['LIB'] = os.pathsep.join(map(str,[msvc/'lib/x86',*[sdk/f'Lib/{version}/{p}/x86' for p in ('ucrt','um')]]))
commands = {
 'msvc':[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS','/I'+str(ROOT/'src/carma2/hooks'),str(harness),'/Fo'+str(out/'msvc.obj'),'/Fe'+str(out/'msvc.exe')],
 'mingw':[str(args.gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-I'+str(ROOT/'src/carma2/hooks'),str(harness),'-o',str(out/'mingw.exe')],
}
for compiler,command in commands.items():
    result = subprocess.run(command,env=env if compiler=='msvc' else None,text=True,capture_output=True)
    (out/f'{compiler}-build.log').write_text(result.stdout+result.stderr)
    assert result.returncode==0,result.stdout+result.stderr

def original(data,capacity=20,image=None,addresses=None):
 b=original_binary if image is None else image
 addresses=addresses or {}
 reverse={v:k for k,v in addresses.items()}
 pe=struct.unpack_from('<I',b,0x3c)[0];sect=pe+24+struct.unpack_from('<H',b,pe+20)[0]
 fail_alloc=0
 u=Uc(UC_ARCH_X86,UC_MODE_32);u.mem_map(0x400000,0x400000);u.mem_map(0x1000000,0x100000)
 for i in range(struct.unpack_from('<H',b,pe+6)[0]):
  _,va,size,raw=struct.unpack_from('<IIII',b,sect+i*40+8);u.mem_write(0x400000+va,b[raw:raw+size])
 def wr(a,v):u.mem_write(a,struct.pack('<I',v&0xffffffff))
 def rd(a):return struct.unpack('<I',u.mem_read(a,4))[0]
 def string(a):
  s=bytearray()
  while (v:=u.mem_read(a+len(s),1)[0]):s.append(v)
  return bytes(s)
 def ret(value=0):
  sp=u.reg_read(UC_X86_REG_ESP);target=rd(sp);u.reg_write(UC_X86_REG_EAX,value&0xffffffff);u.reg_write(UC_X86_REG_ESP,sp+4);u.reg_write(UC_X86_REG_EIP,target)
 trace=[];state={'position':0,'allocations':0,'open':0,'eof':False};allocated=[]
 def eof(v):state['eof']=v;wr(0x104000c,0x10 if v else 0)
 def hook(uc,ip,size,ctx):
  ip=reverse.get(ip,ip)
  sp=uc.reg_read(UC_X86_REG_ESP)
  if ip==0x1080001:uc.emu_stop();return
  if ip==0x577170:
   assert string(rd(sp+4))==b'test.ifr' and string(rd(sp+8))==b'rb';trace.append(['open']);state['open']=data is not None;ret(0x1040000 if data is not None else 0)
  elif ip==0x5770e0:state['position']=0;eof(False);trace.append(['rewind']);ret()
  elif ip==0x576fa0:
   ptr,size,count,file=[rd(sp+i) for i in (4,8,12,16)];assert (size,count,file)==(1,5,0x1040000)
   chunk=data[state['position']:state['position']+5];u.mem_write(ptr,chunk);state['position']+=len(chunk);eof(len(chunk)<5);trace.append(['read',5,len(chunk)]);ret(len(chunk))
  elif ip==0x576f60:
   ptr,sig,n=[rd(sp+i) for i in (4,8,12)];assert n==4 and string(sig)==b'ifpr';trace.append(['signature',bytes(u.mem_read(ptr,4)).hex()]);ret(0 if bytes(u.mem_read(ptr,4))==b'ifpr' else 1)
  elif ip==0x576ec0:
   assert rd(sp+4)==0x1040000 and rd(sp+12)==1
   offset=rd(sp+8);offset=offset-0x100000000 if offset>=0x80000000 else offset
   state['position']+=offset;eof(False);trace.append(['seek',offset,state['position']]);ret()
  elif ip==0x576e80:
   assert rd(sp+4)==0x1040000
   pos=state['position']
   if pos>=len(data):eof(True);value=-1
   else:value=data[pos];state['position']+=1
   trace.append(['getc',pos,value]);ret(value)
  elif ip==0x576df0:
   ch=rd(sp+4);ch=ch-0x100000000 if ch>=0x80000000 else ch
   assert rd(sp+8)==0x1040000
   if ch==-1:value=-1
   else:value=ch&255;state['position']-=1;eof(False)
   trace.append(['ungetc',ch,value,state['position']]);ret(value)
  elif ip==0x576d40:
   size=rd(sp+4);state['allocations']+=1;ptr=0 if state['allocations']==fail_alloc else 0x1050000+state['allocations']*0x1000
   trace.append(['alloc',size,ptr]);allocated.append(ptr)
   if ptr:u.mem_write(ptr,b'\xa5'*256)
   ret(ptr)
  elif ip==0x577640:state['open']=False;trace.append(['close']);ret()
 u.hook_add(UC_HOOK_CODE,hook)
 u.mem_write(0x103f000,b'\xa5'*0x1000);u.mem_write(0x1048000,b'test.ifr\0')
 wr(0x103ff00,0x1080001);wr(0x103ff04,capacity);u.reg_write(UC_X86_REG_ESP,0x103ff00);u.reg_write(UC_X86_REG_ECX,0x1048000);u.reg_write(UC_X86_REG_EDX,0x1041000)
 status='return'
 try:u.emu_start(addresses.get(0x45a390,0x45a390),0x1080001,count=500000)
 except UcError:status='fault'
 if status=='return' and u.reg_read(UC_X86_REG_EIP)!=0x1080001:status='instruction-limit'
 result=u.reg_read(UC_X86_REG_EAX);result=result-0x100000000 if result>=0x80000000 else result
 names=[string(ptr).hex() if ptr else None for ptr in allocated]
 return dict(status=status,result=result if status=='return' else None,names=names,state=state,trace=trace,allocation_bytes=[bytes(u.mem_read(ptr,256)).hex() for ptr in allocated],slots=[rd(0x1041000+4*i) for i in range(24)])

def record(name, metadata=b'ignored', tail=(b'attribute',b''), padding=0):
    return name+b'\0'+metadata+b'\0'+b'D'*47+b''.join(s+b'\0' for s in tail)+b'\0'*padding

header=b'ifpr'+b'\x11'*8
one=header+record(b'Pulse')
two=header+record(b'Pulse',padding=5)+record(b'Bump')
cases=[('missing',None,20),('signature',b'bad!'+b'\x11'*8,20),('header',header,20),('four-byte-header',b'ifpr',20),('empty-name',header+record(b''),20),('two',two,20),('high-name',header+record(b'P\x80\xff'),20),('lookahead-ff',header+record(b'Pulse')+b'\xff'+record(b'Bump'),20)]
cases += [(f'capacity-{n}',two,n) for n in (-1,0,1,2,3,20)]
cases += [(f'name-{n}',header+record(b'A'*n),20) for n in (1,2,31,63,126,127)]
cases += [(f'metadata-{n}',header+record(b'Pulse',b'M'*n,(b'T'*n,b'')),20) for n in (0,1,63,127)]
cases += [(f'padding-{n}',header+record(b'A',padding=n)+record(b'B'),20) for n in (0,1,2,19)]
cases += [('many-attributes',header+record(b'Pulse',tail=(b'a',b'bb',b'ccc',b'')),20),('high-lookahead',header+record(b'Pulse')+record(b'\x80Bump'),20)]
cases += [(f'partial-{n}',one[:n],20) for n in (5,11,12,13,17,18,20,30,66,70,len(one)-1)]
cases += [('twenty',header+b''.join(record(f'effect-{i}'.encode(),padding=i%3) for i in range(20)),20)]
results=[]
for label,data,capacity in cases:
    expected=original(data,capacity)
    assert expected['status']=='return',(label,expected)
    fixture=out/(label+'.ifr');fixture.write_bytes(data or b'')
    for image,addresses in images[1:]:
        actual=original(data,capacity,image=image,addresses=addresses)
        assert actual==expected,(label,'matching PE',actual,expected)
        results.append({'case':label,'mode':'matching PE','passed':True})
    for compiler in commands:
        for mode in (0,1):
            actual=json.loads(subprocess.check_output([str(out/f'{compiler}.exe'),str(fixture),str(capacity),str(int(data is None)),str(mode)],text=True))
            wanted=expected if not mode else {**expected,'trace':[]}
            if actual!=wanted:
                failure=out/f'failure-{compiler}-{label}-{mode}.json'
                failure.write_text(json.dumps({'expected':wanted,'actual':actual},indent=2))
                raise AssertionError(f'IFR comparison differs: {failure}')
            results.append({'case':label,'mode':f'{compiler} '+('real stdio' if mode else 'CRT fixture'),'passed':True})
(out/'verification.json').write_text(json.dumps({'original_sha256':HASH,'source_sha256':hashlib.sha256(SOURCE.read_bytes()).hexdigest(),'cases':len(cases),'comparisons':len(results),'results':results},indent=2)+'\n')
print(f'PASS: {len(cases)} IFR parser cases, {len(results)} original/native/real-stdio comparisons.')
