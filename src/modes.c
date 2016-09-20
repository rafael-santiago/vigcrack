#include "ldbuf.h"
#include "option.h"
#include "kguesser.h"
#include "cipher.h"
#include "kcheck.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

typedef void (*help_detailing)();

static void keyword_guesser_details();

static void assumption_details();

static void decrypt_details();

static void encrypt_details();

static void make_puzzle_details();

static int is_number(const char *number);

static int is_number(const char *number) {
    const char *np = number;
    while (*np != 0) {
        if (!isdigit(*np)) {
            return 0;
        }
    }
    return 1;
}

static void keyword_guesser_details() {
    printf("use: vigcrack --keyword-guesser --filepath=<path> [--min-pattern-len=<len> "
           "--max-pattern-len=<len> --min-key-len=<len> --max-key-len=<len>\n");
}

int keyword_guesser() {
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
        printf("ERROR: --file-path=<path> is missing.\n");
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
    struct see_also {
        const char *mode;
        help_detailing details;
    };
    struct see_also see_also[] = {
        { "keyword-guesser", keyword_guesser_details },
        {      "assumption", assumption_details      },
        {         "decrypt", decrypt_details         },
        {         "encrypt", encrypt_details         },
        {     "make-puzzle", make_puzzle_details     }
    };
    size_t see_also_nr = sizeof(see_also) / sizeof(see_also[0]), s = 0;
    char *mode = get_option("help", NULL);

    if (mode != NULL) {
        while (s < see_also_nr) {
            if (strcmp(see_also[s].mode, mode) == 0) {
                see_also[s].details();
                return 0;
            }
            s++;
        }
    }

    printf("use: vigcrack ");
    for (s = 0; s < see_also_nr; s++) {
        printf("[--%s]%s", see_also[s].mode, ((s+1) != see_also_nr) ? " | " : "\n\n");
    }
    printf("*** Try to use: vigcrack help=<mode>, in order to know more about a given mode.\n");

    return 0;
}

static void assumption_details() {
    printf("use: vigcrack --assumption --pattern=<data> --plaintext=<data> --key-len=<len> "
           "--file-path=<path> [--max-attempts=<amount>]\n");
}

int assumption() {
    char *pattern = NULL;
    size_t pattern_size = 0;
    char *plaintext = NULL;
    size_t plaintext_size = 0;
    char *filepath = NULL;
    size_t key_len = 0, key_usage_count = 0, key_usage_point = 0, k = 0;
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

    key_len = atoi(get_option("key-len", "0"));

    if (key_len == 0) {
        printf("ERROR: --key-len is missing.\n");
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

    key = get_key_by_assumed_plaintext(pattern, pattern_size, plaintext, plaintext_size, key_len);

    key = align_assumed_key(key,
                            key_len,
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
        k = (k + 1) % key_len;
        cp++;
        m++;
    }

    printf("\t__________________\n");

    free(key);
    free(ciphertext);

    return 0;
}

void decrypt_details() {
    printf("use: vigcrack --decrypt --file-path=<path> --key=<data>\n");
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

static void encrypt_details() {
    printf("use: vigcrack --encrypt --file-path=<path> --key=<data>\n");
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

static char *get_random_keyword(const size_t min_len, const size_t max_len) {
    size_t len = rand() % max_len;
    char alpha[26] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
                       'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };

    static char keyword[255] = "";
    size_t k = 0;

    if (len < min_len) {
        len = rand() % 2 ? min_len : max_len;
    }

    memset(keyword, 0, sizeof(keyword));

    for (k = 0; k < len; k++) {
        keyword[k] = alpha[rand() % 26];
    }

    return &keyword[0];
}

static void make_puzzle_details() {
    printf("use: vigcrack --make-puzzle [--min-key-len=<len> --max-key-len=<len> "
           "--plaintext-home=<directory-path> --show-keyword "
           "--save-keyword=<file-path> --save-as=<file-path>]\n");
}

int mkpuzzle() {
    char *plaintexts_home = NULL;
    char *language = NULL;
    DIR *plaintexts_dir = NULL;
    struct dirent *de = NULL;
    char plaintexts[1000][4096];
    size_t plaintexts_nr = 0;
    size_t min_key_len = 0, max_key_len = 0;
    char *keyword = 0;
    size_t keyword_size = 0;
    char *ciphertext = NULL;
    char *plaintext = NULL;
    size_t plaintext_size = 0;
    char *save_as = NULL;
    FILE *fp = NULL;

    srand(time(0));

    min_key_len = atoi(get_option("min-key-len", "2"));

    max_key_len = atoi(get_option("max-key-len", "20"));

    if (max_key_len > 200) {
        printf("ERROR: the keyword can not be too long.\n");
        return 1;
    }

    plaintexts_home = get_option("plaintexts-home", "plaintexts");

    plaintexts_dir = opendir(plaintexts_home);

    if (plaintexts_dir == NULL) {
        printf("ERROR: unable to access the directory \"%s\".\n", plaintexts_home);
        return 1;
    }

    while ((de = readdir(plaintexts_dir)) && plaintexts_nr < 1000) {
        if (strstr(de->d_name, ".txt") == &de->d_name[strlen(de->d_name) - 4]) {
            strncpy(plaintexts[plaintexts_nr], plaintexts_home, 4095);
            if (plaintexts[plaintexts_nr][strlen(plaintexts[plaintexts_nr])-1] != '/') {
                strncat(plaintexts[plaintexts_nr], "/", 4095);
            }
            strncat(plaintexts[plaintexts_nr], de->d_name, 4095);
            plaintexts_nr++;
        }
    }

    closedir(plaintexts_dir);

    keyword = get_random_keyword(min_key_len, max_key_len);
    keyword_size = strlen(keyword);

    if (get_bool_option("show-keyword", 0)) {
        printf("The keyword is = %s\n", keyword);
    }

    save_as = get_option("save-keyword", NULL);
    if (save_as != NULL) {
        fp = fopen(save_as, "wb");
        if (fp == NULL) {
            printf("ERROR: unable to write to \"%s\".\n", save_as);
            return 1;
        } else {
            fprintf(fp, "%s\n", keyword);
            fclose(fp);
        }
    }

    plaintext = ldbuf(plaintexts[rand() % plaintexts_nr], &plaintext_size);

    if (plaintext == NULL) {
        printf("ERROR: NULL plaintext.\n");
        return 1;
    }

    ciphertext = encrypt_buffer(keyword, keyword_size, plaintext, plaintext_size);

    free(plaintext);

    if (ciphertext == NULL) {
        printf("ERROR: NULL ciphertext.\n");
        return 1;
    }

    save_as = get_option("save-as", NULL);
    if (save_as == NULL) {
        printf("%s\n", ciphertext);
    } else {
        fp = fopen(save_as, "wb");
        if (fp == NULL) {
            printf("WARNING: unable to write to \"%s\", using the stdout instead.\n%s", save_as, ciphertext);
        } else {
            fprintf(fp, "%s\n", ciphertext);
            fclose(fp);
        }
    }

    free(ciphertext);

    return 0;
}
