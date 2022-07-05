#ifndef REC2_BRSTDLIB_H
#define REC2_BRSTDLIB_H

#include <stdarg.h>

void BrAbort(void);

char* BrStrCpy(char* s1, const char* s2);

int BrVSprintf(char* buf, const char* fmt, va_list args);

#endif //REC2_BRSTDLIB_H
