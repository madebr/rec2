# Contributing to rec2

Documenting a complete game is no easy task. So every little help is much appreciated.

## What executables are we targeting?

`CARMA2_HW.EXE` and `Carma2_SW.exe` v1.04 as provided by [Steam](https://store.steampowered.com/app/282030/Carmageddon_2_Carpocalypse_Now/) and [gog.com](https://www.gog.com/en/game/carmageddon_2_carpocalypse_now).
See [`reccmp-project.yml`](/reccmp-project.yml) for the sha256 hashes.

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
- Prefer small commits and/or pull requests. Ideally do one function per Pull Request.
- Must build warning-free on MSVC and MinGW. If you don't know how to fix a warning, please ask for help on discord and/or open a pr with the WIP code.

## Legal

To give the rec2 project full flexibility to choose whatever license is best to protect the project,
new contributors must add the following message in their commit message or pull request:
(**Replace `[full name or GitHub username]` and `[YYYY-MM-DD]` with your name and actual date**)
```
Contributor License Authorization

I grant madebr <anonymous.maarten@gmail.com> a perpetual, worldwide, non-exclusive, royalty-free, irrevocable license to use, reproduce, modify, distribute, sublicense, and relicense my contributions to rec2 under any license, including proprietary and open-source licenses, subject to applicable law.

I confirm that I have the right to grant these permissions for the contributions covered by this agreement, excluding third-party material and contributions subject to separate terms.

Date: [YYYY-MM-DD]

Name: [full name or GitHub username]
```


# Hints

- When multiple functions are interdependent, consider adding them in multiple stages:
  1. In initial commits, create hooks for the functions that forward to the binary code
  2. In later commits, implement these functions.
- Pay attention to the x86 call convention of functions:
  `cdecl`, `fastcall` and `stdcall` are used in 99.9% of cases
- The Carmageddon 2 is baded on Carmageddon 1. So a lot of code is shared between these games. See [DethRace](https://github.com/dethrace-labs/dethrace) for inspiration.
