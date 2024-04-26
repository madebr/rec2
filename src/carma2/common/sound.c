#include "sound.h"

#include "globvars.h"
#include "loading.h"
#include "opponent.h"
#include "utility.h"

#include <s3/s3.h>

#include "c2_string.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gSound_detail_level, 0x00595c48, 1);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCD_fully_installed, 0x0068b898);
C2_HOOK_VARIABLE_IMPLEMENT(int, gSound_sources_inited, 0x00684560);

C2_HOOK_VARIABLE_IMPLEMENT(tS3_outlet*, gEffects_outlet, 0x006845fc);
C2_HOOK_VARIABLE_IMPLEMENT(tS3_outlet*, gEngine_outlet, 0x00684604);
C2_HOOK_VARIABLE_IMPLEMENT(int, gMusic_available, 0x00684564);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_00684554, 0x00684554);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_00684568, 0x00684568);

void C2_HOOK_FASTCALL UsePathFileToDetermineIfFullInstallation(void) {
    char line1[80];
    char line2[80];
    char line3[80];
    char path_file[80];
    FILE* fp;

    c2_strcpy(path_file, C2V(gApplication_path));
    c2_strcat(path_file, (char*)C2V(gDir_separator));
    c2_strcat(path_file, "PATHS.TXT");

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
        if (c2_strcmp(line3, "Full") != 0) {
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
#if 0//defined(C2_HOOKS_ENABLED)
    C2_HOOK_START();
    StopMusic_original();
    C2_HOOK_FINISH();
#else
    if (C2V(gCD_fully_installed) && C2V(gMusic_available) && C2V(gINT_00684568) != 0) {
        S3StopSound(C2V(gINT_00684568));
        C2V(gINT_00684554) = 0;
        C2V(gINT_00684568) = 0;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00456910, StopMusic, StopMusic_original)

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

int C2_HOOK_FASTCALL DRS3StopOutletSound(tS3_outlet* pOutlet) {

    if (C2V(gSound_enabled)) {
        return S3StopOutletSound(pOutlet);
    }
    return 0;
}
C2_HOOK_FUNCTION(0x00455970, DRS3StopOutletSound)

void C2_HOOK_FASTCALL ToggleSoundEnable(void) {

    if (C2V(gSound_enabled)) {
        S3StopAllOutletSounds();
        S3Disable();
        C2V(gSound_enabled) = 0;
    } else {
        S3Enable();
        C2V(gSound_enabled) = 1;
    }
}
C2_HOOK_FUNCTION(0x00455a50, ToggleSoundEnable)

void C2_HOOK_FASTCALL DisposeSoundSources(void) {
    int cat;
    int car_count;
    int i;
    int toggle;
    tCar_spec* the_car;

    if (!C2V(gSound_available)) {
        return;
    }
    toggle = !C2V(gSound_enabled);
    if (toggle) {
        ToggleSoundEnable();
    }
    if (C2V(gSound_sources_inited)) {
        DRS3StopOutletSound(C2V(gEngine_outlet));
        if (C2V(gProgram_state).cockpit_on && C2V(gProgram_state).cockpit_image_index >= 0) {
            S3Service(1, 0);
        } else {
            S3Service(0, 0);
        }
        for (cat = eVehicle_self; cat <= eVehicle_rozzer; cat++) {
            if (cat == eVehicle_self) {
                car_count = 1;
            } else {
                car_count = GetCarCount(cat);
            }
            for (i = 0; i < car_count; i++) {
                if (cat == eVehicle_self) {
                    the_car = &C2V(gProgram_state).current_car;
                } else {
                    the_car = GetCarSpec(cat, i);
                }
                if ((the_car != NULL && the_car->driver == eDriver_local_human) || C2V(gSound_detail_level) == 3 || cat == eVehicle_rozzer) {
                    if (the_car->sound_source != NULL) {
                        S3UpdateSoundSource(C2V(gEngine_outlet), -1, the_car->sound_source, 0.0f, 0, 0, 0, 0x10000, 0x10000);
                        S3ReleaseSoundSource(the_car->sound_source);
                    }
                    the_car->sound_source = NULL;
                }
            }
        }
        C2V(gSound_sources_inited) = 0;
    }
    if (toggle) {
        ToggleSoundEnable();
    }
}
C2_HOOK_FUNCTION(0x00455de0, DisposeSoundSources)

void (C2_HOOK_FASTCALL * InitSoundSources_original)(void);
void C2_HOOK_FASTCALL InitSoundSources(void) {

#if defined(C2_HOOKS_ENABLED)
    InitSoundSources_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00455bb0, InitSoundSources, InitSoundSources_original)

void (C2_HOOK_FASTCALL * StartMusicTrack_original)(int pMusic_track);
void C2_HOOK_FASTCALL StartMusicTrack(int pMusic_track) {

#if defined(C2_HOOKS_ENABLED)
    StartMusicTrack_original(pMusic_track);
#else
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00456840, StartMusicTrack, StartMusicTrack_original)
