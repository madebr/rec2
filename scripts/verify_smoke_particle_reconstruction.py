#!/usr/bin/env python3
"""Compare smoke particle production bodies against the original x86 executable.

Full particle/fixture state and random/matrix callback arguments are compared
with native x86 MSVC5, modern MSVC and MinGW. Fresh symbols verify rebuilt PE.
Matrix and random callbacks are controlled boundaries, not BRender integration.
An independently executed physical original x87 probe corrects Unicorn's failure
to quiet signaling NaN strength on the original FLD/FCOM/FSTP path. Only that
stored scalar is corrected, only after an emitted puff; all other state is raw.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import random
import struct
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / 'src/carma2/newcommon/32-spark.c'
ORIGINAL_HASH = '9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
ADDRESSES = dict(GenerateContinuousSmoke=0x4fa5b0, CreatePuffOfSmoke=0x4fb330,
    ResetSmoke=0x4fb510, AdjustSmoke=0x4fb520, gSmoke=0x6a8ce0,
    gSmoke_flags=0x6aa590, gSmoke_next=0x6aa594, gSmoke_on=0x660110,
    gPHIL_mechanics_time_sync=0x74a5e4, gCar_to_view=0x655f54,
    gProgram_state=0x75bb80, gCurrent_race=0x7623a0,
    gDust_rotate=0x6aa568, gNum_dust_tables=0x6a82b4,
    IRandomBetween=0x513520, BrMatrix34ApplyP=0x5334a0, BrMatrix34ApplyV=0x533520)
HEAP, STACK, STOP = 0x1000000, 0x1080000, 0x1081000
SIZE = 0x8000


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--original', type=Path, default=ROOT/'build-reference/originals/CARMA2_HW.EXE')
    parser.add_argument('--recompiled', type=Path, default=ROOT/'build-matching/rec2.exe')
    parser.add_argument('--symbols', type=Path)
    parser.add_argument('--dependency-dir', type=Path, default=ROOT/'build-audit/joystick/python-deps')
    parser.add_argument('--gcc', type=Path, default=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe')
    parser.add_argument('--msvc-root', type=Path, default=ROOT/'build-tools/msvc-modern')
    parser.add_argument('--msvc5-root', type=Path, default=ROOT/'build-tools/msvc500sp3')
    parser.add_argument('--output', type=Path, default=ROOT/'build-audit/smoke-particle-tests')
    args = parser.parse_args()
    sys.path.insert(0, str(args.dependency_dir.resolve()))
    from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
    from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EIP, UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX, UC_X86_REG_FPCW
    from verify_tinted_native_reconstruction import extract
    original = args.original.read_bytes()
    assert hashlib.sha256(original).hexdigest() == ORIGINAL_HASH
    images = [(original, ADDRESSES)]
    if args.symbols:
        mapping = json.loads(args.symbols.read_text())
        rebuilt = args.recompiled.read_bytes()
        assert mapping['format'] == 1 and mapping['original_sha256'] == ORIGINAL_HASH
        assert mapping['recompiled_sha256'] == hashlib.sha256(rebuilt).hexdigest()
        symbols = {s['name']: s for s in mapping['symbols']}
        assert all(symbols[n]['orig'] == a for n, a in ADDRESSES.items())
        images.append((rebuilt, {n: symbols[n]['recomp'] for n in ADDRESSES}))
    out = args.output.resolve()
    out.mkdir(parents=True, exist_ok=True)
    physical_strength = {}
    oracle_corrections = []

    def execute(data, addresses, header, arena):
        u = Uc(UC_ARCH_X86, UC_MODE_32)
        u.mem_map(0x400000, 0x500000)
        u.mem_map(HEAP, 0x100000)
        pe = struct.unpack_from('<I', data, 0x3c)[0]
        table = pe+24+struct.unpack_from('<H', data, pe+20)[0]
        for i in range(struct.unpack_from('<H', data, pe+6)[0]):
            _, rva, size, raw = struct.unpack_from('<IIII', data, table+40*i+8)
            u.mem_write(0x400000+rva, data[raw:raw+size])
        def wr(p, v): u.mem_write(p, struct.pack('<I', v & 0xffffffff))
        def rd(p): return struct.unpack('<I', u.mem_read(p, 4))[0]
        u.mem_write(HEAP, bytes(arena))
        u.mem_write(addresses['gSmoke'], bytes(arena[:1100]))
        wr(HEAP+0x1008, HEAP+0x3000)
        wr(HEAP+0x1010, HEAP+0x4000)
        for name, i in [('gSmoke_flags',6), ('gSmoke_next',7), ('gSmoke_on',8), ('gPHIL_mechanics_time_sync',9), ('gNum_dust_tables',11), ('gDust_rotate',12)]:
            wr(addresses[name], header[i])
        current = addresses['gProgram_state']+0xac
        u.mem_write(current+0x4c, bytes(arena[0x7000:0x700c]))
        wr(addresses['gCar_to_view'], current if header[13] == 0 else HEAP+0x6000)
        for i in range(11): wr(addresses['gCurrent_race']+0xf28+40*i, 2+i)
        calls = []
        def snapshot():
            copy = bytearray(u.mem_read(HEAP, SIZE))
            for off in [0x1008,0x1010]:
                struct.pack_into('<I', copy, off, rd(HEAP+off)-HEAP)
            return (struct.pack('<2I', rd(addresses['gSmoke_flags']), rd(addresses['gSmoke_next'])) + bytes(u.mem_read(addresses['gSmoke'],1100)) + bytes(copy)).hex()
        def hook(uc, at, size, context):
            if at == addresses['IRandomBetween']:
                calls.append(['I', uc.reg_read(UC_X86_REG_ECX), uc.reg_read(UC_X86_REG_EDX), snapshot()])
                if header[8] & 2: wr(HEAP+0x2284+4*header[1],50)
                result = header[10]
            elif at in (addresses['BrMatrix34ApplyP'], addresses['BrMatrix34ApplyV']):
                sp = uc.reg_read(UC_X86_REG_ESP)
                dest, src, mat = [rd(sp+4+i*4) for i in range(3)]
                mode = 0 if at == addresses['BrMatrix34ApplyP'] else 1
                calls.append(['P' if mode == 0 else 'V', bytes(u.mem_read(src,12)).hex(), bytes(u.mem_read(mat,48)).hex(), snapshot()])
                u.mem_write(dest, bytes(u.mem_read(HEAP+0x5020+mode*0x10,12)))
                if mode == 0 and header[8] & 4:
                    wr(HEAP+0x1010,HEAP+0x4100)
                    wr(HEAP+0x2274+4*header[1],8)
                if mode == 1 and header[8] & 8: wr(addresses['gSmoke_on'],0)
                result = dest
            else: return
            sp = uc.reg_read(UC_X86_REG_ESP)
            uc.reg_write(UC_X86_REG_EAX, result)
            uc.reg_write(UC_X86_REG_EIP, rd(sp))
            uc.reg_write(UC_X86_REG_ESP, sp+4)
        u.hook_add(UC_HOOK_CODE, hook)
        u.reg_write(UC_X86_REG_FPCW, 0x27f)
        def pointer(off): return addresses['gSmoke']+off if off < 1100 else HEAP+off
        wr(STACK, STOP)
        mode = header[0]
        if mode == 0:
            name='CreatePuffOfSmoke';ecx=pointer(header[14]);edx=pointer(header[15]);stackargs=header[4:6]+[header[3]]
        elif mode == 1:
            name='GenerateContinuousSmoke';ecx=HEAP+0x1000;edx=header[1];stackargs=[header[2]]
        elif mode == 2:
            name='ResetSmoke';ecx=0;edx=0;stackargs=[]
        else:
            name='AdjustSmoke';ecx=header[7];edx=header[3];stackargs=[pointer(header[14]),header[4],header[5]]
        for i,v in enumerate(stackargs): wr(STACK+4+4*i,v)
        u.reg_write(UC_X86_REG_ESP, STACK)
        u.reg_write(UC_X86_REG_ECX, ecx)
        u.reg_write(UC_X86_REG_EDX, edx)
        u.emu_start(addresses[name], STOP, count=100000)
        assert u.reg_read(UC_X86_REG_EIP) == STOP
        if addresses is ADDRESSES and mode == 0 and rd(addresses['gSmoke_next']) != header[7]:
            strength = header[4]
            if strength & 0x7fc00000 == 0x7f800000 and strength & 0x003fffff:
                stored = addresses['gSmoke']+44*header[7]+32
                observed = rd(stored)
                corrected = physical_strength[strength]
                assert observed in (strength, corrected)
                wr(stored, corrected)
                oracle_corrections.append(dict(input=strength,unicorn=observed,physical_original=corrected))
        return dict(calls=calls,state=snapshot())

    source = SOURCE.read_text()
    names = ['SmokeScalarRound','GenerateContinuousSmoke','CreatePuffOfSmoke','ResetSmoke','AdjustSmoke']
    bodies = [extract(source,n) for n in names]
    driver = r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <math.h>
#include <float.h>
#include "32-spark.h"
#include "41-utility.h"
#include "49-physics.h"
#include "globvars.h"
#include "globvrkm.h"
#include "rec2_macros.h"
typedef char layouts[sizeof(tSmoke)==44&&offsetof(tCar_spec,pos)==0x4c&&offsetof(tCar_spec,wpos)==0x1224&&offsetof(tCar_spec,dust_time)==0x1284&&offsetof(tPhysics_object,omega)==0x74&&offsetof(tPhysics_object,velocity_car_space)==0x1a8&&offsetof(tPhysics_object,bb2)==0x3c&&offsetof(tRace_info,material_modifiers)==0xf08&&offsetof(tProgram_state,current_car)==0xac?1:-1];
tSmoke gSmoke[25];int gSmoke_flags,gSmoke_next,gSmoke_on,gPHIL_mechanics_time_sync,gNum_dust_tables,gDust_rotate;
tProgram_state gProgram_state;tRace_info gCurrent_race;tCar_spec* gCar_to_view;
static unsigned arena[0x8000/4],header[16];static int events;
static unsigned rd(unsigned off){unsigned v;memcpy(&v,(char*)arena+off,4);return v;}
static void wr(unsigned off,unsigned v){memcpy((char*)arena+off,&v,4);}
static void hex(const void*p,unsigned n){unsigned i;for(i=0;i<n;i++)printf("%02x",((const unsigned char*)p)[i]);}
static void snapshot(void){unsigned char copy[sizeof(arena)];unsigned globals[2];unsigned v;
    memcpy(copy,arena,sizeof(copy));v=rd(0x1008)-(unsigned)(uintptr_t)arena;memcpy(copy+0x1008,&v,4);v=rd(0x1010)-(unsigned)(uintptr_t)arena;memcpy(copy+0x1010,&v,4);
    globals[0]=gSmoke_flags;globals[1]=gSmoke_next;hex(globals,8);hex(gSmoke,sizeof(gSmoke));hex(copy,sizeof(copy));
}
int C2_HOOK_FASTCALL IRandomBetween(int a,int b){printf("%s[\"I\",%u,%u,\"",events++?",":"",(unsigned)a,(unsigned)b);snapshot();printf("\"]");if(header[8]&2)wr(0x2284+4*header[1],50);return header[10];}
static br_vector3* matrix(br_vector3*dest,const br_vector3*src,const br_matrix34*mat,int mode){printf("%s[\"%s\",\"",events++?",":"",mode?"V":"P");hex(src,12);printf("\",\"");hex(mat,48);printf("\",\"");snapshot();printf("\"]");memcpy(dest,(char*)arena+0x5020+mode*0x10,12);if(mode==0&&(header[8]&4)){wr(0x1010,(unsigned)(uintptr_t)((char*)arena+0x4100));wr(0x2274+4*header[1],8);}if(mode==1&&(header[8]&8))gSmoke_on=0;return dest;}
void C2_HOOK_CDECL BrMatrix34ApplyP(br_vector3*dest,const br_vector3*src,const br_matrix34*mat){matrix(dest,src,mat,0);}
void C2_HOOK_CDECL BrMatrix34ApplyV(br_vector3*dest,const br_vector3*src,const br_matrix34*mat){matrix(dest,src,mat,1);}
'''
    driver += '\n\n'.join(bodies)
    driver += r'''
/* Push raw scalar bits so the caller cannot quiet a signaling NaN. */
static void puff_raw(br_vector3*pos,br_vector3*v,unsigned strength,unsigned decay,unsigned type_bits){
#ifdef _MSC_VER
    __asm {
        mov ecx,pos
        mov edx,v
        mov eax,strength
        mov ebx,decay
        mov esi,type_bits
        push esi
        push ebx
        push eax
#ifndef REC2_MATCHING
        push edx
        push ecx
#endif
        call CreatePuffOfSmoke
#ifndef REC2_MATCHING
        add esp,20
#endif
    }
#else
    unsigned args[3];unsigned*p=args;void*fn=(void*)(uintptr_t)CreatePuffOfSmoke;
    args[0]=strength;args[1]=decay;args[2]=type_bits;
    __asm__ volatile("pushl 8(%0)\n\tpushl 4(%0)\n\tpushl 0(%0)\n\tpushl %%edx\n\tpushl %%ecx\n\tcall *%3\n\taddl $20,%%esp"
        : "+a"(p),"+c"(pos),"+d"(v) : "r"(fn) : "memory","cc");
#endif
}
static br_vector3* pointer(unsigned off){return (br_vector3*)(off<1100?(char*)gSmoke+off:(char*)arena+off);}
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned i;_controlfp(_PC_53,_MCW_PC);if(argc!=2)return 1;f=fopen(argv[1],"rb");if(!f)return 2;if(fread(header,sizeof(header),1,f)!=1||fread(arena,sizeof(arena),1,f)!=1)return 3;fclose(f);
    memcpy(gSmoke,arena,sizeof(gSmoke));wr(0x1008,(unsigned)(uintptr_t)((char*)arena+0x3000));wr(0x1010,(unsigned)(uintptr_t)((char*)arena+0x4000));
    gSmoke_flags=header[6];gSmoke_next=header[7];gSmoke_on=header[8];gPHIL_mechanics_time_sync=header[9];gNum_dust_tables=header[11];gDust_rotate=header[12];
    memcpy(&gProgram_state.current_car.pos,(char*)arena+0x7000,12);gCar_to_view=header[13]?(tCar_spec*)((char*)arena+0x6000):&gProgram_state.current_car;
    for(i=0;i<11;i++)gCurrent_race.material_modifiers[i].smoke_type=2+i;
    printf("{\"calls\":[");
    if(header[0]==0)puff_raw(pointer(header[14]),pointer(header[15]),header[4],header[5],header[3]);
    else if(header[0]==1)GenerateContinuousSmoke((tCar_spec*)((char*)arena+0x1000),header[1],header[2]);
    else if(header[0]==2)ResetSmoke();
    else AdjustSmoke(header[7],(tU8)header[3],pointer(header[14]),*(float*)&header[4],*(float*)&header[5]);
    printf("],\"state\":\"");snapshot();printf("\"}\n");return 0;
}
'''
    harness=out/'harness.c';harness.write_text(driver)
    includes=[ROOT/p for p in ['src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/carma2/platform','src/carma2/s3/include','src/brender/include']]
    msvc_root=args.msvc_root.resolve();msvc=sorted((msvc_root/'VC/Tools/MSVC').iterdir())[-1];sdk=msvc_root/'Windows Kits/10';sdk_version=sorted(p.name for p in (sdk/'Include').iterdir())[-1]
    env=dict(os.environ);env['PATH']=str(msvc/'bin/Hostx64/x86')+os.pathsep+env['PATH']
    env['INCLUDE']=os.pathsep.join(map(str,[msvc/'include',*[sdk/f'Include/{sdk_version}/{p}' for p in ['ucrt','shared','um']]]))
    env['LIB']=os.pathsep.join(map(str,[msvc/'lib/x86',*[sdk/f'Lib/{sdk_version}/{p}/x86' for p in ['ucrt','um']]]))
    commands={
        'msvc':[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(p) for p in includes],str(harness),'/Fo'+str(out/'msvc.obj'),'/Fe'+str(out/'msvc.exe')],
        'msvc5':[str(args.msvc5_root.resolve()/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(p) for p in includes],str(harness),'/Fo'+str(out/'msvc5.obj'),'/Fe'+str(out/'msvc5.exe')],
        'mingw':[str(args.gcc.resolve()),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-fno-strict-aliasing',*['-I'+str(p) for p in includes],str(harness),'-o',str(out/'mingw.exe')]
    }
    msvc5env=dict(os.environ);msvc5=args.msvc5_root.resolve()
    msvc5env['PATH']=str(msvc5/'bin')+os.pathsep+str(msvc5/'redist')+os.pathsep+msvc5env['PATH']
    msvc5env['INCLUDE']=str(msvc5/'include');msvc5env['LIB']=str(msvc5/'lib')
    for compiler,command in commands.items():
        result=subprocess.run(command,cwd=out,env=env if compiler=='msvc' else msvc5env if compiler=='msvc5' else None,text=True,capture_output=True)
        (out/(compiler+'-build.log')).write_text(result.stdout+result.stderr)
        assert result.returncode==0,result.stdout+result.stderr
    probe_source=ROOT/'scripts/verify_smoke_particle_original_x87_probe.c'
    probe_command=[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX',
        '/D_CRT_SECURE_NO_WARNINGS',str(probe_source),'/Fo'+str(out/'original-x87-probe.obj'),
        '/Fe'+str(out/'original-x87-probe.exe'),'/link','kernel32.lib']
    probe_build=subprocess.run(probe_command,cwd=out,env=env,text=True,capture_output=True)
    (out/'original-x87-probe-build.log').write_text(probe_build.stdout+probe_build.stderr)
    assert probe_build.returncode==0,probe_build.stdout+probe_build.stderr
    def bits(v): return struct.unpack('<I',struct.pack('<f',v))[0]
    cases=[]
    for index in [0,1,24]:
        for kind in [0,7,8,15,16,23,255,0x123407]:
            cases.append(dict(mode=0,index=index,kind=kind))
    for strength in [0,0x80000000,1,0xbf800000,0x3f800001,0x7f800000,0xff800000,
            0x7fc12345,0xffc12345,0x7f800001,0x7f801234,0x7fbfffff,0xff800001,0xff801234,0xffbfffff]:
        cases.append(dict(mode=0,strength=strength))
    for distance in [25.,25.00001,24.99999,100.]:
        for viewed in [0,1,2]:cases.append(dict(mode=0,distance=distance,viewed=viewed))
    for px,py,x,y in [(-3.9976463317871094,.08927056938409805,14.812495231628418,16.55677032470703),
            (.22799310088157654,4.447007656097412,17.232192993164062,22.773414611816406),
            (-4.799004077911377,-4.506677627563477,6.767861366271973,-26.66988182067871),
            (4.288102149963379,-3.3251779079437256,24.220964431762695,11.763924598693848),
            (2.3835742473602295,.68522047996521,27.118139266967773,4.318577766418457),
            (-1.0880637168884277,-2.8075273036956787,17.531606674194336,13.87503433227539)]:
        for viewed in [0,2]:cases.append(dict(mode=0,player=[px,py,0.],point=[x,y,0.],viewed=viewed))
    for y in [1.37044095993042,1.5138989686965942,1.7418776750564575,-1.987126350402832,-1.4649614095687866,-1.0889872312545776]:
        cases.append(dict(mode=0,point=[1.,y,2.],v=[3.,y,7.]))
    cases += [dict(mode=0,on=0),dict(mode=0,posoff=4,voff=16),dict(mode=0,posoff=16,voff=4)]
    for index in [0,1,24]:
        cases += [dict(mode=2,index=index),dict(mode=3,index=index),dict(mode=3,index=index,posoff=index*44+28)]
    for wheel in range(4):
        for time in [0,1,100,250,500,0xffffffff]:
            for velocity in [0.,1.,-2.,8.]:cases.append(dict(mode=1,wheel=wheel,time=time,velocity=velocity))
    rng=random.Random(0x32fa5b0)
    for i in range(100):cases.append(dict(mode=1,wheel=i%4,time=rng.randrange(700),velocity=rng.uniform(-20,20),random=i))
    for on in [3,5,7,9,15]:cases.append(dict(mode=1,on=on,wheel=2,time=250,velocity=8.))
    for strength in sorted({case.get('strength',bits(.65)) for case in cases if case['mode']==0}):
        physical_strength[strength]=int(subprocess.check_output([str(out/'original-x87-probe.exe'),
            str(args.original.resolve()),str(strength)],text=True).strip())
        if strength & 0x7fc00000 == 0x7f800000 and strength & 0x003fffff:
            assert physical_strength[strength]==strength|0x00400000
    physical_evidence=dict(original_sha256=ORIGINAL_HASH,
        probe_source_sha256=hashlib.sha256(probe_source.read_bytes()).hexdigest(),
        probe_executable_sha256=hashlib.sha256((out/'original-x87-probe.exe').read_bytes()).hexdigest(),
        compiler_command=probe_command,precision_control=53,
        strength_results=[dict(input=n,output=v) for n,v in sorted(physical_strength.items())])
    (out/'physical-original-x87.json').write_text(json.dumps(physical_evidence,indent=2)+'\n')
    results=[]
    for index,case in enumerate(cases):
        arena=bytearray(SIZE)
        def put(off,v): struct.pack_into('<I',arena,off,v&0xffffffff)
        def vec(off,values):struct.pack_into('<3f',arena,off,*values)
        for i in range(1100//4):put(4*i,0x3f000000+i)
        vec(0x5000,[case.get('distance',3.),-2.,0.])
        if 'distance' in case:vec(0x5000,[case['distance'],0.,0.])
        if 'player' in case:
            vec(0x7000,case['player'])
        if 'point' in case:vec(0x5000,case['point'])
        vec(0x5010,[6.9,-6.9,13.8]);vec(0x5020,[1.25,2.5,-3.75]);vec(0x5030,[-8.25,4.75,9.5])
        if 'v' in case:vec(0x5010,case['v'])
        vec(0x604c,[100.,0.,0.] if case.get('viewed')==1 else [case.get('distance',3.),0.,0.])
        vec(0x303c,[-2.,-3.,-4.]);vec(0x3048,[2.,3.,4.]);vec(0x3074,[.12,-.24,.36])
        velocity=case.get('velocity',8.)
        vec(0x31a8,[velocity,.35,velocity*.7])
        for wheel in range(4):
            vec(0x2224+wheel*12,[.9+wheel*.1,1.1,-1.3]);put(0x2264+wheel*4,bits(.4));put(0x2274+wheel*4,wheel+3);put(0x2284+wheel*4,100)
        for i in range(12):put(0x402c+4*i,bits(.1*i+.25))
        if 'random' in case:
            for off in [0x3074,0x31a8,0x2224+case['wheel']*12]:vec(off,[rng.uniform(-10,10) for j in range(3)])
            put(0x2264+case['wheel']*4,bits(rng.uniform(-2,2)))
        header=[case['mode'],case.get('wheel',0),case.get('time',250),case.get('kind',8),case.get('strength',bits(.65)),bits(.3),0x80402010,case.get('index',0),case.get('on',1),0x12345678,300,5,2,int(case.get('viewed',0)!=0),case.get('posoff',0x5000),case.get('voff',0x5010)]
        fixture=out/'fixture.bin';fixture.write_bytes(struct.pack('<16I',*header)+arena)
        expected=execute(*images[0],header,arena)
        checks=[]
        if len(images)>1:checks.append(('recompiled-x86',execute(*images[1],header,arena)))
        for compiler in commands:checks.append((compiler,json.loads(subprocess.check_output([str(out/(compiler+'.exe')),str(fixture)],text=True))))
        for compiler,actual in checks:
            if actual!=expected:
                failure=out/f'failure-{compiler}-{index}.json';failure.write_text(json.dumps(dict(case=case,original=expected,actual=actual),indent=2))
                raise AssertionError(f'{compiler} particle case {index} differs: {failure}')
            results.append(dict(case=case,compiler=compiler,passed=True,calls=len(actual['calls']),state_sha256=hashlib.sha256(bytes.fromhex(actual['state'])).hexdigest()))
    (out/'verification.json').write_text(json.dumps(dict(original_sha256=ORIGINAL_HASH,source_sha256=hashlib.sha256(SOURCE.read_bytes()).hexdigest(),bodies_sha256=[hashlib.sha256(b.encode()).hexdigest() for b in bodies],compiler_commands=commands,cases=results,physical_original_x87=physical_evidence,scoped_unicorn_corrections=oracle_corrections),indent=2)+'\n')
    print(f'PASS: {len(cases)} scenarios, {len(results)} complete x86/native particle comparisons.')


if __name__=='__main__':main()
