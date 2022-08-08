#include "json_value.h"
#include <stdlib.h>


static inline JsonArrayItem* locateArrayItem(JsonArray* array, int index)
{
    JsonArrayItem* iter = array->head;
    for (int i = 0; i < index; i++)
        iter = iter->next;
    return iter;
}


static inline JsonArrayItem* newArrayItem(JsonValue* value)
{
    JsonArrayItem* item = (JsonArrayItem*)malloc(sizeof(JsonArrayItem));
    if (item == NULL)
        return NULL;
    item->next = item;
    item->prev = item;
    item->value = value;
    return item;
}


static inline void linkArrayItem(JsonArrayItem* target, JsonArrayItem* node)
{
    JsonArrayItem* successor = target->next;
    node->next = successor;
    successor->prev = node;
    target->next = node;
    node->prev = target;
}


static inline void unlinkArrayItem(JsonArrayItem* node)
{
    JsonArrayItem* precursor = node->prev;
    JsonArrayItem* successor = node->next;
    precursor->next = successor;
    successor->prev = precursor;
    node->prev = node;
    node->next = node;
}


JsonArray* jsonNewArray(void)
{
    JsonArray* array = (JsonArray*)malloc(sizeof(JsonArray));
    if (array == NULL)
        return NULL;
    array->len = 0;
    array->head = newArrayItem(NULL);
    if (array->head == NULL)
        return free(array), NULL;
    return array;
}


void jsonDeleteArray(JsonArray* array)
{
    JsonArrayItem* iter = array->head->next;
    while (iter != array->head)
    {
        jsonDelete(iter->value);
        JsonArrayItem* record = iter;
        iter = iter->next;
        free(record);
    }
    free(array->head);
    free(array);
}


int jsonArraySize(JsonArray* array)
{
    return array->len;
}


int jsonArrayAppend(JsonArray* array, JsonValue* value)
{
    JsonArrayItem* item = newArrayItem(value);
    if (item == NULL)
        return 0;
    linkArrayItem(array->head->prev, item);
    array->len++;
    return 1;
}


int jsonArrayPrepend(JsonArray* array, JsonValue* value)
{
    JsonArrayItem* item = newArrayItem(value);
    if (item == NULL)
        return 0;
    linkArrayItem(array->head, item);
    array->len++;
    return 1;
}


int jsonArrayInsert(JsonArray* array, int index, JsonValue* value)
{
    if (index < 0 || index > array->len)
        return 0;
    JsonArrayItem* item = newArrayItem(value);
    if (item == NULL)
        return 0;
    JsonArrayItem* target = locateArrayItem(array, index);
    linkArrayItem(target, item);
    array->len++;
    return 1;
}


int jsonArrayPop(JsonArray* array, JsonValue** value)
{
    if (array->len == 0)
        return 0;
    JsonArrayItem* item = array->head->prev;
    unlinkArrayItem(item);
    if (value != NULL)
        *value = item->value;
    free(item);
    array->len--;
    return 1;
}


int jsonArrayPoll(JsonArray* array, JsonValue** value)
{
    if (array->len == 0)
        return 0;
    JsonArrayItem* item = array->head->next;
    unlinkArrayItem(item);
    if (value != NULL)
        *value = item->value;
    free(item);
    array->len--;
    return 1;
}


int jsonArrayRemove(JsonArray* array, int index, JsonValue** value)
{
    JsonArrayItem* item = locateArrayItem(array, index);
    item = item->next;
    unlinkArrayItem(item);
    if (value != NULL)
        *value = item->value;
    free(item);
    array->len--;
    return 1;
}


int jsonArrayGet(JsonArray* array, int index, JsonValue** value)
{
    if (index < 0 || index >= array->len)
        return 0;
    JsonArrayItem* item = locateArrayItem(array, index);
    item = item->next;
    if (value != NULL)
        *value = item->value;
    return 1;
}


int jsonArraySet(JsonArray* array, int index, JsonValue* value)
{
    if (index < 0 || index >= array->len)
        return 0;
    JsonArrayItem* item = locateArrayItem(array, index);
    item = item->next;
    jsonDelete(item->value);
    item->value = value;
    return 1;
}
