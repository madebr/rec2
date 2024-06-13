#ifndef SOFTREND_LIGHTMAC_H
#define SOFTREND_LIGHTMAC_H

#define SPECULARPOW_CUTOFF 0.6172f

#define CLAMP_SCALE(c) do {                                             \
    if (comp[(c)] >= 1.f) {                                             \
        comp[(c)] = self->state.cache.comp_offsets[(c)]                 \
            + self->state.cache.comp_scales[(c)] - BR_SCALAR_EPSILON;   \
    } else if (comp[(c)] <= 0.f) {                                      \
        comp[(c)] = self->state.cache.comp_offsets[(c)];                \
    } else {                                                            \
        comp[(c)] = self->state.cache.comp_scales[(c)] * comp[(c)]      \
            + self->state.cache.comp_offsets[(c)];                      \
    }                                                                   \
} while(0)

#define SPECULAR_POWER(l) (dot * (l) / (self->state.surface.power - self->state.surface.power * dot + dot))

#endif /* SOFTREND_LIGHTMAC_H */
