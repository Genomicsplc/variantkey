// VariantKey
//
// test_regionkey.c
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

// Test for regionkey

#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <sys/mman.h>
#include "../src/regionkey.h"

#define TEST_DATA_SIZE 10
#define TEST_OVERLAP_SIZE 12

typedef struct test_data_t
{
    const char* chrom;
    uint32_t startpos;
    uint32_t endpos;
    int8_t strand;
    uint8_t echrom;
    uint8_t estrand;
    uint64_t rk;
    const char* rs;
    uint64_t chrom_startpos;
    uint64_t chrom_endpos;
} test_data_t;

static test_data_t test_data[TEST_DATA_SIZE] =
{
    { "1", 1000, 1100,  0,  1, 0, 0x080001f400002260, "080001f400002260", 0x00000000100003e8, 0x000000001000044c},
    { "2", 1001, 1201,  1,  2, 1, 0x100001f48000258a, "100001f48000258a", 0x00000000200003e9, 0x00000000200004b1},
    { "3", 1002, 1302, -1,  3, 2, 0x180001f5000028b4, "180001f5000028b4", 0x00000000300003ea, 0x0000000030000516},
    { "4", 1003, 1403,  0,  4, 0, 0x200001f580002bd8, "200001f580002bd8", 0x00000000400003eb, 0x000000004000057b},
    { "5", 1004, 1504,  1,  5, 1, 0x280001f600002f02, "280001f600002f02", 0x00000000500003ec, 0x00000000500005e0},
    {"10", 1005, 1605, -1, 10, 2, 0x500001f68000322c, "500001f68000322c", 0x00000000a00003ed, 0x00000000a0000645},
    {"22", 1006, 1706,  0, 22, 0, 0xb00001f700003550, "b00001f700003550", 0x00000001600003ee, 0x00000001600006aa},
    { "X", 1007, 1807,  1, 23, 1, 0xb80001f78000387a, "b80001f78000387a", 0x00000001700003ef, 0x000000017000070f},
    { "Y", 1008, 1908, -1, 24, 2, 0xc00001f800003ba4, "c00001f800003ba4", 0x00000001800003f0, 0x0000000180000774},
    {"MT", 1009, 2009,  0, 25, 0, 0xc80001f880003ec8, "c80001f880003ec8", 0x00000001900003f1, 0x00000001900007d9},
};

typedef struct test_overlap_t
{
    uint8_t a_chrom;
    uint32_t a_startpos;
    uint32_t a_endpos;
    uint64_t a_rk;
    uint64_t a_vk;
    uint8_t b_chrom;
    uint32_t b_startpos;
    uint32_t b_endpos;
    uint64_t b_rk;
    uint8_t res;
} test_overlap_t;

static test_overlap_t test_overlap[TEST_OVERLAP_SIZE] =
{
    { 1, 5,  7, 0x0800000280000038, 0x0800000290920000,  2, 5, 7, 0x1000000280000038, 0}, // different chromosome
    { 1, 0,  2, 0x0800000000000010, 0x0800000010920000,  1, 3, 7, 0x0800000180000038, 0}, // -[-]|---|----
    { 2, 1,  3, 0x1000000080000018, 0x1000000090920000,  2, 3, 7, 0x1000000180000038, 0}, // --[-]---|----
    { 3, 2,  4, 0x1800000100000020, 0x1800000110920000,  3, 3, 7, 0x1800000180000038, 1}, // ---[|]--|----
    { 4, 3,  5, 0x2000000180000028, 0x2000000190920000,  4, 3, 7, 0x2000000180000038, 1}, // ----[-]-|----
    { 5, 4,  6, 0x2800000200000030, 0x2800000210920000,  5, 3, 7, 0x2800000180000038, 1}, // ----|[-]|----
    { 6, 5,  7, 0x3000000280000038, 0x3000000290920000,  6, 3, 7, 0x3000000180000038, 1}, // ----|-[ ]----
    {10, 6,  8, 0x5000000300000040, 0x5000000310920000, 10, 3, 7, 0x5000000180000038, 1}, // ----|--[|]---
    {22, 7,  9, 0xb000000380000048, 0xb000000390920000, 22, 3, 7, 0xb000000180000038, 0}, // ----|---[-]--
    {23, 8, 10, 0xb800000400000050, 0xb800000410920000, 23, 3, 7, 0xb800000180000038, 0}, // ----|---|[-]-
    {24, 2,  8, 0xc000000100000040, 0xc000000130911200, 24, 3, 7, 0xc000000180000038, 1}, // ---[|---|]---
    {25, 3,  7, 0xc800000180000038, 0xc8000001a0912000, 25, 3, 7, 0xc800000180000038, 1}, // ----[---]----
};

// returns current time in nanoseconds
uint64_t get_time()
{
    struct timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return (((uint64_t)t.tv_sec * 1000000000) + (uint64_t)t.tv_nsec);
}

int test_encode_region_strand()
{
    int errors = 0;
    int i;
    uint8_t res;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        res = encode_region_strand(test_data[i].strand);
        if (res != test_data[i].estrand)
        {
            fprintf(stderr, "%s (%d) Expecting encoded STRAND %d, got %d\n", __func__, i, test_data[i].estrand, res);
            ++errors;
        }
    }
    return errors;
}

int test_decode_region_strand()
{
    int errors = 0;
    int i;
    int8_t res;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        res = decode_region_strand(test_data[i].estrand);
        if (res != test_data[i].strand)
        {
            fprintf(stderr, "%s (%d) Expecting STRAND %d, got %d\n", __func__, i, test_data[i].strand, res);
            ++errors;
        }
    }
    return errors;
}

int test_encode_regionkey()
{
    int errors = 0;
    int i;
    uint64_t res;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        res = encode_regionkey(test_data[i].echrom, test_data[i].startpos, test_data[i].endpos, test_data[i].estrand);
        if (res != test_data[i].rk)
        {
            fprintf(stderr, "%s (%d) Expecting RegionKey %016" PRIx64 ", got %016" PRIx64 "\n", __func__, i, test_data[i].rk, res);
            ++errors;
        }
    }
    return errors;
}

int test_extract_regionkey_chrom()
{
    int errors = 0;
    int i;
    uint8_t res;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        res = extract_regionkey_chrom(test_data[i].rk);
        if (res != test_data[i].echrom)
        {
            fprintf(stderr, "%s (%d) Expecting CHROM %d, got %d\n", __func__, i, test_data[i].echrom, res);
            ++errors;
        }
    }
    return errors;
}

int test_extract_regionkey_startpos()
{
    int errors = 0;
    int i;
    uint32_t res;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        res = extract_regionkey_startpos(test_data[i].rk);
        if (res != test_data[i].startpos)
        {
            fprintf(stderr, "%s (%d) Expecting CHROM %" PRIu32 ", got %" PRIu32 "\n", __func__, i, test_data[i].startpos, res);
            ++errors;
        }
    }
    return errors;
}

int test_extract_regionkey_endpos()
{
    int errors = 0;
    int i;
    uint32_t res;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        res = extract_regionkey_endpos(test_data[i].rk);
        if (res != test_data[i].endpos)
        {
            fprintf(stderr, "%s (%d) Expecting CHROM %" PRIu32 ", got %" PRIu32 "\n", __func__, i, test_data[i].endpos, res);
            ++errors;
        }
    }
    return errors;
}

int test_extract_regionkey_strand()
{
    int errors = 0;
    int i;
    uint8_t res;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        res = extract_regionkey_strand(test_data[i].rk);
        if (res != test_data[i].estrand)
        {
            fprintf(stderr, "%s (%d) Expecting CHROM %" PRIu32 ", got %" PRIu32 "\n", __func__, i, test_data[i].estrand, res);
            ++errors;
        }
    }
    return errors;
}

int test_decode_regionkey()
{
    int errors = 0;
    int i;
    regionkey_t h = {0};
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        decode_regionkey(test_data[i].rk, &h);
        if (h.chrom != test_data[i].echrom)
        {
            fprintf(stderr, "%s (%d): Unexpected CHROM code: expected %" PRIu8 ", got %" PRIu8 "\n", __func__, i, test_data[i].echrom, h.chrom);
            ++errors;
        }
        if (h.startpos != test_data[i].startpos)
        {
            fprintf(stderr, "%s (%d): Unexpected START POS: expected %" PRIu32 ", got %" PRIu32 "\n", __func__, i, test_data[i].startpos, h.startpos);
            ++errors;
        }
        if (h.endpos != test_data[i].endpos)
        {
            fprintf(stderr, "%s (%d): Unexpected END POS: expected %" PRIu32 ", got %" PRIu32 "\n", __func__, i, test_data[i].endpos, h.endpos);
            ++errors;
        }
        if (h.strand != test_data[i].estrand)
        {
            fprintf(stderr, "%s (%d): Unexpected CHROM code: expected %" PRIu8 ", got %" PRIu8 "\n", __func__, i, test_data[i].estrand, h.strand);
            ++errors;
        }
    }
    return errors;
}

void benchmark_decode_regionkey()
{
    regionkey_t h = {0};
    uint64_t tstart, tend;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        decode_regionkey(0x080001f400002260 + i, &h);
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/size);
}

int test_reverse_regionkey()
{
    int errors = 0;
    int i;
    regionkey_rev_t h = {0};
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        reverse_regionkey(test_data[i].rk, &h);
        if (strcasecmp(test_data[i].chrom, h.chrom) != 0)
        {
            fprintf(stderr, "%s (%d): Unexpected CHROM: expected %s, got %s\n", __func__, i, test_data[i].chrom, h.chrom);
            ++errors;
        }
        if (h.startpos != test_data[i].startpos)
        {
            fprintf(stderr, "%s (%d): Unexpected START POS: expected %" PRIu32 ", got %" PRIu32 "\n", __func__, i, test_data[i].startpos, h.startpos);
            ++errors;
        }
        if (h.endpos != test_data[i].endpos)
        {
            fprintf(stderr, "%s (%d): Unexpected END POS: expected %" PRIu32 ", got %" PRIu32 "\n", __func__, i, test_data[i].endpos, h.endpos);
            ++errors;
        }
        if (h.strand != test_data[i].strand)
        {
            fprintf(stderr, "%s (%d): Unexpected CHROM code: expected %d, got %d\n", __func__, i, test_data[i].strand, h.strand);
            ++errors;
        }
    }
    return errors;
}

void benchmark_reverse_regionkey()
{
    regionkey_rev_t h = {0};
    uint64_t tstart, tend;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        reverse_regionkey(0x080001f400002260 + i, &h);
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/size);
}

int test_regionkey()
{
    int errors = 0;
    int i;
    uint64_t res;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        res = regionkey(test_data[i].chrom, strlen(test_data[i].chrom), test_data[i].startpos, test_data[i].endpos, test_data[i].strand);
        if (res != test_data[i].rk)
        {
            fprintf(stderr, "%s (%d) Expecting RegionKey %016" PRIx64 ", got %016" PRIx64 "\n", __func__, i, test_data[i].rk, res);
            ++errors;
        }
    }
    return errors;
}

void benchmark_regionkey()
{
    uint64_t res;
    uint64_t tstart, tend;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        res = regionkey("MT", 2, 1000, 2000, -1);
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op (%" PRIx64 ")\n", __func__, (tend - tstart)/size, res);
}

int test_regionkey_hex()
{
    int errors = 0;
    int i;
    char rs[17] = "";
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        regionkey_hex(test_data[i].rk, rs);
        if (strcmp(rs, test_data[i].rs))
        {
            fprintf(stderr, "%s (%d) Expecting RegionKey HEX %s, got %s\n", __func__, i, test_data[i].rs, rs);
            ++errors;
        }
    }
    return errors;
}

int test_parse_regionkey_hex()
{
    int errors = 0;
    int i;
    uint64_t rk;
    rk = parse_regionkey_hex("1234567890AbCdEf");
    if (rk != 0x1234567890abcdef)
    {
        fprintf(stderr, "%s : Unexpected regionkey: expected 0x1234567890abcdef, got 0x%016" PRIx64 "\n", __func__, rk);
        ++errors;
    }
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        rk = parse_variantkey_hex(test_data[i].rs);
        if (rk != test_data[i].rk)
        {
            fprintf(stderr, "%s (%d): Unexpected regionkey: expected 0x%016" PRIx64 ", got 0x%016" PRIx64 "\n", __func__, i, test_data[i].rk, rk);
            ++errors;
        }
    }
    return errors;
}

int test_get_regionkey_chrom_startpos()
{
    int errors = 0;
    int i;
    uint64_t res;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        res = get_regionkey_chrom_startpos(test_data[i].rk);
        if (res != test_data[i].chrom_startpos)
        {
            fprintf(stderr, "%s (%d) Expecting CHROM + START POS %016" PRIx64 ", got %016" PRIx64 "\n", __func__, i, test_data[i].chrom_startpos, res);
            ++errors;
        }
    }
    return errors;
}

int test_get_regionkey_chrom_endpos()
{
    int errors = 0;
    int i;
    uint64_t res;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        res = get_regionkey_chrom_endpos(test_data[i].rk);
        if (res != test_data[i].chrom_endpos)
        {
            fprintf(stderr, "%s (%d) Expecting CHROM + END POS %016" PRIx64 ", got %016" PRIx64 "\n", __func__, i, test_data[i].chrom_endpos, res);
            ++errors;
        }
    }
    return errors;
}

int test_are_overlapping_regions()
{
    int errors = 0;
    int i;
    uint8_t res;
    for (i=0 ; i < TEST_OVERLAP_SIZE; i++)
    {
        res = are_overlapping_regions(test_overlap[i].a_chrom, test_overlap[i].a_startpos, test_overlap[i].a_endpos, test_overlap[i].b_chrom, test_overlap[i].b_startpos, test_overlap[i].b_endpos);
        if (res != test_overlap[i].res)
        {
            fprintf(stderr, "%s (%d) Expecting %" PRIu8 ", got %" PRIu8 "\n", __func__, i, test_overlap[i].res, res);
            ++errors;
        }
    }
    return errors;
}

int test_are_overlapping_region_regionkey()
{
    int errors = 0;
    int i;
    uint8_t res;
    for (i=0 ; i < TEST_OVERLAP_SIZE; i++)
    {
        res = are_overlapping_region_regionkey(test_overlap[i].a_chrom, test_overlap[i].a_startpos, test_overlap[i].a_endpos, test_overlap[i].b_rk);
        if (res != test_overlap[i].res)
        {
            fprintf(stderr, "%s (%d) Expecting %" PRIu8 ", got %" PRIu8 "\n", __func__, i, test_overlap[i].res, res);
            ++errors;
        }
    }
    return errors;
}

int test_are_overlapping_regionkeys()
{
    int errors = 0;
    int i;
    uint8_t res;
    for (i=0 ; i < TEST_OVERLAP_SIZE; i++)
    {
        res = are_overlapping_regionkeys(test_overlap[i].a_rk, test_overlap[i].b_rk);
        if (res != test_overlap[i].res)
        {
            fprintf(stderr, "%s (%d) Expecting %" PRIu8 ", got %" PRIu8 "\n", __func__, i, test_overlap[i].res, res);
            ++errors;
        }
    }
    return errors;
}

int test_are_overlapping_variantkey_regionkey()
{
    int errors = 0;
    int i;
    uint8_t res;
    for (i=0 ; i < TEST_OVERLAP_SIZE; i++)
    {
        res = are_overlapping_variantkey_regionkey(NULL, 0, test_overlap[i].a_vk, test_overlap[i].b_rk);
        if (res != test_overlap[i].res)
        {
            fprintf(stderr, "%s (%d) Expecting %" PRIu8 ", got %" PRIu8 "\n", __func__, i, test_overlap[i].res, res);
            ++errors;
        }
    }
    return errors;
}

int test_variantkey_to_regionkey()
{
    int errors = 0;
    int i;
    uint64_t res;
    for (i=0 ; i < TEST_OVERLAP_SIZE; i++)
    {
        res = variantkey_to_regionkey(NULL, 0, test_overlap[i].a_vk);
        if (res != test_overlap[i].a_rk)
        {
            fprintf(stderr, "%s (%d) Expecting %016" PRIx64 ", got %016" PRIx64 "\n", __func__, i, test_overlap[i].a_rk, res);
            ++errors;
        }
    }
    return errors;
}

int main()
{
    int errors = 0;
    int err;

    mmfile_t vknr = {0};
    mmap_binfile("vknr.10.bin", &vknr);

    if ((vknr.last + 1) != TEST_DATA_SIZE)
    {
        fprintf(stderr, "Expecting %d items, got instead: %" PRIu64 "\n", TEST_DATA_SIZE, vknr.last + 1);
        return 1;
    }

    errors += test_encode_region_strand();
    errors += test_decode_region_strand();
    errors += test_encode_regionkey();
    errors += test_extract_regionkey_chrom();
    errors += test_extract_regionkey_startpos();
    errors += test_extract_regionkey_endpos();
    errors += test_extract_regionkey_strand();
    errors += test_decode_regionkey();
    errors += test_reverse_regionkey();
    errors += test_regionkey();
    errors += test_regionkey_hex();
    errors += test_parse_regionkey_hex();
    errors += test_get_regionkey_chrom_startpos();
    errors += test_get_regionkey_chrom_endpos();
    errors += test_are_overlapping_regions();
    errors += test_are_overlapping_region_regionkey();
    errors += test_are_overlapping_regionkeys();
    errors += test_are_overlapping_variantkey_regionkey();
    errors += test_variantkey_to_regionkey();

    benchmark_decode_regionkey();
    benchmark_reverse_regionkey();
    benchmark_regionkey();

    err = munmap_binfile(vknr);
    if (err != 0)
    {
        fprintf(stderr, "Got %d error while unmapping the vknr file\n", err);
        return 1;
    }

    return errors;
}
