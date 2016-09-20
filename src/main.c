#include "option.h"
#include "ldbuf.h"
#include "kguesser.h"
#include "cipher.h"
#include "kcheck.h"
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
    char *pattern = NULL;
    size_t pattern_size = 0;
    char *plaintext = NULL;
    size_t plaintext_size = 0;
    char *filepath = NULL;
    size_t key_size = 0, key_usage_count = 0, key_usage_point = 0, k = 0;
    int max_attempts = 0, m = 0;
    unsigned char *ciphertext = NULL, *cp = NULL, *cp_end = NULL;
    size_t ciphertext_size = 0;
    char *key = NULL;

    pattern = get_option("pattern", NULL);

    if (pattern == NULL) {
        printf("ERROR: --pattern=<data> is missing.\n");
        return 1;
    }

    pattern_size = strlen(pattern);

    plaintext = get_option("plaintext", NULL);

    if (plaintext == NULL) {
        printf("ERROR: --plaintext=<data> is missing.\n");
        return 1;
    }

    plaintext_size = strlen(plaintext);

    if (plaintext_size != pattern_size) {
        printf("ERROR: --plaintext has data of wrong size.\n");
        return 1;
    }

    key_size = atoi(get_option("key-size", "0"));

    if (key_size == 0) {
        printf("ERROR: --key-size is missing.\n");
        return 1;
    }

    max_attempts = atoi(get_option("max-attempts", "0"));

    if (max_attempts < 0) {
        printf("ERROR: --max-attempts has invalid data.\n");
        return 1;
    }

    filepath = get_option("file-path", NULL);
    if (filepath == NULL) {
        printf("ERROR: --file-path=<path> is missing.\n");
        return 1;
    }

    ciphertext = ldbuf(filepath, &ciphertext_size);

    if (ciphertext == NULL) {
        printf("ERROR: when reading \"%s\".\n", filepath);
        return 1;
    }

    key = get_key_by_assumed_plaintext(pattern, pattern_size, plaintext, plaintext_size, key_size);

    key = align_assumed_key(key,
                            key_size,
                            &key_usage_count,
                            &key_usage_point,
                            pattern,
                            pattern_size,
                            ciphertext,
                            ciphertext_size);

    printf("\n*** The letter at position #%d was used to encipher it. "
           "It was used '%d' times. The effective key is '%s'.\n", key_usage_point, key_usage_count, key);

    printf("\n*** Decryption attempts assuming that '%s' leads to '%s'...\n\n", pattern, plaintext);

    cp = ciphertext;
    cp_end = cp + ciphertext_size;

    printf("\t%1s\t%1s\t%1s\n\t__________________\n", "K", "C", "P");

    if (max_attempts == 0) {
        max_attempts = ciphertext_size;
    }

    while (cp != cp_end && m < max_attempts) {
        printf("\t%c\t%c\t%c\n", key[k], *cp, decrypt(key[k], *cp));
        k = (k + 1) % key_size;
        cp++;
        m++;
    }

    printf("\t__________________\n");

    free(key);
    free(ciphertext);

    return 0;
}

int decryptor() {
    char *filepath = NULL;
    char *ciphertext = NULL;
    char *key = NULL;
    size_t ciphertext_size = 0;
    size_t key_size = 0;
    char *plaintext = NULL;

    filepath = get_option("file-path", NULL);
    if (filepath == NULL) {
        printf("ERROR: --file-path=<path> is missing.\n");
        return 1;
    }

    key = get_option("key", NULL);
    if (key == NULL) {
        printf("ERROR: --key=<data> is missing.\n");
        return 1;
    }
    key_size = strlen(key);

    ciphertext = ldbuf(filepath, &ciphertext_size);

    if (ciphertext == NULL) {
        printf("WARNING: NULL ciphertext.\n");
        return 1;
    }

    plaintext = decrypt_buffer(key, key_size, ciphertext, ciphertext_size);

    if (plaintext == NULL) {
        printf("WARNING: NULL plaintext.\n");
    } else {
        printf("%s\n", plaintext);
        free(plaintext);
    }

    free(ciphertext);

    return 0;
}

int encryptor() {
    char *filepath = NULL;
    char *plaintext = NULL;
    char *key = NULL;
    size_t plaintext_size = 0;
    size_t key_size = 0;
    char *ciphertext = NULL;

    filepath = get_option("file-path", NULL);
    if (filepath == NULL) {
        printf("ERROR: --file-path=<path> is missing.\n");
        return 1;
    }

    key = get_option("key", NULL);
    if (key == NULL) {
        printf("ERROR: --key=<data> is missing.\n");
        return 1;
    }
    key_size = strlen(key);

    plaintext = ldbuf(filepath, &plaintext_size);

    if (plaintext == NULL) {
        printf("WARNING: NULL plaintext.\n");
        return 1;
    }

    ciphertext = encrypt_buffer(key, key_size, plaintext, plaintext_size);

    if (ciphertext == NULL) {
        printf("WARNING: NULL ciphertext.\n");
    } else {
        printf("%s\n", ciphertext);
        free(ciphertext);
    }

    free(plaintext);

    return 0;
}

int new_puzzle() {
    return 0;
}

int main(int argc, char **argv) {
    set_argc_argv(argc, argv);
    if (get_bool_option("guess-key", 0)) {
        return guess_key();
    } else if (get_bool_option("assumption", 0)) {
        return assumption();
    } else if (get_bool_option("decrypt", 0)) {
        return decryptor();
    } else if (get_bool_option("encrypt", 0)) {
        return encryptor();
    } else if (get_bool_option("new-puzzle", 0)) {
        return new_puzzle();
    }
    return help();
}

