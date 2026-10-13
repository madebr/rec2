#!/usr/bin/env python3
"""Compare InitHeadups startup behavior against verified original and reconstructed x86 code.

Requires Python 3.10+ and unicorn==2.1.4. export-map additionally requires reccmp==0.1.7.
The original is never executed natively. File, pixelmap, BRender update, and CRT
parser dependencies have deterministic fixtures; function bodies and inline string
operations execute as actual x86 instructions. Dependency calls include complete
initialized-state snapshots. Both instruction streams perform the inline string
copies and vertex math. BrFailure is intercepted as a fatal diagnostic boundary;
this does not test BRender diagnostic callbacks or asset/file decoding.

Examples:
  python scripts/verify_headup_startup_reconstruction.py export-map --build-directory build-headups-matching --output build-audit/headups/symbols.json
  python scripts/verify_headup_startup_reconstruction.py verify --original build-reference/originals/CARMA2_HW.EXE --recompiled build-headups-matching/rec2.exe --symbols build-audit/headups/symbols.json --output build-audit/headups/results.json
"""
from pathlib import Path
import argparse,hashlib,json,os,re,struct,sys

ORIGINAL_SHA256='9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
parser=argparse.ArgumentParser(description=__doc__,formatter_class=argparse.RawDescriptionHelpFormatter)
commands=parser.add_subparsers(dest='command',required=True)
export=commands.add_parser('export-map',help='Export matched reccmp symbols and image hashes')
export.add_argument('--build-directory',type=Path,required=True)
export.add_argument('--output',type=Path,required=True)
verify=commands.add_parser('verify',help='Execute differential HUD initialization and parser cases')
for option in ('original','recompiled','symbols','output'):
    verify.add_argument('--'+option,type=Path,required=True)
cli=parser.parse_args()
def digest(path):return hashlib.sha256(path.read_bytes()).hexdigest()

if cli.command=='export-map':
    from reccmp.project.detect import argparse_parse_project_target
    from reccmp.compare import Compare
    output=cli.output.resolve()
    os.chdir(cli.build_directory)
    target=argparse_parse_project_target(argparse.Namespace(target='CARMA2_HW'))
    if digest(target.original_path)!=ORIGINAL_SHA256:parser.error('Incorrect original binary hash')
    compare=Compare.from_target(target)
    mapping={'format':1,'original_sha256':ORIGINAL_SHA256,'recompiled_sha256':digest(target.recompiled_path),
             'symbols':[dict(name=e.name,orig=e.orig_addr,recomp=e.recomp_addr) for e in compare.get_all() if e.orig_addr and e.recomp_addr]}
    output.parent.mkdir(parents=True,exist_ok=True)
    output.write_text(json.dumps(mapping,indent=2))
    print(f'Exported {len(mapping["symbols"])} matched symbols to {output}')
    sys.exit(0)

if digest(cli.original)!=ORIGINAL_SHA256:parser.error('Incorrect original binary hash')
mapping=json.loads(cli.symbols.read_text())
if mapping.get('format')!=1 or mapping.get('original_sha256')!=ORIGINAL_SHA256:parser.error('Invalid symbol map')
if mapping.get('recompiled_sha256')!=digest(cli.recompiled):parser.error('Stale symbol map: export a fresh map for this reconstructed image')
symbols={entry['name']:entry for entry in mapping['symbols']}
try:
    from unicorn import Uc,UcError,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE,UC_HOOK_MEM_INVALID,UC_MEM_READ_UNMAPPED
    from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_EAX,UC_X86_REG_ECX,UC_X86_REG_EDX
except ImportError:parser.error('Install the verifier dependency: python -m pip install unicorn==2.1.4')

STOP,STACK,HEAP=0x1000000,0x1010000,0x1040000

class Image:
    def __init__(self,side):
        self.side=side
        binary=(cli.original if side=='orig' else cli.recompiled).read_bytes()
        pe=struct.unpack_from('<I',binary,0x3c)[0]
        count=struct.unpack_from('<H',binary,pe+6)[0]
        table=pe+24+struct.unpack_from('<H',binary,pe+20)[0]
        base=struct.unpack_from('<I',binary,pe+52)[0]
        size=struct.unpack_from('<I',binary,pe+80)[0]
        self.u=Uc(UC_ARCH_X86,UC_MODE_32)
        self.u.mem_map(base,(size+4095)&~4095)
        self.u.mem_map(STOP,0x200000)
        for i in range(count):
            virtual,va,raw_size,raw=struct.unpack_from('<IIII',binary,table+40*i+8)
            if raw_size:self.u.mem_write(base+va,binary[raw:raw+raw_size])
    def address(self,name):return symbols[name][self.side]
    def write(self,address,value):self.u.mem_write(address,struct.pack('<I',value&0xffffffff))
    def read(self,address):return struct.unpack('<I',self.u.mem_read(address,4))[0]
    def global_write(self,name,value):self.write(self.address(name),value)
    def global_read(self,name):return self.read(self.address(name))
    def string(self,address):
        result=bytearray()
        while True:
            ch=self.u.mem_read(address+len(result),1)[0]
            if ch==0:return bytes(result)
            result.append(ch)
            if len(result)>4096:raise AssertionError('Unterminated fixture string')

# Layouts are proved by original instruction offsets, with matching x86 types:
# tHeadup=0x164, tHud_message=0x88, br_pixelmap=0x44 (width/height +0x34/+0x36),
# br_material=0x9c (colour_map +0x40), br_model=0x54 (vertices +8), vertex=0x28.
PIXEL_GLOBALS = ['gArmour_colour_map','gPowerbar','gPower_colour_map',
    'gOffensive_colour_map','gIcon_test','gCurrent_rev','gHud_curve1','gHud_curve2',
    'gHud_curve3','gGrey_top5','gDamage_hud','gTimer_lhs','gHud_timer_rhs',
    'gIcon_armour','gIcon_power','gIcon_offense','gIcon_greyBloc1',
    'gIcon_litBloc1','gIcon_grnBlock1']
MATERIAL_GLOBALS = ['gStatbarHUD5_material','gTimerLeftHUD_material','gTimerRightHUD_material']
MODEL_GLOBALS = ['gTimerLeftHUD_model','gTimerRightHUD_model']
LOAD_NAMES = [f'rev{i}.tif'.encode() for i in range(1,14)] + [
    b'powerbar.tif',b'powerbar.tif',b'powerbar.tif',b'icontest.tif',
    b'hudcurve1.tif',b'hudcurve2.tif',b'hudcurve3.tif',b'greytop5.tif',
    b'timerlhs.tif',b'timerrhs.tif',b'iconarmour.tif',b'iconpower.tif',
    b'iconoffense.tif',b'greybloc1.tif',b'litbloc1.tif',b'grnblock1.tif']

fixtures = [
    dict(name='no_messages',count=0,lines=[]),
    dict(name='negative_count',count=-1,lines=[]),
    dict(name='minimum_count',count=-2147483648,lines=[]),
    dict(name='missing_file',lines=None,expected_fatal=True),
    dict(name='too_many_messages',count=47,lines=[],expected_fatal=True),
    dict(name='maximum_count',count=2147483647,lines=[],expected_fatal=True),
    dict(name='two_fonts_and_underscores',lines=[b'1 2 Hello_world__']),
    dict(name='all46_messages',lines=[f'{i} {-i} Message_{i}'.encode() for i in range(46)]),
    dict(name='delimiters_and_extra_token',lines=[b'\t /+3,, -4 /__First__,ignored']),
    dict(name='literal_spaces_end_token',lines=[b'1 2 First second']),
    dict(name='extended_bytes',lines=[b'5 6 _\x80_\xff_']),
    dict(name='invalid_font1_keeps_bytes',lines=[b'nope 2 Message']),
    dict(name='invalid_font2_keeps_bytes',lines=[b'1 nope Message']),
    dict(name='numeric_prefixes',lines=[b'-12suffix +34x Message']),
    dict(name='mixed_multi_records',lines=[b'1 2 One_Two',b'3 4 Three__Four',b'5 6 Last']),
    dict(name='message127',lines=[b'1 2 '+b'x'*127]),
    dict(name='message128_overwrites_font1',lines=[b'1 2 '+b'x'*128]),
    dict(name='message180_overwrites_next_record',lines=[b'1 2 '+b'x'*180]),
    dict(name='all_underscores',lines=[b'1 2 '+b'_'*127]),
    dict(name='no_tokens',lines=[b''],expected_fault=True),
    dict(name='only_font1',lines=[b'1'],expected_fault=True),
    dict(name='missing_message',lines=[b'1 2'],expected_fault=True),
    dict(name='negative_dimensions',lines=[],dimensions=[-17,-19,-23,-29,-31,-37,-41,-43]),
    dict(name='float_rounding_dimensions',lines=[],dimensions=[16777217,16777219,16777221,16777223,-16777217,-16777219,-16777221,-16777223]),
    dict(name='integer_limit_dimensions',lines=[],dimensions=[2147483647,-2147483648,-2147483648,2147483647,-2147483648,2147483647,2147483647,-2147483648]),
    dict(name='large_pixelmap_dimensions',lines=[],pixel_dimensions=[65535,32768,32769,65534]),
    dict(name='zero_pixelmap_dimensions',lines=[],pixel_dimensions=[0,0,0,0]),
    dict(name='null_res_strdup',lines=[],null_strdup=True),
    dict(name='repeat_initialization',lines=[b'1 2 Hello_world'],repeat=True),
]
for index in range(29):
    fixtures.append(dict(name=f'null_load_{index+1}',lines=[],null_load=index,
                         expected_fault=index in (13,20)))
for index in range(2):
    fixtures.append(dict(name=f'null_pixelmap_allocation_{index+1}',lines=[],null_allocation=index,
                         expected_fault=index==0))
for name in MATERIAL_GLOBALS+MODEL_GLOBALS:
    fixtures.append(dict(name='null_'+name,lines=[],null_object=name,expected_fault=True))

def execute(fixture):
    def one(side):
        image=Image(side);u=image.u
        trace,objects,fault=[],[],[]
        fatal=[False];loads=[0];allocations=[0];line_index=[0];token_position=[0]
        cursor=[HEAP+0x4000]
        lines=fixture.get('lines',[])
        count=fixture.get('count',len(lines) if lines is not None else 0)
        headup_seed=bytes((i*17+91)&255 for i in range(37*0x164))
        message_seed=b'\xa5'*(46*0x88)
        u.mem_write(image.address('gHeadups'),headup_seed)
        u.mem_write(image.address('gHud_messages'),message_seed)
        image.global_write('gHeadup_detail_level',9)
        u.mem_write(image.address('gApplication_path'),b'C:/GAME/DATA/\0')
        for i,name in enumerate(PIXEL_GLOBALS):image.global_write(name,0xdead0000+i)
        for i in range(13):image.write(image.address('gRevs')+4*i,0xdead1000+i)
        fonts=[HEAP+0x100,HEAP+0x200,HEAP+0x300,HEAP+0x400]
        for name,value in zip(['BrFontProp4x6','BrFontProp7x9','gFont_7','gHeadup_font'],fonts):
            image.global_write(name,value)
        u.mem_write(image.address('gBR_fonts'),b'\xa5'*16)
        materials={}
        models={}
        vertices={}
        material_seed=bytes((i*29+37)&255 for i in range(0x9c))
        model_seed=bytes((i*13+51)&255 for i in range(0x54))
        vertex_seed=bytes((i*19+71)&255 for i in range(4*0x28))
        for i,name in enumerate(MATERIAL_GLOBALS):
            address=HEAP+0x1000+i*0x100
            materials[name]=address
            u.mem_write(address,material_seed)
            image.global_write(name,0 if fixture.get('null_object')==name else address)
        for i,name in enumerate(MODEL_GLOBALS):
            address=HEAP+0x2000+i*0x100;vertex_address=HEAP+0x2800+i*0x100
            models[name]=address;vertices[name]=vertex_address
            u.mem_write(address,model_seed);image.write(address+8,vertex_address)
            u.mem_write(vertex_address,vertex_seed)
            image.global_write(name,0 if fixture.get('null_object')==name else address)
        dimensions=fixture.get('dimensions',[270,0,64,64,334,0,64,64])
        for i,prefix in enumerate(['gTimerLeftHUD','gTimerRightHUD']):
            for suffix,value in zip(['_dim_x','_dim_y','_dim_w','_dim_h'],dimensions[4*i:4*i+4]):
                image.global_write(prefix+suffix,value)

        def state():
            return dict(
                headups=bytes(u.mem_read(image.address('gHeadups'),37*0x164)).hex(),
                messages=bytes(u.mem_read(image.address('gHud_messages'),46*0x88)).hex(),
                fonts=[image.read(image.address('gBR_fonts')+i*4) for i in range(4)],
                revs=[image.read(image.address('gRevs')+i*4) for i in range(13)],
                globals={name:image.global_read(name) for name in PIXEL_GLOBALS},
                detail=image.global_read('gHeadup_detail_level'),
                materials={name:bytes(u.mem_read(address,0x9c)).hex() for name,address in materials.items()},
                models={name:bytes(u.mem_read(address,0x54)).hex() for name,address in models.items()},
                vertices={name:bytes(u.mem_read(address,0xa0)).hex() for name,address in vertices.items()},
                objects=[dict(address=a,size=n,data=bytes(u.mem_read(a,n)).hex()) for a,n in objects])
        def state_digest():
            return hashlib.sha256(json.dumps(state(),sort_keys=True,separators=(',',':')).encode()).hexdigest()
        names=['LoadPixelmap','BrPixelmapAllocate','BrResStrDup','BrMapAdd','BrMapUpdate',
               'BrMaterialUpdate','BrModelUpdate','PathCat','DRfopen','GetAnInt',
               'GetALineAndDontArgue','PFfclose','BrFailure','_strtok','_sscanf']
        hooks={image.address(name):name for name in names}
        def invalid(uc,access,address,size,value,data):
            fault.append(dict(access=access,address=address,size=size));return False
        def hook(uc,at,size,data):
            if at==STOP:uc.emu_stop();return
            if at not in hooks:return
            name=hooks[at];sp=uc.reg_read(UC_X86_REG_ESP)
            ecx,edx=uc.reg_read(UC_X86_REG_ECX),uc.reg_read(UC_X86_REG_EDX)
            value,extra=0,0
            event=dict(call=name,state_sha256=state_digest())
            def arg(n):return image.read(sp+4*n)
            if name=='LoadPixelmap':
                text=image.string(ecx)
                if text!=LOAD_NAMES[loads[0]%29]:raise AssertionError(('Unexpected load',loads[0],text))
                value=0 if fixture.get('null_load')==loads[0]%29 else cursor[0]
                if value:
                    u.mem_write(value,b'\xa5'*0x44)
                    dims=fixture.get('pixel_dimensions',[73,19,31,47])
                    w,h=dims[:2] if loads[0]%29==13 else dims[2:]
                    u.mem_write(value+0x34,struct.pack('<HH',w,h))
                    objects.append((value,0x44));cursor[0]+=0x80
                loads[0]+=1;event.update(filename=text.hex(),result=value)
            elif name=='BrPixelmapAllocate':
                parameters=[arg(i) for i in range(1,6)]
                if parameters[0]!=18 or parameters[3:]!=[0,0]:raise AssertionError(('Allocation arguments',parameters))
                value=0 if fixture.get('null_allocation')==allocations[0]%2 else cursor[0]
                if value:
                    u.mem_write(value,b'\xa5'*0x44)
                    u.mem_write(value+0x34,struct.pack('<HH',parameters[1]&65535,parameters[2]&65535))
                    objects.append((value,0x44));cursor[0]+=0x80
                allocations[0]+=1;event.update(parameters=parameters,result=value)
            elif name=='BrResStrDup':
                text=image.string(arg(2))
                if text!=b'Armour_area':raise AssertionError(('Unexpected identifier',text))
                value=0 if fixture.get('null_strdup') else cursor[0]
                if value:
                    u.mem_write(value,text+b'\0');objects.append((value,len(text)+1));cursor[0]+=0x80
                event.update(resource=arg(1),text=text.hex(),result=value)
            elif name=='BrMapAdd':event.update(pixelmap=arg(1));value=arg(1)
            elif name in ('BrMapUpdate','BrMaterialUpdate','BrModelUpdate'):
                event.update(object=arg(1),flags=arg(2))
            elif name=='PathCat':
                first,second=image.string(edx),image.string(arg(1))
                if second!=b'SCROLLERS.TXT':raise AssertionError(('Unexpected filename',second))
                u.mem_write(ecx,first+second+b'\0');extra=4
                event.update(first=first.hex(),second=second.hex())
            elif name=='DRfopen':
                path,mode=image.string(ecx),image.string(edx)
                if path!=b'C:/GAME/DATA/SCROLLERS.TXT' or mode!=b'rt':raise AssertionError(('File arguments',path,mode))
                value=0 if lines is None else HEAP+0x800
                event.update(path=path.hex(),mode=mode.hex(),result=value)
            elif name=='GetAnInt':
                if ecx!=HEAP+0x800:raise AssertionError(('Invalid file',ecx))
                value=count;event.update(value=count)
            elif name=='GetALineAndDontArgue':
                if ecx!=HEAP+0x800 or line_index[0]>=len(lines):raise AssertionError('Read past fixture EOF')
                line=lines[line_index[0]]
                if len(line)>255:raise AssertionError('Oversized fixture line')
                u.mem_write(edx,line+b'\0');line_index[0]+=1;value=edx
                event.update(line=line.hex())
            elif name=='PFfclose':
                if ecx!=HEAP+0x800:raise AssertionError(('Invalid close',ecx))
                event.update(file=ecx)
            elif name=='BrFailure':
                event.update(message=image.string(arg(1)).hex(),path=image.string(arg(2)).hex())
                fatal[0]=True;trace.append(event);uc.emu_stop();return
            elif name=='_strtok':
                start,delimiters=arg(1),image.string(arg(2))
                if start:token_position[0]=start
                position=token_position[0]
                if position:
                    while u.mem_read(position,1)[0] in delimiters:position+=1
                    if u.mem_read(position,1)[0]:
                        value=position
                        while u.mem_read(position,1)[0] and u.mem_read(position,1)[0] not in delimiters:position+=1
                        if u.mem_read(position,1)[0]:u.mem_write(position,b'\0');position+=1
                        token_position[0]=position
                    else:token_position[0]=position
                event.update(delimiters=delimiters.hex(),token=image.string(value).hex() if value else None)
            elif name=='_sscanf':
                pointer=arg(1)
                if pointer==0:
                    event.update(text=None,external_fault='null scanf input')
                    trace.append(event);fault.append(dict(access=UC_MEM_READ_UNMAPPED,address=0,size=1))
                    uc.emu_stop();return
                text,fmt=image.string(pointer),image.string(arg(2))
                if fmt!=b'%d':raise AssertionError(('Unexpected scanf format',fmt))
                match=re.match(rb'\s*([+-]?\d+)',text)
                if match:image.write(arg(3),int(match.group(1)));value=1
                event.update(text=text.hex(),format=fmt.hex(),converted=value)
            trace.append(event)
            uc.reg_write(UC_X86_REG_EAX,value&0xffffffff)
            uc.reg_write(UC_X86_REG_EIP,image.read(sp))
            uc.reg_write(UC_X86_REG_ESP,sp+4+extra)
        u.hook_add(UC_HOOK_CODE,hook);u.hook_add(UC_HOOK_MEM_INVALID,invalid)
        def invoke():
            line_index[0]=0
            image.write(STACK-4,STOP);u.reg_write(UC_X86_REG_ESP,STACK-4)
            try:u.emu_start(image.address('InitHeadups'),STOP,count=1000000)
            except UcError:
                if not fault:raise
            if not fault and not fatal[0] and u.reg_read(UC_X86_REG_EIP)!=STOP:raise AssertionError('Function did not return')
        invoke()
        if fixture.get('repeat'):invoke()
        result=dict(trace=trace,fault=fault,fatal=fatal[0],state=state())
        cleared=bytearray(headup_seed)
        for i in range(37):cleared[i*0x164:i*0x164+4]=b'\0'*4
        if result['state']['headups']!=cleared.hex():raise AssertionError('Headup reset changed fields besides type')
        if result['state']['fonts']!=fonts or result['state']['detail']!=9:raise AssertionError('Font/detail initialization differs')
        return result
    expected,actual=one('orig'),one('recomp')
    if expected!=actual:
        raise AssertionError((fixture['name'],{k:(expected[k],actual[k]) for k in expected if expected[k]!=actual[k]}))
    if bool(actual['fault'])!=bool(fixture.get('expected_fault')) or actual['fatal']!=bool(fixture.get('expected_fatal')):
        raise AssertionError((fixture['name'],'Unexpected completion state',actual['fault'],actual['fatal']))
    return dict(fixture=fixture['name'],passed=True,trace=actual['trace'],fault=actual['fault'],
                fatal=actual['fatal'],final_state_sha256=hashlib.sha256(json.dumps(actual['state'],sort_keys=True).encode()).hexdigest())

records=[execute(fixture) for fixture in fixtures]
cli.output.parent.mkdir(parents=True,exist_ok=True)
cli.output.write_text(json.dumps(dict(format=1,original_sha256=ORIGINAL_SHA256,
    recompiled_sha256=mapping['recompiled_sha256'],function='InitHeadups',address='0x00449090',scenarios=records),indent=2))
print(f'PASS: {len(records)} original/recompiled x86 InitHeadups scenarios agree.')
