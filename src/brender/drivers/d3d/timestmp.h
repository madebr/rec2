/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

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