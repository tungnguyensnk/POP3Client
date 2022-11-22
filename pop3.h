#include "connect.h"
#include "crypto.h"
#include "mail.h"

void getTotalLetters(SSL *ssl);

EMAIL *getHeaderLetter(SSL *ssl, int id);

void showPageLetters(SSL *ssl, int page);

EMAIL *getContent(SSL *ssl, int id);

void genHTML(char *string);