#include "sound.h"

#include "globvars.h"
#include "loading.h"
#include "utility.h"

#include <string.h>

C2_HOOK_VARIABLE_IMPLEMENT(int, gCD_fully_installed, 0x0068b898);

void C2_HOOK_FASTCALL UsePathFileToDetermineIfFullInstallation(void) {
    char line1[80];
    char line2[80];
    char line3[80];
    char path_file[80];
    tTWTFILE* fp;

    strcpy(path_file, C2V(gApplication_path));
    strcat(path_file, (char*)C2V(gDir_separator));
    strcat(path_file, "PATHS.TXT");

    if (PDCheckDriveExists(path_file)) {
        fp = DRfopen(path_file, "rt");
        if (fp == NULL) {
            return;
        }
        line1[0] = '\0';
        line2[0] = '\0';
        line3[0] = '\0';
        GetALineWithNoPossibleService(fp, line1);
        GetALineWithNoPossibleService(fp, line2);
        GetALineWithNoPossibleService(fp, line3);
        DRfclose(fp);
        if (strcmp(line3, "Full") != 0) {
            return;
        }
    }
    C2V(gCD_fully_installed) = 1;
}
C2_HOOK_FUNCTION(0x00454f40, UsePathFileToDetermineIfFullInstallation)


void (C2_HOOK_FASTCALL * ParseSoundFxDetails_original)(tTWTFILE* pF, tSpecial_volume_soundfx_data* pSpec);
void C2_HOOK_FASTCALL ParseSoundFxDetails(tTWTFILE* pF, tSpecial_volume_soundfx_data* pSpec) {
#if defined(C2_HOOKS_ENABLED)
    ParseSoundFxDetails_original(pF, pSpec);
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004569f0, ParseSoundFxDetails, ParseSoundFxDetails_original)

void (C2_HOOK_FASTCALL * StopMusic_original)(void);
void C2_HOOK_FASTCALL StopMusic(void) {
#if defined(C2_HOOKS_ENABLED)
    C2_HOOK_START();
    StopMusic_original();
    C2_HOOK_FINISH();
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00456910, StopMusic, StopMusic_original)
