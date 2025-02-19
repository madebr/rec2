#include "sbuffer.h"

#include "core/fw/object.h"
#include "core/fw/resource.h"
#include "device.h"
#include "object.h"
#include "texture.h"

#include <stdlib.h>
#include <string.h>

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_tv_template_entry, bufferStoredD3DTemplateEntries, 1, 0x100175b0, {
    { BRT_IDENTIFIER_CSTR, 0, offsetof(br_buffer_stored_d3d, identifier), 0x5,  0x3, },
});
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_tv_template, bufferStoredD3DTemplate, 0x100175c8, {
    BR_ASIZE(bufferStoredD3DTemplateEntries),
    bufferStoredD3DTemplateEntries
});

C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_buffer_stored_dispatch, bufferStoredD3DDispatch, 0x100175e8, {
    NULL,
    NULL,
    NULL,
    NULL,
    (void*)_M_br_buffer_stored_d3d_free,
    (void*)_M_br_object_d3d_identifier,
    (void*)_M_br_buffer_stored_d3d_type,
    (void*)_M_br_buffer_stored_d3d_isType,
    (void*)_M_br_object_d3d_device,
    (void*)_M_br_buffer_stored_d3d_space,
    (void*)_M_br_buffer_stored_d3d_templateQuery,
    _M_br_object_query,
    (void*)_M_br_object_queryBuffer,
    _M_br_object_queryMany,
    _M_br_object_queryManySize,
    _M_br_object_queryAll,
    _M_br_object_queryAllSize,
    (void*)_M_br_buffer_stored_d3d_update,
});
C2_HOOK_VARIABLE_IMPLEMENT(int, gStored_buffer_counter, 0x1001d784);

void C2_HOOK_CDECL BufferStoredD3DClearTemplate(void) {

    memset(&C2V(bufferStoredD3DTemplate), 0, sizeof(C2V(bufferStoredD3DTemplate)));
    C2V(bufferStoredD3DTemplate).entries = C2V(bufferStoredD3DTemplateEntries);
    C2V(bufferStoredD3DTemplate).n_entries = BR_ASIZE(C2V(bufferStoredD3DTemplateEntries));
}
C2_HOOK_FUNCTION(0x100081b0, BufferStoredD3DClearTemplate)

int C2_HOOK_CDECL pixelmapD3DBPP(br_device_pixelmap_d3d* pm) {
    switch (pm->pm_type) {
    case BR_PMT_RGB_555:
    case BR_PMT_RGB_565:
    case BR_PMT_DEPTH_16:
    case BR_PMT_RGBA_4444:
        return 2;
    case BR_PMT_RGB_888:
        return 3;
    case BR_PMT_RGBX_888:
    case BR_PMT_RGBA_8888:
        return 4;
    }
    return 1;
}
C2_HOOK_FUNCTION(0x10008370, pixelmapD3DBPP)

br_error C2_HOOK_CDECL SetupD3DBuffer(br_buffer_stored_d3d* buffer, br_device_pixelmap_d3d* pm) {
    br_colour* map;

    buffer->buffer.uncacheable = 1;

    if (pm->pm_width > 256 || pm->pm_height > 256) {
        return 0xa005;
    }

    buffer->buffer.info.field_0x40 = 3;
    buffer->buffer.field_0x28 = 0x10000;
    buffer->buffer.field_0x2c = 0x10000;
    buffer->buffer.width_p = pm->pm_width;
    buffer->buffer.width_b = pm->pm_width * pixelmapD3DBPP(pm);
    buffer->buffer.height = pm->pm_height;
    if (pm->pm_mip_offset) {
        buffer->buffer.info.smallLod = 0;
    } else {
        buffer->buffer.info.smallLod = buffer->buffer.info.largeLod;
    }
    buffer->pixels_pointer = (br_uint_8*)pm->pm_pixels + pixelmapD3DBPP(pm) * pm->pm_base_x + pm->pm_row_bytes * pm->pm_base_y;
    buffer->buffer.palette_pointer = NULL;
    buffer->buffer.pm_type = pm->pm_type;

    switch (pm->pm_type) {
    case BR_PMT_INDEX_8:
        buffer->buffer.info.format = GR_TEXFMT_P_8;
        if (pm->pm_map != NULL) {
            map = pm->pm_map->pixels;
        } else {
            br_device_d3d* dev = (br_device_d3d*)buffer->dispatch->_device((br_object*)buffer);
            map = dev->clut->entries;
        }
        buffer->buffer.palette_pointer = map;
        buffer->buffer.blended = 0;
        break;
    case BR_PMT_RGB_555:
        buffer->buffer.info.format = GR_TEXFMT_ARGB_1555;
        buffer->buffer.blended = 0;
        break;
    case BR_PMT_RGB_565:
        buffer->buffer.info.format = GR_TEXFMT_RGB_565;
        buffer->buffer.blended = 0;
        break;
    case BR_PMT_RGBA_4444:
        buffer->buffer.info.format = GR_TEXFMT_ARGB_4444;
        buffer->buffer.blended = 1;
        break;
    case BR_PMT_ARGB_1555:
        buffer->buffer.info.format = GR_TEXFMT_ARGB_1555;
        buffer->buffer.blended = 1;
        break;
    default:
        return 0xa005;
    }
    buffer->buffer.field_0x30 = 0;
    buffer->buffer.field_0x48 = buffer->pixels_pointer;
    buffer->buffer.field_0x68 = buffer->pixels_pointer;
    buffer->buffer.uncacheable = 0;
    return 0;
}
C2_HOOK_FUNCTION(0x100081e0, SetupD3DBuffer)

br_buffer_stored_d3d* C2_HOOK_CDECL BufferStoredD3DAllocate(br_primitive_library_d3d* plib, br_token use, br_device_pixelmap_d3d* pm, br_token_value* tv) {
    br_buffer_stored_d3d* self;
    const char* ident;

    switch(use) {
    case BRT_TEXTURE_O:
    case BRT_COLOUR_MAP_O:
        ident = "Texture";
        break;

    default:
        return NULL;
    }

    self = BrResAllocate(C2V(DriverDeviceD3D).res, sizeof(*self), BR_MEMORY_OBJECT);

    if(self == NULL)
        return NULL;

    self->dispatch = &C2V(bufferStoredD3DDispatch);
    self->identifier = ident;
    self->plib = plib;
    self->flags |= SBUFF_SHARED;

    self->buffer.on_card = 0;
    SetupD3DBuffer(self, pm);

    plib->dispatch->_addFront((br_object_container*)plib, (br_object*)self);
    self->buffer.field_0x4c = -1;
    self->buffer.field_0x5c = -1;
    self->buffer.uid = C2V(gStored_buffer_counter);
    C2V(gStored_buffer_counter) += 1;
    self->buffer.field_0x58 = 0;
    self->buffer.field_0x60 = 0;
    return self;
}
C2_HOOK_FUNCTION(0x100083d0, BufferStoredD3DAllocate)

void C2_HOOK_CDECL _M_br_buffer_stored_d3d_free(br_buffer_stored_d3d* self) {

    TextureCacheD3DClearEntry(self->buffer.field_0x4c, self->buffer.uid);
    self->buffer.field_0x4c = -1;
    self->buffer.field_0x5c = -1;
    self->plib->dispatch->_remove((br_object_container*)self->plib, (br_object*)self);
    BrResFreeNoCallback(self);
}
C2_HOOK_FUNCTION(0x100084e0, _M_br_buffer_stored_d3d_free)

br_token C2_HOOK_CDECL _M_br_buffer_stored_d3d_type(br_buffer_stored_d3d* self) {
    return BRT_BUFFER_STORED;
}
C2_HOOK_FUNCTION(0x10008520, _M_br_buffer_stored_d3d_free)

br_boolean C2_HOOK_CDECL _M_br_buffer_stored_d3d_isType(br_buffer_stored_d3d* self, br_token t) {
    return t == BRT_BUFFER_STORED || t == BRT_OBJECT;
}
C2_HOOK_FUNCTION(0x10008530, _M_br_buffer_stored_d3d_isType)

br_int_32 C2_HOOK_CDECL _M_br_buffer_stored_d3d_space(br_buffer_stored_d3d* self) {
    return BrResSizeTotal(self);
}
C2_HOOK_FUNCTION(0x10008550, _M_br_buffer_stored_d3d_space)

br_tv_template* C2_HOOK_CDECL _M_br_buffer_stored_d3d_templateQuery(br_buffer_stored_d3d* self) {
    C2V(bufferStoredD3DTemplate).res = C2V(DriverDeviceD3D).res;
    return &C2V(bufferStoredD3DTemplate);
}
C2_HOOK_FUNCTION(0x10008560, _M_br_buffer_stored_d3d_templateQuery)

br_error C2_HOOK_CDECL _M_br_buffer_stored_d3d_update(br_buffer_stored_d3d* self, br_device_pixelmap_d3d* pm, br_token_value* tv) {
    self->buffer.force_reload = 1;
    self->buffer.field_0x58 = 1;
    if (pm->pm_type == BR_PMT_INDEX_8) {
        br_pixelmap* pal;

        self->buffer.info.format = GR_TEXFMT_P_8;
        pal = pm->pm_map;
        if (pal != NULL) {
            self->buffer.palette_pointer = pal->pixels;
            return 0;
        } else {
            abort();
            /* Not implemented */
#if 0
            br_device_d3d* dev = (br_device_d3d*)self->buffer.base->dispatch->_device(self->buffer.base);
            self->buffer.palette_pointer = &dev->clut[8];
#endif
        }
    }
    return 0;
}
C2_HOOK_FUNCTION(0x10008480, _M_br_buffer_stored_d3d_update)
