#ifndef REC2_UTILITY_H
#define REC2_UTILITY_H

#include "brender/br_types.h"

#include "c2_hooks.h"

#include <stdint.h>

br_error C2_HOOK_FASTCALL RemoveAllBrenderDevices(void);

int C2_HOOK_FASTCALL PDCheckDriveExists(char* pThe_path);

#endif //REC2_UTILITY_H
