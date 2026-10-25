/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef __FILES___
#define __FILES___

#include "MacTypes.h"

typedef struct FSSpec {
    short               vRefNum;
    long                parID;
    StrFileName         name;
} FSSpec;

#endif /* __FILES___ */
