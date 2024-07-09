#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEFFLAGSIZE 100
#define FLAGCOUNT 4

#define HELPTEXT \
    "Wordy is a tool for simple csv parsing.\n\n\
Usage:\n\
    wordy <file_path> <args>\n\
Flags:\n\
    -h  help\n\
    -d  delimiter for the file, defaults to \\n, one char only\n\
    -i  parse those that include letters 'abc'\n\
    -p  parse those that match a pattern '#a##b' with # being wildcard\n"

#define error_message(message)      \
    fprintf(stderr, "%s", message); \
    exit(1);

#define user_error_message() \
    error_message("Invalid usage of Wordy. Use -h flag for help.\n")
#define memory_error_message() error_message("Error system is out of memory.\n")

typedef char *String;

enum flags { FILE_NAME, DELIMITER, INCLUDE, PATTERN };

String parse_arg(String);
void parse_file(String *);
void filter_line(String *, String);

String parse_arg(String arg) {
    String res = (String)malloc(strlen(arg) + 1);
    if (!res) {
        memory_error_message();
    }

    strncpy(res, arg, strlen(arg));
    res[strlen(arg)] = '\0';

    return res;
}

void parse_file(String *flags) {
    FILE *file = fopen(flags[FILE_NAME], "r");
    String line = malloc(sizeof(char) * DEFFLAGSIZE);
    int i = 0, max_size = DEFFLAGSIZE;
    char c;

    if (file == NULL) {
        error_message("Unable to open file");
        exit(1);
    }

    while ((c = getc(file)) != EOF) {
        if (i == max_size) {
            max_size *= 2;
            line = realloc(line, max_size);
        } else if (c == flags[DELIMITER][0]) {
            line[i] = '\0';
            filter_line(flags, line);
            i = 0;
        } else {
            line[i++] = c;
        }
    }

    free(line);
    fclose(file);
}

void filter_line(String *flags, String line) {
    if (line[0] == 't') {
        printf("%s\n", line);
    }
}

int main(int argc, String argv[]) {
    String flags[FLAGCOUNT] = {0};
    String arg;

    if (argc % 2 != 0 || --argc < 1) {
        user_error_message();
    }
    arg = *++argv;

    if (strcmp(arg, "-h") == 0) {
        printf("%s", HELPTEXT);
        exit(0);
    } else if (access(arg, F_OK) != 0) {
        user_error_message();
    }

    flags[FILE_NAME] = (String)malloc(strlen(arg) + 1);
    if (!flags[FILE_NAME]) {
        memory_error_message();
    }
    strncpy(flags[FILE_NAME], arg, strlen(arg));
    flags[FILE_NAME][strlen(arg)] = '\0';

    while ((argc -= 2) >= 0) {
        arg = *++argv;
        switch (arg[1]) {
            case 'd':
                flags[DELIMITER] = parse_arg(*(++argv));
                break;
            case 'i':
                flags[INCLUDE] = parse_arg(*(++argv));
                break;
            case 'p':
                flags[PATTERN] = parse_arg(*(++argv));
                break;
            default:
                user_error_message();
                break;
        }
    }

    if (flags[DELIMITER] == NULL) {
        flags[DELIMITER] = malloc(sizeof(char) * DEFFLAGSIZE);
        flags[DELIMITER][0] = '\n';
        flags[DELIMITER][1] = '\0';
    }

    // printf("%s | %s | %s \n", flags[FILE_NAME], flags[DELIMITER],
        //    flags[INCLUDE]);

    parse_file(flags);

    for (int i = 0; i < FLAGCOUNT; ++i) {
        if (flags[i]) {
            free(flags[i]);
        }
    }

    return 0;
}
