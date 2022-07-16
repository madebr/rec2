#ifndef REC2_TOKENVAL_H
#define REC2_TOKENVAL_H

#include "c2_hooks.h"

#include "brender/br_types.h"

br_tv_template* C2_HOOK_CDECL BrTVTemplateAllocate(void* res, br_tv_template_entry* entries, int n_entries);

void C2_HOOK_CDECL BrTVTemplateFree(br_tv_template* t);

void C2_HOOK_STDCALL templateResolveNames(br_tv_template* template);

void C2_HOOK_STDCALL templateMakeMap(br_tv_template* template);

br_fixed_ls* C2_HOOK_STDCALL ConvertFloatToFixed(br_fixed_ls** pextra, br_float* src, br_int_32 count, br_size_t* pextra_space);

br_float* C2_HOOK_STDCALL ConvertFixedToFloat(br_float** pextra, br_fixed_ls* src, br_int_32 count, br_size_t* pextra_space);

br_uint_32* C2_HOOK_STDCALL ConvertLongCopy(br_uint_32** pextra, br_uint_32* src, br_int_32 count, br_size_t* pextra_space);

br_error C2_HOOK_STDCALL ValueQuery(br_token_value* tv, void** pextra, br_size_t* pextra_size, void* block, br_tv_template_entry* tep);

br_error C2_HOOK_STDCALL ValueSet(void* block, br_token_value* tv, br_tv_template_entry* tep);

br_size_t C2_HOOK_STDCALL ValueExtraSize(void* block, br_tv_template_entry* tep);

br_error C2_HOOK_CDECL BrTokenValueQuery(br_uint_32* pvalue, br_uint_32* extra, br_size_t extra_size, br_token t, void* block, br_tv_template* template);

br_error C2_HOOK_CDECL BrTokenValueQueryMany(br_token_value* tv, void* extra, br_size_t extra_size, br_int_32* pcount, void* block, br_tv_template* template);

br_error C2_HOOK_CDECL BrTokenValueQueryManySize(br_size_t* psize, br_token_value* tv, void* block, br_tv_template* template);

br_error C2_HOOK_CDECL BrTokenValueQueryAll(br_token_value* buffer, br_size_t buffer_size, void* block, br_tv_template* template);

br_error C2_HOOK_CDECL BrTokenValueQueryAllSize(br_size_t* psize, void* block, br_tv_template* template);

br_error C2_HOOK_CDECL BrTokenValueSet(void* mem, br_uint_32* pcombined_mask, br_token t, br_uint_32 value, br_tv_template* template);

br_error C2_HOOK_CDECL BrTokenValueSetMany(void* mem, br_int_32* pcount, br_uint_32* pcombined_mask, br_token_value* tv, br_tv_template* template);

void C2_HOOK_STDCALL DumpMatrixInteger(br_int_32* ip, int rows, int cols, char* prefix, char* info_0, char* info_n, br_putline_cbfn* putline, void* arg);

void C2_HOOK_STDCALL DumpMatrixFixed(br_fixed_ls* xp, int rows, int cols, char* prefix, char* info_0, char* info_n, br_putline_cbfn* putline, void* arg);

void C2_HOOK_STDCALL DumpMatrixFloat(br_float* fp, int rows, int cols, char* prefix, char* info_0, char* info_n, br_putline_cbfn* putline, void* arg);

void C2_HOOK_STDCALL DumpObject(br_object* h, char* prefix, char* info, br_putline_cbfn* putline, void* arg);

void C2_HOOK_CDECL BrTokenValueDump(br_token_value* tv, char* prefix, br_putline_cbfn* putline, void* arg);

br_error C2_HOOK_CDECL BrStringToTokenValue(br_token_value* buffer, br_size_t buffer_size, char* str);

br_error C2_HOOK_STDCALL parseTokenValue(br_lexer* l, br_token_value* tv, br_size_t size);

br_boolean C2_HOOK_CDECL BrTokenValueCompare(br_token_value* tv1, br_token_value* tv2);

br_error C2_HOOK_CDECL BrTokenValueDup(br_token_value** dstp, br_token_value* src);

#endif // REC2_TOKENVAL_H
