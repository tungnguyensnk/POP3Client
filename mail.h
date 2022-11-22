#pragma once
#define TEXTPLAIN "multipart/mixed"

#include <stdio.h>
#include "connect.h"
#include "crypto.h"

typedef struct attachment {
    char *data;
    char *filename;
    char id[100];
    struct attachment *next;
} ATTACHMENT;

typedef struct mail {
    char *from, *to, *subject, *plain, *html;
    ATTACHMENT *attachments;
} EMAIL;

EMAIL *phanTichMail(SSL *ssl, int id);