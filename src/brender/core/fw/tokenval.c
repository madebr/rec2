#include "tokenval.h"

#include "resource.h"
#include "token.h"

br_tv_template* C2_HOOK_CDECL BrTVTemplateAllocate(void* res, br_tv_template_entry* entries, int n_entries) {
    br_tv_template* t;

    t = BrResAllocate(res, sizeof(br_tv_template), BR_MEMORY_TOKEN_TEMPLATE);
    t->res = t;
    t->entries = entries;
    t->n_entries = n_entries;
    return t;
}
C2_HOOK_FUNCTION(0x0052d740, BrTVTemplateAllocate)

void C2_HOOK_CDECL BrTVTemplateFree(br_tv_template* t) {

    BrResFree(t);
}
C2_HOOK_FUNCTION(0x0052d770, BrTVTemplateFree)

void C2_HOOK_STDCALL templateResolveNames(br_tv_template* template) {
    br_tv_template_entry* tp;
    int n;

    for (n = 0; n < template->n_map_entries; n++) {
        tp = &template->entries[n];
        if (tp->name != NULL) {
            tp->token = BrTokenCreate(tp->name, BR_NULL_TOKEN);
        }
    }
}

void (C2_HOOK_STDCALL * templateMakeMap_original)(br_tv_template* template);
void C2_HOOK_STDCALL templateMakeMap(br_tv_template* template) {
#if defined(C2_HOOKS_ENABLED)
    templateMakeMap_original(template);
#else
    br_tv_template_entry* tp;
    char* map;
    br_int_32 min;
    br_int_32 max;
    int i;
    int n;
    int e;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052d7f0, templateMakeMap, templateMakeMap_original)

#if 0
br_fixed_ls* C2_HOOK_STDCALL ConvertFloatToFixed(br_fixed_ls** pextra, br_float* src, br_int_32 count, br_size_t* pextra_space) {
    br_fixed_ls* ret;
#error "Not implemented"
}

br_float* C2_HOOK_STDCALL ConvertFixedToFloat(br_float** pextra, br_fixed_ls* src, br_int_32 count, br_size_t* pextra_space) {
    br_float* ret;
#error "Not implemented"
}

br_uint_32* C2_HOOK_STDCALL ConvertLongCopy(br_uint_32** pextra, br_uint_32* src, br_int_32 count, br_size_t* pextra_space) {
    br_uint_32* ret;
#error "Not implemented"
}
#endif

br_error (C2_HOOK_STDCALL * ValueQuery_original)(br_token_value* tv, void** pextra, br_size_t* pextra_size, void* block, br_tv_template_entry* tep);
br_error C2_HOOK_STDCALL ValueQuery(br_token_value* tv, void** pextra, br_size_t* pextra_size, void* block, br_tv_template_entry* tep) {
#if defined(C2_HOOKS_ENABLED)
    return ValueQuery_original(tv, pextra, pextra_size, block, tep);
#else
    void* mem;
    br_uint_32* lp;
    int t;
    br_tv_custom* custp;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052d8e0, ValueQuery, ValueQuery_original)

br_error (C2_HOOK_STDCALL * ValueSet_original)(void* block, br_token_value* tv, br_tv_template_entry* tep);
br_error C2_HOOK_STDCALL ValueSet(void* block, br_token_value* tv, br_tv_template_entry* tep) {
#if defined(C2_HOOKS_ENABLED)
    return ValueSet_original(block, tv, tep);
#else
    void* mem;
    int i;
    int t;
    br_tv_custom* custp;
##error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052e630, ValueSet, ValueSet_original)

br_size_t (C2_HOOK_STDCALL * ValueExtraSize_original)(void* block, br_tv_template_entry* tep);
br_size_t C2_HOOK_STDCALL ValueExtraSize(void* block, br_tv_template_entry* tep) {
#if defined(C2_HOOKS_ENABLED)
    return ValueExtraSize_original(block, tep);
#else
    void* mem;
    br_uint_32* lp;
    int t;
    br_tv_custom* custp;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052e300, ValueExtraSize, ValueExtraSize_original)

// IDA: br_error __cdecl BrTokenValueQuery(br_uint_32 *pvalue, br_uint_32 *extra, br_size_t extra_size, br_token t, void *block, br_tv_template *template)
br_error C2_HOOK_CDECL BrTokenValueQuery(br_uint_32* pvalue, br_uint_32* extra, br_size_t extra_size, br_token t, void* block, br_tv_template* template) {
    br_int_32 o;
    br_tv_template_entry* tep;
    br_token_value tv;
    br_error r;

    if (template->n_map_entries == 0) {
        templateMakeMap(template);
    }
    o = t - template->map_base;
    if (o < 0 || o >= template->n_map_entries) {
        return 0x1001;
    }
    tep = template->map_query_entry[o];
    if (tep != NULL) {
        r = ValueQuery(&tv, (void**)&extra, &extra_size, block, tep);
        *pvalue = tv.v.u32;
        return r;
    }
    return 0x1001;
}
C2_HOOK_FUNCTION(0x0052d780, BrTokenValueQuery)

br_error C2_HOOK_CDECL BrTokenValueQueryMany(br_token_value* tv, void* extra, br_size_t extra_size, br_int_32* pcount, void* block, br_tv_template* template) {
    br_int_32 o;
    br_tv_template_entry* tep;
    br_error r;

    if (template->n_map_entries == 0) {
        templateMakeMap(template);
    }
    *pcount = 0;

    for (; tv->t != BR_NULL_TOKEN; tv++) {
        o = tv->t - template->map_base;
        if (o < 0 || o >= template->n_map_entries) {
            continue;
        }
        tep = template->map_query_entry[o];
        if (tep == NULL) {
            continue;
        }
        r = ValueQuery(tv, &extra, &extra_size, block, tep);
        if (r == 0) {
            (*pcount)++;
        }
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0052e230, BrTokenValueQueryMany)

br_error C2_HOOK_CDECL BrTokenValueQueryManySize(br_size_t* psize, br_token_value* tv, void* block, br_tv_template* template) {
    br_int_32 o;
    br_size_t extra_size;

    extra_size = 0;

    if (template->n_map_entries == 0) {
        templateMakeMap(template);
    }

    for (; tv->t != 0; tv++) {
        o = tv->t - template->map_base;
        if (o < 0 || o >= template->n_map_entries) {
            continue;
        }
        if (template->map_query_entry[o]) {
            extra_size += ValueExtraSize(block, template->map_query_entry[o]);
        }
    }
    *psize = extra_size;
    return 0;
}
C2_HOOK_FUNCTION(0x0052e2a0, BrTokenValueQueryManySize)

br_error (C2_HOOK_CDECL * BrTokenValueQueryAll_original)(br_token_value* buffer, br_size_t buffer_size, void* block, br_tv_template* template);
br_error C2_HOOK_CDECL BrTokenValueQueryAll(br_token_value* buffer, br_size_t buffer_size, void* block, br_tv_template* template) {
#if 0//defined(C2_HOOKS_ENABLED)
    return BrTokenValueQueryAll_original(buffer, buffer_size, block, template);
#else
    br_tv_template_entry* tp;
    int n;
    int e;
    br_token_value* tv;
    char* extra;
    br_size_t extra_size;
    br_error r;

    if (!template->names_resolved) {
        templateResolveNames(template);
    }

    n = 1;
    for (e = 0; e < template->n_entries; e++) {
        tp = &template->entries[e];
        if (tp->token != BR_NULL_TOKEN && (tp->flags & 0x4)) {
            n++;
        }
    }
    extra = (char *)(buffer + n);
    if (buffer_size < sizeof(*buffer) * n) {
        return 0x1004;
    }
    tv = buffer;
    extra_size = buffer_size - sizeof(*buffer) * n;
    for (e = 0; e < template->n_entries; e++) {
        tp = &template->entries[e];
        if (tp->token == BR_NULL_TOKEN) {
            continue;
        }
        if ((tp->flags & 4) == 0) {
            continue;
        }
        tv->t = tp->token;
        r = ValueQuery(tv, (void*)&extra, &extra_size, block, tp);
        if (r != 0) {
            return r;
        }
        tv++;
    }
    tv->t = BR_NULL_TOKEN;
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052e430, BrTokenValueQueryAll, BrTokenValueQueryAll_original)

br_error (C2_HOOK_CDECL * BrTokenValueQueryAllSize_original)(br_size_t* psize, void* block, br_tv_template* template);
br_error C2_HOOK_CDECL BrTokenValueQueryAllSize(br_size_t* psize, void* block, br_tv_template* template) {
#if 0//defined(C2_HOOKS_ENABLED)
    return BrTokenValueQueryAllSize_original(psize, block, template);
#else
    br_tv_template_entry* tp;
    int e;
    int n;
    br_size_t extra_size;

    if (!template->names_resolved) {
        templateResolveNames(template);
    }
    extra_size = 0;
    n = 1;
    for (e = 0; e < template->n_entries; e++) {
        tp = &template->entries[e];
        if (tp->token == BR_NULL_TOKEN) {
            continue;
        }
        if ((tp->flags & 4) == 0) {
            continue;
        }
        n++;
        extra_size += ValueExtraSize(block, tp);
    }
    *psize = extra_size + sizeof(br_token_value) * n;
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052e510, BrTokenValueQueryAllSize, BrTokenValueQueryAllSize_original)

br_error C2_HOOK_CDECL BrTokenValueSet(void* mem, br_uint_32* pcombined_mask, br_token t, br_uint_32 value, br_tv_template* template) {
    br_int_32 o;
    br_error r;
    br_token_value tv;

    tv.t = t;
    tv.v.u32 = value;
    if (template->n_map_entries == 0) {
        templateMakeMap(template);
    }
    o = t - template->map_base;
    if (o < 0 || o >= template->n_map_entries) {
        return 0x1001;
    }

    if (template->map_set_entry[o] != NULL)  {
        r = ValueSet(mem, &tv, template->map_set_entry[o]);
        if (r != 0) {
            return r;
        }
        *pcombined_mask |= template->map_set_entry[o]->mask;
        return 0;
    } else {
        return 0x1001;
    }
}
C2_HOOK_FUNCTION(0x0052e5b0, BrTokenValueSet)

br_error C2_HOOK_CDECL BrTokenValueSetMany(void* mem, br_int_32* pcount, br_uint_32* pcombined_mask, br_token_value* tv, br_tv_template* template) {
    br_int_32 o;
    br_int_32 n;
    br_error r;
    br_uint_32 cm;

    if (template->n_map_entries == 0) {
        templateMakeMap(template);
    }
    cm = 0;
    n = 0;
    for (; tv->t != 0; tv++) {
        o = tv->t - template->map_base;
        if (o < 0 || o >= template->n_map_entries) {
            continue;
        }
        if (template->map_set_entry[o])  {
            r = ValueSet(mem, tv, template->map_set_entry[o]);
            if (r != 0) {
                continue;
            }
            cm |= template->map_set_entry[o]->mask;
            n++;
        }
    }
    if (pcount != NULL) {
        *pcount = n;
    }
    if (pcombined_mask != NULL) {
        *pcombined_mask |= cm;
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0052e940, BrTokenValueSetMany)

#if 0
void C2_HOOK_STDCALL DumpMatrixInteger(br_int_32* ip, int rows, int cols, char* prefix, char* info_0, char* info_n, br_putline_cbfn* putline, void* arg) {
    int i;
    int j;
    char* cp;
    char value[128];
#error "Not implemented"
}

void C2_HOOK_STDCALLDumpMatrixFixed(br_fixed_ls* xp, int rows, int cols, char* prefix, char* info_0, char* info_n, br_putline_cbfn* putline, void* arg) {
    int i;
    int j;
    char* cp;
    char value[128];
#error "Not implemented"
}

void C2_HOOK_STDCALL DumpMatrixFloat(br_float* fp, int rows, int cols, char* prefix, char* info_0, char* info_n, br_putline_cbfn* putline, void* arg) {
    int i;
    int j;
    char* cp;
    char value[128];
#error "Not implemented";
}

void DumpObject(br_object* h, char* prefix, char* info, br_putline_cbfn* putline, void* arg) {
    char value[128];
    char* dev_ident;
    char* h_ident;
    br_object* dev;
#error "Not implemented"
}
#endif

void C2_HOOK_CDECL BrTokenValueDump(br_token_value* tv, char* prefix, br_putline_cbfn* putline, void* arg) {
#if 0
    char* id;
    char value[128];
    char tmp[128];
    char info_0[64];
    char* info_n;
    char* pvalue;
    int n;
    int i;
    br_object** pph;
    void** ppp;
    br_token* pt;
#endif
}
C2_HOOK_FUNCTION(0x0052e9e0, BrTokenValueDump)

br_error (C2_HOOK_CDECL * BrStringToTokenValue_original)(br_token_value* buffer, br_size_t buffer_size, char* str);
br_error C2_HOOK_CDECL BrStringToTokenValue(br_token_value* buffer, br_size_t buffer_size, char* str) {
#if defined(C2_HOOKS_ENABLED)
    return BrStringToTokenValue_original(buffer, buffer_size, str);
#else
    br_lexer* l;
    br_error r;
    static br_lexer_keyword keywords[4];
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052e9f0, BrStringToTokenValue, BrStringToTokenValue_original)

#if 0
br_error C2_HOOK_STDCALL parseTokenValue(br_lexer* l, br_token_value* tv, br_size_t size) {
    int len;
    char name[40];
    br_token type;
    static br_token real_types[2];
    br_error r;
#error "Not implemented"
}
#endif

br_boolean (C2_HOOK_CDECL * BrTokenValueCompare_original)(br_token_value* tv1, br_token_value* tv2);
br_boolean C2_HOOK_CDECL BrTokenValueCompare(br_token_value* tv1, br_token_value* tv2) {
#if defined(C2_HOOKS_ENABLED)
    return BrTokenValueCompare_original
    (tv1, tv2);
#else
    int i;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052ee40, BrTokenValueCompare, BrTokenValueCompare_original)

#if 0
br_error C2_HOOK_CDECl BrTokenValueDup(br_token_value** dstp, br_token_value* src) {
    int i;
    int c;
    br_token_value* tv;
#error "Not implemented"
}
#endif
