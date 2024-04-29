#include "audio.h"

#include "sample.h"

#include "rec2_macros.h"

#include <math.h>
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

#if 0//defined(C2_HOOKS_ENABLED)
    S3UpdateSoundSource_original(outlet, tag, src, pMax_distance_squared, pPeriod, pAmbient_repeats, pVolume, pPitch, pSpeed);
#else
    tS3_channel* channel;
    int is_sample;

    is_sample = 0;
    if (!C2V(gS3_enabled)) {
        return;
    }
    if (src == NULL) {
        return;
    }
    channel = src->channel;
    if (tag != -1 && src->sound_id != tag) {
        tS3_descriptor* desc = S3GetDescriptorByID(tag);
        if (desc == NULL) {
            return;
        }
        if (desc->type == 0) {
            src->sound_id = tag;
            if (desc->buffer_description == NULL || (desc->flags & 0x2)) {
                if (S3LoadSample(tag) == eS3_error_none) {
                    return;
                }
            }
            is_sample = 1;
        }
    }
    if (pSpeed != -1) {
        if (pSpeed <= 0) {
            pSpeed = 0x10000;
        }
        src->speed = pSpeed;
    }
    if (pPitch != -1) {
        if (pPitch <= 0) {
            pPitch = 0x10000;
        }
        src->pitch = pPitch;
        if (channel != NULL && channel->descriptor != NULL && channel->descriptor->type == 0) {
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_channel, source_rate, 0x54);

            channel->source_rate = S3IRandomBetweenLog(channel->descriptor->min_pitch, channel->descriptor->max_pitch, channel->descriptor->buffer_description->sample_rate);
            channel->source_rate = (int)((float)channel->source_rate * ldexpf((float)src->pitch, -16));
        }
    }
    if (pAmbient_repeats != -1) {
        src->ambient_repeats = MAX(0, pAmbient_repeats);
        if (channel != NULL) {
            channel->repetitions = src->ambient_repeats;
        }
    }

    if (pMax_distance_squared != -1.0f) {
        src->max_distance_sq = pMax_distance_squared;
        if (channel != NULL) {
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_channel, max_distance_squared, 0x58);

            channel->max_distance_squared = pMax_distance_squared;
        }
    }
    if (pPeriod != -1) {
        src->period = pPeriod;
    }
    if (pVolume != -1) {
        if (pVolume > 255) {
            pVolume = 255;
        }
        if (pVolume < 0) {
            pVolume = 0;
        }
        if (src->volume != pVolume) {
            src->volume = pVolume;
        }
        if (channel != NULL) {
            channel->volume_multiplier = pVolume;
        }
    }
    if (channel != NULL) {
        if (channel->sound_source_ptr != src) {
            S3StopChannel(channel);
        }
        if (pVolume == 0 || is_sample) {
            channel->spatial_sound = 0;
            channel->needs_service = 1;
            S3StopChannel(channel);
            src->channel = NULL;
            src->tag = 0;
        }
    }
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
