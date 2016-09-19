#include "option.h"
#include "ldbuf.h"
#include "kguesser.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int is_number(const char *s) {
    if (s == NULL) {
        return 0;
    }

    while (*s != 0) {
        if (!isdigit(*s)) {
            return 0;
        }
        s++;
    }

    return 1;
}

int guess_key() {
    char *filepath = NULL;
    unsigned char *ciphertext = NULL;
    size_t ciphertext_size = 0;
    struct kprobs *kprobs = NULL;
    struct found_pattern *fp = NULL;
    int min_pattern_length = 0;
    int max_pattern_length = 0;
    int min_key_length = 0;
    int max_key_length = 0;
    char *number = NULL;
    int exit_code = 1;

    filepath = get_option("file-path", NULL);

    if (filepath == NULL) {
        printf("ERROR: --file-path=<filepath> is missing.\n");
        return 1;
    }

    ciphertext = ldbuf(filepath, &ciphertext_size);

    if (ciphertext == NULL) {
        printf("ERROR: NULL file ciphertext.\n");
        return 1;
    }

    printf("INFO: %d bytes loaded from %s.\n\n", ciphertext_size, filepath);

    printf("%s\n\n", ciphertext);

    if (ciphertext == NULL) {
        printf("ERROR: NULL ciphertext.\n");
        return 1;
    }

    number = get_option("min-pattern-len", "2");
    if (!is_number(number)) {
        printf("ERROR: --min-pattern-len has invalid number.\n");
        goto __clean;
    }
    min_pattern_length = atoi(number);

    number = get_option("max-pattern-len", "20");
    if (!is_number(number)) {
        printf("ERROR: --max-pattern-len has invalid number.\n");
        goto __clean;
    }
    max_pattern_length = atoi(number);

    number = get_option("min-key-len", "2");
    if (!is_number(number)) {
        printf("ERROR: --min-key-len has invalid number.\n");
        goto __clean;
    }
    min_key_length = atoi(number);

    number = get_option("max-key-len", "20");
    if (!is_number(number)) {
        printf("ERROR: --max-key-len has invalid number.\n");
        goto __clean;
    }
    max_key_length = atoi(number);

    if (min_pattern_length < 2 || min_pattern_length > max_pattern_length) {
        printf("ERROR: your --min-pattern-len does not make sense.\n");
        goto __clean;
    }

    if (min_key_length < 2 || min_key_length > max_key_length) {
        printf("ERROR: your --min-key-len does not make sense.\n");
        goto __clean;
    }

    printf("INFO: Now guessing the key length... please wait...\n");

    kprobs = kguesser(ciphertext, ciphertext_size, min_pattern_length, max_pattern_length, min_key_length, max_key_length);

    printf("\n*** Found patterns [nearest - farthest]:\n\n");

    for (fp = kprobs->patterns; fp != NULL; fp = fp->next) {
        if (fp->mult_nr > 0) {
            printf("\tThe pattern %s was found %d letters far from another %s\n", fp->pattern, fp->distance, fp->pattern);
        }
    }

    printf("\n*** Probable key lengths [most - least]: %s.\n", kprobs->probable_keys);

    exit_code = 0;

__clean:
    free(ciphertext);

    free(kprobs);

    return exit_code;
}

int help() {
    printf("use: vigcrack --guess-key --file-path=<file path> [--min-key-len=<number> --max-key-len=<number> --min-pattern-len=<number> --max-pattern-len=<number>]\n");
    return 0;
}

static int is_xstr(const char *data) {
    if (data == NULL) {
        return 0;
    }

    while (*data != 0) {
        if (!isxdigit(*data)) {
            return 0;
        }
        data++;
    }

    return 1;
}

int assumption() {
/*    char *pattern = NULL;
    char *plaintext = NULL;
    char *filepath = NULL;
    char *key_size = NULL;
    char *max_attempts = NULL;
    unsigned char *ciphertext = NULL;
    size_t ciphertext_size = 0;
    size_t buf_size = 0;
    char *buf = NULL;

    pattern = get_option("pattern", NULL);

    if (pattern == NULL) {
        printf("ERROR: --pattern=<data> is missing.\n");
        return 1;
    }

    if (!is_xstr(pattern)) {
        printf("ERROR: --pattern has invalid data.\n");
        return 1;
    }

    plaintext = get_option("plaintext", NULL);

    if (plaintext == NULL) {
        printf("ERROR: --plaintext=<data> is missing.\n");
        return 1;
    }

    if (strlen(plaintext) != strlen(pattern) / 2) {
        printf("ERROR: --plaintext has data of wrong size.\n");
        return 1;
    }

    key_size = get_option("key-size", NULL);

    if (key_size == NULL) {
        printf("ERROR: --key-size is missing.\n");
        return 1;
    }

    if (!is_number(key_size)) {
        printf("ERROR: --key-size is invalid.\n");
        return 1;
    }

    max_attempts = get_option("max-attempts", "0");

    if (!is_number(max_attempts)) {
        printf("ERROR: --max-attempts has invalid data.\n");
        return 1;
    }

    filepath = get_option("file-path", NULL);
    if (filepath == NULL) {
        printf("ERROR: --file-path=<path> is missing.\n");
        return 1;
    }

    buf = ldbuf(filepath, &buf_size);

    if (buf == NULL) {
        printf("ERROR: when reading \"%s\".\n", filepath);
        return 1;
    }

    ciphertext = hex2byte(buf, buf_size, &ciphertext_size);

    if (ciphertext == NULL) {
        printf("ERROR: when processing the ciphertext.\n");
        free(buf);
        return 1;
    }

    free(buf);

    printf("\n*** Decryption attempts assuming that '%s' leads to '%s'...\n\n", pattern, plaintext);

    xor_ciphertext(ciphertext, ciphertext_size, pattern, plaintext, atoi(key_size), atoi(max_attempts));

    free(ciphertext);

    return 0;*/
    return 0;
}

int main(int argc, char **argv) {
    set_argc_argv(argc, argv);
    if (get_bool_option("guess-key", 0)) {
        return guess_key();
    } else if (get_bool_option("assumption", 0)) {
        return assumption();
    }
    return help();
}

