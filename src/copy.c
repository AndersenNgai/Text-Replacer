#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "copy.h"

/* Returns -1 if reached EOF, 1 if no whitespace found, 0 otherwise
   Buffer contains characters, no whitespace
   The whitespace ending (if any) is outputted in *whitespace
   buffer_len is the maximum number of characters that can be read, EXCLUDING NULL
   For instance, if buffer_len = 100, then the buffer must be at least 101 chars */
int read_standard(FILE *infile, char *buffer, int buffer_len, char *whitespace) {
    // Words are separated by whitespace
    int i, c;
    for (i = 0; i < buffer_len; i++) {
        c = fgetc(infile);
        if (c == EOF) {
            buffer[i] = '\0';
            return -1;
        }
        if (isspace(c)) {
            buffer[i] = '\0';
            *whitespace = c;
            return 0;
        }
        buffer[i] = c;
    }
    buffer[i] = '\0';
    return 1;
}

/* Similar to read_standard, but the boundaries are whitespace and punctuation
   And the length of the word read is outputted in *out_len */ 
int read_wild(FILE *infile, char *buffer, int buffer_len, int *out_len, char *boundary) {
    // Words are separated by anything non-alphanumeric
    int i, c;
    for (i = 0; i < buffer_len; i++) {
        c = fgetc(infile);
        if (c == EOF) {
            buffer[i] = '\0';
            *out_len = i;
            return -1;
        }
        if (!isalpha(c) && !isdigit(c)) {
            buffer[i] = '\0';
            *out_len = i;
            *boundary = c;
            return 0;
        }
        buffer[i] = c;
    }
    buffer[i] = '\0';
    *out_len = i;
    return 1;
}

/* Copy a file to another file */
void plain_copy(FILE *infile, FILE *outfile) {
    int c = fgetc(infile);
    while (c != EOF) {
        fputc(c, outfile);
        c = fgetc(infile);
    }
}

/* Copy specific lines from one file to another file */
void copy_lines(FILE *infile, FILE *outfile, long int start, long int end) {
    long int current_line = 1;
    int c;
    while (current_line < start) {
        c = fgetc(infile);
        if (c == EOF)
            return;
        if (c == '\n')
            current_line++;
    }
    while (1) {
        c = fgetc(infile);
        if (c == EOF)
            return;
        if (c == '\n') {
            current_line++;
            if (current_line > end)
                return;
        }
        fputc(c, outfile);
    }
}

/* Copy and replace search text with replacement text */
void copy_replace(FILE *infile, FILE *outfile, char *s_text, char *r_text, int wildcard, long int start_line, long int end_line) {

    long int current_line = 1;
    if (start_line > 0) {
        int c;
        while (current_line < start_line) {
            c = fgetc(infile);
            if (c == EOF)
                return;
            fputc(c, outfile);
            if (c == '\n')
                current_line++;
        }
    }

    char buffer[1001];
    int read, length, s_len;
    char boundary;
    switch (wildcard) {
        case 0: do {
            read = read_standard(infile, buffer, 1000, &boundary);
            if (strcmp(buffer, s_text) == 0)
                fputs(r_text, outfile);
            else
                fputs(buffer, outfile);
            if (read == 0) {
                fputc(boundary, outfile);
                if (start_line > 0 && boundary == '\n') {
                    current_line++;
                    if (current_line > end_line)
                        break;
                }
            }
            } while (read != -1); break;

        case -1: // *text
            s_len = strlen(s_text);
            do {
            read = read_wild(infile, buffer, 1000, &length, &boundary);
            if (length >= s_len && strcmp(buffer + length - s_len, s_text) == 0)
                fputs(r_text, outfile);
            else
                fputs(buffer, outfile);
            if (read == 0) {
                fputc(boundary, outfile);
                if (start_line > 0 && boundary == '\n') {
                    current_line++;
                    if (current_line > end_line)
                        break;
                }
            }
            } while (read != -1);
            break;
        
        case 1: // text*
            int match;
            s_len = strlen(s_text);
            do {
            read = read_wild(infile, buffer, 1000, &length, &boundary);
            match = 0;
            if (length >= s_len) {
                match = 1;
                for (int i = 0; i < s_len; i++)
                    if (buffer[i] != s_text[i]) {
                        match = 0;
                        break;
                    }
            }
            if (match)
                fputs(r_text, outfile);
            else
                fputs(buffer, outfile);
           if (read == 0) {
                fputc(boundary, outfile);
                if (start_line > 0 && boundary == '\n') {
                    current_line++;
                    if (current_line > end_line)
                        break;
                }
            }
            } while (read != -1);
            break;
    }

    if (feof(infile))
        return;
    plain_copy(infile, outfile);
}