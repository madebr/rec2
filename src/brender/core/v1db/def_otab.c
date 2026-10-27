/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "def_otab.h"


// GLOBAL: CARMA2_HW 0x006ad528
br_primitive* _default_table[256];

// GLOBAL: CARMA2_HW 0x00665458
br_order_table _BrDefaultOrderTable = {
    _default_table,
    BR_ASIZE(_default_table),
    NULL,
    1.f,
    10.f,
    0.f,
    1.f/9.f,
    0,
    1,
    0,
};
