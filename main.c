#include "connect.h"
#include "crypto.h"
#include "smtp.h"
#include "pop3.h"

extern ACCOUNT account;
extern int totalLetters;

void clrs() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
//    SSL *ssl = initSSLConnect("smtp.gmail.com", 465);
//    sendMail(ssl);
    sprintf(account.username, "tungnguyensnk");
    sprintf(account.password, "heenydrjzorhurjy");
    SSL *ssl = verifyAccount(account);
    //getTotalLetters(ssl);

    //showPageLetters(ssl, 1);
    genHTML(getContent(ssl, 5)->html);
    return 0;
}