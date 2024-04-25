#include "http.h"

struct request *req(char *input, struct config *conf)
{
    struct request *r = parse_request(input);
    valid(r, conf);
    return r;
}
struct string *http(struct request *r)
{
    return response(r);
}
