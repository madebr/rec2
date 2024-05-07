#ifndef SOFTREND_TYPES_H
#define SOFTREND_TYPES_H

#include <brender/brender.h>

typedef struct br_geometry_primitives br_geometry_primitives;
typedef struct br_geometry_v1_buckets br_geometry_v1_buckets;

struct device_templates {
    struct br_tv_template *templates[3];
};

typedef struct br_soft_device {
    struct br_device_dispatch *dispatch;
    const char *identifier;
    struct br_soft_device *device;
    void *object_list;
    void *res;
    struct device_templates templates;
    struct host_info hostInfo;
} br_soft_device;

typedef struct  {
    br_object_dispatch *dispatch;
    const char *identifier;
    br_soft_device *device;
} br_softrend_object;

typedef struct soft_state_all {
    br_uint_8 field_0x0[0x788];
} soft_state_all;

typedef struct br_soft_renderer_facility {
    br_renderer_facility_dispatch* dispatch;
    const char* identifier;
    br_device* device;
    void* object_list;
    soft_state_all default_state;
    br_int_32 num_instances;
} br_soft_renderer_facility;

#endif /* SOFTREND_TYPES_H */
