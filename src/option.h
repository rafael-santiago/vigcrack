/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef VIGCRACK_OPTION_H
#define VIGCRACK_OPTION_H 1

void set_argc_argv(const int argc, char **argv);

char *get_option(const char *option, char *default_value);

int get_bool_option(const char *option, int default_value);

#endif
