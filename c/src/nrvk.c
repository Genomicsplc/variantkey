// VariantKey
//
// nrvk.c
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
#include <string.h>
#include "nrvk.h"

#define KEYBLKLEN 16 //!< Length in bytes of a binary block containing VARIANTKEY + OFFSET ADDRESS
#define ADDRBLKPOS 8 //!< Position of the OFFSET ADDRESS in bytes in the binary block

size_t find_ref_alt_by_variantkey(const unsigned char *src, uint64_t last, uint64_t vk, char *ref, size_t *sizeref, char *alt, size_t *sizealt)
{
    uint64_t first = 0;
    uint64_t max = last;
    uint64_t found = find_first_uint64_t(src, KEYBLKLEN, 0, &first, &max, vk);
    if (found > last)
    {
        return 0; // not found
    }
    uint64_t offset = bytes_to_uint64_t(src, get_address(KEYBLKLEN, ADDRBLKPOS, found));
    *sizeref = (size_t) bytes_to_uint8_t(src, offset++);
    *sizealt = (size_t) bytes_to_uint8_t(src, offset++);
    memcpy(ref, &src[offset], *sizeref);
    ref[*sizeref] = 0;
    memcpy(alt, &src[(offset + *sizeref)], *sizealt);
    alt[*sizealt] = 0;
    return (*sizeref + *sizealt);
}

size_t reverse_variantkey(const unsigned char *src, uint64_t last, uint64_t vk, variantkey_rev_t *rev)
{
    variantkey_t h = {0,0,0};
    decode_variantkey(vk, &h);
    decode_chrom(h.chrom, rev->chrom);
    rev->pos = h.pos;
    size_t len = decode_refalt(h.refalt, rev->ref, &rev->sizeref, rev->alt, &rev->sizealt);
    if ((len == 0) && (last > 0))
    {
        len = find_ref_alt_by_variantkey(src, last, vk, rev->ref, &rev->sizeref, rev->alt, &rev->sizealt);
    }
    return len;
}

size_t get_variantkey_ref_length(const unsigned char *src, uint64_t last, uint64_t vk)
{
    if ((vk & 0x1) == 0) // check last bit for reversible encoding
    {
        return (size_t)((vk & 0x0000000078000000) >> 27); // [00000000 00000000 00000000 00000000 01111000 00000000 00000000 00000000]
    }
    uint64_t first = 0;
    uint64_t max = last;
    uint64_t found = find_first_uint64_t(src, KEYBLKLEN, 0, &first, &max, vk);
    if (found > last)
    {
        return 0; // not found
    }
    return (size_t) bytes_to_uint8_t(src, bytes_to_uint64_t(src, get_address(KEYBLKLEN, ADDRBLKPOS, found)));
}

uint32_t get_variantkey_endpos(const unsigned char *src, uint64_t last, uint64_t vk)
{
    return (extract_variantkey_pos(vk) + (uint32_t)get_variantkey_ref_length(src, last, vk));
}

uint64_t get_variantkey_chrom_startpos(uint64_t vk)
{
    return (vk >> VKSHIFT_POS);
}

uint64_t get_variantkey_chrom_endpos(const unsigned char *src, uint64_t last, uint64_t vk)
{
    return (((vk & VKMASK_CHROM) >> VKSHIFT_POS) | (uint64_t)get_variantkey_endpos(src, last, vk));
}

size_t vknr_bin_to_tsv(const unsigned char *src, uint64_t last, const char *tsvfile)
{
    FILE * fp;
    uint64_t vk, pos, offset;
    size_t sizeref, sizealt, len = 0;
    char ref[ALLELE_MAXSIZE];
    char alt[ALLELE_MAXSIZE];
    uint64_t i;
    fp = fopen(tsvfile, "we");
    if (fp == NULL)
    {
        return 0;
    }
    for (i = 0; i <= last; i++)
    {
        pos = get_address(KEYBLKLEN, 0, i);
        vk = bytes_to_uint64_t(src, pos);
        offset = bytes_to_uint64_t(src, pos + 8);
        sizeref = (size_t) bytes_to_uint8_t(src, offset++);
        sizealt = (size_t) bytes_to_uint8_t(src, offset++);
        memcpy(ref, &src[offset], sizeref);
        ref[sizeref] = 0;
        memcpy(alt, &src[(offset + sizeref)], sizealt);
        alt[sizealt] = 0;
        fprintf(fp, "%016" PRIx64 "\t%s\t%s\n", vk, ref, alt);
        len += (16 + 1 + sizeref + 1 + sizealt + 1);
    }
    fclose(fp);
    return len;
}
