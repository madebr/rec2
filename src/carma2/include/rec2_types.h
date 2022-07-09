#ifndef REC2_TYPES_H
#define REC2_TYPES_H

#include <brender/br_types.h>

typedef char tPath_name[256];

typedef unsigned char tU8;
typedef signed char tS8;
typedef unsigned short tU16;
typedef signed short tS16;
typedef unsigned int tU32;
typedef signed int tS32;

typedef int tTWTVFS;

typedef struct {
    int width;
    int height;
    tU8 unknown[1304];
} tGraf_data;

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

enum {
    // FIXME: add all c2 memory tags
    kMem_packed_file = 0xd0,
};

enum {
    // FIXME: add all c2 FatalError types
    kFatalError_InvalidScreenDepthSetting = 0x00,
    kFatalError_FailToOpenGeneralSettings = 0x03,
    kFatalError_ScreenDimensionNotInGrafData = 0x18,
    kFatalError_MysteriousX_SS = 0xbd,
};

enum {
    kMem_misc = 0xfd,
};
#endif // REC2_TYPES_H
