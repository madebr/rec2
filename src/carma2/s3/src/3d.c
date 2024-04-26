#include "audio.h"

#include <stddef.h>


void C2_HOOK_FASTCALL S3StopSoundSource(tS3_sound_source* src) {
    if (!C2V(gS3_enabled)) {
        return;
    }
    if (src == NULL) {
        return;
    }
    if (src->channel != NULL && src->tag != 0 && src->channel->tag == src->tag) {
        src->channel->termination_reason = eS3_tr_stopped;
        src->channel->spatial_sound = 0;
        S3StopChannel(src->channel);
        src->channel->sound_source_ptr = NULL;
    }
    src->ambient = 0;
}
C2_HOOK_FUNCTION(0x00567ab8, S3StopSoundSource)

void (C2_HOOK_FASTCALL * S3UpdateSoundSource_original)(tS3_outlet* outlet, tS3_sound_tag tag, tS3_sound_source* src, float pMax_distance_squared, int pPeriod, tS3_repeats pAmbient_repeats, tS3_volume pVolume, int pPitch, tS3_speed pSpeed);
void C2_HOOK_FASTCALL S3UpdateSoundSource(tS3_outlet* outlet, tS3_sound_tag tag, tS3_sound_source* src, float pMax_distance_squared, int pPeriod, tS3_repeats pAmbient_repeats, tS3_volume pVolume, int pPitch, tS3_speed pSpeed) {

#if defined(C2_HOOKS_ENABLED)
    S3UpdateSoundSource_original(outlet, tag, src, pMax_distance_squared, pPeriod, pAmbient_repeats, pVolume, pPitch, pSpeed);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00567875, S3UpdateSoundSource, S3UpdateSoundSource_original)

int (C2_HOOK_STDCALL * S3Set3DSoundEnvironment_original)(float a1, float a2, float a3);
int C2_HOOK_STDCALL S3Set3DSoundEnvironment(float a1, float a2, float a3) {

#if defined(C2_HOOKS_ENABLED)
    return S3Set3DSoundEnvironment_original(a1, a2, a3);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00565d70, S3Set3DSoundEnvironment, S3Set3DSoundEnvironment_original)
