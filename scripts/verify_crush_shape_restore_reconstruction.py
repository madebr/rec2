#!/usr/bin/env python3
"""Complete63B collision shape restore, original/native3/current Win32x86.

Genuine call-free root executes intact; no hull, geometry, API or math boundary.
Fixed65536B arena is compared raw after every invocation. Void EAX is not an
observation; ESP/nonvolatiles/x87TOP are checked at masked PC53. Raw IEEE words,
live count changes and per-coordinate array pointer reload aliases are covered.
Fixtures are finite mapped Win32 object storage; unaligned/effective-type alias
cases do not claim portable ISO C. No invalid-memory, concurrent, unmasked,
private-stack, exhaustive-input or whole-game equivalence claim.
"""
import argparse,json,random,re,struct,subprocess,sys
from pathlib import Path
R=next(p for p in Path(__file__).resolve().parents if(p/'src/carma2').is_dir());sys.path.insert(0,str(R/'scripts'))
import verify_phil_registry_reconstruction as registry
from verify_opponent_route_reconstruction import raw,pe,HASH,sha,words
from verify_tinted_native_reconstruction import extract
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM,CS_GRP_CALL,CS_GRP_JUMP
NAME='CompletelyUnBendCollisionShape';OLD=0x439fd0;SOURCE=R/'src/carma2/newcommon/13-crush1.c'
FIELDS='shape_offset original_count previous_count points_offset alias override repeat seed poison pattern'.split()
COMMON=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "13-crush1.h"
static unsigned char*arena;static unsigned entry;
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison,builder_cdecl;
extern unsigned __cdecl BuilderInvoke(void);
static int run(const char*path){FILE*f;unsigned h[10],i;
if(sizeof(void*)!=4||sizeof(tCar_crush_shape_info)!=28||sizeof(tCar_crush_reordered_shape_info)!=56||offsetof(tCar_crush_shape_info,count_points)!=12||offsetof(tCar_crush_shape_info,original_count_points)!=20||offsetof(tCar_crush_shape_info,field_0x18)!=24||offsetof(tCar_crush_reordered_shape_info,field_0x18)!=24||offsetof(tCar_crush_reordered_shape_info,field_0x2c)!=44)return 15;
SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);arena=VirtualAlloc((void*)0x20000000,65536,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(arena!=(void*)0x20000000)return 11;f=fopen(path,"rb");if(!f)return 10;_setmode(_fileno(stdout),_O_BINARY);_controlfp(_PC_53,_MCW_PC);_clearfp();
while(fread(h,sizeof(h),1,f)==1){if(fread(arena,65536,1,f)!=1)return 12;for(i=0;i<h[6];i++){builder_address=entry;builder_x=(unsigned)arena+h[0];builder_y=h[8]^0x21436587u;builder_mode=1;builder_poison=h[8];
#ifdef REC2_MATCHING
builder_cdecl=0;builder_stack_count=0;
#else
builder_cdecl=1;builder_stack_count=1;builder_stack[0]=builder_x;
#endif
if(BuilderInvoke())return 14;fwrite(arena,65536,1,stdout);}}
if(!feof(f))return 16;fclose(f);return 0;}
'''
LOADER=r'''
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned char*disk,*image;unsigned n,i,h;IMAGE_NT_HEADERS*nt;IMAGE_SECTION_HEADER*s;
if(argc!=4)return 1;f=fopen(argv[2],"rb");if(!f)return 2;fseek(f,0,SEEK_END);n=(unsigned)ftell(f);rewind(f);disk=malloc(n);if(!disk||fread(disk,n,1,f)!=1)return 3;fclose(f);nt=(IMAGE_NT_HEADERS*)(disk+((IMAGE_DOS_HEADER*)disk)->e_lfanew);image=VirtualAlloc(NULL,nt->OptionalHeader.SizeOfImage,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image)return 4;memcpy(image,disk,nt->OptionalHeader.SizeOfHeaders);s=IMAGE_FIRST_SECTION(nt);for(i=0;i<nt->FileHeader.NumberOfSections;i++)memcpy(image+s[i].VirtualAddress,disk+s[i].PointerToRawData,s[i].SizeOfRawData);f=fopen(argv[3],"rb");if(!f||fread(&h,4,1,f)!=1)return 5;fclose(f);entry=(unsigned)image+h;FlushInstructionCache(GetCurrentProcess(),image,nt->OptionalHeader.SizeOfImage);free(disk);return run(argv[1]);}
'''

def packet(binary,start,n,path,identity):
    nt,base,size,tab=pe(binary);cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;end=start+n;todo=[start];ins={};edges=[]
    def section(a,n):
        rows=[]
        for k in range(struct.unpack_from('<H',binary,nt+6)[0]):
            vs,rva,rs,off=struct.unpack_from('<4I',binary,tab+40*k+8);flags=struct.unpack_from('<I',binary,tab+40*k+36)[0]
            if rva<=a-base and a-base+n<=rva+max(vs,rs):rows.append(flags)
        assert len(rows)==1;return rows[0]
    flags=section(start,n);assert flags&0x20000000 and not flags&0x80000000
    while todo:
        a=todo.pop()
        if a in ins:continue
        assert start<=a<end;i=next(cs.disasm(raw(binary,a,min(15,end-a)),a),None);assert i is not None;flags=section(a,i.size);assert flags&0x20000000 and not flags&0x80000000;ins[a]=i
        assert i.mnemonic not in('int3','ud2','int','hlt')
        assert CS_GRP_CALL not in i.groups,('No callee substitute/hidden helper',hex(a),i.op_str)
        for op in i.operands:
            if op.type==CS_OP_MEM:assert op.mem.base and not base<=op.mem.disp<base+size,('No absolute/global operand',hex(a),i.op_str)
            elif op.type==CS_OP_IMM and CS_GRP_JUMP not in i.groups:assert not base<=op.imm<base+size,('Unexpected address literal',hex(a),i.op_str)
        if i.mnemonic.startswith('ret'):continue
        if CS_GRP_JUMP in i.groups:
            op=i.operands[0];assert op.type==CS_OP_IMM and start<=op.imm<end;edges.append(dict(site=hex(a),target=hex(op.imm),kind=i.mnemonic));todo.append(op.imm)
            if i.mnemonic=='jmp':continue
        assert a+i.size<end;todo.append(a+i.size)
    occupied=set()
    for i in ins.values():bits=set(range(i.address,i.address+i.size));assert not occupied&bits;occupied.update(bits)
    rr,rz=struct.unpack_from('<II',binary,nt+24+96+5*8);cursor=rr;active={a-base+j for a,i in ins.items()for j in range(i.size)}
    while cursor<rr+rz:
        page,nblock=struct.unpack('<II',raw(binary,base+cursor,8));assert nblock>=8 and cursor+nblock<=rr+rz
        for v in struct.unpack('<'+'H'*((nblock-8)//2),raw(binary,base+cursor+8,nblock-8)):
            assert v>>12 in(0,3);assert not(v>>12==3 and page+(v&4095)in active),('Unexpected active relocation',hex(page+(v&4095)))
        cursor+=nblock
    assert cursor==rr+rz
    if not rz:assert struct.unpack_from('<H',binary,nt+22)[0]&1
    path.write_bytes(words([start-base]))
    return dict(name=NAME,address=hex(start),extent=n,instructions=len(ins),full_span_sha256=sha(raw(binary,start,n)),reachable=[[hex(a),i.bytes.hex()]for a,i in sorted(ins.items())],identity=identity,edges=edges,callees=[],globals=[],readonly=[],absolute_operands=[],indirect_edges=[],relocation_directory=[rr,rz],packet_sha256=sha(path.read_bytes()),policy='Full unchanged complete call-free root, no code/data/callee patches, API or float/pointer normalization.')

def linked_packet(binary,symbol_bytes,map_bytes,path):
    s=json.loads(symbol_bytes);assert s['format']==1 and s['original_sha256']==HASH and s['recompiled_sha256']==sha(binary);mapping={r['orig']:r['recomp']for r in s['symbols']};assert OLD in mapping
    rows={}
    for name,a,owner in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',map_bytes.decode('latin1'),re.M):rows.setdefault(int(a,16),[]).append((name,owner.strip()))
    a=mapping[OLD];expected=('@'+NAME+'@4','13-crush1.c.obj');assert rows.get(a)==[expected],('Exact public MAP identity/owner',rows.get(a));n=min(v for v in rows if v>a)-a
    m=packet(binary,a,n,path,dict(original=hex(OLD),actual=hex(a),symbol=expected[0],owner=expected[1]));m.update(recompiled_sha256=sha(binary),symbols_sha256=sha(symbol_bytes),map_sha256=sha(map_bytes));return m

def cases():
    rows=[]
    def add(**v):
        c=dict(shape_offset=0x800,original_count=4,previous_count=9,points_offset=0x2000,alias=0,override=2,repeat=2,seed=len(rows)+1,poison=0xcccccccc,pattern=0);c.update(v);rows.append(c)
    for count in[-3,-1,0,1,2,4,8,16,32,128]:
        for old in[0,-1,3,0x7fffffff]:
            for off in[0,1]:
                for pattern in[0,1]:add(original_count=count,previous_count=old,shape_offset=0x800+off,points_offset=0x2000+off,pattern=pattern)
    for n in[-1,0]:
        for pointer in[0,1,0xffffffff,0x80000000]:add(original_count=n,points_offset=pointer,alias=9)
    for component in range(3):
        for count in[1,2,4]:
            for newcount in[-1,0,1,2,4,7]:add(original_count=count,alias=1+component,override=newcount)
            for off in[0,1]:add(original_count=count,alias=4+component,shape_offset=0x800+off)
    for count in[1,2,4]:
        for newcount in[0,1,2,4]:add(original_count=count,alias=7,override=newcount)
    for poison in[0,1,0xffffffff,0x7fc01234,0x12345678]:
        for count in[0,1,2,16]:add(original_count=count,poison=poison,pattern=1)
    return rows

def fixture(c):
    rng=random.Random(c['seed']);arena=bytearray(words([rng.getrandbits(32)for _ in range(65536//4)]));s=c['shape_offset'];base=0x20000000
    def put(a,x):struct.pack_into('<I',arena,a,x&0xffffffff)
    ieee=[0,0x80000000,1,0x80000001,0x7f800000,0xff800000,0x7fc01234,0xffc01234,0x7f800001,0xff800001,0x7fbfffff,0xffbfffff]
    for p in[0x2000+(s&1),0x5000+(s&1)]:
        for i in range(129):
            for j in range(3):put(p+i*56+44+j*4,ieee[(i*3+j)%len(ieee)]if c['pattern']else rng.getrandbits(32))
    put(s+12,c['previous_count']);put(s+20,c['original_count']);p=c['points_offset']
    if c['alias']in[1,2,3]:
        axis=c['alias']-1;p=s+12-(24+axis*4);put(s+32,c['override']);put(s+40,2)
    elif c['alias']in[4,5,6]:
        axis=c['alias']-4;p=s+24-(24+axis*4);put(s+44,base+0x5000+(s&1));put(s+40,2)
    elif c['alias']==7:p=s-12;put(s+32,c['override']);put(s+40,0)
    put(s+24,p if c['alias']==9 else base+p)
    return words([c[k]for k in FIELDS])+arena

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',required=True,type=Path);p.add_argument('--candidate',type=Path);p.add_argument('--original',type=Path,default=R/'build-reference/originals/CARMA2_HW.EXE');p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);p.add_argument('--fixtures-dir',type=Path);a=p.parse_args();assert bool(a.symbols)==bool(a.recompiled);out=a.output.resolve();assert not out.exists();out.mkdir(parents=True);candidate=(a.candidate or SOURCE).resolve()
    paths=[candidate,SOURCE,Path(__file__),a.original.resolve(),R/'scripts/verify_phil_registry_reconstruction.py',R/'scripts/verify_opponent_navigation_reconstruction.py',R/'scripts/verify_opponent_route_reconstruction.py',R/'scripts/verify_tinted_native_reconstruction.py',SOURCE.with_suffix('.h')]+list((R/'src/carma2/include').rglob('*.h'))+list((R/'src/carma2/hooks').rglob('*.h'))+list((R/'src/brender/include').rglob('*.h'))
    if a.symbols:paths +=[a.recompiled.resolve(),a.recompiled.resolve().with_suffix('.map'),a.symbols.resolve()]
    before={str(x.resolve()):x.read_bytes()for x in paths};guards={x:sha(b)for x,b in before.items()};(out/'source-before.json').write_text(json.dumps(guards,indent=2)+'\n');binary=before[str(a.original.resolve())];assert sha(binary)==HASH
    original=packet(binary,OLD,63,out/'original-packet.bin',dict(original=hex(OLD),name=NAME));assert original['instructions']==28
    native=COMMON+'\n'+extract(before[str(candidate)].decode(),NAME)+'\nint C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;entry=(unsigned)CompletelyUnBendCollisionShape;return run(argv[1]);}\n'
    caller,_,_=registry.literals();caller=caller.replace('builder_poison;','builder_poison,builder_cdecl;').replace(' mov builder_result,eax\n mov ecx,esp',' mov builder_result,eax\n cmp builder_cdecl,0\n je unbend_stack\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n unbend_stack:\n mov ecx,esp');assert 'unbend_stack:'in caller
    registry.COMMON=COMMON;commands=registry.compile_sources(out,native,LOADER,caller);rows=cases();inputs=b''.join(fixture(c)for c in rows);(out/'cases.bin').write_bytes(inputs)
    if a.fixtures_dir:assert(a.fixtures_dir/'cases.bin').read_bytes()==inputs
    matching=linked_packet(before[str(a.recompiled.resolve())],before[str(a.symbols.resolve())],before[str(a.recompiled.resolve().with_suffix('.map'))],out/'matching-packet.bin')if a.symbols else None
    outputs={};receipts=[]
    for name in['original','msvc5','msvc','mingw']+(['matching']if a.symbols else[]):
        cmd=[str(out/'physical.exe'),str(out/'cases.bin'),str(a.original.resolve()if name=='original'else a.recompiled.resolve()),str(out/('original-packet.bin'if name=='original'else'matching-packet.bin'))]if name in('original','matching')else[str(out/(name+'.exe')),str(out/'cases.bin')]
        with(out/f'{name}.bin').open('wb')as f:r=subprocess.run(cmd,stdout=f,stderr=subprocess.PIPE,timeout=60)
        (out/f'{name}-stderr.bin').write_bytes(r.stderr);receipts.append(dict(name=name,command=cmd,exit=r.returncode,bytes=(out/f'{name}.bin').stat().st_size));assert r.returncode==0,(name,r.returncode,r.stderr)
        outputs[name]=(out/f'{name}.bin').read_bytes();assert len(outputs[name])==65536*sum(c['repeat']for c in rows);print(name,'closed',len(outputs[name]),flush=True)
    expected=outputs['original'];checks=[];fail=[]
    for name,actual in outputs.items():
        if name=='original':continue
        offset=0
        for i,c in enumerate(rows):
            n=65536*c['repeat'];e=expected[offset:offset+n];v=actual[offset:offset+n];row=dict(index=i,compiler=name,passed=e==v,records=c['repeat'],original_sha256=sha(e),actual_sha256=sha(v));checks.append(row)
            if e!=v:
                j=next(k for k,(x,y)in enumerate(zip(e,v))if x!=y);failure=dict(**row,case=c,first_difference=j,original=e[max(0,j-16):j+16].hex(),actual=v[max(0,j-16):j+16].hex());fail.append(failure);(out/f'failure-{name}-{i}.json').write_text(json.dumps(failure,indent=2)+'\n')
            offset+=n
    after={x:sha(Path(x).read_bytes())for x in before};(out/'source-after.json').write_text(json.dumps(after,indent=2)+'\n');assert after==guards
    result=dict(passed=not fail,scenes=len(rows),comparisons=len(checks),records_per_run=sum(c['repeat']for c in rows),failures=fail,results=checks,cases=rows,fixtures_sha256=sha(inputs),original_sha256=HASH,original_members=[hex(OLD)],original=original,matching=matching,source_before=guards,source_after=after,source_stable=True,commands=commands,runtime_receipts=receipts,output_sha256={n:sha(b)for n,b in outputs.items()},generated_sha256={str(x.resolve()):sha(x.read_bytes())for x in out.iterdir()if x.suffix in('.c','.obj','.exe')},limitations=__doc__)
    (out/'verification.json').write_text(json.dumps(result,indent=2)+'\n');print('PASS'if not fail else'FAIL',len(rows),len(checks),len(fail));assert not fail
if __name__=='__main__':main()
