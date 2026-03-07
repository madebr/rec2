#ifndef REC2_SOUND_H
#define REC2_SOUND_H

#include "c2_hooks.h"

#include "c2_stdio.h"

#include "rec2_types.h"

extern int gSound_detail_level;
extern int gCD_fully_installed;
extern int gSound_sources_inited;
extern tS3_outlet* gEffects_outlet;
extern tS3_outlet* gEngine_outlet;
extern int gMusic_available;
extern int gINT_00684554;
extern int gINT_00684568;
extern int gVirgin_pass;
extern int gOld_sound_detail_level;
extern int gCD_is_disabled;
extern tS3_outlet* gDriver_outlet;
extern tS3_outlet* gMusic_outlet;
extern tS3_outlet* gCar_outlet;
extern tS3_outlet* gPedestrians_outlet;
extern tS3_outlet* gXXX_outlet;
extern tS3_outlet* gIndexed_outlets[6];
extern const char* gSound_periodicity_choices[3];
extern br_vector3 gCamera_left;
extern br_vector3 gCamera_position;
extern br_vector3 gCamera_velocity;
extern tU32 gNext_sound_generator_munging;
extern int gCount_environmental_sound_sources;
extern int gEnvironmental_sound_sources_buffer_index;
extern tEnvironment_sound_source gEnvironment_sound_sources[5];
extern br_vector3 gZero_v__car;
extern br_vector3 gOld_camera_position;

void C2_HOOK_FASTCALL UsePathFileToDetermineIfFullInstallation(void);

void C2_HOOK_FASTCALL ReadSoundSpec(FILE* pF, tSpecial_volume_soundfx_data* pSpec);

void C2_HOOK_FASTCALL WriteOutSoundSpec(FILE* pF, tSpecial_volume_soundfx_data* pSpec);

void C2_HOOK_FASTCALL DoEnvSound(tSpecial_volume* pVolume, br_vector3* pP, tSpecial_volume_soundfx_type pType, tSpecial_volume_soundfx_data* pSound_data, float pSound_dist, br_vector3 *pSound_dir);

void C2_HOOK_FASTCALL DoAnEnvironmentalSound(void* pSrc_object, tEnvironment_sound_generator_info* pEnv_info, int pVolume, br_vector3* pPos);

void C2_HOOK_FASTCALL StartMusic(void);

void C2_HOOK_FASTCALL StopMusic(void);

tS3_sound_tag C2_HOOK_FASTCALL DRS3StartSound(tS3_outlet* pOutlet, tS3_sound_id pSound);

tS3_sound_tag C2_HOOK_FASTCALL DRS3StartSound2(tS3_outlet* pOutlet, tS3_sound_id pSound, tU32 pRepeats, int pLeft_volume, int pRight_volume, int pLeft_pitch, int pRight_pitch);

int C2_HOOK_FASTCALL DRS3StartSoundNoPiping(tS3_outlet* pOutlet, tS3_sound_id pSound);

tS3_outlet* C2_HOOK_FASTCALL GetOutletFromIndex(int pIndex);

int C2_HOOK_FASTCALL GetIndexFromOutlet(tS3_outlet* pOutlet);

int C2_HOOK_FASTCALL DRS3StartCDA(int pSound);

void C2_HOOK_FASTCALL InitSound(void);

void C2_HOOK_FASTCALL SetSoundVolumes(int pCD_audio);

void C2_HOOK_FASTCALL SoundService(void);

void C2_HOOK_FASTCALL DRS3Service(void);

int C2_HOOK_FASTCALL IsCDAPlaying(void);

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

intptr_t C2_HOOK_FASTCALL FoundSoundSource(br_actor* pActor, void* pContext);

void C2_HOOK_FASTCALL MungeSoundGenerators(void);

void C2_HOOK_FASTCALL MungeEnvironmentalSound(void);

void C2_HOOK_FASTCALL MungeEngineNoise(void);

int C2_HOOK_FASTCALL DRS3StopSound(tS3_sound_tag pSound_tag);

int C2_HOOK_FASTCALL DRS3StartSound3D(tS3_outlet* pOutlet, int pSound_id, const br_vector3* pInitial_position, const br_vector3* pInitial_velocity, int pRepeats, int pVolume, int pPitch, int pSpeed);

#endif // REC2_SOUND_H