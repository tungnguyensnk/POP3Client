#include "crypto.h"

char *crypto(char *string, OPTION option, TYPE type) {
    if (option == ENCODE) {
        if (type == BASE64)
            return base64Encode(string);
    } else {
        if (type == BASE64)
            return base64Decode(string);
        if (type == QUOTED_PRINTABLE)
            return qprintDecode(string);
        if (type == _7BIT || type == _8BIT)
            return string;
    }
}

char *readDataOut() {
    FILE *f = fopen("out.txt", "rb");
    if (f != NULL) {
        fseek(f, 0, SEEK_END);
        int fsize = (int) ftell(f);
        fseek(f, 0, SEEK_SET);
        char *data = (char *) calloc(fsize, 1);
        fread(data, 1, fsize, f);
        fclose(f);
        return data;
    } else
        return NULL;
}

char *base64Encode(char *string) {
    char command[200] = {0};
    sprintf(command, "echo \"%s\" | base64 > out.txt", string);
    system(command);
    return readDataOut();
}

char *base64Decode(char *string) {
    replace(string, "\r\n", "");
    char *command = malloc(strlen(string) + 50);
    sprintf(command, "echo \"%s\" | base64 --decode > out.txt", string);
    system(command);
    return readDataOut();
}

char *qprintDecode(char *string) {
    FILE *f1 = fopen("in.txt", "w");
    fprintf(f1, "%s", string);
    fclose(f1);
    f1 = NULL;

    char *command = malloc(strlen(string) + 50);
    sprintf(command, "qprint -d \"in.txt\" > out.txt");
    system(command);

    return readDataOut();
}

void replace(char *string, char *old, char *new) {
    char *tmp = NULL;
    while ((tmp = strstr(string, old)) != NULL) {
        sprintf(tmp, "%s%s", new, tmp + strlen(old));
    }
}

void base64DecodeInString(char *string) {
    char *tmp = NULL;
    if ((tmp = strstr(string, "=?UTF-8?B?")) != NULL) {
        sprintf(string, "%s", tmp + 10);
        char *result = base64Decode(strtok(strdup(string), "?="));
        sprintf(string, "%s%s", result, strstr(string, "?=") + 2);
    }
}