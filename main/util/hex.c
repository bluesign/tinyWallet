#include "hex.h"
unsigned char* hex_to_byte(char* hex){

    printf("hex: %s\n", hex);
    size_t len = strlen(hex);
    printf("len: %d\n", len);

    if (len % 2 != 0)
    {
        printf("Invalid hex string length\n");
        return NULL;
    }
    unsigned char* buffer = (unsigned char*) malloc(len/2);
    if (buffer == NULL)
    {
        printf("Error allocating memory\n");
        return NULL;
    }

    for (int i = 0; i < len; i += 2)
    {
        char hex_byte[3] = {hex[i], hex[i+1], 0};
        char chr = (char) (int) strtol(hex_byte, NULL, 16);
        buffer[i/2]=chr;
    }

    return buffer;

}