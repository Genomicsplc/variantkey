// Benchmark tool for rsidvar
//
// rsidvar_bench.c
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
//
// VariantKey by Nicola Asuni

// NOTE: This test is slow because it generates the test files from scratch.

#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include "../../src/rsidvar.h"

#define TEST_DATA_SIZE 400000000UL

// returns current time in nanoseconds
uint64_t get_time()
{
    struct timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return (((uint64_t)t.tv_sec * 1000000000) + (uint64_t)t.tv_nsec);
}

int benchmark_find_rv_variantkey_by_rsid()
{
    const char *filename = "rsvk_test_400M.bin";

    uint32_t i;

    FILE *f = fopen(filename, "we");
    if (f == NULL)
    {
        fprintf(stderr, " * %s Unable to open %s file in writing mode.\n", __func__, filename);
        return 1;
    }
    unsigned char b0, b1, b2, b3, z = 0;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        b0 = i & 0xFF;
        b1 = (i >> 8) & 0xFF;
        b2 = (i >> 16) & 0xFF;
        b3 = (i >> 24) & 0xFF;
        fprintf(f, "%c%c%c%c%c%c%c%c%c%c%c%c", b3, b2, b1, b0, z, z, z, z, b3, b2, b1, b0);
    }
    fclose(f);

    mmfile_t rv = {0};
    mmap_binfile(filename, &rv);
    uint64_t nitems = (rv.size / BINBLKLEN);
    if (nitems != TEST_DATA_SIZE)
    {
        fprintf(stderr, " * %s Expecting rsvk_test_400M.bin %" PRIu64 " items, got instead: %" PRIu64 "\n", __func__, TEST_DATA_SIZE, nitems);
        return 1;
    }

    uint64_t tstart, tend, offset;
    volatile uint64_t sum = 0;
    uint64_t first;

    tstart = get_time();
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        sum += i;
    }
    tend = get_time();
    offset = (tend - tstart);
    fprintf(stdout, " * %s sum: %" PRIu64 "\n", __func__, sum);

    int j;
    for (j=0 ; j < 3; j++)
    {
        sum = 0;
        tstart = get_time();
        for (i=0 ; i < TEST_DATA_SIZE; i++)
        {
            first = 0;
            sum += find_rv_variantkey_by_rsid(rv.src, &first, TEST_DATA_SIZE, i);
        }
        tend = get_time();
        fprintf(stdout, "   * %s %d. sum: %" PRIu64 " -- time: %" PRIu64 " ns -- %" PRIu64 " ns/op\n", __func__, j, sum, (tend - tstart - offset), (tend - tstart - offset)/TEST_DATA_SIZE);
    }
    return 0;
}

int benchmark_find_vr_rsid_by_variantkey()
{
    const char *filename = "vkrs_test_400M.bin";

    uint64_t i;

    FILE *f = fopen(filename, "we");
    if (f == NULL)
    {
        fprintf(stderr, " * %s Unable to open %s file in writing mode.\n", __func__, filename);
        return 1;
    }
    unsigned char b0, b1, b2, b3, z = 0;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        b0 = i & 0xFF;
        b1 = (i >> 8) & 0xFF;
        b2 = (i >> 16) & 0xFF;
        b3 = (i >> 24) & 0xFF;
        fprintf(f, "%c%c%c%c%c%c%c%c%c%c%c%c", z, z, z, z, b3, b2, b1, b0, b3, b2, b1, b0);
    }
    fclose(f);

    mmfile_t vr = {0};
    mmap_binfile(filename, &vr);
    uint64_t nitems = (vr.size / BINBLKLEN);
    if (nitems != TEST_DATA_SIZE)
    {
        fprintf(stderr, " * %s Expecting vkrs_test_400M.bin %" PRIu64 " items, got instead: %" PRIu64 "\n", __func__, TEST_DATA_SIZE, nitems);
        return 1;
    }

    uint64_t tstart, tend, offset;
    volatile uint64_t sum = 0;
    uint64_t first;

    tstart = get_time();
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        sum += i;
    }
    tend = get_time();
    offset = (tend - tstart);
    fprintf(stdout, " * %s sum: %" PRIu64 "\n", __func__, sum);

    int j;
    for (j=0 ; j < 3; j++)
    {
        sum = 0;
        tstart = get_time();
        for (i=0 ; i < TEST_DATA_SIZE; i++)
        {
            first = 0;
            sum += find_vr_rsid_by_variantkey(vr.src, &first, TEST_DATA_SIZE, i);
        }
        tend = get_time();
        fprintf(stdout, "   * %s %d. sum: %" PRIu64 " -- time: %" PRIu64 " ns -- %" PRIu64 " ns/op\n", __func__, j, sum, (tend - tstart - offset), (tend - tstart - offset)/TEST_DATA_SIZE);
    }
    return 0;
}

int main()
{
    int ret = 0;
    ret += benchmark_find_rv_variantkey_by_rsid();
    ret += benchmark_find_vr_rsid_by_variantkey();
    return ret;
}
