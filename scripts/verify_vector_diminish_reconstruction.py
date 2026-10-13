"""Full original DRVector3Diminish516390/124 physical-original/native3 scratch.
Exact4096-byte arenas, PC53/masked normal ESP/nonvolatiles/x87TOP and true AV
code/address/public state; void EAX excluded. No numeric/control mocks,
normalization, private-frame/nonx86/unmasked/exhaustive or whole-game claim.
"""
import argparse,hashlib,json,random,re,struct,subprocess,sys
from pathlib import Path
sys.path.insert(0,str(Path.cwd()/'scripts'))
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM,CS_GRP_CALL,CS_GRP_JUMP,CS_GRP_RET
from verify_opponent_route_reconstruction import ROOT,HASH,raw,sha,words,pe
from verify_tinted_native_reconstruction import extract
import verify_phil_registry_reconstruction as registry
BASE=0x22000000;ARENA=4096;START=0x516390;END=0x51640c
COMMON='\n#include <windows.h>\n#include <stdio.h>\n#include <stdlib.h>\n#include <string.h>\n#include <stdint.h>\n#include <float.h>\n#include <io.h>\n#include <fcntl.h>\n#include "41-utility.h"\nstatic unsigned char*arena;\n\nextern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[6],builder_stack_count,builder_result,builder_poison,builder_cdecl;\nextern unsigned __cdecl BuilderInvoke(void);\ntypedef char vector_layout[sizeof(void*)==4&&sizeof(br_vector3)==12?1:-1];\nstatic void snapshot(unsigned event,unsigned value,unsigned address){unsigned h[3];h[0]=event;h[1]=value;h[2]=address;fwrite(h,12,1,stdout);fwrite(arena,4096,1,stdout);fflush(stdout);}\nstatic LONG WINAPI observe_fault(EXCEPTION_POINTERS*e){if(e->ExceptionRecord->ExceptionCode!=EXCEPTION_ACCESS_VIOLATION)return EXCEPTION_CONTINUE_SEARCH;snapshot(3,e->ExceptionRecord->ExceptionCode,(unsigned)e->ExceptionRecord->ExceptionInformation[1]);ExitProcess(0);return EXCEPTION_CONTINUE_SEARCH;}\nstatic int run(const char*path){FILE*f;unsigned h[8];typedef void*(WINAPI *add_handler)(ULONG,LONG(WINAPI*)(EXCEPTION_POINTERS*));FARPROC proc;add_handler handler;\nSetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);f=fopen(path,"rb");if(!f)return 10;arena=VirtualAlloc((void*)0x22000000,4096,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(arena!=(void*)0x22000000)return 11;\nif(fread(h,sizeof(h),1,f)!=1||fread(arena,4096,1,f)!=1)return 12;fclose(f);_setmode(_fileno(stdout),_O_BINARY);\nproc=GetProcAddress(GetModuleHandleA("kernel32.dll"),"AddVectoredExceptionHandler");memcpy(&handler,&proc,4);if(!handler||!handler(1,observe_fault))return 13;\nbuilder_mode=0;builder_x=h[0];builder_y=h[1];builder_poison=h[7];\n#if defined(REC2_MATCHING)\nbuilder_cdecl=0;builder_stack_count=0;\n#else\nbuilder_cdecl=1;builder_stack_count=2;builder_stack[0]=builder_x;builder_stack[1]=builder_y;\n#endif\n_controlfp(_PC_53,_MCW_PC);_clearfp();if(BuilderInvoke())return 14;snapshot(0,0,0);return 0;}\n'
PHYSICAL='\nint C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned h[3],i,*patch;unsigned char*image;unsigned value;\nif(argc!=3)return 1;f=fopen(argv[2],"rb");if(!f)return 2;if(fread(h,sizeof(h),1,f)!=1||h[1]>=h[0])return 3;\nimage=VirtualAlloc(NULL,h[0],MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image||fread(image,h[0],1,f)!=1)return 4;\npatch=NULL;if(h[2]){patch=malloc(h[2]*12);if(!patch||fread(patch,h[2]*12,1,f)!=1)return 5;}fclose(f);\nfor(i=0;i<h[2];i++){if(patch[3*i]+4>h[0])return 6;value=patch[3*i+2]?patch[3*i+1]:(unsigned)image+patch[3*i+1];memcpy(image+patch[3*i],&value,4);}free(patch);\nFlushInstructionCache(GetCurrentProcess(),image,h[0]);builder_address=(unsigned)image+h[1];return run(argv[1]);}\n'

def original_image(binary):
 cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;body=raw(binary,START,124);ins=list(cs.disasm(body,START));assert sum(i.size for i in ins)==124 and ins[-1].mnemonic=='ret'and not ins[-1].operands
 patches=[]
 for i in ins:
  assert i.mnemonic!='call'
  if i.mnemonic.startswith('j'):assert i.operands[0].type==CS_OP_IMM and START<=i.operands[0].imm<END
  for op in i.operands:
   if op.type==CS_OP_MEM and not op.mem.base and not op.mem.index:
    assert i.mnemonic=='fcomp'and op.mem.disp==0x58b280 and op.size==4 and i.disp_size==4
    patches.append((i.address-START+i.disp_offset,124,0))
 assert len(patches)==3 and raw(binary,0x58b280,4)==bytes(4)
 blob=words([128,0,3])+body+bytes(4)+b''.join(words(p)for p in patches)
 return blob,dict(address=hex(START),bytes=124,body_sha256=sha(body),constants={'0x58b280':'00000000'},instructions=[[i.address,i.bytes.hex()]for i in ins],patches=patches,calls=[])


def production_capsule(source):
 if source.resolve()!=(ROOT/'src/carma2/newcommon/41-utility.c').resolve():return source.read_text()
 text=source.read_text();marker=text.index('typedef tU32 tDiminishRawWord __attribute__((__may_alias__));');first=text.rfind('#if',0,marker);last=text.index('#endif',marker)+len('#endif')
 typedef=text[first:last];assert typedef.startswith('#if defined(__GNUC__)')and 'typedef tU32 tDiminishRawWord;'in typedef
 return typedef+'\n'+extract(text,'DiminishSubtractScalar')+'\n'+extract(text,'DRVector3Diminish')+'\n'

def linked_image(exe,symbols_path):
 b=exe.read_bytes();mapping=json.loads(symbols_path.read_text());assert mapping['format']==1 and mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(b)
 start=next(r['recomp']for r in mapping['symbols']if r['orig']==START)
 map_bytes=exe.with_suffix('.map').read_bytes();functions={int(a,16):(n,o.strip())for n,a,o in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',map_bytes.decode('latin1'),re.M)}
 assert start in functions and functions[start][0].lstrip('_@').split('@')[0]=='DRVector3Diminish'and functions[start][1]=='41-utility.c.obj'
 nt,base,size,tab=pe(b);regions=[]
 for k in range(struct.unpack_from('<H',b,nt+6)[0]):
  vs,rva,n,off=struct.unpack_from('<IIII',b,tab+40*k+8);flags=struct.unpack_from('<I',b,tab+40*k+36)[0];regions.append((base+rva,max(vs,n),flags))
 cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;todo=[start];seen=set();instructions={};graph=[];edges=[];constants={};patches={}
 while todo:
  root=todo.pop()
  if root in seen:continue
  seen.add(root);name,owner=functions[root];plain=name.lstrip('_@').split('@')[0];assert plain in('DRVector3Diminish','DiminishSubtractScalar')and owner=='41-utility.c.obj',(name,owner)
  end=min(a for a in functions if a>root);pending=[root];local={}
  while pending:
   at=pending.pop()
   while at not in local:
    assert root<=at<end;(i,)=cs.disasm(raw(b,at,15),at,count=1);assert i.address+i.size<=end
    local[at]=bytes(i.bytes);instructions[at]=bytes(i.bytes)
    if i.group(CS_GRP_CALL)or i.group(CS_GRP_JUMP):
     assert i.operands[0].type==CS_OP_IMM,('Unknown indirect call/jump',name,hex(at),i.op_str);target=i.operands[0].imm
     if root<=target<end:
      assert not i.group(CS_GRP_CALL),('Unexpected local call',name,hex(at));pending.append(target)
     else:
      assert target in functions,(name,i.mnemonic,i.op_str);todo.append(target);edges.append(dict(site=at,target=target,kind=i.mnemonic))
    for op in i.operands:
     value=site=None;width=4
     if op.type==CS_OP_MEM and base<=op.mem.disp<base+size:
      value=op.mem.disp;site=at+i.disp_offset;width=op.size;assert i.disp_size==4 and not op.mem.base and not op.mem.index,('Unknown static table',name,i.op_str)
     elif op.type==CS_OP_IMM and i.mnemonic in('mov','push')and base<=op.imm<base+size:
      value=op.imm;site=at+i.imm_offset;assert i.imm_size==4
     if value is not None:
      assert struct.unpack('<I',raw(b,site,4))[0]==value
      region=next(r for r in regions if r[0]<=value<r[0]+r[1]);assert not region[2]&0xa0000020,(name,hex(value),i.op_str)
      assert width in(1,2,4,8,10);constants[(value,width)]=raw(b,value,width);patch=(site,value,0);assert site not in patches or patches[site]==patch;patches[site]=patch
    if i.group(CS_GRP_RET):assert i.mnemonic=='ret'and not i.operands;break
    if i.mnemonic in('jmp','int','int3'):break
    at+=i.size
  graph.append(dict(name=name,owner=owner,address=root,end=end,extent=end-root,instructions=[[a,v.hex()]for a,v in sorted(local.items())],instruction_count=len(local),instruction_sha256=sha(b''.join(v for _,v in sorted(local.items())))))
 active={a+k for a,v in instructions.items()for k in range(len(v))};rr,rz=struct.unpack_from('<II',b,nt+24+96+5*8);cursor=base+rr;checked=[]
 while cursor<base+rr+rz:
  page,n=struct.unpack('<II',raw(b,cursor,8));assert n>=8
  for w in struct.unpack('<'+'H'*((n-8)//2),raw(b,cursor+8,n-8)):
   if w>>12==3 and base+page+(w&4095)in active:
    site=base+page+(w&4095);assert site in patches,hex(site);checked.append(site)
  cursor+=n
 low=min([*instructions,*[a for a,n in constants]]);high=max([a+len(v)for a,v in instructions.items()]+[a+len(v)for(a,n),v in constants.items()]);image=bytearray(b'\xcc'*(high-low))
 for a,v in instructions.items():image[a-low:a-low+len(v)]=v
 for(a,n),v in constants.items():
  for k in range(n):assert a+k not in active,'constant/code overlap'
  image[a-low:a-low+n]=v
 transported=[(site-low,target-low,absolute)for site,target,absolute in sorted(patches.values())]
 blob=words([len(image),start-low,len(transported)])+image+b''.join(words(r)for r in transported)
 return blob,dict(recompiled_sha256=sha(b),symbols_sha256=sha(symbols_path.read_bytes()),map_sha256=sha(map_bytes),start=start,low=low,high=high,graph=graph,edges=edges,constants=[dict(address=a,bytes=n,hex=v.hex())for(a,n),v in sorted(constants.items())],patches=[dict(site=s,old=t,target=t,absolute=x)for s,t,x in sorted(patches.values())],transport_patches=transported,active_relocations_checked=checked,indirect_control_sites=[],mutable_globals={},normalization=[])

def fixtures():
 result=[];bits=lambda f:struct.unpack('<I',struct.pack('<f',f))[0]
 def add(left=(0x40400000,0xc0800000,0x3f800000),right=(0x3f800000,0xc0400000,0x40000000),dst=0x100,src=0x200,label='normal',patches=(),seed=None):
  seed=len(result)+0x516390 if seed is None else seed;r=random.Random(seed);arena=bytearray(r.randbytes(ARENA))
  for at,values in [(dst,left),(src,right)]:
   if 0<=at<=ARENA-4:
    for k,value in enumerate(values):
     if at+4*k<=ARENA-4:struct.pack_into('<I',arena,at+4*k,value)
  for at,value in patches:struct.pack_into('<I',arena,at,value)
  dest=BASE+dst if dst>=0 else {-1:0,-2:1,-3:0xffffffff}[dst]
  source=BASE+src if src>=0 else {-1:0,-2:1,-3:0xffffffff}[src]
  result.append((dict(label=label,left=left,right=right,dst=dst,src=src,patches=patches,seed=seed),words([dest,source,0,0,0,0,0,r.choice([0,0xcccccccc,0x7f800001])])+arena))
 add(label='finite-control')
 add(left=(0x7f812345,0xffc54321,0x7fc12345),right=(0,0,0),label='unordered-clamp-counterexample')
 add(left=(1,0x80000001,0x00800000),right=(0,0,0),label='wide-underflow-sign-product')
 add(src=ARENA-4,label='source-Y-fault-after-X-store')
 special=[0,0x80000000,1,0x80000001,0x007fffff,0x807fffff,0x00800000,0x80800000,0x3f800000,0xbf800000,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7f812345,0xff812345,0x7fc12345,0xffc12345]
 for axis in range(3):
  for left in special:
   for right in special:
    a=[bits(3),bits(-4),bits(1)];b=[bits(1),bits(-3),bits(2)];a[axis]=left;b[axis]=right
    add(left=tuple(a),right=tuple(b),label='all-ordered-unordered-product-pairs')
 for offset in range(-15,16):
  for i in range(6):add(dst=0x200,src=0x200+offset,left=tuple(special[(i+k)%len(special)]for k in range(3)),right=tuple(special[(3*i+k+7)%len(special)]for k in range(3)),label='partial-byte-overlap')
 for i in range(180):
  r=random.Random(i+51000);a=[(r.getrandbits(1)<<31)|(r.randrange(1,254)<<23)|r.getrandbits(23)for k in range(3)];b=[(r.getrandbits(1)<<31)|(r.randrange(1,254)<<23)|r.getrandbits(23)for k in range(3)]
  add(left=tuple(a),right=tuple(b),dst=0x100+i%4,src=0x200+i%4,label='finite-exponent-spread')
 for dst,src in [(-1,0x200),(0x100,-1),(-1,-2),(-2,-1),(-3,0x200),(0x100,-3),(ARENA-4,0x200),(ARENA-8,0x200),(0x100,ARENA-4),(0x100,ARENA-8),(ARENA-4,ARENA-4),(ARENA-8,ARENA-8),(ARENA-4,-2),(ARENA-8,-1)]:
  add(dst=dst,src=src,label='authentic-terminal-access-fault')
 for nan in(0x7f812345,0xff812345,0x7fc12345,0xffc12345):
  add(left=(nan,0x3f800000,0x40000000),right=(0xffc12345,0,0),src=ARENA-4,label='paired-NaN-X-visible-before-Y-fault')
 fault_nan=[0x7f812345,0xff812345,0x7fc12345,0xffc12345,0x7fc54321,0xffc54321]
 for left in fault_nan:
  for right in fault_nan:
   add(left=(left,0x3f800000,0x40000000),right=(right,0,0),src=ARENA-4,label='all-paired-NaN-X-visible-before-Y-fault')
   add(left=(0x40400000,left,0x40000000),right=(0x3f800000,right,0),src=ARENA-8,label='all-paired-NaN-Y-visible-before-Z-fault')
 return result

def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',required=True,type=Path);p.add_argument('--candidate-source',type=Path,default=ROOT/'src/carma2/newcommon/41-utility.c');p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);p.add_argument('--limit',type=int);p.add_argument('--case-index',type=int,action='append');a=p.parse_args();assert bool(a.recompiled)==bool(a.symbols);out=a.output.resolve();assert not out.exists(),'Use a fresh uniquely owned output';out.mkdir(parents=True)
 source=a.candidate_source.resolve();script=Path(__file__).resolve();paths=[source,script,ROOT/'src/carma2/newcommon/41-utility.c',ROOT/'src/brender/core/math/vector.c',ROOT/'scripts/verify_phil_registry_reconstruction.py',ROOT/'scripts/verify_opponent_route_reconstruction.py',ROOT/'scripts/verify_opponent_navigation_reconstruction.py',ROOT/'scripts/verify_tinted_native_reconstruction.py']+list((ROOT/'src').rglob('*.h'))
 if a.recompiled:paths.extend([a.recompiled,a.symbols,a.recompiled.with_suffix('.map')])
 before={str(p):sha(p.read_bytes())for p in paths};(out/'source-before.json').write_text(json.dumps(before,indent=2)+'\n');(out/'candidate-snapshot.c').write_bytes(source.read_bytes());(out/'verifier-snapshot.py').write_bytes(script.read_bytes())
 binary=(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes();assert sha(binary)==HASH;blob,graph=original_image(binary);(out/'original-code.bin').write_bytes(blob)
 linked=None
 if a.recompiled:
  packet,linked=linked_image(a.recompiled,a.symbols);(out/'matching-code.bin').write_bytes(packet)
 native=COMMON+'\n'+production_capsule(source)+'\nint C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;builder_address=(unsigned)DRVector3Diminish;return run(argv[1]);}\n'
 caller,_,_=registry.literals();caller=caller.replace('builder_stack[5]','builder_stack[6]').replace('builder_poison;','builder_poison,builder_cdecl;');old=' mov builder_result,eax\n mov ecx,esp';assert old in caller;caller=caller.replace(old,' mov builder_result,eax\n cmp builder_cdecl,0\n je diminish_stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n diminish_stackready:\n mov ecx,esp')
 registry.COMMON=COMMON;commands=registry.compile_sources(out,native,PHYSICAL,caller)
 cases=fixtures();indices=a.case_index or list(range(len(cases)));indices=indices[:a.limit]if a.limit else indices;results=[];failures=[];classification={}
 for index in indices:
  case,payload=cases[index];f=out/'case.bin';f.write_bytes(payload);proc=subprocess.run([str(out/'physical.exe'),str(f),str(out/'original-code.bin')],capture_output=True,timeout=30);assert proc.returncode==0 and len(proc.stdout)==ARENA+12,('original',index,proc.returncode,len(proc.stdout),proc.stderr)
  expected=proc.stdout;event,value,address=struct.unpack_from('<3I',expected);classification[index]=dict(event=event,value=value,address=address)
  for compiler in('msvc5','msvc','mingw',*(['matching']if linked else [])):
   command=[str(out/('physical.exe'if compiler=='matching'else compiler+'.exe')),str(f)]
   if compiler=='matching':command.append(str(out/'matching-code.bin'))
   proc=subprocess.run(command,capture_output=True,timeout=30);same=proc.returncode==0 and proc.stdout==expected;row=dict(index=index,compiler=compiler,passed=same,input_sha256=sha(payload),original_output_sha256=sha(expected),actual_output_sha256=sha(proc.stdout),original_output_bytes=len(expected),actual_output_bytes=len(proc.stdout),event=event,normal_return_abi_checked=event==0,exit=proc.returncode);results.append(row)
   if not same:
    path=out/f'failure-{index}-{compiler}.json';path.write_text(json.dumps(dict(case=case,expected=expected.hex(),actual=proc.stdout.hex(),stderr=proc.stderr.hex(),differences=[dict(offset=k,expected=expected[k:k+4].hex(),actual=proc.stdout[k:k+4].hex())for k in range(0,min(len(expected),len(proc.stdout)),4)if expected[k:k+4]!=proc.stdout[k:k+4]]),indent=2)+'\n');failures.append(dict(index=index,compiler=compiler,path=str(path)))
  if index%100==0:print('checked',index,len(results),'failures',len(failures),flush=True)
 after={p:sha(Path(p).read_bytes())for p in before};assert before==after,'Source guard changed'
 report=dict(passed=not failures,original_sha256=HASH,graph=graph,scenes=len(indices),comparisons=len(results),complete_suite=indices==list(range(len(cases))),selected_indices=indices,total_scenes=len(cases),source_stable=True,source_before=before,source_after=after,fixture_sha256=[sha(cases[i][1])for i in indices],fixtures_sha256=sha(b''.join(cases[i][1]for i in indices)),results=results,failures=failures,classification=classification,commands=commands,generated_sha256={str(p):sha(p.read_bytes())for p in out.iterdir()if p.is_file()},limitations=__doc__,live_handles=[])
 if linked:report.update(linked=linked,recompiled_sha256=linked['recompiled_sha256'],symbols_sha256=linked['symbols_sha256'])
 (out/'verification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS'if not failures else'FAIL',len(indices),len(results),len(failures),flush=True);raise SystemExit(bool(failures))
if __name__=='__main__':main()
