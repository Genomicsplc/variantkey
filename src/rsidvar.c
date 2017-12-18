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

#include "rsidvar.h"

uint32_t get_vr_rsid(const unsigned char *src, uint64_t item)
{
    return bytes_to_uint32_t(src, get_address(RSIDVAR_BIN_BLKLEN, VARRSID_BPOS_RSID, item));
}

varhash_t get_rv_varhash(const unsigned char *src, uint64_t item)
{
    uint64_t i = get_address(RSIDVAR_BIN_BLKLEN, 0, item);
    return (varhash_t)
    {
        .assembly = bytes_to_uint32_t(src, i + RSIDVAR_BPOS_ASSBLY),
         .chrom = bytes_to_uint32_t(src, i + RSIDVAR_BPOS_CHROM),
          .pos = bytes_to_uint32_t(src, i + RSIDVAR_BPOS_POS),
           .refalt = bytes_to_uint32_t(src, i + RSIDVAR_BPOS_REFALT)
    };
}

varhash_t find_rv_varhash_by_rsid(const unsigned char *src, uint64_t *first, uint64_t *last, uint32_t rsid)
{
    *first = find_first_uint32_t(src, RSIDVAR_BIN_BLKLEN, 0, *first, *last, rsid);
    if (*first > last) return (varhash_t)
    {
        .assembly = 0, .chrom = 0, .pos = 0, .refalt = 0
    };
    return get_rv_varhash(src, *first);
}

uint32_t find_vr_chrompos_range(const unsigned char *src, uint64_t *first, uint64_t *last, uint32_t chrom, uint32_t pos_start, uint32_t pos_end)
{
    *first = find_first_uint64_t(src, RSIDVAR_BIN_BLKLEN, VARRSID_BPOS_CHROM, *first, *last, ((uint64_t)chrom << 32 | (uint64_t)pos_start));
    if (*first > *last) return 0;
    *last = find_last_uint64be(src, RSIDVAR_BIN_BLKLEN, VARRSID_BPOS_CHROM, *first, *last, ((uint64_t)chrom << 32 | (uint64_t)pos_end));
    return get_vr_rsid(src, *first);
}

uint32_t find_vr_rsid_by_varhash(const unsigned char *src, uint64_t *first, uint64_t *last, varhash_t vh)
{
    *first = find_first_uint128_t(src, RSIDVAR_BIN_BLKLEN, 0, *first, *last, uint128_t{(uint64_t)vh.assembly << 32 | (uint64_t)vh.chrom), ((uint64_t)vh.pos << 32 | (uint64_t)vh.refalt)};
    if (*first > last) return 0; // not found
    return get_vr_rsid(src, *first);
}
