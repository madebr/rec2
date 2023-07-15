#include "explosions.h"

#include "animation.h"
#include "globvars.h"
#include "graphics.h"
#include "loading.h"
#include "utility.h"
#include "world.h"

#include "brender/brender.h"

#include "c2_hooks.h"
#include "c2_string.h"

#include "rec2_macros.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tExplosion, gExplosions, 50, 0x006a55c8);

void (C2_HOOK_FASTCALL * InitExplosions_original)(void);
void C2_HOOK_FASTCALL InitExplosions(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    InitExplosions_original();
#else
    int i;
    tExplosion* explosion;

    for (i = 0; i < REC2_ASIZE(C2V(gExplosions)); i++) {
        explosion = &C2V(gExplosions)[i];

        explosion->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);

        explosion->actor->model = BrModelAllocate(NULL, 4, 2);
        explosion->actor->model->faces[0].vertices[0] = 0;
        explosion->actor->model->faces[0].vertices[1] = 2;
        explosion->actor->model->faces[0].vertices[2] = 1;
        explosion->actor->model->faces[0].material = NULL;
        explosion->actor->model->faces[0].smoothing = 1;
        explosion->actor->model->faces[1].vertices[0] = 0;
        explosion->actor->model->faces[1].vertices[1] = 3;
        explosion->actor->model->faces[1].vertices[2] = 2;
        explosion->actor->model->faces[1].material = NULL;
        explosion->actor->model->faces[1].smoothing = 1;
        BrVector3Set(&explosion->actor->model->vertices[0].p, -.5f, -.5f, 0.f);
        BrVector2Set(&explosion->actor->model->vertices[0].map, 0.f, 1.f);
        BrVector3Set(&explosion->actor->model->vertices[1].p, -.5f, .5f, 0.f);
        BrVector2Set(&explosion->actor->model->vertices[1].map, 0.f, 0.f);
        BrVector3Set(&explosion->actor->model->vertices[2].p, .5f, .5f, 0.f);
        BrVector2Set(&explosion->actor->model->vertices[2].map, 1.f, 0.f);
        BrVector3Set(&explosion->actor->model->vertices[3].p, .5f, -.5f, 0.f);
        BrVector2Set(&explosion->actor->model->vertices[3].map, 1.f, 1.f);
        BrModelAdd(explosion->actor->model);

        explosion->actor->material = BrMaterialAllocate("BANG!");
        explosion->actor->material->flags &= ~BR_MATF_LIGHT;
        explosion->actor->material->flags |= BR_MATF_ALWAYS_VISIBLE;
        explosion->actor->material->colour_map = C2V(gBack_screen);
        AdaptMaterialsForRenderer(&explosion->actor->material, 1, kRendererShadingType_AmbientOnly);
        BrMaterialAdd(explosion->actor->material);
        explosion->actor->render_style = BR_RSTYLE_FACES;

        explosion->start = 0;
    }
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
