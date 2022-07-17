#ifndef REC2_OTABLE_H
#define REC2_OTABLE_H

#include "brender/br_types.h"

br_order_table* C2_HOOK_CDECL BrZsOrderTableAllocate(br_uint_16 size, br_uint_32 flags, br_uint_16 type);

void C2_HOOK_CDECL BrZsOrderTableFree(br_order_table* order_table);

br_order_table* C2_HOOK_CDECL BrZsActorOrderTableSet(br_actor* actor, br_order_table* order_table);

br_order_table* C2_HOOK_CDECL BrZsActorOrderTableGet(br_actor* actor);

br_order_table* C2_HOOK_CDECL BrZsOrderTableClear(br_order_table* order_table);

void C2_HOOK_CDECL BrZsOrderTablePrimitiveInsert(br_order_table* order_table, br_primitive* primitive, br_uint_16 bucket);

br_uint_16 C2_HOOK_CDECL BrZsPrimitiveBucketSelect(br_scalar* z, br_uint_16 type, br_scalar min_z, br_scalar max_z, br_uint_16 size, br_uint_16 sort_type);

void C2_HOOK_CDECL BrZsOrderTablePrimaryEnable(br_order_table* order_table);

void C2_HOOK_CDECL BrZsOrderTablePrimaryDisable(void);

void C2_HOOK_STDCALL InsertOrderTableList(br_order_table* order_table);

void C2_HOOK_STDCALL SetOrderTableBounds(br_bounds* bounds, br_order_table* order_table);

void C2_HOOK_STDCALL SetOrderTableRange(br_order_table* order_table);

void C2_HOOK_STDCALL RenderOrderTableList(void);

void C2_HOOK_STDCALL RenderPrimaryOrderTable(void);

#endif // REC2_OTABLE_H
