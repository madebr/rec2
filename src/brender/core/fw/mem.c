#include "mem.h"

#include "core/fw/fwsetup.h"
#include "core/std/brstdlib.h"

// FUNCTION: CARMA2_HW 0x005275c0
void* C2_HOOK_CDECL BrMemAllocate(br_size_t size, br_uint_8 type) {
    void* b;

    b = fw.mem->allocate(size, type);
    BrMemSet(b, 0, size);
    return b;
}

// IDA: void __cdecl BrMemFree(void *block)
// FUNCTION: CARMA2_HW 0x005275f0
void C2_HOOK_CDECL BrMemFree(void* block) {

    fw.mem->free(block);
}

// FUNCTION: CARMA2_HW 0x00527610
br_size_t C2_HOOK_CDECL BrMemInquire(br_uint_8 type) {
    br_size_t i;

    i = fw.mem->inquire(type);
    return i;
}

// FUNCTION: CARMA2_HW 0x00527630
br_int_32 C2_HOOK_CDECL BrMemAlign(br_uint_8 type) {
    br_int_32 i = 0;

    if (fw.mem->align) {
        i = fw.mem->align(type);
    }
    return i;
}

// FUNCTION: CARMA2_HW 0x00527650
void* C2_HOOK_CDECL BrMemCalloc(int nelems, br_size_t size, br_uint_8 type) {
    void* b;

    b = fw.mem->allocate(nelems * size, type);
    BrMemSet(b, 0, nelems * size);
    return b;
}

// FUNCTION: CARMA2_HW 0x00527680
char* C2_HOOK_CDECL BrMemStrDup(const char* str) {
    int l;
    char* nstr;

    l = BrStrLen(str);
    nstr = fw.mem->allocate(l + 1, BR_MEMORY_STRING);
    BrMemSet(nstr, 0, l + 1);
    BrStrCpy(nstr,str);
    return nstr;
}
