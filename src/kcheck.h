/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef VIGCRACK_KCHECK_H
#define VIGCRACK_KCHECK_H 1

#include <stdlib.h>

char *get_key_by_assumed_plaintext(const char *plaintext, const size_t plaintext_size, const char *ciphertext, const size_t ciphertext_size, const size_t key_size);

char *align_assumed_key(char *key, const size_t key_size, size_t *usage_count, size_t *usage_point, const char *pattern, const size_t pattern_size, const char *ciphertext, const size_t ciphertext_size);

#endif
