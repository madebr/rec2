try:
    from ghidra.ghidra_builtins import *
except:
    pass

import argparse
import json
import os

from ghidra.program.model.listing import Function
from ghidra.program.model.listing.Function import FunctionUpdateType

from sre_common import FunctionHookItem
from sre_common import VariableHookItem
from sre_common import VariableArrayHookItem
from ghidra.program.model.listing import Parameter
from ghidra.program.model.data import ParameterDefinitionImpl
from ghidra.program.model.listing import ParameterImpl
from ghidra.program.model.symbol import SourceType
from ghidra.program.model.data import GenericCallingConvention
from ghidra.util.data import DataTypeParser
from ghidra.app.util.cparser.C import CParserUtils

raw_args = list(str(a) for a in getScriptArgs())
print "Raw arguments: {}".format(raw_args)

parser = argparse.ArgumentParser()
parser.add_argument("ghidra_root", help="path of ghidra root")
parser.add_argument("symbols", help="path of symbols JSON file")
args = parser.parse_args(raw_args)
print "Arguments: {}".format(args)

args = parser.parse_args(raw_args)

symbols = json.loads(open(args.symbols).read())

hookFuncs = [FunctionHookItem.fromdict(item) for item in symbols["functions"]]
hookVars = [VariableHookItem(**item) for item in symbols["vars"]]
hookVararrs = [VariableArrayHookItem(**item) for item in symbols["vararrs"]]

def callconv_converter(callconv):
    if callconv == "cdecl":
        return GenericCallingConvention.cdecl
    elif callconv == "stdcall":
        return GenericCallingConvention.stdcall
    elif callconv == "fastcall":
        return GenericCallingConvention.fastcall
    elif callconv == "custom":
        return Function.UNKNOWN_CALLING_CONVENTION_STRING
    else:
        raise ValueError("Invalid call convention: {}".format(callconv))

def clean_funcdef(funcdef):
    TO_REMOVE = (
        "C2_HOOK_CDECL",
        "C2_HOOK_STDCALL",
        "C2_HOOK_FASTCALL",
        "C2_NORETURN",
    )
    for tr in TO_REMOVE:
        funcdef = funcdef.replace(tr, "")
    return funcdef

generic_dtmanager = ghidra.program.model.data.FileDataTypeManager.openFileArchive(java.io.File(os.path.join(args.ghidra_root, "Ghidra/Features/Base/data/typeinfo/generic/generic_clib.gdt")), False)

GENCLIB_DTPARSER = DataTypeParser(
    generic_dtmanager,
    currentProgram.dataTypeManager,
    None,
    DataTypeParser.AllowedDataTypes.FIXED_LENGTH,
)

DTPARSER = DataTypeParser(
    currentProgram.dataTypeManager,
    currentProgram.dataTypeManager,
    None,
    DataTypeParser.AllowedDataTypes.FIXED_LENGTH,
)

currentProgram.dataTypeManager.addDataType(GENCLIB_DTPARSER.parse("FILE"), None)
currentProgram.dataTypeManager.addDataType(GENCLIB_DTPARSER.parse("time_t"), None)

# print("FILE -> ", DTPARSER.parse("FILE"))
# print("time_t -> ", DTPARSER.parse("time_t"))
# print("gdt size_t -> ", list(l for l in getDataTypes("size_t")))

def parse_type_string(type_str):
    type_str = type_str.replace("const ", "")
    type_str = type_str.replace("struct ", "")
    try:
        return DTPARSER.parse(type_str)
    except ghidra.program.model.data.InvalidDataTypeException:
        pass
    dataTypes = getDataTypes(type_str)
    if dataTypes:
        return dataTypes[0]
    ptr_depth = 0
    type_str_noptr = type_str
    if type_str[-1] == "*":
        ptr_depth += 1
        type_str_noptr = type_str_noptr[:-1]
    if ptr_depth != 0:
        result_type = parse_type_string(type_str_noptr)
        for _ in range(ptr_depth):
            result_type = currentProgram.dataTypeManager.getPointer(result_type)
        return result_type
    raise ValueError("Could not get data type for '{}'".format(type_str))


start()
for hookFunc in hookFuncs:
    func = getFunctionAt(toAddr(hookFunc.address))
    if func is None:
        func = createFunction(toAddr(hookFunc.address), hookFunc.name)
    if func is None:
        disassemble(toAddr(hookFunc.address))
        func = createFunction(toAddr(hookFunc.address), hookFunc.name)
    if func is None:
        print("Failed to create a Function for {} at 0x{:08x}".format(hookFunc.name, hookFunc.address))
        continue
    func.setCallingConvention(str(callconv_converter(hookFunc.details.callconv)))
    func.setSignatureSource(SourceType.USER_DEFINED)
    func.setName(hookFunc.name, SourceType.USER_DEFINED)
    func.setVarArgs(hookFunc.details.vararg)
    func.setNoReturn(hookFunc.details.noreturn)
    func.setReturnType(parse_type_string(hookFunc.details.rettype), SourceType.USER_DEFINED)

    args = []
    # vars = []
    for arg in hookFunc.details.arguments:
        # print(arg.name, arg.type.replace("const ", ""))
        # args.append(ParameterDefinitionImpl(arg.name, parse_type_string(arg.type.replace("const ", "")), ""))
        # vars.append
        args.append(ParameterImpl(arg.name, parse_type_string(arg.type), currentProgram))
    # print(args)
    func.replaceParameters(args, Function.FunctionUpdateType.DYNAMIC_STORAGE_FORMAL_PARAMS, False, SourceType.USER_DEFINED)

end(True)

start()
for hookVar in hookVars:
    # print(hookVar)
    # removeDataAt(toAddr(hookVar.address))
    hookVarDT = parse_type_string(hookVar.vartype)
    addressSet = createAddressSet()
    addressSet.addRange(toAddr(hookVar.address), toAddr(hookVar.address + hookVarDT.getLength()))
    clearListing(toAddr(hookVar.address), toAddr(hookVar.address + hookVarDT.getLength()))

    createLabel(toAddr(hookVar.address), hookVar.varname, True, SourceType.USER_DEFINED)
    data = createData(toAddr(hookVar.address), parse_type_string(hookVar.vartype))
    # print("data=", data)
    # funcdef_clean = clean_funcdef(hookFunc.funcdef)
    # print funcdef_clean, "->", CParserUtils.parseSignature(None, currentProgram, funcdef_clean)

    # print("return({}) -> {}".format(hookFunc.details.rettype, parse_type_string(hookFunc.details.rettype)))
    # postypes = list(l for l in getDataTypes(hookFunc.details.rettype))
    # print hookFunc.details.rettype, "->", postypes
    #
    # print hookFunc.details.rettype, "->", DTPARSER.parse(hookFunc.details.rettype)

    # rettype = hookFunc.details.rettype
    # rettype = rettype.replace("FILE", "_IO_FILE")
    # print rettype, "->", DTPARSER.parse(rettype)
    # func.setReturnType(SourceType.USER_DEFINED)
end(True)

start()
for hookVararr in hookVararrs:
    # print(hookVararr)
    # removeDataAt(toAddr(hookVararr.address))
    hookVarDT = parse_type_string(hookVararr.vartype)
    addressSet = createAddressSet()
    addressSet.addRange(toAddr(hookVararr.address), toAddr(hookVararr.address + hookVararr.varcount * hookVarDT.getLength()))
    # print("clear {}:{}".format(toAddr(hookVararr.address), toAddr(hookVararr.address + hookVararr.varcount * hookVarDT.getLength())))
    clearListing(toAddr(hookVararr.address), toAddr(hookVararr.address + hookVararr.varcount * hookVarDT.getLength()))

    createLabel(toAddr(hookVararr.address), hookVararr.varname, True, SourceType.USER_DEFINED)
    cmd = ghidra.app.cmd.data.CreateArrayCmd(toAddr(hookVararr.address), hookVararr.varcount, hookVarDT, hookVarDT.getLength())
    cmd.applyTo(currentProgram)
    msg = cmd.getStatusMsg()
    if msg is not None:
        raise ValueError(msg)

    # funcdef_clean = clean_funcdef(hookFunc.funcdef)
    # print funcdef_clean, "->", CParserUtils.parseSignature(None, currentProgram, funcdef_clean)

    # print("return({}) -> {}".format(hookFunc.details.rettype, parse_type_string(hookFunc.details.rettype)))
    # postypes = list(l for l in getDataTypes(hookFunc.details.rettype))
    # print hookFunc.details.rettype, "->", postypes
    #
    # print hookFunc.details.rettype, "->", DTPARSER.parse(hookFunc.details.rettype)

    # rettype = hookFunc.details.rettype
    # rettype = rettype.replace("FILE", "_IO_FILE")
    # print rettype, "->", DTPARSER.parse(rettype)
    # func.setReturnType(SourceType.USER_DEFINED)
end(True)
