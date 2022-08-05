#include "pmfile.h"

#include "pmmem.h"
#include "pmdsptch.h"

#include "core/fw/datafile.h"
#include "core/fw/diag.h"
#include "core/fw/genfile.h"
#include "core/fw/resource.h"

#include "core/v1db/chunkids.h"
#include "core/v1db/stackids.h"

#include "c2_string.h"

#include <stddef.h>

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_file_enum_member, pixelmap_type_FM, 15, 0x0066d7c8, {
    { BR_PMT_INDEX_1, "BR_PMT_INDEX_1", },
    { BR_PMT_INDEX_2, "BR_PMT_INDEX_2", },
    { BR_PMT_INDEX_4, "BR_PMT_INDEX_4", },
    { BR_PMT_INDEX_8, "BR_PMT_INDEX_8", },
    { BR_PMT_RGB_555, "BR_PMT_RGB_555", },
    { BR_PMT_RGB_565, "BR_PMT_RGB_565", },
    { BR_PMT_RGB_888, "BR_PMT_RGB_888", },
    { BR_PMT_RGBX_888, "BR_PMT_RGBX_888", },
    { BR_PMT_RGBA_8888, "BR_PMT_RGBA_8888", },
    { BR_PMT_YUYV_8888, "BR_PMT_YUYV_8888", },
    { BR_PMT_YUV_888, "BR_PMT_YUV_888", },
    { BR_PMT_DEPTH_16, "BR_PMT_DEPTH_16", },
    { BR_PMT_DEPTH_32, "BR_PMT_DEPTH_32", },
    { BR_PMT_ALPHA_8, "BR_PMT_ALPHA_8", },
    { BR_PMT_INDEXA_88, "BR_PMT_INDEXA_88", },
});
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_file_enum, pixelmap_type_F, 0x0066d840, { BR_ASIZE(C2V(pixelmap_type_FM)), pixelmap_type_FM });

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_file_struct_member, br_old_pixelmap_FM, 7, 0x0066d848, {
    { DF_TYPE_ENUM_8, offsetof(br_pixelmap, type), "type", &C2V(pixelmap_type_F) },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, row_bytes), "row_bytes", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, width), "width", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, height), "height", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, origin_x), "origin_x", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, origin_y), "origin_y", NULL },
    { DF_TYPE_ASCIZ, offsetof(br_pixelmap, identifier), "identifier", NULL },
});

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_file_struct_member, br_pixelmap_FM, 8, 0x0066d8b8, {
    { DF_TYPE_ENUM_8, offsetof(br_pixelmap, type), "type", &C2V(pixelmap_type_F) },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, row_bytes), "row_bytes", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, width), "width", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, height), "height", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, origin_x), "origin_x", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, origin_y), "origin_y", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, mip_offset), "mip_offset", NULL },
    { DF_TYPE_ASCIZ, offsetof(br_pixelmap, identifier), "identifier", NULL },
});

C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_file_struct, br_pixelmap_F, 0x0066d938, { "br_pixelmap", BR_ASIZE(br_pixelmap_FM), br_pixelmap_FM, sizeof(br_pixelmap) });
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_file_struct, br_old_pixelmap_F, 0x0066d948, { "br_old_pixelmap", BR_ASIZE(C2V(br_old_pixelmap_FM)), C2V(br_old_pixelmap_FM), sizeof(br_pixelmap) });

br_chunks_table_entry PixelmapLoadEntries[5] = {
    { CHUNKID_END, 0u, FopRead_END },
    { CHUNKID_PIXELMAP, 0u, FopRead_PIXELMAP },
    { CHUNKID_PIXELMAP_PIXELS, 0u, FopRead_PIXELS },
    { CHUNKID_PIXELMAP_ADD_MAP, 0u, FopRead_ADD_MAP },
    { CHUNKID_OLD_PIXELMAP, 0u, FopRead_OLD_PIXELMAP },
};
br_chunks_table PixelmapLoadTable = { BR_ASIZE(PixelmapLoadEntries), PixelmapLoadEntries };

int C2_HOOK_STDCALL FopWrite_PIXELMAP(br_datafile* df, br_pixelmap* pixelmap) {
    br_pixelmap pmap;

    c2_memcpy(&pmap, pixelmap, sizeof(br_pixelmap));
    pmap.row_bytes = (C2V(pmTypeInfo)[pmap.type].bits >> 3) * pmap.width;
    df->prims->chunk_write(df, CHUNKID_PIXELMAP, df->prims->struct_size(df, &C2V(br_pixelmap_F), &pmap));
    df->prims->struct_write(df, &C2V(br_pixelmap_F), &pmap);
    return 0;
}

int C2_HOOK_STDCALL FopRead_OLD_PIXELMAP(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    br_pixelmap* pp;

    pp = (br_pixelmap*)DevicePixelmapMemAllocate(BR_PMAF_NO_PIXELS | BR_PMAF_INVERTED, 0, 0, 0, 2);
    df->res = pp;
    df->prims->struct_read(df, &C2V(br_old_pixelmap_F), pp);
    df->res = NULL;
    pp->row_bytes = (C2V(pmTypeInfo)[pp->type].bits >> 3) * pp->width;
    DfPush(DF_PIXELMAP, pp, 1);
    return 0;
}
C2_HOOK_FUNCTION(0x00538da0, FopRead_OLD_PIXELMAP)

int C2_HOOK_STDCALL FopRead_PIXELMAP(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    br_pixelmap* pp;

    pp = (br_pixelmap*)DevicePixelmapMemAllocate(BR_PMAF_NO_PIXELS | BR_PMAF_INVERTED, 0, 0, 0, 2);
    df->res = pp;
    df->prims->struct_read(df, &C2V(br_pixelmap_F), pp);
    df->res = NULL;
    pp->row_bytes = (C2V(pmTypeInfo)[pp->type].bits >> 3) * pp->width;
    DfPush(DF_PIXELMAP, pp, 1);
    return 0;
}
C2_HOOK_FUNCTION(0x00538e00, FopRead_PIXELMAP)

int C2_HOOK_STDCALL FopWrite_PIXELS(br_datafile* df, br_pixelmap* pixelmap) {
    int size;
    int bytes;
    int block_count;
    char* pixels;

    pixels = (char*)pixelmap->pixels + pixelmap->base_x * pixelmap->width + pixelmap->base_y;
    if (pixelmap->mip_offset == 0) {
        bytes = df->prims->block_size(df, pixels, (C2V(pmTypeInfo)[pixelmap->type].bits >> 3) * pixelmap->width / C2V(pmTypeInfo)[pixelmap->type].file_size, pixelmap->row_bytes, pixelmap->height, C2V(pmTypeInfo)[pixelmap->type].file_size);
        df->prims->chunk_write(df, CHUNKID_PIXELMAP_PIXELS, bytes);
        bytes = (C2V(pmTypeInfo)[pixelmap->type].bits >> 3) * pixelmap->width / C2V(pmTypeInfo)[pixelmap->type].file_size;
        size = pixelmap->row_bytes / C2V(pmTypeInfo)[pixelmap->type].file_size;
        block_count = pixelmap->height;
    } else {
        size = 0;
        for (bytes = pixelmap->width; bytes != 0; bytes = bytes >> 1) {
            size += bytes * bytes * (C2V(pmTypeInfo)[pixelmap->type].bits >> 3);
        }
        df->prims->chunk_write(df, CHUNKID_PIXELMAP_PIXELS, df-> prims->block_size(df, pixels, size / C2V(pmTypeInfo)[pixelmap->type].file_size, size, 1, C2V(pmTypeInfo)[pixelmap->type].file_size));
        bytes = size;
        block_count = 1;
    }
    df->prims->block_write(df, pixels, bytes, size, block_count, C2V(pmTypeInfo)[pixelmap->type].file_size);
    return 0;
}

int C2_HOOK_STDCALL FopRead_PIXELS(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    int icount = 0;
    br_pixelmap* pp;
    int size;

    pp = DfTop(DF_PIXELMAP, NULL);
    size = C2V(pmTypeInfo)[pp->type].file_size;
    df->res = pp;
    pp->pixels = df->prims->block_read(df, NULL, &icount, size, BR_MEMORY_PIXELS);
    pp->flags |= BR_PMF_LINEAR;
    df->res = NULL;
    return 0;
}
C2_HOOK_FUNCTION(0x00538e60, FopRead_PIXELS)

int C2_HOOK_STDCALL FopWrite_ADD_MAP(br_datafile* df) {

    df->prims->chunk_write(df, CHUNKID_PIXELMAP_ADD_MAP, 0);
    return 0;
}

int C2_HOOK_STDCALL FopRead_ADD_MAP(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    br_pixelmap* pp;
    br_pixelmap* map;

    map = DfPop(DF_PIXELMAP, 0);
    pp = DfTop(DF_PIXELMAP, 0);
    pp->map = map;
    BrResAdd(pp, map);
    return 0;
}
C2_HOOK_FUNCTION(0x00538ec0, FopRead_ADD_MAP)

br_uint_32 C2_HOOK_CDECL BrPixelmapLoadMany(char* filename, br_pixelmap** pixelmaps, br_uint_16 num) {
    br_datafile* df;
    int count;
    int r;

    df = DfOpen(filename, 0, BRT_FIXED);
    if (df == NULL) {
        return 0;
    }
    count = 0;
    do {
        if (count >= num) {
            break;
        }
        r = DfChunksInterpret(df, &PixelmapLoadTable);
        if (DfTopType() == DF_PIXELMAP) {
            pixelmaps[count] = DfPop(DF_PIXELMAP, 0);
            count++;
        }
    } while (r != 0);
    DfClose(df);
    return count;
}
C2_HOOK_FUNCTION(0x00538ef0, BrPixelmapLoadMany)

int C2_HOOK_STDCALL WritePixelmap(br_pixelmap* pp, br_datafile* df) {

    BrPixelmapDirectLock(pp, 1);
    if (pp->pixels == NULL) {
        _BrAssert("pp->pixels", "pmfile.c", 291);
    }
    FopWrite_PIXELMAP(df, pp);
    if (pp->map != NULL) {
        WritePixelmap(pp->map, df);
        FopWrite_ADD_MAP(df);
    }
    FopWrite_PIXELS(df, pp);
    BrPixelmapDirectUnlock(pp);
    return 0;
}
C2_HOOK_FUNCTION(0x00539170, WritePixelmap)

br_uint_32 C2_HOOK_CDECL BrPixelmapSaveMany(char* filename, br_pixelmap** pixelmaps, br_uint_16 num) {
    br_datafile* df;
    int i;

    df = DfOpen(filename, 1, BRT_FIXED);
    if (df == NULL) {
        return 0;
    }
    FopWrite_FILE_INFO(df, 2);
    for (i = 0; i < num; i++) {
        WritePixelmap(pixelmaps[i], df);
        FopWrite_END(df);
    }
    DfClose(df);
    return num;
}
C2_HOOK_FUNCTION(0x00538f60, BrPixelmapSaveMany)

br_pixelmap* C2_HOOK_CDECL BrPixelmapLoad(char* filename) {
    br_pixelmap* ptr;

    if (BrPixelmapLoadMany(filename, &ptr, 1) == 1) {
        return ptr;
    }
    return NULL;
}
C2_HOOK_FUNCTION(0x00539320, BrPixelmapLoad)

br_uint_32 C2_HOOK_CDECL BrPixelmapSave(char* filename, br_pixelmap* ptr) {

    return BrPixelmapSaveMany(filename, &ptr, 1);
}
C2_HOOK_FUNCTION(0x005393a0, BrPixelmapSave)
