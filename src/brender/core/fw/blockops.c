/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */
#include "blockops.h"

#ifndef REC2_MATCHING
#include <string.h>
#endif


// FUNCTION: CARMA2_HW 0x00528130
// FUNCTION: CARMA2_SW 0x005237d0
C2_MATCHING_NAKED void C2_HOOK_CDECL BrBlockFill(void* pDestination, int pValue, int pCount_dwords) {
#ifdef REC2_MATCHING
    __asm {
        push        ebp
        mov         ebp, esp
        push        eax
        push        ecx
        push        edi
        push        es
        mov         ax, ds
        mov         es, ax
        mov         eax, dword ptr [ebp+0xc]
        mov         ecx, dword ptr [ebp+0x10]
        mov         edi, dword ptr [ebp+0x8]
        rep stosd
        pop         es
        pop         edi
        pop         ecx
        pop         eax
        leave
        ret
    }
#else
    memset(pDestination, pValue, 4 * pCount_dwords);
#endif
}

// FUNCTION: CARMA2_HW 0x0052814e
// FUNCTION: CARMA2_SW 0x005237ee
C2_MATCHING_NAKED void C2_HOOK_CDECL BrBlockCopy(void* pDestination, const void* pSource, int pCount_dwords) {
#ifdef REC2_MATCHING
    __asm {
        push        ebp
        mov         ebp, esp
        push        ecx
        push        esi
        push        edi
        push        es
        mov         ax, ds
        mov         es, ax
        mov         esi, dword ptr [ebp + 0xc]
        mov         ecx, dword ptr [ebp + 0x10]
        mov         edi, dword ptr [ebp + 8]
        rep movsd
        pop         es
        pop         edi
        pop         esi
        pop         ecx
        leave
        ret
    }
#else
    memcpy(pDestination, pSource, 4 * pCount_dwords);
#endif
}

// FUNCTION: CARMA2_HW 0x0052816c
// FUNCTION: CARMA2_SW 0x0052380c
C2_MATCHING_NAKED void C2_HOOK_CDECL BrFarBlockCopy(void *pDestination, const void *pSource, int pCount_dwords) {
#ifdef REC2_MATCHING
    __asm {
        push        ebp
        mov         ebp, esp
        push        ecx
        push        esi
        push        edi
        push        es
        mov         ecx, dword ptr [ebp + 0x14]
        mov         esi, dword ptr [ebp + 0x10]
        les         edi, [ebp + 8]
        rep movsd
        pop         es
        pop         edi
        pop         esi
        pop         ecx
        leave
        ret
    }
#else
    memcpy(pDestination, pSource, 4 * pCount_dwords);
#endif
}
