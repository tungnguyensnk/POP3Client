#pragma once

#include <bio.h>
#include <string.h>

typedef enum OPTION {
    ENCODE = 0,
    DECODE = 1
} OPTION;

typedef enum TYPE {
    BASE64 = 0,
    QUOTED_PRINTABLE = 1,
    _8BIT = 2,
    _7BIT = 3

} TYPE;

char *crypto(char *string, enum OPTION option, enum TYPE type);

char *base64Encode(char *string);

char *base64Decode(char *string);

char *qprintDecode(char *string);

char *replace(char *string, char *old, char *new);

char *decodeInSubString(char *string);

char *readDataOut();

char *base64EncodeFile(char *filename);