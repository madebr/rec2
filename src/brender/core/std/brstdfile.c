#include "brstdfile.h"

#include "core/fw/diag.h"

#include "c2_stdio.h"
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
    int open_mode;

    open_mode = BR_FS_MODE_BINARY;
    fh = c2_fopen(name, "rb");
    if (fh == NULL) {
        // skip logic around getting BRENDER_PATH from ini files etc
        return NULL;
    }

    if (n_magics != 0) {
        if (c2_fread(magics, 1u, n_magics, fh) != n_magics) {
            c2_fclose(fh);
            return 0;
        }
        if (identify != NULL) {
            open_mode = identify(magics, n_magics);
        }
        if (mode_result != NULL) {
            *mode_result = open_mode;
        }
    }

    c2_fclose(fh);
    if (open_mode == BR_FS_MODE_BINARY) {
        fh = c2_fopen(name, "rb");
    } else if (open_mode == BR_FS_MODE_TEXT) {
        fh = c2_fopen(name, "rt");
    } else {
        BrFailure("BrStdFileOpenRead: invalid open_mode %d", open_mode);
        return NULL;
    }
    return fh;
}

// FUNCTION: CARMA2_HW 0x0053f8a0
void* C2_HOOK_CDECL BrStdioOpenWrite(const char* name, int mode) {
    FILE* fh;

    if (mode == BR_FS_MODE_TEXT) {
        fh = c2_fopen(name, "w");
    } else {
        fh = c2_fopen(name, "wb");
    }

    return fh;
}

// FUNCTION: CARMA2_HW 0x0053f8d0
void C2_HOOK_CDECL BrStdioClose(void* f) {

    c2_fclose(f);
}

// FUNCTION: CARMA2_HW 0x0053f8e0
int C2_HOOK_CDECL BrStdioEof(void* f) {
    return c2_feof(f);
}

// FUNCTION: CARMA2_HW 0x0053f8f0
int C2_HOOK_CDECL BrStdioGetChar(void* f) {
    int c;

    c = c2_fgetc(f);
    return c;
}

// FUNCTION: CARMA2_HW 0x0053f920
void C2_HOOK_CDECL BrStdioPutChar(int c, void* f) {
    c2_fputc(c, f);
}

// FUNCTION: CARMA2_HW 0x0053f940
br_size_t C2_HOOK_CDECL BrStdioRead(void* buf, br_size_t size, unsigned int n, void* f) {
    int i;

    i = c2_fread(buf, size, n, f);
    return i;
}

// FUNCTION: CARMA2_HW 0x0053f960
br_size_t C2_HOOK_CDECL BrStdioWrite(const void* buf, br_size_t size, unsigned int n, void* f) {
    return c2_fwrite(buf, size, n, f);
}

// FUNCTION: CARMA2_HW 0x0053f980
br_size_t C2_HOOK_CDECL BrStdioGetLine(char* buf, br_size_t buf_len, void* f) {
    br_size_t l;

    if (c2_fgets(buf, buf_len, f) == NULL) {
        return 0;
    }

    l = c2_strlen(buf);

    if (l != 0 && buf[l - 1] == '\n') {
        buf[--l] = '\0';
    }

    return l;
}

// FUNCTION: CARMA2_HW 0x0053f9c0
void C2_HOOK_CDECL BrStdioPutLine(char* buf, void* f) {
    c2_fputs(buf, f);
    c2_fputc('\n', f);
}

// FUNCTION: CARMA2_HW 0x0053f9e0
void C2_HOOK_CDECL BrStdioAdvance(br_size_t count, void* f) {
    c2_fseek(f, count, SEEK_CUR);
}