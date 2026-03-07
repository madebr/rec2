#include "scratch.h"

#include "core/fw/diag.h"
#include "core/fw/fwsetup.h"
#include "core/fw/resource.h"

// GLOBAL: CARMA2_HW 0x00666ea8
char scratchString[512] = "SCRATCH";

// FUNCTION: CARMA2_HW 0x00529e80
void* C2_HOOK_CDECL BrScratchAllocate(br_size_t size) {

    if (fw.scratch_inuse != 0) {
        BrFailure("Scratchpad not available");
    }

    fw.scratch_last = size;

    if (size > fw.scratch_size) {
        if (fw.scratch_ptr != NULL) {
            BrResFree(fw.scratch_ptr);
        }
        fw.scratch_ptr = BrResAllocate(fw.res, size, BR_MEMORY_SCRATCH);
        fw.scratch_size = size;
    }
    fw.scratch_inuse = 1;
    return fw.scratch_ptr;
}

// FUNCTION: CARMA2_HW 0x00529ef0
void C2_HOOK_CDECL BrScratchFree(void* scratch) {

    fw.scratch_inuse = 0;
}

// FUNCTION: CARMA2_HW 0x00529f00
void C2_HOOK_CDECL BrScratchFlush(void) {

    if (fw.scratch_inuse != 0) {
        BrFailure("Scratchpad cannot be flushed while in use");
    }
    if (fw.scratch_ptr != NULL) {
        BrResFree(fw.scratch_ptr);
    }
    fw.scratch_ptr = NULL;
    fw.scratch_size = 0;
}

// FUNCTION: CARMA2_HW 0x00529f40
br_size_t C2_HOOK_CDECL BrScratchInquire(void) {

    return fw.scratch_size;
}

// FUNCTION: CARMA2_HW 0x00529f50
char* C2_HOOK_CDECL BrScratchString(void) {

    return scratchString;
}

// FUNCTION: CARMA2_HW 0x00529f60
br_size_t C2_HOOK_CDECL BrScratchStringSize(void) {

    return sizeof(scratchString);
}
