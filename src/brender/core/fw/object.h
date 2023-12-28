#ifndef REC2_OBJECT_H
#define REC2_OBJECT_H

#include "brender/br_types.h"

br_error C2_HOOK_CDECL _M_br_object_query(br_object* self, br_uint_32* pvalue, br_token t);

br_error C2_HOOK_CDECL _M_br_object_queryBuffer(br_object* self, br_uint_32* pvalue, br_uint_32* buffer, br_size_t buffer_size, br_token t);

br_error C2_HOOK_CDECL _M_br_object_queryMany(br_object* self, br_token_value* tv, void* extra, br_size_t extra_size, br_int_32* pcount);

br_error C2_HOOK_CDECL _M_br_object_queryManySize(br_object* self, br_size_t* pextra_size, br_token_value* tv);

br_error C2_HOOK_CDECL _M_br_object_queryAll(br_object* self, br_token_value* buffer, br_size_t buffer_size);

br_error C2_HOOK_CDECL _M_br_object_queryAllSize(br_object* self, br_size_t* psize);

void C2_HOOK_CDECL _BrObjectFree(void* res, br_uint_8 res_class, br_size_t size);

#endif // REC2_OBJECT_H
