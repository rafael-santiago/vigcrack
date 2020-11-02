/*
 *                                Copyright (C) 2016 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "../option.h"
#include "../cipher.h"
#include "../ldbuf.h"
#include "../kguesser.h"
#include <cutest.h>
#include <string.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#endif

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

CUTE_TEST_CASE(kguesser_tests)
    struct kprobs *kbp = NULL;
    struct found_pattern *fp = NULL;
    size_t found_patterns_nr = 0;
    char *ciphertext = "YLPHJVGIOVHWADRGVIEHRJPTUCYDSEIFMQFETUQKW"
                       "EOGNVASCJVGKTCDEUZOWXEIVXOPMGVHKUKEVNFGRU"
                       "AEJEHQNDKAMJUIIJTNFGWMLQPCQTQWODNGARVJBBI"
                       "VJDQFSQEOWDTEBXUPUKPILBWPDUVAOWVYPXHCVEXL"
                       "VUPUFQDQUXPFLNCNAVRAXHCVEXLVUPUFQDQUXPFLN"
                       "CNAVRAMOQMFQYVAGLGETKVROJNAQUTMEYFCCPIPLH"
                       "AWOVKOPAMIFCCPTGYEOFOWVOPALADOTPETSSYLEFK"
                       "NAVYNSOQOIPLZASBGNIGCIPIEOKNGCINBSUWMGZXQ"
                       "DKKPBGSMAGTJGRGPWWMIGRRQTMOFIURRQTMOFAPGY"
                       "GMSNBNGAEYLZAHOVUOOLXDJNIVOTLZABLPQOPLGWO"
                       "KPQWJVAEUFGGLUDLWUEXGRAVYZPDQPTVLPHBNAQNG"
                       "DLWUEXGRAVYZPDQPTVLPHBNAQNGDLWUEXGRAVYZPD"
                       "QPTVLPHBNAQNGDLWUEXGRAVYZPDQPTVLPHJTJKNMF"
                       "SQBLTGAFFOJPWJQWHHVEEGQPOVASOBYAQUMUSSUHG"
                       "CRVPWJUGQPEKTXWLIPIMAZSJHTQVHGNVWWEYJAVLZ"
                       "ASYQWDQKSJUTGNLCUCKOEYJAVLZASYQWDQKSJUTGN"
                       "LCUCKOEYJAVLZASYQWDQKSJUTGNLCUCKOEYJAVLZA"
                       "SYQWDQKSJUTGNL";
    size_t ciphertext_size = strlen(ciphertext);

    kbp = kguesser(ciphertext, ciphertext_size, 3, 3, 3, 20);

    CUTE_ASSERT(kbp != NULL);

    CUTE_ASSERT(strcmp(kbp->probable_keys, "3, 9, 6, 18, 4, 5, 11, 12, 15, 8, 19, 7, 10, 13, 16, 20, 14") == 0);

    CUTE_ASSERT(kbp->patterns != NULL);

    //  INFO(Santiago): I am not being so pedantic here. If we got the right "probable keys" guessing sequence
    //                  is quit probable that everything have worked fine with our "keyword sybil". Then:
    //
    //                  - Dear Ms. Pattern list show me your head!
    //                  - Hmmmm "GCI"... Now distance, "- 9!" and what about that trinket called "mult_nr"? Nice!
    //                  - You seem ok... But let's see your height... Hmmm... 225 patterns... Congrats, you are healthy!
    //
    //                   d;-) (...)

    CUTE_ASSERT(strcmp(kbp->patterns->pattern, "GCI") == 0);

    CUTE_ASSERT(kbp->patterns->distance == 9);

    CUTE_ASSERT(kbp->patterns->mult_nr == 2);

    for (fp = kbp->patterns; fp != NULL; fp = fp->next) {
        found_patterns_nr++;
    }

    CUTE_ASSERT(found_patterns_nr == 225);

    free_kprobs(kbp);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(binpoking_tests)
    char *bin_path =
#ifdef _WIN32
    "../bin/vigcrack.exe"
#else
    "../bin/vigcrack"
#endif
    ;
    char *ciphertext = "YLPHJVGIOVHWADRGVIEHRJPTUCYDSEIFMQFETUQKW"
                       "EOGNVASCJVGKTCDEUZOWXEIVXOPMGVHKUKEVNFGRU"
                       "AEJEHQNDKAMJUIIJTNFGWMLQPCQTQWODNGARVJBBI"
                       "VJDQFSQEOWDTEBXUPUKPILBWPDUVAOWVYPXHCVEXL"
                       "VUPUFQDQUXPFLNCNAVRAXHCVEXLVUPUFQDQUXPFLN"
                       "CNAVRAMOQMFQYVAGLGETKVROJNAQUTMEYFCCPIPLH"
                       "AWOVKOPAMIFCCPTGYEOFOWVOPALADOTPETSSYLEFK"
                       "NAVYNSOQOIPLZASBGNIGCIPIEOKNGCINBSUWMGZXQ"
                       "DKKPBGSMAGTJGRGPWWMIGRRQTMOFIURRQTMOFAPGY"
                       "GMSNBNGAEYLZAHOVUOOLXDJNIVOTLZABLPQOPLGWO"
                       "KPQWJVAEUFGGLUDLWUEXGRAVYZPDQPTVLPHBNAQNG"
                       "DLWUEXGRAVYZPDQPTVLPHBNAQNGDLWUEXGRAVYZPD"
                       "QPTVLPHBNAQNGDLWUEXGRAVYZPDQPTVLPHJTJKNMF"
                       "SQBLTGAFFOJPWJQWHHVEEGQPOVASOBYAQUMUSSUHG"
                       "CRVPWJUGQPEKTXWLIPIMAZSJHTQVHGNVWWEYJAVLZ"
                       "ASYQWDQKSJUTGNLCUCKOEYJAVLZASYQWDQKSJUTGN"
                       "LCUCKOEYJAVLZASYQWDQKSJUTGNLCUCKOEYJAVLZA"
                       "SYQWDQKSJUTGNL";
    size_t ciphertext_size = strlen(ciphertext);
    FILE *fp = NULL;

    printf("\n"
           "=== Hi human! I will poke the generated binary for you in order to see that everything is fine with it.\n"
           "=== Do not mind about the produced output. I will make a mess...\n"
           "===\n\n");
#ifdef _WIN32
    Sleep(3000);
#else
    sleep(3);
#endif
    char *known_modes[] = {
        "sybil", "hangman", "decrypt", "encrypt", "riddler", "brutus"
    };
    size_t known_modes_nr = sizeof(known_modes) / sizeof(known_modes[0]), k = 0;
    char cmdline[255] = "";

    CUTE_ASSERT(system(bin_path) == 0);

    //  INFO(Santiago): Nothing here can explode. The output is irrelevant.

    while (k < known_modes_nr) {
        sprintf(cmdline, "%s --help=%s", bin_path, known_modes[k]);
        CUTE_ASSERT(system(cmdline) == 0);
        k++;
    }

    sprintf(cmdline, "%s --help", bin_path);
    CUTE_ASSERT(system(cmdline) == 0);

    //  INFO(Santiago): Now let's simulate the basic software usage workflow. Again, the output is irrelevant,
    //                  however, nothing can explode.

    fp = fopen("poking-test.txt", "w");
    CUTE_ASSERT(fp != NULL);
    fprintf(fp, "%s\n", ciphertext);
    fclose(fp);

    sprintf(cmdline, "%s --sybil --file-path=poking-test.txt --min-key-length=3 "
                     "--max-key-length=10 --min-pattern-len=3 --max-pattern-len=6", bin_path);
    CUTE_ASSERT(system(cmdline) == 0);

    sprintf(cmdline, "%s --hangman --file-path=poking-test.txt --pattern=YLP "
                     "--plaintext=WEL --max-attempts=10 --key-len=9", bin_path);
    CUTE_ASSERT(system(cmdline) == 0);

    sprintf(cmdline, "%s --hangman --file-path=poking-test.txt --pattern=YLPHJVGIO "
                     "--plaintext=WELLIVEGO --max-attempts=17 --key-len=9", bin_path);
    CUTE_ASSERT(system(cmdline) == 0);

    sprintf(cmdline, "%s --riddler --plaintexts-home=. --show-keyword", bin_path);
    CUTE_ASSERT(system(cmdline) == 0);

    sprintf(cmdline, "%s --encrypt --file-path=poking-test.txt --key=chewbacca", bin_path);
    CUTE_ASSERT(system(cmdline) == 0);

    sprintf(cmdline, "%s --decrypt --file-path=poking-test.txt --key=chewbacca", bin_path);
    CUTE_ASSERT(system(cmdline) == 0);

    sprintf(cmdline, "%s --brutus --file-path=poking-test.txt --plaintexts=and --key-len=9", bin_path);
    CUTE_ASSERT(system(cmdline) == 0);

    remove("poking-test.txt");

    printf("\n\n"
           "=== Hello again,\n"
           "=== The generated software seems fine! It is good for using.\n"
           "===\n\n");

CUTE_TEST_CASE_END

CUTE_TEST_CASE(tests)
    CUTE_RUN_TEST(option_tests);
    CUTE_RUN_TEST(cipher_tests);
    CUTE_RUN_TEST(eval_kn_tests);
    CUTE_RUN_TEST(ldbuf_tests);
    CUTE_RUN_TEST(kguesser_tests);
    CUTE_RUN_TEST(binpoking_tests);
CUTE_TEST_CASE_END

CUTE_MAIN(tests)
