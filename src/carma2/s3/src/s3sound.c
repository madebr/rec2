/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "../include/s3/internal/s3sound.h"

// this function was only called in Carmageddon I DOS build
// FUNCTION: CARMA2_HW 0x0056990f
// FUNCTION: CARMA2_SW 0x00564faf
int C2_HOOK_FASTCALL S3CreateTypeStructs(tS3_channel* chan) {

    return 1;
}
