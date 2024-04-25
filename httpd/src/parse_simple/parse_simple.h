#ifndef PARSE_SIMPLE_H
#define PARSE_SIMPLE_H
#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "config/config.h"
#include "server/server.h"
#include "utils/string/string.h"

enum opt
{
    START,
    STOP,
    RESTART,
    DEFAULT
};
struct parse
{
    int a;
    int dry_run;
    enum opt option_a;
};

struct parse *option(int argc, char **argv);

#endif /* PARSE_SIMPLE_H */
