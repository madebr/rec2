#include "custsupt.h"

#include "dbsetup.h"

#include "core/math/fixed.h"
#include "core/math/matrix4.h"

// Carmageddon 2 adaptation

// GLOBAL: CARMA2_HW 0x0079feb4
br_scalar gScreenZOffset;

// GLOBAL: CARMA2_HW 0x00670530
const float gScreenZOffsetPresetChoices[7] = {
     0.0f,
    -1.5f,
    -3.0f,
    -4.5f,
    -6.0f,
    -7.5f,
    -9.0f,
};

// FUNCTION: CARMA2_HW 0x00540560
void C2_HOOK_CDECL BrSetScreenZOffset(br_uint_32 pOffset) {

    C2_HOOK_BUG_ON(BR_ASIZE(gScreenZOffsetPresetChoices) != 7);

    if (pOffset < BR_ASIZE(gScreenZOffsetPresetChoices)) {
        gScreenZOffset = gScreenZOffsetPresetChoices[pOffset];
    } else {
        gScreenZOffset = 0.f;
    }
}

static br_uint_32 calculate_outcode(const br_vector4* v) {
    br_uint_32 outcode;

    outcode = 0xfff0000;
    if (v->v[0] >= v->v[3]) {
        outcode ^= 0x00020002;
    }
    if (v->v[0] < -v->v[3]) {
        outcode ^= 0x00010001;
    }
    if (v->v[1] >=  v->v[3]) {
        outcode ^= 0x00040004;
    }
    if (v->v[1] < -v->v[3]) {
        outcode ^= 0x00080008;
    }
    if (v->v[2] >= v->v[3]) {
        outcode ^= 0x00100010;
    }
    if (v->v[2] < -v->v[3]) {
        outcode ^= 0x00200020;
    }
    return outcode;
}

// FUNCTION: CARMA2_HW 0x005269f0
void C2_HOOK_CDECL BrModelToScreenQuery(br_matrix4* dest) {
    br_uint_32 dummy;
    br_matrix4 v2s;
    br_matrix34 m2v;

    v1db.renderer->dispatch->_partQueryBuffer(v1db.renderer, BRT_MATRIX, 0, &dummy, (br_uint_32*)&v2s, sizeof(v2s), BRT_VIEW_TO_SCREEN_M4_F);
    v1db.renderer->dispatch->_partQueryBuffer(v1db.renderer, BRT_MATRIX, 0, &dummy, (br_uint_32*)&m2v, sizeof(m2v), BRT_MODEL_TO_VIEW_M34_F);
    BrMatrix4Mul34(dest, &m2v, &v2s);
}

// FUNCTION: CARMA2_HW 0x00526a60
void C2_HOOK_CDECL BrModelToViewQuery(br_matrix34* dest) {
    br_uint_32 dummy;

    v1db.renderer->dispatch->_partQueryBuffer(v1db.renderer, BRT_MATRIX, 0, &dummy, (br_uint_32*)dest, sizeof(*dest), BRT_MODEL_TO_VIEW_M34_F);
}

// FUNCTION: CARMA2_HW 0x00526a90
br_uint_8 C2_HOOK_CDECL BrOriginToScreenXY(br_vector2* screen) {

    if (!v1db.model_to_screen_valid) {
        BrModelToScreenQuery(&v1db.model_to_screen);
        v1db.model_to_screen_valid = 1;
    }

    screen->v[0] = v1db.vp_ox + v1db.vp_width * v1db.model_to_screen.m[3][0] / v1db.model_to_screen.m[3][3];
    screen->v[1] = v1db.vp_oy + v1db.vp_height * v1db.model_to_screen.m[3][1] / v1db.model_to_screen.m[3][3];

    return v1db.model_to_screen.m[3][2] > 0.0f;
}

// FUNCTION: CARMA2_HW 0x00526b70
br_uint_32 C2_HOOK_CDECL BrOriginToScreenXYZO(br_vector3* screen) {
    br_uint_32 outcode;

    if (!v1db.model_to_screen_valid) {
        BrModelToScreenQuery(&v1db.model_to_screen);
        v1db.model_to_screen_valid = 1;
    }

    outcode = calculate_outcode((br_vector4*)v1db.model_to_screen.m[3]);

    if ((outcode & 0x00000fff) == 0) {
        screen->v[0] = v1db.vp_ox + v1db.vp_width * v1db.model_to_screen.m[3][0] / v1db.model_to_screen.m[3][3];
        screen->v[1] = v1db.vp_oy + v1db.vp_height * v1db.model_to_screen.m[3][1] / v1db.model_to_screen.m[3][3];
        screen->v[2] = gScreenZOffset + v1db.model_to_screen.m[3][3] / v1db.model_to_screen.m[3][2] * 32767.f;
    }
    return outcode;
}

// FUNCTION: CARMA2_HW 0x00526cf0
br_uint_8 C2_HOOK_CDECL BrPointToScreenXY(br_vector2* screen, const br_vector3* point) {
    br_vector4 sp;

    if (!v1db.model_to_screen_valid) {
        BrModelToScreenQuery(&v1db.model_to_screen);
        v1db.model_to_screen_valid = 1;
    }
    BrMatrix4ApplyP(&sp, point, &v1db.model_to_screen);
    screen->v[0] = v1db.vp_ox + sp.v[0] * v1db.vp_width / sp.v[3];
    screen->v[1] = v1db.vp_oy + sp.v[1] * v1db.vp_height / sp.v[3];

    return sp.v[2] > 0.f;
}

// FUNCTION: CARMA2_HW 0x00526df0
br_uint_32 C2_HOOK_CDECL BrPointToScreenXYZO(br_vector3* screen, const br_vector3* point) {
    br_vector4 sp;
    br_uint_32 outcode;

    if (!v1db.model_to_screen_valid) {
        BrModelToScreenQuery(&v1db.model_to_screen);
        v1db.model_to_screen_valid = 1;
    }
    BrMatrix4ApplyP(&sp, point, &v1db.model_to_screen);
    outcode = calculate_outcode(&sp);

    if ((outcode & 0x00000fff) == 0) {
        screen->v[0] = v1db.vp_ox + v1db.vp_width * v1db.model_to_screen.m[3][0] / v1db.model_to_screen.m[3][3];
        screen->v[1] = v1db.vp_oy + v1db.vp_height * v1db.model_to_screen.m[3][1] / v1db.model_to_screen.m[3][3];
        screen->v[2] = gScreenZOffset + v1db.model_to_screen.m[3][3] / v1db.model_to_screen.m[3][2] * 32767.f;
    }
    return outcode;
}

// FUNCTION: CARMA2_HW 0x00526f80
void C2_HOOK_CDECL BrPointToScreenXYMany(br_vector2* screens, const br_vector3* points, br_uint_32 npoints) {
    br_vector4 sp;
    br_uint_32 i;

    if (!v1db.model_to_screen_valid) {
        BrModelToScreenQuery(&v1db.model_to_screen);
        v1db.model_to_screen_valid = 1;
    }

    for (i = 0; i < npoints; i++) {
        BrMatrix4ApplyP(&sp, &points[i], &v1db.model_to_screen);

        screens[i].v[0] = v1db.vp_ox + sp.v[0] * v1db.vp_width / sp.v[3];
        screens[i].v[1] = v1db.vp_oy + sp.v[1] * v1db.vp_height / sp.v[3];
    }
}

// FUNCTION: CARMA2_HW 0x00527070
void C2_HOOK_CDECL BrPointToScreenXYZOMany(br_vector3* screens, br_uint_32* outcodes, br_vector3* points, br_uint_32 npoints) {
    br_uint_32 outcode;
    br_vector4 sp;
    br_uint_32 i;

    if (!v1db.model_to_screen_valid) {
        BrModelToScreenQuery(&v1db.model_to_screen);
        v1db.model_to_screen_valid = 1;
    }

    for (i = 0; i < npoints; i++) {
        BrMatrix4ApplyP(&sp, &points[i], &v1db.model_to_screen);
        outcode = calculate_outcode(&sp);

        if ((outcode & 0x00000fff) == 0) {
            screens[i].v[0] = v1db.vp_ox + v1db.vp_width * v1db.model_to_screen.m[3][0] / v1db.model_to_screen.m[3][3];
            screens[i].v[1] = v1db.vp_oy + v1db.vp_height * v1db.model_to_screen.m[3][1] / v1db.model_to_screen.m[3][3];
            screens[i].v[2] = gScreenZOffset + v1db.model_to_screen.m[3][3] / v1db.model_to_screen.m[3][2] * 32767.f;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00527240
br_scalar C2_HOOK_CDECL BrZbDepthToScreenZ(br_uint_32 depth_z, br_camera* camera) {

    return BrFixedToScalar(BrFixedNeg(depth_z));
}

// FUNCTION: CARMA2_HW 0x00527260
br_uint_32 C2_HOOK_CDECL BrZbScreenZToDepth(br_scalar sz, br_camera* camera) {
    br_uint_32 depth;

    depth = BrScalarToFixed(sz) ^ 0x80000000;
    if (sz <= -32768.0f) {
        return 0;
    } else if (sz >= 32768.0f) {
        return -1;
    } else {
        return depth;
    }
}

// FUNCTION: CARMA2_HW 0x005272b0
br_scalar C2_HOOK_CDECL BrZsDepthToScreenZ(br_scalar depth_z, br_camera* camera) {
    br_scalar hither;
    br_scalar yon;

    hither = camera->hither_z;
    yon = camera->yon_z;

    if (depth_z <= hither) {
        return -32768.0f;
    }
    if (depth_z >= yon) {
        return 32768.0f;
    }
    return 2 * ((2 * depth_z - yon - hither) * 16384.0f / (yon - hither));
}

// FUNCTION: CARMA2_HW 0x00527320
br_scalar C2_HOOK_CDECL BrZsScreenZToDepth(br_scalar sz, br_camera* camera) {
    br_scalar hither;
    br_scalar yon;
    br_scalar depth;

    hither = camera->hither_z;
    yon = camera->yon_z;
    depth = ((yon + hither) + (sz / 2.0f) * (yon - hither) / 16384.0f) / 2.0f;
    if (sz <= -32768.0f) {
        return hither;
    } else if (sz >= 32768.0f) {
        return yon;
    }
    return depth;
}

// FUNCTION: CARMA2_HW 0x005273a0
br_scalar C2_HOOK_CDECL BrScreenZToCamera(br_actor* camera, br_scalar sz) {
    br_camera* data;
    br_scalar hither;
    br_scalar yon;

    data = camera->type_data;
    yon = data->yon_z;
    hither = data->hither_z;
    if (sz <= -32768.0f) {
        return -hither;
    }
    if (sz >= 32768.0f) {
        return -yon;
    }
    switch (data->type) {
    case BR_CAMERA_PARALLEL:
        return 0.5f * sz * (hither - yon) * 0.5f / 16384.0f - (yon + hither) * 0.5f;
    case BR_CAMERA_PERSPECTIVE_FOV:
        return 2.0f * ((yon * hither) / (0.5f * sz * (yon - hither) / 16384.0f - (yon + hither)));
    default:
        return 0.f;
    }
}

// FUNCTION: CARMA2_HW 0x00527480
void C2_HOOK_CDECL BrScreenXYZToCamera(br_vector3* point, br_actor* camera, br_pixelmap* screen_buffer, br_int_16 x, br_int_16 y, br_scalar sz) {
    br_scalar hx;
    br_scalar hy;
    br_scalar vz;
    br_camera* data;
    br_angle fov;
    br_scalar scale;


    data = camera->type_data;

    vz = BrScreenZToCamera(camera, sz);

    hx = (float)(screen_buffer->width - 2 * (x + screen_buffer->origin_x)) / (float)screen_buffer->width;
    hy = (float)(2 * (y + screen_buffer->origin_y) - screen_buffer->height) / (float)screen_buffer->height;

    switch (data->type) {
    case BR_CAMERA_PARALLEL:
        point->v[0] = -0.5f * hx * data->width * data->aspect;
        point->v[1] = -0.5f * hy * data->height / 2;
        point->v[2] = vz;
        break;
    case BR_CAMERA_PERSPECTIVE_FOV:
        fov=(br_angle)(data->field_of_view / 2);
        scale = BR_COS(fov) / BR_SIN(fov);
        point->v[0] = hx * data->aspect * vz / scale;
        point->v[1] = hy * vz / scale;
        point->v[2] = vz;
        break;
    default:
        break;
    }
}
