#include "pedestrn.h"

#include "animation.h"
#include "errors.h"
#include "globvars.h"
#include "graphics.h"
#include "loading.h"
#include "skidmark.h"
#include "smashing.h"
#include "utility.h"
#include "world.h"

#include "rec2_macros.h"
#include "rec2_types.h"

C2_HOOK_VARIABLE_IMPLEMENT(tExplosion_animation, gPed_explosion_small_blood_cloud, 0x00694478);
C2_HOOK_VARIABLE_IMPLEMENT(tExplosion_animation, gPed_medium_blood_cloud, 0x00694480);
C2_HOOK_VARIABLE_IMPLEMENT(tExplosion_animation, gPed_large_blood_cloud, 0x00694488);
C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gPed_slick_material, 0x0069bc10);
C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gPed_smear_material, 0x00694460);
C2_HOOK_VARIABLE_IMPLEMENT(tSlick_spec, gPed_slick, 0x00694238);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPed_spurty_lumps_count, 0x0069427c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPed_spurting_period, 0x0069445c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPed_dismember_spurting_member_count, 0x00694454);
C2_HOOK_VARIABLE_IMPLEMENT(tSmashable_initial_position_spec, gPed_spurt_initial_position, 0x0069fda8);
C2_HOOK_VARIABLE_IMPLEMENT(tSmashable_initial_speed_spec, gPed_spurt_initial_speed, 0x006944a0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gPed_spurt_min_max_time, 2, 0x006942f8);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gPed_spurt_model, 0x00694278);
C2_HOOK_VARIABLE_IMPLEMENT(int, gGiblet_scrape_start, 0x0069411c);
C2_HOOK_VARIABLE_IMPLEMENT(tSmashable_initial_position_spec, gGiblet_initial_position, 0x00694468);
C2_HOOK_VARIABLE_IMPLEMENT(tSmashable_initial_speed_spec, gGiblet_initial_speed, 0x0069bc38);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gGiblet_min_max_time, 2, 0x006a0408);
C2_HOOK_VARIABLE_IMPLEMENT(int, gGiblet_size_count, 0x0069bc18);
C2_HOOK_VARIABLE_IMPLEMENT(tPed_giblet_size_spec*, gGiblet_sizes, 0x00694274);

void C2_HOOK_FAKE_THISCALL ScaleModelXYZ(br_model* pModel, int pArg2, float pX, float pY, float pZ) {
    int i;

    REC2_THISCALL_UNUSED(pArg2);

    for (i = 0; i < pModel->nvertices; i++) {
        pModel->vertices[i].p.v[0] *= pX;
        pModel->vertices[i].p.v[1] *= pY;
        pModel->vertices[i].p.v[2] *= pZ;
    }
    BrModelUpdate(pModel, BR_MODU_NORMALS);
}
C2_HOOK_FUNCTION(0x00404290, ScaleModelXYZ)

void C2_HOOK_FAKE_THISCALL ScaleModel(br_model* pModel, int pArg2, float pScale) {
    int i;

    REC2_THISCALL_UNUSED(pArg2);

    for (i = 0; i < pModel->nvertices; i++) {
        pModel->vertices[i].p.v[0] *= pScale;
        pModel->vertices[i].p.v[1] *= pScale;
        pModel->vertices[i].p.v[2] *= pScale;
    }
    BrModelUpdate(pModel, BR_MODU_NORMALS);
}
C2_HOOK_FUNCTION(0x004042f0, ScaleModel)

br_model* ReadSmashableInitialModel(FILE* pFile, br_scalar pScale_factor) {
    float scale;
    int index;
    int r, g, b;
    char s[256];
    br_model* model;

    /* Scaling factor */
    scale = GetAScalar(pFile);
    /* Indexed colour */
    index = GetAnInt(pFile);
    /* rgb colour */
    GetThreeInts(pFile, &r, &g, &b);
    /* Model name */
    GetAString(pFile, s);

    model = BrModelFind(s);
    if (model == NULL) {
        FatalError(kFatalError_CannotFindGibletModel_S, s);
    }
    ScaleModel(model REC2_THISCALL_EDX, scale * pScale_factor);
    model->faces[0].material->index_base = index;
    model->faces[0].material->index_range = 1;
    model->faces[0].material->colour = BR_COLOUR_RGB(r, g, b);
    BrMaterialUpdate(model->faces[0].material, BR_MATU_ALL);
    return model;
}

void C2_HOOK_FASTCALL ReadPedGiblets(FILE* pFile) {
    tPath_name the_path;
    int i;
    int j;

    PathCat(the_path, C2V(gApplication_path), C2V(gPedsFolder));
    PathCat(the_path, the_path, "GIBLETS");
    LoadFolderInStorageWithShading(&C2V(gMisc_storage_space), the_path, kRendererShadingType_Diffuse1);

    /*
     * ***************************
     *         GIBLETS
     * ***************************
     *
     * IMPACT GIBLETS
     */

    /* Min index usable for scrapes (not impacts) */
    C2V(gGiblet_scrape_start) = GetAnInt(pFile);

    ReadSmashableInitialPosition(pFile, &C2V(gGiblet_initial_position));

    ReadSmashableInitialSpeed(pFile, &C2V(gGiblet_initial_speed));

    ReadMinMaxTimeInMilliseconds(pFile, C2V(gGiblet_min_max_time));

    REC2_BUG_ON(sizeof(tPed_giblet_size_spec) != 24);

    /* Number of giblet sizes */
    C2V(gGiblet_size_count) = GetAnInt(pFile);
    C2V(gGiblet_sizes) = BrMemAllocate(C2V(gGiblet_size_count) * sizeof(tPed_giblet_size_spec), kMem_misc_poly_ped);

    for (i = 0; i < C2V(gGiblet_size_count); i++) {
        /* flags */
        C2V(gGiblet_sizes)[i].flags = GetAnInt(pFile);
        /* Min force for creation */
        C2V(gGiblet_sizes)[i].min_force = GetAScalar(pFile);
        /* Force per instance */
        C2V(gGiblet_sizes)[i].instance_force = GetAScalar(pFile);
        /* Max instances */
        C2V(gGiblet_sizes)[i].max_count = GetAnInt(pFile);
        /* Model count */
        C2V(gGiblet_sizes)[i].model_count = GetAnInt(pFile);

        for (j = 0; j < C2V(gGiblet_sizes)[i].model_count; j++) {
            float factor;

            factor = 1.f;
            if (i == C2V(gGiblet_size_count) - 1) {
                factor = 3.f / 4.f;
            }
            C2V(gGiblet_sizes)[i].models[j] = ReadSmashableInitialModel(pFile, factor);
        }

    }

    /* SMALL BLOOD CLOUD SPEC */
    ReadExplosionAnimation(pFile, &C2V(gPed_explosion_small_blood_cloud));
    /* MED BLOOD CLOUD SPEC */
    ReadExplosionAnimation(pFile, &C2V(gPed_medium_blood_cloud));
    /* LARGE BLOOD CLOUD SPEC */
    ReadExplosionAnimation(pFile, &C2V(gPed_large_blood_cloud));

    C2V(gPed_slick_material) = BrMaterialAllocate("GIBSLICK");
    BrMaterialAdd(C2V(gPed_slick_material));
    C2V(gPed_slick_material)->flags |= BR_MATF_LIGHT;
    C2V(gPed_slick_material)->flags |= BR_MATF_PERSPECTIVE;
    C2V(gPed_slick_material)->flags |= BR_MATF_SMOOTH;
    C2V(gPed_slick_material)->ka = .99f;
    C2V(gPed_slick_material)->kd = .0f;
    C2V(gPed_slick_material)->ks = .0f;
    C2V(gPed_slick_material)->power = .0f;
    C2V(gPed_slick_material)->index_base = 0;
    C2V(gPed_slick_material)->index_range = 0;
    BrMatrix23Identity(&C2V(gPed_slick_material)->map_transform);
    C2V(gPed_slick_material)->index_shade = BrTableFind("IDENTITY.TAB");
    C2V(gPed_slick_material)->colour_map = BrTableFind("GIBSLICK");
    AdaptMaterialsForRenderer(&C2V(gPed_slick_material), 1, kRendererShadingType_Diffuse2);

    C2V(gPed_smear_material) = BrMaterialFind("PEDSMEAR");

    /* GIB SLICK! */
    ReadSlick(pFile, &C2V(gPed_slick));

    /* SPURTING STUMP GIBLETS */
    /* Number of spurty lumps at a time */
    C2V(gPed_spurty_lumps_count) = GetAnInt(pFile);
    /* Spurting period (euurrgghhh!!) */
    C2V(gPed_spurting_period) = GetAnInt(pFile);
    /* Total number of parts spurted out by dismembered part */
    C2V(gPed_dismember_spurting_member_count) = GetAnInt(pFile);

    ReadSmashableInitialPosition(pFile, &C2V(gPed_spurt_initial_position));

    ReadSmashableInitialSpeed(pFile, &C2V(gPed_spurt_initial_speed));

    ReadMinMaxTimeInMilliseconds(pFile, C2V(gPed_spurt_min_max_time));

    C2V(gPed_spurt_model) = ReadSmashableInitialModel(pFile, 1.f);
}
C2_HOOK_FUNCTION(0x004cb1e0, ReadPedGiblets)

void (C2_HOOK_CDECL * InitPeds_original)(void);
void C2_HOOK_CDECL InitPeds(void) {
#if defined(C2_HOOKS_ENABLED)
    InitPeds_original();
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004cadc0, InitPeds, InitPeds_original)
