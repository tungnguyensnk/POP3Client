#include "connect.h"
#include "crypto.h"
#include "mail.h"

void sendMail(SSL *ssl, EMAIL *email);

SSL *loginSMTPServer(ACCOUNT account_t);

void formSendMail(SSL *ssl, char *username, char *receiver);