// VariantHash
//
// varianthash.c
//
// @category   Tools
// @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
// @copyright  2017 GENOMICS plc
// @license    MIT (see LICENSE)
// @link       https://github.com/genomicsplc/varianthash
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
// VariantHash by Nicola Asuni

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "varianthash.h"

const char *chrom_str_map[] = {"X", "Y", "MT", "XX", "XY"};

int aztoupper(int c)
{
    if ((c >= 'a') && (c <= 'z'))
    {
        return (c ^ 32);
    }
    return c;
}

uint32_t encode_assembly(const char *assembly)
{
    return farmhash32(assembly, strlen(assembly));
}

uint32_t encode_chrom(const char *chrom)
{
    uint32_t h;
    size_t slen = strlen(chrom);
    // remove "chr" prefix
    if ((slen > 3)
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
        if (slen > 1)
        {
            if ((chrom[1] == 'X') || (chrom[1] == 'X'))
            {
                return 26;
            }
            if ((chrom[1] == 'Y') || (chrom[1] == 'y'))
            {
                return 27;
            }
            return 25;
        }
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
    return 0;
}

size_t decode_chrom(uint32_t code, char *chrom)
{
    if (code < 23)
    {
        return sprintf(chrom, "%"PRIu32, code);
    }
    return sprintf(chrom, "%s", chrom_str_map[(code - 23)]);
}

uint32_t encode_hash_refalt(const char *ref, const char *alt, size_t slen)
{
    char ra[slen + 2];
    char *it = ra;
    while ((*it++ = aztoupper(*ref++))) {};
    it[-1] = '_';
    while ((*it++ = aztoupper(*alt++))) {};
    return (farmhash32(ra, slen + 1) | 0x10000000);
}

void encode_refalt_str(uint32_t *h, int *pos, const char *str)
{
    int c;
    while ((c = aztoupper(*str++)))
    {
        if (c == '*') {
            c = 91;
        }
        *h |= (((c - 64) & 0x1f) << *pos);
        *pos -= 5;
    };
}

uint32_t encode_rev_refalt(const char *ref, const char *alt, size_t lenref)
{
    uint32_t h = 0;
    if (lenref > 1)
    {
        h = (uint32_t)(lenref - 1) << 25; // bits 5 and 6
    }
    int pos = 20;
    encode_refalt_str(&h, &pos, ref);
    encode_refalt_str(&h, &pos, alt);
    return h;
}

uint32_t encode_ref_alt(const char *ref, const char *alt)
{
    size_t lenref = strlen(ref);
    size_t slen = lenref + strlen(alt);
    if (slen > 5)
    {
        return encode_hash_refalt(ref, alt, slen);
    }
    return encode_rev_refalt(ref, alt, lenref);
}

char decode_refalt_char(uint32_t code, int pos)
{
    char c = ((code >> pos) & 0x1f);
    if (c == 27) {
        return '*';
    }
    if (c > 0)
    {
        return (c + 64);
    }
    return c;
}

size_t decode_ref_alt(uint32_t code, char *ref, char *alt)
{
    if ((code & 0xf0000000) > 0)
    {
        return 0; // non reversible encoding
    }
    size_t lenref = 1 + (code >> 25);
    int pos = 20;
    size_t i = 0;
    for(i = 0; i < lenref; i++)
    {
        ref[i] = decode_refalt_char(code, pos);
        pos -= 5;
    }
    ref[i] = 0;
    i = 0;
    while (pos >= 0)
    {
        alt[i] = decode_refalt_char(code, pos);
        pos -= 5;
        i++;
    }
    alt[i] = 0;
    return i;
}

varhash_t variant_hash(const char *assembly, const char *chrom, uint32_t pos, const char *ref, const char *alt)
{
    return (varhash_t)
    {
        encode_assembly(assembly), encode_chrom(chrom), pos, encode_ref_alt(ref, alt)
    };
}

size_t variant_hash_string(char *str, size_t size, varhash_t vh)
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

varhash_t decode_variant_hash_string(const char *vs)
{
    varhash_t vh = {0,0,0,0};
    size_t slen = strlen(vs);
    if (slen != 32)
    {
        return vh;
    }
    char tmp[33];
    strncpy(tmp, vs, slen);
    tmp[32] = 0;
    char* it = tmp + 24;
    vh.refalt = (uint32_t)strtoull(it, NULL, 16);
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
