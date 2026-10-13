"""Complete SetLowGravityPeds39B/ResetLowGravityPeds11B original/native3/linked proof.

Actual full call-free roots execute unchanged. Set observes signed return and
raw public DWORD state; Reset stores raw literal0x3f800000 and excludes incidental void EAX. All mapped arena
bytes, ESP/nonvolatiles/x87TOP compare at masked PC53. No float arithmetic,
game/math/API substitute, numeric normalization, private-stack, concurrent,
unmasked, exhaustive or whole-game claim. Unaligned/effective-type aliases and
integer-converted wrapped addresses are bounded Win32 x86 fixtures, not a
portable ISO C pointer-difference claim. Terminal AV cases compare the first
fault address and public prefix only; no normal ABI credit for those cases.
"""
import argparse,json,random,re,struct,subprocess,sys
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM,CS_GRP_CALL,CS_GRP_JUMP
R=next(p for p in Path(__file__).resolve().parents if(p/'src/carma2').is_dir());sys.path.insert(0,str(R/'scripts'))
from verify_opponent_route_reconstruction import raw,pe,HASH,sha,words
from verify_tinted_native_reconstruction import extract
import verify_phil_registry_reconstruction as registry
SOURCE=R/'src/carma2/newcommon/27-powerup.c';BASE=0x24000000;ARENA=4096
SPEC=[('SetLowGravityPeds',0x4de8d0,39),('ResetLowGravityPeds',0x4de980,11)]
GLOBALS=[('gPed_gravity_multiplier',0x7447dc,0xe00),('gPowerup_array',0x6a0a54,0xe08)]
COMMON=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "27-powerup.h"
static unsigned char*arena;static unsigned addresses[2];
#define gPed_gravity_multiplier (*(float*)(arena+0xe00))
#define gPowerup_array (*(tPowerup**)(arena+0xe08))
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison,builder_cdecl;
extern unsigned __cdecl BuilderInvoke(void);
typedef char powerup_layout[sizeof(void*)==4&&sizeof(tPowerup)==172&&offsetof(tPowerup,float_params)==72?1:-1];
static void snapshot(unsigned kind,unsigned value,unsigned address){unsigned h[3];h[0]=kind;h[1]=value;h[2]=address;fwrite(h,12,1,stdout);fwrite(arena,4096,1,stdout);fflush(stdout);}
static LONG WINAPI observe_fault(EXCEPTION_POINTERS*e){if(e->ExceptionRecord->ExceptionCode!=EXCEPTION_ACCESS_VIOLATION)return EXCEPTION_CONTINUE_SEARCH;snapshot(3,e->ExceptionRecord->ExceptionCode,(unsigned)e->ExceptionRecord->ExceptionInformation[1]);ExitProcess(0);return EXCEPTION_CONTINUE_SEARCH;}
static int run(const char*path){FILE*f;unsigned h[5];FARPROC proc;typedef void*(WINAPI *add_handler)(ULONG,LONG(WINAPI*)(EXCEPTION_POINTERS*));add_handler handler;
if(sizeof(void*)!=4||sizeof(tPowerup)!=172||offsetof(tPowerup,float_params)!=72)return 15;SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);arena=VirtualAlloc((void*)0x24000000,4096,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(arena!=(void*)0x24000000)return 11;f=fopen(path,"rb");if(!f)return 10;_setmode(_fileno(stdout),_O_BINARY);
proc=GetProcAddress(GetModuleHandleA("kernel32.dll"),"AddVectoredExceptionHandler");memcpy(&handler,&proc,4);if(!handler||!handler(1,observe_fault))return 13;
while(fread(h,sizeof(h),1,f)==1){if(fread(arena,4096,1,f)!=1)return 12;if(h[0]>=2)return 16;_controlfp(_PC_53,_MCW_PC);_clearfp();builder_address=addresses[h[0]];builder_mode=h[0];builder_poison=h[4];builder_x=h[1];builder_y=h[2];
#ifdef REC2_MATCHING
builder_cdecl=0;builder_stack_count=0;
#else
builder_cdecl=1;builder_stack_count=2;builder_stack[0]=h[1];builder_stack[1]=h[2];
#endif
if(BuilderInvoke())return 14;snapshot(0,builder_result,0);}
if(!feof(f))return 17;fclose(f);return 0;}
'''
PHYSICAL=r'''
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned h[3],i;unsigned char*image;
if(argc!=3)return 1;f=fopen(argv[2],"rb");if(!f)return 2;if(fread(h,sizeof(h),1,f)!=1)return 3;image=VirtualAlloc(NULL,h[0],MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image||fread(image,h[0],1,f)!=1)return 4;fclose(f);FlushInstructionCache(GetCurrentProcess(),image,h[0]);for(i=0;i<2;i++){if(h[i+1]>=h[0])return 6;addresses[i]=(unsigned)image+h[i+1];}return run(argv[1]);}
'''

def packet(disk,symbols=None,mapbytes=None):
    original=symbols is None;nt,base,size,tab=pe(disk)
    def section(a,n):
        rows=[]
        for k in range(struct.unpack_from('<H',disk,nt+6)[0]):
            vs,rva,rs,off=struct.unpack_from('<4I',disk,tab+40*k+8);flags=struct.unpack_from('<I',disk,tab+40*k+36)[0]
            if base+rva<=a and a+n<=base+rva+max(vs,rs):rows.append(flags)
        assert len(rows)==1,('complete section span',hex(a),n);return rows[0]
    if original:roots=[a for _,a,_ in SPEC];extents={a:n for _,a,n in SPEC};globals_={a:(n,off)for n,a,off in GLOBALS};identities={a:(n,'original')for n,a,_ in SPEC}
    else:
        s=json.loads(symbols);assert s['format']==1 and s['original_sha256']==HASH and s['recompiled_sha256']==sha(disk);mapped={r['orig']:r['recomp']for r in s['symbols']};roots=[mapped[a]for _,a,_ in SPEC];rows={}
        for name,a,owner in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',mapbytes.decode('latin1'),re.M):rows.setdefault(int(a,16),[]).append((name,owner.strip()))
        identities={};extents={}
        for root,(name,_,_)in zip(roots,SPEC):
            assert rows.get(root)==[('@'+name+'@8','27-powerup.c.obj')],('strict root MAP identity',hex(root),rows.get(root));identities[root]=rows[root][0];extents[root]=min(a for a in rows if a>root)-root
        globals_={}
        for name,old,off in GLOBALS:
            gr=[(int(a,16),owner.strip())for a,owner in re.findall(r'^\s*0003:[0-9a-fA-F]{8}\s+_'+name+r'\s+([0-9a-fA-F]{8})\s+([^\r\n]+)\r?$',mapbytes.decode('latin1'),re.M)];assert len(gr)==1 and gr[0][1]=='<common>'and mapped[old]==gr[0][0],(name,gr);globals_[mapped[old]]=(name,off)
    for a in globals_:
        f=section(a,4);assert f&0x80000000 and f&0x40000000 and not f&0x20000000
    cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;bodies=[];patches={};active={}
    for root,(name,old,n)in zip(roots,SPEC):
        extent=extents[root];f=section(root,extent);assert f&0x20000000 and not f&0x80000000;end=root+extent;at=root;ins=[]
        while at<end:
            i=next(cs.disasm(raw(disk,at,min(15,end-at)),at,count=1));assert at+i.size<=end;f=section(at,i.size);assert f&0x20000000 and not f&0x80000000;assert CS_GRP_CALL not in i.groups and CS_GRP_JUMP not in i.groups and i.mnemonic not in('int3','ud2','int','hlt');ins.append(i)
            for q in range(at,at+i.size):assert q not in active;active[q]=at
            for op in i.operands:
                if op.type==CS_OP_MEM and not op.mem.base:
                    assert not op.mem.segment and not op.mem.index and op.mem.disp in globals_ and op.size==4 and i.disp_size==4;gn,off=globals_[op.mem.disp];patches[at+i.disp_offset]=dict(site=at+i.disp_offset,original_value=op.mem.disp,target=BASE+off,width=4,authority=gn,instruction=i.address,instruction_hex=i.bytes.hex())
                elif op.type==CS_OP_IMM:assert not base<=op.imm<base+size,('unexpected address immediate',hex(at),i.op_str)
            at+=i.size
            if i.mnemonic.startswith('ret'):assert i.mnemonic=='ret'and not i.operands;break
        else:raise AssertionError('missing full RET')
        if original:assert at==end
        bodies.append(dict(name=name,original=hex(old),address=root,extent=extent,reachable_bytes=at-root,instruction_count=len(ins),full_span_sha256=sha(raw(disk,root,extent)),identity=identities[root],instructions=[dict(address=i.address,hex=i.bytes.hex(),mnemonic=i.mnemonic,operands=i.op_str)for i in ins],callees=[],edges=[]))
    rr,rz=struct.unpack_from('<II',disk,nt+24+96+5*8);cursor=0;relocs=set()
    while cursor<rz:
        page,n=struct.unpack('<II',raw(disk,base+rr+cursor,8));assert n>=8 and n%2==0 and cursor+n<=rz
        for v in struct.unpack('<'+'H'*((n-8)//2),raw(disk,base+rr+cursor+8,n-8)):
            if not v>>12:continue
            assert v>>12==3;site=base+page+(v&4095);assert site not in relocs;relocs.add(site)
            if any(site+k in active for k in range(4)):assert site in patches,('unclassified active relocation',hex(site))
        cursor+=n
    assert cursor==rz
    if rz:assert set(patches)<=relocs
    else:assert struct.unpack_from('<H',disk,nt+22)[0]&1
    low=min(active);high=max(active)+1;image=bytearray(b'\xcc'*(high-low))
    for row in bodies:
        for i in row['instructions']:bits=bytes.fromhex(i['hex']);image[i['address']-low:i['address']-low+len(bits)]=bits
    for site,row in patches.items():struct.pack_into('<I',image,site-low,row['target'])
    return words([len(image),*[a-low for a in roots]])+image,dict(graph=bodies,patches=list(patches.values()),globals=[dict(name=n,address=a,bytes=4,target=BASE+off,owner='original'if original else'<common>')for a,(n,off)in globals_.items()],readonly=[],calls=[],indirect_edges=[],tables=[],relocation_directory=[rr,rz],active_relocations=sorted(set(patches)&relocs),original_members=[hex(a)for _,a,_ in SPEC],recompiled_sha256=None if original else sha(disk),symbols_sha256=None if original else sha(symbols),map_sha256=None if original else sha(mapbytes))

def cases():
    rows=[];rng=random.Random(0x4dcda0)
    def add(mode=0,label='basic',powerup=BASE+0x100,param=BASE+0x300,array=None,delta=0,value=0x3f800000,gravity=0x7f800001,default=0x7fc12345,car=0,poison=0xcccccccc,fault=False):
        arena=bytearray(words([0xa5a5a5a5]*1024));array=((powerup-delta)&0xffffffff)if array is None else array
        def put(a,v):struct.pack_into('<I',arena,a,v&0xffffffff)
        put(0xe00,gravity);put(0xe04,default);put(0xe08,array)
        if BASE<=powerup+72 and powerup+76<=BASE+ARENA:put(powerup-BASE+72,param)
        if BASE<=param and param+4<=BASE+ARENA and param-BASE not in(0xe00,0xe04,0xe08):put(param-BASE,value)
        if label=='parameter-overlaps-object-pointer-slot':
            # Preserve the intended mapped source pointer while reading the
            # overlapping bytes as a real raw DWORD. No host heap addresses.
            put(powerup-BASE+72,param)
        payload=words([mode,powerup,car,0,poison])+arena;rows.append((dict(index=len(rows),name=SPEC[mode][0],mode=mode,label=label,powerup=powerup,param=param,array=array&0xffffffff,delta=((powerup-array+0x80000000)&0xffffffff)-0x80000000,value=value,gravity=gravity,default=default,car=car,poison=poison,fault=fault),payload))
    special=[0,0x80000000,1,0x80000001,0x007fffff,0x00800000,0x3f800000,0xbf800000,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc12345,0xffc12345,0x7f812345,0xff812345,0xffffffff,0x12345678]
    deltas=[0,1,-1,171,-171,172,-172,173,-173,343,-343,344,-344,345,-345,0x7fffffff,-0x80000000,0x7fffffff-171,-0x80000000+171]
    for bits in special:
        for delta in deltas:add(label='raw-IEEE-and-signed-quotient',value=bits,delta=delta)
    for _ in range(128):add(label='full-signed-delta-random',delta=rng.randrange(-0x80000000,0x80000000),value=rng.getrandbits(32),car=rng.getrandbits(32),poison=rng.getrandbits(32))
    for k in[-12485370,-1000000,-100,-2,-1,0,1,2,100,1000000,12485370]:
        for edge in[-1,0,1]:add(label='division-boundary',delta=k*172+edge)
    for off in(1,2,3):
        for bits in special:add(label='unaligned-object-and-source',powerup=BASE+0x100+off,param=BASE+0x300+off,value=bits,delta=-173)
    for source in(0xe00,0xe04,0xe08):
        for bits in special:add(label='parameter-source-global-alias',param=BASE+source,gravity=bits,default=bits,array=bits)
    for field in(0xe00,0xe04,0xe08):
        for bits in special:add(label='powerup-field-global-alias',powerup=BASE+field-72,param=BASE+0x300,value=bits,array=BASE+0x300 if field==0xe08 else BASE+0x100)
    for delta in range(-7,8):add(label='parameter-overlaps-object-pointer-slot',param=BASE+0x100+72+delta,value=BASE+0x300,fault=True)
    for bits in special:
        for ignored in(0,1,0xffffffff,0x80000000):add(mode=1,label='reset-raw-payload-ignored-arguments',powerup=ignored,car=ignored^0x12345678,default=bits,gravity=~bits&0xffffffff,array=bits)
    for pointer in(0,0xffffffff,0xfffffff8,BASE+ARENA-72,BASE+ARENA-76,BASE+ARENA-75):add(label='powerup-first-fault',powerup=pointer,fault=True)
    for pointer in(0,0xffffffff,0xfffffff8,BASE+ARENA-3,BASE+ARENA-4):add(label='parameter-first-fault',param=pointer,fault=True)
    return rows

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',required=True,type=Path);p.add_argument('--candidate',type=Path);p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);p.add_argument('--fixtures-dir',type=Path);a=p.parse_args();assert bool(a.recompiled)==bool(a.symbols);out=a.output.resolve();assert not out.exists();out.mkdir(parents=True);candidate=(a.candidate or SOURCE).resolve()
    paths=[Path(__file__),candidate,SOURCE,SOURCE.with_suffix('.h'),SOURCE.parent/'61-pedestrn.c',SOURCE.parent/'61-pedestrn.h',R/'build-reference/originals/CARMA2_HW.EXE']+[R/'scripts'/n for n in('verify_phil_registry_reconstruction.py','verify_opponent_navigation_reconstruction.py','verify_opponent_route_reconstruction.py','verify_tinted_native_reconstruction.py')]+list((R/'src/carma2/include').rglob('*.h'))+list((R/'src/carma2/hooks').rglob('*.h'))+list((R/'src/brender/include').rglob('*.h'))
    if a.recompiled:paths.extend([a.recompiled.resolve(),a.recompiled.resolve().with_suffix('.map'),a.symbols.resolve()])
    before={str(f.resolve()):sha(f.read_bytes())for f in paths};(out/'source-before.json').write_text(json.dumps(before,indent=2)+'\n');(out/'verifier-snapshot.py').write_bytes(Path(__file__).read_bytes());(out/'candidate-snapshot.c').write_bytes(candidate.read_bytes())
    try:
        binary=(R/'build-reference/originals/CARMA2_HW.EXE').read_bytes();assert sha(binary)==HASH;blob,original=packet(binary);(out/'original-code.bin').write_bytes(blob);matching=None
        if a.recompiled:blob,matching=packet(a.recompiled.read_bytes(),a.symbols.read_bytes(),a.recompiled.with_suffix('.map').read_bytes());(out/'matching-code.bin').write_bytes(blob)
        text=candidate.read_text();type_start=text.index('#if defined(__GNUC__)');type_end=text.index('#endif',type_start)+len('#endif');body=text[type_start:type_end]+'\n'+'\n'.join(extract(text,n)for n,_,_ in SPEC)
        native=COMMON+'\n'+body+'\nint C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;addresses[0]=(unsigned)SetLowGravityPeds;addresses[1]=(unsigned)ResetLowGravityPeds;return run(argv[1]);}\n'
        caller,_,_=registry.literals();caller=caller.replace('builder_poison;','builder_poison,builder_cdecl;').replace(' mov builder_result,eax\n mov ecx,esp',' mov builder_result,eax\n cmp builder_cdecl,0\n je gravity_stack\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n gravity_stack:\n mov ecx,esp');assert 'gravity_stack:'in caller;registry.COMMON=COMMON;commands=registry.compile_sources(out,native,PHYSICAL,caller);rows=cases();inputs=b''.join(v for _,v in rows);(out/'fixtures.bin').write_bytes(inputs);(out/'fixture-metadata.json').write_text(json.dumps([c for c,_ in rows],indent=2)+'\n')
        if a.fixtures_dir:assert inputs==(a.fixtures_dir/'fixtures.bin').read_bytes()
        normal=[r for r in rows if not r[0]['fault']];fault=[r for r in rows if r[0]['fault']];(out/'normal.bin').write_bytes(b''.join(v for _,v in normal));names=['original','msvc5','msvc','mingw']+(['matching']if matching else[]);outputs={};receipts=[];size=ARENA+12
        def command(name,path):return[str(out/'physical.exe'),str(path),str(out/('original-code.bin'if name=='original'else'matching-code.bin'))]if name in('original','matching')else[str(out/(name+'.exe')),str(path)]
        for name in names:
            cmd=command(name,out/'normal.bin')
            with(out/f'{name}-normal.bin').open('wb')as f:r=subprocess.run(cmd,stdout=f,stderr=subprocess.PIPE,timeout=60)
            (out/f'{name}-normal-stderr.bin').write_bytes(r.stderr);receipts.append(dict(name=name,phase='normal',command=cmd,exit=r.returncode,bytes=(out/f'{name}-normal.bin').stat().st_size));assert r.returncode==0 and(out/f'{name}-normal.bin').stat().st_size==len(normal)*size,(name,r.returncode,r.stderr)
            data=(out/f'{name}-normal.bin').read_bytes();outputs[name]={c['index']:data[k*size:(k+1)*size]for k,(c,_)in enumerate(normal)};print(name,'normal closed',len(normal),flush=True)
        for c,payload in fault:
            (out/'fault-case.bin').write_bytes(payload)
            for name in names:
                cmd=command(name,out/'fault-case.bin');r=subprocess.run(cmd,capture_output=True,timeout=20);assert r.returncode==0 and len(r.stdout)==size,(name,c['index'],r.returncode,len(r.stdout),r.stderr);outputs[name][c['index']]=r.stdout;receipts.append(dict(name=name,phase='fault',index=c['index'],command=cmd,exit=r.returncode,bytes=len(r.stdout),stderr=r.stderr.hex()))
        checks=[];fail=[]
        for name in names:
            (out/f'{name}-outputs.bin').write_bytes(b''.join(outputs[name][c['index']]for c,_ in rows))
            if name=='original':continue
            for c,_ in rows:
                e=outputs['original'][c['index']];v=outputs[name][c['index']];kind,value,address=struct.unpack_from('<3I',e);row=dict(index=c['index'],name=c['name'],compiler=name,passed=e==v,original_sha256=sha(e),actual_sha256=sha(v),original_kind=kind,actual_kind=struct.unpack_from('<I',v)[0],normal_return_abi_checked=kind==0);checks.append(row)
                if e!=v:
                    prefix=out/f'failure-{name}-{c["index"]}';prefix.with_suffix('.original.bin').write_bytes(e);prefix.with_suffix('.actual.bin').write_bytes(v);j=next(i for i,(x,y)in enumerate(zip(e,v))if x!=y);f=dict(**row,case=c,first_difference=j,expected=e[max(0,j-16):j+16].hex(),actual=v[max(0,j-16):j+16].hex());prefix.with_suffix('.json').write_text(json.dumps(f,indent=2)+'\n');fail.append(f)
        after={f:sha(Path(f).read_bytes())for f in before};assert after==before;classification=[dict(index=c['index'],kind=struct.unpack_from('<I',outputs['original'][c['index']])[0],value=struct.unpack_from('<I',outputs['original'][c['index']],4)[0],label=c['label'])for c,_ in rows]
        report=dict(passed=not fail,scenes=len(rows),comparisons=len(checks),normal_scenes=sum(c['kind']==0 for c in classification),fault_scenes=sum(c['kind']==3 for c in classification),complete_suite=True,original=original,matching=matching,original_members=[hex(old)for _,old,_ in SPEC],source_before=before,source_after=after,source_stable=True,classification=classification,results=checks,failures=fail,commands=commands,runtime_receipts=receipts,input_sha256=sha(inputs),output_sha256={name:sha((out/f'{name}-outputs.bin').read_bytes())for name in names},limitations=__doc__,layout=dict(pointer_bytes=4,powerup_bytes=172,float_params_offset=72),live_handles=[])
        (out/'verification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS'if not fail else'FAIL',len(rows),len(checks),len(fail),flush=True);assert not fail
    finally:
        after={f:sha(Path(f).read_bytes())for f in before};(out/'guard-closure.json').write_text(json.dumps(dict(source_before=before,source_after=after,source_stable=before==after),indent=2)+'\n')
if __name__=='__main__':main()
