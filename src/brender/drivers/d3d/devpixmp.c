#include "devpixmp.h"

#include "backend.h"
#include "core/fw/object.h"
#include "core/fw/resource.h"
#include "core/pixelmap/genclip.h"
#include "core/pixelmap/pmgen.h"
#include "core/pixelmap/pmmem.h"
#include "core/pixelmap/pmnull.h"
#include "core/fw/tokenval.h"
#include "device.h"
#include "devpmsys.h"
#include "object.h"

#include <windows.h>

#include <stdlib.h>

// GLOBAL: D3D 0x100152e0
br_tv_template_entry devicePixelmapD3DTemplateEntries[] = {
    { BRT_WIDTH_I32,         0, offsetof(br_device_pixelmap_d3d, pm_width),         0x5,    0xd, },
    { BRT_HEIGHT_I32,        0, offsetof(br_device_pixelmap_d3d, pm_height),        0x5,    0xd, },
    { BRT_PIXEL_TYPE_U8,     0, offsetof(br_device_pixelmap_d3d, pm_type),          0x5,    0xc, },
    { BRT_OUTPUT_FACILITY_O, 0, offsetof(br_device_pixelmap_d3d, output_facility),  0x5,    0x3, },
    { BRT_FACILITY_O,        0, offsetof(br_device_pixelmap_d3d, output_facility),  0x1,    0x3, },
    { BRT_IDENTIFIER_CSTR,   0, offsetof(br_device_pixelmap_d3d, pm_identifier),    0x5,    0x3, },
    { BRT_CLUT_O,            0, offsetof(br_device_pixelmap_d3d, clut),             0x5,    0x3, },
};

// GLOBAL: D3D 0x10015388
br_tv_template devicePixelmapD3DTemplate = {
    BR_ASIZE(devicePixelmapD3DTemplateEntries),
    devicePixelmapD3DTemplateEntries
};

struct match_d3d_tokens {
    br_token    use;
    br_int_32   pixel_bits;
    br_int_32   width;
    br_int_32   height;
    br_object*  renderer;
};

// GLOBAL: D3D 0x100153a8
br_tv_template_entry matchD3DTemplateEntries[] = {
    { BRT_USE_T,            0,  offsetof(struct match_d3d_tokens, use),         0x2,    0x3, },
    { BRT_PIXEL_BITS_I32,   0,  offsetof(struct match_d3d_tokens, pixel_bits),  0x2,    0x3, },
    { BRT_RENDERER_O,       0,  offsetof(struct match_d3d_tokens, renderer),    0x2,    0x3, },
    { BRT_WIDTH_I32,        0,  offsetof(struct match_d3d_tokens, width),       0x2,    0x3, },
    { BRT_HEIGHT_I32,       0,  offsetof(struct match_d3d_tokens, height),      0x2,    0x3, },
};

// GLOBAL: D3D 0x10015420
br_tv_template matchD3DTemplate = {
    BR_ASIZE(matchD3DTemplateEntries),
    matchD3DTemplateEntries,
};

struct devicePixelmapD3DAllocateTokens {
    HWND hwnd;
};

// GLOBAL: D3D 0x10015440
br_tv_template_entry devicePixelmapD3DAllocateTemplateEntries[] = {
    { BRT_WINDOW_HANDLE_H,  0,  offsetof(struct devicePixelmapD3DAllocateTokens, hwnd),   0x2, 0x3, },
};

// GLOBAL: D3D 0x10015458
br_tv_template devicePixelmapD3DAllocateTemplate = {
    BR_ASIZE(devicePixelmapD3DAllocateTemplateEntries),
    devicePixelmapD3DAllocateTemplateEntries
};


// GLOBAL: D3D 0x100cea84
br_boolean passthru_vga;

// GLOBAL: D3D 0x100cea80
br_uint_32 current_width;

// GLOBAL: D3D 0x100cea88
br_uint_32 current_height;

#define GR_LFBWRITEMODE_565 0x0
#define GR_LFBWRITEMODE_555 0x1


// GLOBAL: D3D 0x100cea8c
int lfb_write_mode;

// GLOBAL: D3D 0x10015478
br_device_pixelmap_dispatch devicePixelmapD3DDispatch = {
    NULL,
    NULL,
    NULL,
    NULL,
    (void*)_M_br_device_pixelmap_d3d_free,
    (void*)_M_br_object_d3d_identifier,
    (void*)_M_br_device_pixelmap_d3d_type,
    (void*)_M_br_device_pixelmap_d3d_isType,
    (void*)_M_br_object_d3d_device,
    (void*)_M_br_device_pixelmap_d3d_space,

    (void*)_M_br_device_pixelmap_d3d_queryTemplate,
    (void*)_M_br_object_query,
    (void*)_M_br_object_queryBuffer,
    _M_br_object_queryMany,
    _M_br_object_queryManySize,
    _M_br_object_queryAll,
    _M_br_object_queryAllSize,

    _M_br_device_pixelmap_mem_validSource,
    (void*)_M_br_device_pixelmap_d3d_resize,
    (void*)_M_br_device_pixelmap_d3d_match,
    (void*)_M_br_device_pixelmap_d3d_allocateSub,

    _M_br_device_pixelmap_gen_copy,
    _M_br_device_pixelmap_gen_copyTo,
    _M_br_device_pixelmap_gen_copyFrom,
    (void*)_M_br_device_pixelmap_d3d_fill,
    (void*)_M_br_device_pixelmap_d3d_doubleBuffer,

    _M_br_device_pixelmap_gen_copyDirty,
    _M_br_device_pixelmap_gen_copyToDirty,
    _M_br_device_pixelmap_gen_copyFromDirty,
    _M_br_device_pixelmap_gen_fillDirty,
    _M_br_device_pixelmap_gen_doubleBufferDirty,

    _M_br_device_pixelmap_gen_rectangle,
    _M_br_device_pixelmap_gen_rectangle2,
    (void*)_M_br_device_pixelmap_d3d_rectangleCopy,
    (void*)_M_br_device_pixelmap_d3d_rectangleCopyTo,
    (void*)_M_br_device_pixelmap_d3d_rectangleCopyFrom,
    (void*)_M_br_device_pixelmap_d3d_rectangleStretchCopy,
    (void*)_M_br_device_pixelmap_d3d_rectangleStretchCopyTo,
    (void*)_M_br_device_pixelmap_d3d_rectangleStretchCopyFrom,
    (void*)_M_br_device_pixelmap_d3d_rectangleFill,
    (void*)_M_br_device_pixelmap_d3d_pixelSet,
    _M_br_device_pixelmap_gen_line,
    _M_br_device_pixelmap_gen_copyBits,

    _M_br_device_pixelmap_gen_text,
    _M_br_device_pixelmap_gen_textBounds,

    _M_br_device_pixelmap_mem_rowSize,
    _M_br_device_pixelmap_mem_rowQuery,
    _M_br_device_pixelmap_mem_rowSet,

    _M_br_device_pixelmap_null_pixelQuery,
    _M_br_device_pixelmap_null_pixelAddressQuery,

    _M_br_device_pixelmap_null_pixelAddressSet,
    _M_br_device_pixelmap_mem_originSet,

    (void*)_M_br_device_pixelmap_d3d_flush,
    (void*)_M_br_device_pixelmap_d3d_synchronise,
    (void*)_M_br_device_pixelmap_d3d_directLock,
    (void*)_M_br_device_pixelmap_d3d_directUnlock,
};

// FUNCTION: D3D 0x100013a0
br_error C2_HOOK_CDECL DevicePixelmapD3DAllocateMode(br_device_d3d* dev, br_output_facility_d3d* facility, br_device_pixelmap_d3d** newpm, br_token_value* tv) {
    br_int_32 c;
    br_device_pixelmap_d3d* self;
    int window_width;
    int window_height;
    struct devicePixelmapD3DAllocateTokens pt;

    if (dev->active_buffers[BT_FRONTSCREEN] != NULL) {
        return 0xa00c;
    }

    memset(&devicePixelmapD3DAllocateTemplate, 0, sizeof(devicePixelmapD3DAllocateTemplate));
    devicePixelmapD3DAllocateTemplate.entries = devicePixelmapD3DAllocateTemplateEntries;
    devicePixelmapD3DAllocateTemplate.n_entries = BR_ASIZE(devicePixelmapD3DAllocateTemplateEntries);

    pt.hwnd = NULL;
    BrTokenValueSetMany(&pt, &c, NULL, tv, &devicePixelmapD3DAllocateTemplate);
    if (pt.hwnd == NULL) {
        return 0xa001;
    }

    dev->identifier = "D3D driver";
    if (gD3D_windows) {
        window_width = 640;
        window_height = 480;
    } else {
        window_width = GetSystemMetrics(SM_CXSCREEN);
        window_height = GetSystemMetrics(SM_CYSCREEN);
    }
    SetForegroundWindow(pt.hwnd);
    MoveWindow(pt.hwnd, 0, 0, window_width, window_height, TRUE);

    if (InitD3D(pt.hwnd) != 0) {
        return 0xa002;
    }
    if (facility->colour_type == BR_PMT_RGB_565) {
        lfb_write_mode = GR_LFBWRITEMODE_565;
    } else if (facility->colour_type == BR_PMT_RGB_555) {
        lfb_write_mode = GR_LFBWRITEMODE_555;
    } else {
        return 0xa005;
    }

    memset(&devicePixelmapD3DTemplate, 0, sizeof(devicePixelmapD3DTemplate));
    devicePixelmapD3DTemplate.entries = devicePixelmapD3DTemplateEntries;
    devicePixelmapD3DTemplate.n_entries = BR_ASIZE(devicePixelmapD3DTemplateEntries);

    memset(&matchD3DTemplate, 0, sizeof(matchD3DTemplate));
    matchD3DTemplate.entries = matchD3DTemplateEntries;
    matchD3DTemplate.n_entries = BR_ASIZE(matchD3DTemplateEntries);

    C2_HOOK_BUG_ON(sizeof(br_device_pixelmap_d3d) != 0x5c);

    self = BrResAllocate(dev->res, sizeof(br_device_pixelmap_d3d), BR_MEMORY_OBJECT);
    self->dispatch = &devicePixelmapD3DDispatch;
    self->pm_identifier = dev->identifier;
    self->restore_mode = 1;
    self->pm_pixels = NULL;
    self->pm_width = facility->width;
    self->pm_height = facility->height;
    self->pm_row_bytes = 0;
    self->pm_origin_x = 0;
    self->pm_origin_y = 0;
    self->pm_flags = BR_PMF_NO_ACCESS;
    self->pm_base_x = 0;
    self->pm_base_y = 0;
    self->output_facility = facility;
    self->buffer_type = 0;
    self->clut = dev->clut;
    dev->active_buffers[BT_FRONTSCREEN] = self;
    facility->dispatch->_addFront((br_object_container*)facility, (br_object*)self);
    *newpm = self;
    current_width = facility->width;
    current_height = facility->height;
    passthru_vga = 0;
    if (gAttached_surface_pixel_format.dwRGBAlphaBitMask == 0x0000
            && gAttached_surface_pixel_format.dwRBitMask == 0xf800
            && gAttached_surface_pixel_format.dwGBitMask == 0x07e0
            && gAttached_surface_pixel_format.dwBBitMask == 0x001f) {
        self->pm_type = BR_PMT_RGB_565;
    } else {
        self->pm_type = BR_PMT_RGB_555;
    }
    return 0;
}

// FUNCTION: D3D 0x100015c0
void C2_HOOK_CDECL _M_br_device_pixelmap_d3d_free(br_device_pixelmap_d3d* self) {

    if (self->buffer_type == BT_FRONTSCREEN) {
        br_device_d3d *dev;
        self->output_facility->dispatch->_remove((br_object_container*)self->output_facility, (br_object*)self);
        self->output_facility->num_instances -= 1;
        dev = (br_device_d3d*)self->dispatch->_device((br_object *) self);
        dev->active_buffers[self->buffer_type] = NULL;
        CloseD3D();
    }
    BrResFreeNoCallback(self);
}

// FUNCTION: D3D 0x10001610
br_token C2_HOOK_CDECL _M_br_device_pixelmap_d3d_type(br_device_pixelmap_d3d* self) {
    return BRT_DEVICE_PIXELMAP;
}

// FUNCTION: D3D 0x10001620
br_boolean C2_HOOK_CDECL _M_br_device_pixelmap_d3d_isType(br_device_pixelmap_d3d* self, br_token t) {
    return t == BRT_DEVICE_PIXELMAP || t == BRT_OBJECT;
}

// FUNCTION: D3D 0x10001640
br_int_32 C2_HOOK_CDECL _M_br_device_pixelmap_d3d_space(br_device_pixelmap_d3d* self) {
    return sizeof(br_device_pixelmap_d3d);
}

// FUNCTION: D3D 0x10001650
br_tv_template * C2_HOOK_CDECL _M_br_device_pixelmap_d3d_queryTemplate(br_device_pixelmap_d3d* self) {
    devicePixelmapD3DTemplate.res = DriverDeviceD3D.res;
    return &devicePixelmapD3DTemplate;
}

br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_resize(br_device_pixelmap_d3d* self, br_int_32 width, br_int_32 height) {
    return 0xa005;
}

// FUNCTION: D3D 0x10001660
br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_match(br_device_pixelmap_d3d* self, br_device_pixelmap_d3d** newpm, br_token_value* tv) {
    struct match_d3d_tokens mt = { BRT_NONE, 0 };
    br_int_32 count;
    br_device_pixelmap_d3d* pm;
    br_device_d3d *dev = (br_device_d3d*)self->dispatch->_device((br_object*)self);

    matchD3DTemplate.res = dev->res;

    mt.width = self->pm_width;
    mt.height = self->pm_height;
    BrTokenValueSetMany(&mt, &count, NULL, tv, &matchD3DTemplate);

    if (mt.use == BRT_OFFSCREEN) {
        if (self->buffer_type != BT_FRONTSCREEN) {
            return 0xa005;
        }
        if (dev->active_buffers[BT_BACKSCREEN] != NULL) {
            return AllocateD3DSysMemPixelmap(self, newpm, mt.width, mt.height);
        }

        pm = BrResAllocate(dev->res, sizeof(*pm), BR_MEMORY_OBJECT);
        pm->pm_width = self->pm_width;
        pm->pm_height = self->pm_height;
        pm->pm_pixels = NULL;
        pm->pm_origin_x = self->pm_origin_x;
        pm->pm_origin_y = self->pm_origin_y;
        pm->pm_flags = BR_PMF_NO_ACCESS;
        pm->output_facility = self->output_facility;
        pm->pm_type = self->pm_type;
        if (gAttached_surface_pixel_format.dwRGBAlphaBitMask == 0
                && (gAttached_surface_pixel_format.dwRBitMask == 0xf800
                && gAttached_surface_pixel_format.dwGBitMask == 0x07e0)
                && gAttached_surface_pixel_format.dwBBitMask == 0x001f) {
            pm->pm_type = BR_PMT_RGB_565;
        } else {
            pm->pm_type = BR_PMT_RGB_555;
        }
        pm->dispatch = &devicePixelmapD3DDispatch;
        pm->pm_identifier = "Offscreen";
        pm->buffer_type = BT_BACKSCREEN;
        dev->active_buffers[BT_BACKSCREEN] = pm;
        self->field_0x50 = pm;
    } else if (mt.use == BRT_DEPTH) {
        if (self->buffer_type != BT_FRONTSCREEN && self->buffer_type != BT_BACKSCREEN) {
            return 0xa005;
        }
        if (dev->active_buffers[BT_DEPTH] != NULL) {
            return 0xa00c;
        }
        pm = BrResAllocate(dev->res, sizeof(*pm), BR_MEMORY_OBJECT);

        pm->pm_width = self->pm_width;
        pm->pm_height = self->pm_height;
        pm->pm_pixels = NULL;
        pm->pm_origin_x = self->pm_origin_x;
        pm->pm_origin_y = self->pm_origin_y;
        pm->pm_flags = BR_PMF_NO_ACCESS;
        pm->output_facility = self->output_facility;
        pm->pm_type = BR_PMT_DEPTH_16;
        pm->dispatch = &devicePixelmapD3DDispatch;
        pm->pm_identifier = "Depth";
        pm->buffer_type = BT_DEPTH;
        dev->active_buffers[BT_DEPTH] = pm;
        self->field_0x54 = pm;
    } else if (mt.use == BRT_NO_RENDER) {
        return AllocateD3DSysMemPixelmap(self, newpm, mt.width, mt.height);
    } else {
        return 0xa005;
    }
    *newpm = pm;
    return 0;
}

// FUNCTION: D3D 0x10001c90
br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_allocateSub(br_device_pixelmap_d3d* self, br_device_pixelmap_d3d** newpm, br_rectangle* rect) {
    br_device_pixelmap_d3d* pm;
    br_rectangle out;

    if (self == NULL) {
        return 0x1002;
    }
    pm = BrResAllocate(DriverDeviceD3D.res, sizeof(*pm), BR_MEMORY_PIXELMAP);
    *pm = *self;
    pm->pm_flags &= ~BR_PMF_LINEAR;
    if (PixelmapRectangleClip(&out, rect, (br_pixelmap *)self) == BR_CLIP_REJECT) {
        return 0x1002;
    }
    pm->pm_base_x += out.x;
    pm->pm_base_y += out.y;
    pm->pm_width = out.w;
    pm->pm_height = out.h;
    pm->pm_origin_x = 0;
    pm->pm_origin_y = 0;
    *newpm = pm;
    return 0;
}

br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_fill(br_device_pixelmap *self, br_uint_32 colour) {
    return 0;
}

// FUNCTION: D3D 0x100018d0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_doubleBuffer(br_device_pixelmap_d3d* self, br_device_pixelmap_d3d* src) {
    br_device_d3d* dev = (br_device_d3d*)self->dispatch->_device((br_object*)self);

    if (self != dev->active_buffers[BT_FRONTSCREEN]) {
        return 0xa005;
    }
    if (src != dev->active_buffers[BT_BACKSCREEN]) {
        return 0xa005;
    }

    DoD3DOperation(4);
    return 0;
}

// FUNCTION: D3D 0x10001b00
br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_rectangleCopyTo(br_device_pixelmap_d3d* self, br_point* p, br_device_pixelmap_d3d* src, br_rectangle* r) {
    br_rectangle ar;
    br_point ap;
    br_int_32 y;
    br_uint_8* src_ptr;
    br_uint_8* dst_ptr;
    DDSURFACEDESC surface_desc;

    if(PixelmapRectangleClipTwo(&ar, &ap, r, p, (br_pixelmap*)self, (br_pixelmap*)src) == BR_CLIP_REJECT) {
        return 0;
    }
    gINT_1001bb94 = 1;
    if (gNo_2d_during_3d_scene == 1) {
        FUN_10004780((br_uint_8*)src->pm_pixels + ar.y * src->pm_row_bytes + 2 * ar.x, src->pm_row_bytes);
        return 0;
    }
    gLock_count += 1;
    if (gLock_count == 1) {
        DoD3DOperation(3);
        memset(&surface_desc, 0, sizeof(surface_desc));
        surface_desc.dwSize = sizeof(surface_desc);
        if (IDirectDrawSurface_Lock(gAttached_surface, NULL, &surface_desc, DDLOCK_WAIT, NULL) != DD_OK) {
            return 0;
        }
    }
    src_ptr = (br_uint_8*)src->pm_pixels + ar.y * src->pm_row_bytes + 2 * ar.x;
    dst_ptr = (br_uint_8*)surface_desc.lpSurface + (self->pm_base_y + ap.y) * surface_desc.lPitch + 2 * (self->pm_base_x + ap.x);

    for (y = 0; y < ar.h; y++) {
        memcpy(dst_ptr, src_ptr, 2 * ar.w);
        src_ptr += src->pm_row_bytes;
        dst_ptr += surface_desc.lPitch;
    }
    gLock_count -= 1;
    if (gLock_count == 0) {
        if (IDirectDrawSurface_Unlock(gAttached_surface, NULL) != DD_OK) {
            return 0;
        }
        DoD3DOperation(2);
    } else if (gLock_count < 0) {
        gLock_count = 0;
    }
    return 0;
}

br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_rectangleCopy(br_device_pixelmap_d3d* self, br_point* p, br_device_pixelmap_d3d* src, br_rectangle* r) {
    return _M_br_device_pixelmap_d3d_rectangleCopyTo(self, p, src, r);
}

br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_rectangleCopyFrom(br_device_pixelmap_d3d* self, br_point* p, br_device_pixelmap_d3d* mem, br_rectangle* r) {
    return 0;
}

// FUNCTION: D3D 0x1000c460
br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_rectangleStretchCopy(br_device_pixelmap_d3d* self, br_rectangle* d, br_device_pixelmap_d3d* src, br_rectangle* s) {
    return 0;
}

// FUNCTION: D3D 0x10001d40
br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_rectangleStretchCopyTo(br_device_pixelmap_d3d* self, br_rectangle* d, br_device_pixelmap_d3d* src, br_rectangle *s) {
    _M_br_device_pixelmap_d3d_rectangleFill(self, d, 0);
    return 0xa005;
}

br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_rectangleStretchCopyFrom(br_device_pixelmap_d3d* self, br_rectangle* d, br_device_pixelmap_d3d* src, br_rectangle* s) {
    return 0xa005;
}

// FUNCTION: D3D 0x100018a0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_rectangleFill(br_device_pixelmap_d3d* self, br_rectangle* rect, br_uint_32 colour) {
    br_rectangle r;

    PixelmapRectangleClip(&r, rect, 0);
    return 0;
}

// FUNCTION: D3D 0x10001910
br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_pixelSet(br_device_pixelmap_d3d* self, br_point* p, br_uint_32 colour) {
    abort();
}

br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_flush(br_device_pixelmap_d3d* self) {
    return 0;
}

br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_synchronise(br_device_pixelmap_d3d* self, br_token sync_type, br_boolean block) {
    return 0;
}

// FUNCTION: D3D 0x10001d70
br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_directLock(br_device_pixelmap_d3d* self, br_boolean block) {
    abort();
}

// FUNCTION: D3D 0x10001e40
br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_directUnlock(br_device_pixelmap_d3d* self) {
    abort();
}
