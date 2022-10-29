#include "connect.h"

int openConnection(const char *hostname, int port) {
    struct hostent *host;
    struct sockaddr_in addr;

    if ((host = gethostbyname(hostname)) == NULL) {
        printf("Khong the truy cap may chu.\n");
        exit(-1);
    }

    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//    bzero(&addr, sizeof(addr));
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = *(long *) (host->h_addr);

    if (connect(fd, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
        close(fd);
        printf("Khong the ket noi may chu.\n");
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

    char buffer[1024] = {0};
    SSL_read(ssl, buffer, sizeof(buffer));
    return ssl;
}

char *sendMessage(SSL *ssl, char *message) {
    if (message[strlen(message) - 1] == '\n')
        message[strlen(message) - 1] = '\0';
    char *buffer = malloc(1024 * sizeof(char));
    sprintf(message + strlen(message), "\r\n");
    SSL_write(ssl, message, (int) strlen(message)); /*gui du lieu*/
    int bytes = SSL_read(ssl, buffer, 1024 * sizeof(char)); /*nhan du lieu*/
    buffer[bytes] = 0;
    return buffer;
}