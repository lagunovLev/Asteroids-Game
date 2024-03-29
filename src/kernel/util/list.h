#pragma once 
#include "../common.h"

typedef struct list_elem {
    void* data;
    struct list_elem* prev;
    struct list_elem* next;
} list_elem;

typedef struct List {
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
