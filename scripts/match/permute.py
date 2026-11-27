# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#
# Copyright 2022-2026 rec2 developers

"""permute.py BUILD FILE FUNC ADDR [COMPILES] : search behaviour-preserving rewrites of one function.

Simulated annealing over small source mutations of FUNC in FILE (arguments as for fastmatch.py,
whose compile set-up and scoring it reuses). Each step applies one random mutation, compiles
FILE with MSVC5 (about 1-3 seconds) and scores the function against the original at ADDR. A
better or equal candidate is always taken, a worse one with probability exp(-delta/T) while
the temperature T falls from T0 to T1, so the search can cross a worse intermediate form
(two changes that only help together). The best version seen is kept.

Cost (lower is better, in instructions):
    0.5 * (1 - SCORE) * size + 0.5 * (1 - blk) * size + 4 if the frame differs + 2 per call
    that is missing or extra
  SCORE and blk are fastmatch's whole-function and per-basic-block scores, size the mean
  instruction count of both sides. With LINES=a:b the cost is the number of differing
  instructions among those generated from source lines a..b (fastmatch's region mode; the
  original span is pinned at the start), plus a small weight on the whole function so the
  rest is not made worse, and only mutations inside those lines are tried.

Nothing is compiled or counted twice: a source text seen before is not compiled again
(the cache is kept in FUNC.cache.json between runs), and two sources that give the
same code (hash of the function's listing with labels renumbered) share one score.

Every mutation keeps the function's behaviour; the guards are in the code of each kind:
  cmp      swap the operands of a comparison (a < b  <->  b > a)
  compound x += e  <->  x = x + e / x = e + x  (only where precedence allows it)
  paren    parenthesise a product in front of / + -
  commute  swap the operands of a commutative + or * (any operand expression without side
           effects or calls; parentheses are added when the regrouping would otherwise change)
  literal  float <-> double literal, only for values a float holds exactly (0.5, 2.0, ...)
  half     x / 2.0f  <->  x * 0.5f
  cast     (float)/(br_scalar) cast: drop it from, or move it between, the whole expression and
           its first operand; only when the expression holds an f-suffixed literal, no call,
           and no local declared double (struct fields are assumed not to be double)
  decl     move one local declaration to another place in the declaration block
  split    give the later, independent use of a reused local its own variable
  merge    reuse an earlier local of the same type for a later one (inverse of split)
  order    swap two adjacent statements that cannot affect each other (assignments to distinct
           non-address-taken locals, or to distinct fields/constant indices of the same base,
           with right-hand sides that read only locals and literals)
  ternary  if/else assignment  <->  ?:
  not      !x  <->  x == 0 in a condition
  libm     sin <-> sinf etc. NOT value-preserving in general, off unless ALLOW=libm.
Functions that contain `goto` are not given split/merge/order mutations.

Leaves FILE with the best version found (KEEP=0 restores it) and writes FUNC.best.c,
FUNC.orig.c, FUNC.log and the cache to $PERMUTE_DIR (default: permute-<build> in the temp
directory; the start line of the log names it). The result is a
candidate: build and check it with the full reccmp run before believing it, and read the diff
(modern MSVC may want a cast back that a mutation removed).

Environment: COMPILES (or the 5th argument) compile budget, default 300; T0/T1 temperatures
(default 2.0 / 0.2); SEED; KINDS=a,b only these mutation kinds; SKIP=a,b not these;
LINES=a:b region; ALLOW=libm; STACK=1 (see fastmatch.py; recommended with KINDS=decl and the
text scorer); SCORER as in fastmatch.py. `permute.py --list BUILD FILE FUNC ADDR` only prints
the number of mutations per kind.
"""
import hashlib
import json
import math
import os
import random
import re
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ID = r'[A-Za-z_][A-Za-z0-9_]*'
OPERAND = r'(?:[-!&*]?%s(?:(?:->|\.)%s|\[[^\[\]]*\])*|[-]?\d[\w.]*)' % (ID, ID)
PREC = {'*': 13, '/': 13, '%': 13, '+': 12, '-': 12, '<<': 11, '>>': 11, '<': 10, '<=': 10, '>': 10, '>=': 10,
        '==': 9, '!=': 9, '&': 8, '^': 7, '|': 6, '&&': 5, '||': 4, '?': 3, ':': 3,
        '=': 2, '+=': 2, '-=': 2, '*=': 2, '/=': 2, '%=': 2, '&=': 2, '|=': 2, '^=': 2, '<<=': 2, '>>=': 2, ',': 1}
TOKEN = re.compile(r'\s+|//[^\n]*|/\*.*?\*/|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|(?:\d[\w.]*(?:[eE][+-]?\d+)?[fFuUlL]*)|%s|->|\+\+|--|<<=|>>=|<<|>>|<=|>=|==|!=|&&|\|\||[-+*/%%&|^]=|.' % ID, re.S)
TYPEISH = re.compile(r'^\(\s*(?:const\s+)?(?:unsigned\s+|signed\s+)?(?:float|double|int|char|short|long|unsigned|void|br_\w+|t[A-Z]\w*|\w+_t)(?:\s*\*)*\s*\)$')
KEYWORDS = {'return', 'if', 'else', 'for', 'while', 'do', 'switch', 'case', 'break', 'continue', 'goto', 'sizeof', 'default'}


def tokens(text):
    """[(start, end, token)] without whitespace and comments."""
    out = []
    for m in TOKEN.finditer(text):
        t = m.group(0)
        if t.isspace() or t.startswith('//') or t.startswith('/*'):
            continue
        out.append((m.start(), m.end(), t))
    return out


def _is_operand_end(t):
    return t in (')', ']') or (re.match(r'^[\w"\']', t) is not None and t not in KEYWORDS)


def binary_ops(text):
    """Every binary operator of text as (index, token list): an operator is binary when the token
    before it ends an operand and it is not the token after a cast."""
    tk = tokens(text)
    # match parentheses so casts can be recognised
    stack, pair = [], {}
    for i, (s, e, t) in enumerate(tk):
        if t in '([':
            stack.append(i)
        elif t in ')]' and stack:
            j = stack.pop()
            pair[i], pair[j] = j, i
    res = []
    for i, (s, e, t) in enumerate(tk):
        if t in PREC and i and _is_operand_end(tk[i - 1][2]):
            if tk[i - 1][2] == ')' and i - 1 in pair and TYPEISH.match(text[tk[pair[i - 1]][0]:tk[i - 1][1]]) and t in '*-&+':
                continue  # unary operator after a cast
            res.append(i)
    return tk, pair, res


def operand_extents(tk, pair, binset, i):
    """Token index ranges (l0, l1), (r0, r1) of the operands of the binary operator at token i
    (left-associative reading), or None when they cannot be determined."""
    p = PREC[tk[i][2]]
    j = i - 1
    while j >= 0:
        t = tk[j][2]
        if t in ')]':
            if j not in pair:
                return None
            j = pair[j] - 1
            continue
        if t in '([{};' or t in KEYWORDS:
            break
        if j in binset:
            if PREC[t] < p:
                break
        elif t in PREC and t not in ('*', '-', '&', '+'):
            break  # ?, :, = ... that is not a binary operator here
        j -= 1
    l0 = j + 1
    k = i + 1
    while k < len(tk):
        t = tk[k][2]
        if t in '([':
            if k not in pair:
                return None
            k = pair[k] + 1
            continue
        if t in ')]{};' or t in KEYWORDS:
            break
        if k in binset:
            if PREC[t] <= p:
                break
        elif t in PREC and t not in ('*', '-', '&', '+'):
            break
        k += 1
    if l0 > i - 1 or k - 1 < i + 1:
        return None
    return (l0, i - 1), (i + 1, k - 1)


def _has_low(tk, binset, a, b, p, strict):
    """Does the token range a..b hold a depth-0 binary operator of precedence < p (<= when strict)?"""
    depth = 0
    for j in range(a, b + 1):
        t = tk[j][2]
        if t in '([':
            depth += 1
        elif t in ')]':
            depth -= 1
        elif depth == 0 and j in binset and (PREC[t] <= p if strict else PREC[t] < p):
            return True
    return False


PURE = {'sizeof', 'fabs', 'fabsf', 'sqrt', 'sqrtf', 'sin', 'cos', 'sinf', 'cosf', 'BR_SCALAR', 'BrIntToScalar', 'BrScalarToInt'}


def side_effect_free(s):
    """No assignment, ++/--, or call other than a few pure functions/macros."""
    if re.search(r'\+\+|--|(?<![=!<>])=(?!=)', s):
        return False
    for m in re.finditer(r'(%s)\s*\(' % ID, s):
        if m.group(1) not in PURE:
            return False
    return True


def exact_float(lit):
    try:
        v = float(lit)
    except ValueError:
        return False
    return struct.unpack('<f', struct.pack('<f', v))[0] == v


def decl_block(body):
    """(start, end, [(text, type, name)]) of the local declarations at the top of the function."""
    first = body.index('{\n') + 2
    pos, decls = first, []
    pat = re.compile(r'    ((?:const |unsigned |signed |struct )*%s[ *]+)(%s)((?:\[[^\]\n]*\])*)(?: = [-\s{}\d,.xXa-fA-F]+)?;\n' % (ID, ID))
    while True:
        m = pat.match(body, pos)
        if not m:
            break
        decls.append((m.group(0), re.sub(r'\s+', ' ', m.group(1) + m.group(3)).strip(), m.group(2)))
        pos = m.end()
    return first, pos, decls


def depth_map(body):
    """Brace depth before each character (strings and comments ignored for counting)."""
    d, out = 0, []
    skip = [(m.start(), m.end()) for m in re.finditer(r'//[^\n]*|/\*.*?\*/|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'', body, re.S)]
    masked = bytearray(len(body))
    for a, b in skip:
        for i in range(a, b):
            masked[i] = 1
    for i, c in enumerate(body):
        out.append(d)
        if not masked[i]:
            if c == '{':
                d += 1
            elif c == '}':
                d -= 1
    return out


def mutations(body, allow=()):
    """[(kind, description, new_body)]. Every edit keeps the function's behaviour."""
    seen = set()
    res = []

    def out(kind, desc, nb):
        if nb != body and nb not in seen:
            seen.add(nb)
            res.append((kind, desc, nb))

    first, dend, decls = decl_block(body)
    has_goto = re.search(r'\bgoto\b', body) is not None
    code = body[dend:]
    doubles = {n for _t, ty, n in decls if 'double' in ty} | set(re.findall(r'\bdouble\s+(%s)' % ID, body[:first]))

    # cmp: swap comparison operands (both operands are plain, so nothing is reordered in time)
    flip = {'<': '>', '>': '<', '<=': '>=', '>=': '<=', '==': '==', '!=': '!='}
    for m in re.finditer(r'(?<![\w.>\])])(%s) (<=|>=|<|>|==|!=) (%s)(?=[ );&|?])' % (OPERAND, OPERAND), body):
        a, op, b = m.groups()
        pre = body[:m.start()].rstrip()
        post = body[m.end():].lstrip()
        if pre.endswith(('*', '/', '%', '+', '-', '<<', '>>')) or post[:1] in '*/%+-<>':
            continue  # part of a larger arithmetic expression
        out('cmp', 'cmp swap @%d: %s %s %s' % (m.start(), a, op, b), body[:m.start()] + '%s %s %s' % (b, flip[op], a) + body[m.end():])

    # compound: x += e <-> x = x + e / x = e + x
    def minprec(e):
        tk, pair, ops = binary_ops(e)
        depth, lo = 0, 99
        opset = set(ops)
        for j, (s, en, t) in enumerate(tk):
            if t in '([':
                depth += 1
            elif t in ')]':
                depth -= 1
            elif depth == 0 and (j in opset or t in ('?', ':', '=', ',')):
                lo = min(lo, PREC.get(t, 0))
        return lo

    for m in re.finditer(r'^(\s*)([^\n=;]+?) \+= ([^\n;]+);$', body, re.M):
        ind, lhs, e = m.groups()
        if not side_effect_free(lhs) or not side_effect_free(e):
            continue
        mp = minprec(e)
        if mp >= 12:
            out('compound', '+= -> e + x @%d' % m.start(), body[:m.start()] + '%s%s = %s + %s;' % (ind, lhs, e, lhs) + body[m.end():])
        if mp > 12:
            out('compound', '+= -> x + e @%d' % m.start(), body[:m.start()] + '%s%s = %s + %s;' % (ind, lhs, lhs, e) + body[m.end():])
    for m in re.finditer(r'^(\s*)([^\n=;]+?) = ([^\n;]+) \+ \2;$', body, re.M):
        ind, lhs, e = m.groups()
        if side_effect_free(lhs) and side_effect_free(e) and minprec(e) >= 12:
            out('compound', 'e + x -> += @%d' % m.start(), body[:m.start()] + '%s%s += %s;' % (ind, lhs, e) + body[m.end():])

    # paren: parenthesise a product in front of / + - (same grouping, other code)
    for m in re.finditer(r'(%s \* %s) ([/+-]) ' % (OPERAND, OPERAND), body):
        pre = body[:m.start()].rstrip()
        if pre.endswith(('*', '/', '%', '.', '>')) or re.search(r'[\w\])]$', pre):
            continue
        out('paren', 'paren product @%d' % m.start(), body[:m.start()] + '(' + m.group(1) + ') ' + m.group(2) + ' ' + body[m.end():])

    # commute: swap operands of + and * using real operand extents
    tk, pair, ops = binary_ops(code)
    opset = set(ops)
    for i in ops:
        t = tk[i][2]
        if t not in ('+', '*'):
            continue
        ext = operand_extents(tk, pair, opset, i)
        if not ext:
            continue
        (l0, l1), (r0, r1) = ext
        L, R = code[tk[l0][0]:tk[l1][1]], code[tk[r0][0]:tk[r1][1]]
        if not side_effect_free(L) or not side_effect_free(R) or '"' in L + R:
            continue
        if l0 == l1 and re.match(r'^(?:float|double|int|char|short|long|unsigned|void|br_\w+|t[A-Z]\w*)$', L):
            continue  # a declaration `type *name`
        p = PREC[t]
        newL = R  # the right operand only holds tighter operators
        newR = '(' + L + ')' if _has_low(tk, opset, l0, l1, p, True) else L
        a, b = dend + tk[l0][0], dend + tk[r1][1]
        mid = code[tk[l1][1]:tk[r0][0]]
        out('commute', 'swap %s @%d: %s %s %s' % (t, a, L[:30], t, R[:30]), body[:a] + newL + mid + newR + body[b:])

    # libm float/double variants (not value-preserving in general)
    if 'libm' in allow:
        for a, b in (('sin', 'sinf'), ('cos', 'cosf'), ('sqrt', 'sqrtf'), ('fabs', 'fabsf'), ('fmod', 'fmodf')):
            for m in re.finditer(r'\b%s\(' % a, body):
                out('libm', '%s -> %s @%d' % (a, b, m.start()), body[:m.start()] + b + '(' + body[m.end():])
            for m in re.finditer(r'\b%s\(' % b, body):
                out('libm', '%s -> %s @%d' % (b, a, m.start()), body[:m.start()] + a + '(' + body[m.end():])

    # literal: float <-> double, exactly representable values only
    for m in re.finditer(r'(?<![\w.])(\d+)\.(\d*)f\b', body):
        d = m.group(2) or '0'
        if exact_float('%s.%s' % (m.group(1), d)):
            out('literal', 'f literal -> double @%d' % m.start(), body[:m.start()] + '%s.%s' % (m.group(1), d) + body[m.end():])
    for m in re.finditer(r'(?<![\w.])(\d+)\.(\d+)(?![\w.])', body):
        if exact_float(m.group(0)):
            out('literal', 'double literal -> f @%d' % m.start(), body[:m.start()] + '%s.%sf' % (m.group(1), m.group(2)) + body[m.end():])

    # half: x / 2.0f <-> x * 0.5f
    for m in re.finditer(r' / 2\.0?f?\b', body):
        out('half', '/2 -> *0.5 @%d' % m.start(), body[:m.start()] + ' * 0.5f' + body[m.end():])
    for m in re.finditer(r' \* 0?\.5f?\b', body):
        out('half', '*0.5 -> /2 @%d' % m.start(), body[:m.start()] + ' / 2.0f' + body[m.end():])

    # cast: (float)/(br_scalar) in front of a parenthesised expression that is float anyway
    def float_typed(inner):
        if not re.search(r'(?<![\w.])\d+\.\d*f\b', inner):
            return False
        if re.search(r'%s\s*\(' % ID, inner) or re.search(r'[<>=!?:&|,%]|\(\s*double\s*\)', inner):
            return False
        if any(not exact_float(x) for x in re.findall(r'(?<![\w.])(\d+\.\d*)(?![\w.f])', inner)):
            return False
        return not (set(re.findall(ID, inner)) & doubles)

    for m in re.finditer(r'\((float|br_scalar)\)\(', body):
        o = m.end() - 1
        d, e = 0, None
        for j in range(o, min(len(body), o + 600)):
            if body[j] == '(':
                d += 1
            elif body[j] == ')':
                d -= 1
                if d == 0:
                    e = j
                    break
        if e is None:
            continue
        inner = body[o + 1:e]
        if not float_typed(inner):
            continue
        out('cast', 'drop cast @%d' % m.start(), body[:m.start()] + body[o:])
        tk2, pair2, ops2 = binary_ops(inner)
        top = [i for i in ops2 if not any(tk2[a][0] < tk2[i][0] < tk2[pair2[a]][0] for a in pair2 if pair2[a] > a)]
        if top:
            i = top[0]
            first_op = inner[:tk2[i][0]].rstrip()
            out('cast', 'cast -> first operand @%d' % m.start(), body[:m.start()] + '(%s)(%s)%s' % (m.group(1), first_op, inner[len(first_op):]) + body[e + 1:])

    # decl: move one declaration elsewhere in the declaration block
    if len(decls) > 1:
        for i in range(len(decls)):
            for j in range(len(decls)):
                if i == j:
                    continue
                d = [x[0] for x in decls]
                x = d.pop(i)
                d.insert(j, x)
                out('decl', 'move decl %s to slot %d' % (decls[i][2], j), body[:first] + ''.join(d) + body[dend:])

    if not has_goto:
        dm = depth_map(body)
        scal = [(t, ty, n) for t, ty, n in decls if '[' not in ty]
        for t, ty, n in scal:
            if re.search(r'&\s*%s\b' % n, code):
                continue
            occ = [dend + m.start() for m in re.finditer(r'(?<![\w.>])%s\b' % n, code)]
            # split: a later depth-1 definition `n = ...;` / `for (n = ...` starts a new life
            for k, pos in enumerate(occ[1:], 1):
                ls = body.rfind('\n', 0, pos) + 1
                line = body[ls:body.find('\n', pos)]
                if dm[ls] != 1:
                    continue
                m1 = re.match(r'^    (?:for \()?%s = ([^;]*);' % n, line)
                if not m1 or re.search(r'\b%s\b' % n, m1.group(1)) or pos != ls + line.index(n):
                    continue
                n2 = n + '2'
                if re.search(r'\b%s\b' % n2, body):
                    continue
                nb = body[:pos] + re.sub(r'(?<![\w.>])%s\b' % n, n2, body[pos:])
                nb = nb.replace(t, t + re.sub(r'\b%s\b' % n, n2, t), 1)
                out('split', 'split %s at @%d' % (n, pos), nb)
        # merge: a later local whose whole life starts after the last use of an earlier one
        for t1, ty1, a in scal:
            for t2, ty2, b in scal:
                if a == b or ty1 != ty2:
                    continue
                if re.search(r'&\s*(%s|%s)\b' % (a, b), code):
                    continue
                oa = [dend + m.start() for m in re.finditer(r'(?<![\w.>])%s\b' % a, code)]
                ob = [dend + m.start() for m in re.finditer(r'(?<![\w.>])%s\b' % b, code)]
                if not oa or not ob or ob[0] < oa[-1]:
                    continue
                ls = body.rfind('\n', 0, ob[0]) + 1
                line = body[ls:body.find('\n', ob[0])]
                la = body.rfind('\n', 0, oa[-1]) + 1
                if dm[ls] != 1 or la == ls:
                    continue
                m1 = re.match(r'^    (?:for \()?%s = ([^;]*);' % b, line)
                if not m1 or re.search(r'\b%s\b' % b, m1.group(1)):
                    continue
                nb = body[:ob[0]] + re.sub(r'(?<![\w.>])%s\b' % b, a, body[ob[0]:])
                nb = nb.replace(t2, '', 1)
                out('merge', 'merge %s into %s' % (b, a), nb)

        # order: swap adjacent independent statements
        locals_ = {n for _t, ty, n in decls if '[' not in ty and not re.search(r'&\s*%s\b' % n, code)}
        stm = list(re.finditer(r'^( +)([^\n;{}]+?) = ([^\n;{}]+);\n', body, re.M))
        simple_rhs = lambda e: side_effect_free(e) and all(x in locals_ or x in PURE or re.match(r'^(?:sizeof|f|F|x|X|[A-Z_][A-Z0-9_]*)$', x) for x in re.findall(ID, e)) and not re.search(r'->|\[|\*\s*%s|\.\s*%s' % (ID, ID), e)
        for s1, s2 in zip(stm, stm[1:]):
            if s1.end() != s2.start() or s1.group(1) != s2.group(1) or s1.start() < dend:
                continue
            l1, e1, l2, e2 = s1.group(2), s1.group(3), s2.group(2), s2.group(3)
            if re.search(r'[-+*/%&|^<>]$', l1) or re.search(r'[-+*/%&|^<>]$', l2):
                continue  # compound assignment
            if re.match(r'^(for|if|while|return)\b', l1.strip()) or re.match(r'^(for|if|while|return)\b', l2.strip()):
                continue
            ok = False
            if l1 in locals_ and l2 in locals_ and l1 != l2:
                # plain locals: neither statement may mention the other's target
                ok = side_effect_free(e1) and side_effect_free(e2) and not re.search(r'\b%s\b' % re.escape(l1), e2) and not re.search(r'\b%s\b' % re.escape(l2), e1) and not re.search(r'%s\s*\(' % ID, e1 + e2)
            else:
                m1 = re.match(r'^(%s)((?:->|\.)%s|\[\d+\])((?:\.%s|\[\d+\])*)$' % (ID, ID, ID), l1)
                m2 = re.match(r'^(%s)((?:->|\.)%s|\[\d+\])((?:\.%s|\[\d+\])*)$' % (ID, ID, ID), l2)
                if m1 and m2 and m1.group(1) == m2.group(1) and m1.group(2) != m2.group(2) and m1.group(2)[:1] == m2.group(2)[:1]:
                    ok = simple_rhs(e1) and simple_rhs(e2) and m1.group(1) not in re.findall(ID, e1 + ' ' + e2)
            if ok:
                out('order', 'swap statements @%d: %s / %s' % (s1.start(), l1[:25], l2[:25]), body[:s1.start()] + s2.group(0) + s1.group(0) + body[s2.end():])

    # ternary: simple if/else assignment <-> ?:
    for m in re.finditer(r'^(\s*)if \(([^\n{}]+)\) \{\n\s*(%s) = ([^\n;]+);\n\s*\} else \{\n\s*\3 = ([^\n;]+);\n\s*\}\n' % OPERAND, body, re.M):
        ind, c, v, a, b = m.groups()
        if side_effect_free(v):
            out('ternary', 'if/else -> ternary @%d' % m.start(), body[:m.start()] + '%s%s = (%s) ? (%s) : (%s);\n' % (ind, v, c, a, b) + body[m.end():])
    for m in re.finditer(r'^(\s*)(%s) = ([^\n;?]+) \? ([^\n;:?]+) : ([^\n;?:]+);\n' % OPERAND, body, re.M):
        ind, v, c, a, b = m.groups()
        if side_effect_free(v) and not re.search(r'\b%s\b' % re.escape(v), c):
            out('ternary', 'ternary -> if/else @%d' % m.start(), body[:m.start()] + '%sif (%s) {\n%s    %s = %s;\n%s} else {\n%s    %s = %s;\n%s}\n' % (ind, c, ind, v, a, ind, ind, v, b, ind) + body[m.end():])

    # not: !x <-> x == 0 as a whole condition term
    for m in re.finditer(r'(?<=[(&|] )!(%s)(?=[ )])' % OPERAND, body):
        post = body[m.end():].lstrip()
        if post[:1] in ')' or post[:2] in ('&&', '||'):
            out('not', '!x -> x == 0 @%d' % m.start(), body[:m.start()] + m.group(1) + ' == 0' + body[m.end():])
    for m in re.finditer(r'(?<![\w.>\])])(%s) == 0(?=[ )])' % OPERAND, body):
        pre = body[:m.start()].rstrip()
        post = body[m.end():].lstrip()
        if (pre.endswith('(') or pre.endswith('&&') or pre.endswith('||')) and (post[:1] == ')' or post[:2] in ('&&', '||')) and not m.group(1).startswith(('-', '!')):
            out('not', 'x == 0 -> !x @%d' % m.start(), body[:m.start()] + '!' + m.group(1) + body[m.end():])
    return res


def main():
    if os.environ.get('SCORER', '') != 'text' and '--list' not in sys.argv:
        sys.path.insert(0, HERE)
        import rec2cfg
        rec2cfg.ensure_reccmp_python()  # may restart this command under reccmp's python
    args = [a for a in sys.argv[1:] if a != '--list']
    list_only = '--list' in sys.argv
    wk, rel, func, addr = args[:4]
    budget = int(args[4]) if len(args) > 4 else int(os.environ.get('COMPILES', '300'))
    if budget < 20:
        budget *= 50  # the old meaning of this argument was "rounds"
    E = os.environ.get
    allow = set(filter(None, E('ALLOW', '').split(',')))
    kinds = set(filter(None, E('KINDS', '').split(',')))
    skip = set(filter(None, E('SKIP', '').split(',')))
    rnd = random.Random(int(E('SEED', '1')))
    T0, T1 = float(E('T0', '2.0')), float(E('T1', '0.2'))

    # reuse fastmatch.py's setup (compile command, parsers, scoring)
    src = open(os.path.join(HERE, 'fastmatch.py'), encoding='utf-8').read()
    setup = src.split('\nraw = open(path')[0]
    sys.argv = ['fastmatch.py', wk, rel, func, addr, 'unused']
    os.environ['REC2_REEXEC'] = '1'  # this process is the one that runs; fastmatch must not restart
    ns = {'__file__': os.path.join(HERE, 'fastmatch.py'), '__name__': 'fastmatch_setup'}
    exec(compile(setup, 'fastmatch-setup', 'exec'), ns)
    path = ns['path']
    # logs, the best version and the cache: $PERMUTE_DIR, rec2local.PERMUTE_DIR, or the temp directory
    OUT = E('PERMUTE_DIR') or getattr(ns['rec2cfg'].LOCAL, 'PERMUTE_DIR', None) or os.path.join(ns['tempfile'].gettempdir(), 'permute-' + ns['CFG']['name'])
    raw = open(path, encoding='utf-8', newline='').read()
    nl = '\r\n' if '\r\n' in raw else '\n'
    text = raw.replace('\r\n', '\n')
    fm = re.search(r'^[^\n]*[ *]%s\([^\n]*\{\n.*?^\}\n' % re.escape(func), text, re.M | re.S)
    assert fm, 'function not found in file'
    head, tail = text[:fm.start()], text[fm.end():]
    base_line = head.count('\n') + 1  # file line of the function's first line

    def muts(body, lines=None):
        ms = mutations(body, allow)
        if kinds:
            ms = [x for x in ms if x[0] in kinds]
        ms = [x for x in ms if x[0] not in skip]
        if lines:
            keep = []
            for k, d, nb in ms:
                i = next((i for i, (a, b) in enumerate(zip(body, nb)) if a != b), min(len(body), len(nb)))
                ln = base_line + body.count('\n', 0, i)
                if lines[0] <= ln <= lines[1]:
                    keep.append((k, d, nb))
            ms = keep
        return ms

    lines = [int(x) for x in re.split(r'[:-]', E('LINES'))] if E('LINES') else None
    if list_only:
        import collections
        c = collections.Counter(k for k, d, nb in muts(fm.group(0), lines))
        print('%s: %d mutations: %s' % (func, sum(c.values()), ', '.join('%s %d' % x for x in sorted(c.items()))))
        return

    os.makedirs(OUT, exist_ok=True)
    log = open(os.path.join(OUT, func + '.log'), 'a', encoding='utf-8')
    open(os.path.join(OUT, func + '.orig.c'), 'w', encoding='utf-8', newline='').write(fm.group(0))

    def say(s):
        print(s, flush=True)
        log.write(s + '\n')
        log.flush()

    O = ns['original_ex']()
    ctx = hashlib.sha1((head + '\0' + tail + '\0' + addr + '\0' + E('LINES', '') + E('STACK', '') + E('SCORER', '')).encode()).hexdigest()[:12]
    cache_file = os.path.join(OUT, func + '.cache.json')
    try:
        disk = json.load(open(cache_file))
    except (OSError, ValueError):
        disk = {}
    by_src = disk.setdefault(ctx, {})  # source hash -> [cost, info, asm key]
    by_asm = {v[2]: v for v in by_src.values() if v[2]}
    stats = {'compiled': 0, 'src_hits': 0, 'asm_hits': 0, 'errors': 0}
    state = {'orange': None}

    def cost(body, lines_now):
        """(cost, info, asm key); cost None when it does not compile."""
        h = hashlib.sha1((body + repr(lines_now)).encode()).hexdigest()
        if h in by_src:
            stats['src_hits'] += 1
            return tuple(by_src[h])
        stats['compiled'] += 1
        err = ns['compile_text'](head + body + tail, nl)
        if err:
            stats['errors'] += 1
            by_src[h] = [None, err, None]
            return None, err, None
        try:
            G = ns['listing_ex'](func)
        except StopIteration:
            by_src[h] = [None, 'function missing from listing', None]
            return None, 'function missing from listing', None
        key = G['key'] + repr(lines_now)
        if key in by_asm:
            stats['asm_hits'] += 1
            by_src[h] = by_asm[key]
            return tuple(by_asm[key])
        A = ns['analyse'](O, G, func, addr)
        size = (len(O['ins']) + len(G['ins'])) / 2.0
        import collections
        calls = sum(((collections.Counter(O['calls']) - collections.Counter(G['calls'])) + (collections.Counter(G['calls']) - collections.Counter(O['calls']))).values())
        frame = 0 if ns['frame_of'](O['ins']) == ns['frame_of'](G['ins']) else 4
        whole = 0.5 * (1 - A['ratio']) * size + 0.5 * (1 - A['blk']) * size + frame + 2 * calls
        info = '%s %.4f blk %.3f%s%s' % (A['scorer'].split()[0], A['ratio'], A['blk'], ' eff' if A['eff'] else '', ' !frame' if frame else '')
        if lines_now:
            rr, rnd_, osp, n = ns['region'](A['sm'], O['ins'], G, lines_now[0], lines_now[1], state['orange'])
            if state['orange'] is None:
                state['orange'] = osp
            c = rnd_ + 0.1 * whole
            info = 'region %.3f (%d differing) | ' % (rr, rnd_) + info
        else:
            c = whole
        v = [round(c, 4), info, key]
        by_src[h] = v
        by_asm[key] = v
        return tuple(v)

    cur = best = fm.group(0)
    cur_lines = best_lines = lines
    try:
        cur_cost, info, cur_key = cost(cur, cur_lines)
        assert cur_cost is not None, 'the starting version does not compile: %s' % info
        best_cost = cur_cost
        say('%s: start cost %.2f [%s] (%d original instructions%s)' % (func, cur_cost, info, len(O['ins']), ', original span %s' % (state['orange'],) if lines else ''))
        step = tried = 0
        stale = 0
        while stats['compiled'] < budget and best_cost > 0 and tried < budget * 20:
            ms = muts(cur, cur_lines)
            if not ms:
                say('no mutations apply')
                break
            # pick a kind first so that a kind with many candidates does not crowd out the rest
            kind = rnd.choice(sorted({k for k, d, nb in ms}))
            k, desc, nb = rnd.choice([x for x in ms if x[0] == kind])
            tried += 1
            nlines = cur_lines
            if cur_lines:
                nlines = [cur_lines[0], cur_lines[1] + nb.count('\n') - cur.count('\n')]
            c, info, key = cost(nb, nlines)
            if c is None:
                continue
            step += 1
            T = T0 * (T1 / T0) ** min(1.0, stats['compiled'] / float(budget))
            d = c - cur_cost
            if d <= 0 or rnd.random() < math.exp(-d / T):
                if d != 0 or key != cur_key:
                    say('  %s%8.2f  T=%.2f  %s  [%s]' % ('*' if c < best_cost else ' ', c, T, desc[:70], info))
                cur, cur_cost, cur_key, cur_lines = nb, c, key, nlines
                if c < best_cost:
                    best, best_cost, best_lines, stale = nb, c, nlines, 0
                    open(os.path.join(OUT, func + '.best.c'), 'w', encoding='utf-8', newline='').write(best)
            stale += 1
            if stale > max(40, budget // 4) and cur_cost > best_cost:
                cur, cur_cost, cur_lines, stale = best, best_cost, best_lines, 0  # restart from the best
                cur_key = None
                say('  back to the best version (cost %.2f)' % best_cost)
    finally:
        final = best if E('KEEP', '1') != '0' else fm.group(0)
        open(path, 'w', encoding='utf-8', newline='').write((head + final + tail).replace('\n', nl))
        json.dump(disk, open(cache_file, 'w'))
    say('%s: final cost %.2f (%d compiled, %d compile errors, %d sources seen before, %d repeats of known code)' % (func, best_cost, stats['compiled'], stats['errors'], stats['src_hits'], stats['asm_hits']))
    say('best version, log and cache: %s' % OUT)


if __name__ == '__main__':
    main()
