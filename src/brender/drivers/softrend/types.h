#ifndef SOFTREND_TYPES_H
#define SOFTREND_TYPES_H

#include <brender/brender.h>

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

typedef struct soft_state_all {
    br_uint_8 field_0x0[0x788];
} soft_state_all;

#endif /* SOFTREND_TYPES_H */
