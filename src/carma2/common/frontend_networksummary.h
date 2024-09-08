#ifndef FRONTEND_NETWORKSUMMARY_H
#define FRONTEND_NETWORKSUMMARY_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(tFrontend_spec, gFrontend_NETWORK_SUMMARY);
C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gPlayer_lookup_netsummary, 14);


void C2_HOOK_FASTCALL NetworkSummarySetup(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NetSummary_Infunc(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL DrawThisCarIconNow(int pCar_index, int pX, int pY);

void C2_HOOK_FASTCALL NetSummary_Draw(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NetSummary_MenuHandler(tFrontend_spec* pFrontend);

#endif /* FRONTEND_NETWORKSUMMARY_H */
