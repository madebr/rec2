"""Check x86 solver entry capture without emulating solver arithmetic.

The numerical verifier executes physical original and compiled programs. This
separate control-only check verifies the captured address and rejects a prologue
that overwrites the original private slot before reading it.
"""
import hashlib
import struct

from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE, UC_HOOK_MEM_WRITE, UC_HOOK_MEM_READ
from unicorn.x86_const import UC_X86_REG_EIP, UC_X86_REG_ESP, UC_X86_REG_EBP, UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX


def coff_function_prefix(path):
    """Read the actual function section from a COFF object or MinGW PE."""
    data = path.read_bytes()
    coff = struct.unpack_from('<I', data, 60)[0] + 4 if data[:2] == b'MZ' else 0
    machine, _, _, symbols, count, optional_size, _ = struct.unpack_from('<HHIIIHH', data, coff)
    assert machine == 0x14c
    strings = symbols + count * 18
    found = []
    index = 0
    while index < count:
        at = symbols + index * 18
        raw = data[at:at + 8]
        if raw[:4] == bytes(4):
            name = data[strings + struct.unpack_from('<I', raw, 4)[0]:].split(b'\0', 1)[0]
        else:
            name = raw.split(b'\0', 1)[0]
        value, section, _, _, auxiliaries = struct.unpack_from('<IhHBB', data, at + 8)
        if name in (b'@FUN_004c3b60@44', b'_FUN_004c3b60'):
            assert section > 0
            header = coff + 20 + optional_size + (section - 1) * 40
            length, raw_offset = struct.unpack_from('<II', data, header + 16)
            found.append((name.decode(), data[raw_offset + value:raw_offset + min(length, value + 512)]))
        index += 1 + auxiliaries
    assert len(found) == 1, (path, found)
    return found[0]


def audit_prefix(code, stack_offset):
    """Reject a wrong capture address, an earlier clobber, or an earlier call."""
    cs = Cs(CS_ARCH_X86, CS_MODE_32)
    base = 0x50000000
    captures = [ins for ins in cs.disasm(code, base) if ins.mnemonic == 'mov' and ins.op_str in (
        'eax, dword ptr [eax - 0x8cc]', 'eax, dword ptr [eax - 0x8c0]', 'eax, dword ptr [eax - 0x8c8]')]
    assert len(captures) == 1, [ins.op_str for ins in captures]
    capture = captures[0]
    entry_sp = 0x3000f000
    target = entry_sp - stack_offset
    stop = capture.address + capture.size
    uc = Uc(UC_ARCH_X86, UC_MODE_32)
    uc.mem_map(base, 4096)
    uc.mem_write(base, code)
    # Unresolved COFF security-cookie operands can read address zero. There are
    # no calls or numerical instructions in the accepted prefixes.
    uc.mem_map(0, 4096)
    uc.mem_map(0x30000000, 65536)
    uc.mem_write(0x30000000, b'\x55' * 65536)
    uc.mem_write(target, struct.pack('<I', 0x12345678))
    for register, value in ((UC_X86_REG_ESP, entry_sp), (UC_X86_REG_EBP, 0x13579bdf),
                            (UC_X86_REG_ECX, 0x1234), (UC_X86_REG_EDX, 0x5678)):
        uc.reg_write(register, value)
    trace, writes, reads = [], [], []

    def on_code(machine, address, size, _):
        ins = next(cs.disasm(bytes(machine.mem_read(address, size)), address))
        trace.append(dict(offset=address - base, bytes=ins.bytes.hex(), instruction=ins.mnemonic + ' ' + ins.op_str))
        assert ins.mnemonic != 'call', 'Call before capture requires review'

    def on_write(machine, access, address, size, value, _):
        writes.append(dict(address_relative_to_entry_esp=address - entry_sp, size=size, value=hex(value)))
        assert address + size <= target or address >= target + 4, 'Original slot clobbered before capture'

    def on_read(machine, access, address, size, value, _):
        if machine.reg_read(UC_X86_REG_EIP) == capture.address:
            reads.append(dict(address_relative_to_entry_esp=address - entry_sp, size=size))

    uc.hook_add(UC_HOOK_CODE, on_code)
    uc.hook_add(UC_HOOK_MEM_WRITE, on_write)
    uc.hook_add(UC_HOOK_MEM_READ, on_read)
    uc.emu_start(base, stop, count=100)
    assert uc.reg_read(UC_X86_REG_EIP) == stop
    assert reads == [dict(address_relative_to_entry_esp=-stack_offset, size=4)]
    assert uc.reg_read(UC_X86_REG_EAX) == 0x12345678
    return dict(entry_prefix_sha256=hashlib.sha256(code[:stop - base]).hexdigest(),
                capture_read=reads[0], prior_writes=writes, trace=trace, passed=True)


def audit_native(output_directory):
    rows = []
    for compiler, filename, offset in (('msvc5', 'msvc5.obj', 0x8c8),
                                       ('msvc', 'msvc.obj', 0x8c0),
                                       ('mingw', 'mingw.exe', 0x8c0)):
        path = output_directory / filename
        name, code = coff_function_prefix(path)
        rows.append(dict(compiler=compiler, path=str(path), symbol=name,
                         sha256=hashlib.sha256(path.read_bytes()).hexdigest(), **audit_prefix(code, offset)))
    return rows
