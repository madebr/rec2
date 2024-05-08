#include "rendfcty.h"

#include "gprim.h"
#include "gv1buckt.h"
#include "gv1model.h"
#include "lighting.h"
#include "object.h"
#include "state.h"

#include "core/fw/objectc.h"
#include "core/fw/resource.h"
#include "core/fw/tokenval.h"

HOOK_VARIABLE_DECLARE_STATIC(const struct br_renderer_facility_dispatch, rendererFacilityDispatch);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_token, RendererPartsTokens, 7, 0x0058bd20, {
    BRT_CULL,
    BRT_SURFACE,
    BRT_MATRIX,
    BRT_ENABLE,
    BRT_LIGHT,
    BRT_CLIP,
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_tv_template_entry, rendererFacilityTemplateEntries, 3, 0x0058bbd8, {
    { BRT_IDENTIFIER_CSTR,  NULL,	offsetof(br_renderer_facility , identifier),    5,	3,  0,  0, },
    { BRT_RENDERER_MAX_I32, NULL,	0,					                            5,  1,  1,  0, },
    { BRT_PARTS_TL,         NULL,	&C2V(RendererPartsTokens),                      13, 29, 0,  0, },
});

br_renderer_facility* (C2_HOOK_STDCALL * RendererFacilitySoftAllocate_original)(br_device* dev, const char* identifier);
br_renderer_facility* C2_HOOK_STDCALL RendererFacilitySoftAllocate(br_device* dev, const char* identifier) {

#if 0//defined(C2_HOOKS_ENABLED)
    return RendererFacilitySoftAllocate_original(dev, identifier);
#else
    br_soft_renderer_facility* self;

    C2_HOOK_BUG_ON(sizeof(br_soft_renderer_facility) != 0x79c);

    self = BrResAllocate(NULL, sizeof(br_soft_renderer_facility), BR_MEMORY_OBJECT_DATA);

    self->dispatch = (br_renderer_facility_dispatch*)&C2V(rendererFacilityDispatch);
    self->identifier = identifier;
    self->device = (br_soft_device*)dev;
	self->num_instances = 0;
	self->object_list = BrObjectListAllocate(dev);

    StateInitialise(&self->default_state);

    GeometryV1BucketsAllocate(self, "V1Buckets");
    GeometryPrimitivesAllocate(self, "Primitives");
    GeometryLightingAllocate(self, "Lighting");
    GeometryV1ModelAllocate(self, "V1Model");

    dev->dispatch->_addFront((br_object_container*)dev, (br_object*)self);

    return (br_renderer_facility*)self;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005405e0, RendererFacilitySoftAllocate, RendererFacilitySoftAllocate_original)

void C2_HOOK_CDECL _M_br_renderer_facility_soft_free(br_soft_renderer_facility* self) {

    br_device* dev = self->dispatch->_device((br_object*)self);
    dev->dispatch->_remove((br_object_container*)dev, (br_object*)self);

    BrObjectContainerFree((br_object_container*)self, BR_NULL_TOKEN, NULL, NULL);

    BrResFreeNoCallback(self);
}
C2_HOOK_FUNCTION(0x00540670, _M_br_renderer_facility_soft_free)

br_token C2_HOOK_CDECL _M_br_renderer_facility_soft_type(br_soft_renderer_facility* self) {

    return BRT_RENDERER_FACILITY;
}
C2_HOOK_FUNCTION(0x005406b0, _M_br_renderer_facility_soft_type)

int C2_HOOK_CDECL _M_br_renderer_facility_soft_isType(br_soft_renderer_facility* self, br_token t) {

    return t == BRT_RENDERER_FACILITY || t == BRT_OBJECT_CONTAINER || t == BRT_OBJECT;
}
C2_HOOK_FUNCTION(0x005406c0, _M_br_renderer_facility_soft_isType)

br_int_32 C2_HOOK_CDECL _M_br_renderer_facility_soft_space(br_soft_renderer_facility* self) {

    C2_HOOK_BUG_ON(sizeof(br_soft_renderer_facility) != 0x79c);

    return sizeof(br_soft_renderer_facility);
}
C2_HOOK_FUNCTION(0x005406e0, _M_br_renderer_facility_soft_space)

br_tv_template* C2_HOOK_CDECL _M_br_renderer_facility_soft_templateQuery(br_soft_renderer_facility* self) {

    if (self->device->templates.rendererFacilityTemplate == NULL) {
        C2_HOOK_BUG_ON(BR_ASIZE(C2V(rendererFacilityTemplateEntries)) != 3);

        self->device->templates.rendererFacilityTemplate = BrTVTemplateAllocate(self->device,
            (br_tv_template_entry*)C2V(rendererFacilityTemplateEntries), BR_ASIZE(C2V(rendererFacilityTemplateEntries)));
    }

    return self->device->templates.rendererFacilityTemplate;
}
C2_HOOK_FUNCTION(0x005406f0, _M_br_renderer_facility_soft_templateQuery)

static C2_HOOK_VARIABLE_IMPLEMENT_INIT(const struct br_renderer_facility_dispatch, rendererFacilityDispatch, 0x0058bc20, {
    NULL,
    NULL,
    NULL,
    NULL,
    _M_br_renderer_facility_soft_free,
    _M_br_softrend_object_identifier,
    _M_br_renderer_facility_soft_type,
    _M_br_renderer_facility_soft_isType,
    _M_br_softrend_object_device,
    _M_br_renderer_facility_soft_space,

    _M_br_renderer_facility_soft_templateQuery,
    _M_br_object_query,
    _M_br_object_queryBuffer,
    _M_br_object_queryMany,
    _M_br_object_queryManySize,
    _M_br_object_queryAll,
    _M_br_object_queryAllSize,

    _M_br_renderer_facility_soft_listQuery,
    _M_br_object_container_tokensMatchBegin,
    _M_br_object_container_tokensMatch,
    _M_br_object_container_tokensMatchEnd,
    _M_br_object_container_addFront,
    _M_br_object_container_removeFront,
    _M_br_object_container_remove,
    _M_br_object_container_find,
    _M_br_object_container_findMany,
    _M_br_object_container_count,

    _M_br_renderer_facility_soft_validDestination,
    _M_br_renderer_facility_soft_rendererNew,
});
