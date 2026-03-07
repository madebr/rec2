#include "gv1buckt.h"

#include "object.h"
#include "setup.h"

#include "core/fw/object.h"
#include "core/fw/resource.h"
#include "core/fw/tokenval.h"

// GLOBAL: CARMA2_HW 0x0058c080
const br_geometry_v1_buckets_dispatch geometryV1BucketsDispatch = {
    NULL,
    NULL,
    NULL,
    NULL,
    (void*)_M_br_geometry_v1_buckets_soft_free,
    (void*)_M_br_softrend_object_identifier,
    (void*)_M_br_geometry_v1_buckets_soft_type,
    (void*)_M_br_geometry_v1_buckets_soft_isType,
    (void*)_M_br_softrend_object_device,
    (void*)_M_br_geometry_v1_buckets_soft_space,
    (void*)_M_br_geometry_v1_buckets_soft_templateQuery,
    _M_br_object_query,
    _M_br_object_queryBuffer,
    _M_br_object_queryMany,
    _M_br_object_queryManySize,
    _M_br_object_queryAll,
    _M_br_object_queryAllSize,
    (void*)_M_br_geometry_v1_buckets_soft_render,
    (void*)_M_br_geometry_v1_buckets_soft_render,
};

// GLOBAL: CARMA2_HW 0x0058c038
br_tv_template_entry geometryV1BucketsTemplateEntries[] ={
    { BRT_IDENTIFIER_CSTR,      NULL,   offsetof(br_geometry_v1_buckets_soft, identifier),          5, 3, 0, 0, },
    { BRT_RENDERER_FACILITY_O,  NULL,   offsetof(br_geometry_v1_buckets_soft, renderer_facility),   5, 3, 0, 0, },
    { BRT_FACILITY_O,           NULL,   offsetof(br_geometry_v1_buckets_soft, renderer_facility),   1, 3, 0, 0, },
};


// FUNCTION: CARMA2_HW 0x005412b0
br_geometry_v1_buckets* C2_HOOK_STDCALL GeometryV1BucketsAllocate(br_soft_renderer_facility *type, const char *id) {
    br_geometry_v1_buckets_soft* self;

    C2_HOOK_BUG_ON(sizeof(br_geometry_v1_buckets_soft) != 0x10);
    self = BrResAllocate(type->device, sizeof(br_geometry_v1_buckets_soft), BR_MEMORY_OBJECT_DATA);

    if (self == NULL) {
        return NULL;
    }

    self->dispatch = &geometryV1BucketsDispatch;
    self->identifier = id;
    self->device = type->device;
    self->renderer_facility = type;

    type->dispatch->_addFront((br_object_container*)type, (br_object*)self);
    return (br_geometry_v1_buckets*)self;
}

// FUNCTION: CARMA2_HW 0x00541300
void C2_HOOK_CDECL _M_br_geometry_v1_buckets_soft_free(br_geometry_v1_buckets_soft* self) {

    self->renderer_facility->dispatch->_remove((br_object_container*)self->renderer_facility, (br_object*)self);
    BrResFreeNoCallback(self);
}

// FUNCTION: CARMA2_HW 0x00541320
br_token C2_HOOK_CDECL _M_br_geometry_v1_buckets_soft_type(br_geometry_v1_buckets_soft* self) {

    return BRT_GEOMETRY_V1_BUCKETS;
}

// FUNCTION: CARMA2_HW 0x00541330
br_boolean C2_HOOK_CDECL _M_br_geometry_v1_buckets_soft_isType(br_geometry_v1_buckets_soft* self, br_token t) {

    return t == BRT_GEOMETRY_PRIMITIVES || t == BRT_GEOMETRY || t == BRT_OBJECT;
}

// FUNCTION: CARMA2_HW 0x00541350
br_int_32 C2_HOOK_CDECL _M_br_geometry_v1_buckets_soft_space(br_geometry_v1_buckets_soft* self) {

    C2_HOOK_BUG_ON(sizeof(br_geometry_v1_buckets_soft) != 0x10);
    return sizeof(br_geometry_v1_buckets_soft);
}

// FUNCTION: CARMA2_HW 0x00541360
br_tv_template* C2_HOOK_CDECL _M_br_geometry_v1_buckets_soft_templateQuery(br_geometry_v1_buckets_soft* self) {

#ifndef REC2_MATCHING
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_soft_device, templates.geometryV1BucketsTemplate, 0x28);
    C2_HOOK_BUG_ON(BR_ASIZE(geometryV1BucketsTemplateEntries) != 3);
#endif

    if (self->device->templates.geometryV1BucketsTemplate == NULL) {
        self->device->templates.geometryV1BucketsTemplate = BrTVTemplateAllocate(self->device,
            geometryV1BucketsTemplateEntries,
            BR_ASIZE(geometryV1BucketsTemplateEntries));
    }
    return self->device->templates.geometryV1BucketsTemplate;
}

// FUNCTION: CARMA2_HW 0x00541390
br_error C2_HOOK_CDECL _M_br_geometry_v1_buckets_soft_render(br_renderer_state_stored* self, br_soft_renderer* renderer, br_primitive** buckets, br_int_32 nbuckets) {
    br_primitive* p;
	br_renderer_state_stored* last_state;
	br_error r;
	br_token last_type;

	if (nbuckets <= 0) {
        return 0;
    }

    C2_HOOK_BUG_ON((unsigned)(BR_STATE_PRIMITIVE | BR_STATE_CACHE) != 0x80000100u);

	last_state = self;
	last_type = BR_NULL_TOKEN;
	rend.block = NULL;

	for (buckets += nbuckets - 1; nbuckets != 0; nbuckets--, buckets--) {

		for (p = *buckets; p != NULL; p = p->next) {

			if (p->stored != last_state || p->type != (br_uint_32)last_type) {

				if (rend.block != NULL) {
                    renderer->state.pstate->dispatch->_renderEnd(renderer->state.pstate, rend.block);
                }
				if (p->stored != NULL) {
                    renderer->dispatch->_stateRestore((br_renderer*)renderer, p->stored, (br_uint_32)(BR_STATE_PRIMITIVE | BR_STATE_CACHE));
                }
				r = renderer->state.pstate->dispatch->_renderBegin(renderer->state.pstate, &rend.block, &rend.block_changed, &rend.range_changed, 0, p->type);

				last_type = p->type;
				last_state = p->stored;

				if (r != 0) {
                    continue;
                }
			}
			rend.block->render(rend.block, p->v[0], p->v[1], p->v[2]);
		}
	}
	if (rend.block != NULL) {
        renderer->state.pstate->dispatch->_renderEnd(renderer->state.pstate, rend.block);
    }

	return 0;
}
