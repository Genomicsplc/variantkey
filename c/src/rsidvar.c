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

uint32_t get_vr_rsid(const unsigned char *src, uint64_t item)
{
    return bytes_be_to_uint32_t(src, get_address(BINBLKLEN, VRPOS_RSID, item));
}

uint64_t get_rv_variantkey(const unsigned char *src, uint64_t item)
{
    return bytes_be_to_uint64_t(src, get_address(BINBLKLEN, RVPOS_VK, item));
}

uint64_t find_rv_variantkey_by_rsid(const unsigned char *src, uint64_t *first, uint64_t last, uint32_t rsid)
{
    uint64_t max = last;
    uint64_t found = find_first_be_uint32_t(src, BINBLKLEN, RVPOS_RSID, first, &max, rsid);
    if (found > last)
    {
        return 0;
    }
    *first = found;
    return get_rv_variantkey(src, found);
}

uint64_t get_next_rv_variantkey_by_rsid(const unsigned char *src, uint64_t *pos, uint64_t last, uint32_t rsid)
{
    if (has_next_be_uint32_t(src, BINBLKLEN, RVPOS_RSID, pos, last, rsid))
    {
        return get_rv_variantkey(src, *pos);
    }
    return 0;
}

uint32_t find_vr_rsid_by_variantkey(const unsigned char *src, uint64_t *first, uint64_t last, uint64_t vk)
{
    uint64_t max = last;
    uint64_t found = find_first_be_uint64_t(src, BINBLKLEN, VRPOS_VK, first, &max, vk);
    if (found > last)
    {
        return 0; // not found
    }
    *first = found;
    return get_vr_rsid(src, found);
}

uint32_t find_vr_chrompos_range(const unsigned char *src, uint64_t *first, uint64_t *last, uint8_t chrom, uint32_t pos_min, uint32_t pos_max)
{
    uint64_t ckey = ((uint64_t)chrom << 59);
    uint64_t min = *first;
    uint64_t max = *last;
    *first = find_first_sub_be_uint64_t(src, BINBLKLEN, VRPOS_VK, 0, 32, &min, &max, (ckey | ((uint64_t)pos_min << 31)) >> 31);
    if (*first > *last)
    {
        *first = min;
    }
    else
    {
        min = *first;
    }
    if (min > *last)
    {
        return 0;
    }
    max = *last;
    uint64_t end = find_last_sub_be_uint64_t(src, BINBLKLEN, VRPOS_VK, 0, 32, &min, &max, (ckey | ((uint64_t)pos_max << 31)) >> 31);
    if (end > *last)
    {
        *last = max;
    }
    else
    {
        *last = end;
    }
    return get_vr_rsid(src, *first);
}
