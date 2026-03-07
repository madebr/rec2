#include "prepmap.h"

#include "dbsetup.h"

// FUNCTION: CARMA2_HW 0x00523160
void C2_HOOK_CDECL BrMapUpdate(br_pixelmap* map, br_uint_16 flags) {

    BrBufferUpdate(map, BRT_COLOUR_MAP_O, flags);
}

// FUNCTION: CARMA2_HW 0x005231f0
void C2_HOOK_STDCALL BrBufferUpdate(br_pixelmap* pm, br_token use, br_uint_16 flags) {

    br_token_value tv[] = {
        { BRT_PREFER_SHARE_B,    { 0 } },
        { BRT_CAN_SHARE_B,       { 1 } },
        { 0 },
    };

    if (v1db.renderer == NULL) {
        return;
    }

    if (flags & BR_MAPU_SHARED) {
        tv[0].v.b = 1;
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_renderer_dispatch, _bufferStoredNew, 0x78);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_buffer_stored_dispatch , _update, 0x44);

    if (pm->stored == NULL) {
        v1db.renderer->dispatch->_bufferStoredNew(v1db.renderer, &pm->stored, use, (br_device_pixelmap*)pm, tv);
    } else {
        ((br_buffer_stored*)pm->stored)->dispatch->_update((br_buffer_stored*)pm->stored, (br_device_pixelmap*)pm, tv);
    }
}

// FUNCTION: CARMA2_HW 0x00523280
void C2_HOOK_STDCALL BrBufferClear(br_pixelmap* pm) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_buffer_stored_dispatch , _free, 0x10);

    if (pm->stored != NULL) {
        ((br_buffer_stored*)pm->stored)->dispatch->_free((br_object*)pm->stored);
        pm->stored = NULL;
    }
}
