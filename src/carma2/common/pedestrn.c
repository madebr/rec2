#include "pedestrn.h"

#include "animation.h"
#include "errors.h"
#include "globvars.h"
#include "graphics.h"
#include "loading.h"
#include "skidmark.h"
#include "smashing.h"
#include "utility.h"
#include "world.h"

#include "brender/brender.h"

#include "c2_stdlib.h"
#include "c2_string.h"

#include "rec2_macros.h"
#include "rec2_types.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(int, gPed_cache_sizes_2, 4, 0x0065d7a8, {
    5, 10, 40, 75,
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(int, gPed_cache_sizes_1, 4, 0x0065d7b8, {
    40, 25, 15, 5,
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tPed_face_cache_0x34*, gPed_face_caches, 4, 0x0069bc58);
C2_HOOK_VARIABLE_IMPLEMENT(tPed_face_cache_0x50*, gPed_face_cache, 0x0069bc54);
C2_HOOK_VARIABLE_IMPLEMENT(int, gSelected_ped, 0x007447f0);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gPed_shade, 0x00694458);

C2_HOOK_VARIABLE_IMPLEMENT(tExplosion_animation, gPed_explosion_small_blood_cloud, 0x00694478);
C2_HOOK_VARIABLE_IMPLEMENT(tExplosion_animation, gPed_medium_blood_cloud, 0x00694480);
C2_HOOK_VARIABLE_IMPLEMENT(tExplosion_animation, gPed_large_blood_cloud, 0x00694488);
C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gPed_slick_material, 0x0069bc10);
C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gPed_smear_material, 0x00694460);
C2_HOOK_VARIABLE_IMPLEMENT(tSlick_spec, gPed_slick, 0x00694238);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPed_spurty_lumps_count, 0x0069427c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPed_spurting_period, 0x0069445c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPed_dismember_spurting_member_count, 0x00694454);
C2_HOOK_VARIABLE_IMPLEMENT(tSmashable_initial_position_spec, gPed_spurt_initial_position, 0x0069fda8);
C2_HOOK_VARIABLE_IMPLEMENT(tSmashable_initial_speed_spec, gPed_spurt_initial_speed, 0x006944a0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gPed_spurt_min_max_time, 2, 0x006942f8);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gPed_spurt_model, 0x00694278);
C2_HOOK_VARIABLE_IMPLEMENT(int, gGiblet_scrape_start, 0x0069411c);
C2_HOOK_VARIABLE_IMPLEMENT(tSmashable_initial_position_spec, gGiblet_initial_position, 0x00694468);
C2_HOOK_VARIABLE_IMPLEMENT(tSmashable_initial_speed_spec, gGiblet_initial_speed, 0x0069bc38);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gGiblet_min_max_time, 2, 0x006a0408);
C2_HOOK_VARIABLE_IMPLEMENT(int, gGiblet_size_count, 0x0069bc18);
C2_HOOK_VARIABLE_IMPLEMENT(tPed_giblet_size_spec*, gGiblet_sizes, 0x00694274);

C2_HOOK_VARIABLE_IMPLEMENT(tPedForms_vtable, gPed_forms_vtable, 0x0065d778);
C2_HOOK_VARIABLE_IMPLEMENT(float, gPed_process_distance, 0x00694118);
C2_HOOK_VARIABLE_IMPLEMENT(float, gPed_process_distance_inner, 0x0069bc1c);
C2_HOOK_VARIABLE_IMPLEMENT(float, gPed_popup_distance, 0x006944bc);
C2_HOOK_VARIABLE_IMPLEMENT(float, gPed_popup_distance_inner, 0x00694128);
C2_HOOK_VARIABLE_IMPLEMENT(float, gPed_cos_max_slope, 0x00694464);
C2_HOOK_VARIABLE_IMPLEMENT(float, gPed_reach_squared, 0x0069413c);
C2_HOOK_VARIABLE_IMPLEMENT(float, gPed_min_dist_avoid_collisions_squared, 0x00694300);
C2_HOOK_VARIABLE_IMPLEMENT(float, gPed_buoyancy_factor, 0x007447fc);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gPed_points, 3, 0x0069bcf8);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gPed_times, 3, 0x00694268);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPed_severing_damage, 0x00694234);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPed_severing_chance, 0x0069bc0c);
C2_HOOK_VARIABLE_IMPLEMENT(float, gPed_max_survivable_impact, 0x00694494);
C2_HOOK_VARIABLE_IMPLEMENT(float, gPed_damage_per_force, 0x0069bc20);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPed_movements_count, 0x0069bcec);
C2_HOOK_VARIABLE_IMPLEMENT(tPed_movement_spec*, gPed_movements, 0x0069bcf0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPed_groups_count, 0x00694134);
C2_HOOK_VARIABLE_IMPLEMENT(tPed_group_spec*, gPed_groups, 0x0069bc14);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPed_animal_count, 0x00694230);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tPed_animal_name, gPed_animal_names, 50, 0x0069fdb8);
C2_HOOK_VARIABLE_IMPLEMENT(tExplosion_animation, gNapalmed_ped_animation, 0x0069bc28);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_material*, gBurning_ped_materials, 7, 0x0069fd88);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_model*, gBurning_ped_models, 7, 0x00694308);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tBurning_ped, gBurning_peds, 5, 0x0069b9f0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gBurning_ped_map_names, 7, 0x0065e598, {
    "Ex00006",
    "Ex00006",
    "Ex00006",
    "Ex00006",
    "Ex00006",
    "Ex00006",
    "Ex00006",
});
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_race_pedestrian_specs, 0x0069bce8);
C2_HOOK_VARIABLE_IMPLEMENT(tRace_ped_spec*, gRace_pedestrian_specs, 0x00694138);
C2_HOOK_VARIABLE_IMPLEMENT(tPedestrian*, gPedestrian_array, 0x00744808);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPed_count, 0x007447d4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCamera_is_teleporting, 0x00694490);

void (C2_HOOK_FASTCALL * InitPedsForm_original)(tPedForms_vtable* pTable);
void C2_HOOK_FASTCALL InitPedsForm(tPedForms_vtable* pTable) {
#if defined(C2_HOOKS_ENABLED)

    InitPedsForm_original(pTable);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00403ed0, InitPedsForm, InitPedsForm_original)

void C2_HOOK_FAKE_THISCALL ScaleModelXYZ(br_model* pModel, int pArg2, float pX, float pY, float pZ) {
    int i;

    REC2_THISCALL_UNUSED(pArg2);

    for (i = 0; i < pModel->nvertices; i++) {
        pModel->vertices[i].p.v[0] *= pX;
        pModel->vertices[i].p.v[1] *= pY;
        pModel->vertices[i].p.v[2] *= pZ;
    }
    BrModelUpdate(pModel, BR_MODU_VERTEX_POSITIONS);
}
C2_HOOK_FUNCTION(0x00404290, ScaleModelXYZ)

void C2_HOOK_FAKE_THISCALL ScaleModel(br_model* pModel, int pArg2, float pScale) {
    int i;

    REC2_THISCALL_UNUSED(pArg2);

    for (i = 0; i < pModel->nvertices; i++) {
        pModel->vertices[i].p.v[0] *= pScale;
        pModel->vertices[i].p.v[1] *= pScale;
        pModel->vertices[i].p.v[2] *= pScale;
    }
    BrModelUpdate(pModel, BR_MODU_VERTEX_POSITIONS);
}
C2_HOOK_FUNCTION(0x004042f0, ScaleModel)

br_model* ReadSmashableInitialModel(FILE* pFile, br_scalar pScale_factor) {
    float scale;
    int index;
    int r, g, b;
    char s[256];
    br_model* model;

    /* Scaling factor */
    scale = GetAScalar(pFile);
    /* Indexed colour */
    index = GetAnInt(pFile);
    /* rgb colour */
    GetThreeInts(pFile, &r, &g, &b);
    /* Model name */
    GetAString(pFile, s);

    model = BrModelFind(s);
    if (model == NULL) {
        FatalError(kFatalError_CannotFindGibletModel_S, s);
    }
    ScaleModel(model REC2_THISCALL_EDX, scale * pScale_factor);
    model->faces[0].material->index_base = index;
    model->faces[0].material->index_range = 1;
    model->faces[0].material->colour = BR_COLOUR_RGB(r, g, b);
    BrMaterialUpdate(model->faces[0].material, BR_MATU_ALL);
    return model;
}

void C2_HOOK_FASTCALL ReadPedGiblets(FILE* pFile) {
    tPath_name the_path;
    int i;
    int j;

    PathCat(the_path, C2V(gApplication_path), C2V(gPedsFolder));
    PathCat(the_path, the_path, "GIBLETS");
    LoadFolderInStorageWithShading(&C2V(gMisc_storage_space), the_path, kRendererShadingType_Diffuse1);

    /*
     * ***************************
     *         GIBLETS
     * ***************************
     *
     * IMPACT GIBLETS
     */

    /* Min index usable for scrapes (not impacts) */
    C2V(gGiblet_scrape_start) = GetAnInt(pFile);

    ReadSmashableInitialPosition(pFile, &C2V(gGiblet_initial_position));

    ReadSmashableInitialSpeed(pFile, &C2V(gGiblet_initial_speed));

    ReadMinMaxTimeInMilliseconds(pFile, C2V(gGiblet_min_max_time));

    REC2_BUG_ON(sizeof(tPed_giblet_size_spec) != 24);

    /* Number of giblet sizes */
    C2V(gGiblet_size_count) = GetAnInt(pFile);
    C2V(gGiblet_sizes) = BrMemAllocate(C2V(gGiblet_size_count) * sizeof(tPed_giblet_size_spec), kMem_misc_poly_ped);

    for (i = 0; i < C2V(gGiblet_size_count); i++) {
        /* flags */
        C2V(gGiblet_sizes)[i].flags = GetAnInt(pFile);
        /* Min force for creation */
        C2V(gGiblet_sizes)[i].min_force = GetAScalar(pFile);
        /* Force per instance */
        C2V(gGiblet_sizes)[i].instance_force = GetAScalar(pFile);
        /* Max instances */
        C2V(gGiblet_sizes)[i].max_count = GetAnInt(pFile);
        /* Model count */
        C2V(gGiblet_sizes)[i].model_count = GetAnInt(pFile);

        for (j = 0; j < C2V(gGiblet_sizes)[i].model_count; j++) {
            float factor;

            factor = 1.f;
            if (i == C2V(gGiblet_size_count) - 1) {
                factor = 3.f / 4.f;
            }
            C2V(gGiblet_sizes)[i].models[j] = ReadSmashableInitialModel(pFile, factor);
        }

    }

    /* SMALL BLOOD CLOUD SPEC */
    ReadExplosionAnimation(pFile, &C2V(gPed_explosion_small_blood_cloud));
    /* MED BLOOD CLOUD SPEC */
    ReadExplosionAnimation(pFile, &C2V(gPed_medium_blood_cloud));
    /* LARGE BLOOD CLOUD SPEC */
    ReadExplosionAnimation(pFile, &C2V(gPed_large_blood_cloud));

    C2V(gPed_slick_material) = BrMaterialAllocate("GIBSLICK");
    BrMaterialAdd(C2V(gPed_slick_material));
    C2V(gPed_slick_material)->flags |= BR_MATF_LIGHT;
    C2V(gPed_slick_material)->flags |= BR_MATF_PERSPECTIVE;
    C2V(gPed_slick_material)->flags |= BR_MATF_SMOOTH;
    C2V(gPed_slick_material)->ka = .99f;
    C2V(gPed_slick_material)->kd = .0f;
    C2V(gPed_slick_material)->ks = .0f;
    C2V(gPed_slick_material)->power = .0f;
    C2V(gPed_slick_material)->index_base = 0;
    C2V(gPed_slick_material)->index_range = 0;
    BrMatrix23Identity(&C2V(gPed_slick_material)->map_transform);
    C2V(gPed_slick_material)->index_shade = BrTableFind("IDENTITY.TAB");
    C2V(gPed_slick_material)->colour_map = BrTableFind("GIBSLICK");
    GlorifyMaterial(&C2V(gPed_slick_material), 1, kRendererShadingType_Diffuse2);

    C2V(gPed_smear_material) = BrMaterialFind("PEDSMEAR");

    /* GIB SLICK! */
    ReadSlick(pFile, &C2V(gPed_slick));

    /* SPURTING STUMP GIBLETS */
    /* Number of spurty lumps at a time */
    C2V(gPed_spurty_lumps_count) = GetAnInt(pFile);
    /* Spurting period (euurrgghhh!!) */
    C2V(gPed_spurting_period) = GetAnInt(pFile);
    /* Total number of parts spurted out by dismembered part */
    C2V(gPed_dismember_spurting_member_count) = GetAnInt(pFile);

    ReadSmashableInitialPosition(pFile, &C2V(gPed_spurt_initial_position));

    ReadSmashableInitialSpeed(pFile, &C2V(gPed_spurt_initial_speed));

    ReadMinMaxTimeInMilliseconds(pFile, C2V(gPed_spurt_min_max_time));

    C2V(gPed_spurt_model) = ReadSmashableInitialModel(pFile, 1.f);
}
C2_HOOK_FUNCTION(0x004cb1e0, ReadPedGiblets)

void (C2_HOOK_FASTCALL * InitPeds_original)(void);
void C2_HOOK_FASTCALL InitPeds(void) {
#if 0//defined(C2_HOOKS_ENABLED)
    InitPeds_original();
#else
    tPath_name the_path;
    FILE* file;
    int i;
    int j;
    float v;

    if (C2V(gPedsFolder) == NULL) {
        SetDefaultPedFolderNames();
    }

    InitPedsForm(&C2V(gPed_forms_vtable));

    PathCat(the_path, C2V(gApplication_path), C2V(gPedsFolder));
    PathCat(the_path, the_path, "SETTINGS.TXT");
    file = DRfopen(the_path, "rt");
    if (file == NULL) {
        FatalError(kFatalError_CannotOpenPedFile_S, "SETTINGS.TXT");
    }
    /* ***************************
     *        GENERAL STUFF
     * ***************************
     */

    /* Max distance from camera that peds are processed within */
    C2V(gPed_process_distance) = GetAScalar(file);
    C2V(gPed_process_distance_inner) = 0.6f * C2V(gPed_process_distance);
    /* Min distance from camera that peds can suddenly pop up at */
    C2V(gPed_popup_distance) = GetAScalar(file);
    C2V(gPed_popup_distance_inner) = 0.6f * C2V(gPed_popup_distance);
    /* Maximum slope that peds will walk on (in degreesm) */
    C2V(gPed_cos_max_slope) = BR_COS(BR_ANGLE_DEG(GetAScalar(file)));

    /* Nearest distance to obstacles that ped should ideally reach */
    v = GetAScalar(file);
    C2V(gPed_reach_squared) = v * v;

    /* Distance from which turning to avoid obstacles is a possibility */
    v = GetAScalar(file);
    C2V(gPed_min_dist_avoid_collisions_squared) = v * v;
    /* Buoyancy factor */
    C2V(gPed_buoyancy_factor) = GetAScalar(file);
    /* Points score per ped (each level) */
    GetThreeInts(file, &C2V(gPed_points)[0], &C2V(gPed_points)[1], &C2V(gPed_points)[2]);
    /* Time score per ped (each level) */
    GetThreeInts(file, &C2V(gPed_times)[0], &C2V(gPed_times)[1], &C2V(gPed_times)[2]);
    /* HP damage required per severing */
    C2V(gPed_severing_damage) = GetAnInt(file);
    /* % chance of severing occurring */
    C2V(gPed_severing_chance) = GetAnInt(file);
    /* Maximum survivable initial impact force */
    C2V(gPed_max_survivable_impact) = GetAScalar(file);
    /* HP Damage per unit force */
    C2V(gPed_damage_per_force) = GetAScalar(file);
    ReadPedGiblets(file);

    /* NAPALMED PED SPEC */
    ReadExplosionAnimation(file, &C2V(gNapalmed_ped_animation));

    /*
     * ***************************
     *       MOVEMENT STUFF
     * ***************************
     */

    C2_HOOK_BUG_ON(sizeof(tPed_movement_spec) != 20);

    /* Number of movements */
    C2V(gPed_movements_count) = GetAnInt(file);
    C2V(gPed_movements) = BrMemAllocate(C2V(gPed_movements_count) * sizeof(tPed_movement_spec), kMem_misc_poly_ped);

    for (i = 0; i < C2V(gPed_movements_count); i++) {
        /* Min time between random turns */
        C2V(gPed_movements)[i].min_time_between = (int)(1000.f * GetAScalar(file));
        /* Max time between random turns */
        C2V(gPed_movements)[i].max_time_between = (int)(1000.f * GetAScalar(file));
        /* Max angle of random turn */
        C2V(gPed_movements)[i].max_random_angle = BR_ANGLE_DEG(GetAScalar(file));
        /* Min walk speed multiplier */
        C2V(gPed_movements)[i].min_walk_speed_factor = 1000.f / GetAScalar(file) * 30.f;
        /* Max walk speed multiplier */
        C2V(gPed_movements)[i].max_walk_speed_factor = 1000.f / GetAScalar(file) * 30.f;
    }

    /*
     * ***************************
     *       PED GROUPS
     * ***************************
     */

    C2_HOOK_BUG_ON(sizeof(tPed_group_spec) != 8);

    /* Number of groups
     * (Remember that it starts at 0, so this no. is one more than last group no.)(honest this time ;o) */
    C2V(gPed_groups_count) = GetAnInt(file);
    C2V(gPed_groups) = BrMemAllocate(C2V(gPed_groups_count) * sizeof(tPed_group_spec), kMem_misc_poly_ped);

    for (i = 0; i < C2V(gPed_groups_count); i++) {
        C2_HOOK_BUG_ON(sizeof(tPed_group_member_spec) != 16);

        /* Number of peds types in this group */
        C2V(gPed_groups)[i].count = GetAnInt(file);
        C2V(gPed_groups)[i].members = BrMemAllocate(C2V(gPed_groups)[i].count * sizeof(tPed_group_member_spec), kMem_misc_poly_ped);

        for (j = 0; j < C2V(gPed_groups)[i].count; j++) {
            /* Name of ped */
            GetAString(file, C2V(gPed_groups)[i].members[j].name);
        }
    }

    /*
     * ***************************
     *       GORE LEVEL
     * ***************************
     */

    /* Number of peds to be considered "animals" */
    C2V(gPed_animal_count) = GetAnInt(file);
    for (i = 0; i < C2V(gPed_animal_count); i++) {
        GetAString(file, (char*)(C2V(gPed_animal_names)[i]));
    }

    PFfclose(file);

    C2_HOOK_BUG_ON(sizeof(tPed_face_cache_0x34) != 52);
    C2_HOOK_BUG_ON(sizeof(tPed_face_cache_0x50) != 80);

    for (i = 0; i < REC2_ASIZE(C2V(gPed_cache_sizes_1)); i++) {
        C2V(gPed_face_caches)[i] = BrMemAllocate(C2V(gPed_cache_sizes_1)[i] * sizeof(tPed_face_cache_0x34), kMem_ped_face_cache);
        for (j = 0; j < C2V(gPed_cache_sizes_1)[i]; j++) {
            C2V(gPed_face_caches)[i][j].field_0x0 = BrMemAllocate(C2V(gPed_cache_sizes_2)[i] * sizeof(tPed_face_cache_0x50), kMem_ped_face_cache);
            C2V(gPed_face_caches)[i][j].count = C2V(gPed_cache_sizes_2)[i];
        }
    }
    C2V(gPed_face_cache) = BrMemAllocate((C2V(gPed_cache_sizes_2)[3] + 1) * sizeof(tPed_face_cache_0x50), kMem_ped_face_cache);
    InitBurningPeds();
    C2V(gSelected_ped) = 0;
    C2V(gPed_shade) = GenerateShadeTable(8, C2V(gRender_palette), 0xd7, 0xff, 0xe9, .5f, .75f, .9f);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004cadc0, InitPeds, InitPeds_original)

void (C2_HOOK_FASTCALL * InitBurningPeds_original)(void);
void C2_HOOK_FASTCALL InitBurningPeds(void) {
#if 0//defined(C2_HOOKS_ENABLED)
    InitBurningPeds_original();
#else
    int i;
    int j;
    br_model* model;

    for (i = 0; i < REC2_ASIZE(C2V(gBurning_ped_materials)); i++) {
        model = BrModelAllocate(NULL, 4, 2);
        C2V(gBurning_ped_models)[i] = model;
        model->faces[0].vertices[0] = 0;
        model->faces[0].vertices[1] = 2;
        model->faces[0].vertices[2] = 1;
        model->faces[0].material = NULL;
        model->faces[0].smoothing = 1;
        model->faces[1].vertices[0] = 0;
        model->faces[1].vertices[1] = 3;
        model->faces[1].vertices[2] = 2;
        model->faces[1].material = NULL;
        model->faces[1].smoothing = 1;

        BrVector3Set(&model->vertices[0].p, -.5f, -.5f, 0.f);
        BrVector2Set(&model->vertices[0].map, 0.f, 1.f);
        BrVector3Set(&model->vertices[1].p, -.5f, 1.f, 0.f);
        BrVector2Set(&model->vertices[1].map, 0.f, 0.f);
        BrVector3Set(&model->vertices[2].p, 1.f, 1.f, 0.f);
        BrVector2Set(&model->vertices[2].map, 1.f, 0.f);
        BrVector3Set(&model->vertices[2].p, 1.f, -.5f, 0.f);
        BrVector2Set(&model->vertices[2].map, 1.f, 1.f);

        BrModelAdd(model);

        C2V(gBurning_ped_materials)[i] = BrMaterialAllocate("BURN!");
        C2V(gBurning_ped_materials)[i]->flags &= ~BR_MATF_LIGHT;
        C2V(gBurning_ped_materials)[i]->flags |= BR_MATF_ALWAYS_VISIBLE;
        C2V(gBurning_ped_materials)[i]->colour_map = BrMapFind(C2V(gBurning_ped_map_names)[i]);
        if (C2V(gBurning_ped_materials)[i]->colour_map == NULL) {
            FatalError(kFatalError_CantFindPedTexture_S, C2V(gBurning_ped_map_names)[i]);
        }
        GlorifyMaterial(&C2V(gBurning_ped_materials)[i], 1, kRendererShadingType_Diffuse2);
        BrMaterialAdd(C2V(gBurning_ped_materials)[i]);
    }

    C2_HOOK_BUG_ON(sizeof(tBurning_ped) != 0x6c);
    for (i = 0; i < REC2_ASIZE(C2V(gBurning_peds)); i++) {
        C2V(gBurning_peds)[i].field_0x0 = 0;
        for (j = 0; j < REC2_ASIZE(C2V(gBurning_ped_materials)); j++) {
            br_actor* actor;

            actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            C2V(gBurning_peds)[i].actors[j] = actor;
            actor->material = C2V(gBurning_ped_materials)[j];
            actor->model = C2V(gBurning_ped_models)[j];
            actor->render_style = BR_RSTYLE_NONE;
            BrActorAdd(C2V(gNon_track_actor), actor);
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004cb630, InitBurningPeds, InitBurningPeds_original)

int (C2_HOOK_FASTCALL * ForEachMortalPedestrianInSight_original)(tCar_spec* pCar_spec, int pOnly_alive, float pMax_distance, int pParam_4, void* pData, tForEachPedestrian_cbfn* pCallback);
int C2_HOOK_FASTCALL ForEachMortalPedestrianInSight(tCar_spec* pCar_spec, int pOnly_alive, float pMax_distance, int pParam_4, void* pData, tForEachPedestrian_cbfn* pCallback) {

#if defined(C2_HOOKS_ENABLED)
    return ForEachMortalPedestrianInSight_original(pCar_spec, pOnly_alive, pMax_distance, pParam_4, pData, pCallback);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ce140, ForEachMortalPedestrianInSight, ForEachMortalPedestrianInSight_original)

void C2_HOOK_FASTCALL LoadTrackPedestrians(FILE* pF) {
    int i;
    char s[256];
    char s2[256];

    /* Number of ped spawn sets */
    C2V(gCount_race_pedestrian_specs) = GetAnInt(pF);

    C2_HOOK_BUG_ON(sizeof(tRace_ped_spec) != 0x20);
    C2V(gRace_pedestrian_specs) = BrMemAllocate(C2V(gCount_race_pedestrian_specs) * sizeof(tRace_ped_spec), kMem_misc_poly_ped);

    for (i = 0; i < C2V(gCount_race_pedestrian_specs); i++) {
        tRace_ped_spec* spec = &C2V(gRace_pedestrian_specs)[i];
        int index;
        int j;
        int exclusions_ok_when_scared = 1;

        /* Material name */
        GetAString(pF, s);
        spec->spawn_material = BrMaterialFind(s);
        if (spec->spawn_material == NULL) {
            FatalError(kFatalError_CannotFindPedSpawnMaterial_S, s);
        }

        /* Movement index */
        index = GetAnInt(pF);
        if (index < 0 || index >= C2V(gPed_movements_count)) {
            FatalError(kFatalError_CannotMovePedIndexOutOfRangeForMaterial_S, s);
        }
        spec->movement = &C2V(gPed_movements)[index];

        /* Group index */
        index = GetAnInt(pF);
        if (index < 0 || index >= C2V(gPed_groups_count)) {
            FatalError(kFatalError_CannotGroupPedIndexOutOfRangeForMaterial_S, s);
        }
        spec->group = &C2V(gPed_groups)[index];

        /* Peds per 100 square metres */
        spec->density = GetAScalar(pF);

        spec->field_0x10 = 0;

        /* Number of exclusion materials */
        spec->count_exclusion_materials = GetAnInt(pF);

        spec->exclusion_materials = BrMemAllocate(spec->count_exclusion_materials * sizeof(tRace_spec_exclusion_material), kMem_misc_poly_ped);
        C2_HOOK_BUG_ON(sizeof(tRace_spec_exclusion_material) != 8);

        exclusions_ok_when_scared = 1;

        for (j = 0; j < spec->count_exclusion_materials; j++) {
            tRace_spec_exclusion_material* exclusion = &spec->exclusion_materials[j];

            /* Exclusion flags (1 = OK when scared) */
            exclusion->flags = GetAnInt(pF);
            exclusions_ok_when_scared &= exclusion->flags;

            /* Exclusion material #1 name */
            GetAString(pF, s2);
            exclusion->material = BrMaterialFind(s2);
            if (exclusion->material == NULL) {
                FatalError(kFatalError_CannotFindPedSpawnMaterial_S, s2);
            }
        }
        if (exclusions_ok_when_scared) {
            spec->exclusions_ok_when_scared = 1;
        }

        /* Number of exception materials */
        spec->count_exception_materials = GetAnInt(pF);
        spec->exception_materials = BrMemAllocate(spec->count_exception_materials * sizeof(br_material*), kMem_misc_poly_ped);

        for (j = 0; j < spec->count_exception_materials; j++) {

            GetAString(pF, s2);
            spec->exception_materials[j] = BrMaterialFind(s2);
            if (spec->exception_materials[j] == NULL) {
                FatalError(kFatalError_CannotFindPedSpawnMaterial_S, s2);
            }
        }
    }

    for (i = 0; i < C2V(gCount_race_pedestrian_specs); i++) {
        tRace_ped_spec* spec = &C2V(gRace_pedestrian_specs)[i];
        size_t len;

        len = c2_strlen(spec->spawn_material->identifier);
        if (len < 4) {
            len = 4;
        }
        if (len - 4 < 8) {
            c2_strcpy(s2, spec->spawn_material->identifier);
            c2_strcpy(&s2[len], "xxxxxxxx.MAT");
            BrResFree(spec->spawn_material->identifier);
            spec->spawn_material->identifier = BrResStrDup(spec->spawn_material, s2);
        }
        spec->spawn_material->identifier[7] = '?';
    }

    C2V(gPedestrian_array) = BrMemAllocate(2000 * sizeof(tPedestrian), kMem_ped_array);
    C2_HOOK_BUG_ON(sizeof(tPedestrian) != 84);
}
C2_HOOK_FUNCTION(0x004ca9f0, LoadTrackPedestrians)

void (C2_HOOK_FASTCALL * MaybeSpawnPedestrian_original)(br_face *pFace, br_model *pModel);
void C2_HOOK_FASTCALL MaybeSpawnPedestrian(br_face *pFace, br_model *pModel) {

#if defined(C2_HOOKS_ENABLED)
    MaybeSpawnPedestrian_original(pFace, pModel);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004d2cc0, MaybeSpawnPedestrian, MaybeSpawnPedestrian_original)

void C2_HOOK_FASTCALL AllocateTrackPedestrians(void) {
    tPedestrian* new_pedestrians;
    int i;

    dr_dprintf("%d pedestrians, %d bytes each, total = %d bytes\n", C2V(gPed_count), sizeof(tRace_pedestrian), C2V(gPed_count) * sizeof(tRace_pedestrian));
    C2_HOOK_BUG_ON(sizeof(tRace_pedestrian) != 320);

    new_pedestrians = BrMemAllocate(C2V(gPed_count) * sizeof(tPedestrian), kMem_ped_array);
    C2_HOOK_BUG_ON(sizeof(tPedestrian) != 84);

    c2_memcpy(new_pedestrians, C2V(gPedestrian_array), C2V(gPed_count) * sizeof(tPedestrian));
    C2_HOOK_BUG_ON(sizeof(tRace_pedestrian) != 320);

    BrMemFree(C2V(gPedestrian_array));
    C2V(gPedestrian_array) = new_pedestrians;

    for (i = 0; i < C2V(gPed_count); i++) {

        C2V(gPedestrian_array)[i].race_ped->ped = &C2V(gPedestrian_array)[i];
        C2_HOOK_BUG_ON(offsetof(tRace_pedestrian, ped) != 228);
    }
}
C2_HOOK_FUNCTION(0x004d3520, AllocateTrackPedestrians)

void (C2_HOOK_FASTCALL * DisposePedestrians_original)(void);
void C2_HOOK_FASTCALL DisposePedestrians(void) {

#if defined(C2_HOOKS_ENABLED)
    DisposePedestrians_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004cb8d0, DisposePedestrians, DisposePedestrians_original)

void (C2_HOOK_FASTCALL * ResetPeds_original)(void);
void C2_HOOK_FASTCALL ResetPeds(void) {

#if defined(C2_HOOKS_ENABLED)
    ResetPeds_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004d5bd0, ResetPeds, ResetPeds_original)

void C2_HOOK_FASTCALL CameraIsMovingReallyFast(void) {

    C2V(gCamera_is_teleporting) = 1;
}
C2_HOOK_FUNCTION(0x004d6390, CameraIsMovingReallyFast)
