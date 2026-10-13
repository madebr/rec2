#!/usr/bin/env python3
"""Physical original/native/linked proof of the complete rest-state consumer.

FUN_004baa00,1146 original bytes: FASTCALL ECX object-list, RET0, full EAX0/1.
Actual original and production matrix/vector closure, strict32768-byte arena
and full32-bit return atPC53, ESP/nonvolatile/x87TOP on every return. No
callbacks are mocked; no masks or oracle normalization. Native production
extraction is default; --candidate is development-only. Hash-bound optional
matching PE/MAP graph executes physically with only read-only operands moved.
Includes all1061 retained finite, hierarchy, flags, IEEE-pair, overlap, angular,
gravity and corner scalar-spill scenes. No invalid/cyclic object graphs,
unmasked exceptions/sticky-status or whole-game claim. Numeric name remains
neutral because original lexical name is not established.
"""
import argparse,ast,hashlib,json,os,random,re,struct,subprocess,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parent
while not (ROOT/'src').is_dir():ROOT=ROOT.parent
sys.path.insert(0,str(ROOT/'scripts'));sys.path.insert(0,str(ROOT/'build-audit/joystick/python-deps'))
from capstone import *
from verify_tinted_native_reconstruction import extract
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',default='build-audit/physics-rest-state-tests');p.add_argument('--candidate',type=Path);p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);p.add_argument('--case-start',type=int,default=0);p.add_argument('--case-limit',type=int);p.add_argument('--max-failures',type=int,default=20);args=p.parse_args();assert bool(args.recompiled)==bool(args.symbols),"Supply both immutable PE and symbols"
OUT=ROOT/args.output;OUT.mkdir(parents=True,exist_ok=True)
source=args.candidate.resolve()if args.candidate else ROOT/'src/carma2/newcommon/49-physics.c'
source_file_text=source.read_text();before=hashlib.sha256(source.read_bytes()).hexdigest()
helper_names=['FrictionScalar','FrictionDivideScalars','RestScalar','RestByte','RestSetByte']
def extract_consumer(text):
 pieces=['#if defined(_MSC_VER) && _MSC_VER < 1300\n#pragma auto_inline(off)\n#endif']
 pieces.extend(extract(text,n)for n in helper_names[:2]);pieces.append('#if defined(_MSC_VER) && _MSC_VER < 1300\n#pragma auto_inline(on)\n#endif')
 pieces.extend(extract(text,n)for n in helper_names[2:]+['FUN_004baa00'])
 return '\n'.join(pieces)
text=extract_consumer(source_file_text)
for name in helper_names[:2]:assert extract(source_file_text,name)==extract((ROOT/'src/carma2/newcommon/49-physics.c').read_text(),name),name

guard_paths=[source,Path(__file__),*[ROOT/q for q in ['src/carma2/newcommon/49-physics.c','src/carma2/newcommon/49-physics.h','src/carma2/include/rec2_types.h','src/carma2/hooks/c2_hooks.h','src/brender/core/math/matrix34.c','src/brender/core/math/matrix34.h','src/brender/core/math/vector.c','src/brender/include/brender/brender.h','src/brender/include/brender/br_types.h','scripts/verify_physics_force_leaves_reconstruction.py','scripts/verify_opponent_navigation_reconstruction.py','scripts/verify_tinted_native_reconstruction.py']]]
if args.recompiled:guard_paths.extend([args.recompiled,args.recompiled.with_suffix('.map'),args.symbols])
source_before={str(q):hashlib.sha256(q.read_bytes()).hexdigest()for q in guard_paths}
image=(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes();assert hashlib.sha256(image).hexdigest()=='9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
nt=struct.unpack_from('<I',image,60)[0];tab=nt+24+struct.unpack_from('<H',image,nt+20)[0]
def raw(a,z):
 for n in range(struct.unpack_from('<H',image,nt+6)[0]):
  _,r,s,p=struct.unpack_from('<IIII',image,tab+40*n+8)
  if 0x400000+r<=a<0x400000+r+s:return image[p+a-0x400000-r:p+a-0x400000-r+z]
 raise ValueError(hex(a))
cat={0x4baa00:{'body_bytes':1146},0x534590:{'body_bytes':25},0x5325d0:{'body_bytes':79},0x5334a0:{'body_bytes':116}}
spec=[('FUN_004baa00',0x4baa00),('BrVector3SetFloat',0x534590),('BrMatrix34Copy',0x5325d0),('BrMatrix34ApplyP',0x5334a0)]
cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;arrays=[];patches=[];constants={};placements={a:2048*i for i,(_,a)in enumerate(spec)}
for i,(name,a)in enumerate(spec):
 b=raw(a,cat[a]['body_bytes']);ins=list(cs.disasm(b,a));assert sum(x.size for x in ins)==len(b)
 arrays.append('static unsigned char code%d[]={%s};'%(i,','.join(str(v)for v in b)))
 for x in ins:
  if x.mnemonic=='call':
   target=x.operands[0].imm;assert target in placements
   patches.append('v=(unsigned)(uintptr_t)(p+%d)-(unsigned)(uintptr_t)(p+%d);memcpy(p+%d,&v,4);'%(placements[target],placements[a]+x.address-a+x.size,placements[a]+x.address-a+x.imm_offset))
  for operand in x.operands:
   if operand.type==CS_OP_MEM and not operand.mem.base and not operand.mem.index:
    at=operand.mem.disp;width=operand.size;key=(at,width)
    assert at in [0x58a768,0x58a7a0,0x58a7a8,0x58a7b0,0x58a7b4,0x58a7b8,0x58a7c0]
    if key not in constants:constants[key]=16000+len(constants)*16
    patches.append('v=(unsigned)(uintptr_t)(p+%d);memcpy(p+%d,&v,4);'%(constants[key],placements[a]+x.address-a+x.disp_offset))
common=r'''
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <math.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "49-physics.h"
#include "matrix34.h"
#define A(r,c) A->m[r][c]
#define B(r,c) B->m[r][c]
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison,builder_cdecl;
extern unsigned __cdecl BuilderInvoke(void);
typedef char original_layouts[sizeof(float)==4&&sizeof(br_matrix34)==48&&offsetof(br_actor,t)==40&&offsetof(tPhysics_object,I)==8&&offsetof(tPhysics_object,cmpos)==0x14&&offsetof(tPhysics_object,v)==0x68&&offsetof(tPhysics_object,omega)==0x74&&offsetof(tPhysics_object,pos)==0x80&&offsetof(tPhysics_object,transform_matrix)==0x8c&&offsetof(tPhysics_object,flags)==0x19c&&offsetof(tPhysics_object,field_0x1df)==0x1df&&offsetof(tPhysics_object,field_0x21c)==0x21c&&offsetof(tPhysics_object,next)==0x220&&offsetof(tPhysics_object,child)==0x224?1:-1];
static unsigned header[4];
static unsigned char*arena;
typedef int (C2_HOOK_FASTCALL *rest_fn)(tPhysics_object*);
'''
run=r'''
static int C2_HOOK_CDECL run(const char*path,rest_fn fn){FILE*f;unsigned result;f=fopen(path,"rb");if(!f)return 3;arena=VirtualAlloc((void*)0x20000000,32768,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(arena!=(void*)0x20000000)return 4;if(fread(header,sizeof(header),1,f)!=1||fread(arena,32768,1,f)!=1)return 5;fclose(f);_controlfp(_PC_53,_MCW_PC);_setmode(_fileno(stdout),_O_BINARY);builder_mode=0;builder_poison=0xcccccccc;builder_address=(unsigned)(uintptr_t)fn;builder_x=header[0]?(unsigned)(uintptr_t)(arena+header[0]):0;builder_y=0;
#if defined(REC2_MATCHING)||defined(PROBE_FASTCALL)
 builder_cdecl=0;builder_stack_count=0;
#else
 builder_cdecl=1;builder_stack_count=1;builder_stack[0]=builder_x;
#endif
 if(BuilderInvoke())return 14;result=builder_result;fwrite(&result,4,1,stdout);fwrite(arena,32768,1,stdout);return 0;}
'''
mt=(ROOT/'src/brender/core/math/matrix34.c').read_text();vt=(ROOT/'src/brender/core/math/vector.c').read_text()
math='\n'.join(extract(mt,n)for n in ['BrMatrix34Copy','BrMatrix34ApplyP'])+'\n'+extract(vt,'BrVector3SetFloat')
(OUT/'harness.c').write_text(common+math+'\n'+text+run+'\nint C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 2;return run(argv[1],FUN_004baa00);}\n')
init='unsigned char*p;unsigned v;\np=VirtualAlloc(NULL,32768,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!p)return 3;\n'
for i in range(len(spec)):init+='memcpy(p+%d,code%d,sizeof(code%d));\n'%(i*2048,i,i)
for (a,w),off in constants.items():init+='memcpy(p+%d,(unsigned char[]){%s},%d);\n'%(off,','.join(str(v)for v in raw(a,w)),w)
init+='\n'.join(patches)
(OUT/'original.c').write_text('#define PROBE_FASTCALL\n'+common.replace('C2_HOOK_FASTCALL *','__fastcall *')+'\n'.join(arrays)+run+'\nint C2_HOOK_CDECL main(int argc,char**argv){'+init+'if(argc!=2)return 2;return run(argv[1],(rest_fn)p);}\n')

linked_meta=None
if args.recompiled:
 data=args.recompiled.read_bytes();mapping=json.loads(args.symbols.read_text());original_hash=hashlib.sha256(image).hexdigest()
 assert mapping['format']==1 and mapping['original_sha256']==original_hash and mapping['recompiled_sha256']==hashlib.sha256(data).hexdigest()
 symbols={row['orig']:row['recomp']for row in mapping['symbols']};map_bytes=args.recompiled.with_suffix('.map').read_bytes()
 functions={int(address,16):(name,owner.strip())for name,address,owner in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',map_bytes.decode('latin1'),re.M)}
 nt2=struct.unpack_from('<I',data,60)[0];base2=struct.unpack_from('<I',data,nt2+52)[0];size2=struct.unpack_from('<I',data,nt2+80)[0];tab2=nt2+24+struct.unpack_from('<H',data,nt2+20)[0]
 flat=bytearray(size2);sections=[]
 for si in range(struct.unpack_from('<H',data,nt2+6)[0]):
  virtual,rva,n,offset=struct.unpack_from('<IIII',data,tab2+40*si+8);flags=struct.unpack_from('<I',data,tab2+40*si+36)[0];flat[rva:rva+n]=data[offset:offset+n];sections.append((base2+rva,max(virtual,n),flags))
 initial={symbols[address]:name for name,address in spec};todo=[symbols[0x4baa00]];seen=set();linked_closure=[];patched=set();relocations=[]
 while todo:
  start=todo.pop(0)
  if start in seen:continue
  seen.add(start);assert start in functions;end=min(address for address in functions if address>start);body=bytes(flat[start-base2:end-base2]);instructions=list(cs.disasm(body,start));assert sum(i.size for i in instructions)==len(body)
  name=initial.get(start,functions[start][0])
  for ins in instructions:
   if ins.mnemonic=='call'or ins.mnemonic.startswith('j'):
    assert ins.operands[0].type==CS_OP_IMM,(name,ins.op_str)
    target=ins.operands[0].imm
    if not start<=target<end:
     assert target in functions,(name,ins.op_str)
     if target not in initial:assert functions[target][1]=='49-physics.c.obj'and functions[target][0].lstrip('_@').split('@')[0]in helper_names,(name,functions[target])
     todo.append(target)
   for op in ins.operands:
    if op.type==CS_OP_MEM and base2<=op.mem.disp<base2+size2:
     assert not op.mem.base and not op.mem.index and ins.disp_size==4,(name,ins.op_str)
     section=next(z for z in sections if z[0]<=op.mem.disp<z[0]+z[1]);assert not section[2]&0x80000020,(name,ins.op_str)
     site=ins.address-base2+ins.disp_offset
     if site not in patched:
      relocations.append(dict(site=site,address=op.mem.disp,width=op.size,bits=bytes(flat[op.mem.disp-base2:op.mem.disp-base2+op.size]).hex()));struct.pack_into('<I',flat,site,op.mem.disp-base2+0x50000000);patched.add(site)
  linked_closure.append(dict(name=name,address=start,length=end-start,sha256=hashlib.sha256(body).hexdigest()))
 rr,rz=struct.unpack_from('<II',data,nt2+24+96+5*8);cursor=rr
 while cursor<rr+rz:
  page,n=struct.unpack_from('<II',flat,cursor);assert n>=8
  for word in struct.unpack_from('<'+'H'*((n-8)//2),flat,cursor+8):
   if word>>12==3:
    site=page+(word&4095)
    if any(row['address']-base2<=site<row['address']-base2+row['length']for row in linked_closure):assert site in patched,hex(site)
  cursor+=n
 (OUT/'matching-image.bin').write_bytes(flat)
 matching_main='int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned char*p;if(argc!=3)return 2;p=VirtualAlloc((void*)0x50000000,%d,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(p!=(void*)0x50000000)return 3;f=fopen(argv[2],"rb");if(!f)return 4;if(fread(p,%d,1,f)!=1)return 5;fclose(f);return run(argv[1],(rest_fn)(p+%d));}\n'%(size2,size2,symbols[0x4baa00]-base2)
 (OUT/'matching.c').write_text('#define PROBE_FASTCALL\n'+common.replace('C2_HOOK_FASTCALL *','__fastcall *')+run+matching_main)
 linked_meta=dict(recompiled_sha256=hashlib.sha256(data).hexdigest(),symbols_sha256=hashlib.sha256(args.symbols.read_bytes()).hexdigest(),map_sha256=hashlib.sha256(map_bytes).hexdigest(),closure=linked_closure,constant_relocations=relocations,scope='Actual complete matching rest-state, emitted private helpers and actual matrix/vector dependencies physically at0x50000000. Internal branches/relativecalls untouched; readonlydata absolute operands moved. No dynamic callbacks or masks.')

includes=[ROOT/v for v in ('src/brender/include','src/carma2/include','src/carma2/hooks','src/carma2/s3/include')]
mroot=ROOT/'build-tools/msvc-modern';msvc=sorted((mroot/'VC/Tools/MSVC').iterdir())[-1];sdk=mroot/'Windows Kits/10';version=sorted((sdk/'Include').iterdir())[-1].name
env=dict(os.environ);env['PATH']=str(msvc/'bin/Hostx64/x86')+os.pathsep+env['PATH'];env['INCLUDE']=os.pathsep.join(map(str,[msvc/'include',*[sdk/f'Include/{version}/{part}' for part in ('ucrt','shared','um')]]));env['LIB']=os.pathsep.join(map(str,[msvc/'lib/x86',*[sdk/f'Lib/{version}/{part}/x86' for part in ('ucrt','um')]]))
m5=ROOT/'build-tools/msvc500sp3';env5=dict(os.environ);env5['PATH']=str(m5/'bin')+os.pathsep+str(m5/'redist')+os.pathsep+env5['PATH'];env5['INCLUDE']=str(m5/'include');env5['LIB']=str(m5/'lib')
gcc=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'
commands={
 'msvc':[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(q)for q in includes],str(OUT/'harness.c'),'/Fo'+str(OUT/'msvc.obj'),'/Fe'+str(OUT/'msvc.exe')],
 'msvc5':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(q)for q in includes],str(OUT/'harness.c'),'/Fo'+str(OUT/'msvc5.obj'),'/Fe'+str(OUT/'msvc5.exe')],
 'mingw':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation',*['-I'+str(q)for q in includes],str(OUT/'harness.c'),'-o',str(OUT/'mingw.exe')],
 'original':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation',*['-I'+str(q)for q in includes],str(OUT/'original.c'),'-o',str(OUT/'original.exe')]}

if args.recompiled:commands['matching']=[*commands['original'][:-3],str(OUT/'matching.c'),'-o',str(OUT/'matching.exe')]
caller_tree=ast.parse((ROOT/'scripts/verify_opponent_navigation_reconstruction.py').read_text());nodes=[n.value.value for n in ast.walk(caller_tree)if isinstance(n,ast.Assign)and isinstance(n.value,ast.Constant)and any(isinstance(t,ast.Name)and t.id=='caller'for t in n.targets)];assert len(nodes)==1
caller=nodes[0].replace('builder_poison;','builder_poison,builder_cdecl;').replace(' mov builder_result,eax\n mov ecx,esp',' mov builder_result,eax\n cmp builder_cdecl,0\n je stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n stackready:\n mov ecx,esp');(OUT/'caller.c').write_text(caller)
for name,command in commands.items():
 command.append(str(OUT/'caller.obj'));command.append(('/I' if name in ['msvc','msvc5']else'-I')+str(ROOT/'src/carma2/newcommon'));command.append(('/I' if name in ['msvc','msvc5']else'-I')+str(ROOT/'src/brender/core/math'))
commands={'caller':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Zl','/c',str(OUT/'caller.c'),'/Fo'+str(OUT/'caller.obj')],**commands}
for name,cmd in commands.items():
 r=subprocess.run(cmd,cwd=OUT,env=env if name=='msvc'else env5 if name in ['msvc5','caller']else None,capture_output=True,text=True);(OUT/(name+'-build.log')).write_text(r.stdout+r.stderr)
 if r.returncode:raise RuntimeError(r.stdout+r.stderr)
cases=[]
def scene(label,patch=(),layout='single'):
 a=bytearray(b'\xa5'*32768)
 def u(off,value):struct.pack_into('<I',a,off,value&0xffffffff)
 def floats(off,values):struct.pack_into('<'+'f'*len(values),a,off,*values)
 nodes=[0x1000]if layout=='single'else[0x1000,0x1800,0x2000]
 for j,off in enumerate(nodes):
  a[off:off+0x270]=bytes(0x270);u(off,0x20004000+j*0x400);floats(off+4,[2,1,1.5,2,0.125,-.0625,.25]);floats(off+0x24,[-1,-2,-3,1,2,3]);floats(off+0x54,[0,-9.8,0]);floats(off+0x68,[0.001,-.001,0.001]);floats(off+0x74,[.001,.0002,-.0004]);floats(off+0x1b4,[0,-.02,0]);floats(off+0x21c,[1]);floats(0x4000+j*0x400+44,[1,0,0,0,1,0,0,0,1,3,4,5]);floats(off+0x8c,[1,0,0,0,1,0,0,0,1,3,4,5])
 if layout=='siblings':u(0x1000+0x220,0x20001800);u(0x1800+0x220,0x20002000)
 if layout=='children':u(0x1000+0x224,0x20001800);u(0x1800+0x220,0x20002000)
 for at,val,width in patch:
  if width==1:a[at]=val
  else:u(at,val)
 cases.append(dict(label=label,header=[0x1000,0,0,0],data=bytes(a)))
scene('null');cases[-1]['header'][0]=0
for layout in ['single','siblings','children']:scene('finite-'+layout,layout=layout)
for state in [0,1,2,3,255]:
 for flag in [0,0x20,0x100,0x200,0x320]:scene('state-%d-flags-%x'%(state,flag),[(0x1000+0x1df,state,1),(0x1000+0x19c,flag,4),(0x1000+0xf0,1,4)])
rng=random.Random(0x4baa00)
for i in range(150):
 patch=[]
 for off in [4,8,12,16,0x54,0x58,0x5c,0x68,0x6c,0x70,0x74,0x78,0x7c,0x1cc,0x1d0,0x1d4,0x21c]:
  val=rng.uniform(.5,2)if off in [4,8,12,16,0x21c]else rng.uniform(-.002,.002)
  patch.append((0x1000+off,struct.unpack('<I',struct.pack('<f',val))[0],4))
 scene('finite-random-%d'%i,patch)

def word(value):return struct.unpack('<I',struct.pack('<f',value))[0]
def fp(offset,value):return (0x1000+offset,word(value),4)
# Explicit positive-rest path and complete hierarchy combinations.
rest=[fp(0x1b8,-10),fp(0x68,0),fp(0x6c,0),fp(0x70,0),fp(0x74,0),fp(0x78,0),fp(0x7c,0)]
scene('eligible-rest-reset',rest)
for layout in ['siblings','children']:
 for state in [0,1,2,3,255]:
  scene('hierarchy-'+layout+'-'+str(state),rest+[(0x1000+0x1df,state,1)],layout)
for state in [1,2,3,255]:
 for group in [0,0x20001000,0x20001800]:scene('prior-rest-%d-group-%x'%(state,group),rest+[(0x1000+0x1df,state,1),(0x1000+0x1d8,group,4)])
for collision in [0,1,255]:
 for secondary in [0,0x20006000]:
  for orient in [0.,.999,1.]:scene('eligibility-%d-%x-%s'%(collision,secondary,orient),rest+[(0x1000+0x1dd,collision,1),(0x1000+0x18c,secondary,4),fp(0x9c,orient)])
# Individual corner extrema force all-eight traversal vs an early corner exit.
for component in [0,1,2]:
 for axis in [0,1,2]:
  for boundary in [.0001,.001,.01,.1]:
   patch=rest+[fp(0x74+component*4,boundary),fp(0x24+axis*4,-16),fp(0x30+axis*4,16)]
   scene('corner-%d-%d-%s'%(component,axis,boundary),patch)
for offset,value in [(0x68,.001**.5),(0x74,(.00009*2)**.5),(0x6c,.012),(0x1b8,-8.82),(0x1a8,.0004**.5),(0x9c,.999),(0x21c,1.)]:
 w=word(value)
 for delta in [-2,-1,0,1,2]:scene('threshold-%x-%d'%(offset,delta),rest+[(0x1000+offset,w+delta,4)])
for source_offset in [0x74,0x80,0x88,0x8c,0x90,0x98,0x1e8]:
 scene('actor-matrix-source-alias-%x'%source_offset,rest+[(0x1000,0x20001000+source_offset-44,4)])
special=[0,0x80000000,1,0x80000001,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7f800001,0xff800001,0x7fc00011,0xffc00011,0x7fc12345,0xffc12345]
slots=[4,8,12,16,0x14,0x18,0x1c,0x24,0x28,0x2c,0x30,0x34,0x38,0x54,0x58,0x5c,0x68,0x6c,0x70,0x74,0x78,0x7c,0x1a8,0x1ac,0x1b0,0x1b4,0x1b8,0x1bc,0x1cc,0x1d0,0x1d4,0x21c]
for offset in slots:
 for w in special:scene('raw-%x-%08x'%(offset,w),rest+[(0x1000+offset,w,4)])
for left,right in [(0x1cc,0x74),(0x1d0,0x78),(0x1d4,0x7c),(8,0x74),(0x54,0x58),(0x68,0x6c),(0x1b8,0x6c),(8,0x21c),(0x74,0x78),(0x74,0x14),(0x78,0x1c),(0x7c,0x18),(0x78,0x38),(0x7c,0x34)]:
 for a,b in [(0x7f800011,0xff800022),(0x7f800022,0xffc00011),(0x7fc00011,0xff800022),(0x7fc00011,0xffc00022),(0x7f800011,0xff800011),(0xff800011,0x7f800022)]:scene('paired-%x-%x-%08x-%08x'%(left,right,a,b),rest+[(0x1000+left,a,4),(0x1000+right,b,4)])


# Original FST32 copies are still consumed from rounded memory while ST0 stays wide.
for axis in [0,2]:
 for sign in [-1,1]:
  for isum in range(-3,4):
   for half in [-1,0,1]:
    patch=rest+[fp(4,1),fp(0x74+axis*4,sign),fp(0x1cc+axis*4,sign*(2**-24)),(0x1000+8+axis*4,word(.00009)+isum,4),(0x1000+0x1df,1,1)]
    patch[-3]=(0x1000+0x1cc+axis*4,word(sign*(2**-24))+half,4)
    scene('angular-spill-%d-%d-%d-%d'%(axis,sign,isum,half),patch)
for gravity in [(1.2345,4.5678,-.321),(9.125,-7.4375,3.0625),(1,2**-12,0),(0,-9.8,0),(.001,.002,.003)]:
 for axis in range(3):
  for delta in range(-3,4):
   patch=rest+[fp(0x54+4*i,gravity[i])for i in range(3)]+[fp(0x1b4+4*i,gravity[i]*.9)for i in range(3)]
   patch.append((0x1000+0x1b4+4*axis,word(gravity[axis]*.9)+delta,4))
   scene('gravity-spill-%s-%d-%d'%(gravity,axis,delta),patch)


for component in [0,1,2]:
 for direction in [-1,1]:
  for impulse in [1e-10,2e-10,3e-10,4e-10,5e-10,7e-10,1e-9]:
   patch=rest+[fp(0x1a8+component*4,direction*.02)]
   if component==0:patch.append(fp(0x78,direction*impulse))
   elif component==1:patch.append(fp(0x7c,direction*impulse))
   else:patch.append(fp(0x74,direction*impulse))
   scene('point-spill-%d-%d-%s'%(component,direction,impulse),patch)

results=[];fail=[]
selected=cases[args.case_start:args.case_start+args.case_limit if args.case_limit is not None else None]
for index,c in enumerate(selected,args.case_start):
 f=OUT/'input.bin';f.write_bytes(struct.pack('<4I',*c['header'])+c['data']);oracle=subprocess.run([str(OUT/'original.exe'),str(f)],capture_output=True);assert oracle.returncode==0 and len(oracle.stdout)==32772,(index,oracle.returncode,oracle.stderr)
 for compiler in ['msvc5','msvc','mingw']+(['matching']if args.recompiled else []):
  r=subprocess.run([str(OUT/(compiler+'.exe')),str(f)]+([str(OUT/'matching-image.bin')]if compiler=='matching'else[]),capture_output=True);ok=r.returncode==0 and r.stdout==oracle.stdout;row=dict(index=index,label=c['label'],compiler=compiler,passed=ok,returncode=r.returncode)
  if not ok:
   row['differences']=[dict(offset=j,original=oracle.stdout[j:j+4].hex(),native=r.stdout[j:j+4].hex())for j in range(0,min(len(r.stdout),32772),4)if oracle.stdout[j:j+4]!=r.stdout[j:j+4]];(OUT/f'failure-{index}-{compiler}.bin').write_bytes(f.read_bytes());fail.append(row)
  results.append(row)
 if index%50==0:print('cases',index+1,'failures',len(fail),flush=True)
 if len(fail)>=args.max_failures:break
source_after={str(q):hashlib.sha256(q.read_bytes()).hexdigest()for q in guard_paths};assert source_before==source_after
assert before==hashlib.sha256(source.read_bytes()).hexdigest()
assert text==extract_consumer(source.read_text())
assert math=='\n'.join(extract((ROOT/'src/brender/core/math/matrix34.c').read_text(),n)for n in ['BrMatrix34Copy','BrMatrix34ApplyP'])+'\n'+extract((ROOT/'src/brender/core/math/vector.c').read_text(),'BrVector3SetFloat')
manifest=dict(scenario_count=len(cases),complete=len(results)==len(selected)*(4 if args.recompiled else 3),consumer_extracted_sha256=hashlib.sha256(text.encode()).hexdigest(),matching=linked_meta,total_scenes=len(cases),selected_scenes=len(selected),case_start=args.case_start,source_before=source_before,source_after=source_after,whole_sources_unchanged=source_before==source_after,generated_files_sha256={str(q):hashlib.sha256(q.read_bytes()).hexdigest()for q in OUT.iterdir()if q.suffix in ['.c','.obj','.exe']},fixture_sha256=hashlib.sha256(b''.join(struct.pack('<4I',*c['header'])+c['data']for c in cases)).hexdigest(),original_entries=[dict(name=name,address=hex(a),bytes=cat[a]['body_bytes'],sha256=hashlib.sha256(raw(a,cat[a]['body_bytes'])).hexdigest())for name,a in spec],constants={hex(a):raw(a,z).hex()for a,z in constants},executed_scenes=index+1-args.case_start,comparisons=len(results),failures=fail,results=results,candidate_sha256=before,verifier_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),original_sha256=hashlib.sha256(image).hexdigest(),spec=spec,commands=commands,scope='Complete actual original1146B rest-state body + actual original3math; native actual production closure, raw32768arena/fullEAX, directABI ESP/nonvolatile/x87TOP, PC53. No masks/normalization; terminating accessible object/actor graphs only, masked FP exceptions/sticky flags outside claim.')
(OUT/'verification.json').write_text(json.dumps(manifest,indent=2)+'\n');print('complete',index+1,len(results),'failures',len(fail))
if fail:raise SystemExit(1)
