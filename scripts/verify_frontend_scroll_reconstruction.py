'Complete frontend3/198B original/native3 proof. Raw32KiB public arenas, controlled acyclic Win32 storage, maskedPC53/DF0 entry. Authentic FASTCALL0/1/2arg RET0 and corresponding CDECL0/1/2stackword callers. Normal raw pointer/signed EAX, ESP/nonvolatile/x87TOP checked; outgoing DF not explicitly observed. Terminal AV compares first inaccessible address and complete public prefix only, not normal ABI/private fault stack. No callbacks/math/game substitute or cyclic full-return claim.'
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
ENTRIES=[('GetAnyActiveSlider',0x471cf0,53),('GetScrollSet',0x471d30,62),('ScrollSet_TranslateItemToIndex',0x471d70,83)]
GLOBAL_OFFSETS={'gCurrent_frontend_scrollbars':0x7000,'gFrontend_selected_item_index':0x7100,'gConnected_items':0x7200}

def fixtures():
 cases=[];rng=random.Random(0x471cf0)
 def add(mode,label,item=0,head=None,selected=None,connected=None,nodes=None,patches=(),qualified=False,**extra):
  memory=bytearray(ARENA)
  for off in range(0,ARENA,4):struct.pack_into('<I',memory,off,(0xa5a50000+off*13)&0xffffffff)
  def put(address,value):
   if BASE<=address<=BASE+ARENA-4:struct.pack_into('<I',memory,address-BASE,value&0xffffffff)
  # Every constructed chain is explicitly terminated, including raw field aliases.
  for node in nodes or []:
   address=node['address']
   if node['kind']=='slider':
    for off,v in ((4,node.get('left',0)),(8,node.get('right',10)),(32,node.get('next',0))):put(address+off,v)
   else:
    for off,v in ((4,node.get('length',3)),(8,node.get('base',0)),(12,node.get('count',len(node.get('starts',[0])))),(44,node.get('next',0))):put(address+off,v)
    for i,v in enumerate(node.get('starts',[0])):put(address+16+4*i,v)
  # Public global slots may deliberately be part of a node. These stores are
  # input construction, not an invocation-time mutation or geometry substitute.
  put(BASE+GLOBAL_OFFSETS['gCurrent_frontend_scrollbars'],0 if head is None else head)
  put(BASE+GLOBAL_OFFSETS['gFrontend_selected_item_index'],item if selected is None else selected)
  put(BASE+GLOBAL_OFFSETS['gConnected_items'],0 if connected is None else connected)
  for address,value in patches:put(address,value)
  x=0 if mode==0 else item if mode==1 else (BASE+0x1000 if head is None else head)
  y=item if mode==2 else 0
  metadata=dict(index=len(cases),name=ENTRIES[mode][0],mode=mode,label=label,x=x&0xffffffff,y=y&0xffffffff,item=item&0xffffffff,head_argument=head,connected_argument=connected,selected_argument=selected,nodes=nodes or [],patches=list(patches),controlled_storage_qualified=qualified,**extra)
  header=struct.pack('<7I',mode,x&0xffffffff,y&0xffffffff,0,0,0,0x7f800001 if len(cases)%2 else 0xcccccccc)
  cases.append((metadata,header+memory))
 def slide(address=BASE+0x1000,left=0,right=10,next=0):return dict(kind='slider',address=address,left=left,right=right,next=next)
 def ranges(address=BASE+0x1000,starts=(0,),length=3,base=100,count=None,next=0):return dict(kind='range',address=address,starts=list(starts),length=length,base=base,count=len(starts)if count is None else count,next=next)
 A=BASE+0x1000;B=BASE+0x1800;C=BASE+0x2000
 signed=[-0x80000000,-0x7fffffff,-0x7ffffffe,-11,-3,-2,-1,0,1,2,3,10,0x7ffffffc,0x7ffffffd,0x7ffffffe,0x7fffffff]
 for item in signed:
  add(0,'null-slider-head-selected-unused',item=item)
  add(1,'null-connected-head',item=item)
  add(2,'zero-count-base-and-length-unused',head=A,item=item,nodes=[ranges(count=0,starts=[])])
 for start in signed:
  for delta in (-1,0,1,2,3):
   item=((start+delta+0x80000000)&0xffffffff)-0x80000000
   add(0,'signed-wrapped-slider-left-range',item=item,head=A,nodes=[slide(left=start,right=-100)])
   add(0,'signed-wrapped-slider-right-range',item=item,head=A,nodes=[slide(left=-100,right=start)])
 # Explicit original/common counterexample: start=INTMAX-2, selected=start.
 add(0,'exact-common-shortcut-counterexample',item=0x7ffffffd,head=A,nodes=[slide(left=0x7ffffffd,right=0x7ffffffd)],original_expected=0,common_shortcut_would_match=True)
 for mode in (1,2):
  for start in signed:
   for length in (-0x80000000,-3,0,1,3,0x7fffffff):
    for delta in (-1,0,1,3):
     item=((start+delta+0x80000000)&0xffffffff)-0x80000000
     add(mode,'signed-wrapped-connected-range',item=item,head=A,connected=A,nodes=[ranges(starts=[start],length=length,base=0x7ffffffe)])
  for count in (-0x80000000,-1,0,1,7,8,9):
   for item in (-1,0,6,7,8,20):
    starts=list(range(max(count,0)))
    add(mode,'captured-raw-count-and-overlong-array',head=A,connected=A,item=item,nodes=[ranges(starts=starts,count=count,length=1,next=0)],qualified=count>7)
  for base in signed:
   for item in (-1,0,1,2,3):add(mode,'translated-base-wrap-or-unused',head=A,connected=A,item=item,nodes=[ranges(starts=[0],base=base)])
 # List traversal catches the old decompilation's missing +2c advance. Return
 # first matching pointer; no writes/callbacks mean aliases cannot mutate count.
 for mode in (0,1):
  for size in (1,2,3):
   for match in range(-1,size):
    addresses=[A,B,C][:size]
    nodes=[slide(address=a,left=30 if i==match else 0,right=50,next=addresses[i+1]if i+1<size else 0)if mode==0 else ranges(address=a,starts=[30 if i==match else 0],next=addresses[i+1]if i+1<size else 0)for i,a in enumerate(addresses)]
    add(mode,'acyclic-first-last-no-match',head=A,connected=A,item=31,nodes=nodes)
  duplicate=[slide(A,left=30,next=B),slide(B,left=30)]if mode==0 else[ranges(A,starts=[30],next=B),ranges(B,starts=[30])]
  add(mode,'duplicate-matches-return-first',head=A,connected=A,item=31,nodes=duplicate)
 for shift in (1,2,3):
  for mode in range(3):
   addr=A+shift
   node=slide(addr,left=20,right=40)if mode==0 else ranges(addr,starts=[-10,20,40],length=3,base=-50)
   add(mode,'unaligned-exact-field-seeding',head=addr,connected=addr,item=21,nodes=[node],qualified=True)
 # Struct fields may overlap the global storage; observed memory is input only.
 for mode in range(3):
  offsets=(4,8,32)if mode==0 else(4,8,12,16,44)
  for slot in GLOBAL_OFFSETS.values():
   for field in offsets:
    addr=BASE+slot-field
    node=slide(addr,left=20,right=40)if mode==0 else ranges(addr,starts=[20],length=3,base=50,count=1)
    # next must stay zero; restoring it may also overwrite a head slot and
    # legitimately make the fixture null. Avoid huge aliased counts by finalpatch.
    patches=[(addr+32 if mode==0 else addr+44,0)]
    if mode!=0:patches.append((addr+12,1))
    add(mode,'node-field-global-alias',head=addr,connected=addr,item=21,nodes=[node],patches=patches,qualified=True)
 # Fault/short-circuit boundary cases; first inaccessible address is observed,
 # not inferred from the starting effective address of a multi-byte read.
 for mode in range(3):
  for ptr in (0,1,0xfffffff0,BASE+ARENA,BASE+ARENA-1,BASE+ARENA-3):
   add(mode,'raw-head-or-count-first-fault',head=ptr,connected=ptr,item=1,qualified=True)
 for shift in (0,1,3):
  addr=BASE+ARENA-8-shift
  for item in (0,1,3,10):add(0,'right-read-short-circuit-at-boundary',head=addr,item=item,nodes=[slide(addr,left=0,right=10)],qualified=True)
  addr=BASE+ARENA-32-shift
  for item in (0,3,11,20):add(0,'next-read-short-circuit-at-boundary',head=addr,item=item,nodes=[slide(addr,left=0,right=10)],qualified=True)
 for mode in (1,2):
  for shift in (0,1,3):
   addr=BASE+ARENA-16-shift
   add(mode,'first-start-read-boundary',head=addr,connected=addr,item=1,nodes=[ranges(addr,count=1,starts=[])],qualified=True)
   add(mode,'zero-count-skips-invalid-start',head=addr,connected=addr,item=1,nodes=[ranges(addr,count=0,starts=[])],qualified=True)
   addr=BASE-8-shift
   for item in (-1,0,1,3):
    add(mode,'length-read-short-circuit-before-arena',head=addr,connected=addr,item=item,nodes=[ranges(addr,starts=[0],count=1)],patches=[(addr+44,0)],qualified=True)
  addr=BASE+ARENA-44
  for item in (0,3,10):add(mode,'next-read-at-boundary-or-translate-never-follows',head=addr,connected=addr,item=item,nodes=[ranges(addr,starts=[0])],qualified=True)
 for i in range(180):
  mode=i%3;item=rng.randint(-0x80000000,0x7fffffff);starts=[rng.randint(-0x80000000,0x7fffffff)for _ in range(rng.randrange(8))]
  node=slide(left=rng.randint(-0x80000000,0x7fffffff),right=rng.randint(-0x80000000,0x7fffffff))if mode==0 else ranges(starts=starts,length=rng.randint(-0x80000000,0x7fffffff),base=rng.randint(-0x80000000,0x7fffffff))
  add(mode,'deterministic-raw-finite-list-grid',head=A,connected=A,item=item,nodes=[node])
 return cases


COMMON='\n#include <windows.h>\n#include <stdio.h>\n#include <stdlib.h>\n#include <string.h>\n#include <stdint.h>\n#include <math.h>\n#include <float.h>\n#include <io.h>\n#include <fcntl.h>\n#include "72-interface.h"\n#include "brender/br_defs.h"\nstatic unsigned char*arena;static unsigned addresses[3];static unsigned physical_mode;\nextern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison,builder_cdecl;\nextern unsigned __cdecl BuilderInvoke(void);\n#define gCurrent_frontend_scrollbars (*(tFrontend_slider**)(arena+0x7000))\n#define gFrontend_selected_item_index (*(int*)(arena+0x7100))\n#define gConnected_items (*(tConnected_items**)(arena+0x7200))\ntypedef char layouts[sizeof(void*)==4&&sizeof(tFrontend_slider)==36&&offsetof(tFrontend_slider,itemid_left_reference)==4&&offsetof(tFrontend_slider,itemid_start)==8&&offsetof(tFrontend_slider,next)==32&&sizeof(tConnected_items)==48&&offsetof(tConnected_items,range_length)==4&&offsetof(tConnected_items,field_0x8)==8&&offsetof(tConnected_items,count_ranges)==12&&offsetof(tConnected_items,range_starts)==16&&offsetof(tConnected_items,next)==44?1:-1];\nstatic void snapshot(unsigned event,unsigned value,unsigned address){unsigned h[3];h[0]=event;h[1]=value;h[2]=address;fwrite(h,12,1,stdout);fwrite(arena,32768,1,stdout);fflush(stdout);}\nstatic LONG WINAPI observe_fault(EXCEPTION_POINTERS*e){if(e->ExceptionRecord->ExceptionCode!=EXCEPTION_ACCESS_VIOLATION)return EXCEPTION_CONTINUE_SEARCH;snapshot(3,e->ExceptionRecord->ExceptionCode,(unsigned)e->ExceptionRecord->ExceptionInformation[1]);ExitProcess(0);return EXCEPTION_CONTINUE_SEARCH;}\nstatic int run(const char*path){FILE*f;unsigned h[7],mode;FARPROC proc;typedef void*(WINAPI *add_handler)(ULONG,LONG(WINAPI*)(EXCEPTION_POINTERS*));add_handler handler;\nSetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);f=fopen(path,"rb");if(!f)return 10;arena=VirtualAlloc((void*)0x26000000,65536,MEM_RESERVE,PAGE_READWRITE);if(arena!=(void*)0x26000000)return 11;if(VirtualAlloc(arena,32768,MEM_COMMIT,PAGE_READWRITE)!=arena)return 11;\nif(fread(h,sizeof(h),1,f)!=1||fread(arena,32768,1,f)!=1)return 12;fclose(f);_setmode(_fileno(stdout),_O_BINARY);\nproc=GetProcAddress(GetModuleHandleA("kernel32.dll"),"AddVectoredExceptionHandler");memcpy(&handler,&proc,4);if(!handler||!handler(1,observe_fault))return 13;mode=h[0];if(mode>2)return 15;\nbuilder_address=addresses[mode];builder_mode=0;builder_poison=h[6];builder_x=h[1];builder_y=h[2];\n#if defined(REC2_MATCHING)\nbuilder_cdecl=0;\n#else\nbuilder_cdecl=1;\n#endif\nif(physical_mode)builder_cdecl=0;\nbuilder_stack_count=0;\nif(builder_cdecl){builder_stack_count=mode;builder_stack[0]=h[1];builder_stack[1]=h[2];}\n_controlfp(_PC_53,_MCW_PC);_clearfp();if(BuilderInvoke())return 14;snapshot(0,builder_result,0);return 0;}\n'
PHYSICAL='\nint C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned h[4],offsets[3],i,j,offset,value;unsigned char*image;\nif(argc!=3)return 1;f=fopen(argv[2],"rb");if(!f)return 2;if(fread(h,sizeof(h),1,f)!=1||h[1]!=3||fread(offsets,sizeof(offsets),1,f)!=1)return 3;\nfor(j=0;j<3;j++)if(offsets[j]>=h[0])return 3;\nimage=VirtualAlloc(NULL,h[0],MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image||fread(image,h[0],1,f)!=1)return 4;\nfor(i=0;i<h[2];i++){if(fread(&offset,4,1,f)!=1||offset>h[0]-4)return 5;memcpy(&value,image+offset,4);value=(unsigned)image+value-h[3];memcpy(image+offset,&value,4);}fclose(f);\nFlushInstructionCache(GetCurrentProcess(),image,h[0]);for(j=0;j<3;j++)addresses[j]=(unsigned)image+offsets[j];physical_mode=1;return run(argv[1]);}\n'
GLOBALS={0x686820:(0x7000,4),0x688770:(0x7100,4),0x688ab0:(0x7200,4)}
SPECS=[('gCurrent_frontend_scrollbars',0x686820,0x7000,4,'72-interface.c','tFrontend_slider*'),('gFrontend_selected_item_index',0x688770,0x7100,4,'72-interface.c','int'),('gConnected_items',0x688ab0,0x7200,4,'72-interface.c','tConnected_items*')]
FROZEN_INPUT_SHA256='dd780c70cf810a52019e5727dae37357ca964cea0e2a531414d29983acafdafc'
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
                           calls=[],returns=[dict(site=i.address,bytes=0)for i in ins if i.mnemonic=='ret']))
    assert len(bodies)==3 and sum(b['bytes']for b in bodies)==198 and len(patches)==3
    return words([len(image),3,0,low,*[a-low for _,a,_ in ENTRIES]])+image,dict(bodies=bodies,total_original_bytes=198,low=low,high=high,patches=patches,globals=GLOBALS,relocations=[],constants=[],calls=[],indirect_calls=[],tables=[])

def source_capsule(candidate=None,source_dir=None):
    import re
    directory=source_dir or ROOT/'src/carma2/newcommon'
    source=(candidate if candidate else directory/'72-interface.c').read_text()
    if candidate is None:
        header=(directory/'72-interface.h').read_text()
        for name,address,arena_at,n,owner,typ in SPECS:
            declaration=re.escape(typ)+r'\s+'+re.escape(name)+r'\s*;'
            assert re.search(r'// GLOBAL: CARMA2_HW '+f'{address:#010x}'+r'\s+'+declaration,source),(name,owner)
            assert re.search(r'extern\s+'+declaration,header),(name,owner)
        signatures=[('tFrontend_slider*','void'),('tConnected_items*','int pItem'),('int','tConnected_items* pConnected, int pItem')]
        for mode,(name,address,_)in enumerate(ENTRIES):
            typ,args=signatures[mode]
            pattern=re.escape(typ)+r'\s+C2_HOOK_FASTCALL\s+'+re.escape(name)+r'\('+re.escape(args)+r'\)'
            assert re.search(r'// FUNCTION: CARMA2_HW '+f'{address:#010x}'+r'\s+'+pattern+r'\s*\{',source),(name,'complete annotation/signature')
            assert re.search(r'(?:extern\s+)?'+pattern+r';',header),(name,'prototype')
    alias='#if defined(__GNUC__)\ntypedef tU32 tFrontendScrollWord __attribute__((__may_alias__));\n#else\ntypedef tU32 tFrontendScrollWord;\n#endif\n'
    assert alias.rstrip('\n')in source
    return alias+'\n'.join(extract(source,n)for n in('FrontendScrollRead',*[name for name,_,_ in ENTRIES]))

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
    owners={name:('72-interface.c.obj','@'+name+'@'+str(mode*4))for mode,(name,_,_)in enumerate(ENTRIES)};owners['FrontendScrollRead']=('72-interface.c.obj','_FrontendScrollRead')
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
    starts=sorted(funcs);todo=list(roots);bodies={};active={};patches={};constants={};edges=[];memory_accesses=[]
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
    def prove_private_stack(decoded,successors,name):
        # Exact single offset from this function's caller-established entryESP.
        # Merges must agree. No widening, guessed EBP frame or call-clobber
        # approximation: only the separately complete CDECL helper may be called,
        # and every reached body itself must return with its own offset zero.
        entry=min(decoded);pending=[(entry,0)];offsets={};stores=[];exits=[]
        while pending:
            at,offset=pending.pop()
            if at in offsets:
                assert offsets[at]==offset,('Conflicting stack offsets at CFG merge',name,hex(at),offsets[at],offset)
                continue
            assert -4096<=offset<=0,('Outside authenticated own stack allocation',name,hex(at),offset)
            offsets[at]=offset;ins=decoded[at]
            for index,op in enumerate(ins.operands):
                if op.type==CS_OP_MEM and ins.mnemonic=='mov' and index==0:
                    assert cs.reg_name(op.mem.base)=='esp' and not op.mem.index and op.size==4 and not op.mem.segment
                    start=offset+op.mem.disp
                    assert offset<=start and start+4<=0,('Store reaches caller return/arguments or unallocated stack',name,hex(at),offset,start,op.size)
                    stores.append(dict(site=at,entry_esp_offset=offset,displacement=op.mem.disp,owned_slot_start=start,bytes=4))
            after=offset
            if ins.mnemonic in('push','pop'):
                assert len(ins.operands)==1 and ins.operands[0].size==4
                if ins.mnemonic=='pop':assert ins.operands[0].type==1 and cs.reg_name(ins.operands[0].reg)not in('esp','sp','spl'),('Unproved POP memory/rebase',name,hex(at))
                after+=-4 if ins.mnemonic=='push'else 4
            elif ins.operands and ins.operands[0].type==1 and cs.reg_name(ins.operands[0].reg)=='esp':
                if ins.mnemonic=='add':after+=ins.operands[1].imm
                elif ins.mnemonic=='sub':after-=ins.operands[1].imm
                elif ins.mnemonic=='lea':after+=ins.operands[1].mem.disp
                else:assert ins.mnemonic in('cmp','test'),('Unproved ESP definition',name,hex(at),ins.mnemonic)
            assert -4096<=after<=0,('Stack adjustment crosses own entry/bounded allocation',name,hex(at),after)
            nexts=successors[at]
            if not nexts:
                assert ins.mnemonic in('ret','jmp') and after==0,('Unbalanced normal/tail exit',name,hex(at),after)
                exits.append(dict(site=at,entry_esp_offset=after,kind=ins.mnemonic))
            pending.extend((target,after)for target in nexts)
        assert set(offsets)==set(decoded)and exits
        return dict(entry_esp_relative_offsets=offsets,private_mov_stores=stores,balanced_exits=exits,scope='Own entryESP only; four-byte MOV slots wholly inside currently allocated region below return address')
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
            assert not any(ins.prefix),('Unproved prefixed integer/ABI instruction',hex(at),ins.bytes.hex(),ins.mnemonic,ins.op_str)
            assert not ins.mnemonic.startswith('f') and not any(cs.reg_name(op.reg).startswith(('xmm','mm'))for op in ins.operands if op.type==1),('Unexpected FP/SIMD',hex(at),ins.mnemonic)
            assert ins.mnemonic not in('int','int1','int3','into','ud2','hlt','cli','sti','retf','lret','syscall','sysenter','sysexit','iret','iretd'),('Unexpected trap/privileged control',hex(at),ins.mnemonic)
            assert CS_GRP_JUMP in ins.groups or ins.mnemonic in('mov','lea','cmp','test','push','pop','ret','call','add','sub','inc','dec','and','or','xor','shl','shr','sar','neg','not','imul','cdq','nop','leave','sete','setne','setl','setle','setg','setge'),('Unexpected source integer opcode',hex(at),ins.mnemonic)
            for address in range(at,at+ins.size):assert address not in active;active[address]=at
            decoded[at]=ins
            # Entry ESP is the genuine caller stack. Only full-width constant
            # stack adjustment is admitted; arbitrary/full/partial rebasing,
            # POP ESP and LEAVE are pending rather than misclassified private.
            assert ins.mnemonic!='leave',('Unproved frame-to-ESP restoration',hex(at))
            if ins.operands and ins.operands[0].type==1 and cs.reg_name(ins.operands[0].reg)in('esp','sp','spl'):
                # Comparison/source operands do not write the register.
                if ins.mnemonic not in('cmp','test','push'):
                    assert cs.reg_name(ins.operands[0].reg)=='esp' and ins.operands[0].size==4
                    if ins.mnemonic in('add','sub'):
                        assert len(ins.operands)==2 and ins.operands[1].type==CS_OP_IMM and 0<=ins.operands[1].imm<=4096 and ins.operands[1].imm%4==0,('Unproved ESP arithmetic',hex(at),ins.op_str)
                    elif ins.mnemonic=='lea':
                        op=ins.operands[1];assert op.type==CS_OP_MEM and cs.reg_name(op.mem.base)=='esp' and not op.mem.index and not op.mem.segment and abs(op.mem.disp)<=4096 and op.mem.disp%4==0,('Unproved ESP LEA',hex(at),ins.op_str)
                    else:raise AssertionError(('Unproved ESP register definition',hex(at),ins.mnemonic,ins.op_str))
            for opindex,op in enumerate(ins.operands):
                if op.type==CS_OP_MEM:
                    assert not op.mem.segment and not any(prefix in(0x66,0x67)for prefix in ins.prefix),('Unexpected memory segment/width prefix',hex(at),ins.op_str)
                    if ins.mnemonic=='lea':role='address'
                    elif ins.mnemonic=='mov':role='write'if opindex==0 else'read'
                    elif ins.mnemonic in('cmp','test','push'):role='read'
                    elif ins.mnemonic in('add','sub') and opindex==1 and ins.operands[0].type==1:role='read'
                    elif ins.mnemonic=='pop':role='write'
                    else:raise AssertionError(('Unclassified explicit memory opcode',hex(at),ins.mnemonic,ins.op_str))
                    assert op.size==4,('Unexpected memory operand width',hex(at),op.size)
                    if role=='write':assert ins.mnemonic=='mov' and opindex==0 and cs.reg_name(op.mem.base)=='esp' and not op.mem.index and ins.operands[1].type in(1,CS_OP_IMM),('Unproved explicit store; only MOV own-ESP slot admitted',hex(at),ins.op_str)
                    memory_accesses.append(dict(site=at,operand=opindex,width=op.size,role=role,base=cs.reg_name(op.mem.base),index=cs.reg_name(op.mem.index),scale=op.mem.scale,displacement=op.mem.disp))
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
                target=ins.operands[0].imm;assert name!='FrontendScrollRead' and identity(target)[0]=='FrontendScrollRead',('Unexpected source call edge',name,hex(target));todo.append(target);calls.append(target);edges.append(dict(site=at,target=target,kind='call'));nexts=[at+ins.size]
            elif CS_GRP_JUMP in ins.groups:
                assert ins.operands[0].type==CS_OP_IMM
                target=ins.operands[0].imm
                if ins_root<=target<end:nexts.append(target)
                else:
                    assert ins.mnemonic=='jmp' and name!='FrontendScrollRead' and identity(target)[0]=='FrontendScrollRead',('Unexpected source tail edge',name,hex(target));todo.append(target);edges.append(dict(site=at,target=target,kind='tail-jump'))
                if ins.mnemonic!='jmp':nexts.append(at+ins.size)
            elif ins.mnemonic.startswith('ret'):
                assert ins.mnemonic=='ret'
                expected=0
                assert (ins.operands[0].imm if ins.operands else 0)==expected,(name,ins.op_str,expected)
            else:nexts=[at+ins.size]
            successors[at]=nexts;pending.extend(nexts)
        assert all(target in decoded for nexts in successors.values()for target in nexts)
        stack_provenance=prove_private_stack(decoded,successors,name)
        bodies[ins_root]=dict(name=name,owner=owner,decorated=decorated,address=ins_root,end=end,instruction_count=len(decoded),instructions=[dict(address=i.address,hex=i.bytes.hex(),mnemonic=i.mnemonic,operands=i.op_str)for i in sorted(decoded.values(),key=lambda x:x.address)],direct_calls=sorted(set(calls)),successors=successors,stack_provenance=stack_provenance)
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
    assert len(roots)==3
    blob=words([len(image),3,len(image_refs),low,*[root-low for root in roots]])+image+words(image_refs)
    meta=dict(recompiled_sha256=sha(disk),symbols_sha256=sha(symbols.read_bytes()),map_sha256=sha(mapbytes),roots=roots,original_roots=[hex(a)for _,a,_ in ENTRIES],graph=list(bodies.values()),edges=edges,memory_accesses=memory_accesses,patches=list(patches.values()),constants=[dict(address=at,bytes=n,raw=value.hex())for (at,n),value in constants.items()],globals=list(globals_.values()),low=low,high=high,relocation_directory=[reloc_rva,reloc_size],coff_characteristics=coff,indirect_calls=[],tables=[],map_aliases=[dict(address=a,aliases=sorted(rows),reachable=a in bodies)for a,rows in funcs.items()if len(rows)>1])
    return blob,meta

def project_dependencies():
    # Conservative union of literal project includes across actual three compiler
    # branches. Never guards unrelated project headers or changes source.
    import re
    inc=[ROOT/p for p in('src/msvc500/include','src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/brender/include','src/carma2/s3/include','src/carma2/platform')]
    todo=[ROOT/'src/carma2/newcommon/72-interface.h',ROOT/'src/brender/include/brender/br_defs.h',ROOT/'src/msvc500/include/stdint.h'];seen=set()
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
    relevant=([a.candidate]if a.candidate else[])+[Path(__file__),ROOT/'src/carma2/newcommon/72-interface.c',ROOT/'src/carma2/common/frontend.c',a.original]+[ROOT/'scripts'/s for s in ('verify_phil_registry_reconstruction.py','verify_opponent_navigation_reconstruction.py','verify_opponent_route_reconstruction.py','verify_tinted_native_reconstruction.py')]+project_dependencies()
    if a.source_dir:relevant +=[a.source_dir/n for n in('72-interface.c','72-interface.h')]
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
        caller,_,_=registry.literals();caller=caller.replace('builder_poison;','builder_poison,builder_cdecl;');clear=' cmp builder_mode,8\n jne notwide\n fstp qword ptr builder_wide\n xor eax,eax\n notwide:\n cmp builder_mode,1\n je zeroreturn\n cmp builder_mode,10\n jne resultready\n zeroreturn:\n xor eax,eax\n resultready:';assert clear in caller;caller=caller.replace(clear,'');old=' mov builder_result,eax\n mov ecx,esp';assert old in caller
        caller=caller.replace(old,' mov builder_result,eax\n cmp builder_cdecl,0\n je cachematerial_ready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n cachematerial_ready:\n mov ecx,esp')
        matching=None
        if a.recompiled:
            matching_packet,matching=matching_image(a.recompiled,a.symbols);(out/'matching-code.bin').write_bytes(matching_packet)
        cases=fixtures();assert len(cases)==1553 and sha(b''.join(payload for _,payload in cases))==FROZEN_INPUT_SHA256
        if a.package_only:
            (out/'native.c').write_text(native);(out/'physical.c').write_text(COMMON+PHYSICAL);(out/'caller.c').write_text(caller)
            after={f:sha(Path(f).read_bytes())for f in before};assert before==after
            package=dict(passed=True,package_only=True,scenes=1553,complete_suite=False,runtime_comparisons=0,matching=matching,original=graph,source_before=before,source_after=after,fixture_sha256=[sha(payload)for _,payload in cases],compiler_input_sha256={n:sha((out/n).read_bytes())for n in('native.c','physical.c','caller.c','original-code.bin')},limits=__doc__,live_handles=[])
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
                    row=dict(index=index,compiler=compiler,passed=same,input_sha256=sha(payload),original_output_sha256=sha(expected),actual_output_sha256=sha(r.stdout),normal_return_abi_checked=kind==0,raw_eax_checked=kind==0,exit_code=r.returncode)
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
