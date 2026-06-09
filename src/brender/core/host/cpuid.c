#include "cpuid.h"

#include "c2_hooks.h"

#if (defined(_M_IX86) || defined(_M_X64) || defined(__i386__) || defined(__x86_64__)) && !(defined(_MSC_VER) && _MSC_VER < 1300)
#include <mmintrin.h>
#endif
#include "c2_string.h"


#ifdef REC2_MATCHING

// GLOBAL: CARMA2_HW 0x0054053f
br_token cpu_types[] = {
    BRT_INTEL_386,
    BRT_INTEL_386,
    BRT_INTEL_386,
    BRT_INTEL_386,
    BRT_INTEL_486,
    BRT_INTEL_PENTIUM,
    BRT_INTEL_PENTIUM_PRO,
};

// FUNCTION: CARMA2_HW 0x005404b0
C2_NAKED void C2_HOOK_CDECL CPUInfo(br_token* cpu_type, br_uint_32* features) {
    __asm {
        push              ebp
        mov               ebp,esp
        push              ebx
        push              esi
        push              edi
        mov               esi, dword ptr [features]
        mov               dword ptr [esi], 0x0

        mov               esi,dword ptr [cpu_type]
        mov               dword ptr [esi], BRT_INTEL_386

        pushfd
        pop               eax
        mov               ecx, eax
        xor               eax, 0x40000
        push              eax
        popfd
        pushfd
        pop               eax
        xor               eax, ecx
        jz                quit
        push              ecx
        popfd
        mov               esi, dword ptr [cpu_type]
        mov               dword ptr [esi], BRT_INTEL_486
        mov               eax, ecx
        xor               eax, 0x200000
        push              eax
        popfd
        pushfd
        pop               eax
        xor               eax, ecx
        jz                quit
        mov               eax, 0x0

        // cpuid
        _emit 0x0f
        _emit 0xa2

        cmp               eax, 0x1
        jl                quit
        mov               eax, 0x1

        // cpuid
        _emit 0x0f
        _emit 0xa2

        shr               eax, 0x8
        and               eax, 0xf

        mov               eax, dword ptr cs:[eax * 0x4 + cpu_types]


        mov               esi,dword ptr [cpu_type]
        mov               dword ptr [esi], eax
        xor               eax, eax
        test              edx, 0x1

        jz                no_fpu

        or                eax,0x800
no_fpu:
        test              edx,0x800000

        jz                no_cmov

        or                eax, 0x400
no_cmov:
        mov               esi,dword ptr [features]
        mov               dword ptr [esi], eax
quit:
        pop               edi
        pop               esi
        pop               ebx
        leave
        ret
    }
}
#else
void C2_HOOK_CDECL CPUInfo(br_token* cpu_type, br_uint_32* features) {

    *cpu_type = BRT_INTEL_PENTIUM;
    *features = 0;
}
#endif
