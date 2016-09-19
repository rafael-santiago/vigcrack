#ifndef VIGCRACK_CIPHER_H
#define VIGCRACK_CIPHER_H 1

#include <stdlib.h>

char encrypt(const char k, const char p);

char decrypt(const char k, const char c);

char *encrypt_buffer(const char *key, size_t key_size, const char *plaintext, const size_t plaintext_size);

char *decrypt_buffer(const char *key, size_t key_size, const char *ciphertext, const size_t ciphertext_size);

#endif
