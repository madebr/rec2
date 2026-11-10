# Matching helpers for single functions

Small scripts for working on one function at a time with the MSVC5 matching build. They do not
replace `reccmp`: a full build plus `reccmp-reccmp` remains the result that counts. They make
the loop between two full builds shorter and say where a difference comes from.

| script | what it does |
|---|---|
| `fastmatch.py` | Compiles one source file with the exact MSVC5 command of the build and scores one function against the original: reccmp's own ratio from the fresh `.obj`, structure checks (frame, instruction count, calls), per-basic-block score, differing instructions by source line. Takes a set of source variants of the function and ranks them. |
| `srcmap.py` | The source-line view only: "lines X-Y: N differing instructions" for a function as it is in the tree, or the score and hunks of one line range. Wrapper around `fastmatch.py`. |
| `permute.py` | Simulated-annealing search over behaviour-preserving rewrites of one function (comparison direction, commutative operands, literal type, declaration order, split/merged locals, statement order, ...), scored by `fastmatch.py`'s scorer. |
| `objcmp.py` | Prototype: runs reccmp's function comparison on one function of a COFF `.obj` (relocations resolved in memory, nothing linked). Also the scorer used by `fastmatch.py`. |
| `machelpers.py` | Lists the Mac (debug build, nothing inlined) functions whose bodies are inlined into a given Windows function, with their locals, from a Ghidra export of the Mac executable. |
| `rec2cfg.py` | Shared configuration: finds the repository, the original executable, objdump and the compile command. Not run by itself. |

## Requirements

- Windows, a configured **MSVC5 matching build directory** (cmake + ninja, as in the README of
  the repository) that has been built once. Called `BUILD` below.
- Python 3.10+. The scripts use only the standard library, plus **reccmp** (already required
  for matching work; it brings capstone) for `objcmp.py` and for the default scorer of
  `fastmatch.py`/`permute.py`. Nothing else is needed; `scripts/requirements.txt` is not
  involved. Without reccmp, `SCORER=text` still works.
- `ninja` on `PATH` (the scripts ask it for the compile command of a file).
- An i686 `objdump` (binutils, e.g. from MinGW) on `PATH` or in `OBJDUMP`. It is used to
  disassemble the original for the structure checks and the block/line views.

## Configuration (no machine-specific paths in the scripts)

| what | where it comes from |
|---|---|
| build directory | first argument `BUILD` |
| repository root | `project:` in `BUILD/reccmp-build.yml` (written by cmake), or `REC2_REPO` |
| original executable | `REC2_ORIG_EXE`, else `targets: <id>: path:` in `<repo>/reccmp-user.yml` |
| target id | `REC2_TARGET` (default `CARMA2_HW`) |
| MSVC5 environment | derived from the `cl.exe` path in the compile command of `build.ninja` (INCLUDE/LIB/PATH are set for the child process); if `cl.exe` appears there without a path, run from an MSVC5 prompt |
| objdump | `OBJDUMP`, else `objdump` on `PATH` |
| reccmp's python | the python the script is started with; if reccmp is not importable there and `RECCMP_PYTHON` names another interpreter, the script restarts itself with it |
| names of call targets | the repository's `// FUNCTION:`/`// STUB:`/`// LIBRARY:` annotations and `reccmp/<target>-*.csv`; optionally `REC2_GHIDRA_DECOMP` (see below) |
| permute's output | `PERMUTE_DIR`, default `permute-<build>` in the temp directory |

Optional: `REC2_GHIDRA_DECOMP=<file.jsonl>` with one `{"address": "0x004f39a0", "name": "InsertQuad"}`
object per function of the *Windows* executable (any Ghidra export). It gives exact function
ends and names for unannotated call targets. Without it the end of a function is taken as the
next annotated address or the padding after the last `ret`, which is right for annotated
neighbours and may run on into an unannotated neighbour that follows without padding.

## Usage

All commands take the build directory, the source file relative to the repository, the function
name and its original address.

```
# how does the function stand right now?
python scripts/match/fastmatch.py BUILD src/carma2/newcommon/51-smash.c InsertQuad 0x004f39a0

0.9968  current    frame 0x6c | n 1542 | calls ok(6) | blk 0.996 | 7 differing | text 0.996 | eff
worst blocks of current (2 of 90 original blocks differ; scorer: reccmp):
  0.91  orig[0:54] (54 ins)  best: ours[0:54] lines 1500-1527
  0.93  orig[1521:1535] (14 ins)  best: ours[1521:1535] lines 1807-1807
differing instructions by source line of current:
  lines 1526-1527: 6 differing instructions | mid_u = (((pCorners[1]->map.v[0] + ...
  lines 1807-1807: 1 differing instructions | pPoint_indices[i - 4] = v[i] - model->vertices;
```

- The first number is reccmp's ratio (`eff` = reccmp's effective match). A leading `!` marks a
  failed structure check: frame size (`sub esp`), instruction count, or the set of calls.
- `blk` pairs every basic block with its best counterpart anywhere on the other side, so one
  moved or shifted region does not hide that the rest is fine.
- Rank variants: put `VARIANTS = {"label": "<full text of the function>", ...}` in a Python
  file and pass it as fifth argument. The file is restored afterwards.
- One statement: `LINES=1526:1527` scores only the instructions generated from those lines;
  `ORANGE=i:j` pins the original span it prints. `srcmap.py BUILD FILE FUNC ADDR 1526:1527`
  shows the hunks of that region.
- Other switches (`STACK=1`, `UDIFF=label`, `SCORER=text`, `LEGACY=1`, ...) are in the
  docstring.

```
python scripts/match/permute.py --list BUILD FILE FUNC ADDR      # mutations that apply, per kind
python scripts/match/permute.py BUILD FILE FUNC ADDR 300         # search, 300 compiles
LINES=1526:1527 python scripts/match/permute.py BUILD FILE FUNC ADDR 150
```

`permute.py` leaves the best version in the file (`KEEP=0`: restore). Its mutations are
value-preserving by construction (the guards are documented in the script), but the result is
a candidate: read the diff and run the full check. It never compiles the same source twice and
never scores the same generated code twice.

```
python scripts/match/objcmp.py BUILD path/to/51-smash.c.obj InsertQuad,ProcessTexture [--diff]
```

Prints reccmp's ratio for functions of one object without linking it. `BUILD` must contain a
finished link (`rec2_hw.exe` + `.pdb`): it is the table from symbols to names. See the script's
docstring for how relocations are resolved, and the known limit (C runtime aliases such as
`_stricmp`).

```
python scripts/match/machelpers.py mac-decomp.jsonl ProcessTexture BUILD [--bodies]
```

Needs a Ghidra export of the **Mac** executable, one JSON object per function:

```
{"address": "0x100e8604", "name": "CloneMemory", "size": 112,
 "c": "<decompiler pseudo-C>", "callees": ["BrMemAllocate", "_memmove"]}
```

It is produced by a headless Ghidra run over the analysed Mac program with a script that, for
every function, writes `getEntryPoint()`, `getName()`, `getBody().getNumAddresses()`, the
decompiler output (`DecompInterface.decompileFunction(f, 60, monitor).getDecompiledFunction().getC()`)
and the names of `getCalledFunctions(monitor)`:

```
analyzeHeadless <project dir> <project name> -process "<Mac program>" -readOnly -noanalysis \
    -scriptPath <dir> -postScript ExportFunctions.java mac-decomp.jsonl
```

The export is not part of the repository (it is derived from the Mac executable); its path is
an argument.

## What is not portable / known limits

- Windows only: the compile command is run through `cmd /c`, as ninja would run it.
- `fastmatch.py` writes the candidate into the source file and restores it afterwards; do not
  run two instances on the same file, or a build of the same tree, at the same time.
- The reccmp scorer names symbols through the last link in `BUILD`. If the tree has changed a
  lot since that link (other branch), names of new globals and functions are taken from the
  object's symbols instead, which is the same in almost all cases but not guaranteed.
- `objcmp.py` was checked against reccmp 0.1.7 on this code base (C, MSVC 5.0 SP3): 3303 of
  3304 functions give the same ratio and effective flag as the full run. It imports reccmp
  internals (`reccmp.compare.asm.parse.ParseAsm`, `Compare._db`, ...), which may move between
  reccmp versions.
- MSVC5's listing attributes the code of an inlined function to the line of the call, so the
  source-line view cannot look inside an inlined helper.
- `machelpers.py` decides "inlined" from a missing direct call, by name.
