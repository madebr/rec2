#include "resource.h"

void* C2_HOOK_FASTCALL S3MemAllocate(br_size_t size, br_uint_8 type) {

    return BrMemAllocate(size, type);
}
C2_HOOK_FUNCTION(0x00565af7, S3MemAllocate)

void C2_HOOK_FASTCALL S3MemFree(void* p) {

    BrMemFree(p);
}
C2_HOOK_FUNCTION(0x00565b1d, S3MemFree)

FILE* C2_HOOK_FASTCALL S3_low_memory_fopen(const char* pPath, const char* pMode) {
    return NULL;
}
C2_HOOK_FUNCTION(0x005186f0, S3_low_memory_fopen)
