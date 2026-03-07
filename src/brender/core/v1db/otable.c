#include "otable.h"

#include "custsupt.h"
#include "dbsetup.h"

#include "core/std/brstdlib.h"
#include "core/fw/resource.h"

#include "c2_stdlib.h"

// FUNCTION: CARMA2_HW 0x00526300
br_order_table* C2_HOOK_CDECL BrZsOrderTableAllocate(br_uint_16 size, br_uint_32 flags, br_uint_16 type) {
    br_order_table* order_table;

    C2_HOOK_BUG_ON(sizeof(br_order_table) != 36);

    order_table = BrResAllocate(v1db.res, sizeof(br_order_table), BR_MEMORY_ORDER_TABLE);
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

// FUNCTION: CARMA2_HW 0x00526370
void C2_HOOK_CDECL BrZsOrderTableFree(br_order_table* order_table) {

    BrResFree(order_table);
}

// FUNCTION: CARMA2_HW 0x00526380
br_order_table* C2_HOOK_CDECL BrZsActorOrderTableSet(br_actor* actor, br_order_table* order_table) {

    actor->render_data = order_table;
    return order_table;
}

// FUNCTION: CARMA2_HW 0x00526390
br_order_table* C2_HOOK_CDECL BrZsActorOrderTableGet(br_actor* actor) {

    return actor->render_data;
}

// FUNCTION: CARMA2_HW 0x005263a0
br_order_table* C2_HOOK_CDECL BrZsOrderTableClear(br_order_table* order_table) {

    BrMemSet(order_table->table, 0, order_table->size * sizeof(br_primitive*));
    return order_table;
}

// FUNCTION: CARMA2_HW 0x005263c0
void C2_HOOK_CDECL BrZsOrderTablePrimitiveInsert(br_order_table* order_table, br_primitive* primitive, br_uint_16 bucket) {

    primitive->next = order_table->table[bucket];
    order_table->table[bucket] = primitive;
}

// FUNCTION: CARMA2_HW 0x005263f0
br_uint_16 C2_HOOK_CDECL BrZsPrimitiveBucketSelect(br_scalar* z, br_uint_16 type, br_scalar min_z, br_scalar max_z, br_uint_16 size, br_uint_16 sort_type) {
    br_uint_16 bucket;
    br_scalar zprim;
    br_scalar range;
    br_scalar scale;

#ifdef BRENDER_FIX_BUGS
    zprim = 0.f;
#endif

    switch (type) {
    case BRT_POINT:
        zprim = z[0];
        break;
    case BRT_LINE:
        switch (sort_type) {
        case 1:
            zprim = (z[0] + z[1]) / 2.f;
            break;
        case 2:
            zprim = z[0];
            if (z[1] < zprim) {
                zprim = z[1];
            }
            break;
        case 3:
            zprim = z[0];
            if (zprim < z[1]) {
                zprim = z[1];
            }
            break;
        default:
            c2_abort();
        }
    case BRT_TRIANGLE:
        switch (sort_type) {
        case 1:
            zprim = (z[0] + z[1] + z[3]) * (1.f / 3.f);
            break;
        case 2:
            zprim = z[0];
            if (z[1] < zprim) {
                zprim = z[1];
            }
            if (z[2] < zprim) {
                zprim = z[2];
            }
            break;
        case 3:
            zprim = z[0];
            if (zprim < z[1]) {
                zprim = z[1];
            }
            if (zprim < z[2]) {
                zprim = z[2];
            }
            break;
        default:
            c2_abort();
        }
    default:
        c2_abort();
    }
    range = max_z - min_z;
    if (range < 0.001f) {
        scale = 1000.f;
    } else {
        scale = size / range;
    }
    zprim -= min_z;
    if (zprim < 0.f) {
        zprim = 0.f;
    }
    bucket = (br_uint_16)(scale * zprim);
    if (bucket >= size) {
        bucket = size - 1;
    }
    return bucket;
}

// FUNCTION: CARMA2_HW 0x00526560
void C2_HOOK_CDECL BrZsOrderTablePrimaryEnable(br_order_table* order_table) {

    if (order_table == NULL) {
        order_table = v1db.default_order_table;
    }
    v1db.primary_order_table = order_table;
}

// FUNCTION: CARMA2_HW 0x00526580
void C2_HOOK_CDECL BrZsOrderTablePrimaryDisable(void) {

    v1db.primary_order_table = NULL;
}

// FUNCTION: CARMA2_HW 0x00526590
void C2_HOOK_STDCALL InsertOrderTableList(br_order_table* order_table) {
    br_order_table* previous_table;
    br_order_table* current_table;

    if (order_table == v1db.primary_order_table) {
        return;
    }

    if (v1db.order_table_list == NULL) {
        v1db.order_table_list = order_table;
        order_table->next = NULL;
        return;
    }
    if (v1db.order_table_list->sort_z < order_table->sort_z) {
        order_table->next = v1db.order_table_list;
        v1db.order_table_list = order_table;
        return;
    }
    previous_table = v1db.order_table_list;
    current_table = v1db.order_table_list->next;
    while (current_table != NULL && order_table->sort_z <= current_table->sort_z) {
        previous_table = current_table;
        current_table = current_table->next;
    }
    previous_table->next = order_table;
    order_table->next = current_table;
}

// FUNCTION: CARMA2_HW 0x00526600
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
        BrModelToScreenQuery(&v1db.model_to_screen);
        min_z = v1db.model_to_screen.m[3][3];
        max_z = v1db.model_to_screen.m[3][3];

        for (i = 0; i < 3; i++) {
            element = v1db.model_to_screen.m[i][3];
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

// FUNCTION: CARMA2_HW 0x005266f0
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

// FUNCTION: CARMA2_HW 0x00526770
void C2_HOOK_STDCALL RenderOrderTableList(void) {
    br_order_table* order_table;

    order_table = v1db.order_table_list;
    while (order_table != NULL) {
        ((br_geometry_v1_buckets*)v1db.format_buckets)->dispatch->_render((br_geometry_v1_buckets *)v1db.format_buckets, v1db.renderer, order_table->table, order_table->size);
        order_table->visits = 0;
        order_table = order_table->next;
    }
}

// FUNCTION: CARMA2_HW 0x005267b0
void C2_HOOK_STDCALL RenderPrimaryOrderTable(void) {
    br_uint_16 m;
    br_uint_16 size;
    br_scalar bucket_size;
    br_scalar min_z;
    br_scalar max_z;
    br_primitive** bucket;
    br_order_table* order_table;

    if (v1db.primary_order_table->visits == 0) {
        RenderOrderTableList();
        return;
    }

    min_z = v1db.primary_order_table->min_z;
    max_z = v1db.primary_order_table->max_z;
    size = v1db.primary_order_table->size;
    bucket = v1db.primary_order_table->table + (size - 1);

    order_table = v1db.order_table_list;
    while (order_table!=NULL) {
        if (order_table->sort_z < max_z) {
            break;
        }
        ((br_geometry_v1_buckets*)v1db.format_buckets)->dispatch->_render((br_geometry_v1_buckets *)v1db.format_buckets, v1db.renderer, order_table->table, order_table->size);
        order_table->visits = 0;
        order_table=order_table->next;
    }
    bucket_size = (float)(max_z - min_z) / (float)size;

    for (m = 0; m < size; m++, bucket--) {
        if (*bucket != NULL) {
            ((br_geometry_v1_buckets*)v1db.format_buckets)->dispatch->_render((br_geometry_v1_buckets *)v1db.format_buckets, v1db.renderer, bucket, 1);
        }
        max_z-=bucket_size;
        while(order_table!=NULL) {
            if(order_table->sort_z < max_z) {
                break;
            }
            ((br_geometry_v1_buckets*)v1db.format_buckets)->dispatch->_render((br_geometry_v1_buckets *)v1db.format_buckets, v1db.renderer, order_table->table, order_table->size);
            order_table->visits = 0;
            order_table=order_table->next;
        }
    }
    while (order_table!=NULL) {
        ((br_geometry_v1_buckets*)v1db.format_buckets)->dispatch->_render((br_geometry_v1_buckets *)v1db.format_buckets, v1db.renderer, order_table->table, order_table->size);
        order_table->visits = 0;
        order_table = order_table->next;
    }
}
