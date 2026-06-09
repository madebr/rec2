#!/usr/bin/env python

import argparse
import binascii
import pathlib
import textwrap


parser = argparse.ArgumentParser(allow_abbrev=False)
enc_dec = parser.add_mutually_exclusive_group()
enc_dec.add_argument("-D", dest="encode", action="store_false", help="Decode file")
enc_dec.add_argument("-E", dest="encode", action="store_true", help="Encode file")
parser.set_defaults(encode=False)
parser.add_argument("-i", dest="input", metavar="INPUT", type=pathlib.Path, required=True, help="Input path")
parser.add_argument("-o", dest="output", metavar="OUTPUT", type=pathlib.Path, required=True, help="Output path")
args = parser.parse_args()

if args.encode:
    binary_data = args.input.read_bytes()
    b64text = binascii.b2a_base64(binary_data).decode()
    with args.output.open("w") as f:
        for line in textwrap.wrap(b64text, width=160, break_long_words=True):
            f.write(line)
            f.write("\n")
else:
    with args.input.open("r") as f:
        b64text = "".join(f.readlines())
    binary_data = binascii.a2b_base64(b64text)
    with args.output.open("wb") as f:
        f.write(binary_data)
