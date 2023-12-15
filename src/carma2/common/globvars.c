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

C2_HOOK_VARIABLE_IMPLEMENT(br_scalar, gCamera_yon, 0x00761f4c);;

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

C2_HOOK_VARIABLE_IMPLEMENT(int, gRussels_fannies, 0x006aa5c4);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gQuick_time_quality, 32, 0x00705220);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gQuick_time_compressor, 32, 0x00705200);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gQuick_time_banner_number, 0x0065ef30, 1);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gQuick_time_banner_texture_name, 32, 0x00705340);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gQuick_time_temp_path, 256, 0x00705440);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gQuick_time_movie_path_stub, 256, 0x00705240);

//int gTyping;
//int gArrow_mode;
//int gInfo_on;
//int gHost_abandon_game;
//int gRecover_car;
//int gDisallow_abandon_race;
//int gPipe_reg_snap_before_new_game;
//int gPipe_reg_snap_before_our_car;
//int gPipe_reg_snap_after_our_car;
//int gAction_replay_mode;
//int gDefault_engine_noise_index;
//int gAusterity_mode;
//int gAustere_override;

//int gKey_map_index;
//int gInterface_within_race_mode;
//int gCut_scene_override;
//int gGame_to_load = -1;
//int gCD_is_in_drive;
//br_pixelmap* gRearview_screen;
//br_scalar gCut_delay_1;
//br_scalar gCut_delay_2;
//br_scalar gCut_delay_3;
//br_scalar gCut_delay_4;
//br_scalar gYon_multiplier;
//tSave_game* gPre_race_saved_game;
//VFILE* gDiagnostic_file;
//br_vector3 gDisabled_vector;
//tU32 gI_am_cheating;
//char gNet_player_name[32];
//br_matrix34 gRearview_camera_to_world;
//tSpecial_volume gDefault_default_water_spec_vol;
//br_matrix34 gCamera_to_world;
//tRace_list_spec gRace_list[100];
//float gPinball_factor;
//float gOpponent_speed_factor;
//float gCar_crush_min_fold;
//float gNet_recovery_cost[7];
//float gCar_crush_softness;
//br_vector3* gOur_pos;
//float gCar_crush_max_fold;
//tBrender_storage gNet_cars_storage_space;
//float gNet_softness[7];
//tS32 gJoystick_deadzone;
//float gCar_crush_limit_deviant;
//float gNet_offensive[7];
//float gCar_crush_split_chance;
//float gGravity_multiplier;
//float gCar_crush_wibble;
//float gRepair_cost[3];
//float gCop_speed_factor;
//float gDefault_gravity;
//tU32 gKeys_pressed;
//tSpecial_volume* gDefault_water_spec_vol;
//float gNet_repair_cost[7];
//tU32 gLast_replay_frame_time;
//tU32 gTag_start_time;
//tOpponent* gOpponents;
//tU32 gLast_credit_tick;
//tBrender_storage gTheir_cars_storage_space;
//tU32 gRace_finished;
//tU32 gFrame_start_time;
//tProgram_state gProgram_state;
//tU32 gCredit_period_single[3];
//tU32 gRace_start;
//tBrender_storage gTrack_storage_space;
//tU32 gCredit_period;
//tU32 gCredit_period_network[7];
//tBrender_storage gOur_car_storage_space;
//tU32 gFox_start_time;
//tBrender_storage gPedestrians_storage_space;
//tRace_info gCurrent_race;
//br_actor* gSelf;
//br_pixelmap* gRender_screen;
//br_actor* gRearview_camera;
//br_scalar gCamera_hither;
//br_scalar gCamera_angle;
//br_pixelmap* gDigits_pix;
//br_pixelmap* gRender_shade_table;
//br_pixelmap* gRearview_depth_buffer;
//br_pixelmap* gWhole_screen;
//br_pixelmap* gIcons_pix;
//br_actor* gCamera;
//br_actor* gArrow_actor;
//br_pixelmap* gIcons_pix_low_res;
//int gInitial_net_credits[5];
//float gCar_cred_value[3];
//int gNo_races_yet;
//int gJump_start_fine[3];
//int gReal_graf_data_index;
//float gCar_time_value[3];
//int gTyping_slot;
//int gCunning_stunt_bonus[3];
//int gKnobbled_frame_period;
//int gWait_for_it;
//int gCheck_point_cash[3];
//int gPoints_per_second[3];
//int gMap_mode;
//int gPed_time_value[3];
//int gAbandon_game;
//int gLap;
//int gShow_opponents;
//int gTotal_laps;
//int gFree_repairs;
//int gFreeze_timer;
//int gRoll_over_creds[3];
//int gShow_peds_on_map;
//int gOn_drugs;
//int gWasted_time[3];
//int gCheckpoint;
//int gRoll_over_time[3];
//int gWasted_creds[3];
//int gTimer;
//int gCheckpoint_count;
//int gCountdown;
//int gFrame_rate;
//int gNumber_of_racers;
//int gSound_enabled;
//int gGraf_spec_index;
//int gSound_available;
//int gNumber_of_races;
//char gDir_separator[3] = "";

C2_HOOK_VARIABLE_IMPLEMENT(tPixelFlags, gPixelFlags, 0x0074ca18);
