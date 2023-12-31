#ifndef _SYS_CONF_H_
#define _SYS_CONF_H_

#include "brender/br_types.h"

C2_HOOK_VARIABLE_DECLARE_ARRAY(br_token, valid_system_config_tokens, 5);

br_boolean C2_HOOK_STDCALL Is_Valid_Sys_Config_Token(br_token t);

br_boolean C2_HOOK_STDCALL LoadIniEntry(char* ini_file, char* section_name, br_token t, char* Entry);

#if 0
br_error LoadIniConfig(char* ini_file, char* section_name);
#endif

br_boolean C2_HOOK_STDCALL LoadRegistryEntry(char* Reg_Path, void* hKey, br_token t, char* Entry);

#if 0
br_error C2_HOOK_STDCALL LoadRegistryConfig(char* Reg_Path, void* hKey);
#endif

br_error C2_HOOK_CDECL BrSetDefaultConfig(br_token t, char* Entry);

br_error C2_HOOK_CDECL BrSystemConfigBegin(void);

br_error C2_HOOK_CDECL BrSystemConfigLoad(br_token t, char* Param1, void* Param2);

br_error C2_HOOK_CDECL BrSystemConfigSetString(br_token t, char* string);

br_error C2_HOOK_CDECL BrSystemConfigQueryString(br_token t, char* string, int max_size);

#endif
