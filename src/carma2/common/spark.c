#include "spark.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gSmoke_on, 0x00660110, 1);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gShade_list, 16, 0x006b7840);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int*, gShade_table, 0x00660144, &gShade_list[8]);

void C2_HOOK_FASTCALL SetSmokeOn(int pSmoke_on) {

    C2V(gSmoke_on) = pSmoke_on;
}
C2_HOOK_FUNCTION(0x004fc9d0, SetSmokeOn)

int C2_HOOK_FASTCALL GetSmokeOn(void) {

    return C2V(gSmoke_on);
}
C2_HOOK_FUNCTION(0x004fca30, GetSmokeOn)

void C2_HOOK_FASTCALL GenerateSmokeShades(void) {
    static int rb = 0x00;
    static int gb = 0x00;
    static int bb = 0x00;

    static int rl = 0x40;
    static int gl = 0x40;
    static int bl = 0x40;

    static int rs = 0xc8;
    static int gs = 0xc8;
    static int bs = 0xc8;

    static int rm = 0x80;
    static int gm = 0x80;
    static int bm = 0x80;

    static int rd = 0xfe;
    static int gd = 0xdf;
    static int bd = 0x43;

#define RGB(R, G, B) (((R) << 16) | ((G) << 8) | ((B) << 0))

    C2V(gShade_list)[0] = RGB(rb, gb, bb);
    C2V(gShade_list)[1] = RGB(rl, gl, bl);
    C2V(gShade_list)[3] = RGB(rs, gs, bs);
    C2V(gShade_list)[4] = C2V(gShade_list)[3];
    C2V(gShade_list)[2] = RGB(rm, gm, bm);
    C2V(gShade_list)[7] = RGB(rd, gd, bd);

#undef RGB

}
C2_HOOK_FUNCTION(0x004fb910, GenerateSmokeShades)
