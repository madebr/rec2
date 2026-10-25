/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef _SYS_CONF_H_
#define _SYS_CONF_H_

#include "brender/br_types.h"

extern const br_token valid_system_config_tokens[5];

br_boolean C2_HOOK_STDCALL Is_Valid_Sys_Config_Token(br_token t);

br_boolean C2_HOOK_STDCALL LoadIniEntry(char* ini_file, char* section_name, br_token t, char* Entry);

br_error C2_HOOK_STDCALL LoadIniConfig(char* ini_file, char* section_name);

br_boolean C2_HOOK_STDCALL LoadRegistryEntry(char* Reg_Path, void* hKey, br_token t, char* Entry);

br_error C2_HOOK_STDCALL LoadRegistryConfig(char* Reg_Path, void* hKey);

br_error C2_HOOK_STDCALL BrSetDefaultConfig(br_token t, char* Entry);

br_error C2_HOOK_CDECL BrSystemConfigBegin(void);

br_error C2_HOOK_CDECL BrSystemConfigLoad(br_token t, char* Param1, void* Param2);

br_error C2_HOOK_CDECL BrSystemConfigSetString(br_token t, const char* string);

br_error C2_HOOK_CDECL BrSystemConfigQueryString(br_token t, char* string, int max_size);

#endif
