# Carmageddon 2, revisited

[![Discord Carmageddon server](https://badgen.net/badge/icon/discord?icon=discord&label)](https://discord.gg/f5StsuP)

This project attempts to document 1998's Carmageddon 2 by writing equivalent C code.

The goal is to create an implementation that works completely independent from the original game.

This will be done in two stages:
1. Matching the original: recreate the original executables as authentic as possible, duplicating as much as its behavior as possible, including bugs.
2. Add support for running the game on other platforms (desktop/mobile/console/...).
   We'd like rec2 to become the hub for all ports, so users don't have to hunt for ports in forks.

## Status

<a href="https://gist.githubusercontent.com/madebr/92b121cfd427c51b771fb614b4c893f4/raw/CARMA2_HW.html"><img width="50%" src="https://gist.githubusercontent.com/madebr/92b121cfd427c51b771fb614b4c893f4/raw/CARMA2_HW.svg"></a><a href="https://gist.githubusercontent.com/madebr/92b121cfd427c51b771fb614b4c893f4/raw/CARMA2_SW.html"><img width="50%" src="https://gist.githubusercontent.com/madebr/92b121cfd427c51b771fb614b4c893f4/raw/CARMA2_SW.svg"></a>

## Requirements

- Carmageddon 2 game data (for sale at [gog.com](https://www.gog.com/en/game/carmageddon_2_carpocalypse_now) and [steam](https://store.steampowered.com/app/282030/Carmageddon_2_Carpocalypse_Now/))
- Visual C++ 5.0 SP3 (part of Visual Studio 97)

## Build requirements

- 32-bit x86 compiler:
  - MSVC 11.0 (Visual Studio 97 SP3) compiler for matching build, or
  - a modern MinGW or MSVC toolchain
- DirectX SDK: we need `ddraw`, `dinput`, `dsound` and `dxguid`.
  - MinGW provides these libraries out-of-the box
  - MSVC does not ship with a *DirectX SDK*: this [time-period correct version on archive.org](https://archive.org/details/MicrosoftDirectX7SDK) works perfect.
- [libtiff](https://libtiff.gitlab.io/libtiff/): this library is used to convert tiff images to BRender pixelmaps
- [zlib](https://zlib.net/): there is limited support for compressed textures

## Goals

- Exact same behavior as the original
- Matching binary
- Every single commit should compile and work: this enables `git-bisect` to search for regressions
- Get the game running on most platforms

## License

rec2 is licensed under the [MPL-2.0 (Mozilla Public License 2.0)](https://www.mozilla.org/en-US/MPL/)

Contributors are required to complete the steps shown in [CONTRIBUTING.md](/CONTRIBUTING.md#legal) .

## Credits

- [Stainles Games](https://www.stainlessgames.com/): the creators of the Carmageddon series
- The [DethRace](https://github.com/dethrace-labs/dethrace/) project: Carmageddon 2 is a continuation of Carmageddon 1.
