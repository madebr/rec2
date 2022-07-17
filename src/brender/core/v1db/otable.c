#include "otable.h"

#include "custsupt.h"
#include "dbsetup.h"

#include "core/std/brstdlib.h"
#include "core/fw/resource.h"

br_order_table* C2_HOOK_CDECL BrZsOrderTableAllocate(br_uint_16 size, br_uint_32 flags, br_uint_16 type) {
    br_order_table* order_table;

    C2_HOOK_BUG_ON(sizeof(br_order_table) != 36);

    order_table = BrResAllocate(C2V(v1db).res, sizeof(br_order_table), BR_MEMORY_ORDER_TABLE);
    order_table->table = BrResAllocate(order_table, size * sizeof(br_primitive*), BR_MEMORY_ORDER_TABLE);
    order_table->size = size;
    order_table->next = NULL;
    order_table->visits = 0;
    order_table->min_z = 0.f;
    order_table->max_z = 0.f;
    order_table->sort_z = 0.f;
    order_table->scale = 0.f;
    order_table->flags = flags;
    order_table->type = type;
    return order_table;
}
C2_HOOK_FUNCTION(0x00526300, BrZsOrderTableAllocate)

void C2_HOOK_CDECL BrZsOrderTableFree(br_order_table* order_table) {

    BrResFree(order_table);
}
C2_HOOK_FUNCTION(0x00526370, BrZsOrderTableFree)

br_order_table* C2_HOOK_CDECL BrZsActorOrderTableSet(br_actor* actor, br_order_table* order_table) {

    actor->render_data = order_table;
    return order_table;
}
C2_HOOK_FUNCTION(0x00526380, BrZsActorOrderTableSet)

br_order_table* C2_HOOK_CDECL BrZsActorOrderTableGet(br_actor* actor) {

    return actor->render_data;
}
C2_HOOK_FUNCTION(0x00526390, BrZsActorOrderTableGet)

br_order_table* C2_HOOK_CDECL BrZsOrderTableClear(br_order_table* order_table) {

    BrMemSet(order_table->table, 0, order_table->size * sizeof(br_primitive*));
    return order_table;
}
C2_HOOK_FUNCTION(0x005263a0, BrZsOrderTableClear)

void C2_HOOK_CDECL BrZsOrderTablePrimitiveInsert(br_order_table* order_table, br_primitive* primitive, br_uint_16 bucket) {

    primitive->next = order_table->table[bucket];
    order_table->table[bucket] = primitive;
}
C2_HOOK_FUNCTION(0x005263c0, BrZsOrderTablePrimitiveInsert)

br_uint_16 (C2_HOOK_CDECL * BrZsPrimitiveBucketSelect_original)(br_scalar* z, br_uint_16 type, br_scalar min_z, br_scalar max_z, br_uint_16 size, br_uint_16 sort_type);
br_uint_16 C2_HOOK_CDECL BrZsPrimitiveBucketSelect(br_scalar* z, br_uint_16 type, br_scalar min_z, br_scalar max_z, br_uint_16 size, br_uint_16 sort_type) {
#if defined(C2_HOOKS_ENABLED)
    return BrZsPrimitiveBucketSelect_original(z, type, min_z, max_z, size, sort_type);
#else
    br_uint_16 bucket;
    br_scalar zprim;
    br_scalar range;
    br_scalar scale;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005263f0, BrZsPrimitiveBucketSelect, BrZsPrimitiveBucketSelect_original)

void C2_HOOK_CDECL BrZsOrderTablePrimaryEnable(br_order_table* order_table) {

    if (order_table == NULL) {
        order_table = C2V(v1db).default_order_table;
    }
    C2V(v1db).primary_order_table = order_table;
}
C2_HOOK_FUNCTION(0x00526560, BrZsOrderTablePrimaryEnable)

void C2_HOOK_CDECL BrZsOrderTablePrimaryDisable(void) {

    C2V(v1db).primary_order_table = NULL;
}
C2_HOOK_FUNCTION(0x00526580, BrZsOrderTablePrimaryDisable)

void C2_HOOK_STDCALL InsertOrderTableList(br_order_table* order_table) {
    br_order_table* previous_table;
    br_order_table* current_table;

    if (order_table == C2V(v1db).primary_order_table) {
        return;
    }

    if (C2V(v1db).order_table_list == NULL) {
        C2V(v1db).order_table_list = order_table;
        order_table->next = NULL;
        return;
    }
    if (C2V(v1db).order_table_list->sort_z < order_table->sort_z) {
        order_table->next = C2V(v1db).order_table_list;
        C2V(v1db).order_table_list = order_table;
        return;
    }
    previous_table = C2V(v1db).order_table_list;
    current_table = C2V(v1db).order_table_list->next;
    while (current_table != NULL && order_table->sort_z <= current_table->sort_z) {
        previous_table = current_table;
        current_table = current_table->next;
    }
    previous_table->next = order_table;
    order_table->next = current_table;
}
C2_HOOK_FUNCTION(0x00526590, InsertOrderTableList)

void C2_HOOK_STDCALL SetOrderTableBounds(br_bounds* bounds, br_order_table* order_table) {
    br_uint_32 i;
    br_scalar element;
    br_scalar min_z;
    br_scalar max_z;
    br_vector3* min;
    br_vector3* max;

    if((order_table->flags & 0x1) || ((order_table->flags & 0x2) && order_table->visits==1)) {
        min = &bounds->min;
        max = &bounds->max;
        BrModelToScreenQuery(&C2V(v1db).model_to_screen);
        min_z = C2V(v1db).model_to_screen.m[3][3];
        max_z = C2V(v1db).model_to_screen.m[3][3];

        for (i = 0; i < 3; i++) {
            element = C2V(v1db).model_to_screen.m[i][3];
            if (element > 0.f) {
                max_z += element * max->v[i];
                min_z += element * min->v[i];
            }
            else {
                max_z += element * min->v[i];
                min_z += element * max->v[i];
            }
        }
        order_table->min_z=min_z;
        order_table->max_z=max_z;
    }
    SetOrderTableRange(order_table);
}
C2_HOOK_FUNCTION(0x00526600, SetOrderTableBounds)

void C2_HOOK_STDCALL SetOrderTableRange(br_order_table* order_table) {
    br_scalar range;

    range = order_table->max_z - order_table->min_z;
    if (range > .001f) {
        order_table->scale = ((float) order_table->size) / range;
    } else {
        order_table->scale = 1 / .001f;
    }
    if ((order_table->flags & 0x8) != 0) {
        order_table->sort_z = order_table->min_z;
    } else {
        if ((order_table->flags & 0x10) != 0){
            order_table->sort_z = order_table->max_z;
        } else {
            if ((order_table->flags & 0x20) != 0)
                order_table->sort_z = order_table->min_z + .5f * range;
        }
    }
}
C2_HOOK_FUNCTION(0x005266f0, SetOrderTableRange)

void C2_HOOK_STDCALL RenderOrderTableList(void) {
    br_order_table* order_table;

    order_table = C2V(v1db).order_table_list;
    while (order_table != NULL) {
        ((br_geometry_v1_buckets*)C2V(v1db).format_buckets)->dispatch->_render((br_geometry_v1_buckets *)C2V(v1db).format_buckets, C2V(v1db).renderer, order_table->table, order_table->size);
        order_table->visits = 0;
        order_table = order_table->next;
    }
}
C2_HOOK_FUNCTION(0x00526770, RenderOrderTableList)

void C2_HOOK_STDCALL RenderPrimaryOrderTable(void) {
    br_uint_16 m;
    br_uint_16 size;
    br_scalar bucket_size;
    br_scalar min_z;
    br_scalar max_z;
    br_primitive** bucket;
    br_order_table* order_table;

    if (C2V(v1db).primary_order_table->visits == 0) {
        RenderOrderTableList();
        return;
    }

    min_z = C2V(v1db).primary_order_table->min_z;
    max_z = C2V(v1db).primary_order_table->max_z;
    size = C2V(v1db).primary_order_table->size;
    bucket = C2V(v1db).primary_order_table->table + (size - 1);

    order_table = C2V(v1db).order_table_list;
    while (order_table!=NULL) {
        if (order_table->sort_z < max_z) {
            break;
        }
        ((br_geometry_v1_buckets*)C2V(v1db).format_buckets)->dispatch->_render((br_geometry_v1_buckets *)C2V(v1db).format_buckets, C2V(v1db).renderer, order_table->table, order_table->size);
        order_table->visits = 0;
        order_table=order_table->next;
    }
    bucket_size = (float)(max_z - min_z) / (float)size;

    for (m = 0; m < size; m++, bucket--) {
        if (*bucket != NULL) {
            ((br_geometry_v1_buckets*)C2V(v1db).format_buckets)->dispatch->_render((br_geometry_v1_buckets *)C2V(v1db).format_buckets, C2V(v1db).renderer, bucket, 1);
        }
        max_z-=bucket_size;
        while(order_table!=NULL) {
            if(order_table->sort_z < max_z) {
                break;
            }
            ((br_geometry_v1_buckets*)C2V(v1db).format_buckets)->dispatch->_render((br_geometry_v1_buckets *)C2V(v1db).format_buckets, C2V(v1db).renderer, order_table->table, order_table->size);
            order_table->visits = 0;
            order_table=order_table->next;
        }
    }
    while (order_table!=NULL) {
        ((br_geometry_v1_buckets*)C2V(v1db).format_buckets)->dispatch->_render((br_geometry_v1_buckets *)C2V(v1db).format_buckets, C2V(v1db).renderer, order_table->table, order_table->size);
        order_table->visits = 0;
        order_table = order_table->next;
    }
}
C2_HOOK_FUNCTION(0x005267b0, RenderPrimaryOrderTable)
