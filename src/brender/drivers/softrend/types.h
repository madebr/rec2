#ifndef SOFTREND_TYPES_H
#define SOFTREND_TYPES_H

#include <brender/brender.h>

typedef struct br_geometry_primitives br_geometry_primitives;
typedef struct br_geometry_v1_buckets br_geometry_v1_buckets;
typedef struct br_soft_renderer_state_stored br_soft_renderer_state_stored;
typedef struct br_soft_renderer br_soft_renderer;

struct device_templates {
    struct br_tv_template* deviceTemplate;
    struct br_tv_template* rendererFacilityTemplate;
    struct br_tv_template* template_field_0x08;
    struct br_tv_template* template_field_0x0c;
    struct br_tv_template* template_field_0x10;
    struct br_tv_template* template_field_0x14;
    struct br_tv_template* template_field_0x18;
    struct br_tv_template* template_field_0x1c;
    struct br_tv_template* rendererNewTemplate;
    struct br_tv_template* template_field_0x24;
    struct br_tv_template* template_field_0x28;
    struct br_tv_template* template_field_0x2c;
    struct br_tv_template* template_field_0x30;
    struct br_tv_template* template_field_0x34;
    struct br_tv_template* template_field_0x38;
    struct br_tv_template* template_field_0x3c;
    struct br_tv_template* template_field_0x40;
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

typedef struct soft_state_all {
    br_soft_renderer* renderer;
    br_uint_32 valid;
    undefined field_0x0[0x780];
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
    br_device* device;
    void* object_list;
    soft_state_all state;
    soft_state_all* default_state;
    br_soft_renderer_state_stored* last_restored;
    br_primitive_library* plib;
    soft_state_all state_stack[MAX_STATE_STACK];
    br_uint_32 stack_top;
    br_soft_renderer_facility* renderer_facility;
} br_soft_renderer;

#endif /* SOFTREND_TYPES_H */
