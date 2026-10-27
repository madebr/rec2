/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef C2_STDLIB_H
#define C2_STDLIB_H

#include <stdlib.h>

#if defined(_MSC_VER) && _MSC_VER < 1300
#define c2_strtof(V,X) (float)strtod(V,X)
#else
#define c2_strtof strtof
#endif

#endif // C2_STDLIB_H
