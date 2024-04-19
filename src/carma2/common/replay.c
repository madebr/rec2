#include "replay.h"

#include "globvars.h"
#include "utility.h"

#include "c2_string.h"

C2_HOOK_VARIABLE_IMPLEMENT(tActionReplayCameraMode, gAction_replay_camera_mode, 0x0079efa8);

void C2_HOOK_FASTCALL DefaultQuicktimeSettings(void) {

    if (C2V(gQuick_time_quality)[0] == '\0') {
        c2_strcpy(C2V(gQuick_time_quality), "normal");
    }
    if (C2V(gQuick_time_compressor)[0] == '\0') {
        c2_strcpy(C2V(gQuick_time_compressor), "animation");
    }
    if (C2V(gQuick_time_temp_path)[0] == '\0') {
        PathCat(C2V(gQuick_time_temp_path), C2V(gApplication_path), "QTTMP");
    }
    if (C2V(gQuick_time_movie_path_stub)[0] == '\0') {
        PathCat(C2V(gQuick_time_temp_path), C2V(gApplication_path), "MOVIE");
    }
    if (C2V(gQuick_time_banner_texture_name)[0] == '\0') {
        c2_strcpy(C2V(gQuick_time_banner_texture_name), "ARBANNER.PIX");
    }
}
C2_HOOK_FUNCTION(0x004e1740, DefaultQuicktimeSettings)

void (C2_HOOK_FASTCALL * InitializeActionReplayCamera_original)(tActionReplayCameraMode pCamPos);
void C2_HOOK_FASTCALL InitializeActionReplayCamera(tActionReplayCameraMode pCamPos) {

#if defined(C2_HOOKS_ENABLED)
    InitializeActionReplayCamera_original(pCamPos);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0040e790, InitializeActionReplayCamera, InitializeActionReplayCamera_original)
