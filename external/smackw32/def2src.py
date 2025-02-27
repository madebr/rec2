#!/usr/bin/env python

import argparse
import dataclasses
from pathlib import Path
import re
from typing import Optional

@dataclasses.dataclass
class Symbol:
    convention: str
    name: str
    stdcall_size: Optional[int]


def main():
    parser = argparse.ArgumentParser(allow_abbrev=False)
    parser.add_argument("--def", dest="def_path", type=Path, required=True)
    args = parser.parse_args()

    with args.def_path.open() as f:
        section = None
        symbols = []
        while line := f.readline():
            line = line.strip()
            if not line:
                continue
            line_parts = re.split("[ \t]+", line)
            if line_parts[0] == "NAME":
                section = None
                continue
            if line_parts[0] == "EXPORTS":
                section = "exports"
                continue

            if section == "exports":
                ordinal = None
                full_name = line_parts[0]
                for p in line_parts[1:]:
                    if p.startswith("@"):
                        ordinal = int(p[1:])
                        continue
                    raise ValueError(p)
                name_and_std = full_name.rsplit("@", 1)
                stdcall_size = None
                convention = "cdecl"
                if len(name_and_std) == 1:
                    name = full_name
                else:
                    name, stdcall_size = name_and_std
                    stdcall_size = int(stdcall_size, 10)
                    convention = "stdcall"

                symbols.append(Symbol(name=name, convention=convention, stdcall_size=stdcall_size))

        for s in symbols:
            if s.convention == "cdecl":
                assert s.stdcall_size is None
                print(f"void {s.name}(void) {{ }}")
            elif s.convention == "stdcall":
                assert s.stdcall_size is not None
                assert s.stdcall_size % 4 == 0
                args = ", ".join(f"int a{i}" for i in range(s.stdcall_size // 4))
                print(f"void {s.name}({args}) {{ }}")
            else:
                assert False




if __name__ == "__main__":
    raise SystemExit(main())
