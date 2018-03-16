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

#include "rsidvar64.h"

uint32_t get_vr64_rsid(const unsigned char *src, uint64_t item)
{
    return bytes_to_uint32_t(src, get_address(RSIDVAR64_BIN_BLKLEN, VARRSID64_BPOS_RSID, item));
}

uint64_t get_rv64_varhash(const unsigned char *src, uint64_t item)
{
    uint64_t i = get_address(RSIDVAR64_BIN_BLKLEN, 0, item);
    return bytes_to_uint64_t(src, i + RSIDVAR64_BPOS_CHROM);
}

uint64_t find_rv64_varhash_by_rsid(const unsigned char *src, uint64_t *first, uint64_t last, uint32_t rsid)
{
    uint64_t max = last;
    uint64_t found = find_first_uint32_t(src, RSIDVAR64_BIN_BLKLEN, 0, first, &max, rsid);
    if (found > last)
    {
        return 0;
    }
    *first = found;
    return get_rv64_varhash(src, found);
}

uint32_t find_vr64_rsid_by_varhash(const unsigned char *src, uint64_t *first, uint64_t last, uint64_t vh)
{
    uint64_t max = last;
    uint64_t found = find_first_uint64_t(src, RSIDVAR64_BIN_BLKLEN, 0, first, &max, vh);
    if (found > last)
    {
        return 0; // not found
    }
    *first = found;
    return get_vr64_rsid(src, found);
}

uint32_t find_vr64_chrom_range(const unsigned char *src, uint64_t *first, uint64_t *last, uint8_t chrom)
{
    uint64_t min = *first;
    uint64_t max = *last;
    *first = find_first_uint8_t(src, RSIDVAR64_BIN_BLKLEN, VARRSID64_BPOS_CHROM, &min, &max, chrom);
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
    uint64_t end = find_last_uint8_t(src, RSIDVAR64_BIN_BLKLEN, VARRSID64_BPOS_CHROM, &min, &max, chrom);
    if (end > *last)
    {
        *last = max;
    }
    else
    {
        *last = end;
    }
    return get_vr64_rsid(src, *first);
}

uint32_t find_vr64_pos_range(const unsigned char *src, uint64_t *first, uint64_t *last, uint32_t pos_start, uint32_t pos_end)
{
    uint64_t min = *first;
    uint64_t max = *last;
    *first = find_first_uint32_t(src, RSIDVAR64_BIN_BLKLEN, VARRSID64_BPOS_POS, &min, &max, pos_start);
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
    uint64_t end = find_last_uint32_t(src, RSIDVAR64_BIN_BLKLEN, VARRSID64_BPOS_POS, &min, &max, pos_end);
    if (end > *last)
    {
        *last = max;
    }
    else
    {
        *last = end;
    }
    return get_vr64_rsid(src, *first);
}

uint32_t find_vr64_chrompos_range(const unsigned char *src, uint64_t *first, uint64_t *last, uint8_t chrom, uint32_t pos_start, uint32_t pos_end)
{
    uint64_t item = find_vr64_chrom_range(src, first, last, chrom);
    if (item == 0)
    {
        return 0;
    }
    return find_vr64_pos_range(src, first, last, pos_start, pos_end);
}
