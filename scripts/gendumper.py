#!/usr/bin/env python

import argparse
import contextlib
import io
import itertools
import pathlib
import pcpp
import pycparser
import pycparser.c_generator
import pycparser.c_ast
import re
import typing


class MyPreprocessor(pcpp.Preprocessor):
    pass


def tt(ff, *, includes=None, defines=None):

    p = MyPreprocessor()
    p.path = includes
    p.define("_SRE_")
    p.define("TYPEDEF_FILE")
    if defines:
        for define in defines:
            p.define(define)

    p.parse(ff)
    oh = io.StringIO()
    p.write(oh)
    parser = pycparser.CParser()
    # with open("/tmp/current.txt", "w") as f_debug:
    #     f_debug.write(oh.getvalue())
    t = parser.parse(oh.getvalue())
    return t


class TypeDb:
    def __init__(self):
        self.named_enums = {}
        self.named_structs = {}
        self.named_unions = {}
        self.typedefs: dict[pycparser.c_ast.Typedef] = {}
        self.dumpers = {}

    def get_top_typedef(self, name: str) -> str:
        while name in self.typedefs:
            typedef = self.typedefs[name]
            if not isinstance(typedef.type.type, pycparser.c_ast.IdentifierType):
                break
            names = list(typedef.type.type.names)
            if "const" in names:
                names.remove("const")
            if len(names) > 1:
                assert len(tuple(s for s in names if s not in ("signed", "unsigned"))) == 1
            name = " ".join(names)
        return name

    def find(self, name: str):
        typedef = self.typedefs[name]
        if typedef:
            return typedef.type.type
        struct = self.named_structs[name]
        if struct:
            return struct
        union = self.named_unions[name]
        if union:
            return union
        raise KeyError(name)


class TypeCollectorVisitor:
    def __init__(self, reduce_parentheses=False):
        self.reduce_parentheses = reduce_parentheses
        self.db = TypeDb()

    def visit(self, node):
        method = 'visit_' + node.__class__.__name__
        return getattr(self, method, self.generic_visit)(node)

    def generic_visit(self, node):
        if node is None:
            return ''
        else:
            for c_name, c in node.children():
                self.visit(c)

    def visit_Typedef(self, node):
        if node.name in self.db.typedefs:
            other = self.db.typedefs[node.name]
            if other.type.type.decls:
                raise KeyError(node.name)
        self.db.typedefs[node.name] = node
        self.generic_visit(node)

    def visit_Union(self, node):
        if node.name is not None:
            add = True
            if node.name in self.db.named_unions:
                other = self.db.named_unions[node.name]
                if other.decls:
                    add = False
            if add:
                self.db.named_unions[node.name] = node
        self.generic_visit(node)

    def visit_Struct(self, node):
        if node.name is not None:
            add = True
            if node.name in self.db.named_structs:
                other = self.db.named_structs[node.name]
                if other.decls:
                    add = False
            if add:
                self.db.named_structs[node.name] = node
        self.generic_visit(node)

    def visit_Enum(self, node):
        if node.name is not None:
            if node.name in self.db.named_enums:
                other = self.db.named_enums[node.name]
                if other.type.type.decls:
                    raise KeyError(node.name)
            self.db.named_enums[node.name] = node
        self.generic_visit(node)


class DumpedPending:
    def __init__(self, node, /, typename: str, funcname: str, body: typing.Optional[str]):
        self.node = node
        self.typename = typename
        self.funcname = funcname
        self.body = body

    @property
    def deref(self) ->bool:
        match type(self.node):
            case pycparser.c_ast.Struct | pycparser.c_ast.Union:
                return True
            case pycparser.c_ast.Enum:
                return False

class Key:
    def __init__(self, /, fmt: str, args: str):
        self.fmt = fmt
        self.args = args

    def add_string_literal(self, v: str):
        return Key(fmt=self.fmt+v, args=self.args)

    def add_string_variable(self, v: str):
        if self.args:
            new_args = self.args
        else:
            new_args = ", "
        new_args = self.args
        new_args += ", " + v
        return Key(fmt=self.fmt + "%s", args=new_args)

    @classmethod
    def create_literal(cls, v: str):
        return cls(fmt=v, args="")

    @classmethod
    def create_variable(cls, v: str):
        return cls(fmt="%s", args=v)


class AbstractWriter:
    def __init__(self):
        pass

    @contextlib.contextmanager
    def dump_function(self, /, name: str, typename: str, varname: str, pointer: bool):
        raise NotImplementedError

    def call_dumper_function(self, funcname: str, varname: str, deref: bool):
        raise NotImplementedError

    def dump_sint(self, key: Key, varname: str):
        raise NotImplementedError

    def dump_uint(self, key: Key, varname: str):
        raise NotImplementedError

    def dump_sshort(self, key: Key, varname: str):
        raise NotImplementedError

    def dump_ushort(self, key: Key, varname: str):
        raise NotImplementedError

    def dump_schar(self, key: Key, varname: str):
        raise NotImplementedError

    def dump_uchar(self, key: Key, varname: str):
        raise NotImplementedError

    def dump_float(self, key: Key, varname: str):
        raise NotImplementedError

    def dump_double(self, key: Key, varname: str):
        raise NotImplementedError

    def emit_pure_comment(self, comment: str):
        raise NotImplementedError


class DumperGenerator:
    def __init__(self, db: TypeDb, skips: list[str]):
        self.db = db
        self.dumped: [DumpedPending] = []
        self.pending: [DumpedPending] = []
        self.skips = list(skips)

    def create_dumper_struct(self, structname: str) -> typing.Union[DumpedPending]:
        node = self.db.named_structs[structname]
        typename = f"struct {structname}"
        for dp in itertools.chain(self.dumped, self.pending):
            if dp.typename == typename:
                return dp
            if dp.node == node:
                return dp

        assert isinstance(node, pycparser.c_ast.Struct)

        p = DumpedPending(node, typename=typename, funcname=f"dump_struct_{structname}", body=None)
        self.pending.append(p)
        return p

    def create_dumper_typename(self, typename: str) -> typing.Union[DumpedPending]:
        node = self.db.find(typename)
        for dp in itertools.chain(self.dumped, self.pending):
            if dp.typename == typename:
                return dp
            if dp.node == node:
                return dp

        if isinstance(node, pycparser.c_ast.TypeDecl):
            raise KeyError
        assert isinstance(node, (pycparser.c_ast.Struct, pycparser.c_ast.Union, pycparser.c_ast.Enum))

        p = DumpedPending(node, typename=typename, funcname=f"dump_{typename}", body=None)
        self.pending.append(p)
        return p

    def generate(self, writer: AbstractWriter):
        dumper = Dumper(self, writer=writer)
        while self.pending:
            prev_pending, self.pending = self.pending, []
            self.dumped.extend(prev_pending)
            for p in prev_pending:
                skip = p.typename in self.skips
                match type(p.node):
                    case pycparser.c_ast.Struct:
                        with writer.dump_function(name=p.funcname, typename=p.typename, varname="s", pointer=True, skip=skip):
                            dumper.dump_body_Struct(p.node, varname="s", pointer=True)
                    case pycparser.c_ast.Union:
                        with writer.dump_function(name=p.funcname, typename=p.typename, varname="e", pointer=True, skip=skip):
                            dumper.dump_body_Union(p.node, varname="e", pointer=True)
                    case pycparser.c_ast.Enum:
                        with writer.dump_function(name=p.funcname, typename=p.typename, varname="e", pointer=False, skip=skip):
                            dumper.dump_body_Enum(p.node, varname="e")


class Dumper:
    def __init__(self, /, dumpgen: DumperGenerator, writer: AbstractWriter):
        self.dumpgen = dumpgen
        self.writer = writer

    def dump_body_Struct(self, struct: pycparser.c_ast.Struct, /, varname: str, pointer: bool):
        assert isinstance(struct, pycparser.c_ast.Struct)
        if pointer:
            var_prefix = f"{varname}->"
        else:
            var_prefix = f"{varname}."
        if struct.decls:
            for decl in struct.decls:
                self._dump_item(decl.type, varname=f"{var_prefix}{decl.name}", key=Key.create_literal(decl.name))

    def dump_body_Union(self, union: pycparser.c_ast.Union, /, varname: str, pointer: bool):
        assert isinstance(union, pycparser.c_ast.Union)
        if pointer:
            var_prefix = f"{varname}->"
        else:
            var_prefix = f"{varname}."
        if union.decls:
            with self.writer.switch(-1) as switch:  # FIXME: correct varname
                for i, decl in enumerate(union.decls):
                    with switch.case(case=f"{i}"):  # FIXME: correct case value
                        self._dump_item(decl.type, varname=f"{var_prefix}{decl.name}", key=Key.create_literal(decl.name))
                with switch.default():
                    self.writer.emit_printf("<Unknown type>(%d)", "-1")  # FIXME: correct default

    def _dump_IdentifierType(self, node: pycparser.c_ast.IdentifierType, /, key: Key, varname: str):
        assert isinstance(node, pycparser.c_ast.IdentifierType)

        names = list(n for n in node.names if n not in ("const", ))
        id_type_name = " ".join(names)
        top_id_type_name = self.dumpgen.db.get_top_typedef(id_type_name)
        match top_id_type_name:
            case "int" | "signed int":
                self.writer.dump_sint(key, varname)
            case "unsigned int":
                self.writer.dump_uint(key, varname)
            case "short" | "signed short":
                self.writer.dump_sshort(key, varname)
            case "unsigned short":
                self.writer.dump_ushort(key, varname)
            case "char" | "signed char":
                self.writer.dump_schar(key, varname)
            case "unsigned char":
                self.writer.dump_uchar(key, varname)
            case "float":
                self.writer.dump_float(key, varname)
            case "double":
                self.writer.dump_double(key, varname)
            case _:
                try:
                    f = self.dumpgen.db.find(id_type_name)
                except KeyError:
                    self.writer.emit_printf(f"{key.fmt}: unknown type\\n", key.args)
                    return
                try:
                    dp = self.dumpgen.create_dumper_typename(id_type_name)
                except KeyError:
                    self.writer.emit_printf(f"{key.fmt}: unknown type\\n", key.args)
                    return
                if isinstance(f, (pycparser.c_ast.Struct, pycparser.c_ast.Union)):
                    if f.decls is None:
                        return
                self._call_dumper(dp, funcname=dp.funcname, varname=varname, deref=dp.deref)

    def _call_dumper(self, dp: DumpedPending, /, funcname: str, varname: str, deref: bool):
        self.writer.call_dumper_function(funcname=funcname, varname=varname, deref=deref)

    def _dump_ArrayDecl(self, node: pycparser.c_ast.ArrayDecl, key: Key, varname: str):
        with self.writer.array(key=key, dim=node.dim.value, varname=varname) as array:
            self._dump_item(node.type, key=array.key, varname=array.varname)

    def _dump_PtrDecl(self, node: pycparser.c_ast.TypeDecl, key: Key, varname: str):
        with self.writer.ifelse() as ifelse:
            with ifelse.if_(f"{varname} != NULL"):
                self.writer.dump_not_null(key=key)
                newkey = key.add_string_literal('[0]')
                self._dump_item(node.type, key=newkey, varname=varname + "[0]")
            with ifelse.else_():
                self.writer.dump_null(key=key)

    def _dump_TypeDecl(self, node: pycparser.c_ast.TypeDecl, key: Key, varname: str):
        assert isinstance(node, pycparser.c_ast.TypeDecl)
        assert isinstance(node.type, (pycparser.c_ast.IdentifierType, pycparser.c_ast.Struct, pycparser.c_ast.Union))

        match type(node.type):
            case pycparser.c_ast.IdentifierType:
                self._dump_IdentifierType(node.type, key=key, varname=varname)
            case pycparser.c_ast.Struct:
                ns = node.type
                if not ns.decls and ns.name:
                    ns = self.dumpgen.db.named_structs[node.type.name]
                if ns.name:
                    dp = self.dumpgen.create_dumper_struct(ns.name)
                    self.writer.emit_printf("", key=key)
                    self._call_dumper(dp, varname=varname, funcname=dp.funcname, deref=dp.deref)
                else:
                    self.dump_body_Struct(ns, varname=varname, pointer=False)
            case pycparser.c_ast.Union:
                nu = node.type
                if not nu.decls and nu.name:
                    nu = self.dumpgen.db.named_unions[node.type.name]
                if nu.name:
                    dp = self.dumpgen.create_dumper_struct(nu.name)
                    self._call_dumper(dp, varname=varname, funcname=dp.funcname, deref=dp.deref)
                else:
                    self.dump_body_Union(nu, varname=varname, pointer=False)
            case _:
                raise ValueError

    def _dump_item(self, node, /, key: Key, varname: str):
        if not isinstance(node, (pycparser.c_ast.TypeDecl, pycparser.c_ast.PtrDecl, pycparser.c_ast.ArrayDecl)):
            return
        assert isinstance(node, (pycparser.c_ast.TypeDecl, pycparser.c_ast.PtrDecl, pycparser.c_ast.ArrayDecl))
        match type(node):
            case pycparser.c_ast.TypeDecl:
                self._dump_TypeDecl(node, key=key, varname=varname)
            case pycparser.c_ast.PtrDecl:
                self._dump_PtrDecl(node, key=key, varname=varname)
            case pycparser.c_ast.ArrayDecl:
                self._dump_ArrayDecl(node, key=key, varname=varname)

    def dump_body_Enum(self, node: pycparser.c_ast.Enum, /, varname: str):
        with self.writer.ifelse() as ifelse:
            with ifelse.if_("0"):
                pass
            for value in node.values:
                with ifelse.elif_(f"{varname} == {value.name}"):
                    self.writer.emit_printf(f"<{value.name}>(%d)", "e")
            with ifelse.else_():
                self.writer.emit_printf("<UNKNOWN>(%d)", varname)


class TxtWriter(AbstractWriter):
    ARRAY_INDICES = [chr(c) for c in range(ord("i"), ord("z") + 1)] + [chr(c) for c in range(ord("a"), ord("i") + 1)]

    def __init__(self):
        super().__init__()
        self._indentation = 0
        self._array_depth = 0
        self._fc = io.StringIO()
        self._fh = io.StringIO()

    @contextlib.contextmanager
    def for_loop(self, dim: str):
        index = self.ARRAY_INDICES[self._array_depth]
        self.cprint(f"{self._indent_str}for (unsigned int {index} = 0; {index} < {dim}; {index}++) {{")
        self._array_depth += 1
        with self.indent():
            yield index
        self._array_depth -= 1
        self.cprint(f"{self._indent_str}}}")

    @contextlib.contextmanager
    def indent(self):
        self._indentation += 1
        yield
        self._indentation -= 1

    @contextlib.contextmanager
    def dump_function(self, name: str, typename: str, varname: str, pointer: bool, skip: bool):
        maybe_asterisk = "*" if pointer else ""
        self.hprint(f"\nvoid {name}(FILE *file, const {typename} {maybe_asterisk}{varname}, unsigned int depth, unsigned int max_depth);")
        if skip:
            name += "_SKIP"
        self.cprint(f"\nvoid {name}(FILE *file, const {typename} {maybe_asterisk}{varname}, unsigned int depth, unsigned int max_depth) {{")
        with self.indent():
            self.cprint(f"char spaces[256];")
            self.cprint("memset(spaces, ' ', sizeof(spaces));")
            self.cprint(r"//spaces[depth] = '\0';")
            self.cprint(r"spaces[0] = '\0';")
            with self.ifelse() as ifelse:
                with ifelse.if_("depth == max_depth"):
                    self.cprint(f"{self._indent_str}return;")
            self.emit_printf(f"({typename}) {{\\n")
            with self.indent():
                yield
            self.emit_printf(f"}}\\n")
        self.cprint(f"{self._indent_str}}}")

    @contextlib.contextmanager
    def switch(self, /, varname: str):
        with self.indent():
            class SwitchManager:
                def __init__(self, writer: TxtWriter):
                    self._default = False
                    self._writer = writer

                @contextlib.contextmanager
                def case(self, /, case: str):
                    self._writer.cprint(f"{self._writer._indent_str}case {case}:")
                    with self._writer.indent():
                        yield
                        self._writer.cprint(f"{self._writer._indent_str}break;")

                @contextlib.contextmanager
                def default(self):
                    self._writer.cprint(f"{self._writer._indent_str}default:")
                    with self._writer.indent():
                        yield
                        self._writer.cprint(f"{self._writer._indent_str}break;")
                    self._default = True
        self.cprint(f"{self._indent_str}switch ({varname}) {{")
        with self.indent():
            yield SwitchManager(self)
        self.cprint(f"{self._indent_str}}}")

    @contextlib.contextmanager
    def array(self, /, varname: str, key: Key, dim: int):
        class ArrayManager:
            def __init__(self, index: str, key: Key, varname: str):
                self.index = index
                self.key = key
                self.varname = varname
        with self.for_loop(str(dim)) as index:
            key_buffer_name = f"key_{index}"
            self.cprint(f"{self._indent_str}char {key_buffer_name}[256];")
            self.cprint(f"{self._indent_str}snprintf({key_buffer_name}, sizeof({key_buffer_name}), \"[%u]\", {index});")
            array_varname = f"{varname}[{index}]"
            yield ArrayManager(index=index, key=key.add_string_variable(key_buffer_name), varname=array_varname)

    @contextlib.contextmanager
    def ifelse(self):
        class IfElseCtx:
            def __init__(self, writer):
                self._writer = writer
                self._called = 0  # 0: nothing, 1: if, 2: elseif, 3: else

            @contextlib.contextmanager
            def if_(self, cond: str):
                assert self._called == 0
                self._called = 1
                self._writer.cprint(f"{self._writer._indent_str}if ({cond}) {{")
                with self._writer.indent():
                    yield
                self._writer.cprint(f"{self._writer._indent_str}}}")

            @contextlib.contextmanager
            def elif_(self, cond: str):
                assert self._called in (1, 2)
                self._called = 2
                self._writer.cprint(f"{self._writer._indent_str}else if ({cond}) {{")
                with self._writer.indent():
                    yield
                self._writer.cprint(f"{self._writer._indent_str}}}")

            @contextlib.contextmanager
            def else_(self):
                assert self._called in (1, 2)
                self._called = 3
                self._writer.cprint(f"{self._writer._indent_str}else {{")
                with self._writer.indent():
                    yield
                self._writer.cprint(f"{self._writer._indent_str}}}")
        yield IfElseCtx(self)

    def emit_printf(self, fmt, *args, key: Key=None):
        args_str = ", ".join(args)
        if key:
            fmt += key.fmt
            args_str = key.args + args_str
        if args_str and not args_str.startswith(", "):
            args_str = ", " + args_str
        self.cprint(f"{self._indent_str}fprintf(file, \"%s{fmt}\", spaces{args_str});")

    def call_dumper_function(self, funcname: str, varname: str, deref: bool):
        maybe_ampersand = "&" if deref else ""
        self.cprint(f"{self._indent_str}{funcname}(file, {maybe_ampersand}{varname}, depth + 1, max_depth);")

    @property
    def _indent_str(self):
        return self._indentation * " "

    def dump_sint(self, key: Key, varname: str):
        self.cprint(f"{self._indent_str}fprintf(file, \"%s.{key.fmt} = %d\\n\"{key.args}, spaces, {varname});")

    def dump_uint(self, key: Key, varname: str):
        self.cprint(f"{self._indent_str}fprintf(file, \"%s.{key.fmt} = %u\\n\"{key.args}, spaces, {varname});")

    def dump_sshort(self, key: Key, varname: str):
        self.cprint(f"{self._indent_str}fprintf(file, \"%s.{key.fmt} = %hd\\n\"{key.args}, spaces, {varname});")

    def dump_ushort(self, key: Key, varname: str):
        self.cprint(f"{self._indent_str}fprintf(file, \"%s.{key.fmt} = %hu\\n\"{key.args}, spaces, {varname});")

    def dump_schar(self, key: Key, varname: str):
        self.cprint(f"{self._indent_str}fprintf(file, \"%s.{key.fmt} = %d\\n\"{key.args}, spaces, {varname});")

    def dump_uchar(self, key: Key, varname: str):
        self.cprint(f"{self._indent_str}fprintf(file, \"%s.{key.fmt} = %u\\n\"{key.args}, spaces, {varname});")

    def dump_float(self, key: Key, varname: str):
        self.cprint(f"{self._indent_str}fprintf(file, \"%s.{key.fmt} = %g\\n\"{key.args}, spaces, {varname});")

    def dump_double(self, key: Key, varname: str):
        self.cprint(f"{self._indent_str}fprintf(file, \"%s.{key.fmt} = %g\\n\"{key.fmt}, spaces, {varname});")

    def dump_null(self, key: Key):
        self.cprint(f"{self._indent_str}fprintf(file, \"%s.{key.fmt} = NULL\\n\", spaces{key.args});")

    def dump_not_null(self, key: Key):
        self.cprint(f"{self._indent_str}fprintf(file, \"%s.{key.fmt} != NULL\\n\", spaces{key.args});")

    def emit_pure_comment(self, comment: str):
        for line in comment.splitlines(keepends=False):
            self.cprint(f"{self._indent_str}/* {line} */")

    def cprint(self, *args):
        print(*args, file=self._fc)

    def hprint(self, *args):
        print(*args, file=self._fh)


def guardify_string(s: str) -> str:
    t, _ = re.subn("[^A-Z]", "_", s.upper())
    return t


def main():
    parser = argparse.ArgumentParser(allow_abbrev=False)
    parser.add_argument("inputs", metavar="FILE", nargs="+", help="Input file(s)")
    parser.add_argument("-I", metavar="INCLUDE", dest="includes", action="append", default=[], help="Input directories")
    parser.add_argument("-D", metavar="DEFINE", dest="defines", action="append", default=[], help="Defines")
    parser.add_argument("-g", dest="hguard", metavar="GUARD", help="Header guard")
    parser.add_argument("-j", dest="houtput", metavar="OUTPUT", help="Output header file")
    parser.add_argument("-o", dest="output", metavar="OUTPUT", help="Output file")
    parser.add_argument("--dump-typedefs", dest="typedefs", nargs="+", default=[], metavar="TYPEDEF", help="Generate dumpers for typedefs")
    parser.add_argument("--skip-typedefs", dest="skip_typedefs", nargs="+", default=[], metavar="TYPEDEF", help="Skip dumpers for typedefs")
    parser.add_argument("--dump-structs", dest="structs", nargs="+", default=[], metavar="STRUCT", help="Generate dumpers for structs")

    args = parser.parse_args()

    stream_in = io.StringIO()
    for f in args.inputs:
        stream_in.write(f"#include \"{f}\"\n")
    stream_in.seek(0)
    stream_in.name = "<stdin>"
    tree = tt(stream_in, includes=args.includes, defines=args.defines)

    # with open("/tmp/tree.txt", "wt") as ff:
    #     print(tree, file=ff)
    walker = TypeCollectorVisitor()
    walker.visit(tree)

    db = walker.db
    dg = DumperGenerator(db=db, skips=args.skip_typedefs)
    for td in args.typedefs:
        dg.create_dumper_typename(td)
    for s in args.structs:
        dg.create_dumper_struct(s)

    txt_writer = TxtWriter()
    dg.generate(writer=txt_writer)

    if args.houtput:
        hguard = args.hguard if args.hguard else guardify_string(pathlib.Path(args.houtput).name)
        with open(args.houtput, "w") as f:
            f.write(f"#ifndef {hguard}\n")
            f.write(f"#define {hguard}\n")
            f.write(f"\n")
            for h in args.inputs:
                f.write(f"#include \"{h}\"\n")
            f.write(txt_writer._fh.getvalue())
            f.write(f"\n#endif /* {hguard} */\n")

    if args.output:
        with open(args.output, "w") as f:
            if args.houtput:
                f.write(f"#include \"{pathlib.Path(args.houtput).name}\"\n\n")
            f.write("#include <stdio.h>\n")
            f.write("#include <stdlib.h>\n")
            f.write("#include <string.h>\n")
            f.write(txt_writer._fc.getvalue())


if __name__ == "__main__":
    raise SystemExit(main())
