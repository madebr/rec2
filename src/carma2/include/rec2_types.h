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

typedef struct tBrender_storage {
    int uid; // FIXME: unknown
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
    int** soundIds; // FIXME: correct?
    int** materialProps; // FIXME: correct?
} tBrender_storage;

typedef struct {
    size_t size;
    char items[1000][16];
} tName_list;

typedef struct {
    int twtfile;
    void* stdio;
} tTWTFILE;

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
    tPath_name path;
    tU8* data;
} tTwatVfsMountPoint;

typedef struct {
    int used;
    int unknown[3];
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

enum {
    kMiscString_ShadowNone = 104,
    kMiscString_ShadowUsOnly = 105,
    kMiscString_ShadowUsAndOpponents = 106,
    kMiscString_ShadowEveryone = 107,
};

enum {
    // FIXME: add all c2 memory tags
    kMem_misc_string = 0xab,
    kMem_packed_file = 0xd0,
};

enum {
    // FIXME: add all c2 FatalError types
    kFatalError_InvalidScreenDepthSetting = 0x00,
    kFatalError_FailToOpenGeneralSettings = 0x03,
    kFatalError_CouldNotOpenKeyMapFile = 0x09,
    kFatalError_ScreenDimensionNotInGrafData = 0x18,
    kFatalError_InsufficientPixelmapSlots = 0x43,
    kFatalError_CantLoadPixelmapFile_S = 0x4f,
    kFatalError_CannotOpenTEXT_TXT = 0x63,
    kFatalError_CantOpen_S = 0x6b,
    kFatalError_DuplicatePixelmap_S = 0xb5,
    kFatalError_CantCopeWithVersionFor_SS = 0xb1,
    kFatalError_FileMustStartWith_SS = 0xb2,
    kFatalError_MysteriousX_SS = 0xbd,
};

enum {
    kMem_misc = 0xfd,
};

#endif // REC2_TYPES_H
