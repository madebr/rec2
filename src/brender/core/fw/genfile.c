#include "genfile.h"
#include "datafile.h"

#include "core/v1db/chunkids.h"

#include "c2_stdlib.h"

#include <stddef.h>

// GLOBAL: CARMA2_HW 0x006670f0
br_file_enum_member file_type_FM[] = {
    { FILE_TYPE_NONE, "FILE_TYPE_NONE" },
    { FILE_TYPE_ACTORS, "FILE_TYPE_ACTORS" },
    { FILE_TYPE_PIXELMAP, "FILE_TYPE_PIXELMAP" },
    { FILE_TYPE_LIGHT, "FILE_TYPE_LIGHT" },
    { FILE_TYPE_CAMERA, "FILE_TYPE_CAMERA" },
    { FILE_TYPE_MATERIAL_OLD, "FILE_TYPE_MATERIAL_OLD" },
    { FILE_TYPE_MODEL, "FILE_TYPE_MODEL" },
    { FILE_TYPE_ANIMATION, "FILE_TYPE_ANIMATION" },
    { FILE_TYPE_TREE, "FILE_TYPE_TREE" },
    { FILE_TYPE_MATERIAL, "FILE_TYPE_MATERIAL" },
};

// GLOBAL: CARMA2_HW 0x00667140
br_file_enum file_type_F = { BR_ASIZE(file_type_FM), file_type_FM };

// GLOBAL: CARMA2_HW 0x00667148
br_file_struct_member file_info_FM[] = {
    { DF_TYPE_ENUM_32,  offsetof(file_info, type), "type", &file_type_F },
    { DF_TYPE_BR_UINT_32, offsetof(file_info, version), "version", NULL },
};

// GLOBAL: CARMA2_HW 0x00667168
br_file_struct file_info_F = { "file_info", BR_ASIZE(file_info_FM), file_info_FM, sizeof(file_info) };

// FUNCTION: CARMA2_HW 0x00529f70
int C2_HOOK_STDCALL FopRead_END(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    return 1;
}

// FUNCTION: CARMA2_HW 0x00529f80
int C2_HOOK_STDCALL FopWrite_END(br_datafile* df) {

    df->prims->chunk_write(df, CHUNKID_END, 0);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00529fa0
int C2_HOOK_STDCALL FopWrite_FILE_INFO(br_datafile* df, br_uint_32 type) {
    file_info fi;

    fi.type = type;
    fi.version = 2;
    df->prims->chunk_write(df, CHUNKID_FILE_INFO, df->prims->struct_size(df, &file_info_F, &fi));
    df->prims->struct_write(df, &file_info_F, &fi);
    return 0;
}
