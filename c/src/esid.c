// VariantKey
//
// esid.c
//
// @category   Libraries
// @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
// @copyright  2017-2018 GENOMICS plc
// @license    MIT (see LICENSE)
// @link       https://github.com/genomicsplc/variantkey
//
// LICENSE
//
// Copyright (c) 2017-2018 GENOMICS plc
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <stdio.h>
#include "esid.h"

#define ESID_SHIFT    32 //!< Number used to translate ASCII character values
#define ESIDSHIFT_POS 60 //!< Encoded string ID LEN LSB position from LSB [ ----0000 00111111 22222233 33334444 44555555 66666677 77778888 88999999 ]

static inline int esid_encode_char(int c)
{
    if (c < '!')
    {
        return ('_' - ESID_SHIFT);
    }
    if (c > '_')
    {
        return (c - ('a' - 'A' + ESID_SHIFT));
    }
    return (c - ESID_SHIFT);
}

uint64_t encode_string_id(const char *str, size_t size, size_t start)
{
    size -= start;
    if (size > ESIDSHIFT_MAXLEN)
    {
        size = ESIDSHIFT_MAXLEN;
    }
    uint64_t c;
    uint64_t h = 0;
    uint8_t bitpos = ESIDSHIFT_POS;
    str += start;
    while (size--)
    {
        bitpos -= 6;
        c = (uint64_t)esid_encode_char(*str++);
        h |= (c << bitpos);
    }
    return h;
}

size_t decode_string_id(uint64_t esid, char *str)
{
    size_t size = 0;
    uint8_t bitpos = ESIDSHIFT_POS;
    size_t i = 0;
    char code;
    for(i = 0; i < ESIDSHIFT_MAXLEN; i++)
    {
        bitpos -= 6;
        code = ((esid >> bitpos) & 0x3f);
        if (code == 0)
        {
            break;
        }
        str[i] = (code + ESID_SHIFT); // 0x3f is the 6 bit mask [00111111]
        size++;
    }
    str[i] = 0;
    return size;
}
