#include "prot.h"

#include "c2_stdlib.h"

#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x0053fd90
__declspec(naked) br_error C2_HOOK_CDECL HostInterruptGet(br_uint_8 vector, br_uint_32 *offp, br_uint_16 *selp) {

    __asm {
        push      ebp
        mov       ebp, esp
        push      ebx
        push      es
        xor       ecx, ecx
        mov       cl, byte ptr [vector]
        mov       eax, 0x2502
        int       0x21
        mov       eax,dword ptr [selp]
        mov       word ptr [eax],es
        mov       eax, dword ptr [offp]
        mov       dword ptr [eax],ebx
        xor       eax, eax
        pop       es
        pop       ebx
        leave
        ret

    }
}
#else
br_error C2_HOOK_CDECL HostInterruptGet(br_uint_8 vector, br_uint_32 *offp, br_uint_16 *selp) {

    // Not supported on Windows
    NOT_IMPLEMENTED();
    return 0;
}
#endif

#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x0053fdb1
__declspec(naked) br_error C2_HOOK_CDECL HostInterruptSet(br_uint_8 vector, br_uint_32 off, br_uint_16 sel) {

    __asm {
        push        ebp
        mov         ebp, esp
        push        ebx
        push        ds
        mov         cl, byte ptr [vector]
        mov         edx, dword ptr [off]
        mov         ds, word ptr [sel]
        mov         eax, 0x2504
        int         0x21
        xor         eax, eax
        pop         ds
        pop         ebx
        leave
        ret
    }
}
#else
br_error C2_HOOK_CDECL HostInterruptSet(br_uint_8 vector, br_uint_32 off, br_uint_16 sel) {

    // Not supported on Windows
    NOT_IMPLEMENTED();
    return 0;
}
#endif

#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x0053fdcd
__declspec(naked) br_error C2_HOOK_CDECL HostExceptionGet(br_uint_8 exception, br_uint_32 *offp, br_uint_16 *selp) {

    __asm {
        push        ebp
        mov         ebp, esp
        push        ebx
        xor         ecx, ecx
        mov         cl, byte ptr [exception]
        mov         eax, 0x2532
        int         0x21
        mov         eax, dword ptr [selp]
        mov         word ptr [eax],es
        mov         eax, dword ptr [offp]
        mov         dword ptr [ebx],eax
        xor         eax,eax
        pop         ebx
        leave
        ret
    }
}
#else
br_error C2_HOOK_CDECL HostExceptionGet(br_uint_8 exception, br_uint_32 *offp, br_uint_16 *selp) {

    // Not supported on Windows
    NOT_IMPLEMENTED();
    return 0;
}
#endif

#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x0053fdec
__declspec(naked) br_error C2_HOOK_CDECL HostExceptionSet(br_uint_8 exception, br_uint_32 off, br_uint_16 sel) {

    __asm {
        push      ebp
        mov       ebp, esp
        push      ebx
        mov       cl, byte ptr [exception]
        mov       edx,dword ptr [off]
        mov       ds,word ptr [sel]
        mov       eax, 0x2533
        int       0x21
        xor       eax, eax
        pop       ebx
        leave
        ret
    }
}
#else
br_error C2_HOOK_CDECL HostExceptionSet(br_uint_8 exception, br_uint_32 off, br_uint_16 sel) {

    // Not supported on Windows
    NOT_IMPLEMENTED();
    return 0;
}
#endif

#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x0053fe06
__declspec(naked) br_error C2_HOOK_CDECL HostInterruptCall(br_uint_32 vector, union host_regs *regs) {
    __asm {
        push        ebp
        mov         ebp, esp
        push        ebx
        push        esi
        push        edi
        push        es
        push        fs
        push        gs
        sub         esp, 0x10
        mov         dword ptr [esp + 0], 0xc300cd
        mov         al, byte ptr [vector]
        mov         byte ptr [esp + 0x1],al
        mov         edi, dword ptr [regs]
        mov         word ptr [esp + 0x6],ds
        mov         dword ptr [esp + 0x8],edi
        mov         dword ptr [esp + 0xc],ebp
        lea         eax, [esp]
        push        return_here
        push        eax
        mov         es, word ptr [edi + 0x24]
        push        es
        mov         es, word ptr [edi + 0x22]
        mov         fs, word ptr [edi + 0x26]
        mov         gs, word ptr [edi + 0x28]
        mov         eax, dword ptr [edi + 0x1c]
        mov         ebx, dword ptr [edi + 0x10]
        mov         ecx, dword ptr [edi + 0x18]
        mov         edx, dword ptr [edi + 0x14]
        mov         ebp, dword ptr [edi + 0x8]
        mov         esi, dword ptr [edi + 0x4]
        mov         edi, dword ptr [edi]
        pop         ds
        ret
return_here:
        mov         word ptr [esp + 0x4],ds
        mov         ds, word ptr [esp + 0x6]
        xchg        dword ptr [esp + 0xc], ebp
        xchg        dword ptr [esp + 0x8], edi
        pushf
        mov         dword ptr [edi + 0x1c], eax
        mov         dword ptr [edi + 0x10], ebx
        mov         dword ptr [edi + 0x18], ecx
        mov         dword ptr [edi + 0x14], edx
        mov         dword ptr [edi + 0x4], esi
        pop         ax
        mov         word ptr [edi + 0x20], ax
        mov         ax, word ptr [esp + 0x4]
        mov         word ptr [edi + 0x24], ax
        mov         word ptr [edi + 0x22], es
        mov         word ptr [edi + 0x26], fs
        mov         word ptr [edi + 0x28], gs
        mov         eax, dword ptr [esp + 0xc]
        mov         dword ptr [edi + 0x8], eax
        mov         eax, dword ptr [esp + 0x8]
        mov         dword ptr [edi],eax
        add         esp, 0x10
        xor         eax, eax
        pop         gs
        pop         fs
        pop         es
        pop         edi
        pop         esi
        pop         ebx
        leave
        ret
    }
}
#else
br_error C2_HOOK_CDECL HostInterruptCall(br_uint_32 vector, union host_regs *regs) {

    // Not supported on Windows
    NOT_IMPLEMENTED();
    return 0;
}
#endif

#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x0053febc
__declspec(naked) br_error C2_HOOK_CDECL HostRegistersGet(union host_regs *regs) {

    __asm {
        push              edi
        mov               edi,dword ptr [esp + 12]
        pushf
        pop               word ptr [edi + 0x20]
        mov               dword ptr [edi + 0x1c], eax
        mov               dword ptr [edi + 0x10], ebx
        mov               dword ptr [edi + 0x18], ecx
        mov               dword ptr [edi + 0x14], edx
        mov               dword ptr [edi + 0x8], ebp
        mov               dword ptr [edi + 0x4], esi
        mov               word ptr [edi + 0x24], ds
        mov               word ptr [edi + 0x22], es
        mov               word ptr [edi + 0x26], fs
        mov               word ptr [edi + 0x28], gs
        mov               word ptr [edi + 0x30], ss
        mov               word ptr [edi + 0x2c], cs
        mov               eax, dword ptr [esp + 8]
        mov               dword ptr [edi], eax
        add               esp, 0x4
        xor               eax, eax
        ret
    }
}
#else
br_error C2_HOOK_CDECL HostRegistersGet(union host_regs *regs) {

    // Not supported on Windows
    NOT_IMPLEMENTED();
    return 0;
}
#endif

#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x0053fefd
__declspec(naked) br_error C2_HOOK_CDECL HostSelectorDS(br_uint_16 *selp) {

    __asm {
        push    ebp
        mov     ebp, esp
        mov     eax, dword ptr [selp]
        mov     word ptr [eax], ds
        xor     eax, eax
        leave
        ret
    }
}
#else
br_error C2_HOOK_CDECL HostSelectorDS(br_uint_16 *selp) {

    NOT_IMPLEMENTED();
    return 0;
}
#endif

#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x0053ff0a
__declspec(naked) br_error C2_HOOK_CDECL HostSelectorCS(br_uint_16 *selp) {

    __asm {
        push    ebp
        mov     ebp, esp
        mov     eax, dword ptr [selp]
        mov     word ptr [eax], cs
        xor     eax, eax
        leave
        ret
    }
}
#else
br_error C2_HOOK_CDECL HostSelectorCS(br_uint_16 *selp) {

    NOT_IMPLEMENTED();
    return 0;
}
#endif

#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x0053ff17
__declspec(naked) br_error C2_HOOK_CDECL HostSelectorSS(br_uint_16 *selp) {

    __asm {
        push              ebp
        mov               ebp, esp
        mov               eax, dword ptr [selp]
        mov               word ptr [eax],ss
        xor               eax, eax
        leave
        ret
    }
}
#else
br_error C2_HOOK_CDECL HostSelectorSS(br_uint_16 *selp) {

    NOT_IMPLEMENTED();
    return 0;
}
#endif

#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x0053ff24
__declspec(naked) br_error C2_HOOK_CDECL HostSelectorES(br_uint_16 *selp) {

    __asm {
        push    ebp
        mov     ebp, esp
        mov     eax, dword ptr [selp]
        mov     word ptr [eax], es
        xor     eax, eax
        leave
        ret
    }
}
#else
br_error C2_HOOK_CDECL HostSelectorES(br_uint_16 *selp) {

    NOT_IMPLEMENTED();
    return 0;
}
#endif
