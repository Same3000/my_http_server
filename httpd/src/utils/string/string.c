#include "string.h"

#include <stdlib.h>
#include <string.h>
struct string *string_create(const char *str, size_t size)
{
    struct string *res = malloc(sizeof(struct string));
    res->size = size;
    res->data = malloc(res->size);

    size_t i = 0;
    while (i < size)
    {
        res->data[i] = str[i];
        i++;
    }
    return res;
}

int string_compare_n_str(const struct string *str1, const char *str2, size_t n)
{
    if (n > str1->size)
        return -1;
    size_t i = 0;
    while (i < n)
    {
        if (str1->data[i] != str2[i])
            return 1;
        i++;
    }
    return 0;
}
int string_compare_two_str(const struct string *str1, const struct string *str2)
{
    if (str1->size > str2->size)
        return 1;
    if (str1->size < str2->size)
        return -1;
    size_t i = 0;
    while (i < str1->size)
    {
        if (str1->data[i] != str2->data[i])
            return 1;
        i++;
    }
    return 0;
}
void string_concat_str(struct string *str, const char *to_concat, size_t size)
{
    str->data = realloc(str->data, str->size + size);
    size_t i = 0;
    while (i < size)
    {
        str->data[str->size + i] = to_concat[i];
        i++;
    }
    str->size += size;
}
void string_destroy(struct string *str)
{
    if (str == NULL)
        return;
    free(str->data);
    free(str);
}
