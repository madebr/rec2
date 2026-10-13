"""Complete physical BrMatrix34TApply original/native3/actual linked proof.

Defaults extract the actual production leaf and four private arithmetic helpers.
525 deterministic fixtures preserve finite, IEEE, partial/unaligned overlaps and
raw full4096-byte arena state. The actual142B original executes without math
emulation, and optional hash-bound rebuilt transport closes every real helper.
Normal CDECL3 calls check ESP/nonvolatiles/x87TOP; void EAX is excluded. PC53,
masked exceptions only; no FP status/unmasked-trap, invalid pointers or nonx86
claim. A candidate override is an explicit development source, never accepted
as production automatically. No ignored fixture/driver file is required.
"""
import argparse,ast,json,random,re,struct,subprocess,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'scripts'))
from verify_tinted_native_reconstruction import extract
from verify_opponent_route_reconstruction import HASH,sha,pe,raw
import verify_phil_registry_reconstruction as registry
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_MEM,CS_OP_IMM
NAMES=('Matrix34ApplyProductLoaded','Matrix34ApplyProductMemory','Matrix34ApplyAdd','Matrix34ApplyAddScalar','BrMatrix34TApply')

def linked_transport(binary,mapping,map_text):
    assert mapping['format']==1 and mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(binary)
    symbols={r['orig']:r['recomp']for r in mapping['symbols']};root=symbols[0x533590]
    functions={int(a,16):(n,o.strip())for n,a,o in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',map_text,re.M)}
    nt,base,size,tab=pe(binary);flat=bytearray(size)
    for k in range(struct.unpack_from('<H',binary,nt+6)[0]):
        vs,rva,n,off=struct.unpack_from('<IIII',binary,tab+40*k+8);flat[rva:rva+n]=binary[off:off+n]
    cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;pending=[root];graph=[];seen=set()
    while pending:
        start=pending.pop()
        if start in seen:continue
        seen.add(start);assert start in functions
        if start!=root:
            name,owner=functions[start]
            assert name.lstrip('_')in NAMES[:-1]and owner in('matrix34.c.obj','libbrender:matrix34.c.obj','brender:matrix34.c.obj'),('unapproved helper',hex(start),functions[start])
        end=min(a for a in functions if a>start);at=start;instructions=[];calls=[]
        while at<end:
            i=next(cs.disasm(raw(binary,at,15),at,count=1));assert i.address+i.size<=end
            assert not i.mnemonic.startswith('j'),('unexpected branch',functions[start],i.op_str)
            for op in i.operands:
                assert not(op.type==CS_OP_MEM and base<=op.mem.disp<base+size),('unexpected absolute data',functions[start],i.op_str)
                assert not(op.type==CS_OP_IMM and i.mnemonic in('mov','push')and base<=op.imm<base+size),('unexpected pointer immediate',functions[start],i.op_str)
            if i.mnemonic=='call':
                assert i.operands[0].type==CS_OP_IMM;target=i.operands[0].imm
                pending.append(target);calls.append(dict(site=i.address,target=target))
            instructions.append(dict(address=i.address,hex=i.bytes.hex(),mnemonic=i.mnemonic,operands=i.op_str));at+=i.size
            if i.mnemonic=='ret':assert not i.operands;break
        assert instructions[-1]['mnemonic']=='ret'
        graph.append(dict(name=functions[start][0],owner=functions[start][1],address=start,bytes=at-start,sha256=sha(raw(binary,start,at-start)),instructions=instructions,calls=calls))
    # This complete scalar-only closure has no absolute data/code operands.
    # Confirm all HIGHLOW relocations in the decoded bodies are absent too.
    rr,rs=struct.unpack_from('<II',binary,nt+24+96+5*8);cur=rr
    while cur<rr+rs:
        page,n=struct.unpack('<II',raw(binary,base+cur,8));assert n>=8
        for word in struct.unpack('<'+'H'*((n-8)//2),raw(binary,base+cur+8,n-8)):
            if word>>12==3:
                address=base+page+(word&4095)
                assert not any(row['address']<=address<row['address']+row['bytes']for row in graph),('unaccounted relocation',hex(address))
        cur+=n
    return struct.pack('<4I',size,*([root-base]*3))+flat,graph

p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',required=True,type=Path);p.add_argument('--candidate',type=Path);p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);args=p.parse_args();assert bool(args.recompiled)==bool(args.symbols)
OUT=args.output.resolve();assert not OUT.exists()or not any(OUT.iterdir()),'Use a new empty output directory';OUT.mkdir(parents=True,exist_ok=True)
matrix=ROOT/'src/brender/core/math/matrix34.c';candidate=args.candidate.resolve()if args.candidate else matrix
paths=[Path(__file__),candidate,matrix,ROOT/'scripts/verify_tinted_native_reconstruction.py',ROOT/'scripts/verify_opponent_route_reconstruction.py',ROOT/'scripts/verify_opponent_navigation_reconstruction.py',ROOT/'scripts/verify_phil_registry_reconstruction.py']+list((ROOT/'src').rglob('*.h'))
if args.recompiled:paths.extend([args.recompiled,args.symbols,args.recompiled.with_suffix('.map')])
before={str(f):sha(f.read_bytes())for f in paths};(OUT/'source-before.json').write_text(json.dumps(before,indent=2));(OUT/'verifier-snapshot.py').write_bytes(Path(__file__).read_bytes());(OUT/'candidate-snapshot.c').write_bytes(candidate.read_bytes())
original=(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes();assert sha(original)==HASH
nt,base,size,tab=pe(original);image=bytearray(size)
for k in range(struct.unpack_from('<H',original,nt+6)[0]):
    vs,rva,n,off=struct.unpack_from('<IIII',original,tab+40*k+8);image[rva:rva+n]=original[off:off+n]
cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;ins=list(cs.disasm(raw(original,0x533590,142),0x533590));assert sum(i.size for i in ins)==142 and ins[-1].mnemonic=='ret'and not ins[-1].operands
for i in ins:
    assert i.mnemonic!='call'and not i.mnemonic.startswith('j')
    assert not any(op.type==CS_OP_MEM and base<=op.mem.disp<base+size for op in i.operands)
original_image=OUT/'original-transport.bin';original_image.write_bytes(struct.pack('<4I',size,*([0x133590]*3))+image)
linked=None
if args.recompiled:
    linked_image,linked_graph=linked_transport(args.recompiled.read_bytes(),json.loads(args.symbols.read_text()),args.recompiled.with_suffix('.map').read_text(encoding='latin1'))
    (OUT/'matching-transport.bin').write_bytes(linked_image)
    linked=dict(recompiled_sha256=sha(args.recompiled.read_bytes()),symbols_sha256=sha(args.symbols.read_bytes()),map_sha256=sha(args.recompiled.with_suffix('.map').read_bytes()),graph=linked_graph,transport_sha256=sha(linked_image),relocations=[],controlled_game_boundaries=[])

driver='\n#include <windows.h>\n#include <stdio.h>\n#include <stdlib.h>\n#include <string.h>\n#include <float.h>\n#include <io.h>\n#include <fcntl.h>\nextern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[6],builder_stack_count,builder_result,builder_poison,builder_cdecl;\nextern unsigned __cdecl BuilderInvoke(void);\nint __cdecl main(int argc,char**argv){FILE*f;unsigned n,entries[3],h[4];unsigned char*code,*arena;\nif(argc!=3)return 1;f=fopen(argv[2],"rb");if(!f)return 2;if(fread(&n,4,1,f)!=1||fread(entries,12,1,f)!=1)return 3;\ncode=(unsigned char*)VirtualAlloc((void*)0x50000000,n,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);arena=(unsigned char*)VirtualAlloc((void*)0x20000000,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);if(code!=(void*)0x50000000||arena!=(void*)0x20000000)return 4;\nif(fread(code,n,1,f)!=1)return 5;fclose(f);f=fopen(argv[1],"rb");if(!f)return 6;\n_setmode(_fileno(stdout),_O_BINARY);_controlfp(_PC_53,_MCW_PC);\nwhile(fread(h,sizeof(h),1,f)==1){if(fread(arena,4096,1,f)!=1)return 7;\nbuilder_address=(unsigned)(code+entries[h[0]]);builder_mode=1;builder_poison=0xcccccccc;builder_x=0xcccccccc;builder_y=0xcccccccc;builder_cdecl=1;builder_stack_count=3;\nbuilder_stack[0]=(unsigned)(arena+h[1]);builder_stack[1]=(unsigned)(arena+h[2]);builder_stack[2]=(unsigned)(arena+h[3]);\nif(BuilderInvoke())return 14;fwrite(arena,4096,1,stdout);}\nfclose(f);return 0;}\n'

tree=ast.parse((ROOT/'scripts/verify_opponent_navigation_reconstruction.py').read_text());caller=next(n.value.value for n in ast.walk(tree)if isinstance(n,ast.Assign)and isinstance(n.value,ast.Constant)and any(isinstance(t,ast.Name)and t.id=='caller'for t in n.targets))
caller=caller.replace('builder_stack[5]','builder_stack[6]').replace('builder_poison;','builder_poison,builder_cdecl;').replace(' mov builder_result,eax\n mov ecx,esp',' mov builder_result,eax\n cmp builder_cdecl,0\n je stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n stackready:\n mov ecx,esp')
prefix='#include "49-physics.h"\n#include <math.h>\n'
mat=matrix.read_text();draft=candidate.read_text();capsules={name:extract(mat if name in NAMES[:3]else draft,name)for name in NAMES};body='\n'.join(capsules.values())
(OUT/'verified-capsules.c').write_text(body)
native_driver=driver.replace('builder_address=(unsigned)(code+entries[h[0]]);','builder_address=(unsigned)BrMatrix34TApply;')
registry.COMMON='';commands=registry.compile_sources(OUT,prefix+body+native_driver,driver,caller)

cases=[]
def case(mode,label,values,patches=(),dest=0x100,vector=0x200,matrix=0x300):
    b=bytearray((k*43+17)&255 for k in range(4096));struct.pack_into('<16f',b,0x200,*values[:4],*[0.]*12);struct.pack_into('<12f',b,0x300,*values[4:])
    for at,value in patches:struct.pack_into('<I',b,at,value)
    cases.append((struct.pack('<4I',mode,dest,vector,matrix)+b,dict(mode=mode,label=label,output=dest,vector=vector,matrix=matrix)))
rng=random.Random(533590)
baseline=[.7,-1.3,2.9,-.4,1.,2.,3.,4.,5.,6.,7.,8.,9.,10.,11.,12.]
specials=[0,0x80000000,1,0x80000001,0x007fffff,0x00800000,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc12345,0xffc54321,0x7f800001,0xff800002]
for mode in range(3):
    for k in range(140):
        vals=[rng.uniform(-2,2)*2.**rng.randint(-110,110)for _ in range(16)]
        case(mode,'finite-%d'%k,vals)
    for at in [0x200+4*k for k in range(4)]+[0x300+4*k for k in range(12)]:
        for value in specials:case(mode,'raw-%x-%x'%(at,value),baseline,[(at,value)])
    for pair in [(a,b)for a in specials[-4:]for b in specials[-4:]]:
        for a,b in ((0x200,0x300),(0x204,0x304),(0x208,0x308),(0x204,0x308),(0x200,0x204),(0x300,0x304),(0x308,0x32c),(0x20c,0x32c)):
            case(mode,'pair-%x-%x-%x-%x'%(a,b,*pair),baseline,[(a,pair[0]),(b,pair[1])])
    for dest in(0x1ff,0x200,0x201,0x204,0x208,0x20c,0x2ff,0x300,0x301,0x304,0x308,0x310,0x320,0x324,0x32c):
        case(mode,'overlap-%x'%dest,baseline,dest=dest)
        case(mode,'overlap-raw-%x'%dest,baseline,[(0x200,0x7f800001),(0x304,0xffc54321)],dest=dest)
    for shift in(1,2,3):case(mode,'unaligned-input-%d'%shift,baseline,vector=0x200+shift,matrix=0x300+shift,dest=0x100+shift)

selected=[(i,blob,meta)for i,(blob,meta)in enumerate(cases)if meta['mode']==1];assert len(selected)==525
inp=OUT/'fixtures.bin';inp.write_bytes(b''.join(blob for i,blob,meta in selected));outputs={}
for tag in('original','msvc5','msvc','mingw',*(['matching']if linked else [])):
    actual_image=OUT/('matching-transport.bin'if tag=='matching'else 'original-transport.bin')
    result=subprocess.run([str(OUT/('physical.exe'if tag in('original','matching')else tag+'.exe')),str(inp),str(actual_image)],capture_output=True,timeout=60)
    (OUT/(tag+'-outputs.bin')).write_bytes(result.stdout);(OUT/(tag+'-stderr.bin')).write_bytes(result.stderr)
    assert result.returncode==0 and len(result.stdout)==4096*len(selected),(tag,result.returncode,len(result.stdout),result.stderr)
    outputs[tag]=result.stdout
results=[];failures=[]
for k,(index,blob,meta)in enumerate(selected):
    expected=outputs['original'][k*4096:(k+1)*4096]
    for tag in('msvc5','msvc','mingw',*(['matching']if linked else [])):
        actual=outputs[tag][k*4096:(k+1)*4096];same=actual==expected
        row=dict(index=k,parent_index=index,**meta,compiler=tag,passed=same,fixture_sha256=sha(blob),fixture_length=len(blob),original_sha256=sha(expected),output_sha256=sha(actual),original_length=len(expected),output_length=len(actual));results.append(row)
        if not same:failures.append(dict(**row,offsets=[j for j,(a,b)in enumerate(zip(expected,actual))if a!=b]))
after={str(f):sha(f.read_bytes())for f in paths};assert before==after
v=dict(passed=not failures,scenes=len(selected),comparisons=len(results),failures=failures,results=results,source_before=before,source_after=after,whole_sources_unchanged=True,script_sha256=sha(Path(__file__).read_bytes()),capsule_sha256={n:sha(b.encode())for n,b in capsules.items()},original_sha256=HASH,original=dict(address=0x533590,bytes=142,sha256=sha(raw(original,0x533590,142)),instructions=[dict(address=i.address,hex=i.bytes.hex(),mnemonic=i.mnemonic,operands=i.op_str)for i in ins]),linked=linked,recompiled_sha256=linked['recompiled_sha256']if linked else None,symbols_sha256=linked['symbols_sha256']if linked else None,commands=commands,fixture_sha256=sha(inp.read_bytes()),artifacts={f.name:sha(f.read_bytes())for f in OUT.iterdir()if f.is_file()},scope=__doc__)
(OUT/'verification.json').write_text(json.dumps(v,indent=2)+'\n');print(json.dumps(dict(passed=v['passed'],scenes=len(selected),comparisons=len(results),failures=len(failures)),indent=2));raise SystemExit(bool(failures))
