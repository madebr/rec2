#!/usr/bin/env python3
"""Physical PC53 proof of full opponent nearest/search/route construction.

The six original bodies, real IRandomBetween and published route primitives run
physically. Only CRT integer rand and debugger are controlled. Full public
arenas/globals and ordered boundary snapshots are compared. Raw IEEE outputs
are unnormalized. Private stack route padding copied by Return/NearPlayer is
undefined in the original: poison identifies only those bytes, while direct
SearchForSection tests exact known padding and forward overlap copies.
"""
import argparse,hashlib,json,os,re,struct,subprocess
from pathlib import Path
from verify_tinted_native_reconstruction import extract
from verify_opponent_route_reconstruction import ROOT,HASH,GLOBALS as FOUNDATION_GLOBALS,pe,raw,sha,words

SOURCE=ROOT/'src/carma2/newcommon/30-opponent.c'
ENTRIES=[('FindNearestGeneralSection',0x4a82d0,1217),('TopUpRandomRoute',0x4a93c0,391),('CalcRaceRoute',0x4aae20,732),('SearchForSection',0x4ac180,950),('CalcReturnToStartPointRoute',0x4abf60,532),('NewCalcGetNearPlayerRoute',0x4ac9e0,1088),('IRandomBetween',0x513520,34),('DoNotDprintf_opponent',0x4a8160,1),('AddToOpponentsProjectedRoute',0x4ab150,50),('ShiftOpponentsProjectedRoute',0x4ab100,78)]
GLOBALS={'gProgram_state':FOUNDATION_GLOBALS['gProgram_state'],'gOpponents':(0x7634bc,4),'gTime_stamp_for_this_munging':(0x79efb8,4),'gShortest_route_length':(0x69171c,4),'gRoute_search_target_count':(0x69172c,4),'gRoute_search_last_target_count':(0x69174c,4),'gBit_per_node':(0x69175c,4),'gRoute_search_calls':(0x69176c,4),'gRoute_search_attempts':(0x691770,4),'gRoute_search_total_calls':(0x691774,4),'gRoute_search_max_calls':(0x691778,4)}
HELPERS=['OpponentFloatDifference','OpponentFloatAdd','OpponentAdd','OpponentSubtract','OpponentMultiply','OpponentDivide','OpponentSqrt','OpponentScalarRound','OpponentHybridLengthSquared']
FIELDS='mode count add target depth length force now expiry cached last_target strength velocity aliases graph cheating flags mutate mutation_at rng rng_step px py pz trail seed car_dx car_dy car_dz reserved0 reserved1 reserved2'.split()
RECORD_BYTES=16+128+len(GLOBALS)*4+0x5ed0+0x20000
ARENA_RECORD_OFFSET=16+128+len(GLOBALS)*4+0x5ed0
def fb(x):return struct.unpack('<I',struct.pack('<f',x))[0]

def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE');p.add_argument('--output',type=Path,default=ROOT/'build-audit/opponent-route-builder-tests');p.add_argument('--symbols',type=Path);p.add_argument('--recompiled',type=Path);p.add_argument('--case-index',type=int,action='append');a=p.parse_args()
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
  return dict(base=base,size=size,refs=refs,closure=closure,entries=[v[1]-base for v in entries[:6]],debug=debug-base,rand=rand-base)
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
 cmp builder_mode,0
 jne notshort
 movsx eax,ax
 notshort:
 cmp builder_mode,0
 je resultready
 cmp builder_mode,3
 je resultready
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
#include "30-opponent.h"
#include "41-utility.h"
#include "08-loading1.h"
#include "globvars.h"
#include "platform.h"
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison;extern unsigned __cdecl BuilderInvoke(void);
static unsigned addresses[6],h[32],case_index,rng_count,event_count;static unsigned char*heap;static char message[128];
#define OBJ(type,off) ((type*)(heap+0x200000+(off)))
#define OPP OBJ(tOpponent_spec,0x100)
#define CAR OBJ(tCar_spec,0x1000)
#define PURSUEE OBJ(tCar_spec,0x3000)
#define ACTOR OBJ(br_actor,0x6000)
#define PLAYER_ACTOR OBJ(br_actor,0x6100)
#define PHYS OBJ(tPhysics_object,0x6500)
#define ROUTE OBJ(tRoute_section,0xd200)
#define BEST OBJ(tRoute_section,0xd300)
#define BEST_COUNT OBJ(int,0xd400)
typedef char layouts[sizeof(void*)==4&&sizeof(tProgram_state)==0x5ed0&&sizeof(tOpponent_spec)==420&&sizeof(tPath_node)==32&&sizeof(tPath_section)==20&&sizeof(tRoute_section)==4&&sizeof(tOpponent)==448&&offsetof(tCar_spec,my_trail)==0x13f8&&offsetof(tCar_spec,field_0x1540)==0x1540&&offsetof(tCar_spec,field_0x155c)==0x155c&&offsetof(tPhysics_object,v)==0x68?1:-1];
'''
 for name,(addr,n) in GLOBALS.items():
  typ={'gProgram_state':'tProgram_state','gOpponents':'tOpponent*','gBit_per_node':'tU8*','gTime_stamp_for_this_munging':'tU32','gShortest_route_length':'float'}.get(name,'int');common+=f'#define {name} (*({typ}*)(heap+0x{addr-0x650000:x}))\n'
 common+='enum {'+','.join(x.upper() for x in FIELDS)+'};\n'
 common+=r'''
static void event(unsigned code,unsigned value){unsigned head[4],g[GLOBAL_COUNT];unsigned i;if(event_count++>500)exit(80);head[0]=case_index;head[1]=code;head[2]=value;head[3]=rng_count;fwrite(head,sizeof(head),1,stdout);fwrite(message,sizeof(message),1,stdout);
GLOBAL_SNAPSHOT
fwrite(g,sizeof(g),1,stdout);fwrite(&gProgram_state,sizeof(gProgram_state),1,stdout);fwrite(heap+0x200000,0x20000,1,stdout);(void)i;}
static void mutate(void){if(rng_count-1!=h[MUTATION_AT])return;if(h[MUTATE]&1)gProgram_state.AI_vehicles.path_sections=OBJ(tPath_section,0xa100);if(h[MUTATE]&2)gProgram_state.AI_vehicles.path_nodes=OBJ(tPath_node,0xc100);if(h[MUTATE]&4)OPP->cheating=!OPP->cheating;if(h[MUTATE]&8)gTime_stamp_for_this_munging+=250;if(h[MUTATE]&16)OPP->nnext_sections=11;}
int __cdecl ControlledRand(void){unsigned value=(h[RNG]+rng_count*h[RNG_STEP])&32767u;rng_count++;event(1,value);mutate();return (int)value;}
void __fastcall DebuggerBoundary(const char*text){memset(message,0,sizeof(message));strncpy(message,text,sizeof(message)-1);event(2,0);if(h[MUTATE]&32)OPP->nnext_sections=2;}
void C2_HOOK_FASTCALL PDEnterDebugger(const char*text){DebuggerBoundary(text);}
static void setup(void){unsigned i,off;int j;float one=1.f;
    rng_count=event_count=0;memset(message,0,sizeof(message));memset(&gProgram_state,0xa5,sizeof(gProgram_state));gProgram_state.AI_vehicles.number_of_path_nodes=24;gProgram_state.AI_vehicles.number_of_path_sections=h[GRAPH]==5?15:h[GRAPH]==1?8:h[GRAPH]==2?7:h[GRAPH]==3?8:h[GRAPH]==4?1:12;gProgram_state.AI_vehicles.path_sections=OBJ(tPath_section,0x9100);gProgram_state.AI_vehicles.path_nodes=OBJ(tPath_node,0xb100);gOpponents=OBJ(tOpponent,0x7000);gOpponents[0].strength_rating=h[STRENGTH];gBit_per_node=OBJ(tU8,0xd100);gTime_stamp_for_this_munging=h[NOW];gShortest_route_length=99.f;gRoute_search_target_count=7;gRoute_search_last_target_count=13;gRoute_search_calls=3;gRoute_search_attempts=5;gRoute_search_total_calls=17;gRoute_search_max_calls=9;
    OPP->index=0;OPP->nnext_sections=(int)h[COUNT];OPP->car_spec=CAR;OPP->cheating=!!h[CHEATING];OPP->complete_race_data.finished_calcing_race_route=(tU8)(h[FLAGS]&1);OPP->complete_race_data.found_race_section=(tU8)((h[FLAGS]>>1)&1);OPP->players_section_when_last_calced_full_path=(tS16)h[LAST_TARGET];OPP->return_to_start_data.section_no=(tS16)h[TARGET];OPP->pursue_car_data.pursuee=PURSUEE;CAR->collision_info=PHYS;CAR->car_master_actor=ACTOR;PURSUEE->car_master_actor=PLAYER_ACTOR;PURSUEE->field_0x155c=(tS16)h[CACHED];PURSUEE->field_0x1540=h[EXPIRY];memcpy(&PHYS->v.v[0],&h[VELOCITY],4);PHYS->v.v[1]=0;PHYS->v.v[2]=0;memcpy(&CAR->direction.v[0],&h[CAR_DX],4);memcpy(&CAR->direction.v[1],&h[CAR_DY],4);memcpy(&CAR->direction.v[2],&h[CAR_DZ],4);memset(CAR->driver_name,0,32);strcpy(CAR->driver_name,"Route builder proof");
    memset(&ACTOR->t.t.mat,0,sizeof(br_matrix34));ACTOR->t.t.mat.m[0][0]=ACTOR->t.t.mat.m[1][1]=ACTOR->t.t.mat.m[2][2]=one;memcpy(ACTOR->t.t.mat.m[3],h+PX,12);PLAYER_ACTOR->t.t.mat=ACTOR->t.t.mat;PLAYER_ACTOR->t.t.mat.m[3][0]+=3.f;
    for(i=0;i<12;i++){OPP->next_sections[i].section_no=(tS16)(i%4);OPP->next_sections[i].direction=(tU8)(h[RESERVED0]&0x10000?(h[RESERVED0]>>8)&255:1);OPP->next_sections[i].field_0x3=(tU8)(0x80+i);}
    for(i=0;i<10;i++){ROUTE[i].section_no=(tS16)(i%4);ROUTE[i].direction=(tU8)(h[RESERVED0]&0x10000?(h[RESERVED0]>>8)&255:1);ROUTE[i].field_0x3=(tU8)(0x30+i);BEST[i].section_no=-2;BEST[i].direction=0xff;BEST[i].field_0x3=0xab;}*BEST_COUNT=123;memcpy(OBJ(br_vector3,0xd500),h+PX,12);CAR->my_trail.number_of_nodes=(tU8)h[TRAIL];for(j=-1;j<26;j++){br_vector3*node=OBJ(br_vector3,0x1000+0x13f8+j*12);node->v[0]=(float)j;node->v[1]=0;node->v[2]=(float)(j%3);}
    if(h[FLAGS]&0x100)gProgram_state.AI_vehicles.path_nodes=NULL;if(h[FLAGS]&0x200)gProgram_state.AI_vehicles.path_sections=NULL;if(h[FLAGS]&0x400){gProgram_state.AI_vehicles.number_of_path_sections=0;gProgram_state.AI_vehicles.path_nodes=NULL;gProgram_state.AI_vehicles.path_sections=NULL;}if(h[FLAGS]&0x800)gBit_per_node=NULL;
    builder_poison=h[RESERVED2]?h[RESERVED2]:0xcccccccc;builder_x=(unsigned)OPP;builder_y=h[ADD];builder_stack_count=0;memset(builder_stack,0,sizeof(builder_stack));
    if(h[MODE]==0){builder_x=h[FLAGS]&4?(unsigned)CAR:0;builder_y=(unsigned)OBJ(br_vector3,0xd500);builder_stack_count=3;builder_stack[0]=(unsigned)OBJ(br_vector3,0xd510);builder_stack[1]=(unsigned)OBJ(br_vector3,0xd520);builder_stack[2]=(unsigned)OBJ(float,0xd530);if(h[ALIASES]==1)builder_stack[0]=builder_y;if(h[ALIASES]==2)builder_stack[1]=builder_y;if(h[ALIASES]==3)builder_stack[0]=builder_stack[1];if(h[ALIASES]==4)builder_stack[2]=builder_y;if(h[ALIASES]==5)builder_stack[1]=(unsigned)&gProgram_state.AI_vehicles.path_nodes[0].pos;}
    if(h[MODE]==3){builder_x=(unsigned)ROUTE;builder_y=(unsigned)BEST;builder_stack_count=5;builder_stack[0]=(unsigned)BEST_COUNT;builder_stack[1]=h[TARGET];builder_stack[2]=h[DEPTH];builder_stack[3]=h[LENGTH];builder_stack[4]=(unsigned)OPP;if(h[ALIASES]==1)builder_y=builder_x;if(h[ALIASES]==2)builder_y=builder_x+4;if(h[ALIASES]==3)builder_y=builder_x-4;if(h[ALIASES]==4)builder_stack[0]=builder_x+8;}
    if(h[MODE]==5){builder_y=(unsigned)PURSUEE;builder_stack_count=1;builder_stack[0]=h[FORCE];}
    for(i=0;i<2;i++){off=i?0xc100:0xb100;for(j=-1;j<24;j++){tPath_node*node=OBJ(tPath_node,off+j*32);if(h[RESERVED0]&1)node->pos.v[1]=(float)j*0.125f;}}
}
static int run(const char*path){FILE*f;SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);f=fopen(path,"rb");if(!f)return 10;heap=(unsigned char*)VirtualAlloc((void*)0x20000000,0x300000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(heap!=(void*)0x20000000)return 11;_setmode(_fileno(stdout),_O_BINARY);for(case_index=0;fread(h,sizeof(h),1,f)==1;case_index++){if(fread(heap+0x200000,0x20000,1,f)!=1)return 12;if(h[MODE]>=6)return 13;setup();builder_mode=h[MODE];builder_address=addresses[h[MODE]];builder_result=0;_controlfp(_PC_53,_MCW_PC);_clearfp();if(BuilderInvoke())return 14;event(0,builder_result);}fclose(f);return 0;}
'''
 common=common.replace('GLOBAL_COUNT',str(len(GLOBALS))).replace('GLOBAL_SNAPSHOT','\n'.join(f'memcpy(&g[{i}],&{name},4);' for i,name in enumerate(GLOBALS)))
 src=SOURCE.read_text();native=common+'\nint C2_HOOK_CDECL rand(void){return ControlledRand();}\n'+'\n'.join(extract(src,name) for name in HELPERS+[e[0] for e in ENTRIES if e[0]!='IRandomBetween'])+'\n'+extract((SOURCE.parent/'41-utility.c').read_text(),'IRandomBetween')
 native+=r'''
unsigned __fastcall adapter0(unsigned x,unsigned y,unsigned direction,unsigned intersection,unsigned distance){return (unsigned)(int)FindNearestGeneralSection((tCar_spec*)x,(br_vector3*)y,(br_vector3*)direction,(br_vector3*)intersection,(float*)distance);}
unsigned __fastcall adapter1(unsigned x,unsigned y){TopUpRandomRoute((tOpponent_spec*)x,(int)y);return 0;}
unsigned __fastcall adapter2(unsigned x,unsigned y){(void)y;CalcRaceRoute((tOpponent_spec*)x);return 0;}
static volatile unsigned search_body_address;
unsigned __fastcall adapter3(unsigned x,unsigned y,unsigned best_count,unsigned target,unsigned depth,unsigned length,unsigned opponent){typedef int (C2_HOOK_FASTCALL *raw_fn)(tRoute_section*,tRoute_section*,int*,tS16,int,unsigned,tOpponent_spec*);return ((raw_fn)(size_t)search_body_address)((tRoute_section*)x,(tRoute_section*)y,(int*)best_count,(tS16)target,(int)depth,length,(tOpponent_spec*)opponent);}
unsigned __fastcall adapter4(unsigned x,unsigned y){(void)y;CalcReturnToStartPointRoute((tOpponent_spec*)x);return 0;}
unsigned __fastcall adapter5(unsigned x,unsigned y,unsigned force){NewCalcGetNearPlayerRoute((tOpponent_spec*)x,(tCar_spec*)y,(int)force);return 0;}
'''
 native+='int C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;search_body_address=(unsigned)SearchForSection;'+''.join(f'addresses[{i}]=(unsigned)adapter{i};' for i in range(6))+'return run(argv[1]);}\n'
 physical=common+r'''
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned char*disk,*image;unsigned n,i,value,relative,hdr[9],*patch;IMAGE_NT_HEADERS*nt;IMAGE_SECTION_HEADER*sec;if(argc!=4)return 1;f=fopen(argv[2],"rb");if(!f)return 2;fseek(f,0,SEEK_END);n=(unsigned)ftell(f);rewind(f);disk=(unsigned char*)malloc(n);if(!disk||fread(disk,n,1,f)!=1)return 3;fclose(f);nt=(IMAGE_NT_HEADERS*)(disk+((IMAGE_DOS_HEADER*)disk)->e_lfanew);image=(unsigned char*)VirtualAlloc(NULL,nt->OptionalHeader.SizeOfImage,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image)return 4;memcpy(image,disk,nt->OptionalHeader.SizeOfHeaders);sec=IMAGE_FIRST_SECTION(nt);for(i=0;i<nt->FileHeader.NumberOfSections;i++)memcpy(image+sec[i].VirtualAddress,disk+sec[i].PointerToRawData,sec[i].SizeOfRawData);f=fopen(argv[3],"rb");if(!f||fread(hdr,sizeof(hdr),1,f)!=1)return 5;n=hdr[8];patch=(unsigned*)malloc(n*16);if(n&&(!patch||fread(patch,n*16,1,f)!=1))return 6;fclose(f);for(i=0;i<n;i++){if(patch[i*4]+4>nt->OptionalHeader.SizeOfImage)return 7;memcpy(&value,image+patch[i*4],4);if(value!=patch[i*4+1])return 8;value=patch[i*4+3]?patch[i*4+2]:(unsigned)image+patch[i*4+2];memcpy(image+patch[i*4],&value,4);}for(i=0;i<6;i++)addresses[i]=(unsigned)image+hdr[i];image[hdr[6]]=0xe9;relative=(unsigned)DebuggerBoundary-((unsigned)image+hdr[6]+5);memcpy(image+hdr[6]+1,&relative,4);image[hdr[7]]=0xe9;relative=(unsigned)ControlledRand-((unsigned)image+hdr[7]+5);memcpy(image+hdr[7]+1,&relative,4);free(patch);free(disk);return run(argv[1]);}
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
  private_bytes=0
  with (out/'original.bin').open('rb') as e,(out/f'{name}.bin').open('rb') as f:
   index=0
   while True:
    expected=e.read(RECORD_BYTES);actual=f.read(RECORD_BYTES)
    if not expected and not actual:break
    assert len(expected)==len(actual)==RECORD_BYTES,'Record framing differs'
    case_index,event_,value,rng_count=struct.unpack_from('<4I',expected);case=cases[case_index];mask=[]
    if case['mode'] in (4,5):
     for slot in range(12):
      off=ARENA_RECORD_OFFSET+0x100+0x80+4*slot+3
      poison=case.get('reserved2',0) or 0xcccccccc
      if expected[off]==(poison>>24):mask.append(off)
    actual=bytearray(actual)
    for off in mask:actual[off]=expected[off]
    private_bytes+=len(mask)
    if actual!=expected:
     first=next(i for i,(x,y) in enumerate(zip(actual,expected)) if x!=y);failure=dict(compiler=name,case_index=case_index,case=case,event=event_,rng_count=rng_count,record=index,record_byte=first,original_bytes=expected[max(0,first-12):first+20].hex(),native_bytes=actual[max(0,first-12):first+20].hex());failures.append(failure);(out/f'{name}-failure.json').write_text(json.dumps(failure,indent=2)+'\n');break
    index+=1
  records.append(dict(compiler=name,scenarios=len(cases),records=index,match=not any(x['compiler']==name for x in failures),unknown_private_padding_bytes=private_bytes,output_sha256=sha((out/f'{name}.bin').read_bytes())))
 for name in ('msvc5','msvc','mingw'):execute(name,[str(out/f'{name}.exe'),str(fixture)]);compare(name)
 linked=None
 if a.symbols:
  data=a.recompiled.read_bytes();mapping=json.loads(a.symbols.read_text());assert mapping['format']==1 and mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(data)
  symbols={s['orig']:s['recomp'] for s in mapping['symbols']};map_path=a.recompiled.with_suffix('.map');map_bytes=map_path.read_bytes()
  # MSVC5 maps contain ANSI-decorated string symbols; latin1 is lossless.
  functions={int(address,16):(name,owner) for name,address,owner in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',map_bytes.decode('latin1'),re.MULTILINE)}
  def unique(name):
   found=[address for address,(n,owner) in functions.items() if n==name];assert len(found)==1,(name,found);return found[0]
  debug=symbols.get(0x51a7b0) or unique('@PDEnterDebugger@4');rand=symbols[0x576970];initial=[symbols[e[1]] for e in ENTRIES]
  names={symbols[e[1]]:e[0] for e in ENTRIES};entries=[];pending=list(initial);seen=set();runtime=[]
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
      assert (owner.strip()=='30-opponent.c.obj' and symbol.lstrip('_') in HELPERS) or (symbol in ('_memcpy','_memmove','_memset') and owner.strip().startswith('LIBC:')),(name,symbol,owner)
      runtime.append(dict(address=op.imm,symbol=symbol,owner=owner))
     pending.append(op.imm)
  # Preserve the first six public dispatcher entries in requested mode order.
  by_address={v[1]:v for v in entries};entries=[by_address[x] for x in initial[:6]]+[v for v in entries if v[1] not in initial[:6]]
  globals_={name:(symbols[old],n) for name,(old,n) in GLOBALS.items()};linked=relocation(data,entries,globals_,debug,rand);linked.update(recompiled_sha256=sha(data),symbols_sha256=sha(a.symbols.read_bytes()),companion_map_sha256=sha(map_bytes),controlled_debugger=debug,controlled_rand=rand,real_runtime_dependencies=runtime)
  save(linked,out/'linked-addresses.bin');execute('physical-matching',[str(out/'physical.exe'),str(fixture),str(a.recompiled.resolve()),str(out/'linked-addresses.bin')]);compare('physical-matching')
 faults=[]
 if not a.case_index:
  fault_cases=[dict(mode=0,flags=0x100),dict(mode=0,flags=0x200),dict(mode=0,flags=0x400),dict(mode=3,flags=0x800),dict(mode=5,flags=0x100,cached=-1)]
  for index,case in enumerate(fault_cases):
   fault_input=out/f'fault-{index}.bin';fault_input.write_bytes(make_case(case));commands_=[('original',[str(out/'physical.exe'),str(fault_input),str(a.original.resolve()),str(out/'original-addresses.bin')])]+[(name,[str(out/f'{name}.exe'),str(fault_input)]) for name in ('msvc5','msvc','mingw')]
   if a.symbols:commands_.append(('physical-matching',[str(out/'physical.exe'),str(fault_input),str(a.recompiled.resolve()),str(out/'linked-addresses.bin')]))
   for name,cmd in commands_:
    r=subprocess.run(cmd,capture_output=True,timeout=15);code=r.returncode&0xffffffff
    assert code==0xc0000005,(case,name,hex(code),len(r.stdout),r.stderr)
    # A cache refresh may emit a callback before faulting; CRT buffering can
    # leave its last record truncated. Fault probes claim process status only.
    faults.append(dict(case=case,compiler=name,status=hex(code),stdout_bytes=len(r.stdout),stdout_sha256=sha(r.stdout),fixture_sha256=sha(fault_input.read_bytes())))
 manifest=dict(scenarios=len(cases),comparisons=len(cases)*len(records),entry_scenario_counts={name:sum(c['mode']==i for c in cases) for i,(name,_,_) in enumerate(ENTRIES[:6])},cases=cases,selected_case_indices=a.case_index,expected_fault_scenarios=len(faults)//(5 if a.symbols else 4),expected_fault_comparisons=sum(x['compiler']!='original' for x in faults),expected_fault_checks=faults,checks=records,failures=failures,original_sha256=HASH,physical_original_output_sha256=sha((out/'original.bin').read_bytes()),source_sha256=sha(SOURCE.read_bytes()),header_sha256=sha((SOURCE.parent/'30-opponent.h').read_bytes()),loading_source_sha256=sha((SOURCE.parent/'08-loading1.c').read_bytes()),loading_header_sha256=sha((SOURCE.parent/'08-loading1.h').read_bytes()),verifier_sha256=sha(Path(__file__).read_bytes()),fixtures_sha256=sha(fixture.read_bytes()),caller_sha256=sha((out/'caller.obj').read_bytes()),original=original,linked=linked,compiler_commands=commands,precision_control=53,limitations=__doc__+' Fault probes compare access-violation process status only; they do not assert exact instruction address or post-fault memory. Unset projectionT probes use explicitly poisoned caller stack, not a universal private-stack value. X87 exception flags are outside the state comparison.')
 (out/'verification.json').write_text(json.dumps(manifest,indent=2)+'\n')
 if failures:raise AssertionError(f'Route builder mismatch: {failures[0]}; see {out}')
 print(f'PASS {len(cases)} physical original scenarios / {manifest["comparisons"]} native comparisons')

def make_case(c):
 d=dict(mode=0,count=1,add=-1,target=5,depth=1,length=fb(0),force=1,now=1000,expiry=1000,cached=5,last_target=5,strength=4,velocity=fb(20),aliases=0,graph=0,cheating=0,flags=0,mutate=0,mutation_at=0,rng=12345,rng_step=789,px=fb(0.2),py=fb(0),pz=fb(0.1),trail=10,seed=31,car_dx=fb(1),car_dy=fb(0),car_dz=fb(0),reserved0=0,reserved1=0,reserved2=0);d.update(c);arena=bytearray(b'\xa5'*0x20000)
 if 'direction' in c:d['reserved0']|=0x10000|((c['direction']&255)<<8)
 edges=[(i,i+1) for i in range(12)]
 if d['graph']==1:edges=[(0,1),(1,2),(1,3),(2,4),(3,4),(4,5),(5,6),(6,0)]
 if d['graph']==2:edges=[(0,1),(1,2),(2,3),(3,1),(1,4),(4,5),(5,2)]
 if d['graph']==3:edges=[(0,1),*[(1,i) for i in range(2,9)]]
 if d['graph']==4:edges=[(0,1)]
 if d['graph']==5:edges=[(i,i+1) for i in range(15)]
 for copy in range(2):
  sections=0x9100+copy*0x1000;nodes=0xb100+copy*0x1000
  for i in range(-1,24):
   off=nodes+i*32;struct.pack_into('<3f',arena,off,float(i)+copy*0.5,0,float(i%3));arena[off+28]=0
  for i,(start,finish) in enumerate(edges):
   off=sections+i*20;struct.pack_into('<HHBBBBffBB',arena,off,start,finish,0,0,255,255,1.,float(i%4+1)+copy*0.25,(i+d['reserved1'])%3 if d['graph']!=0 else 0,int(bool(d['flags']&8) and i%2==0))
   for node in (start,finish):
    off=nodes+node*32;n=arena[off+28];assert n<8;struct.pack_into('<H',arena,off+12+2*n,i);arena[off+28]=n+1
  # Sentinel section for signed -1 and trail/global projection quirk cases.
  struct.pack_into('<HHBBBBffBB',arena,sections-20,0,1,0,0,255,255,1.,1.,0,0)
  if 'length_ieee' in c:
   for i in range(len(edges)):struct.pack_into('<I',arena,sections+i*20+12,c['length_ieee'])
  if 'node_ieee' in c:
   for i in range(24):struct.pack_into('<I',arena,nodes+i*32+c.get('ieee_axis',0)*4,c['node_ieee'])
  if c.get('degenerate'):
   for i in range(-1,24):struct.pack_into('<3f',arena,nodes+i*32,0,0,0)
  if c.get('near_threshold'):
   for i in range(-1,24):struct.pack_into('<3f',arena,nodes+i*32,float(i)*0.01,0,0)
  if c.get('negative_node'):struct.pack_into('<H',arena,sections,0xffff)
 return words([d[k] for k in FIELDS])+arena

def fixtures():
 cases=[]
 for mode in range(6):
  for graph in range(6):cases.append(dict(mode=mode,graph=graph,target=5 if graph!=4 else 0))
 for mode in (1,2,4,5):
  for graph in (0,1,2,3,5):
   for rng in (0,1,16384,32767):cases.append(dict(mode=mode,graph=graph,rng=rng))
 for mode in (1,2,5):
  for count in (1,6,11,12,13):
   for flags in (0,1,2,8):cases.append(dict(mode=mode,count=count,flags=flags))
 for mode in (0,):
  for position in ((0.,0.,0.),(.5,0.,.5),(2.,3.,-4.),(-10.,0.,20.),(100.,0.,100.),(16777216.,1.,-16777216.)):
   for aliases in range(6):
    for trail in (0,4):cases.append(dict(mode=mode,px=fb(position[0]),py=fb(position[1]),pz=fb(position[2]),aliases=aliases,flags=trail))
 for mode in (3,):
  for graph in (0,1,2,3,4,5):
   for target in (0,1,2,5,9,13):
    for depth in (1,9,10):cases.append(dict(mode=mode,graph=graph,target=target,depth=depth))
 for mode in (1,2,5):
  for count in (0,1,11,12):
   for add in (-1,0,1,2,13):cases.append(dict(mode=mode,count=count,add=add,mutate=32 if add==0 else 0))
 for mode in (3,):
  for alias in range(5):
   for graph in (0,1,2):cases.append(dict(mode=mode,aliases=alias,graph=graph,target=2))
 for mode in (1,2,5):
  for mutation in (1,2,3,4,8,16):
   for graph in (1,3):cases.append(dict(mode=mode,graph=graph,mutate=mutation,count=0 if mode!=1 else 1,cached=-1))
 for now,expiry in ((0,0),(1000,999),(1000,1000),(1000,1001),(0xffffffff,0xfffffffe),(0,0xffffffff)):
  for cached in (-1,0,5):
   for force in (0,1):cases.append(dict(mode=5,now=now,expiry=expiry,cached=cached,force=force))
 special=[0,0x80000000,1,0x80000001,0x7fffff,0x800000,0x3f800000,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc12345,0xffc54321,0x7f800001,0xff812345]
 for value in special:
  for mode in (0,3,4,5):
   if mode==0:
    for axis in range(3):cases.append(dict(mode=mode,**{('px','py','pz')[axis]:value}))
   elif mode==3:cases.append(dict(mode=mode,graph=1,target=5,length_ieee=value,length=value))
   elif mode==4:cases.append(dict(mode=mode,car_dx=value))
   else:cases.append(dict(mode=mode,velocity=value,force=1))
 for value in (0x7fc12345,0x7f800001,0xff812345):
  for mode in (0,2,4,5):cases.append(dict(mode=mode,node_ieee=value,ieee_axis=0,count=0 if mode!=0 else 1))
 for mode in (0,4,5):
  for kind in ('degenerate','near_threshold'):cases.append(dict(mode=mode,**{kind:True}))
 for poison in (0xa5a5a5a5,0x3f3f3f3f,0x7fc12345):cases.append(dict(mode=0,px=0x7f7fffff,py=0x7f7fffff,pz=0x7f7fffff,reserved2=poison))
 for value in special:
  cases.append(dict(mode=5,velocity=value,px=fb(3.2),count=6,cached=2))
 for position in (3.2,7.8):
  for count in (1,6,11):
   for velocity in (0.,10.,10.000001,20.,100.):cases.append(dict(mode=5,px=fb(position),count=count,velocity=fb(velocity),cached=2))
 for strength in (-1,4,5,2147483647):
  for cached in (0,1,2,3):
   for count in (0,1,6):cases.append(dict(mode=5,strength=strength,cached=cached,count=count))
 for mode in (1,2,3,4,5):
  for graph in (0,1):
   for cheating in (0,1):cases.append(dict(mode=mode,graph=graph,cheating=cheating,flags=8,reserved1=2))
 for mode in (0,1,2,3,4,5):
  for direction in (0,2,3):cases.append(dict(mode=mode,direction=direction,negative_node=True))
 for node_value in (0x7fc12345,0xffc12345,0x7f800001,0xff800001):
  for point_value in (0x7fc54321,0xffc54321,0x7f800002):cases.append(dict(mode=0,node_ieee=node_value,px=point_value))
 for target in (0xabcd0002,0xffff0005):cases.append(dict(mode=3,target=target,graph=1))
 for add in (-2147483648,2147483647):cases.append(dict(mode=1,add=add,count=1))
 return cases
if __name__=='__main__':main()
