#ifndef REC2_TYPES_H
#define REC2_TYPES_H

typedef char tPath_name[256];

typedef unsigned char tU8;
typedef signed char tS8;
typedef unsigned short tU16;
typedef signed short tS16;
typedef unsigned int tU32;
typedef signed int tS32;

typedef int tTWTVFS;

typedef struct {
    int twtfile;
    void* stdio;
} tTWTFILE;

typedef struct {
    tU32 fileSize;
    char filename[48];
    tU8* data;
} tTwatFileHeader;

typedef struct {
    tU32 twatFileSize;
    tU32 nbFiles;
    tTwatFileHeader fileHeaders[];
} tTwatHeader;

typedef struct {
    tTwatHeader* header;
    tPath_name path;
    tU8* data;
} tTwatVfsMountPoint;

typedef struct {
    int used;
    int unknown[3];
} tTwatVfsFile;

enum {
    // FIXME: add all c2 memory tags
    kMem_packed_file = 0xd0,
};

enum {
    // FIXME: add all c2 FatalError types
    kFatalError_InvalidScreenDepthSetting = 0x00,
    kFatalError_RootActorAllocError = 0x03,
};

enum {
    kMem_misc = 0xfd,
};
#endif // REC2_TYPES_H
