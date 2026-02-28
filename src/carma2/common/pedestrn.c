#include "pedestrn.h"

#include "animation.h"
#include "car.h"
#include "errors.h"
#include "explosions.h"
#include "globvars.h"
#include "globvrpb.h"
#include "graphics.h"
#include "loading.h"
#include "physics.h"
#include "piping.h"
#include "platform.h"
#include "raycast.h"
#include "replay.h"
#include "skidmark.h"
#include "smashing.h"
#include "sound.h"
#include "spark.h"
#include "trig.h"
#include "utility.h"
#include "world.h"

#include "brender/brender.h"

#include "c2_stdlib.h"
#include "c2_string.h"

#include "rec2_macros.h"
#include "rec2_types.h"

#define GET_PED_COLLISION_OBJECT(PED) ( ((PED)->character->field_0x14 & 1) ? (PED)->character->personality->form->simple_physicing[(PED)->character->field_0x5].collision_info : GetRootObject((PED)->character))

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
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gProx_ray_shade_table, 0x00694458);

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
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_vector3, gChanged_points, 10, 0x0069bc68);
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
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const int, gPow2_array, 32, 0x0058f2d8, {
           0x1,           0x2,           0x4,           0x8,
          0x10,          0x20,          0x40,          0x80,
         0x100,         0x200,         0x400,         0x800,
        0x1000,        0x2000,        0x4000,        0x8000,
       0x10000,       0x20000,       0x40000,       0x80000,
      0x100000,      0x200000,      0x400000,      0x800000,
     0x1000000,     0x2000000,     0x4000000,     0x8000000,
    0x10000000,    0x20000000,    0x40000000,    0x80000000
});
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_active_peds, 0x007447ec);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPeds_already_munged, 0x0069bce0);
C2_HOOK_VARIABLE_IMPLEMENT(tPedestrian*, gPedestrians_in_sight, 0x007447b4);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gPed_last_munging, 0x00694130);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gLast_scare_time, 0x006a042c);
C2_HOOK_VARIABLE_IMPLEMENT(tPed_cache_006944c0, gPed_cache_00694328, 0x00694328);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const br_vector3, gPed_bone_look_vecs, 9, 0x0065e4f4, {
    { {  0.f,  1.f,  0.f } },
    { {  0.f, -1.f,  0.f } },
    { {  0.f, -1.f,  0.f } },
    { {  0.f, -1.f,  0.f } },
    { {  0.f, -1.f,  0.f } },
    { {  1.f,  0.f,  0.f } },
    { {  1.f,  0.f,  0.f } },
    { { -1.f,  0.f,  0.f } },
    { { -1.f,  0.f,  0.f } },
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(undefined, gPed_scare_head_anim_byte_array, 8, 0x0065e4d8, {
    4, 1, 2, 1, 99, 0, 0, 0,
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const float, gFlamed_ped_flame_scales, 7, 0x0065e5b8, {
    .275f,
    .2245f,
    .2f,
    .175f,
    .15f,
    .075f,
    .05f,
});
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gLimbs_actor, 0x006a0424);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPed_count_limbed_actors, 0x006a0420);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_actor*, gPed_limbed_actors, 30, 0x00694280);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tProximity_ray, gProximity_rays, 20, 0x00694140);

#define PED_SCALAR_EPSILON (2.384186e-6f)

int C2_HOOK_FASTCALL GetPedCount(void) {
    return C2V(gPed_count);
}
C2_HOOK_FUNCTION(0x004d2c40, GetPedCount)

int C2_HOOK_FASTCALL GetPedPosition(int pIndex, br_vector3* pPos) {

    if (C2V(gPedestrian_array)[pIndex].hit_points <= 0) {
        return 0;
    }
    if (!(C2V(gPedestrian_array)[pIndex].flags & 0x1) && C2V(gMap_view) != 2) {
        return 0;
    }
    if (C2V(gCurrent_race).race_spec->race_type == kRaceType_Peds && !(C2V(gPedestrian_array)[pIndex].flags & 0x200)) {
        return 0;
    }
    BrVector3Copy(pPos, &C2V(gPedestrian_array)[pIndex].pos);
    return 1;
}
C2_HOOK_FUNCTION(0x004d2c50, GetPedPosition)

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
    C2V(gProx_ray_shade_table) = GenerateShadeTable(8, C2V(gRender_palette), 0xd7, 0xff, 0xe9, .5f, .75f, .9f);
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
        C2V(gNapalm_bolts)[i].ped = NULL;
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

int (C2_HOOK_FASTCALL * MorphCharacterBonePositions_original)(tPed_character_instance* pPed, tU32 pArg2);
int C2_HOOK_FASTCALL MorphCharacterBonePositions(tPed_character_instance* pPed, tU32 pArg2) {

#if defined(C2_HOOKS_ENABLED)
    return MorphCharacterBonePositions_original(pPed, pArg2);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004097b0, MorphCharacterBonePositions, MorphCharacterBonePositions_original)

void (C2_HOOK_FASTCALL * SetBoner_original)(tPed_character_instance* pPed, br_matrix34* pMat, br_matrix34* pParent_mat, br_matrix34* pArg4, tU8 pAngle_x, tU8 pAngle_y, tU8 pAngle_z, br_vector3* pArg8, br_vector3* pParg9);
void C2_HOOK_FASTCALL SetBoner(tPed_character_instance* pPed, br_matrix34* pMat, br_matrix34* pParent_mat, br_matrix34* pArg4, tU8 pAngle_x, tU8 pAngle_y, tU8 pAngle_z, br_vector3* pArg8, br_vector3* pParg9) {

#if defined(C2_HOOKS_ENABLED)
    SetBoner_original(pPed, pMat, pParent_mat, pArg4, pAngle_x, pAngle_y, pAngle_z, pArg8, pParg9);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00407e70, SetBoner, SetBoner_original)

void (C2_HOOK_FASTCALL * SetCharacterBonePositions_original)(tPed_character_instance* pPed, undefined4 pArg2, undefined4 pArg3);
void C2_HOOK_FASTCALL SetCharacterBonePositions(tPed_character_instance* pPed, undefined4 pArg2, undefined4 pArg3) {

#if 0//defined(C2_HOOKS_ENABLED)
    SetCharacterBonePositions_original(pPed, pArg2, pArg3);
#else
    tPed_personality *personality;
    tPed_form* form;
    tPed_form_actor_set* actor_set;
    tPed_move_frame* ped_move_frame;
    int i;

    if (pPed->field_0x7 < 0) {
        return;
    }
    if ((pPed->field_0x14 & 0x4) && (pPed->field_0x14 & 0x2) && pPed->field_0xe8 == NULL) {
        return;
    }
    pPed->field_0x9 &= ~0x1;
    if (pPed->field_0xbc != NULL) {
        MorphCharacterBonePositions(pPed, 0);
        return;
    }
    personality = pPed->personality;
    form = personality->form;
    actor_set = NULL;
    if (pPed->field_0x4 >= 0) {
        actor_set = &form->actor_sets[pPed->field_0x4];
    }
    ped_move_frame = &form->moves[pPed->field_0x7].move->frames[pPed->field_0x1c];
    for (i = 0; i < form->count_bones; i++) {
        int parent_index = form->bones[i].indices[0];

        if (parent_index >= 0) {
            if (!(C2V(gPow2_array)[i] & pPed->field_0xc) && !(C2V(gPow2_array)[parent_index] & pPed->field_0xc)
                    && pArg2 != 1 && pArg2 != 2 && actor_set != NULL) {
                br_matrix34* mat;
                br_matrix34* parent_mat;
                tPed_move_frame_axis* axis;

                if (pPed->field_0xe8 != NULL) {
                    mat = &pPed->field_0xe8[i];
                    parent_mat = &pPed->field_0xe8[parent_index];
                } else {
                    mat = &actor_set->actors[i]->t.t.mat;
                    parent_mat = &actor_set->actors[parent_index]->t.t.mat;
                }
                axis = &ped_move_frame->axis[i];

                SetBoner(pPed, mat, parent_mat, &pPed->field_0x8c, axis->euler1, axis->euler2, axis->euler3,
                    &personality->bones[i].field_0x20, &personality->bones[i].field_0x2c);
            }
        } else {
            if (pArg2 != 3 && !(pPed->field_0x14 & 0x4)) {
                br_matrix34* mat;
                br_vector3 original_pos;

                if (pPed->field_0x4 < 0) {
                    mat = &pPed->field_0x2c;
                } else if (pPed->field_0xe8 == NULL) {
                    mat = &form->actor_sets[pPed->field_0x4].actors[0]->t.t.mat;
                } else {
                    mat = pPed->field_0xe8;
                }
                BrVector3Copy(&original_pos, (br_vector3*)mat->m[3]);
                if (pArg2 != 2) {
                    BrMatrix34Mul(mat, &ped_move_frame->mat, &pPed->field_0x8c);
                }
                if (pArg3 == 0 || C2V(gPed_overall_movement_disabled)) {
                    BrVector3Copy((br_vector3*)mat->m[3], &original_pos);
                } else {
                    br_vector3 move;
                    br_vector3 delta;

                    BrVector3Scale(&move, (br_vector3*)ped_move_frame->mat.m[3], personality->moves[pPed->field_0x7].scale_factor);
                    BrMatrix34ApplyV(&delta, &move, &pPed->field_0x5c);
                    if (C2V(gAction_replay_mode) && ARGetReplayRate() < 0) {
                        BrVector3Sub((br_vector3*)mat->m[3], &original_pos, &delta);
                    } else {
                        BrVector3Add((br_vector3*)mat->m[3], &original_pos, &delta);
                    }
                }
            }
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00407b30, SetCharacterBonePositions, SetCharacterBonePositions_original)

void C2_HOOK_FASTCALL ResetAnimation(tPed_character_instance* pPed) {

    pPed->field_0x1c = 0;
    pPed->field_0x24 = 0;
    pPed->field_0x18 = 0;
    pPed->field_0x8 = -1;
    if (pPed->field_0xbc != NULL) {
        pPed->field_0xbc->field_0x4 = 0;
        pPed->field_0xbc = NULL;
    }
}

void C2_HOOK_FASTCALL SetOnGroundVector(tPed_character_instance* pPed, br_vector3* pPos, br_vector3* pNew_pos) {
    float cast_y;

    cast_y = FindYVerticallyBelow2(pPos);
    pNew_pos->v[0] = pPos->v[0];
    pNew_pos->v[1] = cast_y + pPed->personality->moves[pPed->field_0x7].grounding_offset;
    pNew_pos->v[2] = pPos->v[2];
}

void C2_HOOK_FASTCALL RecalculateOrientationOfRoot(tPed_character_instance* pPed) {
    br_matrix34* character_matrix;
    tPed_form* form;
    br_matrix34 mat34;

    character_matrix = GetCharacterMatrixPtr(pPed);
    form = pPed->personality->form;
    BrMatrix34LPInverse(&mat34, &form->moves[pPed->field_0x7].move->frames[pPed->field_0x1c].mat);
    BrMatrix34Mul(&pPed->field_0x8c, &mat34, character_matrix);
    BrVector3Set((br_vector3*)pPed->field_0x8c.m[3], 0.f, 0.f, 0.f);
}

int (C2_HOOK_FASTCALL * SetCharacterMove_original)(tPed_character_instance* pPed,int pMove_action, float pSpeed, int pArg4, int pArg5, undefined4 pArg6);
int C2_HOOK_FASTCALL SetCharacterMove(tPed_character_instance* pPed,int pMove_action, float pSpeed, int pArg4, int pArg5, undefined4 pArg6) {

#if 0//defined(C2_HOOKS_ENABLED)
    return SetCharacterMove_original(pPed, pMove_action, pSpeed, pArg4, pArg5, pArg6);
#else
    tPed_personality* personality;
    tPed_form* form;
    tPed_move* original_ped_move;
    tPed_move* ped_move;
    tPed_morph* ped_morph;
    br_matrix34* character_matrix;
    int move_id;
    tS8 original_field_0x7;
    undefined2 original_0x1e;
    float new_field_0x1e_0x20;
    tPed_move_frame* ped_frame;

    personality = pPed->personality;
    form = personality->form;
    original_field_0x7 = pPed->field_0x7;
    original_ped_move = NULL;
    if (original_field_0x7 >= 0) {
        original_ped_move = form->moves[pPed->field_0x7].move;
    }
    character_matrix = GetCharacterMatrixPtr(pPed);
    move_id = pMove_action & ~0x40000000;
    if (move_id >= 0) {
        int i;

        for (i = 0; i < form->count_moves; i++) {
            if (form->moves[i].id == move_id) {
                if (pArg4 >= 0) {
                    pPed->field_0x7 = (tS8)i;
                    pPed->field_0x8 = -1;
                    if (pArg5 != 0) {
                        pPed->field_0x9 |= 0x2;
                    } else {
                        pPed->field_0x9 &= ~0x2;
                    }
                } else {
                    pPed->field_0x8 = (tS8)i;
                    if (pArg5 != 0) {
                        pPed->field_0x9 |= 0x4;
                    } else {
                        pPed->field_0x9 &= ~0x4;
                    }
                }
                break;
            }
        }
        if (i == form->count_moves) {
            return 2;
        }
    }
    original_0x1e = pPed->field_0x1e;
    if (pSpeed >= 0.f) {
        new_field_0x1e_0x20 = pSpeed;
    } else {
        new_field_0x1e_0x20 = -(float) form->moves[original_field_0x7].move->frametime * pSpeed;
    }
    if (pArg4 >= 0) {
        pPed->field_0x1e = (undefined2)new_field_0x1e_0x20;;
    } else {
        pPed->field_0x20 = (undefined2)new_field_0x1e_0x20;
    }
    if ((original_field_0x7 >= 0 && form->moves[original_field_0x7].id == move_id && !(pMove_action & 0x40000000U))
           || move_id < 0 || pArg4 < 0) {
        return 1;
    }
    ped_move = personality->form->moves[pPed->field_0x7].move;
    if (original_ped_move != NULL && original_ped_move->field_0x2a == 0 && !C2V(gPed_overall_movement_disabled)) {
        int i;

        for (i = 0; i < pPed->field_0x1c; i++) {
        }
    }
    if (original_ped_move != NULL) {
        ped_frame = &original_ped_move->frames[pPed->field_0x1c];
    }
    ResetAnimation(pPed);
    if (pArg4 != 0 && original_ped_move != NULL && pArg6 != 0) {
        int i;

        for (i = 0; i < REC2_ASIZE(C2V(gPed_morphs)); i++) {
            if (!C2V(gPed_morphs)[i].field_0x4) {
                pPed->field_0xbc = &C2V(gPed_morphs)[i];
                break;
            }
        }
    }
    ped_morph = pPed->field_0xbc;
    if (ped_morph == NULL) {
        pArg4 = 0;
    }
    if (pArg4 != 0) {
        br_euler e;
        float sum;
        int i;

        BrVector3Copy(&ped_morph->field_0x20, &original_ped_move->field_0x40);
        if (original_0x1e != 0 && original_ped_move->field_0x2a == 0) {
            BrVector3InvScale(&ped_morph->field_0x8, &ped_morph->field_0x20, (float)(original_ped_move->count_frames * original_0x1e));
        } else {
            BrVector3Set(&ped_morph->field_0x8, 0.f, 0.f, 0.f);
        }
        if (new_field_0x1e_0x20 != 0 && ped_move->field_0x2a == 0) {
            BrVector3InvScale(&ped_morph->field_0x14, &ped_move->field_0x34, (float)(ped_move->count_frames * new_field_0x1e_0x20));
        } else {
            BrVector3Set(&ped_morph->field_0x14, 0.f, 0.f, 0.f);
        }
        e.order = 0;
        BrMatrix34ToEuler(&e, &ped_frame->mat);
        ped_morph->field_0x2d[0][0] = e.a / 256;
        ped_morph->field_0x2d[0][1] = e.b / 256;
        ped_morph->field_0x2d[0][2] = e.c / 256;
        BrMatrix34ToEuler(&e, &ped_move->frames[0].mat);
        ped_morph->field_0x2d[0][3] = e.a / 256;
        ped_morph->field_0x2d[0][4] = e.b / 256;
        ped_morph->field_0x2d[0][5] = e.c / 256;

        sum = 0.f;
        for (i = 1; i < form->count_bones; i++) {
            ped_morph->field_0x2d[i][6*i+0] = ped_frame->axis[i].euler1;
            ped_morph->field_0x2d[i][6*i+1] = ped_frame->axis[i].euler2;
            ped_morph->field_0x2d[i][6*i+2] = ped_frame->axis[i].euler3;
            ped_morph->field_0x2d[i][6*i+3] = ped_move->frames->axis[i].euler1 - ped_frame->axis[i].euler1 ;
            ped_morph->field_0x2d[i][6*i+4] = ped_move->frames->axis[i].euler2 - ped_frame->axis[i].euler2 ;
            ped_morph->field_0x2d[i][6*i+5] = ped_move->frames->axis[i].euler3 - ped_frame->axis[i].euler3 ;
            sum += (float)(abs(ped_morph->field_0x2d[i][6*i+3]) + abs(ped_morph->field_0x2d[i][6*i+4]) + abs(ped_morph->field_0x2d[i][6*i+5]));
        }
        pPed->field_0xbc->field_0x2c = !!(pArg4 & 0x40000000);
        ped_morph->field_0x0 = 0.f;
        ped_morph->field_0x4 = 1.f / (sum * 3.f * (float)form->count_bones * (pArg4 & (~0x40000000)) / 64.f);
        pPed->field_0x24 = 0;
    }

    if (C2V(gPed_retain_root_mode)) {
        RecalculateOrientationOfRoot(pPed);
        SetCharacterBonePositions(pPed, 3, 0);
    } else {
        if (personality->moves[pPed->field_0x7].grounding_mode == ePed_personality_grounded) {
            SetOnGroundVector(pPed, (br_vector3*)character_matrix->m[3], (br_vector3*)character_matrix->m[3]);
        }
        SetCharacterBonePositions(pPed, 0, 0);
    }
    return 0;
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

tCollision_info* C2_HOOK_FASTCALL GetRootObject(tPed_character_instance *pPed) {
    tPed_form* form;
    int i;

    form = pPed->personality->form;
    for (i = 0; i < form->count_bones; i++) {
        if (form->boned_physicing[pPed->field_0x5].collision_infos[i]->shape != NULL) {
            return form->boned_physicing[pPed->field_0x5].collision_infos[i];
        }
    }
    return NULL;
}
C2_HOOK_FUNCTION(0x00409040, GetRootObject)

void C2_HOOK_FASTCALL DropPointOntoPlane(const br_vector3* pPoint, const br_vector3* pPlane, br_vector3* pDest) {
    float dot;

    dot = -BrVector3Dot(pPlane, pPoint);
    BrVector3Scale(pDest, pPlane, dot);
    BrVector3Accumulate(pDest, pPoint);
}
C2_HOOK_FUNCTION(0x0040b030, DropPointOntoPlane)

void C2_HOOK_FASTCALL OrthogonalVector(br_vector3* pDest, const br_vector3* pA, const br_vector3* pB) {

    BrVector3Cross(pDest, pA, pB);
    if (fabsf(pDest->v[0]) < 2.384186e-06f && fabsf(pDest->v[1]) < 2.384186e-06f && fabsf(pDest->v[1]) < 2.384186e-06f) {
        if (fabsf(pA->v[0]) <= fabsf(pA->v[2])) {
            BrVector3Cross(pDest, pA, &C2V(g_Ped_x_unit_vector));
        } else {
            BrVector3Cross(pDest, pA, &C2V(g_Ped_z_unit_vector));
        }
    }
}
C2_HOOK_FUNCTION(0x00404150, OrthogonalVector)

void (C2_HOOK_FASTCALL * SetCharacterDirection_original)(tPed_character_instance* pPed, const br_vector3* pDir, const br_vector3* pUp);
void C2_HOOK_FASTCALL SetCharacterDirection(tPed_character_instance* pPed, const br_vector3* pDir, const br_vector3* pUp) {

#if 0//defined(C2_HOOKS_ENABLED)
    return SetCharacterDirection_original(pPed, pDir, pUp);
#else
    int keep_direction;
    tPed_move* move;
    br_matrix34* character_matrix;
    br_vector3 original_pos;
    br_vector3 norm_dir;
    br_vector3 norm_up;
    br_vector3 local_138;
    br_vector3 local_12c;
    br_vector3 local_120;
    br_vector3 local_114;
    br_vector3 local_fc;
    br_vector3 local_e4;
    br_matrix34 inv_field_0x8c;
    br_matrix34 local_c0;
    br_matrix34 local_60;
    br_matrix34 local_30;

    if (pPed->field_0x7 < 0) {
        return;
    }
    if ((pPed->field_0x14 & 0x4) && !C2V(gAction_replay_mode)) {
        return;
    }
    if (pDir == NULL) {
        pDir = &pPed->field_0xc0;
        pUp = &pPed->field_0xcc;
        keep_direction = 1;
    } else {
        BrVector3Normalise(&pPed->field_0xc0, pDir);
        BrVector3Copy(&pPed->field_0xcc, pUp);
        keep_direction = 0;
    }
    move = pPed->personality->form->moves[pPed->field_0x7].move;
    character_matrix = GetCharacterMatrixPtr(pPed);
    BrMatrix34Inverse(&inv_field_0x8c, &pPed->field_0x8c);
    BrVector3Copy(&original_pos, (br_vector3*)character_matrix->m[3]);
    BrVector3Set((br_vector3*)character_matrix->m[3], 0.f, 0.f, 0.f);
    BrMatrix34Mul(&local_c0, character_matrix, &inv_field_0x8c);
    if (pUp != NULL) {
        BrVector3Normalise(&norm_up, pUp);
    } else {
        BrVector3Set(&norm_up, 0.f, 1.f, 0.f);
    }
    BrVector3Normalise(&norm_dir, pDir);
    BrVector3Copy(&local_12c, &move->field_0x40);
    if (pPed->field_0xbc != NULL) {
        tPed_morph* morph = pPed->field_0xbc;
        Vector3Lerp(&local_12c, &local_12c, &morph->field_0x20, morph->field_0x0);
        BrVector3Normalise(&local_12c, &local_12c);
    }
    DropPointOntoPlane(&norm_dir, &norm_up, &local_e4);
    DropPointOntoPlane(&local_12c, &norm_up, &local_114);
    BrVector3Normalise(&local_fc, &local_e4);
    BrVector3Normalise(&local_114, &local_114);
    OrthogonalVector(&local_138, &local_114, &local_fc);
    BrVector3Normalise(&local_120, &local_138);
    DRMatrix34RotateCos(&pPed->field_0x8c, &local_120, BrVector3Dot(&local_fc, &local_114));
    BrVector3Set(&local_138, -norm_up.v[0], 0.f, norm_up.v[2]);
    BrVector3Normalise(&local_120, &local_138);
    local_120.v[1] = 0.0;
    DRMatrix34RotateCos(&local_60, &local_120, norm_up.v[1]);
    BrMatrix34Pre(&pPed->field_0x8c, &local_60);
    DRVector3SafeCross(&local_138, &local_fc, &norm_dir);
    BrVector3Normalise(&local_120, &local_138);
    DRMatrix34RotateCos(&local_30, &local_120, BrVector3Dot(&local_fc, &norm_dir));
    BrMatrix34Mul(&pPed->field_0x5c, &pPed->field_0x8c, &local_30);
    BrMatrix34Post(&local_c0, &pPed->field_0x8c);
    BrVector3Accumulate((br_vector3*)local_c0.m[3], &original_pos);
    BrMatrix34LPNormalise(character_matrix, &local_c0);
    if (!keep_direction) {
        SetCharacterBonePositions(pPed, 3, 0);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0040a730, SetCharacterDirection, SetCharacterDirection_original)

int C2_HOOK_FASTCALL OrientationChanged(const br_matrix34* pOrientation_1, const br_matrix34* pOrientation_2, float pCos_angle) {

    return BrVector3Dot((br_vector3*)pOrientation_1->m[0], (br_vector3*)pOrientation_2->m[0]) < pCos_angle
        || BrVector3Dot((br_vector3*)pOrientation_1->m[1], (br_vector3*)pOrientation_2->m[1]) < pCos_angle
        || BrVector3Dot((br_vector3*)pOrientation_1->m[2], (br_vector3*)pOrientation_2->m[2]) < pCos_angle;
}

void C2_HOOK_FASTCALL AssertRootObjectsMatrix(tPed_character_instance* pPed) {
    tCollision_info* collision_info;
    int i;

    if (pPed->field_0x14 & 0x1) {
        collision_info = pPed->personality->form->simple_physicing[pPed->field_0x5].collision_info;
    } else {
        collision_info = GetRootObject(pPed);
    }
    for (i = 0; i < 3; i++) {
        int j;

        for (j = 0; j < 3; j++) {
            collision_info->transform_matrix.m[i][j] = collision_info->actor->t.t.mat.m[i][j];
        }
    }
    collision_info->flags |= 0x100;
}

void (C2_HOOK_FASTCALL * SetCharacterDirectionAR_original)(tPed_character_instance* pPed, br_vector3* pDir, br_vector3* pUp);
void C2_HOOK_FASTCALL SetCharacterDirectionAR(tPed_character_instance* pPed, br_vector3* pDir, br_vector3* pUp) {

#if 0//defined(C2_HOOKS_ENABLED)
    SetCharacterDirectionAR_original(pPed, pDir, pUp);
#else
    tCollision_info* collision_info;
    if (pPed->field_0x14 & 0x1) {
        collision_info = pPed->personality->form->simple_physicing[pPed->field_0x5].collision_info;
    } else {
        collision_info = GetRootObject(pPed);
    }
    if (!C2V(gAction_replay_mode) || C2V(gBOOL_00744804)) {
        if (!(pPed->field_0x14 & 0x4) && C2V(gProgram_state).racing) {

            C2_HOOK_BUG_ON(sizeof(C2V(gPedestrian_array)[0]) != 0x54);

            PipeSinglePedDir((pPed->ped->field_0x06 << 16) | (pPed->ped - C2V(gPedestrian_array)), pDir);
        }
        SetCharacterDirection(pPed, pDir, pUp);
        if (C2V(gProgram_state).racing && OrientationChanged(&collision_info->transform_matrix, &collision_info->actor->t.t.mat, .7f)) {
            AssertRootObjectsMatrix(pPed);
        }
    }
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

        C2V(gNapalm_bolts)[0].ped = NULL;
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

void C2_HOOK_FASTCALL RenderElectroBastardRays(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world, tU32 pTime) {
    tU32 the_time;
    int i;
    float seed;
    br_vector3 car_pos;
    br_vector3 ped_pos;
    br_vector3 car_pos_cam;
    br_vector3 ped_pos_cam;
    br_vector3 car_add;
    br_vector3 car_add_c;
    br_vector3 ray;
    br_vector3 r1;
    br_vector3 from_pos;
    br_vector3 to_pos;
    br_model* car_model;
    br_scalar distance;
    br_scalar t;
    int model_updated = 0;

    the_time = GetTotalTime();
    ARStartPipingSession(ePipe_chunk_prox_ray);
    for (i = 0; i < REC2_ASIZE(C2V(gProximity_rays)); i++) {

        if (C2V(gProximity_rays)[i].start_time == 0) {
            continue;
        }
        if (the_time - C2V(gProximity_rays)[i].start_time >= 500) {
            C2V(gProximity_rays)[i].start_time = 0;
            continue;
        }
        if (C2V(gNo_2d_effects) && !model_updated) {
            BrActorRemove(C2V(gLine_actor));
            BrActorAdd(C2V(gCamera), C2V(gLine_actor));
            C2V(gLine_model)->vertices[0].red = 0x80;
            C2V(gLine_model)->vertices[0].grn = 0x80;
            C2V(gLine_model)->vertices[0].blu = 0x80;
            C2V(gLine_model)->vertices[1].red = 0xff;
            C2V(gLine_model)->vertices[1].grn = 0xff;
            C2V(gLine_model)->vertices[1].blu = 0xff;
            BrModelUpdate(C2V(gLine_model), BR_MODU_ALL);
            model_updated = 1;
        }
        AddProxRayToPipingSession(i, C2V(gProximity_rays)[i].car, C2V(gProximity_rays)[i].ped - C2V(gPedestrian_array), C2V(gProximity_rays)[i].start_time);
        car_model =  C2V(gProximity_rays)[i].car->car_actor->model;

        BrVector3Set(&car_add_c, 0.f, (car_model->bounds.max.v[1] - car_model->bounds.min.v[1]) / -5.f, 0.f);
        BrMatrix34ApplyV(&car_add, &car_add_c, &C2V(gProximity_rays)[i].car->car_master_actor->t.t.mat);
        BrVector3Add(&car_pos, &C2V(gProximity_rays)[i].car->pos, &car_add);
        BrVector3Copy(&ped_pos, &C2V(gProximity_rays)[i].ped->pos);
        DRMatrix34TApplyP(&car_pos_cam, &car_pos, &C2V(gCamera_to_world));
        DRMatrix34TApplyP(&ped_pos_cam, &ped_pos, &C2V(gCamera_to_world));

        BrVector3Sub(&r1, &ped_pos_cam, &car_pos_cam);
        distance = BrVector3Length(&r1);
        BrVector3Normalise(&r1, &r1);

        BrVector3Copy(&from_pos, &car_pos_cam);

        seed = ped_pos.v[0] + ped_pos.v[1] + ped_pos.v[2] + car_pos.v[0] + car_pos.v[1] + car_pos.v[2];
        srand(the_time + (int)seed);

        for (t = 0.f; t < distance; t += .05f) {
            BrVector3Scale(&ray, &r1, t);
            BrVector3Add(&to_pos, &ray, &car_pos_cam);
            to_pos.v[0] += SRandomPosNeg(.1f);
            to_pos.v[1] += SRandomPosNeg(.1f);
            to_pos.v[2] += SRandomPosNeg(.1f);
            DrawLine3D(&to_pos, &from_pos, pRender_screen, pDepth_buffer, C2V(gProx_ray_shade_table));
            BrVector3Copy(&from_pos, &to_pos);
            t += .05f;
        }
        DrawLine3D(&ped_pos_cam, &from_pos, pRender_screen, pDepth_buffer, C2V(gProx_ray_shade_table));
    }
    AREndPipingSession();
    if (C2V(gNo_2d_effects) && model_updated) {
        BrActorRemove(C2V(gLine_actor));
        BrActorAdd(C2V(gDont_render_actor), C2V(gLine_actor));
    }
}
C2_HOOK_FUNCTION(0x004d5d60, RenderElectroBastardRays)

void C2_HOOK_FASTCALL ResetPedNearness(void) {

    C2V(gPed_nearness) = 1;
}
C2_HOOK_FUNCTION(0x004d6390, ResetPedNearness)

void C2_HOOK_FASTCALL PossiblePedSmear(tPedestrian* pPed) {
    br_vector3 pos;
    tPed_cache_006944c0* smear_info = pPed->field_0x0c;

    if (smear_info == NULL) {
        return;
    }
    if ((smear_info->field_0xc4 & 0x2) && smear_info->field_0xca >= 2) {
        BrVector3Scale(&pos, &smear_info->field_0xe4, 1.f / (float)smear_info->field_0xca);
        if (!(smear_info->field_0xc4 & 0x1)) {
            smear_info->field_0xc8 = -1;
            smear_info->field_0xc4 |= 0x1;
        } else if (SkidSection(
                &smear_info->field_0xc8,
                &smear_info->field_0xcc,
                &smear_info->field_0xd8,
                C2V(gPed_smear_material),
                &pos,
                &smear_info->field_0xf0,
                &smear_info->field_0xfc,
                &smear_info->field_0x108,
                0.0f,
                smear_info->field_0x124 - smear_info->field_0x128 + 0.1f)) {
            smear_info->field_0xc4 &= ~0x1;
        }
        BrVector3Copy(&smear_info->field_0xfc, &pos);
        BrVector3Copy(&smear_info->field_0x108, &smear_info->field_0xf0);
    } else {
        smear_info->field_0xc4 &= ~0x1;
    }
}

void (C2_HOOK_FASTCALL * LastChanceForPedEffects_original)(void);
void C2_HOOK_FASTCALL LastChanceForPedEffects(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    LastChanceForPedEffects_original();
#else
    int i;

    for (i = 0; i < REC2_ASIZE(C2V(gPedestrian_array)); i++) {
        tPedestrian* ped = &C2V(gPedestrian_array)[i];

        if (ped->flags & 0x1) {
            PossiblePedSmear(ped);
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004d5970, LastChanceForPedEffects, LastChanceForPedEffects_original)

void (C2_HOOK_FASTCALL * MungeNapalm_original)(void);
void C2_HOOK_FASTCALL MungeNapalm(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    MungeNapalm_original();
#else
    tU32 the_time;
    int i;

    the_time = GetTotalTime();
    ARStartPipingSession(ePipe_chunk_burning_ped);
    for (i = 0; i < REC2_ASIZE(C2V(gNapalm_bolts)); i++) {
        tNapalm_bolt* bolt;
        int bolt_hit_target;

        bolt = &C2V(gNapalm_bolts)[i];
        if (bolt->ped == NULL) {
            continue;
        }

        bolt_hit_target = 0;
        if (C2V(gAction_replay_mode)) {
            bolt_hit_target = 1;
        } else {

            if (!(bolt->ped->flags & 0x1)) {
                bolt->ped->field_0x0c->field_0x98 = 0;
                KillNapalmBolt(bolt);
            } else {
                tU32 delta_time;
                tCollision_info* object;
                float dt;
                br_vector3 delta_move;
                float delta_length_sq;

                delta_time = the_time - bolt->field_0x28;
                if (the_time - bolt->field_0x24 > 50) {

                    C2_HOOK_BUG_ON((REC2_ASIZE(bolt->field_0x2c) - 1) * sizeof(br_vector3) != 0x30);

                    c2_memmove(&bolt->field_0x2c[1], &bolt->field_0x2c[0], (REC2_ASIZE(bolt->field_0x2c) - 1) * sizeof(br_vector3));
                    BrVector3Sub(&bolt->field_0x2c[0], &bolt->ped->pos, &bolt->actors[0]->t.t.translate.t);
                    BrVector3Normalise(&bolt->field_0x2c[0], &bolt->field_0x2c[0]);
                    BrVector3Scale(&bolt->field_0x2c[0], &bolt->field_0x2c[0], FRandomBetween(.8f, 1.25f));
                    if (delta_time < 500) {
                        bolt->field_0x2c[0].v[1] += (float)(500 - delta_time) * 2.f / 1000.f;
                    }
                }
                dt = (float)C2V(gFrame_period) * 2.f / 1000.f;
                bolt->actors[0]->t.t.translate.t.v[0] += dt * bolt->field_0x2c[0].v[0];
                bolt->actors[0]->t.t.translate.t.v[1] += dt * bolt->field_0x2c[0].v[1];
                bolt->actors[0]->t.t.translate.t.v[2] += dt * bolt->field_0x2c[0].v[2];
                object = bolt->ped->character->personality->form->simple_physicing[bolt->ped->character->field_0x5].collision_info;
                BrVector3Sub(&delta_move, &bolt->actors[0]->t.t.translate.t, &object->actor->t.t.translate.t);
                delta_length_sq = BrVector3LengthSquared(&delta_move);
                if (delta_length_sq < REC2_SQR(object->bb1.max.v[0])
                        || delta_length_sq < REC2_SQR(object->bb1.max.v[1])
                        || delta_length_sq < REC2_SQR(object->bb1.max.v[2])
                        || delta_length_sq < REC2_SQR(object->bb1.min.v[0])
                        || delta_length_sq < REC2_SQR(object->bb1.min.v[1])
                        || delta_length_sq < REC2_SQR(object->bb1.min.v[2])) {
                    int j;

                    for (j = 0; j < REC2_ASIZE(bolt->actors); j++) {
                        br_vector3 delta_world;

                        if (j != 0) {
                            BrVector3Sub(&delta_move, &bolt->actors[j]->t.t.translate.t, &object->actor->t.t.translate.t);
                        }
                        BrMatrix34TApplyP(&delta_world, &delta_move, &object->actor->t.t.mat);
                        if (delta_world.v[0] <= object->bb1.max.v[0]
                                && delta_world.v[0] >= object->bb1.min.v[0]
                                && delta_world.v[1] <= object->bb1.max.v[1]
                                && delta_world.v[1] >= object->bb1.min.v[1]
                                && delta_world.v[2] <= object->bb1.max.v[2]
                                && delta_world.v[2] >= object->bb1.min.v[2]) {
                            bolt_hit_target = 1;
                            break;
                        }
                    }
                }
                if (bolt_hit_target) {
                    br_actor* character_actor;

                    Explode(&C2V(gNapalmed_ped_animation), GetCharacterActorPtr(bolt->ped->character), &bolt->ped->character->personality->bb, &bolt->ped->character->personality->form->simple_physicing[bolt->ped->character->field_0x5].collision_info->cmpos);
                    bolt->ped->field_0x0c->field_0x98 = the_time + 15000;
                    character_actor = GetCharacterActorPtr(bolt->ped->character);
                    bolt->ped->field_0x0c->field_0xc0 = bolt->field_0x68;
                    bolt->ped->field_0x0c->field_0x2e |= 0x20;
                    ScarePedestrian(bolt->ped, the_time, 1, 1);
                    CreateSmokeColumn2(2, character_actor, NULL, bolt->ped->character->personality->form->simple_physicing[bolt->ped->character->field_0x5].collision_info, 0, IRandomBetween(0, character_actor->model->prepared->groups->nvertices), 25000);
                    DRS3StartSound3D(C2V(gPedestrians_outlet), eSoundId_NapalmHitCrackls,
                        &bolt->ped->pos, &C2V(gZero_v__car), 1, 255, BR_FIXED_INT(1), BR_FIXED_INT(1));
                    if (bolt->ped->hit_points > 0 && bolt->field_0x68 != NULL && bolt->field_0x68->driver == eDriver_local_human) {
                        PipeSinglePedIncident(bolt->ped, bolt->field_0x68->collision_info->actor);
                    }
                    KillNapalmBolt(bolt);
                }
            }
        }
        if (!bolt_hit_target) {
            br_vector3 flame_positions[7];
            int j;
            tU32 time = the_time;

            C2_HOOK_BUG_ON(REC2_ASIZE(flame_positions) != REC2_ASIZE(C2V(gFlamed_ped_flame_scales)));
            c2_memset(flame_positions, 0, sizeof(flame_positions));
            for (j = 0; i < REC2_ASIZE(flame_positions); i++) {
                br_actor* actor = bolt->actors[j];
                if (!C2V(gAction_replay_mode)) {
                    time = (int)((float)time - 0.036f);
                    if (j != 0) {
                        int vert_index;

                        if (time < bolt->field_0x28) {
                            break;
                        }
                        vert_index = (bolt->field_0x24 - time) / 50;
                        vert_index = vert_index >= 0 ? vert_index : 0;
                        actor->t.t.translate.t.v[0] = bolt->actors[j - 1]->t.t.translate.t.v[0] - bolt->field_0x2c[vert_index].v[0] * .036f;
                        actor->t.t.translate.t.v[1] = bolt->actors[j - 1]->t.t.translate.t.v[1] - bolt->field_0x2c[vert_index].v[1] * .036f;
                        actor->t.t.translate.t.v[2] = bolt->actors[j - 1]->t.t.translate.t.v[2] - bolt->field_0x2c[vert_index].v[2] * .036f;
                        actor->render_style = BR_RSTYLE_FACES;
                    }
                } else if (actor->render_style != BR_RSTYLE_FACES) {
                    break;
                }
                actor->t = C2V(gCamera)->t;
                BrMatrix34PreScale(&actor->t.t.mat,
                    (float)actor->material->colour_map->width * C2V(gFlamed_ped_flame_scales)[j] / 128.f,
                    (float)actor->material->colour_map->height * C2V(gFlamed_ped_flame_scales)[j] / 128.f,
                    1.f);
                BrMatrix34PreRotateZ(&actor->t.t.mat,
                    (br_angle)actor->t.t.translate.t.v[0] ^ (br_angle)actor->t.t.translate.t.v[1] ^ (br_angle)actor->t.t.translate.t.v[2]);
                actor->render_style = BR_RSTYLE_FACES;
                BrVector3Copy(&flame_positions[j], &actor->t.t.translate.t);
            }
            AddBurningPedToSession(i, bolt->ped, flame_positions);
        }
    }
    AREndPipingSession();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ce7d0, MungeNapalm, MungeNapalm_original)

void (C2_HOOK_FASTCALL * StopCharacterMorphing_original)(tPed_character_instance* pCharacter);
void C2_HOOK_FASTCALL StopCharacterMorphing(tPed_character_instance* pCharacter) {

#if defined(C2_HOOKS_ENABLED)
    StopCharacterMorphing_original(pCharacter);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004096f0, StopCharacterMorphing, StopCharacterMorphing_original)

void (C2_HOOK_CDECL * SetCharacterPhysicsLevelAR_original)(tPed_character_instance* pCharacter, tU32 pLevel);
void C2_HOOK_CDECL SetCharacterPhysicsLevelAR(tPed_character_instance* pCharacter, tU32 pLevel) {

#if defined(C2_HOOKS_ENABLED)
    SetCharacterPhysicsLevelAR_original(pCharacter, pLevel);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004cc860, SetCharacterPhysicsLevelAR, SetCharacterPhysicsLevelAR_original)

undefined4 (C2_HOOK_FASTCALL * CharacterNoLongerRenderable_original)(tPed_character_instance* pCharacter);
undefined4 C2_HOOK_FASTCALL CharacterNoLongerRenderable(tPed_character_instance* pCharacter) {

#if defined(C2_HOOKS_ENABLED)
    return CharacterNoLongerRenderable_original(pCharacter);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00408400, CharacterNoLongerRenderable, CharacterNoLongerRenderable_original)

void (C2_HOOK_FASTCALL * OneLessPed_original)(tPedestrian* pPed);
void C2_HOOK_FASTCALL OneLessPed(tPedestrian* pPed) {

#if defined(C2_HOOKS_ENABLED)
    OneLessPed_original(pPed);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004cd160, OneLessPed, OneLessPed_original)

void (C2_HOOK_FAKE_THISCALL * ScoreForKilledPedestrian_original)(tPedestrian* pPed, undefined4 pArg2, float pHeight);
void C2_HOOK_FAKE_THISCALL ScoreForKilledPedestrian(tPedestrian* pPed, undefined4 pArg2, float pHeight) {

#if defined(C2_HOOKS_ENABLED)
    ScoreForKilledPedestrian_original(pPed REC2_THISCALL_EDX, pHeight);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004cd260, ScoreForKilledPedestrian, ScoreForKilledPedestrian_original)

int (C2_HOOK_FASTCALL * SetCharacterPhysicsLevel_original)(tPed_character_instance* pCharacter, int pLevel);
int C2_HOOK_FASTCALL SetCharacterPhysicsLevel(tPed_character_instance* pCharacter, int pLevel) {

#if defined(C2_HOOKS_ENABLED)
    return SetCharacterPhysicsLevel_original(pCharacter, pLevel);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00409570, SetCharacterPhysicsLevel, SetCharacterPhysicsLevel_original)

void C2_HOOK_FASTCALL MakePedVanish(tPedestrian* pPed) {

    PipeSinglePedPos(pPed, &pPed->pos, &C2V(gZero_v__car));
    pPed->flags |= 0x80;
    C2V(gPipe_halted_ped_status) = 1;
    SetCharacterPhysicsLevelAR(pPed->character, 0);
    CharacterNoLongerRenderable(pPed->character);
    C2V(gPipe_halted_ped_status) = 0;
    OneLessPed(pPed);
}

int C2_HOOK_FASTCALL PedFallingForever(tPedestrian* pPed) {

    if (pPed->flags & 0x80) {
        return 0;
    }
    MakePedVanish(pPed);
    if (pPed->hit_points > 0) {
        ScoreForKilledPedestrian(pPed REC2_THISCALL_EDX, pPed->pos.v[1] - FindYVerticallyBelow(&pPed->pos));
    }
    return 1;
}

void C2_HOOK_FASTCALL KillNapalmBolt(tNapalm_bolt* pBolt) {
    size_t i;

    for (i = 0; i < REC2_ASIZE(pBolt->actors); i++) {
        pBolt->actors[i]->render_style = BR_RSTYLE_NONE;
    }
    pBolt->ped = NULL;
}

void C2_HOOK_FASTCALL SetNextRandomTurn(tPedestrian* pPed, tU32 pTime) {

    if (pPed->movement_spec->max_time_between != 0) {
        pPed->field_0x0c->next_turn_time = pTime + IRandomBetween(pPed->movement_spec->min_time_between, pPed->movement_spec->max_time_between);
    } else {
        pPed->field_0x0c->next_turn_time = 0;
    }
}

void C2_HOOK_FASTCALL InitProcessData(tPedestrian* pPed, tU32 pTime) {
    tPed_cache_006944c0* ped_field_0x0c;

    C2_HOOK_BUG_ON(sizeof(tPed_cache_006944c0) != 0x12c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_cache_006944c0, field_0x18, 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_cache_006944c0, field_0x19, 0x19);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_cache_006944c0, field_0x1c, 0x1c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_cache_006944c0, field_0x21, 0x21);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_cache_006944c0, field_0x23, 0x23);
#if 0
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_cache_006944c0, field_0x24, 0x24);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_cache_006944c0, field_0x25, 0x25);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_cache_006944c0, field_0x26, 0x26);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_cache_006944c0, field_0x27, 0x27);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_cache_006944c0, field_0x28, 0x28);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_cache_006944c0, field_0x29, 0x29);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_cache_006944c0, field_0x2a, 0x2a);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_cache_006944c0, field_0x2b, 0x2b);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_cache_006944c0, field_0x2c, 0x2c);
#endif
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_cache_006944c0, field_0x2e, 0x2e);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_cache_006944c0, field_0x6c, 0x6c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_cache_006944c0, field_0x80, 0x80);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_cache_006944c0, field_0x84, 0x84);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_cache_006944c0, field_0x88, 0x88);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_cache_006944c0, field_0x8c, 0x8c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_cache_006944c0, field_0x90, 0x90);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_cache_006944c0, field_0x94, 0x94);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_cache_006944c0, field_0x98, 0x98);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_cache_006944c0, field_0x9c, 0x9c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_cache_006944c0, field_0xa0, 0xa0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_cache_006944c0, field_0xc0, 0xc0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_cache_006944c0, field_0xc4, 0xc4);

    ped_field_0x0c = pPed->field_0x0c;
    if (ped_field_0x0c->field_0xb0 != NULL) {
        ped_field_0x0c->field_0xb0->field_0x14 = NULL;
        ped_field_0x0c->field_0xb0 = NULL;
    }
    SetNextRandomTurn(pPed, pTime);
    ped_field_0x0c->field_0x18 = 0;
    ped_field_0x0c->field_0x6c = 0;
    ped_field_0x0c->field_0x80 = 0;
    ped_field_0x0c->field_0x19 = 0;
    ped_field_0x0c->field_0xc0 = 0;
    ped_field_0x0c->field_0x1c = NULL;
    ped_field_0x0c->field_0x2e = 0;
    ped_field_0x0c->field_0x21 = 0;
    ped_field_0x0c->field_0xa0 = -1;
    ped_field_0x0c->field_0x9c = 0;
    ped_field_0x0c->field_0x88 = 0;
    ped_field_0x0c->field_0x8c = 0;
    ped_field_0x0c->field_0x84 = C2V(gPed_spurting_period);
    ped_field_0x0c->field_0xc4 &= ~0x1;
    ped_field_0x0c->field_0x94 = 0;
    ped_field_0x0c->field_0x90 = 0;
    ped_field_0x0c->field_0x98 = 0;
    ped_field_0x0c->field_0x23[0] = 0;
    ped_field_0x0c->field_0x23[1] = 0;
    ped_field_0x0c->field_0x23[2] = 0;
    ped_field_0x0c->field_0x23[3] = 0;
    ped_field_0x0c->field_0x23[4] = 0;
    ped_field_0x0c->field_0x23[5] = 0;
    ped_field_0x0c->field_0x23[6] = 0;
    ped_field_0x0c->field_0x23[7] = 0;
    ped_field_0x0c->field_0x23[8] = 0;
    ped_field_0x0c->field_0x23[9] = 0;
}

void C2_HOOK_FASTCALL AcceleratePed(tPedestrian* pPed, tU32 pTime) {

    if (pPed->field_0x0c != NULL && pPed->field_0x0c->field_0x80 != 0) {
        int speed;

        if (!C2V(gAction_replay_mode) || ARGetReplayRate() >= 0.f) {
            speed = (int)((float)pPed->field_0x0c->field_0x80 - (float)C2V(gFrame_period) * pPed->field_0x0c->field_0xb8);
            pPed->field_0x0c->field_0x80 = speed;
            if (speed <= 30) {
                speed = 30;
                pPed->field_0x0c->field_0x80 = 0;
            }
            pPed->field_0x0c->field_0x80 = MAX(30, pPed->field_0x0c->field_0x80);
        } else {
            speed = (int)((float)pPed->field_0x0c->field_0x80 + (float)C2V(gFrame_period) * pPed->field_0x0c->field_0xb8);
            pPed->field_0x0c->field_0x80 = speed;
            if (speed >= 100) {
                speed = 100;
                pPed->field_0x0c->field_0x80 = 0;
            }
        }
        if (C2V(gPedestrian_speed_factor) == 0.f) {
            SetCharacterMoveAR(pPed->character, -1, (float)speed, 0, 0, pTime);
        } else {
            SetCharacterMoveAR(pPed->character, -1, (float)speed / C2V(gPedestrian_speed_factor), 0, 0, pTime);
        }
    }
}

void C2_HOOK_FASTCALL SetCharacterPositionAR(tPed_character_instance* pCharacter, br_vector3* pPos, int pArg3) {
    br_vector3 current_pos;
    br_matrix34* mat;

    mat = GetCharacterMatrixPtr(pCharacter);
    BrVector3Copy(&current_pos, (br_vector3*)mat->m[3]);
    SetCharacterPosition(pCharacter, pPos, pArg3);
    if (!(pCharacter->field_0x14 & 0x4)) {
        PipeSinglePedPos(pCharacter->ped, &current_pos, (br_vector3*)GetCharacterMatrixPtr(pCharacter)->m[3]);
    }
}

void C2_HOOK_FASTCALL StopSmoothTurning(tPedestrian* pPed) {

    pPed->field_0x0c->field_0x6c = 0;
}

void C2_HOOK_FASTCALL SetThisPedPhysicing(tPedestrian* pPed) {

    StopSmoothTurning(pPed);
    SetCharacterPhysicsLevelAR(pPed->character, 5);
}

int C2_HOOK_FASTCALL PedAnimCausesMovement(tPedestrian* pPed) {

    return !pPed->character->personality->form->moves[pPed->character->field_0x7].move->field_0x2a;
}

void C2_HOOK_FASTCALL SmoothTurnPedestrian(tPedestrian* pPed, tU32 pTime) {
    tPed_cache_006944c0* ped_cache;

    ped_cache = pPed->field_0x0c;
    if (pTime < ped_cache->field_0x70) {
        float t;
        float invt;
        br_vector3 dir;

        t = (float)(pTime - ped_cache->field_0x6c) / (float)(ped_cache->field_0x70 - ped_cache->field_0x6c);
        invt = 1.f - t;

        BrVector3Set(&dir,
            t * ped_cache->field_0x54.v[0] + invt * ped_cache->field_0x60.v[0],
            t * ped_cache->field_0x54.v[1] + invt * ped_cache->field_0x60.v[1],
            t * ped_cache->field_0x54.v[2] + invt * ped_cache->field_0x60.v[2]);
        SetCharacterDirectionAR(pPed->character, &dir, &pPed->character->field_0xcc);
    } else {
        StopSmoothTurning(pPed);
        SetCharacterDirectionAR(pPed->character, &pPed->field_0x0c->field_0x48, &pPed->character->field_0xcc);
    }
}

void C2_HOOK_FASTCALL RandomWander(tPedestrian* pPed, tU32 pTime) {

    if (pPed->field_0x0c->next_turn_time != 0 && pTime >= pPed->field_0x0c->next_turn_time) {
        br_matrix34 mat;
        br_vector3 dir_old;
        br_vector3 dirxz_old;
        br_vector3 dirxz_new;
        tPed_cache_006944c0 state_old;

        SetNextRandomTurn(pPed, pTime);
        if (PercentageChance(50)) {
            BrMatrix34Rotate(&mat,  IRandomBetween(BR_ANGLE_DEG(10), pPed->movement_spec->max_random_angle), &C2V(y_unit_vector));
        } else {
            BrMatrix34Rotate(&mat, -IRandomBetween(BR_ANGLE_DEG(10), pPed->movement_spec->max_random_angle), &C2V(y_unit_vector));
        }
        BrVector3Set(&dirxz_old, pPed->character->field_0xc0.v[0], 0.f, pPed->character->field_0xc0.v[2]);
        BrMatrix34ApplyV(&dirxz_new, &dirxz_old, &mat);

        state_old = *pPed->field_0x0c;
        BrVector3Copy(&dir_old, &pPed->character->field_0xc0);

        SetPedXZDirection(pPed, &dirxz_new, 1.f, pTime);
        RescanPedProximity(pPed, &dirxz_new);
        if (pPed->field_0x0c->field_0x18) {
            *pPed->field_0x0c = state_old;
            SetPedXZDirection(pPed, &dir_old, 0.f, pTime);
        }
    }
}

void C2_HOOK_FASTCALL ResetScanDirection(tPedestrian* pPed) {
    BrVector3Set(&pPed->field_0x0c->field_0x0c, 0.f, 0.f, 0.f);
}

void C2_HOOK_FASTCALL CheckForAvoidingAction(tPedestrian* pPed, tU32 pTime) {
    br_vector3* ped_pos;
    float dist_squared;

    ped_pos = GetPedPos(pPed);
    dist_squared = Vector3DistanceSquared(ped_pos, &pPed->field_0x0c->field_0x3c);
    if (dist_squared >= C2V(gPed_min_dist_avoid_collisions_squared)) {
        ResetScanDirection(pPed);
    } else {
        float act_dist;

        if (dist_squared > C2V(gPed_reach_squared)) {
            act_dist = (sqrtf(dist_squared) - sqrtf(C2V(gPed_reach_squared))) * FRandomBetween(.7f, 1.f);
            act_dist = MIN(act_dist, .5f);
        } else {
            act_dist = .0f;
        }
        if (pPed->field_0x0c->field_0x18 == 1 && (C2V(gPeds_suicidal) || C2V(gBlind_pedestrians) || (act_dist == .0f && pPed->action == ePed_action_running && PercentageChance(100)))) {
            pPed->field_0x0c->field_0x18 = 0;
        } else if (!(C2V(gPeds_suicidal) && (pPed->field_0x0c->field_0x18 & 0x4))) {
            float first_ped_prox;
            int v_non_zero;
            br_vector3 new_dir;
            br_vector3 new_0xa4;
            br_vector3 tv;

            if (pPed->field_0x0c->field_0x18 == 4) {
                if (pTime < pPed->field_0x0c->field_0x90) {
                    return;
                }
                pPed->field_0x0c->field_0x90 = pTime + IRandomBetween(200, 500);
            }
            v_non_zero = !Vector3IsZero(&pPed->field_0x0c->field_0x30);

            if (dist_squared == .0f) {
                BrVector3Copy(&new_dir, &pPed->field_0x0c->field_0xa4);
            } else {

                if (fabsf(BrVector3Dot(&pPed->field_0x0c->field_0xa4, &pPed->character->field_0xc0)) > .985f) {
                    BrVector3Set(&tv, 0.f, (float)(PercentageChance(50) ? -1 : 1), 0.f);
                } else {
                    DRVector3SafeCross(&tv, &pPed->field_0x0c->field_0xa4, &pPed->character->field_0xc0);
                }
                DRVector3SafeCross(&new_dir, &tv, &pPed->field_0x0c->field_0xa4);
                if (v_non_zero && BrVector3Dot(&pPed->field_0x0c->field_0x30, &new_dir) < 0.f) {
                    BrVector3Negate(&tv, &tv);
                    DRVector3SafeCross(&new_dir, &tv, &pPed->field_0x0c->field_0xa4);
                }
            }
            SetPedXZDirection(pPed, &new_dir, act_dist, pTime);
            BrVector3Copy(&new_0xa4, &pPed->field_0x0c->field_0xa4);
            first_ped_prox = RescanPedProximity(pPed, &new_dir);
            if (pPed->field_0x0c->field_0x18 != 0 && dist_squared != 0.f) {
                tPed_cache_006944c0 prev_field_0x0c;
                br_vector3 other_dir;

                BrVector3Negate(&tv, &tv);
                DRVector3SafeCross(&other_dir, &tv, &new_0xa4);
                SetPedXZDirection(pPed, &other_dir, act_dist, pTime);
                prev_field_0x0c = *pPed->field_0x0c;
                if (RescanPedProximity(pPed, &other_dir) <= first_ped_prox) {
                    *pPed->field_0x0c = prev_field_0x0c;
                    SetPedXZDirection(pPed, &new_dir, act_dist, pTime);
                    ResetScanDirection(pPed);
                }
            }
        }
    }
}

void C2_HOOK_FASTCALL SetRandomOmega(tCollision_info* pObject, float pMax) {

#ifdef REC2_FIX_BUGS
    BrVector3Set(&pObject->omega, SRandomPosNeg(1.f), SRandomPosNeg(1.f), SRandomPosNeg(1.f));
    BrVector3Normalise(&pObject->omega, &pObject->omega);
    BrVector3Scale(&pObject->omega, &pObject->omega, pMax);
#else
    BrVector3Set(&pObject->omega, SRandomPosNeg(1.f), SRandomPosNeg(1.f), SRandomPosNeg(1.f));
    BrVector3Scale(&pObject->omega, &pObject->omega, &pMax / BrVector3Length(&tv));
#endif
}

void C2_HOOK_FASTCALL CheckPowerupMoveSubstitution(tPedestrian* pPed, tU32 pTime) {
    int current_state;
    int next_state_idx;

    current_state = pPed->character->personality->form->moves[pPed->character->field_0x7].id;
    if (C2V(gDrunk_pedestrians)) {
        next_state_idx = 4;
    } else if (C2V(gDancing_peds)) {
        next_state_idx = 3;
    } else if (C2V(gPanicking_peds)) {
        next_state_idx = 5;
    } else {
        if (current_state == 112 || current_state == 31 || current_state == 111) {
            SetPedMove(pPed, 30, -1, 0, 0, pTime, -1);
        }
        return;
    }
    if (current_state != C2V(gPed_move_fsm)[current_state][next_state_idx + 1]) {
        SetPedMove(pPed, C2V(gPed_move_fsm)[current_state][next_state_idx + 1], -1, 0, 0, pTime, -1);
    }
}

void C2_HOOK_FASTCALL StillifyCorpse(tPedestrian* pPed, tU32 pTime, undefined4 pArg3) {

    if (C2V(gMutant_speed) == 0.f) {
        SetCharacterMoveAR(pPed->character, -1, 0.f, pArg3, 0, pTime);
    } else {
        SetCharacterMoveAR(pPed->character, -1, -C2V(gMutant_speed), pArg3, 0, pTime);
    }
}

void C2_HOOK_FASTCALL MungePedHeadAnim(tPedestrian* pPed, tU32 pTime) {
    tPed_cache_006944c0* ped_field_0x0c;

    ped_field_0x0c = pPed->field_0x0c;
    if (ped_field_0x0c != NULL
            && ped_field_0x0c->field_0x1c != NULL
            && pTime > ped_field_0x0c->field_0x78 + 125) {

        ped_field_0x0c->field_0x1a += 1;
        if (ped_field_0x0c->field_0x1a >= ped_field_0x0c->field_0x1c[0]) {
            ped_field_0x0c->field_0x1a = 1;
        }
        if (ped_field_0x0c->field_0x1c[ped_field_0x0c->field_0x1a + 1] == 99) {
            ped_field_0x0c->field_0x1c = NULL;
        } else {
            ped_field_0x0c->field_0x78 = pTime;
            SetCharacterBoneModelAR(pPed->character,
                pPed->character->personality->form->index_head_bone,
                ped_field_0x0c->field_0x1c[ped_field_0x0c->field_0x1a + 1],
                pPed->hit_points <= 0);
        }
    }
}

void C2_HOOK_FASTCALL MakeEmBleed(tPedestrian* pPed, tU32 pTime) {
    static C2_HOOK_VARIABLE_IMPLEMENT(tU32, last_bleed_giblet_time, 0x006a0428);

    if (GET_PED_COLLISION_OBJECT(pPed)->last_special_volume != NULL && GET_PED_COLLISION_OBJECT(pPed)->last_special_volume->gravity_multiplier < 1.f) {
        return;
    }
    if (C2V(gGoreLevel) <= 0) {
        return;
    }
    if (C2V(gMutant_speed) != 0.f && pPed->hit_points <= 0 && pPed->action == ePed_action_dead && pTime - C2V(last_bleed_giblet_time) > 300 && PercentageChance(40)) {

        C2V(last_bleed_giblet_time) = pTime;
        DoGiblets(pPed, GET_PED_COLLISION_OBJECT(pPed), NULL, .06f, &pPed->pos, C2V(gGiblet_scrape_start));
    }
    if (pPed->character->field_0xc != 0 && pPed->field_0x0c != NULL && pTime - pPed->field_0x0c->field_0x88 > pPed->field_0x0c->field_0x84) {
        int i;

        if (pPed->hit_points <= 0) {
            pPed->field_0x0c->field_0x84 = (tU32)((float)pPed->field_0x0c->field_0x84 * 1.2f);
            if (pPed->field_0x0c->field_0x84 > 1000) {
                pPed->field_0x0c->field_0x84 = 0;
            }
        }
        for (i = 1; i < pPed->character->personality->form->count_bones; i++) {
            if ((pPed->character->field_0xc & C2V(gPow2_array)[i]) && !(pPed->character->field_0x10 & C2V(gPow2_array)[i])) {
                tCollision_info* object;
                br_vector3* bone_v;
                int other_bone_index;

                object = pPed->character->personality->form->boned_physicing[pPed->character->field_0x6].collision_infos[i];
                if (pPed->field_0x0c->field_0x23[i] < C2V(gPed_spurty_lumps_count) && BrVector3Length(&object->v) == 0.f) {
                    pPed->field_0x0c->field_0x23[i] = 0;
                } else {
                    br_matrix34* bone_matrix;
                    br_vector3 bone_look;

                    if (pPed->field_0x0c->field_0x23[i] != 0) {
                        pPed->field_0x0c->field_0x23[i] -= C2V(gPed_spurty_lumps_count);
                    }

                    bone_matrix = GetBoneMatrixPtr(pPed->character, i);
                    BrVector3Copy(&bone_look, (br_vector3*)bone_matrix->m[0]);
                    DoSpurt(bone_matrix, &pPed->character->personality->bones[i].field_0x20, &bone_look, &object->v);
                }
                other_bone_index = pPed->character->personality->form->bones[0].indices[i];
                if (pPed->character->field_0xc & C2V(gPow2_array)[other_bone_index]) {
                    bone_v = &pPed->character->personality->form[0].boned_physicing[pPed->character->field_0x6].collision_infos[other_bone_index]->v;
                    if (pPed->field_0x0c->field_0x23[i] == 0) {
                        continue;
                    }
                } else {
                    bone_v = &pPed->character->field_0xd8;
                }
                if (BrVector3LengthSquared(bone_v) != 0.f || pPed->field_0x0c->field_0x84 != 0) {
                    br_matrix34* other_bone_matrix;
                    br_vector3 other_bone_look;

                    other_bone_matrix = GetBoneMatrixPtr(pPed->character, other_bone_index);
                    if (other_bone_index!= 0) {
                        BrVector3Copy(&other_bone_look, (br_vector3*)other_bone_matrix->m[0]);
                    } else {
                        BrMatrix34ApplyV(&other_bone_look, &C2V(gPed_bone_look_vecs)[i - 1], other_bone_matrix);
                    }
                    DoSpurt(other_bone_matrix, &pPed->character->personality->bones[i].field_0x2c, &other_bone_look, bone_v);
                }
            }
        }
    }
}

void C2_HOOK_FASTCALL SetPedHeadAnim(tPedestrian *pPed, undefined* pArg2, tU32 pTime) {

    if (pPed->hit_points > 0 && pPed->field_0x0c != NULL) {
        pPed->field_0x0c->field_0x1c = pArg2;
        pPed->field_0x0c->field_0x78 = pTime;
        pPed->field_0x0c->field_0x1a = 0;
        SetCharacterBoneModelAR (pPed->character, pPed->character->personality->form->index_head_bone, pPed->field_0x0c->field_0x1c[1], 0);
    }
}

void C2_HOOK_FASTCALL ScarePedestrian(tPedestrian* pPed, tU32 pTime, int pArg3, int pArg4) {

    pPed->action = pPed->action;
    if (((pPed->action != ePed_action_running && pPed->action != ePed_action_panicking) || pArg4) && pPed->action != ePed_action_getting_up) {
        if (pArg3) {
            SetPedMove(pPed, C2V(gPed_move_fsm)[114][IRandomBetween(0, 7)], -1, 0, 1, pTime, ePed_action_panicking);
            MakePedNoise(pPed, 6, 0, NULL);
            SetPedHeadAnim(pPed, C2V(gPed_scare_head_anim_byte_array), pTime);
        } else {
            StartPedRunning(pPed, pTime, 0);
        }
    }
    pPed->field_0x28 = pTime;
}

void (C2_HOOK_FASTCALL * MungePedestrians_original)(void);
void C2_HOOK_FASTCALL MungePedestrians(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    MungePedestrians_original();
#else
    tU32 the_time;
    unsigned int count_scared_pedestrians;
    tPedestrian* scared_pedestrians[10];
    int scare_now;
    int i;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_cache_006944c0, field_0x98, 0x98);
    C2_HOOK_BUG_ON(sizeof(C2V(gPed_cache_00694328)) != 0x12c);

    the_time = GetTotalTime();

    if (C2V(gPed_valium_left) != 0) {
        C2V(gPed_valium_left) -= C2V(gFrame_period);
        if (C2V(gPed_valium_left) < 0) {
            C2V(gPed_valium_left) = 0;
        }
    }
    C2V(gCount_active_peds) = 0;
    C2V(gPedestrians_in_sight) = NULL;
    C2V(gPeds_already_munged) = GetTotalTime() == C2V(gPed_last_munging);
    C2V(gPed_last_munging) = GetTotalTime();
    count_scared_pedestrians = 0;
    if (the_time > C2V(gLast_scare_time) + 300 && !C2V(gBlind_pedestrians) && C2V(gPed_valium_left) == 0) {
        scare_now = 1;
        C2V(gLast_scare_time) = the_time;
    } else {
        scare_now = 0;
    }
    if (!C2V(gAction_replay_mode)) {
        for (i = 0; i < C2V(gPed_count); i++) {
            tPedestrian* ped;
            br_vector3 ped_distance;

            ped = &C2V(gPedestrian_array)[i];
            BrVector3Sub(&ped_distance, (br_vector3*)C2V(gCamera_to_world).m[3], &ped->pos);
            if ((((ped->flags & 0x80)
                        || fabsf(ped_distance.v[0]) >= C2V(gPed_process_distance)
                        || fabsf(ped_distance.v[1]) >= C2V(gPed_process_distance_inner)
                        || fabsf(ped_distance.v[2]) >= C2V(gPed_process_distance)) && (ped->field_0x0c == NULL || ped->field_0x0c->field_0x8c == 0 || the_time - ped->field_0x0c->field_0x8c >= 7500))
                    || (!(ped->flags & 0x1)
                        && !C2V(gPed_nearness)
                        && fabsf(ped_distance.v[0]) <= C2V(gPed_popup_distance)
                        && fabsf(ped_distance.v[1]) <= C2V(gPed_popup_distance_inner)
                        && fabsf(ped_distance.v[2]) <= C2V(gPed_popup_distance))) {
                ped->flags &= ~0x1;
            } else {
                ped->flags |= 0x1;
                if (scare_now && ped->action == ePed_action_walking && count_scared_pedestrians < REC2_ASIZE(scared_pedestrians)) {
                    scared_pedestrians[count_scared_pedestrians] = ped;
                    count_scared_pedestrians += 1;
                }
            }
        }
    }
    for (i = 0; i < C2V(gPed_count); i++) {
        tPedestrian* ped = &C2V(gPedestrian_array)[i];
        br_vector3* ped_pos;

        if (!(ped->flags & 0x1)) {
            tPed_character_instance *character;

            character = ped->character;
            if (character->field_0x4 >= 0 && !C2V(gAction_replay_mode)) {
                undefined4 prev_field_0x10;
                PipeSinglePedStatus(ped,
                    1, 0, 1, 0,
                    character->field_0x4, 255,
                    character->field_0x5, 255,
                    &character->field_0xc0,
                    &character->field_0xcc,
                    &character->field_0x8c);
                if (character->field_0xbc != NULL) {
                    StopCharacterMorphing(character);
                }
                if (character->field_0x14 & 0x4) {
                    PedFallingForever(ped);
                }
                prev_field_0x10 = ped->character->field_0x10;
                SetCharacterPhysicsLevel(ped->character, 0);
                CharacterNoLongerRenderable(ped->character);
                if (ped->character->field_0x10 != prev_field_0x10) {
                    PipeSingleVanishedDismembered(ped, prev_field_0x10, ped->character->field_0x10);
                }
                if (ped->field_0x0c != NULL && ped->field_0x0c->field_0x98 != 0) {
                    if (ped->field_0x0c->field_0x98 >= 0) {
                        if (ped->hit_points > 0) {
                            ScoreForKilledPedestrian(ped REC2_THISCALL_EDX, 0.f);
                        }
                        StopObjectSmokingInstantly(ped->character->personality->form->simple_physicing[ped->character->field_0x5].collision_info);
                        ped->field_0x0c->field_0x98 = 0;
                        MakePedVanish(ped);
                    } else {
                        KillNapalmBolt(&C2V(gNapalm_bolts)[-ped->field_0x0c->field_0x98 + 1]);
                    }
                }
            }
            continue;
        }
        if (ped->character->field_0x4 < 0 && !C2V(gAction_replay_mode)) {
            /* FIXME: should MakeCharacterRenderable return an enum? */
            if (MakeCharacterRenderable(ped->character) == 1) {
                ped->flags &= ~0x1;
                continue;
            }
            /* FIXME: does MakeCharacterCollideWorthy return a enum or count (use REC2_ASIZE)? */
            if (MakeCharacterCollideworthy(ped->character, 0, 0) == 21) {
                CharacterNoLongerRenderable(ped->character);
                ped->flags &= ~0x1;
                continue;
            }
            PipeSinglePedStatus(ped, 0, 1, 0, 1,
                255, ped->character->field_0x4,
                255, ped->character->field_0x5,
                &ped->character->field_0xc0,
                &ped->character->field_0xcc,
                &ped->character->field_0x8c);
        }
        ped->next = C2V(gPedestrians_in_sight);
        C2V(gPedestrians_in_sight) = ped;
        if (!C2V(gAction_replay_mode)) {
            if (ped->flags & 0x20) {
                ped->flags |= 0x40;
            } else {
                ped->flags |= 0x20;
            }
        }
        if (ped->speed_factor != C2V(gPedestrian_speed_factor) && ped->hit_points > 0) {
            SetCharacterMove(ped->character, -1, -1.f, 0, 0, 0);
            if (ped->speed_factor == 0.f) {
                EnableOverallMovement();
            }
            if (C2V(gPedestrian_speed_factor) != 0.f) {
                SetCharacterMove(ped->character, -1, (float)ped->character->field_0x1e / C2V(gPedestrian_speed_factor), 0, 0, 0);
            } else {
                DisableOverallMovement();
            }
            ped->speed_factor = C2V(gPedestrian_speed_factor);
        }
        C2V(gCount_active_peds) += 1;
        if (ped->field_0x0c == NULL || ped->field_0x0c == &C2V(gPed_cache_00694328)) {
            if (C2V(gAction_replay_mode)) {
                ped->field_0x0c = &C2V(gPed_cache_00694328);
            } else {
                tU32 oldest_time;
                int oldest_index;
                size_t j;

                oldest_time = the_time + 1;
                oldest_index = 0;

                for (j = 0; j < REC2_ASIZE(C2V(gPed_cache_006944c0)); j++) {
                    if (C2V(gPed_cache_006944c0)[i].field_0xb4 == NULL) {
                        ped->field_0x0c = &C2V(gPed_cache_006944c0)[j];
                    }
                    if (C2V(gPed_cache_006944c0)[i].field_0x74 < oldest_time) {
                        oldest_index = j;
                        oldest_time = C2V(gPed_cache_006944c0)[j].field_0x74;
                    }
                }
                if (j == REC2_ASIZE(C2V(gPed_cache_006944c0))) {
                    /* FIXME: should these 2 lines be reversed? */
                    ped->field_0x0c = &C2V(gPed_cache_006944c0)[oldest_index];
                    C2V(gPed_cache_006944c0)[oldest_index].field_0xb4->field_0x0c = NULL;
                }
            }
            InitProcessData(ped, the_time);
        }
        ped->field_0x0c->field_0xb4 = ped;
        ped->field_0x0c->field_0x74 = the_time;
        ped->field_0x0c->field_0xc4 &= ~0x2;

        ped_pos = GetPedPos(ped);
        if (!C2V(gAction_replay_mode)) {
            if (ped->character->field_0x14) {
                if (C2V(gPed_gravity_multiplier) != PHILGetObjectProperty(GET_PED_COLLISION_OBJECT(ped), 0)) {
                    PHILSetObjectProperty(GET_PED_COLLISION_OBJECT(ped), 0, C2V(gPed_gravity_multiplier));
                    if (ped->character->field_0x6 >= 0) {
                        int j;

                        for (j = 0; j < ped->character->personality->form->count_bones; j++) {
                            PHILSetObjectProperty(ped->character->personality->form->boned_physicing[ped->character->field_0x6].collision_infos[j], 0, C2V(gPed_gravity_multiplier));
                        }
                    }
                }
            }
            if (!C2V(gAction_replay_mode)) {
                if (GET_PED_COLLISION_OBJECT(ped)->last_special_volume != NULL && GET_PED_COLLISION_OBJECT(ped)->last_special_volume->gravity_multiplier < 1.f) {
                    if (ped->field_0x0c->field_0x98 < 0) {
                        KillNapalmBolt(&C2V(gNapalm_bolts)[1 - ped->field_0x0c->field_0x98]);
                    } else if (ped->field_0x0c->field_0x98 > 0) {
                        StopObjectSmokingInstantly(ped->character->personality->form->simple_physicing[ped->character->field_0x5].collision_info);
                        ped->field_0x0c->field_0x98 = 0;
                    }
                    if (ped->field_0x0c->field_0x94 == 0) {
                        ped->field_0x0c->field_0x94 = the_time;
                    }
                    if (ped->hit_points > 0) {
                        if (ped->character->personality->form->moves[ped->character->field_0x7].id != 110) {
                            SetPedMove(ped, 110, -1, 0, 0, the_time, ePed_action_impacting);
                        }
                        if (the_time - ped->field_0x0c->field_0x94 > 30000) {
                            KillPedestrian(ped, NULL);
                        }
                    } else {
                        if (ped->character->personality->form->moves[ped->character->field_0x7].id != 94) {
                            SetPedMove(ped, 94, -1, 0, 0, the_time, ePed_action_impacting);
                        }
                    }
                } else {
                    ped->field_0x0c->field_0x94 = 0;
                }
                if (ped->field_0x0c->field_0x98 > 0 && ped->hit_points > 0) {
                    if (the_time >= (tU32)ped->field_0x0c->field_0x98 && !C2V(gImmortal_peds)) {
                        KillPedestrian(ped, NULL);
                        DoPostElectricution(ped, the_time, .0f, .5f);
                        ped->field_0x0c->field_0x98 = 0;
                    } else {
                        MakePedNoise(ped, 6, 0, NULL);
                    }
                }
            }
        }

        if (!(ped->character->field_0x14 & 0x4)
                && (ped->action == ePed_action_walking || ped->action == ePed_action_running)
                && (!C2V(gAction_replay_mode) || C2V(gBOOL_00744804))) {
            int j;
            int nearby_changed_point;
            tPed_face_cache_0x34* face_cache;
            int cache_bool1;
            int cache_bool2;
            br_vector3 cache_pos1;

            AcceleratePed(ped, the_time);
            ped->flags &= ~0x6;
            nearby_changed_point = 0;
            for (j = 0; j < C2V(gCount_changed_points); j++) {
                if (Vector3DistanceSquared(&C2V(gChanged_points)[j], &ped->pos) < 400.f) {
                    nearby_changed_point = 1;
                    break;
                }
            }
            face_cache = RecacheAndSetFace(ped, &cache_bool1, &cache_bool2, &cache_pos1, C2V(gBOOL_00744804) | nearby_changed_point, the_time);
            if (face_cache == NULL) {
                face_cache = RecacheAndSetFace(ped, &cache_bool1, &cache_bool2, &cache_pos1, C2V(gBOOL_00744804) | nearby_changed_point, the_time);
            }
            if (cache_bool1 && ped->action != ePed_action_falling && ped->action != ePed_action_dead && ped->action != ePed_action_impacting) {
                if (ped->action == ePed_action_falling || (face_cache->field_0x30 != NULL && ped_pos->v[1] - cache_pos1.v[1] - GetClearanceFromCharacterInstance(ped->character, 0) <= ped->character->personality->jump_height + .4347826f && C2V(gPed_cos_max_slope) < face_cache->field_0x30->normal.v[1])) {
                    SetCharacterPositionAR(ped->character, &cache_pos1, 1);
                    if (ped->field_0x0c->field_0x6c) {
                        StopSmoothTurning(ped);
                        SetPedXZDirection(ped, &ped->field_0x0c->field_0x54, 0.f, the_time);
                    } else {
                        SetPedXZDirection(ped, &ped->character->field_0xc0, 0.f, the_time);
                    }
                    cache_bool2 = 1;
                } else {
                    if (!(ped->flags & 0x40)) {
                        MakePedVanish(ped);
                        continue;
                    }
                    face_cache->field_0x30 = NULL;
                    dr_dprintf("!!!!!! %0x turned on physics due to falling (%s)", (unsigned)(uintptr_t)ped, ped->character->personality->name);
                    SetThisPedPhysicing(ped);
                    SetPedMove(ped, 72, -1, 0, 0, the_time, ePed_action_falling);
                    ped->action = ePed_action_falling;
                }
            }


            if (!(ped->character->field_0x14 & 0x4) && ped->hit_points > 0) {
                if (C2V(gBOOL_00744804)) {
                    cache_bool2 = 1;
                }
                if (!cache_bool2) {
                    cache_bool2 = BrVector3Dot(&ped->character->field_0xc0, &ped->field_0x0c->field_0x0c) < .9998f;
                }
                if (cache_bool2 && face_cache->field_0x30 != NULL && !ped->field_0x0c->field_0x6c) {
                    RescanPedProximity(ped, NULL);
                }
                if (face_cache->field_0x30 != NULL && PedAnimCausesMovement(ped)) {
                    if (ped->field_0x0c->field_0x6c) {
                        SmoothTurnPedestrian(ped, the_time);
                        RescanPedProximity(ped, NULL);
                    } else {
                        RandomWander(ped, the_time);
                    }
                    PedScanForObjects(ped, the_time);
                    if (ped->field_0x0c->field_0x18) {
                        CheckForAvoidingAction(ped, the_time);
                    }
                }
            }
        }
        if (!C2V(gAction_replay_mode)) {
            if (!(ped->character->field_0x14 & 0x4)
                    && GET_PED_COLLISION_OBJECT(ped)->last_special_volume != NULL
                    && GET_PED_COLLISION_OBJECT(ped)->last_special_volume->gravity_multiplier < 1.f
                    && C2V(gGravity_multiplier) == 1.f
                    && GET_PED_COLLISION_OBJECT(ped)->water_depth_factor == 1.f) {
                SetThisPedPhysicing(ped);
                SetRandomOmega(GET_PED_COLLISION_OBJECT(ped), 2.f);
            }
        }
        CheckPowerupMoveSubstitution(ped, the_time);
        MungeCharacterAnimation(ped->character, the_time);
        if (!(ped->character->field_0x14 & 0x4)) {
            tCollision_info* simple_coll_info = ped->character->personality->form->simple_physicing[ped->character->field_0x5].collision_info;
            if (simple_coll_info != NULL) {
                BrVector3Copy(&simple_coll_info->v, &ped->character->field_0xd8);
                if (ped->hit_points <= 0
                        && ped->action == ePed_action_dead && ped->character->personality->form->moves[ped->character->field_0x7].id != 113
                        && ((C2V(gMutant_speed) != 0.f && ped->character->field_0x1e == 0)
                                || (C2V(gMutant_speed) == .0f && ped->character->field_0x1e != 0))) {
                    /* FIXME: 0 in Windows, 6 in mac version */
                    StillifyCorpse(ped, the_time, 0);
                }
            }
        }
        BrVector3Copy(&ped->pos, ped_pos);
        if (!C2V(gAction_replay_mode)) {
            MungePedHeadAnim(ped, the_time);
            MakeEmBleed(ped, the_time);
            if (scare_now && (ped->action == ePed_action_running || ped->hit_points <= 0 || ped->action == ePed_action_impacting)) {
                unsigned int j;

                for (j = 0; j < count_scared_pedestrians; j++) {
                    if (scared_pedestrians[j]->action == ePed_action_walking && Vector3DistanceSquared(ped_pos, &scared_pedestrians[j]->pos) < REC2_SQR(3.f)) {
                        ScarePedestrian(ped, the_time, 1, 0);
                    }
                }
            }
        }
    }

    C2V(gPed_nearness) = 0;
    C2V(gCount_changed_points) = 0;
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
                    if (pForm->bones[k].indices[0] == -1) {
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

            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_form_bone, indices, 0x28);

            /* Collision detection type */
            collision_detection_type = GetALineAndInterpretCommand(f, C2V(gPed_form_collision_type_names), REC2_ASIZE(C2V(gPed_form_collision_type_names)));
            joint_type = GetALineAndInterpretCommand(f, C2V(gPed_form_bone_joint_type_names), REC2_ASIZE(C2V(gPed_form_bone_joint_type_names)));
            joint = NULL;
            if (joint_type < 0 || joint_type == ePed_form_bone_hinge_none) {
                bone->indices[0] = -1;
            } else {
                /* Index of parent */
                bone->indices[0] = GetAnInt(f);
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
                            RemapVector(&limit->parent, form->bones[bone->indices[0]].remapped_bone);

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
                        RemapVector(&joint->parent_bone_axis, form->bones[bone->indices[0]].remapped_bone);
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

        if (personality->form->bones[bone_index].indices[0] >= 0) {
            GetAVector(f, &personality->bones[bone_index].field_0x20);
            RemapVector(&personality->bones[bone_index].field_0x20, personality->form->bones[bone_index].remapped_bone);
            GetAVector(f, &personality->bones[bone_index].field_0x2c);
            RemapVector(&personality->bones[bone_index].field_0x2c, personality->form->bones[personality->form->bones[bone_index].indices[0]].remapped_bone);
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

tPed_character_instance* C2_HOOK_FASTCALL GetTestPed(void) {

    if (C2V(gPed_count) == 0) {
        return NULL;
    }
    return C2V(gPedestrian_array)[C2V(gSelected_ped)].character;
}
C2_HOOK_FUNCTION(0x004d6210, GetTestPed)

undefined4 (C2_HOOK_FASTCALL * MakeCharacterRenderable2_original)(tPed_character_instance* pCharacter, int pIndex);
undefined4 C2_HOOK_FASTCALL MakeCharacterRenderable2(tPed_character_instance* pCharacter, int pIndex) {

#if defined(C2_HOOKS_ENABLED)
    return MakeCharacterRenderable2_original(pCharacter, pIndex);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00408200, MakeCharacterRenderable2, MakeCharacterRenderable2_original)

undefined4 C2_HOOK_FASTCALL MakeCharacterRenderable(tPed_character_instance* pCharacter) {
    int i;

    if (pCharacter->field_0x4 >= 0) {
        return 2;
    }
    for (i = 0; i < pCharacter->personality->form->max_rendering_at_once; i++) {
        if (!pCharacter->personality->form->actor_sets[i].field_0x0) {
            break;
        }
    }
    if (i == pCharacter->personality->form->max_rendering_at_once) {
        return 1;
    }
    return MakeCharacterRenderable2(pCharacter, i);
}
C2_HOOK_FUNCTION(0x004083b0, MakeCharacterRenderable)

int (C2_HOOK_FASTCALL * MakeCharacterCollideworthy2_original)(tPed_character_instance* pCharacter, int pArg2, undefined4 pArg3, int pArg4);
int C2_HOOK_FASTCALL MakeCharacterCollideworthy2(tPed_character_instance* pCharacter, int pArg2, undefined4 pArg3, int pArg4) {

#if defined(C2_HOOKS_ENABLED)
    return MakeCharacterCollideworthy2_original(pCharacter, pArg2, pArg3, pArg4);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00408c30, MakeCharacterCollideworthy2, MakeCharacterCollideworthy2_original)

int C2_HOOK_FASTCALL MakeCharacterCollideworthy(tPed_character_instance *pCharacter, int pArg2, undefined4 pArg3) {

    return MakeCharacterCollideworthy2(pCharacter, pArg2, pArg3, -1);
}
C2_HOOK_FUNCTION(0x00409030, MakeCharacterCollideworthy)

void C2_HOOK_FASTCALL DisableOverallMovement(void) {

    C2V(gPed_overall_movement_disabled) = 1;
}
C2_HOOK_FUNCTION(0x0040c600, DisableOverallMovement)

void C2_HOOK_FASTCALL EnableOverallMovement(void) {

    C2V(gPed_overall_movement_disabled) = 0;
}
C2_HOOK_FUNCTION(0x0040c610, EnableOverallMovement)

br_vector3* C2_HOOK_FASTCALL GetPedPos(tPedestrian* pPed) {

    return (br_vector3*)GetCharacterMatrixPtr(pPed->character)->m[3];
}
C2_HOOK_FUNCTION(0x004cadb0, GetPedPos)

void (C2_HOOK_FASTCALL * KillPedestrian_original)(tPedestrian* pPed, tCollision_info* pCollision_info);
void C2_HOOK_FASTCALL KillPedestrian(tPedestrian* pPed, tCollision_info* pCollision_info) {

#if defined(C2_HOOKS_ENABLED)
    KillPedestrian_original(pPed, pCollision_info);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004cce70, KillPedestrian, KillPedestrian_original)

void (C2_HOOK_FASTCALL * DoPostElectricution_original)(tPedestrian* pPed, tU32 pTime, float pF1, float pF2);
void C2_HOOK_FASTCALL DoPostElectricution(tPedestrian* pPed, tU32 pTime, float pF1, float pF2) {

#if defined(C2_HOOKS_ENABLED)
    DoPostElectricution_original(pPed, pTime, pF1, pF2);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004cc6e0, DoPostElectricution, DoPostElectricution_original)

void (C2_HOOK_FASTCALL * MakePedNoise_original)(tPedestrian* pPed, int pArg2, int pArg3, tCollision_info* pCollision_info);
void C2_HOOK_FASTCALL MakePedNoise(tPedestrian* pPed, int pArg2, int pArg3, tCollision_info* pCollision_info) {

#if defined(C2_HOOKS_ENABLED)
    MakePedNoise_original(pPed, pArg2, pArg3, pCollision_info);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004cd880, MakePedNoise, MakePedNoise_original)

void (C2_HOOK_FASTCALL * SetCharacterMoveAR_original)(tPed_character_instance* pCharacter, int pMove_action, float pSpeed, undefined4 pArg4, undefined4 pArg5, tU32 pTime);
void C2_HOOK_FASTCALL SetCharacterMoveAR(tPed_character_instance* pCharacter, int pMove_action, float pSpeed, undefined4 pArg4, undefined4 pArg5, tU32 pTime) {

#if defined(C2_HOOKS_ENABLED)
    SetCharacterMoveAR_original(pCharacter, pMove_action, pSpeed, pArg4, pArg5, pTime);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004d1c60, SetCharacterMoveAR, SetCharacterMoveAR_original)

tPed_face_cache_0x34* (C2_HOOK_FASTCALL * RecacheAndSetFace_original)(tPedestrian* pPed, int* pArg2, int* pArg3, br_vector3* pArg4, int pArg5, tU32 pTime);
tPed_face_cache_0x34* C2_HOOK_FASTCALL RecacheAndSetFace(tPedestrian* pPed, int* pArg2, int* pArg3, br_vector3* pArg4, int pArg5, tU32 pTime) {

#if defined(C2_HOOKS_ENABLED)
    return RecacheAndSetFace_original(pPed, pArg2, pArg3, pArg4, pArg5, pTime);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004d1d70, RecacheAndSetFace, RecacheAndSetFace_original)

float C2_HOOK_FASTCALL GetClearanceFromCharacterInstance(tPed_character_instance* pCharacter, int pMoveNum) {

    if (pMoveNum < 0 || pMoveNum > pCharacter->personality->form->count_moves) {
        BrFailure("MoveNum out of range in GetClearance()");
    }
    return pCharacter->personality->moves[pMoveNum].grounding_offset;
}
C2_HOOK_FUNCTION(0x0040bae0, GetClearanceFromCharacterInstance)

void (C2_HOOK_FASTCALL * SetCharacterPosition_original)(tPed_character_instance* pCharacter, br_vector3* pPos, int pArg3);
void C2_HOOK_FASTCALL SetCharacterPosition(tPed_character_instance* pCharacter, br_vector3* pPos, int pArg3) {

#if defined(C2_HOOKS_ENABLED)
    SetCharacterPosition_original(pCharacter, pPos, pArg3);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0040b0a0, SetCharacterPosition, SetCharacterPosition_original)

void (C2_HOOK_FASTCALL * SetPedXZDirection_original)(tPedestrian* pPed, br_vector3* pDir, float pArg3, tU32 pTime);
void C2_HOOK_FASTCALL SetPedXZDirection(tPedestrian* pPed, br_vector3* pDir, float pArg3, tU32 pTime) {

#if defined(C2_HOOKS_ENABLED)
    SetPedXZDirection_original(pPed, pDir, pArg3, pTime);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004cc2c0, SetPedXZDirection, SetPedXZDirection_original)

float (C2_HOOK_FASTCALL * RescanPedProximity_original)(tPedestrian* pPed, br_vector3* pArg2);
float C2_HOOK_FASTCALL RescanPedProximity(tPedestrian* pPed, br_vector3* pArg2) {

#if defined(C2_HOOKS_ENABLED)
    return RescanPedProximity_original(pPed, pArg2);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004cfab0, RescanPedProximity, RescanPedProximity_original)

void (C2_HOOK_FASTCALL * PedScanForObjects_original)(tPedestrian* pPed, tU32 pTime);
void C2_HOOK_FASTCALL PedScanForObjects(tPedestrian* pPed, tU32 pTime) {

#if defined(C2_HOOKS_ENABLED)
    PedScanForObjects_original(pPed, pTime);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004cef80, PedScanForObjects, PedScanForObjects_original)

void (C2_HOOK_FASTCALL * MungeCharacterAnimation_original)(tPed_character_instance* pCharacter, tU32 pTime);
void C2_HOOK_FASTCALL MungeCharacterAnimation(tPed_character_instance* pCharacter, tU32 pTime) {

#if defined(C2_HOOKS_ENABLED)
    MungeCharacterAnimation_original(pCharacter, pTime);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00409ca0, MungeCharacterAnimation, MungeCharacterAnimation_original)

void C2_HOOK_FASTCALL SetCharacterBoneModelAR(tPed_character_instance* pCharacter, int pArg2, undefined4 pArg3, int pArg4) {

    if (pCharacter->field_0x4 >= 0) {
        if (pArg2 >= 0) {
            PipeSinglePedModelChange(pCharacter->ped,
                pArg2,
                GetCharacterBoneModel(pCharacter, pArg2),
                pArg3,
                GetCharacterModelSet(pCharacter),
                pArg4);
            SetCharacterBoneModel(pCharacter, pArg2, pArg3, pArg4);
        } else {
            PipeSinglePedModelChange(pCharacter->ped,
                pArg2,
                0,
                -1,
                GetCharacterModelSet(pCharacter),
                pArg4);
            SetCharacterAllBonesModel(pCharacter, pArg3, pArg4);
        }
    }
}
C2_HOOK_FUNCTION(0x004cfa30, SetCharacterBoneModelAR)

br_matrix34* C2_HOOK_FASTCALL GetBoneMatrixPtr(tPed_character_instance* pCharacter, int pBone_index) {
    if (pCharacter->field_0x4 < 0) {
        return &pCharacter->field_0x2c;
    }
    if (pCharacter->field_0xe8 != NULL) {
        return &pCharacter->field_0xe8[pBone_index];
    }
    return &pCharacter->personality->form->actor_sets[pCharacter->field_0x4].actors[pBone_index]->t.t.mat;
}
C2_HOOK_FUNCTION(0x00407ad0, GetBoneMatrixPtr)

void (C2_HOOK_FASTCALL * DoGiblets_original)(tPedestrian* pPed, tCollision_info* pObject, tCollision_info* pOpt_speed_object, float pArg4, br_vector3* pArg5, int pArg6);
void C2_HOOK_FASTCALL DoGiblets(tPedestrian* pPed, tCollision_info* pObject, tCollision_info* pOpt_speed_object, float pArg4, br_vector3* pArg5, int pArg6) {

#if defined(C2_HOOKS_ENABLED)
    DoGiblets_original(pPed, pObject, pOpt_speed_object, pArg4, pArg5, pArg6);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ccff0, DoGiblets, DoGiblets_original)

void (C2_HOOK_FASTCALL * DoSpurt_original)(br_matrix34* pArg1, br_vector3* pArg2, br_vector3* pArg3, br_vector3* pArg4);
void C2_HOOK_FASTCALL DoSpurt(br_matrix34* pArg1, br_vector3* pArg2, br_vector3* pArg3, br_vector3* pArg4) {

#if defined(C2_HOOKS_ENABLED)
    DoSpurt_original(pArg1, pArg2, pArg3, pArg4);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004d5820, DoSpurt, DoSpurt_original)

br_model* C2_HOOK_FASTCALL GetCharacterBoneModel(tPed_character_instance* pCharacter, int pBone_index) {

    return pCharacter->personality->form->actor_sets[pCharacter->field_0x4].actors[pBone_index]->model;
}
C2_HOOK_FUNCTION(0x0040b570, GetCharacterBoneModel)

int C2_HOOK_FASTCALL GetCharacterModelSet(tPed_character_instance* pCharacter) {

    return pCharacter->field_0xa;
}
C2_HOOK_FUNCTION(0x0040b590, GetCharacterModelSet)

int C2_HOOK_FASTCALL SetCharacterBoneModel(tPed_character_instance* pCharacter, int pArg2, int pArg3, int pArg4) {

    if (pArg3 >= 4
            || pArg2 >= pCharacter->personality->form->count_bones
            || pArg2 < 0
            || pCharacter->field_0x4 < 0) {
        return 0;
    }
    if (pArg3 < 0) {
        br_actor* actor;
        int i;

        actor = pCharacter->personality->form->actor_sets[pCharacter->field_0x4].actors[pArg2];
        for (i = 0; i < 4; i++) {
            if (pCharacter->personality->bones[pArg2].models[i].models[pCharacter->field_0xa] == actor->model) {
                break;
            }
        }
        if (i == 4) {
            i = 0;
        }
        actor->model = pCharacter->personality->bones[pArg2].models[i].models[pArg4];
        pCharacter->field_0xa = pArg4;
    } else {
        br_model* model;

        pCharacter->field_0xa = pArg4;
        model = pCharacter->personality->bones[pArg2].models[pArg3].models[pArg4];
        if (model == NULL) {
            return 0;
        }
        pCharacter->personality->form->actor_sets[pCharacter->field_0x4].actors[pArg2]->model = model;
    }
    return 1;
}
C2_HOOK_FUNCTION(0x0040b420, SetCharacterBoneModel)

void C2_HOOK_FASTCALL SetCharacterAllBonesModel(tPed_character_instance* pCharacter, int pArg2, int pArg3) {
    int i;

    for (i = 0; i < pCharacter->personality->form->count_bones; i++) {
        SetCharacterBoneModel(pCharacter, i, pArg2, pArg3);
    }
}
C2_HOOK_FUNCTION(0x0040b530, SetCharacterAllBonesModel)

void C2_HOOK_FASTCALL SetPedStartRun(tPedestrian* pPed) {

    if (pPed->field_0x0c != NULL) {
        pPed->field_0x0c->field_0x80 = 100;
        pPed->field_0x0c->field_0xb8 = .09f;
    }
}

void C2_HOOK_FASTCALL StartPedRunning(tPedestrian* pPed, tU32 pTime, int pArg3) {

    if (!(pPed->character->field_0xc & 0x8) && !(pPed->character->field_0xc & 0x20) && pPed->field_0x0c != NULL) {

        SetPedStartRun(pPed);
        SetPedMove(pPed, pArg3 ? 40 : 41, pPed->field_0x0c->field_0x80, 0, 0, pTime, ePed_action_running);
    } else {
        SetPedMove(pPed, pArg3 ? 40 : 41, -1, 0, 0, pTime, ePed_action_running);
    }
}
C2_HOOK_FUNCTION(0x004cc660, StartPedRunning)

br_actor* C2_HOOK_FASTCALL GetCharacterActorPtr(tPed_character_instance* pCharacter) {

    if (pCharacter->field_0x4 < 0) {
        return NULL;
    }
    return pCharacter->personality->form->actor_sets[pCharacter->field_0x4].actors[0];
}
C2_HOOK_FUNCTION(0x00407b10, GetCharacterActorPtr)

void C2_HOOK_FASTCALL RenderLimbs(void) {
    int i;
    int j;

    if (C2V(gLimbs_actor) == NULL) {
        C2V(gLimbs_actor) = BrActorAllocate(BR_ACTOR_NONE, NULL);
        C2V(gLimbs_actor)->identifier = "Limbs_actor";
        BrActorAdd(C2V(gUniverse_actor), C2V(gLimbs_actor));
    }
    for (i = 0; i < C2V(gPed_count_limbed_actors); i++) {
        br_actor* a = C2V(gPed_limbed_actors)[i];
        tPed_character_instance* ped = a->type_data;
        tPed_form* form = ped->personality->form;
        br_actor** bone_actors = form->actor_sets[ped->field_0x4].actors;
        for (j = 1; j < form->count_bones; j++) {
            if (!(C2V(gPow2_array)[j] & ped->field_0xc)) {
                BrActorAdd(C2V(gLimbs_actor), bone_actors[j]);
            }
        }
    }
    BrZbsSceneRenderAdd(C2V(gLimbs_actor));
    while (C2V(gLimbs_actor)->children != NULL) {
        BrActorRemove(C2V(gLimbs_actor)->children);
    }
    C2V(gPed_count_limbed_actors) = 0;
}
C2_HOOK_FUNCTION(0x004d3610, RenderLimbs)

void C2_HOOK_FASTCALL RenderLollipops(br_pixelmap* pRender_buffer, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world) {
    // Empty
}
C2_HOOK_FUNCTION(0x00445ca0, RenderLollipops)
