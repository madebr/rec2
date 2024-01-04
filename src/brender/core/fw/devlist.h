#ifndef REC2_DEVLIST_H
#define REC2_DEVLIST_H

#include "c2_hooks.h"

#include "brender/br_types.h"

br_error C2_HOOK_CDECL AddRequestedDrivers(void);

br_error C2_HOOK_STDCALL devAdd(br_device** pdev, br_device_begin_fn* dev_begin, char* args, br_image* image);

br_error C2_HOOK_CDECL BrDevAdd(br_device** pdev, char* image, char* args);

br_error C2_HOOK_CDECL BrDevAddStatic(br_device** pdev, br_device_begin_fn* dev_begin, char* args);

br_error C2_HOOK_CDECL BrDevCheckAdd(br_device** pdev, char* name, char* args);

br_error C2_HOOK_CDECL BrDevAddConfig(char* config);

br_error C2_HOOK_CDECL BrDevRemove(br_device* dev);

br_error C2_HOOK_CDECL BrDevFind(br_device** pdev, char* pattern);

br_error C2_HOOK_CDECL BrDevFindMany(br_device** devices, br_int_32* ndevices, br_int_32 max_devices, char* pattern);

br_error C2_HOOK_CDECL BrDevCount(br_int_32* ndevices, char* pattern);

br_error C2_HOOK_CDECL BrDevContainedFind(br_object** ph, br_token type, char* pattern, br_token_value* tv);

br_error C2_HOOK_CDECL BrDevContainedFindMany(br_object** objects, br_int_32 max_objects, br_int_32* pnum_objects, br_token type, char* pattern, br_token_value* tv);

br_error C2_HOOK_CDECL BrDevContainedCount(br_int_32* pcount, br_token type, char* pattern, br_token_value* tv);

#endif
