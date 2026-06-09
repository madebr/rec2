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

// GLOBAL: CARMA2_HW 0x0066d7c8
br_file_enum_member pixelmap_type_FM[] = {
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
};

// GLOBAL: CARMA2_HW 0x0066d840
br_file_enum pixelmap_type_F ={ BR_ASIZE(pixelmap_type_FM), pixelmap_type_FM };

// GLOBAL: CARMA2_HW 0x0066d848
br_file_struct_member br_old_pixelmap_FM[] = {
    { DF_TYPE_ENUM_8, offsetof(br_pixelmap, type), "type", &pixelmap_type_F },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, row_bytes), "row_bytes", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, width), "width", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, height), "height", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, origin_x), "origin_x", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, origin_y), "origin_y", NULL },
    { DF_TYPE_ASCIZ, offsetof(br_pixelmap, identifier), "identifier", NULL },
};

// GLOBAL: CARMA2_HW 0x0066d8b8
br_file_struct_member br_pixelmap_FM[] = {
    { DF_TYPE_ENUM_8, offsetof(br_pixelmap, type), "type", &pixelmap_type_F },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, row_bytes), "row_bytes", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, width), "width", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, height), "height", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, origin_x), "origin_x", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, origin_y), "origin_y", NULL },
    { DF_TYPE_BR_UINT_16, offsetof(br_pixelmap, mip_offset), "mip_offset", NULL },
    { DF_TYPE_ASCIZ, offsetof(br_pixelmap, identifier), "identifier", NULL },
};

// GLOBAL: CARMA2_HW 0x0066d938
br_file_struct br_pixelmap_F = { "br_pixelmap", BR_ASIZE(br_pixelmap_FM), br_pixelmap_FM, sizeof(br_pixelmap) };

// GLOBAL: CARMA2_HW 0x0066d948
br_file_struct br_old_pixelmap_F = { "br_old_pixelmap", BR_ASIZE(br_old_pixelmap_FM), br_old_pixelmap_FM, sizeof(br_pixelmap) };

// GLOBAL: CARMA2_HW 0x0066d958
br_chunks_table_entry PixelmapLoadEntries[5] = {
    { CHUNKID_END, 0u, FopRead_END },
    { CHUNKID_PIXELMAP, 0u, FopRead_PIXELMAP },
    { CHUNKID_PIXELMAP_PIXELS, 0u, FopRead_PIXELS },
    { CHUNKID_PIXELMAP_ADD_MAP, 0u, FopRead_ADD_MAP },
    { CHUNKID_OLD_PIXELMAP, 0u, FopRead_OLD_PIXELMAP },
};
// GLOBAL: CARMA2_HW 0x0066d998
br_chunks_table PixelmapLoadTable = { BR_ASIZE(PixelmapLoadEntries), PixelmapLoadEntries };

int C2_HOOK_STDCALL FopWrite_PIXELMAP(br_datafile* df, br_pixelmap* pixelmap) {
    br_pixelmap pmap;

    memcpy(&pmap, pixelmap, sizeof(br_pixelmap));
    pmap.row_bytes = (pmTypeInfo[pmap.type].bits >> 3) * pmap.width;
    df->prims->chunk_write(df, CHUNKID_PIXELMAP, df->prims->struct_size(df, &br_pixelmap_F, &pmap));
    df->prims->struct_write(df, &br_pixelmap_F, &pmap);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00538da0
int C2_HOOK_STDCALL FopRead_OLD_PIXELMAP(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    br_pixelmap* pp;

    pp = (br_pixelmap*)DevicePixelmapMemAllocate(BR_PMAF_NO_PIXELS | BR_PMAF_INVERTED, 0, 0, 0, 2);
    df->res = pp;
    df->prims->struct_read(df, &br_old_pixelmap_F, pp);
    df->res = NULL;
    pp->row_bytes = (pmTypeInfo[pp->type].bits >> 3) * pp->width;
    DfPush(DF_PIXELMAP, pp, 1);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00538e00
int C2_HOOK_STDCALL FopRead_PIXELMAP(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    br_pixelmap* pp;

    pp = (br_pixelmap*)DevicePixelmapMemAllocate(BR_PMAF_NO_PIXELS | BR_PMAF_INVERTED, 0, 0, 0, 2);
    df->res = pp;
    df->prims->struct_read(df, &br_pixelmap_F, pp);
    df->res = NULL;
    pp->row_bytes = (pmTypeInfo[pp->type].bits >> 3) * pp->width;
    DfPush(DF_PIXELMAP, pp, 1);
    return 0;
}

static int FopWrite_PIXELS(br_datafile* df, br_pixelmap* pixelmap) {
    int size = pmTypeInfo[pixelmap->type].file_size;
    int bytes = pmTypeInfo[pixelmap->type].bits >> 3;
    int block_count = (pixelmap->width * bytes) / size;
    char* pixels;

    pixels = (char*)pixelmap->pixels + pixelmap->base_y * pixelmap->row_bytes + pixelmap->base_x * pixelmap->width;
    if (pixelmap->mip_offset != 0) {
        int mipSize = 0;
        int currentMipSize;

        for (currentMipSize = pixelmap->width; currentMipSize != 0; currentMipSize >>= 1) {
            mipSize += currentMipSize * currentMipSize * bytes;
        }
        df->prims->chunk_write(df, CHUNKID_PIXELMAP_PIXELS, df->prims->block_size(df, pixels, mipSize / size, mipSize, 1, size));
        df->prims->block_write(df, pixels, mipSize, mipSize, 1, size);
    } else {
        df->prims->chunk_write(df, CHUNKID_PIXELMAP_PIXELS, df->prims->block_size(df, pixels, block_count, pixelmap->row_bytes, pixelmap->height, size));
        df->prims->block_write(df, pixels, block_count, pixelmap->row_bytes, pixelmap->height, size);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00538e60
int C2_HOOK_STDCALL FopRead_PIXELS(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    int icount;
    br_pixelmap* pp;
    int size;

    pp = DfTop(DF_PIXELMAP, NULL);
    size = pmTypeInfo[pp->type].file_size;
    df->res = pp;
    pp->pixels = df->prims->block_read(df, NULL, &icount, size, BR_MEMORY_PIXELS);
    pp->flags |= BR_PMF_LINEAR;
    df->res = NULL;
    return 0;
}

int C2_HOOK_STDCALL FopWrite_ADD_MAP(br_datafile* df) {

    df->prims->chunk_write(df, CHUNKID_PIXELMAP_ADD_MAP, 0);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00538ec0
int C2_HOOK_STDCALL FopRead_ADD_MAP(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    br_pixelmap* pp;
    br_pixelmap* map;

    map = DfPop(DF_PIXELMAP, 0);
    pp = DfTop(DF_PIXELMAP, 0);
    pp->map = map;
    BrResAdd(pp, map);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00538ef0
br_uint_32 C2_HOOK_CDECL BrPixelmapLoadMany(char* filename, br_pixelmap** pixelmaps, br_uint_16 num) {
    br_datafile* df;
    int count;
    int r;

    df = DfOpen(filename, 0, BRT_FIXED);
    if (df == NULL) {
        return 0;
    }
    for (count = 0; count < num; ) {
        r = DfChunksInterpret(df, &PixelmapLoadTable);
        if (DfTopType() == DF_PIXELMAP) {
            pixelmaps[count++] = DfPop(DF_PIXELMAP, 0);
        }
        if (r == 0) {
            break;
        }
    }
    DfClose(df);
    return count;
}

// FUNCTION: CARMA2_HW 0x00539170
int C2_HOOK_STDCALL WritePixelmap(br_pixelmap* pp, br_datafile* df) {

    BrPixelmapDirectLock(pp, 1);
#ifdef BR_FIX_BUGS
    if (pp->pixels == NULL) {
        _BrAssert("pp->pixels", "pmfile.c", 291);
    }
#endif
    FopWrite_PIXELMAP(df, pp);
    if (pp->map != NULL) {
        WritePixelmap(pp->map, df);
        FopWrite_ADD_MAP(df);
    }
    FopWrite_PIXELS(df, pp);
    BrPixelmapDirectUnlock(pp);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00538f60
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

// FUNCTION: CARMA2_HW 0x00539320
br_pixelmap* C2_HOOK_CDECL BrPixelmapLoad(char* filename) {
    br_pixelmap* ptr;

    if (BrPixelmapLoadMany(filename, &ptr, 1) == 1) {
        return ptr;
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x005393a0
br_uint_32 C2_HOOK_CDECL BrPixelmapSave(char* filename, br_pixelmap* ptr) {

    return BrPixelmapSaveMany(filename, &ptr, 1);
}
