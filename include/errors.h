void close_files(FILE *, FILE *);

int ERROR_CANNOT_READ(char *);

int ERROR_CANNOT_WRITE(char *, int);

int ERROR_FILE_FLAGS_MISSING();

int ERROR_OTHER_FLAGS_MISSING(char, char);

int ERROR_FLAG_ARG_COUNT(char *);

int ERROR_FLAG_BAD_ARGS(char *);

int ERROR_DUPLICATE_FLAG(char);

int ERROR_UNKNOWN_ARGUMENT(char *);