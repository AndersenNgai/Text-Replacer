#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "errors.h"
#include "copy.h"

#define HELP_TEXT "Flags:\n\
-i  Required. Takes 1 argument (input file)\n\
-o  Required. Takes 1 argument (output file)\n\
-s  Optional (required if -r or -w are defined). Takes 1 argument (search text)\n\
    Replaces all occurrences of the search text with the replacement text\n\
    Is case-sensitive and matches on whole words\n\
    Note that words are separated by whitespace\n\
-r  Optional (required if -s is defined). Takes 1 argument (replacement text)\n\
-w  Optional. Takes no arguments. Enables wildcard searching\n\
    Search text must be either prefix wild* or postfix *wild\n\
    Words are alphanumeric and are separated by punctuation and whitespace\n\
-l  Optional. Takes 2 arguments (start and end lines)\n\
    Only replace occurrences between the start and end lines (inclusive)\n\
    All other text is copied normally\n\n\
If only -i and -o are defined, the input file is copied into the output file\n\
If only -i, -o and -l are defined, only the given lines will be copied\n\n"

/* Flags are arguments that begin with '-' and are followed by exactly 1 letter

Error codes:
1   Cannot read input file
2   Cannot write to output file
    - Output file same as input file
3   File flags required but missing
4   Other flags required but missing
    - If -s is present, -r must be present
    - If -r, -l or -w are present, -s must be present
5   Flag wrong number of arguments
6   Flag bad arguments
    - If -w is present, then -s must be a wildcard
7   Duplicate flag
8   Unknown argument/flag

*/

// gcc src/copy.c src/errors.c src/main.c -I include

// Returns nonzero if str is a flag, 0 otherwise
int isflag(char *str) {
    if (str == NULL)
        return 0;
    return str[0] == '-' && strlen(str) == 2;
}

int main(int argc, char *argv[]) {
    // There is an edge case where the input and output paths are the same
    // This will erase the file if it already exists
    
    // Help flag (if present, ignore all other commands)
    for (int i = 1; i < argc; i++)
        if (strcmp(argv[i], "-h") == 0) {
            printf(HELP_TEXT);
            return 0;
        }

    // Process command line arguments
    char *infile_path = NULL, *outfile_path = NULL,
        *s_text = NULL, *r_text = NULL;
    int wildcard = 0; // -1 = *text, 1 = text*
    char *start_line_str = NULL, *end_line_str = NULL;
    argv++;
    while (*argv) {
        // Check only for flags and act accordingly from there, since each flag takes a set number of arguments
        // This means that each valid argument must begin with '-'
        if (!isflag(*argv))
            return ERROR_UNKNOWN_ARGUMENT(*argv);
        switch (argv[0][1]) {

        case 'i': // Selected a file to read
            if (infile_path) // Infile path already given
                return ERROR_DUPLICATE_FLAG('i');
            argv++; // Get the filename
            if (*argv == NULL || isflag(*argv))
                return ERROR_FLAG_ARG_COUNT("Flag -i takes 1 argument");
            infile_path = *argv;
            break;

        case 'o':
            if (outfile_path)
                return ERROR_DUPLICATE_FLAG('o');
            argv++;
            if (*argv == NULL || isflag(*argv))
                return ERROR_FLAG_ARG_COUNT("Flag -o takes 1 argument");
            outfile_path = *argv;
            break;

        case 's':
            if (s_text)
                return ERROR_DUPLICATE_FLAG('s');
            argv++;
            if (*argv == NULL || isflag(*argv))
                return ERROR_FLAG_ARG_COUNT("Flag -s takes 1 argument");
            s_text = *argv;
            break;

        case 'r':
            if (r_text)
                return ERROR_DUPLICATE_FLAG('r');
            argv++;
            if (*argv == NULL || isflag(*argv))
                return ERROR_FLAG_ARG_COUNT("Flag -r takes 1 argument");
            r_text = *argv;
            break;

        case 'w':
            if (wildcard)
                return ERROR_DUPLICATE_FLAG('w');
            wildcard = 2;
            break;

        case 'l':
            if (start_line_str)
                return ERROR_DUPLICATE_FLAG('l');
            argv++;
            if (*argv == NULL || isflag(*argv))
                return ERROR_FLAG_ARG_COUNT("Flag -l takes 2 arguments");
            start_line_str = *argv++;
            if (*argv == NULL || isflag(*argv))
                return ERROR_FLAG_ARG_COUNT("Flag -l takes 2 arguments");
            end_line_str = *argv;
            break;

        default:
            return ERROR_UNKNOWN_ARGUMENT(*argv);
        }
        argv++;
    }
    
    // Continue to validate the arguments

    if (infile_path == NULL || outfile_path == NULL)
        return ERROR_FILE_FLAGS_MISSING();
    
    int s_length = 0;
    if (s_text) {
        s_length = strlen(s_text);
        if (wildcard) {
            for (int i = 0; i < s_length; i++) {
                if (s_text[i] == '*') {
                    if (i == 0)
                        wildcard = -1;
                    else if (i == s_length - 1) {
                        if (wildcard == -1) // Something like *text* appeared (bad)
                            return ERROR_FLAG_BAD_ARGS("Wildcard must be either prefix or postfix, not both");
                        wildcard = 1;
                    }
                    else
                        return ERROR_FLAG_BAD_ARGS("Wildcard must be prefix or postfix");
                }
                else if (!isalpha(s_text[i]) && !isdigit(s_text[i]))
                    return ERROR_FLAG_BAD_ARGS("Wildcard must be alphanumeric");
            }
            if (wildcard == 2) // Missing wildcard
                return ERROR_FLAG_BAD_ARGS("Flag -w requires search text to be a wildcard");
            s_length--;
            if (wildcard == -1)
                s_text++;
            else
                s_text[s_length] = '\0';
        }
        else
            for (int i = 0; i < s_length; i++)
                if (isspace(s_text[i]))
                    return ERROR_FLAG_BAD_ARGS("Search text must not contain whitespace");

        if (r_text == NULL) // -s is defined and not empty, but -r is missing
            return ERROR_OTHER_FLAGS_MISSING('r', 's');
    }
    else {
        if (r_text)
            return ERROR_OTHER_FLAGS_MISSING('s', 'r');
        if (wildcard)
            return ERROR_OTHER_FLAGS_MISSING('s', 'w');
    }

    long int start_line = -1, end_line = -1;
    if (start_line_str) {
        char *endptr;
        start_line = strtol(start_line_str, &endptr, 10);
        if (endptr == start_line_str || *endptr != '\0')
            return ERROR_FLAG_BAD_ARGS("Flag -l takes positive integers");
        end_line = strtol(end_line_str, &endptr, 10);
        if (endptr == end_line_str || *endptr != '\0')
            return ERROR_FLAG_BAD_ARGS("Flag -l takes positive integers");
        if (start_line < 1 || end_line < 1)
            return ERROR_FLAG_BAD_ARGS("Flag -l takes positive integers");
        if (start_line > end_line)
            return ERROR_FLAG_BAD_ARGS("Starting line cannot be after end line");
    }

    if (strcmp(infile_path, outfile_path) == 0)
        return ERROR_CANNOT_WRITE(outfile_path, 1);

    FILE *infile = fopen(infile_path, "r");
    if (infile == NULL)
        return ERROR_CANNOT_READ(infile_path);
    FILE *outfile = fopen(outfile_path, "w");
    if (outfile == NULL) {
        fclose(infile);
        return ERROR_CANNOT_WRITE(outfile_path, 0);
    }
    
    // We have finished validating arguments
    // Now we can copy text. Cases:
    // -s defined: Copy text from infile to outfile, replacing as necessary
    //             If -l is defined, only replace the lines in -l
    // -s not defined (may be ""), -l not defined: Copy infile to outfile
    // -s not defined, -l defined: Copy only the lines within -l. Ignore everything else

    if (s_text == NULL)
        if (start_line == -1)
            plain_copy(infile, outfile);
        else
            copy_lines(infile, outfile, start_line, end_line);
    else
        copy_replace(infile, outfile, s_text, r_text, wildcard, start_line, end_line);
    
    fclose(infile);
    fclose(outfile);
    return 0;
}