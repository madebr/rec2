#include "brstdmem.h"

#include "core/fw/diag.h"

#include "c2_stdlib.h"

// Global variables

// GLOBAL: CARMA2_HW 0x0066fd70
br_allocator BrStdlibAllocator = { "malloc", BrStdlibAllocate, BrStdlibFree, BrStdlibInquire, BrStdlibAlign };

// GLOBAL: CARMA2_HW 0x0066fd84
br_allocator* _BrDefaultAllocator = &BrStdlibAllocator;

// FUNCTION: CARMA2_HW 0x0053f5f0
void* C2_HOOK_CDECL BrStdlibAllocate(br_size_t size, br_uint_8 type) {
    void* m = malloc(size);

    if (m == NULL) {
        BrFailure("BrStdlibAllocate: failed with size=%d, type=%d", size, type);
    }

    return m;
}

// FUNCTION: CARMA2_HW 0x0053f630
void C2_HOOK_CDECL BrStdlibFree(void* mem) {
    free(mem);
}

// FUNCTION: CARMA2_HW 0x0053f640
br_size_t C2_HOOK_CDECL BrStdlibInquire(br_uint_8 type) {
    return 0;
}

// FUNCTION: CARMA2_HW 0x0053f650
br_uint_32 C2_HOOK_CDECL BrStdlibAlign(br_uint_8 type) {
    return sizeof(void*);
}
