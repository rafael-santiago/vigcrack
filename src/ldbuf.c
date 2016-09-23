/*
 *                           Copyright (C) 2005-2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "ldbuf.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>

static char char_normalize(const char c);

static char char_normalize(const char c) {
    char rc = toupper(c);
    switch ((unsigned char)rc) {
        case 0xc0:
        case 0xc1:
        case 0xc2:
        case 0xc3:
        case 0xc4:
        case 0xc5:
        case 0xc6:
            rc = 'A';
            break;

        case 0xc7:
            rc = 'C';
            break;

        case 0xc8:
        case 0xc9:
        case 0xca:
        case 0xcb:
            rc = 'E';
            break;

        case 0xcc:
        case 0xcd:
        case 0xce:
        case 0xcf:
            rc = 'I';
            break;

        case 0xd1:
            rc = 'D';
            break;

        case 0xd2:
        case 0xd3:
        case 0xd4:
        case 0xd5:
        case 0xd6:
            rc = 'O';
            break;

        case 0xd9:
        case 0xda:
        case 0xdb:
        case 0xdc:
            rc = 'U';
            break;

        case 0xdd:
            rc = 'Y';
            break;
    }
    return rc;
}

char *ldbuf(const char *filepath, size_t *buf_size) {
    char *buf = NULL, *bp = NULL, *bp_end = NULL, *bpp = NULL;
    FILE *fp = NULL;
    long fp_size = 0;
    char c = 0;

    if (buf_size != NULL) {
        *buf_size = 0;
    }

    if (filepath == NULL) {
        return NULL;
    }

    fp = fopen(filepath, "rb");

    if (fp == NULL) {
        return NULL;
    }

    fseek(fp, 0L, SEEK_END);
    fp_size = ftell(fp);

    fseek(fp, 0L, SEEK_SET);

    buf = (char *)malloc(fp_size + 1);
    memset(buf, 0, fp_size + 1);

    bp = buf;

    do {
        *bp = fgetc(fp);
        c = char_normalize(*bp);
        if (isalpha(c)) {
            *bp = toupper(c);
            bp++;
        }
    } while (!feof(fp));

    *bp = 0;

    fclose(fp);

    if (buf_size != NULL) {
        *buf_size = strlen(buf);
    }

    return buf;
}
