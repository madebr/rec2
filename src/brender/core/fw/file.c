#include "file.h"

#include "fwsetup.h"
#include "resource.h"
#include "scratch.h"

#include "core/std/brstdlib.h"

#include <stdarg.h>
#include <stddef.h>

void C2_HOOK_CDECL _BrFileFree(void* res, br_uint_8 res_class, br_size_t size) {
    br_file* file;

    file = (br_file*)res;
    C2V(fw).fsys->close(file->raw_file);
}
C2_HOOK_FUNCTION(0x0052cc60, _BrFileFree)

br_uint_32 C2_HOOK_CDECL BrFileAttributes(void) {

    return C2V(fw).fsys->attributes();
}
C2_HOOK_FUNCTION(0x0052cc80, BrFileAttributes)

void* C2_HOOK_CDECL BrFileOpenRead(char* name, br_size_t n_magics, br_mode_test_cbfn* mode_test, int* mode_result) {
    void* raw_file;
    br_file* file;
    int bin_mode;

    bin_mode = 0;
    if (mode_result != NULL) {
        raw_file = C2V(fw).fsys->open_read(name, n_magics, mode_test, mode_result);
    } else {
        raw_file = C2V(fw).fsys->open_read(name, n_magics, mode_test, &bin_mode);
    }

    if (raw_file == NULL) {
        return NULL;
    }
    file = BrResAllocate(C2V(fw).res, sizeof(br_file) + BrStrLen(name) + 1, BR_MEMORY_FILE);

    file->writing = 0;
    if (mode_result != 0) {
        file->mode = *mode_result;
    } else {
        file->mode = 0;
    }
    file->raw_file = raw_file;
    BrStrCpy(file->name, name);
    return file;
}
C2_HOOK_FUNCTION(0x0052cc90, BrFileOpenRead)

void* C2_HOOK_CDECL BrFileOpenWrite(char* name, int mode) {
    void* raw_file;
    br_file* file;

    raw_file = C2V(fw).fsys->open_write(name, mode);
    if (raw_file == NULL) {
        return NULL;
    }
    file = BrResAllocate(C2V(fw).res, sizeof(br_file) + BrStrLen(name) + 1, BR_MEMORY_FILE);
    file->writing = 1;
    file->mode = mode;
    file->raw_file = raw_file;
    BrStrCpy(file->name, name);
    return file;
}
C2_HOOK_FUNCTION(0x0052cd40, BrFileOpenWrite)

void C2_HOOK_CDECL BrFileClose(void* f) {
    BrResFree(f);
}
C2_HOOK_FUNCTION(0x0052cdb0, BrFileClose)

int C2_HOOK_CDECL BrFileEof(void* f) {
    return C2V(fw).fsys->eof(((br_file*)f)->raw_file);
}
C2_HOOK_FUNCTION(0x0052cdc0, BrFileEof)

int C2_HOOK_CDECL BrFileGetChar(void* f) {
    return C2V(fw).fsys->getchr(((br_file*)f)->raw_file);
}
C2_HOOK_FUNCTION(0x0052cde0, BrFileGetChar)

void C2_HOOK_CDECL BrFilePutChar(int c, void* f) {
    C2V(fw).fsys->putchr(c, ((br_file*)f)->raw_file);
}
C2_HOOK_FUNCTION(0x0052ce00, BrFilePutChar)

int C2_HOOK_CDECL BrFileRead(void* buf, int size, int n, void* f) {
    return C2V(fw).fsys->read(buf, size, n, ((br_file*)f)->raw_file);
}
C2_HOOK_FUNCTION(0x0052ce20, BrFileRead)

int C2_HOOK_CDECL BrFileWrite(void* buf, int size, int n, void* f) {
    return C2V(fw).fsys->write(buf, size, n, ((br_file*)f)->raw_file);
}
C2_HOOK_FUNCTION(0x0052ce50, BrFileWrite)

int C2_HOOK_CDECL BrFileGetLine(char* buf, br_size_t buf_len, void* f) {
    return C2V(fw).fsys->getline(buf, buf_len, ((br_file*)f)->raw_file);
}
C2_HOOK_FUNCTION(0x0052ce80, BrFileGetLine)

void C2_HOOK_CDECL BrFilePutLine(char* buf, void* f) {
    C2V(fw).fsys->putline(buf, ((br_file*)f)->raw_file);
}
C2_HOOK_FUNCTION(0x0052cea0, BrFilePutLine)

void C2_HOOK_CDECL BrFileAdvance(long count, void* f) {
    C2V(fw).fsys->advance(count, ((br_file*)f)->raw_file);
}
C2_HOOK_FUNCTION(0x0052cec0, BrFileAdvance)

int C2_HOOK_CDECL BrFilePrintf(void* f, char* fmt, ...) {
    int n;
    va_list args;

    va_start(args, fmt);
    n = BrVSprintf(BrScratchString(), fmt, args);
    va_end(args);
    BrFileWrite(BrScratchString(), 1, n, f);
    return n;
}
C2_HOOK_FUNCTION(0x0052cee0, BrFilePrintf)
