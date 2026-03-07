#include "pmgen.h"

#include "genclip.h"
#include "pmdsptch.h"

#include "core/std/brstdlib.h"

// FUNCTION: CARMA2_HW 0x0053c230
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_match(br_device_pixelmap* self, br_device_pixelmap** newpm, br_token_value* tv) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c240
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_copy(br_device_pixelmap* self, br_device_pixelmap* src) {
    br_rectangle r;
    br_point p;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_device_pixelmap_dispatch, _rectangleCopy, 0x84);

    p.x = self->pm_origin_x;
    p.y = self->pm_origin_y;
    r.x = src->pm_origin_x;
    r.y = src->pm_origin_y;
    r.w = src->pm_width;
    r.h = src->pm_height;
    return self->dispatch->_rectangleCopy(self, &p, src, &r);
}

// FUNCTION: CARMA2_HW 0x0053c2b0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_copyTo(br_device_pixelmap* self, br_device_pixelmap* src) {
    br_rectangle r;
    br_point p;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_device_pixelmap_dispatch, _rectangleCopyTo, 0x88);

    p.x = -self->pm_origin_x;
    p.y = -self->pm_origin_y;
    r.x = -src->pm_origin_x;
    r.y = -src->pm_origin_y;
    r.w = src->pm_width;
    r.h = src->pm_height;
    return self->dispatch->_rectangleCopyTo(self, &p, src, &r);
}

// FUNCTION: CARMA2_HW 0x0053c320
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_copyFrom(br_device_pixelmap* self, br_device_pixelmap* src) {
    br_rectangle r;
    br_point p;

    p.x = -self->pm_origin_x;
    p.y = -self->pm_origin_y;
    r.x = -src->pm_origin_x;
    r.y = -src->pm_origin_y;
    r.w = src->pm_width;
    r.h = src->pm_height;
    return self->dispatch->_rectangleCopyFrom(self, &p, src, &r);
}

// FUNCTION: CARMA2_HW 0x0053c390
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_fill(br_device_pixelmap* self, br_uint_32 colour) {
    br_rectangle r;

    r.x = -self->pm_origin_x;
    r.y = -self->pm_origin_y;
    r.w = self->pm_width;
    r.h = self->pm_height;
    return self->dispatch->_rectangleFill(self, &r, colour);
}

// FUNCTION: CARMA2_HW 0x0053c3e0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_doubleBuffer(br_device_pixelmap* self, br_device_pixelmap* src) {
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
}

// FUNCTION: CARMA2_HW 0x0053c480
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_copyDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {
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
}

// FUNCTION: CARMA2_HW 0x0053c500
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_copyToDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {
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
}

// FUNCTION: CARMA2_HW 0x0053c580
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_copyFromDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {
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
}

// FUNCTION: CARMA2_HW 0x0053c600
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_fillDirty(br_device_pixelmap* self, br_uint_32 colour, br_rectangle* dirty, br_int_32 num_rects) {
    int i;
    br_error e;

    for (i = 0; i < num_rects; i++) {
        e = self->dispatch->_rectangleFill(self, &dirty[i], colour);
        if (e != 0) {
            return e;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c640
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_doubleBufferDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {
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
}

// FUNCTION: CARMA2_HW 0x0053c6c0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_rectangle(br_device_pixelmap* self, br_rectangle* rect, br_uint_32 colour) {
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
}

// FUNCTION: CARMA2_HW 0x0053c780
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_rectangle2(br_device_pixelmap* self, br_rectangle* rect, br_uint_32 colour_tl, br_uint_32 colour_br) {
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
}

// FUNCTION: CARMA2_HW 0x0053c840
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_line(br_device_pixelmap* self, br_point* s, br_point* e, br_uint_32 colour) {
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
}

// FUNCTION: CARMA2_HW 0x0053d660
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_text(br_device_pixelmap* self, br_point* point, br_font* font, const char* text, br_uint_32 colour) {
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
}

// FUNCTION: CARMA2_HW 0x0053d810
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_textBounds(br_device_pixelmap* self, br_rectangle* rect, br_font* font, const char* text) {
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
}

// FUNCTION: CARMA2_HW 0x0053d8a0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_copyBits(br_device_pixelmap* self, br_point* point, br_uint_8* src, br_uint_16 s_stride, br_rectangle* bit_rect, br_uint_32 colour) {
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
}

// FUNCTION: CARMA2_HW 0x0053d970
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_flush(br_device_pixelmap* self) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053d980
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_synchronise(br_device_pixelmap* self, br_token sync_type, br_boolean block) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053d990
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_directLock(br_device_pixelmap* self, br_boolean block) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053d9a0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_directUnlock(br_device_pixelmap* self) {

    return 0;
}
