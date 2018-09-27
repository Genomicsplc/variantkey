// VariantKey
//
// test_rsidvar.c
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

// Test for rsidvar

#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include "../src/variantkey/rsidvar.h"

#define TEST_DATA_SIZE 10

typedef struct test_data_t
{
    uint8_t  chrom;
    uint32_t pos;
    uint32_t refalt;
    uint32_t rsid;
    uint64_t vk;
} test_data_t;

static const test_data_t test_data[TEST_DATA_SIZE] =
{
    {0X01, 0X0004F44B, 0X00338000, 0X00000001, 0X08027A2580338000},
    {0X09, 0X000143FC, 0X439E3918, 0X00000007, 0X4800A1FE439E3918},
    {0X09, 0X000143FC, 0X7555EB16, 0X0000000B, 0X4800A1FE7555EB16},
    {0X10, 0X000204E8, 0X003A0000, 0X00000061, 0X80010274003A0000},
    {0X10, 0X0002051A, 0X00138000, 0X00000065, 0X8001028D00138000},
    {0X10, 0X00020532, 0X007A0000, 0X000003E5, 0X80010299007A0000},
    {0X14, 0X000256C4, 0X003A0000, 0X000003F1, 0XA0012B62003A0000},
    {0X14, 0X000256C5, 0X00708000, 0X000026F5, 0XA0012B6280708000},
    {0X14, 0X000256CB, 0X63256692, 0X000186A3, 0XA0012B65E3256692},
    {0X14, 0X000256CF, 0X55439803, 0X00019919, 0XA0012B67D5439803},

};

// returns current time in nanoseconds
uint64_t get_time()
{
    struct timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return (((uint64_t)t.tv_sec * 1000000000) + (uint64_t)t.tv_nsec);
}

int test_find_rv_variantkey_by_rsid(rsidvar_cols_t crv)
{
    int errors = 0;
    int i;
    uint64_t vk;
    uint64_t first;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        first = 0;
        vk = find_rv_variantkey_by_rsid(crv, &first, crv.nrows, test_data[i].rsid);
        if (first != (uint64_t)i)
        {
            fprintf(stderr, "%s (%d) Expected first %d, got %" PRIu64 "\n", __func__, i, i, first);
            ++errors;
        }
        if (vk != test_data[i].vk)
        {
            fprintf(stderr, "%s (%d) Expected variantkey %" PRIx64 ", got %" PRIx64 "\n", __func__, i, test_data[i].vk, vk);
            ++errors;
        }
    }
    return errors;
}

int test_find_rv_variantkey_by_rsid_notfound(rsidvar_cols_t crv)
{
    int errors = 0;
    uint64_t vk;
    uint64_t first = 0;
    vk = find_rv_variantkey_by_rsid(crv, &first, crv.nrows, 0xfffffff0);
    if (first != 9)
    {
        fprintf(stderr, "%s : Expected first 9, got %" PRIu64 "\n", __func__, first);
        ++errors;
    }
    if (vk != 0)
    {
        fprintf(stderr, "%s : Expected variantkey 0, got %" PRIu64 "\n", __func__, vk);
        ++errors;
    }
    return errors;
}

int test_get_next_rv_variantkey_by_rsid(rsidvar_cols_t crv)
{
    int errors = 0;
    uint64_t pos = 2;
    uint64_t vk = get_next_rv_variantkey_by_rsid(crv, &pos, crv.nrows, 0X00000061);
    if (pos != 3)
    {
        fprintf(stderr, "%s (1 Expected) pos 3, got %" PRIu64 "\n", __func__, pos);
        ++errors;
    }
    if (vk != 0X80010274003A0000)
    {
        fprintf(stderr, "%s (1) Expected variantkey 0x80010274003A0000, got %" PRIx64 "\n", __func__, vk);
        ++errors;
    }
    vk = get_next_rv_variantkey_by_rsid(crv, &pos, crv.nrows, 0X00000061);
    if (pos != 4)
    {
        fprintf(stderr, "%s (2) Expected pos 4, got %" PRIu64 "\n", __func__, pos);
        ++errors;
    }
    if (vk != 0)
    {
        fprintf(stderr, "%s (2) Expected variantkey 0, got %" PRIx64 "\n", __func__, vk);
        ++errors;
    }
    return errors;
}

int test_find_vr_rsid_by_variantkey(rsidvar_cols_t cvr)
{
    int errors = 0;
    int i;
    uint32_t rsid;
    uint64_t first;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        first = 0;
        rsid = find_vr_rsid_by_variantkey(cvr, &first, cvr.nrows, test_data[i].vk);
        if (rsid != test_data[i].rsid)
        {
            fprintf(stderr, "%s (%d) Expected rsid %" PRIx32 ", got %" PRIx32 "\n",  __func__, i, test_data[i].rsid, rsid);
            ++errors;
        }
        if (first != (uint64_t)i)
        {
            fprintf(stderr, "%s (%d) Expected first %d, got %" PRIu64 "\n",  __func__, i, i, first);
            ++errors;
        }
    }
    return errors;
}

int test_find_vr_rsid_by_variantkey_notfound(rsidvar_cols_t cvr)
{
    int errors = 0;
    uint32_t rsid;
    uint64_t first;
    first = 0;
    rsid = find_vr_rsid_by_variantkey(cvr, &first, cvr.nrows, 0xfffffffffffffff0);
    if (rsid != 0)
    {
        fprintf(stderr, "%s : Expected rsid 0, got %" PRIx32 "\n",  __func__, rsid);
        ++errors;
    }
    if (first != 9)
    {
        fprintf(stderr, "%s :  Expected first 10, got %" PRIu64 "\n",  __func__, first);
        ++errors;
    }
    return errors;
}

int test_find_vr_chrompos_range(rsidvar_cols_t cvr)
{
    int errors = 0;
    uint32_t rsid;
    uint64_t first = 0;
    uint64_t last = 10;
    rsid = find_vr_chrompos_range(cvr, &first, &last, test_data[6].chrom, test_data[7].pos, test_data[8].pos);
    if (rsid != test_data[7].rsid)
    {
        fprintf(stderr, "%s : Expected rsid %" PRIx32 ", got %" PRIx32 "\n", __func__, test_data[7].rsid, rsid);
        ++errors;
    }
    if (first != 7)
    {
        fprintf(stderr, "%s : Expected first 7, got %" PRIu64 "\n", __func__, first);
        ++errors;
    }
    if (last !=9)
    {
        fprintf(stderr, "%s : Expected last 9, got %" PRIu64 "\n", __func__, last);
        ++errors;
    }
    return errors;
}

int test_find_vr_chrompos_range_notfound(rsidvar_cols_t cvr)
{
    int errors = 0;
    uint32_t rsid;
    uint64_t first = 0;
    uint64_t last = cvr.nrows;
    rsid = find_vr_chrompos_range(cvr, &first, &last, 0xff, 0xffffff00, 0xfffffff0);
    if (rsid != 0)
    {
        fprintf(stderr, "%s (1): Expected rsid 0, got %" PRIx32 "\n",  __func__, rsid);
        ++errors;
    }
    if (first != 10)
    {
        fprintf(stderr, "%s (1): Expected first 10, got %" PRIu64 "\n",  __func__, first);
        ++errors;
    }
    if (last != 10)
    {
        fprintf(stderr, "%s (1): Expected last 10, got %" PRIu64 "\n",  __func__, last);
        ++errors;
    }
    first = 0;
    last = cvr.nrows;
    rsid = find_vr_chrompos_range(cvr, &first, &last, 0, 0, 0);
    if (rsid != 0)
    {
        fprintf(stderr, "%s (2): Expected rsid 0, got %" PRIx32 "\n",  __func__, rsid);
        ++errors;
    }
    if (first != 10)
    {
        fprintf(stderr, "%s (2): Expected first 10, got %" PRIu64 "\n",  __func__, first);
        ++errors;
    }
    if (last != 10)
    {
        fprintf(stderr, "%s (2): Expected last 10, got %" PRIu64 "\n",  __func__, last);
        ++errors;
    }
    return errors;
}

void benchmark_find_rv_variantkey_by_rsid(rsidvar_cols_t crv)
{
    uint64_t tstart, tend;
    uint64_t first = 0;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        first = 0;
        find_rv_variantkey_by_rsid(crv, &first, crv.nrows, 0x2F81F575);
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/size);
}

void benchmark_find_vr_rsid_by_variantkey(rsidvar_cols_t cvr)
{
    uint64_t tstart, tend;
    uint64_t first = 0;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        first = 0;
        find_vr_rsid_by_variantkey(cvr, &first, cvr.nrows, 0X160017CCA313D0E0);
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/size);
}

void benchmark_find_vr_chrompos_range(rsidvar_cols_t cvr)
{
    uint64_t tstart, tend;
    uint64_t first = 0, last = 9;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        first = 0;
        last = 9;
        find_vr_chrompos_range(cvr, &first, &last, 0x19, 0x001AF8FD, 0x001C8F2A);
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/size);
}

int main()
{
    int errors = 0;
    int err;

    mmfile_t rv = {0};
    rsidvar_cols_t crv = {0};
    mmap_rsvk_file("rsvk.10.bin", &rv, &crv);
    if (rv.nrows != TEST_DATA_SIZE)
    {
        fprintf(stderr, "Expecting rsvk %d items, got instead: %" PRIu64 "\n", TEST_DATA_SIZE, rv.nrows);
        return 1;
    }

    mmfile_t vr = {0};
    rsidvar_cols_t cvr = {0};
    mmap_vkrs_file("vkrs.10.bin", &vr, &cvr);
    if (vr.nrows != TEST_DATA_SIZE)
    {
        fprintf(stderr, "Expecting vrsk %d items, got instead: %" PRIu64 "\n", TEST_DATA_SIZE, vr.nrows);
        return 1;
    }

    errors += test_find_rv_variantkey_by_rsid(crv);
    errors += test_find_rv_variantkey_by_rsid_notfound(crv);
    errors += test_get_next_rv_variantkey_by_rsid(crv);
    errors += test_find_vr_rsid_by_variantkey(cvr);
    errors += test_find_vr_rsid_by_variantkey_notfound(cvr);
    errors += test_find_vr_chrompos_range(cvr);
    errors += test_find_vr_chrompos_range_notfound(cvr);

    benchmark_find_rv_variantkey_by_rsid(crv);
    benchmark_find_vr_rsid_by_variantkey(cvr);
    benchmark_find_vr_chrompos_range(cvr);

    err = munmap_binfile(rv);
    if (err != 0)
    {
        fprintf(stderr, "Got %d error while unmapping the rv file\n", err);
        return 1;
    }

    err = munmap_binfile(vr);
    if (err != 0)
    {
        fprintf(stderr, "Got %d error while unmapping the vr file\n", err);
        return 1;
    }

    return errors;
}
