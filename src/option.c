#include "option.h"
#include <stdio.h>
#include <string.h>

static int g_argc = 0;

static char **g_argv = NULL;

void set_argc_argv(const int argc, char **argv) {
    g_argc = argc;
    g_argv = argv;
}

char *get_option(const char *option, char *default_value) {
    char temp[255] = "";
    int a = 0;

    sprintf(temp, "--%s=", option);

    for (a = 0; a < g_argc; a++) {
        if (strstr(g_argv[a], temp) == g_argv[a]) {
            return (g_argv[a] + strlen(temp));
        }
    }

    return default_value;
}

int get_bool_option(const char *option, int default_value) {
    char temp[255] = "";
    int a = 0;

    sprintf(temp, "--%s", option);

    for (a = 0; a < g_argc; a++) {
        if (strcmp(g_argv[a], temp) == 0) {
            return 1;
        }
    }

    return default_value;
}
