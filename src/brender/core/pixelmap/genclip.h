/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_GENCLIP_H
#define REC2_GENCLIP_H

#include "c2_hooks.h"

#include "brender/br_types.h"

br_clip_result C2_HOOK_CDECL PixelmapPointClip(br_point* out, br_point* in, br_pixelmap* pm);

br_clip_result C2_HOOK_CDECL PixelmapLineClip(br_point* s_out, br_point* e_out, br_point* s_in, br_point* e_in, br_pixelmap* pm);

br_clip_result C2_HOOK_CDECL PixelmapRectangleClip(br_rectangle* out, br_rectangle* in, br_pixelmap* pm);

br_clip_result C2_HOOK_CDECL PixelmapRectangleClipTwo(br_rectangle* r_out, br_point* p_out, br_rectangle* r_in, br_point* p_in, br_pixelmap* pm_dst, br_pixelmap* pm_src);

br_clip_result C2_HOOK_CDECL PixelmapCopyBitsClip(br_rectangle* r_out, br_point* p_out, br_rectangle* r_in, br_point* p_in, br_pixelmap* pm);

#endif // REC2_GENCLIP_H