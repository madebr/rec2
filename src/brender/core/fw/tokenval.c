#include "tokenval.h"

#include "lexer.h"
#include "resource.h"
#include "token.h"

#include "core/std/brstdlib.h"

#include "c2_stdlib.h"

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

br_fixed_ls* C2_HOOK_STDCALL ConvertFloatToFixed(br_fixed_ls** pextra, br_float* src, br_int_32 count, br_size_t* pextra_space) {
    br_fixed_ls* ret;

    if (pextra == NULL || *pextra == NULL || pextra_space == NULL) {
        return NULL;
    }
    ret = *pextra;
    if (*pextra_space < sizeof(uint32_t) * count) {
        return 0;
    }
    *pextra_space -= sizeof(br_fixed_ls) * count;
    for (; count != 0; count--) {
        **pextra = (br_fixed_ls)(*src * 65536.f);
        (*pextra)++;
        src++;
    }
    return ret;
}

br_float* C2_HOOK_STDCALL ConvertFixedToFloat(br_float** pextra, br_fixed_ls* src, br_int_32 count, br_size_t* pextra_space) {
    br_float* ret;

    if (pextra == NULL || *pextra == NULL || pextra_space == NULL) {
        return NULL;
    }
    ret = *pextra;
    if (*pextra_space < sizeof(float) * count) {
        return 0;
    }
    *pextra_space -= sizeof(float) * count;
    for (; count != 0; count--) {
        **pextra = (float)*src / 65536.f;
        (*pextra)++;
        src++;
    }
    return ret;
}

br_uint_32* C2_HOOK_STDCALL ConvertLongCopy(br_uint_32** pextra, br_uint_32* src, br_int_32 count, br_size_t* pextra_space) {
    br_uint_32* ret;

    if (pextra == NULL || *pextra == NULL || pextra_space == NULL) {
        return NULL;
    }
    ret = *pextra;
    if (*pextra_space < sizeof(uint32_t) * count) {
        return NULL;
    }
    *pextra_space -= sizeof(uint32_t) * count;
    for (; count != 0; count--) {
        **pextra = *src;
        (*pextra)++;
        src++;
    }
    return ret;
}

br_error (C2_HOOK_STDCALL * ValueQuery_original)(br_token_value* tv, void** pextra, br_size_t* pextra_size, void* block, br_tv_template_entry* tep);
br_error C2_HOOK_STDCALL ValueQuery(br_token_value* tv, void** pextra, br_size_t* pextra_size, void* block, br_tv_template_entry* tep) {
#if 0//defined(C2_HOOKS_ENABLED)
    return ValueQuery_original(tv, pextra, pextra_size, block, tep);
#else
    void* mem;
    br_uint_32* lp;
    int t;
    br_tv_custom* custp;

    if (tep->flags & 0x8) {
        /* absolute */
        mem = &tep->offset;
    } else {
        /* relative */
        mem = (br_uint_8*)block + tep->offset;
    }

    switch (tep->conv) {
    case 1:
        /* const */
        tv->v.u32 = (br_uint_32)tep->conv_arg;
        break;
    case 2:
        /* custom */
        custp = (br_tv_custom*)tep->conv_arg;
        if (custp == NULL || custp->query == NULL) {
            return 0x1001;
        }
        return custp->query(&tv->v.u32, pextra, pextra_size, block, tep);
    case 3:
        /* void* */
        tv->v.u32 = *(br_uint_32*)mem;
        break;
    case 4:
        /* vec2 (default=float) */
        tv->v.p = ConvertLongCopy((br_uint_32**)pextra, (br_uint_32*)mem, 2, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 5:
        /* vec3 (default=float) */
        tv->v.p = ConvertLongCopy((br_uint_32**)pextra, (br_uint_32*)mem, 3, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 6:
        /* vec4 (default=float) */
        tv->v.p = ConvertLongCopy((br_uint_32**)pextra, (br_uint_32*)mem, 4, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 7:
        /* mat23 (default=float) */
        tv->v.p = ConvertLongCopy((br_uint_32**)pextra, (br_uint_32*)mem, 6, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 8:
        /* mat34 (default=float) */
        tv->v.p = ConvertLongCopy((br_uint_32**)pextra, (br_uint_32*)mem, 12, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 9:
        /* mat4 (default=float) */
        tv->v.p = ConvertLongCopy((br_uint_32**)pextra, (br_uint_32*)mem, 16, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 10:
        /* i8 */
        tv->v.i8 = *(br_int_8*)mem;
        break;
    case 11:
        /* i16 */
        tv->v.i16 = *(br_int_16*)mem;
        break;
    case 12:
        /* u8 */
        tv->v.u8 = *(br_uint_8*)mem;
        break;
    case 13:
        /* u16 */
        tv->v.u16 = *(br_uint_16*)mem;
        break;
    case 14:
        /* fixed */
        tv->v.x = (br_fixed_ls)(65536.f * *(float*)mem);
        break;
    case 15:
        /* float */
        tv->v.s = (br_fixed_ls)*(br_fixed_ls*)mem / 65536.f;
        break;
    case 16:
        /* vec2f_to_x */
        tv->v.p = ConvertFloatToFixed((br_fixed_ls**)pextra, (float*)mem, 2, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 17:
        /* vec2x_to_f */
        tv->v.p = ConvertFixedToFloat((float**)pextra, (br_fixed_ls*)mem, 2, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 18:
        /* vec3f_to_x */
        tv->v.p = ConvertFloatToFixed((br_fixed_ls**)pextra, (float*)mem, 3, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 19:
        /* vec3x_to_f */
        tv->v.p = ConvertFixedToFloat((float**)pextra, (br_fixed_ls*)mem, 3, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 20:
        /* vec4f_to_x */
        tv->v.p = ConvertFloatToFixed((br_fixed_ls**)pextra, (float*)mem, 4, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 21:
        /* vec4x_to_f */
        tv->v.p = ConvertFixedToFloat((float**)pextra, (br_fixed_ls*)mem, 4, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 22:
        /* mat23f_to_x */
        tv->v.p = ConvertFloatToFixed((br_fixed_ls**)pextra, (float*)mem, 6, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 23:
        /* mat23x_to_f */
        tv->v.p = ConvertFixedToFloat((float**)pextra, (br_fixed_ls*)mem, 6, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 24:
        /* mat34f_to_x */
        tv->v.p = ConvertFloatToFixed((br_fixed_ls**)pextra, (float*)mem, 12, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 25:
        /* mat34x_to_f */
        tv->v.p = ConvertFixedToFloat((float**)pextra, (br_fixed_ls*)mem, 12, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 26:
        /* mat4f_to_x */
        tv->v.p = ConvertFloatToFixed((br_fixed_ls**)pextra, (float*)mem, 16, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 27:
        /* mat4x_to_f */
        tv->v.p = ConvertFixedToFloat((float**)pextra, (br_fixed_ls*)mem, 16, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 28:
        /* string */
        tv->v.cstr = (char*)ConvertLongCopy((br_uint_32**)pextra, (br_uint_32*)*(char**)mem, (BrStrLen(*(const char**)mem) + 1 + sizeof(br_uint_32)) / sizeof(br_uint_32), pextra_size);
        if (tv->v.cstr == NULL) {
            return 0x1004;
        }
        break;
    case 29:
        /* null terminated array */
        lp = *(br_uint_32**)mem;
        t = 0;
        if (lp != NULL) {
            while (*lp++) {
                t++;
            }
            tv->v.p = ConvertLongCopy((br_uint_32**)pextra, *(br_uint_32**)mem, t + 1, pextra_size);
            if (tv->v.p == NULL) {
                return 0x1004;
            }
        } else {
            tv->v.p = ConvertLongCopy((br_uint_32**)pextra, (br_uint_32*)&mem, 1, pextra_size);
            if (tv->v.p == NULL) {
                return 0x1004;
            }
        }
        break;
    case 30:
        /* Invert Boolean */
        tv->v.b = (*(br_uint_32*)mem) != 0x1;
        break;
    case 31:
        /* Boolean mask */
        tv->v.b = !!((*(br_uint_32*)mem) & tep->conv_arg);
        break;
    case 32:
        /* Boolean not mask */
        tv->v.b = !((*(br_uint_32*)mem) & tep->conv_arg);
        break;
    default:
        return 0x1001;
    }
    return 0;
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
    if (template->map_set_entry[o] == NULL) {
        return 0x1001;
    }
    r = ValueSet(mem, &tv, template->map_set_entry[o]);
    if (r != 0) {
        return r;
    }
    *pcombined_mask |= template->map_set_entry[o]->mask;
    return 0;
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

#define T_FALSE (T_KEYWORD + 0)
#define T_TRUE  (T_KEYWORD + 1)

br_error (C2_HOOK_CDECL * BrStringToTokenValue_original)(br_token_value* buffer, br_size_t buffer_size, char* str);
br_error C2_HOOK_CDECL BrStringToTokenValue(br_token_value* buffer, br_size_t buffer_size, char* str) {
#if 0//defined(C2_HOOKS_ENABLED)
    return BrStringToTokenValue_original(buffer, buffer_size, str);
#else
    br_lexer* l;
    br_error r;
    static br_lexer_keyword keywords[4] = {
        { "true",   T_TRUE  },
        { "t",      T_TRUE  },
        { "false",  T_FALSE },
        { "f",      T_FALSE },
    };

    l = BrLexerAllocate(keywords, BR_ASIZE(keywords));
    if (l == NULL) {
        return 0x1003;
    }
    r = BrLexerPushString(l, str, NULL);
    if (r != 0) {
        BrLexerFree(l);
        return r;
    }
    l->advance(l);
    parseTokenValue(l, buffer, buffer_size);
    BrLexerFree(l);
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052e9f0, BrStringToTokenValue, BrStringToTokenValue_original)

br_error C2_HOOK_STDCALL parseTokenValue(br_lexer* l, br_token_value* tv, br_size_t size) {
    int len;
    char name[40];
    br_token type;
    br_error r;

    if (size < sizeof(br_token_value)) {
        return 0x1004;
    }

    size -= sizeof(br_token_value);

    while (l->current.id != T_EOF) {
        while (l->current.id == T_COMMA) {
            l->advance(l);
        }
        if (l->current.id != T_IDENT) {
            break;
        }
        if (size <= sizeof(br_token_value)) {
            r = 0x1004;
            break;
        }
        name[BR_ASIZE(name) - 1] = '\0';
        BrStrNCpy(name, l->current.v.string, BR_ASIZE(name) - 1);
        l->advance(l);
        if (name[1] == '\0') {
            if (name[0] == 'w' || name [0] == 'W') {
                BrStrCpy(name, "width");
            } else if(name[0] == 'h' || name [0] == 'H') {
                BrStrCpy(name, "height");
            } else if(name[0] == 'b' || name [0] == 'B') {
                BrStrCpy(name, "pixel_bits");
            } else if(name[0] == 'm' || name [0] == 'M') {
                BrStrCpy(name, "mode");
            }
        }
        tv->t = BR_NULL_TOKEN;

        if (l->current.id == T_COLON || l->current.id == T_EQUAL) {
            l->advance(l);

            switch ((int)l->current.id) {
            case T_IDENT: {
                static br_token ident_types[] = { BRT_TOKEN };
                tv->t = BrTokenFindType(&type, name, ident_types, BR_ASIZE(ident_types));
                if (tv->t == BR_NULL_TOKEN) {
                    break;
                }
                tv->v.t = BrTokenFind(l->current.v.string);
                if (tv->v.t == BR_NULL_TOKEN) {
                    tv->t = BR_NULL_TOKEN;
                }
                break;
            }
            case T_STRING: {
                static br_token string_types[] = { BRT_STRING, BRT_CONSTANT_STRING };
                tv->t = BrTokenFindType(&type, name, string_types, BR_ASIZE(string_types));
                if (tv->t == BR_NULL_TOKEN) {
                    break;
                }
                len = BrStrLen(l->current.v.string) + 1;
                if (len > (int)size) {
                    tv->t = BR_NULL_TOKEN;
                    r = 0x1004;
                    break;
                }
                size -= len;
                tv->v.str = (char *)tv + size;
                BrMemCpy(tv->v.str, l->current.v.string, len);
                break;
            }
            case T_INTEGER: {
                static br_token pos_int_types[] = {
                    BRT_INT_32, BRT_UINT_32, BRT_INT_16,
                    BRT_UINT_16, BRT_INT_8, BRT_UINT_8,
                    BRT_FLOAT, BRT_FIXED, BRT_BOOLEAN };
                static br_token neg_int_types[] = {
                    BRT_INT_32, BRT_INT_16, BRT_INT_8,
                    BRT_FLOAT, BRT_FIXED, BRT_BOOLEAN };

                if (l->current.v.integer < 0) {
                    tv->t = BrTokenFindType(&type, name, neg_int_types, BR_ASIZE(neg_int_types));
                } else {
                    tv->t = BrTokenFindType(&type, name, pos_int_types, BR_ASIZE(pos_int_types));
                }
                if (tv->t == BR_NULL_TOKEN) {
                    break;
                }
                switch (type) {
                case BRT_BOOLEAN:
                    tv->v.b = l->current.v.integer != 0;
                    break;
                case BRT_INT_8:
                    tv->v.i8 = (br_int_8)l->current.v.integer;
                    break;
                case BRT_UINT_8:
                    tv->v.u8 = (br_uint_8)l->current.v.integer;
                    break;
                case BRT_INT_16:
                    tv->v.i16 = (br_int_16)l->current.v.integer;
                    break;
                case BRT_UINT_16:
                    tv->v.u16 = (br_uint_16)l->current.v.integer;
                    break;
                case BRT_INT_32:
                    tv->v.i32 = l->current.v.integer;
                    break;
                case BRT_UINT_32:
                    tv->v.u32 = (br_uint_32)l->current.v.integer;
                    break;
                case BRT_FIXED:
                    tv->v.x = l->current.v.integer << 16;
                    break;
                case BRT_FLOAT:
                    tv->v.f = (float)l->current.v.integer;
                    break;
                default:
                    c2_abort();
                    break;
                }
                break;
            }
            case T_REAL: {
                static br_token real_types[] = { BRT_FLOAT, BRT_FIXED };

                tv->t = BrTokenFindType(&type, name, real_types, BR_ASIZE(real_types));
                if (tv->t == BR_NULL_TOKEN) {
                    break;
                }
                switch (type) {
                case BRT_FIXED:
                    tv->v.x = (br_fixed_ls)(l->current.v.real * 65536.f);
                    break;
                case BRT_FLOAT:
                    tv->v.f = l->current.v.real;
                    break;
                default:
                    c2_abort();
                    break;
                }
                break;
            }
            case T_FALSE:
            case T_TRUE: {
                static br_token bool_types[] = { BRT_BOOLEAN };

                tv->t = BrTokenFindType(&type, name, bool_types, BR_ASIZE(bool_types));
                if (tv->t == BR_NULL_TOKEN) {
                    break;
                }
                tv->v.b = l->current.id == T_TRUE;;
                break;
            }
            default:
                l->error(l, "expecting a value");
                break;
            }
            l->advance(l);
        } else {
            static br_token none_types[] = { BRT_NONE, BRT_BOOLEAN };

            tv->t = BrTokenFindType(&type, name, none_types, BR_ASIZE(none_types));
            if (tv->t != BR_NULL_TOKEN && type == BRT_BOOLEAN) {
                tv->v.b = T_TRUE;
            }
        }

        if (tv->t != BR_NULL_TOKEN) {
            tv++;
            size -= sizeof(br_token_value);
        } else {
            l->error(l, "unknown token");
        }
    }
    tv->t = BR_NULL_TOKEN;
    tv->v.u32 = 0;
    return r;
}

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
