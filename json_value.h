#ifndef __JSON__VALUE__H__
#define __JSON__VALUE__H__

struct JsonValue;

typedef struct JsonString
{
    int bytes;
    int len;
    char* buf;
} JsonString;

typedef struct JsonArrayItem
{
    struct JsonValue* value;
    struct JsonArrayItem* prev;
    struct JsonArrayItem* next;
} JsonArrayItem;

typedef struct JsonArray
{
    int len;
    struct JsonArrayItem* head;
} JsonArray;

typedef struct JsonObjectPair
{
    int hash;
    char* key;
    struct JsonValue* value;
    struct JsonObjectPair* next;
} JsonObjectPair;

typedef struct JsonObject
{
    int len;
    int cap;
    int size;
    JsonObjectPair* bucket;
} JsonObject;

typedef struct JsonValue
{
    char type;
    union {
        double number;
        struct JsonString* string;
        struct JsonArray* array;
        struct JsonObject* object;
    } value;
} JsonValue;

typedef struct JsonByteBuf
{
    int len;
    int cap;
    char* bytes;
} JsonByteBuf;

#ifdef __cplusplus
extern "C" {
#endif
    JsonValue* newJsonValue(char type);
    JsonString* jsonNewString(const char* str, int len);
    JsonString* jsonWrapString(JsonByteBuf* bytebuf);
    void jsonDeleteString(JsonString* string);
    JsonArray* jsonNewArray(void);
    void jsonDeleteArray(JsonArray* array);
    int jsonArraySize(JsonArray* array);
    int jsonArrayAppend(JsonArray* array, JsonValue* value);
    int jsonArrayPrepend(JsonArray* array, JsonValue* value);
    int jsonArrayInsert(JsonArray* array, int index, JsonValue* value);
    int jsonArrayPop(JsonArray* array, JsonValue** value);
    int jsonArrayPoll(JsonArray* array, JsonValue** value);
    int jsonArrayRemove(JsonArray* array, int index, JsonValue** value);
    int jsonArrayGet(JsonArray* array, int index, JsonValue** value);
    int jsonArraySet(JsonArray* array, int index, JsonValue* value);
    JsonObject* jsonNewObject(void);
    void jsonDeleteObject(JsonObject* object);
    int jsonObjectPut(JsonObject* object, const char* key, JsonValue* value);
    int jsonObjectGet(JsonObject* object, const char* key, JsonValue** value);
    int jsonObjectRemove(JsonObject* object, const char* key);
    void jsonDelete(JsonValue* value);
    int jsonInitByteBuf(JsonByteBuf* bytebuf, int capacity);
    void jsonUninitByteBuf(JsonByteBuf* bytebuf);
    int jsonByteBufWrite(JsonByteBuf* bytebuf, char ch);
    int jsonByteBufWrites(JsonByteBuf* bytebuf, const char* buf, int len);
    int jsonWriteNull(JsonByteBuf* bytebuf);
    int jsonWriteTrue(JsonByteBuf* bytebuf);
    int jsonWriteFalse(JsonByteBuf* bytebuf);
    int jsonWriteNumber(JsonByteBuf* bytebuf, double number);
    int jsonWriteString(JsonByteBuf* bytebuf, JsonString* string);
    int jsonWriteArray(JsonByteBuf* bytebuf, JsonArray* array);
    int jsonWriteObject(JsonByteBuf* bytebuf, JsonObject* object);
    int jsonWrite(JsonByteBuf* bytebuf, JsonValue* json);
    JsonValue* jsonParse(char** str, int *len);
#ifdef __cplusplus
}
#endif
#endif
