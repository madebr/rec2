#ifndef REC2_PHYSICS_H
#define REC2_PHYSICS_H

#include "rec2_types.h"

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(tPhysicsError_cbfn*, gPhysics_error_cb);
C2_HOOK_VARIABLE_DECLARE(void*, gPhysics_buffer1_ptr);
C2_HOOK_VARIABLE_DECLARE(void*, gPhysics_buffer2_ptr);
C2_HOOK_VARIABLE_DECLARE(void*, gPhysics_buffer3_ptr);
C2_HOOK_VARIABLE_DECLARE(int, gPhysics_other_buffer_capacity);
C2_HOOK_VARIABLE_DECLARE(void*, gPhysics_other_buffer);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tU8, gPhysics_buffer, 299792);

void C2_HOOK_FASTCALL OnPhysicsError(tPhysicsError pError);

void C2_HOOK_FASTCALL SetPhysicsErrorCallback(tPhysicsError_cbfn *pError_cbfn);

void C2_HOOK_FASTCALL SetPhysicsBuffer(tU8* pBuffer, int pSize);

void C2_HOOK_FASTCALL InitPhysics(void);

tCollision_shape_box* C2_HOOK_FASTCALL AllocateBoxCollisionShape(br_uint_8 pType);

tCollision_shape_sphere* C2_HOOK_FASTCALL AllocateSphereCollisionShape(br_uint_8 pType);

tCollision_shape_polyhedron* C2_HOOK_FASTCALL AllocatePolyhedronCollisionShape(int pCount_points, br_uint_8 pType);

tCollision_shape_wireframe* C2_HOOK_FASTCALL AllocateWireFrameCollisionShape(int pcount_points, int pCount_lines, br_uint_8 pType);

void C2_HOOK_FASTCALL LoadCollisionShape(tCollision_shape** pShape, FILE* pF);

int C2_HOOK_FASTCALL ArePointsColinear(const br_vector3* pV1, const br_vector3* pV2, const br_vector3* pV3);

void C2_HOOK_FASTCALL UpdateCollisionBoundingBox(tCollision_info* pCollision_info);

void C2_HOOK_FASTCALL ProcessCollisionShape(tCollision_shape* pShape);

tPhysicsError C2_HOOK_FASTCALL ProcessTetrahedronPolyhedronCollisionShape(tCollision_shape_polyhedron_data* pPolyhedron, tPolyhedron_edge_indexes* pEdges);

int C2_HOOK_FASTCALL PolyhedronCollisionShape_AddPoint(tCollision_shape_polyhedron_data* pPolyhedron, br_vector3* pVertex, tU8 pVertex_index, tPolyhedron_edge_indexes* pEdge_to_plane_indices);

br_scalar C2_HOOK_FASTCALL ComparePolyhedronPlaneToNormal(const br_vector3* pV1, const br_vector3* pV2);

int C2_HOOK_CDECL ComparePolyhedronPointIndicesToNormal(const int* pIndex1, const int* pIndex2);

tPhysicsError C2_HOOK_FASTCALL AddPolyhedronCollisionShapePlanes(tCollision_shape_polyhedron_data* pPolyhedron);

tPhysicsError C2_HOOK_FASTCALL ProcessPolyhedronCollisionShape(tCollision_shape_polyhedron_data* pPolyhedron);

void C2_HOOK_FASTCALL CalculateBoundingBox(const br_vector3* pVertices, int pCount_vertices, br_bounds3* pBounds);

tCollision_info* C2_HOOK_FAKE_THISCALL CreateSphericalCollisionObject(br_model* pModel, undefined4 pArg2, float pWeight);

#endif //REC2_PHYSICS_H
