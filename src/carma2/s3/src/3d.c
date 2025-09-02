#include "3d.h"

#include "audio.h"
#include "sample.h"

#include "platform.h"

#include "rec2_macros.h"

#include <math.h>
#include <stddef.h>
#include <string.h>

C2_HOOK_VARIABLE_IMPLEMENT(float, gFLOAT_006b2c6c, 0x006b2c6c);
C2_HOOK_VARIABLE_IMPLEMENT(float, gFLOAT_006b2c4c, 0x006b2c4c);
C2_HOOK_VARIABLE_IMPLEMENT(float, gFLOAT_006b2c68, 0x006b2c68);
C2_HOOK_VARIABLE_IMPLEMENT(br_vector3, gS3_listener_position_now, 0x006b2c20);
C2_HOOK_VARIABLE_IMPLEMENT(br_vector3, gS3_listener_vel_now, 0x006b2c58);
C2_HOOK_VARIABLE_IMPLEMENT(br_vector3, gS3_listener_left_now, 0x006b2c30);
C2_HOOK_VARIABLE_IMPLEMENT(int, gS3_listener_pos_is_brender, 0x006b2c50);
C2_HOOK_VARIABLE_IMPLEMENT(br_vector3*, gS3_listener_pos_ptr, 0x006b2c70);
C2_HOOK_VARIABLE_IMPLEMENT(br_vector3, gS3_listener_position_old, 0x006b2c40);
C2_HOOK_VARIABLE_IMPLEMENT(br_vector3*, gS3_listener_vel_ptr, 0x006b2c74);
C2_HOOK_VARIABLE_IMPLEMENT(int, gS3_listener_vel_is_brender, 0x006b2c64);
C2_HOOK_VARIABLE_IMPLEMENT(br_vector3*, gS3_listener_left_ptr, 0x006b2c78);
C2_HOOK_VARIABLE_IMPLEMENT(int, gS3_listener_left_is_brender, 0x006b2c54);


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

int C2_HOOK_STDCALL FUN_00565e39(float a1, float a2, float a3) {

    if (a1 == -1.f) {
        a1 = .25f;
    }
    if (a2 == -1.f) {
        a2 = 1.2f;
    }
    if (a3 == -1.f) {
        a3 = 130000.f;
    }
    C2V(gFLOAT_006b2c6c) = a1;
    C2V(gFLOAT_006b2c4c) = sqrtf(a3 / a2);
    C2V(gFLOAT_006b2c68) = C2V(gFLOAT_006b2c4c) * C2V(gFLOAT_006b2c6c);
    return 0;
}
C2_HOOK_FUNCTION(0x00565e39, FUN_00565e39)

int (C2_HOOK_STDCALL * S3Set3DSoundEnvironment_original)(float a1, float a2, float a3);
int C2_HOOK_STDCALL S3Set3DSoundEnvironment(float a1, float a2, float a3) {

#if 0//defined(C2_HOOKS_ENABLED)
    return S3Set3DSoundEnvironment_original(a1, a2, a3);
#else

    FUN_00565e39(a1, a2, a3);
    BrVector3Set(&C2V(gS3_listener_position_now), 0.f, 0.f, 0.f);
    BrVector3Set(&C2V(gS3_listener_vel_now), 0.f, 0.f, 0.f);
    BrVector3Set(&C2V(gS3_listener_left_now), 0.f, 0.f, 0.f);
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00565d70, S3Set3DSoundEnvironment, S3Set3DSoundEnvironment_original)

void C2_HOOK_FASTCALL S3CopyBrVector3(br_vector3* pDest, br_vector3* pSrc) {

    BrVector3Copy(pDest, pSrc);
}
C2_HOOK_FUNCTION(0x00565ec7, S3CopyBrVector3)

void C2_HOOK_FASTCALL S3CopyS3Vector3(tS3_vector3* pDest, tS3_vector3* pSrc) {

    pDest->x = pSrc->x;
    pDest->y = pSrc->y;
    pDest->z = pSrc->z;
}
C2_HOOK_FUNCTION(0x0056770a, S3CopyS3Vector3)

void C2_HOOK_FASTCALL S3BindListenerPositionBRender(br_vector3* pos) {

    C2V(gS3_listener_pos_is_brender) = 1;
    C2V(gS3_listener_pos_ptr) = pos;
    S3CopyBrVector3(&C2V(gS3_listener_position_old), pos);
}
C2_HOOK_FUNCTION(0x00565faa, S3BindListenerPositionBRender)

void C2_HOOK_FASTCALL S3BindListenerVelocityBRender(br_vector3* vel) {

    C2V(gS3_listener_vel_ptr) = vel;
    C2V(gS3_listener_vel_is_brender) = 1;
}
C2_HOOK_FUNCTION(0x00565ff1, S3BindListenerVelocityBRender)

void C2_HOOK_FASTCALL S3BindListenerLeftBRender(br_vector3* left) {

    C2V(gS3_listener_left_ptr) = left;
    C2V(gS3_listener_left_is_brender) = 1;
}
C2_HOOK_FUNCTION(0x0056602b, S3BindListenerLeftBRender)

int C2_HOOK_FASTCALL S3BindAmbientSoundToOutlet(tS3_outlet* pOutlet, int pSound, tS3_sound_source* source, float pMax_distance, int pPeriod, int pRepeats, int pVolume, int pPitch, int pSpeed) {
    tS3_descriptor* desc;

    if (!C2V(gS3_enabled)) {
        return eS3_error_none;
    }
    if (source == NULL) {
        return eS3_error_nonexistant_source;
    }
    desc = S3GetDescriptorByID(pSound);
    if (desc == NULL) {
        return eS3_error_bad_id;
    }
    if (desc->type != 0) {
        return eS3_error_none;
    }
    if ((desc->buffer_description == NULL || (desc->flags & 2) != 0) && S3LoadSample(pSound) == eS3_error_none) {
        return eS3_error_load_sound;
    }
    if (pVolume > 255) {
        pVolume = 255;
    }
    if (pVolume < 0) {
        pVolume = 128;
    }
    if (pPitch < 0) {
        pPitch = BR_FIXED_INT(1);
    }
    if (pSpeed < 0) {
        pSpeed = BR_FIXED_INT(1);
    }
    source->bound_outlet = pOutlet;
    source->sound_id = pSound;
    source->volume = pVolume;
    source->max_distance_sq = pMax_distance;
    source->period = pPeriod;
    source->pitch = pPitch;
    source->speed = pSpeed;
    source->ambient = 1;

    source->ambient_repeats = MAX(0, pRepeats);
    source->time_since_last_played = pPeriod;
    source->channel = NULL;
    source->tag = 0;
    return eS3_error_none;
}
C2_HOOK_FUNCTION(0x0056773c, S3BindAmbientSoundToOutlet)

void C2_HOOK_FASTCALL S3CopyVector3(void* pDest, void* pSrc, int pIs_BRender) {

    if (pIs_BRender) {
        S3CopyBrVector3(pDest, pSrc);
    } else {
        S3CopyS3Vector3(pDest, pSrc);
    }
}
C2_HOOK_FUNCTION(0x005676da, S3CopyVector3)

void C2_HOOK_FASTCALL S3UpdateListenerVectors(void) {

    if (C2V(gS3_listener_pos_ptr) != NULL) {
        S3CopyVector3(&C2V(gS3_listener_position_now), C2V(gS3_listener_pos_ptr), C2V(gS3_listener_pos_is_brender));
    }
    if (C2V(gS3_listener_vel_ptr) != NULL) {
        S3CopyVector3(&C2V(gS3_listener_vel_now), C2V(gS3_listener_vel_ptr), C2V(gS3_listener_vel_is_brender));
    } else {
        C2V(gS3_listener_vel_now).v[0] = (C2V(gS3_listener_position_now).v[0] - C2V(gS3_listener_position_old).v[0]) * (float)C2V(gS3_delta_time) / 1000.f;
        C2V(gS3_listener_vel_now).v[1] = (C2V(gS3_listener_position_now).v[1] - C2V(gS3_listener_position_old).v[1]) * (float)C2V(gS3_delta_time) / 1000.f;
        C2V(gS3_listener_vel_now).v[2] = (C2V(gS3_listener_position_now).v[2] - C2V(gS3_listener_position_old).v[2]) * (float)C2V(gS3_delta_time) / 1000.f;
        BrVector3Copy(&C2V(gS3_listener_position_old), &C2V(gS3_listener_position_now));
    }
    if (C2V(gS3_listener_left_ptr) != NULL) {
        S3CopyVector3(&C2V(gS3_listener_left_now), C2V(gS3_listener_left_ptr), C2V(gS3_listener_left_is_brender));
    } else {
        C2V(gS3_listener_left_now).v[0] = C2V(gS3_listener_position_now).v[0] - 1.f;
        C2V(gS3_listener_left_now).v[1] = C2V(gS3_listener_position_now).v[1];
        C2V(gS3_listener_left_now).v[2] = C2V(gS3_listener_position_now).v[2];
    }
}
C2_HOOK_FUNCTION(0x005675ce, S3UpdateListenerVectors)

int (C2_HOOK_FASTCALL * S3UpdateSourcePosition_original)(tS3_sound_source *pSource);
int C2_HOOK_FASTCALL S3UpdateSourcePosition(tS3_sound_source *pSource) {

#if 0//defined(C2_HOOKS_ENABLED)
    return S3UpdateSourcePosition_original(pSource);
#else
    if (pSource == NULL) {
        return 0;
    }
    if (C2V(gS3_enabled) && pSource->ambient) {
        tS3_outlet* outlet;
        tS3_descriptor* descriptor;

        outlet = pSource->bound_outlet;
        descriptor = S3GetDescriptorByID(pSource->sound_id);
        if (descriptor == NULL) {
            C2V(gS3_last_error) = eS3_error_bad_id;
            return eS3_error_none;
        }
        if (descriptor->buffer_description == NULL) {
            return eS3_error_nonexistant_source;
        }
        if (descriptor->type != 0) {
            return eS3_error_none;
        }
        memset(&C2V(gS3_channel_template), 0, sizeof(tS3_channel));
        C2V(gS3_channel_template).source_volume = pSource->volume;
        C2V(gS3_channel_template).rate = S3IRandomBetweenLog(descriptor->min_pitch,
                descriptor->max_pitch,
                descriptor->buffer_description->sample_rate);
        if (pSource->pitch < 0) {
            pSource->pitch = BR_FIXED_INT(1);
        }
        if (pSource->speed < 0) {
            pSource->speed = BR_FIXED_INT(1);
        }
        C2V(gS3_channel_template).rate = (int)(ldexpf((float)pSource->pitch, -16) * (float)C2V(gS3_channel_template).rate);
        if (!outlet->independent_pitch) {
            C2V(gS3_channel_template).rate = (int)(ldexpf((float)pSource->speed, -16) * (float)C2V(gS3_channel_template).rate);
        }
        C2V(gS3_channel_template).source_rate = C2V(gS3_channel_template).rate;
        C2V(gS3_channel_template).sound_source_ptr = pSource;
        if (pSource->velocity_ptr == NULL) {
            S3CopyVector3(&C2V(gS3_channel_template).prev_position, pSource->position_ptr, pSource->brender_vector);
        }
        C2V(gS3_channel_template).max_distance_squared = pSource->max_distance_sq;
        if (S3Calculate3D(&C2V(gS3_channel_template), 1) != 0) {
            tS3_channel* channel;

            channel = S3AllocateChannel(outlet, S3CalculatePriority(C2V(gS3_channel_template).volume_multiplier, descriptor->priority));
            if (channel == NULL) {
                C2V(gS3_last_error) = eS3_error_channel_alloc;
                pSource->tag = 0;
                pSource->channel = NULL;
                return eS3_error_none;
            }
            if ((descriptor->buffer_description == NULL || (descriptor->flags & 0x2)) && S3LoadSample(pSource->sound_id) == 0) {
                C2V(gS3_last_error) = eS3_error_load_sound;
                channel->needs_service = 1;
                return eS3_error_none;
            }
            channel->volume_multiplier = C2V(gS3_channel_template).volume_multiplier;
            channel->field_0x28 = C2V(gS3_channel_template).field_0x28;
            channel->rate = C2V(gS3_channel_template).rate;
            channel->position.x = C2V(gS3_channel_template).position.x;
            channel->position.y = C2V(gS3_channel_template).position.y;
            channel->position.z = C2V(gS3_channel_template).position.z;
            channel->velocity.x = C2V(gS3_channel_template).velocity.x;
            channel->velocity.y = C2V(gS3_channel_template).velocity.y;
            channel->velocity.z = C2V(gS3_channel_template).velocity.z;
            channel->prev_position.x = C2V(gS3_channel_template).prev_position.x;
            channel->prev_position.y = C2V(gS3_channel_template).prev_position.y;
            channel->prev_position.z = C2V(gS3_channel_template).prev_position.z;
            channel->source_volume = C2V(gS3_channel_template).source_volume;
            channel->source_rate = C2V(gS3_channel_template).source_rate;
            channel->max_distance_squared = C2V(gS3_channel_template).max_distance_squared;
            channel->spatial_sound = 2;
            channel->sound_source_ptr = pSource;
            channel->descriptor = descriptor;
            channel->tag = S3GenerateTag(outlet);
            channel->repetitions = pSource->ambient_repeats;
            channel->needs_service = 0;
            channel->termination_reason = 0;
            S3ExecuteSampleFilterFuncs(channel);
            if (S3PlaySample(channel) == 0) {
                channel->needs_service = 1;
                C2V(gS3_last_error) = eS3_error_start_sound;
                return 0;
            }
            pSource->tag = channel->tag;
            pSource->channel = channel;
            return channel->tag;
        }
    }
    pSource->tag = 0;
    pSource->channel = NULL;
    return eS3_error_none;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00567b2d, S3UpdateSourcePosition, S3UpdateSourcePosition_original)

int C2_HOOK_FASTCALL S3UpdateSourceVectors(void) {
    tS3_sound_source* source;

    for (source = C2V(gS3_sound_sources); source != NULL; source = source->next) {
        if (!source->ambient) {
            continue;
        }
        if (source->period > 0) {
            source->time_since_last_played += C2V(gS3_delta_time);
        }
        if (source->channel != NULL && source->tag != source->channel->tag) {
            S3StopChannel(source->channel);
            source->channel = NULL;
            source->tag = 0;
        }
        if (source->channel == NULL) {
            if (source->period < source->time_since_last_played && source->period != 0 && source->tag == 0) {
                if (source->volume > 0) {
                    if (S3UpdateSourcePosition(source) == 0) {
                        source->channel = NULL;
                        source->tag = 0;
                    }
                }
                source->time_since_last_played = 0;
            }
            else if ((!source->ambient_repeats || source->period == 0) && source->tag == 0) {
                if (source->volume > 0) {
                    if (S3UpdateSourcePosition(source) == 0) {
                        source->channel = NULL;
                        source->tag = 0;
                    }
                }
                source->time_since_last_played = 0;
            }
        }
    }
    return 0;
}
C2_HOOK_FUNCTION(0x00567ea5, S3UpdateSourceVectors)

int (C2_HOOK_FASTCALL * S3Calculate3D_original)(tS3_channel* pChannel, int pAmbient);
int C2_HOOK_FASTCALL S3Calculate3D(tS3_channel* pChannel, int pAmbient) {
#if 0//defined(C2_HOOKS_ENABLED)
    return S3Calculate3D_original(pChannel, pAmbient);
#else
    float dist_squared;
    float dist;
    float sound_volume_factor;
    br_vector3 dir_pos_listener;
    br_vector3 dir_left;

    if (pChannel->sound_source_ptr != NULL) {
        tS3_sound_source* sound_source = pChannel->sound_source_ptr;

        if (sound_source->position_ptr != NULL) {
            S3CopyVector3(&pChannel->position, sound_source->position_ptr, sound_source->brender_vector);
        }
        if (sound_source->velocity_ptr != NULL) {
            S3CopyVector3(&pChannel->velocity, sound_source->velocity_ptr, sound_source->brender_vector);
        } else {
            pChannel->velocity.x = (pChannel->position.x - pChannel->prev_position.x) * (float)C2V(gS3_delta_time) / 1000.f;
            pChannel->velocity.y = (pChannel->position.y - pChannel->prev_position.y) * (float)C2V(gS3_delta_time) / 1000.f;
            pChannel->velocity.z = (pChannel->position.z - pChannel->prev_position.z) * (float)C2V(gS3_delta_time) / 1000.f;
            pChannel->prev_position.x = pChannel->position.x;
            pChannel->prev_position.y = pChannel->position.y;
            pChannel->prev_position.z = pChannel->position.z;
        }
    }
    dist_squared =
              (pChannel->position.x - C2V(gS3_listener_position_now).v[0]) * (pChannel->position.x - C2V(gS3_listener_position_now).v[0])
            + (pChannel->position.y - C2V(gS3_listener_position_now).v[1]) * (pChannel->position.y - C2V(gS3_listener_position_now).v[1])
            + (pChannel->position.z - C2V(gS3_listener_position_now).v[2]) * (pChannel->position.z - C2V(gS3_listener_position_now).v[2]);

    if (dist_squared < 0.f) {
        dist_squared = -dist_squared;
    }
    if (dist_squared > pChannel->max_distance_squared) {
        return 0;
    }
    dist = dist_squared != 0.f ? sqrtf(dist_squared) : 0.f;
    if (pAmbient && dist != 0.f) {
        float doppler;

        doppler = 1.f -
                ( (pChannel->velocity.x - C2V(gS3_listener_vel_now).v[0]) * (pChannel->position.x - C2V(gS3_listener_position_now).v[0])
                + (pChannel->velocity.y - C2V(gS3_listener_vel_now).v[1]) * (pChannel->position.y - C2V(gS3_listener_position_now).v[1])
                + (pChannel->velocity.z - C2V(gS3_listener_vel_now).v[2]) * (pChannel->position.z - C2V(gS3_listener_position_now).v[2])) / dist / C2V(gFLOAT_006b2c68);
        if (doppler > 2.f) {
            doppler = 2.f;
        } else if (doppler < .5f) {
            doppler = .5f;
        }
        pChannel->rate = (int)((float)pChannel->source_rate * doppler);
    } else {
        pChannel->rate = pChannel->source_rate;
    }
    sound_volume_factor = 1.f / (1.f + dist / 6.f);
    if (!C2V(gS3_inside_cockpit)) {
        sound_volume_factor *= 1.3f;
    }
    dir_pos_listener.v[0] = pChannel->position.x - C2V(gS3_listener_position_now).v[0];
    dir_pos_listener.v[1] = pChannel->position.y - C2V(gS3_listener_position_now).v[1];
    dir_pos_listener.v[2] = pChannel->position.z - C2V(gS3_listener_position_now).v[2];
    dir_left.v[0] = C2V(gS3_listener_left_now).v[0];
    dir_left.v[1] = C2V(gS3_listener_left_now).v[1];
    dir_left.v[2] = C2V(gS3_listener_left_now).v[2];
    BrVector3Normalise(&dir_left, &dir_left);
    BrVector3Normalise(&dir_pos_listener, &dir_pos_listener);
    pChannel->field_0x28 = -BrVector3Dot(&dir_left, &dir_pos_listener);
    pChannel->volume_multiplier = (int)((float)pChannel->source_volume * sound_volume_factor);
    if (pChannel->volume_multiplier < 0) {
        pChannel->volume_multiplier = 0;
    }
    if (pChannel->volume_multiplier > 255) {
        pChannel->volume_multiplier = 255;
    }
    return 1;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0056711c, S3Calculate3D, S3Calculate3D_original)

int C2_HOOK_FASTCALL S3Service3D(tS3_channel* pChannel) {
    int updated;

    updated = S3Calculate3D(pChannel, pChannel->sound_source_ptr != NULL && !!pChannel->sound_source_ptr->ambient);
    if (updated) {
        PDS3UpdateChannelVolume(pChannel);
        PDS3UpdateChannelFrequency(pChannel);
    }
    return updated;
}
C2_HOOK_FUNCTION(0x00567575, S3Service3D)
