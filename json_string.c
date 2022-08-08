#include <stdlib.h>
#include <string.h>
#include "json.h"
#include "json_value.h"
#include "unicode.h"


JsonString* jsonNewString(const char* str, int len)
{
    JsonString* json = (JsonString*)malloc(sizeof(JsonString));
    if (json == NULL)
        return NULL;
    int bytes = 0;
    char* ptr = (char*)str;
    while (bytes < len)
    {
        if (unicodeIsHead(*ptr))
        {
            int seqLen = unicodeSequenceLen(*ptr);
            ptr += seqLen;
            bytes += seqLen;
        }
        else
        {
            break;
        }
    }
    json->buf = (char*)malloc((bytes + 1) * sizeof(char));
    if (json->buf == NULL)
        return free(json), NULL;
    memcpy(json->buf, str, bytes);
    json->buf[bytes] = 0;
    json->len = unicodeStrlen(json->buf);
    json->bytes = bytes + 1;
    return json;
}


JsonString* jsonWrapString(JsonByteBuf* bytebuf)
{
    JsonString* json = (JsonString*)malloc(sizeof(JsonString));
    if (json == NULL)
        return NULL;
    jsonByteBufWrite(bytebuf, 0);
    json->buf = bytebuf->bytes;
    json->bytes = bytebuf->len;
    json->len = unicodeStrlen(json->buf);
    return json;
}


void jsonDeleteString(JsonString* string)
{
    free(string->buf);
    free(string);
}
