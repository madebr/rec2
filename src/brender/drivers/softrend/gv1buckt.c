#include "gv1buckt.h"

#include "setup.h"

#include "core/fw/resource.h"
#include "core/fw/tokenval.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(const br_geometry_v1_buckets_dispatch, geometryV1BucketsDispatch, 0x0058c080, {
    NULL,
    NULL,
    NULL,
    NULL,
    _M_br_geometry_v1_buckets_soft_free,
    _M_br_soft_object_identifier,
    _M_br_geometry_v1_buckets_soft_type,
    _M_br_geometry_v1_buckets_soft_isType,
    _M_br_soft_object_device,
    _M_br_geometry_v1_buckets_soft_space,
    _M_br_geometry_v1_buckets_soft_templateQuery,
    _M_br_object_query,
    _M_br_object_queryBuffer,
    _M_br_object_queryMany,
    _M_br_object_queryManySize,
    _M_br_object_queryAll,
    _M_br_object_queryAllSize,
    _M_br_geometry_v1_buckets_soft_render,
    _M_br_geometry_v1_buckets_soft_render,
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_tv_template_entry, geometryV1BucketsTemplateEntries, 3, 0x0058c038, {
    { BRT_IDENTIFIER_CSTR,      NULL,   offsetof(br_geometry_v1_buckets_soft, identifier),          5, 3, 0, 0, },
    { BRT_RENDERER_FACILITY_O,  NULL,   offsetof(br_geometry_v1_buckets_soft, renderer_facility),   5, 3, 0, 0, },
    { BRT_FACILITY_O,           NULL,   offsetof(br_geometry_v1_buckets_soft, renderer_facility),   1, 3, 0, 0, },
});


br_geometry_v1_buckets* (C2_HOOK_STDCALL * GeometryV1BucketsAllocate_original)(br_soft_renderer_facility *type, const char *id);
br_geometry_v1_buckets* C2_HOOK_STDCALL GeometryV1BucketsAllocate(br_soft_renderer_facility *type, const char *id) {

#if 0//defined(C2_HOOKS_ENABLED)
    return GeometryV1BucketsAllocate_original(type, id);
#else
    br_geometry_v1_buckets_soft* self;

    C2_HOOK_BUG_ON(sizeof(br_geometry_v1_buckets_soft) != 0x10);
    self = BrResAllocate(type->device, sizeof(br_geometry_v1_buckets_soft), BR_MEMORY_OBJECT_DATA);

    if (self == NULL) {
        return NULL;
    }

    self->dispatch = &C2V(geometryV1BucketsDispatch);
    self->identifier = id;
    self->device = type->device;
    self->renderer_facility = type;

    type->dispatch->_addFront((br_object_container*)type, (br_object*)self);
    return (br_geometry_v1_buckets*)self;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005412b0, GeometryV1BucketsAllocate, GeometryV1BucketsAllocate_original)

void C2_HOOK_CDECL _M_br_geometry_v1_buckets_soft_free(br_geometry_v1_buckets_soft* self) {

    self->renderer_facility->dispatch->_remove((br_object_container*)self->renderer_facility, (br_object*)self);
    BrResFreeNoCallback(self);
}
C2_HOOK_FUNCTION(0x00541300, _M_br_geometry_v1_buckets_soft_free)

br_token C2_HOOK_CDECL _M_br_geometry_v1_buckets_soft_type(br_geometry_v1_buckets_soft* self) {

    return BRT_GEOMETRY_V1_BUCKETS;
}
C2_HOOK_FUNCTION(0x00541320, _M_br_geometry_v1_buckets_soft_type)

br_boolean C2_HOOK_CDECL _M_br_geometry_v1_buckets_soft_isType(br_geometry_v1_buckets_soft* self, br_token t) {

    return t == BRT_GEOMETRY_PRIMITIVES || t == BRT_GEOMETRY || t == BRT_OBJECT;
}
C2_HOOK_FUNCTION(0x00541330, _M_br_geometry_v1_buckets_soft_isType)

br_int_32 C2_HOOK_CDECL _M_br_geometry_v1_buckets_soft_space(br_geometry_v1_buckets_soft* self) {

    C2_HOOK_BUG_ON(sizeof(br_geometry_v1_buckets_soft) != 0x10);
    return sizeof(br_geometry_v1_buckets);
}
C2_HOOK_FUNCTION(0x00541350, _M_br_geometry_v1_buckets_soft_space)

br_tv_template* C2_HOOK_CDECL _M_br_geometry_v1_buckets_soft_templateQuery(br_geometry_v1_buckets_soft* self) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_soft_device, templates.geometryV1BucketsTemplate, 0x28);
    C2_HOOK_BUG_ON(BR_ASIZE(C2V(geometryV1BucketsTemplateEntries)) != 3);

    if (self->device->templates.geometryV1BucketsTemplate == NULL) {
        self->device->templates.geometryV1BucketsTemplate = BrTVTemplateAllocate(self->device,
            C2V(geometryV1BucketsTemplateEntries),
            BR_ASIZE(C2V(geometryV1BucketsTemplateEntries)));
    }
    return self->device->templates.geometryV1BucketsTemplate;
}
C2_HOOK_FUNCTION(0x00541360, _M_br_geometry_v1_buckets_soft_templateQuery)

br_error (C2_HOOK_CDECL * _M_br_geometry_v1_buckets_soft_render_original)(br_renderer_state_stored* self, br_soft_renderer* renderer, br_primitive** buckets, br_int_32 nbuckets);
br_error C2_HOOK_CDECL _M_br_geometry_v1_buckets_soft_render(br_renderer_state_stored* self, br_soft_renderer* renderer, br_primitive** buckets, br_int_32 nbuckets) {

#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_geometry_v1_buckets_soft_render_original(self, renderer, buckets, nbuckets);
#else
    br_primitive* p;
	br_renderer_state_stored* last_state;
	br_error r;
	br_token last_type;

	if (nbuckets <= 0) {
        return 0;
    }

    C2_HOOK_ASSERT_ADDRESS(C2V(rend).block, 0x0079f9cc);
    C2_HOOK_ASSERT_ADDRESS(C2V(rend).block_changed, 0x0079f9d0);
    C2_HOOK_ASSERT_ADDRESS(C2V(rend).range_changed, 0x0079f9d4);
    C2_HOOK_BUG_ON((unsigned)(BR_STATE_PRIMITIVE | BR_STATE_CACHE) != 0x80000100u);

	last_state = self;
	last_type = BR_NULL_TOKEN;
	C2V(rend).block = NULL;

	for (buckets += nbuckets - 1; nbuckets != 0; nbuckets--, buckets--) {

		for (p = *buckets; p != NULL; p = p->next) {

			if (p->stored != last_state || p->type != (br_uint_32)last_type) {

				if (C2V(rend).block != NULL) {
                    renderer->state.pstate->dispatch->_renderEnd(renderer->state.pstate, C2V(rend).block);
                }
				if (p->stored != NULL) {
                    renderer->dispatch->_stateRestore((br_renderer*)renderer, p->stored, (br_uint_32)(BR_STATE_PRIMITIVE | BR_STATE_CACHE));
                }
				r = renderer->state.pstate->dispatch->_renderBegin(renderer->state.pstate, &C2V(rend).block, &C2V(rend).block_changed, &C2V(rend).range_changed, 0, p->type);

				last_type = p->type;
				last_state = p->stored;

				if (r != 0) {
                    continue;
                }
			}
			C2V(rend).block->render(C2V(rend).block, p->v[0], p->v[1], p->v[2]);
		}
	}
	if (C2V(rend).block != NULL) {
        renderer->state.pstate->dispatch->_renderEnd(renderer->state.pstate, C2V(rend).block);
    }

	return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00541390, _M_br_geometry_v1_buckets_soft_render, _M_br_geometry_v1_buckets_soft_render_original)
