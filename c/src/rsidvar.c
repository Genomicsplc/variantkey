// VariantKey
//
// rsidvar.c
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

#include "rsidvar.h"

void mmap_vkrs_file(const char *file, mmfile_t *mf, rsidvar_cols_t *cvr)
{
    mmap_binfile(file, mf);
    cvr->vk = (const uint64_t *)(mf->src + mf->index[0]);
    cvr->rs = (const uint32_t *)(mf->src + mf->index[1]);
    cvr->nrows = mf->nrows;
}

void mmap_rsvk_file(const char *file, mmfile_t *mf, rsidvar_cols_t *crv)
{
    mmap_binfile(file, mf);
    crv->rs = (const uint32_t *)(mf->src + mf->index[0]);
    crv->vk = (const uint64_t *)(mf->src + mf->index[1]);
    crv->nrows = mf->nrows;
}

uint64_t find_rv_variantkey_by_rsid(rsidvar_cols_t crv, uint64_t *first, uint64_t last, uint32_t rsid)
{
    uint64_t max = last;
    uint64_t found = col_find_first_uint32_t(crv.rs, first, &max, rsid);
    if (found >= last)
    {
        return 0;
    }
    *first = found;
    return *(crv.vk + found);
}

uint64_t get_next_rv_variantkey_by_rsid(rsidvar_cols_t crv, uint64_t *pos, uint64_t last, uint32_t rsid)
{
    if (col_has_next_uint32_t(crv.rs, pos, last, rsid))
    {
        return *(crv.vk + *pos);
    }
    return 0;
}

uint32_t find_vr_rsid_by_variantkey(rsidvar_cols_t cvr, uint64_t *first, uint64_t last, uint64_t vk)
{
    uint64_t max = last;
    uint64_t found = col_find_first_uint64_t(cvr.vk, first, &max, vk);
    if (found >= last)
    {
        return 0; // not found
    }
    *first = found;
    return *(cvr.rs + found);
}

uint32_t find_vr_chrompos_range(rsidvar_cols_t cvr, uint64_t *first, uint64_t *last, uint8_t chrom, uint32_t pos_min, uint32_t pos_max)
{
    uint64_t ckey = ((uint64_t)chrom << 59);
    uint64_t min = *first;
    uint64_t max = *last;
    *first = col_find_first_sub_uint64_t(cvr.vk, 0, 32, &min, &max, (ckey | ((uint64_t)pos_min << 31)) >> 31);
    if (*first >= *last)
    {
        *first = min;
    }
    else
    {
        min = *first;
    }
    if (min >= *last)
    {
        return 0;
    }
    max = *last;
    uint64_t end = col_find_last_sub_uint64_t(cvr.vk, 0, 32, &min, &max, (ckey | ((uint64_t)pos_max << 31)) >> 31);
    if (end >= *last)
    {
        *last = max;
    }
    else
    {
        *last = end;
    }
    return *(cvr.rs + *first);
}
