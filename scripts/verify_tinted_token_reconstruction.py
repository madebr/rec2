#!/usr/bin/env python3
"""Verify aliased tinted material streams through the actual x86 token consumer.

Requires Python 3.10+, unicorn==2.1.4, and the sibling import-safe tinted startup
verifier. Supply a fresh symbol map exported by verify_input_reconstruction.py.
Original/rebuilt CreateTintedPoly bodies run with the shared startup fixtures.
Each material update and each subsequent alias read runs actual templateMakeMap,
BrTokenValueSetMany and ValueSet instructions against all 22 entries copied from
the hash-verified original D3D primitive template. Only BrResAllocate is stubbed
in that consumer. This is not a complete renderer/material integration test.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

import verify_tinted_startup_reconstruction as startup

ROOT = Path(__file__).resolve().parents[1]
D3D_HASH = '5722433d8b66f9b613a399618e48ff27256eb396cce8b9fa2a3032b91c32b55c'
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--original',type=Path,default=ROOT/'build-reference/originals/CARMA2_HW.EXE')
parser.add_argument('--recompiled',type=Path,default=ROOT/'build-matching/rec2.exe')
parser.add_argument('--symbols',type=Path,required=True)
parser.add_argument('--primitive-original',type=Path,default=ROOT/'build-reference/originals/d3d.bdd')
parser.add_argument('--dependency-dir',type=Path,default=ROOT/'build-audit/joystick/python-deps')
parser.add_argument('--output',type=Path,default=ROOT/'build-audit/tinted-startup/token-results.json')


def pe_sections(data):
    pe=struct.unpack_from('<I',data,0x3c)[0]
    base=struct.unpack_from('<I',data,pe+52)[0]
    count=struct.unpack_from('<H',data,pe+6)[0]
    table=pe+24+struct.unpack_from('<H',data,pe+20)[0]
    return base,[struct.unpack_from('<IIII',data,table+40*i+8) for i in range(count)]


def read_pe(data,address,size):
    base,sections=pe_sections(data)
    for virtual,rva,raw_size,raw in sections:
        if base+rva<=address and address+size<=base+rva+raw_size:
            return data[raw+address-base-rva:raw+address-base-rva+size]
    raise AssertionError(('Address outside raw PE sections',hex(address),size))


class Consumer:
    def __init__(self,side,primitive_entries):
        from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE,UC_HOOK_MEM_READ
        from unicorn.x86_const import UC_X86_REG_FPCW
        self.side=side
        self.u=Uc(UC_ARCH_X86,UC_MODE_32)
        data=startup.original if side=='orig' else startup.recompiled
        base,sections=pe_sections(data)
        self.u.mem_map(base,0x500000)
        self.u.mem_map(startup.HEAP,0x220000)
        for virtual,rva,raw_size,raw in sections:
            self.u.mem_write(base+rva,data[raw:raw+raw_size])
        self.u.reg_write(UC_X86_REG_FPCW,0x37f)
        self.base=self.addr('gTinted_poly_tokens')
        self.template=startup.HEAP
        self.entries=startup.HEAP+0x100
        self.block=startup.HEAP+0x1000
        self.counter=startup.HEAP+0x1100
        self.mask=self.counter+4
        self.map=startup.HEAP+0x2000
        self.write(self.template,22)
        self.write(self.template+4,self.entries)
        self.write(self.template+24,1) # All original primitive entries have null names.
        self.write(self.template+28,self.template)
        self.u.mem_write(self.entries,primitive_entries)
        self.trace=[];self.scanned=[];self.allocation=[]
        self.stream_size=0
        self.u.hook_add(UC_HOOK_CODE,self.hook)
        self.u.hook_add(UC_HOOK_MEM_READ,self.read_hook)

    def addr(self,name):return startup.symbols[name][self.side]
    def read(self,address):return struct.unpack('<I',self.u.mem_read(address,4))[0]
    def write(self,address,value):self.u.mem_write(address,struct.pack('<I',value&0xffffffff))

    def hook(self,uc,address,size,context):
        sp=uc.reg_read(startup.UC_X86_REG_ESP)
        if address==self.addr('BrResAllocate'):
            values=[self.read(sp+4*i) for i in (1,2,3)]
            if values[:2]!=[self.template,528]:
                raise AssertionError(('Template map allocation',values))
            self.allocation.append(values)
            uc.reg_write(startup.UC_X86_REG_EAX,self.map)
            uc.reg_write(startup.UC_X86_REG_EIP,self.read(sp))
            uc.reg_write(startup.UC_X86_REG_ESP,sp+4)
        elif address==self.addr('ValueSet'):
            block,tv,entry=[self.read(sp+4*i) for i in (1,2,3)]
            if block!=self.block:raise AssertionError('Unexpected primitive state destination')
            self.trace.append(dict(index=(tv-self.base)//8,token=self.read(tv),value=self.read(tv+4),
                                   entry=(entry-self.entries)//24,flags_before=self.read(block+20),
                                   alpha_bits_before=self.read(block+60)))

    def read_hook(self,uc,access,address,size,value,context):
        offset=address-self.base
        if size==4 and 0<=offset<self.stream_size and offset%8==0:
            self.scanned.append(dict(index=offset//8,token=self.read(address)))

    def consume(self,stream,offset,flags=0xa5a5a5a5):
        self.u.mem_write(self.base,stream)
        self.stream_size=len(stream)
        self.trace=[];self.scanned=[]
        self.u.mem_write(self.block,b'\xa5'*0x100)
        self.write(self.block+20,flags)
        self.write(self.counter,0x12345678);self.write(self.mask,0x4000)
        values=[self.block,self.counter,self.mask,self.base+offset,self.template]
        self.write(startup.STACK,startup.STOP)
        for i,value in enumerate(values):self.write(startup.STACK+4+4*i,value)
        self.u.reg_write(startup.UC_X86_REG_ESP,startup.STACK)
        self.u.emu_start(self.addr('BrTokenValueSetMany'),startup.STOP,count=100000)
        if self.u.reg_read(startup.UC_X86_REG_EIP)!=startup.STOP:
            raise AssertionError('Token consumer instruction budget exhausted')
        result=dict(state=bytes(self.u.mem_read(self.block,0x100)).hex(),count=self.read(self.counter),
                    mask=self.read(self.mask),return_value=self.u.reg_read(startup.UC_X86_REG_EAX),
                    accepted=self.trace.copy())
        expected=[i for i in range(offset//8,len(stream)//8) if struct.unpack_from('<I',stream,8*i)[0]==0]
        if not expected or not any(s['index']==expected[0] and s['token']==0 for s in self.scanned):
            raise AssertionError('Consumer did not reach the actual stream sentinel')
        return result,self.scanned.copy()


class MaterialFixture(startup.Fixture):
    def __init__(self,data,side,case,primitive_entries):
        self.events=[]
        self.consumer=Consumer(side,primitive_entries)
        super().__init__(data,side,case)

    def stream(self):
        # Original scans through eight unknown string tokens; rebuilt terminates at8.
        return bytes(self.u.mem_read(self.addr('gTinted_poly_tokens'),136 if self.side=='orig' else 72))

    def consume_material(self,material,phase):
        extra=self.read(material+0x58)
        offset=extra-self.addr('gTinted_poly_tokens')
        if offset not in (0,16,32):return None
        result,scanned=self.consumer.consume(self.stream(),offset)
        event=dict(phase=phase,material=material,offset=offset,consumer=result)
        self.events.append(event)
        return scanned

    def hook(self,uc,address,size,context):
        if self.hooks.get(address)=='BrMaterialUpdate':
            sp=uc.reg_read(startup.UC_X86_REG_ESP)
            material,flags=self.read(sp+4),self.read(sp+8)
            if flags&0x40:self.consume_material(material,'update')
        super().hook(uc,address,size,context)


def main():
    args=parser.parse_args()
    startup.configure(['--original',str(args.original),'--recompiled',str(args.recompiled),
                       '--symbols',str(args.symbols),'--dependency-dir',str(args.dependency_dir)])
    primitive=args.primitive_original.read_bytes()
    if hashlib.sha256(primitive).hexdigest()!=D3D_HASH:parser.error('Incorrect original D3D binary hash')
    primitive_entries=read_pe(primitive,0x100172a0,22*24)
    entries=[struct.unpack_from('<IIIHHII',primitive_entries,24*i) for i in range(22)]
    relevant={e[0]:e for e in entries if e[0] in (128,133,190)}
    if relevant!={128:(128,0,20,7,31,4,1),133:(133,0,20,7,31,128,1),190:(190,0,60,7,14,0,0)}:
        raise AssertionError('Original D3D primitive template entries differ')
    records=[]
    sequences=[(2,5,0),(0,2,5),(5,0,3),(2,0,2,1),(6,5,2),(0,1,0)]
    for classes in sequences:
        for opacity in (0,1,128,255,-1):
            outcomes=[]
            for side,data in (('orig',startup.original),('recomp',startup.recompiled)):
                case=dict(mode='create',slot=0,opacity=opacity)
                fixture=MaterialFixture(data,side,case,primitive_entries)
                for i in range(len(classes)):fixture.write(fixture.pool+i*startup.STRIDE+0x30,0)
                materials=[];checks=[]
                for i,cls in enumerate(classes):
                    values=(0,0,32,64,cls,0 if cls==6 else 17,64+i*23,81)
                    index=fixture.invoke('CreateTintedPoly',values)
                    if fixture.status!='returned' or index!=i:raise AssertionError(('Creation failed',classes,side,index,fixture.status))
                    materials.append(fixture.read(fixture.pool+i*startup.STRIDE+4))
                    for previous,material in enumerate(materials):
                        scanned=fixture.consume_material(material,f'after_{i}_material_{previous}')
                        if scanned is not None:checks.append(scanned)
                if len(fixture.consumer.allocation)!=1:raise AssertionError('Actual templateMakeMap did not initialize exactly once')
                outcomes.append(fixture.events)
            if outcomes[0]!=outcomes[1]:raise AssertionError(('Original/rebuilt primitive state differs',classes,opacity,outcomes))
            records.append(dict(classes=classes,opacity=opacity,pass_result=True,events=outcomes[1]))

    # Independently exercise every aliased stream and shared mutation, retaining
    # original trailing bytes and rebuilt bounds. Also test the token6/default
    # opacity alias: its first decal value can be false while token7 remains true.
    for opacity in (0,1,128,255,32768,-1):
        for last in (0,1,17,255,-1,32768):
            results=[];scans=[]
            for side in ('orig','recomp'):
                consumer=Consumer(side,primitive_entries)
                size=136 if side=='orig' else 72
                stream=bytearray(consumer.u.mem_read(consumer.base,size))
                struct.pack_into('<I',stream,12,(opacity<<16)&0xffffffff)
                struct.pack_into('<I',stream,28,0)
                struct.pack_into('<I',stream,44,(last<<16)&0xffffffff)
                struct.pack_into('<I',stream,52,opacity&0xffffffff)
                values=[];side_scans=[]
                for offset in (0,16,32):
                    value,scanned=consumer.consume(bytes(stream),offset)
                    values.append(value);side_scans.append(scanned)
                    if value['count']!=8-offset//8 or value['mask']!=0x4001 or value['return_value']!=0:
                        raise AssertionError(('Primitive consumer count/mask',value))
                    if struct.unpack_from('<f',bytes.fromhex(value['state']),60)[0]!=float(struct.unpack('<i',struct.pack('<I',(last<<16)&0xffffffff))[0])/65536:
                        raise AssertionError('Last shared opacity did not override earlier values')
                    if struct.unpack_from('<I',bytes.fromhex(value['state']),20)[0]&0x84 != 0x84:
                        raise AssertionError('Both blend and final decal must be enabled')
                    if bool(value['accepted'][-1]['flags_before']&4)!=bool(opacity):
                        raise AssertionError('The first decal token did not affect primitive flags before the second')
                results.append(values);scans.append(side_scans)
            if results[0]!=results[1]:raise AssertionError(('Aliased token consumers differ',opacity,last,results))
            for original_scan in scans[0]:
                if not all(any(s['index']==i for s in original_scan) for i in range(8,17)):
                    raise AssertionError('Original unterminated stream tail was not consumed')
            records.append(dict(alias_mutation=[opacity,last],pass_result=True,consumers=results[1],original_scanned=scans[0],recompiled_scanned=scans[1]))
    args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_text(json.dumps(dict(original_sha256=startup.ORIGINAL_HASH,
        recompiled_sha256=startup.mapping['recompiled_sha256'],primitive_original_sha256=D3D_HASH,
        primitive_template_address='0x100172a0',primitive_template_sha256=hashlib.sha256(primitive_entries).hexdigest(),
        scenarios=records),indent=2)+'\n')
    print(f'PASS: {len(records)} sequential/aliased token scenarios agree through actual x86 consumers.')


if __name__=='__main__':main()
