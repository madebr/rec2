#ifndef REC2_GLOBVARS_H
#define REC2_GLOBVARS_H

#include "c2_hooks.h"

//#include "brender/br_types.h"
#include "rec2_types.h"

extern int gApplicationDataTwtMounted;
extern tPath_name gApplication_path;
extern const char* gDir_separator;

//extern br_font* gBig_font;
extern int gAusterity_mode;
extern int gAustere_override;

extern br_font* gBig_font;
extern br_font* gFont_7;
extern br_font* gHeadup_font;

extern tProgram_state gProgram_state;
extern int gSausage_override;
extern tRace_list_spec gRace_list[100];
extern int gNumber_of_races;

extern tBrender_storage gOur_car_storage_space;
extern tBrender_storage gTheir_cars_storage_space;
extern tBrender_storage gDroneStorage;
extern tBrender_storage gPedStorage;
extern tBrender_storage gTrack_storage_space;

extern int gCD_is_in_drive;
extern int gDev_initial_race;
extern br_matrix34 gCamera_to_world;
extern br_matrix34 gRearview_camera_to_world;

extern undefined4 gUnknown_00705b80[20];

extern br_actor* gUniverse_actor;
extern br_actor* gNon_track_actor;
extern br_actor* gDont_render_actor;

extern int gAmbient_sound;
extern int gAuto_load;
extern int gRussels_fannies;

extern char gQuick_time_quality[32];
extern char gQuick_time_compressor[32];
extern int gQuick_time_banner_number;

extern int gNet_last_game_type;
extern int gMap_view;

extern int gScreen_lock_often;
extern int gShadow_workaround;
extern int gEnable_texture_interpolation;
extern int gEnable_texture_antialiasing;
extern int gTexture_power_of_2;
extern int gAdapt_sky_model_for_cockpit;
extern int gNo_fog;
extern const char* gRenderer_fixup_basename;
extern const char* gRenderer_fixup_extension;
extern int gNo_render_indent;
extern int gNo_2d_effects;
extern int gEnable_perspective_maps;
extern int gDevious_2d;

extern int gNumber_of_actors;
extern br_actor* gActor_array[100];
extern int gNumber_of_lights;
extern tLighting_data gLighting_data;
extern br_actor* gLight_array[50];

extern br_actor* gHUD_root;
extern br_actor* gHUD_camera;

extern br_actor* gLine_actor;
extern br_model* gLine_model;
extern br_material* gLine_material;

extern br_model* gBlend_model;
extern br_model* gBlend_model2;
extern br_actor* gBlend_actor;
extern tBRender_smoke gBR_smoke_structs[35];

extern int gNo_current_game;
extern int gWaiting_for_unpause;
extern tU32 gWait_for_it;
extern int gGame_to_load;
extern tRace_info gCurrent_race;
extern int gGame_initialized;
extern int gNo_races_yet;
extern tNet_mode gNet_mode_of_last_game;
extern tRace_group_spec* gCurrent_race_group;
extern int gIs_boundary_race;
extern int gNumber_of_racers;
extern tOpponent* gOpponents;
extern int gCurrent_APO_levels[3];
extern int gCurrent_APO_potential_levels[3];
extern tNet_game_details* gCurrent_net_game;

extern int gCockpit_on;
extern int gPratcam_on;

extern int gTree_surgery_pass1_count;
extern int gTree_surgery_pass2_count;
extern tTree_surgery_pass1 gTree_surgery_pass1[5];
extern tTree_surgery_pass2 gTree_surgery_pass2[25];

extern int gTyping;
extern int gCount_opponents;
extern br_actor* gPlayer_car_master_actor;
extern br_vector3 gInitial_position;
extern int gArrow_mode;

extern int gRace_head_ups[22];

//extern int gInfo_on;
extern int gHost_abandon_game;
extern int gRecover_car;
extern int gDisallow_abandon_race;
//extern int gPipe_reg_snap_before_new_game;
//extern int gPipe_reg_snap_before_our_car;
//extern int gPipe_reg_snap_after_our_car;
extern int gAction_replay_mode;
//extern int gDefault_engine_noise_index;
//extern int gSausage_override;
//extern int gAustere_override;
extern int gSound_override;
extern int gNoCutscenes;
extern int gAFE;
//extern int gKey_map_index;
extern int gInterface_within_race_mode;
//extern int gCut_scene_override;
//extern int gGame_to_load;
//extern int gCD_is_in_drive;
extern br_pixelmap* gRearview_screen;
//extern br_scalar gCut_delay_1;
//extern br_scalar gCut_delay_2;
//extern br_scalar gCut_delay_3;
//extern br_scalar gCut_delay_4;
extern br_scalar gYon_multiplier;
//extern tSave_game* gPre_race_saved_game;
//extern VFILE* gDiagnostic_file;
//extern br_vector3 gDisabled_vector;
extern tU32 gI_am_cheating;
//extern char gNet_player_name[32];
//extern br_matrix34 gRearview_camera_to_world;
//extern tSpecial_volume gDefault_default_water_spec_vol;
//extern br_matrix34 gCamera_to_world;
//extern tRace_list_spec gRace_list[100];
extern float gPinball_factor;
extern float gOpponent_speed_factor;
//extern float gCar_crush_min_fold;
//extern float gNet_recovery_cost[7];
//extern float gCar_crush_softness;
extern br_vector3* gOur_pos;
//extern float gCar_crush_max_fold;
extern tBrender_storage gNet_cars_storage_space;
//extern float gNet_softness[7];
//extern tS32 gJoystick_deadzone;
//extern float gCar_crush_limit_deviant;
//extern float gNet_offensive[7];
//extern float gCar_crush_split_chance;
extern float gGravity_multiplier;
//extern float gCar_crush_wibble;
//extern float gRepair_cost[3];
extern float gCop_speed_factor;
extern float gDefault_gravity;
extern tU32 gKeys_pressed;
//extern tSpecial_volume* gDefault_water_spec_vol;
//extern float gNet_repair_cost[7];
extern tU32 gLast_replay_frame_time;
//extern tU32 gTag_start_time;
//extern tOpponent* gOpponents;
//extern tU32 gLast_credit_tick;
//extern tBrender_storage gTheir_cars_storage_space;
extern tU32 gRace_finished;
//extern tU32 gFrame_start_time;
//extern tProgram_state gProgram_state;
//extern tU32 gCredit_period_single[3];
extern tU32 gRace_start;
//extern tBrender_storage gTrack_storage_space;

extern tU32 gFrame_period;

//extern tU32 gCredit_period;
//extern tU32 gCredit_period_network[7];
//extern tBrender_storage gOur_car_storage_space;
extern tBrender_storage gMisc_storage_space;
//extern tU32 gFox_start_time;
//extern tBrender_storage gPedestrians_storage_space;
//extern tRace_info gCurrent_race;
extern br_actor* gSelf;
extern br_pixelmap* gScreen;
//extern br_pixelmap* gScreen;
extern br_pixelmap* gRender_screen;
extern br_actor* gRearview_camera;
extern br_scalar gCamera_hither;
extern br_pixelmap* gBack_screen;
extern br_pixelmap* gDouble_back_screen;
extern int gHas_double_back_screen;
//extern br_scalar gCamera_angle;
//extern br_pixelmap* gDigits_pix;
//extern br_pixelmap* gRender_shade_table;
extern br_pixelmap* gRearview_depth_buffer;
//extern br_pixelmap* gWhole_screen;
extern br_pixelmap* gIcons_pix;
extern br_scalar gCamera_yon;
extern br_actor* gCamera;
extern br_actor* gOther_selfs[4];
//extern br_actor* gArrow_actor;
//extern br_pixelmap* gIcons_pix_low_res;
extern br_pixelmap* gDepth_buffer;
//extern int gInitial_net_credits[5];
//extern float gCar_cred_value[3];
//extern int gNo_races_yet;
extern int gJump_start_fine[3];
//extern int gReal_graf_data_index;
//extern float gCar_time_value[3];
//extern int gTyping_slot;
//extern int gCunning_stunt_bonus[3];
extern tU32 gKnobbled_frame_period;
//extern int gWait_for_it;
//extern int gCheck_point_cash[3];
//extern int gPoints_per_second[3];
extern br_actor* gTrack_actor;
extern tU32 gMap_time;
//extern int gPed_time_value[3];
extern int gAbandon_game;
extern int gLap;
extern int gShow_opponents;
extern int gTotal_laps;
extern int gFree_repairs;
extern int gFreeze_timer;
//extern int gRoll_over_creds[3];
extern int gShow_peds_on_map;
extern int gOn_drugs;
//extern int gWasted_time[3];
extern int gCheckpoint;
//extern int gRoll_over_time[3];
//extern int gWasted_creds[3];
extern tU32 gTimer;
extern int gCheckpoint_count;
extern int gCountdown;
extern int gFrame_rate;
//extern int gNumber_of_racers;
extern int gSound_enabled;
//extern int gGraf_spec_index;
extern int gSound_available;
//extern int gNumber_of_races;
extern tPixelFlags gPixelFlags;
extern int gNo_credits_APO_restore;

#endif // REC2_GLOBVARS_H
