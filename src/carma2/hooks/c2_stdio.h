#ifndef REC2_STDIO_H
#define REC2_STDIO_H

#include <stdio.h>

#define fpos_t fpos_t



#ifdef _WIN32
#define c2_unlink _unlink
#else
#include <unistd.h>
#define c2_unlink unlink
#endif
#define c2_rename rename

#endif //REC2_STDIO_H
