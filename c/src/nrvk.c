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

void mmap_nrvk_file(const char *file, mmfile_t *mf, nrvk_cols_t *nvc)
{
    mmap_binfile(file, mf);
    nvc->vk = (const uint64_t *)(mf->src + mf->index[0]);
    nvc->offset = (const uint64_t *)(mf->src + mf->index[1]);
    nvc->data = (const uint8_t *)(mf->src + mf->index[2]);
    nvc->nrows = mf->nrows;
}

static size_t get_nrvk_ref_alt_by_pos(nrvk_cols_t nvc, uint64_t pos, char *ref, size_t *sizeref, char *alt, size_t *sizealt)
{
    if (pos >= nvc.nrows)
    {
        return 0; // not found
    }
    const uint8_t *data = (nvc.data + *(nvc.offset + pos));
    *sizeref = (size_t)(*(data++));
    *sizealt = (size_t)(*(data++));
    memcpy(ref, data, *sizeref);
    ref[*sizeref] = 0;
    memcpy(alt, (data + *sizeref), *sizealt);
    alt[*sizealt] = 0;
    return (*sizeref + *sizealt);
}

size_t find_ref_alt_by_variantkey(nrvk_cols_t nvc, uint64_t vk, char *ref, size_t *sizeref, char *alt, size_t *sizealt)
{
    uint64_t first = 0;
    uint64_t max = nvc.nrows;
    uint64_t found = col_find_first_uint64_t(nvc.vk, &first, &max, vk);
    return get_nrvk_ref_alt_by_pos(nvc, found, ref, sizeref, alt, sizealt);
}

size_t reverse_variantkey(nrvk_cols_t nvc, uint64_t vk, variantkey_rev_t *rev)
{
    decode_chrom(extract_variantkey_chrom(vk), rev->chrom);
    rev->pos = extract_variantkey_pos(vk);
    size_t len = decode_refalt(extract_variantkey_refalt(vk), rev->ref, &rev->sizeref, rev->alt, &rev->sizealt);
    if ((len == 0) && (nvc.nrows > 0))
    {
        len = find_ref_alt_by_variantkey(nvc, vk, rev->ref, &rev->sizeref, rev->alt, &rev->sizealt);
    }
    return len;
}

size_t get_variantkey_ref_length(nrvk_cols_t nvc, uint64_t vk)
{
    if ((vk & 0x1) == 0) // check last bit for reversible encoding
    {
        return ((vk & 0x0000000078000000) >> 27); // [00000000 00000000 00000000 00000000 01111000 00000000 00000000 00000000]
    }
    uint64_t first = 0;
    uint64_t max = nvc.nrows;
    uint64_t found = col_find_first_uint64_t(nvc.vk, &first, &max, vk);
    if (found >= nvc.nrows)
    {
        return 0; // not found
    }
    return (size_t)(*(nvc.data + *(nvc.offset + found)));
}

uint32_t get_variantkey_endpos(nrvk_cols_t nvc, uint64_t vk)
{
    return (extract_variantkey_pos(vk) + (uint32_t)get_variantkey_ref_length(nvc, vk));
}

uint64_t get_variantkey_chrom_startpos(uint64_t vk)
{
    return (vk >> VKSHIFT_POS);
}

uint64_t get_variantkey_chrom_endpos(nrvk_cols_t nvc, uint64_t vk)
{
    return (((vk & VKMASK_CHROM) >> VKSHIFT_POS) | (uint64_t)get_variantkey_endpos(nvc, vk));
}

size_t nrvk_bin_to_tsv(nrvk_cols_t nvc, const char *tsvfile)
{
    FILE * fp;
    size_t sizeref, sizealt, len = 0;
    char ref[ALLELE_MAXSIZE];
    char alt[ALLELE_MAXSIZE];
    uint64_t i;
    fp = fopen(tsvfile, "we");
    if (fp == NULL)
    {
        return 0;
    }
    for (i = 0; i < nvc.nrows; i++)
    {
        len += (get_nrvk_ref_alt_by_pos(nvc, i, ref, &sizeref, alt, &sizealt) + 19);
        fprintf(fp, "%016" PRIx64 "\t%s\t%s\n", *nvc.vk++, ref, alt);
    }
    fclose(fp);
    return len;
}
