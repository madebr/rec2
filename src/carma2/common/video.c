#include "video.h"

#include "globvars.h"

#include <Movies.h>
#include <QTML.h>

C2_HOOK_VARIABLE_IMPLEMENT(int, gQuickTime_initialized, 0x006a0c24);

void (C2_HOOK_CDECL * InitQuickTimeStuff_original)(void);
void C2_HOOK_CDECL InitQuickTimeStuff(void) {
#if 0//defined(C2_HOOKS_ENABLED)
    InitQuickTimeStuff_original();
#else
    C2V(gQuickTime_initialized) = 0;
    if (InitializeQTML(0) == noErr && EnterMovies() == noErr) {
        C2V(gQuickTime_initialized) = 1;
    }
    C2V(gQuick_time_temp_path)[0] = '\0';
    C2V(gQuick_time_movie_path_stub)[0] = '\0';
    C2V(gQuick_time_banner_texture_name)[0] = '\0';
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e1700, InitQuickTimeStuff, InitQuickTimeStuff_original)
