#include "flicplay.h"

#include "errors.h"
#include "globvars.h"
#include "loading.h"
#include "utility.h"

#include "brender/brender.h"

#include "c2_string.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tFlic_descriptor, gPanel_flic, 2, 0x00686218);

C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gPalette, 0x00686208);
C2_HOOK_VARIABLE_IMPLEMENT(void*, gPalette_pixels, 0x006861d8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPlay_from_disk, 0x00686318);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gLast_flic_name, 14, 0x00686330);

int C2_HOOK_FASTCALL GetPanelFlicFrameIndex(int pIndex) {

    return C2V(gPanel_flic)[pIndex].current_frame;
}
C2_HOOK_FUNCTION(0x00461a40, GetPanelFlicFrameIndex)

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
