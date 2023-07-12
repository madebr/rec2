#include "netgame.h"

#include "globvars.h"
#include "platform.h"

void C2_HOOK_FASTCALL DefaultNetName(void) {

    /* FIXME: 32 overflows the player_name field */
    C2_HOOK_ASSERT((uintptr_t)&C2V(gProgram_state).player_name == 0x0075d590);
    NetObtainSystemUserName(C2V(gProgram_state).player_name, 32);
}
C2_HOOK_FUNCTION(0x0049bd10, DefaultNetName)

void C2_HOOK_FASTCALL NetObtainSystemUserName(char* pName, int pMax_length) {

    PDNetObtainSystemUserName(pName, pMax_length);
    pName[9] = '\0';
}

C2_HOOK_FUNCTION(0x0049ee20, NetObtainSystemUserName)
