#!/usr/bin/env python3
"""Complete MIDI-clear leaf and genuine caller/lookup Win32 x86 proof.

Original 13B leaf, 78B clear caller and 124B lookup execute intact. All native
bodies are real C; linked mode requires exact public MAP identities/owner and
authenticates every reachable edge, mutable operand and active relocation.
No API boundary, entry patch, S3/game substitute or output normalization exists.
Full fixed 65536B arena, 28B state, MIDI-enable DWORD and raw EAX are compared
after calls. ESP, nonvolatile registers and x87 TOP are checked at masked PC53.
Fixtures are finite valid Win32 object/list graphs; arbitrary leaf ECX is safe
because original never dereferences it. No hardware, private-stack, concurrent,
faulting-input, unmasked-exception or whole-game equivalence claim.
"""
import argparse,json,random,re,struct,subprocess,sys
from pathlib import Path
R=next(p for p in Path(__file__).resolve().parents if (p/'src/carma2').is_dir());sys.path.insert(0,str(R/'scripts'))
import verify_phil_registry_reconstruction as registry
from verify_opponent_route_reconstruction import raw,pe,HASH,sha,words
from verify_tinted_native_reconstruction import extract
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM,CS_GRP_CALL,CS_GRP_JUMP
SOURCE=R/'src/carma2/s3/src/audio.c'
ENTRIES=[('nop0_FUN_00569f85',0x569f85,13),('S3ClearBufferOfMidiChannel',0x56a818,78),('S3GetChannelForTag',0x565888,124)]
GLOBALS=[('gS3_enable_midi',0x79feb8,4),('gS3_state',0x7a0580,28)]
FIELDS='mode enabled tag ptr outlet_count channel_count outlet_id channel_tag type target alias repeat seed poison'.split()
RECORD_BYTES=4+4+28+65536
COMMON=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include <s3/internal/audio.h>
static unsigned addresses[3],global_addresses[2];static unsigned char*arena;
#define gS3_enable_midi (*(int*)(uintptr_t)global_addresses[0])
#define gS3_state (*(tS3_state*)(uintptr_t)global_addresses[1])
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison,builder_cdecl;
extern unsigned __cdecl BuilderInvoke(void);
static int run(const char*path){FILE*f;unsigned h[14],j;
if(sizeof(void*)!=4||sizeof(tS3_state)!=28||sizeof(tS3_channel)!=120||offsetof(tS3_channel,tag)!=0x1c||offsetof(tS3_channel,type)!=0x20||offsetof(tS3_channel,descriptor)!=0x60||offsetof(tS3_channel,next)!=0x64||offsetof(tS3_descriptor,buffer_description)!=0x40||offsetof(tS3_outlet,next)!=0x18||offsetof(tS3_state,outlets)!=12)return 15;
SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);arena=VirtualAlloc((void*)0x30000000,65536,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(arena!=(void*)0x30000000)return 11;
f=fopen(path,"rb");if(!f)return 10;_setmode(_fileno(stdout),_O_BINARY);_controlfp(_PC_53,_MCW_PC);_clearfp();
while(fread(h,sizeof(h),1,f)==1){if(fread(arena,65536,1,f)!=1||fread(&gS3_state,28,1,f)!=1)return 12;gS3_enable_midi=(int)h[1];
for(j=0;j<h[11];j++){builder_address=addresses[h[0]];builder_x=h[0]==0?h[3]:h[2];builder_y=h[13]^0x21436587u;builder_mode=0;builder_poison=h[13];
#ifdef REC2_MATCHING
builder_cdecl=0;builder_stack_count=0;
#else
builder_cdecl=1;builder_stack_count=1;builder_stack[0]=builder_x;
#endif
if(BuilderInvoke())return 14;fwrite(&builder_result,4,1,stdout);fwrite(&gS3_enable_midi,4,1,stdout);fwrite(&gS3_state,28,1,stdout);fwrite(arena,65536,1,stdout);}}
if(!feof(f))return 16;fclose(f);return 0;}
'''
LOADER=r'''
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned char*disk,*image;unsigned n,i,v,h[6],*patch;IMAGE_NT_HEADERS*nt;IMAGE_SECTION_HEADER*s;
if(argc!=4)return 1;f=fopen(argv[2],"rb");if(!f)return 2;fseek(f,0,SEEK_END);n=(unsigned)ftell(f);rewind(f);disk=malloc(n);if(!disk||fread(disk,n,1,f)!=1)return 3;fclose(f);nt=(IMAGE_NT_HEADERS*)(disk+((IMAGE_DOS_HEADER*)disk)->e_lfanew);
image=VirtualAlloc(NULL,nt->OptionalHeader.SizeOfImage,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image)return 4;memcpy(image,disk,nt->OptionalHeader.SizeOfHeaders);s=IMAGE_FIRST_SECTION(nt);for(i=0;i<nt->FileHeader.NumberOfSections;i++)memcpy(image+s[i].VirtualAddress,disk+s[i].PointerToRawData,s[i].SizeOfRawData);
f=fopen(argv[3],"rb");if(!f||fread(h,sizeof(h),1,f)!=1)return 5;n=h[5];patch=malloc(n*8);if(n&&(!patch||fread(patch,n*8,1,f)!=1))return 6;fclose(f);
for(i=0;i<n;i++){if(patch[i*2]+4>nt->OptionalHeader.SizeOfImage)return 7;memcpy(&v,image+patch[i*2],4);if(v!=patch[i*2+1])return 8;v=(unsigned)image+v-nt->OptionalHeader.ImageBase;memcpy(image+patch[i*2],&v,4);}for(i=0;i<3;i++)addresses[i]=(unsigned)image+h[i];for(i=0;i<2;i++)global_addresses[i]=(unsigned)image+h[3+i];FlushInstructionCache(GetCurrentProcess(),image,nt->OptionalHeader.SizeOfImage);free(patch);free(disk);return run(argv[1]);}
'''

def packet(binary,entries,globals_,path,identity=None):
    nt,base,size,tab=pe(binary);cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
    functions={a:(name,n) for name,a,n in entries};patches={};all_ins={};graphs=[];edges=[]
    def section(a,n):
        rows=[]
        for k in range(struct.unpack_from('<H',binary,nt+6)[0]):
            vs,rva,rs,off=struct.unpack_from('<4I',binary,tab+40*k+8);flags=struct.unpack_from('<I',binary,tab+40*k+36)[0]
            if rva<=a-base and a-base+n<=rva+max(vs,rs):rows.append(flags)
        assert len(rows)==1,('section',hex(a),n);return rows[0]
    for name,g,n in globals_:
        flags=section(g,n);assert flags&0x80000000 and not flags&0x20000000,('Full writable global declaration span',name,hex(g),n)
    for name,start,n in entries:
        flags=section(start,n);assert flags&0x20000000 and not flags&0x80000000,('Full executable MAP span',name,hex(start),n)
        end=start+n;todo=[start];local={}
        while todo:
            a=todo.pop()
            if a in local:continue
            assert start<=a<end,(name,hex(a),'MAP extent')
            i=next(cs.disasm(raw(binary,a,min(15,end-a)),a),None);assert i is not None
            flags=section(a,i.size);assert flags&0x20000000 and not flags&0x80000000,('Complete executable instruction',name,hex(a),i.size)
            assert i.mnemonic not in ('int3','ud2','int','hlt');local[a]=i
            if i.mnemonic.startswith('ret'):continue
            if CS_GRP_CALL in i.groups or CS_GRP_JUMP in i.groups:
                op=i.operands[0];assert op.type==CS_OP_IMM,('No indirect boundary permitted',hex(a),i.op_str)
                target=op.imm;edges.append(dict(site=hex(a),target=hex(target),kind=i.mnemonic,owner_function=name))
                if CS_GRP_CALL not in i.groups and start<=target<end:todo.append(target)
                else:assert target in functions,(name,hex(a),'Unapproved full callee',hex(target))
                if i.mnemonic=='jmp':continue
            assert a+i.size<end,(name,hex(a),'unterminated span');todo.append(a+i.size)
        used=set()
        for a,i in local.items():
            slots=set(range(a,a+i.size));assert not used&slots;used.update(slots);all_ins[a]=i
            for op in i.operands:
                if op.type==CS_OP_MEM and base<=op.mem.disp<base+size:
                    assert not op.mem.base and not op.mem.index
                    assert any(g<=op.mem.disp and op.mem.disp+op.size<=g+w for _,g,w in globals_),('Unexpected global',hex(a),i.op_str)
                    assert section(op.mem.disp,op.size)&0x80000000 and not section(op.mem.disp,op.size)&0x20000000
                    assert i.disp_size==4;patches[a+i.disp_offset-base]=op.mem.disp
                elif op.type==CS_OP_IMM and not(CS_GRP_CALL in i.groups or CS_GRP_JUMP in i.groups)and base<=op.imm<base+size:
                    raise AssertionError(('Unexpected address literal',hex(a),i.op_str))
        graphs.append(dict(name=name,address=hex(start),extent=n,instructions=len(local),complete_body_sha256=sha(raw(binary,start,n)),reachable=[[hex(a),i.bytes.hex()] for a,i in sorted(local.items())],identity=identity.get(start) if identity else None))
    active={a-base+j for a,i in all_ins.items()for j in range(i.size)};rr,rz=struct.unpack_from('<II',binary,nt+24+96+5*8);cursor=rr;checked=[]
    while cursor<rr+rz:
        page,n=struct.unpack('<II',raw(binary,base+cursor,8));assert n>=8 and cursor+n<=rr+rz
        for v in struct.unpack('<'+'H'*((n-8)//2),raw(binary,base+cursor+8,n-8)):
            assert v>>12 in(0,3)
            if v>>12==3 and page+(v&4095)in active:
                at=page+(v&4095);assert at in patches;checked.append(at)
        cursor+=n
    assert cursor==rr+rz
    if not rz:assert struct.unpack_from('<H',binary,nt+22)[0]&1
    output=words([a-base for _,a,_ in entries]+[a-base for _,a,_ in globals_]+[len(patches)]+[v for at,value in sorted(patches.items())for v in(at,value)])
    path.write_bytes(output)
    return dict(functions=graphs,edges=edges,globals=[dict(name=n,address=hex(a),bytes=w)for n,a,w in globals_],absolute_operands=sorted(patches.items()),original_members=[hex(a)for _,a,_ in ENTRIES],indirect_edges=[],readonly=[],relocation_directory=[rr,rz],active_relocations_checked=checked,packet_sha256=sha(output),policy='Full unchanged image and complete reachable bodies; only authenticated absolute mutable operands rebased. No callee patches, boundaries or output normalization.')

def linked_packet(binary,symbol_bytes,map_bytes,path):
    s=json.loads(symbol_bytes);assert s['format']==1 and s['original_sha256']==HASH and s['recompiled_sha256']==sha(binary)
    mapping={r['orig']:r['recomp']for r in s['symbols']};rows={}
    for name,a,owner in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',map_bytes.decode('latin1'),re.M):rows.setdefault(int(a,16),[]).append((name,owner.strip()))
    entries=[];identities={}
    for name,old,_ in ENTRIES:
        assert old in mapping,('Missing required new/old root',name,hex(old));a=mapping[old];expected=('@'+name+'@4','s3:audio.c.obj')
        assert rows.get(a)==[expected],('Public MAP owner/identity',name,rows.get(a));end=min(v for v in rows if v>a);entries.append((name,a,end-a));identities[a]=dict(original=hex(old),actual=hex(a),symbol=expected[0],owner=expected[1])
    globals_=[]
    for name,old,n in GLOBALS:
        assert old in mapping,('Missing global',name);a=mapping[old]
        gr=[(int(address,16),owner.strip())for address,owner in re.findall(r'^\s*000[23]:[0-9a-fA-F]{8}\s+_'+re.escape(name)+r'\s+([0-9a-fA-F]{8})\s+(.+)$',map_bytes.decode('latin1'),re.M)]
        assert gr==[(a, '<common>')],('Global MAP identity/owner',name,gr);globals_.append((name,a,n))
    result=packet(binary,entries,globals_,path,identities);result.update(recompiled_sha256=sha(binary),symbols_sha256=sha(symbol_bytes),map_sha256=sha(map_bytes),required_public_identities=list(identities.values()),private_identities=[])
    return result

def cases():
    result=[]
    def add(**v):
        c=dict(mode=1,enabled=1,tag=0x307,ptr=0,outlet_count=3,channel_count=6,outlet_id=7,channel_tag=0x307,type=1,target=3,alias=0,repeat=1,seed=len(result)+1,poison=0xcccccccc);c.update(v);result.append(c)
    for ptr in [0,1,3,0xffffffff,0x80000000,0x7fffffff,0x30000000,0x30000001,0x30000200,0x3000ffff]:
        for poison in [0,0xffffffff,0xcccccccc,0x12345678]:add(mode=0,ptr=ptr,poison=poison,repeat=2)
    for mode in [1,2]:
        for tag in [0,1,2,7,255,0x107,0x307,0x607,0xffffffff,0x80000007,0x7fffffff]:
            for count in [0,1,2,3]:add(mode=mode,tag=tag,outlet_count=count)
        for n in range(7):
            for target in range(6):add(mode=mode,channel_count=n,target=target,tag=(target+1)*256+7,channel_tag=(target+1)*256+7)
    for enabled in [0,1,2,0xffffffff,0x80000000]:
        for typ in [0,1,2,0xffffffff,0x80000000]:
            for target in [0,2,5]:add(enabled=enabled,type=typ,target=target,tag=(target+1)*256+7,channel_tag=(target+1)*256+7,repeat=2)
    for alias in range(1,9):
        for target in [0,2,5]:add(alias=alias,target=target,tag=(target+1)*256+7,channel_tag=(target+1)*256+7,repeat=2)
    for tag in [0x107,0x307,0xffffffff,0x80000007]:
        for target in [0,2,5]:add(tag=tag,channel_tag=tag,target=target,outlet_id=tag&255,repeat=2)
    return result

def fixture(c):
    rng=random.Random(c['seed']);arena=bytearray(words([rng.getrandbits(32)for _ in range(65536//4)]));state=bytearray(words([rng.getrandbits(32)for _ in range(7)]));base=0x30000000
    def put(off,*vs):arena[off:off+len(vs)*4]=words(vs)
    for i in range(3):
        o=0x100+i*0x40;put(o,2 if i==0 else c['outlet_id'] if i==1 else 255);put(o+16,base+0x400 if i==1 and c['channel_count']else 0);put(o+24,base+o+0x40 if i+1<c['outlet_count']else 0)
    struct.pack_into('<I',state,12,base+0x100 if c['outlet_count']else 0)
    for i in range(6):
        ch=0x400+i*0x80;desc=0x2000+i*0x80;put(ch+0x1c,c['channel_tag']if i==c['target']else(i+1)*256+7);put(ch+0x20,c['type']);put(ch+0x60,base+desc);put(ch+0x64,base+ch+0x80 if i+1<c['channel_count']else 0)
        put(desc+0x40,base+0x6000+i*0x40)
    target=0x400+c['target']*0x80
    if c['alias']:
        # Buffer-description NULL store aliases a live public DWORD; all stores
        # preserve a finite list for subsequent repeated calls.
        slots=[0,target+0x1c,target+0x20,target+0x64,0x100,0x140,0x140+0x10,0x180,0x7000]
        put(target+0x60,base+slots[c['alias']]-0x40)
    return words([c[k]for k in FIELDS])+arena+state

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',required=True,type=Path);p.add_argument('--candidate',type=Path);p.add_argument('--original',type=Path,default=R/'build-reference/originals/CARMA2_HW.EXE');p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);p.add_argument('--fixtures-dir',type=Path);a=p.parse_args();assert bool(a.symbols)==bool(a.recompiled)
    out=a.output.resolve();assert not out.exists();out.mkdir(parents=True);candidate=(a.candidate or SOURCE).resolve()
    paths=[candidate,SOURCE,Path(__file__),a.original.resolve(),R/'scripts/verify_phil_registry_reconstruction.py',R/'scripts/verify_opponent_navigation_reconstruction.py',R/'scripts/verify_opponent_route_reconstruction.py',R/'scripts/verify_tinted_native_reconstruction.py']
    paths+=list((R/'src/carma2/s3/include').rglob('*.h'))+list((R/'src/carma2/include').rglob('*.h'))+list((R/'src/carma2/hooks').rglob('*.h'))+list((R/'src/brender/include').rglob('*.h'))
    if a.symbols:paths +=[a.recompiled.resolve(),a.recompiled.resolve().with_suffix('.map'),a.symbols.resolve()]
    before={str(x.resolve()):x.read_bytes()for x in paths};guards={x:sha(b)for x,b in before.items()};(out/'source-before.json').write_text(json.dumps(guards,indent=2)+'\n')
    binary=before[str(a.original.resolve())];assert sha(binary)==HASH;original=packet(binary,ENTRIES,GLOBALS,out/'original-packet.bin')
    original_leaf=raw(binary,0x569f85,13);assert original_leaf.hex()=='558bec51894dfc33c08be55dc3';assert original['functions'][0]['instructions']==8
    native=COMMON+'\n'+ '\n\n'.join(extract(before[str(candidate)].decode(),name)for name,_,_ in ENTRIES)
    native+='\nint C2_HOOK_CDECL main(int argc,char**argv){unsigned char*g;if(argc!=2)return 1;g=VirtualAlloc((void*)0x31000000,4096,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(g!=(void*)0x31000000)return 17;global_addresses[0]=(unsigned)g;global_addresses[1]=(unsigned)g+16;'+''.join(f'addresses[{i}]=(unsigned){name};'for i,(name,_,_)in enumerate(ENTRIES))+'return run(argv[1]);}\n'
    caller,_,_=registry.literals();caller=caller.replace('builder_poison;','builder_poison,builder_cdecl;').replace(' mov builder_result,eax\n mov ecx,esp',' mov builder_result,eax\n cmp builder_cdecl,0\n je midi_stack\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n midi_stack:\n mov ecx,esp');assert 'midi_stack:'in caller
    registry.COMMON=COMMON;commands=registry.compile_sources(out,native,LOADER,caller);rows=cases();inputs=b''.join(fixture(c)for c in rows);(out/'cases.bin').write_bytes(inputs)
    if a.fixtures_dir:assert (a.fixtures_dir/'cases.bin').read_bytes()==inputs
    matching=linked_packet(before[str(a.recompiled.resolve())],before[str(a.symbols.resolve())],before[str(a.recompiled.resolve().with_suffix('.map'))],out/'matching-packet.bin')if a.symbols else None
    outputs={};receipts=[]
    for name in ['original','msvc5','msvc','mingw']+(['matching']if a.symbols else []):
        cmd=[str(out/'physical.exe'),str(out/'cases.bin'),str(a.original.resolve()if name=='original'else a.recompiled.resolve()),str(out/('original-packet.bin'if name=='original'else'matching-packet.bin'))]if name in('original','matching')else[str(out/(name+'.exe')),str(out/'cases.bin')]
        with(out/f'{name}.bin').open('wb')as f:r=subprocess.run(cmd,stdout=f,stderr=subprocess.PIPE,timeout=60)
        (out/f'{name}-stderr.bin').write_bytes(r.stderr);receipts.append(dict(name=name,command=cmd,exit=r.returncode,bytes=(out/f'{name}.bin').stat().st_size));assert r.returncode==0,(name,r.returncode,r.stderr)
        outputs[name]=(out/f'{name}.bin').read_bytes();assert len(outputs[name])==RECORD_BYTES*sum(c['repeat']for c in rows)
        print(name,'closed',len(outputs[name]),flush=True)
    expected=outputs['original'];checks=[];fail=[]
    for name,actual in outputs.items():
        if name=='original':continue
        offset=0
        for i,c in enumerate(rows):
            n=RECORD_BYTES*c['repeat'];e=expected[offset:offset+n];v=actual[offset:offset+n];row=dict(index=i,compiler=name,passed=e==v,records=c['repeat'],original_sha256=sha(e),actual_sha256=sha(v));checks.append(row)
            if e!=v:
                j=next(k for k,(x,y)in enumerate(zip(e,v))if x!=y);failure=dict(**row,case=c,first_difference=j,original=e[max(0,j-16):j+16].hex(),actual=v[max(0,j-16):j+16].hex());fail.append(failure);(out/f'failure-{name}-{i}.json').write_text(json.dumps(failure,indent=2)+'\n')
            offset+=n
    after={x:sha(Path(x).read_bytes())for x in before};(out/'source-after.json').write_text(json.dumps(after,indent=2)+'\n');assert after==guards
    verification=dict(passed=not fail,scenes=len(rows),comparisons=len(checks),records_per_run=sum(c['repeat']for c in rows),failures=fail,results=checks,cases=rows,fixtures_sha256=sha(inputs),original_sha256=HASH,original_members=[hex(a)for _,a,_ in ENTRIES],original=original,matching=matching,source_before=guards,source_after=after,source_stable=True,commands=commands,runtime_receipts=receipts,output_sha256={n:sha(b)for n,b in outputs.items()},generated_sha256={str(x.resolve()):sha(x.read_bytes())for x in out.iterdir()if x.suffix in('.c','.obj','.exe')},limitations=__doc__)
    (out/'verification.json').write_text(json.dumps(verification,indent=2)+'\n');print('PASS'if not fail else'FAIL',len(rows),len(checks),len(fail));assert not fail
if __name__=='__main__':main()
