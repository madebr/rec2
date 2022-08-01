try:
    from ghidra.ghidra_builtins import *
except:
    pass

from ghidra.app.util.cparser.CPP import PreProcessor
from ghidra.app.util.cparser.C import CParser

dtMgr = currentProgram.getDataTypeManager()

cpp = PreProcessor()
bos = java.io.ByteArrayOutputStream()
cpp.setArgs([
    "-D_SRE_",
    "-DC2_HOOKS_ENABLED",
    "-I/home/maarten/programming/rec2/scripts/include-sre",
    "-I/home/maarten/programming/rec2/src/hooks",
    "-I/home/maarten/programming/rec2/src/brender/include",
    "-I/home/maarten/programming/rec2/src/carma2/hooks",
    "-I/home/maarten/programming/rec2/src/carma2/common",
])
cpp.setOutputStream(bos)
cpp.parse("/home/maarten/programming/rec2/src/brender/include/brender/br_types.h")
cpp.parse("/home/maarten/programming/rec2/src/carma2/include/rec2_types.h")
cpp.parse("/home/maarten/programming/rec2/src/carma2/hooks/c2_time.h")
cpp.parse("/home/maarten/programming/rec2/scripts/include-sre/dinput.h")

cpp.getDefinitions().populateDefineEquates(dtMgr)

cParser = CParser(dtMgr, True, [dtMgr])
bis = java.io.ByteArrayInputStream(bos.toByteArray())
cParser.parse(bis)
