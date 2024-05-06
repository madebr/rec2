#include "flicplay.h"

#include "displays.h"
#include "errors.h"
#include "globvars.h"
#include "graphics.h"
#include "input.h"
#include "loading.h"
#include "main.h"
#include "sound.h"
#include "utility.h"

#include "platform.h"

#include "brender/brender.h"

#include "c2_string.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tFlic_descriptor, gPanel_flic, 2, 0x00686218);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gPanel_flic_left, 2, 0x006861f0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gPanel_flic_top, 2, 0x00686200);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_pixelmap*, gPanel_buffer, 2, 0x00686320);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gPalette, 0x00686208);
C2_HOOK_VARIABLE_IMPLEMENT(void*, gPalette_pixels, 0x006861d8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPlay_from_disk, 0x00686318);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gLast_flic_name, 14, 0x00686330);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPalette_fuck_prevention, 0x00686308);
C2_HOOK_VARIABLE_IMPLEMENT(int, gTransparency_on, 0x00686310);
C2_HOOK_VARIABLE_IMPLEMENT(int, gTranslation_count, 0x006861dc);
C2_HOOK_VARIABLE_IMPLEMENT(tTranslation_record*, gTranslations, 0x00686210);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tDR_font*, gTrans_fonts, 2, 0x006861e0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(tFlic_spec, gMain_flic_list, 372, 0x005964d0, MISSING);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gSound_time, 0x00686328);
C2_HOOK_VARIABLE_IMPLEMENT(int, gSound_ID, 0x00686300);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDark_mode, 0x0068630c);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(tFlic_bunch, gFlic_bunch, 9, 0x00599c10, FIXME);
C2_HOOK_VARIABLE_IMPLEMENT(tFlic_descriptor*, gFirst_flic, 0x006861e8);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tU32, gLast_panel_frame_time, 2, 0x006862f8);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tU32, gPanel_flic_data_length, 2, 0x006861f8);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tU8*, gPanel_flic_data, 2, 0x006861d0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPanel_flic_disable, 0x00686314);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPending_pending_flic, 0x0068620c);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gPending_flic, 0x005964c8, -1);

// Use this function to avoid unaligned memory access.
// Added by DethRace
static void mem_write_u16(void* memory, tU16 u16) {

    c2_memcpy(memory, &u16, sizeof(tU16));
}

int C2_HOOK_FASTCALL GetPanelFlicFrameIndex(int pIndex) {

    return C2V(gPanel_flic)[pIndex].current_frame;
}
C2_HOOK_FUNCTION(0x00461a40, GetPanelFlicFrameIndex)

void C2_HOOK_FASTCALL DontLetFlicFuckWithPalettes(void) {

    C2V(gPalette_fuck_prevention) = 1;
}

void C2_HOOK_FASTCALL LetFlicFuckWithPalettes(void) {

    C2V(gPalette_fuck_prevention) = 0;
}

void C2_HOOK_FASTCALL TurnFlicTransparencyOff(void) {

    C2V(gTransparency_on) = 0;
}

void C2_HOOK_FASTCALL TurnFlicTransparencyOn(void) {

    C2V(gTransparency_on) = 1;
}

void C2_HOOK_FASTCALL FlicPaletteAllocate(void) {

    C2V(gPalette_pixels) = BrMemAllocate(0x400, kMem_misc);
    C2V(gPalette) = DRPixelmapAllocate(BR_PMT_RGBX_888, 1, 256, C2V(gPalette_pixels), 0);
}
C2_HOOK_FUNCTION(0x00461a60, FlicPaletteAllocate)

void C2_HOOK_FASTCALL AssertFlicPixelmap(tFlic_descriptor_ptr pFlic_info, br_pixelmap* pDest_pixelmap) {

    if (pDest_pixelmap != NULL) {
        pFlic_info->first_pixel = (tU8*)pDest_pixelmap->pixels
                                  + pFlic_info->x_offset
                                  + pFlic_info->y_offset * pDest_pixelmap->row_bytes;
    }
    pFlic_info->the_pixelmap = pDest_pixelmap;
}
C2_HOOK_FUNCTION(0x00461aa0, AssertFlicPixelmap)

int C2_HOOK_FASTCALL StartFlic(char* pFile_name, int pIndex, tFlic_descriptor_ptr pFlic_info, tU32 pSize, tS8* pData_ptr, br_pixelmap* pDest_pixelmap, int pX_offset, int pY_offset, int pFrame_rate) {
    tU16 claimed_speed;
    tU16 magic_number;
    tPath_name the_path;
    int total_size;

    if (C2V(gPlay_from_disk)) {
        PathCat(the_path, C2V(gApplication_path), "ANIM");
        PathCat(the_path, the_path, pFile_name);
        pFlic_info->f = DRfopen(the_path, "rb");

        if (pFlic_info->f == NULL) {
            FatalError(kFatalError_CouldNotOpenFlicFile_S, pFile_name);
        }
        total_size = GetFileLength(pFlic_info->f);
        if (total_size >= 75000) {
            pFlic_info->bytes_in_buffer = 75000;
        } else {
            pFlic_info->bytes_in_buffer = total_size;
        }
        if (pFlic_info->data_start == NULL) {
            pFlic_info->data_start = BrMemAllocate(pFlic_info->bytes_in_buffer, kMem_misc);
        }

        pFlic_info->data = pFlic_info->data_start;
        c2_strcpy(C2V(gLast_flic_name), pFile_name);
        fread(pFlic_info->data_start, 1, pFlic_info->bytes_in_buffer, pFlic_info->f);
        pFlic_info->bytes_still_to_be_read = total_size - pFlic_info->bytes_in_buffer;
    } else {
        pFlic_info->f = NULL;
        pFlic_info->data = (char*)pData_ptr;
    }
    pFlic_info->bytes_remaining = MemReadU32(&pFlic_info->data);
    magic_number = MemReadU16(&pFlic_info->data);
    if (magic_number == 0xaf11) {
        pFlic_info->new_format = 0;
    } else if (magic_number == 0xaf12) {
        pFlic_info->new_format = 1;
    } else {
        return -1;
    }

    pFlic_info->frames_left = MemReadU16(&pFlic_info->data);
    pFlic_info->current_frame = 0;
    pFlic_info->width = MemReadU16(&pFlic_info->data);
    pFlic_info->height = MemReadU16(&pFlic_info->data);
    if (MemReadU16(&pFlic_info->data) != 8) {
        FatalError(kFatalError_FlicFileWasNot8BitsDeep_S, C2V(gLast_flic_name));
    }
    MemSkipBytes(&pFlic_info->data, 2);
    claimed_speed = MemReadU16(&pFlic_info->data);
    MemSkipBytes(&pFlic_info->data, 0x6e);
    pFlic_info->the_pixelmap = pDest_pixelmap;

    if (pX_offset != -1) {
        pFlic_info->x_offset = pX_offset;
    } else if (pDest_pixelmap != NULL) {
        pFlic_info->x_offset = (pDest_pixelmap->width - pFlic_info->width) / 2;
    } else {
        pFlic_info->x_offset = 0;
    }
    if (pY_offset != -1) {
        pFlic_info->y_offset = pY_offset;
    } else if (pDest_pixelmap != NULL) {
        pFlic_info->y_offset = (pDest_pixelmap->height - pFlic_info->height) / 2;
    } else {
        pFlic_info->y_offset = 0;
    }

    if (pFrame_rate != 0) {
        pFlic_info->frame_period = 1000 / pFrame_rate;
    } else {
        if (claimed_speed == 0) {
            FatalError(kFatalError_CouldNotAscertainFrameRateForFlicFile, C2V(gLast_flic_name));
        }
        if (pFlic_info->new_format != 0) {
            pFlic_info->frame_period = claimed_speed;
        } else {
            pFlic_info->frame_period = 14 * claimed_speed;
        }
    }
    pFlic_info->the_index = pIndex;
    AssertFlicPixelmap(pFlic_info, pDest_pixelmap);
    return 0;
}
C2_HOOK_FUNCTION(0x00461ad0, StartFlic)

int C2_HOOK_FASTCALL EndFlic(tFlic_descriptor_ptr pFlic_info) {

    if (pFlic_info->f != NULL) {
        BrMemFree(pFlic_info->data_start);
        pFlic_info->data_start = NULL;
        DRfclose(pFlic_info->f);
        pFlic_info->f = NULL;
    }
    if (pFlic_info->data != NULL) {
        pFlic_info->data = NULL;
    }
    return 0;
}
C2_HOOK_FUNCTION(0x00461d60, EndFlic)

void C2_HOOK_FASTCALL DoColour256(tFlic_descriptor* pFlic_info, tU32 chunk_length) {
    int i;
    int j;
    int packet_count;
    int skip_count;
    int change_count;
    int current_colour;
    tU8* palette_pixels;
    tU8 red;
    tU8 green;
    tU8 blue;

    current_colour = 0;
    palette_pixels = C2V(gPalette_pixels);

    packet_count = MemReadU16(&pFlic_info->data);
    for (i = 0; i < packet_count; i++) {
        skip_count = MemReadU8(&pFlic_info->data);
        change_count = MemReadU8(&pFlic_info->data);
        if (change_count == 0) {
            change_count = 256;
        }
        palette_pixels += skip_count * sizeof(br_int_32);
        current_colour += skip_count;
        for (j = 0; j < change_count; j++) {
            red = MemReadU8(&pFlic_info->data);
            blue = MemReadU8(&pFlic_info->data);
            green = MemReadU8(&pFlic_info->data);
            // argb
            palette_pixels[0] = green;
            palette_pixels[1] = blue;
            palette_pixels[2] = red;
            palette_pixels[3] = 0;
            palette_pixels += 4;
            // LOG_DEBUG("color %d", current_colour);
        }
        if (!C2V(gPalette_fuck_prevention)) {
            DRSetPaletteEntries(C2V(gPalette), current_colour, change_count);
        }
    }
}

void C2_HOOK_FASTCALL DoDeltaTrans(tFlic_descriptor* pFlic_info, tU32 chunk_length) {
    int i;
    int j;
    int k;
    int line_count;
    int number_of_packets;
    int skip_count;
    int size_count;
    tU8* pixel_ptr;
    tU8 the_byte;
    tU8 the_byte2;
    tU32 the_row_bytes;
    tU16* line_pixel_ptr;
    tU16 the_word;

    line_count = MemReadU16(&pFlic_info->data);
    the_row_bytes = pFlic_info->the_pixelmap->row_bytes;
    pixel_ptr = pFlic_info->first_pixel;

    for (i = 0; i < line_count;) {
        number_of_packets = MemReadS16(&pFlic_info->data);
        line_pixel_ptr = (tU16*)pixel_ptr;

        if (number_of_packets < 0) {
            pixel_ptr = pixel_ptr + the_row_bytes * -number_of_packets;
        } else {
            for (j = 0; j < number_of_packets; j++) {
                skip_count = MemReadU8(&pFlic_info->data);
                size_count = MemReadS8(&pFlic_info->data);
                line_pixel_ptr += skip_count / 2;
                if (size_count < 0) {
                    the_byte = *pFlic_info->data++;
                    the_byte2 = *pFlic_info->data++;

                    if (the_byte && the_byte2) {
                        the_word = *((tU16*)pFlic_info->data - 1);
                        for (k = 0; k < -size_count; k++) {
                            mem_write_u16(line_pixel_ptr, the_word);
                            line_pixel_ptr++;
                        }
                    } else {
                        for (k = 0; k < -size_count; k++) {
                            if (the_byte) {
                                *(tU8*)line_pixel_ptr = the_byte;
                            }
                            line_pixel_ptr = (tU16*)((tU8*)line_pixel_ptr + 1);
                            if (the_byte2) {
                                *(tU8*)line_pixel_ptr = the_byte2;
                            }
                            line_pixel_ptr = (tU16*)((tU8*)line_pixel_ptr + 1);
                        }
                    }
                } else {
                    for (k = 0; k < size_count; k++) {
                        the_word = *(tU16*)pFlic_info->data;
                        pFlic_info->data += 2;
                        if (the_word) {
                            mem_write_u16(line_pixel_ptr, the_word);
                        }
                        line_pixel_ptr++;
                    }
                }
            }
            pixel_ptr = pixel_ptr + the_row_bytes;
            i++;
        }
    }
}

void C2_HOOK_FASTCALL DoDeltaX(tFlic_descriptor* pFlic_info, tU32 chunk_length) {
    int i;
    int j;
    int k;
    int line_count;
    int number_of_packets;
    int skip_count;
    int size_count;
    tU8* pixel_ptr;
    tU32 the_row_bytes;
    tU16* line_pixel_ptr;
    tU16 the_word;

    line_count = MemReadU16(&pFlic_info->data);
    the_row_bytes = pFlic_info->the_pixelmap->row_bytes;
    pixel_ptr = pFlic_info->first_pixel;

    for (i = 0; i < line_count;) {
        number_of_packets = MemReadS16(&pFlic_info->data);
        line_pixel_ptr = (tU16*)pixel_ptr;

        if (number_of_packets < 0) {
            pixel_ptr = pixel_ptr + the_row_bytes * -number_of_packets;
        } else {
            for (j = 0; j < number_of_packets; j++) {
                skip_count = MemReadU8(&pFlic_info->data);
                size_count = MemReadS8(&pFlic_info->data);
                line_pixel_ptr += skip_count / 2;
                if (size_count < 0) {
                    the_word = *(tU16*)pFlic_info->data;
                    pFlic_info->data += 2;
                    for (k = 0; k < -size_count; k++) {
                        *line_pixel_ptr = the_word;
                        line_pixel_ptr++;
                    }
                } else {
                    for (k = 0; k < size_count; k++) {
                        the_word = *(tU16*)pFlic_info->data;
                        pFlic_info->data += 2;
                        *line_pixel_ptr = the_word;
                        line_pixel_ptr++;
                    }
                }
            }
            pixel_ptr = pixel_ptr + the_row_bytes;
            i++;
        }
    }
}

void C2_HOOK_FASTCALL DoColourMap(tFlic_descriptor_ptr pFlic_info, tU32 chunk_length) {
    int i;
    int j;
    int packet_count;
    int skip_count;
    int change_count;
    int current_colour = 0;
    tU8* palette_pixels;
    tU8 red;
    tU8 green;
    tU8 blue;

    palette_pixels = C2V(gPalette_pixels);

    packet_count = MemReadU16(&pFlic_info->data);
    for (i = 0; i < packet_count; i++) {
        skip_count = MemReadU8(&pFlic_info->data);
        change_count = MemReadU8(&pFlic_info->data);
        if (change_count == 0) {
            change_count = 256;
        }
        palette_pixels += skip_count * sizeof(br_int_32);
        current_colour += skip_count;
        for (j = 0; j < change_count; j++) {
            red = MemReadU8(&pFlic_info->data);
            blue = MemReadU8(&pFlic_info->data);
            green = MemReadU8(&pFlic_info->data);
            // argb
            palette_pixels[0] = green * 4;
            palette_pixels[1] = blue * 4;
            palette_pixels[2] = red * 4;
            palette_pixels[3] = 0;
            palette_pixels += 4;
        }
        if (!C2V(gPalette_fuck_prevention)) {
            DRSetPaletteEntries(C2V(gPalette), current_colour, change_count);
        }
    }
}

void (C2_HOOK_FASTCALL * DoDifferenceX_original)(tFlic_descriptor* pFlic_info, tU32 chunk_length);
void C2_HOOK_FASTCALL DoDifferenceX(tFlic_descriptor* pFlic_info, tU32 chunk_length) {

#if defined(C2_HOOKS_ENABLED)
    DoDifferenceX_original(pFlic_info, chunk_length);
#else
    int i;
    int j;
    int k;
    int first_line;
    int line_count;
    int number_of_packets;
    int skip_count;
    int size_count;
    tU8* pixel_ptr;
    tU8* line_pixel_ptr;
    tU8 the_byte;
    tU32 the_row_bytes;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00462390, DoDifferenceX, DoDifferenceX_original)

void (C2_HOOK_FASTCALL * DoDifferenceTrans_original)(tFlic_descriptor* pFlic_info, tU32 chunk_length);
void C2_HOOK_FASTCALL DoDifferenceTrans(tFlic_descriptor* pFlic_info, tU32 chunk_length) {

#if defined(C2_HOOKS_ENABLED)
    DoDifferenceTrans_original(pFlic_info, chunk_length);
#else
    int i;
    int j;
    int k;
    int first_line;
    int line_count;
    int number_of_packets;
    int skip_count;
    int size_count;
    tU8* pixel_ptr;
    tU8* line_pixel_ptr;
    tU8 the_byte;
    tU32 the_row_bytes;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00462480, DoDifferenceTrans, DoDifferenceTrans_original)

void (C2_HOOK_FASTCALL * DoBlack_original)(tFlic_descriptor* pFlic_info, tU32 chunk_length);
void C2_HOOK_FASTCALL DoBlack(tFlic_descriptor* pFlic_info, tU32 chunk_length) {

#if defined(C2_HOOKS_ENABLED)
    DoBlack_original(pFlic_info, chunk_length);
#else
    int i;
    int j;
    int the_width;
    tU8* pixel_ptr;
    tU32 the_row_bytes;
    tU32* line_pixel_ptr;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00462580, DoBlack, DoBlack_original)

void (C2_HOOK_FASTCALL * DoRunLengthX_original)(tFlic_descriptor* pFlic_info, tU32 chunk_length);
void C2_HOOK_FASTCALL DoRunLengthX(tFlic_descriptor* pFlic_info, tU32 chunk_length) {

#if 0 //defined(C2_HOOKS_ENABLED)
    DoRunLengthX(pFlic_info, chunk_length);
#else
    int i;
    int j;
    int k;
    int number_of_packets;
    int size_count;
    tU8* pixel_ptr;
    tU8* line_pixel_ptr;
    tU8 the_byte;
    tU32 the_row_bytes;

    the_row_bytes = pFlic_info->the_pixelmap->row_bytes;
    pixel_ptr = pFlic_info->first_pixel;

    for (i = 0; i < pFlic_info->height; i++) {
        line_pixel_ptr = pixel_ptr;
        number_of_packets = MemReadU8(&pFlic_info->data);
        for (j = 0; j < number_of_packets; j++) {
            size_count = MemReadS8(&pFlic_info->data);
            if (size_count >= 0) {
                the_byte = MemReadU8(&pFlic_info->data);
                for (k = 0; k < size_count; k++) {
                    *line_pixel_ptr = the_byte;
                    line_pixel_ptr++;
                }
            } else {
                for (k = 0; k < -size_count; k++) {
                    the_byte = MemReadU8(&pFlic_info->data);
                    *line_pixel_ptr = the_byte;
                    line_pixel_ptr++;
                }
            }
        }
        pixel_ptr += the_row_bytes;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004625d0, DoRunLengthX, DoRunLengthX_original)

void (C2_HOOK_FASTCALL * DoRunLengthTrans_original)(tFlic_descriptor* pFlic_info, tU32 chunk_length);
void C2_HOOK_FASTCALL DoRunLengthTrans(tFlic_descriptor* pFlic_info, tU32 chunk_length) {

#if 0 //defined(C2_HOOKS_ENABLED)
    DoRunLengthTrans_original(pFlic_info, chunk_length);
#else
    int i;
    int j;
    int k;
    int number_of_packets;
    int size_count;
    tU8* pixel_ptr;
    tU8* line_pixel_ptr;
    tU8 the_byte;
    tU32 the_row_bytes;

    the_row_bytes = pFlic_info->the_pixelmap->row_bytes;
    pixel_ptr = pFlic_info->first_pixel;

    for (i = 0; i < pFlic_info->height; i++) {
        line_pixel_ptr = pixel_ptr;
        number_of_packets = MemReadU8(&pFlic_info->data);
        for (j = 0; j < number_of_packets; j++) {
            size_count = MemReadS8(&pFlic_info->data);
            if (size_count >= 0) {
                the_byte = MemReadU8(&pFlic_info->data);

                for (k = 0; k < size_count; k++) {
                    if (the_byte) {
                        *line_pixel_ptr = the_byte;
                    }
                    line_pixel_ptr++;
                }
            } else {
                for (k = 0; k < -size_count; k++) {
                    the_byte = MemReadU8(&pFlic_info->data);
                    if (the_byte) {
                        *line_pixel_ptr = the_byte;
                    }
                    line_pixel_ptr++;
                }
            }
        }
        pixel_ptr += the_row_bytes;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004626a0, DoRunLengthTrans, DoRunLengthTrans_original)


void (C2_HOOK_FASTCALL * DoUncompressed_original)(tFlic_descriptor* pFlic_info, tU32 chunk_length);
void C2_HOOK_FASTCALL DoUncompressed(tFlic_descriptor* pFlic_info, tU32 chunk_length) {

#if defined(C2_HOOKS_ENABLED)
    DoUncompressed_original(pFlic_info, chunk_length);
#else
    int i;
    int j;
    int the_width;
    tU8* pixel_ptr;
    tU32 the_row_bytes;
    tU32* line_pixel_ptr;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00462780, DoUncompressed, DoUncompressed_original)

void (C2_HOOK_FASTCALL * DoUncompressedTrans_original)(tFlic_descriptor* pFlic_info, tU32 chunk_length);
void C2_HOOK_FASTCALL DoUncompressedTrans(tFlic_descriptor* pFlic_info, tU32 chunk_length) {

#if defined(C2_HOOKS_ENABLED)
    DoUncompressedTrans_original(pFlic_info, chunk_length);
#else
    int i;
    int j;
    int the_width;
    tU8* pixel_ptr;
    tU8* line_pixel_ptr;
    tU8 the_byte;
    tU32 the_row_bytes;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004627f0, DoUncompressedTrans, DoUncompressedTrans_original)

void (C2_HOOK_FASTCALL * DrawTranslations_original)(tFlic_descriptor* pFlic_info);
void C2_HOOK_FASTCALL DrawTranslations(tFlic_descriptor* pFlic_info) {

#if 0 //defined(C2_HOOKS_ENABLED)
    DrawTranslations_orginal(pFlic_info, pLast_frame);
#else
    tTranslation_record* trans;
    int i;
    int x;
    int width;
    int right_edge;

    x = 0;
    right_edge = 0;
    for (i = 0; i < C2V(gTranslation_count); i++) {
        trans = &C2V(gTranslations)[i];
        if (trans->flic_index == pFlic_info->the_index) {
            width = DRTextWidth(C2V(gTrans_fonts)[trans->font_index], trans->text);
            switch (trans->justification) {
                case eJust_left:
                    x = pFlic_info->x_offset + trans->x;
                    right_edge = x + width;
                    break;
                case eJust_right:
                    x = pFlic_info->x_offset - width + trans->x;
                    right_edge = x;
                    break;
                case eJust_centre:
                    x = pFlic_info->x_offset + trans->x - width / 2;
                    right_edge = x + width / 2;
                    break;
            }
            TransDRPixelmapText(
                    pFlic_info->the_pixelmap,
                    x,
                    pFlic_info->y_offset + trans->y,
                    C2V(gTrans_fonts)[trans->font_index],
                    trans->text,
                    right_edge);
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00462860, DrawTranslations, DrawTranslations_original)

int C2_HOOK_FASTCALL PlayNextFlicFrame2(tFlic_descriptor* pFlic_info, int pPanel_flic) {
    tU32 frame_length;
    tU32 chunk_length;
    int chunk_count;
    int chunk_counter;
    int chunk_type;
    int magic_bytes;
    int data_knocked_off;
    int read_amount;

    PossibleService();
    frame_length = MemReadU32(&pFlic_info->data);
    magic_bytes = MemReadU16(&pFlic_info->data);
    chunk_count = MemReadU16(&pFlic_info->data);

    MemSkipBytes(&pFlic_info->data, 8);
    if (magic_bytes == 0xf1fa) {
        for (chunk_counter = 0; chunk_counter < chunk_count; chunk_counter++) {
            chunk_length = MemReadU32(&pFlic_info->data);
            chunk_type = MemReadU16(&pFlic_info->data);
            switch (chunk_type) {
                case 4:
                    DoColour256(pFlic_info, chunk_length);
                    break;
                case 7:
                    if (C2V(gTransparency_on)) {
                        DoDeltaTrans(pFlic_info, chunk_length);
                    } else {
                        DoDeltaX(pFlic_info, chunk_length);
                    }
                    break;
                case 11:
                    DoColourMap(pFlic_info, chunk_length);
                    break;
                case 12:
                    if (C2V(gTransparency_on)) {
                        DoDifferenceTrans(pFlic_info, chunk_length);
                    } else {
                        DoDifferenceX(pFlic_info, chunk_length);
                    }
                    break;
                case 13:
                    DoBlack(pFlic_info, chunk_length);
                    break;
                case 15:
                    if (C2V(gTransparency_on)) {
                        DoRunLengthTrans(pFlic_info, chunk_length);
                    } else {
                        DoRunLengthX(pFlic_info, chunk_length);
                    }
                    break;
                case 16:
                    if (C2V(gTransparency_on)) {
                        DoUncompressedTrans(pFlic_info, chunk_length);
                    } else {
                        DoUncompressed(pFlic_info, chunk_length);
                    }
                    break;
                default:
                    // LOG_WARN("unrecognized chunk type");
                    MemSkipBytes(&pFlic_info->data, chunk_length - 6);
                    break;
            }
            // Align on even byte
            pFlic_info->data = (char*)((uintptr_t)(pFlic_info->data + 1) & (~(uintptr_t)1));
        }
    } else {
        // LOG_WARN("not frame header");
        MemSkipBytes(&pFlic_info->data, frame_length - 16);
        pFlic_info->frames_left++;
        pFlic_info->current_frame--;
    }
    pFlic_info->current_frame++;
    pFlic_info->frames_left--;
    if (pFlic_info->frames_left != 0 && C2V(gTranslation_count) != 0) {
        DrawTranslations(pFlic_info);
    }
    if (pFlic_info->f != NULL && pFlic_info->bytes_still_to_be_read) {
        data_knocked_off = pFlic_info->data - pFlic_info->data_start;
        c2_memmove(pFlic_info->data_start, pFlic_info->data, pFlic_info->bytes_in_buffer - data_knocked_off);
        pFlic_info->data = pFlic_info->data_start;
        pFlic_info->bytes_in_buffer -= data_knocked_off;

        if (pFlic_info->bytes_still_to_be_read > data_knocked_off) {
            read_amount = data_knocked_off;
        } else {
            read_amount = pFlic_info->bytes_still_to_be_read;
        }
        if (read_amount != 0) {
            DRfread(&pFlic_info->data_start[pFlic_info->bytes_in_buffer], 1, read_amount, pFlic_info->f);
        }
        pFlic_info->bytes_in_buffer += read_amount;
        pFlic_info->bytes_still_to_be_read -= read_amount;
    }
    return pFlic_info->frames_left == 0;
}

int C2_HOOK_FASTCALL PlayNextFlicFrame(tFlic_descriptor* pFlic_info) {

    return PlayNextFlicFrame2(pFlic_info, 0);
}
C2_HOOK_FUNCTION(0x00461da0, PlayNextFlicFrame)

int C2_HOOK_FASTCALL PlayFlic(int pIndex, tU32 pSize, tS8* pData_ptr, br_pixelmap* pDest_pixelmap, int pX_offset, int pY_offset, tPlayFlic_DoPerFrame DoPerFrame, int pInterruptable, int pFrame_rate) {
    int finished_playing;
    tFlic_descriptor the_flic;
    tU32 last_frame;
    tU32 new_time;
    tU32 frame_period;

    finished_playing = 0;
    the_flic.data_start = NULL;
    if (StartFlic(C2V(gMain_flic_list)[pIndex].file_name, pIndex, &the_flic, pSize, pData_ptr, pDest_pixelmap, pX_offset, pY_offset, pFrame_rate)) {
        // LOG_WARN("startflic returned error");
        return -1;
    }

    last_frame = 0;
    while ((!pInterruptable || !AnyKeyDown()) && !finished_playing) {
        new_time = PDGetTotalTime();
        frame_period = new_time - last_frame;

        if (C2V(gSound_time) != 0 && new_time >= C2V(gSound_time)) {
            DRS3StartSound(C2V(gEffects_outlet), C2V(gSound_ID));
            C2V(gSound_time) = finished_playing;
        }
        if (frame_period >= the_flic.frame_period) {
            finished_playing = PlayNextFlicFrame(&the_flic);
            DoPerFrame();
            if (!C2V(gDark_mode)) {
                EnsurePaletteUp();
            }
            ServiceGame();
            last_frame = new_time;
        }
    }
    ServiceGame();
    EndFlic(&the_flic);
    return 0;
}
C2_HOOK_FUNCTION(0x00462930, PlayFlic)

void C2_HOOK_FASTCALL SwapScreen(void) {

    PDScreenBufferSwap(0);
}
C2_HOOK_FUNCTION(0x00462c10, SwapScreen)

void C2_HOOK_FASTCALL InitFlics(void) {
    int i;

    for (i = 0; i < REC2_ASIZE(C2V(gMain_flic_list)); i++) {
        C2V(gMain_flic_list)[i].data_ptr = NULL;
    }
}
C2_HOOK_FUNCTION(0x00462a40, InitFlics)

int C2_HOOK_FASTCALL LoadFlic(int pIndex) {

    return 0;
}
C2_HOOK_FUNCTION(0x00462a60, LoadFlic)

void C2_HOOK_FASTCALL UnlockFlic(int pIndex) {

    if (pIndex >= 0 && C2V(gMain_flic_list)[pIndex].data_ptr != NULL) {
        MAMSUnlock((void **) &C2V(gMain_flic_list)[pIndex].data_ptr);
    }
}
C2_HOOK_FUNCTION(0x00462a70, UnlockFlic)

int C2_HOOK_FASTCALL LoadFlicData(char* pName, tU8** pData, tU32* pData_length) {
    FILE* f;
    tPath_name the_path;

    if (*pData != NULL) {
        MAMSLock((void**)pData);
        return 1;
    }
    if (C2V(gPlay_from_disk)) {
        return 1;
    }
    PossibleService();
    PathCat(the_path, C2V(gApplication_path), "ANIM");
    PathCat(the_path, the_path, pName);
    f = DRfopen(the_path, "rb");
    if (f == NULL) {
        return 0;
    }
    *pData_length = GetFileLength(f);
    *pData = BrMemAllocate(*pData_length, kMem_misc);
    if (*pData == NULL) {
        DRfclose(f);
        return 0;
    }
    DRfread(*pData, 1, *pData_length, f);
    DRfclose(f);
    return 1;
}
C2_HOOK_FUNCTION(0x00462a90, LoadFlicData)

void C2_HOOK_FASTCALL FreeFlic(int pIndex) {

    PossibleService();
    if (C2V(gMain_flic_list)[pIndex].data_ptr != NULL) {
        BrMemFree(C2V(gMain_flic_list)[pIndex].data_ptr);
        C2V(gMain_flic_list)[pIndex].data_ptr = NULL;
    }
}
C2_HOOK_FUNCTION(0x00462b80, FreeFlic)

void C2_HOOK_FASTCALL RunFlicAt(int pIndex, int pX, int pY) {

    LoadFlic(pIndex);
    PlayFlic(
            pIndex,
            C2V(gMain_flic_list)[pIndex].the_size,
            C2V(gMain_flic_list)[pIndex].data_ptr,
            C2V(gBack_screen),
            pX,
            pY,
            SwapScreen,
            0,
            0);
    UnlockFlic(pIndex);
}
C2_HOOK_FUNCTION(0x00462bb0, RunFlicAt)

void C2_HOOK_FASTCALL RunFlic(int pIndex) {

}
C2_HOOK_FUNCTION(0x00462c20, RunFlic)

void C2_HOOK_FASTCALL PreloadBunchOfFlics(int pBunch_index) {

}
C2_HOOK_FUNCTION(0x00462c30, PreloadBunchOfFlics)

void C2_HOOK_FASTCALL UnlockBunchOfFlics(int pBunch_index) {
    int i;

    for (i = 0; i < C2V(gFlic_bunch)[pBunch_index].count; i++) {
        UnlockFlic(C2V(gFlic_bunch)[pBunch_index].indexes[i]);
    }
}
C2_HOOK_FUNCTION(0x00462c40, UnlockBunchOfFlics)

void C2_HOOK_FASTCALL FlushAllFlics(int pBunch_index) {
    int i;

    for (i = 0; i < REC2_ASIZE(C2V(gFlic_bunch)); i++) {
        FreeFlic(i);
    }
}
C2_HOOK_FUNCTION(0x00462c90, FlushAllFlics)

void C2_HOOK_FASTCALL InitFlicQueue(void) {

    C2V(gFirst_flic) = NULL;
}
C2_HOOK_FUNCTION(0x00462cc0, InitFlicQueue)

int C2_HOOK_FASTCALL FlicQueueFinished(void) {
    tFlic_descriptor* the_flic;

    the_flic = C2V(gFirst_flic);
    while (the_flic != NULL) {
        if (the_flic->must_finish) {
            return 0;
        }
        the_flic = the_flic->next;
    }
    return 1;
}
C2_HOOK_FUNCTION(0x00462cd0, FlicQueueFinished)

void C2_HOOK_FASTCALL ProcessFlicQueue(tU32 pInterval) {
    tFlic_descriptor* the_flic;
    tFlic_descriptor* last_flic;
    tFlic_descriptor* doomed_flic;
    tU32 new_time;
    int finished_playing;

    DontLetFlicFuckWithPalettes();
    TurnFlicTransparencyOn();
    the_flic = C2V(gFirst_flic);
    last_flic = NULL;
    new_time = PDGetTotalTime();
    while (the_flic != NULL) {
        if (new_time - the_flic->last_frame < the_flic->frame_period) {
            finished_playing = 0;
        } else {
            the_flic->last_frame = new_time;
            finished_playing = PlayNextFlicFrame(the_flic);
        }
        if (finished_playing) {
            EndFlic(the_flic);
            if (last_flic != NULL) {
                last_flic->next = the_flic->next;
            } else {
                C2V(gFirst_flic) = the_flic->next;
            }
            doomed_flic = the_flic;
            the_flic = the_flic->next;
            BrMemFree(doomed_flic);
        } else {
            last_flic = the_flic;
            the_flic = the_flic->next;
        }
    }
    TurnFlicTransparencyOff();
    LetFlicFuckWithPalettes();
}
C2_HOOK_FUNCTION(0x00462d00, ProcessFlicQueue)

void C2_HOOK_FASTCALL FlushFlicQueue(void) {

    while (!FlicQueueFinished()) {
        RemoveTransientBitmaps(1);
        ProcessFlicQueue(C2V(gFrame_period));
        DoMouseCursor();
        PDScreenBufferSwap(0);
    }
}
C2_HOOK_FUNCTION(0x00462dc0, FlushFlicQueue)

void C2_HOOK_FASTCALL AddToFlicQueue(int pIndex, int pX, int pY, int pMust_finish) {
    tFlic_descriptor* the_flic = NULL;
    tFlic_descriptor* new_flic = NULL;
    tFlic_descriptor* last_flic = NULL;
    tFlic_descriptor* doomed_flic = NULL;

    the_flic = C2V(gFirst_flic);
    while (the_flic != NULL) {
        if (pX == the_flic->x_offset && pY == the_flic->y_offset) {
            doomed_flic = the_flic;
            break;
        }
        last_flic = the_flic;
        the_flic = the_flic->next;
    }

    if (doomed_flic != NULL) {
        EndFlic(doomed_flic);
        if (last_flic != NULL) {
            last_flic->next = doomed_flic->next;
        } else {
            C2V(gFirst_flic) = doomed_flic->next;
        }
        BrMemFree(doomed_flic);
    }

    C2_HOOK_BUG_ON(sizeof(tFlic_descriptor) != 0x70);

    LoadFlic(pIndex);
    new_flic = BrMemAllocate(sizeof(tFlic_descriptor), kMem_misc);
    new_flic->next = NULL;
    the_flic = C2V(gFirst_flic);
    if (C2V(gFirst_flic) != NULL) {
        while (the_flic->next != NULL) {
            the_flic = the_flic->next;
        }
        the_flic->next = new_flic;
    } else {
        C2V(gFirst_flic) = new_flic;
    }
    new_flic->last_frame = 0;
    new_flic->data_start = NULL;
    new_flic->the_index = pIndex;
    new_flic->must_finish = pMust_finish;

    StartFlic(
            C2V(gMain_flic_list)[pIndex].file_name,
            pIndex,
            new_flic,
            C2V(gMain_flic_list)[pIndex].the_size,
            C2V(gMain_flic_list)[pIndex].data_ptr,
            C2V(gBack_screen),
            pX >= 0 ? pX : C2V(gMain_flic_list)[pIndex].x_offset,
            pY >= 0 ? pY : C2V(gMain_flic_list)[pIndex].y_offset,
            20);
}
C2_HOOK_FUNCTION(0x00462f00, AddToFlicQueue)

void C2_HOOK_FASTCALL InitialiseFlicPanel(int pIndex, int pLeft, int pTop, int pWidth, int pHeight) {
    void* the_pixels;

    C2V(gPanel_flic)[pIndex].data = NULL;
    C2V(gPanel_flic_left)[pIndex] = pLeft;
    C2V(gPanel_flic_top)[pIndex] = pTop;
    the_pixels = BrMemAllocate(pHeight * ((pWidth + 3) & ~3), kMem_misc);
    if (C2V(gScreen)->row_bytes < 0) {
        BrFatal(
                "C:\\Carma2\\Source\\Common\\Flicplay.c",
                2082,
                "Bruce bug at line %d, file C:\\Carma2\\Source\\Common\\Flicplay.c",
                2082);
    }
    C2V(gPanel_buffer)[pIndex] = DRPixelmapAllocate(C2V(gScreen)->type, pWidth, pHeight, the_pixels, 0);
}
C2_HOOK_FUNCTION(0x00463020, InitialiseFlicPanel)

void C2_HOOK_FASTCALL DisposeFlicPanel(int pIndex) {

    EndFlic(&C2V(gPanel_flic)[pIndex]);
    BrMemFree(C2V(gPanel_buffer)[pIndex]->pixels);
    BrPixelmapFree(C2V(gPanel_buffer)[pIndex]);
    C2V(gPanel_buffer)[pIndex] = NULL;
}
C2_HOOK_FUNCTION(0x004630b0, DisposeFlicPanel)

void C2_HOOK_FASTCALL ServicePanelFlics(int pCopy_to_buffer) {
    tU32 time_diff;
    tU32 the_time;
    tU32 old_last_time[2];
    int i;
    int j;
    int iteration_count;
    int finished;

    if (C2V(gPanel_flic_disable)) {
        return;
    }
    the_time = PDGetTotalTime();
    DontLetFlicFuckWithPalettes();
    TurnFlicTransparencyOn();

    for (i = 0; i < REC2_ASIZE(C2V(gPanel_flic)); i++) {
        old_last_time[i] = C2V(gLast_panel_frame_time)[i];
        if (C2V(gPanel_buffer)[i] != NULL && C2V(gPanel_flic)[i].data != NULL) {
            if (old_last_time[i] != 0) {
                time_diff = the_time - old_last_time[i];
                iteration_count = time_diff / C2V(gPanel_flic)[i].frame_period;
            } else {
                iteration_count = 1;
            }
            for (j = 0; j < iteration_count; j++) {
                finished = PlayNextFlicFrame(&C2V(gPanel_flic)[i]);
                if (finished) {
                    EndFlic(&C2V(gPanel_flic)[i]);
                    StartFlic(
                            C2V(gPanel_flic)[i].file_name,
                            C2V(gPanel_flic)[i].the_index,
                            &C2V(gPanel_flic)[i],
                            C2V(gPanel_flic_data_length)[i],
                            (tS8*)C2V(gPanel_flic_data)[i],
                            C2V(gPanel_buffer)[i],
                            0,
                            0,
                            0);
                }
                C2V(gLast_panel_frame_time)[i] = the_time;
            }
            if (pCopy_to_buffer) {
                BrPixelmapRectangleCopy(
                        C2V(gBack_screen),
                        C2V(gPanel_flic_left)[i],
                        C2V(gPanel_flic_top)[i],
                        C2V(gPanel_buffer)[i],
                        0,
                        0,
                        C2V(gPanel_buffer)[i]->width,
                        C2V(gPanel_buffer)[i]->height);
            }
        }
    }
    TurnFlicTransparencyOff();
    LetFlicFuckWithPalettes();
}
C2_HOOK_FUNCTION(0x00463130, ServicePanelFlics)

void C2_HOOK_FASTCALL ChangePanelFlic(int pIndex, tU8* pData, tU32 pData_length) {

    EndFlic(&C2V(gPanel_flic)[pIndex]);
    C2V(gPanel_flic_data)[pIndex] = pData;
    C2V(gPanel_flic_data_length)[pIndex] = pData_length;
    BrPixelmapFill(C2V(gPanel_buffer)[pIndex], 0);
    StartFlic(
            C2V(gPanel_flic)[pIndex].file_name,
            pIndex,
            &C2V(gPanel_flic)[pIndex],
            C2V(gPanel_flic_data_length)[pIndex],
            (tS8*)C2V(gPanel_flic_data)[pIndex],
            C2V(gPanel_buffer)[pIndex],
            0,
            0,
            0);
    C2V(gLast_panel_frame_time)[pIndex] = 0;
    ServicePanelFlics(0);
}
C2_HOOK_FUNCTION(0x00463270, ChangePanelFlic)

br_pixelmap* C2_HOOK_FASTCALL GetPanelPixelmap(int pIndex) {

    return C2V(gPanel_buffer)[pIndex];
}
C2_HOOK_FUNCTION(0x00463330, GetPanelPixelmap)

void (C2_HOOK_FASTCALL * LoadInterfaceStrings_original)(void);
void C2_HOOK_FASTCALL LoadInterfaceStrings(void) {

#if 0 //defined(C2_HOOKS_ENABLED)
    LoadInterfaceStrings_original();
#else
    FILE* f;
    char s[256];
    char s2[256];
    char* str;
    char ch;
    tPath_name the_path;
    int i;
    int j;

    C2V(gTranslation_count) = 0;
    PathCat(the_path, C2V(gApplication_path), "TRNSLATE.TXT");
    f = TWT_fopen(the_path, "rt");
    if (f == NULL) {
        return;
    }
    while (!DRfeof(f)) {
        GetALineAndDontArgue(f, s);
        C2V(gTranslation_count)++;
    }
    DRrewind(f);
    C2V(gTranslations) = BrMemAllocate(C2V(gTranslation_count) * sizeof(tTranslation_record), kMem_misc);
    for (i = 0; i < C2V(gTranslation_count); i++) {
        GetALineAndDontArgue(f, s);
        str = c2_strtok(s, "\t ,/");
        c2_strcpy(s2, str);
        c2_strtok(s2, ".");
        c2_strcat(s2, ".FLI");
        C2V(gTranslations)[i].flic_index = -1;
        for (j = 0; j < REC2_ASIZE(C2V(gMain_flic_list)); j++) {
            if (c2_strcmp(C2V(gMain_flic_list)[j].file_name, s2) == 0) {
                C2V(gTranslations)[i].flic_index = j;
                break;
            }
        }
        if (C2V(gTranslations)[i].flic_index < 0) {
            FatalError(kFatalError_CannotFindFlicReferencedTranslation_S, s2);
        }
        str[c2_strlen(str)] = ',';
        c2_strtok(s, "\t ,/");
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &C2V(gTranslations)[i].x);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &C2V(gTranslations)[i].y);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &C2V(gTranslations)[i].font_index);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%c", &ch);
        switch (ch) {
            case 'C':
            case 'c':
                C2V(gTranslations[i]).justification = eJust_centre;
                break;
            case 'L':
            case 'l':
                C2V(gTranslations[i]).justification = eJust_left;
                break;
            case 'R':
            case 'r':
                C2V(gTranslations[i]).justification = eJust_right;
                break;
        }
        str += c2_strlen(str) + 1;
        C2V(gTranslations)[i].text = BrMemAllocate(c2_strlen(str) + 1, kMem_misc);
        c2_strcpy(C2V(gTranslations)[i].text, str);
    }
    LoadFont(1);
    LoadFont(2);
    C2V(gTrans_fonts)[0] = &C2V(gFonts)[1];
    C2V(gTrans_fonts)[1] = &C2V(gFonts)[2];

    DRfclose(f);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00463340, LoadInterfaceStrings, LoadInterfaceStrings_original)

void C2_HOOK_FASTCALL SuspendPendingFlic(void) {

    C2V(gPending_pending_flic) = C2V(gPending_flic);
    C2V(gPending_flic) = -1;
}
C2_HOOK_FUNCTION(0x00463700, SuspendPendingFlic)

void C2_HOOK_FASTCALL ResumePendingFlic(void) {

    C2V(gPending_flic) = C2V(gPending_pending_flic);
}
C2_HOOK_FUNCTION(0x00463720, ResumePendingFlic)

int C2_HOOK_FASTCALL TranslationMode(void) {

    return C2V(gTranslation_count);
}
C2_HOOK_FUNCTION(0x00461990, TranslationMode)

void C2_HOOK_FASTCALL PlayFlicsFromDisk(void) {

    C2V(gPlay_from_disk) = 1;
}
C2_HOOK_FUNCTION(0x00461a00, PlayFlicsFromDisk)
