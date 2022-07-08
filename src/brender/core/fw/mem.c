#include "mem.h"

void* (C2_HOOK_CDECL * BrMemAllocate_original)(br_size_t size, br_uint_8 type);
void* C2_HOOK_CDECL BrMemAllocate(br_size_t size, br_uint_8 type) {
#if defined(C2_HOOKS_ENABLED)
return BrMemAllocate_original(size, type);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005275c0, BrMemAllocate, BrMemAllocate_original)

// IDA: void __cdecl BrMemFree(void *block)
void (C2_HOOK_CDECL * BrMemFree_original)(void* block);
void C2_HOOK_CDECL BrMemFree(void* block) {
#if defined(C2_HOOKS_ENABLED)
    BrMemFree_original(block);
#else
#error "Not implemented"
#endif
}

C2_HOOK_FUNCTION_ORIGINAL(0x005275f0, BrMemFree, BrMemFree_original)
