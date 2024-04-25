#define _POSIX_C_SOURCE 200809L
#include "server.h"

static struct config *glob_conf = NULL;
static char ip[INET_ADDRSTRLEN];
static int rtn = 1;

static int create_bind(char *node, char *service)
{
    struct addrinfo hints = { 0 };
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    struct addrinfo *res = NULL;
    if (getaddrinfo(node, service, &hints, &res) != 0)
    {
        fprintf(stdout, "create_bind: failed addrinfo\n");
        return -1;
    }
    int sock = -1;
    for (struct addrinfo *p = res; p != NULL; p = p->ai_next)
    {
        sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        int yes = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        if (sock == -1)
            continue;
        if (bind(sock, p->ai_addr, p->ai_addrlen) != -1)
            break;

        close(sock);
        sock = -1;
    }

    freeaddrinfo(res);

    return sock;
}

static void respond(int client_fd, struct request *r)
{
    size_t total = 0;
    ssize_t sent = 0;
    struct string *response = http(r);
    log_request(glob_conf, r, ip);
    while (total != response->size)
    {
        sent = send(client_fd, response->data, response->size, 0);
        log_response(glob_conf, r, ip);
        if (sent == -1)
        {
            fprintf(stdout, "Send Failed\n");
            return;
        }
        total += sent;
    }
    request_destroy(r);
    string_destroy(response);
}
static int add_until(char *src, char *dst, ssize_t size, ssize_t former_size)
{
    ssize_t i = 0;
    if (size < 4)
    {
        strncpy(&dst[former_size + i], &src[i], size);
        return 0;
    }
    while (i < size)
    {
        if (i < (size - 4) && strncmp("\r\n\r\n", &src[i], 4) == 0)
        {
            strncpy(&dst[former_size + i], &src[i], 4);
            return i + 4;
        }
        strncpy(&dst[former_size + i], &src[i], 1);
        i++;
    }
    return 0;
}
static void communicate(int client_fd)
{
    ssize_t bytes = 0;
    size_t tot = 0;
    char *header = NULL;
    char buffer[BUFFER_SIZE];
    struct request *r = NULL;
    int added;
    while ((bytes = recv(client_fd, buffer, BUFFER_SIZE, 0)) > 0)
    {
        tot += bytes;
        header = realloc(header, tot);
        added = add_until(buffer, header, bytes, tot - bytes);
        if (added != 0)
        {
            header = realloc(header, (tot - bytes) + 1);
            header[tot] = '\0';
            r = req(header, glob_conf);
            break;
        }
        if (fnmatch("*\r\n\r\n*", header, 0) == 0)
        {
            header = realloc(header, tot + 1);
            header[tot] = '\0';
            r = req(header, glob_conf);
            break;
        }
        memset(buffer, 0, bytes);
    }
    free(header);
    /*char *body = NULL;
    if (added < bytes && added != 0)
    {
        tot = bytes - added;
        body = realloc(body, tot);
        strncpy(body, &buffer[added], tot);
    }
    bytes = 0;
    while ((bytes = recv(client_fd, buffer, BUFFER_SIZE, 0)) > 0 && tot <=
    r->content_length)
    {
        tot += bytes;
        body = realloc(body, tot);
        strncpy(&body[tot - bytes], buffer, bytes);
        memset(buffer, 0, bytes);
    }
    free(body);
    if (tot != r->content_length)
        r->is_bad_req = 1;*/
    respond(client_fd, r);
}
static void handler(int signum)
{
    switch (signum)
    {
    case SIGINT:
        rtn = 0;
        break;

    default:
        break;
    }
}

static void start_server(int server_socket)
{
    if (listen(server_socket, SOMAXCONN) == -1)
    {
        return;
    }
    struct sigaction sa;
    sa.sa_flags = 0; // Nothing special to do
    sa.sa_handler = handler;

    if (sigaction(SIGINT, &sa, NULL) < 0)
    {
        perror("Error: ");
        return;
    }
    while (rtn)
    {
        int client_fd = accept(server_socket, NULL, NULL);
        if (client_fd != -1)
        {
            struct sockaddr_in peeraddr_in;
            socklen_t peeraddrlen;
            void *peeraddr_void = &peeraddr_in;
            struct sockaddr *peeraddr = peeraddr_void;
            getpeername(client_fd, peeraddr, &peeraddrlen);

            inet_ntop(AF_INET, &(peeraddr_in.sin_addr), ip, INET_ADDRSTRLEN);

            communicate(client_fd);
            close(client_fd);
        }
        else
        {
            close(server_socket);
            break;
        }
    }
}

int server(struct config *conf)
{
    glob_conf = conf;
    if (conf->servers[0].ip == NULL || conf->servers[0].port == NULL)
    {
        fprintf(stdout, "Usage Wrong\n");
        return 1;
    }
    int server_socket = create_bind(conf->servers[0].ip, conf->servers[0].port);
    if (server_socket == -1)
    {
        fprintf(stdout, "create and bind failed create_bind\n");
        return 1;
    }
    start_server(server_socket);
    close(server_socket);
    return 0;
}
