#include "genfile.h"
#include "datafile.h"

#include "core/v1db/chunkids.h"

#include "c2_stdlib.h"

#include <stddef.h>

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_file_enum_member, file_type_FM, 10, 0x006670f0, {
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
});
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_file_enum, file_type_F, 0x00667140, { BR_ASIZE(file_type_FM), file_type_FM });

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_file_struct_member, file_info_FM, 2, 0x00667148, {
    { DF_TYPE_ENUM_32,  offsetof(file_info, type), "type", &file_type_F },
    { DF_TYPE_BR_UINT_32, offsetof(file_info, version), "version", NULL },
});
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_file_struct, file_info_F, 0x00667168, { "file_info", BR_ASIZE(file_info_FM), file_info_FM, sizeof(file_info) });

int C2_HOOK_STDCALL FopRead_END(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count) {
    return 1;
}
C2_HOOK_FUNCTION(0x00529f70, FopRead_END)

int C2_HOOK_STDCALL FopWrite_END(br_datafile* df) {

    df->prims->chunk_write(df, CHUNKID_END, 0);
    return 0;
}
C2_HOOK_FUNCTION(0x00529f80, FopWrite_END)

int C2_HOOK_STDCALL FopWrite_FILE_INFO(br_datafile* df, br_uint_32 type) {
    file_info fi;

    fi.type = type;
    fi.version = 2;
    df->prims->chunk_write(df, CHUNKID_FILE_INFO, df->prims->struct_size(df, &C2V(file_info_F), &fi));
    df->prims->struct_write(df, &C2V(file_info_F), &fi);
    return 0;
}
C2_HOOK_FUNCTION(0x00529fa0, FopWrite_FILE_INFO)
