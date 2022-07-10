#ifndef REC2_FWSETUP_H
#define REC2_FWSETUP_H

#include "c2_hooks.h"

#include "brender/br_types.h"

C2_HOOK_VARIABLE_DECLARE(br_framework_state, fw);

br_error C2_HOOK_CDECL BrFwBegin(void);

br_error C2_HOOK_CDECL BrFwEnd(void);

br_diaghandler* C2_HOOK_CDECL BrDiagHandlerSet(br_diaghandler* newdh);

br_filesystem* C2_HOOK_CDECL BrFilesystemSet(br_filesystem* newfs);

br_allocator* C2_HOOK_CDECL BrAllocatorSet(br_allocator* newal);

#endif // REC2_FWSETUP_H
