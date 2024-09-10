#include "options.h"

#include "errors.h"
#include "globvars.h"
#include "loading.h"
#include "utility.h"

#include "c2_string.h"

#include "rec2_macros.h"
#include "rec2_types.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char*, gKey_names, 153, 0x00688458);
C2_HOOK_VARIABLE_IMPLEMENT(int, gOrig_key_map_index, 0x00688440);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_ADV(int, gBackup_key_mappings, [4][77], 0x006869e0);


void (C2_HOOK_FASTCALL * DoOptions_original)(void);
void C2_HOOK_FASTCALL DoOptions(void) {

    // Relict from Carmageddon 1, unused here
    C2_HOOK_ASSUME_UNUSED();
#if defined(C2_HOOKS_ENABLED)
    DoOptions_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b39f0, DoOptions, DoOptions_original)

void C2_HOOK_FASTCALL StripControls(char* pStr) {
    int i;
    int len;

    len = c2_strlen(pStr);
    for (i = 0; i < len; i++) {
        if ((tU8)pStr[i] < ' ') {
            c2_memmove(&pStr[i], &pStr[i + 1], (len - i) * sizeof(char));
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

    PathCat(the_path, C2V(gApplication_path), "KEYNAMES.TXT");
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CantOpenKeyNamesFile);
    }
    for (i = 0; i < REC2_ASIZE(C2V(gKey_names)); i++) {
        PFfgets(s, sizeof(s), f);
        StripControls(s);
        C2V(gKey_names)[i] = BrMemAllocate(c2_strlen(s) + 1, kMem_misc);
        c2_strcpy(C2V(gKey_names)[i], s);
    }
    PFfclose(f);
}

void C2_HOOK_FASTCALL DisposeKeyNames(void) {
    int i;

    for (i = 0; i < REC2_ASIZE(C2V(gKey_names)); i++) {
        BrMemFree(C2V(gKey_names)[i]);
    }
}

void C2_HOOK_FASTCALL BackupKeyMappings(void) {
    int i;

    C2V(gOrig_key_map_index) = C2V(gKey_map_index);

    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gBackup_key_mappings)) != 4);
    C2_HOOK_BUG_ON(sizeof(C2V(gKey_mapping)) != 4 * 77);

    for (i = 0; i < REC2_ASIZE(C2V(gBackup_key_mappings)); i++) {

        C2V(gKey_map_index) = i;
        LoadKeyMapping();
        c2_memcpy(C2V(gBackup_key_mappings)[i],
            C2V(gKey_mapping),
            sizeof(C2V(gKey_mapping)));
    }
    C2V(gKey_map_index) = C2V(gOrig_key_map_index);
    c2_memcpy(C2V(gKey_mapping),
        C2V(gBackup_key_mappings)[C2V(gKey_map_index)],
        sizeof(C2V(gKey_mapping)));
}
