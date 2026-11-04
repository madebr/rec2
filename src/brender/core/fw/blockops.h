/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_BLOCKOPS_H
#define REC2_BLOCKOPS_H

#include "c2_hooks.h"

extern void C2_HOOK_CDECL BrBlockFill(void* pDestination, int pValue, int pCount_dwords);

extern void C2_HOOK_CDECL BrBlockCopy(void* pDestination, const void* pSource, int pCount_dwords);

extern void C2_HOOK_CDECL BrFarBlockCopy(void *pDestination, const void *pSource, int pCount_dwords);

#endif /* REC2_BLOCKOPS_H */
