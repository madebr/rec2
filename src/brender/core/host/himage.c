#include "himage.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

void* (C2_HOOK_CDECL * HostImageLoad_original)(const char* name);
void* C2_HOOK_CDECL HostImageLoad(const char* name) {
#if 0//defined(C2_HOOKS_ENABLED)
    return HostImageLoad_original(name);
#else

#ifdef _WIN32
    return LoadLibraryA(name);
#else
    return dlopen(name, RTLD_NOW);
#endif
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053fb50, HostImageLoad, HostImageLoad_original)


void (C2_HOOK_CDECL * HostImageUnload_original)(void* image);
void C2_HOOK_CDECL HostImageUnload(void* image) {
#if 0//defined(C2_HOOKS_ENABLED)
    HostImageUnload_original(image);
#else

#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053fb60, HostImageUnload, HostImageUnload_original)

void* (C2_HOOK_CDECL * HostImageLookupName_original)(void* img, const char* name, br_uint_32 hint);
void* C2_HOOK_CDECL HostImageLookupName(void* img, const char* name, br_uint_32 hint) {
#if 0//defined(C2_HOOKS_ENABLED)
    return HostImageLookupName_original(img, name, hint);
#else
#ifdef _WIN32
    return GetProcAddress((HMODULE)img, name);
#else
    return dlsym(img, name);
#endif
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053fb70, HostImageLookupName, HostImageLookupName_original)

void* (C2_HOOK_CDECL * HostImageLookupOrdinal_original)(void* img, br_uint_32 ordinal);
void* C2_HOOK_CDECL HostImageLookupOrdinal(void* img, br_uint_32 ordinal) {
#if 0//defined(C2_HOOKS_ENABLED)
    return HostImageLookupOrdinal_original(img, ordinal);
#else

    return NULL;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053fb90, HostImageLookupOrdinal, HostImageLookupOrdinal_original)
