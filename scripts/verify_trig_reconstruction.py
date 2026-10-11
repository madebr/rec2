#!/usr/bin/env python3
"""Compare trigonometry with physical original x87 and three native compilers.

Only the reviewed sine/cosine/axis-rotation closure and original CRT _ftol
execute from the reference PE. Only disassembled absolute data references
are relocated; instructions, internal branches and relative calls stay intact.
No game entry point, imports, TLS or initialization runs. Full table/guard
state and raw return words are compared, including every 16-bit angle.
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
SOURCE = ROOT / 'src/carma2/newcommon/36-trig.c'
HASH = '9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
ENTRIES = [
    ('FastFloatSin', 0x5113d0, 105), ('FastFloatCos', 0x511440, 105),
    ('FastScalarSin', 0x5115b0, 105), ('FastScalarCos', 0x511620, 105),
    ('FastScalarSinAngle', 0x511780, 141), ('FastScalarCosAngle', 0x511810, 141),
    ('DRMatrix34RotateX', 0x512030, 305), ('DRMatrix34RotateY', 0x512170, 305),
    ('DRMatrix34RotateZ', 0x5122b0, 305), ('DRMatrix23Rotate', 0x513020, 287),
]


def sha(data):
    return hashlib.sha256(data).hexdigest()


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--original', type=Path, default=ROOT / 'build-reference/originals/CARMA2_HW.EXE')
    p.add_argument('--output', type=Path, default=ROOT / 'build-audit/trig-tests')
    p.add_argument('--symbols', type=Path)
    p.add_argument('--recompiled', type=Path)
    args = p.parse_args()
    if bool(args.symbols) != bool(args.recompiled):
        p.error('--symbols and --recompiled are required together')
    from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_OP_IMM, CS_OP_MEM
    original = args.original.read_bytes()
    assert sha(original) == HASH
    out = args.output.resolve()
    out.mkdir(parents=True, exist_ok=True)

    def raw(data, address, length):
        nt = struct.unpack_from('<I', data, 60)[0]
        base = struct.unpack_from('<I', data, nt + 52)[0]
        tab = nt + 24 + struct.unpack_from('<H', data, nt + 20)[0]
        for i in range(struct.unpack_from('<H', data, nt + 6)[0]):
            _, va, size, off = struct.unpack_from('<IIII', data, tab + 40*i + 8)
            if va <= address-base and address-base+length <= va+size:
                return data[off+address-base-va:off+address-base-va+length]
        raise ValueError(hex(address))

    cs = Cs(CS_ARCH_X86, CS_MODE_32)
    cs.detail = True
    def relocations(data, spans):
        nt=struct.unpack_from('<I',data,60)[0]
        base=struct.unpack_from('<I',data,nt+52)[0]
        size=struct.unpack_from('<I',data,nt+80)[0]
        result={}
        for address,length in spans:
            for instruction in cs.disasm(raw(data,address,length),address):
                for operand in instruction.operands:
                    offset=None;value=None
                    if operand.type==CS_OP_MEM and base<=operand.mem.disp<base+size:
                        assert instruction.disp_size==4
                        offset=instruction.disp_offset;value=operand.mem.disp
                    elif operand.type==CS_OP_IMM and base<=operand.imm<base+size and not instruction.mnemonic.startswith('j') and instruction.mnemonic!='call':
                        assert instruction.imm_size==4
                        offset=instruction.imm_offset;value=operand.imm
                    if offset is not None:
                        result[instruction.address+offset-base]=value
        return sorted(result.items())
    def address_file(path,addresses,table,data,spans):
        patches=relocations(data,spans)
        path.write_bytes(struct.pack('<12I',*addresses,table,len(patches))+b''.join(struct.pack('<2I',*r) for r in patches))
        return patches
    original_closure = []
    for name, address, length in ENTRIES + [('_ftol', 0x575b80, 39)]:
        code = raw(original, address, length)
        calls = []
        for instruction in cs.disasm(code, address):
            if instruction.mnemonic == 'call':
                assert instruction.operands[0].type == CS_OP_IMM
                calls.append(instruction.operands[0].imm)
                assert calls[-1] == 0x575b80
        original_closure.append(dict(name=name, address=address, length=length, sha256=sha(code), calls=calls))

    common = r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "36-trig.h"
#ifdef TRIG_ORIGINAL_ABI
#define TRIG_CALL __fastcall
extern unsigned __cdecl InvokeChecked(unsigned,unsigned,unsigned,unsigned,unsigned*);
#else
#define TRIG_CALL C2_HOOK_FASTCALL
#endif
typedef float (TRIG_CALL *scalar_fn)(unsigned);
typedef void (TRIG_CALL *matrix_fn)(void*, unsigned);
static unsigned char arena[128];
static unsigned table_initial[91];
static unsigned addresses[10];
static float* table;
static int run(FILE* input) {
    unsigned h[5],result,i;
    float value;
    void* dest;
    _setmode(_fileno(stdout),_O_BINARY);
    memcpy(table_initial,table,sizeof(table_initial));
    fwrite(table_initial,sizeof(table_initial),1,stdout);
    while(fread(h,sizeof(h),1,input)==1) {
        if(h[0]>=10 || (h[2]!=0xffffffffu && h[2]>79))return 20;
        memcpy(table,table_initial,sizeof(table_initial));
        if(h[3]) {
            for(i=0;i<91;i++) {
                result = h[3]==1 ? h[4] : h[4] ^ (i*0x01020305u);
                memcpy(table+i,&result,4);
            }
        }
        memset(arena,0xa5,sizeof(arena));
        dest=h[2]==0xffffffffu ? (void*)(arena+32) : (void*)(table+h[2]);
        _controlfp(_PC_53,_MCW_PC);
        result=0x11223344u;
#ifdef TRIG_ORIGINAL_ABI
        if(InvokeChecked(addresses[h[0]],h[0]<6 ? h[1] : (unsigned)(size_t)dest,h[1],h[0]<6,&result))return 22;
        (void)value;
#else
        if(h[0]<6) {
            value=((scalar_fn)(size_t)addresses[h[0]])(h[1]);
            memcpy(&result,&value,4);
        } else {
            ((matrix_fn)(size_t)addresses[h[0]])(dest,h[1]);
        }
#endif
        fwrite(&result,4,1,stdout);
        fwrite(arena,sizeof(arena),1,stdout);
        fwrite(table,sizeof(table_initial),1,stdout);
    }
    return ferror(input) ? 21 : 0;
}
'''
    native = common + '\n#include "' + SOURCE.as_posix() + '"\n'
    native += 'int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;int result;if(argc!=2)return 2;\n'
    native += 'table=gFloat_sine_table;\n'
    native += '\n'.join(f'addresses[{i}]=(unsigned)(size_t){name};' for i,(name,_,_) in enumerate(ENTRIES))
    native += '\nf=fopen(argv[1],"rb");if(!f)return 3;result=run(f);fclose(f);return result;}\n'
    (out/'native.c').write_text(native)
    # Relative code positions stay unchanged; patch only audited absolute operands.
    probe = '#define TRIG_ORIGINAL_ABI\n' + common + r'''
static unsigned saved_stack,saved_scalar,saved_output,saved_top;
unsigned __declspec(naked) __cdecl InvokeChecked(unsigned address,unsigned x,unsigned y,unsigned scalar,unsigned* output) {
    __asm {
        push ebp
        push ebx
        push esi
        push edi
        mov saved_stack,esp
        mov eax,[esp+32]
        mov saved_scalar,eax
        mov eax,[esp+36]
        mov saved_output,eax
        fnstsw ax
        and eax,3800h
        mov saved_top,eax
        mov ebp,13579bdfh
        mov ebx,2468ace0h
        mov esi,11223344h
        mov edi,55667788h
        mov ecx,[esp+24]
        mov edx,[esp+28]
        mov eax,[esp+20]
        call eax
        mov ecx,esp
        mov esp,saved_stack
        xor eax,eax
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
        cmp saved_scalar,0
        je check_top
        mov ecx,saved_output
        fstp dword ptr [ecx]
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
    FILE*f;unsigned char*disk,*image;unsigned size,i,table_address,count,*patches,value,delta;int result;
    IMAGE_NT_HEADERS32*nt;IMAGE_SECTION_HEADER*sections;
    if(argc!=4)return 2;
    f=fopen(argv[3],"rb");if(!f)return 3;
    if(fread(addresses,sizeof(addresses),1,f)!=1||fread(&table_address,4,1,f)!=1||fread(&count,4,1,f)!=1)return 4;
    patches=(unsigned*)malloc(count*8);if(!patches||fread(patches,8,count,f)!=count)return 4;fclose(f);
    f=fopen(argv[2],"rb");if(!f)return 5;fseek(f,0,SEEK_END);size=(unsigned)ftell(f);rewind(f);
    disk=(unsigned char*)malloc(size);if(!disk||fread(disk,size,1,f)!=1)return 6;fclose(f);
    nt=(IMAGE_NT_HEADERS32*)(disk+((IMAGE_DOS_HEADER*)disk)->e_lfanew);
    image=(unsigned char*)VirtualAlloc(NULL,
        nt->OptionalHeader.SizeOfImage,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);
    if(!image)return 7;
    sections=IMAGE_FIRST_SECTION(nt);
    for(i=0;i<nt->FileHeader.NumberOfSections;i++)memcpy(image+sections[i].VirtualAddress,
        disk+sections[i].PointerToRawData,sections[i].SizeOfRawData);
    delta=(unsigned)(ULONG_PTR)image-nt->OptionalHeader.ImageBase;
    for(i=0;i<count;i++) {
        if(patches[2*i]+4>nt->OptionalHeader.SizeOfImage)return 9;
        memcpy(&value,image+patches[2*i],4);if(value!=patches[2*i+1])return 10;
        value+=delta;memcpy(image+patches[2*i],&value,4);
    }
    for(i=0;i<10;i++)addresses[i]+=delta;
    table=(float*)(ULONG_PTR)(table_address+delta);
    f=fopen(argv[1],"rb");if(!f)return 8;result=run(f);fclose(f);
    VirtualFree(image,0,MEM_RELEASE);free(disk);free(patches);return result;
}
'''
    (out/'physical.c').write_text(probe)
    include_dirs = [ROOT/x for x in ('src/carma2/newcommon','src/brender/include','src/carma2/hooks','src/carma2/include')]
    mroot=ROOT/'build-tools/msvc-modern'
    msvc=sorted((mroot/'VC/Tools/MSVC').iterdir())[-1]
    sdk=mroot/'Windows Kits/10'; version=sorted((sdk/'Include').iterdir())[-1].name
    env=dict(os.environ)
    env['PATH']=str(msvc/'bin/Hostx64/x86')+os.pathsep+env['PATH']
    env['INCLUDE']=os.pathsep.join(map(str,[msvc/'include',*[sdk/f'Include/{version}/{part}' for part in ('ucrt','shared','um')]]))
    env['LIB']=os.pathsep.join(map(str,[msvc/'lib/x86',*[sdk/f'Lib/{version}/{part}/x86' for part in ('ucrt','um')]]))
    m5=ROOT/'build-tools/msvc500sp3'; env5=dict(os.environ)
    env5['PATH']=str(m5/'bin')+os.pathsep+str(m5/'redist')+os.pathsep+env5['PATH']
    env5['INCLUDE']=str(m5/'include'); env5['LIB']=str(m5/'lib')
    gcc=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'
    commands={
        'msvc':[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(d) for d in include_dirs],str(out/'native.c'),'/Fo'+str(out/'msvc.obj'),'/Fe'+str(out/'msvc.exe')],
        'msvc5':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(d) for d in include_dirs],str(out/'native.c'),'/Fo'+str(out/'msvc5.obj'),'/Fe'+str(out/'msvc5.exe')],
        'mingw':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-fno-strict-aliasing',*['-I'+str(d) for d in include_dirs],str(out/'native.c'),'-o',str(out/'mingw.exe')],
        'physical':[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(d) for d in include_dirs],str(out/'physical.c'),'/Fo'+str(out/'physical.obj'),'/Fe'+str(out/'physical.exe')],
    }
    for compiler,command in commands.items():
        r=subprocess.run(command,cwd=out,env=env if compiler in ('msvc','physical') else env5 if compiler=='msvc5' else None,capture_output=True,text=True)
        (out/(compiler+'-build.txt')).write_text(r.stdout+r.stderr)
        if r.returncode:raise RuntimeError(r.stdout+r.stderr)
    cases=[]
    # All signed integer degrees around multiple periods and extreme valid input.
    # INT_MIN makes the original access outside its sine table and is excluded.
    rng=random.Random(0x5113d0)
    degrees=list(range(-1080,1081))+[-2147483647,-2147483646,2147483646,2147483647]+[rng.randrange(-2147483647,2147483648) for _ in range(512)]
    for mode in range(4):
        cases.extend((mode,a&0xffffffff,0xffffffff,0,0) for a in degrees)
    # Exhaustive br_angle domain with original lookup table, including matrix stores.
    for mode in range(4,10):
        cases.extend((mode,a,0xffffffff,0,0) for a in range(65536))
    # Dirty high argument bits, table aliases, NaN quieting/sign/payload and guards.
    patterns=(0,0x80000000,1,0x80000001,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc12345,0xffc54321,0x7f800001,0xff812345)
    for mode in range(10):
        angles=(0,90,180,270,-360) if mode<4 else (0,1,16384,32768,49152,65535,0xabcdef01,0xffffffff)
        for angle in angles:
            for bits in patterns:
                cases.append((mode,angle&0xffffffff,0xffffffff,1,bits))
            if mode>=6:
                for offset in (0,1,30,45,60,79):
                    cases.append((mode,angle,offset,0,0))
                    cases.append((mode,angle,offset,2,0x3f800000))
    packed=b''.join(struct.pack('<5I',*c) for c in cases)
    (out/'fixtures.bin').write_bytes(packed)
    original_patches=address_file(out/'original-addresses.bin',[a for _,a,_ in ENTRIES],0x661398,original,[(a,n) for _,a,n in ENTRIES]+[(0x575b80,39)])
    assert {value for _,value in original_patches}=={0x661938,0x6610c8,0x661668,0x661398,0x660f60,0x6617d0,0x661230,0x661500,0x58b218}
    with (out/'original.bin').open('wb') as output:
        subprocess.run([str(out/'physical.exe'),str(out/'fixtures.bin'),str(args.original.resolve()),str(out/'original-addresses.bin')],stdout=output,check=True)
    original_output=(out/'original.bin').read_bytes()
    assert original_output[:364]==raw(original,0x661398,364)
    assert len(original_output)==364+496*len(cases)
    checks=[]
    for compiler in ('msvc','msvc5','mingw'):
        with (out/(compiler+'.bin')).open('wb') as output:
            subprocess.run([str(out/(compiler+'.exe')),str(out/'fixtures.bin')],stdout=output,check=True)
        actual=(out/(compiler+'.bin')).read_bytes()
        if actual!=original_output:
            at=next((i for i,(a,b) in enumerate(zip(actual,original_output)) if a!=b),min(len(actual),len(original_output)))
            index=(at-364)//496
            raise AssertionError(f'{compiler} byte {at}: fixture {index} {cases[index] if index>=0 else "initializer"}, byte in record {(at-364)%496}')
        checks.append(dict(compiler=compiler,comparisons=len(cases),output_sha256=sha(actual)))
    linked=None
    if args.symbols:
        exported=json.loads(args.symbols.read_text());rebuilt=args.recompiled.read_bytes()
        assert exported['original_sha256']==HASH and exported['recompiled_sha256']==sha(rebuilt)
        symbols={s['orig']:s['recomp'] for s in exported['symbols']}
        addresses=[symbols[a] for _,a,_ in ENTRIES]
        # The matching build may share/in-line scalar helpers. Disassemble each
        # identified entry and reject unreviewed imports or indirect call sites.
        allowed=set(addresses)
        ftol=symbols.get(0x575b80)
        if ftol:allowed.add(ftol)
        spans=[]
        for address in sorted(allowed):
            end=min(s['recomp'] for s in exported['symbols'] if s['recomp']>address)
            spans.append((address,end-address))
            for instruction in cs.disasm(raw(rebuilt,address,end-address),address):
                if instruction.mnemonic=='call':
                    assert instruction.operands[0].type==CS_OP_IMM and instruction.operands[0].imm in allowed, f'Unreviewed linked call: {instruction.op_str}'
                if instruction.mnemonic.startswith('j'):
                    assert instruction.operands[0].type==CS_OP_IMM
                    target=instruction.operands[0].imm
                    assert address<=target<end or target in allowed, f'Unreviewed linked branch: {instruction.op_str}'
        linked_patches=address_file(out/'linked-addresses.bin',addresses,symbols[0x661398],rebuilt,spans)
        with (out/'linked.bin').open('wb') as output:
            subprocess.run([str(out/'physical.exe'),str(out/'fixtures.bin'),str(args.recompiled.resolve()),str(out/'linked-addresses.bin')],stdout=output,check=True)
        actual=(out/'linked.bin').read_bytes()
        assert actual==original_output,'Linked physical PE output differs'
        checks.append(dict(compiler='physical-matching',comparisons=len(cases),output_sha256=sha(actual)))
        linked=dict(recompiled_sha256=sha(rebuilt),symbols_sha256=sha(args.symbols.read_bytes()),absolute_data_relocations=linked_patches)
    report=dict(scenario_count=len(cases),comparisons=sum(c['comparisons'] for c in checks),checks=checks,original_sha256=HASH,source_sha256=sha(SOURCE.read_bytes()),header_sha256=sha(SOURCE.with_suffix('.h').read_bytes()),verifier_sha256=sha(Path(__file__).read_bytes()),fixtures_sha256=sha(packed),original_closure=original_closure,original_absolute_data_relocations=original_patches,compiler_commands=commands,linked=linked,precision_control=53,scope='Ten actual functions; full table and arena state; every16bit angle. INT_MIN degree lookup is outside original table contract. No output normalization; only audited absolute data operands relocated.')
    (out/'verification.json').write_text(json.dumps(report,indent=2)+'\n')
    print(f'PASS: {len(cases)} cases, {report["comparisons"]} comparisons.')


if __name__=='__main__':
    main()
