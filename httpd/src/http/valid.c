#include "valid.h"

char *requested_file_path(struct request *r, struct config *c)
{
    if (r->target->data[0] == '/' && r->target->size == 1)
    {
        char *def_file = NULL;
        if (c->servers[0].default_file == NULL)
            def_file = "index.html";
        else
            def_file = c->servers[0].default_file;
        char *tmp_cat =
            malloc(strlen(c->servers[0].root_dir) + strlen(def_file) + 2);

        strcpy(tmp_cat, c->servers[0].root_dir);

        if (tmp_cat[strlen(c->servers[0].root_dir) - 1] != '/')
            strcat(tmp_cat, "/");

        if (def_file[0] == '/')
            strcat(tmp_cat, &def_file[1]);
        else
            strcat(tmp_cat, def_file);
        r->path = malloc(strlen(c->servers[0].root_dir) + strlen(def_file) + 1);
        r->path = strcpy(r->path, tmp_cat);
        return tmp_cat;
    }

    char *tmp_cat =
        malloc(strlen(c->servers[0].root_dir) + r->target->size + 1);

    strcpy(tmp_cat, c->servers[0].root_dir);

    if (tmp_cat[strlen(c->servers[0].root_dir) - 1] != '/')
        strcat(tmp_cat, "/");

    if (r->target->data[0] == '/')
        strncat(tmp_cat, &r->target->data[1], r->target->size - 1);
    else
        strncat(tmp_cat, r->target->data, r->target->size);

    r->path = malloc(strlen(c->servers[0].root_dir) + r->target->size + 1);
    r->path = strcpy(r->path, tmp_cat);
    return tmp_cat;
}
static void check_ip(struct request *r, struct config *c, size_t len_c_ip)
{
    if (len_c_ip == r->host->size)
    {
        r->is_bad_req = 0;
        return;
    }
    else if (strncmp(&r->host->data[len_c_ip + 1], c->servers[0].port,
                     r->host->size - (len_c_ip + 1))
             == 0)
    {
        r->is_bad_req = 0;
        return;
    }
    else
        r->is_bad_req = 1;
}
static void check_host(struct request *r, struct config *c)
{
    size_t len_c_ip = strlen(c->servers[0].ip);
    if (string_compare_n_str(r->host, c->servers[0].ip, len_c_ip) == 0)
    {
        check_ip(r, c, len_c_ip);
        return;
    }
    if (string_compare_two_str(r->host, c->servers[0].server_name) == 0)
    {
        r->is_bad_req = 0;
        return;
    }
}
void check_localhost(struct request *r, struct config *c)
{
    size_t len_c_ip = strlen(c->servers[0].ip);
    if (string_compare_n_str(r->host, "localhost", strlen("localhost")) == 0
        || string_compare_n_str(r->host, "127.0.0.1", strlen("127.0.0.1")) == 0)
    {
        if (strcmp(c->servers[0].ip, "localhost") == 0
            || strcmp(c->servers[0].ip, "127.0.0.1") == 0)
        {
            check_ip(r, c, len_c_ip);
            return;
        }
    }
}
void valid(struct request *r, struct config *c)
{
    r->is_bad_req = 1;
    if (r->host != NULL)
    {
        check_localhost(r, c);
        check_host(r, c);
    }
    if (r->is_bad_req == 1)
    {
        r->return_value = 400;
        return;
    }
    else
    {
        r->return_value = 200;
    }

    if (r->right_version == false)
    {
        r->return_value = 505;
        return;
    }
    if (r->req_method == OTHER)
    {
        r->return_value = 405;
        return;
    }
    char *tmp_cat = requested_file_path(r, c);
    FILE *fp1 = fopen(tmp_cat, "r");
    free(tmp_cat);
    if (fp1 == NULL)
    {
        if (errno == EACCES)
            r->return_value = 403;
        else if (errno == ENOENT)
            r->return_value = 404;
        return;
    }
    fclose(fp1);
}
