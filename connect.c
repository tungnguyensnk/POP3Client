#include "connect.h"

ACCOUNT account;

int openConnection(const char *hostname, int port) {
    struct hostent *host;
    struct sockaddr_in addr;

    if ((host = gethostbyname(hostname)) == NULL) {
        printf("Không thể kết nối máy chủ.\n");
        exit(-1);
    }

    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = *(long *) (host->h_addr);

    if (connect(fd, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
        close(fd);
        printf("Không thể kết nối máy chủ.\n");
        exit(-1);
    }

    return fd;
}

SSL *initSSLConnect(const char *hostname, int port) {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());   /* set phuong thuc */

    if (ctx == NULL) {
        ERR_print_errors_fp(stderr);
        exit(-1);
    }
    int server = openConnection(hostname, port);
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, server);    /* set socket */
    if (SSL_connect(ssl) == -1)   /* thuc hien handshake */
    { ERR_print_errors_fp(stderr); }
    recvMessage(ssl);
    return ssl;
}

char *recvMessage(SSL *ssl) {
    int child;
    if ((child = fork()) == 0) {
        loading();
    } else {
        char *buffer = NULL;
        int bytes, count = 0;
        do {
            buffer = (char *) realloc(buffer, (count + 1024) * sizeof(char));
            memset(buffer + count, 0, 1024);
            bytes = SSL_read(ssl, buffer + strlen(buffer), 1024 * sizeof(char)); /*nhan du lieu*/
            count += bytes;
        } while (buffer[count - 1] != '\n');
        buffer[count] = 0;
        kill(child, SIGINT);
        usleep(1000);
        return buffer;
    }
}

char *recvMessageMultiLines(SSL *ssl) {
    int child;
    if ((child = fork()) == 0) {
        loading();
    } else {
        char *buffer = NULL;
        int bytes, count = 0;
        do {
            buffer = (char *) realloc(buffer, (count + 1024) * sizeof(char));
            memset(buffer + count, 0, 1024);
            bytes = SSL_read(ssl, buffer + strlen(buffer), 1024 * sizeof(char)); /*nhan du lieu*/
            count += bytes;
        } while (buffer[count - 1] != '\n' || buffer[count - 3] != '.');
        buffer[count] = 0;
        kill(child, SIGINT);
        usleep(1000);
        return buffer;
    }
}

void sendMessage(SSL *ssl, char *message) {
    char *tmp = (char *) malloc(strlen(message) + 3);
    bzero(tmp, strlen(message) + 3);
    if (message[strlen(message) - 1] == '\n')
        message[strlen(message) - 1] = 0;

    sprintf(tmp, "%s\r\n", message);
    SSL_write(ssl, tmp, (int) strlen(tmp));
}

void sendRawMessage(SSL *ssl, char *message) {
    SSL_write(ssl, message, (int) strlen(message));
}


SSL *verifyAccount(ACCOUNT account_t) {
    SSL *ssl = initSSLConnect("pop.gmail.com", 995);
    char command[500] = {0}, *response = NULL, check[10] = {0};

    sprintf(command, "user %s", account_t.username);
    sendMessage(ssl, command);
    recvMessage(ssl);

    sprintf(command, "pass %s", account_t.password);
    sendMessage(ssl, command);
    response = recvMessage(ssl);

    sscanf(response, "%s %*s", check);
    free(response);
    response = NULL;
    if (strcmp(check, "+OK") == 0)
        return ssl;
    SSL_free(ssl);
    return NULL;
}