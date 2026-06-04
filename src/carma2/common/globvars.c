#include "globvars.h"


// GLOBAL: CARMA2_HW 0x0068b868
int gApplicationDataTwtMounted;

// GLOBAL: CARMA2_HW 0x0075ba60
tPath_name gApplication_path;

// GLOBAL: CARMA2_HW 0x00761a5c
const char* gDir_separator;


// GLOBAL: CARMA2_HW 0x0068b87c
int gAusterity_mode;

// GLOBAL: CARMA2_HW 0x0068b884
int gAustere_override;


// GLOBAL: CARMA2_HW 0x0068b8a4
br_pixelmap* gDepth_buffer;

// GLOBAL: CARMA2_HW 0x0074d3e0
br_pixelmap* gScreen;

// GLOBAL: CARMA2_HW 0x0068b888
int gSound_override;


// GLOBAL: CARMA2_HW 0x0074d360
br_pixelmap* gBack_screen;

// GLOBAL: CARMA2_HW 0x006ad47c
br_pixelmap* gDouble_back_screen;

// GLOBAL: CARMA2_HW 0x006ad480
int gHas_double_back_screen;


// GLOBAL: CARMA2_HW 0x00761f4c
br_scalar gCamera_yon;


// GLOBAL: CARMA2_HW 0x0075bb60
br_actor* gTrack_actor;


// GLOBAL: CARMA2_HW 0x0074d3e4
tU32 gFrame_period;


// GLOBAL: CARMA2_HW 0x0075b9c0
tBrender_storage gMisc_storage_space;


// GLOBAL: CARMA2_HW 0x0075ba54
br_font* gBig_font;

// GLOBAL: CARMA2_HW 0x0074d3c4
br_font* gFont_7;

// GLOBAL: CARMA2_HW 0x0074cf9c
br_font* gHeadup_font;


// GLOBAL: CARMA2_HW 0x0075bb80
tProgram_state gProgram_state;

// GLOBAL: CARMA2_HW 0x0068b880
int gSausage_override;

// GLOBAL: CARMA2_HW 0x0074d660
tRace_list_spec gRace_list[100];

// GLOBAL: CARMA2_HW 0x0074d4a0
int gNumber_of_races;


// GLOBAL: CARMA2_HW 0x0075b960
tBrender_storage gOur_car_storage_space;

// GLOBAL: CARMA2_HW 0x00761ca0
tBrender_storage gTheir_cars_storage_space;

// GLOBAL: CARMA2_HW 0x00762340
tBrender_storage gDroneStorage;

// GLOBAL: CARMA2_HW 0x0074d5a0
tBrender_storage gPedStorage;

// GLOBAL: CARMA2_HW 0x0074d400
tBrender_storage gTrack_storage_space;


// GLOBAL: CARMA2_HW 0x0068b89c
int gCD_is_in_drive;

// GLOBAL: CARMA2_HW 0x0076211c
int gDev_initial_race;

// GLOBAL: CARMA2_HW 0x0074d460
br_matrix34 gCamera_to_world;

// GLOBAL: CARMA2_HW 0x0074d560
br_matrix34 gRearview_camera_to_world;


// GLOBAL: CARMA2_HW 0x00705b80
undefined4 gUnknown_00705b80[20];


// GLOBAL: CARMA2_HW 0x0074d44c
br_actor* gUniverse_actor;

// GLOBAL: CARMA2_HW 0x007634b8
br_actor* gNon_track_actor;

// GLOBAL: CARMA2_HW 0x00761ce4
br_actor* gDont_render_actor;


// GLOBAL: CARMA2_HW 0x00655dc4
int gAmbient_sound = 1;

// GLOBAL: CARMA2_HW 0x0068b864
int gAuto_load;

// GLOBAL: CARMA2_HW 0x006aa5c4
int gRussels_fannies;


// GLOBAL: CARMA2_HW 0x00705220
char gQuick_time_quality[32];

// GLOBAL: CARMA2_HW 0x00705200
char gQuick_time_compressor[32];

// GLOBAL: CARMA2_HW 0x0065ef30
int gQuick_time_banner_number = 1;

// GLOBAL: CARMA2_HW 0x00705340
char gQuick_time_banner_texture_name[256];

// GLOBAL: CARMA2_HW 0x00705440
char gQuick_time_temp_path[256];

// GLOBAL: CARMA2_HW 0x00705240
char gQuick_time_movie_path_stub[256];


// GLOBAL: CARMA2_HW 0x007638a0
int gNet_last_game_type;

// GLOBAL: CARMA2_HW 0x0075b9a4
int gMap_view;


// GLOBAL: CARMA2_HW 0x0074cadc
int gScreen_lock_often;

// GLOBAL: CARMA2_HW 0x0074ca0c
int gShadow_workaround;

// GLOBAL: CARMA2_HW 0x0074cf38
int gEnable_texture_interpolation;

// GLOBAL: CARMA2_HW 0x0074cf20
int gEnable_texture_antialiasing;

// GLOBAL: CARMA2_HW 0x0074ca20
int gTexture_power_of_2;

// GLOBAL: CARMA2_HW 0x0074cab8
int gAdapt_sky_model_for_cockpit;

// GLOBAL: CARMA2_HW 0x0074ca10
int gNo_fog;

// GLOBAL: CARMA2_HW 0x0074c9f4
const char* gRenderer_fixup_basename;

// GLOBAL: CARMA2_HW 0x0074ca6c
const char* gRenderer_fixup_extension;

// GLOBAL: CARMA2_HW 0x0074cf8c
int gNo_render_indent;

// GLOBAL: CARMA2_HW 0x0074cf68
int gNo_2d_effects;

// GLOBAL: CARMA2_HW 0x0074ca54
int gEnable_perspective_maps;

// GLOBAL: CARMA2_HW 0x0074cf60
int gDevious_2d;


// GLOBAL: CARMA2_HW 0x0068c71c
int gNumber_of_actors;

// GLOBAL: CARMA2_HW 0x0074ac00
br_actor* gActor_array[100];

// GLOBAL: CARMA2_HW 0x0068c720
int gNumber_of_lights;

// GLOBAL: CARMA2_HW 0x006572cc
tLighting_data gLighting_data = {
    { 255, 255, 255 },
    .2f, .8f,
    .2f, .8f,
    .2f, .8f,
};

// GLOBAL: CARMA2_HW 0x0074b3e0
br_actor* gLight_array[50];


// GLOBAL: CARMA2_HW 0x0074ca00
br_actor* gHUD_root;

// GLOBAL: CARMA2_HW 0x0074cf74
br_actor* gHUD_camera;


// GLOBAL: CARMA2_HW 0x0074ca34
br_actor* gLine_actor;

// GLOBAL: CARMA2_HW 0x0074cac8
br_model* gLine_model;

// GLOBAL: CARMA2_HW 0x0074ca4c
br_material* gLine_material;


// GLOBAL: CARMA2_HW 0x0074cf30
br_model* gBlend_model;

// GLOBAL: CARMA2_HW 0x0074cf94
br_model* gBlend_model2;

// GLOBAL: CARMA2_HW 0x0074caac
br_actor* gBlend_actor;

// GLOBAL: CARMA2_HW 0x006a87f0
tBRender_smoke gBR_smoke_structs[35];


// GLOBAL: CARMA2_HW 0x00655dc0
int gNo_current_game = 1;

// GLOBAL: CARMA2_HW 0x0074b7d8
int gWaiting_for_unpause;

// GLOBAL: CARMA2_HW 0x0075b8f0
tU32 gWait_for_it;

// GLOBAL: CARMA2_HW 0x00655dd4
int gGame_to_load = -1;

// GLOBAL: CARMA2_HW 0x007623a0
tRace_info gCurrent_race;

// GLOBAL: CARMA2_HW 0x0068be3c
int gGame_initialized;

// GLOBAL: CARMA2_HW 0x0074d49c
int gNo_races_yet;

// GLOBAL: CARMA2_HW 0x0074b750
tNet_mode gNet_mode_of_last_game;

// GLOBAL: CARMA2_HW 0x0074d5e4
tRace_group_spec* gCurrent_race_group;

// GLOBAL: CARMA2_HW 0x00761a60
int gIs_boundary_race;

// GLOBAL: CARMA2_HW 0x0074d1ac
int gNumber_of_racers;

// GLOBAL: CARMA2_HW 0x007634bc
tOpponent* gOpponents;

// GLOBAL: CARMA2_HW 0x00761f50
int gCurrent_APO_levels[3];

// GLOBAL: CARMA2_HW 0x0074d490
int gCurrent_APO_potential_levels[3];

// GLOBAL: CARMA2_HW 0x0074b784
tNet_game_details* gCurrent_net_game;


// GLOBAL: CARMA2_HW 0x00660ce4
int gPratcam_on = 1;

// GLOBAL: CARMA2_HW 0x006aaa30
int gCockpit_on;


// GLOBAL: CARMA2_HW 0x006aaf44
int gTree_surgery_pass1_count;

// GLOBAL: CARMA2_HW 0x006aaf10
int gTree_surgery_pass2_count;

// GLOBAL: CARMA2_HW 0x006aaf18
tTree_surgery_pass1 gTree_surgery_pass1[5];

// GLOBAL: CARMA2_HW 0x006aaa60
tTree_surgery_pass2 gTree_surgery_pass2[25];


// GLOBAL: CARMA2_HW 0x0068b858
int gTyping;

// GLOBAL: CARMA2_HW 0x0074b588
int gCount_opponents;

// GLOBAL: CARMA2_HW 0x0076350c
br_actor* gPlayer_car_master_actor;

// GLOBAL: CARMA2_HW 0x00655de0
br_vector3 gInitial_position = {{1000.f, 1000.f, 1000.f}};

// GLOBAL: CARMA2_HW 0x0068b85c
int gArrow_mode;


// GLOBAL: CARMA2_HW 0x0074d040
int gRace_head_ups[22];

//int gInfo_on;

// GLOBAL: CARMA2_HW 0x0068b86c
int gHost_abandon_game;

// GLOBAL: CARMA2_HW 0x0068b870
int gRecover_car;

// GLOBAL: CARMA2_HW 0x0068b874
int gDisallow_abandon_race;
//int gPipe_reg_snap_before_new_game;
//int gPipe_reg_snap_before_our_car;
//int gPipe_reg_snap_after_our_car;

// GLOBAL: CARMA2_HW 0x00676914
int gAction_replay_mode = 0;
//int gDefault_engine_noise_index;
//int gAusterity_mode;
//int gAustere_override;

//int gKey_map_index;

// GLOBAL: CARMA2_HW 0x0068b890
int gInterface_within_race_mode;
//int gCut_scene_override;
//int gGame_to_load = -1;
//int gCD_is_in_drive;

// GLOBAL: CARMA2_HW 0x0068b8a8
br_pixelmap* gRearview_screen;
//br_scalar gCut_delay_1;
//br_scalar gCut_delay_2;
//br_scalar gCut_delay_3;
//br_scalar gCut_delay_4;

// GLOBAL: CARMA2_HW 0x00655ddc
br_scalar gYon_multiplier = 1.f;
//tSave_game* gPre_race_saved_game;
//VFILE* gDiagnostic_file;
//br_vector3 gDisabled_vector;

// GLOBAL: CARMA2_HW 0x0068b8e0
tU32 gI_am_cheating;
//char gNet_player_name[32];
//br_matrix34 gRearview_camera_to_world;
//tSpecial_volume gDefault_default_water_spec_vol;
//br_matrix34 gCamera_to_world;
//tRace_list_spec gRace_list[100];

// GLOBAL: CARMA2_HW 0x00762124
float gPinball_factor;

// GLOBAL: CARMA2_HW 0x007620f0
float gOpponent_speed_factor;
//float gCar_crush_min_fold;
//float gNet_recovery_cost[7];
//float gCar_crush_softness;

// GLOBAL: CARMA2_HW 0x007634b0
br_vector3* gOur_pos;
//float gCar_crush_max_fold;

// GLOBAL: CARMA2_HW 0x00761f00
tBrender_storage gNet_cars_storage_space;
//float gNet_softness[7];
//tS32 gJoystick_deadzone;
//float gCar_crush_limit_deviant;
//float gNet_offensive[7];
//float gCar_crush_split_chance;

// GLOBAL: CARMA2_HW 0x0074d1a8
float gGravity_multiplier;
//float gCar_crush_wibble;
//float gRepair_cost[3];

// GLOBAL: CARMA2_HW 0x0074d634
float gCop_speed_factor;

// GLOBAL: CARMA2_HW 0x0074d3bc
float gDefault_gravity;

// GLOBAL: CARMA2_HW 0x0074d350
tU32 gKeys_pressed;
//tSpecial_volume* gDefault_water_spec_vol;
//float gNet_repair_cost[7];

// GLOBAL: CARMA2_HW 0x0079efb4
tU32 gLast_replay_frame_time;
//tU32 gTag_start_time;
//tOpponent* gOpponents;
//tU32 gLast_credit_tick;
//tBrender_storage gTheir_cars_storage_space;

// GLOBAL: CARMA2_HW 0x0074d62c
tU32 gRace_finished;
//tU32 gFrame_start_time;
//tProgram_state gProgram_state;
//tU32 gCredit_period_single[3];

// GLOBAL: CARMA2_HW 0x00763460
tU32 gRace_start;
//tBrender_storage gTrack_storage_space;
//tU32 gCredit_period;
//tU32 gCredit_period_network[7];
//tBrender_storage gOur_car_storage_space;
//tU32 gFox_start_time;
//tBrender_storage gPedestrians_storage_space;
//tRace_info gCurrent_race;

// GLOBAL: CARMA2_HW 0x0068be28
br_actor* gSelf;

// GLOBAL: CARMA2_HW 0x00762128
br_pixelmap* gRender_screen;

// GLOBAL: CARMA2_HW 0x0075b940
br_actor* gRearview_camera;

// GLOBAL: CARMA2_HW 0x0074d3e8
br_scalar gCamera_hither;
//br_scalar gCamera_angle;
//br_pixelmap* gDigits_pix;
//br_pixelmap* gRender_shade_table;

// GLOBAL: CARMA2_HW 0x0075b93c
br_pixelmap* gRearview_depth_buffer;
//br_pixelmap* gWhole_screen;

// GLOBAL: CARMA2_HW 0x0068b8ac
br_pixelmap* gIcons_pix;

// GLOBAL: CARMA2_HW 0x0074d35c
br_actor* gCamera;

// GLOBAL: CARMA2_HW 0x0074d644
br_actor* gOther_selfs[4];
//br_actor* gArrow_actor;
//br_pixelmap* gIcons_pix_low_res;
//int gInitial_net_credits[5];
//float gCar_cred_value[3];
//int gNo_races_yet;

// GLOBAL: CARMA2_HW 0x0074d3d0
int gJump_start_fine[3];
//int gReal_graf_data_index;
//float gCar_time_value[3];
//int gTyping_slot;
//int gCunning_stunt_bonus[3];

// GLOBAL: CARMA2_HW 0x007634f0
tU32 gKnobbled_frame_period;
//int gWait_for_it;
//int gCheck_point_cash[3];
//int gPoints_per_second[3];

// GLOBAL: CARMA2_HW 0x00761edc
tU32 gMap_time;
//int gPed_time_value[3];

// GLOBAL: CARMA2_HW 0x0074d1a0
int gAbandon_game;

// GLOBAL: CARMA2_HW 0x0075b944
int gLap;

// GLOBAL: CARMA2_HW 0x00763510
int gShow_opponents;

// GLOBAL: CARMA2_HW 0x0075ba50
int gTotal_laps;

// GLOBAL: CARMA2_HW 0x0075b8f4
int gFree_repairs;

// GLOBAL: CARMA2_HW 0x0075bb64
int gFreeze_timer;
//int gRoll_over_creds[3];

// GLOBAL: CARMA2_HW 0x0074d09c
int gShow_peds_on_map;

// GLOBAL: CARMA2_HW 0x0074d444
int gOn_drugs;
//int gWasted_time[3];

// GLOBAL: CARMA2_HW 0x00761eec
int gCheckpoint;
//int gRoll_over_time[3];
//int gWasted_creds[3];

// GLOBAL: CARMA2_HW 0x00761ee8
tU32 gTimer;

// GLOBAL: CARMA2_HW 0x00761ce8
int gCheckpoint_count;

// GLOBAL: CARMA2_HW 0x00761f48
int gCountdown;

// GLOBAL: CARMA2_HW 0x00761ee0
int gFrame_rate;
//int gNumber_of_racers;

// GLOBAL: CARMA2_HW 0x00762328
int gSound_enabled;
//int gGraf_spec_index;

// GLOBAL: CARMA2_HW 0x00761ee4
int gSound_available;
//int gNumber_of_races;
//char gDir_separator[3] = "";


// GLOBAL: CARMA2_HW 0x0074ca18
tPixelFlags gPixelFlags;

// GLOBAL: CARMA2_HW 0x0074d358
int gNo_credits_APO_restore;


// GLOBAL: CARMA2_HW 0x0074d3dc
int gAFE;

// GLOBAL: CARMA2_HW 0x0068b894
int gNoCutscenes;
