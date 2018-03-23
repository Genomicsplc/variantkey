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

int aztoupper(int c)
{
    if ((c >= 'a') && (c <= 'z'))
    {
        return (c ^ ('a' - 'A'));
    }
    return c;
}

uint8_t encode_chrom(const char *chrom, size_t size)
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
    if ((chrom[0] >= '0') && (chrom[0] <= '9'))
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

size_t decode_chrom(uint8_t code, char *chrom)
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

uint32_t encode_refalt_hash(const char *ref, size_t sizeref, const char *alt, size_t sizealt)
{
    size_t slen = sizeref + sizealt;
    char ra[slen + 2];
    char *it = ra;
    while ((*it++ = aztoupper(*ref++)) && (sizeref--)) {};
    it[-1] = '+';
    while ((*it++ = aztoupper(*alt++)) && (sizealt--)) {};
    it[-1] = '\0';
    return (farmhash32(ra, slen + 1) | 0x40000000); // set bit to indicate HASH mode [01000000 00000000 00000000 00000000]
}

void encode_refalt_str(uint32_t *h, uint8_t *pos, const char *str, size_t size)
{
    int c;
    while ((c = aztoupper(*str++)) && (size--))
    {
        if (c == '*')
        {
            c = 91;
        }
        *pos -= 5;
        *h |= (((c - 64) & 0x1F) << *pos);
    };
}

uint32_t encode_refalt_rev(const char *ref, size_t sizeref, const char *alt, size_t sizealt)
{
    uint32_t h = 0;
    h |= (uint32_t)(sizeref - 1) << 27; // length of REF - 1
    h |= (uint32_t)(sizealt - 1) << 25; // length of ALT - 1
    uint8_t pos = 25;
    encode_refalt_str(&h, &pos, ref, sizeref);
    encode_refalt_str(&h, &pos, alt, sizealt);
    return h;
}

uint32_t encode_refalt(const char *ref, size_t sizeref, const char *alt, size_t sizealt)
{
    if ((sizeref + sizealt) > 5)
    {
        return encode_refalt_hash(ref, sizeref, alt, sizealt);
    }
    return encode_refalt_rev(ref, sizeref, alt, sizealt);
}

char decode_refalt_char(uint32_t code, int pos)
{
    char c = ((code >> pos) & 0x1F);
    if (c == 27)
    {
        return '*';
    }
    return (c + 64);
}

size_t decode_refalt_rev(uint32_t code, char *ref, size_t *sizeref, char *alt, size_t *sizealt)
{
    *sizeref = 1 + ((code & 0x18000000) >> 27); // [000 11 00 00000 00000 00000 00000 00000]
    *sizealt = 1 + ((code & 0x06000000) >> 25); // [000 00 11 00000 00000 00000 00000 00000]
    uint8_t pos = 25;
    size_t i = 0;
    for(i = 0; i < *sizeref; i++)
    {
        pos -= 5;
        ref[i] = decode_refalt_char(code, pos);
    }
    ref[i] = '\0';
    for(i = 0; i < *sizealt; i++)
    {
        pos -= 5;
        alt[i] = decode_refalt_char(code, pos);
    }
    alt[i] = '\0';
    return (*sizeref + *sizealt);
}

size_t decode_refalt(uint32_t code, char *ref, size_t *sizeref, char *alt, size_t *sizealt)
{
    if (CHECK_BIT(code, 30))
    {
        return 0; // non reversible encoding
    }
    return decode_refalt_rev(code, ref, sizeref, alt, sizealt);
}

uint64_t variantkey(const char *chrom, size_t sizechrom, uint32_t pos, const char *ref, size_t sizeref, const char *alt, size_t sizealt)
{
    return (((uint64_t)encode_chrom(chrom, sizechrom) << 59) | ((uint64_t)pos << 31) | (uint64_t)encode_refalt(ref, sizeref, alt, sizealt));
}

size_t variantkey_string(uint64_t code, char *str, size_t size)
{
    size_t slen = 17; // = 16 hex chars + '\0'
    if (slen > size)
    {
        return slen;
    }
    char* it = str;
    const char* end = it + size;
    it += snprintf(it, (end - it), "%016"PRIx64"", code);
    return (end - it);
}

uint64_t parse_variantkey_string(const char *vs)
{
    uint64_t v = 0;
    uint8_t b;
    size_t i;
    for (i = 0; i < 16; i++)
    {
        b = vs[i];
        if (b >= 'a')
        {
            b -= ('a' - 10);
        }
        else
        {
            if (b >= 'A')
            {
                b -= ('A' - 10);
            }
            else
            {
                b -= '0';
            }
        }
        v = ((v << 4) | b);
    }
    return v;
}

variantkey_t decode_variantkey(uint64_t code)
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
