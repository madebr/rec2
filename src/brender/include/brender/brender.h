#ifndef BRENDER_H
#define BRENDER_H

#include "c2_hooks.h"

#include "br_inline_funcs.h"
#include "br_types.h"

extern struct br_font* BrFontFixed3x5;
extern struct br_font* BrFontProp4x6;
extern struct br_font* BrFontProp7x9;

extern br_error C2_HOOK_CDECL BrDevBeginVar(br_pixelmap** ppm, const char* setup_string, ...);

// BrActor
br_actor* C2_HOOK_CDECL BrActorLoad(const char* filename);
extern br_uint_32 C2_HOOK_CDECL BrActorLoadMany(const char* filename, br_actor** actors, br_uint_16 num);
extern br_uint_32 C2_HOOK_CDECL BrModelSaveMany(char* filename, br_model** models, br_uint_16 num);
extern br_uint_32 C2_HOOK_CDECL BrActorSave(char* filename, br_actor* ptr);
br_actor* C2_HOOK_CDECL BrActorAllocate(br_uint_8 type, void* type_data);
br_actor* C2_HOOK_CDECL BrActorAdd(br_actor* parent, br_actor* a);
extern void C2_HOOK_CDECL BrActorRelink(br_actor* parent, br_actor* a);
br_actor* C2_HOOK_CDECL BrActorRemove(br_actor* a);
extern void C2_HOOK_CDECL BrActorFree(br_actor* a);
extern br_uint_32 C2_HOOK_CDECL BrActorEnum(br_actor* parent, br_actor_enum_cbfn* callback, void* arg);
br_bounds* C2_HOOK_CDECL BrActorToBounds(br_bounds* b, br_actor* ap);
br_uint_16 C2_HOOK_CDECL BrActorToActorMatrix34(br_matrix34* m, br_actor* a, br_actor* b);

extern void C2_HOOK_CDECL BrLightEnable(br_actor* l);
extern void C2_HOOK_CDECL BrLightDisable(br_actor* l);

// BrAllocator
extern br_allocator* C2_HOOK_CDECL BrAllocatorSet(br_allocator* newal);

// BrClip
extern void C2_HOOK_CDECL BrClipPlaneEnable(br_actor* c);
extern void C2_HOOK_CDECL BrClipPlaneDisable(br_actor* c);

// BrDev
extern void C2_HOOK_CDECL BrDevPaletteSetOld(br_pixelmap* pm);
extern void C2_HOOK_CDECL BrDevPaletteSetEntryOld(int i, br_colour colour);

// BrEnvironment
br_actor* C2_HOOK_CDECL BrEnvironmentSet(br_actor* a);

// BrFileSystem
br_filesystem* C2_HOOK_CDECL BrFilesystemSet(br_filesystem* newfs);

// BrMap
extern br_pixelmap* C2_HOOK_CDECL BrMapAdd(br_pixelmap* pixelmap);
extern br_pixelmap* C2_HOOK_CDECL BrMapRemove(br_pixelmap* pixelmap);
extern br_pixelmap* C2_HOOK_CDECL BrMapFind(const char* pattern);
extern br_uint_32 C2_HOOK_CDECL BrMapAddMany(br_pixelmap** items, int n);
extern br_map_find_cbfn* C2_HOOK_CDECL BrMapFindHook(br_map_find_cbfn* hook);
extern void C2_HOOK_CDECL BrMapUpdate(br_pixelmap* map, br_uint_16 flags);

extern br_pixelmap* C2_HOOK_CDECL BrPixelmapDirectLock(br_pixelmap* src, br_boolean block);
extern br_pixelmap* C2_HOOK_CDECL BrPixelmapDirectUnlock(br_pixelmap* src);

// BrMaterial
extern br_material* C2_HOOK_CDECL BrMaterialAllocate(const char* name);
extern br_material* C2_HOOK_CDECL BrMaterialLoad(const char* filename);
extern br_uint_32 C2_HOOK_CDECL BrMaterialLoadMany(const char* filename, br_material** materials, br_uint_16 num);
extern br_material* C2_HOOK_CDECL BrMaterialAdd(br_material* material);
extern br_material* C2_HOOK_CDECL BrMaterialRemove(br_material* material);
extern void C2_HOOK_CDECL BrMaterialUpdate(br_material* mat, br_uint_16 flags);
extern br_material* C2_HOOK_CDECL BrMaterialFind(const char* pattern);
extern void C2_HOOK_CDECL BrMaterialFree(br_material* m);
extern br_uint_32 C2_HOOK_CDECL BrMaterialAddMany(br_material** items, int n);
extern br_uint_32 C2_HOOK_CDECL BrMaterialEnum(const char* pattern, br_material_enum_cbfn* callback, void* arg);
extern br_material_find_cbfn* C2_HOOK_CDECL BrMaterialFindHook(br_material_find_cbfn* hook);

// BrMatrix236
extern void C2_HOOK_CDECL BrMatrix23Copy(br_matrix23* A, br_matrix23* B);
extern void C2_HOOK_CDECL BrMatrix23Identity(br_matrix23* mat);
extern void C2_HOOK_CDECL BrMatrix23LPNormalise(br_matrix23* A, const br_matrix23* B);
extern void C2_HOOK_CDECL BrMatrix23Mul(br_matrix23* A, br_matrix23* B, br_matrix23* C);
extern void C2_HOOK_CDECL BrMatrix23Rotate(br_matrix23* mat, br_angle rz);
extern void C2_HOOK_CDECL BrMatrix23Scale(br_matrix23* mat, br_scalar sx, br_scalar sy);
extern void C2_HOOK_CDECL BrMatrix23PreTranslate(br_matrix23* mat, br_scalar x, br_scalar y);
extern void C2_HOOK_CDECL BrMatrix23PostTranslate(br_matrix23* A, br_scalar x, br_scalar y);
extern void C2_HOOK_CDECL BrMatrix23PreScale(br_matrix23* mat, br_scalar sx, br_scalar sy);
extern void C2_HOOK_CDECL BrMatrix23PostScale(br_matrix23* mat, br_scalar sx, br_scalar sy);
extern void C2_HOOK_CDECL BrMatrix23PreShearX(br_matrix23* mat, br_scalar sy);
extern void C2_HOOK_CDECL BrMatrix23PostShearX(br_matrix23* mat, br_scalar sy);
extern void C2_HOOK_CDECL BrMatrix23PreShearY(br_matrix23* mat, br_scalar sx);
extern void C2_HOOK_CDECL BrMatrix23PostShearY(br_matrix23* mat, br_scalar sx);
extern void C2_HOOK_CDECL BrMatrix23TApplyV(br_vector2* A, const br_vector2* B, const br_matrix23* C);

// BrMatrix34
extern void C2_HOOK_CDECL BrMatrix34Identity(br_matrix34* mat);
extern void C2_HOOK_CDECL BrMatrix34Translate(br_matrix34* mat, br_scalar dx, br_scalar dy, br_scalar dz);
extern void C2_HOOK_CDECL BrMatrix34Pre(br_matrix34* mat, const br_matrix34* A);
extern void C2_HOOK_CDECL BrMatrix34Post(br_matrix34* mat, const br_matrix34* A);
extern void C2_HOOK_CDECL BrMatrix34TApplyP(br_vector3* A, const br_vector3* B, const br_matrix34* C);
extern void C2_HOOK_CDECL BrMatrix34ApplyV(br_vector3* A, const br_vector3* B, const br_matrix34* C);
extern br_scalar C2_HOOK_CDECL BrMatrix34Inverse(br_matrix34* B, const br_matrix34* A);
extern void C2_HOOK_CDECL BrMatrix34LPInverse(br_matrix34* A, const br_matrix34* B);
extern void C2_HOOK_CDECL BrMatrix34ApplyP(br_vector3* A, const br_vector3* B, const br_matrix34* C);
extern void C2_HOOK_CDECL BrMatrix34Scale(br_matrix34* mat, br_scalar sx, br_scalar sy, br_scalar sz);
extern void C2_HOOK_CDECL BrMatrix34PostTranslate(br_matrix34* mat, br_scalar x, br_scalar y, br_scalar z);
extern void C2_HOOK_CDECL BrMatrix34Mul(br_matrix34* A, const br_matrix34* B, const br_matrix34* C);
extern void C2_HOOK_CDECL BrMatrix34Copy(br_matrix34* A, const br_matrix34* B);
extern void C2_HOOK_CDECL BrMatrix34RotateX(br_matrix34* mat, br_angle rx);
extern void C2_HOOK_CDECL BrMatrix34RotateY(br_matrix34* mat, br_angle ry);
extern void C2_HOOK_CDECL BrMatrix34PreScale(br_matrix34* mat, br_scalar sx, br_scalar sy, br_scalar sz);
extern void C2_HOOK_CDECL BrMatrix34PostScale(br_matrix34* mat, br_scalar sx, br_scalar sy, br_scalar sz);
extern void C2_HOOK_CDECL BrMatrix34PreTransform(br_matrix34* mat, const br_transform* xform);
extern void C2_HOOK_CDECL BrMatrix34PostTransform(br_matrix34* mat, const br_transform* xform);
extern void C2_HOOK_CDECL BrMatrix34TApplyV(br_vector3* A, const br_vector3* B, const br_matrix34* C);
extern void C2_HOOK_CDECL BrMatrix34LPNormalise(br_matrix34* A, const br_matrix34* B);
extern void C2_HOOK_CDECL BrMatrix34PreRotate(br_matrix34* mat, br_angle r, const br_vector3* axis);
extern void C2_HOOK_CDECL BrMatrix34Rotate(br_matrix34* mat, br_angle r, const br_vector3* a);
extern void C2_HOOK_CDECL BrMatrix34PreTranslate(br_matrix34* mat, br_scalar x, br_scalar y, br_scalar z);
extern void C2_HOOK_CDECL BrMatrix34PostShearX(br_matrix34* mat, br_scalar sy, br_scalar sz);
extern void C2_HOOK_CDECL BrMatrix34PostShearY(br_matrix34* mat, br_scalar sx, br_scalar sz);
extern void C2_HOOK_CDECL BrMatrix34PostShearZ(br_matrix34* mat, br_scalar sx, br_scalar sy);
extern void C2_HOOK_CDECL BrMatrix34PreShearX(br_matrix34* mat, br_scalar sy, br_scalar sz);
extern void C2_HOOK_CDECL BrMatrix34PostRotate(br_matrix34* mat, br_angle r, const br_vector3* axis);
extern void C2_HOOK_CDECL BrMatrix34PostRotateX(br_matrix34* mat, br_angle ry);
extern void C2_HOOK_CDECL BrMatrix34PostRotateY(br_matrix34* mat, br_angle ry);
extern void C2_HOOK_CDECL BrMatrix34PostRotateZ(br_matrix34* mat, br_angle ry);
extern void C2_HOOK_CDECL BrMatrix34PreRotateX(br_matrix34* mat, br_angle ry);
extern void C2_HOOK_CDECL BrMatrix34PreRotateY(br_matrix34* mat, br_angle ry);
extern void C2_HOOK_CDECL BrMatrix34PreRotateZ(br_matrix34* mat, br_angle rz);
extern void C2_HOOK_CDECL BrMatrix34RollingBall(br_matrix34 *mat, int dx,int dy, int radius);
extern br_euler* C2_HOOK_CDECL BrMatrix34ToEuler(br_euler* euler, const br_matrix34* mat);

// BrMatrix4
extern void C2_HOOK_CDECL BrMatrix4Copy(br_matrix4* A, const br_matrix4* B);
extern br_scalar C2_HOOK_CDECL BrMatrix4Inverse(br_matrix4* A, const br_matrix4* B);
extern void C2_HOOK_CDECL BrMatrix4TApply(br_vector4* A, const br_vector4* B, const br_matrix4* C);
extern void C2_HOOK_CDECL BrMatrix4ApplyP(br_vector4* A, br_vector3* B, const br_matrix4* C);
extern void C2_HOOK_CDECL BrMatrix4Perspective(br_matrix4* mat, br_angle field_of_view, br_scalar aspect, br_scalar hither, br_scalar yon);
extern void C2_HOOK_CDECL BrMatrix4Scale(br_matrix4* mat, br_scalar sx, br_scalar sy, br_scalar sz);
extern void C2_HOOK_CDECL BrMatrix4Mul(br_matrix4* A, const br_matrix4* B, const br_matrix4* C);
extern void C2_HOOK_CDECL BrMatrix4Mul34(br_matrix4* A, const br_matrix34* B, const br_matrix4* C);

// BrQuaternion
extern br_quat* C2_HOOK_CDECL BrMatrix34ToQuat(br_quat* q, const br_matrix34* mat);
extern br_quat* C2_HOOK_CDECL BrQuatInvert(br_quat* q, const br_quat* qq);
extern br_quat* C2_HOOK_CDECL BrQuatMul(br_quat* q, const br_quat* l, const br_quat* r);

// BrMem
extern void C2_HOOK_CDECL BrMemFree(void* block);
extern void* C2_HOOK_CDECL BrMemAllocate(br_size_t size, br_uint_8 type);
extern void* C2_HOOK_CDECL BrMemCalloc(int nelems, br_size_t size, br_uint_8 type);
extern char* C2_HOOK_CDECL BrMemStrDup(const char* str);

// BrModel
extern br_model* C2_HOOK_CDECL BrModelAllocate(const char* name, int nvertices, int nfaces);
extern br_model* C2_HOOK_CDECL BrModelAdd(br_model* model);
extern br_uint_32 C2_HOOK_CDECL BrModelAddMany(br_model** items, int n);
extern br_model* C2_HOOK_CDECL BrModelRemove(br_model* model);
extern br_model* C2_HOOK_CDECL BrModelLoad(const char* filename);
extern br_model* C2_HOOK_CDECL BrModelFind(const char* name);
extern void C2_HOOK_CDECL BrModelFree(br_model* model);
extern void C2_HOOK_CDECL BrModelUpdate(br_model* model, br_uint_16 flags);
extern br_uint_32 C2_HOOK_CDECL BrModelLoadMany(const char* filename, br_model** models, br_uint_16 num);
extern br_model_find_cbfn* C2_HOOK_CDECL BrModelFindHook(br_model_find_cbfn* hook);

// BrPixelmap
extern br_pixelmap* C2_HOOK_CDECL BrPixelmapLoad(const char* filename);
extern br_uint_32 C2_HOOK_CDECL BrPixelmapLoadMany(const char* filename, br_pixelmap** pixelmaps, br_uint_16 num);
extern br_uint_32 C2_HOOK_CDECL BrPixelmapSave(const char* filename, br_pixelmap* ptr);
extern br_uint_32 C2_HOOK_CDECL BrPixelmapSaveMany(const char* filename, br_pixelmap** pixelmaps, br_uint_16 num);
extern void C2_HOOK_CDECL BrPixelmapFree(br_pixelmap* src);
extern br_pixelmap* C2_HOOK_CDECL BrPixelmapMatch(br_pixelmap* src, br_uint_8 match_type);
extern br_pixelmap* C2_HOOK_CDECL BrPixelmapAllocateSub(br_pixelmap* src, br_int_32 x, br_int_32 y, br_int_32 w, br_int_32 h);
extern void C2_HOOK_CDECL BrPixelmapFill(br_pixelmap* dst, br_uint_32 colour);
extern void C2_HOOK_CDECL BrPixelmapRectangleCopy(br_pixelmap* dst, br_int_32 dx, br_int_32 dy, br_pixelmap* src, br_int_32 sx, br_int_32 sy, br_int_32 w, br_int_32 h);
extern void C2_HOOK_CDECL BrPixelmapCopy(br_pixelmap* dst, br_pixelmap* src);
extern void C2_HOOK_CDECL BrPixelmapLine(br_pixelmap* dst, br_int_32 x1, br_int_32 y1, br_int_32 x2, br_int_32 y2, br_uint_32 colour);
extern void C2_HOOK_CDECL BrPixelmapRectangle(br_pixelmap* dst, br_int_32 x, br_int_32 y, br_int_32 w, br_int_32 h, br_uint_32 colour);
extern void C2_HOOK_CDECL BrPixelmapRectangleFill(br_pixelmap* dst, br_int_32 x, br_int_32 y, br_int_32 w, br_int_32 h, br_uint_32 colour);
extern br_uint_32 C2_HOOK_CDECL BrPixelmapPixelGet(br_pixelmap* dst, br_int_32 x, br_int_32 y);
extern void C2_HOOK_CDECL BrPixelmapPixelSet(br_pixelmap* dst, br_int_32 x, br_int_32 y, br_uint_32 colour);
extern br_pixelmap* C2_HOOK_CDECL BrPixelmapAllocate(br_uint_8 type, br_int_32 w, br_int_32 h, void* pixels, int flags);
extern void C2_HOOK_CDECL BrPixelmapDoubleBuffer(br_pixelmap* dst, br_pixelmap* src);
extern void C2_HOOK_CDECL BrPixelmapPaletteSet(br_pixelmap *pm, br_pixelmap *pal);
extern void C2_HOOK_CDECL BrPixelmapPaletteEntrySet(br_pixelmap *pm, br_int_32 i,br_colour colour);
extern void C2_HOOK_CDECL BrPixelmapPaletteEntrySetMany(br_pixelmap *pm, br_int_32 index , br_int_32 ncolours, br_colour *colours);
extern void C2_HOOK_CDECL BrPixelmapText(br_pixelmap* dst, br_int_32 x, br_int_32 y, br_uint_32 colour, br_font* font, char* text);
extern void C2_HOOK_CDECL BrPixelmapTextF(br_pixelmap* dst, br_int_32 x, br_int_32 y, br_uint_32 colour, br_font* font, char* fmt, ...);
extern br_uint_16 C2_HOOK_CDECL BrPixelmapTextWidth(br_pixelmap* dst, br_font* font, char* text);

// BrQSort
extern void C2_HOOK_CDECL BrQsort(void* basep, unsigned int nelems, unsigned int size, br_qsort_cbfn comp);

// BrRes
extern void* C2_HOOK_CDECL BrResAllocate(void* vparent, br_size_t size, br_uint_8 res_class);
extern void* C2_HOOK_CDECL BrResAdd(void* vparent, void* vres);
extern br_resource_class* C2_HOOK_CDECL BrResClassAdd(br_resource_class* rclass);
extern void* C2_HOOK_CDECL BrResRemove(void* vres);
extern void C2_HOOK_CDECL BrResFree(void* vres);
extern char* C2_HOOK_CDECL BrResStrDup(void* vparent, const char* str);
extern br_uint_32 C2_HOOK_CDECL BrResSize(void* vres);
extern br_uint_8 C2_HOOK_CDECL BrResClass(void* vres);

// BrTable
extern br_pixelmap* C2_HOOK_CDECL BrTableAdd(br_pixelmap* pixelmap);
extern br_pixelmap* C2_HOOK_CDECL BrTableFind(const char* pattern);
extern br_pixelmap* C2_HOOK_CDECL BrTableRemove(br_pixelmap* pixelmap);
extern br_uint_32 C2_HOOK_CDECL  BrTableAddMany(br_pixelmap** items, int n);
extern void C2_HOOK_CDECL BrTableUpdate(br_pixelmap* table, br_uint_16 flags);
extern br_uint_32 C2_HOOK_CDECL BrTableEnum(const char* pattern, br_table_enum_cbfn* callback, void* arg);
extern br_table_find_cbfn* C2_HOOK_CDECL BrTableFindHook(br_table_find_cbfn* hook);

// BrTransform
extern void C2_HOOK_CDECL BrTransformToMatrix34(br_matrix34* mat, br_transform* xform);

// BrEuler
extern br_matrix34* C2_HOOK_CDECL BrEulerToMatrix34(br_matrix34* mat, const br_euler* euler);

// BrV1db
extern void C2_HOOK_CDECL BrV1dbBeginWrapper_Float(void);

// BrVector3
// extern void C2_HOOK_CDECL BrVector3Cross(br_vector3* v1, br_vector3* v2, br_vector3* v3);
extern void C2_HOOK_CDECL BrVector3SetFloat(br_vector3* v1, float f1, float f2, float f3);
// extern void C2_HOOK_CDECL BrVector3Sub(br_vector3* v1, br_vector3* v2, br_vector3* v3);
// extern void C2_HOOK_CDECL BrVector3Accumulate(br_vector3* v1, br_vector3* v2);
// extern void C2_HOOK_CDECL BrVector3Normalise(br_vector3* v1, br_vector3* v2);

// BrTransform
extern void C2_HOOK_CDECL BrTransformToTransform(br_transform* dest, br_transform* src);

// Logging
extern C2_NORETURN void C2_HOOK_CDECL BrFailure(const char* s, ...);
extern void C2_HOOK_CDECL BrFatal(const char* name, int line, const char* s, ...);

// Z-buffer with primitive (or so I think)
extern void C2_HOOK_CDECL BrZbsBegin(br_uint_8 colour_type, br_uint_8 depth_type, void* primitive, br_uint_32 size);

// Z-buffer
extern void C2_HOOK_CDECL BrZbBegin(br_uint_8 colour_type, br_uint_8 depth_type);

extern void C2_HOOK_CDECL BrZbSceneRender(br_actor* world, br_actor* camera, br_pixelmap* colour_buffer, br_pixelmap* depth_buffer);
extern void C2_HOOK_CDECL BrZbSceneRenderBegin(br_actor* world, br_actor* camera, br_pixelmap* colour_buffer, br_pixelmap* depth_buffer);
extern void C2_HOOK_CDECL BrZbSceneRenderAdd(br_actor* tree);
extern void C2_HOOK_CDECL BrZbSceneRenderEnd(void);

extern void C2_HOOK_CDECL BrZbsSceneRender(br_actor* world, br_actor* camera, br_pixelmap* colour_buffer, br_pixelmap* depth_buffer);

extern void C2_HOOK_CDECL BrZbModelRender(br_actor* actor, br_model* model, br_material* material, br_uint_8 style, int on_screen, int use_custom);
extern void C2_HOOK_CDECL BrZsModelRender(br_actor* actor, br_model* model, br_material* material, br_order_table* order_table, br_uint_8 style, int on_screen, int use_custom);

extern void C2_HOOK_CDECL BrZbsSceneRenderBegin(br_actor* world, br_actor* camera, br_pixelmap* colour_buffer, br_pixelmap* depth_buffer);
extern void C2_HOOK_CDECL BrZbsSceneRenderAdd(br_actor* tree);
extern void C2_HOOK_CDECL BrZbsSceneRenderEnd(void);

extern br_pixelmap* C2_HOOK_CDECL DOSGfxBegin(const char* setup_string);

// Various
extern br_uint_32 C2_HOOK_CDECL BrOnScreenCheck(br_bounds3* bounds);

extern int C2_HOOK_CDECLBrWriteModeSet(int mode);
extern br_uint_32 C2_HOOK_CDECL BrSwap32(br_uint_32 l);
extern br_diaghandler* C2_HOOK_CDECL BrDiagHandlerSet(br_diaghandler* newdh);

extern void C2_HOOK_CDECL _BrEndHook(void);
extern br_error C2_HOOK_CDECL BrDevFind(br_device** pdev, const char* pattern);
extern br_error C2_HOOK_CDECL BrDevRemove(br_device* dev);

// Carmageddon 2 adaptation
extern void C2_HOOK_CDECL BrSetScreenZOffset(br_uint_32 pOffset);

#endif
