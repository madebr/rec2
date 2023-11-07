#include "audio.h"

#include <stddef.h>

int (C2_HOOK_FASTCALL * S3StopChannel_original)(tS3_channel* chan);
int C2_HOOK_FASTCALL S3StopChannel(tS3_channel* chan) {

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
