#ifndef VALID_H
#define VALID_H
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../config/config.h"
#include "request.h"

void valid(struct request *r, struct config *conf);

#endif /* VALID_H */
