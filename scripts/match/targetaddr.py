# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#
# Copyright 2022-2026 rec2 developers

"""targetaddr.py BUILD TARGET HWADDR... : find a CARMA2_HW function in another target's executable by its bytes.

TARGET is a target id of reccmp-user.yml (CARMA2_SW, CARMA2_CHEAT). Both executables are taken from
`targets: <id>: path:` in <repo>/reccmp-user.yml (the repository is found from BUILD as in rec2cfg.py).

The start of the HW function is disassembled and every byte that depends on where code or data was linked
(absolute addresses in immediates and displacements, rel32 call/jmp/jcc targets) becomes a wildcard. The
pattern grows instruction by instruction until exactly one place in the target's first section matches.
Without capstone it falls back to plain byte prefixes, which fail when the first instructions reference
globals. A hit found with fewer than 8 instructions is reported as 'weak': check it before using it.

Prints: HW address, instructions used (or 'raw' bytes), and the hit.
Also importable: load(path), rd(img, addr, n), find(hw_img, target_img, addr), target_exe(repo, target).
"""
import os
import re
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import rec2cfg  # noqa: E402


def load(p):
    d = open(p, 'rb').read()
    pe = struct.unpack_from('<I', d, 0x3c)[0]
    nsec = struct.unpack_from('<H', d, pe + 6)[0]
    opt = struct.unpack_from('<H', d, pe + 20)[0]
    base = struct.unpack_from('<I', d, pe + 52)[0]
    secs = []
    o = pe + 24 + opt
    for _ in range(nsec):
        vs, va, rs, rp = struct.unpack_from('<IIII', d, o + 8)
        secs.append((base + va, max(vs, rs), rp, rs))
        o += 40
    return d, secs


def rd(img, a, n):
    d, secs = img
    for va, vs, rp, rs in secs:
        if va <= a < va + vs:
            return d[rp + a - va: rp + a - va + n]
    return b''


def _text(img):
    d, secs = img
    va, vs, rp, rs = secs[0]
    return va, d[rp:rp + rs]


def image_range(img):
    d, secs = img
    return secs[0][0], secs[-1][0] + secs[-1][1]


def _masked(hw_img, addr, max_ins=40):
    """List of (bytes, mask) per instruction; mask True = byte must match."""
    from capstone import Cs, CS_ARCH_X86, CS_MODE_32
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    md.detail = True
    lo, hi = image_range(hw_img)
    out = []
    for ins in md.disasm(rd(hw_img, addr, max_ins * 15), addr):
        b = bytes(ins.bytes)
        keep = [True] * len(b)

        def wipe(off, size):
            for k in range(off, min(off + size, len(b))):
                keep[k] = False
        if ins.imm_size and ins.imm_offset:
            rel = ins.mnemonic == 'call' or ins.mnemonic.startswith('j')
            val = int.from_bytes(b[ins.imm_offset:ins.imm_offset + ins.imm_size], 'little')
            if (rel and ins.imm_size == 4) or (ins.imm_size == 4 and lo <= val < hi):
                wipe(ins.imm_offset, ins.imm_size)
        if ins.disp_size == 4 and ins.disp_offset:
            val = int.from_bytes(b[ins.disp_offset:ins.disp_offset + 4], 'little')
            if lo <= val < hi:
                wipe(ins.disp_offset, 4)
        out.append((b, keep))
        if ins.mnemonic == 'ret':
            break
    return out


def find(hw_img, tgt_img, addr):
    """(address or None, instructions used / 'rawN' / 'N weak')."""
    va, text = _text(tgt_img)
    try:
        ins = _masked(hw_img, addr)
    except ImportError:
        pat = rd(hw_img, addr, 32)
        for n in (12, 16, 24, 32):
            hits = [m.start() for m in re.finditer(re.escape(pat[:n]), text)][:3]
            if len(hits) == 1:
                return va + hits[0], 'raw%d' % n
            if not hits:
                break
        return None, 'raw'
    best = None
    for n in range(4, len(ins) + 1):
        rx = b''.join(re.escape(bytes([c])) if k else b'.' for b, keep in ins[:n] for c, k in zip(b, keep))
        hits = [m.start() for m in re.finditer(rx, text, re.S)][:3]
        if not hits:
            break
        if len(hits) == 1:
            best = (va + hits[0], n)
            if n >= 8 or n == len(ins):
                return best
    if best:
        return best[0], '%d weak' % best[1]
    return None, 0


def target_exe(repo, target):
    p = rec2cfg._yaml_value(os.path.join(repo, 'reccmp-user.yml'), target, 'path')
    if not p or not os.path.exists(p):
        sys.exit('no executable for %s in %s' % (target, os.path.join(repo, 'reccmp-user.yml')))
    return p


def main():
    if len(sys.argv) < 4:
        sys.exit(__doc__)
    cfg = rec2cfg.resolve(sys.argv[1])
    hw = load(target_exe(cfg['repo'], 'CARMA2_HW'))
    tgt = load(target_exe(cfg['repo'], sys.argv[2]))
    for a in sys.argv[3:]:
        a = int(a, 16)
        hit, n = find(hw, tgt, a)
        print(hex(a), n, hex(hit) if hit else 'not found')


if __name__ == '__main__':
    main()
