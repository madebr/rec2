#ifndef D3D_TYPES_H
#define D3D_TYPES_H

#include <brender/brender.h>

#define D3D_MAX(X, Y) ((X) >= (Y) ? (X) : (Y))
#define D3D_MIN(X, Y) ((X) <  (Y) ? (X) : (Y))

typedef struct br_object_d3d br_object_d3d;
typedef struct br_device_d3d br_device_d3d;
typedef struct brp_block_d3d brp_block_d3d;
typedef struct br_device_pixelmap_d3d br_device_pixelmap_d3d;
typedef struct br_device_clut_d3d br_device_clut_d3d;
typedef struct br_output_facility_d3d br_output_facility_d3d;
typedef struct local_block_d3d local_block_d3d;
typedef struct br_primitive_state_d3d br_primitive_state_d3d;
typedef struct br_buffer_stored_d3d br_buffer_stored_d3d;
typedef struct br_primitive_library_d3d br_primitive_library_d3d;

typedef void C2_HOOK_CDECL render_begin_fn(brp_block_d3d* block);
typedef void C2_HOOK_CDECL render_end_fn(brp_block_d3d* block);

enum {
    BT_FRONTSCREEN,
    BT_BACKSCREEN,
    BT_DEPTH,
    BT_MAX,
};

#define CLUT_SIZE 256

#define PRIMF_SMOOTH            0x001
#define PRIMF_PERSPECTIVE       0x002
#define PRIMF_DECAL             0x004
#define PRIMF_DEPTH_WRITE       0x010
#define PRIMF_COLOUR_WRITE      0x020
#define PRIMF_BLEND             0x080
#define PRIMF_MODULATE          0x200
#define PRIMF_TEXTURE_BUFFER    0x400
#define PRIMF_DEPTH_BUFFER      0x800

#define NUM_COMPONENTS 16

enum {
    SBUFF_SHARED    = 0x0001,   /* Data is shared with user */
};

enum {
    GR_TEXFMT_P_8 = 0x5,
    GR_TEXFMT_RGB_565 = 0x1,
    GR_TEXFMT_ARGB_1555 = 0xb,
    GR_TEXFMT_ARGB_4444 = 0xc,
};

typedef struct br_object_d3d {
    void* dispatch;
    const char *identifier;
    br_device_d3d *device;
} br_object_d3d;

typedef struct br_device_clut_d3d {
    struct br_device_clut_dispatch* dispatch;
    const char* identifier;
    br_colour entries[CLUT_SIZE];
} br_device_clut_d3d;

typedef struct br_device_d3d {
    const struct br_device_dispatch *dispatch;
    const char* identifier;
    br_uint_32 version;
    const char *creator;
    const char *title;
    const char *product;
    const char *product_version;
    void *object_list;
    void *res;
    br_device_clut_d3d* clut;
    br_device_pixelmap_d3d* active_buffers[BT_MAX];
    undefined4 field_0x2c;
    undefined4 field_0x30;
    undefined4 field_0x34;
    undefined4 field_0x38;
    undefined4 field_0x3c;
    undefined4 field_0x40;
} br_device_d3d;

typedef struct br_primitive_library_d3d {
    struct br_primitive_library_dispatch *dispatch;
    const char* identifier;
    void* object_list;
} br_primitive_library_d3d;

typedef struct br_output_facility_d3d {
    struct br_output_facility_dispatch *dispatch;
    const char* identifier;
    void* object_list;
    br_int_32 width;
    br_int_32 height;
    br_int_32 colour_bits;
    br_int_32 depth_bits;
    br_int_32 colour_type;
    br_int_32 depth_type;
    br_boolean indexed;
    undefined4 field_0x28;
    undefined4 field_0x2c;
    br_boolean fullscreen;
    br_int_32 num_instances;
    br_primitive_library_d3d* prim_lib;
    br_int_32 d3d_res_code; /* 3Dfx GrScreenResolution_t */
} br_output_facility_d3d;

typedef struct render_buffer_d3d {
    void* base;
    br_uint_32 width_b;
    br_uint_32 width_p;
    br_uint_32 height;
    br_uint_32 size;
    br_boolean uncacheable;
    br_boolean on_card;
    br_boolean force_reload;
    br_boolean blended;
    void* palette_pointer;
    undefined4 field_0x28;
    undefined4 field_0x2c;
    undefined4 field_0x30;
    undefined4 field_0x34;
    struct {
        undefined4 smallLod;
        undefined4 largeLod;
        undefined4 field_0x40;
        int format;
    } info;
    void* field_0x48;
    int field_0x4c;
    int pm_type;
    br_uint_32 uid;
    undefined4 field_0x58;
    int field_0x5c;
    undefined4 field_0x60;
    undefined4 field_0x64;
    void* field_0x68;
    undefined4 field_0x6c;
    undefined4 field_0x70;
    int field_0x74;
    undefined4 field_0x78;
} render_buffer_d3d;

typedef struct br_buffer_stored_d3d {
    br_buffer_stored_dispatch* dispatch;
    const char* identifier;
    br_primitive_library_d3d* plib;
    br_uint_32 flags;
    render_buffer_d3d buffer;
    undefined4 field_0x80;
    undefined4 field_0x84;
    undefined4 field_0x88;
    void* pixels_pointer;
    undefined4 field_0x90;
} br_buffer_stored_d3d;

typedef struct br_device_pixelmap_d3d {
    struct br_device_pixelmap_dispatch* dispatch;
    const char* pm_identifier;
    /** Start of br_pixelmap fields (not including identifier) **/
    void* pm_pixels;
    br_uint_32 pm_pixels_qualifier;
    br_pixelmap* pm_map;
    br_colour_range pm_src_key;
    br_colour_range pm_dst_key;
    br_uint_32 pm_key;
    br_int_16 pm_row_bytes;
    br_int_16 pm_mip_offset;
    br_uint_8 pm_type;
    br_uint_8 pm_flags;
    br_uint_16 pm_copy_function;
    br_uint_16 pm_base_x;
    br_uint_16 pm_base_y;
    br_uint_16 pm_width;
    br_uint_16 pm_height;
    br_int_16 pm_origin_x;
    br_int_16 pm_origin_y;
    void* pm_user;
    br_buffer_stored_d3d* pm_stored;
    /** End of br_pixelmap fields **/

    br_output_facility_d3d* output_facility;
    br_device_clut_d3d* clut;
    br_int_32 buffer_type;
    br_device_pixelmap_d3d* field_0x50;
    br_device_pixelmap_d3d* field_0x54;
    br_boolean restore_mode;
} br_device_pixelmap_d3d;

typedef struct brp_block_d3d {
    brp_render_fn* render;
    brp_block_d3d* chain;
    const char* identifier;
    void* _reserved0;
    br_token type;
    br_uint_32 flags;
    br_uint_32 constant_components;
    br_uint_32 vertex_components;
    br_uint_32 convert_mask_f;
    br_uint_32 convert_mask_x;
    br_uint_32 convert_mask_i;
    br_uint_32 constant_mask;
    br_int_32 subdivide_tolerance;
    br_uint_32 _reserved_0;
    br_uint_32 _reserved_1;
    br_uint_32 _reserved_2;
} brp_block_d3d;

typedef struct local_block_d3d {
    brp_block_d3d p;
    br_uint_32 flags_mask;
    br_uint_32 flags_cmp;
    render_begin_fn* begin_fn;
    render_end_fn* end_fn;
    br_boolean no_render;
} local_block_d3d;

typedef struct br_primitive_state_d3d {
    br_primitive_state_dispatch* dispatch;
    const char* identifier;
    br_primitive_library_d3d* plib;
    struct {
        br_uint_32 timestamp;
        br_uint_32 timestamp_major;
        br_uint_32 flags;
        br_token colour_type;
        br_buffer_stored_d3d* colour_map;
        br_token map_antialiasing;
        br_token map_interpolation;
        br_token mip_interpolation;
        br_token fog_type;
        br_colour fog_colour;
        float fog_min;
        float fog_max;
        float alpha_val;
    } prim;
    struct {
        br_uint_32 timestamp;
        br_uint_32 timestamp_major;
        br_device_pixelmap_d3d* pixelmap;
        void* depth;
    } out;
    struct {
        local_block_d3d* last_block;
        br_token last_type;
        br_fixed_lu comp_offsets[NUM_COMPONENTS];
        br_fixed_lu comp_scales[NUM_COMPONENTS];
        br_uint_32 timestamp_prim;
        br_uint_32 timestamp_out;
    } cache;
} br_primitive_state_d3d;

#endif /* D3D_TYPES_H */
