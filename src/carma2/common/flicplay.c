#include "flicplay.h"

#include "utility.h"

#include "brender/brender.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tFlic_descriptor, gPanel_flic, 2, 0x00686218);

C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gPalette, 0x00686208);
C2_HOOK_VARIABLE_IMPLEMENT(void*, gPalette_pixels, 0x006861d8);

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
