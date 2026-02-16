int read_standard(FILE *, char *, int, char *);

int read_wild(FILE *, char *, int, int *, char *);

void plain_copy(FILE *, FILE *);

void copy_lines(FILE *, FILE *, long int, long int);

void copy_replace(FILE *, FILE *, char *, char *, int, long int, long int);