#include <stdlib.h>
#include <string.h>
#include "json.h"
#include "json_value.h"
#include "unicode.h"


extern JsonValue jsonNull;
extern JsonValue jsonTrue;
extern JsonValue jsonFalse;


int Json_Type(JSON* json)
{
    JsonValue* jsonValue = (JsonValue*)json;
    return jsonValue->type;
}


JSON* Json_Null(void)
{
    return &jsonNull;
}


JSON* Json_True(void)
{
    return &jsonTrue;
}


JSON* Json_False(void)
{
    return &jsonFalse;
}


JSON* Json_Number(double number)
{
    JsonValue* jsonValue = newJsonValue(JSON_NUMBER);
    if (jsonValue != NULL)
        jsonValue->value.number = number;
    return jsonValue;
}


double Json_ToNumber(JSON* json)
{
    if (json == NULL)
        return 0;
    JsonValue* jsonValue = (JsonValue*)json;
    if (jsonValue->type != JSON_NUMBER)
        return 0;
    return jsonValue->value.number;
}


JSON* Json_String(const char* bytes, int len)
{
    if (bytes == NULL)
        return NULL;
    if (len < 0)
        len = strlen(bytes);
    JsonValue* jsonValue = newJsonValue(JSON_STRING);
    if (jsonValue != NULL)
        jsonValue->value.string = jsonNewString(bytes, len);
    return jsonValue;
}


int Json_Strlen(JSON* json)
{
    if (json == NULL)
        return 0;
    JsonValue* jsonValue = (JsonValue*)json;
    if (jsonValue->type != JSON_STRING)
        return 0;
    return jsonValue->value.string->len;
}


int Json_ToString(JSON* json, char* buf, int len)
{
    if (json == NULL || buf == NULL || len <= 0)
        return 0;
    JsonValue* jsonValue = (JsonValue*)json;
    if (jsonValue->type != JSON_STRING)
        return 0;
    JsonString* string = jsonValue->value.string;
    int total = 0;
    char* src = string->buf;
    char* dst = buf;
    int bytes = string->bytes < len ? string->bytes - 1 : len - 1;
    while (bytes > 0)
    {
        int rune = 0;
        int seqLen = unicodeSequenceLen(*src);
        if (!unicodeDecode(src, bytes, &rune))
            break;
        unicodeEncode(rune, dst, bytes);
        src += seqLen;
        dst += seqLen;
        bytes -= seqLen;
        total += seqLen;
    }
    *dst = 0;
    return 1 + total;
}


JSON* Json_Array(void)
{
    JsonValue* jsonValue = newJsonValue(JSON_ARRAY);
    if (jsonValue != NULL)
        jsonValue->value.array = jsonNewArray();
    return jsonValue;
}


int Json_ArraySize(JSON* array)
{
    if (array == NULL)
        return 0;
    JsonValue* arrayValue = (JsonValue*)array;
    if (arrayValue->type != JSON_ARRAY)
        return 0;
    JsonArray* jsonArray = arrayValue->value.array;
    return jsonArray->len;
}


int Json_ArrayAppend(JSON* array, JSON* value)
{
    if (array == NULL || value == NULL)
        return 0;
    JsonValue* arrayValue = (JsonValue*)array;
    JsonValue* newValue = (JsonValue*)value;
    if (arrayValue->type != JSON_ARRAY)
        return 0;
    return jsonArrayAppend(arrayValue->value.array, newValue);
}


int Json_ArrayPrepend(JSON* array, JSON* value)
{
    if (array == NULL || value == NULL)
        return 0;
    JsonValue* arrayValue = (JsonValue*)array;
    JsonValue* newValue = (JsonValue*)value;
    if (arrayValue->type != JSON_ARRAY)
        return 0;
    return jsonArrayPrepend(arrayValue->value.array, newValue);
}


int Json_ArrayInsert(JSON* array, int index, JSON* value)
{
    if (array == NULL || value == NULL)
        return 0;
    JsonValue* arrayValue = (JsonValue*)array;
    JsonValue* newValue = (JsonValue*)value;
    if (arrayValue->type != JSON_ARRAY)
        return 0;
    if (index < 0 || index > arrayValue->value.array->len)
        return 0;
    return jsonArrayInsert(arrayValue->value.array, index, newValue);
}


int Json_ArrayPop(JSON* array, JSON** value)
{
    if (array == NULL || value == NULL)
        return 0;
    JsonValue* arrayValue = (JsonValue*)array;
    JsonValue** newValue = (JsonValue**)value;
    if (arrayValue->type != JSON_ARRAY)
        return 0;
    if (arrayValue->value.array->len == 0)
        return 0;
    return jsonArrayPop(arrayValue->value.array, newValue);
}


int Json_ArrayPoll(JSON* array, JSON** value)
{
    if (array == NULL || value == NULL)
        return 0;
    JsonValue* arrayValue = (JsonValue*)array;
    JsonValue** newValue = (JsonValue**)value;
    if (arrayValue->type != JSON_ARRAY)
        return 0;
    if (arrayValue->value.array->len == 0)
        return 0;
    return jsonArrayPoll(arrayValue->value.array, newValue);
}


int Json_ArrayRemove(JSON* array, int index, JSON** value)
{
    if (array == NULL || value == NULL)
        return 0;
    JsonValue* arrayValue = (JsonValue*)array;
    JsonValue** newValue = (JsonValue**)value;
    if (arrayValue->type != JSON_ARRAY)
        return 0;
    if (index < 0 || index >= arrayValue->value.array->len)
        return 0;
    return jsonArrayRemove(arrayValue->value.array, index, newValue);
}


int Json_ArrayGet(JSON* array, int index, JSON** value)
{
    if (array == NULL || value == NULL)
        return 0;
    JsonValue* arrayValue = (JsonValue*)array;
    JsonValue** newValue = (JsonValue**)value;
    if (arrayValue->type != JSON_ARRAY)
        return 0;
    if (index < 0 || index >= arrayValue->value.array->len)
        return 0;
    return jsonArrayGet(arrayValue->value.array, index, newValue);
}


int Json_ArraySet(JSON* array, int index, JSON* value)
{
    if (array == NULL || value == NULL)
        return 0;
    JsonValue* arrayValue = (JsonValue*)array;
    JsonValue* newValue = (JsonValue*)value;
    if (arrayValue->type != JSON_ARRAY)
        return 0;
    if (index < 0 || index >= arrayValue->value.array->len)
        return 0;
    return jsonArraySet(arrayValue->value.array, index, newValue);
}


JSON* Json_Object(void)
{
    JsonValue* jsonValue = newJsonValue(JSON_OBJECT);
    if (jsonValue != NULL)
        jsonValue->value.object = jsonNewObject();
    return jsonValue;
}


int Json_ObjectPut(JSON* object, const char* key, JSON* value)
{
    if (object == NULL || key == NULL || value == NULL)
        return 0;
    JsonValue* objectValue = (JsonValue*)object;
    if (objectValue->type != JSON_OBJECT)
        return 0;
    JsonValue* newValue = (JsonValue*)value;
    return jsonObjectPut(objectValue->value.object, key, newValue);
}


int Json_ObjectGet(JSON* object, const char* key, JSON** value)
{
    if (object == NULL || key == NULL || value == NULL)
        return 0;
    JsonValue* objectValue = (JsonValue*)object;
    if (objectValue->type != JSON_OBJECT)
        return 0;
    JsonValue** newValue = (JsonValue**)value;
    return jsonObjectGet(objectValue->value.object, key, newValue);
}


int Json_ObjectRemove(JSON* object, const char* key)
{
    if (object == NULL || key == NULL)
        return 0;
    JsonValue* objectValue = (JsonValue*)object;
    if (objectValue->type != JSON_OBJECT)
        return 0;
    return jsonObjectRemove(objectValue->value.object, key);
}


void Json_Delete(JSON* json)
{
    JsonValue* jsonValue = (JsonValue*)json;
    jsonDelete(jsonValue);
}


char* Json_Print(JSON* json, int* len)
{
    if (json == NULL)
        return NULL;
    JsonByteBuf bytebuf;
    if (0 == jsonInitByteBuf(&bytebuf, 0))
        return NULL;
    int bytes = 0;
    JsonValue* jsonValue = (JsonValue*)json;
    bytes += jsonWrite(&bytebuf, jsonValue);
    jsonByteBufWrite(&bytebuf, 0);
    if (len != NULL)
        *len = bytes;
    return bytebuf.bytes;
}


JSON* Json_Parse(const char* str, int len)
{
    int max = strlen(str);
    if (len < 0)
        len = max;
    len = len < max ? len : max;
    char* ptr = (char*)str;
    return (JSON*)jsonParse(&ptr, &len);
}

