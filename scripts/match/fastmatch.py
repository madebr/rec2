# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#
# Copyright 2022-2026 rec2 developers

"""fastmatch.py BUILD FILE FUNC ADDR [VARIANTS_PY] : search for a C form of one function quickly.

BUILD is the MSVC5 build directory (see rec2cfg.py for how the repository, the original
executable and objdump are found), FILE the source file relative to the repository, FUNC the
function and ADDR its address in the original.

For each variant in VARIANTS_PY (dict VARIANTS = {label: full function text}), writes it into FILE,
compiles only that file with MSVC5 (the command of the matching build from build.ninja, plus an
/FAs listing) and compares the function with the original executable at ADDR. Restores FILE
afterwards. Without VARIANTS_PY the function as it is in FILE is measured (label "current").
This takes seconds where a full build plus reccmp takes minutes; it is a search aid, and the
result that counts is still the one of the full reccmp run.

Per variant it prints one line:
    SCORE label  frame .. | n .. | calls .. | blk .. | N differing
  SCORE  similarity of the two instruction streams (1.000 = identical). With the reccmp scorer
         (default when BUILD holds a finished build, see objcmp.py) this is reccmp's own ratio
         computed from the freshly compiled .obj, and "eff" is appended when reccmp's
         effective-match test passes. Otherwise, or with SCORER=text, it is the ratio of the
         normalised listing text (addresses of globals and branch targets ignored).
  frame  `sub esp, N` of both sides;  n  instruction counts;  calls  multiset of call targets.
         A structure check that differs is marked with `!` and the line gets a leading `!`.
  blk    per-basic-block score: both sides are split at branch targets and after jumps/returns,
         every block is paired with the block of the other side that resembles it most (at any
         position), and the block ratios are averaged by block length. One shifted or moved
         region does not sink this number the way it sinks SCORE.
For the best variant (or DETAIL=label) it then prints the worst blocks and the source map:
"lines X-Y: N differing instructions", attributed through the line numbers of the /FAs listing
(line numbers are those of FILE with the variant written in), sorted by size. MSVC5 charges
the code of an inlined helper to the line of the call, so a helper's body cannot be narrowed
down this way; measure the helper on its own (MEASURE, or a non-inline copy) instead.

Environment:
  TOP=n          number of variants listed (default 15)
  DETAIL=label   variant whose worst blocks / source map are printed (default: the best)
  BLOCKS=n       worst blocks printed (default 5, 0 = none);  SRCMAP=n  source ranges (default 8)
  LINES=a:b      region mode: score only the instructions generated from source lines a..b
                 (file lines, inclusive) against the original instructions they align with.
                 The original span is printed; pass it back as ORANGE=i:j (original instruction
                 indices) to pin it, so that every variant of one statement is scored against
                 the same original instructions. Variants are then ranked by the region score.
  RANGE=a:b      compare only instruction indices a..b of both sides (crude; prefer LINES)
  UDIFF=label    print the differing hunks of that variant;  DIFF=label  side-by-side dump
  DUMP=a:b       print our instructions a..b of every variant;  SHOW=1  print the original
  MEASURE=Name:0xADDR   compare another function than the one rewritten
  STACK=1        keep stack offsets ([esp+N]) in the text comparison instead of normalising them
                 (needed to see the effect of reordering local declarations)
  SCORER=text|reccmp    force the scorer (see above)
  LEGACY=1       the scorer exactly as it was before the block/structure work (old normaliser,
                 difflib autojunk on, one number per variant)
"""
import collections
import difflib
import json
import os
import re
import struct
import subprocess
import sys
import tempfile

HERE_FM = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE_FM)
import rec2cfg  # noqa: E402

LEGACY = os.environ.get('LEGACY') == '1'
KEEP_STACK = os.environ.get('STACK') == '1'
if not LEGACY and os.environ.get('SCORER', '') != 'text':
    rec2cfg.ensure_reccmp_python()  # may restart this command under reccmp's python

wk, rel, func, addr = sys.argv[1:5]
vfile = sys.argv[5] if len(sys.argv) > 5 and sys.argv[5] not in ('-', 'unused') else None
CFG = rec2cfg.resolve(wk)
W, B, EXE, OBJDUMP = CFG['repo'], CFG['build'], CFG['exe'], CFG['objdump']
path = os.path.join(W, rel)
if not OBJDUMP:
    sys.exit('objdump not found: set OBJDUMP to an i686 objdump (binutils / MinGW)')

cmds = subprocess.run(['ninja', '-C', B, '-t', 'commands'], capture_output=True, text=True).stdout.splitlines()
src_win = os.path.normpath(path).lower()
cmd = next((c for c in cmds if 'cl.exe' in c.lower() and os.path.normpath(c.rstrip().rsplit('-c ', 1)[-1].strip('"')).lower() == src_win), None)
if cmd is None:
    sys.exit('no cl.exe command for %s in %s/build.ninja' % (rel, B))
env = rec2cfg.msvc_env(cmd)
out_dir = os.path.join(tempfile.gettempdir(), 'fastmatch-' + CFG['name'])
os.makedirs(out_dir, exist_ok=True)
asm = os.path.join(out_dir, 'f.asm')
obj = os.path.join(out_dir, 'f.obj')
cmd = re.sub(r'/Fo\S+', lambda _m: '/Fo' + obj + ' /FAs /Fa' + asm, cmd)


def norm_old(ins):
    ins = ins.lower().replace('dword ptr ', '').replace('qword ptr ', '').replace('word ptr ', '').replace('byte ptr ', '')
    ins = re.sub(r'\s+', ' ', ins.strip())
    ins = re.sub(r'ds:0x[0-9a-f]+', 'G', ins)
    ins = re.sub(r'\b_[a-z_]\w*(\$\d*)?', 'G', ins)  # global symbol in the listing
    ins = re.sub(r'\bG\+\d+', 'G', ins)
    ins = re.sub(r'\b(j\w+|call) .*', r'\1 X', ins)
    ins = re.sub(r'0x([0-9a-f]+)', lambda m: str(int(m.group(1), 16)), ins)
    ins = re.sub(r'\b([0-9a-f]+)h\b', lambda m: str(int(m.group(1), 16)), ins)
    ins = re.sub(r'(\d+)\[(\w+)\]', r'[\2+\1]', ins)  # 8[edx] -> [edx+8]
    ins = ins.replace(' + ', '+').replace(', ', ',').replace('st(0)', 'st')
    return ins


def norm_new(ins):
    ins = norm_old(ins)
    ins = ins.replace('offset flat:', '')
    ins = re.sub(r'\$[a-z]\d+', 'G', ins)
    ins = re.sub(r'-?\d+\+\[esp', '[esp', ins)
    if not KEEP_STACK:
        ins = re.sub(r'G?\[esp(\+[a-z0-9*]+)*?([+-]\d+)?\]', lambda m: '[esp' + ''.join(x for x in re.findall(r'\+[a-z]\w*(?:\*\d)?', m.group(0))) + '+S]', ins)
    ins = re.sub(r'^rep stos.*', 'rep stosd', ins)
    ins = re.sub(r'^rep movs.*', 'rep movsd', ins)
    ins = re.sub(r'(-?\d{6,})', lambda m: 'G' if 0x400000 <= int(m.group(1)) < 0x800000 else str(int(m.group(1)) & 0xffffffff), ins)
    m = re.match(r'^test (\w+),(\w+)$', ins)
    if m and m.group(1) > m.group(2):
        ins = 'test %s,%s' % (m.group(2), m.group(1))
    return ins


norm = norm_old if LEGACY else norm_new

_names = None


def call_names():
    """(address -> name, name -> address) of functions: the repository's annotations, the
    reccmp CSVs (decorated library names) and, when available, Ghidra's names."""
    global _names
    if _names is None:
        _names = rec2cfg.function_names(CFG)
    return _names


def _read_dwords(a, limit):
    """Up to `limit` dwords of the original executable at virtual address a."""
    d = open(EXE, 'rb').read()
    pe = struct.unpack_from('<I', d, 0x3c)[0]
    nsec, opt = struct.unpack_from('<H', d, pe + 6)[0], struct.unpack_from('<H', d, pe + 20)[0]
    base = struct.unpack_from('<I', d, pe + 24 + 28)[0]
    out = []
    for i in range(nsec):
        o = pe + 24 + opt + 40 * i
        vsize, va, rsize, rawp = struct.unpack_from('<IIII', d, o + 8)
        lo, hi = base + va, base + va + min(vsize or rsize, rsize)
        if lo <= a < hi:
            while a + 4 <= hi and len(out) < limit:
                out.append(struct.unpack_from('<I', d, rawp + a - lo)[0])
                a += 4
    return out


def original_ex():
    """The original function: dict(ins, starts, calls, addrs). `starts` are the instruction
    indices that begin a basic block; `calls` the call targets by name."""
    a = int(addr, 16)
    if CFG['ghidra']:
        starts = sorted(int(json.loads(l)['address'], 16) for l in open(CFG['ghidra'], encoding='utf-8'))
    else:
        starts = sorted(call_names()[0])
    end = next((s for s in starts if s > a), a + 0x1000)
    out = subprocess.run([OBJDUMP, '-d', '-M', 'intel', '--start-address=0x%x' % a, '--stop-address=0x%x' % end, EXE],
                         capture_output=True, text=True).stdout.splitlines()
    ins, addrs, raws = [], [], []
    limit = end
    for l in out:
        m = re.match(r'^\s+([0-9a-f]+):\s+(?:[0-9a-f]{2} )+\s*(.*)$', l)
        if not m:
            continue
        ia = int(m.group(1), 16)
        if not LEGACY and ia >= limit:
            break
        s = m.group(2).strip()
        if not s or s.startswith('nop') or s.startswith('(bad)'):
            if ins and ins[-1].startswith('ret'):
                break
            continue
        t = re.search(r'jmp\s+DWORD PTR \[\w+\*4\+0x([0-9a-f]+)\]', s)
        if t and a < int(t.group(1), 16) < limit:
            limit = int(t.group(1), 16)  # a jump table: the code ends before it
        ins.append(norm(s))
        addrs.append(ia)
        raws.append(s)
    # cut after the last ret before padding
    last = max(i for i, x in enumerate(ins) if x.startswith('ret'))
    ins, addrs, raws = ins[:last + 1], addrs[:last + 1], raws[:last + 1]
    a2n = call_names()[0] if not LEGACY else {}
    idx = {x: i for i, x in enumerate(addrs)}
    st, calls = {0}, []
    for i, s in enumerate(raws):
        mn = s.split()[0]
        if mn == 'call':
            m = re.match(r'call\s+0x([0-9a-f]+)', s)
            if m:
                calls.append(a2n.get(int(m.group(1), 16), '0x%x' % int(m.group(1), 16)))
            elif 'ds:0x' in s:
                calls.append('<import>')
            else:
                calls.append('<indirect>')
        elif mn.startswith('j') or mn.startswith('ret') or mn.startswith('loop'):
            st.add(i + 1)
            m = re.match(r'j\w+\s+0x([0-9a-f]+)', s)
            if m and int(m.group(1), 16) in idx:
                st.add(idx[int(m.group(1), 16)])
            t = re.search(r'jmp\s+DWORD PTR \[\w+\*4\+0x([0-9a-f]+)\]', s)
            if t:
                for v in _read_dwords(int(t.group(1), 16), 512):
                    if v not in idx:
                        break
                    st.add(idx[v])
    st.discard(len(ins))
    return {'ins': ins, 'starts': st, 'calls': calls, 'addrs': addrs}


def original():
    return original_ex()['ins']


def listing_ex(fname):
    """Our function from the /FAs listing: dict(ins, starts, calls, lines, src, key). `lines[i]` is
    the source line instruction i was generated from, `src` maps line -> source text."""
    lines = open(asm, encoding='latin-1').read().splitlines()
    start = next(i for i, l in enumerate(lines) if re.match(r'^(_%s|@%s@\d+|_%s@\d+)\s+PROC' % (fname, fname, fname), l))
    syms = {}
    for l in reversed(lines[max(0, start - 400):start]):
        m = re.match(r'^(\S+) = (-?\d+)', l)
        if m:
            syms[m.group(1)] = int(m.group(2))
        elif re.match(r'^\S+\s+ENDP', l):
            break
    a2n, n2a = call_names() if not LEGACY else ({}, {})

    def stack(m):
        v = syms.get(m.group(1))
        if v is None:
            return m.group(0)
        v += int(m.group(2) or 0)
        inner = m.group(3)
        k = re.search(r'([+-]\d+)$', inner)
        if k:
            v += int(k.group(1))
            inner = inner[:k.start()]
        return '[' + inner + ('+%d' % v if v > 0 else '%d' % v if v < 0 else '') + ']'

    ins, src_line, st, calls, src = [], [], {0}, [], {}
    keyl, ren = [], {}
    rn = lambda mm: ren.setdefault(mm.group(0), '$%d' % len(ren))
    cur = 0
    for l in lines[start + 1:]:
        if re.match(r'^\S+\s+ENDP', l):
            break
        m = re.match(r'^; (\d+)\s+: ?(.*)$', l)
        if m:
            cur = int(m.group(1))
            src[cur] = m.group(2)
            continue
        l = l.split(';')[0]
        if not l.strip():
            continue
        if re.match(r'^\$?\w+:\s*$', l.strip()) or l.strip().endswith(':'):
            st.add(len(ins))
            keyl.append(re.sub(r'\$[A-Za-z]+\d+', rn, l.strip()))
            continue
        if re.match(r'^\s', l):
            if re.match(r'^\s+(npad|dd|db)', l):
                continue
            s = l.strip()
            if not LEGACY:
                s = re.sub(r'([_$][\w$]*\$\w*|\$T\d+)([+-]\d+)?\[(esp[^\]]*)\]', stack, s)
            mn = s.split()[0]
            if mn == 'call':
                t = s.split(None, 1)[1].strip()
                if '__imp_' in t:
                    calls.append('<import>')
                elif '[' in t or re.match(r'^e[a-ds][xip]$', t):
                    calls.append('<indirect>')
                else:
                    plain = re.sub(r'^@(\w+)@\d+$', r'\1', t)
                    plain = plain if plain != t else re.sub(r'@\d+$', '', t[1:] if t.startswith('_') else t)
                    a = n2a.get(t, n2a.get(plain))
                    calls.append(a2n.get(a, plain) if a is not None else plain)
            elif mn.startswith('j') or mn.startswith('ret') or mn.startswith('loop'):
                st.add(len(ins) + 1)
            ins.append(norm(s))
            src_line.append(cur)
            keyl.append(re.sub(r'\$[A-Za-z]+\d+', rn, re.sub(r'\s+', ' ', s)))
    st.discard(len(ins))
    # `key` identifies the generated code exactly (stack offsets kept, labels renumbered)
    key = __import__('hashlib').sha1('\n'.join(keyl).encode()).hexdigest()
    return {'ins': ins, 'starts': st, 'calls': calls, 'lines': src_line, 'src': src, 'key': key}


def listing(fname):
    return listing_ex(fname)['ins']


def frame_of(ins):
    """Stack frame: the `sub esp, N` of the prologue (None when there is none)."""
    for i, x in enumerate(ins[:12]):
        m = re.match(r'^sub esp,(\d+)$', x)
        if m:
            return int(m.group(1))
        if x.startswith('call') and i and re.match(r'^mov eax,(\d+)$', ins[i - 1]):
            return int(ins[i - 1].split(',')[1])  # __chkstk
    return None


def structure(o, g):
    """The three structure checks. Returns (text, flagged)."""
    fo, fg = frame_of(o['ins']), frame_of(g['ins'])
    hx = lambda v: 'none' if v is None else '0x%x' % v
    parts, bad = [], False
    if fo == fg:
        parts.append('frame %s' % hx(fg))
    else:
        parts.append('!frame %s vs %s' % (hx(fg), hx(fo)))
        bad = True
    if len(o['ins']) == len(g['ins']):
        parts.append('n %d' % len(g['ins']))
    else:
        parts.append('!n %d vs %d' % (len(g['ins']), len(o['ins'])))
        bad = True
    co, cg = collections.Counter(o['calls']), collections.Counter(g['calls'])
    if co == cg:
        parts.append('calls ok(%d)' % len(o['calls']))
    else:
        fmt = lambda c: ','.join('%s%s' % (k, 'x%d' % v if v > 1 else '') for k, v in sorted(c.items()))
        parts.append('!calls orig-only[%s] ours-only[%s]' % (fmt(co - cg), fmt(cg - co)))
        bad = True
    return ' | '.join(parts), bad


def split_blocks(n, starts):
    b = sorted(s for s in starts if 0 <= s < n)
    if not b or b[0] != 0:
        b = [0] + b
    return [(b[i], b[i + 1] if i + 1 < len(b) else n) for i in range(len(b))]


def block_scores(tgt, tstarts, got, gstarts, sm=None):
    """Per-basic-block comparison. Every original block is paired with the block of ours that
    resembles it most, wherever it is, and the other way round. Returns (score, rows): score is
    the length-weighted mean of the best ratios over both sides, rows a list of
    (ratio, (i0, i1), (j0, j1) or None) for the original's blocks."""
    tb, gb = split_blocks(len(tgt), tstarts), split_blocks(len(got), gstarts)
    if sm is None:
        sm = difflib.SequenceMatcher(None, tgt, got, autojunk=False)
    o2g = {}
    for i, j, n in sm.get_matching_blocks():
        for k in range(n):
            o2g[i + k] = j + k
    gstart = {j0: (j0, j1) for j0, j1 in gb}
    rows, gbest, todo = [], {}, []
    for i0, i1 in tb:
        j0 = o2g.get(i0)
        if j0 is not None and gstart.get(j0, (0, -1))[1] - j0 == i1 - i0 and all(o2g.get(i0 + k) == j0 + k for k in range(i1 - i0)):
            rows.append((1.0, (i0, i1), gstart[j0]))
            gbest[gstart[j0]] = 1.0
        else:
            todo.append((i0, i1))
    cand = [b for b in gb if gbest.get(b) != 1.0]
    gkey = {b: tuple(got[b[0]:b[1]]) for b in cand}
    memo = {}
    for i0, i1 in todo:
        a = tuple(tgt[i0:i1])
        m = difflib.SequenceMatcher(None, autojunk=False)
        m.set_seq2(a)
        best, bj = 0.0, None
        for b in cand:
            k = (a, gkey[b])
            r = memo.get(k)
            if r is None:
                m.set_seq1(gkey[b])
                r = m.ratio() if m.quick_ratio() > 0 else 0.0
                memo[k] = r
            if r > best:
                best, bj = r, b
            if r > gbest.get(b, 0.0):
                gbest[b] = r
        rows.append((best, (i0, i1), bj))
    num = sum(r * (b[1] - b[0]) for r, b, _ in rows) + sum(gbest.get(b, 0.0) * (b[1] - b[0]) for b in gb)
    den = len(tgt) + len(got)
    return (num / den if den else 1.0), rows


def source_map(sm, g):
    """Attribute every differing hunk to the source lines of our instructions in it.
    Returns [(count, first_line, last_line)] sorted by count."""
    per = collections.Counter()
    lines = g['lines']
    if not lines:
        return []
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == 'equal':
            continue
        for j in range(j1, j2):
            per[lines[j]] += 1
        extra = (i2 - i1) - (j2 - j1)
        if extra > 0:  # original instructions we do not have: charge the line at that point
            per[lines[min(j2 - 1 if j2 > j1 else j1, len(lines) - 1)]] += extra
    out, run = [], None
    for ln in sorted(per):
        if run and ln - run[2] <= 1:
            run = (run[0] + per[ln], run[1], ln)
        else:
            if run:
                out.append(run)
            run = (per[ln], ln, ln)
    if run:
        out.append(run)
    return sorted(out, key=lambda x: -x[0])


def region(sm, tgt, g, lo, hi, orange=None):
    """Score only our instructions generated from source lines lo..hi. Returns
    (ratio, differing, (i0, i1) original span, number of our instructions)."""
    got = g['ins']
    sel = [j for j, ln in enumerate(g['lines']) if lo <= ln <= hi]
    if not sel:
        return 0.0, 0, orange or (0, 0), 0
    mine = [got[j] for j in sel]
    if orange is None:
        pairs = [(j + k, i + k) for i, j, n in sm.get_matching_blocks() for k in range(n)]
        before = [i for j, i in pairs if j < sel[0]]
        after = [i for j, i in pairs if j > sel[-1]]
        orange = ((before[-1] + 1) if before else 0, after[0] if after else len(tgt))
    theirs = tgt[orange[0]:orange[1]]
    m = difflib.SequenceMatcher(None, theirs, mine, autojunk=False)
    nd = sum(max(i2 - i1, j2 - j1) for tag, i1, i2, j1, j2 in m.get_opcodes() if tag != 'equal')
    if os.environ.get('UDIFF_REGION'):
        for tag, i1, i2, j1, j2 in m.get_opcodes():
            if tag != 'equal':
                print('@@ orig %d (line %s)' % (orange[0] + i1, g['lines'][sel[min(j1, len(sel) - 1)]]))
                for x in theirs[i1:i2]:
                    print('  - ' + x)
                for x in mine[j1:j2]:
                    print('  + ' + x)
    return m.ratio(), nd, orange, len(mine)


def compile_text(full_text, nl):
    """Write FILE and compile it; returns None, or the compiler's last message on failure."""
    open(path, 'w', encoding='utf-8', newline='').write(full_text.replace('\n', nl))
    for f in (asm, obj):
        if os.path.exists(f):
            os.remove(f)
    p = subprocess.run('cmd /c ' + cmd, shell=True, env=env, capture_output=True, text=True, cwd=B)
    if not os.path.exists(asm):
        return 'compile error: ' + ((p.stdout + p.stderr).strip().splitlines() or ['?'])[-1][:120]
    return None


_objcmp = None


def reccmp_scorer():
    """objcmp.Scorer for this build (reccmp's own comparison fed from the .obj), or None."""
    global _objcmp
    if _objcmp is None:
        _objcmp = False
        want = os.environ.get('SCORER', '')
        if not LEGACY and want != 'text' and os.path.exists(os.path.join(HERE_FM, 'objcmp.py')):
            try:
                import objcmp
                _objcmp = objcmp.Scorer(CFG)
            except Exception as e:  # no finished build in BUILD, reccmp missing, ...
                if want == 'reccmp':
                    raise
                print('(reccmp scorer not available: %s; using the text scorer)' % str(e)[:150])
    return _objcmp or None


def analyse(o, g, mname, maddr):
    """Everything fastmatch knows about one compiled variant. `o`/`g` from original_ex/listing_ex."""
    res = {}
    tgt, got = o['ins'], g['ins']
    sm = difflib.SequenceMatcher(None, tgt, got, autojunk=False)
    res['sm'] = sm
    res['text_ratio'] = sm.ratio()
    res['nd'] = sum(max(i2 - i1, j2 - j1) for tag, i1, i2, j1, j2 in sm.get_opcodes() if tag != 'equal')
    res['struct'], res['flag'] = structure(o, g)
    res['blk'], res['rows'] = block_scores(tgt, o['starts'], got, g['starts'], sm)
    res['ratio'], res['eff'], res['scorer'] = res['text_ratio'], False, 'text'
    sc = reccmp_scorer()
    if sc is not None:
        try:
            r = sc.score(obj, mname, int(maddr, 16))
            res['ratio'], res['eff'], res['scorer'] = r['ratio'], r['effective'], 'reccmp'
            res['rc'] = r
        except Exception as e:
            res['scorer'] = 'text (reccmp scorer failed: %s)' % str(e)[:80]
    return res


raw = open(path, encoding='utf-8', newline='').read()
nl = '\r\n' if '\r\n' in raw else '\n'
orig_text = raw.replace('\r\n', '\n')
m = re.search(r'^[^\n]*[ *]%s\([^\n]*\{\n.*?^\}\n' % re.escape(func), orig_text, re.M | re.S)
assert m, 'function not found in file'
ns = {'VARIANTS': {'current': m.group(0)}}
if vfile:
    exec(open(vfile, encoding='utf-8').read(), ns)
measure = os.environ.get('MEASURE')  # 'Name:0xADDR' to compare another function than the one rewritten
if measure:
    mname, addr = measure.split(':')
else:
    mname = func
E = os.environ.get
res = []
if LEGACY:
    target = original()
    try:
        for label, body in ns['VARIANTS'].items():
            err = compile_text(orig_text[:m.start()] + body.strip('\n') + '\n' + orig_text[m.end():], nl)
            if err:
                res.append((0.0, label, err))
                continue
            got = listing(mname)
            r = difflib.SequenceMatcher(None, target, got).ratio()
            res.append((r, label, '%d vs %d instructions' % (len(got), len(target))))
            if E('DIFF') == label:
                print(chr(10).join('%-28s | %s' % (a, b) for a, b in __import__('itertools').zip_longest(target, got, fillvalue='')))
    finally:
        open(path, 'w', encoding='utf-8', newline='').write(raw)
    res.sort(key=lambda x: -x[0])
    for r, label, info in res[:int(E('TOP', '15'))]:
        print('%.3f  %-50s %s' % (r, label, info))
    if E('SHOW'):
        print('\n'.join(target))
    sys.exit(0)

O = original_ex()
target = O['ins']
_r = E('RANGE')
_lines = [int(x) for x in re.split(r'[:-]', E('LINES'))] if E('LINES') else None
_orange = tuple(int(x) for x in E('ORANGE').split(':')) if E('ORANGE') else None
details = {}
try:
    for label, body in ns['VARIANTS'].items():
        err = compile_text(orig_text[:m.start()] + body.strip('\n') + '\n' + orig_text[m.end():], nl)
        if err:
            res.append((-1.0, label, err))
            continue
        G = listing_ex(mname)
        o = O
        if _r:
            _a, _b = map(int, _r.split(':'))
            cut = lambda d: dict(d, ins=d['ins'][_a:_b], starts={s - _a for s in d['starts'] if _a <= s < _b}, lines=d.get('lines', [])[_a:_b])
            o, G = cut(O), cut(G)
        A = analyse(o, G, mname, addr)
        got, tgt, sm = G['ins'], o['ins'], A['sm']
        if E('DUMP'):
            _a, _b = map(int, E('DUMP').split(':'))
            print('## ' + label + ': ' + ' ; '.join((got if _r else got[_a:_b])))
        if E('UDIFF') == label:
            for tag, i1, i2, j1, j2 in sm.get_opcodes():
                if tag == 'equal':
                    continue
                print('@@ orig %d ours %d (line %s)' % (i1, j1, G['lines'][min(j1, len(got) - 1)] if got else '?'))
                for x in tgt[i1:i2]:
                    print('  - ' + x)
                for x in got[j1:j2]:
                    print('  + ' + x)
        key = A['ratio']
        info = '%s | blk %.3f | %d differing' % (A['struct'], A['blk'], A['nd'])
        if A['scorer'] == 'reccmp':
            info += ' | text %.3f' % A['text_ratio'] + (' | eff' if A['eff'] else '')
        elif A['scorer'] != 'text':
            info += ' | ' + A['scorer']
        if _lines:
            rr, rnd, osp, n = region(sm, tgt, G, _lines[0], _lines[1], _orange)
            info = 'region %.3f (%d differing; ours %d ins, orig[%d:%d]) | whole %.4f | ' % (rr, rnd, n, osp[0], osp[1], A['ratio']) + info
            key = rr
        res.append((key, label, ('! ' if A['flag'] else '  ') + info))
        details[label] = (A, G, o)
        if E('DIFF') == label:
            print(chr(10).join('%-28s | %s' % (a, b) for a, b in __import__('itertools').zip_longest(tgt, got, fillvalue='')))
finally:
    open(path, 'w', encoding='utf-8', newline='').write(raw)
res.sort(key=lambda x: -x[0])
for r, label, info in res[:int(E('TOP', '15'))]:
    print('%.4f  %-32s %s' % (max(r, 0.0), label, info))
pick = E('DETAIL') or next((l for r, l, i in res if l in details), None)
if pick in details:
    A, G, o = details[pick]
    nb, nsrc = int(E('BLOCKS', '5')), int(E('SRCMAP', '8'))
    worst = sorted((x for x in A['rows'] if x[0] < 1.0), key=lambda x: (x[0] - 1.0) * (x[1][1] - x[1][0]))[:nb]
    if worst:
        print('worst blocks of %s (%d of %d original blocks differ; scorer: %s):' % (pick, sum(1 for x in A['rows'] if x[0] < 1.0), len(A['rows']), A['scorer']))
    for r, (i0, i1), bj in worst:
        if bj:
            ls = [x for x in G['lines'][bj[0]:bj[1]] if x]
            where = 'ours[%d:%d] lines %s-%s' % (bj[0], bj[1], min(ls) if ls else '?', max(ls) if ls else '?')
        else:
            where = 'no counterpart'
        print('  %.2f  orig[%d:%d] (%d ins)  best: %s' % (r, i0, i1, i1 - i0, where))
    smap = source_map(A['sm'], G)[:nsrc]
    if smap:
        print('differing instructions by source line of %s:' % pick)
    for n, a, b in smap:
        print('  lines %d-%d: %d differing instructions | %s' % (a, b, n, G['src'].get(a, '').strip()[:70]))
if E('SHOW'):
    print('\n'.join(target))
