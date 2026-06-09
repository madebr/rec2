#include "brstdfile.h"

#include "core/fw/diag.h"
#include "core/v1db/sys_conf.h"

#include <stdio.h>
#include "c2_string.h"

// Global variables

// GLOBAL: CARMA2_HW 0x0066fdc0
br_filesystem BrStdioFilesystem = {
    "Standard IO",
    BrStdioAttributes,
    BrStdioOpenRead,
    BrStdioOpenWrite,
    BrStdioClose,
    BrStdioEof,
    BrStdioGetChar,
    BrStdioPutChar,
    BrStdioRead,
    BrStdioWrite,
    BrStdioGetLine,
    BrStdioPutLine,
    BrStdioAdvance,
};

// GLOBAL: CARMA2_HW 0x0066fdf4
br_filesystem* _BrDefaultFilesystem = &BrStdioFilesystem;

// FUNCTION: CARMA2_HW 0x0053f660
br_uint_32 C2_HOOK_CDECL BrStdioAttributes(void) {
    return BR_FS_ATTR_READABLE | BR_FS_ATTR_WRITEABLE | BR_FS_ATTR_HAS_TEXT | BR_FS_ATTR_HAS_BINARY | BR_FS_ATTR_HAS_ADVANCE;
}

// FUNCTION: CARMA2_HW 0x0053f670
void* C2_HOOK_CDECL BrStdioOpenRead(const char* name, br_size_t n_magics, br_mode_test_cbfn* identify, int* mode_result) {
    FILE* fh;
    br_uint_8 magics[16];
    char brender_path[512];
    char path[512];
    const char *brender_path_ptr;
    int open_mode;

    open_mode = BR_FS_MODE_BINARY;
    strncpy(path, name, sizeof(path) - 1);
    fh = fopen(path, "rb");
    if (fh == NULL) {
        if (strchr(name, ':') != NULL || strchr(name, '/') != NULL || strchr(name, '\\') != NULL) {
            return NULL;
        }
        if (BrSystemConfigQueryString(BRT_BRENDER_PATH_STR, brender_path, sizeof(brender_path)) != 0) {
            return NULL;
        }
        brender_path_ptr = brender_path;
        while (*brender_path_ptr != '\0') {
            char *path_ptr = path;
            while (*brender_path_ptr != ';' && *brender_path_ptr != '\0') {
                *path_ptr++ = *brender_path_ptr++;
            }
            if (*brender_path_ptr == ';') {
                brender_path_ptr++;
            }
            if (path != path_ptr && path_ptr[-1] != ':'&& path_ptr[-1] != '/'&& path_ptr[-1] != '\\') {
                *path_ptr++ = '/';
            }
            strcpy(path_ptr, name);
            fh = fopen(path, "rb");
            if (fh != NULL) {
                break;
            }
        }
        if (fh == NULL) {
            return NULL;
        }
    }
    if (mode_result != NULL) {
        open_mode = *mode_result;
    }
    if (n_magics != 0) {
        if (fread(magics, 1u, n_magics, fh) != n_magics) {
            fclose(fh);
            return NULL;
        }
        if (identify != NULL) {
            open_mode = identify(magics, n_magics);
        }
        if (mode_result != NULL) {
            *mode_result = open_mode;
        }
    }

    fclose(fh);
    switch (open_mode) {
    case BR_FS_MODE_TEXT:
        fh = fopen(path, "r");
        break;
    case BR_FS_MODE_BINARY:
        fh = fopen(path, "rb");
        break;
    case BR_FS_MODE_UNKNOWN:
        fh = NULL;
        break;
    default:
        BrFailure("BrStdFileOpenRead: invalid open_mode %d", open_mode);
    }
    return fh;
}

// FUNCTION: CARMA2_HW 0x0053f8a0
void* C2_HOOK_CDECL BrStdioOpenWrite(const char* name, int mode) {
    FILE* fh;

    fh = fopen(name, mode == BR_FS_MODE_TEXT ? "w" : "wb");

    return fh;
}

// FUNCTION: CARMA2_HW 0x0053f8d0
void C2_HOOK_CDECL BrStdioClose(void* f) {

    fclose(f);
}

// FUNCTION: CARMA2_HW 0x0053f8e0
int C2_HOOK_CDECL BrStdioEof(void* f) {
    return feof((FILE*)f);
}

// FUNCTION: CARMA2_HW 0x0053f8f0
int C2_HOOK_CDECL BrStdioGetChar(void* f) {

    return getc((FILE *)f);
}

// FUNCTION: CARMA2_HW 0x0053f920
void C2_HOOK_CDECL BrStdioPutChar(int c, void* f) {
    fputc(c, f);
}

// FUNCTION: CARMA2_HW 0x0053f940
br_size_t C2_HOOK_CDECL BrStdioRead(void* buf, br_size_t size, unsigned int n, void* f) {
    int i;

    i = fread(buf, size, n, f);
    return i;
}

// FUNCTION: CARMA2_HW 0x0053f960
br_size_t C2_HOOK_CDECL BrStdioWrite(const void* buf, br_size_t size, unsigned int n, void* f) {

    return fwrite(buf, size, n, f);
}

// FUNCTION: CARMA2_HW 0x0053f980
br_size_t C2_HOOK_CDECL BrStdioGetLine(char* buf, br_size_t buf_len, void* f) {
    br_size_t l;

    if (fgets(buf, buf_len, f) == NULL) {
        return 0;
    }

    l = strlen(buf);

    if (l != 0 && buf[l - 1] == '\n') {
        buf[--l] = '\0';
    }

    return l;
}

// FUNCTION: CARMA2_HW 0x0053f9c0
void C2_HOOK_CDECL BrStdioPutLine(char* buf, void* f) {

    fputs(buf, f);
    fputc('\n', f);
}

// FUNCTION: CARMA2_HW 0x0053f9e0
void C2_HOOK_CDECL BrStdioAdvance(br_size_t count, void* f) {
    fseek(f, count, SEEK_CUR);
}
