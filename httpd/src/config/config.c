#define _GNU_SOURCE

#include "config.h"

#include <fnmatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void config_destroy(struct config *config)
{
    free(config->pid_file);
    free(config->log_file);
    if (config->servers != NULL)
    {
        for (size_t i = 0; i < config->nb_servers; i++)
        {
            free(config->servers[i].port);
            free(config->servers[i].ip);
            free(config->servers[i].root_dir);
            free(config->servers[i].default_file);
            string_destroy(config->servers[i].server_name);
        }
    }
    free(config->servers);
    free(config);
}
static int get_type(char *buff)
{
    if (fnmatch("log_file*", buff, 0) == 0)
        return 1;
    if (fnmatch("log*", buff, 0) == 0)
        return 2;
    if (fnmatch("pid_file*", buff, 0) == 0)
        return 3;
    if (fnmatch("server_name*", buff, 0) == 0)
        return 4;
    if (fnmatch("port*", buff, 0) == 0)
        return 5;
    if (fnmatch("ip*", buff, 0) == 0)
        return 6;
    if (fnmatch("default_file*", buff, 0) == 0)
        return 7;
    if (fnmatch("root_dir*", buff, 0) == 0)
        return 8;
    return 0;
}
static void set_type_glob(struct config *conf, int type, char *data)
{
    size_t len = strlen(data);
    switch (type)
    {
    case 1:
        conf->log_file = malloc(len);
        strncpy(conf->log_file, data, len - 1);
        conf->log_file[len - 1] = '\0';
        break;
    case 2:
        conf->log = strcmp(data, "true\n") == 0 ? true : false;
        break;
    case 3:
        conf->pid_file = malloc(len);
        strncpy(conf->pid_file, data, len - 1);
        conf->pid_file[len - 1] = '\0';
        break;
    default:
        break;
    }
}
static void set_type_host(struct server_config *serv, int type, char *data)
{
    size_t len = strlen(data);
    switch (type)
    {
    case 4:
        serv->server_name = string_create(data, len - 1);
        break;
    case 5:
        serv->port = malloc(len);
        strncpy(serv->port, data, len - 1);
        serv->port[len - 1] = '\0';
        break;
    case 6:
        serv->ip = malloc(len);
        strncpy(serv->ip, data, len - 1);
        serv->ip[len - 1] = '\0';
        break;
    case 7:
        serv->default_file = malloc(len);
        strncpy(serv->default_file, data, len - 1);
        serv->default_file[len - 1] = '\0';
        break;
    case 8:
        serv->root_dir = malloc(len);
        strncpy(serv->root_dir, data, len - 1);
        serv->root_dir[len - 1] = '\0';
        break;
    default:
        break;
    }
}
static struct server_config parse_server(FILE *fp1)
{
    char *buff;
    size_t buffsize = 0;
    struct server_config res = { .ip = NULL };
    int type;
    while (getline(&buff, &buffsize, fp1) != -1)
    {
        if (buff[0] == '\n')
            break;
        type = get_type(buff);
        set_type_host(&res, type, strchr(buff, '=') + 2);
    }
    free(buff);
    return res;
}
static int check_config(struct config *conf)
{
    if (conf->pid_file == NULL)
    {
        config_destroy(conf);
        return -1;
    }
    if (conf->servers == NULL || conf->nb_servers == 0)
    {
        config_destroy(conf);
        return -1;
    }
    for (size_t i = 0; i < conf->nb_servers; i++)
    {
        if (conf->servers[0].server_name == NULL)
        {
            config_destroy(conf);
            return -1;
        }
        if (conf->servers[0].port == NULL)
        {
            config_destroy(conf);
            return -1;
        }
        if (conf->servers[0].ip == NULL)
        {
            config_destroy(conf);
            return -1;
        }
        if (conf->servers[0].root_dir == NULL)
        {
            config_destroy(conf);
            return -1;
        }
    }
    return 0;
}
struct config *parse_configuration(const char *path)
{
    struct config *res = malloc(sizeof(struct config));
    FILE *fp1 = fopen(path, "r");
    if (fp1 == NULL)
        return NULL;
    char *buff;
    size_t buffsize = 0;
    int type;
    res->pid_file = NULL;
    res->log = true;
    res->log_file = NULL;
    getline(&buff, &buffsize, fp1);
    while (getline(&buff, &buffsize, fp1) != -1)
    {
        if (buff[0] == '\n')
            break;
        type = get_type(buff);
        set_type_glob(res, type, strchr(buff, '=') + 2);
    }
    res->nb_servers = 1;
    res->servers = NULL;
    while (getline(&buff, &buffsize, fp1) != -1)
    {
        res->servers = realloc(res->servers,
                               sizeof(struct server_config) * res->nb_servers);
        res->servers[res->nb_servers - 1] = parse_server(fp1);
        res->nb_servers++;
    }
    fclose(fp1);
    res->nb_servers--;
    free(buff);
    if (check_config(res) == -1)
        return NULL;
    return res;
}
