// SPDX-License-Identifier: GPL-2.0
/*
 * base64.c - RFC4648-compliant base64 encoding
 *
 * Copyright (c) 2020 Hannes Reinecke, SUSE
 *
 * Based on the base64url routines from fs/crypto/fname.c
 * (which are using the URL-safe base64 encoding),
 * modified to use the standard coding table from RFC4648 section 4.
 */
#include "sds.h"
#include <string.h>

#ifndef _UTIL_BASE64DECODE_H
#define _UTIL_BASE64DECODE_H

static const char base64_table[65] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


sds base64_encode(const char *src, int srclen)
{
    char* dst = malloc(srclen * 4/3+4);

    int i, bits = 0;
    u_int32_t ac = 0;
    char *cp = dst;

    for (i = 0; i < srclen; i++) {
        ac = (ac << 8) | src[i];
        bits += 8;
        do {
            bits -= 6;
            *cp++ = base64_table[(ac >> bits) & 0x3f];
        } while (bits >= 6);
    }
    if (bits) {
        *cp++ = base64_table[(ac << (6 - bits)) & 0x3f];
        bits -= 6;
    }
    while (bits < 0) {
        *cp++ = '=';
        bits += 2;
    }

    sds result =  sdsnewlen(dst, cp - dst);
    free(dst);
    return result;
}


sds base64_decode(const char *src, int srclen)
{
    char *dst = malloc(srclen * 3/4+4);
    uint32_t ac = 0;
    int bits = 0;
    int i;
    char *bp = dst;

    for (i = 0; i < srclen; i++) {
        const char *p = strchr(base64_table, src[i]);

        if (src[i] == '=') {
            ac = (ac << 6);
            bits += 6;
            if (bits >= 8)
                bits -= 8;
            continue;
        }
        if (p == NULL || src[i] == 0)
            return sdsnew("");
        ac = (ac << 6) | (p - base64_table);
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            *bp++ = (uint8_t)(ac >> bits);
        }
    }
    if (ac & ((1 << bits) - 1))
        return sdsnew("");

    return sdsnewlen(dst, bp - dst);
}



#endif