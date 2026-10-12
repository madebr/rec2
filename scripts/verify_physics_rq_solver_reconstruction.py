"""Complete constrained RQ solver with real cached/expansion dependencies.

The physical original entry uses ECX original matrix, EDX RHS, nine stack arguments, RET36.
Native bridge capacity is eleven arguments, with native cdecl cleanup verified.
All262144 arena bytes compare, including cached records and pointer aliases.
"""
import argparse,ast,hashlib,json,os,random,re,struct,subprocess,sys
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_MEM,CS_OP_IMM

ROOT=next(p for p in Path(__file__).resolve().parents if (p/'src/carma2').is_dir())
sys.path.insert(0,str(ROOT/'scripts'))
from verify_tinted_native_reconstruction import extract as extract_hooked
from inventory_reconstruction_backlog import mask_c
from verify_physics_rq_solver_stack_capture import audit_native, audit_prefix
def extract(source,name):
 if name=='FUN_004c3b60':
  clean=mask_c(source);hits=list(re.finditer(r'^int C2_HOOK_FASTCALL '+name+r'\([^;{}]*\)\s*\{',clean,re.M))
  assert len(hits)==1,(name,len(hits))
  start,end=hits[0].start(),hits[0].end();depth=1
  while depth:depth+=(clean[end]=='{')-(clean[end]=='}');end+=1
  return source[start:end]
 if name not in ('ForceBytes','ForcePointerAt'):return extract_hooked(source,name)
 clean=mask_c(source);hits=list(re.finditer(r'^static [^\n;{}]*\b'+re.escape(name)+r'\([^;{}\n]*\)\s*\{',clean,re.M))
 assert len(hits)==1,(name,len(hits))
 start,end=hits[0].start(),hits[0].end();depth=1
 while depth:depth+=(clean[end]=='{')-(clean[end]=='}');end+=1
 return source[start:end]
p=argparse.ArgumentParser();p.add_argument('--output',required=True);p.add_argument('--expanded',action='store_true');p.add_argument('--stress',action='store_true');p.add_argument('--aliases',action='store_true');p.add_argument('--ieee',action='store_true');p.add_argument('--stack-history',action='store_true');p.add_argument('--limit',type=int);p.add_argument('--candidate',type=Path);p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);args=p.parse_args()
assert bool(args.recompiled)==bool(args.symbols), 'Both immutable PE and symbol map are required'
assert args.limit is None or args.limit > 0, 'A scene limit must be positive'
OUT=ROOT/args.output;OUT.mkdir(parents=True,exist_ok=True)
SOURCE=args.candidate.resolve() if args.candidate else ROOT/'src/carma2/newcommon/49-physics.c'
PRODUCTION=ROOT/'src/carma2/newcommon/49-physics.c'
NAMES=['PhysicsWallNaN','ForceBytes','ForcePointerAt','RQScalar','RQNaN','RQAccumulate','PartnerAddWide','PartnerSubtractWide','PartnerMultiplyWide','PartnerMultiplyRawScalar','PartnerReverseSubtractScalar','RQDivideScalars','RQSubtractProduct','PartnerAddScalar']
candidate_text='\n'.join(extract(PRODUCTION.read_text(),name)for name in NAMES+['RQExpand','RQSingleExpand','RQFromCachedGroups'])+'\n'+'\n'.join(extract(SOURCE.read_text(),name)for name in ('FUN_004c61e0','RQSolveSeen','RQSolveToggle','FUN_004c3b60'))
paths=[SOURCE,PRODUCTION,ROOT/'src/carma2/newcommon/49-physics.h',ROOT/'src/carma2/include/rec2_types.h',ROOT/'src/carma2/hooks/c2_hooks.h',ROOT/'src/brender/include/brender/brender.h',ROOT/'src/brender/include/brender/br_types.h',Path(__file__),ROOT/'scripts/verify_tinted_native_reconstruction.py',ROOT/'scripts/inventory_reconstruction_backlog.py',ROOT/'scripts/verify_opponent_navigation_reconstruction.py',ROOT/'scripts/verify_physics_rq_solver_stack_capture.py']
source_before={str(p):hashlib.sha256(p.read_bytes()).hexdigest()for p in paths}
(OUT/'source-before.json').write_text(json.dumps(source_before,indent=2)+'\n')
spec=[('FUN_004c3b60',0x4c3b60,3304),('FUN_004c61e0',0x4c61e0,1),('RQFromCachedGroups',0x4c5220,398),('RQExpand',0x4c4850,1771),('RQSingleExpand',0x4c4f40,727)]
b=(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes();original_hash=hashlib.sha256(b).hexdigest();assert original_hash=='9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
nt=struct.unpack_from('<I',b,60)[0];table=nt+24+struct.unpack_from('<H',b,nt+20)[0];sz=struct.unpack_from('<I',b,nt+80)[0];flat=bytearray(sz);sections=[]
for k in range(struct.unpack_from('<H',b,nt+6)[0]):
    vs,va,n,off=struct.unpack_from('<IIII',b,table+40*k+8);flat[va:va+n]=b[off:off+n]
cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;patches=[];entries=[]
for name,start,n in spec:
    body=bytes(flat[start-0x400000:start-0x400000+n]);ins=list(cs.disasm(body,start));assert sum(i.size for i in ins)==n
    entries.append(dict(name=name,address=start,size=n,sha256=hashlib.sha256(body).hexdigest()))
    for i in ins:
        if i.mnemonic=='call':assert i.operands[0].type==CS_OP_IMM and i.operands[0].imm in {a for _,a,_ in spec}
        if i.mnemonic.startswith('j'):assert i.operands[0].type==CS_OP_IMM and start<=i.operands[0].imm<start+n
        for op in i.operands:
            if op.type==CS_OP_MEM and 0x400000<=op.mem.disp<0x400000+sz:
                assert not op.mem.index and not op.mem.base and op.mem.disp in (0x58a8b8,0x58a8c0,0x58a8e0,0x58a8e8,0x58a8ec,0x58a8f0,0x58a8f8,0x58a900,0x744818,0x744814,0x6940b8)
                assert op.size == (8 if op.mem.disp in (0x58a8c0,0x58a8e0,0x58a8f0,0x58a8f8,0x58a900) else 4)
                assert op.mem.disp not in (0x744818,0x744814,0x6940b8) or bytes(flat[op.mem.disp-0x400000:op.mem.disp-0x400000+4])==bytes(4)
                at=i.address-0x400000+i.disp_offset;value=op.mem.disp-0x400000+0x50000000
                patches.append(dict(address=i.address,constant=op.mem.disp,bytes=bytes(flat[op.mem.disp-0x400000:op.mem.disp-0x400000+op.size]).hex()))
                struct.pack_into('<I',flat,at,value)
assert bytes(flat[0xc462b:0xc4630])==bytes.fromhex('b968d06500')
struct.pack_into('<I',flat,0xc462c,0x5025d068)
(OUT/'physical-image.bin').write_bytes(flat)
prefix=r'''
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "49-physics.h"
#include "c2_hooks.h"
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[11],builder_stack_count,builder_result,builder_poison,builder_cdecl,builder_history;
extern unsigned __cdecl BuilderInvoke(void);
void* gPhysics_buffer1_ptr;
void* gPhysics_buffer2_ptr;
int gPhysics_rq_mode_6940b8;
#if defined(_MSC_VER) && _MSC_VER >= 1300 && defined(_M_IX86)
void* _AddressOfReturnAddress(void);
#pragma intrinsic(_AddressOfReturnAddress)
#endif
'''
driver=r'''
int C2_HOOK_CDECL main(int argc,char**argv){
 FILE*f;unsigned char*code,*arena;unsigned h[16],values[11],n,i;
 unsigned entry_offsets[1]={0xc3b60};
 if(argc!=3)return 2;SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);
 code=VirtualAlloc((void*)0x50000000,IMAGE_SIZE,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);
 arena=VirtualAlloc((void*)0x20000000,262144,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
 if(code!=(void*)0x50000000||arena!=(void*)0x20000000)return 3;
 f=fopen(argv[2],"rb");if(!f)return 4;if(fread(code,IMAGE_SIZE,1,f)!=1)return 5;fclose(f);
 f=fopen(argv[1],"rb");if(!f)return 6;if(fread(h,sizeof(h),1,f)!=1||fread(arena,262144,1,f)!=1)return 7;fclose(f);
 _controlfp(_PC_53,_MCW_PC);_setmode(_fileno(stdout),_O_BINARY);
 values[0]=(unsigned)(uintptr_t)(arena+h[2]);values[1]=(unsigned)(uintptr_t)(arena+h[7]);
 values[2]=(unsigned)(uintptr_t)(arena+h[6]);values[3]=h[5];values[4]=0x13572468;
 values[5]=(unsigned)(uintptr_t)(arena+h[4]);values[6]=(unsigned)(uintptr_t)(arena+h[12]);
 values[7]=h[10];values[8]=0x24681357;values[9]=(unsigned)(uintptr_t)(arena+h[8]);values[10]=h[9];n=11;

#ifdef PHYSICAL_ORIGINAL
 *(unsigned*)(code+0x344818)=(unsigned)(uintptr_t)(arena+h[1]);
 *(unsigned*)(code+0x344814)=(unsigned)(uintptr_t)(arena+h[3]);
 *(unsigned*)(code+0x2940b8)=h[11];
 builder_address=(unsigned)(uintptr_t)(code+entry_offsets[h[0]]);
#else
 (void)entry_offsets;
 gPhysics_buffer1_ptr=arena+h[1];gPhysics_buffer2_ptr=arena+h[3];gPhysics_rq_mode_6940b8=h[11];
 builder_address=(unsigned)(uintptr_t)FUN_004c3b60;
#endif
 builder_mode=0;builder_x=values[0];builder_y=values[1];builder_poison=h[13];builder_history=h[14];
#if defined(REC2_MATCHING)||defined(PHYSICAL_ORIGINAL)
 builder_cdecl=0;builder_stack_count=n-2;for(i=2;i<n;i++)builder_stack[i-2]=values[i];
#else
 builder_cdecl=1;builder_stack_count=n;for(i=0;i<n;i++)builder_stack[i]=values[i];
#endif
 if(BuilderInvoke())return 14;
 fwrite(&builder_result,4,1,stdout);fwrite(arena,262144,1,stdout);return 0;
}
'''.replace('IMAGE_SIZE',str(sz))
(OUT/'harness.c').write_text(prefix+candidate_text+driver)
(OUT/'original.c').write_text('#define PHYSICAL_ORIGINAL\n'+prefix+driver)
linked_meta=None
linked_entry_capture=None
if args.recompiled:
 data=args.recompiled.read_bytes();mapping=json.loads(args.symbols.read_text());assert mapping['original_sha256']==original_hash and mapping['recompiled_sha256']==hashlib.sha256(data).hexdigest()
 symbols={x['orig']:x['recomp'] for x in mapping['symbols']};map_path=args.recompiled.with_suffix('.map');map_bytes=map_path.read_bytes()
 functions={int(address,16):(name,owner.strip()) for name,address,owner in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',map_bytes.decode('latin1'),re.M)}
 nt2=struct.unpack_from('<I',data,60)[0];base2=struct.unpack_from('<I',data,nt2+52)[0];size2=struct.unpack_from('<I',data,nt2+80)[0];table2=nt2+24+struct.unpack_from('<H',data,nt2+20)[0]
 flat=bytearray(size2);sections=[]
 for si in range(struct.unpack_from('<H',data,nt2+6)[0]):
  virtual,rva,n,offset=struct.unpack_from('<IIII',data,table2+40*si+8);flags=struct.unpack_from('<I',data,table2+40*si+36)[0];flat[rva:rva+n]=data[offset:offset+n];sections.append((base2+rva,max(virtual,n),flags))
 mutable={symbols[a] for a in (0x744818,0x744814,0x6940b8)}
 initial={symbols[address]:name for name,address,_ in spec};todo=list(initial);seen=set();linked_closure=[];patched=set();relocations=[]
 while todo:
  start=todo.pop(0)
  if start in seen:continue
  seen.add(start);end=min(address for address in functions if address>start);body=bytes(flat[start-base2:end-base2]);instructions=list(cs.disasm(body,start));assert sum(i.size for i in instructions)==len(body)
  name=initial.get(start,functions[start][0])
  for ins in instructions:
   if ins.mnemonic=='call' or ins.mnemonic.startswith('j'):
    assert ins.operands[0].type==CS_OP_IMM,(name,ins.op_str)
    target=ins.operands[0].imm
    if not start<=target<end:
     assert target in functions,(name,ins.op_str)
     if target not in initial:assert functions[target][1]=='49-physics.c.obj' and functions[target][0].lstrip('_@').startswith(('RQ','Partner','ForcePointerAt','ForceBytes','PhysicsWallNaN')),(name,functions[target])
     todo.append(target)
   for operand in ins.operands:
    if operand.type==CS_OP_MEM and base2<=operand.mem.disp<base2+size2:
     assert not operand.mem.base and not operand.mem.index and ins.disp_size==4,(name,ins.op_str)
     section=next(sec for sec in sections if sec[0]<=operand.mem.disp<sec[0]+sec[1]);assert operand.mem.disp in mutable or not(section[2]&0x80000020),(name,ins.op_str)
     if operand.mem.disp in mutable:assert operand.size==4 and bytes(flat[operand.mem.disp-base2:operand.mem.disp-base2+4])==bytes(4)
     site=ins.address-base2+ins.disp_offset
     if site not in patched:
      relocations.append(dict(site=site,original=operand.mem.disp,width=operand.size,bits=bytes(flat[operand.mem.disp-base2:operand.mem.disp-base2+operand.size]).hex()));struct.pack_into('<I',flat,site,operand.mem.disp-base2+0x50000000);patched.add(site)
   for operand in ins.operands:
    if operand.type==CS_OP_IMM and ins.mnemonic not in ('call',) and not ins.mnemonic.startswith('j') and base2<=operand.imm<base2+size2:
     assert ins.imm_size==4 and ins.mnemonic in ('mov','push'),(name,ins.op_str)
     raw=bytes(flat[operand.imm-base2:operand.imm-base2+27])
     assert raw==b'Matrix solve loop overflow\0',(name,ins.op_str,raw)
     site=ins.address-base2+ins.imm_offset
     assert site not in patched
     relocations.append(dict(site=site,original=operand.imm,width=27,bits=raw.hex()))
     struct.pack_into('<I',flat,site,operand.imm-base2+0x50000000);patched.add(site)
  linked_closure.append(dict(name=name,address=start,length=end-start,sha256=hashlib.sha256(body).hexdigest()))
 # Reject any unreviewed address-bearing immediate or other relocation in closure.
 rel_rva,rel_size=struct.unpack_from('<II',data,nt2+24+96+5*8);cursor=rel_rva
 while cursor<rel_rva+rel_size:
  page,n=struct.unpack_from('<II',flat,cursor);assert n>=8
  for word in struct.unpack_from('<'+'H'*((n-8)//2),flat,cursor+8):
   if word>>12==3:
    site=page+(word&4095)
    if any(row['address']-base2<=site<row['address']-base2+row['length'] for row in linked_closure):assert site in patched,hex(site)
  cursor+=n
 (OUT/'matching-image.bin').write_bytes(flat)
 linked_entry_capture=audit_prefix(bytes(flat[symbols[0x4c3b60]-base2:symbols[0x4c3b60]-base2+512]),0x8c8)
 matching_driver=driver.replace(str(sz),str(size2)).replace('0xc3b60',hex(symbols[0x4c3b60]-base2))
 for old,original in ((0x344818,0x744818),(0x344814,0x744814),(0x2940b8,0x6940b8)):
  matching_driver=matching_driver.replace('code+'+hex(old),'code+'+hex(symbols[original]-base2))
 (OUT/'matching.c').write_text('#define PHYSICAL_ORIGINAL\n'+prefix+matching_driver)
 linked_meta=dict(recompiled_sha256=hashlib.sha256(data).hexdigest(),symbols_sha256=hashlib.sha256(args.symbols.read_bytes()).hexdigest(),native_map_sha256=hashlib.sha256(map_bytes).hexdigest(),closure=linked_closure,constant_relocations=relocations)

includes=[ROOT/v for v in ('src/brender/include','src/carma2/include','src/carma2/hooks','src/carma2/s3/include','src/carma2/newcommon')]
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

# Use the retained ABI checker template without substituting leaf semantics.
caller_tree=ast.parse((ROOT/'scripts/verify_opponent_navigation_reconstruction.py').read_text())
caller_nodes=[node.value.value for node in ast.walk(caller_tree) if isinstance(node,ast.Assign) and isinstance(node.value,ast.Constant) and any(isinstance(target,ast.Name) and target.id=='caller' for target in node.targets)]
assert len(caller_nodes)==1
caller=caller_nodes[0].replace('builder_stack[5]','builder_stack[11]').replace('builder_poison;','builder_poison,builder_cdecl,builder_history;')
caller=caller.replace(' cld\n mov ebp', ' cld\n mov eax,builder_history\n mov dword ptr [esp-08f0h],eax\n mov ebp')
caller=caller.replace(' mov builder_result,eax\n mov ecx,esp',' mov builder_result,eax\n cmp builder_cdecl,0\n je stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n stackready:\n mov ecx,esp')
(OUT/'caller.c').write_text(caller)
for command in commands.values():command.append(str(OUT/'caller.obj'))
commands={'caller':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Zl','/c',str(OUT/'caller.c'),'/Fo'+str(OUT/'caller.obj')],**commands}

for name,cmd in commands.items():
    r=subprocess.run(cmd,cwd=OUT,env=env if name=='msvc'else env5 if name in ('msvc5','caller')else None,capture_output=True,text=True);(OUT/(name+'-build.txt')).write_text(r.stdout+r.stderr)
    if r.returncode:raise RuntimeError(r.stdout+r.stderr)

entry_captures=audit_native(OUT)
if linked_entry_capture:entry_captures.append(dict(compiler='matching',**linked_entry_capture))
(OUT/'entry-capture-provenance.json').write_text(json.dumps(dict(passed=True,entries=entry_captures,scope='Control-only emitted-entry address and pre-capture clobber audit. Physical programs provide all arithmetic comparisons.'),indent=2)+'\n')




rng=random.Random(0x4c3b60);cases=[]
BASE=0x20000000
def scene(size,unconstrained,kind=0,mode=0,alias=None,patch=None):
 h=[0,0x1000,0x15000,0xb000,0x1f000,size,0x36400,0x36000,0x37000,0,unconstrained,mode,0x36800,0xcccccccc,0xcccccccc,0]
 if alias:
  for key,value in alias.items():h[key]=value
 arena=bytearray(b'\xa5'*262144)
 for i in range(100):struct.pack_into('<f',arena,h[6]+4*i,-123.0)
 for at in (h[1],h[2],h[3]):
  for i in range(10000):struct.pack_into('<f',arena,at+4*i,0.0)
 for i in range(size):
  for j in range(i,size):
   v=(size+2.0+rng.random()) if i==j else rng.uniform(-0.8,0.8)
   if kind==1:v=1.0 if i==j else 0.0
   if kind==2:v=2.0 if i==j else 0.5
   for row,col in ((i,j),(j,i)):struct.pack_into('<f',arena,h[2]+4*(row*100+col),v)
 for i in range(100):
  v=rng.uniform(-2,5) if i else rng.uniform(1,5)
  struct.pack_into('<f',arena,h[7]+4*i,v)
 for at,word in patch or []:struct.pack_into('<I',arena,at,word)
 cases.append(dict(size=size,unconstrained=unconstrained,kind=kind,mode=mode,alias=alias,patch=patch,header=h,data=bytes(arena)))
for mode in (0,1):
 for n in (1,2,3,5,8,12,20):
  for free in sorted({0,n//2,n-1}):
   for kind in (0,1,2):scene(n,free,kind,mode)
if args.expanded:
 for n in (2,3,5,8,12,20,32,40,60):
  for free in (0,1,n//2,n-1):
   for mode in (0,1):
    for _ in range(3):scene(n,free,mode=mode)
if args.stress:
 def replace_last(matrix=None,rhs=None):
  c=cases[-1];data=bytearray(c['data']);h=c['header']
  if matrix is not None:
   for i,row in enumerate(matrix):
    for j,v in enumerate(row):struct.pack_into('<f',data,h[2]+4*(i*100+j),v)
  if rhs is not None:
   for i,v in enumerate(rhs):struct.pack_into('<f',data,h[7]+4*i,v)
  c['data']=bytes(data);c['matrix_override']=matrix;c['rhs_override']=rhs
 # Exact singular constraints, inconsistent ranks, both RQ update modes.
 for mode in (0,1):
  for n in (2,3,4,8):
   for free in (0,1):
    for kind in range(4):
     scene(n,free,mode=mode)
     mat=[[1.0 if kind==0 else (1.0 if i%2==j%2 else -1.0) if kind==1 else float((i+1)*(j+1)) if kind==2 else (1.0 if i==j else 0.0) for j in range(n)]for i in range(n)]
     if kind==3:mat[-1]=mat[0][:]
     replace_last(mat,[float(i+1)for i in range(n)])
 for mode in (0,1):
  scene(64,0,1,mode);replace_last(rhs=[1.0]*64)
 # The dimension-one exit has an exact F64 threshold and raw scalar quotient.
 for numerator in (0,0x80000000,1,0x3f800000,0x7f7fffff,0x7f800000,0xff800000,0x7fc12345,0x7f812345):
  for denominator in (0,0x80000000,1,0x38d1b716,0x38d1b717,0x38d1b718,0x3f800000,0xbf800000,0x7f800000,0x7fc12345,0x7f812345):
   scene(1,0,patch=[(0x36000,numerator),(0x15000,denominator)])
 # Deterministic nonsymmetric / indefinite matrices exercise removal/history.
 for mode in (0,1):
  for n in (2,3,4,5):
   for trial in range(12):
    scene(n,0,mode=mode)
    mat=[[float(rng.randrange(-3,4))for j in range(n)]for i in range(n)]
    for i in range(n):mat[i][i]=float(rng.randrange(1,4))
    replace_last(mat,[float(rng.randrange(1,5))for i in range(n)])
if args.aliases:
 for n,free in ((1,0),(2,0),(3,1),(5,2)):
  for mode in (0,1):
   for alias in ({6:0x36000},{6:0x36004},{6:0x35ffc},{6:0x15000},{6:0x15004},{6:0x14ffc},{6:0x1000},{6:0x1004},{6:0xb000},{6:0xb004},{4:0x36400},{12:0x36400},{12:0x36000}):
    if alias.get(4)==0x36400:
     scene(n,free,1,mode,alias=alias,patch=[(0x36000+4*i,0x40000000)for i in range(n)]+[(0x36000,0x40000000|free|(free<<8)|(free<<16))])
    elif n>1 and alias.get(12)==0x36400:
     scene(n,max(free,1),1,mode,alias=alias,patch=[(0x36000,0)])
    else:scene(n,free,1,mode,alias=alias)
 for n,free in ((2,0),(3,1),(5,2)):
  for mode in (0,1):
   for alias in ({1:0x15000},{1:0x15004},{3:0x15000},{3:0x15004},{3:0x1000},{3:0x1004}):scene(n,free,1,mode,alias=alias)
 # Valid cached identity blocks cover complete and incremental initial RQ assembly.
 for n,free,counts in ((2,1,[1]),(3,2,[1,1]),(5,3,[2,1]),(8,3,[1,1,1,1]),(8,4,[2,2])):
  for mode in (0,1):
   scene(n,free,1,mode)
   c=cases[-1];h=c['header'];h[9]=len(counts);data=bytearray(c['data'])
   cursor=0
   for group,count in enumerate(counts):
    record=0x35000+group*32;r=0x21000+cursor;q=0x2b000+cursor
    struct.pack_into('<I',data,h[8]+group*4,BASE+record)
    struct.pack_into('<I',data,record,count);struct.pack_into('<II',data,record+0x14,BASE+q,BASE+r)
    for i in range(count):
     for j in range(count):
      for at in (r,q):struct.pack_into('<f',data,at+4*(i*count+j),1.0 if i==j else 0.0)
    cursor+=4*count*count
   c['data']=bytes(data);c['cached_counts']=counts
if args.ieee:
 for n,free in ((2,1),(3,1),(5,2)):
  for mode in (0,1):
   for at in (0x15000,0x15004,0x15190,0x15194,0x36000,0x36004,0x36000+4*(n-1)):
    for word in (0,0x80000000,1,0x80000001,0x7f7fffff,0x7f800000,0xff800000,0x7fc12345,0x7f812345):
     scene(n,free,1,mode,patch=[(at,word)])
   for at1,at2 in ((0x15000,0x15004),(0x15000,0x36000),(0x15190,0x36004),(0x36000,0x36004)):
    for word1,word2 in ((0x7fc12345,0xffc12345),(0x7f812345,0x7fc12345),(0x7fc54321,0xff812345),(0x7f800000,0xff800000),(0x7f7fffff,1)):
     scene(n,free,1,mode,patch=[(at1,word1),(at2,word2)])
if args.stack_history:
 for n,free in ((50,49),(51,50),(52,50),(52,51),(53,52),(58,57)):
  for mode in (0,1):
   for rank in (1,2):
    for word in (0,0xcccccccc,0xffffffff,0x12345678):
     scene(n,free,mode=mode)
     c=cases[-1];h=c['header'];h[13]=word^0xffffffff;h[14]=word
     data=bytearray(c['data'])
     for i in range(n):
      struct.pack_into('<f',data,h[7]+4*i,1.0)
      for j in range(n):
       struct.pack_into('<f',data,h[2]+4*(i*100+j),1.0 if rank==1 else float(1+(i&1)*(j&1)))
     c['data']=bytes(data);c['rank']=rank;c['history_word']=hex(word);c['surrounding_stack_word']=hex(h[13])
(OUT/'inputs.bin').write_bytes(b''.join(struct.pack('<16I',*c['header'])+c['data']for c in cases[:args.limit]))
results=[];failures=[]
for index,c in enumerate(cases[:args.limit]):
    path=OUT/'input.bin';path.write_bytes(struct.pack('<16I',*c['header'])+c['data'])
    oracle=subprocess.run([str(OUT/'original.exe'),str(path),str(OUT/'physical-image.bin')],capture_output=True,timeout=20);assert oracle.returncode==0 and len(oracle.stdout)==262148,(index,oracle.returncode)
    for compiler in ['msvc5','msvc','mingw']+(['matching']if args.recompiled else []):
        actual=subprocess.run([str(OUT/(compiler+'.exe')),str(path),str(OUT/('matching-image.bin'if compiler=='matching' else 'physical-image.bin'))],capture_output=True,timeout=20)
        good=actual.returncode==0 and actual.stdout==oracle.stdout
        r=dict(index=index,compiler=compiler,passed=good,case={k:v for k,v in c.items()if k!='data'});results.append(r)
        if not good:
            r['returncode']=actual.returncode;r['differences']=[dict(offset=i,expected=oracle.stdout[i:i+4].hex(),actual=actual.stdout[i:i+4].hex())for i in range(0,min(len(actual.stdout),len(oracle.stdout)),4)if oracle.stdout[i:i+4]!=actual.stdout[i:i+4]]
            failures.append(r);(OUT/f'failure-{index}-{compiler}.json').write_text(json.dumps(r,indent=2));(OUT/f'failure-{index}.bin').write_bytes(path.read_bytes())
    if index%25==0:print(index+1,'cases,',len(failures),'failures',flush=True)
    if len(failures)>=12:break
source_after={str(p):hashlib.sha256(p.read_bytes()).hexdigest()for p in paths}
assert source_before==source_after,'Source changed during verification'
(OUT/'verifier.py').write_bytes(Path(__file__).read_bytes())
v=dict(entries=entries,patches=patches,original_sha256=original_hash,candidate_sha256=hashlib.sha256(candidate_text.encode()).hexdigest(),source_sha256=source_before,source_stable=True,matching=linked_meta,total_scenarios=len(cases),complete_suite=index+1==len(cases),input_sha256=hashlib.sha256(b''.join(struct.pack('<16I',*c['header'])+c['data']for c in cases[:index+1])).hexdigest(),generated_sha256={p.name:hashlib.sha256(p.read_bytes()).hexdigest()for p in OUT.iterdir()if p.suffix in('.c','.obj','.exe')},scope='Complete solver original/native3 and optional actual linked PE closure at PC53 with masked exceptions. Full262144-byte public arena and int EAX compared; ESP, nonvolatile registers and x87 TOP checked. No output normalization. Tests include valid bounded dimensions1..64, finite/IEEE inputs, singular/rank-deficient matrices, both update modes, valid alias domains and cached groups. Targeted high-free tests set only the original private stack slot independently of surrounding stack contents. This is an original x86 entry-stack compatibility contract; no claim for undefined out-of-bounds histories or non-x86 fallback equivalence.',suite_flags={name:getattr(args,name)for name in ('expanded','stress','aliases','ieee','stack_history')},scenarios=index+1,comparisons=len(results),failures=failures,results=results)
(OUT/'verification.json').write_text(json.dumps(v,indent=2)+'\n');print('finished',index+1,len(results),len(failures))
if failures:raise SystemExit(1)
