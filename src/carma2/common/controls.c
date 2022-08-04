#include "controls.h"

#include "sound.h"

void C2_HOOK_FASTCALL SetSoundDetailLevel(int pLevel) {

    C2V(gSound_detail_level) = pLevel;
}
C2_HOOK_FUNCTION(0x00456950, SetSoundDetailLevel)

void (C2_HOOK_FASTCALL * ReallySetSoundDetailLevel_original)(int pLevel);
void C2_HOOK_FASTCALL ReallySetSoundDetailLevel(int pLevel) {

#if defined(C2_HOOKS_ENABLED)
    ReallySetSoundDetailLevel_original(pLevel);
#else
    DRS3StopAllOutletSounds();
    DisposeSoundSources();
    C2V(gSound_detail_level) = pLevel;
    InitSound();
    InitSoundSources();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00456960, ReallySetSoundDetailLevel, ReallySetSoundDetailLevel_original)

int C2_HOOK_FASTCALL GetSoundDetailLevel(void) {

    return C2V(gSound_detail_level);
}
C2_HOOK_FUNCTION(0x004569e0, GetSoundDetailLevel)
