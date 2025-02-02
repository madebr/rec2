#include "sys_conf.h"

#include "core/fw/assocarr.h"
#include "core/fw/brprintf.h"
#include "core/fw/diag.h"
#include "core/fw/fwsetup.h"
#include "core/host/hostcfg.h"
#include "core/std/brstdlib.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_token, valid_system_config_tokens, 5, 0x0066c870, {
    BRT_BRENDER_PATH_STR,
    BRT_BRENDER_DRIVERS_STR,
    BRT_DEFAULT_DEVICE_STR,
    BRT_BRENDER_DEVICES_STR,
    BR_NULL_TOKEN,
});

br_boolean C2_HOOK_STDCALL Is_Valid_Sys_Config_Token(br_token t) {
    int i;

    for (i = 0; C2V(valid_system_config_tokens)[i] != BR_NULL_TOKEN; i++) {
        if (C2V(valid_system_config_tokens)[i] == t) {
            return 1;
        }
    }
    return 0;
}

br_boolean (C2_HOOK_STDCALL * LoadIniEntry_original)(char* ini_file, char* section_name, br_token t, char* Entry);
br_boolean C2_HOOK_STDCALL LoadIniEntry(char* ini_file, char* section_name, br_token t, char* Entry) {

#if 0//defined(C2_HOOKS_ENABLED)
    return LoadIniEntry_original(ini_file, section_name, t, Entry);
#else
    char Temp[255];
    br_uint_16 size;
    br_value v;
    br_error r;

    r = HostIniQuery(ini_file, section_name, Entry, Temp, sizeof(Temp), &size);
    if (r != 0) {
        return r;
    }
    v.cstr = Temp;
    BrAssociativeArraySetEntry(C2V(fw).sys_config, t, v);
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005308b0, LoadIniEntry, LoadIniEntry_original)

br_error C2_HOOK_STDCALL LoadIniConfig(char* ini_file, char* section_name) {

    if (!HostIniSectionExists(ini_file, section_name)) {
        return 0x1002;
    }
    LoadIniEntry(ini_file, section_name, BRT_BRENDER_PATH_STR, "BRENDER_PATH");
    LoadIniEntry(ini_file, section_name, BRT_BRENDER_DRIVERS_STR, "BRENDER_DRIVERS");
    LoadIniEntry(ini_file, section_name, BRT_BRENDER_DEVICES_STR, "BRENDER_DEVICES");
    LoadIniEntry(ini_file, section_name, BRT_DEFAULT_DEVICE_STR, "BRENDER_DOS_GFX");
    LoadIniEntry(ini_file, section_name, BRT_DEFAULT_DEVICE_STR, "BR_DEFAULT_DEVICE");
    return 0;
}

br_boolean (C2_HOOK_STDCALL * LoadRegistryEntry_original)(char* Reg_Path, void* hKey, br_token t, char* Entry);
br_boolean C2_HOOK_STDCALL LoadRegistryEntry(char* Reg_Path, void* hKey, br_token t, char* Entry) {

#if 0//defined(C2_HOOKS_ENABLED)
    return LoadRegistryEntry_original(Reg_Path, hKey, t, Entry);
#else
    char Temp[255];
    br_uint_16 size;
    br_value v;
    br_error r;

    r = HostRegistryQuery(hKey, Reg_Path, Entry, Temp, sizeof(Temp), &size);
    if (r != 0) {
        return r;
    }
    v.cstr = Temp;
    BrAssociativeArraySetEntry(C2V(fw).sys_config, t, v);
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00530920, LoadRegistryEntry, LoadRegistryEntry_original)

br_error C2_HOOK_STDCALL LoadRegistryConfig(char* Reg_Path, void* hKey) {

    if (!LoadRegistryEntry(Reg_Path, hKey, BRT_BRENDER_PATH_STR, "BRENDER_PATH")) {
        return 0x1002;
    }
    if (!LoadRegistryEntry(Reg_Path, hKey, BRT_BRENDER_DRIVERS_STR, "BRENDER_DRIVERS")) {
        return 0x1002;
    }
    if (!LoadRegistryEntry(Reg_Path, hKey, BRT_BRENDER_DEVICES_STR, "BRENDER_DEVICES")) {
        return 0x1002;
    }
    if (!LoadRegistryEntry(Reg_Path, hKey, BRT_DEFAULT_DEVICE_STR, "BRENDER_DOS_GFX")) {
        return 0x1002;
    }
    if (!LoadRegistryEntry(Reg_Path, hKey, BRT_DEFAULT_DEVICE_STR, "BR_DEFAULT_DEVICE")) {
        return 0x1002;
    }
    return 0;
}

br_error (C2_HOOK_CDECL * BrSetDefaultConfig_original)(br_token t, char* Entry);
br_error C2_HOOK_CDECL BrSetDefaultConfig(br_token t, char* Entry) {

#if 0//defined(C2_HOOKS_ENABLED)
    return BrSetDefaultConfig_original(t, Entry);
#else
    char Reg_Path[255];
    int v0;
    int v1;
    int v2;
    br_value v;

    v0 = 1;
    v1 = 3;
    v2 = 0;
    BrSprintf(Reg_Path, "%s\\%d.%d.%d","SOFTWARE\\Argonaut\\BRender", v0, v1, v2);
    if (LoadRegistryEntry(Reg_Path, NULL, t, Entry)) {
        return 0;
    }

    if (LoadIniEntry("BRender.ini", "BRender", t, Entry)) {
        return 0;
    }

    v.cstr = BrGetEnv(Entry);
    if (v.cstr != NULL) {
        BrAssociativeArraySetEntry(C2V(fw).sys_config, t, v);
    }
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00530990, BrSetDefaultConfig, BrSetDefaultConfig_original)

br_error (C2_HOOK_CDECL * BrSystemConfigBegin_original)(void);
br_error C2_HOOK_CDECL BrSystemConfigBegin(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    return BrSystemConfigBegin_original();
#else
    char temp[255];
    br_value v;

    C2V(fw).sys_config = BrAssociativeArrayAllocate();
    if (C2V(fw).sys_config == NULL) {
        BrFailure("System config associative array allocate failed.\n");
        return 0x1002;
    }
    BrSetDefaultConfig(BRT_BRENDER_PATH_STR, "BRENDER_PATH");
    BrSetDefaultConfig(BRT_BRENDER_DRIVERS_STR, "BRENDER_DRIVERS");
    BrSetDefaultConfig(BRT_BRENDER_DRIVERS_STR, "BR_DRIVERS");
    BrSetDefaultConfig(BRT_BRENDER_DEVICES_STR, "BRENDER_DEVICES");
    BrSetDefaultConfig(BRT_BRENDER_DEVICES_STR, "BR_DEVICES");
    BrSetDefaultConfig(BRT_DEFAULT_DEVICE_STR, "BRENDER_DOS_GFX");
    BrSetDefaultConfig(BRT_DEFAULT_DEVICE_STR, "BRENDER_DEFAULT_DEVICE");
    BrSetDefaultConfig(BRT_DEFAULT_DEVICE_STR, "BR_DEFAULT_DEVICE");
    BrSystemConfigQueryString(BRT_DEFAULT_DEVICE_STR, temp, sizeof(temp));
    if (temp[0] == '\0') {
        v.cstr = HostDefaultDevice();
        BrAssociativeArraySetEntry(C2V(fw).sys_config, BRT_DEFAULT_DEVICE_STR, v);
    }
    BrSystemConfigQueryString(BRT_BRENDER_PATH_STR, temp, sizeof(temp));
    if (temp[0] == '\0') {
        v.cstr = "dat;../dat;../../dat;../../../dat;../../../../dat";
        BrAssociativeArraySetEntry(C2V(fw).sys_config, BRT_BRENDER_PATH_STR, v);
    }
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00530460, BrSystemConfigBegin, BrSystemConfigBegin_original)


br_error (C2_HOOK_CDECL * BrSystemConfigLoad_original)(br_token t, char* Param1, void* Param2);
br_error C2_HOOK_CDECL BrSystemConfigLoad(br_token t, char* Param1, void* Param2) {

#if 0//defined(C2_HOOKS_ENABLED)
    return BrSystemConfigLoad_original(t, Param1, Param2);
#else
    br_error r;

    switch (t) {
    case BRT_REGISTRY_STR:
        r = LoadRegistryConfig(Param1, Param2);
        break;
    case BRT_INI_STR:
        r = LoadIniConfig(Param1, Param2);
        break;
    default:
        BrFailure("Invalid system config load token. Must be REGISTRY or INI");
        r = 0x1002;
        break;
    }
    return r;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00530a90, BrSystemConfigLoad, BrSystemConfigLoad_original)

br_error (C2_HOOK_CDECL * BrSystemConfigSetString_original)(br_token t, char* string);
br_error C2_HOOK_CDECL BrSystemConfigSetString(br_token t, char* string) {

#if 0//defined(C2_HOOKS_ENABLED)
    return BrSystemConfigSetString_original(t, string);
#else
    br_value v;

    if (!Is_Valid_Sys_Config_Token(t)) {
        BrFailure("Not a valid System configuration token.\n");
        return 0x1002;
    }
    v.cstr = string;
    return BrAssociativeArraySetEntry(C2V(fw).sys_config, t, v);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00530d80, BrSystemConfigSetString, BrSystemConfigSetString_original)


br_error (C2_HOOK_CDECL * BrSystemConfigQueryString_original)(br_token t, char* string, int max_size);
br_error C2_HOOK_CDECL BrSystemConfigQueryString(br_token t, char* string, int max_size) {

#if 0//defined(C2_HOOKS_ENABLED)
    return BrSystemConfigQueryString_original(t, string, max_size);
#else
    br_error r;
    br_value v;

    if (!Is_Valid_Sys_Config_Token(t)) {
        BrFailure("Not a valid system configuration token.\n");
        return 0x1002;
    }
    r = BrAssociativeArrayQuery(C2V(fw).sys_config, t, &v);
    if (r != 0) {
        string[0] = '\0';
        return r;
    }
    if (v.cstr == NULL) {
        string[0] = '\0';
        return 0;
    }
    BrStrNCpy(string, v.cstr, max_size);
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00530de0, BrSystemConfigQueryString, BrSystemConfigQueryString_original)
