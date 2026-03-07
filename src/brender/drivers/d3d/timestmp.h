#ifndef D3D_TIMESTMP_H
#define D3D_TIMESTMP_H

#include <brender/brender.h>

#include "c2_hooks.h"

typedef br_uint_32 br_timestamp;

extern br_uint_32 PrimD3DDriverTimestamp;

#define Timestamp()		(PrimD3DDriverTimestamp += 2)

#define TIMESTAMP_START		1

#define INVALID_TIME		0

#endif /* D3D_TIMESTMP_H */