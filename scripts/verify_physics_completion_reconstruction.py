"""Complete object notification, passive-state and replay regression tests."""
import argparse,ast,hashlib,json,random,re,struct,subprocess,sys
from capstone import *
from pathlib import Path
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'src/carma2').is_dir());D=Path(__file__).resolve().parent
SOURCE=ROOT/'src/carma2/newcommon/60-phil.c'
PIPING=ROOT/'src/carma2/newcommon/23-piping.c'
sys.path.insert(0,str(ROOT/'scripts'))
import verify_physics_registration_reconstruction as registration
import verify_phil_registry_reconstruction as registry
from verify_tinted_native_reconstruction import extract
from verify_opponent_route_reconstruction import HASH,sha,words,pe,raw
from inventory_reconstruction_backlog import mask_c
BASE=0x23000000;SIZE=0x40000
COMMON=r'''/* Complete completion/replay proof. Callbacks are external observations;
 * all game/replay lower bodies execute. Fixed callback thunks avoid pointer
 * normalization in the full public arena. Masked x87 PC53, DF clear. */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "60-phil.h"
#include "23-piping.h"
#include "52-errors.h"
#include "globvars.h"
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5];
extern unsigned builder_stack_count,builder_result,builder_poison,builder_cdecl;
extern unsigned __cdecl BuilderInvoke(void);
static unsigned addresses[3],h[24],case_index,callback_calls;
static unsigned char* heap;
enum {ARENA_BYTES=0x40000};
#define gPHIL_disabled (*(int*)(heap+0xf000))
#define gPipe_buffer_start (*(tU8**)(heap+0xf168))
#define gLocal_buffer (*(tU8**)(heap+0xf114))
#define gMr_chunky (*(tU8**)(heap+0xf100))
#define gPipe_callbacks (*(const tReplay_callback**)(heap+0xf148))
#define gPipe_buffer_phys_end (*(tU8**)(heap+0xf150))
#define gPipe_buffer_working_end (*(tU8**)(heap+0xf124))
#define gPipe_buffer_oldest (*(tU8**)(heap+0xf10c))
#define gPipe_record_ptr (*(tU8**)(heap+0xf11c))
#define gLocal_buffer_size (*(tU32*)(heap+0xf154))
#define gReentrancy_count (*(int*)(heap+0xf144))
#define gReentrancy_array ((tPipe_chunk_type*)(heap+0xf130))
#define gAction_replay_mode (*(int*)(heap+0xf174))
#define gProgram_state (*(tProgram_state*)(heap+0x20000))
typedef char CompletionLayouts[sizeof(void*)==4&&sizeof(tReplay_callback)==24&&
    offsetof(tPhysics_callbacks,changed_objects)==0xc&&offsetof(tPhysics_callbacks,active_halted)==0x10&&
    offsetof(tProgram_state,racing)==0x28&&offsetof(tPhysics_object,next)==0x220?1:-1];
static unsigned rd(unsigned offset) {unsigned value;memcpy(&value,heap+offset,4);return value;}
static void wr(unsigned offset,unsigned value) {memcpy(heap+offset,&value,4);}
static void snapshot(unsigned event,unsigned value,unsigned detail,const void* packet) {
    unsigned header[4];unsigned char bytes[52];
    header[0]=case_index;header[1]=event;header[2]=value;header[3]=detail;
    memset(bytes,0,sizeof(bytes));if(packet)memcpy(bytes,packet,sizeof(bytes));
    fwrite(header,sizeof(header),1,stdout);fwrite(bytes,sizeof(bytes),1,stdout);
    fwrite(heap,ARENA_BYTES,1,stdout);
}
/* Mutation records are raw offset/value pairs, applied once after the first
 * observed callback. They exercise actual mutable table/object/global state. */
static void mutate(unsigned kind,unsigned pointer) {unsigned i;
    if(h[8]&&h[8]!=kind)return;
    if(kind==3&&h[9]==1&&(pointer<0x23000000||pointer>=0x23040000))return;
    if(kind==3&&h[9]==2&&(pointer>=0x23000000&&pointer<0x23040000))return;
    if(callback_calls++!=0)return;
    for(i=0;i<h[5];i++)wr(rd(0xe000+i*8),rd(0xe004+i*8));
}
static int halted_action(tPhysics_object* object,unsigned which) {
    snapshot(1,(unsigned)(uintptr_t)object,which,NULL);mutate(1,(unsigned)(uintptr_t)object);return (int)h[6];
}
static void changed_action(tPhysics_object* object,tU32 time,unsigned which) {
    /* Event10 distinguishes the replacement changed callback from event2. */
    snapshot(which?10:2,(unsigned)(uintptr_t)object,time,NULL);mutate(2,(unsigned)(uintptr_t)object);
}
static int calc_action(tPipe_chunk* chunk,unsigned which) {
    unsigned address=(unsigned)(uintptr_t)chunk;
    snapshot(3,which,(address>=0x23000000&&address<0x23040000)?address:0xffffffffu,chunk);
    mutate(3,address);return (int)h[7];
}
static int __fastcall original_halted(tPhysics_object* o) {return halted_action(o,0);}
static int C2_HOOK_FASTCALL native_halted(tPhysics_object* o) {return halted_action(o,0);}
static int __fastcall original_halted2(tPhysics_object* o) {return halted_action(o,1);}
static int C2_HOOK_FASTCALL native_halted2(tPhysics_object* o) {return halted_action(o,1);}
static void __fastcall original_changed(tPhysics_object* o,tU32 time) {changed_action(o,time,0);}
static void C2_HOOK_FASTCALL native_changed(tPhysics_object* o,tU32 time) {changed_action(o,time,0);}
static void __fastcall original_changed2(tPhysics_object* o,tU32 time) {changed_action(o,time,1);}
static void C2_HOOK_FASTCALL native_changed2(tPhysics_object* o,tU32 time) {changed_action(o,time,1);}
static int __fastcall original_calc(tPipe_chunk* p) {return calc_action(p,0);}
static int C2_HOOK_FASTCALL native_calc(tPipe_chunk* p) {return calc_action(p,0);}
static int __fastcall original_calc2(tPipe_chunk* p) {return calc_action(p,1);}
static int C2_HOOK_FASTCALL native_calc2(tPipe_chunk* p) {return calc_action(p,1);}
void C2_NORETURN C2_HOOK_CDECL FatalError(int value,...) {
    snapshot(4,(unsigned)value,0,NULL);fflush(stdout);ExitProcess(0);
}
static LONG WINAPI fault(EXCEPTION_POINTERS* x) {
    if(x->ExceptionRecord->ExceptionCode!=EXCEPTION_ACCESS_VIOLATION)return EXCEPTION_CONTINUE_SEARCH;
    snapshot(5,x->ExceptionRecord->ExceptionCode,(unsigned)x->ExceptionRecord->ExceptionInformation[1],NULL);
    fflush(stdout);ExitProcess(0);return EXCEPTION_CONTINUE_SEARCH;
}
static void install_observer(void) {
    typedef void*(WINAPI *install_fn)(ULONG,LONG(WINAPI*)(EXCEPTION_POINTERS*));
    FARPROC address=GetProcAddress(GetModuleHandleA("kernel32.dll"),"AddVectoredExceptionHandler");install_fn fn;
    memcpy(&fn,&address,4);if(!fn||!fn(1,fault))exit(38);
}
static void install_thunks(void) {
    unsigned i,value;unsigned char* thunks;unsigned targets[6];
    thunks=(unsigned char*)VirtualAlloc((void*)0x23050000,0x1000,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);
    if(thunks!=(void*)0x23050000)exit(39);
#ifdef PHYSICAL_IMAGE
    targets[0]=(unsigned)original_halted;targets[1]=(unsigned)original_changed;targets[2]=(unsigned)original_calc;
    targets[3]=(unsigned)original_halted2;targets[4]=(unsigned)original_changed2;targets[5]=(unsigned)original_calc2;
    (void)native_halted;(void)native_changed;(void)native_calc;(void)native_halted2;(void)native_changed2;(void)native_calc2;
#else
    targets[0]=(unsigned)native_halted;targets[1]=(unsigned)native_changed;targets[2]=(unsigned)native_calc;
    targets[3]=(unsigned)native_halted2;targets[4]=(unsigned)native_changed2;targets[5]=(unsigned)native_calc2;
    (void)original_halted;(void)original_changed;(void)original_calc;(void)original_halted2;(void)original_changed2;(void)original_calc2;
#endif
    for(i=0;i<6;i++){thunks[i*16]=0xe9;value=targets[i]-((unsigned)thunks+i*16+5);memcpy(thunks+i*16+1,&value,4);}
    FlushInstructionCache(GetCurrentProcess(),thunks,0x1000);
}
static int run(const char* path) {
    FILE*f;unsigned mode;SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);
    f=fopen(path,"rb");if(!f)return 10;
    heap=(unsigned char*)VirtualAlloc((void*)0x23000000,ARENA_BYTES,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
    if(heap!=(void*)0x23000000)return 11;
    _setmode(_fileno(stdout),_O_BINARY);install_observer();install_thunks();
    for(case_index=0;fread(h,sizeof(h),1,f)==1;case_index++){
        if(fread(heap,ARENA_BYTES,1,f)!=1)return 12;
        mode=h[0];if(mode>=3)return 37;callback_calls=0;
        _controlfp(_PC_53,_MCW_PC);_clearfp();
        builder_mode=1;builder_address=addresses[mode];builder_x=h[1];builder_y=h[2];
        builder_stack[0]=h[3];
#if defined(REC2_MATCHING)
        builder_cdecl=0;builder_stack_count=mode==0?1:0;
#else
        builder_cdecl=1;builder_stack_count=mode==0?3:1;
        builder_stack[0]=h[1];builder_stack[1]=h[2];builder_stack[2]=h[3];
#endif
        builder_poison=h[4];if(BuilderInvoke())exit(14);
        snapshot(0,0,0,NULL);
    }
    fclose(f);return 0;
}
'''

ROOTS=[('ChangedObjectsCallbacks',0x4b6be0,249),('MarkObjectAndChildrenAsPassive',0x4b6e90,40),('PipeSinglePHILObject',0x4c8cc0,24)]
LOWER=[('ARDoSingleVariedSession',0x402bd0,503),('EndPipingSession2',0x402660,555),('__chkstk',0x575bb0,47)]
MUTABLE={0x65d004:('gPHIL_disabled',0xf000,4),
 0x6768a0:('CanonicalReplayGlobals',0xf100,0x78),
 0x75bb80:('gProgram_state',0x20000,0x4000)}

def packet(data,out):
    registration.ROOTS=ROOTS;registration.MUTABLE=MUTABLE;registration.BASE=BASE
    registration.EXIT=0x44c230;registration.TABLES={}
    registration.INDIRECT={0x4b6be0,0x402bd0,0x402660}
    catalog={a:(n,s)for n,a,s in ROOTS+LOWER}
    meta=registration.original_packet(data,catalog,out)
    expected={(0x4b6be0,0x4b6c4e),(0x4b6be0,0x4b6c9b),(0x402bd0,0x402d10),(0x402660,0x40274d)}
    assert {(v['owner'],v['site'])for v in meta['indirect_sites']}==expected
    assert len(meta['graph'])==6 and sum(v['bytes']for v in meta['graph'])==1418
    assert any(v[1]==0x6768cc and v[2]==BASE+0xf12c for v in meta['patches'])
    return meta

def bridge():
    caller,physical,_=registry.literals()
    caller=caller.replace('builder_poison;','builder_poison,builder_cdecl;')
    a=' mov builder_result,eax\n mov ecx,esp'
    assert a in caller
    caller=caller.replace(a,' mov builder_result,eax\n cmp builder_cdecl,0\n je completion_stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n completion_stackready:\n mov ecx,esp')
    physical=physical.replace('hdr[15]','hdr[5]').replace('hdr[14]','hdr[5]').replace('n=hdr[13]','n=hdr[3]').replace('i<12;i++)addresses','i<3;i++)addresses')
    a=physical.index('image[hdr[12]]=0xe9;');b=physical.index('free(patch);',a)
    physical=physical[:a]+'image[hdr[4]]=0xe9;relative=(unsigned)FatalError-((unsigned)image+hdr[4]+5);memcpy(image+hdr[4]+1,&relative,4);'+physical[b:]
    assert 'hdr[12]'not in physical and 'hdr[13]'not in physical
    return caller,physical

def fixtures():
    rows=[]
    def add(**kw):
        c=dict(mode=0,object=0x100,callbacks=0xd000,time=17,disabled=0,kind=1,status=2,
               stopped=0,flags=4,changed=1,halted=1,halt_return=1,tree=0,replay=0,
               action=0,racing=1,reentrancy=0,calc=0,calc_return=0,length=48,
               local=0x4000,local_size=8,local_count=1,record=0x8000,oldest=0,old_count=1,old_type=48,row47_length=None,row47_calc=0,history_first_stride=None,working=0xc000,physical=0xc000,
               mutations=[],trigger=0,calc_location=0,patches=[],byte_patches=[],seed=len(rows)+1)
        c.update(kw);rows.append(c)
    for mode in range(3):
        for obj in (0,0x100):
            for replay in (0,1):add(mode=mode,object=obj,replay=replay)
    for disabled in (0,1,0xffffffff,0x80000000):
        for kind in (0,1,2,3,255):
            for status in (0,1,2,0xffffffff):add(disabled=disabled,kind=kind,status=status)
    for stopped in (0,1,255):
        for flags in (0,1,2,3,4,6,0xffff):
            for halted in (0,1):
                for returned in (0,1,0xffffffff,0x80000000):
                    add(stopped=stopped,flags=flags,halted=halted,halt_return=returned)
    for period in (0,1,0x7fffffff,0x80000000,0xffffffff,0x7f812345,0xffc12345):
        for changed in (0,1):add(time=period,changed=changed)
    for tree in range(5):
        for mode in (0,1):add(mode=mode,tree=tree,replay=1)
    for mode in (0,2):
        for replay in (0,1):
            for action in (0,1,0xffffffff):
                for racing in (0,1,0xffffffff):
                    for reentrancy in (0,1,2,5):
                        add(mode=mode,replay=replay,action=action,racing=racing,reentrancy=reentrancy)
    # Live callback mutations, including replacement callback table, pointers,
    # hierarchy links, headers, gates, local chunk and replay record cursor.
    mutations=[(0xf000,1),(0x100+0x240,BASE+0x6800),(0x100+0x240,0),
        (0x100+0x220,BASE+0x1100),(0x100+0x224,BASE+0x1900),
        (0xd010,0x23050030),(0xd00c,0x23050040),(0xf148,BASE+0x15000),
        (0xf10c,BASE+0x9000),(0xf11c,BASE+0x8100),(0xf168,0),(0xf174,1),
        (0x20028,0),(0xf114,BASE+0x5000),(0xf100,BASE+0x5008),
        (0xf154,8),(0xf144,2),(0x6804,2),(0x14000+48*24+4,44)]
    for change in mutations:
        for stopped in (0,1):add(replay=1,stopped=stopped,flags=6,calc=1,mutations=[change])
    for mode in (0,2):
        for calc in (0,1):
            for record in (0x8000,0xbfc8,0xbfcc,0xbfd0,0xbfff):
                for oldest in (0,0x8000,0x803e,0x8ffc):
                    add(mode=mode,replay=1,calc=calc,record=record,oldest=oldest)
    for mode in (0,2):
        for offset in (-8,-4,-1,0,1,4,8):
            # Source matrix partially overlaps the final record destination.
            add(mode=mode,replay=1,patches=[(0x100,BASE+0x800c+offset-0x2c)])
            # Forward replay copy's destination partially overlaps local source.
            add(mode=mode,replay=1,record=0x4000+offset)
    ieee=(0,0x80000000,1,0x80000001,0x7f7fffff,0x7f800000,0xff800000,
          0x7f812345,0xff812345,0x7fc12345,0xffc12345,0x3f800001)
    for lane in range(12):
        for value in ieee:add(mode=2,replay=1,patches=[(0x502c+lane*4,value)])
    # Odd ring scan is a real FatalError path. Returned copied length stays <=48.
    for mode in (0,2):
        for returned in (0,1,2,3):add(mode=mode,replay=1,calc=1,calc_return=returned,length=44,oldest=0x8000)
    # End trailer/append comparisons have distinct equality boundaries. Full
    # initialized local arena permits real reentrant End before fresh append.
    for mode in (0,2):
        for local_size in (0,7,8,14996,14997,14998,14999,15000,15001):
            for local_count in (0,1):
                add(mode=mode,replay=1,reentrancy=1,local_size=local_size,local_count=local_count)
    for reentrancy in (6,0xffffffff,0x80000000):
        add(mode=2,replay=1,reentrancy=reentrancy)
    for table_change in ((0xf148,BASE+0x15000),(0x14000+48*24+4,44)):
        for old_count in (0,1,2):
            add(mode=2,replay=1,calc=1,oldest=0x8000,old_count=old_count,mutations=[table_change])
    # Mutated oldest/count/type/index are live independently at the actual
    # original reload boundaries. No substitution for replay length calculation.
    for at,value in ((0xf10c,BASE+0x9000),(0x8004,0),(0x8004,2),(0x8000,47)):
        add(mode=2,replay=1,calc=1,oldest=0x8000,mutations=[(at,value)])
    for at,value in ((0xf148,BASE+0x15000),(0x14000+48*24+4,44),
                     (0xf10c,BASE+0x8ffc),(0x8004,0),(0x8004,2),(0x8000,47),
                     (0xf168,0),(0xf174,1),(0x20028,0),(0xf114,BASE+0x5000),
                     (0xf11c,BASE+0x8100),(0xf144,2),(0xf154,8)):
        add(mode=2,replay=1,calc=1,reentrancy=1,oldest=0x8000,
            trigger=3,calc_location=1,mutations=[(at,value)])
    # Controlled unmapped callback target: no foreign code/data read is needed.
    for slot in (0xd00c,0xd010,0x14000+48*24+8):
        add(mode=0,replay=1,calc=1,stopped=1 if slot==0xd010 else 0,
            flags=6,patches=[(slot,0x23040000)])
    # Null/invalid field pointers retain actual normal-or-terminal behavior.
    for mode in (0,1,2):
        for replay in (0,1):
            for where,value in ((0x100,0),(0x100+0x240,0),(0x100+0x224,BASE+SIZE),
                                (0x100+0x220,BASE+SIZE),(0xf114,BASE+SIZE),
                                (0xf148,BASE+SIZE),(0xf11c,BASE+SIZE)):
                add(mode=mode,replay=replay,patches=[(where,value)])
    # Child captured before f0 store; child next loaded after recursive stores.
    for displacement in (-4,0,4):
        add(mode=1,patches=[(0x100+0x224,BASE+0x100+0xf0+displacement-0x220),
            (0x100+0xf0+displacement,0)])
        add(mode=1,patches=[(0x100+0x224,BASE+0x100+0xf0+displacement-0x220),
            (0x100+0xf0+displacement,0),(0x100+0xf0+displacement+4,0)])
    # First item retains type48's index through its callback; the next item
    # freshly loads mutated type47, whose length/callback identity differ.
    for count in (1,2):
        add(mode=2,replay=1,calc=1,reentrancy=1,oldest=0x8000,old_count=count,
            row47_length=40,row47_calc=1,history_first_stride=62 if count==1 else 106,
            trigger=3,calc_location=1,mutations=[(0x8000,47)])
    # Queued type2 reads its header unconditionally after the disabled gate.
    for disabled in (0,1):
        add(kind=2,disabled=disabled,patches=[(0x100+0x240,0)])
    return rows

def make_case(c):
    r=random.Random(c['seed']);b=bytearray(r.randbytes(SIZE))
    def put(at,value):b[at:at+4]=words([value&0xffffffff])
    def pointer(off):return BASE+off if off else 0
    objects=(0x100,0x900,0x1100,0x1900)
    for i,obj in enumerate(objects):
        put(obj,BASE+0x5000+i*0x100);put(obj+0x240,BASE+0x6800+i*24)
        put(obj+0x220,BASE+objects[i+1]if i<1 else 0);put(obj+0x224,0)
        b[obj+0x239]=c['kind'];b[obj+0xec]=c['stopped'];put(obj+0xf0,0x12345678)
        put(0x6800+i*24+8,c['status']);put(0x6800+i*24+4,c['flags'])
        for lane in range(12):put(0x502c+i*0x100+lane*4,0x3f800000+lane)
    if c['tree']:
        put(0x100+0x220,0);put(0x100+0x224,BASE+0x900)
        if c['tree']>=2:put(0x900+0x224,BASE+0x1100)
        if c['tree']>=3:put(0x900+0x220,BASE+0x1900)
        if c['tree']==4:put(0x1900+0x224,BASE+0x1100)
    # Raw initial program/globals and two concrete mutable callback tables.
    b[0xf000:0xf004]=bytes(4);b[0xf100:0xf178]=bytes(0x78);b[0x20000:0x24000]=bytes(0x4000)
    put(0xf000,c['disabled']);put(0xf168,BASE+0x8000 if c['replay']else 0)
    put(0xf114,BASE+c['local']);put(0xf100,BASE+c['local']+8)
    put(0xf148,BASE+0x14000);put(0xf150,BASE+c['physical']);put(0xf124,BASE+c['working'])
    put(0xf10c,pointer(c['oldest']));put(0xf11c,BASE+c['record']);put(0xf154,c['local_size'])
    put(0xf144,c['reentrancy']);put(0xf174,c['action']);put(0x20028,c['racing'])
    for i in range(5):put(0xf130+i*4,48)
    for table in (0x14000,0x15000):
        b[table:table+71*24]=bytes(71*24)
        for i in range(71):put(table+i*24+4,c['length'])
        put(table+48*24+8,0x23050020 if c['calc']else 0)
        put(table+47*24+4,c['length']if c['row47_length']is None else c['row47_length']);put(table+47*24+8,0x23050050 if c['row47_calc']else 0)
    put(0x15000+48*24+8,0x23050050 if c['calc']else 0)
    put(0xd00c,0x23050010 if c['changed']else 0);put(0xd010,0x23050000 if c['halted']else 0)
    # Valid initialized ring history; no stale/private stack values are read.
    stride=c['length']+(c['calc_return']if c['calc']else 0)+14
    for off in range(0x8000,0xc000,stride):
        if off+stride<=SIZE:put(off,48);put(off+4,1);b[off+stride-2:off+stride]=struct.pack('<H',stride-2)
    if c['oldest']:
        # Materialize the actual follow-on history from this session, including
        # its count and the directed callback's changed count/fixed length.
        count=c['old_count'];following_length=c['length'];following_calc=c['calc_return']if c['calc']else 0
        for at,value in c['mutations']:
            if at==c['oldest']+4:count=value
            if at==0x14000+48*24+4:following_length=value
        next_stride=following_length+following_calc+14
        first_stride=count*(following_length+following_calc+4)+10
        if c['history_first_stride']is not None:first_stride=c['history_first_stride']
        for off in range(c['oldest']+first_stride,0xc000,next_stride):
            if off+next_stride<=SIZE:put(off,48);put(off+4,1);b[off+next_stride-2:off+next_stride]=struct.pack('<H',next_stride-2)
        put(c['oldest'],c['old_type']);put(c['oldest']+4,c['old_count'])
    put(c['local'],48);put(c['local']+4,c['local_count'])
    for at,value in c['patches']:put(at,value)
    for at,value in c['byte_patches']:b[at]=value
    for i,(at,value)in enumerate(c['mutations']):put(0xe000+i*8,at);put(0xe004+i*8,value)
    h=[c['mode'],pointer(c['object']),pointer(c['callbacks']),c['time'],0xcccccccc,
       len(c['mutations']),c['halt_return'],c['calc_return'],c['trigger'],c['calc_location']]+[0]*14
    return words(h)+b

def completion_native_source(before,candidate):
    replay=before[str(PIPING)].decode().replace('\r\n','\n')
    phil=before[str(SOURCE)].decode().replace('\r\n','\n')
    lower_names=('ReplayCopyForward','LengthOfSession','EndPipingSession2','ARStartPipingSession','ARAddDataToSession','ARDoSingleVariedSession')
    lower='\n'.join(extract(replay,n)for n in lower_names)
    capsule={}
    if candidate:
        body=before[str(candidate)].decode().replace('\r\n','\n')
        capsule['mode']='Scratch override; actual production extraction not claimed'
    else:
        pattern=r'#if defined\(__GNUC__\)\s+typedef tU32 tCompletion_word[^\n]*;\s*#else\s*typedef tU32 tCompletion_word;\s*#endif'
        typedef=re.search(pattern,phil).group(0);other=re.search(pattern,replay).group(0)
        tokenize=lambda text:re.findall(r'[A-Za-z_][A-Za-z_0-9]*|0[xX][0-9a-fA-F]+|[0-9]+|[^\s]',mask_c(text))
        assert tokenize(typedef)==tokenize(other),'Two TU typedef mismatch'
        word=extract(phil,'CompletionWord');second=extract(replay,'CompletionWord')
        assert tokenize(word)==tokenize(second),'Two TU private word-helper mismatch'
        names=('CompletionPointer','CompletionByte','CompletionStore','CompletionStatus','ChangedObjectsCallbacks','MarkObjectAndChildrenAsPassive')
        body=typedef+'\n'+word+'\n'+'\n'.join(extract(phil,n)for n in names)+'\n'+extract(replay,'PipeSinglePHILObject')
        capsule['mode']='Exact canonical two-TU bodies; shared identical private helper emitted once in merged native input'
        capsule['two_tu_helper_tokens_sha256']=sha(json.dumps(tokenize(word)).encode())
        capsule['bodies']={name:sha(extract(phil,name).encode())for name in ('CompletionWord',)+names}
        capsule['bodies']['23:CompletionWord']=sha(second.encode())
        capsule['bodies']['PipeSinglePHILObject']=sha(extract(replay,'PipeSinglePHILObject').encode())
    capsule['lower_bodies']={name:sha(extract(replay,name).encode())for name in lower_names}
    native=COMMON+'\n'+lower+'\n'+body
    native+='\nint C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;'+''.join('addresses[%d]=(unsigned)%s;'%(i,n)for i,(n,_,_)in enumerate(ROOTS))+'return run(argv[1]);}\n'
    return native,capsule

def matching_packet(image_path,symbol_path,before,out):
    data=image_path.read_bytes();mapping=json.loads(symbol_path.read_text())
    assert mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(data)
    symbols={s['orig']:s['recomp']for s in mapping['symbols']}
    map_path=image_path.with_suffix('.map');map_bytes=map_path.read_bytes();map_text=map_bytes.decode('latin1')
    map_rows=re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',map_text,re.M)
    function_aliases={}
    for name,address,owner in map_rows:
        function_aliases.setdefault(int(address,16),set()).add((name,owner.strip()))
    functions={address:sorted(identities)[0]for address,identities in function_aliases.items()}
    # Legacy CRT aliases omit the MAP f flag. Admit only the authentic paired
    # code identities and verify the entire47-byte probe against the original.
    probe_aliases=re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(__chkstk|__alloca_probe)\s+([0-9a-fA-F]{8})\s+LIBC:chkstk\.obj\s*$',map_text,re.M)
    assert {name for name,address in probe_aliases}=={'__chkstk','__alloca_probe'}
    assert len({address for name,address in probe_aliases})==1
    probe=int(probe_aliases[0][1],16)
    assert sha(raw(data,probe,47))=='4be32df90e8e3db3f5eed0d31857bddd88255c7ed09ae57e0e20e4510124a255'
    functions[probe]=('__chkstk','LIBC:chkstk.obj')
    assert not function_aliases.get(probe)or function_aliases[probe].issubset({('__chkstk','LIBC:chkstk.obj'),('__alloca_probe','LIBC:chkstk.obj')})
    function_aliases[probe]={('__chkstk','LIBC:chkstk.obj')}
    nt,base,size,tab=pe(data);cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
    sections=[]
    for j in range(struct.unpack_from('<H',data,nt+6)[0]):
        vs,rva,n,off=struct.unpack_from('<IIII',data,tab+40*j+8)
        sections.append((base+rva,max(vs,n),struct.unpack_from('<I',data,tab+40*j+36)[0]))
    globals_={0x65d004:('gPHIL_disabled',0xf000,4),0x676908:('gPipe_buffer_start',0xf168,4),
        0x6768b4:('gLocal_buffer',0xf114,4),0x6768a0:('gMr_chunky',0xf100,4),
        0x6768e8:('gPipe_callbacks',0xf148,4),0x6768f0:('gPipe_buffer_phys_end',0xf150,4),
        0x6768c4:('gPipe_buffer_working_end',0xf124,4),0x6768ac:('gPipe_buffer_oldest',0xf10c,4),
        0x6768bc:('gPipe_record_ptr',0xf11c,4),0x6768f4:('gLocal_buffer_size',0xf154,4),
        0x6768e4:('gReentrancy_count',0xf144,4),0x6768d0:('gReentrancy_array',0xf130,20),
        0x676914:('gAction_replay_mode',0xf174,4),0x75bb80:('gProgram_state',0x20000,0x4000)}
    mutable={};global_rows=[];all_sources='\n'.join(v.decode('latin1')for p,v in before.items()if p.endswith('.c'))
    for original,(name,offset,n)in globals_.items():
        found=re.findall(r'^\s*000[23]:[0-9a-fA-F]{8}\s+_'+re.escape(name)+r'\s+([0-9a-fA-F]{8})\s+(.+)$',map_text,re.M)
        assert len(found)==1,(name,found)
        address=int(found[0][0],16);owner=found[0][1].strip()
        assert original not in symbols or symbols[original]==address,(name,'MAP/symbol disagreement')
        assert re.search(r'// GLOBAL: CARMA2_HW 0x00'+format(original,'06x')+r'\s+[^;{}]*\b'+re.escape(name)+r'\b',all_sources),(name,'source identity')
        section=next(s for s in sections if s[0]<=address<s[0]+s[1]);assert section[2]&0x80000000 and not section[2]&0x20
        mutable[address]=(BASE+offset,n);symbols[original]=address
        global_rows.append(dict(original=original,name=name,address=address,bytes=n,arena_offset=offset,owner=owner))
    array=symbols[0x6768d0];replay_callbacks=symbols[0x6768e8]
    public={symbols[a]:name for name,a,n in ROOTS+LOWER if a in symbols}
    extra=((0x4025e0,'ARStartPipingSession'),(0x4028a0,'ARAddDataToSession'),(0x403750,'LengthOfSession'))
    public.update({symbols[a]:name for a,name in extra if a in symbols})
    roots=[symbols[a]for name,a,n in ROOTS];fatal=symbols[0x44c230]
    todo=list(roots);seen=set();graph=[];edges=[];patches={};callbacks=[];array_minus4=[];scalar_immediates=[]
    families={alias:family for family,aliases in {'eax':('eax','ax','al','ah'),'ebx':('ebx','bx','bl','bh'),
        'ecx':('ecx','cx','cl','ch'),'edx':('edx','dx','dl','dh'),'esi':('esi','si'),'edi':('edi','di'),
        'ebp':('ebp','bp'),'esp':('esp','sp')}.items()for alias in aliases}
    def reg(op):return families.get(cs.reg_name(op.reg))if op.type==CS_OP_REG else None
    def patch(at,value,target,absolute):
        row=(at-base,value,target,int(absolute));assert at-base not in patches or patches[at-base]==row
        patches[at-base]=row
    while todo:
        start=todo.pop(0)
        if start in seen:continue
        seen.add(start);assert start in functions,hex(start)
        assert len(function_aliases[start])==1,('ambiguous reachable MAP identity',hex(start),function_aliases[start])
        name,owner=functions[start];plain=name.lstrip('_@').split('@')[0]
        if start in public:
            expected_owner='60-phil.c.obj'if public[start]in('ChangedObjectsCallbacks','MarkObjectAndChildrenAsPassive')else '23-piping.c.obj'
            if public[start]=='__chkstk':assert owner=='LIBC:chkstk.obj' and plain in('chkstk','_chkstk')
            else:assert plain==public[start]and owner==expected_owner,(name,owner,public[start])
        else:
            assert (owner in('23-piping.c.obj','60-phil.c.obj')and plain.startswith(('Completion','Replay'))or owner=='LIBC:chkstk.obj'and plain in('chkstk','_chkstk')),(name,owner)
        end=min(at for at in functions if at>start)
        decoded={};occupied={};successors={};pending=[start];indirect=[]
        while pending:
            address=pending.pop()
            while address not in decoded:
                assert start<=address<end,(name,hex(address))
                ins=next(cs.disasm(raw(data,address,15),address));assert ins.size and address+ins.size<=end
                assert all(at not in occupied for at in range(address,address+ins.size)),('overlapping instruction',name,hex(address))
                for at in range(address,address+ins.size):occupied[at]=address
                decoded[address]=ins;successors[address]=[]if CS_GRP_RET in ins.groups or ins.mnemonic in('int','int3')else[address+ins.size]
                control=CS_GRP_CALL in ins.groups or CS_GRP_JUMP in ins.groups
                if CS_GRP_CALL in ins.groups:
                    assert ins.mnemonic=='call'and len(ins.operands)==1 and ins.operands[0].size==4,('non-near32 call',name,ins.op_str)
                    assert not any(p in (0x26,0x2e,0x36,0x3e,0x64,0x65,0x66)for p in ins.prefix),('call override prefix',name,ins.op_str)
                if control:
                    op=ins.operands[0]
                    if op.type==CS_OP_IMM:
                        target=op.imm;inside=start<=target<end
                        if inside:
                            if CS_GRP_CALL in ins.groups:assert target==start
                            else:pending.append(target);successors[address].append(target)
                        elif target!=fatal:
                            assert target in functions,(name,ins.op_str);todo.append(target)
                        edges.append(dict(owner=start,site=address,target=target,terminal=target==fatal))
                        if ins.mnemonic=='jmp':successors[address]=[target]if inside else[]
                    else:
                        assert CS_GRP_CALL in ins.groups and op.type in(CS_OP_REG,CS_OP_MEM),('unproved dispatch',name,ins.op_str)
                        indirect.append(ins)
                for op in ins.operands:
                    value=None
                    if op.type==CS_OP_MEM and base<=op.mem.disp<base+size:
                        value=op.mem.disp;at=address+ins.disp_offset;assert ins.disp_size==4
                    elif op.type==CS_OP_IMM and not control and base<=op.imm<base+size:
                        if ins.mnemonic=='or'and op.imm==0x400000 and ins.operands[0].type==CS_OP_REG and ins.operands[0].size==4:
                            assert public.get(start)=='ARDoSingleVariedSession'
                            scalar_immediates.append(dict(owner=start,site=address,value=op.imm,authority='Published varied writer signaling-NaN quiet bit; scalar OR immediate, not pointer'))
                            continue
                        assert ins.mnemonic in('mov','push','cmp'),(name,hex(address),ins.mnemonic,ins.op_str)
                        value=op.imm;at=address+ins.imm_offset;assert ins.imm_size==4
                    if value is None:continue
                    if op.type==CS_OP_MEM and value==array-4 and op.mem.index:
                        assert op.mem.scale==4 and op.size==4
                        target=BASE+0xf12c;array_minus4.append(dict(owner=start,site=address,address=value,index=cs.reg_name(op.mem.index),scale=4,arena_address=target))
                    else:
                        possible=[(a,d,n)for a,(d,n)in mutable.items()if a<=value<a+n]
                        assert len(possible)==1,('unauthenticated absolute data/alias',name,ins.op_str,hex(value))
                        a,d,n=possible[0];target=d+value-a
                    patch(at,value,target,True)
                if CS_GRP_RET in ins.groups or ins.mnemonic in('jmp','int','int3'):break
                address+=ins.size
        assert all(target in decoded for targets in successors.values()for target in targets),'noninstruction successor'
        # Must-dataflow authenticates every indirect callback against the actual
        # formal Physics callback base or fresh canonical replay callback table.
        # Partial family writes and volatile caller clobbers kill provenance.
        inputs={start:{'edx':('table','physics',0)}if start==roots[0]else{}};work=[start]
        def memory_value(op,state,at):
            if op.size!=4 or op.mem.segment:return None
            if not op.mem.base and not op.mem.index and op.mem.disp==replay_callbacks:return ('table','replay',0)
            b=state.get(families.get(cs.reg_name(op.mem.base)));i=state.get(families.get(cs.reg_name(op.mem.index)))if op.mem.index else None
            if b and b[0]=='table':
                offset=b[2]+op.mem.disp
                if b[1]=='physics'and not op.mem.index and offset in(12,16):return ('callback','changed'if offset==12 else'halted')
                if b[1]=='replay'and i and i[0]=='integer'and i[2]*op.mem.scale==24 and offset==8:return ('callback','calc_length')
            return ('integer',at,1)
        def lea_value(op,state):
            b=state.get(families.get(cs.reg_name(op.mem.base)));i=state.get(families.get(cs.reg_name(op.mem.index)))if op.mem.index else None
            if b and b[0]=='integer'and i and i==b and op.mem.disp==0:return ('integer',b[1],b[2]*(1+op.mem.scale))
            if b and b[0]=='table'and not op.mem.index:return ('table',b[1],b[2]+op.mem.disp)
            return None
        while work:
            at=work.pop(0);ins=decoded[at];old=inputs[at];state=dict(old)
            for r in ins.regs_access()[1]:
                family=families.get(cs.reg_name(r))
                if family:state.pop(family,None)
            if CS_GRP_CALL in ins.groups:
                for family in('eax','ecx','edx'):state.pop(family,None)
            if len(ins.operands)==2:
                a,b=ins.operands;dest=reg(a);fact=None
                if dest and a.size==4 and cs.reg_name(a.reg)==dest:
                    if ins.mnemonic=='mov':
                        if b.type==CS_OP_REG:fact=old.get(reg(b))
                        elif b.type==CS_OP_MEM and b.size==4:fact=memory_value(b,old,at)
                    elif ins.mnemonic=='lea'and b.type==CS_OP_MEM:fact=lea_value(b,old)
                    elif ins.mnemonic=='shl'and b.type==CS_OP_IMM:
                        previous=old.get(dest)
                        if previous and previous[0]=='integer':fact=('integer',previous[1],previous[2]*(1<<b.imm))
                    if fact:state[dest]=fact
            for target in successors[at]:
                prior=inputs.get(target);merged=dict(state)if prior is None else{k:v for k,v in prior.items()if state.get(k)==v}
                if prior is None or merged!=prior:inputs[target]=merged;work.append(target)
        for ins in indirect:
            op=ins.operands[0];state=inputs.get(ins.address,{})
            fact=state.get(reg(op))if op.type==CS_OP_REG else memory_value(op,state,ins.address)
            assert fact and fact[0]=='callback',('unproved callback must-provenance',name,hex(ins.address),ins.op_str,state)
            callbacks.append(dict(owner=start,site=ins.address,kind=fact[1],operand=ins.op_str,authority='All-path CFG formal/table provenance, field offsets and24-byte index; full/partial family writes and call-volatile kills'))
        graph.append(dict(name=public.get(start,plain),map_name=name,owner=owner,address=start,extent=end-start,instructions=[(a,decoded[a].bytes.hex())for a in sorted(decoded)],instruction_sha256=sha(b''.join(decoded[a].bytes for a in sorted(decoded)))))
    active={at+k for row in graph for at,bits in row['instructions']for k in range(len(bits)//2)}
    rr,rz=struct.unpack_from('<II',data,nt+24+96+5*8);cursor=base+rr;checked=[]
    while cursor<base+rr+rz:
        page,n=struct.unpack('<II',raw(data,cursor,8));assert n>=8
        for word in struct.unpack('<'+'H'*((n-8)//2),raw(data,cursor+8,n-8)):
            if word>>12==3 and base+page+(word&4095)in active:
                at=page+(word&4095);assert at in patches;checked.append(at)
        cursor+=n
    assert len(callbacks)==4 and sorted(c['kind']for c in callbacks)==['calc_length','calc_length','changed','halted']
    blob=words([a-base for a in roots]+[len(patches),fatal-base])+b''.join(words(v)for _,v in sorted(patches.items()))
    (out/'matching-addresses.bin').write_bytes(blob)
    return dict(recompiled_sha256=sha(data),symbols_sha256=sha(symbol_path.read_bytes()),map_sha256=sha(map_bytes),graph=graph,edges=edges,callbacks=callbacks,patches=list(patches.values()),relocations_checked=checked,mutable_globals=global_rows,array_minus4=array_minus4,packet_sha256=sha(blob),terminal_fatal=fatal,tables=[],crt_probe_aliases=probe_aliases,scalar_immediates=scalar_immediates)

def main():
    a=argparse.ArgumentParser();a.add_argument('--output',required=True,type=Path)
    a.add_argument('--case-index',type=int,action='append');a.add_argument('--limit',type=int)
    a.add_argument('--package-only',action='store_true');a.add_argument('--candidate-source',type=Path);a.add_argument('--recompiled',type=Path);a.add_argument('--symbols',type=Path);args=a.parse_args();assert bool(args.recompiled)==bool(args.symbols)
    out=args.output.resolve();assert not out.exists();out.mkdir(parents=True)
    candidate=args.candidate_source.resolve()if args.candidate_source else None;replay=PIPING
    paths=[Path(__file__),replay,ROOT/'src/carma2/newcommon/60-phil.c',
        ROOT/'src/brender/core/math/matrix34.c',ROOT/'scripts/verify_phil_registry_reconstruction.py',
        ROOT/'scripts/verify_physics_registration_reconstruction.py',ROOT/'scripts/verify_opponent_navigation_reconstruction.py',
        ROOT/'scripts/verify_tinted_native_reconstruction.py',ROOT/'scripts/verify_opponent_route_reconstruction.py',
        ROOT/'scripts/inventory_reconstruction_backlog.py']
    paths+=[ROOT/'src/carma2/newcommon/globvars.c']
    if candidate:paths.append(candidate)
    if args.recompiled:paths.extend([args.recompiled.resolve(),args.symbols.resolve(),args.recompiled.with_suffix('.map').resolve()])
    paths+=list((ROOT/'src').rglob('*.h'));paths=list(dict.fromkeys(p.resolve()for p in paths))
    before={str(p):p.read_bytes()for p in paths};snap=out/'sources';snap.mkdir();snapshots={}
    for i,(path,bits)in enumerate(before.items()):
        target=snap/(str(i)+'-'+Path(path).name);target.write_bytes(bits)
        snapshots[path]=dict(path=str(target),sha256=sha(bits))
    original=(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes();assert sha(original)==HASH
    meta=packet(original,out);common=COMMON
    matching=matching_packet(args.recompiled.resolve(),args.symbols.resolve(),before,out)if args.recompiled else None
    native,capsule=completion_native_source(before,candidate)
    caller,physical=bridge();registry.COMMON='#define PHYSICAL_IMAGE\n'+common
    cases=fixtures();indices=args.case_index or list(range(len(cases)));indices=indices[:args.limit]if args.limit else indices
    payloads=[make_case(cases[i])for i in indices]
    record=dict(stage='Packaged; no acceptance',original_sha256=HASH,original=meta,
        matching=matching,native_capsule=capsule,total_scenes=len(cases),selected_indices=indices,fixture_sha256=[sha(b)for b in payloads],
        sources={k:sha(v)for k,v in before.items()},source_snapshots=snapshots,
        published_head=subprocess.check_output(['git','rev-parse','HEAD'],text=True).strip(),
        complete_entries=ROOTS,complete_suite=indices==list(range(len(cases))),
        relocated_storage_qualifiers=[i for i in indices if cases[i]['reentrancy']not in range(6)],
        limitations=['PC53 masked exceptions/DF0; floating flags excluded.',
         'Replay globals are relocated into controlled storage preserving original adjacency. Out-of-array reentrancy fixtures are storage qualifiers only: original working_end is array-12, whereas this linked build places it at array-8. They do not establish unmodified linked global-layout equivalence; ordinary game-layout claims require legal array slots0..4.',
         'Historical malformed fixture468 and its foreign-memory-dependent terminal mismatch are retained separately as a zero-credit environment qualification; it is not normalized into a passing result.',
         'Callbacks are controlled external boundaries with public-state snapshots and real mutations.',
         'Private stack callback packet pointer identity is disclosed as0xffffffff; its initialized52 bytes are compared.',
         'Nonterminating callback-created cycles excluded; terminal AV/FatalError retains full public state, no normal ABI claim.',
         'Variable payload lengths greater than initialized48 matrix bytes are excluded unless an actual terminal address fault precedes copy.'])
    (out/'package.json').write_text(json.dumps(record,indent=2)+'\n')
    (out/'native.c').write_text(native);(out/'caller.c').write_text(caller);(out/'physical.c').write_text(registry.COMMON+physical)
    if args.package_only:
        assert all(Path(p).read_bytes()==b for p,b in before.items());return
    try:commands=registry.compile_sources(out,native,physical,caller)
    except Exception:
        (out/'compile-guard-closure.json').write_text(json.dumps(dict(source_stable=all(Path(p).read_bytes()==b for p,b in before.items()),sources=record['sources']),indent=2)+'\n');raise
    results=[];failures=[]
    for index,payload in zip(indices,payloads):
        case=out/'case.bin';case.write_bytes(payload)
        runs=[('original',[str(out/'physical.exe'),str(case),str(ROOT/'build-reference/originals/CARMA2_HW.EXE'),str(out/'original-addresses.bin')])]+[(name,[str(out/(name+'.exe')),str(case)])for name in ('msvc5','msvc','mingw')]
        if matching:runs.append(('matching',[str(out/'physical.exe'),str(case),str(args.recompiled.resolve()),str(out/'matching-addresses.bin')]))
        expected=None
        for label,command in runs:
            r=subprocess.run(command,capture_output=True,timeout=30);(out/('%d-%s.bin'%(index,label))).write_bytes(r.stdout)
            if label=='original':
                assert r.returncode==0,(index,r.returncode);expected=r.stdout;assert expected and len(expected)%(16+52+SIZE)==0;continue
            row=dict(index=index,compiler=label,exit=r.returncode,fixture_sha256=sha(payload),
                original_output_sha256=sha(expected),original_output_bytes=len(expected),
                actual_output_sha256=sha(r.stdout),actual_output_bytes=len(r.stdout),passed=r.returncode==0 and r.stdout==expected)
            results.append(row)
            if not row['passed']:failures.append(row)
        print(index,len(results),len(failures),flush=True)
    stable=all(Path(p).read_bytes()==b for p,b in before.items());assert stable
    record.update(stage='Complete original/native3/current executable proof'if matching else'Complete original/native3 proof',scenes=len(indices),comparisons=len(results),source_stable=stable,results=results,failures=failures,commands=commands,
        artifact_sha256={str(f):sha(f.read_bytes())for f in out.iterdir()if f.suffix in('.c','.obj','.exe','.bin','.txt')})
    (out/'verification.json').write_text(json.dumps(record,indent=2)+'\n');assert not failures

if __name__=='__main__':main()
