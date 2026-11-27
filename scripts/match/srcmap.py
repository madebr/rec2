# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#
# Copyright 2022-2026 rec2 developers

"""srcmap.py BUILD FILE FUNC ADDR [FIRST:LAST] : which source lines the differing instructions come from.

Compiles FILE as it is (MSVC5, /FAs listing; see fastmatch.py for the arguments), compares FUNC
with the original at ADDR and prints, largest first,
    lines X-Y: N differing instructions | first source line of the range
for every run of source lines that owns differing instructions. Instructions the original has
and ours lacks are charged to the line at that point of our code.

With FIRST:LAST (file line numbers, inclusive) it instead scores only the instructions
generated from those lines against the original instructions they align with, and prints that
region's score, the differing hunks inside it and the original span as ORANGE=i:j for
fastmatch.py / permute.py, so one statement can be searched on its own inside a big function.

Limits: MSVC5 attributes the code of an inlined function to the line of the call, so the body
of an inlined helper appears as one large entry on the caller's line. The attribution follows
the text alignment of the two instruction streams; in a badly matching function a hunk can be
charged to a neighbouring line.

Environment: TOP=n entries (default 40); everything fastmatch.py accepts (STACK=1, SCORER=...).
"""
import os
import subprocess
import sys

if len(sys.argv) < 5:
    sys.exit(__doc__)
here = os.path.dirname(os.path.abspath(__file__))
env = dict(os.environ)
env.setdefault('SRCMAP', env.get('TOP', '40'))
env.setdefault('BLOCKS', '0')
if len(sys.argv) > 5:
    env['LINES'] = sys.argv[5]
    env['SRCMAP'] = '0'
    env['UDIFF_REGION'] = '1'
sys.exit(subprocess.run([sys.executable, os.path.join(here, 'fastmatch.py')] + sys.argv[1:5], env=env).returncode)
