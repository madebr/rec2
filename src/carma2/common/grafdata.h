/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_GRAFDATA_H
#define REC2_GRAFDATA_H

#include "c2_hooks.h"

#include "rec2_types.h"

extern tGraf_data gGraf_data[2];
extern tGraf_data* gCurrent_graf_data;
extern int gReal_graf_data_index;
extern int gGraf_data_index;
extern float gMap_render_x;
extern float gMap_render_y;
extern float gMap_render_width;
extern float gMap_render_height;

void C2_HOOK_FASTCALL CalcGrafDataIndex(void);

#endif // REC2_GRAFDATA_H