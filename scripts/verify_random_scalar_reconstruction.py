#!/usr/bin/env python3
"""Physical PC53 proof of four actual random-scalar bodies, with rand controlled.

Only the CRT integer-rand boundary is replaced. Original operand order, raw
double return, scalar store, actual FlameAnimate FMUL32/store fragment, RNG call
count and x87 exception flags at the RNG boundary are compared. Raw argument
words bypass C caller NaN quieting. No output normalization is applied.
"""
import argparse,hashlib,json,math,os,random,struct,subprocess
from pathlib import Path
from verify_tinted_native_reconstruction import extract
ROOT=Path(__file__).resolve().parents[1]
SOURCE=ROOT/'src/carma2/newcommon/41-utility.c'
HASH='9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
ENTRIES=[('FRandomBetween',0x5135b0,38),('FRandomPosNeg',0x5135e0,50),('SRandomBetween',0x513620,38),('SRandomPosNeg',0x513650,50)]
def sha(x):return hashlib.sha256(x).hexdigest()
def words(x):return struct.pack('<'+'I'*len(x),*[v&0xffffffff for v in x])
def fb(x):return struct.unpack('<I',struct.pack('<f',x))[0]
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE');p.add_argument('--output',type=Path,default=ROOT/'build-audit/random-scalar-tests');p.add_argument('--expect-current-mismatch',action='store_true');p.add_argument('--symbols',type=Path);p.add_argument('--recompiled',type=Path);a=p.parse_args()
 if bool(a.symbols)!=bool(a.recompiled):p.error('--symbols and --recompiled required together')
 original=a.original.read_bytes();assert sha(original)==HASH;out=a.output.resolve();out.mkdir(parents=True,exist_ok=True)
 from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM
 cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
 def pe(data):
  nt=struct.unpack_from('<I',data,60)[0];return nt,struct.unpack_from('<I',data,nt+52)[0],struct.unpack_from('<I',data,nt+80)[0],nt+24+struct.unpack_from('<H',data,nt+20)[0]
 def raw(data,address,length):
  nt,base,size,t=pe(data)
  for j in range(struct.unpack_from('<H',data,nt+6)[0]):
   _,v,n,o=struct.unpack_from('<IIII',data,t+j*40+8)
   if v<=address-base and address-base+length<=v+n:return data[o+address-base-v:o+address-base-v+length]
  raise ValueError(hex(address))
 assert raw(original,0x58b268,4)==words([0xb8000000])
 assert [(i.mnemonic,i.op_str) for i in cs.disasm(raw(original,0x4fbf46,6),0x4fbf46)]==[('fmul','dword ptr [0x58b0d8]')]
 caller_constant=raw(original,0x58b0d8,4);patches={}
 for name,address,n in ENTRIES:
  for i in cs.disasm(raw(original,address,n),address):
   if i.mnemonic=='call':assert i.operands[0].type==CS_OP_IMM and i.operands[0].imm==0x576970
   for op in i.operands:
    if op.type==CS_OP_MEM and not op.mem.base and not op.mem.index:assert op.mem.disp==0x58b268;patches[i.address+i.disp_offset-0x400000]=op.mem.disp-0x400000
 helper=r'''
unsigned random_address,random_mode,random_a,random_b,random_value,random_count,random_status,random_cw,random_output[4];
unsigned random_caller_bits=CALLER;
static unsigned saved_stack,saved_top;
int __declspec(naked) __cdecl ControlledRand(void){__asm{
 fnstsw ax
 movzx eax,ax
 and eax,3fh
 mov random_status,eax
 fnstcw word ptr [random_cw]
 inc random_count
 mov eax,random_value
 ret
}}
unsigned __declspec(naked) __cdecl RandomInvoke(void){__asm{
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
 cmp random_mode,0
 jne posneg
 push random_b
 posneg:
 push random_a
 call dword ptr [random_address]
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
 fnstsw ax
 and eax,3800h
 add eax,800h
 and eax,3800h
 cmp eax,saved_top
 jne failed
 fst qword ptr [random_output]
 fst dword ptr [random_output+8]
 fmul dword ptr [random_caller_bits]
 fstp dword ptr [random_output+12]
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
}}
'''.replace('CALLER',str(struct.unpack('<I',caller_constant)[0]))
 common=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "41-utility.h"
extern unsigned random_address,random_mode,random_a,random_b,random_value,random_count,random_status,random_cw,random_output[4];
extern unsigned __cdecl RandomInvoke(void);
extern int __cdecl ControlledRand(void);
static unsigned addresses[4];static unsigned char*image;static unsigned image_base,image_size;
static int load_image(const char*name){FILE*f;unsigned char*disk;unsigned n,i;IMAGE_NT_HEADERS*nt;IMAGE_SECTION_HEADER*sec;f=fopen(name,"rb");if(!f)return 20;fseek(f,0,SEEK_END);n=(unsigned)ftell(f);rewind(f);disk=(unsigned char*)malloc(n);if(!disk||fread(disk,n,1,f)!=1)return 21;fclose(f);nt=(IMAGE_NT_HEADERS*)(disk+((IMAGE_DOS_HEADER*)disk)->e_lfanew);sec=IMAGE_FIRST_SECTION(nt);image_base=nt->OptionalHeader.ImageBase;image_size=nt->OptionalHeader.SizeOfImage;image=(unsigned char*)VirtualAlloc(NULL,image_size,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image)return 22;memcpy(image,disk,nt->OptionalHeader.SizeOfHeaders);for(i=0;i<nt->FileHeader.NumberOfSections;i++)memcpy(image+sec[i].VirtualAddress,disk+sec[i].PointerToRawData,sec[i].SizeOfRawData);free(disk);return 0;}
static int run(const char*name){FILE*f;unsigned h[4];f=fopen(name,"rb");if(!f)return 2;_setmode(_fileno(stdout),_O_BINARY);while(fread(h,sizeof(h),1,f)==1){if(h[0]>=4)return 3;random_address=addresses[h[0]];random_mode=h[0]&1;random_a=h[1];random_b=h[2];random_value=h[3];random_count=random_status=random_cw=0;memset(random_output,0,sizeof(random_output));_controlfp(_PC_53,_MCW_PC);_clearfp();if(RandomInvoke())return 4;fwrite(random_output,sizeof(random_output),1,stdout);fwrite(&random_count,4,1,stdout);fwrite(&random_status,4,1,stdout);fwrite(&random_cw,4,1,stdout);}fclose(f);return 0;}
'''
 source=SOURCE.read_text();native=(common[:common.index('static int load_image')]+common[common.index('static int run'):]).replace('static unsigned char*image;static unsigned image_base,image_size;','')+'\nint C2_HOOK_CDECL rand(void){return ControlledRand();}\n'
 for name in ('UtilityRandomNegative','UtilityRandomScalar',*[e[0] for e in ENTRIES]):
  if name.startswith('Utility') and name+'(' not in source:continue
  native+='\n'+extract(source,name)
 # Preserve raw input words while adapting the normal native cdecl boundary.
 # Passing through C float arguments can load denormals/quiet NaNs before rand.
 native+='\nstatic volatile unsigned body_addresses[4];\n'
 for i,(name,address,n) in enumerate(ENTRIES):native+=f'\ndouble __stdcall adapter{i}(unsigned x'+(',unsigned y' if i%2==0 else '')+') {typedef double (C2_HOOK_STDCALL *raw_fn)(unsigned'+(',unsigned' if i%2==0 else '')+f');return ((raw_fn)(size_t)body_addresses[{i}])(x'+(',y' if i%2==0 else '')+');}\n'
 native+='\nint C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;'+''.join(f'body_addresses[{i}]=(unsigned){name};addresses[{i}]=(unsigned)adapter{i};' for i,(name,address,n) in enumerate(ENTRIES))+'return run(argv[1]);}\n'
 probe=common+'\nint C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned h[6],i,*patch,n,value,relative;int r;if(argc!=4)return 1;r=load_image(argv[2]);if(r)return r;f=fopen(argv[3],"rb");if(!f)return 10;if(fread(h,sizeof(h),1,f)!=1)return 11;n=h[5];patch=(unsigned*)malloc(n*8);if(n&&(!patch||fread(patch,n*8,1,f)!=1))return 12;fclose(f);for(i=0;i<n;i++){if(patch[2*i]+4>image_size||patch[2*i+1]>=image_size)return 13;memcpy(&value,image+patch[2*i],4);if(value!=image_base+patch[2*i+1])return 14;value=(unsigned)image+patch[2*i+1];memcpy(image+patch[2*i],&value,4);}for(i=0;i<4;i++)addresses[i]=(unsigned)image+h[i];image[h[4]]=0xe9;relative=(unsigned)ControlledRand-((unsigned)image+h[4]+5);memcpy(image+h[4]+1,&relative,4);free(patch);return run(argv[1]);}\n'
 (out/'caller.c').write_text(helper);(out/'native.c').write_text(native);(out/'physical.c').write_text(probe);(out/'original-addresses.bin').write_bytes(words([*[e[1]-0x400000 for e in ENTRIES],0x176970,len(patches)])+b''.join(words([k,v]) for k,v in sorted(patches.items())))
 m5=ROOT/'build-tools/msvc500sp3';env5=dict(os.environ);env5['PATH']=str(m5/'bin')+os.pathsep+str(m5/'redist')+os.pathsep+env5['PATH'];env5['INCLUDE']=str(m5/'include');env5['LIB']=str(m5/'lib');msroot=ROOT/'build-tools/msvc-modern';ms=sorted((msroot/'VC/Tools/MSVC').iterdir())[-1];sdk=msroot/'Windows Kits/10';v=sorted((sdk/'Include').iterdir())[-1].name;env=dict(os.environ);env['PATH']=str(ms/'bin/Hostx64/x86')+os.pathsep+env['PATH'];env['INCLUDE']=os.pathsep.join(map(str,[ms/'include',*[sdk/f'Include/{v}/{x}' for x in ('ucrt','shared','um')]]));env['LIB']=os.pathsep.join(map(str,[ms/'lib/x86',*[sdk/f'Lib/{v}/{x}/x86' for x in ('ucrt','um')]]));inc=[ROOT/x for x in ('src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/brender/include','src/carma2/s3/include','src/carma2/platform')];gcc=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'
 commands={'caller':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Zl','/c',str(out/'caller.c'),'/Fo'+str(out/'caller.obj')],'msvc5':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(x) for x in inc],str(out/'native.c'),str(out/'caller.obj'),'/Fo'+str(out/'msvc5.obj'),'/Fe'+str(out/'msvc5.exe')],'msvc':[str(ms/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(x) for x in inc],str(out/'native.c'),str(out/'caller.obj'),'/Fo'+str(out/'msvc.obj'),'/Fe'+str(out/'msvc.exe')],'mingw':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation',*['-I'+str(x) for x in inc],str(out/'native.c'),str(out/'caller.obj'),'-o',str(out/'mingw.exe')],'physical':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-DREC2_MATCHING',*['-I'+str(x) for x in inc],str(out/'physical.c'),str(out/'caller.obj'),'-o',str(out/'physical.exe')]}
 for name,cmd in commands.items():
  r=subprocess.run(cmd,cwd=out,env=env5 if name in ('caller','msvc5') else env if name=='msvc' else None,capture_output=True,text=True);(out/f'{name}-build.txt').write_text(r.stdout+r.stderr)
  if r.returncode:raise RuntimeError(r.stdout+r.stderr)
 special=[0,0x80000000,1,0x80000001,0x7fffff,0x800000,fb(1),fb(-1),0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc12345,0xffc54321,0x7f800001,0xff812345]
 cases=[(0,fb(0),fb(1),32767)]+[(entry,x,y,r) for entry in range(4) for x in special for y in (special if entry%2==0 else [0]) for r in (0,1,16384,32766,32767)]
 cases += [(entry,fb(x),fb(y),r) for entry in range(4) for x,y in ((1,1.5),(.5,1),(-.03,0),(.5,.99),(1,3),(16777216,16777218)) for r in (0,1,13,8191,8192,16383,16384,32766,32767)]
 rng=random.Random(0x5135b0);cases += [(entry,rng.getrandbits(32),rng.getrandbits(32),rng.randrange(32768)) for entry in range(4) for _ in range(400)]
 fixture=out/'cases.bin';fixture.write_bytes(b''.join(words(c) for c in cases));expected=subprocess.check_output([str(out/'physical.exe'),str(fixture),str(a.original.resolve()),str(out/'original-addresses.bin')]);assert len(expected)==28*len(cases);(out/'original.bin').write_bytes(expected);records=[];failures=[]
 wide_differences=[];caller_differences=[];coefficient=struct.unpack('<f',caller_constant)[0]
 for i in range(len(cases)):
  wide,scalar,caller=struct.unpack_from('<dff',expected,i*28)
  if math.isfinite(wide) and math.isfinite(scalar):
   if wide!=scalar:wide_differences.append(i)
   try:rounded=fb(scalar*coefficient)
   except OverflowError:continue
   if rounded!=struct.unpack_from('<I',expected,i*28+12)[0]:caller_differences.append(i)
 assert wide_differences and caller_differences,'No wide-return/caller distinction exercised'
 for name in ('msvc5','msvc','mingw'):
  actual=subprocess.check_output([str(out/f'{name}.exe'),str(fixture)]);(out/f'{name}.bin').write_bytes(actual)
  if actual!=expected:
   first=next(i for i,(x,y) in enumerate(zip(actual,expected)) if x!=y);index=first//28;failure=dict(compiler=name,index=index,case=cases[index],original=expected[index*28:index*28+28].hex(),native=actual[index*28:index*28+28].hex(),byte_offset=first%28);failures.append(failure)
   (out/f'{name}-failure.json').write_text(json.dumps(failure,indent=2))
  records.append(dict(compiler=name,comparisons=len(cases),output_sha256=sha(actual),match=actual==expected))
 linked=None
 if a.symbols:
  rebuilt=a.recompiled.read_bytes();export=json.loads(a.symbols.read_text())
  if export.get('format')!=1 or export.get('original_sha256')!=HASH or export.get('recompiled_sha256')!=sha(rebuilt):raise ValueError('Matching PE or symbol hash differs')
  symbols={v['orig']:v['recomp'] for v in export['symbols']};starts=[symbols[e[1]] for e in ENTRIES];rand_address=symbols[0x576970];allowed={*starts,rand_address};nt,base,size,tab=pe(rebuilt);refs={};closure=[]
  for start in starts:
   end=min(v['recomp'] for v in export['symbols'] if v['recomp']>start);code=raw(rebuilt,start,end-start);calls=[]
   for ins in cs.disasm(code,start):
    if ins.mnemonic=='call':assert ins.operands[0].type==CS_OP_IMM and ins.operands[0].imm in allowed,f'Unapproved matching call{ins.op_str}';calls.append(ins.operands[0].imm)
    if ins.mnemonic.startswith('j'):assert ins.operands[0].type==CS_OP_IMM and (start<=ins.operands[0].imm<end or ins.operands[0].imm in allowed)
    for op in ins.operands:
     if op.type==CS_OP_MEM and not op.mem.base and not op.mem.index:
      assert base<=op.mem.disp<base+size and ins.disp_size==4
      refs[ins.address+ins.disp_offset-base]=op.mem.disp-base
   closure.append(dict(address=start,length=end-start,bytes_sha256=sha(code),calls=calls))
  address_file=out/'linked-addresses.bin';address_file.write_bytes(words([*[v-base for v in starts],rand_address-base,len(refs)])+b''.join(words([k,v]) for k,v in sorted(refs.items())))
  actual=subprocess.check_output([str(out/'physical.exe'),str(fixture),str(a.recompiled.resolve()),str(address_file)]);(out/'linked.bin').write_bytes(actual)
  if actual!=expected:
   first=next((i for i,(x,y) in enumerate(zip(actual,expected)) if x!=y),min(len(actual),len(expected)));index=first//28;failure=dict(compiler='physical-matching',index=index,case=cases[index],original=expected[index*28:index*28+28].hex(),matching=actual[index*28:index*28+28].hex(),byte_offset=first%28);failures.append(failure);(out/'linked-failure.json').write_text(json.dumps(failure,indent=2))
  records.append(dict(compiler='physical-matching',comparisons=len(cases),output_sha256=sha(actual),match=actual==expected));linked=dict(recompiled_sha256=sha(rebuilt),symbols_sha256=sha(a.symbols.read_bytes()),closure=closure,controlled_rand=rand_address,absolute_data_relocations=sorted(refs.items()))
 manifest=dict(scenario_count=len(cases),comparisons=sum(x['comparisons'] for x in records),checks=records,failures=failures,original_sha256=HASH,source_sha256=sha(SOURCE.read_bytes()),header_sha256=sha((SOURCE.parent/'41-utility.h').read_bytes()),verifier_sha256=sha(Path(__file__).read_bytes()),fixtures_sha256=sha(fixture.read_bytes()),caller_object_sha256=sha((out/'caller.obj').read_bytes()),physical_original_output_sha256=sha(expected),original_absolute_data_relocations=sorted(patches.items()),caller_constant_hex=caller_constant.hex(),wide_vs_scalar_cases=wide_differences,caller_premature_scalar_rounding_changes=caller_differences,compiler_commands=commands,precision_control=53,limitations='CRT rand is the sole controlled integer boundary (standard 0..32767 range); callback captures x87 exception flags, precision word and count. Native SSE sticky flags are not equated with x87 flags. Raw double/scalar/caller outputs remain unnormalized; no full FlameAnimate behavior claimed. Non-x86 arithmetic fallback and wider-CRT RNG distributions are outside these three Windows32 toolchain proofs.')
 if linked:manifest['linked']=linked
 (out/'verification.json').write_text(json.dumps(manifest,indent=2)+'\n')
 if a.expect_current_mismatch:assert failures,'Expected old implementation mismatch absent';print('Established existing mismatch',failures[0]);return
 if failures:raise AssertionError(f'Random scalar mismatch: {failures[0]}; see{out}')
 print(f'PASS {len(cases)} physical original scenarios / {manifest["comparisons"]} native comparisons')
if __name__=='__main__':main()


