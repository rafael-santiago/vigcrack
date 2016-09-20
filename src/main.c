#include "option.h"
#include "modes.h"

int main(int argc, char **argv) {
    set_argc_argv(argc, argv);
    if (get_bool_option("keyword-guesser", 0)) {
        return keyword_guesser();
    } else if (get_bool_option("assumption", 0)) {
        return assumption();
    } else if (get_bool_option("decrypt", 0)) {
        return decryptor();
    } else if (get_bool_option("encrypt", 0)) {
        return encryptor();
    } else if (get_bool_option("make-puzzle", 0)) {
        return mkpuzzle();
    }
    return help();
}

