#ifndef S3_3D_H
#define S3_3D_H

#include <s3/s3.h>

C2_HOOK_VARIABLE_DECLARE(float, gFLOAT_006b2c6c);
C2_HOOK_VARIABLE_DECLARE(float, gFLOAT_006b2c4c);
C2_HOOK_VARIABLE_DECLARE(float, gFLOAT_006b2c68);
C2_HOOK_VARIABLE_DECLARE(br_vector3, gS3_listener_position_now);
C2_HOOK_VARIABLE_DECLARE(br_vector3, gS3_listener_vel_now);
C2_HOOK_VARIABLE_DECLARE(br_vector3, gS3_listener_left_now);

void C2_HOOK_FASTCALL S3StopSoundSource(tS3_sound_source* src);

void C2_HOOK_FASTCALL S3UpdateSoundSource(tS3_outlet* outlet, tS3_sound_tag tag, tS3_sound_source* src, float pMax_distance_squared, int pPeriod, tS3_repeats pAmbient_repeats, tS3_volume pVolume, int pPitch, tS3_speed pSpeed);

int C2_HOOK_STDCALL FUN_00565e39(float a1, float a2, float a3);

int C2_HOOK_STDCALL S3Set3DSoundEnvironment(float a1, float a2, float a3);

void C2_HOOK_FASTCALL S3CopyBrVector3(br_vector3* pDest, br_vector3* pSrc);

void C2_HOOK_FASTCALL S3BindListenerPositionBRender(br_vector3* pos);

void C2_HOOK_FASTCALL S3BindListenerVelocityBRender(br_vector3* vel);

void C2_HOOK_FASTCALL S3BindListenerLeftBRender(br_vector3* left);

int C2_HOOK_FASTCALL S3BindAmbientSoundToOutlet(tS3_outlet* pOutlet, int pSound, tS3_sound_source* source, float pMax_distance, int pPeriod, int pRepeats, int pVolume, int pPitch, int pSpeed);

#endif // S3_3D_H
