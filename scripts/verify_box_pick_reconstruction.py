"""Compare actual box picking bodies with original physical/x86 state and calls.

Requires Capstone5, Unicorn2, scoped x86 MSVC5SP3/modern MSVC/i686 MinGW.
Original PE is hash checked and only the two box entries plus seven verified
lower geometry/matrix functions execute in the physical probe. Matrix math
uses genuine original/native bodies; world callbacks are controlled boundaries.
No game entry point/imports/TLS execute; public scalar outputs are not normalized.
"""
import argparse,hashlib,json,os,random,struct,subprocess,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
from verify_tinted_native_reconstruction import extract
HASH='9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
GEOMETRY=ROOT/'src/carma2/newcommon/34-finteray.c'
MATRIX=ROOT/'src/brender/core/math/matrix34.c'
FUNCTIONS=[('ActorBoxPick',0x45f8c0,1735),('ModelPickBox',0x45ff90,3133),('GetNewBoundingBox',0x45f5b0,213),('BoundsOverlapTest__finteray',0x425ba0,250),('BrMatrix34Mul',0x532620,404),('BrMatrix34LPInverse',0x532eb0,175),('BrMatrix34ApplyP',0x5334a0,116),('BrMatrix34ApplyV',0x533520,107),('BrMatrix34TApplyV',0x533690,107)]
HEAP,SIZE=0x20000000,0x4000
def words(a):return struct.pack('<'+'I'*len(a),*[v&0xffffffff for v in a])
def fb(v):return struct.unpack('<I',struct.pack('<f',v))[0]
def make_case(**options):
 o=dict(mode=0,capacity=8,out=0x2000,transform=0,callbacks=1,pull_return=0,mutate=0,stop=1,model_arg=1,material_arg=1,groups=1,faces=2,updateable=0,material=1,identifier='ordinary',actor_model=1,actor_type=1,render=4,children=0,model_bounds=[-3,-3,-3,3,3,3],box_bounds=[-1,-1,-1,1,1,1],box_matrix=[1,0,0,0,1,0,0,0,1,0,0,0],actor_matrix=[1,0,0,0,1,0,0,0,1,0,0,0],output_matrix=[1,0,0,0,1,0,0,0,1,2,-3,4],vertices=[[-.75,-.5,0],[.75,-.5,0],[0,.75,0]],norm=[0,0,1,0],patch=[])
 o.update(options);a=bytearray(b'\xa5'*SIZE)
 def wr(off,v):a[off:off+4]=words([v])
 def fl(off,v):a[off:off+4*len(v)]=struct.pack('<'+'f'*len(v),*v)
 def sh(off,v):struct.pack_into('<H',a,off,v&65535)
 def ptr(off,target):wr(off,HEAP+target if target is not None else 0)
 ptr(0,0x80);fl(4,o['box_bounds']);bounds=o['box_bounds'];m=o['box_matrix'];centre=[(bounds[i]+bounds[i+3])*.5 for i in range(3)];fl(28,[sum(centre[j]*m[j*3+i] for j in range(3))+m[9+i] for i in range(3)]);fl(40,[sum((bounds[i+3]-bounds[i])**2 for i in range(3))**.5*.5]);points=[[sum(bounds[j+3 if corner&(1<<j) else j]*m[j*3+i] for j in range(3))+m[9+i] for i in range(3)] for corner in range(8)];fl(44,[min(p[i] for p in points) for i in range(3)]+[max(p[i] for p in points) for i in range(3)])
 fl(0x80,o['box_matrix']);fl(0xc0,o['output_matrix'])
 for i in range(5):
  p=0x180+i*100;a[p:p+100]=bytes(100);ptr(p,None);ptr(p+4,None);ptr(p+8,None);ptr(p+0xc,None);a[p+0x12]=o['actor_type'] if i==0 else 1;a[p+0x20]=o['render'] if i==0 else 4
  ptr(p+0x14,0x1c80+i*24);name=o['identifier'] if i==0 else '&1child';a[0x1c80+i*24:0x1c80+i*24+len(name)+1]=name.encode()+b'\0';ptr(p+0x18,0x500 if i or o['actor_model'] else None);ptr(p+0x1c,0x1a00+156);fl(p+0x2c,o['actor_matrix']);ptr(p+0x5c,0x2680)
 if o['children']:
  ptr(0x180+8,0x180+100)
  for i in range(1,o['children']):ptr(0x180+i*100,0x180+(i+1)*100)
 a[0x500:0x500+92]=bytes(92);ptr(0x500+8,0x1500);ptr(0x500+12,0x1800);sh(0x500+0x10,9);sh(0x500+0x12,6);sh(0x500+0x20,0x80 if o['updateable'] else 0);fl(0x500+0x34,o['model_bounds']);ptr(0x500+0x4c,0x800)
 a[0x800:0x800+28]=bytes(28);sh(0x800+8,o['groups']);ptr(0x800+24,0x900)
 for g in range(2):
  p=0x900+g*36;a[p:p+36]=bytes(36);ptr(p+4,0x1100+g*84);ptr(p+8,0x1340+g*4);ptr(p+12,0x1300+g*6);ptr(p+16,0xc00+g*256);sh(p+28,o['faces']);sh(p+30,8);sh(p+32,9)
  ptr(0x1340+g*4,0x1a00 if o['material'] else None)
  for v in range(8):
   fl(0xc00+g*256+v*32,o['vertices'][v%3]);fl(0xc00+g*256+v*32+12,[v*.125,g*.25]);fl(0xc00+g*256+v*32+20,[0,0,1])
  for f in range(3):
   p=0x1100+g*84+f*28;sh(p,0);sh(p+2,1);sh(p+4,2);sh(p+6,f&1);sh(p+8,0);sh(p+10,2);fl(p+12,o['norm']);sh(0x1300+g*6+f*2,5-f)
 for v in range(9):a[0x1500+v*40:0x1500+(v+1)*40]=bytes([37+v])*40
 for f in range(6):
  p=0x1800+f*40;a[p:p+40]=bytes(40);sh(p,f%3);sh(p+2,(f+1)%9);sh(p+4,(f+2)%9);a[p+16]=0x80|f
 for i,name in enumerate(['!pling','ordinary','fallback']):
  p=0x1a00+i*156;a[p:p+156]=bytes([0x30+i])*156;ptr(p+4,0x1c00+i*24);a[0x1c00+i*24:0x1c00+i*24+len(name)+1]=name.encode()+b'\0'
 a[0x2600:0x2620]=bytes(32);wr(0x2610,0x108f000);wr(0x2614,0x108f010);fl(0x2680,o['model_bounds'])
 for off,val in o['patch']:wr(off,val)
 return dict(options=o,header=[o[k] for k in ('mode','capacity','out','transform','callbacks','pull_return','mutate','stop','model_arg','material_arg')],data=bytes(a))

SCAFFOLD=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "34-finteray.h"
#include "matrix34.h"
#include "rec2_macros.h"
#include "c2_math.h"
#define A(x,y) A->m[x][y]
#define B(x,y) B->m[x][y]
#define C(x,y) C->m[x][y]
#undef BR_MAC3
#define BR_MAC3(a,b,c,d,e,f) ((a)*(b)+(c)*(d)+(e)*(f))
static unsigned header[10],events;
static unsigned char* arena;
int gActorBoxPick_StopGroovidelics;
tFace_ref* gPling_face;
static tNon_car_spec* C2_HOOK_FASTCALL pull(br_actor* actor);
static void C2_HOOK_FASTCALL stop(br_actor* actor);
static tWorld_callbacks* world(void){return (tWorld_callbacks*)(arena+0x2600);}
static void C2_HOOK_CDECL snapshot(void){
 unsigned char copy[0x4000];unsigned v[2],x;
 memcpy(copy,arena,sizeof(copy));memcpy(&x,copy+0x2610,4);if(x==(unsigned)pull){x=0x108f000;memcpy(copy+0x2610,&x,4);}memcpy(&x,copy+0x2614,4);if(x==(unsigned)stop){x=0x108f010;memcpy(copy+0x2614,&x,4);}
 v[0]=(unsigned)gActorBoxPick_StopGroovidelics;v[1]=(unsigned)gPling_face;fwrite(v,sizeof(v),1,stdout);fwrite(copy,sizeof(copy),1,stdout);
}
static void C2_HOOK_CDECL event(unsigned code,void* output,void* input,void* matrix,unsigned output_size,unsigned input_size,int after){
 unsigned v[8],i;unsigned char operands[144];memset(v,0,sizeof(v));memset(operands,0,sizeof(operands));v[0]=code;v[1]=after;
 v[2]=(unsigned)output>=0x20000000&&(unsigned)output<0x20004000?(unsigned)output:0;
 v[3]=(unsigned)input>=0x20000000&&(unsigned)input<0x20004000?(unsigned)input:0;
 v[4]=(unsigned)matrix>=0x20000000&&(unsigned)matrix<0x20004000?(unsigned)matrix:0;
 v[5]=output_size;v[6]=input_size;v[7]=0xffffffff;
 if(!v[2]&&!v[3]&&((unsigned)input-(unsigned)output<output_size||(unsigned)output-(unsigned)input<input_size))v[7]=(unsigned)input-(unsigned)output;
 if(after&&output)memcpy(operands,output,output_size);if(input)memcpy(operands+48,input,input_size);if(matrix)memcpy(operands+96,matrix,48);
 fwrite(v,sizeof(v),1,stdout);fwrite(operands,sizeof(operands),1,stdout);snapshot();events++;i=0;(void)i;
}
static void C2_HOOK_CDECL mutate(br_actor* actor){
 if(header[6]&1)gActorBoxPick_StopGroovidelics=!gActorBoxPick_StopGroovidelics;
 if(header[6]&2)actor->children=actor->children?actor->children->next:NULL;
 if(header[6]&4){actor->model=NULL;actor->material=(br_material*)(arena+0x1a00+312);}
 if(header[6]&8)actor->next=NULL;
 if(header[6]&16)world()->stop_groovidelic=NULL;
 if(header[6]&32)world()->pull_actor_from_world=NULL;
 if(header[6]&64)gPling_face=(tFace_ref*)(arena+0x2000+240);
 if(header[6]&128)gActorBoxPick_StopGroovidelics=0;
 if(header[6]&256){actor->type=BR_ACTOR_BOUNDS;actor->type_data=arena+0x2680;}
 if(header[6]&512)actor->identifier=(char*)(arena+0x1c00+24);
}
static tNon_car_spec* C2_HOOK_FASTCALL pull(br_actor* actor){event(20,actor,NULL,NULL,0,0,0);mutate(actor);return header[5]?(tNon_car_spec*)(arena+0x3000):NULL;}
static void C2_HOOK_FASTCALL stop(br_actor* actor){event(21,actor,NULL,NULL,0,0,0);mutate(actor);}
'''

def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE');p.add_argument('--output',type=Path,default=ROOT/'build-audit/finteray-box-next/box-tests');p.add_argument('--gcc',type=Path,default=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe');p.add_argument('--msvc-root',type=Path,default=ROOT/'build-tools/msvc-modern');p.add_argument('--msvc5-root',type=Path,default=ROOT/'build-tools/msvc500sp3');p.add_argument('--case-index',type=int,action='append');p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);p.add_argument('--linked-only',action='store_true',help='Rerun only linked/original physical proof after an unchanged native-body update');p.add_argument('--previous-native-proof',type=Path,help='Prior complete proof whose extracted native harness hash must remain identical');args=p.parse_args();out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)
 if bool(args.recompiled)!=bool(args.symbols):p.error('--recompiled and --symbols must be supplied together')
 if args.linked_only and (not args.recompiled or not args.previous_native_proof):p.error('--linked-only requires linked PE/map and --previous-native-proof')
 image=args.original.read_bytes();assert hashlib.sha256(image).hexdigest()==HASH
 from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_MEM
 nt=struct.unpack_from('<I',image,60)[0];tab=nt+24+struct.unpack_from('<H',image,nt+20)[0]
 def raw(a,n):
  for i in range(struct.unpack_from('<H',image,nt+6)[0]):
   _,va,size,off=struct.unpack_from('<IIII',image,tab+40*i+8)
   if va<=a-0x400000<va+size:return image[off+a-0x400000-va:off+a-0x400000-va+n]
 cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;index={a:i for i,(name,a,n) in enumerate(FUNCTIONS)};patches=[]
 for i,(name,a,n) in enumerate(FUNCTIONS):
  for ins in cs.disasm(raw(a,n),a):
   if ins.mnemonic=='call' and ins.op_str.startswith('0x'):
    target=ins.operands[0].imm;assert target in index,(name,hex(ins.address),hex(target));patches.append((i,ins.address-a+ins.imm_offset,1,index[target]))
   for operand in ins.operands:
    if operand.type==CS_OP_MEM and not operand.mem.base and not operand.mem.index:
     address=operand.mem.disp;assert address in (0x589fac,0x5964c4,0x79d858),(name,hex(address));patches.append((i,ins.address-a+ins.disp_offset,0,[0x589fac,0x5964c4,0x79d858].index(address)))
 native=SCAFFOLD
 for name,a,n in FUNCTIONS[4:]:native+='\n'+extract(MATRIX.read_text(),name).replace(name,'Native'+name)
 native+='\n'
 probe=SCAFFOLD+r'''
static void* original_code[9];
static const float original_zero=0.f;
'''
 probe+='static const unsigned addresses[9]={'+','.join(hex(a-0x400000) for name,a,n in FUNCTIONS)+'};\nstatic const unsigned sizes[9]={'+','.join(str(n) for name,a,n in FUNCTIONS)+'};\n'
 probe+='static const unsigned patches[][4]={'+','.join('{'+','.join(str(v) for v in row)+'}' for row in patches)+'};\n'
 wrappers=''
 for i,(name,a,n) in enumerate(FUNCTIONS[4:]):
  if i==1:signature='br_matrix34* A, const br_matrix34* B';args_call='A,B';output_size=input_size=48;matrix='NULL'
  elif i==0:signature='br_matrix34* A, const br_matrix34* B, const br_matrix34* C';args_call='A,B,C';output_size=input_size=48;matrix='(void*)C'
  else:signature='br_vector3* A, const br_vector3* B, const br_matrix34* C';args_call='A,B,C';output_size=input_size=12;matrix='(void*)C'
  body=f'void C2_HOOK_CDECL {name}({signature}) {{event({i+1},A,(void*)B,{matrix},{output_size},{input_size},0);CALL;event({i+1},A,(void*)B,{matrix},{output_size},{input_size},1);}}\n'
  native+=body.replace('CALL','Native'+name+'('+args_call+')')
  typedef=f'typedef void (C2_HOOK_CDECL *original_matrix_{i})({signature});\n'
  probe+=typedef+body.replace('CALL',f'((original_matrix_{i})original_code[{i+4}])({args_call})')
  wrappers+=f'case {i+4}:target=(unsigned){name};break;'
 native+='\n'.join(extract(GEOMETRY.read_text(),name) for name in ['FinterayScalar','FinterayBoxNaN','FinterayBoxCopyScalar','FinterayBoxStoreScalar','FinterayBoxClip','ModelPickBox','FinterayBoxBoundsTransform','ActorBoxPick','GetNewBoundingBox','BoundsOverlapTest__finteray'])
 probe+=r'''
static int install_original(const char* name){
 FILE*f;unsigned char*disk;unsigned size,i,j,old,target,site;IMAGE_DOS_HEADER*dos;IMAGE_NT_HEADERS32*nt;IMAGE_SECTION_HEADER*sec;
 f=fopen(name,"rb");if(!f)return 91;fseek(f,0,SEEK_END);size=(unsigned)ftell(f);rewind(f);disk=(unsigned char*)malloc(size);if(!disk||fread(disk,size,1,f)!=1)return 92;fclose(f);
 dos=(IMAGE_DOS_HEADER*)disk;nt=(IMAGE_NT_HEADERS32*)(disk+dos->e_lfanew);sec=IMAGE_FIRST_SECTION(nt);
 for(i=0;i<9;i++){original_code[i]=VirtualAlloc(NULL,4096,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!original_code[i])return 93;
  for(j=0;j<nt->FileHeader.NumberOfSections;j++)if(sec[j].VirtualAddress<=addresses[i]&&addresses[i]+sizes[i]<=sec[j].VirtualAddress+sec[j].SizeOfRawData){memcpy(original_code[i],disk+sec[j].PointerToRawData+addresses[i]-sec[j].VirtualAddress,sizes[i]);break;}if(j==nt->FileHeader.NumberOfSections)return 94;}
 for(i=0;i<sizeof(patches)/sizeof(patches[0]);i++){
  site=(unsigned)original_code[patches[i][0]]+patches[i][1];
  if(patches[i][2]){target=(unsigned)original_code[patches[i][3]];switch(patches[i][3]){WRAPPERS}old=target-(site+4);}
  else{target=patches[i][3];old=target==0?(unsigned)&original_zero:target==1?(unsigned)&gActorBoxPick_StopGroovidelics:(unsigned)&gPling_face;}
  memcpy((void*)site,&old,4);
 }
 free(disk);return 0;
}
typedef int (C2_HOOK_FASTCALL *original_model)(br_actor*,tBounds*,br_model*,br_material*,tFace_ref*,int,br_matrix34*);
typedef int (C2_HOOK_FASTCALL *original_actor)(tBounds*,br_actor*,br_model*,br_material*,tFace_ref*,int,br_matrix34*,tWorld_callbacks*);
'''.replace('WRAPPERS',wrappers)
 driver=r'''
int C2_HOOK_CDECL main(int argc,char**argv){
 FILE*f;int result;unsigned v[8];br_actor*actor;br_model*model;br_material*material;br_matrix34*transform;tFace_ref*faces;tBounds*bounds;tWorld_callbacks*callbacks;
 SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX|SEM_NOOPENFILEERRORBOX);
 if(argc!=ARGS)return 2;f=fopen(argv[1],"rb");if(!f)return 3;if(fread(header,sizeof(header),1,f)!=1)return 4;
 arena=(unsigned char*)VirtualAlloc((void*)0x20000000,0x4000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(arena!=(void*)0x20000000)return 5;if(fread(arena,0x4000,1,f)!=1)return 6;fclose(f);
 if(sizeof(br_actor)!=100||sizeof(br_model)!=92||sizeof(br_material)!=156||sizeof(tBounds)!=68||sizeof(tFace_ref)!=80||sizeof(v11group)!=36||sizeof(v11face)!=28||sizeof(fmt_vertex)!=32||sizeof(v11model)!=28)return 7;
 _controlfp(_PC_53,_MCW_PC);_setmode(_fileno(stdout),_O_BINARY);world()->pull_actor_from_world=pull;world()->stop_groovidelic=stop;gActorBoxPick_StopGroovidelics=(int)header[7];gPling_face=(tFace_ref*)(arena+0x2000+400);
 actor=(br_actor*)(arena+0x180);bounds=(tBounds*)arena;model=header[8]?(br_model*)(arena+0x500):NULL;material=header[9]?(br_material*)(arena+0x1a00+312):NULL;faces=(tFace_ref*)(arena+header[2]);transform=header[3]?(br_matrix34*)(arena+0xc0):NULL;callbacks=header[4]?world():NULL;
 INSTALL
 result=CALL;
 memset(v,0,sizeof(v));v[0]=0;v[1]=(unsigned)result;v[2]=events;fwrite(v,sizeof(v),1,stdout);{unsigned char zero[144];memset(zero,0,sizeof(zero));fwrite(zero,sizeof(zero),1,stdout);}snapshot();return 0;
}
'''
 native+=driver.replace('ARGS','2').replace('INSTALL','').replace('CALL','header[0]==0?ModelPickBox(actor,bounds,model,material,faces,(int)header[1],transform):ActorBoxPick(bounds,actor,model,material,faces,(int)header[1],transform,callbacks)')
 probe+=driver.replace('ARGS','3').replace('INSTALL','result=install_original(argv[2]);if(result)return result;').replace('CALL','header[0]==0?((original_model)original_code[1])(actor,bounds,model,material,faces,(int)header[1],transform):((original_actor)original_code[0])(bounds,actor,model,material,faces,(int)header[1],transform,callbacks)')
 (out/'harness.c').write_text(native);(out/'physical_original.c').write_text(probe)
 if args.linked_only:
  previous=json.loads(args.previous_native_proof.read_text())
  if not previous.get('complete_suite') or not previous.get('all_cases_passed') or previous.get('harness_sha256')!=hashlib.sha256(native.encode()).hexdigest():raise ValueError('Prior native proof harness differs or is incomplete')
 includes=[ROOT/path for path in ('src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/carma2/platform','src/carma2/s3/include','src/brender/core/math','src/brender/include')]
 msvc=sorted((args.msvc_root/'VC/Tools/MSVC').iterdir())[-1];sdk=args.msvc_root/'Windows Kits/10';version=sorted((sdk/'Include').iterdir())[-1].name;env=dict(os.environ);env['PATH']=str(msvc/'bin/Hostx64/x86')+os.pathsep+env['PATH'];env['INCLUDE']=os.pathsep.join(map(str,[msvc/'include',*[sdk/f'Include/{version}/{part}' for part in ('ucrt','shared','um')]]));env['LIB']=os.pathsep.join(map(str,[msvc/'lib/x86',*[sdk/f'Lib/{version}/{part}/x86' for part in ('ucrt','um')]]))
 m5=args.msvc5_root;env5=dict(os.environ);env5['PATH']=str(m5/'bin')+os.pathsep+str(m5/'redist')+os.pathsep+env5['PATH'];env5['INCLUDE']=str(m5/'include');env5['LIB']=str(m5/'lib')
 commands={
 'msvc':[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(i) for i in includes],str(out/'harness.c'),'/Fo'+str(out/'msvc.obj'),'/Fe'+str(out/'msvc.exe')],
 'msvc5':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(i) for i in includes],str(out/'harness.c'),'/Fo'+str(out/'msvc5.obj'),'/Fe'+str(out/'msvc5.exe')],
 'mingw':[str(args.gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation',*['-I'+str(i) for i in includes],str(out/'harness.c'),'-o',str(out/'mingw.exe')],
 'physical':[str(args.gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-DREC2_MATCHING',*['-I'+str(i) for i in includes],str(out/'physical_original.c'),'-o',str(out/'physical.exe')]}
 for compiler,command in commands.items():
  if args.linked_only and compiler!='physical':continue
  r=subprocess.run(command,cwd=out,env=env if compiler=='msvc' else env5 if compiler=='msvc5' else None,text=True,capture_output=True);(out/f'{compiler}-build.txt').write_text(r.stdout+r.stderr)
  if r.returncode:raise RuntimeError(r.stdout+r.stderr)
 rebuilt=None;linked_manifest=None
 if args.recompiled:
  from capstone import CS_OP_IMM
  rebuilt=args.recompiled.read_bytes();mapping=json.loads(args.symbols.read_text())
  if mapping.get('format')!=1 or mapping.get('original_sha256')!=HASH or mapping.get('recompiled_sha256')!=hashlib.sha256(rebuilt).hexdigest():raise ValueError('Linked PE or symbol export hash differs')
  mapped={v['orig']:v['recomp'] for v in mapping['symbols']};names={v['recomp']:v['name'] for v in mapping['symbols']}
  linked_nt=struct.unpack_from('<I',rebuilt,60)[0];linked_base=struct.unpack_from('<I',rebuilt,linked_nt+52)[0];image_size=struct.unpack_from('<I',rebuilt,linked_nt+80)[0];linked_tab=linked_nt+24+struct.unpack_from('<H',rebuilt,linked_nt+20)[0];segments=[]
  for i in range(struct.unpack_from('<H',rebuilt,linked_nt+6)[0]):
   _,va,n,offset=struct.unpack_from('<IIII',rebuilt,linked_tab+40*i+8);flags=struct.unpack_from('<I',rebuilt,linked_tab+40*i+36)[0];segments.append((va,n,offset,flags))
  def linked_raw(address,n=15):
   for va,size,offset,flags in segments:
    if flags&0x20 and va<=address-linked_base<va+size:return rebuilt[offset+address-linked_base-va:offset+address-linked_base-va+n]
   raise ValueError(f'Linked closure address{address:x} is outside code')
  starts=[mapped[address] for name,address,n in FUNCTIONS];matrix_indices={starts[i]:i for i in range(4,9)};relocations={};wrapped_calls={};instructions={};entries=set(starts);pending=list(starts)
  while pending:
   function=pending.pop();todo=[function];visited=set()
   while todo:
    address=todo.pop()
    if address in visited:continue
    visited.add(address);ins=next(cs.disasm(linked_raw(address),address,count=1),None)
    if ins is None:raise ValueError(f'Invalid linked instruction{address:x}')
    instructions[address]=ins.bytes.hex()
    for operand in ins.operands:
     if operand.type==CS_OP_MEM and not operand.mem.base and not operand.mem.index:
      value=operand.mem.disp
      if not(linked_base<=value<linked_base+image_size) or ins.disp_size!=4:raise ValueError(f'Unapproved linked absolute operand{value:x}')
      kind=0 if value==mapped[0x5964c4] else 1 if value==mapped[0x79d858] else 2
      relocations[ins.address-linked_base+ins.disp_offset]=kind
    if ins.mnemonic=='call':
     if ins.operands[0].type==CS_OP_IMM:
      target=ins.operands[0].imm;linked_raw(target)
      if target not in entries:entries.add(target);pending.append(target)
      if target in matrix_indices:wrapped_calls[ins.address-linked_base+ins.imm_offset]=matrix_indices[target]
     elif function!=starts[0] or (ins.operands[0].type==CS_OP_MEM and not ins.operands[0].mem.base):raise ValueError('Linked closure has unapproved indirect API call')
    if ins.mnemonic=='ret':continue
    if ins.mnemonic.startswith('j'):
     if ins.operands[0].type!=CS_OP_IMM:raise ValueError('Linked closure has an indirect jump')
     todo.append(ins.operands[0].imm)
     if ins.mnemonic=='jmp':continue
    todo.append(address+ins.size)
    if len(instructions)>20000:raise ValueError('Linked closure instruction budget exceeded')
  linked_probe=probe[:probe.index('static int install_original')]
  linked_probe+='static const unsigned linked_starts[9]={'+','.join(hex(v-linked_base) for v in starts)+'};\n'
  linked_probe+='static const unsigned linked_references[][2]={'+','.join('{'+str(a)+','+str(kind)+'}' for a,kind in sorted(relocations.items()))+'};\n'
  linked_probe+='static const unsigned linked_calls[][2]={'+','.join('{'+str(a)+','+str(kind)+'}' for a,kind in sorted(wrapped_calls.items()))+'};\n'
  linked_probe+=r'''
static int install_original(const char* name){
 FILE*f;unsigned char*disk,*image;unsigned size,i,site,target,value;IMAGE_DOS_HEADER*dos;IMAGE_NT_HEADERS32*nt;IMAGE_SECTION_HEADER*sec;
 f=fopen(name,"rb");if(!f)return 91;fseek(f,0,SEEK_END);size=(unsigned)ftell(f);rewind(f);disk=(unsigned char*)malloc(size);if(!disk||fread(disk,size,1,f)!=1)return 92;fclose(f);
 dos=(IMAGE_DOS_HEADER*)disk;nt=(IMAGE_NT_HEADERS32*)(disk+dos->e_lfanew);sec=IMAGE_FIRST_SECTION(nt);
 image=(unsigned char*)VirtualAlloc(NULL,nt->OptionalHeader.SizeOfImage,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image)return 93;
 memcpy(image,disk,nt->OptionalHeader.SizeOfHeaders);for(i=0;i<nt->FileHeader.NumberOfSections;i++)memcpy(image+sec[i].VirtualAddress,disk+sec[i].PointerToRawData,sec[i].SizeOfRawData);
 for(i=0;i<sizeof(linked_references)/sizeof(linked_references[0]);i++){
  site=(unsigned)image+linked_references[i][0];memcpy(&value,(void*)site,4);target=linked_references[i][1];value=target==0?(unsigned)&gActorBoxPick_StopGroovidelics:target==1?(unsigned)&gPling_face:(unsigned)image+value-nt->OptionalHeader.ImageBase;memcpy((void*)site,&value,4);
 }
 for(i=0;i<sizeof(linked_calls)/sizeof(linked_calls[0]);i++){
  site=(unsigned)image+linked_calls[i][0];target=linked_calls[i][1];switch(target){WRAPPERS}value=target-(site+4);memcpy((void*)site,&value,4);
 }
 for(i=0;i<9;i++)original_code[i]=image+linked_starts[i];free(disk);return 0;
}
'''.replace('WRAPPERS',wrappers)
  linked_probe+=probe[probe.index('typedef int (C2_HOOK_FASTCALL *original_model)'):]
  (out/'physical_matching.c').write_text(linked_probe)
  linked_command=[str(args.gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-Wno-unused-const-variable','-DREC2_MATCHING',*['-I'+str(i) for i in includes],str(out/'physical_matching.c'),'-o',str(out/'physical-matching.exe')]
  r=subprocess.run(linked_command,cwd=out,text=True,capture_output=True);(out/'physical-matching-build.txt').write_text(r.stdout+r.stderr)
  if r.returncode:raise RuntimeError(r.stdout+r.stderr)
  linked_manifest=dict(recompiled_sha256=hashlib.sha256(rebuilt).hexdigest(),symbols_sha256=hashlib.sha256(args.symbols.read_bytes()).hexdigest(),closure_entries=[dict(address=hex(v),name=names.get(v,'unannotated compiler helper')) for v in sorted(entries)],closure_instruction_count=len(instructions),closure_instruction_sha256=hashlib.sha256(json.dumps(sorted(instructions.items())).encode()).hexdigest(),relocations=sorted(relocations.items()),matrix_call_sites=sorted(wrapped_calls.items()),physical_probe_sha256=hashlib.sha256(linked_probe.encode()).hexdigest(),command=linked_command)
 cases=[make_case(mode=mode,capacity=capacity,transform=t,groups=g,updateable=u) for mode in (0,1) for capacity in (0,1,8,-1) for t in (0,1) for g in (0,1,2) for u in (0,1)]
 # Ordinary finite scene contracts before pathological numeric/alias cases.
 cases += [make_case(mode=1,identifier=name,children=children,transform=transform) for name in ('&node','&0node','&9node','&/node','&:node','&') for children in (0,2) for transform in (0,1)]
 cases += [make_case(mode=1,identifier='&1',children=3,pull_return=ret,mutate=mutate,capacity=capacity) for mutate in (0,1,2,4,8,16,32,64,128,256,512) for ret in (0,1) for capacity in (1,8)]
 cases += [make_case(mode=1,identifier=ident,actor_type=actor_type,actor_model=own,model_arg=inherited,render=render,children=2) for ident in ('ordinary','&1') for actor_type in (0,1,5,6) for own,inherited in ((1,0),(0,1),(0,0)) for render in (1,4) if not(ident=='&1' and not own and inherited)]
 cases += [make_case(mode=mode,groups=g,faces=0,stop=stop,material=material) for mode in (0,1) for g in (0,2) for stop in (0,1) for material in (0,1)]
 rotations=[[0,1,0,-1,0,0,0,0,1,0,0,0],[0,0,-1,0,1,0,1,0,0,0,0,0],[1,0,0,0,0,1,0,-1,0,0,0,0],[-1,0,0,0,1,0,0,0,-1,.25,-.5,.75]]
 for matrix in rotations:
  for mode in (0,1):
   cases.append(make_case(mode=mode,identifier='&1',actor_matrix=matrix,children=2,box_matrix=matrix,transform=1))
 # Six clipping sides, exact plane/edge contacts, degenerates and sphere reject.
 for axis in range(3):
  for displacement in (-2,-1,-.999,0,.999,1,2):
   vertices=[v[:] for v in [[-.75,-.5,0],[.75,-.5,0],[0,.75,0]]]
   for v in vertices:v[axis]+=displacement
   norm=[0,0,1,displacement if axis==2 else 0]
   cases.append(make_case(vertices=vertices,norm=norm))
 cases += [make_case(vertices=v) for v in ([[0,0,0]]*3,[[0,0,0],[.5,0,0],[1,0,0]],[[-2,-2,0],[2,-2,0],[0,2,0]])]
 # Valid overlapping vertex/normal/map/transform storage. Keep readable metadata.
 for output in (0xc00,0xc04,0xc0c,0xc00-4,0x1118,0xc0):
  for transform in (0,1):cases.append(make_case(out=output,transform=transform,faces=1,capacity=1))
 # Bounded nonfinite gaps: stored metadata versus arithmetic/branch behavior.
 for value in (0x7fc12345,0x7f800001):
  for offset in (40,0x110c,0x1118,0xc00):
   for transform in (0,1):cases.append(make_case(transform=transform,patch=[(offset,value)]))
 for offset in (4,16,44,56):cases.append(make_case(patch=[(offset,0x7fc12345)]))
 for value in (0x7f800000,0xff800000):cases.append(make_case(patch=[(0xc00,value)]))
 for value in (0x7fc12345,0x7f800001):
  for children in (0,2):cases.append(make_case(mode=1,identifier='&1',children=children,patch=[(0x180+0x2c,value)]))
 cases += [make_case(mode=1,identifier='&1',children=2,capacity=-1),make_case(material=0,material_arg=0),make_case(mode=1,identifier='&1',callbacks=0)]
 # Preserve explicit original dereferences/termination, not invented null guards.
 cases += [make_case(capacity=capacity,model_arg=0,expect_fault=True) for capacity in (0,-1,1)]
 cases += [make_case(mode=1,identifier='&1',actor_model=0,model_arg=1,children=children,expect_fault=True) for children in (0,2)]
 cases.append(make_case(patch=[(0x900+8,0)],expect_fault=True))
 # Nonaxis planes and non-dyadic clipping intersections at real PC53 precision.
 rng=random.Random(0x45ff90)
 for i in range(24):
  vertices=[[rng.uniform(-2.5,2.5),rng.uniform(-2.5,2.5),0] for _ in range(3)]
  for v in vertices:v[2]=.075*v[0]+.125*v[1]+.025*(i-12)
  n=[-.075,-.125,1.0];length=sum(v*v for v in n)**.5;n=[v/length for v in n];d=sum(n[j]*vertices[0][j] for j in range(3))
  cases.append(make_case(vertices=vertices,norm=[*n,d],transform=1,updateable=i&1,mode=i&1,identifier='&1' if i&1 else 'ordinary'))
 if args.case_index is not None and any(i<0 or i>=len(cases) for i in args.case_index):p.error('case index outside suite')
 results=[]
 for i,c in enumerate(cases):
  if args.case_index is not None and i not in args.case_index:continue
  fixture=out/f'case-{i:03d}.bin';fixture.write_bytes(words(c['header'])+c['data'])
  reference=subprocess.run([str(out/'physical.exe'),str(fixture),str(args.original.resolve())],capture_output=True);expected=reference.stdout;expected_exit=reference.returncode&0xffffffff
  if c['options'].get('expect_fault'):
   assert expected_exit==0xc0000005,f'Original expected access violation, got{expected_exit:x}'
  else:assert expected_exit==0,f'Physical original case{i} exits{expected_exit:x}'
  (out/f'case-{i:03d}.physical.bin').write_bytes(expected)
  checks=[] if args.linked_only else [(compiler,subprocess.run([str(out/f'{compiler}.exe'),str(fixture)],capture_output=True)) for compiler in ('msvc5','msvc','mingw')]
  if rebuilt:checks.append(('matching-physical',subprocess.run([str(out/'physical-matching.exe'),str(fixture),str(args.recompiled.resolve())],capture_output=True)))
  for compiler,execution in checks:
   actual=execution.stdout;actual_exit=execution.returncode&0xffffffff
   if actual!=expected or actual_exit!=expected_exit:
    prefix=out/f'failure-{compiler}-{i}';prefix.with_suffix('.expected.bin').write_bytes(expected);prefix.with_suffix('.actual.bin').write_bytes(actual);prefix.with_suffix('.json').write_text(json.dumps(dict(options=c['options'],header=c['header'],first_difference=next((j for j,(a,b) in enumerate(zip(actual,expected)) if a!=b),None),expected_size=len(expected),actual_size=len(actual),original_exit=hex(expected_exit),native_exit=hex(actual_exit)),indent=2));raise AssertionError(f'Box {compiler} case{i} differs; see {prefix}')
   results.append(dict(case_index=i,compiler=compiler,all_state_calls_match=True,state_calls_sha256=hashlib.sha256(actual).hexdigest(),exit_code=hex(actual_exit)))
 manifest=dict(original_sha256=HASH,cases=len(cases),comparisons=len(results),complete_suite=args.case_index is None,all_cases_passed=True,results=results,functions=FUNCTIONS,source_sha256={str(f.relative_to(ROOT)):hashlib.sha256(f.read_bytes()).hexdigest() for f in (GEOMETRY,MATRIX,ROOT/'src/carma2/newcommon/34-finteray.h')},harness_sha256=hashlib.sha256(native.encode()).hexdigest(),physical_probe_sha256=hashlib.sha256(probe.encode()).hexdigest(),commands=commands,limitations='World callbacks controlled; matrix bodies and original/native geometry are real. Only function-pointer slots and private stack pointer identities are translated, never public scalars.')
 if linked_manifest:manifest['matching_physical']=linked_manifest
 if args.linked_only:manifest.update(linked_only=True,prior_native_proof_sha256=hashlib.sha256(args.previous_native_proof.read_bytes()).hexdigest(),prior_native_comparisons=sum(r['compiler'] in ('msvc5','msvc','mingw') for r in previous['results']))
 (out/'verification.json').write_text(json.dumps(manifest,indent=2)+'\n');print(f'PASS: {len(results)} full-state/call box comparisons against physical original PC53')
if __name__=='__main__':main()
