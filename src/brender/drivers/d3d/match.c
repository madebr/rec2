#include "match.h"

#include "backend.h"
#include "prim_l15.h"
#include "prim_p15.h"
#include "prim_t15.h"
#include "renderfuncs.h"

#include <stdlib.h>


void C2_HOOK_CDECL ResetComputedOffsetsAndScales(br_primitive_state_d3d* prim) {
    br_device_pixelmap_d3d *pm = prim->out.pixelmap;

    prim->cache.comp_offsets[0] = BR_FIXED_UINT(0);
    prim->cache.comp_offsets[4] = BR_FIXED_UINT(0);
    prim->cache.comp_offsets[5] = BR_FIXED_UINT(0);
    prim->cache.comp_offsets[6] = BR_FIXED_UINT(0);
    prim->cache.comp_offsets[7] = BR_FIXED_UINT(0);
    prim->cache.comp_offsets[14] = BR_FIXED_UINT(0);
    prim->cache.comp_scales[0] = BR_FIXED_UINT(1);
    prim->cache.comp_scales[4] = BR_FIXED_UINT(1);
    prim->cache.comp_scales[5] = BR_FIXED_UINT(1);
    prim->cache.comp_scales[6] = BR_FIXED_UINT(1);
    prim->cache.comp_scales[7] = BR_FIXED_UINT(1);
    prim->cache.comp_scales[14] = BR_FIXED_UINT(1);
    prim->cache.comp_offsets[5] = BR_FIXED_UINT(pm->pm_base_x + pm->pm_width / 2);
    prim->cache.comp_scales[5] = BR_FIXED_UINT(pm->pm_width / 2);
    prim->cache.comp_offsets[6] = BR_FIXED_UINT(pm->pm_height / 2 + pm->pm_base_y);
    prim->cache.comp_scales[6] = BR_FIXED_UINT(-pm->pm_height / 2);
    prim->cache.comp_offsets[7] = BR_FIXED_UINT(0x7fff);
    prim->cache.comp_scales[7] = BR_FIXED_UINT(0xa628);
    prim->cache.comp_scales[8] = BR_FIXED_UINT(1);
    prim->cache.comp_scales[9] = BR_FIXED_UINT(1);
    prim->cache.comp_scales[11] = BR_FIXED_UINT(0xff);
    prim->cache.comp_scales[12] = BR_FIXED_UINT(0xff);
    prim->cache.comp_scales[13] = BR_FIXED_UINT(0xff);
    prim->cache.comp_scales[10] = BR_FIXED_UINT(0xff);
}
C2_HOOK_FUNCTION(0x10006490, ResetComputedOffsetsAndScales)

struct {
    local_block_d3d *blocks;
    int nblocks;
} primInfoD3DTable[3] = {
    { primInfo_d3d_p15, BR_ASIZE(primInfo_d3d_p15), },
    { primInfo_d3d_l15, BR_ASIZE(primInfo_d3d_l15), },
    { primInfo_d3d_t15, BR_ASIZE(primInfo_d3d_t15), },
};

void C2_HOOK_CDECL FUN_10005e30(br_primitive_state_d3d* self, br_uint_32 flags_cmp) {
    int t;

    C2V(gCount_queued_render_states) = 0;
    C2V(gINT_1001bba4) = 0;
    if (flags_cmp & 0x400) {
        if (!(self->prim.flags & 2) && ((self->prim.flags & 0x80) || (self->prim.colour_map != NULL && self->prim.colour_map->buffer.blended))) {
            if (self->prim.flags & 0x200) {
                C2V(gINT_1001bba4) = 1;
            } else if (self->prim.colour_map != NULL && self->prim.colour_map->field_0x84 == -1) {
                C2V(gINT_1001bba4) = 9;
            } else {
                C2V(gINT_1001bba4) = 2;
            }
            C2V(gDAT_1001dc50) = self->prim.colour_map->buffer.field_0x68;
            C2V(gINT_1001dc4c) = self->prim.colour_map->buffer.width_p;
            C2V(gINT_1001dc44) = self->prim.colour_map->buffer.height;
            C2V(gINT_1001dc54) = self->prim.colour_map->buffer.width_b;
        }
        if ((self->prim.flags & 0x80) && self->prim.alpha_val < 16.0f) {
            C2V(gINT_1001bba4) = 9;
        }
        if (!C2V(gTexture_format_is_NOT_ARGB4444)) {
            d3d_load(&self->prim.colour_map->buffer, -1);
        } else if ((self->prim.flags & 1) && !(self->prim.flags & 2)) {
            d3d_load(&self->prim.colour_map->buffer, -2);
        } else if ((self->prim.flags & 0x80) == 0) {
            d3d_load(&self->prim.colour_map->buffer, -1);
        } else {
            d3d_load(&self->prim.colour_map->buffer, (int)self->prim.alpha_val);
        }
        if (self->prim.colour_map != NULL && !C2V(gTexture_format_is_NOT_ARGB4444) && (C2V(gRegister_flags) & 1)) {
            if (self->prim.map_interpolation == 1 || self->prim.map_interpolation == 0x92) {
                QUEUE_RENDER_STATE(0x11, 1);
                QUEUE_RENDER_STATE(0x12, 1);
            } else {
                QUEUE_RENDER_STATE(0x11, 2);
                QUEUE_RENDER_STATE(0x12, 2);
            }
        }
    } else {
        C2V(gDWORD_1001bbb0) = 0;
        if ((self->prim.flags & 0x200) && (self->prim.flags & 0x80)) {
            C2V(gINT_1001bba4) = 3;
        }
        if ((self->prim.flags & 0x80) && (self->prim.alpha_val < 16.f)) {
            C2V(gINT_1001bba4) = 9;
        }
    }
    if (flags_cmp & 0x8) {
        t = 2;
    } else if (flags_cmp & 0xb) {
        t = 1;
    } else {
        t = 0;
    }
    switch (t) {
    case 0:
    case 2:
        QUEUE_RENDER_STATE(0x17, 8);
        QUEUE_RENDER_STATE(0xe, 1);
        break;
    case 1:
        QUEUE_RENDER_STATE(0x17, 7);
        if (C2V(gTexture_format_is_NOT_ARGB4444)) {
            QUEUE_RENDER_STATE(0xe, 1);
        } else {
            if (self->prim.colour_map == NULL) {
                if ((self->prim.flags & 0x80) || (self->prim.flags & 0x100)) {
                    QUEUE_RENDER_STATE(0xe, 0);
                } else {
                    QUEUE_RENDER_STATE(0xe, 1);
                }
            }
            else {
                if (self->prim.colour_map->buffer.blended || (self->prim.flags & 0x80)) {
                    QUEUE_RENDER_STATE(0xe, 1);
                } else if (self->prim.flags & 0x100) {
                    QUEUE_RENDER_STATE(0xe, 1);
                } else {
                    QUEUE_RENDER_STATE(0xe, 0);
                }
            }
        }
        break;
    case 3:
        QUEUE_RENDER_STATE(0x17, 8);
        if (C2V(gTexture_format_is_NOT_ARGB4444)) {
            QUEUE_RENDER_STATE(0xe, 1);
        } else if (self->prim.colour_map != NULL && self->prim.colour_map->buffer.blended) {
            QUEUE_RENDER_STATE(0xe, 0);
        } else {
            if ((self->prim.flags & 0x80) || (self->prim.flags & 0x100)) {
                QUEUE_RENDER_STATE(0xe, 0);
            } else {
                QUEUE_RENDER_STATE(0xe, 1);
            }
        }
        break;
    }

    if (C2V(gTexture_format_is_NOT_ARGB4444)) {
        if (self->prim.flags & 0x80) {
            C2V(gINT_1001dc30) = (int)self->prim.alpha_val;
            C2V(gDWORD_1001bbb4) = C2V(gStruct_100381c0)[C2V(gINT_1001dc30) % 16].texture_handle;
        } else {
            C2V(gDWORD_1001bbb4) = 0;
        }
        if ((self->prim.flags & 0x80) || (self->prim.colour_map != NULL && self->prim.colour_map->buffer.blended)) {
            QUEUE_RENDER_STATE(0x29, 1);
        } else {
            QUEUE_RENDER_STATE(0x29, 0);
        }
        C2V(gINT_1001bbf0) = 2;
    } else {
        QUEUE_RENDER_STATE(0x29, 0);
        if (self->prim.flags & 0x80) {
            C2V(gINT_1001dc30) = (int)self->prim.alpha_val;
            C2V(gCOLOUR_100156e8) = C2V(gINT_1001dc30) << 24;
            QUEUE_RENDER_STATE(0x13, 5);
            QUEUE_RENDER_STATE(0x14, 6);
            QUEUE_RENDER_STATE(0x1b, 1);
            QUEUE_RENDER_STATE(0xe, 0);
            C2V(gINT_1001bbf0) = 4;
        } else if (self->prim.colour_map != NULL && self->prim.colour_map->buffer.blended) {
            C2V(gCOLOUR_100156e8) = 0xff000000;
            QUEUE_RENDER_STATE(0x13, 5);
            QUEUE_RENDER_STATE(0x14, 6);
            QUEUE_RENDER_STATE(0x1b, 1);
            QUEUE_RENDER_STATE(0xe, 0);
            C2V(gINT_1001bbf0) = 2;
        } else {
            C2V(gCOLOUR_100156e8) = 0xff000000;
            QUEUE_RENDER_STATE(0x1b, 0);
            QUEUE_RENDER_STATE(0xe, 0);
            C2V(gINT_1001bbf0) = 2;
        }
        return;
    }
}
C2_HOOK_FUNCTION(0x10005e30, FUN_10005e30)

void C2_HOOK_CDECL FUN_10006570(br_primitive_state_d3d* self, local_block_d3d* block) {

    if (C2V(gNo_2d_during_3d_scene) || C2V(gTexture_format_is_NOT_ARGB4444)) {
        return;
    }
    if ((self->prim.flags & PRIMF_BLEND) || (self->prim.colour_map != NULL && self->prim.colour_map->buffer.blended)) {
        block->p.flags |= 0x8;
    } else {
        block->p.flags &= ~0x8;
    }
}
C2_HOOK_FUNCTION(0x10006570, FUN_10006570)

br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_renderBegin(br_primitive_state_d3d* self, brp_block_d3d* *rpb, br_boolean* block_changed, br_boolean* ranges_changed, br_boolean no_render, br_token prim_type) {
    int j, b;
    local_block_d3d* pb;
    br_uint_32 flags;

    C2V(gINT_1001bbc4) = 0;
    ResetComputedOffsetsAndScales(self);
    *ranges_changed = 1;

    if (self->cache.last_type == prim_type
            && self->cache.timestamp_prim == self->prim.timestamp_major
            && self->cache.timestamp_out == self->out.timestamp_major) {

        pb = self->cache.last_block;
        *rpb = &pb->p;
        *block_changed = 0;
        if (no_render) {
            return 0;
        }
        FUN_10005e30(self, pb->flags_cmp);
        FUN_10006570(self, pb);
        if (pb->begin_fn != NULL) {
            pb->begin_fn(&pb->p);
        }
        return 0;
    }

    *block_changed = 1;

    if (self->out.pixelmap->buffer_type != BT_BACKSCREEN) {
        self->cache.last_block = NULL;
        return 0xa005;
    }
    flags = self->prim.flags;
    if (self->prim.colour_map != NULL) {
        flags |= PRIMF_TEXTURE_BUFFER;
    }
    if (self->out.depth != NULL) {
        flags |= PRIMF_DEPTH_BUFFER;
    }

    switch (prim_type) {
    case BRT_POINT:
        j = 0;
        break;
    case BRT_LINE:
        j = 1;
        break;
    case BRT_TRIANGLE:
        j = 2;
        break;
    default:
        self->cache.last_block = NULL;
        return 0xa005;
    }

    pb = C2V(primInfoD3DTable)[j].blocks;

    for (b = 0; b < primInfoD3DTable[j].nblocks; b++,pb++) {
        if ((flags & pb->flags_mask) == pb->flags_cmp) {
            break;
        }
    }
    if (b >= primInfoD3DTable[j].nblocks) {
        pb -= 1;
    }
    *rpb = &pb->p;

    if (pb == self->cache.last_block) {
        *block_changed = 0;
    } else {
        self->cache.last_block = pb;
    }
    self->cache.last_type = prim_type;
    self->cache.timestamp_prim = self->prim.timestamp_major;
    self->cache.timestamp_out = self->out.timestamp_major;
    pb->no_render = no_render;
    if (no_render) {
        return 0;
    }

    FUN_10005e30(self, pb->flags_cmp);
    FUN_10006570(self, pb);
    if (pb->begin_fn != NULL) {
        pb->begin_fn(&pb->p);
    }
    return 0;
}
C2_HOOK_FUNCTION(0x10006310, _M_br_primitive_state_d3d_renderBegin)

br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_renderEnd(br_primitive_state_d3d* self, brp_block_d3d* pb) {
    local_block_d3d *lb = (local_block_d3d *) pb;

    if (lb->no_render) {
        return 0;
    }
    if (lb->end_fn != NULL) {
        lb->end_fn(pb);
    }
    FUN_10007140();
    if (C2V(gINT_1001bbc4) == 0) {
        C2V(gINT_1001bbc0) += 1;
    }
    C2V(gINT_1001bbbc) += 1;
    return 0;
}
C2_HOOK_FUNCTION(0x100065b0, _M_br_primitive_state_d3d_renderEnd)

br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_rangesQueryF(br_primitive_state_d3d* self, br_float* offset, br_float* scale, br_int_32 max_comp) {
    int i;

    if (self->cache.timestamp_prim != self->prim.timestamp_major ||
            self->cache.timestamp_out != self->out.timestamp_major) {
        return 0xa001;
    }
    for (i = 0; i < max_comp; i++) {
        offset[i] = (float)self->cache.comp_offsets[i] / 65536.f;
        scale[i] = (float)self->cache.comp_scales[i] / 65536.f;
    }

    return 0;
}
C2_HOOK_FUNCTION(0x100065f0, _M_br_primitive_state_d3d_rangesQueryF)

br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_rangesQueryX(br_primitive_state_d3d* self, br_fixed_ls* offset, br_fixed_ls* scale, br_int_32 max_comp) {
    abort();
}
C2_HOOK_FUNCTION(0x10006660, _M_br_primitive_state_d3d_rangesQueryX)



#if 0
#if ARSE_DEBUG
#include <windows.h>
#include <stdio.h>
#endif

#include <math.h>

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

#include "texture.h"

/*
 * Invalid value for unknown pixelmap types
 */
#define PMT_NONE 255

extern br_primitive_library PrimitiveLibrary3Dfx;

/*
 * Fogging control table and cache
 */
static GrFog_t Local_Fog_Table[64];
static br_token Fog_Type_Old = BRT_NONE;
static br_uint_32 Fog_Colour_Old = 0;
static float Fog_Min_Old = 1.0f;
static float Fog_Max_Old = 0.0f;

float linear_fog[64];
float fog_max_q;
float fog_q_scale = 1.0f;

float alpha_value;

int fog_enabled = -1;

/*
 * Descriptions of all the available renderers
 */
static struct local_block primInfo_t15[] = {
#include "prim_t15.c"
};

static struct local_block primInfo_l15[] = {
#include "prim_l15.c"
};

static struct local_block primInfo_p15[] = {
#include "prim_p15.c"
};

static struct {
    struct local_block *blocks;
    int nblocks;
} primInfoD3DTable[3] = {
    { primInfo_p15, BR_ASIZE(primInfo_p15), },
    { primInfo_l15, BR_ASIZE(primInfo_l15), },
    { primInfo_t15, BR_ASIZE(primInfo_t15), },
};

/*
 * Quickly modified to return a boolean indicating if any changes were detected.  Also changed to
 * not touch anything that will never change (this is done in initConstantRanges now)
 */
static br_boolean updateRanges(struct br_primitive_state *self)
{
    br_boolean changed = BR_FALSE;
    br_scalar sx_scale, sx_offset, sy_scale, sy_offset, u_scale, v_scale;

    /* SX,SY range are based on current colour_buffer
     *
     * Multiplied by 16 so the vertex snapping can be accelerated */

    sx_scale = BrIntToScalar((self->out.colour.pixelmap->pm_width  / 2) * 16);
    sx_offset = BrIntToScalar(self->out.colour.pixelmap->pm_base_x * 16) + sx_scale;
    sy_scale = -BrIntToScalar((self->out.colour.pixelmap->pm_height  / 2) * 16);
    sy_offset = BrIntToScalar(self->out.colour.pixelmap->pm_base_y * 16) - sy_scale;

    changed |= (self->cache.comp_scales[C_SX] != sx_scale || self->cache.comp_offsets[C_SX] != sx_offset ||
        self->cache.comp_scales[C_SY] != sy_scale || self->cache.comp_offsets[C_SY] != sy_offset);

    self->cache.comp_offsets[C_SX] = sx_offset;
    self->cache.comp_scales[C_SX] = sx_scale;

    self->cache.comp_offsets[C_SY] =  sy_offset;
    self->cache.comp_scales[C_SY] = sy_scale;

    /* U,V range are set in SetupBuffer */
    if(self->prim.colour_map) {

        u_scale = self->prim.colour_map->buffer.u_range;
        v_scale = self->prim.colour_map->buffer.v_range;

        changed |= (self->cache.comp_scales[C_U] != u_scale || self->cache.comp_scales[C_V] != v_scale);

        self->cache.comp_scales[C_U] = u_scale;
        self->cache.comp_scales[C_V] = v_scale;
    }

    return changed;
}

void initConstantRanges(struct br_primitive_state *self)
{
    int i;

    /*
     * Everything defaults to offset = 0, scale = 1
     */
    for(i=0; i < NUM_COMPONENTS; i++) {
        self->cache.comp_offsets[i] = BR_SCALAR(0.0);
        self->cache.comp_scales[i] = BR_SCALAR(1.0);
    }

    /* SZ: BRender 1.3.0 now supports scaling on the Z buffer; in a slightly strange way,
     * where the range specifies the range either side of 0.
     * the 3Dfx seems to like 0-65535.0, although I don't think this is quite what the
     * documentation says, it seems to work. */
    self->cache.comp_offsets[C_SZ] = BR_SCALAR(32767);
    self->cache.comp_scales[C_SZ] = -BR_SCALAR(32767);

    /*
     * R,G,B,A are 0-255
     */
    self->cache.comp_scales[C_R] = BR_SCALAR(255.0);
    self->cache.comp_scales[C_G] = BR_SCALAR(255.0);
    self->cache.comp_scales[C_B] = BR_SCALAR(255.0);
    self->cache.comp_scales[C_A] = BR_SCALAR(255.0);
}




/*
 * Texture cache interface
 */


br_boolean texture_cache_created_3dfx = BR_FALSE;
void *current_palette = NULL;


br_boolean FXTextureFreedCallback(struct render_buffer *texture)
{
    texture->on_card = BR_FALSE;

    /* Doesn't have to do anything to unregister with GLIDE */
    return(BR_TRUE);
}




/*
 * Cache a texture in the card
 *
 * In;  the render buffer with all details of the texture
 */
br_error FXCacheTexture(struct render_buffer *src)
{
    br_int_32      card_mem_pos;      // Position of texture in card memory (relative to start of texture cache)
    br_uint_32     texture_size;      // Size inc all mipmaps
    GrMipMapId_t   id=0;


    /* Initialise texture cache if required */
    if (!texture_cache_created_3dfx) {
        br_uint_32 base, size;

        /* Initialise texture cache */
        base = grTexMinAddress( GR_TMU0 );
        size = grTexMaxAddress( GR_TMU0 ) - base;

        if (!TextureCacheInit(FXTextureFreedCallback))
            return(BRE_DEV_NO_MEMORY);

        /*
         * Add separate blocks for each 2MB chunk, as textures cannot cross
         * 2MB boundaries
         */
        while (size > 1024*2048) {

            if (!TextureCacheAddBlock(base, 1024*2048))
                return(BRE_DEV_NO_MEMORY);

            size -= 1024*2048;
            base += 1024*2048;
        }

        if (size > 0)
            if (!TextureCacheAddBlock(base, size))
                return(BRE_DEV_NO_MEMORY);

        texture_cache_created_3dfx = BR_TRUE;
    }

    /* Send In The Palettes if required */
    if (src->palette_pointer && (current_palette != src->palette_pointer)) {
        grTexDownloadTable(GR_TMU0, GR_TEXTABLE_PALETTE, src->palette_pointer);
        current_palette = src->palette_pointer;
    }

    /* If we need to force a reload, order the texture cache to clear any
     * entries corresponding to this texture, and make another attempt
     * to cache it (in case the failure condition has now been corrected) */
    if (src->force_reload) {
        TextureCacheClearEntry(src);
        src->force_reload = BR_FALSE;
    }


    /* Already uploaded? */
    if (src->on_card)
        return(BRE_OK);


    /* If it's been marked as uncacheable, don't suffer the overhead of
     * attempting to set it up again: failure conditions exist that
     * will mean it's never loadable. */
    if (src->uncacheable)
        return(BRE_DEV_FAIL);


    /* Ask the texture cache to sort itself out for this texture */
    texture_size = grTexTextureMemRequired(GR_MIPMAPLEVELMASK_BOTH, &src->info);
    card_mem_pos = TextureCacheRequest(src, texture_size);
    if (card_mem_pos < 0) {
        src->uncacheable = BR_TRUE; // Any failure here also means never renderable
        return(BRE_DEV_NO_VIDEO_MEMORY);
    }
    src->startAddress = card_mem_pos;

    /* Send it to the card */
    grTexDownloadMipMap(src->tmu_id, card_mem_pos,
                        GR_MIPMAPLEVELMASK_BOTH, &src->info);

    src->on_card = BR_TRUE;

    return(BRE_OK);
}




/*
 * Sort out everything to do with fog
 *
 * Completely rewritten by Dave
 */
void RecalculateFog(struct br_primitive_state *self)
{
    br_uint_32 i;
    float fog_scale, fog_offset, f;

    if (self->prim.fog_type == BRT_NONE) {

        if (fog_enabled != 0) {

            grFogMode(GR_FOG_DISABLE);
            fog_enabled = 0;
        }

        return;
    }

    if (fog_enabled != 1) {

        grFogMode(GR_FOG_WITH_TABLE);
        fog_enabled = 1;
    }

    if (self->prim.fog_colour != Fog_Colour_Old) {

        grFogColorValue(self->prim.fog_colour);
        Fog_Colour_Old = self->prim.fog_colour;
    }

    if (self->prim.fog_min != Fog_Min_Old ||
        self->prim.fog_max != Fog_Max_Old) {

        fog_q_scale = self->prim.fog_max * fog_max_q;

        f = 1.0f / (self->prim.fog_max - self->prim.fog_min);

        fog_scale = self->prim.fog_max * f;
        fog_offset = 255.0f * self->prim.fog_min * f;

        for (i = 0; i < 64; i++) {

            f = linear_fog[i] * fog_scale - fog_offset;

            Local_Fog_Table[i] = f > 0.0f? (FxU8)f: 0;
        }

        grFogTable(Local_Fog_Table);

        Fog_Min_Old = self->prim.fog_min;
        Fog_Max_Old = self->prim.fog_max;
    }
}



/* 3Dfx local state cache */
static int    blending_on = -1;
static int blending_alpha = -1;
static int    depth_on = -1;
int depth_write_on = 1;
static int    texture_on = -1;
static int    chroma_on = -1;
static int    filter_on = -1;
static int    colour_mode = -1;
static int    mipmap_mode = -1;
static int    depth_test = -1;
static int    blend_mode = -1;
static int    alphatest_mode = -1;
static int    u_clamp = -1;
static int    v_clamp = -1;


/*
 * Need to clear out the state when the driver is restarted
 * Pointed out by Matt G
 */
void Clear3DfxStateCache(void)
{
    blending_on = -1;
    blend_mode = -1;
    alphatest_mode = -1;
    blending_alpha = -1;
    depth_on = -1;
    depth_write_on = -1;
    texture_on = -1;
    chroma_on = -1;
    filter_on = -1;
    colour_mode = -1;
    mipmap_mode = -1;
    depth_write_on = 1;
}

/*
 * Set the 3Dfx state that has to be reset on every pass
 */
void Set3DfxState(struct br_primitive_state *self, br_uint_32 flags)
{
    int new_depth_test, new_u_clamp, new_v_clamp;

    /* Check and reset fogging parameters */
    RecalculateFog(self);

    /* Setup glide texture */
    if ((flags & PRIMF_TEXTURE_BUFFER) &&
        (FXCacheTexture(&self->prim.colour_map->buffer) == BRE_OK)) {

        if (flags & PRIMF_COLOUR_KEY) {
            if (chroma_on != 1) {
                grChromakeyMode(GR_CHROMAKEY_ENABLE);
                chroma_on = 1;
            }
        } else {
            if (chroma_on != 0) {
                grChromakeyMode(GR_CHROMAKEY_DISABLE);
                chroma_on = 0;
            }
        }

        grTexSource(self->prim.colour_map->buffer.tmu_id,
                    self->prim.colour_map->buffer.startAddress,
                    GR_MIPMAPLEVELMASK_BOTH,
                    &self->prim.colour_map->buffer.info );

        texture_on = 1;

        /* Set up filtering and mipmapping
         * BRender definition says map_antialiasing enables mipmapping, mip_interpolation
         * controls interpolation between different levels of mipmapping */
        if ((self->prim.map_interpolation != BRT_NONE) &&
            (self->prim.map_interpolation != BRT_NEAREST)) {
            if (filter_on != 1) {
                grTexFilterMode(self->prim.colour_map->buffer.tmu_id, GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
                filter_on = 1;
            }
        } else {
            if (filter_on != 0) {
                grTexFilterMode(self->prim.colour_map->buffer.tmu_id, GR_TEXTUREFILTER_POINT_SAMPLED, GR_TEXTUREFILTER_POINT_SAMPLED);
                filter_on = 0;
            }
        }

        if (self->prim.map_antialiasing != BRT_NONE) {
            if (self->prim.mip_interpolation != BRT_NONE) {
                if (mipmap_mode != 2) {
                    grTexMipMapMode(self->prim.colour_map->buffer.tmu_id, GR_MIPMAP_NEAREST_DITHER, FXFALSE);
                    grHints(GR_HINT_ALLOW_MIPMAP_DITHER, 1);
                    mipmap_mode = 2;
                }
            } else {
                if (mipmap_mode != 1) {
                    grTexMipMapMode(self->prim.colour_map->buffer.tmu_id, GR_MIPMAP_NEAREST, FXFALSE);
                    mipmap_mode = 1;
                }
            }
        } else {
            if (mipmap_mode != 0) {
                grTexMipMapMode(self->prim.colour_map->buffer.tmu_id, GR_MIPMAP_DISABLE, FXFALSE);
                mipmap_mode = 0;
            }
        }

        /* Setup colour combine unit for appropriate shade mode */
        if (flags & PRIMF_MODULATE) {
            if (flags & PRIMF_SMOOTH) {
                if (colour_mode != 0) {
                    guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ITRGB);
                    colour_mode = 0;
                }
            } else {
                if (colour_mode != 1) {
                    guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ITRGB_DELTA0);
                    colour_mode = 1;
                }
            }
        } else {
            if (colour_mode != 2) {
                guColorCombineFunction(GR_COLORCOMBINE_DECAL_TEXTURE);
                colour_mode = 2;
            }
        }

        /*
         * Set up clamping
         */
        new_u_clamp = self->prim.map_width_limit == BRT_CLAMP;
        new_v_clamp = self->prim.map_height_limit == BRT_CLAMP;

        if (u_clamp != new_u_clamp || v_clamp != new_v_clamp) {
            grTexClampMode(0, new_u_clamp, new_v_clamp);
            u_clamp = new_u_clamp;
            v_clamp = new_v_clamp;
        }

    } else {
        /* Not textured or texture load failed */
        if (texture_on != 0) {
            guTexSource(GR_NULL_MIPMAP_HANDLE);
            grChromakeyMode(GR_CHROMAKEY_DISABLE);

            chroma_on = 0;
            texture_on = 0;
        }

        /* Setup colour combine unit for appropriate shade mode */
        if (flags & PRIMF_SMOOTH) {
            if (colour_mode != 3) {
                guColorCombineFunction(GR_COLORCOMBINE_ITRGB);
                colour_mode = 3;
            }
        } else {
            if (colour_mode != 4) {
                guColorCombineFunction(GR_COLORCOMBINE_ITRGB_DELTA0);
                colour_mode = 4;
            }
        }
    }

    /* Setup Glide depth buffer */
    if (flags & PRIMF_DEPTH_BUFFER) {

        if (depth_on != 1) {
            grDepthBufferMode(GR_DEPTHBUFFER_ZBUFFER);
            depth_on = 1;
        }

        switch (self->prim.depth_test) {

        case BRT_GREATER:
            new_depth_test = GR_CMP_GREATER;
            break;

        case BRT_GREATER_OR_EQUAL:
            new_depth_test = GR_CMP_GEQUAL;
            break;

        case BRT_EQUAL:
            new_depth_test = GR_CMP_EQUAL;
            break;

        case BRT_NOT_EQUAL:
            new_depth_test = GR_CMP_NOTEQUAL;
            break;

        case BRT_LESS_OR_EQUAL:
            new_depth_test = GR_CMP_LEQUAL;
            break;

        case BRT_LESS:
            new_depth_test = GR_CMP_LESS;
            break;

        case BRT_NEVER:
            new_depth_test = GR_CMP_NEVER;
            break;

        case BRT_ALWAYS:
            new_depth_test = GR_CMP_ALWAYS;
            break;
        }

        if (depth_test != new_depth_test) {
            grDepthBufferFunction(new_depth_test);
            depth_test = new_depth_test;
        }

        if (flags & PRIMF_DEPTH_WRITE) {
            if (depth_write_on != FXTRUE) {
                grDepthMask(FXTRUE);
                depth_write_on = FXTRUE;
            }
        } else {
            if (depth_write_on != FXFALSE) {
                grDepthMask(FXFALSE);
                depth_write_on = FXFALSE;
            }
        }

    } else {

        if (depth_on != 0) {
            grDepthBufferMode(GR_DEPTHBUFFER_DISABLE);
            depth_on = 0;
        }

        if (depth_write_on != FXFALSE) {
            grDepthMask(FXFALSE);
            depth_write_on = FXFALSE;
        }
    }

    /* Setup Glide alpha blending */

#if 1
    if (self->prim.flags & PRIMF_BLEND) {
        if (self->prim.flags & PRIMF_SMOOTH_ALPHA) {
            if (self->prim.colour_map != NULL && self->prim.colour_map->buffer.blended) {
                if (blending_on != 5) {
#if ARSE_DEBUG
OutputARSE_DEBUGString("blending on = 5: iterated alpha * texture alpha\n");
#endif
                    grAlphaCombine(GR_COMBINE_FUNCTION_SCALE_OTHER,
                                   GR_COMBINE_FACTOR_LOCAL,
                                   GR_COMBINE_LOCAL_ITERATED,
                                   GR_COMBINE_OTHER_TEXTURE,
                                   FXFALSE);
                    blending_on = 5;
                }
            } else {
                if (blending_on != 4) {
#if ARSE_DEBUG
OutputARSE_DEBUGString("blending on = 4: iterated alpha\n");
#endif
                    grAlphaCombine(GR_COMBINE_FUNCTION_SCALE_OTHER,
                                   GR_COMBINE_FACTOR_ONE,
                                   GR_COMBINE_LOCAL_NONE,
                                   GR_COMBINE_OTHER_ITERATED,
                                   FXFALSE);
                    blending_on = 4;
                }
            }
        } else {
            if (self->prim.colour_map != NULL && self->prim.colour_map->buffer.blended) {
                if (blending_on != 3) {
#if ARSE_DEBUG
OutputARSE_DEBUGString("blending on = 3: constant alpha * texture alpha\n");
#endif
                    grAlphaCombine(GR_COMBINE_FUNCTION_SCALE_OTHER,
                                   GR_COMBINE_FACTOR_LOCAL,
                                   GR_COMBINE_LOCAL_CONSTANT,
                                   GR_COMBINE_OTHER_TEXTURE,
                                   FXFALSE);
                    blending_on = 3;
                }
            } else {
                if (blending_on != 2) {
#if ARSE_DEBUG
OutputARSE_DEBUGString("blending on = 2: constant alpha\n");
#endif
                    grAlphaCombine(GR_COMBINE_FUNCTION_SCALE_OTHER,
                                   GR_COMBINE_FACTOR_ONE,
                                   GR_COMBINE_LOCAL_NONE,
                                   GR_COMBINE_OTHER_CONSTANT,
                                   FXFALSE);
                    blending_on = 2;
                }
            }
        }
    } else {
        if (self->prim.colour_map != NULL && self->prim.colour_map->buffer.blended) {
            if (blending_on != 1) {
#if ARSE_DEBUG
OutputARSE_DEBUGString("blending on = 1: texture alpha\n");
#endif
                grAlphaCombine(GR_COMBINE_FUNCTION_SCALE_OTHER,
                               GR_COMBINE_FACTOR_ONE,
                               GR_COMBINE_LOCAL_NONE,
                               GR_COMBINE_OTHER_TEXTURE,
                               FXFALSE);
                blending_on = 1;
            }
        } else {
#if ARSE_DEBUG
if (blending_on != 0)
OutputARSE_DEBUGString("blending on = 0: no alpha\n");
#endif
            blending_on = 0;
        }
    }

    switch (self->prim.blend_mode) {

    case BRT_BLEND_STANDARD:
    case BRT_BLEND_DIMMED:

        if (blending_on != 0) {
            if (blend_mode != 1) {
#if ARSE_DEBUG
OutputARSE_DEBUGString("blending mode = 1: src * alpha + dest * (1 - alpha)\n");
#endif
                grAlphaBlendFunction(GR_BLEND_SRC_ALPHA, GR_BLEND_ONE_MINUS_SRC_ALPHA,
                                     GR_BLEND_ONE, GR_BLEND_ZERO);
                blend_mode = 1;
            }
            if (alphatest_mode != 1) {
                grAlphaTestFunction(GR_CMP_GREATER);
                alphatest_mode = 1;
            }
        } else {
            if (blend_mode != 0) {
#if ARSE_DEBUG
OutputARSE_DEBUGString("blending mode = 0: src\n");
#endif
                grAlphaBlendFunction(GR_BLEND_ONE, GR_BLEND_ZERO,
                                     GR_BLEND_ONE, GR_BLEND_ZERO);
                blend_mode = 0;
            }
            if (alphatest_mode != 0) {
                grAlphaTestFunction(GR_CMP_ALWAYS);
                alphatest_mode = 0;
            }
        }

        break;

    case BRT_BLEND_SUMMED:

        if (blending_on != 0) {
            if (blend_mode != 4) {
#if ARSE_DEBUG
OutputARSE_DEBUGString("blending mode = 4: src * alpha + dest\n");
#endif
                grAlphaBlendFunction(GR_BLEND_SRC_ALPHA, GR_BLEND_ONE,
                                     GR_BLEND_ONE, GR_BLEND_ZERO);
                blend_mode = 4;
            }
            if (alphatest_mode != 1) {
                grAlphaTestFunction(GR_CMP_GREATER);
                alphatest_mode = 1;
            }
        } else {
            if (blend_mode != 3) {
#if ARSE_DEBUG
OutputARSE_DEBUGString("blending mode = 3: src + dest\n");
#endif
                grAlphaBlendFunction(GR_BLEND_ONE, GR_BLEND_ONE,
                                     GR_BLEND_ONE, GR_BLEND_ZERO);
                blend_mode = 3;
            }
            if (alphatest_mode != 1) {
                grAlphaTestFunction(GR_CMP_GREATER);
                alphatest_mode = 1;
            }
        }

        break;

    case BRT_BLEND_PREMULTIPLIED:

        if (blending_on != 0) {
            if (blend_mode != 2) {
#if ARSE_DEBUG
OutputARSE_DEBUGString("blending mode = 2: src + dest * (1 - alpha)\n");
#endif
                grAlphaBlendFunction(GR_BLEND_ONE, GR_BLEND_ONE_MINUS_SRC_ALPHA,
                                     GR_BLEND_ONE, GR_BLEND_ZERO);
                blend_mode = 2;
            }
            if (alphatest_mode != 1) {
                grAlphaTestFunction(GR_CMP_GREATER);
                alphatest_mode = 1;
            }
        } else {
            if (blend_mode != 0) {
#if ARSE_DEBUG
OutputARSE_DEBUGString("blending mode = 0: src\n");
#endif
                grAlphaBlendFunction(GR_BLEND_ONE, GR_BLEND_ZERO,
                                     GR_BLEND_ONE, GR_BLEND_ZERO);
                blend_mode = 0;
            }
            if (alphatest_mode != 0) {
                grAlphaTestFunction(GR_CMP_ALWAYS);
                alphatest_mode = 0;
            }
        }

        break;
    }
#else
    switch (self->prim.blend_mode) {

    case BRT_BLEND_STANDARD:
    case BRT_BLEND_DIMMED:

        if (self->prim.flags & PRIMF_BLEND) {
            if (blending_on != 1) {
                guAlphaSource(GR_ALPHASOURCE_CC_ALPHA);
                blending_on = 1;
            }
            alpha_value = self->prim.alpha_val*255.0f;
            if (blending_alpha != (br_uint_32)alpha_value) {
                grConstantColorValue(((br_uint_32)alpha_value) << 24);
                blending_alpha = (br_uint_32)alpha_value;
            }
        } else if (self->prim.colour_map != NULL && self->prim.colour_map->buffer.blended) {
            if (blending_on != 2) {
                guAlphaSource(GR_ALPHASOURCE_TEXTURE_ALPHA);
                blending_on = 2;
            }
        } else
            blending_on = 0;

        if (blending_on != 0) {
            if (blend_mode != 1) {
                grAlphaBlendFunction(GR_BLEND_SRC_ALPHA, GR_BLEND_ONE_MINUS_SRC_ALPHA,
                                     GR_BLEND_ONE, GR_BLEND_ZERO);
                blend_mode = 1;
            }
            if (alphatest_mode != 1) {
                grAlphaTestFunction(GR_CMP_GREATER);
                alphatest_mode = 1;
            }
        } else {
            if (blend_mode != 0) {
                grAlphaBlendFunction(GR_BLEND_ONE, GR_BLEND_ZERO,
                                     GR_BLEND_ONE, GR_BLEND_ZERO);
                blend_mode = 0;
            }
            if (alphatest_mode != 0) {
                grAlphaTestFunction(GR_CMP_ALWAYS);
                alphatest_mode = 0;
            }
        }

        break;

    case BRT_BLEND_SUMMED:

        if (self->prim.flags & PRIMF_BLEND) {
            if (blending_on != 1) {
                guAlphaSource(GR_ALPHASOURCE_CC_ALPHA);
                blending_on = 1;
            }
            alpha_value = self->prim.alpha_val*255.0f;
            if (blending_alpha != (br_uint_32)alpha_value) {
                grConstantColorValue(((br_uint_32)alpha_value) << 24);
                blending_alpha = (br_uint_32)alpha_value;
            }
            if (blend_mode != 4) {
                grAlphaBlendFunction(GR_BLEND_SRC_ALPHA, GR_BLEND_ONE,
                                     GR_BLEND_ONE, GR_BLEND_ZERO);
                blend_mode = 4;
            }
            if (alphatest_mode != 1) {
                grAlphaTestFunction(GR_CMP_GREATER);
                alphatest_mode = 1;
            }
        } else if (self->prim.colour_map != NULL && self->prim.colour_map->buffer.blended) {
            if (blending_on != 2) {
                guAlphaSource(GR_ALPHASOURCE_TEXTURE_ALPHA);
                blending_on = 2;
            }
            if (blend_mode != 4) {
                grAlphaBlendFunction(GR_BLEND_SRC_ALPHA, GR_BLEND_ONE,
                                     GR_BLEND_ONE, GR_BLEND_ZERO);
                blend_mode = 4;
            }
            if (alphatest_mode != 1) {
                grAlphaTestFunction(GR_CMP_GREATER);
                alphatest_mode = 1;
            }
        } else {
            blending_on = 0;
            if (blend_mode != 3) {
                grAlphaBlendFunction(GR_BLEND_ONE, GR_BLEND_ONE,
                                     GR_BLEND_ONE, GR_BLEND_ZERO);
                blend_mode = 3;
            }
            if (alphatest_mode != 0) {
                grAlphaTestFunction(GR_CMP_ALWAYS);
                alphatest_mode = 0;
            }
        }

        break;

    case BRT_BLEND_PREMULTIPLIED:

        if (self->prim.flags & PRIMF_BLEND) {
            if (blending_on != 1) {
                guAlphaSource(GR_ALPHASOURCE_CC_ALPHA);
                blending_on = 1;
            }
            alpha_value = self->prim.alpha_val*255.0f;
            if (blending_alpha != (br_uint_32)alpha_value) {
                grConstantColorValue(((br_uint_32)alpha_value) << 24);
                blending_alpha = (br_uint_32)alpha_value;
            }
        } else if (self->prim.colour_map != NULL && self->prim.colour_map->buffer.blended) {
            if (blending_on != 2) {
                guAlphaSource(GR_ALPHASOURCE_TEXTURE_ALPHA);
                blending_on = 2;
            }
        } else
            blending_on = 0;

        if (blending_on != 0) {
            if (blend_mode != 3) {
                grAlphaBlendFunction(GR_BLEND_ONE, GR_BLEND_ONE_MINUS_SRC_ALPHA,
                                     GR_BLEND_ONE, GR_BLEND_ZERO);
                blend_mode = 3;
            }
            if (alphatest_mode != 1) {
                grAlphaTestFunction(GR_CMP_GREATER);
                alphatest_mode = 1;
            }
        } else {
            if (blend_mode != 0) {
                grAlphaBlendFunction(GR_BLEND_ONE, GR_BLEND_ZERO,
                                     GR_BLEND_ONE, GR_BLEND_ZERO);
                blend_mode = 0;
            }
            if (alphatest_mode != 0) {
                grAlphaTestFunction(GR_CMP_ALWAYS);
                alphatest_mode = 0;
            }
        }

        break;
    }
#endif
}





/*
 * Start of a new block: parse renderer state
 */
br_error BR_CMETHOD_DECL(br_primitive_state_3dfx, renderBegin)(
        struct br_primitive_state *self,
        struct brp_block **rpb,
        br_boolean *block_changed,
        br_boolean *ranges_changed,
        br_boolean no_render,
        br_token prim_type)
{
    int j, b;
    struct local_block *pb;
    br_uint_32 flags;


    ASSERT(rpb);


    /* Derive the mode flags; we could probably store this away somewhere
     * Doing this eliminates a lot of extra prim_ states, and make the
     * Set3DfxState function a little less confusing */
    flags = self->prim.flags;
    if (self->prim.colour_map)
        flags |= PRIMF_TEXTURE_BUFFER;
    if (self->out.depth.pixelmap)
        flags |= PRIMF_DEPTH_BUFFER;

    if (self->prim.fog_type != BRT_NONE)
        flags |= PRIMF_FOG;

    /*
     * If previous match is still valid, return that
     */
    if (self->cache.last_type == prim_type) {

        if (self->cache.timestamp_prim == self->prim.timestamp_major &&
            self->cache.timestamp_out == self->out.timestamp_major) {

            pb = self->cache.last_block;
            *rpb = &pb->p;

            *block_changed = BR_FALSE;
            *ranges_changed = BR_FALSE;

            /*
             * Setup glide state
             */
            if (no_render)
                return BRE_OK;


            Set3DfxState(self, flags);

            /*
             * Faff around with the block a bit
             */
            if (blend_mode != 0)
                pb->p.flags |= BR_PRIMF_BLENDED;
            else
                pb->p.flags &= ~BR_PRIMF_BLENDED;

            /* Call the begin function (if present) */
            if (pb->begin_fn)
                pb->begin_fn(&pb->p);

            return BRE_OK;
        }
    }


    /* The current block has probably changed if we make it here.
     * Assume the worst, that changes will have happened */
    *block_changed = BR_TRUE;

    /*
     * Try to be clever and work out if the ranges have changed
     */
    *ranges_changed = updateRanges(self);

    /* Can render only to back buffers */
    if (self->out.colour.pixelmap->buffer_type != BT_BACKSCREEN) {
        self->cache.last_block = NULL;
        return BRE_DEV_UNSUPPORTED;
    }

    switch(prim_type) {
    case BRT_POINT:           j = 0;    break;
    case BRT_LINE:            j = 1;    break;
    case BRT_TRIANGLE:        j = 2;    break;
    default:
        self->cache.last_block = NULL;
        return BRE_DEV_UNSUPPORTED;
    }

    /*
     * Match against selected primitives
     */
    pb = primInfoD3DTable[j].blocks;

    for (b=0; b < primInfoD3DTable[j].nblocks; b++,pb++) {
        /* Do the flags match */
        if ((flags & pb->flags_mask) == pb->flags_cmp)
            break;
    }

    /* Default to last primitive in block */
    if(b >= primInfoD3DTable[j].nblocks)
        pb--;

    /* return pointer to primitive, and set 'unchanged' if this
     * pointer is the same as previous match */
    *rpb = &pb->p;

    if (pb == self->cache.last_block)
        *block_changed = BR_FALSE;
    else
        self->cache.last_block = pb;

    /* Update the caching (confidence is NOT high) */
    self->cache.last_type = prim_type;
    self->cache.timestamp_prim = self->prim.timestamp_major;
    self->cache.timestamp_out = self->out.timestamp_major;

    /* Stash copy of no_render flag in block */
    pb->no_render = no_render;

    /* Setup glide state */
    if (no_render)
        return BRE_OK;

    Set3DfxState(self, flags);

    /*
     * Faff around with the block a bit
     */
    if (blend_mode != 0)
        pb->p.flags |= BR_PRIMF_BLENDED;
    else
        pb->p.flags &= ~BR_PRIMF_BLENDED;

    if (pb->begin_fn)
        pb->begin_fn(&pb->p);

    return BRE_OK;
}


br_error BR_CMETHOD_DECL(br_primitive_state_3dfx, renderEnd)(
        struct br_primitive_state *self,
        struct brp_block *pb)
{
    struct local_block *lb = (struct local_block *)pb;

    if (lb->no_render)
        return BRE_OK;

    if (lb->end_fn)
        lb->end_fn(pb);

    return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_primitive_state_3dfx, rangesQueryF)(
        struct br_primitive_state *self,
        br_float *offset,
        br_float *scale,
        br_int_32 max_comp)
{
    int i;

    /*
     * Fail if the current info is not valid
     */
    if (self->cache.timestamp_prim != self->prim.timestamp_major ||
        self->cache.timestamp_out != self->out.timestamp_major)
        return BRE_DEV_FAIL;

    for(i=0; i < max_comp; i++) {
        offset[i] = BrScalarToFloat(self->cache.comp_offsets[i]);
        scale[i] = BrScalarToFloat(self->cache.comp_scales[i]);
    }

    return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_primitive_state_3dfx, rangesQueryX)(
        struct br_primitive_state *self,
        br_fixed_ls *offset,
        br_fixed_ls *scale,
        br_int_32 max_comp)
{
    int i;

    /*
     * Fail if the current info is not valid
     */
    if (self->cache.timestamp_prim != self->prim.timestamp_major ||
        self->cache.timestamp_out != self->out.timestamp_major)
        return BRE_DEV_FAIL;

    for(i=0; i < max_comp; i++) {
        offset[i] = BrScalarToFixed(self->cache.comp_offsets[i]);
        scale[i] = BrScalarToFixed(self->cache.comp_scales[i]);
    }

    return BRE_OK;
}

#endif
