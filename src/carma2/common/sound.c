#include "sound.h"

#include "globvars.h"
#include "loading.h"
#include "utility.h"

#include <string.h>


C2_HOOK_VARIABLE_IMPLEMENT(int, gCD_fully_installed, 0x0068b898);

void C2_HOOK_FASTCALL UsePathFileToDetermineIfFullInstallation() {
    char line1[80];
    char line2[80];
    char line3[80];
    char path_file[80];
    DRFILE* fp;

    strcpy(path_file, C2V(gApplication_path));
    strcat(path_file, (char*)C2V(gDir_separator));
    strcat(path_file, "PATHS.TXT");

    if (PDCheckDriveExists(path_file)) {
        fp = DRfopen(path_file, "rt");
        if (fp == NULL) {
            return;
        }
        line1[0] = '\0';
        line2[0] = '\0';
        line3[0] = '\0';
        DRreadline(fp, line1);
        DRreadline(fp, line2);
        DRreadline(fp, line3);
        DRfclose(fp);
        if (strcmp(line3, "Full") != 0) {
            return;
        }
    }
    C2V(gCD_fully_installed) = 1;
}
C2_HOOK_FUNCTION(0x00454f40, UsePathFileToDetermineIfFullInstallation)
