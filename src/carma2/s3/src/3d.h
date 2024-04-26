#ifndef S3_3D_H
#define S3_3D_H

#include <s3/s3.h>

void C2_HOOK_FASTCALL S3StopSoundSource(tS3_sound_source* src);

void C2_HOOK_FASTCALL S3UpdateSoundSource(tS3_outlet* outlet, tS3_sound_tag tag, tS3_sound_source* src, float pMax_distance_squared, int pPeriod, tS3_repeats pAmbient_repeats, tS3_volume pVolume, int pPitch, tS3_speed pSpeed);

int C2_HOOK_STDCALL S3Set3DSoundEnvironment(float a1, float a2, float a3);

#endif // S3_3D_H
