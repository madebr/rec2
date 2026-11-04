/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "realmem.h"

#include "c2_stdlib.h"

extern br_uint_16 _RealSelector;

// FUNCTION: CARMA2_HW 0x0054009c
// FUNCTION: CARMA2_SW 0x0053b73c
C2_MATCHING_NAKED void C2_HOOK_CDECL HostRealBlockWrite(br_uint_16 offset, br_uint_16 seg, void *block, br_uint_32 count) {
#ifdef REC2_MATCHING
    __asm {
        push ebp
        mov ebp, esp
        push es
        push esi
        push edi
        mov es, word ptr [_RealSelector]
        mov edi, dword ptr [ebp+0xc]
        shl edi, 4
        add edi, dword ptr [ebp+8]
        mov esi, dword ptr [ebp+0x10]
        mov ecx, dword ptr [ebp+0x14]
        shr ecx, 1
        rep movsw
        adc ecx, ecx
        rep movsb
        pop edi
        pop esi
        pop es
        leave
        ret
    }
#else
    abort();
#endif
}

// FUNCTION: CARMA2_HW 0x005400c6
// FUNCTION: CARMA2_SW 0x0053b766
C2_MATCHING_NAKED void C2_HOOK_CDECL HostRealBlockFill(br_uint_16 offset, br_uint_16 seg, br_uint_8 value, br_uint_32 count) {
#ifdef REC2_MATCHING
    __asm {
        push ebp
        mov ebp, esp
        push es
        push esi
        push edi
        mov es, word ptr [_RealSelector]
        mov edi, dword ptr [ebp+0xc]
        shl edi, 4
        add edi, dword ptr [ebp+8]
        mov al, byte ptr [ebp+0x10]
        mov ecx, dword ptr [ebp+0x14]
        mov ah, al
        shr ecx, 1
        rep stosw
        adc ecx, ecx
        rep stosb
        pop edi
        pop esi
        pop es
        leave
        ret
    }
#else
    abort();
#endif
}

// FUNCTION: CARMA2_HW 0x005400f2
// FUNCTION: CARMA2_SW 0x0053b792
C2_MATCHING_NAKED void C2_HOOK_CDECL HostRealBlockRead(br_uint_16 offset, br_uint_16 seg, void *block, br_uint_32 count) {
#ifdef REC2_MATCHING
    __asm {
        push ebp
        mov ebp, esp
        push ds
        push esi
        push edi
        mov ds, word ptr [_RealSelector]
        mov esi, dword ptr [ebp+0xc]
        shl esi, 4
        add esi, dword ptr [ebp+8]
        mov edi, dword ptr [ebp+0x10]
        mov ecx, dword ptr [ebp+0x14]
        shr ecx, 1
        rep movsw
        adc ecx, ecx
        rep movsb
        pop edi
        pop esi
        pop ds
        leave
        ret
    }
#else
    abort();
#endif
}

// FUNCTION: CARMA2_HW 0x0054011c
// FUNCTION: CARMA2_SW 0x0053b7bc
C2_MATCHING_NAKED br_uint_32 C2_HOOK_CDECL HostRealStringWrite(br_uint_16 offset, br_uint_16 seg, br_uint_8 *string, br_uint_32 max) {
#ifdef REC2_MATCHING
    __asm {
        push ebp
        mov ebp, esp
        push es
        push esi
        push edi
        mov es, word ptr [_RealSelector]
        mov edi, dword ptr [ebp+0xc]
        shl edi, 4
        add edi, dword ptr [ebp+8]
        mov esi, dword ptr [ebp+0x10]
        mov ecx, dword ptr [ebp+0x14]
        xor eax, eax
        and ecx, ecx
        je string_write_done
    string_write_loop:
        mov dl, byte ptr [esi]
        mov byte ptr es:[edi], dl
        inc eax
        and dl, dl
        je string_write_done
        inc esi
        inc edi
        dec ecx
        jne string_write_loop
        mov byte ptr es:[edi - 1], 0
    string_write_done:
        pop edi
        pop esi
        pop es
        leave
        ret
    }
#else
    abort();
#endif
}

// FUNCTION: CARMA2_HW 0x00540157
// FUNCTION: CARMA2_SW 0x0053b7f7
C2_MATCHING_NAKED br_uint_32 C2_HOOK_CDECL HostRealStringRead(br_uint_16 offset, br_uint_16 seg, br_uint_8 *string, br_uint_32 max) {
#ifdef REC2_MATCHING
    __asm {
        push ebp
        mov ebp, esp
        push es
        push esi
        push edi
        mov es, word ptr [_RealSelector]
        mov edi, dword ptr [ebp+0xc]
        shl edi, 4
        add edi, dword ptr [ebp+8]
        mov esi, dword ptr [ebp+0x10]
        mov ecx, dword ptr [ebp+0x14]
        xor eax, eax
        and ecx, ecx
        je string_read_done
    string_read_loop:
        mov dl, byte ptr es:[edi]
        mov byte ptr [esi], dl
        inc eax
        and dl, dl
        je string_read_done
        inc esi
        inc edi
        dec ecx
        jne string_read_loop
        mov byte ptr [esi - 1], 0
    string_read_done:
        pop edi
        pop esi
        pop es
        leave
        ret
    }
#else
    abort();
#endif
}

// FUNCTION: CARMA2_HW 0x00540191
// FUNCTION: CARMA2_SW 0x0053b831
C2_MATCHING_NAKED void C2_HOOK_CDECL HostRealByteWrite(br_uint_16 offset, br_uint_16 seg, br_uint_8 value) {
#ifdef REC2_MATCHING
    __asm {
        push ebp
        mov ebp, esp
        push ds
        mov ds, word ptr [_RealSelector]
        mov edx, dword ptr [ebp+0xc]
        shl edx, 4
        add edx, dword ptr [ebp+8]
        mov al, byte ptr [ebp+0x10]
        mov byte ptr [edx], al
        pop ds
        leave
        ret
    }
#else
    abort();
#endif
}

// FUNCTION: CARMA2_HW 0x005401ad
// FUNCTION: CARMA2_SW 0x0053b84d
C2_MATCHING_NAKED br_uint_8 C2_HOOK_CDECL HostRealByteRead(br_uint_16 offset, br_uint_16 seg) {
#ifdef REC2_MATCHING
    __asm {
        push ebp
        mov ebp, esp
        push ds
        mov ds, word ptr [_RealSelector]
        mov eax, dword ptr [ebp+0xc]
        shl eax, 4
        add eax, dword ptr [ebp+8]
        mov al, byte ptr [eax]
        pop ds
        leave
        ret
    }
#else
    abort();
#endif
}

// FUNCTION: CARMA2_HW 0x005401c6
// FUNCTION: CARMA2_SW 0x0053b866
C2_MATCHING_NAKED void C2_HOOK_CDECL HostRealWordWrite(br_uint_16 offset, br_uint_16 seg, br_uint_16 value) {
#ifdef REC2_MATCHING
    __asm {
        push ebp
        mov ebp, esp
        push ds
        mov ds, word ptr [_RealSelector]
        mov edx, dword ptr [ebp+0xc]
        shl edx, 4
        add edx, dword ptr [ebp+8]
        mov ax, word ptr [ebp+0x10]
        mov word ptr [edx], ax
        pop ds
        leave
        ret
    }
#else
    abort();
#endif
}

// FUNCTION: CARMA2_HW 0x005401e4
// FUNCTION: CARMA2_SW 0x0053b884
C2_MATCHING_NAKED br_uint_16 C2_HOOK_CDECL HostRealWordRead(br_uint_16 offset, br_uint_16 seg) {
#ifdef REC2_MATCHING
    __asm {
        push ebp
        mov ebp, esp
        push ds
        mov ds, word ptr [_RealSelector]
        mov eax, dword ptr [ebp+0xc]
        shl eax, 4
        add eax, dword ptr [ebp+8]
        mov ax, word ptr [eax]
        pop ds
        leave
        ret
    }
#else
    abort();
#endif
}

// FUNCTION: CARMA2_HW 0x005401fe
// FUNCTION: CARMA2_SW 0x0053b89e
C2_MATCHING_NAKED void C2_HOOK_CDECL HostRealDWordWrite(br_uint_16 offset, br_uint_16 seg, br_uint_32 value) {
#ifdef REC2_MATCHING
    __asm {
        push ebp
        mov ebp, esp
        push ds
        mov ds, word ptr [_RealSelector]
        mov edx, dword ptr [ebp+0xc]
        shl edx, 4
        add edx, dword ptr [ebp+8]
        mov eax, dword ptr [ebp+0x10]
        mov dword ptr [edx], eax
        pop ds
        leave
        ret
    }
#else
    abort();
#endif
}

// FUNCTION: CARMA2_HW 0x0054021a
// FUNCTION: CARMA2_SW 0x0053b8ba
C2_MATCHING_NAKED br_uint_32 C2_HOOK_CDECL HostRealDWordRead(br_uint_16 offset, br_uint_16 seg) {
#ifdef REC2_MATCHING
    __asm {
        push ebp
        mov ebp, esp
        push ds
        mov ds, word ptr [_RealSelector]
        mov edx, dword ptr [ebp+0xc]
        shl edx, 4
        add edx, dword ptr [ebp+8]
        mov eax, dword ptr [ebp+0xc]
        shl eax, 4
        add eax, dword ptr [ebp+8]
        mov eax, dword ptr [eax]
        pop ds
        leave
        ret
    }
#else
    abort();
#endif
}
