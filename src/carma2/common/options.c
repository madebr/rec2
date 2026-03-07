#include "options.h"

#include "52-errors.h"
#include "globvars.h"
#include "loading.h"
#include "utility.h"

#include "c2_string.h"

#include "rec2_macros.h"
#include "rec2_types.h"


// GLOBAL: CARMA2_HW 0x00688458
char* gKey_names[153];

// GLOBAL: CARMA2_HW 0x00688440
int gOrig_key_map_index;

// GLOBAL: CARMA2_HW 0x006869e0
int gBackup_key_mappings[4][77];


// FUNCTION: CARMA2_HW 0x004b39f0
void C2_HOOK_FASTCALL DoOptions(void) {

    // Relict from Carmageddon 1, unused here
    UNUSED();
    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL StripControls(char* pStr) {
    int i;
    int len;

    len = strlen(pStr);
    for (i = 0; i < len; i++) {
        if ((tU8)pStr[i] < ' ') {
            memmove(&pStr[i], &pStr[i + 1], (len - i) * sizeof(char));
            len--;
#ifdef REC2_FIX_BUGS
            /* correctly handle stripping multiple control characters */
            i--;
#endif
        }
    }
}

void C2_HOOK_FASTCALL LoadKeyNames(void) {
    int i;
    FILE* f;
    tPath_name the_path;
    char s[256];

    PathCat(the_path, gApplication_path, "KEYNAMES.TXT");
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CantOpenKeyNamesFile);
    }
    for (i = 0; i < REC2_ASIZE(gKey_names); i++) {
        PFfgets(s, sizeof(s), f);
        StripControls(s);
        gKey_names[i] = BrMemAllocate(strlen(s) + 1, kMem_misc);
        strcpy(gKey_names[i], s);
    }
    PFfclose(f);
}

void C2_HOOK_FASTCALL DisposeKeyNames(void) {
    int i;

    for (i = 0; i < REC2_ASIZE(gKey_names); i++) {
        BrMemFree(gKey_names[i]);
    }
}

void C2_HOOK_FASTCALL BackupKeyMappings(void) {
    int i;

    gOrig_key_map_index = gKey_map_index;

    C2_HOOK_BUG_ON(REC2_ASIZE(gBackup_key_mappings) != 4);
    C2_HOOK_BUG_ON(sizeof(gKey_mapping) != 4 * 77);

    for (i = 0; i < REC2_ASIZE(gBackup_key_mappings); i++) {

        gKey_map_index = i;
        LoadKeyMapping();
        memcpy(gBackup_key_mappings[i],
            gKey_mapping,
            sizeof(gKey_mapping));
    }
    gKey_map_index = gOrig_key_map_index;
    memcpy(gKey_mapping,
        gBackup_key_mappings[gKey_map_index],
        sizeof(gKey_mapping));
}
