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
