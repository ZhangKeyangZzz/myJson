#include <stdlib.h>
#include <string.h>
#include "unicode.h"
#include "json.h"
#include "json_value.h"


extern JsonValue jsonNull;
extern JsonValue jsonTrue;
extern JsonValue jsonFalse;


static inline double quickMul(double x, long long N)
{
    double ans = 1.0;
    double x_contribute = x;
    while (N > 0) {
        if (N % 2 == 1) {
            ans *= x_contribute;
        }
        x_contribute *= x_contribute;
        N /= 2;
    }
    return ans;
}


static inline double qpow(double x, int n)
{
    long long N = n;
    return N >= 0 ? quickMul(x, N) : 1.0 / quickMul(x, -N);
}


static inline void backRune(char** str, int* max, int rune)
{
    int size = unicodeRuneSize(rune);
    *str -= size;
    *max += size;
}


static inline int peekRune(char** str, int* max)
{
    int rune = 0;
    char* ptr = *str;
    int len = *max;
    unicodeDecode(ptr, len, &rune);
    return rune;
}


static inline int takeRune(char** str, int* max)
{
    int rune = 0;
    char* ptr = *str;
    int len = *max;
    int seqLen = unicodeSequenceLen(*ptr);
    unicodeDecode(ptr, len, &rune);
    *str += seqLen;
    *max -= seqLen;
    return rune;
}


static inline void skipSpace(char** str, int* max)
{
    int rune = takeRune(str, max);
    while (rune == ' ' || rune == '\t' || rune == '\r' || rune == '\n')
        rune = takeRune(str, max);
    if (rune != 0)
        backRune(str, max, rune);
}


static inline int jsonParseNull(char** str, int *max)
{
    if (takeRune(str, max) != 'n')
        return 0;
    if (takeRune(str, max) != 'u')
        return 0;
    if (takeRune(str, max) != 'l')
        return 0;
    if (takeRune(str, max) != 'l')
        return 0;
    return 1;
}


static inline int jsonParseTrue(char** str, int *max)
{
    if (takeRune(str, max) != 't')
        return 0;
    if (takeRune(str, max) != 'r')
        return 0;
    if (takeRune(str, max) != 'u')
        return 0;
    if (takeRune(str, max) != 'e')
        return 0;
    return 1;
}


static inline int jsonParseFalse(char** str, int *max)
{
    if (takeRune(str, max) != 'f')
        return 0;
    if (takeRune(str, max) != 'a')
        return 0;
    if (takeRune(str, max) != 'l')
        return 0;
    if (takeRune(str, max) != 's')
        return 0;
    if (takeRune(str, max) != 'e')
        return 0;
    return 1;
}


static inline int jsonParseNumber(char** str, int* max, double* value)
{
    int sign = 1;
    double number = 0.0f;
    double integer = 0.0f;
    double fraction = 0.0f;
    int fractions = 0;
    int exponent = 0;
    int expsign = 1;
    int rune = 0;
    rune = takeRune(str, max);
    if (rune == '-')
        sign = -1, rune = takeRune(str, max);
    int state = 0;
    int oldState = 0;
    while (rune != 0 && state != -1)
    {
        oldState = state;
        switch (state) {
        case -1:
            break;
        case 0:
            if (rune == '0')
                state = 2, integer = integer * 10 + rune - '0';
            else if (rune >= '1' && rune <= '9')
                state = 1, integer = integer * 10 + rune - '0';
            else state = -1;
            break;
        case 1:
            if (rune >= '0' && rune <= '9')
                state = 1, integer = integer * 10 + rune - '0';
            else if (rune == '.')
                state = 3;
            else if (rune == 'e')
                state = 5;
            else
                state = -1;
            break;
        case 2:
            if (rune == '.')
                state = 3;
            else if (rune == 'e')
                state = 5;
            else
                state = -1;
            break;
        case 3:
            if (rune >= '0' && rune <= '9')
                state = 4, fraction = fraction * 10 + rune - '0', fractions++;
            else
                state = -1;
            break;
        case 4:
            if (rune >= '0' && rune <= '9')
                state = 4, fraction = fraction * 10 + rune - '0', fractions++;
            else if (rune == 'e')
                state = 5;
            else
                state = -1;
            break;
        case 5:
            if (rune == '+')
                state = 6, expsign = 1;
            else if (rune == '-')
                state = 8, expsign = -1;
            else if (rune >= '0' && rune <= '9')
                state = 7, expsign = 1, exponent = exponent * 10 + rune - '0';
            else
                state = -1;
            break;
        case 6:
            if (rune >= '0' && rune <= '9')
                state = 7, exponent = exponent * 10 + rune - '0';
            else
                state = -1;
            break;
        case 7:
            if (rune >= '0' && rune <= '9')
                state = 7, exponent = exponent * 10 + rune - '0';
            else
                state = -1;
            break;
        case 8:
            if (rune >= '0' && rune <= '9')
                state = 9, exponent = exponent * 10 + rune - '0';
            else
                state = -1;
            break;
        case 9:
            if (rune >= '0' && rune <= '9')
                state = 9, exponent = exponent * 10 + rune - '0';
            else
                state = -1;
            break;
        default:
            state = -1;
            break;
        }
        if (state == -1)
        {
            if (oldState == 0 || oldState == 3 || oldState == 5 || 
                oldState == 6 || oldState == 8)
            {
                return 0;
            }
            backRune(str, max, rune);
            break;
        }
        else
        {
            rune = takeRune(str, max);
        }
    }
    number = sign * integer;
    if (fractions > 0)
        number = number + fraction * qpow(10, -fractions);
    if (exponent > 0)
        number = number * qpow(10, expsign * exponent);
    *value = number;
    return 1;
}


static inline JsonString* jsonParseString(char** str, int* max)
{
    int state = 0;    
    int size = 0;
    int rune = takeRune(str, max);
    JsonByteBuf bytebuf;
    if (jsonInitByteBuf(&bytebuf, 8) == 0)
        return NULL;
    while (rune != 0 && state != -1)
    {
        size = unicodeRuneSize(rune);
        switch (state)
        {
        case -1:
            break;
        case 0:
            if (rune == '"')
                state = 1;
            else
                state = -1;
            break;
        case 1:
            if (rune == '"')
                state = 2;
            else if (rune == '\\')
                state = 3;
            else
                state = 1, jsonByteBufWrites(&bytebuf,(*str) - size, size);
            break;
        case 2:
            break;
        case 3:
            if (rune == '"')
                state = 1, jsonByteBufWrite(&bytebuf, '"');
            else if (rune == '\\')
                state = 1, jsonByteBufWrite(&bytebuf, '\\');
            else if (rune == '/')
                state = 1, jsonByteBufWrite(&bytebuf, '/');
            else if (rune == 'b')
                state = 1, jsonByteBufWrite(&bytebuf, 8);
            else if (rune == 'f')
                state = 1, jsonByteBufWrite(&bytebuf, 12);
            else if (rune == 'n')
                state = 1, jsonByteBufWrite(&bytebuf, 10);
            else if (rune == 'r')
                state = 1, jsonByteBufWrite(&bytebuf, 13);
            else if (rune == 't')
                state = 1, jsonByteBufWrite(&bytebuf, 9);
            else if (rune == 'u')
                state = 4;
            else
                state = -1;
            break;
        case 4:
            if ((rune >= '0' && rune <= '9') || (rune >= 'a' && rune <= 'f') || (rune >= 'A' && rune <= 'F'))
            {
                if (rune >= '0' && rune <= '9') rune = rune - '0';
                else if (rune >= 'a' && rune <= 'f') rune = 10 + rune - 'a';
                else if (rune >= 'A' && rune <= 'F') rune = 10 + rune - 'A';
                state = 5, jsonByteBufWrite(&bytebuf, (char)rune);
            }
            else
                state = -1;
            break;
        case 5:
            if ((rune >= '0' && rune <= '9') || (rune >= 'a' && rune <= 'f') || (rune >= 'A' && rune <= 'F'))
            {
                if (rune >= '0' && rune <= '9') rune = rune - '0';
                else if (rune >= 'a' && rune <= 'f') rune = 10 + rune - 'a';
                else if (rune >= 'A' && rune <= 'F') rune = 10 + rune - 'A';
                state = 6, jsonByteBufWrite(&bytebuf, (char)rune);
            }
            else
                state = -1;
            break;
        case 6:
            if ((rune >= '0' && rune <= '9') || (rune >= 'a' && rune <= 'f') || (rune >= 'A' && rune <= 'F'))
            {
                if (rune >= '0' && rune <= '9') rune = rune - '0';
                else if (rune >= 'a' && rune <= 'f') rune = 10 + rune - 'a';
                else if (rune >= 'A' && rune <= 'F') rune = 10 + rune - 'A';
                state = 7, jsonByteBufWrite(&bytebuf, (char)rune);
            }
            else
                state = -1;
            break;
        case 7:
            if ((rune >= '0' && rune <= '9') || (rune >= 'a' && rune <= 'f') || (rune >= 'A' && rune <= 'F'))
            {
                if (rune >= '0' && rune <= '9') rune = rune - '0';
                else if (rune >= 'a' && rune <= 'f') rune = 10 + rune - 'a';
                else if (rune >= 'A' && rune <= 'F') rune = 10 + rune - 'A';
                state = 1, jsonByteBufWrite(&bytebuf, (char)rune);
            }
            else
                state = -1;
            break;
        default:
            state = -1;
            break;
        }
        if (state == 2)
            break;
        if (state == -1)
        {
            jsonUninitByteBuf(&bytebuf);
            return NULL;
        }
        rune = takeRune(str, max);
    }
    return state == 2 ? jsonWrapString(&bytebuf) : NULL;
}


static inline JsonArray* jsonParseArray(char** str, int* len)
{
    JsonArray* array = jsonNewArray();
    int rune = 0;
    if ((rune = takeRune(str, len)) != '[')
        return backRune(str, len, rune), NULL;
    do
    {
        skipSpace(str, len);
        JsonValue* value = jsonParse(str, len);
        if (value == NULL || !jsonArrayAppend(array, value))
            return jsonDeleteArray(array), NULL;
        skipSpace(str, len);
        rune = takeRune(str, len);
        if (rune == ']')
            break;
        if (rune != ',')
            return jsonDeleteArray(array), backRune(str, len, rune), NULL;
    } while (1);
    return array;
}


static inline JsonObject* jsonParseObject(char** str, int* len)
{
    JsonObject* object = jsonNewObject();
    int rune = 0;
    if ((rune = takeRune(str, len)) != '{')
        return backRune(str, len, rune), NULL;
    do
    {
        skipSpace(str, len);
        JsonString* key = jsonParseString(str, len);
        if (key == NULL)
            return jsonDeleteObject(object), NULL;
        skipSpace(str, len);
        if ((rune = takeRune(str, len)) != ':')
            return jsonDeleteObject(object), backRune(str, len, rune), NULL;
        skipSpace(str, len);
        JsonValue* value = jsonParse(str, len);
        if (value == NULL)
            return jsonDeleteObject(object), jsonDeleteString(key), NULL;
        if (!jsonObjectPut(object, key->buf, value))
            return jsonDeleteObject(object), jsonDeleteString(key), NULL;
        jsonDeleteString(key);
        skipSpace(str, len);
        rune = takeRune(str, len);
        if (rune == '}')
            break;
        if (rune != ',')
            return jsonDeleteObject(object), backRune(str, len, rune), NULL;
    } while (1);
    return object;
}


JsonValue* jsonParse(char** str, int *max)
{
    int rune = peekRune(str, max);
    if (rune == 't')
    {
        if (jsonParseTrue(str, max))
            return &jsonTrue;
    }
    else if (rune == 'f')
    {
        if (jsonParseFalse(str, max))
            return &jsonFalse;
    }
    else if (rune == 'n')
    {
        if (jsonParseNull(str, max))
            return &jsonNull;
    }
    else if (rune == '-' || (rune >= '0' && rune <= '9'))
    {
        double value = 0;
        if (!jsonParseNumber(str, max, &value))
            return NULL;
        JsonValue* number = newJsonValue(JSON_NUMBER);
        if (number == NULL)
            return NULL;
        number->value.number = value;
        return number;
    }
    else if (rune == '"')
    {
        JsonString* jsonString = jsonParseString(str, max);
        if (jsonString == NULL)
            return NULL;
        JsonValue* string = newJsonValue(JSON_STRING);
        if (string == NULL)
            return NULL;
        string->value.string = jsonString;
        return string;
    }
    else if (rune == '[')
    {
        JsonArray* jsonArray = jsonParseArray(str, max);
        if (jsonArray == NULL)
            return NULL;
        JsonValue* array = newJsonValue(JSON_ARRAY);
        if (array == NULL)
            return NULL;
        array->value.array = jsonArray;
        return array;
    }
    else if (rune == '{')
    {
        JsonObject* jsonObject = jsonParseObject(str, max);
        if (jsonObject == NULL)
            return NULL;
        JsonValue* object = newJsonValue(JSON_OBJECT);
        if (object == NULL)
            return NULL;
        object->value.object = jsonObject;
        return object;
    }
    return NULL;
}

