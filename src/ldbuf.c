#include "ldbuf.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>

char *ldbuf(const char *filepath, size_t *buf_size) {
    char *buf = NULL, *bp = NULL, *bp_end = NULL, *bpp = NULL;
    FILE *fp = NULL;
    long fp_size = 0;

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
        if (isalpha(*bp)) {
            *bp = toupper(*bp);
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
