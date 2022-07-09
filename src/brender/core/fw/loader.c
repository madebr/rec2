#include "loader.h"

br_image* (C2_HOOK_STDCALL * ImageLoad_original)(char* name);
br_image* C2_HOOK_STDCALL ImageLoad(char* name) {
#if defined(C2_HOOKS_ENABLED)
    return ImageLoad_original(name);
#else
    void* fh;
    int mode;
    msdos_header dos_header;
    coff_header coff_header;
    nt_optional_header nt_header;
    section_header section_header;
    br_uint_32 pe;
    br_image* img;
    br_uint_8* arena_base;
    int arena_size;
    int arena_align;
    int i;
    br_uint_32 offset;
    unsigned int n;
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00530e70, ImageLoad, ImageLoad_original)
