# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#
# Copyright 2022-2026 rec2 developers

"""rec2cfg.py : where things are, for fastmatch.py / permute.py / srcmap.py / objcmp.py.

Nothing here is specific to one machine. Every path comes from an argument, an environment
variable, or a file of the rec2 repository:

  BUILD (first argument of the scripts)
        the MSVC5 build directory (the one with build.ninja, reccmp-build.yml and, after a
        build, rec2_hw.exe / rec2_hw.pdb).
  repository root
        `project:` in BUILD/reccmp-build.yml (cmake writes it), or $REC2_REPO.
  original executable
        $REC2_ORIG_EXE, else the target's `path:` in <repo>/reccmp-user.yml.
  target id
        $REC2_TARGET (default CARMA2_HW).
  objdump (i686 objdump from binutils / MinGW)
        $OBJDUMP, else `objdump` on PATH.
  MSVC5 environment
        taken from the compile command in build.ninja: INCLUDE/LIB/PATH are set from the
        directory of the cl.exe named there, so the scripts need no "enter MSVC5" step.
        If cl.exe is not given with a path there, run the scripts from an MSVC5 prompt.
  Ghidra export of the original (optional)
        $REC2_GHIDRA_DECOMP: a .jsonl with one {"address": "0x..", "name": ..} object per
        function. Only used for function ends and for names of unannotated call targets;
        without it the annotations of the repository are used.

An optional module rec2local.py next to the scripts may define defaults for one machine:
build_dir(name) (so a short name can be passed instead of BUILD), EXE, OBJDUMP, GHIDRA_DECOMP,
RECCMP_PYTHON (the python that has reccmp installed, when the scripts are started with another
one; $RECCMP_PYTHON does the same).
"""
import os
import re
import shutil
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
try:
    sys.path.insert(0, HERE)
    import rec2local as LOCAL
except ImportError:
    LOCAL = None
TARGET = os.environ.get('REC2_TARGET', 'CARMA2_HW')


def _yaml_value(path, section, key):
    """Value of `key:` under `  section:` in a small YAML file (no YAML module needed)."""
    if not os.path.exists(path):
        return None
    inside = False
    for l in open(path, encoding='utf-8'):
        if re.match(r'^\s+%s:\s*$' % re.escape(section), l):
            inside = True
        elif inside:
            m = re.match(r'^\s+%s:\s*(.+?)\s*$' % re.escape(key), l)
            if m:
                return m.group(1).strip('\'"')
            if re.match(r'^\s{0,2}\S', l):
                inside = False
    return None


def resolve(build_arg):
    """dict(build, repo, exe, objdump, ghidra, name) for the BUILD argument."""
    if os.path.isdir(build_arg):
        build = os.path.abspath(build_arg)
    elif LOCAL is not None and hasattr(LOCAL, 'build_dir') and os.path.isdir(LOCAL.build_dir(build_arg)):
        build = LOCAL.build_dir(build_arg)
    else:
        sys.exit('BUILD must be the MSVC5 build directory: %s' % build_arg)
    repo = os.environ.get('REC2_REPO')
    if not repo:
        y = os.path.join(build, 'reccmp-build.yml')
        mm = re.search(r'^project:\s*(.+?)\s*$', open(y, encoding='utf-8').read(), re.M) if os.path.exists(y) else None
        if not mm:
            sys.exit('cannot find the repository: no `project:` in %s; set REC2_REPO' % y)
        repo = mm.group(1).strip('\'"')
    repo = os.path.normpath(repo)
    exe = os.environ.get('REC2_ORIG_EXE') or _yaml_value(os.path.join(repo, 'reccmp-user.yml'), TARGET, 'path') or getattr(LOCAL, 'EXE', None)
    if not exe or not os.path.exists(exe):
        sys.exit('original executable not found (%s); set REC2_ORIG_EXE or reccmp-user.yml' % exe)
    objdump = os.environ.get('OBJDUMP') or getattr(LOCAL, 'OBJDUMP', None) or shutil.which('objdump')
    ghidra = os.environ.get('REC2_GHIDRA_DECOMP') or getattr(LOCAL, 'GHIDRA_DECOMP', None)
    if ghidra and not os.path.exists(ghidra):
        ghidra = None
    name = re.sub(r'\W+', '_', os.path.basename(build.rstrip('\\/')))
    return {'build': build, 'repo': repo, 'exe': os.path.normpath(exe), 'objdump': objdump, 'ghidra': ghidra, 'name': name}


def msvc_env(cl_command):
    """Environment for running the cl.exe command taken from build.ninja."""
    env = dict(os.environ)
    mm = re.match(r'^\s*"?([^"]*?)[\\/]bin[\\/]cl\.exe', cl_command, re.I)
    if mm:
        tc = mm.group(1)
        env['INCLUDE'] = ';'.join([tc + r'\include', tc + r'\atl\include', tc + r'\mfc\include'])
        env['LIB'] = ';'.join([tc + r'\lib', tc + r'\mfc\lib'])
        env['PATH'] = tc + r'\bin;' + tc + r'\redist;' + env['PATH']
    return env


def ensure_reccmp_python():
    """reccmp must be importable. If it is not under this python, run the same command again
    with $RECCMP_PYTHON (or rec2local.RECCMP_PYTHON), the python that has reccmp installed.
    Returns False when reccmp is not available at all."""
    try:
        import reccmp  # noqa: F401
        return True
    except ImportError:
        pass
    py = os.environ.get('RECCMP_PYTHON') or getattr(LOCAL, 'RECCMP_PYTHON', None)
    if py and os.path.exists(py) and not os.environ.get('REC2_REEXEC'):
        import subprocess
        e = dict(os.environ, REC2_REEXEC='1')
        sys.exit(subprocess.run([py] + sys.argv, env=e).returncode)
    return False


def function_names(cfg):
    """(address -> name, name -> address) of the original's functions: the repository's
    FUNCTION/STUB/LIBRARY annotations, the reccmp CSVs (decorated library names) and, when
    available, the names of the Ghidra export."""
    import glob
    import json
    a2n, n2a = {}, {}
    if cfg['ghidra']:
        for l in open(cfg['ghidra'], encoding='utf-8'):
            d = json.loads(l)
            a2n[int(d['address'], 16)] = d['name']
            n2a.setdefault(d['name'], int(d['address'], 16))
    for f in glob.glob(os.path.join(cfg['repo'], 'reccmp', '%s-*.csv' % TARGET.lower())):
        for l in open(f, encoding='utf-8', errors='replace'):
            p = l.strip().split(',')
            if len(p) >= 3 and p[1] in ('library', 'function', 'stub'):
                a2n[int(p[0], 16)] = p[2]
                n2a[p[2]] = int(p[0], 16)
    pat = re.compile(r'^// (?:FUNCTION|STUB|LIBRARY|SYNTHETIC): %s (0x[0-9a-fA-F]+)\n(?://[^\n]*\n)*([^\n]*)' % TARGET, re.M)
    for f in glob.glob(os.path.join(cfg['repo'], 'src', '**', '*.c*'), recursive=True):
        try:
            txt = open(f, encoding='utf-8', errors='replace').read()
        except OSError:
            continue
        if TARGET not in txt:
            continue
        for m in pat.finditer(txt.replace('\r\n', '\n')):
            nm = re.search(r'([A-Za-z_]\w*)\s*\(', m.group(2))
            if nm:
                a2n[int(m.group(1), 16)] = nm.group(1)
                n2a[nm.group(1)] = int(m.group(1), 16)
    return a2n, n2a
