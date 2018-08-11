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

static inline uint64_t esid_encode_char(int c)
{
    if (c < '!')
    {
        return (uint64_t)('_' - ESID_SHIFT);
    }
    if (c > '_')
    {
        return (uint64_t)(c - ('a' - 'A' + ESID_SHIFT));
    }
    return (uint64_t)(c - ESID_SHIFT);
}

uint64_t encode_string_id(const char *str, size_t size, size_t start)
{
    size -= start;
    if (size > ESIDSHIFT_MAXLEN)
    {
        size = ESIDSHIFT_MAXLEN;
    }
    uint64_t h = 0;
    str += start;
    const char *pos = (str + size - 1);
    switch (size)
    {
    case 10:
        h ^= esid_encode_char(*pos--);
    // fall through
    case 9:
        h ^= esid_encode_char(*pos--) << 6;
    // fall through
    case 8:
        h ^= esid_encode_char(*pos--) << 12;
    // fall through
    case 7:
        h ^= esid_encode_char(*pos--) << 18;
    // fall through
    case 6:
        h ^= esid_encode_char(*pos--) << 24;
    // fall through
    case 5:
        h ^= esid_encode_char(*pos--) << 30;
    // fall through
    case 4:
        h ^= esid_encode_char(*pos--) << 36;
    // fall through
    case 3:
        h ^= esid_encode_char(*pos--) << 42;
    // fall through
    case 2:
        h ^= esid_encode_char(*pos--) << 48;
    // fall through
    case 1:
        h ^= esid_encode_char(*pos) << 54;
    }
    return h;
}

size_t decode_string_id(uint64_t esid, char *str)
{
    if (esid >> 63)
    {
        str[0] = 0;
        return 0; // hash encoding
    }
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

// Mix two 64 bit hash numbers using a MurmurHash3-like algorithm
static inline uint64_t muxhash64(uint64_t k, uint64_t h)
{
    k *= 0x87c37b91114253d5;
    k = (k >> 33) | (k << 31);
    k *= 0x4cf5ad432745937f;
    h ^= k;
    h = (h >> 37) | (h << 27);
    return ((h * 5) + 0x52dce729);
}

uint64_t hash_string_id(const char *str, size_t size)
{
    const uint64_t *pos = (const uint64_t *)str; // NOTE endianness
    const uint64_t *end = pos + (size / 8);
    uint64_t h = 0;
    while (pos < end)
    {
        h = muxhash64(*pos++, h);
    }
    const uint8_t* tail = (const uint8_t*)pos;
    uint64_t v = 0;
    switch (size & 7)
    {
    case 7:
        v ^= (uint64_t)tail[6] << 48;
    // fall through
    case 6:
        v ^= (uint64_t)tail[5] << 40;
    // fall through
    case 5:
        v ^= (uint64_t)tail[4] << 32;
    // fall through
    case 4:
        v ^= (uint64_t)tail[3] << 24;
    // fall through
    case 3:
        v ^= (uint64_t)tail[2] << 16;
    // fall through
    case 2:
        v ^= (uint64_t)tail[1] << 8;
    // fall through
    case 1:
        v ^= (uint64_t)tail[0];
    }
    if (v > 0)
    {
        h = muxhash64(v, h);
    }
    // MurmurHash3 finalization mix - force all bits of a hash block to avalanche
    h ^= h >> 33;
    h *= 0xff51afd7ed558ccd;
    h ^= h >> 33;
    h *= 0xc4ceb9fe1a85ec53;
    h ^= h >> 33;
    return (h | 0x8000000000000000); // set the first bit to indicate HASH mode
}
