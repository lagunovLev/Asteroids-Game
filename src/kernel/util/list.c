#include "list.h"
#include "../debug.h"

void list_constructor(List* list)
{
    list->size = 0;
    list->begin = NULL;
    list->end = NULL;
}

void list_destructor(List* list, void(*func)(void* data))
{
    list_elem* cur_item = list->begin;
    for (uint32 i = 0; i < list->size; i++)
    {
        list_elem* next_item = cur_item->next;
        func(cur_item->data);
        free(cur_item);
        cur_item = next_item;
    }
}

List* list_new()
{
    List* list = malloc(sizeof(List));
    list_constructor(list);
    return list;
}

void list_delete(List* list, void(*func)(void* data))
{
    list_destructor(list, func);
    free(list);
}

void* list_get(List* list, int32 index)
{
    uint32 size = list->size;
    if (index < 0)
        index += size;
    if (index <= size / 2 - 1)
    {
        list_elem* item = list->begin;
        for (uint32 i = 0; i < index; i++)
            item = item->next;
        return item->data;
    }
    else 
    {
        list_elem* item = list->end;
        for (uint32 i = list->size - 1; i > index; i--)
            item = item->prev;
        return item->data;
    }
}

list_elem* list_get_elem(List* list, int32 index)
{
    uint32 size = list->size;
    if (index < 0)
        index += size;
    if (index <= size / 2 - 1)
    {
        list_elem* item = list->begin;
        for (uint32 i = 0; i < index; i++)
            item = item->next;
        return item;
    }
    else 
    {
        list_elem* item = list->end;
        for (uint32 i = list->size - 1; i > index; i--)
            item = item->prev;
        return item;
    }
}

void list_push_back(List* list, void* data)
{
    list_elem* item = malloc(sizeof(list_elem));
    item->data = data;
    if (list->size == 0)
    {
        list->size = 1;
        item->next = NULL;
        item->prev = NULL;
        list->begin = item;
        list->end = item;
        return;
    }
    list->size++;
    item->next = NULL;
    item->prev = list->end;
    list->end->next = item;
    list->end = item;
}

void list_push_front(List* list, void* data)
{
    list_elem* item = malloc(sizeof(list_elem));
    item->data = data;
    if (list->size == 0)
    {
        list->size = 1;
        item->next = NULL;
        item->prev = NULL;
        list->begin = item;
        list->end = item;
        return;
    }
    list->size++;
    item->next = list->begin;
    item->prev = NULL;
    list->begin->prev = item;
    list->begin = item;
}

void* list_pop_front(List* list)
{
    if (list->size == 0)
        return NULL;
    list_elem* item = list->begin;
    list_elem* next_item = item->next;
    next_item->prev = NULL;
    void* data = item->data;
    free(item);
    list->size--;
    list->begin = next_item;
    if (list->size == 0)
        list->end = NULL;
    return data;
}

void* list_pop_back(List* list)
{
    if (list->size == 0)
        return NULL;
    list_elem* item = list->end;
    list_elem* prev_item = item->prev;
    prev_item->next = NULL;
    void* data = item->data;
    free(item);
    list->size--;
    list->end = prev_item;
    if (list->size == 0)
        list->begin = NULL;
    return data;
}

void list_insert(List* list, void* data, int32 index)
{
    if (index < 0)
        index += list->size + 1;
    if (list->size == 0 || index >= list->size)
        list_push_back(list, data);
    else if (index == 0)
        list_push_front(list, data);
    else 
    {
        list_elem* item_prev = list_get_elem(list, index - 1);
        list_elem* item_next = item_prev->next;
        list_elem* item = malloc(sizeof(list_elem));
        item->data = data;
        item->prev = item_prev;
        item->next = item_next;
        item_prev->next = item;
        item_next->prev = item;
        list->size++;
    }
}

void* list_remove(List* list, int32 index)
{
    if (index < 0)
        index += list->size;
    if (list->size == 0)
        return NULL;
    if (index >= list->size)
        return list_pop_back(list);
    if (index == 0)
        return list_pop_front(list);

    list_elem* item = list_get_elem(list, index);
    list_elem* item_prev = item->prev;
    list_elem* item_next = item->next;
    void* data = item->data;
    free(item);
    item_prev->next = item_next;
    item_next->prev = item_prev;
    list->size--;
    return data;
}

void* list_remove_by_elem(List* list, list_elem* elem)
{
    if (list->size == 0 || elem == NULL)
        return NULL;
    if (elem == list->begin)
        return list_pop_front(list);
    if (elem == list->end)
        return list_pop_back(list);
    
    list_elem* item_prev = elem->prev;
    list_elem* item_next = elem->next;
    void* data = elem->data;
    free(elem);
    item_prev->next = item_next;
    item_next->prev = item_prev;
    list->size--;
    return data;
}