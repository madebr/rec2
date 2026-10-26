/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef D3D_TEXTURE_H
#define D3D_TEXTURE_H

#include "c2_hooks.h"

void C2_HOOK_CDECL TextureCacheD3DClearEntry(int index, int uid);

#endif /* D3D_TEXTURE_H */