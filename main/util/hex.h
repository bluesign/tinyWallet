#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sds.h"

#ifndef TINY_WALLET_H
#define TINY_WALLET_H

unsigned char* hex_to_byte(char* hex);
sds bytes_to_hex(unsigned char* bytes, size_t length);


#endif
