#ifndef REC2_RESOURCE_H
#define REC2_RESOURCE_H

#include "c2_hooks.h"

#include "brender/br_types.h"

void* C2_HOOK_CDECL ResToUser(resource_header* r);

resource_header* C2_HOOK_CDECL UserToRes(void* r);

void* C2_HOOK_CDECL BrResAllocate(void* vparent, br_size_t size, br_uint_8 res_class);

void C2_HOOK_STDCALL BrResInternalFree(resource_header* res, br_boolean callback);

void C2_HOOK_CDECL BrResFree(void* vres);

void C2_HOOK_CDECL BrResAssert(void* vres);

void C2_HOOK_CDECL BrResFreeNoCallback(void* vres);

void* C2_HOOK_CDECL BrResAdd(void* vparent, void* vres);

void* C2_HOOK_CDECL BrResRemove(void* vres);

br_uint_8 C2_HOOK_CDECL BrResClass(void* vres);

br_boolean C2_HOOK_CDECL BrResIsChild(void* vparent, void* vchild);

br_uint_32 C2_HOOK_CDECL BrResSize(void* vres);

br_uint_32 C2_HOOK_CDECL ResSizeTotal(void* vres, br_uint_32* ptotal);

br_uint_32 C2_HOOK_CDECL BrResSizeTotal(void* vres);

br_uint_32 C2_HOOK_CDECL BrResChildEnum(void* vres, br_resenum_cbfn* callback, void* arg);

br_uint_32 C2_HOOK_CDECL BrResCheck(void* vres, int no_tag);

char* C2_HOOK_CDECL BrResStrDup(void* vparent, char* str);

void C2_HOOK_CDECL InternalResourceDump(resource_header* res, br_putline_cbfn* putline, void* arg, int level);

void C2_HOOK_CDECL BrResDump(void* vres, br_putline_cbfn* putline, void* arg);

char* C2_HOOK_CDECL BrResClassIdentifier(br_uint_8 res_class);

#endif // REC2_RESOURCE_H
