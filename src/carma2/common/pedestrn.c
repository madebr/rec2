#include "pedestrn.h"

#include "animation.h"
#include "errors.h"
#include "globvars.h"
#include "globvrpb.h"
#include "graphics.h"
#include "loading.h"
#include "physics.h"
#include "platform.h"
#include "skidmark.h"
#include "smashing.h"
#include "trig.h"
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
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tPed_face_cache_0x34*, gPed_face_cache_heads, 4, 0x0069bc58);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tPed_face_cache_0x34*, gPed_face_caches, 4, 0x006a03f8);
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
C2_HOOK_VARIABLE_IMPLEMENT_INIT(const br_vector3, g_Ped_x_unit_vector, 0x0058f2a8, { { 1.f, 0.f, 0.f}});
C2_HOOK_VARIABLE_IMPLEMENT_INIT(const br_vector3, g_Ped_y_unit_vector, 0x0058f2b8, { { 0.f, 1.f, 0.f}});
C2_HOOK_VARIABLE_IMPLEMENT_INIT(const br_vector3, g_Ped_z_unit_vector, 0x0058f2c8, { { 0.f, 0.f, 1.f}});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gPed_form_collision_type_names, 9, 0x0058f230, {
    "BOX",
    "POLYHEDRON",
    "LINE",
    "POLYGON",
    "SPHERE",
    "",
    "",
    "",
    "NONE",
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gPed_form_bone_joint_type_names, 10, 0x0058f258, {
    "NONE",
    "HINGE",
    "UNIVERSAL",
    "BALLNSOCKET",
    "QUICKHINGE",
    "",
    "",
    "",
    "PHANTOM",
    "FALSE",
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gPed_form_joint_limit_type_names, 2, 0x0058f280, {
    "PLANE",
    "UNIVERSAL",
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_material*, gPedestrian_character_cloned_materials, 50, 0x00676d68);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_pedestrian_personality_cloned_materials, 0x0067772c);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gPed_personality_grounding_type_names, 1, 0x0058f288, {
    "GROUND",
});

C2_HOOK_VARIABLE_IMPLEMENT_INIT(tPedForms_vtable, gPed_forms_vtable, 0x0065d778, {
    CBPassiveCollision,
    CBActiveHalted,
    CBMovedByPhysics,
    CBMoveCompleted,
    CBFillInObject,
    CBLoadForm,
    CBDisposeForm,
    CBLoadPersonality,
    CBDisposePersonality,
});
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
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tNapalm_bolt, gNapalm_bolts, 5, 0x0069b9f0);
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
C2_HOOK_VARIABLE_IMPLEMENT(int, gPed_nearness, 0x00694490);
C2_HOOK_VARIABLE_IMPLEMENT(int, gTotal_count_smash_peds, 0x0074480c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_killed_peds, 0x007447cc);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(tPed_form*, gPed_forms, 20, 0x00676928, { 0 });
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tPed_personality*, gPed_personalities, 50, 0x00677260);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(tPed_move*, gPed_moves, 250, 0x00676980, { 0 });
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tPed_remap*, gPed_remaps, 10, 0x00677238);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tPed_morph, gPed_morphs, 20, 0x00677730);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(float, gPed_sin_table, 256, 0x00677328);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(float, gPed_cos_table, 256, 0x00676e30);
C2_HOOK_VARIABLE_IMPLEMENT(undefined4, gDAT_00677728, 0x00677728);
C2_HOOK_VARIABLE_IMPLEMENT(float, gFLOAT_00677230, 0x00677230);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_0067772c, 0x0067772c);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gPed_overall_movement_disabled, 0x00676978, 0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPed_retain_root_mode, 0x00677234);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_changed_points, 0x0069412c);
C2_HOOK_VARIABLE_IMPLEMENT(tTWTVFS, gTwtPeds, 0x00694498);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPed_score_multiplier, 0x0069bce4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPed_recent_points, 0x0069bc30);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPipe_halted_ped_status, 0x006a0410);
C2_HOOK_VARIABLE_IMPLEMENT(int, gImmortal_peds, 0x00744810);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPeds_suicidal, 0x007447ac);
C2_HOOK_VARIABLE_IMPLEMENT(float, gMutant_speed, 0x007447a4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gExploding_pedestrians, 0x007447b0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPed_dismemberfest, 0x007447e4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gBlind_pedestrians, 0x007447d0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDancing_peds, 0x007447a8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPanicking_peds, 0x007447f8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDrunk_pedestrians, 0x007447b8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gEthereal_pedestrians, 0x007447a0);
C2_HOOK_VARIABLE_IMPLEMENT(float, gPedestrian_speed_factor, 0x007447e0);
C2_HOOK_VARIABLE_IMPLEMENT(br_vector3, gPed_scale, 0x007447c0);
C2_HOOK_VARIABLE_IMPLEMENT(float, gAverage_ped_scale, 0x0069bc24);
C2_HOOK_VARIABLE_IMPLEMENT(float, gPed_head_scale, 0x00744800);
C2_HOOK_VARIABLE_IMPLEMENT(float, gPed_gravity_multiplier, 0x007447dc);
C2_HOOK_VARIABLE_IMPLEMENT(int, gBOOL_00744804, 0x00744804);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPed_valium_left, 0x00744804);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tPed_cache_006944c0, gPed_cache_006944c0, 100, 0x006944c0);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tPedForms_vtable*, gPed_vtable, 0x0067697c, NULL);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gPed_remap_axis_choices, 6, 0x0058f290, {
    "+X",
    "+Y",
    "+Z",
    "-X",
    "-Y",
    "-Z",
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gForm_non_humanoid_names, 1, 0x0065e560, {
    "NONHUMANOID",
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_ADV_INIT(int, gPed_move_fsm, [118][7], 0x0065d7f0, {
    {   0,  -2,   5,   6, 111,  31, 112 },
    {   0,   0,   5,   6, 111,  31, 112 },
    {   0,   1,   5,   6, 111,  31, 112 },
    {   0,   2,   5,   6, 111,  31, 112 },
    {   0,   2,   5,   6, 111,  31, 112 },
    {   0,   2,   2,   6, 111,  31, 112 },
    {   0,   2,   2,   6, 111,  31, 112 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,   2,   5,   6, 111,  31, 112 },
    {   0,  10,   5,   6, 111,  31, 112 },
    {   0,   2,   5,   6, 111,  31, 112 },
    {   0,  10,   5,   6, 111,  31, 112 },
    {   0,   1,   5,   6, 111,  31, 112 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   1,  -1,  -1,  -1,  -1,  -1,  -1 },
    {   1,  -1,  -1,  -1,  -1,  -1,  -1 },
    {   1,  -1,  -1,  -1,  -1,  -1,  -1 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   1,  -2,  50,  51, 111,  31, 112 },
    {   1,  30,  31,  31, 111,  31, 112 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   1,  -2,  50,  51, 111,  31, 112 },
    {   1,  40,  50,  51, 111,  31, 112 },
    {   1,  40,  50,  51, 111,  31, 112 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   1,  52,  50,  51, 111,  31, 112 },
    {   1,  52,  50,  51, 111,  31, 112 },
    {   1,  40,  50,  51, 111,  31, 112 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   1,  -1,  -1,  -1,  -1,  -1,  -1 },
    {   1,  -1,  -1,  -1,  -1,  -1,  -1 },
    {   1,  -1,  -1,  -1,  -1,  -1,  -1 },
    {   1,  -1,  -1,  -1,  -1,  -1,  -1 },
    {   1,  -1,  -1,  -1,  -1,  -1,  -1 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   1,  -1,  -1,  -1,  -1,  -1,  -1 },
    {   1,  72,  71,  71,  71,  71,  71 },
    {   1,  -2,  72,  72,  72,  72,  72 },
    {   1,  -1,  -1,  -1,  -1,  -1,  -1 },
    {   1,  -2,  74,  74,  74,  74,  74 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -2,  82,  83,  80,  80,  80 },
    {   0,  -2,  84,  85,  81,  81,  81 },
    {   0,  80,  82,  83,  80,  80,  80 },
    {   0,  80,  82,  83,  80,  80,  80 },
    {   0,  81,  84,  85,  81,  81,  81 },
    {   0,  81,  84,  85,  81,  81,  81 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  91,  90,  90,  90,  90,  90 },
    {   0,  -2,  91,  91,  91,  91,  91 },
    {   0,  91,  92,  92,  92,  92,  92 },
    {   0,  94,  93,  93,  93,  93,  93 },
    {   0,  -2,  94,  94,  94,  94,  94 },
    {   0,  -2,  95,  95,  95,  95,  95 },
    {   0,  91,  96,  96,  96,  96,  96 },
    {   0,  72,  97,  97,  97,  97,  97 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   1,  72, 110, 110, 110, 110, 110 },
    {   1,  -2, 111, 111, 111,  31, 112 },
    {   1,  -2, 112, 112, 111,  31, 112 },
    {   0,  91, 113, 113, 113, 113, 113 },
    {   2,   2,   2,   2,   2,   2,   3 },
    {   4,   2,   2,   2,  10,  11,  13 },
    { -41, -41, -41,   0,   2,   2,   2 },
    {   2,   2,   3,   4,   4,  10,  11 },
});

#define PED_SCALAR_EPSILON (2.384186e-6f)

void C2_HOOK_FASTCALL ClearOutMorphs(void) {
    int i;

    for (i = 0; i < REC2_ASIZE(C2V(gPed_morphs)); i++) {
        C2V(gPed_morphs)[i].field_0x4 = 0;
    }
}

void (C2_HOOK_FASTCALL * InitPedsForm_original)(tPedForms_vtable* pTable);
void C2_HOOK_FASTCALL InitBoner(tPedForms_vtable* pTable) {
#if 0//defined(C2_HOOKS_ENABLED)

    InitPedsForm_original(pTable);
#else
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gPed_forms)) != 20);
    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gPed_personalities)) != 50);
    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gPed_moves)) != 250);
    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gPed_remaps)) != 10);

    C2V(gPed_vtable) = pTable;
    c2_memset(C2V(gPed_forms), 0, sizeof(C2V(gPed_forms)));
    c2_memset(C2V(gPed_personalities), 0, sizeof(C2V(gPed_personalities)));
    c2_memset(C2V(gPed_moves), 0, sizeof(C2V(gPed_moves)));
    c2_memset(C2V(gPed_remaps), 0, sizeof(C2V(gPed_remaps)));

    ClearOutMorphs();

    for (i = 0; i < 256; i++) {
        float a = (i * 256) * 2 * REC2_PI_F / 256.f / 256.f;
        C2V(gPed_sin_table)[i] = sinf(a);
        C2V(gPed_cos_table)[i] = cosf(a);
    }

    C2V(gDAT_00677728) = 0;
    C2V(gFLOAT_00677230) = 1.f;
    C2V(gINT_0067772c) = 0;
    C2V(gPed_overall_movement_disabled) = 0;
    C2V(gPed_retain_root_mode) = 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00403ed0, InitBoner, InitPedsForm_original)

void C2_HOOK_FASTCALL DRVector3SafeCross(br_vector3* pDest, const br_vector3* pA, const br_vector3* pB) {

    BrVector3Cross(pDest, pA, pB);
    if (fabsf(pDest->v[0]) < PED_SCALAR_EPSILON && fabsf(pDest->v[0]) < PED_SCALAR_EPSILON && fabsf(pDest->v[2]) < PED_SCALAR_EPSILON) {
        if (fabsf(pA->v[0]) <= fabsf(pA->v[1]) && fabsf(pA->v[0]) <= fabsf(pA->v[2])) {
            BrVector3Cross(pDest, pA, &C2V(g_Ped_x_unit_vector));
        } else if (fabsf(pA->v[1]) <= fabsf(pA->v[0]) && fabsf(pA->v[1]) <= fabsf(pA->v[2])) {
            BrVector3Cross(pDest, pA, &C2V(g_Ped_y_unit_vector));
        } else {
            BrVector3Cross(pDest, pA, &C2V(g_Ped_z_unit_vector));
        }
    }
}
C2_HOOK_FUNCTION(0x00403f90, DRVector3SafeCross)

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
    LoadAllStuffInDirectory(&C2V(gMisc_storage_space), the_path, kRendererShadingType_Diffuse1);

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
    ReadExplosion(pFile, &C2V(gPed_explosion_small_blood_cloud));
    /* MED BLOOD CLOUD SPEC */
    ReadExplosion(pFile, &C2V(gPed_medium_blood_cloud));
    /* LARGE BLOOD CLOUD SPEC */
    ReadExplosion(pFile, &C2V(gPed_large_blood_cloud));

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
    ReadSpillData(pFile, &C2V(gPed_slick));

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

void C2_HOOK_FASTCALL ReadSettingsFile(void) {
    tPath_name the_path;
    FILE* file;
    int i;
    int j;
    float v;

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
    ReadExplosion(file, &C2V(gNapalmed_ped_animation));

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
}

void C2_HOOK_FASTCALL InitFaceCaches(void) {
    int i;
    int j;

    C2_HOOK_BUG_ON(sizeof(tPed_face_cache_0x34) != 52);
    C2_HOOK_BUG_ON(sizeof(tPed_face_cache_0x50) != 80);

    for (i = 0; i < REC2_ASIZE(C2V(gPed_cache_sizes_1)); i++) {
        C2V(gPed_face_cache_heads)[i] = BrMemAllocate(C2V(gPed_cache_sizes_1)[i] * sizeof(tPed_face_cache_0x34), kMem_ped_face_cache);
        for (j = 0; j < C2V(gPed_cache_sizes_1)[i]; j++) {
            C2V(gPed_face_cache_heads)[i][j].field_0x0 = BrMemAllocate(C2V(gPed_cache_sizes_2)[i] * sizeof(tPed_face_cache_0x50), kMem_ped_face_cache);
            C2V(gPed_face_cache_heads)[i][j].count = C2V(gPed_cache_sizes_2)[i];
        }
    }
    C2V(gPed_face_cache) = BrMemAllocate((C2V(gPed_cache_sizes_2)[3] + 1) * sizeof(tPed_face_cache_0x50), kMem_ped_face_cache);
}

void C2_HOOK_FASTCALL InitOtherPedStuff(void) {

    C2V(gSelected_ped) = 0;
    C2V(gPed_shade) = GenerateShadeTable(8, C2V(gRender_palette), 0xd7, 0xff, 0xe9, .5f, .75f, .9f);
}

void (C2_HOOK_FASTCALL * InitPeds_original)(void);
void C2_HOOK_FASTCALL InitPolyPedSystem(void) {
#if 0//defined(C2_HOOKS_ENABLED)
    InitPeds_original();
#else

    if (C2V(gPedsFolder) == NULL) {
        SetDefaultPedFolderNames();
    }

    InitBoner(&C2V(gPed_forms_vtable));
    ReadSettingsFile();
    InitFaceCaches();
    InitNapalmNolts();
    InitOtherPedStuff();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004cadc0, InitPolyPedSystem, InitPeds_original)

void (C2_HOOK_FASTCALL * InitBurningPeds_original)(void);
void C2_HOOK_FASTCALL InitNapalmNolts(void) {
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

    C2_HOOK_BUG_ON(sizeof(tNapalm_bolt) != 0x6c);
    for (i = 0; i < REC2_ASIZE(C2V(gNapalm_bolts)); i++) {
        C2V(gNapalm_bolts)[i].field_0x0 = 0;
        for (j = 0; j < REC2_ASIZE(C2V(gBurning_ped_materials)); j++) {
            br_actor* actor;

            actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            C2V(gNapalm_bolts)[i].actors[j] = actor;
            actor->material = C2V(gBurning_ped_materials)[j];
            actor->model = C2V(gBurning_ped_models)[j];
            actor->render_style = BR_RSTYLE_NONE;
            BrActorAdd(C2V(gNon_track_actor), actor);
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004cb630, InitNapalmNolts, InitBurningPeds_original)

int (C2_HOOK_FASTCALL * DoToPeds_original)(tCar_spec* pCar_spec, int pOnly_alive, float pMax_distance, int pParam_4, void* pData, tForEachPedestrian_cbfn* pCallback);
int C2_HOOK_FASTCALL DoToPeds(tCar_spec* pCar_spec, int pOnly_alive, float pMax_distance, int pParam_4, void* pData, tForEachPedestrian_cbfn* pCallback) {

#if defined(C2_HOOKS_ENABLED)
    return DoToPeds_original(pCar_spec, pOnly_alive, pMax_distance, pParam_4, pData, pCallback);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ce140, DoToPeds, DoToPeds_original)

void (C2_HOOK_FASTCALL * ReadPedSpecs_original)(FILE* pF);
void C2_HOOK_FASTCALL ReadPedSpecs(FILE* pF) {
#if 0//defined(C2_HOOKS_ENABLED)
    ReadPedSpecs_original(pF);
#else
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
        spec->exclusion.count_exclusion_materials = GetAnInt(pF);

        spec->exclusion.exclusion_materials = BrMemAllocate(spec->exclusion.count_exclusion_materials * sizeof(tRace_spec_exclusion_material), kMem_misc_poly_ped);
        C2_HOOK_BUG_ON(sizeof(tRace_spec_exclusion_material) != 8);

        exclusions_ok_when_scared = 1;

        for (j = 0; j < spec->exclusion.count_exclusion_materials; j++) {
            tRace_spec_exclusion_material* exclusion = &spec->exclusion.exclusion_materials[j];

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
            spec->exclusion.exclusions_ok_when_scared = 1;
        }

        /* Number of exception materials */
        spec->exclusion.count_exception_materials = GetAnInt(pF);
        spec->exclusion.exception_materials = BrMemAllocate(spec->exclusion.count_exception_materials * sizeof(br_material*), kMem_misc_poly_ped);

        for (j = 0; j < spec->exclusion.count_exception_materials; j++) {

            GetAString(pF, s2);
            spec->exclusion.exception_materials[j] = BrMaterialFind(s2);
            if (spec->exclusion.exception_materials[j] == NULL) {
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

    C2V(gPedestrian_array) = BrMemAllocate(PEDESTRIAN_MAX_COUNT * sizeof(tPedestrian), kMem_ped_array);
    C2_HOOK_BUG_ON(sizeof(tPedestrian) != 0x54);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca9f0, ReadPedSpecs, ReadPedSpecs_original)

tPed_personality* FindOrOpenPersonality(const char* pName) {
    int i;

    for (i = 0; i < REC2_ASIZE(C2V(gPed_personalities)); i++) {
        if (C2V(gPed_personalities)[i] == NULL) {
            continue;
        }
        if (c2_strcmp(C2V(gPed_personalities)[i]->name, pName) == 0) {
            return C2V(gPed_personalities)[i];
        }
    }
    return ReadPersonality(pName);
}

tPed_character_instance* (C2_HOOK_FASTCALL * BuildCharacterInstance_original)(const char* pGroup_name, br_matrix34* pMat34);
tPed_character_instance* C2_HOOK_FASTCALL BuildCharacterInstance(const char* pGroup_name, br_matrix34* pMat34) {

#if 0//defined(C2_HOOKS_ENABLED)
    return BuildCharacterInstance_original(pGroup_name, pMat34);
#else
    tPed_character_instance* instance;

    C2_HOOK_BUG_ON(sizeof(tPed_character_instance) != 0xec);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_character_instance, field_0x4, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_character_instance, field_0x6, 0x6);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_character_instance, field_0x7, 0x7);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_character_instance, field_0x8, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_character_instance, field_0x9, 0x9);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_character_instance, field_0xa, 0xa);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_character_instance, field_0xc, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_character_instance, field_0x14, 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_character_instance, field_0x28, 0x28);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_character_instance, field_0x2c, 0x2c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_character_instance, field_0x5c, 0x5c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_character_instance, field_0x8c, 0x8c);

    instance = BrMemAllocate(sizeof(tPed_character_instance), kBoner_mem_type_char_instance);
    if (instance == NULL) {
        FatalError(kFatalError_BonerError_UnableToAllocateMemory, 0);
    }
    instance->personality = FindOrOpenPersonality(pGroup_name);
    instance->field_0x4 = 0xff;
    instance->field_0x6 = 0xff;
    instance->field_0x7 = 0xff;
    instance->field_0x8 = 0;
    instance->field_0x9 = 0;
    instance->field_0xa = 0;
    instance->field_0xc = 0;
    instance->field_0x14 = 0;
    instance->field_0x28 = 0;
    instance->field_0x2c = *pMat34;
    BrMatrix34Identity(&instance->field_0x5c);
    BrMatrix34Identity(&instance->field_0x8c);
    return instance;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004079a0, BuildCharacterInstance, BuildCharacterInstance_original)

br_matrix34* C2_HOOK_FASTCALL GetCharacterMatrixPtr(tPed_character_instance *pCharacter) {

    if (pCharacter->field_0x4 < 0) {
        return &pCharacter->field_0x2c;
    }
    if (pCharacter->field_0xe8 != NULL) {
        return pCharacter->field_0xe8;
    }
    return &pCharacter->personality->form->actor_sets[pCharacter->field_0x4].actors[0]->t.t.mat;
}
C2_HOOK_FUNCTION(0x00407aa0, GetCharacterMatrixPtr)

void (C2_HOOK_CDECL * TurnLimbsOnAndOff_original)(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen);
void C2_HOOK_CDECL TurnLimbsOnAndOff(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen) {

#if defined(C2_HOOKS_ENABLED)

    TurnLimbsOnAndOff_original(actor, model, material, render_data, style, on_screen);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004d34e0, TurnLimbsOnAndOff, TurnLimbsOnAndOff_original)

void (C2_HOOK_FASTCALL * SetCharacterBonePositions_original)(tPed_character_instance* pPed, undefined4 pArg2, undefined4 pArg3);
void C2_HOOK_FASTCALL SetCharacterBonePositions(tPed_character_instance* pPed, undefined4 pArg2, undefined4 pArg3) {

#if defined(C2_HOOKS_ENABLED)
    SetCharacterBonePositions_original(pPed, pArg2, pArg3);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00407b30, SetCharacterBonePositions, SetCharacterBonePositions_original)

int (C2_HOOK_FASTCALL * SetCharacterMove_original)(tPed_character_instance* pPed,int pMove_action, float pSpeed, int pArg4, int pArg5, undefined4 pArg6);
int C2_HOOK_FASTCALL SetCharacterMove(tPed_character_instance* pPed,int pMove_action, float pSpeed, int pArg4, int pArg5, undefined4 pArg6) {

#if defined(C2_HOOKS_ENABLED)
    return SetCharacterMove_original(pPed, pMove_action, pSpeed, pArg4, pArg5, pArg6);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0040a050, SetCharacterMove, SetCharacterMove_original)

void (C2_HOOK_FASTCALL * SetPedMove_original)(tPedestrian* pPed, int ppMove_action, int pWalk_speed_factor, int pArg4, int pArg5, int pArg6, int pAction);
void C2_HOOK_FASTCALL SetPedMove(tPedestrian* pPed, int pMove_action, int pWalk_speed_factor, int pArg4, int pArg5, int pArg6, int pAction) {

#if defined(C2_HOOKS_ENABLED)
    SetPedMove_original(pPed, pMove_action, pWalk_speed_factor, pArg4, pArg5, pArg6, pAction);
#else
    tPed_action original_action;
    int original_move_id;
    undefined2 original_field_0x1e;
    undefined2 original_field_0x1c;
    br_vector3 original_pos;
    br_matrix34* character_matrix;
    int new_move_lut_id;
    int i;

    original_action = pPed->action;
    original_move_id = pPed->character->personality->form->moves[pPed->character->field_0x7].id;
    original_field_0x1e = pPed->character->field_0x1e;
    original_field_0x1c = pPed->character->field_0x1c;
    character_matrix = GetCharacterMatrixPtr(pPed->character);
    BrVector3Copy(&original_pos, (br_vector3*)character_matrix->m[3]);
    if (pAction >= 0 && pMove_action >= 0 && pPed->action != ePed_action_dead) {
        pPed->action = pAction;
    }
    if (C2V(gDrunk_pedestrians)) {
        new_move_lut_id = 4;
    } else if (C2V(gDancing_peds)) {
        new_move_lut_id = 3;
    } else if (C2V(gPanicking_peds)) {
        new_move_lut_id = 5;
    } else if (pPed->character->field_0xc & 0x8) {
        new_move_lut_id = 1;
    } else if (pPed->character->field_0xc & 0x20) {
        new_move_lut_id = 2;
    } else {
        new_move_lut_id = 0;
    }
    if (new_move_lut_id != 0) {
        pMove_action = C2V(gPed_move_fsm)[pMove_action][new_move_lut_id + 1];
    }
    if (pMove_action == 112 || pMove_action == 31 || pMove_action == 111) {
        pPed->action = ePed_action_running;
    }
    if (pPed->field_0x0c != NULL && pMove_action != 112 && pMove_action != 31
            && pMove_action != 40 && pMove_action != 41 && pMove_action != 42) {
        pPed->field_0x0c->field_0x80 = 0;
    }
    for (i = 0; i < 10; i++) {
        int move_result;
        float speed;

        speed = (float)pWalk_speed_factor;
        if (C2V(gPedestrian_speed_factor) != 0.f) {
            speed /= C2V(gPedestrian_speed_factor);
        }
        move_result = SetCharacterMove(pPed->character, pMove_action, speed, pArg4, pArg5, pArg6);
        if (move_result == 2) {
            pMove_action = C2V(gPed_move_fsm)[pMove_action][1 + 0];
            if (pMove_action < -1) {
                char buffer[256];

                c2_sprintf(buffer, "%d", pMove_action);
                if (pMove_action == -2) {
                    FatalError(kFatalError_CoreMoveMissingFor_SS, buffer, pPed->character->personality->name);
                } else {
                    FatalError(kFatalError_BadMoveAttemptedFor_SS, buffer, pPed->character->personality->name);
                }
            }
        }
        if (pMove_action == -1 || move_result != 2) {
            break;
        }
    }
    if (i >= 10) {
        char buffer[256];

        c2_sprintf(buffer, "%d", pMove_action);
        FatalError(kFatalError_CyclicMoveDefinitionFor_SS, buffer, pPed->character->personality->name);
    }
    PipeSinglePedMove(pPed, original_field_0x1c, original_move_id,
        pMove_action, original_field_0x1e, pWalk_speed_factor, pArg5, pArg4,
        &original_pos, (br_vector3*)character_matrix->m[3], original_action, pPed->action, &pPed->character->field_0x8c);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004cbcd0, SetPedMove, SetPedMove_original)

void (C2_HOOK_FASTCALL * SetCharacterDirectionAR_original)(tPed_character_instance* pPed, br_vector3* pDir, br_vector3* pUp);
void C2_HOOK_FASTCALL SetCharacterDirectionAR(tPed_character_instance* pPed, br_vector3* pDir, br_vector3* pUp) {

#if defined(C2_HOOKS_ENABLED)
    SetCharacterDirectionAR_original(pPed, pDir, pUp);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004d3360, SetCharacterDirectionAR, SetCharacterDirectionAR_original)

br_scalar C2_HOOK_FASTCALL AreaOfTriangle(br_vector3* pV0, br_vector3* pV1, br_vector3* pV2) {

    return fabsf(pV0->v[0] * (pV1->v[2] - pV2->v[2]) + pV1->v[0] * (pV2->v[2] - pV0->v[2]) + pV2->v[0] * (pV0->v[2]  - pV1->v[2])) / 2.f;
}

void C2_HOOK_FASTCALL MakeRandomPointInTriangle(br_vector3* pDest, br_vector3* pV0, br_vector3* pV1, br_vector3* pV2) {
    br_scalar alpha;
    br_scalar beta;
    br_scalar gamma;

    alpha = SRandomBetween(0.1f, 0.9f);
    beta = SRandomBetween(0.1f, 0.9f);
    if (alpha + beta > 1.f) {
        alpha = 1.f - alpha;
        beta = 1.f - beta;
    }
    gamma = 1 - alpha - beta;
    pDest->v[0] = gamma * pV0->v[0] + alpha * pV1->v[0] + beta * pV2->v[0];
    pDest->v[1] = gamma * pV0->v[1] + alpha * pV1->v[1] + beta * pV2->v[1];
    pDest->v[2] = gamma * pV0->v[2] + alpha * pV1->v[2] + beta * pV2->v[2];
}

void C2_HOOK_FASTCALL SetModelCallbacks(tPed_character_instance* pPed) {
    int i;

    C2_HOOK_BUG_ON(sizeof(tPed_character_instance) != 0xec);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_character_instance, personality, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_personality, bones, 0x2c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_personality_bone, models, 0x0);

    C2_HOOK_BUG_ON(REC2_ASIZE(pPed->personality->bones->models) != 4);
    for (i = 0; i < REC2_ASIZE(pPed->personality->bones->models); i++) {
        int j;

        C2_HOOK_BUG_ON(REC2_ASIZE(pPed->personality->bones->models[i].models) != 2);
        for (j = 0; j < REC2_ASIZE(pPed->personality->bones->models[i].models); j++) {
            br_model* model;

            model = pPed->personality->bones->models[i].models[j];
            if (model != NULL) {
                model->flags |= BR_MODF_CUSTOM;
                model->custom = TurnLimbsOnAndOff;
            }
        }
    }
}

void C2_HOOK_FASTCALL BuildPedestrian(tPedestrian* pPed, const char* pGroup_name, br_vector3* pPos, br_vector3* pDir, tPed_movement_spec* pMovement_spec, tRace_ped_exclusion_spec* pExclusion_spec) {
    br_matrix34 mat;
    char texture_name[32];
    char material_name[32];
    br_pixelmap* texture;
    br_material* material;

    BrMatrix34Identity(&mat);
    BrVector3Copy((br_vector3*)mat.m[3], pPos);
    pPed->character = BuildCharacterInstance(pGroup_name, &mat);
    SetModelCallbacks(pPed->character);
    c2_sprintf(texture_name, "%sB", pGroup_name);
    texture = BrMapFind(texture_name);
    if (texture == NULL) {
        FatalError(kFatalError_CantFindPedTexture_S, texture_name);
    }
    c2_sprintf(material_name, "%s%d.MAT", pGroup_name, 1);
    material = BrMaterialFind(material_name);
    if (material != NULL) {
        material->colour_map = texture;
        BrMaterialUpdate(material, BR_MATU_COLOURMAP);
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPedestrian, character, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPedestrian, hit_points, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPedestrian, action, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPedestrian, movement_spec, 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPedestrian, exclusion_spec, 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPedestrian, pos, 0x1c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPedestrian, speed_factor, 0x4c);

    pPed->hit_points = 100;
    pPed->movement_spec = pMovement_spec;
    pPed->speed_factor = 1.f;
    pPed->exclusion_spec = pExclusion_spec;
    if (pMovement_spec->max_walk_speed_factor != 0.f) {
        SetPedMove(pPed, 30, IRandomBetween((int)pMovement_spec->min_walk_speed_factor, (int)pMovement_spec->max_walk_speed_factor), 0, 0, 0, 0);
    } else {
        SetPedMove(pPed, 0, -1, 0, 0, 0, 0);
    }
    BrVector3Normalise(pDir, pDir);
    SetCharacterDirectionAR(pPed->character, pDir, &C2V(y_unit_vector));
    BrVector3Copy(&pPed->pos, pPos);
}

void (C2_HOOK_FASTCALL * SpawnPedsOnFace_original)(br_face *pFace, br_model *pModel);
void C2_HOOK_FASTCALL SpawnPedsOnFace(br_face *pFace, br_model *pModel) {

#if 0//defined(C2_HOOKS_ENABLED)
    SpawnPedsOnFace_original(pFace, pModel);
#else
    br_material* material;
    tRace_ped_spec* ped_spec;
    int i;
    br_vector3 v0;
    br_vector3 v1;
    br_vector3 v2;
    float fNew_ped_count;
    int new_ped_count;

    if (C2V(gNet_mode) != eNet_mode_none) {
        return;
    }
    if (C2V(gPed_cos_max_slope) >= pFace->n.v[1]) {
        return;
    }
    material = pFace->material;
    if (material == NULL) {
        return;
    }
    if (material->identifier == NULL || c2_strlen(material->identifier) - 4 <= 7) {
        return;
    }
    if (material->identifier[7] != '?') {
        return;
    }
    if (C2V(gPed_count) >= PEDESTRIAN_MAX_COUNT) {
        return;
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tRace_ped_spec, movement, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tRace_ped_spec, spawn_material, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tRace_ped_spec, density, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tRace_ped_spec, field_0x10, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tRace_ped_spec, exclusion, 0x14);

    ped_spec = NULL;
    for (i = 0; i < C2V(gCount_race_pedestrian_specs); i++) {
        ped_spec = &C2V(gRace_pedestrian_specs)[i];
        if (ped_spec->spawn_material == material) {
            break;
        }
    }

#ifdef REC2_FIX_BUGS
    if (ped_spec == NULL) {
        PDFatalError("Could not find ped spec");
    }
#endif
    BrVector3Copy(&v0, &pModel->vertices[pFace->vertices[0]].p);
    BrVector3Copy(&v1, &pModel->vertices[pFace->vertices[1]].p);
    BrVector3Copy(&v2, &pModel->vertices[pFace->vertices[2]].p);
    if (ped_spec->density >= 0.f) {
        fNew_ped_count = ped_spec->field_0x10 + WORLD_SCALE * WORLD_SCALE * AreaOfTriangle(&v0, &v1, &v2) * ped_spec->density / 1000.f;
    } else {
        fNew_ped_count = -ped_spec->density;
    }
    new_ped_count = (int)floorf(fNew_ped_count);

    for (i = 0; i < new_ped_count; i++) {
        char group_name[32];
        int illegal_ped = 0;
        br_vector3 ped_dir;
        br_vector3 ped_pos;

        if (C2V(gPed_count) >= PEDESTRIAN_MAX_COUNT) {
            return;
        }
        c2_strcpy(group_name, ped_spec->group->members[IRandomBetween(0, ped_spec->group->count - 1)].name);
        if (!C2V(gAnimalsOn)) {
            int j;

            for (j = 0; j < C2V(gPed_animal_count); j++) {
                if (DRStricmp(C2V(gPed_animal_names)[j], group_name) == 0) {
                    illegal_ped = 1;
                }
            }
        }
        if (illegal_ped) {
            continue;
        }
        BrVector3Set(&ped_dir, SRandomPosNeg(1.f), 0.f, SRandomPosNeg(1.f));
        MakeRandomPointInTriangle(&ped_pos, &v0, &v1, &v2);
        BuildPedestrian(&C2V(gPedestrian_array)[C2V(gPed_count)], group_name, &ped_pos, &ped_dir,
                        ped_spec->movement, &ped_spec->exclusion);

        if (C2V(gCurrent_race).race_spec->race_type == kRaceType_SmashNPed) {
            if (C2V(gCurrent_race).race_spec->options.smash_and_peds.ped_group_index == ped_spec->group - C2V(gPed_groups)) {
                C2V(gPedestrian_array)[C2V(gPed_count)].flags |= 0x200;
                AddSmashableRaceTarget(NULL, NULL, C2V(gPed_count));
            }
        } else if (C2V(gCurrent_race).race_spec->race_type == kRaceType_Peds) {
            int smash_number = C2V(gCurrent_race).race_spec->options.smash_and_peds.var_smash_number;
            if (smash_number >= 0) {
                int j;
                for (j = 0; j < smash_number; j++) {
                    if (ped_spec->group - C2V(gPed_groups) == C2V(gCurrent_race).race_spec->options.smash_and_peds.var_smash_target) {
                        C2V(gTotal_count_smash_peds) += 1;
                        C2V(gPedestrian_array)[C2V(gPed_count)].flags |= 0x200;
                        break;
                    }
                }
            } else {
                C2V(gTotal_count_smash_peds) += 1;
                C2V(gPedestrian_array)[C2V(gPed_count)].flags |= 0x200;
            }
        }
        C2V(gPed_count) += 1;
    }
    ped_spec->field_0x10 = fNew_ped_count - new_ped_count;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004d2cc0, SpawnPedsOnFace, SpawnPedsOnFace_original)

void (C2_HOOK_FASTCALL * FinishUpLoadingPeds_original)(void);
void C2_HOOK_FASTCALL FinishUpLoadingPeds(void) {
#if 0//defined(C2_HOOKS_ENABLED)
    FinishUpLoadingPeds_original();
#else
    tPedestrian* new_pedestrians;
    int i;

    dr_dprintf("%d pedestrians, %d bytes each, total = %d bytes\n", C2V(gPed_count), sizeof(tRace_pedestrian), C2V(gPed_count) * sizeof(tRace_pedestrian));
    C2_HOOK_BUG_ON(sizeof(tRace_pedestrian) != 0x140);

    new_pedestrians = BrMemAllocate(C2V(gPed_count) * sizeof(tPedestrian), kMem_ped_array);
    C2_HOOK_BUG_ON(sizeof(tPedestrian) != 0x54);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPedestrian, character, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_character_instance, ped, 0xe4);

    c2_memcpy(new_pedestrians, C2V(gPedestrian_array), C2V(gPed_count) * sizeof(tPedestrian));

    BrMemFree(C2V(gPedestrian_array));
    C2V(gPedestrian_array) = new_pedestrians;

    for (i = 0; i < C2V(gPed_count); i++) {

        C2V(gPedestrian_array)[i].character->ped = &C2V(gPedestrian_array)[i];
        C2_HOOK_BUG_ON(offsetof(tRace_pedestrian, ped) != 0xe4);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004d3520, FinishUpLoadingPeds, FinishUpLoadingPeds_original)

void (C2_HOOK_FASTCALL * DisposePedestrians_original)(void);
void C2_HOOK_FASTCALL DisposePedestrians(void) {

#if defined(C2_HOOKS_ENABLED)
    DisposePedestrians_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004cb8d0, DisposePedestrians, DisposePedestrians_original)

void C2_HOOK_FASTCALL ResetPedFaceCache(void) {
    int i;

    for (i = 0; i < REC2_ASIZE(C2V(gPed_cache_sizes_1)); i++) {
        int j;

        C2V(gPed_face_caches)[i] = C2V(gPed_face_cache_heads)[i];
        for (j = 0; j < C2V(gPed_cache_sizes_1)[i]; j++) {
            C2V(gPed_face_cache_heads)[i][j].field_0x14 = 0;
            if (j != C2V(gPed_cache_sizes_1)[REC2_ASIZE(C2V(gPed_cache_sizes_1)) - 1]) {
                C2V(gPed_face_cache_heads)[i][j].next = &C2V(gPed_face_cache_heads)[i][j + 1];
                C2V(gPed_face_cache_heads)[i][j + 1].prev = &C2V(gPed_face_cache_heads)[i][j];
            } else {
                C2V(gPed_face_cache_heads)[i][j].next = &C2V(gPed_face_cache_heads)[i][0];
                C2V(gPed_face_cache_heads)[i][0].prev = &C2V(gPed_face_cache_heads)[i][j];
            }
        }
    }
}

void C2_HOOK_FASTCALL ResetPedSystem(void) {
    ResetPedFaceCache();
}

void C2_HOOK_FASTCALL ResetNapalmBolts(void) {
    int i;

    for (i = 0; i < REC2_ASIZE(C2V(gNapalm_bolts)); i++) {
        int j;

        C2V(gNapalm_bolts)[0].field_0x0 = 0;
        for (j = 0; j < REC2_ASIZE(C2V(gNapalm_bolts)[0].actors); j++) {
            C2V(gNapalm_bolts)[0].actors[j]->render_style = BR_RSTYLE_NONE;
        }
    }
}

void (C2_HOOK_FASTCALL * InitPedsForRace_original)(void);
void C2_HOOK_FASTCALL InitPedsForRace(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    InitPedsForRace_original();
#else
    C2_HOOK_BUG_ON(sizeof(C2V(gPed_cache_006944c0)) != 30000);

    ResetPedSystem();
    C2V(gPed_count) = 0;
    C2V(gCount_changed_points) = 0;
    C2V(gTwtPeds) = -1;
    C2V(gTotal_count_smash_peds) = 0;
    C2V(gPed_score_multiplier) = 0;
    C2V(gPed_recent_points) = 0;
    C2V(gPipe_halted_ped_status) = 0;
    C2V(gImmortal_peds) = 0;
    C2V(gPeds_suicidal) = 0;
    C2V(gMutant_speed) = 0.f;
    C2V(gExploding_pedestrians) = 0;
    C2V(gPed_dismemberfest) = 0;
    C2V(gBlind_pedestrians) = 0;
    C2V(gDancing_peds) = 0;
    C2V(gPanicking_peds) = 0;
    C2V(gDrunk_pedestrians) = 0;
    C2V(gEthereal_pedestrians) = 0;
    C2V(gPedestrian_speed_factor) = 1.f;
    BrVector3Set(&C2V(gPed_scale), 1.f, 1.f, 1.f);
    C2V(gAverage_ped_scale) = 1.f;
    C2V(gPed_head_scale) = 1.f;
    C2V(gPed_gravity_multiplier) = 1.f;
    C2V(gBOOL_00744804) = 0;
    C2V(gCount_killed_peds) = 0;
    C2V(gPed_valium_left) = 0;
    ResetNapalmBolts();
    c2_memset(C2V(gPed_cache_006944c0), 0, sizeof(C2V(gPed_cache_006944c0)));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004d5bd0, InitPedsForRace, InitPedsForRace_original)

void C2_HOOK_FASTCALL ResetPedNearness(void) {

    C2V(gPed_nearness) = 1;
}
C2_HOOK_FUNCTION(0x004d6390, ResetPedNearness)

void (C2_HOOK_FASTCALL * LastChanceForPedEffects_original)(void);
void C2_HOOK_FASTCALL LastChanceForPedEffects(void) {

#if defined(C2_HOOKS_ENABLED)
    LastChanceForPedEffects_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004d5970, LastChanceForPedEffects, LastChanceForPedEffects_original)

void (C2_HOOK_FASTCALL * MungeNapalm_original)(void);
void C2_HOOK_FASTCALL MungeNapalm(void) {

#if defined(C2_HOOKS_ENABLED)
    MungeNapalm_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ce7d0, MungeNapalm, MungeNapalm_original)

void (C2_HOOK_FASTCALL * MungePedestrians_original)(void);
void C2_HOOK_FASTCALL MungePedestrians(void) {

#if defined(C2_HOOKS_ENABLED)
    MungePedestrians_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004d3740, MungePedestrians, MungePedestrians_original)

void C2_HOOK_FASTCALL MakeFlagWavingBastardWaveHisFlagWhichIsTheProbablyTheLastThingHeWillEverDo(void) {

}
C2_HOOK_FUNCTION(0x004d35d0, MakeFlagWavingBastardWaveHisFlagWhichIsTheProbablyTheLastThingHeWillEverDo)

void (C2_HOOK_FASTCALL * FlushAllPedCaches_original)(void);
void C2_HOOK_FASTCALL FlushAllPedCaches(void) {

#if defined(C2_HOOKS_ENABLED)
    FlushAllPedCaches_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004d6c70, FlushAllPedCaches, FlushAllPedCaches_original)

undefined4 (C2_HOOK_FASTCALL * CBPassiveCollision_original)(undefined4* pArg1, undefined4 pArg2, undefined4* pArg3);
undefined4 C2_HOOK_FASTCALL CBPassiveCollision(undefined4* pArg1, undefined4 pArg2, undefined4* pArg3) {
#if defined(C2_HOOKS_ENABLED)
    return CBPassiveCollision_original(pArg1, pArg2, pArg3);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004cdc00, CBPassiveCollision, CBPassiveCollision_original)

int (C2_HOOK_FASTCALL * CBActiveHalted_original)(undefined4* pArg1, undefined4* pArg2);
int C2_HOOK_FASTCALL CBActiveHalted(undefined4* pArg1, undefined4* pArg2) {
#if defined(C2_HOOKS_ENABLED)
    return CBActiveHalted_original(pArg1, pArg2);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004d1020, CBActiveHalted, CBActiveHalted_original)

void (C2_HOOK_FASTCALL * CBMovedByPhysics_original)(undefined4* pArg1, undefined4* pArg2, undefined4 pArg3);
void C2_HOOK_FASTCALL CBMovedByPhysics(undefined4* pArg1, undefined4* pArg2, undefined4 pArg3) {
#if defined(C2_HOOKS_ENABLED)
    CBMovedByPhysics_original(pArg1, pArg2, pArg3);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004d2930, CBMovedByPhysics, CBMovedByPhysics_original)

int (C2_HOOK_FASTCALL * CBMoveCompleted_original)(undefined4* pArg1);
int C2_HOOK_FASTCALL CBMoveCompleted(undefined4* pArg1) {
#if defined(C2_HOOKS_ENABLED)
    return CBMoveCompleted_original(pArg1);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004cbf20, CBMoveCompleted, CBMoveCompleted_original)

void (C2_HOOK_FASTCALL * CBFillInObject_original)(undefined4* pArg1, undefined4 pArg2);
void C2_HOOK_FASTCALL CBFillInObject(undefined4* pArg1, undefined4 pArg2) {
#if defined(C2_HOOKS_ENABLED)
    CBFillInObject_original(pArg1, pArg2);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ccab0, CBFillInObject, CBFillInObject_original)

void (C2_HOOK_FASTCALL * CBLoadForm_original)(tPed_form* pPed_form, FILE* pF);
void C2_HOOK_FASTCALL CBLoadForm(tPed_form* pPed_form, FILE* pF) {
#if 0//defined(C2_HOOKS_ENABLED)
    CBLoadForm_original(pPed_form, pF);
#else
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_form, non_humanoid, 0x4c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_form, index_head_bone, 0x4d);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_form, axis_when_lying_down_A, 0x4e);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_form, axis_when_lying_down_B, 0x4f);

    /* Other stuff */

    pPed_form->non_humanoid = GetALineAndInterpretCommand(pF, C2V(gForm_non_humanoid_names), REC2_ASIZE(C2V(gForm_non_humanoid_names))) == 0;

    /* Index of head bone */
    pPed_form->index_head_bone = GetAnInt(pF);

    /* Direction axis when lying down (A) */
    pPed_form->axis_when_lying_down_A = GetAnInt(pF);

    /* Direction axis when lying down (B) */
    pPed_form->axis_when_lying_down_B = GetAnInt(pF);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004cc900, CBLoadForm, CBLoadForm_original)

void (C2_HOOK_FASTCALL * CBDisposeForm_original)(undefined4* pArg1);
void C2_HOOK_FASTCALL CBDisposeForm(undefined4* pArg1) {
#if defined(C2_HOOKS_ENABLED)
    CBDisposeForm_original(pArg1);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004cc940, CBDisposeForm, CBDisposeForm_original)

void (C2_HOOK_FASTCALL * CBLoadPersonality_original)(tPed_personality* pPersonality, FILE* pF);
void C2_HOOK_FASTCALL CBLoadPersonality(tPed_personality* pPersonality, FILE* pF) {
#if defined(C2_HOOKS_ENABLED)
    CBLoadPersonality_original(pPersonality, pF);
#else
    char s[256];
    tPath_name path;
    FILE* sound_f;
    int i;

    C2_HOOK_BUG_ON(sizeof(tPed_personality_sounds) != 0x84);
    C2_HOOK_BUG_ON(REC2_ASIZE(((tPed_personality_sounds*)NULL)->sounds) != 11);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_personality, sounds, 0x64)
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_personality, jump_height, 0x50)

    /* Name of sound definition file */
    GetAString(pF, s);

    PathCat(path, C2V(gApplication_path), C2V(gPedsFolder));
    PathCat(path, path, "SOUND");
    PathCat(path, path, s);
    sound_f = DRfopen(path, "rt");
    if (sound_f == NULL) {
        FatalError(kFatalError_CannotOpenPedFile_S, s);
    }

    pPersonality->sounds = BrMemAllocate(sizeof(tPed_personality_sounds), kMem_misc_poly_ped);

    for (i = 0; i < REC2_ASIZE(pPersonality->sounds->sounds); i++) {
        int j;

        pPersonality->sounds->sounds[i].count_sounds = GetAnInt(sound_f);
        for (j = 0; j < pPersonality->sounds->sounds[i].count_sounds; j++) {
            pPersonality->sounds->sounds[i].sounds[j] = LoadSingleSound(&C2V(gPedStorage), GetAnInt(sound_f));
        }
    }
    PFfclose(sound_f);

    pPersonality->jump_height = 0.f;
    while (!PFfeof(pF)) {
        GetAString(pF, s);
        if (DRStricmp(s,"JUMPHEIGHT") == 0) {
            pPersonality->jump_height = GetAScalar(pF);
        }
    }
    ClosePackFileAndSetTiffLoading(C2V(gTwtPeds));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004cc950, CBLoadPersonality, CBLoadPersonality_original)

void (C2_HOOK_FASTCALL * CBDisposePersonality_original)(undefined4* pArg1);
void C2_HOOK_FASTCALL CBDisposePersonality(undefined4* pArg1) {
#if defined(C2_HOOKS_ENABLED)
    CBDisposePersonality_original(pArg1);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ccaa0, CBDisposePersonality, CBDisposePersonality_original)

FILE* C2_HOOK_FASTCALL BonerOpenRemaps(const char* pFile_name) {
    tPath_name path;

    PathCat(path, C2V(gApplication_path), C2V(gPedsFolder));
    PathCat(path, path, "REMAPS");
    PathCat(path, path, pFile_name);
    return DRfopen(path, "rb");
}
C2_HOOK_FUNCTION(0x004cbbb0, BonerOpenRemaps)

tPed_remap* (C2_HOOK_FASTCALL * ReadRemap_original)(const char *pFile_name);
tPed_remap* C2_HOOK_FASTCALL ReadRemap(const char *pFile_name) {

#if 0//defined(C2_HOOKS_ENABLED)
    return ReadRemap_original(pFile_name);
#else
    FILE* f;
    tPed_remap* remap;
    char s[256];
    int i;

    if (c2_strcmp(pFile_name, "NONE") == 0) {
        return NULL;
    }
    f = BonerOpenRemaps(pFile_name);
    if (f == NULL) {
        FatalError(kFatalError_BonerError_UnableToOpenFile_S, pFile_name);
    }

    C2_HOOK_BUG_ON(sizeof(tPed_remap) != 0x30);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_remap, count_bones, 0x28);
    C2_HOOK_BUG_ON(sizeof(tPed_remap_bone) != 0x40);

    remap = BrMemAllocate(sizeof(tPed_remap), kBoner_mem_type_remap);
    if (remap == NULL) {
        FatalError(kFatalError_BonerError_UnableToAllocateMemory);
    }
    remap->count_bones = 0;
    GetALineAndDontArgue(f, s);
    if (c2_strcmp(s, "START OF BONES") != 0) {
        FatalError(kFatalError_BonerError_SyntaxErrorInFormFileExpected_S, "START OF BONES");
    }
    while (!PFfeof(f)) {
        GetALineAndDontArgue(f, s);
        /* FIXME: should be 'c2_strcmp(s, "END OF BONES") == 0'? */
        if ((remap->count_bones == 0 && c2_strcmp(s, "END OF BONES") != 0) || c2_strcmp(s, "NEXT BONE") == 0) {
            remap->count_bones += 1;
        }
    }
    remap->bones = BrMemAllocate(sizeof(tPed_remap_bone) * remap->count_bones, kBoner_mem_type_remap);
    PFrewind(f);
    GetALineAndDontArgue(f, s);
    for (i = 0; i < remap->count_bones; i++) {
        int j;

        GetALineAndDontArgue(f, remap->bones[i].name);
        for (j = 0; j < 3; j++) {
            remap->bones[i].general_remap[j] = GetALineAndInterpretCommand(f, C2V(gPed_remap_axis_choices), REC2_ASIZE(C2V(gPed_remap_axis_choices)));
        }
        for (j = 0; j < 3; j++) {
            remap->bones[i].powerup_axis[j] = GetALineAndInterpretCommand(f, C2V(gPed_remap_axis_choices), REC2_ASIZE(C2V(gPed_remap_axis_choices)));
        }
        GetALineAndDontArgue(f, s);
    }
    c2_strcpy(remap->name, pFile_name);
    for (i = 0; ; i++) {
        if (i >= REC2_ASIZE(C2V(gPed_remaps))) {
            FatalError(kFatalError_BonerError_TooManyRemapsLoaded);
        }
        if (C2V(gPed_remaps)[i] == NULL) {
            C2V(gPed_remaps)[i] = remap;
            break;
        }
    }
    return remap;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00404410, ReadRemap, ReadRemap_original)

FILE* C2_HOOK_FASTCALL BonerOpenPersonality(const char* pName) {
    tPath_name path;

    PathCat(path, C2V(gApplication_path), C2V(gPedsFolder));
    PathCat(path, path, "PEEPS");
    PathCat(path, path, pName);
    C2V(gTwtPeds) = OpenPackFileAndSetTiffLoading(path);
    PathCat(path, path, pName);
    c2_strcat(path, ".TXT");
    return DRfopen(path, "rt");
}
C2_HOOK_FUNCTION(0x004cbaa0, BonerOpenPersonality)

FILE* C2_HOOK_FASTCALL BonerOpenCharacterForm(const char* pName) {
    tPath_name path;

    PathCat(path, C2V(gApplication_path), C2V(gPedsFolder));
    PathCat(path, path, "FORMS");
    PathCat(path, path, pName);
    c2_strcat(path, ".TXT");
    return DRfopen(path, "rt");
}
C2_HOOK_FUNCTION(0x004cb980, BonerOpenCharacterForm)

FILE* C2_HOOK_FASTCALL BonerOpenMoves(const char* pName) {
    tPath_name path;

    PathCat(path, C2V(gApplication_path), C2V(gPedsFolder));
    PathCat(path, path, "MOVES");
    PathCat(path, path, pName);
    return DRfopen(path, "rb");
}
C2_HOOK_FUNCTION(0x004cbb50, BonerOpenMoves)

int C2_HOOK_FASTCALL DRVector3NonZero(br_vector3* pV) {
    return pV->v[0] == 0.f && pV->v[1] == 0.f && pV->v[2] == 0.f;
}
C2_HOOK_FUNCTION(0x00516350, DRVector3NonZero)

void C2_HOOK_FASTCALL BonerReadPersonalityModels(const char* pName) {
    tPath_name path;

    PathCat(path, C2V(gApplication_path), C2V(gPedsFolder));
    PathCat(path, path, "PEEPS");
    PathCat(path, path, pName);
    DisallowDuplicates();
    LoadAllStuffInDirectory(&C2V(gPedStorage), path, kRendererShadingType_Diffuse1);
    AllowDuplicates();
}

void C2_HOOK_FASTCALL RemapVector(br_vector3* pV, const tPed_remap_bone* pRemap) {
    int i;
    br_vector3 original;

    if (pRemap == NULL) {
        return;
    }
    BrVector3Copy(&original, pV);
    for (i = 0; i < 3; i++) {
        float v = original.v[(int)pRemap->general_remap[i] % 3];
        if ((int)pRemap->general_remap[i] >= 3) {
            v = -v;
        }
        pV->v[i] = v;
    }
}

tPed_remap* C2_HOOK_FASTCALL FindOrOpenRemap(const char* pName) {
    int i;

    for (i = 0; i < REC2_ASIZE(C2V(gPed_remaps)); i++) {
        if (C2V(gPed_remaps)[i] != NULL && c2_strcmp(C2V(gPed_remaps)[i]->name, pName) == 0) {
            return C2V(gPed_remaps)[i];
        }
    }
    return ReadRemap(pName);
}

void C2_HOOK_FASTCALL Flip3DStoBRaxes(br_matrix34* pMat) {
    br_vector3 tv;
    float f;

    BrVector3Copy(&tv, (br_vector3*)pMat->m[1]);
    BrVector3Copy((br_vector3*)pMat->m[1], (br_vector3*)pMat->m[2]);
    BrVector3Negate((br_vector3*)pMat->m[2], &tv);

    f = pMat->m[0][1];
    pMat->m[0][1] = pMat->m[0][2];
    pMat->m[0][2] = -f;

    f = pMat->m[1][1];
    pMat->m[1][1] = pMat->m[1][2];
    pMat->m[1][2] = -f;

    f = pMat->m[2][1];
    pMat->m[2][1] = pMat->m[2][2];
    pMat->m[2][2] = -f;

    f = pMat->m[3][1];
    pMat->m[3][1] = pMat->m[3][2];
    pMat->m[3][2] = -f;
}

tPed_move* C2_HOOK_FASTCALL ReadMove(const char* pName, tPed_form* pForm, const char* pLooping_reset_flags, const br_vector3* pMovement_direction, tU32 pFlags) {
    FILE* f;
    tPed_move* move;
    int i;
    char bone_names[50][36];
    tS32 count_bones_in_anim_file;
    br_vector3 rot_pos;
    br_vector3 move_pos;

    C2_HOOK_BUG_ON(sizeof(tPed_move) != 0x50);
    C2_HOOK_BUG_ON(sizeof(tPed_move_frame) != 0x34);
    C2_HOOK_BUG_ON(sizeof(tPed_move_frame_axis) != 0x3);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_move, frames, 0x4c);

    f = BonerOpenMoves(pName);
    if (f == NULL) {
        FatalError(kFatalError_BonerError_UnableToOpenFile_S, pName);
    }
    move = BrMemAllocate(sizeof(tPed_move), kBoner_mem_type_sequence);
    if (move == NULL) {
        FatalError(kFatalError_BonerError_UnableToAllocateMemory);
    }
    count_bones_in_anim_file = ReadS32(f);
    move->count_frames = ReadS32(f);
    for (i = 0; i < count_bones_in_anim_file; i++) {
        PFfread(bone_names[i], 1, 33, f);
        PFfseek(f, sizeof(tU32) * ReadS32(f), SEEK_CUR);
    }
    BrVector3Set(&move_pos, 0.f, 0.f, 0.f);
    move->frames = BrMemAllocate(move->count_frames * sizeof(tPed_move_frame), kMem_boner_type_frames);
    for (i = 0; i < move->count_frames; i++) {
        int j;

        move->frames[i].axis = BrMemAllocate(pForm->count_bones * sizeof(tPed_move_frame_axis), kBoner_mem_type_frame_2);
        for (j = 0; j < count_bones_in_anim_file; j++) {
            br_matrix34 mat;
            int k = 0;
            const char* bone_name_anim_file = bone_names[j];

            for (k = 0; k < 4; k++) {
                int l;

                for (l = 0; l < 3; l++) {
                    mat.m[k][l] = ReadF32(f);
                }
            }
            for (k = 0; k < pForm->count_bones; k++) {
                if (c2_strcmp(pForm->bones[k].name, bone_name_anim_file) == 0) {
                    br_euler e;
                    br_matrix34 mat_lp;

                    Flip3DStoBRaxes(&mat);
                    BrMatrix34LPNormalise(&mat_lp, &mat);
                    if (pForm->bones[k].parent_index == -1) {
                        c2_memmove(&move->frames[i].mat, &mat_lp, 9 * sizeof(float));
                        if (i != 0) {
                            BrVector3Sub((br_vector3*)move->frames[i].mat.m[3], (br_vector3*)mat_lp.m[3], &rot_pos);
                            BrVector3Accumulate(&move_pos, (br_vector3*)move->frames[i].mat.m[3]);
                        }
                        BrVector3Copy(&rot_pos, (br_vector3*)mat_lp.m[3]);
                    }
                    e.order = 0;
                    BrMatrix34ToEuler(&e, &mat_lp);
                    move->frames[i].axis[k].euler1 = e.a / 256;
                    move->frames[i].axis[k].euler2 = e.b / 256;
                    move->frames[i].axis[k].euler3 = e.c / 256;
                    break;
                }
            }
        }
    }
    if (c2_strchr(pLooping_reset_flags, 'X') != NULL) {
        move->frames[0].mat.m[3][0] = -move_pos.v[0];
        move_pos.v[0] = 0.f;
        move->looping_reset_flags |= 0x1;
    } else {
        if (move->count_frames > 1) {
            move->frames[0].mat.m[3][0] = move->frames[1].mat.m[3][0];
            move_pos.v[0] += move->frames[0].mat.m[3][0];
        } else {
            move->frames[0].mat.m[3][0] = 0.f;
        }
    }
    if (c2_strchr(pLooping_reset_flags, 'Y') != NULL) {
        move->frames[0].mat.m[3][1] = -move_pos.v[1];
        move_pos.v[1] = 0.f;
        move->looping_reset_flags |= 0x2;
    } else {
        if (move->count_frames > 1) {
            move->frames[0].mat.m[3][1] = move->frames[1].mat.m[3][1];
            move_pos.v[1] += move->frames[0].mat.m[3][1];
        } else {
            move->frames[0].mat.m[3][1] = 0.f;
        }
    }
    if (c2_strchr(pLooping_reset_flags, 'Z') != NULL) {
        move->frames[0].mat.m[3][2] = -move_pos.v[2];
        move_pos.v[2] = 0.f;
        move->looping_reset_flags |= 0x4;
    } else {
        if (move->count_frames > 1) {
            move->frames[0].mat.m[3][2] = move->frames[1].mat.m[3][2];
            move_pos.v[2] += move->frames[0].mat.m[3][2];
        } else {
            move->frames[0].mat.m[3][2] = 0.f;
        }
    }

    if (BrVector3LengthSquared(&move_pos) != 0.f && !(move->looping_reset_flags & 0x1)) {
        move->field_0x2a = 0;
        BrVector3Normalise(&move->field_0x34, &move_pos);
        if (BrVector3LengthSquared(pMovement_direction) != 0) {
            BrVector3Copy(&move->field_0x40, pMovement_direction);
        } else {
            BrVector3Copy(&move->field_0x40, &move->field_0x34);
        }
    } else {
        move->field_0x2a = 1;
        if (BrVector3LengthSquared(pMovement_direction) != 0) {
            BrVector3Copy(&move->field_0x40, pMovement_direction);
        } else {
            BrVector3Set(&move->field_0x40, 0.f, 0.f, 1.f);
        }
        BrVector3Set(&move->field_0x34, 0.f, 0.f, 0.f);
    }
    move->move_flags = pFlags;
    PFfclose(f);
    c2_strcpy(move->name, pName);

    for (i = 0; ; i++) {
        if (i >= REC2_ASIZE(C2V(gPed_moves))) {
            FatalError(kFatalError_BonerError_TooManyMovesLoaded);
        }
        if (C2V(gPed_moves)[i] == NULL) {
            C2V(gPed_moves)[i] = move;
            break;
        }
    }
    return move;
}

tPed_move* C2_HOOK_FASTCALL FindOrOpenMove(const char* pName, tPed_form* pForm, const char* pLooping_reset_flags, const br_vector3* pMovement_direction, tU32 flags) {
    int i;

    for (i = 0; i < REC2_ASIZE(C2V(gPed_moves)); i++) {
        if (C2V(gPed_moves)[i] != NULL) {
            if (c2_strcmp(C2V(gPed_moves)[i]->name, pName) == 0) {
                return C2V(gPed_moves)[i];
            }
        }
    }
    return ReadMove(pName, pForm, pLooping_reset_flags, pMovement_direction, flags);
}

tPed_form* (C2_HOOK_FASTCALL * SetUpCharacterForm_original)(const char* pName);
tPed_form* C2_HOOK_FASTCALL SetUpCharacterForm(const char* pName) {
#if 0//defined(C2_HOOKS_ENABLED)
    return SetUpCharacterForm_original(pName);
#else
    FILE* f;
    tPed_form* form;
    int max_simple_physicing_at_once;
    int max_boned_physicing_at_once;
    int max_rendering_at_once;
    tPed_form_move_buffer_item form_moves[100];
    char s[256];
    int i;
    float total_mass;

    f = BonerOpenCharacterForm(pName);
    if (f == NULL) {
        FatalError(kFatalError_BonerError_UnableToOpenFile_S, pName);
    }

    C2_HOOK_BUG_ON(sizeof(tPed_form) != 0x50);

    form = BrMemAllocate(sizeof(tPed_form), kBoner_mem_type_form);
    if (form == NULL) {
        FatalError(kFatalError_BonerError_UnableToAllocateMemory);
    }

    /* Max simple physicing at once */
    max_simple_physicing_at_once = GetAnInt(f);

    /* Max boned physicing at once */
    max_boned_physicing_at_once = GetAnInt(f);

    /* Max rendering at once */
    max_rendering_at_once = GetAnInt(f);

    /* Max stored dismembered characters */
    form->max_stored_dismembered_characters = GetAnInt(f);

    GetALineAndDontArgue(f, s);
    form->remap = FindOrOpenRemap(s);

    while (!PFfeof(f)) {
        GetALineAndDontArgue(f, s);
        if (c2_strcmp(s, "NEXT BONE") == 0) {
            form->count_bones += 1;
        } else if (c2_strcmp(s, "END OF BONES") == 0) {
            form->count_bones += 1;
            break;
        }
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_form, max_simple_physicing_at_once, 0x2b);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_form, max_boned_physicing_at_once, 0x2c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_form, max_rendering_at_once, 0x2a);
    C2_HOOK_BUG_ON(sizeof(tPed_form_actor_set) != 0x8);
    C2_HOOK_BUG_ON(sizeof(tPed_form_simple_phys) != 0x8);
    C2_HOOK_BUG_ON(sizeof(tPed_form_boned_phys) != 0x8);
    C2_HOOK_BUG_ON(sizeof(tPed_form_bone) != 0x34);

    form->max_simple_physicing_at_once = max_simple_physicing_at_once;
    form->max_boned_physicing_at_once = max_boned_physicing_at_once;
    form->max_rendering_at_once = max_rendering_at_once;
    form->actor_sets = BrMemAllocate(max_rendering_at_once * sizeof(tPed_form_actor_set), kBoner_mem_type_actor_sets);
    form->simple_physicing = BrMemAllocate(max_simple_physicing_at_once * sizeof(tPed_form_simple_phys), kBoner_mem_type_simple_phys);
    form->boned_physicing = BrMemAllocate(max_boned_physicing_at_once * sizeof(tPed_form_boned_phys), kBoner_mem_type_boned_phys);
    form->bones = BrMemAllocate(form->count_bones * sizeof(tPed_form_bone), kBoner_mem_type_bone_info);
    PFrewind(f);
    for (i = 0; i < 5; i++) {
        GetALineAndDontArgue(f, s);
    }
    GetALineAndDontArgue(f, s);
    if (c2_strcmp(s, "START OF BONES") != 0) {
        FatalError(kFatalError_BonerError_SyntaxErrorInFormFileExpected_S, "START OF BONES");
    }
    for (i = 0; i < max_boned_physicing_at_once; i++) {
        form->boned_physicing[i].collision_infos = BrMemAllocate(form->count_bones * sizeof(tCollision_info*), kBoner_mem_type_boned_coll);
    }
    total_mass = 0.f;
    for (i = 0; i < form->count_bones; i++) {
        float mass;
        tPed_form_bone* bone;
        int j;

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_form_bone, name, 0x0);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_form_bone, remapped_bone, 0x2c);

        bone = &form->bones[i];

        GetALineAndDontArgue(f, bone->name);

        bone->remapped_bone = NULL;
        if (form->remap != NULL) {
            int j;

            for (j = 0; j < form->remap->count_bones; j++) {
                if (c2_strcmp(bone->name, form->remap->bones[j].name) == 0) {
                    bone->remapped_bone = &form->remap->bones[j];
                    break;
                }
            }
        }

        /* Mass in kg */
        mass = GetAScalar(f) / 1000.f;
        total_mass += mass;

        if (mass == 0.f) {

            for (j = 0; j < max_boned_physicing_at_once; j++) {
                form->boned_physicing[j].field_0x0 = 0;
                form->boned_physicing[j].collision_infos[i] = NULL;
            }
        } else {
            tPed_form_collision_detection_type collision_detection_type;
            tPed_form_bone_hinge_type joint_type;
            tPhysics_joint* joint;

            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_form_bone, parent_index, 0x28);

            /* Collision detection type */
            collision_detection_type = GetALineAndInterpretCommand(f, C2V(gPed_form_collision_type_names), REC2_ASIZE(C2V(gPed_form_collision_type_names)));
            joint_type = GetALineAndInterpretCommand(f, C2V(gPed_form_bone_joint_type_names), REC2_ASIZE(C2V(gPed_form_bone_joint_type_names)));
            joint = NULL;
            if (joint_type < 0 || joint_type == ePed_form_bone_hinge_none) {
                bone->parent_index = -1;
            } else {
                /* Index of parent */
                bone->parent_index = GetAnInt(f);
                if (joint_type != ePed_form_bone_hinge_phantom && joint_type != ePed_form_bone_hinge_false) {
                    int count_limits;
                    int is_rotate;
                    int j;

                    /* Number of limits */
                    count_limits = GetAnInt(f);
                    joint = AllocatePhysicsJoint(count_limits, kMem_physics_joint);
                    joint->type = joint_type;
                    joint->count_limits = count_limits;
                    is_rotate = 0;
                    for (j = 0; j < count_limits; j++) {
                        tPhysics_joint_limit* limit = &joint->limits[j];

                        if (is_rotate) {
                            br_scalar angle;
                            br_matrix34 mat;

                            is_rotate = 0;
                            /* Angle to rotate cross-product */
                            angle = GetAScalar(f);
                            DRVector3SafeCross(&limit->child, &joint->limits[j - 1].child, &joint->limits[j - 1].parent);
                            BrMatrix34Rotate(&mat, BR_ANGLE_DEG(angle), &joint->limits[j - 1].parent);
                            BrMatrix34ApplyV(&limit->parent, &joint->limits[j].child, &mat);
                        } else {
                            /* Limit type */
                            limit->type = GetALineAndInterpretCommand(f,
                                C2V(gPed_form_joint_limit_type_names),
                                REC2_ASIZE(C2V(gPed_form_joint_limit_type_names)));
                            /* Child vector */
                            GetAVector(f, &limit->child);
                            /* Parent vector */
                            GetAVector(f, &limit->parent);

                            RemapVector(&limit->child, bone->remapped_bone);
                            RemapVector(&limit->parent, form->bones[bone->parent_index].remapped_bone);

                            is_rotate = limit->type == eJoint_limit_universal;
                            /* Maximum angle allowed between above two vectors */
                            limit->value = FastFloatCos((int) GetAScalar);
                        }
                        BrVector3Normalise(&limit->child, &limit->child);
                        BrVector3Normalise(&limit->parent, &limit->parent);
                    }
                    if (joint->type == eJoint_hinge || joint->type == eJoint_quick_hinge) {
                        /* Hinge axis */
                        GetAVector(f, &joint->hinge_axis);
                        BrVector3Copy(&joint->parent_bone_axis, &joint->hinge_axis);
                        RemapVector(&joint->hinge_axis, bone->remapped_bone);
                        RemapVector(&joint->parent_bone_axis, form->bones[bone->parent_index].remapped_bone);
                        if (fabsf(joint->hinge_axis.v[0]) > fabsf(joint->hinge_axis.v[1]) && fabsf(joint->hinge_axis.v[0]) > fabsf(joint->hinge_axis.v[2])) {
                            BrVector3Cross(&joint->hinge_axis2, &joint->hinge_axis, &C2V(g_Ped_y_unit_vector));
                            BrVector3Cross(&joint->hinge_axis3, &joint->hinge_axis, &C2V(g_Ped_z_unit_vector));
                        } else if (fabsf(joint->hinge_axis.v[1]) > fabsf(joint->hinge_axis.v[0]) && fabsf(joint->hinge_axis.v[1]) > fabsf(joint->hinge_axis.v[2])) {
                            BrVector3Cross(&joint->hinge_axis2, &joint->hinge_axis, &C2V(g_Ped_x_unit_vector));
                            BrVector3Cross(&joint->hinge_axis3, &joint->hinge_axis, &C2V(g_Ped_z_unit_vector));
                        } else {
                            BrVector3Cross(&joint->hinge_axis2, &joint->hinge_axis, &C2V(g_Ped_x_unit_vector));
                            BrVector3Cross(&joint->hinge_axis3, &joint->hinge_axis, &C2V(g_Ped_y_unit_vector));
                        }
                    }
                }
            }
            bone->hinge = joint;
            for (j = 0; j < max_boned_physicing_at_once; j++) {
                tCollision_info* coll_info;

                C2_HOOK_BUG_ON(sizeof(tCollision_info) != 0x4d8);

                coll_info = BrMemAllocate(sizeof(tCollision_info), kBoner_mem_type_boned_coll_obj);
                form->boned_physicing[j].collision_infos[i] = coll_info;
                coll_info->uid = i;
                coll_info->M = mass;
                switch (collision_detection_type) {
                case ePed_form_collision_type_box:
                    coll_info->shape = (tCollision_shape*)AllocateBoxCollisionShape(kMem_collision_shape);
                    break;
                case ePed_form_collision_type_polyhedron:
                case ePed_form_collision_type_polygon:
                case ePed_form_collision_type_none:
                    coll_info->shape = NULL;
                case ePed_form_collision_type_line:
                    coll_info->shape = (tCollision_shape*)AllocateWireFrameCollisionShape(2, 1, kMem_collision_shape);
                    coll_info->shape->wireframe.wireframe.lines[0].index1 = 0;
                    coll_info->shape->wireframe.wireframe.lines[0].index2 = 1;
                    break;
                case ePed_form_collision_type_sphere:
                    coll_info->shape = (tCollision_shape*)AllocateShapeSphere(kMem_collision_shape);
                    break;
                }
                form->boned_physicing[j].field_0x0 = 0;
            }
        }
        GetALineAndDontArgue(f, s);
        if (c2_strcmp(s, "NEXT BONE") != 0 && c2_strcmp(s, "END OF BONES") != 0) {
            FatalError(kFatalError_BonerError_SyntaxErrorInFormFileExpected_S, "NEXT BONE");
        }
    }
    for (i = 0; i < max_simple_physicing_at_once; i++) {
        tCollision_info* coll_info;
        coll_info = BrMemAllocate(sizeof(tCollision_info), kBoner_mem_type_simple_coll_obj);
        form->simple_physicing[i].collision_info = coll_info;
        coll_info->M = total_mass;
        coll_info->shape = (tCollision_shape*)AllocateBoxCollisionShape(kMem_collision_shape);
        form->simple_physicing[i].type = kCollisionShapeType_Box;
    }

    for (i = 0; i < max_rendering_at_once; i++) {
        int j;

        form->actor_sets[i].actors = BrMemAllocate(sizeof(br_actor*) * form->count_bones, kBoner_mem_type_actor_ptrs);
        for (j = 0; j < form->count_bones; j++) {
            form->actor_sets[i].actors[j] = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            form->actor_sets[i].actors[j]->render_style = BR_RSTYLE_FACES;
        }
    }
    GetALineAndDontArgue(f, s);
    if (c2_strcmp(s, "START OF MOVES") != 0) {
        FatalError(kFatalError_BonerError_UnableToOpenFile_S, "START OF MOVES");
    }
    form->count_moves = 0;
    for (i = 0;; i++) {
        const char* str;

        GetALineAndDontArgue(f, s);
        if (c2_strcmp(s, "END OF MOVES") == 0) {
            break;
        } else if (PFfeof(f)) {
            FatalError(kFatalError_BonerError_SyntaxErrorInFormFileExpected_S, "END OF MOVES");
        }
        /* ID */
        str = c2_strtok(s, "\t ,/");
        c2_sscanf(str, "%d", &form_moves[i].id);

        /* Default frame rate */
        form_moves[i].frametime = 1000 / GetAnInt(f);

        /* Looping reset flags */
        GetAString(f, form_moves[i].looping_reset_flags);

        /* Direction of movement */
        GetAVector(f, &form_moves[i].movement_direction);
        if (DRVector3NonZero(&form_moves[i].movement_direction)) {
            BrVector3Normalise(&form_moves[i].movement_direction, &form_moves[i].movement_direction);
        }

        /* Biped export file name */
        GetAString(f, form_moves[i].biped_export_filename);

        /* Flags */
        form_moves[i].flags = GetAnInt(f);

        form->count_moves += 1;
    }

    if (C2V(gPed_vtable)->load_form != NULL) {
        C2V(gPed_vtable)->load_form(form, f);
    }
    PFfclose(f);

    C2_HOOK_BUG_ON(sizeof(tPed_form_move) != 8);

    form->moves = BrMemAllocate(form->count_moves * sizeof(tPed_form_move), kBoner_mem_type_moves);
    for (i = 0; i < form->count_moves; i++) {
        form->moves[i].id = form_moves[i].id;
        form->moves[i].move = FindOrOpenMove(form_moves[i].biped_export_filename,
                form,
                form_moves[i].looping_reset_flags,
                &form_moves[i].movement_direction,
                form_moves[i].flags);
        form->moves[i].move->frametime = form_moves[i].frametime;
    }

    C2_HOOK_BUG_ON(sizeof(tPed_form_dismembered_character) != 0x30);

    form->stored_dismembered_characters = BrMemAllocate(form->max_stored_dismembered_characters * sizeof(tPed_form_dismembered_character*), kBoner_mem_type_stored_ptrs);
    for (i = 0; i < form->max_stored_dismembered_characters; i++) {
        form->stored_dismembered_characters[i] = BrMemAllocate(form->count_bones * sizeof(tPed_form_dismembered_character), kBoner_mem_type_stored);
    }

    c2_strcpy(form->name, pName);
    for (i = 0; ; i++) {
        if (i >= REC2_ASIZE(C2V(gPed_forms))) {
            FatalError(kFatalError_BonerError_TooManyFormsLoaded);
        }
        if (C2V(gPed_forms)[i] == NULL) {
            C2V(gPed_forms)[i] = form;
            break;
        }
    }
    return form;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00404f60, SetUpCharacterForm, SetUpCharacterForm_original)

FILE* C2_HOOK_FASTCALL BonerOpenDefaultMoves(const char* pName) {
    tPath_name path;

    PathCat(path, C2V(gApplication_path), C2V(gPedsFolder));
    PathCat(path, path, "MOVES");
    PathCat(path, path, pName);
    c2_strcat(path, ".TXT");
    return DRfopen(path, "rt");
}
C2_HOOK_FUNCTION(0x004cba10, BonerOpenDefaultMoves)

tPed_form* C2_HOOK_FASTCALL FindOrOpenForm(const char* pName) {
    int i;

    for (i = 0; i < REC2_ASIZE(C2V(gPed_forms)); i++) {
        if (C2V(gPed_forms)[i] != NULL) {
            if (c2_strcmp(C2V(gPed_forms)[i]->name, pName) == 0) {
                return C2V(gPed_forms)[i];
            }
        }
    }
    return SetUpCharacterForm(pName);
}

void C2_HOOK_FASTCALL RemapModelAxis(br_model* pModel, const tPed_remap_bone* pRemap) {
    if (pRemap != NULL) {
        int i;

        for (i = 0; i < pModel->nvertices; i++) {
            RemapVector(&pModel->vertices[i].p, pRemap);
        }
    }
    BrModelUpdate(pModel, BR_MODU_ALL);
}

br_model* C2_HOOK_FASTCALL BonerCloneModel(br_model *pModel, int pIndex, int pUpdateable) {
    char identifier[40];
    char* str;
    br_model* clone;
    br_material* original_material;
    br_material* cloned_material;
    int i;

    c2_strcpy(identifier, pModel->identifier);
    str = c2_strtok(identifier, ".");
    c2_sprintf(str + c2_strlen(str), "%d.DAT", pIndex);
    clone = BrModelAllocate(str, pModel->nvertices ,pModel->nfaces);
    c2_memmove(clone->vertices, pModel->vertices, pModel->nvertices * sizeof(br_vertex));
    c2_memmove(clone->faces, pModel->faces, pModel->nfaces * sizeof(br_face));
    c2_strcpy(identifier, pModel->faces->material->identifier);
    str = c2_strtok(identifier, ".");
    c2_sprintf(str + c2_strlen(str), "%d.MAT", pIndex);
    original_material = pModel->faces[0].material;
    cloned_material = BrMaterialFind(str);
    if (cloned_material == NULL && C2V(gCount_pedestrian_personality_cloned_materials) < REC2_ASIZE(C2V(gPedestrian_character_cloned_materials))) {
        cloned_material = DRMaterialClone(original_material, 0);
        cloned_material->identifier = BrResStrDup(cloned_material, identifier);
        BrMaterialUpdate(cloned_material, BR_MATU_ALL);
        C2V(gPedestrian_character_cloned_materials)[C2V(gCount_pedestrian_personality_cloned_materials)] = cloned_material;
        C2V(gCount_pedestrian_personality_cloned_materials) += 1;
    }
    for (i = 0; i < pModel->nfaces; i++) {
        if (pModel->faces[i].material == original_material) {
            pModel->faces[i].material = cloned_material;
        }
    }
    if (pUpdateable) {
        clone->flags |= BR_MODF_UPDATEABLE;
    } else {
        clone->flags &= ~BR_MODF_UPDATEABLE;
    }
    BrModelAdd(clone);
    return clone;
}

float C2_HOOK_FASTCALL CalcBoundsRadius(const br_bounds3* pBounds) {
    float v;
    float r;

    r = 0.f;

    v = fabsf(pBounds->min.v[0]);
    if (v > r) {
        r = v;
    }
    v = fabsf(pBounds->min.v[1]);
    if (v > r) {
        r = v;
    }
    v = fabsf(pBounds->min.v[2]);
    if (v > r) {
        r = v;
    }
    v = fabsf(pBounds->max.v[0]);
    if (v > r) {
        r = v;
    }
    v = fabsf(pBounds->max.v[1]);
    if (v > r) {
        r = v;
    }
    v = fabsf(pBounds->max.v[2]);
    if (v > r) {
        r = v;
    }
    return r;
}

tPed_personality* (C2_HOOK_FASTCALL * ReadPersonality_original)(const char* pName);
tPed_personality* C2_HOOK_FASTCALL ReadPersonality(const char* pName) {
#if 0//defined(C2_HOOKS_ENABLED)
    return ReadPersonality_original(pName);
#else
    FILE* f;
    FILE* g;
    FILE* move_f;
    tPed_personality* personality;
    char default_moves_file[256];
    char s[256];
    int i;

    C2_HOOK_BUG_ON(sizeof(tPed_personality) != 0x68);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_personality, form, 0x28);

    f = BonerOpenPersonality(pName);
    if (f == NULL) {
        FatalError(kFatalError_BonerError_UnableToOpenFile_S, pName);
    }
    personality = BrMemAllocate(sizeof(tPed_personality), kBoner_mem_type_person);
    if (personality == NULL) {
        FatalError(kFatalError_BonerError_UnableToAllocateMemory);
    }

    /* Form file to use */
    GetALineAndDontArgue(f, s);
    personality->form = FindOrOpenForm(s);
    if (personality->form == NULL) {
        FatalError(kFatalError_BonerError_UnableToLinkForm_to_personality_S, s);
    }

    /* Default moves file to use */
    GetALineAndDontArgue(f, default_moves_file);
    BonerReadPersonalityModels(pName);

    GetALineAndDontArgue(f, s);
    if (c2_strcmp(s, "START OF BONES") != 0) {
        FatalError(kFatalError_BonerError_SyntaxErrorInFormFileExpected_S, "START OF BONES");
    }

    C2_HOOK_BUG_ON(sizeof(tPed_personality_bone) != 0x3c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_personality, bones, 0x2c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_personality_bone, models, 0x0);

    personality->bones = BrMemAllocate(personality->form->count_bones * sizeof(tPed_personality_bone), kBoner_mem_type_person_parts);
    for (i = 0; i < personality->form->count_bones; i++) {
        int bone_index;
        char *str;
        char s2[256];
        int model_i;
        int j;

        GetALineAndDontArgue(f, s);
        for (bone_index = 0; bone_index < personality->form->count_bones; bone_index++) {
            if (c2_strcmp(personality->form->bones[bone_index].name, s) == 0) {
                break;
            }
        }
        GetALineAndDontArgue(f, s2);
        if (DRStricmp(s2, "NULL") != 0 && bone_index >= personality->form->count_bones) {
            FatalError(kFatalError_BonerError_UnableToFindBone_S, s);
        }
        str = s2;
        model_i = 0;
        for (;;) {
            char* comma_str = c2_strchr(str, ',');
            char* next_str;
            if (comma_str != NULL) {
                *comma_str = '\0';
                next_str = comma_str + 1;
            } else {
                next_str = NULL;
            }
            if (DRStricmp(str, "NULL") != 0) {
                br_model* model = BrModelFind(str);
                personality->bones[bone_index].models[model_i].models[0] = model;
                if (model == NULL) {
                    FatalError(kFatalError_BonerError_UnableToFindModel_S, str);
                }
                RemapModelAxis(model, personality->form->bones[bone_index].remapped_bone);
            }
            str = next_str;
            if (str == NULL) {
                break;
            }
            model_i += 1;
        }
        for (j = 0; j < 4; j++) {
            if (personality->bones[bone_index].models[j].models[0] == NULL) {
                break;
            }
            personality->bones[bone_index].models[j].models[1] = BonerCloneModel(personality->bones[bone_index].models[j].models[0], 1, 1);
            personality->bones[bone_index].models[j].models[0]->flags |= BR_MODF_UPDATEABLE;
            BrModelUpdate(personality->bones[bone_index].models[j].models[0], BR_MODU_ALL);
        }

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_personality_bone, field_0x20, 0x20);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_personality_bone, field_0x2c, 0x2c);

        if (personality->form->bones[bone_index].parent_index >= 0) {
            GetAVector(f, &personality->bones[bone_index].field_0x20);
            RemapVector(&personality->bones[bone_index].field_0x20, personality->form->bones[bone_index].remapped_bone);
            GetAVector(f, &personality->bones[bone_index].field_0x2c);
            RemapVector(&personality->bones[bone_index].field_0x2c, personality->form->bones[personality->form->bones[bone_index].parent_index].remapped_bone);
            if (personality->form->bones[bone_index].hinge != NULL) {
                tPhysics_joint* hinge_copy = ClonePhysicsJoint(personality->form->bones[bone_index].hinge, kMem_physics_joint);
                BrVector3Copy(&hinge_copy->field_0x08, &personality->bones[bone_index].field_0x20);
                BrVector3Copy(&hinge_copy->field_0x14, &personality->bones[bone_index].field_0x2c);
                personality->bones[bone_index].hinge = hinge_copy;
            }
        }
    }
    personality->M = personality->form->simple_physicing[0].collision_info->M;
    GetALineAndDontArgue(f, s);
    if (c2_strcmp(s, "END OF BONES") != 0) {
        FatalError(kFatalError_BonerError_SyntaxErrorInFormFileExpected_S, "END OF BONES");
    }
    GetALineAndDontArgue(f, s);
    if (c2_strcmp(s, "START OF MOVES") != 0) {
        FatalError(kFatalError_BonerError_SyntaxErrorInFormFileExpected_S, "START OF MOVES");
    }

    C2_HOOK_BUG_ON(sizeof(tPed_personality_move) != 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_personality, moves, 0x30);

    personality->moves = BrMemAllocate(personality->form->count_moves * sizeof(tPed_personality_move), kBoner_mem_type_person_moves);
    for (i = 0; i < personality->form->count_moves; i++) {
        personality->moves[i].scale_factor = 1.f;
        personality->moves[i].grounding_mode = ePed_personality_floating;
    }
    g = BonerOpenDefaultMoves(default_moves_file);
    if (g == NULL) {
        FatalError(kFatalError_BonerError_UnableToOpenFile_S, default_moves_file);
    }
    GetALineAndDontArgue(g, s);
    if (c2_strcmp(s, "START OF MOVES") != 0) {
        FatalError(kFatalError_BonerError_SyntaxErrorInFormFileExpected_S, "START OF BONES");
    }
    move_f = g;
    for (;;) {
        int move_id;
        char* str;

        /* ID or "END OF MOVES" */
        GetALineAndDontArgue(move_f, s);
        if (c2_strcmp(s, "END OF MOVES") == 0 || PFfeof(move_f)) {
            if (move_f == g) {
                PFfclose(g);
                move_f = f;
                continue;
            } else {
                move_f = NULL;
                break;
            }
        }
        str = c2_strtok(s, "\t ,/");
        c2_sscanf(str, "%d", &move_id);
        for (i = 0; i < personality->form->count_moves; i++) {
            if (move_id == personality->form->moves[i].id) {
                /* Scaling factor */
                personality->moves[i].scale_factor = GetAScalar(move_f);
                /* Grounding mode */
                personality->moves[i].grounding_mode = GetALineAndInterpretCommand(move_f, C2V(gPed_personality_grounding_type_names), REC2_ASIZE(C2V(gPed_personality_grounding_type_names)));
                /* Grounding offset */
                personality->moves[i].grounding_offset = GetAScalar(move_f);
                break;
            }
        }
        if (i == personality->form->count_moves) {
            c2_sprintf(s, "%s/%d", personality->name, move_id);
            FatalError(kFatalError_BonerError_IllegalMoveID_S, s);
        }
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_personality, bb, 0x34);

    /* Bounding box */
    GetAVector(f, &personality->bb.min);
    GetAVector(f, &personality->bb.max);
    RemapVector(&personality->bb.min, personality->form->bones[0].remapped_bone);
    RemapVector(&personality->bb.max, personality->form->bones[0].remapped_bone);
    if (personality->bb.max.v[0] < personality->bb.min.v[0]) {
        float tv;
        tv = personality->bb.max.v[0];
        personality->bb.max.v[0] = personality->bb.min.v[0];
        personality->bb.min.v[0] = tv;
    }
    if (personality->bb.max.v[1] < personality->bb.min.v[1]) {
        float tv;
        tv = personality->bb.max.v[1];
        personality->bb.max.v[1] = personality->bb.min.v[1];
        personality->bb.min.v[1] = tv;
    }
    if (personality->bb.max.v[2] < personality->bb.min.v[2]) {
        float tv;
        tv = personality->bb.max.v[2];
        personality->bb.max.v[2] = personality->bb.min.v[2];
        personality->bb.min.v[2] = tv;
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_personality, centre_of_mass, 0x54);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_personality, radius, 0x60);

    /* Centre of mass */
    GetAVector(f, &personality->centre_of_mass);
    RemapVector(&personality->centre_of_mass, personality->form->bones[0].remapped_bone);
    personality->radius = CalcBoundsRadius(&personality->bb);

    if (C2V(gPed_vtable)->load_personality != NULL) {
        C2V(gPed_vtable)->load_personality(personality, f);
    }

    PFfclose(f);
    c2_strcpy(personality->name, pName);
    for (i = 0; ; i++) {
        if (i >= REC2_ASIZE(C2V(gPed_personalities))) {
            FatalError(eFatalError_BonerError_TooManyPersonalitiesLoaded);
        }
        if (C2V(gPed_personalities)[i] == NULL) {
            C2V(gPed_personalities)[i] = personality;
            break;
        }
    }
    return personality;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00406ab0, ReadPersonality, ReadPersonality_original)
