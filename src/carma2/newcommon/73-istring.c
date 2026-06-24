#include "73-istring.h"

#include "08-loading1.h"
#include "41-utility.h"
#include "52-errors.h"
#include "70-packfile.h"
#include "globvars.h"
#include "rec2_macros.h"
#include "rec2_types.h"

#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x0068c6e0
int gCount_interface_strings;

// GLOBAL: CARMA2_HW 0x0068c230
char* gInterface_strings[300];

// FUNCTION: CARMA2_HW 0x00484fd0
void C2_HOOK_FASTCALL IString_Load(void) {
    tPath_name the_path;
    FILE* f;
    char s[256];
    int i;

    if (gCount_interface_strings != 0) {
        return;
    }
    PathCat(the_path, gApplication_path, "INTRFACE");
    PathCat(the_path, the_path, "interface.txt");
    f = PFfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotOpenTEXT_TXT);
    }
    gCount_interface_strings = 0;
    for (i = 0; i < (int)REC2_ASIZE(gInterface_strings); i++) {
        if (PFfeof(f)) {
            break;
        }
        GetALineAndDontArgue(f, s);
        gInterface_strings[i] = BrMemAllocate(strlen(s) + 1, kMem_misc_string);
        strcpy(gInterface_strings[i], s);
        gCount_interface_strings += 1;
    }
    PFfclose(f);
}

// FUNCTION: CARMA2_HW 0x004850d0
void C2_HOOK_FASTCALL IString_Free(void) {
    int i;

    for (i = 0; i < gCount_interface_strings; i++) {
        BrMemFree(gInterface_strings[i]);;
    }
    gCount_interface_strings = 0;
}

// FUNCTION: CARMA2_HW 0x00485110
const char* C2_HOOK_FASTCALL IString_Get(int pIndex) {

    if (pIndex > gCount_interface_strings) {
        return NULL;
    }
    return gInterface_strings[pIndex];
}
