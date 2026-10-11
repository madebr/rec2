#!/usr/bin/env python3
"""Compare replay session writers and traversal with the physical original.

No internal recording functions are mocked. Fixtures compare complete local/ring
memory, pointer/count globals, callback packet bytes, and callback snapshots.
FatalError and variable-length callbacks are the only controlled boundaries.
The physical driver executes only audited function bodies and the original
stack probe, never the game entry point. Public scalar bytes are not normalized.
Fixtures use masked exceptions and PC53; x87 exception flags are not compared.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import struct
import subprocess
import sys

ROOT=Path(__file__).resolve().parents[1]
SOURCE=ROOT/'src/carma2/newcommon/23-piping.c'
ORIGINAL_HASH='9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
ADDRESSES=dict(ARStartPipingSession=0x4025e0,EndPipingSession2=0x402660,
    AREndPipingSession=0x402890,ARAddDataToSession=0x4028a0,
    ARAddVariedDataToSession=0x402930,LengthOfSession=0x403750,
    ARDoSingleSession=0x402ab0,ARDoSingleVariedSession=0x402bd0,
    ARAdvanceChunkPtr=0x4024e0,ARPipeSearchForwards=0x402540,
    ARReplayForwards=0x402300,ARGetStartTime=0x403020,
    gPipe_chunk_end=0x6768c8,gDisable_advance=0x67690c,
    gPipe_play_ptr=0x6768b8,gPlay_direction=0x6768c0,gReplay_rate=0x676900,
    gPipe_buffer_start=0x676908,
    gReentrancy_count=0x6768e4,gReentrancy_array=0x6768d0,
    gLocal_buffer=0x6768b4,gLocal_buffer_size=0x6768f4,gMr_chunky=0x6768a0,
    gPipe_callbacks=0x6768e8,gPipe_buffer_phys_end=0x6768f0,
    gPipe_buffer_working_end=0x6768c4,gPipe_buffer_oldest=0x6768ac,
    gPipe_record_ptr=0x6768bc,gAction_replay_mode=0x676914,
    gProgram_state=0x75bb80,FatalError=0x44c230)
POINTERS=['gPipe_buffer_start','gLocal_buffer','gMr_chunky','gPipe_callbacks',
    'gPipe_buffer_phys_end','gPipe_buffer_working_end','gPipe_buffer_oldest','gPipe_record_ptr']
HEAP,STACK,STOP,CB1,CB2,SIZE=0x1000000,0x1080000,0x1081000,0x1082000,0x1082010,0x11000
SPANS = [(0x4025e0,119),(0x402660,555),(0x402890,10),(0x4028a0,140),
    (0x402930,376),(0x403750,103),(0x402ab0,273),(0x402bd0,503),
    (0x4024e0,87),(0x402540,93),(0x402300,93),(0x403020,164),(0x575bb0,47)]
GLOBAL_TYPES = {
    **{n:'tU8*' for n in POINTERS if n!='gPipe_callbacks'},
    'gPipe_callbacks':'const tReplay_callback*','gPipe_chunk_end':'tU8*',
    'gPipe_play_ptr':'tU8*','gDisable_advance':'int','gPlay_direction':'int',
    'gReplay_rate':'float','gLocal_buffer_size':'tU32','gReentrancy_count':'int',
    'gAction_replay_mode':'int','gProgram_state':'tProgram_state',
}


def raw(data, address, length):
    nt=struct.unpack_from('<I',data,60)[0]
    table=nt+24+struct.unpack_from('<H',data,nt+20)[0]
    for i in range(struct.unpack_from('<H',data,nt+6)[0]):
        _,rva,size,off=struct.unpack_from('<IIII',data,table+40*i+8)
        if rva<=address-0x400000 and address-0x400000+length<=rva+size:
            return data[off+address-0x400000-rva:off+address-0x400000-rva+length]
    raise ValueError(hex(address))


def address_file(path,data,addresses,spans):
    from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM,CS_OP_REG
    cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
    patches={};closure=[]
    code={a for a,_ in spans}|{addresses['FatalError']}
    ranges=[(addresses[n],addresses[n]+(0x4000 if n=='gProgram_state' else 4)) for n in GLOBAL_TYPES]
    ranges.append((addresses['gReentrancy_array']-4,addresses['gReentrancy_array']+20))
    for address,length in spans:
        body=raw(data,address,length);decoded=list(cs.disasm(body,address))
        assert sum(i.size for i in decoded)==length
        closure.append(dict(address=address,length=length,sha256=hashlib.sha256(body).hexdigest()))
        for ins in decoded:
            if ins.mnemonic=='call' or ins.mnemonic.startswith('j'):
                op=ins.operands[0]
                if op.type==CS_OP_IMM:
                    assert op.imm in code or (ins.mnemonic.startswith('j') and address<=op.imm<address+length),(hex(ins.address),ins.op_str)
                else:
                    # All original indirect calls are calc_length in EAX. Linked
                    # compilers may call that field directly from the table.
                    assert ins.mnemonic=='call' and (op.type==CS_OP_REG or (op.type==CS_OP_MEM and op.mem.base)),(hex(ins.address),ins.op_str)
            for op in ins.operands:
                if op.type==CS_OP_MEM and not op.mem.base and op.mem.disp>=0x400000:
                    allowed=any(lo<=op.mem.disp<hi for lo,hi in ranges)
                    if not allowed:
                        assert ins.mnemonic in ('fld','fcomp','fcom') and raw(data,op.mem.disp,op.size)==bytes(op.size),(hex(ins.address),ins.op_str)
                    assert ins.disp_size==4
                    patches[ins.address+ins.disp_offset-0x400000]=op.mem.disp
    patches=sorted(patches.items())
    path.write_bytes(struct.pack('<'+str(len(ADDRESSES)+1)+'I',*[addresses[n] for n in ADDRESSES],len(patches))+b''.join(struct.pack('<2I',*v) for v in patches))
    return dict(closure=closure,absolute_data_relocations=patches)


PHYSICAL_LOADER=r'''
static unsigned save_stack,save_result,save_top,save_cleanup;
static unsigned __declspec(naked) __cdecl checked(unsigned fn,unsigned cx,unsigned dx,unsigned*words,unsigned count,unsigned cleanup) {
    __asm {
        push ebp
        push ebx
        push esi
        push edi
        mov save_stack,esp
        fnstsw ax
        and eax,3800h
        mov save_top,eax
        mov eax,[esp+40]
        mov save_cleanup,eax
        mov esi,[esp+32]
        mov ecx,[esp+36]
    push_words:
        test ecx,ecx
        jz prepare_call
        dec ecx
        push dword ptr [esi+ecx*4]
        jmp push_words
    prepare_call:
        mov eax,save_stack
        mov ecx,[eax+24]
        mov edx,[eax+28]
        mov eax,[eax+20]
        mov ebp,13579bdfh
        mov ebx,2468ace0h
        mov esi,11223344h
        mov edi,55667788h
        call eax
        mov save_result,eax
        add esp,save_cleanup
        mov ecx,esp
        mov esp,save_stack
        cmp ecx,esp
        jne failed
        cmp ebp,13579bdfh
        jne failed
        cmp ebx,2468ace0h
        jne failed
        cmp esi,11223344h
        jne failed
        cmp edi,55667788h
        jne failed
        fnstsw ax
        and eax,3800h
        cmp eax,save_top
        jne failed
        mov eax,save_result
        jmp done
    failed:
        push 90
        call exit
    done:
        pop edi
        pop esi
        pop ebx
        pop ebp
        ret
    }
}
static int load_image(const char*file,const char*metadata) {
    FILE*f;unsigned char*disk,*image;unsigned size,i,count,*patches,value,delta;
    IMAGE_NT_HEADERS32*nt;IMAGE_SECTION_HEADER*sections;
    f=fopen(metadata,"rb");if(!f)return 3;
    if(fread(addresses,sizeof(addresses),1,f)!=1||fread(&count,4,1,f)!=1)return 4;
    patches=(unsigned*)malloc(count*8);if(!patches||fread(patches,8,count,f)!=count)return 4;fclose(f);
    f=fopen(file,"rb");if(!f)return 5;fseek(f,0,SEEK_END);size=(unsigned)ftell(f);rewind(f);
    disk=(unsigned char*)malloc(size);if(!disk||fread(disk,size,1,f)!=1)return 6;fclose(f);
    nt=(IMAGE_NT_HEADERS32*)(disk+((IMAGE_DOS_HEADER*)disk)->e_lfanew);
    image=(unsigned char*)VirtualAlloc(NULL,nt->OptionalHeader.SizeOfImage,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);
    if(!image)return 7;sections=IMAGE_FIRST_SECTION(nt);
    for(i=0;i<nt->FileHeader.NumberOfSections;i++)memcpy(image+sections[i].VirtualAddress,disk+sections[i].PointerToRawData,sections[i].SizeOfRawData);
    delta=(unsigned)(uintptr_t)image-nt->OptionalHeader.ImageBase;
    for(i=0;i<count;i++){
        if(patches[2*i]+4>nt->OptionalHeader.SizeOfImage)return 9;
        memcpy(&value,image+patches[2*i],4);if(value!=patches[2*i+1])return 10;
        value+=delta;memcpy(image+patches[2*i],&value,4);
    }
    for(i=0;i<sizeof(addresses)/sizeof(addresses[0]);i++)addresses[i]+=delta;
    /* Only FatalError is replaced; all internal replay bodies remain intact. */
    {unsigned char*site=(unsigned char*)(uintptr_t)addresses[FATAL_INDEX];site[0]=0xe9;value=(unsigned)(uintptr_t)FatalError-((unsigned)(uintptr_t)site+5);memcpy(site+1,&value,4);}
    FlushInstructionCache(GetCurrentProcess(),image,nt->OptionalHeader.SizeOfImage);
    free(disk);free(patches);return 0;
}
int C2_HOOK_CDECL main(int argc,char**argv){int error;if(argc!=4)return 2;error=load_image(argv[2],argv[3]);return error?error:run(2,argv);}
'''


def physical_definitions():
    ix={n:i for i,n in enumerate(ADDRESSES)}
    lines=[f'static unsigned addresses[{len(ix)}];',
        'static unsigned __cdecl checked(unsigned,unsigned,unsigned,unsigned*,unsigned,unsigned);']
    for n,t in GLOBAL_TYPES.items():lines.append(f'#define {n} (*({t}*)(uintptr_t)addresses[{ix[n]}])')
    lines.append(f'#define gReentrancy_array ((tPipe_chunk_type*)(uintptr_t)addresses[{ix["gReentrancy_array"]}])')
    for n in ('ARStartPipingSession','EndPipingSession2'):
        typ='tPipe_chunk_type' if n=='ARStartPipingSession' else 'int'
        lines.append(f'void C2_HOOK_FASTCALL {n}({typ} v){{checked(addresses[{ix[n]}],v,0,NULL,0,0);}}')
    lines.append(f'void C2_HOOK_FASTCALL AREndPipingSession(void){{checked(addresses[{ix["AREndPipingSession"]}],0,0,NULL,0,0);}}')
    for n in ('ARAddDataToSession','ARDoSingleSession'):
        lines.append(f'void C2_HOOK_FASTCALL {n}(int t,uintptr_t owner,void*p,tU32 size){{unsigned words[2];words[0]=(unsigned)(uintptr_t)p;words[1]=size;checked(addresses[{ix[n]}],t,owner,words,2,0);}}')
    for n in ('ARAddVariedDataToSession','ARDoSingleVariedSession'):
        lines.append(f'void C2_HOOK_CDECL {n}(int t,uintptr_t owner,int count,...){{unsigned words[24],i;va_list ap;words[0]=t;words[1]=owner;words[2]=count;va_start(ap,count);for(i=3;i<24;i++)words[i]=va_arg(ap,unsigned);va_end(ap);checked(addresses[{ix[n]}],0,0,words,24,96);}}')
    lines.append(f'tU32 C2_HOOK_FASTCALL LengthOfSession(tPipe_chunk*p){{return checked(addresses[{ix["LengthOfSession"]}],(unsigned)(uintptr_t)p,0,NULL,0,0);}}')
    lines.append(f'void C2_HOOK_FASTCALL ARAdvanceChunkPtr(tU8**p,int type){{checked(addresses[{ix["ARAdvanceChunkPtr"]}],(unsigned)(uintptr_t)p,type,NULL,0,0);}}')
    for n,typ in [('ARPipeSearchForwards','int'),('ARGetStartTime','tU32')]:
        lines.append(f'{typ} C2_HOOK_FASTCALL {n}(void){{return checked(addresses[{ix[n]}],0,0,NULL,0,0);}}')
    return '\n'.join(lines)


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE')
    p.add_argument('--recompiled',type=Path,default=ROOT/'build-matching/rec2.exe')
    p.add_argument('--symbols',type=Path)
    p.add_argument('--output',type=Path,default=ROOT/'build-audit/replay-session-tests')
    p.add_argument('--source',type=Path,default=SOURCE)
    p.add_argument('--case-limit',type=int)
    p.add_argument('--dependency-dir',type=Path,default=ROOT/'build-audit/joystick/python-deps')
    p.add_argument('--gcc',type=Path,default=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe')
    p.add_argument('--msvc-root',type=Path,default=ROOT/'build-tools/msvc-modern')
    p.add_argument('--msvc5-root',type=Path,default=ROOT/'build-tools/msvc500sp3')
    args=p.parse_args();sys.path.insert(0,str(args.dependency_dir.resolve()))
    from verify_tinted_native_reconstruction import extract
    original=args.original.read_bytes();assert hashlib.sha256(original).hexdigest()==ORIGINAL_HASH
    images=[(original,ADDRESSES)]
    if args.symbols:
        mapping=json.loads(args.symbols.read_text());rebuilt=args.recompiled.read_bytes()
        assert mapping['format']==1 and mapping['original_sha256']==ORIGINAL_HASH
        assert mapping['recompiled_sha256']==hashlib.sha256(rebuilt).hexdigest()
        syms={s['orig']:s for s in mapping['symbols']}
        images.append((rebuilt,{n:syms[a]['recomp'] for n,a in ADDRESSES.items()}))
    out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)

    source=args.source.read_text();names=['ReplayCopyForward','LengthOfSession','ARStartPipingSession','EndPipingSession2',
        'AREndPipingSession','ARAddDataToSession','ARAddVariedDataToSession','ARReplayForwards',
        'ARAdvanceChunkPtr','ARPipeSearchForwards','ARDoSingleSession','ARDoSingleVariedSession','ARGetStartTime']
    bodies=[extract(source,n) for n in names]
    driver=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <float.h>
#include "23-piping.h"
#include "52-errors.h"
#include "globvars.h"
typedef char layouts[sizeof(void*)==4&&sizeof(tReplay_callback)==24&&sizeof(tPipe_chunk)==8&&sizeof(tReduced_pos)==6&&sizeof(tPipe_chunk_smoke)==10&&offsetof(tProgram_state,racing)==0x28&&offsetof(tProgram_state,current_car)==0xac?1:-1];
#ifndef PHYSICAL
tU8*gPipe_buffer_start,*gLocal_buffer,*gMr_chunky,*gPipe_buffer_phys_end,*gPipe_buffer_working_end,*gPipe_buffer_oldest,*gPipe_record_ptr;
const tReplay_callback*gPipe_callbacks;tU32 gLocal_buffer_size;int gReentrancy_count;tPipe_chunk_type gReentrancy_array[5];
int gAction_replay_mode;tProgram_state gProgram_state;
tU8*gPipe_chunk_end,*gPipe_play_ptr;int gDisable_advance,gPlay_direction;float gReplay_rate;
#else
PHYSICAL_DEFINITIONS
#endif
static unsigned arena[0x11000/4],h[33];static int events,callback_calls;
static int C2_HOOK_FASTCALL callback1(tPipe_chunk*packet);
static int C2_HOOK_FASTCALL callback2(tPipe_chunk*packet);
static unsigned norm(unsigned v){unsigned b=(unsigned)(uintptr_t)arena;if(v>=b&&v<=b+sizeof(arena))return v-b+0x1000000;if(v==(unsigned)(uintptr_t)callback1)return 0x1082000;if(v==(unsigned)(uintptr_t)callback2)return 0x1082010;return v;}
static unsigned rd(unsigned off){unsigned v;memcpy(&v,(char*)arena+off,4);return v;}
static void wr(unsigned off,unsigned v){memcpy((char*)arena+off,&v,4);}
static void hex(const void*p,unsigned n){unsigned i;for(i=0;i<n;i++)printf("%02x",((const unsigned char*)p)[i]);}
static void snapshot(void){unsigned char copy[sizeof(arena)];unsigned prefix[22],i,j,v;
    prefix[0]=norm((unsigned)(uintptr_t)gPipe_buffer_start);prefix[1]=norm((unsigned)(uintptr_t)gLocal_buffer);prefix[2]=norm((unsigned)(uintptr_t)gMr_chunky);prefix[3]=norm((unsigned)(uintptr_t)gPipe_callbacks);
    prefix[4]=norm((unsigned)(uintptr_t)gPipe_buffer_phys_end);prefix[5]=norm((unsigned)(uintptr_t)gPipe_buffer_working_end);prefix[6]=norm((unsigned)(uintptr_t)gPipe_buffer_oldest);prefix[7]=norm((unsigned)(uintptr_t)gPipe_record_ptr);prefix[8]=gLocal_buffer_size;prefix[9]=gReentrancy_count;
    for(i=0;i<5;i++){prefix[10+i]=gReentrancy_array[i];}prefix[15]=gAction_replay_mode;prefix[16]=gProgram_state.racing;
    prefix[17]=norm((unsigned)(uintptr_t)gPipe_chunk_end);prefix[18]=norm((unsigned)(uintptr_t)gPipe_play_ptr);prefix[19]=gDisable_advance;prefix[20]=gPlay_direction;memcpy(prefix+21,&gReplay_rate,4);
    memcpy(copy,arena,sizeof(copy));for(j=0;j<2;j++)for(i=0;i<71;i++){v=norm(rd(0xe000+0x1000*j+24*i+8));memcpy(copy+0xe000+0x1000*j+24*i+8,&v,4);}if(h[0]==10){v=norm(rd(h[23]));memcpy(copy+h[23],&v,4);}hex(prefix,sizeof(prefix));hex(copy,sizeof(copy));
}
static int callback(tPipe_chunk*packet,int which){printf("%s[\"C\",%d,%u,\"",events++?",":"",which,((h[0]==4||h[0]==9)&&((unsigned)(uintptr_t)packet<(unsigned)(uintptr_t)arena||(unsigned)(uintptr_t)packet>=(unsigned)(uintptr_t)arena+sizeof(arena)))?0x1090000:norm((unsigned)(uintptr_t)packet));hex(packet,48);printf("\",\"");snapshot();printf("\"]");
    if(callback_calls++==0){if(h[15]&1)gPipe_callbacks=(tReplay_callback*)((char*)arena+0xf000);if(h[15]&2){wr(h[23],h[9]+1);wr(h[23]+4,1);}if(h[15]&4){gPipe_buffer_start=NULL;gAction_replay_mode=1;gProgram_state.racing=0;}if(h[15]&8)gPipe_buffer_oldest=(tU8*)arena+0x4800;if(h[15]&16){gLocal_buffer=(tU8*)arena+0x2000;gMr_chunky=(tU8*)arena+0x200c;gLocal_buffer_size=12;}if(h[15]&32)gPipe_record_ptr=(tU8*)arena+0x4080;if(h[15]&64)wr(h[23],(unsigned)(uintptr_t)arena+0xd080);if(h[15]&128)gPipe_chunk_end=(tU8*)arena+0xd090;}
    return h[16]+2*which;
}
static int C2_HOOK_FASTCALL callback1(tPipe_chunk*packet){return callback(packet,0);}
static int C2_HOOK_FASTCALL callback2(tPipe_chunk*packet){return callback(packet,1);}
void C2_NORETURN C2_HOOK_CDECL FatalError(int index,...){printf("%s[\"F\",%d,\"",events++?",":"",index);snapshot();printf("\"]],\"state\":\"");snapshot();printf("\",\"value\":null,\"fatal\":%d}\n",index);exit(0);}
'''
    driver+='\n#ifndef PHYSICAL\n'+'\n\n'.join(bodies)+'\n#endif\n'
    driver+=r'''
static tU8* pointer(unsigned off){return off==0xffffffff?NULL:(tU8*)arena+off;}
static int run(int argc,char**argv){FILE*f;unsigned i,j,value=0;if(argc!=2)return 1;f=fopen(argv[1],"rb");if(!f)return 2;if(fread(h,sizeof(h),1,f)!=1||fread(arena,sizeof(arena),1,f)!=1)return 3;fclose(f);_controlfp(_PC_53,_MCW_PC);
    gDisable_advance=h[24];gPipe_chunk_end=(tU8*)(uintptr_t)h[25];gPipe_play_ptr=(tU8*)(uintptr_t)h[26];gPlay_direction=(int)h[27];memcpy(&gReplay_rate,h+28,4);
    if(h[0]==10){if(!(h[31]&1))wr(h[23],(unsigned)(uintptr_t)arena+h[29]);else wr(h[23],h[29]);if(!(h[31]&2))gPipe_chunk_end=(tU8*)arena+h[25];}
    gPipe_buffer_start=(h[1]&1)?(tU8*)arena+0x4000:NULL;gAction_replay_mode=(h[1]>>1)&1;gProgram_state.racing=(h[1]>>2)&1;
    gProgram_state.current_car.car_master_actor=(br_actor*)((char*)arena+0xc000);gLocal_buffer=(tU8*)arena;gMr_chunky=(tU8*)arena+h[3];gPipe_callbacks=(tReplay_callback*)((char*)arena+0xe000);
    gPipe_buffer_phys_end=pointer(h[8]);gPipe_buffer_working_end=pointer(h[7]);gPipe_buffer_oldest=pointer(h[6]);gPipe_record_ptr=pointer(h[5]);gLocal_buffer_size=h[3];gReentrancy_count=h[2];for(i=0;i<5;i++)gReentrancy_array[i]=(tPipe_chunk_type)(20+i);
    for(j=0;j<2;j++)for(i=0;i<71;i++){unsigned off=0xe000+0x1000*j+24*i+8,v=rd(off);if(v==0x1082000)wr(off,(unsigned)(uintptr_t)callback1);else if(v==0x1082010)wr(off,(unsigned)(uintptr_t)callback2);}
    if(h[0]==11){gPipe_record_ptr=(tU8*)(uintptr_t)h[5];gPipe_buffer_oldest=(tU8*)(uintptr_t)h[6];}
    printf("{\"calls\":[");
    if(h[0]==0)ARStartPipingSession((tPipe_chunk_type)h[9]);
    else if(h[0]==1)EndPipingSession2(h[21]);
    else if(h[0]==2)AREndPipingSession();
    else if(h[0]==3)ARAddDataToSession(h[9],h[10],(char*)arena+h[20],h[11]);
    else if(h[0]==8)ARDoSingleSession(h[9],h[10],(char*)arena+h[20],h[11]);
    else if(h[0]==4)ARAddVariedDataToSession(h[9],h[10],h[22],64,0,(char*)arena+h[20],1,1,0x12345678,2,3,0x11223344,4,5,0x56789abc,-4,16,h[17],7,20,(char*)arena+0xd040,0,0,0);
    else if(h[0]==5)value=LengthOfSession((tPipe_chunk*)((char*)arena+h[23]));
    else if(h[0]==7){ARStartPipingSession((tPipe_chunk_type)h[9]);ARAddDataToSession(h[9],h[10],(char*)arena+h[20],h[11]);ARStartPipingSession((tPipe_chunk_type)(h[9]+1));ARAddDataToSession(h[9]+1,h[10]+1,(char*)arena+h[20],h[11]);AREndPipingSession();AREndPipingSession();}
    else if(h[0]==9)ARDoSingleVariedSession(h[9],h[10],h[22],64,0,(char*)arena+h[20],1,1,0x12345678,2,3,0x11223344,4,5,0x56789abc,-4,16,h[17],7,20,(char*)arena+0xd040,0,0,0);
    else if(h[0]==10)ARAdvanceChunkPtr(h[32]?&gPipe_chunk_end:(tU8**)((char*)arena+h[23]),h[9]);
    else if(h[0]==11)value=ARPipeSearchForwards();
    else if(h[0]==12)value=ARGetStartTime();
    printf("],\"state\":\"");snapshot();printf("\",\"value\":");if(h[0]==5||h[0]==11||h[0]==12)printf("%u",value);else printf("null");printf(",\"fatal\":null}\n");return 0;
}
'''
    driver=driver.replace('PHYSICAL_DEFINITIONS',physical_definitions())
    native=driver+'\nint C2_HOOK_CDECL main(int argc,char**argv){return run(argc,argv); }\n'
    harness=out/'harness.c';harness.write_text(native)
    physical='#define PHYSICAL\n'+driver+PHYSICAL_LOADER.replace('FATAL_INDEX',str(list(ADDRESSES).index('FatalError')))
    (out/'physical.c').write_text(physical)
    original_meta=address_file(out/'original-addresses.bin',original,ADDRESSES,SPANS)
    linked_meta=None
    if args.symbols:
        # Cover genuine compiler-emitted helpers as well as the entry bodies.
        # Code closure is audited before any linked image is executed.
        linked=images[1][1]
        functions=[n for n in ADDRESSES if not n.startswith('g') and n!='FatalError']
        addresses=sorted({linked[n] for n in functions})
        known={s['recomp'] for s in mapping['symbols']}
        spans=[(a,min(x for x in known if x>a)-a) for a in addresses]
        from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM
        cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
        pending=list(spans);seen={a for a,_ in spans}
        while pending:
            a,length=pending.pop()
            for ins in cs.disasm(raw(rebuilt,a,length),a):
                if ins.mnemonic=='call' and ins.operands[0].type==CS_OP_IMM:
                    target=ins.operands[0].imm
                    if target in seen or target==linked['FatalError']:continue
                    assert target in known,(hex(ins.address),ins.op_str)
                    extra=(target,min(x for x in known if x>target)-target)
                    seen.add(target);spans.append(extra);pending.append(extra)
        linked_meta=address_file(out/'linked-addresses.bin',rebuilt,linked,spans)
    includes=[ROOT/r for r in ['src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/carma2/platform','src/carma2/s3/include','src/brender/include']]
    msvc=sorted((args.msvc_root.resolve()/'VC/Tools/MSVC').iterdir())[-1];sdk=args.msvc_root.resolve()/'Windows Kits/10';version=sorted(p.name for p in (sdk/'Include').iterdir())[-1]
    env=dict(os.environ);env['PATH']=str(msvc/'bin/Hostx64/x86')+os.pathsep+env['PATH'];env['INCLUDE']=os.pathsep.join(map(str,[msvc/'include',*[sdk/f'Include/{version}/{r}' for r in ['ucrt','shared','um']]]));env['LIB']=os.pathsep.join(map(str,[msvc/'lib/x86',*[sdk/f'Lib/{version}/{r}/x86' for r in ['ucrt','um']]]))
    old=args.msvc5_root.resolve();oldenv=dict(os.environ);oldenv['PATH']=str(old/'bin')+os.pathsep+str(old/'redist')+os.pathsep+oldenv['PATH'];oldenv['INCLUDE']=str(old/'include');oldenv['LIB']=str(old/'lib')
    commands={
        'msvc':[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(r) for r in includes],str(harness),'/Fo'+str(out/'msvc.obj'),'/Fe'+str(out/'msvc.exe')],
        'msvc5':[str(old/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(r) for r in includes],str(harness),'/Fo'+str(out/'msvc5.obj'),'/Fe'+str(out/'msvc5.exe')],
        'mingw':[str(args.gcc.resolve()),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-fno-strict-aliasing',*['-I'+str(r) for r in includes],str(harness),'-o',str(out/'mingw.exe')]
    }
    commands['physical']=[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS','/DREC2_MATCHING',*['/I'+str(r) for r in includes],str(out/'physical.c'),'/Fo'+str(out/'physical.obj'),'/Fe'+str(out/'physical.exe')]
    for compiler,cmd in commands.items():
        build=subprocess.run(cmd,cwd=out,env=env if compiler in ('msvc','physical') else oldenv if compiler=='msvc5' else None,text=True,capture_output=True)
        (out/(compiler+'-build.log')).write_text(build.stdout+build.stderr);assert build.returncode==0,build.stdout+build.stderr
    def bits(v):return struct.unpack('<I',struct.pack('<f',v))[0]
    cases=[]
    # Reproduce the old negative-size scalar bug before testing the new entries.
    for mode in (4,9):
        for scalar in [0x7f800001,0xff812345,0x7fc12345,0xffc12345,0,0x80000000,1,0x80000001,0x7f800000,0xff800000,0xa1b2c3d4]:
            cases.append(dict(mode=mode,radius=scalar,length=32))
    for mode in (8,9):
        for gate in range(8):
            for reentrancy in (0,1,2,5):cases.append(dict(mode=mode,gate=gate,reentrancy=reentrancy,length=32))
        for record in (0x4000,0x4078,0x9ffe):
            for oldest in (0xffffffff,0x4000,0x4078,0x9000):
                cases.append(dict(mode=mode,record=record,oldest=oldest,length=32))
        for reentrancy in (0,1,2):
            for mutate in (0,1,2,4,8,16,32,3,5,17):
                cases.append(dict(mode=mode,callback=True,mutate=mutate,oldest=0x4000,reentrancy=reentrancy,length=32))
    for size in (0,1,3,4,5,6,10,24,63,14987,14988,14989):cases.append(dict(mode=8,size=size))
    for data,size in [(0,10),(4,10),(8,10),(12,10),(0,24),(4,24)]:cases.append(dict(mode=8,data=data,size=size))
    for count in (-1,0,1,2,7):cases.append(dict(mode=9,varied_count=count,length=0 if count<=0 else 24))
    for disabled in (0,1,0xffffffff):
        for end in (0xd000,0xd00d,0xd00e,0xd00f,0xd100):
            for callback in (False,True):cases.append(dict(mode=10,disabled=disabled,chunk_end=end,callback=callback))
    for mutate in (0,1,64,128,65,129,192,193):
        for end in (0xd00e,0xd086,0xd090,0xd100):cases.append(dict(mode=10,callback=True,mutate=mutate,chunk_end=end))
    for start,end,length in [(0x7ffffff8,0x7fffffff,10),(0xfffffff8,0x100,10),(0x80000000,0x100,0),(0x10,0x80000000,0),(0,0xffffffff,0),(0xfffffffc,0,0)]:
        cases.append(dict(mode=10,chunk=start,chunk_end=end,pointer_flags=3,length=length))
    for alias in (0,1):cases.append(dict(mode=10,alias=alias,chunk_end=0xd100))
    for rate in (0,0x80000000,1,0x80000001,0x3f800000,0xbf800000,0x7f800000,0xff800000,0x7fc12345,0xffc12345,0x7f800001,0xff800001):
        for direction in (0,1,0xffffffff,0x7fffffff,0x80000000):
            for relation in range(4):cases.append(dict(mode=11,rate=rate,direction=direction,relation=relation))
    for layout in ('frame','skip-oldest','multiple','empty','wrap','wrap-record','at-end-record','odd'):
        for callback in (False,True):cases.append(dict(mode=12,layout=layout,callback=callback))
    for mutate in (1,2,4,32,3):cases.append(dict(mode=12,layout='frame',callback=True,mutate=mutate))
    if args.case_limit:cases=cases[:args.case_limit]
    results=[]
    for index,case in enumerate(cases):
        arena=bytearray(b'\xa5'*SIZE)
        def put(off,v):struct.pack_into('<I',arena,off,v&0xffffffff)
        def vec(off,v):struct.pack_into('<3f',arena,off,*v)
        h=[case['mode'],case.get('gate',5),case.get('reentrancy',0),case.get('local',8),case.get('local_count',1),case.get('record',0x4000),case.get('oldest',0xffffffff),case.get('working',0xa000),case.get('physical',0xa000),15,0x12345678,case.get('size',10),case.get('session_count',2),int(case.get('callback',False)),case.get('length',10),case.get('mutate',0),case.get('extra',0),case.get('radius',bits(.05)),case.get('strength',bits(.65)),0x17,case.get('data',0xd000),1,case.get('varied_count',7),case.get('session',0xb000),case.get('disabled',0),case.get('chunk_end',0xd100),case.get('play',0),case.get('direction',1),case.get('rate',0x3f800000),case.get('chunk',0xd000),0,case.get('pointer_flags',0),case.get('alias',0)]
        h=[v&0xffffffff for v in h]
        put(0,h[9]);put(4,h[4]);put(0x2000,h[9]);put(0x2004,1)
        for off in range(0x4000,0xa000-24,24):put(off,h[9]);put(off+4,1)
        if h[6]!=0xffffffff:put(h[6],h[9]);put(h[6]+4,1)
        put(h[23],h[9]);put(h[23]+4,h[12])
        for off in [0xd000,0xd040]:
            for j in range(64):arena[off+j]=(j*13+7)&255
        for table in range(2):
            for i in range(71):
                off=0xe000+0x1000*table+24*i;arena[off:off+24]=bytes(24)
                put(off+4,h[14]+2*table+(4 if i==h[9]+1 else 0));put(off+8,(CB1 if table==0 else CB2) if h[13] else 0)
        # Recorder eviction walks must follow real session lengths. With a
        # variable-length callback the first invocation may switch tables;
        # subsequent records use that replacement callback's extra two bytes.
        if h[0] in (8,9) and h[6]!=0xffffffff:
            at=h[6]
            first_switch=h[13] and h[15]&1
            first_size=10+h[14]+4+(2 if first_switch else 0)
            later_size=first_size+(2 if first_switch else 0)
            if h[0]==9 and h[2]==0 and first_switch:first_size=later_size
            for slot in range(40):
                if at+8>=0xe000:break
                put(at,h[9]);put(at+4,1)
                at+=first_size if slot==0 else later_size
        vec(0xc050,[.1,-.2,.3]);vec(0x10000,[3.125,-4.25,5.375])
        if 'actor' in case:vec(0xc050,case['actor'])
        if 'position' in case:vec(0x10000,case['position'])
        if h[0]==11:
            h[5],h[6],h[26]=0x12345000,0x23456000,0x34567000
            relation=case['relation']
            if relation==1:h[26]=h[5]
            if relation==2:h[26]=h[6]
            if relation==3:h[5]=h[6]=h[26]
        if h[0]==12:
            # Valid replay rings with variable callback lengths; no scratch
            # bytes or malformed ring walks are used as expected values.
            layout=case['layout'];h[6]=0x4000;h[5]=0x4800;h[23]=h[6]
            if layout=='odd':h[14]=1
            for table in range(2):
                for kind in range(71):put(0xe000+table*0x1000+24*kind+4,h[14]+2*table)
            advance=10+h[14]+4
            if h[13] and h[15]&1:advance+=2
            put(0x4000,0 if layout=='skip-oldest' else 15);put(0x4004,1);put(0x400c,0x11111111)
            put(0x4000+advance,0);put(0x4004+advance,1);put(0x400c+advance,0x87654321)
            if layout=='multiple':
                put(0x4000+advance,15);put(0x4000+advance*2,0);put(0x4004+advance*2,1);put(0x400c+advance*2,0x12345678)
            if layout=='empty':h[5]=h[6]+advance
            if layout in ('wrap','wrap-record','at-end-record'):
                h[6]=0x6000;h[7]=0x6000+advance;h[23]=h[6]
                put(h[6],15);put(h[6]+4,1);put(0x4000,0);put(0x400c,0xfedcba98)
                if layout=='wrap-record':h[5]=0x4000
                if layout=='at-end-record':h[5]=h[7]
            if h[15]&32:h[5]=0x4080
        fixture=out/'fixture.bin';fixture.write_bytes(struct.pack('<33I',*h)+arena)
        expected=json.loads(subprocess.check_output([str(out/'physical.exe'),str(fixture),str(args.original.resolve()),str(out/'original-addresses.bin')],text=True,timeout=15))
        checks=[]
        if args.symbols:
            checks.append(('matching-physical',json.loads(subprocess.check_output([str(out/'physical.exe'),str(fixture),str(args.recompiled.resolve()),str(out/'linked-addresses.bin')],text=True,timeout=15))))
        for compiler in ('msvc','msvc5','mingw'):checks.append((compiler,json.loads(subprocess.check_output([str(out/(compiler+'.exe')),str(fixture)],text=True,timeout=15))))
        for compiler,actual in checks:
            if actual!=expected:
                failure=out/f'failure-{compiler}-{index}.json';failure.write_text(json.dumps(dict(case=case,original=expected,actual=actual),indent=2))
                raise AssertionError(f'{compiler} replay session case {index} differs: {failure}')
            results.append(dict(case=case,compiler=compiler,passed=True,calls=len(actual['calls']),fatal=actual['fatal'],state_sha256=hashlib.sha256(bytes.fromhex(actual['state'])).hexdigest()))
    (out/'verification.json').write_text(json.dumps(dict(scenario_count=len(cases),comparisons=len(results),original_sha256=ORIGINAL_HASH,source_sha256=hashlib.sha256(args.source.read_bytes()).hexdigest(),header_sha256=hashlib.sha256(SOURCE.with_suffix('.h').read_bytes()).hexdigest(),verifier_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),bodies_sha256=[hashlib.sha256(b.encode()).hexdigest() for b in bodies],compiler_commands=commands,original=original_meta,linked=linked_meta,recompiled_sha256=hashlib.sha256(args.recompiled.read_bytes()).hexdigest() if args.symbols else None,cases=results,limitations=__doc__),indent=2)+'\n')
    print(f'PASS: {len(cases)} scenarios, {len(results)} complete replay-session comparisons.')


if __name__=='__main__':main()
