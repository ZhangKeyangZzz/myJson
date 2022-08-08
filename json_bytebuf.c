#include <stdlib.h>
#include <string.h>
#include "json.h"
#include "json_value.h"


#define DEFAULT_CAPACITY    8


static inline int jsonByteBufGrow(JsonByteBuf* bytebuf, int increment)
{
    int newCapacity = 2 * bytebuf->cap + increment;
    char* newBytes = (char*)malloc(newCapacity * sizeof(char));
    if (newBytes == NULL)
        return 0;
    memcpy(newBytes, bytebuf->bytes, bytebuf->len);
    free(bytebuf->bytes);
    bytebuf->cap = newCapacity;
    bytebuf->bytes = newBytes;
    return 1;
}


int jsonInitByteBuf(JsonByteBuf* bytebuf, int capacity)
{
    if (capacity <= 0)
        capacity = DEFAULT_CAPACITY;
    bytebuf->bytes = (char*)malloc(capacity * sizeof(char));
    if (bytebuf->bytes == NULL)
        return 0;
    bytebuf->len = 0;
    bytebuf->cap = capacity;
    return 1;
}


void jsonUninitByteBuf(JsonByteBuf* bytebuf)
{
    free(bytebuf->bytes);
}


int jsonByteBufWrite(JsonByteBuf* bytebuf, char ch)
{
    if (bytebuf->len + 1 >= bytebuf->cap)
        if (!jsonByteBufGrow(bytebuf, 1))
            return 0;
    bytebuf->bytes[bytebuf->len] = ch;
    bytebuf->len++;
    return 1;
}


int jsonByteBufWrites(JsonByteBuf* bytebuf, const char* buf, int len)
{
    if (bytebuf->len + len >= bytebuf->cap)
        if (!jsonByteBufGrow(bytebuf, len))
            return 0;
    memcpy(bytebuf->bytes + bytebuf->len, buf, len);
    bytebuf->len += len;
    return 1;
}

