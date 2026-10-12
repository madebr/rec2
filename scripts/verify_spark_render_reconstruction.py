#!/usr/bin/env python3
"""Verify complete spark consumer, both line-rendering branches and actual replay serializer.

Exact original drawing, consumer, projection, matrix, vector, random and FTOL
bytes run physically at PC53. Published replay recording bodies execute unchanged.
Only BRender callbacks and CRT rand are controlled. Raw native outputs are strict.
"""
import argparse,hashlib,json,os,struct,subprocess,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
SOURCE=ROOT/'src/carma2/newcommon/32-spark.c'
PIPE=ROOT/'src/carma2/newcommon/23-piping.c'
MAT=ROOT/'src/brender/core/math/matrix34.c'
MAT4=ROOT/'src/brender/core/math/matrix4.c'
VECTOR=ROOT/'src/brender/core/math/vector.c'
UTIL=ROOT/'src/carma2/newcommon/41-utility.c'
HASH='9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
SIZE=0x80000
ADDR=dict(RenderSparks=0x4f7450,SetLineModelCols=0x4f7cb0,DrawLine3D=0x4f6b80,DrawLine2D=0x4f6de0,AddSparkToPipingSession=0x4c6f20,
 BrMatrix4Perspective=0x536820,BrMatrix4Mul=0x535d40,BrMatrix4Scale=0x536030,BrMatrix4ApplyP=0x536990,
 BrMatrix34ApplyP=0x5334a0,BrMatrix34ApplyV=0x533520,BrMatrix34TApplyV=0x533690,BrVector3SetFloat=0x534590,FRandomBetween=0x5135b0,ftol=0x575b80,rand=0x576970,
 BrModelUpdate=0x51f950,BrZbsSceneRenderAdd=0x522ea0,BrActorRemove=0x51e070,BrActorAdd=0x51dfe0,
 ARStartPipingSession=0x4025e0,AREndPipingSession=0x402890,ARAddVariedDataToSession=0x402930,
 gSpark=0x6a9b80,gSpark_cam=0x6a82b0,gCameraToScreen=0x6a8718,gSpark_flags=0x6aa57c,gNo_2d_effects=0x74cf68,gLine_model=0x74cac8,gLine_actor=0x74ca34,
 gDont_render_actor=0x761ce4,gProgram_state=0x75bb80,gAction_replay_mode=0x676914,gPipe_buffer_start=0x676908,gLocal_buffer=0x6768b4,gMr_chunky=0x6768a0,gPipe_callbacks=0x6768e8,gPipe_buffer_phys_end=0x6768f0,gPipe_buffer_working_end=0x6768c4,gPipe_buffer_oldest=0x6768ac,gPipe_record_ptr=0x6768bc,gLocal_buffer_size=0x6768f4,gReentrancy_count=0x6768e4,gReentrancy_array=0x6768d0,gCamera_to_world=0x74d460,gRearview_camera_to_world=0x74d560,gRendering_mirror=0x74b778,gFog_shade_table=0x79ec38,gAcid_shade_table=0x79ec24)

def main():
 p=argparse.ArgumentParser(description=__doc__)
 for n,d in [('original',ROOT/'build-reference/originals/CARMA2_HW.EXE'),('output',ROOT/'build-audit/spark-render-tests'),('gcc',ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'),('msvc-root',ROOT/'build-tools/msvc-modern'),('msvc5-root',ROOT/'build-tools/msvc500sp3'),('dependency-dir',ROOT/'build-audit/joystick/python-deps')]:p.add_argument('--'+n,type=Path,default=d)
 p.add_argument('--symbols',type=Path);p.add_argument('--recompiled',type=Path);args=p.parse_args()
 sys.path.insert(0,str(args.dependency_dir.resolve()))
 from verify_tinted_native_reconstruction import extract
 from capstone import Cs,CS_ARCH_X86,CS_MODE_32
 from capstone.x86 import X86_OP_MEM
 original=args.original.read_bytes();assert hashlib.sha256(original).hexdigest()==HASH
 images=[(original,ADDR)]
 if args.symbols:
  mapping=json.loads(args.symbols.read_text());recompiled=args.recompiled.read_bytes()
  assert mapping['original_sha256']==HASH and mapping['recompiled_sha256']==hashlib.sha256(recompiled).hexdigest()
  symbols={v['orig']:v for v in mapping['symbols']};images.append((recompiled,{n:symbols[a]['recomp'] for n,a in ADDR.items()}))
 out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)

 from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
 from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_ECX,UC_X86_REG_EDX,UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_FPCW,UC_X86_REG_EBX,UC_X86_REG_ESI,UC_X86_REG_EDI,UC_X86_REG_EBP
 def execute(data,a,h,arena,sparks,mat4,front,rear):
  HEAP,STACK,STOP,CALLBACK=0x1000000,0x1100000,0x1180000,0x1082000
  u=Uc(UC_ARCH_X86,UC_MODE_32);u.mem_map(0x400000,0x500000);u.mem_map(HEAP,0x200000)
  pe=struct.unpack_from('<I',data,0x3c)[0];t=pe+24+struct.unpack_from('<H',data,pe+20)[0]
  for i in range(struct.unpack_from('<H',data,pe+6)[0]):
   _,rva,n,raw=struct.unpack_from('<IIII',data,t+40*i+8);u.mem_write(0x400000+rva,data[raw:raw+n])
  def wr(at,v):u.mem_write(at,struct.pack('<I',v&0xffffffff))
  def rd(at):return struct.unpack('<I',u.mem_read(at,4))[0]
  u.mem_write(HEAP,bytes(arena));u.mem_write(a['gSpark'],bytes(sparks));u.mem_write(a['gCameraToScreen'],mat4);u.mem_write(a['gCamera_to_world'],front);u.mem_write(a['gRearview_camera_to_world'],rear)
  u.mem_write(a['gProgram_state'],bytes(24272));wr(a['gProgram_state']+0x28,h[7])
  values=dict(gSpark_flags=h[1],gNo_2d_effects=h[2],gRendering_mirror=h[3],gAction_replay_mode=h[4],gSpark_cam=HEAP+0x700,gLine_model=HEAP+0x800,gLine_actor=HEAP+0x500,gDont_render_actor=HEAP+0x600,gAcid_shade_table=HEAP+0x200,gFog_shade_table=HEAP+0x300,gPipe_buffer_start=HEAP+0x70000 if h[6] else 0,gLocal_buffer=HEAP+0x60000,gMr_chunky=HEAP+0x60008,gPipe_buffer_phys_end=HEAP+0x78000,gPipe_buffer_working_end=HEAP+0x78000,gPipe_buffer_oldest=0,gPipe_record_ptr=HEAP+(h[17] or 0x70000),gPipe_callbacks=HEAP+0x65000,gLocal_buffer_size=8,gReentrancy_count=h[18])
  for n,v in values.items():wr(a[n],v)
  for i in range(5):wr(a['gReentrancy_array']+4*i,20+i)
  rng=0;callback_count=0;calls=[]
  global_names=['gSpark_flags','gNo_2d_effects','gRendering_mirror','gAction_replay_mode','gSpark_cam','gLine_model','gLine_actor','gDont_render_actor','gFog_shade_table','gAcid_shade_table','gPipe_buffer_start','gLocal_buffer','gMr_chunky','gPipe_callbacks','gPipe_buffer_phys_end','gPipe_buffer_working_end','gPipe_buffer_oldest','gPipe_record_ptr','gLocal_buffer_size','gReentrancy_count']
  def snapshot():
   return bytes(u.mem_read(HEAP,SIZE))+bytes(u.mem_read(a['gSpark'],2048))+b''.join(struct.pack('<I',rd(a[n])) for n in global_names)+struct.pack('<I',rd(a['gProgram_state']+0x28))+bytes(u.mem_read(a['gCameraToScreen'],64))+bytes(u.mem_read(a['gCamera_to_world'],48))+bytes(u.mem_read(a['gRearview_camera_to_world'],48))+bytes(u.mem_read(a['gReentrancy_array'],20))+struct.pack('<II',rng,callback_count)
  previous=snapshot()
  def delta():
   nonlocal previous
   now=snapshot();assert len(previous)==len(now);parts=[];i=0
   while i<len(now):
    if i%1024==0 and now[i:i+1024]==previous[i:i+1024]:i+=1024;continue
    if now[i]==previous[i]:i+=1;continue
    first=i
    while i<len(now) and now[i]!=previous[i]:i+=1
    parts.append('%08x:'%first+now[first:i].hex())
   previous=now;return ';'.join(parts)
  def mutation():
   nonlocal callback_count
   match=callback_count==h[10];callback_count+=1
   if not match:return
   if h[11]&1:wr(a['gSpark_flags'],h[12])
   if h[11]&2:u.mem_write(a['gSpark']+60,bytes([h[13]&255]))
   if h[11]&4:wr(a['gLine_actor'],HEAP+0x600)
   if h[11]&8:wr(a['gNo_2d_effects'],int(not rd(a['gNo_2d_effects'])))
   if h[11]&16:wr(a['gRendering_mirror'],int(not rd(a['gRendering_mirror'])))
   if h[11]&32:wr(a['gAction_replay_mode'],1)
   if h[11]&64:wr(a['gPipe_callbacks'],HEAP+0x65c00)
   if h[11]&128:wr(rd(a['gLocal_buffer'])+4,h[14])
  def hook(uc,ip,size,unused):
   nonlocal rng
   sp=uc.reg_read(UC_X86_REG_ESP)
   def ret(v=0,pop=0):uc.reg_write(UC_X86_REG_EAX,v);uc.reg_write(UC_X86_REG_ESP,sp+4+pop);uc.reg_write(UC_X86_REG_EIP,rd(sp))
   if ip==a['rand']:
    rng+=1;calls.append(['rand',h[9],0,delta()]);mutation();ret(h[9]);return
   if ip==CALLBACK:
    packet=uc.reg_read(UC_X86_REG_ECX);calls.append(['length',bytes(uc.mem_read(packet,28)).hex(),delta()]);mutation();ret();return
   if ip not in [a[n] for n in ['BrActorRemove','BrActorAdd','BrModelUpdate','BrZbsSceneRenderAdd']]:return
   x=rd(sp+4);y=rd(sp+8)
   if ip==a['BrActorRemove']:calls.append(['remove',x,0,delta()]);wr(x+12,0);mutation();ret(x)
   elif ip==a['BrActorAdd']:calls.append(['add',x,y,delta()]);wr(y+12,x);mutation();ret(y)
   elif ip==a['BrModelUpdate']:calls.append(['model',x,y&0xffff,delta()]);mutation();ret()
   else:calls.append(['render',x,0,delta()]);mutation();ret()
  u.hook_add(UC_HOOK_CODE,hook);u.reg_write(UC_X86_REG_FPCW,0x27f)
  x=HEAP+(h[15] or 0x3000);y=HEAP+(h[16] or 0x3010)
  if h[0]==0:name='RenderSparks';x=HEAP;y=HEAP+0x100;tail=[HEAP+0x400,a['gCamera_to_world'],h[5]]
  elif h[0]==1:name='DrawLine2D';tail=[HEAP,HEAP+0x100,h[8],HEAP+0x200]
  elif h[0]==2:name='DrawLine3D';tail=[HEAP,HEAP+0x100,HEAP+0x200]
  elif h[0]==3:name='SetLineModelCols';x=h[13]&255;y=0;tail=[]
  elif h[0]==4:name='AddSparkToPipingSession';x=h[19];y=HEAP+(h[15] or 0x3000);tail=[HEAP+(h[16] or 0x3010)]
  elif h[0]==5:name='BrMatrix4ApplyP';tail=[HEAP+(h[20] or 0x3200),HEAP+0x3000,a['gCameraToScreen']]
  else:name='BrMatrix4Mul';tail=[a['gCameraToScreen'],a['gCameraToScreen'] if h[20]==1 else HEAP+0x3300,a['gCameraToScreen'] if h[20]==2 else HEAP+0x3400]
  wr(STACK,STOP)
  for i,v in enumerate(tail):wr(STACK+4+4*i,v)
  u.reg_write(UC_X86_REG_ESP,STACK);u.reg_write(UC_X86_REG_ECX,x);u.reg_write(UC_X86_REG_EDX,y)
  saved=[UC_X86_REG_EBX,UC_X86_REG_ESI,UC_X86_REG_EDI,UC_X86_REG_EBP]
  for i,r in enumerate(saved):u.reg_write(r,0x12340000+i)
  u.emu_start(a[name],STOP,count=5000000);assert u.reg_read(UC_X86_REG_EIP)==STOP
  assert u.reg_read(UC_X86_REG_ESP)==STACK+4+(0 if h[0] in [5,6] else 4*len(tail))
  assert [u.reg_read(r) for r in saved]==[0x12340000+i for i in range(4)]
  value=u.reg_read(UC_X86_REG_EAX) if h[0] in [1,2] else 0
  return dict(calls=calls,state=delta(),value=value)
 bodies=[]
 for path,names in [(MAT,['BrMatrix34ApplyP','BrMatrix34ApplyV','Matrix34ApplyProductLoaded','Matrix34ApplyProductMemory','Matrix34ApplyAdd','BrMatrix34TApplyV']),(MAT4,['BrMatrix4Perspective','BrMatrix4Scale','BrMatrix4Mul','BrMatrix4ApplyP']),(VECTOR,['BrVector3SetFloat']),(UTIL,['UtilityRandomScalar','FRandomBetween']),(PIPE,['ReplayCopyForward','LengthOfSession','ARStartPipingSession','EndPipingSession2','AREndPipingSession','ARAddDataToSession','ARAddVariedDataToSession','AddSparkToPipingSession']),(SOURCE,['SmokeScalarRound','SmokeScalarBits','SparkFtol','SparkClipInteger','SparkClipDepth','DrawLine2D','DrawLine3D','SetLineModelCols','RenderSparks'])]:
  bodies.extend(extract(path.read_text(),n) for n in names)
 driver=r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <float.h>
#include <math.h>
#include "32-spark.h"
#include "23-piping.h"
#include "33-depth.h"
#include "41-utility.h"
#include "52-errors.h"
#include "globvars.h"
#include "rec2_macros.h"
#define A(x,y) A->m[x][y]
#define B(x,y) B->m[x][y]
#define C(x,y) C->m[x][y]
#define M(x,y) mat->m[x][y]
typedef char spark_layouts[sizeof(tSpark)==64&&offsetof(tSpark,time_sync)==52&&offsetof(tSpark,car)==56&&offsetof(tSpark,colour)==60&&sizeof(br_actor)==100&&offsetof(br_actor,type_data)==92&&offsetof(br_pixelmap,pixels)==8&&offsetof(br_pixelmap,row_bytes)==40&&offsetof(br_pixelmap,width)==52&&offsetof(br_model,vertices)==8&&sizeof(br_vertex)==40?1:-1];
static unsigned arena[0x80000/4],h[32];
static unsigned char previous[0x82000],now[0x82000];
static unsigned events,rng_count,callback_count;
tSpark gSpark[32];int gSpark_flags,gNo_2d_effects,gRendering_mirror,gAction_replay_mode;
br_camera*gSpark_cam;br_matrix4 gCameraToScreen;br_matrix34 gCamera_to_world,gRearview_camera_to_world;
br_model*gLine_model;br_actor*gLine_actor,*gDont_render_actor;br_pixelmap*gFog_shade_table,*gAcid_shade_table;
tU8*gPipe_buffer_start,*gLocal_buffer,*gMr_chunky,*gPipe_buffer_phys_end,*gPipe_buffer_working_end,*gPipe_buffer_oldest,*gPipe_record_ptr;
const tReplay_callback*gPipe_callbacks;tU32 gLocal_buffer_size;int gReentrancy_count;tPipe_chunk_type gReentrancy_array[5];tProgram_state gProgram_state;
static int C2_HOOK_FASTCALL test_length(tPipe_chunk*packet);
static unsigned norm(unsigned v){unsigned base=(unsigned)(uintptr_t)arena;if(v>=base&&v<=base+sizeof(arena))return v-base+0x1000000;if(v==(unsigned)(uintptr_t)test_length)return 0x1082000;return v;}
static unsigned rd(unsigned off){unsigned v;memcpy(&v,(char*)arena+off,4);return v;}
static void wr(unsigned off,unsigned v){memcpy((char*)arena+off,&v,4);}
static void*ptr(unsigned off){return (char*)arena+off;}
static void hex(const void*p,unsigned n){static const char digits[]="0123456789abcdef";unsigned i;const unsigned char*b=p;for(i=0;i<n;i++){putchar(digits[b[i]>>4]);putchar(digits[b[i]&15]);}}
static unsigned full(void){unsigned n=sizeof(arena),i,v;unsigned globals[21];tSpark sparks[32];
 memcpy(now,arena,n);
 {static unsigned offsets[]={8,0x108,0x208,0x308,0x45c,0x50c,0x60c,0x808,0x1010};for(i=0;i<sizeof(offsets)/sizeof(offsets[0]);i++){v=norm(rd(offsets[i]));memcpy(now+offsets[i],&v,4);}}
 for(i=0;i<71;i++){v=norm(rd(0x65000+24*i+8));memcpy(now+0x65000+24*i+8,&v,4);}
 memcpy(sparks,gSpark,sizeof(sparks));for(i=0;i<32;i++)sparks[i].car=(tCar_spec*)(uintptr_t)norm((unsigned)(uintptr_t)sparks[i].car);
 memcpy(now+n,sparks,sizeof(sparks));n+=sizeof(sparks);
 globals[0]=gSpark_flags;globals[1]=gNo_2d_effects;globals[2]=gRendering_mirror;globals[3]=gAction_replay_mode;
 globals[4]=norm((unsigned)(uintptr_t)gSpark_cam);globals[5]=norm((unsigned)(uintptr_t)gLine_model);globals[6]=norm((unsigned)(uintptr_t)gLine_actor);globals[7]=norm((unsigned)(uintptr_t)gDont_render_actor);globals[8]=norm((unsigned)(uintptr_t)gFog_shade_table);globals[9]=norm((unsigned)(uintptr_t)gAcid_shade_table);
 globals[10]=norm((unsigned)(uintptr_t)gPipe_buffer_start);globals[11]=norm((unsigned)(uintptr_t)gLocal_buffer);globals[12]=norm((unsigned)(uintptr_t)gMr_chunky);globals[13]=norm((unsigned)(uintptr_t)gPipe_callbacks);globals[14]=norm((unsigned)(uintptr_t)gPipe_buffer_phys_end);globals[15]=norm((unsigned)(uintptr_t)gPipe_buffer_working_end);globals[16]=norm((unsigned)(uintptr_t)gPipe_buffer_oldest);globals[17]=norm((unsigned)(uintptr_t)gPipe_record_ptr);globals[18]=gLocal_buffer_size;globals[19]=gReentrancy_count;globals[20]=gProgram_state.racing;
 memcpy(now+n,globals,sizeof(globals));n+=sizeof(globals);memcpy(now+n,&gCameraToScreen,64);n+=64;memcpy(now+n,&gCamera_to_world,48);n+=48;memcpy(now+n,&gRearview_camera_to_world,48);n+=48;memcpy(now+n,gReentrancy_array,20);n+=20;memcpy(now+n,&rng_count,4);n+=4;memcpy(now+n,&callback_count,4);n+=4;return n;}
static void delta(void){unsigned n=full(),i=0,first;int pieces=0;while(i<n){if(now[i]==previous[i]){i++;continue;}first=i;while(i<n&&now[i]!=previous[i])i++;printf("%s%08x:",pieces++?";":"",first);hex(now+first,i-first);}memcpy(previous,now,n);}
static void begin(const char*kind,unsigned a,unsigned b){printf("%s[\"%s\",%u,%u,\"",events++?",":"",kind,a,b);delta();printf("\"]");}
static void mutation(unsigned kind){if(callback_count++!=h[10])return;if(h[11]&1)gSpark_flags=h[12];if(h[11]&2)gSpark[0].colour=(tU8)h[13];if(h[11]&4)gLine_actor=(br_actor*)ptr(0x600);if(h[11]&8)gNo_2d_effects=!gNo_2d_effects;if(h[11]&16)gRendering_mirror=!gRendering_mirror;if(h[11]&32)gAction_replay_mode=1;if(h[11]&64)gPipe_callbacks=(tReplay_callback*)ptr(0x65c00);if(h[11]&128)((tPipe_chunk*)gLocal_buffer)->count=h[14];(void)kind;}
static int C2_HOOK_CDECL test_rand(void){unsigned result=h[9];rng_count++;begin("rand",result,0);mutation(1);return (int)result;}
br_actor*C2_HOOK_CDECL BrActorRemove(br_actor*actor){begin("remove",norm((unsigned)(uintptr_t)actor),0);actor->parent=NULL;mutation(2);return actor;}
br_actor*C2_HOOK_CDECL BrActorAdd(br_actor*parent,br_actor*actor){begin("add",norm((unsigned)(uintptr_t)parent),norm((unsigned)(uintptr_t)actor));actor->parent=parent;mutation(3);return actor;}
void C2_HOOK_CDECL BrModelUpdate(br_model*model,br_uint_16 flags){begin("model",norm((unsigned)(uintptr_t)model),flags);mutation(4);}
void C2_HOOK_CDECL BrZbsSceneRenderAdd(br_actor*actor){begin("render",norm((unsigned)(uintptr_t)actor),0);mutation(5);}
static int C2_HOOK_FASTCALL test_length(tPipe_chunk*packet){printf("%s[\"length\",\"",events++?",":"");hex(packet,28);printf("\",\"");delta();printf("\"]");mutation(6);return 0;}
void C2_NORETURN C2_HOOK_CDECL FatalError(int code,...){(void)code;abort();}
#define rand test_rand
'''
 driver+='\n\n'.join(bodies)+'\n#undef rand\n'
 def raw_at(address,size):
  pe=struct.unpack_from('<I',original,0x3c)[0];t=pe+24+struct.unpack_from('<H',original,pe+20)[0]
  for i in range(struct.unpack_from('<H',original,pe+6)[0]):
   _,rva,n,raw=struct.unpack_from('<IIII',original,t+40*i+8)
   if 0x400000+rva<=address<0x400000+rva+n:return original[raw+address-0x400000-rva:raw+address-0x400000-rva+size]
  raise AssertionError(hex(address))
 sizes=dict(RenderSparks=2134,SetLineModelCols=161,DrawLine3D=604,DrawLine2D=1637,AddSparkToPipingSession=50,BrMatrix4Perspective=186,BrMatrix4Mul=680,BrMatrix4Scale=70,BrMatrix4ApplyP=151,BrMatrix34ApplyP=116,BrMatrix34ApplyV=107,BrMatrix34TApplyV=107,BrVector3SetFloat=25,FRandomBetween=38,ftol=39)
 # Catalog lengths are authoritative, including the serializer's exact RET4.
 catalog={int(v['address'],16):v for v in map(json.loads,(ROOT/'build-audit/splash-next/functions.jsonl').read_text().splitlines())}
 for n in sizes:sizes[n]=catalog[ADDR[n]]['body_bytes']
 placement={};off=0
 for n,z in sizes.items():placement[n]=off;off=(off+z+63)&~63
 md=Cs(CS_ARCH_X86,CS_MODE_32);md.detail=True
 targets={ADDR[n]:'(unsigned)(uintptr_t)(code+%d)'%placement[n] for n in sizes}
 for n in ['ARStartPipingSession','AREndPipingSession','ARAddVariedDataToSession','BrActorRemove','BrActorAdd','BrModelUpdate','BrZbsSceneRenderAdd']:targets[ADDR[n]]='(unsigned)(uintptr_t)'+n
 targets[ADDR['rand']]='(unsigned)(uintptr_t)test_rand'
 data={ADDR[n]:'(unsigned)(uintptr_t)&'+n for n in ADDR if n.startswith('g')}
 ranges=[('gSpark',2048+64),('gCameraToScreen',64),('gCamera_to_world',48),('gRearview_camera_to_world',48)]
 constants={};patches=[];physical=[]
 def mapped(a,width):
  if a in data:return data[a]
  for n,z in ranges:
   if ADDR[n]<=a<ADDR[n]+z:return '(unsigned)(uintptr_t)&%s+%d'%(n,a-ADDR[n])
  assert 0x580000<=a<0x600000,hex(a)
  constants[a,width]=raw_at(a,width);return '(unsigned)(uintptr_t)constant_%x_%d'%(a,width)
 driver+='\n#ifdef PHYSICAL_ORIGINAL\n'
 for n,z in sizes.items():
  raw=raw_at(ADDR[n],z);physical.append(dict(name=n,address=ADDR[n],body_bytes=z,sha256=hashlib.sha256(raw).hexdigest(),hex=raw.hex()))
  driver+='static const unsigned char physical_%s[]={%s};\n'%(n,','.join(map(str,raw)))
  for ins in md.disasm(raw,ADDR[n]):
   at=placement[n]+ins.address-ADDR[n]
   if ins.mnemonic=='call':
    a=ins.operands[0].imm;assert a in targets,(n,hex(a))
    patches.append('patch(code+%d,%s-(unsigned)(uintptr_t)(code+%d));'%(at+ins.imm_offset,targets[a],at+ins.size))
   else:
    for op in ins.operands:
     if op.type==X86_OP_MEM and op.mem.disp>=0x400000:patches.append('patch(code+%d,%s);'%(at+ins.disp_offset,mapped(op.mem.disp,op.size)))
     elif op.type!=X86_OP_MEM and ins.imm_size==4 and ins.mnemonic in ['mov','cmp','push'] and 0x600000<=op.imm<0x800000:patches.append('patch(code+%d,%s);'%(at+ins.imm_offset,mapped(op.imm,4)))
 for (a,z),raw in constants.items():driver+='static const unsigned char constant_%x_%d[]={%s};\n'%(a,z,','.join(map(str,raw)))
 driver+='''
__declspec(dllimport) void* __stdcall VirtualAlloc(void*,unsigned,unsigned,unsigned);
static unsigned char*physical_base;
static void patch(unsigned char*p,unsigned v){memcpy(p,&v,4);}
static void physical_init(void){unsigned char*code=VirtualAlloc(NULL,16384,0x3000,0x40);if(!code)abort();physical_base=code;
'''
 for n,z in placement.items():driver+='memcpy(code+%d,physical_%s,sizeof(physical_%s));\n'%(z,n,n)
 driver+='\n'.join(patches)+'\n}\n'
 for n in ['RenderSparks','SetLineModelCols','DrawLine3D','DrawLine2D','AddSparkToPipingSession','BrMatrix4ApplyP','BrMatrix4Mul']:
  signatures=dict(RenderSparks='void(C2_HOOK_FASTCALL*)(br_pixelmap*,br_pixelmap*,br_actor*,const br_matrix34*,tU32)',SetLineModelCols='void(C2_HOOK_FASTCALL*)(tU8)',DrawLine3D='int(C2_HOOK_FASTCALL*)(const br_vector3*,const br_vector3*,br_pixelmap*,br_pixelmap*,br_pixelmap*)',DrawLine2D='int(C2_HOOK_FASTCALL*)(br_vector3*,br_vector3*,br_pixelmap*,br_pixelmap*,br_scalar,br_pixelmap*)',AddSparkToPipingSession='void(C2_HOOK_FASTCALL*)(int,const br_vector3*,const br_vector3*)',BrMatrix4ApplyP='void(C2_HOOK_CDECL*)(br_vector4*,const br_vector3*,const br_matrix4*)',BrMatrix4Mul='void(C2_HOOK_CDECL*)(br_matrix4*,const br_matrix4*,const br_matrix4*)')
  driver+='#define %s ((%s)(physical_base+%d))\n'%(n,signatures[n],placement[n])
 driver+='\n#endif\n'
 driver+=r'''
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned i,v,n;int value=0;float brightness;if(argc!=2)return 1;f=fopen(argv[1],"rb");if(!f)return 2;if(fread(h,sizeof(h),1,f)!=1||fread(arena,sizeof(arena),1,f)!=1||fread(gSpark,sizeof(gSpark),1,f)!=1||fread(&gCameraToScreen,64,1,f)!=1||fread(&gCamera_to_world,48,1,f)!=1||fread(&gRearview_camera_to_world,48,1,f)!=1)return 3;fclose(f);_controlfp(_PC_53,_MCW_PC);
#ifdef PHYSICAL_ORIGINAL
 physical_init();
#endif
 {static unsigned offsets[]={8,0x108,0x208,0x308,0x45c,0x808,0x1010};for(i=0;i<sizeof(offsets)/sizeof(offsets[0]);i++){v=rd(offsets[i]);wr(offsets[i],(unsigned)(uintptr_t)arena+v-0x1000000);}}
 for(i=0;i<32;i++)if(gSpark[i].car)gSpark[i].car=(tCar_spec*)((char*)arena+(unsigned)(uintptr_t)gSpark[i].car-0x1000000);
 for(i=0;i<71;i++){unsigned off=0x65000+24*i+8;if(rd(off)==0x1082000)wr(off,(unsigned)(uintptr_t)test_length);off=0x65c00+24*i+8;if(rd(off)==0x1082000)wr(off,(unsigned)(uintptr_t)test_length);}
 gSpark_flags=h[1];gNo_2d_effects=h[2];gRendering_mirror=h[3];gAction_replay_mode=h[4];gSpark_cam=ptr(0x700);gLine_model=ptr(0x800);gLine_actor=ptr(0x500);gDont_render_actor=ptr(0x600);gAcid_shade_table=ptr(0x200);gFog_shade_table=ptr(0x300);
 gPipe_buffer_start=h[6]?ptr(0x70000):NULL;gLocal_buffer=ptr(0x60000);gMr_chunky=ptr(0x60008);gPipe_buffer_phys_end=ptr(0x78000);gPipe_buffer_working_end=gPipe_buffer_phys_end;gPipe_buffer_oldest=NULL;gPipe_record_ptr=ptr(h[17]?h[17]:0x70000);gPipe_callbacks=ptr(0x65000);gLocal_buffer_size=8;gReentrancy_count=h[18];gProgram_state.racing=h[7];for(i=0;i<5;i++)gReentrancy_array[i]=(tPipe_chunk_type)(20+i);
 n=full();memcpy(previous,now,n);memcpy(&brightness,h+8,4);printf("{\"calls\":[");
 if(h[0]==0)RenderSparks(ptr(0),ptr(0x100),ptr(0x400),&gCamera_to_world,h[5]);
 else if(h[0]==1)value=DrawLine2D(ptr(h[15]?h[15]:0x3000),ptr(h[16]?h[16]:0x3010),ptr(0),ptr(0x100),brightness,ptr(0x200));
 else if(h[0]==2)value=DrawLine3D(ptr(h[15]?h[15]:0x3000),ptr(h[16]?h[16]:0x3010),ptr(0),ptr(0x100),ptr(0x200));
 else if(h[0]==3)SetLineModelCols((tU8)h[13]);
 else if(h[0]==5)BrMatrix4ApplyP(ptr(h[20]?h[20]:0x3200),ptr(0x3000),&gCameraToScreen);
 else if(h[0]==6)BrMatrix4Mul(&gCameraToScreen,h[20]==1?&gCameraToScreen:ptr(0x3300),h[20]==2?&gCameraToScreen:ptr(0x3400));
 else AddSparkToPipingSession(h[19],ptr(h[15]?h[15]:0x3000),ptr(h[16]?h[16]:0x3010));
 printf("],\"state\":\"");delta();printf("\",\"value\":%d}\n",value);return 0;
}
'''
 harness=out/'harness.c';harness.write_text(driver)
 includes=[ROOT/r for r in ['src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/carma2/platform','src/carma2/s3/include','src/brender/include']]
 msvc=sorted((args.msvc_root.resolve()/'VC/Tools/MSVC').iterdir())[-1];sdk=args.msvc_root.resolve()/'Windows Kits/10';version=sorted(p.name for p in (sdk/'Include').iterdir())[-1]
 env=dict(os.environ);env['PATH']=str(msvc/'bin/Hostx64/x86')+os.pathsep+env['PATH'];env['INCLUDE']=os.pathsep.join(map(str,[msvc/'include',*[sdk/f'Include/{version}/{r}' for r in ['ucrt','shared','um']]]));env['LIB']=os.pathsep.join(map(str,[msvc/'lib/x86',*[sdk/f'Lib/{version}/{r}/x86' for r in ['ucrt','um']]]))
 old=args.msvc5_root.resolve();oldenv=dict(os.environ);oldenv['PATH']=str(old/'bin')+os.pathsep+str(old/'redist')+os.pathsep+oldenv['PATH'];oldenv['INCLUDE']=str(old/'include');oldenv['LIB']=str(old/'lib')
 commands=dict(msvc=[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(r) for r in includes],str(harness),'/Fo'+str(out/'msvc.obj'),'/Fe'+str(out/'msvc.exe')],msvc5=[str(old/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(r) for r in includes],str(harness),'/Fo'+str(out/'msvc5.obj'),'/Fe'+str(out/'msvc5.exe')],mingw=[str(args.gcc.resolve()),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-fno-strict-aliasing',*['-I'+str(r) for r in includes],str(harness),'-o',str(out/'mingw.exe')])
 commands['physical-original']=commands['msvc5'][:-2]+['/DPHYSICAL_ORIGINAL','/Fo'+str(out/'physical-original.obj'),'/Fe'+str(out/'physical-original.exe'),'/link','kernel32.lib']
 for compiler,cmd in commands.items():
  build=subprocess.run(cmd,cwd=out,env=env if compiler=='msvc' else oldenv if compiler in ['msvc5','physical-original'] else None,text=True,capture_output=True);(out/(compiler+'-build.log')).write_text(build.stdout+build.stderr);assert build.returncode==0,build.stdout+build.stderr
 cases=[dict(mode=0,flags=0,fov=a,aspect=1.234567,hither=.1234567,yon=29.12345) for a in [1,17,1234,8192,12000,16383,16384,20000,32767,50000,65535]]
 for mode in [0,2]:
  for hardware in [0,1]:
   for replay in [0,1]:
    for car in [0,1]:cases.append(dict(mode=mode,hardware=hardware,replay=replay,car=car))
 for mode in [1,2]:
  for ends in [(0,0,0,0),(-10,-3,10,3),(10,3,-10,-3),(-3,-10,3,10),(3,10,-3,-10),(-50,-40,50,40),(-50,20,-45,30),(50,-20,45,30),(-5,50,10,40),(5,-50,-10,-40),(-40,-1,1,15),(20,1,40,5),(-3,3,3,3),(3,-3,3,3)]:cases.append(dict(mode=mode,ends=ends))
 for colour in [0,1,2,255]:cases.append(dict(mode=3,colour=colour))
 for owner in [0,1,0x12345678,0x80000000,0xffffffff]:
  for gate in [0,1,2,3]:cases.append(dict(mode=4,owner=owner,gate=gate))
 for mode in [0,4]:
  for mutation in [1,2,4,8,16,32,64,128]:cases.append(dict(mode=mode,hardware=1,mutation=mutation))
 for mode in [5,6]:
  for seed in range(12):cases.append(dict(mode=mode,seed=seed))
 for replay in [0,1]:
  for time in [0,1,16777217,0x80000001,0xffffffff]:cases.append(dict(mode=0,hardware=1,replay=replay,time=time,sync=0x81234567))
 for mode in [0,2,4]:
  for alias in [0,4,8]:cases.append(dict(mode=mode,hardware=1,alias=alias,length=1))
 for mode in [0,1,2,4]:
  for bits in [0x80000000,1,0x7f800000,0xff800000,0x7fc12345,0xffc54321,0x7f800001,0xff801234]:
   cases.append(dict(mode=mode,hardware=1 if mode in [0,2] else 0,bits=bits,replay=1 if mode==0 else 0,length=1))
 for z0,z1 in [(.5,-5),(-5,.5),(.5,.8),(-1,-1),(-.9999999,-4),(-1.0000001,-4),(-5,-100),(-.1,-.1)]:cases.append(dict(mode=2,z0=z0,z1=z1))
 for b in [-1,0,.0009,.5,1,1.0001]:cases.append(dict(mode=1,brightness=b))
 for bits in [0x7fc12345,0x7f800001,0x7f800000,0xff800000]:cases.append(dict(mode=1,brightness_bits=bits))
 for count in [-1,0,1,999,1000,1200,0x7fffffff]:cases.append(dict(mode=0,hardware=1,count=count))
 for mode in [1,2]:
  for alias in [0,4,8]:cases.append(dict(mode=mode,alias=alias,hardware=1 if mode==2 else 0))
 for seed in range(24):cases.append(dict(mode=0,hardware=1,random_particle=seed,time=17*seed+1,sync=13*seed))
 for alias in [1,2]:cases.append(dict(mode=6,seed=2,math_alias=alias))
 for alias in [0x3000,0x3004,0x3008]:cases.append(dict(mode=5,seed=3,math_alias=alias))
 for mode in [5,6]:
  for bits in [0x7fc12345,0xffc54321,0x7f800001,0xff801234]:cases.append(dict(mode=mode,seed=4,bits=bits))
 for alias in [0x60008,0x6000c]:cases.append(dict(mode=4,pipe_alias=alias,length=1))
 for mode in [0,4]:
  for wrap in [0x77ff0,0x77ffe]:cases.append(dict(mode=mode,hardware=1,record_ptr=wrap))
 for reentrant in [1,2]:cases.append(dict(mode=0,hardware=1,reentrant=reentrant))
 for mutation in [4,8,16]:cases.append(dict(mode=2,hardware=1,mutation=mutation))
 for bits in [0x7fc12345,0x7f800001,0xff801234,0x7f800000]:cases.append(dict(mode=0,hardware=1,bits=bits,active_bits=True,length=1))
 for mode in [1,2]:
  for row in [53,-65]:cases.append(dict(mode=mode,row=row,depth_row=-129,origins=True))
 for bits in [0x4f800000,0x4f800001,0x5f000000,0xdf000000]:cases.append(dict(mode=1,ftol_bits=bits))
 for width,height,row in [(0x8001,1,32767),(1,0x8001,1)]:cases.append(dict(mode=1,width=width,height=height,row=row,depth_row=2,ends=(0,0,0,0)))
 for a,b in [(0x8fc,0x924),(0x900,0x928),(0x904,0x92c),(0x924,0x900)]:cases.append(dict(mode=2,hardware=1,model_alias=(a,b)))
 cases.append(dict(mode=4,owner=0x1001000,length=1))
 for hardware in [0,1]:
  for replay in [0,1]:
   for car in [0,1]:cases.append(dict(mode=0,hardware=hardware,replay=replay,car=car,mirror=1))
 def f(v):return struct.unpack('<I',struct.pack('<f',v))[0]
 results=[]
 for index,case in enumerate(cases):
  h=[0]*32;h[0]=case.get('mode',0);h[1]=case.get('flags',1);h[2]=case.get('hardware',0);h[3]=case.get('mirror',0);h[4]=case.get('replay',0);h[5]=case.get('time',16);h[6]=int(case.get('gate',0)!=1);h[7]=int(case.get('gate',0)!=3);h[8]=case.get('brightness_bits',f(case.get('brightness',1)));h[9]=case.get('rand',12345);h[10]=case.get('callback',0);h[11]=case.get('mutation',0);h[12]=0x80000001;h[13]=case.get('colour',255);h[14]=7;h[19]=case.get('owner',0xdeadbeef);h[20]=case.get('math_alias',0);h[17]=case.get('record_ptr',0);h[18]=case.get('reentrant',0)
  if case.get('gate')==2:h[4]=1
  if 'alias' in case:h[15]=0x3000;h[16]=0x3000+case['alias']
  if 'pipe_alias' in case:h[15]=case['pipe_alias'];h[16]=case['pipe_alias']+4
  if 'model_alias' in case:h[15],h[16]=case['model_alias']
  arena=bytearray(SIZE);sparks=bytearray([0xa5])*2048
  def w(off,v):struct.pack_into('<I',arena,off,v&0xffffffff)
  for off,pixels,row,width,height in [(0,0x4800 if case.get('row',64)<0 else 0x4000,case.get('row',64),case.get('width',32),case.get('height',32)),(0x100,0x8800 if case.get('depth_row',128)<0 else 0x8000,case.get('depth_row',128),case.get('width',32),case.get('height',32)),(0x200,0x10000,256,256,16),(0x300,0x10000,256,256,16)]:
   w(off+8,0x1000000+pixels);struct.pack_into('<h',arena,off+40,row);struct.pack_into('<HHHH',arena,off+48,0,0,width,height)
  arena[0x4000:0x5000]=bytes((i*17+63)&255 for i in range(0x1000));arena[0x8000:0x9000]=bytes([255])*0x1000
  arena[0x10000:0x50000]=bytes((i*7+(i>>8))&255 for i in range(0x40000))
  w(0x45c,0x1000700);struct.pack_into('<H',arena,0x706,case.get('fov',16384));w(0x708,f(case.get('hither',1)));w(0x70c,f(case.get('yon',100)));w(0x710,f(case.get('aspect',1.25)));w(0x808,0x1000900);w(0x1010,0x1000a00)
  identity=struct.pack('<12f',1,0,0,0,1,0,0,0,1,0,0,0);arena[0xa2c:0xa5c]=identity
  ends=case.get('ends',(-3,-2,3,2));struct.pack_into('<3f',arena,0x3000,ends[0],ends[1],case.get('z0',-.5 if h[0]==1 else -5));struct.pack_into('<3f',arena,0x3010,ends[2],ends[3],case.get('z1',.5 if h[0]==1 else -5))
  for i in range(32):
   struct.pack_into('<i12fII',sparks,64*i,case.get('count',900),-.5,.3,-5,1.234567,-2.345678,.456789,-.1,.2,0,0,1,0,case.get('sync',0),0x1001000 if case.get('car',0) else 0);sparks[64*i+60]=case.get('colour',i&1)
  for off in [0x65000,0x65c00]:
   for j in range(71):w(off+24*j+12,24)
   if case.get('mutation') or case.get('length'):w(off+24*10+8,0x1082000)
  w(0x60000,10);w(0x60004,0)
  if case.get('origins'):
   struct.pack_into('<HH',arena,48,2,3);struct.pack_into('<HH',arena,0x100+48,7,9);struct.pack_into('<HH',arena,0x200+48,17,3)
  if 'ftol_bits' in case:w(0x3000,case['ftol_bits'])
  if 'bits' in case:
   for off in [0x3000,0x3010]:w(off,case['bits']);w(off+4,0x7f800001);w(off+8,0xff801234)
   struct.pack_into('<I',sparks,4,case['bits']);struct.pack_into('<I',sparks,8,0x7f800001);struct.pack_into('<I',sparks,12,0xff801234)
   if case.get('active_bits'):struct.pack_into('<I',sparks,16,case['bits'])
  if 'random_particle' in case:
   import random
   r=random.Random(case['random_particle'])
   struct.pack_into('<9f',sparks,4,*[r.uniform(-7,7) for _ in range(9)])
   struct.pack_into('<3f',sparks,40,*[r.uniform(-1,1) for _ in range(3)])
  mat4=struct.pack('<16f',1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1)
  if 'seed' in case:
   import random
   randomizer=random.Random(case['seed'])
   vals=[randomizer.uniform(-17,19) for _ in range(32)]
   mat4=struct.pack('<16f',*vals[:16]);arena[0x3300:0x3380]=struct.pack('<32f',*vals);arena[0x3400:0x3440]=mat4
   if 'bits' in case:
    mat4=struct.pack('<4I',case['bits'],0xffc54321,0x7f800001,0xff801234)*4;arena[0x3300:0x3340]=mat4;arena[0x3400:0x3440]=mat4
  if case.get('mutation')==64:w(0x65c00+24*10+12,20)
  if 'model_alias' in case:
   for j in range(16):w(0x8fc+4*j,[0x7f800001,0xff801234,0x3f123456,0x80000000][j%4])
  rear=struct.pack('<12f',-1,0,0,0,1,0,0,0,-1,.1234567,-.7654321,.3456789)
  fixture=out/('case-%04d.bin'%index);fixture.write_bytes(struct.pack('<32I',*h)+arena+sparks+mat4+identity+rear)
  expected=json.loads(subprocess.check_output([str(out/'physical-original.exe'),str(fixture)],text=True))
  checks=[(compiler,json.loads(subprocess.check_output([str(out/(compiler+'.exe')),str(fixture)],text=True))) for compiler in ['msvc5','msvc','mingw']]
  emulator=execute(*images[0],h,arena,sparks,mat4,identity,rear)
  if emulator!=expected:
   failure=out/('failure-original-emulator-%d.json'%index);failure.write_text(json.dumps(dict(case=case,physical=expected,emulator=emulator),indent=2));raise AssertionError('physical/original emulator discrepancy: '+str(failure))
  if len(images)>1:checks.append(('recompiled-x86',execute(*images[1],h,arena,sparks,mat4,identity,rear)))
  for compiler,actual in checks:
   if actual!=expected:
    failure=out/('failure-%s-%d.json'%(compiler,index));failure.write_text(json.dumps(dict(case=case,original=expected,actual=actual),indent=2));raise AssertionError('%s case%d differs: %s'%(compiler,index,failure))
   results.append(dict(case=case,compiler=compiler,passed=True))
 verification=dict(scenario_count=len(cases),comparisons=len(results),original_sha256=HASH,source_sha256=hashlib.sha256(SOURCE.read_bytes()).hexdigest(),piping_source_sha256=hashlib.sha256(PIPE.read_bytes()).hexdigest(),matrix4_source_sha256=hashlib.sha256(MAT4.read_bytes()).hexdigest(),header_sha256=hashlib.sha256((ROOT/'src/carma2/newcommon/32-spark.h').read_bytes()).hexdigest(),script_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),bodies_sha256=[hashlib.sha256(b.encode()).hexdigest() for b in bodies],physical_executable_sha256=hashlib.sha256((out/'physical-original.exe').read_bytes()).hexdigest(),limitations=['PC53 only; sticky FPU exception flags are outside observed game state.','Accessible bounded screen/depth backing storage and shade table rows, valid camera/car/model graph. Actual software division/trap and nonterminating fade configurations are outside the terminating fixture envelope; production retains them.','The replay-only unwritten private endpoint is transformed then overwritten; no externally observed bytes are masked.','Original/matching PE closures execute in Unicorn; physical original instruction bytes independently validate each scenario, including all NaN output bits. Published recording dependencies run actual source bodies in native and physical fixtures.'],new_original_entries=['RenderSparks4f7450','DrawLine3D4f6b80','DrawLine2D4f6de0','SetLineModelCols4f7cb0','AddSparkToPipingSession4c6f20'],emulator_physical_crosschecks=len(cases),compiler_commands=commands,physical_original=dict(bodies=physical,patches=patches,precision_control=53),oracle_correction=None,cases=results)
 if args.symbols:verification.update(recompiled_sha256=hashlib.sha256(args.recompiled.read_bytes()).hexdigest(),symbols_sha256=hashlib.sha256(args.symbols.read_bytes()).hexdigest())
 (out/'verification.json').write_text(json.dumps(verification,indent=2));print('PASS',len(cases),len(results))
if __name__=='__main__':main()



