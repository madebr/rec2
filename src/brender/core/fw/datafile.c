#include "datafile.h"
#include "bswap.h"
#include "diag.h"
#include "file.h"
#include "fwsetup.h"
#include "resource.h"
#include "scratch.h"

#include "core/math/fixed.h"
#include "core/std/brstdlib.h"

#include "c2_stdlib.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_file_primitives, _BrFilePrimsNull, 0x006673c8, {
    "NULL",
    (int (*)(br_datafile*, br_uint_32)) & BrNullOther,
    (int (*)(br_datafile*, br_uint_32, br_uint_32)) & BrNullOther,
    (int (*)(br_datafile*, br_uint_32*)) & BrNullOther,
    (void (*)(br_datafile*, br_uint_32)) & BrNullOther,
    (br_uint_32(*)(br_datafile*)) & BrNullOther,
    (int (*)(br_datafile*)) & BrNullOther,
    (br_uint_32(*)(br_datafile*, br_file_struct*, void*)) & BrNullOther,
    (br_uint_32(*)(br_datafile*, br_file_struct*, void*)) & BrNullOther,
    (int (*)(br_datafile*, br_file_struct*, void*)) & BrNullOther,
    (int (*)(br_datafile*, void*, int, int, int, int)) & BrNullOther,
    (void* (*)(br_datafile*, void*, int*, int, int)) & BrNullOther,
    (int (*)(br_datafile*, void*, int, int, int, int)) & BrNullOther,
    (int (*)(br_datafile*, char*)) & BrNullOther,
    (char* (*)(br_datafile*, char*)) & BrNullOther,
    (int (*)(br_datafile*, char*)) & BrNullOther,
});

C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_file_primitives, _BrFilePrimsReadBinary, 0x00667408, {
    "Read Binary",
    &DfSkipBinary,
    (int (*)(br_datafile*, br_uint_32, br_uint_32)) & BrNullOther,
    &DfChunkReadBinary,
    (void (*)(br_datafile*, br_uint_32)) & BrNullOther,
    &DfCountReadBinary,
    &DfCountSizeBinary,
    (br_uint_32(*)(br_datafile*, br_file_struct*, void*)) & BrNullOther,
    &DfStructReadBinary,
    &DfStructSizeBinary,
    (int (*)(br_datafile*, void*, int, int, int, int)) & BrNullOther,
    &DfBlockReadBinary,
    &DfBlockSizeBinary,
    (int (*)(br_datafile*, char*)) & BrNullOther,
    &DfNameReadBinary,
    &DfNameSizeBinary,
});

C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_file_primitives, _BrFilePrimsWriteBinary, 0x00667448, {
    "Write Binary",
    &DfSkipBinary,
    &DfChunkWriteBinary,
    (int (*)(br_datafile*, br_uint_32*)) & BrNullOther,
    &DfCountWriteBinary,
    (br_uint_32(*)(br_datafile*)) & BrNullOther,
    &DfCountSizeBinary,
    &DfStructWriteBinary,
    (br_uint_32(*)(br_datafile*, br_file_struct*, void*)) & BrNullOther,
    &DfStructSizeBinary,
    &DfBlockWriteBinary,
    (void* (*)(br_datafile*, void*, int*, int, int)) & BrNullOther,
    &DfBlockSizeBinary,
    &DfNameWriteBinary,
    (char* (*)(br_datafile*, char*)) & BrNullOther,
    &DfNameSizeBinary,
});

C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_file_primitives, _BrFilePrimsReadText, 0x00667488, {
    "Read Text",
    &DfSkipText,
    (int (*)(br_datafile*, br_uint_32, br_uint_32)) & BrNullOther,
    &DfChunkReadText,
    (void (*)(br_datafile*, br_uint_32)) & BrNullOther,
    &DfCountReadText,
    &DfCountSizeText,
    (br_uint_32(*)(br_datafile*, br_file_struct*, void*)) & BrNullOther,
    &DfStructReadText,
    &DfStructSizeText,
    (int (*)(br_datafile*, void*, int, int, int, int)) & BrNullOther,
    &DfBlockReadText,
    &DfBlockSizeText,
    (int (*)(br_datafile*, char*)) & BrNullOther,
    &DfNameReadText,
    &DfNameSizeText,
});

C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_file_primitives, _BrFilePrimsWriteText, 0x006674c8, {
    "Write Text",
    &DfSkipText,
    &DfChunkWriteText,
    (int (*)(br_datafile*, br_uint_32*)) & BrNullOther,
    &DfCountWriteText,
    (br_uint_32(*)(br_datafile*)) & BrNullOther,
    &DfCountSizeText,
    &DfStructWriteText,
    (br_uint_32(*)(br_datafile*, br_file_struct*, void*)) & BrNullOther,
    &DfStructSizeText,
    &DfBlockWriteText,
    (void* (*)(br_datafile*, void*, int*, int, int)) & BrNullOther,
    &DfBlockSizeText,
    &DfNameWriteText,
    (char* (*)(br_datafile*, char*)) & BrNullOther,
    &DfNameSizeText,
});

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, member_type_names, 32, 0x00667250, {
    "int_8",
    "uint_8",
    "int_16",
    "uint_16",
    "int_32",
    "uint_32",
    "fixed",
    "angle",
    "float",
    "double",
    "scalar",
    "fraction",
    "ufraction",
    "enum_8",
    "enum_16",
    "enum_32",
    "struct",
    "asciz",
    "colour",
    "vector2",
    "vector3",
    "vector4",
    "fixed_fraction",
    "fixed_ufraction",
    "float_fraction",
    "float_ufraction",
    "fixed_vector2",
    "fixed_vector3",
    "fixed_vector4",
    "float_vector2",
    "float_vector3",
    "float_vector4",
});

typedef struct {
    int type;
    void* value;
    unsigned int count;
} datafile_stack_t;

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(datafile_stack_t, DatafileStack, 1024, 0x006ad9b0);

#if defined(C2_HOOKS_ENABLED) || !defined(BRENDER_FIX_BUGS)
#define CHUNKNAMES_NB 61
#else
#define CHUNKNAMES_NB 62
#endif

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(char*, ChunkNames, CHUNKNAMES_NB, 0x006672d0, {
    "END",
    "IMAGE_PLANE",
    "RLE_IMAGE_PLANE",
#ifdef BRENDER_FIX_BUGS
    "PIXELMAP_OLD",
#else
    "PIXELMAP",
#endif
    "MATERIAL_OLD",
    "ADD_MATERIAL",
    "OLD_ACTOR",
    "OLD_ADD_SIBLING",
    "OLD_ADD_CHILD",
    "OLD_MATERIAL_INDEX",
    "OLD_VERTICES",
    "OLD_VERTICES_UV",
    "OLD_FACES",
    "OLD_MODEL",
    "ADD_MODEL",
    "ANIM",
    "ANIM_TRANSFORM",
    "ANIM_RATE",
    "FILE_INFO",
    "OLD_LIGHT",
    "OLD_CAMERA",
    "PIVOT",
    "MATERIAL_INDEX",
    "VERTICES",
    "VERTEX_UV",
    "OLD_FACES_1",
    "FACE_MATERIAL",
    "OLD_MODEL_1",
    "COLOUR_MAP_REF",
    "OPACITY_MAP_REF",
    "INDEX_BLEND_REF",
    "INDEX_SHADE_REF",
    "SCREENDOOR_REF",
    "PIXELS",
    "ADD_MAP",
    "ACTOR",
    "ACTOR_MODEL",
    "ACTOR_TRANSFORM",
    "ACTOR_MATERIAL",
    "ACTOR_LIGHT",
    "ACTOR_CAMERA",
    "ACTOR_BOUNDS",
    "ACTOR_ADD_CHILD",
    "TRANSFORM_MATRIX34",
    "TRANSFORM_MATRIX34_LP",
    "TRANSFORM_QUAT",
    "TRANSFORM_EULER",
    "TRANSFORM_LOOK_UP",
    "TRANSFORM_TRANSLATION",
    "TRANSFORM_IDENTITY",
    "BOUNDS",
    "LIGHT",
    "CAMERA",
    "FACES",
    "MODEL",
    "ACTOR_CLIP_PLANE",
    "PLANE",
    "SATURN_FACES",
    "SATURN_MODEL",
    "INDEX_FOG_REF",
#ifdef BRENDER_FIX_BUGS
    "MATERIAL",
    "PIXELMAP",
#else
    "MATERIAL_OLD",
#endif
});

C2_HOOK_VARIABLE_IMPLEMENT(int, DatafileStackTop, 0x006b09b0);

void C2_HOOK_STDCALL DfPush(int type, void* value, unsigned int count) {

    if (C2V(DatafileStackTop) >= BR_ASIZE(C2V(DatafileStack))) {
        BrFailure("DatafileStack Overflow");
    }
    C2V(DatafileStack)[C2V(DatafileStackTop)].type = type;
    C2V(DatafileStack)[C2V(DatafileStackTop)].count = count;
    C2V(DatafileStack)[C2V(DatafileStackTop)].value = value;
    C2V(DatafileStackTop)++;
}
C2_HOOK_FUNCTION(0x00529ff0, DfPush)

void* C2_HOOK_STDCALL DfPop(int type, unsigned int* countp) {

    if (C2V(DatafileStackTop) <= 0) {
        BrFailure("DatafileStack Underflow");
    }
    C2V(DatafileStackTop)--;
    if (type != C2V(DatafileStack)[C2V(DatafileStackTop)].type) {
        BrFailure("DatafileStack type mismatch, wanted %d, got %d", type, C2V(DatafileStack)[C2V(DatafileStackTop)].type);
    }
    if (countp != NULL) {
        *countp = C2V(DatafileStack)[C2V(DatafileStackTop)].count;
    }
    return C2V(DatafileStack)[C2V(DatafileStackTop)].value;
}
C2_HOOK_FUNCTION(0x0052a040, DfPop)

void* C2_HOOK_STDCALL DfTop(int type, unsigned int* countp) {
    if (C2V(DatafileStackTop) <= 0)
        BrFailure("DatafileStack Underflow");
    if (type != C2V(DatafileStack)[C2V(DatafileStackTop) - 1].type)
        BrFailure("DatafileStack type mismatch, wanted %d, got %d", type, C2V(DatafileStack)[C2V(DatafileStackTop)].type);
    if (countp != NULL)
        *countp = C2V(DatafileStack)[C2V(DatafileStackTop) - 1].count;
    return C2V(DatafileStack)[C2V(DatafileStackTop) - 1].value;
}
C2_HOOK_FUNCTION(0x0052a0b0, DfTop)

int C2_HOOK_STDCALL DfTopType(void) {
    if (C2V(DatafileStackTop) > 0) {
        return C2V(DatafileStack)[C2V(DatafileStackTop) - 1].type;
    } else {
        return 0;
    }
}
C2_HOOK_FUNCTION(0x0052a120, DfTopType)

int C2_HOOK_STDCALL TextReadLine(br_datafile* df, char** ident, char** data) {
    char* cp;

    while (1) {
        cp = BrScratchString();
        BrFileGetLine(cp, 256, df->h);
        if (BrFileEof(df->h) != 0) {
            return 0;
        }
        for (; (*cp == ' ') || (*cp == '\t'); cp++) {
        }
        if (*cp != '\0')
            break;
    }
    *ident = cp;
    while ((*cp != ' ') && (*cp != '\t') && (*cp != '\0')) {
        cp++;
    }
    *cp = '\0';
    cp++;
    while ((*cp == ' ') || (*cp == '\t')) {
        cp++;
    }
    *data = cp;
    if (*cp == '\"') {
        cp++;
        while ((*cp != '\0') && (*cp != '\"')) {
            cp++;
        }
    } else {
        while ((*cp != ' ') && (*cp != '\t') && (*cp != '\0')) {
            cp++;
        }
    }
    *cp = '\0';
    return 1;
}
C2_HOOK_FUNCTION(0x0052b630, TextReadLine)

br_uint_16 C2_HOOK_STDCALL scalarTypeConvert(br_datafile* df, br_uint_16 t) {

    if (df->scalar_type == BRT_FIXED) {
        switch (t) {
        case DF_TYPE_BR_SCALAR:
            return DF_TYPE_BR_FIXED;
        case DF_TYPE_BR_FRACTION:
            return DF_TYPE_BR_FRACTION_X;
        case DF_TYPE_BR_UFRACTION:
            return DF_TYPE_BR_UFRACTION_X;
        case DF_TYPE_BR_VECTOR2:
            return DF_TYPE_BR_VECTOR2_X;
        case DF_TYPE_BR_VECTOR3:
            return DF_TYPE_BR_VECTOR3_X;
        case DF_TYPE_BR_VECTOR4:
            return DF_TYPE_BR_VECTOR4_X;
        }
    } else if (df->scalar_type == BRT_FLOAT) {
        switch (t) {
        case DF_TYPE_BR_SCALAR:
            return DF_TYPE_FLOAT;
        case DF_TYPE_BR_UFRACTION:
            return DF_TYPE_BR_FRACTION_F;
        case DF_TYPE_BR_FRACTION:
            return DF_TYPE_BR_UFRACTION_F;
        case DF_TYPE_BR_VECTOR2:
            return DF_TYPE_BR_VECTOR2_F;
        case DF_TYPE_BR_VECTOR3:
            return DF_TYPE_BR_VECTOR3_F;
        case DF_TYPE_BR_VECTOR4:
            return DF_TYPE_BR_VECTOR4_F;
        }
    } else if ((t == DF_TYPE_BR_SCALAR) || ((DF_TYPE_BR_VECTOR2 <= t) && (t <= DF_TYPE_BR_VECTOR4))) {
        BrFailure("Incorrect scalar type");
    }
    return t;
}

br_uint_32 C2_HOOK_STDCALL DfStructWriteBinary(br_datafile* df, br_file_struct* str, void* base) {
    unsigned int m;
    int i;
    int n;
    unsigned char* mp;
    br_file_struct_member* sm;
    union { // size: 0x8
        unsigned char b[8]; // @0x0
        float f; // @0x0
        double d; // @0x0
    } conv;

    for (m = 0; m < str->nmembers; m++) {
        sm = &str->members[m];
        mp = ((unsigned char*)base) + sm->offset;

        switch (scalarTypeConvert(df, sm->type)) {
        case DF_TYPE_BR_INT_8:
        case DF_TYPE_BR_UINT_8:
        case DF_TYPE_ENUM_8:
            BrFilePutChar(((br_uint_8*)mp)[0], df->h);
            break;
        case DF_TYPE_BR_ANGLE:
        case DF_TYPE_BR_INT_16:
        case DF_TYPE_BR_UINT_16:
        case DF_TYPE_ENUM_16:
            BrFilePutChar(((br_uint_8*)mp)[1], df->h);
            BrFilePutChar(((br_uint_8*)mp)[0], df->h);
            break;
        case DF_TYPE_BR_INT_32:
        case DF_TYPE_BR_UINT_32:
        case DF_TYPE_ENUM_32:
            BrFilePutChar(((br_uint_8*)mp)[3], df->h);
            BrFilePutChar(((br_uint_8*)mp)[2], df->h);
            BrFilePutChar(((br_uint_8*)mp)[1], df->h);
            BrFilePutChar(((br_uint_8*)mp)[0], df->h);
            break;
        case DF_TYPE_FLOAT:
        case DF_TYPE_BR_FRACTION_F:
        case DF_TYPE_BR_UFRACTION_F:
            conv.f = *(float*)mp;
            BrFilePutChar(conv.b[3], df->h);
            BrFilePutChar(conv.b[2], df->h);
            BrFilePutChar(conv.b[1], df->h);
            BrFilePutChar(conv.b[0], df->h);
            break;
        case DF_TYPE_DOUBLE:
            conv.d = *(double*)mp;
            BrFilePutChar(conv.b[7], df->h);
            BrFilePutChar(conv.b[6], df->h);
            BrFilePutChar(conv.b[5], df->h);
            BrFilePutChar(conv.b[4], df->h);
            BrFilePutChar(conv.b[3], df->h);
            BrFilePutChar(conv.b[2], df->h);
            BrFilePutChar(conv.b[1], df->h);
            BrFilePutChar(conv.b[0], df->h);
            break;
        case DF_TYPE_STRUCT:
            DfStructWriteBinary(df, sm->extra, mp);
            break;
        case DF_TYPE_ASCIZ:
            if (*(char**)mp != NULL) {
                BrFileWrite(*(char**)mp, 1, BrStrLen(*(char**)mp), df->h);
            }
            BrFilePutChar('\0', df->h);
            break;
        case DF_TYPE_BR_COLOUR:
            BrFilePutChar(((br_uint_8*)mp)[2], df->h);
            BrFilePutChar(((br_uint_8*)mp)[1], df->h);
            BrFilePutChar(((br_uint_8*)mp)[0], df->h);
            break;
        case DF_TYPE_BR_FIXED:
            conv.f = BrFixedToFloat(*(br_fixed_ls*)mp);
            BrFilePutChar(conv.b[3], df->h);
            BrFilePutChar(conv.b[2], df->h);
            BrFilePutChar(conv.b[1], df->h);
            BrFilePutChar(conv.b[0], df->h);
            break;
        case DF_TYPE_BR_FRACTION_X:
            conv.f = BrFixedFractionToFloat(*(br_fraction_x*)mp);
            BrFilePutChar(conv.b[3], df->h);
            BrFilePutChar(conv.b[2], df->h);
            BrFilePutChar(conv.b[1], df->h);
            BrFilePutChar(conv.b[0], df->h);
            break;
        case DF_TYPE_BR_UFRACTION_X:
            conv.f = BrFixedUFractionToFloat(*(br_fraction_x*)mp);
            BrFilePutChar(conv.b[3], df->h);
            BrFilePutChar(conv.b[2], df->h);
            BrFilePutChar(conv.b[1], df->h);
            BrFilePutChar(conv.b[0], df->h);
            break;
        case DF_TYPE_BR_VECTOR2_X:
            n = 2;
            goto copy_fixed_vector;
        case DF_TYPE_BR_VECTOR3_X:
            n = 3;
            goto copy_fixed_vector;
        case DF_TYPE_BR_VECTOR4_X:
            n = 4;
        copy_fixed_vector:
            for (i = 0; i < n; i++) {
                conv.f = BrFixedToFloat(*(br_fixed_ls*)mp);
                BrFilePutChar(conv.b[3], df->h);
                BrFilePutChar(conv.b[2], df->h);
                BrFilePutChar(conv.b[1], df->h);
                BrFilePutChar(conv.b[0], df->h);
                mp += sizeof(br_fixed_ls);
            }
            break;
        case DF_TYPE_BR_VECTOR2_F:
            n = 2;
            goto copy_float_vector;
        case DF_TYPE_BR_VECTOR3_F:
            n = 3;
            goto copy_float_vector;
        case DF_TYPE_BR_VECTOR4_F:
            n = 4;
        copy_float_vector:
            for (i = 0; i < n; i++) {
                conv.f = *(float*)mp;
                BrFilePutChar(conv.b[3], df->h);
                BrFilePutChar(conv.b[2], df->h);
                BrFilePutChar(conv.b[1], df->h);
                BrFilePutChar(conv.b[0], df->h);
                mp += sizeof(br_fixed_ls);
            }
            break;
        }
    }
    return 1;
}
C2_HOOK_FUNCTION(0x0052a140, DfStructWriteBinary)

br_uint_32 C2_HOOK_STDCALL DfStructReadBinary(br_datafile* df, br_file_struct* str, void* base) {
    char tmp_string[256];
    unsigned int m;
    int i;
    int c;
    int n;
    unsigned char* mp;
    br_file_struct_member* sm;
    union {
        unsigned char b[8];
        float f;
        double d;
    } conv;


    for (m = 0; m < str->nmembers; m++) {
        sm = &str->members[m];

        c = scalarTypeConvert(df, sm->type);
        mp = ((unsigned char*)base) + sm->offset;

        switch (c) {
        case DF_TYPE_BR_INT_8:
        case DF_TYPE_BR_UINT_8:
        case DF_TYPE_ENUM_8:
            mp[0] = BrFileGetChar(df->h);
            break;
        case DF_TYPE_BR_INT_16:
        case DF_TYPE_BR_UINT_16:
        case DF_TYPE_BR_ANGLE:
        case DF_TYPE_ENUM_16:
            mp[1] = BrFileGetChar(df->h);
            mp[0] = BrFileGetChar(df->h);
            break;
        case DF_TYPE_BR_INT_32:
        case DF_TYPE_BR_UINT_32:
        case DF_TYPE_FLOAT:
        case DF_TYPE_ENUM_32:
        case DF_TYPE_BR_FRACTION_F:
        case DF_TYPE_BR_UFRACTION_F:
            mp[3] = BrFileGetChar(df->h);
            mp[2] = BrFileGetChar(df->h);
            mp[1] = BrFileGetChar(df->h);
            mp[0] = BrFileGetChar(df->h);
            break;
        case DF_TYPE_BR_FIXED:
            conv.b[3] = BrFileGetChar(df->h);
            conv.b[2] = BrFileGetChar(df->h);
            conv.b[1] = BrFileGetChar(df->h);
            conv.b[0] = BrFileGetChar(df->h);
            *(br_fixed_ls*)mp = BrFloatToFixed(conv.f);
            break;
        case DF_TYPE_DOUBLE:
            conv.b[7] = BrFileGetChar(df->h);
            conv.b[6] = BrFileGetChar(df->h);
            conv.b[5] = BrFileGetChar(df->h);
            conv.b[4] = BrFileGetChar(df->h);
            conv.b[3] = BrFileGetChar(df->h);
            conv.b[2] = BrFileGetChar(df->h);
            conv.b[1] = BrFileGetChar(df->h);
            conv.b[0] = BrFileGetChar(df->h);
            *(double*)mp = conv.d;
            break;
        case DF_TYPE_STRUCT:
            DfStructReadBinary(df, sm->extra, mp);
            break;
        case DF_TYPE_ASCIZ:
            for (n = 0; n < 255; n++) {
                c = BrFileGetChar(df->h);
                if (c == '\0' || c == -1)
                    break;
                tmp_string[n] = c;
            }
            tmp_string[n] = 0;
            *(char**)mp = (char*)BrResStrDup(df->res ? df->res : C2V(fw).res, tmp_string);
            break;
        case DF_TYPE_BR_COLOUR:
            mp[2] = BrFileGetChar(df->h);
            mp[1] = BrFileGetChar(df->h);
            mp[0] = BrFileGetChar(df->h);
            break;
        case DF_TYPE_BR_FRACTION_X:
            conv.b[3] = BrFileGetChar(df->h);
            conv.b[2] = BrFileGetChar(df->h);
            conv.b[1] = BrFileGetChar(df->h);
            conv.b[0] = BrFileGetChar(df->h);
            *((br_fraction_x*)mp) = BrFloatToFixedFraction(conv.f);
            break;
        case DF_TYPE_BR_UFRACTION_X:
            conv.b[3] = BrFileGetChar(df->h);
            conv.b[2] = BrFileGetChar(df->h);
            conv.b[1] = BrFileGetChar(df->h);
            conv.b[0] = BrFileGetChar(df->h);
            *(br_ufraction_x*)mp = BrFloatToFixedUFraction(conv.f);
            break;
        case DF_TYPE_BR_VECTOR2_X:
            n = 2;
            goto copy_fixed_vector;
        case DF_TYPE_BR_VECTOR3_X:
            n = 3;
            goto copy_fixed_vector;
        case DF_TYPE_BR_VECTOR4_X:
            n = 4;
        copy_fixed_vector:
            for (i = 0; i < n; i++) {
                conv.b[3] = BrFileGetChar(df->h);
                conv.b[2] = BrFileGetChar(df->h);
                conv.b[1] = BrFileGetChar(df->h);
                conv.b[0] = BrFileGetChar(df->h);
                *(br_fixed_ls*)mp = BrFloatToFixed(conv.f);
                mp += sizeof(br_fixed_ls);
            }
            break;
        case DF_TYPE_BR_VECTOR2_F:
            n = 2;
            goto copy_float_vector;
            break;
        case DF_TYPE_BR_VECTOR3_F:
            n = 3;
            goto copy_float_vector;
            break;
        case DF_TYPE_BR_VECTOR4_F:
            n = 4;
        copy_float_vector:
            for (i = 0; i < n; i++) {
                conv.b[3] = BrFileGetChar(df->h);
                conv.b[2] = BrFileGetChar(df->h);
                conv.b[1] = BrFileGetChar(df->h);
                conv.b[0] = BrFileGetChar(df->h);
                *(float*)mp = conv.f;
                mp += sizeof(float);
            }
            break;
        }
    }

    return 1;
}
C2_HOOK_FUNCTION(0x0052a610, DfStructReadBinary)

int C2_HOOK_STDCALL DfStructSizeBinary(br_datafile* df, br_file_struct* str, void* base) {
    unsigned char* mp;
    unsigned int m;
    br_file_struct_member* sm;
    int bytes;

    bytes = 0;
    for (m = 0; m < str->nmembers; m++) {
        sm = &str->members[m];

        mp = ((unsigned char*)base) + sm->offset;

        switch (scalarTypeConvert(df, sm->type)) {
        case DF_TYPE_BR_INT_8:
        case DF_TYPE_BR_UINT_8:
        case DF_TYPE_ENUM_8:
            bytes += 1;
            break;
        case DF_TYPE_BR_INT_16:
        case DF_TYPE_BR_UINT_16:
        case DF_TYPE_BR_ANGLE:
        case DF_TYPE_ENUM_16:
            bytes += 2;
            break;
        case DF_TYPE_BR_COLOUR:
            bytes += 4;
            break;
        case DF_TYPE_BR_INT_32:
        case DF_TYPE_BR_UINT_32:
        case DF_TYPE_FLOAT:
        case DF_TYPE_ENUM_32:
        case DF_TYPE_BR_FRACTION_F:
        case DF_TYPE_BR_UFRACTION_F:
        case DF_TYPE_BR_FIXED:
        case DF_TYPE_BR_FRACTION_X:
        case DF_TYPE_BR_UFRACTION_X:
            bytes += 4;
            break;
        case DF_TYPE_DOUBLE:
            bytes += 8;
            break;
        case DF_TYPE_STRUCT:
            bytes += DfStructSizeBinary(df, sm->extra, mp);
            break;
        case DF_TYPE_ASCIZ:
            if (*(char**)mp != NULL) {
                bytes += BrStrLen(*(char**)mp);
            }
            break;
        case DF_TYPE_BR_VECTOR2_X:
        case DF_TYPE_BR_VECTOR2_F:
            bytes += 8;
            break;
        case DF_TYPE_BR_VECTOR3_X:
        case DF_TYPE_BR_VECTOR3_F:
            bytes += 12;
            break;
        case DF_TYPE_BR_VECTOR4_X:
        case DF_TYPE_BR_VECTOR4_F:
            bytes += 16;
            break;
        }
    }

    return bytes;
}
C2_HOOK_FUNCTION(0x0052ab30, DfStructSizeBinary)

int C2_HOOK_STDCALL EnumFromString(br_file_enum* e, char* str) {
    unsigned int m;

    if (e == NULL) {
        BrFailure("Unknown enum string: %s", str);
    }
    for (m = 0; m < e->nmembers; m++) {
        if (BrStrCmp(str, e->members[m].name) == 0) {
            return e->members[m].value;
        }
    }
    BrFailure("Unknown enum string: %s", str);
    return -1;
}

char* C2_HOOK_STDCALL EnumToString(br_file_enum* e, int num) {
    unsigned int m;

    if (e == NULL) {
        goto fail;
    }
    for (m = 0; m < e->nmembers; m++) {
        if (e->members[m].value == num) {
            return e->members[m].name;
        }
    }
fail:
    BrFailure("Unknown enum %d", num);
    return NULL;
}

br_uint_32 C2_HOOK_STDCALL DfStructWriteText(br_datafile* df, br_file_struct* str, void* base) {

    BrFilePrintf(df->h, "  struct    %s\n", str->name);
    StructWriteTextSub(df, str, base, 4);
    return 1;
}
C2_HOOK_FUNCTION(0x0052ada0, DfStructWriteText)

br_uint_32 C2_HOOK_STDCALL StructWriteTextSub(br_datafile* df, br_file_struct* str, void* base, int indent) {
    unsigned int m;
    int i;
    int w;
    int add_comment;
    void* mp;
    br_file_struct_member* sm;

    for (m = 0; m < str->nmembers; m++) {
        sm = &str->members[m];
        mp = ((unsigned char*)base) + sm->offset;
        w = 0;
        add_comment = 1;

        for (i = 0; i < indent; i++) {
            BrFilePutChar(' ', df->h);
        }

        // Modified from "%-10s", to ensure space after long type names
        BrFilePrintf(df->h, "%-9s ", C2V(member_type_names)[sm->type]);

        switch (scalarTypeConvert(df, sm->type)) {
        case DF_TYPE_BR_INT_8:
            w = BrFilePrintf(df->h, "%d", *(br_int_8*)mp);
            break;
        case DF_TYPE_BR_UINT_8:
            w = BrFilePrintf(df->h, "%u", *(br_uint_8*)mp);
            break;
        case DF_TYPE_BR_INT_16:
            w = BrFilePrintf(df->h, "%d", *(br_int_16*)mp);
            break;
        case DF_TYPE_BR_UINT_16:
            w = BrFilePrintf(df->h, "%u", *(br_uint_16*)mp);
            break;
        case DF_TYPE_BR_INT_32:
            w = BrFilePrintf(df->h, "%d", *(br_int_32*)mp);
            break;
        case DF_TYPE_BR_UINT_32:
            w = BrFilePrintf(df->h, "%u", *(br_uint_32*)mp);
            break;
        case DF_TYPE_BR_FIXED:
            w = BrFilePrintf(df->h, "%g", BrFixedToFloat(*(br_fixed_ls*)mp));
            break;
        case DF_TYPE_BR_ANGLE:
            w = BrFilePrintf(df->h, "%g", BrFixedToFloat(BrFixedMul(*(br_angle*)mp, BrIntToFixed(360))));
            break;
        case DF_TYPE_FLOAT:
        case DF_TYPE_BR_FRACTION_F:
        case DF_TYPE_BR_UFRACTION_F:
            w = BrFilePrintf(df->h, "%g", (double)*(float*)mp);
            break;
        case DF_TYPE_DOUBLE:
            w = BrFilePrintf(df->h, "%g", *(double*)mp);
            break;
        case DF_TYPE_ENUM_8:
            w = BrFilePrintf(df->h, "%s", EnumToString(sm->extra, *(br_uint_8*)mp));
            break;
        case DF_TYPE_ENUM_16:
            w = BrFilePrintf(df->h, "%s", EnumToString(sm->extra, *(br_uint_16*)mp));
            break;
        case DF_TYPE_ENUM_32:
            w = BrFilePrintf(df->h, "%s", EnumToString(sm->extra, *(br_uint_32*)mp));
            break;
        case DF_TYPE_STRUCT:
            add_comment = 0;
            w = BrFilePrintf(df->h, "%s", ((br_file_struct*)sm->extra)->name);
            if (sm->name != NULL) {
                if (w < 40) {
                    for (i = 40 - w; i != 0; i--) {
                        BrFilePutChar(' ', df->h);
                    }
                }
                BrFilePrintf(df->h, " # %s", sm->name);
            }
            BrFilePutChar('\n', df->h);
            StructWriteTextSub(df, sm->extra, mp, indent + 2);
            break;
        case DF_TYPE_ASCIZ:
            if (*(char**)mp == NULL) {
                w = BrFilePrintf(df->h, "NULL");
            } else {
                w = BrFilePrintf(df->h, "\"%s\"", *(char**)mp);
            }
            break;
        case DF_TYPE_BR_COLOUR:
            w = BrFilePrintf(df->h, "%d,%d,%d", (br_uint_8)((*(br_uint_32*)mp) >> 16), (br_uint_8)((*(br_uint_32*)mp) >> 8), (br_uint_8)((*(br_uint_32*)mp)));
            break;
        case DF_TYPE_BR_FRACTION_X:
            w = BrFilePrintf(df->h, "%g", (double)BrFixedFractionToFloat(*(br_fraction_x*)mp));
            break;
        case DF_TYPE_BR_UFRACTION_X:
            w = BrFilePrintf(df->h, "%g", (double)BrFixedUFractionToFloat(*(br_fraction_x*)mp));
            break;
        case DF_TYPE_BR_VECTOR2_X:
            w = BrFilePrintf(df->h, "%g,%g", BrFixedToFloat(((br_fixed_ls*)mp)[0]), BrFixedToFloat(((br_fixed_ls*)mp)[1]));
            break;
        case DF_TYPE_BR_VECTOR3_X:
            w = BrFilePrintf(df->h, "%g,%g,%g", BrFixedToFloat(((br_fixed_ls*)mp)[0]), BrFixedToFloat(((br_fixed_ls*)mp)[1]), BrFixedToFloat(((br_fixed_ls*)mp)[2]));
            break;
        case DF_TYPE_BR_VECTOR4_X:
            w = BrFilePrintf(df->h, "%g,%g,%g,%g", BrFixedToFloat(((br_fixed_ls*)mp)[0]), BrFixedToFloat(((br_fixed_ls*)mp)[1]), BrFixedToFloat(((br_fixed_ls*)mp)[2]), BrFixedToFloat(((br_fixed_ls*)mp)[3]));
            break;
        case DF_TYPE_BR_VECTOR2_F:
            w = BrFilePrintf(df->h, "%g,%g", ((float*)mp)[0], ((float*)mp)[1]);
            break;
        case DF_TYPE_BR_VECTOR3_F:
            w = BrFilePrintf(df->h, "%g,%g,%g", ((float*)mp)[0], ((float*)mp)[1], ((float*)mp)[2]);
            break;
        case DF_TYPE_BR_VECTOR4_F:
            w = BrFilePrintf(df->h, "%g,%g,%g,%g", ((float*)mp)[0], ((float*)mp)[1], ((float*)mp)[2], ((float*)mp)[3]);
            break;
        }
        if ((add_comment != 0) && (sm->name != NULL)) {
            if (w < 40) {
                for (i = 40 - w; i != 0; i--) {
                    BrFilePutChar(' ', df->h);
                }
            }
            BrFilePrintf(df->h, " # %s\n", sm->name);
        }
    }
    return 1;
}
C2_HOOK_FUNCTION(0x0052ade0, StructWriteTextSub)

br_uint_32 C2_HOOK_STDCALL DfStructReadText(br_datafile* df, br_file_struct* str, void* base) {
    char* id;
    char* data;

    TextReadLine(df, &id, &data);
    if (BrStrCmp(id, "struct") != 0) {
        BrFailure("Unknown text identifer \"%s\"", id);
    }
    if (BrStrCmp(data, str->name) != 0) {
        BrFailure("Incorrect structure name \"%s\"", data);
    }
    StructReadTextSub(df, str, base);
    return 1;
}
C2_HOOK_FUNCTION(0x0052b5b0, DfStructReadText)

br_uint_32 C2_HOOK_STDCALL StructReadTextSub(br_datafile* df, br_file_struct* str, void* base) {
    unsigned int m;
    unsigned int r;
    unsigned int g;
    unsigned int b;
    int i;
    int n;
    void* mp;
    br_file_struct_member* sm;
    char* id;
    char* data;
    char* ep;

    for (m = 0; m < str->nmembers; m++) {
        sm = &str->members[m];
        mp = ((unsigned char*)base) + sm->offset;

        if (TextReadLine(df, &id, &data) == 0) {
            BrFailure("Unexpected EOF in strructure");
        }
        if (BrStrCmp(id, C2V(member_type_names)[sm->type]) != 0) {
            BrFailure("Unknown member identifer \"%s\"", id);
        }

        switch (scalarTypeConvert(df, sm->type)) {
        case DF_TYPE_BR_INT_8:
            *(br_int_8*)mp = (br_int_8)BrStrToL(data, NULL, 0);
            break;
        case DF_TYPE_BR_UINT_8:
            *(br_uint_8*)mp = (br_uint_8)BrStrToL(data, NULL, 0);
            break;
        case DF_TYPE_BR_INT_16:
            *(br_int_16*)mp = (br_int_16)BrStrToL(data, NULL, 0);
            break;
        case DF_TYPE_BR_UINT_16:
            *(br_uint_16*)mp = (br_uint_16)BrStrToL(data, NULL, 0);
            break;
        case DF_TYPE_BR_INT_32:
            *(br_int_32*)mp = (br_int_32)BrStrToL(data, NULL, 0);
            break;
        case DF_TYPE_BR_UINT_32:
            *(br_uint_32*)mp = (br_uint_32)BrStrToUL(data, NULL, 0);
            break;
        case DF_TYPE_BR_FIXED:
            *(br_fixed_ls*)mp = BrFloatToFixed(BrStrToD(data, NULL));
            break;
        case DF_TYPE_BR_ANGLE:
            *(br_angle*)mp = BrFixedMulDiv(BrFloatToFixed(BrStrToF(data, NULL)), BrIntToFixed(1), BrIntToFixed(360));
            break;
        case DF_TYPE_FLOAT:
        case DF_TYPE_BR_FRACTION_F:
        case DF_TYPE_BR_UFRACTION_F:
            *(float*)mp = BrStrToF(data, NULL);
            break;
        case DF_TYPE_DOUBLE:
            *(double*)mp = BrStrToD(data, NULL);
            break;
        case DF_TYPE_ENUM_8:
            *(br_uint_8*)mp = EnumFromString(sm->extra, data);
            break;
        case DF_TYPE_ENUM_16:
            *(br_uint_16*)mp = EnumFromString(sm->extra, data);
            break;
        case DF_TYPE_ENUM_32:
            *(br_uint_32*)mp = EnumFromString(sm->extra, data);
            break;
        case DF_TYPE_STRUCT:
            if (BrStrCmp(data, ((br_file_struct*)sm->extra)->name) != 0) {
                BrFailure("Incorrect structure name \"%s\"", data);
            }
            StructReadTextSub(df, sm->extra, mp);
            break;
        case DF_TYPE_ASCIZ:
            if (BrStrCmp(data, "NULL") == 0) {
                *(char**)mp = NULL;
            } else {
                *(char**)mp = BrResStrDup(df->res == NULL ? C2V(fw).res : df->res, data + 1);
            }
            break;
        case DF_TYPE_BR_COLOUR:
            r = BrStrToUL(data, &ep, 0);
            if (*ep != ',') {
                BrFailure("Incorrect colour");
            }
            g = BrStrToUL(ep + 1, &ep, 0);
            if (*ep != ',') {
                BrFailure("Incorrect colour");
            }
            b = BrStrToUL(ep + 1, &ep, 0);
            *(br_colour*)mp = BR_COLOUR_RGB(r, g, b);
            break;
        case DF_TYPE_BR_FRACTION_X:
            *(br_fixed_ss*)mp = BrFloatToFixedFraction(BrStrToD(data, NULL));
            break;
        case DF_TYPE_BR_UFRACTION_X:
            *(br_fixed_su*)mp = BrFloatToFixedUFraction(BrStrToD(data, NULL));
            break;
        case DF_TYPE_BR_VECTOR2_X:
            n = 2;
            goto copy_fixed_vector;
        case DF_TYPE_BR_VECTOR3_X:
            n = 3;
            goto copy_fixed_vector;
        case DF_TYPE_BR_VECTOR4_X:
            n = 4;
        copy_fixed_vector:
            for (i = 0; i < n; i++) {
                while (1) {
                    if ((*data != ',') && (*data != ' ') && (*data != '\t')) {
                        break;
                    }
                    data++;
                }
                if (*data == '\0') {
                    BrFailure("Incorrect vector");
                }
                *(br_fixed_ls*)mp = BrFloatToFixed(BrStrToF(data, NULL));
                while ((*data != ' ' && *data != ',' && *data != ' ' && *data != '\t')) {
                    data++;
                }
                mp = (char*)mp + sizeof(br_fixed_ls);
            }
            break;
        case DF_TYPE_BR_VECTOR2_F:
            n = 2;
            goto copy_float_vector;
        case DF_TYPE_BR_VECTOR3_F:
            n = 3;
            goto copy_float_vector;
        case DF_TYPE_BR_VECTOR4_F:
            n = 4;
        copy_float_vector:
            for (i = 0; i < n; i++) {
                while (1) {
                    if ((*data != ',') && (*data != ' ') && (*data != '\t')) {
                        break;
                    }
                    data++;
                }
                if (*data == '\0') {
                    BrFailure("Incorrect vector");
                }
                *(float*)mp = BrStrToF(data, NULL);
                while ((*data != ' ' && *data != ',' && *data != ' ' && *data != '\t')) {
                    data++;
                }
                mp = (char*)mp + sizeof(float);
            }
            break;
        }
    }

    return 1;
}
C2_HOOK_FUNCTION(0x0052b720, StructReadTextSub)

int C2_HOOK_STDCALL DfStructSizeText(br_datafile* df, br_file_struct* str, void* base) {
    unsigned int m;
    br_file_struct_member* sm;
    int lines;

    lines = 1;
    for (m = 0; m < str->nmembers; m++) {
        sm = &str->members[m];
        if (sm->type == DF_TYPE_STRUCT) {
            lines += DfStructSizeText(df, sm->extra, (br_uint_8*)base + sm->offset);
        } else {
            lines++;
        }
    }
    return lines;
}
C2_HOOK_FUNCTION(0x0052be60, DfStructSizeText)

br_uint_32 C2_HOOK_STDCALL DfStructWriteArray(br_datafile* df, br_file_struct* str, void* base, int n) {
    char* cp;
    int i;

    cp = (char*)base;
    for (i = 0; i < n; i++) {
        df->prims->struct_write(df, str, cp);
        cp += str->mem_size;
    }
    return i;
}
C2_HOOK_FUNCTION(0x0052beb0, DfStructWriteArray)

br_uint_32 C2_HOOK_STDCALL DfStructReadArray(br_datafile* df, br_file_struct* str, void* base, int n) {
    char* cp;
    int i;

    cp = (char*)base;
    for (i = 0; i < n; i++) {
        if (BrFileEof(df->h) != 0) {
            break;
        }
        df->prims->struct_read(df, str, cp);
        cp += str->mem_size;
    }
    return i;
}
C2_HOOK_FUNCTION(0x0052bef0, DfStructReadArray)

int C2_HOOK_STDCALL DfChunkWriteText(br_datafile* df, br_uint_32 id, br_uint_32 length) {

    if (id < BR_ASIZE(C2V(ChunkNames))) {
        BrFilePrintf(df->h, "*%-16s %d\n", C2V(ChunkNames)[id], length);
    } else {
        BrFilePrintf(df->h, "*0x%08x %d\n", id, length);
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0052bf40, DfChunkWriteText)

int C2_HOOK_STDCALL DfChunkReadText(br_datafile* df, br_uint_32* plength) {
    int i;
    char* id;
    char* data;

    if (TextReadLine(df, &id, &data) == 0) {
        return -1;
    }
    if (*id != '*') {
        BrFailure("Chunk ID not found");
    }
    id++;
    if (*id == '0') {
        i = BrStrToUL(id, NULL, 0);
    } else {
        for (i = 0; i < BR_ASIZE(C2V(ChunkNames)); i++) {
            if (BrStrCmp(C2V(ChunkNames)[i], id) == 0) {
                break;
            }
        }
        if (i >= BR_ASIZE(C2V(ChunkNames))) {
            BrFailure("Chunk ID not known: %s", id);
        }
    }
    if (plength != NULL) {
        *plength = BrStrToUL(data, NULL, 0);
    }
    return i;
}
C2_HOOK_FUNCTION(0x0052bf90, DfChunkReadText)

int C2_HOOK_STDCALL DfChunkWriteBinary(br_datafile* df, br_uint_32 id, br_uint_32 length) {
    br_uint_32 l;

    l = BrSwap32(id);
    BrFileWrite(&l, sizeof(br_uint_32), 1, df->h);
    l = BrSwap32(length);
    BrFileWrite(&l, sizeof(br_uint_32), 1, df->h);
    return 0;
}
C2_HOOK_FUNCTION(0x0052c050, DfChunkWriteBinary)

int C2_HOOK_STDCALL DfChunkReadBinary(br_datafile* df, br_uint_32* plength) {
    br_uint_32 id;
    br_uint_32 l;

    if (BrFileEof(df->h) != 0) {
        return -1;
    }
    BrFileRead(&id, sizeof(br_uint_32), 1, df->h);
    if (BrFileEof(df->h) != 0) {
        return -1;
    }
    id = BrSwap32(id);
    BrFileRead(&l, sizeof(br_uint_32), 1, df->h);
    if (BrFileEof(df->h) != 0) {
        return -1;
    }
    if (plength != NULL) {
        *plength = BrSwap32(l);
    }
    return id;
}
C2_HOOK_FUNCTION(0x0052c0b0, DfChunkReadBinary)

void C2_HOOK_STDCALL DfCountWriteText(br_datafile* df, br_uint_32 count) {

    BrFilePrintf(df->h, " count %d\n", count);
}
C2_HOOK_FUNCTION(0x0052c160, DfCountWriteText)

br_uint_32 C2_HOOK_STDCALL DfCountReadText(br_datafile* df) {
    char* id;
    char* data;

    TextReadLine(df, &id, &data);
    if (BrStrCmp(id, "count") != 0) {
        BrFailure("no element count for chunk");
    }
    return BrStrToUL(data, NULL, 0);
}
C2_HOOK_FUNCTION(0x0052c180, DfCountReadText)

void C2_HOOK_STDCALL DfCountWriteBinary(br_datafile* df, br_uint_32 count) {
    br_uint_32 l;

    l = BrSwap32(count);
    BrFileWrite(&l, sizeof(l), 1, df->h);
}
C2_HOOK_FUNCTION(0x0052c1d0, DfCountWriteBinary)

br_uint_32 C2_HOOK_STDCALL DfCountReadBinary(br_datafile* df) {
    br_uint_32 l;

    BrFileRead(&l, sizeof(br_uint_32), 1, df->h);
    return BrSwap32(l);
}
C2_HOOK_FUNCTION(0x0052c200, DfCountReadBinary)

int C2_HOOK_STDCALL DfCountSizeText(br_datafile* df) {

    return 1;
}
C2_HOOK_FUNCTION(0x0052c230, DfCountSizeText)

int C2_HOOK_STDCALL DfCountSizeBinary(br_datafile* df) {

    return sizeof(br_uint_32);
}
C2_HOOK_FUNCTION(0x0052c240, DfCountSizeBinary)

br_uint_8* C2_HOOK_STDCALL BlockWriteSetup(void* base, int block_size, int block_stride, int block_count, int size) {
    c2_abort();
    return NULL;
}

int C2_HOOK_STDCALL DfBlockWriteText(br_datafile* df, void* base, int block_size, int block_stride, int block_count, int size) {
    int i;
    br_uint_8* block;
    int count;

    if (block_stride == block_size) {
        block_size = block_count * block_size;
        block_count = 1;
    }
    block = base;
    if ((size != 1) || (block_count != 1)) {
        block = BrScratchAllocate(block_count * block_size * size);
        for (count = 0; count < block_count; count++) {
            BrMemCpy(block + count * block_size * size, (br_uint_8*)base + count * block_stride * size, block_size * size);
        }
        BrSwapBlock(block, block_count * block_size, size);
    }
    BrFilePrintf(df->h, "  block %d\n", block_count * block_size);
    BrFilePrintf(df->h, "  size %d\n", size);
    for (i = 0; i < block_count * block_size * size; i++) {
        if ((i & 0x1f) == 0) {
            BrFilePrintf(df->h, "    %08x: %02x", i, block[i]);
        } else {
            BrFilePrintf(df->h, "%02x", block[i]);
        }
        if ((i & 0x1f) == 0x1f) {
            BrFilePutChar('\n', df->h);
        }
    }
    if ((i & 0x1f) != 0x0) {
        BrFilePutChar('\n', df->h);
    }
    if (block != base) {
        BrScratchFree(block);
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0052c250, DfBlockWriteText)

void* C2_HOOK_STDCALL DfBlockReadText(br_datafile* df, void* base, int* count, int size, int mtype) {
    char* id;
    char* data;
    int l;
    int s;
    int a;
    char b[3];

    TextReadLine(df, &id, &data);
    if (BrStrCmp(id, "block") != 0) {
        BrFailure("no block");
    }
    l = BrStrToUL(data, NULL, 0);
    TextReadLine(df, &id, &data);
    if (BrStrCmp(id, "size") != 0) {
        BrFailure("no size");
    }
    s = BrStrToUL(data, NULL, 0);
    if (s != size) {
        BrFailure("block size mismatch");
    }
    if (base == NULL) {
        base = BrResAllocate(df->res ? df->res : C2V(fw).res, l * size, mtype);
    } else {
        if (*count < l) {
            BrFailure("DfBlockReadText: block too long: %d", l);
        }
    }
    *count = l;
    for (a = 0; a < l * size;) {
        TextReadLine(df, &id, &data);
        if (a != BrStrToL(id, NULL, 16)) {
            BrFailure("block address mismatch");
        }
        while ((data[0] != '\0') && (data[1] != '\0')) {
            b[0] = data[0];
            b[1] = data[1];
            b[2] = '\0';
            ((br_uint_8*)base)[a] = (br_uint_8)BrStrToUL(b, NULL, 16);
            data += 2;
            a++;
        }
    }
    BrSwapBlock(base, l, size);
    return base;
}
C2_HOOK_FUNCTION(0x0052c3d0, DfBlockReadText)

int C2_HOOK_STDCALL DfBlockWriteBinary(br_datafile* df, void* base, int block_size, int block_stride, int block_count, int size) {
    int count;
    br_uint_32 l;
    br_uint_32 s;
    void* block;

    l = BrSwap32(block_count * block_size);
    s = BrSwap32(size);
    if (block_stride == block_size) {
        block_size = block_count * block_size;
        block_count = 1;
    }
    block = base;
    if ((size != 1) || (block_count != 1)) {
        block = BrScratchAllocate(block_count * block_size * size);
        for (count = 0; count < block_count; count++) {
            BrMemCpy((br_uint_8*)block + count * block_size * size, (br_uint_8*)base + count * block_stride * size, block_size * size);
        }
        BrSwapBlock(block, block_count * block_size, size);
    }
    BrFileWrite(&l, sizeof(l), 1, df->h);
    BrFileWrite(&s, sizeof(s), 1, df->h);
    BrFileWrite(block, block_count * block_size, size, df->h);
    if (block != base) {
        BrScratchFree(block);
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0052c570, DfBlockWriteBinary)

void* C2_HOOK_STDCALL DfBlockReadBinary(br_datafile* df, void* base, int* count, int size, int mtype) {
    int l;
    int s;

    BrFileRead(&l, 4, 1, df->h);
    l = BrSwap32(l);
    BrFileRead(&s, 4, 1, df->h);
    s = BrSwap32(s);
    if (s != size) {
        BrFailure("block size mismatch");
    }
    if (base != NULL) {
        if (l > *count) {
            BrFailure("DfBlockReadBinary: block too long: %d", l);
        }
    } else {
        base = BrResAllocate(df->res != NULL ? df->res : C2V(fw).res, size * l, mtype);
    }
    *count = l;
    BrFileRead(base, l, size, df->h);
    BrSwapBlock(base, l, size);
    return base;
}
C2_HOOK_FUNCTION(0x0052c690, DfBlockReadBinary)

int C2_HOOK_STDCALL DfBlockSizeText(br_datafile* df, void* base, int block_size, int block_stride, int block_count, int size) {

    return ((size * block_count * block_size + (32 - 1)) >> 5) + 2;
}
C2_HOOK_FUNCTION(0x0052c780, DfBlockSizeText)

int C2_HOOK_STDCALL DfBlockSizeBinary(br_datafile* df, void* base, int block_size, int block_stride, int block_count, int size) {

    return size * block_count * block_size + 8;
}
C2_HOOK_FUNCTION(0x0052c7a0, DfBlockSizeBinary)

char* C2_HOOK_STDCALL DfNameReadText(br_datafile* df, char* name) {
    char* id;
    char* data;

    TextReadLine(df, &id, &data);
    if (BrStrCmp(id, "name") != 0) {
        BrFailure("no name");
    }
    if ((data != NULL) && (*data == '\"')) {
        BrStrNCpy(name, data + 1, 0xff);
        name[0xff] = '\0';
        return data;
    }
    BrFailure("no name string");
    return NULL;
}
C2_HOOK_FUNCTION(0x0052c7c0, DfNameReadText)

int C2_HOOK_STDCALL DfNameWriteText(br_datafile* df, char* name) {

    if (name == NULL) {
        name = "NULL";
    }
    BrFilePrintf(df->h, "  name \"%s\"\n", name);
    return 0;
}
C2_HOOK_FUNCTION(0x0052c840, DfNameWriteText)

int C2_HOOK_STDCALL DfNameSizeText(br_datafile* df, char* name) {

    return 1;
}
C2_HOOK_FUNCTION(0x0052c870, DfNameSizeText)

char* C2_HOOK_STDCALL DfNameReadBinary(br_datafile* df, char* name) {
    int c;
    int i;

    for (i = 0; i < 255; i++) {
        c = BrFileGetChar(df->h);
        if ((c == 0) || (c == -1)) {
            break;
        }
        name[i] = c;
    }
    name[i] = '\0';
    return name;
}
C2_HOOK_FUNCTION(0x0052c880, DfNameReadBinary)

int C2_HOOK_STDCALL DfNameWriteBinary(br_datafile* df, char* name) {

    if (name != NULL) {
        BrFileWrite(name, 1, BrStrLen(name), df->h);
    }
    BrFilePutChar('\0', df->h);
    return 0;
}
C2_HOOK_FUNCTION(0x0052c8c0, DfNameWriteBinary)

int C2_HOOK_STDCALL DfNameSizeBinary(br_datafile* df, char* name) {

    if (name != NULL) {
        return BrStrLen(name) + 1;
    }
    return 1;
}
C2_HOOK_FUNCTION(0x0052c900, DfNameSizeBinary)

int C2_HOOK_STDCALL DfSkipText(br_datafile* df, br_uint_32 length) {
    char* id;
    char* data;

    while ((BrFileEof(df->h) == 0) && (length != 0)) {
        TextReadLine(df, &data, &id);
        length--;
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0052c920, DfSkipText)

int C2_HOOK_STDCALL DfSkipBinary(br_datafile* df, br_uint_32 length) {
    BrFileAdvance(length, df->h);
    return 0;
}
C2_HOOK_FUNCTION(0x0052c970, DfSkipBinary)

int C2_HOOK_STDCALL DfChunksInterpret(br_datafile* df, br_chunks_table* table) {
    br_uint_32 length;
    br_uint_32 count;
    br_uint_32 id;
    int r;
    int i;

    while (1) {
        id = df->prims->chunk_read(df, &length);
        // LOG_DEBUG("chunk id=%d, len=%d", id, length);
        if (id == (br_uint_32)-1) {
            break;
        }
        for (i = 0; i < table->nentries; i++) {
            if (table->entries[i].id == id) {
                break;
            }
        }
        if (i >= table->nentries) {
            df->prims->skip(df, length);
        } else {
            if (table->entries[i].has_count != 0) {
                count = df->prims->count_read(df);
            } else {
                count = 0;
            }
            r = table->entries[i].handler(df, id, length, count);
            if (r != 0) {
                return r;
            }
        }
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0052c990, DfChunksInterpret)

void C2_HOOK_STDCALL BrNullOther(void) {

    // Yes, "invalid" is misspelled.
    BrFatal("datafile.c", 1825, "Invald file primitive call");
}
C2_HOOK_FUNCTION(0x0052ca30, BrNullOther)

int C2_HOOK_STDCALL DfFileIdentify(br_uint_8* magics, br_size_t n_magics) {
    static char text_magics[8] = { '*', 'F', 'I', 'L', 'E', '_', 'I', 'N' };
    static char binary_magics[8] = { '\0', '\0', '\0', '\x12', '\0', '\0', '\0', '\b' };

    if (BrMemCmp(magics, text_magics, sizeof(magics)) == 0) {
        return BR_FS_MODE_TEXT;
    }
    if (BrMemCmp(magics, binary_magics, sizeof(magics)) == 0) {
        return BR_FS_MODE_BINARY;
    }
    // file does not match magics
    return BR_FS_MODE_UNKNOWN;
}
C2_HOOK_FUNCTION(0x0052cb40, DfFileIdentify)

br_datafile* C2_HOOK_STDCALL DfOpen(char* name, int write, br_token scalar_type) {
    int mode;
    br_datafile* df;
    void* h;

    mode = C2V(fw).open_mode;
    if (write != 0) {
        h = BrFileOpenWrite(name, C2V(fw).open_mode);
    } else {
        h = BrFileOpenRead(name, 8u, DfFileIdentify, &mode);
    }
    if (h == NULL) {
        return NULL;
    }
    df = BrResAllocate(C2V(fw).res, sizeof(br_datafile), BR_MEMORY_DATAFILE);
    df->prims = &C2V(_BrFilePrimsNull);
    df->h = h;
    df->scalar_type = scalar_type;

    if (mode == BR_FS_MODE_BINARY) {
        if (write != 0) {
            df->prims = &C2V(_BrFilePrimsWriteBinary);
        } else {
            df->prims = &C2V(_BrFilePrimsReadBinary);
        }
    } else {
        if (write != 0)
            df->prims = &C2V(_BrFilePrimsWriteText);
        else
            df->prims = &C2V(_BrFilePrimsReadText);
    }
    DfPush(BR_MEMORY_FILE, df, 1); // Wrong enum -> should be DF_STACK_FILE (see comment in src/BRSRC13/CORE/V1DB/v1dbfile.c)
    return df;
}
C2_HOOK_FUNCTION(0x0052ca50, DfOpen)

void C2_HOOK_STDCALL DfClose(br_datafile* df) {
    br_datafile* dfp;

    while (1) {
        if (C2V(DatafileStackTop) > 0) {
            if (C2V(DatafileStack)[C2V(DatafileStackTop) - 1].type == BR_MEMORY_FILE) {
                break;
            }
            DfPop(C2V(DatafileStack)[C2V(DatafileStackTop) - 1].type, NULL);
        }
    }
    dfp = DfPop(BR_MEMORY_FILE, NULL);
    BrFileClose(dfp->h);
    BrResFree(dfp);
}
C2_HOOK_FUNCTION(0x0052cb80, DfClose)

int C2_HOOK_STDCALL BrWriteModeSet(int mode) {
    int old;

    old = C2V(fw).open_mode;
    C2V(fw).open_mode = mode;
    return old;
}
C2_HOOK_FUNCTION(0x0052cc50, BrWriteModeSet)
