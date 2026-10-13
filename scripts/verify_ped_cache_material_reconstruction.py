"""Complete 65B cache /169B material /43B genuine-copy original/native3 proof.
Raw 32KiB public arenas, masked PC53/DF0, authentic FASTCALL RET0/RET12 and
CDECL RET0 callers. Normal ESP/nonvolatile/x87TOP checks; terminal AV compares
first fault address and public prefix only. No callback/control/math substitute.
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
BASE=0x22000000
ARENA=32768
ENTRIES=[('FlushAllPedCaches',0x4d6c70,65),('MungeMaterialSV',0x5074d0,169),('BrMatrix23Copy',0x534ae0,43)]
COMMON=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#define BrMatrix23Copy BrMatrix23CopyHeaderOnly
#include "61-pedestrn.h"
#undef BrMatrix23Copy
#include "brender/br_defs.h"
static unsigned char*arena;static unsigned addresses[3];static unsigned physical_mode;
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison,builder_cdecl;
extern unsigned __cdecl BuilderInvoke(void);
#define gPed_count (*(int*)(arena+0x7000))
#define gPedestrian_array (*(tPedestrian**)(arena+0x7004))
#define A(x,y) A->m[x][y]
#define B(x,y) B->m[x][y]
void C2_HOOK_CDECL BrMatrix23Copy(br_matrix23*,const br_matrix23*);
typedef char layouts[sizeof(void*)==4&&sizeof(tPedestrian)==0x54&&offsetof(tPedestrian,field_0x0c)==0xc&&offsetof(tPed_cache_006944c0,field_0xb0)==0xb0&&offsetof(tPed_face_cache_0x34,field_0x14)==0x14&&offsetof(br_material,map_transform)==0x24&&sizeof(br_matrix23)==24&&sizeof(br_matrix34)==48?1:-1];
static void snapshot(unsigned event,unsigned value,unsigned address){unsigned h[3];h[0]=event;h[1]=value;h[2]=address;fwrite(h,12,1,stdout);fwrite(arena,32768,1,stdout);fflush(stdout);}
static LONG WINAPI observe_fault(EXCEPTION_POINTERS*e){if(e->ExceptionRecord->ExceptionCode!=EXCEPTION_ACCESS_VIOLATION)return EXCEPTION_CONTINUE_SEARCH;snapshot(3,e->ExceptionRecord->ExceptionCode,(unsigned)e->ExceptionRecord->ExceptionInformation[1]);ExitProcess(0);return EXCEPTION_CONTINUE_SEARCH;}
static int run(const char*path){FILE*f;unsigned h[7],mode;FARPROC proc;typedef void*(WINAPI *add_handler)(ULONG,LONG(WINAPI*)(EXCEPTION_POINTERS*));add_handler handler;
SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);f=fopen(path,"rb");if(!f)return 10;arena=VirtualAlloc((void*)0x22000000,32768,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(arena!=(void*)0x22000000)return 11;
if(fread(h,sizeof(h),1,f)!=1||fread(arena,32768,1,f)!=1)return 12;fclose(f);_setmode(_fileno(stdout),_O_BINARY);
proc=GetProcAddress(GetModuleHandleA("kernel32.dll"),"AddVectoredExceptionHandler");memcpy(&handler,&proc,4);if(!handler||!handler(1,observe_fault))return 13;mode=h[0];if(mode>2)return 15;
builder_address=addresses[mode];builder_mode=0;builder_poison=h[6];builder_x=h[1];builder_y=h[2];
#if defined(REC2_MATCHING)
builder_cdecl=0;
#else
builder_cdecl=1;
#endif
if(mode==0){builder_stack_count=0;}else if(mode==1){builder_stack_count=3;builder_stack[0]=h[3];builder_stack[1]=h[4];builder_stack[2]=h[5];if(builder_cdecl){builder_stack_count=5;builder_stack[0]=h[1];builder_stack[1]=h[2];builder_stack[2]=h[3];builder_stack[3]=h[4];builder_stack[4]=h[5];}}
else{builder_stack_count=2;builder_stack[0]=h[1];builder_stack[1]=h[2];builder_cdecl=1;}
_controlfp(_PC_53,_MCW_PC);_clearfp();if(BuilderInvoke())return 14;snapshot(0,0,0);return 0;}
'''
PHYSICAL=r'''
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned h[6],i,offset,value;unsigned char*image;
if(argc!=3)return 1;f=fopen(argv[2],"rb");if(!f)return 2;if(fread(h,sizeof(h),1,f)!=1||h[1]>=h[0]||h[2]>=h[0]||h[3]>=h[0])return 3;
image=VirtualAlloc(NULL,h[0],MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image||fread(image,h[0],1,f)!=1)return 4;
for(i=0;i<h[4];i++){if(fread(&offset,4,1,f)!=1||offset>h[0]-4)return 5;memcpy(&value,image+offset,4);value=(unsigned)image+value-h[5];memcpy(image+offset,&value,4);}fclose(f);
FlushInstructionCache(GetCurrentProcess(),image,h[0]);for(i=0;i<3;i++)addresses[i]=(unsigned)image+h[i+1];physical_mode=1;return run(argv[1]);}
'''

def original_image(binary):
    cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
    starts={a for _,a,_ in ENTRIES};low=min(starts);constant=0x58b1cc
    assert raw(binary,constant,4)==words([0x40c00000])
    high=constant+4;image=bytearray(b'\xcc'*(high-low));relocs=[];patches=[];bodies=[]
    for name,a,n in ENTRIES:
        code=raw(binary,a,n);image[a-low:a-low+n]=code;ins=list(cs.disasm(code,a));assert sum(i.size for i in ins)==n
        todo=[a];seen={};occupied={};calls=[]
        while todo:
            at=todo.pop()
            while at not in seen:
                assert a<=at<a+n
                i=next(cs.disasm(raw(binary,at,min(15,a+n-at)),at,count=1));assert at+i.size<=a+n
                for q in range(at,at+i.size):assert q not in occupied;occupied[q]=at
                seen[at]=i
                if CS_GRP_CALL in i.groups:assert i.operands[0].type==CS_OP_IMM and i.operands[0].imm in starts;calls.append(dict(site=at,target=i.operands[0].imm))
                for op in i.operands:
                    if op.type==CS_OP_MEM and not op.mem.base and not op.mem.index:
                        assert i.disp_size==4
                        if op.mem.disp==constant:
                            assert op.size==4;relocs.append(at+i.disp_offset-low);target=constant
                        else:
                            assert a==0x4d6c70 and op.size==4 and op.mem.disp in (0x7447d4,0x744808)
                            target=BASE+(0x7000 if op.mem.disp==0x7447d4 else 0x7004)
                            struct.pack_into('<I',image,at+i.disp_offset-low,target)
                        patches.append(dict(site=at,instruction=i.bytes.hex(),operand_offset=i.disp_offset,original=op.mem.disp,target=target,width=op.size))
                if CS_GRP_JUMP in i.groups:
                    assert i.operands[0].type==CS_OP_IMM and a<=i.operands[0].imm<a+n
                    todo.append(i.operands[0].imm)
                    if i.mnemonic=='jmp':break
                if i.mnemonic.startswith('ret'):
                    assert (not i.operands) if a!=0x5074d0 else (i.operands[0].imm==12)
                    break
                at+=i.size
        assert len(occupied)==n
        bodies.append(dict(name=name,address=a,bytes=n,sha256=sha(code),reachable_bytes=len(occupied),instructions=[dict(address=i.address,bytes=i.bytes.hex(),instruction=i.mnemonic+' '+i.op_str)for i in sorted(seen.values(),key=lambda i:i.address)],calls=calls))
    image[constant-low:constant-low+4]=raw(binary,constant,4)
    packet=words([len(image),*[a-low for _,a,_ in ENTRIES],len(relocs),low])+image+words(relocs)
    return packet,dict(bodies=bodies,total_original_bytes=277,patches=patches,constant=dict(address=constant,raw=raw(binary,constant,4).hex()),low=low,high=high,absolute_image_relocations=relocs,indirect_calls=[],tables=[])

def fixtures():
    cases=[];rng=random.Random(0x5074d0)
    def add(mode,label,x=BASE+0x1000,y=BASE+0x2000,z=BASE+0x3000,a0=0,a1=1,count=3,array=BASE+0x100,patches=(),rows=None):
        arena=bytearray(ARENA)
        # Raw, initialized canaries preserve observations without large random arenas.
        for at in range(0,ARENA,256):struct.pack_into('<I',arena,at,(at*73+len(cases))&0xffffffff)
        def put(at,v):struct.pack_into('<I',arena,at,v&0xffffffff)
        for j in range(8):
            put(0x10c+j*0x54,BASE+0x4000+j*0x200)
            put(0x40b0+j*0x200,BASE+0x5000+j*0x80)
            put(0x5014+j*0x80,BASE+0x100+j*0x54)
        put(0x7000,count);put(0x7004,array)
        struct.pack_into('<12f',arena,0x1000,*([1.,2.,3.,-4.,5.,-6.,7.,-8.,9.,10.,11.,12.]if rows is None else rows))
        for at in (0x2024,0x3024):struct.pack_into('<6I',arena,at,0x7f800001,0x80000000,0x3f800000,0xff800002,1,0xffc12345)
        for at,value in patches:put(at,value)
        payload=words([mode,x,y,z,a0,a1,0xcccccccc if len(cases)%2 else 0x7f800001])+arena
        cases.append((dict(index=len(cases),mode=mode,label=label,x=x,y=y,z=z,axis0=a0&0xffffffff,axis1=a1&0xffffffff,count=count&0xffffffff,array=array,patches=patches),payload))
    for count in (-2147483648,-3,-1,0,1,2,3,4,8):
        for array in (0,BASE+0x100,BASE+0x101):
            # An unaligned row needs its pointer initialized at that same address.
            # The historical aligned seed shifted to an unmanaged host address.
            patches=[(0x10d+j*0x54,BASE+0x4000+j*0x200)for j in range(count)]if array==BASE+0x101 and count>0 else[]
            add(0,'initial-count-and-array',count=count,array=array,patches=patches)
    for n in range(8):
        for kind in (0,1,2):
            patches=[(0x10c+n*0x54,0)]if kind==0 else[(0x40b0+n*0x200,0)]if kind==1 else[(0x40b0+n*0x200,BASE+0x5000)]
            add(0,'null-object-null-cache-shared-cache',count=8,patches=patches)
    # First store can mutate the live count, captured global array, or row identity.
    for target in (0x7000,0x7004,0x10c,0x160,0x40b0,0x42b0):add(0,'cache-store-live-alias',patches=[(0x40b0,BASE+target-0x14)])
    # Object-cache clearing can change the count or replace the next row.
    for target in (0x7000,0x7004,0x160,0x1b4):
        obj=BASE+target-0xb0
        add(0,'object-cache-store-live-alias',patches=[(0x10c,obj),(target,BASE+0x5000)])
    for delta in range(-7,8):
        add(0,'partial-row-alias',count=1,patches=[(0x40b0,BASE+0x10c+delta-0x14)])
        add(0,'partial-count-alias',patches=[(0x40b0,BASE+0x7000+delta-0x14)])
    for at in (0,BASE+ARENA-3,BASE+ARENA-4,BASE+ARENA-12,BASE+ARENA-16):add(0,'array-first-fault',array=at,count=1)
    for obj in (0xfffffff0,BASE+ARENA-0xb0,BASE+ARENA-0xb4,BASE+ARENA-0xb3):add(0,'object-first-fault',count=1,patches=[(0x10c,obj)])
    for cache in (0xfffffff0,BASE+ARENA-0x14,BASE+ARENA-0x18,BASE+ARENA-0x17):add(0,'cache-store-first-fault',count=1,patches=[(0x40b0,cache)])
    for k in range(64):
        count=rng.randrange(9);patches=[]
        for j in range(count):
            if rng.randrange(4)==0:patches.append((0x10c+j*0x54,0))
            elif rng.randrange(4)==0:patches.append((0x40b0+j*0x200,0))
        add(0,'bounded-random-cache-layout',count=count,patches=patches)
    for a0 in range(4):
        for a1 in range(4):add(1,'axis-pairs',a0=a0,a1=a1)
    for k in range(120):add(1,'finite-scale',rows=[rng.uniform(-2,2)*2.**rng.randrange(-125,125)for _ in range(12)],a0=k%4,a1=(k//4)%4)
    special=[0,0x80000000,1,0x80000001,0x007fffff,0x00800000,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7f800001,0xff800002,0x7fc12345,0xffc54321]
    for word in range(12):
        for value in special:add(1,'single-row-IEEE',a0=word//3,a1=(word//3+1)%4,patches=[(0x1000+4*word,value)])
    for row in range(4):
        for pair in [(a,b)for a in special[-4:]for b in special[-4:]]:
            for lanes in((0,1),(0,2),(1,2)):add(1,'paired-row-NaNs',a0=row,a1=row,patches=[(0x1000+12*row+4*lanes[0],pair[0]),(0x1000+12*row+4*lanes[1],pair[1])])
    for delta in range(-23,24):
        add(1,'material-copy-partial-overlap',z=BASE+0x2000+delta)
        add(1,'first-store-alters-second-row',y=BASE+0x100c+delta-0x24,a0=0,a1=1)
        add(1,'second-store-alters-copy-source',z=BASE+0x1000+delta-0x24,patches=[(0x1000,0x7f800001),(0x1010,0xffc54321)])
    for delta in (1,2,3):add(1,'unaligned-matrix-materials',x=BASE+0x1000+delta,y=BASE+0x2000+delta,z=BASE+0x3000+delta)
    for axis in (-2147483648,-1,4,5,0x15555555,0x40000000,0x7fffffff,-715827883):
        add(1,'axis-raw-modulo-addressing',a0=axis);add(1,'second-axis-raw-modulo-addressing',a1=axis)
    for pointer in (0,0xfffffff8,BASE+ARENA-4,BASE+ARENA-8,BASE+ARENA-12):add(1,'matrix-first-fault',x=pointer)
    for pointer in (0,0xfffffff0,BASE+ARENA-0x24,BASE+ARENA-0x28,BASE+ARENA-0x30,BASE+ARENA-0x34):
        add(1,'first-material-fault',y=pointer);add(1,'final-copy-destination-fault',z=pointer)
    for x,y,z in((0,0,0),(BASE+ARENA-8,0,0),(BASE+ARENA-4,BASE+ARENA-0x24,0),(BASE+0x1000,BASE+ARENA-0x34,0)):add(1,'competing-source-output-fault',x=x,y=y,z=z)
    for delta in range(-23,24):
        add(2,'direct-copy-overlap',x=BASE+0x2024+delta,y=BASE+0x2024)
    for word in range(6):
        for bits in special:add(2,'direct-copy-IEEE',x=BASE+0x3024,y=BASE+0x2024,patches=[(0x2024+word*4,bits)])
    for pointer in (0,0xfffffff8,BASE+ARENA-4,BASE+ARENA-8,BASE+ARENA-12,BASE+ARENA-16,BASE+ARENA-20,BASE+ARENA-24):
        add(2,'direct-copy-output-fault',x=pointer,y=BASE+0x2024);add(2,'direct-copy-input-fault',x=BASE+0x3024,y=pointer)
    # Appended qualifiers preserve all original992 indices and bytes.
    for row in range(4):
        for lanes in ((0,1),(0,2),(1,2)):
            for pair in ((0x7fc12345,0xffc12345),(0xffc12345,0x7fc12345),(0x7f812345,0xff812345),(0xff812345,0x7f812345)):
                add(1,'equal-payload-opposite-sign-NaNs',a0=row,a1=row,patches=[(0x1000+row*12+lanes[0]*4,pair[0]),(0x1000+row*12+lanes[1]*4,pair[1])])
    for x,y in ((0,BASE+ARENA-4),(0,BASE+ARENA-8),(BASE+ARENA-4,0),(BASE+ARENA-8,BASE+ARENA-4),(BASE+ARENA-4,BASE+ARENA-8)):
        add(2,'copy-competing-source-output-fault',x=x,y=y)
    for a,b,c in ((2.**24,1.,1.),(2.**27,2.**-1,1.),(2.**-125,2.**-126,2.**-127),(16777215.,16777214.,1.),(2.**60,2.**35,2.**34)):
        for row in range(4):
            values=[0.]*12;values[row*3:row*3+3]=[a,b,c];add(1,'directed-row-sum-rounding',rows=values,a0=row,a1=row)
    return cases

def production_capsule(source_dir=None):
    directory=source_dir or ROOT/'src/carma2/newcommon'
    ped=(directory/'61-pedestrn.c').read_text();world=(directory/'11-world1.c').read_text()
    matrix=(source_dir/'matrix23.c'if source_dir else ROOT/'src/brender/core/math/matrix23.c').read_text()
    import re
    assert re.search(r'// GLOBAL: CARMA2_HW 0x007447d4\s+int gPed_count;',ped)
    assert re.search(r'// GLOBAL: CARMA2_HW 0x00744808\s+tPedestrian\* gPedestrian_array;',ped)
    header=(directory/'61-pedestrn.h').read_text()
    assert 'extern int gPed_count;'in header and 'extern void C2_HOOK_FASTCALL FlushAllPedCaches(void);'in header
    header=(directory/'11-world1.h').read_text()
    assert 'extern void C2_HOOK_FASTCALL MungeMaterialSV(br_matrix34* matrix, br_material* material1, br_material* material2, int axis0, int axis1);'in header
    alias='#if defined(__GNUC__)\ntypedef tU32 tPedCacheWord __attribute__((__may_alias__));\n#else\ntypedef tU32 tPedCacheWord;\n#endif\n'
    assert alias.rstrip('\n')in ped
    body=alias+'\n'.join(extract(ped,n)for n in('PedCacheRead','PedCacheWrite','FlushAllPedCaches'))
    physics=(ROOT/'src/carma2/newcommon/49-physics.c').read_text()
    for name in('PhysicsWallNaN','PhysicsJointSum'):assert extract(world,name)==extract(physics,name)
    body+='\n'+'\n'.join(extract(world,n)for n in('PhysicsWallNaN','PhysicsJointSum','SpecVolSum','SpecVolRowLength','MungeMaterialSV'))
    body+='\n'+extract(matrix,'BrMatrix23Copy')
    return body

def candidate_capsule(candidate,copy_source):
    source=candidate.read_text()
    body='#if defined(__GNUC__)\ntypedef tU32 tPedCacheWord __attribute__((__may_alias__));\n#else\ntypedef tU32 tPedCacheWord;\n#endif\n'
    names=['PedCacheRead','PedCacheWrite','FlushAllPedCaches','PhysicsWallNaN','PhysicsJointSum','SpecVolSum','SpecVolRowLength','MungeMaterialSV']
    body+='\n'.join(extract(source,n)for n in names)
    return body+'\n'+extract(copy_source.read_text(),'BrMatrix23Copy')

def matching_image(exe, symbols):
    from verify_opponent_route_reconstruction import pe
    """Exact whole-CFG transport; no indirect calls, imports, tables or substitutes."""
    disk=exe.read_bytes();mapbytes=exe.with_suffix('.map').read_bytes();mapping=json.loads(symbols.read_text())
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
    owners={'FlushAllPedCaches':('61-pedestrn.c.obj','@FlushAllPedCaches@0'),
            'PedCacheRead':('61-pedestrn.c.obj','_PedCacheRead'),
            'PedCacheWrite':('61-pedestrn.c.obj','_PedCacheWrite'),
            'MungeMaterialSV':('11-world1.c.obj','@MungeMaterialSV@20'),
            'SpecVolRowLength':('11-world1.c.obj','_SpecVolRowLength'),
            'SpecVolSum':('11-world1.c.obj','_SpecVolSum'),
            'PhysicsWallNaN':('11-world1.c.obj','@PhysicsWallNaN@8'),
            'PhysicsJointSum':('11-world1.c.obj','_PhysicsJointSum'),
            'BrMatrix23Copy':('libbrender:matrix23.c.obj','_BrMatrix23Copy')}
    def identity(address):
        assert address in funcs
        aliases=funcs[address];assert len(aliases)==1,('Ambiguous reached MAP identity',hex(address),sorted(aliases))
        decorated,owner=next(iter(aliases));name=decorated.lstrip('_@').split('@')[0]
        assert name in owners and (owner,decorated)==owners[name],('Unexpected complete callee',hex(address),decorated,owner)
        return name,owner,decorated
    roots=[mapped[a]for _,a,_ in ENTRIES]
    for root,(name,_,_)in zip(roots,ENTRIES):assert identity(root)[0]==name
    globals_={}
    for name,orig,arena_at in [('gPed_count',0x7447d4,0x7000),('gPedestrian_array',0x744808,0x7004)]:
        entries=data.get('_'+name,set());assert len(entries)==1,(name,entries)
        address,owner=next(iter(entries));assert owner=='<common>'and mapped[orig]==address,(name,owner,address,mapped[orig])
        globals_[address]=dict(name=name,original=orig,address=address,bytes=4,target=BASE+arena_at,owner=owner)
    nt,base,image_size,tab=pe(disk);sections=[]
    for index in range(struct.unpack_from('<H',disk,nt+6)[0]):
        at=tab+index*40;virtual_size,rva,raw_size,file=struct.unpack_from('<IIII',disk,at+8)
        sections.append(dict(start=base+rva,end=base+rva+max(virtual_size,raw_size),flags=struct.unpack_from('<I',disk,at+36)[0],name=disk[at:at+8].split(b'\0')[0].decode()))
    def section(address,n):
        rows=[s for s in sections if s['start']<=address and address+n<=s['end']]
        assert len(rows)==1,('PE section span',hex(address),n)
        return rows[0]
    for address,row in globals_.items():
        s=section(address,4);assert s['flags']&0x80000000 and s['flags']&0x40000000 and not s['flags']&0x20000000
    starts=sorted(funcs);todo=list(roots);bodies={};active={};patches={};constants={};edges=[]
    cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
    def transport(site,old,width,kind,ins):
        if any(address<=old and old+width<=address+4 for address in globals_):
            address=next(address for address in globals_ if address<=old and old+width<=address+4)
            assert identity(ins_root)[0]in ('FlushAllPedCaches','PedCacheRead','PedCacheWrite'),('Mutable global outside cache closure',hex(site))
            target=globals_[address]['target']+old-address;mode='arena';authority=globals_[address]['name']
        else:
            assert kind=='memory'and width in(4,8)and identity(ins_root)[0]=='MungeMaterialSV'
            s=section(old,width);assert s['flags']&0x40000000 and not s['flags']&(0x80000000|0x20000000)
            value=raw(disk,old,width);expected=struct.pack('<f'if width==4 else'<d',6.)
            assert value==expected,('Unexpected readonly scalar',hex(site),hex(old),value.hex())
            constants[(old,width)]=value;target=old;mode='image';authority='exact readonly scalar6'
        row=dict(site=site,original_value=old,target=target,mode=mode,width=width,authority=authority,instruction_address=ins.address,instruction_hex=ins.bytes.hex())
        if site in patches:assert patches[site]==row
        patches[site]=row
    while todo:
        ins_root=todo.pop()
        if ins_root in bodies:continue
        name,owner,decorated=identity(ins_root);end=next(a for a in starts if a>ins_root)
        pending=[ins_root];decoded={};successors={};calls=[]
        while pending:
            at=pending.pop()
            if at in decoded:continue
            assert ins_root<=at<end
            ins=next(cs.disasm(raw(disk,at,min(15,end-at)),at,count=1));assert at+ins.size<=end
            s=section(at,ins.size);assert s['flags']&0x20 and s['flags']&0x20000000
            for address in range(at,at+ins.size):assert address not in active;active[address]=at
            decoded[at]=ins
            for op in ins.operands:
                if op.type==CS_OP_MEM and not op.mem.base:
                    assert not op.mem.segment,('Unclassified explicit segment',hex(at),ins.op_str)
                    assert not op.mem.index,('Unclassified absolute table/index',hex(at),ins.op_str)
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
                expected=12 if name=='MungeMaterialSV'else 8 if name=='PhysicsWallNaN'else 0
                assert (ins.operands[0].imm if ins.operands else 0)==expected,(name,ins.op_str,expected)
            else:nexts=[at+ins.size]
            successors[at]=nexts;pending.extend(nexts)
        assert all(target in decoded for nexts in successors.values()for target in nexts)
        bodies[ins_root]=dict(name=name,owner=owner,decorated=decorated,address=ins_root,end=end,instruction_count=len(decoded),instructions=[dict(address=i.address,hex=i.bytes.hex(),mnemonic=i.mnemonic,operands=i.op_str)for i in sorted(decoded.values(),key=lambda x:x.address)],direct_calls=sorted(set(calls)),successors=successors)
    # Root1 itself must reach the genuine copy lower, independent of direct-copy fixtures.
    reach={roots[1]};pending=[roots[1]]
    while pending:
        current=pending.pop()
        for edge in edges:
            if bodies[current]['address']<=edge['site']<bodies[current]['end']and edge['target']not in reach:reach.add(edge['target']);pending.append(edge['target'])
    assert roots[2]in reach,('Material root does not reach authentic copy',reach)
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
    blob=words([len(image),*[root-low for root in roots],len(image_refs),low])+image+words(image_refs)
    meta=dict(recompiled_sha256=sha(disk),symbols_sha256=sha(symbols.read_bytes()),map_sha256=sha(mapbytes),roots=roots,original_roots=[hex(a)for _,a,_ in ENTRIES],graph=list(bodies.values()),edges=edges,patches=list(patches.values()),constants=[dict(address=at,bytes=n,raw=value.hex())for (at,n),value in constants.items()],globals=list(globals_.values()),low=low,high=high,relocation_directory=[reloc_rva,reloc_size],coff_characteristics=coff,indirect_calls=[],tables=[],root_material_reaches_copy=True,map_aliases=[dict(address=a,aliases=sorted(rows),reachable=a in bodies)for a,rows in funcs.items()if len(rows)>1])
    return blob,meta


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True);p.add_argument('--candidate',type=Path);p.add_argument('--copy-source',type=Path);p.add_argument('--source-dir',type=Path);p.add_argument('--package-only',action='store_true');p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);p.add_argument('--case-index',type=int,action='append');p.add_argument('--limit',type=int);a=p.parse_args()
    assert bool(a.recompiled)==bool(a.symbols);assert bool(a.candidate)==bool(a.copy_source);assert not(a.recompiled and(a.candidate or a.source_dir));assert not a.source_dir or a.package_only
    out=a.output.resolve();assert not out.exists();out.mkdir(parents=True)
    relevant=([a.candidate,a.copy_source]if a.candidate else[])+[Path(__file__),ROOT/'src/carma2/newcommon/61-pedestrn.c',ROOT/'src/carma2/newcommon/11-world1.c',ROOT/'src/carma2/newcommon/49-physics.c',ROOT/'src/brender/core/math/matrix23.c',ROOT/'src/carma2/common/pedestrn.c',ROOT/'build-reference/originals/CARMA2_HW.EXE']+[ROOT/'scripts'/s for s in ('verify_phil_registry_reconstruction.py','verify_opponent_navigation_reconstruction.py','verify_opponent_route_reconstruction.py','verify_tinted_native_reconstruction.py')]+list((ROOT/'src').rglob('*.h'))
    if a.source_dir:relevant +=[a.source_dir/n for n in('61-pedestrn.c','61-pedestrn.h','11-world1.c','11-world1.h','matrix23.c')]
    if a.recompiled:relevant +=[a.recompiled,a.symbols,a.recompiled.with_suffix('.map')]
    before={str(f.resolve()):sha(f.read_bytes())for f in relevant};(out/'source-before.json').write_text(json.dumps(before,indent=2)+'\n');(out/'verifier-snapshot.py').write_bytes(Path(__file__).read_bytes())
    if a.candidate:(out/'candidate-snapshot.c').write_bytes(a.candidate.read_bytes());(out/'copy-source-snapshot.c').write_bytes(a.copy_source.read_bytes())
    snapshots={};(out/'sources').mkdir()
    for index,f in enumerate(dict.fromkeys(relevant)):
        if f.suffix in ('.c','.h','.py'):
            dest=out/'sources'/f'{index}-{f.name}';dest.write_bytes(f.read_bytes());snapshots[str(f.resolve())]=str(dest.resolve())
    (out/'source-snapshots.json').write_text(json.dumps(snapshots,indent=2)+'\n')
    try:
        binary=(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes();assert sha(binary)==HASH;packet,graph=original_image(binary);(out/'original-code.bin').write_bytes(packet)
        body=candidate_capsule(a.candidate,a.copy_source)if a.candidate else production_capsule(a.source_dir)
        native=COMMON+'\n'+body+'\nint C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;addresses[0]=(unsigned)FlushAllPedCaches;addresses[1]=(unsigned)MungeMaterialSV;addresses[2]=(unsigned)BrMatrix23Copy;physical_mode=0;return run(argv[1]);}\n'
        caller,_,_=registry.literals();caller=caller.replace('builder_poison;','builder_poison,builder_cdecl;');old=' mov builder_result,eax\n mov ecx,esp';assert old in caller
        caller=caller.replace(old,' xor eax,eax\n mov builder_result,eax\n cmp builder_cdecl,0\n je cachematerial_ready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n cachematerial_ready:\n mov ecx,esp')
        matching=None
        if a.recompiled:
            matching_packet,matching=matching_image(a.recompiled,a.symbols);(out/'matching-code.bin').write_bytes(matching_packet)
        cases=fixtures();assert len(cases)==1065 and sha(b''.join(payload for _,payload in cases))==FROZEN1065_INPUT_SHA256
        if a.package_only:
            (out/'native.c').write_text(native);(out/'physical.c').write_text(COMMON+PHYSICAL);(out/'caller.c').write_text(caller)
            after={f:sha(Path(f).read_bytes())for f in before};assert before==after
            package=dict(passed=True,package_only=True,scenes=1065,complete_suite=False,runtime_comparisons=0,matching=matching,original=graph,source_before=before,source_after=after,fixture_sha256=[sha(payload)for _,payload in cases],compiler_input_sha256={n:sha((out/n).read_bytes())for n in('native.c','physical.c','caller.c','original-code.bin')},limits=__doc__,live_handles=[])
            (out/'package.json').write_text(json.dumps(package,indent=2)+'\n');print('PACKAGE ONLY PASS',len(cases));return
        registry.COMMON=COMMON;commands=registry.compile_sources(out,native,PHYSICAL,caller)
        cases=fixtures();indices=a.case_index or list(range(len(cases)));indices=indices[:a.limit]if a.limit else indices
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
                    row=dict(index=index,compiler=compiler,passed=same,input_sha256=sha(payload),original_output_sha256=sha(expected),actual_output_sha256=sha(r.stdout),normal_return_abi_checked=kind==0,exit_code=r.returncode)
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
FROZEN1065_INPUT_SHA256='47d88229d5b7159d8d5e225e4fd254616cc8cf9a2ef58a4d1725d47a9030e625'
if __name__=='__main__':main()
