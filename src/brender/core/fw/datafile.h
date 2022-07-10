#ifndef REC2_DATAFILE_H
#define REC2_DATAFILE_H

#include "c2_hooks.h"

#include "brender/br_types.h"

// The names of these enum values are a wild guess
enum {
    DF_TYPE_BR_INT_8 = 0,
    DF_TYPE_BR_UINT_8 = 1,
    DF_TYPE_BR_INT_16 = 2,
    DF_TYPE_BR_UINT_16 = 3,
    DF_TYPE_BR_INT_32 = 4,
    DF_TYPE_BR_UINT_32 = 5,
    DF_TYPE_BR_FIXED = 6,
    DF_TYPE_BR_ANGLE = 7,
    DF_TYPE_FLOAT = 8,
    DF_TYPE_DOUBLE = 9,
    DF_TYPE_BR_SCALAR = 10,
    DF_TYPE_BR_FRACTION = 11,
    DF_TYPE_BR_UFRACTION = 12,
    DF_TYPE_ENUM_8 = 13,
    DF_TYPE_ENUM_16 = 14,
    DF_TYPE_ENUM_32 = 15,
    DF_TYPE_STRUCT = 16,
    DF_TYPE_ASCIZ = 17,
    DF_TYPE_BR_COLOUR = 18,
    DF_TYPE_BR_VECTOR2 = 19,
    DF_TYPE_BR_VECTOR3 = 20,
    DF_TYPE_BR_VECTOR4 = 21,
    DF_TYPE_BR_FRACTION_X = 22,
    DF_TYPE_BR_UFRACTION_X = 23,
    DF_TYPE_BR_FRACTION_F = 24,
    DF_TYPE_BR_UFRACTION_F = 25,
    DF_TYPE_BR_VECTOR2_X = 26,
    DF_TYPE_BR_VECTOR3_X = 27,
    DF_TYPE_BR_VECTOR4_X = 28,
    DF_TYPE_BR_VECTOR2_F = 29,
    DF_TYPE_BR_VECTOR3_F = 30,
    DF_TYPE_BR_VECTOR4_F = 31,
    DF_TYPE_COUNT,
};

enum {
    DF_CHUNKID_END = 0,
    DF_CHUNKID_IMAGE_PLANE = 1,
    DF_CHUNKID_RLE_IMAGE_PLANE = 2,
    DF_CHUNKID_PIXELMAP = 3,
    DF_CHUNKID_MATERIAL_OLD = 4,
    DF_CHUNKID_ADD_MATERIAL = 5,
    DF_CHUNKID_OLD_ACTOR = 6,
    DF_CHUNKID_OLD_ADD_SIBLING = 7,
    DF_CHUNKID_OLD_ADD_CHILD = 8,
    DF_CHUNKID_OLD_MATERIAL_INDEX = 9,
    DF_CHUNKID_OLD_VERTICES = 10,
    DF_CHUNKID_OLD_VERTICES_UV = 11,
    DF_CHUNKID_OLD_FACES = 12,
    DF_CHUNKID_OLD_MODEL = 13,
    DF_CHUNKID_ADD_MODEL = 14,
    DF_CHUNKID_ANIM = 15,
    DF_CHUNKID_ANIM_TRANSFORM = 16,
    DF_CHUNKID_ANIM_RATE = 17,
    DF_CHUNKID_FILE_INFO = 18,
    DF_CHUNKID_OLD_LIGHT = 19,
    DF_CHUNKID_OLD_CAMERA = 20,
    DF_CHUNKID_PIVOT = 21,
    DF_CHUNKID_MATERIAL_INDEX = 22,
    DF_CHUNKID_VERTICES = 23,
    DF_CHUNKID_VERTEX_UV = 24,
    DF_CHUNKID_OLD_FACES_1 = 25,
    DF_CHUNKID_FACE_MATERIAL = 26,
    DF_CHUNKID_OLD_MODEL_1 = 27,
    DF_CHUNKID_COLOUR_MAP_REF = 28,
    DF_CHUNKID_OPACITY_MAP_REF = 29,
    DF_CHUNKID_INDEX_BLEND_REF = 30,
    DF_CHUNKID_INDEX_SHADE_REF = 31,
    DF_CHUNKID_SCREENDOOR_REF = 32,
    DF_CHUNKID_PIXELS = 33,
    DF_CHUNKID_ADD_MAP = 34,
    DF_CHUNKID_ACTOR = 35,
    DF_CHUNKID_ACTOR_MODEL = 36,
    DF_CHUNKID_ACTOR_TRANSFORM = 37,
    DF_CHUNKID_ACTOR_MATERIAL = 38,
    DF_CHUNKID_ACTOR_LIGHT = 39,
    DF_CHUNKID_ACTOR_CAMERA = 40,
    DF_CHUNKID_ACTOR_BOUNDS = 41,
    DF_CHUNKID_ACTOR_ADD_CHILD = 42,
    DF_CHUNKID_TRANSFORM_MATRIX34 = 43,
    DF_CHUNKID_TRANSFORM_MATRIX34_LP = 44,
    DF_CHUNKID_TRANSFORM_QUAT = 45,
    DF_CHUNKID_TRANSFORM_EULER = 46,
    DF_CHUNKID_TRANSFORM_LOOK_UP = 47,
    DF_CHUNKID_TRANSFORM_TRANSLATION = 48,
    DF_CHUNKID_TRANSFORM_IDENTITY = 49,
    DF_CHUNKID_BOUNDS = 50,
    DF_CHUNKID_LIGHT = 51,
    DF_CHUNKID_CAMERA = 52,
    DF_CHUNKID_FACES = 53,
    DF_CHUNKID_MODEL = 54,
    DF_CHUNKID_ACTOR_CLIP_PLANE = 55,
    DF_CHUNKID_PLANE = 56,
    DF_CHUNKID_SATURN_FACES = 57,
    DF_CHUNKID_SATURN_MODEL = 58,
    DF_CHUNKID_INDEX_FOG_REF = 59,
    DF_CHUNKID_MATERIAL_OLD_2 = 60,
};

void C2_HOOK_STDCALL DfPush(int type, void* value, int count);

void* C2_HOOK_STDCALL DfPop(int type, int* countp);

void* C2_HOOK_STDCALL DfTop(int type, int* countp);

int C2_HOOK_STDCALL DfTopType(void);

int C2_HOOK_STDCALL TextReadLine(br_datafile* df, char** ident, char** data);

br_uint_16 C2_HOOK_STDCALL scalarTypeConvert(br_datafile* df, br_uint_16 t);

br_uint_32 C2_HOOK_STDCALL DfStructWriteBinary(br_datafile* df, br_file_struct* str, void* base);

br_uint_32 C2_HOOK_STDCALL DfStructReadBinary(br_datafile* df, br_file_struct* str, void* base);

int C2_HOOK_STDCALL DfStructSizeBinary(br_datafile* df, br_file_struct* str, void* base);

int C2_HOOK_STDCALL EnumFromString(br_file_enum* e, char* str);

char* C2_HOOK_STDCALL EnumToString(br_file_enum* e, int num);

br_uint_32 C2_HOOK_STDCALL DfStructWriteText(br_datafile* df, br_file_struct* str, void* base);

br_uint_32 C2_HOOK_STDCALL StructWriteTextSub(br_datafile* df, br_file_struct* str, void* base, int indent);

br_uint_32 C2_HOOK_STDCALL DfStructReadText(br_datafile* df, br_file_struct* str, void* base);

br_uint_32 C2_HOOK_STDCALL StructReadTextSub(br_datafile* df, br_file_struct* str, void* base);

int C2_HOOK_STDCALL DfStructSizeText(br_datafile* df, br_file_struct* str, void* base);

br_uint_32 C2_HOOK_STDCALL DfStructWriteArray(br_datafile* df, br_file_struct* str, void* base, int n);

br_uint_32 C2_HOOK_STDCALL DfStructReadArray(br_datafile* df, br_file_struct* str, void* base, int n);

int C2_HOOK_STDCALL DfChunkWriteText(br_datafile* df, br_uint_32 id, br_uint_32 length);

int C2_HOOK_STDCALL DfChunkReadText(br_datafile* df, br_uint_32* plength);

int C2_HOOK_STDCALL DfChunkWriteBinary(br_datafile* df, br_uint_32 id, br_uint_32 length);

int C2_HOOK_STDCALL DfChunkReadBinary(br_datafile* df, br_uint_32* plength);

void C2_HOOK_STDCALL DfCountWriteText(br_datafile* df, br_uint_32 count);

br_uint_32 C2_HOOK_STDCALL DfCountReadText(br_datafile* df);

void C2_HOOK_STDCALL DfCountWriteBinary(br_datafile* df, br_uint_32 count);

br_uint_32 C2_HOOK_STDCALL DfCountReadBinary(br_datafile* df);

int C2_HOOK_STDCALL DfCountSizeText(br_datafile* df);

int C2_HOOK_STDCALL DfCountSizeBinary(br_datafile* df);

br_uint_8* C2_HOOK_STDCALL BlockWriteSetup(void* base, int block_size, int block_stride, int block_count, int size);

int C2_HOOK_STDCALL DfBlockWriteText(br_datafile* df, void* base, int block_size, int block_stride, int block_count, int size);

void* C2_HOOK_STDCALL DfBlockReadText(br_datafile* df, void* base, int* count, int size, int mtype);

int C2_HOOK_STDCALL DfBlockWriteBinary(br_datafile* df, void* base, int block_size, int block_stride, int block_count, int size);

void* C2_HOOK_STDCALL DfBlockReadBinary(br_datafile* df, void* base, int* count, int size, int mtype);

int C2_HOOK_STDCALL DfBlockSizeText(br_datafile* df, void* base, int block_size, int block_stride, int block_count, int size);

int C2_HOOK_STDCALL DfBlockSizeBinary(br_datafile* df, void* base, int block_size, int block_stride, int block_count, int size);

char* C2_HOOK_STDCALL DfNameReadText(br_datafile* df, char* name);

int C2_HOOK_STDCALL DfNameWriteText(br_datafile* df, char* name);

int C2_HOOK_STDCALL DfNameSizeText(br_datafile* df, char* name);

char* C2_HOOK_STDCALL DfNameReadBinary(br_datafile* df, char* name);

int C2_HOOK_STDCALL DfNameWriteBinary(br_datafile* df, char* name);

int C2_HOOK_STDCALL DfNameSizeBinary(br_datafile* df, char* name);

int C2_HOOK_STDCALL DfSkipText(br_datafile* df, br_uint_32 length);

int C2_HOOK_STDCALL DfSkipBinary(br_datafile* df, br_uint_32 length);

int C2_HOOK_STDCALL DfChunksInterpret(br_datafile* df, br_chunks_table* table);

void C2_HOOK_STDCALL BrNullOther(void);

int C2_HOOK_STDCALL DfFileIdentify(br_uint_8* magics, br_size_t n_magics);

br_datafile* C2_HOOK_STDCALL DfOpen(char* name, int write, br_token scalar_type);

void C2_HOOK_STDCALL DfClose(br_datafile* df);

int C2_HOOK_STDCALL BrWriteModeSet(int mode);

#endif // REC2_DATAFILE_H
