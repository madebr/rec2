#include "real.h"

#include "c2_stdlib.h"

// GLOBAL: CARMA2_HW 0x0067052a
br_uint_16 _RealSelector;

#ifdef REC2_MATCHING
#pragma pack(push, 2)

// GLOBAL: CARMA2_HW 0x00670518
// pl_registers

struct {
    br_uint_16 interrupt;
    br_uint_16 rds;
    br_uint_16 res;
    br_uint_16 rfs;
    br_uint_16 rgs;
    br_uint_32 reax;
    br_uint_32 redx;
} pl_registers;
#pragma pack(pop)

#endif


// FUNCTION: CARMA2_HW 0x00540086
br_error C2_HOOK_CDECL HostSelectorReal(br_uint_16 *selp) {

    *selp = _RealSelector;
    return 0;
}

#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x0053ff34
__declspec(naked) br_error C2_HOOK_CDECL HostRealAllocate(struct host_real_memory *mem, br_uint_32 size_arg) {

    __asm {
        push    ebp
        mov     ebp, esp
        push    ebx
        push    ecx
        mov     ebx, size_arg
        add     ebx, 0xf
        shr     ebx, 0x4
        xor     eax, eax
        pop     ecx
        pop     ebx
        leave
        ret
    }
}
#else
br_error C2_HOOK_CDECL HostRealAllocate(struct host_real_memory *mem, br_uint_32 size_arg) {

    return 0;
}
#endif

#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x0053ff51
__declspec(naked) br_error C2_HOOK_CDECL HostRealFree(struct host_real_memory *mem) {

    __asm {
        push  ebp
        mov   ebp, esp
        push  ebx
        xor   eax, eax
        pop   ebx
        leave
        ret
    }
}
#else
br_error C2_HOOK_CDECL HostRealFree(struct host_real_memory *mem) {

    return 0;
}
#endif

#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x0053ff5a
__declspec(naked) br_error C2_HOOK_CDECL HostRealInterruptGet(br_uint_32 vector, br_uint_16 *offp, br_uint_16 *vsegp) {

    __asm {
        push  ebp
        mov   ebp,esp
        push  ebx
        xor   ecx,ecx
        mov   cl, byte ptr [vector]
        mov   eax, 0x2503
        int   0x21
        mov   eax,dword ptr [offp]
        mov   word ptr [eax],bx
        shr   ebx, 0x10
        mov   eax, dword ptr [vsegp]
        mov   word ptr [eax], bx
        xor   eax, eax
        pop   ebx
        leave
        ret
    }
}
#else
br_error C2_HOOK_CDECL HostRealInterruptGet(br_uint_32 vector, br_uint_16 *offp, br_uint_16 *vsegp) {

    return 0x2503;
}
#endif

#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x0053ff7e
__declspec(naked) br_error C2_HOOK_CDECL HostRealInterruptSet(br_uint_32 vector, br_uint_32 voff, br_uint_32 vseg) {
    __asm {
        push    ebp
        mov     ebp,esp
        push    ebx
        mov     cl, byte ptr [vector]
        mov     bx, word ptr [vseg]
        shl     ebx, 0x10
        mov     bx, word ptr [voff]
        mov     eax, 0x2505
        int     0x21
        xor     eax,eax
        pop     ebx
        leave
        ret
    }
}
#else
br_error C2_HOOK_CDECL HostRealInterruptSet(br_uint_32 vector, br_uint_32 voff, br_uint_32 vseg) {

    return 0x2505;
}
#endif

#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x0053ff9c
__declspec(naked) br_error C2_HOOK_CDECL HostRealInterruptCall(br_uint_32 vector, union host_regs *regs) {

    __asm {
        push     ebp
        mov      ebp, esp
        push     ebx
        push     esi
        push     edi
        push     es
        mov      bx, word ptr [vector]
        mov      word ptr [pl_registers],bx
        push     ebp
        mov      edx, dword ptr [regs]
        mov      edi, dword ptr [edx]
        mov      esi, dword ptr [edx + 0x4]
        mov      ebp, dword ptr [edx + 0x8]
        mov      ecx, dword ptr [edx + 0x18]
        mov      ebx, dword ptr [edx + 0x10]
        mov      eax, dword ptr [edx + 0x1c]
        mov      [pl_registers.reax], eax
        mov      eax, dword ptr [edx + 0x14]
        mov      [pl_registers.redx], eax
        mov      ax, word ptr [edx + 0x24]
        mov      [pl_registers.rds], ax
        mov      ax, word ptr [edx + 0x22]
        mov      [pl_registers.res], ax
        mov      ax, word ptr [edx + 0x26]
        mov      [pl_registers.rfs], ax
        mov      ax, word ptr [edx + 0x28]
        mov      [pl_registers.rgs], ax
        push     edx
        mov      eax,0x2511
        mov      edx, offset pl_registers
        int      0x21
        mov      [pl_registers.reax], eax
        pushf
        pop      ax
        pop      edx
        mov      word ptr [edx + 0x20], ax
        mov      dword ptr [edx],edi
        mov      dword ptr [edx + 0x4], esi
        mov      dword ptr [edx + 0x8], ebp
        mov      dword ptr [edx + 0x18], ecx
        mov      dword ptr [edx + 0x10], ebx
        mov      eax,[pl_registers.reax]
        mov      dword ptr [edx + 0x1c], eax
        mov      eax,[pl_registers.redx]
        mov      dword ptr [edx + 0x14], eax
        mov      ax,[pl_registers.rds]
        mov      word ptr [edx + 0x24], ax
        mov      ax,[pl_registers.res]
        mov      word ptr [edx + 0x22], ax
        mov      ax,[pl_registers.rfs]
        mov      word ptr [edx + 0x26], ax
        mov      ax,[pl_registers.rgs]
        mov      word ptr [edx + 0x28], ax
        pop      ebp
        xor      eax, eax
        pop      es
        pop      edi
        pop      esi
        pop      ebx
        leave
        ret
    }
}
#else
br_error C2_HOOK_CDECL HostRealInterruptCall(br_uint_32 vector, union host_regs *regs) {
    return 0x2511;
}
#endif

#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x00540062
__declspec(naked) br_error C2_HOOK_CDECL RealSelectorBegin(void) {

    __asm {
        push    ebx
        cmp     word ptr [_RealSelector], 0x0
        jz      not_active
        mov     eax, 0x1007
        pop     ebx
        ret
    not_active:
        mov     [_RealSelector], ax
        xor     eax, eax
        pop     ebx
        ret
        mov     eax, 0x1002
        pop     ebx
        ret
    }
}
#else
br_error C2_HOOK_CDECL RealSelectorBegin(void) {

    return 0x1002;
}
#endif

// FUNCTION: CARMA2_HW 0x00540085
void C2_HOOK_CDECL RealSelectorEnd(void) {

    // empty
}
