#ifndef REC2_HSTSETUP_H
#define REC2_HSTSETUP_H

#include "c2_hooks.h"

#include "brender/br_types.h"

void C2_HOOK_CDECL HostBegin(void);

void C2_HOOK_CDECL HostEnd(void);

br_error HostInfo(host_info* buffer, br_size_t buffersize);

#endif // REC2_HSTSETUP_H
