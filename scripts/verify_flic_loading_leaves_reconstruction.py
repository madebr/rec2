"""Eleven complete FLIC/loading leaves:122B/30instructions, genuine noargs RET0.
Original/native3 execute real bodies; raw9global state and each sequence step are
captured. Native startup reads real compiled int declarations; original startup
reads authenticated original PE initial words, ignoring fixture overrides.
Production default authenticates nine owning declarations, including Gore2 and
pending -1. Matching mode authenticates all nine actual PE initial words and
strong initialized MAP owners separately from tentative common globals.
Candidate override is a diagnostic mode, not production acceptance.
Normal ESP/nonvolatile/TOP checked, DF0 entry only, voidEAX excluded. No FP/calls/
callbacks/helpers/import replacements exist in the original game closure.
"""
import argparse,json,re,struct,subprocess,sys,random
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_MEM,CS_GRP_CALL,CS_GRP_JUMP
ROOT=next(p for p in Path(__file__).resolve().parents if(p/'src/carma2').is_dir())
D=Path(__file__).resolve().parent
sys.path.insert(0,str(ROOT/'scripts'))
from verify_opponent_route_reconstruction import raw,pe,HASH,sha,words
from verify_tinted_native_reconstruction import extract
import verify_phil_registry_reconstruction as registry
ENTRIES=[('PlayFlicsFromDisk', 4594176, 11), ('PlayFlicsFromMemory', 4594192, 11), ('TurnOffPanelFlics', 4594208, 11), ('TurnOnPanelFlics', 4594224, 11), ('SuspendPendingFlic', 4601600, 21), ('ResumePendingFlic', 4601632, 11), ('GetHowMuchBloodAndSnotToSmearAbout', 5074880, 14), ('GetAnimalsOn', 5074896, 6), ('IsItOkayToFireHorribleBallsOfNastyNapalmDeathAtPerfectlyInnocentPassersByAndByInnocentIDoMeanInTheBiblicalSense', 5074912, 6), ('IsItReallyOKThatWeDontMakeAnyEffortToProtectAnySadFuckersOutThereThatDontWishToSeeInnocentPeopleBlownToBitsByHighExplosiveMinesAndShells', 5074928, 6), ('GetDronesOn', 4517120, 14)]
SPECS=[('gPending_flic', 5858504, 256, 4294967295, '38-flicplay.c'), ('gPending_pending_flic', 6840844, 512, 0, '38-flicplay.c'), ('gPanel_flic_disable', 6841108, 776, 0, '38-flicplay.c'), ('gPlay_from_disk', 6841112, 780, 0, '38-flicplay.c'), ('gAnimalsOn', 6674376, 1536, 1, '61-pedestrn.c'), ('gFlameThrowerOn', 6674380, 1540, 1, '61-pedestrn.c'), ('gExplosives_on', 6674384, 1544, 1, '61-pedestrn.c'), ('gGoreLevel', 6674388, 1548, 2, '61-pedestrn.c'), ('gTraffic_disabled', 6833432, 2048, 0, '57-drone.c')]
GLOBALS={address:(offset,4)for _,address,offset,_,_ in SPECS}
BASE=0x28000000
ARENA=4096
VALUES=(0, 1, 2, 3, 2147483647, 2147483648, 2147483649, 4294967293, 4294967294, 4294967295, 2863311530, 1431655765, 3735928559, 16909060, 2139095040, 2143363909, 4286578689)

def fixtures():
    cases=[];rng=random.Random(0x463700)
    def add(label,modes,values=None,startup=False):
        assert 0<len(modes)<=16 and all(0<=m<11 for m in modes)
        memory=bytearray(ARENA)
        for offset in range(0,ARENA,128):struct.pack_into('<I',memory,offset,(offset*73+len(cases))&0xffffffff)
        if values is None:values=[row[3]for row in SPECS]
        assert len(values)==9
        # Startup rows intentionally contain contrary words. The harness must
        # initialize from actual declarations/PE metadata, never hide it by input.
        fixture_words=[value^0xffffffff for value in values]if startup else values
        for row,value in zip(SPECS,fixture_words):struct.pack_into('<I',memory,row[2],value&0xffffffff)
        header=struct.pack('<6I',int(startup),len(modes),len(cases),rng.getrandbits(32),rng.getrandbits(32),rng.getrandbits(32))
        commands=struct.pack('<16I',*modes,*([0]*(16-len(modes))))
        case=dict(index=len(cases),label=label,modes=list(modes),startup_from_declarations_or_PE=startup,
                  fixture_global_words=[word&0xffffffff for word in fixture_words],
                  intended_initial_words=[row[3]for row in SPECS]if startup else[value&0xffffffff for value in values],
                  expected_events=1+len(modes),normal_calls=len(modes),startup_events=1)
        cases.append((case,header+commands+memory))
    for mode in range(11):add('genuine-startup-no-fixture-global-overrides',[mode],startup=True)
    for mode in range(11):
        for index in range(len(VALUES)):
            add('all-nine-independent-raw-word-cycle',[mode],[VALUES[(index+slot)%len(VALUES)]for slot in range(9)])
    for pending in VALUES:
        for saved in VALUES:
            values=[row[3]for row in SPECS];values[0]=pending;values[1]=saved
            add('distinct-pending-pair-roundtrip-and-double-suspend',[4,5,4,4,5,5],values)
    for index in range(len(VALUES)):
        values=[VALUES[(index+slot)%len(VALUES)]for slot in range(9)]
        add('resume-before-suspend-is-live-saved-word',[5,4,5],values)
        add('all-eleven-forward-sequence',list(range(11)),values)
        add('all-eleven-reversed-sequence',list(reversed(range(11))),values)
    for index in range(132):
        modes=[index%11,*[rng.randrange(11)for _ in range(2+(index%4))]]
        add('bounded-random-raw-state-sequence',modes,[rng.getrandbits(32)for _ in range(9)])
    return cases


COMMON=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "c2_hooks.h"
#include "rec2_types.h"
static unsigned char*arena;static unsigned addresses[11],physical_mode,physical_initial[9],step,current_mode;
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison;
extern unsigned __cdecl BuilderInvoke(void);
typedef char native_word_layout[sizeof(int)==4&&sizeof(tU32)==4&&sizeof(tS32)==4&&sizeof(void*)==4?1:-1];
/* Genuine prospective owning int declarations. This is not canonical Gore1. */
int gPending_flic=-1;
int gPending_pending_flic;
int gPanel_flic_disable;
int gPlay_from_disk;
int gAnimalsOn=1;
int gFlameThrowerOn=1;
int gExplosives_on=1;
int gGoreLevel=2;
int gTraffic_disabled;
static const unsigned global_offsets[9]={0x100,0x200,0x308,0x30c,0x600,0x604,0x608,0x60c,0x800};
static void native_initial_words(unsigned*words){
words[0]=(unsigned)*(volatile int*)&gPending_flic;
words[1]=(unsigned)*(volatile int*)&gPending_pending_flic;
words[2]=(unsigned)*(volatile int*)&gPanel_flic_disable;
words[3]=(unsigned)*(volatile int*)&gPlay_from_disk;
words[4]=(unsigned)*(volatile int*)&gAnimalsOn;
words[5]=(unsigned)*(volatile int*)&gFlameThrowerOn;
words[6]=(unsigned)*(volatile int*)&gExplosives_on;
words[7]=(unsigned)*(volatile int*)&gGoreLevel;
words[8]=(unsigned)*(volatile int*)&gTraffic_disabled;
}
#define gPending_flic (*(int*)(arena+0x100))
#define gPending_pending_flic (*(int*)(arena+0x200))
#define gPanel_flic_disable (*(int*)(arena+0x308))
#define gPlay_from_disk (*(int*)(arena+0x30c))
#define gAnimalsOn (*(int*)(arena+0x600))
#define gFlameThrowerOn (*(int*)(arena+0x604))
#define gExplosives_on (*(int*)(arena+0x608))
#define gGoreLevel (*(int*)(arena+0x60c))
#define gTraffic_disabled (*(int*)(arena+0x800))
static void snapshot(unsigned kind,unsigned mode,unsigned value){unsigned h[4];h[0]=kind;h[1]=step;h[2]=mode;h[3]=value;fwrite(h,16,1,stdout);fwrite(arena,4096,1,stdout);fflush(stdout);}
static int run(const char*path){FILE*f;unsigned h[6],modes[16],initial[9],i,mode;
SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);f=fopen(path,"rb");if(!f)return 10;
arena=VirtualAlloc((void*)0x28000000,65536,MEM_RESERVE,PAGE_READWRITE);if(arena!=(void*)0x28000000)return 11;if(VirtualAlloc(arena,4096,MEM_COMMIT,PAGE_READWRITE)!=arena)return 11;
if(fread(h,sizeof(h),1,f)!=1||fread(modes,sizeof(modes),1,f)!=1||fread(arena,4096,1,f)!=1)return 12;fclose(f);_setmode(_fileno(stdout),_O_BINARY);if(h[0]>1||h[1]==0||h[1]>16)return 13;
if(h[0]){if(physical_mode)memcpy(initial,physical_initial,sizeof(initial));else native_initial_words(initial);for(i=0;i<9;i++)memcpy(arena+global_offsets[i],initial+i,4);}
step=0;snapshot(2,0xffffffff,0);
for(i=0;i<h[1];i++){mode=modes[i];if(mode>10)return 15;current_mode=mode;step=i+1;builder_address=addresses[mode];builder_mode=0;builder_x=h[3];builder_y=h[4];builder_poison=h[5];builder_stack_count=0;_controlfp(_PC_53,_MCW_PC);_clearfp();if(BuilderInvoke())return 14;snapshot(0,mode,mode<6?0:builder_result);}
return 0;}
'''
PHYSICAL=r'''
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned h[4],offsets[11],i,j,offset,value;unsigned char*image;
if(argc!=3)return 1;f=fopen(argv[2],"rb");if(!f)return 2;if(fread(h,sizeof(h),1,f)!=1||h[1]!=11||fread(offsets,sizeof(offsets),1,f)!=1||fread(physical_initial,sizeof(physical_initial),1,f)!=1)return 3;
for(j=0;j<11;j++)if(offsets[j]>=h[0])return 3;
image=VirtualAlloc(NULL,h[0],MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image||fread(image,h[0],1,f)!=1)return 4;
for(i=0;i<h[2];i++){if(fread(&offset,4,1,f)!=1||offset>h[0]-4)return 5;memcpy(&value,image+offset,4);value=(unsigned)image+value-h[3];memcpy(image+offset,&value,4);}fclose(f);
FlushInstructionCache(GetCurrentProcess(),image,h[0]);for(j=0;j<11;j++)addresses[j]=(unsigned)image+offsets[j];physical_mode=1;return run(argv[1]);}
'''

def initial_words(binary):
    nt,base,_,table=pe(binary);words_=[];evidence=[]
    for name,address,offset,expected,owner in SPECS:
        sections=[]
        for j in range(struct.unpack_from('<H',binary,nt+6)[0]):
            at=table+j*40;vsize,rva,rsize,file=struct.unpack_from('<4I',binary,at+8)
            if base+rva<=address and address+4<=base+rva+vsize:
                sections.append((base+rva,rsize,file,struct.unpack_from('<I',binary,at+36)[0]))
        assert len(sections)==1;start,size,file,flags=sections[0];assert flags&0xc0000000==0xc0000000 and not flags&0x20000000
        if address+4<=start+size:
            value=struct.unpack('<I',raw(binary,address,4))[0];kind='raw-backed initialized bytes'
        else:
            assert address>=start+size;value=0;kind='zero-filled virtual tail'
        assert value==expected,(name,hex(value),hex(expected))
        words_.append(value);evidence.append(dict(name=name,address=address,width=4,initial_word=value,evidence=kind,owner=owner))
    return words_,evidence

def original_image(binary):
    cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
    low=min(a for _,a,_ in ENTRIES);high=max(a+n for _,a,n in ENTRIES)
    image=bytearray(b'\xcc'*(high-low));bodies=[];patches=[]
    for name,address,size in ENTRIES:
        body=raw(binary,address,size);image[address-low:address-low+size]=body
        ins=list(cs.disasm(body,address));assert sum(i.size for i in ins)==size
        for i in ins:
            assert CS_GRP_CALL not in i.groups and CS_GRP_JUMP not in i.groups and not i.mnemonic.startswith('f')
            if i.mnemonic.startswith('ret'):assert i.mnemonic=='ret'and not i.operands
            for op in i.operands:
                if op.type==CS_OP_MEM:
                    assert not op.mem.base and not op.mem.index and not op.mem.segment and op.size==4 and i.disp_size==4 and op.mem.disp in GLOBALS
                    target=BASE+GLOBALS[op.mem.disp][0];struct.pack_into('<I',image,i.address+i.disp_offset-low,target)
                    patches.append(dict(site=i.address,offset=i.disp_offset,instruction=i.bytes.hex(),original=op.mem.disp,target=target,width=4))
        assert ins[-1].mnemonic=='ret'and ins[-1].address+ins[-1].size==address+size
        bodies.append(dict(name=name,address=address,bytes=size,sha256=sha(body),instructions=[dict(address=i.address,bytes=i.bytes.hex(),instruction=i.mnemonic+' '+i.op_str)for i in ins],calls=[],returns=[dict(site=ins[-1].address,bytes=0)]))
    initial,evidence=initial_words(binary)
    assert len(bodies)==11 and sum(b['bytes']for b in bodies)==122 and sum(len(b['instructions'])for b in bodies)==30 and len(patches)==14
    return words([len(image),11,0,low,*[a-low for _,a,_ in ENTRIES],*initial])+image,dict(bodies=bodies,total_original_bytes=122,patches=patches,globals=GLOBALS,initial_words=initial,initializer_evidence=evidence,calls=[],indirect_calls=[],constants=[],tables=[],relocations=[])

def dependencies():
    include=[ROOT/p for p in('src/msvc500/include','src/carma2/include','src/carma2/hooks','src/brender/include','src/carma2/s3/include','src/carma2/platform')]
    todo=[ROOT/'src/carma2/include/rec2_types.h',ROOT/'src/carma2/hooks/c2_hooks.h'];seen=set()
    while todo:
        path=todo.pop().resolve()
        if path in seen:continue
        seen.add(path)
        for name in re.findall(r'^\s*#\s*include\s*[<"]([^>"]+)',path.read_text(),re.M):
            found=[directory/name for directory in[path.parent,*include]if(directory/name).is_file()]
            if found:todo.append(found[0])
    assert not any(path.name in('27-powerup.h','61-pedestrn.h','zconf.h')for path in seen)
    return sorted(seen)


def source_capsule(candidate=None,source_dir=None):
    if candidate:return '\n'.join(extract(candidate.read_text(),name)for name,_,_ in ENTRIES)
    directory=source_dir or ROOT/'src/carma2/newcommon'
    for name,address,offset,expected,owner in SPECS:
        source=(directory/owner).read_text();header=(directory/owner).with_suffix('.h').read_text()
        initialized=name in('gPending_flic','gAnimalsOn','gFlameThrowerOn','gExplosives_on','gGoreLevel')
        value=-1 if expected==0xffffffff else expected
        declaration=r'int\s+'+re.escape(name)+(r'\s*=\s*'+str(value) if initialized else '')+r'\s*;'
        assert re.search(r'// GLOBAL: CARMA2_HW '+f'{address:#010x}'+r'\s+'+declaration,source),('Authenticated owning declaration and initializer',name,owner,value)
        assert re.search(r'extern\s+int\s+'+re.escape(name)+r'\s*;',header),('Exact four-byte global declaration',name)
    result=[]
    for index,(name,address,_)in enumerate(ENTRIES):
        owner='38-flicplay.c'if index<6 else'61-pedestrn.c'if index<10 else'57-drone.c'
        source=(directory/owner).read_text();header=(directory/owner).with_suffix('.h').read_text()
        typ='void'if index<6 else'int'
        assert re.search(r'// FUNCTION: CARMA2_HW '+f'{address:#010x}'+r'\s+'+typ+r' C2_HOOK_FASTCALL '+re.escape(name)+r'\(void\)',source),('Complete root signature/annotation',name)
        assert re.search(r'(?:extern\s+)?'+typ+r' C2_HOOK_FASTCALL '+re.escape(name)+r'\(void\);',header),('Noargs root prototype',name)
        result.append(extract(source,name))
    return '\n'.join(result)

def matching_image(exe,symbols):
    """Eleven exact complete no-call roots, nine authenticated scalar initializers."""
    from capstone import CS_OP_IMM
    disk=exe.read_bytes();mapbytes=exe.with_suffix('.map').read_bytes();mapping=json.loads(symbols.read_bytes())
    assert mapping['format']==1 and mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(disk)
    mapped={}
    for row in mapping['symbols']:
        if row['orig']in mapped:assert mapped[row['orig']]==row['recomp']
        mapped[row['orig']]=row['recomp']
    funcs={};data={}
    for line in mapbytes.decode('latin1').splitlines():
        fields=line.split()
        if len(fields)<4 or len(fields[0].split(':'))!=2:continue
        try:address=int(fields[2],16)
        except ValueError:continue
        if 'f'in fields[3:-1]:funcs.setdefault(address,set()).add((fields[1],fields[-1]))
        else:data.setdefault(fields[1],set()).add((address,fields[-1]))
    owners={name:('38-flicplay.c.obj'if index<6 else'61-pedestrn.c.obj'if index<10 else'57-drone.c.obj','@'+name+'@0')for index,(name,_,_)in enumerate(ENTRIES)}
    def identity(address):
        assert address in funcs,('Missing mapped function',hex(address))
        assert len(funcs[address])==1,('Ambiguous reached MAP identity',hex(address),funcs[address])
        decorated,owner=next(iter(funcs[address]));name=decorated.lstrip('_@').split('@')[0]
        assert name in owners and (owner,decorated)==owners[name],('Unexpected complete function identity',hex(address),decorated,owner)
        return name,owner,decorated
    roots=[]
    for name,original,_ in ENTRIES:
        assert original in mapped,('Missing complete root mapping',name,hex(original))
        address=mapped[original];assert identity(address)[0]==name;roots.append(address)
    nt,base,image_size,tab=pe(disk);sections=[]
    for index in range(struct.unpack_from('<H',disk,nt+6)[0]):
        at=tab+index*40;vsize,rva,rsize,file=struct.unpack_from('<4I',disk,at+8)
        sections.append(dict(start=base+rva,end=base+rva+max(vsize,rsize),raw_end=base+rva+rsize,flags=struct.unpack_from('<I',disk,at+36)[0],name=disk[at:at+8].split(b'\0')[0].decode('ascii')))
    def section(address,n):
        found=[s for s in sections if s['start']<=address and address+n<=s['end']]
        assert len(found)==1,('PE whole span',hex(address),n)
        return found[0]
    globals_={};initial=[]
    for name,original,offset,expected,source_owner in SPECS:
        assert original in mapped,('Missing named global mapping',name)
        values=data.get('_'+name,set());assert len(values)==1,('Ambiguous named global',name,values)
        address,owner=next(iter(values))
        expected_owner=source_owner+'.obj'if name in('gPending_flic','gAnimalsOn','gFlameThrowerOn','gExplosives_on','gGoreLevel')else'<common>'
        assert address==mapped[original]and owner==expected_owner,('Exact initialized/common owner',name,owner,expected_owner)
        sec=section(address,4);assert sec['flags']&0xc0000000==0xc0000000 and not sec['flags']&0x20000000
        if address+4<=sec['raw_end']:value=struct.unpack('<I',raw(disk,address,4))[0];kind='raw-backed initialized bytes'
        else:assert address>=sec['raw_end'];value=0;kind='zero-filled virtual tail'
        assert value==expected,('Genuine current PE initializer mismatch; no fixture override credit',name,hex(value),hex(expected))
        globals_[address]=dict(name=name,original=original,address=address,bytes=4,target=BASE+offset,owner=owner,source_owner=source_owner,initial_word=value,initializer_evidence=kind)
        initial.append(value)
    rows=list(globals_.values());assert len(rows)==9
    assert all(a['address']+4<=b['address']or b['address']+4<=a['address']for index,a in enumerate(rows)for b in rows[index+1:])
    starts=sorted(funcs);cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
    active={};bodies=[];patches={}
    for ins_root in roots:
        name,owner,decorated=identity(ins_root);later=[a for a in starts if a>ins_root];assert later;end=later[0]
        sec=section(ins_root,end-ins_root);assert sec['flags']&0x20 and sec['flags']&0x40000000 and sec['flags']&0x20000000 and not sec['flags']&0x80000000
        pending=[ins_root];decoded={};successors={}
        while pending:
            at=pending.pop()
            if at in decoded:continue
            assert ins_root<=at<end
            instruction=next(cs.disasm(raw(disk,at,min(15,end-at)),at,count=1));assert at+instruction.size<=end
            sec=section(at,instruction.size);assert sec['flags']&0x20 and sec['flags']&0x40000000 and sec['flags']&0x20000000 and not sec['flags']&0x80000000
            assert instruction.mnemonic not in('int','int1','int3','into','ud2','hlt','cli','sti','retf','lret','syscall','sysenter','sysexit','iret','iretd')
            assert CS_GRP_CALL not in instruction.groups,('No arbitrary/genuine new callee authority',name,hex(at),instruction.op_str)
            assert not any(prefix in(0x26,0x2e,0x36,0x3e,0x64,0x65,0x66,0x67)for prefix in instruction.prefix if prefix)
            assert not instruction.mnemonic.startswith('f'),('Unexpected FP operation',name,hex(at))
            for byte in range(at,at+instruction.size):assert byte not in active;active[byte]=at
            decoded[at]=instruction
            for operand in instruction.operands:
                if operand.type==CS_OP_MEM:
                    assert not operand.mem.segment and not operand.mem.index
                    if operand.mem.base:
                        assert instruction.reg_name(operand.mem.base)in('esp','ebp'),('Unexpected indirect non-stack storage',name,hex(at),instruction.op_str)
                    else:
                        assert operand.size==4 and instruction.disp_size==4 and operand.mem.disp in globals_
                        global_=globals_[operand.mem.disp];site=at+instruction.disp_offset
                        patch=dict(site=site,original_value=operand.mem.disp,target=global_['target'],mode='arena',width=4,authority=global_['name'],instruction_address=at,instruction_hex=instruction.bytes.hex())
                        if site in patches:assert patches[site]==patch
                        patches[site]=patch
                if operand.type==CS_OP_IMM and CS_GRP_JUMP not in instruction.groups:
                    assert not base<=operand.imm<base+image_size,('Unclassified image immediate',hex(at),instruction.op_str)
            if CS_GRP_JUMP in instruction.groups:
                assert len(instruction.operands)==1 and instruction.operands[0].type==CS_OP_IMM
                target=instruction.operands[0].imm;assert ins_root<=target<end;nexts=[target]
                if instruction.mnemonic!='jmp':nexts.append(at+instruction.size)
            elif instruction.mnemonic.startswith('ret'):
                assert instruction.mnemonic=='ret'and not instruction.operands;nexts=[]
            else:nexts=[at+instruction.size]
            successors[at]=nexts;pending+=nexts
        assert all(target in decoded for values in successors.values()for target in values)
        bodies.append(dict(name=name,owner=owner,decorated=decorated,address=ins_root,end=end,instruction_count=len(decoded),instructions=[dict(address=i.address,hex=i.bytes.hex(),mnemonic=i.mnemonic,operands=i.op_str)for i in sorted(decoded.values(),key=lambda x:x.address)],successors=successors,direct_calls=[]))
    rva,n=struct.unpack_from('<II',disk,nt+24+96+5*8);coff=struct.unpack_from('<H',disk,nt+22)[0];relocations=set()
    if rva or n:
        assert rva and n;cursor=0
        while cursor<n:
            assert cursor+8<=n
            page,length=struct.unpack('<II',raw(disk,base+rva+cursor,8));assert length>=8 and length%2==0 and cursor+length<=n and page%4096==0
            for item in struct.unpack('<'+'H'*((length-8)//2),raw(disk,base+rva+cursor+8,length-8)):
                kind=item>>12
                if not kind:continue
                assert kind==3;site=base+page+(item&4095);assert site+4<=base+image_size and site not in relocations;relocations.add(site)
                if any(site+j in active for j in range(4)):assert site in patches,('Unclassified active HIGHLOW',hex(site))
            cursor+=length
        assert cursor==n and all(site in relocations for site in patches)
    else:assert coff&1
    low=min(active);high=max(active)+1;image=bytearray(b'\xcc'*(high-low))
    for row in bodies:
        for ins in row['instructions']:blob=bytes.fromhex(ins['hex']);image[ins['address']-low:ins['address']-low+len(blob)]=blob
    for site,row in patches.items():struct.pack_into('<I',image,site-low,row['target'])
    # Both original and matching startup packets carry independently authenticated
    # genuine PE initializer words; raw-state fixtures cannot hide Gore1.
    packet=words([len(image),11,0,low,*[a-low for a in roots],*initial])+image
    meta=dict(recompiled_sha256=sha(disk),symbols_sha256=sha(symbols.read_bytes()),map_sha256=sha(mapbytes),roots=roots,original_roots=[hex(a)for _,a,_ in ENTRIES],graph=bodies,patches=list(patches.values()),globals=rows,initial_words=initial,low=low,high=high,relocation_directory=[rva,n],coff_characteristics=coff,relocations=sorted(site for site in relocations if any(site+j in active for j in range(4))),calls=[],indirect_calls=[],tables=[],constants=[],map_aliases=[dict(address=a,aliases=sorted(values),reachable=a in roots)for a,values in funcs.items()if len(values)>1])
    return packet,meta


def main():
    parser=argparse.ArgumentParser();parser.add_argument('--output',type=Path,required=True);parser.add_argument('--case-index',type=int,action='append');parser.add_argument('--limit',type=int);parser.add_argument('--candidate',type=Path);parser.add_argument('--source-dir',type=Path);parser.add_argument('--package-only',action='store_true');parser.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE');parser.add_argument('--recompiled',type=Path);parser.add_argument('--symbols',type=Path);args=parser.parse_args()
    assert bool(args.recompiled)==bool(args.symbols);assert not(args.candidate and args.source_dir);assert not(args.recompiled and(args.candidate or args.source_dir));assert not args.source_dir or args.package_only
    out=args.output.resolve();assert not out.exists();out.mkdir(parents=True)
    relevant=[Path(__file__),args.original]+[ROOT/'scripts'/p for p in('verify_phil_registry_reconstruction.py','verify_opponent_navigation_reconstruction.py','verify_opponent_route_reconstruction.py','verify_tinted_native_reconstruction.py')]+dependencies()
    if args.candidate:relevant.append(args.candidate)
    else:
        directory=args.source_dir or ROOT/'src/carma2/newcommon'
        relevant +=[directory/owner for owner in('38-flicplay.c','38-flicplay.h','61-pedestrn.c','61-pedestrn.h','57-drone.c','57-drone.h')]
    if args.recompiled:relevant +=[args.recompiled,args.symbols,args.recompiled.with_suffix('.map')]
    before={str(path.resolve()):sha(path.read_bytes())for path in relevant};(out/'source-before.json').write_text(json.dumps(before,indent=2)+'\n');(out/'sources').mkdir();snapshots={}
    for i,path in enumerate(relevant):
        if path.suffix in('.c','.h','.py','.json'):
            dest=out/'sources'/f'{i}-{path.name}';dest.write_bytes(path.read_bytes());snapshots[str(path.resolve())]=str(dest.resolve())
    (out/'source-snapshots.json').write_text(json.dumps(snapshots,indent=2)+'\n');(out/'verifier-snapshot.py').write_bytes(Path(__file__).read_bytes());
    if args.candidate:(out/'candidate-snapshot.c').write_bytes(args.candidate.read_bytes())
    try:
        binary=args.original.read_bytes();assert sha(binary)==HASH
        packet,graph=original_image(binary);(out/'original-code.bin').write_bytes(packet)
        body=source_capsule(args.candidate,args.source_dir)
        native=COMMON+'\n'+body+'\nint C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;'+''.join('addresses[%d]=(unsigned)%s;'%(i,name)for i,(name,_,_)in enumerate(ENTRIES))+'physical_mode=0;return run(argv[1]);}\n'
        caller,_,_=registry.literals()
        matching=None
        if args.recompiled:
            matching_packet,matching=matching_image(args.recompiled,args.symbols)
            assert matching['initial_words']==graph['initial_words'],('All nine authentic startup words',matching['initial_words'],graph['initial_words'])
            (out/'matching-code.bin').write_bytes(matching_packet)
        cases=fixtures();assert len(cases)==670 and sha(b''.join(payload for _,payload in cases))=='93eaff42c2a61c0667100a818c174b3d2602e75f4d2ce032dae98ff27efb803b'
        if args.package_only:
            for filename,text in [('native.c',native),('physical.c',COMMON+PHYSICAL),('caller.c',caller)]: (out/filename).write_text(text)
            after={path:sha(Path(path).read_bytes())for path in before};assert before==after
            package=dict(passed=True,package_only=True,scenes=670,complete_suite=False,runtime_comparisons=0,matching=matching,original=graph,source_before=before,source_after=after,fixture_sha256=[sha(payload)for _,payload in cases],compiler_input_sha256={name:sha((out/name).read_bytes())for name in('native.c','physical.c','caller.c','original-code.bin')},abi_modes=['void']*6+['int']*5,limits=__doc__,live_handles=[])
            (out/'package.json').write_text(json.dumps(package,indent=2)+'\n');print('PACKAGE ONLY PASS670; zero runtime credit');return
        registry.COMMON=COMMON;commands=registry.compile_sources(out,native,PHYSICAL,caller)
        indices=args.case_index or list(range(len(cases)));indices=indices[:args.limit]if args.limit else indices
        (out/'fixtures.bin').write_bytes(b''.join(cases[i][1]for i in indices));(out/'fixture-metadata.json').write_text(json.dumps([cases[i][0]for i in indices],indent=2)+'\n')
        stride=ARENA+16;results=[];events=[];failures=[];streams={name:(out/(name+'-outputs.bin')).open('wb')for name in('original','msvc5','msvc','mingw')+(('matching',)if matching else())}
        try:
            for index in indices:
                case,payload=cases[index];fixture=out/'case.bin';fixture.write_bytes(payload)
                reference=subprocess.run([str(out/'physical.exe'),str(fixture),str(out/'original-code.bin')],capture_output=True,timeout=20)
                assert reference.returncode==0 and len(reference.stdout)==case['expected_events']*stride,(index,reference.returncode,len(reference.stdout),reference.stderr)
                expected=reference.stdout;streams['original'].write(expected)
                for event in range(case['expected_events']):
                    kind,step,mode,value=struct.unpack_from('<4I',expected,event*stride)
                    assert step==event and((event==0 and kind==2 and mode==0xffffffff and value==0)or(event>0 and kind==0 and mode==case['modes'][event-1]))
                    events.append(dict(index=index,event=event,kind=kind,mode=mode,value=value,normal_return_abi_checked=kind==0,raw_eax_checked=kind==0 and mode>=6))
                for compiler in('msvc5','msvc','mingw')+(('matching',)if matching else()):
                    command=[str(out/'physical.exe'),str(fixture),str(out/'matching-code.bin')]if compiler=='matching'else[str(out/(compiler+'.exe')),str(fixture)]
                    actual=subprocess.run(command,capture_output=True,timeout=20);streams[compiler].write(actual.stdout)
                    same=actual.returncode==0 and actual.stdout==expected
                    row=dict(index=index,compiler=compiler,passed=same,input_sha256=sha(payload),original_output_sha256=sha(expected),actual_output_sha256=sha(actual.stdout),exit_code=actual.returncode,event_count=case['expected_events'],normal_calls=case['normal_calls'],startup_initializer_case=case['startup_from_declarations_or_PE'])
                    results.append(row)
                    if not same:
                        prefix=out/f'failure-{compiler}-{index}';prefix.with_suffix('.original.bin').write_bytes(expected);prefix.with_suffix('.actual.bin').write_bytes(actual.stdout);prefix.with_suffix('.json').write_text(json.dumps(dict(case=case,row=row,stderr=actual.stderr.hex()),indent=2)+'\n');failures.append(row)
                if len(results)%(400 if matching else 300)==0:print('checked',len(results)//(4 if matching else 3),len(results),'failures',len(failures),flush=True)
        finally:
            for stream in streams.values():stream.close()
        after={path:sha(Path(path).read_bytes())for path in before};assert after==before
        report=dict(passed=not failures,scenes=len(indices),comparisons=len(results),event_comparisons=sum(cases[i][0]['expected_events']for i in indices)*(4 if matching else 3),
                    normal_calls=sum(cases[i][0]['normal_calls']for i in indices),startup_events=len(indices),complete_suite=indices==list(range(len(cases))),
                    genuine_startup_scenes=sum(cases[i][0]['startup_from_declarations_or_PE']for i in indices),source_stable=True,source_before=before,source_after=after,
                    original_sha256=HASH,graph=graph,matching=matching,recompiled_sha256=matching['recompiled_sha256']if matching else None,symbols_sha256=matching['symbols_sha256']if matching else None,abi_modes=['void']*6+['int']*5,commands=commands,fixture_sha256=[sha(cases[i][1])for i in indices],fixtures_sha256=sha((out/'fixtures.bin').read_bytes()),
                    events=events,results=results,failures=failures,limits=__doc__,live_handles=[],new_acceptance_credit=0,
                    artifacts={str(path.resolve()):sha(path.read_bytes())for path in out.iterdir()if path.is_file()})
        (out/'verification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS'if not failures else'FAIL',len(indices),len(results),report['event_comparisons'],flush=True)
        if failures:raise SystemExit(1)
    finally:
        after={path:sha(Path(path).read_bytes())for path in before};(out/'guard-closure.json').write_text(json.dumps(dict(source_before=before,source_after=after,source_stable=before==after),indent=2)+'\n')

if __name__=='__main__':main()
