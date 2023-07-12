#include "netgame.h"

#include "globvars.h"
#include "platform.h"

void C2_HOOK_FASTCALL DefaultNetName(void) {

    NetObtainSystemUserName(C2V(gProgram_state).player_name, 0x20);
}
C2_HOOK_FUNCTION(0x0049bd10, DefaultNetName)

void C2_HOOK_FASTCALL NetObtainSystemUserName(char* pName, int pMax_length) {

    PDNetObtainSystemUserName(pName, pMax_length);
    pName[9] = '\0';
}

C2_HOOK_FUNCTION(0x0049ee20, NetObtainSystemUserName)
