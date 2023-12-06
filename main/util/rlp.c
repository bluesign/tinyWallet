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

#include "rlp.h"
#include "sds.h"
#include "hex.h"
#include <string.h>
#include <stdio.h>


enum rlpProtocolConstants {
    RLP_EXTENDED_LENGTH_THRESHOLD = 55,
    RLP_OFFSET_ITEM_SHORT         = 0x80,
    RLP_OFFSET_ITEM_LONG          = 0xB7,
    RLP_OFFSET_LIST_SHORT         = 0xC0,
    RLP_OFFSET_LIST_LONG          = 0xF7,
};

sds rlp_encode_number(uint32_t number){
    size_t tmpLength = number;
    size_t lengthOfNumber = (size_t) 0;

    printf("number: %d\n", number);
    do{
        lengthOfNumber++;
        printf("tmpLength: %d\n", tmpLength);
        tmpLength = tmpLength >> 8;

    }while(tmpLength != 0);
    printf("lengthOfNumber: %d\n", lengthOfNumber);


    unsigned char numberEncoded[lengthOfNumber];
    for(size_t i = lengthOfNumber; i > 0; --i) {
        numberEncoded[i-1] = (char)number;
        number = number >> 8;
    }

    return rlp_encode_element(numberEncoded, lengthOfNumber);
}

sds rlp_encode_element(unsigned char *payload,size_t payloadSize) {
    sds result = sdsempty();
    char length = 0;

    if(payloadSize == 0) {
        length = (char)RLP_OFFSET_ITEM_SHORT;
        result = sdscatlen(result, &length, 1);
    }
    else if(payloadSize == 1 && payload[0] < RLP_OFFSET_ITEM_SHORT) {
        result = sdscatlen(result, &payload[0], 1);
    }
    else if(payloadSize < RLP_EXTENDED_LENGTH_THRESHOLD) {
        length = (char)(RLP_OFFSET_ITEM_SHORT + payloadSize);
        result = sdscatlen(result, &length, 1);
        result = sdscatlen(result, payload, payloadSize);
    }
    else {
        size_t tmpLength = payloadSize;
        size_t lengthOfLength = (size_t) 0;
        while (tmpLength != 0) {
            ++lengthOfLength;
            tmpLength = tmpLength >> 8;
        }
        length = (char)(RLP_OFFSET_ITEM_LONG + lengthOfLength);
        result = sdscatlen(result, &length, 1);

        unsigned char numberEncoded[lengthOfLength];
        uint32_t size = payloadSize;

        for(size_t i = lengthOfLength; i > 0; --i) {
            numberEncoded[i-1] = (char)size;
            size = size >> 8;
        }

        result = sdscatlen(result, numberEncoded, lengthOfLength);
        result = sdscatlen(result, payload, payloadSize);
    }

    return result;
}

sds rlp_encode_list(sds elements, size_t elementsSize) {
    sds result = sdsempty();
    char length = 0;

    if(elementsSize == 0) {
        length = (char)RLP_OFFSET_LIST_SHORT;
        result = sdscatlen(result, &length, 1);
    }
    else if(elementsSize < RLP_EXTENDED_LENGTH_THRESHOLD) {
        length = (char)(RLP_OFFSET_LIST_SHORT + elementsSize);
        result = sdscatlen(result, &length, 1);
    }
    else {
        size_t tmpLength = elementsSize;
        size_t lengthOfLength = (size_t) 0;
        while (tmpLength != 0) {
            ++lengthOfLength;
            tmpLength = tmpLength >> 8;
        }
        length = (char)(RLP_OFFSET_LIST_LONG + lengthOfLength);
        result = sdscatlen(result, &length, 1);

        uint32_t size = elementsSize;
        unsigned char numberEncoded[lengthOfLength];
        for(size_t i = lengthOfLength; i > 0; --i) {
            numberEncoded[i-1] = (char)size;
            size = size >> 8;
        }

        result = sdscatlen(result, numberEncoded, lengthOfLength);

    }

    result = sdscatlen(result, elements, elementsSize);

    return result;
}


