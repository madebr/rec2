#!/usr/bin/env python3
"""Physical-original proof of the real opponent lookup/projected-route closure.

Only PDEnterDebugger is controlled: its exact message and ordered state are
recorded, and optional mutation tests verify post-callback accesses. The genuine
diagnostic RET, all route/lookup bodies, raw input registers, and repository
layouts execute. Original/matching bytes are relocated only at audited absolute
operands and the two six-entry switch tables; no game imports/entry point run.
Width returns are captured as raw PC53 ST0 doubles, without NaN normalization.
"""
import argparse, hashlib, json, os, re, struct, subprocess
from pathlib import Path
from verify_tinted_native_reconstruction import extract

ROOT=Path(__file__).resolve().parents[1]
SOURCE=ROOT/'src/carma2/newcommon/30-opponent.c'
HASH='9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
ENTRIES=[('DoNotDprintf_opponent',0x4a8160,1),('ShiftOpponentsProjectedRoute',0x4ab100,78),('AddToOpponentsProjectedRoute',0x4ab150,50),('GetCarCount',0x4ae790,80),('GetCarSpec',0x4ae7e0,152),('GetOpponentSpecFromCarSpec',0x4ae8e0,134),('GetCarSpecFromGlobalOppoIndex',0x4ae970,63),('GetOpponentsRealSection',0x4ae9e0,31),('GetOpponentsFirstSection',0x4aea00,39),('GetOpponentsNextSection',0x4aea30,114),('GetOpponentsSectionStartNodePoint',0x4aeb90,166),('GetOpponentsSectionFinishNodePoint',0x4aec40,156),('GetOpponentsSectionWidth',0x4aece0,99),('GetOpponentsSectionMinSpeed',0x4aed50,148),('GetOpponentsSectionMaxSpeed',0x4aedf0,151)]
GLOBALS={'gProgram_state':(0x75bb80,0x5ed0),'gNet_mode':(0x68b918,4),'gNumber_of_net_players':(0x74b7d4,4),'gThis_net_player_index':(0x74b7d0,4),'gNet_players':(0x74bcc0,12*212),'gActive_non_car_list':(0x79eda0,99*4),'gNum_active_non_cars':(0x6793dc,4),'gNumber_of_cops_before_faffage':(0x691744,4)}
FIELDS='mode p q r count direction objective state cheating trail car_id index match opponents cops net_mode net_count net_self non_count width seed mutate'.split()
HEAP=0x20000000
def sha(b):return hashlib.sha256(b).hexdigest()
def words(xs):return struct.pack('<'+'I'*len(xs),*[x&0xffffffff for x in xs])
def pe(b):
 n=struct.unpack_from('<I',b,60)[0];return n,struct.unpack_from('<I',b,n+52)[0],struct.unpack_from('<I',b,n+80)[0],n+24+struct.unpack_from('<H',b,n+20)[0]
def raw(b,a,n):
 nt,base,size,t=pe(b)
 for i in range(struct.unpack_from('<H',b,nt+6)[0]):
  _,v,s,o=struct.unpack_from('<IIII',b,t+40*i+8)
  if v<=a-base and a-base+n<=v+s:return b[o+a-base-v:o+a-base-v+n]
 raise ValueError(hex(a))

def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE');p.add_argument('--output',type=Path,default=ROOT/'build-audit/opponent-route-tests');p.add_argument('--symbols',type=Path);p.add_argument('--recompiled',type=Path);p.add_argument('--case-index',type=int,action='append');a=p.parse_args()
 if bool(a.symbols)!=bool(a.recompiled):p.error('--symbols and --recompiled required together')
 b=a.original.read_bytes();assert sha(b)==HASH;out=a.output.resolve();out.mkdir(parents=True,exist_ok=True)
 from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM
 cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
 def relocation(data,entries,globals_,debug,tables):
  nt,base,size,tab=pe(data);patches={};closure=[];allowed={debug,*[v[1] for v in entries]}
  def relocated(v):
   for orig,(addr,n) in globals_.items():
    if addr<=v<addr+n:return HEAP+GLOBALS[orig][0]-0x650000+(v-addr)
   assert base<=v<base+size,hex(v)
   return v-base
  def add(where,value):
   target=relocated(value);kind=int(target>=HEAP);patches[where-base]=(value,target,kind)
  for name,start,n in entries:
   todo=[start];seen=set();insns=[];calls=[]
   while todo:
    pos=todo.pop()
    while pos not in seen:
     assert start<=pos<start+n,(name,hex(pos));ins=next(cs.disasm(raw(data,pos,min(15,start+n-pos)),pos,count=1));seen.add(pos);insns.append(ins)
     for op in ins.operands:
      if op.type==CS_OP_MEM and not op.mem.base and base<=op.mem.disp<base+size:
       assert ins.disp_size==4;add(pos+ins.disp_offset,op.mem.disp)
      if op.type==CS_OP_IMM and base<=op.imm<base+size and not (ins.mnemonic=='call' or ins.mnemonic.startswith('j')):
       assert ins.imm_size==4;add(pos+ins.imm_offset,op.imm)
     if ins.mnemonic=='call':
      assert ins.operands[0].type==CS_OP_IMM and ins.operands[0].imm in allowed,(name,ins.op_str);calls.append(ins.operands[0].imm)
     if ins.mnemonic.startswith('ret'):break
     if ins.mnemonic.startswith('j'):
      if ins.operands[0].type==CS_OP_IMM:
       dest=ins.operands[0].imm
       if start<=dest<start+n:todo.append(dest)
       else:assert dest in allowed,(name,ins.op_str)
      else:
       assert start in tables,(name,ins.op_str)
       t=tables[start];targets=struct.unpack('<6I',raw(data,t,24))
       for i,dest in enumerate(targets):assert start<=dest<start+n;add(t+4*i,dest);todo.append(dest)
      if ins.mnemonic=='jmp':break
     pos+=ins.size
   closure.append(dict(name=name,address=start,length=n,instructions=len(insns),bytes_sha256=sha(raw(data,start,n)),calls=sorted(set(calls))))
  return dict(base=base,size=size,patches=patches,closure=closure,entries=[x[1]-base for x in entries],debug=debug-base)
 original=relocation(b,ENTRIES,GLOBALS,0x51a7b0,{0x4ae790:0x4ae7c8,0x4ae7e0:0x4ae860})
 assert raw(b,0x4a8160,1)==b'\xc3' and raw(b,0x58a48c,4)==words([0x3f000000])
 def save_relocations(meta,path):path.write_bytes(words([*meta['entries'],meta['debug'],len(meta['patches'])])+b''.join(words([k,*v]) for k,v in sorted(meta['patches'].items())))
 save_relocations(original,out/'original-addresses.bin')
 caller=r'''
unsigned opponent_address,opponent_mode,opponent_x,opponent_y,opponent_z,opponent_result;double opponent_wide;static unsigned saved_stack,saved_top;
unsigned __declspec(naked) __cdecl OpponentInvoke(void){__asm{
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
 cmp opponent_mode,0
 je diagnostic
 cmp opponent_mode,2
 je third
 cmp opponent_mode,13
 je third
 cmp opponent_mode,14
 jne firsttwo
 third:
 push opponent_z
 firsttwo:
 mov ecx,opponent_x
 mov edx,opponent_y
 call dword ptr [opponent_address]
 jmp result
 diagnostic:
 push opponent_y
 push opponent_x
 call dword ptr [opponent_address]
 add esp,8
 xor eax,eax
 result:
 cmp opponent_mode,12
 jne integerresult
 fstp qword ptr [opponent_wide]
 xor eax,eax
 integerresult:
 cmp opponent_mode,13
 jl integerready
 movsx eax,ax
 integerready:
 mov opponent_result,eax
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
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "30-opponent.h"
#include "00-car.h"
#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"
extern unsigned opponent_address,opponent_mode,opponent_x,opponent_y,opponent_z,opponent_result;extern double opponent_wide;extern unsigned __cdecl OpponentInvoke(void);
static unsigned addresses[15],a[22],debug_count;static unsigned char*heap;static char debug_message[128];
#define OBJ(type,off) ((type*)(heap+(off)))
#define OPP OBJ(tOpponent_spec,0x200100)
#define CAR OBJ(tCar_spec,0x208000)
typedef char layouts[sizeof(void*)==4&&sizeof(tOpponent_spec)==420&&sizeof(tPath_node)==32&&sizeof(tPath_section)==20&&sizeof(tRoute_section)==4&&sizeof(tNet_game_player_info)==212&&sizeof(tProgram_state)==0x5ed0&&offsetof(tOpponent_spec,next_sections)==128&&offsetof(tCar_spec,my_trail)==0x13f8?1:-1];
'''
 for name,(addr,n) in GLOBALS.items():
  typ={'gProgram_state':'tProgram_state','gNet_players':'tNet_game_player_info','gActive_non_car_list':'tNon_car_spec*','gNet_mode':'tNet_mode'}.get(name,'int');off=addr-0x650000
  common+=f'#define {name} '+(f'OBJ({typ},0x{off:x})' if name in ('gNet_players','gActive_non_car_list') else f'(*OBJ({typ},0x{off:x}))')+'\n'
 common+=r'''
enum {MODE,P,Q,R,COUNT,DIRECTION,OBJECTIVE,STATE,CHEATING,TRAIL,CAR_ID,INDEX,MATCH,NOPP,NCOPS,NET_MODE,NET_COUNT,NET_SELF,NON_COUNT,WIDTH,SEED,MUTATE};
static void snapshot(void){unsigned values[6];values[0]=gNumber_of_cops_before_faffage;values[1]=gNum_active_non_cars;values[2]=gNet_mode;values[3]=gNumber_of_net_players;values[4]=gThis_net_player_index;values[5]=debug_count;fwrite(values,sizeof(values),1,stdout);fwrite(&gProgram_state,sizeof(gProgram_state),1,stdout);fwrite(gNet_players,sizeof(tNet_game_player_info)*12,1,stdout);fwrite(gActive_non_car_list,99*4,1,stdout);fwrite(heap+0x200000,0x1000,1,stdout);fwrite(heap+0x207f00,0x1800,1,stdout);fwrite(heap+0x21f000,0x6000,1,stdout);}
void __fastcall DebuggerBoundary(const char*message){debug_count++;memset(debug_message,0,sizeof(debug_message));strncpy(debug_message,message,sizeof(debug_message)-1);fwrite(debug_message,sizeof(debug_message),1,stdout);snapshot();if(a[MUTATE]&1){gProgram_state.AI_vehicles.number_of_opponents=0;gNumber_of_cops_before_faffage=-1;gNum_active_non_cars=17;OPP->nnext_sections=3;}}
void C2_HOOK_FASTCALL PDEnterDebugger(const char*message){DebuggerBoundary(message);}
static void setup(void){unsigned i,j;int k;tPath_section*section;memset(heap,0xa5,0x300000);for(i=0;i<0x300000;i+=4096)heap[i]=(unsigned char)a[SEED];debug_count=0;memset(debug_message,0,sizeof(debug_message));gProgram_state.AI_vehicles.number_of_opponents=(int)a[NOPP];gProgram_state.AI_vehicles.number_of_cops=987;gNumber_of_cops_before_faffage=(int)a[NCOPS];gNum_active_non_cars=(int)a[NON_COUNT];gNet_mode=(tNet_mode)a[NET_MODE];gNumber_of_net_players=(int)a[NET_COUNT];gThis_net_player_index=(int)a[NET_SELF];
 for(i=0;i<30;i++){gProgram_state.AI_vehicles.opponents[i].index=(int)(i%7)-2;gProgram_state.AI_vehicles.opponents[i].car_spec=OBJ(tCar_spec,0x280000+i*0x100);if(a[MATCH]==i||a[MATCH]==100)gProgram_state.AI_vehicles.opponents[i].car_spec=CAR;}
 for(i=0;i<10;i++){gProgram_state.AI_vehicles.cops[i].index=100+i;gProgram_state.AI_vehicles.cops[i].car_spec=OBJ(tCar_spec,0x290000+i*0x100);if(a[MATCH]==i||a[MATCH]==100)gProgram_state.AI_vehicles.cops[i].car_spec=CAR;}
 for(i=0;i<12;i++)gNet_players[i].car=OBJ(tCar_spec,0x2a0000+i*0x100);for(i=0;i<99;i++)gActive_non_car_list[i]=OBJ(tNon_car_spec,0x2b0000+i*0x100);
 OPP->nnext_sections=(int)a[COUNT];OPP->current_objective=(tOpponent_objective_type)a[OBJECTIVE];OPP->car_spec=CAR;OPP->pursue_car_data.pursuee=CAR;OPP->pursue_car_data.state=(tU8)a[STATE];OPP->follow_path_data.section_no=(tS16)a[INDEX];OPP->cheating=!!a[CHEATING];CAR->car_ID=(tS16)a[CAR_ID];CAR->my_trail.number_of_nodes=(tU8)a[TRAIL];memset(CAR->driver_name,0,sizeof(CAR->driver_name));strcpy(CAR->driver_name,"Route proof driver");
 for(i=0;i<12;i++){OPP->next_sections[i].section_no=(tS16)((int)(i%6)-1);OPP->next_sections[i].direction=(tU8)(a[DIRECTION]+i);OPP->next_sections[i].field_0x3=(unsigned char)(a[SEED]+i);}
 gProgram_state.AI_vehicles.path_sections=OBJ(tPath_section,0x220000);gProgram_state.AI_vehicles.path_nodes=OBJ(tPath_node,0x224000);
 for(k=-4;k<40;k++){section=&gProgram_state.AI_vehicles.path_sections[k];for(j=0;j<20;j++)((unsigned char*)section)[j]=(unsigned char)(j*13+k+a[SEED]);section->node_indices[0]=(tU16)(k-1);section->node_indices[1]=(tU16)(k+2);section->type=(tU8)(k%3);memcpy(&section->width,&a[WIDTH],4);}
 memcpy(&OPP->pursue_car_data.direct_line_section.width,&a[WIDTH],4);OPP->pursue_car_data.direct_line_section.min_speed[0]=17;OPP->pursue_car_data.direct_line_section.min_speed[1]=23;OPP->pursue_car_data.direct_line_section.max_speed[0]=99;OPP->pursue_car_data.direct_line_section.max_speed[1]=255;
 opponent_mode=a[MODE];opponent_x=(unsigned)OPP;opponent_y=a[P];opponent_z=a[R];if(opponent_mode==0){opponent_x=0x12345678;opponent_y=0x87654321;}if(opponent_mode==3||opponent_mode==4){opponent_x=a[P];opponent_y=a[Q];}if(opponent_mode==5)opponent_x=(unsigned)CAR;if(opponent_mode==6)opponent_x=a[P];opponent_result=0;opponent_wide=0;
}
static int run(const char*path){FILE*f;unsigned abi;f=fopen(path,"rb");if(!f)return 10;heap=(unsigned char*)VirtualAlloc((void*)0x20000000,0x300000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(heap!=(void*)0x20000000)return 11;_setmode(_fileno(stdout),_O_BINARY);while(fread(a,sizeof(a),1,f)==1){if(a[MODE]>=15)return 12;setup();opponent_address=addresses[a[MODE]];_controlfp(_PC_53,_MCW_PC);_clearfp();abi=OpponentInvoke();if(abi)return 13;fwrite(&opponent_result,4,1,stdout);fwrite(&opponent_wide,8,1,stdout);fwrite(debug_message,128,1,stdout);snapshot();}fclose(f);return 0;}
'''
 source=SOURCE.read_text();native=common+'\n'+'\n'.join(extract(source,name) for name,_,_ in ENTRIES)
 calls=['DoNotDprintf_opponent((const char*)x,y)','ShiftOpponentsProjectedRoute((tOpponent_spec*)x,(int)y)','AddToOpponentsProjectedRoute((tOpponent_spec*)x,(tS16)y,(int)z)','GetCarCount((tVehicle_type)x)','GetCarSpec((tVehicle_type)x,(int)y)','GetOpponentSpecFromCarSpec((tCar_spec*)x)','GetCarSpecFromGlobalOppoIndex((int)x)','GetOpponentsRealSection((tOpponent_spec*)x,(int)y)','GetOpponentsFirstSection((tOpponent_spec*)x)','GetOpponentsNextSection((tOpponent_spec*)x,(tS16)y)','GetOpponentsSectionStartNodePoint((tOpponent_spec*)x,(tS16)y)','GetOpponentsSectionFinishNodePoint((tOpponent_spec*)x,(tS16)y)','GetOpponentsSectionWidth((tOpponent_spec*)x,(tS16)y)','GetOpponentsSectionMinSpeed((tOpponent_spec*)x,(tS16)y,(int)z)','GetOpponentsSectionMaxSpeed((tOpponent_spec*)x,(tS16)y,(int)z)']
 for i,call in enumerate(calls):native+=f'\n'+('double' if i==12 else 'unsigned')+(' __cdecl' if i==0 else ' __fastcall')+f' adapter{i}(unsigned x,unsigned y'+(',unsigned z' if i in (2,13,14) else '')+'){'+(' (void)x; (void)y;' if i==0 else ' (void)y;' if i in (3,5,6,8) else '')+(' '+call+';return 0;' if i==0 else 'return '+('' if i==12 else '(unsigned)')+call+';')+'}\n'
 native+='\nint C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;'+''.join(f'addresses[{i}]=(unsigned)adapter{i};' for i in range(15))+'return run(argv[1]);}\n'
 physical=common+r'''
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned char*disk,*image;unsigned n,i,value,relative,h[17],*patch;IMAGE_NT_HEADERS*nt;IMAGE_SECTION_HEADER*sec;if(argc!=4)return 1;f=fopen(argv[2],"rb");if(!f)return 2;fseek(f,0,SEEK_END);n=(unsigned)ftell(f);rewind(f);disk=(unsigned char*)malloc(n);if(!disk||fread(disk,n,1,f)!=1)return 3;fclose(f);nt=(IMAGE_NT_HEADERS*)(disk+((IMAGE_DOS_HEADER*)disk)->e_lfanew);image=(unsigned char*)VirtualAlloc(NULL,nt->OptionalHeader.SizeOfImage,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image)return 4;memcpy(image,disk,nt->OptionalHeader.SizeOfHeaders);sec=IMAGE_FIRST_SECTION(nt);for(i=0;i<nt->FileHeader.NumberOfSections;i++)memcpy(image+sec[i].VirtualAddress,disk+sec[i].PointerToRawData,sec[i].SizeOfRawData);f=fopen(argv[3],"rb");if(!f||fread(h,sizeof(h),1,f)!=1)return 5;n=h[16];patch=(unsigned*)malloc(n*16);if(n&&(!patch||fread(patch,n*16,1,f)!=1))return 6;fclose(f);for(i=0;i<n;i++){if(patch[i*4]+4>nt->OptionalHeader.SizeOfImage)return 7;memcpy(&value,image+patch[i*4],4);if(value!=patch[i*4+1])return 8;value=patch[i*4+3]?patch[i*4+2]:(unsigned)image+patch[i*4+2];memcpy(image+patch[i*4],&value,4);}for(i=0;i<15;i++)addresses[i]=(unsigned)image+h[i];image[h[15]]=0xe9;relative=(unsigned)DebuggerBoundary-((unsigned)image+h[15]+5);memcpy(image+h[15]+1,&relative,4);free(patch);free(disk);return run(argv[1]);}
'''
 (out/'caller.c').write_text(caller);(out/'native.c').write_text(native);(out/'physical.c').write_text(physical)
 m5=ROOT/'build-tools/msvc500sp3';env5=dict(os.environ);env5['PATH']=str(m5/'bin')+os.pathsep+str(m5/'redist')+os.pathsep+env5['PATH'];env5['INCLUDE']=str(m5/'include');env5['LIB']=str(m5/'lib');msroot=ROOT/'build-tools/msvc-modern';ms=sorted((msroot/'VC/Tools/MSVC').iterdir())[-1];sdk=msroot/'Windows Kits/10';v=sorted((sdk/'Include').iterdir())[-1].name;env=dict(os.environ);env['PATH']=str(ms/'bin/Hostx64/x86')+os.pathsep+env['PATH'];env['INCLUDE']=os.pathsep.join(map(str,[ms/'include',*[sdk/f'Include/{v}/{x}' for x in ('ucrt','shared','um')]]));env['LIB']=os.pathsep.join(map(str,[ms/'lib/x86',*[sdk/f'Lib/{v}/{x}/x86' for x in ('ucrt','um')]]));inc=[ROOT/x for x in ('src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/brender/include','src/carma2/s3/include','src/carma2/platform')];gcc=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'
 commands={'caller':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Zl','/c',str(out/'caller.c'),'/Fo'+str(out/'caller.obj')],'msvc5':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(x) for x in inc],str(out/'native.c'),str(out/'caller.obj'),'/Fo'+str(out/'msvc5.obj'),'/Fe'+str(out/'msvc5.exe')],'msvc':[str(ms/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(x) for x in inc],str(out/'native.c'),str(out/'caller.obj'),'/Fo'+str(out/'msvc.obj'),'/Fe'+str(out/'msvc.exe')],'mingw':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation',*['-I'+str(x) for x in inc],str(out/'native.c'),str(out/'caller.obj'),'-o',str(out/'mingw.exe')],'physical':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-DREC2_MATCHING',*['-I'+str(x) for x in inc],str(out/'physical.c'),str(out/'caller.obj'),'-o',str(out/'physical.exe')]}
 for name,cmd in commands.items():
  r=subprocess.run(cmd,cwd=out,env=env5 if name in ('caller','msvc5') else env if name=='msvc' else None,capture_output=True,text=True);(out/f'{name}-build.txt').write_text(r.stdout+r.stderr)
  if r.returncode:raise RuntimeError(r.stdout+r.stderr)
 def case(mode,**kw):
  d=dict(mode=mode,p=20000,q=1,r=1,count=6,direction=0,objective=2,state=1,cheating=0,trail=25,car_id=0x200,index=2,match=2,opponents=8,cops=6,net_mode=1,net_count=4,net_self=1,non_count=7,width=0x3f800000,seed=31,mutate=0);d.update(kw);return [d[k] for k in FIELDS]
 cases=[case(0)]
 cases += [case(1,p=places,count=count) for count in (-2,0,1,6,12,13,2147483647) for places in (0,1,5,11,12,13,2147483647)]
 # Negative shifts traverse fields preceding the route, and forward overlap
 # repeats values. Negative append slots -1/-2 alias count/header storage.
 # Keep low16 count unchanged before the original direction-index reload.
 cases += [case(1,p=places,count=count) for count in (-1,0,1,6,12) for places in (-2,-1)]
 cases += [case(2,p=section,r=direction,count=count) for count in (0,1,11,12,13,2147483647) for section in (-32768,-1,0,32767,0xabcdffff) for direction in (-1,0,1,2,255,256,0x12345678)]
 cases += [case(2,p=section,r=direction,count=count) for count in (-2,-1) for section in (-1,0xabcdffff) for direction in (-1,0,1,2,255,256,0x12345678)]
 cases += [case(3,p=category,opponents=count,cops=count,net_count=count,non_count=count,net_mode=net) for category in (-1,0,1,2,3,4,5,6,2147483647) for count in (-2147483648,-1,0,1,10,2147483647) for net in (0,1,2)]
 cases += [case(4,p=category,q=index,net_self=self_,mutate=1) for category in (-1,0,1,2,3,4,5,6) for index in (0,1,3,7) for self_ in (-1,0,1,5)]
 cases += [case(5,car_id=id_,match=match,opponents=count,cops=min(count,10)) for id_ in (0,0x100,0x200,0x2ff,0x300,0x3ff,0xffff) for match in (-1,0,2,5,8,100) for count in (-1,0,1,6,10)]
 cases += [case(6,p=index,opponents=count) for index in (-2147483648,-2,-1,0,2,4,99,2147483647) for count in (-1,0,1,6,30)]
 cases += [case(7,p=section) for section in (-2147483648,-32768,-1,0,10000,14999,15000,15001,19999,*range(20000,20012))]
 cases += [case(8,objective=obj,state=state,index=section) for obj in (-1,0,2,3,99) for state in (0,1,2,3,255) for section in (-32768,0,15000,20000,32767)]
 cases += [case(9,p=section,count=count,cheating=cheat,trail=trail) for section in (-32768,0,10000,14999,15000,15001,15022,15023,19999,20000,20001,20004,20005,20011,32767,0xabcd4e20) for count in (0,1,6,12) for cheat in (0,1) for trail in (0,1,2,25,255)]
 cases += [case(mode,p=section,direction=direction,count=count,mutate=1) for mode in (10,11) for section in (-32768,-1,0,9999,10000,14999,15000,15024,20000,20001,20005,20011,20012,0xabcd4e20) for direction in (0,1,2,255) for count in (0,6,12)]
 special=[0,0x80000000,1,0x80000001,0x7fffff,0x800000,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc12345,0xffc54321,0x7f800001,0xff812345]
 cases += [case(12,p=section,width=width,count=count) for section in (-4,-1,0,4,10000,15000,20000,20001,20011,20012,0xabcd4e20) for width in special for count in (0,6,12)]
 cases += [case(mode,p=section,r=towards,direction=direction,count=count,mutate=1) for mode in (13,14) for section in (-32768,-1,0,10000,14999,15000,20000,20001,20005,20011,20012,0xabcd4e20) for towards in (-1,0,1,2,255,256,0x12345678) for direction in (0,1,2,255) for count in (0,6,12) if section!=10000 or towards in (-1,0,1,2)]
 if a.case_index:cases=[cases[i] for i in a.case_index]
 fixture=out/'cases.bin';fixture.write_bytes(b''.join(words(c) for c in cases));expected=subprocess.check_output([str(out/'physical.exe'),str(fixture),str(a.original.resolve()),str(out/'original-addresses.bin')]);(out/'original.bin').write_bytes(expected);records=[];failures=[]
 snapshot_bytes=24+0x5ed0+12*212+99*4+0x1000+0x1800+0x6000
 offsets=[];position=0
 for c in cases:
  mode,section=c[0],struct.unpack('<h',struct.pack('<H',c[1]&0xffff))[0]
  debug=(mode==4 and c[1]==4) or (mode in (10,11,13,14) and section<15000 and section!=10000)
  size=140+snapshot_bytes+(128+snapshot_bytes if debug else 0)
  offsets.append((position,size));position+=size
 assert len(expected)==position,'Unexpected original record framing/extra debugger callbacks'
 def failure(name,actual):
  first=next((i for i,(x,y) in enumerate(zip(actual,expected)) if x!=y),min(len(actual),len(expected)))
  index=next((i for i,(pos,n) in enumerate(offsets) if pos<=first<pos+n),len(cases)-1);pos,n=offsets[index]
  result=dict(compiler=name,first_byte=first,case_index=index,case=dict(zip(FIELDS,cases[index])),record_byte=first-pos,original_record_sha256=sha(expected[pos:pos+n]),actual_record_sha256=sha(actual[pos:pos+n]))
  (out/f'{name}-failure.json').write_text(json.dumps(result,indent=2)+'\n');return result
 for name in ('msvc5','msvc','mingw'):
  actual=subprocess.check_output([str(out/f'{name}.exe'),str(fixture)]);(out/f'{name}.bin').write_bytes(actual);records.append(dict(compiler=name,scenarios=len(cases),match=actual==expected,sha256=sha(actual)))
  if actual!=expected:failures.append(failure(name,actual))
 linked=None
 if a.symbols:
  data=a.recompiled.read_bytes();m=json.loads(a.symbols.read_text());assert m['format']==1 and m['original_sha256']==HASH and m['recompiled_sha256']==sha(data);symbols={x['orig']:x['recomp'] for x in m['symbols']};entries=[]
  for name,old,n in ENTRIES:
   start=symbols[old];end=min(x['recomp'] for x in m['symbols'] if x['recomp']>start);entries.append((name,start,end-start))
  tables={}
  for i in (3,4):
   start=entries[i][1]
   for ins in cs.disasm(raw(data,start,entries[i][2]),start):
    if ins.mnemonic=='jmp' and ins.operands[0].type==CS_OP_MEM and ins.operands[0].mem.index and not ins.operands[0].mem.base:tables[start]=ins.operands[0].mem.disp;break
  debug=symbols.get(0x51a7b0);debug_map=None
  if debug is None:
   # Platform debugger has no original-address annotation in current source.
   # Resolve exactly one linker symbol, then audit every body call against it.
   map_path=a.recompiled.with_suffix('.map');map_bytes=map_path.read_bytes();found=re.findall(r'^\s*[0-9a-fA-F]{4}:[0-9a-fA-F]{8}\s+@PDEnterDebugger@4\s+([0-9a-fA-F]{8})\s+f\b',map_bytes.decode('ascii'),re.MULTILINE)
   assert len(found)==1,'Missing/ambiguous PDEnterDebugger companion-map symbol'
   debug=int(found[0],16);debug_map=dict(path=str(map_path),sha256=sha(map_bytes),symbol='@PDEnterDebugger@4',address=debug)
  g={k:(symbols[v[0]],v[1]) for k,v in GLOBALS.items()};linked=relocation(data,entries,g,debug,tables);linked['debugger_companion_map']=debug_map;save_relocations(linked,out/'linked-addresses.bin');actual=subprocess.check_output([str(out/'physical.exe'),str(fixture),str(a.recompiled.resolve()),str(out/'linked-addresses.bin')]);(out/'linked.bin').write_bytes(actual);records.append(dict(compiler='physical-matching',scenarios=len(cases),match=actual==expected,sha256=sha(actual)))
  if actual!=expected:failures.append(failure('physical-matching',actual))
  linked['recompiled_sha256']=sha(data);linked['symbols_sha256']=sha(a.symbols.read_bytes())
 manifest=dict(scenarios=len(cases),comparisons=len(cases)*len(records),entry_scenario_counts={name:sum(c[0]==i for c in cases) for i,(name,_,_) in enumerate(ENTRIES)},checks=records,failures=failures,original_sha256=HASH,source_sha256=sha(SOURCE.read_bytes()),header_sha256=sha((SOURCE.parent/'30-opponent.h').read_bytes()),car_source_sha256=sha((SOURCE.parent/'00-car.c').read_bytes()),car_header_sha256=sha((SOURCE.parent/'00-car.h').read_bytes()),verifier_sha256=sha(Path(__file__).read_bytes()),fixtures_sha256=sha(fixture.read_bytes()),caller_object_sha256=sha((out/'caller.obj').read_bytes()),physical_original_sha256=sha(expected),original=original,linked=linked,precision_control=53,compiler_commands=commands,limitations='Only debugger API is controlled. Readable adversarial indices are intentional original raw addressing, not added bounds checks. Unmapped original failure paths and full opponent objectives are outside this closure. Short returns compare the public low16 signed value; unused highEAX bits are not a return contract. Native and original x87 exception status flags are not equated; raw width values are unnormalized.')
 (out/'verification.json').write_text(json.dumps(manifest,indent=2)+'\n')
 if failures:raise AssertionError(f'Opponent mismatch {failures}; see {out}')
 print(f'PASS {len(cases)} physical original scenarios / {manifest["comparisons"]} native comparisons')
if __name__=='__main__':main()
