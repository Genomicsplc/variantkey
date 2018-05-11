// Nicola Asuni

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
#include "../src/nrvk.h"

#define TEST_DATA_SIZE 10

typedef struct test_data_t
{
    uint64_t vk;
    uint64_t len;
    size_t sizeref;
    size_t sizealt;
    const char ref[256];
    const char alt[256];
} test_data_t;

static test_data_t test_data[TEST_DATA_SIZE] =
{
    {0x08027A2580338000, 0x00000004, 1, 1, "C", "T"},
    {0x4800A1FE439E3918, 0x00000005, 2, 1, "CG", "T"},
    {0x4800A1FE7555EB16, 0x00000006, 2, 2, "CG", "TA"},
    {0x80010274003A0000, 0x00000007, 2, 3, "CG", "TAG"},
    {0x8001028D00138000, 0x00000008, 3, 3, "CGT", "TAG"},
    {0x80010299007A0000, 0x00000009, 4, 3, "CGTT", "TAG"},
    {0xA0012B62003A0000, 0x0000000A, 4, 4, "CGTT", "TAGC"},
    {0xA0012B6280708000, 0x0000000B, 4, 5, "CGTT", "TAGCT"},
    {0xA0012B65E3256692, 0x0000000C, 5, 5, "CGTTA", "TAGCT"},
    {0xA0012B67D5439803, 0x0000000D, 5, 6, "CGTTA", "TAGCTC"},
};

// returns current time in nanoseconds
uint64_t get_time()
{
    struct timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return (((uint64_t)t.tv_sec * 1000000000) + (uint64_t)t.tv_nsec);
}

int test_find_ref_alt_by_variantkey(mmfile_t vknr, uint64_t last)
{
    int errors = 0;
    int i;
    char ref[256], alt[256];
    size_t sizeref,sizealt, len;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        len = find_ref_alt_by_variantkey(vknr.src, last, test_data[i].vk, ref, &sizeref, alt, &sizealt);
        if (len != (test_data[i].len - 2))
        {
            fprintf(stderr, "%s (%d) Expected len %lu, got %lu\n",  __func__, i, test_data[i].len, len);
            ++errors;
        }
        if (sizeref != test_data[i].sizeref)
        {
            fprintf(stderr, "%s (%d) Expecting ref size %lu, got %lu\n", __func__, i, test_data[i].sizeref, sizeref);
            ++errors;
        }
        if (sizealt != test_data[i].sizealt)
        {
            fprintf(stderr, "%s (%d) Expecting alt size %lu, got %lu\n", __func__, i, test_data[i].sizealt, sizealt);
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

int test_find_ref_alt_by_variantkey_notfound(mmfile_t vknr, uint64_t last)
{
    int errors = 0;
    char ref[256], alt[256];
    size_t sizeref,sizealt, len;
    len = find_ref_alt_by_variantkey(vknr.src, last, 0xffffffff, ref, &sizeref, alt, &sizealt);
    if (len != 0)
    {
        fprintf(stderr, "%s : Expected len 0, got %lu\n",  __func__, len);
        ++errors;
    }
    return errors;
}

void benchmark_find_ref_alt_by_variantkey(mmfile_t vknr, uint64_t last)
{
    uint64_t tstart, tend;
    char ref[256], alt[256];
    size_t sizeref,sizealt;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        find_ref_alt_by_variantkey(vknr.src, last, 0xA0012B6280708000, ref, &sizeref, alt, &sizealt);
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/(size*4));
}

int main()
{
    int errors = 0;
    int err;

    mmfile_t vknr = mmap_binfile("vknr.10.bin");

    uint64_t nitems = (uint64_t) bytes_to_uint32_t(vknr.src, (uint64_t)(vknr.size - 4), 0, 31);
    if (nitems != TEST_DATA_SIZE)
    {
        fprintf(stderr, "Expecting %d items, got instead: %"PRIu64"\n", TEST_DATA_SIZE, nitems);
        return 1;
    }

    errors += test_find_ref_alt_by_variantkey(vknr, nitems - 1);
    errors += test_find_ref_alt_by_variantkey_notfound(vknr, nitems - 1);

    benchmark_find_ref_alt_by_variantkey(vknr, nitems - 1);

    err = munmap_binfile(vknr);
    if (err != 0)
    {
        fprintf(stderr, "Got %d error while unmapping the rv file\n", err);
        return 1;
    }

    return errors;
}
