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


// GLOBAL: CARMA2_HW 0x006a55c8
tExplosion gExplosions[50];

// GLOBAL: CARMA2_HW 0x006a6d40
tSmash_explosion gSmash_explosions[20];

// FUNCTION: CARMA2_HW 0x004ea880
void C2_HOOK_FASTCALL InitExplosions(void) {
    int i;
    tExplosion* explosion;

    for (i = 0; i < REC2_ASIZE(gExplosions); i++) {
        explosion = &gExplosions[i];

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
        explosion->actor->material->colour_map = gBack_screen;
        GlorifyMaterial(&explosion->actor->material, 1, kRendererShadingType_AmbientOnly);
        BrMaterialAdd(explosion->actor->material);
        explosion->actor->render_style = BR_RSTYLE_FACES;

        explosion->start = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004eaac0
void C2_HOOK_FASTCALL ResetExplosions(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gExplosions) != 50);
    C2_HOOK_BUG_ON(sizeof(gExplosions[0]) != 0x78);
    C2_HOOK_BUG_ON(REC2_ASIZE(gSmash_explosions) != 20);
    C2_HOOK_BUG_ON(sizeof(gSmash_explosions[0]) != 0xc8);

    for (i = 0; i < REC2_ASIZE(gExplosions); i++) {
        gExplosions[i].start = 0;
        gExplosions[i].finished = 0;
    }
    for (i = 0; i < REC2_ASIZE(gSmash_explosions); i++) {
        gSmash_explosions[i].active = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004f5ec0
void C2_HOOK_FASTCALL ReadExplosionInfo(FILE* pF, int* pChance_explosion, int* pExplosion_sound_id, tExplosion_animation* pExplosion_groups) {
    tPath_name the_path;
    char* name;
    char s[256];

    /* Wasted explosion settings */

    /* Percentage chance of car exploding */
    *pChance_explosion = GetAnInt(pF);
    /* Sound ID */
    *pExplosion_sound_id = GetAnInt(pF);

    PathCat(the_path, gApplication_path, "COMMON");
    /* Name of pixelmap file */
    GetALineAndDontArgue(pF, s);
    name = c2_strtok(s, "\t ,/");
    name[c2_strlen(name) - 4] = '\0';
    PathCat(the_path, the_path, name);
    LoadAllImagesInDirectory(&gMisc_storage_space, the_path);
    ReadExplosion(pF, pExplosion_groups);
}

int C2_HOOK_FAKE_THISCALL PointOutOfSightNotAR(const br_vector3* pPoint, undefined4 pArg2, float pMax_distance) {

    if (gAction_replay_mode) {
        return 1;
    }
    return PointOutOfSight(pPoint, pArg2, pMax_distance);
}

// FUNCTION: CARMA2_HW 0x004eaaf0
void C2_HOOK_FASTCALL MungeExplosions(void) {
    int i;
    tU32 now;

    now = GetTotalTime();
    for (i = 0; i < REC2_ASIZE(gExplosions); i++) {
        tExplosion* explosion = &gExplosions[i];
        int frame;
        undefined prev_frame;
        br_vector3 tv;

        if (explosion->start == 0) {
            continue;
        }
        frame = (now - explosion->start) / explosion->period;
        prev_frame = explosion->field_0x8;
        if (now < explosion->start) {
            if (explosion->actor->parent != NULL) {
                BrActorRemove(explosion->actor);
            }
            continue;
        }
        if (frame >= explosion->count_frames) {
            if (explosion->actor->parent != NULL) {
                BrActorRemove(explosion->actor);
            }
            explosion->finished = 1;
            continue;
        }
        if (PointOutOfSightNotAR(&explosion->collision_actor->t.t.translate.t REC2_THISCALL_EDX, 0.f)) {
            if (explosion->actor->parent != NULL) {
                BrActorRemove(explosion->actor);
            }
            continue;
        }
        explosion->field_0x8 = frame;
        if (explosion->actor->parent == NULL) {
            BrActorAdd(gNon_track_actor, explosion->actor);
        }
        if (frame != prev_frame) {
            explosion->actor->material->colour_map = explosion->frames[frame].map;
            BrMaterialUpdate(explosion->actor->material, BR_MATU_ALL);
        }
        if (explosion->actor->material->colour_map == NULL) {
            if (explosion->actor->parent != NULL) {
                BrActorRemove(explosion->actor);
            }
            continue;
        }
        explosion->actor->t.t.mat = gCamera->t.t.mat;
        BrMatrix34ApplyV(&tv, &explosion->field_0x1c, &explosion->collision_actor->t.t.mat);
        BrVector3Accumulate(&explosion->actor->t.t.translate.t, &tv);
        BrMatrix34PreScale(&explosion->actor->t.t.mat,
            (float)explosion->actor->material->colour_map->width * explosion->field_0x14 / 128.f,
            (float)explosion->actor->material->colour_map->height * explosion->field_0x14 / 128.f,
            1.f);
        BrMatrix34PreRotateZ(&explosion->actor->t.t.mat, explosion->angle);
    }
}

// FUNCTION: CARMA2_HW 0x004ead00
void C2_HOOK_FASTCALL Explode(tExplosion_animation* pExplosion_animation, br_actor* pActor, br_bounds3* pBounds, br_vector3* pPos) {

    NOT_IMPLEMENTED();
}
