#ifndef BRENDER_H
#define BRENDER_H

#include "c2_hooks.h"

#include "br_inline_funcs.h"
#include "br_types.h"

C2_HOOK_VARIABLE_DECLARE(struct br_font*, BrFontFixed3x5);
C2_HOOK_VARIABLE_DECLARE(struct br_font*, BrFontProp4x6);
C2_HOOK_VARIABLE_DECLARE(struct br_font*, BrFontProp7x9);

void C2_HOOK_CDECL BrV1dbBeginWrapper_Float(void);
br_error C2_HOOK_CDECL BrDevBeginVar(br_pixelmap** ppm, char* setup_string, ...);

// BrActor
br_actor* C2_HOOK_CDECL BrActorLoad(const char* filename);
br_uint_32 C2_HOOK_CDECL BrActorLoadMany(const char* filename, br_actor** actors, br_uint_16 num);
br_actor* C2_HOOK_CDECL BrActorAllocate(br_uint_8 type, void* type_data);
br_actor* C2_HOOK_CDECL BrActorAdd(br_actor* parent, br_actor* a);
void C2_HOOK_CDECL BrActorRelink(br_actor* parent, br_actor* a);
br_actor* C2_HOOK_CDECL BrActorRemove(br_actor* a);
void C2_HOOK_CDECL BrActorFree(br_actor* a);
br_uint_32 C2_HOOK_CDECL BrActorEnum(br_actor* parent, br_actor_enum_cbfn* callback, void* arg);
br_bounds* C2_HOOK_CDECL BrActorToBounds(br_bounds* b, br_actor* ap);
br_uint_16 C2_HOOK_CDECL BrActorToActorMatrix34(br_matrix34* m, br_actor* a, br_actor* b);

void C2_HOOK_CDECL BrLightEnable(br_actor* l);
void C2_HOOK_CDECL BrLightDisable(br_actor* l);

// BrAllocator
br_allocator* C2_HOOK_CDECL BrAllocatorSet(br_allocator* newal);

// BrClip
void C2_HOOK_CDECL BrClipPlaneEnable(br_actor* c);
void C2_HOOK_CDECL BrClipPlaneDisable(br_actor* c);

// BrDev
void C2_HOOK_CDECL BrDevPaletteSetOld(br_pixelmap* pm);
void C2_HOOK_CDECL BrDevPaletteSetEntryOld(int i, br_colour colour);

// BrEnvironment
br_actor* C2_HOOK_CDECL BrEnvironmentSet(br_actor* a);

// BrFileSystem
br_filesystem* C2_HOOK_CDECL BrFilesystemSet(br_filesystem* newfs);

// BrMap
br_pixelmap* C2_HOOK_CDECL BrMapAdd(br_pixelmap* pixelmap);
br_pixelmap* C2_HOOK_CDECL BrMapRemove(br_pixelmap* pixelmap);
br_pixelmap* C2_HOOK_CDECL BrMapFind(const char* pattern);
br_uint_32 C2_HOOK_CDECL BrMapAddMany(br_pixelmap** items, int n);
br_map_find_cbfn* C2_HOOK_CDECL BrMapFindHook(br_map_find_cbfn* hook);
void C2_HOOK_CDECL BrMapUpdate(br_pixelmap* map, br_uint_16 flags);

br_pixelmap* C2_HOOK_CDECL BrPixelmapDirectLock(br_pixelmap* src, br_boolean block);
br_pixelmap* C2_HOOK_CDECL BrPixelmapDirectUnlock(br_pixelmap* src);

// BrMaterial
br_material* C2_HOOK_CDECL BrMaterialAllocate(const char* name);
br_material* C2_HOOK_CDECL BrMaterialLoad(const char* filename);
br_uint_32 C2_HOOK_CDECL BrMaterialLoadMany(const char* filename, br_material** materials, br_uint_16 num);
br_material* C2_HOOK_CDECL BrMaterialAdd(br_material* material);
br_material* C2_HOOK_CDECL BrMaterialRemove(br_material* material);
void C2_HOOK_CDECL BrMaterialUpdate(br_material* mat, br_uint_16 flags);
br_material* C2_HOOK_CDECL BrMaterialFind(const char* pattern);
void C2_HOOK_CDECL BrMaterialFree(br_material* m);
br_uint_32 C2_HOOK_CDECL BrMaterialAddMany(br_material** items, int n);
br_uint_32 C2_HOOK_CDECL BrMaterialEnum(const char* pattern, br_material_enum_cbfn* callback, void* arg);
br_material_find_cbfn* C2_HOOK_CDECL BrMaterialFindHook(br_material_find_cbfn* hook);

// BrMatrix236
void C2_HOOK_CDECL BrMatrix23Copy(br_matrix23* A, br_matrix23* B);
void C2_HOOK_CDECL BrMatrix23Identity(br_matrix23* mat);
void C2_HOOK_CDECL BrMatrix23Mul(br_matrix23* A, br_matrix23* B, br_matrix23* C);
void C2_HOOK_CDECL BrMatrix23Rotate(br_matrix23* mat, br_angle rz);
void C2_HOOK_CDECL BrMatrix23Scale(br_matrix23* mat, br_scalar sx, br_scalar sy);

// BrMatrix34
void C2_HOOK_CDECL BrMatrix34Identity(br_matrix34* mat);
void C2_HOOK_CDECL BrMatrix34Translate(br_matrix34* mat, br_scalar dx, br_scalar dy, br_scalar dz);
void C2_HOOK_CDECL BrMatrix34Pre(br_matrix34* mat, br_matrix34* A);
void C2_HOOK_CDECL BrMatrix34Post(br_matrix34* mat, br_matrix34* A);
void C2_HOOK_CDECL BrMatrix34ApplyV(br_vector3* A, br_vector3* B, br_matrix34* C);
br_scalar C2_HOOK_CDECL BrMatrix34Inverse(br_matrix34* B, br_matrix34* A);
void C2_HOOK_CDECL BrMatrix34LPInverse(br_matrix34* A, br_matrix34* B);
void C2_HOOK_CDECL BrMatrix34ApplyP(br_vector3* A, br_vector3* B, br_matrix34* C);
void C2_HOOK_CDECL BrMatrix34Scale(br_matrix34* mat, br_scalar sx, br_scalar sy, br_scalar sz);
void C2_HOOK_CDECL BrMatrix34PostTranslate(br_matrix34* mat, br_scalar x, br_scalar y, br_scalar z);
void C2_HOOK_CDECL BrMatrix34Mul(br_matrix34* A, br_matrix34* B, br_matrix34* C);
void C2_HOOK_CDECL BrMatrix34Copy(br_matrix34* A, br_matrix34* B);
void C2_HOOK_CDECL BrMatrix34PreRotateY(br_matrix34* mat, br_angle ry);
void C2_HOOK_CDECL BrMatrix34RotateX(br_matrix34* mat, br_angle rx);
void C2_HOOK_CDECL BrMatrix34RotateY(br_matrix34* mat, br_angle ry);
void C2_HOOK_CDECL BrMatrix34PreScale(br_matrix34* mat, br_scalar sx, br_scalar sy, br_scalar sz);
void C2_HOOK_CDECL BrMatrix34PostScale(br_matrix34* mat, br_scalar sx, br_scalar sy, br_scalar sz);
void C2_HOOK_CDECL BrMatrix34PreTransform(br_matrix34* mat, br_transform* xform);
void C2_HOOK_CDECL BrMatrix34PostTransform(br_matrix34* mat, br_transform* xform);
void C2_HOOK_CDECL BrMatrix34TApplyV(br_vector3* A, br_vector3* B, br_matrix34* C);
void C2_HOOK_CDECL BrMatrix34LPNormalise(br_matrix34* A, br_matrix34* B);
void C2_HOOK_CDECL BrMatrix34PreRotate(br_matrix34* mat, br_angle r, br_vector3* axis);
void C2_HOOK_CDECL BrMatrix34Rotate(br_matrix34* mat, br_angle r, br_vector3* a);
void C2_HOOK_CDECL BrMatrix34PreTranslate(br_matrix34* mat, br_scalar x, br_scalar y, br_scalar z);
void C2_HOOK_CDECL BrMatrix34PostShearX(br_matrix34* mat, br_scalar sy, br_scalar sz);
void C2_HOOK_CDECL BrMatrix34PostShearY(br_matrix34* mat, br_scalar sx, br_scalar sz);
void C2_HOOK_CDECL BrMatrix34PostShearZ(br_matrix34* mat, br_scalar sx, br_scalar sy);
void C2_HOOK_CDECL BrMatrix34PreShearX(br_matrix34* mat, br_scalar sy, br_scalar sz);
void C2_HOOK_CDECL BrMatrix34PostRotateX(br_matrix34* mat, br_angle ry);
void C2_HOOK_CDECL BrMatrix34PostRotateY(br_matrix34* mat, br_angle ry);
void C2_HOOK_CDECL BrMatrix34PostRotateZ(br_matrix34* mat, br_angle ry);
void C2_HOOK_CDECL BrMatrix34PreRotateX(br_matrix34* mat, br_angle ry);
void C2_HOOK_CDECL BrMatrix34PreRotateY(br_matrix34* mat, br_angle ry);
void C2_HOOK_CDECL BrMatrix34PreRotateZ(br_matrix34* mat, br_angle rz);
void C2_HOOK_CDECL BrMatrix34RollingBall(br_matrix34 *mat, int dx,int dy, int radius);

// BrMatrix4
void C2_HOOK_CDECL BrMatrix4Copy(br_matrix4* A, br_matrix4* B);
br_scalar C2_HOOK_CDECL BrMatrix4Inverse(br_matrix4* A, br_matrix4* B);
void C2_HOOK_CDECL BrMatrix4TApply(br_vector4* A, br_vector4* B, br_matrix4* C);
void C2_HOOK_CDECL BrMatrix4ApplyP(br_vector4* A, br_vector3* B, br_matrix4* C);
void C2_HOOK_CDECL BrMatrix4Perspective(br_matrix4* mat, br_angle field_of_view, br_scalar aspect, br_scalar hither, br_scalar yon);
void C2_HOOK_CDECL BrMatrix4Scale(br_matrix4* mat, br_scalar sx, br_scalar sy, br_scalar sz);
void C2_HOOK_CDECL BrMatrix4Mul(br_matrix4* A, br_matrix4* B, br_matrix4* C);
void C2_HOOK_CDECL BrMatrix4Mul34(br_matrix4* A, const br_matrix34* B, const br_matrix4* C);

// BrMem
void C2_HOOK_CDECL BrMemFree(void* block);
void* C2_HOOK_CDECL BrMemAllocate(br_size_t size, br_uint_8 type);
void* C2_HOOK_CDECL BrMemCalloc(int nelems, br_size_t size, br_uint_8 type);
char* C2_HOOK_CDECL BrMemStrDup(const char* str);

// BrModel
br_model* C2_HOOK_CDECL BrModelAllocate(const char* name, int nvertices, int nfaces);
br_model* C2_HOOK_CDECL BrModelAdd(br_model* model);
br_uint_32 C2_HOOK_CDECL BrModelAddMany(br_model** items, int n);
br_model* C2_HOOK_CDECL BrModelRemove(br_model* model);
br_model* C2_HOOK_CDECL BrModelLoad(const char* filename);
br_model* C2_HOOK_CDECL BrModelFind(const char* name);
void C2_HOOK_CDECL BrModelFree(br_model* model);
void C2_HOOK_CDECL BrModelUpdate(br_model* model, br_uint_16 flags);
br_uint_32 C2_HOOK_CDECL BrModelLoadMany(const char* filename, br_model** models, br_uint_16 num);
br_model_find_cbfn* C2_HOOK_CDECL BrModelFindHook(br_model_find_cbfn* hook);

// BrPixelmap
br_pixelmap* C2_HOOK_CDECL BrPixelmapLoad(const char* filename);
br_uint_32 C2_HOOK_CDECL BrPixelmapLoadMany(const char* filename, br_pixelmap** pixelmaps, br_uint_16 num);
br_uint_32 C2_HOOK_CDECL BrPixelmapSave(const char* filename, br_pixelmap* ptr);
br_uint_32 C2_HOOK_CDECL BrPixelmapSaveMany(const char* filename, br_pixelmap** pixelmaps, br_uint_16 num);
void C2_HOOK_CDECL BrPixelmapFree(br_pixelmap* src);
br_pixelmap* C2_HOOK_CDECL BrPixelmapMatch(br_pixelmap* src, br_uint_8 match_type);
br_pixelmap* C2_HOOK_CDECL BrPixelmapAllocateSub(br_pixelmap* src, br_int_32 x, br_int_32 y, br_int_32 w, br_int_32 h);
void C2_HOOK_CDECL BrPixelmapFill(br_pixelmap* dst, br_uint_32 colour);
void C2_HOOK_CDECL BrPixelmapRectangleCopy(br_pixelmap* dst, br_int_32 dx, br_int_32 dy, br_pixelmap* src, br_int_32 sx, br_int_32 sy, br_int_32 w, br_int_32 h);
void C2_HOOK_CDECL BrPixelmapCopy(br_pixelmap* dst, br_pixelmap* src);
void C2_HOOK_CDECL BrPixelmapLine(br_pixelmap* dst, br_int_32 x1, br_int_32 y1, br_int_32 x2, br_int_32 y2, br_uint_32 colour);
void C2_HOOK_CDECL BrPixelmapRectangle(br_pixelmap* dst, br_int_32 x, br_int_32 y, br_int_32 w, br_int_32 h, br_uint_32 colour);
void C2_HOOK_CDECL BrPixelmapRectangleFill(br_pixelmap* dst, br_int_32 x, br_int_32 y, br_int_32 w, br_int_32 h, br_uint_32 colour);
br_uint_32 C2_HOOK_CDECL BrPixelmapPixelGet(br_pixelmap* dst, br_int_32 x, br_int_32 y);
void C2_HOOK_CDECL BrPixelmapPixelSet(br_pixelmap* dst, br_int_32 x, br_int_32 y, br_uint_32 colour);
br_pixelmap* C2_HOOK_CDECL BrPixelmapAllocate(br_uint_8 type, br_int_32 w, br_int_32 h, void* pixels, int flags);
void C2_HOOK_CDECL BrPixelmapDoubleBuffer(br_pixelmap* dst, br_pixelmap* src);
void C2_HOOK_CDECL BrPixelmapPaletteSet(br_pixelmap *pm, br_pixelmap *pal);
void C2_HOOK_CDECL BrPixelmapPaletteEntrySet(br_pixelmap *pm, br_int_32 i,br_colour colour);
void C2_HOOK_CDECL BrPixelmapPaletteEntrySetMany(br_pixelmap *pm, br_int_32 index , br_int_32 ncolours, br_colour *colours);
void C2_HOOK_CDECL BrPixelmapText(br_pixelmap* dst, br_int_32 x, br_int_32 y, br_uint_32 colour, br_font* font, char* text);
void C2_HOOK_CDECL BrPixelmapTextF(br_pixelmap* dst, br_int_32 x, br_int_32 y, br_uint_32 colour, br_font* font, char* fmt, ...);
br_uint_16 C2_HOOK_CDECL BrPixelmapTextWidth(br_pixelmap* dst, br_font* font, char* text);

// BrRes
void* C2_HOOK_CDECL BrResAllocate(void* vparent, br_size_t size, br_uint_8 res_class);
void* C2_HOOK_CDECL BrResAdd(void* vparent, void* vres);
br_resource_class* C2_HOOK_CDECL BrResClassAdd(br_resource_class* rclass);
void* C2_HOOK_CDECL BrResRemove(void* vres);
void C2_HOOK_CDECL BrResFree(void* vres);
char* C2_HOOK_CDECL BrResStrDup(void* vparent, const char* str);
br_uint_32 C2_HOOK_CDECL BrResSize(void* vres);
br_uint_8 C2_HOOK_CDECL BrResClass(void* vres);

// BrTable
br_pixelmap* C2_HOOK_CDECL BrTableAdd(br_pixelmap* pixelmap);
br_pixelmap* C2_HOOK_CDECL BrTableFind(const char* pattern);
br_pixelmap* C2_HOOK_CDECL BrTableRemove(br_pixelmap* pixelmap);
br_uint_32 C2_HOOK_CDECL  BrTableAddMany(br_pixelmap** items, int n);
void C2_HOOK_CDECL BrTableUpdate(br_pixelmap* table, br_uint_16 flags);
br_uint_32 C2_HOOK_CDECL BrTableEnum(const char* pattern, br_table_enum_cbfn* callback, void* arg);
br_table_find_cbfn* C2_HOOK_CDECL BrTableFindHook(br_table_find_cbfn* hook);

// BrTransform
void C2_HOOK_CDECL BrTransformToMatrix34(br_matrix34* mat, br_transform* xform);

// BrV1db
void C2_HOOK_CDECL BrV1dbBeginWrapper_Float(void);

// BrVector3
// void C2_HOOK_CDECL BrVector3Cross(br_vector3* v1, br_vector3* v2, br_vector3* v3);
void C2_HOOK_CDECL BrVector3SetFloat(br_vector3* v1, float f1, float f2, float f3);
// void C2_HOOK_CDECL BrVector3Sub(br_vector3* v1, br_vector3* v2, br_vector3* v3);
// void C2_HOOK_CDECL BrVector3Accumulate(br_vector3* v1, br_vector3* v2);
// void C2_HOOK_CDECL BrVector3Normalise(br_vector3* v1, br_vector3* v2);

// BrTransform
void C2_HOOK_CDECL BrTransformToTransform(br_transform* dest, br_transform* src);

// Logging
void C2_HOOK_CDECL BrFailure(const char* s, ...);
void C2_HOOK_CDECL BrFatal(const char* name, int line, const char* s, ...);

// Z-buffer with primitive (or so I think)
void C2_HOOK_CDECL BrZbsBegin(br_uint_8 colour_type, br_uint_8 depth_type, void* primitive, br_uint_32 size);

// Z-buffer
void C2_HOOK_CDECL BrZbBegin(br_uint_8 colour_type, br_uint_8 depth_type);

void C2_HOOK_CDECL BrZbSceneRender(br_actor* world, br_actor* camera, br_pixelmap* colour_buffer, br_pixelmap* depth_buffer);
void C2_HOOK_CDECL BrZbSceneRenderBegin(br_actor* world, br_actor* camera, br_pixelmap* colour_buffer, br_pixelmap* depth_buffer);
void C2_HOOK_CDECL BrZbSceneRenderAdd(br_actor* tree);
void C2_HOOK_CDECL BrZbSceneRenderEnd(void);

void C2_HOOK_CDECL BrZbsSceneRender(br_actor* world, br_actor* camera, br_pixelmap* colour_buffer, br_pixelmap* depth_buffer);

void C2_HOOK_CDECL BrZbModelRender(br_actor* actor, br_model* model, br_material* material, br_uint_8 style, int on_screen, int use_custom);

br_pixelmap* C2_HOOK_CDECL DOSGfxBegin(const char* setup_string);

// Various
br_uint_32 C2_HOOK_CDECL BrOnScreenCheck(br_bounds3* bounds);

int BrWriteModeSet(int mode);
br_uint_32 C2_HOOK_CDECL BrSwap32(br_uint_32 l);
br_diaghandler* C2_HOOK_CDECL BrDiagHandlerSet(br_diaghandler* newdh);

void C2_HOOK_CDECL _BrEndHook(void);
br_error C2_HOOK_CDECL BrDevFind(br_device** pdev, char* pattern);
br_error C2_HOOK_CDECL BrDevRemove(br_device* dev);

// Carmageddon 2 adaptation
void C2_HOOK_CDECL BrSetScreenZOffset(br_uint_32 pOffset);

#endif
