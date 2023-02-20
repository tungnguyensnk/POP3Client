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

void writeDataIn(char *string) {
    FILE *f1 = fopen("in.txt", "wb");
    if (f1 != NULL) {
        fwrite(string, 1, strlen(string), f1);
        fclose(f1);
    }
}

char *base64EncodeFile(char *filename) {
    char command[200] = {0};
    snprintf(command, sizeof command, "base64 %s > out.txt", filename);
    system(command);
    return readDataOut();
}

char *base64Encode(char *string) {
    writeDataIn(string);
    system("base64 in.txt > out.txt");
    return readDataOut();
}

char *base64EncodeString(char *string) {
    char command[2000] = {0};
    snprintf(command, sizeof command, "echo -n \"%s\" | base64 > out.txt", string);
    system(command);
    return readDataOut();
}

char *base64Decode(char *string) {
    string = replace(string, "\r\n", "");
    writeDataIn(string);
    system("base64 -d in.txt > out.txt");
    return readDataOut();
}

char *qprintDecode(char *string) {
    writeDataIn(string);
    system("qprint -d \"in.txt\" > out.txt");
    return readDataOut();
}

char *replace(char *string, char *old, char *new) {
    char *temp = NULL;
    char *tmp = NULL;
    if (strlen(new) > strlen(old)) {
        temp = malloc(strlen(string) + (strlen(new) - strlen(old)) * 100 + 1);
        snprintf(temp, sizeof temp, "%s", string);
    } else
        temp = strdup(string);

    while ((tmp = strstr(temp, old)) != NULL) {
        sprintf(tmp, "%s%s", new, tmp + strlen(old));
    }
    return temp;
}

char *decodeInSubString(char *string) {
    char *tmp = NULL, *decode = NULL;
    char *tmp1 = strdup(string);
    while ((tmp = strstr(tmp1, "=?")) != NULL) {
        char *begin = tmp;
        while (!(begin[0] == '?' && begin[2] == '?'))
            begin++;

        strstr(begin + 3, "?=")[2] = 0;
        char *input = strdup(begin + 3);
        strstr(input, "?=")[0] = 0;

        switch (begin[1]) {
            case 'B':
            case 'b':
                decode = base64Decode(input);
                break;
            case 'q':
            case 'Q':
                decode = qprintDecode(input);
                break;
        }

        string = replace(string, tmp, decode);
        tmp1 = strdup(string);
    }
    free(decode);
    free(tmp1);
    decode = NULL;
    tmp1 = NULL;
    return string;
}