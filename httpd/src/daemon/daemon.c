#include "daemon.h"

static int my_pow(size_t n)
{
    int res = 1;
    for (size_t i = 0; i < n; i++)
        res *= 10;
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

static int rtn = 1;
static struct config *glob_conf = NULL;
static struct parse *glob_parsed = NULL;

void handler(int signum)
{
    switch (signum)
    {
    case SIGUSR2:
        fflush(stdout);
        break;

    case SIGINT:
        // kill
        // Stop
        rtn = 0;
        fflush(stdout);
        break;

    default:
        // Not expecting to catch this signal
        break;
    }
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

static void update_pid_file(void)
{
    pid_t pid = getpid();
    FILE *fp1 = fopen(glob_conf->pid_file, "w");
    char *str_in_file = malloc(get_int_len(pid) + 2);
    my_itoa(pid, str_in_file);
    str_in_file[get_int_len(pid)] = '\n';
    str_in_file[get_int_len(pid) + 1] = '\0';
    fwrite(str_in_file, 1, get_int_len(pid) + 1, fp1);
    free(str_in_file);
    fclose(fp1);
}
static int stop(void)
{
    FILE *fp1 = fopen(glob_conf->pid_file, "r");
    if (fp1 == NULL)
    {
        return -1;
    }

    pid_t pid = 0;
    fscanf(fp1, "%i\n", &pid);

    if (kill(pid, 0) == -1)
        return -1;
    if (kill(pid, SIGINT) == -1)
        return -1;
    fclose(fp1);
    if (remove(glob_conf->pid_file) == -1)
        return -1;
    return 0;
}
static int check_is_alive(void)
{
    FILE *fp1 = fopen(glob_conf->pid_file, "r");
    if (fp1 == NULL)
        return 0;

    pid_t pid = 0;
    fscanf(fp1, "%i\n", &pid);
    fclose(fp1);
    int killed = kill(pid, 0);
    if (killed == 0)
        return -1;
    return 0;
}
int daemon(struct config *conf, struct parse *parsed)
{
    glob_conf = conf;
    glob_parsed = parsed;
    if (parsed->option_a == STOP)
    {
        stop();
        return 0;
    }
    else if (parsed->option_a == RESTART)
        stop();
    else
    {
        if (check_is_alive() == -1)
            return 1;
    }

    struct sigaction sa;
    sa.sa_flags = 0; // Nothing special to do
    sa.sa_handler = handler;
    // Initialize mask
    pid_t pid = fork();

    if (pid == -1)
        return 1;

    if (pid > 0)
    {
        return 0;
    }

    if (sigemptyset(&sa.sa_mask) < 0)
    {
        perror("Error: ");
        return 1;
    }
    if (sigaction(SIGINT, &sa, NULL) < 0)
    {
        perror("Error: ");
        return 1;
    }
    if (sigaction(SIGUSR1, &sa, NULL) < 0)
    {
        perror("Error: ");
        return 1;
    }
    if (sigaction(SIGUSR2, &sa, NULL) < 0)
    {
        perror("Error: ");
        return 1;
    }
    if (sigaction(SIGTERM, &sa, NULL) < 0)
    {
        perror("Error: ");
        return 1;
    }
    update_pid_file();
    server(glob_conf);
    return 0;
}
