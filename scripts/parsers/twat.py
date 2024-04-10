#!/usr/bin/env python

import argparse
import pathlib
import struct
import sys
import typing


class TwatFile:
    def __init__(self, name: str, data: bytes, start: int):
        self.name = name
        self.data = data
        self.start = start


class TwatVfs:
    def __init__(self, files: typing.Iterable[TwatFile]):
        self.files = list(file for file in files)

    def get_file(self, name: str) -> TwatFile:
        for file in self.files:
            if file.name.lower() == name.lower():
                return file
        raise KeyError(name)

    @classmethod
    def from_file(cls, inputfile: typing.IO, *, fatal=True) -> "TwatVfs":
        def warn(message: str):
            print(message, file=sys.stderr)
            if fatal:
                raise IOError(message)

        rawdata = inputfile.read()

        file_size, count_files = struct.unpack_from("<II", rawdata, offset=0)

        if file_size != len(rawdata):
            warn(f"Header correct: header says size={file_size} but actual size is {len(rawdata)} bytes.")

        files = [{"name": None, "start": None, "data": None} for _ in range(count_files)]

        current_offset = 8 + count_files * 0x38
        for file_i in range(count_files):

            struct.unpack_from("<II", rawdata, offset=0)

            file_i_size, file_i_name, _ = struct.unpack_from("<I48sI", rawdata, offset=8 + file_i * 0x38)
            file_i_name = file_i_name[:file_i_name.index(0)].decode()

            if current_offset > file_size:
                warn(f"File {file_i_name} has an incorrect starting offset: it starts after the file size")

            files[file_i]["name"] = file_i_name
            files[file_i]["start"] = current_offset
            files[file_i]["data"] = rawdata[current_offset:current_offset+file_i_size]
            if len(files[file_i]["data"]) != file_i_size:
                warn(f"File {file_i_name} has incorrect size: header says size={file_i_size} but actual size is {len(files[file_i]['data'])} bytes.")
            current_offset += (file_i_size + 3) & ~3

        return cls(TwatFile(name=file["name"], data=file["data"], start=file["start"]) for file in files)

    @classmethod
    def from_path(cls, path: pathlib.Path) -> "TwatVfs":
        return cls.from_file(path.open("rb"))


def print_listing(twat_vfs: TwatVfs) -> None:

    title = f"{'Name':<48} {'Size':>10} {'Start':>10}"
    print(title)
    print("=" * len(title))
    for file in twat_vfs.files:
        print(f"{file.name:<48} {len(file.data):>10x} {file.start:>10x}")


def main():
    parser = argparse.ArgumentParser(allow_abbrev=False)
    parser.add_argument("twat", metavar="TWAT", default=None, nargs="?", help="file to parse")

    mutex = parser.add_mutually_exclusive_group()
    mutex.add_argument("--file", metavar="FILE", default=None, help="file to read")

    args = parser.parse_args()

    inputfile = open(args.twat, "rb") if args.twat else sys.stdin.buffer

    try:
        twat_vfs = TwatVfs.from_file(inputfile)
    except IOError as e:
        return 1

    if args.file:
        try:
            twat_file = twat_vfs.get_file(args.file)
        except KeyError:
            print(f"Could not find '{args.file}'", file=sys.stderr)
            return 1
        sys.stdout.buffer.write(twat_file.data)
        return 0

    print_listing(twat_vfs)

    return 0

if __name__ == "__main__":
    raise SystemExit(main())
