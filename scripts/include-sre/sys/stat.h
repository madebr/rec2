#ifndef SRE_SYS_STAT_H
#define SRE_SYS_STAT_H

#ifndef _SRE_
#error "This file is only meant for SRE"
#endif

struct c2_stat {
    unsigned int st_dev;
    unsigned short st_ino;
    unsigned short st_mode;
    short st_nlink;
    short st_uid;
    short st_gid;
    unsigned int st_rdev;
    long st_size;
    long st_atime;
    long st_mtime;
    long st_ctime;
};

#endif // SRE_SYS_STAT_H

