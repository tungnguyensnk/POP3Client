#ifndef connect_h
#define connect_h

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <bits/sigthread.h>
#include <math.h>

typedef struct accounts {
    char username[100];
    char password[100];
} ACCOUNT;
#endif

int openConnection(const char *hostname, int port);

SSL *initSSLConnect(const char *hostname, int port);

void sendMessage(SSL *ssl, char *message);

char *recvMessage(SSL *ssl);

char *recvMessageMultiLines(SSL *ssl);

SSL *verifyAccount(ACCOUNT account_t);
