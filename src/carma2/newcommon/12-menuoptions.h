/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_12_MENUOPTIONS_H
#define GUARD_12_MENUOPTIONS_H

#include "c2_hooks.h"
#include "rec2_types.h"

// SaveOrigKeyMapping

// SaveKeyMapping

// ChangeKeyMapIndex

extern void C2_HOOK_FASTCALL GetGraphicsSettingsData(tFrontend_spec* pFrontend);

extern void C2_HOOK_FASTCALL SetGraphicsSettingsData(tFrontend_spec* pFrontend);

#endif // GUARD_12_MENUOPTIONS_H
