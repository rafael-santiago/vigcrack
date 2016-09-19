#ifndef VIGENERE_STANFORD_CRACKER_KGUESSER_H
#define VIGENERE_STANFORD_CRACKER_KGUESSER_H 1

#include <stdlib.h>

struct found_pattern {
    char pattern[255];
    size_t distance;
    size_t mult_nr;
    struct found_pattern *last, *next;
};

struct kprobs {
    char probable_keys[0xffff];
    struct found_pattern *patterns;
};

struct kprobs *kguesser(const unsigned char *ciphertext, const size_t ciphertext_size,
                        const size_t min_pattern_size, const size_t max_pattern_size,
                        const size_t min_key_length, const size_t max_key_length);

void free_kprobs(struct kprobs *kprobs);

#endif
