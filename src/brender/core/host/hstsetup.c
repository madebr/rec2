#include "hstsetup.h"

#include "core/fw/image.h"

//C2_HOOK_VARIABLE_IMPLEMENT(host_info, hostInfo, xxx);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(void*, functionPointers_BRHOST1, 50, 0x0066ffa0, xxx);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_image, Image_BRHOST1, 0x00670068, xxx);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(char*, namePointers_BRHOST1, 50, 0x0066fe70, xxx);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_uint_16, nameOrdinals_BRHOST1, 50, 0x0066ff38, xxx);

void (C2_HOOK_CDECL * HostBegin_original)(void);
void C2_HOOK_CDECL HostBegin(void) {
#if defined(C2_HOOKS_ENABLED)
    HostBegin_original();
#else
#if defined(__I86__)
    RealSelectorBegin();
#endif

    BrImageAdd(&C2V(Image_BRHOST1));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053fab0, HostBegin, HostBegin_original)

void C2_HOOK_CDECL HostEnd(void) {
    BrImageRemove(&C2V(Image_BRHOST1));

#if defined(__I86__)
    RealSelectorEnd();
#endif
}
C2_HOOK_FUNCTION(0x0053fad0, HostEnd)

// IDA: br_error __cdecl HostInfo(host_info *buffer, br_size_t buffersize)
br_error (C2_HOOK_CDECL * HostInfo_original)(host_info* buffer, br_size_t buffersize);
br_error C2_HOOK_CDECL HostInfo(host_info* buffer, br_size_t buffersize) {
#if defined(C2_HOOKS_ENABLED)
    return HostInfo_original(buffer, buffersize);
#else
    br_int_32 features;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053faf0, HostInfo, HostInfo_original)
