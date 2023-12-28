#include "prepmap.h"

#include "dbsetup.h"

void (C2_HOOK_CDECL * BrMapUpdate_original)(br_pixelmap* map, br_uint_16 flags);
void C2_HOOK_CDECL BrMapUpdate(br_pixelmap* map, br_uint_16 flags) {
#if 0//defined(C2_HOOKS_ENABLED)
    BrMapUpdate_original(map, flags);
#else
    BrBufferUpdate(map, BRT_COLOUR_MAP_O, flags);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00523160, BrMapUpdate, BrMapUpdate_original)

void (C2_HOOK_STDCALL * BrBufferUpdate_original)(br_pixelmap* pm, br_token use, br_uint_16 flags);
void C2_HOOK_STDCALL BrBufferUpdate(br_pixelmap* pm, br_token use, br_uint_16 flags) {
#if 0//defined(C2_HOOKS_ENABLED)
    BrBufferUpdate_original(pm, use, flags);
#else
    br_token_value tv[] = {
        { BRT_PREFER_SHARE_B,    { 0 } },
        { BRT_CAN_SHARE_B,       { 1 } },
        { 0 },
    };

    if (C2V(v1db).renderer == NULL) {
        return;
    }

    if (flags & BR_MAPU_SHARED) {
        tv[0].v.b = 1;
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_renderer_dispatch, _bufferStoredNew, 0x78);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_buffer_stored_dispatch , _update, 0x44);

    if (pm->stored == NULL) {
        C2V(v1db).renderer->dispatch->_bufferStoredNew(C2V(v1db).renderer, &pm->stored, use, (br_device_pixelmap*)pm, tv);
    } else {
        ((br_buffer_stored*)pm->stored)->dispatch->_update((br_buffer_stored*)pm->stored, (br_device_pixelmap*)pm, tv);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005231f0, BrBufferUpdate, BrBufferUpdate_original)

void (C2_HOOK_STDCALL * BrBufferClear_original)(br_pixelmap* pm);
void C2_HOOK_STDCALL BrBufferClear(br_pixelmap* pm) {
#if 0//defined(C2_HOOKS_ENABLED)
    BrBufferClear_original(pm);
#else

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_buffer_stored_dispatch , _free, 0x10);

    if (pm->stored != NULL) {
        ((br_buffer_stored*)pm->stored)->dispatch->_free((br_object*)pm->stored);
        pm->stored = NULL;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00523280, BrBufferClear, BrBufferClear_original)
