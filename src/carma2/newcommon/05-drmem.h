#ifndef GUARD_05_DRMEM_H
#define GUARD_05_DRMEM_H

#include "c2_hooks.h"

// SetNonFatalAllocationErrors

// ResetNonFatalAllocationErrors

extern void C2_HOOK_FASTCALL MAMSInitMem(void);

extern void C2_HOOK_FASTCALL PrintMemoryDump(int pFlags, char* pTitle);

// DRStdlibAllocate

// DRStdlibFree

// DRStdlibInquire

// Claim4ByteAlignment

extern void C2_HOOK_FASTCALL InstallDRMemCalls(void);

// MAMSUnlock

// MAMSLock

extern void C2_HOOK_FASTCALL CreateStainlessClasses(void);

// CheckMemory

#endif // GUARD_05_DRMEM_H
