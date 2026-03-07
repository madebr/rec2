#include "powerups.h"

#include "animation.h"
#include "car.h"
#include "controls.h"
#include "crush.h"
#include "displays.h"
#include "errors.h"
#include "explosions.h"
#include "funk.h"
#include "globvars.h"
#include "globvrpb.h"
#include "grafdata.h"
#include "graphics.h"
#include "init.h"
#include "input.h"
#include "loading.h"
#include "network.h"
#include "pedestrn.h"
#include "physics.h"
#include "piping.h"
#include "platform.h"
#include "shrapnel.h"
#include "skidmark.h"
#include "smashing.h"
#include "sound.h"
#include "spark.h"
#include "utility.h"
#include "world.h"

#include "brender/brender.h"

#include "rec2_macros.h"

#include "c2_string.h"
#include "rec2_types.h"


// GLOBAL: CARMA2_HW 0x007059e0
tHeadup_icon gPickedup_powerups[5];

// GLOBAL: CARMA2_HW 0x0065feb0
const char* gRepeatability_names[4] = {
    "none",
    "singleshot",
    "repeated",
    "doitregardless",
};

// GLOBAL: CARMA2_HW 0x0065ebb0
const char* gFizzle_names[3] = {
    "CIRCLES",
    "SQUARES",
    "DIAMONDS",
};

// GLOBAL: CARMA2_HW 0x006a52d0
tSmashable_item_spec gPowerup_pickup_smashable;

// GLOBAL: CARMA2_HW 0x006a7ce0
tSmashable_item_spec gPowerup_respawn_smashable;

// GLOBAL: CARMA2_HW 0x006a3660
tSmashable_item_spec gUnknown_smashable_006a3660;

// GLOBAL: CARMA2_HW 0x006a0ac0
br_pixelmap* gFizzle_in[3];

// GLOBAL: CARMA2_HW 0x006a0950
int gFizzle_height;

// GLOBAL: CARMA2_HW 0x006a0ad0
int gNumber_of_powerups;

// GLOBAL: CARMA2_HW 0x006a0a54
tPowerup* gPowerup_array;

// GLOBAL: CARMA2_HW 0x006a0948
int gNumber_of_powerup_respawn_specs;

// GLOBAL: CARMA2_HW 0x006a0a64
tPowerup_respawn_spec* gPowerup_respawn_specs;

// GLOBAL: CARMA2_HW 0x006a0a50
tU8* gRace_powerup_respawn_bools;

// GLOBAL: CARMA2_HW 0x006a0a60
int gNet_powerup_time_between_goodies_min;

// GLOBAL: CARMA2_HW 0x006a090c
int gNet_powerup_time_between_goodies_max;

// GLOBAL: CARMA2_HW 0x006a0450
int gNet_count_powerup_goodies;

// GLOBAL: CARMA2_HW 0x006a094c
int* gNet_powerup_goodies;

// GLOBAL: CARMA2_HW 0x0065e910
tGot_proc* gGot_procs[56] = {
    GotCredits,
    SetPedSpeed,
    SetPedSize,
    SetPedExplode,
    PickAtRandom,
    SetInvulnerability,
    SetFreeRepairs,
    DoInstantRepair,
    FreezeTimer,
    SetEngineFactor,
    SetUnderwater,
    GotTimeOrPower,
    TrashBodywork,
    TakeDrugs,
    SetOpponentsSpeed,
    SetCopsSpeed,
    SetGravity,
    SetPinball,
    SetWallclimb,
    SetBouncey,
    SetSuspension,
    SetTyreGrip,
    SetDamageMultiplier,
    SetImmortalPeds,
    SetStupidPedestrians,
    SetSuicidalPedestrians,
    GotVouchers,
    SetMassMultiplier,
    SetInstantHandbrake,
    ShowPedestrians,
    HitMine,
    SetElectroBastard,
    SetPedHeadSize,
    SetMutantCorpses,
    ShitMine,
    PissOutOil,
    KangerooJump,
    AnnihilatePeds,
    RepulseOpponents,
    SetPedBrittle,
    SetGhostPeds,
    SetDancingPeds,
    SetPanickingPeds,
    SetLowGravityPeds,
    SetPissedPeds,
    RepulsePedestrians,
    GainAPO,
    GainAPOPotential,
    SetPissed,
    NapalmPeds,
    PedValium,
    PowerupCancel,
    GrowATail,
    CutOffTail,
    TurnOnCloaking,
    NULL,
};

// GLOBAL: CARMA2_HW 0x0065e9f0
tLose_proc* gLose_procs[56] = {
    NULL,
    ResetPedSpeed,
    ResetPedSize,
    ResetPedExplode,
    NULL,
    ResetInvulnerability,
    ResetFreeRepairs,
    NULL,
    UnfreezeTimer,
    ResetEngineFactor,
    ResetUnderwater,
    NULL,
    NULL,
    PukeDrugsBackUp,
    ResetOpponentsSpeed,
    ResetCopsSpeed,
    ResetGravity,
    ResetPinball,
    ResetWallclimb,
    ResetBouncey,
    ResetSuspension,
    ResetTyreGrip,
    ResetDamageMultiplier,
    ResetImmortalPeds,
    ResetStupidPedestrians,
    ResetSuicidalPeds,
    NULL,
    ResetMassMultiplier,
    ResetInstantHandbrake,
    HidePedestrians,
    NULL,
    ResetElectroBastard,
    ResetPedHeadSize,
    ResetMutantCorpses,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    ResetPedBrittle,
    ResetGhostPeds,
    ResetDancingPeds,
    ResetPanickingPeds,
    ResetLowGravityPeds,
    ResetPissedPeds,
    NULL,
    NULL,
    NULL,
    ResetPissed,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TurnOffCloaking,
    NULL,
};

// GLOBAL: CARMA2_HW 0x0065ead0
tPeriodic_proc* gPeriodic_procs[56] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TheEffectsOfDrugs,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    DoBouncey,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    MungeVouchers,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    PissedEffect,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

// GLOBAL: CARMA2_HW 0x00705560
tShit_mine gShit_mines[20];

// GLOBAL: CARMA2_HW 0x006a0960
tRepulse_link gRepulse_links[6];

// GLOBAL: CARMA2_HW 0x006a0acc
tPhysics_joint* gMutant_tail_first_joint;

// GLOBAL: CARMA2_HW 0x006a0908
tCollision_info* gMutant_tail_first_collision_info;

// GLOBAL: CARMA2_HW 0x00705540
int gMutant_tail_state;

// GLOBAL: CARMA2_HW 0x006a0ae0
br_model* gModel_powerup_armor;

// GLOBAL: CARMA2_HW 0x006a0ae4
br_model* gModel_powerup_power;

// GLOBAL: CARMA2_HW 0x006a0ae8
br_model* gModel_powerup_offence;

// GLOBAL: CARMA2_HW 0x006a0a70
int gInventory[20];

// GLOBAL: CARMA2_HW 0x006a0a6c
int gINT_006a0a6c;

// GLOBAL: CARMA2_HW 0x006a0a5c
int gINT_006a0a5c;

// GLOBAL: CARMA2_HW 0x006a0458
tRespawn_powerup gRespawn_powerups[100];

// GLOBAL: CARMA2_HW 0x006a0a58
int gInventory_selected;

// GLOBAL: CARMA2_HW 0x006a0944
int gNumber_of_icons;

// GLOBAL: CARMA2_HW 0x006a0aec
tU32 gNext_goody_time;

// GLOBAL: CARMA2_HW 0x006a0958
undefined4 gDAT_006a0958;

// GLOBAL: CARMA2_HW 0x006a0454
int gCount_cloaked_cars;

// GLOBAL: CARMA2_HW 0x006a0910
tCar_spec* gCloaked_cars[12];


// FUNCTION: CARMA2_HW 0x004df1e0
int C2_HOOK_FASTCALL GetPowerupCount(void) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x004df1f0
void C2_HOOK_FASTCALL GetPowerupPosition(int pIndex, br_vector3* pPosition) {

}

void C2_HOOK_FASTCALL InitRepulseEffects(void) {
    br_pixelmap* repulse;
    int i;
    int j;

    repulse = LoadPixelmap("REPULSE.PIX");
    if (repulse == NULL) {
        FatalError(kFatalError_CantFindFile_S, "REPULSE.PIX");
    }
    BrMapAdd(repulse);

    C2_HOOK_BUG_ON(REC2_ASIZE(gRepulse_links) != 6);
    C2_HOOK_BUG_ON(sizeof(tRepulse_link) != 0x28);

    for (i = 0; i < REC2_ASIZE(gRepulse_links); i++) {
        tRepulse_link *link = &gRepulse_links[i];
        br_model *model;
        br_material *material;

        link->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        model = BrModelAllocate(NULL, 102, 100);
        model->flags |= BR_MODF_UPDATEABLE;
        material = BrMaterialAllocate("");
        material->colour_map = repulse;
        material->flags &= ~BR_MATF_LIGHT;
        material->flags |= BR_MATF_TWO_SIDED;
        BrMaterialAdd(material);
        BlendifyMaterial(material, 100);
        BrMaterialUpdate(material, BR_MATU_ALL);
        for (j = 0; j < model->nfaces / 2; j++) {
            model->faces[j].material = material;
            model->faces[j].vertices[0] = j + 0;
            model->faces[j].vertices[1] = j + 1;
            model->faces[j].vertices[2] = j + 1 + model->nvertices / 2;
        }
        for (j = model->nfaces / 2; j < model->nfaces; j++) {
            model->faces[j].material = material;
            model->faces[j].vertices[0] = j + 1;
            model->faces[j].vertices[1] = j + 2;
            model->faces[j].vertices[2] = j + 1 - model->nvertices / 2;
        }
        for (j = 0; j < model->nvertices / 2; j++) {
            model->vertices[j].p.v[0] = (float) (j % 2);
            model->vertices[j].p.v[1] = 0.f;
        }
        for (j = model->nvertices / 2; j < model->nvertices; j++) {
            model->vertices[j].p.v[0] = (float) (j % 2);
            model->vertices[j].p.v[1] = 1.f;
        }
        link->actor->model = model;
        link->actor->render_style = BR_RSTYLE_FACES;
        link->material = material;
        BrModelAdd(model);
    }
}

void C2_HOOK_FASTCALL InitTail(void) {
    int i;
    tCollision_info* parent_collision_info;
    tPath_name the_path;
    tTWTVFS twt;

    parent_collision_info = NULL;
    PathCat(the_path, gApplication_path, "TAIL");
    twt = OpenPackFileAndSetTiffLoading(the_path);
    LoadAllStuffInDirectory(&gMisc_storage_space, the_path, kRendererShadingType_Specular);
    ClosePackFileAndSetTiffLoading(twt);
    for (i = 0; i < gCount_mutant_tail_parts; i++) {
        int is_link;
        tCollision_info *collision_info;
        br_model *model;

        is_link = i != gCount_mutant_tail_parts - 1;
        if (is_link) {
            model = BrModelFind("TAILLINK");
            collision_info = MungeBoxObject(model REC2_THISCALL_EDX, gMass_mutant_tail_link);
        } else {
            model = BrModelFind("TAILBALL");
            collision_info = MungeSphereObject(model REC2_THISCALL_EDX, gMass_mutant_tail_ball);
        }

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, physics_joint1, 0x188);

        collision_info->flags_0x238 = 0x20;
        collision_info->physics_joint1 = AllocatePhysicsJoint(1, kMem_physics_joint);
        collision_info->physics_joint1->type = eJoint_ball_n_socket;
        BrVector3SetFloat(&collision_info->physics_joint1->field_0x08,
                          0.f, 0.f, model->bounds.min.v[2] * (is_link ? 0.99f : 0.45f));
        if (i == 0) {
            gMutant_tail_first_joint = collision_info->physics_joint1;
            gMutant_tail_first_collision_info = collision_info;

            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, type, 0x00);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, friction, 0x04);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, field_0x08, 0x08);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, hinge_axis2, 0x20);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, hinge_axis3, 0x2c);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, hinge_axis, 0x38);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, count_limits, 0x50);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, limits, 0x54);

            collision_info->physics_joint1->count_limits = 1;
            BrVector3SetFloat(&collision_info->physics_joint1->limits[0].child, 0.f, 0.f, 1.f);
            BrVector3SetFloat(&collision_info->physics_joint1->limits[0].parent, 0.f, 0.f, 1.f);
        } else {
            BrVector3SetFloat(&collision_info->physics_joint1->field_0x14,
                              0.f, 0.f, model->bounds.max.v[2] * (is_link ? 0.99f : 0.45f));
            PhysicsAddObject(parent_collision_info, collision_info);
        }
        BrVector3SetFloat(&collision_info->physics_joint1->hinge_axis2, 0.f, 0.f, 1.f);
        BrVector3SetFloat(&collision_info->physics_joint1->hinge_axis3, 0.f, 1.f, 0.f);
        BrVector3SetFloat(&collision_info->physics_joint1->hinge_axis, 1.f, 0.f, 0.f);
        BrMatrix34Translate(&collision_info->transform_matrix, 0.f, 0.f, 0.f);
        BrMatrix34Translate(&collision_info->actor->t.t.mat, 0.f, 0.f, 0.f);
        parent_collision_info = collision_info;
    }
    gMutant_tail_state = 0;
}

// FUNCTION: CARMA2_HW 0x004d9ea0
void C2_HOOK_FASTCALL InitPowerups(void) {

    InitRepulseEffects();
    InitTail();
    InitMineShit();
}

void C2_HOOK_FASTCALL RemoveRepulseEffect(tRepulse_link* pRepulse) {

    pRepulse->time = 0;
    if (pRepulse->actor->parent == gNon_track_actor) {
        BrActorRemove(pRepulse->actor);
    }
}

void C2_HOOK_FASTCALL ResetRepulseEffects(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gRepulse_links) != 6);

    for (i = 0; i < REC2_ASIZE(gRepulse_links); i++) {
        RemoveRepulseEffect(&gRepulse_links[i]);
    }
}

// FUNCTION: CARMA2_HW 0x004da600
void C2_HOOK_FASTCALL EmptyInventory(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gInventory) != 20);

    for (i = 0; i < REC2_ASIZE(gInventory); i++) {
        gInventory[i] = -1;
    }
    gInventory_selected = -1;
    gINT_006a0a6c = 0;
    gINT_006a0a5c = 0;
}

// FUNCTION: CARMA2_HW 0x004da630
void C2_HOOK_FASTCALL ResetPowerups(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gRespawn_powerups) != 100);
    C2_HOOK_BUG_ON(sizeof(gRespawn_powerups[0]) != 0xc);

    gRace_powerup_respawn_bools = gPowerup_respawn_specs[gNet_mode == eNet_mode_none ? 0 : 1].bools;

    for (i = 0; i < gNumber_of_powerups; i++) {
        gPowerup_array[i].got_time = 0;
        gPowerup_array[i].current_value = -1;
    }
    for (i = 0; i < REC2_ASIZE(gRespawn_powerups); i++) {
        gRespawn_powerups[i].actor = NULL;
    }
    for (i = 0; i < REC2_ASIZE(gPickedup_powerups); i++) {
        if (gPickedup_powerups[i].icon_actor != NULL) {
            gPickedup_powerups[i].icon_actor->render_style = BR_RSTYLE_NONE;
        }
    }
    EmptyInventory();

    gNumber_of_icons = 0;
    gDAT_006a0958 = 0;
    gInventory_cycling = 0;
    gInventory_timeout = 0;
    gCount_cloaked_cars = 0;
    ResetRepulseEffects();
    gNext_goody_time = 0;
}

// FUNCTION: CARMA2_HW 0x004e07f0
void C2_HOOK_FASTCALL PrintPowerupIconIn3D(int pX, int pY, tHeadup_icon* pIcon, tPowerup* pPowerup, int pScale, tU32 pTime) {
    float f;
    float scale;
    float angle;
    br_fixed_ls prev_opacity;

    if (pIcon == NULL) {
        pPowerup->icon_actor->render_style = BR_RSTYLE_FACES;
        BrVector3Set(&pIcon->icon_actor->t.t.translate.t, (float)pX + 16.f, (float)-(pY + 16), 0.f);
        RenderThisHeadup(pIcon->icon_actor);
        return;
    }
    if (pIcon->icon_actor != NULL) {
        pIcon->icon_actor->render_style = BR_RSTYLE_FACES;
        if (pScale) {
            f = (float)(pTime - pIcon->fizzle_start) / 600.f;
            if (pIcon->fizzle_direction >= 0) {
                scale = 1.f + 5.f * (1.f - f);
                angle = 1.f - f;
            } else {
                scale = 1.01f - f;
                angle = f;
            }
            BrMatrix34Scale(&pIcon->icon_actor->t.t.mat, scale, scale, 1.f);
            BrMatrix34PostRotateZ(&pIcon->icon_actor->t.t.mat, BrScalarToFixed(1.f - angle / 2.f));
            pIcon->icon_actor->material->extra_prim[1].v.x = BR_FIXED_INT((1.f - angle) * 255.f);
            BrMaterialUpdate(pIcon->icon_actor->material, BR_MATU_EXTRA_PRIM);
        } else {
            prev_opacity = pIcon->icon_actor->material->extra_prim[1].v.x;
            pIcon->icon_actor->material->extra_prim[1].v.x = 0xff0000;
            if (prev_opacity != pIcon->icon_actor->material->extra_prim[1].v.x) {
                BrMaterialUpdate(pIcon->icon_actor->material, BR_MATU_EXTRA_PRIM);
            }
            BrMatrix34Identity(&pIcon->icon_actor->t.t.mat);
        }
        BrVector3Set(&pIcon->icon_actor->t.t.translate.t, (float)pX + 16.f, (float)-(pY + 16.f), 0.f);
        RenderThisHeadup(pIcon->icon_actor);
    }
}

// FUNCTION: CARMA2_HW 0x004e09d0
br_actor* C2_HOOK_FASTCALL CreateBillBoard(br_pixelmap* pMap) {
    br_actor* actor;
    br_material* material;
    br_model* model;
    br_scalar half_width;
    br_scalar half_height;

    model = BrModelAllocate("Billboard Model", 4, 2);
    material = BrMaterialAllocate("Billboard Material");
    actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    actor->identifier = "Billboard Actor";
    actor->model = model;
    actor->material = material;
    actor->render_style = BR_RSTYLE_FACES;
    model->faces[0].vertices[0] = 0;
    model->faces[0].vertices[1] = 1;
    model->faces[0].vertices[2] = 2;
    model->faces[1].vertices[0] = 2;
    model->faces[1].vertices[1] = 3;
    model->faces[1].vertices[2] = 0;
    model->faces[0].material = NULL;
    model->faces[1].material = NULL;
    half_width = (float)(pMap->width / 2);
    half_height = (float)(pMap->height / 2);
    BrVector3Set(&model->vertices[0].p, -half_width,  half_height, -2.f);
    BrVector3Set(&model->vertices[1].p, -half_width, -half_height, -2.f);
    BrVector3Set(&model->vertices[2].p,  half_width, -half_height, -2.f);
    BrVector3Set(&model->vertices[3].p,  half_width,  half_height, -2.f);
    BrVector2Set(&model->vertices[0].map, 0.f, 0.f);
    BrVector2Set(&model->vertices[1].map, 0.f, 1.f);
    BrVector2Set(&model->vertices[2].map, 1.f, 1.f);
    BrVector2Set(&model->vertices[3].map, 1.f, 0.f);
    material->colour = 0;
    material->colour_map = pMap;
    material->flags = BR_MATF_ALWAYS_VISIBLE | BR_MATF_FORCE_FRONT;
    model->flags |= BR_MODF_KEEP_ORIGINAL;
    BrMapAdd(pMap);
    BrMaterialAdd(material);
    BrModelAdd(model);
    return actor;
}

static br_pixelmap* DRMapFindMeAPixie(const char* name) {
    char s[256];
    br_pixelmap* map;

    map = BrMapFind(name);
    if (map != NULL) {
        return map;
    }
    c2_strcpy(s, name);
    c2_strcat(s, ".PIX");
    map = BrMapFind(s);
    if (map != NULL) {
        return map;
    }
    c2_strcpy(s, name);
    c2_strcat(s, ".pix");
    map = BrMapFind(s);
    if (map != NULL) {
        return map;
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x004d96c0
void C2_HOOK_FASTCALL LoadPowerups(void) {
    FILE* f;
    tPath_name the_path;
    tTWTVFS twtvfs;
    int action_index;
    int first;
    int i, j, k;
    int time;
    int count_index_groups;
    int index_range;
    char s[256];
    tPowerup* the_powerup;

    C2_HOOK_BUG_ON(sizeof(tPowerup) != 172);

    if (gPowerup_txt_path == NULL) {
        SetDefaultPowerupFilename();
    }
    PathCat(the_path, gApplication_path, gGraf_specs[gGraf_spec_index].data_dir_name);
    PathCat(the_path, the_path, "POWERUPS");
    twtvfs = OpenPackFileAndSetTiffLoading(the_path);
    LoadAllImagesInDirectory(&gMisc_storage_space, the_path);
    ClosePackFileAndSetTiffLoading(twtvfs);

    for (i = 0; i < REC2_ASIZE(gFizzle_in); i++) {
        gFizzle_in[i] = DRMapFindMeAPixie(gFizzle_names[i]);
    }
    gFizzle_height = gFizzle_in[0]->height / 4;

    PathCat(the_path, gApplication_path, gPowerup_txt_path);
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotLoadCarResolutionIndependentFile);
    }
    /* Number of powerups */
    gNumber_of_powerups = GetAnInt(f);
    gPowerup_array = BrMemAllocate(sizeof(tPowerup) * gNumber_of_powerups, kMem_powerup_array);
    for (i = 0; i < gNumber_of_powerups; i++) {
        the_powerup = &gPowerup_array[i];

        GetALineAndDontArgue(f, the_powerup->message);
        if (c2_strcmp(the_powerup->message, "dummy") == 0) {
            the_powerup->type = ePowerup_dummy;
        } else {
            if (c2_strcmp(the_powerup->message, "n/a") == 0) {
                the_powerup->message[0] = '\0';
            }
            GetAString(f, s);
            the_powerup->icon = DRMapFindMeAPixie(s);
            if (the_powerup->icon != NULL) {
                BrMapAdd(the_powerup->icon);
                BRPM_convert(the_powerup->icon, gBack_screen->type);
            }
            if (the_powerup->icon != NULL) {
                the_powerup->icon_actor = CreateBillBoard(the_powerup->icon);
                the_powerup->icon_actor->material->extra_prim = the_powerup->material_prims;
            }

            /* Fizzle type */
            the_powerup->fizzle_type = GetAnInt(f);
            /* Time limit (-1 instantaneous, 0 whole race, x seconds) */
            time = 1000 * GetAnInt(f);
            the_powerup->duration = time;
            if (time < 0) {
                the_powerup->type = ePowerup_instantaneous;
            } else if (time == 0) {
                the_powerup->type = ePowerup_whole_race;
            } else {
                the_powerup->type = ePowerup_timed;
            }
            /* Initial value for keyboard operable powerups
             * (zero if not key operable,
             *  positive if time in seconds,
             *  negative if number of uses)
             */
            the_powerup->initial_value = GetAnInt(f);
            if (the_powerup->initial_value > 0) {
                the_powerup->initial_value *= 1000;
            }
            /* Action index */
            action_index = GetAnInt(f);
            if (action_index >= 0) {
                the_powerup->got_proc = gGot_procs[action_index];
                the_powerup->lose_proc = gLose_procs[action_index];
                the_powerup->periodic_proc = gPeriodic_procs[action_index];
            } else {
                the_powerup->lose_proc = NULL;
                the_powerup->periodic_proc = NULL;
                the_powerup->got_proc = NULL;
            }
            /*  Number of floating point params */
            the_powerup->number_of_float_params = GetAnInt(f);
            the_powerup->float_params = BrMemAllocate(sizeof(float) * the_powerup->number_of_float_params, kMem_powerup_parms);
            for (j = 0; j < the_powerup->number_of_float_params; j++) {
                the_powerup->float_params[j] = GetAFloat(f);
            }
            /* Number of integer params */
            the_powerup->number_of_integer_params = GetAnInt(f);
            the_powerup->integer_params = BrMemAllocate(sizeof(int) * the_powerup->number_of_integer_params, kMem_powerup_parms);
            for (j = 0; j < the_powerup->number_of_integer_params; j++) {
                the_powerup->integer_params[j] = GetAnInt(f);
            }
            /* Group inclusion */
            the_powerup->group_inclusion = GetAnInt(f);
            /* Pratcam sequence */
            the_powerup->prat_cam_event = GetAnInt(f);
            /* Flags (1 = net_global, 2 = net_inappropriate, 4 = off_before_snapweld) */
            the_powerup->net_type = GetAnInt(f);
        }
    }

    /* RESPAWN SPECS */

    /* Number of respawn specs */
    gNumber_of_powerup_respawn_specs = GetAnInt(f);
    gPowerup_respawn_specs = BrMemAllocate(gNumber_of_powerup_respawn_specs * sizeof(tPowerup_respawn_spec), kMem_powerup_array);
    for (i = 0; i < gNumber_of_powerup_respawn_specs; i++) {
        gPowerup_respawn_specs[i].bools = BrMemAllocate(gNumber_of_powerups * sizeof(tU8), kMem_powerup_array);
        /* Number of index groups */
        count_index_groups = GetAnInt(f);
        for (j = 0; j < count_index_groups; j++) {
            index_range = GetAnInt(f);
            if (index_range >= 0) {
                /* index_range is index */
                gPowerup_respawn_specs[i].bools[index_range] = 1;
            }  else {
                /* index_range is range */
                first = GetAnInt(f);
                for (k = first; k < first - index_range; k++) {
                    gPowerup_respawn_specs[i].bools[k] = 1;
                }
            }
        }
    }
    gRace_powerup_respawn_bools = gPowerup_respawn_specs[0].bools;

    /* NETWORK AUTO-GOODIES */

    /* Min,max times between Auto-goodies */
    GetPairOfInts(f, &gNet_powerup_time_between_goodies_min, &gNet_powerup_time_between_goodies_max);
    gNet_powerup_time_between_goodies_min *= 1000;
    gNet_powerup_time_between_goodies_max *= 1000;
    /* Number of goodies to choose from */
    gNet_count_powerup_goodies = GetAnInt(f);
    gNet_powerup_goodies = BrMemAllocate(gNet_count_powerup_goodies * sizeof(int), kMem_powerup_array);
    for (i = 0; i < gNet_count_powerup_goodies; i++) {
        gNet_powerup_goodies[i] = GetAnInt(f);
    }
    PFfclose(f);
}

static void C2_HOOK_FASTCALL ReadPowerupSmashable(FILE* pF, tSmashable_item_spec* pSmashable_spec) {
    int i;
    int d1, d2;

#ifndef REC2_MATCHING
    C2_HOOK_BUG_ON(offsetof(tShrapnel_spec, initial_pos) != 40);
    C2_HOOK_BUG_ON(offsetof(tShrapnel_spec, type_info) != 56);
    C2_HOOK_BUG_ON(sizeof(tShrapnel_spec) != 88);
    C2_HOOK_BUG_ON(offsetof(tSmashable_item_spec, mode_data) + offsetof(tSmashable_item_spec_shrapnel, connotations.shrapnel) != 44);
    C2_HOOK_BUG_ON(offsetof(tSmashable_item_spec, mode_data) + offsetof(tSmashable_item_spec_shrapnel, connotations.special_effects.explosion_animation) != 572);
    C2_HOOK_BUG_ON(sizeof(tSmashable_item_spec) != 736);
#endif

    c2_memset(pSmashable_spec, 0, sizeof(tSmashable_item_spec));
    /* Number of sounds */
    pSmashable_spec->mode_data.shrapnel.connotations.count_sounds = GetAnInt(pF);
    for (i = 0; i < pSmashable_spec->mode_data.shrapnel.connotations.count_sounds; i++) {
        /* Sound ID */
        pSmashable_spec->mode_data.shrapnel.connotations.sounds[i] = LoadSingleSound(&gTrack_storage_space, GetAnInt(pF));
    }
    ReadShrapnelSpec(pF, pSmashable_spec->mode_data.shrapnel.connotations.shrapnel, &pSmashable_spec->mode_data.shrapnel.connotations.count_shrapnel);
    ReadExplosion(pF, &pSmashable_spec->mode_data.shrapnel.connotations.special_effects.explosion_animation);
    ReadSpillData(pF, &pSmashable_spec->mode_data.shrapnel.connotations.special_effects.slick);
    ReadNonCarCuboidActivation(pF, &pSmashable_spec->mode_data.shrapnel.connotations.side_effects.activations);
    ReadShrapnelSideEffects(pF, &pSmashable_spec->mode_data.shrapnel.connotations.side_effects.side_effects);

    /* Extension flags */
    pSmashable_spec->mode_data.shrapnel.connotations.side_effects.extension_flags = GetAnInt(pF);
    if (pSmashable_spec->mode_data.shrapnel.connotations.side_effects.extension_flags & 0x1) {
        pSmashable_spec->mode_data.shrapnel.connotations.side_effects.extension_arg = GetAnInt(pF);
    }
    /* Room turn on */
    pSmashable_spec->mode_data.shrapnel.connotations.side_effects.room_turn_on_code = GetAnInt(pF);
    /* Award code */
    ReadAward(pF, &pSmashable_spec->mode_data.shrapnel.connotations.award);
    /* Count variable changes */
    pSmashable_spec->mode_data.shrapnel.connotations.variable_changes.count = GetAnInt(pF);
    for (i = 0; i < pSmashable_spec->mode_data.shrapnel.connotations.variable_changes.count; i++) {
        GetPairOfInts(pF, &d1, &d2);
        pSmashable_spec->mode_data.shrapnel.connotations.variable_changes.runtime_changes[i].field_0x0 = d2;
        pSmashable_spec->mode_data.shrapnel.connotations.variable_changes.runtime_changes[i].field_0x2 = d1;
    }
    pSmashable_spec->trigger_type = kSmashableTrigger_Model | kSmashableTrigger_Number;
    REC2_BUG_ON((kSmashableTrigger_Model | kSmashableTrigger_Number) != 0x3);
    pSmashable_spec->mode = kSmashableMode_Remove;
    REC2_BUG_ON(kSmashableMode_Remove != 0x3);
}

// FUNCTION: CARMA2_HW 0x004efa00
void C2_HOOK_FASTCALL ReadPowerupSmashables(FILE* pF) {

    ReadPowerupSmashable(pF, &gPowerup_pickup_smashable);
    ReadPowerupSmashable(pF, &gPowerup_respawn_smashable);

    c2_memset(&gUnknown_smashable_006a3660, 0, sizeof(gUnknown_smashable_006a3660));
    gUnknown_smashable_006a3660.trigger_type = kSmashableTrigger_Model | kSmashableTrigger_Number;
    gUnknown_smashable_006a3660.mode = kSmashableMode_Remove;
}

// FUNCTION: CARMA2_HW 0x004da530
void C2_HOOK_FASTCALL InitMineShit(void) {
    tPath_name the_path;
    tTWTVFS twt;
    int i;

    PathCat(the_path, gApplication_path, "MINE");
    twt = OpenPackFileAndSetTiffLoading(the_path);
    LoadAllStuffInDirectory(&gMisc_storage_space, the_path, kRendererShadingType_Specular);
    ClosePackFileAndSetTiffLoading(twt);

    C2_HOOK_BUG_ON(sizeof(tShit_mine) != 56);
    C2_HOOK_BUG_ON(REC2_ASIZE(gShit_mines) != 20);
    for (i = 0; i < REC2_ASIZE(gShit_mines); i++) {
        gShit_mines[i].flags = 4;
        gShit_mines[i].field_0x35 = 0;
        gShit_mines[i].collision_info = MungeSphereObject(BrModelFind("SHITMINE.ACT") REC2_THISCALL_EDX, gMass_mine);
        gShit_mines[i].collision_info->uid = i;
        gShit_mines[i].collision_info->flags_0x238 = 0x40;
        gShit_mines[i].collision_info->owner = &gShit_mines[i];
        BrMatrix34Translate(&gShit_mines[i].collision_info->transform_matrix, 0.f, 0.f, 0.f);
        BrMatrix34Translate(&gShit_mines[i].collision_info->actor->t.t.mat, 0.f, 0.f, 0.f);
    }
}

void C2_HOOK_FASTCALL FillInMine(tNet_message_chunk* pChunk, tShit_mine* pMine, int pArg3) {

#ifndef REC2_MATCHING
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk, mine_explode.mine_index, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk, mine_explode.field_0x3, 0x3);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk, mine_explode.field_0x4, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk, mine_explode.field_0x5, 0x5);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk, mine_explode.field_0x8, 0x8);
#endif

    pChunk->mine_explode.mine_index = pMine - gShit_mines;
    pChunk->mine_explode.field_0x3 = pMine->field_0x35;
    pChunk->mine_explode.field_0x4 = pMine->field_0x34;
    pChunk->mine_explode.field_0x5 = NetPlayerFromCar(pMine->field_0x30) - gNet_players;
    pChunk->mine_explode.field_0x8 = pArg3;
    GetHierarchyNetworkStuff(pMine->collision_info, pChunk->mine_explode.object_hierarchy, NET_OBJECT_HIERARCHY_DEFAULT_SIZE);
}

// FUNCTION: CARMA2_HW 0x004ddab0
int C2_HOOK_FASTCALL MineExplode(tShit_mine *pMine) {
    tNet_message* message;
    br_vector3 offset;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, contents, 0x18);

    if (pMine->flags & 0x2) {
        return 0;
    }
    pMine->flags |= 0x2;
    if (gNet_mode == eNet_mode_host) {
        message = NetBuildGuaranteedMessage(49, GetHierarchyNetworkSize(pMine->collision_info));
        FillInMine(&message->contents, pMine, -1);
        NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, message, 0);
    }
    PHILRemoveObject(pMine->collision_info);
    pMine->next_think_time = GetTotalTime() + 1600;
    BrVector3Negate(&offset, &gExplosion_pix_animation_groups.groups[0].offset);
    Explode(&gExplosion_pix_animation_groups, pMine->collision_info->actor, &pMine->collision_info->actor->model->bounds, &offset);
    DRS3StartSound3D(gCar_outlet, eSoundId_Explosion, &pMine->collision_info->actor->t.t.translate.t, &gZero_v__car, 1, 255, BR_FIXED_INT(1), BR_FIXED_INT(1));
    PipeSingleEndShitMine(pMine->collision_info->actor);
    if (pMine->collision_info->actor->parent != NULL) {
        BrActorRemove(pMine->collision_info->actor);
    }
    pMine->collision_info->actor->render_style = BR_RSTYLE_NONE;
    return 1;
}

int C2_HOOK_FASTCALL CarVulnerableByMine(tCar_spec* pCar) {

    if (pCar->field_0x4c8 > 1.f) {
        return 0;
    }
    if (gNet_mode != eNet_mode_none) {
        tNet_game_player_info* net_player = NetPlayerFromCar(pCar);
        if (net_player != NULL && net_player->field_0x80 != 0) {
            return 0;
        }
    }
    if (pCar->invulnerable_no_crushage) {
        return 0;
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x004dde20
void C2_HOOK_FASTCALL ProcessShitMines(tU32 pTime) {
    int i;
    int play_effect;

    C2_HOOK_BUG_ON(REC2_ASIZE(gShit_mines) != 20);

    for (i = 0; i < REC2_ASIZE(gShit_mines); i++) {
        tShit_mine* mine = &gShit_mines[i];

        if (!(mine->flags & 0x1)) {
            continue;
        }
        if (mine->flags & 0x80) {
            tCar_spec *car = mine->car;

            if (car != NULL) {
                int car_vulnerable;

                mine->flags &= ~0x80;
                car_vulnerable = CarVulnerableByMine(car);
                if (car_vulnerable) {
                    TotallySpamTheModel(car REC2_THISCALL_EDX, 0.1f * mine->max_damage);
                }
                if (car_vulnerable && (gNet_mode == eNet_mode_none || gNet_mode == eNet_mode_host)) {
                    car->collision_info->v.v[1] += FRandomBetween(mine->field3_0xc, mine->initial_y_speed_factor) / (car->collision_info->M * WORLD_SCALE);
                    car->collision_info->omega.v[2] += FRandomPosNeg(mine->initial_z_omega_factor) * TAU_F / car->collision_info->M;
                    car->collision_info->omega.v[0] += FRandomPosNeg(mine->initial_x_omega_factor) * TAU_F / car->collision_info->M;
                    car->collision_info->disable_move_rotate = 0;

                    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, field_0x49c, 0x49c);
                    car->collision_info->field_0x49c = 40 + gPHIL_last_physics_tick;
                    if (!car->invulnerable_no_damage) {
                        int j;

                        C2_HOOK_BUG_ON(REC2_ASIZE(car->damage_units) != 12);
                        for (j = 0; j < REC2_ASIZE(car->damage_units); j++) {
                            DoDamage(car, j, IRandomBetween(0, (int)(25.f * mine->max_damage)));
                        }
                        if (car != mine->field_0x30) {
                            car->last_hit_by = mine->field_0x30;
                            car->time_last_hit = PDGetTotalTime();
                            CrashEarnings(car, NULL);
                        }
                    }
                }
            }
        }
        play_effect = 0;
        if (mine->flags & 0x20) {
            mine->flags &= ~0x20;
            play_effect = 1;
        } else {
            if (!(mine->flags & 0x2)) {
                if ((mine->flags & 0x4) && pTime > mine->next_think_time) {
                    play_effect = 1;
                }
            } else if (pTime > mine->next_think_time) {
                mine->flags &= ~0x3;
            }
        }
        if (play_effect) {
            MineExplode(mine);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004e06a0
void C2_HOOK_FASTCALL RemoveTail(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004da730
void C2_HOOK_FASTCALL CloseDownPowerUps(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00502e00
void C2_HOOK_FASTCALL MaxOutAPO(void) {
    int i;

    for (i = 0; i < 3; i++) {
        gProgram_state.current_car.power_up_levels[i] = gInitial_powerup_slots[i];
        gProgram_state.current_car.power_up_slots[i] = gInitial_powerup_slots[i];
    }
}

void C2_HOOK_FASTCALL CheckRespawnQueue(tU32 pTime) {
    int i;

    for (i = 0; i < REC2_ASIZE(gRespawn_powerups); i++) {
        tRespawn_powerup *respawn_powerup = &gRespawn_powerups[i];
        if (respawn_powerup->actor == NULL || pTime < respawn_powerup->respawn_time) {
            continue;
        }
        if (respawn_powerup->actor->render_style != BR_RSTYLE_FACES) {
            respawn_powerup->actor->render_style = BR_RSTYLE_FACES;
        }
        PipeSinglePowerupRespawn(respawn_powerup->actor, respawn_powerup->index);
        DoPowerupRespawnSmash(respawn_powerup->actor);
        respawn_powerup->actor = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x004db880
void C2_HOOK_FASTCALL MungePowerupStuff(undefined4 pArg1) {
    tU32 time;

    time = GetTotalTime();
    MungeRepulseRays(time);
    CheckRespawnQueue(time);
}

tU32 C2_HOOK_FASTCALL GetNextGoodyTime(tU32 pTime) {

    return pTime + IRandomBetween(gNet_powerup_time_between_goodies_min, gNet_powerup_time_between_goodies_max);
}

void C2_HOOK_FASTCALL AutoGoody(tU32 pTime) {

    if (gNext_goody_time == 0) {
        gNext_goody_time = GetNextGoodyTime(pTime);
    } else if (gNet_mode != eNet_mode_none
            && !gRace_finished
            && !gProgram_state.current_car.knackered
            && gCurrent_net_game->options.random_car_choice
            && pTime > gNext_goody_time) {
        gNext_goody_time = GetNextGoodyTime(pTime);
        GotPowerupEarwig(&gProgram_state.current_car, gNet_powerup_goodies[IRandomBetween(0, gNet_count_powerup_goodies - 1)], 2, 0);
    }
}

void C2_HOOK_FASTCALL PeriodicCloaking(void) {

    if (gNet_mode != eNet_mode_none) {
        int i;

        for (i = 0; i < gCount_cloaked_cars; i++) {
            DoCamouflageThing(gCloaked_cars[i]);
        }
    }
}

void C2_HOOK_FASTCALL LosePowerup(tPowerup* pPowerup) {

    LosePowerupX(pPowerup, 1);
}

int C2_HOOK_FASTCALL GotPowerupXX(tCar_spec* pCar, int pArg2, int pArg3) {
    int value;

    value = 0;
    if (gNet_mode != eNet_mode_none) {
        if (pCar != NULL && pCar->driver ==  eDriver_local_human) {
            value = 1;
        }
    }
    return GotPowerupX(pCar, pArg2, value, pArg3, 0);
}

void C2_HOOK_FASTCALL MungeKeyboardPowerup(void) {
    tPowerup* powerup;

    if (gInventory_selected < 0) {
        return;
    }
    if (gCountdown) {
        return;
    }
    if (gRace_finished) {
        return;
    }
    if (gProgram_state.current_car.knackered) {
        return;
    }
    powerup = &gPowerup_array[gInventory[gInventory_selected]];
    if (gINT_006a0a5c) {
        gINT_006a0a5c = KeyIsDown(61);
    } else if (gINT_006a0a6c != KeyIsDown(61)) {
        gINT_006a0a6c = !gINT_006a0a6c && !gProgram_state.current_car.knackered;
        if (gINT_006a0a6c) {
            int result;

            result = GotPowerupXX(&gProgram_state.current_car, gInventory[gInventory_selected], 1);
            if (result >= 0 && powerup->initial_value < 0) {
                powerup->value += 1;
                if (powerup->value >= 0) {
                    KeyboardPowerupFinished(powerup, 1);
                }
        } else
            LosePowerup(powerup);
        }
    }
    if (powerup->initial_value > 0 && gINT_006a0a6c) {
        powerup->value -= gFrame_period;
        if (powerup->value <= 0) {
            KeyboardPowerupFinished(powerup, 1);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004db1c0
void C2_HOOK_FASTCALL DoPowerupPeriodics(tU32 pFrame_period) {
    tU32 the_time;
    int i;

    the_time = GetTotalTime();
    AutoGoody(the_time);
    PeriodicCloaking();
    for (i = 0; i < gNumber_of_powerups; i++) {
        tPowerup* powerup;

        powerup = &gPowerup_array[i];
        if (powerup->got_time != 0) {
            if (powerup->type == ePowerup_timed && the_time >= powerup->lose_time && powerup->initial_value == 0) {
                LosePowerup(powerup);
            } else if (powerup->current_value == 0 && powerup->initial_value == 0) {
                LosePowerup(powerup);
            } else if (powerup->periodic_proc != NULL) {
                powerup->periodic_proc(powerup, pFrame_period);
            }
        }
    }
    MungeKeyboardPowerup();
}

// FUNCTION: CARMA2_HW 0x004df650
void C2_HOOK_CDECL RenderSpinningPowerup(br_actor* actor, br_model* model, br_material* material, void* order_table, br_uint_8 style, int on_screen) {

    if (actor->user == NULL) {
        actor->user = (void*)(uintptr_t)GetTotalTime();
    } else {
        tU32 prev;
        tU32 now;

        now = GetTotalTime();
        prev = (tU32)(uintptr_t)actor->user;
        actor->user = (void*)(uintptr_t)now;
        BrMatrix34PreRotateY(&actor->t.t.mat, BR_ANGLE_DEG((now - prev) / 16));
    }
    BrZsModelRender(actor, model, material, order_table, style, on_screen, 0);
}

// FUNCTION: CARMA2_HW 0x004df570
void C2_HOOK_FASTCALL SetSpinningPowerup(br_actor* pActor, int pOpacity) {
    // GLOBAL: CARMA2_HW 0x0065ebc0
    static br_token_value spinning_powerup_extra_prim[] = {
        { BRT_BLEND_B, { 1 } },
        { BRT_OPACITY_X, { BrScalarToFixed(75) } },
    };
    int i;

    if (pActor == NULL) {
        return;
    }
    BrMatrix34PreTranslate(&pActor->t.t.mat, 0.f, .1f, 0.f);
    pActor->model->flags |= BR_MODF_CUSTOM;
    pActor->model->custom = RenderSpinningPowerup;
    if (pOpacity == 0 || pOpacity == 255) {
        return;
    }
    if (pActor->material != NULL) {
        pActor->material->extra_prim = spinning_powerup_extra_prim;
        pActor->material->extra_prim[1].v.x = BrScalarToFixed(pOpacity);
        BrMaterialUpdate(pActor->material, BR_MATU_EXTRA_PRIM);
        return;
    }
    for (i = 0; i < pActor->model->nfaces; i++) {
        br_material* material;

        material = pActor->model->faces[i].material;
        if (material != NULL) {
            material->extra_prim = spinning_powerup_extra_prim;
            material->extra_prim[1].v.x = BrScalarToFixed(pOpacity);
            BrMaterialUpdate(material, BR_MATU_EXTRA_PRIM);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004dfe10
void C2_HOOK_CDECL RenderChangingPowerup(br_actor* actor, br_model* model, br_material* material, void* order_table, br_uint_8 style, int on_screen) {

    if (actor->user == NULL) {
        actor->user = (void*)(uintptr_t)GetTotalTime();
        actor->model = gModel_powerup_armor;
    } else {
        tU32 prev;
        tU32 now;
        tU32 diff;

        now = GetTotalTime();
        prev = (tU32)(uintptr_t)actor->user;
        diff = (prev - now) / 16;
        if (diff > 15) {
            if (actor->model == gModel_powerup_armor) {
                actor->model = gModel_powerup_power;
            } else if (actor->model == gModel_powerup_power) {
                actor->model = gModel_powerup_offence;
            } else {
                actor->model = gModel_powerup_armor;
            }
            actor->user = (void*)(uintptr_t)now;
        }
        BrMatrix34PreRotateY(&actor->t.t.mat, BR_ANGLE_DEG(diff + 12));
    }
    BrZsModelRender(actor, model, material, order_table, style, on_screen, 0);
}

br_model* C2_HOOK_FASTCALL DuplicateModel(br_model* pModel, const char* pName) {
    br_model* copy;

    copy = BrModelAllocate(pName, pModel->nvertices, pModel->nfaces);
    if (copy == NULL) {
        return copy;
    }
    BrVector3Copy(&copy->pivot, &pModel->pivot);
    copy->flags = pModel->flags;
    copy->user = pModel->user;
    copy->crease_angle = pModel->crease_angle;
    copy->radius = pModel->radius;
    copy->bounds = pModel->bounds;
    c2_memcpy(copy->vertices, pModel->vertices, pModel->nvertices * sizeof(br_vertex));
    c2_memcpy(copy->faces, pModel->faces, pModel->nfaces * sizeof(br_face));
    return copy;
}

// FUNCTION: CARMA2_HW 0x004df6c0
void C2_HOOK_FASTCALL SetChangingPowerup(br_actor* pActor) {

    if (pActor == NULL) {
        return;
    }
    if (gModel_powerup_armor == NULL || gModel_powerup_power == NULL || gModel_powerup_offence == NULL) {

        gModel_powerup_armor = DuplicateModel(BrModelFind("&68powerup1.ACT"), "PowArm");
        gModel_powerup_armor->flags |= BR_MODF_CUSTOM;
        gModel_powerup_armor->custom = RenderChangingPowerup;
        BrModelAdd(gModel_powerup_armor);

        gModel_powerup_power = DuplicateModel(BrModelFind("&70powerup1.ACT"), "PowPow");
        gModel_powerup_power->flags |= BR_MODF_CUSTOM;
        gModel_powerup_power->custom = RenderChangingPowerup;
        BrModelAdd(gModel_powerup_power);

        gModel_powerup_offence = DuplicateModel(BrModelFind("&69powerup1.ACT"), "PowOff");
        gModel_powerup_offence->flags |= BR_MODF_CUSTOM;
        gModel_powerup_offence->custom = RenderChangingPowerup;
        BrModelAdd(gModel_powerup_offence);
    }

    if (gModel_powerup_armor == NULL) {
        tPath_name path;
        char s[256];
        FILE* f;
        int i;
        br_actor* actor;

        PathCat(path, gApplication_path, "POWRSHIT.TXT");
        f = DRfopen(path, "rt");
        if (f == NULL) {
            c2_sprintf(s, "Cant open %s", "POWRSHIT.TXT");
            PDFatalError(s);
        }
        for (i = 0; i < 67; i++) {
            GetAString(f, s);
        }
        actor = LoadActor(s);
        if (actor != NULL && actor->model != NULL) {
            gModel_powerup_armor = DuplicateModel(actor->model, "PowArm");
            gModel_powerup_armor->flags |= BR_MODF_CUSTOM;
            gModel_powerup_armor->custom = RenderChangingPowerup;
            BrModelAdd(gModel_powerup_armor);
            BrActorFree(actor);
        }
        PFfclose(f);
    }

    if (gModel_powerup_power == NULL) {
        tPath_name path;
        char s[256];
        FILE* f;
        int i;
        br_actor* actor;

        PathCat(path, gApplication_path, "POWRSHIT.TXT");
        f = DRfopen(path, "rt");
        if (f == NULL) {
            c2_sprintf(s, "Cant open %s", "POWRSHIT.TXT");
            PDFatalError(s);
        }
        for (i = 0; i < 68; i++) {
            GetAString(f, s);
        }
        actor = LoadActor(s);
        if (actor != NULL && actor->model != NULL) {
            gModel_powerup_power = DuplicateModel(actor->model, "PowPow");
            gModel_powerup_power->flags |= BR_MODF_CUSTOM;
            gModel_powerup_power->custom = RenderChangingPowerup;
            BrModelAdd(gModel_powerup_power);
            BrActorFree(actor);
        }
        PFfclose(f);
    }

    if (gModel_powerup_offence == NULL) {
        tPath_name path;
        char s[256];
        FILE* f;
        int i;
        br_actor* actor;

        PathCat(path, gApplication_path, "POWRSHIT.TXT");
        f = DRfopen(path, "rt");
        if (f == NULL) {
            c2_sprintf(s, "Cant open %s", "POWRSHIT.TXT");
            PDFatalError(s);
        }
        for (i = 0; i < 69; i++) {
            GetAString(f, s);
        }
        actor = LoadActor(s);
        if (actor != NULL && actor->model != NULL) {
            gModel_powerup_offence = DuplicateModel(actor->model, "PowOff");
            gModel_powerup_offence->flags |= BR_MODF_CUSTOM;
            gModel_powerup_offence->custom = RenderChangingPowerup;
            BrModelAdd(gModel_powerup_offence);
            BrActorFree(actor);
        }
        PFfclose(f);
    }
    BrMatrix34PreTranslate(&pActor->t.t.mat, .0f, .1f, .0f);
    pActor->model = gModel_powerup_armor;
    pActor->model->flags |= BR_MODF_CUSTOM;
    pActor->model->custom = RenderChangingPowerup;
}

// FUNCTION: CARMA2_HW 0x004d8d30
int C2_HOOK_FASTCALL GotPowerup(tCar_spec* pCar, int pIndex) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dc170
int C2_HOOK_FASTCALL GotCredits(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dc720
int C2_HOOK_FASTCALL SetPedSpeed(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dc750
int C2_HOOK_FASTCALL SetPedSize(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dc7d0
int C2_HOOK_FASTCALL SetPedExplode(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004de7a0
int C2_HOOK_FASTCALL PickAtRandom(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dc830
int C2_HOOK_FASTCALL SetInvulnerability(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dc8a0
int C2_HOOK_FASTCALL SetFreeRepairs(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dc990
int C2_HOOK_FASTCALL DoInstantRepair(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dc930
int C2_HOOK_FASTCALL FreezeTimer(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dca30
int C2_HOOK_FASTCALL SetEngineFactor(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dcab0
int C2_HOOK_FASTCALL SetUnderwater(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dc470
int C2_HOOK_FASTCALL GotTimeOrPower(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dcae0
int C2_HOOK_FASTCALL TrashBodywork(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dcb90
int C2_HOOK_FASTCALL TakeDrugs(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dcca0
int C2_HOOK_FASTCALL SetOpponentsSpeed(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dcd70
int C2_HOOK_FASTCALL SetCopsSpeed(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dcda0
int C2_HOOK_FASTCALL SetGravity(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dcdd0
int C2_HOOK_FASTCALL SetPinball(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dce00
int C2_HOOK_FASTCALL SetWallclimb(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dce30
int C2_HOOK_FASTCALL SetBouncey(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dce80
int C2_HOOK_FASTCALL SetSuspension(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dced0
int C2_HOOK_FASTCALL SetTyreGrip(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dcf80
int C2_HOOK_FASTCALL SetDamageMultiplier(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dcf00
int C2_HOOK_FASTCALL SetImmortalPeds(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dc8f0
int C2_HOOK_FASTCALL SetStupidPedestrians(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dcf40
int C2_HOOK_FASTCALL SetSuicidalPedestrians(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004de7d0
int C2_HOOK_FASTCALL GotVouchers(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004debb0
int C2_HOOK_FASTCALL SetMassMultiplier(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004de820
int C2_HOOK_FASTCALL SetInstantHandbrake(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dec10
int C2_HOOK_FASTCALL ShowPedestrians(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dea40
int C2_HOOK_FASTCALL HitMine(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dec60
int C2_HOOK_FASTCALL SetElectroBastard(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dc7a0
int C2_HOOK_FASTCALL SetPedHeadSize(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004deca0
int C2_HOOK_FASTCALL SetMutantCorpses(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dd700
int C2_HOOK_FASTCALL ShitMine(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004de040
int C2_HOOK_FASTCALL PissOutOil(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004de0c0
int C2_HOOK_FASTCALL KangerooJump(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004de140
int C2_HOOK_FASTCALL AnnihilatePeds(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004de1e0
int C2_HOOK_FASTCALL RepulseOpponents(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dc800
int C2_HOOK_FASTCALL SetPedBrittle(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004de930
int C2_HOOK_FASTCALL SetGhostPeds(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004de870
int C2_HOOK_FASTCALL SetDancingPeds(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004de8a0
int C2_HOOK_FASTCALL SetPanickingPeds(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004de8d0
int C2_HOOK_FASTCALL SetLowGravityPeds(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004de900
int C2_HOOK_FASTCALL SetPissedPeds(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004de540
int C2_HOOK_FASTCALL RepulsePedestrians(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dc240
int C2_HOOK_FASTCALL GainAPO(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dc4d0
int C2_HOOK_FASTCALL GainAPOPotential(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004df220
int C2_HOOK_FASTCALL SetPissed(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004de190
int C2_HOOK_FASTCALL NapalmPeds(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dfec0
int C2_HOOK_FASTCALL PedValium(tPowerup* powerup, tCar_spec* car) {

    if (!CalmDownAllPeds()) {
        return -1;
    }

    NewTextHeadupSlot(4,0, 3000, -4, GetMiscString(eMiscString_pedestrians_are_calm_now));
    DRS3StartSound2(gCar_outlet, eSoundId_PedValium, 1, 0xff, 0xff, -1, -1);
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dff30
int C2_HOOK_FASTCALL PowerupCancel(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e02b0
int C2_HOOK_FASTCALL GrowATail(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e04c0
int C2_HOOK_FASTCALL CutOffTail(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e0c40
int C2_HOOK_FASTCALL TurnOnCloaking(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e0cb0
void C2_HOOK_FASTCALL RemoveFromCloakingList(tCar_spec* pCar_spec) {

    if (gNet_mode != eNet_mode_none) {
        int i;

        for (i = 0; i < gCount_cloaked_cars; i++) {
            if (gCloaked_cars[i] == pCar_spec) {
                c2_memmove(&gCloaked_cars[i], &gCloaked_cars[i + 1],
                    (REC2_ASIZE(gCloaked_cars) - i - 1) * sizeof(tCar_spec *));
                gCount_cloaked_cars -= 1;
                break;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004e0d10
int C2_HOOK_FASTCALL IsCarCloaked(tCar_spec* pCar_spec) {

    if (gNet_mode != eNet_mode_none) {
        int i;

        for (i = 0; i < gCount_cloaked_cars; i++) {
            if (gCloaked_cars[i] == pCar_spec) {
                return 1;
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004dc9c0
void C2_HOOK_FASTCALL ResetPedSpeed(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004dc9d0
void C2_HOOK_FASTCALL ResetPedSize(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004dca10
void C2_HOOK_FASTCALL ResetPedExplode(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004dc860
void C2_HOOK_FASTCALL ResetInvulnerability(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004dc8d0
void C2_HOOK_FASTCALL ResetFreeRepairs(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004dc970
void C2_HOOK_FASTCALL UnfreezeTimer(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004de5d0
void C2_HOOK_FASTCALL ResetEngineFactor(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004de600
void C2_HOOK_FASTCALL ResetUnderwater(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004de610
void C2_HOOK_FASTCALL PukeDrugsBackUp(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004de6b0
void C2_HOOK_FASTCALL ResetOpponentsSpeed(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004de710
void C2_HOOK_FASTCALL ResetCopsSpeed(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004de720
void C2_HOOK_FASTCALL ResetGravity(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004de730
void C2_HOOK_FASTCALL ResetPinball(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004de740
void C2_HOOK_FASTCALL ResetWallclimb(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004de750
void C2_HOOK_FASTCALL ResetBouncey(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004de760
void C2_HOOK_FASTCALL ResetSuspension(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004de790
void C2_HOOK_FASTCALL ResetTyreGrip(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004de780
void C2_HOOK_FASTCALL ResetDamageMultiplier(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004dcf30
void C2_HOOK_FASTCALL ResetImmortalPeds(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004dc920
void C2_HOOK_FASTCALL ResetStupidPedestrians(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004dcf70
void C2_HOOK_FASTCALL ResetSuicidalPeds(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004debf0
void C2_HOOK_FASTCALL ResetMassMultiplier(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004de850
void C2_HOOK_FASTCALL ResetInstantHandbrake(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004dec40
void C2_HOOK_FASTCALL HidePedestrians(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004dec90
void C2_HOOK_FASTCALL ResetElectroBastard(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004dca00
void C2_HOOK_FASTCALL ResetPedHeadSize(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004decd0
void C2_HOOK_FASTCALL ResetMutantCorpses(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004dca20
void C2_HOOK_FASTCALL ResetPedBrittle(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004de9a0
void C2_HOOK_FASTCALL ResetGhostPeds(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004de960
void C2_HOOK_FASTCALL ResetDancingPeds(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004de970
void C2_HOOK_FASTCALL ResetPanickingPeds(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004de980
void C2_HOOK_FASTCALL ResetLowGravityPeds(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004de990
void C2_HOOK_FASTCALL ResetPissedPeds(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004df300
void C2_HOOK_FASTCALL ResetPissed(tPowerup* powerup, tCar_spec* car) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004e0d50
void C2_HOOK_FASTCALL TurnOffCloaking(tPowerup* powerup, tCar_spec* car) {

    if (gNet_mode != eNet_mode_none) {
        UnBlendifyCar(car);
        RemoveFromCloakingList(car);
        MasterEnableCarFunks(car);
        RestoreCarPixelmaps(car);
    }
}

// FUNCTION: CARMA2_HW 0x004dcc00
void C2_HOOK_FASTCALL TheEffectsOfDrugs(tPowerup* powerup, tU32 time) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004de9b0
void C2_HOOK_FASTCALL DoBouncey(tPowerup* powerup, tU32 time) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004de810
void C2_HOOK_FASTCALL MungeVouchers(tPowerup* powerup, tU32 time) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004df3b0
void C2_HOOK_FASTCALL PissedEffect(tPowerup* powerup, tU32 time) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004dcfb0
void C2_HOOK_FASTCALL SendMines(tU32 pNext_frame_time) {

    if (gNet_mode != eNet_mode_host) {
        return;
    }
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004d8d40
void C2_HOOK_FASTCALL GotPowerupEarwig(tCar_spec* pCar, int pIndex, int pMessage, int pSelect_inventory) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004db630
void C2_HOOK_FASTCALL LosePowerupX(tPowerup* pPowerup, undefined4 pArg2) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004db730
void C2_HOOK_FASTCALL KeyboardPowerupFinished(tPowerup* pPowerup, int pSelect_new) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004d9150
int C2_HOOK_FASTCALL GotPowerupX(tCar_spec* pCar, int pIndex, int pArg3, int pMessage, tU32 pTime) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004daff0
int C2_HOOK_FASTCALL DrawSinglePowerupIcon(int pDraw, int pTime, tPowerup* pPowerup, tHeadup_icon* pIcon, int pX, int pY, int pUpdate_x) {
    br_pixelmap *fizzle_pix;
    char s[8];
    int timer;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, power_up_space_between_icon_text_dx, 0x294);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, power_up_space_between_icon_text_dy, 0x2a0);

    if (pIcon != NULL && pIcon->fizzle_stage < 4) {
        fizzle_pix = gFizzle_in[pPowerup->fizzle_type];
        if (pDraw) {
            PrintPowerupIconIn3D(pX, pY, pIcon, pPowerup, 1, pTime);
        }
        if (pUpdate_x) {
            return pX + fizzle_pix->width;
        }
        return 0;
    }
    if (pDraw) {
        PrintPowerupIconIn3D(pX, pY, pIcon, pPowerup, 0, pTime);
    }
    if (pPowerup->initial_value < 0) {
        c2_sprintf(s, "%d", -pPowerup->value);
        if (pDraw) {
            TransDRPixelmapText(gBack_screen,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx,
                pY + gCurrent_graf_data->power_up_space_between_icon_text_dy,
                &gFonts[2],
                s,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + 30);
        }
        if (pUpdate_x) {
            return pX + DRTextWidth(&gFonts[2], s);
        }
    } else if (pPowerup->initial_value > 0) {
        TimerString(pPowerup->value, s, 0, 0, 1);
        if (pDraw) {
            TransDRPixelmapText(gBack_screen,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx,
                pY + gCurrent_graf_data->power_up_space_between_icon_text_dy,
                &gFonts[2],
                s,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + 30);
        }
        if (pUpdate_x) {
            return pX + DRTextWidth(&gFonts[2], s);
        }
    } else if (pPowerup->type == ePowerup_timed) {

        timer = pPowerup->lose_time - pTime;
        if (timer < 0) {
            timer = 0;
        }
        TimerString(timer, s, 0, 0, 1);
        if (pDraw) {
            TransDRPixelmapText(gBack_screen,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx,
                pY + gCurrent_graf_data->power_up_space_between_icon_text_dy,
                &gFonts[1],
                s,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + 30);
        }
        if (pUpdate_x) {
            return pX + DRTextWidth(&gFonts[1], s);
        }
    } else if (pPowerup->current_value > 0) {

        c2_sprintf(s, "%d", pPowerup->current_value);
        if (pDraw) {
            TransDRPixelmapText(gBack_screen,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx,
                pY + gCurrent_graf_data->power_up_space_between_icon_text_dy,
                &gFonts[2],
                s,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + 30);
        }
        if (pUpdate_x) {
            return pX + DRTextWidth(&gFonts[2], s);
        }
    }
    return 0;
}

void C2_HOOK_FASTCALL DrawPowerupIcons2(tU32 pTime, tHeadup_icon* pHeadup_icons, int* pCount_icons, int pMax_count_icons, int pX, int pY) {
    int i;
    int y;
    tPowerup *the_powerup;
    tHeadup_icon *the_icon;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, power_up_icon_y_pitch, 0x29c);

    y =  pY;
    for (i = 0; i < *pCount_icons && i < pMax_count_icons; i++) {
        the_icon = &pHeadup_icons[i];
        the_powerup = the_icon->powerup;
        if (the_powerup->icon == NULL) {
            continue;
        }
        y += gCurrent_graf_data->power_up_icon_y_pitch;
        if (the_icon->fizzle_stage < 4) {
            if (the_icon->fizzle_direction < 0) {
                the_icon->fizzle_stage = 3 - (pTime - the_icon->fizzle_start) / 150;
            } else {
                the_icon->fizzle_stage = (pTime - the_icon->fizzle_start) / 150;
            }
            if (the_icon->fizzle_stage > 4) {
                the_icon->fizzle_stage = 4;
            } else if (the_icon->fizzle_stage < 0) {
                c2_memmove(the_icon, &the_icon[1], sizeof(tHeadup_icon) * (*pCount_icons - i - 1));
                *pCount_icons -= 1;
                continue;
            }
        }
        DrawSinglePowerupIcon(1, pTime, the_icon->powerup, the_icon, pX, y, 0);
    }
}

void C2_HOOK_FASTCALL DrawKeyPowerups(tU32 pTime) {
    int x;
    int i;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, keyboard_powerup_x, 0x2a4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, keyboard_powerup_y, 0x2a8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, keyboard_powerup_bg_y, 0x2ac);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, keyboard_powerup_bg_bottom, 0x2b0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, keyboard_powerup_bg_x, 0x2b4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, keyboard_powerup_spacing_x, 0x2b8);

    if (gInventory_cycling) {
        if (gInventory_selected >= 0) {
            if (GetTotalTime() >= gInventory_timeout) {
                gInventory_timeout = 0;
                gInventory_cycling = 0;
            }
            x = gCurrent_graf_data->keyboard_powerup_x;
            i = gInventory_selected;
            do {
                if (gInventory[i] >= 0) {
                    x = DrawSinglePowerupIcon(0, pTime, &gPowerup_array[gInventory[i]], NULL,
                        x, gCurrent_graf_data->keyboard_powerup_y, 1) + gCurrent_graf_data->keyboard_powerup_spacing_x;
                }
                i += 1;
                if (i >= REC2_ASIZE(gInventory)) {
                    i = 0;
                }
            } while (i != gInventory_selected);
            if (x >= 0) {
                DimRectangle(gBack_screen, gCurrent_graf_data->keyboard_powerup_bg_x, gCurrent_graf_data->keyboard_powerup_bg_y,
                    x, gCurrent_graf_data->keyboard_powerup_bg_bottom, 1);
                i = gInventory_selected;
                x = gCurrent_graf_data->keyboard_powerup_x;
                TransDRPixelmapText(gBack_screen,
                    gCurrent_graf_data->keyboard_powerup_bg_x + 1, gCurrent_graf_data->keyboard_powerup_bg_bottom - 9,
                    &gFonts[2],
                    gPowerup_array[gInventory[gInventory_selected]].message,
                    gCurrent_graf_data->keyboard_powerup_bg_x + 100);
                do {
                    if (gInventory[i] >= 0 && gInventory_cycling) {
                        x = DrawSinglePowerupIcon(1, pTime, &gPowerup_array[gInventory[i]], NULL,
                            x, gCurrent_graf_data->keyboard_powerup_y, 1) + gCurrent_graf_data->keyboard_powerup_spacing_x;
                    }
                    i += 1;
                    if (i >= REC2_ASIZE(gInventory)) {
                        i = 0;
                    }
                } while (i != gInventory_selected);
            }
        }
    } else {
        if (gInventory_selected >= 0) {
            DrawSinglePowerupIcon(1, pTime, gPowerup_array + gInventory[gInventory_selected], NULL,
            gCurrent_graf_data->keyboard_powerup_x, gCurrent_graf_data->keyboard_powerup_y, 0);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004dad00
void C2_HOOK_FASTCALL DrawPowerups(tU32 pTime) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, power_up_icon_x, 0x290);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, power_up_icon_y, 0x298);

    DrawPowerupIcons2(pTime, gPickedup_powerups, &gNumber_of_icons, 5,
        gCurrent_graf_data->power_up_icon_x,
        gCurrent_graf_data->power_up_icon_y);
    DrawKeyPowerups(pTime);
}

// FUNCTION: CARMA2_HW 0x004e0750
void C2_HOOK_FASTCALL MayQueuePowerupRespawn(int pPowerup_index, br_actor* actor) {
    int i;

    if (actor != NULL && gRace_powerup_respawn_bools[pPowerup_index] && (gNet_mode == eNet_mode_none || gCurrent_net_game->options.powerup_respawn)) {
        for (i = 0; i < REC2_ASIZE(gRespawn_powerups); i++) {
            if (gRespawn_powerups[i].actor != NULL) {
                gRespawn_powerups[i].actor = actor;
                gRespawn_powerups[i].index = pPowerup_index;;
                gRespawn_powerups[i].respawn_time = GetTotalTime() + gPickup_respawn_max_extra_time_ms / 2 + gPickup_respawn_min_time_ms;
                break;
            }
        }
    }
}
