#ifndef REC2_RESOURCE_H
#define REC2_RESOURCE_H

#include "c2_hooks.h"

#include "brender/br_types.h"

extern void* C2_HOOK_CDECL ResToUser(resource_header* r);

resource_header* C2_HOOK_CDECL UserToRes(void* r);

extern void* C2_HOOK_CDECL BrResAllocate(void* vparent, br_size_t size, br_uint_8 res_class);

extern void C2_HOOK_STDCALL BrResInternalFree(resource_header* res, br_boolean callback);

extern void C2_HOOK_CDECL BrResFree(void* vres);

extern void C2_HOOK_CDECL BrResAssert(void* vres);

extern void C2_HOOK_CDECL BrResFreeNoCallback(void* vres);

extern void* C2_HOOK_CDECL BrResAdd(void* vparent, void* vres);

extern void* C2_HOOK_CDECL BrResRemove(void* vres);

extern br_uint_8 C2_HOOK_CDECL BrResClass(void* vres);

extern br_boolean C2_HOOK_CDECL BrResIsChild(void* vparent, void* vchild);

extern br_uint_32 C2_HOOK_CDECL BrResSize(void* vres);

extern br_uint_32 C2_HOOK_CDECL ResSizeTotal(void* vres, br_uint_32* ptotal);

extern br_uint_32 C2_HOOK_CDECL BrResSizeTotal(void* vres);

extern br_uint_32 C2_HOOK_CDECL BrResChildEnum(void* vres, br_resenum_cbfn* callback, void* arg);

extern br_uint_32 C2_HOOK_CDECL BrResCheck(void* vres, int no_tag);

extern char* C2_HOOK_CDECL BrResStrDup(void* vparent, const char* str);

extern void C2_HOOK_CDECL InternalResourceDump(resource_header* res, br_putline_cbfn* putline, void* arg, int level);

extern void C2_HOOK_CDECL BrResDump(void* vres, br_putline_cbfn* putline, void* arg);

extern char* C2_HOOK_CDECL BrResClassIdentifier(br_uint_8 res_class);

#endif // REC2_RESOURCE_H
