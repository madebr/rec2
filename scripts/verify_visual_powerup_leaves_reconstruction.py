"""Complete six visual powerup handlers173B original/native3 proof.
Raw32KiB public arenas, maskedPC53 and DF0 entry environment (outgoingDF not explicitly observed), authentic two-register FASTCALL RET0 and compiler two-word CDECL RET0 callers; raw setterEAX checked, void resetEAX excluded. Normal ESP/nonvolatile/x87TOP checks; terminal AV compares
first fault address and public prefix only. No callback/control/math substitute. Electro executes genuine parameter-square arithmetic. Fixed64KiB reservation, only32KiB committed.
"""
import argparse, json, random, struct, subprocess, sys
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_OP_IMM, CS_OP_MEM, CS_GRP_CALL, CS_GRP_JUMP
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'src/carma2').is_dir())
D=Path(__file__).resolve().parent
sys.path.insert(0,str(ROOT/'scripts'))
from verify_opponent_route_reconstruction import raw,HASH,sha,words
from verify_tinted_native_reconstruction import extract
import verify_phil_registry_reconstruction as registry
BASE=0x26000000
ARENA=32768
ENTRIES=[('ShowPedestrians',0x4dec10,48),('HidePedestrians',0x4dec40,21),('SetElectroBastard',0x4dec60,43),('ResetElectroBastard',0x4dec90,11),('SetMutantCorpses',0x4deca0,39),('ResetMutantCorpses',0x4decd0,11)]
GLOBAL_OFFSETS={'gPowerup_array':0x7000,'gShow_peds_on_map':0x7200,'gMutant_speed':0x7100}
IEEE=[0,0x80000000,1,0x80000001,0x007fffff,0x00800000,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7f800001,0xff800002,0x7fc12345,0xffc54321,0x7fffffff,0xffffffff]

def fixtures():
 cases=[];rng=random.Random(0x4dec60)
 def add(mode,label,powerup=BASE+0x1000,car=BASE+0x3000,array=BASE+0x1000,params=BASE+0x2000,value=0x40000000,driver=8,patches=(),**extra):
  memory=bytearray(ARENA)
  def put(a,v):
   assert 0<=a<=ARENA-4
   struct.pack_into('<I',memory,a,v&0xffffffff)
  for a in range(0,ARENA,256):put(a,a*73+len(cases))
  for name,a in GLOBAL_OFFSETS.items():put(a,array if name=='gPowerup_array'else 0xa5a5a5a5)
  for address,v in(((powerup+72)&0xffffffff,params),(params,value),((car+12)&0xffffffff,driver)):
   if BASE<=address<=BASE+ARENA-4:put(address-BASE,v)
  for a,v in patches:put(a,v)
  header=struct.pack('<7I',mode,powerup&0xffffffff,car&0xffffffff,0,0,0,0x7f800001 if len(cases)%2 else 0xcccccccc)
  case=dict(index=len(cases),mode=mode,name=ENTRIES[mode][0],label=label,powerup=powerup&0xffffffff,car=car&0xffffffff,array_argument=array&0xffffffff,actual_array=struct.unpack_from('<I',memory,GLOBAL_OFFSETS['gPowerup_array'])[0],parameters=params&0xffffffff,raw_value=value,driver=driver,patches=list(patches),**extra)
  cases.append((case,header+memory))
 deltas=sorted({0,1,-1,171,-171,172,-172,173,-173,343,-343,344,-344,345,-345,0x7fffffff,-0x80000000,-0x7fffffff,*[q*172+r for q in(-12485370,-10000,-2,-1,0,1,2,10000,12485370)for r in(-171,-1,0,1,171)if -0x80000000<=q*172+r<=0x7fffffff]})
 for mode,(name,_,_)in enumerate(ENTRIES):
  add(mode,'whole-entry')
  add(mode,'exact-unaligned-inputs',powerup=BASE+0x1101,car=BASE+0x3103,params=BASE+0x2103)
  add(mode,'null-car-gate-unused-or-fault',car=0)
  if mode%2==0:
   for delta in deltas:add(mode,'wrapped-signed-index',array=(BASE+0x1000-delta)&0xffffffff)
  if mode in(0,1):
   for driver in(0,1,7,8,9,0xffffffff,0x80000000):add(mode,'raw-driver-gate',driver=driver)
   for car in(0,1,0xfffffff8,BASE+ARENA-12,BASE+ARENA-15):add(mode,'driver-read-boundary',car=car)
   for off in GLOBAL_OFFSETS.values():add(mode,'driver-global-alias',car=BASE+off-12,patches=[(off,8)])
   for powerup in(0,1,0xffffffff,0x80000000):add(mode,'undereferenced-powerup',powerup=powerup)
   for state in(0,1,2,0xffffffff,0x80000000,0x7fffffff):add(mode,'map-global-raw-initial-state',patches=[(GLOBAL_OFFSETS['gShow_peds_on_map'],state)])
  if mode in(2,4):
   for value in IEEE:
    add(mode,'IEEE-raw-or-real-square',value=value)
    add(mode,'unaligned-IEEE-parameter',value=value,powerup=BASE+0x1101,params=BASE+0x2103)
   for off in GLOBAL_OFFSETS.values():
    add(mode,'parameter-pointer-global-alias',powerup=BASE+off-72)
    add(mode,'parameter-source-global-alias',params=BASE+off,value=BASE+0x1000)
   for powerup in(0,0xfffffff0,BASE+ARENA-72,BASE+ARENA-75):add(mode,'parameter-pointer-read-fault',powerup=powerup)
   for params in(0,0xfffffff8,BASE+ARENA,BASE+ARENA-1,BASE+ARENA-3):add(mode,'parameter-word-read-fault',params=params)
  if mode in(2,3):
   for off in GLOBAL_OFFSETS.values():
    for shift in(-3,0,3):add(mode,'output-partial-global-alias',car=BASE+off-0x170c+shift,output_offset=off+shift)
   for car in(0,0xfffff000,BASE+ARENA-0x170c,BASE+ARENA-0x170c-1):
    add(mode,'output-store-boundary',car=car)
    if mode==2:add(mode,'competing-source-output-fault',car=car,params=0)
  if mode==5:
   for p in(0,1,0xffffffff,0x80000000):add(mode,'both-register-words-unused',powerup=p,car=p)
 # Electro captures its single pointer/value before storing and must not reload.
 add(2,'square-output-replaces-parameter-pointer-no-reload',powerup=BASE+0x3000+0x170c-72)
 add(2,'square-output-overlaps-input-value',params=BASE+0x470c)
 add(2,'square-output-overlaps-input-pointer-slot',car=BASE+0x1048-0x170c)
 for exponent in(1,2,51,52,53,62,63,64,65,66,126,127,128,189,190,191,192,193,252,253,254):
  for mantissa in(0,1,0x3fffff,0x400000,0x7ffffe,0x7fffff):
   for sign in(0,0x80000000):add(2,'square-finite-exponent-rounding',value=sign|(exponent<<23)|mantissa)
 for i in range(144):
  mode=i%6;delta=rng.randrange(-0x80000000,0x80000000)
  add(mode,'bounded-random-complete-entry',array=(BASE+0x1000-delta)&0xffffffff,value=rng.getrandbits(32),driver=8 if i%3==0 else rng.getrandbits(32))
 return cases

COMMON='\n#include <windows.h>\n#include <stdio.h>\n#include <stdlib.h>\n#include <string.h>\n#include <stdint.h>\n#include <math.h>\n#include <float.h>\n#include <io.h>\n#include <fcntl.h>\n#include "27-powerup.h"\n#include "61-pedestrn.h"\n#include "globvars.h"\n#include "brender/br_defs.h"\nstatic unsigned char*arena;static unsigned addresses[6];static unsigned physical_mode;\nextern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison,builder_cdecl;\nextern unsigned __cdecl BuilderInvoke(void);\n#define gPowerup_array (*(tPowerup**)(arena+0x7000))\n#define gShow_peds_on_map (*(int*)(arena+0x7200))\n#define gMutant_speed (*(float*)(arena+0x7100))\ntypedef char layouts[sizeof(void*)==4&&sizeof(tPowerup)==172&&offsetof(tPowerup,float_params)==0x48&&offsetof(tCar_spec,driver)==0xc&&offsetof(tCar_spec,proxy_ray_distance)==0x170c?1:-1];\nstatic void snapshot(unsigned event,unsigned value,unsigned address){unsigned h[3];h[0]=event;h[1]=value;h[2]=address;fwrite(h,12,1,stdout);fwrite(arena,32768,1,stdout);fflush(stdout);}\nstatic LONG WINAPI observe_fault(EXCEPTION_POINTERS*e){if(e->ExceptionRecord->ExceptionCode!=EXCEPTION_ACCESS_VIOLATION)return EXCEPTION_CONTINUE_SEARCH;snapshot(3,e->ExceptionRecord->ExceptionCode,(unsigned)e->ExceptionRecord->ExceptionInformation[1]);ExitProcess(0);return EXCEPTION_CONTINUE_SEARCH;}\nstatic int run(const char*path){FILE*f;unsigned h[7],mode;FARPROC proc;typedef void*(WINAPI *add_handler)(ULONG,LONG(WINAPI*)(EXCEPTION_POINTERS*));add_handler handler;\nSetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);f=fopen(path,"rb");if(!f)return 10;arena=VirtualAlloc((void*)0x26000000,65536,MEM_RESERVE,PAGE_READWRITE);if(arena!=(void*)0x26000000)return 11;if(VirtualAlloc(arena,32768,MEM_COMMIT,PAGE_READWRITE)!=arena)return 11;\nif(fread(h,sizeof(h),1,f)!=1||fread(arena,32768,1,f)!=1)return 12;fclose(f);_setmode(_fileno(stdout),_O_BINARY);\nproc=GetProcAddress(GetModuleHandleA("kernel32.dll"),"AddVectoredExceptionHandler");memcpy(&handler,&proc,4);if(!handler||!handler(1,observe_fault))return 13;mode=h[0];if(mode>5)return 15;\nbuilder_address=addresses[mode];builder_mode=0;builder_poison=h[6];builder_x=h[1];builder_y=h[2];\n#if defined(REC2_MATCHING)\nbuilder_cdecl=0;\n#else\nbuilder_cdecl=1;\n#endif\nif(physical_mode)builder_cdecl=0;\nbuilder_stack_count=0;\nif(builder_cdecl){builder_stack_count=2;builder_stack[0]=h[1];builder_stack[1]=h[2];}\n_controlfp(_PC_53,_MCW_PC);_clearfp();if(BuilderInvoke())return 14;snapshot(0,mode%2?0:builder_result,0);return 0;}\n'
PHYSICAL='\nint C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned h[4],offsets[6],i,j,offset,value;unsigned char*image;\nif(argc!=3)return 1;f=fopen(argv[2],"rb");if(!f)return 2;if(fread(h,sizeof(h),1,f)!=1||h[1]!=6||fread(offsets,sizeof(offsets),1,f)!=1)return 3;\nfor(j=0;j<6;j++)if(offsets[j]>=h[0])return 3;\nimage=VirtualAlloc(NULL,h[0],MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image||fread(image,h[0],1,f)!=1)return 4;\nfor(i=0;i<h[2];i++){if(fread(&offset,4,1,f)!=1||offset>h[0]-4)return 5;memcpy(&value,image+offset,4);value=(unsigned)image+value-h[3];memcpy(image+offset,&value,4);}fclose(f);\nFlushInstructionCache(GetCurrentProcess(),image,h[0]);for(j=0;j<6;j++)addresses[j]=(unsigned)image+offsets[j];physical_mode=1;return run(argv[1]);}\n'
GLOBALS={0x6a0a54:(0x7000,4),0x74d09c:(0x7200,4),0x7447a4:(0x7100,4)}
SPECS=[('gPowerup_array',0x6a0a54,0x7000,4,'27-powerup.c','tPowerup*'),('gShow_peds_on_map',0x74d09c,0x7200,4,'globvars.c','int'),('gMutant_speed',0x7447a4,0x7100,4,'61-pedestrn.c','float')]
FROZEN_INPUT_SHA256='ce8d4d98af86eaca93e194a3587fc5653d74f7487746b968fb816538a860c8d5'

def original_image(binary):
    cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
    low=min(a for _,a,_ in ENTRIES);high=max(a+n for _,a,n in ENTRIES)
    image=bytearray(b'\xcc'*(high-low));bodies=[];patches=[]
    for name,a,n in ENTRIES:
        code=raw(binary,a,n);image[a-low:a-low+n]=code
        ins=list(cs.disasm(code,a));assert sum(i.size for i in ins)==n
        starts={i.address for i in ins}
        for i in ins:
            assert CS_GRP_CALL not in i.groups
            if CS_GRP_JUMP in i.groups:
                assert len(i.operands)==1 and i.operands[0].type==CS_OP_IMM and i.operands[0].imm in starts
            if i.mnemonic.startswith('ret'):assert i.mnemonic=='ret'and not i.operands
            for op in i.operands:
                if op.type==CS_OP_MEM and not op.mem.base:
                    assert not op.mem.index and not op.mem.segment and op.size==4 and i.disp_size==4
                    assert op.mem.disp in GLOBALS,(name,hex(op.mem.disp))
                    target=BASE+GLOBALS[op.mem.disp][0]
                    struct.pack_into('<I',image,i.address+i.disp_offset-low,target)
                    patches.append(dict(site=i.address,offset=i.disp_offset,instruction=i.bytes.hex(),original=op.mem.disp,target=target,width=4,kind='named scalar global'))
        assert ins[-1].address+ins[-1].size==a+n and ins[-1].mnemonic=='ret'
        bodies.append(dict(name=name,address=a,bytes=n,sha256=sha(code),reachable_bytes=n,
                           instructions=[dict(address=i.address,bytes=i.bytes.hex(),instruction=i.mnemonic+' '+i.op_str)for i in ins],
                           calls=[],returns=[dict(site=ins[-1].address,bytes=0)]))
    assert len(bodies)==6 and sum(b['bytes']for b in bodies)==173 and len(patches)==7
    return words([len(image),6,0,low,*[a-low for _,a,_ in ENTRIES]])+image,dict(bodies=bodies,total_original_bytes=173,low=low,high=high,patches=patches,globals=GLOBALS,relocations=[],constants=[],calls=[],indirect_calls=[],tables=[])

def source_capsule(candidate=None,source_dir=None):
    import re
    directory=source_dir or ROOT/'src/carma2/newcommon'
    source=(candidate if candidate else directory/'27-powerup.c').read_text()
    if candidate is None:
        for name,address,arena_at,n,owner,typ in SPECS:
            owner_path=directory/owner
            if not owner_path.exists():
                assert owner=='globvars.c'
                owner_path=ROOT/'src/carma2/newcommon'/owner
            owner_source=owner_path.read_text();header=owner_path.with_suffix('.h').read_text()
            declaration=re.escape(typ)+r'\s+'+re.escape(name)+r'\s*;'
            assert re.search(r'// GLOBAL: CARMA2_HW '+f'{address:#010x}'+r'\s+'+declaration,owner_source),(name,owner)
            assert re.search(r'extern\s+'+declaration,header),(name,owner)
        header=(directory/'27-powerup.h').read_text()
        for mode,(name,address,_)in enumerate(ENTRIES):
            typ='int'if mode%2==0 else'void'
            assert re.search(r'// FUNCTION: CARMA2_HW '+f'{address:#010x}'+r'\s+'+typ+r' C2_HOOK_FASTCALL '+name+r'\(',source)
            assert re.search(r'(?:extern\s+)?'+typ+r' C2_HOOK_FASTCALL '+name+r'\(tPowerup\* powerup, tCar_spec\* car\);',header)
    alias='#if defined(__GNUC__)\ntypedef tU32 tPedPowerupWord __attribute__((__may_alias__));\n#else\ntypedef tU32 tPedPowerupWord;\n#endif\n'
    assert alias.rstrip('\n')in source
    return alias+'\n'.join(extract(source,n)for n in('PedPowerupIndex',*[name for name,_,_ in ENTRIES]))

def matching_image(exe, symbols):
    """Exact whole-CFG transport; no indirect calls, imports, tables or substitutes."""
    from verify_opponent_route_reconstruction import pe
    from capstone import CS_AC_READ, CS_AC_WRITE
    disk=exe.read_bytes();mapbytes=exe.with_suffix('.map').read_bytes();mapping=json.loads(symbols.read_text())
    assert mapping['format']==1
    assert mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(disk)
    mapped={}
    for row in mapping['symbols']:
        if row['orig']in mapped:assert mapped[row['orig']]==row['recomp']
        mapped[row['orig']]=row['recomp']
    funcs={};data={}
    for line in mapbytes.decode('latin1').splitlines():
        values=line.split()
        if len(values)>=4 and len(values[0].split(':'))==2:
            try:address=int(values[2],16)
            except ValueError:continue
            if 'f'in values[3:-1]:funcs.setdefault(address,set()).add((values[1],values[-1]))
            else:data.setdefault(values[1],set()).add((address,values[-1]))
    owners={name:('27-powerup.c.obj','@'+name+'@8')for name,_,_ in ENTRIES};owners['PedPowerupIndex']=('27-powerup.c.obj','_PedPowerupIndex')
    def identity(address):
        assert address in funcs
        aliases=funcs[address];assert len(aliases)==1,('Ambiguous reached MAP identity',hex(address),sorted(aliases))
        decorated,owner=next(iter(aliases));name=decorated.lstrip('_@').split('@')[0]
        assert name in owners and (owner,decorated)==owners[name],('Unexpected complete callee',hex(address),decorated,owner)
        return name,owner,decorated
    roots=[mapped[a]for _,a,_ in ENTRIES]
    for root,(name,_,_)in zip(roots,ENTRIES):assert identity(root)[0]==name
    globals_={}
    specs=[row[:5]for row in SPECS]
    for name,orig,arena_at,n,source_owner in specs:
        entries=data.get('_'+name,set());assert len(entries)==1,(name,entries)
        address,owner=next(iter(entries));assert owner=='<common>'and mapped[orig]==address,(name,owner,address,mapped[orig])
        globals_[address]=dict(name=name,original=orig,address=address,bytes=n,target=BASE+arena_at,owner=owner,source_owner=source_owner)
    assert all(a['address']+a['bytes']<=b['address']or b['address']+b['bytes']<=a['address']for i,a in enumerate(globals_.values())for b in list(globals_.values())[i+1:])
    nt,base,image_size,tab=pe(disk);sections=[]
    for index in range(struct.unpack_from('<H',disk,nt+6)[0]):
        at=tab+index*40;virtual_size,rva,raw_size,file=struct.unpack_from('<IIII',disk,at+8)
        sections.append(dict(start=base+rva,end=base+rva+max(virtual_size,raw_size),flags=struct.unpack_from('<I',disk,at+36)[0],name=disk[at:at+8].split(b'\0')[0].decode()))
    def section(address,n):
        rows=[s for s in sections if s['start']<=address and address+n<=s['end']]
        assert len(rows)==1,('PE section span',hex(address),n)
        return rows[0]
    for address,row in globals_.items():
        s=section(address,row['bytes']);assert s['flags']&0x80000000 and s['flags']&0x40000000 and not s['flags']&0x20000000
    starts=sorted(funcs);todo=list(roots);bodies={};active={};patches={};constants={};edges=[]
    cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
    def transport(site,old,width,kind,ins):
        matches=[row for row in globals_.values()if row['address']<=old and old+width<=row['address']+row['bytes']]

        if matches:
            assert len(matches)==1
            global_=matches[0];target=global_['target']+old-global_['address']
            authority=global_['name'];mode='arena'
        else:
            raise AssertionError(('Unclassified absolute/image data operand',hex(site),hex(old),width,kind))
        row=dict(site=site,original_value=old,target=target,mode=mode,width=width,authority=authority,instruction_address=ins.address,instruction_hex=ins.bytes.hex())
        if site in patches:assert patches[site]==row
        patches[site]=row
    while todo:
        ins_root=todo.pop()
        if ins_root in bodies:continue
        name,owner,decorated=identity(ins_root);end=next(a for a in starts if a>ins_root)
        s=section(ins_root,end-ins_root);assert s['flags']&0x20 and s['flags']&0x40000000 and s['flags']&0x20000000 and not s['flags']&0x80000000,('Whole MAP code span',name,hex(ins_root),hex(end))
        pending=[ins_root];decoded={};successors={};calls=[]
        while pending:
            at=pending.pop()
            if at in decoded:continue
            assert ins_root<=at<end
            ins=next(cs.disasm(raw(disk,at,min(15,end-at)),at,count=1));assert at+ins.size<=end
            s=section(at,ins.size);assert s['flags']&0x20 and s['flags']&0x40000000 and s['flags']&0x20000000 and not s['flags']&0x80000000
            assert ins.mnemonic not in('int','int1','int3','into','ud2','hlt','cli','sti','retf','lret','syscall','sysenter','sysexit','iret','iretd'),('Unexpected trap/privileged control',hex(at),ins.mnemonic)
            for address in range(at,at+ins.size):assert address not in active;active[address]=at
            decoded[at]=ins
            for op in ins.operands:
                if op.type==CS_OP_MEM and not op.mem.base:
                    assert not op.mem.segment,('Unclassified explicit segment',hex(at),ins.op_str)
                    assert not op.mem.index,('Unexpected absolute indexed operand',hex(at),ins.op_str)
                    assert base<=op.mem.disp<base+image_size,('Unclassified absolute memory',hex(at),ins.op_str)
                    assert ins.disp_size==4;transport(at+ins.disp_offset,op.mem.disp,op.size,'memory',ins)
                if op.type==CS_OP_IMM and not(CS_GRP_CALL in ins.groups or CS_GRP_JUMP in ins.groups)and base<=op.imm<base+image_size:
                    assert ins.mnemonic in('mov','push')and ins.imm_size==4
                    transport(at+ins.imm_offset,op.imm,1,'immediate',ins)
            nexts=[]
            if CS_GRP_CALL in ins.groups:
                assert ins.mnemonic=='call'and len(ins.operands)==1 and ins.operands[0].type==CS_OP_IMM and ins.imm_size==4
                assert not any(prefix in(0x26,0x2e,0x36,0x3e,0x64,0x65,0x66,0x67)for prefix in ins.prefix if prefix)
                target=ins.operands[0].imm;identity(target);todo.append(target);calls.append(target);edges.append(dict(site=at,target=target,kind='call'));nexts=[at+ins.size]
            elif CS_GRP_JUMP in ins.groups:
                assert ins.operands[0].type==CS_OP_IMM
                target=ins.operands[0].imm
                if ins_root<=target<end:nexts.append(target)
                else:
                    assert ins.mnemonic=='jmp';identity(target);todo.append(target);edges.append(dict(site=at,target=target,kind='tail-jump'))
                if ins.mnemonic!='jmp':nexts.append(at+ins.size)
            elif ins.mnemonic.startswith('ret'):
                assert ins.mnemonic=='ret'
                expected=0
                assert (ins.operands[0].imm if ins.operands else 0)==expected,(name,ins.op_str,expected)
            else:nexts=[at+ins.size]
            successors[at]=nexts;pending.extend(nexts)
        assert all(target in decoded for nexts in successors.values()for target in nexts)
        bodies[ins_root]=dict(name=name,owner=owner,decorated=decorated,address=ins_root,end=end,instruction_count=len(decoded),instructions=[dict(address=i.address,hex=i.bytes.hex(),mnemonic=i.mnemonic,operands=i.op_str)for i in sorted(decoded.values(),key=lambda x:x.address)],direct_calls=sorted(set(calls)),successors=successors)
    reloc_rva,reloc_size=struct.unpack_from('<II',disk,nt+24+96+5*8);coff=struct.unpack_from('<H',disk,nt+22)[0];relocations=set()
    if reloc_rva or reloc_size:
        assert reloc_rva and reloc_size;cursor=0
        while cursor<reloc_size:
            assert cursor+8<=reloc_size
            page,n=struct.unpack('<II',raw(disk,base+reloc_rva+cursor,8));assert n>=8 and n%2==0 and cursor+n<=reloc_size
            for entry in struct.unpack('<'+'H'*((n-8)//2),raw(disk,base+reloc_rva+cursor+8,n-8)):
                kind=entry>>12
                if not kind:continue
                assert kind==3;site=base+page+(entry&4095);assert site+4<=base+image_size and site not in relocations;relocations.add(site)
                if any(site+k in active for k in range(4)):assert site in patches,('Unclassified active HIGHLOW',hex(site))
            cursor+=n
        assert cursor==reloc_size
        assert all(site in relocations for site in patches),('Expected operand missing HIGHLOW',set(patches)-relocations)
    else:assert coff&1
    low=min(active);high=max([max(active)+1]+[at+n for at,n in constants]);image=bytearray(b'\xcc'*(high-low));image_refs=[]
    for row in bodies.values():
        for ins in row['instructions']:
            blob=bytes.fromhex(ins['hex']);image[ins['address']-low:ins['address']-low+len(blob)]=blob
    for (at,n),value in constants.items():image[at-low:at-low+n]=value
    for site,row in patches.items():
        if row['mode']=='arena':struct.pack_into('<I',image,site-low,row['target'])
        else:image_refs.append(site-low)
    assert len(roots)==6
    blob=words([len(image),6,len(image_refs),low,*[root-low for root in roots]])+image+words(image_refs)
    meta=dict(recompiled_sha256=sha(disk),symbols_sha256=sha(symbols.read_bytes()),map_sha256=sha(mapbytes),roots=roots,original_roots=[hex(a)for _,a,_ in ENTRIES],graph=list(bodies.values()),edges=edges,patches=list(patches.values()),constants=[dict(address=at,bytes=n,raw=value.hex())for (at,n),value in constants.items()],globals=list(globals_.values()),low=low,high=high,relocation_directory=[reloc_rva,reloc_size],coff_characteristics=coff,indirect_calls=[],tables=[],map_aliases=[dict(address=a,aliases=sorted(rows),reachable=a in bodies)for a,rows in funcs.items()if len(rows)>1])
    return blob,meta

def project_dependencies():
    # Conservative union of literal project includes across actual three compiler
    # branches. Never guards unrelated project headers or changes source.
    import re
    inc=[ROOT/p for p in('src/msvc500/include','src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/brender/include','src/carma2/s3/include','src/carma2/platform')]
    todo=[ROOT/'src/carma2/newcommon/27-powerup.h',ROOT/'src/carma2/newcommon/61-pedestrn.h',ROOT/'src/carma2/newcommon/globvars.h',ROOT/'src/brender/include/brender/br_defs.h',ROOT/'src/msvc500/include/stdint.h'];seen=set()
    while todo:
        path=todo.pop().resolve()
        if path in seen:continue
        seen.add(path)
        for name in re.findall(r'^\s*#\s*include\s*[<"]([^>"]+)',path.read_text(),re.M):
            found=[p/name for p in [path.parent,*inc]if(p/name).is_file()]
            if found:todo.append(found[0])
    return sorted(seen)


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True);p.add_argument('--candidate',type=Path);p.add_argument('--source-dir',type=Path);p.add_argument('--package-only',action='store_true');p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE');p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);p.add_argument('--case-index',type=int,action='append');p.add_argument('--limit',type=int);a=p.parse_args()
    assert bool(a.recompiled)==bool(a.symbols);assert not(a.candidate and a.source_dir);assert not(a.recompiled and(a.candidate or a.source_dir));assert not a.source_dir or a.package_only
    out=a.output.resolve();assert not out.exists();out.mkdir(parents=True)
    relevant=([a.candidate]if a.candidate else[])+[Path(__file__),ROOT/'src/carma2/newcommon/27-powerup.c',ROOT/'src/carma2/newcommon/61-pedestrn.c',ROOT/'src/carma2/newcommon/globvars.c',ROOT/'src/carma2/common/powerups.c',ROOT/'src/carma2/common/pedestrn.c',a.original]+[ROOT/'scripts'/s for s in ('verify_phil_registry_reconstruction.py','verify_opponent_navigation_reconstruction.py','verify_opponent_route_reconstruction.py','verify_tinted_native_reconstruction.py')]+project_dependencies()
    if a.source_dir:relevant +=[a.source_dir/n for n in('27-powerup.c','27-powerup.h','61-pedestrn.c','61-pedestrn.h')]
    if a.recompiled:relevant +=[a.recompiled,a.symbols,a.recompiled.with_suffix('.map')]
    before={str(f.resolve()):sha(f.read_bytes())for f in relevant};(out/'source-before.json').write_text(json.dumps(before,indent=2)+'\n');(out/'verifier-snapshot.py').write_bytes(Path(__file__).read_bytes())
    if a.candidate:(out/'candidate-snapshot.c').write_bytes(a.candidate.read_bytes())
    snapshots={};(out/'sources').mkdir()
    for index,f in enumerate(dict.fromkeys(relevant)):
        if f.suffix in ('.c','.h','.py'):
            dest=out/'sources'/f'{index}-{f.name}';dest.write_bytes(f.read_bytes());snapshots[str(f.resolve())]=str(dest.resolve())
    (out/'source-snapshots.json').write_text(json.dumps(snapshots,indent=2)+'\n')
    try:
        binary=a.original.read_bytes();assert sha(binary)==HASH;packet,graph=original_image(binary);(out/'original-code.bin').write_bytes(packet)
        body=source_capsule(a.candidate,a.source_dir)
        native=COMMON+'\n'+body+'\nint C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;'+''.join('addresses[%d]=(unsigned)%s;'%(i,name)for i,(name,_,_)in enumerate(ENTRIES))+'physical_mode=0;return run(argv[1]);}\n'
        caller,_,_=registry.literals();caller=caller.replace('builder_poison;','builder_poison,builder_cdecl;');old=' mov builder_result,eax\n mov ecx,esp';assert old in caller
        caller=caller.replace(old,' mov builder_result,eax\n cmp builder_cdecl,0\n je cachematerial_ready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n cachematerial_ready:\n mov ecx,esp')
        matching=None
        if a.recompiled:
            matching_packet,matching=matching_image(a.recompiled,a.symbols);(out/'matching-code.bin').write_bytes(matching_packet)
        cases=fixtures();assert len(cases)==709 and sha(b''.join(payload for _,payload in cases))==FROZEN_INPUT_SHA256
        if a.package_only:
            (out/'native.c').write_text(native);(out/'physical.c').write_text(COMMON+PHYSICAL);(out/'caller.c').write_text(caller)
            after={f:sha(Path(f).read_bytes())for f in before};assert before==after
            package=dict(passed=True,package_only=True,scenes=709,complete_suite=False,runtime_comparisons=0,matching=matching,original=graph,source_before=before,source_after=after,fixture_sha256=[sha(payload)for _,payload in cases],compiler_input_sha256={n:sha((out/n).read_bytes())for n in('native.c','physical.c','caller.c','original-code.bin')},limits=__doc__,live_handles=[])
            (out/'package.json').write_text(json.dumps(package,indent=2)+'\n');print('PACKAGE ONLY PASS',len(cases));return
        registry.COMMON=COMMON;commands=registry.compile_sources(out,native,PHYSICAL,caller)
        indices=a.case_index or list(range(len(cases)));indices=indices[:a.limit]if a.limit else indices
        (out/'fixtures.bin').write_bytes(b''.join(cases[i][1]for i in indices));(out/'fixture-metadata.json').write_text(json.dumps([cases[i][0]for i in indices],indent=2)+'\n')
        results=[];failures=[];classification=[]
        output_files={c:(out/(c+'-outputs.bin')).open('wb')for c in ('original','msvc5','msvc','mingw')+(('matching',)if matching else())}
        try:
            for index in indices:
                case,payload=cases[index];f=out/'case.bin';f.write_bytes(payload)
                r=subprocess.run([str(out/'physical.exe'),str(f),str(out/'original-code.bin')],capture_output=True,timeout=20)
                assert r.returncode==0 and len(r.stdout)==ARENA+12,('original',index,r.returncode,len(r.stdout),r.stderr)
                expected=r.stdout;output_files['original'].write(expected);kind,value,address=struct.unpack_from('<3I',expected);classification.append(dict(index=index,kind=kind,value=value,address=address))
                for compiler in ('msvc5','msvc','mingw')+(('matching',)if matching else()):
                    command=[str(out/'physical.exe'),str(f),str(out/'matching-code.bin')]if compiler=='matching'else[str(out/(compiler+'.exe')),str(f)]
                    r=subprocess.run(command,capture_output=True,timeout=20);output_files[compiler].write(r.stdout);same=r.returncode==0 and r.stdout==expected
                    row=dict(index=index,compiler=compiler,passed=same,input_sha256=sha(payload),original_output_sha256=sha(expected),actual_output_sha256=sha(r.stdout),normal_return_abi_checked=kind==0,raw_eax_checked=kind==0 and case['mode']%2==0,exit_code=r.returncode)
                    results.append(row)
                    if not same:
                        prefix=out/f'failure-{compiler}-{index}';prefix.with_suffix('.original.bin').write_bytes(expected);prefix.with_suffix('.actual.bin').write_bytes(r.stdout);prefix.with_suffix('.json').write_text(json.dumps(dict(case=case,row=row,stderr=r.stderr.hex()),indent=2)+'\n');failures.append(row)
                if len(classification)%100==0:print('checked',len(classification),len(results),'failures',len(failures),flush=True)
        finally:
            for f in output_files.values():f.close()
        after={f:sha(Path(f).read_bytes())for f in before};assert before==after
        report=dict(passed=not failures,scenes=len(indices),comparisons=len(results),total_scenes=len(cases),complete_suite=indices==list(range(len(cases))),source_stable=True,matching=matching,recompiled_sha256=matching['recompiled_sha256']if matching else None,symbols_sha256=matching['symbols_sha256']if matching else None,source_before=before,source_after=after,original_sha256=HASH,graph=graph,fixtures_sha256=sha((out/'fixtures.bin').read_bytes()),fixture_sha256=[sha(cases[i][1])for i in indices],classification=classification,normal=sum(r['kind']==0 for r in classification),actual_av=sum(r['kind']==3 for r in classification),results=results,failures=failures,commands=commands,limits=__doc__,live_handles=[],artifacts={str(f.resolve()):sha(f.read_bytes())for f in out.iterdir()if f.is_file()})
        (out/'verification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS'if not failures else'FAIL',len(indices),len(results),len(failures),flush=True)
        if failures:raise SystemExit(1)
    finally:
        after={f:sha(Path(f).read_bytes())for f in before};(out/'guard-closure.json').write_text(json.dumps(dict(source_before=before,source_after=after,source_stable=before==after),indent=2)+'\n')
if __name__=='__main__':main()
