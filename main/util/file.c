#include "file.h"

char* read_file_all(char *filename) {
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        printf("Failed to open file\n");
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *string = malloc(fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);

    string[fsize] = 0;
    return string;
}
