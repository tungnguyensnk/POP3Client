#include "mail.h"

void checkLine(char string[], char head[], char **pt) {
    if (strncasecmp(string, head, strlen(head)) == 0) {
        char tmp[50] = {0};
        sscanf(string, "%s", tmp);
        *pt = strdup(strstr(string, tmp) + strlen(tmp) + 1);
    }
}

void getBoundary(char string[], char **pt) {
    if (strncasecmp(string, "Content-Type: multipart", 23) == 0) {
        char tmp[50] = {0};
        sprintf(tmp, "--%s", strstr(string, "boundary") + 10);
        *pt = strdup(tmp);
        (*pt)[strlen(*pt) - 1] = 0;
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
            attachment->filename = strdup(strstr(line, "filename=") + 9);
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
        char tmp10[50] = {0};
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
    return email;
}

EMAIL *phanTichMail(SSL *ssl, int id) {
    char command[50] = {0}, *line = NULL, *boundary1 = NULL, *boundary2 = NULL, *data = NULL;
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

    line = strtok(strdup(response), "\r\n");
    while (line != NULL) {
        checkLine(line, "subject", &(email->subject));
        checkLine(line, "from", &(email->from));
        checkLine(line, "to:", &(email->to));
        if (boundary1 == NULL)
            getBoundary(line, &boundary1);
        else if (boundary2 == NULL)
            getBoundary(line, &boundary2);
        else
            break;
        line = strtok(NULL, "\r\n");
    }

    base64DecodeInString(email->subject);
    base64DecodeInString(email->from);
    base64DecodeInString(email->to);

    if (boundary2 != NULL)
        replace(response, boundary2, boundary1);
    data = strdup(response);
    while (data != NULL) {
        data = strstr(data, boundary1) != NULL ? strdup(strstr(data, boundary1) + strlen(boundary1) + 2) : NULL;
        if (data == NULL || strncasecmp(data, "Content-Type", 12) != 0 || strstr(data, boundary2 + 2) != NULL)
            continue;
        char *part = strdup(data);
        char *tmp = strstr(part, boundary1);
        if (tmp != NULL)
            tmp[0] = 0;

        while (part[strlen(part) - 1] == '\r' || part[strlen(part) - 1] == '\n')
            part[strlen(part) - 1] = 0;
        email = checkPart(part, email);

    }
    return email;

}