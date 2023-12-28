#ifndef SOFTREND_TIMESTMP_H
#define SOFTREND_TIMESTMP_H

#include "c2_hooks.h"

typedef br_uint_32 br_timestamp;

C2_HOOK_VARIABLE_DECLARE(br_uint_32, DriverTimestamp);

#define Timestamp()		(C2V(DriverTimestamp) += 2)

#define TIMESTAMP_START		1

#define INVALID_TIME		0

#endif /* SOFTREND_TIMESTMP_H */

