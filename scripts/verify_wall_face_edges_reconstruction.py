"""Verify genuine291B0x424140 face traversal against physical original/native3.

Requires CPython3.13, Capstone5, scoped x86 MSVC5SP3/modern MSVC/MinGW. The96
bounded fixtures execute actual424270, shape/quadratic/fallback/BRender and
contact insertion at masked PC53. Translation, flags, inclusive/exclusive face
indices, valid flag/end-index aliases and paired IEEE values compare all64KiB
with no scalar normalization. Direct ABI checks ECX/EDX+five stackargs/RET20,
native seven-slot cdecl, defined EAX, ESP/nonvolatile registers and x87TOP. Four
uniform incoming16KiB stack histories only; no arbitrary private-history,
malformed-pointer, sticky-fenv/unmasked-trap or whole-game claim. Optional
immutable linked PE requires its hash-bound symbol map and actual MAP.
Default extracts actual production; --candidate-physics permits a frozen body.
Full4228b0 consumer remains unfinished beyond this complete prerequisite.
"""
import hashlib,inspect,json,struct,sys
from pathlib import Path
ROOT=next(p for p in Path(__file__).resolve().parents if(p/'scripts').is_dir())
sys.path.insert(0,str(ROOT/'scripts'))
import verify_wall_edge_consumer_reconstruction as e
wall=e
from verify_tinted_native_reconstruction import extract
globals().update({n:v for n,v in e.__dict__.items()if not n.startswith('__')})
ENTRIES=[('FUN_00424140',0x424140,291)]
PREFIX=e.PREFIX+'#define gFace_list__car (*(tFace_ref(*)[300])(arena+0x8000))\n'
DRIVER=e.DRIVER.replace('values[5]=h[5]','values[3]=h[3];values[4]=h[4];values[6]=h[6]')
DRIVER=DRIVER.replace('builder_stack_count=6;for(i=2;i<8;i++)','builder_stack_count=5;for(i=2;i<7;i++)').replace('builder_stack_count=8;for(i=0;i<8;i++)','builder_stack_count=7;for(i=0;i<7;i++)')

def native_harness(candidate_physics,candidate_geometry=None):
    assert candidate_geometry is None
    original=e.native_harness(None);original=original[:original.index('static void* entries[1];')].replace(e.PREFIX,PREFIX,1)
    b=PHYSICS.read_text();chosen=candidate_physics.read_text()if candidate_physics else b
    original+=extract(b,'PhysicsWallAddScalar')+'\n'+extract(chosen,'FUN_00424140')+'\nstatic void* entries[1];\n'
    return original+DRIVER.replace('ARGS','2').replace('INSTALL','callback_slot=&gPhysics_error_cb;message_slot=&gPhysics_empty_message;entries[0]=(void*)FUN_00424140;')

def physical_harness(binary,addresses):
    source=inspect.getsource(e.physical_harness)
    source=source.replace("                if op.type==CS_OP_MEM and op.mem.disp==addresses[0x679438]:refs[a-base+i.disp_offset]=4", """                if op.type==CS_OP_MEM and op.mem.disp==addresses[0x679438]:refs[a-base+i.disp_offset]=4
                face=addresses[0x744820]
                if op.type==CS_OP_IMM and face<=op.imm<face+300*80:refs[a-base+i.imm_offset]=5
                if op.type==CS_OP_MEM and face<=op.mem.disp<face+300*80:refs[a-base+i.disp_offset]=5""")
    source=source.replace("source=source.replace(\"refs[i][1]?(unsigned)&gPling_materials:\",", "source=source.replace(\"refs[i][1]?(unsigned)&gPling_materials:\",")
    source=source.replace('refs[i][1]==3?(unsigned)&gPhysics_contact_679430:', 'refs[i][1]==5?(unsigned)(arena+0x8000)+value-FACE_ADDRESS:refs[i][1]==3?(unsigned)&gPhysics_contact_679430:')
    source=source.replace("    nt=struct.unpack_from", "    s=s.replace('FACE_ADDRESS',str(addresses[0x744820]))\n    nt=struct.unpack_from",1)
    ctx=dict(e.__dict__);ctx.update(globals());exec(source,ctx);return ctx['physical_harness'](binary,addresses)

def scenarios():
    cases=[dict(flags=flags,faces=n,label='traversal-flags')for flags in [0,1,2,3,4,5,6,7,0x80,0x81]for n in [0,1,2]]
    for k in range(5):
        for disabled in [0,1]:cases.append(dict(kind=k,disabled=disabled,faces=2,edges=2,label='shape-dispatch'))
    for face_start,face_end in [(1,1),(1,2),(2,1),(298,300)]:cases.append(dict(face_start=face_start,face_end=face_end,faces=2,label='captured-start-inclusive-exclusive-end'))
    for translation in [(-2,0,0),(2,0,0),(.25,.5,-.75)]:cases.append(dict(actor=(1,0,0,0,1,0,0,0,1,*translation),faces=2,label='bound-translation'))
    for seed in [0,0x3f800000,0x7fc12345,0xa5a5a5a5]:cases.append(dict(seed=seed,faces=2,label='uniform-private-history'))
    for flags in [2,4,6]:cases.append(dict(face_start=1,face_end=2,contacts=0x807c,flags=flags,edges=2,label='insertion-reloads-face-flags'))
    for end in [1,2,3]:cases.append(dict(faces=end,contacts=0x1160,edges=2,label='insertion-reloads-face-end'))
    for bits in [0x7fc12345,0x7f812345,0x7f800000,0xff800000,0x80000000]:
        for at in [0x1850,0x1854,0x1858,0x141c,0x1420,0x1424]:cases.append(dict(faces=0,raw=[(at,bits)],label='translated-bounds-IEEE'))
    for x,y in [(0x7fc10001,0x7fc20002),(0x7f810001,0xffc20002),(0xffc20002,0x7f810001)]:
        for at1,at2 in [(0x1850,0x141c),(0x1854,0x1420),(0x1858,0x1424)]:cases.append(dict(faces=0,raw=[(at1,x),(at2,y)],label='translated-bounds-paired-NaNs'))
    return cases

def make_fixture(o):
    a=bytearray(e.make_fixture(o)[48:]);start=o.get('face_start',0);end=o.get('face_end',start+o.get('faces',1));struct.pack_into('<HH',a,0x1174,start,end)
    for i in range(300):
        at=0x8000+i*80;a[at:at+80]=bytes(80)
        for j,p in enumerate(o.get('triangle',[(-2,0,0),(2,0,0),(0,0,2)])):struct.pack_into('<3f',a,at+4+j*12,*p)
        struct.pack_into('<3f',a,at+0x34,*o.get('normal',[0,1,0]));struct.pack_into('<I',a,at+0x40,o.get('flags',0))
    for at,bits in o.get('raw',[]):struct.pack_into('<I',a,at,bits)
    return struct.pack('<12I',o.get('object',0x1000),o.get('shape',0x1400),o.get('contacts',0x4000),o.get('count',0),o.get('capacity',4),o.get('workspace',0x3000),o.get('unused',0),0,o.get('mask',0x12345678),o.get('flag',0),o.get('seed',0xcccccccc),o.get('callback',0))+a
FIXTURE_SHA256='4ea196f0ea5edd60b927bdb9034a619e411824505ad27c9fd411fc6b69dd75bd'

def assert_oracle_contract(options,data):
    assert len(data)==65548 and struct.unpack_from('<I',data)[0]==0,('Unexpected fault/exit',options)
    if options.get('faces')==0 or options.get('flags',0)&0x80:assert struct.unpack_from('<I',data,12+0x7ff4)[0]==0

def main():
    source=inspect.getsource(e.main).replace("'len(fixtures)=='+str(len(scenarios()))", "'len(fixtures)=='+str(len(scenarios()))")
    source=source.replace('Path(q.__file__)]','Path(q.__file__),Path(wall.__file__)]')
    ctx=dict(e.__dict__);ctx.update(globals());exec(source,ctx);ctx['main']()
if __name__=='__main__':main()
