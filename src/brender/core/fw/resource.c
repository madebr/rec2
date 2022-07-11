#include "resource.h"

#define HOOK_RESOURCE 1

#if !HOOK_RESOURCE
#include "core/fw/brlists.h"
#include "core/fw/diag.h"
#include "core/fw/fwsetup.h"
#include "core/fw/mem.h"
#include "core/fw/scratch.h"
#include "core/std/brstdlib.h"

#include "c2_stdio.h"
#include "c2_stdlib.h"
#endif

#define RES_ALIGN 4
#define RESOURCE_SIZE(RES) ((RES)->size_h << 18)  | ((RES)->size_m << 10) | ((RES)->size_l << 2)

#if !HOOK_RESOURCE
void* ResToUser(resource_header* r) {
    br_int_32 align;

    align = C2V(fw).resource_class_index[r->class]->alignment;
    if (align <= 0) {
        align = RES_ALIGN;
    }
    return (void*)(((br_size_t)r + sizeof(resource_header) + align - 1) & ~(align - 1));
}

resource_header* UserToRes(void* r) {
    br_uint_8* p;

    p = (br_uint_8*)r - 1;
    while (*p == 0) {
        p--;
    }
    if (((resource_header*)(p - (sizeof(resource_header) - 1)))->magic_num != 0xdeadbeef) {
        c2_abort();
        // FIXME: add panic macro
        // LOG_PANIC("Bad resource header from user at %p. Was 0x%x", r, ((resource_header*)p)->magic_num);
    }
    return (resource_header*)(p - (sizeof(resource_header) - 1));
}
#endif

void* (C2_HOOK_CDECL * BrResAllocate_original)(void* vparent, br_size_t size, br_uint_8 res_class);
void* C2_HOOK_CDECL BrResAllocate(void* vparent, br_size_t size, br_uint_8 res_class) {
#if HOOK_RESOURCE
    return BrResAllocate_original(vparent, size, res_class);
#else
    resource_header* res;
    resource_header* parent;
    br_int_32 malign;
    br_int_32 calign;
    br_int_32 pad;
    br_int_32 actual_pad;

    size = (size + sizeof(resource_header) + RES_ALIGN - 1) & ~(RES_ALIGN - 1);

    malign = BrMemAlign(res_class);

    calign = C2V(fw).resource_class_index[res_class]->alignment;
    if (calign <= 0) {
        calign = RES_ALIGN;
    }

    pad = (~(malign - 1)) & (calign - 1);
    res = (resource_header*)BrMemAllocate(pad + size, res_class);
    actual_pad = ((((br_size_t)res + calign - 1) & (~(calign - 1))) - (br_size_t)res);
    if (actual_pad > pad) {
        BrFailure("Memory allocator broke alignment");
    }
    res->class = res_class;
    res->size_l = (br_uint_8)(size >> 2);
    res->size_m = (br_uint_8)(size >> 10);
    res->size_h = (br_uint_8)(size >> 18);
    BrSimpleNewList(&res->children);
    res->magic_num = 0xdeadbeef;
    res->magic_ptr = res;

    if (vparent != NULL) {
        // vparent points to a resource body, we track backwards to obtain its resource_header
        parent = UserToRes(vparent);
        BrSimpleAddHead(&parent->children, &res->node);
    }

    return ResToUser(res);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005276c0, BrResAllocate, BrResAllocate_original)

void (C2_HOOK_STDCALL * BrResInternalFree_original)(resource_header* res, br_boolean callback);
void C2_HOOK_STDCALL BrResInternalFree(resource_header* res, br_boolean callback) {
#if HOOK_RESOURCE
    BrResInternalFree_original(res, callback);
#else
    void* r;

    if (res->class == BR_MEMORY_FREE) {
        return;
    }
    if (callback != 0) {
        if (C2V(fw).resource_class_index[res->class]->free_cb != NULL) {
            C2V(fw).resource_class_index[res->class]->free_cb(ResToUser(res), res->class, RESOURCE_SIZE(res));
        }
    }

    while (res->children.head != NULL) {
        r = BrSimpleRemove(res->children.head);
        BrResInternalFree((resource_header*)r, 1);
    }
    if (res->node.prev != NULL) {
        BrSimpleRemove(&res->node);
    }
    res->magic_num = 1;
    res->magic_ptr = NULL;
    BrMemFree(res);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005277f0, BrResInternalFree, BrResInternalFree_original)

void (C2_HOOK_CDECL * BrResFree_original)(void* vres);
void C2_HOOK_CDECL BrResFree(void* vres) {

#if HOOK_RESOURCE
    BrResFree_original(vres);
#else
    BrResInternalFree(UserToRes(vres), 1);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005277c0, BrResFree, BrResFree_original)

void (C2_HOOK_CDECL * BrResFreeNoCallback_original)(void* vres);
void C2_HOOK_CDECL BrResFreeNoCallback(void* vres) {

#if HOOK_RESOURCE
    BrResFreeNoCallback_original(vres);
#else
    BrResInternalFree(UserToRes(vres), 0);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005278b0, BrResFreeNoCallback, BrResFreeNoCallback_original)

void* (C2_HOOK_CDECL * BrResAdd_original)(void* vparent, void* vres);
void* C2_HOOK_CDECL BrResAdd(void* vparent, void* vres) {

#if HOOK_RESOURCE
    return BrResAdd_original(vparent, vres);
#else
    resource_header* res;
    resource_header* parent;

    res = UserToRes(vres);
    parent = UserToRes(vparent);

    if (res->node.prev != NULL) {
        BrSimpleRemove(&res->node);
    }
    BrSimpleAddHead(&parent->children, &res->node);
    return vres;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005278e0, BrResAdd, BrResAdd_original)

void* (C2_HOOK_CDECL * BrResRemove_original)(void* vres);
void* C2_HOOK_CDECL BrResRemove(void* vres) {
#if HOOK_RESOURCE
    return BrResRemove_original(vres);
#else
    resource_header* res;

    res = UserToRes(vres);
    BrSimpleRemove(&res->node);
    return vres;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00527940, BrResRemove, BrResRemove_original)

br_uint_8 (C2_HOOK_CDECL * BrResClass_original)(void* vres);
br_uint_8 C2_HOOK_CDECL BrResClass(void* vres) {

#if HOOK_RESOURCE
    return BrResClass_original(vres);
#else
    resource_header* res;

    res = UserToRes(vres);
    return res->class;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00527970, BrResClass, BrResClass_original)

br_boolean (C2_HOOK_CDECL * BrResIsChild_original)(void* vparent, void* vchild);
br_boolean C2_HOOK_CDECL BrResIsChild(void* vparent, void* vchild) {

#if HOOK_RESOURCE
    return BrResIsChild_original(vparent, vchild);
#else
    resource_header* parent;
    resource_header* child;
    resource_header* cp;

    parent = UserToRes(vparent);
    child = UserToRes(vchild);

    for (cp = (resource_header*)parent->children.head; cp != NULL; cp = (resource_header*)cp->node.next) {
        if (cp == child) {
            return 1;
        }
    }
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00527990, BrResIsChild, BrResIsChild_original)

br_uint_32 (C2_HOOK_CDECL * BrResSize_original)(void* vres);
br_uint_32 C2_HOOK_CDECL BrResSize(void* vres) {

#if HOOK_RESOURCE
    return BrResSize_original(vres);
#else
    resource_header* res;

    res = UserToRes(vres);
    return RESOURCE_SIZE(res);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005279f0, BrResSize, BrResSize_original)

br_uint_32 C2_HOOK_CDECL ResSizeTotal(void* vres, br_uint_32* ptotal) {
    *ptotal += BrResSize(vres);
    BrResChildEnum(vres, (br_resenum_cbfn*)ResSizeTotal, ptotal);
    return 0;
}

br_uint_32 (C2_HOOK_CDECL * BrResSizeTotal_original)(void* vres);
br_uint_32 C2_HOOK_CDECL BrResSizeTotal(void* vres) {
#if HOOK_RESOURCE
    return BrResSizeTotal_original(vres);
#else
    br_uint_32 total;

    total = BrResSize(vres);
    BrResChildEnum(vres, (br_resenum_cbfn*)ResSizeTotal, &total);
    return total;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00527a30, BrResSizeTotal, BrResSizeTotal_original)

br_uint_32 (C2_HOOK_CDECL * BrResChildEnum_original)(void* vres, br_resenum_cbfn* callback, void* arg);
br_uint_32 C2_HOOK_CDECL BrResChildEnum(void* vres, br_resenum_cbfn* callback, void* arg) {
#if HOOK_RESOURCE
    return BrResChildEnum_original(vres, callback, arg);
#else
    resource_header* res;
    resource_header* rp;
    br_uint_32 r;

    res = UserToRes(vres);
    r = 0;
    for (rp = (resource_header*)res->children.head; rp != NULL; rp = (resource_header*)rp->node.next) {
        r = callback(ResToUser(rp), arg);
        if (r != 0) {
            break;
        }
    }
    return r;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00527b60, BrResChildEnum, BrResChildEnum_original)

br_uint_32 (C2_HOOK_CDECL * BrResCheck_original)(void* vres, int no_tag);
br_uint_32 C2_HOOK_CDECL BrResCheck(void* vres, int no_tag) {
#if HOOK_RESOURCE
    return BrResCheck_original(vres, no_tag);
#else
    resource_header* res;

    res = UserToRes(vres);
    if ((res->magic_ptr ==  res) & (res->magic_num == 0xdeadbeef)) {
        return 1;
    }
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00527bd0, BrResCheck, BrResCheck_original)

char* (C2_HOOK_CDECL * BrResStrDup_original)(void* vparent, const char* str);
char* C2_HOOK_CDECL BrResStrDup(void* vparent, const char* str) {
#if HOOK_RESOURCE
    return BrResStrDup_original(vparent, str);
#else
    int l;
    char* nstr;

    l = BrStrLen(str);
    nstr = (char*)BrResAllocate(vparent, l + 1, BR_MEMORY_STRING);
    BrStrCpy(nstr, str);
    return nstr;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00527c10, BrResStrDup, BrResStrDup_original)

#if !HOOK_RESOURCE
void InternalResourceDump(resource_header* res, br_putline_cbfn* putline, void* arg, int level) {
    int i;
    char* cp;
    resource_header* child;
    br_resource_class* rclass;

    rclass = C2V(fw).resource_class_index[res->class];
    cp = BrScratchString();
    for (i = 0; i < level; i++) {
        *cp = ' ';
        cp++;
    }
    sprintf(cp, "%p [%s] size=%u", res, rclass->identifier, RESOURCE_SIZE(res));
    putline(BrScratchString(), arg);
    for (child = (resource_header*)res->children.head; child != NULL; child = (resource_header*)child->node.next) {
        InternalResourceDump(child, putline, arg, level + 1);
    }
}
#endif

void (C2_HOOK_CDECL * BrResDump_original)(void* vres, br_putline_cbfn* putline, void* arg);
void C2_HOOK_CDECL BrResDump(void* vres, br_putline_cbfn* putline, void* arg) {
#if HOOK_RESOURCE
    BrResDump_original(vres, putline, arg);
#else
    resource_header* res;

    res = UserToRes(vres);
    InternalResourceDump(res, putline, arg, 0);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00527c50, BrResDump, BrResDump_original)

char* (C2_HOOK_CDECL * BrResClassIdentifier_original)(br_uint_8 res_class);
char* C2_HOOK_CDECL BrResClassIdentifier(br_uint_8 res_class) {
#if HOOK_RESOURCE
    return BrResClassIdentifier_original(res_class);
#else
    br_resource_class* rclass;

    rclass = C2V(fw).resource_class_index[res_class];
    if (rclass == NULL) {
        return "<NULL>";
    }
    return rclass->identifier;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00527c60, BrResClassIdentifier, BrResClassIdentifier_original)
