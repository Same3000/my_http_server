#include "parse_simple.h"

struct parse *option(int argc, char **argv)
{
    struct parse *parsing = malloc(sizeof(struct parse));
    parsing->a = 0;
    parsing->dry_run = 0;
    parsing->option_a = DEFAULT;
    for (int i = 1; i < argc - 1; i++)
    {
        if (strcmp(argv[i], "--dry-run") == 0)
        {
            if (parsing->dry_run == 1)
                errx(1, "Wrong Input to httpd");
            parsing->dry_run = 1;
            continue;
        }
        if (strcmp(argv[i], "-a") == 0)
        {
            if (parsing->a == 1 || parsing->dry_run == 1)
                errx(1, "Wrong Input to httpd");
            parsing->a = 1;
            i++;
            if (i > argc)
                errx(1, "Wrong Input to httpd");

            if (strcmp(argv[i], "start") == 0)
                parsing->option_a = START;

            else if (strcmp(argv[i], "stop") == 0)
                parsing->option_a = STOP;

            else if (strcmp(argv[i], "restart") == 0)
                parsing->option_a = RESTART;
            else
                errx(1, "Wrong Input to httpd");
        }
    }
    return parsing;
}
