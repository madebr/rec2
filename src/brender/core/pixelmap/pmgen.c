#include "pmgen.h"

#include "genclip.h"
#include "pmdsptch.h"

#include "core/std/brstdlib.h"

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_match_original)(br_device_pixelmap* self, br_device_pixelmap** newpm, br_token_value* tv);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_match(br_device_pixelmap* self, br_device_pixelmap** newpm, br_token_value* tv) {
#if defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_match_original(self, newpm, tv);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c230, _M_br_device_pixelmap_gen_match, _M_br_device_pixelmap_gen_match_original)

br_error (C2_HOOK_CDECL *  _M_br_device_pixelmap_gen_copy_original)(br_device_pixelmap* self, br_device_pixelmap* src);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_copy(br_device_pixelmap* self, br_device_pixelmap* src) {
#if defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_copy_original(self, src);
#else
    br_rectangle r;
    br_point p;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c240, _M_br_device_pixelmap_gen_copy, _M_br_device_pixelmap_gen_copy_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_copyTo_original)(br_device_pixelmap* self, br_device_pixelmap* src);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_copyTo(br_device_pixelmap* self, br_device_pixelmap* src) {
#if defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_copyTo_original(self, src);
#else
    br_rectangle r;
    br_point p;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c2b0, _M_br_device_pixelmap_gen_copyTo, _M_br_device_pixelmap_gen_copyTo_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_copyFrom_original)(br_device_pixelmap* self, br_device_pixelmap* src);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_copyFrom(br_device_pixelmap* self, br_device_pixelmap* src) {
#if defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_copyFrom_original(self, src);
#else
    br_rectangle r;
    br_point p;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c320, _M_br_device_pixelmap_gen_copyFrom, _M_br_device_pixelmap_gen_copyFrom_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_fill_original)(br_device_pixelmap* self, br_uint_32 colour);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_fill(br_device_pixelmap* self, br_uint_32 colour) {
#if defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_fill_original(self, colour);
#else
    br_rectangle r;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c390, _M_br_device_pixelmap_gen_fill, _M_br_device_pixelmap_gen_fill_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_doubleBuffer_original)(br_device_pixelmap* self, br_device_pixelmap* src);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_doubleBuffer(br_device_pixelmap* self, br_device_pixelmap* src) {
#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_doubleBuffer_original(self, src);
#else
    br_rectangle s;
    br_rectangle d;

    if (self->pm_width == src->pm_width && self->pm_height == src->pm_height) {
        return DispatchCopy(self,src);
    } else {
        s.x = -self->pm_origin_x;
        s.y = -self->pm_origin_y;
        s.w = self->pm_width;
        s.h = self->pm_height;
        d.x = -src->pm_origin_x;
        d.y = -src->pm_origin_y;
        d.w = src->pm_width;
        d.h = src->pm_height;
        return DispatchRectangleStretchCopy(self, &s, src, &d);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c3e0, _M_br_device_pixelmap_gen_doubleBuffer, _M_br_device_pixelmap_gen_doubleBuffer_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_copyDirty_original)(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_copyDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {
#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_copyDirty_original(self, src, dirty, num_rects);
#else
    int i;
    br_point p;
    br_error e;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_device_pixelmap_dispatch, _rectangleCopy, 0x84);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_device_pixelmap_dispatch, _copy, 0x54);

    if (self->pm_width == src->pm_width && self->pm_height == src->pm_height) {
        for (i = 0; i < num_rects; i++) {
            p.x = dirty->x;
            p.y = dirty->y;
            e = self->dispatch->_rectangleCopy(self, &p, src, &dirty[i]);
            if (e != 0) {
                return e;
            }
        }
        return 0;
    } else {
        return self->dispatch->_copy(self, src);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c480, _M_br_device_pixelmap_gen_copyDirty, _M_br_device_pixelmap_gen_copyDirty_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_copyToDirty_original)(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_copyToDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {
#if 0//defined(C2_HOOKS_ENABLED)
return _M_br_device_pixelmap_gen_copyToDirty_original(self, src, dirty, num_rects);
#else
    int i;
    br_point p;
    br_error e;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_device_pixelmap_dispatch, _rectangleCopyTo, 0x88);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_device_pixelmap_dispatch, _copyTo, 0x58);

    if (self->pm_width == src->pm_width && self->pm_height == src->pm_height) {
        for (i = 0; i < num_rects; i++) {
            p.x = dirty->x;
            p.y = dirty->y;
            e = self->dispatch->_rectangleCopyTo(self, &p, src, &dirty[i]);
            if (e != 0) {
                return e;
            }
        }
        return 0;
    } else {
        return self->dispatch->_copyTo(self, src);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c500, _M_br_device_pixelmap_gen_copyToDirty, _M_br_device_pixelmap_gen_copyToDirty_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_copyFromDirty_original)(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_copyFromDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {
#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_copyFromDirty_original(self, src, dirty, num_rects);
#else
    int i;
    br_point p;
    br_error e;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_device_pixelmap_dispatch, _rectangleCopyFrom, 0x8c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_device_pixelmap_dispatch, _copyFrom, 0x5c);

    if (self->pm_width == src->pm_width && self->pm_height == src->pm_height) {
        for (i = 0; i < num_rects; i++) {
            p.x = dirty->x;
            p.y = dirty->y;
            e = self->dispatch->_rectangleCopyFrom(self, &p, src, &dirty[i]);
            if (e != 0) {
                return e;
            }
        }
        return 0;
    } else {
        return self->dispatch->_copyFrom(self, src);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c580, _M_br_device_pixelmap_gen_copyFromDirty, _M_br_device_pixelmap_gen_copyFromDirty_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_fillDirty_original)(br_device_pixelmap* self, br_uint_32 colour, br_rectangle* dirty, br_int_32 num_rects);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_fillDirty(br_device_pixelmap* self, br_uint_32 colour, br_rectangle* dirty, br_int_32 num_rects) {
#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_fillDirty_original(self, colour, dirty, num_rects);
#else
    int i;
    br_error e;

    for (i = 0; i < num_rects; i++) {
        e = self->dispatch->_rectangleFill(self, &dirty[i], colour);
        if (e != 0) {
            return e;
        }
    }
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c600, _M_br_device_pixelmap_gen_fillDirty, _M_br_device_pixelmap_gen_fillDirty_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_doubleBufferDirty_original)(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_doubleBufferDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {
#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_doubleBufferDirty_original(self, src, dirty, num_rects);
#else
    int i;
    br_point p;
    br_error e;

    if (self->pm_width == src->pm_width && self->pm_height == src->pm_height) {
        for (i = 0; i < num_rects; i++) {
            p.x = dirty[i].x;
            p.y = dirty[i].y;
            e = self->dispatch->_rectangleCopy(self, &p, src, &dirty[i]);
            if (e != 0) {
                return e;
            }
        }
        return 0;
    } else {
        return self->dispatch->_doubleBuffer(self, src);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c640, _M_br_device_pixelmap_gen_doubleBufferDirty, _M_br_device_pixelmap_gen_doubleBufferDirty_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_rectangle_original)(br_device_pixelmap* self, br_rectangle* rect, br_uint_32 colour);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_rectangle(br_device_pixelmap* self, br_rectangle* rect, br_uint_32 colour) {
#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_rectangle_original(self, rect, colour);
#else
    br_point tl;
    br_point tr;
    br_point bl;
    br_point br;
    br_error e;

    tl.x = bl.x = rect->x;
    tr.y = tl.y = rect->y;
    tr.x = br.x = rect->x + rect->w - 1;
    bl.y = br.y = rect->y + rect->h - 1;

    e = self->dispatch->_line(self, &tl, &tr, colour);
    if (e != 0) {
        return e;
    }
    e = self->dispatch->_line(self, &tl, &bl, colour);
    if (e != 0) {
        return e;
    }
    e = self->dispatch->_line(self, &bl, &br, colour);
    if (e != 0) {
        return e;
    }
    e = self->dispatch->_line(self, &tr, &br, colour);
    return e;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c6c0, _M_br_device_pixelmap_gen_rectangle, _M_br_device_pixelmap_gen_rectangle_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_rectangle2_original)(br_device_pixelmap* self, br_rectangle* rect, br_uint_32 colour_tl, br_uint_32 colour_br);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_rectangle2(br_device_pixelmap* self, br_rectangle* rect, br_uint_32 colour_tl, br_uint_32 colour_br) {
#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_rectangle2_original(self, rect, colour_tl, colour_br);
#else
    br_point tl;
    br_point tr;
    br_point bl;
    br_point br;
    br_error e;

    tl.x = bl.x = rect->x;
    tr.y = tl.y = rect->y;
    tr.x = br.x = rect->x + rect->w - 1;
    bl.y = br.y = rect->y + rect->h - 1;

    e = self->dispatch->_line(self, &tl, &tr, colour_tl);
    if (e != 0) {
        return e;
    }
    e = self->dispatch->_line(self, &tl, &bl, colour_tl);
    if (e != 0) {
        return e;
    }
    e = self->dispatch->_line(self, &bl, &br, colour_br);
    if (e != 0) {
        return e;
    }
    e = self->dispatch->_line(self, &tr, &br, colour_br);
    return e;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c780, _M_br_device_pixelmap_gen_rectangle2, _M_br_device_pixelmap_gen_rectangle2_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_line_original)(br_device_pixelmap* self, br_point* s, br_point* e, br_uint_32 colour);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_line(br_device_pixelmap* self, br_point* s, br_point* e, br_uint_32 colour) {
#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_line_original(self, s, e, colour);
#else
#if 0
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
    br_point as;
    br_point ae;
    br_point p;
#else
    int err;
    int e2;
    int sx;
    int sy;
    int dx;
    int dy;
    br_point as;
    br_point ae;
    br_point p;
#endif

    if (PixelmapLineClip(&as, &ae, s, e, (br_pixelmap*)self) == BR_CLIP_REJECT) {
        return 0;
    }
    as.x -= self->pm_origin_x;
    as.y -= self->pm_origin_y;
    ae.x -= self->pm_origin_x;
    ae.y -= self->pm_origin_y;

    sx = as.x < ae.x ? 1 : -1;
    dx = (ae.x - as.x) * sx;
    sy = as.y < ae.y ? 1 : -1;
    dy = (ae.y - as.y) * sy;
    err = dx + dy;

    p.x = as.x;
    p.y = as.y;

    for (;;) {
        self->dispatch->_pixelSet(self, &p, colour);
        if (p.x == ae.x && p.y == ae.y) {
            break;
        }
        e2 = 2 * err;
        if (e2 >= dy) {
            if (p.x == ae.x) {
                break;
            }
            err += dy;
            p.x += sx;
        }
        if (e2 <=  dx) {
            if (p.y == ae.y) {
                break;
            }
            err += dx;
            p.y += sy;
        }
    }
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c840, _M_br_device_pixelmap_gen_line, _M_br_device_pixelmap_gen_line_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_text_original)(br_device_pixelmap* self, br_point* point, br_font* font, const char* text, br_uint_32 colour);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_text(br_device_pixelmap* self, br_point* point, br_font* font, const char* text, br_uint_32 colour) {
#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_text_original(self, point, font, text, colour);
#else
    br_rectangle r;
    br_point p;
    br_int_32 x;
    br_int_32 y;
    br_int_32 s_stride;

    x = self->pm_origin_x + point->x;
    y = self->pm_origin_y + point->y;
    r.h = font->glyph_y;
    if (y <= -r.h || y >= self->pm_height || x >= self->pm_width) {
        return 0;
    }
    p.x = point->x;
    p.y = point->y;
    r.x = 0;
    r.y = 0;
    if (font->flags & BR_FONTF_VARIABLE_WIDTH) {
        while (*text != '\0') {
            r.w = font->width[*(br_uint_8*)text];
            s_stride = (r.w + 7) / 8;
            if (x + r.w > 0) {
                self->dispatch->_copyBits(self, &p, &font->glyphs[font->encoding[*(br_uint_8*)text]], s_stride, &r, colour);
            }
            x += r.w + 1;
            p.x += r.w + 1;
            if (p.x > self->pm_width) {
                break;
            }
            text++;
        }
    } else {
        r.w = font->glyph_x;
        s_stride = (r.w + 7) / 8;
        while (*text != '\0') {
            if (x + r.w > 0) {
                self->dispatch->_copyBits(self, &p, &font->glyphs[font->encoding[*(br_uint_8*)text]], s_stride, &r, colour);
            }
            x += r.w + 1;
            p.x += r.w  + 1;
            if (x > self->pm_width) {
                break;
            }
            text++;
        }
    }
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053d660, _M_br_device_pixelmap_gen_text, _M_br_device_pixelmap_gen_text_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_textBounds_original)(br_device_pixelmap* self, br_rectangle* rect, br_font* font,const  char* text);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_textBounds(br_device_pixelmap* self, br_rectangle* rect, br_font* font, const char* text) {
#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_textBounds_original(self, rect, font, text);
#else
    int i;
    int j;

    rect->x = 0;
    rect->y = 0;
    rect->w = 0;
    rect->h = font->glyph_y;
    if (text == NULL) {
        return 0;
    }
    if (!(font->flags & BR_FONTF_VARIABLE_WIDTH)) {
        rect->w = BrStrLen(text) * (font->glyph_x + 1) - 1;
        return 0;
    }
    j = BrStrLen(text);
    for (i = 0; i < j; i++) {
        rect->w += font->width[(br_uint_8)text[i]] + 1;
    }
    rect->w -= 1;
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053d810, _M_br_device_pixelmap_gen_textBounds, _M_br_device_pixelmap_gen_textBounds_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_copyBits_original)(br_device_pixelmap* self, br_point* point, br_uint_8* src, br_uint_16 s_stride, br_rectangle* bit_rect, br_uint_32 colour);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_copyBits(br_device_pixelmap* self, br_point* point, br_uint_8* src, br_uint_16 s_stride, br_rectangle* bit_rect, br_uint_32 colour) {
#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_copyBits_original(self, point, src, s_stride, bit_rect, colour);
#else
    br_int_32 x;
    br_int_32 y;
    br_point p;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_device_pixelmap_dispatch, _pixelSet, 0xa0);

    for (y = bit_rect->y; y < bit_rect->y + bit_rect->h; y++) {
        for (x = bit_rect->x; x < bit_rect->x + bit_rect->w; x++) {
            if (src[y * s_stride + x / 8] & (0x80 >> (x % 8))) {
                p.x = point->x + x;
                p.y = point->y + y;
                self->dispatch->_pixelSet(self, &p, colour);
            }
        }
    }
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053d8a0, _M_br_device_pixelmap_gen_copyBits, _M_br_device_pixelmap_gen_copyBits_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_flush_original)(br_device_pixelmap* self);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_flush(br_device_pixelmap* self) {
#if 0//defined(C2_HOOKS_ENABLED)
return _M_br_device_pixelmap_gen_flush_original(self);
#else
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053d970, _M_br_device_pixelmap_gen_flush, _M_br_device_pixelmap_gen_flush_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_synchronise_original)(br_device_pixelmap* self, br_token sync_type, br_boolean block);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_synchronise(br_device_pixelmap* self, br_token sync_type, br_boolean block) {
#if 0//defined(C2_HOOKS_ENABLED)
return _M_br_device_pixelmap_gen_synchronise_original(self, sync_type, block);
#else
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053d980, _M_br_device_pixelmap_gen_synchronise, _M_br_device_pixelmap_gen_synchronise_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_directLock_original)(br_device_pixelmap* self, br_boolean block);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_directLock(br_device_pixelmap* self, br_boolean block) {
#if 0//
return _M_br_device_pixelmap_gen_directLock_original(self, block);
#else
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053d990, _M_br_device_pixelmap_gen_directLock, _M_br_device_pixelmap_gen_directLock_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_directUnlock_original)(br_device_pixelmap* self);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_directUnlock(br_device_pixelmap* self) {
#if 0//defined(C2_HOOKS_ENABLED)
return _M_br_device_pixelmap_gen_directUnlock_original(self);
#else
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053d9a0, _M_br_device_pixelmap_gen_directUnlock, _M_br_device_pixelmap_gen_directUnlock_original)
