"""Physical original/native/linked driver for the complete joint response graph.

Modes0/1: inverse/force; modes2/3: torque/force partner; mode4: MakeMatrix.
Input16DWORDheader +65536-byte raw arena. All pointer identities use fixed heap
base0x20000000. Output4-byte returned partner pointer +complete unmasked arena.
Only original absolute constant addresses are relocated in the physical PE;
original internal/relative dependency calls remain untouched.
"""
import ast,argparse,hashlib,json,os,random,re,struct,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_MEM,CS_OP_IMM
from verify_tinted_native_reconstruction import extract
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',default='build-audit/physics-force-matrix-tests');p.add_argument('--matrix-source',type=Path);p.add_argument('--partner-source',type=Path);p.add_argument('--symbols',type=Path);p.add_argument('--recompiled',type=Path);p.add_argument('--case-limit',type=int);p.add_argument('--max-failures',type=int,default=20);p.add_argument('--paired-only',action='store_true');p.add_argument('--pointerarray-alias-only',action='store_true',help='Run only the retained MakeMatrix output/pointer-array overlap fixture');args=p.parse_args()
OUT=ROOT/args.output;OUT.mkdir(parents=True,exist_ok=True)
assert bool(args.matrix_source)==bool(args.partner_source),'Supply both development source paths or neither'
assert bool(args.recompiled)==bool(args.symbols),'Supply both immutable PE and symbol map or neither'
leaves=ROOT/'src/carma2/newcommon/49-physics.c'
source_paths=[leaves,leaves.with_suffix('.h'),ROOT/'src/brender/core/math/matrix34.c',ROOT/'src/brender/core/math/vector.c',ROOT/'src/carma2/include/rec2_types.h',ROOT/'src/carma2/hooks/c2_hooks.h',Path(__file__),ROOT/'scripts/verify_opponent_navigation_reconstruction.py',ROOT/'scripts/verify_tinted_native_reconstruction.py']
if args.partner_source:source_paths += [args.partner_source,args.matrix_source]
source_before={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in source_paths}

original=(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes();original_hash=hashlib.sha256(original).hexdigest();assert original_hash=='9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
nt=struct.unpack_from('<I',original,60)[0];table=nt+24+struct.unpack_from('<H',original,nt+20)[0];sz=struct.unpack_from('<I',original,nt+80)[0];image=bytearray(sz)
for i in range(struct.unpack_from('<H',original,nt+6)[0]):
 _,rva,n,off=struct.unpack_from('<IIII',original,table+40*i+8);image[rva:rva+n]=original[off:off+n]
spec=[('DRMatrix33Inverse',0x4be350,352),('SetUpForce',0x4be4b0,159),('SetUpTorqueHingePartners',0x4be550,1943),('SetUpHingePartners',0x4becf0,1942),('MakeMatrix',0x4bf490,3922),('BrMatrix34ApplyV',0x533520,107),('BrMatrix34TApplyV',0x533690,107),('BrVector3SetFloat',0x534590,25)]
cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;patches=[];entries=[]
for name,address,n in spec:
 body=bytes(image[address-0x400000:address-0x400000+n]);instructions=list(cs.disasm(body,address));assert sum(i.size for i in instructions)==n
 entries.append(dict(name=name,address=address,bytes=n,sha256=hashlib.sha256(body).hexdigest()))
 for ins in instructions:
  if ins.mnemonic=='call':assert ins.operands[0].type==CS_OP_IMM and ins.operands[0].imm in {a for _,a,_ in spec}
  if ins.mnemonic.startswith('j'):assert address<=ins.operands[0].imm<address+n
  for op in ins.operands:
   if op.type==CS_OP_MEM and 0x400000<=op.mem.disp<0x400000+sz:
    assert not op.mem.base and not op.mem.index and ins.disp_size==4
    assert op.mem.disp==0x58a780 and op.size==8 and image[0x18a780:0x18a788]==struct.pack('<d',1.)
    at=ins.address-0x400000+ins.disp_offset;struct.pack_into('<I',image,at,op.mem.disp-0x400000+0x50000000);patches.append(dict(site=ins.address+ins.disp_offset,original=op.mem.disp,mapped=op.mem.disp-0x400000+0x50000000))
(OUT/'physical-image.bin').write_bytes(image)
math_source=(ROOT/'src/brender/core/math/matrix34.c').read_text();vector_source=(ROOT/'src/brender/core/math/vector.c').read_text()
math_bodies='\n'.join(extract(math_source,n) for n in ['BrMatrix34ApplyV','BrMatrix34TApplyV'])+'\n'+extract(vector_source,'BrVector3SetFloat')
prefix=r'''
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "49-physics.h"
#include "c2_hooks.h"
#define M(row,col) mat->m[row][col]
typedef void(__fastcall*inverse_fn)(br_scalar*,const br_scalar*);
typedef void(__fastcall*force_fn)(br_vector3*,br_vector3*,const br_vector3*,const br_vector3*,const tPhysics_object*);
typedef tPhysics_object*(__fastcall*torque_fn)(br_vector3*,br_vector3*,br_vector3*,br_vector3*,const br_vector3*,tPhysics_object*);
typedef tPhysics_object*(__fastcall*hinge_fn)(br_vector3*,br_vector3*,br_vector3*,br_vector3*,tPhysics_object*);
typedef void(__fastcall*matrix_fn)(br_scalar*,int,void*,int);
static unsigned char*code,*arena;
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[6],builder_stack_count,builder_result,builder_poison,builder_cdecl;
extern unsigned __cdecl BuilderInvoke(void);
'''
production=leaves.read_text()
if args.partner_source:
 candidate_text=args.partner_source.read_text()+'\n'+args.matrix_source.read_text()
else:
 candidate_text=production[production.index('/* Hinge response workspace. */'):production.index('// RestoreListOmega')]
leaf_text='\n'.join(extract(production,name) for name in ['DRMatrix33Inverse','SetUpForce'])
(OUT/'candidate.c').write_text(candidate_text);(OUT/'leaf-candidate.c').write_text(leaf_text);(OUT/'verifier.py').write_bytes(Path(__file__).read_bytes())
native=math_bodies+'\n'+leaf_text+'\n'+candidate_text
driver=prefix+'\n#ifndef PHYSICAL_ORIGINAL\n'+native+'\n#endif\n'+r'''
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned h[16],result=0,values[6],n=0,i;unsigned entry_offsets[5]={ENTRY_OFFSETS};if(argc!=3)return 2;
 SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);
 code=VirtualAlloc((void*)0x50000000,IMAGE_SIZE,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);arena=VirtualAlloc((void*)0x20000000,65536,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(code!=(void*)0x50000000||arena!=(void*)0x20000000)return 3;
 f=fopen(argv[2],"rb");if(!f)return 4;if(fread(code,IMAGE_SIZE,1,f)!=1)return 5;fclose(f);f=fopen(argv[1],"rb");if(!f)return 6;if(fread(h,sizeof(h),1,f)!=1||fread(arena,65536,1,f)!=1)return 7;fclose(f);
 _controlfp(_PC_53,_MCW_PC);_setmode(_fileno(stdout),_O_BINARY);
 values[0]=(unsigned)(uintptr_t)(arena+h[1]);values[1]=(unsigned)(uintptr_t)(arena+h[2]);
 if(h[0]==0){values[1]=(unsigned)(uintptr_t)(arena+h[5]);n=2;}
 else if(h[0]==1){values[2]=(unsigned)(uintptr_t)(arena+h[5]);values[3]=(unsigned)(uintptr_t)(arena+h[8]);values[4]=(unsigned)(uintptr_t)(arena+h[6]);n=5;}
 else if(h[0]==2){values[2]=(unsigned)(uintptr_t)(arena+h[3]);values[3]=(unsigned)(uintptr_t)(arena+h[4]);values[4]=(unsigned)(uintptr_t)(arena+h[5]);values[5]=(unsigned)(uintptr_t)(arena+h[6]);n=6;}
 else if(h[0]==3){values[2]=(unsigned)(uintptr_t)(arena+h[3]);values[3]=(unsigned)(uintptr_t)(arena+h[4]);values[4]=(unsigned)(uintptr_t)(arena+h[6]);n=5;}
 else if(h[0]==4){values[1]=h[7];values[2]=(unsigned)(uintptr_t)(arena+h[9]);values[3]=h[10];n=4;}
 else return 8;
#ifdef PHYSICAL_ORIGINAL
 builder_address=(unsigned)(uintptr_t)(code+entry_offsets[h[0]]);
#else
 (void)entry_offsets;
 if(h[0]==0)builder_address=(unsigned)(uintptr_t)DRMatrix33Inverse;
 else if(h[0]==1)builder_address=(unsigned)(uintptr_t)SetUpForce;
 else if(h[0]==2)builder_address=(unsigned)(uintptr_t)SetUpTorqueHingePartners;
 else if(h[0]==3)builder_address=(unsigned)(uintptr_t)SetUpHingePartners;
 else builder_address=(unsigned)(uintptr_t)MakeMatrix;
#endif
 builder_mode=(h[0]==2||h[0]==3)?0:2;builder_x=values[0];builder_y=values[1];builder_poison=0xcccccccc;
#if defined(REC2_MATCHING) || defined(PHYSICAL_ORIGINAL)
 builder_cdecl=0;builder_stack_count=n-2;for(i=2;i<n;i++)builder_stack[i-2]=values[i];
#else
 builder_cdecl=1;builder_stack_count=n;for(i=0;i<n;i++)builder_stack[i]=values[i];
#endif
 if(BuilderInvoke())return 14;result=builder_result;
 fwrite(&result,4,1,stdout);fwrite(arena,65536,1,stdout);return 0;}
'''

driver=driver.replace('IMAGE_SIZE',str(sz)).replace('ENTRY_OFFSETS',','.join(hex(address-0x400000) for _,address,_ in spec[:5]))
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
     if target not in initial:assert functions[target][1]=='49-physics.c.obj' and functions[target][0].lstrip('_@').startswith(('Force','Partner')),(name,functions[target])
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
 matching_driver=driver.replace(str(sz),str(size2)).replace(','.join(hex(address-0x400000) for _,address,_ in spec[:5]),','.join(hex(symbols[address]-base2) for _,address,_ in spec[:5]))
 (OUT/'matching.c').write_text('#define PHYSICAL_ORIGINAL\n'+matching_driver)
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
caller=caller_nodes[0].replace('builder_stack[5]','builder_stack[6]').replace('builder_poison;','builder_poison,builder_cdecl;')
caller=caller.replace(' mov builder_result,eax\n mov ecx,esp',' mov builder_result,eax\n cmp builder_cdecl,0\n je stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n stackready:\n mov ecx,esp')
(OUT/'caller.c').write_text(caller)
for command in commands.values():command.append(str(OUT/'caller.obj'))
commands={'caller':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Zl','/c',str(OUT/'caller.c'),'/Fo'+str(OUT/'caller.obj')],**commands}

for name,cmd in commands.items():
    r=subprocess.run(cmd,cwd=OUT,env=env if name=='msvc'else env5 if name in ('msvc5','caller')else None,capture_output=True,text=True);(OUT/(name+'-build.txt')).write_text(r.stdout+r.stderr)
    if r.returncode:raise RuntimeError(r.stdout+r.stderr)
rng=random.Random(0x4be550);cases=[]
def scene(mode,relation,seed=0,patch=None,alias=None,flag=0):
 h=[mode,0,16,32,48,0x800,0x1000,4,0x840,0x3000,4]+[0]*5;arena=bytearray(b'\xa5'*65536)
 def words(at,*values):struct.pack_into('<'+'I'*len(values),arena,at,*[v&0xffffffff for v in values])
 def floats(at,*values):struct.pack_into('<'+'f'*len(values),arena,at,*values)
 for i,off in enumerate([0x1000,0x1800]):
  arena[off:off+0x300]=bytes(0x300);floats(off+4,2.375,1.25,4.75,8.0625);floats(off+0x14,.2,-.1,.3);floats(off+0x8c,1,0,0,0,1,0,0,0,1,0,0,0);words(off+0x188,0x20002800+128*i);words(off+0x1e4,0x20002400+128*i);words(off+0x230,0x20001800 if relation==0 and i==0 else 0);words(off+0x19c,flag)
  words(0x2400+128*i,i,0x20001800 if i==0 else 0x20001000);floats(0x2408+128*i,1,0,0,0,1,0,0,0,1)
  arena[0x2800+128*i:0x2850+128*i]=bytes(80);floats(0x2808+128*i,.4,.3,.2,.6,-.2,.7,1,0,0,0,1,0,.3,.5,.7,-.4,.8,.2)
 if relation==2:words(0x11e4,0)
 floats(0,1.25,-2.5,3.125);floats(16,.1,.2,.3);floats(32,-.25,.5,.75);floats(48,.2,-.1,.4);floats(0x800,2.375,-4.125,8.0625,1,0,0,0,1,0);floats(0x840,2.375,-4.125,8.0625)
 if seed:
  for at in [0,16,32,48,0x800,0x1004,0x1804,0x1014,0x1814,0x2808,0x2814,0x2820,0x282c,0x2838,0x2844,0x2888,0x2894,0x28a0,0x28ac,0x28b8,0x28c4]:floats(at,*[rng.uniform(-2,2) for _ in range(3)])
  for at in [0x2408,0x2488,0x108c,0x188c]:floats(at,*[rng.uniform(-1,1) for _ in range(9)])
 for at,v in patch or []:words(at,v)
 if alias:
  for k,v in alias.items():h[k]=v
 cases.append(dict(mode=mode,relation=relation,seed=seed,flag=flag,alias=alias,patch=patch,header=h,data=bytes(arena)))
for mode in [2,3]:
 for relation in ([0,1,2] if mode==2 else [0,1]):
  scene(mode,relation)
  for seed in range(1,25):scene(mode,relation,seed)
  for flag in [0x800,0xffffffff,0x7ff]:scene(mode,relation,flag=flag)
  for offset in [0,4,16,20,32,48,0x800,0x804,0x1004,0x1008,0x1804,0x1808]:scene(mode,relation,alias={3:offset})
  for at in [0,4,8,16,20,24,0x800,0x804,0x1004,0x1008,0x1014,0x1814,0x2408,0x2808,0x2838,0x2844]:
   for word in [0x80000000,1,0x7f800000,0xff800000,0x7fc00011,0xffc00022,0x7f800011,0xff800022]:scene(mode,relation,patch=[(at,word)])
# Bounded additional live alias/reload and finite exponent cases.
for mode in [2,3]:
 for relation in [0,1]:
  for key in [1,2,3,4]:
   for offset in [0,1,4,8,12,16,20,32,48,0x800,0x1004,0x108c,0x1804,0x2408,0x2808,0x2838]:scene(mode,relation,alias={key:offset})
  for offsets in [(0,0,0,0),(0,4,8,12),(12,8,4,0),(0x800,0x804,0x808,0x800)]:scene(mode,relation,alias={k:v for k,v in zip([1,2,3,4],offsets)})
  scene(mode,relation,patch=[(0x2404,0x20001000),(0x1230,0x20001000 if relation==0 else 0)])
  for bits in [0x00800000,0x007fffff,0x3f000001,0xbf000001,0x7f7fffff,0xff7fffff,0x01000000,0x60000001]:
   for at in [0,4,8,0x800,0x1008,0x1014,0x1814,0x2408,0x2838]:scene(mode,relation,patch=[(at,bits)])
if args.paired_only:cases=[]
for mode in [2,3]:
 for relation in [0,1]:
  joint=0x2800 if relation==0 else 0x2880
  pairs=[(joint+(8 if relation==0 else 0x14),0x1014),
         (joint+(0xc if relation==0 else 0x18),0x1018),
         (joint+(0x10 if relation==0 else 0x1c),0x101c),
         (joint+0x20,0x800 if mode==2 else 0),
         (joint+(0x38 if relation==0 else 0x44),0x804 if mode==2 else 4),
         (0x800,0x1008),(0x804,0x100c),(0x808,0x1010),
         (0,0x1008),(4,0x100c),(8,0x1010),
         (0x2408 if relation==0 else 0x2488,joint+0x38)]
  nan_pairs=[(0x7fc12345,0x7f812345),(0x7f812345,0x7fc12345),
             (0xffc12345,0x7f812345),(0x7fc12345,0xff812345),
             (0x7fc00001,0x7f812345),(0x7f812345,0x7fc00001),
             (0x7f812345,0xff812345),(0xff812345,0x7f812345),
             (0x7fc12345,0xffc12345),(0xffc12345,0x7fc12345)]
  for left,right in pairs:
   for a,b in nan_pairs:scene(mode,relation,patch=[(left,a),(right,b)])
def matrix_fixtures():
    rng=random.Random(0x4bf490);cases=[]
    def scene(count,dimension,pattern,index,patch=None,output=0x8000,partners=0):
        arena=bytearray(b'\xa5'*65536)
        def put(at,values):arena[at:at+4*len(values)]=struct.pack('<'+'I'*len(values),*values)
        def f32(at,values):arena[at:at+4*len(values)]=struct.pack('<'+'f'*len(values),*values)
        for i in range(4):
            off=0x800+i*0x800;arena[off:off+1240]=bytes(1240);f32(off+4,[rng.uniform(.25,8) for _ in range(4)])
            if index%9==0:put(off+0x19c,[0x800])
            if partners:
                partner=off if partners==4 else 0x800+0x800*((i+1)%4)
                joint=0x6000+0x100*i;workspace=0x5000+0x100*i
                put(off+0x188,[0x20000000+joint]);put(off+0x1e4,[0x20000000+workspace]);put(off+0x230,[0x20000000+partner if partners in (1,4) or (partners==3 and i%2) else 0])
                f32(off+0x14,[rng.uniform(-2,2) for _ in range(3)])
                f32(off+0x8c,[1,0,0,0,1,0,0,0,1,0,0,0])
                arena[joint:joint+80]=bytes(80)
                f32(joint+8,[rng.uniform(-2,2) for _ in range(18)])
                put(workspace,[i%2,0x20000000+partner])
                f32(workspace+8,[rng.uniform(-1,1) for _ in range(9)])
                f32(workspace+0x38,[1,0,0,0,1,0,0,0,1,0,0,0])
        for i in range(max(count,0)):
            at=0x4000+0x80*i;put(0x3000+4*i,[0x20000000+at]);put(at+4,[0x20000800+0x800*((i+index)%4),0 if index%3==0 else 0x20000800+0x800*((i+index+1)%4)])
            f32(at+0x14,[rng.uniform(-4,4) for _ in range(12)])
            put(at+0x5c,[pattern[i%len(pattern)]])
        for at,word in patch or []:put(at,[word])
        cases.append(dict(index=index,count=count,dimension=dimension,pattern=pattern,output=output,partners=partners,data=bytes(arena)))
    for count in (-1,0):scene(count,4,[0],0)
    for i in range(120):scene(1+i%4,(-1,0,1,2,4,7)[i%6],([0],[11],[14],[0,11,14],[11,0,11,14])[i%5],i)
    for field in (0x402c,0x4030,0x4034,0x4038,0x804,0x808):
        for word in (0x80000000,1,0x7f800000,0xff800000,0x7f800001,0xffc00011):scene(3,4,[0,11,14],2,patch=[(field,word)])
    for i in range(80):
        count=5+i%8
        scene(count,(-2,0,1,count,count+3)[i%5],([0],[11],[14],[10,11,14])[i%4],i+120)
    for output in (0x4014,0x4015,0x4020,0x402c,0x4038,0x405c,0x804,0x808,0x80c,0x810):
        for kind in (0,11,14):scene(1,1,[kind],1,output=output)
    for kind in ([0],[11],[14],[0,11,14]):
        for count in (1,2,3,4):
            fixture_patches=[(0x4004+0x80*i,0x20000800) for i in range(count)]
            fixture_patches += [(0x4008+0x80*i,0x20000800) for i in range(count)]
            scene(count,count,kind,2,patch=fixture_patches)
    for left,right in ((0x7fc12345,0xffc54321),(0x7f812345,0xff854321),(0xffc12345,0x7fc12345),(0x7f800001,0xff800001)):
        for a,b in ((0x4014,0x4018),(0x402c,0x4030),(0x4014,0x402c),(0x804,0x808),(0x4038,0x4020),(0x4080+0x2c,0x4030)):
            for kinds in ([0],[11],[0,11,14]):scene(3,4,kinds,2,patch=[(a,left),(b,right)])
    for partners in (1,2,3,4):
        for i in range(50):scene(1+i%8,(-1,0,1,8)[i%4],([0],[11],[14],[10,11,14])[i%4],i,partners=partners)
        for kinds in ([0],[11],[0,11,14]):
            for field in (0x402c,0x4030,0x804,0x808,0x5008,0x6008,0x6038):
                for bits in (0x80000000,1,0x7f800000,0x7f812345):scene(4,4,kinds,2,patch=[(field,bits)],partners=partners)
    return cases

for c in matrix_fixtures():
 h=[4,c['output'],16,32,48,0x800,0x1000,c['dimension']&0xffffffff,0x840,0x3000,c['count']&0xffffffff]+[0]*5
 cases.append(dict(mode=4,header=h,**c))

def pointerarray_alias_fixture():
 # Independent fixture: reciprocal I.x rounds to the address of a second
 # record. Writing the first result overwrites records[0]; subsequent reads
 # must use the original's captured record and pointer-array reload order.
 h=[4,0x3000,16,32,48,0x800,0x1000,1,0x840,0x3000,1]+[0]*5
 arena=bytearray(65536)
 values={0x804:0x3f800000,0x808:0x5eff7e42,0x80c:0x3f800000,0x810:0x3f800000,
         0x1004:0x3f800000,0x1008:0x3f800000,0x100c:0x3f800000,0x1010:0x3f800000,
         0x3000:0x20004000,0x4004:0x20000800,0x4008:0x20001000,
         0x402c:0x3f800000,0x4038:0x3f800000,0x405c:11,
         0x4104:0x20001000,0x412c:0x40000000,0x415c:11}
 for at,value in values.items():struct.pack_into('<I',arena,at,value)
 digest=hashlib.sha256(struct.pack('<16I',*h)+arena).hexdigest()
 assert digest=='862549ff8b7f6638b13b54c8dc171e29250278da458f264a06cc36de2b3be21b'
 return dict(mode=4,header=h,data=bytes(arena),fixture='pointerarray-alias-independent',input_sha256=digest)

extra=pointerarray_alias_fixture()
cases=[extra] if args.pointerarray_alias_only else [*cases,extra]

results=[];failures=[]
for index,c in enumerate(cases[:args.case_limit]):
 f=OUT/'input.bin';f.write_bytes(struct.pack('<16I',*c['header'])+c['data']);oracle=subprocess.run([str(OUT/'original.exe'),str(f),str(OUT/'physical-image.bin')],capture_output=True);assert oracle.returncode==0 and len(oracle.stdout)==65540,(index,oracle.returncode)
 if c.get('fixture')=='pointerarray-alias-independent':
  assert struct.unpack_from('<I',oracle.stdout,4+0x3000)[0]==0xc0000000
  assert hashlib.sha256(oracle.stdout).hexdigest()=='8f2d2769943a4b7831637288c6c4d44ee5576162255e1d4611dd22f1da219e39'
 for compiler in ['msvc5','msvc','mingw']+(['matching'] if args.recompiled else []):
  actual=subprocess.run([str(OUT/(compiler+'.exe')),str(f),str(OUT/('matching-image.bin' if compiler=='matching' else 'physical-image.bin'))],capture_output=True);ok=actual.returncode==0 and actual.stdout==oracle.stdout
  row=dict(index=index,case={k:v for k,v in c.items() if k!='data'},compiler=compiler,passed=ok)
  if not ok:
   row.update(returncode=actual.returncode,differences=[dict(offset=i,original=oracle.stdout[i:i+4].hex(),actual=actual.stdout[i:i+4].hex()) for i in range(0,min(len(actual.stdout),65540),4) if oracle.stdout[i:i+4]!=actual.stdout[i:i+4]])
   (OUT/('failure-%d-%s.bin'%(index,compiler))).write_bytes(f.read_bytes());failures.append(row)
  results.append(row)
 if index%100==0:print('scenes',index+1,'failures',len(failures),flush=True)
 if len(failures)>=args.max_failures:break
source_after={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in source_paths}
assert source_before==source_after,'Verified sources changed during proof'
v=dict(source_sha256=source_before,verified_sources_unchanged=True,fixture_input_sha256=hashlib.sha256(b''.join(struct.pack('<16I',*c['header'])+c['data'] for c in cases[:args.case_limit])).hexdigest(),matching=linked_meta,generated_files_sha256={p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in OUT.iterdir() if p.suffix in ('.c','.obj','.exe')},candidate_sha256=hashlib.sha256(candidate_text.encode()).hexdigest(),leaf_source_sha256=hashlib.sha256(leaf_text.encode()).hexdigest(),original_sha256=original_hash,entries=entries,patches=patches,scenario_count=len(cases),executed_scenarios=index+1,comparisons=len(results),results=results,failures=failures,compiler_commands=commands,scope='Complete joint-response graph, direct ABI caller checks ESP, EBX/ESI/EDI/EBP and x87 TOP; void EAX excluded, returned partner pointer preserved; original full bodies and original matrix/vector dependencies execute physically. Native uses actual corrected leaf and production matrix/vector bodies. Strict returned-pointer and65536-byte arena; no normalization or masks; PC53, default masked FPU exceptions. Default masked FP exceptions, accessible bounded objects; no whole-game or unmasked-FP-trap claim.')
(OUT/'verification.json').write_text(json.dumps(v,indent=2)+'\n');print('complete',index+1,len(results),'failures',len(failures))


if failures:raise SystemExit(1)
