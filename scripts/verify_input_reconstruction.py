#!/usr/bin/env python3
"""Differential verification of reconstructed input functions against verified CARMA2_HW.

Dependencies: Python 3.10+, unicorn==2.1.4; export-map additionally needs reccmp==0.1.7.
The verified original is never executed natively. Both x86 images run under Unicorn
with identical deterministic dependency fixtures. Tests compare full input arrays,
key repeat state, mouse coordinates, results, and dependency call order.
Rolling/typing fixtures also compare every pool byte, signed characters, descending
random fills, full-pool failures, visible bounds, typing buffers, and cursor state.

Example from the repository root:
  python scripts/verify_input_reconstruction.py export-map --build-directory build-input-matching --output build-audit/input/symbols.json
  python scripts/verify_input_reconstruction.py verify --original build-reference/originals/CARMA2_HW.EXE --recompiled build-input-matching/rec2.exe --symbols build-audit/input/symbols.json --output build-audit/input/differential-results.json

The symbol map includes image hashes; stale maps and incorrect originals are rejected.
These fixtures isolate platform dependencies and do not test the entire running game.
"""
from pathlib import Path
import argparse, hashlib, json, os, struct, sys

ORIGINAL_SHA256 = '9b896c2cbb170c01b3e9f904ce5e1808db29fe5b51184a5d55a6d19b1799b58d'
parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
commands = parser.add_subparsers(dest='command', required=True)
export = commands.add_parser('export-map', help='Export matched addresses and image hashes using reccmp')
export.add_argument('--build-directory', type=Path, required=True)
export.add_argument('--output', type=Path, required=True)
verify = commands.add_parser('verify', help='Compare the original and reconstructed x86 input functions')
verify.add_argument('--original', type=Path, required=True)
verify.add_argument('--recompiled', type=Path, required=True)
verify.add_argument('--symbols', type=Path, required=True)
verify.add_argument('--output', type=Path, required=True)
cli = parser.parse_args()

def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()

if cli.command == 'export-map':
    from reccmp.project.detect import argparse_parse_project_target
    from reccmp.compare import Compare
    output = cli.output.resolve()
    os.chdir(cli.build_directory)
    target = argparse_parse_project_target(argparse.Namespace(target='CARMA2_HW'))
    original_hash = digest(target.original_path)
    if original_hash != ORIGINAL_SHA256:
        parser.error('Original binary SHA256 does not match reccmp-project.yml')
    compare = Compare.from_target(target)
    mapping = {'format': 1, 'original_sha256': original_hash,
               'recompiled_sha256': digest(target.recompiled_path),
               'symbols': [dict(name=e.name, orig=e.orig_addr, recomp=e.recomp_addr)
                           for e in compare.get_all() if e.orig_addr and e.recomp_addr]}
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(mapping, indent=2))
    print(f'Exported {len(mapping["symbols"])} matched symbols to {output}')
    sys.exit(0)

if digest(cli.original) != ORIGINAL_SHA256:
    parser.error('Original binary SHA256 does not match reccmp-project.yml')
mapping = json.loads(cli.symbols.read_text())
if mapping.get('format') != 1 or mapping.get('original_sha256') != ORIGINAL_SHA256:
    parser.error('Invalid symbol map or original hash')
if mapping.get('recompiled_sha256') != digest(cli.recompiled):
    parser.error('Recompiled binary SHA256 differs from the symbol map; export a fresh map')
symbols = {e['name']: e for e in mapping['symbols']}
try:
    from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
    from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EIP, UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX
except ImportError:
    parser.error('Install the verifier dependency: python -m pip install unicorn==2.1.4')
STOP, STACK, BUFFER, HEAP = 0x1000000, 0x1010000, 0x1020000, 0x1030000

rolling_cases = {
    'AddRollingLetter': [
        dict(pool=p, rolling_type=t, char=c, letter_count=n)
        for p in ('first','last','full') for t in range(4)
        for c in (65,128,255) for n in ((3,9) if t<2 else (3,))
    ] + [dict(pool='first',rolling_type=4,char=255,letter_count=6)],
    'ChangeCharTo': [
        dict(pool=p,char=c,index=1,slot=14,visible=4,letter_count=6)
        for p in ('first','existing','last_existing','full','duplicates')
        for c in (65,48,57,32,96,128,255)
    ] + [dict(pool='first',char=96,index=i,slot=0,visible=v,letter_count=3)
         for i,v in ((-1,4),(0,0),(4,4),(3,4),(0,-1))],
    'RevertTyping': [
        dict(pool=p,text=t,total=n,visible=4,slot=0,letter_count=5)
        for p in ('first','existing','full')
        for t,n in ((b'',0),(b'',4),(b'A9',4),(b'\x80\xff`',4),(b'A9ZQXY',6),(b'A',-1))
    ],
    'BlankSlot': [
        dict(pool=p,name_length=n,visible=v,slot=0,letter_count=3)
        for p in ('first','existing','full')
        for n,v in ((-1,4),(0,4),(1,4),(6,4),(4,0),(4,-1))
    ],
    'StartTyping': [
        dict(pool=p,text=t,visible=v,slot=14,letter_count=5)
        for p in ('first','existing','full')
        for t,v in ((b'',4),(b'A9',4),(b'ABCD',4),(b'ABCDE',4),
                    (b'\x80\xff',4),(b'A',0),(b'A',-1),(b'A'*109,110))
    ],
}

def execute(side, name, scenario):
    path = cli.original if side == 'orig' else cli.recompiled
    binary = path.read_bytes()
    pe = struct.unpack_from('<I', binary, 0x3c)[0]
    count, optsize = struct.unpack_from('<H', binary, pe+6)[0], struct.unpack_from('<H', binary, pe+20)[0]
    base = struct.unpack_from('<I', binary, pe+52)[0]
    image_size = struct.unpack_from('<I', binary, pe+80)[0]
    u = Uc(UC_ARCH_X86, UC_MODE_32)
    u.mem_map(base, (image_size+4095)&~4095)
    u.mem_map(STOP, 0x40000)
    for i in range(count):
        vs, va, rs, raw = struct.unpack_from('<IIII', binary, pe+24+optsize+i*40+8)
        if rs:
            u.mem_write(base+va, binary[raw:raw+rs])
    def address(n): return symbols[n][side]
    def write(a, value): u.mem_write(a, struct.pack('<I', value&0xffffffff))
    def read(a): return struct.unpack('<i', u.mem_read(a, 4))[0]
    def setglobal(n,v): write(address(n),v)
    def setarray(n,values):
        for i,v in enumerate(values): write(address(n)+4*i,v)
    def array(n,count): return [read(address(n)+4*i) for i in range(count)]
    for n in ['gKey_poll_counter','gLast_poll_keys','gLast_key_down_time','gModifiers_down','gLast_roll',
              'gCurrent_cursor','gJoy1_x','gJoy1_y','gJoy2_x','gJoy2_y','gGraf_spec_index']:
        setglobal(n,0)
    setglobal('gKey_poll_counter',5)
    setglobal('gLast_poll_keys',1000)
    setglobal('gLast_key_down',-1000)
    setglobal('gEdge_trigger_mode',0)
    setglobal('gRolling_letters',HEAP)
    setglobal('gJoystick_deadzone',0x1000)
    for n in ['gINT_006621e4','gINT_006621e8','gINT_006621ec','gINT_006621f0']: setglobal(n,1)
    setarray('gGo_ahead_keys',[51,52,106])
    setarray('gKey_mapping',range(77))
    setarray('gKey_array',[0]*151)
    setarray('gJoy_array',[7]*8)
    setarray('gCurrent_mouse_position',[5,6])
    write(address('gGraf_specs')+12,640)
    write(address('gGraf_specs')+16,480)
    u.mem_write(HEAP, b'\x5a'*28000)
    trace, ticks = [], [0]
    now = 1000
    args = [42,0]
    random_letter_count = 5
    random_letter_index = [0]
    px,py,click,left,right = 5,6,0,0,0
    cx,cy = 5,6
    axes = [10000,40000,60000,-1]
    buttons = [1,0,1,0,0,1,0,1]
    if name == 'GetMousePosition':
        px,py = [(-3,-4),(2000,3000),(640,480),(31,22)][scenario]
        args=[BUFFER,BUFFER+4]
    elif name == 'EitherMouseButtonDown':
        px,py,cx,cy,click,left,right = [
            (5,6,5,6,1,0,0),(5,6,5,6,0,0,0),(9,6,9,6,1,0,0),
            (5,6,8,6,1,1,0),(5,6,5,8,1,0,1),(5,6,5,6,0,1,1),
        ][scenario]
    elif name == 'CyclePollKeys':
        setarray('gKey_array',[((i%5)-1)*3 for i in range(151)])
    elif name in ['PDAnyKeyDown','AnyKeyDown']:
        setglobal('gEdge_trigger_mode',scenario>=3)
        key=[-1,4,42,42,42,42,142,10,-1][scenario]
        if key>=0: write(address('gKey_array')+key*4,1)
        if scenario==4:
            setglobal('gLast_key_down',42);setglobal('gLast_key_down_time',900)
        if scenario==5:
            setglobal('gLast_key_down',42);setglobal('gLast_key_down_time',600)
        if scenario==8: click=1
    elif name in ['EdgeTriggeryKey','PDKeyDown']:
        # Cases 10..12 straddle the 300 ms repeat threshold. Cases 13..15
        # repeat the same intervals with the previous timestamp near UINT32_MAX.
        key,down,mode,last,when,mods,modifier,reset = [
            (42,7,0,-1000,0,0,0,0),(42,0,1,42,900,0,0,0),
            (42,1,1,42,900,0,0,0),(42,1,1,42,600,0,0,0),
            (42,1,1,42,900,0,1,0),(42,1,1,-42,0,0,0,0),
            (10,1,1,-1000,0,0,0,0),(42,1,1,-1000,0,0,1,0),
            (42,1,1,42,900,1,1,0),(42,1,1,42,900,0,0,1),
            (42,1,1,42,701,0,0,0),(42,1,1,42,700,0,0,0),
            (42,1,1,42,699,0,0,0),
            (42,1,1,42,0xfffffff0,0,0,0),
            (42,1,1,42,0xfffffff0,0,0,0),
            (42,1,1,42,0xfffffff0,0,0,0),
        ][scenario]
        if scenario>=13:
            now=(0xfffffff0+299+scenario-13)&0xffffffff
            setglobal('gLast_poll_keys',now)
        args=[key,reset]
        write(address('gKey_array')+key*4,down)
        if modifier:write(address('gKey_array')+4,1)
        for n,v in [('gEdge_trigger_mode',mode),('gLast_key_down',last),('gLast_key_down_time',when),('gModifiers_down',mods)]:setglobal(n,v)
    elif name in ['KeyIsDown','KeyIsDownNoMouldiness']:
        args=[42 if scenario>=40 else [-2,-1,42,42,-1][scenario],0]
        if scenario==2:write(address('gKey_array')+42*4,7)
        if scenario==4:write(address('gKey_array')+106*4,1)
        if scenario>=40:
            now=(0xffffff00+499+scenario-40)&0xffffffff
            setglobal('gLast_poll_keys',0xffffff00)
            write(address('gKey_array')+42*4,7)
    elif name in ['PollKeys','SetJoystickArrays','CheckKeysForMouldiness']:
        args=[address('gKey_array'),17]
        if scenario==0:setglobal('gLast_poll_keys',0)
        elif scenario==1:
            for n in ['gINT_006621e4','gINT_006621e8','gINT_006621ec','gINT_006621f0']:setglobal(n,0)
            setglobal('gLast_poll_keys',0)
        elif scenario==2:
            axes=[0x4000,0x8000,0xc000,0x9000]
            for n in ['gJoy1_x','gJoy1_y','gJoy2_x','gJoy2_y']:setglobal(n,0x8000)
            setglobal('gLast_poll_keys',500)
        elif scenario==3:
            axes=[-1,-1,-1,-1];setglobal('gLast_poll_keys',499)
        elif 16<=scenario<32:
            # Every combination of the four independent axis-enable flags;
            # axis motion crosses lower/upper thresholds and all buttons are down.
            mask=scenario-16
            for bit,n in enumerate(['gINT_006621e4','gINT_006621e8','gINT_006621ec','gINT_006621f0']):
                setglobal(n,(mask>>bit)&1)
            axes=[0x3fff,0xc001,0x3fff,0xc001]
            buttons=[1]*8
            for n in ['gJoy1_x','gJoy1_y','gJoy2_x','gJoy2_y']:setglobal(n,0x8000)
            setglobal('gLast_poll_keys',0)
        elif scenario>=40:
            # 499/500/501 ms since the previous poll, crossing UINT32_MAX.
            now=(0xffffff00+499+scenario-40)&0xffffffff
            setglobal('gLast_poll_keys',0xffffff00)
    elif name=='WaitForNoKeys':
        write(address('gKey_array')+42*4,1)
    elif name in rolling_cases:
        fixture=rolling_cases[name][scenario]
        graf=BUFFER+0x1000
        setglobal('gCurrent_graf_data',graf)
        write(graf+0x10,9)
        write(graf+0x24,23)
        setarray('gLetter_x_coords',[100+i*10 for i in range(15)])
        setarray('gLetter_y_coords',[200+i*20 for i in range(15)])
        setglobal('gVisible_length',fixture.get('visible',4))
        setglobal('gThe_length',fixture.get('total',4))
        setglobal('gCurrent_position',2)
        setglobal('gCurrent_cursor',77)
        u.mem_write(address('gCurrent_typing'),b'old text\0'+b'\x33'*101)
        random_letter_count=fixture['letter_count']
        pool=fixture['pool']
        for i in range(500):write(HEAP+i*56+44,3 if pool in ('last','full') else -1)
        if pool=='last':write(HEAP+499*56+44,-1)
        slot=fixture.get('slot',0)
        target_index=fixture.get('index',len(fixture.get('text',b'')) if name=='StartTyping' else 0)
        if pool in ('existing','last_existing','duplicates'):
            indices=(499,) if pool=='last_existing' else (0,499) if pool=='duplicates' else range(6)
            for j in indices:
                char_index=target_index if name=='ChangeCharTo' or name=='StartTyping' else j
                record=HEAP+56*j
                write(record,70+j)
                write(record+36,100+slot*10+char_index*9)
                write(record+40,200+slot*20)
                write(record+44,7)
                write(record+48,1)
        if name=='AddRollingLetter':args=[fixture['char'],13,-20,fixture['rolling_type']]
        elif name=='ChangeCharTo':args=[slot,fixture['index'],fixture['char']]
        elif name=='BlankSlot':args=[slot,fixture['name_length'],fixture['visible']]
        else:
            text_pointer=BUFFER+0x2000
            u.mem_write(text_pointer,fixture['text']+b'\0')
            args=[slot,text_pointer]
            if name=='StartTyping':args.append(fixture['visible'])
    hooks=['PDGetTotalTime','PDSetKeyArray','PDSetKeysFromJoystick','PDReadJoysticks',
           'PDGetJoy1Button1','PDGetJoy1Button2','PDGetJoy1Button3','PDGetJoy1Button4',
           'PDGetJoy2Button1','PDGetJoy2Button2','PDGetJoy2Button3','PDGetJoy2Button4',
           'PDGetJoy1X','PDGetJoy1Y','PDGetJoy2X','PDGetJoy2Y',
           'PDGetMousePosition','PDGetMouseClickPosition','PDMouseButtons','CheckQuit','BrMemAllocate','BrMemFree','IRandomBetween']
    hooknames={address(n):n for n in hooks}
    def hook(uc,at,size,user):
        if at==STOP:uc.emu_stop();return
        if at not in hooknames:return
        n=hooknames[at]
        sp=uc.reg_read(UC_X86_REG_ESP)
        ecx,edx=uc.reg_read(UC_X86_REG_ECX),uc.reg_read(UC_X86_REG_EDX)
        if n!='IRandomBetween':trace.append(n)
        value=0
        if n=='PDGetTotalTime':value=now
        elif n=='IRandomBetween':
            if (ecx,edx)==(3,9):value=random_letter_count
            elif (ecx,edx)==(65,91):
                value=[65,90,91,66,89,70,80,88,77][random_letter_index[0]%9]
                random_letter_index[0]+=1
            else:raise AssertionError((side,name,scenario,'unexpected random bounds',ecx,edx))
            trace.append(f'IRandomBetween({ecx},{edx})->{value}')
        elif n=='PDSetKeyArray':write(ecx+42*4,edx)
        elif n=='PDSetKeysFromJoystick':write(ecx+43*4,9)
        elif n in ['PDGetJoy1X','PDGetJoy1Y','PDGetJoy2X','PDGetJoy2Y']:
            value=axes[['PDGetJoy1X','PDGetJoy1Y','PDGetJoy2X','PDGetJoy2Y'].index(n)]
        elif n.startswith('PDGetJoy'):
            value=buttons[(int(n[8])-1)*4+int(n[-1])-1]
        elif n=='PDGetMousePosition':write(ecx,px);write(edx,py)
        elif n=='PDGetMouseClickPosition':write(ecx,cx);write(edx,cy);value=click
        elif n=='PDMouseButtons':write(ecx,left);write(edx,right)
        elif n=='CheckQuit':
            ticks[0]+=1
            if ticks[0]>=2:setarray('gKey_array',[0]*151)
        elif n=='BrMemAllocate':
            if read(sp+4)!=28000 or read(sp+8)!=253:
                raise AssertionError((side,name,scenario,'incorrect rolling allocation size/tag'))
            value=HEAP
        elif n=='BrMemFree':
            if read(sp+4)!=HEAP:
                raise AssertionError((side,name,scenario,'incorrect rolling allocation pointer'))
        uc.reg_write(UC_X86_REG_EAX,value&0xffffffff)
        uc.reg_write(UC_X86_REG_EIP,read(sp)&0xffffffff)
        uc.reg_write(UC_X86_REG_ESP,sp+4)
    u.hook_add(UC_HOOK_CODE,hook)
    write(STACK-4,STOP)
    for i,arg in enumerate(args[2:]):write(STACK+i*4,arg)
    u.reg_write(UC_X86_REG_ESP,STACK-4)
    u.reg_write(UC_X86_REG_ECX,args[0]&0xffffffff)
    u.reg_write(UC_X86_REG_EDX,args[1]&0xffffffff)
    u.emu_start(address(name),STOP,count=100000)
    if u.reg_read(UC_X86_REG_EIP)!=STOP:
        raise AssertionError((side,name,scenario,'did not return'))
    result={'trace':trace,'keys':array('gKey_array',151),'joy':array('gJoy_array',8),
            'mouse':array('gCurrent_mouse_position',2),'output': [read(BUFFER),read(BUFFER+4)],
            'state':{n:read(address(n)) for n in ['gKey_poll_counter','gLast_poll_keys','gLast_key_down',
                    'gLast_key_down_time','gModifiers_down','gLast_roll','gCurrent_cursor','gJoy1_x','gJoy1_y','gJoy2_x','gJoy2_y']}}
    if name in ['PDAnyKeyDown','AnyKeyDown','PDKeyDown','EdgeTriggeryKey','KeyIsDown','KeyIsDownNoMouldiness','EitherMouseButtonDown']:
        result['return']=u.reg_read(UC_X86_REG_EAX)
    if name=='InitRollingLetters':result['letters']=[read(HEAP+56*i+44) for i in range(500)]
    if name in rolling_cases:
        result['rolling_pool_sha256']=hashlib.sha256(u.mem_read(HEAP,28000)).hexdigest()
        result['typing_buffer']=bytes(u.mem_read(address('gCurrent_typing'),110)).hex()
        result['typing_state']={n:read(address(n)) for n in ['gThe_length','gVisible_length','gCurrent_position','gCurrent_cursor']}
        result['rolling_records']=[list(struct.unpack('<14i',u.mem_read(HEAP+56*j,56))) for j in (0,1,499)]
        if name in ['AddRollingLetter','ChangeCharTo']:result['return']=u.reg_read(UC_X86_REG_EAX)
    return result

cases={'EitherMouseButtonDown':range(6),'GetMousePosition':range(4),'InitRollingLetters':range(1),
       'EndRollingLetters':range(1),'ResetPollKeys':range(1),'CyclePollKeys':range(1),
       'EdgeTriggeryKey':range(16),'PDKeyDown':[*range(9),*range(10,16)],'PDAnyKeyDown':range(9),'AnyKeyDown':range(9),
       'KeyIsDown':[*range(5),*range(40,43)],'KeyIsDownNoMouldiness':range(2,4),'PollKeys':[*range(4),*range(16,32)],
       'SetJoystickArrays':[*range(4),*range(16,32)],
       'CheckKeysForMouldiness':[*range(4),*range(16,32),*range(40,43)],'WaitForNoKeys':range(1)}
cases.update({name:range(len(fixtures)) for name,fixtures in rolling_cases.items()})
records=[]
for name,scenarios in cases.items():
    for scenario in scenarios:
        expected,actual=execute('orig',name,scenario),execute('recomp',name,scenario)
        if actual!=expected:
            raise AssertionError((name,scenario,{k:(expected[k],actual[k]) for k in expected if expected[k]!=actual[k]}))
        record={'function':name,'scenario':scenario,'pass':True,'trace':actual['trace']}
        if name in ['PollKeys','SetJoystickArrays','CheckKeysForMouldiness'] and 16<=scenario<32:
            record['joystick_enable_mask']=scenario-16
        elif name in ['EdgeTriggeryKey','PDKeyDown'] and scenario>=10:
            record['elapsed_ms']=299+(scenario-10)%3
            record['timestamp_wrap']=scenario>=13
            record['return']=actual['return']
            record['last_key_down_time']=actual['state']['gLast_key_down_time']&0xffffffff
        elif name in ['KeyIsDown','CheckKeysForMouldiness'] and scenario>=40:
            record['elapsed_ms']=499+scenario-40
            record['timestamp_wrap']=True
            record['repoll']='PDSetKeyArray' in actual['trace']
            if record['repoll']!=(record['elapsed_ms']>500):
                raise AssertionError((name,scenario,'unexpected reference freshness boundary'))
        if name in rolling_cases:
            record['fixture']={k:v.hex() if isinstance(v,bytes) else v for k,v in rolling_cases[name][scenario].items()}
            record['rolling_pool_sha256']=actual['rolling_pool_sha256']
            record['typing_state']=actual['typing_state']
            if 'return' in actual:record['return']=actual['return']
        records.append(record)
cli.output.parent.mkdir(parents=True, exist_ok=True)
cli.output.write_text(json.dumps({'format': 1, 'original_sha256': ORIGINAL_SHA256, 'recompiled_sha256': mapping['recompiled_sha256'], 'scenarios': records}, indent=2))
print(f'PASS: {len(records)} original/recompiled x86 scenarios, exact returns/state/arrays/dependency call order agree.')
