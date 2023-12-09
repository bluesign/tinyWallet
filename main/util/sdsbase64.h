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



sds sds_base64_encode(const char *src, size_t src_len);
sds sds_base64_decode(const char *src, size_t src_len);


#endif