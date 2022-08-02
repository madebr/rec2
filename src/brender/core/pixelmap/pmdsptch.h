#ifndef REC2_PMDSPTCH_H
#define REC2_PMDSPTCH_H

#include "c2_hooks.h"

#include "brender/br_types.h"

br_pixelmap* C2_HOOK_CDECL BrPixelmapAllocateSub(br_pixelmap* src, br_int_32 x, br_int_32 y, br_int_32 w, br_int_32 h);

void C2_HOOK_CDECL BrPixelmapFree(br_pixelmap* src);

br_pixelmap* C2_HOOK_CDECL BrPixelmapResize(br_pixelmap* src, br_int_32 width, br_int_32 height);

br_pixelmap* C2_HOOK_CDECL BrPixelmapMatch(br_pixelmap* src, br_uint_8 match_type);

br_pixelmap* C2_HOOK_CDECL BrPixelmapMatchSized(br_pixelmap* src, br_uint_8 match_type, br_int_32 width, br_int_32 height);

br_pixelmap* C2_HOOK_CDECL BrPixelmapMatchTyped(br_pixelmap* src, br_uint_8 match_type, br_uint_8 pixelmap_type);

br_pixelmap* C2_HOOK_CDECL BrPixelmapMatchTypedSized(br_pixelmap* src, br_uint_8 match_type, br_uint_8 pixelmap_type, br_int_32 width, br_int_32 height);

br_pixelmap* C2_HOOK_CDECL BrPixelmapClone(br_pixelmap* src);

void C2_HOOK_CDECL BrPixelmapFill(br_pixelmap* dst, br_uint_32 colour);

void C2_HOOK_CDECL BrPixelmapRectangle(br_pixelmap* dst, br_int_32 x, br_int_32 y, br_int_32 w, br_int_32 h, br_uint_32 colour);

void C2_HOOK_CDECL BrPixelmapRectangle2(br_pixelmap* dst, br_int_32 x, br_int_32 y, br_int_32 w, br_int_32 h, br_uint_32 colour_tl, br_uint_32 colour_br);

br_error C2_HOOK_STDCALL DispatchCopy(br_device_pixelmap* self, br_device_pixelmap* src);

br_error C2_HOOK_STDCALL DispatchRectangleCopy(br_device_pixelmap* self, br_point* p, br_device_pixelmap* src, br_rectangle* r);

br_error C2_HOOK_STDCALL DispatchRectangleStretchCopy(br_device_pixelmap* self, br_rectangle* r, br_device_pixelmap* src, br_rectangle* s);

void C2_HOOK_CDECL BrPixelmapRectangleCopy(br_pixelmap* dst, br_int_32 dx, br_int_32 dy, br_pixelmap* src, br_int_32 sx, br_int_32 sy, br_int_32 w, br_int_32 h);

void C2_HOOK_CDECL BrPixelmapRectangleStretchCopy(br_pixelmap* dst, br_int_32 dx, br_int_32 dy, br_int_32 dw, br_int_32 dh, br_pixelmap* src, br_int_32 sx, br_int_32 sy, br_int_32 sw, br_int_32 sh);

void C2_HOOK_CDECL BrPixelmapRectangleFill(br_pixelmap* dst, br_int_32 x, br_int_32 y, br_int_32 w, br_int_32 h, br_uint_32 colour);

void C2_HOOK_CDECL BrPixelmapDirtyRectangleCopy(br_pixelmap* dst, br_pixelmap* src, br_int_32 x, br_int_32 y, br_int_32 w, br_int_32 h);

void C2_HOOK_CDECL BrPixelmapDirtyRectangleClear(br_pixelmap* dst, br_int_32 x, br_int_32 y, br_int_32 w, br_int_32 h, br_uint_32 colour);

void C2_HOOK_CDECL BrPixelmapDirtyRectangleDoubleBuffer(br_pixelmap* dst, br_pixelmap* src, br_int_32 x, br_int_32 y, br_int_32 w, br_int_32 h);

void C2_HOOK_CDECL BrPixelmapPixelSet(br_pixelmap* dst, br_int_32 x, br_int_32 y, br_uint_32 colour);

br_uint_32 C2_HOOK_CDECL BrPixelmapPixelGet(br_pixelmap* dst, br_int_32 x, br_int_32 y);

void C2_HOOK_CDECL BrPixelmapCopy(br_pixelmap* dst, br_pixelmap* src);

void C2_HOOK_CDECL BrPixelmapLine(br_pixelmap* dst, br_int_32 x1, br_int_32 y1, br_int_32 x2, br_int_32 y2, br_uint_32 colour);

void C2_HOOK_CDECL BrPixelmapDoubleBuffer(br_pixelmap* dst, br_pixelmap* src);

void C2_HOOK_CDECL BrPixelmapText(br_pixelmap* dst, br_int_32 x, br_int_32 y, br_uint_32 colour, br_font* font, char* text);

void C2_HOOK_CDECL BrPixelmapTextF(br_pixelmap* dst, br_int_32 x, br_int_32 y, br_uint_32 colour, br_font* font, char* fmt, ...);

br_uint_16 C2_HOOK_CDECL BrPixelmapTextWidth(br_pixelmap* dst, br_font* font, char* text);

br_uint_16 C2_HOOK_CDECL BrPixelmapTextHeight(br_pixelmap* dst, br_font* font);

void C2_HOOK_CDECL BrPixelmapCopyBits(br_pixelmap* dst, br_int_32 x, br_int_32 y, br_uint_8* src, br_int_32 s_stride, br_int_32 start_bit, br_int_32 end_bit, br_int_32 nrows, br_uint_32 colour);

br_uint_16 C2_HOOK_CDECL BrPixelmapFileSize(br_pixelmap* pm);

br_uint_16 C2_HOOK_CDECL BrPixelmapPixelSize(br_pixelmap* pm);

br_uint_16 C2_HOOK_CDECL BrPixelmapChannels(br_pixelmap* pm);

void C2_HOOK_CDECL BrPixelmapPaletteSet(br_pixelmap* pm, br_pixelmap* pal);

void C2_HOOK_CDECL BrPixelmapPaletteEntrySet(br_pixelmap* pm, br_int_32 index, br_colour colour);

void C2_HOOK_CDECL BrPixelmapPaletteEntrySetMany(br_pixelmap* pm, br_int_32 index, br_int_32 ncolours, br_colour* colours);

br_pixelmap* C2_HOOK_CDECL BrPixelmapDirectLock(br_pixelmap* src, br_boolean block);

br_pixelmap* C2_HOOK_CDECL BrPixelmapDirectUnlock(br_pixelmap* src);

#endif // REC2_PMDSPTCH_H
