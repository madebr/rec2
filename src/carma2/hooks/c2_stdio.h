/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_STDIO_H
#define REC2_STDIO_H

#include <stdio.h>

#define fpos_t fpos_t



#ifdef _WIN32
#define c2_unlink _unlink
#else
#include <unistd.h>
#define c2_unlink unlink
#endif
#define c2_rename rename

#endif //REC2_STDIO_H
