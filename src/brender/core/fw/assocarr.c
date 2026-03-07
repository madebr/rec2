#include "assocarr.h"

#include "fwsetup.h"
#include "mem.h"
#include "resource.h"
#include "token.h"

#include "core/std/brstdlib.h"

#include "c2_stdlib.h"

// FUNCTION: CARMA2_HW 0x00531500
br_associative_array* C2_HOOK_CDECL BrAssociativeArrayAllocate(void) {
    br_associative_array* pArray;

    pArray = BrResAllocate(fw.res, sizeof(br_associative_array), BR_MEMORY_SCRATCH);
    if (pArray == NULL) {
        return NULL;
    }
    pArray->tv = BrResAllocate(pArray, 10 * sizeof(br_token_value), BR_MEMORY_SCRATCH);
    if (pArray->tv == NULL) {
        return NULL;
    }
    pArray->max_elements = 10;
    pArray->num_elements = 0;
    return pArray;
}

br_error C2_HOOK_CDECL Set_Associative_Array_Value(br_associative_array* pArray, int index, br_value v) {

    if (BrTokenType(pArray->tv[index].t) == BRT_STRING) {
        if (pArray->tv[index].v.str != NULL) {
            BrResFree(pArray->tv[index].v.str);
        }
        if (v.str != NULL) {
            pArray->tv[index].v.str = BrResStrDup(pArray, v.str);
        } else {
            pArray->tv[index].v.str = NULL;
        }
    } else {
        pArray->tv[index].v = v;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00531540
br_error C2_HOOK_STDCALL BrAssociativeArraySetEntry(br_associative_array* pArray, br_token t, br_value v) {
    br_uint_16 i;
    br_token_value* temp;

    for (i = 0; i < pArray->num_elements; i++) {
        if (pArray->tv[i].t == t) {
            br_error res = Set_Associative_Array_Value(pArray, i, v);
            return res;
        }
    }
    if (pArray->num_elements < pArray->max_elements) {
        pArray->tv[pArray->num_elements].t = t;
        Set_Associative_Array_Value(pArray, pArray->num_elements, v);
        pArray->num_elements += 1;
        return 0;
    }
    temp = BrResAllocate(pArray, (pArray->max_elements + 10) * sizeof(br_token_value), BR_MEMORY_APPLICATION);
    BrMemCpy(temp, pArray->tv, pArray->max_elements * sizeof(br_token_value));
    BrResFree(pArray->tv);
    pArray->tv = temp;
    if (temp == NULL) {
        return 0x1002;
    }
    pArray->max_elements += 10;
    Set_Associative_Array_Value(pArray, pArray->num_elements, v);
    pArray->num_elements += 1;
    return 0;
}

// FUNCTION: CARMA2_HW 0x00531750
br_error C2_HOOK_STDCALL BrAssociativeArrayRemoveEntry(br_associative_array* pArray, br_token t) {
    br_uint_16 i;
    br_boolean bFound;

    bFound = 0;
    for (i = 0; i < pArray->num_elements; i++) {
        if (pArray->tv[i].t == t) {
            bFound = 1;
            break;
        }
    }
    if (bFound) {
        if ((BrTokenType(t) == BRT_STRING) && (pArray->tv[i].v.str != NULL)) {
            BrResFree(pArray->tv[i].v.str);
        }
        for (; i < pArray->num_elements - 1; i++) {
            BrMemCpy(&pArray->tv[i], &pArray->tv[i+1], sizeof(br_token_value));
        }
        pArray->num_elements--;
        return 0; // BRE_OK
    }
    return 0x1002; // BRE_NOTFOUND
}

// FUNCTION: CARMA2_HW 0x00531810
br_error C2_HOOK_STDCALL BrAssociativeArrayQuery(br_associative_array* pArray, br_token t, br_value* pValue) {
    br_uint_16 i;

    for (i = 0; i < pArray->num_elements; i++) {
        if (pArray->tv[i].t == t) {
            *pValue = pArray->tv[i].v;
            return 0;
        }
    }
    return 0x1002;
}
