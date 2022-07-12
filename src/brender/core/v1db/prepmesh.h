#ifndef REC2_PREPMESH_H
#define REC2_PREPMESH_H

#include "c2_hooks.h"

#include "brender/br_types.h"

int C2_HOOK_STDCALL addEdge(br_uint_16 first, br_uint_16 last);

void C2_HOOK_STDCALL prepareEdges(v11group* group, br_model* model);

void C2_HOOK_STDCALL BrPrepareEdges(br_model* model);

int C2_HOOK_CDECL FacesCompare(const void* p1, const void* p2);

int C2_HOOK_CDECL TVCompare_XYZ(const void* p1, const void* p2);

int C2_HOOK_CDECL TVCompare_MXYZUVN(const void* p1, const void* p2);

int C2_HOOK_CDECL TVCompare_MVN(const void* p1, const void* p2);

br_fraction C2_HOOK_CDECL BrScalarToFractionClamp(br_scalar s);

void C2_HOOK_STDCALL PrepareFaceNormals(br_model* model);

void C2_HOOK_STDCALL Smoothing(br_model* model, br_scalar crease_limit, prep_vertex** start, prep_vertex** end);

void C2_HOOK_STDCALL SmoothingCreased(br_model* model, br_scalar crease_limit, prep_vertex** start, prep_vertex** end);

void C2_HOOK_STDCALL CopyVertex(v11group* group, int v, prep_vertex* src, br_model* model);

void C2_HOOK_STDCALL CopyFace(v11group* group, int f, br_face* src, br_model* model);

void C2_HOOK_STDCALL PrepareGroups(br_model* model);

void C2_HOOK_STDCALL PrepareBoundingRadius(br_model* model);

void C2_HOOK_STDCALL PrepareBoundingBox(br_model* model);

void C2_HOOK_STDCALL RegenerateFaceNormals(v11model* v11m);

void C2_HOOK_STDCALL RegenerateVertexNormals(v11model* v11m);

void C2_HOOK_CDECL BrModelUpdate(br_model* model, br_uint_16 flags);

void C2_HOOK_STDCALL BrModelClear(br_model* model);

#endif // REC2_PREPMESH_H
