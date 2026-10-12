#!/usr/bin/env python3
"""Strict physical-original/native proof of the complete shrapnel/splash consumer.

Original instruction bytes execute at PC53. Only relative dependency calls and
absolute globals/constants are relocated. Published recording, replay search,
car access, registry iteration, trig and smoke bodies execute actual source.
CRT rand and BRender actor/material callbacks are controlled boundaries.
Private stacks are explicitly seeded; only proved type11 payload padding14..15
and precisely tracked propagated copies are excluded from recording snapshots.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import random
import struct
import subprocess
import sys

ROOT=Path(__file__).resolve().parents[1]
HASH='9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
HEAP,SIZE=0x20000000,0x80000
FILES={n:ROOT/p for n,p in dict(spark='src/carma2/newcommon/32-spark.c',pipe='src/carma2/newcommon/23-piping.c',utility='src/carma2/newcommon/41-utility.c',trig='src/carma2/newcommon/36-trig.c',matrix='src/brender/core/math/matrix34.c',quat='src/brender/core/math/quat.c',vector='src/brender/core/math/vector.c',crush='src/carma2/newcommon/13-crush1.c',ped='src/carma2/newcommon/61-pedestrn.c',phil='src/carma2/newcommon/60-phil.c',car='src/carma2/newcommon/30-opponent.c').items()}
SPEC={
 'spark':['SmokeScalarRound','SmokeScalarBits','SparkFtol','SplashReplayMoving','SplashScaleProduct','FlameAnimate','CreatePuffOfSmoke','SmudgeCar','DoSmokeColumn','MungeSmokeColumn','DrMatrix34Rotate','GetVelocitiesFromMatrices','SplashSubtractFromScalar','SingleSplash','GeneralCreateSplash','CreateSplash','MungeShrapnel'],
 'pipe':['ARGetReplayRate','ReplayCopyForward','ReplaySearchSessionLength','LengthOfSession','ARStartPipingSession','EndPipingSession2','AREndPipingSession','ARAddDataToSession','ARAddVariedDataToSession','AddSmokeColumnToPipingSession','AddFlameToPipingSession','FindNextChunk','AddShrapnelToPipingSession','CalcShrapnelLength'],
 'utility':['UtilityRandomNegative','UtilityRandomScalar','IRandomBetween','FRandomBetween','SRandomBetween','SRandomPosNeg','GetTotalTime'],
 'trig':['TrigArcTanIsNaN','TrigArcTanAbs','TrigArcTanRatio','FastFloatArcTan2Angle','FastScalarArcTan2Angle','FastFloatSin','FastFloatCos','FastScalarSin','FastScalarCos','FastScalarSinAngle','FastScalarCosAngle','DRMatrix34RotateY','DRMatrix34RotateZ','DRMatrix34PreRotateZ'],
 'matrix':['BrMatrix34Copy','BrMatrix34RawShearX','BrMatrix34ShearX','BrMatrix34Mul','BrMatrix34Scale','BrMatrix34ApplyP','BrMatrix34ApplyV','BrMatrix34TApplyV','BrMatrix34PreShearX'],
 'quat':['BrQuatMul','BrQuatInvert','BrMatrix34ToQuat'],
 'vector':['BrVector3SetFloat'], 'crush':['IsOnNonCarHalf'],
 'ped':['GetCharacterActorPtr'], 'phil':['PHILGetFirstObject','PHILGetNextObject'],
 'car':['GetCarCount','GetCarSpec']}
NEW=['MungeShrapnel','DrMatrix34Rotate','GeneralCreateSplash','SingleSplash','CreateSplash','GetVelocitiesFromMatrices','GetCharacterActorPtr']

def main():
 p=argparse.ArgumentParser(description=__doc__)
 for n,d in [('original',ROOT/'build-reference/originals/CARMA2_HW.EXE'),('recompiled',ROOT/'build-matching/rec2.exe'),('output',ROOT/'build-audit/shrapnel-splash-tests'),('dependency-dir',ROOT/'build-audit/joystick/python-deps'),('gcc',ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'),('msvc-root',ROOT/'build-tools/msvc-modern'),('msvc5-root',ROOT/'build-tools/msvc500sp3')]:p.add_argument('--'+n,type=Path,default=d)
 p.add_argument('--symbols',type=Path);p.add_argument('--build-only',action='store_true');args=p.parse_args()
 sys.path.insert(0,str(args.dependency_dir.resolve()))
 from capstone import Cs,CS_ARCH_X86,CS_MODE_32
 from capstone.x86 import X86_OP_MEM,X86_OP_IMM
 from verify_tinted_native_reconstruction import extract
 original=args.original.read_bytes();assert hashlib.sha256(original).hexdigest()==HASH
 out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)
 texts={n:v.read_text() for n,v in FILES.items()}
 bodies=[extract(texts[k],n) for k,names in SPEC.items() for n in names]
 addr={}
 for k,names in SPEC.items():
  for n in names:
   b=extract(texts[k],n);prefix=texts[k][:texts[k].index(b)]
   marker=re.findall(r'// FUNCTION: CARMA2_HW (0x[0-9a-fA-F]+)',prefix[-200:])
   if marker:addr[n]=int(marker[-1],16)
 addr.update(ftol=0x575b80,rand=0x576970,BrActorAdd=0x51dfe0,BrActorRemove=0x51e070,BrMaterialUpdate=0x520e70)
 catalog={int(v['address'],16):v for v in map(json.loads,(ROOT/'build-audit/splash-next/functions.jsonl').read_text().splitlines())}
 def raw_at(a,z):
  pe=struct.unpack_from('<I',original,60)[0];table=pe+24+struct.unpack_from('<H',original,pe+20)[0]
  for i in range(struct.unpack_from('<H',original,pe+6)[0]):
   _,r,n,o=struct.unpack_from('<IIII',original,table+40*i+8)
   if 0x400000+r<=a<0x400000+r+n:return original[o+a-0x400000-r:][:z]
  raise AssertionError(hex(a))
 # Canonical globals are taken from real declarations, rather than invented
 # oracle variables. Each physical operand is recorded in the evidence below.
 globals_code=r'''
tU8 *gPipe_buffer_start,*gLocal_buffer,*gMr_chunky,*gPipe_buffer_phys_end,*gPipe_buffer_working_end,*gPipe_buffer_oldest,*gPipe_record_ptr,*gPipe_play_ptr;
const tReplay_callback*gPipe_callbacks;tU32 gLocal_buffer_size,gUNK_0069410c;int gReentrancy_count;tPipe_chunk_type gReentrancy_array[5];
int gSmoke_flags,gSmoke_next,gPHIL_mechanics_time_sync;tSmoke gSmoke[25];tCar_spec*gCar_to_view;
int gAction_replay_mode,gColumn_flags,gColumn_next,gSmoke_on;tProgram_state gProgram_state;tSmoke_column gSmoke_column[10];br_actor*gNon_track_actor;
br_matrix34 gCamera_to_world,mattmp1,mattmp2,mattmp1__trig,mattmp2__trig;br_pixelmap*gFlame_map[20];float gReplay_rate;
tSplash gSplash[32];tShrapnel gShrapnel[30];tSpark gSpark[32];tReplay_splash gReplay_splashes[10];
int gSplash_flags,gShrapnel_flags,gSpark_flags,gSplash_next,gSpark_next,gNum_splash_types,gReplay_splash_flags,gAFE,gAFE_color_cycle;
tU32 gNext_AFE_color_cycle,gLast_replay_frame_time,gLast_tick_count,gFrame_period,gLost_time;
int gPHIL_disabled,gNumber_of_cops_before_faffage,gNum_active_non_cars,gNumber_of_net_players,gThis_net_player_index;
tPhysics_object*gPHIL_list_collision_infos;tPedestrian*gPedestrian_array;tNet_mode gNet_mode;
tNet_game_player_info gNet_players[12];tNon_car_spec*gActive_non_car_list[99];
'''
 driver=r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <float.h>
#include <math.h>
#include "32-spark.h"
#include "13-crush1.h"
#include "49-physics.h"
#include "60-phil.h"
#include "61-pedestrn.h"
#include "30-opponent.h"
#include "00-car.h"
#include "23-piping.h"
#include "52-errors.h"
#include "41-utility.h"
#include "36-trig.h"
#include "globvars.h"
#include "globvrpb.h"
#include "rec2_macros.h"
#include "quat.h"
#include "matrix34.h"
#define M(row,col) mat->m[row][col]
#define A(row,col) A->m[row][col]
#define B(row,col) B->m[row][col]
#define C(row,col) C->m[row][col]
__declspec(dllimport) void* __stdcall VirtualAlloc(void*,unsigned,unsigned,unsigned);
typedef char fixture_layouts[sizeof(tReplay_splash)==48&&sizeof(tSplash)==28&&sizeof(tShrapnel)==44&&sizeof(tSpark)==64&&offsetof(br_actor,identifier)==20&&offsetof(br_actor,t)==40&&offsetof(tPhysics_object,bb1)==36&&offsetof(tPhysics_object,next)==0x220&&offsetof(tPed_character_instance,ped)==0xe4?1:-1];
'''+globals_code+r'''
static unsigned char*arena;static unsigned h[64];static unsigned events,rng_count,callback_count;
static unsigned char previous[0x120000],now[0x120000],padding[0x80000];
static unsigned rd(unsigned off){unsigned v;memcpy(&v,arena+off,4);return v;}
static void wr(unsigned off,unsigned v){memcpy(arena+off,&v,4);}
static void*ptr(unsigned off){return arena+off;}
static unsigned full(void){unsigned n=0,i,v;unsigned values[]={gSplash_flags,gShrapnel_flags,gSpark_flags,gSplash_next,gSpark_next,gNum_splash_types,gReplay_splash_flags,gAFE,gAFE_color_cycle,gNext_AFE_color_cycle,gAction_replay_mode,gColumn_flags,gColumn_next,gSmoke_flags,gSmoke_next,gPHIL_mechanics_time_sync,gPHIL_disabled,(unsigned)(uintptr_t)gPHIL_list_collision_infos,gLocal_buffer_size,gReentrancy_count,(unsigned)(uintptr_t)gPipe_record_ptr,(unsigned)(uintptr_t)gPipe_working_dummy};
 (void)v;
 memcpy(now,arena,0x80000);n=0x80000;for(i=0;i<0x80000;i++)if(padding[i])now[i]=0;
 memcpy(now+n,gSplash,sizeof(gSplash));n+=sizeof(gSplash);memcpy(now+n,gShrapnel,sizeof(gShrapnel));n+=sizeof(gShrapnel);memcpy(now+n,gSpark,sizeof(gSpark));n+=sizeof(gSpark);memcpy(now+n,gReplay_splashes,sizeof(gReplay_splashes));n+=sizeof(gReplay_splashes);
 memcpy(now+n,values,sizeof(values));n+=sizeof(values);memcpy(now+n,&gCamera_to_world,48);n+=48;memcpy(now+n,&gReplay_rate,4);n+=4;memcpy(now+n,&gProgram_state,sizeof(gProgram_state));n+=sizeof(gProgram_state);memcpy(now+n,gSmoke_column,sizeof(gSmoke_column));n+=sizeof(gSmoke_column);memcpy(now+n,gSmoke,sizeof(gSmoke));n+=sizeof(gSmoke);memcpy(now+n,&rng_count,4);n+=4;memcpy(now+n,&callback_count,4);n+=4;
 {unsigned extra[]={(unsigned)(uintptr_t)gMr_chunky,(unsigned)(uintptr_t)gLocal_buffer,(unsigned)(uintptr_t)gPipe_buffer_oldest,(unsigned)(uintptr_t)gPipe_buffer_start,(unsigned)(uintptr_t)gPipe_buffer_phys_end,(unsigned)(uintptr_t)gPipe_play_ptr,(unsigned)(uintptr_t)gPipe_callbacks};memcpy(now+n,extra,sizeof(extra));n+=sizeof(extra);}
 memcpy(now+n,gReentrancy_array,20);n+=20;memcpy(now+n,&mattmp1,48);n+=48;memcpy(now+n,&mattmp2,48);n+=48;memcpy(now+n,&mattmp1__trig,48);n+=48;memcpy(now+n,&mattmp2__trig,48);n+=48;memcpy(now+n,padding,sizeof(padding));n+=sizeof(padding);return n;}
static void hex(const void*p,unsigned n){static const char d[]="0123456789abcdef";unsigned i;const unsigned char*b=p;for(i=0;i<n;i++){putchar(d[b[i]>>4]);putchar(d[b[i]&15]);}}
static void delta(void){unsigned n=full(),i=0,start;int pieces=0;while(i<n){if(now[i]==previous[i]){i++;continue;}start=i;while(i<n&&now[i]!=previous[i])i++;printf("%s%08x:",pieces++?";":"",start);hex(now+start,i-start);}memcpy(previous,now,n);}
static void event(const char*kind,unsigned a,unsigned b){printf("%s[\"%s\",%u,%u,\"",events++?",":"",kind,a,b);delta();printf("\"]");}
static void mutation(void){if(callback_count++!=h[12])return;if(h[13]&1)gShrapnel_flags=h[14];if(h[13]&2)gShrapnel[0].age=h[15];if(h[13]&4)gShrapnel[0].actor=ptr(0x1800);if(h[13]&8)gSplash_next=h[46];if(h[13]&16)gSpark_next=h[47];if(h[13]&32)gPHIL_disabled=1;if(h[13]&64)((tPhysics_object*)ptr(0x10000))->next=NULL;if(h[13]&128)gAction_replay_mode=1;}
static int C2_HOOK_CDECL test_rand(void){unsigned r=h[10];rng_count++;event("rand",r,0);mutation();return (int)r;}
br_actor*C2_HOOK_CDECL BrActorAdd(br_actor*p,br_actor*a){event("add",(unsigned)(uintptr_t)p,(unsigned)(uintptr_t)a);a->parent=p;mutation();return a;}
br_actor*C2_HOOK_CDECL BrActorRemove(br_actor*a){event("remove",(unsigned)(uintptr_t)a,0);a->parent=NULL;mutation();return a;}
void C2_HOOK_CDECL BrMaterialUpdate(br_material*m,br_uint_16 f){event("material",(unsigned)(uintptr_t)m,f);mutation();}
void C2_HOOK_CDECL PDEnterDebugger(const char*s){(void)s;abort();}
void C2_NORETURN C2_HOOK_CDECL FatalError(int i,...){(void)i;abort();}
/* Read-only instrumentation marks only original unwritten payload bytes. */
static void observe_add(int type,uintptr_t owner,unsigned length){unsigned dest;if(!gPipe_buffer_start||gAction_replay_mode||!gProgram_state.racing||gLocal_buffer_size+length+4>=15000)return;dest=(unsigned)(gMr_chunky-arena);memset(padding+dest,0,length+4);if(type==11&&(owner&0x8000)){padding[dest+18]=padding[dest+19]=1;}}
static void observe_end(void){unsigned local,size,dest;if(!gPipe_buffer_start||gAction_replay_mode||!gProgram_state.racing)return;local=(unsigned)(gLocal_buffer-arena);size=gLocal_buffer_size;memset(padding+local+size,0,2);size+=2;if(rd(local+4)&&size<=15000){dest=(unsigned)(gPipe_record_ptr-arena);if(gPipe_record_ptr+size>gPipe_buffer_phys_end)dest=(unsigned)(gPipe_buffer_start-arena);memmove(padding+dest,padding+local,size);}}
#define rand test_rand
'''
 driver=driver.replace('gPipe_working_dummy','gPipe_buffer_working_end')
 for file,pattern in [('spark',r'static tU8 gFlame_frame_size\[20\]\[2\] = \{.*?\n\};'),('trig',r'float gFloat_sine_table\[91\] = \{.*?\n\};')]:
  driver+=re.search(pattern,texts[file],re.S).group(0)+'\n'
 for b in bodies:
  name=re.search(r'\b([A-Za-z_][A-Za-z_0-9]*)\s*\([^;]*\)\s*\{',b).group(1)
  if name=='ARAddDataToSession':b=b.replace('    (void)pType;','    observe_add(pType, pOwner, pSize);\n    (void)pType;',1)
  if name=='EndPipingSession2':b=b.replace('    tPipe_chunk_type type;','    tPipe_chunk_type type;\n\n    observe_end();',1)
  if name=='GeneralCreateSplash':b='#if defined(_MSC_VER)\n#if _MSC_VER >= 1300\n#pragma warning(push)\n#endif\n#pragma warning(disable:4700)\n#endif\n'+b+'\n#if defined(_MSC_VER)\n#if _MSC_VER >= 1300\n#pragma warning(pop)\n#else\n#pragma warning(default:4700)\n#endif\n#endif'
  driver+=b+'\n'
 driver+='\n#undef rand\n'
 physical_names=NEW+['AddShrapnelToPipingSession','CalcShrapnelLength','BrMatrix34ApplyP','BrMatrix34ApplyV','BrMatrix34TApplyV','BrQuatMul','BrQuatInvert','BrMatrix34ToQuat','IRandomBetween','FRandomBetween','SRandomBetween','SRandomPosNeg','ftol','BrVector3SetFloat']
 placement={};off=0
 for n in physical_names:placement[n]=off;off=(off+catalog[addr[n]]['body_bytes']+63)&~63
 targets={addr[n]:'(unsigned)(uintptr_t)(code+%d)'%z for n,z in placement.items()}
 for n in addr:
  if n not in physical_names:targets[addr[n]]='(unsigned)(uintptr_t)'+('test_rand' if n=='rand' else n)
 # Derive real global addresses from active markers. Unknown operands fail.
 global_names=set(re.findall(r'\bg[A-Z][A-Za-z_0-9]*\b',globals_code))|{'gFlame_frame_size','mattmp1','mattmp2','mattmp1__trig','mattmp2__trig'}
 global_addr={}
 for path in list((ROOT/'src/carma2/newcommon').glob('*.c'))+[FILES['matrix']]:
  content=path.read_text()
  for name in global_names:
   match=re.search(r'// (?:GLOBAL|DATA): CARMA2_HW (0x[0-9a-fA-F]+)\s*\n[^;=\n]*\b'+name+r'\b',content)
   if match:global_addr[name]=int(match.group(1),16)
 image_pairs=[(original,{**addr,**global_addr})]
 if args.symbols:
  mapping=json.loads(args.symbols.read_text());recompiled=args.recompiled.read_bytes();assert mapping['original_sha256']==HASH and mapping['recompiled_sha256']==hashlib.sha256(recompiled).hexdigest()
  symbols={v['orig']:v for v in mapping['symbols']}
  image_pairs.append((recompiled,{n:symbols[v]['recomp'] for n,v in {**addr,**global_addr}.items()}))
 lengths={'gProgram_state':24272,'gSpark':2048,'gSplash':896,'gShrapnel':1320,'gReplay_splashes':480,'gCamera_to_world':48,'gFlame_map':80,'gSmoke_column':1240,'gSmoke':1100,'gFlame_frame_size':40,'mattmp1':48,'mattmp2':48,'mattmp1__trig':48,'mattmp2__trig':48}
 md=Cs(CS_ARCH_X86,CS_MODE_32);md.detail=True;constants={};patches=[];physical=[]
 def mapped(a,z):
  for n,v in global_addr.items():
   if a==v:return '(unsigned)(uintptr_t)&'+n
  for n,v in global_addr.items():
   if v<=a<=v+lengths.get(n,3):return '(unsigned)(uintptr_t)&%s+%d'%(n,a-v)
  if a==0x66cc20:
   z=12;assert raw_at(a,z)==struct.pack('<3I',1,2,0)
  else:assert 0x580000<=a<0x600000,('unmapped original data',hex(a),global_addr)
  constants[a,z]=raw_at(a,z);return '(unsigned)(uintptr_t)constant_%x_%d'%(a,z)
 driver+='\n#ifdef PHYSICAL_ORIGINAL\n'
 for n,z in placement.items():
  raw=raw_at(addr[n],catalog[addr[n]]['body_bytes']);physical.append(dict(name=n,address=addr[n],body_bytes=len(raw),sha256=hashlib.sha256(raw).hexdigest(),hex=raw.hex()))
  driver+='static const unsigned char physical_%s[]={%s};\n'%(n,','.join(map(str,raw)))
  for ins in md.disasm(raw,addr[n]):
   at=z+ins.address-addr[n]
   if ins.mnemonic=='call':
    assert ins.operands[0].type==X86_OP_IMM,(n,ins.op_str);a=ins.operands[0].imm;assert a in targets,(n,hex(a))
    patches.append('patch(code+%d,%s-(unsigned)(uintptr_t)(code+%d));'%(at+ins.imm_offset,targets[a],at+ins.size))
   else:
    for op in ins.operands:
     if op.type==X86_OP_MEM and op.mem.disp>=0x400000:patches.append('patch(code+%d,%s);'%(at+ins.disp_offset,mapped(op.mem.disp,op.size)))
     elif op.type==X86_OP_IMM and ins.imm_size==4 and ins.mnemonic in ['mov','cmp','push'] and 0x600000<=op.imm<0x800000:patches.append('patch(code+%d,%s);'%(at+ins.imm_offset,('(unsigned)(uintptr_t)&gShrapnel+'+str(op.imm-0x6a9180) if n=='MungeShrapnel' and ins.mnemonic=='cmp' and op.imm in [0x6a96bc,0x6a96b8] else '(unsigned)(uintptr_t)&gSplash+896' if n=='MungeShrapnel' and ins.mnemonic=='cmp' and op.imm==0x6a8638 else mapped(op.imm,4))))
 for (a,z),raw in constants.items():driver+='static const unsigned char constant_%x_%d[]={%s};\n'%(a,z,','.join(map(str,raw)))
 driver+='static unsigned char*physical_base;\nstatic void patch(unsigned char*p,unsigned v){memcpy(p,&v,4);}\nstatic void physical_init(void){unsigned char*code=VirtualAlloc(NULL,32768,0x3000,0x40);if(!code)abort();physical_base=code;\n'
 for n,z in placement.items():driver+='memcpy(code+%d,physical_%s,sizeof(physical_%s));\n'%(z,n,n)
 driver+='\n'.join(patches)+'\n}\n'
 for n in NEW+['AddShrapnelToPipingSession','CalcShrapnelLength']:
  b=extract(texts['ped' if n=='GetCharacterActorPtr' else 'pipe' if n in ['AddShrapnelToPipingSession','CalcShrapnelLength'] else 'spark'],n);signature=b[:b.index('{')].strip();ret,params=signature.split(n,1);signature=ret.replace('C2_HOOK_FASTCALL','')+'(C2_HOOK_FASTCALL*)'+params
  driver+='#define %s ((%s)(physical_base+%d))\n'%(n,signature,placement[n])
 driver+='\n#endif\n'
 driver+=r'''
#ifdef PHYSICAL_ORIGINAL
__declspec(dllimport) void* __stdcall GetProcAddress(void*,const char*);
__declspec(dllimport) void* __stdcall GetModuleHandleA(const char*);
static long __stdcall exception_log(unsigned**p){unsigned*c=p[1];fprintf(stderr,"exception EIP%x relative%x EAX%x ECX%x EDX%x ESI%x EDI%x ESP%x\n",c[46],c[46]-(unsigned)physical_base,c[44],c[43],c[42],c[40],c[39],c[49]);return 0;}
#endif
static unsigned value;
static void C2_HOOK_CDECL dispatch(void){br_scalar water;memcpy(&water,h+18,4);
 if(h[0]==0)MungeShrapnel(h[1]);
 else if(h[0]==1)GeneralCreateSplash(h[19]?ptr(0x10000):NULL,ptr(0x20000),ptr(0x20010),ptr(0x20020),water,ptr(0x20040),ptr(0x20100),h[1]);
 else if(h[0]==2)SingleSplash(h[19]?ptr(0x10000):NULL,ptr(0x20100),ptr(0x20000),ptr(0x20030),ptr(0x20010),ptr(0x20060),ptr(0x20020),h[1]);
 else if(h[0]==3)GetVelocitiesFromMatrices(ptr(h[20]?h[20]:0x20200),ptr(h[21]?h[21]:0x20210),ptr(0x20100),ptr(0x20140),h[1]);
 else if(h[0]==4)DrMatrix34Rotate(ptr(0x20100),(br_angle)h[22],ptr(h[23]?h[23]:0x20010));
 else if(h[0]==5)value=(unsigned)(uintptr_t)GetCharacterActorPtr(ptr(0x21000));
 else if(h[0]==6)CreateSplash(ptr(0x10000),h[1]);
 else if(h[0]==7)AddShrapnelToPipingSession((int)h[24],ptr(h[25]?h[25]:0x20000),(tU16)h[26],ptr(h[27]?h[27]:0x22000));
 else value=(unsigned)CalcShrapnelLength(ptr(0x20000));
}
/* Fresh stack history is intentional and reported per fixture, not zeroed
 * implicitly. The ABI bridge changes only the private stack/TEB bounds. */
static void fresh_call(void){static const unsigned char raw[]={0x53,0x56,0x57,0x55,0x89,0xe3,0x64,0x8b,0x35,4,0,0,0,0x64,0x8b,0x3d,8,0,0,0,0x8b,0x44,0x24,0x14,0x8b,0x54,0x24,0x18,0x64,0x89,0x15,4,0,0,0,0x81,0xea,0,0,0x20,0,0x64,0x89,0x15,8,0,0,0,0x81,0xc2,0,0,0x20,0,0x89,0xd4,0x83,0xe4,0xf0,0xff,0xd0,0x89,0xdc,0x64,0x89,0x35,4,0,0,0,0x64,0x89,0x3d,8,0,0,0,0x5d,0x5f,0x5e,0x5b,0xc3};void*code=VirtualAlloc(NULL,4096,0x3000,0x40);unsigned char*stack=VirtualAlloc(NULL,0x200000,0x3000,4);if(!code||!stack)abort();memcpy(code,raw,sizeof(raw));memset(stack,(int)h[11],0x200000);((void(C2_HOOK_CDECL*)(void*,void*))code)((void*)(uintptr_t)dispatch,stack+0x200000);}
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned i,n;setvbuf(stderr,NULL,_IONBF,0);if(argc==1){printf("{\"opponent_count\":%u,\"opponent_car\":%u,\"cop_car\":%u,\"opponent_stride\":%u,\"net_car\":%u,\"net_stride\":%u}", (unsigned)(offsetof(tProgram_state,AI_vehicles)+offsetof(tIntelligent_vehicles,number_of_opponents)),(unsigned)(offsetof(tProgram_state,AI_vehicles)+offsetof(tIntelligent_vehicles,opponents)+offsetof(tOpponent_spec,car_spec)),(unsigned)(offsetof(tProgram_state,AI_vehicles)+offsetof(tIntelligent_vehicles,cops)+offsetof(tOpponent_spec,car_spec)),(unsigned)sizeof(tOpponent_spec),(unsigned)offsetof(tNet_game_player_info,car),(unsigned)sizeof(tNet_game_player_info));return 0;}if(argc!=2)return 1;arena=VirtualAlloc((void*)0x20000000,0x80000,0x3000,4);if(arena!=(void*)0x20000000)abort();f=fopen(argv[1],"rb");if(!f)return 2;if(fread(h,sizeof(h),1,f)!=1||fread(arena,0x80000,1,f)!=1||fread(gShrapnel,sizeof(gShrapnel),1,f)!=1||fread(gSplash,sizeof(gSplash),1,f)!=1||fread(gSpark,sizeof(gSpark),1,f)!=1||fread(gReplay_splashes,sizeof(gReplay_splashes),1,f)!=1||fread(&gCamera_to_world,48,1,f)!=1)return 3;fclose(f);_controlfp(_PC_53,_MCW_PC);
#ifdef PHYSICAL_ORIGINAL
 physical_init();((void*(__stdcall*)(unsigned,void*))GetProcAddress(GetModuleHandleA("kernel32.dll"),"AddVectoredExceptionHandler"))(1,(void*)(uintptr_t)exception_log);
#endif
 gShrapnel_flags=h[2];gSplash_flags=h[3];gAction_replay_mode=h[4];gNum_splash_types=h[5];gReplay_splash_flags=h[6];gAFE=h[7];gNext_AFE_color_cycle=h[8];gSplash_next=h[16];gSpark_next=h[17];memcpy(&gReplay_rate,h+9,4);gPHIL_disabled=h[28];gPHIL_list_collision_infos=h[29]?ptr(0x10000):NULL;gPedestrian_array=ptr(0x23000);gNon_track_actor=ptr(0x3000);gProgram_state.racing=h[30];gProgram_state.cockpit_on=h[31];gProgram_state.current_car.collision_info=ptr(0x10000);gLast_tick_count=h[32];gFrame_period=h[33];gLost_time=h[34];gLast_replay_frame_time=h[35];gNet_mode=(tNet_mode)h[36];
 gProgram_state.AI_vehicles.number_of_opponents=h[41];gNumber_of_cops_before_faffage=h[42];gNumber_of_net_players=h[43];for(i=0;i<h[41];i++)gProgram_state.AI_vehicles.opponents[i].car_spec=ptr(0x12000);for(i=0;i<h[42];i++)gProgram_state.AI_vehicles.cops[i].car_spec=ptr(0x12000);for(i=0;i<h[43];i++)gNet_players[i].car=ptr(0x12000);
 gPipe_buffer_start=h[37]?ptr(0x60000):NULL;gPipe_record_ptr=ptr(h[38]?h[38]:0x60000);gPipe_buffer_oldest=NULL;gPipe_buffer_phys_end=ptr(0x78000);gPipe_buffer_working_end=gPipe_buffer_phys_end;gPipe_play_ptr=ptr(0x60000);gPipe_callbacks=ptr(0x50000);gLocal_buffer=ptr(0x40000);gMr_chunky=ptr(0x40008);gLocal_buffer_size=8;gReentrancy_count=h[39];for(i=0;i<5;i++)gReentrancy_array[i]=(tPipe_chunk_type)(20+i);gSmoke_on=1;gCar_to_view=ptr(0x12000);
 for(i=0;i<71;i++){unsigned off=0x50000+24*i+8;if(rd(off)==0x1082000)wr(off,(unsigned)(uintptr_t)CalcShrapnelLength);}n=full();memcpy(previous,now,n);printf("{\"calls\":[");fresh_call();printf("],\"state\":\"");delta();printf("\",\"value\":%u}\n",value);return 0;}
'''
 # Function addresses are transport identities in the callback table only.
 driver=driver.replace('memcpy(now,arena,0x80000);n=0x80000;', 'memcpy(now,arena,0x80000);n=0x80000;{unsigned identity=0x1082000;memcpy(now+0x50110,&identity,4);}')
 harness=out/'harness.c'
 harness.write_text(driver.replace('abort();','{fprintf(stderr, "abort line%d\\n", __LINE__);abort();}'))
 includes=[ROOT/r for r in ['src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/carma2/platform','src/carma2/s3/include','src/brender/include','src/brender/core/math']]
 msvc=sorted((args.msvc_root.resolve()/'VC/Tools/MSVC').iterdir())[-1];sdk=args.msvc_root.resolve()/'Windows Kits/10';version=sorted(p.name for p in (sdk/'Include').iterdir())[-1]
 env=dict(os.environ);env['PATH']=str(msvc/'bin/Hostx64/x86')+os.pathsep+env['PATH'];env['INCLUDE']=os.pathsep.join(map(str,[msvc/'include',*[sdk/f'Include/{version}/{r}' for r in ['ucrt','shared','um']]]));env['LIB']=os.pathsep.join(map(str,[msvc/'lib/x86',*[sdk/f'Lib/{version}/{r}/x86' for r in ['ucrt','um']]]))
 old=args.msvc5_root.resolve();oldenv=dict(os.environ);oldenv['PATH']=str(old/'bin')+os.pathsep+str(old/'redist')+os.pathsep+oldenv['PATH'];oldenv['INCLUDE']=str(old/'include');oldenv['LIB']=str(old/'lib')
 commands=dict(msvc=[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(r) for r in includes],str(harness),'/Fo'+str(out/'msvc.obj'),'/Fe'+str(out/'msvc.exe')],msvc5=[str(old/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(r) for r in includes],str(harness),'/Fo'+str(out/'msvc5.obj'),'/Fe'+str(out/'msvc5.exe')],mingw=[str(args.gcc.resolve()),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-fno-strict-aliasing',*['-I'+str(r) for r in includes],str(harness),'-o',str(out/'mingw.exe')])
 commands['physical-original']=commands['msvc5'][:-2]+['/DPHYSICAL_ORIGINAL','/Fo'+str(out/'physical-original.obj'),'/Fe'+str(out/'physical-original.exe')]
 for name in ['msvc','msvc5','physical-original']:commands[name]+=['/link','kernel32.lib','/STACK:8388608']
 for name,cmd in commands.items():
  build=subprocess.run(cmd,cwd=out,env=env if name=='msvc' else oldenv if name in ['msvc5','physical-original'] else None,text=True,capture_output=True);(out/(name+'-build.log')).write_text(build.stdout+build.stderr);assert build.returncode==0,build.stdout+build.stderr
 (out/'physical-original.json').write_text(json.dumps(dict(original_sha256=HASH,bodies=physical,patches=patches,global_operands=global_addr,precision_control=53),indent=2)+'\n')
 if args.build_only:return
 layouts=json.loads(subprocess.check_output([str(out/'msvc5.exe')],text=True))
 def execute(data,a,h,arena,shrap,splash,spark,replay,camera):
  from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
  from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_EAX,UC_X86_REG_ECX,UC_X86_REG_EDX,UC_X86_REG_FPCW
  u=Uc(UC_ARCH_X86,UC_MODE_32);u.mem_map(0x400000,0x800000);u.mem_map(HEAP,0x400000)
  pe=struct.unpack_from('<I',data,60)[0];table=pe+24+struct.unpack_from('<H',data,pe+20)[0]
  for i in range(struct.unpack_from('<H',data,pe+6)[0]):
   _,r,n,o=struct.unpack_from('<IIII',data,table+40*i+8);u.mem_write(0x400000+r,data[o:o+n])
  def wr(at,v):u.mem_write(at,struct.pack('<I',v&0xffffffff))
  def rd(at):return struct.unpack('<I',u.mem_read(at,4))[0]
  u.mem_write(HEAP,bytes(arena));u.mem_write(HEAP+0x100000,bytes([h[11]])*0x200000)
  for n,b in [('gShrapnel',shrap),('gSplash',splash),('gSpark',spark),('gReplay_splashes',replay),('gCamera_to_world',camera)]:u.mem_write(a[n],bytes(b))
  initial={'gShrapnel_flags':h[2],'gSplash_flags':h[3],'gAction_replay_mode':h[4],'gNum_splash_types':h[5],'gReplay_splash_flags':h[6],'gAFE':h[7],'gNext_AFE_color_cycle':h[8],'gSplash_next':h[16],'gSpark_next':h[17],'gReplay_rate':h[9],'gPHIL_disabled':h[28],'gPHIL_list_collision_infos':HEAP+0x10000 if h[29] else 0,'gPedestrian_array':HEAP+0x23000,'gNon_track_actor':HEAP+0x3000,'gLast_tick_count':h[32],'gFrame_period':h[33],'gLost_time':h[34],'gLast_replay_frame_time':h[35],'gNet_mode':h[36],'gPipe_buffer_start':HEAP+0x60000 if h[37] else 0,'gPipe_record_ptr':HEAP+(h[38] or 0x60000),'gPipe_buffer_oldest':0,'gPipe_buffer_phys_end':HEAP+0x78000,'gPipe_buffer_working_end':HEAP+0x78000,'gPipe_play_ptr':HEAP+0x60000,'gPipe_callbacks':HEAP+0x50000,'gLocal_buffer':HEAP+0x40000,'gMr_chunky':HEAP+0x40008,'gLocal_buffer_size':8,'gReentrancy_count':h[39],'gSmoke_on':1,'gCar_to_view':HEAP+0x12000}
  for n,v in initial.items():wr(a[n],v)
  u.mem_write(a['gProgram_state'],bytes(24272));wr(a['gProgram_state']+40,h[30]);wr(a['gProgram_state']+76,h[31]);wr(a['gProgram_state']+172+8,HEAP+0x10000)
  wr(a['gProgram_state']+layouts['opponent_count'],h[41]);wr(a['gNumber_of_cops_before_faffage'],h[42]);wr(a['gNumber_of_net_players'],h[43])
  for i in range(h[41]):wr(a['gProgram_state']+layouts['opponent_car']+i*layouts['opponent_stride'],HEAP+0x12000)
  for i in range(h[42]):wr(a['gProgram_state']+layouts['cop_car']+i*layouts['opponent_stride'],HEAP+0x12000)
  for i in range(h[43]):wr(a['gNet_players']+i*layouts['net_stride']+layouts['net_car'],HEAP+0x12000)
  u.mem_write(a['gSmoke_column'],bytes(1240));u.mem_write(a['gSmoke'],bytes(1100))
  for i in range(5):wr(a['gReentrancy_array']+4*i,20+i)
  wr(HEAP+0x50110,a['CalcShrapnelLength'])
  calls=[];padding=bytearray(SIZE);counts=[0,0];previous=[b'']
  values_names=['gSplash_flags','gShrapnel_flags','gSpark_flags','gSplash_next','gSpark_next','gNum_splash_types','gReplay_splash_flags','gAFE','gAFE_color_cycle','gNext_AFE_color_cycle','gAction_replay_mode','gColumn_flags','gColumn_next','gSmoke_flags','gSmoke_next','gPHIL_mechanics_time_sync','gPHIL_disabled','gPHIL_list_collision_infos','gLocal_buffer_size','gReentrancy_count','gPipe_record_ptr','gPipe_buffer_working_end']
  def full():
   copy=bytearray(u.mem_read(HEAP,SIZE));struct.pack_into('<I',copy,0x50110,0x1082000)
   for i in range(SIZE):
    if padding[i]:copy[i]=0
   extra_names=['gMr_chunky','gLocal_buffer','gPipe_buffer_oldest','gPipe_buffer_start','gPipe_buffer_phys_end','gPipe_play_ptr','gPipe_callbacks']
   return bytes(copy)+b''.join(bytes(u.mem_read(a[n],z)) for n,z in [('gSplash',896),('gShrapnel',1320),('gSpark',2048),('gReplay_splashes',480)])+b''.join(struct.pack('<I',rd(a[n])) for n in values_names)+bytes(u.mem_read(a['gCamera_to_world'],48))+bytes(u.mem_read(a['gReplay_rate'],4))+bytes(u.mem_read(a['gProgram_state'],24272))+bytes(u.mem_read(a['gSmoke_column'],1240))+bytes(u.mem_read(a['gSmoke'],1100))+struct.pack('<2I',*counts)+b''.join(struct.pack('<I',rd(a[n])) for n in extra_names)+bytes(u.mem_read(a['gReentrancy_array'],20))+b''.join(bytes(u.mem_read(a[n],48)) for n in ['mattmp1','mattmp2','mattmp1__trig','mattmp2__trig'])+bytes(padding)
  def delta():
   now=full();before=previous[0];i=0;parts=[]
   while i<len(now):
    if now[i:i+1024]==before[i:i+1024]:i+=min(1024,len(now)-i);continue
    if now[i]==before[i]:i+=1;continue
    start=i
    while i<len(now) and now[i]!=before[i]:i+=1
    parts.append('%08x:'%start+now[start:i].hex())
   previous[0]=now;return ';'.join(parts)
  def mutation():
   index=counts[1];counts[1]+=1
   if index!=h[12]:return
   if h[13]&1:wr(a['gShrapnel_flags'],h[14])
   if h[13]&2:wr(a['gShrapnel']+20,h[15])
   if h[13]&4:wr(a['gShrapnel'],HEAP+0x1800)
   if h[13]&8:wr(a['gSplash_next'],h[46])
   if h[13]&16:wr(a['gSpark_next'],h[47])
   if h[13]&32:wr(a['gPHIL_disabled'],1)
   if h[13]&64:wr(HEAP+0x10220,0)
   if h[13]&128:wr(a['gAction_replay_mode'],1)
  def observe_add(kind,owner,size):
   if not rd(a['gPipe_buffer_start']) or rd(a['gAction_replay_mode']) or not rd(a['gProgram_state']+40) or rd(a['gLocal_buffer_size'])+size+4>=15000:return
   dest=rd(a['gMr_chunky'])-HEAP;padding[dest:dest+size+4]=bytes(size+4)
   if kind==11 and owner&0x8000:padding[dest+18:dest+20]=b'\1\1'
  def observe_end():
   if not rd(a['gPipe_buffer_start']) or rd(a['gAction_replay_mode']) or not rd(a['gProgram_state']+40):return
   local=rd(a['gLocal_buffer'])-HEAP;size=rd(a['gLocal_buffer_size']);padding[local+size:local+size+2]=bytes(2);size+=2
   if rd(HEAP+local+4) and size<=15000:
    dest=rd(a['gPipe_record_ptr'])-HEAP
    if HEAP+dest+size>rd(a['gPipe_buffer_phys_end']):dest=rd(a['gPipe_buffer_start'])-HEAP
    padding[dest:dest+size]=padding[local:local+size]
  def hook(uc,at,size,context):
   sp=uc.reg_read(UC_X86_REG_ESP)
   if at==a['ARAddVariedDataToSession']:
    kind=rd(sp+4);owner=rd(sp+8);observe_add(kind,owner,20 if owner&0x8000 else 12);return
   if at==a['ARAddDataToSession']:observe_add(uc.reg_read(UC_X86_REG_ECX),uc.reg_read(UC_X86_REG_EDX),rd(sp+8));return
   if at in [a['EndPipingSession2'],a['AREndPipingSession']]:observe_end();return
   if at==a['rand']:
    counts[0]+=1;calls.append(['rand',h[10],0,delta()]);mutation();value=h[10]
   elif at==a['BrActorAdd']:
    parent,actor=rd(sp+4),rd(sp+8);calls.append(['add',parent,actor,delta()]);wr(actor+12,parent);mutation();value=actor
   elif at==a['BrActorRemove']:
    actor=rd(sp+4);calls.append(['remove',actor,0,delta()]);wr(actor+12,0);mutation();value=actor
   elif at==a['BrMaterialUpdate']:
    material,flags=rd(sp+4),rd(sp+8);calls.append(['material',material,flags,delta()]);mutation();value=0
   else:return
   uc.reg_write(UC_X86_REG_EAX,value);uc.reg_write(UC_X86_REG_EIP,rd(sp));uc.reg_write(UC_X86_REG_ESP,sp+4)
  u.hook_add(UC_HOOK_CODE,hook);u.reg_write(UC_X86_REG_FPCW,0x27f);previous[0]=full()
  modes=[('MungeShrapnel',h[1],0,[]),('GeneralCreateSplash',HEAP+0x10000 if h[19] else 0,HEAP+0x20000,[HEAP+0x20010,HEAP+0x20020,h[18],HEAP+0x20040,HEAP+0x20100,h[1]]),('SingleSplash',HEAP+0x10000 if h[19] else 0,HEAP+0x20100,[HEAP+0x20000,HEAP+0x20030,HEAP+0x20010,HEAP+0x20060,HEAP+0x20020,h[1]]),('GetVelocitiesFromMatrices',HEAP+(h[20] or 0x20200),HEAP+(h[21] or 0x20210),[HEAP+0x20100,HEAP+0x20140,h[1]]),('DrMatrix34Rotate',HEAP+0x20100,h[22],[HEAP+(h[23] or 0x20010)]),('GetCharacterActorPtr',HEAP+0x21000,0,[]),('CreateSplash',HEAP+0x10000,h[1],[]),('AddShrapnelToPipingSession',h[24],HEAP+(h[25] or 0x20000),[h[26]&65535,HEAP+(h[27] or 0x22000)]),('CalcShrapnelLength',HEAP+0x20000,0,[])]
  name,x,y,tail=modes[h[0]];stack,stop=HEAP+0x300000,HEAP+0x380000;wr(stack,stop)
  for i,v in enumerate(tail):wr(stack+4+4*i,v)
  u.reg_write(UC_X86_REG_ESP,stack);u.reg_write(UC_X86_REG_ECX,x);u.reg_write(UC_X86_REG_EDX,y);u.emu_start(a[name],stop,count=4000000);assert u.reg_read(UC_X86_REG_EIP)==stop;assert u.reg_read(UC_X86_REG_ESP)==stack+4+4*len(tail)
  value=u.reg_read(UC_X86_REG_EAX) if h[0] in [5,8] else 0
  return dict(calls=calls,state=delta(),value=value)
 def f(v):return struct.unpack('<I',struct.pack('<f',v))[0]
 cases=[dict(mode=m,seed=s) for m in range(9) for s in [0,0xa5,0xff]]
 for mode in [0,1,2,3,4,6]:
  for numeric in range(24):cases.append(dict(mode=mode,seed=0xa5,numeric=numeric))
 for time in [0,1,99,100,1000,1001,16777217,0x80000001,0xffffffff]:
  for mode in [0,3]:cases.append(dict(mode=mode,seed=255,time=time))
 for age in [0,1,999,1000,1001,0xffffffff,0xfffffff0]:cases.append(dict(mode=0,age=age))
 for sync in [1,99,1000,16777217,0x80000001,0xffffffff]:cases.append(dict(mode=0,sync=sync))
 for rate in [0,0x80000000,f(.33333334),f(-1),f(3.14),0x7f800000,0x7fc01234,0x7f800001]:cases.append(dict(mode=0,replay=1,rate=rate))
 for mask in [1,2,4,8,16,32,64,128]:
  for mode in [0,1,2]:cases.append(dict(mode=mode,mutation=mask,splash_enabled=mode==0))
 for enabled in [0,1]:
  for cockpit in [0,1]:
   for done in [0,1]:cases.append(dict(mode=0,splash_enabled=enabled,cockpit=cockpit,done=done,head=1))
 for owner in [0,1,0x7fff,0x8000,0x8001,0x12345678,0x80000000,0xffffffff]:
  for mode in [7,8]:cases.append(dict(mode=mode,owner=owner))
 for mode in [0,7]:
  for gate in [0,1,2,3]:cases.append(dict(mode=mode,gate=gate))
 for off in [0x20100,0x20104,0x20108,0x20110,0x20120]:cases.append(dict(mode=3,velocity_out=off,omega_out=off+4))
 for off in [0x20100,0x20104,0x20110,0x20120]:cases.append(dict(mode=4,axis_at=off))
 for index in [0,1,2,127,128,255]:cases.append(dict(mode=5,character_index=index))
 for mode in [0,1,2,3,4,6,7]:
  for raw in [0x80000000,1,0x7f800000,0xff800000,0x7fc01234,0xffc01234,0x7f800001,0xff800001]:cases.append(dict(mode=mode,raw=raw))
 for slot in [1,2,255]:
  for found in [0,1]:
   for rate in [f(1),f(-1),0,0x7fc01234,0x7f800001]:cases.append(dict(mode=0,replay=1,splash_enabled=1,slot=slot,found=found,rate=rate))
 for driver in [5,6,8,-1]:
  for cars in [1,2,3]:cases.append(dict(mode=0,replay=1,splash_enabled=1,cars=cars,driver=driver))
 for depth in [1,2,4]:cases.append(dict(mode=0,reentrancy=depth))
 for random_value in [0,32767]:
  for seed in [0,165,255]:cases.append(dict(mode=1,random_value=random_value,seed=seed,numeric=3))
 for mode in [1,2,6]:
  for emission in [0,1]:
   for mask in [0,8,16,128]:cases.append(dict(mode=mode,emission=emission,mutation=mask,mutation_at=1))
 for gravity in [f(.99999994),f(1),f(1.01),0x7fc01234,0x7f800001]:cases.append(dict(mode=0,splash_enabled=1,head=1,gravity=gravity))
 for clock in [0,99,100,101,0x80000001,0xffffffff]:cases.append(dict(mode=0,afe=1,clock=clock))
 for mode in [0,2,3,4,6]:
  for pair in [(0x7fc01234,0xffc05678),(0x7f801234,0xff805678),(0x7f800000,0xff800000),(0x7f7fffff,0xff7fffff)]:cases.append(dict(mode=mode,pair=pair))
 for velocity in [(-1,0,0),(-1,1e-9,0),(-1,1e-6,0),(-1,2.384185791015625e-7,0),(-.3,.2,-.7)]:
  for seed in [0,165,255]:cases.append(dict(mode=1,velocity=velocity,normal=(1,0,0),seed=seed))
 for mode in [7]:
  for alias in [0x40008,0x4000c,0x60000]:cases.append(dict(mode=mode,packet_alias=alias))
 for wrap in [0x77ff0,0x77ff8,0x78000]:cases.append(dict(mode=0,record_at=wrap,age=1001))
 for angle in [0,1,16383,16384,32767,32768,49152,65535,0xffffffff]:cases.append(dict(mode=4,angle=angle))
 for flags in [0,0x80000000,0x3fffffff,0xffffffff]:cases.append(dict(mode=0,flags=flags))
 for time,age,rate in [(0x80000001,0x80000001,f(.1)),(0xffffffff,0xffffffff,f(-1)),(16777217,0x7fffffff,f(.33333334)),(1,0x80000000,0x7f7fffff)]:cases.append(dict(mode=0,replay=1,time=time,age=age,rate=rate))
 for column in [0,1,2]:
  for bits in [0x7fc01234,0x7f801234,0x80000000]:cases.append(dict(mode=0,splash_enabled=1,cockpit=1,camera_component=column,camera_bits=bits))
 for a,b in [(0x7fc12345,0x7f812345),(0x7f812345,0x7fc12345),(0xffc12345,0x7f812345),(0x7fc00001,0x7f812345),(0x7f812345,0xff812345),(0xffc12345,0x7fc12345)]:
  cases.append(dict(mode=0,splash_enabled=1,splash_pair=(a,b),physical_only=True))
 results=[]
 for index,case in enumerate(cases):
  h=[0]*64;h[0]=case['mode'];h[1]=case.get('time',16);h[2]=1;h[3]=1;h[4]=case.get('replay',0);h[5]=case.get('splash_enabled',0);h[9]=case.get('rate',f(1));h[10]=8191;h[11]=case.get('seed',0xa5);h[13]=case.get('mutation',0);h[14]=3;h[15]=1001;h[16]=31;h[17]=31;h[18]=f(.2);h[19]=1;h[20]=case.get('velocity_out',0);h[21]=case.get('omega_out',0);h[22]=12345;h[23]=case.get('axis_at',0);h[28]=0 if case.get('head') else 1;h[29]=case.get('head',0);h[30]=1;h[31]=case.get('cockpit',0);h[32]=101;h[37]=1;h[24]=case.get('owner',0x8001);h[26]=0x1234
  if case.get('gate')==1:h[37]=0
  if case.get('gate')==2:h[4]=1
  if case.get('gate')==3:h[30]=0
  h[6]=1 if case.get('slot') else 0;h[7]=case.get('afe',0);h[12]=case.get('mutation_at',0);h[32]=case.get('clock',101)
  if case.get('cars')==2:h[41]=2
  if case.get('cars')==3:h[42]=2
  if case.get('cars')==1:h[43]=3;h[36]=1
  if 'emission' in case:h[10]=0 if case['emission'] else 32767
  h[25]=case.get('packet_alias',0);h[38]=case.get('record_at',0)
  h[22]=case.get('angle',h[22])
  h[2]=case.get('flags',h[2])
  h[39]=case.get('reentrancy',0)
  arena=bytearray(SIZE);shrap=bytearray(1320);splash=bytearray(896);spark=bytearray([0xa5])*2048;replay=bytearray(480)
  def put(off,*v):struct.pack_into('<'+'I'*len(v),arena,off,*[x&0xffffffff for x in v])
  def vec(off,*v):put(off,*map(f,v))
  def matrix(off):put(off,*map(f,[1,0,0,0,1,0,0,0,1,.1,.2,.3]))
  for i in range(62):
   at=0x1000+128*i;put(at+12,HEAP+0x3000);put(at+28,HEAP+0x22000);matrix(at+44)
  for i in range(30):struct.pack_into('<I3fII2f3f',shrap,44*i,HEAP+0x1000+128*i,.03,.02,.01,case.get('sync',0),case.get('age',32),.01,-.02,.2,.3,.9327379)
  for i in range(32):struct.pack_into('<I3fI2f',splash,28*i,HEAP+0x1000+128*(30+i),.1,.2,.3,case.get('done',1),.15,1.2)
  put(0x10000,HEAP+0x1000);put(0x10020,HEAP+0x11000);put(0x1023c,HEAP+0x12000);put(0x10258,HEAP+0x13000);arena[0x10238]=1;put(0x1200c,8);put(0x12010,HEAP+0x1000);vec(0x10024,-1,0,-1,1,1,1);vec(0x10068,.3,-.5,.4);vec(0x10074,.1,.2,.3);vec(0x10244,0,1,0);put(0x10250,f(.2));put(0x13088,f(.5));vec(0x11004,-1,0,-1,1,1,1)
  put(0x12008,HEAP+0x10000);put(0x1200c,case.get('driver',8));put(0x13088,case.get('gravity',f(.5)))
  if case.get('head'):arena[0x10400:0x10700]=arena[0x10000:0x10300];put(0x10220,HEAP+0x10400)
  vec(0x20000,.3,-.5,.4);vec(0x20010,.1,.2,.3);vec(0x20020,0,1,0);vec(0x20030,0,-.5,0);vec(0x20040,-1,0,-1,1,1,1);vec(0x20060,.1,.2,.3);matrix(0x20100);matrix(0x20140);vec(0x20164,.2,.4,.5)
  put(0x21000,HEAP+0x21100);arena[0x21004]=case.get('character_index',0);put(0x21128,HEAP+0x21200);put(0x21230,HEAP+0x21300)
  for i in range(128):put(0x21304+8*i,HEAP+0x21800);put(0x21800,HEAP+0x1000)
  put(0x210e4,HEAP+0x23000+168);struct.pack_into('<H',arena,0x23000+168+6,0xfffe);put(0x1000+20,HEAP+0x24000);arena[0x24000:0x24009]=b'ABCD0001\0'
  if 'numeric' in case:
   rng=random.Random(99182+case['numeric']);v=[rng.uniform(-2,2) for _ in range(3)];o=[rng.uniform(-.5,.5) for _ in range(3)];n=[rng.uniform(-1,1) for _ in range(3)]
   vec(0x20000,*v);vec(0x10068,*v);vec(0x20010,*o);vec(0x10074,*o);vec(0x20020,*n);vec(0x10244,*n);h[18]=f(rng.uniform(-1,1));put(0x10250,h[18]);h[10]=rng.randrange(32768)
   mat=[rng.uniform(-1,1) for _ in range(12)];put(0x20100,*map(f,mat));put(0x20140,*map(f,[rng.uniform(-1,1) for _ in range(12)]));put(0x1000+44,*map(f,mat));vec(0x20060,*[rng.uniform(-1,1) for _ in range(3)])
   struct.pack_into('<3f',shrap,4,*v);struct.pack_into('<3f',shrap,32,*n);h[22]=rng.randrange(65536)
  if 'random_value' in case:h[10]=case['random_value']
  if 'raw' in case:
   bits=case['raw'];put(0x20000,bits,bits,bits);put(0x10068,bits,bits,bits);put(0x20010,bits,bits,bits);put(0x10074,bits,bits,bits);struct.pack_into('<3I',shrap,4,bits,bits,bits);struct.pack_into('<3I',shrap,32,bits,bits,bits)
   if h[0]==3:put(0x20100,bits);put(0x20124,bits,bits,bits)
  if 'pair' in case:
   x,y=case['pair'];put(0x20000,x,y,x);put(0x10068,x,y,x);put(0x20010,y,x,y);put(0x10074,y,x,y);struct.pack_into('<3I',shrap,4,x,y,x);struct.pack_into('<3I',shrap,32,y,x,y)
   if h[0]==3:put(0x20100,x);put(0x20110,y);put(0x20124,x,y,x)
  if 'velocity' in case:vec(0x20000,*case['velocity']);vec(0x20020,*case['normal'])
  if h[0]==8:put(0x20000,h[24])
  for i in range(71):put(0x50000+24*i,0,12,0,0,0,0)
  put(0x50108,0,0,0x1082000,0,0,0);put(0x40000,11,0)
  if 'slot' in case:
   slot=case['slot'];obj={1:HEAP+0x1000,2:HEAP+0x21000,255:HEAP+0x10000}[slot];kind={1:14,2:43,255:48}[slot];owner={1:1,2:0xfffe0002,255:HEAP+0x10000}[slot];payload=52 if slot==1 else 48
   struct.pack_into('<II4f6f',replay,0,obj,slot,.2,0,1,0,-1,0,-1,1,1,1);put(0x60000,kind,1);put(0x60008,owner if case['found'] else owner+1)
   matrix(0x60008+(8 if slot==1 else 4));vec(0x60008+(8 if slot==1 else 4)+36,.25,.3,.4);put(0x50000+24*kind+4,payload);h[38]=0x60000+10+4+payload
  camera=bytearray(struct.pack('<12I',*map(f,[1,0,0,0,1,0,0,0,1,0,0,0])))
  if 'camera_component' in case:struct.pack_into('<I',camera,4*case['camera_component'],case['camera_bits'])
  if 'splash_pair' in case:struct.pack_into('<2I',splash,20,*case['splash_pair'])
  fixture=out/('case-%03d.bin'%index);fixture.write_bytes(struct.pack('<64I',*h)+arena+shrap+splash+spark+replay+camera)
  expected=json.loads(subprocess.check_output([str(out/'physical-original.exe'),str(fixture)],text=True))
  for name in ['msvc5','msvc','mingw']:
   actual=json.loads(subprocess.check_output([str(out/(name+'.exe')),str(fixture)],text=True))
   if actual!=expected:
    failure=out/('failure-%s-%d.json'%(name,index));failure.write_text(json.dumps(dict(case=case,original=expected,actual=actual),indent=2));raise AssertionError(str(failure))
   results.append(dict(case=case,compiler=name,passed=True))
  if args.symbols and not case.get('physical_only'):
   expected_emulated=execute(*image_pairs[0],h,arena,shrap,splash,spark,replay,camera)
   actual_emulated=execute(*image_pairs[1],h,arena,shrap,splash,spark,replay,camera)
   if actual_emulated!=expected_emulated:
    failure=out/('failure-recompiled-%d.json'%index);failure.write_text(json.dumps(dict(case=case,original=expected_emulated,actual=actual_emulated),indent=2));raise AssertionError(str(failure))
   results.append(dict(case=case,compiler='recompiled-x86',passed=True,physical_original_agrees=expected_emulated==expected))
 assert bodies==[extract(FILES[k].read_text(),n) for k,names in SPEC.items() for n in names], 'A tested source body changed during this run'
 verification=dict(scenario_count=len(cases),comparisons=len(results),source_sha256={n:hashlib.sha256(v.read_bytes()).hexdigest() for n,v in FILES.items()},header_sha256={n:hashlib.sha256((FILES[n].with_suffix('.h')).read_bytes()).hexdigest() for n in ['spark','pipe','ped']},types_sha256=hashlib.sha256((ROOT/'src/carma2/include/rec2_types.h').read_bytes()).hexdigest(),verifier_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),bodies_sha256=[hashlib.sha256(b.encode()).hexdigest() for b in bodies],original_sha256=HASH,new_original_entries={n:addr[n] for n in NEW+['AddShrapnelToPipingSession','CalcShrapnelLength']},cases=results,stack_history_patterns=[0,165,255],oracle_normalizations=[],padding_exclusions={'type11_payload_offsets':[14,15],'propagation':'Only these bytes at the actual committed packet destination and their actual session-ring copies; the exclusion bitmap is itself compared strictly.'},physical_original={'body_bytes':physical,'patches':patches,'executable_sha256':hashlib.sha256((out/'physical-original.exe').read_bytes()).hexdigest()},compiler_commands=commands,limitations=['32-bit x86 PC53; sticky FPU status flags are outside observed game state.', 'Six retained paired splash scale/size NaN cases are physical-original/native-only. Unicorn disagrees with the physical original on signaling NaN memory-operand selection; no emulator output is corrected or normalized.','All selected new original bodies execute physically, including the serializer and calculator; published trig, smoke, registry iteration, car access and replay recording/search bodies execute actual source. Original and rebuilt complete PE closures are independently compared in Unicorn; each original emulator result is also checked against the physical original result.','Accessible valid actor, matrix, physics/shape/car, character/form/actor-set and pedestrian graphs; bounded aliased matrix/vector outputs, ordinary and nonfinite IEEE values, terminating recording/search sessions and zero-to-two cars in each moving replay category. Existing smoke-column processing executes its real body with column_flags0, and is separately covered by retained smoke-updater regressions.','GeneralCreateSplash preserves its original unwritten private minimum-point copy. Fresh private physical and native stacks are seeded with00/A5/FF, including extrema/triplet geometry. No uninitialized temporary is zeroed or masked.','CRT rand is controlled in0..32767; actor add/remove and material update are controlled callbacks with strict lossless state deltas. Replay search types14/43/48 use bounded fixed payload descriptors. Real type11 CalcShrapnelLength is used.'])
 if args.symbols:
  verification.update(physical_only_scenarios=sum(bool(c.get('physical_only')) for c in cases),recompiled_sha256=hashlib.sha256(args.recompiled.read_bytes()).hexdigest(),symbols_sha256=hashlib.sha256(args.symbols.read_bytes()).hexdigest(),original_emulator_physical_checks=sum(v['compiler']=='recompiled-x86' for v in results),original_emulator_physical_disagreements=sum(v['compiler']=='recompiled-x86' and not v['physical_original_agrees'] for v in results))
 (out/'verification.json').write_text(json.dumps(verification,indent=2)+'\n');print('PASS',len(cases),'scenarios/',len(results),'physical-original/native comparisons')

if __name__=='__main__':main()
