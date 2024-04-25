#ifndef RESPONSE_H
#define RESPONSE_H
#include <fnmatch.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#include "../config/config.h"
#include "../utils/string/string.h"
#include "request.h"
struct string *response(struct request *r);

#endif /* RESPONSE_H */
