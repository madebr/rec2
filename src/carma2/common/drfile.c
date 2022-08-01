#include "drfile.h"

#include "loading.h"

#include "brender/brender.h"

#include "rec2_types.h"

#include "c2_stdlib.h"

#include <string.h>

C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_filesystem, gFilesystem, 0x005933a8, {   \
    "Carmageddon",                                                          \
    NULL,                                                                   \
    &DRStdioOpenRead,                                                       \
    &DRStdioOpenWrite,                                                      \
    &DRStdioClose,                                                          \
    &DRStdioEOF,                                                            \
    &DRStdioGetChr,                                                         \
    NULL,                                                                   \
    &DRStdioRead,                                                           \
    &DRStdioWrite,                                                          \
    &DRStdioGetLine,                                                        \
    NULL,                                                                   \
    NULL,                                                                   \
});
C2_HOOK_VARIABLE_IMPLEMENT(br_filesystem*, gOld_file_system, 0x006815c4);

void* C2_HOOK_CDECL DRStdioOpenRead(const char* name, br_size_t n_magics, br_mode_test_cbfn* identify, int* mode_result) {
    if (mode_result != NULL) {
        *mode_result = 0;
    }
    return DRfopen(name, "rb");
}
C2_HOOK_FUNCTION(0x0044c700, DRStdioOpenRead)

void* C2_HOOK_CDECL DRStdioOpenWrite(const char* name, int mode) {
    return C2V(gOld_file_system)->open_write(name, mode);
}
C2_HOOK_FUNCTION(0x0044c720, DRStdioOpenWrite)

void C2_HOOK_CDECL DRStdioClose(void* f) {
    DRfclose(f);
}
C2_HOOK_FUNCTION(0x0044c740, DRStdioClose)

int C2_HOOK_CDECL DRStdioEOF(void* f) {
    return DRfeof(f);
}
C2_HOOK_FUNCTION(0x0044c6a0, DRStdioEOF)

int C2_HOOK_CDECL DRStdioGetChr(void* f) {
    return DRfgetc(f);
}
C2_HOOK_FUNCTION(0x0044c6b0, DRStdioGetChr)

br_size_t C2_HOOK_CDECL DRStdioRead(void* buf, br_size_t size, unsigned int n, void* f) {
    return DRfread(buf, size, n, f);
}
C2_HOOK_FUNCTION(0x0044c750, DRStdioRead)

br_size_t C2_HOOK_CDECL DRStdioWrite(void* buf, br_size_t size, unsigned int n, void* f) {
    return DRfwrite(buf, size, n, f);
}
C2_HOOK_FUNCTION(0x0044c770, DRStdioWrite)

br_size_t C2_HOOK_CDECL DRStdioGetLine(char* buf, br_size_t buf_len, void* f) {
    DRfgets(buf, buf_len, f);
    return strlen(buf);
}
C2_HOOK_FUNCTION(0x0044c6c0, DRStdioGetLine)

void C2_HOOK_FASTCALL InstallDRFileCalls(void) {
    br_filesystem* temp_system;

    temp_system = BrMemAllocate(sizeof(br_filesystem), kMem_misc);
    C2V(gOld_file_system) = BrFilesystemSet(temp_system);
    C2V(gFilesystem).attributes = C2V(gOld_file_system)->attributes;
    C2V(gFilesystem).putchr = C2V(gOld_file_system)->putchr;
    C2V(gFilesystem).putline = C2V(gOld_file_system)->putline;
    BrFilesystemSet(&C2V(gFilesystem));
    BrMemFree(temp_system);
}
C2_HOOK_FUNCTION(0x0044c790, InstallDRFileCalls)
