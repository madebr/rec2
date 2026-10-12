#!/usr/bin/env python3
"""Physical PC53 proof of opponent straight/corner/constraint/speed navigation.

The original four bodies and actual route accessors, IRandomBetween and CRT
_ftol execute physically; CRT rand and PDEnterDebugger are controlled boundaries.
Production bodies are extracted unchanged for MSVC5, MSVC and MinGW builds.
"""
import argparse,json,os,re,struct,subprocess
from pathlib import Path
from verify_tinted_native_reconstruction import extract
from verify_opponent_route_reconstruction import ROOT,HASH,pe,raw,sha,words
SOURCE=ROOT/'src/carma2/newcommon/26-oppoproc.c'
ENTRIES=[('GetStraight',0x4b13d0,395),('CalcCorners',0x4b1560,847),('SetMaxSpeedFromSOCs',0x4b18b0,509),('CalcSOCs',0x4b1ab0,481),('IRandomBetween',0x513520,34),('DoNotDprintf_opponent',0x4a8160,1),('GetOpponentsNextSection',0x4aea30,114),('GetOpponentsSectionStartNodePoint',0x4aeb90,166),('GetOpponentsSectionFinishNodePoint',0x4aec40,156),('GetOpponentsSectionWidth',0x4aece0,99),('GetOpponentsSectionMaxSpeed',0x4aedf0,151),('__ftol',0x575b80,39)]
GLOBALS={'gProgram_state':(0x75bb80,0x5ed0),'gFrame_period_for_this_munging':(0x79efc4,4)}
HELPERS=['NavigationFloatDifference','NavigationAdd','NavigationSubtract','NavigationMultiply','NavigationDivide','NavigationSqrt','NavigationScalarRound','NavigationOrderedGreater','NavigationLessOrUnordered','NavigationTruncate','NavigationCornerSectionLength']
FIELDS='mode count section geometry width speed desired px pz period corner_count extent radius corner_index aliases mutate mutation_at rng rng_step cheating trail direction seed reserved0 reserved1 reserved2 reserved3 reserved4 reserved5 reserved6 reserved7 reserved8'.split()
RECORD_BYTES=16+128+4+0x5ed0+0x20000
def fb(x):return struct.unpack('<I',struct.pack('<f',x))[0]
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE');p.add_argument('--output',type=Path,default=ROOT/'build-audit/opponent-navigation-tests');p.add_argument('--symbols',type=Path);p.add_argument('--recompiled',type=Path);p.add_argument('--case-index',type=int,action='append');a=p.parse_args()
 if bool(a.symbols)!=bool(a.recompiled):p.error('--symbols and --recompiled required together')
 b=a.original.read_bytes();assert sha(b)==HASH;out=a.output.resolve();out.mkdir(parents=True,exist_ok=True)
 from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM
 cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
 def relocation(data,entries,globals_,debug,rand):
  nt,base,size,tab=pe(data);refs={};closure=[];allowed={debug,rand,*[v[1] for v in entries]}
  def translated(value):
   for name,(start,n) in globals_.items():
    if start<=value<start+n:return 0x20000000+GLOBALS[name][0]-0x650000+value-start,1
   assert base<=value<base+size,hex(value);return value-base,0
  for name,start,n in entries:
   insns=list(cs.disasm(raw(data,start,n),start));assert sum(x.size for x in insns)==n,(name,n)
   calls=[]
   for ins in insns:
    if ins.mnemonic=='call' or ins.mnemonic.startswith('j'):
     op=ins.operands[0];assert op.type==CS_OP_IMM,(name,ins.op_str)
     assert op.imm in allowed or (ins.mnemonic.startswith('j') and start<=op.imm<start+n),(name,hex(ins.address),ins.op_str)
     if ins.mnemonic=='call':calls.append(op.imm)
    for op in ins.operands:
     if op.type==CS_OP_MEM and not op.mem.base and base<=op.mem.disp<base+size:
      assert ins.disp_size==4;refs[ins.address+ins.disp_offset-base]=(op.mem.disp,*translated(op.mem.disp))
     if op.type==CS_OP_IMM and base<=op.imm<base+size and not (ins.mnemonic=='call' or ins.mnemonic.startswith('j')):
      assert ins.imm_size==4;refs[ins.address+ins.imm_offset-base]=(op.imm,*translated(op.imm))
   closure.append(dict(name=name,address=start,length=n,sha256=sha(raw(data,start,n)),calls=sorted(set(calls))))
  return dict(base=base,size=size,refs=refs,closure=closure,entries=[v[1]-base for v in entries[:4]],debug=debug-base,rand=rand-base)
 def save(meta,path):path.write_bytes(words([*meta['entries'],meta['debug'],meta['rand'],len(meta['refs'])])+b''.join(words([k,*v]) for k,v in sorted(meta['refs'].items())))
 original=relocation(b,ENTRIES,GLOBALS,0x51a7b0,0x576970);save(original,out/'original-addresses.bin')
 caller=r'''
unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison;static unsigned saved_stack,saved_top;
unsigned __declspec(naked) __cdecl BuilderInvoke(void){__asm{
 push ebp
 push ebx
 push esi
 push edi
 mov saved_stack,esp
 fnstsw ax
 and eax,3800h
 mov saved_top,eax
 /* Poison only callee stack, descending through guard pages in order. */
 lea edi,[esp-4]
 mov eax,builder_poison
 mov ecx,4096
 std
 rep stosd
 cld
 mov ebp,13579bdfh
 mov ebx,2468ace0h
 mov esi,11223344h
 mov edi,55667788h
 mov eax,builder_stack_count
 test eax,eax
 jz argsready
 args:
 dec eax
 push dword ptr [builder_stack+eax*4]
 test eax,eax
 jnz args
 argsready:
 mov ecx,builder_x
 mov edx,builder_y
 call dword ptr [builder_address]
 cmp builder_mode,2
 jne resultready
 xor eax,eax
 resultready:
 mov builder_result,eax
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
'''
 common=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "26-oppoproc.h"
#include "30-opponent.h"
#include "41-utility.h"
#include "08-loading1.h"
#include "globvars.h"
#include "platform.h"
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison;
extern unsigned __cdecl BuilderInvoke(void);
static unsigned addresses[4],h[32],case_index,event_count,rng_count,step;static unsigned char*heap;static char message[128];
#define OBJ(type,off) ((type*)(heap+0x200000+(off)))
#define OPP OBJ(tOpponent_spec,0x100)
#define CAR OBJ(tCar_spec,0x1000)
#define PURSUEE OBJ(tCar_spec,0x3000)
#define START OBJ(br_vector2,0x7000)
#define FINISH OBJ(br_vector2,0x7010)
#define WIDTH OBJ(float,0x7020)
#define DESIRED OBJ(float,0x7030)
#define CORNERS OBJ(tCorner,0x7100)
#define SOCS OBJ(tSOC,0x7200)
#define gProgram_state (*(tProgram_state*)(heap+0x10bb80))
#define gFrame_period_for_this_munging (*(tU32*)(heap+0x14efc4))
typedef char layouts[sizeof(void*)==4&&sizeof(tProgram_state)==0x5ed0&&sizeof(tOpponent_spec)==420&&sizeof(tPath_node)==32&&sizeof(tPath_section)==20&&sizeof(tSOC)==8&&sizeof(tCorner)==28&&offsetof(tCorner,section)==20?1:-1];
enum {MODE,COUNT,SECTION,GEOMETRY,WIDTH_BITS,SPEED,DESIRED_BITS,PX,PZ,PERIOD,CORNER_COUNT,EXTENT,RADIUS,CORNER_INDEX,ALIASES,MUTATE,MUTATION_AT,RNG,RNG_STEP,CHEATING,TRAIL,DIRECTION,SEED,RESERVED0,RESERVED1,RESERVED2,RESERVED3,RESERVED4,RESERVED5,RESERVED6,RESERVED7,RESERVED8};
static void event(unsigned code,unsigned value){unsigned head[4];if(event_count++>100)exit(80);head[0]=case_index;head[1]=code;head[2]=value;head[3]=step;fwrite(head,sizeof(head),1,stdout);fwrite(message,sizeof(message),1,stdout);fwrite(&gFrame_period_for_this_munging,4,1,stdout);fwrite(&gProgram_state,sizeof(gProgram_state),1,stdout);fwrite(heap+0x200000,0x20000,1,stdout);}
static void mutate(void){if(rng_count-1!=h[MUTATION_AT])return;if(h[MUTATE]&1)gProgram_state.AI_vehicles.path_nodes=OBJ(tPath_node,0xc100);if(h[MUTATE]&2)gProgram_state.AI_vehicles.path_sections=OBJ(tPath_section,0xa100);if(h[MUTATE]&4)gFrame_period_for_this_munging=h[RESERVED0];if(h[MUTATE]&8)OPP->nnext_sections=3;if(h[MUTATE]&16)CORNERS[0].field_0x08=-1.f;}
int __cdecl ControlledRand(void){unsigned value=(h[RNG]+rng_count*h[RNG_STEP])&32767u;rng_count++;event(1,value);mutate();return (int)value;}
void __fastcall DebuggerBoundary(const char*text){memset(message,0,sizeof(message));strncpy(message,text,sizeof(message)-1);event(2,0);}
void C2_HOOK_FASTCALL PDEnterDebugger(const char*text){DebuggerBoundary(text);}
static unsigned invoke(unsigned mode,unsigned x,unsigned y,unsigned n,unsigned a,unsigned b,unsigned c,unsigned d,unsigned e){builder_mode=mode;builder_address=addresses[mode];builder_x=x;builder_y=y;builder_stack_count=n;builder_stack[0]=a;builder_stack[1]=b;builder_stack[2]=c;builder_stack[3]=d;builder_stack[4]=e;if(BuilderInvoke())exit(14);event(0,builder_result);step++;return builder_result;}
static void setup(void){unsigned i;tPath_section*section;rng_count=event_count=step=0;memset(message,0,sizeof(message));memset(&gProgram_state,0xa5,sizeof(gProgram_state));gProgram_state.AI_vehicles.path_sections=OBJ(tPath_section,0x9100);gProgram_state.AI_vehicles.path_nodes=OBJ(tPath_node,0xb100);gFrame_period_for_this_munging=h[PERIOD];OPP->nnext_sections=(int)h[COUNT];OPP->car_spec=CAR;OPP->cheating=!!h[CHEATING];OPP->pursue_car_data.pursuee=PURSUEE;PURSUEE->my_trail.number_of_nodes=(tU8)h[TRAIL];strcpy(CAR->driver_name,"Navigation proof");for(i=0;i<12;i++){OPP->next_sections[i].section_no=(tS16)i;OPP->next_sections[i].direction=(tU8)h[DIRECTION];}
for(i=0;i<24;i++){section=&gProgram_state.AI_vehicles.path_sections[i];section->type=0;section->one_way=0;}
OPP->pursue_car_data.direct_line_nodes[0].pos=gProgram_state.AI_vehicles.path_nodes[0].pos;OPP->pursue_car_data.direct_line_nodes[1].pos=gProgram_state.AI_vehicles.path_nodes[1].pos;memcpy(&OPP->pursue_car_data.direct_line_section.width,&h[WIDTH_BITS],4);OPP->pursue_car_data.direct_line_section.max_speed[0]=61;OPP->pursue_car_data.direct_line_section.max_speed[1]=79;
for(i=0;i<4;i++){CORNERS[i].section=(unsigned)(h[SECTION]+h[CORNER_INDEX]+i);memcpy(&CORNERS[i].field_0x08,&h[RADIUS],4);memcpy(&CORNERS[i].field_0x0c,&h[EXTENT],4);CORNERS[i].field_0x18=(unsigned)(9+i);}
for(i=0;i<10;i++){SOCS[i].field_0x0=i%2?-1:0;SOCS[i].field_0x4=i%2?(int)h[SECTION]+(int)i/2:(int)i/2;}
memcpy(START->v,h+PZ,4);memcpy(&START->v[1],h+PX,4);memcpy(DESIRED,h+DESIRED_BITS,4);memcpy(WIDTH,h+WIDTH_BITS,4);builder_poison=0xcccccccc;
}
static int run(const char*path){FILE*f;unsigned cc,sc,section;unsigned start,finish,width,desired,corners,socs;SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);f=fopen(path,"rb");if(!f)return 10;heap=(unsigned char*)VirtualAlloc((void*)0x20000000,0x300000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(heap!=(void*)0x20000000)return 11;_setmode(_fileno(stdout),_O_BINARY);for(case_index=0;fread(h,sizeof(h),1,f)==1;case_index++){if(fread(heap+0x200000,0x20000,1,f)!=1)return 12;setup();_controlfp(_PC_53,_MCW_PC);_clearfp();start=(unsigned)START;finish=(unsigned)FINISH;width=(unsigned)WIDTH;desired=(unsigned)DESIRED;corners=(unsigned)CORNERS;socs=(unsigned)SOCS;if(h[ALIASES]==1)finish=start;if(h[ALIASES]==2)width=start;if(h[ALIASES]==3)start=(unsigned)&gProgram_state.AI_vehicles.path_nodes[0].pos.v[2];if(h[ALIASES]==4)desired=(unsigned)&CORNERS[0].field_0x08;if(h[ALIASES]==5)socs=corners;if(h[ALIASES]==6)corners=(unsigned)START;
if(h[ALIASES]==7)desired=(unsigned)&gFrame_period_for_this_munging;
if(h[ALIASES]==8)desired=start;
if(h[ALIASES]==9)desired=(unsigned)&CORNERS[0].field_0x0c;
if(h[ALIASES]==10)width=finish;
if(h[ALIASES]==11)socs=(unsigned)OPP->next_sections;
if(h[MODE]==0)invoke(0,start,finish,3,width,h[SECTION],(unsigned)OPP,0,0);
else if(h[MODE]==1)invoke(1,corners,h[SECTION],3,h[WIDTH_BITS],start,(unsigned)OPP,0,0);
else if(h[MODE]==2)invoke(2,socs,h[CORNER_COUNT],5,desired,h[SPEED],start,corners,(unsigned)OPP);
else if(h[MODE]==3)invoke(3,h[SECTION],h[CORNER_COUNT],4,corners,(unsigned)OPP,socs,(unsigned)CAR,0);
else if(h[MODE]==4){section=invoke(0,start,finish,3,width,h[SECTION],(unsigned)OPP,0,0);cc=invoke(1,corners,section,3,*(unsigned*)WIDTH,start,(unsigned)OPP,0,0);sc=invoke(3,h[SECTION],cc,4,corners,(unsigned)OPP,socs,(unsigned)CAR,0);invoke(2,socs,sc,5,desired,h[SPEED],start,corners,(unsigned)OPP);}else return 13;}fclose(f);return 0;}
'''
 src=SOURCE.read_text();opponent=(SOURCE.parent/'30-opponent.c').read_text();native=common+'\nint C2_HOOK_CDECL rand(void){return ControlledRand();}\n'
 native+='\n'.join(extract(src,name) for name in HELPERS+[e[0] for e in ENTRIES[:4]])
 native+='\n'+'\n'.join(extract(opponent,e[0]) for e in ENTRIES[5:-1])
 native+='\n'+extract((SOURCE.parent/'41-utility.c').read_text(),'IRandomBetween')
 native+='\nstatic volatile unsigned native_bodies[4];\n'
 for i,n in enumerate((3,3,5,4)):
  args=','.join(['unsigned x','unsigned y']+[f'unsigned a{j}' for j in range(n)]);types=','.join('unsigned' for j in range(n+2));values=','.join(['x','y']+[f'a{j}' for j in range(n)])
  native+=f'unsigned __fastcall adapter{i}({args}){{typedef '+('void' if i==2 else 'int')+f' (C2_HOOK_FASTCALL *body_fn)({types});'+('' if i==2 else 'return (unsigned)')+f'((body_fn)(size_t)native_bodies[{i}])({values});'+('return 0;' if i==2 else '')+'}\n'
 native+='\nint C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;'+''.join(f'native_bodies[{i}]=(unsigned){e[0]};addresses[{i}]=(unsigned)adapter{i};' for i,e in enumerate(ENTRIES[:4]))+'return run(argv[1]);}\n'
 physical=common+r'''
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned char*disk,*image;unsigned n,i,value,relative,hdr[7],*patch;IMAGE_NT_HEADERS*nt;IMAGE_SECTION_HEADER*sec;if(argc!=4)return 1;f=fopen(argv[2],"rb");if(!f)return 2;fseek(f,0,SEEK_END);n=(unsigned)ftell(f);rewind(f);disk=(unsigned char*)malloc(n);if(!disk||fread(disk,n,1,f)!=1)return 3;fclose(f);nt=(IMAGE_NT_HEADERS*)(disk+((IMAGE_DOS_HEADER*)disk)->e_lfanew);image=(unsigned char*)VirtualAlloc(NULL,nt->OptionalHeader.SizeOfImage,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image)return 4;memcpy(image,disk,nt->OptionalHeader.SizeOfHeaders);sec=IMAGE_FIRST_SECTION(nt);for(i=0;i<nt->FileHeader.NumberOfSections;i++)memcpy(image+sec[i].VirtualAddress,disk+sec[i].PointerToRawData,sec[i].SizeOfRawData);f=fopen(argv[3],"rb");if(!f||fread(hdr,sizeof(hdr),1,f)!=1)return 5;n=hdr[6];patch=(unsigned*)malloc(n*16);if(n&&(!patch||fread(patch,n*16,1,f)!=1))return 6;fclose(f);for(i=0;i<n;i++){if(patch[i*4]+4>nt->OptionalHeader.SizeOfImage)return 7;memcpy(&value,image+patch[i*4],4);if(value!=patch[i*4+1])return 8;value=patch[i*4+3]?patch[i*4+2]:(unsigned)image+patch[i*4+2];memcpy(image+patch[i*4],&value,4);}for(i=0;i<4;i++)addresses[i]=(unsigned)image+hdr[i];image[hdr[4]]=0xe9;relative=(unsigned)DebuggerBoundary-((unsigned)image+hdr[4]+5);memcpy(image+hdr[4]+1,&relative,4);image[hdr[5]]=0xe9;relative=(unsigned)ControlledRand-((unsigned)image+hdr[5]+5);memcpy(image+hdr[5]+1,&relative,4);free(patch);free(disk);return run(argv[1]);}
'''
 (out/'caller.c').write_text(caller);(out/'native.c').write_text(native);(out/'physical.c').write_text(physical)
 m5=ROOT/'build-tools/msvc500sp3';env5=dict(os.environ);env5['PATH']=str(m5/'bin')+os.pathsep+str(m5/'redist')+os.pathsep+env5['PATH'];env5['INCLUDE']=str(m5/'include');env5['LIB']=str(m5/'lib');msroot=ROOT/'build-tools/msvc-modern';ms=sorted((msroot/'VC/Tools/MSVC').iterdir())[-1];sdk=msroot/'Windows Kits/10';v=sorted((sdk/'Include').iterdir())[-1].name;env=dict(os.environ);env['PATH']=str(ms/'bin/Hostx64/x86')+os.pathsep+env['PATH'];env['INCLUDE']=os.pathsep.join(map(str,[ms/'include',*[sdk/f'Include/{v}/{x}' for x in ('ucrt','shared','um')]]));env['LIB']=os.pathsep.join(map(str,[ms/'lib/x86',*[sdk/f'Lib/{v}/{x}/x86' for x in ('ucrt','um')]]));inc=[ROOT/x for x in ('src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/brender/include','src/carma2/s3/include','src/carma2/platform')];gcc=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'
 commands={'caller':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Zl','/c',str(out/'caller.c'),'/Fo'+str(out/'caller.obj')],'msvc5':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(x) for x in inc],str(out/'native.c'),str(out/'caller.obj'),'/Fo'+str(out/'msvc5.obj'),'/Fe'+str(out/'msvc5.exe')],'msvc':[str(ms/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(x) for x in inc],str(out/'native.c'),str(out/'caller.obj'),'/Fo'+str(out/'msvc.obj'),'/Fe'+str(out/'msvc.exe')],'mingw':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation',*['-I'+str(x) for x in inc],str(out/'native.c'),str(out/'caller.obj'),'-o',str(out/'mingw.exe')],'physical':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-DREC2_MATCHING',*['-I'+str(x) for x in inc],str(out/'physical.c'),str(out/'caller.obj'),'-o',str(out/'physical.exe')]}
 for name,cmd in commands.items():
  r=subprocess.run(cmd,cwd=out,env=env5 if name in ('caller','msvc5') else env if name=='msvc' else None,capture_output=True,text=True);(out/f'{name}-build.txt').write_text(r.stdout+r.stderr)
  if r.returncode:raise RuntimeError(r.stdout+r.stderr)
 cases=fixtures()
 if a.case_index:cases=[cases[i] for i in a.case_index]
 fixture=out/'cases.bin';fixture.write_bytes(b''.join(make_case(c) for c in cases));records=[];failures=[]
 def execute(name,cmd):
  with (out/f'{name}.bin').open('wb') as f:r=subprocess.run(cmd,stdout=f,stderr=subprocess.PIPE,timeout=120)
  if r.returncode:raise RuntimeError(f'{name} returned {r.returncode}, {r.stderr!r}')
 execute('original',[str(out/'physical.exe'),str(fixture),str(a.original.resolve()),str(out/'original-addresses.bin')])
 def compare(name):
  with (out/'original.bin').open('rb') as expected,(out/f'{name}.bin').open('rb') as actual:
   index=0
   while True:
    e=expected.read(RECORD_BYTES);v=actual.read(RECORD_BYTES)
    if not e and not v:break
    assert len(e)==len(v)==RECORD_BYTES,'Record framing mismatch'
    if e!=v:
     first=next(i for i,(x,y) in enumerate(zip(e,v)) if x!=y);case_index=struct.unpack_from('<I',e)[0];failure=dict(compiler=name,case_index=case_index,case=cases[case_index],record=index,record_byte=first,arena_byte=first-(16+128+4+0x5ed0),original=e[max(0,first-12):first+20].hex(),native=v[max(0,first-12):first+20].hex());failures.append(failure);(out/f'{name}-failure.json').write_text(json.dumps(failure,indent=2));break
    index+=1
  records.append(dict(compiler=name,scenarios=len(cases),records=index,match=not any(f['compiler']==name for f in failures),output_sha256=sha((out/f'{name}.bin').read_bytes())))
 for name in ('msvc5','msvc','mingw'):execute(name,[str(out/f'{name}.exe'),str(fixture)]);compare(name)
 linked=None
 if a.symbols:
  data=a.recompiled.read_bytes();mapping=json.loads(a.symbols.read_text());assert mapping['format']==1 and mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(data)
  symbols={s['orig']:s['recomp'] for s in mapping['symbols']};map_path=a.recompiled.with_suffix('.map');map_bytes=map_path.read_bytes()
  functions={int(address,16):(name,owner) for name,address,owner in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',map_bytes.decode('latin1'),re.MULTILINE)}
  def unique(name):
   found=[address for address,(n,owner) in functions.items() if n==name];assert len(found)==1,(name,found);return found[0]
  debug=symbols.get(0x51a7b0) or unique('@PDEnterDebugger@4');rand=symbols[0x576970];initial=[symbols[e[1]] for e in ENTRIES];names={symbols[e[1]]:e[0] for e in ENTRIES};entries=[];pending=list(initial);seen=set();runtime=[]
  while pending:
   start=pending.pop(0)
   if start in seen:continue
   seen.add(start);end=min(x for x in functions if x>start);name=names.get(start,functions[start][0]);entries.append((name,start,end-start))
   for ins in cs.disasm(raw(data,start,end-start),start):
    if ins.mnemonic=='call' or ins.mnemonic.startswith('j'):
     op=ins.operands[0];assert op.type==CS_OP_IMM,(name,hex(ins.address),ins.op_str)
     if start<=op.imm<end or op.imm in (debug,rand):continue
     assert op.imm in functions,(name,hex(ins.address),ins.op_str)
     symbol,owner=functions[op.imm]
     if op.imm not in initial:
      assert (owner.strip()=='26-oppoproc.c.obj' and symbol.lstrip('_') in HELPERS) or (symbol in ('_memcpy','_memmove','_memset') and owner.strip().startswith('LIBC:')),(name,symbol,owner)
      runtime.append(dict(address=op.imm,symbol=symbol,owner=owner))
     pending.append(op.imm)
  by_address={v[1]:v for v in entries};entries=[by_address[x] for x in initial[:4]]+[v for v in entries if v[1] not in initial[:4]]
  globals_={name:(symbols[old],n) for name,(old,n) in GLOBALS.items()};linked=relocation(data,entries,globals_,debug,rand);linked.update(recompiled_sha256=sha(data),symbols_sha256=sha(a.symbols.read_bytes()),companion_map_sha256=sha(map_bytes),real_runtime_dependencies=runtime)
  save(linked,out/'linked-addresses.bin');execute('physical-matching',[str(out/'physical.exe'),str(fixture),str(a.recompiled.resolve()),str(out/'linked-addresses.bin')]);compare('physical-matching')
 manifest=dict(scenarios=len(cases),comparisons=len(cases)*len(records),entries=ENTRIES[:4],original=original,linked=linked,input_sha256=sha(fixture.read_bytes()),original_output_sha256=sha((out/'original.bin').read_bytes()),records=records,failures=failures,commands=commands,checks=['PC53 physical x87 original','raw FASTCALL/RET12/RET20/RET16 and callee-saved/x87TOP','full globals/Programstate/arena at every ordered RNG/debug/final boundary','complete GetStraight/CalcCorners/CalcSOCs/SetMaxSpeed integration','unused SOC flag bits preserved','no output normalization'],limits=['CRT rand and debugger controlled only; exception-status flags not compared','non-x86 fallback arithmetic not verified'],sources={str(p.relative_to(ROOT)):sha(p.read_bytes()) for p in [SOURCE,SOURCE.with_suffix('.h'),SOURCE.parent/'08-loading1.c',SOURCE.parent/'08-loading1.h',SOURCE.parent/'30-opponent.c',SOURCE.parent/'30-opponent.h',SOURCE.parent/'41-utility.c',SOURCE.parent/'41-utility.h',ROOT/'src/carma2/include/rec2_types.h',ROOT/'src/carma2/hooks/c2_hooks.h',Path(__file__).resolve()]})
 manifest.update(original_sha256=sha(b),selected_case_indices=a.case_index,generated_files_sha256={name:sha((out/name).read_bytes()) for name in ('caller.c','caller.obj','native.c','physical.c','physical.exe','msvc5.exe','msvc.exe','mingw.exe')})
 (out/'verification.json').write_text(json.dumps(manifest,indent=2)+'\n')
 if failures:raise SystemExit(json.dumps(failures,indent=2))
 print(f'PASS {len(cases)} scenarios / {manifest["comparisons"]} comparisons')

def make_case(options):
 defaults=dict(mode=4,count=8,section=20000,geometry=1,width=fb(1.5),speed=fb(20),desired=fb(80),px=fb(0),pz=fb(0),period=100,corner_count=1,extent=fb(4),radius=fb(2),corner_index=2,aliases=0,mutate=0,mutation_at=0,rng=0,rng_step=8191,cheating=1,trail=13,direction=1,seed=41)
 defaults.update(options);h=[defaults.get(k,0)&0xffffffff for k in FIELDS];arena=bytearray((i*29+defaults['seed'])&255 for i in range(0x20000))
 # Fixed complete repository-layout graph arenas, plus a mutable alternate copy.
 nodes=[]
 for i in range(25):
  geom=defaults['geometry'];x=float(i*10);z=float((i//2)*10) if geom==1 else float((i%2)*10) if geom==2 else 0.0
  if geom==3:x=z=0.0
  if geom==4:x=float(i*10);z=float(i)*0.10000000149011612
  if geom==5:x=float(i*10);z=float(i//3)*0.10000000149011612
  nodes.append([fb(x),0,fb(z)])
 for index,value,axis in defaults.get('node_patches',[]):nodes[index][axis]=value
 for i,node in enumerate(nodes):
  struct.pack_into('<3I',arena,0xb100+i*32,*node)
  struct.pack_into('<3I',arena,0xc100+i*32,fb(i*10+3.5),0,fb((i%3)*5+.125))
 for i in range(24):
  struct.pack_into('<2H4B2I2B',arena,0x9100+i*20,i,i+1,0,0,(15+i*3)&255,(30+i*3)&255,defaults['width'] if 'widths' not in defaults else defaults['widths'][i%len(defaults['widths'])],0,0,0)
 arena[0xa100:0xa100+480]=arena[0x9100:0x9100+480]
 for i in range(24):struct.pack_into('<I',arena,0xa100+i*20+8,fb(0.5+i*.03125))
 for i in range(25):struct.pack_into('<3I',arena,0x3000+0x13f8+i*12,*nodes[i])
 # Direct-line mode has real points/width/maxspeed in the opponent structure.
 struct.pack_into('<3I',arena,0x100+0x140,*nodes[0]);struct.pack_into('<3I',arena,0x100+0x160,*nodes[1])
 return words(h)+arena

def fixtures():
 cases=[dict(mode=mode,geometry=g,count=n) for mode in (0,1,4) for g in range(6) for n in (4,8,12)]+[dict(mode=3),dict(mode=2,corner_count=4),dict(mode=2,corner_count=0)]
 cases += [dict(mode=mode,count=n,geometry=g) for mode in (0,1,4) for n in (1,2,3) for g in (0,1,2,3)]
 cases += [dict(mode=mode,geometry=g,direction=0) for mode in (0,1,4) for g in (0,1,2)]
 cases += [dict(mode=mode,section=s,geometry=g) for mode in (0,1,4) for s in (10000,15000,15006) for g in (0,1,2)]
 cases += [dict(mode=1,section=-1),dict(mode=0,section=0xabcd4e20),dict(mode=1,section=0xabcd4e20)]
 cases += [dict(mode=mode,geometry=2,widths=ws) for mode in (0,1,4) for ws in ([fb(1),fb(4),fb(.5)],[0,fb(1)],[fb(-1),fb(2)]) if mode!=4 or ws[0]!=fb(-1)]
 cases += [dict(mode=mode,aliases=alias,geometry=g) for mode in (0,1) for alias in (1,2,3,6) for g in (0,1,2)]
 cases += [dict(mode=2,aliases=alias,corner_count=4) for alias in (4,7)]
 cases += [dict(mode=3,aliases=5,corner_count=1,corner_index=0)]
 cases += [dict(mode=mode,mutate=mask,geometry=g,rng=rng) for mode in (1,4) for mask in (1,2,4,8,16) for g in (0,3) for rng in (0,32767)]
 cases += [dict(mode=2,period=period,speed=fb(speed),corner_count=count) for period in (0,1,100,10000,0x80000000,0xffffffff) for speed in (0,1,10,20,100) for count in (-1,0,1,4)]
 special=[0,0x80000000,1,0x80000001,0x7fffff,0x800000,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc12345,0xffc54321,0x7f800001,0xff812345]
 cases += [dict(mode=2,speed=x,desired=y,corner_count=4) for x in special for y in (fb(80),0x7fc12345,0x7f800001)]
 cases += [dict(mode=3,radius=x,extent=extent) for x in special for extent in (fb(0),fb(4),fb(20),0x7fc12345,0x7f800001)]
 cases += [dict(mode=mode,width=x,geometry=g) for mode in (0,1) for x in special for g in (0,1,2)]
 cases += [dict(mode=mode,node_patches=[(2,x,axis)],geometry=2) for mode in (0,1) for x in special for axis in (0,2)]
 cases += [dict(mode=mode,geometry=g,mutate=mask,mutation_at=at,rng=rng) for mode in (1,4) for g in (0,3) for mask in (1,2,3,7) for at in (0,1,3) for rng in (0,32767)]
 cases += [dict(mode=mode,node_patches=patches,geometry=2) for mode in (0,1) for patches in ([(1,0x7fc12345,0),(2,0xffc54321,0)],[(1,0x7f800001,0),(2,0x7fc54321,0)],[(1,0xff800001,2),(2,0x7f800002,2)])]
 cases += [dict(mode=mode,widths=widths,geometry=g) for mode in (0,1) for widths in ([0x7f800001,fb(2)],[fb(2),0x7f800001],[0xffc54321,0x7fc12345],[0x80000000,0]) for g in (0,2)]
 cases += [dict(mode=3,radius=fb(value),extent=fb(extent),corner_index=index) for value in (1e8,1e16,1e20,1e30) for extent in (0,4,30) for index in (0,1,3)]
 cases += [dict(mode=2,aliases=7,period=period,corner_count=4,speed=fb(100)) for period in (0x7fc12345,0x7f800001,0xff812345,0x7f7fffff)]
 cases += [dict(mode=mode,geometry=g,px=fb(px),pz=fb(pz)) for mode in (0,1,4) for g in (1,4,5) for px,pz in ((1e8,1e8),(16777217.0,16777219.0),(1e-20,1e-20),(-1.0,.10000000149011612))]
 cases += [dict(mode=2,aliases=alias,corner_count=count,speed=fb(100)) for alias in (8,9) for count in (1,4)]
 cases += [dict(mode=0,aliases=10,geometry=g) for g in (0,1,2)]
 cases += [dict(mode=3,aliases=11,corner_count=1,corner_index=0)]
 cases += [dict(mode=3,corner_count=1,count=12,corner_index=index,extent=fb(extent),direction=d) for index in (0,1,8,10,11) for extent in (0,4,50) for d in (0,1)]
 cases += [dict(mode=3,corner_count=count) for count in (-2147483648,-1,0)]
 return cases
if __name__=='__main__':main()
