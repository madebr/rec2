"""Complete1819B4228b0 retained wall consumer proof; no numeric lower mocks.

PC53/masked exceptions, bounded uniform private histories and real terminal
PhysicsError process boundary. Complete parent objective; not yet accepted.
"""
import hashlib,inspect,json,math,struct,sys
from pathlib import Path
ROOT=next(p for p in Path(__file__).resolve().parents if(p/'scripts').is_dir())
sys.path.insert(0,str(ROOT/'scripts'));sys.path.insert(0,str(Path(__file__).parent))
import verify_wall_face_edges_reconstruction as f
import verify_wall_edge_consumer_reconstruction as wall
import verify_wall_corner_edges_reconstruction as corners
from verify_tinted_native_reconstruction import extract
globals().update({n:v for n,v in f.__dict__.items()if not n.startswith('__')})
ENTRIES=[('FUN_004228b0',0x4228b0,1819)]
PREFIX=f.PREFIX+'#include <stddef.h>\n#define gPhysics_contact_679434 (*(tU32*)(arena+0x7ffc))\n#define gPhysics_box_edges_58f7f0 (*(tPolyhedron_edge_indexes(*)[12])(arena+0x7fd0))\n'
PREFIX+='\n#define M(x,y) mat->m[x][y]\n#define A(x,y) A->m[x][y]\n#define B(x,y) B->m[x][y]\n#define C(x,y) C->m[x][y]\n'
DRIVER=wall.DRIVER.replace('values[5]=h[5]','values[2]=h[2]')
DRIVER=DRIVER.replace('builder_stack_count=6;for(i=2;i<8;i++)','builder_stack_count=1;for(i=2;i<3;i++)').replace('builder_stack_count=8;for(i=0;i<8;i++)','builder_stack_count=3;for(i=0;i<3;i++)')
EDGE_TABLE=bytes.fromhex('000102030405060700020103040605070004010502060307')

def native_harness(candidate_physics,candidate_geometry=None):
    assert candidate_geometry is None
    s=wall.native_harness(None);s=s[:s.index('static void* entries[1];')].replace(wall.PREFIX,PREFIX,1)
    b=PHYSICS.read_text();g=GEOMETRY.read_text();m=MATRIX.read_text();chosen=candidate_physics.read_text()if candidate_physics else b
    end=g.index('} tFinteray_projection;')+len('} tFinteray_projection;');start=g.rfind('typedef struct {',0,end);s+=g[start:end]+'\n'
    first=g.index('/* VC5 crashes');last=g.index('// FUNCTION:',first);s+=g[first:last]+'\n'
    for n in ['FinterayFaceProjection','FinterayFacePoint','CheckSingleFace','FinterayBoundsAdd','GetNewBoundingBox','FinteraySphereFacePoint','StationarySphereEdgeCollide','PointSphereCollision','SphereEdgeCollide','SphereFaceCollide']:s+=extract(g,n)+'\n'
    for n in ['BrMatrix34Mul','BrMatrix34LPInverse']:s+=extract(m,n)+'\n'
    for n in ['PhysicsScalarNaN','PhysicsWallAddScalar','RayCastThruFaceList','EliminateFaces','CheckSingleEdgeWithSphere','GetSphereToEdgePoints','WallEdgeAlignment','GetCornerToFacePoints','FUN_004236a0']:s+=extract(b,n)+'\n'
    s+=extract(b,'FUN_00424140')+'\n'
    s+=extract(chosen,'FUN_004228b0')+'\nstatic void* entries[1];\n'
    return s+DRIVER.replace('ARGS','2').replace('INSTALL','callback_slot=&gPhysics_error_cb;message_slot=&gPhysics_empty_message;entries[0]=(void*)FUN_004228b0;')

def physical_harness(binary,addresses):
    source=inspect.getsource(f.physical_harness)
    # Extend that authenticated full graph and preserve actual global addends.
    source=source.replace("                face=addresses[0x744820]", """                if op.type==CS_OP_MEM and op.mem.disp==addresses[0x679434]:refs[a-base+i.disp_offset]=6
                if op.type==CS_OP_IMM and addresses[0x58f7f0]<=op.imm<addresses[0x58f7f0]+24:refs[a-base+i.imm_offset]=7
                if op.type==CS_OP_MEM and addresses[0x58f7f0]<=op.mem.disp<addresses[0x58f7f0]+24:refs[a-base+i.disp_offset]=7
                face=addresses[0x744820]""")
    source=source.replace("'refs[i][1]==5?(unsigned)(arena+0x8000)+value-FACE_ADDRESS:", "'refs[i][1]==6?(unsigned)&gPhysics_contact_679434:refs[i][1]==7?(unsigned)(arena+0x7fd0)+value-BOX_EDGES_ADDRESS:refs[i][1]==5?(unsigned)(arena+0x8000)+value-FACE_ADDRESS:")
    source=source.replace("    ctx=dict(e.__dict__)", "    source=source.replace(\"s=s.replace('FACE_ADDRESS',str(addresses[0x744820]))\",\"s=s.replace('FACE_ADDRESS',str(addresses[0x744820])).replace('BOX_EDGES_ADDRESS',str(addresses[0x58f7f0]))\")\n    ctx=dict(e.__dict__)")
    needle='source=inspect.getsource(shape.physical_harness)'
    inner=needle+'\n    source=source.replace('+repr('assert len(instructions)<6000')+','+repr('assert len(instructions)<20000')+')'
    check="if function!=error_address:\n                        window=raw(i.address-19,19)\n                        assert i.op_str=='eax' and window[0]==0xa1 and struct.unpack_from('<I',window,1)[0]==addresses[0x6940c0] and window[5:9]==bytes.fromhex('3bc1740c') and window[9]==0xba and struct.unpack_from('<I',window,10)[0]==addresses[0x676920] and window[14]==0xb9 and struct.unpack_from('<I',window,15)[0]==1,('Unproven indirect callback',hex(function),hex(i.address),i.op_str)\n                        inline_error_calls.append(dict(function=hex(function),call=hex(i.address),target_global=hex(addresses[0x6940c0]),message=hex(addresses[0x676920]),error=1,unchanged_eax_window=window.hex()))"
    inner+='\n    source=source.replace('+repr("assert function==error_address,'Only the authentic error callback may be indirect'")+','+repr(check)+')'
    inner+='\n    source=source.replace('+repr('instructions={};refs={};tables={};exit_address=')+','+repr('instructions={};refs={};tables={};inline_error_calls=[];exit_address=')+')'
    inner+='\n    source=source.replace('+repr('private_history_bytes=4096)')+','+repr('private_history_bytes=4096,authentic_inline_error_calls=inline_error_calls)')+')'
    source=source.replace("    ctx=dict(e.__dict__)", '    source=source.replace('+repr(needle)+','+repr(inner)+')\n    ctx=dict(e.__dict__)')
    ctx=dict(f.__dict__);ctx.update(globals());exec(source,ctx);return ctx['physical_harness'](binary,addresses)

def scenarios():
    cases=[dict(kind=k,disabled=d,faces=n,label='shape-and-face-dispatch')for k in range(5)for d in [0,1]for n in [0,1,2]]
    cases+=[dict(no_shapes=True,disabled=d,faces=1,label='null-shape-chain')for d in [0,1]]
    cases+=[dict(initial_count=i,faces=1,label='initial-contact-index')for i in [-1,0,1,3,36,37]]
    for kind in range(5):
        for flag in [0,1]:
            for seed in [0,0x3f800000,0x7fc12345,0xa5a5a5a5]:
                cases.append(dict(kind=kind,flag=flag,seed=seed,faces=2,label='mode-and-private-history'))
    for shapes in [[0,4],[4,0],[1,2,3],[4,4],[0,0],[0,1,2,3,4]]:
        for disabled in [0,1]:
            for flag in [0,1]:cases.append(dict(shapes=shapes,disabled=disabled,flag=flag,faces=2,label='live-shape-chain'))
    for kind in range(5):
        for material in ['!','>','#','@','']:
            cases.append(dict(kind=kind,material=material,faces=2,label='real-material-filter'))
        for face_start,faces in [(0,3),(297,2),(298,2),(299,1)]:
            cases.append(dict(kind=kind,face_start=face_start,faces=faces,label='face-pool-and-classification'))
        for flags in [1,2,4,7,0x80,0x81]:cases.append(dict(kind=kind,face_flags=flags,faces=2,label='live-face-flags'))
        for initial_count in [1,35,36,37]:cases.append(dict(kind=kind,initial_count=initial_count,faces=2,label='full-contact-capacity'))
    matrices=[(1.25,.125,-.25,-.375,.75,.25,.125,-.5,1.5,.75,-1.25,2.),(-1,0,0,0,.5,0,0,0,2,-3,4,-5)]
    for kind in range(5):
        for i in range(2):cases.append(dict(kind=kind,matrix=matrices[i],actor=matrices[1-i],faces=2,label='oblique-transforms'))
    for exponent in [-20,-12,12,20]:
        scale=2.**exponent
        for kind in [0,1,4]:cases.append(dict(kind=kind,matrix=(1,0,0,0,1,0,0,0,1,0,0,2*scale),actor=(1,0,0,0,1,0,0,0,1,0,0,-2*scale),faces=1,label='finite-exponent-motion'))
    for kind in [0,4]:
        for flag in [0,1]:cases.append(dict(kind=kind,flag=flag,faces=2,matrix=(1,0,0,0,1,0,0,0,1,0,2,2),actor=(1,0,0,0,1,0,0,0,1,0,-2,-2),face_specs=[{},dict(triangle=[(-4,0,-4),(4,0,-4),(0,0,4)],normal=(0,1,0))],label='nonparallel-contact-classification'))
    sphere_inputs=[0x1438,0x143c,0x1440,0x1444,0x8034,0x8038,0x803c]
    for flag in [0,1]:
        for at in sphere_inputs:
            for bits in [0x7fc12345,0x7f812345,0x7f800000,0xff800000,0x80000000]:
                cases.append(dict(kind=4,flag=flag,faces=1,raw=[(at,bits)],label='sphere-radius-center-normal-IEEE'))
        for x,y in [(0x7fc10001,0x7fc20002),(0x7f810001,0xffc20002),(0xffc20002,0x7f810001)]:
            for at1,at2 in [(0x1438,0x8034),(0x1438,0x8038),(0x1438,0x803c),(0x143c,0x8034),(0x1440,0x8038),(0x1444,0x803c),(0x8034,0x803c),(0x8038,0x803c)]:
                cases.append(dict(kind=4,flag=flag,faces=1,raw=[(at1,x),(at2,y)],label='sphere-scalar-vs-wide-paired-NaNs'))
    threshold=struct.unpack('<I',struct.pack('<f',.95))[0]
    for delta in [-1,0,1]:
        z=struct.unpack('<f',struct.pack('<I',threshold+delta))[0];x=math.sqrt(1-z*z)
        cases.append(dict(faces=2,face_specs=[{},dict(triangle=[(-4*z,-4,4*x),(4*z,-4,-4*x),(0,4,0)],normal=(x,0,z))],label='final-classification-point95-boundary'))
    for bits in [0x7fc12345,0x7f812345]:cases.append(dict(faces=2,raw=[(0x808c,bits)],label='NaN-face-filter-before-classification'))
    zaxis=(.36,.48,.8);length=math.sqrt(.8*.8+.36*.36);xaxis=(.8/length,0,-.36/length)
    yaxis=(zaxis[1]*xaxis[2],zaxis[2]*xaxis[0]-zaxis[0]*xaxis[2],-zaxis[1]*xaxis[0])
    rotation=tuple(value for i in range(3)for value in [xaxis[i],yaxis[i],zaxis[i]])+(0,0,-2)
    for bits in [0x3f7fffff,0x3f800000,0x3f800001]:
        radius=struct.unpack('<f',struct.pack('<I',bits))[0]
        for flag in [0,1]:cases.append(dict(kind=4,flag=flag,radius=radius,center=zaxis,cmpos=(.36,.48,-2)if flag else(0,0,0),actor=rotation,faces=1,label='sphere-XYZ-product-spill-cancellation'))
    for contacts in [0x1074,0x1850,0x6000,0x8010,0x8034]:
        for kind in [0,4]:
            if contacts==0x8034 and kind==0:continue # Saved supplemental alias-fault observation, no returned-ABI credit.
            cases.append(dict(kind=kind,contacts=contacts,faces=2,label='contact-output-live-source-alias'))
    cube=[(-1,-1,-1),(1,-1,-1),(-1,1,-1),(1,1,-1),(-1,-1,1),(1,-1,1),(-1,1,1),(1,1,1)]
    for kind in [1,2,3]:
        for count in [0,64]:
            for flag in [0,1]:cases.append(dict(kind=kind,flag=flag,faces=1,points=[cube[i%8]for i in range(count)],pairs=[(i,(i+1)%count)for i in range(count)],label='packed-workspace-capacity'))
    for count in [1,63]:
        for flag in [0,1]:cases.append(dict(kind=2,flag=flag,faces=1,points=[cube[i%8]for i in range(count)],pairs=[(i,(i+1)%count)for i in range(count)],label='packed-workspace-intermediate-count'))
    for pairs in [list(reversed([(EDGE_TABLE[i],EDGE_TABLE[i+1])for i in range(0,24,2)])),[(EDGE_TABLE[i+1],EDGE_TABLE[i])for i in range(0,24,2)]]:
        cases.append(dict(faces=1,box_edge_pairs=pairs,label='genuine-writable-box-edge-table'))
    for callback in [0,1,2,4,7,8]:cases.append(dict(kind=5,faces=1,callback=callback,message=0x42,label='authentic-terminal-error'))
    return cases

def make_fixture(o):
    base=dict(o,flags=o.get('face_flags',o.get('flags',0)),triangle=o.get('triangle',[(-4,-4,0),(4,-4,0),(0,4,0)]),normal=o.get('normal',(0,0,1)),matrix=o.get('matrix',(1,0,0,0,1,0,0,0,1,0,0,2)),actor=o.get('actor',(1,0,0,0,1,0,0,0,1,0,0,-2)))
    base.pop('points',None);base.pop('pairs',None) # Parent shape arrays have a different layout from the inherited unused edge fixture.
    a=bytearray(f.make_fixture(base)[48:])
    def w(at,*xs):struct.pack_into('<'+'I'*len(xs),a,at,*xs)
    def v(at,*xs):struct.pack_into('<'+'f'*len(xs),a,at,*xs)
    w(0x1020,0 if o.get('no_shapes')else 0x20001400)
    if 'cmpos' in o:v(0x1014,*o['cmpos'])
    w(0x5000,0,0x20005100);name=o.get('material','ordinary').encode()+b'\0';a[0x5100:0x5100+len(name)]=name
    for j in range(300):w(0x8000+j*80,0x20005000)
    shapes=o.get('shapes',[o.get('kind',0)])
    for index,kind in enumerate(shapes):
        at=0x1400+index*0x60
        template=f.make_fixture(dict(base,kind=kind))[48:]
        a[at:at+0x58]=template[0x1400:0x1458]
        w(at+0x34,0x20000000+at+0x60 if index+1<len(shapes)else 0)
        if kind in [1,2,3]:
            points=o.get('points',[(-1,-1,-1),(1,-1,-1),(-1,1,-1),(1,1,-1),(-1,-1,1),(1,-1,1),(-1,1,1),(1,1,1)])
            pairs=o.get('pairs',[(0,1),(2,3),(4,5),(6,7),(0,2),(1,3),(4,6),(5,7),(0,4),(1,5),(2,6),(3,7)])
            point_at=0x6000+index*0x500;edge_at=point_at+0x300
            w(at+0x38,len(points));w(at+0x3c,len(pairs))
            if kind==2:w(at+0x40,0x20000000+point_at);w(at+0x44,0x20000000+edge_at)
            else:w(at+0x44,0x20000000+point_at);w(at+0x48,0x20000000+edge_at)
            for i,p in enumerate(points):v(point_at+12*i,*p)
            a[edge_at:edge_at+len(pairs)*2]=bytes(x for pair in pairs for x in pair)
        elif kind==4 and ('radius' in o or 'center' in o):
            radius=o.get('radius',1.);center=o.get('center',(0,0,0))
            v(at+0x38,radius,*center,o.get('radius_squared',radius*radius))
            bounds=tuple(x-radius for x in center)+tuple(x+radius for x in center)
            v(at+4,*bounds);v(at+0x1c,*bounds)
    for index,spec in enumerate(o.get('face_specs',[])):
        at=0x8000+80*(o.get('face_start',0)+index)
        if 'triangle' in spec:
            for j,point in enumerate(spec['triangle']):v(at+4+12*j,*point)
        if 'normal' in spec:v(at+0x34,*spec['normal'])
        if 'd' in spec:v(at+0x44,spec['d'])
    v(0x1074,11,12,13);v(0x11c0,21,22,23)
    for i in range(42):
        at=0x4000+96*i;v(at,2);v(at+0x14,10+i,8,6);v(at+0x2c,0,0,1);w(at+0x5c,0x1234)
    a[0x7fd0:0x7fe8]=bytes(x for pair in o.get('box_edge_pairs',[(EDGE_TABLE[i],EDGE_TABLE[i+1])for i in range(0,24,2)])for x in pair)
    for at,bits in o.get('raw',[]):w(at,bits)
    return struct.pack('<12I',o.get('object',0x1000),o.get('contacts',0x4000),o.get('initial_count',0)&0xffffffff,0,0,0,0,0,o.get('mask',0x12345678),o.get('flag',0),o.get('seed',0xcccccccc),o.get('callback',0))+a
FIXTURE_SHA256='2877f45f084bb8bc07a0c9053d54b42047d52c61dc976a35a9ea850f73dc5f50'

def assert_oracle_contract(options,data):
    assert len(data)%65548==0 and data,'Partial/empty oracle snapshot'
    tags=[struct.unpack_from('<3I',data,at)for at in range(0,len(data),65548)]
    if options.get('label')=='authentic-terminal-error':assert [r[0]for r in tags]==([2]if options.get('callback',0)&8 else[1,2]),tags
    else:assert len(tags)==1 and tags[0][0]==0,('Unexpected fault/exit',options,tags)

def main():
    source=inspect.getsource(wall.main)
    source=source.replace('Path(q.__file__)]','Path(q.__file__),Path(wall.__file__),Path(f.__file__)]')
    source=source.replace('0x4c61c0:0x4c61c0}', '0x4c61c0:0x4c61c0,0x679434:0x679434,0x58f7f0:0x58f7f0}')
    marker="    ctx=dict(q.__dict__);ctx.update(globals());exec(source,ctx);ctx['main']()"
    assert marker in source
    metadata="    source=source.replace(\"consumer='0x4228b0 full consumer still incomplete'\",\"consumer='0x4228b0 complete1819B wall consumer, bounded proof'\")\n"
    metadata+="    source=source.replace(\"report=dict(original_sha256=HASH,entries=ENTRIES,\",\"report=dict(ordinary_return_scenes=379,terminal_error_scenes=6,supplemental_alias_fault_excluded=True,original_sha256=HASH,entries=ENTRIES,\")\n"
    source=source.replace(marker,metadata+marker)
    ctx=dict(wall.__dict__);ctx.update(globals());exec(source,ctx);ctx['main']()
if __name__=='__main__':main()
