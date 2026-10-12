"""Physical original/native3/linked proof of three complete group-limit entries.

Only supplied PhysicsError callback and CRT process exit are controlled.

Raw entire arena/global/count/ordered callbacks and direct ABI compare at PC53.

"""

import argparse,ast,hashlib,json,random,re,struct,subprocess,sys

from pathlib import Path

ROOT=next(p for p in Path(__file__).resolve().parents if(p/'src').is_dir())

sys.path.insert(0,str(ROOT/'scripts'))

from capstone import *

from verify_tinted_native_reconstruction import extract

from verify_opponent_route_reconstruction import HASH,sha,pe,raw

import verify_phil_registry_reconstruction as registry

p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--candidate',type=Path);p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);p.add_argument('--build-only',action='store_true');p.add_argument('--case-limit',type=int);p.add_argument('--case-index',type=int,action='append');p.add_argument('--max-failures',type=int,default=20);a=p.parse_args()

assert bool(a.recompiled)==bool(a.symbols),'Supply both immutable PE and symbol map'

if a.candidate is None:a.candidate=ROOT/'src/carma2/newcommon/49-physics.c'

OUT=a.output.resolve();OUT.mkdir(parents=True,exist_ok=True)

source=ROOT/'src/carma2/newcommon/49-physics.c';matrix=ROOT/'src/brender/core/math/matrix34.c';vector=ROOT/'src/brender/core/math/vector.c'

paths=[a.candidate.resolve(),Path(__file__).resolve(),source,source.with_suffix('.h'),matrix,matrix.with_suffix('.h'),vector,ROOT/'src/carma2/include/rec2_types.h',ROOT/'src/carma2/hooks/c2_hooks.h',ROOT/'src/brender/include/brender/brender.h',ROOT/'src/brender/include/brender/br_types.h',ROOT/'scripts/verify_phil_registry_reconstruction.py',ROOT/'scripts/verify_opponent_navigation_reconstruction.py',ROOT/'scripts/verify_tinted_native_reconstruction.py',ROOT/'scripts/verify_opponent_route_reconstruction.py']

if a.recompiled:paths.extend([a.recompiled.resolve(),a.recompiled.with_suffix('.map').resolve(),a.symbols.resolve()])

before={str(q):sha(q.read_bytes())for q in paths};(OUT/'candidate-snapshot.c').write_bytes(a.candidate.read_bytes());(OUT/'verifier-snapshot.py').write_bytes(Path(__file__).read_bytes())

src=source.read_text();mat=matrix.read_text();vec=vector.read_text()

helpers=['FrictionRead','FrictionWord','FrictionPointer','FrictionScalar','FrictionWrite','FrictionProductScalars','FrictionSubtractScalar','FrictionAddScalar','FrictionAddWide','PartnerSubtractWide','PartnerMultiplyRawScalar','FrictionReverseSubtractScalar']

candidate_text=a.candidate.read_text().replace('extern tU32 gPhysics_group_limit_692dd8;', '')

if a.candidate.resolve()==source.resolve():candidate_text='\n'.join(extract(src,n)for n in ['LimitScalarAt','LimitWord','LimitCopyWord','LimitActorMatrix','LimitRow','LimitRecord','LimitDot','LimitCross','FUN_004bd4f0','FUN_004bca10','FUN_004bd6d0'])

body='#if defined(_MSC_VER) && _MSC_VER < 1300\n#pragma auto_inline(off)\n#endif\n'+'\n'.join(extract(src,n)for n in helpers)+'\n'+'\n'.join(extract(mat,n)for n in ['BrMatrix34ApplyV','BrMatrix34TApplyV'])+'\n'+extract(vec,'BrVector3SetFloat')+'\n#define exit LimitExit\n'+extract(src,'PhysicsError')+'\n#undef exit\n#if defined(_MSC_VER) && _MSC_VER < 1300\n#pragma auto_inline(on)\n#endif\n'+candidate_text

spec=[('FUN_004bca10',0x4bca10,2770),('FUN_004bd4f0',0x4bd4f0,477),('FUN_004bd6d0',0x4bd6d0,74),('BrMatrix34ApplyV',0x533520,107),('BrMatrix34TApplyV',0x533690,107),('BrVector3SetFloat',0x534590,25),('PhysicsError',0x4c61c0,25)]

original=(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes();assert sha(original)==HASH

nt,base,size,table=pe(original);image=bytearray(size);sections=[]

for i in range(struct.unpack_from('<H',original,nt+6)[0]):

 v,r,n,o=struct.unpack_from('<IIII',original,table+40*i+8);flags=struct.unpack_from('<I',original,table+40*i+36)[0];image[r:r+n]=original[o:o+n];sections.append((base+r,max(v,n),flags))

cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;patches=[];edges=[];closures=[];sites=set();globals_={0x692dd8,0x6940c0,0x676920}

for name,start,n in spec:

 ins=list(cs.disasm(raw(original,start,n),start));assert sum(i.size for i in ins)==n

 for i in ins:

  control=i.mnemonic=='call'or i.mnemonic.startswith('j')

  if control:

   op=i.operands[0]

   if op.type==CS_OP_IMM:

    assert start<=op.imm<start+n or op.imm in {r[1]for r in spec}or op.imm==0x5784e0,(name,i.op_str)

    if not start<=op.imm<start+n:edges.append([i.address,op.imm])

   else:assert name=='PhysicsError'and i.mnemonic=='call',('unproven indirect',name,i.op_str)

  for op in i.operands:

   value=op.mem.disp if op.type==CS_OP_MEM else op.imm if op.type==CS_OP_IMM and not control else 0

   if base<=value<base+size:

    if op.type==CS_OP_MEM:assert not op.mem.base and not op.mem.index and i.disp_size==4;site=i.address-base+i.disp_offset;width=op.size

    else:assert i.imm_size==4;site=i.address-base+i.imm_offset;width=1

    section=next(s for s in sections if s[0]<=value<s[0]+s[1]);assert value in globals_ or not section[2]&0x80000020,(name,i.op_str)

    assert site not in sites;sites.add(site);struct.pack_into('<I',image,site,value-base+0x50000000);patches.append(dict(site=site,original=value,transport=value-base+0x50000000,width=width))

 closures.append(dict(name=name,address=start,bytes=n,sha256=sha(raw(original,start,n))))

# Prove every relocated absolute site in these complete spans is transported.

rr,rz=struct.unpack_from('<II',original,nt+24+96+5*8);cursor=rr;checked=[]

while cursor<rr+rz:

 page,n=struct.unpack('<II',raw(original,base+cursor,8));assert n>=8

 for value in struct.unpack('<'+'H'*((n-8)//2),raw(original,base+cursor+8,n-8)):

  site=page+(value&4095)

  if value>>12==3 and any(s-base<=site<s-base+z for _,s,z in spec):assert site in sites;checked.append(site)

 cursor+=n

(OUT/'physical-image.bin').write_bytes(image)

prefix=r'''

#include <windows.h>

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <stdint.h>

#include <math.h>

#include <float.h>

#include <io.h>

#include <fcntl.h>

#include "49-physics.h"

#define M(x,y) mat->m[x][y]

#define gPhysics_group_limit_692dd8 (*(tU32*)(code+0x292dd8))

#define gPhysics_error_cb (*(tPhysicsError_cbfn**)(code+0x2940c0))

#define gPhysics_empty_message (*(char*)(code+0x276920))

extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[8],builder_stack_count,builder_result,builder_poison,builder_cdecl;

extern unsigned __cdecl BuilderInvoke(void);

static unsigned char*code;static unsigned char*arena;static unsigned h[16];

typedef char LimitLayouts[sizeof(void*)==4&&sizeof(tPhysics_joint_force)==96&&sizeof(tPhysics_joint_limit)==32?1:-1];

static void LimitSnapshot(unsigned event,unsigned value,unsigned message){unsigned head[4];head[0]=event;head[1]=value;head[2]=message;head[3]=gPhysics_group_limit_692dd8;fwrite(head,sizeof(head),1,stdout);fwrite(arena,65536,1,stdout);}

static void LimitErrorAction(tPhysicsError value,const char*message){LimitSnapshot(1,(unsigned)value,(unsigned)message);if(h[12]&1)*(unsigned*)(arena+0x1000+0x224)=0x20003000;if(h[12]&2)gPhysics_group_limit_692dd8=0xfefdfcfb;}

static void __fastcall LimitOriginalError(tPhysicsError value,const char*message){LimitErrorAction(value,message);}

static void C2_HOOK_FASTCALL LimitNativeError(tPhysicsError value,const char*message){LimitErrorAction(value,message);}

static void C2_NORETURN C2_HOOK_CDECL LimitExit(int value){LimitSnapshot(2,(unsigned)value,0);exit(0);}

'''

driver=r'''

int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned relative,mode,i;void*function;unsigned x,y,stack[6],n;

if(argc!=3)return 2;SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);code=VirtualAlloc((void*)0x50000000,IMAGE_SIZE,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);arena=VirtualAlloc((void*)0x20000000,65536,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(code!=(void*)0x50000000||arena!=(void*)0x20000000)return 3;

f=fopen(argv[2],"rb");if(!f||fread(code,IMAGE_SIZE,1,f)!=1)return 4;fclose(f);f=fopen(argv[1],"rb");if(!f||fread(h,sizeof(h),1,f)!=1||fread(arena,65536,1,f)!=1)return 5;fclose(f);

gPhysics_group_limit_692dd8=h[11];gPhysics_error_cb=h[13]?(tPhysicsError_cbfn*)(code+0x3f20):NULL;code[0x3f20]=0xe9;

#ifdef PHYSICAL_ORIGINAL

relative=(unsigned)LimitOriginalError-((unsigned)code+0x3f25);(void)LimitNativeError;

#else

relative=(unsigned)LimitNativeError-((unsigned)code+0x3f25);(void)LimitOriginalError;

#endif

memcpy(code+0x3f21,&relative,4);code[0x1784e0]=0xe9;relative=(unsigned)LimitExit-((unsigned)code+0x1784e5);memcpy(code+0x1784e1,&relative,4);_controlfp(_PC_53,_MCW_PC);_clearfp();_setmode(_fileno(stdout),_O_BINARY);mode=h[0];

#ifdef PHYSICAL_ORIGINAL

{unsigned entries[3]={0xbca10,0xbd4f0,0xbd6d0};function=code+entries[mode];}

#else

function=mode==0?(void*)FUN_004bca10:mode==1?(void*)FUN_004bd4f0:(void*)FUN_004bd6d0;

#endif

x=h[1]?(unsigned)(arena+h[1]):0;y=h[2]?(unsigned)(arena+h[2]):0;n=mode==1?6:2;for(i=0;i<n;i++)stack[i]=mode==1?(unsigned)(arena+h[3+i]):h[3+i];

builder_address=(unsigned)function;builder_mode=0;builder_poison=0xcccccccc;builder_x=x;builder_y=y;

#if defined(REC2_MATCHING) || defined(PHYSICAL_ORIGINAL)

builder_cdecl=0;builder_stack_count=n;for(i=0;i<n;i++)builder_stack[i]=stack[i];

#else

builder_cdecl=1;builder_stack_count=n+2;builder_stack[0]=x;builder_stack[1]=y;for(i=0;i<n;i++)builder_stack[i+2]=stack[i];

#endif

if(BuilderInvoke())return 14;LimitSnapshot(0,mode==1?0:builder_result,0);return 0;}

'''.replace('IMAGE_SIZE',str(size))

tree=ast.parse((ROOT/'scripts/verify_opponent_navigation_reconstruction.py').read_text())

caller=next(n.value.value for n in ast.walk(tree)if isinstance(n,ast.Assign)and isinstance(n.value,ast.Constant)and any(isinstance(t,ast.Name)and t.id=='caller'for t in n.targets))

caller=caller.replace('builder_stack[5]','builder_stack[8]').replace('builder_poison;','builder_poison,builder_cdecl;').replace(' mov builder_result,eax\n mov ecx,esp',' mov builder_result,eax\n cmp builder_cdecl,0\n je stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n stackready:\n mov ecx,esp')



def linked_image(recompiled,symbols_path):

 data=recompiled.read_bytes();mapping=json.loads(symbols_path.read_text());assert mapping['format']==1 and mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(data)

 symbols={r['orig']:r['recomp']for r in mapping['symbols']};map_bytes=recompiled.with_suffix('.map').read_bytes()

 functions={int(v,16):(n,o.strip())for n,v,o in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',map_bytes.decode('latin1'),re.M)}

 _,b,z,t=pe(data);flat=bytearray(max(z,size));sections=[]

 for i in range(struct.unpack_from('<H',data,pe(data)[0]+6)[0]):

  v,r,n,o=struct.unpack_from('<IIII',data,t+40*i+8);flags=struct.unpack_from('<I',data,t+40*i+36)[0];flat[r:r+n]=data[o:o+n];sections.append((b+r,max(v,n),flags))

 def address(orig,name,owner=None):

  if orig in symbols:return symbols[orig]

  matches=[a for a,(n,o)in functions.items()if n==name and (owner is None or o.startswith(owner))];assert len(matches)==1,(orig,name,matches);return matches[0]

 exit_address=address(0x5784e0,'_exit','LIBC:')

 global_fallbacks=[]

 for orig,type_name,var_name,width in [(0x676920,'char','gPhysics_empty_message',1),(0x692dd8,'tU32','gPhysics_group_limit_692dd8',4)]:

  if orig not in symbols:

   pattern=r'// GLOBAL: CARMA2_HW 0x%08x\s+%s %s;'%(orig,type_name,var_name)

   assert re.search(pattern,src),('missing canonical original global declaration',hex(orig),var_name)

   rows=re.findall(r'^\s*0003:[0-9a-fA-F]{8}\s+_'+var_name+r'\s+([0-9a-fA-F]{8})\s+<common>\s*$',map_bytes.decode('latin1'),re.M);assert len(rows)==1,(var_name,rows)

   value=int(rows[0],16);section=next(r for r in sections if r[0]<=value<r[0]+r[1]);assert section[2]&0x80000000 and not section[2]&0x20000000

   symbols[orig]=value;global_fallbacks.append(dict(original=orig,recompiled=value,name=var_name,owner='<common>',bytes=width,authority='Exact canonical common MAP symbol, original-address annotated source declaration and writable non-code section.'))

 globals_map={symbols[0x692dd8]:0x50292dd8,symbols[0x6940c0]:0x502940c0,symbols[0x676920]:0x50276920}

 roots=[symbols[a]for _,a,_ in spec[:3]];pending=roots.copy();seen=set();graph=[];relocations=[];accounted=set()

 while pending:

  start=pending.pop(0)

  if start in seen:continue

  seen.add(start);end=min(a for a in functions if a>start);name,owner=functions[start];short=name.lstrip('_@').split('@')[0]

  work=[start];insns={};callback_loads={}

  while work:

   cursor=work.pop()

   while cursor not in insns:

    assert start<=cursor<end,(name,hex(cursor),'outside complete function')

    ins=next(cs.disasm(raw(data,cursor,15),cursor,count=1));insns[cursor]=ins.bytes.hex();control=ins.mnemonic=='call'or ins.mnemonic.startswith('j')

    if ins.mnemonic=='mov'and ins.operands[0].type==CS_OP_REG and ins.operands[1].type==CS_OP_MEM and ins.operands[1].mem.disp==symbols[0x6940c0]:callback_loads[ins.operands[0].reg]=ins.address

    for op in ins.operands:

     value=op.mem.disp if op.type==CS_OP_MEM else op.imm if op.type==CS_OP_IMM and not control else 0

     if b<=value<b+z:

      if op.type==CS_OP_MEM:assert not op.mem.base and not op.mem.index and ins.disp_size==4;site=ins.address-b+ins.disp_offset;width=op.size

      else:assert ins.imm_size==4;site=ins.address-b+ins.imm_offset;width=1

      target=globals_map.get(value)

      if target is None:

       section=next(r for r in sections if r[0]<=value<r[0]+r[1]);assert not section[2]&0x80000020,(name,ins.op_str,'unproved mutable/code absolute reference');target=value-b+0x50000000

      if site not in accounted:struct.pack_into('<I',flat,site,target);accounted.add(site);relocations.append(dict(site=site,actual=value,transport=target,width=width))

    if control:

     op=ins.operands[0]

     if op.type==CS_OP_IMM:

      target=op.imm

      if start<=target<end:work.append(target)

      elif target!=exit_address:

       assert target in functions,(name,ins.op_str,'unmapped linked callee')

       callee,calowner=functions[target];cn=callee.lstrip('_@').split('@')[0]

       allowed=target in roots or (calowner=='49-physics.c.obj'and cn.startswith(('Limit','Friction','Partner','PhysicsError')))

       allowed=allowed or (calowner.startswith(('brender:','libbrender:'))and cn in ('BrMatrix34ApplyV','BrMatrix34TApplyV','BrVector3SetFloat'))

       assert allowed,(name,functions[target],'unapproved actual lower')

       pending.append(target)

     else:assert ins.mnemonic=='call'and (short=='PhysicsError'or start in roots)and op.type==CS_OP_REG and op.reg in callback_loads and 0<ins.address-callback_loads[op.reg]<=64,(name,ins.op_str,'unproven indirect')

    if ins.mnemonic.startswith('ret')or ins.mnemonic in ('jmp','int','int3'):break

    cursor+=ins.size

  graph.append(dict(name=name,address=start,extent=end-start,instruction_count=len(insns),instruction_sha256=sha(json.dumps(sorted(insns.items())).encode())))

 ntt=pe(data)[0];rr,rz=struct.unpack_from('<II',data,ntt+24+96+5*8);cursor=rr;checked=[]

 while cursor<rr+rz:

  page,n=struct.unpack('<II',raw(data,b+cursor,8));assert n>=8

  for v in struct.unpack('<'+'H'*((n-8)//2),raw(data,b+cursor+8,n-8)):

   site=page+(v&4095)

   if v>>12==3 and any(r['address']-b<=site<r['address']-b+r['extent']for r in graph):assert site in accounted,('unaccounted relocation',hex(site));checked.append(site)

  cursor+=n

 assert not any(r['address']-b<=0x3f20<r['address']-b+r['extent']for r in graph)

 (OUT/'matching-image.bin').write_bytes(flat)

 d=driver.replace(str(size),str(len(flat))).replace('0xbca10,0xbd4f0,0xbd6d0',','.join(hex(a-b)for a in roots)).replace('0x1784e0',hex(exit_address-b)).replace('0x1784e5',hex(exit_address-b+5)).replace('0x1784e1',hex(exit_address-b+1))

 (OUT/'matching.c').write_text(prefix+'\n#define PHYSICAL_ORIGINAL\n'+d)

 gcc=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe';inc=[ROOT/v for v in ('src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/brender/include','src/carma2/s3/include','src/carma2/platform')]

 command=[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-DREC2_MATCHING',*['-I'+str(p)for p in inc],str(OUT/'matching.c'),str(OUT/'caller.obj'),'-o',str(OUT/'matching.exe')]

 r=subprocess.run(command,cwd=OUT,capture_output=True,text=True);(OUT/'matching-build.txt').write_text(r.stdout+r.stderr);assert r.returncode==0,r.stdout+r.stderr

 return dict(recompiled_sha256=sha(data),symbols_sha256=sha(symbols_path.read_bytes()),map_sha256=sha(map_bytes),global_fallbacks=global_fallbacks,graph=graph,relocations=relocations,HIGHLOW_sites_checked=checked,globals=globals_map,command=command)

registry.COMMON='';commands=registry.compile_sources(OUT,prefix+body+driver,prefix+'\n#define PHYSICAL_ORIGINAL\n'+driver,caller)

matching=linked_image(a.recompiled,a.symbols)if a.recompiled else None

assert before=={str(q):sha(q.read_bytes())for q in paths}

if a.build_only:raise SystemExit(0)

HEAP=0x20000000

fb=lambda f:struct.unpack('<I',struct.pack('<f',f))[0]

cases=[]

def scene(mode=0,label='base',primary=None,secondary=None,count=0,capacity=8,actor=None,pred=None,rows=1,patches=(),header=(),seed=1):

 data=bytearray((i*29+seed)&255 for i in range(65536))

 def w(at,value):struct.pack_into('<I',data,at,value&0xffffffff)

 def v(at,values):struct.pack_into('<3f',data,at,*values)

 def mat(at,values=None):struct.pack_into('<12f',data,at,*(values or [1,0,0,0,1,0,0,0,1,0,0,0]))

 for j,obj in enumerate((0x1000,0x2000,0x3000,0x4000)):

  data[obj:obj+0x4d8]=bytes(0x4d8);act=0x5000+0x100*j;w(obj,HEAP+act);mat(act+0x2c,actor if j==0 else None);mat(obj+0x8c,pred if j==0 else None);v(obj+0x74,[.3,-.2,.1]);v(obj+0x17c,[0,0,0]);w(obj+0x230,HEAP+0x2000)

 for j in range(4):

  at=0x6000+j*0x400;data[at:at+0x400]=bytes(0x400);v(at+8,[.4,.2,-.3]);v(at+0x14,[-.1,.7,.9]);v(at+0x38,[1,0,0]);w(at+0x50,rows)

  for r in range(6):

   row=at+32*r;w(row+0x54,0);w(row+0x58,fb(.2));v(row+0x5c,[1,0,0]);v(row+0x68,[0,1,0])

 if primary is not None:w(0x1188,HEAP+0x6000);w(0x6054,primary)

 if secondary is not None:w(0x118c,HEAP+0x6400);w(0x6454,secondary)

 for j in range(42):

  record=0xa000+96*j;w(record+4,HEAP+0x1000 if j%2==0 else HEAP+0x2000);w(record+8,HEAP+0x2000);v(record+0x14,[.25,.5,-.75]);v(record+0x20,[-.375,.125,.625]);v(record+0x2c,[1.3,-2.7,.41]);v(record+0x38,[-.9,.75,2.3]);w(record+0x5c,10)

 mat(0x8200,actor);mat(0x8240,pred);v(0x8500,[1,0,0]);v(0x8520,[0,1,0]);v(0x8540,[0,0,1]);v(0x8560,[.2,.3,.4])

 h=[mode,0x1000,0xa000,count&0xffffffff,capacity&0xffffffff]+[0]*11

 if mode==1:h[:9]=[1,0x8200,0x8240,0x8500,0x8520,0x8540,0x8560,0x8600,0x8620]

 h[11]=0x1234abcd;h[13]=1

 for at,value in patches:w(at,value)

 for at,value in header:h[at]=value

 cases.append(dict(label=label,header=h,data=data));return cases[-1]

# Direct helpers exercise genuine reflection chains and accessible source/output aliases.

for seed in range(32):

 rng=random.Random(seed);axes=[(at,fb(rng.uniform(-2,2)))for at in [0x8500,0x8504,0x8508,0x8520,0x8524,0x8528,0x8540,0x8544,0x8548]]

 scene(1,'helper-finite-%d'%seed,patches=axes,seed=seed+1)

for out0,out1 in [(0x8500,0x8620),(0x8520,0x8620),(0x8540,0x8620),(0x8600,0x8500),(0x8600,0x8520),(0x8600,0x8540),(0x8600,0x8600),(0x8504,0x8524)]:scene(1,'helper-alias-%x-%x'%(out0,out1),header=[(7,out0),(8,out1)])

scene(1,'helper-opposite-bisector',patches=[(0x8520,fb(-1)),(0x8524,0)])

for mode in [0,2]:

 for count in [0,1,7,8,-1]:scene(mode,'empty-%d-%d'%(mode,count),count=count,header=[(1,0)]if mode==2 else [])

for kind in [0,1]:

 for pending in [0,1]:

  for dots in [-.2,0,.0001,.001,.5]:

   # actor childX sees parentY*dot; predicted childX gains +Y for active plane path.

   actor=[1,dots,0,0,1,0,0,0,1,0,0,0];pred=[1,.2,0,0,1,0,0,0,1,0,0,0]

   scene(label='primary-%d-%d-%g'%(kind,pending,dots),primary=kind,rows=2 if kind==1 else 1,actor=actor,pred=pred,patches=[(0x10f0,pending)])

for kind in [0,10,11]:

 for count in [0,1,3,8]:

  for gate in [0,1]:scene(label='secondary-%d-%d-%d'%(kind,count,gate),secondary=kind,count=count,patches=[(0x11e0,gate)])

for count in [1,2,3,8]:

 for single in [0,1,2]:

  patch=[(0x11e0,1)]

  for j in range(count):

   if j%3==single:patch.extend([(0xa000+96*j+4,HEAP+0x1000),(0xa000+96*j+8,0)])

  scene(label='secondary-delete-%d-%d'%(count,single),secondary=10,count=count,patches=patch)

for actorX,predX in [(1,0),(0,1),(-1,0),(0,-1),(.2,.3),(.5,-.25)]:scene(label='secondary11-pos-%g-%g'%(actorX,predX),secondary=11,actor=[1,0,0,0,1,0,0,0,1,actorX,0,0],pred=[1,0,0,0,1,0,0,0,1,predX,0,0])

# Real error/exit ordering, including supplied error callback mutation.

scene(label='primary-missing-partner',primary=0,patches=[(0x1230,0)],header=[(12,3)])

scene(label='primary-unknown-kind',primary=8,header=[(12,2)])

scene(label='secondary-unknown-kind',secondary=8,header=[(12,1)])

for bits in [0,0x80000000,1,0x007fffff,0x3a83126e,0x3a83126f,0x3a831270,0x7f800000,0xff800000,0x7fc01234,0x7f801234,0xff801234]:

 for at in [0x605c,0x6068,0x6474,0x6438,0x1050]:scene(label='consumer-special-%x-%x'%(at,bits),primary=0,secondary=11,patches=[(at,bits),(0x10f0,1)])

 for at in [0x8500,0x8504,0x8508,0x8520,0x8524,0x8528]:scene(1,'helper-special-%x-%x'%(at,bits),patches=[(at,bits)])



# Full emitting planes/universals with real nonorthogonal matrix paths.

for seed in range(120):

 rng=random.Random(300+seed)

 actor=[rng.uniform(-2,2)for _ in range(12)];pred=[rng.uniform(-2,2)for _ in range(12)]

 for kind in [0,1]:

  scene(label='primary-random-%d-%d'%(kind,seed),primary=kind,rows=2 if kind else 1,actor=actor,pred=pred,seed=seed+3,patches=[(0x10f0,seed&1),(0x6058,fb(rng.uniform(-1,1)))])

 for kind in [0,11]:scene(label='secondary-random-%d-%d'%(kind,seed),secondary=kind,actor=actor,pred=pred,seed=seed+4)

# Capacity distinctions and multiple rows, including universal adjacent-row consumption.

for count,cap in [(0,0),(1,0),(1,1),(0,1),(3,2),(2,3),(0,0x7fffffff)]:

 for primary,secondary in [(0,0),(1,11),(None,10)]:

  scene(label='capacity-%s-%s-%d-%d'%(primary,secondary,count,cap),primary=primary,secondary=secondary,count=count,capacity=cap,rows=4,actor=[1,-.25,0,0,1,0,0,0,1,.5,0,0],pred=[1,.2,0,0,1,0,0,0,1,-.25,0,0],patches=[(0x11e0,1),(0x10f0,1)])

# Type10 raw transport must distinguish FLD/FCHS/FSTP axes/leverX from MOV Y/Z.

for bits in [0x7f800001,0x7fc00001,0xff801234,0xffc01234,0,0x80000000,1,0x7f800000,0xff800000]:

 for at in [0xa014,0xa018,0xa01c,0xa020,0xa024,0xa028,0xa02c,0xa030,0xa034,0xa038,0xa03c,0xa040]:scene(label='swap-word-%x-%x'%(at,bits),secondary=10,count=1,patches=[(0x11e0,1),(at,bits)])

# Directed paired IEEE inputs in every reflection component and actual accessible aliases.

pairs=[(0x7f801234,0x7fc05678),(0x7fc01234,0x7f805678),(0xff801234,0x7f805678),(0x7f801234,0xff801234),(0x7fc01234,0xffc01234)]

for x,y in pairs:

 for left,right in [(0x8504,0x8544),(0x8508,0x8548),(0x8524,0x8544),(0x8528,0x8548),(0x8500,0x8540),(0x8520,0x8540)]:

  scene(1,'helper-pair-%x-%x-%x-%x'%(left,right,x,y),patches=[(left,x),(right,y)])

 for dest in [0x8500,0x8520,0x8540,0x8504,0x8524,0x8544]:

  scene(1,'helper-pair-alias-%x-%x-%x'%(dest,x,y),patches=[(0x8504,x),(0x8544,y)],header=[(7,dest),(8,0x8620)])

 for field0,field1 in [(0x605c,0x6068),(0x6060,0x606c),(0x6064,0x6070),(0x6470,0x645c),(0x6458,0x6470)]:

  scene(label='consumer-pair-%x-%x-%x-%x'%(field0,field1,x,y),primary=0,secondary=0,patches=[(field0,x),(field1,y),(0x10f0,1)])

# Real recursive hierarchy, each object's distinct primary/secondary data live.

for primary,secondary in [(0,None),(None,0),(None,10),(1,11)]:

 c=scene(2,'recursive-%s-%s'%(primary,secondary),primary=primary,secondary=secondary,rows=2 if primary==1 else 1,count=2,patches=[(0x1224,HEAP+0x3000),(0x3220,HEAP+0x4000),(0x1220,HEAP+0x2000),(0x11e0,1),(0x31e0,1),(0x41e0,1)])

 for obj,joint in [(0x3000,0x6800),(0x4000,0x6c00)]:

  if primary is not None:struct.pack_into('<I',c['data'],obj+0x188,HEAP+joint);struct.pack_into('<I',c['data'],joint+0x54,primary)

  if secondary is not None:struct.pack_into('<I',c['data'],obj+0x18c,HEAP+joint);struct.pack_into('<I',c['data'],joint+0x54,secondary)

# Output aliases preserve valid object/joint pointer fields; each selected slot's

# write footprint is in scalar regions or harmless pending-state fields.

for destination in [0x6074,0x64b4,0x108c,0x5000+0x50,0x1174]:

 scene(label='consumer-output-alias-%x'%destination,secondary=0,header=[(2,destination)])



# Finite exponent/cancellation inputs and arbitrary nonorthogonal dual frames.

for seed in range(80):

 rng=random.Random(700+seed)

 def finite():return (-1 if rng.randrange(2)else 1)*rng.uniform(.5,1.0)*2.0**rng.randrange(-60,61)

 actor=[finite()for _ in range(12)];pred=[finite()for _ in range(12)]

 axes=[(at,fb(finite()))for at in [0x8500,0x8504,0x8508,0x8520,0x8524,0x8528,0x8540,0x8544,0x8548]]

 scene(1,'helper-exponents-%d'%seed,actor=actor,pred=pred,patches=axes,seed=seed+8)

 for kind in [0,1]:scene(label='primary-exponents-%d-%d'%(kind,seed),primary=kind,rows=2 if kind else 1,actor=actor,pred=pred,patches=[(0x10f0,1)],seed=seed+9)

# Cancellation is sensitive to association even when the ultimate result is a boolean.

for axes in [[1,2**55,-2**55],[2**55,-2**55,1],[-2**55,1,2**55],[1e-30,1e30,-1e30]]:

 for kind in [0,11]:

  scene(label='secondary-cancellation-%d-%s'%(kind,axes),secondary=kind,patches=[(0x645c+4*i,fb(v))for i,v in enumerate(axes)]+[(0x6468+4*i,fb(1))for i in range(3)])

# Adjacent original scalar thresholds used in wide compare/reloaded-scalar branches.

for target in [.001,-.001,.002,1e-6,2**-22,0.0]:

 bits=fb(target)

 for delta in [-1,0,1]:

  b=(bits+delta)&0xffffffff

  for kind in [0,1]:scene(label='primary-threshold-%d-%x'%(kind,b),primary=kind,rows=2 if kind else 1,patches=[(0x6060,b),(0x10f0,1)],pred=[1,.2,0,0,1,0,0,0,1,0,0,0])

  for kind in [0,11]:scene(label='secondary-threshold-%d-%x'%(kind,b),secondary=kind,patches=[(0x6468,b),(0x6458,b)])

  scene(1,'helper-small-bisector-%x'%b,patches=[(0x8500,0),(0x8520,b),(0x8524,0)])

# Type10 consecutive removals carry every raw sentinel word, without clearing trailing slots.

for n in [1,2,4,8,40]:

 patch=[(0x11e0,1)]

 for i in range(n):patch.extend([(0xa000+96*i+4,HEAP+0x1000),(0xa000+96*i+8,0)])

 scene(label='delete-all-%d'%n,secondary=10,count=n,capacity=40,patches=patch)

# Disabled callback still reaches authentic CRT exit, exact supplied message pointer.

scene(label='unknown-secondary-no-callback',secondary=9,header=[(13,0)])



# Complete reflected/bisector outputs partially overlap; both are legal vector

# destinations. This exposes reload/operand order without an artificial kernel.

for out0,out1 in [(0x8600,0x8604),(0x8604,0x8600),(0x8600,0x8608),(0x8608,0x8600)]:

 scene(1,'helper-partial-output-finite-%x-%x'%(out0,out1),header=[(7,out0),(8,out1)])

 for x,y in pairs:

  for left,right in [(0x8524,0x8544),(0x8528,0x8548),(0x8504,0x8544)]:scene(1,'helper-partial-output-pair-%x-%x-%x-%x-%x-%x'%(out0,out1,left,right,x,y),patches=[(left,x),(right,y)],header=[(7,out0),(8,out1)])

for count in [0,0x80000000,0xffffffff]:

 for initial in [0,0x80000000,0xffffffff]:scene(2,'recursive-empty-raw-count-%x-%x'%(count,initial),count=count,header=[(1,0),(11,initial)])

for count in [0,-1]:scene(label='type10-nonpositive-count-%d'%count,secondary=10,count=count,patches=[(0x11e0,1)],header=[(11,0)])



# Authentic output-header writes alter the current recursive node's live links.

# Secondary11 at object+218 writes objectA+4, null next+8 and null child+C.

scene(2,'live-child-next-cleared-by-record',secondary=11,actor=[1,0,0,0,1,0,0,0,1,1,0,0],pred=[1,0,0,0,1,0,0,0,1,0,0,0],header=[(2,0x1218)],patches=[(0x1220,HEAP+0x2000),(0x1224,HEAP+0x3000)])

# Primary header writes replace primaryjoint(+188) with this object's valid

# zero-limit prefix; secondaryjoint(+18C) becomes partner's zero-limit prefix.

scene(label='live-primary-joint-replaced-by-record',primary=0,actor=[1,-.2,0,0,1,0,0,0,1,0,0,0],pred=[1,.2,0,0,1,0,0,0,1,0,0,0],header=[(2,0x1184)])

faultcase=scene(label='fault-cleared-secondary-pointer',secondary=0,header=[(2,0x114c)],patches=[(0x6468,0),(0x646c,0),(0x6470,fb(1))]);cases.pop()

selected=list(range(len(cases)))

if a.case_limit is not None:selected=selected[:a.case_limit]

if a.case_index:selected=a.case_index

results=[];failures=[]

for done,index in enumerate(selected):

 c=cases[index];path=OUT/'input.bin';path.write_bytes(struct.pack('<16I',*c['header'])+c['data'])

 e=subprocess.run([str(OUT/'physical.exe'),str(path),str(OUT/'physical-image.bin')],capture_output=True,timeout=15)

 assert e.returncode==0 and len(e.stdout)in (65552,131104),(index,c['label'],e.returncode,len(e.stdout),e.stderr)

 for compiler in ['msvc5','msvc','mingw',*(['matching']if a.recompiled else [])]:

  v=subprocess.run([str(OUT/(compiler+'.exe')),str(path),str(OUT/('matching-image.bin'if compiler=='matching'else 'physical-image.bin'))],capture_output=True,timeout=15);equal=v.returncode==0 and v.stdout==e.stdout

  results.append(dict(index=index,label=c['label'],compiler=compiler,passed=equal,original_output_sha256=sha(e.stdout),output_sha256=sha(v.stdout),original_event=struct.unpack_from('<I',e.stdout)[0],original_value=struct.unpack_from('<I',e.stdout,4)[0],original_global=struct.unpack_from('<I',e.stdout,12)[0],original_changed_arena_words=[o for o in range(0,65536,4)if e.stdout[-65536+o:len(e.stdout)-65536+o+4]!=c['data'][o:o+4]]))

  if not equal:

   offsets=[i for i,(x,y)in enumerate(zip(e.stdout,v.stdout))if x!=y]

   failure=dict(index=index,label=c['label'],compiler=compiler,returncode=v.returncode,stderr=v.stderr.decode(errors='replace'),first_offsets=offsets[:30],original_bytes=len(e.stdout),native_bytes=len(v.stdout));failures.append(failure)

   (OUT/('failure-%d-%s-original.bin'%(index,compiler))).write_bytes(e.stdout);(OUT/('failure-%d-%s-native.bin'%(index,compiler))).write_bytes(v.stdout);(OUT/('failure-%d-input.bin'%index)).write_bytes(path.read_bytes())

 if done%25==0:print(done+1,'/',len(selected),'failures',len(failures),flush=True)

 if len(failures)>=a.max_failures:break

# One separately classified authentic invalid alias verifies the initial-only

# NULL guard. Fault status/empty output compare; no post-return ABI claim.

fault_input=OUT/'fault-input.bin';fault_input.write_bytes(struct.pack('<16I',*faultcase['header'])+faultcase['data']);fault_results=[]

for compiler in ['physical','msvc5','msvc','mingw',*(['matching']if a.recompiled else [])]:

 cmd=[str(OUT/(compiler+'.exe')),str(fault_input),str(OUT/('matching-image.bin'if compiler=='matching'else 'physical-image.bin'))]

 v=subprocess.run(cmd,capture_output=True,timeout=15);passed=(v.returncode&0xffffffff)==0xc0000005 and v.stdout==b''

 fault_results.append(dict(compiler=compiler,passed=passed,status=hex(v.returncode&0xffffffff),stdout_sha256=sha(v.stdout),stdout_bytes=len(v.stdout)))

 (OUT/(compiler+'-fault.bin')).write_bytes(v.stdout)

assert all(r['passed']for r in fault_results),fault_results

after={str(q):sha(q.read_bytes())for q in paths};assert before==after

manifest=dict(supplemental_invalid_alias_fault=dict(scenes=1,normal_return_ABI_claim=False,fixture_sha256=sha(fault_input.read_bytes()),results=fault_results),matching=matching,scenario_count=len(cases),selected_scenes=len(selected),executed_scenes=done+1,comparisons=len(results),failures=failures,results=results,source_before=before,source_after=after,whole_sources_unchanged=True,commands=commands,original_closure=closures,actual_direct_edges=edges,relocations=patches,HIGHLOW_sites_checked=checked,fixture_sha256=sha(b''.join(struct.pack('<16I',*c['header'])+c['data']for c in cases)),generated_sha256={str(q):sha(q.read_bytes())for q in OUT.iterdir()if q.suffix in ('.c','.obj','.exe','.bin','.py')and not q.name.startswith(('input','failure'))},scope=__doc__,limits=['PC53/masked exceptions; no sticky/unmasked/nonx86/exhaustive claim.','Error scenes compare actual callback plus CRT exit states/order, no normal-return ABI claim on those terminal paths.','Three complete entries; credit remains gated by independent native and later actual linked review. Parent4baec0 not implemented.'])

(OUT/'verification.json').write_text(json.dumps(manifest,indent=2)+'\n');print(done+1,len(results),len(failures));raise SystemExit(bool(failures))
