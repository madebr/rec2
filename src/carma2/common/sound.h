#ifndef REC2_SOUND_H
#define REC2_SOUND_H

#include "c2_hooks.h"

#include "c2_stdio.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(int, gSound_detail_level);
C2_HOOK_VARIABLE_DECLARE(int, gCD_fully_installed);
C2_HOOK_VARIABLE_DECLARE(int, gSound_sources_inited);
C2_HOOK_VARIABLE_DECLARE(tS3_outlet*, gEffects_outlet);
C2_HOOK_VARIABLE_DECLARE(tS3_outlet*, gEngine_outlet);
C2_HOOK_VARIABLE_DECLARE(int, gMusic_available);
C2_HOOK_VARIABLE_DECLARE(int, gINT_00684554);
C2_HOOK_VARIABLE_DECLARE(int, gINT_00684568);
C2_HOOK_VARIABLE_DECLARE(int, gVirgin_pass);
C2_HOOK_VARIABLE_DECLARE(int, gOld_sound_detail_level);
C2_HOOK_VARIABLE_DECLARE(int, gCD_is_disabled);
C2_HOOK_VARIABLE_DECLARE(tS3_outlet*, gDriver_outlet);
C2_HOOK_VARIABLE_DECLARE(tS3_outlet*, gMusic_outlet);
C2_HOOK_VARIABLE_DECLARE(tS3_outlet*, gCar_outlet);
C2_HOOK_VARIABLE_DECLARE(tS3_outlet*, gPedestrians_outlet);
C2_HOOK_VARIABLE_DECLARE(tS3_outlet*, gXXX_outlet);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tS3_outlet*, gIndexed_outlets, 6);
C2_HOOK_VARIABLE_DECLARE_ARRAY(const char*, gSound_periodicity_choices, 3);
C2_HOOK_VARIABLE_DECLARE(br_vector3, gCamera_left);
C2_HOOK_VARIABLE_DECLARE(br_vector3, gCamera_position);
C2_HOOK_VARIABLE_DECLARE(br_vector3, gCamera_velocity);
C2_HOOK_VARIABLE_DECLARE(int, gINT_00684540);
C2_HOOK_VARIABLE_DECLARE(int, gINT_0079e18c);
C2_HOOK_VARIABLE_DECLARE(int, gINT_0079e17c);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tEnvironment_sound_source, gEnvironment_sound_sources, 5);
C2_HOOK_VARIABLE_DECLARE(br_vector3, gZero_v__car);

void C2_HOOK_FASTCALL UsePathFileToDetermineIfFullInstallation(void);

void C2_HOOK_FASTCALL ReadSoundSpec(FILE* pF, tSpecial_volume_soundfx_data* pSpec);

void C2_HOOK_FASTCALL WriteOutSoundSpec(FILE* pF, tSpecial_volume_soundfx_data* pSpec);

void C2_HOOK_FASTCALL StopMusic(void);

tS3_sound_tag C2_HOOK_FASTCALL DRS3StartSound(tS3_outlet* pOutlet, tS3_sound_id pSound);

void C2_HOOK_FASTCALL InitSound(void);

void C2_HOOK_FASTCALL SetSoundVolumes(int pCD_audio);

void C2_HOOK_FASTCALL SoundService(void);

int C2_HOOK_FASTCALL DRS3StopAllOutletSoundsExceptCDA(void);

int C2_HOOK_FASTCALL DRS3StopOutletSound(tS3_outlet* pOutlet);

void C2_HOOK_FASTCALL ToggleSoundEnable(void);

void C2_HOOK_FASTCALL DisposeSoundSources(void);

void C2_HOOK_FASTCALL InitSoundSources(void);

void C2_HOOK_FASTCALL StartMusicTrack(int pMusic_track);

int C2_HOOK_FASTCALL DRS3SetOutletVolume(tS3_outlet* pOutlet, int pVolume);

int C2_HOOK_FASTCALL DRS3ShutDown(void);

int C2_HOOK_FASTCALL DRS3SoundStillPlaying(int pTag);

int C2_HOOK_FASTCALL DRS3SetVolume(int pVolume);

void C2_HOOK_FASTCALL MungeEnvironmentalSound(void);

void C2_HOOK_FASTCALL MungeEngineNoise(void);

#endif // REC2_SOUND_H
