#include "audio.h"

#include "3d.h"
#include "resource.h"

#include <stddef.h>

C2_HOOK_VARIABLE_IMPLEMENT(int, gS3_enabled, 0x007a06c0);
C2_HOOK_VARIABLE_IMPLEMENT(tS3_sound_source*, gS3_sound_sources, 0x007a0590);
C2_HOOK_VARIABLE_IMPLEMENT(int, gS3_nsound_sources, 0x007a0584);
C2_HOOK_VARIABLE_IMPLEMENT(tS3_outlet*, gS3_outlets, 0x007a058c);

int (C2_HOOK_FASTCALL * S3Init_original)(const char* pPath, int pLow_memory_mode, const char* pSound_path);
int C2_HOOK_FASTCALL S3Init(const char* pPath, int pLow_memory_mode, const char* pSound_path) {

#if defined(C2_HOOKS_ENABLED)
    return S3Init_original(pPath, pLow_memory_mode, pSound_path);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005651d0, S3Init, S3Init_original)

void C2_HOOK_FASTCALL S3Enable(void) {

    C2V(gS3_enabled) = 1;
}
C2_HOOK_FUNCTION(0x00564ce7, S3Enable)

void C2_HOOK_FASTCALL S3Disable(void) {

    S3StopAllOutletSounds();
    C2V(gS3_enabled) = 0;
}
C2_HOOK_FUNCTION(0x00564cf6, S3Disable)

int (C2_HOOK_FASTCALL * S3StopChannel_original)(tS3_channel* chan);
int C2_HOOK_FASTCALL S3StopChannel(tS3_channel* chan) {

    C2_HOOK_BUG_ON(sizeof(tS3_channel) != 120);

#if defined(C2_HOOKS_ENABLED)
    return S3StopChannel_original(chan);
#else
    if (chan-> == 0) {
        return eS3_error_bad_stag;
    }
    chan->termination_reason = eS3_tr_stopped;
    if (chan->active) {
        chan->needs_service = 1;
    }
    if (chan->type == eS3_ST_sample) {
        if (chan->sound_source_ptr != NULL) {
            chan->sound_source_ptr->tag = 0;
            chan->sound_source_ptr->channel = 0;
            chan->sound_source_ptr->volume = 0;
        }
        if (S3StopSample(chan) == 0) {
            return eS3_error_function_failed;
        }
    } else if (chan->type == eS3_ST_midi) {
        if (S3StopMIDI(chan) != 0) {
            return eS3_error_function_failed;
        }
    } else if (chan->type == eS3_ST_cda) {
        if (S3StopCDA(chan) != 0) {
            return eS3_error_function_failed;
        }
    }

    if ((chan->descriptor->flags & 2) != 0) {
        S3ReleaseSound(chan->descriptor->id);
    }
    chan->repetitions = 1;
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00564bfd, S3StopChannel, S3StopChannel_original)

tS3_outlet* (C2_HOOK_FASTCALL * S3CreateOutlet_original)(int pCount_channels_1, int pCount_channels_2);
tS3_outlet* C2_HOOK_FASTCALL S3CreateOutlet(int pCount_channels_1, int pCount_channels_2) {

#if defined(C2_HOOKS_ENABLED)
    return S3CreateOutlet_original(pCount_channels_1, pCount_channels_2);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005653c7, S3CreateOutlet, S3CreateOutlet_original)

int C2_HOOK_FASTCALL S3StopOutletSound(tS3_outlet* pOutlet) {
    tS3_channel* c;

    if (!C2V(gS3_enabled)) {
        return 0;
    }
    for (c = pOutlet->channel_list; c != NULL; c = c->next) {
        // FUN_006a280(c);
        if (c->active) {
            c->spatial_sound = 0;
            S3StopChannel(c);
            c->needs_service = 1;
        }
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0056575b, S3StopOutletSound)

int C2_HOOK_FASTCALL S3ReleaseSoundSource(tS3_sound_source* src) {
    tS3_sound_source* prev;
    tS3_sound_source* next;

    C2_HOOK_BUG_ON(sizeof(tS3_sound_source) != 68);

    if (!C2V(gS3_enabled)) {
        return 0;
    }

    if (src == NULL) {
        return 0;
    }
    prev = src->prev;
    next = src->next;
    if (prev != NULL) {
        prev->next = next;
    } else {
        C2V(gS3_sound_sources) = src->next;
    }
    if (next != NULL) {
        next->prev = prev;
    }
    if (C2V(gS3_nsound_sources) != 0) {
        C2V(gS3_nsound_sources)--;
        if (C2V(gS3_nsound_sources) == 0) {
            C2V(gS3_sound_sources) = NULL;
        }
    }
    S3StopSoundSource(src);
    S3MemFree(src);
    return 0;
}
C2_HOOK_FUNCTION(0x0056671e, S3ReleaseSoundSource)

void C2_HOOK_FASTCALL S3StopAllOutletSounds(void) {
    tS3_outlet* o;

    if (!C2V(gS3_enabled)) {
        return;
    }

    for (o = C2V(gS3_outlets); o != NULL; o = o->next) {
        S3StopOutletSound(o);
    }
}
C2_HOOK_FUNCTION(0x005657bd, S3StopAllOutletSounds)

int (C2_HOOK_FASTCALL * S3StopSound_original)(int pTag);
int C2_HOOK_FASTCALL S3StopSound(int pTag) {

#if defined(C2_HOOKS_ENABLED)
    return S3StopSound_original(pTag);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00564ae2, S3StopSound, S3StopSound_original)

void (C2_HOOK_FASTCALL * S3Service_original)(int inside_cockpit, int unk1);
void C2_HOOK_FASTCALL S3Service(int inside_cockpit, int unk1) {

#if defined(C2_HOOKS_ENABLED)
    S3Service_original(inside_cockpit, unk1);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00564358, S3Service, S3Service_original)

void (C2_HOOK_FASTCALL * S3StopAll_original)(void);
void C2_HOOK_FASTCALL S3StopAll(void) {

#if defined(C2_HOOKS_ENABLED)
    S3StopAll_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00455910, S3StopAll, S3StopAll_original)
