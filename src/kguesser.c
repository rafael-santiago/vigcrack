/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "kguesser.h"
#include <string.h>
#include <stdio.h>

#define new_pattern(p, s, d) ( (p) = (struct found_pattern *)malloc(sizeof(struct found_pattern)),\
                         (p)->next = NULL, sprintf((p)->pattern, "%s", s), (p)->distance = d, (p)->mult_nr = 0 )

static struct found_pattern *add_pattern(struct found_pattern *list, const char *pattern, size_t distance);

static struct found_pattern *get_pattern(struct found_pattern *list, const char *pattern, size_t distance);

static struct found_pattern *pattern_tail(struct found_pattern *list);

static void del_pattern(struct found_pattern *list);

static char *hex_pattern(const unsigned char *p, const size_t p_size);

static void bubble_pattern(struct found_pattern **list);

static ssize_t get_next_greater_non_zero_pos(const size_t mult[0xffff]);

static void eval_used_key_lengths(struct kprobs **probs, const size_t min_key_length, const size_t max_key_length);

static char *pick_pattern(const char *pattern, const char *pattern_end);

static struct found_pattern *pattern_tail(struct found_pattern *list) {
    struct found_pattern *lp = NULL;
    for (lp = list; lp->next != NULL; lp = lp->next);
    return lp;
}

static struct found_pattern *get_pattern(struct found_pattern *list, const char *pattern, size_t distance) {
    struct found_pattern *lp = NULL;

    for (lp = list; lp != NULL; lp = lp->next) {
        if (strcmp(lp->pattern, pattern) == 0 && lp->distance == distance) {
            return lp;
        }
    }

    return NULL;
}

static struct found_pattern *add_pattern(struct found_pattern *list, const char *pattern, size_t distance) {
    struct found_pattern *head = list, *p;

    if (head == NULL) {
        new_pattern(head, pattern, distance);
    } else {
        p = get_pattern(head, pattern, distance);
        if (p == NULL) {
            p = pattern_tail(head);
            new_pattern(p->next, pattern, distance);
        }
    }

    return head;
}

static void del_pattern(struct found_pattern *list) {
    struct found_pattern *p, *t;
    for (t = p = list; t; p = t) {
        t = p->next;
        free(p);
    }
}

static void bubble_pattern(struct found_pattern **list) {
    struct found_pattern *curr = NULL;
    size_t distance;
    char pattern[255];
    int has_swap = 1;

    if (list == NULL) {
        return;
    }

    if (*list == NULL || (*list)->next == NULL) {
        return;
    }

    while (has_swap) {
        has_swap = 0;
        curr = *list;
        while (curr != NULL && curr->next != NULL && !has_swap) {
            if (curr->distance > curr->next->distance) {
                sprintf(pattern, "%s", curr->pattern);
                distance = curr->distance;
                curr->distance = curr->next->distance;
                sprintf(curr->pattern, "%s", curr->next->pattern);
                curr->next->distance = distance;
                sprintf(curr->next->pattern, "%s", pattern);
                has_swap = 1;
            } else {
                curr = curr->next;
            }
        }
    }
}

static char *pick_pattern(const char *pattern, const char *pattern_end) {
    static char split[255] = "";
    char *sp = NULL;

    memset(split, 0, sizeof(split));

    sp = &split[0];
    while (pattern != pattern_end) {
        *sp = *pattern;
        sp++;
        pattern++;
    }

    return &split[0];
}

struct kprobs *kguesser(const char *ciphertext, const size_t ciphertext_size,
                        const size_t min_pattern_size, const size_t max_pattern_size,
                        const size_t min_key_length, const size_t max_key_length) {
    const char *cp = NULL, *cp_end = NULL, *pt = NULL, *pt_end = NULL;
    const char *cpp = NULL;
    struct kprobs *probs = NULL;
    size_t p = 0;
    struct found_pattern *fp = NULL;

    if (ciphertext == NULL || ciphertext_size == 0) {
        return NULL;
    }

    probs = (struct kprobs *)malloc(sizeof(struct kprobs));
    probs->patterns = NULL;

    cp_end = ciphertext + ciphertext_size;

    for (p = min_pattern_size; p <= max_pattern_size; p++) {

        cp = ciphertext;

        while (cp != cp_end) {
            pt = cp;
            pt_end = pt + p;

            if (pt_end > cp_end) {
                break;
            }

            cpp = pt_end;
            while (cpp <= cp_end) {
                if (((cpp + p) <= cp_end) && (memcmp(cpp, pt, p) == 0)) {
                    probs->patterns = add_pattern(probs->patterns, pick_pattern(pt, pt_end), cpp - pt);
                }
                cpp++;
            }
            cp++;
        }
    }

    bubble_pattern(&probs->patterns);

    eval_used_key_lengths(&probs, min_key_length, max_key_length);

    return probs;
}

static void eval_used_key_lengths(struct kprobs **probs, const size_t min_key_length, const size_t max_key_length) {
    struct kprobs *p = NULL;
    size_t mult[0xffff], k;
    struct found_pattern *fp = NULL;
    ssize_t pos = 0;
    char temp[10] = "";

    if (probs == NULL) {
        return;
    }

    memset(mult, 0, sizeof(mult));

    p = *probs;

    for (k = min_key_length; k <= max_key_length; k++) {
        for (fp = p->patterns; fp != NULL; fp = fp->next) {
            if ((fp->distance % k) == 0) {
                mult[k]++;
                fp->mult_nr++;
            }
        }
    }

    pos = get_next_greater_non_zero_pos(mult);

    memset(p->probable_keys, 0, sizeof(p->probable_keys));

    while (pos != -1) {
        sprintf(temp, "%d", pos);
        strncat(p->probable_keys, temp, sizeof(p->probable_keys) - 1);
        mult[pos] = 0;
        pos = get_next_greater_non_zero_pos(mult);
        if (pos != -1) {
            strncat(p->probable_keys, ", ", sizeof(p->probable_keys) - 1);
        }
    }

}

static ssize_t get_next_greater_non_zero_pos(const size_t mult[0xffff]) {
    ssize_t pos = -1;
    size_t gr = 0;
    size_t m;
    for (m = 0; m < 0xffff; m++) {
        if (mult[m] > 0 && gr < mult[m]) {
            gr = mult[m];
            pos = m;
        }
    }
    return pos;
}

void free_kprobs(struct kprobs *kprobs) {
    del_pattern(kprobs->patterns);
}
