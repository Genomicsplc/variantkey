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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "variantkey.h"


int aztoupper(int c)
{
    if ((c >= 'a') && (c <= 'z'))
    {
        return (c ^ 32);
    }
    return c;
}

uint32_t encode_assembly_32bit(const char *assembly)
{
    return farmhash32(assembly, strlen(assembly));
}

uint32_t encode_chrom_32bit(const char *chrom)
{
    uint32_t h;
    // remove "chr" prefix
    if ((strlen(chrom) > 3)
            && ((chrom[0] == 'C') || (chrom[0] == 'c'))
            && ((chrom[1] == 'H') || (chrom[1] == 'h'))
            && ((chrom[2] == 'R') || (chrom[2] == 'r')))
    {
        chrom += 3;
    }
    char *endptr;
    h = (uint32_t)strtoul(chrom, &endptr, 10);
    if (*endptr == '\0')
    {
        return h; // numerical chromosome
    }
    // HUMAN
    if ((chrom[0] == 'X') || (chrom[0] == 'x'))
    {
        return 23;
    }
    if ((chrom[0] == 'Y') || (chrom[0] == 'y'))
    {
        return 24;
    }
    if ((chrom[0] == 'M') || (chrom[0] == 'm'))
    {
        return 25;
    }
    return 0; // NA
}

uint8_t encode_chrom_8bit(const char *chrom)
{
    return (uint8_t)(encode_chrom_32bit(chrom) & 0x000000FF);
}

size_t decode_chrom_32bit(uint32_t code, char *chrom)
{
    if ((code < 1) || (code > 25))
    {
        return sprintf(chrom, "NA");
    }
    if (code < 23)
    {
        return sprintf(chrom, "%"PRIu32, code);
    }
    const char *map[] = {"X", "Y", "MT"};
    return sprintf(chrom, "%s", map[(code - 23)]);
}

size_t decode_chrom_8bit(uint8_t code, char *chrom)
{
    return decode_chrom_32bit((uint32_t)code, chrom);
}

uint32_t encode_hash_refalt_32bit(const char *ref, const char *alt, size_t slen)
{
    char ra[slen + 2];
    char *it = ra;
    while ((*it++ = aztoupper(*ref++))) {};
    it[-1] = '_';
    while ((*it++ = aztoupper(*alt++))) {};
    return (farmhash32(ra, slen + 1) | 0x80000000);
}

uint32_t encode_hash_refalt_24bit(const char *ref, const char *alt, size_t slen)
{
    return (encode_hash_refalt_32bit(ref, alt, slen) >> 8);
}

void encode_refalt_str(uint32_t *h, uint8_t *pos, const char *str)
{
    int c;
    while ((c = aztoupper(*str++)))
    {
        if (c == '*')
        {
            c = 91;
        }
        *pos -= 5;
        *h |= (((c - 64) & 0x1F) << *pos);
    };
}

uint32_t encode_rev_refalt(const char *ref, const char *alt, size_t lenref, uint8_t pos)
{
    uint32_t h = 0;
    if (lenref > 1)
    {
        h = (uint32_t)(lenref - 1) << pos; // length of REF - 1
    }
    encode_refalt_str(&h, &pos, ref);
    encode_refalt_str(&h, &pos, alt);
    return h;
}

uint32_t encode_refalt_32bit(const char *ref, const char *alt)
{
    // pos = floor((32 bit - 2 bit for REF-1 lenght - 1 bit for hash/rev) / 5 bit for each nucleotide) * 5 bit for each nucleotide;
    size_t lenref = strlen(ref);
    size_t slen = lenref + strlen(alt);
    if (slen > 5)
    {
        return encode_hash_refalt_32bit(ref, alt, slen);
    }
    return encode_rev_refalt(ref, alt, lenref, 25);
}

uint32_t encode_refalt_24bit(const char *ref, const char *alt)
{
    // pos = floor((24 bit - 2 bit for REF-1 lenght - 1 bit for hash/rev) / 5 bit for each nucleotide) * 5 bit for each nucleotide;
    size_t lenref = strlen(ref);
    size_t slen = lenref + strlen(alt);
    if (slen > 4)
    {
        return encode_hash_refalt_24bit(ref, alt, slen);
    }
    return encode_rev_refalt(ref, alt, lenref, 20);
}

char decode_refalt_char(uint32_t code, int pos)
{
    char c = ((code >> pos) & 0x1F);
    if (c == 27)
    {
        return '*';
    }
    if (c > 0)
    {
        return (c + 64);
    }
    return c;
}

size_t decode_refalt(uint32_t code, char *ref, char *alt, uint8_t pos)
{
    size_t lenref = 1 + (code >> pos);
    size_t i = 0;
    for(i = 0; i < lenref; i++)
    {
        pos -= 5;
        ref[i] = decode_refalt_char(code, pos);
    }
    ref[i] = 0;
    i = 0;
    while (pos >= 5)
    {
        pos -= 5;
        alt[i] = decode_refalt_char(code, pos);
        if (alt[i] == 0)
        {
            return i;
        }
        i++;
    }
    alt[i] = 0;
    return i;
}

size_t decode_refalt_32bit(uint32_t code, char *ref, char *alt)
{
    if ((code & 0x80000000) > 0)
    {
        return 0; // non reversible encoding
    }
    return decode_refalt(code, ref, alt, 25);
}

size_t decode_refalt_24bit(uint32_t code, char *ref, char *alt)
{
    if ((code & 0x800000) > 0)
    {
        return 0; // non reversible encoding
    }
    return decode_refalt(code, ref, alt, 20);
}

variantkey128_t variantkey128(const char *assembly, const char *chrom, uint32_t pos, const char *ref, const char *alt)
{
    return (variantkey128_t)
    {
        encode_assembly_32bit(assembly), encode_chrom_32bit(chrom), pos, encode_refalt_32bit(ref, alt)
    };
}

uint64_t variantkey64(const char *chrom, uint32_t pos, const char *ref, const char *alt)
{
    return (((uint64_t)encode_chrom_8bit(chrom) << 56) | ((uint64_t)pos << 24) | (uint64_t)encode_refalt_24bit(ref, alt));
}

size_t variantkey128_string(char *str, size_t size, variantkey128_t vh)
{
    size_t slen = 33; // = 32 hex chars + '\0'
    if (slen > size)
    {
        return slen;
    }
    char* it = str;
    const char* end = it + size;
    it += snprintf(it, (end - it), "%08"PRIx32"%08"PRIx32"%08"PRIx32"%08"PRIx32"", vh.assembly, vh.chrom, vh.pos, vh.refalt);
    return (end - it);
}

size_t variantkey64_string(char *str, size_t size, uint64_t vh)
{
    size_t slen = 17; // = 16 hex chars + '\0'
    if (slen > size)
    {
        return slen;
    }
    char* it = str;
    const char* end = it + size;
    it += snprintf(it, (end - it), "%016"PRIx64"", vh);
    return (end - it);
}

variantkey128_t parse_variantkey128_string(const char *vs)
{
    variantkey128_t vh = {0,0,0,0};
    size_t slen = strlen(vs);
    if (slen != 32)
    {
        return vh;
    }
    char tmp[33];
    strncpy(tmp, vs, slen);
    tmp[32] = 0;
    char* it = tmp + 24;
    vh.refalt = (uint32_t)strtoul(it, NULL, 16);
    *it = 0;
    it = tmp + 16;
    vh.pos = (uint32_t)strtoul(it, NULL, 16);
    *it = 0;
    it = tmp + 8;
    vh.chrom = (uint32_t)strtoul(it, NULL, 16);
    *it = 0;
    vh.assembly = (uint32_t)strtoul(tmp, NULL, 16);
    return vh;
}

uint64_t parse_variantkey64_string(const char *vs)
{
    if (strlen(vs) != 16)
    {
        return 0;
    }
    return (uint64_t)strtoull(vs, NULL, 16);
}

variantkey64_t split_variantkey64(uint64_t code)
{
    variantkey64_t vh = {0,0,0};
    vh.chrom = (uint8_t)((code & 0xFF00000000000000) >> 56);
    vh.pos = (uint32_t)((code & 0x00FFFFFFFF000000) >> 24);
    vh.refalt = (uint32_t)(code & 0x0000000000FFFFFF);
    return vh;
}

