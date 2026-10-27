/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "loadtif_zlib.h"

#include <zlib.h>

#include "c2_string.h"
#include <stdio.h>

// GLOBAL: CARMA2_HW 0x006631c0
// GLOBAL: CARMA2_SW 0x0065d958
br_filesystem gZlib_filesystem = {
    "Zlib filesystem",
    ZlibFsGetAttributes,
    ZlibFsOpenRead,
    ZlibFsOpenWrite,
    ZlibFsClose,
    ZlibFsEof,
    ZlibFsGetChr,
    ZlibFsPutChr,
    ZlibFsRead,
    ZlibFsWrite,
    ZlibFsGetLine,
    ZlibFsPutLine,
    ZlibFsAdvance,
};

// FUNCTION: CARMA2_HW 0x0051db90
// FUNCTION: CARMA2_SW 0x00519230
br_uint_32 C2_HOOK_CDECL ZlibFsGetAttributes(void) {

    return BR_FS_ATTR_READABLE | BR_FS_ATTR_WRITEABLE | BR_FS_ATTR_HAS_BINARY;
}

// FUNCTION: CARMA2_HW 0x0051dc10
// FUNCTION: CARMA2_SW 0x005192b0
void* C2_HOOK_CDECL ZlibFsOpenRead(const char* buffer, br_size_t capacity, br_mode_test_cbfn* cbfn, int* type) {

    return gzopen(buffer, "rb");
}

// FUNCTION: CARMA2_HW 0x0051dc20
// FUNCTION: CARMA2_SW 0x005192c0
void* C2_HOOK_CDECL ZlibFsOpenWrite(const char* path, int type) {

    if (type != BR_FS_MODE_BINARY) {
        return NULL;
    }
    return gzopen(path, "wb");
}

// FUNCTION: CARMA2_HW 0x0051dba0
// FUNCTION: CARMA2_SW 0x00519240
void C2_HOOK_CDECL ZlibFsClose(void* context) {

    gzclose(context);
}

// FUNCTION: CARMA2_HW 0x0051dbb0
// FUNCTION: CARMA2_SW 0x00519250
int C2_HOOK_CDECL ZlibFsEof(void* context) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0051db40
// FUNCTION: CARMA2_SW 0x005191e0
int C2_HOOK_CDECL ZlibFsGetChr(void* context) {
    char chr;

    if (gzread(context, &chr, 1) <= 0) {
        return EOF;
    }
    return chr;
}

// FUNCTION: CARMA2_HW 0x0051dc40
// FUNCTION: CARMA2_SW 0x005192e0
void C2_HOOK_CDECL ZlibFsPutChr(int chr, void* context) {
    char data[1];

    data[0] = chr;
    gzwrite(context, data, sizeof(data));
}

// FUNCTION: CARMA2_HW 0x0051dca0
// FUNCTION: CARMA2_SW 0x00519340
br_size_t C2_HOOK_CDECL ZlibFsRead(void* buffer, br_size_t elem_size, unsigned int count, void* context) {
    int bytes_read;

    bytes_read = gzread(context, buffer, elem_size * count);
    if (bytes_read < 0) {
        return 0;
    }
    return bytes_read / elem_size;
}

// FUNCTION: CARMA2_HW 0x0051dcd0
// FUNCTION: CARMA2_SW 0x00519370
br_size_t C2_HOOK_CDECL ZlibFsWrite(const void* buffer, br_size_t elem_size, unsigned int count, void* context) {
    int bytes_written;

    bytes_written = gzwrite(context, buffer, elem_size * count);
    return bytes_written / elem_size;
}

// FUNCTION: CARMA2_HW 0x0051dbc0
// FUNCTION: CARMA2_SW 0x00519260
br_size_t C2_HOOK_CDECL ZlibFsGetLine(char* buffer, br_size_t capacity, void* context) {
    char char_buffer[1];
    int chr;
    size_t i;

    for (i = 0; i < capacity - 1; i++) {
        if (gzread(context, char_buffer, sizeof(char_buffer)) <= 0) {
            chr = -1;
        } else {
            chr = char_buffer[0];
        }
        if (chr == EOF) {
            return i;
        }
        if (chr == '\n') {
            return i;
        }
        buffer[i] = chr;
    }
    return i;
}

// FUNCTION: CARMA2_HW 0x0051dc60
// FUNCTION: CARMA2_SW 0x00519300
void C2_HOOK_CDECL ZlibFsPutLine(char* line, void* context) {
    char newline_buffer[1];

    gzwrite(context, line, strlen(line));
    newline_buffer[0] = '\n';
    gzwrite(context, newline_buffer, sizeof(newline_buffer));
}

// FUNCTION: CARMA2_HW 0x0051db60
// FUNCTION: CARMA2_SW 0x00519200
void C2_HOOK_CDECL ZlibFsAdvance(br_size_t advance, void* context) {
    size_t i;
    char buffer[1];

    for (i = 0; i != advance; i++) {
        gzread(context, buffer, sizeof(buffer));
    }
}
