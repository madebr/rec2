#!/usr/bin/env python3
"""Compare smoke rendering/comparison with original x86 and native production bodies.

Requires Unicorn2.1.4 and scoped modern x86 MSVC/MinGW. Allocator-free fixtures
compare full memory and every renderer callback snapshot, including callbacks
that mutate pointer arrays, colors, loop counts and the shared blend actor.
Sort ordering is deterministic; CmpSmokeZ is independently executed for normal
and nonfinite pairs. This is not an actual BRender renderer or sort integration.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import struct
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "src/carma2/newcommon/32-spark.c"
ORIGINAL_HASH = "9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d"

ADDRESSES=dict(RenderRecordedSmokeCircles=0x4fb1b0,CmpSmokeZ=0x4fb2f0,
    gBR_smoke_pointers=0x6a8760,gN_BR_smoke_structs=0x6aa56c,gBlend_actor=0x74caac,
    BrQsort=0x528410,BrMaterialUpdate=0x520e70,BrModelUpdate=0x51f950,BrZbsSceneRenderAdd=0x522ea0)
HEAP,STACK,STOP,SIZE=0x1000000,0x1080000,0x1081000,0x13000
ACTORS,MATERIALS,TOKENS,MODELS,VERTICES,SMOKES=0,0x1000,0x4000,0x5000,0x9000,0xe000

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE')
    parser.add_argument('--recompiled',type=Path,default=ROOT/'build-matching/rec2.exe')
    parser.add_argument('--symbols',type=Path)
    parser.add_argument('--dependency-dir',type=Path,default=ROOT/'build-audit/joystick/python-deps')
    parser.add_argument('--gcc',type=Path,default=ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe')
    parser.add_argument('--msvc-root',type=Path,default=ROOT/'build-tools/msvc-modern')
    parser.add_argument('--output',type=Path,default=ROOT/'build-audit/smoke-render')
    args=parser.parse_args();sys.path.insert(0,str(args.dependency_dir.resolve()))
    from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
    from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_EAX,UC_X86_REG_FPCW
    from verify_tinted_native_reconstruction import extract
    original=args.original.read_bytes()
    if hashlib.sha256(original).hexdigest()!=ORIGINAL_HASH:parser.error('Incorrect original binary hash')
    images=[(original,ADDRESSES)]
    if args.symbols:
        mapping=json.loads(args.symbols.read_text());rebuilt=args.recompiled.read_bytes()
        if mapping.get('format')!=1 or mapping.get('original_sha256')!=ORIGINAL_HASH or mapping.get('recompiled_sha256')!=hashlib.sha256(rebuilt).hexdigest():parser.error('Stale or invalid symbol map')
        symbols={s['name']:s for s in mapping['symbols']}
        if any(symbols[n]['orig']!=a for n,a in ADDRESSES.items()):parser.error('Unexpected original symbol address')
        images.append((rebuilt,{n:symbols[n]['recomp'] for n in ADDRESSES}))
    out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)

    def execute(data,addresses,case):
        u=Uc(UC_ARCH_X86,UC_MODE_32);u.mem_map(0x400000,0x500000);u.mem_map(HEAP,0x100000)
        pe=struct.unpack_from('<I',data,0x3c)[0];table=pe+24+struct.unpack_from('<H',data,pe+20)[0]
        for i in range(struct.unpack_from('<H',data,pe+6)[0]):
            _,rva,size,raw=struct.unpack_from('<IIII',data,table+40*i+8)
            u.mem_write(0x400000+rva,data[raw:raw+size])
        def wr(p,v):u.mem_write(p,struct.pack('<I',v&0xffffffff))
        def rd(p):return struct.unpack('<I',u.mem_read(p,4))[0]
        u.mem_write(HEAP,b'\xa5'*SIZE)
        wr(addresses['gBlend_actor'],HEAP);wr(addresses['gN_BR_smoke_structs'],case.get('count',1))
        for i in range(35):
            smoke=HEAP+SMOKES+36*i;material=HEAP+MATERIALS+256*i;model=HEAP+MODELS+256*i
            wr(addresses['gBR_smoke_pointers']+4*i,smoke)
            wr(smoke,0x3f800000+i*0x8000);wr(smoke+4,0xbf800000-i*0x8000);wr(smoke+8,0x40000000+i*0x8000)
            wr(smoke+12,case.get('radius',0x3fc00000));wr(smoke+16,case.get('strength',0x3f000000))
            wr(smoke+20,0x00102030+i*0x010101);wr(smoke+24,case.get('aspect',0x3f800000));wr(smoke+28,material);wr(smoke+32,model)
            wr(material+0x58,HEAP+TOKENS);wr(model+8,HEAP+VERTICES+512*i)
            u.mem_write(model+16,struct.pack('<H',4 if i==0 else i%7))
        u.mem_write(HEAP+TOKENS,struct.pack('<6I',133,1,190,75<<16,0,0))
        calls=[];material_calls=[0];model_calls=[0]
        def snapshot():
            return (struct.pack('<2I',rd(addresses['gBlend_actor']),rd(addresses['gN_BR_smoke_structs']))+
                    bytes(u.mem_read(addresses['gBR_smoke_pointers'],140))+bytes(u.mem_read(HEAP,SIZE))).hex()
        hooks={addresses[n]:n for n in ['BrQsort','BrMaterialUpdate','BrModelUpdate','BrZbsSceneRenderAdd']}
        def hook(uc,at,size,context):
            if at not in hooks:return
            name=hooks[at];sp=uc.reg_read(UC_X86_REG_ESP);a=[rd(sp+4+4*i) for i in range(4)]
            if name=='BrQsort':
                if a[0]!=addresses['gBR_smoke_pointers'] or a[2:]!=[4,addresses['CmpSmokeZ']]:raise AssertionError('Sort callback arguments differ')
                calls.append(['Q',a[1],a[2],1,snapshot()])
                if case.get('shuffle'):
                    values=[rd(a[0]+4*i) for i in range(35)]
                    for i,value in enumerate(reversed(values)):wr(a[0]+4*i,value)
                if case.get('sort_count',-1)>=0:wr(addresses['gN_BR_smoke_structs'],case['sort_count'])
            elif name=='BrMaterialUpdate':
                calls.append(['M',a[0],a[1],snapshot()])
                if material_calls[0]==0 and case.get('mutate',0)&1:
                    i=(a[0]-HEAP-MATERIALS)//256;old=HEAP+SMOKES+i*36
                    wr(old+20,0x1234ab56)
                    pointers=[rd(addresses['gBR_smoke_pointers']+4*j) for j in range(35)]
                    index=pointers.index(old)
                    wr(addresses['gBR_smoke_pointers']+4*index,HEAP+SMOKES+34*36)
                    wr(addresses['gN_BR_smoke_structs'],0)
                    wr(addresses['gBlend_actor'],HEAP+256)
                material_calls[0]+=1
            elif name=='BrModelUpdate':
                calls.append(['U',a[0],a[1],snapshot()])
                if model_calls[0]==0 and case.get('mutate',0)&2:
                    wr(addresses['gBlend_actor'],HEAP+256)
                    wr(HEAP+SMOKES+34*36+32,HEAP+MODELS+33*256)
                model_calls[0]+=1
            else:calls.append(['R',a[0],snapshot()])
            uc.reg_write(UC_X86_REG_EAX,0);uc.reg_write(UC_X86_REG_EIP,rd(sp));uc.reg_write(UC_X86_REG_ESP,sp+4)
        u.hook_add(UC_HOOK_CODE,hook);u.reg_write(UC_X86_REG_FPCW,0x37f)
        wr(STACK,STOP);u.reg_write(UC_X86_REG_ESP,STACK)
        if case['mode']==0:
            wr(HEAP+SMOKES+8,case['a']);wr(HEAP+SMOKES+36+8,case['b'])
            wr(STACK+4,addresses['gBR_smoke_pointers']);wr(STACK+8,addresses['gBR_smoke_pointers']+4)
        u.emu_start(addresses['CmpSmokeZ' if case['mode']==0 else 'RenderRecordedSmokeCircles'],STOP,count=100000)
        if u.reg_read(UC_X86_REG_EIP)!=STOP:raise AssertionError('Instruction budget exhausted')
        value=u.reg_read(UC_X86_REG_EAX) if case['mode']==0 else None
        return dict(calls=calls,state=snapshot(),value=value)

    source=SOURCE.read_text();bodies=[extract(source,n) for n in ['CmpSmokeZ','RenderRecordedSmokeCircles']]
    driver=r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "32-spark.h"
#include "18-graphics2.h"
#include "rec2_macros.h"
typedef char layouts[sizeof(void*)==4&&sizeof(tBRender_smoke)==36&&sizeof(br_vertex)==40&&sizeof(br_model)==92&&sizeof(br_material)==156&&sizeof(br_actor)==100?1:-1];
tBRender_smoke* gBR_smoke_pointers[35];int gN_BR_smoke_structs;br_actor* gBlend_actor;
static unsigned arena[0x13000/4];static int sort_count,shuffle,mutate,material_calls,model_calls;
static unsigned norm(unsigned v){unsigned b=(unsigned)(uintptr_t)arena;return v>=b&&v<b+sizeof(arena)?v-b+0x1000000:v;}
static void wr(unsigned p,unsigned v){memcpy((char*)arena+p,&v,4);}
static void normalize(unsigned char*p,unsigned off){unsigned v;memcpy(&v,p+off,4);v=norm(v);memcpy(p+off,&v,4);}
static void snapshot(void){unsigned char copy[sizeof(arena)];unsigned i;unsigned v;
    memcpy(copy,arena,sizeof(copy));normalize(copy,0x18);normalize(copy,0x1c);normalize(copy,0x118);normalize(copy,0x11c);
    for(i=0;i<35;i++){normalize(copy,0x1000+256*i+0x58);normalize(copy,0x5000+256*i+8);normalize(copy,0xe000+36*i+28);normalize(copy,0xe000+36*i+32);}
    v=norm((unsigned)(uintptr_t)gBlend_actor);for(i=0;i<4;i++)printf("%02x",((unsigned char*)&v)[i]);
    v=(unsigned)gN_BR_smoke_structs;for(i=0;i<4;i++)printf("%02x",((unsigned char*)&v)[i]);
    for(i=0;i<35;i++){unsigned j;v=norm((unsigned)(uintptr_t)gBR_smoke_pointers[i]);for(j=0;j<4;j++)printf("%02x",((unsigned char*)&v)[j]);}
    for(i=0;i<sizeof(copy);i++)printf("%02x",copy[i]);
}
static int events;
static void event(const char*name,unsigned a,unsigned b,int n){printf("%s[\"%s\"",events++?",":"",name);if(n>0)printf(",%u",a);if(n>1)printf(",%u",b);if(n==3)printf(",1");printf(",\"");snapshot();printf("\"]");}
void C2_HOOK_CDECL BrQsort(void*base,unsigned nelems,unsigned size,br_qsort_cbfn cmp){int i;if(base!=gBR_smoke_pointers||size!=4||cmp!=CmpSmokeZ)abort();event("Q",nelems,size,3);if(shuffle)for(i=0;i<17;i++){tBRender_smoke*p=gBR_smoke_pointers[i];gBR_smoke_pointers[i]=gBR_smoke_pointers[34-i];gBR_smoke_pointers[34-i]=p;}if(sort_count>=0)gN_BR_smoke_structs=sort_count;}
void C2_HOOK_CDECL BrMaterialUpdate(br_material*mat,br_uint_16 flags){event("M",norm((unsigned)(uintptr_t)mat),flags,2);if(material_calls++==0&&(mutate&1)){int i,index=-1;unsigned k=((unsigned)(uintptr_t)mat-(unsigned)(uintptr_t)arena-0x1000)/256;tBRender_smoke*old=(tBRender_smoke*)((char*)arena+0xe000+k*36);old->col=0x1234ab56;for(i=0;i<35;i++)if(gBR_smoke_pointers[i]==old){index=i;break;}if(index<0)abort();gBR_smoke_pointers[index]=(tBRender_smoke*)((char*)arena+0xe000+34*36);gN_BR_smoke_structs=0;gBlend_actor=(br_actor*)((char*)arena+256);}}
void C2_HOOK_CDECL BrModelUpdate(br_model*model,br_uint_16 flags){event("U",norm((unsigned)(uintptr_t)model),flags,2);if(model_calls++==0&&(mutate&2)){gBlend_actor=(br_actor*)((char*)arena+256);((tBRender_smoke*)((char*)arena+0xe000+34*36))->model=(br_model*)((char*)arena+0x5000+33*256);}}
void C2_HOOK_CDECL BrZbsSceneRenderAdd(br_actor*actor){event("R",norm((unsigned)(uintptr_t)actor),0,1);}
'''
    driver+='\n\n'.join(bodies)+'\n'
    driver+=r'''
int main(int argc,char**argv){unsigned i;int mode;unsigned strength,aspect,radius,a,b,value=0;
    if(argc!=11)return 1;
    mode=atoi(argv[1]);gN_BR_smoke_structs=atoi(argv[2]);strength=(unsigned)strtoul(argv[3],NULL,0);aspect=(unsigned)strtoul(argv[4],NULL,0);radius=(unsigned)strtoul(argv[5],NULL,0);sort_count=atoi(argv[6]);shuffle=atoi(argv[7]);mutate=atoi(argv[8]);a=(unsigned)strtoul(argv[9],NULL,0);b=(unsigned)strtoul(argv[10],NULL,0);
    memset(arena,0xa5,sizeof(arena));gBlend_actor=(br_actor*)arena;
    for(i=0;i<35;i++){unsigned p=0xe000+36*i;unsigned nv=i?i%7:4;gBR_smoke_pointers[i]=(tBRender_smoke*)((char*)arena+p);wr(p,0x3f800000+i*0x8000);wr(p+4,0xbf800000-i*0x8000);wr(p+8,0x40000000+i*0x8000);wr(p+12,radius);wr(p+16,strength);wr(p+20,0x102030+i*0x10101);wr(p+24,aspect);wr(p+28,(unsigned)(uintptr_t)((char*)arena+0x1000+256*i));wr(p+32,(unsigned)(uintptr_t)((char*)arena+0x5000+256*i));wr(0x1000+256*i+0x58,(unsigned)(uintptr_t)((char*)arena+0x4000));wr(0x5000+256*i+8,(unsigned)(uintptr_t)((char*)arena+0x9000+512*i));memcpy((char*)arena+0x5000+256*i+16,&nv,2);}
    wr(0x4000,133);wr(0x4004,1);wr(0x4008,190);wr(0x400c,75<<16);wr(0x4010,0);wr(0x4014,0);
    if(mode==0){wr(0xe008,a);wr(0xe02c,b);}
    printf("{\"calls\":[");if(mode==0)value=(unsigned)CmpSmokeZ(&gBR_smoke_pointers[0],&gBR_smoke_pointers[1]);else RenderRecordedSmokeCircles();printf("],\"state\":\"");snapshot();printf("\",\"value\":");if(mode==0)printf("%u",value);else printf("null");printf("}\n");return 0;
}
'''
    harness=out/'harness.c';harness.write_text(driver)
    includes=[ROOT/p for p in ['src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/carma2/platform','src/carma2/s3/include','src/brender/include']]
    msvc_root=args.msvc_root.resolve();msvc=sorted((msvc_root/'VC/Tools/MSVC').iterdir())[-1];sdk=msvc_root/'Windows Kits/10';sdk_version=sorted(p.name for p in (sdk/'Include').iterdir())[-1]
    env=dict(os.environ);env['PATH']=str(msvc/'bin/Hostx64/x86')+os.pathsep+env['PATH']
    env['INCLUDE']=os.pathsep.join(map(str,[msvc/'include',*[sdk/f'Include/{sdk_version}/{p}' for p in ['ucrt','shared','um']]]))
    env['LIB']=os.pathsep.join(map(str,[msvc/'lib/x86',*[sdk/f'Lib/{sdk_version}/{p}/x86' for p in ['ucrt','um']]]))
    commands={
        'msvc':[str(msvc/'bin/Hostx64/x86/cl.exe'),'/nologo','/O2','/W3','/WX','/D_CRT_SECURE_NO_WARNINGS',*['/I'+str(p) for p in includes],str(harness),'/Fo'+str(out/'msvc.obj'),'/Fe'+str(out/'msvc.exe')],
        'mingw':[str(args.gcc.resolve()),'-std=gnu89','-O2','-Wall','-Wextra','-Werror',*['-I'+str(p) for p in includes],str(harness),'-o',str(out/'mingw.exe')]
    }
    for compiler,command in commands.items():
        result=subprocess.run(command,cwd=out,env=env if compiler=='msvc' else None,text=True,capture_output=True)
        (out/(compiler+'-build.log')).write_text(result.stdout+result.stderr)
        if result.returncode:raise AssertionError(result.stdout+result.stderr)
    cases=[]
    pair_bits=[0,0x80000000,0x3f800000,0xbf800000,0x7f800000,0xff800000,0x7fc12345,0x7f800001]
    for a in pair_bits:
        for b in pair_bits:cases.append(dict(mode=0,a=a,b=b))
    for count in (0,1,35):cases.append(dict(mode=1,count=count))
    for strength in [0,0x80000000,1,0x80000001,0x3eaaaaab,0xbf800000,0x47ffffff,0x48000001,0x4b800001,0x53800000,0x7f7fffff,0x7f800000,0xff800000,0x7fc12345,0x7f800001]:
        cases.append(dict(mode=1,count=1,strength=strength))
    for aspect in (0,0x80000000,0xbf800000,0x7f800000,0x7fc12345):cases.append(dict(mode=1,count=1,aspect=aspect))
    for mutate in (1,2,3):cases.append(dict(mode=1,count=3,mutate=mutate))
    cases+= [dict(mode=1,count=0,sort_count=1),dict(mode=1,count=35,sort_count=0),dict(mode=1,count=3,shuffle=True,mutate=3)]
    results=[]
    for index,case in enumerate(cases):
        expected=execute(*images[0],case)
        values=[case['mode'],case.get('count',1),case.get('strength',0x3f000000),case.get('aspect',0x3f800000),case.get('radius',0x3fc00000),case.get('sort_count',-1),int(case.get('shuffle',False)),case.get('mutate',0),case.get('a',0),case.get('b',0)]
        checks=[]
        if len(images)>1:checks.append(('recompiled-x86',execute(*images[1],case)))
        for compiler in commands:
            checks.append((compiler,json.loads(subprocess.check_output([str(out/(compiler+'.exe')),*map(str,values)],text=True))))
        for compiler,actual in checks:
            if actual!=expected:
                path=out/f'failure-{compiler}-{index}.json';path.write_text(json.dumps(dict(case=case,original=expected,actual=actual),indent=2))
                raise AssertionError(f'{compiler} smoke render case{index} differs: {path}')
            results.append(dict(case=case,compiler=compiler,passed=True,state_sha256=hashlib.sha256(bytes.fromhex(actual['state'])).hexdigest(),calls=len(actual['calls']),value=actual['value']))
    (out/'verification.json').write_text(json.dumps(dict(original_sha256=ORIGINAL_HASH,source_sha256=hashlib.sha256(SOURCE.read_bytes()).hexdigest(),
        bodies_sha256=[hashlib.sha256(b.encode()).hexdigest() for b in bodies],compiler_commands=commands,cases=results),indent=2)+'\n')
    print(f'PASS: {len(cases)} scenarios, {len(results)} complete x86/native smoke-render comparisons.')

if __name__=='__main__':main()
