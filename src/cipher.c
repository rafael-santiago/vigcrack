#include "cipher.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static char g_vigenere_alpha[26] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
};

typedef char (*vigF)(const char k, const char c);

#define get_letter_value(l) ( ( ( toupper(l) ) - 'A' ) )

#define vigenere_cipher(k, p) ( g_vigenere_alpha[(get_letter_value(p) + get_letter_value(k)) % 26] )

#define vigenere_cipher_inv(k, c) ( g_vigenere_alpha[((get_letter_value(c) - get_letter_value(k)) < 0 ? 26 : 0) +\
                                                         (get_letter_value(c) - get_letter_value(k))] )

static char *process_buffer(const char *key, size_t key_size, vigF F, const char *buffer, const size_t buffer_size);

char encrypt(const char k, const char p) {
    return vigenere_cipher(k, p);
}

char decrypt(const char k, const char c) {
    return (k != '*') ? tolower(vigenere_cipher_inv(k, c)) : toupper(c);
}

static char *process_buffer(const char *key, size_t key_size, vigF F, const char *buffer, const size_t buffer_size) {
    char *resbuf = NULL, *rp = NULL;
    const char *bp = NULL, *bp_end = NULL;
    size_t k = 0;

    resbuf = (char *)malloc(buffer_size);

    if (resbuf == NULL) {
        return NULL;
    }

    memset(resbuf, 0, buffer_size);

    rp = resbuf;
    bp = buffer;
    bp_end = buffer + buffer_size;

    while (bp != bp_end) {
        *rp = F(key[k], *bp);
        k = (k + 1) % key_size;
        rp++;
        bp++;
    }

    return resbuf;
}

char *encrypt_buffer(const char *key, size_t key_size, const char *plaintext, const size_t plaintext_size) {
    return process_buffer(key, key_size, encrypt, plaintext, plaintext_size);
}

char *decrypt_buffer(const char *key, size_t key_size, const char *ciphertext, const size_t ciphertext_size) {
    return process_buffer(key, key_size, decrypt, ciphertext, ciphertext_size);
}
