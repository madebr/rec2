#include "renderer.h"

br_renderer* (C2_HOOK_STDCALL * RendererSoftAllocate_original)(br_device *dev, br_soft_renderer_facility *type, br_primitive_library *prims);
br_renderer* C2_HOOK_STDCALL RendererSoftAllocate(br_device *dev, br_soft_renderer_facility *type, br_primitive_library *prims) {

#if defined(C2_HOOKS_ENABLED)
    return RendererSoftAllocate_original(dev, type, prims);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00540a40, RendererSoftAllocate, RendererSoftAllocate_original)
