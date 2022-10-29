#include "connect.h"
#include "base64.h"
#include "smtp.h"
#include "pop3.h"

int main() {
    SSL *ssl = initSSLConnect("smtp.gmail.com", 465);
    sendMail(ssl);
    return 0;
}