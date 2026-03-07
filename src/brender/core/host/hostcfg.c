#include "hostcfg.h"

#include "core/fw/diag.h"

#ifdef _WIN32
#include <windows.h>
#endif

// FUNCTION: CARMA2_HW 0x0053fba0
const char* C2_HOOK_CDECL HostDefaultDevice(void) {

    return "DDRAW";
}


// FUNCTION: CARMA2_HW 0x0053fbb0
br_boolean C2_HOOK_CDECL HostIniSectionExists(char* ini_file, char* section_name) {
#ifdef _WIN32
    char buffer[5];

    return GetPrivateProfileSectionA(section_name, buffer, sizeof(buffer), ini_file) != 0;
#else
    return 0;
#endif
}


// FUNCTION: CARMA2_HW 0x0053fbe0
br_error C2_HOOK_CDECL HostIniQuery(char* ini_file, char* section_name, char* entry, char* Buffer, int max, br_uint_16* size) {

#ifdef _WIN32
    static char default_entry[] = "";

    *size = (br_uint_16)GetPrivateProfileStringA(section_name, entry, default_entry, Buffer, max, ini_file);
    return 0;
#else
    return 0x1002;
#endif
}


// FUNCTION: CARMA2_HW 0x0053fc10
br_error C2_HOOK_CDECL HostRegistryQuery(void* hKey, char* Path, char* entry, char* Buffer, br_uint_16 max, br_uint_16* size) {
#ifdef _WIN32
    DWORD cbData;
    DWORD type;

    if (hKey == NULL) {
        hKey = HKEY_LOCAL_MACHINE;
    }
    if (RegOpenKeyExA(hKey, Path, 0, KEY_READ, (PHKEY)&hKey) != ERROR_SUCCESS) {
        return 0x1002;
    }
    type = REG_NONE;
    cbData = max;
    if (RegQueryValueExA((HKEY)hKey, entry, NULL, &type, (LPBYTE)Buffer, &cbData) != ERROR_SUCCESS) {
        return 0x1002;
    }
    if (type != REG_SZ) {
        BrFailure("Registry entry type is not a string");
    }
    *size = (br_uint_16)cbData;
    return 0;
#else
    return 0x1002;
#endif
}
