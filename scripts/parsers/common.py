import io
import pathlib
import re
import string
import sys
import typing

import twat


def open_data_file(path: typing.Optional[pathlib.Path], *, text: bool=True) -> typing.IO:
    if path is None:
        return sys.stdin if text else sys.stdin.buffer

    try:
        return path.open("r" if text else "rb")
    except FileNotFoundError:
        pass

    twatvfs_path = path.parent.with_suffix(".twt")
    twat_vfs = twat.TwatVfs.from_path(twatvfs_path)

    twat_file = twat_vfs.get_file(path.name)

    return io.StringIO(twat_file.data.decode()) if text else io.BytesIO(twat_file.data)


text_characters = string.digits + string.ascii_letters + "*-+.!&}{~(\\\""


class TextReader:
    def __init__(self, file: typing.IO):
        self.file = file

    def read_line(self) -> str:
        while 1:
            line = self.file.readline()
            if line == "":
                raise EOFError
            line = line.rstrip()
            if not line:
                continue
            start = 0
            if line[0] not in text_characters:
                continue
            return line

    def read_line_parts(self) -> list[str]:
        line = self.read_line()
        line_comment = re.split(" // ", line, maxsplit=1)
        if len(line_comment) > 1:
            comment = line_comment[1]
            line = line_comment[0]
        else:
            comment = None
        parts = re.split("[ \t]+", line)
        if comment:
            parts.append(f"// {comment}")
        # print(parts)
        # return int(parts[0])
        return parts

    def read_int(self) -> int:
        parts = self.read_line_parts()
        return int(parts[0])

    def read_ints(self, count: int) -> tuple[int]:
        line = self.read_line()
        parts = re.split("[, \t]+", line)
        return tuple(int(parts[i]) for i in range(count))

    def read_string(self) -> int:
        parts = self.read_line_parts()
        return parts[0]
