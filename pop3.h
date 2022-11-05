#include "connect.h"
#include "crypto.h"

typedef struct mail {
    char *from, *to, *subject, *content,*html, *filename, *attachment;
} EMAIL;

void getTotalLetters(SSL *ssl);

EMAIL *getHeaderLetter(SSL *ssl, int id);

void showPageLetters(SSL *ssl, int page);

EMAIL *getContent(SSL *ssl, int id);

void genHTML(char *string);