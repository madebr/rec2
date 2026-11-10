# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#
# Copyright 2022-2026 rec2 developers

"""objcmp.py BUILD OBJ FUNC [0xADDR] [--diff] : reccmp's function comparison on one COFF object.

Prototype of "compare a single object file": takes one function out of an MSVC .obj (bytes and
relocations, nothing linked) and runs it through reccmp's own code - ParseAsm for the
disassembly and name substitution, SequenceMatcherWithPins/find_effective_match for the ratio
and the effective-match test - against the original executable at the annotated address.
Prints reccmp's ratio for the function; --diff also prints the differing hunks.

How the unlinked object is fed to reccmp
  reccmp's comparison works on a function's bytes plus two callbacks: "is this value an
  address?" and "what is the name of this address?". For a linked image both are answered from
  the relocation table and the entity database (PDB + annotations). An object has neither
  addresses nor a PDB, so this script performs the one step of linking that the comparison
  needs, in memory, for this one function:
    * the function is placed at a base address outside the image;
    * every relocation of its section is resolved to an address and patched into the bytes
      (DIR32: address + addend, REL32: address + addend - next instruction);
    * a symbol the entity database knows by its decorated name (BUILD's last link: functions,
      globals, imports, pooled strings) resolves to that entity's address, so reccmp's own
      lookup then yields exactly the name the full run prints;
    * a label of the function's own section (jump tables) resolves into the placed function;
    * any other symbol (the object's private float constants `$T..`, string literals `$SG..`,
      statics, and functions or globals that were not in the last link) gets a synthetic
      address. Its name is formed the way reccmp forms it: a float constant referenced by an
      x87 instruction is named by its value, a string by its text, a function or global by its
      undecorated symbol name; anything else stays an address without a name and becomes one
      of reccmp's numbered placeholders.
  The original side is untouched reccmp: same entity database, same sanitizer.

BUILD must hold a finished MSVC5 build (reccmp-build.yml, rec2_hw.exe, rec2_hw.pdb): its link is
the source of the symbol -> name table. OBJ may come from that build or from a later compile
of one file (fastmatch.py); the function does not need to exist in the last link, and nothing
is linked again. ADDR defaults to the address annotated for FUNC.

Used as a module: Scorer(cfg).score(obj_path, func, orig_addr) -> dict(ratio, effective, ...).
Needs the python environment that has reccmp (and its capstone) installed.
"""
import os
import re
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import rec2cfg  # noqa: E402

SYNTH_BASE = 0x7D000000   # where the function is placed
SYNTH_DATA = 0x7E000000   # synthetic addresses of symbols the last link does not know


class Coff:
    """Just enough of a COFF object: sections, symbols, relocations."""

    def __init__(self, path):
        d = self.d = open(path, 'rb').read()
        machine, nsec, _ts, symptr, nsym, opt, _ch = struct.unpack_from('<HHIIIHH', d, 0)
        assert machine == 0x14c, 'not an i386 COFF object'
        strtab = symptr + 18 * nsym
        self.sections = []
        for i in range(nsec):
            o = 20 + opt + 40 * i
            name = d[o:o + 8].rstrip(b'\0').decode('latin-1')
            vsize, va, rsize, raw, relptr, _ln, nrel, _nln, ch = struct.unpack_from('<IIIIIIHHI', d, o + 8)
            self.sections.append({'name': name, 'size': rsize, 'raw': raw, 'relptr': relptr, 'nrel': nrel, 'flags': ch})
        self.symbols = []
        i = 0
        while i < nsym:
            o = symptr + 18 * i
            if d[o:o + 4] == b'\0\0\0\0':
                off = struct.unpack_from('<I', d, o + 4)[0]
                name = d[strtab + off:d.index(b'\0', strtab + off)].decode('latin-1')
            else:
                name = d[o:o + 8].rstrip(b'\0').decode('latin-1')
            value, sec, typ, cls, naux = struct.unpack_from('<IhHBB', d, o + 8)
            self.symbols.append({'name': name, 'value': value, 'sec': sec, 'type': typ, 'cls': cls, 'index': i})
            for _ in range(naux):
                self.symbols.append(None)
            i += 1 + naux

    def data(self, sec):
        s = self.sections[sec - 1]
        if s['raw'] == 0:
            return b'\0' * s['size']  # .bss
        return self.d[s['raw']:s['raw'] + s['size']]

    def relocs(self, sec):
        s = self.sections[sec - 1]
        for i in range(s['nrel']):
            va, sym, typ = struct.unpack_from('<IIH', self.d, s['relptr'] + 10 * i)
            yield va, self.symbols[sym], typ

    def function(self, name):
        """(section number, offset, size) of the function with this undecorated name."""
        pat = re.compile(r'^(?:_%s|@%s@\d+|_%s@\d+)$' % (re.escape(name), re.escape(name), re.escape(name)))
        sym = next((s for s in self.symbols if s and s['sec'] > 0 and pat.match(s['name']) and self.sections[s['sec'] - 1]['flags'] & 0x20), None)
        if sym is None:
            raise KeyError('%s is not defined in this object' % name)
        sec = self.sections[sym['sec'] - 1]
        if sec['flags'] & 0x1000:  # COMDAT: the section is the function
            end = sec['size']
        else:
            later = [s['value'] for s in self.symbols if s and s['sec'] == sym['sec'] and s['value'] > sym['value'] and (s['type'] == 0x20 or s['cls'] == 2)]
            end = min(later) if later else sec['size']
        size = end - sym['value']
        fpo = self.fpo_size(sym)
        if fpo and fpo <= size:
            size = fpo  # exact length; the section itself is padded to its alignment
        else:
            body = self.data(sym['sec'])[sym['value']:end]
            size = len(body.rstrip(bytes([0x90, 0xcc]))) or size
        return sym['sec'], sym['value'], size, sym['name']

    def fpo_size(self, fsym):
        """Procedure size from the FPO record (.debug$F) that points at this function."""
        for n, s in enumerate(self.sections, 1):
            if s['name'] != '.debug$F' or s['size'] < 16:
                continue
            for va, sym, typ in self.relocs(n):
                if sym is not None and va % 16 == 0 and (sym is fsym or (sym['sec'] == fsym['sec'] and sym['value'] == fsym['value'])):
                    return struct.unpack_from('<I', self.data(n), va + 4)[0]
        return None


def undecorate(sym):
    m = re.match(r'^@(\w+)@\d+$', sym)
    if m:
        return m.group(1)
    return re.sub(r'@\d+$', '', sym[1:] if sym.startswith('_') else sym)


class Scorer:
    """Loads reccmp's comparison for BUILD once; score() then compares one object function."""

    def __init__(self, cfg):
        if isinstance(cfg, str):
            cfg = rec2cfg.resolve(cfg)
        self.cfg = cfg
        import logging
        from pathlib import Path
        logging.disable(logging.ERROR)  # the full run prints these; here they are noise
        from reccmp.project.detect import RecCmpProject
        from reccmp.compare import Compare
        from reccmp.compare.asm.parse import ParseAsm
        from reccmp.compare.asm.fixes import find_effective_match
        from reccmp.compare.pinned_sequences import SequenceMatcherWithPins
        from reccmp.compare.db import entity_name_from_string
        from reccmp.analysis.float_const import SINGLE_PRECISION_OPCODES, DOUBLE_PRECISION_OPCODES
        from reccmp.types import ImageId
        self.ImageId, self.ParseAsm = ImageId, ParseAsm
        self.find_effective_match, self.Matcher = find_effective_match, SequenceMatcherWithPins
        self.string_name = entity_name_from_string
        self.F32, self.F64 = SINGLE_PRECISION_OPCODES, DOUBLE_PRECISION_OPCODES
        cwd = os.getcwd()
        try:
            os.chdir(cfg['build'])
            project = RecCmpProject.from_directory(directory=Path.cwd())
            target = project.get(rec2cfg.TARGET)
            self.compare = Compare.from_target(target)
        finally:
            os.chdir(cwd)
        self.db = self.compare._db
        self.image_has_relocs = bool(self.compare.recomp_bin.relocations)
        self.fc = self.compare.function_comparator
        self.by_symbol = {}
        for e in self.db.all(ImageId.RECOMP):
            s = e.get('symbol')
            if s and e.recomp_addr is not None:
                self.by_symbol.setdefault(s, e.recomp_addr)
        # every public of the PDB, so that an alias (_stricmp for __strcmpi) resolves too
        for node in self.compare.cvdump_analysis.nodes:
            a = getattr(node, 'addr', None)
            if node.decorated_name and a is not None:
                self.by_symbol.setdefault(node.decorated_name, a)
        self.by_name = {}
        self.recomp_by_name = {}   # statics have a name in the PDB but no public symbol
        for e in self.db.get_all():
            if e.orig_addr is not None and e.name:
                self.by_name.setdefault(e.name, e.orig_addr)
            if e.recomp_addr is not None and e.name:
                self.recomp_by_name.setdefault(e.name, []).append((e.recomp_addr, e.any_size(ImageId.RECOMP)))

    def linked_values(self, orig_addr):
        """Every 4-byte value in the function as the last link has it (to tell apart statics
        of the same name: the one this function referred to then is the one it means now)."""
        m = self.db.get(self.ImageId.ORIG, orig_addr) if orig_addr is not None else None
        if m is None or m.recomp_addr is None:
            return set()
        try:
            raw = self.compare.recomp_bin.read(m.recomp_addr, m.any_size(self.ImageId.RECOMP) or 0)
        except Exception:
            return set()
        return {struct.unpack_from('<I', raw, i)[0] for i in range(max(0, len(raw) - 3))}

    def resolve_by_name(self, sym, orig_addr):
        """Address in the last link of a symbol that has no public there: a file-level static
        (`_name`), a function-level static (`_?name@?1??Func@@9@9$S123`), a string literal."""
        raw = re.sub(r'\$S\d+$', '', sym['name'])          # the compiler's static suffix
        raw = raw[1:] if raw.startswith('_') else raw
        m = re.match(r'^\?(\w+)@\?\w+\?\?\w+@@', raw)
        for name in ([raw, m.group(1)] if m else [raw]):
            cands = self.recomp_by_name.get(name, [])
            if len(cands) == 1:
                return cands[0][0]
            if len(cands) > 1:
                hit = self.referenced(cands, orig_addr)
                if len(hit) == 1:
                    return hit[0]
        return None

    def referenced(self, cands, orig_addr):
        """Those of the candidate (address, size) entities that the last link's version of the
        function refers to."""
        vals = self.linked_values(orig_addr)
        return [a for a, size in cands if any(a <= v < a + max(size, 1) for v in vals)]

    def orig_addr(self, func):
        a = self.by_name.get(func)
        if a is None:
            # not in the last link (reccmp names entities from the PDB): read the annotations
            a = rec2cfg.function_names(self.cfg)[1].get(func)
        if a is None:
            raise KeyError('no annotation found for %s' % func)
        return a

    def link_function(self, coff, func, orig_addr=None):
        """Place one function and resolve its relocations. Returns (base, bytes, names, values):
        names maps synthetic addresses to reccmp-style names, values is the set of patched-in
        addresses (they are addresses by construction: they come from relocations)."""
        sec, off, size, _sym = coff.function(func)
        code = bytearray(coff.data(sec)[off:off + size])
        base = SYNTH_BASE
        names, values, synth = {}, set(), {}
        next_data = [SYNTH_DATA]
        unresolved = []
        for va, sym, typ in coff.relocs(sec):
            if not (off <= va < off + size) or sym is None:
                continue
            p = va - off
            addend = struct.unpack_from('<i', code, p)[0]
            if sym['sec'] == sec and off <= sym['value'] < off + size and (sym['value'] != off or sym['type'] != 0x20) and sym['name'] not in self.by_symbol:
                target = base + sym['value'] - off          # a label inside this function
            elif sym['name'] in self.by_symbol:
                target = self.by_symbol[sym['name']]         # known to the last link
            elif self.resolve_private(coff, sym, orig_addr) is not None:
                target = self.resolve_private(coff, sym, orig_addr)
            else:
                key = (sym['name'], sym['sec'], sym['value'])
                if key not in synth:
                    synth[key] = next_data[0]
                    next_data[0] += 0x1000
                    nm = self.name_private(coff, sym, code, p)
                    if nm:
                        names[synth[key]] = nm
                    else:
                        unresolved.append(sym['name'])
                target = synth[key]
            if typ == 0x14:    # REL32
                value = (target + addend - (base + p + 4)) & 0xffffffff
                values.add((target + addend) & 0xffffffff)
            elif typ in (6, 7):  # DIR32 / DIR32NB
                value = (target + addend) & 0xffffffff
                values.add(value)
            else:
                raise ValueError('relocation type 0x%x is not handled' % typ)
            struct.pack_into('<I', code, p, value)
        return base, bytes(code), names, values, unresolved

    def resolve_private(self, coff, sym, orig_addr):
        """A symbol without a public in the last link that the link nevertheless has: statics by
        their PDB name, string literals by their text."""
        if sym['sec'] > 0 and (sym['name'].startswith('$SG') or sym['name'].startswith('??_C@')):
            data = coff.data(sym['sec'])[sym['value']:]
            end = data.find(b'\0')
            if end < 0:
                return None
            # String literals are not pooled: every object has its own copy and reccmp
            # recognises only some of them as strings. Take a copy only if the last link's
            # version of this function refers to it.
            c = self.recomp_by_name.get(self.string_name(data[:end].decode('latin-1')), [])
            hit = self.referenced(c, orig_addr) if c else []
            return hit[0] if hit else None
        if sym['name'].startswith('$') or sym['sec'] <= 0:
            return None
        return self.resolve_by_name(sym, orig_addr)

    def name_private(self, coff, sym, code, p):
        """Name of a symbol the last link does not know, formed as reccmp forms it."""
        if sym['sec'] > 0:
            s = coff.sections[sym['sec'] - 1]
            data = coff.data(sym['sec'])[sym['value']:]
            if s['flags'] & 0x20:
                return '%s (FUNCTION)' % undecorate(sym['name'])  # a static function of this object
            op = (code[p - 2], code[p - 1]) if p >= 2 else None
            # reccmp names a float constant only when the instruction's operand is a relocation
            # of the linked image; an image without a relocation table gets placeholders
            readonly = self.image_has_relocs and not s['flags'] & 0x80000000
            if readonly and op in self.F32 and len(data) >= 4:
                return '%s (FLOAT)' % str(struct.unpack_from('<f', data)[0])
            if readonly and op in self.F64 and len(data) >= 8:
                return '%s (FLOAT)' % str(struct.unpack_from('<d', data)[0])
            # a string the last link does not hold stays nameless: whether reccmp would
            # recognise it as a string depends on the linked image (resolve_private covers
            # the strings that it did recognise)
            if sym['cls'] == 2:
                return '%s (DATA)' % undecorate(sym['name'])
            return None  # private data without a name: reccmp would show a placeholder
        # undefined external that the last link did not have: a new function or global
        if sym['name'].startswith('__imp_'):
            return None
        kind = 'FUNCTION' if sym['type'] == 0x20 or sym['name'].startswith('@') else 'DATA'
        return '%s (%s)' % (undecorate(sym['name']), kind)

    def score(self, obj_path, func, orig_addr=None, want_diff=False):
        ImageId = self.ImageId
        coff = Coff(obj_path)
        if orig_addr is None:
            orig_addr = self.orig_addr(func)
        base, code, names, values, unresolved = self.link_function(coff, func, orig_addr)
        rs = self.fc.recomp_sanitize
        inner_test, inner_lookup = rs.addr_test, rs.name_lookup

        def addr_test(a):
            # as for a linked image: a relocated value (when the image keeps its relocation
            # table), or an address inside a known entity; the function itself is one
            if a in names or base <= a < base + len(code):
                return True
            return (self.image_has_relocs and a in values) or inner_test(a)

        def name_lookup(a, exact=False, indirect=False):
            if a in names:
                return None if indirect else names[a]
            if SYNTH_BASE <= a < SYNTH_DATA + 0x1000000:
                return None
            return inner_lookup(a, exact=exact, indirect=indirect)

        recomp = self.ParseAsm(addr_test=addr_test, name_lookup=name_lookup, is_32bit=True).parse_asm(code, base)
        # the original: exactly what FunctionComparator.compare_function does
        match = self.db.get(ImageId.ORIG, orig_addr)
        orig_size = match.size(ImageId.ORIG) if match is not None else None
        recomp_size = len(code)
        if orig_size is None:
            orig_max = match.max_size(ImageId.ORIG) if match is not None else None
            orig_size = min(orig_max, recomp_size) if orig_max is not None else recomp_size
        orig_raw = self.compare.orig_bin.read(orig_addr, orig_size)
        orig = self.fc.orig_sanitize.parse_asm(orig_raw, orig_addr)
        o, r = [x[1] for x in orig], [x[1] for x in recomp]
        diff = self.Matcher(o, r, [])
        ratio = diff.ratio()
        eff = self.find_effective_match(diff.get_opcodes(), o, r) if ratio != 1.0 else False
        res = {'ratio': ratio, 'effective': bool(eff), 'orig_n': len(o), 'recomp_n': len(r), 'recomp_size': recomp_size,
               'orig_size': orig_size, 'unresolved': unresolved}
        if want_diff:
            res['orig'], res['recomp'], res['opcodes'] = o, r, diff.get_opcodes()
        return res


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    if len(args) < 3:
        sys.exit(__doc__)
    if not rec2cfg.ensure_reccmp_python():
        sys.exit('reccmp is not importable; run this with the python that has reccmp installed (or set RECCMP_PYTHON)')
    sc = Scorer(args[0])
    for func in args[2].split(','):
        addr = int(args[3], 16) if len(args) > 3 else None
        r = sc.score(args[1], func, addr, want_diff='--diff' in sys.argv)
        print('%s: %.10f%s  (%d original / %d recompiled instructions, %d bytes%s)' % (
            func, r['ratio'], ' effective' if r['effective'] else '', r['orig_n'], r['recomp_n'], r['recomp_size'],
            '; private symbols without a name: %s' % ', '.join(sorted(set(r['unresolved']))) if r['unresolved'] else ''))
        if '--diff' in sys.argv:
            for tag, i1, i2, j1, j2 in r['opcodes']:
                if tag == 'equal':
                    continue
                print('@@ orig %d recomp %d' % (i1, j1))
                for x in r['orig'][i1:i2]:
                    print('  - ' + x)
                for x in r['recomp'][j1:j2]:
                    print('  + ' + x)


if __name__ == '__main__':
    main()
