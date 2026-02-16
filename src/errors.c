#include <stdio.h>
#include "errors.h"

int ERROR_CANNOT_READ(char *infile_path) {
    fprintf(stderr, "Error: Failed to open %s\n", infile_path);
    return 1;
}

int ERROR_CANNOT_WRITE(char *outfile_path, int same) {
    if (same)
        fprintf(stderr, "Error: Same input and output file\n");
    else
        fprintf(stderr, "Error: Cannot write to %s\n", outfile_path);
    return 2;
}

int ERROR_FILE_FLAGS_MISSING() {
    fprintf(stderr, "Error: Both flags -i and -o must be present\n");
    return 3;
}

int ERROR_OTHER_FLAGS_MISSING(char missing, char present) {
    fprintf(stderr, "Error: Flag -%c must be present when -%c is present\n", missing, present);
    return 4;
}

int ERROR_FLAG_ARG_COUNT(char *message) {
    fprintf(stderr, "Error: ");
    fprintf(stderr, "%s\n", message);
    return 5;
}

int ERROR_FLAG_BAD_ARGS(char *message) {
    fprintf(stderr, "Error: ");
    fprintf(stderr, "%s\n", message);
    return 6;
}

int ERROR_DUPLICATE_FLAG(char f) {
    fprintf(stderr, "Error: Duplicate flag -%c\n", f);
    return 7;
}

int ERROR_UNKNOWN_ARGUMENT(char *arg) {
    fprintf(stderr, "Unknown argument %s\n", arg);
    return 8;
}