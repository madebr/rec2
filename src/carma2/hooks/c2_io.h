/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_IO_H_
#define REC2_IO_H_

#ifdef _MSC_VER
#include <io.h>

#define c2_open _open

#define c2_close _close

#define c2_read _read
#else
#include <fcntl.h>
#include <unistd.h>

#ifndef _O_BINARY
#define _O_BINARY 0
#endif

#define c2_open open

#define c2_close close

#define c2_read read

#endif

#endif //REC2_IO_H_
