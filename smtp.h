#include "connect.h"
#include "crypto.h"

void sendMail(SSL *ssl);

SSL *loginSMTPServer(ACCOUNT account_t);