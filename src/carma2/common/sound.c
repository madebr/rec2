#include "sound.h"

#include "errors.h"
#include "globvars.h"
#include "loading.h"
#include "opponent.h"
#include "piping.h"
#include "replay.h"
#include "utility.h"
#include "world.h"

#include "platform.h"

#include <s3/s3.h>

#include "c2_string.h"

#include "rec2_macros.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gSound_detail_level, 0x00595c48, 1);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCD_fully_installed, 0x0068b898);
C2_HOOK_VARIABLE_IMPLEMENT(int, gSound_sources_inited, 0x00684560);

C2_HOOK_VARIABLE_IMPLEMENT(tS3_outlet*, gEffects_outlet, 0x006845fc);
C2_HOOK_VARIABLE_IMPLEMENT(tS3_outlet*, gEngine_outlet, 0x00684604);
C2_HOOK_VARIABLE_IMPLEMENT(int, gMusic_available, 0x00684564);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_00684554, 0x00684554);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_00684568, 0x00684568);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gVirgin_pass, 0x00655dd8, 1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gOld_sound_detail_level, 0x00595c4c, -1);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCD_is_disabled, 0x0079e13c);
C2_HOOK_VARIABLE_IMPLEMENT(tS3_outlet*, gDriver_outlet, 0x00684608);
C2_HOOK_VARIABLE_IMPLEMENT(tS3_outlet*, gMusic_outlet, 0x00684610);
C2_HOOK_VARIABLE_IMPLEMENT(tS3_outlet*, gCar_outlet, 0x00684600);
C2_HOOK_VARIABLE_IMPLEMENT(tS3_outlet*, gPedestrians_outlet, 0x0068460c);
C2_HOOK_VARIABLE_IMPLEMENT(tS3_outlet*, gXXX_outlet, 0x00684614);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tS3_outlet*, gIndexed_outlets, 6, 0x0079e160);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gSound_periodicity_choices, 3, 0x00595c28, {
    "RANDOM",
    "PERIODIC",
    "CONTINUOUS",
});
C2_HOOK_VARIABLE_IMPLEMENT(br_vector3, gCamera_left, 0x0079e120);
C2_HOOK_VARIABLE_IMPLEMENT(br_vector3, gCamera_position, 0x0079e130);
C2_HOOK_VARIABLE_IMPLEMENT(br_vector3, gCamera_velocity, 0x0079ea60);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_00684540, 0x00684540);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_0079e18c, 0x0079e18c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_0079e17c, 0x0079e17c);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tEnvironment_sound_source, gEnvironment_sound_sources, 5, 0x00684570);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gINT_00595c44, 0x00595c44, 1);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gNext_track_finished_check, 0x00684548);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gINT_00595c20, 0x00595c20, 9998);
C2_HOOK_VARIABLE_IMPLEMENT(int, gServicing_sound, 0x00684558);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gLast_sound_service, 0x00684544);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCDA_started_playing, 0x0068454c);
C2_HOOK_VARIABLE_IMPLEMENT(undefined4, gUNK_0068455c, 0x0068455c);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gLast_tune, 0x00595c50, -1);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(int, gRandom_CDA_tunes, 8, 0x00595c78, { 9600, 9601, 9602, 9603, 9604, 9605, 9606, 9607 });
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(int, gRandom_CDA_tunes_1, 4, 0x00595c98, { 9600, 9601, 9602, 9603 });
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(int, gRandom_CDA_tunes_2, 4, 0x00595ca8, { 9604, 9605, 9606, 9607 });
C2_HOOK_VARIABLE_IMPLEMENT(br_vector3, gOld_camera_position, 0x0079e180);

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
        fp = PFfopen(path_file, "rt");
        if (fp == NULL) {
            return;
        }
        line1[0] = '\0';
        line2[0] = '\0';
        line3[0] = '\0';
        GetALineWithNoPossibleService(fp, line1);
        GetALineWithNoPossibleService(fp, line2);
        GetALineWithNoPossibleService(fp, line3);
        PFfclose(fp);
        if (c2_strcmp(line3, "Full") != 0) {
            return;
        }
    }
    C2V(gCD_fully_installed) = 1;
}
C2_HOOK_FUNCTION(0x00454f40, UsePathFileToDetermineIfFullInstallation)

void (C2_HOOK_FASTCALL * ReadSoundSpec_original)(FILE* pF, tSpecial_volume_soundfx_data* pSpec);
void C2_HOOK_FASTCALL ReadSoundSpec(FILE* pF, tSpecial_volume_soundfx_data* pSpec) {
#if 0//defined(C2_HOOKS_ENABLED)
    ReadSoundSpec_original(pF, pSpec);
#else
    int i;

    pSpec->periodicity = GetALineAndInterpretCommand(pF, C2V(gSound_periodicity_choices), REC2_ASIZE(C2V(gSound_periodicity_choices)));
    if (pSpec->periodicity == kSoundFxPeriodicity_None) {
        return;
    }
    if (pSpec->periodicity == kSoundFxPeriodicity_Random) {
        float f1, f2;

        GetPairOfFloats(pF, &f1, &f2);
        pSpec->periodic1 = (int)(1000.f * f1);
        pSpec->periodic2 = (int)(1000.f * f2);
    } else if (pSpec->periodicity == kSoundFxPeriodicity_Periodic) {
        pSpec->periodic1 = (int)(1000.f * GetAScalar(pF));
    }
    pSpec->field_0x14 = BR_FIXED_INT(GetAnInt(pF)) / 100;
    pSpec->count_sound_alternatives = GetAnInt(pF);

    C2_HOOK_BUG_ON(REC2_ASIZE(pSpec->sound_alternatives) != 5);
    if (pSpec->count_sound_alternatives > REC2_ASIZE(pSpec->sound_alternatives)) {
        FatalError(kFatalError_TooManyEnvironmentalSoundAlternatives);
    }
    for (i = 0; i < pSpec->count_sound_alternatives; i++) {

        pSpec->sound_alternatives[i] = LoadSingleSound(&C2V(gTrack_storage_space), GetAnInt(pF));
    }
    pSpec->field_0xc = 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004569f0, ReadSoundSpec, ReadSoundSpec_original)

void C2_HOOK_FASTCALL WriteOutSoundSpec(FILE* pF, tSpecial_volume_soundfx_data* pSpec) {
    int i;

    if (pSpec->periodicity == kSoundFxPeriodicity_None) {
        c2_fprintf(pF, "NONE\t\t\t\t\t// sound time type\n");
        return;
    }

    c2_fprintf(pF, "%s\t\t\t\t\t// sound time type\n", C2V(gSound_periodicity_choices)[pSpec->periodicity]);
    switch (pSpec->periodicity) {
    case kSoundFxPeriodicity_Random:
        c2_fprintf(pF, "%.2f,%.2f\t\t\t\t\t// min,max gap\n",
           (float)pSpec->periodic1 / 1000.f,
           (float)pSpec->periodic2 / 1000.f);
        break;
    case kSoundFxPeriodicity_Periodic:
        c2_fprintf(pF,"%.2f\t\t\t\t\t// period\n",
            (float)pSpec->periodic1 / 1000.f);
        break;
    default:
        break;
    }
    c2_fprintf(pF, "%d\t\t\t\t\t// max deviation\n", (int)BrFixedToFloat(100.f * pSpec->field_0x14));
    c2_fprintf(pF, "%d\t\t\t\t\t// num sounds\n", pSpec->count_sound_alternatives);
    for (i = 0; i < pSpec->count_sound_alternatives; i++) {

        c2_fprintf(pF, "%d\t\t\t\t\t// sound ID\n", pSpec->sound_alternatives[i]);
    }
}
C2_HOOK_FUNCTION(0x00457450, WriteOutSoundSpec)

void C2_HOOK_FASTCALL StartMusic(void) {

    if (!C2V(gINT_00595c44) || C2V(gProgram_state).music_volume >= 128) {
        if (!IsCDAPlaying()) {
            C2V(gNext_track_finished_check) = PDGetTotalTime() + 10000;
            dr_dprintf("CDINFO: StartMusic(): New gNext_track_finished_check %d", C2V(gNext_track_finished_check));
            C2V(gINT_00684568) = DRS3StartCDA(C2V(gINT_00595c20));
        }
    }
}
C2_HOOK_FUNCTION(0x004568c0, StartMusic)

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

#if 0//defined(C2_HOOKS_ENABLED)
    return DRS3StartSound_original(pOutlet, pSound);
#else
    if (!C2V(gSound_enabled)) {
        return 0;
    }
    if (pSound != eSoundId_pratcam_whirr_noise && !(pSound >= 3000 && pSound <= 3007) && !(pSound >= 5300 && pSound <= 5320)) {
        PipeSingleSound(pOutlet, pSound, 0, 0, -1, NULL);
    }
    return S3StartSound(pOutlet, pSound);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00455690, DRS3StartSound, DRS3StartSound_original)

int C2_HOOK_FASTCALL DRS3StartSoundNoPiping(tS3_outlet* pOutlet, tS3_sound_id pSound) {
    if (C2V(gSound_enabled)) {
        return S3StartSound(pOutlet, pSound);
    } else {
        return 0;
    }
}

int C2_HOOK_FASTCALL DRS3StartCDA(int pSound) {

    if (pSound == 9999 || pSound == 9998 || pSound == 9997) {
        C2V(gINT_00595c20) = pSound;
    }
    if (!C2V(gCD_is_disabled) && C2V(gMusic_available) && !C2V(gINT_00684554) && !C2V(gINT_00684568) && S3IsCDAEnabled()) {
        dr_dprintf("CDINFO: DRS3StartCDA(): Requested track id %d", pSound);
        C2V(gCDA_started_playing) = 1;
        S3StopOutletSound(C2V(gMusic_outlet));
        if (C2V(gSound_enabled)) {
            if (C2V(gProgram_state).cockpit_on != 0 && C2V(gProgram_state).cockpit_image_index >= 0) {
                S3Service(1, 0);
            } else {
                S3Service(0, 0);
            }
            /* Random CDA track */
            if (pSound == 9999 || pSound == 9998 || pSound == 9997) {
                do {
                    switch (pSound) {
                    case 9997:
                        pSound = C2V(gRandom_CDA_tunes_2)[IRandomBetween(0, REC2_ASIZE(C2V(gRandom_CDA_tunes_2)) - 1)];
                        break;
                    case 9998:
                        pSound = C2V(gRandom_CDA_tunes_1)[IRandomBetween(0, REC2_ASIZE(C2V(gRandom_CDA_tunes_1)) - 1)];
                        break;
                    default:
                        pSound = C2V(gRandom_CDA_tunes)[IRandomBetween(0, REC2_ASIZE(C2V(gRandom_CDA_tunes)) - 1)];
                    }
                } while (pSound == C2V(gLast_tune));
            }
            C2V(gLast_tune) = pSound;
            C2V(gINT_00684554) = DRS3StartSoundNoPiping(C2V(gMusic_outlet), pSound);
            C2V(gINT_00684568) = C2V(gINT_00684554);
            if (!C2V(gINT_00684554)) {
                dr_dprintf("CDINFO: DRS3StartCDA(): Chosen actual CD track %d", pSound);
            }
            C2V(gUNK_0068455c) = 0;
        }
    }
    return C2V(gINT_00684568);
}
C2_HOOK_FUNCTION(0x004566d0, DRS3StartCDA)

void (C2_HOOK_FASTCALL * InitSound_original)(void);
void C2_HOOK_FASTCALL InitSound(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    InitSound_original();
#else
    tPath_name the_path;
    int engine_channel_count;
    int car_channel_count;
    int ped_channel_count;
    int xxx_channel_count;

    if (C2V(gVirgin_pass)) {
        PathCat(the_path, C2V(gApplication_path), "SOUND");
        PathCat(the_path, the_path, "SOUND.TXT");
        if (C2V(gSound_override)) {
            C2V(gSound_enabled) = 0;
        } else {
            C2V(gSound_enabled) = S3Init(the_path, C2V(gAusterity_mode), C2V(gPedSoundPath)) == 0;
        }
        C2V(gSound_available) = C2V(gSound_enabled);
        S3Set3DSoundEnvironment(0.14492753f, -1.0f, -1.0f);
        C2V(gVirgin_pass) = 0;
        C2V(gCD_is_disabled) = 0;
        UsePathFileToDetermineIfFullInstallation();
    }
    if (!C2V(gSound_available)) {
        return;
    }
    switch (C2V(gSound_detail_level)) {
    case 0:
        engine_channel_count = 2;
        car_channel_count = 2;
        ped_channel_count = 3;
        xxx_channel_count = 1;
        break;
    case 1:
        engine_channel_count = 4;
        car_channel_count = 3;
        ped_channel_count = 4;
        xxx_channel_count = 3;
        break;
    default:
        engine_channel_count = 6;
        car_channel_count = 4;
        ped_channel_count = 5;
        xxx_channel_count = 5;
        break;
    }
    if (C2V(gDriver_outlet) == NULL) {
        C2V(gIndexed_outlets)[0] = C2V(gDriver_outlet) = S3CreateOutlet(1, 1);
        if (C2V(gDriver_outlet) == NULL) {
            C2V(gSound_available) = 0;
            return;
        }
    }
    if (C2V(gMusic_outlet) == NULL) {
        C2V(gIndexed_outlets)[2] = C2V(gMusic_outlet) = S3CreateOutlet(1, 1);
        C2V(gMusic_available) = C2V(gMusic_outlet) != NULL;
        DRS3SetOutletVolume(C2V(gMusic_outlet), C2V(gProgram_state).music_volume);
    }
    if (C2V(gSound_detail_level) != C2V(gOld_sound_detail_level)) {
        if (C2V(gCar_outlet) != NULL) {
            S3ReleaseOutlet(C2V(gCar_outlet));
            C2V(gCar_outlet) = NULL;
        }
        if (C2V(gPedestrians_outlet) != NULL) {
            S3ReleaseOutlet(C2V(gPedestrians_outlet));
            C2V(gPedestrians_outlet) = NULL;
        }
        if (C2V(gEngine_outlet) != NULL) {
            S3ReleaseOutlet(C2V(gEngine_outlet));
            C2V(gEngine_outlet) = NULL;
        }
        if (C2V(gXXX_outlet) != NULL) {
            S3ReleaseOutlet(C2V(gXXX_outlet));
            C2V(gXXX_outlet) = NULL;
        }
        if (C2V(gEngine_outlet) == NULL) {
            C2V(gIndexed_outlets)[1] = C2V(gEngine_outlet) = S3CreateOutlet(engine_channel_count, engine_channel_count);
            if (C2V(gEngine_outlet) == NULL) {
                C2V(gSound_available) = 0;
                return;
            }
            DRS3SetOutletVolume(C2V(gEngine_outlet), C2V(gProgram_state).effects_volume);
        }
        if (C2V(gCar_outlet) == NULL) {
            C2V(gIndexed_outlets)[3] = C2V(gCar_outlet) = S3CreateOutlet(car_channel_count, car_channel_count);
            if (C2V(gCar_outlet) == NULL) {
                C2V(gSound_available) = 0;
                return;
            }
            DRS3SetOutletVolume(C2V(gCar_outlet), C2V(gProgram_state).effects_volume);
        }
        if (C2V(gPedestrians_outlet) == NULL) {
            C2V(gIndexed_outlets)[4] = C2V(gPedestrians_outlet) = S3CreateOutlet(ped_channel_count, ped_channel_count);
            if (C2V(gPedestrians_outlet) == NULL) {
                C2V(gSound_available) = 0;
                return;
            }
            DRS3SetOutletVolume(C2V(gPedestrians_outlet), C2V(gProgram_state).effects_volume);
        }
        if (C2V(gXXX_outlet) == NULL) {
            C2V(gIndexed_outlets)[5] = C2V(gXXX_outlet) = S3CreateOutlet(xxx_channel_count, xxx_channel_count);
            if (C2V(gXXX_outlet) == NULL) {
                C2V(gSound_available) = 0;
                return;
            }
            DRS3SetOutletVolume(C2V(gXXX_outlet), C2V(gProgram_state).effects_volume);
        }
    }
    if (C2V(gEffects_outlet) == NULL) {
        C2V(gIndexed_outlets)[5] = C2V(gEffects_outlet) = S3CreateOutlet(2, 2);
        if (C2V(gEffects_outlet) == NULL) {
            C2V(gSound_available) = 0;
            return;
        }
        DRS3SetOutletVolume(C2V(gEffects_outlet), C2V(gProgram_state).effects_volume);
    }
    C2V(gOld_sound_detail_level) = C2V(gSound_detail_level);
    SetSoundVolumes(0);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00455080, InitSound, InitSound_original)

void (C2_HOOK_FASTCALL * SetSoundVolumes_original)(int pCD_audio);
void C2_HOOK_FASTCALL SetSoundVolumes(int pCD_audio) {
#if 0//defined(C2_HOOKS_ENABLED)
    SetSoundVolumes_original(pCD_audio);
#else
    if (!C2V(gSound_enabled)) {
        return;
    }
    if (C2V(gEffects_outlet) != NULL) {
        DRS3SetOutletVolume(C2V(gEffects_outlet), C2V(gProgram_state).effects_volume);
    }
    DRS3SetOutletVolume(C2V(gCar_outlet), C2V(gProgram_state).effects_volume);
    DRS3SetOutletVolume(C2V(gEngine_outlet), C2V(gProgram_state).effects_volume);
    DRS3SetOutletVolume(C2V(gDriver_outlet), C2V(gProgram_state).effects_volume);
    DRS3SetOutletVolume(C2V(gPedestrians_outlet), C2V(gProgram_state).effects_volume);
    DRS3SetOutletVolume(C2V(gCar_outlet), C2V(gProgram_state).effects_volume);
    if (C2V(gINT_00595c44)) {
        dr_dprintf("CD: Setting volume to %d", C2V(gProgram_state).music_volume);
        if (!C2V(gCDA_started_playing) && !pCD_audio) {
            dr_dprintf("CD: Hasn't played yet, so not stopping/starting it");
        } else {
            if (C2V(gProgram_state).music_volume < 128) {
                StopMusic();
            } else {
                StartMusic();
            }
        }
    } else {
        DRS3SetOutletVolume(C2V(gMusic_outlet), C2V(gProgram_state).music_volume);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00456530, SetSoundVolumes, SetSoundVolumes_original)

void (C2_HOOK_FASTCALL * SoundService_original)(void);
void C2_HOOK_FASTCALL SoundService(void) {
#if defined(C2_HOOKS_ENABLED)
    SoundService_original();
#else

    if (!C2V(gSound_enabled)) {
        return;
    }
    if (C2V(gServicing_sound)) {
        return;
    }
    C2V(gServicing_sound) = 1;
    C2V(gLast_sound_service) = PDGetTotalTime();
    if (C2V(gCDA_started_playing)) {
        tU32 now = PDGetTotalTime();
        if (now > C2V(gNext_track_finished_check) && !IsCDAPlaying()) {
            dr_dprintf("CDINFO: SoundService(): Stopping & starting CD, time %d, gNext_track_finished_check %d",
                    now, C2V(gNext_track_finished_check));
            StopMusic();
            StartMusic();
        }
    }
    DRS3Service();
    gServicing_sound = 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00455a80, SoundService, SoundService_original)

void C2_HOOK_FASTCALL DRS3Service(void) {

    if (C2V(gSound_enabled)) {
        if (C2V(gProgram_state).cockpit_on  && C2V(gProgram_state).cockpit_image_index >= 0) {
            S3Service(1, 1);
        } else {
            S3Service(0, 1);
        }
    }
}

int C2_HOOK_FASTCALL IsCDAPlaying(void) {
    return PDS3IsCDAPlaying();
}
C2_HOOK_FUNCTION(0x00565d1a, IsCDAPlaying)

int (C2_HOOK_FASTCALL * DRStopCarSounds_original)(void);
int C2_HOOK_FASTCALL DRS3StopAllOutletSoundsExceptCDA(void) {
#if 0//defined(C2_HOOKS_ENABLED)
    return DRStopCarSounds_original();
#else

    if (!C2V(gSound_enabled)) {
        return 0;
    }
    DRS3StopOutletSound(C2V(gCar_outlet));
    DRS3StopOutletSound(C2V(gEngine_outlet));
    DRS3StopOutletSound(C2V(gXXX_outlet));
    DRS3StopOutletSound(C2V(gEffects_outlet));
    DRS3StopOutletSound(C2V(gPedestrians_outlet));
    return 1;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004559e0, DRS3StopAllOutletSoundsExceptCDA, DRStopCarSounds_original)

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

#if 0//defined(C2_HOOKS_ENABLED)
    InitSoundSources_original();
#else
    int toggle;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, sound_source, 0x1370);

    if (!C2V(gSound_available)) {
        return;
    }

    toggle = !C2V(gSound_enabled);
    if (toggle) {
        ToggleSoundEnable();
    }

    BrVector3Negate(&C2V(gCamera_left), (br_vector3*)C2V(gCamera_to_world).m[0]);
    BrVector3Copy(&C2V(gCamera_position), (br_vector3*)C2V(gCamera_to_world).m[3]);
    S3BindListenerPositionBRender(&C2V(gCamera_position));
    S3BindListenerVelocityBRender(&C2V(gCamera_velocity));
    S3BindListenerLeftBRender(&C2V(gCamera_left));
    if (!C2V(gSound_sources_inited)) {
        tVehicle_type category;
        int i;

        for (category = eVehicle_rozzer; category >= 0; category -= 1) {
            int car_count;

            car_count = category == eVehicle_self ? 1 : GetCarCount(category);
            for (i = 0; i < car_count; i++) {
                tCar_spec* car;

                PossibleService();
                car = category == eVehicle_self ? &C2V(gProgram_state).current_car : GetCarSpec(category, i);

                if ((car != NULL && car->driver == eDriver_local_human) || C2V(gSound_detail_level) > 2 || category == eVehicle_rozzer) {

                    car->sound_source = S3CreateSoundSourceBR(&car->pos, &car->vel, C2V(gEngine_outlet));
                    if (car->sound_source != NULL) {
                        S3BindAmbientSoundToOutlet(C2V(gEngine_outlet), category == eVehicle_rozzer ? eSoundId_Cop_Siren : car->engine_noises[0], car->sound_source, 250.f, 0, 0, 0, BR_FIXED_INT(1), BR_FIXED_INT(1));
                    }
                }
            }
        }

        C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gEnvironment_sound_sources)) != 5);
        C2_HOOK_BUG_ON(sizeof(C2V(gEnvironment_sound_sources)[0]) != 0x1c);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tEnvironment_sound_source, source, 0x0);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tEnvironment_sound_source, position, 0x4);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tEnvironment_sound_source, field_0x10, 0x10);

        for (i = 0; i < REC2_ASIZE(C2V(gEnvironment_sound_sources)); i++) {
            tEnvironment_sound_source* env_src;

            env_src = &C2V(gEnvironment_sound_sources)[i];
            env_src->field_0x10 = NULL;
            if (env_src->source == NULL) {
                env_src->source = S3CreateSoundSourceBR(&env_src->position, &C2V(gZero_v__car), C2V(gXXX_outlet));
                if (env_src->source != NULL) {
                    S3BindAmbientSoundToOutlet(C2V(gXXX_outlet), eSoundId_Cop_Siren, env_src->source, 100.f, 0, 0, 0, BR_FIXED_INT(1), BR_FIXED_INT(1));
                }
            }
        }
        C2V(gSound_sources_inited) = 1;
    }
    if (toggle) {
        ToggleSoundEnable();
    }

    C2V(gINT_0079e18c) = 0;
    C2V(gINT_0079e17c) = 0;
    C2V(gINT_00684540) = 0;
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

int C2_HOOK_FASTCALL DRS3SetOutletVolume(tS3_outlet* pOutlet, int pVolume) {

    if (!C2V(gSound_enabled)) {
        return 0;
    }
    return S3SetOutletVolume(pOutlet, pVolume);
}
C2_HOOK_FUNCTION(0x00455930, DRS3SetOutletVolume)

int (C2_HOOK_FASTCALL * DRS3ShutDown_original)(void);
int C2_HOOK_FASTCALL DRS3ShutDown(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    DRS3ShutDown_original();
#else

    if (!C2V(gSound_enabled)) {
        return 0;
    }
    C2V(gSound_enabled) = 0;
    return S3DisableSound();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00455910, DRS3ShutDown, DRS3ShutDown_original)

int C2_HOOK_FASTCALL DRS3SoundStillPlaying(int pTag) {

    if (!C2V(gSound_enabled)) {
        return 0;
    }

    return S3SoundStillPlaying(pTag);
}
C2_HOOK_FUNCTION(0x004558f0, DRS3SoundStillPlaying)

int C2_HOOK_FASTCALL DRS3SetVolume(int pVolume) {

    if (!C2V(gSound_enabled)) {
        return 0;
    }
    return S3SetVolume(pVolume);
}
C2_HOOK_FUNCTION(0x00455950, DRS3SetVolume)

void (C2_HOOK_FASTCALL * MungeEnvironmentalSound_original)(void);
void C2_HOOK_FASTCALL MungeEnvironmentalSound(void) {

#if defined(C2_HOOKS_ENABLED)
    MungeEnvironmentalSound_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00456e20, MungeEnvironmentalSound, MungeEnvironmentalSound_original)

void (C2_HOOK_FASTCALL * MungeEngineNoise_original)(void);
void C2_HOOK_FASTCALL MungeEngineNoise(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    MungeEngineNoise_original();
#else
    int cat;
    int stop_all;

    if (!C2V(gSound_available) || !C2V(gProgram_state).racing) {
        return;
    }
    BrVector3Copy(&C2V(gCamera_position), (br_vector3*)&C2V(gCamera_to_world).m[3][0]);
    BrVector3Negate(&C2V(gCamera_left), (br_vector3*)&C2V(gCamera_to_world).m[0][0]);
    if (C2V(gFrame_period) != 0) {
        BrVector3Sub(&C2V(gCamera_velocity), &C2V(gCamera_position), &C2V(gOld_camera_position));
        BrVector3InvScale(&C2V(gCamera_velocity), &C2V(gCamera_velocity), C2V(gFrame_period) / 1000.0f);
    } else {
        BrVector3Set(&C2V(gCamera_velocity), 0.0f, 0.0f, 0.0f);
    }
    BrVector3Copy(&C2V(gOld_camera_position), &C2V(gCamera_position));
    stop_all = C2V(gAction_replay_mode) && (fabsf(ARGetReplayRate()) > 1.0f || ARGetReplayRate() == 0.0f);
    for (cat = eVehicle_rozzer; cat >= 0; cat--) {
        int car_count;
        int i;

        if (cat == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(cat);
        }
        for (i = 0; i < car_count; i++) {
            tCar_spec* the_car;

            if (cat == eVehicle_self) {
                the_car = &C2V(gProgram_state).current_car;
            } else {
                the_car = GetCarSpec(cat, i);
            }
            if ((the_car != NULL && the_car->driver == eDriver_local_human)
                    || C2V(gSound_detail_level) > 2 || cat == eVehicle_rozzer) {
                float pitch;
                float vol;
                int type_of_engine_noise;

                BrVector3Copy(&the_car->vel, &the_car->collision_info->v);
                if (cat == eVehicle_rozzer) {
                    vol = 255.0f;
                    pitch = (float)BR_FIXED_INT(1);
                } else {
                    if (the_car->collision_info->last_special_volume != NULL) {
                        type_of_engine_noise = the_car->collision_info->last_special_volume->engine_noise_index;
                    } else {
                        type_of_engine_noise = 0;
                    }
                    pitch = 40960.0f + 10.0f * the_car->revs;
                    if (C2V(gAction_replay_mode)) {
                        pitch *= fabsf(ARGetReplayRate());
                    }
                    if (type_of_engine_noise == 1) {
                        pitch *= 0.75f;
                    } else if (type_of_engine_noise == 2) {
                        pitch *= 0.55f;
                    }

                    pitch = MAX(pitch, 4096.0f);
                    pitch = MIN(pitch, 131072.0f);

                    vol = 96.0f + the_car->revs * 0.0015f;
                    if (type_of_engine_noise == 1) {
                        vol *= 5.0f;
                    } else if (type_of_engine_noise == 2) {
                        vol *= 2.0f;
                    } else {
                        vol *= 2.5f;
                    }
                    vol = MIN(vol, 255);
                }
                if (the_car->field_0x4d4 > 1.0f) {
                    vol *= 1.0f + (the_car->field_0x4d4 - 1.0f) / 12.0f;
                }
                S3UpdateSoundSource(C2V(gEngine_outlet), -1, the_car->sound_source, (float)(C2V(gAction_replay_mode) ? 300 : 250), 0, 0, (int)vol, (int)pitch, BR_FIXED_INT(1));
            }
        }
    }
    SoundService();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00456070, MungeEngineNoise, MungeEngineNoise_original)

int C2_HOOK_FASTCALL DRS3StopSound(tS3_sound_tag pSound_tag) {

    if (!C2V(gSound_enabled)) {
        return 0;
    }
    return S3StopSound(pSound_tag);
}
C2_HOOK_FUNCTION(0x00455840, DRS3StopSound)

int C2_HOOK_FASTCALL DRS3StartSound3D(tS3_outlet* pOutlet, int pSound_id, const br_vector3* pInitial_position, const br_vector3* pInitial_velocity, int pRepeats, int pVolume, int pPitch, int pSpeed) {

    if (!C2V(gSound_enabled)) {
        return 0;
    }
    if (pVolume != 0
            && pSound_id != eSoundId_pratcam_whirr_noise
            && !(pSound_id >= 3000 && pSound_id <= 3007)
            && !(pSound_id >= 5300 && pSound_id <= 5320)) {
        PipeSingleSound(pOutlet, pSound_id, pVolume, 0, pPitch, pInitial_position);
    }
    return DRS3StartSound3D(pOutlet, pSound_id, pInitial_position, pInitial_velocity, pRepeats, pVolume, pPitch, pSpeed);
}
C2_HOOK_FUNCTION(0x00455f80, DRS3StartSound3D)
