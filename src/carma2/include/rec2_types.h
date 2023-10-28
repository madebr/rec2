#ifndef REC2_TYPES_H
#define REC2_TYPES_H

#include <brender/br_types.h>
#include <s3/s3.h>

#include "c2_stdio.h"

#define kMax_crush_datas 64

typedef char tPath_name[256];

typedef unsigned char tU8;
typedef signed char tS8;
typedef unsigned short tU16;
typedef signed short tS16;
typedef unsigned int tU32;
typedef signed int tS32;

typedef unsigned char undefined;
typedef unsigned short undefined2;
typedef unsigned int undefined4;

typedef int tTWTVFS;
typedef tU32 tPlayer_ID;

// FIXME: incomplete type
typedef struct tCar_spec tCar_spec;
typedef struct tCar_crush_spec tCar_crush_spec;
typedef struct tCollision_info tCollision_info;
typedef struct tNon_car_spec tNon_car_spec;
typedef struct tPath_section_struct tPath_section_struct;
typedef struct tPowerup tPowerup;
typedef struct tRace_list_spec tRace_list_spec;
typedef struct tMin_message tMin_message;
typedef struct tMid_message tMid_message;
typedef struct tMax_message tMax_message;
typedef union tCollision_shape tCollision_shape;

typedef char tPed_animal_name[50]; /* FIXME: should not really exist */

// Function callbacks are moved to a header for automatic SRE project generation
typedef void (C2_HOOK_FASTCALL * tPlayFlic_DoPerFrame)(void);
typedef void (C2_HOOK_FASTCALL * tPDForEveryFile_cbfn)(const char*);
typedef void (C2_HOOK_FASTCALL * tPDForEveryFileRecurse_cbfn)(const char*);
typedef int  C2_HOOK_FASTCALL tGot_proc(tPowerup*, tCar_spec*);
typedef void C2_HOOK_FASTCALL tLose_proc(tPowerup*, tCar_spec*);
typedef void C2_HOOK_FASTCALL tPeriodic_proc(tPowerup*, tU32);

typedef int tJoy_array[8];
typedef int tKey_array[151];

typedef struct {
    int r;
    int g;
    int b;
} tRGBColour;

typedef struct {
    tU32 total_duration;
    tU32 durations[1];
    tU8 index;
    undefined field_0x9[3];
    tU32 start_time;
    tU32 longest_duration;
    int colour;
    char identifier[4];
} tTimer;

typedef struct tToggle_element {
    int key1;
    int key2;
    int in_game_only;
    int exact_modifiers;
    int on_last_time;
    void (*action_proc)(void);
} tToggle_element;

typedef struct tCheat {
    tU32 code;
    tU32 code2;
    void (C2_HOOK_FASTCALL * action_proc)(int);
    int num;
} tCheat;

typedef struct {
    tRGBColour tl;
    tRGBColour tr;
    tRGBColour bl;
    tRGBColour br;
} tPolyFontBorderColours;

typedef struct tPolyFont tPolyFont;

typedef struct tText_chunk {
    int frame_cue;
    int frame_end;
    int x_coord;
    int y_coord;
    int line_count;
    char* text[8];
} tText_chunk;

typedef struct {
    char name[8];
} tTree_surgery_pass1;

typedef struct {
    char original[24];
    char replacement[24];
} tTree_surgery_pass2;

typedef enum {
    eSoftness_VerySoft = 0,
    eSoftness_Soft = 1,
    eSoftness_Normal = 2,
    eSoftness_Hard = 3,
    eSoftness_VeryHard = 4,
    eSoftness_Uncrushable = 5,
} tSoftness;

typedef enum {
    eCrushType_Boring = 0,
    eCrushType_Flap = 1,
    eCrushType_Detach = 2,
} tCrush_type;

typedef enum {
    eEaseOfDetachment_VeryEasy = 0,
    eEaseOfDetachment_Easy = 1,
    eEaseOfDetachment_Normal = 2,
    eEaseOfDetachment_Hard = 3,
    eEaseOfDetachment_VeryHard = 4,
} tEase_of_detachment;

typedef enum {
    eCarCrushShape_box = 0,
    eCarCrushShape_poly = 1,
} tCar_crush_shape;

typedef enum {
    eGroove_mode_constant = 0,
    eGroove_mode_distance = 1,
} tGroove_trigger_mode;

typedef enum {
    eGroove_path_none = -1,
    eGroove_path_straight = 0,
    eGroove_path_circular = 1,
} tGroove_path_mode;

typedef enum {
    eLollipop_none = -1,
    eLollipop_x_match = 0,
    eLollipop_y_match = 1,
    eLollipop_z_match = 2,
} tLollipop_mode;

typedef enum {
    eMove_none = -1,
    eMove_linear = 0,
    eMove_harmonic = 1,
    eMove_flash = 2,
    eMove_controlled = 3,
    eMove_absolute = 4,
    eMove_continuous = 5,
    eMove_texturebits = 6,
} tMove_mode;

typedef enum {
    eInterrupt_none = 0,
    eInterrupt_less_than = 1,
    eInterrupt_greater_than = 2,
} tInterrupt_status;

typedef enum {
    eGroove_object_none = -1,
    eGroove_object_spin = 0,
    eGroove_object_rock = 1,
    eGroove_object_throb = 2,
    eGroove_object_shear = 3,
} tGroove_object_mode;

typedef struct tDynamic_message {
    struct tDynamic_message* next;
    char data[];
} tDynamic_message;

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
    int field_0x8;
    int field_0xc;
    int field_0x10;
    int field_0x14;
    int field_0x18;
    int field_0x1c;
    int field_0x20;
    int field_0x24;
    int field_0x28;
    int field_0x2c;
    int field_0x30;
    int field_0x34;
    int field_0x38;
    int field_0x3c;
    int field_0x40;
    int field_0x44;
    int field_0x48;
    int field_0x4c;
    int field_0x50;
    int field_0x54;
    int field_0x58;
    int field_0x5c;
    int field_0x60;
    int field_0x64;
    int field_0x68;
    int field_0x6c;
    int field_0x70;
    int field_0x74;
    int field_0x78;
    int field_0x7c;
    int field_0x80;
    int field_0x84;
    int field_0x88;
    int field_0x8c;
    int field_0x90;
    int field_0x94;
    int field_0x98;
    int field_0x9c;
    int field_0xa0;
    int field_0xa4;
    int field_0xa8;
    int field_0xac;
    int field_0xb0;
    int field_0xb4;
    int field_0xb8;
    int field_0xbc;
    int field_0xc0;
    int field_0xc4;
    int field_0xc8;
    int field_0xcc;
    int field_0xd0;
    int field_0xd4;
    int field_0xd8;
    int field_0xdc;
    int field_0xe0;
    int field_0xe4;
    int field_0xe8;
    int field_0xec;
    int field_0xf0;
    int field_0xf4;
    int field_0xf8;
    int field_0xfc;
    int field_0x100;
    int cock_margin_x;
    int cock_margin_y;
    int field_0x10c;
    int field_0x110;
    int field_0x114;
    int field_0x118;
    int field_0x11c;
    int field_0x120;
    int field_0x124;
    int field_0x128;
    int field_0x12c;
    int field_0x130;
    int field_0x134;
    int field_0x138;
    int field_0x13c;
    int field_0x140;
    int field_0x144;
    int field_0x148;
    int field_0x14c;
    int field_0x150;
    int field_0x154;
    int field_0x158;
    int field_0x15c;
    int field_0x160;
    int field_0x164;
    int field_0x168;
    int field_0x16c;
    int field_0x170;
    int field_0x174;
    int field_0x178;
    int field_0x17c;
    int field_0x180;
    int field_0x184;
    int field_0x188;
    int field_0x18c;
    int field_0x190;
    int field_0x194;
    int field_0x198;
    int field_0x19c;
    int field_0x1a0;
    int field_0x1a4;
    int field_0x1a8;
    int field_0x1ac;
    int field_0x1b0;
    int field_0x1b4;
    int field_0x1b8;
    int field_0x1bc;
    int field_0x1c0;
    int field_0x1c4;
    int field_0x1c8;
    int field_0x1cc;
    int field_0x1d0;
    int field_0x1d4;
    int field_0x1d8;
    int field_0x1dc;
    int field_0x1e0;
    int field_0x1e4;
    int field_0x1e8;
    int field_0x1ec;
    int field_0x1f0;
    int field_0x1f4;
    int field_0x1f8;
    int field_0x1fc;
    int field_0x200;
    int field_0x204;
    int field_0x208;
    int field_0x20c;
    int field_0x210;
    int field_0x214;
    int field_0x218;
    int field_0x21c;
    int field_0x220;
    int field_0x224;
    int field_0x228;
    int field_0x22c;
    int field_0x230;
    int field_0x234;
    int field_0x238;
    int field_0x23c;
    int field_0x240;
    int field_0x244;
    int field_0x248;
    int field_0x24c;
    int field_0x250;
    int field_0x254;
    int field_0x258;
    int field_0x25c;
    int field_0x260;
    int field_0x264;
    int field_0x268;
    int field_0x26c;
    int field_0x270;
    int field_0x274;
    int field_0x278;
    int field_0x27c;
    int field_0x280;
    int field_0x284;
    int field_0x288;
    int field_0x28c;
    int field_0x290;
    int field_0x294;
    int field_0x298;
    int field_0x29c;
    int field_0x2a0;
    int field_0x2a4;
    int field_0x2a8;
    int field_0x2ac;
    int field_0x2b0;
    int field_0x2b4;
    int field_0x2b8;
    int field_0x2bc;
    int field_0x2c0;
    int field_0x2c4;
    int field_0x2c8;
    int field_0x2cc;
    int field_0x2d0;
    int field_0x2d4;
    int field_0x2d8;
    int field_0x2dc;
    int field_0x2e0;
    int field_0x2e4;
    int field_0x2e8;
    int field_0x2ec;
    int field_0x2f0;
    int field_0x2f4;
    int field_0x2f8;
    int field_0x2fc;
    int field_0x300;
    int field_0x304;
    int field_0x308;
    int field_0x30c;
    int field_0x310;
    int field_0x314;
    int field_0x318;
    int field_0x31c;
    int field_0x320;
    int field_0x324;
    int field_0x328;
    int field_0x32c;
    int field_0x330;
    int field_0x334;
    int field_0x338;
    int field_0x33c;
    int field_0x340;
    int field_0x344;
    int field_0x348;
    int field_0x34c;
    int field_0x350;
    int field_0x354;
    int field_0x358;
    int field_0x35c;
    int field_0x360;
    int field_0x364;
    int field_0x368;
    int field_0x36c;
    int field_0x370;
    int field_0x374;
    int field_0x378;
    int field_0x37c;
    int field_0x380;
    int field_0x384;
    int field_0x388;
    int field_0x38c;
    int field_0x390;
    int field_0x394;
    int field_0x398;
    int field_0x39c;
    int field_0x3a0;
    int field_0x3a4;
    int field_0x3a8;
    int field_0x3ac;
    int field_0x3b0;
    int field_0x3b4;
    int field_0x3b8;
    int field_0x3bc;
    int field_0x3c0;
    int field_0x3c4;
    int field_0x3c8;
    int field_0x3cc;
    int field_0x3d0;
    int field_0x3d4;
    int field_0x3d8;
    int field_0x3dc;
    int field_0x3e0;
    int field_0x3e4;
    int field_0x3e8;
    int field_0x3ec;
    int field_0x3f0;
    int field_0x3f4;
    int field_0x3f8;
    int field_0x3fc;
    int field_0x400;
    int field_0x404;
    int field_0x408;
    int field_0x40c;
    int field_0x410;
    int field_0x414;
    int field_0x418;
    int field_0x41c;
    int field_0x420;
    int field_0x424;
    int field_0x428;
    int field_0x42c;
    int field_0x430;
    int field_0x434;
    int field_0x438;
    int field_0x43c;
    int field_0x440;
    int field_0x444;
    int field_0x448;
    int field_0x44c;
    int field_0x450;
    int field_0x454;
    int field_0x458;
    int field_0x45c;
    int field_0x460;
    int field_0x464;
    int field_0x468;
    int field_0x46c;
    int field_0x470;
    int field_0x474;
    int field_0x478;
    int field_0x47c;
    int field_0x480;
    int field_0x484;
    int field_0x488;
    int field_0x48c;
    int field_0x490;
    int field_0x494;
    int field_0x498;
    int field_0x49c;
    int field_0x4a0;
    int field_0x4a4;
    int field_0x4a8;
    int field_0x4ac;
    int field_0x4b0;
    int field_0x4b4;
    int field_0x4b8;
    int field_0x4bc;
    int field_0x4c0;
    int field_0x4c4;
    int field_0x4c8;
    int field_0x4cc;
    int field_0x4d0;
    int field_0x4d4;
    int field_0x4d8;
    int field_0x4dc;
    int field_0x4e0;
    int field_0x4e4;
    int field_0x4e8;
    int field_0x4ec;
    int field_0x4f0;
    int field_0x4f4;
    int field_0x4f8;
    int field_0x4fc;
    int field_0x500;
    int field_0x504;
    int field_0x508;
    int field_0x50c;
    int field_0x510;
    int field_0x514;
    int field_0x518;
    int field_0x51c;
} tGraf_data;

typedef enum {
    kPixelFlags_16bbp = 0x1,
    kPixelFlags_unknown = 0x2,
    kPixelFlags_unknown_0x4 = 0x4,
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
    tU32 flags;
    int models_count;
    int materials_count;
    int shade_tables_count;
    int pixelmaps_count;
    int sounds_count;
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
    eNot_ever_ever_ever = 3,
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
    br_pixelmap* screen_pixelmap;
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

typedef struct {
    br_pixelmap* images;
    int file_read_once;
    int height;
    int width;
    int spacing;
    int offset;
    int num_entries;
    int id;
    int width_table[224];
} tDR_font;

typedef enum {
    eHeadup_unused = 0,
    eHeadup_text = 1,
    eHeadup_coloured_text = 2,
    eHeadup_image = 3,
    eHeadup_fancy = 4,
    eHeadup_box_text = 5,
} tHeadup_type;

typedef struct tHeadup {
    tHeadup_type type;
    int x;
    int y;
    int original_x;
    int right_edge;
    int flash_period;
    int slot_index;
    int dimmed_background;
    int dim_left;
    int dim_top;
    int dim_right;
    int dim_bottom;
    int clever;
    int cockpit_anchored;
    int flash_state;
    int field_0x3c;
    tJustification justification;
    tU32 end_time;
    tU32 last_flash;
    union {
        struct {
            char text[252];
            int colour;
            br_font* font;
        } text_info;
        struct {
            char text[252];
            tDR_font* coloured_font;
        } coloured_text_info;
        struct {
            undefined field_0x0[280];
        } unknown;
    } data;
} tHeadup;

typedef struct {
    int flash_rate;
    int lifetime;
    int font_index;
    char text[256];
} tQueued_headup;

typedef struct {
    float opacity;
    br_pixelmap* map;
} tExplosion_frame;

typedef struct {
    tU32 start;
    tU32 period;
    undefined field_0x8;
    tU8 count_frames;
    tU8 finished;
    undefined field_0xb;
    br_actor* collision_actor;
    br_actor* actor;
    float field_0x14;
    br_angle angle;
    undefined padding[2];
    br_vector3 field_0x1c;
    tExplosion_frame frames[10];
} tExplosion;

typedef enum {
    kRendererShadingType_Undefined = -1,
    kRendererShadingType_Default = 1,
    kRendererShadingType_Diffuse1 = 3,
    kRendererShadingType_Specular = 2,
    kRendererShadingType_Diffuse2 = 0,
    kRendererShadingType_AmbientOnly = 4,
} tRendererShadingType;

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
    int x_coord;
    int y_coord;
    int damage_level;
    int last_level;
    int smoke_last_level;
    int periods[5];
    br_pixelmap* images;
} tDamage_unit;

typedef enum {
    eImpact_top = 0,
    eImpact_bottom = 1,
    eImpact_left = 2,
    eImpact_right = 3,
    eImpact_front = 4,
    eImpact_back = 5,
    eImpact_location_count = 6,
    eImpact_unknown = 7
} tImpact_location;

typedef enum {
    eAxis_x = 0,
    eAxis_y = 1,
    eAxis_z = 2
} tAxis_comp;

typedef enum {
    eCondition_less_than = 0,
    eCondition_greater_than = 1
} tCondition_operator;

typedef struct {
    tAxis_comp axis_comp;
    int condition_operator;
    float comparitor;
} tDamage_condition;

typedef enum {
    eDamage_engine = 0,
    eDamage_transmission = 1,
    eDamage_driver = 2,
    eDamage_steering = 3,
    eDamage_lf_brake = 4,
    eDamage_rf_brake = 5,
    eDamage_lr_brake = 6,
    eDamage_rr_brake = 7,
    eDamage_lf_wheel = 8,
    eDamage_rf_wheel = 9,
    eDamage_lr_wheel = 10,
    eDamage_rr_wheel = 11
} tDamage_type;

typedef struct  {
    tDamage_type type;
    float weakness_factor;
} tDamage_effect;

typedef struct {
    tDamage_condition conditions[2];
    int effect_count;
    int condition_count;
    tDamage_effect effects[4];
} tDamage_clause;

typedef struct {
    int clause_count;
    tDamage_clause* clauses;
} tDamage_program;

typedef enum tDriver {
    eDriver_oppo = 6,
    eDriver_net_human = 7,
    eDriver_local_human = 8
} tDriver;

typedef enum {
    eView_undefined = 0,
    eView_left = 1,
    eView_forward = 2,
    eView_right = 3
} tWhich_view;

typedef enum {
    eProg_intro = 0,
    eProg_opening = 1,
    eProg_idling = 2,
    eProg_demo = 3,
    eProg_game_starting = 4,
    eProg_game_ongoing = 5,
    eProg_quit = 6
} tProg_status;

typedef struct {
    br_vector3 trail_nodes[25];
    br_vector3 base_heading;
    tU32 time_of_next_recording;
    tU32 end_of_deviation;
    tU8 number_of_nodes;
    tU8 has_deviated_recently;
    tU8 nodes_shifted_this_frame;
    undefined field_0x143;
} tPursuee_trail;

typedef struct {
    br_material* material;
    int count_maps;
    br_pixelmap* maps[3];
} tCarCockpitWindowThing;

typedef struct tCar_spec {
    int index;
    int disabled;
    tCollision_info* collision_info;
    tDriver driver;
    br_actor* car_master_actor;
    undefined field_0x14[8];
    br_matrix34 old_frame_mat;
    br_vector3 pos;
    br_vector3 centre_of_mass_world_scale;
    int frame_collision_flag;
    undefined field_0x68[8];
    br_vector3 direction;
    float speed;
    short car_ID;
    undefined field_0x82;
    undefined field_0x83;
    br_material* shrapnel_material[3];
    undefined field_0x90[28];
    tU16 fire_vertex[12];
    float dt;
    undefined* field_0xc8;
    char name[32];
    char driver_name[32];
    char grid_icon_names[3][14];
    undefined field_0x136;
    undefined field_0x137;
    undefined4* cockpit_images[3];
    br_pixelmap* prat_cam_left;
    br_pixelmap* prat_cam_top;
    br_pixelmap* prat_cam_right;
    br_pixelmap* prat_cam_bottom;
    undefined field_0x154[4];
    br_pixelmap* speedo_image[2];
    br_pixelmap* tacho_image[2];
    br_pixelmap* damage_background;
    br_pixelmap* lhands_images[7];
    br_pixelmap* rhands_images[7];
    br_pixelmap* grid_icon_image;
    br_pixelmap* gears_image;
    int fg_index;
    int underwater_ability;
    int invulnerable1;
    int invulnerable2;
    undefined field_0x1bc[4];
    int is_race_goal;
    int wall_climber_mode;
    int can_be_stolen;
    int has_been_stolen;
    int active;
    int knackered;
    int pre_car_col_knackered;
    int render_left[3];
    int render_top[3];
    int render_right[3];
    int render_bottom[3];
    int mirror_left;
    int mirror_top;
    int mirror_right;
    int mirror_bottom;
    int prat_left;
    int prat_top;
    int prat_right;
    int prat_bottom;
    int speedo_x[2];
    int speedo_y[2];
    int speedo_centre_x[2];
    int speedo_centre_y[2];
    int speedo_x_pitch[2];
    int speedo_y_pitch[2];
    int speedo_radius_1[2];
    int speedo_radius_2[2];
    int speedo_start_angle[2];
    int speedo_end_angle[2];
    int speedo_needle_colour[2];
    int tacho_x[2];
    int tacho_y[2];
    int tacho_centre_x[2];
    int tacho_centre_y[2];
    int tacho_x_pitch[2];
    int tacho_y_pitch[2];
    int tacho_radius_1[2];
    int tacho_radius_2[2];
    int tacho_start_angle[2];
    int tacho_end_angle[2];
    int tacho_needle_colour[2];
    int gear_x[2];
    int gear_y[2];
    int red_line;
    int lhands_x[7];
    int lhands_y[7];
    int rhands_x[7];
    int rhands_y[7];
    int number_of_hands_images;
    int max_speed;
    int damage_x_offset;
    int damage_y_offset;
    int damage_background_x;
    int damage_background_y;
    int dim_count[2];
    int dim_left[2][4];
    int dim_top[2][4];
    int dim_right[2][4];
    int dim_bottom[2][4];
    undefined field_0x400[4];
    int number_of_steerable_wheels;
    int steering_ref[6];
    int lf_sus_ref[4];
    int rf_sus_ref[4];
    int lr_sus_ref[2];
    int rr_sus_ref[2];
    int driven_wheels_spin_ref_1;
    int driven_wheels_spin_ref_2;
    int driven_wheels_spin_ref_3;
    int driven_wheels_spin_ref_4;
    int non_driven_wheels_spin_ref_1;
    int non_driven_wheels_spin_ref_2;
    int non_driven_wheels_spin_ref_3;
    int non_driven_wheels_spin_ref_4;
    int engine_noises[3];
    float driver_x_offset;
    float driver_y_offset;
    float driver_z_offset;
    float mirror_x_offset;
    float mirror_y_offset;
    float mirror_z_offset;
    float rearview_camera_angle;
    float head_left_angle;
    float head_right_angle;
    float steering_angle;
    float speedo_speed;
    float lf_sus_position;
    float rf_sus_position;
    float lr_sus_position;
    float rr_sus_position;
    float driven_wheels_circum;
    float non_driven_wheels_circum;
    float bounce_rate;
    float bounce_amount;
    undefined field_0x4c8[4];
    float damage_multiplier;
    float grip_multiplier;
    undefined field_0x4d4[4];
    tDamage_unit damage_units[12];
    int frame_start_damage[3];
    float last_impact_location;
    tDamage_program damage_programs[6];
    tHeadup_slot headup_slots[2][20];
    undefined field_0xe08[4];
    br_actor* car_model_actor;
    br_actor* car_actor;
    int count_detail_levels;
    float field_0xe18;
    float detail_levels[2];
    int use_shell_model;
    br_model* shell_model;
    undefined field_0xe2c[12];
    br_matrix34 last_safe_positions[5];
    undefined field_0xf28[720];
    int wheel_slip;
    float damping;
    undefined field_0x1200[16];
    float steerable_suspension_give;
    float susp_give;
    float susp_height[2];
    float maybe_ride_height;
    br_vector3 wpos[4];
    float curvature;
    float maxcurve;
    float turn_speed;
    float field_0x1260;
    int oldd[4];
    int material_index[4];
    undefined field_0x1284[16];
    br_vector3 mu;
    float traction_fractional_multiplier;
    float downforce_to_weight;
    undefined field_0x12a8;
    undefined field_0x12a9;
    undefined field_0x12aa;
    undefined field_0x12ab;
    float initial_brake;
    float brake_increase;
    float friction_slipping_reduction;
    float acc_force;
    float torque;
    float brake_force;
    int traction_control;
    float steerable_rolling_resistance;
    float rolling_resistance;
    tU32 keys;
    undefined field_0x12d4[8];
    int joystick_acc;
    int joystick_dec;
    undefined field_0x12e4[4];
    int number_of_wheels_on_ground;
    br_actor* wheel_actors[6];
    br_actor* pivot_actors[6];
    undefined field_0x131c[32];
    float damage_magnitude_accumulator;
    float revs;
    float target_revs;
    undefined field_0x1348[12];
    float max_force_front;
    float max_force_rear;
    float gear;
    int just_changed_gear;
    int max_gear;
    float speed_revs_ratio;
    float force_torque_ratio;
    undefined4 sound_source;
     br_matrix34 pre_car_col_mat;
    float pre_car_col_speed;
    float pre_car_col_direction;
    float field_0x13ac;
    float field_0x13b0;
    br_vector3 pre_car_col_velocity;
    float pre_car_col_velocity_car_space;
    float field_0x13c4;
    float field_0x13c8;
    undefined field_0x13cc[20];
    float field_0x13e0;
    int time_last_hit;
    int time_last_victim;
    undefined* last_hit_by;
    undefined* field_0x13f0;
    int no_of_processes_recording_my_trail;
    tPursuee_trail my_trail;
    int flags;
    undefined field_0x1540[8];
    undefined4 field_0x1548;
    undefined field_0x154c[20];
    struct tCar_spec* last_person_to_hit_us;
    struct tCar_spec* last_person_we_hit;
    undefined field_0x1568[44];
    int shadow_intersection_flags;
    tU32 last_bounce;
    int new_skidding;
    undefined field_0x15a0[12];
    br_vector3 field_0x15ac[4];
    undefined field_0x15dc[240];
    float oil_remaining[4];
    float maybe_blood_remaining[4];
    float maybe_total_length[4];
    undefined4 field_0x16fc[4];
    float proxy_ray_distance;
    int powerups[80]; /* unknown_size */
    undefined field_0x1850[80];
    int time_to_recover;
    undefined field_0x18a4[4];
    int power_up_levels[3];
    int power_up_slots[3];
    int horn_sound_tag;
    int is_girl;
    undefined field_0x18c8[4];
    int field_0x18cc;
    float softness_factor;
    tCar_crush_spec* car_crush_spec;
    br_vector3 camera_bumper_position;
    br_vector3 camera_cockpit_position;
    int count_window_things;
    tCarCockpitWindowThing window_things[5];
    tU16 old_material_count;
    tU16 new_material_count;
    undefined4 field_0x195c;
    undefined field_0x1960[4];
} tCar_spec;

typedef struct {
    tU8 ncolumns_x;
    tU8 ncolumns_z;
    undefined field_0x2;
    undefined field_0x3;
    float column_size_x;
    float column_size_z;
    float origin_x;
    float origin_z;
    br_actor* the_actor; /* for newcity1 ==> actor from newcity1.twt/newcity1.act */
    undefined4* columns;
    int count_non_cars;
    br_actor** non_car_list;
} tTrack_spec;

typedef enum {
    /* eDepth_effect_none = -1, */
    eDepth_effect_darkness = 0,
    eDepth_effect_fog = 1,
    eDepth_effect_colour = 2,
} tDepth_effect_type;

typedef struct {
    int red;
    int green;
    int blue;
} tRGB_colour;

typedef struct {
    tDepth_effect_type type;
    int start;
    int end;
    tRGB_colour colour;
    br_pixelmap* sky_texture;
} tDepth_effect;

typedef struct {
    br_material* material;
    float min_x;
    float min_z;
    float max_x;
    float max_z;
} tSpecial_screen;

typedef enum {
    kSoundGeneratorType_noncar = 0,
    kSoundGeneratorType_actor = 1,
    kSoundGeneratorType_point = 2,
} tSoundGeneratorType;

typedef struct {
    tSoundGeneratorType type;
    tSpecial_volume_soundfx_data fx;
    tSpecial_volume_soundfx_data fx1_noncar;
    tSpecial_volume_soundfx_data fx2_noncar;
    br_vector3 point;
} tTrackSoundGenerator;

typedef enum {
    eOOT_none = 0,
    eOOT_complete_race = 1,
    eOOT_pursue_and_twat = 2,
    eOOT_run_away = 3,
    eOOT_get_near_player = 4,
    eOOT_levitate = 5,
    eOOT_knackered_and_freewheeling = 6,
    eOOT_frozen = 7,
    eOOT_wait_for_some_hapless_sod = 8,
    eOOT_return_to_start = 9
} tOpponent_objective_type;

typedef struct {
    tS16 section_no;
    tU8 direction;
    undefined field_0x3;
} tRoute_section;

typedef struct {
    tU8 finished_calcing_race_route;
    tU8 found_race_section;
} tComplete_race_data;

typedef struct tReturn_to_start_data {
    br_vector3 nearest_path_point;
    tS16 section_no;
    tU8 waiting_near_start;
} tReturn_to_start_data;

typedef struct {
    int index;
    tOpponent_objective_type current_objective;
    tCar_spec* car_spec;
    float nastiness;
    float distance_to_camera;
    float distance_from_home;
    float player_to_oppo_d;
    br_vector3 start_pos;
    br_vector3 start_direction;
    undefined field9_0x34[12];
    br_vector3 player_to_oppo_v;
    tU32 next_out_of_world_check;
    tU32 next_repair_check;
    tU32 repair_interval;
    tU32 last_repair_time;
    tU32 stun_time_ends;
    tU32 next_player_visibility_check;
    undefined field17_0x64[4];
    tU32 last_in_view;
    undefined field19_0x6c[4];
    tU32 time_this_objective_started;
    tU32 time_for_this_objective_to_finish;
    undefined field22_0x78[4];
    int nnext_sections;
    tRoute_section next_sections[11];
    undefined field25_0xac[4];
    unsigned int flags;
    undefined field27_0xb4[4];
    tComplete_race_data complete_race_data;
    undefined field29_0xba[2];
    /* FIXME: START tFollow_path_data */
    tU32 follow_path_data__struggle_time;
    int follow_path_data__last_finished_struggle_time;
    undefined4 field32_0xc4;
    undefined field33_0xc8[4];
    float follow_path_data__prev_acc;
    float follow_path_data__desired_speed;
    float follow_path_data__desired_speed2;
    float follow_path_data__last_distance;
    br_vector3 follow_path_data__cheaty_intersect;
    tS16 follow_path_data__section_no;
    tS16 follow_path_data__first_section_no;
    tS16 follow_path_last__struggle_section;
    undefined field42_0xee[2];
    undefined4 field43_0xf0;
    undefined4 field44_0xf4;
    undefined field45_0xf8[4];
    float field46_0xfc;
    float field47_0x100;
    float field48_0x104;
    float field49_0x108;
    float follow_path_data__corner_width;
    int field51_0x110;
    /* FIXME: start tPursue_car_data */
    tCar_spec* pursue_car_data__pursuee;
    undefined field53_0x118[16];
    tU32 time_last_away_from_pursuee;
    br_vector3 pursue_car_data__direct_line_nodes_p;
    undefined field56_0x138[60];
    float pursue_car_data__direct_line_section__width;
    /* FIXME: missing:
        tLevitate_data levitate_data;
        tRun_away_data run_away_data;
    */
    undefined field58_0x178[24];
    tReturn_to_start_data return_to_start_data;
    undefined field63_0x1a3;
} tOpponent_spec;

typedef struct {
    undefined4 number_of_opponents;
    undefined4 number_of_cops;
    undefined4 number_of_path_nodes;
    undefined4 number_of_path_sections;
    br_vector3 cop_start_points[20];
    tOpponent_spec opponents[40];
    undefined4 path_nodes;
    tPath_section_struct* path_sections;
} tIntelligent_vehicles;

typedef enum {
    eVT_Scene = 0,
    eVT_Info = 1,
    eVT_Opponents = 2
} tRace_sel_view_type;

typedef enum tFrank_anne {
    eFrankie = 0,
    eAnnie = 1
} tFrank_anne;

typedef struct {
    int credits;
    tU32 view_change_start;
    tU32 pratcam_move_start;
    int peds_killed;
    int sausage_eater_mode;
    int rank;
    int field_0x18;
    int last_slot;
    int skill_level;
    undefined4 field_0x24;
    int racing;
    int field_0x2c;
    int cut_scene;
    int saving;
    int loading;
    int dont_save_or_load;
    int dont_load;
    int mirror_on;
    int prat_cam_on;
    int cockpit_on;
    int cockpit_image_index;
    int current_render_left;
    int current_render_top;
    int current_render_right;
    int current_render_bottom;
    int frame_rate_headup;
    int field26_0x68;
    int revs;
    int music_volume;
    int effects_volume;
    int current_race_index;
    int redo_race_index;
    int credits_per_rank;
    int game_completed;
    int number_of_cars;
    int current_car_index;
    tWhich_view which_view;
    tWhich_view new_view;
    tWhich_view pending_view;
    tWhich_view old_view;
    tRace_sel_view_type view_type;
    tProg_status prog_status;
    tFrank_anne frank_or_anniness;
    tCar_spec current_car;
    char player_name[14];
    char track_file_name[64];
    char car_name[64];
    char padding_0x1a9e[2];
    int cars_available[60];
    br_vector3 initial_position;
    float initial_yaw;
    tTrack_spec track_spec;
    tDepth_effect default_depth_effect;
    tDepth_effect current_depth_effect;
    int special_volume_count;
    tSpecial_volume* special_volumes;
    int count_track_sound_generators;
    tTrackSoundGenerator* track_sound_generators;
    br_pixelmap* field59_0x1c0c;
    br_pixelmap* field60_0x1c10;
    br_pixelmap* field61_0x1c14;
    int special_screens_count;
    tSpecial_screen* special_screens;
    tIntelligent_vehicles AI_vehicles;
    tNon_car_spec* non_cars;
    int num_non_car_spaces;
} tProgram_state;

typedef struct {
    tPowerup* powerup;
    int fizzle_stage;
    int fizzle_direction;
    tU32 fizzle_start;
} tHeadup_icon;

typedef enum {
    ePowerup_dummy = 0,
    ePowerup_instantaneous = 1,
    ePowerup_timed = 2,
    ePowerup_whole_race = 3,
} tPowerup_type;

typedef enum {
    eNet_powerup_local = 0,
    eNet_powerup_global = 1,
    eNet_powerup_inappropriate = 2,
    eNet_powerup_off_before_snapweld = 4,
} tNet_powerup_type;

typedef struct tPowerup{
    tPowerup_type type;
    undefined4 got_time;
    int duration;
    int lose_time;
    int group_inclusion;
    struct br_pixelmap * icon;
    int fizzle_type;
    int number_of_float_params;
    int number_of_integer_params;
    int* integer_params;
    int current_value;
    int prat_cam_event;
    int value;
    int initial_value;
    tNet_powerup_type net_type;
    tGot_proc* got_proc;
    tLose_proc* lose_proc;
    tPeriodic_proc* periodic_proc;
    float * float_params;
    tCar_spec* car;
    char message[64]; /* name is written here */
    br_actor* icon_actor;
    br_token_value material_prims[3];
} tPowerup;

typedef struct {
    int initial[3];
    int initial_network[8];
} tSlot_info;

typedef struct {
    float initial[3];
    float initial_network[8];
} tFloat_bunch_info;

typedef struct {
    float opacity;
    br_pixelmap* map;
} tExplosion_animation_frame;

typedef enum {
    kExplosionRotateModus_NoRotate = 0,
    kExplosionRotateModus_RandomRotate = 1,
} tExplosion_animation_rotate_modus;

typedef struct {
    tU16 min_count;
    tU16 max_count;
    tU16 count_frames;
    tU16 min_start_delay_ms;
    tU16 max_start_delay_ms;
    tU16 min_frametime_ms;
    tU16 max_frametime_ms;
    char padding_0xe[2];
    float min_scaling_factor;
    float max_scaling_factor;
    br_vector3 min_xyz_factor;
    br_vector3 max_xyz_factor;
    br_vector3 offset;
    tExplosion_animation_rotate_modus rotate_modus;
    tExplosion_animation_frame* frames;
} tExplosion_animation_group;

typedef struct {
    int count_groups;
    tExplosion_animation_group* groups;
} tExplosion_animation;

typedef struct {
    tU8* bools;
} tPowerup_respawn_spec;

typedef enum {
    kRepeatability_None,
    kRepeatability_SingleShot,
    kRepeatability_Repeated,
    kRepeatability_DoItRegardless,
} tRepeatability;

typedef enum {
    kShrapnelRelPos_SphereClumped = 0,
    kShrapnelRelPos_BoxClumped = 1,
    kShrapnelRelPos_ActorBased = 2
} tShrapnel_rel_pos_type;

typedef struct {
    tShrapnel_rel_pos_type type;
    br_vector3 pos;
} tShrapnel_pos;

typedef enum {
    kShrapnelType_Abstract = 0,
    kShrapnelType_Shards = 1,
    kShrapnelType_GhostParts = 2,
    kShrapnelType_NonCars = 3,
} tShrapnel_type;

typedef struct {
    tShrapnel_type type;
    float min_speed_towards_you;
    float max_speed_towards_you;
    float impactee_velocity_factor;
    float random_velocity_max;
    float random_up_velocity_max;
    float random_normal_velocity_max;
    float random_spin_rate_max;
    float min_time_ms;
    float max_time_ms;
    tShrapnel_pos initial_pos;
    union {
        struct {
            br_material* material;
            float field_0x4;
            int field_0x8;
        } material;
        struct {
            int count;
            int invalid_count;
            int actor_count;
            float bounds_dx;
            float bounds_dy;
            br_actor* actor;
        } ghost;
        struct {
            int amount_min;
            int amount_max;
            int count_separate_actors;
            br_actor* actors;
            int probability_fire;
            int count_fire_columns;
            int min_smokiness;
            int max_smokiness;
        } noncar;
    } type_info;
} tShrapnel_spec;

typedef enum {
    kSmashableTrigger_Material = 0,
    kSmashableTrigger_Model = 1,
    kSmashableTrigger_Number = 2,
} tSmashable_trigger_type;

typedef struct {
    short field_0x0;
    short field_0x2;
} tSmash_run_time_variable_change;

typedef enum {
    kSmashableMode_NoChange = 0,
    kSmashableMode_Decal = 1,
    kSmashableMode_TextureChange = 2,
    kSmashableMode_Remove = 3,
    kSmashableMode_ReplaceModel = 4,
    kSmashableMode_Crush = 5,
} tSmashable_item_mode;

typedef struct {
    int field_0x0;
    float field_0x4;
    float field_0x8;
    float field_0xc;
    float field_0x10;
    float field_0x14;
    float field_0x18;
    float field_0x1c;
    float field_0x20;
    float field_0x24;
    br_material* material;
    br_material* material2;
} tSlick_spec;

typedef struct {
    int count_activations;
    struct tNon_car_cuboid_activation* activations;
} tNon_car_cuboid_activations;

typedef struct {
    int count_side_effects;
    struct tShrapnel_side_effect* side_effects;
} tShrapnel_side_effects;

typedef struct {
    tRepeatability frequency;
    float field_0x4;
    float field_0x8;
    int field_0xc;
    int field_0x10;
} tAward_info;

typedef struct {
    int flags;
    union {
        struct {
            tU8 field_0x0;
            tU8 field_0x1;
            undefined field_0x2;
            undefined field_0x3;
        } number;
        br_actor* actor;
        br_material* material;
    } trigger_object;
    tSmashable_trigger_type trigger_type;
    tSmashable_item_mode mode;
    undefined4 field_0x10;
    undefined4 field_0x14;
    int count_sounds;
    int sounds[3];
    int count_shrapnel;
    tShrapnel_spec shrapnel[6];
    tExplosion_animation explosion_animation;
    tSlick_spec slick;
    tNon_car_cuboid_activations activations;
    tShrapnel_side_effects side_effects;
    int extension_arg;
    int extension_flags;
    int room_turn_on_code;
    tRepeatability award_code;
    float award_arg1_f;
    float award_arg2_f;
    int award_arg3_i;
    int award_arg4_i;
    undefined field_0x2a4[4];
    tU8 count_variable_changes;
    undefined field27_0x2a9;
    tSmash_run_time_variable_change variable_changes[5];
    undefined field29_0x2be;
    undefined field30_0x2bf;
    br_model* replace_model;
    int replace_modelchance_fire;
    int replace_model_2_int;
    undefined field_0x2cc[8];
    undefined4 field_0x2d4;
    undefined4 field_0x2d8;
    undefined field_0x2dc;
    undefined field_0x2dd[3];
} tSmashable_item_spec;

typedef struct {
    tRGB_colour directional;
    float ambient_else;
    float diffues_else;
    float ambient_1;
    float diffuse_1;
    float ambient_0;
    float diffuse_0;
} tDRLighting_data;

typedef struct tMaterial_exception {
    struct tMaterial_exception* next;
    char* texture_name;
    int flags;
} tMaterial_exception;

typedef enum {
    eMaterial_exception_nobilinear = 0x1,
    eMaterial_exception_wrap = 0x2,
} tMaterial_exception_flags;

typedef struct {
    undefined4 field_0x0;
    undefined4 field_0x4;
    undefined4 field_0x8;
    float field_0xc;
    float field_0x10;
    float field_0x14;
    float field_0x18;
    struct br_material* material;
    struct br_model* model;
} tSmoke_info;

typedef struct {
    br_actor* actor;
    br_model* model;
    undefined4 field_0x8;
    undefined field_0xc[44];
} tSmash_vertex;

typedef struct {
    br_actor* actor;
    undefined4 field_0x4;
} tSmash_quad;

typedef struct {
    br_actor* actor;
    br_actor* car_actor;
    br_model* car_model;
    tCar_spec* car;
    tU32 spill_time;
    undefined4 field_0x14;
    float full_size;
    float field_0x1c;
    float field_0x20;
    float field_0x24;
    br_vector3 original_pos;
    br_vector3 field_0x34;
    float field_0x40;
    float field_0x44;
    float field_0x48;
    float field_0x4c;
    undefined4 field_0x50;
} tOil_spill_info;

typedef struct {
    br_actor* actor;
    br_vector3 normal;
    br_vector3 pos;
} tSkid;

typedef enum {
    kInitialSmashablePosition_SphereClumped = 0,
    kInitialSmashablePosition_BoxClumped = 1,
    kInitialSmashablePosition_ActorBased = 2,
} tInitial_smashable_position_type;

typedef enum {
    kSmashableSpherePosition_Impact = 0,
    kSmashableSpherePosition_Model = 0,
} tSmashable_sphere_position;

typedef struct {
    tInitial_smashable_position_type type;
    union {
        struct {
            float radius;
            tSmashable_sphere_position where;
        } sphere;
        br_vector3 box;
    } position;
} tSmashable_initial_position_spec;

typedef struct {
    undefined4 (C2_HOOK_FASTCALL * field_0x0)(undefined4*, undefined4, undefined4*);
    undefined4 (C2_HOOK_FASTCALL * field_0x4)(undefined4*, undefined4*);
    void (C2_HOOK_FASTCALL * field_0x8)(undefined4*, undefined4*);
    int (C2_HOOK_FASTCALL * field_0xc)(undefined4*);
    void (C2_HOOK_FASTCALL * field_0x10)(undefined4*, undefined4);
    void (C2_HOOK_FASTCALL * field_0x14)(undefined4*, FILE*);
    void (C2_HOOK_FASTCALL * field_0x18)(undefined4*);
    void (C2_HOOK_FASTCALL * field_0x1c)(undefined4*, FILE*);
    void (C2_HOOK_FASTCALL * field_0x20)(undefined4*);
} tPedForms_vtable;

typedef struct {
    float min_speed;
    float max_speed;
    float impactee_velocity_factor;
    float random_velocity_max;
    float random_up_velocity_max;
    float random_normal_velocity_max;
    float random_spin_rate_max;
} tSmashable_initial_speed_spec;

typedef enum {
    eNonCarLocation_Relative = 0,
    eNonCarLocation_Absolute = 1
} tNon_car_location_type;

typedef struct {
    undefined2 time_0x0;
    undefined2 time_0x2;
    tNon_car_location_type location_type;
    int field_0x8; /* can be -1 in file */
    br_bounds3 bounds;
    tSmashable_initial_speed_spec speed_info;
} tNoncar_activation;

typedef enum {
    kTextureLevelCollisionChange_Solid = 0,
    kTextureLevelCollisionChange_PassThrough = 1,
    kTextureLevelCollisionChange_Edges = 2
} tSmashable_texture_level_collision_type;

typedef struct {
    br_pixelmap * pixelmaps[3];
    int count_pixelmaps;
    float trigger_threshold;
    int count_sounds;
    int sounds[3];
    int count_shrapnels;
    tShrapnel_spec shrapnels[6];
    tExplosion_animation animation;
    tSlick_spec slick;
    int count_noncar_activations;
    tNoncar_activation* noncar_activations;
    tShrapnel_side_effects side_effects;
    int field_0x280;
    int extension_flags;
    int room_turn_on_code;
    tAward_info award;
    undefined field_0x2a0[4];
    tU8 count_runtime_variable_changes;
    undefined field_0x2a5;
    tSmash_run_time_variable_change runtime_variable_changes[1]; /* FIXME: unknown size */
    undefined field_0x2aa[18];
    int flags;
    tSmashable_texture_level_collision_type collision_type;
} tSmashable_level;

typedef struct  {
    undefined field_0x0[2];
    tU16 field_0x2;
    tU16 field_0x4;
    tU16 field_0x6;
    tU16 field_0x8;
    tU16 hinge0;
    tU16 field_0xc;
    tU16 hinge1;
    tU16 hinge2;
    undefined field_0x12[2];
    tU8 field_0x14;
    tU8 field_0x15;
    undefined field_0x16[2];
    undefined4 field_0x18;
    undefined field_0x1c[4];
    tU8 kev_o_flap;
    undefined field_0x21[7];
    tU8 is_door;
    undefined field_0x29[3];
} tCar_crush_flap_data;

typedef enum {
    eDetachType_normal=0,
    eDetachType_stubborn=1,
    eDetachType_fully_detachable=2,
    eDetachType_joint_index=3,
    eDetachType_kev_o_flap=5
} tDetach_type;

typedef struct  {
    undefined4 field_0x0;
    float force;
    tU16 min_vertex;
    tU16 max_vertex;
    undefined field_0xc[24];
    tDetach_type type;
    undefined4 field_0x28;
    int field_0x2c;
    tU16 field_0x30;
    undefined field_0x32[2];
    int count_shape_points; /* >0 if shape is poly, ==0 if box */
    int shape_points[16];
    undefined field_0x78[16];
} tCar_crush_detach_data;

typedef struct {
    char material_name[16];
    undefined field_0x10[48];
    int funk;
    br_material* funk_material;
    int count_smashable_levels;
    undefined field_0x4c[4];
    tSmashable_level* levels;
    undefined field_0x56[28];
    undefined4* field_0x70;
    undefined field_0x74[4];
    undefined4* field_0x78;
} tCar_crush_smashable_part;

typedef struct {
    char actor_name[16];
    undefined field_0xc[16];
    tU8 id;
    undefined field_0x21[3];
    float field_0x24;
    float softness_factor;
    tU8 field_0x2c;
    undefined field_0x2d[3];
    tCar_crush_flap_data* flap_data;
    tCar_crush_detach_data* detach_data;
    tU16 count_smashables;
    undefined field_0x3a[2];
    tCar_crush_smashable_part* smashables;
} tCar_crush_buffer_entry;

typedef struct {
    int count_entries;
    tCar_crush_buffer_entry* entries[kMax_crush_datas];
} tCar_crush_buffer;

typedef struct {
    float min_max[2][2];
} tCar_crush_limit;

typedef struct {
    undefined field_0x0[6];
} tCar_crush_vertex_data;

typedef struct tCar_crush_spec {
    int field_0x0;
    undefined* field_0x4;
    int expand_bounding_box;
    tCollision_shape *field_0xc;
    undefined field_0x10[12];
    int version_le_100;
    undefined* field_0x20;
    undefined2 field_0x24;
    undefined2 field_0x26;
    int field_0x28;
    int count_limits[3][2];
    tCar_crush_limit limits[3];
    undefined field_0x74[192];
    tU8 field_0x134;
    undefined field_0x135[15];
    tU8 field_0x144;
    undefined field_0x145[3];
    float bendability_factor;
    float bend_z_min;
    float bend_z_max;
    undefined field_0x154[120];
    tCar_crush_vertex_data* field_0x1cc;
    undefined field_0x1d0[28];
    br_vector3 field_0x1ec[6];
    undefined field_0x1f8[60];
    undefined4 field_0x270;
    undefined field_0x274[60];
    undefined4 field_0x2b0;
    undefined field_0x2b4[516];
    tU8 field_0x4b8;
    undefined field_0x4b9[3];
    float snappability_factor;
    float split_y_pos;
    undefined field_0x4c4[8];
    br_actor* actor;
    br_actor* model_actor;
    undefined field_0x4d4[132];
    br_vector3 driver_position;
    struct tCollisionShape_Polyhedron * field1245_0x564[4];
    int field_0x574;
    int nb_entries;
} tCar_crush_spec;

typedef struct {
    int owner;
    int done_this_frame;
    int block_flags;
    struct br_actor * actor;
    tLollipop_mode lollipop_mode;
    tGroove_trigger_mode mode;
    tGroove_path_mode path_type;
    tMove_mode path_mode;
    tInterrupt_status path_interrupt_status;
    float path_resumption_value;
    undefined field_0x28[16];
    br_vector3 field_0x38;
    br_vector3 object_centre;
    br_vector3 object_position;
    tGroove_object_mode object_type;
    tMove_mode object_mode; /* Created by retype action */
    tInterrupt_status object_interrupt_status; /* Created by retype action */
    float object_resumption_value;
    undefined field_0x6c[24];
} tGroovidelic_spec;

typedef struct {
    br_model** models;
    int model_count;
    int count_detail_levels;
} tCrush_model_pool;

typedef struct {
    undefined field_0x0[12];
} tUser_detail_level_model;

typedef struct {
    br_model* models[2]; /* model for every detail level. Each model has a tUser_detail_level_model. */
    tCar_crush_buffer_entry* crush_data;
    tGroovidelic_spec* groove;
} tUser_crush_data;

typedef struct {
    tU32 min_time_between;
    tU32 max_time_between;
    float min_walk_speed_factor;
    float max_walk_speed_factor;
    br_angle max_random_angle;
    tU8 padding_0x12[2];
} tPed_movement_spec;

typedef struct {
    undefined field_0x0[0x50];
} tPed_face_cache_0x50;

typedef struct  {
    tPed_face_cache_0x50* field_0x0;
    undefined field_0x4;
    tU8 count;
    undefined field_0x6[46];
} tPed_face_cache_0x34;

typedef struct {
    tU8 flags;
    tU8 max_count;
    tU8 model_count;
    undefined field_0x3;
    float min_force;
    float instance_force;
    br_model* models[3];
} tPed_giblet_size_spec;

typedef struct {
    char name[16];
} tPed_group_member_spec;

typedef struct {
    int count;
    tPed_group_member_spec *members;
} tPed_group_spec;

typedef struct {
    undefined4 field_0x0;
    br_actor* actors[7];
    undefined field_0x8[76];
} tBurning_ped;

typedef struct {
    br_vector3 points[4];
} tQuad;

typedef struct {
    int timer_increments[3];
    int count_quads;
    tQuad quads[4];
    br_vector3 normals[4];
    br_vector3 pos;
    br_vector2 map_position;
} tRace_checkpoint_spec;

typedef struct {
    int index;
    int net_player_index; /* FIXME: order correct? */
    int ranking; /* FIXME: order correct? */
    tCar_spec* car_spec;
} tOpp_spec;

typedef struct {
    float car_wall_friction;
    float tyre_road_friction;
    float down_force;
    float bumpiness;
    int tyre_noise_index;
    int crash_noise_index;
    int scrape_noise_index;
    float sparkiness;
    int smoke_type;
    br_material* skid_mark_material;
} tMaterial_modifiers;

typedef struct {
    int index;
    int total_laps;
    int check_point_count;
    int initial_timer[3];
    int completion_bonus[3];
    int completion_bonus_peds[3];
    int completion_bonus_opponents[3];
    undefined field_0x3c[84];
    int number_of_racers;
    undefined field_0x94[4];
    tRace_list_spec* race_spec;
    undefined field_0x9c[384];
    tRace_checkpoint_spec checkpoints[5];
    undefined field_0x780[1380];
    tOpp_spec opponent_list[30];
    br_vector3 initial_position;
    float initial_yaw;
    br_matrix34 map_transformation;
    br_pixelmap* map_image;
    tMaterial_modifiers material_modifiers[11];
} tRace_info;

typedef enum {
    kActionReplayCameraMode_Standard = 0,
    tActionReplayCameraMode_Panning = 1,
    tActionReplayCameraMode_ActionTracking = 2,
    tActionReplayCameraMode_Manual = 3,
    tActionReplayCameraMode_Rigid = 4,
    tActionReplayCameraMode_Peds = 5,
    tActionReplayCameraMode_Drone = 6,
    tActionReplayCameraMode_Reversing = 7,
    tActionReplayCameraMode_Internal = 8,
} tActionReplayCameraMode;

typedef enum {
    ePlayer_status_unknown = 0,
    ePlayer_status_ready = 1,
    ePlayer_status_loading = 2,
    ePlayer_status_wrecks_gallery = 3,
    ePlayer_status_summary = 4,
    ePlayer_status_not_responding = 5,
    ePlayer_status_racing = 6,
    ePlayer_status_main_menu = 7,
    ePlayer_status_recovering = 8,
    ePlayer_status_action_replay = 9
} tPlayer_status;

typedef enum {
    eNet_avail_never = 0,
    eNet_avail_eagle = 1,
    eNet_avail_hawk = 2,
    eNet_avail_all = 3
} tNet_avail;

typedef struct {
    tU8 grudge_against_player;
} tOppo_psyche;

typedef struct {
    char name[24];
    char abbrev_name[56];
    char car_file_name[64];
    char car_name[32];
    char line1_topspeed[32];
    char line2_weight[32];
    char line3_acceleration[32];
    char line4_description[120];
    tU8* mug_shot_image_data;
    tU8* stolen_car_image_data;
    int mug_shot_image_data_length;
    int stolen_car_image_data_length;
    int car_number;
    int strength_rating;
    int price;
    int picked;
    int dead;
    int text_chunk_count;
    tNet_avail network_availability;
    br_pixelmap* grid_icon_image;
    tOppo_psyche psyche;
    undefined field_0x1b9[3];
    tText_chunk* text_chunks;
} tOpponent;

typedef enum {
    eNet_game_type_foxy = 7
} tNet_game_type;

typedef struct {
    char addr[16]; /* struct sockaddr addr; */
} tPD_net_player_info;

typedef enum {
    eNet_game_XXXXX = -1 /* FIXE: remove eNet_game_XXXXX and add real stages */
} tNet_game_stage;

typedef struct {
    tNet_game_stage stage;
} tNet_game_status;

typedef struct {
    tPD_net_player_info pd_net_info;
    undefined4 field_0x10;
    char field_0x14[28];
    tPlayer_ID host_ID;
    int num_players;
    int start_race;
    int no_races_yet;
    tNet_game_status status;
    tNet_game_options options;
    tNet_game_type type;
} tNet_game_details;

typedef struct {
    tPD_net_player_info pd_net_info;
    tU32 this_players_time_stamp;
    tU32 last_heard_from_him;
    tU32 reposition_time;
    int last_waste_message;
    int host;
    tPlayer_ID ID;
    char player_name[32];
    tPlayer_status player_status;
    int car_index;
    int grid_index;
    int grid_position_set;
    undefined field_0x58[4];
    int opponent_list_index;
    undefined field_0x60[16];
    int wasteage_attributed;
    undefined field_0x74[12];
    int field_0x80;
    undefined field_0x84[12];
    int score;
    undefined field_0x94[4];
    struct br_matrix34 initial_position;
    struct tCar_spec * car;
    undefined field_0xcc[8];
} tNet_game_player_info;typedef enum {
    kDroneType_car = 0,
    kDroneType_plane = 1,
    kDroneType_train = 2,
    kDroneType_chopper = 3,
} tDrone_type;

typedef struct {
    char name[13];
    undefined field1_0xd[3];
    tDrone_type type;
    float M;
    float speed;
    float min_speed;
    float max_speed;
    float max_acceleration;
    float crushability;
    int flags;
    struct br_vector3 center;
    float crush_limits_front;
    float crush_limits_back;
    float crush_limits_left;
    float crush_limits_right;
    float crush_limits_bottom;
    float crush_limits_top;
    undefined field_0x54[24];
    undefined4 field_0x6c;
    undefined4 field_0x70;
    undefined4 field_0x74;
    undefined4 field_0x78;
    undefined4 field_0x7c;
    undefined4 field_0x80;
    undefined4 field_0x84;
} tDrone;

typedef struct {
    br_material* material;
    br_vector3 v[3];
    br_vector2* map[3];
    br_vector3 normal;
    int flags;
    br_scalar d;
    undefined field_0x48[8];
} tFace_ref;

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
    kFatalError_AllocateOffScreenBuffer = 0x01,
    kFatalError_AllocateZBuffer = 0x02,
    kFatalError_FailToOpenGeneralSettings = 0x03,
    kFatalError_CouldNotOpenKeyMapFile = 0x09,
    kFatalError_CouldNotOpenFlicFile_S = 0x0d,
    kFatalError_FlicFileWasNot8BitsDeep_S = 0x0f,
    kFatalError_CouldNotAscertainFrameRateForFlicFile = 0x10,
    kFatalError_CannotLoadFontWidthTable_S = 0x15,
    kFatalError_CantLoadCarResolutionIndependentFile = 0x18,
    kFatalError_ScreenDimensionNotInGrafData = 0x19,
    kFatalError_CannotLoadSpeedoImage = 0x23,
    kFatalError_CannotOpenRacesFile = 0x32,
    kFatalError_CannotOpenOpponentsFile = 0x36,
    kFatalError_OpponentCountMismatchesActualNumberOfOpponents = 0x37,
    kFatalError_InsufficientPixelmapSlots = 0x43,
    kFatalError_InsufficientShadeTableSlots = 0x44,
    kFatalError_InsufficientMaterialSlots = 0x45,
    kFatalError_InsufficientModelSlots = 0x46,
    kFatalError_CantLoadPixelmapFile_S = 0x4f,
    kFatalError_CannotLoadShadeTableFileOrItIsEmpty_S = 0x50,
    kFatalError_CannotLoadMaterialFileOrItIsEmpty_S = 0x51,
    kFatalError_CannotLoadModelFileOrItIsEmpty_S = 0x52,
    kFatalError_CannotLoadDepthCueShadeTable = 0x57,
    kFatalError_CannotFindSkyMaterial_S = 0x59,
    kFatalError_UnknownDamageType_S = 0x5b,
    kFatalError_ConfusedByFormatOfConditionalDamageInCarFile = 0x5c,
    kFatalError_OOM_S = 0x5e,
    kFatalError_CannotOpenTEXT_TXT = 0x63,
    kFatalError_CannotFindFlicReferencedTranslation_S = 0x65,
    kFatalError_CantOpen_S = 0x6b,
    kFatalError_CannotLoadAGeneratedShadeTable = 0x6d,
    kFatalError_CantLoadSmashPix_S = 0x77,
    kFatalError_CannotFindSmashMaterial_S = 0x78,
    kFatalError_TooManyCrushLimits = 0x7a,
    kFatalError_TooManyCrushDatas = 0x7c,
    kFatalError_CannotOpenPedFile_S = 0xa0,
    kFatalError_CantFindPedTexture_S = 0xa8,
    kFatalError_CannotFindGibletModel_S = 0xa9,
    kFatalError_WrongCrushDataFileVersion_SDD = 0xab,
    kFatalError_UnableToOpenDroneFileOrFileCorrupted_S = 0xaf,
    kFatalError_DuplicatePixelmap_S = 0xb5,
    kFatalError_CantCopeWithVersionFor_SS = 0xb1,
    kFatalError_FileMustStartWith_SS = 0xb2,
    kFatalError_DuplicateModel_S = 0xb3,
    kFatalError_DuplicateMaterial_S = 0xb4,
    kFatalError_WriteAttemptToPackedFile_S = 0xb6,
    kFatalError_CouldNotCreateTexturesPages_S = 0xb7,
    kFatalError_MysteriousX_SS = 0xbd,
};

#endif // REC2_TYPES_H
