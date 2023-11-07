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
