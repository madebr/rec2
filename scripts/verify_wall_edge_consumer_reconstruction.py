"""Verify genuine1819B wall-edge consumer0x424270 against physical original x86.

Requires scoped x86 MSVC5SP3/modern MSVC/MinGW, CPython3.13 and Capstone5.
The292 bounded fixtures execute actual shape, quadratic/fallback, BRender and
contact-insertion bodies at PC53 with masked exceptions. They cover five shapes,
endpoint/edge/gate routes, 0/1/63/64 planes, late success at edge indices2/30/31/
32/62/63, transforms/exponent spread, valid aliases, IEEE inputs and four uniform
incoming private-stack histories. No public scalar/pointer normalization occurs.
The raw ECX/EDX plus six-stack-argument/RET24 ABI (native eight-slot cdecl), defined
EAX, ESP, nonvolatile registers and x87TOP compare at ordinary returns, along with
all65536 arena bytes. Actual PhysicsError and its controlled callback execute;
only terminal exit is an explicit process boundary, with snapshots at callback
and exit. Those terminal cases do not claim post-return ABI validation.

--candidate-physics supports a frozen development snapshot; default extracts the
actual production entry. Optional --recompiled/--symbols require an immutable
hash-bound linked graph. No claim for arbitrary private-stack history, >64-plane
storage, malformed pointers, sticky exception flags, unmasked traps or game
startup. The >64 result guard is retained statically; bounded shape results cannot
enter it. Lexical function identity remains unproved, so its numerical name stays.
Full0x4228b0 reconstruction remains active beyond this complete prerequisite.
"""
import ast,hashlib,inspect,json,os,random,re,struct,subprocess,sys
from pathlib import Path
ROOT=next(p for p in Path(__file__).resolve().parents if (p/'scripts').is_dir())
sys.path.insert(0,str(ROOT/'scripts'))
import verify_wall_quadratic_reconstruction as q
import verify_shape_poly_reconstruction as shape
from verify_tinted_native_reconstruction import extract
HASH=q.HASH;PHYSICS=q.PHYSICS;GEOMETRY=q.GEOMETRY;MATRIX=q.MATRIX
VECTOR=ROOT/'src/brender/core/math/vector.c'
ENTRIES=[('FUN_00424270',0x424270,1819)]
PREFIX=shape.PREFIX.split('/* Uniform stack history')[0].replace('static void* entries[4];','').replace('0x4000','65536')+q.DECLS+'#define gPhysics_contact_679430 (*(tU32*)(arena+0x7ff4))\n#define gPhysics_contact_679438 (*(tU32*)(arena+0x7ff0))\n'
DRIVER=r'''
int C2_HOOK_CDECL main(int argc,char**argv){FILE*f;unsigned h[12],values[8],i;
 if(argc!=ARGS)return 2;arena=VirtualAlloc((void*)0x20000000,65536,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(!arena)return 3;
 f=fopen(argv[1],"rb");if(!f)return 4;if(fread(h,sizeof(h),1,f)!=1||fread(arena,65536,1,f)!=1)return 5;fclose(f);INSTALL
 callback_mode=h[11];*callback_slot=(callback_mode&8)?NULL:ErrorCallback;*message_slot=(char)arena[0x7fe8];
 InstallFaultBoundary();_controlfp(_PC_53,_MCW_PC);for(i=0;i<8;i++)values[i]=(unsigned)(arena+h[i]);values[5]=h[5];gPhysics_contact_679430=h[8];gPhysics_contact_679438=h[9];
 builder_address=(unsigned)entries[0];builder_mode=0;builder_x=values[0];builder_y=values[1];builder_poison=h[10];
 #ifdef REC2_MATCHING
 builder_cdecl=0;builder_stack_count=6;for(i=2;i<8;i++)builder_stack[i-2]=values[i];
 #else
 builder_cdecl=1;builder_stack_count=8;for(i=0;i<8;i++)builder_stack[i]=values[i];
 #endif
 _setmode(_fileno(stdout),_O_BINARY);if(BuilderInvoke())return 14;snapshot(0,builder_result);return 0;
}
'''

def native_harness(candidate_physics,candidate_geometry=None):
    assert candidate_geometry is None
    b=PHYSICS.read_text();g=GEOMETRY.read_text();m=MATRIX.read_text();chosen=candidate_physics.read_text()if candidate_physics else b;s=PREFIX
    for n in ['PhysicsCopyWord','PhysicsSubtractScalar','PhysicsScalar','PhysicsScalarStore','PhysicsWallNaN','AddObjectWallForce']:s+=extract(b,n)+'\n'
    for n in ['Matrix34ApplyProductLoaded','Matrix34ApplyProductMemory','Matrix34ApplyAdd','BrMatrix34ApplyP','BrMatrix34ApplyV','BrMatrix34TApplyV']:s+=extract(m,n)+'\n'
    for n in ['FinterayScalar','FinterayBoxNaN','FinterayBoxCopyScalar','FinterayBoxStoreScalar','FinterayShapeLoadScalar','FinterayShapePlanes','FinterayShapePlaneCount','LineBoxColl','SphereBoxIntersection','LineBoxCollWithSphere','LineSphereColl','LineSphereCollWithSphere','LinePolyColl','LinePolyCollWithSphere','ShapeRayCast','ShapeRayCastWithSphere']:s+=extract(g,n)+'\n'
    s+=extract(VECTOR.read_text(),'BrVector3SetFloat')+'\n'+extract(b,'PhysicsError')+'\n'
    for n in ['FUN_00425630','FUN_00424990']:s+=extract(b,n)+'\n'
    s+=extract(chosen,'FUN_00424270')+'\nstatic void* entries[1];\n'
    return s+DRIVER.replace('ARGS','2').replace('INSTALL','callback_slot=&gPhysics_error_cb;message_slot=&gPhysics_empty_message;entries[0]=(void*)FUN_00424270;')

def physical_harness(binary,addresses):
    source=inspect.getsource(shape.physical_harness)
    inject="""            for op in i.operands:
                if op.type==CS_OP_MEM and op.mem.disp==addresses[0x679430]:refs[a-base+i.disp_offset]=3
                if op.type==CS_OP_MEM and op.mem.disp==addresses[0x679438]:refs[a-base+i.disp_offset]=4
"""
    source=source.replace("            if i.mnemonic=='call':",inject+"            if i.mnemonic=='call':")
    source=source.replace('offsets[4]','offsets[1]').replace('i<4;i++','i<1;i++')
    source=source.replace("refs[i][1]?(unsigned)&gPling_materials:","refs[i][1]==3?(unsigned)&gPhysics_contact_679430:refs[i][1]==4?(unsigned)&gPhysics_contact_679438:refs[i][1]?(unsigned)&gPling_materials:")
    source=source.replace("    return s,dict(entries=","    return s,dict(instructions=sorted(instructions.items()),entries=")
    ctx=dict(PREFIX=PREFIX+'static void* entries[1];\n',DRIVER=DRIVER,ENTRIES=ENTRIES,struct=struct,json=json,hashlib=hashlib)
    exec(source,ctx);s,meta=ctx['physical_harness'](binary,addresses)
    nt=struct.unpack_from('<I',binary,60)[0];base=struct.unpack_from('<I',binary,nt+52)[0];tab=nt+24+struct.unpack_from('<H',binary,nt+20)[0]
    sections=[struct.unpack_from('<IIII',binary,tab+40*i+8)for i in range(struct.unpack_from('<H',binary,nt+6)[0])]
    def raw(rva,n):
        for _,va,size,off in sections:
            if va<=rva<va+size:return binary[off+rva-va:off+rva-va+n]
        raise AssertionError(('Unmapped relocation',hex(rva)))
    rel,size=struct.unpack_from('<II',binary,nt+24+96+5*8);cursor=rel;covered={site for site,_ in meta['references']};active={a-base+k for a,b in meta['instructions']for k in range(len(b)//2)};sites=[]
    while cursor<rel+size:
        page,n=struct.unpack('<II',raw(cursor,8));assert n>=8
        for word in struct.unpack('<'+'H'*((n-8)//2),raw(cursor+8,n-8)):
            if word>>12==3:
                at=page+(word&4095)
                if at in active:assert at in covered,hex(at);sites.append(at)
        cursor+=n
    meta['relocations_checked']=sites
    meta['private_history_bytes']=16384
    meta['private_history_scope']='Incoming16KiB seeded with one32-bit word by direct ABI bridge; actual nested calls retain their real stack writes.'
    return s,meta

def scenarios():
    cases=[dict(kind=k,disabled=d,edges=e,label='shape-dispatch')for k in [0,1,2,3,4]for d in [0,1]for e in [0,1,2]]
    for k in range(5):
        for translation in [-2.,2.,.25]:
            for disabled in [0,1]:cases.append(dict(kind=k,disabled=disabled,edges=2,start=(.5,0,0),end=(.5,0,1),matrix=(1,0,0,0,1,0,0,0,1,translation,0,0),label='endpoint-motion'))
    for normal in [(0,-1,0),(1,0,0),(0,0,-1),(0,0,0)]:
        for k in [0,1,4]:cases.append(dict(kind=k,edges=2,normal=normal,label='normal-side-gate'))
    for axis in range(3):
        for side in [-1,1]:
            start=[0.,0.,0.];end=[0.,0.,0.];start[axis]=side*2;end[axis]=side*3
            cases.append(dict(start=start,end=end,edges=2,label='bounds-outside'))
    for count in [1,3,10]:cases.append(dict(count=count,edges=2,mask=0xa1000000,label='existing-contact-list'))
    for edges in [3,31,32,33,63,64]:cases.append(dict(edges=edges,label='repeated-edge-mask'))
    for seed in [0,0x3f800000,0x7fc12345,0xa5a5a5a5]:cases.append(dict(edges=2,seed=seed,label='uniform-private-history'))
    for contacts in [0x1078,0x2000,0x3000,0x3300,0x100]:cases.append(dict(edges=2,contacts=contacts,label='contact-output-source-alias'))
    rng=random.Random(424270)
    for i in range(48):
        origin=tuple(rng.uniform(-2,2)for _ in range(3));direction=tuple(rng.uniform(-3,3)for _ in range(3));edge=tuple(rng.uniform(-2,2)for _ in range(3));velocity=tuple(rng.uniform(-2,2)for _ in range(3));change=tuple(rng.uniform(-1,1)for _ in range(3))if i>=24 else(0,0,0)
        moving=tuple(origin[j]+.6*direction[j]-.3*(edge[j]+.5*change[j])-.5*velocity[j]for j in range(3))
        old2=tuple(moving[j]+edge[j]for j in range(3));trial1=tuple(moving[j]+velocity[j]for j in range(3));trial2=tuple(old2[j]+velocity[j]+change[j]for j in range(3))
        cases.append(dict(start=origin,end=tuple(origin[j]+direction[j]for j in range(3)),points=[(moving,trial1),(old2,trial2)],edges=1,bounds=(-100,-100,-100,100,100,100),normal=tuple(rng.uniform(-1,1)for _ in range(3)),kind=i%5,disabled=i%2,label='oblique-known-intersection'))
    matrices=[(1.25,.125,-.25,-.375,.75,.25,.125,-.5,1.5,.75,-1.25,2.),(-1,0,0,0,.5,0,0,0,2,-3,4,-5)]
    for i in range(12):cases.append(dict(cases[96+i],matrix=matrices[i%2],actor=matrices[(i+1)%2],label='distinct-current-previous-transform'))
    for at in [0x100,0x104,0x108,0x10c,0x110,0x114,0x3000,0x3004,0x3008,0x3300,0x3304,0x3308,0x2034,0x2038,0x203c]:
        for bits in [0x7fc12345,0x7f812345,0x7f800000,0xff800000]:cases.append(dict(kind=1,edges=1,disabled=1,raw=[(at,bits)],label='single-source-IEEE'))
    for pair in [(0x7fc10001,0x7fc20002),(0x7f810001,0xffc20002),(0xffc20002,0x7f810001)]:
        for at1,at2 in [(0x3004,0x3304),(0x3008,0x3308),(0x100,0x10c),(0x182c,0x1834),(0x1090,0x1094)]:cases.append(dict(kind=1,edges=1,disabled=1,raw=[(at1,pair[0]),(at2,pair[1])],label='paired-source-NaNs'))
    for callback in [0,1,2,4,7,8]:cases.append(dict(kind=5,edges=0,callback=callback,message=0x42,label='authentic-terminal-error'))
    for exponents in [(-12,-12,-12),(12,12,12),(20,0,-20),(-20,0,20)]:
        for i in range(6):
            o=dict(cases[96+i]);o['label']='finite-exponent-spread';o['bounds']=(-2.**24,)*3+(2.**24,)*3
            for name in ['start','end']:o[name]=tuple(x*2.**exponents[j]for j,x in enumerate(o[name]))
            o['points']=[tuple(tuple(x*2.**exponents[j]for j,x in enumerate(p))for p in pair)for pair in o['points']];cases.append(o)
    for threshold in [-1.005,1.005]:
        bits=struct.unpack('<I',struct.pack('<f',threshold))[0]
        for delta in [-1,0,1]:
            value=struct.unpack('<f',struct.pack('<I',bits+delta))[0]
            cases.append(dict(start=(value,0,0),end=(value,0,1),edges=2,label='bounds-margin-adjacent-float'))
    for start_offset,end_offset in [(0x100,0x104),(0x104,0x100),(0x3000,0x300c),(0x4000,0x4004)]:cases.append(dict(start_offset=start_offset,end_offset=end_offset,edges=2,label='valid-shifted-endpoint-alias'))
    for seed in [0,0x3f800000,0x7fc12345,0xa5a5a5a5]:cases.append(dict(kind=1,edges=2,disabled=1,raw=[(0x3004,0x7fc10001),(0x3304,0x7fc20002)],seed=seed,label='paired-NaNs-private-history'))
    cases.append(dict(start=(.5,0,0),end=(.5,0,1),matrix=(1,0,0,0,1,0,0,0,1,2,0,0),contacts=0x36a4,edges=2,label='endpoint-insertion-workspace-count-alias'))
    old=[((0,1,-1),(0,-1,-1)),((0,1,1),(0,-1,1)),((1,2,-1),(1,-2,-1)),((1,2,1),(1,-2,1)),((0,3,-1),(0,3,-1)),((0,3,1),(0,3,1))]
    for edges in [3,31,32,33,63,64]:cases.append(dict(edges=edges,points=old,pairs=[(4,5)]*(edges-1)+[(0,1)],mask=0x100,label='late-success-edge-mask'))
    for count in [0,1,63,64]:
        for k in [1,3]:cases.append(dict(kind=k,edges=0,start=(.5,0,0),end=(.5,0,1),matrix=(1,0,0,0,1,0,0,0,1,2,0,0),planes=[(0,0,0,1)]*max(0,count-1)+([(1,0,0,1)]if count else[]),label='plane-capacity-and-mask'))
    for edges in [1,2]:cases.append(dict(edges=edges,flag=1,label='quadratic-global-flag'))
    return cases

def make_fixture(o):
    a=bytearray(b'\xa5'*65536)
    def f(at,*xs):struct.pack_into('<'+'f'*len(xs),a,at,*xs)
    def u(at,*xs):struct.pack_into('<'+'I'*len(xs),a,at,*xs)
    a[0x1000:0x1260]=bytes(0x260);u(0x1000,0x20001800);f(0x1004,1);f(0x108c,*o.get('matrix',[1,0,0,0,1,0,0,0,1,0,0,0]));a[0x10ed]=o.get('disabled',0)
    a[0x1800:0x1880]=bytes(0x80);f(0x182c,*o.get('actor',[1,0,0,0,1,0,0,0,1,0,0,0]))
    a[0x1400:0x1458]=bytes(0x58);u(0x1400,o.get('kind',0));f(0x1404,-1,-1,-1,1,1,1);f(0x141c,*o.get('bounds',[-1,-1,-1,1,1,1]))
    if o.get('kind',0)==4:f(0x1438,1,0,0,0,1)
    else:
        planes=o.get('planes',[(1,0,0,1),(-1,0,0,1),(0,1,0,1),(0,-1,0,1),(0,0,1,1),(0,0,-1,1)]);u(0x1440,len(planes));u(0x144c,0x20005500)
        for i,v in enumerate(planes):f(0x5500+i*16,*v)
        u(0x54f0,0x3f012345,0x3f456789,0xbf234567,0x3f800000)
    f(0x100,*o.get('start',[-2,0,0]));f(0x10c,*o.get('end',[2,0,0]))
    a[0x2000:0x2050]=bytes(0x50);f(0x2034,*o.get('normal',[0,1,0]));f(0x2040,0)
    a[0x3000:0x3708]=bytes(0x708);struct.pack_into('<HHI',a,0x3700,4,o.get('edges',1),0x20003800)
    for i,(old,new)in enumerate(o.get('points',[((0,1,-1),(0,-1,-1)),((0,1,1),(0,-1,1)),((1,2,-1),(1,-2,-1)),((1,2,1),(1,-2,1))])):f(0x3000+i*12,*old);f(0x3300+i*12,*new)
    pairs=o.get('pairs',[(2*(i%2),2*(i%2)+1)for i in range(o.get('edges',1))]);a[0x3800:0x3800+2*len(pairs)]=bytes(x for pair in pairs for x in pair)
    for slot in range(6):f(0x4000+slot*96,2);f(0x4014+slot*96,10+slot,8,6);f(0x402c+slot*96,0,1,0)
    a[0x7fe8]=o.get('message',0)
    for at,bits in o.get('raw',[]):u(at,bits)
    h=[o.get('start_offset',0x100),o.get('end_offset',0x10c),o.get('object',0x1000),o.get('shape',0x1400),o.get('contacts',0x4000),o.get('count',0),o.get('face',0x2000),o.get('workspace',0x3000),o.get('mask',0),o.get('flag',0),o.get('seed',0xcccccccc),o.get('callback',0)]
    return struct.pack('<12I',*h)+a

FIXTURE_SHA256='809f43323eb6bedb445b2471d4e13919bb19cef76e804dd2552b42410b31e9ed'
def assert_oracle_contract(options,data):
    assert len(data)%65548==0,'Partial oracle snapshot'
    frames=[struct.unpack_from('<3I',data,at)for at in range(0,len(data),65548)]
    if options.get('label')=='authentic-terminal-error':
        assert [x[0]for x in frames]==([2]if options.get('callback',0)&8 else[1,2]),frames
    else:assert len(frames)==1 and frames[0][0]==0,('Unexpected fault/exit',options,frames)
    if options.get('label')=='late-success-edge-mask':assert struct.unpack_from('<I',data,12+0x7ff4)[0]==(options['mask']|(1<<((options['edges']-1)&31)))
def main():
    # Reuse the frozen transport/compiler/direct-ABI engine without editing it.
    source=inspect.getsource(q.main).replace("0x679438:0x679438}","0x679438:0x679438,0x679430:0x679430,0x5784e0:0x5784e0,0x6940c0:0x6940c0,0x676920:0x676920,0x4c61c0:0x4c61c0}")
    source=source.replace('len(fixtures)==318','len(fixtures)=='+str(len(scenarios()))).replace('len(original.stdout)==65540','len(original.stdout)>=65548')
    source=source.replace("ROOT/'src/carma2/hooks/c2_hooks.h']","ROOT/'src/carma2/hooks/c2_hooks.h',VECTOR,Path(shape.__file__),Path(q.__file__)]")
    # reccmp's symbol export omits this genuine one-byte BSS global. Bind its
    # unique actual linker symbol, preserving the original callback identity.
    source=source.replace("addresses={v['orig']:v['recomp']for v in mapping['symbols']};code,linked=physical_harness(rebuilt,addresses)","""addresses={v['orig']:v['recomp']for v in mapping['symbols']}
        actual_map=args.recompiled.with_suffix('.map').read_text();fallback=[]
        if 0x676920 not in addresses:
            found=re.findall(r'^\\s*[0-9a-fA-F]{4}:[0-9a-fA-F]{8}\\s+_gPhysics_empty_message\\s+([0-9a-fA-F]{8})\\s+',actual_map,re.M);assert len(found)==1,'Missing/ambiguous actual one-byte BSS linker symbol'
            addresses[0x676920]=int(found[0],16);fallback.append(dict(original='0x676920',symbol='_gPhysics_empty_message',recompiled=hex(addresses[0x676920]),source='actual immutable MAP'))
        code,linked=physical_harness(rebuilt,addresses);linked['actual_map_global_bindings']=fallback""")
    source=source.replace("return_scope='Defined int EAX/raw ABI/arena compared'","return_scope='Ordinary returns: defined int EAX/ESP/nonvolatile registers/x87TOP. Terminal error cases: actual callback/exit snapshots, no post-return ABI claim'")
    source=source.replace("        for tag in ['msvc5'","        assert_oracle_contract(options_,original.stdout)\n        for tag in ['msvc5'")
    ctx=dict(q.__dict__);ctx.update(globals());exec(source,ctx);ctx['main']()
if __name__=='__main__':main()
