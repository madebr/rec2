/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef C2_SYS_STAT_H
#define C2_SYS_STAT_H

#include <sys/stat.h>

#ifdef _WIN32
#define c2_stat _stat32

#define c2_stat32 _stat32
#else
#define c2_stat stat

#define c2_stat32 stat
#endif

#endif // C2_SYS_STAT_H
