"""Complete707B tinted material updater, physical original/native3/current PE.
Clock, pixel provider and renderer-update API boundaries are controlled inputs
and observed effects, as in the retained tinted runtime fixture. No game/math
callee is substituted. Fixed arena pointers, full raw bytes/events; PC53/ABI.
"""
import argparse,json,random,struct,subprocess,sys,re
from pathlib import Path
R=next(p for p in Path(__file__).resolve().parents if (p/'src/carma2').is_dir());D=Path(__file__).resolve().parent;sys.path.insert(0,str(R/'scripts'))
import verify_phil_registry_reconstruction as registry
from verify_opponent_route_reconstruction import raw,pe,HASH,sha,words
from verify_tinted_native_reconstruction import extract
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM,CS_GRP_CALL,CS_GRP_JUMP
BASE=0x25000000;SIZE=0x50000
GLOBALS={0x705c80:(0,10*0x6450),0x65e870:(0x40000,4),0x6a0438:(0x40004,4),0x6a043c:(0x40008,4)}
BOUNDARIES={'time':(0x514c30,60),'pixel':(0x538840,56),'material':(0x520e70,15)}
COMMON=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#define BASE 0x25000000u
#include "66-tintedpoly.h"
#include "41-utility.h"
#include "rec2_macros.h"
static unsigned char*arena;static unsigned entry,h[8],time_calls,pixel_calls,material_calls;
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[5],builder_stack_count,builder_result,builder_poison,builder_cdecl;
extern unsigned __cdecl BuilderInvoke(void);
#define gTintedPolys (*(tTintedPoly(*)[10])(arena))
#define gTinted_poly_tokens ((br_token_value*)(arena+0x3ffd0))
#define gTintedColourMap_grn (*(br_uint_32*)(arena+0x40004))
#define gTintedColourMap_blu (*(br_uint_32*)(arena+0x40008))
typedef char layouts[sizeof(void*)==4&&sizeof(tTintedPoly)==0x6450&&sizeof(br_material)==156&&sizeof(br_pixelmap)==68&&offsetof(tTintedPoly,material)==4&&offsetof(tTintedPoly,color_red)==12&&offsetof(tTintedPoly,color_grn)==16&&offsetof(tTintedPoly,color_blu)==20&&offsetof(tTintedPoly,material2)==44&&offsetof(tTintedPoly,visible)==52&&offsetof(tTintedPoly,lastTime)==60&&offsetof(br_material,colour_map)==64&&offsetof(br_pixelmap,pixels)==8&&offsetof(br_pixelmap,width)==52&&offsetof(br_pixelmap,height)==54?1:-1];
static void event(unsigned code,unsigned a,unsigned b,unsigned c) {unsigned x[4];x[0]=code;x[1]=a;x[2]=b;x[3]=c;fwrite(x,16,1,stdout);fwrite(arena,0x50000,1,stdout);fflush(stdout);}
static void mutate(unsigned phase,unsigned call) {
 if(h[3]==phase&&h[4]==call){memcpy(arena+h[5],&h[6],4);}
}
tU32 C2_HOOK_FASTCALL GetTotalTime(void){unsigned v=h[1]+4096*time_calls;event(1,v,time_calls,0);mutate(1,time_calls++);return v;}
br_colour C2_HOOK_CDECL BrPixelmapPixelGet(br_pixelmap*map,br_int_32 x,br_int_32 y){unsigned i=((unsigned)map-BASE-0x43000)/68;unsigned c=0xff000000u|(((unsigned)x*17+i*7)&255)<<16|(((unsigned)y*19+i*11)&255)<<8|(((unsigned)x*3+(unsigned)y*5+i*13)&255);event(2,(unsigned)map,(unsigned)x,(unsigned)y);mutate(2,pixel_calls++);return c;}
void C2_HOOK_CDECL BrMaterialUpdate(br_material*mat,br_uint_16 flags){event(3,(unsigned)mat,flags,mat->colour);mutate(3,material_calls++);}
static LONG WINAPI fault(EXCEPTION_POINTERS*e){if(e->ExceptionRecord->ExceptionCode!=EXCEPTION_ACCESS_VIOLATION)return EXCEPTION_CONTINUE_SEARCH;event(9,e->ExceptionRecord->ExceptionCode,(unsigned)e->ExceptionRecord->ExceptionInformation[1],(unsigned)e->ExceptionRecord->ExceptionInformation[0]);ExitProcess(0);return EXCEPTION_CONTINUE_SEARCH;}
static void observer(void){typedef void*(WINAPI*fn)(ULONG,LONG(WINAPI*)(EXCEPTION_POINTERS*));FARPROC p=GetProcAddress(GetModuleHandleA("kernel32.dll"),"AddVectoredExceptionHandler");fn f;memcpy(&f,&p,4);if(!f||!f(1,fault))exit(38);}
static int run(const char*path){FILE*f;unsigned i;SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);if(VirtualAlloc((void*)0x24ff0000,0x70000,MEM_RESERVE,PAGE_NOACCESS)!=(void*)0x24ff0000)return 39;arena=VirtualAlloc((void*)0x25000000,0x50000,MEM_COMMIT,PAGE_READWRITE);if(arena!=(void*)0x25000000)return 11;f=fopen(path,"rb");if(!f||fread(h,sizeof(h),1,f)!=1||fread(arena,0x50000,1,f)!=1)return 12;fclose(f);_setmode(_fileno(stdout),_O_BINARY);observer();_controlfp(_PC_53,_MCW_PC);_clearfp();for(i=0;i<h[2];i++){builder_address=entry;builder_x=h[0];builder_y=0x21436587;builder_mode=1;builder_poison=0xcccccccc;
#ifdef REC2_MATCHING
 builder_cdecl=0;builder_stack_count=0;
#else
 builder_cdecl=1;builder_stack_count=1;builder_stack[0]=h[0];
#endif
 if(BuilderInvoke())return 14;event(0,i,0,0);}return 0;}
'''
def packet(binary,start,n,globals_,boundaries,out,name):
 nt,base,size,tab=pe(binary);cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;patch={};instructions={};todo=[start];calls=[];allowed=set(boundaries.values())
 def translation(v):
  for old,(off,z)in globals_.items():
   if old<=v<old+z:return BASE+off+v-old,1
  assert base<=v<base+size,hex(v);return v-base,0
 while todo:
  a=todo.pop()
  while a not in instructions:
   assert start<=a<start+n;ins=next(cs.disasm(raw(binary,a,min(15,start+n-a)),a));instructions[a]=ins.bytes.hex()
   control=CS_GRP_CALL in ins.groups or CS_GRP_JUMP in ins.groups
   if control:
    op=ins.operands[0];assert op.type==CS_OP_IMM
    if CS_GRP_CALL in ins.groups:assert op.imm in allowed;calls.append((a,op.imm))
    else:
     assert start<=op.imm<start+n;todo.append(op.imm)
     if ins.mnemonic=='jmp':break
   for op in ins.operands:
    if op.type==CS_OP_MEM and base<=op.mem.disp<base+size:
     assert ins.disp_size==4;patch[a+ins.disp_offset-base]=(op.mem.disp,*translation(op.mem.disp))
    elif op.type==CS_OP_IMM and not control and base<=op.imm<base+size:
     assert ins.imm_size==4;patch[a+ins.imm_offset-base]=(op.imm,*translation(op.imm))
   if ins.mnemonic.startswith('ret'):break
   a+=ins.size
 words_=[start-base,*[boundaries[k]-base for k in('time','pixel','material')],len(patch)]+[v for at,x in sorted(patch.items())for v in(at,*x)]
 bits=words(words_);(out/(name+'-packet.bin')).write_bytes(bits)
 return dict(address=start,extent=n,instructions=sorted(instructions.items()),calls=calls,patches=[(at,*x)for at,x in sorted(patch.items())],packet_sha256=sha(bits),boundaries=boundaries)
def cases():
 result=[]
 def add(**kw):
  c=dict(index=0,visible=1,map=0,red=0,green=0,blue=0,target=(128,73,29),time=9000,last=0,repeat=1,width=16,height=12,mutation=(0,0,0,0),seed=len(result)+1);c.update(kw);result.append(c)
 for map_ in(0,1,2,3):
  for rgb in((0,0,0),(10,90,200),(128,73,29),(0xffffffff,1,0)):
   for time,last in((4095,0),(4096,0),(0,8192),(0xffffffff,0x80000000)):add(map=map_,red=rgb[0],green=rgb[1],blue=rgb[2],time=time,last=last,repeat=2)
 add(visible=0);add(index=9,map=1,red=127,green=74,blue=29)
 for index in(0,9):
  for dims in((0,0),(1,1),(65535,32768),(3,5),(16,12)):add(index=index,width=dims[0],height=dims[1],red=33,green=44,blue=55)
 for t in(0,1,4095,4096,4097,0x7fffffff,0x80000000,0xffffffff):
  for v in(0,1,255,0x7fffffff,0x80000000,0xffffffff):add(map=1,red=v,green=v^0x3f,blue=v^0xa5,time=t,target=(v^0x100,v^0x80,v^0x10))
 for phase in(1,2,3):
  for call in range(4):
   for at,value in((0x40004,0xffffffff),(0x2c,0),(0x34,0),(0x42040,BASE+0x43000+68),(0x43034,0x00030005),(0xc,0xffffffff)):
    add(red=17,green=18,blue=19,mutation=(phase,call,at,value))
 return result
def fixture(c):
 r=random.Random(c['seed']);arena=bytearray(r.randbytes(SIZE))
 def put(at,v):arena[at:at+4]=words([v])
 for i in range(10):
  p=i*0x6450;put(p+4,BASE+0x41000+i*156);put(p+0x2c,0 if c['map']==3 else BASE+0x42000+i*156);put(p+0x34,c['visible']);put(p+0xc,c['red']);put(p+0x10,c['green']);put(p+0x14,c['blue']);put(p+0x3c,c['last'])
  put(0x42000+i*156+0x40,0 if c['map']==1 else BASE+0x43000+i*68);put(0x43000+i*68+8,0 if c['map']==2 else BASE+0x48000);struct.pack_into('<HH',arena,0x43000+i*68+0x34,c['width'],c['height'])
 for off,v in zip((0x40000,0x40004,0x40008),c['target']):put(off,v)
 return words([c['index'],c['time'],c['repeat'],*c['mutation'],c['seed']])+arena
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',required=True,type=Path);p.add_argument('--original',type=Path,default=R/'build-reference/originals/CARMA2_HW.EXE');p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);p.add_argument('--candidate',type=Path);args=p.parse_args();assert bool(args.recompiled)==bool(args.symbols),'Supply both current PE and symbols';out=args.output.resolve();assert not out.exists();out.mkdir(parents=True)
 source=(args.candidate or R/'src/carma2/newcommon/66-tintedpoly.c').resolve()
 paths=[source,Path(__file__),R/'src/carma2/newcommon/66-tintedpoly.c',R/'scripts/verify_phil_registry_reconstruction.py',R/'scripts/verify_opponent_navigation_reconstruction.py',R/'scripts/verify_tinted_native_reconstruction.py',R/'scripts/verify_opponent_route_reconstruction.py',R/'scripts/inventory_reconstruction_backlog.py',args.original.resolve()]+list((R/'src').rglob('*.h'))
 if args.symbols:paths += [args.recompiled.resolve(),args.recompiled.resolve().with_suffix('.map'),args.symbols.resolve()]
 before={str(p.resolve()):p.read_bytes()for p in paths};(out/'source-before.json').write_text(json.dumps({p:sha(b)for p,b in before.items()},indent=2))
 binary=before[str(args.original.resolve())];assert sha(binary)==HASH
 original=packet(binary,0x4d7d80,707,GLOBALS,{k:a for k,(a,n)in BOUNDARIES.items()},out,'original')
 image=None;matching=None
 if args.symbols:
  symbols=json.loads(before[str(args.symbols.resolve())].decode());image=before[str(args.recompiled.resolve())];assert sha(image)==symbols['recompiled_sha256'];mapping={r['orig']:r['recomp']for r in symbols['symbols']}
  text=before[str(args.recompiled.resolve().with_suffix('.map'))].decode('latin1');rows=[(int(a,16),n,o.strip())for n,a,o in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',text,re.M)]
  start=mapping[0x4d7d80];assert [r for r in rows if r[0]==start]==[(start,'@UpdateTintedPolyByMaterial@4','66-tintedpoly.c.obj')];end=min(a for a,n,o in rows if a>start)
  g={mapping[0x705c80]:GLOBALS[0x705c80],mapping[0x65e840]+0x30:GLOBALS[0x65e870],mapping[0x6a0438]:GLOBALS[0x6a0438],mapping[0x6a043c]:GLOBALS[0x6a043c]}
  matching=packet(image,start,end-start,g,{k:mapping[a]for k,(a,n)in BOUNDARIES.items()},out,'matching')
 caller,_,_=registry.literals();caller=caller.replace('builder_poison;','builder_poison,builder_cdecl;').replace(' mov builder_result,eax\n mov ecx,esp',' mov builder_result,eax\n cmp builder_cdecl,0\n je tint_stack\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n tint_stack:\n mov ecx,esp')
 loader=r'''
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned char*disk,*image;unsigned n,i,v,rel,hdr[5],*p;IMAGE_NT_HEADERS*nt;IMAGE_SECTION_HEADER*s;if(argc!=4)return 1;f=fopen(argv[2],"rb");if(!f)return 2;fseek(f,0,SEEK_END);n=(unsigned)ftell(f);rewind(f);disk=malloc(n);if(!disk||fread(disk,n,1,f)!=1)return 3;fclose(f);nt=(IMAGE_NT_HEADERS*)(disk+((IMAGE_DOS_HEADER*)disk)->e_lfanew);image=VirtualAlloc(NULL,nt->OptionalHeader.SizeOfImage,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image)return 4;memcpy(image,disk,nt->OptionalHeader.SizeOfHeaders);s=IMAGE_FIRST_SECTION(nt);for(i=0;i<nt->FileHeader.NumberOfSections;i++)memcpy(image+s[i].VirtualAddress,disk+s[i].PointerToRawData,s[i].SizeOfRawData);f=fopen(argv[3],"rb");if(!f||fread(hdr,sizeof(hdr),1,f)!=1)return 5;n=hdr[4];p=malloc(n*16);if(n&&(!p||fread(p,n*16,1,f)!=1))return 6;fclose(f);for(i=0;i<n;i++){memcpy(&v,image+p[i*4],4);if(v!=p[i*4+1])return 8;v=p[i*4+3]?p[i*4+2]:(unsigned)image+p[i*4+2];memcpy(image+p[i*4],&v,4);}entry=(unsigned)image+hdr[0];for(i=1;i<4;i++){v=i==1?(unsigned)GetTotalTime:i==2?(unsigned)BrPixelmapPixelGet:(unsigned)BrMaterialUpdate;image[hdr[i]]=0xe9;rel=v-((unsigned)image+hdr[i]+5);memcpy(image+hdr[i]+1,&rel,4);}free(p);free(disk);return run(argv[1]);}
'''
 native=COMMON+'\n'+extract(before[str(source)].decode(),'UpdateTintedPolyByMaterial')+'\nint C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;entry=(unsigned)UpdateTintedPolyByMaterial;return run(argv[1]);}\n'
 registry.COMMON='#define PHYSICAL_IMAGE\n'+COMMON;commands=registry.compile_sources(out,native,loader,caller)
 result=[];fails=[];rows_=cases()
 for i,c in enumerate(rows_):
  case=out/f'case-{i}.bin';case.write_bytes(fixture(c));expected=None
  for name in('original','msvc5','msvc','mingw')+ (('matching',)if args.symbols else ()):
   cmd=[str(out/'physical.exe'),str(case),str(args.original.resolve()if name=='original'else args.recompiled.resolve()),str(out/(name+'-packet.bin'))]if name in('original','matching')else[str(out/(name+'.exe')),str(case)]
   run_=subprocess.run(cmd,capture_output=True,timeout=30);(out/f'{i}-{name}.bin').write_bytes(run_.stdout)
   if name=='original':assert run_.returncode==0 and run_.stdout;expected=run_.stdout;continue
   row=dict(index=i,compiler=name,exit=run_.returncode,passed=run_.returncode==0 and run_.stdout==expected,expected_sha256=sha(expected),actual_sha256=sha(run_.stdout));result.append(row)
   if not row['passed']:fails.append(row)
  if i%30==0:print(i,len(result),len(fails),flush=True)
 after={p:sha(Path(p).read_bytes())for p in before};assert after=={p:sha(b)for p,b in before.items()}
 v=dict(passed=not fails,source_stable=True,source_before={p:sha(b)for p,b in before.items()},source_after=after,scenes=len(rows_),comparisons=len(result),results=result,failures=fails,original=original,matching=matching,recompiled_sha256=sha(image)if image else None,symbols_sha256=sha(before[str(args.symbols.resolve())])if args.symbols else None,original_sha256=HASH,original_members=['0x4d7d80'],commands=commands,limitations=__doc__,live_handles=[])
 (out/'fixtures.json').write_text(json.dumps(rows_,indent=2));v['generated_sha256']={str(p.resolve()):sha(p.read_bytes())for p in out.iterdir()if p.suffix in('.c','.obj','.exe') or p.name.endswith('packet.bin')};v['fixtures_sha256']=sha((out/'fixtures.json').read_bytes());v['complete_suite']=True;v['candidate_path']=str(source);v['scene_input_sha256']={str((out/f'case-{i}.bin').resolve()):sha((out/f'case-{i}.bin').read_bytes())for i in range(len(rows_))};v['observation_policy']='Fixed320KiB arena and API events exactly; no byte masks or pointer/FP normalization. Void EAX excluded; ESP, nonvolatiles and x87TOP checked on returns. Fault observer records exact access address/type and public bytes, without return ABI claim.';v['api_boundaries']={'GetTotalTime':'controlled monotonic clock','BrPixelmapPixelGet':'controlled device provider; ordinary read-only queries plus explicit diagnostic input mutations','BrMaterialUpdate':'observed material-update event; full renderer outside scope'};(out/'verification.json').write_text(json.dumps(v,indent=2));print('DONE',len(rows_),len(result),len(fails));assert not fails
if __name__=='__main__':main()
