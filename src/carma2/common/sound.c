#include "sound.h"

#include "globvars.h"
#include "loading.h"
#include "utility.h"

#include <string.h>

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gSound_detail_level, 0x00595c48, 1);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCD_fully_installed, 0x0068b898);

C2_HOOK_VARIABLE_IMPLEMENT(tS3_outlet*, gEffects_outlet, 0x006845fc);

void C2_HOOK_FASTCALL UsePathFileToDetermineIfFullInstallation(void) {
    char line1[80];
    char line2[80];
    char line3[80];
    char path_file[80];
    FILE* fp;

    strcpy(path_file, C2V(gApplication_path));
    strcat(path_file, (char*)C2V(gDir_separator));
    strcat(path_file, "PATHS.TXT");

    if (PDCheckDriveExists(path_file)) {
        fp = TWT_fopen(path_file, "rt");
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


void (C2_HOOK_FASTCALL * ParseSoundFxDetails_original)(FILE* pF, tSpecial_volume_soundfx_data* pSpec);
void C2_HOOK_FASTCALL ParseSoundFxDetails(FILE* pF, tSpecial_volume_soundfx_data* pSpec) {
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

void (C2_HOOK_FASTCALL * DRS3StartCDA_original)(tS3_sound_id pCDA_id);
void C2_HOOK_FASTCALL DRS3StartCDA(tS3_sound_id pCDA_id) {
#if defined(C2_HOOKS_ENABLED)
    DRS3StartCDA_original(pCDA_id);
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00456840, DRS3StartCDA, DRS3StartCDA_original)

tS3_sound_tag (C2_HOOK_FASTCALL * DRS3StartSound_original)(tS3_outlet* pOutlet, tS3_sound_id pSound);
tS3_sound_tag C2_HOOK_FASTCALL DRS3StartSound(tS3_outlet* pOutlet, tS3_sound_id pSound) {

#if defined(C2_HOOKS_ENABLED)
    return DRS3StartSound_original(pOutlet, pSound);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00455690, DRS3StartSound, DRS3StartSound_original)

void (C2_HOOK_FASTCALL * InitSound_original)(void);
void C2_HOOK_FASTCALL InitSound(void) {

#if defined(C2_HOOKS_ENABLED)
    InitSound_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00455080, InitSound, InitSound_original)

void (C2_HOOK_FASTCALL * SetSoundVolumes_original)(int pCD_audio);
void C2_HOOK_FASTCALL SetSoundVolumes(int pCD_audio) {
#if defined(C2_HOOKS_ENABLED)
    SetSoundVolumes_original(pCD_audio);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00456530, SetSoundVolumes, SetSoundVolumes_original)

void (C2_HOOK_FASTCALL * SoundService_original)(void);
void C2_HOOK_FASTCALL SoundService(void) {
#if defined(C2_HOOKS_ENABLED)
    SoundService_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00455a80, SoundService, SoundService_original)

int (C2_HOOK_FASTCALL * DRStopCarSounds_original)(void);
int C2_HOOK_FASTCALL DRStopCarSounds(void) {
#if defined(C2_HOOKS_ENABLED)
    return DRStopCarSounds_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004559e0, DRStopCarSounds, DRStopCarSounds_original)
