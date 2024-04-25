#include "request.h"

static int set_method(struct request *r, char *input)
{
    if (strncmp("GET", input, 3) == 0)
    {
        r->req_method = GET;
        return 3;
    }
    if (strncmp("HEAD", input, 4) == 0)
    {
        r->req_method = HEAD;
        return 4;
    }
    size_t i = 0;
    while (input[i] != ' ')
        i++;
    r->req_method = OTHER;
    return i;
}
static int set_target(struct request *r, char *input)
{
    size_t len = 0;
    while (input[len] != ' ')
        len++;
    r->target = string_create(input, len);

    return len;
}
static int check_header(struct request *r, char *input)
{
    while (strncmp("\r\n\r\n", input, 4) != 0 && strlen(input) >= 4)
    {
        if (strncmp("Content-Length:", input, 15) == 0)
        {
            input += strlen("Content-Length: ");
            r->content_length = strtoul(input, &input, 10);
            input += 2;
        }
        else if (strncmp("Host:", input, 5) == 0)
        {
            input += strlen("Host: ");
            size_t i = 0;
            while (strncmp("\r\n", &input[i], 2) != 0)
                i++;
            r->host = string_create(input, i);
            input += i + 2;
        }
        else
            input++;
    }
    return 0;
}
struct request *parse_request(char *input) // maybe add the len if necessary
{
    struct request *r = malloc(sizeof(struct request));
    size_t i = 0;
    int offset = set_method(r, input);
    r->content_length = 0;
    r->host = NULL;
    r->target = NULL;
    i += offset + 1;
    i += set_target(r, &input[i]) + 1;

    if (strncmp("HTTP/1.1", &input[i], 8) == 0)
        r->right_version = true;
    else
        r->right_version = false;
    while (strncmp("\r\n", &input[i], 2) != 0)
    {
        i++;
    }
    i += 2;
    r->path = NULL;
    check_header(r, &input[i]);
    return r;
}
void request_destroy(struct request *r)
{
    if (r->target != NULL)
        string_destroy(r->target);
    if (r->host != NULL)
        string_destroy(r->host);
    if (r->path != NULL)
        free(r->path);
    free(r);
}
