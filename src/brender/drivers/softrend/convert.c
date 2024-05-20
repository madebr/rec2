#include "convert.h"

#include "setup.h"

#define FCONV       (float)
#define XCONV(V)    (br_fixed_ls)((V)*65536.f)
#define ICONV       (br_int_32)

void (C2_HOOK_STDCALL * ConvertVertex_original)(brp_vertex* dest, brp_vertex* src);
void C2_HOOK_STDCALL ConvertVertex(brp_vertex* dest, brp_vertex* src) {

#if 0//defined(C2_HOOKS_ENABLED)
    ConvertVertex_original(dest, src);
#else
    int c;
    br_uint_32 m;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(brp_block, convert_mask_f, 0x20);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(brp_block, convert_mask_x, 0x24);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(brp_block, convert_mask_i, 0x28);

    m = C2V(rend).block->convert_mask_f;
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 0x1) {
                dest[0].comp_f[c] = FCONV(src->comp[c]);
            }
        }
    }

    m = C2V(rend).block->convert_mask_x;
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 0x1) {
                dest[0].comp_x[c] = XCONV(src->comp[c]);
            }
        }
    }

    m = C2V(rend).block->convert_mask_i;
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 0x1) {
                dest[0].comp_i[c] = ICONV(src->comp[c]);
            }
        }
    }

    m = 0x0000ffff ^ (C2V(rend).block->convert_mask_i | C2V(rend).block->convert_mask_x | C2V(rend).block->convert_mask_f);
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 0x1) {
                dest[0].comp[c] = src->comp[c];
            }
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005460e0, ConvertVertex, ConvertVertex_original)

void C2_HOOK_CDECL RenderConvert1(brp_block* block, brp_vertex* v0) {
    int c;
    br_uint_32 m;
    brp_vertex outv[1];

    m = C2V(rend).block->convert_mask_f;
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 1) {
                outv[0].comp_f[c] = FCONV(v0->comp[c]);
            }
        }
    }

    m = C2V(rend).block->convert_mask_x;
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 1) {
                outv[0].comp_x[c] = XCONV(v0->comp[c]);
            }
        }
    }

    m = C2V(rend).block->convert_mask_i;
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 1) {
                outv[0].comp_i[c] = ICONV(v0->comp[c]);
            }
        }
    }

    m = 0x0000ffff ^ (C2V(rend).block->convert_mask_i | C2V(rend).block->convert_mask_x | C2V(rend).block->convert_mask_f);
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 1) {
                outv[0].comp_i[c] = v0->comp_i[c];
            }
        }
    }

    block->chain->render(block->chain, outv);
}
C2_HOOK_FUNCTION(0x005461b0, RenderConvert1)

void C2_HOOK_CDECL RenderConvert2(brp_block* block, brp_vertex* v0, brp_vertex* v1) {
    int c;
    br_uint_32 m;
    brp_vertex outv[2];

    m = C2V(rend).block->convert_mask_f;
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 1) {
                outv[0].comp_f[c] = FCONV(v0->comp[c]);
                outv[1].comp_f[c] = FCONV(v1->comp[c]);
            }
        }
    }

    m = C2V(rend).block->convert_mask_x;
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 1) {
                outv[0].comp_x[c] = XCONV(v0->comp[c]);
                outv[1].comp_x[c] = XCONV(v1->comp[c]);
            }
        }
    }

    m = C2V(rend).block->convert_mask_i;
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 1) {
                outv[0].comp_i[c] = ICONV(v0->comp[c]);
                outv[1].comp_i[c] = ICONV(v1->comp[c]);
            }
        }
    }

    m = 0x0000ffff ^ (C2V(rend).block->convert_mask_i | C2V(rend).block->convert_mask_x | C2V(rend).block->convert_mask_f);
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 1) {
                outv[0].comp_i[c] = v0->comp_i[c];
                outv[1].comp_i[c] = v1->comp_i[c];
            }
        }
    }

    block->chain->render(block->chain, &outv[0], &outv[1]);
}
C2_HOOK_FUNCTION(0x00546290, RenderConvert2)

void C2_HOOK_CDECL RenderConvert3(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2) {
    int c;
    br_uint_32 m;
    brp_vertex outv[3];

    m = C2V(rend).block->convert_mask_f;
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 1) {
                outv[0].comp_f[c] = FCONV(v0->comp[c]);
                outv[1].comp_f[c] = FCONV(v1->comp[c]);
                outv[2].comp_f[c] = FCONV(v2->comp[c]);
            }
        }
    }

    m = C2V(rend).block->convert_mask_x;
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 1) {
                outv[0].comp_x[c] = XCONV(v0->comp[c]);
                outv[1].comp_x[c] = XCONV(v1->comp[c]);
                outv[2].comp_x[c] = XCONV(v2->comp[c]);
            }
        }
    }

    m = C2V(rend).block->convert_mask_i;
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 1) {
                outv[0].comp_i[c] = ICONV (v0->comp[c]);
                outv[1].comp_i[c] = ICONV (v1->comp[c]);
                outv[2].comp_i[c] = ICONV (v2->comp[c]);
            }
        }
    }


    m = 0x0000ffff ^ (C2V(rend).block->convert_mask_i | C2V(rend).block->convert_mask_x | C2V(rend).block->convert_mask_f);
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 1) {
                outv[0].comp_i[c] = v0->comp_i[c];
                outv[1].comp_i[c] = v1->comp_i[c];
                outv[2].comp_i[c] = v2->comp_i[c];
            }
        }
    }

    block->chain->render(block->chain, &outv[0], &outv[1], &outv[2]);
}
C2_HOOK_FUNCTION(0x005463f0, RenderConvert3)

void C2_HOOK_CDECL RenderConvert4(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, brp_vertex* v3) {
    int c;
    br_uint_32 m;
    brp_vertex outv[4];

    m = C2V(rend).block->convert_mask_f;
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 1) {
                outv[0].comp_f[c] = FCONV(v0->comp[c]);
                outv[1].comp_f[c] = FCONV(v1->comp[c]);
                outv[2].comp_f[c] = FCONV(v2->comp[c]);
                outv[3].comp_f[c] = FCONV(v3->comp[c]);
            }
        }
    }

    m = C2V(rend).block->convert_mask_x;
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 1) {
                outv[0].comp_x[c] = XCONV(v0->comp[c]);
                outv[1].comp_x[c] = XCONV(v1->comp[c]);
                outv[2].comp_x[c] = XCONV(v2->comp[c]);
                outv[3].comp_x[c] = XCONV(v3->comp[c]);
            }
        }
    }

    m = C2V(rend).block->convert_mask_i;
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 1) {
                outv[0].comp_i[c] = ICONV (v0->comp[c]);
                outv[1].comp_i[c] = ICONV (v1->comp[c]);
                outv[2].comp_i[c] = ICONV (v2->comp[c]);
                outv[3].comp_i[c] = ICONV (v3->comp[c]);
            }
        }
    }

    m = 0x0000ffff ^ (C2V(rend).block->convert_mask_i | C2V(rend).block->convert_mask_x | C2V(rend).block->convert_mask_f);
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if(m & 1) {
                outv[0].comp_i[c] = v0->comp_i[c];
                outv[1].comp_i[c] = v1->comp_i[c];
                outv[2].comp_i[c] = v2->comp_i[c];
                outv[3].comp_i[c] = v3->comp_i[c];
            }
        }
    }

    block->chain->render(block->chain, &outv[0], &outv[1], &outv[2], &outv[3]);
}
C2_HOOK_FUNCTION(0x005465e0, RenderConvert4)
