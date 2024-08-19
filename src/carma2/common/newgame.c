#include "newgame.h"

#include "globvars.h"
#include "loading.h"
#include "utility.h"

#include "rec2_types.h"

#include "rec2_macros.h"
#include "c2_stdio.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tNet_game_options, gNet_settings, 9, 0x00763960);

void C2_HOOK_FASTCALL DefaultNetSettings(void) {
    FILE* file;
    tNet_game_options* pOptions;
    tPath_name the_path;
    int i;

    PathCat(the_path, C2V(gApplication_path), "NETDFLT.TXT");
    file = DRfopen(the_path, "rt");
    if (file == NULL) {
        return;
    }
    ReadNetworkSettings(file, &C2V(gNet_settings)[0]);
    PFrewind(file);
    for (i = 1; i < REC2_ASIZE(C2V(gNet_settings)); i++) {
        pOptions = &C2V(gNet_settings)[i];
        ReadNetworkSettings(file, pOptions);
    }
    PFfclose(file);
}
C2_HOOK_FUNCTION(0x004663e0, DefaultNetSettings)
