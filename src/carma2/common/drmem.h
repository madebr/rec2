#ifndef REC3_DRMEM_H
#define REC3_DRMEM_H

#include "brender/br_types.h"

#include "c2_hooks.h"

//extern br_allocator gAllocator;
//extern int gNon_fatal_allocation_errors;
//extern char* gMem_names[247];
//extern br_resource_class gStainless_classes[118];
//
//void SetNonFatalAllocationErrors();
//
//void ResetNonFatalAllocationErrors();
//
//int AllocationErrorsAreFatal();
//
//void MAMSInitMem();
//
void C2_HOOK_FASTCALL PrintMemoryDump(int pFlags, char* pTitle);

void* C2_HOOK_CDECL DRStdlibAllocate(br_size_t size, br_uint_8 type);

void C2_HOOK_CDECL DRStdlibFree(void* mem);
//
//br_size_t DRStdlibInquire(br_uint_8 type);
//
//br_uint_32 Claim4ByteAlignment(br_uint_8 type);
//
void C2_HOOK_FASTCALL InstallDRMemCalls(void);
//
//void MAMSUnlock(void** pPtr);
//
//void MAMSLock(void** pPtr);
//
void C2_HOOK_FASTCALL CreateStainlessClasses(void);
//
//void CheckMemory();

#endif // REC3_DRMEM_H
