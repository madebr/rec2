#ifndef D3D_MATCH_H
#define D3D_MATCH_H

#include "types.h"

#include "c2_hooks.h"


br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_renderBegin(br_primitive_state_d3d* self, brp_block_d3d* *rpb, br_boolean* block_changed, br_boolean* ranges_changed, br_boolean no_render, br_token prim_type);

br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_renderEnd(br_primitive_state_d3d* self, brp_block_d3d* pb);

br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_rangesQueryF(br_primitive_state_d3d* self, br_float* offset, br_float* scale, br_int_32 max_comp);

br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_rangesQueryX(br_primitive_state_d3d* self, br_fixed_ls* offset, br_fixed_ls* scale, br_int_32 max_comp);

#if 0

/*
 * Local version of brp_info extended with information used to match primitve with current state
 */

typedef void BR_ASM_CALL render_begin_fn(struct brp_block *block);
typedef void BR_ASM_CALL render_end_fn(struct brp_block *block);

struct local_block {
	brp_block	p;

	/* Mask of match flags */
	br_uint_32	flags_mask;
	br_uint_32	flags_cmp;

	/* Pointers to block specific Begin/End routines */
	render_begin_fn *begin_fn;
	render_end_fn *end_fn;

	/* Copy of no-rendering flag */
	br_boolean no_render;
};

/*
 * Valid range flags
 */
enum {
	RF_DECAL		= 0x0008,
	RF_RGB_SHADE	= 0x0010,
};

/* 3Dfx local state cache */
extern int depth_write_on;

extern float linear_fog[64];
extern float fog_max_q;
extern float fog_q_scale;
#endif

#endif /* D3D_MATCH_H*/
