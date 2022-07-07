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
