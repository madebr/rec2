#include "scratch.h"

#define HOOK_SCRATCH 1

#if !HOOK_SCRATCH
#include "core/fw/diag.h"
#include "core/fw/fwsetup.h"
#include "core/fw/resource.h"
#endif

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(char, scratchString, 512, 0x00666ea8, "SCRATCH");

void* (C2_HOOK_CDECL * BrScratchAllocate_original)(br_size_t size);
void* C2_HOOK_CDECL BrScratchAllocate(br_size_t size) {
#if HOOK_SCRATCH
    return BrScratchAllocate_original(size);
#else
    if (C2V(fw).scratch_inuse != 0) {
        BrFailure("Scratchpad not available");
    }

    C2V(fw).scratch_last = size;

    if (size > C2V(fw).scratch_size) {
        if (C2V(fw).scratch_ptr != NULL) {
            BrResFree(C2V(fw).scratch_ptr);
        }
        C2V(fw).scratch_ptr = BrResAllocate(C2V(fw).res, size, BR_MEMORY_SCRATCH);
        C2V(fw).scratch_size = size;
    }
    C2V(fw).scratch_inuse = 1;
    return C2V(fw).scratch_ptr;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00529e80, BrScratchAllocate, BrScratchAllocate_original)

void (C2_HOOK_CDECL * BrScratchFree_original)(void* scratch);
void C2_HOOK_CDECL BrScratchFree(void* scratch) {
#if HOOK_SCRATCH
    BrScratchFree_original(scratch);
#else
    C2V(fw).scratch_inuse = 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00529ef0, BrScratchFree, BrScratchFree_original)

void (C2_HOOK_CDECL * BrScratchFlush_original)(void);
void C2_HOOK_CDECL BrScratchFlush(void) {
#if HOOK_SCRATCH
    BrScratchFlush_original();
#else
    if (C2V(fw).scratch_inuse != 0) {
        BrFailure("Scratchpad cannot be flushed while in use");
    }
    if (C2V(fw).scratch_ptr != NULL) {
        BrResFree(C2V(fw).scratch_ptr);
    }
    C2V(fw).scratch_ptr = NULL;
    C2V(fw).scratch_size = 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00529f00, BrScratchFlush, BrScratchFlush_original)

br_size_t (C2_HOOK_CDECL * BrScratchInquire_original)(void);
br_size_t C2_HOOK_CDECL BrScratchInquire(void) {
#if HOOK_SCRATCH
    return BrScratchInquire_original();
#else
    return C2V(fw).scratch_size;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00529f40, BrScratchInquire, BrScratchInquire_original)

char* (C2_HOOK_CDECL * BrScratchString_original)(void);
char* C2_HOOK_CDECL BrScratchString(void) {
#if HOOK_SCRATCH
    return BrScratchString_original();
#else
    return C2V(scratchString);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00529f50, BrScratchString, BrScratchString_original)

br_size_t (C2_HOOK_CDECL * BrScratchStringSize_original)(void);
br_size_t C2_HOOK_CDECL BrScratchStringSize(void) {
#if HOOK_SCRATCH
    return BrScratchStringSize_original();
#else
    return sizeof(C2V(scratchString));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00529f60, BrScratchStringSize, BrScratchStringSize_original)
