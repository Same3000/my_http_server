#include "logger.h"

static int my_pow(size_t n)
{
    int res = 1;
    for (size_t i = 0; i < n; i++)
    {
        res *= 10;
    }
    return res;
}

static char *my_itoa(int value, char *s)
{
    if (value == 0)
    {
        s[0] = '0';
        s[1] = '\0';
        return s;
    }
    size_t i = 0;
    if (value < 0)
    {
        s[i] = '-';
        i++;
        value *= -1;
    }
    int tmp = value;
    size_t len = 0;
    while (tmp != 0)
    {
        len++;
        tmp /= 10;
    }
    int p = my_pow(len - 1);
    while (value != 0 || len > 0)
    {
        len--;
        s[i] = (value / p) + '0';
        i++;
        value %= p;
        p = p / 10;
    }
    s[i] = '\0';
    return s;
}

static int get_int_len(int value)
{
    int l = 1;
    while (value > 9)
    {
        l++;
        value /= 10;
    }
    return l;
}
static void set_time(FILE *file)
{
    time_t rawtime;
    struct tm *info;
    char *buffer = malloc(30);

    time(&rawtime);

    info = gmtime(&rawtime);

    strftime(buffer, 30, "%a, %d %b %Y %X GMT", info);
    buffer[28] = 'T';

    fwrite("Date: ", 1, strlen("Date: "), file);
    fwrite(buffer, 1, 29, file);
    free(buffer);
}
static void set_server_name(struct config *conf, FILE *file)
{
    fwrite("[", 1, 1, file);
    for (size_t i = 0; i < conf->servers[0].server_name->size; i++)
    {
        fwrite(&conf->servers[0].server_name->data[i], 1, 1, file);
    }
    fwrite("]", 1, 1, file);
}
static void set_target(struct request *r, FILE *file)
{
    fwrite(" '", 1, 2, file);
    for (size_t i = 0; i < r->target->size; i++)
    {
        fwrite(&r->target->data[i], 1, 1, file);
    }
    fwrite("'", 1, 1, file);
}
static void set_request_type(struct request *r, FILE *file)
{
    if (r->req_method == GET)
    {
        fwrite("GET", 1, 3, file);
        set_target(r, file);
    }
    else if (r->req_method == HEAD)
    {
        fwrite("HEAD", 1, 4, file);
        set_target(r, file);
    }
    else
    {
        fwrite("Bad Request", 1, 11, file);
    }
}
static void set_ip_client(FILE *file, char *ip_client)
{
    fwrite(ip_client, 1, strlen(ip_client), file);
}
void log_request(struct config *conf, struct request *r, char *ip_client)
{
    if (conf->log == 0)
        return;
    FILE *file = fopen(conf->log_file, "a");
    if (file == NULL)
    {
        fprintf(stderr, "Error while opening the log_file\n");
        return;
    }
    set_time(file);
    fwrite(" ", 1, 1, file);
    set_server_name(conf, file);
    fwrite(" received ", 1, 10, file);
    if (r->return_value != 400)
        set_request_type(r, file);

    else
        fwrite("Bad Request", 1, strlen("Bad Request"), file);
    fwrite(" from ", 1, 6, file);
    set_ip_client(file, ip_client);
    fwrite("\n", 1, 1, file);
    fclose(file);
}
static void set_status_code(struct request *r, FILE *file)
{
    char *file_len = malloc(get_int_len(r->return_value) + 1);
    file_len = my_itoa(r->return_value, file_len);
    fwrite(file_len, 1, get_int_len(r->return_value), file);
    free(file_len);
}
void log_response(struct config *conf, struct request *r, char *ip_client)
{
    if (conf->log == 0)
        return;
    FILE *file = fopen(conf->log_file, "a");
    if (file == NULL)
    {
        fprintf(stderr, "Error while opening the log_file\n");
        return;
    }
    set_time(file);
    fwrite(" ", 1, 1, file);
    set_server_name(conf, file);
    fwrite(" responding with ", 1, strlen(" responding with "), file);
    set_status_code(r, file);
    fwrite(" to ", 1, 4, file);
    set_ip_client(file, ip_client);
    if (r->return_value != 400)
    {
        fwrite(" for ", 1, 5, file);
        if (r->req_method == GET)
            fwrite("GET", 1, 3, file);

        else if (r->req_method == HEAD)
            fwrite("HEAD", 1, 4, file);

        else
            fwrite("UNKNOWN", 1, 7, file);
        fwrite(" on", 1, 3, file);
        set_target(r, file);
    }
    fwrite("\n", 1, 1, file);
    fclose(file);
}
