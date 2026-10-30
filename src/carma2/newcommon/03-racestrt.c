/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "03-racestrt.h"

#include "rec2_types.h"

// GLOBAL: CARMA2_HW 0x006a0d38
// GLOBAL: CARMA2_SW 0x0069ad90
tU32 gLast_host_query;

// FUNCTION: CARMA2_HW 0x004e2b50
// FUNCTION: CARMA2_SW 0x004deed0
int C2_HOOK_CDECL SortGridFunction(const void* pFirst_one, const void* pSecond_one) {

    return ((tOpp_spec*)pFirst_one)->ranking - ((tOpp_spec*)pSecond_one)->ranking;
}

// SortOpponents

// DoGridPosition

// ReceivedViewCar

