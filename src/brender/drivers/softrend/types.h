#ifndef SOFTREND_TYPES_H
#define SOFTREND_TYPES_H

#include <brender/brender.h>

#define MAX_STATE_LIGHTS        16
#define MAX_STATE_CLIP_PLANES   6

typedef struct br_geometry_primitives br_geometry_primitives;
typedef struct br_geometry_v1_buckets br_geometry_v1_buckets;
typedef struct br_soft_renderer_state_stored br_soft_renderer_state_stored;
typedef struct br_soft_renderer br_soft_renderer;
typedef br_uint_32 br_timestamp;

struct device_templates {
    struct br_tv_template* deviceTemplate;
    struct br_tv_template* rendererFacilityTemplate;
    struct br_tv_template* rendererTemplate;
    struct br_tv_template* template_field_0x0c;
    struct br_tv_template* template_field_0x10;
    struct br_tv_template* template_field_0x14;
    struct br_tv_template* template_field_0x18;
    struct br_tv_template* template_field_0x1c;
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
    undefined field_0x0[0x1e8];
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
    br_soft_renderer_state_stored* last_restored;
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

#endif /* SOFTREND_TYPES_H */
