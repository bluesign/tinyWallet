/**
 * RLP Serializer
 * https://github.com/afkamalipour/simple-rlp
 *
 * This library implements the RLP Encoding scheme used by Ethereum.
 * The specifications referred to are available at the URL below.
 * Spec: https://github.com/ethereum/wiki/wiki/%5BEnglish%5D-RLP
 *
 * Note: This library does not validate ethereum transactions,
 * this RLP serializer will serialize anything into the RLP encoding standard.
 */

/**
 * MIT License
 *
 * Copyright (c) 2020 Aurash Kamalipour <afkamalipour@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __RLP_SERIALIZER_H_
#define __RLP_SERIALIZER_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "sds.h"

sds rlp_encode_number(uint32_t number);
sds rlp_encode_element(unsigned char *payload,size_t payloadSize);
sds rlp_encode_list(sds elements, size_t elementsSize);



#endif