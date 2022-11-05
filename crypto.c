#include "crypto.h"

char *base64Encode(char *string) {
    char command[200] = {0};
    sprintf(command, "echo \"%s\" | base64 > out.txt", string);
    system(command);
    FILE *f = fopen("out.txt", "rb");
    if (f != NULL) {
        fseek(f, 0, SEEK_END);
        int fsize = ftell(f);
        fseek(f, 0, SEEK_SET);
        char *data = (char *) calloc(fsize, 1);
        fread(data, 1, fsize, f);
        fclose(f);
        return data;
    } else
        return NULL;
}

char *base64Decode(char *string) {
    char *tmp = NULL;
    while ((tmp = strstr(string, "\r\n")) != NULL) {
        memmove(tmp, tmp + 2, strlen(tmp) - 1);
    }
    char *command = malloc(strlen(string) + 50);
    sprintf(command, "echo \"%s\" | base64 --decode > out.txt", string);
    system(command);
    FILE *f = fopen("out.txt", "rb");
    if (f != NULL) {
        fseek(f, 0, SEEK_END);
        int fsize = ftell(f);
        fseek(f, 0, SEEK_SET);
        char *data = (char *) calloc(fsize, 1);
        fread(data, 1, fsize, f);
        fclose(f);
        return data;
    } else
        return NULL;
}

char *qprintDecode(char *string) {
    FILE *f1 = fopen("in.txt", "w");
    fprintf(f1, "%s", string);
    fclose(f1);
    f1 = NULL;

    char *command = malloc(strlen(string) + 50);
    sprintf(command, "qprint -d \"in.txt\" > out.txt");
    system(command);

    FILE *f2 = fopen("out.txt", "rb");
    if (f2 != NULL) {
        fseek(f2, 0, SEEK_END);
        int fsize = ftell(f2);
        fseek(f2, 0, SEEK_SET);
        char *data = (char *) calloc(fsize, 1);
        fread(data, 1, fsize, f2);
        fclose(f2);
        f2 = NULL;

        return data;
    } else
        return NULL;
}