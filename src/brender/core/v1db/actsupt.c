#include "actsupt.h"

#include "dbsetup.h"
#include "enables.h"

#include "core/fw/brlists.h"
#include "core/fw/diag.h"
#include "core/fw/fwsetup.h"
#include "core/fw/pattern.h"
#include "core/fw/resource.h"

#include "core/math/matrix34.h"
#include "core/math/matrix4.h"
#include "core/math/transfrm.h"

br_uint_32 C2_HOOK_CDECL BrActorEnum(br_actor* parent, br_actor_enum_cbfn* callback, void* arg) {
    br_actor* a;
    br_actor* next;
    br_uint_32 r;

    a = parent->children;

    while (a != NULL) {
        next = a->next;
        r = callback(a, arg);
        if (r != 0) {
            return r;
        }
        a = next;
    }

    return 0;
}
C2_HOOK_FUNCTION(0x0051ded0, BrActorEnum)

br_uint_32 C2_HOOK_CDECL BrActorSearchMany(br_actor* root, char* pattern, br_actor** actors, int max) {
    br_actor* a;
    char* sub;
    int n;
    int remaining;

    sub = NULL;
    if (pattern != NULL) {
        while (*pattern == '/') {
            pattern++;
        }
        sub = pattern;
        while (*sub != '\0' && *sub != '/') {
            sub++;
        }
        while (*sub == '/') {
            sub++;
        }
    }
    a = root->children;
    remaining = max;
    while (a != NULL && remaining > 0) {
        if (BrNamePatternMatch(pattern, a->identifier) != 0) {
            if (sub == NULL || *sub == '\0') {
                *actors = a;
                actors++;
                remaining--;
            } else {
                n = BrActorSearchMany(a, sub, actors, remaining);
                actors += n;
                remaining -= n;
            }
        }
        a = a->next;
    }
    return max - remaining;
}
C2_HOOK_FUNCTION(0x0051df00, BrActorSearchMany)

br_actor* C2_HOOK_CDECL BrActorSearch(br_actor* root, char* pattern) {
    br_actor* a;

    if (BrActorSearchMany(root, pattern, &a, 1) == 1) {
        return a;
    }
    return NULL;
}
C2_HOOK_FUNCTION(0x0051dfb0, BrActorSearch)

void C2_HOOK_STDCALL RenumberActor(br_actor* a, int d) {
    br_actor* ac;

    a->depth = d;
    ac = a->children;
    while (ac != NULL) {
        RenumberActor(ac, d + 1);
        ac = ac->next;
    }
}
C2_HOOK_FUNCTION(0x0051e040, RenumberActor)

br_actor* C2_HOOK_CDECL BrActorAdd(br_actor* parent, br_actor* a) {
    br_actor* ac;

    BrSimpleAddHead((br_simple_list*)&parent->children, (br_simple_node*)a);
    a->parent = parent;

    a->depth = parent->depth + 1;
    for (ac = a->children; ac != NULL; ac = ac->next) {
        RenumberActor(ac, a->depth + 1);
    }
    return a;
}
C2_HOOK_FUNCTION(0x0051dfe0, BrActorAdd)

br_actor* C2_HOOK_CDECL BrActorRemove(br_actor* a) {
    br_actor* ac;

    BrSimpleRemove((br_simple_node*)a);
    a->parent = NULL;
    a->depth = 0;

    for (ac = a->children; ac != NULL; ac = ac->next) {
        RenumberActor(ac, 1);
    }
    return a;
}
C2_HOOK_FUNCTION(0x0051e070, BrActorRemove)

void C2_HOOK_CDECL BrActorRelink(br_actor* parent, br_actor* a) {
    br_matrix34 mat;

    if (a->parent == parent) {
        return;
    }

    BrActorToActorMatrix34(&mat, a, parent);
    BrMatrix34ToTransform(&a->t, &mat);
    BrActorAdd(parent, BrActorRemove(a));
}
C2_HOOK_FUNCTION(0x0051e0d0, BrActorRelink)

br_actor* C2_HOOK_CDECL BrActorAllocate(br_uint_8 type, void* type_data) {
    br_actor* a;
    br_light* light;
    br_camera* camera;
    br_bounds* bounds;
    br_vector4* clip_plane;

    C2_HOOK_BUG_ON(sizeof(br_actor) != 100);
    a = BrResAllocate(C2V(v1db).res, sizeof(br_actor), BR_MEMORY_ACTOR);
    BrSimpleNewList((br_simple_list*)&a->children);
    a->type = type;
    a->depth = 0;
    a->t.type = BR_TRANSFORM_MATRIX34;
    a->type_data = NULL;
    BrMatrix34Identity(&a->t.t.mat);

    if (type_data != NULL) {
        a->type_data = type_data;
    } else {
        switch (type) {
        case BR_ACTOR_NONE:
            break;
        case BR_ACTOR_LIGHT:
            C2_HOOK_BUG_ON(sizeof(br_light) != 32);
            light = BrResAllocate(a, sizeof(br_light), BR_MEMORY_LIGHT);
            a->type_data = light;
            light->type = BR_LIGHT_DIRECT;
            light->colour = BR_COLOUR_RGB(255, 255, 255);
            light->attenuation_c = 1.0f;
            light->cone_outer = BR_ANGLE_DEG(15);
            light->cone_inner = BR_ANGLE_DEG(10);
            break;
        case BR_ACTOR_CAMERA:
            C2_HOOK_BUG_ON(sizeof(br_camera) != 36);
            camera = (br_camera*)BrResAllocate(a, sizeof(br_camera), BR_MEMORY_CAMERA);
            a->type_data = camera;
            camera->type = BR_CAMERA_PERSPECTIVE_FOV;
            camera->field_of_view = BR_ANGLE_DEG(45.0f);
            camera->hither_z = 1.0f;
            camera->yon_z = 10.0f;
            camera->aspect = 1.0f;
            break;
        case BR_ACTOR_BOUNDS:
        case BR_ACTOR_BOUNDS_CORRECT:
            C2_HOOK_BUG_ON(sizeof(br_bounds) != 24);
            bounds = BrResAllocate(a, sizeof(br_bounds), BR_MEMORY_CLIP_PLANE);
            a->type_data = bounds;
            break;
        case BR_ACTOR_CLIP_PLANE:
            clip_plane = BrResAllocate(a, sizeof(br_vector4), BR_MEMORY_CLIP_PLANE);
            clip_plane->v[0] = 0.f;
            clip_plane->v[1] = 0.f;
            clip_plane->v[2] = 1.f;
            clip_plane->v[3] = 0.f;
            a->type_data = clip_plane;
            break;
        case BR_ACTOR_MODEL:
            // nothing to do
            break;
        default:
            // FIXME: LOG_WARN("Warning: Unknown type %d for BrActorAllocate", type);
            break;
        }
    }
    return a;
}
C2_HOOK_FUNCTION(0x0051e1a0, BrActorAllocate)

void C2_HOOK_STDCALL InternalActorFree(br_actor* a) {

    while (a->children != NULL) {
        InternalActorFree((br_actor*)BrSimpleRemove((br_simple_node*)a->children));
    }
    BrActorEnableCheck(a);
    BrResFree(a);
}
C2_HOOK_FUNCTION(0x0051e300, InternalActorFree)

void C2_HOOK_CDECL BrActorFree(br_actor* a) {

    InternalActorFree(a);
}
C2_HOOK_FUNCTION(0x0051e2c0, BrActorFree)

br_boolean C2_HOOK_STDCALL ActorToRoot(br_actor* a, br_actor* world, br_matrix34* m) {
    if (a == world) {
        BrMatrix34Identity(m);
        return 1;
    } else {
        BrTransformToMatrix34(m, &a->t);
        for (a = a->parent; a != NULL && a != world; a = a->parent) {
            if (a->t.type != BR_TRANSFORM_IDENTITY) {
                BrMatrix34PostTransform(m, &a->t);
            }
        }
        return a == world;
    }
}
C2_HOOK_FUNCTION(0x0051e340, ActorToRoot)

br_boolean (C2_HOOK_STDCALL * ActorToRootTyped_original)(br_actor* a, br_actor* world, br_matrix34* m, br_int_32* type);
br_boolean C2_HOOK_STDCALL ActorToRootTyped(br_actor* a, br_actor* world, br_matrix34* m, br_int_32* type) {
#if 0//defined(C2_HOOKS_ENABLED)
    return ActorToRootTyped_original(a, world, m, type);
#else
    br_int_32 t;

	if (a == world) {
		BrMatrix34Identity(m);
		*type = BR_TRANSFORM_IDENTITY;
		return 1;
	}
    BrTransformToMatrix34(m, &a->t);
	t = a->t.type;
	for (a = a->parent; a != NULL && a != world; a = a->parent) {
		if (a->t.type != BR_TRANSFORM_IDENTITY) {
            BrTransformToMatrix34(m, &a->t);
			t = BrTransformCombineTypes(t, a->t.type);
		}
	}
	*type = t;
	return a == world;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0051e3b0, ActorToRootTyped, ActorToRootTyped_original)

void (C2_HOOK_STDCALL * Matrix4PerspectiveNew_original)(br_matrix4* mat, br_angle field_of_view, br_scalar aspect, br_scalar hither, br_scalar yon, br_scalar origin_x, br_scalar origin_y);
void C2_HOOK_STDCALL Matrix4PerspectiveNew(br_matrix4* mat, br_angle field_of_view, br_scalar aspect, br_scalar hither, br_scalar yon, br_scalar origin_x, br_scalar origin_y) {
#if 0//defined(C2_HOOKS_ENABLED)
    Matrix4PerspectiveNew_original(mat, field_of_view, aspect, hither, yon, origin_x, origin_y);
#else
    br_scalar scale;

    scale = BR_COS(field_of_view / 2) / BR_SIN(field_of_view / 2);

    mat->m[0][0] = scale / aspect;
    mat->m[0][1] = 0.f;
    mat->m[0][2] = 0.f;
    mat->m[0][3] = 0.f;
    mat->m[1][0] = 0.f;
    mat->m[1][1] = scale;
    mat->m[1][2] = 0.f;
    mat->m[1][3] = 0.f;
    mat->m[2][0] = -origin_x;
    mat->m[2][1] = -origin_y;
    mat->m[2][2] = (yon + hither) / (yon - hither);
    mat->m[2][3] = -1.f;
    mat->m[3][0] = 0.f;
    mat->m[3][1] = 0.f;
    mat->m[3][2] = -2.f * (yon * hither) / (yon - hither);
    mat->m[3][3] = 0.f;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0051e450, Matrix4PerspectiveNew, Matrix4PerspectiveNew_original)

br_token (C2_HOOK_STDCALL * CameraToScreenMatrix4_original)(br_matrix4* mat, br_actor* camera);
br_token C2_HOOK_STDCALL CameraToScreenMatrix4(br_matrix4* mat, br_actor* camera) {
#if 0//defined(C2_HOOKS_ENABLED)
    return CameraToScreenMatrix4_original(mat, camera);
#else
    br_camera* camera_type;
    br_matrix34 mat34;

    camera_type = camera->type_data;
	switch (camera_type->type) {
	case BR_CAMERA_PARALLEL:
		if (camera_type->width == 0.f) {
			BrFailure("Parallel camera has zero width");
        }
		if (camera_type->height == 0.f) {
			BrFailure("Parallel camera has zero height");
        }
		if (camera_type->yon_z <= camera_type->hither_z) {
			BrFailure("Parallel camera has invalid yon and hither");
        }

		BrMatrix34Scale(&mat34,
            2.f / (camera_type->width * camera_type->aspect),
            2.f / camera_type->height,
            2.f / (camera_type->yon_z - camera_type->hither_z));
		BrMatrix34PreTranslate(&mat34,
            0.f,
            0.f,
            (camera_type->hither_z + camera_type->yon_z) / 2.f);
		BrMatrix34PostTranslate(&mat34,
            C2V(v1db).origin.v[0],
            C2V(v1db).origin.v[1],
            0.f);
		BrMatrix4Copy34(mat, &mat34);
		return BRT_PARALLEL;
	case BR_CAMERA_PERSPECTIVE_FOV:
		Matrix4PerspectiveNew(mat,
			camera_type->field_of_view,
            camera_type->aspect,
			-camera_type->hither_z,
            -camera_type->yon_z,
			C2V(v1db).origin.v[0],
            C2V(v1db).origin.v[1]);
		return BRT_PERSPECTIVE;
    case BR_CAMERA_PERSPECTIVE_WHD:
        return BRT_PERSPECTIVE;
	case BR_CAMERA_PARALLEL_OLD:
		if (camera_type->width == 0.f) {
            BrFailure("Parallel camera has zero width");
        }
        if (camera_type->height == 0.f) {
            BrFailure("Parallel camera has zero height");
        }
		if (camera_type->yon_z <= camera_type->hither_z) {
            BrFailure("Parallel camera has invalid yon and hither");
        }
		BrMatrix34Scale(&mat34,
			1.f / camera_type->width,
			1.f / camera_type->height,
			1.f / (camera_type->yon_z - camera_type->hither_z));
		BrMatrix34PreTranslate(&mat34,
            0.f,
            0.f,
            camera_type->hither_z);
		BrMatrix4Copy34(mat,&mat34);
		return BRT_PARALLEL;
	}
	return BR_NULL_TOKEN;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0051e520, CameraToScreenMatrix4, CameraToScreenMatrix4_original)

br_uint_16 C2_HOOK_CDECL BrActorToActorMatrix34(br_matrix34* m, br_actor* a, br_actor* b) {
    br_matrix34 mata;
    br_matrix34 matb;
    br_matrix34 matc;
    br_uint_8 at;
    br_uint_8 bt;

    if (a == b) {
        BrMatrix34Identity(m);
        return BR_TRANSFORM_IDENTITY;
    }

    if (a->parent == b) {
        BrTransformToMatrix34(m, &a->t);
        return a->t.type;
    }

    if (b->parent == a) {
        BrTransformToMatrix34(&matb, &b->t);

        if (BrTransformTypeIsLP(b->t.type))
            BrMatrix34LPInverse(m, &matb);
        else
            BrMatrix34Inverse(m, &matb);

        return b->t.type;
    }

    at = BR_TRANSFORM_IDENTITY;
    BrMatrix34Identity(&mata);

    bt = BR_TRANSFORM_IDENTITY;
    BrMatrix34Identity(&matb);

    while (a && b && a != b) {

        if (a->depth > b->depth) {

            if (a->t.type != BR_TRANSFORM_IDENTITY) {
                BrMatrix34PostTransform(&mata, &a->t);
                at = BrTransformCombineTypes(at, a->t.type);
            }
            a = a->parent;

        } else if (b->depth > a->depth) {

            if (b->t.type != BR_TRANSFORM_IDENTITY) {
                BrMatrix34PostTransform(&matb, &b->t);
                bt = BrTransformCombineTypes(bt, b->t.type);
            }
            b = b->parent;

        } else {

            if (a->t.type != BR_TRANSFORM_IDENTITY) {
                BrMatrix34PostTransform(&mata, &a->t);
                at = BrTransformCombineTypes(at, a->t.type);
            }
            if (b->t.type != BR_TRANSFORM_IDENTITY) {
                BrMatrix34PostTransform(&matb, &b->t);
                bt = BrTransformCombineTypes(bt, b->t.type);
            }
            b = b->parent;
            a = a->parent;
        }
    }

    if (bt == BR_TRANSFORM_IDENTITY) {
        BrMatrix34Copy(m, &mata);
        return at;
    }

    if (at == BR_TRANSFORM_IDENTITY) {
        if (BrTransformTypeIsLP(bt)) {
            BrMatrix34LPInverse(m, &matb);
        } else
            BrMatrix34Inverse(m, &matb);

        return bt;
    }

    if (BrTransformTypeIsLP(bt)) {
        BrMatrix34LPInverse(&matc, &matb);
    } else
        BrMatrix34Inverse(&matc, &matb);

    BrMatrix34Mul(m, &mata, &matc);

    return BrTransformCombineTypes(at, bt);
}
C2_HOOK_FUNCTION(0x0051e7d0, BrActorToActorMatrix34)

void (C2_HOOK_CDECL * BrActorToScreenMatrix4_original)(br_matrix4* m, br_actor* a, br_actor* camera);
void C2_HOOK_CDECL BrActorToScreenMatrix4(br_matrix4* m, br_actor* a, br_actor* camera) {
#if 0//defined(C2_HOOKS_ENABLED)
    BrActorToScreenMatrix4_original(m, a, camera);
#else
    br_matrix34 a2c;

    CameraToScreenMatrix4(m, camera);
    if (a != camera) {
        BrActorToActorMatrix34(&a2c, a, camera);
        BrMatrix4Pre34(m, &a2c);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0051eae0, BrActorToScreenMatrix4, BrActorToScreenMatrix4_original)

void BrMatrix34ApplyBounds(br_bounds* A, br_bounds* B, br_matrix34* C) {
    int i;
    int j;
    br_scalar a;
    br_scalar b;

    A->min.v[0] = A->max.v[0] = C->m[3][0];
    A->min.v[1] = A->max.v[1] = C->m[3][1];
    A->min.v[2] = A->max.v[2] = C->m[3][2];

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            a = C->m[j][i] * B->min.v[j];
            b = C->m[j][i] * B->max.v[j];
            if (a < b) {
                A->min.v[i] += a;
                A->max.v[i] += b;
            } else {
                A->min.v[i] += b;
                A->max.v[i] += a;
            }
        }
    }
}

void C2_HOOK_STDCALL ActorToBounds(br_bounds* dest, br_actor* ap, br_model* model) {
    br_actor* a;
    br_bounds new;
    br_matrix34 m2v;
    int i;

    if (ap->model != NULL) {
        model = ap->model;
    }

    m2v = C2V(v1db).model_to_view;
    BrMatrix34PreTransform(&C2V(v1db).model_to_view, &ap->t);

    if (ap->type == BR_ACTOR_MODEL) {
        BrMatrix34ApplyBounds(&new, &model->bounds, &C2V(v1db).model_to_view);

        for (i = 0; i < 3; i++) {
            if (new.min.v[i] < dest->min.v[i]) {
                dest->min.v[i] = new.min.v[i];
            }

            if (new.max.v[i] > dest->max.v[i]) {
                dest->max.v[i] = new.max.v[i];
            }
        }
    }

    for(a = ap->children; a != NULL; a = a->next) {
        ActorToBounds(dest, a, model);
    }

    C2V(v1db).model_to_view = m2v;
}
C2_HOOK_FUNCTION(0x0051ebc0, ActorToBounds)

br_bounds* C2_HOOK_CDECL BrActorToBounds(br_bounds* b, br_actor* ap) {
    br_matrix34 m2v;
    br_model* model;
    br_actor* a;

    m2v = C2V(v1db).model_to_view;

    if (ap->model == NULL) {
        model = C2V(v1db).default_model;
    } else {
        model = ap->model;
    }

    BrMatrix34Identity(&C2V(v1db).model_to_view);

    if (ap->type == BR_ACTOR_MODEL) {
        *b = model->bounds;
    } else {
        b->min.v[0] = b->min.v[1] = b->min.v[2] = BR_SCALAR_MAX;
        b->max.v[0] = b->max.v[1] = b->max.v[2] = BR_SCALAR_MIN;
    }

    for (a = ap->children; a != NULL; a = a->next) {
        ActorToBounds(b, a, model);
    }

    C2V(v1db).model_to_view = m2v;
    return b;
}
C2_HOOK_FUNCTION(0x0051eb20, BrActorToBounds)
