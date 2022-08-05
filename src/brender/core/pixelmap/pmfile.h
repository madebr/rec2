#ifndef REC2_PMFILE_H
#define REC2_PMFILE_H

#include "brender/br_types.h"

#include "c2_hooks.h"

int C2_HOOK_STDCALL FopWrite_PIXELMAP(br_datafile* df, br_pixelmap* pixelmap);

int C2_HOOK_STDCALL FopRead_OLD_PIXELMAP(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopRead_PIXELMAP(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopWrite_PIXELS(br_datafile* df, br_pixelmap* pixelmap);

int C2_HOOK_STDCALL FopRead_PIXELS(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopWrite_ADD_MAP(br_datafile* df);

int C2_HOOK_STDCALL FopRead_ADD_MAP(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

br_uint_32 C2_HOOK_CDECL BrPixelmapLoadMany(char* filename, br_pixelmap** pixelmaps, br_uint_16 num);

int C2_HOOK_STDCALL WritePixelmap(br_pixelmap* pp, br_datafile* df);

br_uint_32 C2_HOOK_CDECL BrPixelmapSaveMany(char* filename, br_pixelmap** pixelmaps, br_uint_16 num);

br_pixelmap* C2_HOOK_CDECL BrPixelmapLoad(char* filename);

br_uint_32 C2_HOOK_CDECL BrPixelmapSave(char* filename, br_pixelmap* ptr);

#endif // REC2_PMFILE_H
