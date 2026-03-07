#include "himage.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

// FUNCTION: CARMA2_HW 0x0053fb50
void* C2_HOOK_CDECL HostImageLoad(const char* name) {

#ifdef _WIN32
    return LoadLibraryA(name);
#else
    return dlopen(name, RTLD_NOW);
#endif
}


// FUNCTION: CARMA2_HW 0x0053fb60
void C2_HOOK_CDECL HostImageUnload(void* image) {

}

// FUNCTION: CARMA2_HW 0x0053fb70
void* C2_HOOK_CDECL HostImageLookupName(void* img, const char* name, br_uint_32 hint) {

#ifdef _WIN32
    return GetProcAddress((HMODULE)img, name);
#else
    return dlsym(img, name);
#endif
}

// FUNCTION: CARMA2_HW 0x0053fb90
void* C2_HOOK_CDECL HostImageLookupOrdinal(void* img, br_uint_32 ordinal) {

    return NULL;
}
