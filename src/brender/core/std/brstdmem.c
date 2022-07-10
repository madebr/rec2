#include "brstdmem.h"

#include "core/fw/diag.h"

#include "c2_stdlib.h"

// Global variables
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_allocator, BrStdlibAllocator, 0x0066fd70, { "malloc", BrStdlibAllocate, BrStdlibFree, BrStdlibInquire, BrStdlibAlign });
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_allocator*, _BrDefaultAllocator, 0x0066fd84, &BrStdlibAllocator);

void* C2_HOOK_CDECL BrStdlibAllocate(br_size_t size, br_uint_8 type) {
    void* m = c2_malloc(size);

    if (m == NULL) {
        BrFailure("BrStdlibAllocate: failed with size=%d, type=%d", size, type);
    }

    return m;
}
C2_HOOK_FUNCTION(0x0053f5f0, BrStdlibAllocate)

void C2_HOOK_CDECL BrStdlibFree(void* mem) {
    c2_free(mem);
}
C2_HOOK_FUNCTION(0x0053f630, BrStdlibFree)

br_size_t C2_HOOK_CDECL BrStdlibInquire(br_uint_8 type) {
    return 0;
}
C2_HOOK_FUNCTION(0x0053f640, BrStdlibInquire)

br_uint_32 C2_HOOK_CDECL BrStdlibAlign(br_uint_8 type) {
    return sizeof(void*);
}
C2_HOOK_FUNCTION(0x0053f650, BrStdlibAlign)
