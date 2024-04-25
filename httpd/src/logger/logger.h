#ifndef LOGGER_H
#define LOGGER_H
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../http/http.h"

void log_request(struct config *conf, struct request *r, char *ip_client);

void log_response(struct config *conf, struct request *r, char *ip_client);
#endif /* LOGGER_H */
