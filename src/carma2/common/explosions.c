#include "explosions.h"

#include "animation.h"
#include "globvars.h"
#include "loading.h"
#include "utility.h"
#include "world.h"

#include "brender/brender.h"

#include "c2_hooks.h"
#include "c2_string.h"

void (C2_HOOK_FASTCALL * InitExplosions_original)(void);
void C2_HOOK_FASTCALL InitExplosions(void) {

#if defined(C2_HOOKS_ENABLED)
    InitExplosions_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ea880, InitExplosions, InitExplosions_original)

void C2_HOOK_FASTCALL ReadExplosionInfo(FILE* pF, int* pChance_explosion, int* pExplosion_sound_id, tExplosion_animation* pExplosion_groups) {
    tPath_name the_path;
    char* name;
    char s[256];

    /* Wasted explosion settings */

    /* Percentage chance of car exploding */
    *pChance_explosion = GetAnInt(pF);
    /* Sound ID */
    *pExplosion_sound_id = GetAnInt(pF);

    PathCat(the_path, C2V(gApplication_path), "COMMON");
    /* Name of pixelmap file */
    GetALineAndDontArgue(pF, s);
    name = c2_strtok(s, "\t ,/");
    name[c2_strlen(name) - 4] = '\0';
    PathCat(the_path, the_path, name);
    LoadAllTexturesFromTexSubdirectories(&C2V(gMisc_storage_space), the_path);
    ReadExplosionAnimation(pF, pExplosion_groups);
}
C2_HOOK_FUNCTION(0x004f5ec0, ReadExplosionInfo)
