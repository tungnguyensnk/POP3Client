#include <ctype.h>
#include "connect.h"
#include "crypto.h"
#include "mail.h"
#include "text.h"

extern char *strptime(const char *__restrict _s,
                      const char *__restrict _fmt, struct tm *_tp)
__THROW;

EMAIL *analyzingMail(SSL *ssl, int id);

EMAIL *analyzingHeaderMail(SSL *ssl, int id);

void getTotalLetters(SSL *ssl);

void convertTime(char *string);

void showPageLetters(SSL *ssl, int page);

void genHTML(char *string);