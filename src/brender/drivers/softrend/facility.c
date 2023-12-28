#include "facility.h"

br_renderer_facility* (C2_HOOK_STDCALL * RendererFacilitySoftAllocate_original)(br_device* dev, const char* identifier);
br_renderer_facility* C2_HOOK_STDCALL RendererFacilitySoftAllocate(br_device* dev, const char* identifier) {

#if defined(C2_HOOKS_ENABLED)
    return RendererFacilitySoftAllocate_original(dev, identifier);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005405e0, RendererFacilitySoftAllocate, RendererFacilitySoftAllocate_original)
