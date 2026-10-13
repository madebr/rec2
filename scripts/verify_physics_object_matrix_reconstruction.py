"""Compare complete object matrix installation with the original two-body graph.

Both real sequential BrMatrix34Copy calls execute. Raw full arenas, accessible
unaligned overlaps, IEEE payloads, and direct x86 register/stack/x87-TOP ABI are
checked. No mathematical or game dependency is substituted. Optional rebuilt
PE support traverses actual instruction graphs and permits only the real copy
callee. Bounded fixtures do not establish whole-game or non-x86 equivalence.
"""
import argparse,json,random,re,struct,subprocess,sys
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM
from verify_opponent_route_reconstruction import ROOT,HASH,raw,sha,words
from verify_tinted_native_reconstruction import extract
import verify_phil_registry_reconstruction as registry

SOURCE=ROOT/'src/carma2/newcommon/49-physics.c'
MATRIX=ROOT/'src/brender/core/math/matrix34.c'
ENTRIES=[('FUN_004b7220',0x4b7220,40),('BrMatrix34Copy',0x5325d0,79)]
ARENA=4096
BASE=0x22000000
COMMON=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "49-physics.h"
#define A(x,y) A->m[x][y]
#define B(x,y) B->m[x][y]
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison,builder_cdecl;
extern unsigned __cdecl BuilderInvoke(void);
typedef char layout_assert[sizeof(void*)==4&&offsetof(tPhysics_object,transform_matrix)==0x8c&&offsetof(br_actor,t)==0x28&&offsetof(br_transform,t)==4?1:-1];
static int run(const char*path){
 FILE*f;unsigned h[3],index=0;unsigned char*arena;
 SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);
 f=fopen(path,"rb");if(!f)return 10;
 arena=(unsigned char*)VirtualAlloc((void*)0x22000000,4096,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
 if(arena!=(void*)0x22000000)return 11;_setmode(_fileno(stdout),_O_BINARY);
 while(fread(h,sizeof(h),1,f)==1){
  if(fread(arena,4096,1,f)!=1)return 12;
  builder_x=0x22000000+h[0];builder_y=0x22000000+h[1];builder_poison=h[2];builder_mode=1;
#ifdef REC2_MATCHING
  builder_stack_count=0;builder_cdecl=0;
#else
  builder_stack_count=2;builder_cdecl=1;builder_stack[0]=builder_x;builder_stack[1]=builder_y;
#endif
  _controlfp(_PC_53,_MCW_PC);_clearfp();
  if(BuilderInvoke())return 14;
  fwrite(&index,4,1,stdout);fwrite(arena,4096,1,stdout);++index;
 }
 if(!feof(f))return 13;fclose(f);return 0;
}
'''
PHYSICAL=r'''
int C2_HOOK_CDECL main(int argc,char**argv){
 FILE*f;unsigned h[2];unsigned char*image;
 if(argc!=3)return 1;f=fopen(argv[2],"rb");if(!f)return 2;
 if(fread(h,sizeof(h),1,f)!=1||!h[0]||h[1]>=h[0])return 3;
 image=(unsigned char*)VirtualAlloc(NULL,h[0],MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image)return 4;
 if(fread(image,h[0],1,f)!=1)return 5;fclose(f);
 FlushInstructionCache(GetCurrentProcess(),image,h[0]);builder_address=(unsigned)(image+h[1]);return run(argv[1]);
}
'''

def image_graph(binary,roots,ends):
    cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
    functions={};edges=[];all_instructions={}
    for start in roots:
        todo=[start];seen={}
        while todo:
            address=todo.pop()
            while address not in seen:
                assert start<=address<ends[start],(hex(start),hex(address))
                i=next(cs.disasm(raw(binary,address,15),address))
                assert i.address+i.size<=ends[start]
                for op in i.operands:
                    if op.type==CS_OP_MEM:
                        assert op.mem.base or op.mem.index,('unexpected absolute data',i.op_str)
                seen[address]=bytes(i.bytes);all_instructions[address]=bytes(i.bytes)
                if i.mnemonic=='call':
                    assert i.operands[0].type==CS_OP_IMM and i.operands[0].imm==roots[1]
                    edges.append(dict(site=hex(address),target=hex(i.operands[0].imm)))
                if i.mnemonic.startswith('j'):
                    assert i.operands[0].type==CS_OP_IMM
                    target=i.operands[0].imm;assert start<=target<ends[start];todo.append(target)
                    if i.mnemonic=='jmp':break
                if i.mnemonic=='ret':
                    assert not i.operands or i.operands[0].imm==0
                    break
                address+=i.size
        functions[hex(start)]={hex(a):b.hex()for a,b in sorted(seen.items())}
    assert sum(e['target']==hex(roots[1])for e in edges)==2
    low=min(all_instructions);high=max(a+len(b)for a,b in all_instructions.items())
    image=bytearray(b'\xcc'*(high-low))
    for a,b in all_instructions.items():image[a-low:a-low+len(b)]=b
    return words([len(image),roots[0]-low])+image,dict(functions=functions,calls=edges,low=hex(low),high=hex(high))

def fixtures():
    cases=[]
    def add(source,actor_matrix,seed,object_=0x100):
        assert 0<=source<=ARENA-48 and 0x2c<=actor_matrix<=ARENA-48
        r=random.Random(seed);arena=bytearray(r.randbytes(ARENA))
        patterns=[0,0x80000000,1,0x80000001,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7f800001,0xff800002,0x7fc12345,0xffc54321]
        arena[source:source+48]=words(patterns[seed%12:]+patterns[:seed%12])
        # Actual accessible actor pointer; any later overlapping write is retained.
        struct.pack_into('<I',arena,object_,BASE+actor_matrix-0x2c)
        header=words([object_,source,(0,0xcccccccc,0x7f800001)[seed%3]])
        cases.append((dict(source=source,actor_matrix=actor_matrix,object=object_,seed=seed),header+arena))
    for delta in range(-49,50):
        for seed in (0,1,7):
            add(0x18c+delta,0x92c,seed)
            add(0x92c+delta,0x92c,seed)
            add(0x18c+delta,0x18c-delta,seed)
    # Second destination aliases fields of the object and source in both directions.
    for dest in (0x100,0x101,0x104,0x188,0x18b,0x18c,0x18d,0x1b8,0x1bb):
        for source in (0x100,0x18c,0x92c,dest-1,dest,dest+1,0xd00):
            for seed in (2,5):add(source,dest,seed)
    for seed in range(64):
        r=random.Random(5000+seed);add(r.randrange(0x600,ARENA-48),r.randrange(0x600,ARENA-48),5000+seed)
    return cases

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',required=True,type=Path);p.add_argument('--candidate-source',type=Path);p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);a=p.parse_args()
    assert bool(a.recompiled)==bool(a.symbols)
    out=a.output.resolve();out.mkdir(parents=True,exist_ok=True)
    candidate=a.candidate_source.resolve()if a.candidate_source else SOURCE
    paths=[candidate,SOURCE,MATRIX,Path(__file__).resolve(),ROOT/'scripts/verify_phil_registry_reconstruction.py',ROOT/'scripts/verify_opponent_navigation_reconstruction.py',ROOT/'scripts/verify_opponent_route_reconstruction.py',ROOT/'scripts/verify_tinted_native_reconstruction.py',ROOT/'src/carma2/newcommon/49-physics.h',ROOT/'src/carma2/include/rec2_types.h',ROOT/'src/carma2/hooks/c2_hooks.h',ROOT/'src/brender/include/brender/br_types.h',ROOT/'src/brender/include/brender/brender.h']
    if a.recompiled:paths.extend([a.recompiled,a.symbols,a.recompiled.with_suffix('.map')])
    before={str(f):sha(f.read_bytes())for f in paths};(out/'source-before.json').write_text(json.dumps(before,indent=2))
    original=(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes();assert sha(original)==HASH
    images={};graphs={}
    images['original'],graphs['original']=image_graph(original,[x[1]for x in ENTRIES],{v:v+n for _,v,n in ENTRIES})
    if a.recompiled:
        binary=a.recompiled.read_bytes();mapping=json.loads(a.symbols.read_text());assert mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(binary)
        symbols={r['orig']:r['recomp']for r in mapping['symbols']};roots=[symbols[x[1]]for x in ENTRIES]
        map_text=a.recompiled.with_suffix('.map').read_text(encoding='latin1')
        starts={int(v,16)for v in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+\S+\s+([0-9a-fA-F]{8})\s+f\s+',map_text,re.M)}
        assert all(v in starts for v in roots)
        images['matching'],graphs['matching']=image_graph(binary,roots,{v:min(w for w in starts if w>v)for v in roots})
    for label,image in images.items():(out/(label+'-code.bin')).write_bytes(image)
    native=COMMON+extract(MATRIX.read_text(),'BrMatrix34Copy')+'\n'+extract(candidate.read_text(),'FUN_004b7220')+'\nint C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;builder_address=(unsigned)FUN_004b7220;return run(argv[1]);}\n'
    caller,_,_=registry.literals();caller=caller.replace('builder_poison;','builder_poison,builder_cdecl;')
    old=' mov builder_result,eax\n mov ecx,esp';assert old in caller
    caller=caller.replace(old,' mov builder_result,eax\n cmp builder_cdecl,0\n je object_matrix_stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n object_matrix_stackready:\n mov ecx,esp')
    registry.COMMON=COMMON;commands=registry.compile_sources(out,native,PHYSICAL,caller)
    cases=fixtures();fixture=out/'fixtures.bin';fixture.write_bytes(b''.join(b for _,b in cases))
    runs={};results=[]
    for label in ('original','msvc5','msvc','mingw',*(['matching']if a.recompiled else [])):
        command=[str(out/('physical.exe'if label in images else label+'.exe')),str(fixture)]
        if label in images:command.append(str(out/(label+'-code.bin')))
        proc=subprocess.run(command,capture_output=True,timeout=90);(out/(label+'-output.bin')).write_bytes(proc.stdout);(out/(label+'-stderr.bin')).write_bytes(proc.stderr)
        assert proc.returncode==0,(label,proc.returncode,proc.stderr)
        assert len(proc.stdout)==len(cases)*(ARENA+4),(label,len(proc.stdout))
        runs[label]=proc.stdout
    for index,(case,blob)in enumerate(cases):
        expected=runs['original'][index*(ARENA+4):(index+1)*(ARENA+4)]
        assert struct.unpack_from('<I',expected)[0]==index
        for label in runs.keys()-{'original'}:
            actual=runs[label][index*(ARENA+4):(index+1)*(ARENA+4)]
            passed=actual==expected
            results.append(dict(case_index=index,compiler=label,passed=passed,fixture_sha256=sha(blob),original_sha256=sha(expected),actual_sha256=sha(actual),case=case))
            if not passed:(out/f'failure-{label}-{index}.json').write_text(json.dumps(dict(case=case,original=expected.hex(),actual=actual.hex()),indent=2))
    after={f:sha(Path(f).read_bytes())for f in before};assert before==after
    report=dict(passed=all(r['passed']for r in results),scenes=len(cases),comparisons=len(results),source_sha256=before,original_sha256=HASH,graphs=graphs,commands=commands,artifacts={str(f):sha(f.read_bytes())for f in out.iterdir()if f.is_file()},results=results,limitations=__doc__)
    if a.recompiled:report.update(recompiled_sha256=sha(a.recompiled.read_bytes()),symbols_sha256=sha(a.symbols.read_bytes()))
    (out/'verification.json').write_text(json.dumps(report,indent=2)+'\n')
    assert report['passed'],f"{sum(not r['passed']for r in results)} failures retained"
    print('PASS object matrix',len(cases),len(results))

if __name__=='__main__':main()
