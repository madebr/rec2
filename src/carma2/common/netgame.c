#include "netgame.h"

#include "globvars.h"
#include "opponent.h"
#include "platform.h"

#include <brender/brender.h>

C2_HOOK_VARIABLE_IMPLEMENT(int, gInitialised_grid, 0x0074a738);
C2_HOOK_VARIABLE_IMPLEMENT(int, gIt_or_fox, 0x0074a734);

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

void C2_HOOK_FASTCALL DisableCar(tCar_spec* pCar) {

    if (pCar->driver_name[0] != '\0') {
        if (!pCar->disabled) {
            pCar->disabled = 1;
            ForceRebuildActiveCarList();
        }
        if (pCar->car_master_actor->t.t.mat.m[3][0] < 500.0f) {
            BrVector3Accumulate(&pCar->car_master_actor->t.t.translate.t, &C2V(gInitial_position));
            BrVector3Copy((br_vector3*)pCar->old_frame_mat.m[3], &pCar->car_master_actor->t.t.translate.t);
        }
    }
}
C2_HOOK_FUNCTION(0x00499260, DisableCar)

void C2_HOOK_FASTCALL EnableCar(tCar_spec* pCar) {

    if (pCar->driver_name[0] != '\0') {
        if (pCar->disabled) {
            pCar->disabled = 0;
            ForceRebuildActiveCarList();
        }
        if (pCar->car_master_actor->t.t.mat.m[3][0] > 500.0f) {
            BrVector3Sub(&pCar->car_master_actor->t.t.translate.t, &pCar->car_master_actor->t.t.translate.t, &C2V(gInitial_position));
            BrVector3Copy((br_vector3*)pCar->old_frame_mat.m[3], &pCar->car_master_actor->t.t.translate.t);
        }
    }
}
C2_HOOK_FUNCTION(0x004992e0, EnableCar)

void (C2_HOOK_FASTCALL * InitPlayers_original)(void);
void C2_HOOK_FASTCALL InitPlayers(void) {

#if defined(C2_HOOKS_ENABLED)
    InitPlayers_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0049b720, InitPlayers, InitPlayers_original)

void C2_HOOK_FASTCALL InitNetGameplayStuff(void) {

}
C2_HOOK_FUNCTION(0x0049bd00, InitNetGameplayStuff)

void (C2_HOOK_FASTCALL * DeclareWinner_original)(int pWinner_index);
void C2_HOOK_FASTCALL DeclareWinner(int pWinner_index) {

#if defined(C2_HOOKS_ENABLED)
    DeclareWinner_original(pWinner_index);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0049a760, DeclareWinner, DeclareWinner_original)

void (C2_HOOK_FASTCALL * SendGameplay_original)(tPlayer_ID pPlayer, tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4);
void C2_HOOK_FASTCALL SendGameplay(tPlayer_ID pPlayer, tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4) {

#if defined(C2_HOOKS_ENABLED)
    SendGameplay_original(pPlayer, pMess, pParam_1, pParam_2, pParam_3, pParam_4);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0049bc20, SendGameplay, SendGameplay_original)

void (C2_HOOK_FASTCALL * SendGameplayToAllPlayers_original)(tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4);
void C2_HOOK_FASTCALL SendGameplayToAllPlayers(tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4) {

#if defined(C2_HOOKS_ENABLED)
    SendGameplayToAllPlayers_original(pMess, pParam_1, pParam_2, pParam_3, pParam_4);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0049bc70, SendGameplayToAllPlayers, SendGameplayToAllPlayers_original)

void (C2_HOOK_FASTCALL * DoNetGameManagement_original)(void);
void C2_HOOK_FASTCALL DoNetGameManagement(void) {

#if defined(C2_HOOKS_ENABLED)
    DoNetGameManagement_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0049ab00, DoNetGameManagement, DoNetGameManagement_original)

void (C2_HOOK_FASTCALL * SetUpNetCarPositions_original)(void);
void C2_HOOK_FASTCALL SetUpNetCarPositions(void) {

#if defined(C2_HOOKS_ENABLED)
    SetUpNetCarPositions_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00499000, SetUpNetCarPositions, SetUpNetCarPositions_original)

void (C2_HOOK_FASTCALL * SignalToStartRace_original)(void);
void C2_HOOK_FASTCALL SignalToStartRace(void) {

#if defined(C2_HOOKS_ENABLED)
    SignalToStartRace_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00498e70, SignalToStartRace, SignalToStartRace_original)

void (C2_HOOK_FASTCALL * DoNetworkHeadups_original)(int pCredits);
void C2_HOOK_FASTCALL DoNetworkHeadups(int pCredits) {

#if defined(C2_HOOKS_ENABLED)
    DoNetworkHeadups_original(pCredits);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00499360, DoNetworkHeadups, DoNetworkHeadups_original)
