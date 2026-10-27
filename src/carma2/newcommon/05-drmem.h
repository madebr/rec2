/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_05_DRMEM_H
#define GUARD_05_DRMEM_H

#include "c2_hooks.h"
#include "brender/brender.h"

extern void C2_HOOK_FASTCALL SetNonFatalAllocationErrors(void);

extern void C2_HOOK_FASTCALL ResetNonFatalAllocationErrors(void);

extern int C2_HOOK_FASTCALL AllocationErrorsAreFatal(void);

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

extern void C2_HOOK_FASTCALL CheckMemory(void);

#endif // GUARD_05_DRMEM_H
