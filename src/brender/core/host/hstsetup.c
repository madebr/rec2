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

// GLOBAL: CARMA2_HW 0x006700a8
host_info hostInfo = {
    sizeof(host_info),
    "Microsoft WIN32",
    0,
    BRT_INTEL,
    BRT_INTEL_386,
};

// GLOBAL: CARMA2_HW 0x0066ffa0
void* functionPointers_BRHOST1[50] = {
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
};

// GLOBAL: CARMA2_HW 0x0066fe70
const char* namePointers_BRHOST1[50] = {
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
};

// GLOBAL: CARMA2_HW 0x0066ff38
br_uint_16 nameOrdinals_BRHOST1[50] = {
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
};

// GLOBAL: CARMA2_HW 0x00670068
br_image Image_BRHOST1 = {
    { 0 },
    "BRHOST1",
    2,
    0,
    1,
    BR_ASIZE(functionPointers_BRHOST1),
    functionPointers_BRHOST1,
    BR_ASIZE(namePointers_BRHOST1),
    namePointers_BRHOST1,
    nameOrdinals_BRHOST1,
    0,
    NULL,
    0,
    NULL,
    NULL
};

// FUNCTION: CARMA2_HW 0x0053fab0
void C2_HOOK_CDECL HostBegin(void) {

    RealSelectorBegin();
    BrImageAdd(&Image_BRHOST1);
}

// FUNCTION: CARMA2_HW 0x0053fad0
void C2_HOOK_CDECL HostEnd(void) {

    BrImageRemove(&Image_BRHOST1);
    RealSelectorEnd();
}

// IDA: br_error __cdecl HostInfo(host_info *buffer, br_size_t buffersize)
// FUNCTION: CARMA2_HW 0x0053faf0
br_error C2_HOOK_CDECL HostInfo(host_info* buffer, br_size_t buffersize) {
    br_uint_32 features;

    if (buffersize < sizeof(host_info)) {
        BrMemCpy(buffer, &hostInfo, buffersize);
        return 0x1004;
    }
    BrMemCpy(buffer, &hostInfo, sizeof(host_info));
    CPUInfo(&buffer->processor_type, &features);
    buffer->capabilities |= features;
    return 0;
}
