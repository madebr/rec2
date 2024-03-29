#include "pmdsptch.h"

#include "fontptrs.h"
#include "pmmem.h"

#include "core/fw/devsetup.h"
#include "core/fw/scratch.h"
#include "core/std/brstdlib.h"

#include <stdarg.h>

br_pixelmap* C2_HOOK_CDECL BrPixelmapAllocateSub(br_pixelmap* src, br_int_32 x, br_int_32 y, br_int_32 w, br_int_32 h) {
    br_pixelmap* new;
    br_rectangle r;

    r.h = h;
    r.w = w;
    r.x = x;
    r.y = y;
    CheckDispatch((br_device_pixelmap*)src);
    if (((br_device_pixelmap*)src)->dispatch->_allocateSub((br_device_pixelmap*)src, (br_device_pixelmap**)&new, &r) != 0) {
        return NULL;
    }
    return new;
}
C2_HOOK_FUNCTION(0x00537d60, BrPixelmapAllocateSub)

void C2_HOOK_CDECL BrPixelmapFree(br_pixelmap* src) {

    CheckDispatch((br_device_pixelmap*)src);
    if (BrDevLastBeginQuery() == src) {
        BrDevLastBeginSet(NULL);
    }
    ((br_device_pixelmap*)src)->dispatch->_free((br_object*)src);
}
C2_HOOK_FUNCTION(0x00537dc0, BrPixelmapFree)

br_pixelmap* C2_HOOK_CDECL BrPixelmapResize(br_pixelmap* src, br_int_32 width, br_int_32 height) {

    CheckDispatch((br_device_pixelmap*)src);
    if (((br_device_pixelmap*)src)->dispatch->_resize((br_device_pixelmap*)src, width, height) != 0) {
        return NULL;
    }
    return src;
}
C2_HOOK_FUNCTION(0x00537df0, BrPixelmapResize)

br_pixelmap* (C2_HOOK_CDECL * BrPixelmapMatch_original)(br_pixelmap* src, br_uint_8 match_type);
br_pixelmap* C2_HOOK_CDECL BrPixelmapMatch(br_pixelmap* src, br_uint_8 match_type) {
#if defined(C2_HOOKS_ENABLED)
    br_pixelmap* res = BrPixelmapMatch_original(src, match_type);
    return res;
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00537e20, BrPixelmapMatch, BrPixelmapMatch_original)

br_pixelmap* (C2_HOOK_CDECL * BrPixelmapMatchSized_original)(br_pixelmap* src, br_uint_8 match_type, br_int_32 width, br_int_32 height);
br_pixelmap* C2_HOOK_CDECL BrPixelmapMatchSized(br_pixelmap* src, br_uint_8 match_type, br_int_32 width, br_int_32 height) {
#if defined(C2_HOOKS_ENABLED)
    return BrPixelmapMatchSized_original(src, match_type, width, height);
#else
    br_pixelmap* new;
    br_token_value tv[5];
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00537f40, BrPixelmapMatchSized, BrPixelmapMatchSized_original)

br_pixelmap* (C2_HOOK_CDECL * BrPixelmapMatchTyped_original)(br_pixelmap* src, br_uint_8 match_type, br_uint_8 pixelmap_type);
br_pixelmap* C2_HOOK_CDECL BrPixelmapMatchTyped(br_pixelmap* src, br_uint_8 match_type, br_uint_8 pixelmap_type) {
#if defined(C2_HOOKS_ENABLED)
    return BrPixelmapMatchTyped_original(src, match_type, pixelmap_type);
#else
    br_pixelmap* new;
    br_token_value tv[4];
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00538060, BrPixelmapMatchTyped, BrPixelmapMatchTyped_original)

br_pixelmap* (C2_HOOK_CDECL * BrPixelmapMatchTypedSized_original)(br_pixelmap* src, br_uint_8 match_type, br_uint_8 pixelmap_type, br_int_32 width, br_int_32 height);
br_pixelmap* C2_HOOK_CDECL BrPixelmapMatchTypedSized(br_pixelmap* src, br_uint_8 match_type, br_uint_8 pixelmap_type, br_int_32 width, br_int_32 height) {
#if defined(C2_HOOKS_ENABLED)
    return BrPixelmapMatchTypedSized_original(src, match_type, pixelmap_type, width, height);
#else
    br_pixelmap* new;
    br_token_value tv[6];
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00538160, BrPixelmapMatchTypedSized, BrPixelmapMatchTypedSized_original)

br_pixelmap* (C2_HOOK_CDECL * BrPixelmapClone_original)(br_pixelmap* src);
br_pixelmap* C2_HOOK_CDECL BrPixelmapClone(br_pixelmap* src) {
#if defined(C2_HOOKS_ENABLED)
    return BrPixelmapClone_original(src);
#else
    br_pixelmap* new;
    br_token_value tv[2];
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005382a0, BrPixelmapClone, BrPixelmapClone_original)

void C2_HOOK_CDECL BrPixelmapFill(br_pixelmap* dst, br_uint_32 colour) {

    CheckDispatch((br_device_pixelmap*)dst);
    (*(br_device_pixelmap_dispatch**)dst)->_fill((br_device_pixelmap*)dst, colour);
}
C2_HOOK_FUNCTION(0x005382f0, BrPixelmapFill)

void C2_HOOK_CDECL BrPixelmapRectangle(br_pixelmap* dst, br_int_32 x, br_int_32 y, br_int_32 w, br_int_32 h, br_uint_32 colour) {
    br_rectangle r;

    CheckDispatch((br_device_pixelmap*)dst);
    r.x = x;
    r.y = y;
    r.w = w;
    r.h = h;
    (*(br_device_pixelmap_dispatch**)dst)->_rectangle((br_device_pixelmap*)dst, &r, colour);
}
C2_HOOK_FUNCTION(0x00538310, BrPixelmapRectangle)

void C2_HOOK_CDECL BrPixelmapRectangle2(br_pixelmap* dst, br_int_32 x, br_int_32 y, br_int_32 w, br_int_32 h, br_uint_32 colour_tl, br_uint_32 colour_br) {
    br_rectangle r;

    CheckDispatch((br_device_pixelmap*)dst);
    r.x = x;
    r.y = y;
    r.w = w;
    r.h = h;
    (*(br_device_pixelmap_dispatch**)dst)->_rectangle2((br_device_pixelmap*)dst, &r, colour_tl, colour_br);
}
C2_HOOK_FUNCTION(0x00538360, BrPixelmapRectangle2)

br_error (C2_HOOK_STDCALL * DispatchCopy_original)(br_device_pixelmap* self, br_device_pixelmap* src);
br_error C2_HOOK_STDCALL DispatchCopy(br_device_pixelmap* self, br_device_pixelmap* src) {
#if defined(C2_HOOKS_ENABLED)
    return DispatchCopy_original(self, src);
#else
    CheckDispatch((br_device_pixelmap*)dst);
    if ((*(br_device_pixelmap_dispatch**)self)->_device((br_object*)self) == (*(br_device_pixelmap_dispatch**)self)->_device((br_object*)src)) {
        return (*(br_device_pixelmap_dispatch**)self)->_copyTo(self, src);
    }
    if (src->pm_flags & 1 == 0) {
        return (*(br_device_pixelmap_dispatch**)self)->_copyTo2(self, src);
    }
    if (self->pm_flags & 1 == 0) {
        return (*(br_device_pixelmap_dispatch**)src)->_copyFrom(src, self);
    }
##error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005383b0, DispatchCopy, DispatchCopy_original)

br_error (C2_HOOK_STDCALL * DispatchRectangleCopy_original)(br_device_pixelmap* self, br_point* p, br_device_pixelmap* src, br_rectangle* r);
br_error C2_HOOK_STDCALL DispatchRectangleCopy(br_device_pixelmap* self, br_point* p, br_device_pixelmap* src, br_rectangle* r) {
#if defined(C2_HOOKS_ENABLED)
    return DispatchRectangleCopy_original(self, p, src, r);
#else
    CheckDispatch(self);
    CheckDispatch(src);
    if ((*(br_device_pixelmap_dispatch**)self)->_device((br_object*)self) == (*(br_device_pixelmap_dispatch**)src)->_device((br_object*)src)) {
        return (*(br_device_pixelmap_dispatch**)self)->_rectangleCopy(self, p, src, r);
    }
    if ((src->pm_flags & BR_PMF_NO_ACCESS) == 0) {
        return (*(br_device_pixelmap_dispatch**)self)->_rectangleCopyTo(self, p, src, r);
    }
    if ((self->pm_flags & BR_PMF_NO_ACCESS) == 0) {
        return (*(br_device_pixelmap_dispatch**)self)->_rectangleCopyFrom(src, p, self, r);
    }
    return GeneralRectangleCopy(self, p, src, r);
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00538430, DispatchRectangleCopy, DispatchRectangleCopy_original)

br_error (C2_HOOK_STDCALL * DispatchRectangleStretchCopy_original)(br_device_pixelmap* self, br_rectangle* r, br_device_pixelmap* src, br_rectangle* s);
br_error C2_HOOK_STDCALL DispatchRectangleStretchCopy(br_device_pixelmap* self, br_rectangle* r, br_device_pixelmap* src, br_rectangle* s) {
#if defined(C2_HOOKS_ENABLED)
    return DispatchRectangleStretchCopy_original(self, r, src, s);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005384e0, DispatchRectangleStretchCopy, DispatchRectangleStretchCopy_original)

void C2_HOOK_CDECL BrPixelmapRectangleCopy(br_pixelmap* dst, br_int_32 dx, br_int_32 dy, br_pixelmap* src, br_int_32 sx, br_int_32 sy, br_int_32 w, br_int_32 h) {
    br_rectangle r;
    br_point p;

    r.x = sx;
    r.y = sy;
    r.w = w;
    r.h = h;
    p.x = dx;
    p.y = dy;
    DispatchRectangleCopy((br_device_pixelmap*)dst, &p, (br_device_pixelmap*)src, &r);
}
C2_HOOK_FUNCTION(0x00538590, BrPixelmapRectangleCopy)

void (C2_HOOK_CDECL * BrPixelmapRectangleStretchCopy_original)(br_pixelmap* dst, br_int_32 dx, br_int_32 dy, br_int_32 dw, br_int_32 dh, br_pixelmap* src, br_int_32 sx, br_int_32 sy, br_int_32 sw, br_int_32 sh);
void C2_HOOK_CDECL BrPixelmapRectangleStretchCopy(br_pixelmap* dst, br_int_32 dx, br_int_32 dy, br_int_32 dw, br_int_32 dh, br_pixelmap* src, br_int_32 sx, br_int_32 sy, br_int_32 sw, br_int_32 sh) {
#if defined(C2_HOOKS_ENABLED)
    BrPixelmapRectangleStretchCopy_original(dst, dx, dy, dw, dh, src, sx, sy, sw, sh);
#else
    br_rectangle s;
    br_rectangle d;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00538690, BrPixelmapRectangleStretchCopy, BrPixelmapRectangleStretchCopy_original)

void C2_HOOK_CDECL BrPixelmapRectangleFill(br_pixelmap* dst, br_int_32 x, br_int_32 y, br_int_32 w, br_int_32 h, br_uint_32 colour) {
    br_rectangle r;

    CheckDispatch((br_device_pixelmap*)dst);
    r.x = x;
    r.y = y;
    r.w = w;
    r.h = h;
    (*(br_device_pixelmap_dispatch**)dst)->_fillDirty((br_device_pixelmap*)dst, colour, &r, 1);
}
C2_HOOK_FUNCTION(0x00538640, BrPixelmapRectangleFill)

#if 0
void C2_HOOK_CDECL BrPixelmapDirtyRectangleCopy(br_pixelmap* dst, br_pixelmap* src, br_int_32 x, br_int_32 y, br_int_32 w, br_int_32 h) {
    br_rectangle r;
    br_point p;
#error "Not implemented"
}

void C2_HOOK_CDECL BrPixelmapDirtyRectangleClear(br_pixelmap* dst, br_int_32 x, br_int_32 y, br_int_32 w, br_int_32 h, br_uint_32 colour) {
    br_rectangle r;
#error "Not implemented"
}
#endif

void C2_HOOK_CDECL BrPixelmapDirtyRectangleDoubleBuffer(br_pixelmap* dst, br_pixelmap* src, br_int_32 x, br_int_32 y, br_int_32 w, br_int_32 h) {
    br_rectangle r;

    CheckDispatch((br_device_pixelmap*)dst);
    r.x = x;
    r.y = y;
    r.w = w;
    r.h = h;
    (*(br_device_pixelmap_dispatch**)dst)->_doubleBufferDirty((br_device_pixelmap*)dst, (br_device_pixelmap*)src, &r, 1);
}
C2_HOOK_FUNCTION(0x005387b0, BrPixelmapDirtyRectangleDoubleBuffer)

void C2_HOOK_CDECL BrPixelmapPixelSet(br_pixelmap* dst, br_int_32 x, br_int_32 y, br_uint_32 colour) {
    br_point p;

    CheckDispatch((br_device_pixelmap*)dst);
    p.x = x;
    p.y = y;
    (*(br_device_pixelmap_dispatch**)dst)->_pixelSet((br_device_pixelmap*)dst, &p, colour);
}
C2_HOOK_FUNCTION(0x00538800, BrPixelmapPixelSet)

br_uint_32 C2_HOOK_CDECL BrPixelmapPixelGet(br_pixelmap* dst, br_int_32 x, br_int_32 y) {
    br_point p;
    br_uint_32 colour;

    CheckDispatch((br_device_pixelmap*)dst);
    p.x = x;
    p.y = y;

    (*(br_device_pixelmap_dispatch**)dst)->_pixelQuery((br_device_pixelmap*)dst, &colour, &p);
    return colour;
}
C2_HOOK_FUNCTION(0x00538840, BrPixelmapPixelGet)

void (C2_HOOK_CDECL * BrPixelmapCopy_original)(br_pixelmap* dst, br_pixelmap* src);
void C2_HOOK_CDECL BrPixelmapCopy(br_pixelmap* dst, br_pixelmap* src) {
#if defined(C2_HOOKS_ENABLED)
    BrPixelmapCopy_original(dst, src);
#else
    br_rectangle s;
    br_rectangle d;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00538880, BrPixelmapCopy, BrPixelmapCopy_original)

void C2_HOOK_CDECL BrPixelmapLine(br_pixelmap* dst, br_int_32 x1, br_int_32 y1, br_int_32 x2, br_int_32 y2, br_uint_32 colour) {
     br_point s;
     br_point e;

     s.x = x1;
     s.y = y1;
     e.x = x2;
     e.y = y2;
    ((br_device_pixelmap*)dst)->dispatch->_line((br_device_pixelmap*)dst, &s, &e, colour);
}
C2_HOOK_FUNCTION(0x00538990, BrPixelmapLine)

void (C2_HOOK_CDECL * BrPixelmapDoubleBuffer_original)(br_pixelmap* dst, br_pixelmap* src);
void C2_HOOK_CDECL BrPixelmapDoubleBuffer(br_pixelmap* dst, br_pixelmap* src) {
#if defined(C2_HOOKS_ENABLED)
    BrPixelmapDoubleBuffer_original(dst, src);
#else
    CheckDispatch(src);
    CheckDispatch(dst);
    PIXELMAP_DISPATCH_GEN_DOUBLEBUFFER(dst, src);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005389e0, BrPixelmapDoubleBuffer, BrPixelmapDoubleBuffer_original)

void C2_HOOK_CDECL BrPixelmapText(br_pixelmap* dst, br_int_32 x, br_int_32 y, br_uint_32 colour, br_font* font, char* text) {
    br_point p;

    CheckDispatch((br_device_pixelmap*)dst);
    if (font == NULL) {
        font = C2V(BrFontProp7x9);
    }
    p.x = x;
    p.y = y;
    ((br_device_pixelmap*)dst)->dispatch->_text((br_device_pixelmap*)dst, &p, font, text, colour);
}
C2_HOOK_FUNCTION(0x00538a10, BrPixelmapText)

void C2_HOOK_CDECL BrPixelmapTextF(br_pixelmap* dst, br_int_32 x, br_int_32 y, br_uint_32 colour, br_font* font, char* fmt, ...) {
    char* ss;
    br_point p;
    va_list args;

    CheckDispatch((br_device_pixelmap*)dst);
    ss = BrScratchString();
    if (font == NULL) {
        font = C2V(BrFontProp7x9);
    }
    va_start(args, fmt);
    BrVSprintfN(ss, BrScratchStringSize(), fmt, args);
    va_end(args);
    p.x = x;
    p.y = y;
    (*(br_device_pixelmap_dispatch**)dst)->_text((br_device_pixelmap*)dst, &p, font, ss, colour);
}
C2_HOOK_FUNCTION(0x00538a60, BrPixelmapTextF)

br_uint_16 C2_HOOK_CDECL BrPixelmapTextWidth(br_pixelmap* dst, br_font* font, char* text) {
    int i;
    int j;
    int w;

    CheckDispatch((br_device_pixelmap*)dst);
    if (text == NULL) {
        return font->glyph_x;
    }
    if (BrStrLen(text) == 0) {
        return 0;
    }
    if (font->flags & BR_FONTF_PROPORTIONAL) {
        for (i = 0, w = 0, j = BrStrLen(text); i < j; i++, text++)
            w += font->width[(int)*text] + 1;
        return w - 1;
    } else {
        return (br_uint_16)((font->glyph_x + 1) * BrStrLen(text) - 1);
    }
}
C2_HOOK_FUNCTION(0x00538ad0, BrPixelmapTextWidth)

br_uint_16 C2_HOOK_CDECL  BrPixelmapTextHeight(br_pixelmap* dst, br_font* font) {

    CheckDispatch((br_device_pixelmap*)dst);
    return font->glyph_y;
}

void C2_HOOK_CDECL BrPixelmapCopyBits(br_pixelmap* dst, br_int_32 x, br_int_32 y, br_uint_8* src, br_int_32 s_stride, br_int_32 start_bit, br_int_32 end_bit, br_int_32 nrows, br_uint_32 colour) {
    br_point p;
    br_rectangle r;

    p.x = x;
    p.y = y;
    r.x = start_bit;
    r.y = 0;
    r.w = end_bit - start_bit;
    r.h = nrows;
    (*(br_device_pixelmap_dispatch**)dst)->_copyBits((br_device_pixelmap*)dst, &p, src, s_stride, &r, colour);
}
C2_HOOK_FUNCTION(0x00538b70, BrPixelmapCopyBits)

br_uint_16 C2_HOOK_CDECL BrPixelmapFileSize(br_pixelmap* pm) {

    return C2V(pmTypeInfo)[pm->type].file_size;
}
C2_HOOK_FUNCTION(0x00538be0, BrPixelmapFileSize)

br_uint_16 C2_HOOK_CDECL BrPixelmapPixelSize(br_pixelmap* pm) {

    return C2V(pmTypeInfo)[pm->type].bits;
}
C2_HOOK_FUNCTION(0x00538c00, BrPixelmapPixelSize)

br_uint_16 C2_HOOK_CDECL BrPixelmapChannels(br_pixelmap* pm) {

    return C2V(pmTypeInfo)[pm->type].channels;
}
C2_HOOK_FUNCTION(0x00538c20, BrPixelmapChannels)

void (C2_HOOK_CDECL * BrPixelmapPaletteSet_original)(br_pixelmap* pm, br_pixelmap* pal);
void C2_HOOK_CDECL BrPixelmapPaletteSet(br_pixelmap* pm, br_pixelmap* pal) {

    CheckDispatch((br_device_pixelmap*)pm);

    BrPixelmapPaletteEntrySetMany(pm, 0, pal->height, (br_colour *)pal->pixels);
}
C2_HOOK_FUNCTION(0x00538c40, BrPixelmapPaletteSet)

void C2_HOOK_CDECL BrPixelmapPaletteEntrySet(br_pixelmap* pm, br_int_32 index, br_colour colour) {
    br_device_clut* clut;

    CheckDispatch((br_device_pixelmap*)pm);
    if (((br_device_pixelmap*)pm)->dispatch->_query((br_object*)pm, (br_uint_32 *)&clut, BRT_CLUT_O) != 0) {
        return;
    }
    if (clut == NULL) {
        return;
    }
    clut->dispatch->_entrySet(clut, index, colour);
}
C2_HOOK_FUNCTION(0x00538c90, BrPixelmapPaletteEntrySet)

void C2_HOOK_CDECL BrPixelmapPaletteEntrySetMany(br_pixelmap* pm, br_int_32 index, br_int_32 ncolours, br_colour* colours) {
    br_device_clut* clut;

    CheckDispatch((br_device_pixelmap*)pm);
    if (((br_device_pixelmap*)pm)->dispatch->_query((br_object*)pm, (br_uint_32*)&clut, BRT_CLUT_O) != 0) {
        return;
    }
    if (clut == NULL) {
        return;
    }
    clut->dispatch->_entrySetMany(clut, index, ncolours, colours);
}
C2_HOOK_FUNCTION(0x00538cd0, BrPixelmapPaletteEntrySetMany)

br_pixelmap* C2_HOOK_CDECL BrPixelmapDirectLock(br_pixelmap* src, br_boolean block) {

    CheckDispatch((br_device_pixelmap*)src);
    if (((br_device_pixelmap*)(src))->dispatch->_directLock((br_device_pixelmap*)src, block) == 0) {
        return src;
    } else {
        return NULL;
    }
}
C2_HOOK_FUNCTION(0x00538d20, BrPixelmapDirectLock)

br_pixelmap* C2_HOOK_CDECL BrPixelmapDirectUnlock(br_pixelmap* src) {

    CheckDispatch((br_device_pixelmap*)src);
    if (((br_device_pixelmap*)(src))->dispatch->_directUnlock((br_device_pixelmap*)src) == 0) {
        return src;
    } else {
        return NULL;
    }
}
C2_HOOK_FUNCTION(0x00538d50, BrPixelmapDirectUnlock)
