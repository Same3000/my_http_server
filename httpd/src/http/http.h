#ifndef HTTP_H
#define HTTP_H

#include <fnmatch.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "../config/config.h"
#include "../utils/string/string.h"
#include "request.h"
#include "response.h"
#include "valid.h"
// Create fct to destroy the structs below
struct request *req(char *input, struct config *conf);
struct string *http(struct request *r);

#endif /* HTTP_H */
