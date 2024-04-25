#include <criterion/criterion.h>
#include <criterion/new/assert.h>

#include "../http.h"

Test(valid_request, base)
{
    char *request = "GET /path/to/resource HTTP/1.1\nContent-Length: 25\nHost: "
                    "www.example.com\n";

    struct request *test = parse_request(request);
    cr_assert(test->req_method == GET, "Method is wrong");
    cr_assert(strcmp(test->target, "/path/to/resource") == 0,
              "Target is wrong");

    cr_assert(strcmp(test->host, "www.example.com") == 0, "Host Wrong");

    cr_assert(test->right_version == true, "Wrong Version Parse");
}
