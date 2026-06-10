# Carmageddon 2, revisited

[![Discord Carmageddon server](https://badgen.net/badge/icon/discord?icon=discord&label)](https://discord.gg/f5StsuP)

This project attempts to document 1998's Carmageddon 2 by writing equivalent C code.

When enough functions have been replaced, it should become possible to not need the retail binary anymore and let the game run on modern non-Windows systems.

## Status

<img width="50%" src="https://gist.githubusercontent.com/madebr/92b121cfd427c51b771fb614b4c893f4/raw/CARMA2_HW.svg">

## Requirements

- Carmageddon 2: this project needs the original game data function
- MSVC 5 SP3

## Build requirements

- 32-bit x86 compiler:
  - MSVC 11.0 (Visual Studio 97 SP3) compiler for matching build, or
  - a modern MinGW or MSVC toolchain
- DirectX SDK: we need `dinput` and `dxguid`.
  - MinGW provides these libraries out-of-the box
  - MSVC does not ship with a *DirectX SDK*: this [time-period correct version on archive.org](https://archive.org/details/MicrosoftDirectX7SDK) works perfect.
- [libtiff](http://www.libtiff.org/): this library is used to convert tiff images to BRender pixelmaps

## Goals

- Exact same behavior as the original
- Matching binary
- Every single commit should compile and work: this enables `git-bisect` to search for regressions

## Credits

- The [DethRace](https://github.com/dethrace-labs/dethrace/) project: Carmageddon 2 is a continuation of Carmageddon 1.
