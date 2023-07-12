#include "demo.h"

#include "globvars.h"
#include "loading.h"
#include "utility.h"

#include "c2_hooks.h"
#include "c2_stdio.h"

C2_HOOK_VARIABLE_IMPLEMENT(int, gDemo, 0x00762284);

void C2_HOOK_FASTCALL CheckDemo(void) {
    FILE* f;
    tPath_name path;

    PathCat(path, C2V(gApplication_path), "DEMO.TXT");
    f = TWT_fopen(path, "rt");
    C2V(gDemo) = f != NULL;
    if (C2V(gDemo)) {
        DRfclose(f);
    }
}
C2_HOOK_FUNCTION(0x004815d0, CheckDemo)
