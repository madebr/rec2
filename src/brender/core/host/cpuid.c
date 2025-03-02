#include "cpuid.h"

#include "c2_hooks.h"

#if defined(_M_IX86) || defined(_M_X64) || defined(__i386__) || defined(__x86_64__)
#include <mmintrin.h>
#endif
#include <string.h>

br_token cpu_types[] = {
    BRT_INTEL_386,
    BRT_INTEL_386,
    BRT_INTEL_386,
    BRT_INTEL_386,
    BRT_INTEL_486,
    BRT_INTEL_PENTIUM,
    BRT_INTEL_PENTIUM_PRO,
};

static void cpuid_wrapper(int(*values)[4], int function) {
    _Static_assert(sizeof(*values) == 16, "In total, cpuid returns 16 bytes");
    memset(*values, 0, sizeof(*values));

#ifdef _MSC_VER
#if  defined(_M_IX86) || defined(_M_X64)
    __cpuid(cpuid_res, 0);
#endif
#elif defined(__i386__)
    __asm__ __volatile__(
            "        pushl %%ebx        \n"
            "        xorl %%ecx,%%ecx   \n"
            "        cpuid              \n"
            "        movl %%ebx, %%esi  \n"
            "        popl %%ebx         \n"
            : "=a"((*values)[0]), "=b"((*values)[1]), "=c"((*values)[2]), "=d"((*values)[3])
            : "a"(0));
#elif defined(__x86_64__)
    __asm__ __volatile__(
        "        pushq %%rbx        \n"
        "        xorq %%rcx,%%rcx   \n"
        "        cpuid              \n"
        "        movq %%rbx, %%rsi  \n"
        "        popq %%rbx         \n"
        : "=a"((*cpuid_res)[0]), "=b"((*cpuid_res)[1]), "=c"((*cpuid_res)[2]), "=d"((*cpuid_res)[3])
        : "a"(0));
#endif
}

void C2_HOOK_CDECL CPUInfo(br_token* processor_type, br_uint_32* features) {
    int is_486_or_newer = 0;
    int has_cpuid = 0;
    int cpuid_res[4];
    br_uint_32 cpu_family;

    *features = 0;
    *processor_type = BRT_INTEL_386;
#ifdef _MSC_VER
    __asm {
        ; Cannot set 0x40000 on 386
        pushfd
        pop     eax
        mov     ecx, eax
        xor     eax, 40000h
        push    eax
        popfd
        pushfd
        pop     eax
        sub     eax, ecx
        mov     dword ptr[is_486_or_newer], eax
        popfd
    }
#else
    asm (
        "pushf\n\t"
        "pop        %%eax\n\t"
        "mov        %%eax, %%ecx\n\t"
        "xor        $0x40000, %%eax\n\t"
        "pushl      %%eax\n\t"
        "popf\n\t"
        "pushf\n\t"
        "popl       %%eax\n\t"
        "subl       %%ecx, %%eax\n\t"
        "popf\n\t"
        : "=a"(is_486_or_newer)
        :
        : "memory"
    );
#endif
    if (!is_486_or_newer) {
        return;
    }
    *processor_type = BRT_INTEL_486;

#ifdef _MSC_VER
    __asm {
        ; bit 21 (0x200000) of EFLAGS indicates cpuid support
        pushfd
        pop        ecx
        mov        eax, ecx
        xor        eax, 200000h
        push       eax
        popfd
        pushfd
        pop        eax
        sub        eax, ecx
        mov        dword ptr[has_cpuid], eax
        popfd
    }
#else
    asm (
        "pushf\n\t"
        "pop        %%ecx\n\t"
        "mov        %%ecx, %%eax\n\t"
        "xor        $0x200000, %%eax\n\t"
        "push       %%eax\n\t"
        "popf\n\t"
        "pushf\n\t"
        "pop        %%eax\n\t"
        "sub        %%ecx, %%eax\n\t"
        "popf\n\t"
        : "=a"(has_cpuid)
        :
        : "memory"
    );
#endif
    if (!has_cpuid) {
        return;
    }
    cpuid_wrapper(&cpuid_res, 0);
    if (cpuid_res[0] < 1) {
        return;
    }
    cpuid_wrapper(&cpuid_res, 1);
    cpu_family = (cpuid_res[0] >> 8) & 0xf;
    if (cpu_family >= BR_ASIZE(cpu_types)) {
        *processor_type = BRT_INTEL_PENTIUM_PRO;
        return;
    }
    *processor_type = cpu_types[cpu_family];
    *features = 0;
    if (cpuid_res[2] & 0x1) {
        *features |= 0x800;
    }
    if (cpuid_res[2] & 0x80000) {
        *features |= 0x400;
    }
}
