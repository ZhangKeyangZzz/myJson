#include <stdlib.h>
#include <string.h>
#include "json_value.h"
#include "unicode.h"


#define DEFAULT_CAPACITY    16
#define DEFAULT_LOADFACTOR  0.75


static inline JsonObjectPair* newObjectBucket(int capacity)
{
    JsonObjectPair* bucket = (JsonObjectPair*)malloc(capacity * sizeof(JsonObjectPair));
    if (bucket == NULL)
        return NULL;
    for (int i = 0; i < capacity; i++)
    {
        bucket[i].key = NULL;
        bucket[i].value = NULL;
        bucket[i].next = &bucket[i];
    }
    return bucket;
}


static inline void jsonObjectGrow(JsonObject* object)
{
    int newCap = 2 * object->cap;
    JsonObjectPair* newBucket = newObjectBucket(newCap);
    if (newBucket == NULL)
        return;
    for (int index = 0; index < object->cap; index++)
    {
        JsonObjectPair* pair = object->bucket[index].next;
        while (pair != &object->bucket[index])
        {
            object->bucket[index].next = pair->next;
            if ((pair->hash & object->cap) != 0)
            {
                pair->next = newBucket[2 * index].next;
                newBucket[2 * index].next = pair;
            }
            else
            {
                pair->next = newBucket[index].next;
                newBucket[index].next = pair;
            }
            pair = object->bucket[index].next;
        }
    }
    free(object->bucket);
    object->cap = newCap;
    object->bucket = newBucket;
}


static inline JsonObjectPair* newObjectPair(const char* key, JsonValue* value, int hash)
{
    JsonObjectPair* pair = (JsonObjectPair*)malloc(sizeof(JsonObjectPair));
    if (pair == NULL)
        return pair;
    int keylen = strlen(key);
    pair->key = (char*)malloc((keylen + 1) * sizeof(char));
    if (pair->key == NULL)
        return free(pair), NULL;
    memcpy(pair->key, key, keylen);
    pair->key[keylen] = 0;
    pair->hash = hash;
    pair->value = value;
    pair->next = pair;
    return pair;
}


JsonObject* jsonNewObject(void)
{
    JsonObject* object = (JsonObject*)malloc(sizeof(JsonObject));
    if (object == NULL)
        return NULL;
    object->bucket = newObjectBucket(DEFAULT_CAPACITY);
    if (object->bucket == NULL)
        return free(object), NULL;
    object->len = 0;
    object->cap = DEFAULT_CAPACITY;
    object->size = 0;
    return object;
}


void jsonDeleteObject(JsonObject* object)
{
    for (int i = 0; i < object->cap; i++)
    {
        JsonObjectPair* pair = object->bucket[i].next;
        while (pair != &object->bucket[i])
        {
            JsonObjectPair* record = pair;
            pair = pair->next;
            jsonDelete(record->value);
            free(record->key);
            free(record);
        }
    }
    free(object->bucket);
    free(object);
}


int jsonObjectPut(JsonObject* object, const char* key, JsonValue* value)
{
    int hash = unicodeHash(key);
    int index = hash & (object->cap - 1);
    JsonObjectPair* target = object->bucket[index].next;
    while (target != &object->bucket[index])
    {
        if (target->hash == hash && unicodeStrcmp(target->key, key))
        {
            jsonDelete(target->value);
            target->value = value;
            return 1;
        }
        target = target->next;
    }
    JsonObjectPair* pair = newObjectPair(key, value, hash);
    if (pair == NULL)
        return 0;
    pair->next = object->bucket[index].next;
    object->bucket[index].next = pair;
    object->size++;
    if (pair->next == &object->bucket[index])
    {
        object->len++;
        if (object->len >= (object->cap * DEFAULT_LOADFACTOR))
            jsonObjectGrow(object);
    }
    return 1;
}


int jsonObjectGet(JsonObject* object, const char* key, JsonValue** value)
{
    int finish = 0;
    int hash = unicodeHash(key);
    int index = hash & (object->cap - 1);
    JsonValue* target = NULL;
    JsonObjectPair* pair = object->bucket[index].next;
    while (pair != &object->bucket[index])
    {
        if (hash == pair->hash && unicodeStrcmp(key, pair->key) == 0)
        {
            finish = 1;
            target = pair->value;
            break;
        }
        pair = pair->next;
    }
    *value = target;
    return finish;
}


int jsonObjectRemove(JsonObject* object, const char* key)
{
    int hash = unicodeHash(key);
    int index = hash & (object->cap - 1);
    JsonObjectPair* prev = &object->bucket[index];
    JsonObjectPair* pair = object->bucket[index].next;
    while (pair != &object->bucket[index])
    {
        if (hash == pair->hash && unicodeStrcmp(key, pair->key) == 0)
        {
            prev->next = pair->next;
            jsonDelete(pair->value);
            free(pair->key);
            free(pair);
            return 1;
        }
        prev = pair;
        pair = pair->next;
    }
    return 0;
}


