/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_ANIMATION_H
#define REC2_ANIMATION_H

#include "rec2_types.h"

#include "c2_hooks.h"
#include "c2_stdio.h"

void C2_HOOK_FASTCALL ReadExplosion(FILE *pF, tExplosion_animation* pAnimation);

#endif // REC2_ANIMATION_H