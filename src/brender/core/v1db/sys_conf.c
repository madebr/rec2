#include "sys_conf.h"

#include "core/fw/assocarr.h"
#include "core/fw/brprintf.h"
#include "core/fw/diag.h"
#include "core/fw/fwsetup.h"
#include "core/host/hostcfg.h"
#include "core/std/brstdlib.h"

// GLOBAL: CARMA2_HW 0x0066c870
br_token valid_system_config_tokens[] = {
    BRT_BRENDER_PATH_STR,
    BRT_BRENDER_DRIVERS_STR,
    BRT_DEFAULT_DEVICE_STR,
    BRT_BRENDER_DEVICES_STR,
    BR_NULL_TOKEN,
};

br_boolean C2_HOOK_STDCALL Is_Valid_Sys_Config_Token(br_token t) {
    int i;

    for (i = 0; valid_system_config_tokens[i] != BR_NULL_TOKEN; i++) {
        if (valid_system_config_tokens[i] == t) {
            return 1;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x005308b0
br_boolean C2_HOOK_STDCALL LoadIniEntry(char* ini_file, char* section_name, br_token t, char* Entry) {
    char Temp[255];
    br_uint_16 size;
    br_value v;
    br_error r;

    r = HostIniQuery(ini_file, section_name, Entry, Temp, sizeof(Temp), &size);
    if (r != 0) {
        return r;
    }
    v.cstr = Temp;
    BrAssociativeArraySetEntry(fw.sys_config, t, v);
    return 0;
}

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

// FUNCTION: CARMA2_HW 0x00530920
br_boolean C2_HOOK_STDCALL LoadRegistryEntry(char* Reg_Path, void* hKey, br_token t, char* Entry) {
    char Temp[255];
    br_uint_16 size;
    br_value v;
    br_error r;

    r = HostRegistryQuery(hKey, Reg_Path, Entry, Temp, sizeof(Temp), &size);
    if (r != 0) {
        return r;
    }
    v.cstr = Temp;
    BrAssociativeArraySetEntry(fw.sys_config, t, v);
    return 0;
}

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

// FUNCTION: CARMA2_HW 0x00530990
br_error C2_HOOK_CDECL BrSetDefaultConfig(br_token t, char* Entry) {
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
        BrAssociativeArraySetEntry(fw.sys_config, t, v);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00530460
br_error C2_HOOK_CDECL BrSystemConfigBegin(void) {
    char temp[255];
    br_value v;

    fw.sys_config = BrAssociativeArrayAllocate();
    if (fw.sys_config == NULL) {
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
        BrAssociativeArraySetEntry(fw.sys_config, BRT_DEFAULT_DEVICE_STR, v);
    }
    BrSystemConfigQueryString(BRT_BRENDER_PATH_STR, temp, sizeof(temp));
    if (temp[0] == '\0') {
        v.cstr = "dat;../dat;../../dat;../../../dat;../../../../dat";
        BrAssociativeArraySetEntry(fw.sys_config, BRT_BRENDER_PATH_STR, v);
    }
    return 0;
}


// FUNCTION: CARMA2_HW 0x00530a90
br_error C2_HOOK_CDECL BrSystemConfigLoad(br_token t, char* Param1, void* Param2) {
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
}

// FUNCTION: CARMA2_HW 0x00530d80
br_error C2_HOOK_CDECL BrSystemConfigSetString(br_token t, char* string) {
    br_value v;

    if (!Is_Valid_Sys_Config_Token(t)) {
        BrFailure("Not a valid System configuration token.\n");
        return 0x1002;
    }
    v.cstr = string;
    return BrAssociativeArraySetEntry(fw.sys_config, t, v);
}


// FUNCTION: CARMA2_HW 0x00530de0
br_error C2_HOOK_CDECL BrSystemConfigQueryString(br_token t, char* string, int max_size) {
    br_error r;
    br_value v;

    if (!Is_Valid_Sys_Config_Token(t)) {
        BrFailure("Not a valid system configuration token.\n");
        return 0x1002;
    }
    r = BrAssociativeArrayQuery(fw.sys_config, t, &v);
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
}
