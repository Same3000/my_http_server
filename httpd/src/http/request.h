#ifndef REQUEST_H
#define REQUEST_H
#include <fnmatch.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/string/string.h"
enum method
{
    GET,
    HEAD,
    OTHER
};

struct request
{
    enum method req_method;
    struct string *target;
    bool right_version;
    size_t content_length;
    struct string *host;
    int return_value; // check in reponse
    char *path;
    int is_bad_req;
};
struct request *parse_request(char *input);
void request_destroy(struct request *r);
#endif /* REQUEST_H */
