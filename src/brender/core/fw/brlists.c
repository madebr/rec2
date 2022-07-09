#include "brlists.h"

#include "c2_stdio.h"

#include <assert.h>

void BrNewList(br_list* list) {

    list->head = (br_node*)&list->_null;
    list->_null = NULL;
    list->tail = (br_node*)list;
}
C2_HOOK_FUNCTION(0x00529960, BrNewList)

void BrAddHead(br_list* list, br_node* node) {

    assert(node != NULL);
    assert(list != NULL);
    assert(list->head != NULL);

    //node->prev = (br_node*)list;
    node->prev = (br_node*)&(list->head);
    node->next = list->head;
    list->head->prev = node;
    list->head = node;
}
C2_HOOK_FUNCTION(0x00529980, BrAddHead)

void BrAddTail(br_list* list, br_node* node) {

    node->next = (br_node*)&list->_null;
    node->prev = list->tail;
    list->tail->next = node;
    list->tail = node;
}
C2_HOOK_FUNCTION(0x005299a0, BrAddTail)

br_node* BrRemHead(br_list* list) {
    br_node* n;

    n = list->head;
    if (n == (br_node*)&list->_null) {
        return NULL;
    }
    list->head = n->next;
    n->next->prev = (br_node*)&list->head;
    return n;
}
C2_HOOK_FUNCTION(0x005299c0, BrRemHead)

br_node* BrRemTail(br_list* list) {
    br_node* n;

    n = list->tail;
    if (n == (br_node*)&list->head) {
        return NULL;
    }
    list->tail = n->prev;
    n->prev->next = (br_node*)&list->_null;
    return n;
}
C2_HOOK_FUNCTION(0x005299e0, BrRemTail)

void BrInsert(br_list* list, br_node* here, br_node* node) {

    node->prev = here;
    node->next = here->next;
    here->next->prev = node;
    here->next = node;
}
C2_HOOK_FUNCTION(0x00529a00, BrInsert)

br_node* BrRemove(br_node* node) {

    node->next->prev = node->prev;
    node->prev->next = node->next;
    return node;
}
C2_HOOK_FUNCTION(0x00529a20, BrRemove)

void BrSimpleNewList(br_simple_list* list) {

    list->head = NULL;
}
C2_HOOK_FUNCTION(0x00529a40, BrSimpleNewList)

void BrSimpleAddHead(br_simple_list* list, br_simple_node* node) {

    node->next = list->head;
    node->prev = (br_simple_node**)&list->head;
    if (list->head != NULL) {
        list->head->prev = &node->next;
    }
    list->head = node;
}
C2_HOOK_FUNCTION(0x00529a50, BrSimpleAddHead)

br_simple_node* BrSimpleRemHead(br_simple_list* list) {
    br_simple_node* node;

    node = list->head;
    if (node != NULL) {
        BrSimpleRemove(node);
    }
    return node;
}
C2_HOOK_FUNCTION(0x00529a70, BrSimpleRemHead)

void BrSimpleInsert(br_simple_list* list, br_simple_node* here, br_simple_node* node) {

    node->prev = &here->next;
    node->next = here->next;
    if (here->next != NULL) {
        here->next->prev = &node->next;
    }
    here->next = node;
}
C2_HOOK_FUNCTION(0x00529aa0, BrSimpleInsert)

br_simple_node* BrSimpleRemove(br_simple_node* node) {

    *node->prev = node->next;
    if (node->next != NULL) {
        node->next->prev = node->prev;
    }
    node->next = NULL;
    node->prev = NULL;
    return node;
}
C2_HOOK_FUNCTION(0x00529ac0, BrSimpleRemove)
