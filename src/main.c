#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEFARGSIZE 100
#define FLAGCOUNT 3

#define HELPTEXT \
    "Wordy is a tool for simple csv parsing.\n\n\
Usage:\n\
    wordy <file_name> <args>\n\
Flags:\n\
    -h  help\n\
    -d  delimiter for the file, defaults to \\n\n\
    -i  parse those that include letters 'abc'\n"

#define error_message(message)      \
    fprintf(stderr, "%s", message); \
    exit(1);

#define user_error_message() \
    error_message("Invalid usage of Wordy. Use -h flag for help.\n")
#define memory_error_message() error_message("Error system is out of memory.\n")

typedef char* String;

enum flags { FILE_NAME, DELIMITER, INCLUDE };

String parse_arg(String arg) {
    String res = (String)malloc(strlen(arg) + 1);
    if (!res) {
        memory_error_message();
    }

    strncpy(res, arg, strlen(arg));
    res[strlen(arg)] = '\0';

    return res;
}

int main(int argc, String argv[]) {
    String args[FLAGCOUNT] = {NULL, NULL, NULL};
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

    args[FILE_NAME] = (String)malloc(strlen(arg) + 1);
    if (!args[FILE_NAME]) {
        memory_error_message();
    }
    strncpy(args[FILE_NAME], arg, strlen(arg));
    args[FILE_NAME][strlen(arg)] = '\0';

    while ((argc -= 2) >= 0) {
        arg = *++argv;
        switch (arg[1]) {
            case 'd':
                args[DELIMITER] = parse_arg(*(++argv));
                break;
            case 'i':
                args[INCLUDE] = parse_arg(*(++argv));
                break;
            default:
                user_error_message();
                break;
        }
    }

    printf("%s | %s | %s \n", args[FILE_NAME], args[DELIMITER], args[INCLUDE]);

    for (int i = 0; i < FLAGCOUNT; ++i) {
        if (args[i]) {
            free(args[i]);
        }
    }

    return 0;
}
