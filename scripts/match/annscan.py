# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#
# Copyright 2022-2026 rec2 developers

"""annscan.py BUILD TARGET ADDR... [--add] : find references that miss an annotation for TARGET.

When a function, stub or global has a `// FUNCTION:`/`// STUB:`/`// GLOBAL:` line for CARMA2_HW but not
for TARGET (CARMA2_SW, CARMA2_CHEAT), reccmp cannot name it on the original side of that target: the
diff shows `call <OFFSETn>` or `[<OFFSETn>]` against `Name (FUNCTION)` / `Name (DATA)` on ours, and every
such line counts as a mismatch. For each ADDR (a function's address in TARGET) this runs
`reccmp-reccmp --target TARGET --verbose ADDR` in BUILD, pairs each unresolved operand with our name and
reads the original address from the instruction bytes.

With --add, a missing line is inserted for every name that is paired with one address by more than half of
its unresolved operands (a diff line can pair the wrong instructions): next to the
name's existing annotation lines (in HW, SW, CHEAT order) in src/**/*.c, excluding src/carma2/common/,
keeping the file's line endings. Rebuild and check the result: a pair can be wrong when one instruction
references two entities.

reccmp-reccmp is taken from $RECCMP (a path) or PATH; capstone comes with reccmp. String literals are not
annotated in this repository, so `push <OFFSETn>` against `"text" (STRING)` is printed but never added.
Example: InitSmokeStuff on CARMA2_CHEAT went from 70.93% to 92.80% with four GLOBAL lines and no code change.
"""
import glob
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import rec2cfg  # noqa: E402
import targetaddr  # noqa: E402

ORDER = ['CARMA2_HW', 'CARMA2_SW', 'CARMA2_CHEAT']
KINDS = 'DATA|FUNCTION|STRING|POINTER|FLOAT|VTABLE'


def operands(img, va):
    import capstone
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    ins = next(md.disasm(targetaddr.rd(img, va, 16), va))
    lo, hi = targetaddr.image_range(img)
    vals = []
    for op in ins.operands:
        if op.type == capstone.x86.X86_OP_MEM and lo <= (op.mem.disp & 0xffffffff) < hi:
            vals.append(op.mem.disp & 0xffffffff)
        if op.type == capstone.x86.X86_OP_IMM and lo <= (op.imm & 0xffffffff) < hi:
            vals.append(op.imm & 0xffffffff)
    return ins, vals


def scan(build, target, img, addr):
    rc = os.environ.get('RECCMP') or shutil.which('reccmp-reccmp')
    if not rc:
        sys.exit('reccmp-reccmp not found: put it on PATH or set RECCMP')
    env = dict(os.environ, PYTHONIOENCODING='utf-8')
    out = subprocess.run([rc, '--target', target, '--verbose', addr, '--no-color'], cwd=build, capture_output=True,
                         text=True, encoding='utf-8', errors='replace', env=env).stdout
    status = [l for l in out.splitlines() if 'similar' in l or '% match' in l]
    print('##', addr, status[-1].strip() if status else '?')
    lines = out.splitlines()
    seen, pairs = set(), []
    for i, l in enumerate(lines):
        m = re.match(r'\s*(0x[0-9a-f]+) : -(.*<OFFSET\d+>.*)', l)
        if not m:
            continue
        plus = ''
        for l2 in lines[i + 1:i + 6]:
            m2 = re.match(r'\s*(?:0x[0-9a-f]+)?\s*: \+(.*)', l2)
            if m2:
                plus = m2.group(1)
                break
        ins, vals = operands(img, int(m.group(1), 16))
        names = re.findall(r'([A-Za-z_]\w*)(?:\+0x[0-9a-f]+)? \((?:%s)[^)]*\)' % KINDS, plus)
        if len(names) == 1 and len(vals) == 1 and '(STRING)' not in plus:
            pairs.append((names[0], vals[0]))
        key = (tuple(names), tuple(vals))
        if key in seen:
            continue
        seen.add(key)
        print('  %s  orig: %s %s | ours: %s' % (m.group(1), ins.mnemonic, ins.op_str, plus.strip()))
    return pairs


def vote(pairs):
    """name -> address that most of its pairs agree on. A diff line can pair the wrong instructions
    (one instruction more or less in between), so a name is only kept when more than half of its
    pairs give the same address."""
    import collections
    by = collections.defaultdict(collections.Counter)
    for name, addr in pairs:
        by[name][addr] += 1
    out = {}
    for name, c in by.items():
        addr, n = c.most_common(1)[0]
        if 2 * n > sum(c.values()):
            out[name] = addr
        else:
            print('    skip %s: pairs disagree %s' % (name, ', '.join('0x%x x%d' % kv for kv in c.items())))
    return out


def add(repo, name, target, addr):
    defre = re.compile(r'^[A-Za-z_].*[\s*]%s\s*(\(|\[|;|=)' % re.escape(name))
    hits = []
    for p in glob.glob(os.path.join(repo, 'src', '**', '*.c'), recursive=True):
        if os.sep + 'common' + os.sep in os.path.normpath(p):
            continue
        s = open(p, 'rb').read().decode('latin-1')
        nl = '\r\n' if '\r\n' in s else '\n'
        lines = s.replace('\r\n', '\n').split('\n')
        for i, l in enumerate(lines):
            if defre.match(l) and not l.startswith('extern'):
                j = i
                while j > 0 and re.match(r'// (FUNCTION|STUB|GLOBAL|LIBRARY): ', lines[j - 1]):
                    j -= 1
                if j < i:
                    hits.append((p, i, j, lines, nl))
    if len(hits) != 1:
        print('    skip %s: %d annotated definitions' % (name, len(hits)))
        return
    p, i, j, lines, nl = hits[0]
    block = lines[j:i]
    if any(len(l.split()) > 2 and l.split()[2] == target for l in block):
        print('    %s already has a %s line' % (name, target))
        return
    kind = block[0].split(':')[0][3:]
    k = j
    for idx, l in enumerate(block):
        t = l.split()[2] if len(l.split()) > 2 else ''
        if t in ORDER and ORDER.index(t) < ORDER.index(target):
            k = j + idx + 1
    lines.insert(k, '// %s: %s 0x%08x' % (kind, target, addr))
    open(p, 'wb').write(nl.join(lines).encode('latin-1'))
    print('    added %s %s 0x%08x in %s' % (kind, name, addr, os.path.relpath(p, repo)))


def main():
    args = [a for a in sys.argv[1:] if a != '--add']
    if len(args) < 3:
        sys.exit(__doc__)
    cfg = rec2cfg.resolve(args[0])
    target = args[1]
    img = targetaddr.load(targetaddr.target_exe(cfg['repo'], target))
    pairs = []
    for a in args[2:]:
        pairs += scan(cfg['build'], target, img, a)
    if '--add' in sys.argv:
        for name, addr in vote(pairs).items():
            add(cfg['repo'], name, target, addr)


if __name__ == '__main__':
    main()
