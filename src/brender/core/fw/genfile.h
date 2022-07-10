#ifndef REC2_GENFILE_H
#define REC2_GENFILE_H

#include "c2_hooks.h"

#include "brender/br_types.h"

#define FILE_TYPE_NONE          0x0
#define FILE_TYPE_ACTORS        0x1
#define FILE_TYPE_PIXELMAP      0x2
#define FILE_TYPE_LIGHT         0x3
#define FILE_TYPE_CAMERA        0x4
#define FILE_TYPE_MATERIAL_OLD  0x5
#define FILE_TYPE_MODEL         0xface
#define FILE_TYPE_ANIMATION     0xa11
#define FILE_TYPE_TREE          0x5eed
#define FILE_TYPE_MATERIAL      0x6

int C2_HOOK_STDCALL FopRead_END(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopWrite_END(br_datafile* df);

int C2_HOOK_STDCALL FopWrite_FILE_INFO(br_datafile* df, br_uint_32 type);

#endif // REC2_GENFILE_H
