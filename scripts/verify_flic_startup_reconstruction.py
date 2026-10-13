#!/usr/bin/env python3
"""Compare flic startup functions against verified original and reconstructed x86 code.

Requires Python 3.10+ and unicorn==2.1.4. export-map additionally requires reccmp==0.1.7.
The original is never executed natively. File, allocator, pixelmap, font, and CRT
parser dependencies have deterministic fixtures; function bodies and inline string
operations execute as actual x86 instructions. All372 static table records are
compared, normalizing only relocated filename pointers to their exact bytes.

Examples:
  python scripts/verify_flic_startup_reconstruction.py export-map --build-directory build-flic-startup-matching --output build-audit/flic-startup/symbols.json
  python scripts/verify_flic_startup_reconstruction.py verify --original build-reference/originals/CARMA2_HW.EXE --recompiled build-flic-startup-matching/rec2.exe --symbols build-audit/flic-startup/symbols.json --output build-audit/flic-startup/results.json
"""
from pathlib import Path
import argparse,hashlib,json,os,re,struct,sys

ORIGINAL_SHA256='9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
parser=argparse.ArgumentParser(description=__doc__,formatter_class=argparse.RawDescriptionHelpFormatter)
commands=parser.add_subparsers(dest='command',required=True)
export=commands.add_parser('export-map',help='Export matched reccmp symbols and image hashes')
export.add_argument('--build-directory',type=Path,required=True)
export.add_argument('--output',type=Path,required=True)
verify=commands.add_parser('verify',help='Execute differential table, palette, initialization and parser cases')
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
    def table(self):
        rows=[]
        for i in range(372):
            row=list(struct.unpack('<9I',self.u.mem_read(self.address('gMain_flic_list')+36*i,36)))
            row[0]=self.string(row[0]).hex() if row[0] else None
            rows.append(row)
        return rows

reference_table=Image('orig').table()
recompiled_table=Image('recomp').table()
if reference_table!=recompiled_table:raise AssertionError('Original/reconstructed flic table differs')
if any(row[0] is None for row in reference_table[:340]) or any(row!=[None]+[0]*8 for row in reference_table[340:]):
    raise AssertionError('Expected340 initialized entries followed by32 zero entries')
table_digest=hashlib.sha256(json.dumps(reference_table,separators=(',',':')).encode()).hexdigest()

fixtures=[
    dict(name='missing_file',lines=None),
    dict(name='empty_file',lines=[]),
    dict(name='empty_zero_allocation',lines=[],zero_allocation=True),
    dict(name='centre_upper',lines=[b'MAINSTIL 12 34 1 C Hello world']),
    dict(name='centre_lower',lines=[b'MAINSTIL.FOO -12 +34 2 c Centred']),
    dict(name='left_upper',lines=[b'MAI2OPGL.FLI 1 2 1 L Last initialized flic']),
    dict(name='left_lower',lines=[b'MAINSTIL,3,4,0,l,Comma text, with slash/']),
    dict(name='right_upper',lines=[b'MAINSTIL\t5\t6\t-2\tR\tRight text']),
    dict(name='right_lower',lines=[b'MAINSTIL/7/8/2/r/Slash text']),
    dict(name='unsupported_justification',lines=[b'MAINSTIL 9 10 1 X Keep allocator sentinel']),
    dict(name='text_spacing',lines=[b'MAINSTIL 1 2 1 L,  leading and trailing  ']),
    dict(name='empty_text',lines=[b'MAINSTIL 1 2 1 L ']),
    dict(name='extended_text',lines=[b'MAINSTIL 1 2 1 L \x80\xff text']),
    dict(name='multi_record',lines=[b'MAINSTIL 1 2 1 C First',b'MAI2OPGL 3 4 2 L Last',b'DEMSTRT2 5 6 0 r Again']),
    dict(name='invalid_integer',lines=[b'MAINSTIL nope 2 1 L Invalid integer']),
    dict(name='extra_integer_suffix',lines=[b'MAINSTIL 12xyz -34suffix +2x L Numeric prefix']),
    dict(name='unknown_flic_null_tail',lines=[b'NOT_A_FLIC 1 2 1 L Unknown'],expected_fault=True),
    dict(name='case_sensitive_flic',lines=[b'mainstil 1 2 1 L Case mismatch'],expected_fault=True),
    dict(name='missing_first_token',lines=[b''],expected_fault=True),
    dict(name='missing_argument_token',lines=[b'MAINSTIL'],expected_fault=True),
    dict(name='fatal_path_with_nonnull_tail',lines=[b'NOT_A_FLIC 1 2 1 L Unknown'],nonnull_tail=True,expected_fatal=True),
]

def execute(function,fixture):
    def one(side):
        image=Image(side);u=image.u
        trace,allocations=[],[]
        cursor=[HEAP]
        line_index=[0]
        token_position=[0]
        fault=[]
        fatal=[False]
        lines=fixture.get('lines',[])
        image.global_write('gTranslation_count',99)
        image.global_write('gTranslations',HEAP+0x18000)
        image.global_write('gTrans_fonts',HEAP+0x18100)
        image.write(image.address('gTrans_fonts')+4,HEAP+0x18200)
        image.global_write('gPalette_pixels',HEAP+0x18300)
        image.global_write('gPalette',HEAP+0x18400)
        u.mem_write(image.address('gApplication_path'),b'C:/GAME/DATA/\0')
        if fixture.get('nonnull_tail'):
            for i in range(340,372):image.write(image.address('gMain_flic_list')+36*i,image.read(image.address('gMain_flic_list')))
        if function=='InitFlics' and fixture.get('dirty'):
            for i in range(372):image.write(image.address('gMain_flic_list')+36*i+28,0x12340000+i)
        hooks=['BrMemAllocate','DRPixelmapAllocate','PathCat','PFfopen','PFfeof','PFrewind',
               'PFfclose','GetALineAndDontArgue','LoadFont','FatalError','_strtok','_sscanf']
        hooks={image.address(n):n for n in hooks}
        def invalid(uc,access,address,size,value,data):
            fault.append(dict(access=access,address=address,size=size))
            return False
        def hook(uc,at,size,data):
            if at==STOP:uc.emu_stop();return
            if at not in hooks:return
            name=hooks[at]
            sp=uc.reg_read(UC_X86_REG_ESP)
            ecx,edx=uc.reg_read(UC_X86_REG_ECX),uc.reg_read(UC_X86_REG_EDX)
            value,extra=0,0
            event={'call':name}
            if name=='BrMemAllocate':
                amount,tag=image.read(sp+4),image.read(sp+8)
                if amount>65536:raise AssertionError(('Excessive fixture allocation',amount))
                value=0 if fixture.get('allocation_failure') or (amount==0 and fixture.get('zero_allocation')) else cursor[0]
                if value:
                    u.mem_write(value,b'\xa5'*max(amount,1))
                    cursor[0]+=max(16,(amount+15)&~15)
                allocations.append(dict(address=value,size=amount,tag=tag))
                event.update(size=amount,tag=tag,result=value)
            elif name=='DRPixelmapAllocate':
                parameters=[ecx&255,edx&65535,image.read(sp+4)&65535,image.read(sp+8),image.read(sp+12)]
                if parameters[:3]!=[7,1,256] or parameters[4]!=0:raise AssertionError(('Pixelmap arguments',parameters))
                value=0 if fixture.get('pixelmap_failure') else HEAP+0x19000
                extra=12;event.update(parameters=parameters,result=value)
            elif name=='PathCat':
                first,second=image.string(edx),image.string(image.read(sp+4))
                if second!=b'TRNSLATE.TXT':raise AssertionError(('Translation filename',second))
                u.mem_write(ecx,first+second+b'\0');extra=4
                event.update(first=first.hex(),second=second.hex())
            elif name=='PFfopen':
                path,mode=image.string(ecx),image.string(edx)
                if path!=b'C:/GAME/DATA/TRNSLATE.TXT' or mode!=b'rt':raise AssertionError(('File arguments',path,mode))
                value=0 if lines is None else 0x1030000
                event.update(path=path.hex(),mode=mode.hex(),result=value)
            elif name=='PFfeof':value=int(line_index[0]>=len(lines))
            elif name=='PFrewind':line_index[0]=0
            elif name=='GetALineAndDontArgue':
                if line_index[0]>=len(lines):raise AssertionError('Read past fixture EOF')
                line=lines[line_index[0]]
                if len(line)>255:raise AssertionError('Oversized fixture line')
                u.mem_write(edx,line+b'\0');line_index[0]+=1;value=edx
                event.update(line=line.hex())
            elif name=='LoadFont':event.update(index=ecx)
            elif name=='FatalError':
                event.update(index=image.read(sp+4),filename=image.string(image.read(sp+8)).hex())
                fatal[0]=True;trace.append(event);uc.emu_stop();return
            elif name=='PFfclose':raise AssertionError('Original translation loader does not close its file')
            elif name=='_strtok':
                start,delimiters=image.read(sp+4),image.string(image.read(sp+8))
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
                text_pointer=image.read(sp+4)
                if text_pointer==0:
                    event.update(text=None,external_fault='null scanf input')
                    trace.append(event)
                    fault.append(dict(access=UC_MEM_READ_UNMAPPED,address=0,size=1))
                    uc.emu_stop();return
                text=image.string(text_pointer)
                fmt=image.string(image.read(sp+8))
                destination=image.read(sp+12)
                if fmt==b'%d':
                    match=re.match(rb'\s*([+-]?\d+)',text)
                    if match:image.write(destination,int(match.group(1)));value=1
                elif fmt==b'%c':
                    if text:u.mem_write(destination,text[:1]);value=1
                else:raise AssertionError(('Unexpected scanf format',fmt))
                event.update(text=text.hex(),format=fmt.hex(),converted=value)
            trace.append(event)
            uc.reg_write(UC_X86_REG_EAX,value&0xffffffff)
            uc.reg_write(UC_X86_REG_EIP,image.read(sp))
            uc.reg_write(UC_X86_REG_ESP,sp+4+extra)
        u.hook_add(UC_HOOK_CODE,hook)
        u.hook_add(UC_HOOK_MEM_INVALID,invalid)
        def invoke():
            image.write(STACK-4,STOP)
            u.reg_write(UC_X86_REG_ESP,STACK-4)
            try:u.emu_start(image.address(function),STOP,count=500000)
            except UcError:
                if not fault:raise
            if not fault and not fatal[0] and u.reg_read(UC_X86_REG_EIP)!=STOP:raise AssertionError('Function did not return')
        invoke()
        if function=='InitFlics' and fixture.get('repeat'):invoke()
        count=image.global_read('gTranslation_count')
        translations=[]
        if function=='LoadInterfaceStrings' and lines is not None:
            pointer=image.global_read('gTranslations')
            for i in range(count):
                record=list(struct.unpack('<6I',u.mem_read(pointer+24*i,24)))
                if record[5]!=0xa5a5a5a5:record[5]=image.string(record[5]).hex()
                translations.append(record)
        fonts=[]
        for i in range(2):
            pointer=image.read(image.address('gTrans_fonts')+i*4)
            offset=pointer-image.address('gFonts')
            fonts.append(f'gFonts[{offset//0x3a0}]' if 0<=offset<24*0x3a0 and offset%0x3a0==0 else pointer)
        return dict(trace=trace,allocations=allocations,fault=fault,fatal=fatal[0],
                    table=image.table(),translation_count=count,translation_pointer=image.global_read('gTranslations'),
                    translations=translations,fonts=fonts,palette=image.global_read('gPalette'),pixels=image.global_read('gPalette_pixels'))
    expected,actual=one('orig'),one('recomp')
    if expected!=actual:raise AssertionError((function,fixture['name'],{k:(expected[k],actual[k]) for k in expected if expected[k]!=actual[k]}))
    if bool(actual['fault'])!=bool(fixture.get('expected_fault')) or actual['fatal']!=bool(fixture.get('expected_fatal')):
        raise AssertionError((function,fixture['name'],'Unexpected completion state',actual['fault'],actual['fatal']))
    if function=='InitFlics':
        for i,row in enumerate(actual['table']):
            if row[7]!=0 or row[:7]+row[8:]!=reference_table[i][:7]+reference_table[i][8:]:raise AssertionError('InitFlics changed fields besides data_ptr')
    return {'function':function,'fixture':fixture['name'],'pass':True,'trace':actual['trace'],
            'fault':actual['fault'],'fatal':actual['fatal'],'allocations':actual['allocations'],
            'translations':actual['translations'],'translation_count':actual['translation_count'],'fonts':actual['fonts']}

records=[]
for fixture in [dict(name='allocate_palette'),dict(name='null_pixels',allocation_failure=True),dict(name='null_pixelmap',pixelmap_failure=True)]:
    records.append(execute('FlicPaletteAllocate',fixture))
for fixture in [dict(name='initial_zero_pointers'),dict(name='clear_all372',dirty=True),dict(name='repeat_initialization',dirty=True,repeat=True)]:
    records.append(execute('InitFlics',fixture))
for fixture in fixtures:records.append(execute('LoadInterfaceStrings',fixture))
cli.output.parent.mkdir(parents=True,exist_ok=True)
cli.output.write_text(json.dumps({'format':1,'original_sha256':ORIGINAL_SHA256,'recompiled_sha256':mapping['recompiled_sha256'],
                                 'table_records':372,'initialized_records':340,'zero_records':32,'normalized_table_sha256':table_digest,'scenarios':records},indent=2))
print(f'PASS: all372 table records and {len(records)} original/recompiled x86 startup scenarios agree.')
