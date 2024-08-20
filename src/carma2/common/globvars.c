#include "globvars.h"

C2_HOOK_VARIABLE_IMPLEMENT(int, gApplicationDataTwtMounted, 0x0068b868);
C2_HOOK_VARIABLE_IMPLEMENT(tPath_name, gApplication_path, 0x0075ba60);
C2_HOOK_VARIABLE_IMPLEMENT(const char*, gDir_separator, 0x00761a5c);

C2_HOOK_VARIABLE_IMPLEMENT(int, gAusterity_mode, 0x0068b87c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gAustere_override, 0x0068b884);

C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gDepth_buffer, 0x0068b8a4);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gScreen, 0x0074d3e0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gSound_override, 0x0068b888);

C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gBack_screen, 0x0074d360);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gDouble_back_screen, 0x006ad47c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gHas_double_back_screen, 0x006ad480);

C2_HOOK_VARIABLE_IMPLEMENT(br_scalar, gCamera_yon, 0x00761f4c);

C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gTrack_actor, 0x0075bb60);

C2_HOOK_VARIABLE_IMPLEMENT(tU32, gFrame_period, 0x0074d3e4);

C2_HOOK_VARIABLE_IMPLEMENT(tBrender_storage, gMisc_storage_space, 0x0075b9c0);

C2_HOOK_VARIABLE_IMPLEMENT(br_font*, gBig_font, 0x0075ba54);
C2_HOOK_VARIABLE_IMPLEMENT(br_font*, gFont_7, 0x0074d3c4);
C2_HOOK_VARIABLE_IMPLEMENT(br_font*, gHeadup_font, 0x0074cf9c);

C2_HOOK_VARIABLE_IMPLEMENT(tProgram_state, gProgram_state, 0x0075bb80);
C2_HOOK_VARIABLE_IMPLEMENT(int, gSausage_override, 0x0068b880);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tRace_list_spec, gRace_list, 100, 0x0074d660);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNumber_of_races, 0x0074d4a0);

C2_HOOK_VARIABLE_IMPLEMENT(tBrender_storage, gOur_car_storage_space, 0x0075b960);
C2_HOOK_VARIABLE_IMPLEMENT(tBrender_storage, gTheir_cars_storage_space, 0x00761ca0);
C2_HOOK_VARIABLE_IMPLEMENT(tBrender_storage, gDroneStorage, 0x00762340);
C2_HOOK_VARIABLE_IMPLEMENT(tBrender_storage, gPedStorage, 0x0074d5a0);
C2_HOOK_VARIABLE_IMPLEMENT(tBrender_storage, gTrack_storage_space, 0x0074d400);

C2_HOOK_VARIABLE_IMPLEMENT(int, gCD_is_in_drive, 0x0068b89c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDev_initial_race, 0x0076211c);
C2_HOOK_VARIABLE_IMPLEMENT(br_matrix34, gCamera_to_world, 0x0074d460);
C2_HOOK_VARIABLE_IMPLEMENT(br_matrix34, gRearview_camera_to_world, 0x0074d560);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(undefined4, gUnknown_00705b80, 20, 0x00705b80);

C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gUniverse_actor, 0x0074d44c);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gNon_track_actor, 0x007634b8);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gDont_render_actor, 0x00761ce4);

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gAmbient_sound, 0x00655dc4, 1);
C2_HOOK_VARIABLE_IMPLEMENT(int, gAuto_load, 0x0068b864);
C2_HOOK_VARIABLE_IMPLEMENT(int, gRussels_fannies, 0x006aa5c4);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gQuick_time_quality, 32, 0x00705220);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gQuick_time_compressor, 32, 0x00705200);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gQuick_time_banner_number, 0x0065ef30, 1);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gQuick_time_banner_texture_name, 32, 0x00705340);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gQuick_time_temp_path, 256, 0x00705440);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gQuick_time_movie_path_stub, 256, 0x00705240);

C2_HOOK_VARIABLE_IMPLEMENT(int, gNet_last_game_type, 0x007638a0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gMap_view, 0x0075b9a4);

C2_HOOK_VARIABLE_IMPLEMENT(int, gScreen_lock_often, 0x0074cadc);
C2_HOOK_VARIABLE_IMPLEMENT(int, gShadow_workaround, 0x0074ca0c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gEnable_texture_interpolation, 0x0074cf38);
C2_HOOK_VARIABLE_IMPLEMENT(int, gEnable_texture_antialiasing, 0x0074cf20);
C2_HOOK_VARIABLE_IMPLEMENT(int, gTexture_power_of_2, 0x0074ca20);
C2_HOOK_VARIABLE_IMPLEMENT(int, gAdapt_sky_model_for_cockpit, 0x0074cab8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNo_fog, 0x0074ca10);
C2_HOOK_VARIABLE_IMPLEMENT(const char*, gRenderer_fixup_basename, 0x0074c9f4);
C2_HOOK_VARIABLE_IMPLEMENT(const char*, gRenderer_fixup_extension, 0x0074ca6c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNo_render_indent, 0x0074cf8c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gTranslucent_smoke, 0x0074cf68);
C2_HOOK_VARIABLE_IMPLEMENT(int, gEnable_perspective_maps, 0x0074ca54);
C2_HOOK_VARIABLE_IMPLEMENT(int, gUse_actor_dimming, 0x0074cf60);

C2_HOOK_VARIABLE_IMPLEMENT(int, gNumber_of_actors, 0x0068c71c);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_actor*, gActor_array, 100, 0x0074ac00);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNumber_of_lights, 0x0068c720);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tLighting_data, gLighting_data, 0x006572cc, {
    { 255, 255, 255 },
    .2f, .8f,
    .2f, .8f,
    .2f, .8f,
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_actor*, gLight_array, 50, 0x0074b3e0);

C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gHUD_root, 0x0074ca00);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gHUD_camera, 0x0074cf74);

C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gLine_actor, 0x0074ca34);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gLine_model, 0x0074cac8);
C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gLine_material, 0x0074ca4c);

C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gBlend_model, 0x0074cf30);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gBlend_model2, 0x0074cf94);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gBlend_actor, 0x0074caac);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tSmoke_info, gSmoke_infos, 35, 0x006a87f0);

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gNo_current_game, 0x00655dc0, 1);
C2_HOOK_VARIABLE_IMPLEMENT(int, gWaiting_for_unpause, 0x0074b7d8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gWait_for_it, 0x0075b8f0);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gGame_to_load, 0x00655dd4, -1);
C2_HOOK_VARIABLE_IMPLEMENT(tRace_info, gCurrent_race, 0x007623a0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gGame_initialized, 0x0068be3c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNo_races_yet, 0x0074d49c);
C2_HOOK_VARIABLE_IMPLEMENT(tNet_mode, gNet_mode_of_last_game, 0x0074b750);
C2_HOOK_VARIABLE_IMPLEMENT(tRace_group_spec*, gCurrent_race_group, 0x0074d5e4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gIs_boundary_race, 0x00761a60);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNumber_of_racers, 0x0074d1ac);
C2_HOOK_VARIABLE_IMPLEMENT(tOpponent*, gOpponents, 0x007634bc);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gCurrent_APO_levels, 3, 0x00761f50);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gCurrent_APO_potential_levels, 3, 0x0074d490);
C2_HOOK_VARIABLE_IMPLEMENT(tNet_game_details*, gCurrent_net_game, 0x0074b784);

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gPratcam_on, 0x00660ce4, 1);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCockpit_on, 0x006aaa30);

C2_HOOK_VARIABLE_IMPLEMENT(int, gTree_surgery_pass1_count, 0x006aaf44);
C2_HOOK_VARIABLE_IMPLEMENT(int, gTree_surgery_pass2_count, 0x006aaf10);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tTree_surgery_pass1, gTree_surgery_pass1, 5, 0x006aaf18);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tTree_surgery_pass2, gTree_surgery_pass2, 25, 0x006aaa60);

C2_HOOK_VARIABLE_IMPLEMENT(int, gTyping, 0x0068b858);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_opponents, 0x0074b588);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gPlayer_car_master_actor, 0x0076350c);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_vector3, gInitial_position, 0x00655de0, {1000.f, 1000.f, 1000.f});
C2_HOOK_VARIABLE_IMPLEMENT(int, gArrow_mode, 0x0068b85c);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gRace_head_ups, 22, 0x0074d040);

//int gInfo_on;
C2_HOOK_VARIABLE_IMPLEMENT(int, gHost_abandon_game, 0x0068b86c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gRecover_car, 0x0068b870);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDisallow_abandon_race, 0x0068b874);
//int gPipe_reg_snap_before_new_game;
//int gPipe_reg_snap_before_our_car;
//int gPipe_reg_snap_after_our_car;
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gAction_replay_mode, 0x00676914, 0);
//int gDefault_engine_noise_index;
//int gAusterity_mode;
//int gAustere_override;

//int gKey_map_index;
C2_HOOK_VARIABLE_IMPLEMENT(int, gInterface_within_race_mode, 0x0068b890);
//int gCut_scene_override;
//int gGame_to_load = -1;
//int gCD_is_in_drive;
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gRearview_screen, 0x0068b8a8);
//br_scalar gCut_delay_1;
//br_scalar gCut_delay_2;
//br_scalar gCut_delay_3;
//br_scalar gCut_delay_4;
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_scalar, gYon_multiplier, 0x00655ddc, 1.f);
//tSave_game* gPre_race_saved_game;
//VFILE* gDiagnostic_file;
//br_vector3 gDisabled_vector;
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gI_am_cheating, 0x0068b8e0);
//char gNet_player_name[32];
//br_matrix34 gRearview_camera_to_world;
//tSpecial_volume gDefault_default_water_spec_vol;
//br_matrix34 gCamera_to_world;
//tRace_list_spec gRace_list[100];
C2_HOOK_VARIABLE_IMPLEMENT(float, gPinball_factor, 0x00762124);
C2_HOOK_VARIABLE_IMPLEMENT(float, gOpponent_speed_factor, 0x007620f0);
//float gCar_crush_min_fold;
//float gNet_recovery_cost[7];
//float gCar_crush_softness;
//br_vector3* gOur_pos;
//float gCar_crush_max_fold;
C2_HOOK_VARIABLE_IMPLEMENT(tBrender_storage, gNet_cars_storage_space, 0x00761f00);
//float gNet_softness[7];
//tS32 gJoystick_deadzone;
//float gCar_crush_limit_deviant;
//float gNet_offensive[7];
//float gCar_crush_split_chance;
C2_HOOK_VARIABLE_IMPLEMENT(float, gGravity_multiplier, 0x0074d1a8);
//float gCar_crush_wibble;
//float gRepair_cost[3];
C2_HOOK_VARIABLE_IMPLEMENT(float, gCop_speed_factor, 0x0074d634);
C2_HOOK_VARIABLE_IMPLEMENT(float, gDefault_gravity, 0x0074d3bc);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gKeys_pressed, 0x0074d350);
//tSpecial_volume* gDefault_water_spec_vol;
//float gNet_repair_cost[7];
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gLast_replay_frame_time, 0x0079efb4);
//tU32 gTag_start_time;
//tOpponent* gOpponents;
//tU32 gLast_credit_tick;
//tBrender_storage gTheir_cars_storage_space;
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gRace_finished, 0x0074d62c);
//tU32 gFrame_start_time;
//tProgram_state gProgram_state;
//tU32 gCredit_period_single[3];
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gRace_start, 0x00763460);
//tBrender_storage gTrack_storage_space;
//tU32 gCredit_period;
//tU32 gCredit_period_network[7];
//tBrender_storage gOur_car_storage_space;
//tU32 gFox_start_time;
//tBrender_storage gPedestrians_storage_space;
//tRace_info gCurrent_race;
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gSelf, 0x0068be28);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gRender_screen, 0x00762128);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gRearview_camera, 0x0075b940);
C2_HOOK_VARIABLE_IMPLEMENT(br_scalar, gCamera_hither, 0x0074d3e8);
//br_scalar gCamera_angle;
//br_pixelmap* gDigits_pix;
//br_pixelmap* gRender_shade_table;
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gRearview_depth_buffer, 0x0075b93c);
//br_pixelmap* gWhole_screen;
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gIcons_pix, 0x0068b8ac);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gCamera, 0x0074d35c);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_actor*, gOther_selfs, 4, 0x0074d644);
//br_actor* gArrow_actor;
//br_pixelmap* gIcons_pix_low_res;
//int gInitial_net_credits[5];
//float gCar_cred_value[3];
//int gNo_races_yet;
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gJump_start_fine, 3, 0x0074d3d0);
//int gReal_graf_data_index;
//float gCar_time_value[3];
//int gTyping_slot;
//int gCunning_stunt_bonus[3];
//int gKnobbled_frame_period;
//int gWait_for_it;
//int gCheck_point_cash[3];
//int gPoints_per_second[3];
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gMap_time, 0x00761edc);
//int gPed_time_value[3];
C2_HOOK_VARIABLE_IMPLEMENT(int, gAbandon_game, 0x0074d1a0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gLap, 0x0075b944);
C2_HOOK_VARIABLE_IMPLEMENT(int, gShow_opponents, 0x00763510);
C2_HOOK_VARIABLE_IMPLEMENT(int, gTotal_laps, 0x0075ba50);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFree_repairs, 0x0075b8f4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFreeze_timer, 0x0075bb64);
//int gRoll_over_creds[3];
C2_HOOK_VARIABLE_IMPLEMENT(int, gShow_peds_on_map, 0x0074d09c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gOn_drugs, 0x0074d444);
//int gWasted_time[3];
C2_HOOK_VARIABLE_IMPLEMENT(int, gCheckpoint, 0x00761eec);
//int gRoll_over_time[3];
//int gWasted_creds[3];
C2_HOOK_VARIABLE_IMPLEMENT(int, gTimer, 0x00761ee8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCheckpoint_count, 0x00761ce8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCountdown, 0x00761f48);
//int gFrame_rate;
//int gNumber_of_racers;
C2_HOOK_VARIABLE_IMPLEMENT(int, gSound_enabled, 0x00762328);
//int gGraf_spec_index;
C2_HOOK_VARIABLE_IMPLEMENT(int, gSound_available, 0x00761ee4);
//int gNumber_of_races;
//char gDir_separator[3] = "";

C2_HOOK_VARIABLE_IMPLEMENT(tPixelFlags, gPixelFlags, 0x0074ca18);
