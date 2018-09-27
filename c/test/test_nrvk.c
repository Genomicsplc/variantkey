// VariantKey
//
// test_nrvk.c
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

// Test for nrvk

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
#include "../src/variantkey/nrvk.h"

#define TEST_DATA_SIZE 10

typedef struct test_data_t
{
    uint64_t   vk;
    const char chrom[3];
    uint32_t   pos;
    uint64_t   len;
    size_t     sizeref;
    size_t     sizealt;
    uint64_t   chrom_startpos;
    uint64_t   chrom_endpos;
    const char ref[256];
    const char alt[256];
} test_data_t;

static const test_data_t test_data[TEST_DATA_SIZE] =
{
    {0x0800c35093ace339,  "1", 100001, 0x00000004, 0x01, 0x01, 0x00000000100186a1, 0x00000000100186a2, "N", "A"},
    {0x1000c3517f91cdb1,  "2", 100002, 0x0000000e, 0x0b, 0x01, 0x00000000200186a2, 0x00000000200186ad, "AAGAAAGAAAG", "A"},
    {0x1800c351f61f65d3,  "3", 100003, 0x0000000e, 0x01, 0x0b, 0x00000000300186a3, 0x00000000300186a4, "A", "AAGAAAGAAAG"},
    {0x2000c3521f1c15ab,  "4", 100004, 0x0000000e, 0x08, 0x04, 0x00000000400186a4, 0x00000000400186ac, "ACGTACGT", "ACGT"},
    {0x2800c352d8f2d5b5,  "5", 100005, 0x0000000e, 0x04, 0x08, 0x00000000500186a5, 0x00000000500186a9, "ACGT", "ACGTACGT"},
    {0x5000c3553bbf9c19, "10", 100010, 0x00000012, 0x08, 0x08, 0x00000000a00186aa, 0x00000000a00186b2, "ACGTACGT", "CGTACGTA"},
    {0xb000c35b64690b25, "22", 100022, 0x0000000b, 0x08, 0x01, 0x00000001600186b6, 0x00000001600186be, "ACGTACGT", "N"},
    {0xb800c35bbcece603,  "X", 100023, 0x0000000e, 0x0a, 0x02, 0x00000001700186b7, 0x00000001700186c1, "AAAAAAAAGG", "AG"},
    {0xc000c35c63741ee7,  "Y", 100024, 0x0000000e, 0x02, 0x0a, 0x00000001800186b8, 0x00000001800186ba, "AG", "AAAAAAAAGG"},
    {0xc800c35c96c18499, "MT", 100025, 0x00000012, 0x04, 0x0c, 0x00000001900186b9, 0x00000001900186bd, "ACGT", "AAACCCGGGTTT"},
};

// returns current time in nanoseconds
uint64_t get_time()
{
    struct timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return (((uint64_t)t.tv_sec * 1000000000) + (uint64_t)t.tv_nsec);
}

int test_find_ref_alt_by_variantkey(nrvk_cols_t nvc)
{
    int errors = 0;
    int i;
    char ref[256], alt[256];
    size_t sizeref = 0, sizealt = 0, len;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        len = find_ref_alt_by_variantkey(nvc, test_data[i].vk, ref, &sizeref, alt, &sizealt);
        if (len != (test_data[i].len - 2))
        {
            fprintf(stderr, "%s (%d) Expected len %lu, got %lu\n",  __func__, i, test_data[i].len, len);
            ++errors;
        }
        if (sizeref != test_data[i].sizeref)
        {
            fprintf(stderr, "%s (%d) Expecting REF size %lu, got %lu\n", __func__, i, test_data[i].sizeref, sizeref);
            ++errors;
        }
        if (sizealt != test_data[i].sizealt)
        {
            fprintf(stderr, "%s (%d) Expecting ALT size %lu, got %lu\n", __func__, i, test_data[i].sizealt, sizealt);
            ++errors;
        }
        if (strcasecmp(test_data[i].ref, ref) != 0)
        {
            fprintf(stderr, "%s (%d) Expecting REF %s, got %s\n", __func__, i, test_data[i].ref, ref);
            ++errors;
        }
        if (strcasecmp(test_data[i].alt, alt) != 0)
        {
            fprintf(stderr, "%s (%d) Expecting ALT %s, got %s\n", __func__, i, test_data[i].alt, alt);
            ++errors;
        }
    }
    return errors;
}

int test_find_ref_alt_by_variantkey_notfound(nrvk_cols_t nvc)
{
    int errors = 0;
    char ref[256], alt[256];
    size_t sizeref, sizealt, len;
    len = find_ref_alt_by_variantkey(nvc, 0xffffffff, ref, &sizeref, alt, &sizealt);
    if (len != 0)
    {
        fprintf(stderr, "%s : Expected len 0, got %lu\n",  __func__, len);
        ++errors;
    }
    return errors;
}

void benchmark_find_ref_alt_by_variantkey(nrvk_cols_t nvc)
{
    char ref[256], alt[256];
    size_t sizeref, sizealt;
    uint64_t tstart, tend;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        find_ref_alt_by_variantkey(nvc, 0xb000c35b64690b25, ref, &sizeref, alt, &sizealt);
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/size);
}

int test_reverse_variantkey(nrvk_cols_t nvc)
{
    int errors = 0;
    int i;
    variantkey_rev_t rev = {0};
    size_t len;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        len = reverse_variantkey(nvc, test_data[i].vk, &rev);
        if (len != (test_data[i].len - 2))
        {
            fprintf(stderr, "%s (%d) Expected len %lu, got %lu\n",  __func__, i, test_data[i].len, len);
            ++errors;
        }
        if (rev.sizeref != test_data[i].sizeref)
        {
            fprintf(stderr, "%s (%d) Expecting REF size %lu, got %lu\n", __func__, i, test_data[i].sizeref, rev.sizeref);
            ++errors;
        }
        if (rev.sizealt != test_data[i].sizealt)
        {
            fprintf(stderr, "%s (%d) Expecting ALT size %lu, got %lu\n", __func__, i, test_data[i].sizealt, rev.sizealt);
            ++errors;
        }
        if (strcasecmp(test_data[i].ref, rev.ref) != 0)
        {
            fprintf(stderr, "%s (%d) Expecting REF %s, got %s\n", __func__, i, test_data[i].ref, rev.ref);
            ++errors;
        }
        if (strcasecmp(test_data[i].alt, rev.alt) != 0)
        {
            fprintf(stderr, "%s (%d) Expecting ALT %s, got %s\n", __func__, i, test_data[i].alt, rev.alt);
            ++errors;
        }
        if (strcasecmp(test_data[i].chrom, rev.chrom) != 0)
        {
            fprintf(stderr, "%s (%d) Expecting CHROM %s, got %s\n", __func__, i, test_data[i].chrom, rev.chrom);
            ++errors;
        }
        if (rev.pos != test_data[i].pos)
        {
            fprintf(stderr, "%s (%d) Expecting POS size %" PRIu32 ", got %" PRIu32 "\n", __func__, i, test_data[i].pos, rev.pos);
            ++errors;
        }
    }
    return errors;
}

void benchmark_reverse_variantkey(nrvk_cols_t nvc)
{
    variantkey_rev_t rev = {0};
    uint64_t tstart, tend;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        reverse_variantkey(nvc, 0xb000c35b64690b25, &rev);
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/size);
}

int test_get_variantkey_ref_length(nrvk_cols_t nvc)
{
    int errors = 0;
    int i;
    size_t sizeref;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        sizeref = get_variantkey_ref_length(nvc, test_data[i].vk);
        if (sizeref != test_data[i].sizeref)
        {
            fprintf(stderr, "%s (%d) Expecting REF size %lu, got %lu\n", __func__, i, test_data[i].sizeref, sizeref);
            ++errors;
        }
    }
    return errors;
}

int test_get_variantkey_ref_length_reversible(nrvk_cols_t nvc)
{
    int errors = 0;
    size_t sizeref = get_variantkey_ref_length(nvc, 0x1800925199160000);
    if (sizeref != 3)
    {
        fprintf(stderr, "%s : Expected REF size 3, got %lu\n",  __func__, sizeref);
        ++errors;
    }
    return errors;
}

int test_get_variantkey_ref_length_notfound(nrvk_cols_t nvc)
{
    int errors = 0;
    size_t sizeref = get_variantkey_ref_length(nvc, 0xffffffffffffffff);
    if (sizeref != 0)
    {
        fprintf(stderr, "%s : Expected REF size 0, got %lu\n",  __func__, sizeref);
        ++errors;
    }
    return errors;
}

int test_get_variantkey_endpos(nrvk_cols_t nvc)
{
    int errors = 0;
    int i;
    uint32_t endpos, exp;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        endpos = get_variantkey_endpos(nvc, test_data[i].vk);
        exp = test_data[i].pos + test_data[i].sizeref;
        if (endpos != exp)
        {
            fprintf(stderr, "%s (%d) Expecting END POS size %" PRIu32 ", got %" PRIu32 "\n", __func__, i, exp, endpos);
            ++errors;
        }
    }
    return errors;
}

int test_get_variantkey_chrom_startpos()
{
    int errors = 0;
    int i;
    uint64_t res;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        res = get_variantkey_chrom_startpos(test_data[i].vk);
        if (res != test_data[i].chrom_startpos)
        {
            fprintf(stderr, "%s (%d) Expecting CHROM + START POS %016" PRIx64 ", got %016" PRIx64 "\n", __func__, i, test_data[i].chrom_startpos, res);
            ++errors;
        }
    }
    fprintf(stderr, "\n");
    return errors;
}

int test_get_variantkey_chrom_endpos(nrvk_cols_t nvc)
{
    int errors = 0;
    int i;
    uint64_t res;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        res = get_variantkey_chrom_endpos(nvc, test_data[i].vk);
        if (res != test_data[i].chrom_endpos)
        {
            fprintf(stderr, "%s (%d) Expecting CHROM + END POS %016" PRIx64 ", got %016" PRIx64 "\n", __func__, i, test_data[i].chrom_endpos, res);
            ++errors;
        }
    }
    return errors;
}

int test_nrvk_bin_to_tsv(nrvk_cols_t nvc)
{
    int errors = 0;
    size_t len = nrvk_bin_to_tsv(nvc, "nrvk.test");
    if (len != 305)
    {
        fprintf(stderr, "%s Expecting file with 305 bytes, got %lu\n", __func__, len);
        ++errors;
    }
    return errors;
}

int test_nrvk_bin_to_tsv_error(nrvk_cols_t nvc)
{
    int errors = 0;
    size_t len = nrvk_bin_to_tsv(nvc, "/WRONG/../../nrvk.test");
    if (len != 0)
    {
        fprintf(stderr, "%s Expecting 0 bytes, got %lu\n", __func__, len);
        ++errors;
    }
    return errors;
}

int main()
{
    int errors = 0;
    int err;

    mmfile_t nrvk = {0};
    nrvk_cols_t nvc = {0};
    mmap_nrvk_file("nrvk.10.bin", &nrvk, &nvc);

    if (nrvk.nrows != TEST_DATA_SIZE)
    {
        fprintf(stderr, "Expecting %d items, got instead: %" PRIu64 "\n", TEST_DATA_SIZE, nrvk.nrows);
        return 1;
    }

    errors += test_find_ref_alt_by_variantkey(nvc);
    errors += test_find_ref_alt_by_variantkey_notfound(nvc);
    errors += test_reverse_variantkey(nvc);
    errors += test_get_variantkey_ref_length(nvc);
    errors += test_get_variantkey_ref_length_reversible(nvc);
    errors += test_get_variantkey_ref_length_notfound(nvc);
    errors += test_get_variantkey_endpos(nvc);
    errors += test_get_variantkey_chrom_startpos();
    errors += test_get_variantkey_chrom_endpos(nvc);
    errors += test_nrvk_bin_to_tsv(nvc);
    errors += test_nrvk_bin_to_tsv_error(nvc);

    benchmark_find_ref_alt_by_variantkey(nvc);
    benchmark_reverse_variantkey(nvc);

    err = munmap_binfile(nrvk);
    if (err != 0)
    {
        fprintf(stderr, "Got %d error while unmapping the nrvk file\n", err);
        return 1;
    }

    return errors;
}
