#ifndef REC2_V1DBFILE_H
#define REC2_V1DBFILE_H

#include "c2_hooks.h"

#include "brender/br_types.h"

int C2_HOOK_STDCALL FopWrite_VERTICES(br_datafile* df, br_vertex* vertices, int nvertices);

int C2_HOOK_STDCALL FopRead_VERTICES(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopRead_OLD_VERTICES(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopWrite_VERTEX_UV(br_datafile* df, br_vertex* vertices, int nvertices);

int C2_HOOK_STDCALL FopRead_VERTEX_UV(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopRead_OLD_VERTICES_UV(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopRead_MATERIAL_INDEX(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopWrite_MATERIAL_INDEX(br_datafile* df, br_material** materials, int nmaterials);

int C2_HOOK_STDCALL FopRead_OLD_MATERIAL_INDEX(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopRead_FACES(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopWrite_FACES(br_datafile* df, br_face* faces, int nfaces);

int C2_HOOK_STDCALL FopRead_OLD_FACES_1(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopRead_OLD_FACES(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopWrite_FACE_MATERIAL(br_datafile* df, br_face* faces, int nfaces, br_material** mindex, int nmaterials);

int C2_HOOK_STDCALL FopRead_FACE_MATERIAL(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopRead_MODEL(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopWrite_MODEL(br_datafile* df, br_model* mp);

int C2_HOOK_STDCALL FopRead_OLD_MODEL_1(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopRead_OLD_MODEL(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopRead_PIVOT(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopRead_MATERIAL_OLD(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopRead_MATERIAL(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopWrite_MATERIAL(br_datafile* df, br_material* mp);

int C2_HOOK_STDCALL FopRead_PIXELMAP_REF(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopWrite_PIXELMAP_REF(br_datafile* df, int id, br_pixelmap* pixelmap);

int C2_HOOK_STDCALL FopWrite_ACTOR(br_datafile* df, br_actor* ap);

int C2_HOOK_STDCALL FopRead_ACTOR(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopWrite_ACTOR_MODEL(br_datafile* df, br_model* model);

int C2_HOOK_STDCALL FopRead_ACTOR_MODEL(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopWrite_ACTOR_MATERIAL(br_datafile* df, br_material* material);

int C2_HOOK_STDCALL FopRead_ACTOR_MATERIAL(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopWrite_ACTOR_TRANSFORM(br_datafile* df);

int C2_HOOK_STDCALL FopRead_ACTOR_TRANSFORM(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopWrite_ACTOR_LIGHT(br_datafile* df);

int C2_HOOK_STDCALL FopRead_ACTOR_LIGHT(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopWrite_ACTOR_CAMERA(br_datafile* df);

int C2_HOOK_STDCALL FopRead_ACTOR_CAMERA(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopWrite_ACTOR_BOUNDS(br_datafile* df);

int C2_HOOK_STDCALL FopRead_ACTOR_BOUNDS(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopWrite_ACTOR_CLIP_PLANE(br_datafile* df);

int C2_HOOK_STDCALL FopRead_ACTOR_CLIP_PLANE(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopWrite_ACTOR_ADD_CHILD(br_datafile* df);

int C2_HOOK_STDCALL FopRead_ACTOR_ADD_CHILD(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopWrite_TRANSFORM(br_datafile* df, br_transform* t);

int C2_HOOK_STDCALL FopRead_TRANSFORM(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopWrite_BOUNDS(br_datafile* df, br_bounds* bp);

int C2_HOOK_STDCALL FopRead_BOUNDS(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopWrite_PLANE(br_datafile* df, br_vector4* pp);

int C2_HOOK_STDCALL FopRead_PLANE(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopWrite_LIGHT(br_datafile* df, br_light* lp);

int C2_HOOK_STDCALL FopRead_LIGHT(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

int C2_HOOK_STDCALL FopWrite_CAMERA(br_datafile* df, br_camera* cp);

int C2_HOOK_STDCALL FopRead_CAMERA(br_datafile* df, br_uint_32 id, br_uint_32 length, br_uint_32 count);

br_uint_32 C2_HOOK_CDECL BrModelLoadMany(char* filename, br_model** models, br_uint_16 num);

int C2_HOOK_CDECL PtrCompare(void* a, void* b);

br_uint_32 C2_HOOK_CDECL WriteModel(br_model* mp, br_datafile* df);

br_uint_32 C2_HOOK_CDECL BrModelSaveMany(char* filename, br_model** models, br_uint_16 num);

br_uint_32 C2_HOOK_CDECL BrActorLoadMany(char* filename, br_actor** actors, br_uint_16 num);

int C2_HOOK_STDCALL WriteActor(br_actor* a, br_datafile* df);

br_uint_32 C2_HOOK_CDECL BrActorSaveMany(char* filename, br_actor** actors, br_uint_16 num);

br_uint_32 C2_HOOK_CDECL BrMaterialLoadMany(char* filename, br_material** materials, br_uint_16 num);

br_uint_32 C2_HOOK_STDCALL WriteMaterial(br_material* mp, br_datafile* df);

br_uint_32 C2_HOOK_CDECL BrMaterialSaveMany(char* filename, br_material** materials, br_uint_16 num);

br_model* C2_HOOK_CDECL BrModelLoad(char* filename);

br_uint_32 C2_HOOK_CDECL BrModelSave(char* filename, br_model* ptr);

br_material* C2_HOOK_CDECL BrMaterialLoad(char* filename);

br_uint_32 C2_HOOK_CDECL BrMaterialSave(char* filename, br_material* ptr);

br_actor* C2_HOOK_CDECL BrActorLoad(char* filename);

br_uint_32 C2_HOOK_CDECL BrActorSave(char* filename, br_actor* ptr);

br_error C2_HOOK_CDECL BrModelFileCount(char* filename, br_uint_16* num);

br_error C2_HOOK_CDECL BrActorFileCount(char* filename, br_uint_16* num);

br_error C2_HOOK_CDECL BrMaterialFileCount(char* filename, br_uint_16* num);

#endif // REC2_V1DBFILE_H
