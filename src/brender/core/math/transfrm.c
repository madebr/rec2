#include "transfrm.h"
#include <string.h>
#include <stdint.h>
#include "c2_math.h"

#include "angles.h"
#include "matrix34.h"
#include "matrix4.h"
#include "quat.h"
#include "vector.h"

#include "c2_stdlib.h"
#include "c2_string.h"

#define M34 BR_TRANSFORM_MATRIX34
#define MLP BR_TRANSFORM_MATRIX34_LP
#define QUT BR_TRANSFORM_QUAT
#define EUL BR_TRANSFORM_EULER
#define LUP BR_TRANSFORM_LOOK_UP
#define TRA BR_TRANSFORM_TRANSLATION
#define IDT BR_TRANSFORM_IDENTITY

// GLOBAL: CARMA2_HW 0x0066cb68
br_uint_8 _CombineTransforms[7][7] = {

    /* M34 */ {
        M34,
        M34,
        M34,
        M34,
        M34,
        M34,
        M34,
    },
    /* MLP */ {
        M34,
        MLP,
        MLP,
        MLP,
        MLP,
        MLP,
        MLP,
    },
    /* QUT */ {
        M34,
        MLP,
        MLP,
        MLP,
        MLP,
        QUT,
        QUT,
    },
    /* EUL */ {
        M34,
        MLP,
        MLP,
        MLP,
        MLP,
        EUL,
        EUL,
    },
    /* LUP */ {
        M34,
        MLP,
        MLP,
        MLP,
        MLP,
        LUP,
        LUP,
    },
    /* TRA */ {
        M34,
        MLP,
        QUT,
        EUL,
        LUP,
        TRA,
        TRA,
    },
    /* IDT */ {
        M34,
        MLP,
        QUT,
        EUL,
        LUP,
        TRA,
        IDT,
    },
};

#if defined(__GNUC__)
typedef br_uint_32 tActivation_transform_word __attribute__((__may_alias__));
#else
typedef br_uint_32 tActivation_transform_word;
#endif
#if defined(_MSC_VER) && _MSC_VER < 1300
#pragma auto_inline(off)
#endif
static br_uint_32 C2_HOOK_CDECL ActivationTransformWord(const void* base, unsigned offset) {
    return *(volatile const tActivation_transform_word*)((br_uint_32)(uintptr_t)base + offset);
}
static void C2_HOOK_CDECL ActivationTransformCopy(br_matrix34* mat, const br_transform* xform, unsigned output_word, unsigned input_word) {
    br_uint_32 value = ActivationTransformWord(xform, input_word * 4u);

    *(volatile tActivation_transform_word*)((br_uint_32)(uintptr_t)mat + output_word * 4u) = value;
}
static double C2_HOOK_CDECL ActivationTransformRead(const br_transform* xform, unsigned word) {
    return *(volatile const br_scalar*)((br_uint_32)(uintptr_t)xform + word * 4u);
}
static double C2_HOOK_CDECL ActivationTransformScalar(double value) {
    union { br_scalar scalar; br_uint_32 bits; } rounded;
    volatile br_uint_32 stored;

    rounded.scalar = (br_scalar)value;
    stored = rounded.bits;
    rounded.bits = stored;
    return rounded.scalar;
}
static double C2_HOOK_CDECL ActivationTransformNegativeZero(void) {
    union { br_scalar scalar; br_uint_32 bits; } value;
    volatile br_uint_32 stored = 0x80000000u;

    /* MSVC5 folds the literal -0.0 to positive zero. Original FCHS changes
     * both zero components of the fallback basis before their scalar stores. */
    value.bits = stored;
    return value.scalar;
}
#if defined(_MSC_VER) && _MSC_VER < 1300
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00531870
void C2_HOOK_CDECL BrTransformToMatrix34(br_matrix34* mat, const br_transform* xform) {
    volatile br_matrix34* output = mat;
    unsigned type = *(volatile const br_uint_16*)xform;
    unsigned word;
    double x, y, z, norm, reciprocal;
    double zx, zy, zz, ux_first, ux_second, uy_first, uy_second, uz_first, uz_second;
    double cross_x, cross_y, cross_z, round_x, round_y, round_z;
    double xx, xy, xz, yx, yy, yz;

    switch (type) {
    case BR_TRANSFORM_MATRIX34:
    case BR_TRANSFORM_MATRIX34_LP:
        for (word = 0; word < 12; word++) {
            ActivationTransformCopy(mat, xform, word, word + 1);
        }
        return;
    case BR_TRANSFORM_QUAT:
        BrQuatToMatrix34(mat, &xform->t.quat.q);
        ActivationTransformCopy(mat, xform, 9, 10);
        ActivationTransformCopy(mat, xform, 10, 11);
        ActivationTransformCopy(mat, xform, 11, 12);
        return;
    case BR_TRANSFORM_EULER:
        BrEulerToMatrix34(mat, &xform->t.euler.e);
        ActivationTransformCopy(mat, xform, 9, 10);
        ActivationTransformCopy(mat, xform, 10, 11);
        ActivationTransformCopy(mat, xform, 11, 12);
        return;
    case BR_TRANSFORM_TRANSLATION:
        /* Original three PUSHes capture raw Z/Y/X before any output store. */
        {
            br_uint_32 bx, by, bz;
            br_scalar tx, ty, tz;
            bz = ActivationTransformWord(xform, 48);
            by = ActivationTransformWord(xform, 44);
            bx = ActivationTransformWord(xform, 40);
            memcpy(&tx, &bx, 4); memcpy(&ty, &by, 4); memcpy(&tz, &bz, 4);
            BrMatrix34Translate(mat, tx, ty, tz);
        }
        return;
    case BR_TRANSFORM_LOOK_UP:
        x = ActivationTransformRead(xform, 1);
        y = ActivationTransformRead(xform, 2);
        z = ActivationTransformRead(xform, 3);
        norm = sqrt((x * ActivationTransformScalar(x) + y * y) + z * ActivationTransformScalar(z));
        if (norm > 2.384185791015625e-7) {
            reciprocal = 1.0 / norm;
            x = ActivationTransformRead(xform, 1);
            y = ActivationTransformRead(xform, 2);
            z = ActivationTransformRead(xform, 3);
            zx = ActivationTransformScalar(-(x * reciprocal));
            zy = ActivationTransformScalar(-(y * reciprocal));
            /* Original Z component is rounded before its sign change. */
            zz = ActivationTransformScalar(-ActivationTransformScalar(z * reciprocal));
        } else {
            zx = -1.0;
            zy = ActivationTransformNegativeZero();
            zz = ActivationTransformNegativeZero();
        }
        /* Retain the original first-fault address as well as the individual
         * loads before any public store. The duplicate operands serve different
         * cross products in the x87 stack, and are all loaded while input is live. */
        uy_first = ActivationTransformRead(xform, 5);
        uz_first = ActivationTransformRead(xform, 6);
        uz_second = ActivationTransformRead(xform, 6);
        ux_first = ActivationTransformRead(xform, 4);
        ux_second = ActivationTransformRead(xform, 4);
        uy_second = ActivationTransformRead(xform, 5);
        cross_x = uy_first * zz - uz_first * zy;
        round_x = ActivationTransformScalar(cross_x);
        cross_y = uz_second * zx - ux_first * zz;
        round_y = ActivationTransformScalar(cross_y);
        cross_z = ux_second * zy - uy_second * zx;
        round_z = ActivationTransformScalar(cross_z);
        norm = ActivationTransformScalar(sqrt((cross_x * round_x + cross_y * round_y) + cross_z * round_z));
        if (norm > 2.384185791015625e-7) {
            reciprocal = 1.0 / norm;
            xx = ActivationTransformScalar(reciprocal * round_x);
            xy = ActivationTransformScalar(reciprocal * round_y);
            xz = ActivationTransformScalar(reciprocal * round_z);
        } else {
            xx = 1.0; xy = 0.0; xz = 0.0;
        }
        yx = zy * xz - zz * xy;
        yy = zz * xx - zx * xz;
        yz = zx * xy - zy * xx;
        output->m[0][0] = (br_scalar)xx;
        output->m[1][1] = (br_scalar)yy;
        output->m[0][1] = (br_scalar)xy;
        output->m[0][2] = (br_scalar)xz;
        output->m[1][2] = (br_scalar)yz;
        output->m[1][0] = (br_scalar)yx;
        output->m[2][0] = (br_scalar)zx;
        output->m[2][1] = (br_scalar)zy;
        output->m[2][2] = (br_scalar)zz;
        ActivationTransformCopy(mat, xform, 9, 10);
        ActivationTransformCopy(mat, xform, 10, 11);
        ActivationTransformCopy(mat, xform, 11, 12);
        return;
    case BR_TRANSFORM_IDENTITY:
        BrMatrix34Identity(mat);
        return;
    }
}

// FUNCTION: CARMA2_HW 0x00531b60
void C2_HOOK_CDECL BrMatrix34PreTransform(br_matrix34* mat, const br_transform* xform) {
    br_matrix34 tmp;

    if (xform->type == BR_TRANSFORM_IDENTITY) {
        return;
    }

    BrTransformToMatrix34(&tmp, xform);
    BrMatrix34Pre(mat, &tmp);
}

// FUNCTION: CARMA2_HW 0x00531ba0
void C2_HOOK_CDECL BrMatrix34PostTransform(br_matrix34* mat, const br_transform* xform) {
    br_matrix34 tmp;

    if (xform->type == BR_TRANSFORM_IDENTITY) {
        return;
    }

    BrTransformToMatrix34(&tmp, xform);
    BrMatrix34Post(mat, &tmp);
}

// FUNCTION: CARMA2_HW 0x00531be0
void C2_HOOK_CDECL BrMatrix4PreTransform(br_matrix4* mat, const br_transform* xform) {
    br_matrix34 tmp;

    if (xform->type != BR_TRANSFORM_IDENTITY) {
        BrTransformToMatrix34(&tmp, xform);
        BrMatrix4Pre34(mat, &tmp);
    }
}

// FUNCTION: CARMA2_HW 0x00531c20
void C2_HOOK_CDECL BrMatrix34ToTransform(br_transform* xform, const br_matrix34* mat) {

    switch (xform->type) {
    case BR_TRANSFORM_MATRIX34:
        memcpy(&xform->t.mat, mat, sizeof(br_matrix34));
        break;
    case BR_TRANSFORM_MATRIX34_LP:
        memcpy(&xform->t.mat, mat, sizeof(br_matrix34));
        BrMatrix34LPNormalise(&xform->t.mat, &xform->t.mat);
        break;
    case BR_TRANSFORM_QUAT:
        BrMatrix34ToQuat(&xform->t.quat.q, mat);
        BrVector3Copy(&xform->t.quat.t, (br_vector3*)mat->m[3]);
        break;
    case BR_TRANSFORM_EULER:
        BrMatrix34ToEuler(&xform->t.euler.e, mat);
        BrVector3Copy(&xform->t.quat.t, (br_vector3*)mat->m[3]);
        break;
    case BR_TRANSFORM_LOOK_UP:
        BrVector3Negate(&xform->t.look_up.look, (br_vector3*)mat->m[2]);
        if (xform->t.look_up.up.v[0] == 0.f && xform->t.look_up.up.v[1] == 0.f && xform->t.look_up.up.v[2] == 0.f) {
            BrVector3Set(&xform->t.look_up.up, 0.f, 1.f, 0.f);
        }
        BrVector3Copy(&xform->t.look_up.t, (br_vector3*)mat->m[3]);
        break;
    case BR_TRANSFORM_TRANSLATION:
        BrVector3Copy(&xform->t.quat.t, (br_vector3*)mat->m[3]);
        break;
    }
}

// FUNCTION: CARMA2_HW 0x00531d70
void C2_HOOK_CDECL BrTransformToTransform(br_transform* dest, const br_transform* src) {
    br_matrix34 temp;

    if (src->type == dest->type) {
        memcpy(dest, src, sizeof(br_transform));
        return;
    }
    switch (dest->type) {
    case BR_TRANSFORM_MATRIX34:
        BrTransformToMatrix34(&dest->t.mat, src);
        break;
    case BR_TRANSFORM_MATRIX34_LP:
        BrTransformToMatrix34(&dest->t.mat, src);
        BrMatrix34LPNormalise(&dest->t.mat, &dest->t.mat);
        break;
    default:
        BrTransformToMatrix34(&temp, src);
        BrMatrix34ToTransform(dest, &temp);
    }
}
