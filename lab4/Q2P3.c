#include <stdio.h>
#include <stdlib.h>
#include<string.h>

int main(int argc, char *argv[]) {
    FILE *src, *dst;
    char c;

    // Check for command-line arguments
    if (argc > 1 && strcmp(argv[1], "-i") == 0) {
        if (argc > 2) {
            src = fopen(argv[2], "r");  // Open input file
        } else {
            printf("Error: Filename not provided after -i switch.\n");
            return 1;
        }
    } else {
        src = fopen("sample.txt", "r");  // Open default input file
    }

    if (src == NULL) {
        printf("Error: Input file not found.\n");
        return 1;
    }

    // Determine output file
    if (argc > 3 && strcmp(argv[3], "-o") == 0) {
        if (argc > 4) {
            dst = fopen(argv[4], "w");  // Open specified output file
        } else {
            printf("Error: Filename not provided after -o switch.\n");
            return 1;
        }
    } else {
        dst = fopen("sample_out.txt", "w");  // Open default output file
    }

    if (dst == NULL) {
        printf("Error: Failed to create output file.\n");
        fclose(src);
        return 1;
    }

    // Copy contents
    while ((c = fgetc(src)) != EOF) {
        fputc(c, dst);
    }

    fclose(src);
    fclose(dst);
    printf("File copied successfully.\n");
    return 0;
}
