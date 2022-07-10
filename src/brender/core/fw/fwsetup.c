#include "fwsetup.h"

#include "core/std/brstddiag.h"
#include "core/std/brstdfile.h"
#include "core/std/brstdmem.h"

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

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(void*, functionPointers_BRCORE1, 185, 0x00665b70, xxx);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, namePointers_BRCORE1, 185, 0x00665710, xxx);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_uint_16, nameOrdinals_BRCORE1, 185, 0x006659f8, TODO);

C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_image, Image_BRCORE1, 0x00665e58, {
        { 0 },
        "BRCORE1",
        2,
        0,
        1,
        185, // FIXME: array sizeof
        C2V(functionPointers_BRCORE1),
        185, // FIXME: array sizeof
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
#if HOOK_FW
    return BrFwBegin_original();
#else
    int i;

    if (C2V(fw).active) {
        return 4103;
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
    C2V(fw).open_mode = 0;
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
    BrImageAdd(&C2V(Image_BRCORE1));

    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00527e20, BrFwBegin, BrFwBegin_original)

br_error (C2_HOOK_CDECL * BrFwEnd_original)(void);
br_error C2_HOOK_CDECL BrFwEnd(void) {
#if HOOK_FW
    return BrFwEnd_original();
#else

    if (fw.active == 0) {
        return 4102;
    }
    BrImageRemove(&Image_BRCORE1);
    BrResFree(fw.res);
    BrMemSet(&fw, 0, sizeof(fw));
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
