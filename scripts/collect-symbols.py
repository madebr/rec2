#!/usr/bin/env python

import argparse
import collections
import json
import os
import pathlib
import re
import string
import sys
import typing

from sre_common import FunctionArgument
from sre_common import FunctionDetails
from sre_common import FunctionHookItem
from sre_common import VariableHookItem
from sre_common import VariableArrayHookItem

REC2_ROOT = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
REC2_SRC_ROOT = os.path.join(REC2_ROOT, "src")

RE_HOOK_FUNC = re.compile(r"(?:C2_HOOK_FUNCTION|C2_HOOK_FUNCTION_ORIGINAL|C2_NO_HOOK_FUNCTION)\((0x[a-fA-F0-9]{1,8})[ \t]*,[ \t]+([A-Za-z0-9_]+)", flags=re.M)
RE_HOOK_VAR = re.compile(r"(?:C2_HOOK_VARIABLE_IMPLEMENT|C2_HOOK_VARIABLE_IMPLEMENT_INIT)\(([a-zA-Z0-9_]+),[ \t]*([a-zA-Z0-9_]+)[ \t]*,[ \t]*(0x[a-fA-F0-9]{1,8})", flags=re.M)
RE_HOOK_VAR_ARR = re.compile(r"(?:C2_HOOK_VARIABLE_IMPLEMENT_ARRAY|C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT)\(([a-zA-Z0-9_ *]+),[ \t]*([a-zA-Z0-9_]+)[ \t]*,[ \t]*([0-9]+)[ \t]*,[ \t]*(0x[a-fA-F0-9]{1,8})", flags=re.M)


def to_int(thing):
    if isinstance(thing, int):
        return thing
    elif isinstance(thing, str):
        if thing[:2] == "0x":
            return int(thing[2:], 16)
        elif thing[0] == "0":
            return int(thing[1:], 8)
        else:
            return int(thing)
    else:
        raise ValueError("Invalid integer", thing)


class DuplicateItemException(Exception):
    def __init__(self, name1: str, addr1: int, name2:str, addr2: int):
        super().__init__(name1)
        self.name1 = name1
        self.addr1 = addr1
        self.name2 = name2
        self.addr2 = addr2


class HookDatabase:
    def __init__(self):
        self.db = {}

    def add(self, item):
        address = to_int(item.address)
        if address in self.db:
            raise DuplicateItemException(item.name, item.address, self.db[address].name, self.db[address].address)
        self.db[address] = item

class CallConvParser(object):
    def __init__(self, name: str, choices: tuple[str], remove: tuple[int]=()):
        self.name = name
        self.choices = choices
        re_text = f"(.*)([ \t]{'|'.join(self.choices)}[ \t])(.*)"
        self.re = re.compile(re_text)
        self.remove = remove

CALL_CONVENTIONS = (
    CallConvParser("fastcall", ("C2_HOOK_FASTCALL", "__fastcall")),
    CallConvParser("cdecl", ("C2_HOOK_CDECL", "__cdecl")),
    CallConvParser("stdcall", ("C2_HOOK_STDCALL", "__stdcall", "CALLBACK", "WINAPI", "APIENTRY")),
    CallConvParser("custom", ("C2_HOOK_CUSTOMCALL", )),
    CallConvParser("thiscall", ("C2_HOOK_THISCALL", )),
    CallConvParser("thiscall", ("C2_HOOK_FAKE_THISCALL", ), (1, )),
)

def analyze_funcdef(funcdef: str, warn: typing.Callable[[str], None]) -> FunctionDetails:
    callconv = None
    for convparser in CALL_CONVENTIONS:
        if convparser.re.match(funcdef) is not None:
            funcdef = convparser.re.subn(r"\1 \3", funcdef)[0]
            callconv = convparser.name
            remove = convparser.remove
            break
    else:
        warn(f"function '{funcdef}' has unknown call convention")

    noreturn = "C2_NORETURN" in funcdef
    vararg = False
    funcdef = funcdef.replace("C2_NORETURN", "")
    # funcdef = re.sub("(?:^| )const(?:$| )", "", funcdef)

    arguments_str = funcdef[funcdef.find("("):].strip()[1:-1].strip()
    if not arguments_str:
        warn(f"function '{funcdef}' accepts no arguments, but has no (void) declaration")
    if arguments_str == "void" or not arguments_str:
        arguments = []
    else:
        argumentstr_list = re.split("[ \t]*,[ \t]*", arguments_str)
        arguments = []
        if argumentstr_list and argumentstr_list[-1] == "...":
            argumentstr_list = argumentstr_list[:-1]
            vararg = True
        for argstr in argumentstr_list:
            argtype = argstr.rstrip(string.ascii_letters + string.digits + "_").strip()
            argname = argstr[len(argtype):].strip()
            arguments.append(FunctionArgument(type=argtype, name=argname))

    for r in sorted(convparser.remove, reverse=True):
        arguments = arguments[:r] + arguments[r+1:]

    ret_name_str = funcdef[:funcdef.find("(")].strip()
    ret_str, name_str = ret_name_str.rsplit(" ", 1)
    ret_str = ret_str.strip()
    name_str = name_str.strip()
    if callconv is None:
        callconv = "cdecl"
        warn(f"no call convention detected for {name_str} (found: \"{funcdef}\"). Assuming { callconv }.")
    return FunctionDetails(
        rettype=ret_str,
        callconv=callconv,
        name=name_str,
        arguments=arguments,
        noreturn=noreturn,
        vararg=vararg,
    )


def format_address(addr):
    if isinstance(addr, int):
        pass
    elif isinstance(addr, str):
        if addr[:2] == "0x":
            addr = int(addr[2:], 16)
        else:
            addr = int(addr)
    else:
        raise ValueError("invalid address", addr)
    return f"0x{addr:08x}"


def main() -> int:
    parser = argparse.ArgumentParser(allow_abbrev=False)
    parser.add_argument("-Werror", dest="werror", action="store_true", help="Treat warnings as errors")
    parser.add_argument("--output", "-o", dest="output", default=pathlib.Path("symbols.json"), type=pathlib.Path, help="Output file (default=symbols.json)")
    parser.add_argument("--summary", dest="summary", action="store_true", help="Print summary")
    args = parser.parse_args()

    class Namespace:
        def __init__(self):
            self.warning_happened = False
    ns = Namespace()

    def warn(message: str) -> None:
        print(f"Warning: { message }", file=sys.stderr)
        ns.warning_happened = True


    funcdb = HookDatabase()
    vardb = HookDatabase()
    vararrdb = HookDatabase()

    duplicates = []

    for folder, dirs, files in os.walk(REC2_SRC_ROOT):
        for file in files:
            base, ext = os.path.splitext(file)
            if ext in (".h", ".c"):
                full_fn = os.path.join(folder, file)
                source_text = open(full_fn).read()
                for funcaddress, funcname in RE_HOOK_FUNC.findall(source_text):
                    funcdefs = set(re.findall("^([a-zA-Z].*[ \t]" + funcname + "[ \t]*\\(.*\\))[ \t]*\\{[ \t]*$", source_text, flags=re.M))
                    if len(funcdefs) == 0:
                        warn(f"{ file }: could not find function definition for {funcname} in {full_fn}")
                        continue
                    elif len(funcdefs) > 1:
                        warn(f"{ file }: multiple function definitions found for {funcname} in {full_fn}")
                    funcdef = funcdefs.pop()
                    funcdetails = analyze_funcdef(funcdef, warn)

                    try:
                        funcdb.add(FunctionHookItem(to_int(funcaddress), funcname, funcdef, funcdetails))
                    except DuplicateItemException as e:
                        duplicates.append(e)

                for vartype, varname, varaddress in RE_HOOK_VAR.findall(source_text):
                    try:
                        vardb.add(VariableHookItem(to_int(varaddress), vartype, varname))
                    except DuplicateItemException as e:
                        duplicates.append(e)
                for arrtype, arrname, arrcount, arraddress in RE_HOOK_VAR_ARR.findall(source_text):
                    try:
                        vararrdb.add(VariableArrayHookItem(to_int(arraddress), arrtype, arrname, to_int(arrcount)))
                    except DuplicateItemException as e:
                        duplicates.append(e)

    if duplicates:
        warn(f"Found {len(duplicates)} duplicates:")
        for d in duplicates:
            warn(f"- {d.name1:<30}: 0x{d.addr1:08x} | {d.name2:<30}: 0x{d.addr2:08x}")


    json_data = json.dumps({
        "functions": [item.asdict() for item in funcdb.db.values()],
        "vars": [item._asdict() for item in vardb.db.values()],
        "vararrs": [item._asdict() for item in vararrdb.db.values()],
    })

    with open(args.output, "w") as f:
        f.write(json_data)

    if args.summary:
        print(f"rec2 hooks { len(funcdb.db) } function")
        print(f"rec2 hooks { len(vardb.db) } regular variables")
        print(f"rec2 hooks { len(vararrdb.db) } array variables")
        print("Functions:")
        for funcaddress in sorted(funcdb.db.keys()):
            hookinfo = funcdb.db[funcaddress]
            print(f"{format_address(hookinfo.address)} {hookinfo.name:<50} {hookinfo.funcdef}")
        print("Variables:")
        for varaddr in sorted(vardb.db.keys()):
            hookinfo = vardb.db[varaddr]
            print(f"{format_address(hookinfo.address)} {hookinfo.vartype:<30} {hookinfo.name}")
        print("Variable arrays:")
        for arraddr in sorted(vararrdb.db.keys()):
            hookinfo = vararrdb.db[arraddr]
            print(f"{format_address(hookinfo.address)} {hookinfo.vartype:<30} {hookinfo.name}[{hookinfo.varcount}]")

    if ns.warning_happened:
        print("Warnings treated as error. Exiting.", file=sys.stderr)
        raise SystemExit(1)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
