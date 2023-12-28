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

#endif /* SOFTREND_TYPES_H */
