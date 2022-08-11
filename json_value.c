#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json.h"
#include "json_value.h"


#define FLOAT_EPS   1e-11


JsonValue jsonNull = {
    .type = JSON_NULL,
    .value = { 0 },
};


JsonValue jsonTrue = {
    .type = JSON_TRUE,
    .value = { 0 },
};


JsonValue jsonFalse = {
    .type = JSON_FALSE,
    .value = { 0 },
};


JsonValue* newJsonValue(char type)
{
    JsonValue* jsonValue = (JsonValue*)malloc(sizeof(JsonValue));
    if (jsonValue != NULL)
        jsonValue->type = type;
    return jsonValue;
}


void jsonDelete(JsonValue* value)
{
    switch (value->type)
    {
    case JSON_NULL:
        break;
    case JSON_TRUE:
        break;
    case JSON_FALSE:
        break;
    case JSON_NUMBER:
        free(value);
        break;
    case JSON_STRING:
        jsonDeleteString(value->value.string);
        free(value);
        break;
    case JSON_ARRAY:
        jsonDeleteArray(value->value.array);
        free(value);
        break;
    case JSON_OBJECT:
        jsonDeleteObject(value->value.object);
        free(value);
        break;
    }
}


int jsonWriteNull(JsonByteBuf* bytebuf)
{
    return jsonByteBufWrites(bytebuf, "null", 4);
}


int jsonWriteTrue(JsonByteBuf* bytebuf)
{
    return jsonByteBufWrites(bytebuf, "true", 4);
}


int jsonWriteFalse(JsonByteBuf* bytebuf)
{
    return jsonByteBufWrites(bytebuf, "false", 5);
}


int jsonWriteNumber(JsonByteBuf* bytebuf, double number)
{
    int n = 0;
    int ans = 0;
    int high = (int)number;
    double low = number - high;
    char ch[64] = { 0 };
    if (number < 0)
    {
        jsonByteBufWrite(bytebuf, '-');
        number = -number;
    }
    while (high > 0)
    {
        ch[n++] = high % 10 + '0';
        high = high / 10;
    }
    for (int i = n - 1; i >= 0; i--)
        ans += jsonByteBufWrite(bytebuf, ch[i]);
    number = number - (int)number;
    if (number > 0)
    {
        double step = 0.1;
        ans += jsonByteBufWrite(bytebuf, '.');
        while (number > FLOAT_EPS)
        {
            number -= step * (int)(low * 10);
            step /= 10;
            ans += jsonByteBufWrite(bytebuf, (int)(low * 10));
            low = low * 10.0 - (int)(low * 10);
        }
    }
    return ans;
}


int jsonWriteString(JsonByteBuf* bytebuf, JsonString* string)
{
    int ans = 0;
    ans += jsonByteBufWrite(bytebuf, '"');
    ans += jsonByteBufWrites(bytebuf, string->buf, string->bytes - 1);
    ans += jsonByteBufWrite(bytebuf, '"');
    return ans;
}


int jsonWriteArray(JsonByteBuf* bytebuf, JsonArray* array)
{
    int ans = 0;
    JsonArrayItem* iter = array->head->next;
    ans += jsonByteBufWrite(bytebuf, '[');
    while (iter != array->head)
    {
        ans += jsonWrite(bytebuf, iter->value);
        if (iter->next != array->head)
            ans += jsonByteBufWrite(bytebuf, ',');
        iter = iter->next;
    }
    ans += jsonByteBufWrite(bytebuf, ']');
    return ans;
}


int jsonWriteObject(JsonByteBuf* bytebuf, JsonObject* object)
{
    int ans = 0;
    int size = 0;
    ans += jsonByteBufWrite(bytebuf, '{');
    for (int i = 0; i < object->cap; i++)
    {
        JsonObjectPair* pair = object->bucket[i].next;
        while (pair != &object->bucket[i])
        {
            size++;
            int keylen = strlen(pair->key);
            ans += jsonByteBufWrite(bytebuf, '"');
            ans += jsonByteBufWrites(bytebuf, pair->key, keylen);
            ans += jsonByteBufWrite(bytebuf, '"');
            ans += jsonByteBufWrite(bytebuf, ':');
            ans += jsonWrite(bytebuf, pair->value);
            if (size < object->size)
                ans += jsonByteBufWrite(bytebuf, ',');
            pair = pair->next;
        }
    }
    ans += jsonByteBufWrite(bytebuf, '}');
    return ans;
}


int jsonWrite(JsonByteBuf* bytebuf, JsonValue* json)
{
    int ans = 0;
    switch (json->type)
    {
    case JSON_NULL:
        ans += jsonWriteNull(bytebuf);
        break;
    case JSON_TRUE:
        ans += jsonWriteTrue(bytebuf);
        break;
    case JSON_FALSE:
        ans += jsonWriteFalse(bytebuf);
        break;
    case JSON_NUMBER:
        ans += jsonWriteNumber(bytebuf, json->value.number);
        break;
    case JSON_STRING:
        ans += jsonWriteString(bytebuf, json->value.string);
        break;
    case JSON_ARRAY:
        ans += jsonWriteArray(bytebuf, json->value.array);
        break;
    case JSON_OBJECT:
        ans += jsonWriteObject(bytebuf, json->value.object);
        break;
    default:
        break;
    }
    return ans;
}
