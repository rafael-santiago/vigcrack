/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef VIGCRACK_CIPHER_H
#define VIGCRACK_CIPHER_H 1

#include <stdlib.h>

char encrypt(const char k, const char p);

char decrypt(const char k, const char c);

char *encrypt_buffer(const char *key, size_t key_size, const char *plaintext, const size_t plaintext_size);

char *decrypt_buffer(const char *key, size_t key_size, const char *ciphertext, const size_t ciphertext_size);

char eval_kn(const char m, const char c);

#endif
