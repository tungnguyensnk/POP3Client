#include <time.h>
#include "pop3.h"

int totalLetters = 0, totalPages = 0;

void checkPara(char *string, char *head, char **pt) {
    if (strncasecmp(string, head, strlen(head)) == 0) {
        char *tmp1 = malloc(strlen(string) + 1);
        snprintf(tmp1, strlen(string) + 1, "%s", string);
        char *temp = strstr(tmp1, " ");
        if (temp != NULL) {
            *pt = strdup(temp + 1);
            temp = NULL;
        } else {
            *pt = strdup("NULL");
        }
        free(tmp1);
    }
}

void getBoundary(char string[], char **pt) {
    char *tmp1 = NULL;
    if ((tmp1 = strstr(string, "boundary=")) != NULL) {
        char tmp[50] = {0};
        sprintf(tmp, "--%s", tmp1 + 10);
        *pt = strdup(tmp);
        (*pt)[strlen(*pt) - 1] = 0;
        tmp1 = NULL;
    }
}

EMAIL *checkPart(char part[], EMAIL *email) {
    char encoding[50] = {0}, *header = NULL, *body = NULL, *line = NULL, kind[50] = {0};
    TYPE type;
    ATTACHMENT *attachment = NULL;
    body = strdup(strstr(part, "\r\n\r\n") + 4);
    header = strdup(part);
    strstr(header, "\r\n\r\n")[0] = 0;
    line = strtok(strdup(header), "\r\n");

    int hasAttachment = 0;
    while (line != NULL) {
        if (strncasecmp(line, "Content-Type:", 13) == 0) {
            sscanf(line, "%*s %s", kind);
            if (kind[strlen(kind) - 1] == ';')
                kind[strlen(kind) - 1] = 0;

        }
        if (strncasecmp(line, "Content-Transfer-Encoding:", 26) == 0) {
            sscanf(line, "%*s %s", encoding);
            if (strcasecmp(encoding, "base64") == 0) {
                type = BASE64;
            } else if (strcasecmp(encoding, "quoted-printable") == 0) {
                type = QUOTED_PRINTABLE;
            }
        }
        if (strncasecmp(line, "Content-Disposition: attachment", 31) == 0) {
            hasAttachment = 1;
            attachment = malloc(sizeof(ATTACHMENT));
            attachment->next = NULL;
            if (strstr(line, "filename=") != NULL)
                attachment->filename = strdup(strstr(line, "filename=") + 9);
            else {
                line = strtok(NULL, "\r\n");
                attachment->filename = strdup(strstr(line, "filename=") + 9);
            }
            if (attachment->filename[0] == '"') {
                attachment->filename = attachment->filename + 1;
                attachment->filename[strlen(attachment->filename) - 1] = 0;
            }
        }
        if (strncasecmp(line, "X-Attachment-Id:", 16) == 0) {
            sscanf(line, "%*s %s", attachment->id);
        }
        line = strtok(NULL, "\r\n");
    }

    char *result = crypto(body, DECODE, type);

    if (!hasAttachment) {
        if (strcasecmp(kind, "text/plain") == 0)
            email->plain = strdup(result);
        else if (strcasecmp(kind, "text/html") == 0)
            email->html = strdup(result);
    } else {
        attachment->data = strdup(body);
        rename("out.txt", attachment->filename);
        char tmp10[120] = {0};
        sprintf(tmp10, "cid:%s", attachment->id);
        replace(email->html, tmp10, attachment->filename);
        if (email->attachments == NULL)
            email->attachments = attachment;
        else {
            ATTACHMENT *tmp = email->attachments;
            while (tmp->next != NULL)
                tmp = tmp->next;
            tmp->next = attachment;
        }
    }
    free(header);
    free(body);
    header = NULL;
    body = NULL;

    return email;
}

void convertTime(char *string) {
    struct tm *info = malloc(sizeof(struct tm));
    memset(info, 0, sizeof(struct tm));
    int timezone_t = 0;
    strptime(string, "%a, %d %b %Y %H:%M:%S", info);
    sscanf(string, "%*s %*s %*s %*s %*s %d", &timezone_t);
    timezone_t /= 100;

    time_t oldtime = mktime(info) + (7 - timezone_t) * 3600;
    info = localtime(&oldtime);

    time_t now = time(NULL);

    int diff = (int) difftime(now, oldtime);
    if (diff < 60)
        sprintf(string, "%d giây trước", diff);
    else if (diff < 3600)
        sprintf(string, "%d phút trước", diff / 60);
    else if (diff < 86400)
        sprintf(string, "%d giờ trước", diff / 3600);
    else if (diff < 604800)
        sprintf(string, "%d ngày trước", diff / 86400);
    else
        strftime(string, 30, "%H:%M:%S %d/%m/%Y", info);
}

EMAIL *getHeader(EMAIL *email, char *response, char **boundary1, char **boundary2) {
    char *line = NULL, *part = NULL;
    part = response;
    while ((part = strstr(part, "\r\n")) != NULL) {
        if (strncasecmp(part, "\r\nContent-Type", 14) == 0)
            break;
        if (part[2] == ' ' || part[2] == '\t') {
            int i = 2;
            while (part[i] == ' ' || part[i] == '\t')
                i++;
            memcpy(part, part + i, strlen(part + i) + 1);
        } else
            part += 2;
    }

    int ran = 0;
    char *tmp = malloc(strlen(response) + 1);
    snprintf(tmp, strlen(response) + 1, "%s", response);
    while ((line = strtok((ran == 0 ? tmp : NULL), "\r\n")) != NULL) {
        ran = 1;
        checkPara(line, "subject", &(email->subject));
        checkPara(line, "from", &(email->from));
        checkPara(line, "to:", &(email->to));
        checkPara(line, "date:", &(email->date));
        if (*boundary1 == NULL)
            getBoundary(line, boundary1);
        else if (*boundary2 == NULL)
            getBoundary(line, boundary2);
        else
            break;
    }
    free(tmp);
    email->subject = decodeInSubString(email->subject);
    email->from = decodeInSubString(email->from);
    email->to = decodeInSubString(email->to);
    convertTime(email->date);

    return email;
}

EMAIL *analyzingHeaderMail(SSL *ssl, int id) {
    char command[50] = {0}, *tmp = NULL;
    EMAIL *email = malloc(sizeof(EMAIL));
    bzero(email, sizeof(EMAIL));
    sprintf(command, "top %d 0", id);
    sendMessage(ssl, command);
    char *response = recvMessageMultiLines(ssl);
    email = getHeader(email, response, &tmp, &tmp);
    return email;
}

EMAIL *analyzingMail(SSL *ssl, int id) {
    char command[50] = {0}, *boundary1 = NULL, *boundary2 = NULL, *data = NULL;
    EMAIL *email = malloc(sizeof(EMAIL));
    bzero(email, sizeof(EMAIL));
    sprintf(command, "retr %d", id);
    sendMessage(ssl, command);
    char *response = recvMessageMultiLines(ssl);

//    char *response = NULL;
//    FILE *f = fopen("tmp1.txt", "rb");
//    if (f != NULL) {
//        fseek(f, 0, SEEK_END);
//        int fsize = ftell(f);
//        fseek(f, 0, SEEK_SET);
//        response = (char *) calloc(fsize, 1);
//        fread(response, 1, fsize, f);
//        fclose(f);
//    }

    email = getHeader(email, response, &boundary1, &boundary2);

    if (boundary2 != NULL)
        replace(response, boundary2, boundary1);
    data = strdup(response);

    while (data != NULL) {
        if (boundary1 != NULL) {
            char *tmp = strstr(data, boundary1);
            if (tmp != NULL) {
                data = tmp != NULL ? strdup(tmp) : NULL;
                data += strlen(boundary1) + 2;
            } else
                data = NULL;
            if (data == NULL || strncasecmp(data, "Content-Type", 12) != 0 ||
                (boundary2 != NULL && strstr(data, boundary2 + 2) != NULL))
                continue;
            char *part = strdup(data);
            tmp = strstr(part, boundary1);
            if (tmp != NULL)
                tmp[0] = 0;

            while (part[strlen(part) - 1] == '\r' || part[strlen(part) - 1] == '\n')
                part[strlen(part) - 1] = 0;
            email = checkPart(part, email);
        } else {
            data = strstr(data, "\r\nContent-Type:") != NULL ? strstr(data, "\r\nContent-Type:") + 2 : NULL;
            if (data != NULL) {
                strstr(data, "\r\n.\r\n")[0] = 0;
                email = checkPart(data, email);
            }
            data = NULL;
        }

    }
    return email;

}

void showPageLetters(SSL *ssl, int page) {
    for (int i = (page - 1) * 10 + 1; i <= (page * 10 < totalLetters ? page * 10 : totalLetters); ++i) {
        EMAIL *email = analyzingHeaderMail(ssl, i);
        printf(C_BRIGHT_WHITE"\n%-4d------------------------------------------------------------"C_OFF, i);
        printf("\nTiêu đề: %.60s\nThời gian: %s\nNgười gửi: %s\nNgười nhận: %s\n", email->subject, email->date,
               email->from, email->to);
        printf(C_BRIGHT_WHITE"----------------------------------------------------------------\n"C_OFF);
    }
}

void getTotalLetters(SSL *ssl) {
    char command[50] = "stat", *response = NULL;
    sendMessage(ssl, command);
    response = recvMessage(ssl);
    sscanf(response, "%*s %d %*s", &totalLetters);
    totalPages = ceil(totalLetters / 10.0);
    free(response);
    response = NULL;
}

void genHTML(char *string) {
    char *content = NULL;
    if (strstr(string, "!DOCTYPE") == NULL) {
        content = malloc(strlen(string) + 600);
        memset(content, 0, strlen(string) + 600);
        snprintf(content, strlen(string) + 600,
                 "<!DOCTYPE html> <html lang=\"vi\"> <head> <meta charset=\"UTF-8\"> "
                 "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"> "
                 "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> "
                 "<title>Gmail</title><style> body { height: 95vh; display: flex; align-items: center; } "
                 " #main { width: 50%%; margin: auto; border-radius: 20px; padding: 40px;"
                 " background: #ecf0f3; box-shadow: 14px 14px 20px #cbced1, -14px -14px 20px white; } "
                 "</style></head> <body> <div id=\"main\">%s</div> </body> </html>",
                 string);
    } else
        content = strdup(string);
    FILE *f3 = fopen("content.html", "w");
    fprintf(f3, "%s", content);
    fclose(f3);
    free(content);
    content = NULL;
    f3 = NULL;
}