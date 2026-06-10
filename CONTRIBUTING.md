# Contributing to rec2

Documenting a complete game is no easy task. So every little help is much appreciated.

## Start documenting yourself

This projects uses [reccmp](https://github.com/isledecomp/reccmp).
This collection of tools allows writing equivalent C code for every function.

## Code conventions

- No hard tabs, only soft tabs. Indentation if 4 characters.
- An [.editorconfig](https://editorconfig.org/) file can be found in the root directory, which is able to configure most common IDE's.
- Explicitly add the call convention to every single function
- Call convention must be applied to function declaration in header and the function implementation
- The function implementation header (`int C2_HOOK_FASTCALL c2_function(int a1, int a2)`) must be 1 line. This limitation is imposed by the symbol extraction for creation of the Ghidra project.
- Follow the established conventions

## Your contribution

- Must be as faithful as possible to the binary code.
- Every single commit must build and work. This is a requirement to be able to use `git-bisect`.
- Prefer small commits and/or pull requests.
- Must build warning-free on MSVC and MinGW. If you don't know how to fix a warning, please ask for help on discord and/or open a pr with the WIP code.

# Hints

- When multiple functions are interdependent, consider adding them in multiple stages:
  1. In initial commits, create hooks for the functions that forward to the binary code
  2. In later commits, implement these functions.
- Pay attention to the x86 call convention of functions:
  `cdecl`, `fastcall` and `stdcall` are used in 99.9% of cases
- The Carmageddon 2 is baded on Carmageddon 1. So a lot of code is shared between these games. See [DethRace](https://github.com/dethrace-labs/dethrace) for inspiration.
