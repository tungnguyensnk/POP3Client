#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

int openConnection(const char *hostname, int port);

SSL *initSSLConnect(const char *hostname, int port);

char *sendMessage(SSL *ssl, char *message);