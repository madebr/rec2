"""Full contact-filter reference comparison with raw forward-copy alias cases.

Runs the complete original73B routine and native MSVC5/modern/MinGW bodies.
The full16KiB arena, raw EAX count, ESP, nonvolatile registers and x87TOP are
compared without output normalization. Optional final-PE mode executes the
actual linked entry and any emitted PhysicsCopyWord helper. Only accessible
bounded buffers with ABI-default clear direction flag are claimed.
"""
import argparse,json,random,re,struct,subprocess
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM
from verify_opponent_route_reconstruction import ROOT,HASH,raw,sha,words
from verify_tinted_native_reconstruction import extract
from verify_physics_object_matrix_reconstruction import PHYSICAL
import verify_phil_registry_reconstruction as registry

SOURCE=ROOT/'src/carma2/newcommon/49-physics.c'
ARENA=0x4000
COMMON=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "49-physics.h"
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison,builder_cdecl;
extern unsigned __cdecl BuilderInvoke(void);
typedef char layout_assert[sizeof(void*)==4&&sizeof(tPhysics_contact)==96&&offsetof(tPhysics_contact,type)==92?1:-1];
static int run(const char*path){
 FILE*f;unsigned h[5],head[2];unsigned char*arena;unsigned index=0;
 SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);
 f=fopen(path,"rb");if(!f)return 10;
 arena=(unsigned char*)VirtualAlloc((void*)0x22000000,16384,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
 if(arena!=(void*)0x22000000)return 11;_setmode(_fileno(stdout),_O_BINARY);
 while(fread(h,sizeof(h),1,f)==1){
  if(fread(arena,16384,1,f)!=1)return 12;
  builder_x=0x22000000+h[0];builder_y=h[1];builder_poison=h[4];builder_mode=0;
#ifdef REC2_MATCHING
  builder_stack_count=2;builder_cdecl=0;builder_stack[0]=0x22000000+h[2];builder_stack[1]=h[3];
#else
  builder_stack_count=4;builder_cdecl=1;builder_stack[0]=builder_x;builder_stack[1]=builder_y;builder_stack[2]=0x22000000+h[2];builder_stack[3]=h[3];
#endif
  _controlfp(_PC_53,_MCW_PC);_clearfp();
  if(BuilderInvoke())return 14;
  head[0]=index++;head[1]=builder_result;fwrite(head,sizeof(head),1,stdout);fwrite(arena,16384,1,stdout);
 }
 if(!feof(f))return 13;fclose(f);return 0;
}
'''

def code_image(binary,start,end,helper=None):
    cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
    roots={start:end}
    if helper:roots[helper[0]]=helper[1]
    functions={};instructions={};edges=[];visited_functions=set();todo=[start]
    while todo:
        root=todo.pop()
        if root in visited_functions:continue
        visited_functions.add(root);pending=[root];seen={}
        while pending:
            address=pending.pop()
            while address not in seen:
                assert root<=address<roots[root]
                i=next(cs.disasm(raw(binary,address,15),address));assert i.address+i.size<=roots[root]
                for op in i.operands:
                    if op.type==CS_OP_MEM:assert op.mem.base or op.mem.index,('absolute data',i.op_str)
                seen[address]=bytes(i.bytes);instructions[address]=bytes(i.bytes)
                if i.mnemonic=='call':
                    assert helper and i.operands[0].type==CS_OP_IMM and i.operands[0].imm==helper[0]
                    edges.append(dict(site=hex(address),target=hex(helper[0])));todo.append(helper[0])
                if i.mnemonic.startswith('j'):
                    assert i.operands[0].type==CS_OP_IMM
                    target=i.operands[0].imm;assert root<=target<roots[root];pending.append(target)
                    if i.mnemonic=='jmp':break
                if i.mnemonic=='ret':
                    assert (i.operands[0].imm if i.operands else 0)==(8 if root==start else 0)
                    break
                address+=i.size
        functions[hex(root)]={hex(v):b.hex()for v,b in sorted(seen.items())}
    low=min(instructions);high=max(v+len(b)for v,b in instructions.items());image=bytearray(b'\xcc'*(high-low))
    for v,b in instructions.items():image[v-low:v-low+len(b)]=b
    return words([len(image),start-low])+image,dict(functions=functions,calls=edges,low=hex(low),high=hex(high))

def fixtures():
    cases=[]
    def add(count,n,pattern,source=0x500,output=0x2000,seed=0):
        r=random.Random(seed+17);arena=bytearray(r.randbytes(ARENA))
        for i in range(max(n,0)):
            assert source+(i+1)*96<=ARENA
            value=9 if pattern==0 else 0 if pattern==1 else (9,8,10,-1)[i%4]
            struct.pack_into('<I',arena,source+i*96+92,value&0xffffffff)
        h=words([output,count,source,n,(0,0xcccccccc,0x7f800001)[seed%3]])
        case=dict(count=count,input_count=n,pattern=pattern,source=source,output=output,seed=seed)
        cases.append((case,h+arena))
    for count in (-2147483648,-2,-1,0,1,38,39,40,41,2147483647):
        for n in (-2147483648,-1,0,1,2,40,41,45):
            for pattern in (0,1,2):add(count,n,pattern,seed=len(cases))
    for delta in range(-99,100):
        for pattern in (0,1,2):add(1,7,pattern,source=0x2060+delta,seed=len(cases))
    for seed in range(80):
        r=random.Random(6000+seed);add(r.randrange(0,41),r.randrange(1,46),r.randrange(3),source=r.randrange(0x100,0x1000),seed=seed+6000)
    return cases

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',required=True,type=Path);p.add_argument('--candidate-source',type=Path);p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);a=p.parse_args();assert bool(a.recompiled)==bool(a.symbols)
    out=a.output.resolve();out.mkdir(parents=True,exist_ok=True);candidate=a.candidate_source.resolve()if a.candidate_source else SOURCE
    paths=[candidate,SOURCE,Path(__file__).resolve(),ROOT/'scripts/verify_physics_object_matrix_reconstruction.py',ROOT/'scripts/verify_phil_registry_reconstruction.py',ROOT/'scripts/verify_opponent_navigation_reconstruction.py',ROOT/'scripts/verify_opponent_route_reconstruction.py',ROOT/'scripts/verify_tinted_native_reconstruction.py',ROOT/'src/carma2/newcommon/49-physics.h',ROOT/'src/carma2/include/rec2_types.h',ROOT/'src/carma2/hooks/c2_hooks.h']
    if a.recompiled:paths.extend([a.recompiled,a.symbols,a.recompiled.with_suffix('.map')])
    before={str(f):sha(f.read_bytes())for f in paths};(out/'source-before.json').write_text(json.dumps(before,indent=2))
    binary=(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes();assert sha(binary)==HASH
    images={};graphs={};images['original'],graphs['original']=code_image(binary,0x428cf0,0x428d39)
    assert sum(len(bytes.fromhex(b))for b in graphs['original']['functions']['0x428cf0'].values())==73
    if a.recompiled:
        binary=a.recompiled.read_bytes();mapping=json.loads(a.symbols.read_text());assert mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(binary)
        start=next(r['recomp']for r in mapping['symbols']if r['orig']==0x428cf0)
        rows=re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',a.recompiled.with_suffix('.map').read_text(encoding='latin1'),re.M)
        functions={int(v,16):(n,o.strip())for n,v,o in rows};assert start in functions
        helper=[v for v,(n,o)in functions.items()if n.lstrip('_@').split('@')[0]=='PhysicsCopyWord'and o=='49-physics.c.obj'];assert len(helper)<=1
        bound=lambda v:min(w for w in functions if w>v)
        images['matching'],graphs['matching']=code_image(binary,start,bound(start),(helper[0],bound(helper[0]))if helper else None)
    for label,image in images.items():(out/(label+'-code.bin')).write_bytes(image)
    native=COMMON+extract(SOURCE.read_text(),'PhysicsCopyWord')+'\n'+extract(candidate.read_text(),'FUN_00428cf0')+'\nint C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;builder_address=(unsigned)FUN_00428cf0;return run(argv[1]);}\n'
    caller,_,_=registry.literals();caller=caller.replace('builder_poison;','builder_poison,builder_cdecl;')
    old=' mov builder_result,eax\n mov ecx,esp';assert old in caller
    caller=caller.replace(old,' mov builder_result,eax\n cmp builder_cdecl,0\n je contact_filter_stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n contact_filter_stackready:\n mov ecx,esp')
    registry.COMMON=COMMON;commands=registry.compile_sources(out,native,PHYSICAL,caller)
    cases=fixtures();fixture=out/'fixtures.bin';fixture.write_bytes(b''.join(b for _,b in cases));runs={};results=[]
    for label in ('original','msvc5','msvc','mingw',*(['matching']if a.recompiled else [])):
        command=[str(out/('physical.exe'if label in images else label+'.exe')),str(fixture)]
        if label in images:command.append(str(out/(label+'-code.bin')))
        proc=subprocess.run(command,capture_output=True,timeout=90);(out/(label+'-output.bin')).write_bytes(proc.stdout);(out/(label+'-stderr.bin')).write_bytes(proc.stderr)
        assert proc.returncode==0,(label,proc.returncode,proc.stderr);assert len(proc.stdout)==len(cases)*(ARENA+8)
        runs[label]=proc.stdout
    for index,(case,blob)in enumerate(cases):
        expected=runs['original'][index*(ARENA+8):(index+1)*(ARENA+8)];assert struct.unpack_from('<I',expected)[0]==index
        for label in runs.keys()-{'original'}:
            actual=runs[label][index*(ARENA+8):(index+1)*(ARENA+8)];passed=expected==actual
            results.append(dict(case_index=index,compiler=label,passed=passed,fixture_sha256=sha(blob),original_sha256=sha(expected),actual_sha256=sha(actual),case=case))
            if not passed:(out/f'failure-{label}-{index}.json').write_text(json.dumps(dict(case=case,original=expected.hex(),actual=actual.hex()),indent=2))
    assert before=={f:sha(Path(f).read_bytes())for f in before}
    report=dict(passed=all(r['passed']for r in results),scenes=len(cases),comparisons=len(results),source_sha256=before,original_sha256=HASH,graphs=graphs,commands=commands,artifacts={str(f):sha(f.read_bytes())for f in out.iterdir()if f.is_file()},results=results,limitations=__doc__)
    if a.recompiled:report.update(recompiled_sha256=sha(a.recompiled.read_bytes()),symbols_sha256=sha(a.symbols.read_bytes()))
    (out/'verification.json').write_text(json.dumps(report,indent=2)+'\n');assert report['passed'],f"{sum(not r['passed']for r in results)} failures retained"
    print('PASS contact filter',len(cases),len(results))

if __name__=='__main__':main()
