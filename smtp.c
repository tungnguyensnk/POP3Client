#include "smtp.h"

void sendMail(SSL *ssl) {

}

SSL *loginSMTPServer(ACCOUNT account_t) {
    SSL *ssl = initSSLConnect("smtp.gmail.com", 465);
    char command[500] = {0}, *response = NULL;
    int check = 0;

    sprintf(command, "helo localhost");
    sendMessage(ssl, command);
    recvMessage(ssl);

    sprintf(command, "auth plain");
    sendMessage(ssl, command);
    recvMessage(ssl);

    sprintf(command, "\\0%s\\0%s", account_t.username, account_t.password);
    sendMessage(ssl, base64Encode(command));
    response = recvMessage(ssl);
    sscanf(response, "%d %*s", &check);
    if (check == 235)
        return ssl;
    return NULL;
}