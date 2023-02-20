#pragma once

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
    char *from, *to, *subject, *date, *plain, *html;
    ATTACHMENT *attachments;
} EMAIL;