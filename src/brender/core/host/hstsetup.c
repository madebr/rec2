#include "hstsetup.h"

#include "core/host/cpuid.h"
#include "core/host/farmem.h"
#include "core/host/hook.h"
#include "core/host/memmgmt.h"
#include "core/host/prot.h"
#include "core/host/real.h"
#include "core/host/realmem.h"
#include "core/std/brstdlib.h"
#include "core/fw/image.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(host_info, hostInfo, 0x005d7268, {
    sizeof(host_info),
    "Microsoft WIN32",
    0,
    BRT_INTEL,
    BRT_INTEL_386,
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(void*, functionPointers_BRHOST1, 50, 0x0066ffa0, {
    HostExceptionHook,
    HostExceptionUnhook,
    HostFarBlockFill,
    HostFarBlockRead,
    HostFarBlockWrite,
    HostFarByteRead,
    HostFarByteWrite,
    HostFarDWordRead,
    HostFarDWordWrite,
    HostFarStringRead,
    HostFarStringWrite,
    HostFarWordRead,
    HostFarWordWrite,
    HostInfo,
    HostInterruptCall,
    HostInterruptHook,
    HostInterruptUnhook,
    HostLock,
    HostRealAllocate,
    HostRealBlockFill,
    HostRealBlockRead,
    HostRealBlockWrite,
    HostRealByteRead,
    HostRealByteWrite,
    HostRealDWordRead,
    HostRealDWordWrite,
    HostRealFree,
    HostRealInterruptCall,
    HostRealInterruptGet,
    HostRealInterruptSet,
    HostRealStringRead,
    HostRealStringWrite,
    HostRealWordRead,
    HostRealWordWrite,
    HostRegistersGet,
    HostSelectorAllocate,
    HostSelectorAllocateAlias,
    HostSelectorAllocateLinear,
    HostSelectorAllocatePhysical,
    HostSelectorBaseQuery,
    HostSelectorBaseSet,
    HostSelectorCS,
    HostSelectorDS,
    HostSelectorES,
    HostSelectorFree,
    HostSelectorLimitQuery,
    HostSelectorLimitSet,
    HostSelectorReal,
    HostSelectorSS,
    HostUnlock,
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, namePointers_BRHOST1, 50, 0x0066fe70, {
    "_HostExceptionHook",
    "_HostExceptionUnhook",
    "_HostFarBlockFill",
    "_HostFarBlockRead",
    "_HostFarBlockWrite",
    "_HostFarByteRead",
    "_HostFarByteWrite",
    "_HostFarDWordRead",
    "_HostFarDWordWrite",
    "_HostFarStringRead",
    "_HostFarStringWrite",
    "_HostFarWordRead",
    "_HostFarWordWrite",
    "_HostInfo",
    "_HostInterruptCall",
    "_HostInterruptHook",
    "_HostInterruptUnhook",
    "_HostLock",
    "_HostRealAllocate",
    "_HostRealBlockFill",
    "_HostRealBlockRead",
    "_HostRealBlockWrite",
    "_HostRealByteRead",
    "_HostRealByteWrite",
    "_HostRealDWordRead",
    "_HostRealDWordWrite",
    "_HostRealFree",
    "_HostRealInterruptCall",
    "_HostRealInterruptGet",
    "_HostRealInterruptSet",
    "_HostRealStringRead",
    "_HostRealStringWrite",
    "_HostRealWordRead",
    "_HostRealWordWrite",
    "_HostRegistersGet",
    "_HostSelectorAllocate",
    "_HostSelectorAllocateAlias",
    "_HostSelectorAllocateLinear",
    "_HostSelectorAllocatePhysical",
    "_HostSelectorBaseQuery",
    "_HostSelectorBaseSet",
    "_HostSelectorCS",
    "_HostSelectorDS",
    "_HostSelectorES",
    "_HostSelectorFree",
    "_HostSelectorLimitQuery",
    "_HostSelectorLimitSet",
    "_HostSelectorReal",
    "_HostSelectorSS",
    "_HostUnlock",
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_uint_16, nameOrdinals_BRHOST1, 50, 0x0066ff38, {
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
    10,
    11,
    12,
    13,
    14,
    15,
    16,
    17,
    18,
    19,
    20,
    21,
    22,
    23,
    24,
    25,
    26,
    27,
    28,
    29,
    30,
    31,
    32,
    33,
    34,
    35,
    36,
    37,
    38,
    39,
    40,
    41,
    42,
    43,
    44,
    45,
    46,
    47,
    48,
    49
});
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_image, Image_BRHOST1, 0x00670068, {
    { 0 },
    "BRHOST1",
    2,
    0,
    1,
    BR_ASIZE(C2V(functionPointers_BRHOST1)),
    C2V(functionPointers_BRHOST1),
    BR_ASIZE(C2V(namePointers_BRHOST1)),
    C2V(namePointers_BRHOST1),
    C2V(nameOrdinals_BRHOST1),
    0,
    NULL,
    0,
    NULL,
    NULL
});

void (C2_HOOK_CDECL * HostBegin_original)(void);
void C2_HOOK_CDECL HostBegin(void) {
#if 0//defined(C2_HOOKS_ENABLED)
    HostBegin_original();
#else
#if defined(__I86__)
    RealSelectorBegin();
#endif

#if !defined(REC2_STANDALONE)
    BrImageAdd(&C2V(Image_BRHOST1));
#endif
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053fab0, HostBegin, HostBegin_original)

void C2_HOOK_CDECL HostEnd(void) {
#if !defined(REC2_STANDALONE)
    BrImageRemove(&C2V(Image_BRHOST1));
#endif

#if defined(__I86__)
    RealSelectorEnd();
#endif
}
C2_HOOK_FUNCTION(0x0053fad0, HostEnd)

// IDA: br_error __cdecl HostInfo(host_info *buffer, br_size_t buffersize)
br_error (C2_HOOK_CDECL * HostInfo_original)(host_info* buffer, br_size_t buffersize);
br_error C2_HOOK_CDECL HostInfo(host_info* buffer, br_size_t buffersize) {
#if 0//defined(C2_HOOKS_ENABLED)
    return HostInfo_original(buffer, buffersize);
#else
    br_uint_32 features;

    if (buffersize < sizeof(host_info)) {
        BrMemCpy(buffer, &C2V(hostInfo), buffersize);
        return 0x1004;
    }
    BrMemCpy(buffer, &C2V(hostInfo), sizeof(host_info));
    CPUInfo(&buffer->processor_type, &features);
    buffer->capabilities |= features;
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053faf0, HostInfo, HostInfo_original)
