#ifndef REC2_STDIO_H
#define REC2_STDIO_H

#include <stdio.h>

#define c2_fpos_t fpos_t

#define c2_stdin    (stdin)
#define c2_stdout   (stdout)
#define c2_stderr   (stderr)

#define c2_fopen fopen
#define c2_fclose fclose
#define c2_feof feof
#define c2_ferror ferror
#define c2_clearerr clearerr

#define c2_fflush fflush

#define c2_ftell ftell
#define c2_fseek fseek
#define c2_rewind rewind

#define c2_fgetc fgetc
#define c2_ungetc ungetc
#define c2_fgets fgets
#define c2_fputc fputc
#define c2_fputs fputs

#define c2_fgetpos fgetpos
#define c2_fsetpos fsetpos

#define c2_printf printf
#define c2_fprintf fprintf
#define c2_vfprintf vfprintf

#define c2_sprintf sprintf
#define c2_vsprintf vsprintf

#define c2_fwrite fwrite
#define c2_fread fread

#define c2_fscanf fscanf
#define c2_vfscanf vfscanf

#define c2_sscanf sscanf
#define c2_vsscanf vsscanf

#ifdef _WIN32
#define c2_unlink _unlink
#else
#include <unistd.h>
#define c2_unlink unlink
#endif
#define c2_rename rename

#endif //REC2_STDIO_H
