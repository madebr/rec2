#ifndef REC2_HOSTCFG_H
#define REC2_HOSTCFG_H

#include "brender/br_types.h"

#include "c2_hooks.h"

const char* C2_HOOK_CDECL HostDefaultDevice(void);

br_boolean C2_HOOK_CDECL HostIniSectionExists(char* ini_file, char* section_name);

br_error HostIniQuery(char* ini_file, char* section_name, char* entry, char* Buffer, int max, br_uint_16* size);

br_error HostRegistryQuery(void* hKey, char* Path, char* entry, char* Buffer, br_uint_16 max, br_uint_16* size);

#endif //REC2_HOSTCFG_H
