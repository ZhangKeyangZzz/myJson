#ifndef __JSON__UNICODE__H__
#define __JSON__UNICODE__H__
#ifdef __cplusplus
extern "C" {
#endif

int unicodeIsBody(char byte);
int unicodeIsHead(char byte);
int unicodeSequenceLen(char head);
int unicodeRuneSize(int value);
char unicodeHeadData(char head);
char unicodeBodyData(char body);
int unicodeDecode(const char *utf8, int len, int *const rune);
int unicodeEncode(int rune, char *const utf8, int len);
int unicodeHash(const char* str);
int unicodeStrlen(const char* str);
int unicodeStrcmp(const char* str1, const char* str2);

#ifdef __cplusplus
}
#endif
#endif
