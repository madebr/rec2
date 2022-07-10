#include "pmsetup.h"

#include "core/fw/image.h"
#include "core/fw/resource.h"
#include "core/fw/resreg.h"
#include "core/std/brstdlib.h"

//#include "c2_stddef.h"
#include "c2_stdio.h"

C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap_state, _pixelmap, 0x0079f950);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_resource_class, pm_resourceClasses, 2, 0x0058b968, {
    { 0u, "PIXELMAP", BR_MEMORY_PIXELMAP, NULL, 0u },
    { 0u, "PIXELS", BR_MEMORY_PIXELS, NULL, 32u }
});

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(void*, functionPointers_BRPMAP1, 77, 0x0066dd30, xxx);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(char*, namePointers_BRPMAP1, 77, 0x0066db58, xxx);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_uint_16, nameOrdinals_BRPMAP1, 77, 0x0066dc90, xxx);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_image, Image_BRMAP1, 0x0066de68, xxx);

void (C2_HOOK_CDECL * BrPixelmapBegin_original)(void);
void C2_HOOK_CDECL BrPixelmapBegin(void) {
    C2_HOOK_START();
    C2_HOOK_BUG_ON(sizeof(br_pixelmap_state) != 12);
#if defined(C2_HOOKS_ENABLED)
    BrPixelmapBegin_original();
#else
    int i;

    BrMemSet(&C2V(_pixelmap), 0, sizeof(br_pixelmap_state));
    C2V(_pixelmap).res = BrResAllocate(NULL, 0, BR_MEMORY_ANCHOR);
    for (i = 0; i < BR_ASIZE(C2V(pm_resourceClasses)); i++) {
        BrResClassAdd(&C2V(pm_resourceClasses)[i]);
    }
    BrImageAdd(&C2V(Image_BRMAP1));
#endif
    C2_HOOK_FINISH();
}
C2_HOOK_FUNCTION_ORIGINAL(0x005395a0, BrPixelmapBegin, BrPixelmapBegin_original)

void (C2_HOOK_CDECL * BrPixelmapEnd_original)(void);
void C2_HOOK_CDECL BrPixelmapEnd(void) {
    C2_HOOK_START();
#if defined(C2_HOOKS_ENABLED)
    BrPixelmapEnd_original();
#else
    BrImageRemove(&C2V(Image_BRMAP1));
    BrResFree(C2V(_pixelmap).res);
    BrMemSet(&C2V(_pixelmap), 0, sizeof(br_pixelmap_state));
#endif
    C2_HOOK_FINISH();
}
C2_HOOK_FUNCTION_ORIGINAL(0x005395f0, BrPixelmapEnd, BrPixelmapEnd_original)
