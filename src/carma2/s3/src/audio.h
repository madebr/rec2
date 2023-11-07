#ifndef S3_AUDIO_H
#define S3_AUDIO_H

#include <s3/s3.h>

C2_HOOK_VARIABLE_DECLARE(int, gS3_enabled);

int C2_HOOK_FASTCALL S3StopChannel(tS3_channel* chan);

int C2_HOOK_FASTCALL S3StopOutletSound(tS3_outlet* pOutlet);

#endif // S3_AUDIO_H
