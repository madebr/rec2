#include "brstdfile.h"

#include "core/fw/diag.h"

#include "c2_stdio.h"
#include "c2_string.h"

// Global variables
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_filesystem, BrStdioFilesystem, 0x0066fdc0, {
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
    BrStdioAdvance
});
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_filesystem*, _BrDefaultFilesystem, 0x0066fdf4, &C2V(BrStdioFilesystem));

br_uint_32 C2_HOOK_CDECL BrStdioAttributes(void) {
    return BR_FS_ATTR_READABLE | BR_FS_ATTR_WRITEABLE | BR_FS_ATTR_HAS_TEXT | BR_FS_ATTR_HAS_BINARY | BR_FS_ATTR_HAS_ADVANCE;
}
C2_HOOK_FUNCTION(0x0053f660, BrStdioAttributes)

void* C2_HOOK_CDECL BrStdioOpenRead(char* name, br_size_t n_magics, br_mode_test_cbfn* identify, int* mode_result) {
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
C2_HOOK_FUNCTION(0x0053f670, BrStdioOpenRead)

void* C2_HOOK_CDECL BrStdioOpenWrite(char* name, int mode) {
    FILE* fh;

    if (mode == BR_FS_MODE_TEXT) {
        fh = c2_fopen(name, "w");
    } else {
        fh = c2_fopen(name, "wb");
    }

    return fh;
}
C2_HOOK_FUNCTION(0x0053f8a0, BrStdioOpenWrite)

void C2_HOOK_CDECL BrStdioClose(void* f) {

    c2_fclose(f);
}
C2_HOOK_FUNCTION(0x0053f8d0, BrStdioClose)

int C2_HOOK_CDECL BrStdioEof(void* f) {
    return c2_feof(f);
}
C2_HOOK_FUNCTION(0x0053f8e0, BrStdioEof)

int C2_HOOK_CDECL BrStdioGetChar(void* f) {
    int c;

    c = c2_fgetc(f);
    return c;
}
C2_HOOK_FUNCTION(0x0053f8f0, BrStdioGetChar)

void C2_HOOK_CDECL BrStdioPutChar(int c, void* f) {
    c2_fputc(c, f);
}
C2_HOOK_FUNCTION(0x0053f920, BrStdioPutChar)

br_size_t C2_HOOK_CDECL BrStdioRead(void* buf, br_size_t size, unsigned int n, void* f) {
    int i;

    i = c2_fread(buf, size, n, f);
    return i;
}
C2_HOOK_FUNCTION(0x0053f940, BrStdioRead)

br_size_t C2_HOOK_CDECL BrStdioWrite(void* buf, br_size_t size, unsigned int n, void* f) {
    return c2_fwrite(buf, size, n, f);
}
C2_HOOK_FUNCTION(0x0053f960, BrStdioWrite)

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
C2_HOOK_FUNCTION(0x0053f980, BrStdioGetLine)

void C2_HOOK_CDECL BrStdioPutLine(char* buf, void* f) {
    c2_fputs(buf, f);
    c2_fputc('\n', f);
}
C2_HOOK_FUNCTION(0x0053f9c0, BrStdioPutLine)

void C2_HOOK_CDECL BrStdioAdvance(br_size_t count, void* f) {
    c2_fseek(f, count, SEEK_CUR);
}
C2_HOOK_FUNCTION(0x0053f9e0, BrStdioAdvance)
