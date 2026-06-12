#ifndef REC2_FWSETUP_H
#define REC2_FWSETUP_H

#include "c2_hooks.h"

#include "brender/br_types.h"

extern br_framework_state fw;

extern br_error C2_HOOK_CDECL BrFwBegin(void);

extern br_error C2_HOOK_CDECL BrFwEnd(void);

extern br_diaghandler* C2_HOOK_CDECL BrDiagHandlerSet(br_diaghandler* newdh);

extern br_filesystem* C2_HOOK_CDECL BrFilesystemSet(br_filesystem* newfs);

extern br_allocator* C2_HOOK_CDECL BrAllocatorSet(br_allocator* newal);

#endif // REC2_FWSETUP_H
