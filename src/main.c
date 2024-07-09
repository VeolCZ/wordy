#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEFFLAGSIZE 100
#define FLAGCOUNT 5

#define HELPTEXT \
    "Wordy is a tool for simple csv parsing.\n\n\
Usage:\n\
    wordy <file_path> <args>\n\
Flags:\n\
    -h  help\n\
    -d  delimiter for the file, defaults to \\n, one char only\n\
    -i  parse those that include letters 'abc'\n\
    -c  parse those that are composed of letters 'abc'\n\
    -p  parse those that match a pattern '#a##b' with # being wildcard\n\
    -l  parse word of certain length\n"

#define error_message(message)      \
    fprintf(stderr, "%s", message); \
    exit(1);

#define user_error_message() \
    error_message("Invalid usage of Wordy. Use -h flag for help.\n")
#define memory_error_message() error_message("Error system is out of memory.\n")

typedef char *String;

enum flags { FILE_NAME, DELIMITER, INCLUDE, COMPOSE, PATTERN, LENGTH };

String parse_arg(String);
void parse_file(String *);
int filter_includes_line(String, String);
int filter_composes_line(String, String);
int filter_pattern_line(String, String);
int filter_length_line(String, int);

String parse_arg(String arg) {
    String res = malloc(strlen(arg) + 1);
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
    }

    while ((c = getc(file)) != EOF) {
        if (i == max_size) {
            max_size *= 2;
            line = realloc(line, max_size);
        } else if (c == flags[DELIMITER][0]) {
            line[i] = '\0';
            i = 0;
            // Add filtering
        } else {
            line[i++] = c;
        }
    }

    free(line);
    fclose(file);
}

int filter_includes_line(String line, String chars) {
    if (line == NULL || chars == NULL) {
        return -1;
    }

    while (*chars != '\0') {
        if (strchr(line, *chars) == NULL) {
            return -1;
        }
        chars++;
    }

    return 0;
}

int filter_composes_line(String line, String chars) {
    if (line == NULL || chars == NULL) {
        return -1;
    }

    while (*line != '\0') {
        if (strchr(chars, *line) == NULL) {
            return -1;
        }
        line++;
    }

    return 0;
}

int filter_pattern_line(String line, String pattern) {
    if (line == NULL || pattern == NULL) {
        return -1;
    }
    int p_size = strlen(pattern), l_size = strlen(line);
    if (p_size > l_size) {
        user_error_message();
    }
    while (l_size > p_size) {
        pattern = realloc(pattern, l_size);
        while (p_size++ < l_size) {
            pattern[p_size] = '#';
        }
    }

    while (--p_size > 0) {
        if (pattern[p_size] != '#' && pattern[p_size] != line[p_size]) {
            return -1;
        }
    }

    return 0;
}

int filter_length_line(String line, int length) {
    return strlen(line) == length ? 0 : -1;
}

int main(int argc, String argv[]) {
    String flags[FLAGCOUNT] = {0};

    if (argc % 2 != 0 || --argc < 1) {
        user_error_message();
    }
    ++argv;

    if (strcmp(*argv, "-h") == 0) {
        printf("%s", HELPTEXT);
        exit(0);
    } else if (access(*argv, F_OK) != 0) {
        user_error_message();
    }

    flags[FILE_NAME] = (String)malloc(strlen(*argv) + 1);
    if (!flags[FILE_NAME]) {
        memory_error_message();
    }
    strncpy(flags[FILE_NAME], *argv, strlen(*argv));
    flags[FILE_NAME][strlen(*argv)] = '\0';

    while ((argc -= 2) >= 0) {
        switch (*(++argv)[1]) {
            case 'd':
                flags[DELIMITER] = parse_arg(*(++argv));
                break;
            case 'i':
                flags[INCLUDE] = parse_arg(*(++argv));
                break;
            case 'c':
                flags[COMPOSE] = parse_arg(*(++argv));
                break;
            case 'p':
                flags[PATTERN] = parse_arg(*(++argv));
                break;
            case 'l':
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

    if (flags[COMPOSE] != NULL) {
        flags[COMPOSE] = realloc(
            flags[COMPOSE],
            sizeof(char) * (strlen(flags[COMPOSE]) + strlen(flags[DELIMITER])));
        flags[COMPOSE] = strcat(flags[COMPOSE], flags[DELIMITER]);
    }

    printf("%s | %s | %s | %s\n", flags[FILE_NAME], flags[DELIMITER],
           flags[INCLUDE], flags[COMPOSE]);

    parse_file(flags);

    for (int i = 0; i < FLAGCOUNT; ++i) {
        if (flags[i]) {
            free(flags[i]);
        }
    }

    return 0;
}
