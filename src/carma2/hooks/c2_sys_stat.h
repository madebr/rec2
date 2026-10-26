/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_SYS_STAT_H_
#define REC2_SYS_STAT_H_

#include <sys/types.h>
#include <sys/stat.h>

#include <sys/stat.h>

#ifdef _WIN32
#ifdef REC2_MATCHING
#define struct_c2_stat32 struct _stat
#define c2_fstat32 _fstat
#define c2_stat32 _stat
#else
#define struct_c2_stat32 struct _stat
#define c2_fstat32 _fstat
#define c2_stat32 _stat
#endif
#else
#define struct_c2_stat32 struct stat
#define c2_fstat32 fstat
#define c2_stat32 stat
#endif

#endif //REC2_SYS_STAT_H_
