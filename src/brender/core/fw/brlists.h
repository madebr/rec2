#ifndef REC2_BRLISTS_H
#define REC2_BRLISTS_H

#include "c2_hooks.h"

#include "brender/br_types.h"

void C2_HOOK_CDECL BrNewList(br_list* list);

void C2_HOOK_CDECL BrAddHead(br_list* list, br_node* node);

void C2_HOOK_CDECL BrAddTail(br_list* list, br_node* node);

br_node* C2_HOOK_CDECL BrRemHead(br_list* list);

br_node* C2_HOOK_CDECL BrRemTail(br_list* list);

void C2_HOOK_CDECL BrInsert(br_list* list, br_node* here, br_node* node);

br_node* C2_HOOK_CDECL BrRemove(br_node* node);

void C2_HOOK_CDECL BrSimpleNewList(br_simple_list* list);

void C2_HOOK_CDECL BrSimpleAddHead(br_simple_list* list, br_simple_node* node);

br_simple_node* C2_HOOK_CDECL BrSimpleRemHead(br_simple_list* list);

void C2_HOOK_CDECL BrSimpleInsert(br_simple_list* list, br_simple_node* here, br_simple_node* node);

br_simple_node* C2_HOOK_CDECL BrSimpleRemove(br_simple_node* node);

#endif // REC2_BRLISTS_H
