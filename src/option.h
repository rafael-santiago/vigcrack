#ifndef VIGENERE_STANFORD_CRACKER_OPTION_H
#define VIGENERE_STANFORD_CRACKER_OPTION_H 1

void set_argc_argv(const int argc, char **argv);

char *get_option(const char *option, char *default_value);

int get_bool_option(const char *option, int default_value);

#endif
