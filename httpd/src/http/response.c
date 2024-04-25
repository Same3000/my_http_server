#define _POSIX_C_SOURCE 200809L
#include "response.h"

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
static void add_reason(struct string *str, int status_code)
{
    if (status_code == 200)
    {
        string_concat_str(str, "OK", strlen("OK"));
    }
    else if (status_code == 403)
    {
        string_concat_str(str, "Forbidden", strlen("Forbidden"));
    }
    else if (status_code == 404)
    {
        string_concat_str(str, "Not Found", strlen("Not Found"));
    }
    else if (status_code == 405)
    {
        string_concat_str(str, "Method Not Allowed",
                          strlen("Method Not Allowed"));
    }
    else if (status_code == 505)
    {
        string_concat_str(str, "HTTP Version Not Supported",
                          strlen("HTTP Version Not Supported"));
    }
    else if (status_code == 400)
    {
        string_concat_str(str, "Bad Request", strlen("Bad Request"));
    }
    string_concat_str(str, "\r\n", 2);
}
static void set_time(struct string *str)
{
    time_t rawtime;
    struct tm *info;
    char *buffer = malloc(30);

    time(&rawtime);

    info = gmtime(&rawtime);

    strftime(buffer, 30, "%a, %d %b %Y %X GMT", info);
    buffer[28] = 'T';

    string_concat_str(str, "Date: ", strlen("Date: "));
    string_concat_str(str, buffer, 29);
    string_concat_str(str, "\r\n", 2);
    free(buffer);
}
static void set_content_len(struct string *str, struct request *r)
{
    string_concat_str(str, "Content-Length: ", strlen("Content-Length: "));
    if (r->return_value != 200)
    {
        string_concat_str(str, "0", 1);
        return;
    }
    char *tmp_cat = malloc(strlen(r->path) + 1);
    strcpy(tmp_cat, r->path);
    // tmp_cat[r->path->size] = '\0';
    FILE *fp1 = fopen(tmp_cat, "r");

    fseek(fp1, 0, SEEK_END);
    size_t count = ftell(fp1);

    char *file_len = malloc(get_int_len(count) + 1);
    file_len = my_itoa(count, file_len);
    file_len[get_int_len(count)] = '\0';
    string_concat_str(str, file_len, get_int_len(count));
    free(tmp_cat);
    free(file_len);
    fclose(fp1);
}
static void add_body(struct request *r, struct string *str)
{
    if (r->req_method == HEAD || r->return_value != 200)
        return;
    FILE *fp1 = fopen(r->path, "r");
    char *buffer = NULL;
    size_t bufsize = 0;

    ssize_t read = 0;

    while ((read = getline(&buffer, &bufsize, fp1)) != -1)
    {
        string_concat_str(str, buffer, read);
    }
    free(buffer);
}
struct string *response(struct request *r)
{
    struct string *str = string_create("HTTP/1.1 ", strlen("HTTP/1.1 "));
    char *tmp_status = malloc(get_int_len(r->return_value) + 1);
    tmp_status = my_itoa(r->return_value, tmp_status);
    string_concat_str(str, tmp_status, get_int_len(r->return_value));
    string_concat_str(str, " ", 1);

    add_reason(str, r->return_value);

    set_time(str);
    set_content_len(str, r);
    string_concat_str(str, "\r\n", 2);
    string_concat_str(str, "Connection: close\r\n",
                      strlen("Connection: close\r\n"));
    string_concat_str(str, "\r\n", 2);
    add_body(r, str);
    free(tmp_status);
    return str;
}
