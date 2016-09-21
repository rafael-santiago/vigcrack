#include "../option.h"
#include "../cipher.h"
#include "../ldbuf.h"
#include <cutest.h>
#include <string.h>
#include <stdlib.h>

CUTE_TEST_CASE(option_tests)
    char *argv[] = {
        "--test=foo",
        "--teste=bar",
        "--redbool"
    };
    int argc = 3;
    char *data = NULL;

    set_argc_argv(argc, argv);

    data = get_option("test", NULL);
    CUTE_ASSERT(data != NULL);
    CUTE_ASSERT(strcmp(data, "foo") == 0);

    data = get_option("teste", NULL);
    CUTE_ASSERT(data != NULL);
    CUTE_ASSERT(strcmp(data, "bar") == 0);

    data = get_option("redbool", NULL);
    CUTE_ASSERT(data == NULL);

    CUTE_ASSERT(get_bool_option("redbool", 0) == 1);

    CUTE_ASSERT(get_bool_option("redbull", 1) == 1);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(cipher_tests)
    struct test_vector_ctx {
        char *plaintext;
        char *keyword;
        char *ciphertext;
    };
    struct test_vector_ctx test_vector[] = {
        {    "diverttroopstoeastridge", "white",  "ZPDXVPAZHSLZBHIWZBKMZNM" },
        {   "thesunandthemaninthemoon",  "king", "DPRYEVNTNBUKWIAOXBUKWWBT" }
    };
    size_t test_vector_nr = sizeof(test_vector) / sizeof(test_vector[0]), t = 0;
    char *ciphertext = NULL;
    char *plaintext = NULL;

    for (t = 0; t < test_vector_nr; t++) {
        ciphertext = encrypt_buffer(test_vector[t].keyword, strlen(test_vector[t].keyword),
                                    test_vector[t].plaintext, strlen(test_vector[t].plaintext));

        CUTE_ASSERT(ciphertext != NULL);
        CUTE_ASSERT(strcmp(ciphertext, test_vector[t].ciphertext) == 0);

        plaintext = decrypt_buffer(test_vector[t].keyword, strlen(test_vector[t].keyword),
                                   ciphertext, strlen(ciphertext));

        CUTE_ASSERT(plaintext != NULL);
        CUTE_ASSERT(strcmp(plaintext, test_vector[t].plaintext) == 0);

        free(ciphertext);
        free(plaintext);
    }
CUTE_TEST_CASE_END

CUTE_TEST_CASE(eval_kn_tests)
    CUTE_ASSERT(eval_kn('Z', 'D') == 'W');
    CUTE_ASSERT(eval_kn('p', 'I') == 'H');
    CUTE_ASSERT(eval_kn('d', 'v') == 'I');
    CUTE_ASSERT(eval_kn('X', 'E') == 'T');
    CUTE_ASSERT(eval_kn('V', 'R') == 'E');

    CUTE_ASSERT(eval_kn('D', 'T') == 'K');
    CUTE_ASSERT(eval_kn('P', 'H') == 'I');
    CUTE_ASSERT(eval_kn('R', 'E') == 'N');
    CUTE_ASSERT(eval_kn('Y', 'S') == 'G');
CUTE_TEST_CASE_END

CUTE_TEST_CASE(ldbuf_tests)
    char *buf = "ÄbcdÉfg h i j k l m n Ö     p \n\n\n\n\n\nq\r\r\rrst Ü v w x yz.";
    char *rdbuf = NULL;
    size_t rdbuf_size = 0;
    FILE *fp = fopen("ldbuf-test.txt", "w");
    CUTE_ASSERT(fp != NULL);
    fprintf(fp, "%s", buf);
    fclose(fp);
    rdbuf = ldbuf("ldbuf-test.txt", &rdbuf_size);
    remove("ldbuf-test.txt");
    CUTE_ASSERT(rdbuf != NULL);
    CUTE_ASSERT(rdbuf_size == 26);
    CUTE_ASSERT(strcmp(rdbuf, "ABCDEFGHIJKLMNOPQRSTUVWXYZ") == 0);
    free(rdbuf);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(tests)
    CUTE_RUN_TEST(option_tests);
    CUTE_RUN_TEST(cipher_tests);
    CUTE_RUN_TEST(eval_kn_tests);
    CUTE_RUN_TEST(ldbuf_tests);
CUTE_TEST_CASE_END

CUTE_MAIN(tests)
