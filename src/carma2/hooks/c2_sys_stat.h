#ifndef REC2_SYS_STAT_H_
#define REC2_SYS_STAT_H_

#include <sys/types.h>
#include <sys/stat.h>

#include <sys/stat.h>

#ifdef _WIN32
#define c2_fstat32 _fstat32
#else
#define c2_fstat32 fstat
#define _stat32 stat
#endif

#endif //REC2_SYS_STAT_H_
