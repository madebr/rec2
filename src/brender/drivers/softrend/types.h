#ifndef SOFTREND_TYPES_H
#define SOFTREND_TYPES_H

#include <brender/brender.h>

#define MAX_STATE_LIGHTS        16
#define MAX_STATE_CLIP_PLANES   6

typedef struct br_geometry_primitives br_geometry_primitives;
typedef struct br_renderer_state_stored_soft br_renderer_state_stored_soft;
typedef struct br_soft_renderer br_soft_renderer;
typedef struct temp_face_soft temp_face_soft;
typedef br_uint_32 br_timestamp;
typedef void C2_HOOK_CDECL geometry_fn(br_geometry* self, br_renderer* renderer);
typedef void C2_HOOK_CDECL surface_fn(br_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp);

typedef enum {
    C_FLAGS = 0,
    C_X     = 1,
    C_Y     = 2,
    C_Z     = 3,
    C_W     = 4,
    C_SX    = 5,
    C_SY    = 6,
    C_SZ    = 7,
    C_U     = 8,
    C_V     = 9,
    C_I     = 10,
    C_A     = 10,
    C_R     = 11,
    C_G     = 12,
    C_B     = 13,
    C_UI    = 10,
    C_UR    = 11,
    C_UG    = 12,
    C_UB    = 13,
    NUM_COMPONENTS = 16,
} brp_components;

#define MAX_GEOMETRY_FNS 16
#define MAX_SURFACE_FNS 8
#define MAX_FACE_BLOCKS 7

struct device_templates {
    struct br_tv_template* deviceTemplate;
    struct br_tv_template* rendererFacilityTemplate;
    struct br_tv_template* rendererTemplate;
    struct br_tv_template* rendererStateStoredTemplate;
    struct br_tv_template* geometryV1ModelTemplate;
    struct br_tv_template* geometryV1BucketsTemplate;
    struct br_tv_template* geometryPrimitivesTemplate;
    struct br_tv_template* geometryLightingTemplate;
    struct br_tv_template* rendererNewTemplate;
    struct br_tv_template* partCullTemplate;
    struct br_tv_template* partSurfaceTemplate;
    struct br_tv_template* partMatrixTemplate;
    struct br_tv_template* partEnableTemplate;
    struct br_tv_template* partHiddenSurfaceTemplate;
    struct br_tv_template* partBoundsTemplate;
    struct br_tv_template* partLightTemplate;
    struct br_tv_template* partClipTemplate;
};

typedef struct br_soft_device {
    struct br_device_dispatch *dispatch;
    const char *identifier;
    struct br_soft_device *device;
    void *object_list;
    void *res;
    struct device_templates templates;
} br_soft_device;

typedef struct  {
    br_object_dispatch *dispatch;
    const char *identifier;
    br_soft_device *device;
} br_softrend_object;

typedef struct {
    br_token type;
    br_token space;
    br_timestamp timestamp;
} soft_state_cull;

typedef struct {
    br_colour colour;
    br_scalar opacity;
    br_scalar ka;
    br_scalar kd;
    br_scalar ks;
    br_scalar power;
    br_boolean lighting;
    br_boolean force_front;
    br_token colour_source;
    br_token mapping_source;
    br_matrix23 map_transform;
    br_timestamp timestamp;
    br_timestamp timestamp_transform;
} soft_state_surface;

typedef struct {
    br_matrix34 model_to_view;
    br_matrix4 view_to_screen;
    br_matrix34 view_to_environment;
    br_token model_to_view_hint;
    br_token view_to_screen_hint;
    br_token view_to_environment_hint;
    br_timestamp timestamp;
    br_timestamp timestamp_v2s_hint;
    br_timestamp timestamp_m2v;
    br_timestamp timestamp_v2s;
} soft_state_matrix;

typedef struct {
    br_uint_32 flags;
    br_timestamp timestamp;
} soft_state_enable;

typedef void C2_HOOK_CDECL soft_state_hidden_insert_cbfn(br_primitive* primitive, void* arg1, void* arg2, void* arg3,br_order_table* order_table, br_scalar* z);

typedef struct {
    br_token type;
    br_order_table* order_table;
    br_primitive_heap* heap;
    soft_state_hidden_insert_cbfn* insert_fn;
    void* insert_arg1;
    void* insert_arg2;
    void* insert_arg3;
    br_timestamp timestamp;
} soft_state_hidden;

typedef struct {
    br_vector2 min;
    br_vector2 max;
} soft_state_bounds;

typedef struct {
    br_token type;
    br_token lighting_space;
    br_vector3 position;
    br_vector3 direction;
    br_colour colour;
    br_scalar spot_outer;
    br_scalar spot_inner;
    br_scalar attenuation_l;
    br_scalar attenuation_c;
    br_scalar attenuation_q;
    br_timestamp timestamp;
} soft_state_light;

typedef struct {
    br_token type;
    br_vector4 plane;
} soft_state_clip;

typedef struct {
    undefined4 field_0x0;
    undefined4 field_0x4;
} brp_block_min;

typedef struct {
    br_uint_32 clip_slots;
    br_scalar comp_offsets[NUM_COMPONENTS];
    br_scalar comp_scales[NUM_COMPONENTS];
    br_int_32 ngeometry_fns;
    geometry_fn* geometry_fns[MAX_GEOMETRY_FNS];
    br_int_32 ngeometry_fns_onscreen;
    geometry_fn* geometry_fns_onscreen[MAX_GEOMETRY_FNS];
    brp_block_min face_blocks[MAX_FACE_BLOCKS];
    brp_block_min face_blocks_onscreen[MAX_FACE_BLOCKS];
    br_int_8 nface_blocks;
    br_int_8 nface_blocks_onscreen;
    undefined field_0x17e[2];
    br_geometry* format;
    br_int_32 nvertex_fns;
    surface_fn* vertex_fns[MAX_SURFACE_FNS];
    br_int_32 nconstant_fns;
    surface_fn* constant_fns[MAX_SURFACE_FNS];
    br_matrix23	map_transform;
    undefined field_0x1e4[4];
    br_boolean valid;
} soft_state_cache;

typedef struct soft_state_all {
    br_soft_renderer* renderer;
    br_uint_32 valid;
    soft_state_cull cull;
    soft_state_surface surface;
    soft_state_light light[MAX_STATE_LIGHTS];
    br_timestamp  timestamp_lights;
    soft_state_clip clip[MAX_STATE_CLIP_PLANES];
    br_timestamp  timestamp_clips;
    soft_state_matrix matrix;
    soft_state_bounds bounds;
    soft_state_enable enable;
    soft_state_hidden hidden;
    soft_state_cache cache;
    undefined field_0x77c[0x8];
    br_primitive_state* pstate;
} soft_state_all;

typedef struct br_soft_renderer_facility {
    br_renderer_facility_dispatch* dispatch;
    const char* identifier;
    br_soft_device* device;
    void* object_list;
    soft_state_all default_state;
    br_int_32 num_instances;
} br_soft_renderer_facility;

typedef struct {
    br_object* dest;
    br_primitive_library* prims;
}  newRendererTokens;

#define MAX_STATE_STACK 32

typedef struct br_soft_renderer {
    br_renderer_dispatch* dispatch;
    const char* identifier;
    br_soft_device* device;
    void* object_list;
    soft_state_all state;
    soft_state_all* default_state;
    br_renderer_state_stored_soft* last_restored;
    br_primitive_library* plib;
    soft_state_all state_stack[MAX_STATE_STACK];
    br_uint_32 stack_top;
    br_soft_renderer_facility* renderer_facility;
} br_soft_renderer;

typedef struct {
    br_geometry_v1_model_dispatch* dispatch;
    const char* identifier;
    br_soft_device* device;
    br_soft_renderer_facility* renderer_facility;
} br_geometry_v1_model_soft;

typedef struct {
    const br_geometry_v1_buckets_dispatch* dispatch;
    const char* identifier;
    br_soft_device* device;
    br_soft_renderer_facility* renderer_facility;
} br_geometry_v1_buckets_soft;

typedef struct {
    const br_geometry_lighting_dispatch* dispatch;
    const char* identifier;
    br_soft_device* device;
    br_soft_renderer_facility* renderer_facility;
} br_geometry_lighting_soft;

typedef struct {
    void (C2_HOOK_CDECL*_reserved0)(br_object*);
    void (C2_HOOK_CDECL*_reserved1)(br_object*);
    void (C2_HOOK_CDECL*_reserved2)(br_object*);
    void (C2_HOOK_CDECL*_reserved3)(br_object*);
    void (C2_HOOK_CDECL*_free)(br_object*);
    char* (C2_HOOK_CDECL*_identifier)(br_object*);
    br_token (C2_HOOK_CDECL*_type)(br_object*);
    br_boolean (C2_HOOK_CDECL*_isType)(br_object*, br_token);
    br_device* (C2_HOOK_CDECL*_device)(br_object*);
    br_int_32 (C2_HOOK_CDECL*_space)(br_object*);
    br_tv_template* (C2_HOOK_CDECL*_templateQuery)(br_object*);
    br_error (C2_HOOK_CDECL*_query)(br_object*,br_uint_32*,br_token);
    br_error (C2_HOOK_CDECL*_queryBuffer)(br_object*,br_uint_32*,br_uint_32*,br_size_t,br_token);
    br_error (C2_HOOK_CDECL*_queryMany)(br_object*,br_token_value*,void*,br_size_t,br_int_32*);
    br_error (C2_HOOK_CDECL*_queryManySize)(br_object*,br_size_t*,br_token_value*);
    br_error (C2_HOOK_CDECL*_queryAll)(br_object*,br_token_value*,br_size_t);
    br_error (C2_HOOK_CDECL*_queryAllSize)(br_object*,br_size_t*);
    br_error (C2_HOOK_CDECL*_renderF)(br_geometry_primitives*,br_renderer*,fmt_vertex*,int,br_token);
    br_error (C2_HOOK_CDECL*_renderX)(br_geometry_primitives*,br_renderer*,fmt_vertex*,int,br_token);
    br_error (C2_HOOK_CDECL*_renderOnScreenF)(br_geometry_primitives*,br_renderer*,fmt_vertex*,int,br_token);
    br_error (C2_HOOK_CDECL*_renderOnScreenX)(br_geometry_primitives*,br_renderer*,fmt_vertex*,int,br_token);
    br_error (C2_HOOK_CDECL*_storedNewF)(br_geometry_primitives*,br_renderer*,br_geometry_stored**,fmt_vertex*,int,br_token,br_token_value*);
    br_error (C2_HOOK_CDECL*_storedNewX)(br_geometry_primitives*,br_renderer*,br_geometry_stored **,fmt_vertex*,int,br_token,br_token_value*);
    br_error (C2_HOOK_CDECL*_storedAvail)(br_geometry_primitives*,br_int_32*,br_token_value*);
} br_geometry_primitives_dispatch;

typedef struct {
    const br_geometry_primitives_dispatch* dispatch;
    const char* identifier;
    br_soft_device* device;
    br_soft_renderer_facility* renderer_facility;
} br_geometry_primitives_soft;

typedef struct br_renderer_state_stored_soft {
    const br_renderer_state_stored_dispatch* dispatch;
    const char* identifier;
    br_soft_device* device;
    br_soft_renderer* renderer;
    br_uint_32 valid;
    soft_state_cull cull;
    soft_state_surface surface;
    soft_state_cache cache;
    br_timestamp timestamp_copy_cache;
    br_timestamp timestamp_cache;
    br_primitive_state* pstate;
} br_renderer_state_stored_soft;

typedef struct {
    void* scratch;
    temp_face_soft* temp_faces;
    br_int_8* vertex_counts;
    brp_vertex* temp_vertices;
    brp_vertex** vertex_heap_pointers;
    br_int_8* vertex_flags;
    br_int_8* edge_flags;
    br_int_32 nvisible_faces;
    undefined4 field_0x20;
    v11face* faces;
    fmt_vertex* vertices;
    br_colour* face_colours;
    br_colour* vertex_colours;
    br_int_32 nfaces;
    br_int_32 nvertices;
    br_int_32 nedges;
    br_vector3 eye_l;
    brp_block* block;
    br_boolean block_changed;
    br_boolean range_changed;
    br_soft_renderer* renderer;
    br_geometry* geometry;
    br_scalar subdivide_threshold;
} rend_block_soft;

typedef struct {
    br_token type;
    br_vector3 position;
    br_vector3 direction;
    br_vector3 half;
    br_scalar intensity;
    undefined4 accumulate_index;
    undefined4 accumulate_colour;
    soft_state_light* s;
} active_light;

typedef struct {
    br_matrix4 model_to_screen;
    br_matrix34 view_to_model;
    br_matrix34 model_to_environment;
    br_matrix4 model_to_viewport;
    br_colour colour;
    br_vector2 min;
    br_vector2 max;
    br_vector4 eye_m;
    br_vector3 eye_m_normalised;
    br_boolean user_clip_active;
    br_boolean light_1md;
    active_light lights[MAX_STATE_LIGHTS];
    br_int_32 nlights_model;
    br_int_32 nlights_view;
    br_boolean valid_v2m;
    br_boolean valid_m2s;
    br_boolean valid_per_model;
    br_boolean valid_per_scene;
} static_cache_soft;

enum {
    MASK_STATE_CULL     = BR_STATE_CULL,
    MASK_STATE_SURFACE  = BR_STATE_SURFACE,
    MASK_STATE_PRIMITIVE= BR_STATE_PRIMITIVE,
    MASK_STATE_MATRIX   = BR_STATE_MATRIX,
    MASK_STATE_ENABLE   = BR_STATE_ENABLE,
    MASK_STATE_LIGHT    = BR_STATE_LIGHT,
    MASK_STATE_CLIP     = BR_STATE_CLIP,
    MASK_STATE_BOUNDS   = BR_STATE_BOUNDS,
    MASK_STATE_CACHE    = BR_STATE_CACHE,

    MASK_STATE_LOCAL    = ( MASK_STATE_CULL |
                            MASK_STATE_SURFACE |
                            MASK_STATE_MATRIX |
                            MASK_STATE_ENABLE |
                            MASK_STATE_LIGHT |
                            MASK_STATE_CLIP |
                            MASK_STATE_BOUNDS)
};

#define MASK_CACHED_STATES (MASK_STATE_CULL | MASK_STATE_SURFACE)

#endif /* SOFTREND_TYPES_H */
