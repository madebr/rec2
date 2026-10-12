#include "49-physics.h"

#include "00-car.h"
#include "16-graphics1.h"
#include "28-world3.h"
#include "32-spark.h"
#include "34-finteray.h"
#include "37-brucetrk.h"
#include "52-errors.h"
#include "globvars.h"
#include "c2_string.h"
#include "c2_stdlib.h"
#include "c2_math.h"

#include <stddef.h>

#define PHYSICS_BUFFER_PART_SIZE 40000
#define PHYSICS_BUFFER_OTHER_SIZE 50000

static int C2_HOOK_FASTCALL PhysicsScalarNaN(const br_scalar* value);
static int C2_HOOK_FASTCALL PhysicsLessOrUnordered(const br_scalar* a, const br_scalar* b);
static int C2_HOOK_FASTCALL PhysicsGreaterOrdered(const br_scalar* a, const br_scalar* b);
static br_scalar C2_HOOK_FASTCALL PhysicsScalar(double value);
static void C2_HOOK_FASTCALL PhysicsScalarStore(br_scalar* output, const br_scalar* input);

// GLOBAL: CARMA2_HW 0x0074a5e4
int gPHIL_mechanics_time_sync;

// GLOBAL: CARMA2_HW 0x0065d010
int gFace_num__car = 1;

// GLOBAL: CARMA2_HW 0x00692dd0
br_scalar gPhysics_bounds_max_x;

// GLOBAL: CARMA2_HW 0x00692dd4
br_scalar gPhysics_bounds_min_x;

// GLOBAL: CARMA2_HW 0x00676920
char gPhysics_empty_message;

// GLOBAL: CARMA2_HW 0x006940c0
tPhysicsError_cbfn* gPhysics_error_cb;

// GLOBAL: CARMA2_HW 0x00744818
void* gPhysics_buffer1_ptr;

// GLOBAL: CARMA2_HW 0x00744814
void* gPhysics_buffer2_ptr;

// GLOBAL: CARMA2_HW 0x0074a5e0
void* gPhysics_buffer3_ptr;

// GLOBAL: CARMA2_HW 0x006940c8
int gPhysics_other_buffer_capacity;

// GLOBAL: CARMA2_HW 0x006940c4
void* gPhysics_other_buffer;

// GLOBAL: CARMA2_HW 0x006940bc
void* gPhysics_scratch_ptr;

// count

static void C2_HOOK_FASTCALL PhysicsCopyWord(void* output, const void* input) {
    volatile tU32 value;

    memcpy((void*)&value, input, sizeof(value));
    memcpy(output, (const void*)&value, sizeof(value));
}

/* The original memory-operand subtraction selects distinct NaNs by x87 rules. */
static void C2_HOOK_FASTCALL PhysicsSubtractScalar(br_scalar* output, const br_scalar* a, const br_scalar* b) {
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, a
        mov ecx, b
        mov edx, output
        fld dword ptr [eax]
        fsub dword ptr [ecx]
        fstp dword ptr [edx]
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile("flds %1; fsubs %2; fstps %0"
        : "=m" (*output) : "m" (*a), "m" (*b) : "st");
#else
    *output = (br_scalar)((double)*a - *b);
#endif
}

// FUNCTION: CARMA2_HW 0x00422fe0
int C2_HOOK_FASTCALL AddObjectWallForce(tPhysics_contact* pContacts, const br_vector3* pPosition, br_scalar pT, const br_vector3* pNormal, int pUnused, int pCount, tPhysics_object* pObject, int pType, tFace_ref* pFace, const br_vector3* pReference_position) {
    br_vector3 relative;
    double difference[3];
    br_scalar rounded_x;
    br_scalar rounded_y;
    br_vector3 low;
    br_vector3 high;
    br_scalar margin = 0.f;
    double distance_squared;
    double normal_dot;
    int i;
    int slot;
    int last;
    tPhysics_contact* contact;

    (void)pUnused;
    if (pObject->parent == NULL && pObject->physics_joint2 != NULL && pObject->physics_joint2->type != eJoint_none) {
        for (i = 0; i < 3; i++) {
            PhysicsSubtractScalar(&low.v[i], &pObject->bb1.min.v[i], &pObject->physics_joint2->field_0x08.v[i]);
            PhysicsSubtractScalar(&high.v[i], &pObject->bb1.max.v[i], &pObject->physics_joint2->field_0x08.v[i]);
        }
        for (i = 0; i < 3; i++) {
            double smallest = fabs((double)high.v[i]);

            if (fabs((double)low.v[i]) <= smallest) {
                smallest = fabs((double)low.v[i]);
            }
            if (smallest > margin) {
                margin = (br_scalar)smallest;
            }
        }
        margin = (br_scalar)((double)margin - -.01);
        if (!(fabs((double)pPosition->v[0] - pObject->physics_joint2->field_0x08.v[0]) >= margin)
            && !(fabs((double)pPosition->v[1] - pObject->physics_joint2->field_0x08.v[1]) >= margin)
            && fabs((double)pPosition->v[1] - pObject->physics_joint2->field_0x08.v[1]) * 2. < (double)pObject->bb1.max.v[1] - pObject->bb1.min.v[1]
            && !(fabs((double)pPosition->v[2] - pObject->physics_joint2->field_0x08.v[2]) >= margin)) {
            return pCount;
        }
    }
    for (i = 0; i < 3; i++) {
        PhysicsSubtractScalar(&relative.v[i], &pPosition->v[i], &pObject->cmpos.v[i]);
    }
    for (slot = 0; slot < pCount; slot++) {
        contact = &pContacts[slot];
        for (i = 0; i < 3; i++) {
            difference[i] = (double)contact->position.v[i] - relative.v[i];
        }
        rounded_x = PhysicsScalar(difference[0]);
        rounded_y = PhysicsScalar(difference[1]);
        distance_squared = (difference[0] * rounded_x + difference[1] * rounded_y) + difference[2] * difference[2];
        if (!(distance_squared >= .0045000000000000005)) {
            normal_dot = ((double)contact->normal.v[2] * pNormal->v[2] + (double)contact->normal.v[1] * pNormal->v[1]) + (double)contact->normal.v[0] * pNormal->v[0];
            if (normal_dot > .95 && !(PhysicsScalar(distance_squared) >= .0005)) {
                return pCount;
            }
            if (!(normal_dot >= -.95)) {
                return pCount;
            }
        }
    }
    if (pCount < 4) {
        pContacts[pCount].t = 2.f;
        pCount++;
    }
    if (pCount <= 0) {
        return pCount;
    }
    for (slot = 0; slot < pCount; slot++) {
        if (!(pT >= pContacts[slot].t)) {
            break;
        }
    }
    if (slot == pCount) {
        return pCount;
    }
    last = pCount < 4 ? pCount : 3;
    for (; last >= slot; last--) {
        for (i = 0; i < 24; i++) {
            PhysicsCopyWord((tU8*)&pContacts[last] + 4 * i, (const tU8*)&pContacts[last - 1] + 4 * i);
        }
    }
    contact = &pContacts[slot];
    PhysicsScalarStore(&contact->t, &pT);
    contact->object1 = pObject;
    contact->object2 = NULL;
    contact->face = pFace;
    for (i = 0; i < 3; i++) {
        PhysicsCopyWord(&contact->normal.v[i], &pNormal->v[i]);
    }
    for (i = 0; i < 3; i++) {
        PhysicsSubtractScalar(&contact->position.v[i], &pPosition->v[i], &pObject->cmpos.v[i]);
    }
    if (pReference_position != NULL) {
        for (i = 0; i < 3; i++) {
            PhysicsCopyWord(&contact->reference_position.v[i], &pReference_position->v[i]);
        }
    }
    contact->type = pType;
    return pCount;
}

// AddObjectObjectForce

// FUNCTION: CARMA2_HW 0x004225f0
tFace_ref* C2_HOOK_FASTCALL RayCastThruFaceList(br_vector3* pPosition, br_vector3* pDirection, br_vector3* pNormal, br_scalar* pT, br_vector3* pCollision_position, tFace_ref* pFaces, int pCount) {
    br_vector3 normal;
    br_vector3 collision_position;
    br_scalar t;
    tFace_ref* best = NULL;
    int i;

    *pT = 2.f;
    for (; pCount > 0; pCount--, pFaces++) {
        if ((pFaces->flags & 0x80) == 0) {
            CheckSingleFace(pFaces, pPosition, pDirection, &normal, &t, &collision_position);
            if (!(t >= *pT)) {
                PhysicsCopyWord(pT, &t);
                for (i = 0; i < 3; i++) {
                    PhysicsCopyWord(&pNormal->v[i], &normal.v[i]);
                }
                best = pFaces;
                for (i = 0; i < 3; i++) {
                    PhysicsCopyWord(&pCollision_position->v[i], &collision_position.v[i]);
                }
            }
        }
    }
    return !(*pT >= 2.f) ? best : NULL;
}

// RayCastThruFaceList2

// RayCastInBox

// FaceOffCount2

// FaceOffCount

// FaceOnCount

static int C2_HOOK_FASTCALL PhysicsWallNaN(double value) {
    union { double d; tU32 w[2]; } u;
    u.d=value;
    return (u.w[1]&0x7ff00000u)==0x7ff00000u && ((u.w[1]&0xfffffu)||u.w[0]);
}

static double C2_HOOK_FASTCALL PhysicsWallAddScalar(const br_scalar* a, const volatile br_scalar* b) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, a
        mov ecx, b
        fld dword ptr [eax]
        fadd dword ptr [ecx]
        fstp qword ptr result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("flds %1\n\tfadds %2\n\tfstpl %0" : "=m"(result) : "m"(*a), "m"(*b) : "st");
#else
    result = (double)*a + *b;
#endif
    return result;
}

// FUNCTION: CARMA2_HW 0x00423320
int C2_HOOK_FASTCALL EliminateFaces(tPhysics_object* pObject, tPhysics_shape* pShape, br_matrix34* pTransform) {
    br_matrix34* actor_matrix=&pObject->actor->t.t.mat;
    br_bounds* bounds=&pShape->common.field_0x1c;
    br_vector3 center;
    volatile br_vector3 scaled[3];
    volatile br_scalar extent_scalar;
    volatile br_scalar low_scalar;
    volatile br_scalar threshold;
    double extent;
    double low;
    double high;
    double translated;
    double vertex;
    double x;
    double y;
    double z;
    int count=0;
    int i;
    int j;
    int ordinal=0;
    tFace_ref* face;
    br_material* material;

    GetNewBoundingBox(bounds,&pShape->common.bb,pTransform);
    for (i=0;i<3;i++) {
        if (bounds->min.v[i]>pShape->common.bb.min.v[i]) {
            PhysicsCopyWord(&bounds->min.v[i],&pShape->common.bb.min.v[i]);
        }
        if (PhysicsScalarNaN(&bounds->max.v[i])||PhysicsScalarNaN(&pShape->common.bb.max.v[i])||bounds->max.v[i]<pShape->common.bb.max.v[i]) {
            PhysicsCopyWord(&bounds->max.v[i],&pShape->common.bb.max.v[i]);
        }
    }
    BrMatrix34ApplyV(&center,&bounds->min,actor_matrix);
    for (i=0;i<3;i++) center.v[i]=PhysicsScalar((double)center.v[i]+actor_matrix->m[3][i]);
    for (i=0;i<3;i++) {
        extent=(double)bounds->max.v[i]-bounds->min.v[i];
        for (j=0;j<3;j++) scaled[i].v[j]=PhysicsScalar(actor_matrix->m[i][j]*extent);
    }
    face=&gFace_list__car[pObject->box_face_start];
    while (ordinal<(int)pObject->box_face_end-(int)pObject->box_face_start) {
        x=((double)center.v[0]-face->v[0].v[0])*face->normal.v[0];
        z=((double)center.v[2]-face->v[0].v[2])*face->normal.v[2];
        y=((double)center.v[1]-face->v[0].v[1])*face->normal.v[1];
        low=(x+z)+y;
        high=low;
        for (i=0;i<3;i++) {
            extent=((double)scaled[i].v[0]*face->normal.v[0]+(double)scaled[i].v[1]*face->normal.v[1])+(double)scaled[i].v[2]*face->normal.v[2];
            extent_scalar=PhysicsScalar(extent);
            if (PhysicsWallNaN(extent)||extent<0.0) low+=extent_scalar;
            else high+=extent_scalar;
        }
        low_scalar=PhysicsScalar(low);
        if ((!PhysicsWallNaN(high)&&high>(double)0.005f && !PhysicsWallNaN(low)&&low>(double)0.005f)
            ||((PhysicsWallNaN(high)||high<(double)-0.005f)&&(PhysicsScalarNaN((const br_scalar*)&low_scalar)||low_scalar<-0.005f))) {
            *(volatile int*)&face->flags=face->flags|0x80;
        } else {
            material=face->material;
            if ((material!=NULL&&material->identifier[0]=='!')||material->identifier[0]=='>') {
                *(volatile int*)&face->flags=face->flags|0x80;
            } else {
                for (i=0;i<3;i++) {
                    translated=((double)actor_matrix->m[3][2]*actor_matrix->m[i][2]+(double)actor_matrix->m[3][0]*actor_matrix->m[i][0])+(double)actor_matrix->m[3][1]*actor_matrix->m[i][1];
                    vertex=((double)face->v[0].v[2]*actor_matrix->m[i][2]+(double)face->v[0].v[0]*actor_matrix->m[i][0])+(double)face->v[0].v[1]*actor_matrix->m[i][1];
                    threshold=PhysicsScalar(((double)bounds->min.v[i]-0.005f)+translated);
                    if (PhysicsWallNaN(vertex)||PhysicsScalarNaN((const br_scalar*)&threshold)||vertex<threshold) {
                        vertex=((double)face->v[1].v[2]*actor_matrix->m[i][2]+(double)face->v[1].v[0]*actor_matrix->m[i][0])+(double)face->v[1].v[1]*actor_matrix->m[i][1];
                        if (PhysicsWallNaN(vertex)||PhysicsScalarNaN((const br_scalar*)&threshold)||vertex<threshold) {
                            vertex=((double)face->v[2].v[2]*actor_matrix->m[i][2]+(double)face->v[2].v[0]*actor_matrix->m[i][0])+(double)face->v[2].v[1]*actor_matrix->m[i][1];
                            if (PhysicsWallNaN(vertex)||PhysicsScalarNaN((const br_scalar*)&threshold)||vertex<threshold) *(volatile int*)&face->flags=face->flags|0x80;
                        }
                    } else {
                        threshold=PhysicsScalar(((double)bounds->max.v[i]+translated)-(-0.005f));
                        if (!PhysicsWallNaN(vertex)&&!PhysicsScalarNaN((const br_scalar*)&threshold)&&vertex>threshold) {
                            vertex=((double)face->v[1].v[2]*actor_matrix->m[i][2]+(double)face->v[1].v[0]*actor_matrix->m[i][0])+(double)face->v[1].v[1]*actor_matrix->m[i][1];
                            if (!PhysicsWallNaN(vertex)&&!PhysicsScalarNaN((const br_scalar*)&threshold)&&vertex>threshold) {
                                vertex=((double)face->v[2].v[2]*actor_matrix->m[i][2]+(double)face->v[2].v[0]*actor_matrix->m[i][0])+(double)face->v[2].v[1]*actor_matrix->m[i][1];
                                if (!PhysicsWallNaN(vertex)&&!PhysicsScalarNaN((const br_scalar*)&threshold)&&vertex>threshold) *(volatile int*)&face->flags=face->flags|0x80;
                            }
                        }
                    }
                }
                *(volatile int*)&face->flags=face->flags&~0x80;
                count++;
            }
        }
        ordinal++;
        face++;
    }
    return count;
}

// FUNCTION: CARMA2_HW 0x00423ce0
int C2_HOOK_FASTCALL CheckSingleEdgeWithSphere(br_vector3* pEdge_start, br_vector3* pEdge_end, tPhysics_object* pObject, tCollision_shape_sphere_data* pSphere, tPhysics_contact* pContacts, int pCount, br_vector3* pSphere_start, br_vector3* pSphere_end, tFace_ref* pFace) {
    br_vector3 point;
    br_vector3 normal;
    br_vector3 trial_normal;
    br_vector3 actor_normal;
    volatile br_vector3 scaled_normal;
    br_vector3 surface_point;
    br_actor* actor;
    volatile br_scalar time;
    volatile br_scalar normalized_z;
    volatile br_scalar radius_product[3];
    double remainder;
    double x;
    double y;
    double z;
    double length;
    double scale;
    int i;

    SphereEdgeCollide(pEdge_start,pEdge_end,pSphere_start,pSphere_end,pSphere,&point,&normal,(br_scalar*)&time,0,0);
    if (!PhysicsScalarNaN((const br_scalar*)&time) && time>1.0f) return pCount;
    BrMatrix34TApplyV(&trial_normal,&normal,&pObject->transform_matrix);
    actor=*(br_actor* volatile*)&pObject->actor;
    BrMatrix34TApplyV(&actor_normal,&normal,&actor->t.t.mat);
    remainder=1.0-(double)time;
    for (i=0;i<3;i++) scaled_normal.v[i]=PhysicsScalar((double)time*actor_normal.v[i]);
    x=(double)trial_normal.v[0]*remainder+scaled_normal.v[0];
    y=(double)trial_normal.v[1]*remainder+scaled_normal.v[1];
    z=(double)trial_normal.v[2]*remainder+scaled_normal.v[2];
    length=sqrt((x*x+y*y)+z*z);
    if (PhysicsWallNaN(length)||length<=2.3841858e-7f) {
        x=1.0;y=0.0;normalized_z=0.0f;
    } else {
        scale=1.0/length;x=scale*x;y=scale*y;normalized_z=PhysicsScalar(scale*z);
    }
    radius_product[0]=PhysicsScalar(pSphere->radius*x);
    radius_product[1]=PhysicsScalar(pSphere->radius*y);
    radius_product[2]=PhysicsScalar(pSphere->radius*(double)normalized_z);
    surface_point.v[0]=PhysicsScalar(PhysicsWallAddScalar(&pSphere->center.v[0], &radius_product[0]));
    surface_point.v[1]=PhysicsScalar(PhysicsWallAddScalar(&pSphere->center.v[1], &radius_product[1]));
    surface_point.v[2]=PhysicsScalar(PhysicsWallAddScalar(&pSphere->center.v[2], &radius_product[2]));
    for(i=0;i<3;i++) trial_normal.v[i]=PhysicsScalar(-(double)trial_normal.v[i]);
    return AddObjectWallForce(pContacts,&surface_point,time,&trial_normal,0,pCount,pObject,4,pFace,NULL);
}

// FUNCTION: CARMA2_HW 0x00423c00
int C2_HOOK_FASTCALL GetSphereToEdgePoints(tPhysics_object* pObject, tCollision_shape_sphere_data* pSphere, tPhysics_contact* pContacts, int pCount, int pUnused_capacity, br_vector3* pSphere_start, br_vector3* pSphere_end, br_matrix34* pUnused_matrix) {
    int index=pObject->box_face_start;
    tFace_ref* face=&gFace_list__car[index];

    (void)pUnused_capacity;
    (void)pUnused_matrix;
    if(index>=(int)pObject->box_face_end) return pCount;
    do {
        if((*(volatile int*)&face->flags&0x80)==0) {
            if((*(volatile int*)&face->flags&1)==0) pCount=CheckSingleEdgeWithSphere(&face->v[0],&face->v[1],pObject,pSphere,pContacts,pCount,pSphere_start,pSphere_end,face);
            if((*(volatile int*)&face->flags&2)==0) pCount=CheckSingleEdgeWithSphere(&face->v[1],&face->v[2],pObject,pSphere,pContacts,pCount,pSphere_start,pSphere_end,face);
            if((*(volatile int*)&face->flags&4)==0) pCount=CheckSingleEdgeWithSphere(&face->v[2],&face->v[0],pObject,pSphere,pContacts,pCount,pSphere_start,pSphere_end,face);
        }
        index++;face++;
    }while(index<(int)*(volatile tU16*)&pObject->box_face_end);
    return pCount;
}

// GetShapePoint

// GetObjectObjectEdgeIntersections

// GetFaceEdgeIntersection

// GetSphereToFacePoints

// GetCornerToFacePoints

// CylinderCheck

// CheckTwoEdges

// WorldPointCollision

// CheckSingleEdgeWithObjectEdges

// GetEdgeToEdgePoints

// SetUpWorkspace

// GetObjectWallForceInfo

// GetObjectCornerToObjectFacePoints

// BoundsOverlapTest

// GetObjectObjectEdgePoints

// GetSphereNonSphereForceInfo

// GetSphereSphereForceInfo

// GetSphereShapeForceInfo

// GetNonSphereNonSphereForceInfo

// GetNonSphereShapeForceInfo

// GetObjectObjectForceInfo

// AddOldForces

// TestShapeEdgeIntersection

// TestObjectOverlap

// TestObjectOverlapIgnoreCloseRelatives

// TestObjectOverlapRecurse

// TestForObjectInSensiblePlace

/* Original x87 operand order preserves the tested distinct NaN payloads. */
static br_scalar C2_HOOK_FASTCALL PhysicsObjectEnergy(tPhysics_object* object) {
    br_scalar energy;
#if defined(_MSC_VER) && defined(_M_IX86)
    br_scalar omega;
    __asm {
        mov esi, object
        lea eax, [esi + 8]
        mov ecx, 3
        fldz
    energy_component:
        fld dword ptr [eax + 60h]
        fld dword ptr [eax + 6ch]
        fst omega
        fld st(1)
        fmul st(0), st(2)
        fld dword ptr [esi + 4]
        fxch st(2)
        fmul omega
        fld dword ptr [eax]
        fxch st(3)
        fmulp st(2), st(0)
        fxch st(2)
        fmulp st(2), st(0)
        fxch st(1)
        faddp st(1), st(0)
        add eax, 4
        dec ecx
        faddp st(2), st(0)
        fstp st(0)
        jnz energy_component
        fstp energy
    }
#elif defined(__GNUC__) && defined(__i386__)
    br_scalar omega;
    br_scalar* component = object->I.v;
    int count = 3;
    __asm__ volatile(
        "fldz; 1: flds 0x60(%[p]); flds 0x6c(%[p]); fsts %[omega]; "
        "fld %%st(1); fmul %%st(2),%%st; flds 4(%[object]); fxch %%st(2); "
        "fmuls %[omega]; flds (%[p]); fxch %%st(3); fmulp %%st,%%st(2); "
        "fxch %%st(2); fmulp %%st,%%st(2); fxch %%st(1); faddp %%st,%%st(1); "
        "add $4,%[p]; dec %[n]; faddp %%st,%%st(2); fstp %%st(0); jnz 1b; fstps %[energy]"
        : [energy] "=m" (energy), [omega] "=m" (omega), [p] "+&a" (component), [n] "+&c" (count)
        : [object] "r" (object)
        : "memory", "cc", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)");
#else
    double total = 0.;
    int i;
    for (i = 0; i < 3; i++) {
        double velocity_squared = (double)object->v.v[i] * object->v.v[i];
        double omega_squared = (double)object->omega.v[i] * object->omega.v[i];
        total += (double)object->M * velocity_squared + (double)object->I.v[i] * omega_squared;
    }
    energy = (br_scalar)total;
#endif
    return energy;
}

static br_scalar C2_HOOK_FASTCALL PhysicsAccumulateEnergy(br_scalar total, br_scalar energy, br_scalar children) {
    br_scalar result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        fld children
        fld energy
        fadd total
        faddp st(1), st(0)
        fstp result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile("flds %1; flds %2; fadds %3; faddp %%st,%%st(1); fstps %0"
        : "=m" (result) : "m" (children), "m" (energy), "m" (total) : "st", "st(1)");
#else
    volatile double subtotal = (double)energy + total;

    result = (br_scalar)(subtotal + children);
#endif
    return result;
}

// FUNCTION: CARMA2_HW 0x004b7290
br_scalar C2_HOOK_FASTCALL HierarchyCalcEnergy(tPhysics_object* pObject) {
    br_scalar total = 0.f;

    while (pObject != NULL) {
        br_scalar energy = PhysicsObjectEnergy(pObject);
        br_scalar children = HierarchyCalcEnergy(pObject->child);
        total = PhysicsAccumulateEnergy(total, energy, children);
        pObject = pObject->next;
    }
    return total;
}

// CheckCollisionList

/* MSVC5 can return the pre-spill ST0 value from a scalar-return C helper. */
static double C2_HOOK_CDECL PhysicsJointScalar(double pValue) {
    br_scalar scalar;
    double rounded;

#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        fld pValue
        fstp scalar
        fld scalar
        fstp rounded
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile("fldl %2; fstps %0; flds %0; fstpl %1"
        : "=m" (scalar), "=m" (rounded) : "m" (pValue) : "st");
#else
    volatile br_scalar stored = (br_scalar)pValue;

    scalar = stored;
    rounded = scalar;
#endif
    return rounded;
}

/* SSE sums choose a different operand for distinct NaN payloads. */
static double C2_HOOK_CDECL PhysicsJointSum(double pA, double pB) {
    double result;

#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        fld pA
        fld pB
        faddp st(1), st(0)
        fstp result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile("fldl %1; fldl %2; faddp; fstpl %0"
        : "=m" (result) : "m" (pA), "m" (pB) : "st", "st(1)");
#else
    result = pA + pB;
#endif
    return result;
}

static double C2_HOOK_CDECL PhysicsJointProduct(const br_scalar* pA, const br_scalar* pB) {
    double result;

#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, pA
        mov edx, pB
        fld dword ptr [eax]
        fld dword ptr [edx]
        fmulp st(1), st(0)
        fstp result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile("flds %1; flds %2; fmulp; fstpl %0"
        : "=m" (result) : "m" (*pA), "m" (*pB) : "st", "st(1)");
#else
    result = (double)*pA * *pB;
#endif
    return result;
}

static double C2_HOOK_CDECL PhysicsJointProductMemory(const br_scalar* pA, const br_scalar* pB) {
    double result;

#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, pA
        mov edx, pB
        fld dword ptr [eax]
        fmul dword ptr [edx]
        fstp result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile("flds %1; fmuls %2; fstpl %0"
        : "=m" (result) : "m" (*pA), "m" (*pB) : "st");
#else
    result = (double)*pA * *pB;
#endif
    return result;
}

static double C2_HOOK_CDECL PhysicsJointAddScalar(double pValue, const br_scalar* pScalar) {
    double result;

#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, pScalar
        fld pValue
        fadd dword ptr [eax]
        fstp result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile("fldl %1; fadds %2; fstpl %0"
        : "=m" (result) : "m" (pValue), "m" (*pScalar) : "st");
#else
    result = pValue + *pScalar;
#endif
    return result;
}

static void C2_HOOK_CDECL PhysicsJointVelocityA(br_scalar* pOutput, const tPhysics_joint_force* pForce, const tPhysics_object* pObject, int pTorque) {
    const volatile tPhysics_joint_force* force = pForce;
    const volatile tPhysics_object* object = pObject;
    br_scalar x;
    br_scalar y;
    br_scalar z;
    double value;

    if (pTorque) {
        value = PhysicsJointSum(PhysicsJointSum(PhysicsJointProduct(&pObject->omega.v[1], &pForce->direction_a.v[1]), PhysicsJointProduct(&pForce->direction_a.v[2], &pObject->omega.v[2])), PhysicsJointProductMemory(&pObject->omega.v[0], &pForce->direction_a.v[0]));
    } else {
        x = (br_scalar)PhysicsJointScalar(PhysicsJointAddScalar((double)force->point_a.v[2] * object->omega.v[1] - (double)force->point_a.v[1] * object->omega.v[2], &pObject->velocity_car_space.v[0]));
        y = (br_scalar)PhysicsJointScalar(PhysicsJointAddScalar((double)force->point_a.v[0] * object->omega.v[2] - (double)force->point_a.v[2] * object->omega.v[0], &pObject->velocity_car_space.v[1]));
        z = (br_scalar)PhysicsJointScalar(PhysicsJointAddScalar((double)force->point_a.v[1] * object->omega.v[0] - (double)force->point_a.v[0] * object->omega.v[1], &pObject->velocity_car_space.v[2]));
        value = PhysicsJointSum(PhysicsJointSum(PhysicsJointProductMemory(&pForce->direction_a.v[1], &y), PhysicsJointProductMemory(&pForce->direction_a.v[2], &z)), PhysicsJointProductMemory(&pForce->direction_a.v[0], &x));
    }
    *pOutput = (br_scalar)PhysicsJointScalar(-value);
}

/* FADD32 keeps the original tie-breaking between opposite-sign NaNs. */
static void C2_HOOK_CDECL PhysicsJointAddVelocity(br_scalar* pOutput, double pValue) {
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, pOutput
        fld pValue
        fadd dword ptr [eax]
        fstp dword ptr [eax]
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile("fldl %1; fadds %0; fstps %0"
        : "+m" (*pOutput) : "m" (pValue) : "st");
#else
    *pOutput = (br_scalar)(pValue + *pOutput);
#endif
}

static void C2_HOOK_CDECL PhysicsJointVelocityB(br_scalar* pOutput, const tPhysics_joint_force* pForce, const tPhysics_object* pObject, int pTorque) {
    const volatile tPhysics_joint_force* force = pForce;
    const volatile tPhysics_object* object = pObject;
    br_scalar x;
    double y;
    br_scalar z;
    double value;

    if (pTorque) {
        value = PhysicsJointSum(PhysicsJointSum(PhysicsJointProduct(&pObject->omega.v[1], &pForce->direction_b.v[1]), PhysicsJointProduct(&pForce->direction_b.v[0], &pObject->omega.v[0])), PhysicsJointProductMemory(&pObject->omega.v[2], &pForce->direction_b.v[2]));
    } else {
        x = (br_scalar)PhysicsJointScalar(PhysicsJointAddScalar((double)force->point_b.v[2] * object->omega.v[1] - (double)force->point_b.v[1] * object->omega.v[2], &pObject->velocity_car_space.v[0]));
        y = PhysicsJointAddScalar((double)force->point_b.v[0] * object->omega.v[2] - (double)force->point_b.v[2] * object->omega.v[0], &pObject->velocity_car_space.v[1]);
        z = (br_scalar)PhysicsJointScalar(PhysicsJointAddScalar((double)force->point_b.v[1] * object->omega.v[0] - (double)force->point_b.v[0] * object->omega.v[1], &pObject->velocity_car_space.v[2]));
        value = PhysicsJointSum(PhysicsJointSum(PhysicsJointProductMemory(&pForce->direction_b.v[0], &x), PhysicsJointProductMemory(&pForce->direction_b.v[2], &z)), (double)force->direction_b.v[1] * y);
    }
    PhysicsJointAddVelocity(pOutput, value);
}

// FUNCTION: CARMA2_HW 0x004b77b0
void C2_HOOK_FASTCALL MakeJointVelocitiesVector(br_scalar* pOutput, tPhysics_joint_force* const* pForces, int pCount) {
    tPhysics_joint_force* const volatile* forces = pForces;
    tPhysics_joint_force* force;
    tPhysics_object* object;
    int torque;

    while (pCount > 0) {
        force = *forces;
        torque = force->type == 11;
        object = force->object_a;
        PhysicsJointVelocityA(pOutput, force, object, torque);
        force = *forces;
        object = force->object_b;
        if (object != NULL) {
            PhysicsJointVelocityB(pOutput, force, object, torque);
        }
        ++forces;
        ++pOutput;
        --pCount;
    }
}

// ApplySingleForce

// SkipForces

// ScaleUpObjectMI

// CheckForTwang

// FUNCTION: CARMA2_HW 0x004b8c20
void C2_HOOK_FASTCALL CreateJointForce(tPhysics_object* pObject, tPhysics_object* pPartner, tPhysics_joint_force* pForce, const br_vector3* pDirection) {
    br_vector3 direction;

    pForce->field_0 = 0;
    pForce->object_a = pObject;
    pForce->object_b = pPartner;
    pForce->field_c = 0;
    PhysicsSubtractScalar(&pForce->point_a.v[0], &pObject->physics_joint1->field_0x08.v[0], &pObject->cmpos.v[0]);
    PhysicsSubtractScalar(&pForce->point_a.v[1], &pObject->physics_joint1->field_0x08.v[1], &pObject->cmpos.v[1]);
    PhysicsSubtractScalar(&pForce->point_a.v[2], &pObject->physics_joint1->field_0x08.v[2], &pObject->cmpos.v[2]);
    PhysicsSubtractScalar(&pForce->point_b.v[0], &pObject->physics_joint1->field_0x14.v[0], &pPartner->cmpos.v[0]);
    PhysicsSubtractScalar(&pForce->point_b.v[1], &pObject->physics_joint1->field_0x14.v[1], &pPartner->cmpos.v[1]);
    PhysicsSubtractScalar(&pForce->point_b.v[2], &pObject->physics_joint1->field_0x14.v[2], &pPartner->cmpos.v[2]);
    PhysicsCopyWord(&pForce->direction_a.v[0], &pDirection->v[0]);
    PhysicsCopyWord(&pForce->direction_a.v[1], &pDirection->v[1]);
    PhysicsCopyWord(&pForce->direction_a.v[2], &pDirection->v[2]);
    BrMatrix34ApplyV(&direction, pDirection, &pObject->transform_matrix);
    BrMatrix34TApplyV(&pForce->direction_b, &direction, &pPartner->transform_matrix);
    pForce->type = 10;
}

// FUNCTION: CARMA2_HW 0x004b8e10
void C2_HOOK_FASTCALL CreateJointTorque(tPhysics_object* pObject, tPhysics_object* pPartner, tPhysics_joint_force* pForce, const br_vector3* pDirection) {
    br_vector3 direction;

    pForce->field_0 = 0;
    pForce->object_a = pObject;
    pForce->object_b = pPartner;
    PhysicsSubtractScalar(&pForce->point_a.v[0], &pObject->physics_joint1->field_0x08.v[0], &pObject->cmpos.v[0]);
    PhysicsSubtractScalar(&pForce->point_a.v[1], &pObject->physics_joint1->field_0x08.v[1], &pObject->cmpos.v[1]);
    PhysicsSubtractScalar(&pForce->point_a.v[2], &pObject->physics_joint1->field_0x08.v[2], &pObject->cmpos.v[2]);
    PhysicsSubtractScalar(&pForce->point_b.v[0], &pObject->physics_joint1->field_0x14.v[0], &pPartner->cmpos.v[0]);
    PhysicsSubtractScalar(&pForce->point_b.v[1], &pObject->physics_joint1->field_0x14.v[1], &pPartner->cmpos.v[1]);
    PhysicsSubtractScalar(&pForce->point_b.v[2], &pObject->physics_joint1->field_0x14.v[2], &pPartner->cmpos.v[2]);
    PhysicsCopyWord(&pForce->direction_a.v[0], &pDirection->v[0]);
    PhysicsCopyWord(&pForce->direction_a.v[1], &pDirection->v[1]);
    PhysicsCopyWord(&pForce->direction_a.v[2], &pDirection->v[2]);
    BrMatrix34ApplyV(&direction, &pForce->direction_a, &pObject->transform_matrix);
    BrMatrix34TApplyV(&pForce->direction_b, &direction, &pPartner->transform_matrix);
    pForce->type = 11;
}

// GetJointForces

// GetUniversalRotationAngle

// GetObjectsParent

// GetJointLimits

// GetListJointLimits

// GetGroupJointLimits

// LimitOmega

// CapOmega

// ListLimitOmega

// GroupLimitOmega

// PositionWorldJointedObject

// TranslateAndRotatePartialObject

// TranslateAndRotateJointedObject

static tU32 C2_HOOK_CDECL PhysicsJointWorkState(const tPhysics_object* pObject) {
    tU32 value;

    memcpy(&value, (const char*)pObject + 0x1e4, sizeof(value));
    return value;
}

static void C2_HOOK_CDECL PhysicsSetJointWorkState(tPhysics_object* pObject, tU32 pValue) {
    memcpy((char*)pObject + 0x1e4, &pValue, sizeof(pValue));
}

// FUNCTION: CARMA2_HW 0x004b9650
int C2_HOOK_FASTCALL CountJointForces(tPhysics_object* pObject, tPhysics_object* pRoot) {
    tPhysics_joint* joint;
    tPhysics_object* child;
    tU32 count = 0;

    joint = pObject->physics_joint1;
    pObject->field_0x230 = pRoot;
    PhysicsSetJointWorkState(pObject, 0);
    if (joint != NULL && !(pObject->flags & 0x400)) {
        switch (joint->type) {
        case 0:
            break;
        case 2:
            count = 4;
            break;
        case 3:
            count = 3;
            break;
        case 4:
            if (PhysicsJointWorkState(pRoot) == 0) {
                count = 2;
                PhysicsSetJointWorkState(pObject, 0xffffffffu);
                PhysicsSetJointWorkState(pRoot, 0xffffffffu);
                break;
            }
            joint->type = (tPhysics_joint_type)1;
            /* fall through */
        case 1:
            count = 5;
            break;
        default:
            PhysicsError((tPhysicsError)8, &gPhysics_empty_message);
        }
    }
    if (pObject->physics_joint2 != NULL) {
        switch (pObject->physics_joint2->type) {
        case 2:
            count += 4;
            break;
        case 3:
            count += 3;
            break;
        case 5:
            count += 2;
            break;
        default:
            break;
        }
    }
    if (!(pObject->flags & 0x400)) {
        pRoot = pObject;
    }
    for (child = pObject->child; child != NULL; child = child->next) {
        count += (tU32)CountJointForces(child, pRoot);
    }
    return (int)count;
}

// MoveJointedObject

// TellPhysicsJointsHaveChanged

// CombineBounds

// GetWorldBoundingBoxForObjects

// FUNCTION: CARMA2_HW 0x004b9ce0
void C2_HOOK_FASTCALL GetWorldBoundingBoxForObject(tPhysics_object* pObject, br_bounds* pBounds) {
    br_bounds old_bounds;
    tPhysics_object* child;
    int i;

    GetNewBoundingBox(&pObject->field_0xf4, &pObject->bb1, &pObject->transform_matrix);
    GetNewBoundingBox(&old_bounds, &pObject->bb1, &pObject->actor->t.t.mat);
    for (i = 0; i < 3; i++) {
        if (PhysicsGreaterOrdered(&pObject->field_0xf4.min.v[i], &old_bounds.min.v[i])) {
            memcpy(&pObject->field_0xf4.min.v[i], &old_bounds.min.v[i], sizeof(br_scalar));
        }
        if (PhysicsLessOrUnordered(&pObject->field_0xf4.max.v[i], &old_bounds.max.v[i])) {
            memcpy(&pObject->field_0xf4.max.v[i], &old_bounds.max.v[i], sizeof(br_scalar));
        }
        pObject->field_0xf4.min.v[i] = (br_scalar)((double)pObject->field_0xf4.min.v[i] - .0025);
        pObject->field_0xf4.max.v[i] = (br_scalar)((double)pObject->field_0xf4.max.v[i] - -.0025);
    }
    memcpy(&pObject->field_0x10c, &pObject->field_0xf4, sizeof(br_bounds));
    for (child = pObject->child; child != NULL; child = child->next) {
        GetWorldBoundingBoxForObject(child, &pObject->field_0x10c);
    }
    if (pBounds != NULL) {
        for (i = 0; i < 3; i++) {
            if (PhysicsLessOrUnordered(&pObject->field_0x10c.min.v[i], &pBounds->min.v[i])) {
                memcpy(&pBounds->min.v[i], &pObject->field_0x10c.min.v[i], sizeof(br_scalar));
            }
            if (PhysicsGreaterOrdered(&pObject->field_0x10c.max.v[i], &pBounds->max.v[i])) {
                memcpy(&pBounds->max.v[i], &pObject->field_0x10c.max.v[i], sizeof(br_scalar));
            }
        }
    } else {
        double extent = (double)pObject->field_0x10c.max.v[0] - pObject->transform_matrix.m[3][0];
        if (!(gPhysics_bounds_max_x > extent)) {
            gPhysics_bounds_max_x = (br_scalar)extent;
        }
        extent = (double)pObject->field_0x10c.min.v[0] - pObject->transform_matrix.m[3][0];
        if (gPhysics_bounds_min_x >= extent) {
            gPhysics_bounds_min_x = (br_scalar)extent;
        }
    }
}

// MatricesAreIdentical

// InternalPrepareObject

// PrepareObject

static int C2_HOOK_FASTCALL PhysicsScalarNaN(const br_scalar* value) {
    tU32 bits;

    memcpy(&bits, value, sizeof(bits));
    return (bits & 0x7fffffffu) > 0x7f800000u;
}

static int C2_HOOK_FASTCALL PhysicsLessOrUnordered(const br_scalar* a, const br_scalar* b) {
    return *a < *b || PhysicsScalarNaN(a) || PhysicsScalarNaN(b);
}

static int C2_HOOK_FASTCALL PhysicsGreaterOrdered(const br_scalar* a, const br_scalar* b) {
    return *a > *b && !PhysicsScalarNaN(a) && !PhysicsScalarNaN(b);
}

static br_scalar C2_HOOK_FASTCALL PhysicsScalar(double value) {
    volatile br_scalar rounded = (br_scalar)value;

    return rounded;
}

static void C2_HOOK_FASTCALL PhysicsScalarStore(br_scalar* output, const br_scalar* input) {
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, output
        mov ecx, input
        fld dword ptr [ecx]
        fstp dword ptr [eax]
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile("flds %1; fstps %0" : "=m" (*output) : "m" (*input) : "st");
#else
    volatile double value = *input;

    *output = (br_scalar)value;
#endif
}

// FUNCTION: CARMA2_HW 0x004c2060
void C2_HOOK_FASTCALL PrepareForCollisions(tPhysics_object* pObject, tWorld_callbacks* pWorld_callbacks) {
    tBounds bounds;
    br_bounds transformed;
    br_matrix34 inverse;
    br_matrix34 delta;
    br_matrix34 squared;
    br_matrix34 cubed;
    br_matrix34* actor_matrix;
    br_vector3 movement;
    br_scalar zero = 0.f;
    double high;
    int i;
    int valid_cache;
    int found;

    if (pObject->flags & 0x40) {
        pObject->box_face_start = pObject->box_face_end;
        return;
    }
    if ((pObject->flags & 2) && pObject->parent != NULL) {
        pObject->box_face_start = pObject->parent->box_face_start;
        pObject->box_face_end = pObject->parent->box_face_end;
        pObject->box_face_ref = pObject->parent->box_face_ref;
        return;
    }
    valid_cache = (tU32)pObject->box_face_ref == (tU32)gFace_num__car
        || ((tU32)pObject->box_face_ref == (tU32)gFace_num__car - 1u && (int)pObject->box_face_start > gFace_count);
    if (pObject->flags & 1) {
        if (valid_cache
            && PhysicsGreaterOrdered(&pObject->field_0x10c.min.v[0], &pObject->field_0x124.min.v[0])
            && PhysicsGreaterOrdered(&pObject->field_0x10c.min.v[1], &pObject->field_0x124.min.v[1])
            && PhysicsGreaterOrdered(&pObject->field_0x10c.min.v[2], &pObject->field_0x124.min.v[2])
            && PhysicsLessOrUnordered(&pObject->field_0x10c.max.v[0], &pObject->field_0x124.max.v[0])
            && PhysicsLessOrUnordered(&pObject->field_0x10c.max.v[1], &pObject->field_0x124.max.v[1])
            && PhysicsLessOrUnordered(&pObject->field_0x10c.max.v[2], &pObject->field_0x124.max.v[2])) {
            return;
        }
        for (i = 0; i < 3; i++) {
            movement.v[i] = PhysicsScalar((double)pObject->v.v[i] * 0.12f);
        }
        for (i = 0; i < 3; i++) {
            high = (double)pObject->field_0x10c.max.v[i] - -0.15;
            bounds.original_bounds.min.v[i] = PhysicsScalar((double)pObject->field_0x10c.min.v[i] - 0.15);
            bounds.original_bounds.max.v[i] = PhysicsScalar(high);
            if (PhysicsLessOrUnordered(&movement.v[i], &zero)) {
                bounds.original_bounds.min.v[i] = PhysicsScalar((double)bounds.original_bounds.min.v[i] + movement.v[i]);
            } else {
                bounds.original_bounds.max.v[i] = PhysicsScalar(high + movement.v[i]);
            }
            pObject->field_0x124.min.v[i] = PhysicsScalar((double)bounds.original_bounds.min.v[i] - -0.005);
            pObject->field_0x124.max.v[i] = PhysicsScalar((double)bounds.original_bounds.max.v[i] - 0.005);
        }
        BrMatrix34Identity(&inverse);
        bounds.mat = &inverse;
    } else {
        actor_matrix = &pObject->actor->t.t.mat;
        if (valid_cache) {
            BrMatrix34Mul(&squared, actor_matrix, &pObject->field_0x144);
            GetNewBoundingBox(&transformed, &pObject->bb2, &squared);
            if (PhysicsLessOrUnordered(&transformed.max.v[0], &pObject->field_0x124.max.v[0])
                && PhysicsLessOrUnordered(&transformed.max.v[1], &pObject->field_0x124.max.v[1])
                && PhysicsLessOrUnordered(&transformed.max.v[2], &pObject->field_0x124.max.v[2])
                && PhysicsGreaterOrdered(&transformed.min.v[0], &pObject->field_0x124.min.v[0])
                && PhysicsGreaterOrdered(&transformed.min.v[1], &pObject->field_0x124.min.v[1])
                && PhysicsGreaterOrdered(&transformed.min.v[2], &pObject->field_0x124.min.v[2])) {
                return;
            }
        }
        BrMatrix34LPInverse(&inverse, actor_matrix);
        BrMatrix34Mul(&delta, &pObject->transform_matrix, &inverse);
        GetNewBoundingBox(&bounds.original_bounds, &pObject->bb2, &delta);
        for (i = 0; i < 3; i++) {
            PhysicsScalarStore(&bounds.original_bounds.min.v[i], PhysicsLessOrUnordered(&bounds.original_bounds.min.v[i], &pObject->bb2.min.v[i]) ? &bounds.original_bounds.min.v[i] : &pObject->bb2.min.v[i]);
            PhysicsScalarStore(&bounds.original_bounds.max.v[i], PhysicsGreaterOrdered(&bounds.original_bounds.max.v[i], &pObject->bb2.max.v[i]) ? &bounds.original_bounds.max.v[i] : &pObject->bb2.max.v[i]);
            bounds.original_bounds.min.v[i] = PhysicsScalar((double)bounds.original_bounds.min.v[i] - 0.0025f);
            bounds.original_bounds.max.v[i] = PhysicsScalar((double)bounds.original_bounds.max.v[i] - -0.0025f);
        }
        BrMatrix34Mul(&squared, &delta, &delta);
        BrMatrix34Mul(&cubed, &squared, &delta);
        BrMatrix34LPInverse(&squared, &cubed);
        GetNewBoundingBox(&transformed, &pObject->bb2, &squared);
        for (i = 0; i < 3; i++) {
            PhysicsScalarStore(&bounds.original_bounds.min.v[i], PhysicsLessOrUnordered(&bounds.original_bounds.min.v[i], &transformed.min.v[i]) ? &bounds.original_bounds.min.v[i] : &transformed.min.v[i]);
            PhysicsScalarStore(&bounds.original_bounds.max.v[i], PhysicsGreaterOrdered(&bounds.original_bounds.max.v[i], &transformed.max.v[i]) ? &bounds.original_bounds.max.v[i] : &transformed.max.v[i]);
            bounds.original_bounds.min.v[i] = PhysicsScalar((double)bounds.original_bounds.min.v[i] - 0.02f);
            bounds.original_bounds.max.v[i] = PhysicsScalar((double)bounds.original_bounds.max.v[i] - -0.02f);
        }
        memcpy(&pObject->field_0x124, &bounds.original_bounds, sizeof(pObject->field_0x124));
        BrMatrix34Copy(&pObject->field_0x144, &inverse);
        bounds.mat = actor_matrix;
    }
    pObject->box_face_start = (tU16)gFace_count;
    gPling_face = NULL;
    gActorBoxPick_StopGroovidelics = ((tU8)~(tU8)pObject->flags) >> 7;
    if (pWorld_callbacks != NULL && pWorld_callbacks->find_faces_in_box != NULL) {
        found = pWorld_callbacks->find_faces_in_box(&bounds, &gFace_list__car[gFace_count], (int)(300u - (tU32)gFace_count), pWorld_callbacks);
        gFace_count = (int)((tU32)gFace_count + (tU32)found);
        if (gFace_count >= 300) {
            pObject->box_face_start = 0;
            gFace_count = pWorld_callbacks->find_faces_in_box(&bounds, gFace_list__car, 300, pWorld_callbacks);
            gFace_num__car = (int)((tU32)gFace_num__car + 1u);
        }
    }
    pObject->box_face_end = (tU16)gFace_count;
    pObject->box_face_ref = (tU16)gFace_num__car;
    gActorBoxPick_StopGroovidelics = 1;
    if (pWorld_callbacks != NULL && pWorld_callbacks->new_face_list != NULL) {
        pWorld_callbacks->new_face_list(pObject, (undefined4*)gPling_face);
    }
    if (pObject->flags & 0x80) {
        pObject->box_face_end = pObject->box_face_start;
    }
}

// CheckPointVelocities

// FUNCTION: CARMA2_HW 0x004b9eb0
void C2_HOOK_FASTCALL SetListDoingNothingFlag(tPhysics_object* pObject, tU8 pFlag) {
    while (pObject != NULL) {
        pObject->disable_move_rotate = pFlag;
        pObject->field_0x1df = 0;
        if (pObject->child != NULL) {
            SetListDoingNothingFlag(pObject->child, pFlag);
        }
        pObject = pObject->next;
    }
}

// FUNCTION: CARMA2_HW 0x004b9e40
void C2_HOOK_FASTCALL SetGroupDoingNothingFlag(tPhysics_object* pObject, tU8 pFlag) {
    tPhysics_object* item = pObject;

    pObject->disable_move_rotate = pFlag;
    do {
        SetHierarchyDoingNothingFlag(item, pFlag);
        memcpy(&item, &item->field183_0x1d8, sizeof(item));
    } while (item != NULL && item != pObject);
}

// FUNCTION: CARMA2_HW 0x004b9ef0
void C2_HOOK_FASTCALL SetHierarchyDoingNothingFlag(tPhysics_object* pObject, tU8 pFlag) {
    tPhysics_object* child;

    pObject->disable_move_rotate = pFlag;
    pObject->field_0x1df = 0;
    for (child = pObject->child; child != NULL; child = child->next) {
        child->disable_move_rotate = pFlag;
        child->field_0x1df = 0;
        if (child->child != NULL) {
            SetListDoingNothingFlag(child->child, pFlag);
        }
    }
}

// CheckDoingNothingFlag

// SetGroup

// GroupTogether

// GetPreCalculatedJointForces

// FUNCTION: CARMA2_HW 0x004b9f40
void C2_HOOK_FASTCALL MakeObjectListDoSomething(tPhysics_object* pObject) {
    while (pObject != NULL) {
        if (pObject->disable_move_rotate) {
            pObject->disable_move_rotate = 0;
            pObject->box_face_ref = (tU16)((tU32)gFace_num__car - 2u);
        }
        if (pObject->child != NULL) {
            MakeObjectListDoSomething(pObject->child);
        }
        pObject = pObject->next;
    }
}

// CheckForObjectTouchingAnotherObject

// CheckForObjectListTouchingAnotherObject

// CheckForObjectHierachyTouchingAnotherObject

// CheckForObjectHierachyTouchingObjectList

// MakeObjectDoSomething

// GetReducedList

// ReduceReducedList

// SetObjectTrialPositionType

// FillInTrialWorkspace

// FlagsAreGo

// CollideObjectWithReducedObjectList

// AddObjectToReducedList

// ReducedFindObjectCollisions

// FindObjectCollisions

// DoRigidJoints

// DoInterGroupCollisions

// ReduceEffectOfForces

// DoCollisions

// AddDoubleTorqueToMatrix

/* Original cofactor spills and source reloads preserve x87 rounding and NaN operand order. */
// FUNCTION: CARMA2_HW 0x004be350
void C2_HOOK_FASTCALL DRMatrix33Inverse(br_scalar* result, const br_scalar* pInput) {
    const double one = 1.0;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov ecx, result
        mov edx, pInput
        fld dword ptr [edx + 0x14]
        fld dword ptr [edx + 0x20]
        fld dword ptr [edx + 0x1c]
        fxch st(2)
        fchs
        fxch st(1)
        fmul dword ptr [edx + 0x10]
        fxch st(1)
        fmulp st(2), st(0)
        fxch st(1)
        faddp st(1), st(0)
        fstp dword ptr [ecx]
        fld dword ptr [edx + 8]
        fld dword ptr [edx + 0x20]
        fld dword ptr [edx + 4]
        fld dword ptr [edx + 0x1c]
        fxch st(3)
        fchs
        fxch st(1)
        fmulp st(2), st(0)
        fmulp st(2), st(0)
        fxch st(1)
        faddp st(1), st(0)
        fchs
        fstp dword ptr [ecx + 4]
        fld dword ptr [edx + 8]
        fld dword ptr [edx + 0x14]
        fld dword ptr [edx + 4]
        fld dword ptr [edx + 0x10]
        fxch st(3)
        fchs
        fxch st(1)
        fmulp st(2), st(0)
        fmulp st(2), st(0)
        fld dword ptr [ecx + 4]
        fxch st(2)
        faddp st(1), st(0)
        fxch st(1)
        fld st(1)
        fstp dword ptr [ecx + 8]
        fld dword ptr [edx + 8]
        fld dword ptr [edx]
        fmul dword ptr [ecx]
        fld dword ptr [edx + 4]
        fxch st(2)
        fmul st(0), st(4)
        fxch st(2)
        fmulp st(3), st(0)
        fxch st(1)
        faddp st(1), st(0)
        fxch st(1)
        faddp st(1), st(0)
        fstp st(1)
        fdivr one
        fld dword ptr [ecx]
        fmul st(0), st(1)
        fld dword ptr [ecx + 4]
        fxch st(1)
        fstp dword ptr [ecx]
        fld dword ptr [edx + 0x14]
        fld dword ptr [edx + 0xc]
        fld dword ptr [edx + 0x20]
        fld dword ptr [edx + 0x18]
        fxch st(3)
        fchs
        fxch st(1)
        fmulp st(2), st(0)
        fmulp st(2), st(0)
        fxch st(2)
        fmul st(0), st(3)
        fxch st(1)
        faddp st(2), st(0)
        fld dword ptr [ecx + 8]
        fxch st(2)
        fmul st(0), st(3)
        fxch st(2)
        fmul st(0), st(3)
        fxch st(2)
        fchs
        fstp dword ptr [ecx + 0xc]
        fld dword ptr [edx + 0x10]
        fld dword ptr [edx + 0xc]
        fld dword ptr [edx + 0x1c]
        fld dword ptr [edx + 0x18]
        fxch st(1)
        fmulp st(2), st(0)
        fxch st(2)
        fchs
        fmulp st(2), st(0)
        fxch st(2)
        fstp dword ptr [ecx + 4]
        faddp st(1), st(0)
        fmul st(0), st(2)
        fstp dword ptr [ecx + 0x18]
        fld dword ptr [edx + 8]
        fld dword ptr [edx + 0x20]
        fld dword ptr [edx + 0x18]
        fxch st(2)
        fchs
        fxch st(1)
        fmul dword ptr [edx]
        fxch st(1)
        fmulp st(2), st(0)
        fxch st(1)
        faddp st(1), st(0)
        fmul st(0), st(2)
        fstp dword ptr [ecx + 0x10]
        fld dword ptr [edx + 4]
        fld dword ptr [edx + 0x1c]
        fld dword ptr [edx + 0x18]
        fxch st(2)
        fchs
        fxch st(1)
        fmul dword ptr [edx]
        fxch st(1)
        fmulp st(2), st(0)
        fxch st(2)
        fstp dword ptr [ecx + 8]
        faddp st(1), st(0)
        fmul st(0), st(1)
        fchs
        fstp dword ptr [ecx + 0x1c]
        fld dword ptr [edx + 8]
        fld dword ptr [edx + 0x14]
        fld dword ptr [edx + 0xc]
        fxch st(2)
        fchs
        fxch st(1)
        fmul dword ptr [edx]
        fxch st(1)
        fmulp st(2), st(0)
        fxch st(1)
        faddp st(1), st(0)
        fmul st(0), st(1)
        fchs
        fstp dword ptr [ecx + 0x14]
        fld dword ptr [edx + 4]
        fld dword ptr [edx + 0x10]
        fld dword ptr [edx + 0xc]
        fxch st(2)
        fchs
        fxch st(1)
        fmul dword ptr [edx]
        fxch st(1)
        fmulp st(2), st(0)
        fxch st(1)
        faddp st(1), st(0)
        fmul st(0), st(1)
        fstp dword ptr [ecx + 0x20]
        fstp st(0)
    }
#elif defined(__GNUC__) && defined(__i386__)
    {
        const double* one_ptr = &one;
        __asm__ volatile (
            "movl %[result], %%ecx\n\t"
            "movl %[input], %%edx\n\t"
            "movl %[one], %%eax\n\t"
            ".intel_syntax noprefix\n\t"
            "fld dword ptr [edx + 0x14]\n\t"
            "fld dword ptr [edx + 0x20]\n\t"
            "fld dword ptr [edx + 0x1c]\n\t"
            "fxch st(2)\n\t"
            "fchs\n\t"
            "fxch st(1)\n\t"
            "fmul dword ptr [edx + 0x10]\n\t"
            "fxch st(1)\n\t"
            "fmulp st(2), st(0)\n\t"
            "fxch st(1)\n\t"
            "faddp st(1), st(0)\n\t"
            "fstp dword ptr [ecx]\n\t"
            "fld dword ptr [edx + 8]\n\t"
            "fld dword ptr [edx + 0x20]\n\t"
            "fld dword ptr [edx + 4]\n\t"
            "fld dword ptr [edx + 0x1c]\n\t"
            "fxch st(3)\n\t"
            "fchs\n\t"
            "fxch st(1)\n\t"
            "fmulp st(2), st(0)\n\t"
            "fmulp st(2), st(0)\n\t"
            "fxch st(1)\n\t"
            "faddp st(1), st(0)\n\t"
            "fchs\n\t"
            "fstp dword ptr [ecx + 4]\n\t"
            "fld dword ptr [edx + 8]\n\t"
            "fld dword ptr [edx + 0x14]\n\t"
            "fld dword ptr [edx + 4]\n\t"
            "fld dword ptr [edx + 0x10]\n\t"
            "fxch st(3)\n\t"
            "fchs\n\t"
            "fxch st(1)\n\t"
            "fmulp st(2), st(0)\n\t"
            "fmulp st(2), st(0)\n\t"
            "fld dword ptr [ecx + 4]\n\t"
            "fxch st(2)\n\t"
            "faddp st(1), st(0)\n\t"
            "fxch st(1)\n\t"
            "fld st(1)\n\t"
            "fstp dword ptr [ecx + 8]\n\t"
            "fld dword ptr [edx + 8]\n\t"
            "fld dword ptr [edx]\n\t"
            "fmul dword ptr [ecx]\n\t"
            "fld dword ptr [edx + 4]\n\t"
            "fxch st(2)\n\t"
            "fmul st(0), st(4)\n\t"
            "fxch st(2)\n\t"
            "fmulp st(3), st(0)\n\t"
            "fxch st(1)\n\t"
            "faddp st(1), st(0)\n\t"
            "fxch st(1)\n\t"
            "faddp st(1), st(0)\n\t"
            "fstp st(1)\n\t"
            "fdivr qword ptr [eax]\n\t"
            "fld dword ptr [ecx]\n\t"
            "fmul st(0), st(1)\n\t"
            "fld dword ptr [ecx + 4]\n\t"
            "fxch st(1)\n\t"
            "fstp dword ptr [ecx]\n\t"
            "fld dword ptr [edx + 0x14]\n\t"
            "fld dword ptr [edx + 0xc]\n\t"
            "fld dword ptr [edx + 0x20]\n\t"
            "fld dword ptr [edx + 0x18]\n\t"
            "fxch st(3)\n\t"
            "fchs\n\t"
            "fxch st(1)\n\t"
            "fmulp st(2), st(0)\n\t"
            "fmulp st(2), st(0)\n\t"
            "fxch st(2)\n\t"
            "fmul st(0), st(3)\n\t"
            "fxch st(1)\n\t"
            "faddp st(2), st(0)\n\t"
            "fld dword ptr [ecx + 8]\n\t"
            "fxch st(2)\n\t"
            "fmul st(0), st(3)\n\t"
            "fxch st(2)\n\t"
            "fmul st(0), st(3)\n\t"
            "fxch st(2)\n\t"
            "fchs\n\t"
            "fstp dword ptr [ecx + 0xc]\n\t"
            "fld dword ptr [edx + 0x10]\n\t"
            "fld dword ptr [edx + 0xc]\n\t"
            "fld dword ptr [edx + 0x1c]\n\t"
            "fld dword ptr [edx + 0x18]\n\t"
            "fxch st(1)\n\t"
            "fmulp st(2), st(0)\n\t"
            "fxch st(2)\n\t"
            "fchs\n\t"
            "fmulp st(2), st(0)\n\t"
            "fxch st(2)\n\t"
            "fstp dword ptr [ecx + 4]\n\t"
            "faddp st(1), st(0)\n\t"
            "fmul st(0), st(2)\n\t"
            "fstp dword ptr [ecx + 0x18]\n\t"
            "fld dword ptr [edx + 8]\n\t"
            "fld dword ptr [edx + 0x20]\n\t"
            "fld dword ptr [edx + 0x18]\n\t"
            "fxch st(2)\n\t"
            "fchs\n\t"
            "fxch st(1)\n\t"
            "fmul dword ptr [edx]\n\t"
            "fxch st(1)\n\t"
            "fmulp st(2), st(0)\n\t"
            "fxch st(1)\n\t"
            "faddp st(1), st(0)\n\t"
            "fmul st(0), st(2)\n\t"
            "fstp dword ptr [ecx + 0x10]\n\t"
            "fld dword ptr [edx + 4]\n\t"
            "fld dword ptr [edx + 0x1c]\n\t"
            "fld dword ptr [edx + 0x18]\n\t"
            "fxch st(2)\n\t"
            "fchs\n\t"
            "fxch st(1)\n\t"
            "fmul dword ptr [edx]\n\t"
            "fxch st(1)\n\t"
            "fmulp st(2), st(0)\n\t"
            "fxch st(2)\n\t"
            "fstp dword ptr [ecx + 8]\n\t"
            "faddp st(1), st(0)\n\t"
            "fmul st(0), st(1)\n\t"
            "fchs\n\t"
            "fstp dword ptr [ecx + 0x1c]\n\t"
            "fld dword ptr [edx + 8]\n\t"
            "fld dword ptr [edx + 0x14]\n\t"
            "fld dword ptr [edx + 0xc]\n\t"
            "fxch st(2)\n\t"
            "fchs\n\t"
            "fxch st(1)\n\t"
            "fmul dword ptr [edx]\n\t"
            "fxch st(1)\n\t"
            "fmulp st(2), st(0)\n\t"
            "fxch st(1)\n\t"
            "faddp st(1), st(0)\n\t"
            "fmul st(0), st(1)\n\t"
            "fchs\n\t"
            "fstp dword ptr [ecx + 0x14]\n\t"
            "fld dword ptr [edx + 4]\n\t"
            "fld dword ptr [edx + 0x10]\n\t"
            "fld dword ptr [edx + 0xc]\n\t"
            "fxch st(2)\n\t"
            "fchs\n\t"
            "fxch st(1)\n\t"
            "fmul dword ptr [edx]\n\t"
            "fxch st(1)\n\t"
            "fmulp st(2), st(0)\n\t"
            "fxch st(1)\n\t"
            "faddp st(1), st(0)\n\t"
            "fmul st(0), st(1)\n\t"
            "fstp dword ptr [ecx + 0x20]\n\t"
            "fstp st(0)\n\t"
            ".att_syntax prefix\n\t"
            :
            : [result] "m" (result), [input] "m" (pInput), [one] "m" (one_ptr)
            : "eax", "ecx", "edx", "memory", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)");
    }
#else
#error DRMatrix33Inverse requires the original x86 floating-point behavior
#endif
}

// SetUpQuickHingeData

// InitQuickHinge

// CalcQuickHingeData

/* Keep the sequential cross-product stores, inertia divisions and wide mass reciprocal. */
// FUNCTION: CARMA2_HW 0x004be4b0
void C2_HOOK_FASTCALL SetUpForce(br_vector3* out_omega, br_vector3* out_velocity, const br_vector3* lever, const br_vector3* force, const tPhysics_object* object) {
    const double one = 1.0;
#if defined(__GNUC__) && defined(__i386__)
    const double* one_ptr = &one;
#endif
    if (!(object->flags & 0x800)) {
#if defined(_MSC_VER) && defined(_M_IX86)
        __asm {
            mov eax, out_omega
            mov ecx, lever
            mov edx, force
            mov esi, object
            fld dword ptr [edx + 8]
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx + 8]
            fxch st(1)
            fmulp st(2), st(0)
            fmul dword ptr [edx + 4]
            fsubp st(1), st(0)
            fstp dword ptr [eax]
            fld dword ptr [edx + 8]
            fld dword ptr [ecx + 8]
            fmul dword ptr [edx]
            fxch st(1)
            fmul dword ptr [ecx]
            fsubp st(1), st(0)
            fstp dword ptr [eax + 4]
            fld dword ptr [ecx + 4]
            fld dword ptr [edx + 4]
            fmul dword ptr [ecx]
            fxch st(1)
            fmul dword ptr [edx]
            fsubp st(1), st(0)
            fstp dword ptr [eax + 8]
            fld dword ptr [eax]
            fdiv dword ptr [esi + 8]
            fstp dword ptr [eax]
            fld dword ptr [eax + 4]
            fdiv dword ptr [esi + 12]
            fstp dword ptr [eax + 4]
            fld dword ptr [eax + 8]
            fdiv dword ptr [esi + 16]
            fstp dword ptr [eax + 8]
        }
#elif defined(__GNUC__) && defined(__i386__)
        __asm__ volatile (
            "movl %[out_omega], %%eax\n\t"
            "movl %[lever], %%ecx\n\t"
            "movl %[force], %%edx\n\t"
            "movl %[object], %%esi\n\t"
            ".intel_syntax noprefix\n\t"
            "fld dword ptr [edx + 8]\n\t"
            "fld dword ptr [ecx + 4]\n\t"
            "fld dword ptr [ecx + 8]\n\t"
            "fxch st(1)\n\t"
            "fmulp st(2), st(0)\n\t"
            "fmul dword ptr [edx + 4]\n\t"
            "fsubp st(1), st(0)\n\t"
            "fstp dword ptr [eax]\n\t"
            "fld dword ptr [edx + 8]\n\t"
            "fld dword ptr [ecx + 8]\n\t"
            "fmul dword ptr [edx]\n\t"
            "fxch st(1)\n\t"
            "fmul dword ptr [ecx]\n\t"
            "fsubp st(1), st(0)\n\t"
            "fstp dword ptr [eax + 4]\n\t"
            "fld dword ptr [ecx + 4]\n\t"
            "fld dword ptr [edx + 4]\n\t"
            "fmul dword ptr [ecx]\n\t"
            "fxch st(1)\n\t"
            "fmul dword ptr [edx]\n\t"
            "fsubp st(1), st(0)\n\t"
            "fstp dword ptr [eax + 8]\n\t"
            "fld dword ptr [eax]\n\t"
            "fdiv dword ptr [esi + 8]\n\t"
            "fstp dword ptr [eax]\n\t"
            "fld dword ptr [eax + 4]\n\t"
            "fdiv dword ptr [esi + 12]\n\t"
            "fstp dword ptr [eax + 4]\n\t"
            "fld dword ptr [eax + 8]\n\t"
            "fdiv dword ptr [esi + 16]\n\t"
            "fstp dword ptr [eax + 8]\n\t"
            ".att_syntax prefix\n\t"
            :
            : [out_omega] "m" (out_omega), [lever] "m" (lever), [force] "m" (force), [object] "m" (object)
            : "eax", "ecx", "edx", "esi", "memory", "st", "st(1)", "st(2)");
#else
#error SetUpForce requires the original x86 floating-point behavior
#endif
    } else {
        out_omega->v[0] = 0.f;
        out_omega->v[1] = 0.f;
        out_omega->v[2] = 0.f;
    }
#if defined(_MSC_VER) && defined(_M_IX86)
        __asm {
            mov eax, out_velocity
            mov edx, force
            mov esi, object
            fld dword ptr [esi + 4]
            fdivr one
            fld dword ptr [edx]
            fmul st(0), st(1)
            fstp dword ptr [eax]
            fld dword ptr [edx + 4]
            fmul st(0), st(1)
            fstp dword ptr [eax + 4]
            fld dword ptr [edx + 8]
            fmul st(0), st(1)
            fstp dword ptr [eax + 8]
            fstp st(0)
        }
#elif defined(__GNUC__) && defined(__i386__)
        __asm__ volatile (
            "movl %[out_velocity], %%eax\n\t"
            "movl %[force], %%edx\n\t"
            "movl %[object], %%esi\n\t"
            "movl %[one_ptr], %%ecx\n\t"
            ".intel_syntax noprefix\n\t"
            "fld dword ptr [esi + 4]\n\t"
            "fdivr qword ptr [ecx]\n\t"
            "fld dword ptr [edx]\n\t"
            "fmul st(0), st(1)\n\t"
            "fstp dword ptr [eax]\n\t"
            "fld dword ptr [edx + 4]\n\t"
            "fmul st(0), st(1)\n\t"
            "fstp dword ptr [eax + 4]\n\t"
            "fld dword ptr [edx + 8]\n\t"
            "fmul st(0), st(1)\n\t"
            "fstp dword ptr [eax + 8]\n\t"
            "fstp st(0)\n\t"
            ".att_syntax prefix\n\t"
            :
            : [out_velocity] "m" (out_velocity), [force] "m" (force), [object] "m" (object), [one_ptr] "m" (one_ptr)
            : "eax", "ecx", "edx", "esi", "memory", "st", "st(1)", "st(2)");
#else
#error SetUpForce requires the original x86 floating-point behavior
#endif
 }

/* Hinge response workspace. */
typedef struct {
    tU32 selector;
    tPhysics_object* partner;
    br_scalar response[3][3];
    tU8 padding_0x2c[12];
    br_matrix34 relative_matrix;
} tForce_workspace_view;
typedef char force_workspace_layout[sizeof(tForce_workspace_view)==104 && offsetof(tForce_workspace_view,partner)==4 && offsetof(tForce_workspace_view,response)==8 && offsetof(tForce_workspace_view,relative_matrix)==0x38 ? 1:-1];

static tU8* ForceBytes(const void* pBase, unsigned pOffset) {
    return (tU8*)(uintptr_t)pBase + pOffset;
}
static uintptr_t ForcePointerAt(const void* pBase, unsigned pOffset) {
    volatile const tU32* field=(volatile const tU32*)ForceBytes(pBase,pOffset);
    return *field;
}
#if defined(_MSC_VER) && _MSC_VER >= 1300
__declspec(noinline)
#endif
static br_scalar ForceScalarRound(double pValue) {
    volatile union {br_scalar scalar;tU32 bits;} stored;
    union {br_scalar scalar;tU32 bits;} result;
    stored.scalar=(br_scalar)pValue;result.bits=stored.bits;return result.scalar;
}

/* Retained physical comparisons demonstrate modern SSE NaN-sign changes
 * in combined additions/subtractions. Keep these small wide operations x87. */
static double C2_HOOK_CDECL PartnerAddWide(double a,double b) {
 double result;
#if defined(_MSC_VER) && defined(_M_IX86)
 __asm {
 fld qword ptr a
 fadd qword ptr b
 fstp qword ptr result
 }
#elif defined(__GNUC__) && defined(__i386__)
 __asm__ __volatile__("fldl %1; faddl %2; fstpl %0" : "=m"(result) : "m"(a),"m"(b) : "st");
#else
 result=a+b;
#endif
 return result;
}
static double C2_HOOK_CDECL PartnerSubtractWide(double a, double b) {
 double result;
#if defined(_MSC_VER) && defined(_M_IX86)
 __asm {
 fld qword ptr a
 fsub qword ptr b
 fstp qword ptr result
 }
#elif defined(__GNUC__) && defined(__i386__)
 __asm__ __volatile__("fldl %1; fsubl %2; fstpl %0" : "=m"(result) : "m"(a),"m"(b) : "st");
#else
 result=a-b;
#endif
 return result;
}
static double C2_HOOK_CDECL PartnerNegateWide(double a) {
 double result;
#if defined(_MSC_VER) && defined(_M_IX86)
 __asm {
 fld qword ptr a
 fchs
 fstp qword ptr result
 }
#elif defined(__GNUC__) && defined(__i386__)
 __asm__ __volatile__("fldl %1; fchs; fstpl %0" : "=m"(result) : "m"(a) : "st");
#else
 result=-a;
#endif
 return result;
}

static double C2_HOOK_CDECL PartnerAddScalar(double a,const br_scalar* b) {
 double result;
#if defined(_MSC_VER) && defined(_M_IX86)
 __asm {
 mov eax,b
 fld qword ptr a
 fadd dword ptr [eax]
 fstp qword ptr result
 }
#elif defined(__GNUC__) && defined(__i386__)
 __asm__ __volatile__("fldl %1; fadds %2; fstpl %0" : "=m"(result) : "m"(a),"m"(*b) : "st");
#else
 result=a+*b;
#endif
 return result;
}
static const br_scalar* PartnerScalarPtr(const void* base,unsigned offset) {
 return (const br_scalar*)ForceBytes(base,offset);
}
static double C2_HOOK_CDECL PartnerDifferenceScalars(const br_scalar* a,const br_scalar* b) {
 double result;
#if defined(_MSC_VER) && defined(_M_IX86)
 __asm {
 mov eax,a
 mov ecx,b
 fld dword ptr [eax]
 fsub dword ptr [ecx]
 fstp qword ptr result
 }
#elif defined(__GNUC__) && defined(__i386__)
 __asm__ __volatile__("flds %1; fsubs %2; fstpl %0" : "=m"(result) : "m"(*a),"m"(*b) : "st");
#else
 result=(double)*a - *b;
#endif
 return result;
}
static double C2_HOOK_CDECL PartnerDivideScalars(const br_scalar* a,const br_scalar* b) {
 double result;
#if defined(_MSC_VER) && defined(_M_IX86)
 __asm {
 mov eax,a
 mov ecx,b
 fld dword ptr [eax]
 fdiv dword ptr [ecx]
 fstp qword ptr result
 }
#elif defined(__GNUC__) && defined(__i386__)
 __asm__ __volatile__("flds %1; fdivs %2; fstpl %0" : "=m"(result) : "m"(*a),"m"(*b) : "st");
#else
 result=(double)*a / *b;
#endif
 return result;
}
static double C2_HOOK_CDECL PartnerReverseDivideScalars(const br_scalar* a,const br_scalar* b) {
 double result;
#if defined(_MSC_VER) && defined(_M_IX86)
 __asm {
 mov eax,a
 mov ecx,b
 fld dword ptr [eax]
 fdivr dword ptr [ecx]
 fstp qword ptr result
 }
#elif defined(__GNUC__) && defined(__i386__)
 __asm__ __volatile__("flds %1; fdivrs %2; fstpl %0" : "=m"(result) : "m"(*a),"m"(*b) : "st");
#else
 result=*b / (double)*a;
#endif
 return result;
}
static double C2_HOOK_CDECL PartnerReverseSubtractScalar(double a,const br_scalar* b) {
 double result;
#if defined(_MSC_VER) && defined(_M_IX86)
 __asm {
 mov eax,b
 fld qword ptr a
 fsubr dword ptr [eax]
 fstp qword ptr result
 }
#elif defined(__GNUC__) && defined(__i386__)
 __asm__ __volatile__("fldl %1; fsubrs %2; fstpl %0" : "=m"(result) : "m"(a),"m"(*b) : "st");
#else
 result=*b-a;
#endif
 return result;
}

static double C2_HOOK_CDECL PartnerMultiplyWide(double a, double b) {
 double result;
#if defined(_MSC_VER) && defined(_M_IX86)
 __asm {
 fld qword ptr a
 fmul qword ptr b
 fstp qword ptr result
 }
#elif defined(__GNUC__) && defined(__i386__)
 __asm__ __volatile__("fldl %1; fmull %2; fstpl %0" : "=m"(result) : "m"(a),"m"(b) : "st");
#else
 result=(double)a * b;
#endif
 return result;
}
static double C2_HOOK_CDECL PartnerMultiplyScalars(const br_scalar* a,const br_scalar* b) {
 double result;
#if defined(_MSC_VER) && defined(_M_IX86)
 __asm {
 mov eax,a
 fld dword ptr [eax]
 mov ecx,b
 fmul dword ptr [ecx]
 fstp qword ptr result
 }
#elif defined(__GNUC__) && defined(__i386__)
 __asm__ __volatile__("flds %1; fmuls %2; fstpl %0" : "=m"(result) : "m"(*a),"m"(*b) : "st");
#else
 result=(double)*a * *b;
#endif
 return result;
}
static double C2_HOOK_CDECL PartnerMultiplyScalarWide(const br_scalar* a,double b) {
 double result;
#if defined(_MSC_VER) && defined(_M_IX86)
 __asm {
 mov eax,a
 fld dword ptr [eax]
 fmul qword ptr b
 fstp qword ptr result
 }
#elif defined(__GNUC__) && defined(__i386__)
 __asm__ __volatile__("flds %1; fmull %2; fstpl %0" : "=m"(result) : "m"(*a),"m"(b) : "st");
#else
 result=(double)*a * b;
#endif
 return result;
}
static double C2_HOOK_CDECL PartnerMultiplyRawScalar(double a,const br_scalar* b) {
 double result;
#if defined(_MSC_VER) && defined(_M_IX86)
 __asm {
 fld qword ptr a
 mov ecx,b
 fmul dword ptr [ecx]
 fstp qword ptr result
 }
#elif defined(__GNUC__) && defined(__i386__)
 __asm__ __volatile__("fldl %1; fmuls %2; fstpl %0" : "=m"(result) : "m"(a),"m"(*b) : "st");
#else
 result=(double)a * *b;
#endif
 return result;
}

/* Semantic name inferred from the original hinge response. */
// FUNCTION: CARMA2_HW 0x004be550
tPhysics_object* C2_HOOK_FASTCALL SetUpTorqueHingePartners(br_vector3* pAngular, br_vector3* pLinear, br_vector3* pPartner_angular, br_vector3* pPartner_linear, const br_vector3* pTorque, tPhysics_object* pObject) {
  br_scalar* angular=pAngular->v;
  br_scalar* linear=pLinear->v;
  br_scalar* partner_angular=pPartner_angular->v;
  br_scalar* partner_linear=pPartner_linear->v;
  const br_scalar* torque=pTorque->v;
  double fVar1;
  tPhysics_object* partner;
  double fVar3;
  double fVar4;
  double fVar5;
  double fVar6;
  float *pfVar7;
  uintptr_t cursor;
  uintptr_t state;
  float *pfVar10;
  br_vector3 angular_temp, lever, response, rotated, rhs;
  double local_4, lever_z_wide;

  if ((pObject->flags & 0x800) == 0) {
    *angular = ForceScalarRound(PartnerReverseDivideScalars(PartnerScalarPtr(pObject, 8), torque));
    angular[1] = ForceScalarRound(PartnerDivideScalars(&torque[1], PartnerScalarPtr(pObject, 0xc)));
    angular[2] = ForceScalarRound(PartnerDivideScalars(&torque[2], PartnerScalarPtr(pObject, 0x10)));
  }
  else {
    *angular = ForceScalarRound(0.0);
    angular[1] = ForceScalarRound(0.0);
    angular[2] = ForceScalarRound(0.0);
  }
  state = ForcePointerAt(pObject, 0x1e4);
  if (state == 0) {
    BrVector3SetFloat((br_vector3*)linear,0,0,0);
    return 0;
  }
  partner = (tPhysics_object*)ForcePointerAt((const void*)(uintptr_t)(state), 4);
  if ((uintptr_t)partner == ForcePointerAt((const void*)(uintptr_t)(pObject), 0x230)) {
    cursor = ForcePointerAt((const void*)(uintptr_t)(pObject), 0x188);
    lever.v[0] = ForceScalarRound(PartnerDifferenceScalars(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 8), PartnerScalarPtr((const void*)(uintptr_t)(pObject), 0x14)));
    lever.v[1] = ForceScalarRound(PartnerDifferenceScalars(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0xc), PartnerScalarPtr((const void*)(uintptr_t)(pObject), 0x18)));
    lever.v[2] = ForceScalarRound(PartnerDifferenceScalars(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x10), PartnerScalarPtr((const void*)(uintptr_t)(pObject), 0x1c)));
    rhs.v[0] = ForceScalarRound(PartnerNegateWide((PartnerAddWide(PartnerAddWide(PartnerMultiplyRawScalar((PartnerSubtractWide(PartnerMultiplyWide((double)angular[1], (double)lever.v[2]), PartnerMultiplyWide((double)angular[2], (double)lever.v[1]))), PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x38)), PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x3c), (PartnerSubtractWide(PartnerMultiplyWide((double)angular[2], (double)lever.v[0]), PartnerMultiplyWide((double)*angular, (double)lever.v[2]))))), PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x40), (PartnerSubtractWide(PartnerMultiplyWide((double)*angular, (double)lever.v[1]), PartnerMultiplyWide((double)angular[1], (double)lever.v[0]))))))));
    rhs.v[1] = ForceScalarRound(PartnerNegateWide((PartnerAddWide(PartnerAddWide(PartnerMultiplyScalars(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x20), angular), PartnerMultiplyScalars(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x24), &angular[1])), PartnerMultiplyScalars(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x28), &angular[2])))));
    rhs.v[2] = ForceScalarRound(PartnerNegateWide((PartnerAddWide(PartnerAddWide(PartnerMultiplyScalars(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x34), &angular[2]), PartnerMultiplyScalars(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x30), &angular[1])), PartnerMultiplyScalars(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x2c), angular)))));
    BrMatrix34ApplyV((br_vector3*)(response.v), (const br_vector3*)(rhs.v), (const br_matrix34*)(state + 8));
    fVar1 = PartnerReverseDivideScalars(PartnerScalarPtr(pObject, 4), &response.v[0]);
    *linear = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(pObject), 0x188), 0x38), fVar1));
    linear[1] = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(pObject), 0x188), 0x3c), fVar1));
    linear[2] = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(pObject), 0x188), 0x40), fVar1));
    fVar1 = PartnerNegateWide((PartnerReverseDivideScalars(PartnerScalarPtr(partner, 4), &response.v[0])));
    *partner_linear = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(pObject), 0x188), 0x44), fVar1));
    partner_linear[1] = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(pObject), 0x188), 0x48), fVar1));
    cursor = 3;
    partner_linear[2] = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(pObject), 0x188), 0x4c), fVar1));
    state = ForcePointerAt((const void*)(uintptr_t)(pObject), 0x188);
    rotated.v[0] = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(state), 0x20), (double)response.v[1]));
    rotated.v[0] = ForceScalarRound(PartnerAddWide(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(state), 0x2c), (double)response.v[2]), (double)rotated.v[0]));
    rotated.v[1] = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(state), 0x24), (double)response.v[1]));
    rotated.v[1] = ForceScalarRound(PartnerAddWide(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(state), 0x30), (double)response.v[2]), (double)rotated.v[1]));
    rotated.v[2] = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(state), 0x28), (double)response.v[1]));
    rotated.v[2] = ForceScalarRound(PartnerAddWide(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(state), 0x34), (double)response.v[2]), (double)rotated.v[2]));
    fVar1 = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(state), 0x38), (double)response.v[0]));
    fVar3 = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(state), 0x3c), (double)response.v[0]));
    fVar4 = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(state), 0x40), (double)response.v[0]));
    pfVar7 = (br_scalar*)ForceBytes(pObject, 8);
    local_4 = (PartnerSubtractWide(PartnerMultiplyWide((double)lever.v[0], fVar3), PartnerMultiplyWide((double)lever.v[1], fVar1)));
    angular_temp.v[1] = ForceScalarRound(PartnerAddWide((PartnerSubtractWide(PartnerMultiplyWide((double)lever.v[2], fVar1), PartnerMultiplyWide((double)lever.v[0], fVar4))), (double)rotated.v[1]));
    angular_temp.v[2] = ForceScalarRound(PartnerAddWide(local_4, (double)rotated.v[2]));
    angular_temp.v[0] = ForceScalarRound(PartnerAddWide((PartnerSubtractWide(PartnerMultiplyWide((double)lever.v[1], fVar4), PartnerMultiplyWide((double)lever.v[2], fVar3))), (double)rotated.v[0]));
    pfVar10 = angular;
    do {
      fVar1 = (double)*pfVar7;
      pfVar7 = pfVar7 + 1;
      cursor = cursor + -1;
      *pfVar10 = ForceScalarRound(PartnerAddScalar(PartnerDivideScalars(&angular_temp.v[((uintptr_t)pfVar10 - (uintptr_t)angular) / sizeof(br_scalar)], pfVar7-1), pfVar10));
      pfVar10 = pfVar10 + 1;
    } while (cursor != 0);
    BrMatrix34ApplyV((br_vector3*)(angular_temp.v), (const br_vector3*)(rotated.v), (const br_matrix34*)(ForceBytes(pObject, 0x8c)));
    BrMatrix34TApplyV((br_vector3*)(partner_angular), (const br_vector3*)(angular_temp.v), (const br_matrix34*)(ForceBytes(partner, 0x8c)));
    state = ForcePointerAt((const void*)(uintptr_t)(pObject), 0x188);
    pfVar7 = (br_scalar*)ForceBytes(partner, 8);
    fVar1 = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(state), 0x44), (double)response.v[0]));
    fVar3 = PartnerDifferenceScalars(PartnerScalarPtr((const void*)(uintptr_t)(state), 0x14), PartnerScalarPtr((const void*)(uintptr_t)(partner), 0x14));
    fVar4 = PartnerDifferenceScalars(PartnerScalarPtr((const void*)(uintptr_t)(state), 0x18), PartnerScalarPtr((const void*)(uintptr_t)(partner), 0x18));
    fVar5 = PartnerDifferenceScalars(PartnerScalarPtr((const void*)(uintptr_t)(state), 0x1c), PartnerScalarPtr((const void*)(uintptr_t)(partner), 0x1c));
    fVar6 = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(state), 0x48), (double)response.v[0]));
    response.v[0] = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(state), 0x4c), (double)response.v[0]));
    *partner_angular = ForceScalarRound(PartnerAddScalar((PartnerSubtractWide(PartnerMultiplyWide(fVar4, (double)response.v[0]), PartnerMultiplyWide(fVar5, fVar6))), partner_angular));
    state = 3;
    partner_angular[1] = ForceScalarRound(PartnerAddScalar((PartnerSubtractWide(PartnerMultiplyWide((double)ForceScalarRound(fVar5), fVar1), PartnerMultiplyWide(fVar3, (double)response.v[0]))), &partner_angular[1]));
    partner_angular[2] = ForceScalarRound(PartnerAddScalar((PartnerSubtractWide(PartnerMultiplyWide(fVar3, fVar6), PartnerMultiplyWide((double)ForceScalarRound(fVar4), fVar1))), &partner_angular[2]));
    do {
      fVar1 = (double)*pfVar7;
      pfVar7 = pfVar7 + 1;
      state = state + -1;
      *partner_angular = ForceScalarRound(PartnerNegateWide((PartnerDivideScalars(partner_angular, pfVar7-1))));
      partner_angular = partner_angular + 1;
    } while (state != 0);
    return partner;
  }
  state = ForcePointerAt((const void*)(uintptr_t)(partner), 0x188);
  lever.v[0] = ForceScalarRound(PartnerDifferenceScalars(PartnerScalarPtr((const void*)(uintptr_t)(state), 0x14), PartnerScalarPtr((const void*)(uintptr_t)(pObject), 0x14)));
  lever.v[1] = ForceScalarRound(PartnerDifferenceScalars(PartnerScalarPtr((const void*)(uintptr_t)(state), 0x18), PartnerScalarPtr((const void*)(uintptr_t)(pObject), 0x18)));
  lever.v[2] = ForceScalarRound(PartnerDifferenceScalars(PartnerScalarPtr((const void*)(uintptr_t)(state), 0x1c), PartnerScalarPtr((const void*)(uintptr_t)(pObject), 0x1c)));
  rhs.v[0] = ForceScalarRound(PartnerAddWide(PartnerAddWide(PartnerMultiplyRawScalar((PartnerSubtractWide(PartnerMultiplyWide((double)angular[1], (double)lever.v[2]), PartnerMultiplyWide((double)angular[2], (double)lever.v[1]))), PartnerScalarPtr((const void*)(uintptr_t)(state), 0x44)), PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(state), 0x48), (PartnerSubtractWide(PartnerMultiplyWide((double)angular[2], (double)lever.v[0]), PartnerMultiplyWide((double)*angular, (double)lever.v[2]))))), PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(state), 0x4c), (PartnerSubtractWide(PartnerMultiplyWide((double)*angular, (double)lever.v[1]), PartnerMultiplyWide((double)angular[1], (double)lever.v[0]))))));
  BrMatrix34ApplyV((br_vector3*)(rotated.v), (const br_vector3*)(angular), (const br_matrix34*)(ForceBytes(pObject, 0x8c)));
  BrMatrix34TApplyV((br_vector3*)(angular_temp.v), (const br_vector3*)(rotated.v), (const br_matrix34*)(ForceBytes(partner, 0x8c)));
  state = ForcePointerAt((const void*)(uintptr_t)(partner), 0x188);
  rhs.v[1] = ForceScalarRound(PartnerAddWide(PartnerAddWide(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(state), 0x24), (double)angular_temp.v[1]), PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(state), 0x20), (double)angular_temp.v[0])), PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(state), 0x28), (double)angular_temp.v[2])));
  state = ForcePointerAt((const void*)(uintptr_t)(partner), 0x188);
  rhs.v[2] = ForceScalarRound(PartnerAddWide(PartnerAddWide(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(state), 0x2c), (double)angular_temp.v[0]), PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(state), 0x34), (double)angular_temp.v[2])), PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(state), 0x30), (double)angular_temp.v[1])));
  BrMatrix34ApplyV((br_vector3*)(response.v), (const br_vector3*)(rhs.v), (const br_matrix34*)(ForcePointerAt((const void*)(uintptr_t)(partner), 0x1e4) + 8));
  fVar1 = PartnerReverseDivideScalars(PartnerScalarPtr(partner, 4), &response.v[0]);
  *partner_linear = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x38), fVar1));
  partner_linear[1] = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x3c), fVar1));
  partner_linear[2] = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x40), fVar1));
  fVar1 = PartnerNegateWide((PartnerReverseDivideScalars(PartnerScalarPtr(pObject, 4), &response.v[0])));
  *linear = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x44), fVar1));
  linear[1] = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x48), fVar1));
  linear[2] = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x4c), fVar1));
  *partner_angular = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x20), (double)response.v[1]));
  partner_angular[1] = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x24), (double)response.v[1]));
  partner_angular[2] = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x28), (double)response.v[1]));
  *partner_angular = ForceScalarRound(PartnerAddScalar(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x2c), (double)response.v[2]), partner_angular));
  partner_angular[1] = ForceScalarRound(PartnerAddScalar(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x30), (double)response.v[2]), &partner_angular[1]));
  partner_angular[2] = ForceScalarRound(PartnerAddScalar(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x34), (double)response.v[2]), &partner_angular[2]));
  BrMatrix34ApplyV((br_vector3*)(angular_temp.v), (const br_vector3*)(partner_angular), (const br_matrix34*)(ForceBytes(partner, 0x8c)));
  BrMatrix34TApplyV((br_vector3*)(rotated.v), (const br_vector3*)(angular_temp.v), (const br_matrix34*)(ForceBytes(pObject, 0x8c)));
  fVar1 = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x44), (double)response.v[0]));
  pfVar10 = (br_scalar*)ForceBytes(pObject, 8);
  fVar3 = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x48), (double)response.v[0]));
  fVar4 = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x4c), (double)response.v[0]));
  rotated.v[0] = ForceScalarRound(PartnerAddWide((PartnerSubtractWide(PartnerMultiplyWide((double)lever.v[1], fVar4), PartnerMultiplyWide((double)lever.v[2], fVar3))), (double)rotated.v[0]));
  rotated.v[1] = ForceScalarRound(PartnerAddWide((PartnerSubtractWide(PartnerMultiplyWide((double)lever.v[2], fVar1), PartnerMultiplyWide((double)lever.v[0], fVar4))), (double)rotated.v[1]));
  rotated.v[2] = ForceScalarRound(PartnerAddWide((PartnerSubtractWide(PartnerMultiplyWide((double)lever.v[0], fVar3), PartnerMultiplyWide((double)lever.v[1], fVar1))), (double)rotated.v[2]));
  state = ForcePointerAt((const void*)(uintptr_t)(partner), 0x188);
  angular_temp.v[0] = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(state), 0x38), (double)response.v[0]));
  fVar1 = PartnerDifferenceScalars(PartnerScalarPtr((const void*)(uintptr_t)(state), 8), PartnerScalarPtr((const void*)(uintptr_t)(partner), 0x14));
  lever.v[1] = ForceScalarRound(PartnerDifferenceScalars(PartnerScalarPtr((const void*)(uintptr_t)(state), 0xc), PartnerScalarPtr((const void*)(uintptr_t)(partner), 0x18)));
  lever_z_wide = PartnerDifferenceScalars(PartnerScalarPtr((const void*)(uintptr_t)(state), 0x10), PartnerScalarPtr((const void*)(uintptr_t)(partner), 0x1c));
  lever.v[2] = ForceScalarRound(lever_z_wide);
  angular_temp.v[1] = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x3c), (double)response.v[0]));
  angular_temp.v[2] = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x40), (double)response.v[0]));
  state = 3;
  *partner_angular = ForceScalarRound(PartnerAddScalar((PartnerSubtractWide(PartnerMultiplyWide((double)lever.v[1], (double)angular_temp.v[2]), PartnerMultiplyWide(lever_z_wide, (double)angular_temp.v[1]))), partner_angular));
  partner_angular[1] = ForceScalarRound(PartnerAddScalar((PartnerSubtractWide(PartnerMultiplyWide((double)lever.v[2], (double)angular_temp.v[0]), PartnerMultiplyWide(fVar1, (double)angular_temp.v[2]))), &partner_angular[1]));
  partner_angular[2] = ForceScalarRound(PartnerAddScalar((PartnerSubtractWide(PartnerMultiplyWide(fVar1, (double)angular_temp.v[1]), PartnerMultiplyWide((double)lever.v[1], (double)angular_temp.v[0]))), &partner_angular[2]));
  pfVar7 = angular;
  do {
    fVar1 = (double)*pfVar10;
    pfVar10 = pfVar10 + 1;
    state = state + -1;
    *pfVar7 = ForceScalarRound(PartnerReverseSubtractScalar(PartnerDivideScalars(&rotated.v[((uintptr_t)pfVar7 - (uintptr_t)angular) / sizeof(br_scalar)], pfVar10-1), pfVar7));
    pfVar7 = pfVar7 + 1;
  } while (state != 0);
  pfVar7 = (br_scalar*)ForceBytes(partner, 8);
  state = 3;
  do {
    fVar1 = (double)*pfVar7;
    pfVar7 = pfVar7 + 1;
    state = state + -1;
    *partner_angular = ForceScalarRound(PartnerDivideScalars(partner_angular, pfVar7-1));
    partner_angular = partner_angular + 1;
  } while (state != 0);
  return partner;
}


/* Semantic name inferred from the original hinge response. */
// FUNCTION: CARMA2_HW 0x004becf0
tPhysics_object* C2_HOOK_FASTCALL SetUpHingePartners(br_vector3* pAngular, br_vector3* pLinear, br_vector3* pPartner_angular, br_vector3* pPartner_linear, tPhysics_object* pObject) {
  br_scalar* angular=pAngular->v;
  br_scalar* linear=pLinear->v;
  br_scalar* partner_angular=pPartner_angular->v;
  br_scalar* partner_linear=pPartner_linear->v;
  double fVar1;
  tPhysics_object* partner;
  double fVar3;
  double fVar4;
  double fVar5;
  double fVar6;
  float *pfVar7;
  uintptr_t cursor;
  float *pfVar9;
  br_vector3 angular_temp, lever, rotated, response, rhs;
  double local_4, lever_y_wide, lever_z_wide;

  partner = (tPhysics_object*)ForcePointerAt((const void*)ForcePointerAt(pObject, 0x1e4), 4);
  if ((uintptr_t)partner == ForcePointerAt((const void*)(uintptr_t)(pObject), 0x230)) {
    cursor = ForcePointerAt((const void*)(uintptr_t)(pObject), 0x188);
    lever_y_wide = PartnerDifferenceScalars(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0xc), PartnerScalarPtr((const void*)(uintptr_t)(pObject), 0x18));
    lever.v[1] = ForceScalarRound(lever_y_wide);
    lever_z_wide = PartnerDifferenceScalars(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x10), PartnerScalarPtr((const void*)(uintptr_t)(pObject), 0x1c));
    lever.v[2] = ForceScalarRound(lever_z_wide);
    lever.v[0] = ForceScalarRound(PartnerDifferenceScalars(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 8), PartnerScalarPtr((const void*)(uintptr_t)(pObject), 0x14)));
    response.v[0] = ForceScalarRound(PartnerAddScalar((PartnerSubtractWide(PartnerMultiplyWide((double)angular[1], (double)lever.v[2]), PartnerMultiplyWide((double)angular[2], (double)lever.v[1]))), linear));
    response.v[1] = ForceScalarRound(PartnerAddScalar((PartnerSubtractWide(PartnerMultiplyWide((double)angular[2], (double)lever.v[0]), PartnerMultiplyWide(lever_z_wide, (double)*angular))), &linear[1]));
    rhs.v[0] = ForceScalarRound(PartnerNegateWide((PartnerAddWide(PartnerAddWide(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x40), (PartnerAddScalar((PartnerSubtractWide(PartnerMultiplyWide(lever_y_wide, (double)*angular), PartnerMultiplyWide((double)angular[1], (double)lever.v[0]))), &linear[2]))), PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x3c), (double)response.v[1])), PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x38), (double)response.v[0])))));
    rhs.v[1] = ForceScalarRound(PartnerNegateWide((PartnerAddWide(PartnerAddWide(PartnerMultiplyScalars(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x20), angular), PartnerMultiplyScalars(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x28), &angular[2])), PartnerMultiplyScalars(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x24), &angular[1])))));
    rhs.v[2] = ForceScalarRound(PartnerNegateWide((PartnerAddWide(PartnerAddWide(PartnerMultiplyScalars(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x2c), angular), PartnerMultiplyScalars(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x30), &angular[1])), PartnerMultiplyScalars(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x34), &angular[2])))));
    BrMatrix34ApplyV((br_vector3*)(response.v), (const br_vector3*)(rhs.v), (const br_matrix34*)(ForcePointerAt(pObject, 0x1e4) + 8));
    fVar1 = PartnerReverseDivideScalars(PartnerScalarPtr(pObject, 4), &response.v[0]);
    *linear = ForceScalarRound(PartnerAddScalar(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(pObject), 0x188), 0x38), fVar1), linear));
    linear[1] = ForceScalarRound(PartnerAddScalar(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(pObject), 0x188), 0x3c), fVar1), &linear[1]));
    linear[2] = ForceScalarRound(PartnerAddScalar(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(pObject), 0x188), 0x40), fVar1), &linear[2]));
    fVar1 = PartnerNegateWide((PartnerReverseDivideScalars(PartnerScalarPtr(partner, 4), &response.v[0])));
    *partner_linear = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(pObject), 0x188), 0x44), fVar1));
    partner_linear[1] = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(pObject), 0x188), 0x48), fVar1));
    partner_linear[2] = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(pObject), 0x188), 0x4c), fVar1));
    cursor = ForcePointerAt((const void*)(uintptr_t)(pObject), 0x188);
    rotated.v[0] = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x20), (double)response.v[1]));
    rotated.v[0] = ForceScalarRound(PartnerAddWide(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x2c), (double)response.v[2]), (double)rotated.v[0]));
    rotated.v[1] = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x24), (double)response.v[1]));
    rotated.v[1] = ForceScalarRound(PartnerAddWide(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x30), (double)response.v[2]), (double)rotated.v[1]));
    rotated.v[2] = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x28), (double)response.v[1]));
    rotated.v[2] = ForceScalarRound(PartnerAddWide(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x34), (double)response.v[2]), (double)rotated.v[2]));
    fVar1 = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x38), (double)response.v[0]));
    fVar3 = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x3c), (double)response.v[0]));
    fVar4 = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x40), (double)response.v[0]));
    local_4 = (PartnerSubtractWide(PartnerMultiplyWide((double)lever.v[0], fVar3), PartnerMultiplyWide((double)lever.v[1], fVar1)));
    angular_temp.v[1] = ForceScalarRound(PartnerAddWide((PartnerSubtractWide(PartnerMultiplyWide((double)lever.v[2], fVar1), PartnerMultiplyWide((double)lever.v[0], fVar4))), (double)rotated.v[1]));
    pfVar7 = (br_scalar*)ForceBytes(pObject, 8);
    angular_temp.v[2] = ForceScalarRound(PartnerAddWide(local_4, (double)rotated.v[2]));
    cursor = 3;
    angular_temp.v[0] = ForceScalarRound(PartnerAddWide((PartnerSubtractWide(PartnerMultiplyWide((double)lever.v[1], fVar4), PartnerMultiplyWide((double)lever.v[2], fVar3))), (double)rotated.v[0]));
    pfVar9 = angular;
    do {
      fVar1 = (double)*pfVar7;
      pfVar7 = pfVar7 + 1;
      cursor = cursor + -1;
      *pfVar9 = ForceScalarRound(PartnerAddScalar(PartnerDivideScalars(&angular_temp.v[((uintptr_t)pfVar9 - (uintptr_t)angular) / sizeof(br_scalar)], pfVar7-1), pfVar9));
      pfVar9 = pfVar9 + 1;
    } while (cursor != 0);
    BrMatrix34ApplyV((br_vector3*)(angular_temp.v), (const br_vector3*)(rotated.v), (const br_matrix34*)(ForceBytes(pObject, 0x8c)));
    BrMatrix34TApplyV((br_vector3*)(partner_angular), (const br_vector3*)(angular_temp.v), (const br_matrix34*)(ForceBytes(partner, 0x8c)));
    cursor = ForcePointerAt((const void*)(uintptr_t)(pObject), 0x188);
    pfVar7 = (br_scalar*)ForceBytes(partner, 8);
    fVar1 = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x44), (double)response.v[0]));
    fVar3 = PartnerDifferenceScalars(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x14), PartnerScalarPtr((const void*)(uintptr_t)(partner), 0x14));
    fVar4 = PartnerDifferenceScalars(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x18), PartnerScalarPtr((const void*)(uintptr_t)(partner), 0x18));
    fVar5 = PartnerDifferenceScalars(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x1c), PartnerScalarPtr((const void*)(uintptr_t)(partner), 0x1c));
    fVar6 = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x48), (double)response.v[0]));
    response.v[0] = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x4c), (double)response.v[0]));
    *partner_angular = ForceScalarRound(PartnerAddScalar((PartnerSubtractWide(PartnerMultiplyWide(fVar4, (double)response.v[0]), PartnerMultiplyWide(fVar5, fVar6))), partner_angular));
    cursor = 3;
    partner_angular[1] = ForceScalarRound(PartnerAddScalar((PartnerSubtractWide(PartnerMultiplyWide((double)ForceScalarRound(fVar5), fVar1), PartnerMultiplyWide(fVar3, (double)response.v[0]))), &partner_angular[1]));
    partner_angular[2] = ForceScalarRound(PartnerAddScalar((PartnerSubtractWide(PartnerMultiplyWide(fVar3, fVar6), PartnerMultiplyWide((double)ForceScalarRound(fVar4), fVar1))), &partner_angular[2]));
    do {
      fVar1 = (double)*pfVar7;
      pfVar7 = pfVar7 + 1;
      cursor = cursor + -1;
      *partner_angular = ForceScalarRound(PartnerNegateWide((PartnerDivideScalars(partner_angular, pfVar7-1))));
      partner_angular = partner_angular + 1;
    } while (cursor != 0);
    return partner;
  }
  cursor = ForcePointerAt((const void*)(uintptr_t)(partner), 0x188);
  lever_y_wide = PartnerDifferenceScalars(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x18), PartnerScalarPtr((const void*)(uintptr_t)(pObject), 0x18));
  lever.v[1] = ForceScalarRound(lever_y_wide);
  lever.v[0] = ForceScalarRound(PartnerDifferenceScalars(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x14), PartnerScalarPtr((const void*)(uintptr_t)(pObject), 0x14)));
  lever_z_wide = PartnerDifferenceScalars(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x1c), PartnerScalarPtr((const void*)(uintptr_t)(pObject), 0x1c));
  lever.v[2] = ForceScalarRound(lever_z_wide);
  response.v[1] = ForceScalarRound(PartnerAddScalar((PartnerSubtractWide(PartnerMultiplyWide((double)angular[2], (double)lever.v[0]), PartnerMultiplyWide(lever_z_wide, (double)*angular))), &linear[1]));
  response.v[2] = ForceScalarRound(PartnerAddScalar((PartnerSubtractWide(PartnerMultiplyWide(lever_y_wide, (double)*angular), PartnerMultiplyWide((double)angular[1], (double)lever.v[0]))), &linear[2]));
  rhs.v[0] = ForceScalarRound(PartnerAddWide(PartnerAddWide(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x44), (PartnerAddScalar((PartnerSubtractWide(PartnerMultiplyWide((double)angular[1], (double)lever.v[2]), PartnerMultiplyWide((double)angular[2], (double)lever.v[1]))), linear))), PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x4c), (double)response.v[2])), PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x48), (double)response.v[1])));
  BrMatrix34ApplyV((br_vector3*)(rotated.v), (const br_vector3*)(angular), (const br_matrix34*)(ForceBytes(pObject, 0x8c)));
  BrMatrix34TApplyV((br_vector3*)(angular_temp.v), (const br_vector3*)(rotated.v), (const br_matrix34*)(ForceBytes(partner, 0x8c)));
  cursor = ForcePointerAt((const void*)(uintptr_t)(partner), 0x188);
  rhs.v[1] = ForceScalarRound(PartnerAddWide(PartnerAddWide(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x28), (double)angular_temp.v[2]), PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x24), (double)angular_temp.v[1])), PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x20), (double)angular_temp.v[0])));
  cursor = ForcePointerAt((const void*)(uintptr_t)(partner), 0x188);
  rhs.v[2] = ForceScalarRound(PartnerAddWide(PartnerAddWide(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x34), (double)angular_temp.v[2]), PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x30), (double)angular_temp.v[1])), PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x2c), (double)angular_temp.v[0])));
  BrMatrix34ApplyV((br_vector3*)(response.v), (const br_vector3*)(rhs.v), (const br_matrix34*)(ForcePointerAt((const void*)(uintptr_t)(partner), 0x1e4) + 8));
  fVar1 = PartnerReverseDivideScalars(PartnerScalarPtr(partner, 4), &response.v[0]);
  *partner_linear = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x38), fVar1));
  partner_linear[1] = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x3c), fVar1));
  partner_linear[2] = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x40), fVar1));
  fVar1 = PartnerNegateWide((PartnerReverseDivideScalars(PartnerScalarPtr(pObject, 4), &response.v[0])));
  *linear = ForceScalarRound(PartnerAddScalar(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x44), fVar1), linear));
  linear[1] = ForceScalarRound(PartnerAddScalar(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x48), fVar1), &linear[1]));
  linear[2] = ForceScalarRound(PartnerAddScalar(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x4c), fVar1), &linear[2]));
  *partner_angular = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x20), (double)response.v[1]));
  partner_angular[1] = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x24), (double)response.v[1]));
  partner_angular[2] = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x28), (double)response.v[1]));
  *partner_angular = ForceScalarRound(PartnerAddScalar(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x2c), (double)response.v[2]), partner_angular));
  partner_angular[1] = ForceScalarRound(PartnerAddScalar(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x30), (double)response.v[2]), &partner_angular[1]));
  partner_angular[2] = ForceScalarRound(PartnerAddScalar(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x34), (double)response.v[2]), &partner_angular[2]));
  BrMatrix34ApplyV((br_vector3*)(angular_temp.v), (const br_vector3*)(partner_angular), (const br_matrix34*)(ForceBytes(partner, 0x8c)));
  BrMatrix34TApplyV((br_vector3*)(rotated.v), (const br_vector3*)(angular_temp.v), (const br_matrix34*)(ForceBytes(pObject, 0x8c)));
  angular_temp.v[0] = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x44), (double)response.v[0]));
  angular_temp.v[1] = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x48), (double)response.v[0]));
  angular_temp.v[2] = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x4c), (double)response.v[0]));
  pfVar9 = (br_scalar*)ForceBytes(pObject, 8);
  rotated.v[0] = ForceScalarRound(PartnerAddWide((PartnerSubtractWide(PartnerMultiplyWide((double)lever.v[1], (double)angular_temp.v[2]), PartnerMultiplyWide((double)lever.v[2], (double)angular_temp.v[1]))), (double)rotated.v[0]));
  cursor = 3;
  rotated.v[1] = ForceScalarRound(PartnerAddWide((PartnerSubtractWide(PartnerMultiplyWide((double)lever.v[2], (double)angular_temp.v[0]), PartnerMultiplyWide((double)lever.v[0], (double)angular_temp.v[2]))), (double)rotated.v[1]));
  rotated.v[2] = ForceScalarRound(PartnerAddWide((PartnerSubtractWide(PartnerMultiplyWide((double)lever.v[0], (double)angular_temp.v[1]), PartnerMultiplyWide((double)lever.v[1], (double)angular_temp.v[0]))), (double)rotated.v[2]));
  pfVar7 = angular;
  do {
    fVar1 = (double)*pfVar9;
    pfVar9 = pfVar9 + 1;
    cursor = cursor + -1;
    *pfVar7 = ForceScalarRound(PartnerReverseSubtractScalar(PartnerDivideScalars(&rotated.v[((uintptr_t)pfVar7 - (uintptr_t)angular) / sizeof(br_scalar)], pfVar9-1), pfVar7));
    pfVar7 = pfVar7 + 1;
  } while (cursor != 0);
  cursor = ForcePointerAt((const void*)(uintptr_t)(partner), 0x188);
  pfVar7 = (br_scalar*)ForceBytes(partner, 8);
  fVar1 = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x38), (double)response.v[0]));
  fVar3 = PartnerDifferenceScalars(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 8), PartnerScalarPtr((const void*)(uintptr_t)(partner), 0x14));
  fVar4 = PartnerDifferenceScalars(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0xc), PartnerScalarPtr((const void*)(uintptr_t)(partner), 0x18));
  fVar5 = PartnerDifferenceScalars(PartnerScalarPtr((const void*)(uintptr_t)(cursor), 0x10), PartnerScalarPtr((const void*)(uintptr_t)(partner), 0x1c));
  fVar6 = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x3c), (double)response.v[0]));
  cursor = 3;
  response.v[0] = ForceScalarRound(PartnerMultiplyScalarWide(PartnerScalarPtr((const void*)ForcePointerAt((const void*)(uintptr_t)(partner), 0x188), 0x40), (double)response.v[0]));
  *partner_angular = ForceScalarRound(PartnerAddScalar((double)ForceScalarRound(PartnerSubtractWide(PartnerMultiplyWide((double)ForceScalarRound(fVar4), (double)response.v[0]), PartnerMultiplyWide(fVar5, fVar6))), partner_angular));
  partner_angular[1] = ForceScalarRound(PartnerAddScalar((PartnerSubtractWide(PartnerMultiplyWide((double)ForceScalarRound(fVar5), fVar1), PartnerMultiplyWide(fVar3, (double)response.v[0]))), &partner_angular[1]));
  partner_angular[2] = ForceScalarRound(PartnerAddScalar((PartnerSubtractWide(PartnerMultiplyWide(fVar3, fVar6), PartnerMultiplyWide((double)ForceScalarRound(fVar4), fVar1))), &partner_angular[2]));
  do {
    fVar1 = (double)*pfVar7;
    pfVar7 = pfVar7 + 1;
    cursor = cursor + -1;
    *partner_angular = ForceScalarRound(PartnerDivideScalars(partner_angular, pfVar7-1));
    partner_angular = partner_angular + 1;
  } while (cursor != 0);
  return partner;
}





static int C2_HOOK_CDECL ForceMatrixHasPartner(const tPhysics_object* object) {
    tU32 value;
    memcpy(&value,(const char*)object+0x1e4,4);
    return value != 0;
}

#if defined(_MSC_VER) && _MSC_VER < 1300
#pragma auto_inline(off)
#endif
static double C2_HOOK_CDECL ForceMatrixScalar(double value) {
#if defined(_MSC_VER) && defined(_M_IX86)
    br_scalar scalar;
    double result;
    __asm {
        fld qword ptr value
        fstp dword ptr scalar
        fld dword ptr scalar
        fstp qword ptr result
    }
    return result;
#else
    volatile br_scalar result=(br_scalar)value;
    return result;
#endif
}
#if defined(_MSC_VER) && _MSC_VER < 1300
#pragma auto_inline(on)
#endif

/* Distinct raw NaN inputs expose SSE/x87 operand-selection differences.
 * These small operations retain the original FLD and arithmetic operand order. */
static double C2_HOOK_CDECL ForceMatrixMultiplyScalars(const br_scalar* a, const br_scalar* b) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, a
        mov ecx, b
        fld dword ptr [eax]
        fmul dword ptr [ecx]
        fstp qword ptr result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("flds %1; fmuls %2; fstpl %0" : "=m"(result) : "m"(*a), "m"(*b) : "st");
#else
    result=(double)*a * *b;
#endif
    return result;
}

static double C2_HOOK_CDECL ForceMatrixMultiplyWide(const br_scalar* a, double b) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, a
        fld dword ptr [eax]
        fmul qword ptr b
        fstp qword ptr result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("flds %1; fmull %2; fstpl %0" : "=m"(result) : "m"(*a), "m"(b) : "st");
#else
    result=(double)*a * b;
#endif
    return result;
}

static double C2_HOOK_CDECL ForceMatrixAddScalar(double a, const br_scalar* b) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, b
        fld qword ptr a
        fadd dword ptr [eax]
        fstp qword ptr result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("fldl %1; fadds %2; fstpl %0" : "=m"(result) : "m"(a), "m"(*b) : "st");
#else
    result=a + *b;
#endif
    return result;
}

static double C2_HOOK_CDECL ForceMatrixReverseSubtractScalar(double a, const br_scalar* b) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, b
        fld qword ptr a
        fsubr dword ptr [eax]
        fstp qword ptr result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("fldl %1; fsubrs %2; fstpl %0" : "=m"(result) : "m"(a), "m"(*b) : "st");
#else
    result=*b - a;
#endif
    return result;
}

static double C2_HOOK_CDECL ForceMatrixAddWide(double a, double b) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        fld qword ptr a
        fadd qword ptr b
        fstp qword ptr result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("fldl %1; faddl %2; fstpl %0" : "=m"(result) : "m"(a), "m"(b) : "st");
#else
    result=a + b;
#endif
    return result;
}

static double C2_HOOK_CDECL ForceMatrixSubtractWide(double a, double b) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        fld qword ptr a
        fsub qword ptr b
        fstp qword ptr result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("fldl %1; fsubl %2; fstpl %0" : "=m"(result) : "m"(a), "m"(b) : "st");
#else
    result=a - b;
#endif
    return result;
}

/* Source torque and force routes have different original sum and spill orders. */
static double C2_HOOK_CDECL ForceMatrixResponse(const tPhysics_joint_force* row,
    int side, const br_vector3* angular, const br_vector3* linear, int source_torque, int row_torque) {
    const br_vector3* point;
    const br_vector3* direction;
    double x,y,z;

    point=side ? &row->point_b : &row->point_a;
    direction=side ? &row->direction_b : &row->direction_a;
    if (row_torque) {
        x=ForceMatrixMultiplyScalars(&direction->v[0],&angular->v[0]);
        y=ForceMatrixMultiplyScalars(&direction->v[1],&angular->v[1]);
        z=ForceMatrixMultiplyScalars(&direction->v[2],&angular->v[2]);
        if (source_torque) {
            return side ? ForceMatrixAddWide(ForceMatrixAddWide(y,x),z) : ForceMatrixAddWide(ForceMatrixAddWide(z,y),x);
        }
        return side ? ForceMatrixAddWide(ForceMatrixAddWide(z,x),y) : ForceMatrixAddWide(ForceMatrixAddWide(x,y),z);
    }
    x=ForceMatrixAddScalar(ForceMatrixSubtractWide(ForceMatrixMultiplyScalars(&point->v[2],&angular->v[1]),
        ForceMatrixMultiplyScalars(&point->v[1],&angular->v[2])),&linear->v[0]);
    y=ForceMatrixAddScalar(ForceMatrixSubtractWide(ForceMatrixMultiplyScalars(&point->v[0],&angular->v[2]),
        ForceMatrixMultiplyScalars(&point->v[2],&angular->v[0])),&linear->v[1]);
    z=ForceMatrixAddScalar(ForceMatrixSubtractWide(ForceMatrixMultiplyScalars(&point->v[1],&angular->v[0]),
        ForceMatrixMultiplyScalars(&point->v[0],&angular->v[1])),&linear->v[2]);
    if (source_torque || side) {
        x=ForceMatrixScalar(x);
    } else {
        y=ForceMatrixScalar(y);
    }
    z=ForceMatrixScalar(z);
    x=ForceMatrixMultiplyWide(&direction->v[0],x);
    y=ForceMatrixMultiplyWide(&direction->v[1],y);
    z=ForceMatrixMultiplyWide(&direction->v[2],z);
    if (source_torque) {
        return side ? ForceMatrixAddWide(ForceMatrixAddWide(x,y),z) : ForceMatrixAddWide(ForceMatrixAddWide(y,z),x);
    }
    return side ? ForceMatrixAddWide(ForceMatrixAddWide(z,y),x) : ForceMatrixAddWide(ForceMatrixAddWide(x,z),y);
}

// FUNCTION: CARMA2_HW 0x004bf490
void C2_HOOK_FASTCALL MakeMatrix(br_scalar* matrix, int dimension, tPhysics_joint_force* const* records, int count) {
    br_vector3 angular_a,linear_a,angular_b,linear_b;
    br_vector3 angular_partner_a,linear_partner_a,angular_partner_b,linear_partner_b;
    tPhysics_object *object_a,*object_b,*partner_a,*partner_b;
    tPhysics_joint_force *source,*row;
    br_scalar *column_cell,*row_cell;
    int index,j,source_torque,row_torque;

    C2_HOOK_BUG_ON(sizeof(tPhysics_joint_force)!=0x60);
    for (index=0;index<count;index++) {
        source=records[index];
        object_a=source->object_a;
        object_b=source->object_b;
        if (object_b!=NULL && source->type==14) {
            object_b=NULL;
        }
        source_torque=source->type==11;
        partner_a=NULL;
        partner_b=NULL;
        if (source_torque) {
            object_a=source->object_a;
            object_b=source->object_b;
            partner_a=SetUpTorqueHingePartners(&angular_a,&linear_a,
                &angular_partner_a,&linear_partner_a,&source->direction_a,object_a);
            if (object_b!=NULL) {
                partner_b=SetUpTorqueHingePartners(&angular_b,&linear_b,
                    &angular_partner_b,&linear_partner_b,&source->direction_b,object_b);
            }
        } else {
            SetUpForce(&angular_a,&linear_a,&source->point_a,&source->direction_a,object_a);
            if (ForceMatrixHasPartner(object_a)) {
                partner_a=SetUpHingePartners(&angular_a,&linear_a,
                    &angular_partner_a,&linear_partner_a,object_a);
            }
            if (object_b!=NULL) {
                SetUpForce(&angular_b,&linear_b,&source->point_b,&source->direction_b,object_b);
                if (ForceMatrixHasPartner(object_b)) {
                    partner_b=SetUpHingePartners(&angular_b,&linear_b,
                        &angular_partner_b,&linear_partner_b,object_b);
                }
            }
        }
        column_cell=matrix+index;
        row_cell=matrix+(tU32)index*(tU32)dimension;
        for (j=0;j<=index;j++) {
            if (!source_torque) {
                *column_cell=0.f;
            }
            row=records[j];
            row_torque=row->type==11;
            if (row->object_a==object_a) {
                *column_cell=(br_scalar)ForceMatrixResponse(row,0,&angular_a,&linear_a,source_torque,row_torque);
            } else if (row->object_b==object_a && (source_torque || row->type!=14)) {
                *column_cell=(br_scalar)-ForceMatrixResponse(row,1,&angular_a,&linear_a,source_torque,row_torque);
            } else if (source_torque) {
                *column_cell=0.f;
            }
            if (source_torque && partner_a!=NULL) {
                row=records[j];
                if (row->object_a==partner_a) {
                    *column_cell=(br_scalar)ForceMatrixAddScalar(ForceMatrixResponse(row,0,&angular_partner_a,&linear_partner_a,1,row_torque),column_cell);
                } else if (row->object_b==partner_a) {
                    *column_cell=(br_scalar)ForceMatrixReverseSubtractScalar(ForceMatrixResponse(row,1,&angular_partner_a,&linear_partner_a,1,row_torque),column_cell);
                }
            }
            if (object_b!=NULL) {
                if (source_torque) {
                    row=records[j];
                }
                if (row->object_a==object_b) {
                    *column_cell=(br_scalar)ForceMatrixReverseSubtractScalar(ForceMatrixResponse(row,0,&angular_b,&linear_b,source_torque,row_torque),column_cell);
                } else if (row->object_b==object_b && (source_torque || row->type!=14)) {
                    *column_cell=(br_scalar)ForceMatrixAddScalar(ForceMatrixResponse(row,1,&angular_b,&linear_b,source_torque,row_torque),column_cell);
                }
            }
            if (!source_torque && partner_a!=NULL) {
                if (row->object_a==partner_a) {
                    *column_cell=(br_scalar)ForceMatrixAddScalar(ForceMatrixResponse(row,0,&angular_partner_a,&linear_partner_a,0,row_torque),column_cell);
                } else if (row->object_b==partner_a) {
                    *column_cell=(br_scalar)ForceMatrixReverseSubtractScalar(ForceMatrixResponse(row,1,&angular_partner_a,&linear_partner_a,0,row_torque),column_cell);
                }
            }
            if (partner_b!=NULL) {
                if (source_torque) {
                    row=records[j];
                }
                if (row->object_a==partner_b) {
                    *column_cell=(br_scalar)ForceMatrixReverseSubtractScalar(ForceMatrixResponse(row,0,&angular_partner_b,&linear_partner_b,source_torque,row_torque),column_cell);
                } else if (row->object_b==partner_b) {
                    *column_cell=(br_scalar)ForceMatrixAddScalar(ForceMatrixResponse(row,1,&angular_partner_b,&linear_partner_b,source_torque,row_torque),column_cell);
                }
            }
            memcpy(row_cell,column_cell,4);
            row_cell++;
            column_cell+=dimension;
        }
    }
}

static void C2_HOOK_CDECL ApplicationScaleAdd(br_vector3* value, br_scalar impulse, br_vector3* target) {
    double x = (double)value->v[0] * impulse;
    value->v[0] = (br_scalar)PhysicsJointScalar(x);
    value->v[1] = (br_scalar)PhysicsJointScalar((double)value->v[1] * impulse);
    value->v[2] = (br_scalar)PhysicsJointScalar((double)value->v[2] * impulse);
    PhysicsJointAddVelocity(&target->v[0], x);
    PhysicsJointAddVelocity(&target->v[1], value->v[1]);
    PhysicsJointAddVelocity(&target->v[2], value->v[2]);
}

static void C2_HOOK_CDECL ApplicationWorldAdd(tPhysics_object* object, const br_vector3* delta) {
    br_vector3 world;
    BrMatrix34ApplyV(&world, delta, &object->transform_matrix);
    PhysicsJointAddVelocity(&object->v.v[0], world.v[0]);
    PhysicsJointAddVelocity(&object->v.v[1], world.v[1]);
    PhysicsJointAddVelocity(&object->v.v[2], world.v[2]);
}

static void C2_HOOK_CDECL ApplicationDirect(tPhysics_joint_force* record, tPhysics_object* object, br_scalar impulse, int second) {
    br_vector3 force;
    br_vector3 delta;
    br_vector3 cross;
    const br_vector3* direction = second ? &record->direction_b : &record->direction_a;
    const br_vector3* point = second ? &record->point_b : &record->point_a;
    double x;
    double y;
    double z;
    tU32 flags;
    force.v[0] = (br_scalar)PhysicsJointScalar((double)direction->v[0] * impulse);
    delta.v[0] = (br_scalar)PhysicsJointScalar(PartnerReverseDivideScalars(&object->M, &force.v[0]));
    force.v[1] = (br_scalar)PhysicsJointScalar((double)direction->v[1] * impulse);
    delta.v[1] = (br_scalar)PhysicsJointScalar(PartnerReverseDivideScalars(&object->M, &force.v[1]));
    force.v[2] = (br_scalar)PhysicsJointScalar((double)direction->v[2] * impulse);
    delta.v[2] = (br_scalar)PhysicsJointScalar(PartnerReverseDivideScalars(&object->M, &force.v[2]));
    PhysicsJointAddVelocity(&object->velocity_car_space.v[0], delta.v[0]);
    PhysicsJointAddVelocity(&object->velocity_car_space.v[1], delta.v[1]);
    PhysicsJointAddVelocity(&object->velocity_car_space.v[2], delta.v[2]);
    BrMatrix34ApplyV(&cross, &delta, &object->transform_matrix);
    PhysicsJointAddVelocity(&object->v.v[0], cross.v[0]);
    flags = object->flags;
    PhysicsJointAddVelocity(&object->v.v[1], cross.v[1]);
    PhysicsJointAddVelocity(&object->v.v[2], cross.v[2]);
    if (!(flags & 0x800)) {
        x = PartnerSubtractWide(PhysicsJointProductMemory(&point->v[1], &force.v[2]), PhysicsJointProductMemory(&point->v[2], &force.v[1]));
        y = PartnerSubtractWide(PhysicsJointProductMemory(&point->v[2], &force.v[0]), PhysicsJointProductMemory(&point->v[0], &force.v[2]));
        z = PartnerSubtractWide(PhysicsJointProductMemory(&point->v[0], &force.v[1]), PhysicsJointProductMemory(&point->v[1], &force.v[0]));
        cross.v[0] = (br_scalar)PhysicsJointScalar(x);
        cross.v[1] = (br_scalar)PhysicsJointScalar(y);
        cross.v[2] = (br_scalar)PhysicsJointScalar(z);
        PhysicsJointAddVelocity(&object->omega.v[0], PartnerReverseDivideScalars(&object->I.v[0], &cross.v[0]));
        PhysicsJointAddVelocity(&object->omega.v[1], PartnerReverseDivideScalars(&object->I.v[1], &cross.v[1]));
        PhysicsJointAddVelocity(&object->omega.v[2], PartnerReverseDivideScalars(&object->I.v[2], &cross.v[2]));
    }
}

static void C2_HOOK_CDECL ApplicationHinged(tPhysics_object* object, br_vector3* angular, br_vector3* linear, tPhysics_object* partner, br_vector3* partner_angular, br_vector3* partner_linear, br_scalar impulse, int second) {
    ApplicationScaleAdd(angular, impulse, &object->omega);
    ApplicationScaleAdd(partner_angular, impulse, &partner->omega);
    if (second) {
        linear->v[0] = (br_scalar)PhysicsJointScalar((double)linear->v[0] * impulse);
        linear->v[1] = (br_scalar)PhysicsJointScalar((double)linear->v[1] * impulse);
        linear->v[2] = (br_scalar)PhysicsJointScalar((double)linear->v[2] * impulse);
        PhysicsJointAddVelocity(&object->velocity_car_space.v[0], linear->v[0]);
        PhysicsJointAddVelocity(&object->velocity_car_space.v[1], linear->v[1]);
        PhysicsJointAddVelocity(&object->velocity_car_space.v[2], linear->v[2]);
    } else {
        ApplicationScaleAdd(linear, impulse, &object->velocity_car_space);
    }
    ApplicationWorldAdd(object, linear);
    ApplicationScaleAdd(partner_linear, impulse, &partner->velocity_car_space);
    ApplicationWorldAdd(partner, partner_linear);
}

// FUNCTION: CARMA2_HW 0x004b7960
void C2_HOOK_FAKE_THISCALL ApplySingleForce(tPhysics_joint_force* record, undefined4 unused, br_scalar impulse) {
    tPhysics_object* a;
    tPhysics_object* b;
    tPhysics_object* partner;
    br_vector3 angular;
    br_vector3 linear;
    br_vector3 partner_angular;
    br_vector3 partner_linear;
    int type = record->type;
    (void)unused;
    if (type == 9 || impulse == 0.f || PhysicsScalarNaN(&impulse)) {
        return;
    }
    a = record->object_a;
    b = record->object_b;
    if (type == 11) {
        partner = SetUpTorqueHingePartners(&angular, &linear, &partner_angular, &partner_linear, &record->direction_a, a);
        ApplicationScaleAdd(&angular, impulse, &a->omega);
        if (partner != NULL) {
            ApplicationScaleAdd(&linear, impulse, &a->velocity_car_space);
            BrMatrix34ApplyV(&a->v, &a->velocity_car_space, &a->transform_matrix);
            ApplicationScaleAdd(&partner_linear, impulse, &partner->velocity_car_space);
            BrMatrix34ApplyV(&partner->v, &partner->velocity_car_space, &partner->transform_matrix);
            ApplicationScaleAdd(&partner_angular, impulse, &partner->omega);
        }
        if (b != NULL) {
            partner = SetUpTorqueHingePartners(&angular, &linear, &partner_angular, &partner_linear, &record->direction_b, b);
            impulse = -impulse;
            ApplicationScaleAdd(&angular, impulse, &b->omega);
            if (partner != NULL) {
                ApplicationScaleAdd(&linear, impulse, &b->velocity_car_space);
                BrMatrix34ApplyV(&b->v, &b->velocity_car_space, &b->transform_matrix);
                ApplicationScaleAdd(&partner_linear, impulse, &partner->velocity_car_space);
                BrMatrix34ApplyV(&partner->v, &partner->velocity_car_space, &partner->transform_matrix);
                ApplicationScaleAdd(&partner_angular, impulse, &partner->omega);
            }
        }
        return;
    }
    if (ForcePointerAt(a, 0x1e4) == 0) {
        ApplicationDirect(record, a, impulse, 0);
    } else {
        SetUpForce(&angular, &linear, &record->point_a, &record->direction_a, a);
        partner = SetUpHingePartners(&angular, &linear, &partner_angular, &partner_linear, a);
        ApplicationHinged(a, &angular, &linear, partner, &partner_angular, &partner_linear, impulse, 0);
    }
    if (b != NULL && record->type != 14) {
        if (ForcePointerAt(b, 0x1e4) != 0) {
            SetUpForce(&angular, &linear, &record->point_b, &record->direction_b, b);
            partner = SetUpHingePartners(&angular, &linear, &partner_angular, &partner_linear, b);
            impulse = -impulse;
            ApplicationHinged(b, &angular, &linear, partner, &partner_angular, &partner_linear, impulse, 1);
        } else {
            ApplicationDirect(record, b, -impulse, 1);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004b8cf0
void C2_HOOK_FASTCALL RedoQuickHingeVelocities(tPhysics_object* object) {
    tPhysics_object* partner = (tPhysics_object*)ForcePointerAt((void*)ForcePointerAt(object, 0x1e4), 4);
    tPhysics_object* root = object->field_0x230;
    tPhysics_joint_force records[3];
    tPhysics_joint_force* pointers[3];
    br_scalar matrix[9];
    br_vector3 velocity;
    br_vector3 coefficients;
    void* saved_root;
    void* saved_object;
    int i;
    if (partner != root) {
        root = object;
        object = partner;
    }
    CreateJointForce(object, root, &records[0], &object->physics_joint1->hinge_axis);
    CreateJointTorque(object, root, &records[1], &object->physics_joint1->hinge_axis2);
    CreateJointTorque(object, root, &records[2], &object->physics_joint1->hinge_axis3);
    for (i = 0; i < 3; i++) {
        pointers[i] = &records[i];
    }
    saved_root = (void*)ForcePointerAt(root, 0x1e4);
    saved_object = (void*)ForcePointerAt(object, 0x1e4);
    *(void**)((tU8*)object + 0x1e4) = NULL;
    *(void**)((tU8*)root + 0x1e4) = NULL;
    MakeMatrix(matrix, 3, pointers, 3);
    DRMatrix33Inverse((br_scalar*)((tU8*)saved_object + 8), matrix);
    MakeJointVelocitiesVector(velocity.v, pointers, 3);
    BrMatrix34ApplyV(&coefficients, &velocity, (br_matrix34*)((tU8*)saved_object + 8));
    for (i = 0; i < 3; i++) {
        ApplySingleForce(pointers[i], 0, coefficients.v[i]);
    }
    *(void**)((tU8*)object + 0x1e4) = saved_object;
    *(void**)((tU8*)root + 0x1e4) = saved_root;
}

static tPhysics_joint* C2_HOOK_CDECL ApplicationJoint(tPhysics_object* object, int secondary) {
    return (tPhysics_joint*)ForcePointerAt(object, secondary ? 0x18c : 0x188);
}

/* Unlike CreateJointTorque, the inlined constructors reload the joint pointer
 * at each source load. Record storage may overlap the object's joint pointer. */
static void C2_HOOK_CDECL ApplicationInlineTorque(tPhysics_object* object, tPhysics_object* root, tPhysics_joint_force* record, unsigned axis) {
    br_vector3 world;
    int i;
    record->field_0 = 0;
    record->object_a = object;
    record->object_b = root;
    for (i = 0; i < 3; i++) {
        PhysicsSubtractScalar(&record->point_a.v[i], &ApplicationJoint(object, 0)->field_0x08.v[i], &object->cmpos.v[i]);
    }
    for (i = 0; i < 3; i++) {
        PhysicsSubtractScalar(&record->point_b.v[i], &ApplicationJoint(object, 0)->field_0x14.v[i], &root->cmpos.v[i]);
    }
    for (i = 0; i < 3; i++) {
        PhysicsCopyWord(&record->direction_a.v[i], (tU8*)ApplicationJoint(object, 0) + axis + 4*i);
    }
    BrMatrix34ApplyV(&world, &record->direction_a, &object->transform_matrix);
    BrMatrix34TApplyV(&record->direction_b, &world, &root->transform_matrix);
    record->type = 11;
}

static void C2_HOOK_CDECL ApplicationSecondary(tPhysics_object* object, tPhysics_joint_force* record, unsigned axis, int kind, int point) {
    int i;
    record->field_0 = 0;
    record->object_a = object;
    record->object_b = NULL;
    record->field_c = 0;
    if (point) {
        for (i = 0; i < 3; i++) {
            PhysicsSubtractScalar(&record->point_a.v[i], &ApplicationJoint(object, 1)->field_0x08.v[i], &object->cmpos.v[i]);
        }
    }
    for (i = 0; i < 3; i++) {
        PhysicsCopyWord(&record->direction_a.v[i], (tU8*)ApplicationJoint(object, 1) + axis + 4*i);
    }
    record->type = kind;
}

// FUNCTION: CARMA2_HW 0x004b8350
int C2_HOOK_FASTCALL AllocateObjectWorkspace(tPhysics_object* object, tPhysics_joint_force* output) {
    tPhysics_joint* joint = ApplicationJoint(object, 0);
    tPhysics_object* root;
    tPhysics_object* partner;
    tPhysics_object* child;
    tU8* workspace;
    br_matrix34 inverse;
    br_vector3 axis;
    br_vector3 temp;
    br_vector3 world;
    br_vector3 response;
    br_vector3 cross;
    double projection;
    double high;
    int count = 0;
    int i;
    int added;
    int type;
    if (joint != NULL && joint->type != 0 && !(object->flags & 0x400)) {
        root = object->field_0x230;
        if (joint->type == 4) {
            CreateJointForce(object, root, output, &joint->hinge_axis2);
            CreateJointForce(object, root, output + 1, &ApplicationJoint(object, 0)->hinge_axis3);
            output += 2;
            count = 2;
        } else {
            for (i = 0; i < 3; i++) {
                axis.v[0] = axis.v[1] = axis.v[2] = 0.f;
                axis.v[i] = 1.f;
                CreateJointForce(object, root, output, &axis);
                output++;
            }
            count = 3;
        }
        type = ApplicationJoint(object, 0)->type;
        switch (type) {
        case 1:
            ApplicationInlineTorque(object, root, output, 0x20);
            ApplicationInlineTorque(object, root, output + 1, 0x2c);
            output += 2;
            count += 2;
            break;
        case 2:
            ApplicationInlineTorque(object, root, output, 0x20);
            output++;
            count++;
            break;
        case 3:
            break;
        case 4:
            partner = object->field_0x230;
            workspace = GetPhysicsScratchPointer(208);
            IncreasePhysicsScratchPointer(208);
            *(void**)((tU8*)object + 0x1e4) = workspace;
            *(void**)((tU8*)partner + 0x1e4) = workspace + 104;
            *(tU32*)ForcePointerAt(object, 0x1e4) = 0;
            *(tPhysics_object**)(ForcePointerAt(object, 0x1e4) + 4) = partner;
            *(tU32*)ForcePointerAt(partner, 0x1e4) = 0;
            *(tPhysics_object**)(ForcePointerAt(partner, 0x1e4) + 4) = object;
            partner = (tPhysics_object*)ForcePointerAt((void*)ForcePointerAt(object, 0x1e4), 4);
            BrMatrix34LPInverse(&inverse, &object->transform_matrix);
            BrMatrix34Mul((br_matrix34*)(ForcePointerAt(object, 0x1e4) + 0x38), &partner->transform_matrix, &inverse);
            BrMatrix34LPInverse((br_matrix34*)(ForcePointerAt(partner, 0x1e4) + 0x38), (br_matrix34*)(ForcePointerAt(object, 0x1e4) + 0x38));
            *(tU32*)ForcePointerAt(object, 0x1e4) = 1;
            *(tU32*)ForcePointerAt(partner, 0x1e4) = 1;
            RedoQuickHingeVelocities(object);
            joint = ApplicationJoint(object, 0);
            projection = PhysicsJointSum(PhysicsJointSum(PhysicsJointProductMemory(&joint->parent_bone_axis.v[1], &root->omega.v[1]), PhysicsJointProductMemory(&root->omega.v[0], &joint->parent_bone_axis.v[0])), PhysicsJointProductMemory(&joint->parent_bone_axis.v[2], &root->omega.v[2]));
            for (i = 0; i < 3; i++) {
                temp.v[i] = (br_scalar)PhysicsJointScalar(PartnerMultiplyScalarWide(&joint->parent_bone_axis.v[i], projection));
            }
            for (i = 0; i < 3; i++) {
                PhysicsSubtractScalar(&temp.v[i], &root->omega.v[i], &temp.v[i]);
            }
            BrMatrix34ApplyV(&world, &temp, &root->transform_matrix);
            BrMatrix34TApplyV(&temp, &world, &object->transform_matrix);
            joint = ApplicationJoint(object, 0);
            projection = PhysicsJointSum(PhysicsJointSum(PhysicsJointProduct(&object->omega.v[0], &joint->hinge_axis.v[0]), PhysicsJointProductMemory(&joint->hinge_axis.v[2], &object->omega.v[2])), PhysicsJointProductMemory(&joint->hinge_axis.v[1], &object->omega.v[1]));
            object->omega.v[0] = (br_scalar)PhysicsJointScalar(PartnerMultiplyScalarWide(&joint->hinge_axis.v[0], projection));
            object->omega.v[1] = (br_scalar)PhysicsJointScalar(PartnerMultiplyScalarWide(&joint->hinge_axis.v[1], projection));
            high = PartnerMultiplyScalarWide(&joint->hinge_axis.v[2], projection);
            PhysicsJointAddVelocity(&object->omega.v[0], temp.v[0]);
            PhysicsJointAddVelocity(&object->omega.v[1], temp.v[1]);
            object->omega.v[2] = (br_scalar)PhysicsJointScalar(high);
            PhysicsJointAddVelocity(&object->omega.v[2], temp.v[2]);
            for (i = 0; i < 3; i++) {
                PhysicsSubtractScalar(&temp.v[i], &joint->field_0x14.v[i], &root->cmpos.v[i]);
            }
            cross.v[0] = (br_scalar)PhysicsJointScalar((double)root->omega.v[1] * temp.v[2] - (double)root->omega.v[2] * temp.v[1]);
            cross.v[1] = (br_scalar)PhysicsJointScalar((double)root->omega.v[2] * temp.v[0] - (double)root->omega.v[0] * temp.v[2]);
            cross.v[2] = (br_scalar)PhysicsJointScalar((double)root->omega.v[0] * temp.v[1] - (double)root->omega.v[1] * temp.v[0]);
            BrMatrix34ApplyV(&temp, &cross, &root->transform_matrix);
            for (i = 0; i < 3; i++) {
                temp.v[i] = (br_scalar)PhysicsJointScalar((double)root->v.v[i] + temp.v[i]);
            }
            BrMatrix34TApplyV(&response, &temp, &object->transform_matrix);
            joint = ApplicationJoint(object, 0);
            for (i = 0; i < 3; i++) {
                PhysicsSubtractScalar(&temp.v[i], &joint->field_0x08.v[i], &object->cmpos.v[i]);
            }
            cross.v[0] = (br_scalar)PhysicsJointScalar((double)object->omega.v[1] * temp.v[2] - (double)object->omega.v[2] * temp.v[1]);
            cross.v[1] = (br_scalar)PhysicsJointScalar((double)object->omega.v[2] * temp.v[0] - (double)object->omega.v[0] * temp.v[2]);
            cross.v[2] = (br_scalar)PhysicsJointScalar((double)object->omega.v[0] * temp.v[1] - (double)object->omega.v[1] * temp.v[0]);
            for (i = 0; i < 3; i++) {
                PhysicsSubtractScalar(&response.v[i], &response.v[i], &cross.v[i]);
            }
            high = PhysicsJointSum(PhysicsJointSum(PhysicsJointProductMemory(&joint->hinge_axis.v[2], &response.v[2]), PhysicsJointProductMemory(&joint->hinge_axis.v[0], &response.v[0])), PhysicsJointProductMemory(&joint->hinge_axis.v[1], &response.v[1]));
            projection = PartnerSubtractWide(high, PhysicsJointSum(PhysicsJointSum(PhysicsJointProduct(&object->velocity_car_space.v[2], &joint->hinge_axis.v[2]), PhysicsJointProduct(&object->velocity_car_space.v[0], &joint->hinge_axis.v[0])), PhysicsJointProduct(&object->velocity_car_space.v[1], &joint->hinge_axis.v[1])));
            for (i = 0; i < 3; i++) {
                temp.v[i] = (br_scalar)PhysicsJointScalar(PartnerMultiplyScalarWide(&joint->hinge_axis.v[i], projection));
            }
            for (i = 0; i < 3; i++) {
                PhysicsJointAddVelocity(&object->velocity_car_space.v[i], temp.v[i]);
            }
            BrMatrix34ApplyV(&object->v, &object->velocity_car_space, &object->transform_matrix);
            break;
        default:
            PhysicsError((tPhysicsError)5, NULL);
        }
    }
    joint = ApplicationJoint(object, 1);
    if (joint != NULL) {
        if (joint->type == 5) {
            ApplicationSecondary(object, output, 0x20, 10, 0);
            ApplicationSecondary(object, output + 1, 0x2c, 10, 0);
            output += 2;
            count += 2;
        } else {
            for (i = 0; i < 3; i++) {
                output->field_0 = 0;
                output->object_a = object;
                output->object_b = NULL;
                output->field_c = 0;
                PhysicsSubtractScalar(&output->point_a.v[0], &ApplicationJoint(object, 1)->field_0x08.v[0], &object->cmpos.v[0]);
                PhysicsSubtractScalar(&output->point_a.v[1], &ApplicationJoint(object, 1)->field_0x08.v[1], &object->cmpos.v[1]);
                PhysicsSubtractScalar(&output->point_a.v[2], &ApplicationJoint(object, 1)->field_0x08.v[2], &object->cmpos.v[2]);
                output->direction_a.v[0] = output->direction_a.v[1] = output->direction_a.v[2] = 0.f;
                output->direction_a.v[i] = 1.f;
                output->type = 10;
                output++;
            }
            count += 3;
            type = ApplicationJoint(object, 1)->type;
            if (type == 2) {
                ApplicationSecondary(object, output, 0x20, 11, 1);
                output++;
                count++;
            } else if (type != 1 && type != 3) {
                PhysicsError((tPhysicsError)5, NULL);
            }
        }
    }
    for (child = object->child; child != NULL; child = child->next) {
        added = AllocateObjectWorkspace(child, output);
        count = (int)((tU32)count + (tU32)added);
        output = (tPhysics_joint_force*)((uintptr_t)output + (tU32)added * 96u);
    }
    return count;
}

static br_scalar* C2_HOOK_CDECL VelocityField(void* base,unsigned offset) {return (br_scalar*)((tU8*)base+offset);}
static double C2_HOOK_CDECL VelocityRead(const void* base,unsigned offset) {return *(const volatile br_scalar*)((const tU8*)base+offset);}
static uintptr_t C2_HOOK_CDECL VelocityPointer(const void* base,unsigned offset) {return *(const volatile tU32*)((const tU8*)base+offset);}
static br_scalar C2_HOOK_CDECL VelocityScalar(double value) {volatile union {br_scalar f;tU32 u;} stored;union {br_scalar f;tU32 u;} result;stored.f=(br_scalar)value;result.u=stored.u;return result.f;}
/* MSVC5 reverses a scalar comparison and drops unordered for !(value >= 0).
 * Keep the original FCOMP C0 test for the demonstrated nonfinite cases. */
static int C2_HOOK_CDECL VelocityBelowOrUnordered(double value,double threshold) {
#if defined(_MSC_VER) && defined(_M_IX86)
 unsigned status;
 __asm {
 fld qword ptr value
 fcomp qword ptr threshold
 fnstsw ax
 movzx eax,ax
 mov status,eax
 }
 return (status & 0x100)!=0;
#else
 return !(value >= threshold);
#endif
}
static int C2_HOOK_CDECL VelocityGreater(double value,double threshold) {
#if defined(_MSC_VER) && defined(_M_IX86)
 unsigned status;
 __asm {
 fld qword ptr value
 fcomp qword ptr threshold
 fnstsw ax
 movzx eax,ax
 mov status,eax
 }
 return (status & 0x4100)==0;
#else
 return value > threshold;
#endif
}
// FUNCTION: CARMA2_HW 0x004c03f0
void C2_HOOK_FASTCALL RestoreListOmega(tPhysics_object* object) {
 while(object!=NULL) {
  tU32 words[3];
  memcpy(words,&object->rotate_omega,12);
  memcpy(&object->omega,words,12);
  if(object->child!=NULL)RestoreListOmega(object->child);
  object=object->next;
 }
}
/* Paired NaN physical counterexamples show modern SSE and whole-TU MSVC5
 * promote the raw matrix operand before subtraction. Keep original FLD source
 * / FSUB matrix operand form on both Microsoft x86 compilers. */
static br_scalar C2_HOOK_CDECL VelocitySubtractScalar(const br_scalar* source,const br_scalar* translation) {
#if defined(_MSC_VER) && defined(_M_IX86)
 br_scalar result;
 __asm {
 mov eax,source
 fld dword ptr [eax]
 mov eax,translation
 fsub dword ptr [eax]
 fstp dword ptr result
 }
 return result;
#else
 return VelocityScalar((double)*source-*translation);
#endif
}
// FUNCTION: CARMA2_HW 0x004225b0
void C2_HOOK_FASTCALL DrMatrix34ApplyLPInverse(br_vector3* dest,const br_vector3* source,const br_matrix34* matrix) {
 br_vector3 relative;
 relative.v[0]=VelocitySubtractScalar(&source->v[0],&matrix->m[3][0]);
 relative.v[1]=VelocitySubtractScalar(&source->v[1],&matrix->m[3][1]);
 relative.v[2]=VelocitySubtractScalar(&source->v[2],&matrix->m[3][2]);
 BrMatrix34TApplyV(dest,&relative,matrix);
}
static const br_scalar VelocityConstant58a768 = 0.0f;
static const double VelocityConstant58a798 = 0.01;
static const double VelocityConstant58a7a0 = 0.001;
static const br_scalar VelocityConstant58a7c8 = 25.0f;
static const double VelocityConstant58a7d8 = -1.001;
static const double VelocityConstant58a7e0 = 25.0;
static const br_scalar VelocityConstant58a800 = 1.0f;
static const br_scalar VelocityConstant58a804 = 0.10000000149011612f;
static const double VelocityConstant58a830 = 0.1;
static const double VelocityConstant58a838 = -0.1;
static const double VelocityConstant58a840 = 0.0099;
// FUNCTION: CARMA2_HW 0x004c0430
int C2_HOOK_FASTCALL MakeVelocitiesVector2(br_scalar* pOutput, tPhysics_joint_force* const* pForces, int pCapacity, int pCount) {
  /* The public pointers describe the real ABI. Raw word cursors preserve
   * output/pointer-array aliases and opaque field_0/field_10 scalar reads;
   * every original pointer-array reload remains volatile. */
  tU32* output_words = (tU32*)pOutput;
  tU32* force_words = (tU32*)pForces;
  int capacity = pCapacity;
  int count = pCount;
  uintptr_t force_address;
  uintptr_t object_a_address;
  uintptr_t object_b_address;
  float *force_fields;
  double value;
  double minimum;
  int has_velocity;
  int has_separation;
  tU32 *force_cursor;
  intptr_t remaining;
  intptr_t cursor_delta;
  intptr_t output_delta;
  intptr_t current_force_address;
  tU32 *output_tail;
  br_vector3 projected,delta,point,other;

  has_velocity = 0;
  if (count < capacity) {
    output_tail = output_words + count;
    for (capacity = capacity - count; capacity != 0; capacity = capacity + -1) {
      *(volatile tU32*)output_tail = 0;
      output_tail = output_tail + 1;
    }
  }
  if (0 < count) {
    output_delta = (intptr_t)output_words - (intptr_t)force_words;
    force_cursor = force_words;
    remaining = count;
    do {
      force_address = *(const volatile tU32*)force_cursor;
      object_a_address = *(int *)(force_address + 4);
      if (*(int *)(force_address + 0x5c) == 0xb) {
        *VelocityField((void*)(output_delta+(intptr_t)force_cursor),0) = VelocityScalar(-((VelocityRead((const void*)(object_a_address + 0x74),0) * VelocityRead((const void*)(force_address + 0x2c),0) +
              VelocityRead((const void*)(object_a_address + 0x78),0) * VelocityRead((const void*)(force_address + 0x30),0)) +
              VelocityRead((const void*)(object_a_address + 0x7c),0) * VelocityRead((const void*)(force_address + 0x34),0)));
        force_address = *(const volatile tU32*)force_cursor;
        object_a_address = *(int *)(force_address + 8);
        if (object_a_address != 0) {
          *VelocityField((void*)(output_delta+(intptr_t)force_cursor),0) = VelocityScalar(((VelocityRead((const void*)(object_a_address + 0x7c),0) * VelocityRead((const void*)(force_address + 0x40),0) +
               VelocityRead((const void*)(object_a_address + 0x74),0) * VelocityRead((const void*)(force_address + 0x38),0)) +
               VelocityRead((const void*)(object_a_address + 0x78),0) * VelocityRead((const void*)(force_address + 0x3c),0)) +
               VelocityRead((const void*)(output_delta+(intptr_t)force_cursor),0));
        }
        if (VelocityConstant58a7a0 <= VelocityRead((const void*)(output_delta+(intptr_t)force_cursor),0)) {
          has_velocity = 1;
        }
        else {
          *(volatile tU32 *)(output_delta + (intptr_t)force_cursor) = 0x3a83126f;
        }
      }
      else {
        projected.v[0] = VelocityScalar(VelocityRead((const void*)(object_a_address + 0x78),0) * VelocityRead((const void*)(force_address + 0x1c),0) -
                   VelocityRead((const void*)(force_address + 0x18),0) * VelocityRead((const void*)(object_a_address + 0x7c),0));
        projected.v[0] = VelocityScalar(VelocityRead((const void*)(object_a_address + 0x1a8),0) + (double)projected.v[0]);
        projected.v[1] = VelocityScalar(VelocityRead((const void*)(object_a_address + 0x7c),0) * VelocityRead((const void*)(force_address + 0x14),0) -
                   VelocityRead((const void*)(force_address + 0x1c),0) * VelocityRead((const void*)(object_a_address + 0x74),0));
        projected.v[1] = VelocityScalar(VelocityRead((const void*)(object_a_address + 0x1ac),0) + (double)projected.v[1]);
        projected.v[2] = VelocityScalar(VelocityRead((const void*)(object_a_address + 0x74),0) * VelocityRead((const void*)(force_address + 0x18),0) -
                   VelocityRead((const void*)(force_address + 0x14),0) * VelocityRead((const void*)(object_a_address + 0x78),0));
        projected.v[2] = VelocityScalar(VelocityRead((const void*)(object_a_address + 0x1b0),0) + (double)projected.v[2]);
        *VelocityField((void*)(output_delta+(intptr_t)force_cursor),0) = VelocityScalar(-((VelocityRead((const void*)(force_address + 0x2c),0) * projected.v[0] +
              VelocityRead((const void*)(force_address + 0x30),0) * projected.v[1]) + VelocityRead((const void*)(force_address + 0x34),0) * projected.v[2]));
        force_address = *(const volatile tU32*)force_cursor;
        object_b_address = *(int *)(force_address + 8);
        if (object_b_address != 0) {
          projected.v[0] = VelocityScalar(VelocityRead((const void*)(object_b_address + 0x78),0) * VelocityRead((const void*)(force_address + 0x28),0) -
                   VelocityRead((const void*)(force_address + 0x24),0) * VelocityRead((const void*)(object_b_address + 0x7c),0));
        projected.v[0] = VelocityScalar(VelocityRead((const void*)(object_b_address + 0x1a8),0) + (double)projected.v[0]);
          projected.v[1] = VelocityScalar(VelocityRead((const void*)(object_b_address + 0x7c),0) * VelocityRead((const void*)(force_address + 0x20),0) -
                   VelocityRead((const void*)(force_address + 0x28),0) * VelocityRead((const void*)(object_b_address + 0x74),0));
        projected.v[1] = VelocityScalar(VelocityRead((const void*)(object_b_address + 0x1ac),0) + (double)projected.v[1]);
          projected.v[2] = VelocityScalar(VelocityRead((const void*)(object_b_address + 0x74),0) * VelocityRead((const void*)(force_address + 0x24),0) -
                   VelocityRead((const void*)(force_address + 0x20),0) * VelocityRead((const void*)(object_b_address + 0x78),0));
        projected.v[2] = VelocityScalar(VelocityRead((const void*)(object_b_address + 0x1b0),0) + (double)projected.v[2]);
          *VelocityField((void*)(output_delta+(intptr_t)force_cursor),0) = VelocityScalar(((VelocityRead((const void*)(force_address + 0x40),0) * projected.v[2] +
               VelocityRead((const void*)(force_address + 0x38),0) * projected.v[0]) + VelocityRead((const void*)(force_address + 0x3c),0) * projected.v[1]) +
               VelocityRead((const void*)(output_delta+(intptr_t)force_cursor),0));
        }
        if (VelocityConstant58a798 < VelocityRead((const void*)(output_delta+(intptr_t)force_cursor),0)) {
          has_velocity = 1;
        }
        if (VelocityConstant58a830 < VelocityRead((const void*)(output_delta+(intptr_t)force_cursor),0)) {
          value = VelocityConstant58a830 - (VelocityRead((const void*)*(const volatile tU32*)force_cursor,0) - VelocityConstant58a800);
          if (VelocityGreater(value,VelocityConstant58a800)) {
            value = VelocityConstant58a800;
          }
          if (!(value >= VelocityConstant58a830)) {
            value = VelocityConstant58a804;
          }
          *VelocityField((void*)(output_delta+(intptr_t)force_cursor),0) = VelocityScalar(value * (VelocityRead((const void*)(output_delta+(intptr_t)force_cursor),0) - VelocityConstant58a830) -
               VelocityConstant58a838);
        }
        if (VelocityBelowOrUnordered(VelocityRead((const void*)(output_delta+(intptr_t)force_cursor),0),VelocityConstant58a768)) {
          *(volatile tU32 *)(output_delta + (intptr_t)force_cursor) = 0;
        }
        value = VelocityConstant58a798 - VelocityRead((const void*)(output_delta+(intptr_t)force_cursor),0) * VelocityConstant58a7d8;
        *VelocityField((void*)(output_delta+(intptr_t)force_cursor),0) = VelocityScalar(value);
        if (((object_b_address == 0) || (*(char *)(object_b_address + 0x1df) != '\0')) &&
           (*(char *)(object_a_address + 0x1df) != '\0')) {
          *VelocityField((void*)(output_delta+(intptr_t)force_cursor),0) = VelocityScalar((double)VelocityRead((const void*)(output_delta+(intptr_t)force_cursor),0) - VelocityConstant58a840);
        }
      }
      force_cursor = force_cursor + 1;
      remaining = remaining + -1;
    } while (remaining != 0);
  }
  if (!has_velocity) {
    has_separation = 0;
    if (0 < count) {
      cursor_delta = (intptr_t)output_words - (intptr_t)force_words;
      capacity = count;
      force_cursor = force_words;
      do {
        force_fields = (float *)*(const volatile tU32*)force_cursor;
        if (*(const tU32*)((tU8*)force_fields+0x5c) == 11) {
          value = (VelocityConstant58a7a0 - *force_fields) * VelocityConstant58a7e0;
          minimum = VelocityConstant58a7a0;
          *VelocityField((void*)((intptr_t)force_cursor+cursor_delta),0) = VelocityScalar(value);
          if (minimum <= value) {
            has_velocity = 1;
          }
          else {
            *(volatile tU32 *)((intptr_t)force_cursor + cursor_delta) = 0x3a83126f;
          }
        }
        else {
          if (*(const tU32*)((tU8*)force_fields+8) == 0) {
            point.v[0] = VelocityScalar(force_fields[5] + VelocityRead((const void*)VelocityPointer(force_fields,4),0x14));
            point.v[1] = VelocityScalar(force_fields[6] + VelocityRead((const void*)VelocityPointer(force_fields,4),0x18));
            point.v[2] = VelocityScalar(force_fields[7] + VelocityRead((const void*)VelocityPointer(force_fields,4),0x1c));
            BrMatrix34ApplyP(&projected,&point,(const br_matrix34*)(VelocityPointer((const void*)VelocityPointer(force_fields,4),0)+0x2c));
            DrMatrix34ApplyLPInverse(&delta,&projected,(const br_matrix34*)(VelocityPointer((const void*)VelocityPointer(force_cursor,0),4)+0x8c));
            delta.v[0] = VelocityScalar((double)delta.v[0] - point.v[0]);
            delta.v[1] = VelocityScalar((double)delta.v[1] - point.v[1]);
            delta.v[2] = VelocityScalar((double)delta.v[2] - point.v[2]);
            current_force_address = *(const volatile tU32*)force_cursor;
            *VelocityField((void*)((intptr_t)force_cursor+cursor_delta),0) = VelocityScalar(VelocityConstant58a798 -
                 ((VelocityRead((const void*)(current_force_address + 0x30),0) * delta.v[1] +
                 VelocityRead((const void*)(current_force_address + 0x34),0) * delta.v[2]) + VelocityRead((const void*)(current_force_address + 0x2c),0) * delta.v[0]) *
                 VelocityConstant58a7c8);
          }
          else {
            point.v[0] = VelocityScalar(force_fields[5] + VelocityRead((const void*)VelocityPointer(force_fields,4),0x14));
            point.v[1] = VelocityScalar(force_fields[6] + VelocityRead((const void*)VelocityPointer(force_fields,4),0x18));
            point.v[2] = VelocityScalar(force_fields[7] + VelocityRead((const void*)VelocityPointer(force_fields,4),0x1c));
            BrMatrix34ApplyP(&projected,&point,(const br_matrix34*)(VelocityPointer(force_fields,4)+0x8c));
            DrMatrix34ApplyLPInverse(&delta,&projected,(const br_matrix34*)(VelocityPointer((const void*)VelocityPointer(force_cursor,0),8)+0x8c));
            BrMatrix34ApplyP(&projected,&point,(const br_matrix34*)(VelocityPointer((const void*)(VelocityPointer((const void*)VelocityPointer(force_cursor,0),4)),0)+0x2c));
            DrMatrix34ApplyLPInverse(&other,&projected,(const br_matrix34*)(VelocityPointer((const void*)VelocityPointer((const void*)VelocityPointer(force_cursor,0),8),0)+0x2c));
            delta.v[0] = VelocityScalar((double)other.v[0] - delta.v[0]);
            current_force_address = *(const volatile tU32*)force_cursor;
            delta.v[1] = VelocityScalar((double)other.v[1] - delta.v[1]);
            delta.v[2] = VelocityScalar((double)other.v[2] - delta.v[2]);
            *VelocityField((void*)((intptr_t)force_cursor+cursor_delta),0) = VelocityScalar(-(((VelocityRead((const void*)(current_force_address + 0x38),0) * delta.v[0] +
                   VelocityRead((const void*)(current_force_address + 0x3c),0) * delta.v[1]) + VelocityRead((const void*)(current_force_address + 0x40),0) * delta.v[2]) *
                  VelocityConstant58a7c8));
            current_force_address = *(const volatile tU32*)force_cursor;
            if (*(int *)(current_force_address + 0x5c) == 5) {
              BrMatrix34ApplyV(&projected,(const br_vector3*)(current_force_address+0x38),(const br_matrix34*)(VelocityPointer((const void*)current_force_address,8)+0x8c));
              BrMatrix34TApplyV(&other,&projected,(const br_matrix34*)(VelocityPointer((const void*)VelocityPointer((const void*)VelocityPointer(force_cursor,0),8),0)+0x2c));
              value = -((((double)other.v[0] * delta.v[0] + (double)other.v[1] * delta.v[1]) + (double)other.v[2] * delta.v[2]) *
                       VelocityConstant58a7c8);
              if (VelocityGreater(value,VelocityRead((const void*)((intptr_t)force_cursor+cursor_delta),0))) {
                *VelocityField((void*)((intptr_t)force_cursor+cursor_delta),0) = VelocityScalar(value);
              }
            }
            if ((VelocityConstant58a768 < VelocityRead((const void*)(*(const volatile tU32*)force_cursor + 0x10),0)) &&
               (value = VelocityRead((const void*)(*(const volatile tU32*)force_cursor + 0x10),0) * VelocityConstant58a7c8,
               VelocityGreater(value,VelocityRead((const void*)((intptr_t)force_cursor+cursor_delta),0)))) {
              *VelocityField((void*)((intptr_t)force_cursor+cursor_delta),0) = VelocityScalar(value);
            }
          }
          if (!(VelocityRead((const void*)((intptr_t)force_cursor+cursor_delta),0) > VelocityConstant58a830)) {
            if (VelocityBelowOrUnordered(VelocityRead((const void*)((intptr_t)force_cursor+cursor_delta),0),VelocityConstant58a768)) {
              *(volatile tU32 *)((intptr_t)force_cursor + cursor_delta) = 0;
            }
          }
          else {
            value = VelocityConstant58a830 - (VelocityRead((const void*)*(const volatile tU32*)force_cursor,0) - VelocityConstant58a800);
            if (VelocityGreater(value,VelocityConstant58a800)) {
              value = VelocityConstant58a800;
            }
            if (!(value >= VelocityConstant58a830)) {
              value = VelocityConstant58a804;
            }
            *VelocityField((void*)((intptr_t)force_cursor+cursor_delta),0) = VelocityScalar(value * (VelocityRead((const void*)((intptr_t)force_cursor+cursor_delta),0) - VelocityConstant58a830) -
                 VelocityConstant58a838);
          }
          if (VelocityConstant58a7a0 < VelocityRead((const void*)((intptr_t)force_cursor+cursor_delta),0)) {
            has_separation = 1;
          }
          *VelocityField((void*)((intptr_t)force_cursor+cursor_delta),0) = VelocityScalar(VelocityConstant58a798 - VelocityRead((const void*)((intptr_t)force_cursor+cursor_delta),0) * VelocityConstant58a7d8);
        }
        force_cursor = force_cursor + 1;
        capacity = capacity + -1;
      } while (capacity != 0);
    }
    if ((!has_separation) && (0 < count)) {
      cursor_delta = (intptr_t)force_words - (intptr_t)output_words;
      do {
        if (*(int *)(*(int *)(cursor_delta + (intptr_t)output_words) + 0x5c) != 0xb) {
          *(volatile tU32*)output_words = 0x3d4ccccd;
        }
        output_words = output_words + 1;
        count = count + -1;
      } while (count != 0);
    }
  }
  return !has_velocity;
}

// RestoreGroupOmegas

// MakeVelocitiesVector

// NewAddFrictionObjectObject

// NewAddFriction

// ApplyForces

// DoJointFriction

// ListDoJointFriction

// PostAddFriction

// RotateObjectSecondOrder

// RotateObjectFirstOrder

// RotateObject

// FUNCTION: CARMA2_HW 0x004b8fe0
void C2_HOOK_FAKE_THISCALL TranslateObject(tPhysics_object* pObject, undefined4 pUnused, br_scalar pTime) {
    volatile tPhysics_object* object = pObject;
    tPhysics_joint* joint = object->physics_joint2;
    br_vector3 axis, point, delta;
    br_scalar before, speed[3];
    double x, y, z, projection, correction;
    br_actor* actor;

    (void)pUnused;
    if (joint->type == 5) {
        BrMatrix34ApplyV(&axis, &joint->hinge_axis, &pObject->transform_matrix);
        actor = object->actor;
        /* Original FADDP first combines Z+Y, then adds X before the scalar spill. */
        before = (br_scalar)PhysicsJointScalar(PhysicsJointSum(PhysicsJointSum(PhysicsJointProductMemory(&actor->t.t.mat.m[3][2], &axis.v[2]), PhysicsJointProductMemory(&actor->t.t.mat.m[3][1], &axis.v[1])), PhysicsJointProductMemory(&actor->t.t.mat.m[3][0], &axis.v[0])));
        joint = object->physics_joint2;
        BrMatrix34ApplyP(&point, &joint->field_0x08, &actor->t.t.mat);
        x = PartnerDifferenceScalars(&pObject->field_0x17c.v[0], &point.v[0]);
        z = PartnerDifferenceScalars(&pObject->field_0x17c.v[2], &point.v[2]);
        y = PartnerDifferenceScalars(&pObject->field_0x17c.v[1], &point.v[1]);
        delta.v[1] = (br_scalar)PhysicsJointScalar(y);
        delta.v[2] = (br_scalar)PhysicsJointScalar(z);
        actor = object->actor;
        actor->t.t.mat.m[3][0] = (br_scalar)PhysicsJointScalar(PhysicsJointAddScalar(x, &actor->t.t.mat.m[3][0]));
        actor = object->actor;
        actor->t.t.mat.m[3][1] = (br_scalar)PhysicsJointScalar(PhysicsJointAddScalar((double)delta.v[1], &actor->t.t.mat.m[3][1]));
        actor = object->actor;
        actor->t.t.mat.m[3][2] = (br_scalar)PhysicsJointScalar(PhysicsJointAddScalar((double)delta.v[2], &actor->t.t.mat.m[3][2]));
        actor = object->actor;
        projection = PhysicsJointSum(PhysicsJointSum(PhysicsJointProductMemory(&actor->t.t.mat.m[3][0], &axis.v[0]), PhysicsJointProductMemory(&actor->t.t.mat.m[3][2], &axis.v[2])), PhysicsJointProductMemory(&actor->t.t.mat.m[3][1], &axis.v[1]));
        correction = PartnerReverseSubtractScalar(projection, &before);
        delta.v[0] = (br_scalar)PhysicsJointScalar(PartnerMultiplyRawScalar(correction, &axis.v[0]));
        delta.v[1] = (br_scalar)PhysicsJointScalar(PartnerMultiplyRawScalar(correction, &axis.v[1]));
        delta.v[2] = (br_scalar)PhysicsJointScalar(PartnerMultiplyRawScalar(correction, &axis.v[2]));
        actor->t.t.mat.m[3][0] = (br_scalar)PhysicsJointScalar(PhysicsJointAddScalar((double)delta.v[0], &actor->t.t.mat.m[3][0]));
        actor = object->actor;
        actor->t.t.mat.m[3][1] = (br_scalar)PhysicsJointScalar(PhysicsJointAddScalar((double)delta.v[1], &actor->t.t.mat.m[3][1]));
        actor = object->actor;
        actor->t.t.mat.m[3][2] = (br_scalar)PhysicsJointScalar(PhysicsJointAddScalar((double)delta.v[2], &actor->t.t.mat.m[3][2]));
        projection = PhysicsJointSum(PhysicsJointSum(PhysicsJointProductMemory(&pObject->v.v[2], &axis.v[2]), PhysicsJointProductMemory(&pObject->v.v[1], &axis.v[1])), PhysicsJointProductMemory(&pObject->v.v[0], &axis.v[0]));
        speed[0] = (br_scalar)PhysicsJointScalar(PartnerMultiplyRawScalar(projection, &axis.v[0]));
        speed[1] = (br_scalar)PhysicsJointScalar(PartnerMultiplyRawScalar(projection, &axis.v[1]));
        speed[2] = (br_scalar)PhysicsJointScalar(PartnerMultiplyRawScalar(projection, &axis.v[2]));
        object->v.v[1] = speed[1];
        object->v.v[2] = speed[2];
        object->v.v[0] = speed[0];
        actor = object->actor;
        actor->t.t.mat.m[3][0] = (br_scalar)PhysicsJointScalar(PhysicsJointAddScalar(PhysicsJointProductMemory(&pTime, &speed[0]), &actor->t.t.mat.m[3][0]));
        actor = object->actor;
        actor->t.t.mat.m[3][1] = (br_scalar)PhysicsJointScalar(PhysicsJointAddScalar(PhysicsJointProductMemory(&pTime, &speed[1]), &actor->t.t.mat.m[3][1]));
        actor = object->actor;
        actor->t.t.mat.m[3][2] = (br_scalar)PhysicsJointScalar(PhysicsJointAddScalar(PhysicsJointProductMemory(&pTime, &speed[2]), &actor->t.t.mat.m[3][2]));
    } else {
        BrMatrix34ApplyP(&point, &joint->field_0x08, &object->actor->t.t.mat);
        x = PartnerDifferenceScalars(&pObject->field_0x17c.v[0], &point.v[0]);
        z = PartnerDifferenceScalars(&pObject->field_0x17c.v[2], &point.v[2]);
        y = PartnerDifferenceScalars(&pObject->field_0x17c.v[1], &point.v[1]);
        delta.v[1] = (br_scalar)PhysicsJointScalar(y);
        delta.v[2] = (br_scalar)PhysicsJointScalar(z);
        actor = object->actor;
        actor->t.t.mat.m[3][0] = (br_scalar)PhysicsJointScalar(PhysicsJointAddScalar(x, &actor->t.t.mat.m[3][0]));
        actor = object->actor;
        actor->t.t.mat.m[3][1] = (br_scalar)PhysicsJointScalar(PhysicsJointAddScalar((double)delta.v[1], &actor->t.t.mat.m[3][1]));
        actor = object->actor;
        actor->t.t.mat.m[3][2] = (br_scalar)PhysicsJointScalar(PhysicsJointAddScalar((double)delta.v[2], &actor->t.t.mat.m[3][2]));
    }
}

// FUNCTION: CARMA2_HW 0x004c2a10
void C2_HOOK_FASTCALL WriteNewPositions(tPhysics_object* pObject) {
    volatile tPhysics_object* object = pObject;
    tPhysics_object* child = object->child;
    tPhysics_object* root = pObject;
    br_vector3 parent_point, child_point;
    br_actor* actor;
    int i;

    if (object->flags & 0x400) {
        root = object->field_0x230;
    }
    while (child != NULL) {
        if (!(object->flags & 0x400)) {
            BrMatrix34ApplyP(&parent_point, &child->physics_joint1->field_0x14, &object->actor->t.t.mat);
            BrMatrix34ApplyP(&child_point, &child->physics_joint1->field_0x08, &child->actor->t.t.mat);
            for (i = 0; i < 3; i++) {
                parent_point.v[i] = (br_scalar)PhysicsJointScalar(PartnerDifferenceScalars(&parent_point.v[i], &child_point.v[i]));
            }
            for (i = 0; i < 3; i++) {
                actor = ((volatile tPhysics_object*)child)->actor;
                actor->t.t.mat.m[3][i] = (br_scalar)PhysicsJointScalar(PhysicsJointAddScalar((double)parent_point.v[i], &actor->t.t.mat.m[3][i]));
            }
        } else {
            BrMatrix34Mul(&object->actor->t.t.mat, &pObject->field_0x1e8, &root->actor->t.t.mat);
        }
        WriteNewPositions(child);
        child = ((volatile tPhysics_object*)child)->next;
    }
}

// GetFacesNearObject

// ResetObjectAndChildren

// ResetObjectList

// RepositionObjectGroup

// SimpleRotate

// InterpolateSingleObject

// InterpolateObjects

// ApplyGravitySingleObject

// ApplyGravity

// SetGravityDeltaV

// TrialPositionChildren

/* The original stores all three differences before adding child translation. */
static void C2_HOOK_CDECL PhysicsPositionChildrenAdjust(br_vector3* pParent_point, const br_vector3* pChild_point, br_vector3* pTranslation) {
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, pParent_point
        mov ecx, pChild_point
        mov edx, pTranslation
        fld dword ptr [eax]
        fsub dword ptr [ecx]
        fstp dword ptr [eax]
        fld dword ptr [eax + 4]
        fsub dword ptr [ecx + 4]
        fstp dword ptr [eax + 4]
        fld dword ptr [eax + 8]
        fsub dword ptr [ecx + 8]
        fstp dword ptr [eax + 8]
        fld dword ptr [eax]
        fadd dword ptr [edx]
        fstp dword ptr [edx]
        fld dword ptr [eax + 4]
        fadd dword ptr [edx + 4]
        fstp dword ptr [edx + 4]
        fld dword ptr [eax + 8]
        fadd dword ptr [edx + 8]
        fstp dword ptr [edx + 8]
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile(
        ".intel_syntax noprefix\n\t"
        "fld dword ptr [eax]\n\t"
        "fsub dword ptr [ecx]\n\t"
        "fstp dword ptr [eax]\n\t"
        "fld dword ptr [eax + 4]\n\t"
        "fsub dword ptr [ecx + 4]\n\t"
        "fstp dword ptr [eax + 4]\n\t"
        "fld dword ptr [eax + 8]\n\t"
        "fsub dword ptr [ecx + 8]\n\t"
        "fstp dword ptr [eax + 8]\n\t"
        "fld dword ptr [eax]\n\t"
        "fadd dword ptr [edx]\n\t"
        "fstp dword ptr [edx]\n\t"
        "fld dword ptr [eax + 4]\n\t"
        "fadd dword ptr [edx + 4]\n\t"
        "fstp dword ptr [edx + 4]\n\t"
        "fld dword ptr [eax + 8]\n\t"
        "fadd dword ptr [edx + 8]\n\t"
        "fstp dword ptr [edx + 8]\n\t"
        ".att_syntax prefix\n\t"
        :
        : "a"(pParent_point), "c"(pChild_point), "d"(pTranslation)
        : "memory", "st");
#else
    int i;

    for (i = 0; i < 3; i++) {
        pParent_point->v[i] = PhysicsScalar((double)pParent_point->v[i] - pChild_point->v[i]);
    }
    for (i = 0; i < 3; i++) {
        pTranslation->v[i] = PhysicsScalar((double)pParent_point->v[i] + pTranslation->v[i]);
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004c2b20
void C2_HOOK_FASTCALL InternalPositionChildren(tPhysics_object* pParent, tPhysics_object* pRoot) {
    tPhysics_object* child;
    br_vector3 parent_point;
    br_vector3 child_point;

    child = pParent->child;
    if (!(pParent->flags & 0x400)) {
        pRoot = pParent;
    }
    for (; child != NULL; child = child->next) {
        if (!(pParent->flags & 0x400)) {
            BrMatrix34ApplyP(&parent_point, &child->physics_joint1->field_0x14, &pParent->transform_matrix);
            BrMatrix34ApplyP(&child_point, &child->physics_joint1->field_0x08, &child->transform_matrix);
            PhysicsPositionChildrenAdjust(&parent_point, &child_point, (br_vector3*)child->transform_matrix.m[3]);
        } else {
            BrMatrix34Mul(&pParent->transform_matrix, &pParent->field_0x1e8, &pRoot->transform_matrix);
        }
        InternalPositionChildren(child, pRoot);
    }
}

// FUNCTION: CARMA2_HW 0x004c2b10
void C2_HOOK_FASTCALL PositionChildren(tPhysics_object* pObject) {
    InternalPositionChildren(pObject, pObject);
}

// ApplyTorque

// ApplyDiminishingTorque

// OrderObjectListOnX

// SwapPair

// CheckObjectsPostionInList

// TestForWrongAnswer

// RQIterativeImprovement

/* RQ arithmetic preserves the original scalar spills and NaN operand order. */
static br_scalar C2_HOOK_CDECL RQScalar(double x) {
    volatile br_scalar result = (br_scalar)x;
    return result;
}

static int C2_HOOK_CDECL RQNaN(double x) {
    return x != x;
}

/* The retained NaN191/192 probes first diverge at the x87 dot accumulation.
 * Head, denominator and all preceding public writes match; SSE accumulation
 * chooses a different sign when equal NaN payloads have opposite signs. */
static double C2_HOOK_CDECL RQAccumulate(double sum, const volatile br_scalar* left, const volatile br_scalar* right) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        fld qword ptr sum
        mov eax, left
        fld dword ptr [eax]
        mov eax, right
        fmul dword ptr [eax]
        faddp st(1), st(0)
        fstp qword ptr result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("fldl %1; flds %2; fmuls %3; faddp; fstpl %0"
        : "=m"(result) : "m"(sum), "m"(*left), "m"(*right) : "st", "st(1)");
#else
#error RQAccumulate requires x86
#endif
    return result;
}

/* With dot order fixed, retained first-coefficient snapshots match. The
 * next public update still differs under SSE when multiplier NaN signs tie. */
static br_scalar C2_HOOK_CDECL RQSubtractProduct(br_scalar coefficient, const br_scalar* factor, const br_scalar* value) {
    br_scalar result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        fld dword ptr coefficient
        mov eax, factor
        fmul dword ptr [eax]
        mov eax, value
        fsubr dword ptr [eax]
        fstp dword ptr result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("flds %1; fmuls %2; fsubrs %3; fstps %0"
        : "=m"(result) : "m"(coefficient), "m"(*factor), "m"(*value) : "st");
#else
#error RQSubtractProduct requires x86
#endif
    return result;
}

/* Paired raw NaNs in the retained solve cases require memory-operand
 * division and multiplication before the scalar spill. */
static br_scalar C2_HOOK_CDECL RQDivideScalars(const br_scalar* numerator, const br_scalar* denominator) {
    br_scalar result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, numerator
        fld dword ptr [eax]
        mov eax, denominator
        fdiv dword ptr [eax]
        fstp dword ptr result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("flds %1; fdivs %2; fstps %0"
        : "=m"(result) : "m"(*numerator), "m"(*denominator) : "st");
#else
#error RQDivideScalars requires x86
#endif
    return result;
}

static br_scalar C2_HOOK_CDECL RQMultiplyScalars(const br_scalar* left, const volatile br_scalar* right) {
    br_scalar result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, left
        fld dword ptr [eax]
        mov eax, right
        fmul dword ptr [eax]
        fstp dword ptr result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("flds %1; fmuls %2; fstps %0"
        : "=m"(result) : "m"(*left), "m"(*right) : "st");
#else
#error RQMultiplyScalars requires x86
#endif
    return result;
}

// FUNCTION: CARMA2_HW 0x004c4850
void C2_HOOK_FASTCALL RQExpand(br_scalar* matrix, const br_scalar* original, int old_size, int size, br_scalar* orthogonal, const unsigned char* indices) {
    br_scalar row[100];
    br_scalar maximum_scalar, diagonal_scalar, denominator, coefficient_scalar;
    volatile br_scalar first;
    volatile double root;
    double value, sum, maximum, diagonal, length, head, coefficient;
    const br_scalar* selected;
    int i, j, k;

    for (j = old_size; j < size; j++) {
        selected = &original[100 * indices[j]];
        for (i = 0; i < old_size; i++) {
            row[i] = RQScalar(selected[indices[i]]);
            matrix[i * 100 + j] = row[i];
        }
        for (i = 0; i < old_size; i++) {
            sum = 0.0;
            for (k = 0; k < old_size; k++) {
                sum = RQAccumulate(sum, &row[k], &orthogonal[i * 100 + k]);
            }
            matrix[j * 100 + i] = RQScalar(sum);
        }
    }

    for (i = old_size; i < size; i++) {
        selected = &original[100 * indices[i]];
        for (j = old_size; j < size; j++) {
            memcpy(&matrix[i * 100 + j], &selected[indices[j]], sizeof(br_scalar));
            orthogonal[i * 100 + j] = 0.0f;
        }
        for (j = 0; j < old_size; j++) {
            orthogonal[i * 100 + j] = 0.0f;
            orthogonal[j * 100 + i] = 0.0f;
        }
        orthogonal[i * 101] = 1.0f;
    }

    /* Extend each existing reflector through the appended columns. */
    for (i = 0; i < old_size; i++) {
        diagonal_scalar = RQScalar(matrix[i * 101]);
        maximum_scalar = 0.0f;
        for (k = old_size; k < size; k++) {
            value = fabs(matrix[i * 100 + k]);
            if (!(maximum_scalar > value)) {
                maximum_scalar = RQScalar(value);
            }
        }
        if (maximum_scalar == 0.0f || RQNaN(maximum_scalar)) {
            continue;
        }
        maximum = fabs(diagonal_scalar);
        if (maximum_scalar > maximum) {
            maximum = maximum_scalar;
        }
        for (k = old_size; k < size; k++) {
            matrix[i * 100 + k] = RQScalar((double)matrix[i * 100 + k] / maximum);
        }
        diagonal = (double)diagonal_scalar / maximum;
        sum = 0.0;
        for (k = old_size; k < size; k++) {
            sum = RQAccumulate(sum, &matrix[i * 100 + k], &matrix[i * 100 + k]);
        }
        root = sqrt(diagonal * diagonal + sum);
        length = fabs(root);
        if (!(diagonal > 0.0)) {
            length = -length;
        }
        head = PartnerAddWide(diagonal, length);
        first = RQScalar(head);
        denominator = RQScalar(PartnerMultiplyWide(head, length));
        for (j = 0; j < size; j++) {
            sum = PartnerMultiplyRawScalar(first, &orthogonal[i * 100 + j]);
            for (k = old_size; k < size; k++) {
                sum = RQAccumulate(sum, &orthogonal[k * 100 + j], &matrix[i * 100 + k]);
            }
            coefficient = sum / denominator;
            for (k = old_size; k < size; k++) {
                orthogonal[k * 100 + j] = RQScalar(PartnerReverseSubtractScalar(PartnerMultiplyRawScalar(coefficient, &matrix[i * 100 + k]), &orthogonal[k * 100 + j]));
            }
            orthogonal[i * 100 + j] = RQScalar(PartnerReverseSubtractScalar(PartnerMultiplyWide(first, coefficient), &orthogonal[i * 100 + j]));
        }
        for (j = i + 1; j < size; j++) {
            sum = PartnerMultiplyRawScalar(first, &matrix[j * 100 + i]);
            for (k = old_size; k < size; k++) {
                sum = RQAccumulate(sum, &matrix[j * 100 + k], &matrix[i * 100 + k]);
            }
            coefficient = sum / denominator;
            for (k = old_size; k < size; k++) {
                matrix[j * 100 + k] = RQScalar(PartnerReverseSubtractScalar(PartnerMultiplyRawScalar(coefficient, &matrix[i * 100 + k]), &matrix[j * 100 + k]));
            }
            matrix[j * 100 + i] = RQScalar(PartnerReverseSubtractScalar(PartnerMultiplyWide(first, coefficient), &matrix[j * 100 + i]));
        }
        matrix[i * 101] = RQScalar(-PartnerMultiplyWide(length, maximum));
        for (k = old_size; k < size; k++) {
            matrix[i * 100 + k] = 0.0f;
        }
    }

    /* Factor the new rows with the original fixed matrix stride. */
    for (i = old_size; i < size; i++) {
        maximum_scalar = 0.0f;
        for (k = i; k < size; k++) {
            value = fabs(matrix[i * 100 + k]);
            if (!(maximum_scalar > value)) {
                maximum_scalar = RQScalar(value);
            }
        }
        if (maximum_scalar == 0.0f || RQNaN(maximum_scalar)) {
            continue;
        }
        for (k = i; k < size; k++) {
            matrix[i * 100 + k] = RQDivideScalars(&matrix[i * 100 + k], &maximum_scalar);
        }
        sum = 0.0;
        for (k = i; k < size; k++) {
            sum = RQAccumulate(sum, &matrix[i * 100 + k], &matrix[i * 100 + k]);
        }
        root = sqrt(sum);
        length = fabs(root);
        if (!(matrix[i * 101] > 0.0f)) {
            length = -length;
        }
        head = PartnerAddScalar(length, &matrix[i * 101]);
        denominator = RQScalar(PartnerMultiplyWide(length, head));
        matrix[i * 101] = RQScalar(head);
        for (j = 0; j < size; j++) {
            sum = 0.0;
            for (k = i; k < size; k++) {
                sum = RQAccumulate(sum, &matrix[i * 100 + k], &orthogonal[k * 100 + j]);
            }
            coefficient_scalar = RQScalar(sum / denominator);
            for (k = i; k < size; k++) {
                orthogonal[k * 100 + j] = RQSubtractProduct(coefficient_scalar, &matrix[i * 100 + k], &orthogonal[k * 100 + j]);
            }
        }
        for (j = i + 1; j < size; j++) {
            sum = 0.0;
            for (k = i; k < size; k++) {
                sum = RQAccumulate(sum, &matrix[j * 100 + k], &matrix[i * 100 + k]);
            }
            coefficient_scalar = RQScalar(sum / denominator);
            for (k = i; k < size; k++) {
                matrix[j * 100 + k] = RQSubtractProduct(coefficient_scalar, &matrix[i * 100 + k], &matrix[j * 100 + k]);
            }
        }
        matrix[i * 101] = RQScalar(-PartnerMultiplyRawScalar(length, &maximum_scalar));
        for (k = i + 1; k < size; k++) {
            matrix[i * 100 + k] = 0.0f;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004c4f40
void C2_HOOK_FASTCALL RQSingleExpand(br_scalar* matrix, const br_scalar* original, int size, br_scalar* orthogonal, const unsigned char* indices) {
    br_scalar row[100];
    br_scalar magnitude, second, second_coefficient, first_coefficient;
    /* The original reloads the rounded head for its coefficient division. */
    volatile br_scalar first;
    /* Keep FABS after sqrt: GCC otherwise drops it even for negative NaNs. */
    volatile double root;
    br_scalar updated_row, updated_matrix;
    double sum, maximum, diagonal, length, head, denominator;
    int i, j;
    if (size >= 0) {
        for (i = 0; i <= size; i++) {
            memcpy(&row[i], &original[100 * indices[size] + indices[i]], sizeof(br_scalar));
        }
    }
    for (i = 0; i < size; i++) {
        sum = 0.0;
        for (j = 0; j < size; j++) {
            sum = RQAccumulate(sum, &orthogonal[i * 100 + j], &row[j]);
        }
        matrix[size * 100 + i] = RQScalar(sum);
    }
    orthogonal[size * 101] = 1.0f;
    for (i = 0; i < size; i++) {
        orthogonal[i * 100 + size] = 0.0f;
        orthogonal[size * 100 + i] = 0.0f;
    }
    for (i = 0; i < size; i++) {
        magnitude = RQScalar(fabs(row[i]));
        matrix[i * 100 + size] = 0.0f;
        if (magnitude == 0.0f || RQNaN(magnitude)) {
            continue;
        }
        maximum = fabs(matrix[i * 101]);
        if (magnitude > maximum) {
            maximum = magnitude;
        }
        second = RQScalar((double)row[i] / maximum);
        diagonal = (double)matrix[i * 101] / maximum;
        root = sqrt(diagonal * diagonal + (double)second * second);
        length = fabs(root);
        row[i] = second;
        if (!(diagonal > 0.0)) {
            length = -length;
        }
        head = PartnerAddWide(diagonal, length);
        first = RQScalar(head);
        denominator = head * length;
        second_coefficient = RQScalar((double)second / denominator);
        first_coefficient = RQScalar((double)first / denominator);
        for (j = 0; j <= size; j++) {
            sum = PartnerAddWide(PartnerMultiplyRawScalar(first, &orthogonal[i * 100 + j]), PartnerMultiplyRawScalar(second, &orthogonal[size * 100 + j]));
            orthogonal[size * 100 + j] = RQScalar(PartnerReverseSubtractScalar(PartnerMultiplyWide(second_coefficient, sum), &orthogonal[size * 100 + j]));
            orthogonal[i * 100 + j] = RQScalar(PartnerReverseSubtractScalar(PartnerMultiplyWide(first_coefficient, sum), &orthogonal[i * 100 + j]));
        }
        for (j = i + 1; j <= size; j++) {
            sum = PartnerAddWide(PartnerMultiplyRawScalar(first, &matrix[j * 100 + i]), PartnerMultiplyRawScalar(second, &row[j]));
            updated_row = RQScalar(PartnerReverseSubtractScalar(PartnerMultiplyWide(second_coefficient, sum), &row[j]));
            updated_matrix = RQScalar(PartnerReverseSubtractScalar(PartnerMultiplyWide(first_coefficient, sum), &matrix[j * 100 + i]));
            row[j] = updated_row;
            matrix[j * 100 + i] = updated_matrix;
        }
        matrix[i * 101] = RQScalar(-PartnerMultiplyWide(length, maximum));
    }
    memcpy(&matrix[size * 101], &row[size], sizeof(br_scalar));
}

/* Semantic name inferred from the complete original 0x004c5220 entry.
 * Cached records expose a count at0, Q pointer at0x14 and R pointer at0x18;
 * the rest of their layout is not required or inferred here. */
// FUNCTION: CARMA2_HW 0x004c5220
void C2_HOOK_FASTCALL RQFromCachedGroups(br_scalar* matrix, br_scalar* orthogonal, const br_scalar* original, br_scalar* result, const br_scalar* rhs, const unsigned char* indices, int size, const void* const* cached_groups, int group_count) {
    int total, group, count, i, j, k;
    const void* record;
    const br_scalar* cached;
    total = 0;
    for (group = 0; group < group_count; group++) {
        record = (const void*)ForcePointerAt(cached_groups, group * sizeof(void*));
        count = (int)ForcePointerAt(record, 0);
        for (i = 0; i < count; i++) {
            for (j = 0; j < count; j++) {
                record = (const void*)ForcePointerAt(cached_groups, group * sizeof(void*));
                cached = (const br_scalar*)ForcePointerAt(record, 0x18);
                memcpy(&matrix[(total + i) * 100 + total + j], &cached[i * count + j], sizeof(br_scalar));
                record = (const void*)ForcePointerAt(cached_groups, group * sizeof(void*));
                cached = (const br_scalar*)ForcePointerAt(record, 0x14);
                memcpy(&orthogonal[(total + i) * 100 + total + j], &cached[i * count + j], sizeof(br_scalar));
            }
            for (k = 0; k < total; k++) {
                matrix[k * 100 + total + i] = 0.0f;
                matrix[(total + i) * 100 + k] = 0.0f;
                orthogonal[k * 100 + total + i] = 0.0f;
                orthogonal[(total + i) * 100 + k] = 0.0f;
            }
        }
        total += count;
    }
    if (size - total == 1) {
        RQSingleExpand(matrix, original, total, orthogonal, indices);
    } else if (total < size) {
        RQExpand(matrix, original, total, size, orthogonal, indices);
    }
    for (i = 0; i < size; i++) {
        memcpy(&result[i], &rhs[indices[i]], sizeof(br_scalar));
    }
}

// FUNCTION: CARMA2_HW 0x004c5830
int C2_HOOK_FASTCALL RQDcmpV(br_scalar* matrix, int size, br_scalar* orthogonal) {
    int i, j, k;
    int singular = 0;
    br_scalar maximum, denominator, coefficient, other_coefficient;
    double value, sum, length, head;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            orthogonal[i * size + j] = 0.0f;
        }
        orthogonal[i * size + i] = 1.0f;
    }
    for (i = 0; i < size; i++) {
        maximum = 0.0f;
        for (k = i; k < size; k++) {
            value = fabs(matrix[i * size + k]);
            if (!(maximum > value)) {
                maximum = RQScalar(value);
            }
        }
        if (maximum == 0.0f || RQNaN(maximum)) {
            singular = 1;
            continue;
        }
        for (k = i; k < size; k++) {
            matrix[i * size + k] = RQDivideScalars(&matrix[i * size + k], &maximum);
        }
        sum = 0.0;
        for (k = i; k < size; k++) {
            sum = RQAccumulate(sum, &matrix[i * size + k], &matrix[i * size + k]);
        }
        length = fabs(sqrt(sum));
        if (!(matrix[i * size + i] > 0.0f)) {
            length = -length;
        }
        head = length + matrix[i * size + i];
        matrix[i * size + i] = RQScalar(head);
        denominator = RQScalar(length * head);
        for (j = 0; j <= i; j++) {
            sum = 0.0;
            for (k = i; k < size; k++) {
                sum = RQAccumulate(sum, &matrix[i * size + k], &orthogonal[k * size + j]);
            }
            coefficient = RQScalar(sum / denominator);
            for (k = i; k < size; k++) {
                orthogonal[k * size + j] = RQSubtractProduct(coefficient, &matrix[i * size + k], &orthogonal[k * size + j]);
            }
        }
        for (j = i + 1; j < size; j++) {
            sum = 0.0;
            for (k = i; k < size; k++) {
                sum = RQAccumulate(sum, &matrix[i * size + k], &matrix[j * size + k]);
            }
            coefficient = RQScalar(sum / denominator);
            sum = 0.0;
            for (k = i; k < size; k++) {
                sum = RQAccumulate(sum, &orthogonal[k * size + j], &matrix[i * size + k]);
            }
            other_coefficient = RQScalar(sum / denominator);
            for (k = i; k < size; k++) {
                matrix[j * size + k] = RQSubtractProduct(coefficient, &matrix[i * size + k], &matrix[j * size + k]);
                orthogonal[k * size + j] = RQSubtractProduct(other_coefficient, &matrix[i * size + k], &orthogonal[k * size + j]);
            }
        }
        matrix[i * size + i] = RQScalar(-(length * maximum));
        for (j = i + 1; j < size; j++) {
            matrix[i * size + j] = 0.0f;
        }
    }
    return singular;
}

// MakeIdentity

// FUNCTION: CARMA2_HW 0x004c53b0
void C2_HOOK_FASTCALL SparseRQDcmpV(br_scalar* matrix, br_scalar* orthogonal, const br_scalar* rhs, br_scalar* result, int size) {
    br_scalar values[40];
    unsigned char columns[40];
    unsigned short rows[40];
    volatile br_scalar intermediate[100];
    int i, j, k, count;
    br_scalar maximum, denominator, coefficient, other_coefficient;
    double value, sum, length, head;
    if (size < 10) {
        RQDcmpV(matrix, size, orthogonal);
    } else {
        for (i = 0; i < size; i++) {
            for (j = 0; j < size; j++) {
                orthogonal[i * size + j] = 0.0f;
            }
            orthogonal[i * size + i] = 1.0f;
        }
        for (i = 0; i < size; i++) {
            values[0] = RQScalar(matrix[i * size + i]);
            maximum = RQScalar(fabs(matrix[i * size + i]));
            rows[0] = (unsigned short)(size * i);
            columns[0] = (unsigned char)i;
            count = 1;
            for (j = i + 1; j < size; j++) {
                value = matrix[i * size + j];
                if (value != 0.0 && !RQNaN(value)) {
                    value = fabs(value);
                    if (!(maximum > value)) {
                        maximum = RQScalar(value);
                    }
                    columns[count] = (unsigned char)j;
                    rows[count] = (unsigned short)(j * size);
                    memcpy(&values[count], &matrix[i * size + j], 4);
                    count++;
                    matrix[i * size + j] = 0.0f;
                }
            }
            if (maximum == 0.0f || RQNaN(maximum)) {
                continue;
            }
            for (k = 0; k < count; k++) {
                values[k] = RQDivideScalars(&values[k], &maximum);
            }
            sum = 0.0;
            for (k = 0; k < count; k++) {
                sum = RQAccumulate(sum, &values[k], &values[k]);
            }
            length = fabs(sqrt(sum));
            if (!(values[0] > 0.0f)) {
                length = -length;
            }
            head = values[0] + length;
            values[0] = RQScalar(head);
            denominator = RQScalar(head * length);
            for (j = 0; j <= i; j++) {
                sum = 0.0;
                for (k = 0; k < count; k++) {
                    sum = RQAccumulate(sum, &orthogonal[rows[k] + j], &values[k]);
                }
                coefficient = RQScalar(sum / denominator);
                for (k = 0; k < count; k++) {
                    orthogonal[rows[k] + j] = RQSubtractProduct(coefficient, &values[k], &orthogonal[rows[k] + j]);
                }
            }
            for (j = i + 1; j < size; j++) {
                sum = 0.0;
                for (k = 0; k < count; k++) {
                    sum = RQAccumulate(sum, &matrix[j * size + columns[k]], &values[k]);
                }
                coefficient = RQScalar(sum / denominator);
                sum = 0.0;
                for (k = 0; k < count; k++) {
                    sum = RQAccumulate(sum, &orthogonal[rows[k] + j], &values[k]);
                }
                other_coefficient = RQScalar(sum / denominator);
                for (k = 0; k < count; k++) {
                    matrix[j * size + columns[k]] = RQSubtractProduct(coefficient, &values[k], &matrix[j * size + columns[k]]);
                    orthogonal[rows[k] + j] = RQSubtractProduct(other_coefficient, &values[k], &orthogonal[rows[k] + j]);
                }
            }
            matrix[i * size + i] = RQScalar(-(length * maximum));
        }
    }
    if (rhs != NULL && result != NULL) {
        intermediate[0] = RQDivideScalars(&rhs[0], &matrix[0]);
        for (i = 1; i < size; i++) {
            sum = 0.0;
            for (j = 0; j < i; j++) {
                sum = RQAccumulate(sum, &matrix[i * size + j], &intermediate[j]);
            }
            intermediate[i] = RQScalar(((double)rhs[i] - sum) / matrix[i * size + i]);
        }
        for (i = 0; i < size; i++) {
            result[i] = RQMultiplyScalars(&orthogonal[i], &intermediate[0]);
            for (j = 1; j < size; j++) {
                result[i] = RQScalar(RQAccumulate(result[i], &orthogonal[j * size + i], &intermediate[j]));
            }
        }
    }
}

// MakeMatrixFromIndices

// HasBeenDoneBefore

// RemoveRQSingularities

// AddToMatrix

// RQRemovedOneTooManyForces

// GetInverseM

// RQMatrixSolveNoNegatives

// RQMatrixSolveWithNegatives

// FUNCTION: CARMA2_HW 0x004c5b50
int C2_HOOK_FASTCALL MatrixProcess(br_scalar* matrix, br_scalar* orthogonal, int size) {
    return RQDcmpV(matrix, size, orthogonal);
}

// FUNCTION: CARMA2_HW 0x004c5b60
void C2_HOOK_FASTCALL RqBackSub(br_scalar* matrix, br_scalar* orthogonal, const br_scalar* rhs, br_scalar* result, int size) {
    volatile br_scalar intermediate[100];
    int i, j;
    double sum;
    intermediate[0] = RQDivideScalars(&rhs[0], &matrix[0]);
    for (i = 1; i < size; i++) {
        sum = 0.0;
        for (j = 0; j < i; j++) {
            sum = RQAccumulate(sum, &intermediate[j], &matrix[i * size + j]);
        }
        intermediate[i] = RQScalar(((double)rhs[i] - sum) / matrix[i * size + i]);
    }
    for (i = 0; i < size; i++) {
        result[i] = RQMultiplyScalars(&orthogonal[i], &intermediate[0]);
        for (j = 1; j < size; j++) {
            result[i] = RQScalar(RQAccumulate(result[i], &intermediate[j], &orthogonal[j * size + i]));
        }
    }
}

// FUNCTION: CARMA2_HW 0x004c5c70
void* C2_HOOK_FASTCALL GetPhysicsScratchPointer(int pSize) {
    tU32 end = (tU32)(uintptr_t)gPhysics_other_buffer + (tU32)gPhysics_other_buffer_capacity;

    if ((tU32)(uintptr_t)gPhysics_scratch_ptr + (tU32)pSize > end) {
        if (gPhysics_error_cb != NULL) {
            gPhysics_error_cb((tPhysicsError)6, NULL);
        }
        exit(6);
    }
    return gPhysics_scratch_ptr;
}

// FUNCTION: CARMA2_HW 0x004c5cb0
void C2_HOOK_FASTCALL IncreasePhysicsScratchPointer(int pSize) {
    tU32 end = (tU32)(uintptr_t)gPhysics_other_buffer + (tU32)gPhysics_other_buffer_capacity;

    if ((tU32)(uintptr_t)gPhysics_scratch_ptr + (tU32)pSize > end) {
        if (gPhysics_error_cb != NULL) {
            gPhysics_error_cb((tPhysicsError)6, NULL);
        }
        exit(6);
    }
    gPhysics_scratch_ptr = (void*)(uintptr_t)((tU32)(uintptr_t)gPhysics_scratch_ptr + (tU32)pSize);
}

// FUNCTION: CARMA2_HW 0x004c5d00
void C2_HOOK_FASTCALL ClearPhysicsScratchSpace(void) {
    gPhysics_scratch_ptr = gPhysics_other_buffer;
}

// FUNCTION: CARMA2_HW 0x004c5d10
tCollision_shape_sphere* C2_HOOK_FASTCALL AllocateShapeSphere(int pType) {
    tCollision_shape_sphere* result;

    result = BrMemAllocate(sizeof(tCollision_shape_sphere), (br_uint_8)pType);
    result->common.type = kCollisionShapeType_Sphere;
    return result;
}

// FUNCTION: CARMA2_HW 0x004c5d70
tCollision_shape_polyhedron* C2_HOOK_FASTCALL AllocateShapePolyhedron(int pCount_points, int pType) {
    tCollision_shape_polyhedron* result;
    tU8* raw_memory;
    int count_edges;

    count_edges = (pCount_points - 2) * 3;
    result = BrMemAllocate(sizeof(tCollision_shape_polyhedron) + pCount_points * sizeof(br_vector3)
        + count_edges * sizeof(tPolyhedron_edge_indexes) + (pCount_points - 2) * 2 * sizeof(br_vector4), (br_uint_8)pType);
    raw_memory = (tU8*)result + sizeof(tCollision_shape_polyhedron);
    result->polyhedron.points = (br_vector3*)raw_memory;
    raw_memory += pCount_points * sizeof(br_vector3);
    result->polyhedron.edges = (tPolyhedron_edge_indexes*)raw_memory;
    raw_memory += count_edges * sizeof(tPolyhedron_edge_indexes);
    result->polyhedron.planes = (br_vector4*)raw_memory;
    result->polyhedron.count_points = 0;
    result->polyhedron.count_edges = 0;
    result->polyhedron.count_planes = 0;
    result->common.type = kCollisionShapeType_Polyhedron;
    return result;
}

// FUNCTION: CARMA2_HW 0x004c5dc0
tCollision_shape_wireframe* C2_HOOK_FASTCALL AllocateShapeWireFrame(int pCount_points, int pCount_lines, int pType) {
    tCollision_shape_wireframe* result;
    tU8* raw_memory;

    result = BrMemAllocate(sizeof(tCollision_shape_wireframe) + pCount_points * sizeof(br_vector3)
        + pCount_lines * sizeof(tPolyhedron_edge_indexes), (br_uint_8)pType);
    raw_memory = (tU8*)result + sizeof(tCollision_shape_wireframe);
    result->wireframe.points = (br_vector3*)raw_memory;
    raw_memory += pCount_points * sizeof(br_vector3);
    result->wireframe.count_lines = pCount_lines;
    result->wireframe.lines = (tPolyhedron_edge_indexes*)raw_memory;
    result->wireframe.count_points = pCount_points;
    result->common.type = kCollisionShapeType_Wireframe;
    return result;
}

// FUNCTION: CARMA2_HW 0x004c5e00
tCollision_shape_box* C2_HOOK_FASTCALL AllocateShapeBox(int pType) {
    tCollision_shape_box* result;

    result = BrMemAllocate(sizeof(tCollision_shape_box), (br_uint_8)pType);
    result->common.type = kCollisionShapeType_Box;
    return result;
}

// GetJointSize

// FUNCTION: CARMA2_HW 0x004c5e20
tPhysics_joint* C2_HOOK_FASTCALL AllocatePhysicsJoint(int pCount_limits, int pType) {
    return BrMemAllocate(offsetof(tPhysics_joint, limits) + pCount_limits * sizeof(tPhysics_joint_limit), (br_uint_8)pType);
}

// FUNCTION: CARMA2_HW 0x004c5e40
tPhysics_joint* C2_HOOK_FASTCALL ClonePhysicsJoint(const tPhysics_joint* pJoint, int pType) {
    size_t size;
    tPhysics_joint* clone;

    size = offsetof(tPhysics_joint, limits) + pJoint->count_limits * sizeof(tPhysics_joint_limit);
    clone = BrMemAllocate(size, (br_uint_8)pType);
    memcpy(clone, pJoint, size);
    return clone;
}

// FreeShape

// FUNCTION: CARMA2_HW 0x004c5e80
void C2_HOOK_FASTCALL FreeShapeList(tPhysics_shape *pShape) {

    if (pShape->common.next != NULL) {
        FreeShapeList(pShape->common.next);
    }
    BrMemFree(pShape);
}

// FUNCTION: CARMA2_HW 0x004c5ea0
void C2_HOOK_FASTCALL FreePhysicsJoint(tPhysics_joint* pJoint) {

    BrMemFree(pJoint);
}

// GetBoundsFromPointList

// FillInShape

// UpdateCollisionObject

// FUNCTION: CARMA2_HW 0x004c61c0
void C2_NORETURN C2_HOOK_FASTCALL PhysicsError(tPhysicsError pError, const char* pMessage) {
    if (gPhysics_error_cb != NULL) {
        gPhysics_error_cb(pError, pMessage);
    }
    exit(pError);
}

// PhysicsWarning

// FUNCTION: CARMA2_HW 0x004c61f0
void C2_HOOK_FASTCALL PhysicsSetErrorHandler(tPhysicsError_cbfn* pError_cbfn) {
    gPhysics_error_cb = pError_cbfn;
}

// RepositionAndOrientateChildren

// PhysicsAddObject

// PhysicsAddObjectList

// PhysicsRemoveObject

// FUNCTION: CARMA2_HW 0x004c6470
int C2_HOOK_FASTCALL PhysicsObjectRecurse(tPhysics_object* pObject, tEnumCollision_cbfn* pCallback, void* pContext) {
    int result;
    tPhysics_object* child;

    result = pCallback(pObject, pContext);
    if (result != 0) {
        return result;
    }
    for (child = pObject->child; child != NULL; child = child->next) {
        result = PhysicsObjectRecurse(child, pCallback, pContext);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004c64b0
int C2_HOOK_FASTCALL PhysicsObjectRecurseChildren(tPhysics_object* pObject, tEnumCollision_cbfn* pCallback, void* pContext) {
    tPhysics_object* child;

    for (child = pObject->child; child != NULL; child = child->next) {
        int result;

        result = PhysicsObjectRecurse(child, pCallback, pContext);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004c6510
void C2_HOOK_FASTCALL InitPhysicsWorkspace(tU8* pBuffer, int pSize) {
    gPhysics_buffer1_ptr = pBuffer;
    pSize -= 3 * PHYSICS_BUFFER_PART_SIZE;
    gPhysics_buffer2_ptr = pBuffer + PHYSICS_BUFFER_PART_SIZE;
    gPhysics_buffer3_ptr = pBuffer + 2 * PHYSICS_BUFFER_PART_SIZE;
    pBuffer += 3 * PHYSICS_BUFFER_PART_SIZE;
    if (pSize < PHYSICS_BUFFER_OTHER_SIZE) {
        PhysicsError(ePhysicsError_InsufficientSizedBuffer, "Physics workspace too small");
    }
    gPhysics_other_buffer_capacity = pSize;
    gPhysics_other_buffer = pBuffer;
}

// FUNCTION: CARMA2_HW 0x004b57d0
int C2_HOOK_FASTCALL FindFacesInBox(tBounds* bnds, tFace_ref* face_list, int max_face, tWorld_callbacks* pWorld_callbacks) {
    tU8 min_x, min_z, max_x, max_z;
    tU32 dimensions;
    int x, z;
    int emitted = 0;

    FillInBounds(bnds);
    XZToColumnXZ(&min_x, &min_z, bnds->real_bounds.min.v[0], bnds->real_bounds.min.v[2], &gProgram_state.track_spec);
    XZToColumnXZ(&max_x, &max_z, bnds->real_bounds.max.v[0], bnds->real_bounds.max.v[2], &gProgram_state.track_spec);
    if (min_x != 0) {
        min_x--;
    }
    if (min_z != 0) {
        min_z--;
    }
    memcpy(&dimensions, &gProgram_state.track_spec, sizeof(dimensions));
    if (max_x + 1 < (int)(dimensions & 0xff)) {
        max_x++;
    }
    if (max_z + 1 < (int)((dimensions >> 8) & 0xff)) {
        max_z++;
    }
    for (x = min_x; x <= max_x; x++) {
        for (z = min_z; z <= max_z; z++) {
            if (gProgram_state.track_spec.columns[z][x].actor_0x0 != NULL) {
                emitted = max_face - ActorBoxPick(bnds, gProgram_state.track_spec.columns[z][x].actor_0x0, NULL, gBlack_material, &face_list[emitted], max_face - emitted, NULL, pWorld_callbacks);
            }
        }
    }
    if (gAdditional_actors != NULL) {
        emitted = max_face - ActorBoxPick(bnds, gAdditional_actors, NULL, gBlack_material, &face_list[emitted], max_face - emitted, NULL, pWorld_callbacks);
    }
    return emitted;
}

// GetFrictionFromFace

// FUNCTION: CARMA2_HW 0x004b5990
void C2_HOOK_FASTCALL DoPhysicsError(tPhysicsError pError, const char* pMessage) {
    char s[256];

    (void)pMessage;
    switch (pError) {
    case ePhysicsError_UnknownShapeType:
        strcpy(s, "Unknown shape type");
        break;
    case ePhysicsError_WireFrameHasNoPoints:
        strcpy(s, "Wire frame shape has no points");
        break;
    case ePhysicsError_PolyhedronHasNoPoints:
        strcpy(s, "Polyhedron shape has no points");
        break;
    case ePhysicsError_UnknownHingeType:
        strcpy(s, "Unknown hinge type");
        /* The original overwrites this message with the code. */
        /* fall through */
    default:
        sprintf(s, "%d ", pError);
        break;
    }
    FatalError(1000 + pError, s);
}

// ResetMan2

// SetUpTestObjects

// ResetObjectList

// ResetMan

#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth(0)
#endif
// FUNCTION: CARMA2_HW 0x004b5ca0
void C2_HOOK_FASTCALL InitPhysics(void) {
    PhysicsSetErrorHandler(DoPhysicsError);
    InitPhysicsWorkspace(gTemporary_physics_render_buffer, sizeof(gTemporary_physics_render_buffer));
}
#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth()
#endif

// PointOutOfSightNotAR

// CalcCollObjIDFromCarSpec

// CalcCollObjID

// GetCarSpecFromCollObjID

// GetCollObjFromID
