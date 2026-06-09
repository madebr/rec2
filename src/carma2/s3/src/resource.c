#include "../include/s3/internal/resource.h"

// FUNCTION: CARMA2_HW 0x00565af7
void* C2_HOOK_FASTCALL S3MemAllocate(br_size_t size, br_uint_8 type) {
    void *result;

    result = BrMemAllocate(size, type);
    return result;
}

// FUNCTION: CARMA2_HW 0x00565b1d
void C2_HOOK_FASTCALL S3MemFree(void* p) {

    BrMemFree(p);
}

// FUNCTION: CARMA2_HW 0x005186f0
FILE* C2_HOOK_FASTCALL S3_low_memory_fopen(const char* pPath, const char* pMode) {
    return NULL;
}

// FUNCTION: CARMA2_HW 0x00568bdc
int C2_HOOK_FASTCALL S3GetFileSize(FILE *pF) {
    struct {
        int current;
        int end;
    } pos;

    pos.current = ftell(pF);
    fseek(pF, 0, SEEK_END);
    pos.end = ftell(pF);
    fseek(pF, pos.current, SEEK_SET);
    return pos.end;
}
