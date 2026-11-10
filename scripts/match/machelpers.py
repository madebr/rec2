# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#
# Copyright 2022-2026 rec2 developers

"""machelpers.py MAC_DECOMP.jsonl FUNC [BUILD] : which Mac functions are inlined in a Windows function.

The Mac build of Carmageddon 2 is a debug build in which nothing is inlined, so its call graph
shows the helper functions the Windows compiler folded into their callers. For the Windows
function FUNC (a name, or 0xADDR) this walks the Mac function's callees, transitively, and
sorts each into:
  inlined   the Windows function has no direct call to it: its body is part of FUNC there.
            The walk continues into its own callees. "inline-only" is added when the
            repository has no Windows function of that name at all.
  called    the Windows function calls it directly (the walk stops there).
  runtime   C runtime / compiler helpers and callees Ghidra could not name: not judged.
For every inlined helper it prints the Mac size, parameter list and the local variables of
Ghidra's pseudo-C (types and count; the names are Ghidra's), so helper boundaries and the
locals each helper brings into the caller's frame are known before any code is written.
With --bodies the pseudo-C of each inlined helper is printed too.

MAC_DECOMP.jsonl is a Ghidra export of the Mac executable with one JSON object per function:
  {"address": "0x..", "name": "CloneMemory", "size": 112, "c": "<pseudo-C>",
   "callees": ["BrMemAllocate", ...]}
(any headless Ghidra script that writes, for each function, getName(), getBody().getNumAddresses(),
DecompInterface.decompileFunction(...).getDecompiledFunction().getC() and the names of
getCalledFunctions() produces it; names with a leading '.' or '_' are accepted).

The Windows side comes from the original executable (objdump) and the repository's annotations:
BUILD is the MSVC5 build directory as for fastmatch.py; without it set REC2_REPO and, if
reccmp-user.yml does not name it, REC2_ORIG_EXE. A callee is recognised as "called" by name, so
the Windows function must be annotated under the same name as on the Mac (a Ghidra export of
the Windows executable in REC2_GHIDRA_DECOMP adds names for unannotated functions).
"""
import json
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import rec2cfg  # noqa: E402

DEBUG = {'_debug_fn_start', '_debug_fn_end'}
RUNTIME = re.compile(r'^(?:FUN|thunk_FUN|sub|LAB|EXT|DAT)_[0-9a-fA-F]+$|^_?_?(?:mem(?:move|cpy|set|cmp)|str\w+|floor|ceil|sqrt|sin|cos|tan|atan2?|asin|acos|fabs|fmod|pow|exp|log|abs|rand|srand|malloc|free|printf|sprintf|sscanf|fopen|fclose|fread|fwrite|fgets|fseek|ftell|ftol|toupper|tolower|qsort|__\w+)f?$')


def clean(name):
    return name.lstrip('._') if not name.startswith('_debug') else name


def windows_calls(cfg, a, a2n):
    """Names (or hex addresses) of the direct call targets of the original function at a."""
    starts = sorted(a2n)
    end = next((s for s in starts if s > a), a + 0x4000)
    out = subprocess.run([cfg['objdump'], '-d', '-M', 'intel', '--start-address=0x%x' % a, '--stop-address=0x%x' % end, cfg['exe']],
                         capture_output=True, text=True).stdout
    calls = []
    for m in re.finditer(r'\bcall\s+0x([0-9a-f]+)', out):
        t = int(m.group(1), 16)
        calls.append(a2n.get(t, '0x%x' % t))
    return calls


def locals_of(c):
    """(parameters, [local declarations]) from Ghidra's pseudo-C."""
    m = re.search(r'\(([^)]*)\)\s*\{', c, re.S)
    params = re.sub(r'\s+', ' ', m.group(1)).strip() if m else '?'
    body = c[m.end():] if m else c
    decls = []
    for l in body.splitlines():
        l = l.strip()
        if not l:
            if decls:
                break
            continue
        d = re.match(r'^([A-Za-z_][\w ]*?[\w*])\s+(\**\w+(?:\s*\[\d+\])*);$', l)
        if not d:
            break
        decls.append((d.group(1).strip(), d.group(2)))
    return params, decls


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    bodies = '--bodies' in sys.argv
    if len(args) < 2:
        sys.exit(__doc__)
    mac = {}
    for l in open(args[0], encoding='utf-8'):
        d = json.loads(l)
        mac.setdefault(clean(d['name']), d)
    if len(args) > 2:
        cfg = rec2cfg.resolve(args[2])
    else:
        repo = os.environ.get('REC2_REPO') or sys.exit('give BUILD or set REC2_REPO')
        exe = os.environ.get('REC2_ORIG_EXE') or rec2cfg._yaml_value(os.path.join(repo, 'reccmp-user.yml'), rec2cfg.TARGET, 'path')
        cfg = {'repo': repo, 'exe': exe, 'objdump': os.environ.get('OBJDUMP') or getattr(rec2cfg.LOCAL, 'OBJDUMP', None) or 'objdump',
               'ghidra': os.environ.get('REC2_GHIDRA_DECOMP') or getattr(rec2cfg.LOCAL, 'GHIDRA_DECOMP', None)}
    a2n, n2a = rec2cfg.function_names(cfg)
    key = args[1]
    if key.lower().startswith('0x'):
        addr = int(key, 16)
        name = a2n.get(addr) or sys.exit('no name known for %s' % key)
    else:
        name = key
        addr = n2a.get(name)
        if addr is None:
            sys.exit('%s is not annotated in the repository; pass its address' % name)
    if name not in mac:
        sys.exit('%s is not in the Mac export' % name)
    wcalls = windows_calls(cfg, addr, a2n)
    wset = set(wcalls)
    unnamed = sorted(c for c in wset if c.startswith('0x') or re.match(r'^FUN_', c))
    print('%s: Windows 0x%08x, Mac %s (%s bytes)' % (name, addr, mac[name]['address'], mac[name].get('size', '?')))
    print('Windows direct calls: %d sites, %d targets%s' % (len(wcalls), len(wset), ('; without a name: ' + ', '.join(unnamed)) if unnamed else ''))
    inlined, called, runtime, seen = [], [], [], {name}

    def walk(fn, depth, via):
        for c in mac[fn].get('callees', []):
            c = clean(c)
            if c in DEBUG or c in seen:
                continue
            seen.add(c)
            if c in wset or '_' + c in wset or '__' + c in wset:
                called.append((c, via))
            elif RUNTIME.match(c) or c not in mac:
                runtime.append((c, via))
            else:
                inlined.append((c, depth, via))
                walk(c, depth + 1, c)

    walk(name, 1, name)
    print('\nInlined on Windows (%d), in Mac call order:' % len(inlined))
    for c, depth, via in inlined:
        d = mac[c]
        params, decls = locals_of(d.get('c') or '')
        note = '' if c in n2a else '  [inline-only: no Windows function of this name]'
        print('%s%s  (Mac %s, %s bytes; called from %s)%s' % ('  ' * depth, c, d['address'], d.get('size', '?'), via, note))
        print('%s    parameters: %s' % ('  ' * depth, params))
        print('%s    locals (%d): %s' % ('  ' * depth, len(decls), ', '.join('%s %s' % x for x in decls) or '-'))
        own = [clean(x) for x in d.get('callees', []) if clean(x) not in DEBUG]
        if own:
            print('%s    calls: %s' % ('  ' * depth, ', '.join(dict.fromkeys(own))))
    print('\nCalled directly on Windows too (%d): %s' % (len(called), ', '.join('%s' % c for c, v in called) or '-'))
    print('Runtime / unnamed, not judged (%d): %s' % (len(runtime), ', '.join('%s (in %s)' % x for x in runtime) or '-'))
    if unnamed:
        print('Note: the Windows function calls targets without a name; a Mac callee listed as inlined may be one of them.')
    if bodies:
        for c, depth, via in inlined:
            print('\n===== %s =====\n%s' % (c, (mac[c].get('c') or '').replace('\r\n', '\n').strip()))


if __name__ == '__main__':
    main()
