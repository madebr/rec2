# Carmageddon 2, revisited

[![Discord Carmageddon server](https://badgen.net/badge/icon/discord?icon=discord&label)](https://discord.gg/f5StsuP)

This project attempts to documents 1998's Carmageddon 2 by replacing binary code in the released binary with C code.

When enough functions have been replaced, it should become possible to not need the retail binary anymore and let the game run on modern non-Windows systems.

## Status

- This project is still in its infancy, so nothing major has been achieved yet
- The hook dll works
- The standalone exe does not work, and won't even compile

## Requirements

- Carmageddon 2: this project still needs the original game data + executable(s) to function
- Windows

## Build requirements

- 32-bit x86 compiler: MinGW and MSVC are supported
- DirectX SDK: we need `dinput` and `dxguid`.
  - MinGW provides these libraries out-of-the box
  - MSVC does not ship with a *DirectX SDK*: this [time-period correct version on archive.org](https://archive.org/details/MicrosoftDirectX7SDK) works perfect.
- [Detours](https://github.com/microsoft/Detours): this library is used to hook functions

## Goals

- Hook as much functions as possible until we don't even need the original executable anymore
- Exact same behavior as the original
- Every single commit should compile and work: this enables `git-bisect` to search for regressions

## Credits

- The [DethRace](https://github.com/dethrace-labs/dethrace/) project: Carmageddon 2 is a continuation of Carmageddon 1.
