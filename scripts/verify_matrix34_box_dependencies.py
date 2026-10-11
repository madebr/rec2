"""Validate actual matrix dependencies against hash-checked physical original x87.

Requires Capstone5, scoped MSVC5SP3/modern x86 MSVC and i686 MinGW. Executes
only five no-call/no-branch/no-global original leaves, never the game entry,
imports or TLS. Actual production bodies and repository layouts execute under
native3; full arenas retain PC53, raw NaN bits and overlapping operands without
normalization. Optional immutable rebuilt PE proof requires its symbol export.
"""
import argparse,hashlib,json,os,random,struct,subprocess,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'scripts'))
from verify_tinted_native_reconstruction import extract
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_MEM
HASH='9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
LEAVES=[('BrMatrix34Mul',0x532620,404),('BrMatrix34LPInverse',0x532eb0,175),('BrMatrix34ApplyP',0x5334a0,116),('BrMatrix34ApplyV',0x533520,107),('BrMatrix34TApplyV',0x533690,107)]
OUT=ROOT/'build-audit/finteray-box-next/matrix-leaf-final'
SOURCE=ROOT/'src/brender/core/math/matrix34.c'
def words(a):return struct.pack('<'+'I'*len(a),*[x&0xffffffff for x in a])
def floats(a):return struct.pack('<'+'f'*len(a),*a)
def main():
 global OUT
 parser=argparse.ArgumentParser(description=__doc__)
 parser.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE')
 parser.add_argument('--output',type=Path,default=OUT)
 parser.add_argument('--msvc-root',type=Path,default=ROOT/'build-tools/msvc-modern')
 parser.add_argument('--msvc5-root',type=Path,default=ROOT/'build-tools/msvc500sp3')
 parser.add_argument('--gcc',type=Path,default=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe')
 parser.add_argument('--recompiled',type=Path)
 parser.add_argument('--symbols',type=Path)
 args=parser.parse_args();OUT=args.output.resolve();OUT.mkdir(parents=True,exist_ok=True)
 if bool(args.recompiled)!=bool(args.symbols):parser.error('--recompiled and --symbols must be supplied together')
 image=args.original.read_bytes();assert hashlib.sha256(image).hexdigest()==HASH
 nt=struct.unpack_from('<I',image,60)[0];tab=nt+24+struct.unpack_from('<H',image,nt+20)[0]
 def raw(a,n):
  for i in range(struct.unpack_from('<H',image,nt+6)[0]):
   _,va,sz,off=struct.unpack_from('<IIII',image,tab+40*i+8)
   if va<=a-0x400000<va+sz:return image[off+a-0x400000-va:off+a-0x400000-va+n]
 cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;leaves=[]
 for name,a,n in LEAVES:
  b=raw(a,n);ins=list(cs.disasm(b,a))
  assert all(i.mnemonic!='call' and not i.mnemonic.startswith('j') for i in ins)
  assert all(not(o.type==CS_OP_MEM and not o.mem.base and not o.mem.index) for i in ins for o in i.operands)
  assert any(i.mnemonic=='ret' for i in ins)
  leaves.append(dict(name=name,address=hex(a),bytes_sha256=hashlib.sha256(b).hexdigest(),body_bytes=n))
 common=r'''
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "matrix34.h"
static unsigned header[4];
static unsigned char arena[512];
'''
 driver=common+r'''
#define A(x,y) A->m[x][y]
#define B(x,y) B->m[x][y]
#define C(x,y) C->m[x][y]
#undef BR_MAC3
#define BR_MAC3(a,b,c,d,e,f) ((a)*(b)+(c)*(d)+(e)*(f))
'''
 driver+='\n'.join(extract(SOURCE.read_text(),n) for n,a,s in LEAVES)
 call=r'''
static int run(FILE* file) {
 void *a,*b,*c;
 if(fread(header,sizeof(header),1,file)!=1||fread(arena,sizeof(arena),1,file)!=1)return 3;
 a=arena+header[1];b=arena+header[2];c=arena+header[3];
 _controlfp(_PC_53,_MCW_PC);_setmode(_fileno(stdout),_O_BINARY);
'''
 driver+=call+r'''
 switch(header[0]){
 case 0:BrMatrix34Mul((br_matrix34*)a,(br_matrix34*)b,(br_matrix34*)c);break;
 case 1:BrMatrix34LPInverse((br_matrix34*)a,(br_matrix34*)b);break;
 case 2:BrMatrix34ApplyP((br_vector3*)a,(br_vector3*)b,(br_matrix34*)c);break;
 case 3:BrMatrix34ApplyV((br_vector3*)a,(br_vector3*)b,(br_matrix34*)c);break;
 case 4:BrMatrix34TApplyV((br_vector3*)a,(br_vector3*)b,(br_matrix34*)c);break;
 default:return 4;}
 fwrite(arena,sizeof(arena),1,stdout);return 0;
}
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;int result;if(argc!=2)return 2;f=fopen(argv[1],"rb");if(!f)return 3;result=run(f);fclose(f);return result;}
'''
 probe=common+'static const unsigned addresses[5]={'+','.join(hex(a-0x400000) for n,a,s in LEAVES)+'};\n'
 probe+='static const unsigned sizes[5]={'+','.join(str(s) for n,a,s in LEAVES)+'};\n'
 probe+=r'''
typedef void (__cdecl *leaf3)(void*,void*,void*);
typedef void (__cdecl *leaf2)(void*,void*);
int main(int argc,char**argv){
 FILE*f;unsigned char*disk,*code;unsigned size,i;void*a,*b,*c;
 IMAGE_DOS_HEADER*dos;IMAGE_NT_HEADERS32*nt;IMAGE_SECTION_HEADER*sec;
 if(argc!=3)return 2;f=fopen(argv[1],"rb");if(!f)return 3;
 if(fread(header,sizeof(header),1,f)!=1||fread(arena,sizeof(arena),1,f)!=1)return 4;fclose(f);
 if(header[0]>=5)return 5;f=fopen(argv[2],"rb");if(!f)return 6;
 fseek(f,0,SEEK_END);size=(unsigned)ftell(f);rewind(f);disk=(unsigned char*)VirtualAlloc(NULL,size,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
 if(!disk||fread(disk,size,1,f)!=1)return 7;fclose(f);
 dos=(IMAGE_DOS_HEADER*)disk;nt=(IMAGE_NT_HEADERS32*)(disk+dos->e_lfanew);sec=IMAGE_FIRST_SECTION(nt);
 code=(unsigned char*)VirtualAlloc(NULL,4096,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!code)return 8;
 for(i=0;i<nt->FileHeader.NumberOfSections;i++)if(sec[i].VirtualAddress<=addresses[header[0]]&&addresses[header[0]]+sizes[header[0]]<=sec[i].VirtualAddress+sec[i].SizeOfRawData){memcpy(code,disk+sec[i].PointerToRawData+addresses[header[0]]-sec[i].VirtualAddress,sizes[header[0]]);break;}
 if(i==nt->FileHeader.NumberOfSections)return 9;
 a=arena+header[1];b=arena+header[2];c=arena+header[3];_controlfp(_PC_53,_MCW_PC);_setmode(_fileno(stdout),_O_BINARY);
 if(header[0]==1)((leaf2)code)(a,b);else((leaf3)code)(a,b,c);
 fwrite(arena,sizeof(arena),1,stdout);VirtualFree(code,0,MEM_RELEASE);VirtualFree(disk,0,MEM_RELEASE);return 0;
}
'''
 (OUT/'harness.c').write_text(driver);(OUT/'physical_original.c').write_text(probe)
 includes=[ROOT/p for p in ('src/brender/core/math','src/brender/include','src/carma2/hooks','src/carma2/include')]
 mroot=args.msvc_root;msvc=sorted((mroot/'VC/Tools/MSVC').iterdir())[-1];sdk=mroot/'Windows Kits/10';version=sorted((sdk/'Include').iterdir())[-1].name
 env=dict(os.environ);env['PATH']=str(msvc/'bin/Hostx64/x86')+os.pathsep+env['PATH'];env['INCLUDE']=os.pathsep.join(map(str,[msvc/'include',*[sdk/f'Include/{version}/{part}' for part in ('ucrt','shared','um')]]));env['LIB']=os.pathsep.join(map(str,[msvc/'lib/x86',*[sdk/f'Lib/{version}/{part}/x86' for part in ('ucrt','um')]]))
 m5=args.msvc5_root;env5=dict(os.environ);env5['PATH']=str(m5/'bin')+os.pathsep+str(m5/'redist')+os.pathsep+env5['PATH'];env5['INCLUDE']=str(m5/'include');env5['LIB']=str(m5/'lib')
 gcc=args.gcc
 commands={
 'msvc':[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(p) for p in includes],str(OUT/'harness.c'),'/Fo'+str(OUT/'msvc.obj'),'/Fe'+str(OUT/'msvc.exe')],
 'msvc5':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(p) for p in includes],str(OUT/'harness.c'),'/Fo'+str(OUT/'msvc5.obj'),'/Fe'+str(OUT/'msvc5.exe')],
 'mingw':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror',*['-I'+str(p) for p in includes],str(OUT/'harness.c'),'-o',str(OUT/'mingw.exe')],
 'original':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation',*['-I'+str(p) for p in includes],str(OUT/'physical_original.c'),'-o',str(OUT/'original.exe')]}
 for name,command in commands.items():
  r=subprocess.run(command,cwd=OUT,env=env if name=='msvc' else env5 if name=='msvc5' else None,text=True,capture_output=True);(OUT/f'{name}-build.txt').write_text(r.stdout+r.stderr)
  if r.returncode:raise RuntimeError(r.stdout+r.stderr)
 identity=[1,0,0,0,1,0,0,0,1,2,-4,8]
 general=[1.125,-0.375,0.0625,0.4375,1.75,-0.25,-0.625,0.15625,0.75,2.375,-4.125,8.0625]
 rng=random.Random(0x45ff90);cases=[]
 def case(mode,label,output=0,inp=128,matrix=256,mat=general,mat2=None,vector=[1.25,-2.5,3.125],patch=[]):
  arena=bytearray(b'\xa5'*512)
  arena[inp:inp+48]=floats(mat2 or mat)
  if mode>=2:arena[inp:inp+12]=floats(vector)
  arena[matrix:matrix+48]=floats(mat)
  for offset,value in patch:arena[offset:offset+4]=words([value])
  cases.append(dict(mode=mode,label=label,header=[mode,output,inp,matrix],data=bytes(arena)))
 for mode in range(5):
  case(mode,'identity',mat=identity)
  case(mode,'general')
  for i in range(5):
   mat=[rng.uniform(-8,8) for _ in range(12)];v=[rng.uniform(-8,8) for _ in range(3)]
   case(mode,f'finite-rounding-{i}',mat=mat,mat2=[rng.uniform(-8,8) for _ in range(12)],vector=v)
  # Concrete overlapping records/vectors/matrices; no API restrict contract.
  for delta in (0,4,-4):case(mode,f'output-input-alias-{delta}',output=128+delta)
  for delta in (1,-1):case(mode,f'byte-output-input-alias-{delta}',output=128+delta)
  if mode!=1:
   for delta in (0,4,-4):case(mode,f'output-matrix-alias-{delta}',output=256+delta)
  # Raw transposition/copy fields versus arithmetic quieting, at physical x87.
  for value,label in [(0x80000000,'negative-zero'),(0x7f800001,'positive-snan')]:case(mode,label,patch=[(128,value),(256,value)])
  case(mode,'distinct-qnan-pair',patch=[(128,0x7fc00011),(256,0xffc12345)])
  case(mode,'qnan-snan-pair',patch=[(128,0x7fc00011),(256,0xff812345)])
  case(mode,'snan-qnan-pair',patch=[(128,0x7f800011),(256,0xffc12345)])
 rebuilt=None;rebuilt_map=None;linked_leaves=[];emulation_gaps=[]
 if args.recompiled:
  from unicorn import Uc,UC_ARCH_X86,UC_MODE_32
  from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_FPCW
  rebuilt=args.recompiled.read_bytes();export=json.loads(args.symbols.read_text())
  if export.get('format')!=1 or export.get('original_sha256')!=HASH or export.get('recompiled_sha256')!=hashlib.sha256(rebuilt).hexdigest():raise ValueError('Rebuilt symbol export hashes differ')
  rebuilt_map={v['orig']:v['recomp'] for v in export['symbols']}
  linked_nt=struct.unpack_from('<I',rebuilt,60)[0];linked_tab=linked_nt+24+struct.unpack_from('<H',rebuilt,linked_nt+20)[0];linked_base=struct.unpack_from('<I',rebuilt,linked_nt+52)[0]
  linked_addresses=[];linked_sizes=[]
  for name,address,_ in LEAVES:
   start=rebuilt_map[address];span=min(v['recomp'] for v in export['symbols'] if v['recomp']>start)-start
   for j in range(struct.unpack_from('<H',rebuilt,linked_nt+6)[0]):
    _,va,n,offset=struct.unpack_from('<IIII',rebuilt,linked_tab+40*j+8)
    if va<=start-linked_base<va+n:
     code=rebuilt[offset+start-linked_base-va:offset+start-linked_base-va+span];break
   instructions=list(cs.disasm(code,start))
   if any(i.mnemonic=='call' or i.mnemonic.startswith('j') for i in instructions) or any(o.type==CS_OP_MEM and not o.mem.base and not o.mem.index for i in instructions for o in i.operands):raise AssertionError('Linked leaf is outside approved no-call/no-branch/no-global physical scope')
   assert any(i.mnemonic=='ret' for i in instructions)
   linked_addresses.append(start-linked_base);linked_sizes.append(span);linked_leaves.append(dict(name=name,address=hex(start),bytes_sha256=hashlib.sha256(code).hexdigest(),body_bytes=span))
  original_array='static const unsigned addresses[5]={'+','.join(hex(a-0x400000) for n,a,s in LEAVES)+'};'
  original_sizes='static const unsigned sizes[5]={'+','.join(str(s) for n,a,s in LEAVES)+'};'
  linked_probe=probe.replace(original_array,'static const unsigned addresses[5]={'+','.join(hex(a) for a in linked_addresses)+'};').replace(original_sizes,'static const unsigned sizes[5]={'+','.join(str(s) for s in linked_sizes)+'};')
  (OUT/'physical_matching.c').write_text(linked_probe)
  linked_command=[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation',*['-I'+str(p) for p in includes],str(OUT/'physical_matching.c'),'-o',str(OUT/'physical-matching.exe')]
  r=subprocess.run(linked_command,cwd=OUT,text=True,capture_output=True);(OUT/'physical-matching-build.txt').write_text(r.stdout+r.stderr)
  if r.returncode:raise RuntimeError(r.stdout+r.stderr)
  def matching(c):
   u=Uc(UC_ARCH_X86,UC_MODE_32);pe=struct.unpack_from('<I',rebuilt,60)[0];base=struct.unpack_from('<I',rebuilt,pe+52)[0];size=struct.unpack_from('<I',rebuilt,pe+80)[0]
   u.mem_map(base,(size+4095)&~4095);heap,stack,stop=0x1000000,0x1080000,0x1082000;u.mem_map(heap,0x100000)
   table=pe+24+struct.unpack_from('<H',rebuilt,pe+20)[0]
   for j in range(struct.unpack_from('<H',rebuilt,pe+6)[0]):
    _,va,n,offset=struct.unpack_from('<IIII',rebuilt,table+40*j+8)
    if n:u.mem_write(base+va,rebuilt[offset:offset+n])
   u.mem_write(heap,c['data']);parameters=[heap+c['header'][1],heap+c['header'][2]]
   if c['mode']!=1:parameters.append(heap+c['header'][3])
   u.mem_write(stack,words([stop,*parameters]));u.reg_write(UC_X86_REG_ESP,stack);u.reg_write(UC_X86_REG_FPCW,0x27f)
   u.emu_start(rebuilt_map[LEAVES[c['mode']][1]],stop,count=100000)
   if u.reg_read(UC_X86_REG_EIP)!=stop or u.reg_read(UC_X86_REG_ESP)!=stack+4:raise AssertionError('Matching matrix instruction budget or cdecl stack cleanup differs')
   return bytes(u.mem_read(heap,512))
 results=[];failures=[]
 for i,c in enumerate(cases):
  path=OUT/f'case-{i:03d}.bin';path.write_bytes(words(c['header'])+c['data'])
  expected=subprocess.check_output([str(OUT/'original.exe'),str(path),str(args.original.resolve())]);assert len(expected)==512
  (OUT/f'case-{i:03d}.original.bin').write_bytes(expected)
  checks=[(compiler,subprocess.check_output([str(OUT/f'{compiler}.exe'),str(path)])) for compiler in ('msvc5','msvc','mingw')]
  if rebuilt:
   linked_actual=subprocess.check_output([str(OUT/'physical-matching.exe'),str(path),str(args.recompiled.resolve())]);checks.append(('matching-physical',linked_actual))
   emulated=matching(c)
   if emulated!=linked_actual:
    emulation_gaps.append(dict(index=i,function=LEAVES[c['mode']][0],label=c['label'],differences=[dict(offset=j,physical=linked_actual[j:j+4].hex(),unicorn=emulated[j:j+4].hex()) for j in range(0,512,4) if emulated[j:j+4]!=linked_actual[j:j+4]]))
    (OUT/f'case-{i:03d}.matching-unicorn.bin').write_bytes(emulated)
  for compiler,actual in checks:
   assert len(actual)==512
   differing=[j for j in range(0,512,4) if expected[j:j+4]!=actual[j:j+4]]
   row=dict(index=i,function=LEAVES[c['mode']][0],label=c['label'],compiler=compiler,match=not differing,header=c['header'],differences=[dict(offset=j,original=expected[j:j+4].hex(),native=actual[j:j+4].hex()) for j in differing])
   results.append(row)
   if differing:
    failures.append(row);(OUT/f'case-{i:03d}.{compiler}.bin').write_bytes(actual)
 manifest=dict(original_sha256=HASH,source_sha256=hashlib.sha256(SOURCE.read_bytes()).hexdigest(),harness_sha256=hashlib.sha256(driver.encode()).hexdigest(),physical_probe_sha256=hashlib.sha256(probe.encode()).hexdigest(),functions=leaves,commands=commands,cases=len(cases),comparisons=len(results),failures=len(failures),results=results)
 if rebuilt:manifest.update(recompiled_sha256=hashlib.sha256(rebuilt).hexdigest(),symbols_sha256=hashlib.sha256(args.symbols.read_bytes()).hexdigest(),linked_leaves=linked_leaves,physical_linked_command=linked_command,physical_linked_probe_sha256=hashlib.sha256(linked_probe.encode()).hexdigest(),unicorn_diagnostics=emulation_gaps,matching_oracle='Actual linked leaf instructions executed physically at PC53; Unicorn differences retained independently, never normalized')
 (OUT/'verification.json').write_text(json.dumps(manifest,indent=2)+'\n')
 for name,_,_ in LEAVES:
  rows=[r for r in results if r['function']==name];bad=[r for r in rows if not r['match']];print(name,f'{len(rows)-len(bad)}/{len(rows)} matches',[(r['label'],r['compiler']) for r in bad])
 print('No output normalization; all originals physically execute at PC53.')
 if failures:raise AssertionError(f'{len(failures)} matrix comparisons differ; see {OUT}')
if __name__=='__main__':main()
