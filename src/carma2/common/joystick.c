#include "joystick.h"

#include "globvars.h"
#include "loading.h"
#include "utility.h"

#include "rec2_types.h"

#include "c2_string.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(float, gForce_feedback_upper_limit, 0x00655e58, 2.7f);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(float, Force_feedback_lower_limit, 0x00655e5c, 1.5f);

void C2_HOOK_STDCALL ReadFFB_TXT(void) {
    tPath_name path;
    FILE *f;
    char s[256];

    PathCat(path, C2V(gApplication_path), "FFB.TXT");
    f = DRfopen(path, "rt");
    GetALineAndDontArgue(f, s);
    c2_sscanf(c2_strtok(s, "\t ,/"), "%f", &C2V(gForce_feedback_upper_limit));
    GetALineAndDontArgue(f, s);
    c2_sscanf(c2_strtok(s, "\t ,/"), "%f", &C2V(Force_feedback_lower_limit));
    DRfclose(f);
}

int (C2_HOOK_FASTCALL * PlayFFBEffect_original)(const char* pEffect_name, int pArg2);
int C2_HOOK_FASTCALL PlayFFBEffect(const char* pEffect_name, int pArg2) {

#if defined(C2_HOOKS_ENABLED)
    return PlayFFBEffect_original(pEffect_name, pArg2);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045c6b0, PlayFFBEffect, PlayFFBEffect_original)
