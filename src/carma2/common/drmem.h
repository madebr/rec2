#ifndef REC3_DRMEM_H
#define REC3_DRMEM_H

#include "brender/br_types.h"

#include "c2_hooks.h"

extern br_allocator gAllocator;

extern int gNon_fatal_allocation_errors;
extern const char* gMem_names[256];
extern br_resource_class gStainless_classes[126];

void C2_HOOK_FASTCALL SetNonFatalAllocationErrors(void);

void C2_HOOK_FASTCALL ResetNonFatalAllocationErrors(void);

int C2_HOOK_FASTCALL AllocationErrorsAreFatal(void);

void C2_HOOK_FASTCALL MAMSInitMem(void);

void C2_HOOK_FASTCALL CloseGlobalPackedFile(void);

void C2_HOOK_FASTCALL PrintMemoryDump(int pFlags, char* pTitle);

void* C2_HOOK_CDECL DRStdlibAllocate(br_size_t size, br_uint_8 type);

void C2_HOOK_CDECL DRStdlibFree(void* mem);

br_size_t C2_HOOK_CDECL DRStdlibInquire(br_uint_8 type);

br_uint_32 C2_HOOK_CDECL Claim4ByteAlignment(br_uint_8 type);

void C2_HOOK_FASTCALL InstallDRMemCalls(void);

void C2_HOOK_FASTCALL CreateStainlessClasses(void);

void C2_HOOK_FASTCALL CheckMemory(void);

#endif // REC3_DRMEM_H