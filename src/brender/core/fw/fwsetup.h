#ifndef REC2_FWSETUP_H
#define REC2_FWSETUP_H

#include "c2_hooks.h"

#include "brender/br_types.h"

C2_HOOK_VARIABLE_DECLARE(br_framework_state, fw);

br_filesystem* C2_HOOK_CDECL BrFilesystemSet(br_filesystem* newfs);


#endif //REC2_FWSETUP_H
