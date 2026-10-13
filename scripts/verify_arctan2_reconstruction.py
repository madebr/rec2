#!/usr/bin/env python3
"""Prove the four ratio-based ArcTan2 entries against physical original PC53.

Actual production bodies execute under MSVC5, modern MSVC and i686 MinGW.
The reviewed original four-entry closure and _ftol execute physically, with
only twelve original constant operands relocated. A common audited x86 caller
captures unrounded double ST0, scalar-store bits, and the actual DrawMapBlip
FSUBR32/FSUB64/_ftol fragment. No scalar or NaN output normalization occurs.
Normal PC53 follows original CRT initializer0x57a2d0's
_controlfp(0x10000,0x30000), independently executed/scanned by the retained
verify_tinted_runtime_reconstruction.py. Alternate precision is not claimed.
"""
import argparse, hashlib, json, os
from pathlib import Path
import math, random, struct, subprocess
from verify_tinted_native_reconstruction import extract

ROOT=Path(__file__).resolve().parents[1]
SOURCE=ROOT/'src/carma2/newcommon/36-trig.c'
HASH='9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
ENTRIES=[('FastFloatArcTan2',0x511ce0,365),('FastScalarArcTan2',0x511e50,18),('FastFloatArcTan2Angle',0x511e70,402),('FastScalarArcTan2Angle',0x512010,18)]
CONSTANTS={0x58b220,0x58b224,0x58b234,0x58b238,0x58b240,0x58b248,0x58b24c,0x58b250,0x58b254,0x58b258,0x58b25c,0x58b260}
def sha(b):return hashlib.sha256(b).hexdigest()
def words(v):return struct.pack('<'+'I'*len(v),*[x&0xffffffff for x in v])
def fb(v):return struct.unpack('<I',struct.pack('<f',v))[0]

def main():
 p=argparse.ArgumentParser(description=__doc__)
 p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE')
 p.add_argument('--output',type=Path,default=ROOT/'build-audit/arctan2-tests')
 p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path)
 a=p.parse_args()
 if bool(a.recompiled)!=bool(a.symbols):p.error('--recompiled and --symbols required together')
 from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM
 original=a.original.read_bytes();assert sha(original)==HASH
 out=a.output.resolve();out.mkdir(parents=True,exist_ok=True)
 def pe(data):
  nt=struct.unpack_from('<I',data,60)[0];return nt,struct.unpack_from('<I',data,nt+52)[0],struct.unpack_from('<I',data,nt+80)[0],nt+24+struct.unpack_from('<H',data,nt+20)[0]
 def raw(data,address,length):
  nt,base,size,t=pe(data)
  for j in range(struct.unpack_from('<H',data,nt+6)[0]):
   _,v,n,o=struct.unpack_from('<IIII',data,t+j*40+8)
   if v<=address-base and address-base+length<=v+n:return data[o+address-base-v:o+address-base-v+length]
  raise ValueError(hex(address))
 cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
 spans=[(addr,n) for name,addr,n in ENTRIES]+[(0x575b80,39)]
 patches={};closure=[]
 for addr,n in spans:
  calls=[]
  for i in cs.disasm(raw(original,addr,n),addr):
   if i.mnemonic=='call':
    assert i.operands[0].type==CS_OP_IMM and i.operands[0].imm in (0x511ce0,0x511e70,0x575b80)
    calls.append(i.operands[0].imm)
   if i.mnemonic.startswith('j'):assert i.operands[0].type==CS_OP_IMM and addr<=i.operands[0].imm<addr+n
   for op in i.operands:
    if op.type==CS_OP_MEM and not op.mem.base and not op.mem.index:
     assert op.mem.disp in CONSTANTS and i.disp_size==4
     patches[i.address+i.disp_offset-0x400000]=op.mem.disp
  closure.append(dict(address=addr,length=n,bytes_sha256=sha(raw(original,addr,n)),calls=calls))
 assert set(patches.values())==CONSTANTS
 caller=raw(original,0x496477,17)
 assert [(i.mnemonic,i.op_str) for i in cs.disasm(caller,0x496477)]==[('fsubr','dword ptr [0x58a388]'),('fsub','qword ptr [0x58a390]'),('call','0x575b80')]
 assert raw(original,0x58a388,4)==struct.pack('<f',360.) and raw(original,0x58a390,8)==struct.pack('<d',-12.25)
 # A shared real x86 caller object allows raw stack words and identical ABI
 # checks under all compilers, without a C float argument load quieting sNaNs.
 helper=r'''
unsigned arc_address,arc_ftol,arc_y,arc_x,arc_mode,arc_output[5];
float arc_caller_float=360.f;double arc_caller_double=-12.25;
static unsigned saved_stack,saved_top;
unsigned __declspec(naked) __cdecl ArcInvoke(void) {
 __asm {
  push ebp
  push ebx
  push esi
  push edi
  mov saved_stack,esp
  fnstsw ax
  and eax,3800h
  mov saved_top,eax
  mov ebp,13579bdfh
  mov ebx,2468ace0h
  mov esi,11223344h
  mov edi,55667788h
  push arc_x
  push arc_y
  call dword ptr [arc_address]
  mov dword ptr [arc_output+12],eax
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
  cmp arc_mode,0
  jne angle
  fnstsw ax
  and eax,3800h
  add eax,800h
  and eax,3800h
  cmp eax,saved_top
  jne failed
  fst qword ptr [arc_output]
  fst dword ptr [arc_output+8]
  fsubr dword ptr [arc_caller_float]
  fsub qword ptr [arc_caller_double]
  call dword ptr [arc_ftol]
  mov dword ptr [arc_output+16],eax
  mov dword ptr [arc_output+12],0
  jmp check_top
 angle:
  and dword ptr [arc_output+12],0ffffh
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
'''
 common=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "36-trig.h"
extern unsigned arc_address,arc_ftol,arc_y,arc_x,arc_mode,arc_output[5];
extern unsigned __cdecl ArcInvoke(void);
static unsigned addresses[4];
static unsigned char* image;
static unsigned image_base,image_size;
static int load_image(const char* name) {
 FILE*f;unsigned char*disk;unsigned size,i;IMAGE_DOS_HEADER*dos;IMAGE_NT_HEADERS*nt;IMAGE_SECTION_HEADER*sec;
 f=fopen(name,"rb");if(!f)return 20;fseek(f,0,SEEK_END);size=(unsigned)ftell(f);rewind(f);disk=(unsigned char*)malloc(size);if(!disk||fread(disk,size,1,f)!=1)return 21;fclose(f);
 dos=(IMAGE_DOS_HEADER*)disk;nt=(IMAGE_NT_HEADERS*)(disk+dos->e_lfanew);sec=IMAGE_FIRST_SECTION(nt);image_base=nt->OptionalHeader.ImageBase;image_size=nt->OptionalHeader.SizeOfImage;image=(unsigned char*)VirtualAlloc(NULL,image_size,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image)return 22;
 memcpy(image,disk,nt->OptionalHeader.SizeOfHeaders);for(i=0;i<nt->FileHeader.NumberOfSections;i++)memcpy(image+sec[i].VirtualAddress,disk+sec[i].PointerToRawData,sec[i].SizeOfRawData);free(disk);return 0;
}
'''
 run=r'''
static int run(const char* name){FILE*f;unsigned h[3];f=fopen(name,"rb");if(!f)return 2;_setmode(_fileno(stdout),_O_BINARY);while(fread(h,sizeof(h),1,f)==1){if(h[0]>=4)return 3;arc_address=addresses[h[0]];arc_mode=h[0]>=2;arc_y=h[1];arc_x=h[2];memset(arc_output,0,sizeof(arc_output));_controlfp(_PC_53,_MCW_PC);if(ArcInvoke())return 4;fwrite(arc_output,sizeof(arc_output),1,stdout);}fclose(f);return 0;}
'''
 # Modern builds use normal repository C calling conventions; adapters expose
 # only the physical two-stack-argument boundary to the shared original caller.
 native=common
 for name in ('TrigArcTanIsNaN','TrigArcTanAbs','TrigArcTanRatio',*[e[0] for e in ENTRIES]):
  body=extract(SOURCE.read_text(),name)
  if name in ('FastFloatArcTan2','FastFloatArcTan2Angle'):
   source=SOURCE.read_text();at=source.index(body)
   before='#if defined(_MSC_VER) && _MSC_VER < 1300\n#pragma auto_inline(off)\n#endif'
   after='#if defined(_MSC_VER) && _MSC_VER < 1300\n#pragma auto_inline(on)\n#endif'
   assert before in source[max(0,at-160):at] and source[at+len(body):].lstrip().startswith(after),'Missing production no-inline boundary'
   body=before+'\n'+body+'\n'+after
  native+='\n'+body
 for i,(name,addr,n) in enumerate(ENTRIES):native+=f'\n{"double" if i<2 else "br_angle"} __stdcall adapter{i}(float y,float x){{return {name}(y,x);}}\n'
 native+=run+'\nint C2_HOOK_CDECL main(int argc,char**argv){int r;if(argc!=3)return 1;r=load_image(argv[2]);if(r)return r;arc_ftol=(unsigned)image+0x175b80;'+''.join(f'addresses[{i}]=(unsigned)adapter{i};' for i in range(4))+'return run(argv[1]);}\n'
 # Native fixture only uses copied _ftol for the caller fragment. Production
 # angle casts execute the compiler's actual conversion and are independently
 # compared with the genuine original function/_ftol result.
 probe=common+run+'\nint C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned h[6],i,n,*patch,value;int r;if(argc!=4)return 1;r=load_image(argv[2]);if(r)return r;f=fopen(argv[3],"rb");if(!f)return 10;if(fread(h,sizeof(h),1,f)!=1)return 11;n=h[5];patch=(unsigned*)malloc(n*8);if(!patch||fread(patch,n*8,1,f)!=1)return 12;fclose(f);for(i=0;i<n;i++){if(patch[2*i]+4>image_size||patch[2*i+1]>=image_size)return 13;memcpy(&value,image+patch[2*i],4);if(value!=image_base+patch[2*i+1])return 14;value=(unsigned)image+patch[2*i+1];memcpy(image+patch[2*i],&value,4);}for(i=0;i<4;i++)addresses[i]=(unsigned)image+h[i];arc_ftol=(unsigned)image+h[4];free(patch);return run(argv[1]);}\n'
 # The original CRT leaf is at its unchanged RVA for both original paths.
 (out/'caller.c').write_text(helper);(out/'native.c').write_text(native);(out/'physical.c').write_text(probe)
 (out/'original-addresses.bin').write_bytes(words([*[e[1]-0x400000 for e in ENTRIES],0x175b80,len(patches)])+b''.join(words([k,v-0x400000]) for k,v in sorted(patches.items())))
 m5=ROOT/'build-tools/msvc500sp3';env5=dict(os.environ);env5['PATH']=str(m5/'bin')+os.pathsep+str(m5/'redist')+os.pathsep+env5['PATH'];env5['INCLUDE']=str(m5/'include');env5['LIB']=str(m5/'lib')
 mroot=ROOT/'build-tools/msvc-modern';msvc=sorted((mroot/'VC/Tools/MSVC').iterdir())[-1];sdk=mroot/'Windows Kits/10';version=sorted((sdk/'Include').iterdir())[-1].name;env=dict(os.environ);env['PATH']=str(msvc/'bin/Hostx64/x86')+os.pathsep+env['PATH'];env['INCLUDE']=os.pathsep.join(map(str,[msvc/'include',*[sdk/f'Include/{version}/{x}' for x in ('ucrt','shared','um')]]));env['LIB']=os.pathsep.join(map(str,[msvc/'lib/x86',*[sdk/f'Lib/{version}/{x}/x86' for x in ('ucrt','um')]]))
 includes=[ROOT/x for x in ('src/carma2/newcommon','src/carma2/hooks','src/brender/include')];gcc=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'
 commands={'caller':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Zl','/c',str(out/'caller.c'),'/Fo'+str(out/'caller.obj')],
 'msvc5':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(x) for x in includes],str(out/'native.c'),str(out/'caller.obj'),'/Fo'+str(out/'msvc5.obj'),'/Fe'+str(out/'msvc5.exe')],
 'msvc':[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(x) for x in includes],str(out/'native.c'),str(out/'caller.obj'),'/Fo'+str(out/'msvc.obj'),'/Fe'+str(out/'msvc.exe')],
 'mingw':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation',*['-I'+str(x) for x in includes],str(out/'native.c'),str(out/'caller.obj'),'-o',str(out/'mingw.exe')],
 'physical':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-DREC2_MATCHING',*['-I'+str(x) for x in includes],str(out/'physical.c'),str(out/'caller.obj'),'-o',str(out/'physical.exe')]}
 for name,command in commands.items():
  r=subprocess.run(command,cwd=out,env=env5 if name in ('caller','msvc5') else env if name=='msvc' else None,capture_output=True,text=True);(out/f'{name}-build.txt').write_text(r.stdout+r.stderr)
  if r.returncode:raise RuntimeError(r.stdout+r.stderr)
 special=[0,0x80000000,1,0x80000001,0x007fffff,0x00800000,0x3f800000,0xbf800000,0x3f000000,0xbf000000,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc00001,0xffc12345,0x7f800001,0xff812345]
 pairs=[(y,x) for y in special for x in special]
 pairs += [(fb(y),fb(x)) for y in (-7,-3,-1,1,3,7) for x in (-7,-3,-1,1,3,7)]
 # Original degrees lie just above a quarter while the scalar store rounds
 # exactly to it; premature float return changes DrawMapBlip's integer by one.
 pairs += [(1143276761,1206390481),(1164495654,1208152645),(1161725274,1198401200),(1150298296,1182368141),(1150003832,1178130967),(1163656581,1189852211)]
 for x in (fb(1),fb(3),fb(7),fb(1e-30),fb(1e30)):
  for y in (x-1,x,x+1):
   for sy in (0,0x80000000):
    for sx in (0,0x80000000):pairs.append((y^sy,x^sx))
 # Ratio thresholds for truncation and caller conversion, including values whose
 # PC53 return differs from its scalar copy before the caller's subtraction.
 for j in (1,2,17,8191):
  for offset in (-1,0,1):pairs.append((fb(j/8192)+offset,fb(1)))
 rng=random.Random(0x511ce0);pairs += [(rng.getrandbits(32),rng.getrandbits(32)) for _ in range(2000)]
 cases=[(entry,y,x) for entry in range(4) for y,x in pairs];fixture=out/'cases.bin';fixture.write_bytes(b''.join(words(c) for c in cases))
 expected=subprocess.check_output([str(out/'physical.exe'),str(fixture),str(a.original.resolve()),str(out/'original-addresses.bin')]);assert len(expected)==20*len(cases);(out/'original.bin').write_bytes(expected)
 wide_differences=[];caller_differences=[]
 for i,(entry,y,x) in enumerate(cases):
  if entry>=2:continue
  wide,scalar,angle,converted=struct.unpack_from('<dfII',expected,20*i)
  if math.isfinite(wide) and math.isfinite(scalar) and wide!=scalar:
   wide_differences.append(i)
   if (int(372.25-scalar)&0xffffffff)!=converted:caller_differences.append(i)
 assert wide_differences and caller_differences,'No physical evidence establishes the wide return/caller boundary'
 records=[]
 for name in ('msvc5','msvc','mingw'):
  actual=subprocess.check_output([str(out/f'{name}.exe'),str(fixture),str(a.original.resolve())]);(out/f'{name}.bin').write_bytes(actual)
  if actual!=expected:
   first=next(i for i,(x,y) in enumerate(zip(actual,expected)) if x!=y);index=first//20;(out/f'{name}-failure.json').write_text(json.dumps(dict(index=index,case=cases[index],original=expected[index*20:index*20+20].hex(),native=actual[index*20:index*20+20].hex(),byte_offset=first%20),indent=2));raise AssertionError(f'ArcTan2 {name} case{index} differs; see{out}')
  records.append(dict(compiler=name,comparisons=len(cases),output_sha256=sha(actual)))
 linked=None
 if a.recompiled:
  rebuilt=a.recompiled.read_bytes();export=json.loads(a.symbols.read_text())
  if export.get('format')!=1 or export.get('original_sha256')!=HASH or export.get('recompiled_sha256')!=sha(rebuilt):raise ValueError('Matching PE or symbol hash differs')
  symbols={v['orig']:v['recomp'] for v in export['symbols']};starts=[symbols[e[1]] for e in ENTRIES];ftol=symbols[0x575b80];allowed={*starts,ftol};nt,base,size,tab=pe(rebuilt);refs={};linked_closure=[]
  for start in sorted(allowed):
   end=min(v['recomp'] for v in export['symbols'] if v['recomp']>start);code=raw(rebuilt,start,end-start);calls=[]
   for i in cs.disasm(code,start):
    if i.mnemonic=='call':
     assert i.operands[0].type==CS_OP_IMM and i.operands[0].imm in allowed,f'Unreviewed matching call{i.op_str}'
     calls.append(i.operands[0].imm)
    if i.mnemonic.startswith('j'):assert i.operands[0].type==CS_OP_IMM and (start<=i.operands[0].imm<end or i.operands[0].imm in allowed)
    for op in i.operands:
     if op.type==CS_OP_MEM and not op.mem.base and not op.mem.index:
      assert base<=op.mem.disp<base+size and i.disp_size==4
      # This four-entry closure has no mutable game/global state. Its absolute
      # operands must point into mapped, nonwritable constant storage.
      target=op.mem.disp-base
      sections=[struct.unpack_from('<IIII',rebuilt,tab+40*j+8)+(struct.unpack_from('<I',rebuilt,tab+40*j+36)[0],) for j in range(struct.unpack_from('<H',rebuilt,nt+6)[0])]
      assert any(va<=target<va+n and not flags&0x80000000 for _,va,n,off,flags in sections)
      refs[i.address+i.disp_offset-base]=op.mem.disp-base
   linked_closure.append(dict(address=start,length=end-start,bytes_sha256=sha(code),calls=calls))
  address_file=out/'linked-addresses.bin';address_file.write_bytes(words([*[v-base for v in starts],ftol-base,len(refs)])+b''.join(words([k,v]) for k,v in sorted(refs.items())))
  actual=subprocess.check_output([str(out/'physical.exe'),str(fixture),str(a.recompiled.resolve()),str(address_file)]);(out/'linked.bin').write_bytes(actual)
  if actual!=expected:
   first=next((i for i,(x,y) in enumerate(zip(actual,expected)) if x!=y),min(len(actual),len(expected)));index=first//20
   (out/'linked-failure.json').write_text(json.dumps(dict(index=index,case=cases[index],original=expected[index*20:index*20+20].hex(),matching=actual[index*20:index*20+20].hex(),byte_offset=first%20),indent=2));raise AssertionError(f'ArcTan2 linked case{index} differs; see{out}')
  records.append(dict(compiler='physical-matching',comparisons=len(cases),output_sha256=sha(actual)))
  linked=dict(recompiled_sha256=sha(rebuilt),symbols_sha256=sha(a.symbols.read_bytes()),closure=linked_closure,absolute_data_relocations=sorted(refs.items()))
 manifest=dict(scenario_count=len(cases),comparisons=sum(v['comparisons'] for v in records),checks=records,original_sha256=HASH,source_sha256=sha(SOURCE.read_bytes()),header_sha256=sha((SOURCE.parent/'36-trig.h').read_bytes()),verifier_sha256=sha(Path(__file__).read_bytes()),caller_object_sha256=sha((out/'caller.obj').read_bytes()),fixtures_sha256=sha(fixture.read_bytes()),physical_original_output_sha256=sha(expected),original_closure=closure,original_absolute_data_relocations=sorted(patches.items()),original_caller_fragment_sha256=sha(caller),wide_vs_scalar_cases=wide_differences,caller_premature_scalar_rounding_changes=caller_differences,compiler_commands=commands,precision_control=53,scope='Four genuine ratio-approximation entries; raw float32 input words, raw double return, scalar store, actual DrawMapBlip FSUBR32/FSUB64/FTOL and low16 angle; RET8/callee-saved/x87 TOP checked. Only audited original constant references relocated. No output normalization. Sticky FPU exception flags and alternate PC modes are outside this proof.')
 if linked:manifest['linked']=linked
 (out/'verification.json').write_text(json.dumps(manifest,indent=2)+'\n');print(f'PASS {len(cases)} physical original scenarios / {manifest["comparisons"]} native/linked comparisons')

if __name__=='__main__':main()
