#include "sds.h"
#include "sdsbase64.h"

static const char base64_table[65] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

sds sds_base64_decode(const char *src, size_t src_len)
{
    char *dst = malloc(src_len * 3 / 4 + 4);
    uint32_t ac = 0;
    int bits = 0;
    int i;
    char *bp = dst;

    for (i = 0; i < src_len; i++) {
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

sds sds_base64_encode(const char *src, size_t src_len)
{
    char* dst = malloc(src_len * 4 / 3 + 4);

    int i, bits = 0;
    u_int32_t ac = 0;
    char *cp = dst;

    for (i = 0; i < src_len; i++) {
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
