#include "preptab.h"

#include "prepmap.h"

void C2_HOOK_CDECL BrTableUpdate(br_pixelmap* table, br_uint_16 flags) {
    BrBufferUpdate(table, BRT_UNKNOWN, flags);
}
C2_HOOK_FUNCTION(0x00525fa0, BrTableUpdate)
