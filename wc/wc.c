#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

typedef struct {
    unsigned long lines;
    unsigned long words;
    unsigned long bytes;
    unsigned long max_line;
} Count;

// Processes a single file or standard input stream
Count count_stream(FILE *fp) {
    Count c = {0, 0, 0, 0};
    int ch;
    bool in_word = false;
    int bytes = 0;

    while ((ch = fgetc(fp)) != EOF) {
        c.bytes++;
        bytes++;

        if (ch == '\n') {
            if (bytes > c.max_line){
                c.max_line = bytes - 1;
            }
            c.lines++;
            bytes = 0;
        }

        // isspace() expects an unsigned char value or EOF
        if (isspace((unsigned char)ch)) {
            in_word = false;
        // prevent counting double space as a word
        } else if (!in_word) {
            in_word = true;
            c.words++;
        }
    }

    return c;
}

int main(int argc, char *argv[]) {
    // 1. Read from standard input if no file arguments are given
    if (argc == 1) {
        Count c = count_stream(stdin);
        printf("%7lu %7lu %7lu\n", c.lines, c.words, c.bytes);
        return EXIT_SUCCESS;
    }

    Count total = {0, 0, 0, 0};
    int files_processed = 0;
    int exit_status = EXIT_SUCCESS;

    // 2. Loop through all file arguments
    for (int i = 1; i < argc; i++) {
        FILE *fp = fopen(argv[i], "rb");
        if (!fp) {
            fprintf(stderr, "wc: %s: ", argv[i]);
            perror("");
            exit_status = EXIT_FAILURE;
            continue;
        }

        Count c = count_stream(fp);
        fclose(fp);

        printf("%7lu %7lu %7lu %7lu %s\n", c.lines, c.words, c.bytes, c.max_line, argv[i]);

        total.lines += c.lines;
        total.words += c.words;
        total.bytes += c.bytes;
        files_processed++;
    }

    // 3. Print grand total if multiple files were provided
    if (argc - 1 > 1 && files_processed > 0) {
        printf("%7lu %7lu %7lu total\n", total.lines, total.words, total.bytes);
    }

    return exit_status;
}