# Text-Replacer
Replaces user-inputted search text with replacement text. Supports wildcard search and replace.
Example use: `./text_replacer -i in.txt -o out.txt -s foo -r bar` replaces all occurrences of "the" in `in.txt` with "bar" and outputs it to `out.txt`.

# How to use
You must have the GCC compiler installed. Once installed, open the terminal, navigate to the proper directory and enter `gcc src/copy.c src/errors.c src/main.c -I include -o text_replacer`. Then execute `text_replacer`, using the flags below:

-i  Required. Takes 1 argument (input file)
-o  Required. Takes 1 argument (output file)
-s  Optional (required if -r or -w are defined). Takes 1 argument (search text)
    Replaces all occurrences of the search text with the replacement text
    Is case-sensitive and matches on whole words
    Note that words are separated by whitespace
-r  Optional (required if -s is defined). Takes 1 argument (replacement text)
-w  Optional. Takes no arguments. Enables wildcard searching
    Search text must be either prefix wild* or postfix *wild
    Words are alphanumeric and are separated by punctuation and whitespace
-l  Optional. Takes 2 arguments (start and end lines)
    Only replace occurrences between the start and end lines (inclusive)
    All other text is copied normally

If only -i and -o are defined, the input file is copied into the output file.
If only -i, -o and -l are defined, only the given lines will be copied.
