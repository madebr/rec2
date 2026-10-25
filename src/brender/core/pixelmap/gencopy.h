/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_GENCOPY_H
#define REC2_GENCOPY_H

#include "c2_hooks.h"

#include "brender/br_types.h"

br_error C2_HOOK_STDCALL GeneralRectangleCopy(br_device_pixelmap* dest, br_point* p, br_device_pixelmap* src, br_rectangle* r);

br_error C2_HOOK_STDCALL GeneralCopy(br_device_pixelmap* dst, br_device_pixelmap* src);

br_error C2_HOOK_STDCALL GeneralRectangleStretchCopy(br_device_pixelmap* dst, br_rectangle* d, br_device_pixelmap* src, br_rectangle* s);

#endif // REC2_GENCOPY_H
