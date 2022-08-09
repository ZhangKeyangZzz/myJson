#ifndef __JSON__H__
#define __JSON__H__

enum {
    JSON_NULL,
    JSON_TRUE,
    JSON_FALSE,
    JSON_NUMBER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT,
};

typedef void JSON;

#ifdef __cplusplus
extern "C" {
#endif


int Json_Type(JSON* json);
JSON* Json_Null(void);
JSON* Json_True(void);
JSON* Json_False(void);
JSON* Json_Number(double number);
double Json_ToNumber(JSON* json);
JSON* Json_String(const char* bytes, int len);
int Json_Strlen(JSON* json);
int Json_ToString(JSON* json, char* buf, int len);
JSON* Json_Array(void);
int Json_ArraySize(JSON* array);
int Json_ArrayAppend(JSON* array, JSON* value);
int Json_ArrayPrepend(JSON* array, JSON* value);
int Json_ArrayInsert(JSON* array, int index, JSON* value);
int Json_ArrayPop(JSON* array, JSON** value);
int Json_ArrayPoll(JSON* array, JSON** value);
int Json_ArrayRemove(JSON* array, int index, JSON** value);
int Json_ArrayGet(JSON* array, int index, JSON** value);
int Json_ArraySet(JSON* array, int index, JSON* value);
JSON* Json_Object(void);
int Json_ObjectSize(JSON* object);
int Json_ObjectContains(JSON* object, const char* key);
int Json_ObjectPut(JSON* object, const char* key, JSON* value);
int Json_ObjectGet(JSON* object, const char* key, JSON** value);
int Json_ObjectRemove(JSON* object, const char* key);
void Json_Delete(JSON* json);
char* Json_Print(JSON* json, int* len);
JSON* Json_Parse(const char* str, int len);

#ifdef __cplusplus
}
#endif
#endif
