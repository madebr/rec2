#ifndef C2_SYS_STAT_H
#define C2_SYS_STAT_H

#include <sys/stat.h>

#ifdef _WIN32
#define c2_stat _stat32

#define c2_stat32 _stat32
#else
#define c2_stat stat

#define c2_stat32 stat
#endif

#endif // C2_SYS_STAT_H
