#include "c2_stdio.h"

#if defined(C2_HOOKS_ENABLED)

#include "c2_hooks.h"

#include <stdarg.h>
#include <stdio.h>

#if defined(DEBUG_STDIO)
#define DEBUG_PRINTF(...) C2_HOOK_DEBUGF(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#endif

C2_HOOK_VARIABLE_IMPLEMENT(FILE, c2_stdin_value, 0x006737d0);
C2_HOOK_VARIABLE_IMPLEMENT(FILE, c2_stdout_value, 0x006737f0);
C2_HOOK_VARIABLE_IMPLEMENT(FILE, c2_stderr_value, 0x00673810);

#define HOOK_STDIO 1

#if HOOK_STDIO
struct c2_iobuf {
    char * _ptr;
    int _cnt;
    char * _base;
    int _flag;
    int _file;
    int _charbuf;
    int _bufsiz;
    char * _tmpfname;
};
#define C2_IOBUF_EOF 0x10
#define C2_IOBUF_ERROR 0x20
#endif

#if !defined(HOOK_STDIO)
static FILE* hook_FILE(FILE* file) {
    if (file == c2_stdin) {
        return stdin;
    }
    if (file == c2_stdout) {
        return stdout;
    }
    if (file == c2_stderr) {
        return stderr;
    }
    return file;
}
#endif

static FILE* (C2_HOOK_CDECL * fopen_original)(const char*, const char*);
FILE* C2_HOOK_CDECL c2_fopen(const char* path, const char* mode) {
    FILE* res;
    DEBUG_PRINTF("(\"%s\", \"%s\")", path, mode);
#if HOOK_STDIO
    res = fopen_original(path, mode);
#else
    res = fopen(path, mode);
#endif
    DEBUG_PRINTF("-> %p", res);
    return res;
}
C2_HOOK_FUNCTION_ORIGINAL(0x00577170, c2_fopen, fopen_original)

static void (C2_HOOK_CDECL * fclose_original)(FILE* file);
void C2_HOOK_CDECL c2_fclose(FILE* file) {
    DEBUG_PRINTF("(%p)", file);
#if HOOK_STDIO
    fclose_original(file);
#else
    fclose(hook_FILE(file));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00577640, c2_fclose, fclose_original)

int C2_HOOK_CDECL c2_feof(FILE* file) {
#if HOOK_STDIO
    return ((struct c2_iobuf*)file)->_flag & C2_IOBUF_EOF;
#else
    return feof(file);
#endif
}

int C2_HOOK_CDECL c2_ferror(FILE* file) {
#if HOOK_STDIO
    return ((struct c2_iobuf*)file)->_flag & C2_IOBUF_ERROR;
#else
    return ferror(file);
#endif
}

void (C2_HOOK_CDECL * c2_clearerr_original)(FILE* file);
void C2_HOOK_CDECL c2_clearerr(FILE* file) {
#if HOOK_STDIO
    c2_clearerr_original(file);
#else
    clearerr(file);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00578690, c2_clearerr, c2_clearerr_original)

static int(C2_HOOK_CDECL * fflush_original)(FILE* file);
int C2_HOOK_CDECL c2_fflush(FILE* file) {
    DEBUG_PRINTF("(%p)", file);
#if HOOK_STDIO
    return fflush_original(file);
#else
    return fflush(hook_FILE(file));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00577700, c2_fflush, fflush_original)

int (C2_HOOK_CDECL * ftell_original)(FILE*);
int C2_HOOK_CDECL c2_ftell(FILE* file) {
    DEBUG_PRINTF("(%p)", file);
#if HOOK_STDIO
    return ftell_original(file);
#else
    return ftell(hook_FILE(file));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005772f0, c2_ftell, ftell_original)

int (C2_HOOK_CDECL * fseek_original)(FILE *stream, int offset, int whence);
int C2_HOOK_CDECL c2_fseek(FILE *stream, int offset, int whence) {

    DEBUG_PRINTF("(%p, %d, %d)", stream, offset, whence);
#if HOOK_STDIO
    return fseek_original(stream, offset, whence);
#else
    return fseek(hook_FILE(stream), offset, whence);
#endif
}

C2_HOOK_FUNCTION_ORIGINAL(0x00576ec0, c2_fseek, fseek_original)
void (C2_HOOK_CDECL * rewind_original)(FILE *stream);
void C2_HOOK_CDECL c2_rewind(FILE *stream) {

    DEBUG_PRINTF("(%p)", stream);
#if HOOK_STDIO
    rewind_original(stream);
#else
    rewind(hook_FILE(stream));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005770e0, c2_rewind, rewind_original)

int (C2_HOOK_CDECL * c2_fgetc_original)(FILE * stream);
int C2_HOOK_CDECL c2_fgetc(FILE * stream) {

    DEBUG_PRINTF("(%p)", stream);
#if HOOK_STDIO
    return c2_fgetc_original(stream);
#else
    return fgetc(stream);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00576e80, c2_fgetc, c2_fgetc_original)

int (C2_HOOK_CDECL * c2_ungetc_original)(int ch, FILE* file);
int C2_HOOK_CDECL c2_ungetc(int ch, FILE* file) {

    DEBUG_PRINTF("(%d, %p)", ch, file);
#if HOOK_STDIO
    return c2_ungetc_original(ch, file);
#else
    return ungetc(ch, file);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00576df0, c2_ungetc, c2_ungetc_original)

char* (C2_HOOK_CDECL * fgets_original)(char* str, size_t num, FILE* stream);
char* C2_HOOK_CDECL c2_fgets(char* str, size_t num, FILE* stream) {
    DEBUG_PRINTF("(\"%s\", %d, %p)", str, num, stream);
#if HOOK_STDIO
    return fgets_original(str, num, stream);
#else
    return fgetc(stream);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00578610, c2_fgets, fgets_original)

static int (C2_HOOK_CDECL * fputc_original)(int character, FILE* file);
int C2_HOOK_CDECL c2_fputc(int character, FILE* file) {
    DEBUG_PRINTF("(%d, %p)", character, file);
#if HOOK_STDIO
    return fputc_original(character, file);
#else
    return fputc(character, hook_FILE(file));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00577c40, c2_fputc, fputc_original);

static int (C2_HOOK_CDECL * fputs_original)(const char*, FILE*);
int C2_HOOK_CDECL c2_fputs(const char* text, FILE* file) {
    DEBUG_PRINTF("(%p)", file);
#if HOOK_STDIO
    return fputs_original(text, file);
#else
    return fputs(text, hook_FILE(file));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00577ef0, c2_fputs, fputs_original);

int (C2_HOOK_CDECL * c2_fgetpos_original)(FILE* file, fpos_t* pos);
int C2_HOOK_CDECL c2_fgetpos(FILE* file, fpos_t* pos) {
    DEBUG_PRINTF("(%p, %p)", file, pos);
#if HOOK_STDIO
    return c2_fgetpos_original(file, pos);
#else
    return fgetpos(file, pos);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005772c0, c2_fgetpos, c2_fgetpos_original);

int (C2_HOOK_CDECL * c2_fsetpos_original)(FILE* file, fpos_t* pos);
int C2_HOOK_CDECL c2_fsetpos(FILE* file, fpos_t* pos) {
    DEBUG_PRINTF("(%p, %p)", file, pos);
#if HOOK_STDIO
    return c2_fsetpos_original(file, pos);
#else
    return fsetpos(file, pos);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005772a0, c2_fsetpos, c2_fsetpos_original);

static int (C2_HOOK_CDECL * printf_original)(const char* format, ...);
int C2_HOOK_CDECL c2_printf(const char* format, ...) {
    va_list ap;
    int res;

    DEBUG_PRINTF("(%s, ...)", format);
    va_start(ap, format);
#if HOOK_STDIO
    res = c2_vfprintf(c2_stdout, format, ap);
#else
    res = vfprintf(stdout, format, ap);
#endif
    va_end(ap);
    return res;
}
C2_HOOK_FUNCTION_ORIGINAL(0x00577930, c2_printf, printf_original);

static int (C2_HOOK_CDECL * fprintf_original)(FILE*, const char* format, ...);
int C2_HOOK_CDECL c2_fprintf(FILE* file, const char* format, ...) {
    va_list ap;
    int res;

    DEBUG_PRINTF("(%p, \"%s\", ...)", file, format);
    va_start(ap, format);
#if HOOK_STDIO
    res = c2_vfprintf(file, format, ap);
#else
    res = vfprintf(hook_FILE(file), format, ap);
#endif
    va_end(ap);
    return res;
}
C2_HOOK_FUNCTION_ORIGINAL(0x00576d00, c2_fprintf, fprintf_original);

int C2_HOOK_CDECL c2_sprintf(char* str, const char* format, ...) {
    va_list ap;
    int res;

    DEBUG_PRINTF("(\"%s\", \"%s\", ...)", str, format);

    va_start(ap, format);
    res = vsprintf(str, format, ap);
    va_end(ap);
    return res;
}
C2_HOOK_FUNCTION(0x00575de0, c2_sprintf)


int C2_HOOK_CDECL c2_vsprintf(char* str, const char* format, va_list ap) {

    DEBUG_PRINTF("(\"%s\", \"%s\", ...)", str, format);
    return vsprintf(str, format, ap);
}
C2_HOOK_FUNCTION(0x00578ee0, c2_vsprintf)

int (C2_HOOK_CDECL * vfprintf_original)(FILE*, const char*, va_list ap);
int C2_HOOK_CDECL c2_vfprintf(FILE* file, const char* format, va_list ap) {
    int res;

    DEBUG_PRINTF("(%p, \"%s\", ...)", file, format);
#if HOOK_STDIO
    res = vfprintf_original(file, format, ap);
#else
    res = vfprintf(hook_FILE(file), format, ap);
#endif
    return res;
}
C2_HOOK_FUNCTION_ORIGINAL(0x0057b800, c2_vfprintf, vfprintf_original);

size_t (C2_HOOK_CDECL * fwrite_original)(const void* ptr, size_t size, size_t count, FILE* stream);
size_t C2_HOOK_CDECL c2_fwrite(const void* ptr, size_t size, size_t count, FILE* stream) {
    size_t res;

    DEBUG_PRINTF("(%p, %d, %d, %p)", ptr, size, count, stream);
#if HOOK_STDIO
    res = fwrite_original(ptr, size, count, stream);
#else
    res = fwrite(ptr, size, count, hook_FILE(stream));
#endif
    DEBUG_PRINTF("-> %d", res);
    return res;
}
C2_HOOK_FUNCTION_ORIGINAL(0x005774a0, c2_fwrite, fwrite_original)

size_t (C2_HOOK_CDECL * fread_original)(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t C2_HOOK_CDECL c2_fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t res;

    DEBUG_PRINTF("(%p, %d, %d, %p)", ptr, size, nmemb, stream);
#if HOOK_STDIO
    res = fread_original(ptr, size, nmemb, stream);
#else
    res = fread(ptr, size, nmemb, hook_FILE(stream));
#endif
//    DEBUG_PRINTF("-> %d", res);
    return res;
}
C2_HOOK_FUNCTION_ORIGINAL(0x00576fa0, c2_fread, fread_original)

int (C2_HOOK_CDECL * fscanf_original)(FILE* file, const char* format, ...);
int C2_HOOK_CDECL c2_fscanf(FILE* file, const char* format, ...) {
    int res;
    va_list ap;

    va_start(ap, format);
#if HOOK_STDIO
    res = c2_vfscanf(file, format, ap);
#else
    res = vfscanf(file, format, ap);
#endif
    va_end(ap);

    return res;
}
C2_HOOK_FUNCTION_ORIGINAL(0x00577c20, c2_fscanf, fscanf_original)

int (C2_HOOK_CDECL * vfscanf_original)(FILE* file, const char* format, va_list ap);
int C2_HOOK_CDECL c2_vfscanf(FILE* file, const char* format, va_list ap) {
    int res;

#if HOOK_STDIO
    res = vfscanf_original(file, format, ap);
#else
    res = vfscanf(file, format, ap);
#endif
    return res;
}
C2_HOOK_FUNCTION_ORIGINAL(0x0057a8b0, c2_vfscanf, vfscanf_original)

int C2_HOOK_CDECL c2_sscanf(const char* str, const char* format, ...) {
    va_list ap;
    int result;

    va_start(ap, format);
    result = vsscanf(str, format, ap);
    va_end(ap);
    return result;
}
C2_HOOK_FUNCTION(0x00575cb0, c2_sscanf)

int C2_HOOK_CDECL c2_vsscanf(const char* str, const char* format, va_list ap) {
    return vsscanf(str, format, ap);
}

int C2_HOOK_CDECL c2_unlink(const char* path) {
    return _unlink(path);
}
C2_HOOK_FUNCTION(0x005776c0, c2_unlink)

int C2_HOOK_CDECL c2_rename(const char *oldname, const char *newname) {
    return rename(oldname, newname);
}
C2_HOOK_FUNCTION(0x00578470, c2_rename)

#endif
