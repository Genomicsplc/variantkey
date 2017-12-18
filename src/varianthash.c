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
// Copyright (c) 2017 GENOMICS plc
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

int aztoupper(int c)
{
    if ((c >= 0x61) && (c <= 0x7A)) return (c ^ 0x20);
    return c;
}

uint32_t encode_assembly(const char *assembly)
{
    return farmhash32(assembly, strlen(assembly));
}

// @TODO: support other species
uint32_t encode_chrom(const char *chrom)
{
    uint32_t h;
    char *endptr;
    h = (uint32_t)strtoul(chrom, &endptr, 10);
    if (*endptr == '\0') return h; // numerical chromosome
    // HUMAN
    if ((chrom[0] == 'X') || (chrom[0] == 'x'))
    {
        if ((chrom[1] == 'Y') || (chrom[1] == 'y')) return 25;
        return 23;
    }
    if ((chrom[0] == 'Y') || (chrom[0] == 'y')) return 24;
    if ((chrom[0] == 'M') || (chrom[0] == 'm')) return 26;
    return 0;
}

uint32_t encode_ref_alt(const char *ref, const char *alt)
{
    size_t slen = 1 + strlen(ref) + strlen(alt);
    char ra[slen + 1];
    char *it = ra;
    while ((*it++ = aztoupper(*ref++)));
    it[-1] = '_';
    while ((*it++ = aztoupper(*alt++)));
    return farmhash32(ra, slen);
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
    if (slen > size) return slen;
    char* it = str;
    const char* end = it + size;
    it += snprintf(it, (end - it), "%08"PRIx32"%08"PRIx32"%08"PRIx32"%08"PRIx32"", vh.assembly, vh.chrom, vh.pos, vh.refalt);
    return (end - it);
}

varhash_t decode_variant_hash_string(const char *vs)
{
    varhash_t vh = {0,0,0,0};
    size_t slen = strlen(vs);
    if (slen != 32) return vh;
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
