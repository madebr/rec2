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

#if defined(__I86__) || defined(_M_IX86)
C2_NAKED
#endif
// FUNCTION: CARMA2_HW 0x0054009c
// STUB: CARMA2_SW 0x0053b73c
void C2_HOOK_CDECL HostRealBlockWrite(br_uint_16 offset, br_uint_16 seg, void *block, br_uint_32 count) {
#if defined(__I86__) || defined(_M_IX86)
#if defined(_MSC_VER)
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
    __asm__(
        "push %ebp\n"
        "mov %esp, %ebp\n"
        "push %es\n"
        "push %esi\n"
        "push %edi\n"
        "mov __RealSelector, %es\n"
        "mov 0xc(%ebp), %edi\n"
        "shl $4, %edi\n"
        "add 8(%ebp), %edi\n"
        "mov 0x10(%ebp), %esi\n"
        "mov 0x14(%ebp), %ecx\n"
        "shr $1, %ecx\n"
        "rep movsw\n"
        "adc %ecx, %ecx\n"
        "rep movsb\n"
        "pop %edi\n"
        "pop %esi\n"
        "pop %es\n"
        "leave\n"
        "ret\n"
    );
#endif
#else
    abort();
#endif
}

#if defined(__I86__) || defined(_M_IX86)
C2_NAKED
#endif
// FUNCTION: CARMA2_HW 0x005400c6
// STUB: CARMA2_SW 0x0053b766
void C2_HOOK_CDECL HostRealBlockFill(br_uint_16 offset, br_uint_16 seg, br_uint_8 value, br_uint_32 count) {
#if defined(__I86__) || defined(_M_IX86)
#if defined(_MSC_VER)
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
    __asm__(
        "push %ebp\n"
        "mov %esp, %ebp\n"
        "push %es\n"
        "push %esi\n"
        "push %edi\n"
        "mov __RealSelector, %es\n"
        "mov 0xc(%ebp), %edi\n"
        "shl $4, %edi\n"
        "add 8(%ebp), %edi\n"
        "mov 0x10(%ebp), %al\n"
        "mov 0x14(%ebp), %ecx\n"
        "mov %al, %ah\n"
        "shr $1, %ecx\n"
        "rep stosw\n"
        "adc %ecx, %ecx\n"
        "rep stosb\n"
        "pop %edi\n"
        "pop %esi\n"
        "pop %es\n"
        "leave\n"
        "ret\n"
    );
#endif
#else
    abort();
#endif
}

#if defined(__I86__) || defined(_M_IX86)
C2_NAKED
#endif
// FUNCTION: CARMA2_HW 0x005400f2
// STUB: CARMA2_SW 0x0053b792
void C2_HOOK_CDECL HostRealBlockRead(br_uint_16 offset, br_uint_16 seg, void *block, br_uint_32 count) {
#if defined(__I86__) || defined(_M_IX86)
#if defined(_MSC_VER)
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
    __asm__(
        "push %ebp\n"
        "mov %esp, %ebp\n"
        "push %ds\n"
        "push %esi\n"
        "push %edi\n"
        "mov __RealSelector, %ds\n"
        "mov 0xc(%ebp), %esi\n"
        "shl $4, %esi\n"
        "add 8(%ebp), %esi\n"
        "mov 0x10(%ebp), %edi\n"
        "mov 0x14(%ebp), %ecx\n"
        "shr $1, %ecx\n"
        "rep movsw\n"
        "adc %ecx, %ecx\n"
        "rep movsb\n"
        "pop %edi\n"
        "pop %esi\n"
        "pop %ds\n"
        "leave\n"
        "ret\n"
    );
#endif
#else
    abort();
#endif
}

#if defined(__I86__) || defined(_M_IX86)
C2_NAKED
#endif
// FUNCTION: CARMA2_HW 0x0054011c
// STUB: CARMA2_SW 0x0053b7bc
br_uint_32 C2_HOOK_CDECL HostRealStringWrite(br_uint_16 offset, br_uint_16 seg, br_uint_8 *string, br_uint_32 max) {
#if defined(__I86__) || defined(_M_IX86)
#if defined(_MSC_VER)
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
    __asm__(
        "push %ebp\n"
        "mov %esp, %ebp\n"
        "push %es\n"
        "push %esi\n"
        "push %edi\n"
        "mov __RealSelector, %es\n"
        "mov 0xc(%ebp), %edi\n"
        "shl $4, %edi\n"
        "add 8(%ebp), %edi\n"
        "mov 0x10(%ebp), %esi\n"
        "mov 0x14(%ebp), %ecx\n"
        "xor %eax, %eax\n"
        "and %ecx, %ecx\n"
        "je 1f\n"
        "0:\n"
        "mov (%esi), %dl\n"
        "mov %dl, %es:(%edi)\n"
        "inc %eax\n"
        "and %dl, %dl\n"
        "je 1f\n"
        "inc %esi\n"
        "inc %edi\n"
        "dec %ecx\n"
        "jne 0b\n"
        "movb $0, %es:-1(%edi)\n"
        "1:\n"
        "pop %edi\n"
        "pop %esi\n"
        "pop %es\n"
        "leave\n"
        "ret\n"
    );
#endif
#else
    abort();
#endif
}

#if defined(__I86__) || defined(_M_IX86)
C2_NAKED
#endif
// FUNCTION: CARMA2_HW 0x00540157
// STUB: CARMA2_SW 0x0053b7f7
br_uint_32 C2_HOOK_CDECL HostRealStringRead(br_uint_16 offset, br_uint_16 seg, br_uint_8 *string, br_uint_32 max) {
#if defined(__I86__) || defined(_M_IX86)
#if defined(_MSC_VER)
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
    __asm__(
        "push %ebp\n"
        "mov %esp, %ebp\n"
        "push %es\n"
        "push %esi\n"
        "push %edi\n"
        "mov __RealSelector, %es\n"
        "mov 0xc(%ebp), %edi\n"
        "shl $4, %edi\n"
        "add 8(%ebp), %edi\n"
        "mov 0x10(%ebp), %esi\n"
        "mov 0x14(%ebp), %ecx\n"
        "xor %eax, %eax\n"
        "and %ecx, %ecx\n"
        "je 1f\n"
        "0:\n"
        "mov %es:(%edi), %dl\n"
        "mov %dl, (%esi)\n"
        "inc %eax\n"
        "and %dl, %dl\n"
        "je 1f\n"
        "inc %esi\n"
        "inc %edi\n"
        "dec %ecx\n"
        "jne 0b\n"
        "movb $0, -1(%esi)\n"
        "1:\n"
        "pop %edi\n"
        "pop %esi\n"
        "pop %es\n"
        "leave\n"
        "ret\n"
    );
#endif
#else
    abort();
#endif
}

#if defined(__I86__) || defined(_M_IX86)
C2_NAKED
#endif
// FUNCTION: CARMA2_HW 0x00540191
// STUB: CARMA2_SW 0x0053b831
void C2_HOOK_CDECL HostRealByteWrite(br_uint_16 offset, br_uint_16 seg, br_uint_8 value) {
#if defined(__I86__) || defined(_M_IX86)
#if defined(_MSC_VER)
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
    __asm__(
        "push %ebp\n"
        "mov %esp, %ebp\n"
        "push %ds\n"
        "mov __RealSelector, %ds\n"
        "mov 0xc(%ebp), %edx\n"
        "shl $4, %edx\n"
        "add 8(%ebp), %edx\n"
        "mov 0x10(%ebp), %al\n"
        "mov %al, (%edx)\n"
        "pop %ds\n"
        "leave\n"
        "ret\n"
    );
#endif
#else
    abort();
#endif
}

#if defined(__I86__) || defined(_M_IX86)
C2_NAKED
#endif
// FUNCTION: CARMA2_HW 0x005401ad
// STUB: CARMA2_SW 0x0053b84d
br_uint_8 C2_HOOK_CDECL HostRealByteRead(br_uint_16 offset, br_uint_16 seg) {
#if defined(__I86__) || defined(_M_IX86)
#if defined(_MSC_VER)
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
    __asm__(
        "push %ebp\n"
        "mov %esp, %ebp\n"
        "push %ds\n"
        "mov __RealSelector, %ds\n"
        "mov 0xc(%ebp), %eax\n"
        "shl $4, %eax\n"
        "add 8(%ebp), %eax\n"
        "mov (%eax), %al\n"
        "pop %ds\n"
        "leave\n"
        "ret\n"
    );
#endif
#else
    abort();
#endif
}

// STUB: CARMA2_HW 0x005401c6
// STUB: CARMA2_SW 0x0053b866
void C2_HOOK_CDECL HostRealWordWrite(br_uint_16 offset, br_uint_16 seg, br_uint_16 value) {
    abort();
}

// STUB: CARMA2_HW 0x005401e4
// STUB: CARMA2_SW 0x0053b884
br_uint_16 C2_HOOK_CDECL HostRealWordRead(br_uint_16 offset, br_uint_16 seg) {
    abort();
}

// STUB: CARMA2_HW 0x005401fe
// STUB: CARMA2_SW 0x0053b89e
void C2_HOOK_CDECL HostRealDWordWrite(br_uint_16 offset, br_uint_16 seg, br_uint_32 value) {
    abort();
}

// STUB: CARMA2_HW 0x0054021a
// STUB: CARMA2_SW 0x0053b8ba
br_uint_32 C2_HOOK_CDECL HostRealDWordRead(br_uint_16 offset, br_uint_16 seg) {
    abort();
}
