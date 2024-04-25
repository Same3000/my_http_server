#ifndef DAEMON_H
#define DAEMON_H
#define _POSIX_SOURCE

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../config/config.h"
#include "../parse_simple/parse_simple.h"
#include "../server/server.h"

int daemon(struct config *conf, struct parse *parsed);

#endif /* DAEMON_H */
