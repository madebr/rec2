#include "pstate.h"

#include "core/fw/object.h"
#include "core/fw/resource.h"
#include "core/fw/tokenval.h"
#include "device.h"
#include "match.h"
#include "object.h"
#include "plib.h"
#include "timestmp.h"

#include <string.h>

// GLOBAL: D3D 0x10017250
br_tv_template_entry primitiveD3DStateTemplateEntries[] = {
    { BRT_IDENTIFIER_CSTR,  0, offsetof(br_primitive_state_d3d, identifier), 0x5, 0x3,  },
    { BRT_PARTS_TL,         0, (uintptr_t)&PrimPartsD3DTokens,          0xd, 0x1d, },
};

// GLOBAL: D3D 0x10017280
br_tv_template primitiveD3DStateTemplate = {
    BR_ASIZE(primitiveD3DStateTemplateEntries),
    primitiveD3DStateTemplateEntries,
};

// GLOBAL: D3D 0x100174d0
br_tv_template_entry outputD3DTemplateEntries[] = {
    { BRT_COLOUR_BUFFER_O,  0,  offsetof(br_primitive_state_d3d, out.pixelmap), 0x7, 0x3, },
    { BRT_DEPTH_BUFFER_O,   0,  offsetof(br_primitive_state_d3d, out.depth),    0x7, 0x3, },
};

// GLOBAL: D3D 0x10017500
br_tv_template outputD3DTemplate = {
    BR_ASIZE(outputD3DTemplateEntries),
    outputD3DTemplateEntries,
};

// GLOBAL: D3D 0x100172a0
br_tv_template_entry primitiveD3DTemplateEntries[] = {
    { BRT_SMOOTH_B,             0, offsetof(br_primitive_state_d3d, prim.flags),              0x7,    0x1f,    PRIMF_SMOOTH,            1, },
    { BRT_PERSPECTIVE_B,        0, offsetof(br_primitive_state_d3d, prim.flags),              0x7,    0x1f,    PRIMF_PERSPECTIVE,       1, },
    { BRT_DECAL_B,              0, offsetof(br_primitive_state_d3d, prim.flags),              0x7,    0x1f,    PRIMF_DECAL,             1, },
    { BRT_DEPTH_WRITE_B,        0, offsetof(br_primitive_state_d3d, prim.flags),              0x7,    0x1f,    PRIMF_DEPTH_WRITE,       1, },
    { BRT_COLOUR_WRITE_B,       0, offsetof(br_primitive_state_d3d, prim.flags),              0x7,    0x1f,    PRIMF_COLOUR_WRITE,      1, },
    { BRT_BLEND_B,              0, offsetof(br_primitive_state_d3d, prim.flags),              0x7,    0x1f,    PRIMF_BLEND,             1, },
    { BRT_MODULATE_B,           0, offsetof(br_primitive_state_d3d, prim.flags),              0x7,    0x1f,    PRIMF_MODULATE,          1, },
    { BRT_COLOUR_T,             0, offsetof(br_primitive_state_d3d, prim.colour_type),        0x7,    0x3,     0,                       1, },
    { BRT_COLOUR_B,             0, offsetof(br_primitive_state_d3d, prim.colour_type),        0x7,    0x1e,    BRT_DEFAULT,             1, },
    { BRT_COLOUR_MAP_O,         0, offsetof(br_primitive_state_d3d, prim.colour_map),         0x7,    0x3, },
    { BRT_TEXTURE_O,            0, offsetof(br_primitive_state_d3d, prim.colour_map),         0x7,    0x3, },
    { BRT_FOG_T,                0, offsetof(br_primitive_state_d3d, prim.fog_type),           0x7,    0x3, },
    { BRT_FOG_RGB,              0, offsetof(br_primitive_state_d3d, prim.fog_colour),         0x7,    0x3, },
    { BRT_FOG_MIN_X,            0, offsetof(br_primitive_state_d3d, prim.fog_min),            0x7,    0xe, },
    { BRT_FOG_MAX_X,            0, offsetof(br_primitive_state_d3d, prim.fog_max),            0x7,    0xe, },
    { BRT_FOG_MIN_F,            0, offsetof(br_primitive_state_d3d, prim.fog_min),            0x7,    0x3, },
    { BRT_FOG_MAX_F,            0, offsetof(br_primitive_state_d3d, prim.fog_max),            0x7,    0x3, },
    { BRT_OPACITY_X,            0, offsetof(br_primitive_state_d3d, prim.alpha_val),          0x7,    0xe, },
    { BRT_OPACITY_F,            0, offsetof(br_primitive_state_d3d, prim.alpha_val),          0x7,    0x3, },
    { BRT_MAP_ANTIALIASING_T,   0, offsetof(br_primitive_state_d3d, prim.map_antialiasing),   0x7,    0x3, },
    { BRT_MAP_INTERPOLATION_T,  0, offsetof(br_primitive_state_d3d, prim.map_interpolation),  0x7,    0x3, },
    { BRT_MIP_INTERPOLATION_T,  0, offsetof(br_primitive_state_d3d, prim.mip_interpolation),  0x7,    0x3, },
};

// GLOBAL: D3D 0x100174b0
br_tv_template primitiveD3DTemplate = {
    BR_ASIZE(primitiveD3DTemplateEntries),
    primitiveD3DTemplateEntries,
};

// GLOBAL: D3D 0x10017520
br_primitive_state_dispatch primitiveD3DStateDispatch = {
    NULL,
    NULL,
    NULL,
    NULL,
    (void*)_M_br_primitive_state_d3d_free,
    (void*)_M_br_object_d3d_identifier,
    (void*)_M_br_primitive_state_d3d_type,
    (void*)_M_br_primitive_state_d3d_isType,
    (void*)_M_br_object_d3d_device,
    (void*)_M_br_primitive_state_d3d_space,
    (void*)_M_br_primitive_state_d3d_templateQuery,
    _M_br_object_query,
    (void*)_M_br_object_queryBuffer,
    _M_br_object_queryMany,
    _M_br_object_queryManySize,
    _M_br_object_queryAll,
    _M_br_object_queryAllSize,
    (void*)_M_br_primitive_state_d3d_partSet,
    (void*)_M_br_primitive_state_d3d_partSetMany,
    (void*)_M_br_primitive_state_d3d_partQuery,
    (void*)_M_br_primitive_state_d3d_partQueryBuffer,
    (void*)_M_br_primitive_state_d3d_partQueryMany,
    (void*)_M_br_primitive_state_d3d_partQueryManySize,
    (void*)_M_br_primitive_state_d3d_partQueryAll,
    (void*)_M_br_primitive_state_d3d_partQueryAllSize,
    (void*)_M_br_primitive_state_d3d_partIndexQuery,
    (void*)_M_br_primitive_state_d3d_stateDefault,
    (void*)_M_br_primitive_state_d3d_stateCopy,
    (void*)_M_br_primitive_state_d3d_renderBegin,
    (void*)_M_br_primitive_state_d3d_renderEnd,
    (void*)_M_br_primitive_state_d3d_rangesQueryF,
    (void*)_M_br_primitive_state_d3d_rangesQueryX,
    (void*)_M_br_primitive_state_d3d_partQueryCapability,
    (void*)_M_br_primitive_state_d3d_stateQueryPerformance,
};

// FUNCTION: D3D 0x10006c10
br_tv_template* C2_HOOK_CDECL findTemplate(br_token part) {
    switch(part) {
    case BRT_PRIMITIVE:
        return &primitiveD3DTemplate;
    case BRT_OUTPUT:
        return &outputD3DTemplate;
    default:
        break;
    }
    return NULL;
}

// FUNCTION: D3D 0x10006a90
br_primitive_state_d3d* C2_HOOK_CDECL PrimitiveStateD3DAllocate(br_primitive_library_d3d* plib) {
    br_primitive_state_d3d* self;

    self = BrResAllocate(DriverDeviceD3D.res, sizeof(*self), BR_MEMORY_OBJECT);
    if (self == NULL) {
        return NULL;
    }
    self->plib = plib;
    self->dispatch = &primitiveD3DStateDispatch;
    self->prim.map_antialiasing = BRT_NONE;
    self->prim.map_interpolation = BRT_NONE;
    self->prim.mip_interpolation = BRT_NONE;
    self->prim.fog_type = BRT_NONE;
    self->prim.fog_colour = 0xff0000;
    self->prim.fog_min = 0.0f;
    self->prim.fog_max = 1.0f;
    plib->dispatch->_addFront((br_object_container*)plib, (br_object*)self);
    return self;
}

// FUNCTION: D3D 0x10006fc0
void C2_HOOK_CDECL PrimitiveStateD3DClearTemplates(void) {
    memset(&primitiveD3DStateTemplate, 0, sizeof(primitiveD3DStateTemplate));
    primitiveD3DStateTemplate.n_entries = BR_ASIZE(primitiveD3DStateTemplateEntries);
    primitiveD3DStateTemplate.entries = primitiveD3DStateTemplateEntries;

    memset(&outputD3DTemplate, 0, sizeof(outputD3DTemplate));
    outputD3DTemplate.n_entries = BR_ASIZE(outputD3DTemplateEntries);
    outputD3DTemplate.entries = outputD3DTemplateEntries;

    C2_HOOK_BUG_ON(offsetof(primitive_d3d_state, out.colour.pixelmap) != 0x48);
    C2_HOOK_BUG_ON(offsetof(primitive_d3d_state, out.depth) != 0x4c);

    memset(&primitiveD3DTemplate, 0, sizeof(primitiveD3DTemplate));
    primitiveD3DTemplate.n_entries = BR_ASIZE(primitiveD3DTemplateEntries);
    primitiveD3DTemplate.entries = primitiveD3DTemplateEntries;
#if 0
    CLEAR_TEMPLATE(primitiveState);
    CLEAR_TEMPLATE(output);
    CLEAR_TEMPLATE(primitive);
#endif
}

// FUNCTION: D3D 0x10006af0
void C2_HOOK_CDECL _M_br_primitive_state_d3d_free(br_primitive_state_d3d* self) {
    self->plib->dispatch->_remove((br_object_container*)self->plib, (br_object*)self);
    BrResFreeNoCallback(self);
}

// FUNCTION: D3D 0x10006b10
br_token C2_HOOK_CDECL _M_br_primitive_state_d3d_type(br_primitive_state_d3d* self) {
    return BRT_PRIMITIVE_STATE;
}

// FUNCTION: D3D 0x10006b20
br_boolean C2_HOOK_CDECL _M_br_primitive_state_d3d_isType(br_primitive_state_d3d* self, br_token t) {
    return t == BRT_PRIMITIVE_STATE || t == BRT_OBJECT_CONTAINER || t == BRT_OBJECT;
}

// FUNCTION: D3D 0x10006b40
br_int_32 C2_HOOK_CDECL  C2_HOOK_CDECL _M_br_primitive_state_d3d_space(br_primitive_state_d3d *self) {
    return sizeof(br_primitive_state_d3d);
}

// FUNCTION: D3D 0x10006b50
br_tv_template * C2_HOOK_CDECL  C2_HOOK_CDECL _M_br_primitive_state_d3d_templateQuery(br_primitive_state_d3d* self) {
    primitiveD3DStateTemplate.res = DriverDeviceD3D.res;
    return &primitiveD3DStateTemplate;
}

// FUNCTION: D3D 0x10006b60
br_error C2_HOOK_CDECL  C2_HOOK_CDECL _M_br_primitive_state_d3d_partSet(br_primitive_state_d3d* self, br_token part, br_int_32 index, br_token t, br_uint_32 value) {
    br_error r;
    br_tv_template* tp = findTemplate(part);
    br_uint_32 m;

    if (tp == NULL) {
        return 0xa001;
    }
    m = 0;
    r = BrTokenValueSet(self, &m, t, value, tp);

    if (r != 0) {
        return r;
    }
    switch (part) {
    case BRT_PRIMITIVE:
        self->prim.timestamp = Timestamp();
        if (m != 0) {
            self->prim.timestamp_major = Timestamp();
        }
        break;

    case BRT_OUTPUT:
        self->out.timestamp = Timestamp();
        if (m != 0) {
            self->out.timestamp_major = Timestamp();
        }
        break;
    default:
        break;
    }
    return 0;
}

// FUNCTION: D3D 0x10006c30
br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_partSetMany(br_primitive_state_d3d* self, br_token part, br_int_32 index, br_token_value* tv, br_int_32* pcount) {
    br_error r;
    br_tv_template* tp = findTemplate(part);
    br_uint_32 m;
    br_int_32 c;

    if (tp == NULL) {
        return 0xa001;
    }

    m = 0;
    r = BrTokenValueSetMany(self, &c, &m, tv, tp);
    if (r != 0 || c == 0) {
        return r;
    }
    if (pcount != NULL) {
        *pcount = c;
    }
    switch (part) {
    case BRT_PRIMITIVE:
        self->prim.timestamp = Timestamp();
        if (m != 0) {
            self->prim.timestamp_major = Timestamp();
        }
        break;

    case BRT_OUTPUT:
        self->out.timestamp = Timestamp();
        if (m != 0) {
            self->out.timestamp_major = Timestamp();
        }
        break;
    default:
        break;
    }
    return 0;
}

// FUNCTION: D3D 0x10006cf0
br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_partQuery(br_primitive_state_d3d* self, br_token part, br_int_32 index, br_uint_32* pvalue, br_token t) {
    br_tv_template* tp = findTemplate(part);

    if (tp == NULL) {
        return 0xa001;
    }
    return BrTokenValueQuery(pvalue, NULL, 0, t, self, tp);
}

// FUNCTION: D3D 0x10006d30
br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_partQueryBuffer(br_primitive_state_d3d* self, br_token part, br_int_32 index, br_uint_32* pvalue, br_uint_32 *buffer, br_size_t buffer_size, br_token t) {
    br_tv_template* tp = findTemplate(part);

    if (tp == NULL) {
        return 0xa001;
    }
    return BrTokenValueQuery(pvalue, buffer, buffer_size, t, self, tp);
}

// FUNCTION: D3D 0x10006d70
br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_partQueryMany(br_primitive_state_d3d* self, br_token part, br_int_32 index, br_token_value* tv, void* extra, br_size_t extra_size, br_int_32* pcount) {
    br_tv_template* tp = findTemplate(part);

    if (tp == NULL) {
        return 0xa001;
    }
    return BrTokenValueQueryMany(tv, extra, extra_size, pcount, self, tp);
}

// FUNCTION: D3D 0x10006db0
br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_partQueryManySize(br_primitive_state_d3d* self, br_token part, br_int_32 index, br_size_t* pextra_size, br_token_value* tv) {
    br_tv_template* tp = findTemplate(part);

    if (tp == NULL) {
        return 0xa001;
    }
    return BrTokenValueQueryManySize(pextra_size, tv, self, tp);
}

// FUNCTION: D3D 0x10006de0
br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_partQueryAll(br_primitive_state_d3d* self, br_token part, br_int_32 index, br_token_value* buffer, br_size_t buffer_size) {
    br_tv_template* tp = findTemplate(part);

    if (tp == NULL) {
        return 0xa001;
    }
    return BrTokenValueQueryAll(buffer, buffer_size, self, tp);
}

// FUNCTION: D3D 0x10006e10
br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_partQueryAllSize(br_primitive_state_d3d* self, br_token part, br_int_32 index, br_size_t* psize) {
    br_tv_template* tp = findTemplate(part);

    if (tp == NULL) {
        return 0xa001;
    }
    return BrTokenValueQueryAllSize(psize, self, tp);
}

// FUNCTION: D3D 0x10006e50
br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_partIndexQuery(br_primitive_state_d3d* self, br_token part, br_int_32* pnindex) {
    int n;

    switch (part) {
    case BRT_PRIMITIVE:
    case BRT_OUTPUT:
        n = 1;
        break;
    default:
        n = 0;
    }
    if (pnindex == NULL) {
        return 0xa001;
    }
    *pnindex = n;
    return 0;
}

// FUNCTION: D3D 0x10006e80
br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_stateDefault(br_primitive_state_d3d* self, br_uint_32 mask) {

    if (mask & BR_STATE_PRIMITIVE) {
        self->prim.flags = PRIMF_COLOUR_WRITE | PRIMF_DEPTH_WRITE;
        self->prim.colour_map = NULL;
        self->prim.colour_type = BRT_DEFAULT;

        self->prim.timestamp = Timestamp();
        self->prim.timestamp_major = Timestamp();
    }
    if (mask & BR_STATE_OUTPUT) {
        self->out.pixelmap = NULL;
        self->out.depth = NULL;

        self->out.timestamp = Timestamp();
        self->out.timestamp_major = Timestamp();
    }
    return 0;
}

// FUNCTION: D3D 0x10006f00
br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_stateCopy(br_primitive_state_d3d *self, br_primitive_state_d3d *source, br_uint_32 mask) {

    if ((mask & BR_STATE_PRIMITIVE) && self->prim.timestamp != source->prim.timestamp) {
        self->prim = source->prim;
    }
    if((mask & BR_STATE_OUTPUT) && (self->out.timestamp != source->out.timestamp ||
            self->out.pixelmap->pm_base_x != source->out.pixelmap->pm_base_x ||
            self->out.pixelmap->pm_base_y != source->out.pixelmap->pm_base_y)) {

        self->out = source->out;
    }

    if ((mask & BR_STATE_CACHE) &&
           ((self->cache.timestamp_prim != source->cache.timestamp_prim) ||
            (self->cache.timestamp_out != source->cache.timestamp_out) ||
            (self->cache.last_type != source->cache.last_type))) {

        self->cache = source->cache;
    }

    return 0;
}

// FUNCTION: D3D 0x10006e40
br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_partQueryCapability(br_primitive_state_d3d* self, br_token part, br_int_32 index, br_token_value* buffer, br_size_t buffer_size) {

    return 0x1002;
}

br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_stateQueryPerformance(br_primitive_state_d3d* self, br_fixed_lu* speed) {
    return 0x1002;
}
