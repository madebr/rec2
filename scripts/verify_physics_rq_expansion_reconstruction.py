"""Physical original/native/linked checks for RQ matrix expansion.

RQSingleExpand uses a fixed 100-scalar matrix stride and a 100-scalar private
row. Fixtures cover sizes 0 through 99, raw copies, matrix aliases, scalar
rounding and paired NaNs at PC53. Negative sizes read uninitialized original
stack storage and are explicitly outside these bounded fixtures.

The default extracts actual production source; --candidate selects a complete
development override. --recompiled and --symbols bind an immutable linked PE.
"""
import argparse,ast,hashlib,json,os,random,re,struct,subprocess
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_MEM,CS_OP_IMM

ROOT=Path(__file__).resolve().parents[1]
from verify_tinted_native_reconstruction import extract
p=argparse.ArgumentParser();p.add_argument('--output',required=True);p.add_argument('--expanded',action='store_true');p.add_argument('--limit',type=int);p.add_argument('--candidate',type=Path);p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);args=p.parse_args()
assert bool(args.recompiled)==bool(args.symbols), 'Both immutable PE and symbol map are required'
assert args.limit is None or args.limit > 0, 'A scene limit must be positive'
OUT=ROOT/args.output;OUT.mkdir(parents=True,exist_ok=True)
SOURCE=args.candidate or ROOT/'src/carma2/newcommon/49-physics.c'
PRODUCTION=ROOT/'src/carma2/newcommon/49-physics.c'
NAMES=['RQScalar','RQNaN','RQAccumulate','PartnerAddWide','PartnerMultiplyWide','PartnerMultiplyRawScalar','PartnerReverseSubtractScalar']
candidate_text='\n'.join(extract(PRODUCTION.read_text(),name)for name in NAMES)+'\n'+extract(SOURCE.read_text(),'RQSingleExpand')
paths=[SOURCE,PRODUCTION,ROOT/'src/carma2/newcommon/49-physics.h',ROOT/'src/carma2/include/rec2_types.h',ROOT/'src/carma2/hooks/c2_hooks.h',Path(__file__),ROOT/'scripts/verify_tinted_native_reconstruction.py',ROOT/'scripts/verify_opponent_navigation_reconstruction.py']
source_before={str(p):hashlib.sha256(p.read_bytes()).hexdigest()for p in paths}
spec=[('RQSingleExpand',0x4c4f40,727)]
b=(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes();original_hash=hashlib.sha256(b).hexdigest();assert original_hash=='9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
nt=struct.unpack_from('<I',b,60)[0];table=nt+24+struct.unpack_from('<H',b,nt+20)[0];sz=struct.unpack_from('<I',b,nt+80)[0];flat=bytearray(sz);sections=[]
for k in range(struct.unpack_from('<H',b,nt+6)[0]):
    vs,va,n,off=struct.unpack_from('<IIII',b,table+40*k+8);flat[va:va+n]=b[off:off+n]
cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True;patches=[];entries=[]
for name,start,n in spec:
    body=bytes(flat[start-0x400000:start-0x400000+n]);ins=list(cs.disasm(body,start));assert sum(i.size for i in ins)==n
    entries.append(dict(name=name,address=start,size=n,sha256=hashlib.sha256(body).hexdigest()))
    for i in ins:
        if i.mnemonic=='call':assert i.operands[0].type==CS_OP_IMM and i.operands[0].imm in {a for _,a,_ in spec}
        if i.mnemonic.startswith('j'):assert i.operands[0].type==CS_OP_IMM and start<=i.operands[0].imm<start+n
        for op in i.operands:
            if op.type==CS_OP_MEM and 0x400000<=op.mem.disp<0x400000+sz:
                assert not op.mem.index and not op.mem.base and op.mem.disp in (0x58a8b8,0x58a8c0)
                assert op.size == {0x58a8b8:4,0x58a8c0:8}[op.mem.disp]
                assert bytes(flat[op.mem.disp-0x400000:op.mem.disp-0x400000+op.size]) == bytes(op.size)
                at=i.address-0x400000+i.disp_offset;value=op.mem.disp-0x400000+0x50000000
                patches.append(dict(address=i.address,constant=op.mem.disp,bytes=bytes(flat[op.mem.disp-0x400000:op.mem.disp-0x400000+op.size]).hex()))
                struct.pack_into('<I',flat,at,value)
(OUT/'physical-image.bin').write_bytes(flat)
prefix=r'''
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>
#include "49-physics.h"
#include "c2_hooks.h"
extern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[6],builder_stack_count,builder_result,builder_poison,builder_cdecl;
extern unsigned __cdecl BuilderInvoke(void);
'''
driver=r'''
int C2_HOOK_CDECL main(int argc,char**argv){
 FILE*f;unsigned char*code,*arena;unsigned h[16],values[6],n,i;
 unsigned entry_offsets[1]={0xc4f40};
 if(argc!=3)return 2;SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);
 code=VirtualAlloc((void*)0x50000000,IMAGE_SIZE,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);
 arena=VirtualAlloc((void*)0x20000000,131072,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
 if(code!=(void*)0x50000000||arena!=(void*)0x20000000)return 3;
 f=fopen(argv[2],"rb");if(!f)return 4;if(fread(code,IMAGE_SIZE,1,f)!=1)return 5;fclose(f);
 f=fopen(argv[1],"rb");if(!f)return 6;if(fread(h,sizeof(h),1,f)!=1||fread(arena,131072,1,f)!=1)return 7;fclose(f);
 _controlfp(_PC_53,_MCW_PC);_setmode(_fileno(stdout),_O_BINARY);
 values[0]=(unsigned)(uintptr_t)(arena+h[1]);values[1]=(unsigned)(uintptr_t)(arena+h[2]);
 values[2]=h[5];values[3]=(unsigned)(uintptr_t)(arena+h[3]);values[4]=(unsigned)(uintptr_t)(arena+h[4]);n=5;
#ifdef PHYSICAL_ORIGINAL
 builder_address=(unsigned)(uintptr_t)(code+entry_offsets[h[0]]);
#else
 (void)entry_offsets;
 builder_address=(unsigned)(uintptr_t)RQSingleExpand;
#endif
 builder_mode=2;builder_x=values[0];builder_y=values[1];builder_poison=0xcccccccc;
#if defined(REC2_MATCHING)||defined(PHYSICAL_ORIGINAL)
 builder_cdecl=0;builder_stack_count=n-2;for(i=2;i<n;i++)builder_stack[i-2]=values[i];
#else
 builder_cdecl=1;builder_stack_count=n;for(i=0;i<n;i++)builder_stack[i]=values[i];
#endif
 if(BuilderInvoke())return 14;
 fwrite(&builder_result,4,1,stdout);fwrite(arena,131072,1,stdout);return 0;
}
'''.replace('IMAGE_SIZE',str(sz))
(OUT/'harness.c').write_text(prefix+candidate_text+driver)
(OUT/'original.c').write_text('#define PHYSICAL_ORIGINAL\n'+prefix+driver)
linked_meta=None
if args.recompiled:
 data=args.recompiled.read_bytes();mapping=json.loads(args.symbols.read_text());assert mapping['original_sha256']==original_hash and mapping['recompiled_sha256']==hashlib.sha256(data).hexdigest()
 symbols={x['orig']:x['recomp'] for x in mapping['symbols']};map_path=args.recompiled.with_suffix('.map');map_bytes=map_path.read_bytes()
 functions={int(address,16):(name,owner.strip()) for name,address,owner in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',map_bytes.decode('latin1'),re.M)}
 nt2=struct.unpack_from('<I',data,60)[0];base2=struct.unpack_from('<I',data,nt2+52)[0];size2=struct.unpack_from('<I',data,nt2+80)[0];table2=nt2+24+struct.unpack_from('<H',data,nt2+20)[0]
 flat=bytearray(size2);sections=[]
 for si in range(struct.unpack_from('<H',data,nt2+6)[0]):
  virtual,rva,n,offset=struct.unpack_from('<IIII',data,table2+40*si+8);flags=struct.unpack_from('<I',data,table2+40*si+36)[0];flat[rva:rva+n]=data[offset:offset+n];sections.append((base2+rva,max(virtual,n),flags))
 initial={symbols[address]:name for name,address,_ in spec};todo=list(initial);seen=set();linked_closure=[];patched=set();relocations=[]
 while todo:
  start=todo.pop(0)
  if start in seen:continue
  seen.add(start);end=min(address for address in functions if address>start);body=bytes(flat[start-base2:end-base2]);instructions=list(cs.disasm(body,start));assert sum(i.size for i in instructions)==len(body)
  name=initial.get(start,functions[start][0])
  for ins in instructions:
   if ins.mnemonic=='call' or ins.mnemonic.startswith('j'):
    assert ins.operands[0].type==CS_OP_IMM,(name,ins.op_str)
    target=ins.operands[0].imm
    if not start<=target<end:
     assert target in functions,(name,ins.op_str)
     if target not in initial:assert functions[target][1]=='49-physics.c.obj' and functions[target][0].lstrip('_@').startswith(('RQ','Partner')),(name,functions[target])
     todo.append(target)
   for operand in ins.operands:
    if operand.type==CS_OP_MEM and base2<=operand.mem.disp<base2+size2:
     assert not operand.mem.base and not operand.mem.index and ins.disp_size==4,(name,ins.op_str)
     section=next(sec for sec in sections if sec[0]<=operand.mem.disp<sec[0]+sec[1]);assert not(section[2]&0x80000020),(name,ins.op_str)
     site=ins.address-base2+ins.disp_offset
     if site not in patched:
      relocations.append(dict(site=site,original=operand.mem.disp,width=operand.size,bits=bytes(flat[operand.mem.disp-base2:operand.mem.disp-base2+operand.size]).hex()));struct.pack_into('<I',flat,site,operand.mem.disp-base2+0x50000000);patched.add(site)
  linked_closure.append(dict(name=name,address=start,length=end-start,sha256=hashlib.sha256(body).hexdigest()))
 # Reject any unreviewed address-bearing immediate or other relocation in closure.
 rel_rva,rel_size=struct.unpack_from('<II',data,nt2+24+96+5*8);cursor=rel_rva
 while cursor<rel_rva+rel_size:
  page,n=struct.unpack_from('<II',flat,cursor);assert n>=8
  for word in struct.unpack_from('<'+'H'*((n-8)//2),flat,cursor+8):
   if word>>12==3:
    site=page+(word&4095)
    if any(row['address']-base2<=site<row['address']-base2+row['length'] for row in linked_closure):assert site in patched,hex(site)
  cursor+=n
 (OUT/'matching-image.bin').write_bytes(flat)
 matching_driver=driver.replace(str(sz),str(size2)).replace(','.join(hex(address-0x400000) for _,address,_ in spec[:5]),','.join(hex(symbols[address]-base2) for _,address,_ in spec[:5]))
 (OUT/'matching.c').write_text('#define PHYSICAL_ORIGINAL\n'+prefix+matching_driver)
 linked_meta=dict(recompiled_sha256=hashlib.sha256(data).hexdigest(),symbols_sha256=hashlib.sha256(args.symbols.read_bytes()).hexdigest(),native_map_sha256=hashlib.sha256(map_bytes).hexdigest(),closure=linked_closure,constant_relocations=relocations)
includes=[ROOT/v for v in ('src/brender/include','src/carma2/include','src/carma2/hooks','src/carma2/s3/include','src/carma2/newcommon')]
mroot=ROOT/'build-tools/msvc-modern';msvc=sorted((mroot/'VC/Tools/MSVC').iterdir())[-1];sdk=mroot/'Windows Kits/10';version=sorted((sdk/'Include').iterdir())[-1].name
env=dict(os.environ);env['PATH']=str(msvc/'bin/Hostx64/x86')+os.pathsep+env['PATH'];env['INCLUDE']=os.pathsep.join(map(str,[msvc/'include',*[sdk/f'Include/{version}/{part}' for part in ('ucrt','shared','um')]]));env['LIB']=os.pathsep.join(map(str,[msvc/'lib/x86',*[sdk/f'Lib/{version}/{part}/x86' for part in ('ucrt','um')]]))
m5=ROOT/'build-tools/msvc500sp3';env5=dict(os.environ);env5['PATH']=str(m5/'bin')+os.pathsep+str(m5/'redist')+os.pathsep+env5['PATH'];env5['INCLUDE']=str(m5/'include');env5['LIB']=str(m5/'lib')
gcc=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe'
commands={
 'msvc':[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(q)for q in includes],str(OUT/'harness.c'),'/Fo'+str(OUT/'msvc.obj'),'/Fe'+str(OUT/'msvc.exe')],
 'msvc5':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(q)for q in includes],str(OUT/'harness.c'),'/Fo'+str(OUT/'msvc5.obj'),'/Fe'+str(OUT/'msvc5.exe')],
 'mingw':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation',*['-I'+str(q)for q in includes],str(OUT/'harness.c'),'-o',str(OUT/'mingw.exe')],
 'original':[str(gcc),'-std=gnu89','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation',*['-I'+str(q)for q in includes],str(OUT/'original.c'),'-o',str(OUT/'original.exe')]}
if args.recompiled:commands['matching']=[*commands['original'][:-3],str(OUT/'matching.c'),'-o',str(OUT/'matching.exe')]

# Use the retained ABI checker template without substituting leaf semantics.
caller_tree=ast.parse((ROOT/'scripts/verify_opponent_navigation_reconstruction.py').read_text())
caller_nodes=[node.value.value for node in ast.walk(caller_tree) if isinstance(node,ast.Assign) and isinstance(node.value,ast.Constant) and any(isinstance(target,ast.Name) and target.id=='caller' for target in node.targets)]
assert len(caller_nodes)==1
caller=caller_nodes[0].replace('builder_stack[5]','builder_stack[6]').replace('builder_poison;','builder_poison,builder_cdecl;')
caller=caller.replace(' mov builder_result,eax\n mov ecx,esp',' mov builder_result,eax\n cmp builder_cdecl,0\n je stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n stackready:\n mov ecx,esp')
(OUT/'caller.c').write_text(caller)
for command in commands.values():command.append(str(OUT/'caller.obj'))
commands={'caller':[str(m5/'bin/cl.exe'),'/nologo','/O2','/Zl','/c',str(OUT/'caller.c'),'/Fo'+str(OUT/'caller.obj')],**commands}

for name,cmd in commands.items():
    r=subprocess.run(cmd,cwd=OUT,env=env if name=='msvc'else env5 if name in ('msvc5','caller')else None,capture_output=True,text=True);(OUT/(name+'-build.txt')).write_text(r.stdout+r.stderr)
    if r.returncode:raise RuntimeError(r.stdout+r.stderr)


rng=random.Random(0x4c4f40);cases=[]
def scene(size,kind=0,alias=None,patch=None):
 h=[0,0x1000,0x15000,0xb000,0x1f000,size]+[0]*10
 if alias:
  for key,value in alias.items():h[key]=value
 arena=bytearray(b'\xa5'*131072)
 for at in(h[1],h[2],h[3]):
  for i in range(10000):
   value=rng.uniform(-4,4)if kind==0 else (1.0 if i//100==i%100 else 0.0)if kind==1 else 0.0
   struct.pack_into('<f',arena,at+4*i,value)
 indices=list(range(100))
 if kind==0:rng.shuffle(indices)
 arena[h[4]:h[4]+100]=bytes(indices)
 for offset,value in patch or []:struct.pack_into('<I',arena,offset,value)
 cases.append(dict(mode=0,size=size,kind=kind,alias=alias,patch=patch,header=h,data=bytes(arena)))
for n in(0,1,2,3,5,9,10,20,39,99):
 for kind in range(3):scene(n,kind)
for n in(1,2,3,5,10):
 for i in range(10):scene(n)
if args.expanded:
 for n in(1,3,10):
  for alias in({3:0x1000},{3:0x1004},{2:0x1000},{2:0x1004},{2:0xb000},{2:0xb004}):scene(n,alias=alias)
  for offset in(0x1000,0x1000+404,0xb000,0xb004,0x15000+400*n,0x15000+404*n):
   for value in(0,0x80000000,1,0x7f7fffff,0x7f800000,0xff800000,0x7fc12345,0x7f812345):scene(n,kind=1,patch=[(offset,value)])
if args.expanded:
 for n in(1,2,3,9,10,39):
  source=0x15000+400*n
  for offsets in((0x1000,source),(0xb000,source),(0xb000,0xb004),(source,source+4),(0x1000,0xb000),(0x1000,0x1194)):
   for values in((0x7fc12345,0xffc12345),(0x7fc12345,0x7f812345),(0xff812345,0x7fc12345),(0x7fc54321,0xffc12345),(0x7f800000,0xff800000),(0x7f7fffff,1)):
    scene(n,kind=1,patch=list(zip(offsets,values)))
if args.expanded:
 for n in(1,2,3,9,10,39):
  source=0x15000+400*n
  for offsets in((0x1000,source),(0xb000,source),(0xb000,0xb004),(source,source+4),(0x1000,0xb000),(0x1000,0x1194)):
   for values in((0x7fc12345,0xffc12345),(0x7fc12345,0x7f812345),(0xff812345,0x7fc12345),(0x7fc54321,0xffc12345),(0x7f800000,0xff800000),(0x7f7fffff,1)):
    scene(n,kind=0,patch=list(zip(offsets,values)))
    raw=bytearray(cases[-1]['data']);raw[0x1f000:0x1f064]=bytes(range(100));cases[-1]['data']=bytes(raw);cases[-1]['identity_indices']=True
results=[];failures=[]
for index,c in enumerate(cases[:args.limit]):
    path=OUT/'input.bin';path.write_bytes(struct.pack('<16I',*c['header'])+c['data'])
    oracle=subprocess.run([str(OUT/'original.exe'),str(path),str(OUT/'physical-image.bin')],capture_output=True);assert oracle.returncode==0 and len(oracle.stdout)==131076,(index,oracle.returncode)
    for compiler in ['msvc5','msvc','mingw']+(['matching']if args.recompiled else []):
        actual=subprocess.run([str(OUT/(compiler+'.exe')),str(path),str(OUT/('matching-image.bin'if compiler=='matching' else 'physical-image.bin'))],capture_output=True)
        good=actual.returncode==0 and actual.stdout==oracle.stdout
        r=dict(index=index,compiler=compiler,passed=good,case={k:v for k,v in c.items()if k!='data'});results.append(r)
        if not good:
            r['returncode']=actual.returncode;r['differences']=[dict(offset=i,expected=oracle.stdout[i:i+4].hex(),actual=actual.stdout[i:i+4].hex())for i in range(0,min(len(actual.stdout),len(oracle.stdout)),4)if oracle.stdout[i:i+4]!=actual.stdout[i:i+4]]
            failures.append(r);(OUT/f'failure-{index}-{compiler}.json').write_text(json.dumps(r,indent=2));(OUT/f'failure-{index}.bin').write_bytes(path.read_bytes())
    if index%25==0:print(index+1,'cases,',len(failures),'failures',flush=True)
    if len(failures)>=12:break
source_after={str(p):hashlib.sha256(p.read_bytes()).hexdigest()for p in paths}
assert source_before==source_after,'Source changed during verification'
(OUT/'verifier.py').write_bytes(Path(__file__).read_bytes())
v=dict(entries=entries,patches=patches,original_sha256=original_hash,candidate_sha256=hashlib.sha256(candidate_text.encode()).hexdigest(),source_sha256=source_before,source_stable=True,matching=linked_meta,total_scenarios=len(cases),complete_suite=index+1==len(cases),input_sha256=hashlib.sha256(b''.join(struct.pack('<16I',*c['header'])+c['data']for c in cases[:index+1])).hexdigest(),generated_sha256={p.name:hashlib.sha256(p.read_bytes()).hexdigest()for p in OUT.iterdir()if p.suffix in('.c','.obj','.exe')},scope='Complete RQSingleExpand original entry at PC53, masked FP exceptions. All131072 arena bytes and defined returnEAX compared; ESP, EBX/ESI/EDI/EBP and x87 TOP checked directly. No output normalization. Sizes0..99, stride100 and byte indices0..99. Negative size uses original uninitialized private stack and is excluded. No whole-game or arbitrary invalid-pointer claim.',scenarios=index+1,comparisons=len(results),failures=failures,results=results)
(OUT/'verification.json').write_text(json.dumps(v,indent=2)+'\n');print('finished',index+1,len(results),len(failures))
if failures:raise SystemExit(1)
