#include "fwsetup.h"

#include "core/fw/brlists.h"
#include "core/fw/brprintf.h"
#include "core/fw/brqsort.h"
#include "core/fw/bswap.h"
#include "core/fw/devlist.h"
#include "core/fw/devsetup.h"
#include "core/fw/diag.h"
#include "core/fw/epiprolog.h"
#include "core/fw/error.h"
#include "core/fw/file.h"
#include "core/fw/image.h"
#include "core/fw/lexer.h"
#include "core/fw/mem.h"
#include "core/fw/object.h"
#include "core/fw/objectc.h"
#include "core/fw/pattern.h"
#include "core/fw/register.h"
#include "core/fw/resource.h"
#include "core/fw/resreg.h"
#include "core/fw/scratch.h"
#include "core/fw/tokenval.h"
#include "core/fw/token.h"
#include "core/std/brexcept.h"
#include "core/std/brstddiag.h"
#include "core/std/brstdfile.h"
#include "core/std/brstdlib.h"
#include "core/std/brstdmem.h"
#include "core/v1db/sys_conf.h"

#define HOOK_FW 1

C2_HOOK_VARIABLE_IMPLEMENT(br_framework_state, fw, 0x0079f4e0);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_resource_class, fw_resourceClasses, 25, 0x00665518, {
    { 0u, "REGISTRY", BR_MEMORY_REGISTRY, NULL, 0u },
    { 0u, "ANCHOR", BR_MEMORY_ANCHOR, NULL, 0u },
    { 0u, "RESOURCE_CLASS", BR_MEMORY_RESOURCE_CLASS, NULL, 0u },
    { 0u, "SCRATCH,", BR_MEMORY_SCRATCH, NULL, 16u },
    { 0u, "STRING", BR_MEMORY_STRING, NULL, 1u },
    { 0u, "FILE", BR_MEMORY_FILE, &_BrFileFree, 0u },
    { 0u, "POOL", BR_MEMORY_POOL, NULL, 16u },
    { 0u, "EXCEPTION_HANDLER", BR_MEMORY_EXCEPTION_HANDLER, NULL, 0u },
    { 0u, "TOKEN", BR_MEMORY_TOKEN, NULL, 0u },
    { 0u, "TOKEN_MAP", BR_MEMORY_TOKEN_MAP, NULL, 8u },
    { 0u, "DATAFILE", BR_MEMORY_DATAFILE, NULL, 0u },
    { 0u, "LEXER", BR_MEMORY_LEXER, NULL, 0u },
    { 0u, "OBJECT_LIST", BR_MEMORY_OBJECT_LIST, NULL, 0u },
    { 0u, "OBJECT_LIST_ENTRY", BR_MEMORY_OBJECT_LIST_ENTRY, NULL, 0u },
    { 0u, "APPLICATION", BR_MEMORY_APPLICATION, NULL, 0u },
    { 0u, "IMAGE", BR_MEMORY_IMAGE, &_BrImageFree, 0u },
    { 0u, "IMAGE_ARENA", BR_MEMORY_IMAGE_ARENA, NULL, 0u },
    { 0u, "IMAGE_SECTIONS", BR_MEMORY_IMAGE_SECTIONS, NULL, 0u },
    { 0u, "IMAGE_NAMES", BR_MEMORY_IMAGE_NAMES, NULL, 0u },
    { 0u, "OBJECT", BR_MEMORY_OBJECT, &_BrObjectFree, 16u },
    { 0u, "OBJECT_DATA", BR_MEMORY_OBJECT_DATA, NULL, 16u },
    { 0u, "DRIVER", BR_MEMORY_DRIVER, NULL, 0u },
    { 0u, "FMT_RESULTS", BR_MEMORY_FMT_RESULTS, NULL, 0u },
    { 0u, "TOKEN_VALUE", BR_MEMORY_TOKEN_VALUE, NULL, 0u },
    { 0u, "TOKEN_TEMPLATE", BR_MEMORY_TOKEN_TEMPLATE, NULL, 0u },
});

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(void*, functionPointers_BRCORE1, 185, 0x00665b70, {
    BrAbort,
    BrAddHead,
    BrAddTail,
    BrDevAdd,
    BrDevAddStatic,
    BrDevBegin,
    BrDevBeginVar,
    BrDevCheckAdd,
    BrDevContainedCount,
    BrDevContainedFind,
    BrDevContainedFindMany,
    BrDevCount,
    BrDevFind,
    BrDevFindMany,
    BrDevLastBeginQuery,
    BrDevLastBeginSet,
    BrDevRemove,
    BrFailure,
    BrFatal,
    BrFileAdvance,
    BrFileAttributes,
    BrFileClose,
    BrFileEof,
    BrFileGetChar,
    BrFileGetLine,
    BrFileOpenRead,
    BrFileOpenWrite,
    BrFilePrintf,
    BrFilePutChar,
    BrFilePutLine,
    BrFileRead,
    BrFileWrite,
    BrGetEnv,
    BrImageAdd,
    BrImageDereference,
    BrImageLookupName,
    BrImageLookupOrdinal,
    BrImageReference,
    BrImageRemove,
    BrInsert,
    BrIsAlpha,
    BrIsDigit,
    BrIsPrint,
    BrIsSpace,
    BrLastErrorGet,
    BrLastErrorSet,
    BrLexerAllocate,
    BrLexerCommentSet,
    BrLexerDumpSet,
    BrLexerErrorSet,
    BrLexerFree,
    BrLexerPop,
    BrLexerPosition,
    BrLexerPushFile,
    BrLexerPushString,
    BrLexerTokenError,
    BrLogPrintf,
    BrMemAllocate,
    BrMemCalloc,
    BrMemCmp,
    BrMemCpy,
    BrMemFree,
    BrMemInquire,
    BrMemSet,
    BrMemStrDup,
    BrNamePatternMatch,
    BrNewList,
    BrObjectContainerFree,
    BrObjectListAllocate,
    BrParseFixed,
    BrParseFloat,
    BrParseInteger,
    BrParseMatrixFixed,
    BrParseMatrixFloat,
    BrParseVectorFixed,
    BrParseVectorFloat,
    BrPrimitiveLibraryFind,
    BrQsort,
    BrRegistryAdd,
    BrRegistryAddMany,
    BrRegistryAddStatic,
    BrRegistryClear,
    BrRegistryCount,
    BrRegistryEnum,
    BrRegistryFind,
    BrRegistryFindMany,
    BrRegistryNew,
    BrRegistryNewStatic,
    BrRegistryRemove,
    BrRegistryRemoveMany,
    BrRegistryRemoveStatic,
    BrRemHead,
    BrRemTail,
    BrRemove,
    BrRendererFacilityFind,
    BrResAdd,
    BrResAllocate,
    BrResCheck,
    BrResChildEnum,
    BrResClass,
    BrResDump,
    BrResFree,
    BrResFreeNoCallback,
    BrResIsChild,
    BrResRemove,
    BrResSize,
    BrResSizeTotal,
    BrResStrDup,
    BrSScanf,
    BrScratchAllocate,
    BrScratchFlush,
    BrScratchFree,
    BrScratchInquire,
    BrScratchString,
    BrScratchStringSize,
    BrSimpleAddHead,
    BrSimpleInsert,
    BrSimpleNewList,
    BrSimpleRemHead,
    BrSimpleRemove,
    BrSprintf,
    BrSprintfN,
    BrStrCat,
    BrStrCmp,
    BrStrCpy,
    BrStrICmp,
    BrStrLen,
    BrStrNCmp,
    BrStrNCpy,
    BrStrNICmp,
    BrStrRChr,
    BrStrToD,
    BrStrToF,
    BrStrToL,
    BrStrToUL,
    BrStringToTokenValue,
    BrSwap16,
    BrSwap32,
    BrSwapBlock,
    BrSwapFloat,
    BrTVTemplateAllocate,
    BrTVTemplateFree,
    BrTokenCount,
    BrTokenCreate,
    BrTokenFind,
    BrTokenFindMany,
    BrTokenIdentifier,
    BrTokenType,
    BrTokenValueCompare,
    BrTokenValueDump,
    BrTokenValueQuery,
    BrTokenValueQueryAll,
    BrTokenValueQueryAllSize,
    BrTokenValueQueryMany,
    BrTokenValueQueryManySize,
    BrTokenValueSet,
    BrTokenValueSetMany,
    BrVSScanf,
    BrVSprintf,
    BrVSprintfN,
    BrWarning,
    _BrAssert,
    _BrExceptionBegin,
    _BrExceptionEnd,
    _BrExceptionResource,
    _BrExceptionThrow,
    _BrExceptionValueFetch,
    _BrUAssert,
    _EPI,
    _M_br_object_container_addFront,
    _M_br_object_container_count,
    _M_br_object_container_find,
    _M_br_object_container_findMany,
    _M_br_object_container_remove,
    _M_br_object_container_removeFront,
    _M_br_object_container_tokensMatch,
    _M_br_object_container_tokensMatchBegin,
    _M_br_object_container_tokensMatchEnd,
    _M_br_object_query,
    _M_br_object_queryAll,
    _M_br_object_queryAllSize,
    _M_br_object_queryBuffer,
    _M_br_object_queryMany,
    _M_br_object_queryManySize,
    _PRO
});

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, namePointers_BRCORE1, 185, 0x00665710, {
    "_BrAbort",
    "_BrAddHead",
    "_BrAddTail",
    "_BrDevAdd",
    "_BrDevAddStatic",
    "_BrDevBegin",
    "_BrDevBeginVar",
    "_BrDevCheckAdd",
    "_BrDevContainedCount",
    "_BrDevContainedFind",
    "_BrDevContainedFindMany",
    "_BrDevCount",
    "_BrDevFind",
    "_BrDevFindMany",
    "_BrDevLastBeginQuery",
    "_BrDevLastBeginSet",
    "_BrDevRemove",
    "_BrFailure",
    "_BrFatal",
    "_BrFileAdvance",
    "_BrFileAttributes",
    "_BrFileClose",
    "_BrFileEof",
    "_BrFileGetChar",
    "_BrFileGetLine",
    "_BrFileOpenRead",
    "_BrFileOpenWrite",
    "_BrFilePrintf",
    "_BrFilePutChar",
    "_BrFilePutLine",
    "_BrFileRead",
    "_BrFileWrite",
    "_BrGetEnv",
    "_BrImageAdd",
    "_BrImageDereference",
    "_BrImageLookupName",
    "_BrImageLookupOrdinal",
    "_BrImageReference",
    "_BrImageRemove",
    "_BrInsert",
    "_BrIsAlpha",
    "_BrIsDigit",
    "_BrIsPrint",
    "_BrIsSpace",
    "_BrLastErrorGet",
    "_BrLastErrorSet",
    "_BrLexerAllocate",
    "_BrLexerCommentSet",
    "_BrLexerDumpSet",
    "_BrLexerErrorSet",
    "_BrLexerFree",
    "_BrLexerPop",
    "_BrLexerPosition",
    "_BrLexerPushFile",
    "_BrLexerPushString",
    "_BrLexerTokenError",
    "_BrLogPrintf",
    "_BrMemAllocate",
    "_BrMemCalloc",
    "_BrMemCmp",
    "_BrMemCpy",
    "_BrMemFree",
    "_BrMemInquire",
    "_BrMemSet",
    "_BrMemStrDup",
    "_BrNamePatternMatch",
    "_BrNewList",
    "_BrObjectContainerFree",
    "_BrObjectListAllocate",
    "_BrParseFixed",
    "_BrParseFloat",
    "_BrParseInteger",
    "_BrParseMatrixFixed",
    "_BrParseMatrixFloat",
    "_BrParseVectorFixed",
    "_BrParseVectorFloat",
    "_BrPrimitiveLibraryFind",
    "_BrQsort",
    "_BrRegistryAdd",
    "_BrRegistryAddMany",
    "_BrRegistryAddStatic",
    "_BrRegistryClear",
    "_BrRegistryCount",
    "_BrRegistryEnum",
    "_BrRegistryFind",
    "_BrRegistryFindMany",
    "_BrRegistryNew",
    "_BrRegistryNewStatic",
    "_BrRegistryRemove",
    "_BrRegistryRemoveMany",
    "_BrRegistryRemoveStatic",
    "_BrRemHead",
    "_BrRemTail",
    "_BrRemove",
    "_BrRendererFacilityFind",
    "_BrResAdd",
    "_BrResAllocate",
    "_BrResCheck",
    "_BrResChildEnum",
    "_BrResClass",
    "_BrResDump",
    "_BrResFree",
    "_BrResFreeNoCallback",
    "_BrResIsChild",
    "_BrResRemove",
    "_BrResSize",
    "_BrResSizeTotal",
    "_BrResStrDup",
    "_BrSScanf",
    "_BrScratchAllocate",
    "_BrScratchFlush",
    "_BrScratchFree",
    "_BrScratchInquire",
    "_BrScratchString",
    "_BrScratchStringSize",
    "_BrSimpleAddHead",
    "_BrSimpleInsert",
    "_BrSimpleNewList",
    "_BrSimpleRemHead",
    "_BrSimpleRemove",
    "_BrSprintf",
    "_BrSprintfN",
    "_BrStrCat",
    "_BrStrCmp",
    "_BrStrCpy",
    "_BrStrICmp",
    "_BrStrLen",
    "_BrStrNCmp",
    "_BrStrNCpy",
    "_BrStrNICmp",
    "_BrStrRChr",
    "_BrStrToD",
    "_BrStrToF",
    "_BrStrToL",
    "_BrStrToUL",
    "_BrStringToTokenValue",
    "_BrSwap16",
    "_BrSwap32",
    "_BrSwapBlock",
    "_BrSwapFloat",
    "_BrTVTemplateAllocate",
    "_BrTVTemplateFree",
    "_BrTokenCount",
    "_BrTokenCreate",
    "_BrTokenFind",
    "_BrTokenFindMany",
    "_BrTokenIdentifier",
    "_BrTokenType",
    "_BrTokenValueCompare",
    "_BrTokenValueDump",
    "_BrTokenValueQuery",
    "_BrTokenValueQueryAll",
    "_BrTokenValueQueryAllSize",
    "_BrTokenValueQueryMany",
    "_BrTokenValueQueryManySize",
    "_BrTokenValueSet",
    "_BrTokenValueSetMany",
    "_BrVSScanf",
    "_BrVSprintf",
    "_BrVSprintfN",
    "_BrWarning",
    "__BrAssert",
    "__BrExceptionBegin",
    "__BrExceptionEnd",
    "__BrExceptionResource",
    "__BrExceptionThrow",
    "__BrExceptionValueFetch",
    "__BrUAssert",
    "__EPI",
    "__M_br_object_container_addFront",
    "__M_br_object_container_count",
    "__M_br_object_container_find",
    "__M_br_object_container_findMany",
    "__M_br_object_container_remove",
    "__M_br_object_container_removeFront",
    "__M_br_object_container_tokensMatch",
    "__M_br_object_container_tokensMatchBegin",
    "__M_br_object_container_tokensMatchEnd",
    "__M_br_object_query",
    "__M_br_object_queryAll",
    "__M_br_object_queryAllSize",
    "__M_br_object_queryBuffer",
    "__M_br_object_queryMany",
    "__M_br_object_queryManySize",
    "__PRO"
});

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_uint_16, nameOrdinals_BRCORE1, 185, 0x006659f8, {
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
    10,
    11,
    12,
    13,
    14,
    15,
    16,
    17,
    18,
    19,
    20,
    21,
    22,
    23,
    24,
    25,
    26,
    27,
    28,
    29,
    30,
    31,
    32,
    33,
    34,
    35,
    36,
    37,
    38,
    39,
    40,
    41,
    42,
    43,
    44,
    45,
    46,
    47,
    48,
    49,
    50,
    51,
    52,
    53,
    54,
    55,
    56,
    57,
    58,
    59,
    60,
    61,
    62,
    63,
    64,
    65,
    66,
    67,
    68,
    69,
    70,
    71,
    72,
    73,
    74,
    75,
    76,
    77,
    78,
    79,
    80,
    81,
    82,
    83,
    84,
    85,
    86,
    87,
    88,
    89,
    90,
    91,
    92,
    93,
    94,
    95,
    96,
    97,
    98,
    99,
    100,
    101,
    102,
    103,
    104,
    105,
    106,
    107,
    108,
    109,
    110,
    111,
    112,
    113,
    114,
    115,
    116,
    117,
    118,
    119,
    120,
    121,
    122,
    123,
    124,
    125,
    126,
    127,
    128,
    129,
    130,
    131,
    132,
    133,
    134,
    135,
    136,
    137,
    138,
    139,
    140,
    141,
    142,
    143,
    144,
    145,
    146,
    147,
    148,
    149,
    150,
    151,
    152,
    153,
    154,
    155,
    156,
    157,
    158,
    159,
    160,
    161,
    162,
    163,
    164,
    165,
    166,
    167,
    168,
    169,
    170,
    171,
    172,
    173,
    174,
    175,
    176,
    177,
    178,
    179,
    180,
    181,
    182,
    183,
    184
});

C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_image, Image_BRCORE1, 0x00665e58, {
    { 0 },
    "BRCORE1",
    2,
    0,
    1,
    BR_ASIZE(C2V(functionPointers_BRCORE1)),
    C2V(functionPointers_BRCORE1),
    BR_ASIZE(C2V(namePointers_BRCORE1)),
    C2V(namePointers_BRCORE1),
    C2V(nameOrdinals_BRCORE1),
    0,
    NULL,
    0,
    NULL,
    NULL
});

#define NBR_DEV_SLOTS 16

br_error (C2_HOOK_CDECL * BrFwBegin_original)(void);
br_error C2_HOOK_CDECL BrFwBegin(void) {
#if 0//HOOK_FW
    return BrFwBegin_original();
#else
    int i;

    if (C2V(fw).active) {
        return 0x1007;
    }

    if (C2V(fw).diag == NULL) {
        C2V(fw).diag = C2V(_BrDefaultDiagHandler);
    }
    if (C2V(fw).fsys == NULL) {
        C2V(fw).fsys = C2V(_BrDefaultFilesystem);
    }
    if (C2V(fw).mem == NULL) {
        C2V(fw).mem = C2V(_BrDefaultAllocator);
    }
    C2V(fw).open_mode = BR_FS_MODE_BINARY;
    BrRegistryNew(&C2V(fw).reg_resource_classes);

    C2V(fw).resource_class_index[BR_MEMORY_REGISTRY] = &C2V(fw_resourceClasses)[0];
    C2V(fw).resource_class_index[BR_MEMORY_ANCHOR] = &C2V(fw_resourceClasses)[1];
    C2V(fw).res = BrResAllocate(NULL, 0, BR_MEMORY_ANCHOR);
    for (i = 0; i < BR_ASIZE(C2V(fw_resourceClasses)); i++) {
        BrResClassAdd(&C2V(fw_resourceClasses)[i]);
    }
    BrNewList(&C2V(fw).images);
    C2V(fw).dev_slots = (br_open_device*)BrResAllocate(C2V(fw).res, sizeof(br_open_device) * NBR_DEV_SLOTS, BR_MEMORY_OBJECT_DATA);
    C2V(fw).ndev_slots = NBR_DEV_SLOTS;
    C2V(fw).active = 1;
    BrTokenBegin();
    BrSystemConfigBegin();
    C2V(fw).bAlreadyLoadedDrivers = 0;
#if !defined(REC2_STANDALONE)
    BrImageAdd(&C2V(Image_BRCORE1));
#endif

    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00527e20, BrFwBegin, BrFwBegin_original)

br_error (C2_HOOK_CDECL * BrFwEnd_original)(void);
br_error C2_HOOK_CDECL BrFwEnd(void) {
#if 0//HOOK_FW
    return BrFwEnd_original();
#else

    if (!C2V(fw).active) {
        return 0x1006;
    }
#if !defined(REC2_STANDALONE)
    BrImageRemove(&C2V(Image_BRCORE1));
#endif
    BrResFree(C2V(fw).res);
    BrMemSet(&C2V(fw), 0, sizeof(C2V(fw)));
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00527f20, BrFwEnd, BrFwEnd_original)

br_diaghandler* C2_HOOK_CDECL BrDiagHandlerSet(br_diaghandler* newdh) {
    br_diaghandler* old;

    old = C2V(fw).diag;
    if (newdh == NULL) {
        newdh = C2V(_BrDefaultDiagHandler);
    }
    C2V(fw).diag = newdh;
    return old;
}
C2_HOOK_FUNCTION(0x00527f70, BrDiagHandlerSet)

br_filesystem* (C2_HOOK_CDECL * BrFilesystemSet_original)(br_filesystem* newfs);
br_filesystem* C2_HOOK_CDECL BrFilesystemSet(br_filesystem* newfs) {
    br_filesystem* old;

    old = C2V(fw).fsys;
    if (newfs == NULL) {
        newfs = C2V(_BrDefaultFilesystem);
    }
    C2V(fw).fsys = newfs;
    return old;
}
C2_HOOK_FUNCTION_ORIGINAL(0x00527f90, BrFilesystemSet, BrFilesystemSet_original)

br_allocator* (C2_HOOK_CDECL * BrAllocatorSet_original)(br_allocator* newal);
br_allocator* C2_HOOK_CDECL BrAllocatorSet(br_allocator* newal) {
    br_allocator* old;

    old = C2V(fw).mem;
    if (newal == NULL) {
        newal = C2V(_BrDefaultAllocator);
    }
    C2V(fw).mem = newal;

    return old;
}
C2_HOOK_FUNCTION_ORIGINAL(0x00527fb0, BrAllocatorSet, BrAllocatorSet_original)
