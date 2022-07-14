# Contributing to rec2

Documenting a complete game is no easy task. So every little help is much appreciated.

## Code conventions


- No hard tabs, only soft tabs. Indentation if 4 characters.
- An [.editorconfig](https://editorconfig.org/) file can be found in the root directory, which is able to configure most common IDE's.
- Explicitly add the call convention to every single function
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


## Various

- This poject merges PR's using "rebase merge"
