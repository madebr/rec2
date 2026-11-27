# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#
# Copyright 2022-2026 rec2 developers

"""isolate.py BUILD FILE FUNC ADDR [scan] : is a function's remaining diff caused by the code before it?

MSVC5's code for a function can depend on what was compiled earlier in the same file (operand order of
commutative operations, register choice). This compiles FILE with only FUNC kept - plus the includes,
the globals and the inline/static helpers FUNC needs - and scores it with fastmatch.py, next to the
score in the full file. If the isolated score is 1.0 (or the only hunks left are display differences),
the source is right and the function is order-sensitive: it will move again as the file fills up.

'scan' also scores FUNC with the first k functions of the file kept, for a spread of k, to show the
flips. The file is always restored. Needs the same environment as fastmatch.py (OBJDUMP, PATH).

PollCarControls (2026-11-11): 0.9539 in the full file, exact in isolation; flips ORIG/ours as dummy
functions are added before it."""
import io
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import rec2cfg  # noqa: E402


def top_level_items(s):
    """Split C source into top-level items: (text, kind, name). kind is 'func' for a function
    definition, 'other' otherwise. Line based, for clang-formatted code: a function ends at a line
    that is exactly '}' and starts at the last column-0 line before it that begins a declaration;
    the '//' annotation lines right above belong to it. (Counting braces fails on #if/#else bodies.)"""
    lines = s.split('\n')
    n = len(lines)
    funcs = []  # (first line, last line, name)
    prev_end = -1
    for e in range(n):
        if lines[e].rstrip() != '}':
            continue
        st = None
        for k in range(e - 1, prev_end, -1):
            l = lines[k]
            if l[:1] and (l[0].isalpha() or l[0] == '_') and not re.match(r'[A-Za-z_]\w*:\s*$', l):
                st = k
                break
        if st is None:
            continue
        head = ' '.join(lines[st:e])
        head = head[:head.index('{')] if '{' in head else head
        m = re.search(r'([A-Za-z_]\w*)\s*\([^()]*(?:\([^()]*\)[^()]*)*\)\s*$', head)
        if not m or re.match(r'(typedef|struct|union|enum)\b', head) or '=' in head.split('(')[0]:
            prev_end = e
            continue
        while st - 1 > prev_end and lines[st - 1].startswith('//'):
            st -= 1
        funcs.append((st, e, m.group(1)))
        prev_end = e
    items = []
    pos = 0
    for st, e, name in funcs:
        if st > pos:
            items.append(('\n'.join(lines[pos:st]) + '\n', 'other', ''))
        items.append(('\n'.join(lines[st:e + 1]) + '\n', 'func', name))
        pos = e + 1
    if pos < n:
        items.append(('\n'.join(lines[pos:]), 'other', ''))
    return items


def strip_comments(t):
    t = re.sub(r'/\*.*?\*/', ' ', t, flags=re.S)
    t = re.sub(r'//[^\n]*', ' ', t)
    return '\n'.join(l for l in t.split('\n') if not l.lstrip().startswith('#'))


def needed_helpers(items, func):
    """Names of file-local inline/static functions FUNC reaches (other functions have externs in headers)."""
    funcs = {name: text for text, kind, name in items if kind == 'func'}
    local = set(funcs)  # /Ob2 also inlines plain functions defined earlier in the file
    keep, todo = set(), [func]
    while todo:
        f = todo.pop()
        body = strip_comments(funcs.get(f, ''))
        for name in local:
            if name not in keep and name != func and re.search(r'\b%s\s*\(' % re.escape(name), body):
                keep.add(name)
                todo.append(name)
    return keep


def score(wk, rel, func, addr, env):
    o = subprocess.run([sys.executable, os.path.join(HERE, 'fastmatch.py'), wk, rel, func, addr], capture_output=True, text=True, env=env)
    l = [x for x in o.stdout.splitlines() if ' current ' in x]
    hunks = [x for x in o.stdout.splitlines() if x.startswith('  -') or x.startswith('  +')]
    return (l[-1] if l else 'ERR ' + (o.stdout + o.stderr)[-300:].replace('\n', ' | ')), hunks


def display_only(hunks):
    """True when every differing line is a known display difference of the text diff (not code)."""
    minus = [h[3:].strip() for h in hunks if h.startswith('  -')]
    plus = [h[3:].strip() for h in hunks if h.startswith('  +')]
    if len(minus) != len(plus):
        return False
    for a, b in zip(minus, plus):
        if a == 'ret' and b == 'ret 0':
            continue
        ma, mb = re.match(r'(\w+ \w+),(\d+)$', a), re.match(r'(\w+ \w+),(-\d+)$', b)
        if ma and mb and ma.group(1) == mb.group(1) and int(ma.group(2)) == int(mb.group(2)) + (1 << 32):
            continue
        if a.split(',')[0] == b.split(',')[0] and a.endswith(',G') and '??' in b:
            continue  # string literal symbol
        return False
    return True


def main():
    wk, rel, func, addr = sys.argv[1:5]
    scan = len(sys.argv) > 5 and sys.argv[5] == 'scan'
    cfg = rec2cfg.resolve(wk)
    p = os.path.join(cfg['repo'], *rel.split('/'))
    env = dict(os.environ, BLOCKS='0', SRCMAP='0', UDIFF='current')
    raw = io.open(p, encoding='utf-8', newline='').read()
    nl = '\r\n' if '\r\n' in raw else '\n'
    src = raw.replace('\r\n', '\n')
    items = top_level_items(src)
    names = [name for _, kind, name in items if kind == 'func']
    if func not in names:
        sys.exit('%s: no definition of %s found (%d functions parsed)' % (rel, func, len(names)))
    helpers = needed_helpers(items, func)
    fidx = [k for k, it in enumerate(items) if it[1] == 'func']
    tpos = next(k for k, it in enumerate(items) if it[1] == 'func' and it[2] == func)
    before = [k for k in fidx if k < tpos and items[k][2] not in helpers]

    def build(keep_before):
        out = []
        for k, (text, kind, name) in enumerate(items):
            if k > tpos:
                break
            if kind == 'func' and k != tpos and name not in helpers and k not in keep_before:
                continue
            out.append(text)
        return ''.join(out)

    try:
        full, _ = score(wk, rel, func, addr, env)
        print('full file      %s' % full[:120], flush=True)
        io.open(p, 'w', encoding='utf-8', newline='').write(build(set()).replace('\n', nl))
        iso, hunks = score(wk, rel, func, addr, env)
        tag = ''
        # only a real change counts: equal scores with display-only hunks are array-end aliases etc.
        if iso.startswith('1.0000') and not full.startswith('1.0000'):
            tag = '   <- exact in isolation: order-sensitive, source is right'
        print('isolated       %s%s' % (iso[:120], tag), flush=True)
        if helpers:
            print('  kept helpers: %s' % ' '.join(sorted(helpers)))
        if hunks and not tag:
            for h in hunks[:40]:
                print('   ' + h)
        if scan:
            ks = sorted({max(1, len(before) * q // 8) for q in range(1, 8)} | {len(before) - 1}) if len(before) > 1 else []
            for k in ks:
                io.open(p, 'w', encoding='utf-8', newline='').write(build(set(before[:k])).replace('\n', nl))
                r, _ = score(wk, rel, func, addr, env)
                print('first %3d of %3d functions kept: %s' % (k, len(before), r[:60]), flush=True)
    finally:
        io.open(p, 'w', encoding='utf-8', newline='').write(raw)


if __name__ == '__main__':
    main()
