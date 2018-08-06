// VariantKey
//
// regionkey.c
//
// @category   Libraries
// @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
// @copyright  2017-2018 GENOMICS plc
// @license    MIT (see LICENSE)
// @link       https://github.com/genomicsplc/regionkey
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
#include "regionkey.h"

uint8_t encode_strand(int8_t strand)
{
    static const uint8_t map[] = {2, 0, 1, 0};
    return map[((uint8_t)(strand + 1) & 3)];
}

int8_t decode_strand(uint8_t code)
{
    static const int8_t map[] = {0, 1, -1, 0};
    return map[(code & 3)];
}

uint64_t encode_regionkey(uint8_t chrom, uint32_t startpos, uint32_t endpos, uint8_t strand)
{
    return (((uint64_t)chrom << RKSHIFT_CHROM) | ((uint64_t)startpos << RKSHIFT_STARTPOS) | ((uint64_t)endpos << RKSHIFT_ENDPOS) | ((uint64_t)strand << RKSHIFT_STRAND));
}

uint8_t extract_regionkey_chrom(uint64_t rk)
{
    return (uint8_t)((rk & RKMASK_CHROM) >> RKSHIFT_CHROM);
}

uint32_t extract_regionkey_startpos(uint64_t rk)
{
    return (uint32_t)((rk & RKMASK_STARTPOS) >> RKSHIFT_STARTPOS);
}

uint32_t extract_regionkey_endpos(uint64_t rk)
{
    return (uint32_t)((rk & RKMASK_ENDPOS) >> RKSHIFT_ENDPOS);
}

uint8_t extract_regionkey_strand(uint64_t rk)
{
    return (uint8_t)((rk & RKMASK_STRAND) >> RKSHIFT_STRAND);
}

void decode_regionkey(uint64_t code, regionkey_t *rk)
{
    rk->chrom = extract_regionkey_chrom(code);
    rk->startpos = extract_regionkey_startpos(code);
    rk->endpos = extract_regionkey_endpos(code);
    rk->strand = extract_regionkey_strand(code);
}

void reverse_regionkey(uint64_t rk, regionkey_rev_t *rev)
{
    regionkey_t h = {0,0,0,0};
    decode_regionkey(rk, &h);
    decode_chrom(h.chrom, rev->chrom);
    rev->startpos = h.startpos;
    rev->endpos = h.endpos;
    rev->strand = decode_strand(h.strand);
}

uint64_t regionkey(const char *chrom, size_t sizechrom, uint32_t startpos, uint32_t endpos, int8_t strand)
{
    return encode_regionkey(encode_chrom(chrom, sizechrom), startpos, endpos, encode_strand(strand));
}

size_t regionkey_hex(uint64_t rk, char *str)
{
    return hex_uint64_t(rk, str);
}

uint64_t parse_regionkey_hex(const char *rs)
{
    return parse_hex_uint64_t(rs);
}
