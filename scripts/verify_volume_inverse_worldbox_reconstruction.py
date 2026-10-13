"""Complete92B volume root with real213B bounds,725B inverse,116B apply lower bodies.
Native Win32x86 original + current C compiled MSVC5/modern/MinGW at PC53,
masked exceptions. Fixed65536B arena compared byte-for-byte after each call.
No callee boundary, geometry/math substitute, float/pointer normalization or
private-stack exposure. Valid public volume layouts only; no whole-game,
unmasked trap, sticky-FP-flag or exhaustive-input claim.
"""
import argparse,json,random,struct,subprocess,sys
from pathlib import Path
R=next(p for p in Path(__file__).resolve().parents if (p/'src/carma2').is_dir());D=Path(__file__).resolve().parent;sys.path.insert(0,str(R/'scripts'))
import verify_phil_registry_reconstruction as registry
from verify_opponent_route_reconstruction import raw,pe,HASH,sha,words
from verify_tinted_native_reconstruction import extract
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM,CS_OP_MEM,CS_GRP_CALL,CS_GRP_JUMP
COMMON=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "55-volume.h"
#include "34-finteray.h"
static unsigned char*arena;static unsigned entry;
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[12],builder_stack_count,builder_result,builder_poison,builder_cdecl;
extern unsigned __cdecl BuilderInvoke(void);
typedef char layout[sizeof(tSpecial_volume)==220?1:-1];
static int run(const char*path){FILE*f;unsigned h[4],i;if(offsetof(tSpecial_volume,boundary.box.mat)!=4||offsetof(tSpecial_volume,boundary.box.inv_mat)!=0x34||offsetof(tSpecial_volume,boundary.box.bounds)!=0x70)return 15;SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);arena=VirtualAlloc((void*)0x20000000,65536,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(arena!=(void*)0x20000000)return 11;f=fopen(path,"rb");if(!f||fread(h,sizeof(h),1,f)!=1||fread(arena,65536,1,f)!=1)return 12;fclose(f);_setmode(_fileno(stdout),_O_BINARY);_controlfp(_PC_53,_MCW_PC);_clearfp();for(i=0;i<h[1];i++){builder_address=entry;builder_x=(unsigned)arena+h[0];builder_y=0x21436587;builder_mode=1;builder_poison=h[2];
#ifdef REC2_MATCHING
 builder_cdecl=0;builder_stack_count=0;
#else
 builder_cdecl=1;builder_stack_count=1;builder_stack[0]=(unsigned)arena+h[0];
#endif
if(BuilderInvoke())return 14;fwrite(arena,65536,1,stdout);}return 0;}
'''
LOADER=r'''
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned char*disk,*image;unsigned n,i,v,h[2],*patch;IMAGE_NT_HEADERS*nt;IMAGE_SECTION_HEADER*s;if(argc!=4)return 1;f=fopen(argv[2],"rb");if(!f)return 2;fseek(f,0,SEEK_END);n=(unsigned)ftell(f);rewind(f);disk=malloc(n);if(!disk||fread(disk,n,1,f)!=1)return 3;fclose(f);nt=(IMAGE_NT_HEADERS*)(disk+((IMAGE_DOS_HEADER*)disk)->e_lfanew);image=VirtualAlloc(NULL,nt->OptionalHeader.SizeOfImage,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);if(!image)return 4;memcpy(image,disk,nt->OptionalHeader.SizeOfHeaders);s=IMAGE_FIRST_SECTION(nt);for(i=0;i<nt->FileHeader.NumberOfSections;i++)memcpy(image+s[i].VirtualAddress,disk+s[i].PointerToRawData,s[i].SizeOfRawData);f=fopen(argv[3],"rb");if(!f||fread(h,sizeof(h),1,f)!=1)return 5;n=h[1];patch=malloc(n*8);if(n&&(!patch||fread(patch,n*8,1,f)!=1))return 6;fclose(f);for(i=0;i<n;i++){memcpy(&v,image+patch[i*2],4);if(v!=patch[i*2+1])return 7;v=(unsigned)image+v-nt->OptionalHeader.ImageBase;memcpy(image+patch[i*2],&v,4);}entry=(unsigned)image+h[0];free(patch);free(disk);return run(argv[1]);}
'''
ORIGINAL_SPEC={'status': 'Authenticated complete original graph only; scratch candidate/native proof pending', 'original_sha256': '9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d', 'rows': [{'name': 'FindInverseAndWorldBox', 'address': '0x4ffd20', 'bytes': 92, 'sha256': '93e1cd001b7881f3fc46331230e2469ce745f9fcb07189d9f2ed47d3bb59f954', 'calls': [{'site': '0x4ffd62', 'target': '0x45f5b0'}, {'site': '0x4ffd6c', 'target': '0x532bd0'}], 'readonly': []}, {'name': 'GetNewBoundingBox', 'address': '0x45f5b0', 'bytes': 213, 'sha256': '9b9393f0eafe1726d1f4387f188fdc713f35f6707e8c62861a4965c8fea1a882', 'calls': [{'site': '0x45f5c1', 'target': '0x5334a0'}], 'readonly': [{'site': '0x45f653', 'address': '0x589fac', 'width': 4, 'bytes': '00000000'}]}, {'name': 'BrMatrix34Inverse', 'address': '0x532bd0', 'bytes': 725, 'sha256': '4e540d0946543f2752c12f0bf0789d81d030a2c0bff39de571fbe8ebb466a8da', 'calls': [], 'readonly': [{'site': '0x532bdd', 'address': '0x58b8bc', 'width': 4, 'bytes': '00000000'}, {'site': '0x532be3', 'address': '0x58b8bc', 'width': 4, 'bytes': '00000000'}, {'site': '0x532bef', 'address': '0x58b8bc', 'width': 4, 'bytes': '00000000'}, {'site': '0x532c1b', 'address': '0x58b8bc', 'width': 4, 'bytes': '00000000'}, {'site': '0x532c37', 'address': '0x58b8bc', 'width': 4, 'bytes': '00000000'}, {'site': '0x532c57', 'address': '0x58b8bc', 'width': 4, 'bytes': '00000000'}, {'site': '0x532c77', 'address': '0x58b8bc', 'width': 4, 'bytes': '00000000'}, {'site': '0x532c94', 'address': '0x58b8bc', 'width': 4, 'bytes': '00000000'}, {'site': '0x532caf', 'address': '0x58b8bc', 'width': 4, 'bytes': '00000000'}, {'site': '0x532cc2', 'address': '0x58b8c0', 'width': 4, 'bytes': '00008034'}, {'site': '0x532cd3', 'address': '0x58b8bc', 'width': 4, 'bytes': '00000000'}, {'site': '0x532ce1', 'address': '0x58b8bc', 'width': 4, 'bytes': '00000000'}, {'site': '0x532cf0', 'address': '0x58b8c4', 'width': 4, 'bytes': '7d1d9026'}, {'site': '0x532cfd', 'address': '0x58b8bc', 'width': 4, 'bytes': '00000000'}, {'site': '0x532d05', 'address': '0x58b8b8', 'width': 4, 'bytes': '0000803f'}]}, {'name': 'BrMatrix34ApplyP', 'address': '0x5334a0', 'bytes': 116, 'sha256': '0684a171263ff17a75be9f00d3afb3a41bc8fc558600c6e72da0453446b6999d', 'calls': [], 'readonly': []}], 'functions': 4, 'original_bytes': 1146, 'dependencies': ['Real canonical GetNewBoundingBox + FinterayScalar/FinterayBoundsAdd', 'Real canonical BrMatrix34ApplyP + Matrix34ApplyProductLoaded/Memory/Add', 'Real canonical BrMatrix34Inverse + ActivationInverseNaN/Add/Read/Scalar/Write; genuine elementary x87 helper used only by accepted arithmetic bodies'], 'source_guard': '55-volume.c/h,34-finteray.c/h,matrix34.c,types/hooks/brender headers and relevant shared transport only;66-tintedpoly.c is deliberately outside this independent graph'}
FROZEN500_INPUT_SHA256='94f46f0ac813acb9f9b5a517d5728dd7cdb4e891077216f4a006b11cbe7e31f3'
"""Strict complete linked closure builder for the future retained volume verifier."""
def linked_packet(binary,symbol_bytes,map_bytes,out,require_root=True):
 import re
 nt,base,size,tab=pe(binary);symbols=json.loads(symbol_bytes);assert symbols['original_sha256']==HASH and symbols['recompiled_sha256']==sha(binary)
 mapping={r['orig']:r['recomp']for r in symbols['symbols']}
 identities={0x4ffd20:('@FindInverseAndWorldBox@4','55-volume.c.obj'),0x45f5b0:('@GetNewBoundingBox@12','34-finteray.c.obj'),0x532bd0:('_BrMatrix34Inverse','libbrender:matrix34.c.obj'),0x5334a0:('_BrMatrix34ApplyP','libbrender:matrix34.c.obj')}
 private={'_ActivationInverseNaN':'libbrender:matrix34.c.obj','_ActivationInverseAdd':'libbrender:matrix34.c.obj','_ActivationInverseRead':'libbrender:matrix34.c.obj','_ActivationInverseScalar':'libbrender:matrix34.c.obj','_ActivationInverseWrite':'libbrender:matrix34.c.obj','_Matrix34ApplyAdd':'libbrender:matrix34.c.obj','_Matrix34ApplyProductLoaded':'libbrender:matrix34.c.obj','_Matrix34ApplyProductMemory':'libbrender:matrix34.c.obj','@FinterayScalar@8':'34-finteray.c.obj','@FinterayBoundsAdd@8':'34-finteray.c.obj'}
 rows={}
 for name,address,owner in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',map_bytes.decode('latin1'),re.M):rows.setdefault(int(address,16),[]).append((name,owner.strip()))
 required=set(identities)if require_root else set(identities)-{0x4ffd20};public={}
 for old in required:
  assert old in mapping,('Missing linked full public root',hex(old));a=mapping[old];assert rows[a]==[identities[old]],('Public MAP identity/owner',hex(old),rows.get(a));public[a]=(old,*identities[old])
 sections=[(*struct.unpack_from('<IIII',binary,tab+40*i+8),struct.unpack_from('<I',binary,tab+40*i+36)[0])for i in range(struct.unpack_from('<H',binary,nt+6)[0])]
 def section(address,n):
  matches=[(vs,va,z,off,flags)for vs,va,z,off,flags in sections if va<=address-base and address-base+n<=va+z];assert len(matches)==1,('Section range',hex(address),n);return matches[0]
 cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;pending=[mapping[0x4ffd20]]if require_root else list(public);seen=set();functions=[];instructions={};patches={};constants={};edges=[]
 while pending:
  start=pending.pop(0)
  if start in seen:continue
  seen.add(start);assert len(rows.get(start,[]))==1,('Ambiguous/unknown MAP function',hex(start));name,owner=rows[start][0]
  if start not in public:assert private.get(name)==owner,('Unapproved private identity/owner',name,owner)
  end=min(a for a in rows if a>start);todo=[start];local={}
  while todo:
   a=todo.pop()
   if a in local:continue
   assert start<=a<end,('Edge outside exact MAP span',name,hex(a));assert section(a,1)[4]&0x20000000
   i=next(cs.disasm(raw(binary,a,min(15,end-a)),a));assert i.mnemonic not in('int','int3','ud2');local[a]=i.bytes.hex();assert a not in instructions;instructions[a]=i.bytes.hex()
   for op in i.operands:
    if op.type==CS_OP_MEM and base<=op.mem.disp<base+size:
     assert not op.mem.base and not op.mem.index,('Unapproved indexed/global operand',name,hex(a),i.op_str)
     assert i.disp_size==4;at=a+i.disp_offset-base;patches[at]=op.mem.disp;seg=section(op.mem.disp,op.size);assert seg[4]&0x40000000 and not seg[4]&0xa0000000,('Data operand not immutable readable non-executable data',hex(op.mem.disp));constants[(op.mem.disp,op.size)]=raw(binary,op.mem.disp,op.size)
    elif op.type==CS_OP_IMM and not(CS_GRP_CALL in i.groups or CS_GRP_JUMP in i.groups)and base<=op.imm<base+size:
     assert i.imm_size==4;assert op.imm not in rows,('Unexpected code pointer',hex(a));seg=section(op.imm,1);assert seg[4]&0x40000000 and not seg[4]&0xa0000000;patches[a+i.imm_offset-base]=op.imm
     # The new root can materialize the whole constant24B unit-box address.
     assert name=='@FindInverseAndWorldBox@4',('Unapproved address-valued data literal',name,hex(a));section(op.imm,24);constant=raw(binary,op.imm,24);assert struct.unpack('<6I',constant)==(0xbf800000,0xbf800000,0xbf800000,0x3f800000,0x3f800000,0x3f800000);constants[(op.imm,24)]=constant
   if i.mnemonic.startswith('ret'):continue
   if CS_GRP_CALL in i.groups or CS_GRP_JUMP in i.groups:
    assert i.operands[0].type==CS_OP_IMM,('No indirect/import/callback edge',name,hex(a),i.op_str);target=i.operands[0].imm
    edges.append(dict(site=hex(a),target=hex(target),kind=i.mnemonic,owner_function=name))
    if start<=target<end:
     assert CS_GRP_CALL not in i.groups,('Unexpected local-call thunk',name,hex(a));todo.append(target)
    else:
     assert target in rows,('Unknown imported/outside callee',hex(target));pending.append(target)
    if i.mnemonic=='jmp':continue
   todo.append(a+i.size)
  functions.append(dict(name=name,owner=owner,address=hex(start),original_address=hex(public[start][0])if start in public else None,map_extent=end-start,instruction_count=len(local),instructions=sorted(local.items()),instruction_sha256=sha(b''.join(bytes.fromhex(h)for _,h in sorted(local.items())))))
 # Every absolute operand touching the loaded image is explicitly transported.
 # PE HIGHLOW active sites must match the same operand set when present. The
 # matching compiler's /FIXED image normally has a stripped relocation table.
 reloc_rva,reloc_size=struct.unpack_from('<II',binary,nt+24+96+5*8);cursor=reloc_rva;active={a-base+k for a,h in instructions.items()for k in range(len(h)//2)};checked=[]
 while cursor<reloc_rva+reloc_size:
  page,z=struct.unpack('<II',raw(binary,base+cursor,8));assert z>=8
  for value in struct.unpack('<'+'H'*((z-8)//2),raw(binary,base+cursor+8,z-8)):
   if value>>12==3:
    site=page+(value&4095)
    if site in active:assert site in patches;checked.append(site)
   else:assert value>>12==0,('Unexpected relocation type',value>>12)
  cursor+=z
 assert cursor==reloc_rva+reloc_size
 if not reloc_size:assert struct.unpack_from('<H',binary,nt+22)[0]&1,'Absent relocations require explicit RELOCS_STRIPPED contract'
 if require_root:assert all(mapping[a]in seen for a in identities)
 entry=mapping[0x4ffd20]if require_root else mapping[0x45f5b0]
 packet=words([entry-base,len(patches)]+[v for at,value in sorted(patches.items())for v in(at,value)]);(out/'matching-packet.bin').write_bytes(packet)
 metadata=dict(recompiled_sha256=sha(binary),symbols_sha256=sha(symbol_bytes),map_sha256=sha(map_bytes),whole_root=require_root,graph=functions,instructions=sorted(instructions.items()),instruction_count=len(instructions),public_identity_grid=[dict(original=hex(old),actual=hex(mapping[old]),symbol=identities[old][0],owner=identities[old][1])for old in sorted(required)],private_identity_grid=[dict(symbol=r['name'],owner=r['owner'],address=r['address'])for r in functions if r['original_address']is None],edges=edges,globals=[],readonly=[dict(address=hex(a),bytes=n,hex=b.hex(),section_flags=hex(section(a,n)[4]))for(a,n),b in sorted(constants.items())],patches=sorted(patches.items()),relocation_directory=[reloc_rva,reloc_size],active_relocations_checked=checked,coff_characteristics=hex(struct.unpack_from('<H',binary,nt+22)[0]),transport_policy='Full loaded PE; all complete reachable function bodies intact. Only authenticated absolute data operand DWORDs rebased into same unchanged image. No callee entry patches, API callbacks, code prefixes, math substitution, success mocks or output normalization.',packet_sha256=sha(packet))
 return metadata

def original_relocation_contract(binary,instructions,patches,spec):
 nt,base,size,tab=pe(binary);active={a-base+k for a,h in instructions for k in range(len(h)//2)};declared={int(r['address'],16):r for r in spec['rows']};constants={}
 for row in spec['rows']:
  for r in row['readonly']:
   key=(int(r['address'],16),r['width']);bits=raw(binary,key[0],key[1]);assert bits.hex()==r['bytes'];constants[key]=bits
 cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
 for a,h in instructions:
  i=next(cs.disasm(bytes.fromhex(h),a))
  for op in i.operands:
   if op.type==CS_OP_MEM and base<=op.mem.disp<base+size:
    assert not op.mem.base and not op.mem.index and (op.mem.disp,op.size)in constants
    assert patches[a+i.disp_offset-base]==op.mem.disp
   elif op.type==CS_OP_IMM and not(CS_GRP_CALL in i.groups or CS_GRP_JUMP in i.groups)and base<=op.imm<base+size:raise AssertionError(('Unexpected original address literal',hex(a),hex(op.imm)))
 rr,rz=struct.unpack_from('<II',binary,nt+24+96+5*8);cursor=rr;checked=[]
 while cursor<rr+rz:
  page,n=struct.unpack('<II',raw(binary,base+cursor,8));assert n>=8
  for value in struct.unpack('<'+'H'*((n-8)//2),raw(binary,base+cursor+8,n-8)):
   if value>>12==3:
    at=page+(value&4095)
    if at in active:assert at in patches;checked.append(at)
   else:assert value>>12==0
  cursor+=n
 assert cursor==rr+rz
 if not rz:assert struct.unpack_from('<H',binary,nt+22)[0]&1
 return dict(relocation_directory=[rr,rz],coff_characteristics=hex(struct.unpack_from('<H',binary,nt+22)[0]),active_relocations_checked=checked,absolute_data_operands=sorted(patches.items()),readonly=[dict(address=hex(a),bytes=n,hex=b.hex())for(a,n),b in sorted(constants.items())],globals=[],policy='All four complete original spans hash-checked; all direct graph edges, exact readonly constants and active HIGHLOW operands authenticated. No runtime/mutable global or callback transport.')

def cases():
 rows=[];identity=[1,0,0,0,1,0,0,0,1,0,0,0]
 def add(matrix=None,**kw):c=dict(matrix=matrix or identity,offset=0x100,repeat=1,poison=0xcccccccc,seed=len(rows)+1,patch=[]);c.update(kw);rows.append(c)
 for m in[identity,[0]*12,[-1,0,0,0,1,0,0,0,-1,3,4,5],[1,.3,.2,-.4,1,.5,.7,-.2,1,.25,-.5,.75]]:
  for offset in[0,1,4,0x100,0xff00]:
   for repeat in[1,2,4]:add(m,offset=offset,repeat=repeat)
 for axis in range(3):
  for bits in[0,0x80000000,1,0x33000000,0x33ffffff,0x34000000,0x34000001,0x347fffff,0x34800000,0x34800001,0x7f7fffff,0x7f800000,0xff800000,0x7fc12345,0x7f812345,0xffc54321]:add(patch=[(axis*3+axis,bits)])
 ieee=[0,0x80000000,1,0x007fffff,0x00800000,0x7f7fffff,0x7f800000,0xff800000,0x7fc12345,0xffc54321,0x7f812345,0xff854321]
 for i in range(12):
  for b in ieee:add(patch=[(i,b)])
 for a,b in[(0x7fc12345,0xffc54321),(0x7f812345,0xff854321),(0x7f800000,0xff800000),(1,0x7f7fffff)]:
  for i in range(9):add(patch=[(i,a),((i+4)%9,b)],repeat=2)
 rng=random.Random(0x4ffd20)
 for _ in range(192):add([rng.uniform(-4,4)for _ in range(9)]+[rng.uniform(-100,100)for _ in range(3)],repeat=2)
 for e in[-126,-60,-24,-22,-1,0,1,30,60,100]:
  for sign in[-1,1]:add([sign*2.**e,0,0,0,2.**(-e//2),0,0,0,1,3,-4,5])
 return rows
def fixture(c):
 arena=bytearray(random.Random(c['seed']).randbytes(65536));at=c['offset']+4;arena[at:at+48]=struct.pack('<12f',*c['matrix'])
 for i,b in c['patch']:arena[at+i*4:at+i*4+4]=words([b])
 return words([c['offset'],c['repeat'],c['poison'],c['seed']])+arena
def main():
 p=argparse.ArgumentParser();p.add_argument('--output',required=True,type=Path);p.add_argument('--original',type=Path,default=R/'build-reference/originals/CARMA2_HW.EXE');p.add_argument('--candidate',type=Path);p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);p.add_argument('--fixtures-dir',type=Path);a=p.parse_args();assert bool(a.recompiled)==bool(a.symbols);out=a.output.resolve();assert not out.exists();out.mkdir(parents=True)
 candidate=(a.candidate or R/'src/carma2/newcommon/55-volume.c').resolve();paths=[candidate,Path(__file__),R/'src/carma2/newcommon/55-volume.c',R/'src/carma2/newcommon/34-finteray.c',R/'src/brender/core/math/matrix34.c',R/'scripts/verify_phil_registry_reconstruction.py',R/'scripts/verify_opponent_navigation_reconstruction.py',R/'scripts/verify_opponent_route_reconstruction.py',R/'scripts/verify_tinted_native_reconstruction.py',a.original.resolve()]+list((R/'src').rglob('*.h'))
 if a.symbols:paths +=[a.recompiled.resolve(),a.recompiled.resolve().with_suffix('.map'),a.symbols.resolve()]
 before={str(p.resolve()):p.read_bytes()for p in paths};(out/'source-before.json').write_text(json.dumps({p:sha(b)for p,b in before.items()},indent=2));binary=before[str(a.original.resolve())];assert sha(binary)==HASH;nt,base,size,tab=pe(binary)
 spec=ORIGINAL_SPEC;roots={int(r['address'],16):r for r in spec['rows']};cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;patches={};instructions=[]
 for address,row in roots.items():
  code=raw(binary,address,row['bytes']);assert sha(code)==row['sha256'];ins=list(cs.disasm(code,address));starts={i.address for i in ins};assert sum(i.size for i in ins)==len(code)
  for i in ins:
   instructions.append((i.address,i.bytes.hex()))
   if CS_GRP_CALL in i.groups or CS_GRP_JUMP in i.groups:assert i.operands[0].type==CS_OP_IMM and i.operands[0].imm in starts|set(roots)
   for op in i.operands:
    if op.type==CS_OP_MEM and base<=op.mem.disp<base+size:assert i.disp_size==4;patches[i.address+i.disp_offset-base]=op.mem.disp
    if op.type==CS_OP_IMM and not(CS_GRP_CALL in i.groups or CS_GRP_JUMP in i.groups)and base<=op.imm<base+size:assert i.imm_size==4;patches[i.address+i.imm_offset-base]=op.imm
 original_transport=original_relocation_contract(binary,instructions,patches,spec);packet=words([0x4ffd20-base,len(patches)]+[v for at,value in sorted(patches.items())for v in(at,value)]);(out/'original-packet.bin').write_bytes(packet)
 matrix=before[str((R/'src/brender/core/math/matrix34.c').resolve())].decode();geometry=before[str((R/'src/carma2/newcommon/34-finteray.c').resolve())].decode()
 native=COMMON+'\n#if defined(_MSC_VER) && _MSC_VER < 1300\n#pragma auto_inline(off)\n#endif\n'
 for n in['Matrix34ApplyAdd','ActivationInverseNaN','ActivationInverseAdd','ActivationInverseRead','ActivationInverseScalar','ActivationInverseWrite']:native+=extract(matrix,n)+'\n'
 native+='#if defined(_MSC_VER) && _MSC_VER < 1300\n#pragma auto_inline(on)\n#endif\n'
 for n in['BrMatrix34ApplyP','BrMatrix34Inverse']:native+=extract(matrix,n)+'\n'
 for n in['FinterayScalar','FinterayBoundsAdd','GetNewBoundingBox']:native+=extract(geometry,n)+'\n'
 native+=extract(before[str(candidate)].decode(),'FindInverseAndWorldBox')+'\nint C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;entry=(unsigned)FindInverseAndWorldBox;return run(argv[1]);}\n'
 caller,_,_=registry.literals();caller=caller.replace('builder_poison;','builder_poison,builder_cdecl;').replace(' mov builder_result,eax\n mov ecx,esp',' mov builder_result,eax\n cmp builder_cdecl,0\n je vol_stack\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n vol_stack:\n mov ecx,esp')
 registry.COMMON=COMMON;commands=registry.compile_sources(out,native,LOADER,caller);rows=cases();assert len(rows)==500 and sha(b''.join(fixture(c)for c in rows))==FROZEN500_INPUT_SHA256;checks=[];fail=[];matching=None
 if a.symbols:matching=linked_packet(before[str(a.recompiled.resolve())],before[str(a.symbols.resolve())],before[str(a.recompiled.resolve().with_suffix('.map'))],out)
 for i,c in enumerate(rows):
  case=out/f'case-{i}.bin';input_=fixture(c)
  if a.fixtures_dir:assert (a.fixtures_dir/f'case-{i}.bin').read_bytes()==input_
  case.write_bytes(input_);expected=None
  for name in['original','msvc5','msvc','mingw']+(['matching']if a.symbols else []):
   cmd=[str(out/'physical.exe'),str(case),str(a.original.resolve()if name=='original'else a.recompiled.resolve()),str(out/('original-packet.bin'if name=='original'else'matching-packet.bin'))]if name in('original','matching')else[str(out/(name+'.exe')),str(case)];run=subprocess.run(cmd,capture_output=True,timeout=30);(out/f'{i}-{name}.bin').write_bytes(run.stdout)
   if name=='original':assert run.returncode==0 and len(run.stdout)==65536*c['repeat'];expected=run.stdout;continue
   row=dict(index=i,compiler=name,exit=run.returncode,passed=run.returncode==0 and run.stdout==expected,expected_sha256=sha(expected),actual_sha256=sha(run.stdout));checks.append(row)
   if not row['passed']:fail.append(row)
  if i%30==0:print(i,len(checks),len(fail),flush=True)
 after={p:sha(Path(p).read_bytes())for p in before};assert after=={p:sha(b)for p,b in before.items()}
 result=dict(passed=not fail,scenes=len(rows),comparisons=len(checks),results=checks,failures=fail,source_stable=True,source_before={p:sha(b)for p,b in before.items()},source_after=after,original=spec,original_transport=original_transport,matching=matching,original_members=['0x4ffd20','0x45f5b0','0x532bd0','0x5334a0'],complete_suite=True,frozen500_input_sha256=FROZEN500_INPUT_SHA256,candidate_path=str(candidate),original_instructions=instructions,original_packet_sha256=sha(packet),patches=sorted(patches.items()),commands=commands,generated_sha256={str(p.resolve()):sha(p.read_bytes())for p in out.iterdir()if p.suffix in['.c','.obj','.exe']},cases=rows,limitations=__doc__)
 result['scene_input_sha256']={str((out/f'case-{i}.bin').resolve()):sha((out/f'case-{i}.bin').read_bytes())for i in range(500)};(out/'verification.json').write_text(json.dumps(result,indent=2));print('DONE',len(rows),len(checks),len(fail));assert not fail
if __name__=='__main__':main()
