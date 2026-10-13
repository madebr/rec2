#!/usr/bin/env python3
"""ResetCarSpecialVolume/PHIL activation through the genuine finite raycast chain.

FindFace and its seven-entry geometry closure are actual production/original
bodies. The inherited geometry fixture uses its explicitly documented BRender
transform-dispatch boundary and canonicalizes only unused inverse-matrix zero
signs. No volume/object/nearest-hit output is normalized beyond pointer bases.
The separate physical registry verifier covers raw IEEE and callback mutations.
"""
import argparse,hashlib,json,os,struct,subprocess,sys
from pathlib import Path
from verify_tinted_native_reconstruction import extract
import verify_finteray_reconstruction as geometry

ROOT=Path(__file__).resolve().parents[1]
PHIL=ROOT/'src/carma2/newcommon/60-phil.c'
VOLUME=ROOT/'src/carma2/newcommon/55-volume.c'
ADDRESSES=dict(geometry.ADDRESSES,ResetCarSpecialVolume=0x4ff530,
 PHILMakeObjectActive=0x4b6090,gPHIL_disabled=0x65d004,
 gDefault_water_spec_vol_real=0x761f44)
REFERENCE=geometry.HEAP+0x6000
SIZE,ACTOR,INFO,VOLUME_OFFSET=1660,1240,1340,1440

class VolumeFixture(geometry.Fixture):
 def __init__(self,image,addresses,case):
  super().__init__(image,addresses,case)
  o=case['options'];self.u.mem_write(REFERENCE,b'\xa5'*SIZE)
  self.write(REFERENCE,REFERENCE+ACTOR)
  self.u.mem_write(REFERENCE+ACTOR+0x50,struct.pack('<3f',*o['rp']))
  self.write(REFERENCE+0x19c,0xa5a5)
  self.u.mem_write(REFERENCE+0x239,b'\x01')
  self.write(REFERENCE+0x240,REFERENCE+INFO)
  self.write(REFERENCE+INFO,REFERENCE)
  self.u.mem_write(REFERENCE+INFO+4,struct.pack('<H',0xa5a5))
  self.write(REFERENCE+INFO+8,1)
  self.write(addresses['gPHIL_disabled'],o['disabled'])
  self.write(addresses['gDefault_water_spec_vol_real'],REFERENCE+VOLUME_OFFSET if o['water'] else 0)
 def execute(self):
  from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_ECX,UC_X86_REG_EDX
  o=self.case['options']
  for _ in range(o['repeat']):
   args=[geometry.STOP]
   if o['active']:args += [geometry.HEAP+0x50,o['flag']]
   self.u.mem_write(geometry.STACK,geometry.words(args));self.u.reg_write(UC_X86_REG_ESP,geometry.STACK)
   self.u.reg_write(UC_X86_REG_ECX,REFERENCE);self.u.reg_write(UC_X86_REG_EDX,geometry.HEAP+0x40 if o['active'] else 0)
   self.u.emu_start(self.addresses['PHILMakeObjectActive' if o['active'] else 'ResetCarSpecialVolume'],geometry.STOP,count=1000000)
   assert self.u.reg_read(UC_X86_REG_EIP)==geometry.STOP,'Volume geometry budget'
   assert self.u.reg_read(UC_X86_REG_ESP)==geometry.STACK+len(args)*4,'Volume ABI cleanup'
  owner=bytes(self.u.mem_read(REFERENCE,SIZE))
  return geometry.words([0,0])+self.snapshot()+geometry.words([self.read(self.addresses['gPHIL_disabled']),self.read(self.addresses['gDefault_water_spec_vol_real'])])+owner

def transform_driver(source):
 replacements={'int main(int argc,char** argv)':'int C2_HOOK_CDECL main(int argc,char** argv)',
  'unsigned header[11]':'unsigned header[16]',
  '    switch(header[0]){':'    volume_init(header+11);\n    switch(header[0]){',
  '    default:return 90;}':'    case 7:for(i=0;i<header[15];i++){if(header[11])PHILMakeObjectActive(&volume_owner.object,(br_vector3*)(a+0x40),(br_vector3*)(a+0x50),(int)header[14]);else ResetCarSpecialVolume(&volume_owner.object);}break;\n    default:return 90;}',
  '    emit(0);emit((unsigned)r);snapshot();return 0;':'    emit(0);emit((unsigned)r);snapshot();volume_emit();return 0;'}
 for before,after in replacements.items():
  assert source.count(before)==1,before
  source=source.replace(before,after)
 return source

def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE');p.add_argument('--dependency-dir',type=Path,default=ROOT/'build-audit/joystick/python-deps');p.add_argument('--symbols',type=Path);p.add_argument('--recompiled',type=Path);p.add_argument('--output',type=Path,default=ROOT/'build-audit/phil-volume-geometry');a=p.parse_args()
 assert bool(a.symbols)==bool(a.recompiled);sys.path.insert(0,str(a.dependency_dir.resolve()));original=a.original.read_bytes();assert hashlib.sha256(original).hexdigest()==geometry.ORIGINAL_SHA256
 rebuilt=None;rebuilt_addresses={}
 if a.symbols:
  mapping=json.loads(a.symbols.read_text());rebuilt=a.recompiled.read_bytes();assert mapping['format']==1 and mapping['original_sha256']==geometry.ORIGINAL_SHA256 and mapping['recompiled_sha256']==hashlib.sha256(rebuilt).hexdigest()
  symbols={s['orig']:s['recomp'] for s in mapping['symbols']};rebuilt_addresses={n:symbols[v] for n,v in ADDRESSES.items()}
 out=a.output.resolve();out.mkdir(parents=True,exist_ok=True)
 extra=r'''
#include "60-phil.h"
#include "55-volume.h"
int gPHIL_disabled;
tSpecial_volume* gDefault_water_spec_vol_real;
typedef struct {tPhysics_object object;br_actor actor;tPHIL_queued_object_info info;tSpecial_volume water;} volume_heap;
typedef char volume_layout[sizeof(volume_heap)==1660&&offsetof(volume_heap,actor)==1240&&offsetof(volume_heap,info)==1340&&offsetof(volume_heap,water)==1440?1:-1];
static volume_heap volume_owner;
static void volume_init(unsigned*);
static void volume_emit(void);
'''+ '\n'.join(extract(PHIL.read_text(),n) for n in ('PhilCopyWords','PHILMakeObjectActive'))+'\n'+'\n'.join(extract(VOLUME.read_text(),n) for n in ('VolumeIdentifierHasLength','VolumeBelowSurfaceLimit','ResetCarSpecialVolume'))
 driver=r'''
static unsigned volume_norm(unsigned value){unsigned base=(unsigned)(uintptr_t)&volume_owner;return value>=base&&value<base+sizeof(volume_owner)?value-base+0x1006000:value;}
static void volume_init(unsigned*h){memset(&volume_owner,0xa5,sizeof(volume_owner));volume_owner.object.actor=&volume_owner.actor;memcpy(&volume_owner.actor.t.t.mat.m[3][0],(char*)arena+0x40,12);volume_owner.object.flags=0xa5a5;volume_owner.object.field_0x239=1;volume_owner.object.field_0x240=&volume_owner.info;volume_owner.info.collision_info=&volume_owner.object;volume_owner.info.field_0x4=0xa5a5;volume_owner.info.field_0x8=1;gPHIL_disabled=(int)h[2];gDefault_water_spec_vol_real=h[1]?&volume_owner.water:NULL;}
static void volume_emit(void){unsigned v;unsigned char copy[sizeof(volume_owner)];emit((unsigned)gPHIL_disabled);emit(volume_norm((unsigned)(uintptr_t)gDefault_water_spec_vol_real));memcpy(copy,&volume_owner,sizeof(copy));v=volume_norm((unsigned)(uintptr_t)volume_owner.object.actor);memcpy(copy,&v,4);v=volume_norm((unsigned)(uintptr_t)volume_owner.object.field_0x240);memcpy(copy+0x240,&v,4);v=volume_norm((unsigned)(uintptr_t)volume_owner.object.auto_special_volume);memcpy(copy+0x25c,&v,4);v=volume_norm((unsigned)(uintptr_t)volume_owner.info.collision_info);memcpy(copy+1340,&v,4);fwrite(copy,sizeof(copy),1,stdout);}
'''
 commands=geometry.compile_native(out,ROOT/'build-tools/winlibs-i686/mingw32/bin/gcc.exe',ROOT/'build-tools/msvc-modern',extra,driver,transform_driver)
 # Add the matching compiler to the same actual production-body harness.
 old=ROOT/'build-tools/msvc500sp3';env=dict(os.environ);env['PATH']=str(old/'bin')+os.pathsep+str(old/'redist')+os.pathsep+env['PATH'];env['INCLUDE']=str(old/'include');env['LIB']=str(old/'lib');inc=[ROOT/x for x in ('src/carma2/newcommon','src/carma2/include','src/carma2/hooks','src/brender/include','src/carma2/s3/include','src/carma2/platform')]
 command=[str(old/'bin/cl.exe'),'/nologo','/O2','/Ob2','/G5','/Gr','/ML','/W3','/WX','/DREC2_MATCHING','/I'+str(ROOT/'src/msvc500/include'),*['/I'+str(x) for x in inc],str(out/'harness.c'),'/Fo'+str(out/'msvc5.obj'),'/Fe'+str(out/'msvc5.exe')]
 result=subprocess.run(command,cwd=out,env=env,text=True,capture_output=True);(out/'msvc5-build.log').write_text(result.stdout+result.stderr);assert not result.returncode,result.stdout+result.stderr;commands['msvc5']=command
 cases=[]
 for identifier in ('!','#','@','normal',''):
  for position in ([.25,-2,.25],[2,-2,2],[.25,2,.25],[0,-2,0],[.5,-2,.5]):
   for active,water,disabled,flag in ((0,0,0,0),(0,1,0,0),(1,1,0,0),(1,1,0,1),(1,1,1,1)):
    cases.append(dict(mode=7,identifier=identifier,rp=position,rd=[0,200,0],axis=1,sign=-1,active=active,water=water,disabled=disabled,flag=flag,repeat=2))
 results=[]
 for index,o in enumerate(cases):
  case=geometry.make_case(o);expected=VolumeFixture(original,ADDRESSES,case).execute();path=out/'case.bin';path.write_bytes(geometry.words([7,case['callback_return'],case['mutate'],*case['globals'],0,o['active'],o['water'],o['disabled'],o['flag'],o['repeat']])+case['data'])
  checks=[(compiler,subprocess.check_output([str(out/f'{compiler}.exe'),str(path)])) for compiler in commands]
  if rebuilt is not None:checks.append(('matching-PE',VolumeFixture(rebuilt,rebuilt_addresses,case).execute()))
  for compiler,actual in checks:
   if actual!=expected:
    first=next(i for i,(x,y) in enumerate(zip(expected,actual)) if x!=y) if len(expected)==len(actual) else -1;(out/f'failure-{index}-{compiler}.json').write_text(json.dumps(dict(options=o,first_byte=first,original=expected.hex(),actual=actual.hex()),indent=2));raise AssertionError((index,compiler,first))
   results.append(dict(case=index,compiler=compiler,output_sha256=hashlib.sha256(actual).hexdigest()))
  # Interior finite hit really reaches the material and canonical water object.
  if o['rp']==[.25,-2,.25] and o['identifier'] in ('!','#') and o['water'] and not o['disabled']:
   assert struct.unpack_from('<I',expected,len(expected)-SIZE+0x25c)[0]==REFERENCE+VOLUME_OFFSET
  if (index+1)%25==0:print(f'Verified {index+1}/{len(cases)} actual volume geometry scenes',flush=True)
 paths=[PHIL,PHIL.with_suffix('.h'),VOLUME,VOLUME.with_suffix('.h'),ROOT/'src/carma2/newcommon/00-car.c',ROOT/'src/carma2/newcommon/34-finteray.c',ROOT/'src/brender/core/math/matrix34.c',ROOT/'src/carma2/include/rec2_types.h',ROOT/'scripts/verify_finteray_reconstruction.py',Path(__file__).resolve()]
 manifest=dict(scenarios=len(cases),comparisons=len(results),all_cases_passed=True,cases=cases,results=results,original_sha256=geometry.ORIGINAL_SHA256,recompiled_sha256=hashlib.sha256(rebuilt).hexdigest() if rebuilt else None,symbols_sha256=hashlib.sha256(a.symbols.read_bytes()).hexdigest() if a.symbols else None,source_sha256={str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in paths},compiler_commands=commands,harness_sha256=hashlib.sha256((out/'harness.c').read_bytes()).hexdigest(),limitations=__doc__)
 (out/'verification.json').write_text(json.dumps(manifest,indent=2)+'\n');print(f'PASS {len(cases)} scenes /{len(results)} actual volume geometry comparisons')

if __name__=='__main__':main()
