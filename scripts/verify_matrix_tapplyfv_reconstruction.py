"""Full genuine109B TApplyFV physical/native3 raw arenas and real terminal faults.
Original stdcall RET12 and native CDECL RET0 use distinct authentic callers.
MaskedPC53; normal ESP/nonvolatiles/x87TOP, void EAX excluded. AV only observes
fault address and raw public arena prefix; private stack/exhaustive excluded.
"""
import argparse,json,random,struct,subprocess,sys
from pathlib import Path
ROOT=next(p for p in Path(__file__).resolve().parents if(p/'src/carma2').is_dir());P=Path(__file__).resolve().parent;sys.path.insert(0,str(ROOT/'scripts'))
from verify_opponent_route_reconstruction import HASH,sha,raw,words,pe
from verify_tinted_native_reconstruction import extract
import verify_phil_registry_reconstruction as registry
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_MEM,CS_OP_IMM,CS_GRP_JUMP,CS_GRP_CALL
p=argparse.ArgumentParser();p.add_argument('--output',required=True,type=Path);p.add_argument('--candidate',type=Path);p.add_argument('--helpers',action='store_true');p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);a=p.parse_args();assert bool(a.recompiled)==bool(a.symbols);O=a.output.resolve();assert not O.exists();O.mkdir(parents=True)
matrix=ROOT/'src/brender/core/math/matrix34.c';original=ROOT/'build-reference/originals/CARMA2_HW.EXE';candidate=a.candidate or matrix
paths=[Path(__file__),candidate,matrix,original]+list((ROOT/'src').rglob('*.h'))+[ROOT/'scripts'/n for n in['verify_phil_registry_reconstruction.py','verify_opponent_navigation_reconstruction.py','verify_opponent_route_reconstruction.py','verify_tinted_native_reconstruction.py']]
if a.recompiled:paths +=[a.recompiled,a.symbols,a.recompiled.with_suffix('.map')]
before={str(p.resolve()):sha(p.read_bytes())for p in paths};(O/'source-before.json').write_text(json.dumps(before,indent=2)+'\n');(O/'verifier-snapshot.py').write_bytes(Path(__file__).read_bytes());(O/'candidate-snapshot.c').write_bytes(candidate.read_bytes())
COMMON=r'''
#include "49-physics.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[6],builder_stack_count,builder_result,builder_poison,builder_cdecl;
extern unsigned __cdecl BuilderInvoke(void);
static unsigned char*arena;static unsigned target;static int original_cleanup;
static void event(unsigned code,unsigned address){unsigned h[4];h[0]=code;h[1]=address;h[2]=0;h[3]=0;fwrite(h,16,1,stdout);fwrite(arena,4096,1,stdout);fflush(stdout);}
static LONG WINAPI observe_fault(PEXCEPTION_POINTERS p){if(p->ExceptionRecord->ExceptionCode==EXCEPTION_ACCESS_VIOLATION){event(9,(unsigned)p->ExceptionRecord->ExceptionInformation[1]);ExitProcess(0);}return EXCEPTION_CONTINUE_SEARCH;}
typedef LONG(WINAPI* Observer)(PEXCEPTION_POINTERS);
typedef PVOID(WINAPI* Handler)(ULONG,Observer);
static int run(char*path){FILE*f;unsigned h[4];FARPROC p;Handler handler;f=fopen(path,"rb");if(!f)return 6;
arena=VirtualAlloc((void*)0x20000000,4096,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(arena!=(void*)0x20000000)return 7;
if(fread(h,16,1,f)!=1||fread(arena,4096,1,f)!=1)return 8;fclose(f);_setmode(_fileno(stdout),_O_BINARY);
p=GetProcAddress(GetModuleHandleA("kernel32.dll"),"AddVectoredExceptionHandler");memcpy(&handler,&p,4);if(!handler||!handler(1,observe_fault))return 9;
builder_address=target;builder_mode=1;builder_x=builder_y=0xcccccccc;builder_poison=0xcccccccc;builder_cdecl=!original_cleanup;builder_stack_count=3;
builder_stack[0]=h[1]?0x20000000+h[1]:0;builder_stack[1]=h[2]?0x20000000+h[2]:0;builder_stack[2]=h[3]?0x20000000+h[3]:0;
_controlfp(_PC_53,_MCW_PC);_clearfp();if(BuilderInvoke())return 10;event(0,0);return 0;}
'''
PHYSICAL=r'''
int __cdecl main(int argc,char**argv){FILE*f;unsigned h[3];unsigned char*code;if(argc!=3)return 1;f=fopen(argv[2],"rb");if(!f)return 2;if(fread(h,12,1,f)!=1||h[1]>=h[0])return 3;code=VirtualAlloc(NULL,h[0],MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!code||fread(code,h[0],1,f)!=1)return 4;fclose(f);FlushInstructionCache(GetCurrentProcess(),code,h[0]);target=(unsigned)code+h[1];original_cleanup=(int)h[2];return run(argv[1]);}
'''
helpers=''
if a.helpers or not a.candidate:helpers='\n'.join(extract(matrix.read_text(),name)for name in['Matrix34ApplyProductLoaded','Matrix34ApplyProductMemory','Matrix34ApplyAdd'])
native=COMMON+'\n'+helpers+'\n'+(candidate.read_text()if a.candidate else extract(candidate.read_text(),'BrMatrix34TApplyFV'))+'\nint __cdecl main(int argc,char**argv){if(argc!=2)return 1;target=(unsigned)BrMatrix34TApplyFV;original_cleanup=0;return run(argv[1]);}\n'
caller=registry.literals()[0];caller=caller.replace('builder_poison;','builder_poison,builder_cdecl;').replace(' mov builder_result,eax\n mov ecx,esp',' mov builder_result,eax\n cmp builder_cdecl,0\n je fv_stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n fv_stackready:\n mov ecx,esp')
registry.COMMON=COMMON;commands=registry.compile_sources(O,native,PHYSICAL,caller)
binary=original.read_bytes();assert sha(binary)==HASH;code=raw(binary,0x5333a0,109)
cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;ins=list(cs.disasm(code,0x5333a0));assert sum(i.size for i in ins)==109 and ins[-1].mnemonic=='ret'and ins[-1].operands[0].imm==12
assert all(i.mnemonic!='call'and not i.mnemonic.startswith('j')and not any(op.type==CS_OP_MEM and op.mem.disp>=0x400000 and not op.mem.base for op in i.operands)for i in ins)
(O/'original-code.bin').write_bytes(words([len(code),0,1])+code)

def matching_image(exe,symbols):
 disk=exe.read_bytes();mapping=json.loads(symbols.read_text());assert mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(disk)
 mapped={}
 for row in mapping['symbols']:
  if row['orig']in mapped:assert mapped[row['orig']]==row['recomp']
  mapped[row['orig']]=row['recomp']
 root=mapped[0x5333a0];mapbytes=exe.with_suffix('.map').read_bytes();funcs={}
 for line in mapbytes.decode('latin1').splitlines():
  v=line.split()
  if len(v)>=5 and v[0].startswith('0001:')and v[3]=='f':
   address=int(v[2],16);funcs.setdefault(address,set()).add((v[1],v[-1]))
 def identity(address):
  assert address in funcs
  normalized={(name.split('@')[0].lstrip('_'),owner)for name,owner in funcs[address]}
  assert len(normalized)==1,('Ambiguous reachable function aliases',hex(address),sorted(funcs[address]))
  return next(iter(normalized))
 assert root in funcs
 root_name,root_owner=identity(root)
 assert root_name=='BrMatrix34TApplyFV'and root_owner in('libbrender:matrix34.c.obj','brender:matrix34.c.obj'),('Wrong genuine FV root',hex(root),root_name,root_owner)
 starts=sorted(funcs);nt,base,size,tab=pe(disk);todo=[root];seen={};edges=[];active=set()
 allowed={'BrMatrix34TApplyFV','Matrix34ApplyProductLoaded','Matrix34ApplyProductMemory','Matrix34ApplyAdd'}
 while todo:
  start=todo.pop()
  if start in seen:continue
  assert start in funcs;name,owner=identity(start);assert name in allowed and owner in('libbrender:matrix34.c.obj','brender:matrix34.c.obj'),(hex(start),name,owner)
  end=next(x for x in starts if x>start);pending=[start];decoded={}
  while pending:
   at=pending.pop()
   if at in decoded:continue
   assert start<=at<end
   i=next(cs.disasm(raw(disk,at,15),at,count=1));assert i.address+i.size<=end
   assert not any(i.address+k in active for k in range(i.size));active.update(range(i.address,i.address+i.size));decoded[at]=i
   for op in i.operands:
    if op.type==CS_OP_MEM and not op.mem.base:assert not(base<=op.mem.disp<base+size),('Static/global operand',hex(at),i.op_str)
    if op.type==CS_OP_IMM and i.mnemonic in('mov','push'):assert not(base<=op.imm<base+size),('Unclassified literal address',hex(at),i.op_str)
   if CS_GRP_CALL in i.groups:
    assert i.mnemonic=='call'and len(i.operands)==1 and i.operands[0].type==CS_OP_IMM and i.imm_size==4
    target=i.operands[0].imm;assert target in funcs;todo.append(target);edges.append([at,target]);pending.append(at+i.size)
   elif CS_GRP_JUMP in i.groups:
    assert i.operands[0].type==CS_OP_IMM;target=i.operands[0].imm
    if start<=target<end:pending.append(target)
    else:assert i.mnemonic=='jmp'and target in funcs;todo.append(target);edges.append([at,target])
    if i.mnemonic!='jmp':pending.append(at+i.size)
   elif not i.mnemonic.startswith('ret'):pending.append(at+i.size)
   else:assert not i.operands
  seen[start]=dict(name=name,owner=owner,address=start,end=end,instructions=[dict(address=i.address,hex=i.bytes.hex(),mnemonic=i.mnemonic,operands=i.op_str)for i in sorted(decoded.values(),key=lambda x:x.address)])
 low=min(active);high=max(active)+1;image=bytearray(b'\xcc'*(high-low))
 for row in seen.values():
  for i in row['instructions']:address=i['address'];blob=bytes.fromhex(i['hex']);image[address-low:address-low+len(blob)]=blob
 # All transported active instructions have no absolute image operand; relative
 # calls/branches preserve exact displacement under a common image translation.
 reloc_rva,reloc_size=struct.unpack_from('<II',disk,nt+24+96+5*8)
 characteristics=struct.unpack_from('<H',disk,nt+22)[0]
 if reloc_size:
  cursor=0
  while cursor<reloc_size:
   page,n=struct.unpack('<II',raw(disk,base+reloc_rva+cursor,8));assert n>=8 and n%2==0
   for entry in struct.unpack('<'+'H'*((n-8)//2),raw(disk,base+reloc_rva+cursor+8,n-8)):
    kind=entry>>12;site=base+page+(entry&4095)
    if kind:assert kind==3 and not any(site+k in active for k in range(4)),('Unexpected active HIGHLOW',hex(site))
   cursor+=n
 else:assert(reloc_rva,reloc_size)==(0,0)and characteristics&1
 return words([len(image),root-low,0])+image,dict(recompiled_sha256=sha(disk),symbols_sha256=sha(symbols.read_bytes()),map_sha256=sha(mapbytes),root=root,original_root='0x5333a0',graph=list(seen.values()),edges=edges,low=low,high=high,absolute_patches=[],map_aliases=[dict(address=address,aliases=sorted(aliases),reachable=address in seen)for address,aliases in sorted(funcs.items())if len(aliases)>1],coff_characteristics=characteristics,relocation_directory=[reloc_rva,reloc_size],contract='Only authenticated complete same-TU FV/three existing elementary helpers; no static/global operands or absolute relocations in active graph. Original RET12/native CDECL RET0 distinct real caller cleanup; no ABI-equivalence fiction.')
matching=None
if a.recompiled:
 packet,matching=matching_image(a.recompiled,a.symbols);(O/'matching-code.bin').write_bytes(packet)

cases=[]
def seed_case(mode,label,values,patches=(),dest=0x100,vector=0x200,matrix=0x300):
    b=bytearray((k*43+17)&255 for k in range(4096));struct.pack_into('<16f',b,0x200,*values[:4],*[0.]*12);struct.pack_into('<12f',b,0x300,*values[4:])
    for at,value in patches:struct.pack_into('<I',b,at,value)
    cases.append((struct.pack('<4I',mode,dest,vector,matrix)+b,dict(mode=mode,label=label,output=dest,vector=vector,matrix=matrix)))
rng=random.Random(533590)
baseline=[.7,-1.3,2.9,-.4,1.,2.,3.,4.,5.,6.,7.,8.,9.,10.,11.,12.]
specials=[0,0x80000000,1,0x80000001,0x007fffff,0x00800000,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc12345,0xffc54321,0x7f800001,0xff800002]
for mode in (0,):
    for k in range(140):
        vals=[rng.uniform(-2,2)*2.**rng.randint(-110,110)for _ in range(16)]
        seed_case(mode,'finite-%d'%k,vals)
    for at in [0x200+4*k for k in range(4)]+[0x300+4*k for k in range(12)]:
        for value in specials:seed_case(mode,'raw-%x-%x'%(at,value),baseline,[(at,value)])
    for pair in [(a,b)for a in specials[-4:]for b in specials[-4:]]:
        for a,b in ((0x200,0x300),(0x204,0x304),(0x208,0x308),(0x204,0x308),(0x200,0x204),(0x300,0x304),(0x308,0x32c),(0x20c,0x32c)):
            seed_case(mode,'pair-%x-%x-%x-%x'%(a,b,*pair),baseline,[(a,pair[0]),(b,pair[1])])
    for dest in(0x1ff,0x200,0x201,0x204,0x208,0x20c,0x2ff,0x300,0x301,0x304,0x308,0x310,0x320,0x324,0x32c):
        seed_case(mode,'overlap-%x'%dest,baseline,dest=dest)
        seed_case(mode,'overlap-raw-%x'%dest,baseline,[(0x200,0x7f800001),(0x304,0xffc54321)],dest=dest)
    for shift in(1,2,3):seed_case(mode,'unaligned-input-%d'%shift,baseline,vector=0x200+shift,matrix=0x300+shift,dest=0x100+shift)

assert len(cases)==525
cases=[(meta,payload)for payload,meta in cases]

def add(label,dest=0x100,vector=0x200,matrix_at=0x300,patches=()):
 arena=bytearray(random.Random(0x5333a000+len(cases)).randbytes(4096));struct.pack_into('<3f',arena,0x200,.7,-1.3,2.9);struct.pack_into('<12f',arena,0x300,*range(1,13))
 for at,bits in patches:struct.pack_into('<I',arena,at,bits)
 cases.append((dict(label=label,output=dest,vector=vector,matrix=matrix_at,patches=patches),words([0,dest&0xffffffff,vector&0xffffffff,matrix_at&0xffffffff])+arena))
for row in range(3):
 for lane in range(3):
  for pair in[(0x7f800001,0xff800002),(0xff800002,0x7f800001),(0x7fc12345,0xff800002),(0x7f800001,0xffc54321),(0x7fc12345,0xffc12345),(0xffc12345,0x7fc12345)]:add('direct-product-pair',patches=[(0x200+lane*4,pair[0]),(0x300+row*12+lane*4,pair[1])])
for pointer in[0,0xfffffff8,0xfffffffc,0xffc,0xff8,0xff4,0xff0,0xfec,0xfe8,0xfe4,0xfe0]:
 add('output-fault',dest=pointer);add('input-fault',vector=pointer);add('matrix-fault',matrix_at=pointer)
for matrix_at,vector in[(0xffc,0xffc),(0xff8,0xffc),(0xffc,0xff8),(0xff4,0xff8),(0xff8,0xff4)]:add('competing-input-fault',matrix_at=matrix_at,vector=vector)
for dest in[0x1fc,0x200,0x204,0x208,0x2fc,0x300,0x304,0x308,0x30c,0x310,0x314,0x318,0x31c,0x320]:
 add('live-raw-alias',dest=dest,patches=[(0x200,0x7f800001),(0x304,0xff800002),(0x314,0x7fc12345)])

assert len(cases)==631
for row in range(3):
 for sign in[False,True]:
  patches=[(0x200+4*k,0x3f800000)for k in range(3)]
  patches +=[(0x300+row*12,0x3f800000),(0x304+row*12,0xdd800000 if sign else 0x5d800000),(0x308+row*12,0x5d800000 if sign else 0xdd800000)]
  add('sum-cancellation-YZ-before-X',patches=patches)
 for lanes in[(1,2),(0,1)]:
  for pair in[(0x7fc12345,0xffc12345),(0xffc12345,0x7fc12345),(0x7f800001,0xff800002),(0xff800002,0x7f800001),(0x7fc12345,0xff800002),(0x7f800001,0xffc54321)]:
   patches=[(0x200+4*k,0x3f800000)for k in range(3)]+[(0x300+row*12+4*lanes[0],pair[0]),(0x300+row*12+4*lanes[1],pair[1])]
   add('first-final-sum-paired-NaNs',patches=patches)
for vector in[0xff8,0xffc]:add('late-C00-versus-earlier-input-fault',matrix_at=0xfffffffc,vector=vector)
for matrix_at,vector in[(0xff8,0),(0,0xff8),(0xffc,0)]:add('distinct-first-invalid-source',matrix_at=matrix_at,vector=vector)
assert len(cases)==678
(O/'fixtures.bin').write_bytes(b''.join(payload for _,payload in cases));rows=[];failures=[];classify=[]
for index,(meta,payload)in enumerate(cases):
 f=O/'case.bin';f.write_bytes(payload);r=subprocess.run([str(O/'physical.exe'),str(f),str(O/'original-code.bin')],capture_output=True,timeout=30);assert r.returncode==0 and len(r.stdout)==4112,(index,r.returncode,r.stderr,len(r.stdout));expected=r.stdout;classify.append(struct.unpack_from('<4I',expected))
 for compiler in['msvc5','msvc','mingw']+(['matching']if matching else[]):
  command=[str(O/('physical.exe'if compiler=='matching'else compiler+'.exe')),str(f)]+([str(O/'matching-code.bin')]if compiler=='matching'else[])
  r=subprocess.run(command,capture_output=True,timeout=30);same=r.returncode==0 and r.stdout==expected;row=dict(index=index,compiler=compiler,passed=same,input_sha256=sha(payload),original_output_sha256=sha(expected),actual_output_sha256=sha(r.stdout),normal_return_abi_checked=classify[-1][0]==0,exit=r.returncode);rows.append(row)
  if not same:
   stem=O/f'failure-{compiler}-{index}';stem.with_suffix('.original.bin').write_bytes(expected);stem.with_suffix('.actual.bin').write_bytes(r.stdout);stem.with_suffix('.json').write_text(json.dumps(dict(case=meta,stderr=r.stderr.hex(),row=row),indent=2)+'\n');failures.append(row)
 if index%100==0:print('checked',index,len(rows),'failures',len(failures),flush=True)
after={path:sha(Path(path).read_bytes())for path in before};assert after==before
report=dict(passed=not failures,scenes=len(cases),comparisons=len(rows),normal=sum(r[0]==0 for r in classify),actual_av=sum(r[0]==9 for r in classify),classification=classify,results=rows,failures=failures,source_stable=True,source_before=before,source_after=after,original=dict(address='0x5333a0',bytes=109,sha256=sha(code),instructions=[dict(address=i.address,bytes=i.bytes.hex(),instruction=i.mnemonic+' '+i.op_str)for i in ins]),commands=commands,artifacts={str(f.resolve()):sha(f.read_bytes())for f in O.iterdir()if f.is_file()},limits=__doc__,live_handles=[])
if matching:report.update(matching=matching,recompiled_sha256=matching['recompiled_sha256'],symbols_sha256=matching['symbols_sha256'])
(O/'verification.json').write_text(json.dumps(report,indent=2)+'\n',newline='\n');print('PASS'if not failures else'FAIL',len(cases),len(rows),len(failures),flush=True);raise SystemExit(bool(failures))
