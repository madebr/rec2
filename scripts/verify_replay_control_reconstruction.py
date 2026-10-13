#!/usr/bin/env python3
"""Compare replay state controls with the physical original and three compilers.

The reference driver executes only the audited fifteen entry bodies and _ftol.
Absolute data operands are relocated; relative instructions remain unchanged.
All referenced mutable globals and raw public returns are compared. Invalid
pointer bit patterns are only compared/copied, never dereferenced. Floating
exception flags are not part of this fixture; PC53 and a balanced x87 stack are.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import random
import struct
import subprocess

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / 'src/carma2/newcommon/23-piping.c'
HASH = '9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
ENTRIES = [
    ('ARReplayForwards', 0x402300, 93),
    ('ARReplayIsReallyPaused', 0x402360, 28),
    ('ARReplayPauseState', 0x402380, 6),
    ('ARGetReplayRate', 0x402390, 7),
    ('ARSetReplayRate', 0x4023a0, 12),
    ('ARGetReplayDirection', 0x4023b0, 6),
    ('ARSetReplayDirection', 0x4023c0, 17),
    ('ARResetPiping', 0x4023e0, 35),
    ('ARIsActionReplayAvailable', 0x4025a0, 14),
    ('ARSomeReplayLeft', 0x4025b0, 44),
    ('ARGetPipePlayPtr', 0x402df0, 6),
    ('ARSetPipePlayPtr', 0x402e00, 7),
    ('ARDisableAdvance', 0x4030d0, 11),
    ('AREnableAdvance', 0x4030e0, 11),
    ('ARService', 0x403d30, 1),
]
GLOBALS = [
    ('gPipe_record_ptr', 0x6768bc, 'tU8*'),
    ('gPipe_play_ptr', 0x6768b8, 'tU8*'),
    ('gPipe_buffer_oldest', 0x6768ac, 'tU8*'),
    ('gPipe_buffer_start', 0x676908, 'tU8*'),
    ('gPipe_buffer_phys_end', 0x6768f0, 'tU8*'),
    ('gPipe_buffer_working_end', 0x6768c4, 'tU8*'),
    ('gReentrancy_count', 0x6768e4, 'int'),
    ('gPlay_direction', 0x6768c0, 'int'),
    ('gReplay_pause_state', 0x6768fc, 'int'),
    ('gReplay_rate', 0x676900, 'float'),
    ('gDisable_advance', 0x67690c, 'int'),
]
DEPENDENCIES = [('_ftol', 0x575b80, 39)]


def sha(data):
    return hashlib.sha256(data).hexdigest()


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--original', type=Path, default=ROOT/'build-reference/originals/CARMA2_HW.EXE')
    p.add_argument('--output', type=Path, default=ROOT/'build-audit/replay-control-tests')
    p.add_argument('--symbols', type=Path)
    p.add_argument('--recompiled', type=Path)
    args = p.parse_args()
    if bool(args.symbols) != bool(args.recompiled):
        p.error('--symbols and --recompiled are required together')
    from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_OP_IMM, CS_OP_MEM
    from verify_tinted_native_reconstruction import extract
    original = args.original.read_bytes()
    assert sha(original) == HASH
    out = args.output.resolve()
    out.mkdir(parents=True, exist_ok=True)

    def raw(data, address, length):
        nt = struct.unpack_from('<I', data, 60)[0]
        base = struct.unpack_from('<I', data, nt+52)[0]
        tab = nt+24+struct.unpack_from('<H', data, nt+20)[0]
        for i in range(struct.unpack_from('<H', data, nt+6)[0]):
            _, va, size, off = struct.unpack_from('<IIII', data, tab+40*i+8)
            if va <= address-base and address-base+length <= va+size:
                return data[off+address-base-va:off+address-base-va+length]
        raise ValueError(hex(address))

    cs = Cs(CS_ARCH_X86, CS_MODE_32)
    cs.detail = True

    def address_file(path, data, addresses, globals_, spans, allowed_data):
        patches = {}
        allowed_code = {a for a, _ in spans}
        closure = []
        for address, length in spans:
            code = raw(data, address, length)
            closure.append(dict(address=address, length=length, sha256=sha(code)))
            decoded = list(cs.disasm(code, address))
            assert sum(i.size for i in decoded) == length
            for ins in decoded:
                if ins.mnemonic == 'call' or ins.mnemonic.startswith('j'):
                    assert ins.operands[0].type == CS_OP_IMM
                    target = ins.operands[0].imm
                    assert target in allowed_code or (ins.mnemonic.startswith('j') and address <= target < address+length), (hex(ins.address), ins.op_str)
                    continue
                for op in ins.operands:
                    if op.type == CS_OP_MEM and not op.mem.base and not op.mem.index:
                        assert op.mem.disp in allowed_data, (hex(ins.address), ins.op_str)
                        assert ins.disp_size == 4
                        patches[ins.address+ins.disp_offset-0x400000] = op.mem.disp
        patches = sorted(patches.items())
        path.write_bytes(struct.pack('<27I', *addresses, *globals_, len(patches)) + b''.join(struct.pack('<2I', *x) for x in patches))
        return dict(closure=closure, absolute_data_relocations=patches)

    common = r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "23-piping.h"
static unsigned addresses[15];
static void* state[11];
#ifdef REPLAY_PHYSICAL
extern unsigned __cdecl InvokeChecked(unsigned,unsigned,unsigned,unsigned*);
#else
typedef int (C2_HOOK_FASTCALL *int_fn)(void);
typedef float (C2_HOOK_FASTCALL *float_fn)(void);
/* Float fastcall parameters are stack words with RET4; native cdecl uses
 * the same raw word and caller cleanup. Avoid a harness FLD/FSTP before entry. */
typedef void (C2_HOOK_STDCALL *set_float_bits_fn)(unsigned);
typedef void (C2_HOOK_FASTCALL *set_ptr_fn)(tU8*);
typedef tU8* (C2_HOOK_FASTCALL *ptr_fn)(void);
typedef void (C2_HOOK_FASTCALL *void_fn)(void);
#endif
static int run(FILE* input) {
    unsigned h[13],result[3],i,word;
#ifndef REPLAY_PHYSICAL
    float scalar;
    double wide;
#endif
    _setmode(_fileno(stdout),_O_BINARY);
    while(fread(h,sizeof(h),1,input)==1) {
        if(h[0]>=15)return 20;
        for(i=0;i<11;i++)memcpy(state[i],h+2+i,4);
        memset(result,0,sizeof(result));
        _controlfp(_PC_53,_MCW_PC);
#ifdef REPLAY_PHYSICAL
        if(InvokeChecked(addresses[h[0]],h[0],h[1],result))return 21;
#else
        switch(h[0]) {
        case 3:
            wide=((float_fn)(size_t)addresses[h[0]])();
            scalar=(float)wide;memcpy(result,&scalar,4);memcpy(result+1,&wide,8);break;
        case 4:case 6:
            ((set_float_bits_fn)(size_t)addresses[h[0]])(h[1]);break;
        case 11:((set_ptr_fn)(size_t)addresses[h[0]])((tU8*)(size_t)h[1]);break;
        case 7:case 12:case 13:case 14:((void_fn)(size_t)addresses[h[0]])();break;
        case 10:result[0]=(unsigned)(size_t)((ptr_fn)(size_t)addresses[h[0]])();break;
        default:result[0]=((int_fn)(size_t)addresses[h[0]])();break;
        }
#endif
        fwrite(result,sizeof(result),1,stdout);
        for(i=0;i<11;i++){memcpy(&word,state[i],4);fwrite(&word,4,1,stdout);}
    }
    return ferror(input) ? 22 : 0;
}
'''
    native = common + '\n' + '\n'.join(f'{typ} {name};' for name, _, typ in GLOBALS)
    native += '\n' + '\n'.join(extract(SOURCE.read_text(), name) for name, _, _ in ENTRIES)
    native += '\nint C2_HOOK_CDECL main(int argc,char**argv){FILE*f;int result;if(argc!=2)return 2;\n'
    native += '\n'.join(f'addresses[{i}]=(unsigned)(size_t){name};' for i, (name, _, _) in enumerate(ENTRIES))
    native += '\n' + '\n'.join(f'state[{i}]=&{name};' for i, (name, _, _) in enumerate(GLOBALS))
    native += '\nf=fopen(argv[1],"rb");if(!f)return 3;result=run(f);fclose(f);return result;}\n'
    (out/'native.c').write_text(native)
    physical = '#define REPLAY_PHYSICAL\n' + common + r'''
static unsigned saved_stack,saved_mode,saved_output,saved_top,saved_return;
unsigned __declspec(naked) __cdecl InvokeChecked(unsigned address,unsigned mode,unsigned arg,unsigned* output) {
    __asm {
        push ebp
        push ebx
        push esi
        push edi
        mov saved_stack,esp
        mov eax,[esp+24]
        mov saved_mode,eax
        mov eax,[esp+32]
        mov saved_output,eax
        fnstsw ax
        and eax,3800h
        mov saved_top,eax
        mov ebp,13579bdfh
        mov ebx,2468ace0h
        mov esi,11223344h
        mov edi,55667788h
        mov ecx,[esp+28]
        mov edx,0a5a55a5ah
        mov eax,[esp+20]
        cmp saved_mode,4
        je stack_arg
        cmp saved_mode,6
        jne invoke
    stack_arg:
        push ecx
    invoke:
        call eax
        mov saved_return,eax
        mov ecx,esp
        mov esp,saved_stack
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
        mov ecx,saved_output
        mov eax,saved_mode
        cmp eax,3
        jne integer_result
        fld st(0)
        fstp dword ptr [ecx]
        fstp qword ptr [ecx+4]
        jmp check_top
    integer_result:
        cmp eax,4
        je check_top
        cmp eax,6
        je check_top
        cmp eax,7
        je check_top
        cmp eax,11
        jae check_top
        mov eax,saved_return
        mov [ecx],eax
    check_top:
        fnstsw ax
        and eax,3800h
        cmp eax,saved_top
        jne failed
        xor eax,eax
        jmp done
    failed:
        mov eax,1
    done:
        pop edi
        pop esi
        pop ebx
        pop ebp
        ret
    }
}
int main(int argc,char**argv) {
    FILE*f;unsigned char*disk,*image;unsigned size,i,globals_[11],count,*patches,value,delta;int result;
    IMAGE_NT_HEADERS32*nt;IMAGE_SECTION_HEADER*sections;
    if(argc!=4)return 2;
    f=fopen(argv[3],"rb");if(!f)return 3;
    if(fread(addresses,sizeof(addresses),1,f)!=1||fread(globals_,sizeof(globals_),1,f)!=1||fread(&count,4,1,f)!=1)return 4;
    patches=(unsigned*)malloc(count*8);if(!patches||fread(patches,8,count,f)!=count)return 4;fclose(f);
    f=fopen(argv[2],"rb");if(!f)return 5;fseek(f,0,SEEK_END);size=(unsigned)ftell(f);rewind(f);
    disk=(unsigned char*)malloc(size);if(!disk||fread(disk,size,1,f)!=1)return 6;fclose(f);
    nt=(IMAGE_NT_HEADERS32*)(disk+((IMAGE_DOS_HEADER*)disk)->e_lfanew);
    image=(unsigned char*)VirtualAlloc(NULL,nt->OptionalHeader.SizeOfImage,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);
    if(!image)return 7;
    sections=IMAGE_FIRST_SECTION(nt);
    for(i=0;i<nt->FileHeader.NumberOfSections;i++)memcpy(image+sections[i].VirtualAddress,disk+sections[i].PointerToRawData,sections[i].SizeOfRawData);
    delta=(unsigned)(ULONG_PTR)image-nt->OptionalHeader.ImageBase;
    for(i=0;i<count;i++) {
        if(patches[2*i]+4>nt->OptionalHeader.SizeOfImage)return 9;
        memcpy(&value,image+patches[2*i],4);if(value!=patches[2*i+1])return 10;
        value+=delta;memcpy(image+patches[2*i],&value,4);
    }
    for(i=0;i<15;i++)addresses[i]+=delta;
    for(i=0;i<11;i++)state[i]=(void*)(ULONG_PTR)(globals_[i]+delta);
    f=fopen(argv[1],"rb");if(!f)return 8;result=run(f);fclose(f);
    VirtualFree(image,0,MEM_RELEASE);free(disk);free(patches);return result;
}
'''
    (out/'physical.c').write_text(physical)
    dirs = [ROOT/x for x in ('src/carma2/newcommon','src/brender/include','src/carma2/hooks','src/carma2/include','src/carma2/s3/include')]
    mroot = ROOT/'build-tools/msvc-modern'
    msvc = sorted((mroot/'VC/Tools/MSVC').iterdir())[-1]
    sdk = mroot/'Windows Kits/10'
    version = sorted((sdk/'Include').iterdir())[-1].name
    env = dict(os.environ)
    env['PATH'] = str(msvc/'bin/Hostx64/x86')+os.pathsep+env['PATH']
    env['INCLUDE'] = os.pathsep.join(map(str,[msvc/'include',*[sdk/f'Include/{version}/{part}' for part in ('ucrt','shared','um')]]))
    env['LIB'] = os.pathsep.join(map(str,[msvc/'lib/x86',*[sdk/f'Lib/{version}/{part}/x86' for part in ('ucrt','um')]]))
    m5 = ROOT/'build-tools/msvc500sp3'
    env5 = dict(os.environ)
    env5['PATH'] = str(m5/'bin')+os.pathsep+str(m5/'redist')+os.pathsep+env5['PATH']
    env5['INCLUDE'] = str(m5/'include')
    env5['LIB'] = str(m5/'lib')
    gcc = ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'
    commands = {
        'msvc':[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(d) for d in dirs],str(out/'native.c'),'/Fo'+str(out/'msvc.obj'),'/Fe'+str(out/'msvc.exe')],
        'msvc5':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(d) for d in dirs],str(out/'native.c'),'/Fo'+str(out/'msvc5.obj'),'/Fe'+str(out/'msvc5.exe')],
        'mingw':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror',*['-I'+str(d) for d in dirs],str(out/'native.c'),'-o',str(out/'mingw.exe')],
        'physical':[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(d) for d in dirs],str(out/'physical.c'),'/Fo'+str(out/'physical.obj'),'/Fe'+str(out/'physical.exe')],
    }
    for compiler, command in commands.items():
        result = subprocess.run(command,cwd=out,env=env if compiler in ('msvc','physical') else env5 if compiler=='msvc5' else None,capture_output=True,text=True)
        (out/(compiler+'-build.txt')).write_text(result.stdout+result.stderr)
        if result.returncode:
            raise RuntimeError(result.stdout+result.stderr)
    rng = random.Random(0x402300)
    patterns = [0,0x80000000,1,0x80000001,0x007fffff,0x807fffff,0x00800000,0x80800000,0x3f800000,0xbf800000,0x3effffff,0xbeffffff,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc00001,0xffc12345,0x7f800001,0xff812345]
    for bits in (0x3f800000,0x40000000,0x4f000000,0x4f800000,0x5f000000):
        for nearby in range(-3,4):
            patterns.extend((bits+nearby,(bits+nearby)^0x80000000))
    cases = []
    for mode in range(15):
        for bits in patterns:
            for direction in (0,1,2,0xffffffff,0xfffffffe,0x7fffffff,0x80000000):
                for relation in range(4 if mode in (0,9) else 1):
                    state = [rng.getrandbits(32) for _ in GLOBALS]
                    state[7],state[9] = direction,bits
                    if relation==1:state[1]=state[0]
                    if relation==2:state[1]=state[2]
                    if relation==3:state[0]=state[1]=state[2]
                    if mode==8:state[3]=0 if direction&1 else state[3]
                    cases.append((mode,bits,*state))
    for mode in (0,1,3,4,6):
        for _ in range(2048):
            state = [rng.getrandbits(32) for _ in GLOBALS]
            bits = rng.getrandbits(32)
            state[9] = bits
            cases.append((mode,bits,*state))
    packed = b''.join(struct.pack('<13I', *x) for x in cases)
    (out/'fixtures.bin').write_bytes(packed)
    original_meta = address_file(out/'original-addresses.bin',original,[a for _,a,_ in ENTRIES],[a for _,a,_ in GLOBALS],[(a,n) for _,a,n in ENTRIES+DEPENDENCIES],{a for _,a,_ in GLOBALS}|{0x589330})
    assert raw(original,0x589330,4)==b'\0'*4
    checks = []
    with (out/'original.bin').open('wb') as output:
        subprocess.run([str(out/'physical.exe'),str(out/'fixtures.bin'),str(args.original.resolve()),str(out/'original-addresses.bin')],stdout=output,check=True)
    expected = (out/'original.bin').read_bytes()
    assert len(expected)==56*len(cases)

    def check(label, command):
        with (out/(label+'.bin')).open('wb') as output:
            subprocess.run(command,stdout=output,check=True)
        actual = (out/(label+'.bin')).read_bytes()
        if actual != expected:
            at = next((i for i,(a,b) in enumerate(zip(actual,expected)) if a!=b),min(len(actual),len(expected)))
            index = at//56
            failure = dict(compiler=label,fixture=index,inputs=cases[index],offset=at%56,expected=expected[index*56:(index+1)*56].hex(),actual=actual[index*56:(index+1)*56].hex())
            (out/(label+'-failure.json')).write_text(json.dumps(failure,indent=2)+'\n')
            raise AssertionError(str(failure))
        checks.append(dict(compiler=label,comparisons=len(cases),sha256=sha(actual)))
    for compiler in ('msvc','msvc5','mingw'):
        check(compiler,[str(out/(compiler+'.exe')),str(out/'fixtures.bin')])
    linked = None
    if args.symbols:
        exported = json.loads(args.symbols.read_text())
        rebuilt = args.recompiled.read_bytes()
        assert exported['original_sha256']==HASH and exported['recompiled_sha256']==sha(rebuilt)
        symbols = {s['orig']:s['recomp'] for s in exported['symbols']}
        addresses = [symbols[a] for _,a,_ in ENTRIES]
        spans = []
        for address in sorted(set(addresses)|{symbols[0x575b80]}):
            end = min(s['recomp'] for s in exported['symbols'] if s['recomp']>address)
            spans.append((address,end-address))
        # Constant-pool zero may be folded/merged under a different original
        # constant label. Resolve absolute non-global reads and prove zero bytes.
        globals_ = [symbols[a] for _,a,_ in GLOBALS]
        allowed_data = set(globals_)
        for address,length in spans:
            for ins in cs.disasm(raw(rebuilt,address,length),address):
                for op in ins.operands:
                    if op.type==CS_OP_MEM and not op.mem.base and not op.mem.index and op.mem.disp not in allowed_data:
                        assert ins.mnemonic in ('fcomp','fld','fcom') and raw(rebuilt,op.mem.disp,op.size)==b'\0'*op.size,(hex(ins.address),ins.op_str)
                        allowed_data.add(op.mem.disp)
        linked = address_file(out/'linked-addresses.bin',rebuilt,addresses,globals_,spans,allowed_data)
        check('matching-physical',[str(out/'physical.exe'),str(out/'fixtures.bin'),str(args.recompiled.resolve()),str(out/'linked-addresses.bin')])
        linked.update(recompiled_sha256=sha(rebuilt),symbols_sha256=sha(args.symbols.read_bytes()))
    report = dict(scenario_count=len(cases),comparisons=sum(x['comparisons'] for x in checks),checks=checks,original_sha256=HASH,source_sha256=sha(SOURCE.read_bytes()),header_sha256=sha(SOURCE.with_suffix('.h').read_bytes()),verifier_sha256=sha(Path(__file__).read_bytes()),fixtures_sha256=sha(packed),original=original_meta,linked=linked,compiler_commands=commands,limitations=__doc__)
    (out/'verification.json').write_text(json.dumps(report,indent=2)+'\n')
    print(f'PASS: {len(cases)} scenarios, {report["comparisons"]} comparisons.')


if __name__ == '__main__':
    main()
