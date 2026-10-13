"""Physical original/native3/linked proof for two real rotation dependencies.

BrMatrix34Rotate532940/333B and BrMatrix34PostTranslate533e60/45B are existing
entries; these corrections get no new-entry credit. Original instructions,
actual compiled source and optional hash-bound matching graph execute with raw
CDECL arguments. All1024 fixture bytes, ESP/nonvolatiles/x87TOP compare atPC53
masked exceptions; true void EAX is excluded. Rotate10640 and Post972 scenes
retain IEEE/paired inputs, finite exponent spread and axis/matrix aliases.
No math mock, output normalization, sticky status/unmasked-trap or non-x86 claim.
"""
import argparse,ast,hashlib,json,random,re,struct,subprocess,sys
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'src/carma2').is_dir());sys.path.insert(0,str(ROOT/'scripts'))
from verify_opponent_route_reconstruction import raw,pe,HASH,words
from verify_tinted_native_reconstruction import extract
from inventory_reconstruction_backlog import mask_c
import verify_phil_registry_reconstruction as registry
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',required=True,type=Path);p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);p.add_argument('--case-index',type=int,action='append');args=p.parse_args();assert bool(args.recompiled)==bool(args.symbols)
out=args.output.resolve();out.mkdir(parents=True,exist_ok=True);source=ROOT/'src/brender/core/math/matrix34.c';consumer=ROOT/'scripts/verify_physics_rotation_reconstruction.py'
paths=[source,source.with_suffix('.h'),Path(__file__),consumer,ROOT/'scripts/verify_phil_registry_reconstruction.py',ROOT/'scripts/verify_opponent_navigation_reconstruction.py',ROOT/'scripts/verify_opponent_route_reconstruction.py',ROOT/'scripts/verify_tinted_native_reconstruction.py',ROOT/'scripts/inventory_reconstruction_backlog.py',ROOT/'src/carma2/hooks/c2_hooks.h',ROOT/'src/carma2/hooks/c2_math.h',ROOT/'src/brender/include/brender/brender.h',ROOT/'src/brender/include/brender/br_types.h']
if args.recompiled:paths.extend([args.recompiled,args.symbols,args.recompiled.with_suffix('.map')])
before={str(p):sha for p in paths for sha in [hashlib.sha256(p.read_bytes()).hexdigest()]};(out/'source-before.json').write_text(json.dumps(before,indent=2)+'\n')
image=(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes();assert hashlib.sha256(image).hexdigest()==HASH;cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
spec=[('BrMatrix34Rotate',0x532940,333),('BrMatrix34PostTranslate',0x533e60,45)]
common=r'''
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "brender/brender.h"
#include "c2_math.h"
#define M(x,y) mat->m[x][y]
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison,builder_cdecl;
extern unsigned __cdecl BuilderInvoke(void);
static unsigned entries[2];
static unsigned char*arena;
static int run(const char*path){FILE*f;unsigned h[6],i;f=fopen(path,"rb");if(!f)return 2;arena=VirtualAlloc((void*)0x23000000,4096,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(arena!=(void*)0x23000000)return 3;if(fread(h,sizeof(h),1,f)!=1||fread(arena,1024,1,f)!=1)return 4;fclose(f);_controlfp(_PC_53,_MCW_PC);_setmode(_fileno(stdout),_O_BINARY);
builder_address=entries[h[0]];builder_mode=1;builder_poison=0xcccccccc;builder_cdecl=1;builder_stack_count=h[0]?4:3;builder_stack[0]=(unsigned)(uintptr_t)(arena+0x100);
if(h[0]){for(i=0;i<3;i++)builder_stack[i+1]=h[i+3];}else{builder_stack[1]=h[1];builder_stack[2]=(unsigned)(uintptr_t)(arena+h[2]);}
if(BuilderInvoke())return 14;fwrite(arena,1024,1,stdout);return 0;}
'''
tree=ast.parse(consumer.read_text());node=next(n for n in tree.body if isinstance(n,ast.FunctionDef)and n.name=='lower_capsule');ctx=dict(extract=extract,mask_c=mask_c,re=re);exec(compile(ast.Module(body=[node],type_ignores=[]),str(consumer),'exec'),ctx);capsule=ctx['lower_capsule']
body=capsule(source.read_text(),'BrMatrix34Rotate','MatrixRotateScalar')+capsule(source.read_text(),'BrMatrix34PostTranslate','MatrixPostTranslateAdd')
native=common+body+'\nint C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;entries[0]=(unsigned)BrMatrix34Rotate;entries[1]=(unsigned)BrMatrix34PostTranslate;return run(argv[1]);}\n'
nav_tree=ast.parse((ROOT/'scripts/verify_opponent_navigation_reconstruction.py').read_text());caller=next(n.value.value for n in ast.walk(nav_tree)if isinstance(n,ast.Assign)and isinstance(n.value,ast.Constant)and any(isinstance(t,ast.Name)and t.id=='caller'for t in n.targets))
caller=caller.replace('builder_poison;','builder_poison,builder_cdecl;').replace(' mov builder_result,eax\n mov ecx,esp',' mov builder_result,eax\n cmp builder_cdecl,0\n je stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n stackready:\n mov ecx,esp')
def physical_source(data,roots,functions=None):
 original=functions is None;todo=[a for _,a,_ in spec]if original else list(roots);seen=set();functions=dict(functions or {});blobs=[];patches=[];constants={};graph=[];absolute_sites=set()
 nt,b,sz,tab=pe(data)
 for start in todo:
  if start in seen:continue
  seen.add(start);end=start+next(n for _,a,n in spec if a==start)if original else min(a for a in functions if a>start);name=next(n for n,a,_ in spec if a==start)if original else functions[start][0]
  if not original and start not in roots:assert functions[start][1]=='matrix34.c.obj'and name.lstrip('_@').startswith(('MatrixRotate','MatrixPostTranslate')),(name,functions[start])
  n=end-start;assert 0<n<=4096;rawbody=raw(data,start,n);pending=[start];visited={};refs=[]
  while pending:
   at=pending.pop()
   while at not in visited:
    assert start<=at<end;ins=next(cs.disasm(raw(data,at,15),at));visited[at]=ins.bytes.hex()
    if ins.mnemonic=='call'or ins.mnemonic.startswith('j'):
     assert ins.operands[0].type==CS_OP_IMM
     target=ins.operands[0].imm
     if start<=target<end:assert ins.mnemonic!='call';pending.append(target)
     else:assert not original and target in functions and ins.imm_size==4;todo.append(target);refs.append(('call',ins.address-start+ins.imm_offset,target,ins.address-start+ins.size))
    for op in ins.operands:
     if op.type==CS_OP_IMM and ins.mnemonic not in ('call',)and not ins.mnemonic.startswith('j'):
      assert not b<=op.imm<b+sz,('Untransported address immediate',name,ins.op_str)
     if op.type==CS_OP_MEM and b<=op.mem.disp<b+sz:
      assert not op.mem.base and not op.mem.index and ins.disp_size==4
      section=next(struct.unpack_from('<I',data,tab+i*40+36)[0]for i in range(struct.unpack_from('<H',data,nt+6)[0])if b+struct.unpack_from('<I',data,tab+i*40+12)[0]<=op.mem.disp<b+struct.unpack_from('<I',data,tab+i*40+12)[0]+max(struct.unpack_from('<I',data,tab+i*40+8)[0],struct.unpack_from('<I',data,tab+i*40+16)[0]))
      assert not section&0x80000020
      key=(op.mem.disp,op.size);constants[key]=raw(data,*key);refs.append(('constant',ins.address-start+ins.disp_offset,key,0))
      absolute_sites.add(ins.address+ins.disp_offset-b)
    if ins.mnemonic.startswith('ret')or ins.mnemonic in('jmp','int3','int'):break
    at+=ins.size
  blobs.append((start,rawbody,refs));graph.append(dict(name=name,address=start,length=n,body_sha256=hashlib.sha256(rawbody).hexdigest(),instructions=sorted(visited.items())))
 # Every relocation in actually reachable code must be classified above.
 rr,rz=struct.unpack_from('<II',data,nt+24+96+5*8);cursor=b+rr;checked=[]
 active={address+k for row in graph for address,code in row['instructions']for k in range(len(code)//2)}
 while cursor<b+rr+rz:
  page,n=struct.unpack('<II',raw(data,cursor,8));assert n>=8
  for word in struct.unpack('<'+'H'*((n-8)//2),raw(data,cursor+8,n-8)):
   if word>>12==3:
    site=page+(word&4095)
    if b+site in active:assert site in absolute_sites,hex(site);checked.append(site)
  cursor+=n
 positions={start:4096*i for i,(start,_,_)in enumerate(blobs)};text=common
 for i,(start,rawbody,_)in enumerate(blobs):text+='static unsigned char code%d[]={%s};\n'%(i,','.join(str(v)for v in rawbody))
 constant_positions={key:4096*len(blobs)+16*i for i,key in enumerate(constants)};space=4096*len(blobs)+16*len(constants)+4096
 init='unsigned char*p;unsigned v;\np=VirtualAlloc(NULL,%d,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!p)return 3;\n'%space
 for i,(start,rawbody,refs)in enumerate(blobs):
  init+='memcpy(p+%d,code%d,sizeof(code%d));\n'%(positions[start],i,i)
  for kind,offset,target,nextip in refs:
   if kind=='call':init+='v=(unsigned)(p+%d)-(unsigned)(p+%d);'%(positions[target],positions[start]+nextip)
   else:init+='v=(unsigned)(p+%d);'%constant_positions[target]
   init+='memcpy(p+%d,&v,4);\n'%(positions[start]+offset)
 for i,(key,bits)in enumerate(constants.items()):text+='static unsigned char constant%d[]={%s};\n'%(i,','.join(str(v)for v in bits));init+='memcpy(p+%d,constant%d,%d);\n'%(constant_positions[key],i,len(bits))
 for i,start in enumerate(roots):init+='entries[%d]=(unsigned)(p+%d);\n'%(i,positions[start])
 text+='int C2_HOOK_CDECL main(int argc,char**argv){'+init+'if(argc!=2)return 1;return run(argv[1]);}\n'
 return text,dict(graph=graph,constants=[dict(address=hex(a),width=n,bits=v.hex())for(a,n),v in constants.items()],relocations_checked=checked)
physical,original_meta=physical_source(image,[a for _,a,_ in spec]);registry.COMMON='';commands=registry.compile_sources(out,native,physical,caller);linked=None
if args.recompiled:
 data=args.recompiled.read_bytes();symbols=json.loads(args.symbols.read_text());assert symbols['original_sha256']==HASH and symbols['recompiled_sha256']==hashlib.sha256(data).hexdigest();mapping={r['orig']:r['recomp']for r in symbols['symbols']};mapbytes=args.recompiled.with_suffix('.map').read_bytes();functions={int(address,16):(name,owner.strip())for name,address,owner in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',mapbytes.decode('latin1'),re.M)}
 text,linked=physical_source(data,[mapping[a]for _,a,_ in spec],functions);(out/'matching.c').write_text(text);command=[str(ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-DREC2_MATCHING',*['-I'+str(ROOT/q)for q in('src/brender/include','src/carma2/include','src/carma2/hooks')],str(out/'matching.c'),str(out/'caller.obj'),'-o',str(out/'matching.exe')];run=subprocess.run(command,capture_output=True,text=True);(out/'matching-build.txt').write_text(run.stdout+run.stderr);assert run.returncode==0,run.stdout+run.stderr;commands['matching']=command;linked.update(recompiled_sha256=hashlib.sha256(data).hexdigest(),symbols_sha256=hashlib.sha256(args.symbols.read_bytes()).hexdigest(),map_sha256=hashlib.sha256(mapbytes).hexdigest())
cases=[];rng=random.Random(0x532940)
float_words=lambda v:[struct.unpack('<I',struct.pack('<f',x))[0]for x in v]
vectors=[float_words(v)for v in((1,0,0),(0,1,0),(0,0,1),(.26726123690605164,.53452247381210327,.80178368091583252),(.6,.8,0),(.3,.4,.5))]
pool=(0,0x80000000,1,0x80000001,0x3f800001,0xbf800001,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc12345,0xffc12345,0x7f812345,0xff812345)
for slot in range(3):
 for value in pool:v=float_words((.3,.4,.5));v[slot]=value;vectors.append(v)
for slots in((0,1),(0,2),(1,2)):
 for a,b in((0x7fc12345,0xffc12345),(0xffc12345,0x7fc12345),(0x7f812345,0xff812345),(0xff812345,0x7f812345),(0x7f812345,0xffc12345),(0xffc12345,0x7f812345),(0x7f800000,0xff800000)):
  v=float_words((.3,.4,.5));v[slots[0]]=a;v[slots[1]]=b;vectors.append(v)
for _ in range(64):vectors.append([rng.getrandbits(1)<<31|rng.randrange(255)<<23|rng.getrandbits(23)for _ in range(3)])
for angle in(0,1,2,0x1111,0x3fff,0x4000,0x4001,0x7fff,0x8000,0x8001,0x9872,0xbfff,0xc000,0xc001,0xfffe,0xffff):
 for vi,v in enumerate(vectors):
  for alias in range(5):
   arena=bytearray(b'\xa5'*1024);offset=0x200 if alias==0 else 0x100+(alias-1)*4;arena[offset:offset+12]=words(v);cases.append(dict(mode=0,angle=angle,vector=vi,alias=alias,input=words([0,angle,offset,0,0,0])+arena))
post_pool=(0,0x80000000,1,0x80000001,0x3f800001,0xbf800001,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc12345,0xffc12345,0x7f812345,0xff812345,0x7fc54321,0xffc54321,0x7f854321,0xff854321)
for a in post_pool:
 for b in post_pool:
  for lane in range(3):
   arena=bytearray(b'\xa5'*1024);old=[0x3e800000,0xbe000000,0x7fc54321];values=[0x3f000001,0xbf000001,0x7fc12345];old[lane]=a;values[lane]=b;arena[0x124:0x130]=words(old);cases.append(dict(mode=1,lane=lane,old=a,argument=b,input=words([1,0,0]+values)+arena))
assert len(cases)==11612;indices=args.case_index or list(range(len(cases)));results=[];failures=[]
for index in indices:
 c=cases[index];path=out/'case.bin';path.write_bytes(c['input']);r=subprocess.run([str(out/'physical.exe'),str(path)],capture_output=True,timeout=30);assert r.returncode==0,('original',index,r.returncode);expected=r.stdout;assert len(expected)==1024
 for compiler in('msvc5','msvc','mingw')+(('matching',)if linked else ()):
  r=subprocess.run([str(out/(compiler+'.exe')),str(path)],capture_output=True,timeout=30);good=r.returncode==0 and r.stdout==expected;row=dict(index=index,mode=c['mode'],compiler=compiler,passed=good,exit=r.returncode,output_sha256=hashlib.sha256(r.stdout).hexdigest());results.append(row)
  if not good:failures.append(row);(out/f'failure-{index}-{compiler}.bin').write_bytes(r.stdout);(out/f'failure-{index}-original.bin').write_bytes(expected);(out/f'failure-{index}-input.bin').write_bytes(c['input'])
 if index%250==0:print(index,len(failures),flush=True)
 if len(failures)>=20:break
after={str(p):hashlib.sha256(p.read_bytes()).hexdigest()for p in paths};assert before==after
manifest=dict(scope=__doc__,source_before=before,source_after=after,source_stable=True,original_sha256=HASH,original=original_meta,matching=linked,total_scenes=len(cases),scenes=indices.index(index)+1,complete_suite=len(indices)==len(cases)and indices.index(index)+1==len(cases),mode_scenes={str(mode):sum(cases[i]['mode']==mode for i in indices[:indices.index(index)+1])for mode in(0,1)},input_sha256=hashlib.sha256(b''.join(c['input']for c in cases)).hexdigest(),comparisons=len(results),results=results,failures=failures,commands=commands,artifact_sha256={str(f):hashlib.sha256(f.read_bytes()).hexdigest()for f in out.iterdir()if f.suffix in('.c','.obj','.exe','.bin','.txt')})
(out/'verification.json').write_text(json.dumps(manifest,indent=2)+'\n');print('complete',manifest['scenes'],len(results),len(failures));assert not failures
