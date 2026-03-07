#include "brlists.h"

#include "c2_stdio.h"

#include <assert.h>

// FUNCTION: CARMA2_HW 0x00529960
void C2_HOOK_CDECL BrNewList(br_list* list) {

    list->head = (br_node*)&list->_null;
    list->_null = NULL;
    list->tail = (br_node*)list;
}

// FUNCTION: CARMA2_HW 0x00529980
void C2_HOOK_CDECL BrAddHead(br_list* list, br_node* node) {

    assert(node != NULL);
    assert(list != NULL);
    assert(list->head != NULL);

    //node->prev = (br_node*)list;
    node->prev = (br_node*)&(list->head);
    node->next = list->head;
    list->head->prev = node;
    list->head = node;
}

// FUNCTION: CARMA2_HW 0x005299a0
void C2_HOOK_CDECL BrAddTail(br_list* list, br_node* node) {

    node->next = (br_node*)&list->_null;
    node->prev = list->tail;
    list->tail->next = node;
    list->tail = node;
}

// FUNCTION: CARMA2_HW 0x005299c0
br_node* C2_HOOK_CDECL BrRemHead(br_list* list) {
    br_node* n;

    n = list->head;
    if (n == (br_node*)&list->_null) {
        return NULL;
    }
    list->head = n->next;
    n->next->prev = (br_node*)&list->head;
    return n;
}

// FUNCTION: CARMA2_HW 0x005299e0
br_node* C2_HOOK_CDECL BrRemTail(br_list* list) {
    br_node* n;

    n = list->tail;
    if (n == (br_node*)&list->head) {
        return NULL;
    }
    list->tail = n->prev;
    n->prev->next = (br_node*)&list->_null;
    return n;
}

// FUNCTION: CARMA2_HW 0x00529a00
void C2_HOOK_CDECL BrInsert(br_list* list, br_node* here, br_node* node) {

    node->prev = here;
    node->next = here->next;
    here->next->prev = node;
    here->next = node;
}

// FUNCTION: CARMA2_HW 0x00529a20
br_node* C2_HOOK_CDECL BrRemove(br_node* node) {

    node->next->prev = node->prev;
    node->prev->next = node->next;
    return node;
}

// FUNCTION: CARMA2_HW 0x00529a40
void C2_HOOK_CDECL BrSimpleNewList(br_simple_list* list) {

    list->head = NULL;
}

// FUNCTION: CARMA2_HW 0x00529a50
void C2_HOOK_CDECL BrSimpleAddHead(br_simple_list* list, br_simple_node* node) {

    node->next = list->head;
    node->prev = (br_simple_node**)&list->head;
    if (list->head != NULL) {
        list->head->prev = &node->next;
    }
    list->head = node;
}

// FUNCTION: CARMA2_HW 0x00529a70
br_simple_node* C2_HOOK_CDECL BrSimpleRemHead(br_simple_list* list) {
    br_simple_node* node;

    node = list->head;
    if (node != NULL) {
        BrSimpleRemove(node);
    }
    return node;
}

// FUNCTION: CARMA2_HW 0x00529aa0
void C2_HOOK_CDECL BrSimpleInsert(br_simple_list* list, br_simple_node* here, br_simple_node* node) {

    node->prev = &here->next;
    node->next = here->next;
    if (here->next != NULL) {
        here->next->prev = &node->next;
    }
    here->next = node;
}

// FUNCTION: CARMA2_HW 0x00529ac0
br_simple_node* C2_HOOK_CDECL BrSimpleRemove(br_simple_node* node) {

    *node->prev = node->next;
    if (node->next != NULL) {
        node->next->prev = node->prev;
    }
    node->next = NULL;
    node->prev = NULL;
    return node;
}