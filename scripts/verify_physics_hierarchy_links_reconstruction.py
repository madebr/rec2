"""Complete hierarchy attachment/list attachment/removal physical comparison.

Full original public pointer state and authentic terminal fault address; normal
ESP/nonvolatiles/x87TOP checked, void EAX excluded. Finite fixture graphs only;
original nonterminating cycles, concurrency and non-x86 targets are not claimed.
"""
import argparse,hashlib,json,random,re,struct,subprocess,sys
from pathlib import Path
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'src/carma2').is_dir());sys.path.insert(0,str(ROOT/'scripts'))
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM,CS_GRP_JUMP,CS_GRP_CALL
from verify_opponent_route_reconstruction import raw,HASH,sha,words,pe
from verify_tinted_native_reconstruction import extract
import verify_phil_registry_reconstruction as registry
BASE=0x22000000;ARENA=65536
ENTRIES=[('PhysicsAddObject',0x4c63b0,25),('AddCollisionInfoChild',0x4c63d0,57),('FUN_004c6410',0x4c6410,92)]
COMMON='\n#include <windows.h>\n#include <stdio.h>\n#include <stdlib.h>\n#include <string.h>\n#include <stdint.h>\n#include <float.h>\n#include <io.h>\n#include <fcntl.h>\n#include "49-physics.h"\n#include "c2_math.h"\n#include "brender/br_defs.h"\n#define A(x,y) A->m[x][y]\n#define B(x,y) B->m[x][y]\n#define C(x,y) C->m[x][y]\nstatic unsigned char*arena;static unsigned addresses[3];\nextern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison,builder_cdecl;\nextern unsigned __cdecl BuilderInvoke(void);\ntypedef char hierarchy_link_layout[sizeof(void*)==4&&sizeof(tPhysics_object)==1240&&offsetof(tPhysics_object,parent)==0x228&&offsetof(tPhysics_object,disable_move_rotate)==0xec&&offsetof(tPhysics_object,child)==0x224&&offsetof(tPhysics_object,next)==0x220&&offsetof(br_actor,t)==0x28&&offsetof(br_transform,t)==4&&sizeof(br_matrix34)==48?1:-1];\nstatic void snapshot(unsigned event,unsigned value,unsigned address){unsigned h[3];h[0]=event;h[1]=value;h[2]=address;fwrite(h,12,1,stdout);fwrite(arena,65536,1,stdout);fflush(stdout);}\nstatic LONG WINAPI observe_fault(EXCEPTION_POINTERS*e){if(e->ExceptionRecord->ExceptionCode!=EXCEPTION_ACCESS_VIOLATION)return EXCEPTION_CONTINUE_SEARCH;snapshot(3,e->ExceptionRecord->ExceptionCode,(unsigned)e->ExceptionRecord->ExceptionInformation[1]);ExitProcess(0);return EXCEPTION_CONTINUE_SEARCH;}\nstatic int run(const char*path){FILE*f;unsigned h[4];typedef void*(WINAPI *add_handler)(ULONG,LONG(WINAPI*)(EXCEPTION_POINTERS*));FARPROC proc;add_handler handler;\nSetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);f=fopen(path,"rb");if(!f)return 10;arena=VirtualAlloc((void*)0x22000000,65536,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(arena!=(void*)0x22000000)return 11;\nif(fread(h,sizeof(h),1,f)!=1||fread(arena,65536,1,f)!=1)return 12;fclose(f);_setmode(_fileno(stdout),_O_BINARY);\nproc=GetProcAddress(GetModuleHandleA("kernel32.dll"),"AddVectoredExceptionHandler");memcpy(&handler,&proc,4);if(!handler||!handler(1,observe_fault))return 13;if(h[0]>2)return 15;\nbuilder_address=addresses[h[0]];builder_mode=0;builder_x=h[1]?0x22000000+h[1]:0;builder_y=h[2]?0x22000000+h[2]:0;builder_poison=h[3];\n#if defined(REC2_MATCHING)\nbuilder_cdecl=0;builder_stack_count=0;\n#else\nbuilder_cdecl=1;builder_stack_count=h[0]==2?1:2;builder_stack[0]=builder_x;builder_stack[1]=builder_y;\n#endif\n_controlfp(_PC_53,_MCW_PC);_clearfp();if(BuilderInvoke())return 14;snapshot(0,0,0);return 0;}\n'
PHYSICAL='\nint C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned h[6],i,offset,value;unsigned char*image;\nif(argc!=3)return 1;f=fopen(argv[2],"rb");if(!f)return 2;if(fread(h,sizeof(h),1,f)!=1||h[1]>=h[0]||h[2]>=h[0]||h[3]>=h[0])return 3;\nimage=VirtualAlloc(NULL,h[0],MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image||fread(image,h[0],1,f)!=1)return 4;\nfor(i=0;i<h[4];i++){if(fread(&offset,4,1,f)!=1||offset>h[0]-4)return 5;memcpy(&value,image+offset,4);value=(unsigned)image+value-h[5];memcpy(image+offset,&value,4);}fclose(f);\nFlushInstructionCache(GetCurrentProcess(),image,h[0]);addresses[0]=(unsigned)image+h[1];addresses[1]=(unsigned)image+h[2];addresses[2]=(unsigned)image+h[3];return run(argv[1]);}\n'

def original_image(binary):
 cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;bodies=[]
 low=min(a for _,a,_ in ENTRIES);high=max(a+n for _,a,n in ENTRIES);image=bytearray(b'\xcc'*(high-low))
 for name,a,n in ENTRIES:
  code=raw(binary,a,n);ins=list(cs.disasm(code,a));starts={i.address for i in ins};assert sum(i.size for i in ins)==n
  for i in ins:
   assert not i.group(CS_GRP_CALL)
   if i.group(CS_GRP_JUMP):assert i.operands[0].type==CS_OP_IMM and i.operands[0].imm in starts
   if i.mnemonic=='ret':assert not i.operands
   for op in i.operands:assert op.type!=CS_OP_MEM or op.mem.base or op.mem.index
  image[a-low:a-low+n]=code
  bodies.append(dict(name=name,address=a,bytes=n,sha256=sha(code),instructions=[[i.address,i.bytes.hex()]for i in ins]))
 return words([len(image),*[a-low for _,a,_ in ENTRIES],0,low])+image,dict(bodies=bodies,total_original_bytes=174,absolute_relocations=[],low=low,high=high)

def production_capsule(source):
 if source.resolve()!=(ROOT/'src/carma2/newcommon/49-physics.c').resolve():return source.read_text()
 return '\n'.join(extract(source.read_text(),name)for name,a,n in ENTRIES)+'\n'

def matching_image(exe,symbols_path):
 data=exe.read_bytes();mapping=json.loads(symbols_path.read_text());assert mapping['format']==1 and mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(data)
 mapped={r['orig']:r['recomp']for r in mapping['symbols']};roots=[mapped[a]for _,a,n in ENTRIES];mapbytes=exe.with_suffix('.map').read_bytes();functions={}
 for name,a,owner in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',mapbytes.decode('latin1'),re.M):functions.setdefault(int(a,16),[]).append((name,owner.strip()))
 cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;instructions={};graph=[]
 for (name,orig,n),start in zip(ENTRIES,roots):
  assert any(owner=='49-physics.c.obj' for alias,owner in functions[start]);end=min(a for a in functions if a>start);pending=[start];local={}
  while pending:
   at=pending.pop()
   while at not in local:
    assert start<=at<end;i=next(cs.disasm(raw(data,at,15),at));assert at+i.size<=end
    local[at]=bytes(i.bytes);instructions[at]=bytes(i.bytes);assert not i.group(CS_GRP_CALL)
    if i.group(CS_GRP_JUMP):assert i.operands[0].type==CS_OP_IMM and start<=i.operands[0].imm<end;pending.append(i.operands[0].imm)
    for op in i.operands:assert op.type!=CS_OP_MEM or op.mem.base or op.mem.index
    if i.mnemonic=='ret':assert not i.operands;break
    assert i.mnemonic not in ('int','int3')
    if i.mnemonic=='jmp':break
    at+=i.size
  starts=set(local)
  for at,bits in local.items():
   i=next(cs.disasm(bits,at))
   if i.group(CS_GRP_JUMP):assert i.operands[0].imm in starts
   if i.mnemonic not in ('ret','jmp'):assert at+i.size in starts
  graph.append(dict(name=name,address=start,end=end,map_aliases=functions[start],instructions=[[a,b.hex()]for a,b in sorted(local.items())],instruction_count=len(local),instruction_sha256=sha(b''.join(b for a,b in sorted(local.items())))))
 active={a+k for a,b in instructions.items()for k in range(len(b))};assert len(active)==sum(map(len,instructions.values()))
 nt,base,size,tab=pe(data);rr,rz=struct.unpack_from('<II',data,nt+24+96+5*8);cursor=base+rr
 while cursor<base+rr+rz:
  page,n=struct.unpack('<II',raw(data,cursor,8));assert n>=8
  for value in struct.unpack('<'+'H'*((n-8)//2),raw(data,cursor+8,n-8)):
   if value>>12==3:assert base+page+(value&4095)not in active
  cursor+=n
 low=min(instructions);high=max(a+len(b)for a,b in instructions.items());image=bytearray(b'\xcc'*(high-low))
 for a,b in instructions.items():image[a-low:a-low+len(b)]=b
 return words([len(image),*[a-low for a in roots],0,low])+image,dict(recompiled_sha256=sha(data),symbols_sha256=sha(symbols_path.read_bytes()),map_sha256=sha(mapbytes),roots=roots,graph=graph,low=low,high=high,edges=[],absolute_relocations=[],active_relocations_checked=[],mutable_globals=[],normalization=[])

def fixtures():
    cases=[]
    def terminates(mode,x,y,arena):
        # Fixture selection only: expected results always come from real original code.
        memory=bytearray(arena);steps=0
        def load(p,off):
            at=((p+off)&0xffffffff)-BASE
            if not 0<=at<=ARENA-4:raise IndexError
            return struct.unpack_from('<I',memory,at)[0]
        def save(p,off,value):
            at=((p+off)&0xffffffff)-BASE
            if not 0<=at<=ARENA-4:raise IndexError
            struct.pack_into('<I',memory,at,value)
        try:
            if mode==0:
                previous=load(x,0x224);save(y,0x220,previous);save(x,0x224,y);save(y,0x228,x)
            elif mode==1:
                save(y,0x228,x);last=y;current=load(y,0x220)
                while current:
                    steps+=1
                    if steps>128:return False
                    last=current;save(last,0x228,x);current=load(last,0x220)
                previous=load(x,0x224);save(last,0x220,previous);save(x,0x224,y)
            else:
                parent=load(x,0x228);previous=load(parent,0x224)
                if previous==x:save(parent,0x224,load(x,0x220))
                else:
                    current=load(previous,0x220)
                    while current!=x:
                        steps+=1
                        if steps>128:return False
                        previous=current;current=load(previous,0x220)
                    save(previous,0x220,load(x,0x220))
                save(x,0x228,0);save(x,0x220,0)
        except IndexError:pass
        return True
    def add(mode=0,seed=1,count=1,selected=0,parent=0x1000,child=0x2000,patches=(),label='ordinary'):
        rng=random.Random(seed);arena=bytearray(rng.randbytes(ARENA));nodes=[child+i*0x500 for i in range(count)]
        def put(at,value):
            if 0<=at<=ARENA-4:struct.pack_into('<I',arena,at,value&0xffffffff)
        for i,node in enumerate(nodes):
            put(node+0x220,BASE+nodes[i+1]if i+1<len(nodes)else 0);put(node+0x224,0);put(node+0x228,BASE+parent if parent else 0)
        put(parent+0x224,BASE+child if mode==2 else 0)
        for at,value in patches:put(at,value)
        x=nodes[selected]if mode==2 else parent;y=child
        xp=BASE+x if x else 0;yp=BASE+y if y else 0
        if not terminates(mode,xp,yp,arena):return
        case=dict(mode=mode,seed=seed,count=count,selected=selected,parent=parent,child=child,patches=patches,label=label)
        cases.append((case,words([mode,x,y,rng.getrandbits(32)])+arena))
    for mode in range(3):
        for count in range(1,17):
            for seed in range(1,5):
                if mode==2:
                    for selected in sorted({0,count//2,count-1}):add(mode,seed,count,selected,label='head-middle-tail-detach')
                else:add(mode,seed,count,label='complete-list')
    for mode in (0,1):
        for delta in range(-20,21):
            for seed in (10,20,30):add(mode=mode,seed=seed,parent=0x2000+delta,child=0x2000,label='partial-parent-child-alias')
        for value in (0,1,0xffffffff,0x80000000,BASE+0x1000,BASE+0x2000,BASE+0xf000):
            add(mode=mode,patches=[(0x1224,value)],label='raw-old-child-pointer')
        for count in (2,3,8):
            for target in range(count):add(mode=mode,count=count,parent=0x2000+target*0x500,label='parent-is-list-member')
    for mode in range(3):
        for parent in (0,1,0xffd8,0xfdd8,0xfddc,0xfde0,0x10000):add(mode=mode,parent=parent,label='parent-read-write-page-fault')
        for child in (0,1,0xfdd4,0xfdd8,0xfddc,0xfde0,0xfffc,0x10000):add(mode=mode,child=child,label='child-read-write-page-fault')
    for count in (1,2,3,8):
        for selected in range(count):
            for delta in range(-12,13):add(mode=2,count=count,selected=selected,parent=0x2000+selected*0x500+delta,label='detach-partial-parent-alias')
    for mode in (1,2):
        for count in (1,2,5):
            for invalid in (0,1,0xffffffff,BASE+ARENA,BASE+ARENA-0x220,BASE+ARENA-0x224):
                at=0x2000+(count-1)*0x500+0x220
                add(mode=mode,count=count,selected=count-1,patches=[(at,invalid)],label='terminal-link-and-null-fault')
    for selected in range(5):
        for parent_head in (0,BASE+0x8000,BASE+0xfffc):add(mode=2,count=5,selected=selected,patches=[(0x1224,parent_head)],label='unlink-absent-object')
    return cases

def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',required=True,type=Path);p.add_argument('--candidate-source',type=Path,default=ROOT/'src/carma2/newcommon/49-physics.c');p.add_argument('--limit',type=int);p.add_argument('--case-index',type=int,action='append');p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);a=p.parse_args();assert bool(a.recompiled)==bool(a.symbols)
 out=a.output.resolve();assert not out.exists(),'Use a fresh uniquely owned output';out.mkdir(parents=True)
 source=a.candidate_source.resolve();physics=ROOT/'src/carma2/newcommon/49-physics.c';matrix=ROOT/'src/brender/core/math/matrix34.c';script=Path(__file__).resolve()
 paths=[source,physics,matrix,script,ROOT/'scripts/verify_phil_registry_reconstruction.py',ROOT/'scripts/verify_opponent_route_reconstruction.py',ROOT/'scripts/verify_opponent_navigation_reconstruction.py',ROOT/'scripts/verify_tinted_native_reconstruction.py']+list((ROOT/'src').rglob('*.h'))+list((ROOT/'src').rglob('*.c'))
 if a.recompiled:paths.extend([a.recompiled,a.symbols,a.recompiled.with_suffix('.map')])
 before={str(f):sha(f.read_bytes())for f in paths};(out/'source-before.json').write_text(json.dumps(before,indent=2)+'\n');(out/'candidate-snapshot.c').write_bytes(source.read_bytes());(out/'verifier-snapshot.py').write_bytes(script.read_bytes())
 binary=(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes();assert sha(binary)==HASH;image,graph=original_image(binary);(out/'original-code.bin').write_bytes(image)
 linked=None
 if a.recompiled:
  linked_blob,linked=matching_image(a.recompiled,a.symbols);(out/'matching-code.bin').write_bytes(linked_blob)
 native=COMMON+'\n'+production_capsule(source)+'\nint C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;addresses[0]=(unsigned)PhysicsAddObject;addresses[1]=(unsigned)AddCollisionInfoChild;addresses[2]=(unsigned)FUN_004c6410;return run(argv[1]);}\n'
 caller,_,_=registry.literals();caller=caller.replace('builder_poison;','builder_poison,builder_cdecl;');old=' mov builder_result,eax\n mov ecx,esp';assert old in caller;caller=caller.replace(old,' xor eax,eax\n mov builder_result,eax\n cmp builder_cdecl,0\n je hierarchy_link_stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n hierarchy_link_stackready:\n mov ecx,esp')
 registry.COMMON=COMMON;commands=registry.compile_sources(out,native,PHYSICAL,caller)
 cases=fixtures();indices=a.case_index or list(range(len(cases)));indices=indices[:a.limit]if a.limit else indices;results=[];failures=[];classification={}
 for index in indices:
  case,payload=cases[index];f=out/'case.bin';f.write_bytes(payload);r=subprocess.run([str(out/'physical.exe'),str(f),str(out/'original-code.bin')],capture_output=True,timeout=30)
  assert r.returncode==0 and len(r.stdout)==65548,('original',index,r.returncode,len(r.stdout),r.stderr)
  expected=r.stdout;event,value,address=struct.unpack_from('<3I',expected);classification[index]=dict(event=event,value=value,address=address)
  for compiler in('msvc5','msvc','mingw',*(['matching']if linked else [])):
   command=[str(out/('physical.exe'if compiler=='matching'else compiler+'.exe')),str(f)]
   if compiler=='matching':command.append(str(out/'matching-code.bin'))
   r=subprocess.run(command,capture_output=True,timeout=30);passed=r.returncode==0 and r.stdout==expected
   results.append(dict(index=index,compiler=compiler,passed=passed,input_sha256=sha(payload),original_output_sha256=sha(expected),actual_output_sha256=sha(r.stdout),original_output_bytes=len(expected),actual_output_bytes=len(r.stdout),event=event,normal_return_abi_checked=event==0,exit=r.returncode))
   if not passed:
    failure=out/('failure-%s-%d.json'%(compiler,index));failure.write_text(json.dumps(dict(case=case,expected=expected.hex(),actual=r.stdout.hex(),stderr=r.stderr.hex()),indent=2)+'\n');failures.append(dict(index=index,compiler=compiler,case=case,path=str(failure)))
  if index%100==0:print('checked',index,len(results),'failures',len(failures),flush=True)
 after={f:sha(Path(f).read_bytes())for f in before};assert before==after,'source guard changed'
 report=dict(passed=not failures,scenes=len(indices),comparisons=len(results),total_scenes=len(cases),complete_suite=indices==list(range(len(cases))),selected_indices=indices,source_stable=True,source_before=before,source_after=after,original_sha256=HASH,graph=graph,fixtures_sha256=sha(b''.join(cases[i][1]for i in indices)),fixture_sha256=[sha(cases[i][1])for i in indices],classification=classification,results=results,failures=failures,commands=commands,generated_sha256={str(f):sha(f.read_bytes())for f in out.iterdir()if f.is_file()},live_handles=[],limits=__doc__)
 if linked:report.update(matching=linked,recompiled_sha256=linked['recompiled_sha256'],symbols_sha256=linked['symbols_sha256'])
 (out/'verification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS'if not failures else'FAIL',len(indices),len(results),len(failures),flush=True);assert not failures,'Failures retained'
if __name__=='__main__':main()
