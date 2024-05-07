#ifndef SOFTREND_OBJECT_H
#define SOFTREND_OBJECT_H

#include "types.h"

#include "c2_hooks.h"

const char* C2_HOOK_CDECL _M_br_softrend_object_identifier(br_softrend_object* self);

br_device* C2_HOOK_CDECL _M_br_softrend_object_device(br_softrend_object* self);

#endif /* SOFTREND_OBJECT_H */
