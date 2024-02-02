#include <stdio.h>
#include <stdlib.h>
#include<string.h>

int main(int argc, char *argv[]) {
    FILE *src;
    char c;

    // Check for command-line argument
    if (argc > 1 && strcmp(argv[1], "-i") == 0) {
        if (argc > 2) {
            src = fopen(argv[2], "r");  // Open the file specified after -i
        } else {
            printf("Error: Filename not provided after -i switch.\n");
            return 1;
        }
    } else {
        src = fopen("sample.txt", "r");  // Open default file if no -i switch
    }

    if (src == NULL) {
        printf("File not found. Exiting.\n");
        return 1;  // Indicate failure
    }

    while ((c = fgetc(src)) != EOF) {
        printf("%c", c);
    }

    fclose(src);
    return 0;
}
