#include "55-volume.h"

#include "08-loading1.h"
#include "00-car.h"
#include "34-finteray.h"
#include "69-sound.h"
#include "globvars.h"

#include "c2_string.h"

#include "rec2_macros.h"

// GLOBAL: CARMA2_HW 0x00761b80
tSpecial_volume gDefault_default_water_spec_vol;

// GLOBAL: CARMA2_HW 0x00761f44
tSpecial_volume* gDefault_water_spec_vol_real;

// GLOBAL: CARMA2_HW 0x00660268
const char* gSoundType_choices[2] = {
    "SATURATED",
    "SCATTERED"
};

// FUNCTION: CARMA2_HW 0x004ff110
tSpecial_volume* C2_HOOK_FASTCALL GetDefaultSpecialVolumeForWater(void) {
    return gDefault_water_spec_vol_real;
}

// FindSpecialVolume

// MungeSpecialVolume

/* The original scans the complete identifier even though only emptiness is used. */
static int C2_HOOK_CDECL VolumeIdentifierHasLength(const char* pIdentifier) {
    const volatile char* end = pIdentifier;

    while (*end != '\0') {
        end++;
    }
    return end != pIdentifier;
}

static int C2_HOOK_CDECL VolumeBelowSurfaceLimit(const br_scalar* pDistance) {
    tU32 bits;

    memcpy(&bits, pDistance, 4);
    return (bits & 0x7fffffffu) > 0x7f800000u || *pDistance < 100.f;
}

// FUNCTION: CARMA2_HW 0x004ff530
void C2_HOOK_FASTCALL ResetCarSpecialVolume(tPhysics_object* pObject) {
    br_vector3 direction;
    br_vector3 normal;
    br_scalar distance;
    br_material* material;
    const char* identifier;
    tSpecial_volume* volume = NULL;

    direction.v[0] = 0.f;
    direction.v[1] = 200.f;
    direction.v[2] = 0.f;
    DisablePlingMaterials();
    FindFace(&pObject->actor->t.t.translate.t, &direction, &normal, &distance, &material);
    EnablePlingMaterials();
    if (VolumeBelowSurfaceLimit(&distance) && material != NULL) {
        identifier = material->identifier;
        if (identifier != NULL && VolumeIdentifierHasLength(identifier)) {
            if (identifier[0] == '!' || identifier[0] == '#') {
                volume = gDefault_water_spec_vol_real;
            }
        }
    }
    pObject->auto_special_volume = volume;
    pObject->water_depth_factor = 1.f;
}

#if defined(__GNUC__)
typedef tU32 tVolumeWord __attribute__((__may_alias__));
#else
typedef tU32 tVolumeWord;
#endif
static void C2_HOOK_CDECL AutoVolumeCopy(void* out, const void* in) {
    *(volatile tVolumeWord*)out = *(const volatile tVolumeWord*)in;
}
#if defined(_MSC_VER) && _MSC_VER < 1300
#pragma auto_inline(off)
#endif
static double C2_HOOK_CDECL AutoVolumeScalar(double value) {
    float scalar;
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        fld qword ptr value
        fstp dword ptr scalar
        fld dword ptr scalar
        fstp qword ptr result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("fldl %2; fstps %1; flds %1; fstpl %0"
        : "=m"(result),"=m"(scalar) : "m"(value) : "st");
#else
    scalar=(float)value;
    result=scalar;
#endif
    return result;
}
#if defined(_MSC_VER) && _MSC_VER < 1300
#pragma auto_inline(on)
#endif
static int C2_HOOK_CDECL AutoVolumeUnordered(double value) {
    union { double d; tU32 w[2]; } bits;
    bits.d=value;
    return (bits.w[1]&0x7ff00000u)==0x7ff00000u && ((bits.w[1]&0xfffffu)||bits.w[0]);
}
/* Paired payloads571..586 expose modern MSVC's different NaN selection. */
static double C2_HOOK_CDECL AutoVolumeAdd(double a,double b) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        fld qword ptr a
        fld qword ptr b
        faddp st(1),st(0)
        fstp qword ptr result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("fldl %1; fldl %2; faddp %%st,%%st(1); fstpl %0"
        : "=m"(result) : "m"(a),"m"(b) : "st","st(1)");
#else
    result=a+b;
#endif
    return result;
}

static double C2_HOOK_CDECL AutoVolumeRow(const br_scalar* pRow,const br_vector3* pNormal) {
    return AutoVolumeAdd(AutoVolumeAdd((double)pRow[1]*pNormal->v[1],(double)pRow[0]*pNormal->v[0]),(double)pRow[2]*pNormal->v[2]);
}

// FUNCTION: CARMA2_HW 0x004ff5d0
void C2_HOOK_FASTCALL TestAutoSpecialVolume(tPhysics_object* pObject) {
    double lower=0.,upper,increment,width,high_x,dot;
    br_vector3 low,high,edge,direction,normal;
    br_scalar first_distance,second_distance;
    tSpecial_volume* default_water;
    int i;
    for(i=0;i<3;i++) lower=AutoVolumeAdd(lower,AutoVolumeRow(pObject->transform_matrix.m[i],&pObject->water_normal)*pObject->bb2.min.v[i]);
    lower=AutoVolumeAdd(lower,AutoVolumeAdd(AutoVolumeAdd((double)pObject->transform_matrix.m[3][2]*pObject->water_normal.v[2],(double)pObject->transform_matrix.m[3][1]*pObject->water_normal.v[1]),(double)pObject->transform_matrix.m[3][0]*pObject->water_normal.v[0]));
    upper=lower;
    for(i=0;i<3;i++) {
        const br_scalar* row=pObject->transform_matrix.m[i];
        width=(double)pObject->bb2.max.v[i]-pObject->bb2.min.v[i];
        increment=AutoVolumeAdd(AutoVolumeAdd((double)row[0]*pObject->water_normal.v[0],(double)row[2]*pObject->water_normal.v[2]),(double)row[1]*pObject->water_normal.v[1])*width;
        if(AutoVolumeUnordered(increment)||increment<0.) lower=AutoVolumeAdd(lower,AutoVolumeScalar(increment));
        else upper=AutoVolumeAdd(upper,AutoVolumeScalar(increment));
    }
    if(!AutoVolumeUnordered(lower)&&!AutoVolumeUnordered(pObject->water_d)&&!(lower<pObject->water_d)) {
        pObject->auto_special_volume=NULL;
        pObject->water_depth_factor=1.f;
        return;
    }
    if(AutoVolumeUnordered(upper-0.001)||AutoVolumeUnordered(pObject->water_d)||(double)pObject->water_d<upper-0.001)
        pObject->water_depth_factor=(br_scalar)AutoVolumeScalar(((double)pObject->water_d-lower)/(upper-lower));
    else pObject->water_depth_factor=1.f;
    if(pObject->auto_special_volume!=NULL) return;
    default_water=gDefault_water_spec_vol_real;
    if(default_water!=NULL) {
        BrMatrix34ApplyP(&low,&pObject->bb2.min,&pObject->transform_matrix);
        AutoVolumeCopy(&high.v[0],&low.v[0]);
        AutoVolumeCopy(&high.v[1],&low.v[1]);
        AutoVolumeCopy(&high.v[2],&low.v[2]);
        high_x=low.v[0];
        for(i=0;i<3;i++) {
            const br_scalar* row=pObject->transform_matrix.m[i];
            width=(double)pObject->bb2.max.v[i]-pObject->bb2.min.v[i];
            edge.v[0]=(br_scalar)AutoVolumeScalar((double)row[0]*width);
            edge.v[2]=(br_scalar)AutoVolumeScalar((double)row[2]*width);
            edge.v[1]=(br_scalar)AutoVolumeScalar((double)row[1]*width);
            dot=AutoVolumeAdd(AutoVolumeAdd((double)pObject->water_normal.v[0]*edge.v[0],(double)pObject->water_normal.v[2]*edge.v[2]),(double)pObject->water_normal.v[1]*edge.v[1]);
            if(!AutoVolumeUnordered(dot)&&dot>0.) {
                high_x+=edge.v[0];
                high.v[1]=(br_scalar)AutoVolumeScalar((double)high.v[1]+edge.v[1]);
                high.v[2]=(br_scalar)AutoVolumeScalar((double)high.v[2]+edge.v[2]);
            } else {
                low.v[0]=(br_scalar)AutoVolumeScalar((double)low.v[0]+edge.v[0]);
                low.v[1]=(br_scalar)AutoVolumeScalar((double)low.v[1]+edge.v[1]);
                low.v[2]=(br_scalar)AutoVolumeScalar((double)low.v[2]+edge.v[2]);
            }
        }
        high.v[0]=(br_scalar)AutoVolumeScalar(high_x);
        direction.v[0]=(br_scalar)AutoVolumeScalar((double)low.v[0]-high_x);
        direction.v[1]=(br_scalar)AutoVolumeScalar((double)low.v[1]-high.v[1]);
        direction.v[2]=(br_scalar)AutoVolumeScalar((double)low.v[2]-high.v[2]);
        DisablePlingMaterials();
        FUN_0041e480(&high,&direction,&normal,&first_distance,pObject);
        EnablePlingMaterials();
        FUN_0041e480(&high,&direction,&normal,&second_distance,pObject);
        if(AutoVolumeUnordered(first_distance)||AutoVolumeUnordered(second_distance)||first_distance<second_distance) {
            pObject->auto_special_volume=default_water;
            return;
        }
    }
    pObject->water_depth_factor=1.f;
    pObject->auto_special_volume=NULL;
}

// FUNCTION: CARMA2_HW 0x004ff8d0
void C2_HOOK_FASTCALL ParseSpecialVolume(FILE* pF, tSpecial_volume* pSpec, char* pScreen_name_str, int soundfx) {
    char s[256];

    /* gravity multiplier */
    pSpec->gravity_multiplier = GetAFloat(pF);
    if (pSpec->gravity_multiplier < 0 && !gRussels_fannies) {
        pSpec->gravity_multiplier = 1.f;
    }
    /* viscosity multiplier */
    pSpec->viscosity_multiplier = GetAFloat(pF);
    /* Car damage per millisecond */
    pSpec->car_damage_per_ms = GetAScalar(pF);
    /* Pedestrian damage per millisecond */
    pSpec->ped_damage_per_ms = GetAScalar(pF);
    /* camera effect index */
    pSpec->camera_special_effect_index = GetAnInt(pF);
    /* Sky colour */
    pSpec->sky_col = GetAnInt(pF);

    /* Windscreen material to use */
    GetAString(pF, s);
    if (pScreen_name_str != NULL) {
        strcpy(pScreen_name_str, s);
    } else {
        pSpec->screen_pixelmap = BrMapFind(s);
    }

    /* Sound ID of entry noise */
    pSpec->entry_noise = GetAnInt(pF);
    /* Sound ID of exit noise */
    pSpec->exit_noise = GetAnInt(pF);
    /* Engine noise index */
    pSpec->engine_noise_index = GetAnInt(pF);
    /* material index */
    pSpec->material_modifier_index = GetAnInt(pF);
    if (soundfx) {
        pSpec->soundfx_type = GetALineAndInterpretCommand(pF, gSoundType_choices, REC2_ASIZE(gSoundType_choices));
        if (pSpec->soundfx_type != kSoundFx_None) {
            ReadSoundSpec(pF, &pSpec->soundfx_data);
        }
    } else {
        pSpec->soundfx_type = kSoundFx_None;
    }
}

// SaveSpecialVolumes

// FUNCTION: CARMA2_HW 0x004ffd20
void C2_HOOK_FASTCALL FindInverseAndWorldBox(tSpecial_volume* pVolume) {
    br_bounds unit_box = {{{-1.0f, -1.0f, -1.0f}}, {{1.0f, 1.0f, 1.0f}}};

    GetNewBoundingBox(&pVolume->boundary.box.bounds, &unit_box, &pVolume->boundary.box.mat);
    BrMatrix34Inverse(&pVolume->boundary.box.inv_mat, &pVolume->boundary.box.mat);
}

// ReadSpecialVolumes
