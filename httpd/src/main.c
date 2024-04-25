#include <err.h>

#include "config/config.h"
#include "daemon/daemon.h"
#include "parse_simple/parse_simple.h"
#include "server/server.h"
#include "utils/string/string.h"

int main(int argc, char **argv)
{
    if (argc == 1 || argc > 5)
        errx(1, "Wrong Usage");
    struct config *conf = NULL;
    struct parse *parsed = option(argc, argv);
    conf = parse_configuration(argv[argc - 1]);
    if (conf == NULL)
        errx(2, "Config File Problem");
    if (parsed->dry_run != 1)
    {
        if (parsed->a == 1)
            return daemon(conf, parsed);
        else
            server(conf);
    }
    config_destroy(conf);
    free(parsed);
    return 0;
}
