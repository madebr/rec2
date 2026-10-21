#include "preptab.h"

#include "prepmap.h"

// FUNCTION: CARMA2_HW 0x00525fa0
// FUNCTION: CARMA2_SW 0x00521640
void C2_HOOK_CDECL BrTableUpdate(br_pixelmap* table, br_uint_16 flags) {
    BrBufferUpdate(table, BRT_UNKNOWN, flags);
}
