#ifndef GUARD_05_DRMEM_H
#define GUARD_05_DRMEM_H

#include "c2_hooks.h"
#include "brender/brender.h"

// SetNonFatalAllocationErrors

// ResetNonFatalAllocationErrors

extern void C2_HOOK_FASTCALL MAMSInitMem(void);

extern void C2_HOOK_FASTCALL PrintMemoryDump(int pFlags, char* pTitle);

extern void* C2_HOOK_CDECL DRStdlibAllocate(br_size_t size, br_uint_8 type);

extern void C2_HOOK_CDECL DRStdlibFree(void* mem);

extern br_size_t C2_HOOK_CDECL DRStdlibInquire(br_uint_8 type);

extern br_uint_32 C2_HOOK_CDECL Claim4ByteAlignment(br_uint_8 type);

extern void C2_HOOK_FASTCALL InstallDRMemCalls(void);

// MAMSUnlock

// MAMSLock

extern void C2_HOOK_FASTCALL CreateStainlessClasses(void);

// CheckMemory

#endif // GUARD_05_DRMEM_H
