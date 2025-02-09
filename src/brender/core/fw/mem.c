#include "mem.h"

#include "core/fw/fwsetup.h"
#include "core/std/brstdlib.h"

void* (C2_HOOK_CDECL * BrMemAllocate_original)(br_size_t size, br_uint_8 type);
void* C2_HOOK_CDECL BrMemAllocate(br_size_t size, br_uint_8 type) {
#if 0//defined(C2_HOOKS_ENABLED)
return BrMemAllocate_original(size, type);
#else
    void* b;

    b = C2V(fw).mem->allocate(size, type);
    BrMemSet(b, 0, size);
    return b;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005275c0, BrMemAllocate, BrMemAllocate_original)

// IDA: void __cdecl BrMemFree(void *block)
void (C2_HOOK_CDECL * BrMemFree_original)(void* block);
void C2_HOOK_CDECL BrMemFree(void* block) {
#if 0//defined(C2_HOOKS_ENABLED)
    BrMemFree_original(block);
#else
    C2V(fw).mem->free(block);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005275f0, BrMemFree, BrMemFree_original)

br_size_t (C2_HOOK_CDECL * BrMemInquire_original)(br_uint_8 type);
br_size_t C2_HOOK_CDECL BrMemInquire(br_uint_8 type) {
#if 0//defined(C2_HOOKS_ENABLED)
    return BrMemInquire_original(type);
#else
    br_size_t i;
    i = C2V(fw).mem->inquire(type);
    return i;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00527610, BrMemInquire, BrMemInquire_original)

br_int_32 (C2_HOOK_CDECL * BrMemAlign_original)(br_uint_8 type);
br_int_32 C2_HOOK_CDECL BrMemAlign(br_uint_8 type) {
#if 0//defined(C2_HOOKS_ENABLED)
    return BrMemAlign_original(type);
#else
    br_int_32 i = 0;
    if (C2V(fw).mem->align) {
        i = C2V(fw).mem->align(type);
    }
    return i;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00527630, BrMemAlign, BrMemAlign_original)

void* (C2_HOOK_CDECL * BrMemCalloc_original)(int nelems, br_size_t size, br_uint_8 type);
void* C2_HOOK_CDECL BrMemCalloc(int nelems, br_size_t size, br_uint_8 type) {
#if 0//defined(C2_HOOKS_ENABLED)
    return BrMemCalloc_original(nelems, size, type);
#else
    void* b;
    b = C2V(fw).mem->allocate(nelems * size, type);
    BrMemSet(b, 0, nelems * size);
    return b;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00527650, BrMemCalloc, BrMemCalloc_original)

char* (C2_HOOK_CDECL * BrMemStrDup_original)(const char* str);
char* C2_HOOK_CDECL BrMemStrDup(const char* str) {
#if 0//defined(C2_HOOKS_ENABLED)
    return BrMemStrDup_original(str);
#else
    int l;
    char* nstr;

    l = BrStrLen(str);
    nstr = C2V(fw).mem->allocate(l + 1, BR_MEMORY_STRING);
    BrMemSet(nstr, 0, l + 1);
    BrStrCpy(nstr,str);
    return nstr;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00527680, BrMemStrDup, BrMemStrDup_original)
