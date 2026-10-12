"""Complete automatic-volume original/native3/optional actual executable proof.
Bounded masked-PC53 Win32 x86 inputs; genuine complete geometry and raw public
arena comparisons. No exhaustive, unmasked/sticky-FPU, private-stack, arbitrary
foreign-memory, callback substitution or whole-game equivalence claim.
"""
import sys,json,struct,random,subprocess,argparse,hashlib,re
from pathlib import Path
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'src/carma2').is_dir())
sys.path.insert(0,str(ROOT/'scripts'))
import verify_physics_registration_reconstruction as registration
import verify_phil_registry_reconstruction as registry
from verify_opponent_route_reconstruction import raw,HASH,sha,words,pe
from verify_tinted_native_reconstruction import extract
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_GRP_CALL,CS_GRP_JUMP,CS_GRP_RET
from capstone.x86 import CS_OP_IMM,CS_OP_MEM,CS_OP_REG
BASE=0x24000000;SIZE=0x10000
ROOTS=[('TestAutoSpecialVolume',0x4ff5d0,765),('FUN_0041e480',0x41e480,238),('CheckSingleFace',0x45ecc0,1035)]
LOWER=[('CheckSingleFace',0x45ecc0,1035),('BrMatrix34ApplyP',0x5334a0,116),('EnablePlingMaterials',0x45d5c0,11),('DisablePlingMaterials',0x45d5d0,11)]
MUTABLE={0x761f44:('gDefault_water_spec_vol_real',0xf000,4),0x6793d0:('gCollision_skip_pling_faces',0xf004,4),0x5964c0:('gPling_materials',0xf008,4),0x744820:('gFace_list__car',0x4000,300*80)}


COMMON='\n#include <windows.h>\n#include <stdio.h>\n#include <stdlib.h>\n#include <string.h>\n#include <stddef.h>\n#include <stdint.h>\n#include <float.h>\n#include <math.h>\n#include <io.h>\n#include <fcntl.h>\n#include "55-volume.h"\n#include "34-finteray.h"\n#include "00-car.h"\n#include "rec2_macros.h"\n#include "c2_math.h"\nstatic unsigned char*arena;\nstatic unsigned addresses[3],header[10],case_index;\nextern unsigned builder_address,builder_mode,builder_x,builder_y,builder_stack[6],builder_stack_count,builder_result,builder_poison,builder_cdecl;\nextern unsigned __cdecl BuilderInvoke(void);\n#define gDefault_water_spec_vol_real (*(tSpecial_volume**)(arena+0xf000))\n#define gCollision_skip_pling_faces (*(int*)(arena+0xf004))\n#define gPling_materials (*(int*)(arena+0xf008))\n#define gFace_list__car ((tFace_ref*)(arena+0x4000))\ntypedef char AutoLayouts[sizeof(void*)==4&&sizeof(tPhysics_object)==1240&&sizeof(tFace_ref)==80&&offsetof(tPhysics_object,bb2)==0x3c&&offsetof(tPhysics_object,water_normal)==0x244&&offsetof(tPhysics_object,box_face_start)==0x174?1:-1];\nstatic void snapshot(unsigned event,unsigned result,unsigned detail) {\n    unsigned h[4];h[0]=case_index;h[1]=event;h[2]=result;h[3]=detail;\n    fwrite(h,16,1,stdout);fwrite(arena,0x10000,1,stdout);fflush(stdout);\n}\nstatic LONG WINAPI observe_fault(EXCEPTION_POINTERS* e) {\n    if(e->ExceptionRecord->ExceptionCode!=EXCEPTION_ACCESS_VIOLATION)return EXCEPTION_CONTINUE_SEARCH;\n    fprintf(stderr,"AV eip %08lx address %08lx kind %lu\\n",(unsigned long)e->ExceptionRecord->ExceptionAddress,(unsigned long)e->ExceptionRecord->ExceptionInformation[1],(unsigned long)e->ExceptionRecord->ExceptionInformation[0]);\n    snapshot(1,e->ExceptionRecord->ExceptionCode,(unsigned)e->ExceptionRecord->ExceptionInformation[1]);\n    ExitProcess(0);return EXCEPTION_CONTINUE_SEARCH;\n}\nstatic void install_observer(void) {\n    typedef void*(WINAPI *install_fn)(ULONG,LONG(WINAPI*)(EXCEPTION_POINTERS*));\n    FARPROC address=GetProcAddress(GetModuleHandleA("kernel32.dll"),"AddVectoredExceptionHandler");install_fn install;\n    memcpy(&install,&address,4);if(!install||!install(1,observe_fault))exit(38);\n}\nstatic int run(const char* path) {\n    FILE* f;unsigned mode;\n    SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS);\n    f=fopen(path,"rb");if(!f)return 10;\n    if(VirtualAlloc((void*)0x23ff0000,0x30000,MEM_RESERVE,PAGE_NOACCESS)!=(void*)0x23ff0000)return 39;\n    arena=(unsigned char*)VirtualAlloc((void*)0x24000000,0x10000,MEM_COMMIT,PAGE_READWRITE);\n    if(arena!=(void*)0x24000000)return 11;\n    _setmode(_fileno(stdout),_O_BINARY);install_observer();\n    for(case_index=0;fread(header,sizeof(header),1,f)==1;case_index++) {\n        if(fread(arena,0x10000,1,f)!=1)return 12;\n        mode=header[0];if(mode>=3)return 37;\n        _controlfp(_PC_53,_MCW_PC);_clearfp();\n        builder_address=addresses[mode];builder_mode=mode==1?0:1;\n        builder_x=header[1];builder_y=header[2];builder_poison=header[6];\n#if defined(REC2_MATCHING)\n        builder_cdecl=0;builder_stack_count=mode==0?0:mode==1?3:4;\n        builder_stack[0]=header[3];builder_stack[1]=header[4];builder_stack[2]=header[5];builder_stack[3]=header[8];\n#else\n        builder_cdecl=1;builder_stack_count=mode==0?1:mode==1?5:6;\n        builder_stack[0]=header[1];builder_stack[1]=header[2];builder_stack[2]=header[3];builder_stack[3]=header[4];builder_stack[4]=header[5];builder_stack[5]=header[8];\n#endif\n        if(BuilderInvoke())exit(14);\n        snapshot(0,builder_result,0);\n    }\n    fclose(f);return 0;\n}\n'

def bridge():
 caller,physical,_=registry.literals()
 caller=caller.replace('builder_poison;','builder_poison,builder_cdecl;')
 caller=caller.replace('builder_stack[5]','builder_stack[6]')
 old=' mov builder_result,eax\n mov ecx,esp'
 assert old in caller
 caller=caller.replace(old,' mov builder_result,eax\n cmp builder_cdecl,0\n je volume_stackready\n mov ecx,builder_stack_count\n lea esp,[esp+ecx*4]\n volume_stackready:\n mov ecx,esp')
 # This graph has no replaced terminal/callback entry.
 a=physical.index('image[hdr[12]]=0xe9;');b=physical.index('free(patch);',a)
 physical=physical[:a]+physical[b:]
 physical=physical.replace('value,relative,hdr','value,hdr')
 physical=physical.replace('hdr[14]','hdr[5]').replace('n=hdr[13]','n=hdr[3]').replace('i<12;i++)addresses','i<3;i++)addresses')
 return caller,physical

def cases():
 rows=[]
 def add(**kw):
  c=dict(mode=0,water=0.,auto=0,default=1,normal=(0.,1.,0.),translation=(0.,0.,0.),bounds=(-1.,-1.,-1.,1.,1.,1.),face_count=1,start=0,ignore=0,pling=1,identifier='!water',identifier_byte=None,flags=0,material_flags=0,face_y=0.,patches=[],byte_patches=[],position=0xe000,direction=0xe010,output_normal=0xe020,distance=0xe030,seed=len(rows)+1)
  c.update(kw);rows.append(c)
 for mode in range(2):
  for count in(0,1,2,3):
   for identifier in('!water','>water','?water','#water','0water','9water','plain'):
    add(mode=mode,face_count=count,identifier=identifier)
 for default in(0,1):
  for auto in(0,1):
   for water in(-2.,-1.,-.999999,0.,.999,1.,2.):add(default=default,auto=auto,water=water)
 for ignore in(0,1,0xffffffff):
  for flags in(0,0x80,0xff):
   for pling in(0,1):add(mode=1,ignore=ignore,flags=flags,pling=pling)
 for normal in((1.,0.,0.),(0.,0.,1.),(1.,1.,1.),(-1.,-1.,-1.),(0.,0.,0.)):
  for water in(-1.,0.,1.):add(normal=normal,water=water)
 for start,count in((0,0),(0,1),(1,1),(1,2),(299,300),(300,300)):
  for mode in(0,1):add(mode=mode,start=start,face_count=count)
 for value in(0,0x80000000,1,0x80000001,0x7f7fffff,0x7f800000,0xff800000,0x7f812345,0xff812345,0x7fc12345,0xffc12345):
  for at in(0x244,0x248,0x24c,0x250):add(patches=[(0x100+at,value)])
 for i in range(96):
  r=random.Random(0x64a00+i)
  add(water=r.randrange(-32,32)/8,normal=tuple(r.randrange(-16,16)/8 for j in range(3)),translation=tuple(r.randrange(-16,16)/8 for j in range(3)))
 ieee=(0,0x80000000,1,0x80000001,0x00800000,0x3f800001,0xbf800001,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7f812345,0xff812345,0x7fc12345,0xffc12345)
 for at in range(12):
  for value in ieee:add(patches=[(0x100+0x8c+4*at,value)])
 for at in range(6):
  for value in ieee:add(patches=[(0x100+0x3c+4*at,value)])
 for left,right in((0x244,0x248),(0x244,0x24c),(0x248,0x24c),(0x248,0x250),(0x8c,0x244),(0x90,0x248),(0xb4,0x248)):
  for a in(0x7f812345,0xff812345,0x7fc12345,0xffc12345):
   for b in(0x7f854321,0xff854321,0x7fc54321,0xffc54321):add(patches=[(0x100+left,a),(0x100+right,b)])
 for i in range(128):
  r=random.Random(0xace100+i);values=[r.randrange(-32,32)/8 for j in range(12)]
  add(patches=[(0x100+0x8c+j*4,struct.unpack('<I',struct.pack('<f',v))[0])for j,v in enumerate(values)],identifier='!water')
 # Public output aliases influence later genuine face tests and live range reads.
 for distance in(0x100+0x174,0x100+0x176,0xe000,0xe004,0xe010,0xe014,0xe020,0xf004,0xf008):
  for count in(1,2,3):add(mode=1,distance=distance,face_count=count,identifier='0water',pling=0)
 for normal in(0x100+0x170,0x100+0x172,0x100+0x174,0x100+0x176,0xe000,0xe004,0xe010,0xe014,0xe028,0xf000,0xf004,0xf008):
  for count in(1,2,3):add(mode=1,output_normal=normal,face_count=count,identifier='0water',pling=0)
 for at in(0xe000,0xe004,0xe008,0xe010,0xe014,0xe018):
  for value in ieee:add(mode=1,patches=[(at,value)],face_count=3,identifier='0water',pling=0)
 for flags in(0x800,0x1000,0x1800):
  for direction in((0.,-2.,0.),(0.,2.,0.)):
   add(mode=1,material_flags=flags,patches=[(0xe014,struct.unpack('<I',struct.pack('<f',direction[1]))[0])],identifier='0water',pling=0)
 for value in range(256):add(mode=1,identifier_byte=value,pling=0)
 for i in range(3):
  for value in ieee:
   for at in(0x4000+52+4*i,0x4000+4+4*i,0xe000+4*i,0xe010+4*i):add(mode=2,identifier='0water',pling=0,patches=[(at,value)])
 for flags in(0,0x800,0x1000,0x1800,0xffffffff):
  for identifier in('!water','>water','?water','plain'):
   for pling in(0,1):
    for y in(-2.,2.):add(mode=2,identifier=identifier,pling=pling,material_flags=flags,patches=[(0xe014,struct.unpack('<I',struct.pack('<f',y))[0])])
 return rows

def payload(c):
 r=random.Random(c['seed']);arena=bytearray(r.randbytes(SIZE))
 def put(at,value):arena[at:at+4]=words([value])
 def vec(at,values):arena[at:at+4*len(values)]=struct.pack('<'+'f'*len(values),*values)
 put(0xf000,BASE+0xf100 if c['default'] else 0);put(0xf004,c['ignore']);put(0xf008,c['pling'])
 vec(0x100+0x3c,c['bounds']);vec(0x100+0x8c,(1,0,0,0,1,0,0,0,1,*c['translation']))
 vec(0x100+0x244,c['normal']);vec(0x100+0x250,(c['water'],));put(0x100+0x25c,BASE+0xf100 if c['auto'] else 0)
 struct.pack_into('<HH',arena,0x100+0x174,c['start'],c['face_count'])
 for i in range(300):
  at=0x4000+i*80;put(at,BASE+0xd000);vec(at+4,(-10,c['face_y'],-10,10,c['face_y'],-10,0,c['face_y'],10));vec(at+52,(0,1,0));arena[at+64]=c['flags']
  put(0xd004,BASE+0xd100);put(0xd020,c['material_flags']);arena[0xd100:0xd100+len(c['identifier'])+1]=c['identifier'].encode()+b'\0'
 if c['identifier_byte'] is not None:arena[0xd100]=c['identifier_byte']
 vec(0xe000,(0,1,0));vec(0xe010,(0,-2,0));vec(0xe020,(3,4,5));vec(0xe030,(12,))
 for at,v in c['patches']:put(at,v)
 for at,v in c['byte_patches']:arena[at]=v
 h=[c['mode'],BASE+0x100 if c['mode']==0 else BASE+c['position'],BASE+c['direction'],BASE+c['output_normal'],BASE+c['distance'],BASE+0x100,0x21436587,0,0,0]
 if c['mode']==2:h=[2,BASE+0x4000+c['start']*80,BASE+c['position'],BASE+c['direction'],BASE+c['output_normal'],BASE+c['distance'],0x21436587,0,BASE+0xe040,0]
 return words(h)+arena

def original_packet(data,out):
    """Preserve the tested whole six-body packet, including the raw ADD base."""
    assert sha(data)==HASH
    registration.ROOTS=ROOTS;registration.MUTABLE=MUTABLE;registration.BASE=BASE
    registration.TABLES={};registration.INDIRECT=set();registration.EXIT=0x44c230
    original=registration.original_packet(data,{addr:(name,n)for name,addr,n in ROOTS+LOWER},out)
    assert raw(data,0x41e4b5,6)==bytes.fromhex('81c720487400')
    assert not any(row[0]==0x41e4b7-0x400000 for row in original['patches'])
    original['patches'].append((0x41e4b7-0x400000,0x744820,BASE+0x4000,1))
    original['add_immediate_authority']={'site':'0x41e4b5','bytes':'81c720487400','target':'gFace_list__car'}
    assert len(original['graph'])==6 and sum(row['bytes']for row in original['graph'])==2176
    assert not original['indirect_sites']and not original['tables']
    packet=words([addr-0x400000 for name,addr,n in ROOTS]+[len(original['patches']),0x44c230-0x400000])+b''.join(words(v)for v in sorted(original['patches']))
    (out/'original-addresses.bin').write_bytes(packet);original['packet_sha256']=sha(packet)
    (out/'original-packet.json').write_text(json.dumps(original,indent=2))
    return original

def source_paths(source_dir):
    directory=source_dir if source_dir else ROOT/'src/carma2/newcommon'
    return {name:(directory/name).resolve()for name in('00-car.c','55-volume.c','34-finteray.c')}

def native_source(before,paths,candidate,lower_candidate):
    geom=before[str(paths['34-finteray.c'])].decode()
    matrix=before[str((ROOT/'src/brender/core/math/matrix34.c').resolve())].decode()
    typedef=geom[geom.rfind('typedef struct {',0,geom.index('} tFinteray_projection;')):geom.index('} tFinteray_projection;')+len('} tFinteray_projection;')]
    geometry_names=('FinterayScalar','FinterayFaceProjection','FinterayFacePoint','CheckSingleFace','EnablePlingMaterials','DisablePlingMaterials')
    body=typedef+'\n'+'\n'.join(extract(geom,n)for n in geometry_names)+'\n'+extract(matrix,'BrMatrix34ApplyP')
    if candidate:
        volume_body=before[str(candidate)].decode()
        mode='Explicit scratch complete candidate; never production-default'
        shared=[]
    else:
        car=before[str(paths['00-car.c'])].decode().replace('\r\n','\n')
        volume=before[str(paths['55-volume.c'])].decode().replace('\r\n','\n')
        pattern=r'#if defined\(__GNUC__\)\s+typedef tU32 tVolumeWord[^\n]*;\s*#else\s*typedef tU32 tVolumeWord;\s*#endif'
        c_alias=re.search(pattern,car);v_alias=re.search(pattern,volume)
        assert c_alias and v_alias and c_alias.group(0)==v_alias.group(0),'shared alias identity'
        shared=[]
        for name in('AutoVolumeCopy','AutoVolumeUnordered'):
            a=extract(car,name);b=extract(volume,name);assert a==b,(name,'two-owner helper mismatch')
            shared.append(dict(name=name,sha256=sha(a.encode())))
        prefix=volume[v_alias.start():volume.index('static double C2_HOOK_CDECL AutoVolumeRow')].rstrip()
        # Retain the complete MSVC5 off/on scalar capsule context. The helper
        # bytes and two-owner duplicated alias are checked before one emission.
        assert prefix.count('#pragma auto_inline(off)')==prefix.count('#pragma auto_inline(on)')==1
        volume_body=prefix+'\n'+extract(car,'FUN_0041e480')+'\n'+extract(volume,'AutoVolumeRow')+'\n'+extract(volume,'TestAutoSpecialVolume')
        mode='Exact canonical two-TU bodies and shared private alias/helper identity; merged native capsule'
    if lower_candidate:
        original_lower=extract(geom,'CheckSingleFace')
        candidate_lower=extract(before[str(lower_candidate)].decode(),'CheckSingleFace')
        assert candidate_lower==original_lower.replace('(material->flags & (BR_MATF_ALWAYS_VISIBLE | BR_MATF_TWO_SIDED))','(*(const volatile br_uint_32*)&material->flags & (BR_MATF_ALWAYS_VISIBLE | BR_MATF_TWO_SIDED))')
        assert body.count(original_lower)==1;body=body.replace(original_lower,candidate_lower)
    else:
        assert '*(const volatile br_uint_32*)&material->flags' in extract(geom,'CheckSingleFace'),'full original DWORD material flags must be present'
    body+='\n'+volume_body
    driver='\nint C2_HOOK_CDECL main(int argc,char**argv){if(argc!=2)return 1;addresses[0]=(unsigned)TestAutoSpecialVolume;addresses[1]=(unsigned)FUN_0041e480;addresses[2]=(unsigned)CheckSingleFace;return run(argv[1]);}\n'
    return COMMON+'\n'+body+driver,dict(mode=mode,shared_helpers=shared,geometry_names=geometry_names,
        lower_flags_width=4,combined_body_sha256=sha(body.encode()),
        limits='Combined capsule preserves extracted bodies and scalar pragma context; it does not assert whole production translation-unit compiler-context identity. Actual PE uses genuine whole-TU code.')


def matching_packet(image_path,symbol_path,before,out):
    data=image_path.read_bytes();mapping=json.loads(symbol_path.read_text())
    assert mapping['original_sha256']==HASH and mapping['recompiled_sha256']==sha(data)
    pairs={}
    for row in mapping['symbols']:
        original=row['orig'];actual=row['recomp']
        assert original not in pairs or pairs[original]==actual,'conflicting symbol mapping'
        pairs[original]=actual
    map_path=image_path.with_suffix('.map');map_bytes=map_path.read_bytes();text=map_bytes.decode('latin1')
    aliases={}
    for name,address,owner in re.findall(r'^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(.+)$',text,re.M):
        aliases.setdefault(int(address,16),set()).add((name,owner.strip()))
    nt,base,size,tab=pe(data);cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
    sections=[]
    for i in range(struct.unpack_from('<H',data,nt+6)[0]):
        vs,rva,n,offset=struct.unpack_from('<IIII',data,tab+40*i+8)
        sections.append((base+rva,max(vs,n),struct.unpack_from('<I',data,tab+40*i+36)[0]))
    def section(address,width):
        found=[s for s in sections if s[0]<=address and address+width<=s[0]+s[1]]
        assert len(found)==1,(hex(address),width,'cross-section operand')
        return found[0]
    owners={0x761f44:'<common>',0x6793d0:'<common>',0x5964c0:'34-finteray.c.obj',0x744820:'<common>'}
    source_owners={0x761f44:'55-volume.c',0x6793d0:'00-car.c',0x5964c0:'34-finteray.c',0x744820:'00-car.c'}
    mutable={};globals_=[];all_sources='\n'.join(v.decode('latin1')for p,v in before.items()if p.endswith('.c'))
    for original,(name,offset,n)in MUTABLE.items():
        rows=re.findall(r'^\s*000[23]:[0-9a-fA-F]{8}\s+_'+re.escape(name)+r'\s+([0-9a-fA-F]{8})\s+(.+)$',text,re.M)
        assert len(rows)==1,(name,rows)
        address=int(rows[0][0],16);owner=rows[0][1].strip();assert owner==owners[original]
        assert original not in pairs or pairs[original]==address,(name,'MAP/symbol mismatch')
        declaration=r'// GLOBAL: CARMA2_HW 0x00'+format(original,'06x')+r'\s+[^;{}]*\b'+re.escape(name)+r'\b'
        own_sources=[v.decode('latin1')for p,v in before.items()if Path(p).name==source_owners[original]]
        assert len(own_sources)==1 and len(re.findall(declaration,own_sources[0]))==1,(name,'canonical owning-source identity')
        assert len(re.findall(declaration,all_sources))==1,(name,'duplicate source identity')
        s=section(address,n);assert s[2]&0x80000000 and not s[2]&0x20,'mutable storage must be non-code writable'
        mutable[address]=(BASE+offset,n);pairs[original]=address
        globals_.append(dict(original=original,name=name,address=address,owner=owner,bytes=n,arena_offset=offset))
    public_identities={'TestAutoSpecialVolume':('@TestAutoSpecialVolume@4','55-volume.c.obj'),
        'FUN_0041e480':('@FUN_0041e480@20','00-car.c.obj'),
        'CheckSingleFace':('@CheckSingleFace@24','34-finteray.c.obj'),
        'BrMatrix34ApplyP':('_BrMatrix34ApplyP','libbrender:matrix34.c.obj'),
        'EnablePlingMaterials':('@EnablePlingMaterials@0','34-finteray.c.obj'),
        'DisablePlingMaterials':('@DisablePlingMaterials@0','34-finteray.c.obj')}
    public={pairs[a]:public_identities[name]for name,a,n in ROOTS+LOWER}
    private={
        '00-car.c.obj':{'_AutoVolumeCopy','_AutoVolumeUnordered'},
        '55-volume.c.obj':{'_AutoVolumeCopy','_AutoVolumeScalar','_AutoVolumeUnordered','_AutoVolumeAdd','_AutoVolumeRow'},
        '34-finteray.c.obj':{'@FinterayScalar@8','@FinterayFaceProjection@12','@FinterayFacePoint@12'},
    }
    roots=[pairs[a]for name,a,n in ROOTS];todo=list(roots);seen=set();graph=[];edges=[];patches={};constants={};classified=[]
    def patch(at,value,target,absolute,owner):
        row=(at-base,value,target,int(absolute));assert at-base not in patches or patches[at-base]==row
        patches[at-base]=row;classified.append(dict(owner=owner,operand_site=at,original_value=value,target=target,absolute=bool(absolute)))
    def transport(value,width,ins,owner):
        possible=[(a,d,n)for a,(d,n)in mutable.items()if a<=value< a+n]
        if possible:
            assert len(possible)==1
            a,d,n=possible[0];assert value+width<=a+n,'operand crosses canonical storage'
            return d+value-a,True
        s=section(value,width);assert not s[2]&0x80000000 and not s[2]&0x20,('unclassified writable/code data operand',owner,ins.op_str,hex(value))
        # No table dispatch or unbounded indexed constant is allowed. If a
        # future compiler emits one, it must acquire an explicit full-domain
        # table authority before this loader can accept that graph.
        constants.setdefault(hex(value),{})[str(width)]=raw(data,value,width).hex()
        return value-base,False
    while todo:
        start=todo.pop(0)
        if start in seen:continue
        assert start in aliases and len(aliases[start])==1,('unknown/ambiguous MAP code identity',hex(start),aliases.get(start))
        seen.add(start);name,owner=next(iter(aliases[start]));plain=name.lstrip('_@').split('@')[0]
        if start in public:assert (name,owner)==public[start],(name,owner,public[start])
        else:assert name in private.get(owner,set()),('unapproved emitted lower helper',name,owner)
        ends=[a for a in aliases if a>start];assert ends;end=min(ends)
        s=section(start,1);assert s[2]&0x20 and end<=s[0]+s[1]
        decoded={};occupied={};successors={};pending=[start]
        while pending:
            address=pending.pop()
            while address not in decoded:
                assert start<=address<end,(name,hex(address),'outside MAP extent')
                ins=next(cs.disasm(raw(data,address,15),address));assert ins.size and address+ins.size<=end
                assert all(a not in occupied for a in range(address,address+ins.size)),('overlapping decode',name,hex(address))
                for a in range(address,address+ins.size):occupied[a]=address
                decoded[address]=ins;successors[address]=[]if CS_GRP_RET in ins.groups or ins.mnemonic in('int','int3')else[address+ins.size]
                control=CS_GRP_CALL in ins.groups or CS_GRP_JUMP in ins.groups
                if CS_GRP_CALL in ins.groups:
                    assert ins.mnemonic=='call'and len(ins.operands)==1 and ins.operands[0].size==4
                    assert not any(p in(0x26,0x2e,0x36,0x3e,0x64,0x65,0x66)for p in ins.prefix),'call override/far form'
                if control:
                    assert len(ins.operands)==1 and ins.operands[0].type==CS_OP_IMM,('unexpected indirect/table dispatch; no callback substitution',name,ins.op_str)
                    target=ins.operands[0].imm;inside=start<=target<end
                    if inside:
                        if CS_GRP_CALL in ins.groups:assert target==start,'middle-entry internal call'
                        else:pending.append(target);successors[address].append(target)
                    else:
                        assert target in aliases,('unknown external control target',name,hex(target));todo.append(target)
                    edges.append(dict(owner=start,site=address,target=target,call=CS_GRP_CALL in ins.groups))
                    if ins.mnemonic=='jmp':successors[address]=[target]if inside else[]
                for op in ins.operands:
                    value=None
                    if op.type==CS_OP_MEM and base<=op.mem.disp<base+size:
                        assert not op.mem.segment and ins.disp_size==4,'segmented/short image operand'
                        if op.mem.base or op.mem.index:
                            assert any(a<=op.mem.disp<a+n for a,(d,n)in mutable.items()),'unbounded indexed readonly operand'
                        value=op.mem.disp;at=address+ins.disp_offset;width=op.size
                    elif op.type==CS_OP_IMM and not control and base<=op.imm<base+size:
                        assert ins.mnemonic in('mov','push','add','cmp'),'unclassified scalar/pointer immediate'
                        assert ins.imm_size==4
                        value=op.imm;at=address+ins.imm_offset;width=1
                    if value is None:continue
                    target,absolute=transport(value,width,ins,name);patch(at,value,target,absolute,start)
                if CS_GRP_RET in ins.groups or ins.mnemonic in('jmp','int','int3'):break
                address+=ins.size
        assert all(t in decoded for ts in successors.values()for t in ts),'CFG successor not decoded instruction start'
        graph.append(dict(name=plain,map_name=name,owner=owner,address=start,extent=end-start,
            instructions=[(a,decoded[a].bytes.hex())for a in sorted(decoded)],
            instruction_sha256=sha(b''.join(decoded[a].bytes for a in sorted(decoded)))))
    active={a+k for row in graph for a,bits in row['instructions']for k in range(len(bits)//2)}
    rr,rz=struct.unpack_from('<II',data,nt+24+96+5*8);cursor=base+rr;checked=[]
    while cursor<base+rr+rz:
        page,n=struct.unpack('<II',raw(data,cursor,8));assert n>=8 and n%2==0 and cursor+n<=base+rr+rz
        for word in struct.unpack('<'+'H'*((n-8)//2),raw(data,cursor+8,n-8)):
            at=page+(word&4095)
            if word>>12==3 and base+at in active:assert at in patches;checked.append(at)
        cursor+=n
    blob=words([a-base for a in roots]+[len(patches),0])+b''.join(words(v)for _,v in sorted(patches.items()))
    (out/'matching-addresses.bin').write_bytes(blob)
    return dict(recompiled_sha256=sha(data),symbols_sha256=sha(symbol_path.read_bytes()),map_sha256=sha(map_bytes),
        graph=graph,edges=edges,patches=list(patches.values()),constants=constants,mutable_globals=globals_,
        callbacks=[],tables=[],relocations_checked=checked,classified_operands=classified,packet_sha256=sha(blob),
        limits='No indirect calls/table dispatch are accepted. Complete active operands plus available HIGHLOW checks are required; stripped relocation absence is not pointer completeness evidence.')


def main():
    p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True)
    p.add_argument('--limit',type=int);p.add_argument('--package-only',action='store_true')
    p.add_argument('--case-index',type=int,action='append');p.add_argument('--candidate-source',type=Path)
    p.add_argument('--lower-candidate',type=Path);p.add_argument('--source-dir',type=Path)
    p.add_argument('--recompiled',type=Path);p.add_argument('--symbols',type=Path);a=p.parse_args()
    assert bool(a.recompiled)==bool(a.symbols)
    assert not(a.recompiled and(a.source_dir or a.candidate_source or a.lower_candidate)),'Actual proof requires canonical production-default extraction'
    assert not a.source_dir or a.package_only,'Preview source-dir is package-only'
    paths=source_paths(a.source_dir.resolve()if a.source_dir else None)
    candidate=a.candidate_source.resolve()if a.candidate_source else None
    lower=a.lower_candidate.resolve()if a.lower_candidate else None
    guarded=[Path(__file__).resolve(),*paths.values(),(ROOT/'src/brender/core/math/matrix34.c').resolve()]
    guarded += [ROOT/'scripts'/n for n in('verify_physics_registration_reconstruction.py','verify_phil_registry_reconstruction.py','verify_opponent_navigation_reconstruction.py','verify_opponent_route_reconstruction.py','verify_tinted_native_reconstruction.py','inventory_reconstruction_backlog.py')]
    guarded+=list((ROOT/'src').rglob('*.h'))
    if candidate:guarded.append(candidate)
    if lower:guarded.append(lower)
    if a.recompiled:guarded += [a.recompiled.resolve(),a.symbols.resolve(),a.recompiled.with_suffix('.map').resolve()]
    guarded=list(dict.fromkeys(f.resolve()for f in guarded))
    before={str(f):f.read_bytes()for f in guarded}
    out=a.output.resolve();assert not out.exists();out.mkdir(parents=True)
    snap=out/'sources';snap.mkdir();snapshots={}
    for i,(path,b)in enumerate(before.items()):
        target=snap/(str(i)+'-'+Path(path).name);target.write_bytes(b)
        snapshots[path]={'path':str(target),'sha256':sha(b)}
    data=(ROOT/'build-reference/originals/CARMA2_HW.EXE').read_bytes();original=original_packet(data,out)
    native,capsule=native_source(before,paths,candidate,lower)
    matching=matching_packet(a.recompiled.resolve(),a.symbols.resolve(),before,out)if a.recompiled else None
    caller,physical=bridge();registry.COMMON='#define PHYSICAL_IMAGE\n'+COMMON
    rows=cases();assert len(rows)==1454
    indices=a.case_index or list(range(len(rows)));indices=indices[:a.limit]if a.limit else indices
    assert len(set(indices))==len(indices)and all(0<=i<len(rows)for i in indices)
    payloads=[payload(rows[i])for i in indices]
    package=dict(stage='Package only; no acceptance',sources={p:sha(b)for p,b in before.items()},source_snapshots=snapshots,
        original=original,original_sha256=HASH,matching=matching,native_capsule=capsule,
        total_scenes=len(rows),selected_indices=indices,fixture_sha256=[sha(b)for b in payloads],
        complete_suite=indices==list(range(len(rows))),complete_entries=ROOTS,
        relocated_storage_qualifiers=[i for i in indices if i in(779,780,781,806,807,808)],
        limitations=[__doc__,'Full raw arena and EAX integer result; normal stack/nonvolatile/x87 TOP contract is checked by the retained caller. Void returns are ignored consistently.',
            'Terminal AV compares address and complete public-state prefix, without normal-return ABI or arbitrary foreign-memory claim.',
            'Indices779..781 and806..808 are controlled relocated-storage/material-flags DWORD-width qualifiers only: live output aliases inflate the face end, reach index300 beyond the legal300-record array, and read seeded synthetic adjacency as a material pointer. They do not establish unmodified actual game adjacent-global layout equivalence. Legal array-range claims require indices0..299.',
            capsule['limits']])
    (out/'package.json').write_text(json.dumps(package,indent=2)+'\n')
    (out/'native.c').write_text(native);(out/'caller.c').write_text(caller);(out/'physical.c').write_text(registry.COMMON+physical)
    if a.package_only:
        assert all(Path(f).read_bytes()==b for f,b in before.items());return
    try:commands=registry.compile_sources(out,native,physical,caller)
    except Exception:
        (out/'compile-guard.json').write_text(json.dumps(dict(source_stable=all(Path(f).read_bytes()==b for f,b in before.items()),sources=package['sources']),indent=2));raise
    results=[];failures=[]
    for i,bits in zip(indices,payloads):
        case=out/'case.bin';case.write_bytes(bits);expected=None
        runs=[('original',[str(out/'physical.exe'),str(case),str(ROOT/'build-reference/originals/CARMA2_HW.EXE'),str(out/'original-addresses.bin')])]+[(name,[str(out/(name+'.exe')),str(case)])for name in('msvc5','msvc','mingw')]
        if matching:runs.append(('matching',[str(out/'physical.exe'),str(case),str(a.recompiled.resolve()),str(out/'matching-addresses.bin')]))
        for name,cmd in runs:
            r=subprocess.run(cmd,capture_output=True,timeout=30)
            (out/f'{i}-{name}.bin').write_bytes(r.stdout);(out/f'{i}-{name}.stderr.txt').write_bytes(r.stderr)
            if name=='original':
                assert r.returncode==0 and r.stdout and len(r.stdout)%(16+SIZE)==0,(i,r.returncode)
                expected=r.stdout;continue
            row=dict(index=i,compiler=name,exit=r.returncode,passed=r.returncode==0 and r.stdout==expected,
                fixture_sha256=sha(bits),actual_output_sha256=sha(r.stdout),original_output_sha256=sha(expected),
                actual_output_bytes=len(r.stdout),original_output_bytes=len(expected),stderr=r.stderr.decode(errors='replace'))
            results.append(row)
            if not row['passed']:failures.append(row)
        if i%25==0:print(i,len(results),len(failures),flush=True)
    stable=all(Path(f).read_bytes()==b for f,b in before.items());assert stable
    package.update(stage='Complete original/native3/current executable proof'if matching else'Complete original/native3 proof',
        source_stable=stable,scenes=len(indices),comparisons=len(results),results=results,failures=failures,commands=commands,
        artifact_sha256={str(f):sha(f.read_bytes())for f in out.iterdir()if f.suffix in('.c','.obj','.exe','.bin','.txt')})
    (out/'verification.json').write_text(json.dumps(package,indent=2)+'\n')
    print('DONE',len(indices),len(results),len(failures));assert not failures

if __name__=='__main__':main()
