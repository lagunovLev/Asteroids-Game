#pragma once 
#include "../common.h"

typedef struct {
    void* data;
    struct list_elem* prev;
    struct list_elem* next;
} list_elem;

typedef struct {
    list_elem* begin;
    list_elem* end;
    uint32 size;
} List;

List* list_new();
void list_delete(List* list, void(*func)(void* data));
void list_constructor(List* list);
void list_destructor(List* list, void(*func)(void* data));
void* list_get(List* list, int32 index);
list_elem* list_get_elem(List* list, int32 index);
void list_push_back(List* list, void* data);
void list_push_front(List* list, void* data);
void* list_pop_front(List* list);
void* list_pop_back(List* list);
void list_insert(List* list, void* data, int32 index);
void* list_remove(List* list, int32 index);
void* list_remove_by_elem(List* list, list_elem* elem); 

//typedef struct {
//    int32 i;
//    list_elem* elem;
//} List_iterator;
//
//List_iterator list_get_iter(List* list, int32 index);
//static inline uint8 iter_cond(List_iterator iter) {
//    return iter.elem != NULL;
//}
//static inline void iter_next(List_iterator* iter) {
//    iter->elem = iter->elem->next;
//    iter->i++;
//}
//static inline void* iter_get_data(List_iterator iter) {
//    return iter.elem->data;
//}
//void* iter_remove(List* list, List_iterator* iter);