#include "../http.h"

int main(void)
{
    char *request = "GET /home/samy/oop/file1.txt HTTP/1.1\r\n"
                    "Host: localhost\r\n"
                    "Content-Length: 25\r\n\r\n";
    struct config *c = parse_configuration("../config/tests/local_test.txt");
    struct string *res = http(request, c);
    res->size++;
    return 0;
}
