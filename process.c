#include "process.h"

void signal_handler(int signum) {
    if (signum == SIGINT) {
        printf("\r \r");
        exit(0);
    }
}

void loading() {
    char list[] = {'|', '/', '-', '\\'};
    int i = 0;
    int count = 0;
    while (++count) {
        printf("\r%c", list[i++]);
        if (count == TIMEOUT * 10) {
            printf("\r \r");
            exit(0);
        }
        fflush(stdout);
        i = i == 4 ? 0 : i;
        usleep(100000);
    }
}

char *readDataFile() {
    char *buf = NULL;
    FILE *f = fopen("tmp", "rb");
    if (f != NULL) {
        fseek(f, 0, SEEK_END);
        int fsize = (int) ftell(f);
        fseek(f, 0, SEEK_SET);
        buf = (char *) calloc(fsize, 1);
        fread(buf, 1, fsize, f);
        fclose(f);
        return buf;
    } else
        return NULL;
}

void writeDataFile(char *string) {
    FILE *f = fopen("tmp", "wb");
    if (f != NULL) {
        fwrite(string, 1, strlen(string), f);
        fclose(f);
    }
}
