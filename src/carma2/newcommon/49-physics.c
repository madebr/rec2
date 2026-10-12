#include "49-physics.h"

#include "00-car.h"
#include "16-graphics1.h"
#include "28-world3.h"
#include "32-spark.h"
#include "34-finteray.h"
#include "37-brucetrk.h"
#include "52-errors.h"
#include "60-phil.h"
#include "globvars.h"
#include "globvrpb.h"
#include "c2_string.h"
#include "c2_stdlib.h"
#include "c2_math.h"

#include <stddef.h>

#if defined(_MSC_VER) && _MSC_VER >= 1300 && defined(_M_IX86)
void* _AddressOfReturnAddress(void);
#pragma intrinsic(_AddressOfReturnAddress)
#endif

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

// GLOBAL: CARMA2_HW 0x00679430
tU32 gPhysics_contact_679430;

// GLOBAL: CARMA2_HW 0x00679434
tU32 gPhysics_contact_679434;

// GLOBAL: CARMA2_HW 0x00679438
tU32 gPhysics_contact_679438;

/* Original writable box-edge table. */
// GLOBAL: CARMA2_HW 0x0058f7f0
tPolyhedron_edge_indexes gPhysics_box_edges_58f7f0[12]={{0,1},{2,3},{4,5},{6,7},{0,2},{1,3},{4,6},{5,7},{0,4},{1,5},{2,6},{3,7}};

// GLOBAL: CARMA2_HW 0x00676920
char gPhysics_empty_message;

// GLOBAL: CARMA2_HW 0x006940c0
tPhysicsError_cbfn* gPhysics_error_cb;

// GLOBAL: CARMA2_HW 0x006940b8
int gPhysics_rq_mode_6940b8;

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

/* Original0x423ed0: ECX object, EDX unused shape, RET20. Third stack word
 * is remaining capacity, not existing contact count; Ghidra mislabels it. */
// FUNCTION: CARMA2_HW 0x00423ed0
int C2_HOOK_FASTCALL GetCornerToFacePoints(tPhysics_object* pObject, tPhysics_shape* pUnused_shape, tPhysics_contact* pContacts, int pCount, int pCapacity, tPhysics_wall_workspace* pWorkspace, void* pUnused_argument) {
    br_matrix34* matrix=&pObject->transform_matrix;
    br_vector3 direction;
    br_vector3 extension;
    br_vector3 normal;
    br_vector3 transformed_normal;
    br_vector3 intersection;
    br_vector3 relative;
    br_vector3 local_position;
    br_scalar time;
    br_scalar penetration;
    tU32 penetration_word;
    double x;
    double y;
    double z;
    double length;
    double scale;
    double unit_x;
    tFace_ref* face;
    tU32* flags=pWorkspace->vertex_flags;
    br_vector3* first=pWorkspace->original_points;
    br_vector3* second=pWorkspace->trial_points;
    int i;

    (void)pUnused_shape;
    (void)pUnused_argument;
    if(pCapacity<4) return 0;
    gPhysics_contact_679434=0;
    for(i=0;i<(int)*(volatile tU16*)&pWorkspace->count_points;i++,flags++,first++,second++) {
        if(*flags!=0) continue;
        x=(double)second->v[0]-first->v[0];
        y=(double)second->v[1]-first->v[1];
        z=(double)second->v[2]-first->v[2];
        direction.v[1]=PhysicsScalar(y);
        direction.v[2]=PhysicsScalar(z);
        length=sqrt((x*x+y*direction.v[1])+z*direction.v[2]);
        if(PhysicsWallNaN(length)||length<=2.3841858e-7f) {
            unit_x=1.0;
            extension.v[1]=0.0f;
            extension.v[2]=0.0f;
        } else {
            scale=1.0/length;
            unit_x=scale*x;
            extension.v[1]=PhysicsScalar(scale*direction.v[1]);
            extension.v[2]=PhysicsScalar(scale*direction.v[2]);
        }
        extension.v[0]=PhysicsScalar(unit_x*0.0010434782598167658f);
        extension.v[1]=PhysicsScalar((double)extension.v[1]*0.0010434782598167658f);
        extension.v[2]=PhysicsScalar((double)extension.v[2]*0.0010434782598167658f);
        direction.v[0]=PhysicsScalar((double)extension.v[0]+x);
        direction.v[1]=PhysicsScalar((double)direction.v[1]+extension.v[1]);
        direction.v[2]=PhysicsScalar((double)direction.v[2]+extension.v[2]);
        face=RayCastThruFaceList(first,&direction,&normal,&time,&intersection,&gFace_list__car[pObject->box_face_start],(int)pObject->box_face_end-(int)pObject->box_face_start);
        if(face==NULL) continue;
        BrMatrix34TApplyV(&transformed_normal,&normal,matrix);
        penetration=PhysicsScalar(((double)normal.v[0]*direction.v[0]+(double)normal.v[1]*direction.v[1])+(double)normal.v[2]*direction.v[2]);
        PhysicsSubtractScalar(&relative.v[0],&first->v[0],&matrix->m[3][0]);
        PhysicsSubtractScalar(&relative.v[1],&first->v[1],&matrix->m[3][1]);
        PhysicsSubtractScalar(&relative.v[2],&first->v[2],&matrix->m[3][2]);
        BrMatrix34TApplyV(&local_position,&relative,matrix);
        PhysicsCopyWord(&penetration_word,&penetration);
        pCount=AddObjectWallForce(pContacts,&local_position,time,&transformed_normal,(int)penetration_word,pCount,pObject,1,face,NULL);
    }
    return pCount>=4?4:pCount;
}

/* Preserve the wide center-to-hit vector and its normalization. */
static double C2_HOOK_CDECL WallEdgeAlignment(const br_vector3* centre, const br_vector3* hit, const br_vector3* normal) {
    double x=(double)centre->v[0]-hit->v[0];
    double y=(double)centre->v[1]-hit->v[1];
    double z=(double)centre->v[2]-hit->v[2];
    double length=sqrt((x*x+y*y)+z*z);
    double scale;
    if(PhysicsWallNaN(length)||length<=2.384185791015625e-7f) {
        x=1.0;y=0.0;z=0.0;
    } else {
        scale=1.0/length;x=scale*x;y=scale*y;z=scale*z;
    }
    return (x*normal->v[0]+y*normal->v[1])+z*normal->v[2];
}

// FUNCTION: CARMA2_HW 0x004236a0
int C2_HOOK_FASTCALL FUN_004236a0(tPhysics_object* object, tPhysics_shape* shape, tPhysics_contact* contacts, int count, int capacity, tPhysics_wall_workspace* workspace, void* unused, int* touched) {
    br_vector3 centre,direction,reverse,normal,hit,other_normal,other_hit;
    br_vector3 relative,local,previous,local_normal;
    br_vector3* first;
    br_vector3* second;
    tFace_ref* face;
    tFace_ref* other_face;
    br_scalar time,other_time,alignment,depth;
    tU32 depth_word;
    double other_alignment,wide_depth;
    unsigned first_index,second_index;
    int edge,selected;
    (void)capacity;(void)unused;
    if(shape->common.type==4) return count;
    BrMatrix34ApplyP(&centre,&object->cmpos,&object->transform_matrix);
    for(edge=0;edge<(int)*(volatile tU16*)&workspace->count_edges;edge++) {
        first_index=((const tU8*)workspace->edges)[edge*2];
        second_index=((const tU8*)workspace->edges)[edge*2+1];
        first=&workspace->original_points[first_index];
        second=&workspace->original_points[second_index];
        PhysicsSubtractScalar(&direction.v[0],&second->v[0],&first->v[0]);
        PhysicsSubtractScalar(&direction.v[1],&second->v[1],&first->v[1]);
        PhysicsSubtractScalar(&direction.v[2],&second->v[2],&first->v[2]);
        face=RayCastThruFaceList(first,&direction,&normal,&time,&hit,&gFace_list__car[object->box_face_start],(int)object->box_face_end-(int)object->box_face_start);
        PhysicsSubtractScalar(&reverse.v[0],&first->v[0],&second->v[0]);
        PhysicsSubtractScalar(&reverse.v[1],&first->v[1],&second->v[1]);
        PhysicsSubtractScalar(&reverse.v[2],&first->v[2],&second->v[2]);
        selected=0;
        if(face==NULL) {
            face=RayCastThruFaceList(second,&reverse,&normal,&time,&hit,&gFace_list__car[object->box_face_start],(int)object->box_face_end-(int)object->box_face_start);
            if(face!=NULL) {
                other_alignment=WallEdgeAlignment(&centre,&hit,&normal);
                if(!PhysicsWallNaN(other_alignment)&&other_alignment>=0.009999999776482582f) {
                    workspace->vertex_flags[first_index]=1;selected=1;
                }
            }
        } else {
            alignment=PhysicsScalar(WallEdgeAlignment(&centre,&hit,&normal));
            other_face=RayCastThruFaceList(second,&reverse,&other_normal,&other_time,&other_hit,&gFace_list__car[object->box_face_start],(int)object->box_face_end-(int)object->box_face_start);
            if(other_face==NULL) {
                if(!PhysicsWallNaN(alignment)&&alignment>0.009999999776482582f) {
                    workspace->vertex_flags[second_index]=1;selected=1;
                }
            } else {
                other_alignment=WallEdgeAlignment(&centre,&other_hit,&other_normal);
                if((!PhysicsWallNaN(alignment)&&alignment>=0.05000000074505806f)||(!PhysicsWallNaN(other_alignment)&&other_alignment>=0.05000000074505806f)) {
                    if(PhysicsWallNaN(other_alignment)||PhysicsWallNaN(alignment)||other_alignment<=alignment) {
                        workspace->vertex_flags[second_index]=1;
                    } else {
                        PhysicsCopyWord(&hit.v[0],&other_hit.v[0]);PhysicsCopyWord(&hit.v[1],&other_hit.v[1]);PhysicsCopyWord(&hit.v[2],&other_hit.v[2]);
                        PhysicsCopyWord(&normal.v[0],&other_normal.v[0]);PhysicsCopyWord(&normal.v[1],&other_normal.v[1]);PhysicsCopyWord(&normal.v[2],&other_normal.v[2]);
                        workspace->vertex_flags[first_index]=1;
                    }
                    selected=1;
                }
            }
        }
        if(selected) {
            PhysicsSubtractScalar(&relative.v[0],&hit.v[0],&object->transform_matrix.m[3][0]);
            PhysicsSubtractScalar(&relative.v[1],&hit.v[1],&object->transform_matrix.m[3][1]);
            PhysicsSubtractScalar(&relative.v[2],&hit.v[2],&object->transform_matrix.m[3][2]);
            BrMatrix34TApplyV(&local,&relative,&object->transform_matrix);
            BrMatrix34ApplyP(&previous,&local,&object->actor->t.t.mat);
            wide_depth=(((double)hit.v[0]-previous.v[0])*normal.v[0]+((double)hit.v[1]-previous.v[1])*normal.v[1])+((double)hit.v[2]-previous.v[2])*normal.v[2];
            depth=PhysicsScalar(wide_depth);
            if(!PhysicsWallNaN(wide_depth)&&wide_depth>0.0) {
                BrMatrix34TApplyV(&local_normal,&normal,&object->transform_matrix);
                PhysicsCopyWord(&depth_word,&depth);
                count=AddObjectWallForce(contacts,&local,0.0f,&local_normal,(int)depth_word,count,object,1,face,NULL);
            }
            *touched=1;
        }
    }
    return count;
}


/* Traverse actual wall-face edges; original lexical name unknown. */
// FUNCTION: CARMA2_HW 0x00424140
int C2_HOOK_FASTCALL FUN_00424140(tPhysics_object* object, tPhysics_shape* shape, tPhysics_contact* contacts, int count, int unused_capacity, tPhysics_wall_workspace* workspace, void* unused) {
    br_actor* actor;
    br_vector3 translation;
    tFace_ref* face;
    tU32 flags;
    int axis;
    int index;

    (void)unused_capacity;
    (void)unused;
    gPhysics_contact_679430=0;
    PhysicsCopyWord(&actor,&object->actor);
    BrMatrix34TApplyV(&translation,(const br_vector3*)&actor->t.t.mat.m[3],&actor->t.t.mat);
    for(axis=0;axis<3;axis++)shape->common.field_0x1c.min.v[axis]=PhysicsScalar(PhysicsWallAddScalar(&translation.v[axis],&shape->common.field_0x1c.min.v[axis]));
    for(axis=0;axis<3;axis++)shape->common.field_0x1c.max.v[axis]=PhysicsScalar(PhysicsWallAddScalar(&translation.v[axis],&shape->common.field_0x1c.max.v[axis]));
    index=object->box_face_start;
    face=&gFace_list__car[index];
    if(index>=(int)object->box_face_end)return count;
    do {
        flags=*(volatile tU32*)&face->flags;
        if((flags&0x80)==0) {
            if((flags&1)==0)count=FUN_00424270(&face->v[0],&face->v[1],object,shape,contacts,count,face,workspace);
            if((*(volatile tU8*)&face->flags&2)==0)count=FUN_00424270(&face->v[1],&face->v[2],object,shape,contacts,count,face,workspace);
            if((*(volatile tU8*)&face->flags&4)==0)count=FUN_00424270(&face->v[2],&face->v[0],object,shape,contacts,count,face,workspace);
        }
        index++;face++;
    }while(index<(int)*(volatile tU16*)&object->box_face_end);
    return count;
}

/* Complete moving wall-edge consumer; original lexical name unknown. */
// FUNCTION: CARMA2_HW 0x00424270
int C2_HOOK_FASTCALL FUN_00424270(br_vector3* start, br_vector3* end, tPhysics_object* object, tPhysics_shape* shape, tPhysics_contact* contacts, int count, tFace_ref* face, tPhysics_wall_workspace* workspace) {
    /* Preserve the original private-buffer overlaps and initialize only slots
     * explicitly initialized by the authentic body. Offset0x10 begins locals;
     * offsets0..0xf describe saved registers and are not read here. */
    volatile union { tU8 bytes[0x180];tU32 words[0x60];br_scalar values[0x60]; } frame;
    br_actor* actor;
    br_matrix34* matrix;
    br_vector3* relative20=(br_vector3*)(void*)&frame.bytes[0x20];
    br_vector3* change=(br_vector3*)(void*)&frame.bytes[0x2c];
    br_vector3* relative38=(br_vector3*)(void*)&frame.bytes[0x38];
    br_vector3* edge=(br_vector3*)(void*)&frame.bytes[0x44];
    br_vector3* velocity=(br_vector3*)(void*)&frame.bytes[0x54];
    br_vector3* buffer60=(br_vector3*)(void*)&frame.bytes[0x60];
    br_vector3* direction=(br_vector3*)(void*)&frame.bytes[0x6c];
    br_vector3* buffer78=(br_vector3*)(void*)&frame.bytes[0x78];
    br_vector3* buffer84=(br_vector3*)(void*)&frame.bytes[0x84];
    br_vector3* buffer90=(br_vector3*)(void*)&frame.bytes[0x90];
    br_vector3* gate_end=(br_vector3*)(void*)&frame.bytes[0x9c];
    br_vector3* gate_start=(br_vector3*)(void*)&frame.bytes[0xa8];
    br_vector3* gate_point=(br_vector3*)(void*)&frame.bytes[0x174];
    volatile tPhysics_contact* candidates=(volatile tPhysics_contact*)(void*)&frame.bytes[0xb4];
    br_vector3* first;
    br_vector3* second;
    br_vector3* trial_first;
    br_vector3* trial_second;
    double projected_start,low,wide_change_z,normal_side;
    volatile br_scalar projected_end,high;
    int axis,component,result,selector;
    tU8 index1,index2;
    tU32 current;
    frame.words[0x18/4]=1;frame.words[0x1c/4]=1;
    PhysicsCopyWord(&actor,&object->actor);
    matrix=&actor->t.t.mat;
    for(axis=0;axis<3;axis++) {
        projected_start=((double)matrix->m[axis][0]*start->v[0]+(double)matrix->m[axis][2]*start->v[2])+(double)matrix->m[axis][1]*start->v[1];
        projected_end=PhysicsScalar(((double)matrix->m[axis][0]*end->v[0]+(double)matrix->m[axis][2]*end->v[2])+(double)matrix->m[axis][1]*end->v[1]);
        low=(double)shape->common.field_0x1c.min.v[axis]-.005;
        if((PhysicsWallNaN(projected_start)||PhysicsWallNaN(low)||projected_start<low)&&(PhysicsWallNaN(projected_end)||PhysicsWallNaN(low)||projected_end<low))return count;
        frame.words[0x18/4]|=(tU32)(PhysicsWallNaN(projected_start)||PhysicsWallNaN(low)||projected_start<low);
        frame.words[0x1c/4]|=(tU32)(PhysicsWallNaN(projected_end)||PhysicsWallNaN(low)||projected_end<low);
        high=PhysicsScalar((double)shape->common.field_0x1c.max.v[axis]-(-.005));
        frame.words[0x18/4]|=(tU32)(PhysicsWallNaN(projected_start)||PhysicsWallNaN(high)||projected_start<high);
        frame.words[0x1c/4]|=(tU32)(PhysicsWallNaN(projected_end)||PhysicsWallNaN(high)||projected_end<high);
        if(!PhysicsWallNaN(projected_start)&&!PhysicsWallNaN(high)&&projected_start>high&&!PhysicsWallNaN(projected_end)&&projected_end>high)return count;
    }
    if(object->field_0xed==0) {
        if(frame.words[0x18/4]!=0) {
            for(component=0;component<3;component++)PhysicsSubtractScalar(&relative38->v[component],&start->v[component],&object->transform_matrix.m[3][component]);
            BrMatrix34TApplyV(buffer84,relative38,&object->transform_matrix);
            PhysicsCopyWord(&actor,&object->actor);
            for(component=0;component<3;component++)PhysicsSubtractScalar(&relative20->v[component],&start->v[component],&actor->t.t.mat.m[3][component]);
            BrMatrix34TApplyV(buffer90,relative20,&actor->t.t.mat);
            result=ShapeRayCastWithSphere(buffer84,buffer90,shape,buffer60,buffer78,(br_scalar*)(void*)&frame.values[0x10/4]);
            if(result>0) {
                if(result>64)frame.values[0x10/4]=1.1f;
                count=AddObjectWallForce(contacts,buffer60,frame.values[0x10/4],buffer78,0,count,object,2,face,NULL);
            }
        }
        if(frame.words[0x1c/4]!=0) {
            for(component=0;component<3;component++)PhysicsSubtractScalar(&relative20->v[component],&end->v[component],&object->transform_matrix.m[3][component]);
            BrMatrix34TApplyV(buffer78,relative20,&object->transform_matrix);
            PhysicsCopyWord(&actor,&object->actor);
            for(component=0;component<3;component++)PhysicsSubtractScalar(&relative38->v[component],&end->v[component],&actor->t.t.mat.m[3][component]);
            BrMatrix34TApplyV(buffer60,relative38,&actor->t.t.mat);
            result=ShapeRayCastWithSphere(buffer78,buffer60,shape,buffer90,buffer84,(br_scalar*)(void*)&frame.values[0x10/4]);
            if(result>0) {
                if(result>64)frame.values[0x10/4]=1.1f;
                count=AddObjectWallForce(contacts,buffer90,frame.values[0x10/4],buffer84,0,count,object,2,face,NULL);
            }
        }
    }
    for(component=0;component<3;component++)PhysicsSubtractScalar(&direction->v[component],&end->v[component],&start->v[component]);
    selector=0;candidates[0].face=face;candidates[1].face=face;
    candidates[0].t=2.f;candidates[1].t=2.f;
    frame.words[0x14/4]=0xffffffffu;frame.words[0x10/4]=0;
    if(*(volatile tU16*)&workspace->count_edges!=0) {
        do {
            current=frame.words[0x10/4];
            index1=workspace->edges[current].index1;index2=workspace->edges[current].index2;
            first=&workspace->original_points[index1];second=&workspace->original_points[index2];
            trial_first=&workspace->trial_points[index1];trial_second=&workspace->trial_points[index2];
            for(component=0;component<3;component++)PhysicsSubtractScalar(&velocity->v[component],&trial_first->v[component],&first->v[component]);
            for(component=0;component<3;component++)PhysicsSubtractScalar(&edge->v[component],&second->v[component],&first->v[component]);
            PhysicsSubtractScalar(&change->v[0],&trial_second->v[0],&trial_first->v[0]);
            PhysicsSubtractScalar(&change->v[1],&trial_second->v[1],&trial_first->v[1]);
            wide_change_z=(double)trial_second->v[2]-trial_first->v[2];
            PhysicsSubtractScalar(&change->v[0],&change->v[0],&edge->v[0]);
            PhysicsSubtractScalar(&change->v[1],&change->v[1],&edge->v[1]);
            change->v[2]=PhysicsScalar(wide_change_z-edge->v[2]);
            result=FUN_00424990(start,direction,first,edge,velocity,change,(tPhysics_contact*)(void*)&candidates[selector],count,object,NULL);
            if(result!=0) {
                frame.words[0x14/4]=current;
                if(PhysicsWallNaN(candidates[selector].t)||PhysicsWallNaN(candidates[!selector].t)||candidates[selector].t<candidates[!selector].t)selector=!selector;
            }
            current=frame.words[0x10/4]+1;frame.words[0x10/4]=current;
        } while((int)current<(int)*(volatile tU16*)&workspace->count_edges);
    }
    if(!PhysicsWallNaN(candidates[0].t)&&!(candidates[0].t<2.f))return count;
    normal_side=((double)face->normal.v[2]*(double)*(volatile br_scalar*)(void*)(candidates[0].field_0x38+8)+(double)face->normal.v[1]*(double)*(volatile br_scalar*)(void*)(candidates[0].field_0x38+4))+(double)face->normal.v[0]*(double)*(volatile br_scalar*)(void*)candidates[0].field_0x38;
    if(PhysicsWallNaN(normal_side)||normal_side<0.0) {
        matrix=&object->transform_matrix;
        for(component=0;component<3;component++)PhysicsSubtractScalar(&relative20->v[component],&start->v[component],&matrix->m[3][component]);
        BrMatrix34TApplyV(gate_start,relative20,matrix);
        for(component=0;component<3;component++)PhysicsSubtractScalar(&relative20->v[component],&end->v[component],&matrix->m[3][component]);
        BrMatrix34TApplyV(gate_end,relative20,matrix);
        result=ShapeRayCast(gate_start,gate_end,shape,gate_point,(br_scalar*)(void*)&frame.values[0x18/4],NULL);
        if(result!=0)return count;
    }
    gPhysics_contact_679430|=(tU32)1<<(frame.words[0x14/4]&31);
    count=AddObjectWallForce(contacts,(const br_vector3*)(const void*)&candidates[0].position,candidates[0].t,(const br_vector3*)(const void*)&candidates[0].normal,0,count,object,0,face,(const br_vector3*)(const void*)&candidates[0].reference_position);
    if(PhysicsWallNaN(candidates[1].t)||candidates[1].t<2.f)count=AddObjectWallForce(contacts,(const br_vector3*)(const void*)&candidates[1].position,candidates[1].t,(const br_vector3*)(const void*)&candidates[1].normal,0,count,object,0,face,(const br_vector3*)(const void*)&candidates[1].reference_position);
    return count;
}

/* Complete quadratic moving-edge contact; original lexical name unknown. */
// FUNCTION: CARMA2_HW 0x00424990
int C2_HOOK_FASTCALL FUN_00424990(const br_vector3* old_origin, const br_vector3* old_direction, const br_vector3* moving_origin, const br_vector3* moving_direction, const br_vector3* velocity, const br_vector3* direction_change, tPhysics_contact* output, int unused, tPhysics_object* object, tPhysics_object* other) {
    br_vector3 n0,n1,delta,normal,edge,origin,local,world,relative,previous_normal;
    double a,c,ac4,b2,root,wide_time,normal_square,normal_length,scale;
    volatile br_scalar b;
    double dot,numerator,denominator,orientation,mixed[3],wide_delta[3],wide_edge_x;
    volatile br_scalar negative_square,projection[3],stored_dot,stored_z_product;
    volatile br_scalar stored_current[3],stored_blend[3];
    volatile br_scalar time,stored_two_a,stored_root,stored_ac4,stored_b2,edge_fraction,old_fraction;
    volatile br_scalar complement;
    unsigned axis,i;
    n0.v[0]=PhysicsScalar((double)moving_direction->v[1]*old_direction->v[2]-(double)old_direction->v[1]*moving_direction->v[2]);
    n0.v[1]=PhysicsScalar((double)moving_direction->v[2]*old_direction->v[0]-(double)old_direction->v[2]*moving_direction->v[0]);
    n0.v[2]=PhysicsScalar((double)old_direction->v[1]*moving_direction->v[0]-(double)moving_direction->v[1]*old_direction->v[0]);
    n1.v[0]=PhysicsScalar((double)direction_change->v[1]*old_direction->v[2]-(double)direction_change->v[2]*old_direction->v[1]);
    n1.v[1]=PhysicsScalar((double)direction_change->v[2]*old_direction->v[0]-(double)old_direction->v[2]*direction_change->v[0]);
    n1.v[2]=PhysicsScalar((double)old_direction->v[1]*direction_change->v[0]-(double)direction_change->v[1]*old_direction->v[0]);
    for(i=0;i<3;i++)PhysicsSubtractScalar(&delta.v[i],&moving_origin->v[i],&old_origin->v[i]);
    a=((double)velocity->v[1]*n1.v[1]+(double)velocity->v[2]*n1.v[2])+(double)n1.v[0]*velocity->v[0];
    b=PhysicsScalar(((((double)moving_origin->v[2]*n1.v[2]+(double)moving_origin->v[1]*n1.v[1])+(double)n1.v[0]*moving_origin->v[0])+
        (((double)velocity->v[1]*n0.v[1]+(double)velocity->v[2]*n0.v[2])+(double)n0.v[0]*velocity->v[0]))-
        ((((double)old_origin->v[2]*n1.v[2]+(double)old_origin->v[1]*n1.v[1])+(double)n1.v[0]*old_origin->v[0])+
        (((double)old_direction->v[2]*n0.v[2]+(double)old_direction->v[1]*n0.v[1])+(double)n0.v[0]*old_direction->v[0])));
    c=((double)delta.v[0]*n0.v[0]+(double)delta.v[1]*n0.v[1])+(double)delta.v[2]*n0.v[2];
    if(PhysicsWallNaN(a)||a<0.0) {a=-a;b=-b;c=-c;}
    ac4=(c*a)*4.f;
    stored_ac4=PhysicsScalar(ac4);
    b2=b*b;
    stored_b2=PhysicsScalar(b2);
    if((double)stored_b2<fabs(ac4)*1000.f) {
        root=(double)stored_b2-stored_ac4;
        if(PhysicsWallNaN(root)||root<0.0)goto fallback;
        stored_root=PhysicsScalar(sqrt(root));
        stored_two_a=PhysicsScalar(a+a);
        wide_time=(-b-stored_root)/stored_two_a;
        time=PhysicsScalar(wide_time);
        if(wide_time>1.0)goto fallback;
        if(!PhysicsWallNaN(time)&&time>=0.f) {
            for(i=0;i<3;i++) {mixed[i]=(double)time*n1.v[i]+n0.v[i];normal.v[i]=PhysicsScalar(mixed[i]);}
            normal_square=(mixed[0]*normal.v[0]+mixed[1]*normal.v[1])+mixed[2]*normal.v[2];
            if(PhysicsWallNaN(normal_square)||normal_square<1e-7)goto fallback;
        }
        if(PhysicsWallNaN(time)||time<0.f)time=PhysicsScalar(((double)stored_root-b)/stored_two_a);
    } else {
        if(!(fabs(b)>1e-7))goto fallback;
        wide_time=-(c/b)-(((c*c*a)/((double)stored_b2*b))*(double)-1.3333333730697632f);
        time=PhysicsScalar(wide_time);
        if(PhysicsWallNaN(wide_time)||wide_time<0.0||time>1.f||gPhysics_contact_679438!=0)time=PhysicsScalar(-(b/a)-time);
    }
    if(!(time>=0.f)||!(time<=1.f))goto fallback;
    for(i=0;i<3;i++) {mixed[i]=(double)time*n1.v[i]+n0.v[i];normal.v[i]=PhysicsScalar(mixed[i]);}
    normal_square=(mixed[0]*normal.v[0]+mixed[1]*normal.v[1])+mixed[2]*normal.v[2];
    if(PhysicsWallNaN(normal_square)||normal_square<1e-7)goto fallback;
    if(!PhysicsWallNaN(output->t)&&!(time<output->t))return 0;
    normal_length=sqrt(((double)normal.v[0]*normal.v[0]+(double)normal.v[1]*normal.v[1])+(double)normal.v[2]*normal.v[2]);
    if(PhysicsWallNaN(normal_length)||normal_length<=2.384185791015625e-7f) {
        normal.v[0]=1.f;normal.v[1]=0.f;normal.v[2]=0.f;
    } else {
        scale=1.0/normal_length;
        for(i=0;i<3;i++)normal.v[i]=PhysicsScalar(scale*normal.v[i]);
    }
    wide_edge_x=(double)time*direction_change->v[0]+moving_direction->v[0];
    edge.v[0]=PhysicsScalar(wide_edge_x);
    for(i=1;i<3;i++)edge.v[i]=PhysicsScalar((double)direction_change->v[i]*time+moving_direction->v[i]);
    for(i=0;i<3;i++)origin.v[i]=PhysicsScalar((double)time*velocity->v[i]+moving_origin->v[i]);
    dot=((double)old_direction->v[1]*edge.v[1]+(double)old_direction->v[2]*edge.v[2])+wide_edge_x*old_direction->v[0];
    stored_dot=PhysicsScalar(dot);
    negative_square=PhysicsScalar(-(((double)old_direction->v[1]*old_direction->v[1]+(double)old_direction->v[2]*old_direction->v[2])+(double)old_direction->v[0]*old_direction->v[0]));
    projection[0]=PhysicsScalar((double)edge.v[0]*negative_square+dot*old_direction->v[0]);
    projection[1]=PhysicsScalar((double)edge.v[1]*negative_square+(double)old_direction->v[1]*stored_dot);
    stored_z_product=PhysicsScalar((double)old_direction->v[2]*stored_dot);
    projection[2]=PhysicsScalar((double)edge.v[2]*negative_square+stored_z_product);
    for(i=0;i<3;i++) {wide_delta[i]=(double)old_origin->v[i]-origin.v[i];delta.v[i]=PhysicsScalar(wide_delta[i]);}
    numerator=(wide_delta[0]*projection[0]+wide_delta[1]*projection[1])+wide_delta[2]*projection[2];
    denominator=((double)edge.v[0]*projection[0]+(double)edge.v[1]*projection[1])+(double)edge.v[2]*projection[2];
    edge_fraction=PhysicsScalar(numerator/denominator);
    if(!(edge_fraction>=0.f)||!(edge_fraction<=1.f))goto fallback;
    axis=!(fabs((double)old_direction->v[0])>=fabs((double)old_direction->v[1]));
    if(!(fabs((double)old_direction->v[2])<=fabs((double)old_direction->v[axis])))axis=2;
    old_fraction=PhysicsScalar((((double)edge.v[axis]*edge_fraction+origin.v[axis])-old_origin->v[axis])/old_direction->v[axis]);
    if(!(old_fraction>=0.f)||!(old_fraction<=1.f))goto fallback;
    for(i=0;i<3;i++)PhysicsSubtractScalar(&delta.v[i],&moving_origin->v[i],&old_origin->v[i]);
    orientation=((double)delta.v[0]*n0.v[0]+(double)delta.v[1]*n0.v[1])+(double)delta.v[2]*n0.v[2];
    if(PhysicsWallNaN(orientation)||orientation<0.0)for(i=0;i<3;i++)normal.v[i]=-normal.v[i];
    if(other!=NULL) {
        BrMatrix34ApplyV(&output->normal,&normal,&other->transform_matrix);
        BrMatrix34ApplyV(&previous_normal,&normal,&other->actor->t.t.mat);
        complement=PhysicsScalar(1.f-time);
        for(i=0;i<3;i++)stored_current[i]=PhysicsScalar((double)output->normal.v[i]*time);
        output->normal.v[0]=stored_current[0];
        for(i=0;i<3;i++) {
            mixed[i]=(double)previous_normal.v[i]*complement+stored_current[i];
            stored_blend[i]=PhysicsScalar(mixed[i]);
        }
        normal_length=sqrt((mixed[0]*stored_blend[0]+mixed[1]*stored_blend[1])+mixed[2]*stored_blend[2]);
        output->normal.v[1]=stored_current[1];output->normal.v[1]=stored_blend[1];
        output->normal.v[2]=stored_current[2];output->normal.v[0]=stored_blend[0];output->normal.v[2]=stored_blend[2];
        if(PhysicsWallNaN(normal_length)||normal_length<=2.384185791015625e-7f) {
            output->normal.v[0]=1.f;output->normal.v[1]=0.f;output->normal.v[2]=0.f;
        } else {
            scale=1.0/normal_length;
            output->normal.v[0]=PhysicsScalar(scale*mixed[0]);
            output->normal.v[1]=PhysicsScalar(scale*stored_blend[1]);
            output->normal.v[2]=PhysicsScalar(scale*stored_blend[2]);
        }
        for(i=0;i<3;i++)wide_delta[i]=(double)old_fraction*old_direction->v[i];
        for(i=0;i<3;i++)output->reference_position.v[i]=PhysicsScalar(wide_delta[i]+old_origin->v[i]);
        for(i=0;i<3;i++)local.v[i]=PhysicsScalar((double)edge_fraction*moving_direction->v[i]+moving_origin->v[i]);
        BrMatrix34ApplyP(&world,&local,&other->transform_matrix);
        for(i=0;i<3;i++)PhysicsSubtractScalar(&relative.v[i],&world.v[i],&object->transform_matrix.m[3][i]);
        BrMatrix34TApplyV(&output->position,&relative,&object->transform_matrix);
        output->t=time;
    } else {
        BrMatrix34TApplyV(&output->normal,&normal,&object->transform_matrix);
        for(i=0;i<3;i++)PhysicsCopyWord(output->field_0x38+4*i,&normal.v[i]);
        for(i=0;i<3;i++) {
            mixed[i]=(double)edge_fraction*moving_direction->v[i]+moving_origin->v[i];
            relative.v[i]=PhysicsScalar(mixed[i]-object->transform_matrix.m[3][i]);
        }
        BrMatrix34TApplyV(&output->position,&relative,&object->transform_matrix);
        for(i=0;i<3;i++)output->reference_position.v[i]=PhysicsScalar((double)old_fraction*old_direction->v[i]);
        for(i=0;i<3;i++)output->reference_position.v[i]=PhysicsScalar((double)output->reference_position.v[i]+old_origin->v[i]);
        output->t=time;
    }
    return 1;
fallback:
    return FUN_00425630(&n0,&n1,old_origin,old_direction,moving_origin,moving_direction,velocity,direction_change,output,unused,object,other);
}

// FUNCTION: CARMA2_HW 0x00425630
int C2_HOOK_FASTCALL FUN_00425630(const br_vector3* n0, const br_vector3* n1, const br_vector3* old_origin, const br_vector3* old_direction, const br_vector3* moving_origin, const br_vector3* velocity0, const br_vector3* edge_change, const br_vector3* velocity1, tPhysics_contact* output, int unused, tPhysics_object* object, tPhysics_object* other) {
    br_vector3 normal,delta,speed,point,relative,world;
    double combined[3],unit[3],cross[3],wide_delta[3],wide_point[3];
    double length,projection,denominator,numerator,scaled_x,parameter,distance,orientation;
    br_scalar stored_length,time,segment,orientation_y,direction_y,direction_z;
    volatile union { br_scalar value; tU32 word; } scaled_y,scaled_z;
    tU32 rounded_word;
    int i;
    (void)unused;
    if(!((double)output->t>1.0)) return 0;
    combined[0]=(double)n0->v[0]+n1->v[0];
    combined[1]=(double)n1->v[1]+n0->v[1];
    combined[2]=(double)n1->v[2]+n0->v[2];
    for(i=0;i<3;i++)normal.v[i]=PhysicsScalar(combined[i]);
    length=sqrt((combined[0]*normal.v[0]+combined[1]*normal.v[1])+combined[2]*normal.v[2]);
    stored_length=PhysicsScalar(length);
    if(!(length>1e-7)) return 0;
    for(i=0;i<3;i++) {
        wide_delta[i]=((double)moving_origin->v[i]+edge_change->v[i])-old_origin->v[i];
        delta.v[i]=PhysicsScalar(wide_delta[i]);
    }
    projection=fabs(((wide_delta[0]*normal.v[0]+wide_delta[1]*normal.v[1])+wide_delta[2]*normal.v[2])/stored_length);
    if(!PhysicsWallNaN(projection)&&!(projection<.002)) return 0;
    for(i=0;i<3;i++)unit[i]=(double)normal.v[i]/stored_length;
    for(i=0;i<3;i++)normal.v[i]=PhysicsScalar(unit[i]);
    speed.v[0]=PhysicsScalar((double)velocity1->v[0]+velocity0->v[0]);
    speed.v[1]=PhysicsScalar((double)velocity1->v[1]+velocity0->v[1]);
    speed.v[2]=PhysicsScalar((double)velocity1->v[2]+velocity0->v[2]);
    cross[0]=(double)old_direction->v[1]*normal.v[2]-(double)old_direction->v[2]*normal.v[1];
    cross[1]=(double)old_direction->v[2]*normal.v[0]-unit[2]*old_direction->v[0];
    cross[2]=unit[1]*old_direction->v[0]-(double)old_direction->v[1]*normal.v[0];
    denominator=(cross[0]*((double)velocity1->v[0]+velocity0->v[0])+cross[1]*speed.v[1])+cross[2]*speed.v[2];
    if(!(fabs(denominator)>1e-7)) return 0;
    numerator=(cross[0]*delta.v[0]+cross[1]*delta.v[1])+cross[2]*delta.v[2];
    time=PhysicsScalar(-(numerator/denominator));
    if(!(time>=0.f)||!(time<=1.f)) return 0;
    scaled_x=(double)time*speed.v[0]+delta.v[0];
    scaled_y.value=PhysicsScalar((double)time*speed.v[1]+delta.v[1]);
    scaled_z.value=PhysicsScalar((double)time*speed.v[2]+delta.v[2]);
    rounded_word=scaled_y.word;PhysicsCopyWord(&delta.v[1],&rounded_word);
    rounded_word=scaled_z.word;PhysicsCopyWord(&delta.v[2],&rounded_word);
    for(i=0;i<3;i++)normal.v[i]=PhysicsScalar(unit[i]);
    parameter=(((double)old_direction->v[1]*delta.v[1]+(double)old_direction->v[2]*delta.v[2])+scaled_x*old_direction->v[0])/
        (((double)old_direction->v[1]*old_direction->v[1]+(double)old_direction->v[2]*old_direction->v[2])+(double)old_direction->v[0]*old_direction->v[0]);
    segment=PhysicsScalar(parameter);
    if(!(segment>=0.f)||!(segment<=1.f)) return 0;
    output->t=1.f;
    scaled_y.value=PhysicsScalar((double)moving_origin->v[1]-old_origin->v[1]);
    rounded_word=scaled_y.word;PhysicsCopyWord(&orientation_y,&rounded_word);
    orientation=((double)orientation_y*n0->v[1]+((double)moving_origin->v[2]-old_origin->v[2])*n0->v[2])+((double)moving_origin->v[0]-old_origin->v[0])*n0->v[0];
    if(PhysicsWallNaN(orientation)||orientation<0.0) {
        for(i=0;i<3;i++)normal.v[i]=-normal.v[i];
    }
    for(i=0;i<3;i++)wide_delta[i]=((double)moving_origin->v[i]+edge_change->v[i])-old_origin->v[i];
    distance=(wide_delta[0]*normal.v[0]+wide_delta[1]*normal.v[1])+wide_delta[2]*normal.v[2];
    if(PhysicsWallNaN(distance)||distance<0.0) {
        for(i=0;i<3;i++)normal.v[i]=-normal.v[i];
    }
    if(other==NULL&&(PhysicsWallNaN(distance)||fabs(distance)<.001))output->t=1.1f;
    for(i=0;i<3;i++) {
        wide_point[i]=(double)time*velocity0->v[i]+moving_origin->v[i];
        point.v[i]=PhysicsScalar(wide_point[i]);
    }
    if(other!=NULL) {
        BrMatrix34ApplyP(&world,&point,&other->transform_matrix);
        for(i=0;i<3;i++)PhysicsSubtractScalar(&relative.v[i],&world.v[i],&object->transform_matrix.m[3][i]);
        BrMatrix34TApplyV(&output->position,&relative,&object->transform_matrix);
        BrMatrix34ApplyV(&output->normal,&normal,&other->transform_matrix);
    } else {
        for(i=0;i<3;i++)relative.v[i]=PhysicsScalar(wide_point[i]-object->transform_matrix.m[3][i]);
        BrMatrix34TApplyV(&output->position,&relative,&object->transform_matrix);
        BrMatrix34TApplyV(&output->normal,&normal,&object->transform_matrix);
        for(i=0;i<3;i++)PhysicsCopyWord(output->field_0x38+4*i,&normal.v[i]);
    }
    PhysicsCopyWord(&direction_y,&old_direction->v[1]);
    PhysicsCopyWord(&direction_z,&old_direction->v[2]);
    output->reference_position.v[0]=PhysicsScalar((double)segment*old_direction->v[0]+old_origin->v[0]);
    output->reference_position.v[1]=PhysicsScalar((double)direction_y*segment+old_origin->v[1]);
    output->reference_position.v[2]=PhysicsScalar((double)direction_z*segment+old_origin->v[2]);
    return 1;
}

// CylinderCheck

// CheckTwoEdges

// WorldPointCollision

// CheckSingleEdgeWithObjectEdges

// GetEdgeToEdgePoints

// SetUpWorkspace

/* Complete wall-contact consumer; original lexical name unknown. */
// FUNCTION: CARMA2_HW 0x004228b0
int C2_HOOK_FASTCALL FUN_004228b0(tPhysics_object* object, tPhysics_contact* contacts, int initial_count) {
    volatile union {tU8 bytes[0x7e8];tU32 words[0x1fa];br_scalar values[0x1fa];} frame;
    tPhysics_wall_workspace* workspace=(tPhysics_wall_workspace*)(void*)&frame.bytes[0xe0];
    br_matrix34* delta=(br_matrix34*)(void*)&frame.bytes[0x80];
    br_matrix34* inverse=(br_matrix34*)(void*)&frame.bytes[0xb0];
    br_vector3* corner=(br_vector3*)(void*)&frame.bytes[0x48];
    br_vector3* intersection=(br_vector3*)(void*)&frame.bytes[0x34];
    br_vector3* trial_normal=(br_vector3*)(void*)&frame.bytes[0x54];
    br_vector3* best_normal=(br_vector3*)(void*)&frame.bytes[0x68];
    br_vector3* previous_normal=(br_vector3*)(void*)&frame.bytes[0x74];
    br_matrix34* current=&object->transform_matrix;
    br_matrix34* previous;
    tPhysics_shape* shape=object->shape;
    tPhysics_contact* first_contact;
    tPhysics_contact* next_contact;
    tCollision_shape_sphere_data* sphere;
    tFace_ref* face;
    tFace_ref* best_face;
    br_vector3* source;
    br_scalar* radius_product_x=(br_scalar*)(void*)&frame.values[0x34/4];
    br_scalar* radius_product_y=(br_scalar*)(void*)&frame.values[0x38/4];
    double negative_radius;
    double radius_product_z;
    double normal_dot;
    int count=0,touched_all=0,touched;
    int result,ordinal,point,component,sphere_count,classification=1;

    if(object->box_face_start==object->box_face_end)return 0;
    if(initial_count>36)return 0;
    BrMatrix34LPInverse(inverse,&object->actor->t.t.mat);
    BrMatrix34Mul(delta,current,inverse);
    while(shape!=NULL) {
        result=EliminateFaces(object,shape,delta);
        if(result!=0) {
            previous=gPhysics_contact_679438?current:&object->actor->t.t.mat;
            switch(shape->common.type) {
            case kCollisionShapeType_Box:
                workspace->count_points=8;workspace->count_edges=12;
                for(point=0;point<8;point++) {
                    PhysicsCopyWord(&corner->v[0],(point&2)?&shape->common.bb.min.v[0]:&shape->common.bb.max.v[0]);
                    PhysicsCopyWord(&corner->v[1],(point&1)?&shape->common.bb.min.v[1]:&shape->common.bb.max.v[1]);
                    PhysicsCopyWord(&corner->v[2],(point&4)?&shape->common.bb.min.v[2]:&shape->common.bb.max.v[2]);
                    BrMatrix34ApplyP(&workspace->original_points[point],corner,current);
                    BrMatrix34ApplyP(&workspace->trial_points[point],corner,previous);
                    workspace->vertex_flags[point]=0;
                }
                workspace->edges=gPhysics_box_edges_58f7f0;
                break;
            case kCollisionShapeType_Wireframe:
                workspace->count_points=(tU16)shape->wireframe.wireframe.count_points;
                workspace->count_edges=(tU16)shape->wireframe.wireframe.count_lines;
                for(point=0;point<(int)workspace->count_points;point++) {
                    source=&shape->wireframe.wireframe.points[point];
                    BrMatrix34ApplyP(&workspace->original_points[point],source,current);
                    source=&shape->wireframe.wireframe.points[point];
                    BrMatrix34ApplyP(&workspace->trial_points[point],source,previous);
                    workspace->vertex_flags[point]=0;
                }
                workspace->edges=shape->wireframe.wireframe.lines;
                break;
            case kCollisionShapeType_Polyhedron:
            case kCollisionShapeType_Wireframe_Polyhedron:
                workspace->count_points=(tU16)shape->polyhedron.polyhedron.count_points;
                workspace->count_edges=(tU16)shape->polyhedron.polyhedron.count_edges;
                for(point=0;point<(int)workspace->count_points;point++) {
                    source=&shape->polyhedron.polyhedron.points[point];
                    BrMatrix34ApplyP(&workspace->original_points[point],source,current);
                    source=&shape->polyhedron.polyhedron.points[point];
                    BrMatrix34ApplyP(&workspace->trial_points[point],source,previous);
                    workspace->vertex_flags[point]=0;
                }
                workspace->edges=shape->polyhedron.polyhedron.edges;
                break;
            case kCollisionShapeType_Sphere:
                workspace->count_points=1;workspace->count_edges=0;
                BrMatrix34ApplyP(&workspace->original_points[0],&shape->sphere.sphere.center,current);
                BrMatrix34ApplyP(&workspace->trial_points[0],&shape->sphere.sphere.center,previous);
                break;
            default:
                PhysicsError(ePhysicsError_UnknownShapeType,&gPhysics_empty_message);
            }
            if(gPhysics_contact_679438!=0) {
                BrMatrix34ApplyP(&workspace->original_points[0],&object->cmpos,current);
                for(point=1;point<(int)workspace->count_points;point++)for(component=0;component<3;component++)PhysicsCopyWord(&workspace->original_points[point].v[component],&workspace->original_points[0].v[component]);
            }
            if(shape->common.type!=kCollisionShapeType_Sphere) {
                touched=0;
                if(object->field_0xed!=0)count=FUN_004236a0(object,shape,contacts+initial_count,count,40-initial_count,workspace,delta,&touched);
                touched_all|=touched;
                count=GetCornerToFacePoints(object,shape,contacts+initial_count,count,40-initial_count,workspace,delta);
                if(touched==0)count=FUN_00424140(object,shape,contacts+initial_count,count,40-initial_count,workspace,delta);
            } else {
                sphere=&shape->sphere.sphere;
                sphere_count=count;
                frame.values[0x28/4]=2.f;
                ordinal=object->box_face_start;
                face=&gFace_list__car[ordinal];
                while(ordinal<(int)*(volatile tU16*)&object->box_face_end) {
                    SphereFaceCollide(face,&workspace->original_points[0],&workspace->trial_points[0],sphere->radius,intersection,trial_normal,(br_scalar*)(void*)&frame.values[0x40/4]);
                    if(PhysicsWallNaN(frame.values[0x40/4])||PhysicsWallNaN(frame.values[0x28/4])||frame.values[0x40/4]<frame.values[0x28/4]) {
                        PhysicsCopyWord((void*)&frame.values[0x28/4],(const void*)&frame.values[0x40/4]);
                        PhysicsCopyWord((void*)&frame.words[0x60/4],&face);
                        for(component=0;component<3;component++)PhysicsCopyWord(&best_normal->v[component],&trial_normal->v[component]);
                    }
                    ordinal++;face++;
                }
                if(PhysicsWallNaN(frame.values[0x28/4])||frame.values[0x28/4]<=1.f) {
                    BrMatrix34TApplyV(trial_normal,best_normal,current);
                    BrMatrix34TApplyV(previous_normal,best_normal,&object->actor->t.t.mat);
                    negative_radius=-(double)sphere->radius;
                    *radius_product_x=PhysicsScalar((double)previous_normal->v[0]*negative_radius);
                    *radius_product_y=PhysicsScalar((double)previous_normal->v[1]*negative_radius);
                    radius_product_z=negative_radius*previous_normal->v[2];
                    intersection->v[0]=PhysicsScalar(PhysicsWallAddScalar(&sphere->center.v[0],radius_product_x));
                    intersection->v[1]=PhysicsScalar(PhysicsWallAddScalar(&sphere->center.v[1],radius_product_y));
                    intersection->v[2]=PhysicsScalar((double)sphere->center.v[2]+radius_product_z);
                    PhysicsCopyWord(&best_face,(const void*)&frame.words[0x60/4]);
                    sphere_count=AddObjectWallForce(contacts+initial_count,intersection,frame.values[0x28/4],trial_normal,0,sphere_count,object,3,best_face,NULL);
                }
                result=GetSphereToEdgePoints(object,sphere,contacts+initial_count,sphere_count,40-initial_count,&workspace->original_points[0],&workspace->trial_points[0],delta);
                if(result>count)count++;
            }
        }
        PhysicsCopyWord(&shape,&shape->common.next);
    }
    object->field_0xed=(tU8)touched_all;
    if(count==0)return 0;
    if(count>1) {
        first_contact=contacts+initial_count;
        next_contact=first_contact+1;
        do {
            normal_dot=((double)first_contact->normal.v[0]*next_contact->normal.v[0]+(double)first_contact->normal.v[1]*next_contact->normal.v[1])+(double)first_contact->normal.v[2]*next_contact->normal.v[2];
            if(PhysicsWallNaN(normal_dot)||normal_dot<=.95)break;
            classification++;next_contact++;
        }while(classification<count);
    }
    for(component=0;component<3;component++)PhysicsCopyWord(&object->omega.v[component],&object->rotate_omega.v[component]);
    return classification;
}

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

/* Complete force-group foundation; original lexical names unknown. */
static double C2_HOOK_CDECL FrictionProductScalars(volatile const br_scalar* left, volatile const br_scalar* right);

typedef struct tForce_group_container {
    int count;
    tPhysics_joint_force* records;
    unsigned char unused_08[12];
    br_scalar* orthogonal;
    br_scalar* matrix;
} tForce_group_container;

typedef void (C2_HOOK_FASTCALL *tForce_group_callback)(tPhysics_object* object);

static br_scalar C2_HOOK_CDECL GroupScalar(double value) {
    volatile union { br_scalar value; tU32 bits; } stored;
    union { br_scalar value; tU32 bits; } result;
    stored.value = (br_scalar)value;
    result.bits = stored.bits;
    return result.value;
}

static void* C2_HOOK_CDECL GroupPointer(const void* base, int offset) {
    void* result;
    memcpy(&result, (const tU8*)base + offset, 4);
    return result;
}

static tPhysics_object** C2_HOOK_CDECL GroupReducedSlot(tPhysics_object* object) {
    return (tPhysics_object**)((tU8*)object + 0x234);
}

/* A nonempty child list makes its final slot an object field, never this
 * temporary head. Keep the temporary's address opaque to GCC's local-return
 * heuristic; the original returns the recursively produced slot verbatim. */
#if defined(__GNUC__)
__attribute__((noinline))
#endif
static tPhysics_object** C2_HOOK_CDECL GroupTemporaryHead(tPhysics_object** head) {
    return head;
}

/* Original ECX object / stack float / RET4; EDX is not consumed. */
// FUNCTION: CARMA2_HW 0x004bc8e0
void C2_HOOK_FAKE_THISCALL FUN_004bc8e0(tPhysics_object* object, undefined4 unused, br_scalar factor) {
    double x, y, z, scale;
    tPhysics_object* partner;
    tPhysics_object* ancestor;
    tForce_group_container* container;
    tPhysics_joint_force* records[40];
    void* workspace;
    int index;
    (void)unused;
    x = FrictionProductScalars(&object->I.v[0], &factor);
    y = FrictionProductScalars(&object->I.v[1], &factor);
    z = FrictionProductScalars(&object->I.v[2], &factor);
    scale = FrictionProductScalars(&factor, &object->field_0x21c);
    object->I.v[0] = GroupScalar(x);
    workspace = GroupPointer(object, 0x1e4);
    object->I.v[1] = GroupScalar(y);
    object->I.v[2] = GroupScalar(z);
    object->field_0x21c = GroupScalar(scale);
    if (workspace != NULL) {
        partner = GroupPointer(workspace, 4);
        x = FrictionProductScalars(&partner->I.v[0], &factor);
        y = FrictionProductScalars(&partner->I.v[1], &factor);
        z = FrictionProductScalars(&partner->I.v[2], &factor);
        scale = FrictionProductScalars(&factor, &partner->field_0x21c);
        partner->I.v[0] = GroupScalar(x);
        partner->I.v[1] = GroupScalar(y);
        partner->I.v[2] = GroupScalar(z);
        partner->field_0x21c = GroupScalar(scale);
        RedoQuickHingeVelocities(object);
    }
    ancestor = object;
    while (GroupPointer(ancestor, 0x218) == NULL && ancestor->parent != NULL) {
        ancestor = ancestor->parent;
    }
    container = GroupPointer(ancestor, 0x218);
    if (container != NULL) {
        for (index = 0; index < container->count; index++) {
            records[index] = (tPhysics_joint_force*)((tU8*)container->records + 96 * index);
        }
        MakeMatrix(container->matrix, container->count, records, container->count);
        MatrixProcess(container->matrix, container->orthogonal, container->count);
    }
}

// FUNCTION: CARMA2_HW 0x004bd720
int C2_HOOK_FASTCALL FUN_004bd720(tPhysics_object* object) {
    double x, y, z, energy, factor, denominator;
    void* workspace;
    tPhysics_object* partner;
    tForce_group_callback callback;
    x = object->omega.v[0];
    y = object->omega.v[1];
    z = object->omega.v[2];
    energy = ((x * x + y * y) + z * z) * (double)0.0015999999595806003f;
    if (!(energy > (double)2.464900016784668f)) {
        return 0;
    }
    workspace = GroupPointer(object, 0x1e4);
    factor = sqrt(energy) * (double)1.2738852500915527f;
    x = object->omega.v[0];
    y = object->omega.v[1];
    denominator = factor + factor;
    z = object->omega.v[2];
    x /= denominator;
    y /= denominator;
    z /= denominator;
    object->omega.v[0] = GroupScalar(x);
    object->omega.v[1] = GroupScalar(y);
    object->omega.v[2] = GroupScalar(z);
    if (workspace != NULL) {
        partner = GroupPointer(workspace, 4);
        partner->omega.v[0] = GroupScalar((double)partner->omega.v[0] / denominator);
        partner->omega.v[1] = GroupScalar((double)partner->omega.v[1] / denominator);
        partner->omega.v[2] = GroupScalar((double)partner->omega.v[2] / denominator);
    }
    FUN_004bc8e0(object, 0, GroupScalar(factor * factor));
    memcpy(&callback, (tU8*)object + 0x198, 4);
    if (callback != NULL) {
        callback(object);
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x004bd7f0
int C2_HOOK_FASTCALL FUN_004bd7f0(tPhysics_object* list) {
    tU32 total = 0;
    while (list != NULL) {
        total += (tU32)FUN_004bd720(list);
        if (list->child != NULL) {
            total += (tU32)FUN_004bd7f0(list->child);
        }
        list = list->next;
    }
    return (int)total;
}

// FUNCTION: CARMA2_HW 0x004bd830
tPhysics_object** C2_HOOK_FASTCALL FUN_004bd830(tPhysics_object* scan, tPhysics_object* context, tPhysics_object** output) {
    tPhysics_object** tail = output;
    tPhysics_object** child_tail;
    tPhysics_object* node;
    tPhysics_object* children;
    br_scalar cutoff;
    if (context->parent != NULL) {
        for (node = context; node != NULL; node = node->next) {
            if ((node == scan && !(scan->flags & 8)) || BoundsOverlapTest__finteray(&scan->field_0x10c, &node->field_0x10c)) {
                *tail = node;
                tail = GroupReducedSlot(node);
            }
        }
    } else {
        cutoff = GroupScalar((double)scan->field_0x10c.min.v[0] - gPhysics_bounds_max_x);
        for (node = scan->prev; node != NULL; node = node->prev) {
            if (!(node->transform_matrix.m[3][0] >= cutoff)) {
                break;
            }
            if (BoundsOverlapTest__finteray(&scan->field_0x10c, &node->field_0x10c)) {
                *tail = node;
                tail = GroupReducedSlot(node);
            }
        }
        cutoff = GroupScalar((double)scan->field_0x10c.max.v[0] - gPhysics_bounds_min_x);
        for (node = scan->next; node != NULL; node = node->next) {
            if (node->transform_matrix.m[3][0] > cutoff) {
                break;
            }
            if (BoundsOverlapTest__finteray(&scan->field_0x10c, &node->field_0x10c)) {
                *tail = node;
                tail = GroupReducedSlot(node);
            }
        }
        if (!(scan->flags & 8)) {
            *tail = scan;
            tail = GroupReducedSlot(scan);
        }
    }
    *tail = NULL;
    children = NULL;
    child_tail = GroupTemporaryHead(&children);
    for (node = *output; node != NULL; node = GroupPointer(node, 0x234)) {
        if (node->child != NULL) {
            child_tail = FUN_004bd830(scan, node->child, child_tail);
        }
    }
    if (children != NULL) {
        *child_tail = NULL;
        *tail = children;
        return child_tail;
    }
    return tail;
}

// FUNCTION: CARMA2_HW 0x004bd9b0
void C2_HOOK_FASTCALL FUN_004bd9b0(tPhysics_object* object, tU32 state) {
    tPhysics_object* child = object->child;
    object->field_0xf0 = state;
    while (child != NULL) {
        FUN_004bd9b0(child, state);
        child = child->next;
    }
}

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

/* Complete hierarchy motion entries; original lexical names unknown. */
static double C2_HOOK_CDECL PartnerMultiplyRawScalar(double a, const br_scalar* b);

/* Original calls push the time DWORD with MOV/PUSH. GNU's typed float
 * forwarding can instead FLD/FSTP it, quieting sNaNs before a raw-memory
 * multiply. These i386 call views preserve the actual three-DWORD ABI. */
#if defined(__GNUC__) && defined(__i386__)
static tU32 C2_HOOK_CDECL HierarchyTimeWord(const br_scalar* pTime) {
    tU32 word;
    memcpy(&word, pTime, sizeof(word));
    return word;
}
typedef void (C2_HOOK_FASTCALL *tHierarchyRawRecursive)(tPhysics_object*, tPhysics_object**, tU32);
typedef void (C2_HOOK_FAKE_THISCALL *tHierarchyRawMotion)(tPhysics_object*, undefined4, tU32);
#define HIERARCHY_RECURSE(object, selected, time) do { \
    tHierarchyRawRecursive volatile function = (tHierarchyRawRecursive)(void*)FUN_004b9230; \
    function((object), (selected), HierarchyTimeWord(&(time))); \
} while (0)
#define HIERARCHY_MOTION(function_name, object, time) do { \
    tHierarchyRawMotion volatile function = (tHierarchyRawMotion)(void*)(function_name); \
    function((object), 0, HierarchyTimeWord(&(time))); \
} while (0)
#else
#define HIERARCHY_RECURSE(object, selected, time) FUN_004b9230((object), (selected), (time))
#define HIERARCHY_MOTION(function_name, object, time) (function_name)((object), 0, (time))
#endif
// FUNCTION: CARMA2_HW 0x004b9230
void C2_HOOK_FASTCALL FUN_004b9230(tPhysics_object* pObject, tPhysics_object** pSelected, br_scalar pTime) {
    volatile tPhysics_object* object = pObject;
    tPhysics_object* partner;
    tPhysics_object* child;
    tPhysics_joint* joint;
    br_actor* actor;
    br_vector3 point, other, relative_omega, cross;
    br_matrix34 inverse, relative;
    br_scalar angle;
    br_scalar vx, vy, vz;
    double x, y, z;
    int i;

    joint = object->physics_joint1;
    if (joint != NULL && joint->type != eJoint_none) {
        partner = object->field_0x230;
        if (partner == NULL) {
            PhysicsError((tPhysicsError)11, &gPhysics_empty_message);
        }
        if (object->flags & 0x400) {
            BrMatrix34Mul(&object->actor->t.t.mat, &pObject->field_0x1e8, &partner->actor->t.t.mat);
            BrMatrix34TApplyV(&pObject->omega, &partner->rotate_omega, &pObject->field_0x1e8);
            memcpy(&pObject->rotate_omega, &pObject->omega, sizeof(br_vector3));
            DrMatrix34ApplyLPInverse(&point, &partner->cmpos, &pObject->field_0x1e8);
            x = (double)object->cmpos.v[0] - point.v[0];
            y = (double)object->cmpos.v[1] - point.v[1];
            z = (double)object->cmpos.v[2] - point.v[2];
            vx = object->omega.v[0];
            vy = object->omega.v[1];
            vz = object->omega.v[2];
            point.v[0] = (br_scalar)PhysicsJointScalar(x);
            point.v[1] = (br_scalar)PhysicsJointScalar(y);
            point.v[2] = (br_scalar)PhysicsJointScalar(z);
            cross.v[0] = (br_scalar)PhysicsJointScalar((double)vy * point.v[2] - (double)vz * point.v[1]);
            cross.v[1] = (br_scalar)PhysicsJointScalar((double)vz * point.v[0] - (double)vx * point.v[2]);
            cross.v[2] = (br_scalar)PhysicsJointScalar((double)vx * point.v[1] - (double)vy * point.v[0]);
            BrMatrix34ApplyV(&pObject->v, &cross, &pObject->transform_matrix);
            pObject->v.v[0] = (br_scalar)PhysicsJointScalar((double)partner->v.v[0] + object->v.v[0]);
            pObject->v.v[1] = (br_scalar)PhysicsJointScalar((double)partner->v.v[1] + object->v.v[1]);
            pObject->v.v[2] = (br_scalar)PhysicsJointScalar((double)partner->v.v[2] + object->v.v[2]);
            BrMatrix34TApplyV(&pObject->velocity_car_space, &pObject->v, &pObject->transform_matrix);
        } else {
            joint = object->physics_joint1;
            switch (joint->type) {
            case eJoint_hinge:
            case eJoint_quick_hinge:
                BrMatrix34ApplyV(&point, &partner->rotate_omega, &partner->transform_matrix);
                BrMatrix34TApplyV(&relative_omega, &point, &pObject->transform_matrix);
                joint = object->physics_joint1;
                x = ((double)object->omega.v[0] - relative_omega.v[0]) * joint->hinge_axis.v[0];
                y = ((double)object->omega.v[1] - relative_omega.v[1]) * joint->hinge_axis.v[1];
                z = ((double)object->omega.v[2] - relative_omega.v[2]) * joint->hinge_axis.v[2];
                angle = (br_scalar)PhysicsJointScalar(((x + y) + z) * pTime);
                BrMatrix34LPInverse(&inverse, &partner->transform_matrix);
                BrMatrix34Mul(&relative, &pObject->transform_matrix, &inverse);
                BrMatrix34Mul(&object->actor->t.t.mat, &relative, &partner->actor->t.t.mat);
                joint = object->physics_joint1;
                BrMatrix34PreRotate(&object->actor->t.t.mat, (br_angle)(br_int_64)((double)angle * 10430.378350470453), &joint->hinge_axis);
                memcpy(&pObject->rotate_omega, &pObject->omega, sizeof(br_vector3));
                BrMatrix34ApplyV(&point, &pObject->omega, &pObject->transform_matrix);
                BrMatrix34TApplyV(&pObject->omega, &point, &object->actor->t.t.mat);
                break;
            case eJoint_universal:
            case eJoint_ball_n_socket:
                HIERARCHY_MOTION(RotateObject, pObject, pTime);
                break;
            default:
                PhysicsError((tPhysicsError)5, NULL);
            }
            BrMatrix34ApplyP(&point, &object->physics_joint1->field_0x08, &object->actor->t.t.mat);
            BrMatrix34ApplyP(&other, &object->physics_joint1->field_0x14, &partner->actor->t.t.mat);
            for (i = 0; i < 3; ++i) {
                point.v[i] = (br_scalar)PhysicsJointScalar((double)other.v[i] - point.v[i]);
            }
            for (i = 0; i < 3; ++i) {
                actor = object->actor;
                actor->t.t.mat.m[3][i] = (br_scalar)PhysicsJointScalar((double)point.v[i] + actor->t.t.mat.m[3][i]);
            }
        }
    } else {
        joint = object->physics_joint2;
        if (joint != NULL && joint->type != eJoint_none) {
            HIERARCHY_MOTION(RotateObject, pObject, pTime);
            HIERARCHY_MOTION(TranslateObject, pObject, pTime);
        } else {
            x = PhysicsJointProductMemory(&pObject->v.v[0], &pTime);
            actor = object->actor;
            y = object->v.v[1];
            x = PhysicsJointAddScalar(x, &actor->t.t.mat.m[3][0]);
            z = object->v.v[2];
            y = PartnerMultiplyRawScalar(y, &pTime);
            actor->t.t.mat.m[3][0] = (br_scalar)PhysicsJointScalar(x);
            actor = object->actor;
            y = PhysicsJointAddScalar(y, &actor->t.t.mat.m[3][1]);
            z = PartnerMultiplyRawScalar(z, &pTime);
            actor->t.t.mat.m[3][1] = (br_scalar)PhysicsJointScalar(y);
            actor = object->actor;
            actor->t.t.mat.m[3][2] = (br_scalar)PhysicsJointScalar(PhysicsJointAddScalar(z, &actor->t.t.mat.m[3][2]));
            HIERARCHY_MOTION(RotateObject, pObject, pTime);
        }
    }
    child = object->child;
    while (child != NULL) {
        HIERARCHY_RECURSE(child, pSelected, pTime);
        child = ((volatile tPhysics_object*)child)->next;
    }
    if (object->physics_joint2 != NULL) {
        *pSelected = pObject;
    }
}

// FUNCTION: CARMA2_HW 0x004b8ee0
void C2_HOOK_FAKE_THISCALL FUN_004b8ee0(tPhysics_object* pObject, undefined4 pUnused, br_scalar pTime) {
    tPhysics_object* selected = NULL;
    tPhysics_object* current;
    tPhysics_object* next;
    br_vector3 point, other;
    br_actor* actor;
    int i;

    (void)pUnused;
    HIERARCHY_RECURSE(pObject, &selected, pTime);
    if (selected != NULL && ((volatile tPhysics_object*)selected)->field_0x230 != NULL) {
        current = selected;
        HIERARCHY_MOTION(TranslateObject, selected, pTime);
        next = ((volatile tPhysics_object*)current)->field_0x230;
        do {
            BrMatrix34ApplyP(&point, &current->physics_joint1->field_0x08, &current->actor->t.t.mat);
            BrMatrix34ApplyP(&other, &current->physics_joint1->field_0x14, &next->actor->t.t.mat);
            for (i = 0; i < 3; ++i) {
                point.v[i] = (br_scalar)PhysicsJointScalar((double)point.v[i] - other.v[i]);
            }
            for (i = 0; i < 3; ++i) {
                actor = ((volatile tPhysics_object*)((volatile tPhysics_object*)current)->parent)->actor;
                actor->t.t.mat.m[3][i] = (br_scalar)PhysicsJointScalar((double)point.v[i] + actor->t.t.mat.m[3][i]);
            }
            current = ((volatile tPhysics_object*)current)->field_0x230;
            next = ((volatile tPhysics_object*)current)->field_0x230;
        } while (next != NULL);
        WriteNewPositions(pObject);
    }
}
#undef HIERARCHY_RECURSE
#undef HIERARCHY_MOTION

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

#if defined(_MSC_VER) && _MSC_VER < 1300
#pragma auto_inline(off)
#endif
static double C2_HOOK_CDECL FrictionRead(const void* base, unsigned offset) {
    return *(volatile const br_scalar*)((const tU8*)base + offset);
}
static tU32 C2_HOOK_CDECL FrictionWord(const void* base, unsigned offset) {
    return *(volatile const tU32*)((const tU8*)base + offset);
}
static void* C2_HOOK_CDECL FrictionPointer(const void* base, unsigned offset) {
    return (void*)(uintptr_t)FrictionWord(base, offset);
}
static br_scalar C2_HOOK_CDECL FrictionScalar(double value) {
    volatile union { br_scalar scalar; tU32 bits; } stored;
    union { br_scalar scalar; tU32 bits; } result;
    stored.scalar = (br_scalar)value;
    result.bits = stored.bits;
    return result.scalar;
}
static void C2_HOOK_CDECL FrictionWrite(void* base, unsigned offset, double value) {
    *(volatile br_scalar*)((tU8*)base + offset) = FrictionScalar(value);
}
static br_matrix34* C2_HOOK_CDECL FrictionActorMatrix(const void* object) {
    return (br_matrix34*)((tU8*)FrictionPointer(object, 0) + 0x2c);
}
static void C2_HOOK_CDECL FrictionFlag(void* object, unsigned flag) {
    volatile tU8* field = (volatile tU8*)object + 0x1dd;
    *field = (tU8)(*field | flag);
}
/* Paired signaling/quiet NaN fixtures show that promoting both operands
 * changes original raw-memory selection. Keep only these scalar operand
 * forms x87; complete consumer control flow and accumulation stay readable C.
 */
static volatile const br_scalar* C2_HOOK_CDECL FrictionAt(const void* base, unsigned offset) {
    return (volatile const br_scalar*)((const tU8*)base + offset);
}

static double C2_HOOK_CDECL FrictionProductScalars(volatile const br_scalar* left, volatile const br_scalar* right) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, left
        mov edx, right
        fld dword ptr [eax]
        fmul dword ptr [edx]
        fstp qword ptr result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("flds %1; fmuls %2; fstpl %0" : "=m"(result) : "m"(*left), "m"(*right) : "st");
#else
    result = (double)*left * (double)*right;
#endif
    return result;
}

static double C2_HOOK_CDECL FrictionDivideScalars(volatile const br_scalar* left, volatile const br_scalar* right) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, left
        mov edx, right
        fld dword ptr [eax]
        fdiv dword ptr [edx]
        fstp qword ptr result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("flds %1; fdivs %2; fstpl %0" : "=m"(result) : "m"(*left), "m"(*right) : "st");
#else
    result = (double)*left / (double)*right;
#endif
    return result;
}

static double C2_HOOK_CDECL FrictionSubtractScalar(double left, volatile const br_scalar* right) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov edx, right
        fld qword ptr left
        fsub dword ptr [edx]
        fstp qword ptr result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("fldl %1; fsubs %2; fstpl %0" : "=m"(result) : "m"(left), "m"(*right) : "st");
#else
    result = left - (double)*right;
#endif
    return result;
}

static double C2_HOOK_CDECL FrictionAddScalar(double left, volatile const br_scalar* right) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov edx, right
        fld qword ptr left
        fadd dword ptr [edx]
        fstp qword ptr result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("fldl %1; fadds %2; fstpl %0" : "=m"(result) : "m"(left), "m"(*right) : "st");
#else
    result = left + (double)*right;
#endif
    return result;
}

static double C2_HOOK_CDECL FrictionReverseSubtractScalar(double right, volatile const br_scalar* left) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, left
        fld qword ptr right
        fsubr dword ptr [eax]
        fstp qword ptr result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("fldl %1; fsubrs %2; fstpl %0" : "=m"(result) : "m"(right), "m"(*left) : "st");
#else
    result = (double)*left - right;
#endif
    return result;
}

static double C2_HOOK_CDECL FrictionReverseDivideScalar(double divisor, volatile const br_scalar* numerator) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, numerator
        fld qword ptr divisor
        fdivr dword ptr [eax]
        fstp qword ptr result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("fldl %1; fdivrs %2; fstpl %0" : "=m"(result) : "m"(divisor), "m"(*numerator) : "st");
#else
    result = (double)*numerator / divisor;
#endif
    return result;
}

static double C2_HOOK_CDECL FrictionAddWide(double left, double right) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {

        fld qword ptr left
        fadd qword ptr right
        fstp qword ptr result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("fldl %1; faddl %2; fstpl %0" : "=m"(result) : "m"(left), "m"(right) : "st");
#else
    result = left + right;
#endif
    return result;
}

static double C2_HOOK_CDECL FrictionProductWide(double left, double right) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {

        fld qword ptr left
        fmul qword ptr right
        fstp qword ptr result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("fldl %1; fmull %2; fstpl %0" : "=m"(result) : "m"(left), "m"(right) : "st");
#else
    result = left * right;
#endif
    return result;
}
#if defined(_MSC_VER) && _MSC_VER < 1300
/* The retained nonfinite probes demonstrate that MSVC5 reverses some C
 * comparisons and thereby changes their unordered result. Preserve only
 * the original FCOM status-bit predicate, with the arithmetic still in C.
 */
static unsigned C2_HOOK_CDECL FrictionStatus(double left, double right) {
    unsigned short status;
    __asm {
        fld qword ptr left
        fcomp qword ptr right
        fnstsw ax
        mov status, ax
    }
    return status;
}
#endif
static int C2_HOOK_CDECL FrictionGreater(double left, double right) {
#if defined(_MSC_VER) && _MSC_VER < 1300
    return (FrictionStatus(left, right) & 0x4100) == 0;
#else
    return left > right;
#endif
}
static int C2_HOOK_CDECL FrictionLessOrUnordered(double left, double right) {
#if defined(_MSC_VER) && _MSC_VER < 1300
    return (FrictionStatus(left, right) & 0x100) != 0;
#else
    return !(left >= right);
#endif
}
static int C2_HOOK_CDECL FrictionNonZero(double value) {
#if defined(_MSC_VER) && _MSC_VER < 1300
    return (FrictionStatus(value, 0.0) & 0x4000) == 0;
#else
    return value > 0.0 || value < 0.0;
#endif
}

#if defined(_MSC_VER) && _MSC_VER < 1300
#pragma auto_inline(on)
#endif
// FUNCTION: CARMA2_HW 0x004c0ac0
void C2_HOOK_FASTCALL NewAddFriction(tPhysics_joint_force* force, tWorld_callbacks* callbacks) {
    tPhysics_object* object_a = FrictionPointer(force, 4);
    tPhysics_object* object_b = FrictionPointer(force, 8);
    br_vector3 velocity_a, velocity_b, world, normal, direction_a, direction_b;
    br_matrix34* actor_matrix;
    br_vector3 angular_a, cross, scaled_lever;
    br_scalar coefficient, maximum, stored_speed, dot, base;
    double tangent_x, tangent_y, tangent_z, speed, response, inverse_speed;
    double angular_b_x, angular_b_y, angular_b_z, denominator, impulse, scale, negative;
    double callback_coefficient, wide_dot;
    unsigned i;

    if (object_b != NULL && FrictionWord(force, 0x5c) != 14) {
        coefficient = FrictionScalar(FrictionRead(force, 0));
        FrictionFlag(object_a, 2);
        FrictionFlag(object_b, 2);
        velocity_a.v[0] = FrictionScalar(FrictionRead(force, 0x1c) * FrictionRead(object_a, 0x78) - FrictionRead(force, 0x18) * FrictionRead(object_a, 0x7c) + FrictionRead(object_a, 0x1a8));
        velocity_a.v[1] = FrictionScalar(FrictionRead(object_a, 0x7c) * FrictionRead(force, 0x14) - FrictionRead(object_a, 0x74) * FrictionRead(force, 0x1c) + FrictionRead(object_a, 0x1ac));
        velocity_a.v[2] = FrictionScalar(FrictionRead(object_a, 0x74) * FrictionRead(force, 0x18) - FrictionRead(object_a, 0x78) * FrictionRead(force, 0x14) + FrictionRead(object_a, 0x1b0));
        velocity_b.v[0] = FrictionScalar(FrictionRead(object_b, 0x78) * FrictionRead(force, 0x28) - FrictionRead(object_b, 0x7c) * FrictionRead(force, 0x24));
        velocity_b.v[1] = FrictionScalar(FrictionRead(force, 0x20) * FrictionRead(object_b, 0x7c) - FrictionRead(object_b, 0x74) * FrictionRead(force, 0x28));
        velocity_b.v[2] = FrictionScalar(FrictionRead(object_b, 0x74) * FrictionRead(force, 0x24) - FrictionRead(object_b, 0x78) * FrictionRead(force, 0x20));
        for (i = 0; i < 3; i++) velocity_b.v[i] = FrictionScalar((double)velocity_b.v[i] + FrictionRead(object_b, 0x1a8 + 4 * i));
        actor_matrix = FrictionActorMatrix(object_b);
        BrMatrix34ApplyV(&world, &velocity_b, actor_matrix);
        actor_matrix = FrictionActorMatrix(object_a);
        BrMatrix34TApplyV(&velocity_b, &world, actor_matrix);
        for (i = 0; i < 3; i++) velocity_a.v[i] = FrictionScalar((double)velocity_a.v[i] - velocity_b.v[i]);
        BrMatrix34ApplyV(&world, (const br_vector3*)((const tU8*)force + 0x2c), (const br_matrix34*)((const tU8*)object_a + 0x8c));
        actor_matrix = FrictionActorMatrix(object_a);
        BrMatrix34TApplyV(&normal, &world, actor_matrix);
        response = ((double)normal.v[1] * velocity_a.v[1] + (double)normal.v[0] * velocity_a.v[0]) + (double)normal.v[2] * velocity_a.v[2];
        world.v[0] = FrictionScalar((double)normal.v[0] * response);
        world.v[1] = FrictionScalar((double)normal.v[1] * response);
        world.v[2] = FrictionScalar((double)normal.v[2] * response);
        tangent_x = (double)velocity_a.v[0] - world.v[0];
        tangent_y = (double)velocity_a.v[1] - world.v[1];
        tangent_z = (double)velocity_a.v[2] - world.v[2];
        velocity_a.v[1] = FrictionScalar(tangent_y);
        speed = sqrt((tangent_y * velocity_a.v[1] + tangent_x * tangent_x) + tangent_z * tangent_z);
        stored_speed = FrictionScalar(speed);
        base = FrictionScalar((double)coefficient * (double)0.3499999940395355f);
        maximum = FrictionScalar(FrictionRead(object_a, 0x64) * base + base);
        maximum = FrictionScalar(FrictionRead(object_b, 0x64) * maximum + maximum);
        if (!FrictionLessOrUnordered(speed, (double)0.0010000000474974513f)) {
            inverse_speed = 1.0 / -(double)stored_speed;
            direction_a.v[0] = FrictionScalar(inverse_speed * tangent_x);
            direction_a.v[1] = FrictionScalar(inverse_speed * velocity_a.v[1]);
            direction_a.v[2] = FrictionScalar(inverse_speed * tangent_z);
            actor_matrix = FrictionActorMatrix(object_a);
            BrMatrix34ApplyV(&world, &direction_a, actor_matrix);
            actor_matrix = FrictionActorMatrix(object_b);
            BrMatrix34TApplyV(&direction_b, &world, actor_matrix);
            if ((FrictionWord(object_a, 0x19c) & 0x800) == 0) {
                angular_a.v[0] = FrictionScalar((FrictionRead(force, 0x18) * direction_a.v[2] - FrictionRead(force, 0x1c) * direction_a.v[1]) / FrictionRead(object_a, 8));
                angular_a.v[1] = FrictionScalar((FrictionRead(force, 0x1c) * direction_a.v[0] - FrictionRead(force, 0x14) * direction_a.v[2]) / FrictionRead(object_a, 0xc));
                angular_a.v[2] = FrictionScalar((FrictionRead(force, 0x14) * direction_a.v[1] - FrictionRead(force, 0x18) * direction_a.v[0]) / FrictionRead(object_a, 0x10));
            } else {
                angular_a.v[0] = angular_a.v[1] = angular_a.v[2] = 0.0f;
            }
            if ((FrictionWord(object_b, 0x19c) & 0x800) == 0) {
                angular_b_z = FrictionScalar((FrictionRead(force, 0x20) * direction_b.v[1] - FrictionRead(force, 0x24) * direction_b.v[0]) / FrictionRead(object_b, 0x10));
                angular_b_x = (FrictionRead(force, 0x24) * direction_b.v[2] - FrictionRead(force, 0x28) * direction_b.v[1]) / FrictionRead(object_b, 8);
                angular_b_y = (FrictionRead(force, 0x28) * direction_b.v[0] - FrictionRead(force, 0x20) * direction_b.v[2]) / FrictionRead(object_b, 0xc);
            } else {
                angular_b_z = angular_b_x = angular_b_y = 0.0;
            }
            world.v[0] = FrictionScalar(FrictionRead(force, 0x28) * angular_b_y - FrictionRead(force, 0x24) * angular_b_z);
            world.v[1] = FrictionScalar(FrictionRead(force, 0x20) * angular_b_z - FrictionRead(force, 0x28) * angular_b_x);
            world.v[2] = FrictionScalar(FrictionRead(force, 0x24) * angular_b_x - FrictionRead(force, 0x20) * angular_b_y);
            base = FrictionScalar(((FrictionRead(force, 0x14) * angular_a.v[2] - FrictionRead(force, 0x1c) * angular_a.v[0]) * direction_a.v[1] + (FrictionRead(force, 0x1c) * angular_a.v[1] - FrictionRead(force, 0x18) * angular_a.v[2]) * direction_a.v[0]) + (FrictionRead(force, 0x18) * angular_a.v[0] - FrictionRead(force, 0x14) * angular_a.v[1]) * direction_a.v[2] + 1.0 / FrictionRead(object_a, 4) + 1.0 / FrictionRead(object_b, 4));
            denominator = ((double)direction_b.v[1] * world.v[1] + (double)direction_b.v[0] * world.v[0]) + (double)direction_b.v[2] * world.v[2] + base;
            if (FrictionGreater(fabs(denominator), (double)9.999999747378752e-05f)) {
                impulse = (double)stored_speed / denominator;
                if (FrictionGreater(impulse, maximum)) impulse = maximum;
                scale = impulse / FrictionRead(object_a, 4);
                for (i = 0; i < 3; i++) direction_a.v[i] = FrictionScalar((double)direction_a.v[i] * scale);
                scale = FrictionScalar(-(impulse / FrictionRead(object_b, 4)));
                for (i = 0; i < 3; i++) direction_b.v[i] = FrictionScalar((double)direction_b.v[i] * scale);
                FrictionWrite(force, 0x44, FrictionRead(object_a, 4) * direction_a.v[0]);
                FrictionWrite(force, 0x48, FrictionRead(object_a, 4) * direction_a.v[1]);
                FrictionWrite(force, 0x4c, FrictionRead(object_a, 4) * direction_a.v[2]);
                FrictionWrite(force, 0x50, FrictionRead(object_b, 4) * direction_b.v[0]);
                FrictionWrite(force, 0x54, FrictionRead(object_b, 4) * direction_b.v[1]);
                response = FrictionRead(object_b, 4);
                *(volatile tU32*)((tU8*)force + 0x5c) = FrictionWord(force, 0x5c) | 0x100;
                FrictionWrite(force, 0x58, response * direction_b.v[2]);
                for (i = 0; i < 3; i++) FrictionWrite(object_a, 0x74 + 4 * i, impulse * angular_a.v[i] + FrictionRead(object_a, 0x74 + 4 * i));
                negative = FrictionScalar(-impulse);
                FrictionWrite(object_b, 0x74, angular_b_x * negative + FrictionRead(object_b, 0x74));
                FrictionWrite(object_b, 0x78, angular_b_y * negative + FrictionRead(object_b, 0x78));
                FrictionWrite(object_b, 0x7c, angular_b_z * negative + FrictionRead(object_b, 0x7c));
                actor_matrix = FrictionActorMatrix(object_a);
                BrMatrix34ApplyV(&world, &direction_a, actor_matrix);
                for (i = 0; i < 3; i++) FrictionWrite(object_a, 0x68 + 4 * i, (double)world.v[i] + FrictionRead(object_a, 0x68 + 4 * i));
                for (i = 0; i < 3; i++) FrictionWrite(object_a, 0x1a8 + 4 * i, (double)direction_a.v[i] + FrictionRead(object_a, 0x1a8 + 4 * i));
                actor_matrix = FrictionActorMatrix(object_b);
                BrMatrix34ApplyV(&world, &direction_b, actor_matrix);
                for (i = 0; i < 3; i++) FrictionWrite(object_b, 0x68 + 4 * i, (double)world.v[i] + FrictionRead(object_b, 0x68 + 4 * i));
                for (i = 0; i < 3; i++) FrictionWrite(object_b, 0x1a8 + 4 * i, (double)direction_b.v[i] + FrictionRead(object_b, 0x1a8 + 4 * i));
            }
        }
        PostAddFriction(FrictionPointer(force, 4));
        PostAddFriction(FrictionPointer(force, 8));
        return;
    }

    /* The single-object branch saves the raw coefficient before the callback. */
    memcpy(&coefficient, force, sizeof(coefficient));
    FrictionFlag(object_a, 1);
    if (FrictionWord(force, 0x5c) == 1) {
        for (i = 0; i < 3; i++) scaled_lever.v[i] = FrictionScalar(FrictionRead(force, 0x14 + 4 * i) * (double)-0.004999999888241291f + FrictionRead(force, 0x14 + 4 * i));
    } else {
        memcpy(&scaled_lever, (const tU8*)force + 0x14, sizeof(scaled_lever));
    }
    velocity_a.v[0] = FrictionScalar(FrictionRead(object_a, 0x78) * scaled_lever.v[2] - FrictionRead(object_a, 0x7c) * scaled_lever.v[1] + FrictionRead(object_a, 0x1a8));
    velocity_a.v[1] = FrictionScalar(FrictionRead(object_a, 0x7c) * scaled_lever.v[0] - FrictionRead(object_a, 0x74) * scaled_lever.v[2] + FrictionRead(object_a, 0x1ac));
    velocity_a.v[2] = FrictionScalar(FrictionRead(object_a, 0x74) * scaled_lever.v[1] - FrictionRead(object_a, 0x78) * scaled_lever.v[0] + FrictionRead(object_a, 0x1b0));
    BrMatrix34ApplyV(&world, (const br_vector3*)((const tU8*)force + 0x2c), (const br_matrix34*)((const tU8*)object_a + 0x8c));
    actor_matrix = FrictionActorMatrix(object_a);
    BrMatrix34TApplyV(&normal, &world, actor_matrix);
    response = ((double)normal.v[1] * velocity_a.v[1] + (double)normal.v[0] * velocity_a.v[0]) + (double)normal.v[2] * velocity_a.v[2];
    for (i = 0; i < 3; i++) world.v[i] = FrictionScalar(response * normal.v[i]);
    for (i = 0; i < 3; i++) velocity_a.v[i] = FrictionScalar((double)velocity_a.v[i] - world.v[i]);
    if (callbacks != NULL && FrictionWord(callbacks, 0x18) != 0 && FrictionWord(force, 0xc) != 0) {
        callback_coefficient = (double)callbacks->get_friction_from_face(FrictionPointer(force, 0xc)) * (double)coefficient;
    } else {
        callback_coefficient = coefficient;
    }
    speed = sqrt(((double)velocity_a.v[0] * velocity_a.v[0] + (double)velocity_a.v[1] * velocity_a.v[1]) + (double)velocity_a.v[2] * velocity_a.v[2]);
    stored_speed = FrictionScalar(speed);
    base = FrictionScalar(callback_coefficient * (double)0.3499999940395355f);
    maximum = FrictionScalar(FrictionRead(object_a, 0x60) * base + base);
    if (!FrictionLessOrUnordered(speed, (double)0.0010000000474974513f)) {
        inverse_speed = 1.0 / -(double)stored_speed;
        for (i = 0; i < 3; i++) direction_a.v[i] = FrictionScalar(inverse_speed * velocity_a.v[i]);
        if ((FrictionWord(object_a, 0x19c) & 0x800) == 0) {
            angular_a.v[0] = FrictionScalar(FrictionRead(force, 0x18) * direction_a.v[2] - FrictionRead(force, 0x1c) * direction_a.v[1]);
            angular_a.v[1] = FrictionScalar(FrictionRead(force, 0x1c) * direction_a.v[0] - FrictionRead(force, 0x14) * direction_a.v[2]);
            angular_a.v[2] = FrictionScalar(FrictionRead(force, 0x14) * direction_a.v[1] - FrictionRead(force, 0x18) * direction_a.v[0]);
            for (i = 0; i < 3; i++) angular_a.v[i] = FrictionScalar((double)angular_a.v[i] / FrictionRead(object_a, 8 + 4 * i));
        } else {
            angular_a.v[0] = angular_a.v[1] = angular_a.v[2] = 0.0f;
        }
        wide_dot = (FrictionRead(object_a, 0x74) * FrictionRead(force, 0x2c) + FrictionRead(force, 0x34) * FrictionRead(object_a, 0x7c)) + FrictionRead(object_a, 0x78) * FrictionRead(force, 0x30);
        dot = FrictionScalar(wide_dot);
        response = (FrictionRead(object_a, 0x74) * angular_a.v[0] + FrictionRead(object_a, 0x7c) * angular_a.v[2]) + FrictionRead(object_a, 0x78) * angular_a.v[1];
        speed = (FrictionRead(object_a, 0x68) * FrictionRead(object_a, 0x68) + FrictionRead(object_a, 0x6c) * FrictionRead(object_a, 0x6c)) + FrictionRead(object_a, 0x70) * FrictionRead(object_a, 0x70);
        if (FrictionLessOrUnordered(fabs(wide_dot), (double)0.10000000149011612f) && FrictionLessOrUnordered(response, 0.0) && FrictionLessOrUnordered(speed, 0.001)) {
            for (i = 0; i < 3; i++) world.v[i] = FrictionScalar(FrictionRead(force, 0x2c + 4 * i) * dot);
            for (i = 0; i < 3; i++) FrictionWrite(object_a, 0x74 + 4 * i, FrictionRead(object_a, 0x74 + 4 * i) - world.v[i]);
            BrVector3SetFloat(&angular_a, 0.0f, 0.0f, 0.0f);
            direction_a.v[0] = FrictionScalar(FrictionRead(object_a, 0x1a8));
            direction_a.v[2] = FrictionScalar(FrictionRead(object_a, 0x1b0));
            world.v[1] = FrictionScalar(FrictionRead(object_a, 0x1ac));
            speed = sqrt(((double)direction_a.v[0] * direction_a.v[0] + (double)world.v[1] * world.v[1]) + (double)direction_a.v[2] * direction_a.v[2]);
            stored_speed = FrictionScalar(speed);
            if (FrictionLessOrUnordered(speed, (double)0.0010000000474974513f)) return;
            negative = -(double)stored_speed;
            direction_a.v[0] = FrictionScalar((double)direction_a.v[0] / negative);
            direction_a.v[1] = FrictionScalar((double)world.v[1] / negative);
            direction_a.v[2] = FrictionScalar((double)direction_a.v[2] / negative);
        }
        cross.v[0] = FrictionScalar(FrictionRead(force, 0x1c) * angular_a.v[1] - FrictionRead(force, 0x18) * angular_a.v[2]);
        cross.v[1] = FrictionScalar(FrictionRead(force, 0x14) * angular_a.v[2] - FrictionRead(force, 0x1c) * angular_a.v[0]);
        cross.v[2] = FrictionScalar(FrictionRead(force, 0x18) * angular_a.v[0] - FrictionRead(force, 0x14) * angular_a.v[1]);
        denominator = ((double)direction_a.v[1] * cross.v[1] + (double)direction_a.v[0] * cross.v[0]) + (double)direction_a.v[2] * cross.v[2] + 1.0 / FrictionRead(object_a, 4);
        if (FrictionGreater(fabs(denominator), (double)9.999999747378752e-05f)) {
            impulse = (double)stored_speed / denominator;
            if (FrictionGreater(impulse, maximum)) impulse = maximum;
            for (i = 0; i < 3; i++) angular_a.v[i] = FrictionScalar(impulse * angular_a.v[i]);
            scale = impulse / FrictionRead(object_a, 4);
            for (i = 0; i < 3; i++) direction_a.v[i] = FrictionScalar((double)direction_a.v[i] * scale);
            FrictionWrite(force, 0x44, FrictionRead(object_a, 4) * direction_a.v[0]);
            FrictionWrite(force, 0x48, FrictionRead(object_a, 4) * direction_a.v[1]);
            response = FrictionRead(object_a, 4);
            *(volatile tU32*)((tU8*)force + 0x5c) = FrictionWord(force, 0x5c) | 0x100;
            FrictionWrite(force, 0x4c, response * direction_a.v[2]);
            for (i = 0; i < 3; i++) FrictionWrite(object_a, 0x74 + 4 * i, (double)angular_a.v[i] + FrictionRead(object_a, 0x74 + 4 * i));
            actor_matrix = FrictionActorMatrix(object_a);
            BrMatrix34ApplyV(&world, &direction_a, actor_matrix);
            for (i = 0; i < 3; i++) FrictionWrite(object_a, 0x68 + 4 * i, (double)world.v[i] + FrictionRead(object_a, 0x68 + 4 * i));
            for (i = 0; i < 3; i++) FrictionWrite(object_a, 0x1a8 + 4 * i, (double)direction_a.v[i] + FrictionRead(object_a, 0x1a8 + 4 * i));
            PostAddFriction(FrictionPointer(force, 4));
        }
    }
}

// FUNCTION: CARMA2_HW 0x004c1840
void C2_HOOK_FASTCALL PostAddFriction(tPhysics_object* object) {
    double x = FrictionRead(object, 0x74);
    double y = FrictionRead(object, 0x78);
    double z = FrictionRead(object, 0x7c);
    double energy = ((x * x + y * y) + z * z) * (double)0.0015999999595806003f;
    if (FrictionGreater(energy, (double)2.464900016784668f)) {
        double denominator = sqrt(energy) * (double)0.6369426250457764f;
        br_scalar stored_denominator;
        x = FrictionRead(object, 0x74);
        y = FrictionRead(object, 0x78);
        z = FrictionRead(object, 0x7c);
        stored_denominator = FrictionScalar(denominator + denominator);
        x /= stored_denominator;
        y /= stored_denominator;
        z /= stored_denominator;
        FrictionWrite(object, 0x74, x);
        FrictionWrite(object, 0x78, y);
        FrictionWrite(object, 0x7c, z);
    }
}

// FUNCTION: CARMA2_HW 0x004c18d0
void C2_HOOK_FASTCALL DoJointFriction(tPhysics_object* object) {
    tPhysics_object* parent;
    tPhysics_object* child;
    void* joint = FrictionPointer(object, 0x188);
    br_scalar impulse[3], responses[3][3];
    br_vector3 axis;
    br_scalar limit, accumulated, base_numerator;
    double wide_limit, sum, scale, response, numerator, denominator, negative;
    double x, y, z;
    unsigned i, j;

    if (joint != NULL && FrictionNonZero(FrictionRead(joint, 4)) && (parent = FrictionPointer(object, 0x230)) != NULL) {
        scale = FrictionRead(joint, 4) * (double)0.03999999910593033f;
        limit = FrictionScalar(scale * scale);
        accumulated = 0.0f;
        for (i = 0; i < 3; i++) {
            BrMatrix34TApplyV(&axis, (const br_vector3*)((const tU8*)object + 0x8c + 12 * i), (const br_matrix34*)((const tU8*)parent + 0x8c));
            parent = FrictionPointer(object, 0x230);
            x = FrictionProductScalars(FrictionAt(parent, 0x74), &axis.v[0]);
            z = FrictionProductScalars(FrictionAt(parent, 0x7c), &axis.v[2]);
            y = FrictionProductScalars(FrictionAt(parent, 0x78), &axis.v[1]);
            numerator = FrictionScalar(FrictionSubtractScalar(FrictionAddWide(FrictionAddWide(x, z), y), FrictionAt(object, 0x74 + 4 * i)));
            for (j = 0; j < 3; j++) {
                responses[i][j] = FrictionScalar(FrictionDivideScalars(&axis.v[j], FrictionAt(parent, 8 + 4 * j)));
            }
            x = FrictionProductScalars(&responses[i][0], &axis.v[0]);
            z = FrictionProductScalars(&responses[i][2], &axis.v[2]);
            y = FrictionProductScalars(&responses[i][1], &axis.v[1]);
            denominator = FrictionAddWide(FrictionAddWide(FrictionAddWide(x, z), y), 1.0 / FrictionRead(object, 8 + 4 * i));
            base_numerator = FrictionScalar(numerator);
            response = FrictionReverseDivideScalar(denominator, &base_numerator);
            impulse[i] = FrictionScalar(response);
            accumulated = FrictionScalar(FrictionAddScalar(FrictionProductWide(response, response), &accumulated));
        }
        if (FrictionGreater(accumulated, limit)) {
            scale = sqrt((double)limit / accumulated);
            for (i = 0; i < 3; i++) impulse[i] = FrictionScalar(scale * impulse[i]);
        }
        for (i = 0; i < 3; i++) {
            response = FrictionAddScalar(FrictionDivideScalars(&impulse[i], FrictionAt(object, 8 + 4 * i)), FrictionAt(object, 0x74 + 4 * i));
            negative = -(double)impulse[i];
            responses[i][0] = FrictionScalar(FrictionProductWide(responses[i][0], negative));
            FrictionWrite(object, 0x74 + 4 * i, response);
            parent = FrictionPointer(object, 0x230);
            responses[i][1] = FrictionScalar(FrictionProductWide(responses[i][1], negative));
            response = FrictionProductWide(negative, responses[i][2]);
            responses[i][2] = FrictionScalar(response);
            FrictionWrite(parent, 0x74, FrictionAddScalar((double)responses[i][0], FrictionAt(parent, 0x74)));
            parent = FrictionPointer(object, 0x230);
            FrictionWrite(parent, 0x78, FrictionAddScalar((double)responses[i][1], FrictionAt(parent, 0x78)));
            parent = FrictionPointer(object, 0x230);
            FrictionWrite(parent, 0x7c, FrictionAddScalar(response, FrictionAt(parent, 0x7c)));
        }
    }
    joint = FrictionPointer(object, 0x18c);
    if (joint != NULL && FrictionNonZero(FrictionRead(joint, 4))) {
        scale = FrictionRead(joint, 4) * (double)0.03999999910593033f;
        wide_limit = scale * scale;
        sum = 0.0;
        for (i = 0; i < 3; i++) {
            response = FrictionProductScalars(FrictionAt(object, 0x74 + 4 * i), FrictionAt(object, 8 + 4 * i));
            sum = FrictionAddWide(sum, FrictionProductWide(response, response));
            impulse[i] = FrictionScalar(response);
        }
        if (FrictionGreater(sum, wide_limit)) {
            scale = sqrt(wide_limit / sum);
            for (i = 0; i < 3; i++) impulse[i] = FrictionScalar(scale * impulse[i]);
        }
        for (i = 0; i < 3; i++) {
            response = FrictionDivideScalars(&impulse[i], FrictionAt(object, 8 + 4 * i));
            FrictionWrite(object, 0x74 + 4 * i, FrictionReverseSubtractScalar(response, FrictionAt(object, 0x74 + 4 * i)));
        }
    }
    child = FrictionPointer(object, 0x224);
    while (child != NULL) {
        if (*((volatile const tU8*)child + 0xec) == 0) DoJointFriction(child);
        child = FrictionPointer(child, 0x220);
    }
}

static br_scalar* C2_HOOK_CDECL RestScalar(tPhysics_object* object, unsigned offset) {
    return (br_scalar*)((tU8*)object + offset);
}
static tU8 C2_HOOK_CDECL RestByte(tPhysics_object* object, unsigned offset) {
    return *((volatile tU8*)object + offset);
}
static void C2_HOOK_CDECL RestSetByte(tPhysics_object* object, unsigned offset, tU8 value) {
    *((volatile tU8*)object + offset) = value;
}

/* Complete original post-friction rest-state pass. Numeric name retained. */
// FUNCTION: CARMA2_HW 0x004baa00
int C2_HOOK_FASTCALL FUN_004baa00(tPhysics_object* objects) {
    tPhysics_object* object = objects;
    tPhysics_object* child;
    br_scalar angular_x, angular_z, direction[3], gravity_length;
    br_scalar downward_speed, previous_downward_speed, base[3];
    br_scalar corner_x, corner_y, corner_z, point_y, point_z;
    double x, y, z, angular_energy, linear_energy, gravity_energy;
    double point_energy, speed_change, change_limit;
    unsigned flags, corner;
    int all_at_rest = 1, child_at_rest;
    tU8 at_rest;

    while (object != NULL) {
        if (*(volatile tU32*)RestScalar(object, 0x21c) != 0x3f800000u) {
            object->I.v[0] = FrictionScalar(FrictionDivideScalars(&object->I.v[0], RestScalar(object, 0x21c)));
            object->I.v[1] = FrictionScalar(FrictionDivideScalars(&object->I.v[1], RestScalar(object, 0x21c)));
            object->I.v[2] = FrictionScalar(FrictionDivideScalars(&object->I.v[2], RestScalar(object, 0x21c)));
        }
        if (object->field_0xf0 != 0) {
            flags = (unsigned)object->flags;
            object->field_0xf0 = 0;
            object->disable_move_rotate = (tU8)((flags >> 5) & 1);
        }
        flags = (unsigned)object->flags;
        object->flags = (int)((flags & 0xfffffeffu) | ((flags >> 1) & 0x100u));
        at_rest = 0;
        if (object->disable_move_rotate == 0) {
            if (RestByte(object, 0x1df) != 0 && (tU32)object->field183_0x1d8 == (tU32)(uintptr_t)object) {
                RestSetByte(object, 0x1df, 0);
            }
            x = (double)*RestScalar(object, 0x1cc) + object->omega.v[0];
            y = (double)*RestScalar(object, 0x1d0) + object->omega.v[1];
            z = (double)*RestScalar(object, 0x1d4) + object->omega.v[2];
            angular_x = FrictionScalar(x);
            angular_z = FrictionScalar(z);
            angular_energy = ((y * y) * object->I.v[1] + (x * angular_x) * object->I.v[0]) + (z * angular_z) * object->I.v[2];
            angular_energy /= object->M;
            if (RestByte(object, 0x1dd) == 0 && RestByte(object, 0x1df) != 0) {
                linear_energy = 0.0;
            } else {
                x = object->v.v[0]; y = object->v.v[1]; z = object->v.v[2];
                linear_energy = (y * y + x * x) + z * z;
            }
            if (!(linear_energy >= 0.001) && !(angular_energy >= 8.999999999999999e-05)) {
                x = object->field_0x54.v[0]; y = object->field_0x54.v[1]; z = object->field_0x54.v[2];
                gravity_energy = (y * y + x * x) + z * z;
                gravity_length = FrictionScalar(sqrt(gravity_energy));
                direction[0] = FrictionScalar((double)object->field_0x54.v[0] / gravity_length);
                direction[1] = FrictionScalar((double)object->field_0x54.v[1] / gravity_length);
                direction[2] = FrictionScalar((double)object->field_0x54.v[2] / gravity_length);
                downward_speed = FrictionScalar(-(((double)object->v.v[1] * direction[1] + (double)object->v.v[0] * direction[0]) + (double)object->v.v[2] * direction[2]));
                previous_downward_speed = FrictionScalar(-(((double)*RestScalar(object, 0x1b8) * direction[1] + (double)*RestScalar(object, 0x1b4) * direction[0]) + (double)*RestScalar(object, 0x1bc) * direction[2]));
                if (RestByte(object, 0x1df) == 0) {
                    speed_change = (double)downward_speed - previous_downward_speed;
                    change_limit = (double)gravity_length * 0.9f;
                    if (downward_speed >= 0.012f || change_limit >= speed_change) goto update_transform;
                    base[0] = FrictionScalar(((double)object->omega.v[1] * object->cmpos.v[2] - (double)object->omega.v[2] * object->cmpos.v[1]) + object->velocity_car_space.v[0]);
                    base[1] = FrictionScalar(((double)object->omega.v[2] * object->cmpos.v[0] - (double)object->cmpos.v[2] * object->omega.v[0]) + object->velocity_car_space.v[1]);
                    base[2] = FrictionScalar(((double)object->cmpos.v[1] * object->omega.v[0] - (double)object->omega.v[1] * object->cmpos.v[0]) + object->velocity_car_space.v[2]);
                    for (corner = 0; corner < 8; corner++) {
                        corner_x = (corner & 2) ? object->bb1.min.v[0] : object->bb1.max.v[0];
                        corner_y = (corner & 1) ? object->bb1.min.v[1] : object->bb1.max.v[1];
                        corner_z = (corner & 4) ? object->bb1.min.v[2] : object->bb1.max.v[2];
                        x = ((double)object->omega.v[1] * corner_z - (double)object->omega.v[2] * corner_y) + base[0];
                        y = ((double)object->omega.v[2] * corner_x - (double)object->omega.v[0] * corner_z) + base[1];
                        z = ((double)object->omega.v[0] * corner_y - (double)object->omega.v[1] * corner_x) + base[2];
                        point_y = FrictionScalar(y); point_z = FrictionScalar(z);
                        point_energy = (x * x + y * point_y) + z * point_z;
                        if (point_energy > 0.0004f) goto update_transform;
                    }
                }
                if (object->physics_joint2 == NULL || RestByte(object, 0x1dd) != 0 || object->transform_matrix.m[1][1] > 0.999) {
                    at_rest = 1;
                    BrVector3SetFloat(&object->v, 0, 0, 0);
                    BrVector3SetFloat(&object->omega, 0, 0, 0);
                    BrVector3SetFloat(&object->rotate_omega, 0, 0, 0);
                }
            }
        }
update_transform:
        RestSetByte(object, 0x1df, at_rest);
        BrMatrix34Copy(&object->transform_matrix, &object->actor->t.t.mat);
        BrMatrix34ApplyP(&object->pos, &object->cmpos, &object->transform_matrix);
        child = object->child;
        if (child == NULL) {
            RestSetByte(object, 0x1df, (tU8)(RestByte(object, 0x1df) * 3));
        } else {
            child_at_rest = FUN_004baa00(child);
            if (RestByte(object, 0x1df) != 0) RestSetByte(object, 0x1df, (tU8)(child_at_rest * 2 + RestByte(object, 0x1df)));
        }
        all_at_rest &= (RestByte(object, 0x1df) >> 1) & 1;
        object = object->next;
    }
    return all_at_rest;
}

// ApplyForces


// ListDoJointFriction


// RotateObjectSecondOrder

/* PRE-translation uses the original scalar spill before negation. */
static br_scalar C2_HOOK_CDECL RotationArgument(br_scalar value) {
    return (br_scalar)PhysicsJointScalar((double)value);
}

// FUNCTION: CARMA2_HW 0x004c1f10
void C2_HOOK_FAKE_THISCALL RotateObjectFirstOrder(tPhysics_object* pObject, undefined4 pUnused, br_scalar pTime) {
    br_matrix34 rotation;
    br_vector3 axis, momentum, rotated;
    double x = pObject->omega.v[0], y = pObject->omega.v[1], z = pObject->omega.v[2];
    double length = sqrt((x * x + y * y) + z * z);
    double angle_wide = length * pTime;
    br_scalar angle = (br_scalar)PhysicsJointScalar(angle_wide);
    br_angle integer_angle;

    (void)pUnused;
    if (PhysicsWallNaN(angle_wide) || angle_wide < 0.0001) {
        return;
    }
    axis.v[0] = (br_scalar)PhysicsJointScalar((double)pObject->omega.v[0] / length);
    axis.v[1] = (br_scalar)PhysicsJointScalar((double)pObject->omega.v[1] / length);
    axis.v[2] = (br_scalar)PhysicsJointScalar((double)pObject->omega.v[2] / length);
    momentum.v[0] = (br_scalar)PhysicsJointScalar((double)pObject->I.v[0] * pObject->omega.v[0]);
    momentum.v[1] = (br_scalar)PhysicsJointScalar((double)pObject->omega.v[1] * pObject->I.v[1]);
    momentum.v[2] = (br_scalar)PhysicsJointScalar((double)pObject->omega.v[2] * pObject->I.v[2]);
    integer_angle = (br_angle)(br_int_64)((double)angle * 10430.378350470453);
    BrMatrix34Rotate(&rotation, integer_angle, &axis);
    BrMatrix34TApplyV(&rotated, &momentum, &rotation);
    BrMatrix34PreTranslate(&rotation, -RotationArgument(pObject->cmpos.v[0]), -RotationArgument(pObject->cmpos.v[1]), -RotationArgument(pObject->cmpos.v[2]));
    BrMatrix34PostTranslate(&rotation, pObject->cmpos.v[0], pObject->cmpos.v[1], pObject->cmpos.v[2]);
    BrMatrix34Pre(&pObject->actor->t.t.mat, &rotation);
    pObject->omega.v[0] = (br_scalar)PhysicsJointScalar((double)rotated.v[0] / pObject->I.v[0]);
    pObject->omega.v[1] = (br_scalar)PhysicsJointScalar((double)rotated.v[1] / pObject->I.v[1]);
    pObject->omega.v[2] = (br_scalar)PhysicsJointScalar((double)rotated.v[2] / pObject->I.v[2]);
}

// FUNCTION: CARMA2_HW 0x004c1be0
void C2_HOOK_FAKE_THISCALL RotateObject(tPhysics_object* pObject, undefined4 pUnused, br_scalar pTime) {
    br_matrix34 rotation;
    br_vector3 axis, momentum, rotated;
    tU16 packed[4];
    tU32 translation[3];
    double x = pObject->omega.v[0], y = pObject->omega.v[1], z = pObject->omega.v[2];
    br_scalar amount = (br_scalar)PhysicsJointScalar(((x * x + y * y) + z * z) * ((double)pTime * pTime));
    int count, iteration;

    (void)pUnused;
    memcpy(&pObject->rotate_omega, &pObject->omega, sizeof(pObject->rotate_omega));
    if (PhysicsWallNaN(amount) || amount < 0.0000001) {
        return;
    }
    if (amount <= 0.00800000037997961f) {
        RotateObjectFirstOrder(pObject, 0, pTime);
    } else {
        count = (int)(br_int_64)sqrt((double)amount * 31.249998092651367f);
        count = (int)((tU32)count + 1);
        pTime = (br_scalar)PhysicsJointScalar((double)pTime / count);
        for (iteration = 0; iteration < count && iteration < 400; iteration++) {
            double length, next_x, next_y, next_z;
            br_scalar next_y_scalar, next_z_scalar, next_length;
            br_angle integer_angle;

            x = pObject->omega.v[0]; y = pObject->omega.v[1]; z = pObject->omega.v[2];
            length = PhysicsJointScalar(sqrt((x * x + y * y) + z * z));
            axis.v[0] = (br_scalar)PhysicsJointScalar((double)pObject->omega.v[0] / length);
            axis.v[1] = (br_scalar)PhysicsJointScalar((double)pObject->omega.v[1] / length);
            axis.v[2] = (br_scalar)PhysicsJointScalar((double)pObject->omega.v[2] / length);
            momentum.v[0] = (br_scalar)PhysicsJointScalar((double)pObject->omega.v[0] * pObject->I.v[0]);
            momentum.v[1] = (br_scalar)PhysicsJointScalar((double)pObject->omega.v[1] * pObject->I.v[1]);
            momentum.v[2] = (br_scalar)PhysicsJointScalar((double)pObject->I.v[2] * pObject->omega.v[2]);
            integer_angle = (br_angle)((tU16)(br_int_64)((length * pTime) * 10430.378350470453) >> 1);
            BrMatrix34Rotate(&rotation, integer_angle, &axis);
            BrMatrix34TApplyV(&rotated, &momentum, &rotation);
            next_x = (double)rotated.v[0] / pObject->I.v[0];
            next_y = (double)rotated.v[1] / pObject->I.v[1];
            next_z = (double)rotated.v[2] / pObject->I.v[2];
            next_y_scalar = (br_scalar)PhysicsJointScalar(next_y);
            next_z_scalar = (br_scalar)PhysicsJointScalar(next_z);
            next_length = (br_scalar)PhysicsJointScalar(sqrt((next_x * next_x + (double)next_y_scalar * next_y_scalar) + (double)next_z_scalar * next_z_scalar));
            axis.v[0] = (br_scalar)PhysicsJointScalar(next_x / next_length);
            axis.v[1] = (br_scalar)PhysicsJointScalar((double)next_y_scalar / next_length);
            axis.v[2] = (br_scalar)PhysicsJointScalar((double)next_z_scalar / next_length);
            integer_angle = (br_angle)(br_int_64)(((double)next_length * pTime) * 10430.378350470453);
            BrMatrix34Rotate(&rotation, integer_angle, &axis);
            BrMatrix34PreTranslate(&rotation, -RotationArgument(pObject->cmpos.v[0]), -RotationArgument(pObject->cmpos.v[1]), -RotationArgument(pObject->cmpos.v[2]));
            BrMatrix34PostTranslate(&rotation, pObject->cmpos.v[0], pObject->cmpos.v[1], pObject->cmpos.v[2]);
            BrMatrix34Pre(&pObject->actor->t.t.mat, &rotation);
            BrMatrix34TApplyV(&rotated, &momentum, &rotation);
            pObject->omega.v[0] = (br_scalar)PhysicsJointScalar((double)rotated.v[0] / pObject->I.v[0]);
            pObject->omega.v[1] = (br_scalar)PhysicsJointScalar((double)rotated.v[1] / pObject->I.v[1]);
            pObject->omega.v[2] = (br_scalar)PhysicsJointScalar((double)rotated.v[2] / pObject->I.v[2]);
        }
    }
    if (gNet_mode != 0) {
        GetOrientationFromMatrix(packed, &pObject->actor->t.t.mat);
        memcpy(translation, &pObject->actor->t.t.mat.m[3], sizeof(translation));
        GetMatrixFromOrientation(&pObject->actor->t.t.mat, packed);
        memcpy(&pObject->actor->t.t.mat.m[3][0], &translation[0], 4);
        memcpy(&pObject->actor->t.t.mat.m[3][1], &translation[1], 4);
        memcpy(&pObject->actor->t.t.mat.m[3][2], &translation[2], 4);
    }
}

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

/* The original diagnostic entry is a single RET. */
// FUNCTION: CARMA2_HW 0x004c61e0
void C2_HOOK_FASTCALL FUN_004c61e0(const char* ignored) {
    (void)ignored;
}

static int C2_HOOK_CDECL RQSolveSeen(const tU32 history[][2], int count, const tU32* mask) {
    int i;
    for (i = 0; i < count; i++) {
        if (history[i][0] == mask[0] && history[i][1] == mask[1]) return 1;
    }
    return 0;
}

static void C2_HOOK_CDECL RQSolveToggle(tU32* result, const tU32* mask, int index) {
    result[0] = mask[0];
    result[1] = mask[1];
    if (index >= 0) result[index >> 5] ^= (tU32)1 << (index & 31);
}

// FUNCTION: CARMA2_HW 0x004c3b60
int C2_HOOK_FASTCALL FUN_004c3b60(br_scalar* original, br_scalar* rhs, br_scalar* result, int size, int unused1, unsigned char* excluded, int* excluded_count, int unconstrained, int unused2, const void* const* cached_groups, int group_count) {
    unsigned char indices[100];
    br_scalar forward[100], active_rhs[100], temporary[100], forces[100];
    tU32 history[60][2], mask[2], test_mask[2];
    br_scalar* matrix;
    br_scalar* orthogonal;
    br_scalar maximum, scaled, tail_limit;
    double sum, value, residual, best;
    int active, rebuilt, iteration, removed, negative;
    int i, j, k, last, chosen, boundary, bit, repeated, refine;
    unsigned char selected;

    /* Original entry S reserves 0x8e4 and saves four registers.
     * Its negative slot at final ESP+0x2c is S-0x8c8, while &result
     * is S+4. Capture before C locals/dependency calls can reuse it.
     * Its first nonsingular pass initializes the slot. Earlier singular
     * passes can reach the iteration limit using the incoming stack value.
     * In CDECL builds two register arguments become stack arguments; the
     * original slot is therefore 0x8c0 below the incoming return address.
     */
#if defined(_MSC_VER) && _MSC_VER >= 1300 && defined(_M_IX86)
    void* entry_return_slot = _AddressOfReturnAddress();
    __asm {
        mov eax, entry_return_slot
#if defined(REC2_MATCHING)
        mov eax, dword ptr [eax - 08c8h]
#else
        mov eax, dword ptr [eax - 08c0h]
#endif
        mov negative, eax
    }
#elif defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        lea eax, result
        mov eax, dword ptr [eax - 08cch]
        mov negative, eax
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile("movl -2252(%1), %0"
        : "=r" (negative) : "r" (&result) : "memory");
#else
    /* Other architectures have no original x86 stack-history contract. */
    negative = -1;
#endif

    (void)unused1;
    (void)unused2;
    removed = -1;
    *excluded_count = 0;
    if (size == 1) {
        if (!PhysicsWallNaN(original[0]) && original[0] > 0.0001) {
            result[0] = RQDivideScalars(&rhs[0], &original[0]);
        } else {
            result[0] = 0.0f;
        }
        return 0;
    }
    for (i = 0; i < size; i++) indices[i] = (unsigned char)i;
    active = unconstrained + 1;
    rebuilt = active;
    *excluded_count = size - unconstrained - 1;
    for (i = 0; i < *(volatile int*)excluded_count; i++) excluded[i] = (unsigned char)(i + unconstrained + 1);
    RQFromCachedGroups(gPhysics_buffer1_ptr, gPhysics_buffer2_ptr, original,
        active_rhs, rhs, indices, active, cached_groups, group_count);
    mask[0] = 1;
    mask[1] = 0;
    iteration = 0;
    /* Keep captured history until the first nonsingular pass. */
    for (;;) {
        if (iteration >= 50 && negative == -1) {
            FUN_004c61e0("Matrix solve loop overflow");
            return -1;
        }
        history[iteration][0] = mask[0];
        history[iteration][1] = mask[1];
        matrix = (br_scalar*)gPhysics_buffer1_ptr;
        forward[0] = RQDivideScalars(&active_rhs[0], &matrix[0]);
        for (i = 1; i < active; i++) {
            sum = 0.0;
            for (j = 0; j < i; j++) sum = RQAccumulate(sum, &forward[j], &matrix[i * 100 + j]);
            forward[i] = RQScalar(PartnerSubtractWide((double)active_rhs[i], sum) / matrix[i * 101]);
        }
        last = active - 1;
        matrix = (br_scalar*)gPhysics_buffer1_ptr;
        tail_limit = RQScalar(fabs((double)matrix[last * 101] * 100000.0f));
        for (j = 0; j < last; j++) {
            value = fabs((double)matrix[last * 100 + j]);
            if (!PhysicsWallNaN(value) && !PhysicsWallNaN(tail_limit) && value > tail_limit) break;
        }
        if (j != last) {
            orthogonal = (br_scalar*)gPhysics_buffer2_ptr;
            chosen = last;
            maximum = 0.0f;
            for (i = unconstrained; i < last; i++) {
                value = fabs((double)orthogonal[last * 100 + i]);
                if (!PhysicsWallNaN(value) && value > maximum) {
                    maximum = RQScalar(value);
                    chosen = i;
                }
            }
            removed = unconstrained;
            if (chosen != last) {
                RQSolveToggle(test_mask, history[iteration], indices[chosen] - unconstrained);
                if (RQSolveSeen(history, iteration, test_mask)) {
                    best = 0.0;
                    for (i = unconstrained; i < last; i++) {
                        value = fabs((double)((br_scalar*)gPhysics_buffer2_ptr)[last * 100 + i]);
                        if (!PhysicsWallNaN(value) && value > best) {
                            RQSolveToggle(test_mask, history[iteration], indices[i] - unconstrained);
                            if (!RQSolveSeen(history, iteration, test_mask)) {
                                best = RQScalar(value);
                                chosen = i;
                            }
                        }
                    }
                }
                if (chosen != last) removed = chosen;
            }
            excluded[*excluded_count] = indices[removed];
            bit = indices[removed] - unconstrained;
            mask[bit >> 5] &= ~((tU32)1 << (bit & 31));
            *excluded_count = *excluded_count + 1;
            for (i = removed; i < last; i++) indices[i] = indices[i + 1];
            active = last;
            rebuilt = last;
            RQFromCachedGroups(gPhysics_buffer1_ptr, gPhysics_buffer2_ptr, original,
                active_rhs, rhs, indices, active, cached_groups, group_count);
        } else {
            orthogonal = (br_scalar*)gPhysics_buffer2_ptr;
            for (i = 0; i < active; i++) {
                forces[i] = 0.0f;
                sum = forces[i];
                for (j = 0; j < active; j++) sum = RQAccumulate(sum, &orthogonal[j * 100 + i], &forward[j]);
                forces[i] = RQScalar(sum);
            }
            negative = -1;
            if (removed >= 0) {
                removed = -1;
                selected = excluded[*excluded_count - 1];
                if (PhysicsWallNaN(rhs[selected]) || rhs[selected] == 0.0f) rhs[selected] = -10000.0f;
            }
            best = -0.0001f;
            for (i = unconstrained; i < active; i++) {
                if (!PhysicsWallNaN(forces[i]) && forces[i] < best) {
                    best = forces[i];
                    negative = i;
                }
            }
            if (negative >= 0) {
                if (PhysicsWallNaN(forces[last]) || forces[last] < 0.0f) boundary = 0;
                else {
                    boundary = last;
                    for (; boundary >= rebuilt; boundary--) {
                        if (!PhysicsWallNaN(forces[boundary]) && forces[boundary] > 0.0f) {
                            boundary--;
                            break;
                        }
                    }
                    boundary++;
                }
                if (negative >= boundary) {
                    maximum = 0.0f;
                    negative = unconstrained;
                    for (i = unconstrained; i < active; i++) {
                        matrix = (br_scalar*)gPhysics_buffer1_ptr;
                        orthogonal = (br_scalar*)gPhysics_buffer2_ptr;
                        for (j = last; j >= i; j--) {
                            sum = 0.0;
                            for (k = last; k > j; k--) sum = RQAccumulate(sum, &temporary[k], &matrix[k * 100 + j]);
                            temporary[j] = RQScalar(PartnerSubtractWide((double)orthogonal[j * 100 + i], sum) / matrix[j * 101]);
                        }
                        memcpy(&forward[i], &temporary[i], 4);
                    }
                    for (i = unconstrained; i < active; i++) {
                        value = (double)forces[i] / forward[i];
                        if ((PhysicsWallNaN(value) || value < 0.0f) && !PhysicsWallNaN(forward[i]) && fabs((double)forward[i]) > maximum) {
                            maximum = RQScalar(fabs((double)forward[i]));
                            negative = i;
                        }
                    }
                }
                excluded[*excluded_count] = indices[negative];
                bit = indices[negative] - unconstrained;
                indices[negative] = 255;
                mask[bit >> 5] &= ~((tU32)1 << (bit & 31));
                *excluded_count = *excluded_count + 1;
                rebuilt = 0;
                for (i = 0; i < active; i++) {
                    if (indices[i] != 255) {
                        indices[rebuilt] = indices[i];
                        memcpy(&active_rhs[rebuilt], &active_rhs[i], 4);
                        rebuilt++;
                    }
                }
                active = rebuilt;
                RQFromCachedGroups(gPhysics_buffer1_ptr, gPhysics_buffer2_ptr, original,
                    active_rhs, rhs, indices, active, cached_groups, group_count);
            } else {
                for (i = 0; i < size; i++) result[i] = 0.0f;
                for (i = 0; i < active; i++) memcpy(&result[indices[i]], &forces[i], 4);
                repeated = RQSolveSeen(history, iteration, history[iteration]);
                chosen = -1;
                maximum = 0.0f;
                for (i = 0; i < *(volatile int*)excluded_count; i++) {
                    sum = 0.0;
                    selected = excluded[i];
                    for (j = 0; j < size; j++) sum = RQAccumulate(sum, &original[selected * 100 + j], &result[j]);
                    selected = excluded[i];
                    residual = sum - rhs[selected];
                    scaled = RQScalar(residual / original[selected * 101]);
                    if ((PhysicsWallNaN(residual) || residual < -0.001) && (PhysicsWallNaN(scaled) || PhysicsWallNaN(maximum) || scaled < maximum)) {
                        RQSolveToggle(test_mask, history[iteration], selected - unconstrained);
                        if (repeated && RQSolveSeen(history, iteration, test_mask)) {
                            if (chosen == -1) chosen = i;
                        } else {
                            maximum = scaled;
                            chosen = i;
                        }
                    }
                }
                if (chosen < 0) {
                    refine = 0;
                    for (i = 0; i < size; i++) {
                        temporary[i] = 0.0f;
                        sum = temporary[i];
                        for (j = 0; j < size; j++) sum = RQAccumulate(sum, &original[i * 100 + j], &result[j]);
                        temporary[i] = RQScalar(sum);
                        residual = (double)temporary[i] - rhs[i];
                        if ((!PhysicsWallNaN(result[i]) && result[i] != 0.0f && !PhysicsWallNaN(residual) && fabs(residual) > 0.01) || PhysicsWallNaN(residual) || residual < -0.01) refine = 1;
                    }
                    if (refine) {
                        for (i = 0; i < active; i++) {
                            sum = -(double)active_rhs[i];
                            for (j = 0; j < active; j++) sum = RQAccumulate(sum, &original[indices[i] * 100 + indices[j]], &forces[j]);
                            temporary[i] = RQScalar(sum);
                        }
                        matrix = (br_scalar*)gPhysics_buffer1_ptr;
                        forward[0] = RQDivideScalars(&temporary[0], &matrix[0]);
                        for (i = 1; i < active; i++) {
                            sum = 0.0;
                            for (j = 0; j < i; j++) sum = RQAccumulate(sum, &forward[j], &matrix[i * 100 + j]);
                            forward[i] = RQScalar(PartnerSubtractWide((double)temporary[i], sum) / matrix[i * 101]);
                        }
                        orthogonal = (br_scalar*)gPhysics_buffer2_ptr;
                        for (i = 0; i < active; i++) {
                            for (j = 0; j < active; j++) {
                                forces[i] = RQScalar((double)orthogonal[j * 100 + i] * forward[j] + forces[i]);
                            }
                        }
                    }
                    /* Original refinement changes only private forces. */
                    return 0;
                }
                selected = excluded[chosen];
                for (i = chosen + 1; i < *(volatile int*)excluded_count; i++) excluded[i - 1] = excluded[i];
                *excluded_count = *excluded_count - 1;
                excluded[*excluded_count] = selected;
                selected = excluded[*excluded_count];
                indices[active] = selected;
                bit = selected - unconstrained;
                mask[bit >> 5] |= (tU32)1 << (bit & 31);
                matrix = (br_scalar*)gPhysics_buffer1_ptr;
                orthogonal = (br_scalar*)gPhysics_buffer2_ptr;
                if (gPhysics_rq_mode_6940b8 == 0) RQSingleExpand(matrix, original, active, orthogonal, indices);
                else RQExpand(matrix, original, active, active + 1, orthogonal, indices);
                memcpy(&active_rhs[active], &rhs[indices[active]], 4);
                active++;
            }
        }
        iteration++;
    }
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

// GLOBAL: CARMA2_HW 0x00692dc8
tU32 gPhysics_joints_changed_692dc8;

// GLOBAL: CARMA2_HW 0x00692dd8
tU32 gPhysics_group_limit_692dd8;

// FUNCTION: CARMA2_HW 0x004b7220
void C2_HOOK_FASTCALL FUN_004b7220(tPhysics_object* pObject, const br_matrix34* pMatrix) {
    BrMatrix34Copy(&pObject->transform_matrix, pMatrix);
    BrMatrix34Copy(&pObject->actor->t.t.mat, pMatrix);
}

// FUNCTION: CARMA2_HW 0x00428cf0
int C2_HOOK_FASTCALL FUN_00428cf0(tPhysics_contact* output, int count, const tPhysics_contact* input, int input_count) {
    int index, word;
    tU8* destination;
    const tU8* source = (const tU8*)input;

    if (input_count <= 0) {
        return count;
    }
    destination = (tU8*)(uintptr_t)((tU32)(uintptr_t)output + (tU32)count * 96u);
    for (index = 0; index < input_count; index++) {
        if (count >= 40) {
            return count;
        }
        if (((const tPhysics_contact*)source)->type != 9) {
            count++;
            for (word = 0; word < 24; word++) {
                PhysicsCopyWord(destination + word * 4, source + word * 4);
            }
            destination += 96;
        }
        source += 96;
    }
    return count;
}

static volatile br_scalar* C2_HOOK_CDECL LimitScalarAt(const void* base, unsigned offset) {
    return (volatile br_scalar*)((tU8*)(uintptr_t)base + offset);
}
static void C2_HOOK_CDECL LimitWord(void* base, unsigned offset, tU32 value) {
    *(volatile tU32*)((tU8*)base + offset) = value;
}
static void C2_HOOK_CDECL LimitCopyWord(void* dest, unsigned dest_offset, const void* source, unsigned source_offset) {
    tU32 value = FrictionWord(source, source_offset);
    LimitWord(dest, dest_offset, value);
}
static br_matrix34* C2_HOOK_CDECL LimitActorMatrix(const tPhysics_object* object) {
    return (br_matrix34*)((tU8*)FrictionPointer(object, 0) + 0x2c);
}
static tU8* C2_HOOK_CDECL LimitRow(const tPhysics_object* object, unsigned joint_offset, unsigned row_offset) {
    return (tU8*)FrictionPointer(object, joint_offset) + row_offset;
}
static tU8* C2_HOOK_CDECL LimitRecord(tPhysics_joint_force* records, int index) {
    return (tU8*)(uintptr_t)((tU32)(uintptr_t)records + 96u * (tU32)index);
}
/* The arguments preserve original product and accumulation ordering. */
static double C2_HOOK_CDECL LimitDot(const void* a, unsigned ax, const void* b, unsigned bx, unsigned i, unsigned j, unsigned k) {
    double first = FrictionProductScalars(LimitScalarAt(a, ax + 4*i), LimitScalarAt(b, bx + 4*i));
    double second = FrictionProductScalars(LimitScalarAt(a, ax + 4*j), LimitScalarAt(b, bx + 4*j));
    double third = FrictionProductScalars(LimitScalarAt(a, ax + 4*k), LimitScalarAt(b, bx + 4*k));
    return FrictionAddWide(FrictionAddWide(first, second), third);
}
static double C2_HOOK_CDECL LimitCross(br_vector3* out, const void* axis, unsigned offset, const br_vector3* vector) {
    double z, length;
    FrictionWrite(out, 0, PartnerSubtractWide(
        FrictionProductScalars(LimitScalarAt(axis, offset+8), LimitScalarAt(vector, 4)),
        FrictionProductScalars(LimitScalarAt(axis, offset+4), LimitScalarAt(vector, 8))));
    FrictionWrite(out, 4, PartnerSubtractWide(
        FrictionProductScalars(LimitScalarAt(axis, offset), LimitScalarAt(vector, 8)),
        FrictionProductScalars(LimitScalarAt(axis, offset+8), LimitScalarAt(vector, 0))));
    z = PartnerSubtractWide(
        FrictionProductScalars(LimitScalarAt(axis, offset+4), LimitScalarAt(vector, 0)),
        FrictionProductScalars(LimitScalarAt(axis, offset), LimitScalarAt(vector, 4)));
    FrictionWrite(out, 8, z);
    /* Original keeps z wide while its FST scalar copy feeds the last product. */
    length = FrictionAddWide(
        FrictionAddWide(FrictionProductScalars(LimitScalarAt(out, 0), LimitScalarAt(out, 0)),
                       FrictionProductScalars(LimitScalarAt(out, 4), LimitScalarAt(out, 4))),
        z * FrictionRead(out, 8));
    return sqrt(length);
}

// FUNCTION: CARMA2_HW 0x004bd4f0
void C2_HOOK_FASTCALL FUN_004bd4f0(const br_matrix34* child_matrix, const br_matrix34* parent_matrix, const br_vector3* child_axis0, const br_vector3* parent_axis0, const br_vector3* child_axis1, const br_vector3* unused_parent_axis1, br_vector3* reflected, br_vector3* bisector) {
    br_vector3 intermediate, converted;
    br_scalar stored_y, stored_z, twice_dot, product_z;
    double x, y, z, length, reciprocal, reflected_x, reflected_y, reflected_z;
    double parent_y, parent_z, dot, wide_twice_dot;
    (void)unused_parent_axis1;
    BrMatrix34ApplyV(&intermediate, child_axis0, child_matrix);
    BrMatrix34TApplyV(&converted, &intermediate, parent_matrix);
    x = FrictionAddScalar(FrictionRead(&converted, 0), LimitScalarAt(parent_axis0, 0));
    y = FrictionRead(&converted, 4);
    z = FrictionRead(&converted, 8);
    FrictionWrite(bisector, 0, x);
    y = FrictionAddScalar(y, LimitScalarAt(parent_axis0, 4));
    x = FrictionRead(bisector, 0);
    FrictionWrite(bisector, 4, y);
    z = FrictionAddScalar(z, LimitScalarAt(parent_axis0, 8));
    stored_y = FrictionScalar(FrictionRead(bisector, 4));
    stored_z = FrictionScalar(z);
    length = FrictionAddWide(FrictionAddWide((double)stored_y * stored_y,
                                             z * stored_z), x * x);
    FrictionWrite(bisector, 8, z);
    length = sqrt(length);
    if (length > (double)2.384185791015625e-7f) {
        reciprocal = 1.0 / length;
        z = FrictionRead(bisector, 8);
        y = FrictionRead(bisector, 4);
        x = FrictionRead(bisector, 0) * reciprocal;
        y *= reciprocal;
        z *= reciprocal;
        FrictionWrite(bisector, 0, x);
        FrictionWrite(bisector, 4, y);
        FrictionWrite(bisector, 8, z);
    } else {
        LimitWord(bisector, 0, 0x3f800000);
        LimitWord(bisector, 4, 0);
        LimitWord(bisector, 8, 0);
    }
    BrMatrix34ApplyV(&intermediate, child_axis1, child_matrix);
    BrMatrix34TApplyV(reflected, &intermediate, parent_matrix);
    dot = LimitDot(bisector, 0, reflected, 0, 0, 2, 1);
    twice_dot = FrictionScalar(dot + dot);
    /* The original spills each of the first reflection's products independently. */
    reflected_y = FrictionRead(reflected, 4);
    reflected_z = FrictionRead(reflected, 8);
    x = FrictionProductScalars(&twice_dot, LimitScalarAt(bisector, 0));
    y = FrictionProductScalars(LimitScalarAt(bisector, 4), &twice_dot);
    z = FrictionProductScalars(LimitScalarAt(bisector, 8), &twice_dot);
    reflected_x = FrictionReverseSubtractScalar(x, LimitScalarAt(reflected, 0));
    product_z = FrictionScalar(z);
    reflected_y = PartnerSubtractWide(reflected_y, y);
    reflected_z = FrictionSubtractScalar(reflected_z, &product_z);
    stored_y = FrictionScalar(reflected_y);
    stored_z = FrictionScalar(reflected_z);
    FrictionWrite(reflected, 0, reflected_x);
    x = FrictionRead(reflected, 0);
    FrictionWrite(reflected, 4, reflected_y);
    FrictionWrite(reflected, 8, reflected_z);
    parent_y = FrictionRead(parent_axis0, 4);
    parent_z = FrictionRead(parent_axis0, 8);
    dot = FrictionAddWide(FrictionAddWide(
        FrictionProductScalars(LimitScalarAt(reflected, 0), LimitScalarAt(parent_axis0, 0)),
        FrictionProductScalars(LimitScalarAt(parent_axis0, 4), &stored_y)),
        FrictionProductScalars(LimitScalarAt(parent_axis0, 8), &stored_z));
    wide_twice_dot = FrictionAddWide(dot, dot);
    twice_dot = FrictionScalar(wide_twice_dot);
    x = PartnerMultiplyRawScalar(wide_twice_dot, (const br_scalar*)LimitScalarAt(parent_axis0, 0));
    y = PartnerMultiplyRawScalar(parent_y, &twice_dot);
    z = PartnerMultiplyRawScalar(parent_z, &twice_dot);
    FrictionWrite(reflected, 0, FrictionReverseSubtractScalar(x, LimitScalarAt(reflected, 0)));
    reflected_z = FrictionRead(reflected, 8);
    reflected_y = FrictionRead(reflected, 4);
    FrictionWrite(reflected, 4, PartnerSubtractWide(reflected_y, y));
    FrictionWrite(reflected, 8, PartnerSubtractWide(reflected_z, z));
}

// FUNCTION: CARMA2_HW 0x004bca10
int C2_HOOK_FASTCALL FUN_004bca10(tPhysics_object* object, tPhysics_joint_force* records, int count, int capacity) {
    tPhysics_object* partner;
    tU8* joint;
    tU8* row;
    tU8* record;
    tU8* scan;
    br_matrix34* partner_matrix;
    br_vector3 temporary, current, predicted, cross;
    br_scalar current_dot, predicted_dot, reference_dot, actor_dot;
    double wide, next, length, reference, actor_position, predicted_position, threshold;
    unsigned row_offset, word_offset;
    int row_index, kind, index, remaining;
    joint = FrictionPointer(object, 0x188);
    if (joint != NULL) {
        partner = FrictionPointer(object, 0x230);
        if (partner == NULL) PhysicsError((tPhysicsError)11, &gPhysics_empty_message);
        row_index = 0;
        row_offset = 0;
        record = LimitRecord(records, count);
        while (row_index < (int)FrictionWord(FrictionPointer(object, 0x188), 0x50)) {
            if (count >= capacity) break;
            row = LimitRow(object, 0x188, row_offset);
            kind = (int)FrictionWord(row, 0x54);
            if (kind == 0) {
                BrMatrix34ApplyV(&temporary, (br_vector3*)(row+0x5c), LimitActorMatrix(object));
                BrMatrix34TApplyV(&current, &temporary, LimitActorMatrix(partner));
                row = LimitRow(object, 0x188, row_offset);
                wide = LimitDot(row, 0x68, &current, 0, 0, 1, 2);
                current_dot = FrictionScalar(wide);
                if (!(wide >= 0.001)) {
                    BrMatrix34ApplyV(&temporary, (br_vector3*)(row+0x5c), &object->transform_matrix);
                    partner_matrix = &partner->transform_matrix;
                    BrMatrix34TApplyV(&predicted, &temporary, partner_matrix);
                    row = LimitRow(object, 0x188, row_offset);
                    wide = LimitDot(row, 0x68, &predicted, 0, 0, 1, 2);
                    predicted_dot = FrictionScalar(wide);
                    if (wide >= 0.0 && (predicted_dot >= 0.001 || FrictionWord(object, 0xf0))) {
                        FrictionWrite(record, 0, FrictionSubtractScalar(current_dot, &predicted_dot));
                        LimitWord(record, 0x5c, 11);
                        LimitWord(record, 4, (tU32)(uintptr_t)object);
                        LimitWord(record, 8, (tU32)(uintptr_t)partner);
                        row = LimitRow(object, 0x188, row_offset);
                        length = LimitCross(&cross, row, 0x68, &predicted);
                        if (length > 1.0e-6) {
                            FrictionWrite(record, 0x38, FrictionRead(&cross, 0) / length);
                            FrictionWrite(record, 0x3c, FrictionRead(&cross, 4) / length);
                            FrictionWrite(record, 0x40, FrictionRead(&cross, 8) / length);
                        } else {
                            BrMatrix34ApplyV(&cross, &object->omega, &object->transform_matrix);
                            BrMatrix34TApplyV((br_vector3*)(record+0x38), &cross, partner_matrix);
                            FrictionWrite(record, 0x38, FrictionSubtractScalar(FrictionRead(record, 0x38), LimitScalarAt(partner, 0x74)));
                            FrictionWrite(record, 0x3c, FrictionSubtractScalar(FrictionRead(record, 0x3c), LimitScalarAt(partner, 0x78)));
                            next = FrictionSubtractScalar(FrictionRead(record, 0x40), LimitScalarAt(partner, 0x7c));
                            FrictionWrite(record, 0x40, next);
                            length = sqrt(FrictionAddWide(FrictionAddWide(
                                FrictionRead(record, 0x38)*FrictionRead(record, 0x38),
                                next*next), FrictionRead(record, 0x3c)*FrictionRead(record, 0x3c)));
                            if (length > (double)2.384185791015625e-7f) {
                                wide = 1.0 / length;
                                FrictionWrite(record, 0x38, FrictionRead(record, 0x38)*wide);
                                FrictionWrite(record, 0x3c, FrictionRead(record, 0x3c)*wide);
                                FrictionWrite(record, 0x40, FrictionRead(record, 0x40)*wide);
                            } else {
                                LimitWord(record, 0x38, 0x3f800000);
                                LimitWord(record, 0x3c, 0);
                                LimitWord(record, 0x40, 0);
                            }
                        }
                        BrMatrix34ApplyV(&cross, (br_vector3*)(record+0x38), partner_matrix);
                        BrMatrix34TApplyV((br_vector3*)(record+0x2c), &cross, &object->transform_matrix);
                        for (word_offset=0; word_offset<24; word_offset+=4) {
                            joint=FrictionPointer(object, 0x188);
                            LimitCopyWord(record, 0x14+word_offset, joint, 8+word_offset);
                        }
                        ++count;
                        record += 96;
                    }
                }
            } else if (kind == 1) {
                FUN_004bd4f0(LimitActorMatrix(object), LimitActorMatrix(partner),
                    (br_vector3*)(row+0x5c), (br_vector3*)(row+0x68),
                    (br_vector3*)(row+0x7c), (br_vector3*)(row+0x88), &current, &cross);
                row=LimitRow(object, 0x188, row_offset);
                wide=LimitDot(row, 0x88, &current, 0, 0, 2, 1);
                current_dot=FrictionScalar(wide);
                threshold=FrictionRead(row, 0x58)-(-0.001);
                if (wide < threshold) {
                    partner_matrix=&partner->transform_matrix;
                    FUN_004bd4f0(&object->transform_matrix, partner_matrix,
                        (br_vector3*)(row+0x5c), (br_vector3*)(row+0x68),
                        (br_vector3*)(row+0x7c), (br_vector3*)(row+0x88), &predicted, (br_vector3*)(record+0x38));
                    row=LimitRow(object, 0x188, row_offset);
                    wide=LimitDot(row, 0x88, &predicted, 0, 2, 1, 0);
                    predicted_dot=FrictionScalar(wide);
                    if (wide > FrictionRead(row, 0x58)) {
                        /* Original cross: predicted x current, stored componentwise. */
                        FrictionWrite(&cross, 0, PartnerSubtractWide(
                            FrictionProductScalars(LimitScalarAt(&predicted, 8), LimitScalarAt(&current, 4)),
                            FrictionProductScalars(LimitScalarAt(&predicted, 4), LimitScalarAt(&current, 8))));
                        FrictionWrite(&cross, 4, PartnerSubtractWide(
                            FrictionProductScalars(LimitScalarAt(&predicted, 0), LimitScalarAt(&current, 8)),
                            FrictionProductScalars(LimitScalarAt(&predicted, 8), LimitScalarAt(&current, 0))));
                        FrictionWrite(&cross, 8, PartnerSubtractWide(
                            FrictionProductScalars(LimitScalarAt(&predicted, 4), LimitScalarAt(&current, 0)),
                            FrictionProductScalars(LimitScalarAt(&predicted, 0), LimitScalarAt(&current, 4))));
                        wide=LimitDot(record, 0x38, &cross, 0, 1, 2, 0);
                        if (!(wide >= 0.0)) {
                            FrictionWrite(record, 0x38, -FrictionRead(record, 0x38));
                            FrictionWrite(record, 0x3c, -FrictionRead(record, 0x3c));
                            FrictionWrite(record, 0x40, -FrictionRead(record, 0x40));
                        }
                        BrMatrix34ApplyV(&cross, (br_vector3*)(record+0x38), partner_matrix);
                        BrMatrix34TApplyV((br_vector3*)(record+0x2c), &cross, &object->transform_matrix);
                        FrictionWrite(record, 0, FrictionSubtractScalar(current_dot, &predicted_dot));
                        LimitWord(record, 0x5c, 11);
                        LimitWord(record, 4, (tU32)(uintptr_t)object);
                        LimitWord(record, 8, (tU32)(uintptr_t)partner);
                        ++count;
                        record+=96;
                    }
                }
                ++row_index;
                row_offset+=32;
            } else PhysicsError((tPhysicsError)8, &gPhysics_empty_message);
            ++row_index;
            row_offset+=32;
        }
    }
    joint=FrictionPointer(object, 0x18c);
    row_index=0;
    row_offset=0;
    /* Only the entry guard checks NULL. Original post-write reload reads the
     * new joint count unconditionally, including the authentic alias fault. */
    if (joint != NULL) {
        while (row_index < (int)FrictionWord(joint, 0x50)) {
            row=joint+row_offset;
            kind=(int)FrictionWord(row, 0x54);
            if (kind == 0) {
                BrMatrix34ApplyV(&current, (br_vector3*)(row+0x5c), LimitActorMatrix(object));
                row=LimitRow(object, 0x18c, row_offset);
                wide=LimitDot(row, 0x68, &current, 0, 1, 0, 2);
                if (!(wide >= 0.001)) {
                    if (count == capacity) return count;
                    record=LimitRecord(records, count);
                    FrictionWrite(record, 0, wide);
                    LimitWord(record, 0x5c, 11);
                    LimitWord(record, 4, (tU32)(uintptr_t)object);
                    LimitWord(record, 8, 0);
                    row=LimitRow(object, 0x18c, row_offset);
                    length=LimitCross(&cross, row, 0x68, &current);
                    if (length > 1.0e-6) {
                        FrictionWrite(record, 0x38, FrictionRead(&cross, 0)/length);
                        FrictionWrite(record, 0x3c, FrictionRead(&cross, 4)/length);
                        FrictionWrite(record, 0x40, FrictionRead(&cross, 8)/length);
                    }
                    /* Original writes this slot without increasing count. */
                }
            } else if (kind == 10) {
                if (*(volatile tU8*)((tU8*)object+0x1e0)) {
                    gPhysics_group_limit_692dd8=1;
                    scan=(tU8*)records;
                    for (index=0; index<count; ++index,scan+=96) {
                        if (FrictionWord(scan,4)==(tU32)(uintptr_t)object) {
                            if (FrictionWord(scan,8)==0) {
                                /* Forward MOVSD copy includes the trailing sentinel. */
                                for (remaining=0;remaining<count-index;++remaining)
                                    for (word_offset=0;word_offset<96;word_offset+=4)
                                        LimitCopyWord(scan,96u*remaining+word_offset,scan,96u*(remaining+1)+word_offset);
                                --count;
                                --index;
                                scan-=96;
                                continue;
                            }
                            wide=-FrictionRead(scan,0x2c);
                            current_dot=FrictionScalar(-FrictionRead(scan,0x30));
                            predicted_dot=FrictionScalar(-FrictionRead(scan,0x34));
                            FrictionWrite(scan,0x2c,-FrictionRead(scan,0x38));
                            FrictionWrite(scan,0x30,-FrictionRead(scan,0x3c));
                            next=-FrictionRead(scan,0x40);
                            FrictionWrite(scan,0x34,next);
                            FrictionWrite(scan,0x38,wide);
                            LimitCopyWord(scan,0x3c,&current_dot,0);
                            LimitCopyWord(scan,0x40,&predicted_dot,0);
                            wide=FrictionRead(scan,0x14);
                            LimitCopyWord(&current_dot,0,scan,0x18);
                            LimitCopyWord(&predicted_dot,0,scan,0x1c);
                            LimitCopyWord(scan,0x14,scan,0x20);
                            LimitCopyWord(scan,0x18,scan,0x24);
                            LimitCopyWord(scan,0x1c,scan,0x28);
                            FrictionWrite(scan,0x20,wide);
                            LimitCopyWord(scan,0x24,&current_dot,0);
                            LimitCopyWord(scan,0x28,&predicted_dot,0);
                            LimitCopyWord(scan,4,scan,8);
                            LimitWord(scan,8,(tU32)(uintptr_t)object);
                            LimitWord(scan,0x5c,14);
                        } else if (FrictionWord(scan,8)==(tU32)(uintptr_t)object) LimitWord(scan,0x5c,14);
                    }
                }
            } else if (kind == 11) {
                BrMatrix34ApplyV(&temporary,(br_vector3*)(joint+0x38),&object->transform_matrix);
                reference=LimitDot(object,0x17c,&temporary,0,1,2,0);
                reference_dot=FrictionScalar(reference);
                row=LimitRow(object,0x18c,row_offset);
                threshold=FrictionRead(row,0x58)+reference_dot;
                current_dot=FrictionScalar(threshold);
                actor_position=LimitDot(FrictionPointer(object,0),0x50,&temporary,0,2,1,0);
                actor_dot=FrictionScalar(actor_position);
                predicted_position=LimitDot(object,0xb0,&temporary,0,0,1,2);
                predicted_dot=FrictionScalar(predicted_position);
                if (actor_position > current_dot && predicted_dot < current_dot-(-0.001)) {
                    if (count == capacity) return count;
                    record=LimitRecord(records,count);
                    FrictionWrite(record,0,(current_dot-(double)predicted_dot)/(actor_dot-(double)predicted_dot));
                    LimitWord(record,0xc,0);
                    LimitWord(record,0x5c,2);
                    LimitWord(record,4,(tU32)(uintptr_t)object);
                    LimitWord(record,8,0);
                    FrictionWrite(record,0x2c,-FrictionRead(FrictionPointer(object,0x18c),0x38));
                    FrictionWrite(record,0x30,-FrictionRead(FrictionPointer(object,0x18c),0x3c));
                    FrictionWrite(record,0x34,-FrictionRead(FrictionPointer(object,0x18c),0x40));
                    BrVector3SetFloat((br_vector3*)(record+0x14),0,0,0);
                    ++count;
                } else if (actor_dot < reference_dot-0.001 && !(predicted_dot <= reference_dot-0.002)) {
                    if (count == capacity) return count;
                    record=LimitRecord(records,count);
                    FrictionWrite(record,0,(reference_dot-(double)predicted_dot)/(actor_dot-(double)predicted_dot));
                    LimitWord(record,0xc,0);
                    LimitWord(record,0x5c,2);
                    LimitWord(record,4,(tU32)(uintptr_t)object);
                    LimitWord(record,8,0);
                    LimitCopyWord(record,0x2c,FrictionPointer(object,0x18c),0x38);
                    LimitCopyWord(record,0x30,FrictionPointer(object,0x18c),0x3c);
                    LimitCopyWord(record,0x34,FrictionPointer(object,0x18c),0x40);
                    BrVector3SetFloat((br_vector3*)(record+0x14),0,0,0);
                    ++count;
                }
            } else PhysicsError((tPhysicsError)8, &gPhysics_empty_message);
            joint=FrictionPointer(object,0x18c);
            ++row_index;
            row_offset+=32;
        }
    }
    return count;
}

// FUNCTION: CARMA2_HW 0x004bd6d0
int C2_HOOK_FASTCALL FUN_004bd6d0(tPhysics_object* list, tPhysics_joint_force* records, int count, int capacity) {
    tPhysics_object* child;
    while (list != NULL) {
        count=FUN_004bca10(list,records,count,capacity);
        child=FrictionPointer(list,0x224);
        if (child != NULL) count=FUN_004bd6d0(child,records,count,capacity);
        list=FrictionPointer(list,0x220);
    }
    return count;
}

/* Both original inline passes execute even though their accumulated energy
 * is not consumed. Keep the reads, child calls and scalar accumulation. */
static void C2_HOOK_CDECL JointedMotionEnergyPass(tPhysics_object* first) {
    volatile br_scalar total = 0.f;
    volatile br_scalar energy;
    br_scalar children;
    br_scalar omega;
    double sum, velocity, angular, inertia, mass, subtotal;
    tPhysics_object* next;
    tPhysics_object* object = first;
    int component;

    while (object != NULL) {
        sum = 0.;
        for (component = 0; component < 3; component++) {
            velocity = object->v.v[component];
            angular = object->omega.v[component];
            omega = (br_scalar)PhysicsJointScalar(angular);
            angular *= omega;
            inertia = object->I.v[component];
            velocity *= velocity;
            mass = object->M;
            sum += inertia * angular + mass * velocity;
        }
        energy = (br_scalar)PhysicsJointScalar(sum);
        children = HierarchyCalcEnergy(object->child);
        subtotal = (double)energy + total;
        next = ((volatile tPhysics_object*)object)->next;
        total = (br_scalar)PhysicsJointScalar(subtotal + children);
        object = next;
    }
}

/* Reuse the proven original raw-time forwarding rule for this parent.
 * Integration must share the existing GNU-only helper/type declarations. */
#if defined(__GNUC__) && defined(__i386__)
#define JOINTED_MOTION(function_name, object, time) do { \
    tHierarchyRawMotion volatile function = (tHierarchyRawMotion)(void*)(function_name); \
    function((object), 0, HierarchyTimeWord(&(time))); \
} while (0)
#else
#define JOINTED_MOTION(function_name, object, time) (function_name)((object), 0, (time))
#endif

// FUNCTION: CARMA2_HW 0x004b7510
void C2_HOOK_FAKE_THISCALL MoveJointedObject(tPhysics_object* pObject, undefined4 pUnused, br_scalar pTime) {
    volatile tPhysics_object* observed = pObject;
    tPhysics_joint_force* records[40];
    br_scalar velocity[100];
    br_scalar impulses[100];
    tForce_group_container* workspace;
    tPhysics_joint_force* first_record;
    br_actor* actor;
    tU32 count, bytes;
    int actual_count, i;
    double x, y, z;

    (void)pUnused;
    count = (tU32)CountJointForces(pObject, NULL);
    if (count != 0) {
        bytes = (count + 12u) * count * 8u + 28u;
        workspace = (tForce_group_container*)GetPhysicsScratchPointer((int)bytes);
        IncreasePhysicsScratchPointer((int)bytes);
        workspace->count = (int)count;
        first_record = (tPhysics_joint_force*)((tU8*)workspace + 28);
        workspace->orthogonal = (br_scalar*)((tU8*)first_record + count * 96u);
        workspace->records = first_record;
        workspace->matrix = (br_scalar*)((tU8*)workspace->orthogonal + count * count * 4u);
        actual_count = AllocateObjectWorkspace(pObject, first_record);
        memcpy((tU8*)pObject + 0x218, &workspace, sizeof(workspace));
        for (i = 0; i < actual_count; i++) {
            records[i] = first_record++;
        }
        MakeMatrix(workspace->matrix, actual_count, records, actual_count);
        MakeJointVelocitiesVector(velocity, records, actual_count);
        SparseRQDcmpV(workspace->matrix, workspace->orthogonal, velocity, impulses, actual_count);
        if (pTime == 0.f || PhysicsScalarNaN(&pTime)) {
            return;
        }
        JointedMotionEnergyPass(pObject);
        for (i = 0; i < actual_count; i++) {
            ApplySingleForce(records[i], 0, impulses[i]);
        }
        JointedMotionEnergyPass(pObject);
        if (pTime > 0.f) {
            JOINTED_MOTION(FUN_004b8ee0, pObject, pTime);
        }
        return;
    }
    if (((volatile tPhysics_object*)pObject)->child != NULL) {
        JOINTED_MOTION(FUN_004b8ee0, pObject, pTime);
        return;
    }
    x = PhysicsJointProductMemory(&pObject->v.v[0], &pTime);
    actor = observed->actor;
    y = observed->v.v[1];
    x = PhysicsJointAddScalar(x, &actor->t.t.mat.m[3][0]);
    z = observed->v.v[2];
    y = PartnerMultiplyRawScalar(y, &pTime);
    actor->t.t.mat.m[3][0] = (br_scalar)PhysicsJointScalar(x);
    actor = observed->actor;
    y = PhysicsJointAddScalar(y, &actor->t.t.mat.m[3][1]);
    z = PartnerMultiplyRawScalar(z, &pTime);
    actor->t.t.mat.m[3][1] = (br_scalar)PhysicsJointScalar(y);
    actor = observed->actor;
    actor->t.t.mat.m[3][2] = (br_scalar)PhysicsJointScalar(PhysicsJointAddScalar(z, &actor->t.t.mat.m[3][2]));
    JOINTED_MOTION(RotateObject, pObject, pTime);
}

#undef JOINTED_MOTION

/* Original clears the published container before invoking time-zero setup. */
// FUNCTION: CARMA2_HW 0x004b9750
void C2_HOOK_FASTCALL FUN_004b9750(tPhysics_object* object) {
    void* empty = NULL;
    memcpy((tU8*)object + 0x218, &empty, sizeof(empty));
#if defined(__GNUC__) && defined(__i386__)
    {
        tHierarchyRawMotion volatile function = (tHierarchyRawMotion)(void*)MoveJointedObject;
        function(object, 0, 0);
    }
#else
    MoveJointedObject(object, 0, 0.f);
#endif
    gPhysics_joints_changed_692dc8 = 1;
}
