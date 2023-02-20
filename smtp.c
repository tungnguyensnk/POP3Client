#include "smtp.h"

char *randomString(int length) {
    srand(time(NULL));
    char *string = (char *) calloc(length + 1, 1);
    for (int i = 0; i < length; i++) {
        int randChar = rand() % 26 + 97;
        string[i] = (char) randChar;
    }
    return string;
}

void sendMail(SSL *ssl, EMAIL *email) {
    char command[500] = {0}, *response = NULL;
    int check = 0;

    sprintf(command, "mail from: <%s>", email->from);
    sendMessage(ssl, command);
    response = recvMessage(ssl);
    sscanf(response, "%d %*s", &check);
    if (check != 250)
        return;

    sprintf(command, "rcpt to: <%s>", email->to);
    sendMessage(ssl, command);
    response = recvMessage(ssl);
    sscanf(response, "%d %*s", &check);
    if (check != 250)
        return;

    sendMessage(ssl, "data");
    response = recvMessage(ssl);
    sscanf(response, "%d %*s", &check);
    if (check != 354)
        return;

    sprintf(command, "From: %s\r\nTo: %s\r\nSubject: %s\r\n", email->from, email->to, email->subject);
    sendRawMessage(ssl, command);
    char *boundary = randomString(20);
    if (email->attachments != NULL) {
        char *tmp = NULL;
        sprintf(command, "Content-Type: multipart/alternative; boundary=\"%s\"\r\n\r\n", boundary);
        sendRawMessage(ssl, command);
        sprintf(command, "--%s\r\nContent-Type: text/plain; charset=\"utf-8\"\r\n\r\n%s\r\n", boundary,
                email->plain);
        sendRawMessage(ssl, command);
        ATTACHMENT *attachment = email->attachments;
        while (attachment != NULL) {
            sprintf(command,
                    "--%s\r\nContent-Type: %s; name=\"%s\"\r\nContent-Transfer-Encoding: base64\r\n"
                    "Content-Disposition: attachment; filename=\"%s\"\r\n\r\n",
                    boundary, "application/octet-stream", attachment->filename, attachment->filename);
            sendRawMessage(ssl, command);
            tmp = base64EncodeFile(attachment->filename);
            sendRawMessage(ssl, tmp);
            sendRawMessage(ssl, "\r\n");
            attachment = attachment->next;
        }
        sprintf(command, "--%s--\r\n", boundary);
        sendRawMessage(ssl, command);
    } else {
        sprintf(command, "%s\r\n", email->plain);
        sendRawMessage(ssl, command);
    }
    sendMessage(ssl, ".");
    response = recvMessage(ssl);
    sscanf(response, "%d %*s", &check);
    if (check == 250)
        printf("Gửi mail thành công!\n");
    else
        printf("Gửi mail thất bại!\n");
    printf("Nhấn phím bất kỳ để trở về menu...\n");
    getchar();
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
    sendMessage(ssl, base64EncodeString(command));
    response = recvMessage(ssl);
    sscanf(response, "%d %*s", &check);
    if (check == 235)
        return ssl;
    return NULL;
}

void formSendMail(SSL *ssl, char *username, char *receiver) {
    EMAIL *email = (EMAIL *) malloc(sizeof(EMAIL));
    email->from = strdup(username);
    char input[1000] = {0};
    printf("Nhập tiêu đề: ");
    fgets(input, sizeof(input), stdin);
    if (input[strlen(input) - 1] == '\n')
        input[strlen(input) - 1] = '\0';
    email->subject = strdup(input);
    if (receiver == NULL) {
        printf("Nhập người nhận: ");
        fgets(input, sizeof(input), stdin);
        if (input[strlen(input) - 1] == '\n')
            input[strlen(input) - 1] = '\0';
        email->to = strdup(input);
    } else {
        email->to = strdup(receiver);
    }
    printf("Nhập nội dung: ");
    fgets(input, sizeof(input), stdin);
    if (input[strlen(input) - 1] == '\n')
        input[strlen(input) - 1] = '\0';
    email->plain = strdup(input);
    while (1) {
        printf("Nhập file đính kèm (Nhập 0 để bỏ qua): ");
        fgets(input, sizeof(input), stdin);
        if (input[strlen(input) - 1] == '\n')
            input[strlen(input) - 1] = '\0';
        if (strcmp(input, "0") == 0)
            break;
        FILE *file = fopen(input, "r");
        if (file == NULL) {
            printf("Không tìm thấy file.\n");
            continue;
        }
        if (email->attachments == NULL) {
            email->attachments = (ATTACHMENT *) malloc(sizeof(ATTACHMENT));
            email->attachments->filename = strdup(input);
            email->attachments->next = NULL;
        } else {
            ATTACHMENT *attachment = email->attachments;
            while (attachment->next != NULL) {
                attachment = attachment->next;
            }
            attachment->next = (ATTACHMENT *) malloc(sizeof(ATTACHMENT));
            attachment->next->filename = strdup(input);
            attachment->next->next = NULL;
        }
    }
    sendMail(ssl, email);
    free(email);
}