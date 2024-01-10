#ifndef SRE_STDIO_H
#define SRE_STDIO_H

#ifndef _SRE_
#error "This file is only meant for SRE"
#endif

#ifdef TYPEDEF_FILE
typedef struct {
    void* ptr;
    int _cnt;
    void* _base;
    int flag;
    int file;
    int _charbuf;
    int _bufsiz;
    char* _tpfname;
} FILE;
#endif

#endif // SRE_STDIO_H
