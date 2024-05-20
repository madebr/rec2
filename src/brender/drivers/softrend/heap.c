#include "heap.h"

#include "convert.h"
#include "setup.h"
#include "zsrmacro.h"

#include "c2_string.h"

static br_boolean heapCheck(br_primitive_heap* heap, br_size_t s) {

    return heap->current + s <= heap->base + heap->size;
}

static br_primitive *heapPrimitiveAdd(br_primitive_heap* heap, br_token type) {
    br_primitive* p;

    p = (br_primitive *)heap->current;

    C2_HOOK_BUG_ON(sizeof(br_primitive) != 0x18);
    heap->current += sizeof(br_primitive);

    p->type = type;
    p->stored = C2V(rend).renderer->last_restored;

    return p;
}

static brp_vertex *heapVertexAdd(br_primitive_heap* heap, brp_vertex* src, br_boolean share) {
    brp_vertex* v;
    int n;

    C2_HOOK_BUG_ON(sizeof(brp_vertex) != 0x40);

    if (share) {
        if (src >= C2V(rend).temp_vertices) {
            n = src - C2V(rend).temp_vertices;
            if (n < C2V(rend).nvertices) {
                if (C2V(rend).vertex_heap_pointers[n] == NULL) {
                    C2V(rend).vertex_heap_pointers[n] = (brp_vertex*)heap->current;
                    c2_memcpy(C2V(rend).vertex_heap_pointers[n], src, sizeof(brp_vertex));
                    heap->current += sizeof(brp_vertex);
                }
                return C2V(rend).vertex_heap_pointers[n];
            }
        }
    }

    v = (brp_vertex*)heap->current;
    c2_memcpy(v, src, sizeof(brp_vertex));
    *v = *src;
    heap->current += sizeof(brp_vertex);

    return v;
}

void C2_HOOK_CDECL OpHeapAddTriangle(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2) {
    br_primitive* p;
    br_scalar zprim;

    C2_HOOK_BUG_ON(sizeof(br_primitive) + 3 * sizeof(brp_vertex) != 0xd8);
    if (!heapCheck(C2V(rend).renderer->state.hidden.heap, sizeof(br_primitive) + 3 * sizeof(brp_vertex))) {
        return;
    }

    if (C2V(rend).renderer->state.surface.force_front) {
        zprim = 0.f;
    } else {
        SORT_VALUE_TRIANGLE(C2V(rend).renderer->state.hidden.order_table->type, v0, v1, v2);
    }

    p = heapPrimitiveAdd(C2V(rend).renderer->state.hidden.heap, BRT_TRIANGLE);
    p->v[0] = heapVertexAdd(C2V(rend).renderer->state.hidden.heap, v0, C2V(rend).block->constant_components == 0);
    p->v[1] = heapVertexAdd(C2V(rend).renderer->state.hidden.heap, v1, !~(C2V(rend).block->flags & 0x2));
    p->v[2] = heapVertexAdd(C2V(rend).renderer->state.hidden.heap, v2, !~(C2V(rend).block->flags & 0x2));

    if (C2V(rend).renderer->state.hidden.insert_fn != NULL) {
        br_scalar z[3];
        z[0] = VIEW_Z(v0);
        z[1] = VIEW_Z(v1);
        z[2] = VIEW_Z(v2);

        C2V(rend).renderer->state.hidden.insert_fn(p,
            C2V(rend).renderer->state.hidden.insert_arg1,
            C2V(rend).renderer->state.hidden.insert_arg2,
            C2V(rend).renderer->state.hidden.insert_arg3,
            C2V(rend).renderer->state.hidden.order_table,
            z);
    } else {
        INSERT_PRIMITIVE(C2V(rend).renderer->state.hidden.order_table, p, zprim);
    }
}
C2_HOOK_FUNCTION(0x00546860, OpHeapAddTriangle)

void C2_HOOK_CDECL OpHeapAddLine(brp_block* block, brp_vertex* v0, brp_vertex* v1) {
    br_primitive *p;
    br_scalar zprim;

    C2_HOOK_BUG_ON(sizeof(br_primitive) + 2 * sizeof(brp_vertex) != 0x98);
    if (!heapCheck(C2V(rend).renderer->state.hidden.heap, sizeof(br_primitive) + 2 * sizeof(brp_vertex))) {
        return;
    }

    if (C2V(rend).renderer->state.surface.force_front) {
        zprim = 0.f;
    } else {
        SORT_VALUE_EDGE(C2V(rend).renderer->state.hidden.order_table->type, v0, v1);
    }

    p = heapPrimitiveAdd(C2V(rend).renderer->state.hidden.heap, BRT_LINE);
    p->v[0] = heapVertexAdd(C2V(rend).renderer->state.hidden.heap, v0, !C2V(rend).block->constant_components);
    p->v[1] = heapVertexAdd(C2V(rend).renderer->state.hidden.heap, v1, !~(C2V(rend).block->flags & 0x2));

    if (C2V(rend).renderer->state.hidden.insert_fn != NULL) {
        br_scalar z[2];
        z[0] = VIEW_Z(v0);
        z[1] = VIEW_Z(v1);

        C2V(rend).renderer->state.hidden.insert_fn(p,
            C2V(rend).renderer->state.hidden.insert_arg1,
            C2V(rend).renderer->state.hidden.insert_arg2,
            C2V(rend).renderer->state.hidden.insert_arg3,
            C2V(rend).renderer->state.hidden.order_table,
            z);
    } else {
        INSERT_PRIMITIVE(C2V(rend).renderer->state.hidden.order_table, p, zprim);
    }
}
C2_HOOK_FUNCTION(0x00546be0, OpHeapAddLine)

void C2_HOOK_CDECL OpHeapAddPoint(brp_block* block, brp_vertex* v0) {
    br_primitive* p;
    br_scalar zprim;

    C2_HOOK_BUG_ON(sizeof(br_primitive) + 1 * sizeof(brp_vertex) != 0x58);
    if (!heapCheck(C2V(rend).renderer->state.hidden.heap, sizeof(br_primitive) + 1 * sizeof(brp_vertex))) {
        return;
    }

    if (C2V(rend).renderer->state.surface.force_front) {
        zprim = 0.f;
    } else {
        zprim = VIEW_Z(v0);
    }

    p = heapPrimitiveAdd(C2V(rend).renderer->state.hidden.heap, BRT_POINT);
    p->v[0] = heapVertexAdd(C2V(rend).renderer->state.hidden.heap, v0, 1);

    if (C2V(rend).renderer->state.hidden.insert_fn != NULL) {
        br_scalar z[1];
        z[0] = VIEW_Z(v0);

        C2V(rend).renderer->state.hidden.insert_fn(p,
            C2V(rend).renderer->state.hidden.insert_arg1,
            C2V(rend).renderer->state.hidden.insert_arg2,
            C2V(rend).renderer->state.hidden.insert_arg3,
            C2V(rend).renderer->state.hidden.order_table,
            z);
    } else {
        INSERT_PRIMITIVE(C2V(rend).renderer->state.hidden.order_table, p, zprim);
    }
}
C2_HOOK_FUNCTION(0x00546e90, OpHeapAddPoint)

void C2_HOOK_CDECL OpHeapAddTriangleConvert(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2) {
    br_primitive* p;
    br_scalar zprim;
    brp_vertex outv[3];

    C2_HOOK_BUG_ON(sizeof(br_primitive) + 3 * sizeof(brp_vertex) != 0xd8);
    if (!heapCheck(C2V(rend).renderer->state.hidden.heap, sizeof(br_primitive) + 3 * sizeof(brp_vertex))) {
        return;
    }

    if (C2V(rend).renderer->state.surface.force_front) {
        zprim = 0.f;
    } else {
        SORT_VALUE_TRIANGLE(C2V(rend).renderer->state.hidden.order_table->type, v0, v1, v2);
    }

    ConvertVertex(&outv[0], v0);
    ConvertVertex(&outv[1], v1);
    ConvertVertex(&outv[2], v2);

    p = heapPrimitiveAdd(C2V(rend).renderer->state.hidden.heap, BRT_TRIANGLE);
    p->v[0] = heapVertexAdd(C2V(rend).renderer->state.hidden.heap, &outv[0], 0);
    p->v[1] = heapVertexAdd(C2V(rend).renderer->state.hidden.heap, &outv[1], 0);
    p->v[2] = heapVertexAdd(C2V(rend).renderer->state.hidden.heap, &outv[2], 0);

    if (C2V(rend).renderer->state.hidden.insert_fn != NULL) {
        br_scalar z[3];
        z[0] = VIEW_Z(v0);
        z[1] = VIEW_Z(v1);
        z[2] = VIEW_Z(v2);

        C2V(rend).renderer->state.hidden.insert_fn(p,
            C2V(rend).renderer->state.hidden.insert_arg1,
            C2V(rend).renderer->state.hidden.insert_arg2,
            C2V(rend).renderer->state.hidden.insert_arg3,
            C2V(rend).renderer->state.hidden.order_table,
            z);
    } else {
        INSERT_PRIMITIVE(C2V(rend).renderer->state.hidden.order_table, p, zprim);
    }
}
C2_HOOK_FUNCTION(0x00547030, OpHeapAddTriangleConvert)

void C2_HOOK_CDECL OpHeapAddLineConvert(brp_block* block, brp_vertex* v0, brp_vertex* v1) {
    br_primitive* p;
    br_scalar zprim;
    brp_vertex outv[2];

    C2_HOOK_BUG_ON(sizeof(br_primitive) + 2 * sizeof(brp_vertex) != 0x98);
    if (!heapCheck(C2V(rend).renderer->state.hidden.heap, sizeof(br_primitive) + 2 * sizeof(brp_vertex))) {
        return;
    }

    if (C2V(rend).renderer->state.surface.force_front) {
        zprim = 0.f;
    } else {
        SORT_VALUE_EDGE(C2V(rend).renderer->state.hidden.order_table->type, v0, v1);
    }

    ConvertVertex(&outv[0], v0);
    ConvertVertex(&outv[1], v1);

    p = heapPrimitiveAdd(C2V(rend).renderer->state.hidden.heap, BRT_LINE);
    p->v[0] = heapVertexAdd(C2V(rend).renderer->state.hidden.heap, &outv[0], 0);
    p->v[1] = heapVertexAdd(C2V(rend).renderer->state.hidden.heap, &outv[1], 0);

    if (C2V(rend).renderer->state.hidden.insert_fn != NULL) {
        br_scalar z[2];
        z[0] = VIEW_Z(v0);
        z[1] = VIEW_Z(v1);

        C2V(rend).renderer->state.hidden.insert_fn(p,
            C2V(rend).renderer->state.hidden.insert_arg1,
            C2V(rend).renderer->state.hidden.insert_arg2,
            C2V(rend).renderer->state.hidden.insert_arg3,
            C2V(rend).renderer->state.hidden.order_table,
            z);
    } else {
        INSERT_PRIMITIVE(C2V(rend).renderer->state.hidden.order_table, p, zprim);
    }
}
C2_HOOK_FUNCTION(0x00547310, OpHeapAddLineConvert)

void C2_HOOK_CDECL OpHeapAddPointConvert(brp_block* block, brp_vertex* v0) {
    br_primitive* p;
    br_scalar zprim;
    brp_vertex outv[1];

    C2_HOOK_BUG_ON(sizeof(br_primitive) + 1 * sizeof(brp_vertex) != 0x58);
    if (!heapCheck(C2V(rend).renderer->state.hidden.heap, sizeof(br_primitive) + 1 * sizeof(brp_vertex))) {
        return;
    }

    if (C2V(rend).renderer->state.surface.force_front) {
        zprim = BR_SCALAR(0.0);
    } else {
        SORT_VALUE_POINT(C2V(rend).renderer->state.hidden.order_table->type, v0);
    }

    ConvertVertex(&outv[0], v0);

    p = heapPrimitiveAdd(C2V(rend).renderer->state.hidden.heap, BRT_POINT);
    p->v[0] = heapVertexAdd(C2V(rend).renderer->state.hidden.heap, &outv[0], 0);

    if (C2V(rend).renderer->state.hidden.insert_fn != NULL) {
        br_scalar z[1];
        z[0] = VIEW_Z(v0);

        C2V(rend).renderer->state.hidden.insert_fn(p,
            C2V(rend).renderer->state.hidden.insert_arg1,
            C2V(rend).renderer->state.hidden.insert_arg2,
            C2V(rend).renderer->state.hidden.insert_arg3,
            C2V(rend).renderer->state.hidden.order_table,
            z);
    } else {
        INSERT_PRIMITIVE(C2V(rend).renderer->state.hidden.order_table, p, zprim);
    }
}
C2_HOOK_FUNCTION(0x00547530, OpHeapAddPointConvert)
