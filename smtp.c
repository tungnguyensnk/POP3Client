#include "smtp.h"

void sendMail(SSL *ssl) {
    char message[1024] = {0};
    sprintf(message, "helo localhost");
    printf("%s", sendMessage(ssl, message));
    sprintf(message, "auth login");
    printf("%s", sendMessage(ssl, message));
    printf("Nhap email:");
    //fgets(message, sizeof(message), stdin);
    sprintf(message,"tungnguyensnk");
    if (message[strlen(message) - 1] == '\n')
        message[strlen(message) - 1] = '\0';
    printf("%s\n", sendMessage(ssl, base64Encode(message)));

    printf("Nhap pass:");
    //fgets(message, sizeof(message), stdin);
    sprintf(message,"heenydrjzorhurjy");
    if (message[strlen(message) - 1] == '\n')
        message[strlen(message) - 1] = '\0';
    printf("%s\n", sendMessage(ssl, base64Encode(message)));
    sprintf(message, "mail from:<tungnguyen@gmail.com>");
    printf("%s", sendMessage(ssl, message));
    sprintf(message, "rcpt to:<tung20194714@gmail.com>");
    printf("%s", sendMessage(ssl, message));
    sprintf(message, "data");
    printf("%s", sendMessage(ssl, message));
    sprintf(message, "subject: hello\r\nhi\r\n.");
    printf("%s", sendMessage(ssl, message));

    SSL_free(ssl);
}