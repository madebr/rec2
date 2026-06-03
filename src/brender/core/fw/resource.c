#include "resource.h"


#include "core/fw/brlists.h"
#include "core/fw/diag.h"
#include "core/fw/fwsetup.h"
#include "core/fw/mem.h"
#include "core/fw/scratch.h"
#include "core/std/brstdlib.h"

#include <stdio.h>
#include "c2_stdlib.h"

#define RES_ALIGN 4
#define RESOURCE_SIZE(RES) ((RES)->size_h << 18)  | ((RES)->size_m << 10) | ((RES)->size_l << 2)

void* ResToUserWithClass(resource_header* r, br_uint_8 class) {
    br_int_32 align;

    align = fw.resource_class_index[class]->alignment;
    if (align <= 0) {
        align = RES_ALIGN;
    }
    return (void*)(((br_size_t)r + sizeof(resource_header) + align - 1) & ~(align - 1));
}

void* C2_HOOK_CDECL ResToUser(resource_header* r) {

    return ResToUserWithClass(r, r->class);
}

resource_header* C2_HOOK_CDECL UserToRes(void* r) {
    br_uint_8* p = r;

    while (p[-1] == 0) {
        p--;
    }
#ifdef REC2_MATCHING
    p -= offsetof(struct resource_header, magic_num) + sizeof(((struct resource_header *)NULL)->magic_num);
#else
    p -= sizeof(resource_header);
#endif
#ifdef BRENDER_FIX_BUGS
    if (((resource_header*)p)->magic_num != 0xdeadbeef) {
        abort();
        // FIXME: add panic macro
        // LOG_PANIC("Bad resource header from user at %p. Was 0x%x", r, ((resource_header*)p)->magic_num);
    }
#endif
    return (resource_header*)p;
}

// FUNCTION: CARMA2_HW 0x005276c0
void* C2_HOOK_CDECL BrResAllocate(void* vparent, br_size_t size, br_uint_8 res_class) {
    resource_header* res;
    resource_header* parent;
    br_int_32 malign;
    br_int_32 calign;
    br_int_32 pad;
    br_int_32 actual_pad;

    size = (size + sizeof(resource_header) + RES_ALIGN - 1) & ~(RES_ALIGN - 1);

    malign = BrMemAlign(res_class);

    calign = fw.resource_class_index[res_class]->alignment;
    if (calign <= 0) {
        calign = RES_ALIGN;
    }

    pad = (~(malign - 1)) & (calign - 1);
    res = BrMemAllocate(pad + size, res_class);
    actual_pad = (((br_size_t)res + calign - 1) & (~(calign - 1))) - (br_size_t)res;
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

#if 0
    if ((uintptr_t)UserToRes(ResToUser(res)) != (uintptr_t)res) {
        abort();
    }
#endif

    return ResToUser(res);
}

// FUNCTION: CARMA2_HW 0x005277f0
void C2_HOOK_STDCALL BrResInternalFree(resource_header* res, br_boolean callback) {
    void* r;
    int original_class;

    if (res->class == BR_MEMORY_FREE) {
        return;
    }

    original_class = res->class;
    r = ResToUser(res);
    res->class = BR_MEMORY_FREE;
    if (callback) {
        if (fw.resource_class_index[original_class]->free_cb != NULL) {
            fw.resource_class_index[original_class]->free_cb(r, original_class, RESOURCE_SIZE(res));
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
}

// FUNCTION: CARMA2_HW 0x005277c0
void C2_HOOK_CDECL BrResFree(void* vres) {

    BrResInternalFree(UserToRes(vres), 1);
}

// FUNCTION: CARMA2_HW 0x005278b0
void C2_HOOK_CDECL BrResFreeNoCallback(void* vres) {

    BrResInternalFree(UserToRes(vres), 0);
}

// FUNCTION: CARMA2_HW 0x005278e0
void* C2_HOOK_CDECL BrResAdd(void* vparent, void* vres) {
    resource_header* res;
    resource_header* parent;

    res = UserToRes(vres);
    parent = UserToRes(vparent);

    if (res->node.prev != NULL) {
        BrSimpleRemove(&res->node);
    }
    BrSimpleAddHead(&parent->children, &res->node);
    return vres;
}

// FUNCTION: CARMA2_HW 0x00527940
void* C2_HOOK_CDECL BrResRemove(void* vres) {
    resource_header* res;

    res = UserToRes(vres);
    BrSimpleRemove(&res->node);
    return vres;
}

// FUNCTION: CARMA2_HW 0x00527970
br_uint_8 C2_HOOK_CDECL BrResClass(void* vres) {
    resource_header* res;

    res = UserToRes(vres);
    return res->class;
}

// FUNCTION: CARMA2_HW 0x00527990
br_boolean C2_HOOK_CDECL BrResIsChild(void* vparent, void* vchild) {
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
}

// FUNCTION: CARMA2_HW 0x005279f0
br_uint_32 C2_HOOK_CDECL BrResSize(void* vres) {
    resource_header* res;

    res = UserToRes(vres);
    return RESOURCE_SIZE(res);
}

br_uint_32 C2_HOOK_CDECL ResSizeTotal(void* vres, br_uint_32* ptotal) {
    *ptotal += BrResSize(vres);
    BrResChildEnum(vres, (br_resenum_cbfn*)ResSizeTotal, ptotal);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00527a30
br_uint_32 C2_HOOK_CDECL BrResSizeTotal(void* vres) {
    br_uint_32 total;

    total = BrResSize(vres);
    BrResChildEnum(vres, (br_resenum_cbfn*)ResSizeTotal, &total);
    return total;
}

// FUNCTION: CARMA2_HW 0x00527b60
br_uint_32 C2_HOOK_CDECL BrResChildEnum(void* vres, br_resenum_cbfn* callback, void* arg) {
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
}

// FUNCTION: CARMA2_HW 0x00527bd0
br_uint_32 C2_HOOK_CDECL BrResCheck(void* vres, int no_tag) {
    resource_header* res;

    res = UserToRes(vres);
    if ((res->magic_ptr == res) && (res->magic_num == 0xdeadbeef)) {
        return 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00527c10
char* C2_HOOK_CDECL BrResStrDup(void* vparent, const char* str) {
    int l;
    char* nstr;

    l = BrStrLen(str);
    nstr = (char*)BrResAllocate(vparent, l + 1, BR_MEMORY_STRING);
    BrStrCpy(nstr, str);
    return nstr;
}

void C2_HOOK_CDECL InternalResourceDump(resource_header* res, br_putline_cbfn* putline, void* arg, int level) {
    int i;
    char* cp;
    resource_header* child;
    br_resource_class* rclass;

    rclass = fw.resource_class_index[res->class];
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

// FUNCTION: CARMA2_HW 0x00527c50
void C2_HOOK_CDECL BrResDump(void* vres, br_putline_cbfn* putline, void* arg) {
    resource_header* res;

    res = UserToRes(vres);
    InternalResourceDump(res, putline, arg, 0);
}

// FUNCTION: CARMA2_HW 0x00527c60
char* C2_HOOK_CDECL BrResClassIdentifier(br_uint_8 res_class) {
    br_resource_class* rclass;

    rclass = fw.resource_class_index[res_class];
    if (rclass == NULL) {
        return "<NULL>";
    }
    return rclass->identifier;
}
