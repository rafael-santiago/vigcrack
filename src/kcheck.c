/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "kcheck.h"
#include "cipher.h"
#include <string.h>
#include <stdio.h>

void rshkey(char *key, const size_t key_size) {
    size_t k = 0, nk = 1;
    char temp[0xffff] = "";
    sprintf(temp, "%s", key);
    while (k < key_size) {
        key[nk % key_size] = temp[k];
        k++;
        nk++;
    }
}

char *get_key_by_assumed_plaintext(const char *plaintext, const size_t plaintext_size, const char *ciphertext, const size_t ciphertext_size, const size_t key_size) {
    char *key = NULL, *kp = NULL, *kp_end = NULL;
    const char *p = NULL, *p_end = NULL;
    const char *cp = NULL, *cp_end = NULL;

    if (plaintext == NULL || ciphertext == NULL || key_size == 0 || plaintext_size == 0 || ciphertext_size == 0) {
        return NULL;
    }

    key = (char *)malloc(key_size + 1);

    if (key == NULL) {
        return NULL;
    }

    memset(key, 0, key_size + 1);
    memset(key, '*', key_size);

    kp = key;
    kp_end = kp + key_size;

    p = plaintext;
    p_end = p + plaintext_size;

    cp = ciphertext;
    cp_end = cp + ciphertext_size;

    while (kp != kp_end && p != p_end && cp != cp_end) {
        *kp = eval_kn(*cp, *p);
        kp++;
        p++;
        cp++;
    }

    return key;
}

char *align_assumed_key(char *key, const size_t key_size, size_t *usage_count, size_t *usage_point, const char *pattern, const size_t pattern_size, const char *ciphertext, const size_t ciphertext_size) {
    const char *p = NULL, *p_end = NULL;
    const char *cp = NULL, *cp_end = NULL;
    int found = 0;
    size_t ucount = 0, upoint = 0, u = 0;

    if (pattern == NULL || ciphertext == NULL || key == NULL || key_size == 0 || pattern_size == 0 || ciphertext_size == 0) {
        return key;
    }


    p = pattern;
    p_end = p + pattern_size;

    cp = ciphertext;
    cp_end = cp + ciphertext_size;

    while (p != p_end                   &&
           cp != cp_end                 &&
           (cp + pattern_size) < cp_end &&
           !(found = ( memcmp(cp, pattern,
                                  pattern_size ) == 0) )) {
        cp++;
    }

    if (found) {
        ucount = ((cp_end - cp) - (cp_end - ciphertext)) / key_size;
        upoint = ((cp_end - cp) - (cp_end - ciphertext)) % key_size;

        for (u = 0; u < upoint; u++) {
            rshkey(key, key_size);
        }

        if (usage_count != NULL) {
            *usage_count = ucount;
        }

        if (usage_point != NULL) {
            *usage_point = upoint;
        }

    }

    return key;
}

