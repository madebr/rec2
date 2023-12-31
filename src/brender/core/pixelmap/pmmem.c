#include "pmmem.h"

#include "genclip.h"
#include "pmsetup.h"
#include "pmutils.h"

#include "core/fw/resource.h"
#include "core/fw/tokenval.h"

#include "c2_string.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_tv_template_entry, matchTemplateEntries, 6, 0x0066ea98, {
    { BRT_USE_T,            NULL,  0, 2, 3, 0, 0, },
    { BRT_PIXEL_TYPE_U8,    NULL,  4, 2, 3, 0, 0, },
    { BRT_PIXEL_BITS_I32,   NULL,  8, 2, 3, 0, 0, },
    { BRT_RENDERER_O,       NULL, 20, 2, 3, 0, 0, },
    { BRT_WIDTH_I32,        NULL, 12, 2, 3, 0, 0, },
    { BRT_HEIGHT_I32,       NULL, 16, 2, 3, 0, 0, }
});

C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_device_pixelmap_dispatch, devicePixelmapDispatch, 0x0058baf0, {
    NULL,
    NULL,
    NULL,
    NULL,
    (void*)_M_br_device_pixelmap_mem_free,
    (void*)_M_br_device_pixelmap_mem_identifier,
    (void*)_M_br_device_pixelmap_mem_type,
    (void*)_M_br_device_pixelmap_mem_isType,
    (void*)_M_br_device_pixelmap_mem_device,
    (void*)_M_br_device_pixelmap_mem_space,
    (void*)_M_br_device_pixelmap_mem_queryTemplate,
    (void*)_M_br_object_query,
    (void*)_M_br_object_queryBuffer,
    (void*)_M_br_object_queryMany,
    (void*)_M_br_object_queryManySize,
    (void*)_M_br_object_queryAll,
    (void*)_M_br_object_queryAllSize,
    (void*)_M_br_device_pixelmap_mem_validSource,
    _M_br_device_pixelmap_mem_resize,
    _M_br_device_pixelmap_mem_match,
    _M_br_device_pixelmap_mem_allocateSub,
    _M_br_device_pixelmap_mem_copyTo,
    _M_br_device_pixelmap_mem_copyTo,
    _M_br_device_pixelmap_mem_copyFrom,
    _M_br_device_pixelmap_mem_fill,
    _M_br_device_pixelmap_gen_doubleBuffer,
    _M_br_device_pixelmap_gen_copyDirty,
    _M_br_device_pixelmap_gen_copyToDirty,
    _M_br_device_pixelmap_gen_copyFromDirty,
    _M_br_device_pixelmap_gen_fillDirty,
    _M_br_device_pixelmap_gen_doubleBufferDirty,
    _M_br_device_pixelmap_gen_rectangle,
    _M_br_device_pixelmap_gen_rectangle2,
    _M_br_device_pixelmap_mem_rectangleCopyTo,
    _M_br_device_pixelmap_mem_rectangleCopyTo,
    _M_br_device_pixelmap_mem_rectangleCopyFrom,
    _M_br_device_pixelmap_mem_rectangleStretchCopyTo,
    _M_br_device_pixelmap_mem_rectangleStretchCopyTo,
    _M_br_device_pixelmap_mem_rectangleStretchCopyFrom,
    _M_br_device_pixelmap_mem_rectangleFill,
    _M_br_device_pixelmap_mem_pixelSet,
    _M_br_device_pixelmap_mem_line,
    _M_br_device_pixelmap_mem_copyBits,
    _M_br_device_pixelmap_gen_text,
    _M_br_device_pixelmap_gen_textBounds,
    _M_br_device_pixelmap_mem_rowSize,
    (void*)_M_br_device_pixelmap_mem_rowSet,
    (void*)_M_br_device_pixelmap_mem_rowQuery,
    _M_br_device_pixelmap_mem_pixelQuery,
    _M_br_device_pixelmap_mem_pixelAddressQuery,
    _M_br_device_pixelmap_mem_pixelAddressSet,
    _M_br_device_pixelmap_mem_originSet,
    _M_br_device_pixelmap_mem_flush,
    _M_br_device_pixelmap_mem_synchronise,
    _M_br_device_pixelmap_mem_directLock,
    _M_br_device_pixelmap_mem_directUnlock,
});

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_tv_template_entry, devicePixelmapTemplateEntries, 4, 0x0058ba90, {
    { BRT_IDENTIFIER_CSTR, NULL, offsetof(br_pixelmap, identifier), 0x5, 0x3, 0, 0, },
    { BRT_WIDTH_I32, NULL, offsetof(br_pixelmap, width),            0x5, 0xd, 0, 0, },
    { BRT_HEIGHT_I32, NULL, offsetof(br_pixelmap, height),          0x5, 0xd, 0, 0, },
    { BRT_PIXEL_TYPE_U8, NULL, offsetof(br_pixelmap, pm_type),      0x5, 0xc, 0, 0, },
});

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(pm_type_info, pmTypeInfo, 32, 0x0058b990, {
    { 0x1,  0x1,  0x20, 0x1, },
    { 0x2,  0x1,  0x10, 0x1, },
    { 0x4,  0x1,  0x8,  0x1, },
    { 0x8,  0x1,  0x4,  0x1, },
    { 0x10, 0x2,  0x2,  0x2, },
    { 0x10, 0x2,  0x2,  0x2, },
    { 0x18, 0x3,  0x4,  0x2, },
    { 0x20, 0x4,  0x1,  0x2, },
    { 0x20, 0x4,  0x1,  0xa, },
    { 0x10, 0x1,  0x2,  0x10,},
    { 0x20, 0x1,  0x1,  0x10,},
    { 0x10, 0x2,  0x4,  0x4, },
    { 0x20, 0x4,  0x4,  0x4, },
    { 0x8,  0x1,  0x4,  0x8, },
    { 0x10, 0x2,  0x2,  0x9, },
    { 0x10, 0x2,  0x2,  0x9, },
    { 0x10, 0x2,  0x2,  0x9, },
    { 0x10, 0x2,  0x2,  0x9, },
    { 0x10, 0x2,  0x2,  0x9, },
    { 0x10, 0x2,  0x2,  0x9, },
    { 0x10, 0x2,  0x2,  0x9, },
    { 0x8,  0x1,  0x4,  0x2, },
    { 0x8,  0x1,  0x4,  0x4, },
    { 0x20, 0x4,  0x1,  0xa, },
    { 0x4,  0x1,  0x8,  0x8, },
    { 0x8,  0x1,  0x4,  0x9, },
    { 0x8,  0x1,  0x4,  0x9, },
    { 0x10, 0x2,  0x2,  0x4, },
    { 0x20, 0x4,  0x1,  0x4, },
    { 0x10, 0x2,  0x2,  0x4, },
    { 0x10, 0x2,  0x2,  0x4, },
    { 0x10, 0x2,  0x2,  0xa, },
});

br_device_pixelmap* C2_HOOK_STDCALL DevicePixelmapMemAllocate(br_uint_8 type, br_uint_16 w, br_uint_16 h, void* pixels, int flags) {
    br_device_pixelmap* pm;
    pm_type_info* tip;

    C2_HOOK_BUG_ON(sizeof(br_pixelmap) != 0x44);

    tip = &C2V(pmTypeInfo)[type];
    pm = BrResAllocate(C2V(_pixelmap).res, sizeof(br_device_pixelmap), BR_MEMORY_PIXELMAP);
    pm->pm_type = type;
    pm->dispatch = &C2V(devicePixelmapDispatch);
    pm->pm_identifier = NULL;
    pm->pm_map = NULL;
    pm->pm_flags = BR_PMF_LINEAR;
    pm->pm_copy_function = BR_PMCOPY_NORMAL;
    pm->pm_base_x = 0;
    pm->pm_base_y = 0;
    pm->pm_width = w;
    pm->pm_height = h;
    pm->pm_origin_x = 0;
    pm->pm_origin_y = 0;

    pm->pm_row_bytes = tip->bits * tip->align * ((w + tip->align - 1) / tip->align) / 8;

    if ((8 * pm->pm_row_bytes % tip->bits) == 0) {
        pm->pm_flags |= BR_PMF_ROW_WHOLEPIXELS;
    }
    if (!(flags & BR_PMAF_NO_PIXELS)) {
        if (pixels != NULL) {
            pm->pm_pixels = pixels;
        } else {
            pm->pm_pixels = BrResAllocate(pm, pm->pm_height * pm->pm_row_bytes, BR_MEMORY_PIXELS);
        }
    }

     pm->pm_pixels_qualifier = GetSysQual();

    if (flags & BR_PMAF_INVERTED) {
        pm->pm_pixels = (char*)pm->pm_pixels + (pm->pm_height - 1) * pm->pm_row_bytes;
        pm->pm_row_bytes = -pm->pm_row_bytes;
    }
    return pm;
}
C2_HOOK_FUNCTION(0x00539620, DevicePixelmapMemAllocate)

void C2_HOOK_STDCALL CheckDispatch(br_device_pixelmap* pm) {
    if (pm->dispatch == NULL) {
        pm->dispatch = &C2V(devicePixelmapDispatch);
    }
}
C2_HOOK_FUNCTION(0x00539730, CheckDispatch)

br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_allocateSub(br_device_pixelmap* self, br_device_pixelmap** newpm, br_rectangle* rect) {
    br_device_pixelmap* pm;
    br_rectangle out;

    if (PixelmapRectangleClip(&out, rect, (br_pixelmap*)self) == BR_CLIP_REJECT) {
        return 4098;
    }

    pm = (br_device_pixelmap*)BrResAllocate(C2V(_pixelmap).res, sizeof(br_pixelmap), BR_MEMORY_PIXELMAP);
    c2_memcpy(pm, self, sizeof(br_pixelmap));
    pm->pm_base_x += out.x;
    pm->pm_base_y += out.y;
    pm->pm_width = out.w;
    pm->pm_height = out.h;
    pm->pm_origin_x = 0;
    pm->pm_origin_y = 0;
    pm->pm_stored = 0;
    pm->dispatch = &C2V(devicePixelmapDispatch);
    if (pm->pm_width != self->pm_width) {
        pm->pm_flags &= ~BR_PMF_LINEAR;
    }
    *newpm = pm;
    return 0;
}
C2_HOOK_FUNCTION(0x00539750, _M_br_device_pixelmap_mem_allocateSub)

void C2_HOOK_CDECL _M_br_device_pixelmap_mem_free(br_device_pixelmap* self) {
    BrResFree(self);
}
C2_HOOK_FUNCTION(0x005397f0, _M_br_device_pixelmap_mem_free)

char* C2_HOOK_CDECL _M_br_device_pixelmap_mem_identifier(br_device_pixelmap* self) {

    return self->pm_identifier;
}
C2_HOOK_FUNCTION(0x00539800, _M_br_device_pixelmap_mem_identifier)

br_token C2_HOOK_CDECL _M_br_device_pixelmap_mem_type(br_device_pixelmap* self) {

    return BRT_DEVICE_PIXELMAP;
}
C2_HOOK_FUNCTION(0x00539810, _M_br_device_pixelmap_mem_type)

br_boolean C2_HOOK_CDECL _M_br_device_pixelmap_mem_isType(br_device_pixelmap* self, br_token t) {

    return t == BRT_DEVICE_PIXELMAP || t == BRT_OBJECT;
}
C2_HOOK_FUNCTION(0x00539820, _M_br_device_pixelmap_mem_isType)

br_device* C2_HOOK_CDECL _M_br_device_pixelmap_mem_device(br_device_pixelmap* self) {

    return NULL;
}
C2_HOOK_FUNCTION(0x00539840, _M_br_device_pixelmap_mem_device)

br_int_32 C2_HOOK_CDECL _M_br_device_pixelmap_mem_space(br_device_pixelmap* self) {

    return BrResSizeTotal(self);
}
C2_HOOK_FUNCTION(0x00539850, _M_br_device_pixelmap_mem_space)

br_tv_template* C2_HOOK_CDECL _M_br_device_pixelmap_mem_queryTemplate(br_device_pixelmap* self) {

    if (C2V(_pixelmap).device_pixelmap_template == NULL) {
        C2V(_pixelmap).device_pixelmap_template = BrTVTemplateAllocate(C2V(_pixelmap).res, C2V(devicePixelmapTemplateEntries), BR_ASIZE(C2V(devicePixelmapTemplateEntries)));
    }
    return C2V(_pixelmap).device_pixelmap_template;
}
C2_HOOK_FUNCTION(0x00539860, _M_br_device_pixelmap_mem_queryTemplate)

br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_validSource(br_device_pixelmap* self, br_object* h) {

    return 0;
}
C2_HOOK_FUNCTION(0x00539890, _M_br_device_pixelmap_mem_validSource)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_mem_resize_original)(br_device_pixelmap* self, br_int_32 width, br_int_32 height);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_resize(br_device_pixelmap* self, br_int_32 width, br_int_32 height) {
#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_mem_resize_original(self, width, height);
#else
    char* pixels;
    pm_type_info* tip;
    br_int_16 old_row_bytes;

    pixels = self->pm_pixels;
    if (self->pm_row_bytes < 0) {
        pixels += (self->pm_height - 1) * self->pm_row_bytes;
    }
    if (BrResIsChild(self, pixels)) {
        BrResFree(pixels);
    }
    self->pm_width = width;
    self->pm_height = height;
    old_row_bytes = self->pm_row_bytes;
    tip = &C2V(pmTypeInfo)[self->pm_type];
    self->pm_row_bytes = (width + tip->align - 1) / tip->align * tip->bits * tip->align / 8;

    if (((8 * self->pm_row_bytes) % tip->bits) == 0) {
        self->pm_flags |= BR_PMF_ROW_WHOLEPIXELS;
    }
    self->pm_pixels = pixels = BrResAllocate(self, height * self->pm_row_bytes, BR_MEMORY_PIXELS);
// FIXME: _GetSysQual()
//    self->pm_pixels_qualifier = _GetSysQual();
    if (old_row_bytes < 0) {
        self->pm_pixels = pixels + (self->pm_height - 1) * self->pm_row_bytes;
        self->pm_row_bytes = -self->pm_row_bytes;
    }
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005398a0, _M_br_device_pixelmap_mem_resize, _M_br_device_pixelmap_mem_resize_original)

// IDA: br_error __cdecl _M_br_device_pixelmap_mem_match(br_device_pixelmap *self, br_device_pixelmap **newpm, br_token_value *tv)
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_match(br_device_pixelmap* self, br_device_pixelmap** newpm, br_token_value* tv) {
    match_tokens mt;
    br_int_32 count;
    br_device_pixelmap* pm;
    br_int_32 bytes;
    br_int_32 r;

    c2_memset(&mt, 0, sizeof(mt));
    if (C2V(_pixelmap).pixelmap_match_template == NULL) {
        C2V(_pixelmap).pixelmap_match_template = BrTVTemplateAllocate(C2V(_pixelmap).res, C2V(matchTemplateEntries), BR_ASIZE(C2V(matchTemplateEntries)));
        return 0x1002;
    }
    mt.width = self->pm_width;
    mt.height = self->pm_height;
    mt.pixel_type = self->pm_type;
    BrTokenValueSetMany(&mt, &count, NULL, tv, C2V(_pixelmap).pixelmap_match_template);

    switch(mt.use) {
        case BRT_DEPTH:
            switch (mt.pixel_bits) {
            case 0:
            case 8:
                mt.pixel_type = BR_PMT_DEPTH_8;
                break;
            case 16:
                mt.pixel_type = BR_PMT_DEPTH_16;
                break;
            case 32:
                mt.pixel_type = BR_PMT_DEPTH_32;
                break;
            default:
                return 0x1002;
            }
            break;
        case BRT_CLONE:
        case BRT_OFFSCREEN:
        case BRT_HIDDEN:
        case BRT_HIDDEN_BUFFER:
        case BRT_NO_RENDER:
            break;
        default:
            return 0x1002;
    }
    if (mt.use == BRT_DEPTH) {
        pm = DevicePixelmapMemAllocate(mt.pixel_type, mt.width, mt.height, NULL, ((self->pm_row_bytes < 0) ? BR_PMAF_INVERTED : 0) | BR_PMAF_NO_PIXELS);
        bytes = C2V(pmTypeInfo)[self->pm_type].bits >> 3;
        r = self->pm_row_bytes;
        if (r < 0) {
            r = -r;
        }
        r = (r + bytes - 1) / bytes;
        pm->pm_row_bytes = r * bytes;

        pm->pm_pixels = BrResAllocate(pm, pm->pm_row_bytes * pm->pm_height, BR_MEMORY_PIXELS);

        if (pm->pm_width * bytes == pm->pm_row_bytes) {
            pm->pm_flags |= BR_PMF_LINEAR;
        } else {
            pm->pm_flags &= ~BR_PMF_LINEAR;
        }

        if (self->pm_row_bytes < 0) {
            pm->pm_pixels = (char *)pm->pm_pixels + pm->pm_row_bytes * (pm->pm_height-1);
            pm->pm_row_bytes = -pm->pm_row_bytes;
        }
    } else {
        pm = DevicePixelmapMemAllocate(mt.pixel_type, mt.width, mt.height, NULL, (self->pm_row_bytes < 0) ? BR_PMAF_INVERTED : 0);
    }

    pm->pm_origin_x = self->pm_origin_x;
    pm->pm_origin_y = self->pm_origin_y;

    *newpm = pm;

    return 0;
}
C2_HOOK_FUNCTION(0x00539990, _M_br_device_pixelmap_mem_match)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_mem_copyTo_original)(br_device_pixelmap* self, br_device_pixelmap* src);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_copyTo(br_device_pixelmap* self, br_device_pixelmap* src) {
#if defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_mem_copyTo_original(self, src);
#else
    br_int_8 bytes;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00539bf0, _M_br_device_pixelmap_mem_copyTo, _M_br_device_pixelmap_mem_copyTo_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_mem_copyFrom_original)(br_device_pixelmap* self, br_device_pixelmap* dest);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_copyFrom(br_device_pixelmap* self, br_device_pixelmap* dest) {
#if defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_mem_copyFrom_original(self, dest);
#else
    br_int_8 bytes;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00539dc0, _M_br_device_pixelmap_mem_copyFrom, _M_br_device_pixelmap_mem_copyFrom_original)

br_error (C2_HOOK_CDECL  * _M_br_device_pixelmap_mem_fill_original)(br_device_pixelmap* self, br_uint_32 colour);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_fill(br_device_pixelmap* self, br_uint_32 colour) {
#if defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_mem_fill_original(self, colour);
#else
    br_int_8 bytes;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00539f90, _M_br_device_pixelmap_mem_fill, _M_br_device_pixelmap_mem_fill_original)

br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_doubleBuffer(br_device_pixelmap* self, br_device_pixelmap* src) {

    return 0x1008;
}
C2_HOOK_FUNCTION(0x0053a0f0, _M_br_device_pixelmap_mem_doubleBuffer)

br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_copyDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {

    return 0x1008;
}
C2_HOOK_FUNCTION(0x0053a100, _M_br_device_pixelmap_mem_copyDirty)

br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_copyToDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {

    return 0x1008;
}
C2_HOOK_FUNCTION(0x0053a110, _M_br_device_pixelmap_mem_copyToDirty)

br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_copyFromDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {

    return 0x1008;
}
C2_HOOK_FUNCTION(0x0053a120, _M_br_device_pixelmap_mem_copyFromDirty)

br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_fillDirty(br_device_pixelmap* self, br_uint_32 colour, br_rectangle* dirty, br_int_32 num_rects) {

    return 0x1008;
}
C2_HOOK_FUNCTION(0x0053a130, _M_br_device_pixelmap_mem_fillDirty)

br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_doubleBufferDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {

    return 0x1008;
}
C2_HOOK_FUNCTION(0x0053a140, _M_br_device_pixelmap_mem_doubleBufferDirty)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_mem_rectangleCopyTo_original)(br_device_pixelmap* self, br_point* p, br_device_pixelmap* src, br_rectangle* r);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_rectangleCopyTo(br_device_pixelmap* self, br_point* p, br_device_pixelmap* src, br_rectangle* r) {
#if defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_mem_rectangleCopyTo_original(self, p, src, r);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053a150, _M_br_device_pixelmap_mem_rectangleCopyTo, _M_br_device_pixelmap_mem_rectangleCopyTo_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_mem_rectangleCopyFrom_original)(br_device_pixelmap* self, br_point* p, br_device_pixelmap* dest, br_rectangle* r);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_rectangleCopyFrom(br_device_pixelmap* self, br_point* p, br_device_pixelmap* dest, br_rectangle* r) {
#if defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_mem_rectangleCopyFrom_original(self, p, dest, r);
#else
    int bytes;
    br_rectangle ar;
    br_point ap;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053a3f0, _M_br_device_pixelmap_mem_rectangleCopyFrom, _M_br_device_pixelmap_mem_rectangleCopyFrom_original)

br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_rectangleStretchCopyTo(br_device_pixelmap* self, br_rectangle* destinationRectangle, br_device_pixelmap* src, br_rectangle* sourceRectangle) {

    return 0x1002;
}
C2_HOOK_FUNCTION(0x0053a690, _M_br_device_pixelmap_mem_rectangleStretchCopyTo)

br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_rectangleStretchCopyFrom(br_device_pixelmap* self, br_rectangle* d, br_device_pixelmap* src, br_rectangle* s) {

    return 0x1002;
}
C2_HOOK_FUNCTION(0x0053a6a0, _M_br_device_pixelmap_mem_rectangleStretchCopyFrom)


br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_mem_rectangleFill_original)(br_device_pixelmap* self, br_rectangle* rect, br_uint_32 colour);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_rectangleFill(br_device_pixelmap* self, br_rectangle* rect, br_uint_32 colour) {
#if defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_mem_rectangleFill_original(self, rect, colour);
#else
    br_rectangle arect;
    br_int_8 bytes;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053a6b0, _M_br_device_pixelmap_mem_rectangleFill, _M_br_device_pixelmap_mem_rectangleFill_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_mem_pixelSet_original)(br_device_pixelmap* self, br_point* p, br_uint_32 colour);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_pixelSet(br_device_pixelmap* self, br_point* p, br_uint_32 colour) {
#if defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_mem_pixelSet_original(self, p, colour);
#else
    br_point ap;
    br_int_8 bytes;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053a870, _M_br_device_pixelmap_mem_pixelSet, _M_br_device_pixelmap_mem_pixelSet_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_mem_line_original)(br_device_pixelmap* self, br_point* s, br_point* e, br_uint_32 colour);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_line(br_device_pixelmap* self, br_point* s, br_point* e, br_uint_32 colour) {
#if defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_mem_line_original(self, s, e, colour);
#else
    int dx;
    int dy;
    int incr1;
    int incr2;
    int D;
    int x;
    int y;
    int xend;
    int c;
    int pixels_left;
    int x1;
    int y1;
    int sign_x;
    int sign_y;
    int step;
    int reverse;
    int i;
    char* dest;
    int bytes;
    br_point as;
    br_point ae;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053a8f0, _M_br_device_pixelmap_mem_line, _M_br_device_pixelmap_mem_line_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_mem_copyBits_original)(br_device_pixelmap* self, br_point* point, br_uint_8* src, br_uint_16 s_stride, br_rectangle* bit_rect, br_uint_32 colour);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_copyBits(br_device_pixelmap* self, br_point* point, br_uint_8* src, br_uint_16 s_stride, br_rectangle* bit_rect, br_uint_32 colour) {
#if defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_mem_copyBits_original(self, point, src, s_stride, bit_rect, colour);
#else
    int bytes;
    int bit;
    br_rectangle ar;
    br_point ap;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053be40, _M_br_device_pixelmap_mem_copyBits, _M_br_device_pixelmap_mem_copyBits_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_mem_pixelQuery_original)(br_device_pixelmap* self, br_uint_32* pcolour, br_point* p);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_pixelQuery(br_device_pixelmap* self, br_uint_32* pcolour, br_point* p) {
#if defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_mem_pixelQuery_original(self, pcolour, p);
#else
    br_point ap;
    br_int_8 bytes;
    LOG_TRACE("(%p, %p, %p)", self, pcolour, p);
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053bf00, _M_br_device_pixelmap_mem_pixelQuery, _M_br_device_pixelmap_mem_pixelQuery_original)

br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_pixelAddressQuery(br_device_pixelmap* self, void** pptr, br_uint_32* pqual, br_point* p) {
    br_int_8 bytes;
    br_point ap;

    if (PixelmapPointClip(&ap, p, (br_pixelmap*)self) == BR_CLIP_REJECT) {
        return 0x1002;
    }
    if (pptr != NULL) {
        bytes = C2V(pmTypeInfo)[self->pm_type].bits >> 3;
        *pptr = (br_uint_8*)self->pm_pixels \
            + (self->pm_base_y + ap.y) * self->pm_row_bytes \
            + (self->pm_base_x + ap.x) * bytes;
    }
    if (pqual != NULL) {
        *pqual = self->pm_pixels_qualifier;
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0053bf80, _M_br_device_pixelmap_mem_pixelAddressQuery)

br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_pixelAddressSet(br_device_pixelmap* self, void* ptr, br_uint_32* qual) {

    if (ptr != NULL) {
        self->pm_pixels = ptr;
    }
    if (qual != NULL) {
        self->pm_pixels_qualifier = *qual;
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0053c010, _M_br_device_pixelmap_mem_pixelAddressSet)

br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_originSet(br_device_pixelmap* self, br_point* p) {

    self->pm_origin_x = p->x;
    self->pm_origin_y = p->y;
    return 0;
}
C2_HOOK_FUNCTION(0x0053c030, _M_br_device_pixelmap_mem_originSet)

br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_rowSize(br_device_pixelmap* self, br_size_t* sizep) {
    return 0;
}
C2_HOOK_FUNCTION(0x0053c050, _M_br_device_pixelmap_mem_rowSize)

br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_rowQuery(br_device_pixelmap* self, void* buffer, br_size_t buffer_size, br_int_32 row) {

    return 0;
}
C2_HOOK_FUNCTION(0x0053c060, _M_br_device_pixelmap_mem_rowQuery)

br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_rowSet(br_device_pixelmap* self, void* buffer, br_size_t buffer_size, br_int_32 row) {

    return 0;
}
C2_HOOK_FUNCTION(0x0053c070, _M_br_device_pixelmap_mem_rowSet)

br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_flush(br_device_pixelmap* self) {

    return 0;
}
C2_HOOK_FUNCTION(0x0053c080, _M_br_device_pixelmap_mem_flush)

br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_synchronise(br_device_pixelmap* self, br_token sync_type, br_boolean block) {

    return 0;
}
C2_HOOK_FUNCTION(0x0053c090, _M_br_device_pixelmap_mem_synchronise)

br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_directLock(br_device_pixelmap* self, br_boolean block) {

    return 0;
}
C2_HOOK_FUNCTION(0x0053c0a0, _M_br_device_pixelmap_mem_directLock)

br_error C2_HOOK_CDECL _M_br_device_pixelmap_mem_directUnlock(br_device_pixelmap* self) {

    return 0;
}
C2_HOOK_FUNCTION(0x0053c0b0, _M_br_device_pixelmap_mem_directUnlock)
