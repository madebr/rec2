/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_25_GRAFDATA_H
#define GUARD_25_GRAFDATA_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gGraf_data_index;
extern tGraf_data* gCurrent_graf_data;

extern void C2_HOOK_FASTCALL CalcGrafDataIndex(void);

#endif // GUARD_25_GRAFDATA_H
