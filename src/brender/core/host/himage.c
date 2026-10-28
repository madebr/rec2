/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "himage.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

// FUNCTION: CARMA2_HW 0x0053fb50
// FUNCTION: CARMA2_SW 0x0053b1f0
void* C2_HOOK_CDECL HostImageLoad(const char* name) {

#ifdef _WIN32
    return LoadLibraryA(name);
#else
    return dlopen(name, RTLD_NOW);
#endif
}


// FUNCTION: CARMA2_HW 0x0053fb60
// FUNCTION: CARMA2_SW 0x0053b200
void C2_HOOK_CDECL HostImageUnload(void* image) {

}

// FUNCTION: CARMA2_HW 0x0053fb70
// FUNCTION: CARMA2_SW 0x0053b210
void* C2_HOOK_CDECL HostImageLookupName(void* img, const char* name, br_uint_32 hint) {

#ifdef _WIN32
    return GetProcAddress((HMODULE)img, name);
#else
    return dlsym(img, name);
#endif
}

// FUNCTION: CARMA2_HW 0x0053fb90
// FUNCTION: CARMA2_SW 0x0053b230
void* C2_HOOK_CDECL HostImageLookupOrdinal(void* img, br_uint_32 ordinal) {

    return NULL;
}
