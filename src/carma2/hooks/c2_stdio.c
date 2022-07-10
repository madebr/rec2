#include "c2_stdio.h"

#if defined(C2_HOOKS_ENABLED)

#include "c2_hooks.h"

#include <stdarg.h>
#include <stdio.h>

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
#endif

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

static FILE* (C2_HOOK_CDECL * fopen_original)(const char*, const char*);
FILE* C2_HOOK_CDECL c2_fopen(const char* path, const char* mode) {
    FILE* res;
    C2_HOOK_DEBUGF("(\"%s\", \"%s\")", path, mode);
#if HOOK_STDIO
    res = fopen_original(path, mode);
#else
    res = fopen(path, mode);
#endif
    C2_HOOK_DEBUGF("-> %p", res);
    return res;
}
C2_HOOK_FUNCTION_ORIGINAL(0x00577170, c2_fopen, fopen_original)

static void (C2_HOOK_CDECL * fclose_original)(FILE* file);
void C2_HOOK_CDECL c2_fclose(FILE* file) {
    C2_HOOK_DEBUGF("(%p)", file);
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

static int(C2_HOOK_CDECL * fflush_original)(FILE* file);
int C2_HOOK_CDECL c2_fflush(FILE* file) {
    C2_HOOK_DEBUGF("(%p)", file);
#if HOOK_STDIO
    return fflush_original(file);
#else
    return fflush(hook_FILE(file));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00577700, c2_fflush, fflush_original)

int (C2_HOOK_CDECL * ftell_original)(FILE*);
int c2_ftell(FILE* file) {
    C2_HOOK_DEBUGF("(%p)", file);
#if HOOK_STDIO
    return ftell_original(file);
#else
    return ftell(hook_FILE(file));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005772f0, c2_ftell, ftell_original)

int (C2_HOOK_CDECL * fseek_original)(FILE *stream, int offset, int whence);
int C2_HOOK_CDECL c2_fseek(FILE *stream, int offset, int whence) {

    C2_HOOK_DEBUGF("(%p, %d, %d)", stream, offset, whence);
#if HOOK_STDIO
    return fseek_original(stream, offset, whence);
#else
    return fseek(hook_FILE(stream), offset, whence);
#endif
}

C2_HOOK_FUNCTION_ORIGINAL(0x00576ec0, c2_fseek, fseek_original)
void (C2_HOOK_CDECL * rewind_original)(FILE *stream);
void C2_HOOK_CDECL c2_rewind(FILE *stream) {

    C2_HOOK_DEBUGF("(%p)", stream);
#if HOOK_STDIO
    rewind_original(stream);
#else
    rewind(hook_FILE(stream));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005770e0, c2_rewind, rewind_original)

int (C2_HOOK_CDECL * fgetc_original)(FILE * stream);
int C2_HOOK_CDECL c2_fgetc(FILE * stream) {

    C2_HOOK_DEBUGF("(%p)", stream);
#if HOOK_STDIO
    return fgetc_original(stream);
#else
    return fgetc(stream);
#endif
}

char* (C2_HOOK_CDECL * fgets_original)(char* str, int num, FILE* stream);
char* C2_HOOK_CDECL c2_fgets(char* str, int num, FILE* stream) {
    C2_HOOK_DEBUGF("(%p)", stream);
#if HOOK_STDIO
    return fgets_original(str, num, stream);
#else
    return fgetc(stream);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00578610, c2_fgets, fgets_original)

static int (C2_HOOK_CDECL * fputc_original)(int character, FILE* file);
int c2_fputc(int character, FILE* file) {
    C2_HOOK_DEBUGF("(%d, %p)", character, file);
#if HOOK_STDIO
    return fputc_original(character, file);
#else
    return fputc(character, hook_FILE(file));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00577c40, c2_fputc, fputc_original);

static int (C2_HOOK_CDECL * fputs_original)(const char*, FILE*);
int c2_fputs(const char* text, FILE* file) {
    C2_HOOK_DEBUGF("(\"%s\", %p)", text, file);
#if HOOK_STDIO
    return fputs_original(text, file);
#else
    return fputs(text, hook_FILE(file));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00577ef0, c2_fputs, fputs_original);

static int (C2_HOOK_CDECL * printf_original)(const char* format, ...);
int c2_printf(const char* format, ...) {
    va_list ap;
    int res;

    C2_HOOK_DEBUGF("(%s, ...)", format);
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

    C2_HOOK_DEBUGF("(%p, \"%s\", ...)", file, format);
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
    va_start(ap, format);

    C2_HOOK_DEBUGF("(\"%s\", \"%s\", ...)", str, format);
    return vsprintf(str, format, ap);
            va_end(ap);
}
C2_HOOK_FUNCTION(0x00575de0, c2_sprintf)


int C2_HOOK_CDECL c2_vsprintf(char* str, const char* format, va_list ap) {

    C2_HOOK_DEBUGF("(\"%s\", \"%s\", ...)", str, format);
    return vsprintf(str, format, ap);
}
C2_HOOK_FUNCTION(0x00578ee0, c2_vsprintf)

int (C2_HOOK_CDECL * vfprintf_original)(FILE*, const char*, va_list ap);
int c2_vfprintf(FILE* file, const char* format, va_list ap) {
    int res;

    C2_HOOK_DEBUGF("(%p, \"%s\", ...)", file, format);
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

    C2_HOOK_DEBUGF("(%p, %d, %d, %p)", ptr, size, count, stream);
#if HOOK_STDIO
    res = fwrite_original(ptr, size, count, stream);
#else
    res = fwrite(ptr, size, count, hook_FILE(stream));
#endif
    C2_HOOK_DEBUGF("-> %d", res);
    return res;
}
C2_HOOK_FUNCTION_ORIGINAL(0x005774a0, c2_fwrite, fwrite_original)

size_t (C2_HOOK_CDECL * c2_fread_original)(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t C2_HOOK_CDECL c2_fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t res;

//    C2_HOOK_DEBUGF("(%p, %d, %d, %p)", ptr, size, nmemb, stream);
#if HOOK_STDIO
    res = c2_fread_original(ptr, size, nmemb, stream);
#else
    res = fread(ptr, size, nmemb, hook_FILE(stream));
#endif
//    C2_HOOK_DEBUGF("-> %d", res);
    return res;

}
C2_HOOK_FUNCTION_ORIGINAL(0x00576fa0, c2_fread, c2_fread_original)

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

#endif
