#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../http/http.h"
#include "../logger/logger.h"
#define BUFFER_SIZE 4096

int server(struct config *conf);

#endif /* SERVER_H */
