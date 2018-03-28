// VariantKey
//
// variantkey.c
//
// @category   Tools
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
//
// VariantKey by Nicola Asuni

#include <inttypes.h>
#include <stdio.h>
#include "variantkey.h"

inline int aztoupper(int c)
{
    if ((c >= 'a') && (c <= 'z'))
    {
        return (c ^ ('a' - 'A'));
    }
    return c;
}

inline uint8_t encode_chrom(const char *chrom, size_t size)
{
    // remove "chr" prefix
    if ((size > 3)
            && ((chrom[0] == 'C') || (chrom[0] == 'c'))
            && ((chrom[1] == 'H') || (chrom[1] == 'h'))
            && ((chrom[2] == 'R') || (chrom[2] == 'r')))
    {
        chrom += 3;
        size -= 3;
    }
    if ((chrom[0] >= '0') && (chrom[0] <= '9')) // Number
    {
        uint8_t v = 0;
        size_t i;
        for (i = 0; i < size; i++)
        {
            v = ((v * 10) + (chrom[i] - '0'));
        }
        return v;
    }
    if ((chrom[0] == 'X') || (chrom[0] == 'x'))
    {
        return 23;
    }
    if ((chrom[0] == 'Y') || (chrom[0] == 'y'))
    {
        return 24;
    }
    if ((chrom[0] == 'M') || (chrom[0] == 'm')) // MT
    {
        return 25;
    }
    return 0; // NA
}

inline size_t decode_chrom(uint8_t code, char *chrom)
{
    if ((code < 1) || (code > 25))
    {
        return sprintf(chrom, "NA");
    }
    if (code < 23)
    {
        return sprintf(chrom, "%"PRIu8, code);
    }
    const char *map[] = {"X", "Y", "MT"};
    return sprintf(chrom, "%s", map[(code - 23)]);
}

static inline void encode_refalt_str(uint32_t *h, uint8_t *bitpos, const char *str, size_t size)
{
    int c;
    while ((c = aztoupper(*str++)) && (size--))
    {
        if (c == '*')
        {
            c = ('Z' + 1);
        }
        *bitpos -= 5;
        *h |= ((c - 'A' + 1) << *bitpos); // A will be coded as 1
    }
}

static inline uint32_t encode_refalt_rev(const char *ref, size_t sizeref, const char *alt, size_t sizealt)
{
    //[00000000 00000000 00000000 00000000 0RRFF111 11222223 33334444 45555500]
    uint32_t h = 0;
    h |= ((uint32_t)(sizeref - 1) << 29); // RR: length of (REF - 1)
    h |= ((uint32_t)(sizealt - 1) << 27); // FF: length of (ALT - 1)
    uint8_t bitpos = 27;
    encode_refalt_str(&h, &bitpos, ref, sizeref);
    encode_refalt_str(&h, &bitpos, alt, sizealt);
    return h;
}

// Mix two 32 bit hash numbers using the MurmurHash3 algorithm
static inline uint32_t muxhash(uint32_t k, uint32_t h)
{
    k *= 0xcc9e2d51;
    k = (k >> 17) | (k << (32 - 17));
    k *= 0x1b873593;
    h ^= k;
    h = (h >> 19) | (h << (32 - 19));
    return ((h * 5) + 0xe6546b64);
}

// Return a 32 bit hash of a nucleotide string
static inline uint32_t hash32(const char *str, size_t size)
{
    uint32_t h = 0;
    uint32_t k;
    uint8_t bitpos;
    size_t len = 6;
    while (size > 0)
    {
        if (size < len)
        {
            len = size;
        }
        k = 0;
        bitpos = 31;
        //[00000000 00000000 00000000 00000000 01111122 22233333 44444555 55666660]
        encode_refalt_str(&k, &bitpos, str, len); // pack 6 characters in 32 bit (6 x 5 bit + 2 spare bit)
        h = muxhash(k, h);
        size -= len;
        str += len;
    }
    return h;
}

static inline uint32_t encode_refalt_hash(const char *ref, size_t sizeref, const char *alt, size_t sizealt)
{
    // 0x3 is the separator character between REF and ALT [00000000 00000000 00000000 00000011]
    uint32_t h = muxhash(hash32(alt, sizealt), muxhash(0x3, hash32(ref, sizeref)));
    // finalization mix - MurmurHash3 algorithm
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return ((h >> 1) | 0x1); // 0x1 is the set bit to indicate HASH mode [00000000 00000000 00000000 00000001]
}

inline uint32_t encode_refalt(const char *ref, size_t sizeref, const char *alt, size_t sizealt)
{
    if ((sizeref + sizealt) > 5)
    {
        return encode_refalt_hash(ref, sizeref, alt, sizealt);
    }
    return encode_refalt_rev(ref, sizeref, alt, sizealt);
}

static inline char decode_refalt_char(uint32_t code, int bitpos)
{
    char c = ((code >> bitpos) & 0x1F); // 0x1F is the 5 bit mask [00011111]
    if (c == (('Z' + 1 - 'A') + 1)) // character after Z
    {
        return '*';
    }
    return (c + 'A' - 1);
}

static inline size_t decode_refalt_rev(uint32_t code, char *ref, size_t *sizeref, char *alt, size_t *sizealt)
{
    *sizeref = (size_t)((code & 0x60000000) >> 29) + 1; // [01100 00000 00000 00000 00000 00000 00]
    *sizealt = (size_t)((code & 0x18000000) >> 27) + 1; // [00011 00000 00000 00000 00000 00000 00]
    uint8_t bitpos = 27;
    size_t i = 0;
    for(i = 0; i < *sizeref; i++)
    {
        bitpos -= 5;
        ref[i] = decode_refalt_char(code, bitpos);
    }
    ref[i] = '\0';
    for(i = 0; i < *sizealt; i++)
    {
        bitpos -= 5;
        alt[i] = decode_refalt_char(code, bitpos);
    }
    alt[i] = '\0';
    return (*sizeref + *sizealt);
}

inline size_t decode_refalt(uint32_t code, char *ref, size_t *sizeref, char *alt, size_t *sizealt)
{
    if (code & 0x1) // check last bit
    {
        return 0; // non-reversible encoding
    }
    return decode_refalt_rev(code, ref, sizeref, alt, sizealt);
}

inline uint64_t variantkey(const char *chrom, size_t sizechrom, uint32_t pos, const char *ref, size_t sizeref, const char *alt, size_t sizealt)
{
    return (((uint64_t)encode_chrom(chrom, sizechrom) << 59)
            | ((uint64_t)pos << 31)
            | (uint64_t)encode_refalt(ref, sizeref, alt, sizealt));
}

inline size_t variantkey_string(uint64_t code, char *str)
{
    return sprintf(str, "%016"PRIx64"", code);
}

inline uint64_t parse_variantkey_string(const char *vs)
{
    uint64_t v = 0;
    uint8_t b;
    size_t i;
    for (i = 0; i < 16; i++)
    {
        b = vs[i];
        if (b >= 'a')
        {
            b -= ('a' - 10); // a-f
        }
        else
        {
            if (b >= 'A')
            {
                b -= ('A' - 10); // A-F
            }
            else
            {
                b -= '0'; // 0-9
            }
        }
        v = ((v << 4) | b);
    }
    return v;
}

inline variantkey_t decode_variantkey(uint64_t code)
{
    variantkey_t v = {0,0,0};
    // CHROM:   11111000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
    v.chrom = (uint8_t)((code & 0xF800000000000000) >> 59);
    // POS:     00000111 11111111 11111111 11111111 10000000 00000000 00000000 00000000
    v.pos = (uint32_t)((code & 0x07FFFFFF80000000) >> 31);
    // REF+ALT: 00000000 00000000 00000000 00000000 01111111 11111111 11111111 11111111
    v.refalt = (uint32_t)(code & 0x000000007FFFFFFF);
    return v;
}
