"""Complete PipeSingleSound and genuine outlet/replay lower proof; no substitutes."""
import argparse,ast,hashlib,json,random,re,struct,subprocess,sys
from pathlib import Path
from capstone import *
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'src/carma2').is_dir());R=ROOT
sys.path.insert(0,str(R/'scripts'))
import verify_physics_registration_reconstruction as registration
import verify_physics_completion_reconstruction as completion
import verify_phil_registry_reconstruction as registry
from verify_opponent_route_reconstruction import HASH,sha,words,raw,pe
from verify_tinted_native_reconstruction import extract
ROOTS=[('PipeSingleSound',0x4c84a0,95),('GetIndexFromOutlet',0x4566b0,26)]
LOWER=[('ARDoSingleVariedSession',0x402bd0,503),('EndPipingSession2',0x402660,555),('__chkstk',0x575bb0,47)]
BASE=0x23000000;SIZE=0x40000
MUTABLE={0x6768a0:('CanonicalReplayGlobals',0xf100,0x78),0x75bb80:('gProgram_state',0x20000,0x4000),0x79e160:('PhysicalOutletReadStorage',0x16000,28),0x6abee8:('gZero_vector__smash',0xf200,12)}

COMMON='/* Complete completion/replay proof. Callbacks are external observations;\n * all game/replay lower bodies execute. Fixed callback thunks avoid pointer\n * normalization in the full public arena. Masked x87 PC53, DF clear. */\n#include <windows.h>\n#include <stdio.h>\n#include <stdlib.h>\n#include <string.h>\n#include <stdint.h>\n#include <stdarg.h>\n#include <float.h>\n#include <io.h>\n#include <fcntl.h>\n#include "60-phil.h"\n#include "23-piping.h"\n#include "69-sound.h"\n#include "rec2_macros.h"\n#include "52-errors.h"\n#include "globvars.h"\nextern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[6];\nextern unsigned builder_stack_count,builder_result,builder_poison,builder_cdecl;\nextern unsigned __cdecl BuilderInvoke(void);\nstatic unsigned addresses[2],h[24],case_index,callback_calls;\nstatic unsigned char* heap;\nenum {ARENA_BYTES=0x40000};\n#define gIndexed_outlets (*(tS3_outlet* (*)[SOUND_OUTLET_SLOTS])(heap+0x16000))\n#define gZero_vector__smash (*(br_vector3*)(heap+0xf200))\n#define gPipe_buffer_start (*(tU8**)(heap+0xf168))\n#define gLocal_buffer (*(tU8**)(heap+0xf114))\n#define gMr_chunky (*(tU8**)(heap+0xf100))\n#define gPipe_callbacks (*(const tReplay_callback**)(heap+0xf148))\n#define gPipe_buffer_phys_end (*(tU8**)(heap+0xf150))\n#define gPipe_buffer_working_end (*(tU8**)(heap+0xf124))\n#define gPipe_buffer_oldest (*(tU8**)(heap+0xf10c))\n#define gPipe_record_ptr (*(tU8**)(heap+0xf11c))\n#define gLocal_buffer_size (*(tU32*)(heap+0xf154))\n#define gReentrancy_count (*(int*)(heap+0xf144))\n#define gReentrancy_array ((tPipe_chunk_type*)(heap+0xf130))\n#define gAction_replay_mode (*(int*)(heap+0xf174))\n#define gProgram_state (*(tProgram_state*)(heap+0x20000))\ntypedef char SoundLayouts[sizeof(void*)==4&&sizeof(tReplay_callback)==24&&\n    sizeof(tPipe_shunk_single_sound)==20&&offsetof(tPipe_shunk_single_sound,outlet)==16&&\n    offsetof(tPipe_shunk_single_sound,volume)==18&&offsetof(tProgram_state,racing)==0x28?1:-1];\nstatic unsigned rd(unsigned offset) {unsigned value;memcpy(&value,heap+offset,4);return value;}\nstatic void wr(unsigned offset,unsigned value) {memcpy(heap+offset,&value,4);}\nstatic void snapshot(unsigned event,unsigned value,unsigned detail,const void* packet) {\n    unsigned header[4];unsigned char bytes[24];\n    header[0]=case_index;header[1]=event;header[2]=value;header[3]=detail;\n    memset(bytes,0,sizeof(bytes));if(packet)memcpy(bytes,packet,sizeof(bytes));\n    fwrite(header,sizeof(header),1,stdout);fwrite(bytes,sizeof(bytes),1,stdout);\n    fwrite(heap,ARENA_BYTES,1,stdout);\n}\n/* Mutation records are raw offset/value pairs, applied once after the first\n * observed callback. They exercise actual mutable table/object/global state. */\nstatic void mutate(unsigned kind,unsigned pointer) {unsigned i;\n    if(h[10]&&h[10]!=kind)return;\n    if(kind==3&&h[11]==1&&(pointer<0x23000000||pointer>=0x23040000))return;\n    if(kind==3&&h[11]==2&&(pointer>=0x23000000&&pointer<0x23040000))return;\n    if(callback_calls++!=0)return;\n    for(i=0;i<h[8];i++)wr(rd(0xe000+i*8),rd(0xe004+i*8));\n}\nstatic int calc_action(tPipe_chunk* chunk,unsigned which) {\n    unsigned address=(unsigned)(uintptr_t)chunk;\n    snapshot(3,which,(address>=0x23000000&&address<0x23040000)?address:0xffffffffu,chunk);\n    mutate(3,address);return (int)h[9];\n}\nstatic int __fastcall original_calc(tPipe_chunk* p) {return calc_action(p,0);}\nstatic int C2_HOOK_FASTCALL native_calc(tPipe_chunk* p) {return calc_action(p,0);}\nstatic int __fastcall original_calc2(tPipe_chunk* p) {return calc_action(p,1);}\nstatic int C2_HOOK_FASTCALL native_calc2(tPipe_chunk* p) {return calc_action(p,1);}\nvoid C2_NORETURN C2_HOOK_CDECL FatalError(int value,...) {\n    snapshot(4,(unsigned)value,0,NULL);fflush(stdout);ExitProcess(0);\n}\nstatic LONG WINAPI fault(EXCEPTION_POINTERS* x) {\n    if(x->ExceptionRecord->ExceptionCode!=EXCEPTION_ACCESS_VIOLATION)return EXCEPTION_CONTINUE_SEARCH;\n    snapshot(5,x->ExceptionRecord->ExceptionCode,(unsigned)x->ExceptionRecord->ExceptionInformation[1],NULL);\n    fflush(stdout);ExitProcess(0);return EXCEPTION_CONTINUE_SEARCH;\n}\nstatic void install_observer(void) {\n    typedef void*(WINAPI *install_fn)(ULONG,LONG(WINAPI*)(EXCEPTION_POINTERS*));\n    FARPROC address=GetProcAddress(GetModuleHandleA("kernel32.dll"),"AddVectoredExceptionHandler");install_fn fn;\n    memcpy(&fn,&address,4);if(!fn||!fn(1,fault))exit(38);\n}\nstatic void install_thunks(void) {\n    unsigned i,value;unsigned char* thunks;unsigned targets[2];\n    thunks=(unsigned char*)VirtualAlloc((void*)0x23050000,0x1000,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);\n    if(thunks!=(void*)0x23050000)exit(39);\n#ifdef PHYSICAL_IMAGE\n    targets[0]=(unsigned)original_calc;targets[1]=(unsigned)original_calc2;\n    (void)native_calc;(void)native_calc2;\n#else\n    targets[0]=(unsigned)native_calc;targets[1]=(unsigned)native_calc2;\n    (void)original_calc;(void)original_calc2;\n#endif\n    for(i=0;i<2;i++){thunks[i*16]=0xe9;value=targets[i]-((unsigned)thunks+i*16+5);memcpy(thunks+i*16+1,&value,4);}\n    FlushInstructionCache(GetCurrentProcess(),thunks,0x1000);\n}\nstatic int run(const char* path) {\n    FILE*f;unsigned mode;SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);\n    f=fopen(path,"rb");if(!f)return 10;\n    heap=(unsigned char*)VirtualAlloc((void*)0x23000000,ARENA_BYTES,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);\n    if(heap!=(void*)0x23000000)return 11;\n    _setmode(_fileno(stdout),_O_BINARY);install_observer();install_thunks();\n    for(case_index=0;fread(h,sizeof(h),1,f)==1;case_index++){\n        if(fread(heap,ARENA_BYTES,1,f)!=1)return 12;\n        mode=h[0];if(mode>=2)return 37;callback_calls=0;\n        _controlfp(_PC_53,_MCW_PC);_clearfp();\n        builder_mode=mode==0?1:0;builder_address=addresses[mode];builder_x=h[1];builder_y=h[2];\n#if defined(REC2_MATCHING)\n        builder_cdecl=0;builder_stack_count=mode==0?4:0;\n        builder_stack[0]=h[3];builder_stack[1]=h[4];builder_stack[2]=h[5];builder_stack[3]=h[6];\n#else\n        builder_cdecl=1;builder_stack_count=mode==0?6:1;\n        builder_stack[0]=h[1];builder_stack[1]=h[2];builder_stack[2]=h[3];builder_stack[3]=h[4];builder_stack[4]=h[5];builder_stack[5]=h[6];\n#endif\n        builder_poison=h[7];if(BuilderInvoke())exit(14);\n        snapshot(0,builder_result,0,NULL);\n    }\n    fclose(f);return 0;\n}\n'

def packet(data,out):
    assert sha(data)==HASH
    registration.ROOTS=ROOTS;registration.MUTABLE=MUTABLE;registration.BASE=BASE
    registration.EXIT=0x44c230;registration.TABLES={};registration.INDIRECT={0x402bd0,0x402660}
    meta=registration.original_packet(data,{a:(name,n)for name,a,n in ROOTS+LOWER},out)
    # The complete getter compares against the one-past physical endpoint.
    # A data-region interior test alone cannot authenticate/transport that CMP.
    assert raw(data,0x4566bf,6)==bytes.fromhex('81fa7ce17900')
    assert not any(row[0]==0x4566c1-0x400000 for row in meta['patches'])
    meta['patches'].append((0x4566c1-0x400000,0x79e17c,BASE+0x1601c,1))
    meta['one_past_compare_authority']=dict(site='0x4566bf',bytes='81fa7ce17900',operand_site='0x4566c1',endpoint='0x79e17c',arena_endpoint=hex(BASE+0x1601c))
    blob=words([address-0x400000 for name,address,n in ROOTS]+[len(meta['patches']),0x44c230-0x400000])+b''.join(words(row)for row in sorted(meta['patches']))
    (out/'original-addresses.bin').write_bytes(blob);meta['packet_sha256']=sha(blob)
    (out/'original-packet.json').write_text(json.dumps(meta,indent=2)+'\n')
    assert len(meta['graph'])==5 and sum(row['bytes']for row in meta['graph'])==1226
    assert {(row['owner'],row['site'])for row in meta['indirect_sites']}=={(0x402bd0,0x402d10),(0x402660,0x40274d)}
    return meta

def bridge():
    caller,physical=completion.bridge()
    caller=caller.replace('builder_stack[5]','builder_stack[6]')
    physical=physical.replace('hdr[5]','hdr[4]').replace('n=hdr[3]','n=hdr[2]').replace('i<3;i++)addresses','i<2;i++)addresses')
    physical=physical.replace('image[hdr[4]]','image[hdr[3]]').replace('image+hdr[4]+5','image+hdr[3]+5').replace('image+hdr[4]+1','image+hdr[3]+1')
    return caller,physical

def cases():
    rows=[]
    def add(**values):
        c=dict(mode=0,outlet=3,sound=123,left=255,right=128,pitch=0x10000,position=0x6000,
            action=0,racing=1,replay=1,calc=0,calc_return=0,length=20,
            reentrancy=0,old_type=2,old_count=1,local_count=0,local_size=8,
            local=0x4000,record=0x8000,oldest=0,physical=0xc000,working=0xc000,
            mutations=[],trigger=0,calc_location=0,patches=[],duplicates=[],
            poison=0xcccccccc,seed=len(rows)+1)
        c.update(values);rows.append(c)
    # First four preserve direct and whole-consumer seventh-slot counterexamples.
    for mode in(1,0):add(mode=mode,outlet=6)
    for mode in(1,0):add(mode=mode,outlet=None)
    for mode in(0,1):
        for outlet in range(8):add(mode=mode,outlet=outlet)
    for mode in(0,1):
        for outlet in range(7):
            add(mode=mode,outlet=outlet,duplicates=[(0,outlet)])
    for action in(0,1,0xffffffff,0x80000000):
        for racing in(0,1,0xffffffff):
            for replay in(0,1):
                for position in(0,0x6000):add(action=action,racing=racing,replay=replay,position=position)
    integers=(0,1,255,256,257,0x7fffffff,0x80000000,0xffffffff,0xffff0000,0x10000)
    for left in integers:
        for right in integers:add(left=left,right=right)
    for value in integers:
        for field in('sound','pitch'):add(**{field:value})
    ieee=(0,0x80000000,1,0x80000001,0x00800000,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7f812345,0xff812345,0x7fc12345,0xffc12345)
    for position in(0,0x6000):
        for lane in range(3):
            for value in ieee:add(position=position,patches=[((0xf200 if not position else position)+4*lane,value)])
    # Position points into valid live replay state, tables, index storage or ring.
    for position in(0x4000,0x4004,0x4008,0x6001,0x6002,0x6004,0x8000,0x8010,0x16000,0x16004,0xf100,0xf144,0xf148,0xf168,0xf200):
        for calc in(0,1):add(position=position,calc=calc)
    for reentrancy in range(6):
        for calc in(0,1):
            for count in(0,1,2):add(reentrancy=reentrancy,calc=calc,local_count=count)
    # Ring capacity must leave room to advance an old34-byte history record
    # without immediately resetting oldest to start forever. Preserve the
    # original undersized history separately as a nontermination qualifier.
    for physical in(0x8050,0x8060,0x8100,0xbfe0,0xc000):
        aligned_tail=0x8000+((physical-0x8000-1)//34)*34
        for record in(0x8000,aligned_tail):add(physical=physical,record=record)
    for oldest in(0x8000,0x8008,0x8022,0x8ff0):
        for count in(0,1,2):
            for calc in(0,1):add(oldest=oldest,old_count=count,calc=calc)
    # These lengths remain within the initialized20-byte sound payload.
    for length,returned in((20,0),(16,4),(20,0xfffffffc),(16,0),(0,0),(1,0),(2,0),(3,0)):
        add(calc=1,length=length,calc_return=returned)
    for length in(1,3):add(calc=1,length=length,oldest=0x8000)
    # Callback replacements/mutations are real external effects, observed first
    # on the private varied packet or on the public End history as selected.
    varied_mutations=[[(0xf148,BASE+0x15000)],[(0x14000+2*24+4,16)],[(0xf174,1)],[(0x20028,0)],[(0xf168,0)],
        [(0x16000+3*4,BASE+0x7770)],[(0xf200,0x7f812345)],[(0xf11c,BASE+0x8100)],[(0xf10c,0)]]
    for changes in varied_mutations:add(calc=1,mutations=changes,trigger=3,calc_location=2)
    end_mutations=[[(0xf148,BASE+0x15000)],[(0x14000+2*24+4,16)],[(0xf10c,BASE+0x8080)],[(0xf124,BASE+0x8040)],
        [(0xf11c,BASE+0x8100)],[(0xf168,0)],[(0xf174,1)],[(0x20028,0)],[(0x8000+4,0)],[(0x8000+4,2)]]
    for changes in end_mutations:add(calc=1,oldest=0x8000,mutations=changes,trigger=3,calc_location=1)
    # Distinct callback identity and fixed row length after captured type2→3.
    for count in(1,2):add(calc=1,oldest=0x8000,old_count=count,mutations=[(0x8000,3)],trigger=3,calc_location=1)
    for poison in(0,0xaaaaaaaa,0x55555555,0x3f800000):add(calc=1,poison=poison)
    for i in range(64):
        r=random.Random(0x4c84a0+i)
        add(outlet=r.randrange(8),sound=r.getrandbits(32),left=r.getrandbits(32),right=r.getrandbits(32),pitch=r.getrandbits(32),
            position=0 if i&1 else 0x6000,calc=i&1,patches=[(0x6000+4*j,r.getrandbits(32))for j in range(3)])
    return rows

def payload(c):
    rng=random.Random(c['seed']);b=bytearray(rng.randbytes(SIZE))
    def put(at,value):struct.pack_into('<I',b,at,value&0xffffffff)
    def pointer(at):return BASE+at if at else 0
    b[0xf100:0xf178]=bytes(0x78);b[0x20000:0x24000]=bytes(0x4000)
    put(0xf168,pointer(0x8000)if c['replay']else 0);put(0xf114,pointer(c['local']));put(0xf100,pointer(c['local']+8))
    put(0xf148,BASE+0x14000);put(0xf150,pointer(c['physical']));put(0xf124,pointer(c['working']))
    put(0xf10c,pointer(c['oldest']));put(0xf11c,pointer(c['record']));put(0xf154,c['local_size'])
    put(0xf144,c['reentrancy']);put(0xf174,c['action']);put(0x20028,c['racing'])
    for i in range(5):put(0xf130+4*i,2)
    for table in(0x14000,0x15000):
        b[table:table+71*24]=bytes(71*24)
        for i in range(71):put(table+i*24+4,c['length'])
        put(table+2*24+8,(0x23050000 if table==0x14000 else 0x23050010)if c['calc']else 0)
        put(table+3*24+4,16);put(table+3*24+8,0x23050010 if c['calc']else 0)
    for i in range(7):put(0x16000+i*4,BASE+0x18000+i*0x10)
    for lane in range(3):put(0xf200+4*lane,0);put(0x6000+4*lane,0x3f800000+lane)
    put(0x16018,0 if c['outlet']is None else BASE+0x18060)
    for at,from_slot in c['duplicates']:put(0x16000+4*at,BASE+0x18000+from_slot*0x10)
    length=c['length'];returned=c['calc_return']if c['calc']else 0
    total=(length+returned)&0xffffffff
    assert total<=20,'initialized sound payload limit'
    if c['reentrancy']:
        put(0xf154,8+c['local_count']*(total+4))
    stride=total+14
    for off in range(0x8000,0xc000,stride):
        if off+stride<=SIZE:put(off,2);put(off+4,1);struct.pack_into('<H',b,off+stride-2,stride-2)
    if c['oldest']:
        count=c['old_count'];fixed=total
        for at,value in c['mutations']:
            if at==c['oldest']+4:count=value
            if at==0x14000+2*24+4:fixed=(value+returned)&0xffffffff
        first=count*(fixed+4)+10
        if any(at==c['oldest']and value==3 for at,value in c['mutations'])and count>0:
            # First item retains captured type2 row; subsequent items reload3.
            first=fixed+4+(count-1)*(16+returned+4)+10
        for off in range(c['oldest']+first,0xc000,fixed+14):
            if off+fixed+14<=SIZE:put(off,2);put(off+4,1);struct.pack_into('<H',b,off+fixed+12,fixed+12)
        put(c['oldest'],c['old_type']);put(c['oldest']+4,c['old_count'])
    put(c['local'],2);put(c['local']+4,c['local_count'])
    for at,value in c['patches']:put(at,value)
    for i,(at,value)in enumerate(c['mutations']):put(0xe000+8*i,at);put(0xe004+8*i,value)
    outlet=0 if c['outlet']is None else BASE+0x18000+c['outlet']*0x10
    h=[c['mode'],outlet,c['sound'],c['left'],c['right'],c['pitch'],pointer(c['position']),c['poison'],len(c['mutations']),c['calc_return'],c['trigger'],c['calc_location']]+[0]*12
    return struct.pack('<24I',*[value&0xffffffff for value in h])+b

def source_paths(source_dir):
    directory=source_dir if source_dir else R/'src/carma2/newcommon'
    return {name:(directory/name).resolve()for name in('23-piping.c','69-sound.c','51-smash.c','51-smash.h')}

def native_source(before,paths):
    source=before[str(paths['23-piping.c'])].decode().replace('\r\n','\n')
    sound=before[str(paths['69-sound.c'])].decode()
    smash=before[str(paths['51-smash.c'])].decode()
    header=before[str(paths['51-smash.h'])].decode()
    assert re.search(r'// GLOBAL: CARMA2_HW 0x0079e160\s+/\* Lookup includes the trailing zero-initialized DWORD\. \*/\s+tS3_outlet\* gIndexed_outlets\[7\];',sound),'authentic seven-DWORD storage declaration'
    assert len(re.findall(r'// GLOBAL: CARMA2_HW 0x006abee8\s+br_vector3 gZero_vector__smash;',smash))==1,'authentic mutable zero-vector owner'
    assert re.search(r'extern br_vector3 gZero_vector__smash;',header),'owning zero-vector declaration'
    lower_names=('ReplayCopyForward','LengthOfSession','EndPipingSession2','ARStartPipingSession','ARAddDataToSession','ARDoSingleVariedSession')
    lower='\n'.join(extract(source,name)for name in lower_names)+'\n'+extract(sound,'GetIndexFromOutlet')
    body=extract(source,'PipeSingleSound');assert 'NOT_IMPLEMENTED'not in body
    common='#define SOUND_OUTLET_SLOTS 7\n'+COMMON
    native=common+'\n'+lower+'\n'+body+'\nint C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;addresses[0]=(unsigned)PipeSingleSound;addresses[1]=(unsigned)GetIndexFromOutlet;return run(argv[1]);}\n'
    return native,dict(mode='Exact production PipeSingleSound/GetIndexFromOutlet and complete replay lower; seven-DWORD canonical storage and51-smash zero-vector owner authenticated',
        lower_bodies={name:sha(extract(source,name).encode())for name in lower_names},
        outlet_body_sha256=sha(extract(sound,'GetIndexFromOutlet').encode()),root_body_sha256=sha(body.encode()),
        outlet_storage_bytes=28,zero_vector_bytes=12,
        limits='Merged extraction preserves complete function bodies and ABI/schema definitions, not whole production translation-unit compiler context. Actual matching mode executes genuine compiled TU context.')

def storage_qualifiers(cases,payloads,indices):
    result=[]
    for i,bits in zip(indices,payloads):
        h=struct.unpack_from('<24I',bits);table=struct.unpack_from('<7I',bits,96+0x16000)
        c=cases[i];active=c['mode']==1 or(c['action']==0 and c['racing']!=0)
        if active and h[1]not in table[:6]and table[6]!=0:result.append(i)
    return result

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--case-index',type=int,action='append');parser.add_argument('--limit',type=int)
    parser.add_argument('--package-only',action='store_true');parser.add_argument('--source-dir',type=Path)
    parser.add_argument('--recompiled',type=Path);parser.add_argument('--symbols',type=Path);args=parser.parse_args()
    assert bool(args.recompiled)==bool(args.symbols)
    assert not args.source_dir or(args.package_only and not args.recompiled),'preview is package-only; actual proof is canonical production-default'
    paths=source_paths(args.source_dir.resolve()if args.source_dir else None)
    guarded=[Path(__file__).resolve(),*paths.values(),R/'src/carma2/newcommon/globvars.c']
    guarded += [R/'scripts'/name for name in('verify_physics_registration_reconstruction.py','verify_physics_completion_reconstruction.py','verify_phil_registry_reconstruction.py','verify_opponent_navigation_reconstruction.py','verify_opponent_route_reconstruction.py','verify_tinted_native_reconstruction.py','inventory_reconstruction_backlog.py')]
    guarded+=list((R/'src').rglob('*.h'))
    if args.recompiled:guarded += [args.recompiled.resolve(),args.symbols.resolve(),args.recompiled.with_suffix('.map').resolve()]
    guarded=list(dict.fromkeys(p.resolve()for p in guarded));before={str(p):p.read_bytes()for p in guarded}
    out=args.output.resolve();assert not out.exists();out.mkdir(parents=True);snap=out/'sources';snap.mkdir();snapshots={}
    for i,(path,bits)in enumerate(before.items()):
        target=snap/(str(i)+'-'+Path(path).name);target.write_bytes(bits);snapshots[path]=dict(path=str(target),sha256=sha(bits))
    original=(R/'build-reference/originals/CARMA2_HW.EXE').read_bytes();meta=packet(original,out)
    native,capsule=native_source(before,paths);common='#define SOUND_OUTLET_SLOTS 7\n'+COMMON
    matching=matching_packet(args.recompiled.resolve(),args.symbols.resolve(),before,out)if args.recompiled else None
    caller,physical=bridge();registry.COMMON='#define PHYSICAL_IMAGE\n'+common
    rows=cases();assert len(rows)==479
    indices=args.case_index or list(range(len(rows)));indices=indices[:args.limit]if args.limit else indices
    assert len(indices)==len(set(indices))and all(0<=i<len(rows)for i in indices)
    payloads=[payload(rows[i])for i in indices]
    record=dict(stage='Packaged complete sound/replay proof; no acceptance',sources={p:sha(b)for p,b in before.items()},source_snapshots=snapshots,
        original_sha256=HASH,original=meta,matching=matching,native_capsule=capsule,total_scenes=len(rows),selected_indices=indices,fixture_sha256=[sha(b)for b in payloads],
        complete_suite=indices==list(range(len(rows))),synthetic_seventh_read_qualifiers=storage_qualifiers(rows,payloads,indices),
        limitations=['All five original1226B bodies and real complete replay lower execute. Original one-past outlet endpoint operand is authenticated against entire4566bf81fa7ce17900 and28-byte physical read span.',
            'Original declared array extent/semantic seventh outlet identity is unknown. Canonical storage represents all seven observed DWORDs including trailing zero initialization; synthetic nonzero seventh reads are controlled storage qualifiers only.',
            'The independent mutable12-byte gZero_vector__smash fallback has authentic51-smash owning definition/declaration; it is not a substitute zero global or constant.',
            'Only24 initialized private varied-packet bytes plus full262144-byte public arena are serialized without normalization. Private packet address is disclosed as0xffffffff.',
            'Real calc_length callbacks observe and mutate public state; FatalError98 is a nonreturning public-prefix boundary. Terminal paths do not claim normal-return ABI or real error UI/shutdown equivalence.',
            'Historical undersized32-byte ring nontermination is preserved separately with zero equality credit. Normal capacity/wrap inputs are repaired with explicit immutable fixture lineage.',
            'Masked Win32 x86 PC53/DF0, bounded inputs; no exhaustive/unmasked/sticky-FPU/concurrent/foreign-memory/whole-game claim.',capsule['limits']])
    (out/'package.json').write_text(json.dumps(record,indent=2)+'\n')
    (out/'native.c').write_text(native);(out/'physical.c').write_text(registry.COMMON+physical);(out/'caller.c').write_text(caller)
    if args.package_only:
        assert all(Path(p).read_bytes()==b for p,b in before.items());return
    try:commands=registry.compile_sources(out,native,physical,caller)
    except Exception:
        (out/'compile-guard.json').write_text(json.dumps(dict(source_stable=all(Path(p).read_bytes()==b for p,b in before.items()),sources=record['sources']),indent=2));raise
    results=[];failures=[]
    for index,bits in zip(indices,payloads):
        case=out/'case.bin';case.write_bytes(bits);expected=None
        runs=[('original',[str(out/'physical.exe'),str(case),str(R/'build-reference/originals/CARMA2_HW.EXE'),str(out/'original-addresses.bin')])]+[(name,[str(out/(name+'.exe')),str(case)])for name in('msvc5','msvc','mingw')]
        if matching:runs.append(('matching',[str(out/'physical.exe'),str(case),str(args.recompiled.resolve()),str(out/'matching-addresses.bin')]))
        for name,command in runs:
            try:result=subprocess.run(command,capture_output=True,timeout=20)
            except subprocess.TimeoutExpired:
                (out/'timeout-guard.json').write_text(json.dumps(dict(source_stable=all(Path(p).read_bytes()==b for p,b in before.items()),selected_index=index,compiler=name,sources=record['sources'],completed_results=results,normal_abi_claim=False),indent=2));raise
            (out/('%d-%s.bin'%(index,name))).write_bytes(result.stdout);(out/('%d-%s.stderr.txt'%(index,name))).write_bytes(result.stderr)
            if name=='original':
                assert result.returncode==0 and result.stdout and len(result.stdout)%(16+24+SIZE)==0,(index,result.returncode)
                expected=result.stdout;continue
            row=dict(index=index,compiler=name,exit=result.returncode,passed=result.returncode==0 and result.stdout==expected,
                fixture_sha256=sha(bits),original_output_sha256=sha(expected),actual_output_sha256=sha(result.stdout),
                original_output_bytes=len(expected),actual_output_bytes=len(result.stdout))
            results.append(row)
            if not row['passed']:failures.append(row)
        if index%25==0:print(index,len(results),len(failures),flush=True)
    stable=all(Path(p).read_bytes()==b for p,b in before.items());assert stable
    record.update(stage='Complete original/native3/current executable proof'if matching else'Complete original/native3 proof',scenes=len(indices),comparisons=len(results),source_stable=stable,
        results=results,failures=failures,commands=commands,
        artifact_sha256={str(p):sha(p.read_bytes())for p in out.iterdir()if p.suffix in('.c','.obj','.exe','.bin','.txt')})
    (out/'verification.json').write_text(json.dumps(record,indent=2)+'\n');print('DONE',len(indices),len(results),len(failures));assert not failures

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
    globals_={0x79e160:('gIndexed_outlets',0x16000,28),0x6abee8:('gZero_vector__smash',0xf200,12),0x676908:('gPipe_buffer_start',0xf168,4),
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
        declaration=r'// GLOBAL: CARMA2_HW 0x00'+format(original,'06x')+r'\s+(?:/\*[^*]*\*/\s+)?[^;{}]*\b'+re.escape(name)+r'\b'
        expected_source='69-sound.c'if name=='gIndexed_outlets'else'51-smash.c'if name=='gZero_vector__smash'else'globvars.c'if name in('gProgram_state','gAction_replay_mode')else'23-piping.c'
        own=[v.decode('latin1')for p,v in before.items()if Path(p).name==expected_source]
        assert len(own)==1 and len(re.findall(declaration,own[0]))==1,(name,'canonical owning-source identity')
        assert len(re.findall(declaration,all_sources))==1,(name,'duplicate declaration')
        expected_storage='globvars.c.obj'if name=='gAction_replay_mode'else'<common>'
        assert owner==expected_storage,(name,'unexpected MAP storage identity',owner)
        section=next(s for s in sections if s[0]<=address and address+n<=s[0]+s[1]);assert section[2]&0xc0000000==0xc0000000 and not section[2]&0x20000020
        mutable[address]=(BASE+offset,n);symbols[original]=address
        global_rows.append(dict(original=original,name=name,address=address,bytes=n,arena_offset=offset,owner=owner))
    array=symbols[0x6768d0];replay_callbacks=symbols[0x6768e8];outlets=symbols[0x79e160]
    public={symbols[a]:name for name,a,n in ROOTS+LOWER if a in symbols}
    extra=((0x4025e0,'ARStartPipingSession'),(0x4028a0,'ARAddDataToSession'),(0x403750,'LengthOfSession'))
    public.update({symbols[a]:name for a,name in extra if a in symbols})
    roots=[symbols[a]for name,a,n in ROOTS];fatal=symbols[0x44c230]
    assert function_aliases.get(fatal)=={('_FatalError','52-errors.c.obj')},'terminal boundary MAP identity'
    def section_for(address,width):
        rows=[s for s in sections if s[0]<=address and address+width<=s[0]+s[1]]
        assert len(rows)==1,(hex(address),width,'section span')
        return rows[0]
    assert section_for(fatal,5)[2]&0x20000020==0x20000020,'terminal replacement requires complete executable code prefix'
    todo=list(roots);seen=set();graph=[];edges=[];patches={};callbacks=[];array_minus4=[];scalar_immediates=[];outlet_endpoint_compares=[]
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
            expected_owner='69-sound.c.obj'if public[start]=='GetIndexFromOutlet'else'23-piping.c.obj'
            decorated={'PipeSingleSound':'@PipeSingleSound@24','GetIndexFromOutlet':'@GetIndexFromOutlet@4','ARDoSingleVariedSession':'_ARDoSingleVariedSession','EndPipingSession2':'@EndPipingSession2@4','ARStartPipingSession':'@ARStartPipingSession@4','ARAddDataToSession':'@ARAddDataToSession@16','LengthOfSession':'@LengthOfSession@4'}
            if public[start]=='__chkstk':assert owner=='LIBC:chkstk.obj' and plain in('chkstk','_chkstk')
            else:assert name==decorated[public[start]]and owner==expected_owner,(name,owner,public[start])
        else:
            assert (owner=='23-piping.c.obj'and name=='_ReplayCopyForward'or owner=='LIBC:chkstk.obj'and plain in('chkstk','_chkstk')),(name,owner)
        end=min(at for at in functions if at>start)
        decoded={};occupied={};successors={};pending=[start];indirect=[]
        while pending:
            address=pending.pop()
            while address not in decoded:
                assert start<=address<end,(name,hex(address))
                ins=next(cs.disasm(raw(data,address,15),address));assert ins.size and address+ins.size<=end
                assert section_for(address,ins.size)[2]&0x20000020==0x20000020,'instruction range must be executable code'
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
                    if op.type==CS_OP_IMM and ins.mnemonic=='cmp'and public.get(start)=='GetIndexFromOutlet'and value==outlets+28:
                        target=BASE+0x1601c
                        outlet_endpoint_compares.append(dict(owner=start,site=address,bytes=ins.bytes.hex(),endpoint=value,arena_endpoint=target,authority='Complete canonical seven-DWORD getter one-past storage bound'))
                    elif op.type==CS_OP_MEM and value==array-4 and op.mem.index:
                        assert op.mem.scale==4 and op.size==4
                        target=BASE+0xf12c;array_minus4.append(dict(owner=start,site=address,address=value,index=cs.reg_name(op.mem.index),scale=4,arena_address=target))
                    else:
                        possible=[(a,d,n)for a,(d,n)in mutable.items()if a<=value<a+n]
                        assert len(possible)==1,('unauthenticated absolute data/alias',name,ins.op_str,hex(value))
                        a,d,n=possible[0];width=op.size if op.type==CS_OP_MEM else 1
                        assert value+width<=a+n and section_for(value,width)[2]&0xc0000000==0xc0000000 and not section_for(value,width)[2]&0x20000020,'operand exceeds authenticated readable writable data span'
                        target=d+value-a
                    patch(at,value,target,True)
                if CS_GRP_RET in ins.groups or ins.mnemonic in('jmp','int','int3'):break
                address+=ins.size
        assert all(target in decoded for targets in successors.values()for target in targets),'noninstruction successor'
        # Must-dataflow authenticates every indirect callback against the actual
        # formal Physics callback base or fresh canonical replay callback table.
        # Partial family writes and volatile caller clobbers kill provenance.
        inputs={start:{}};work=[start]
        def memory_value(op,state,at):
            if op.size!=4 or op.mem.segment:return None
            if not op.mem.base and not op.mem.index and op.mem.disp==replay_callbacks:return ('table','replay',0)
            b=state.get(families.get(cs.reg_name(op.mem.base)));i=state.get(families.get(cs.reg_name(op.mem.index)))if op.mem.index else None
            if b and b[0]=='table':
                offset=b[2]+op.mem.disp
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
    assert bool(rr)==bool(rz),'partial relocation directory'
    if not rz:assert struct.unpack_from('<H',data,nt+22)[0]&1,'absent relocation directory must be explicitly stripped'
    relocation_sites=set()
    while cursor<base+rr+rz:
        page,n=struct.unpack('<II',raw(data,cursor,8));assert n>=8 and n%2==0 and cursor+n<=base+rr+rz,'relocation block extent'
        for word in struct.unpack('<'+'H'*((n-8)//2),raw(data,cursor+8,n-8)):
            kind=word>>12;assert kind in(0,3),'unexpected x86 relocation type'
            if kind==3:
                at=page+(word&4095);assert at+4<=size and at not in relocation_sites,'relocation span/duplicate'
                relocation_sites.add(at)
                if base+at in active:assert at in patches;checked.append(at)
        cursor+=n
    assert len(callbacks)==2 and all(c['kind']=='calc_length'for c in callbacks)
    # A separately seeded direct getter test cannot supply a missing root
    # dependency. The complete root graph itself must reach both genuine
    # published getter and varied writer entries (reject new inlining forms
    # until their complete compiler-specific provenance is independently proved).
    root_reached=set();root_todo=[roots[0]]
    while root_todo:
        owner=root_todo.pop()
        if owner in root_reached:continue
        root_reached.add(owner)
        root_todo.extend(edge['target']for edge in edges if edge['owner']==owner and edge['target']!=fatal and edge['target']!=owner)
    assert roots[1]in root_reached and symbols[0x402bd0]in root_reached,'complete sound root lacks authentic getter/writer closure'
    blob=words([a-base for a in roots]+[len(patches),fatal-base])+b''.join(words(v)for _,v in sorted(patches.items()))
    (out/'matching-addresses.bin').write_bytes(blob)
    return dict(recompiled_sha256=sha(data),symbols_sha256=sha(symbol_path.read_bytes()),map_sha256=sha(map_bytes),graph=graph,edges=edges,callbacks=callbacks,patches=list(patches.values()),relocations_checked=checked,mutable_globals=global_rows,array_minus4=array_minus4,packet_sha256=sha(blob),terminal_fatal=fatal,tables=[],crt_probe_aliases=probe_aliases,scalar_immediates=scalar_immediates,outlet_endpoint_compares=outlet_endpoint_compares,sound_root_reachable_members=sorted(root_reached),terminal_fatal_identity=('_FatalError','52-errors.c.obj'))

if __name__=='__main__':main()
