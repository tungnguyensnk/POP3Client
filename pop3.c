﻿#include "pop3.h"

int totalLetters = 0, totalPages = 0;

EMAIL *getHeaderLetter(SSL *ssl, int id) {
    char command[50] = {0}, *response = NULL;
    EMAIL *email = malloc(sizeof(EMAIL));
    sprintf(command, "top %d 0", id);
    sendMessage(ssl, command);
    response = recvMessageMultiLines(ssl);

    email->subject = strdup(strstr(response, "Subject"));
    email->from = strdup(strstr(response, "From"));
    email->to = strstr(response, "\nTo") != NULL ? (strdup(strstr(response, "\nTo") + 1)) : NULL;

    if (email->subject != NULL)
        strstr(email->subject, "\r\n")[0] = 0;
    if (email->from != NULL)
        strstr(email->from, "\r\n")[0] = 0;
    if (email->to != NULL)
        strstr(email->to, "\r\n")[0] = 0;

    free(response);
    response = NULL;
    return email;
}

EMAIL *getContent(SSL *ssl, int id) {
    EMAIL *email = getHeaderLetter(ssl, id);

    char command[50] = {0}, *tmp = NULL;
    sprintf(command, "retr %d", id);
    sendMessage(ssl, command);
    char *response = recvMessageMultiLines(ssl);
    email->html = strdup(strstr(response, "Content-Type: text/html;"));
    tmp = strstr(email->html, "\r\n--00");
    if (tmp != NULL) {
        tmp[0] = 0;
    } else
        strstr(email->html, "\r\n--")[0] = 0;

    if (strstr(email->html, "Content-Transfer-Encoding: ") != NULL) {
        email->html = strstr(email->html, "Content-Transfer-Encoding: ") + 27;

        if (email->html[strlen(email->html) - 1] == '\n' && email->html[strlen(email->html) - 2] == '\r')
            email->html[strlen(email->html) - 2] = '\0';

        if (strncasecmp(email->html, "quoted-printable", 16) == 0) {
            email->html = strstr(email->html, "\r\n\r\n") + 4;
            email->html = qprintDecode(email->html);
        } else if (strncasecmp(email->html, "base64", 6) == 0) {
            email->html = strstr(email->html, "\r\n\r\n") + 4;
            email->html = base64Decode(email->html);
        } else {
            printf("Định dạng mã hóa chưa được hỗ trợ.");
            email->html = NULL;
        }
    } else
        email->html = strstr(email->html, "\r\n\r\n") + 4;

    email->attachment = strstr(response, "Content-Disposition: attachment;") != NULL ?
                        strdup(strstr(response, "Content-Disposition: attachment;")) : NULL;
    if (email->attachment != NULL) {
        strstr(email->attachment, "\r\n--00")[0] = 0;

        email->filename = strdup(strstr(email->attachment, "filename=\"") + 10);
        strstr(email->filename, "\"\r\n")[0] = 0;

        email->attachment = strdup(strstr(email->attachment, "\r\n\r\n") + 4);

        tmp = NULL;
        while ((tmp = strstr(email->attachment, "\r\n")) != NULL) {
            memmove(tmp, tmp + 2, strlen(tmp) - 1);
        }

        char *cmd = malloc(strlen(email->attachment) + 50);
        sprintf(cmd, "echo \"%s\" | base64 --decode > %s", email->attachment, email->filename);
        system(cmd);
    }

    return email;
}

void showPageLetters(SSL *ssl, int page) {
    for (int i = (page - 1) * 10 + 1; i <= page * 10; ++i) {
        EMAIL *email = getHeaderLetter(ssl, i);
        printf("\n%-4d------------------------------------------------------------", i);
        printf("\n%s\n", email->subject);
        printf("%s\n", email->from);
        printf("%s\n", email->to);
        printf("----------------------------------------------------------------\n");
    }
}

void getTotalLetters(SSL *ssl) {
    char command[50] = "stat", *response = NULL;
    sendMessage(ssl, command);
    response = recvMessage(ssl);
    sscanf(response, "%*s %d %*s", &totalLetters);
    totalPages = ceil(totalLetters / 10.0);
    return;
}

void genHTML(char *string) {
    char *content = NULL;
    if (strstr(string, "!DOCTYPE") == NULL) {
        content = malloc(strlen(string) + 600);
        memset(content, 0, strlen(string) + 600);
        snprintf(content, strlen(string) + 600,
                 "<!DOCTYPE html> <html lang=\"vi\"> <head> <meta charset=\"UTF-8\"> <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> <title>Gmail</title><style> body { height: 95vh; display: flex; align-items: center; }  #main { width: 50%%; margin: auto; border-radius: 20px; padding: 40px; background: #ecf0f3; box-shadow: 14px 14px 20px #cbced1, -14px -14px 20px white; } </style></head> <body> <div id=\"main\">%s</div> </body> </html>",
                 string);
    } else
        content = strdup(string);
    FILE *f3 = fopen("content.html", "w");
    fprintf(f3, "%s", content);
    fclose(f3);
}