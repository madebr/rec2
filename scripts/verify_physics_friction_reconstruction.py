"""Physical original/native/linked proof of the complete friction consumer closure.

Entries: NewAddFriction4c0ac0 (3448B), PostAddFriction4c1840 (131B),
DoJointFriction4c18d0 (779B). Original full bodies and actual matrix/vector
leaves execute atPC53; native actual production bodies are extracted by default.
Optional --candidate is development only. Canonical float callback contract stays
unchanged;106 cases execute authentic GetFrictionFromFace31B with only its table
operand relocated. Other legitimate callbacks are float-declared with exactly
float-representable values, strict mutations and full pre-call snapshots.

Input16DWORD header+65536byte arena; outputvoid EAX excluded+complete raw arena
and65536byte callback snapshot. Direct caller checks ESP/nonvolatiles/x87TOP.
No masks/oracle normalization. Matching mode loads actual hash-bound PE/MAP
closed graph; internal branches and relative calls remain unchanged.
"""
import math,ast,argparse,hashlib,json,os,random,re,struct,subprocess,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parent
while not (ROOT/'src').is_dir():ROOT=ROOT.parent
sys.path.insert(0,str(ROOT/'build-audit/joystick/python-deps'))
sys.path.insert(0,str(ROOT/'scripts'))
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_MEM,CS_OP_IMM
from verify_tinted_native_reconstruction import extract
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',default='build-audit/physics-friction-tests');p.add_argument('--candidate',type=Path);p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);p.add_argument('--case-limit',type=int);p.add_argument('--case-start',type=int,default=0);p.add_argument('--max-failures',type=int,default=20);args=p.parse_args()
OUT=ROOT/args.output;OUT.mkdir(parents=True,exist_ok=True)
assert bool(args.recompiled)==bool(args.symbols),'Supply both immutable PE and symbol map'
leaves=ROOT/'src/carma2/newcommon/49-physics.c'
source=args.candidate.resolve() if args.candidate else leaves
helper_names=['FrictionRead', 'FrictionWord', 'FrictionPointer', 'FrictionScalar', 'FrictionWrite', 'FrictionActorMatrix', 'FrictionFlag', 'FrictionAt', 'FrictionProductScalars', 'FrictionDivideScalars', 'FrictionSubtractScalar', 'FrictionAddScalar', 'FrictionReverseSubtractScalar', 'FrictionReverseDivideScalar', 'FrictionAddWide', 'FrictionProductWide', 'FrictionStatus', 'FrictionGreater', 'FrictionLessOrUnordered', 'FrictionNonZero']
entry_names=['NewAddFriction','PostAddFriction','DoJointFriction']
def extract_candidate(text):
 pieces=['void C2_HOOK_FASTCALL PostAddFriction(tPhysics_object* object);','void C2_HOOK_FASTCALL DoJointFriction(tPhysics_object* object);','#if defined(_MSC_VER) && _MSC_VER < 1300\n#pragma auto_inline(off)\n#endif']
 for name in helper_names:
  body=extract(text,name)
  if name=='FrictionStatus':body='#if defined(_MSC_VER) && _MSC_VER < 1300\n'+body+'\n#endif'
  pieces.append(body)
 pieces.append('#if defined(_MSC_VER) && _MSC_VER < 1300\n#pragma auto_inline(on)\n#endif')
 pieces.extend(extract(text,name) for name in entry_names)
 return '\n'.join(pieces)
leaf_names=[]
source_paths=[leaves,leaves.with_suffix('.h'),ROOT/'src/brender/core/math/matrix34.c',ROOT/'src/brender/core/math/vector.c',ROOT/'src/brender/include/brender/brender.h',ROOT/'src/carma2/include/rec2_types.h',ROOT/'src/carma2/hooks/c2_hooks.h',Path(__file__),ROOT/'scripts/verify_opponent_navigation_reconstruction.py',ROOT/'scripts/verify_tinted_native_reconstruction.py']
if args.candidate:source_paths.append(source)
source_before={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in source_paths}
leaf_file_text=leaves.read_text()
original=(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes();original_hash=hashlib.sha256(original).hexdigest();assert original_hash=='9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
nt=struct.unpack_from('<I',original,60)[0];table=nt+24+struct.unpack_from('<H',original,nt+20)[0];sz=struct.unpack_from('<I',original,nt+80)[0];image=bytearray(sz)
for i in range(struct.unpack_from('<H',original,nt+6)[0]):
 _,rva,n,off=struct.unpack_from('<IIII',original,table+40*i+8);image[rva:rva+n]=original[off:off+n]
spec=[('NewAddFriction',0x4c0ac0,3448),('PostAddFriction',0x4c1840,131),('DoJointFriction',0x4c18d0,779),('BrMatrix34ApplyV',0x533520,107),('BrMatrix34TApplyV',0x533690,107),('BrVector3SetFloat',0x534590,25)]
cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;patches=[];entries=[]
for name,address,n in spec:
 body=bytes(image[address-0x400000:address-0x400000+n]);instructions=list(cs.disasm(body,address));assert sum(i.size for i in instructions)==n
 entries.append(dict(name=name,address=address,bytes=n,sha256=hashlib.sha256(body).hexdigest()))
 for ins in instructions:
  if ins.mnemonic=='call' and ins.operands[0].type==CS_OP_IMM:assert ins.operands[0].imm in {a for _,a,_ in spec}
  if ins.mnemonic.startswith('j'):assert address<=ins.operands[0].imm<address+n
  for op in ins.operands:
   if op.type==CS_OP_MEM and 0x400000<=op.mem.disp<0x400000+sz:
    assert not op.mem.base and not op.mem.index and ins.disp_size==4
    assert 0x580000<=op.mem.disp<0x600000 and op.size in [4,8]
    at=ins.address-0x400000+ins.disp_offset;struct.pack_into('<I',image,at,op.mem.disp-0x400000+0x50000000);patches.append(dict(site=ins.address+ins.disp_offset,original=op.mem.disp,mapped=op.mem.disp-0x400000+0x50000000))
authentic_original=bytes(image[0xb5970:0xb5970+31])
assert authentic_original.hex()=='8b018b48040fbe0183e82f780583f80b7c0233c08d1480d904d5a8327600c3','Authentic callback bytes do not match original'
authentic_callback=dict(address=0x4b5970,body_bytes=31,original_sha256=hashlib.sha256(authentic_original).hexdigest(),relocated_operand=0x4b598a,original_table=0x7632a8,fixture_table=0x52000000,contract='FASTCALL ECX cookie, RET0, FLD float32 surface value returns ST0. Native CDECL fixture bridge only adapts cookie; arithmetic/branches are original bytes.')
(OUT/'physical-image.bin').write_bytes(image)
math_source=(ROOT/'src/brender/core/math/matrix34.c').read_text();vector_source=(ROOT/'src/brender/core/math/vector.c').read_text()
math_names=['Matrix34ApplyProductLoaded','Matrix34ApplyProductMemory','Matrix34ApplyAdd','BrMatrix34ApplyV','BrMatrix34TApplyV'];math_bodies='\n'.join(extract(math_source,n) for n in math_names)+'\n'+extract(vector_source,'BrVector3SetFloat')
prefix=r'''
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include <math.h>
#include "rec2_types.h"
#include "49-physics.h"
#include "c2_hooks.h"
#define M(row,col) mat->m[row][col]
static unsigned char*code,*arena,*callback_snapshot;
static float C2_HOOK_CDECL FrictionControlledCallback(void* cookie) {
 double result;unsigned index=*(unsigned*)(arena+0xf100);unsigned*control=(unsigned*)cookie;
 memcpy(callback_snapshot,arena,65536);
 *(unsigned*)(arena+0xf100)=index+1;
 if(control[2]) *(unsigned*)(arena+0xf104)=(_statusfp() & _SW_INVALID);
 if(control[3]) *(unsigned*)(arena+control[3])=control[4];
 memcpy(&result,control,sizeof(result));return (float)result;
}
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[6],builder_stack_count,builder_result,builder_poison,builder_cdecl;
extern unsigned __cdecl BuilderInvoke(void);
'''
candidate_file_text=source.read_text();candidate_text=extract_candidate(candidate_file_text);leaf_text='\n'.join(extract(leaf_file_text,name) for name in leaf_names)
(OUT/'candidate.c').write_text(candidate_text);(OUT/'leaf-candidate.c').write_text(leaf_text);(OUT/'verifier.py').write_bytes(Path(__file__).read_bytes())
native='#if defined(_MSC_VER) && _MSC_VER < 1300\n#pragma auto_inline(off)\n#endif\n'+math_bodies+'\n#if defined(_MSC_VER) && _MSC_VER < 1300\n#pragma auto_inline(on)\n#endif\n'+leaf_text+'\n'+candidate_text

driver=prefix+'\n#ifndef PHYSICAL_ORIGINAL\n'+native+'\n#endif\n'+r'''
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned h[16],result=0,values[4],n=0,i;unsigned entry_offsets[3]={0xc0ac0,0xc1840,0xc18d0};unsigned char*trampoline,*surface;unsigned displacement;if(argc!=3)return 2;
 SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);
 code=VirtualAlloc((void*)0x50000000,IMAGE_SIZE,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);arena=VirtualAlloc((void*)0x20000000,65536,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(code!=(void*)0x50000000||arena!=(void*)0x20000000)return 3;
 callback_snapshot=VirtualAlloc(NULL,65536,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(!callback_snapshot)return 10;
 f=fopen(argv[2],"rb");if(!f)return 4;if(fread(code,IMAGE_SIZE,1,f)!=1)return 5;fclose(f);f=fopen(argv[1],"rb");if(!f)return 6;if(fread(h,sizeof(h),1,f)!=1||fread(arena,65536,1,f)!=1)return 7;fclose(f);
 trampoline=VirtualAlloc((void*)0x51000000,4096,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(trampoline!=(void*)0x51000000)return 9;
 #if defined(REC2_MATCHING) || defined(PHYSICAL_ORIGINAL)
 trampoline[0]=0x51;trampoline[1]=0xe8;displacement=(unsigned)(uintptr_t)FrictionControlledCallback-0x51000006;memcpy(trampoline+2,&displacement,4);trampoline[6]=0x83;trampoline[7]=0xc4;trampoline[8]=4;trampoline[9]=0xc3;
#else
 trampoline[0]=0xe9;displacement=(unsigned)(uintptr_t)FrictionControlledCallback-0x51000005;memcpy(trampoline+1,&displacement,4);
#endif
 if(h[3]) {
  static const unsigned char authentic[]={0x8b,0x01,0x8b,0x48,0x04,0x0f,0xbe,0x01,0x83,0xe8,0x2f,0x78,0x05,0x83,0xf8,0x0b,0x7c,0x02,0x33,0xc0,0x8d,0x14,0x80,0xd9,0x04,0xd5,0xa8,0x32,0x76,0x00,0xc3};
  surface=VirtualAlloc((void*)0x52000000,4096,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(surface!=(void*)0x52000000)return 11;
  memcpy(surface,arena+0xe800,440);memcpy(trampoline+64,authentic,sizeof(authentic));displacement=0x52000000;memcpy(trampoline+64+26,&displacement,4);
#if defined(REC2_MATCHING) || defined(PHYSICAL_ORIGINAL)
  trampoline[0]=0xe9;displacement=59;memcpy(trampoline+1,&displacement,4);
#else
  trampoline[0]=0x8b;trampoline[1]=0x4c;trampoline[2]=0x24;trampoline[3]=4;trampoline[4]=0xe9;displacement=55;memcpy(trampoline+5,&displacement,4);
#endif
 }
 _clearfp();_controlfp(_PC_53,_MCW_PC);_setmode(_fileno(stdout),_O_BINARY);
 values[0]=(unsigned)(uintptr_t)(arena+h[1]);values[1]=0;n=1;
 if(h[0]==0){n=2;values[1]=h[2]?(unsigned)(uintptr_t)(arena+h[2]):0;}
 else if(h[0]>2)return 8;
#ifdef PHYSICAL_ORIGINAL
 builder_address=(unsigned)(uintptr_t)(code+entry_offsets[h[0]]);
#else
 (void)entry_offsets;
 if(h[0]==0)builder_address=(unsigned)(uintptr_t)NewAddFriction;
 else if(h[0]==1)builder_address=(unsigned)(uintptr_t)PostAddFriction;
 else builder_address=(unsigned)(uintptr_t)DoJointFriction;
#endif
 builder_mode=2;builder_x=values[0];builder_y=values[1];builder_poison=0xcccccccc;
#if defined(REC2_MATCHING) || defined(PHYSICAL_ORIGINAL)
 builder_cdecl=0;builder_stack_count=n>2?n-2:0;for(i=2;i<n;i++)builder_stack[i-2]=values[i];
#else
 builder_cdecl=1;builder_stack_count=n;for(i=0;i<n;i++)builder_stack[i]=values[i];
#endif
 if(BuilderInvoke()){return 14;}
 fwrite(&result,4,1,stdout);fwrite(arena,65536,1,stdout);fwrite(callback_snapshot,65536,1,stdout);return 0;}
'''
driver=driver.replace('IMAGE_SIZE',str(sz))
(OUT/'harness.c').write_text(driver);(OUT/'original.c').write_text('#define PHYSICAL_ORIGINAL\n'+driver)
linked_meta=None
if args.recompiled:
 data=args.recompiled.read_bytes();mapping=json.loads(args.symbols.read_text());assert mapping['original_sha256']==original_hash and mapping['recompiled_sha256']==hashlib.sha256(data).hexdigest()
 symbols={x['orig']:x['recomp'] for x in mapping['symbols']};map_path=args.recompiled.with_suffix('.map');map_bytes=map_path.read_bytes()
 functions={int(address,16):(name,owner.strip()) for name,address,owner in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',map_bytes.decode('latin1'),re.M)}
 nt2=struct.unpack_from('<I',data,60)[0];base2=struct.unpack_from('<I',data,nt2+52)[0];size2=struct.unpack_from('<I',data,nt2+80)[0];table2=nt2+24+struct.unpack_from('<H',data,nt2+20)[0]
 flat=bytearray(size2);sections=[]
 for si in range(struct.unpack_from('<H',data,nt2+6)[0]):
  virtual,rva,n,offset=struct.unpack_from('<IIII',data,table2+40*si+8);flags=struct.unpack_from('<I',data,table2+40*si+36)[0];flat[rva:rva+n]=data[offset:offset+n];sections.append((base2+rva,max(virtual,n),flags))
 initial={symbols[address]:name for name,address,_ in spec};todo=list(initial);seen=set();linked_closure=[];patched=set();relocations=[];indirect_calls=[]
 while todo:
  start=todo.pop(0)
  if start in seen:continue
  seen.add(start);end=min(address for address in functions if address>start);body=bytes(flat[start-base2:end-base2]);instructions=list(cs.disasm(body,start));assert sum(i.size for i in instructions)==len(body)
  name=initial.get(start,functions[start][0])
  for ins in instructions:
   if ins.mnemonic=='call' or ins.mnemonic.startswith('j'):
    if ins.operands[0].type!=CS_OP_IMM:
     assert ins.mnemonic=='call' and name=='NewAddFriction',(name,ins.op_str)
     indirect_calls.append(dict(name=name,address=ins.address,operand=ins.op_str,boundary='Canonical dynamic world callback'))
    else:
     target=ins.operands[0].imm
     if not start<=target<end:
      assert target in functions,(name,ins.op_str)
      if target not in initial:assert functions[target][1]=='49-physics.c.obj' and functions[target][0].lstrip('_@').startswith('Friction'),(name,functions[target])
      todo.append(target)
   for operand in ins.operands:
    if operand.type==CS_OP_MEM and base2<=operand.mem.disp<base2+size2:
     assert not operand.mem.base and not operand.mem.index and ins.disp_size==4,(name,ins.op_str)
     section=next(sec for sec in sections if sec[0]<=operand.mem.disp<sec[0]+sec[1]);assert not(section[2]&0x80000020),(name,ins.op_str)
     site=ins.address-base2+ins.disp_offset
     if site not in patched:
      relocations.append(dict(site=site,original=operand.mem.disp,width=operand.size,bits=bytes(flat[operand.mem.disp-base2:operand.mem.disp-base2+operand.size]).hex()));struct.pack_into('<I',flat,site,operand.mem.disp-base2+0x50000000);patched.add(site)
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
 matching_driver=driver.replace(str(sz),str(size2)).replace('{0xc0ac0,0xc1840,0xc18d0}','{'+','.join(hex(symbols[address]-base2) for address in [0x4c0ac0,0x4c1840,0x4c18d0])+'}')
 (OUT/'matching.c').write_text('#define PHYSICAL_ORIGINAL\n'+matching_driver)
 linked_meta=dict(recompiled_sha256=hashlib.sha256(data).hexdigest(),symbols_sha256=hashlib.sha256(args.symbols.read_bytes()).hexdigest(),native_map_sha256=hashlib.sha256(map_bytes).hexdigest(),closure=linked_closure,constant_relocations=relocations,indirect_callbacks=indirect_calls)
includes=[ROOT/v for v in ('src/brender/include','src/carma2/newcommon','src/carma2/platform','src/carma2/include','src/carma2/hooks','src/carma2/s3/include')]
mroot=ROOT/'build-tools/msvc-modern';msvc=sorted((mroot/'VC/Tools/MSVC').iterdir())[-1];sdk=mroot/'Windows Kits/10';version=sorted((sdk/'Include').iterdir())[-1].name
env=dict(os.environ);env['PATH']=str(msvc/'bin/Hostx64/x86')+os.pathsep+env['PATH'];env['INCLUDE']=os.pathsep.join(map(str,[msvc/'include',*[sdk/f'Include/{version}/{part}' for part in ('ucrt','shared','um')]]));env['LIB']=os.pathsep.join(map(str,[msvc/'lib/x86',*[sdk/f'Lib/{version}/{part}/x86' for part in ('ucrt','um')]]))
m5=ROOT/'build-tools/msvc500sp3';env5=dict(os.environ);env5['PATH']=str(m5/'bin')+os.pathsep+str(m5/'redist')+os.pathsep+env5['PATH'];env5['INCLUDE']=str(m5/'include');env5['LIB']=str(m5/'lib')
gcc=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'
commands={
 'msvc':[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(q)for q in includes],str(OUT/'harness.c'),'/Fo'+str(OUT/'msvc.obj'),'/Fe'+str(OUT/'msvc.exe')],
 'msvc5':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(q)for q in includes],str(OUT/'harness.c'),'/Fo'+str(OUT/'msvc5.obj'),'/Fe'+str(OUT/'msvc5.exe')],
 'mingw':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation',*['-I'+str(q)for q in includes],str(OUT/'harness.c'),'-o',str(OUT/'mingw.exe')],
 'original':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation',*['-I'+str(q)for q in includes],str(OUT/'original.c'),'-o',str(OUT/'original.exe')]}
caller_tree=ast.parse((ROOT/'scripts/verify_opponent_navigation_reconstruction.py').read_text())
caller_nodes=[node.value.value for node in ast.walk(caller_tree) if isinstance(node,ast.Assign) and isinstance(node.value,ast.Constant) and any(isinstance(target,ast.Name) and target.id=='caller' for target in node.targets)]
assert len(caller_nodes)==1
caller=caller_nodes[0].replace('builder_stack[5]','builder_stack[6]').replace('builder_poison;','builder_poison,builder_cdecl;')
caller=caller.replace(' mov builder_result,eax\n mov ecx,esp',' mov builder_result,eax\n cmp builder_cdecl,0\n je stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n stackready:\n mov ecx,esp')
(OUT/'caller.c').write_text(caller)
if args.recompiled:commands['matching']=[*commands['original'][:-3],str(OUT/'matching.c'),'-o',str(OUT/'matching.exe')]
for command in commands.values():command.append(str(OUT/'caller.obj'))
commands={'caller':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Zl','/c',str(OUT/'caller.c'),'/Fo'+str(OUT/'caller.obj')],**commands}
for name,cmd in commands.items():
 r=subprocess.run(cmd,cwd=OUT,env=env if name=='msvc'else env5 if name in ('msvc5','caller')else None,capture_output=True,text=True);(OUT/(name+'-build.txt')).write_text(r.stdout+r.stderr)
 if r.returncode:raise RuntimeError(r.stdout+r.stderr)
rng=random.Random(0x4c0ac0);cases=[]
def scene(mode,second=True,kind=0,seed=0,graph=0,patch=None,callback=None,alias=None,authentic=None,ordering_probe=None):
 h=[mode,0x4000 if mode==0 else 0x1000,0]+[0]*13;arena=bytearray(b'\xa5'*65536)
 def words(at,*v):struct.pack_into('<'+'I'*len(v),arena,at,*[x&0xffffffff for x in v])
 def floats(at,*v):struct.pack_into('<'+'f'*len(v),arena,at,*v)
 for i,off in enumerate([0x1000,0x1800,0x2000,0x2800]):
  arena[off:off+0x500]=bytes(0x500);words(off,0x20006000+0x100*i);floats(off+4,2,1,2,3);floats(off+0x60,.75,.5);floats(off+0x68,.2,-.4,.1);floats(off+0x74,2,-3,4);floats(off+0x1a8,.6,-.8,1.2);floats(off+0x8c,1,0,0,0,1,0,0,0,1,.1,.2,.3);floats(0x602c+0x100*i,1,0,0,0,1,0,0,0,1,0,0,0)
 arena[0x4000:0x4060]=bytes(96);floats(0x4000,.1);words(0x4004,0x20001000,0x20001800 if second else 0);floats(0x4014,.5,-.4,.3,-.2,.1,-.7,1,0,0,-1,0,0);words(0x405c,kind)
 floats(0x5004,.8);floats(0x5104,.6)
 if graph in [1,3]:words(0x1188,0x20005000);words(0x1230,0x20001800)
 if graph in [2,3]:words(0x118c,0x20005100)
 if graph==4:words(0x1224,0x20001800);words(0x198c,0x20005100)
 if graph==5:words(0x1188,0x20005000);words(0x1230,0x20001000)
 if graph in [6,7]:
  words(0x1224,0x20001800);words(0x1a20,0x20002800);words(0x1a24,0x20002000);words(0x198c,0x20005100);words(0x218c,0x20005100);words(0x298c,0x20005100);arena[0x18ec]=1 if graph==6 else 128
 if graph==8:words(0x1188,0x20005000);words(0x1230,0x20001800);words(0x118c,0x20005000)
 if seed:
  for at in [0x1074,0x1874,0x2074,0x11a8,0x19a8,0x21a8,0x4014,0x4020,0x402c]:floats(at,*[rng.uniform(-3,3) for _ in range(3)])
  for at in [0x108c,0x188c,0x602c,0x612c]:floats(at,*[rng.uniform(-1,1) for _ in range(12)])
 if callback is not None:
  h[2]=0xe000;arena[0xe000:0xe020]=bytes(32);words(0xe018,0x51000000);words(0x400c,0x2000f000);arena[0xf000:0xf200]=bytes(512)
  struct.pack_into('<d',arena,0xf000,callback.get('value',1.25));words(0xf008,callback.get('status',0),callback.get('write',0),callback.get('word',0))
 if authentic is not None:
  h[2]=0xe000;h[3]=1;arena[0xe000:0xe020]=bytes(32);words(0xe018,0x51000000);words(0x400c,0x2000f000)
  arena[0xe800:0xf400]=bytes(3072);words(0xf000,0x2000f200);words(0xf204,0x2000f300);arena[0xf300]=authentic['identifier']&255
  for i in range(11):words(0xe800+40*i,authentic['word'] if i==authentic['index'] else 0x3f400000)
 if alias is not None:
  assert mode==0 and callback is None
  arena[alias:alias+96]=arena[0x4000:0x4060];h[1]=alias
  # Keep aliases that overlap live record owner pointers terminating/valid:
  # zero cap plus no-rotation leaves their encoded pointer words unchanged.
  words(alias,0);words(0x119c,0x800);words(0x199c,0x800)
 for at,value in patch or []:words(at,value)
 cases.append(dict(mode=mode,second=second,kind=kind,seed=seed,graph=graph,patch=patch,callback=callback,alias=alias,authentic=authentic,ordering_probe=ordering_probe,header=h,data=bytes(arena)))
for second in [True,False]:
 for kind in [0,1,14]:
  for seed in range(21):scene(0,second,kind,seed)
for seed in range(21):scene(1,seed=seed)
for graph in range(5):
 for seed in range(13):scene(2,graph=graph,seed=seed)

for second,kind in [(False,0),(True,14),(False,1)]:
 for seed in range(5):
  for value in [0.,.125,1.25,-.75,100.]:scene(0,second,kind,seed,callback={'value':value})
  for at,word in [(0x4000,0x3e4ccccd),(0x405c,0x100),(0x1074,0x40000000),(0x4004,0x20001800),(0x108c,0x3f000000)]:scene(0,second,kind,seed,callback={'value':1.25,'write':at,'word':word})
for mode in [0,1,2]:
 for at in ([0x4000,0x1074,0x1080,0x4004] if mode==0 else [0x1074,0x1078,0x107c,0x1008,0x100c]):
  if at in [0x4004,0x1080]:continue
  for word in [0,0x80000000,1,0x7f800000,0xff800000,0x7fc12345,0xffc12345,0x7f812345,0xff812345]:
   if mode==0:
    for second,kind in [(True,0),(False,0),(True,14),(False,1)]:scene(mode,second,kind,2,patch=[(at,word)])
   else:scene(mode,seed=2,graph=3 if mode==2 else 0,patch=[(at,word)])

# Bounded paired raw IEEE probes across original memory operands and frame routes.
paired_words=[(0x7f801234,0x7fc00001),(0x7fc00001,0x7f801234),(0xff801234,0x7fc00001),(0x7f800001,0xffc01234),(0x7fc01234,0xff800001),(0xffc01234,0x7f800001)]
for first,second in [(0x1074,0x1874),(0x1008,0x1808),(0x108c,0x1874),(0x108c,0x1008),(0x5004,0x1074),(0x5104,0x1074),(0x1074,0x1078)]:
 for a,b in paired_words:scene(2,seed=3,graph=3,patch=[(first,a),(second,b)])
for first,second in [(0x4000,0x1074),(0x4014,0x1078),(0x4018,0x107c),(0x4000,0x1064),(0x1008,0x1074),(0x402c,0x108c)]:
 for a,b in paired_words:
  for second_object,kind in [(True,0),(False,0),(False,1)]:scene(0,second_object,kind,3,patch=[(first,a),(second,b)])
for a in [0x7f800001,0xff800001,0x7f801234,0xff801234,0x7fc00001,0xffc00001]:
 scene(0,False,0,3,patch=[(0x4000,a)],callback={'value':1.25,'status':1})
for a,b in paired_words:
 scene(0,False,0,3,patch=[(0x4000,a),(0xf004,0x7ff00000|((b>>3)&0xfffff)),(0xf000,(b<<29)&0xffffffff)],callback={'value':1.25,'status':1})

def fbits(x):return struct.unpack('<I',struct.pack('<f',x))[0]
threshold=struct.unpack('<f',struct.pack('<I',0x3a83126f))[0]
previous=struct.unpack('<f',struct.pack('<I',0x3a83126e))[0]
tiny=math.sqrt((threshold-(threshold-previous)*.25)**2-previous**2)
# Original retains wide FSQRT/normal-dot comparison value despite FST32.
for rare in [False,True]:
 patch=[(0x1074,fbits(.02 if rare else 0.)),(0x1078,fbits(3. if rare else 0.)),(0x107c,fbits(4. if rare else 0.)),(0x1068,0),(0x106c,0),(0x1070,0),(0x11a8,0),(0x11ac,fbits(previous)),(0x11b0,fbits(tiny))]
 scene(0,False,0,patch=patch)
scene(0,False,0,patch=[(0x1074,fbits(.1)),(0x1078,fbits(3.)),(0x107c,fbits(4.)),(0x1068,0),(0x106c,0),(0x1070,0),(0x11a8,0),(0x11ac,0),(0x11b0,0),(0x402c,fbits(1.)),(0x4030,fbits(-2.**-32)),(0x4034,0)])
for velocity in [0.,.2,-.2]:
 scene(0,False,0,patch=[(0x1074,fbits(.02)),(0x1078,fbits(3.)),(0x107c,fbits(4.)),(0x1068,0),(0x106c,0),(0x1070,0),(0x11a8,0),(0x11ac,fbits(velocity)),(0x11b0,0)])
for graph in [5,6,7,8]:
 for seed in range(6):scene(2,seed=seed,graph=graph)
for second,kind in [(True,0),(False,0),(False,1),(True,14)]:
 for alias in [0x1068,0x1074,0x108c,0x11a8,0x1868,0x1874,0x188c]:scene(0,second,kind,2,alias=alias)
 for rotation in [0x800,0xffffffff,0x80000000]:scene(0,second,kind,2,patch=[(0x119c,rotation),(0x199c,rotation)])
for mode in [0,2]:
 for at in [0x1004,0x1008,0x100c,0x1010,0x1808,0x4000,0x5004,0x5104]:
  for value in [0.,-0.,2.**-149,2.**-100,2.**100,-2.**100,float('inf'),float('-inf')]:
   if mode==0:
    for second,kind in [(True,0),(False,0)]:scene(0,second,kind,3,patch=[(at,fbits(value))])
   else:scene(2,seed=3,graph=3,patch=[(at,fbits(value))])
for null_at in [0x400c,0xe018]:scene(0,False,0,3,patch=[(null_at,0)],callback={'value':1.25})

# Actual original31-byte GetFrictionFromFace callback, canonical float API.
for identifier in [0,46,47,48,57,58,59,127,128,255]:
 index=(identifier if identifier<128 else identifier-256)-47
 index=index if 0<=index<11 else 0
 for word in [0,0x80000000,1,0x3f800001,0x7f800000,0xff800000,0x7fc12345,0x7f801234,0xffc12345,0xff801234]:
  scene(0,False,0,3,authentic={'identifier':identifier,'index':index,'word':word})
for a,b in paired_words:
 scene(0,False,0,3,patch=[(0x4000,a)],authentic={'identifier':48,'index':1,'word':b})

# Callback mutations occur after cached tangent/normal, before live response/actor reads.
scene(0,False,0,3,callback={'value':1.25,'write':0x4014,'word':fbits(-.75)},ordering_probe='live lever after callback')
scene(0,False,0,3,callback={'value':1.25,'write':0x1000,'word':0x20006100},ordering_probe='live actor after callback')
results=[];failures=[]
for index,c in enumerate(cases[:args.case_limit]):
 if index<args.case_start:continue
 f=OUT/'input.bin';f.write_bytes(struct.pack('<16I',*c['header'])+c['data']);oracle=subprocess.run([str(OUT/'original.exe'),str(f),str(OUT/'physical-image.bin')],capture_output=True);assert oracle.returncode==0 and len(oracle.stdout)==131076,(index,oracle.returncode)
 if c.get('ordering_probe'):
  assert struct.unpack_from('<I',oracle.stdout,4+0x405c)[0]&0x100,'Ordering probe must take active positive impulse path'
  assert any(struct.unpack_from('<f',oracle.stdout,4+0x4044+4*j)[0]!=0 for j in range(3)),'Ordering probe must commit a nonzero impulse'
 for compiler in ['msvc5','msvc','mingw']+(['matching'] if args.recompiled else []):
  actual=subprocess.run([str(OUT/(compiler+'.exe')),str(f),str(OUT/('matching-image.bin' if compiler=='matching' else 'physical-image.bin'))],capture_output=True);ok=actual.returncode==0 and actual.stdout==oracle.stdout
  row=dict(index=index,case={k:v for k,v in c.items() if k!='data'},compiler=compiler,passed=ok)
  if not ok:
   row.update(returncode=actual.returncode,differences=[dict(offset=i,original=oracle.stdout[i:i+4].hex(),actual=actual.stdout[i:i+4].hex()) for i in range(0,min(len(actual.stdout),131076),4) if oracle.stdout[i:i+4]!=actual.stdout[i:i+4]])
   (OUT/('failure-%d-%s.bin'%(index,compiler))).write_bytes(f.read_bytes());failures.append(row)
  results.append(row)
 if index%100==0:print('scenes',index+1,'failures',len(failures),flush=True)
 if len(failures)>=args.max_failures:break
source_after={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in source_paths}
assert leaf_text=='\n'.join(extract(leaves.read_text(),n) for n in leaf_names),'Verified lower49 bodies changed during proof'
assert candidate_text==extract_candidate(source.read_text()),'Verified candidate bodies changed during proof'
assert math_bodies=='\n'.join(extract((ROOT/'src/brender/core/math/matrix34.c').read_text(),n) for n in math_names)+'\n'+extract(vector_source,'BrVector3SetFloat'),'Verified math bodies changed during proof'
v=dict(authentic_callback=authentic_callback,matching=linked_meta,generated_files_sha256={p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in OUT.iterdir() if p.suffix in ('.c','.obj','.exe')},fixture_sha256=hashlib.sha256(b''.join(struct.pack('<16I',*c['header'])+c['data'] for c in cases[args.case_start:args.case_limit])).hexdigest(),case_start=args.case_start,source_before=source_before,source_after=source_after,whole_sources_unchanged=source_before==source_after,verified_extractions_unchanged=True,source_sha256=hashlib.sha256(source.read_bytes()).hexdigest(),source_normalized_text_sha256=hashlib.sha256(candidate_file_text.encode()).hexdigest(),verifier_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),candidate_sha256=hashlib.sha256(candidate_text.encode()).hexdigest(),leaf_source_sha256=hashlib.sha256(leaf_text.encode()).hexdigest(),original_sha256=original_hash,entries=entries,patches=patches,scenario_count=len(cases),executed_scenarios=len(results)//(4 if args.recompiled else 3),comparisons=len(results),results=results,failures=failures,compiler_commands=commands,scope='Development complete friction closure comparison; canonical float callback declaration retained. Complete original3 bodies physically atPC53, actual native matrix/vector helpers. Strict65536byte arena; ESP/nonvolatiles/x87TOP checked. Controlled callbacks are float-declared with float-representable raw results; valid graph mutations and lossless65536-byte pre-callback snapshot included. Authentic original GetFrictionFromFace bytes also execute in106 consumer scenes. Callback invalid-status probes observe the controlled callback explicit invalid-bit predicate; no general output mask. Bounded valid accessible graphs, x86 PC53/default masked exceptions only; no unmasked traps/nonx86 claim.')
(OUT/'verification.json').write_text(json.dumps(v,indent=2)+'\n');print('complete',index+1,len(results),'failures',len(failures))
