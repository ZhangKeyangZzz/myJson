#include "unicode.h"
#include <string.h>


int unicodeIsBody(char byte)
{
    unsigned char body = (unsigned char)byte;
    return (body >> 6) == 2;
}


int unicodeIsHead(char byte)
{
    unsigned char head  = (unsigned char)byte;
    unsigned char high  = 7;
    unsigned char scale = 254;
    for (int i = 1; i <= 5; i++, high--)
    {
        scale &= ~(1 << high);
        if (scale == (head >> i))
            return 1;
    }
    return (head >> 7) == 0;
}


int unicodeSequenceLen(char head)
{
    if (!unicodeIsHead(head))
        return 0;
    unsigned char byte = head;
    if (byte >> 7 == 0)
        return 1;
    int result = 0;
    for (int bit = 7; bit >= 2; bit--)
    {
        if ((byte >> bit) == 1)
        {
            byte &= ~(1 << bit);
            result++;
        }
        else break;
    }
    return result;
}


int unicodeRuneSize(int value)
{
    if (value < 0x80) return 1;
    for (int bit = 11, len = 2; bit <= 31; bit += 5, len++)
        if (value < (1 << bit))
            return len;
    return 0;
}


char unicodeHeadData(char head)
{
    unsigned char byte = (unsigned char)head;
    int runeLen = unicodeSequenceLen(head);
    return runeLen == 1 ?
           byte & ~(1 << 7) :
           byte & ~(((1 << (runeLen + 1)) - 1) << (7 - runeLen));
}


char unicodeBodyData(char body)
{
    if (!unicodeIsBody(body))
        return 0;
    unsigned char byte = (unsigned char)body;
    return byte & ~(3 << 6);
}


int unicodeDecode(const char *utf8, int len, int *const rune)
{
    int pos = 0;
    if (!unicodeIsHead(*utf8))
        return 0;
    int seqLen = unicodeSequenceLen(*utf8);
    if (pos + seqLen > len)
        return 0;
    int data = unicodeHeadData(*utf8);
    if (seqLen > 1)
        for (int i = 1; i < seqLen; i++)
            data = (data << 6) + unicodeBodyData(utf8[i]);
    *rune = data;
    return 1;
}


int unicodeEncode(int rune, char *const utf8, int len)
{
    int runeLen = unicodeRuneSize(rune);
    if (runeLen > len)
        return 0;
    for (int i = 0; i < runeLen - 1; i++)
    {
        utf8[runeLen - i - 1] = (char)(rune & 0x3F) | 0x80;
        rune >>= 6;
    }
    *utf8 = (char) rune;
    if (runeLen > 1)
        *utf8 |= ((1 << (runeLen + 1)) - 2) << (7 - runeLen);
    return runeLen;
}


int unicodeHash(const char* str)
{
    if (str == NULL)
        return 0;
    int rune = 0;
    int hash = 1315423911;    
    int len = strlen(str);
    int index = 0;
    while (index < len && unicodeIsHead(*str))
    {
        if (unicodeDecode(str, len, &rune))
        {
            int seqLen = unicodeSequenceLen(*str);
            hash ^= (hash << 5) + rune + (hash >> 2);
            str += seqLen;
            index += seqLen;
        }
        else
        {
            break;
        }
    }
    return hash;
}


int unicodeStrlen(const char* str)
{
    int i = 0;
    int runes = 0;
    int bytes = strlen(str);
    char* ptr = (char*)str;
    while (i < bytes)
    {
        if (unicodeIsHead(*ptr))
        {
            int seqLen = unicodeSequenceLen(*ptr);
            i += seqLen;
            runes++;
        }
        else
        {
            break;
        }
    }
    return runes;
}


int unicodeStrcmp(const char* str1, const char* str2)
{
    int l1 = strlen(str1);
    int l2 = strlen(str2);
    int ans = 0;
    char* ptr1 = (char*)str1;
    char* ptr2 = (char*)str2;
    while (l1 > 0 && l2 > 0 && ans == 0)
    {
        int b1 = 0;
        int b2 = 0;
        int r1 = 0;
        int r2 = 0;
        if ((b1 = unicodeDecode(ptr1, l1, &r1)) == 1)
        {
            int seqLen = unicodeSequenceLen(*ptr1);
            l1 -= seqLen;
            ptr1 += seqLen;
        }
        if ((b2 = unicodeDecode(ptr2, l2, &r2)) == 1)
        {
            int seqLen = unicodeSequenceLen(*ptr2);
            l2 -= seqLen;
            ptr2 += seqLen;
        }
        if (!b1 && !b2) return ans;
        if (b1 && !b2)  return r1;
        if (!b1 && b2)  return r2;
        if (b1 && b2)   ans = r1 - r2;
    }
    return ans;
}
