#include "pmsetup.h"

#include "core/fw/image.h"
#include "core/fw/resource.h"
#include "core/fw/resreg.h"
#include "core/pixelmap/genclip.h"
#include "core/pixelmap/pmgen.h"
#include "core/pixelmap/pmmem.h"
#include "core/pixelmap/pmnull.h"
#include "core/std/brstdlib.h"

//#include "c2_stddef.h"
#include <stdio.h>


// GLOBAL: CARMA2_HW 0x0079f950
br_pixelmap_state _pixelmap;

// GLOBAL: CARMA2_HW 0x0058b968
br_resource_class pm_resourceClasses[] = {
    { 0u, "PIXELMAP", BR_MEMORY_PIXELMAP, NULL, 0u },
    { 0u, "PIXELS",     BR_MEMORY_PIXELS, NULL, 32u }
};

// GLOBAL: CARMA2_HW 0x0066dd30
void* functionPointers_BRPMAP1[] = {
    PixelmapCopyBitsClip,
    PixelmapLineClip,
    PixelmapPointClip,
    PixelmapRectangleClip,
    PixelmapRectangleClipTwo,
    _M_br_device_pixelmap_gen_copy,
    _M_br_device_pixelmap_gen_copyBits,
    _M_br_device_pixelmap_gen_copyDirty,
    _M_br_device_pixelmap_gen_copyFrom,
    _M_br_device_pixelmap_gen_copyFromDirty,
    _M_br_device_pixelmap_gen_copyTo,
    _M_br_device_pixelmap_gen_copyToDirty,
    _M_br_device_pixelmap_gen_directLock,
    _M_br_device_pixelmap_gen_directUnlock,
    _M_br_device_pixelmap_gen_doubleBuffer,
    _M_br_device_pixelmap_gen_doubleBufferDirty,
    _M_br_device_pixelmap_gen_fill,
    _M_br_device_pixelmap_gen_fillDirty,
    _M_br_device_pixelmap_gen_flush,
    _M_br_device_pixelmap_gen_line,
    _M_br_device_pixelmap_gen_rectangle,
    _M_br_device_pixelmap_gen_rectangle2,
    _M_br_device_pixelmap_gen_synchronise,
    _M_br_device_pixelmap_gen_text,
    _M_br_device_pixelmap_gen_textBounds,
    _M_br_device_pixelmap_mem_allocateSub,
    _M_br_device_pixelmap_mem_copyBits,
    _M_br_device_pixelmap_mem_copyDirty,
    _M_br_device_pixelmap_mem_copyFrom,
    _M_br_device_pixelmap_mem_copyFromDirty,
    _M_br_device_pixelmap_mem_copyTo,
    _M_br_device_pixelmap_mem_copyToDirty,
    _M_br_device_pixelmap_mem_directLock,
    _M_br_device_pixelmap_mem_directUnlock,
    _M_br_device_pixelmap_mem_doubleBuffer,
    _M_br_device_pixelmap_mem_doubleBufferDirty,
    _M_br_device_pixelmap_mem_fill,
    _M_br_device_pixelmap_mem_fillDirty,
    _M_br_device_pixelmap_mem_flush,
    _M_br_device_pixelmap_mem_line,
    _M_br_device_pixelmap_mem_match,
    _M_br_device_pixelmap_mem_originSet,
    _M_br_device_pixelmap_mem_pixelAddressQuery,
    _M_br_device_pixelmap_mem_pixelAddressSet,
    _M_br_device_pixelmap_mem_pixelQuery,
    _M_br_device_pixelmap_mem_pixelSet,
    _M_br_device_pixelmap_mem_rectangleCopyFrom,
    _M_br_device_pixelmap_mem_rectangleCopyTo,
    _M_br_device_pixelmap_mem_rectangleFill,
    _M_br_device_pixelmap_mem_rectangleStretchCopyFrom,
    _M_br_device_pixelmap_mem_rectangleStretchCopyTo,
    _M_br_device_pixelmap_mem_resize,
    _M_br_device_pixelmap_mem_rowQuery,
    _M_br_device_pixelmap_mem_rowSet,
    _M_br_device_pixelmap_mem_rowSize,
    _M_br_device_pixelmap_mem_synchronise,
    _M_br_device_pixelmap_mem_validSource,
    _M_br_device_pixelmap_null_copyBits,
    _M_br_device_pixelmap_null_copyDirty,
    _M_br_device_pixelmap_null_copyFrom,
    _M_br_device_pixelmap_null_copyFromDirty,
    _M_br_device_pixelmap_null_copyTo,
    _M_br_device_pixelmap_null_copyToDirty,
    _M_br_device_pixelmap_null_doubleBuffer,
    _M_br_device_pixelmap_null_doubleBufferDirty,
    _M_br_device_pixelmap_null_fill,
    _M_br_device_pixelmap_null_fillDirty,
    _M_br_device_pixelmap_null_pixelAddressQuery,
    _M_br_device_pixelmap_null_pixelAddressSet,
    _M_br_device_pixelmap_null_pixelQuery,
    _M_br_device_pixelmap_null_pixelSet,
    _M_br_device_pixelmap_null_rectangleCopyFrom,
    _M_br_device_pixelmap_null_rectangleCopyTo,
    _M_br_device_pixelmap_null_rectangleFill,
    _M_br_device_pixelmap_null_rectangleStretchCopyFrom,
    _M_br_device_pixelmap_null_rectangleStretchCopyTo,
    _M_br_device_pixelmap_null_resize
};

// GLOBAL: CARMA2_HW 0x0066db58
const char* namePointers_BRPMAP1[] = {
    "_PixelmapCopyBitsClip",
    "_PixelmapLineClip",
    "_PixelmapPointClip",
    "_PixelmapRectangleClip",
    "_PixelmapRectangleClipTwo",
    "__M_br_device_pixelmap_gen_copy",
    "__M_br_device_pixelmap_gen_copyBits",
    "__M_br_device_pixelmap_gen_copyDirty",
    "__M_br_device_pixelmap_gen_copyFrom",
    "__M_br_device_pixelmap_gen_copyFromDirty",
    "__M_br_device_pixelmap_gen_copyTo",
    "__M_br_device_pixelmap_gen_copyToDirty",
    "__M_br_device_pixelmap_gen_directLock",
    "__M_br_device_pixelmap_gen_directUnlock",
    "__M_br_device_pixelmap_gen_doubleBuffer",
    "__M_br_device_pixelmap_gen_doubleBufferDirty",
    "__M_br_device_pixelmap_gen_fill",
    "__M_br_device_pixelmap_gen_fillDirty",
    "__M_br_device_pixelmap_gen_flush",
    "__M_br_device_pixelmap_gen_line",
    "__M_br_device_pixelmap_gen_rectangle",
    "__M_br_device_pixelmap_gen_rectangle2",
    "__M_br_device_pixelmap_gen_synchronise",
    "__M_br_device_pixelmap_gen_text",
    "__M_br_device_pixelmap_gen_textBounds",
    "__M_br_device_pixelmap_mem_allocateSub",
    "__M_br_device_pixelmap_mem_copyBits",
    "__M_br_device_pixelmap_mem_copyDirty",
    "__M_br_device_pixelmap_mem_copyFrom",
    "__M_br_device_pixelmap_mem_copyFromDirty",
    "__M_br_device_pixelmap_mem_copyTo",
    "__M_br_device_pixelmap_mem_copyToDirty",
    "__M_br_device_pixelmap_mem_directLock",
    "__M_br_device_pixelmap_mem_directUnlock",
    "__M_br_device_pixelmap_mem_doubleBuffer",
    "__M_br_device_pixelmap_mem_doubleBufferDirty",
    "__M_br_device_pixelmap_mem_fill",
    "__M_br_device_pixelmap_mem_fillDirty",
    "__M_br_device_pixelmap_mem_flush",
    "__M_br_device_pixelmap_mem_line",
    "__M_br_device_pixelmap_mem_match",
    "__M_br_device_pixelmap_mem_originSet",
    "__M_br_device_pixelmap_mem_pixelAddressQuery",
    "__M_br_device_pixelmap_mem_pixelAddressSet",
    "__M_br_device_pixelmap_mem_pixelQuery",
    "__M_br_device_pixelmap_mem_pixelSet",
    "__M_br_device_pixelmap_mem_rectangleCopyFrom",
    "__M_br_device_pixelmap_mem_rectangleCopyTo",
    "__M_br_device_pixelmap_mem_rectangleFill",
    "__M_br_device_pixelmap_mem_rectangleStretchCopyFrom",
    "__M_br_device_pixelmap_mem_rectangleStretchCopyTo",
    "__M_br_device_pixelmap_mem_resize",
    "__M_br_device_pixelmap_mem_rowQuery",
    "__M_br_device_pixelmap_mem_rowSet",
    "__M_br_device_pixelmap_mem_rowSize",
    "__M_br_device_pixelmap_mem_synchronise",
    "__M_br_device_pixelmap_mem_validSource",
    "__M_br_device_pixelmap_null_copyBits",
    "__M_br_device_pixelmap_null_copyDirty",
    "__M_br_device_pixelmap_null_copyFrom",
    "__M_br_device_pixelmap_null_copyFromDirty",
    "__M_br_device_pixelmap_null_copyTo",
    "__M_br_device_pixelmap_null_copyToDirty",
    "__M_br_device_pixelmap_null_doubleBuffer",
    "__M_br_device_pixelmap_null_doubleBufferDirty",
    "__M_br_device_pixelmap_null_fill",
    "__M_br_device_pixelmap_null_fillDirty",
    "__M_br_device_pixelmap_null_pixelAddressQuery",
    "__M_br_device_pixelmap_null_pixelAddressSet",
    "__M_br_device_pixelmap_null_pixelQuery",
    "__M_br_device_pixelmap_null_pixelSet",
    "__M_br_device_pixelmap_null_rectangleCopyFrom",
    "__M_br_device_pixelmap_null_rectangleCopyTo",
    "__M_br_device_pixelmap_null_rectangleFill",
    "__M_br_device_pixelmap_null_rectangleStretchCopyFrom",
    "__M_br_device_pixelmap_null_rectangleStretchCopyTo",
    "__M_br_device_pixelmap_null_resize"
};

// GLOBAL: CARMA2_HW 0x0066dc90
br_uint_16 nameOrdinals_BRPMAP1[77] = {
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
};

// GLOBAL: CARMA2_HW 0x0066de68
br_image Image_BRMAP1 = {
    { 0 },
    "BRPMAP1",
    2,
    0,
    1,
    BR_ASIZE(functionPointers_BRPMAP1),
    functionPointers_BRPMAP1,
    BR_ASIZE(namePointers_BRPMAP1),
    namePointers_BRPMAP1,
    nameOrdinals_BRPMAP1,
    0,
    NULL,
    0,
    NULL,
    NULL
};

// FUNCTION: CARMA2_HW 0x005395a0
void C2_HOOK_CDECL BrPixelmapBegin(void) {
    int i;

    C2_HOOK_BUG_ON(sizeof(br_pixelmap_state) != 12);

    BrMemSet(&_pixelmap, 0, sizeof(br_pixelmap_state));
    _pixelmap.res = BrResAllocate(NULL, 0, BR_MEMORY_ANCHOR);
    for (i = 0; i < BR_ASIZE(pm_resourceClasses); i++) {
        BrResClassAdd(&pm_resourceClasses[i]);
    }
#if defined(REC2_MATCHING)
    BrImageAdd(&Image_BRMAP1);
#endif
}

// FUNCTION: CARMA2_HW 0x005395f0
void C2_HOOK_CDECL BrPixelmapEnd(void) {

#if defined(REC2_MATCHING)
    BrImageRemove(&Image_BRMAP1);
#endif
    BrResFree(_pixelmap.res);
    BrMemSet(&_pixelmap, 0, sizeof(br_pixelmap_state));
}
