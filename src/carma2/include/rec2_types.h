#ifndef REC2_TYPES_H
#define REC2_TYPES_H

#include <brender/br_types.h>
#include <s3/s3.h>

typedef char tPath_name[256];

typedef unsigned char tU8;
typedef signed char tS8;
typedef unsigned short tU16;
typedef signed short tS16;
typedef unsigned int tU32;
typedef signed int tS32;

typedef int tTWTVFS;

typedef struct tRace_list_spec tRace_list_spec;
typedef struct tRace_list_spec tRace_list_spec;
typedef struct tRace_info tRace_info;
typedef struct tTrack_spec tTrack_spec;
typedef struct tPowerup tPowerup;

// Function callbacks are moved to a header for automatic SRE project generation
typedef void(C2_HOOK_FASTCALL * tPlayFlic_DoPerFrame)(void);
typedef void (C2_HOOK_FASTCALL * tPDForEveryFile_cbfn)(const char*);
typedef void (C2_HOOK_FASTCALL * tPDForEveryFileRecurse_cbfn)(const char*);

// FIXME: incomplete type
typedef struct tDR_font tDR_font;
typedef struct tCar_spec tCar_spec;

typedef struct {
    int r;
    int g;
    int b;
} tRGBColour;

typedef struct {
    tRGBColour tl;
    tRGBColour tr;
    tRGBColour bl;
    tRGBColour br;
} tPolyFontBorderColours;

typedef struct tPolyFont tPolyFont;

typedef struct {
    br_material* material;
    int used;
    br_uint_32 index;
    br_uint_32 glyph_width;
    br_vector2 texCoord;
    tPolyFont* font;
} tPolyFontGlyph;

typedef struct tPolyFont {
    char field0_0x0[68];
    br_uint_32 numberOfCharacters;
    br_uint_32 interCharacterSpacing;
    br_uint_32 asciiOffset;
    br_uint_32 fontCharacterHeight;
    br_uint_32 widthOfBlank;
    br_uint_32 fontSize;
    br_uint_32 unknown_0x5c;
    int available;
    br_uint_32 unknown_0x64;
    tPolyFontGlyph glyphs[256]; /* unsure about offset */
} tPolyFont;

typedef struct tFlic_bunch {
    char count;
    int* indexes;
} tFlic_bunch;

typedef struct tFlic_spec {
    char* file_name;
    int interruptable;
    int queued;
    int frame_rate;
    int repeat;
    int x_offset;
    int y_offset;
    tS8* data_ptr;
    tU32 the_size;
} tFlic_spec;

typedef struct tFlic_descriptor* tFlic_descriptor_ptr;

typedef struct tFlic_descriptor {
    char* data;
    char* data_start;
    char file_name[32];
    tU8* first_pixel;
    tU32 bytes_remaining;
    tU32 frame_period;
    tU32 last_frame;
    br_pixelmap* the_pixelmap;
    int x_offset;
    int y_offset;
    int width;
    int height;
    int frames_left;
    int current_frame;
    int the_index;
    int new_format;
    int must_finish;
    int bytes_still_to_be_read;
    int bytes_in_buffer;
    void* f;
    tFlic_descriptor_ptr next;
} tFlic_descriptor;

typedef enum tJustification {
    eJust_left = 0,
    eJust_right = 1,
    eJust_centre = 2
} tJustification;

typedef struct tHeadup_slot {
    int x;
    int y;
    int colour;
    int cockpit_anchored;
    int dimmed_background;
    int dim_left;
    int dim_top;
    int dim_right;
    int dim_bottom;
    tJustification justification;
    int field_0x28; // FIXME: unknown
} tHeadup_slot;

typedef struct tTranslation_record {
    int flic_index;
    int x;
    int y;
    int font_index;
    tJustification justification;
    char* text;
} tTranslation_record;

typedef int (C2_HOOK_FASTCALL * tEnumPathCallback)(const char*, void*);

typedef enum {
    kLoadTextureFlags_16bbp = 0x01,
    kLoadTextureFlags_UseARGB1555 = 0x02,
    kLoadTextureFlags_SaveBrenderTexture = 0x04,
    kLoadTextureFlags_KeepShadeTable = 0x08,
    kLoadTextureFlags_PalatteRGB555 = 0x10,
    kLoadTextureFlags_ForceTiff = 0x20,
    kLoadTextureFlags_SaveTextureCompressed = 0x40,
} tLoadTextureFlags;

// relict from Carmageddon I
typedef struct tInterface_spec tInterface_spec;

typedef struct tLoadDirectoryStructureCBResult {
    char* name;
    struct tLoadDirectoryStructureCBResult* next;
} tLoadDirectoryStructureCBResult;

typedef struct {
    const char* directory;
    br_pixelmap* pPalette;
    int loadFlags;
    int* errorCode;
    tLoadDirectoryStructureCBResult* results;
    int isTiffx : 1;
} tLoadDirectoryStructureCBData;

typedef enum tNet_mode {
    eNet_mode_none = 0,
    eNet_mode_thinking_about_it = 1,
    eNet_mode_host = 2,
    eNet_mode_client = 3
} tNet_mode;

typedef enum tCar_texturing_level {
    eCTL_none = 0,
    eCTL_transparent = 1,
    eCTL_full = 2,
    eCTL_count = 3,
} tCar_texturing_level;

typedef enum tRoad_texturing_level {
    eRTL_none = 0,
    eRTL_full = 1,
    eRTL_count = 2
} tRoad_texturing_level;

typedef enum tWall_texturing_level {
    eWTL_none = 0,
    eWTL_linear = 1,
    eWTL_full = 2,
    eWTL_count = 3
} tWall_texturing_level;

typedef enum tShadow_level {
    eShadow_none = 0,
    eShadow_us_only = 1,
    eShadow_us_and_opponents = 2,
    eShadow_everyone = 3
} tShadow_level;

typedef enum tAdd_to_storage_result {
    eStorage_not_enough_room = 0,
    eStorage_duplicate = 1,
    eStorage_allocated = 2
} tAdd_to_storage_result;

typedef struct {
    int width;
    int height;
    tU8 unknown[1304];
} tGraf_data;

typedef enum {
    kPixelFlags_16bbp = 0x1,
    kPixelFlags_unknown = 0x2,
} tPixelFlags;

typedef enum {
    kRaceType_Carma1 = 0,
    kRaceType_Cars = 1,
    kRaceType_Peds = 2,
    kRaceType_Checkpoints = 3,
    kRaceType_Smash = 4,
    kRaceType_SmashNPed = 5,
} eRaceType;

typedef struct {
    int depth_bits;
    int depth_bytes;
    int doubled;
    int total_width;
    int total_height;
    long black_value;
    int colour_index;
    char* data_dir_name;
    char* gfx_init_string;
    int row_bytes;
    int phys_width;
    int phys_height;
    void* base_addr;
} tGraf_spec;

typedef struct {
    int unknown; // FIXME: unknown
    int models_count;
    int materials_count;
    int shade_tables_count;
    int pixelmaps_count;
    int sounds_count; // FIXME: correct?
    int max_pixelmaps;
    int max_shade_tables;
    int max_materials;
    int max_models;
    int max_sounds;
    br_model** models;
    br_pixelmap** pixelmaps;
    br_pixelmap** shade_tables;
    br_material** materials;
    int** sounds; // FIXME: correct?
    br_material** materialProps; // FIXME: correct?
} tBrender_storage;

typedef struct {
    size_t size;
    char items[1000][16];
} tName_list;

typedef struct {
    tU32 fileSize;
    char filename[48];
    tU8* data;
} tTwatFileHeader;

typedef struct {
    tU32 twatFileSize;
    tU32 nbFiles;
    tTwatFileHeader fileHeaders[];
} tTwatHeader;

typedef struct {
    tTwatHeader* header;
    char path[256];
    tU8* data;
} tTwatVfsMountPoint;

typedef struct {
    tU8* start;
    tU8* pos;
    tU8* end;
    int error;
} tTwatVfsFile;

typedef struct tWobble_spec {
    float amplitude_x;
    float amplitude_y;
    float period;
    int time_started;
} tWobble_spec;

typedef enum tNet_head_avail {
    eNet_or_otherwise = 0,
    eNet_only = 1,
    eNot_net = 2,
    eNot_ever_ever_ever = 3
} tNet_head_avail;

typedef struct tHeadup_info {
    char* name;
    tNet_head_avail avail;
} tHeadup_info;

typedef enum {
    kSoundFx_None = -1,
    kSoundFx_Saturated = 0,
    kSoundFx_Scattered = 1,
} tSpecial_volume_soundfx_type;

typedef enum {
    kSoundFxPeriodicity_Random = 0,
    kSoundFxPeriodicity_Periodic = 1,
    kSoundFxPeriodicity_Continuous = 2,
} tSpecial_volume_soundfx_periodicity_type;

typedef enum tKey_down_result {
    tKey_down_no = 0,
    tKey_down_yes = 1,
    tKey_down_still = 2,
    tKey_down_repeat = 3
} tKey_down_result;

typedef struct {
    tSpecial_volume_soundfx_periodicity_type periodicity;
    float periodic1;
    float periodic2;
    tU32 unknown1;
    int nbEnvironmentalSoundAlternatives;
    tS32 unknown2; // fixedpoint integer? (* 16 / 100)
    tS16 soundAlternatives[5];
    tS16 unknown3;
} tSpecial_volume_soundfx_data;

typedef struct {
    char unknown1[136];
    br_scalar gravity_multiplier;
    br_scalar viscosity_multiplier;
    float car_damage_per_ms;
    float ped_damage_per_ms;
    int camera_special_effect_index;
    int sky_col;
    int entry_noise;
    int exit_noise;
    int engine_noise_index;
    br_material* screen_material;
    int material_modifier_index;
    tSpecial_volume_soundfx_type soundfx_type;
    tSpecial_volume_soundfx_data soundfx_data;
} tSpecial_volume;

// FIXME: PROBABLY WRONG!!!!!!
typedef enum tCar_choice {
    eNet_car_eagle = 0,
    eNet_car_novelty = 1,
    eNet_car_all = 2,
} tCar_choice;

typedef enum tNet_sequence_type {
    eNet_sequence_sequential = 0,
    eNet_sequence_random = 1,
} tNet_sequence_type;

typedef struct tNet_game_options {
    int show_players_on_map;
    int enable_text_messages;
    int show_powerups_on_map;
    int powerup_respawn;
    int open_game;
    int starting_credits;
    int grid_start;
    int starting_target;
    int waste_to_transfer;
    int random_car_choice;
    tNet_sequence_type race_sequence_type;
    tCar_choice car_choice;
} tNet_game_options;

typedef struct tTransient_bm {
    br_pixelmap* pixmap;
    int in_use;
    int x_coord;
    int y_coord;
    int order_number;
    int user_data;
} tTransient_bm;

typedef struct tDR_font {
    br_pixelmap* images;
    int file_read_once;
    int height;
    int width;
    int spacing;
    int offset;
    int num_entries;
    int font_id;
    int width_table[224];
} tDR_font;

typedef enum {
    kFrontendItemEnabled_default = 0,
    kFrontendItemEnabled_enabled = 1,
    kFrontendItemEnabled_disabled = -1,
} eFrontendItemEnablec;

typedef enum {
    kFrontendSelectableButton = 0,
    kFrontendSelectableOption = 1,
} eFrontendSelectableType;

typedef struct {
    int unknown0;  // FIXME: unknown
    int unknown1;  // FIXME: unknown
    int unknown2;  // FIXME: unknown
    int unknown3;  // FIXME: unknown
    int unknown4;  // FIXME: unknown
    int unknown5;  // FIXME: unknown
} tFrontend_level_spec;

typedef struct {
    int id;
    int count;
    int nbDisplayedAtOnce;
    int indexTopItem;
    int indexFirstScrollableItem;
    int indexLastScrollableItem;
    int greyedOutBackdrop;
    int x_greyArea;
    int y_greyArea;
    int width_greyArea;
    int height_greyArea;
    int indexOfItemAtTop;
    int unknown;  // FIXME: unknown
} tFrontend_scroller_spec;

typedef struct {
    int id;
    int count;
    int indexFirstItem;
    int indexLastItem;
    int greyboxRequested;
    int x_greybox;
    int y_greybox;
    int width_greybox;
    int height_greybox;
    int indexSelected;
    int unknown;  // FIXME: unknown
} tFrontend_radio_spec;

typedef struct tFrontend_spec tFrontend_spec;

typedef struct tFrontend_item_spec {
    int stringId;
    int (C2_HOOK_FASTCALL * action)(int);
    tFrontend_spec* menuInfo;
    int unknown;  // FIXME: unknown
    int unlitFont;
    int highFont;
    int x;
    int y;
    int width;
    int height;
    eFrontendItemEnablec enabled;
    int visible;
    int pixmapIndex; /* 0 = no image */
    int greyedBackdrop;
    eFrontendSelectableType selectable;
    int radioButton_selected;
    int group;
    int idLevelBar;
    int wrapText;
    int glowDisabled;
    int x2;
    int y2;
    char unknownChar;  // FIXME: unknown
    char text[255];
} tFrontend_item_spec;

typedef struct tFrontend_spec {
    char name[260];
    int itemCount;
    int (C2_HOOK_FASTCALL * create)(struct tFrontend_spec*);
    int (C2_HOOK_FASTCALL * destroy)(struct tFrontend_spec*);
    int (C2_HOOK_FASTCALL * tick)(struct tFrontend_spec*);
    struct tFrontend_spec* previousFrontend;
    int unknown1;  // FIXME: unknown
    int unknown2;  // FIXME: unknown
    int unknown3;  // FIXME: unknown
    int model_A_index;
    int model_B_index;
    int model_C_index;
    tFrontend_item_spec items[100];
    int scrollerCount;
    tFrontend_scroller_spec scrollers[100];
    int radioCount;
    tFrontend_radio_spec radios[100];
    int groupCount;
    int loaded;
    char unknownLongString[260];  // FIXME: unknown
    char backdropName[256];
    tFrontend_level_spec levels[100];
    int levelCount;
    char unknownshortString[56];  // FIXME: unknown
    int isPrevimousSomeOtherMenu;
    int unknownLastInt;  // FIXME: unknown
} tFrontend_spec;

typedef struct {
    br_actor* actor;
    br_material* material;
    br_model* model;
    br_uint_32 color_red;
    br_uint_32 color_grn;
    br_uint_32 color_blu;
    br_uint_32 color2_red;
    br_uint_32 color2_grn;
    br_uint_32 color2_blu;
    int subClass;
    br_uint_8 unknown_0;
    br_uint_8 unknown_padding_0;  // FIXME: unknown
    br_uint_8 unknown_padding_1;  // FIXME: unknown
    br_uint_8 unknown_padding_2;  // FIXME: unknown
    br_material* material2;
    int used;
    int visible;
    int class;
    int lastTime;
    br_colour colour;
    br_pixelmap* pixelmap;
    br_colour colours[256];
    br_uint_32 tints1[2048];
    br_uint_32 tints2[2048];
    br_uint_32 tints3[2048];
    br_uint_32 unknown_1;  // FIXME: unknown
    br_uint_32 unknown_2;  // FIXME: unknown
} tTintedPoly;

typedef struct tRace_group_spec {
    int count_races;
    tRace_list_spec* races;
    tRace_list_spec* mission;
} tRace_group_spec;

typedef struct tRace_list_spec {
    char name[64];                      /* readable name */
    char interface_name[32];            /* interface element name */
    char file_name[32];                 /* Text file name */
    char description[256];
    br_scalar opponent_nastiness_level;
    int count_opponents;
    int expansion;
    int no_time_awards;
    int is_boundary;                    /* boundary race/mission */
    int count_explicit_opponents;       /* Number of explicit opponents */
    int explicit_opponents[15];         /* positive number: index / negative number: hardness */
    int count_powerup_exclusions;
    int* powerup_exclusions;
    int count_laps;
    int completion_bonus[3];
    int completion_bonus_peds[3];
    int completion_bonus_opponents[3];
    int initial_timer[3];
    eRaceType race_type;
    union {
        struct {
            int count_laps;
        } checkpoints;
        struct {
            int count_opponents;
            int opponents[10];
        } cars;
        struct {
            int count_ped_groups;
            int ped_groups[10];
        } peds;
        struct {
            int var_smash_number;
            int var_smash_target;
        } smash;
        struct {
            int var_smash_number;
            int var_smash_target;
            int ped_group_index;
        } smash_and_peds;
    } options;
    tRace_group_spec* group;
} tRace_list_spec;

typedef struct {
    tPowerup* powerup;
    int fizzle_stage;
    int fizzle_direction;
    tU32 fizzle_start;
} tHeadup_icon;

enum {
    kMiscString_ShadowNone = 104,
    kMiscString_ShadowUsOnly = 105,
    kMiscString_ShadowUsAndOpponents = 106,
    kMiscString_ShadowEveryone = 107,
};

enum {
    kBoner_mem_type_remap = 0x81,
    kBoner_mem_type_sequence = 0x82,
    kMem_nodes_array = 0x83,
    kMem_sections_array = 0x84,
    kBoner_mem_type_frame_2 = 0x85,
    kMem_columns_z = 0x86,
    kMem_columns_x = 0x87,
    kMem_non_car_list = 0x88,
    kMem_simp_level = 0x89,
    kMem_crush_data = 0x8a,
    kMem_drone_collision_stuff = 0x8b,
    kBoner_mem_type_form = 0x8c,
    kBoner_mem_type_actor_sets = 0x8d,
    kBoner_mem_type_simple_phys = 0x8e,
    kBoner_mem_type_boned_phys = 0x8f,
    kBoner_mem_type_bone_info = 0x90,
    kBoner_mem_type_boned_coll = 0x91,
    kBoner_mem_type_boned_coll_obj = 0x92,
    kBoner_mem_type_simple_coll_obj = 0x93,
    kBoner_mem_type_actor_ptrs = 0x94,
    kBoner_mem_type_moves = 0x95,
    kBoner_mem_type_stored_ptrs = 0x96,
    kBoner_mem_type_stored = 0x97,
    kBoner_mem_type_person = 0x98,
    kBoner_mem_type_person_parts = 0x99,
    kBoner_mem_type_person_moves = 0x9a,
    kBoner_mem_type_char_instance = 0x9b,
    kMem_boner_type_frames = 0x9c,
    kMem_smash_levels = 0x9d,
    kMem_smash_side_effects = 0x9e,
    kMem_damage_clauses = 0x9f,
    kMem_undamaged_vertices = 0xa0,
    kMem_race_text_chunk = 0xa1,
    kMem_race_text_str = 0xa2,
    kMem_oppo_array = 0xa3,
    kMem_oppo_text_chunk = 0xa4,
    kMem_oppo_text_str = 0xa5,
    kMem_br_font = 0xa6,
    kMem_br_font_wid = 0xa7,
    kMem_br_font_enc = 0xa8,
    kMem_br_font_glyphs = 0xa9,
    kMem_oppo_car_spec = 0xaa,
    kMem_misc_string = 0xab,
    kMem_mac_host_buffer_1 = 0xac,
    kMem_mac_host_buffer_2 = 0xad,
    kMem_mac_net_details = 0xae,
    kMem_back_pixels = 0xaf,
    kMem_quit_vfy_pixels = 0xb0,
    kMem_quit_vfy_pal = 0xb1,
    kMem_net_min_messages = 0xb2,
    kMem_net_mid_messages = 0xb3,
    kMem_net_max_messages = 0xb4,
    kMem_net_pid_details = 0xb5,
    kMem_net_car_spec = 0xb6,
    kMem_dynamic_message = 0xb7,
    kMem_player_list_join = 0xb8,
    kMem_player_list_leave = 0xb9,
    kMem_oppo_new_nodes = 0xba,
    kMem_oppo_new_sections = 0xbb,
    kMem_drone_model_dup = 0xbc,
    kMem_oppo_bit_per_node = 0xbd,
    kMem_smash_poly_array = 0xbe,
    kMem_smash_vertex_array = 0xbf,
    kMem_ped_array_stain = 0xc0,
    kMem_ped_array = 0xc1,
    kMem_ped_instructions = 0xc2,
    kMem_ped_new_instruc = 0xc3,
    kMem_pipe_model_geometry = 0xc4,
    kMem_powerup_array = 0xc5,
    kMem_powerup_parms = 0xc6,
    kMem_noisey_parts = 0xc7,
    kMem_sound_generator = 0xc8,
    kMem_pratcam_flic_array = 0xc9,
    kMem_pratcam_flic_data = 0xca,
    kMem_pratcam_sequence_array = 0xcb,
    kMem_pratcam_pixelmap = 0xcc,
    kMem_video_pixels = 0xcd,
    kMem_funk_prox_array = 0xce,
    kMem_new_mat_id = 0xcf,
    kMem_packed_file = 0xd0,
    kMem_stored_smash = 0xd1,
    kMem_special_volume = 0xd2,
    kMem_special_screen = 0xd3,
    kMem_saved_gam = 0xd5,
    kMem_new_save_game = 0xd6,
    kMem_stor_space_table = 0xd7,
    kMem_boner = 0xd8,
    kMem_ped_face_cache = 0xd9,
    kMem_misc_poly_ped = 0xda,
    kMem_physics_joint = 0xdb,
    kMem_funk_spec = 0xdc,
    kMem_groove_spec = 0xdd,
    kMem_non_car_spec = 0xde,
    kMem_S3_scan_name = 0xdf,
    kMem_S3_sound_header = 0xe0,
    kMem_S3_sample = 0xe1,
    kMem_S3_mac_channel = 0xe2,
    kMem_S3_mac_path = 0xe3,
    kMem_S3_sentinel = 0xe4,
    kMem_S3_outlet = 0xe5,
    kMem_S3_channel_00593a58 = 0xe6,
    kMem_S3_descriptor = 0xe7,
    kMem_S3_reverse_buffer = 0xe8,
    kMem_S3_source = 0xe9,
    kMem_drone_specs = 0xea,
    kMem_drone_paths = 0xeb,
    kMem_S3_DOS_SOS_patch = 0xec,
    kMem_S3_DOS_SOS_song_structure = 0xed,
    kMem_S3_DOS_SOS_song_data = 0xee,
    kMem_S3_Windows_95_load_WAV_file = 0xef,
    kMem_S3_Windows_95_create_temp_buffer_space_to_reverse_sample = 0xf0,
    kMem_S3_Windows_95_path = 0xf1,
    kMem_drone_funk_groove = 0xf2,
    kMem_abuse_text = 0xf3,
    kMem_action_replay_buffer = 0xf4,
    kMem_master_crush_data = 0xf5,
    kMem_collision_object = 0xf6,
    kMem_collision_shape = 0xf7,
    kMem_smashable_info = 0xf8,
    kMem_smashable_env_info = 0xf9,
    kMem_detached_bit_non_car_spec = 0xfa,
    kMem_cloned_block = 0xfb,
    kMem_explosion_data = 0xfc,
    kMem_misc = 0xfd,
    kMem_exception = 0xfe,
};

enum {
    // FIXME: add all c2 FatalError types
    kFatalError_InvalidScreenDepthSetting = 0x00,
    kFatalError_FailToOpenGeneralSettings = 0x03,
    kFatalError_CouldNotOpenKeyMapFile = 0x09,
    kFatalError_CouldNotOpenFlicFile_S = 0x0d,
    kFatalError_FlicFileWasNot8BitsDeep_S = 0x0f,
    kFatalError_CouldNotAscertainFrameRateForFlicFile = 0x10,
    kFatalError_ScreenDimensionNotInGrafData = 0x18,
    kFatalError_CannotOpenRacesFile = 0x32,
    kFatalError_InsufficientPixelmapSlots = 0x43,
    kFatalError_CantLoadPixelmapFile_S = 0x4f,
    kFatalError_OOM_S = 0x5e,
    kFatalError_CannotOpenTEXT_TXT = 0x63,
    kFatalError_CannotFindFlicReferencedTranslation_S = 0x65,
    kFatalError_CantOpen_S = 0x6b,
    kFatalError_DuplicatePixelmap_S = 0xb5,
    kFatalError_CantCopeWithVersionFor_SS = 0xb1,
    kFatalError_FileMustStartWith_SS = 0xb2,
    kFatalError_WriteAttemptToPackedFile_S = 0xb6,
    kFatalError_CouldNotCreateTexturesPages_S = 0xb7,
    kFatalError_MysteriousX_SS = 0xbd,
};

#endif // REC2_TYPES_H
