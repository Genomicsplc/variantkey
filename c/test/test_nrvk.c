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
    const char chrom[3];
    uint32_t pos;
    uint64_t len;
    size_t sizeref;
    size_t sizealt;
    const char ref[256];
    const char alt[256];
} test_data_t;

static test_data_t test_data[TEST_DATA_SIZE] =
{
    {0x0800c35093ace339,  "1", 100001, 0x00000004, 0x01, 0x01, "N", "A"},
    {0x1000c3517f91cdb1,  "2", 100002, 0x0000000e, 0x0b, 0x01, "AAGAAAGAAAG", "A"},
    {0x1800c351f61f65d3,  "3", 100003, 0x0000000e, 0x01, 0x0b, "A", "AAGAAAGAAAG"},
    {0x2000c3521f1c15ab,  "4", 100004, 0x0000000e, 0x08, 0x04, "ACGTACGT", "ACGT"},
    {0x2800c352d8f2d5b5,  "5", 100005, 0x0000000e, 0x04, 0x08, "ACGT", "ACGTACGT"},
    {0x5000c3553bbf9c19, "10", 100010, 0x00000012, 0x08, 0x08, "ACGTACGT", "CGTACGTA"},
    {0xb000c35b64690b25, "22", 100022, 0x0000000b, 0x08, 0x01, "ACGTACGT", "N"},
    {0xb800c35bbcece603,  "X", 100023, 0x0000000e, 0x0a, 0x02, "AAAAAAAAGG", "AG"},
    {0xc000c35c63741ee7,  "Y", 100024, 0x0000000e, 0x02, 0x0a, "AG", "AAAAAAAAGG"},
    {0xc800c35c96c18499, "MT", 100025, 0x00000012, 0x04, 0x0c, "ACGT", "AAACCCGGGTTT"},
};

// returns current time in nanoseconds
uint64_t get_time()
{
    struct timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return (((uint64_t)t.tv_sec * 1000000000) + (uint64_t)t.tv_nsec);
}

int test_find_ref_alt_by_variantkey(mmfile_t vknr)
{
    int errors = 0;
    int i;
    char ref[256], alt[256];
    size_t sizeref,sizealt, len;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        len = find_ref_alt_by_variantkey(vknr.src, vknr.last, test_data[i].vk, ref, &sizeref, alt, &sizealt);
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

int test_find_ref_alt_by_variantkey_notfound(mmfile_t vknr)
{
    int errors = 0;
    char ref[256], alt[256];
    size_t sizeref,sizealt, len;
    len = find_ref_alt_by_variantkey(vknr.src, vknr.last, 0xffffffff, ref, &sizeref, alt, &sizealt);
    if (len != 0)
    {
        fprintf(stderr, "%s : Expected len 0, got %lu\n",  __func__, len);
        ++errors;
    }
    return errors;
}

void benchmark_find_ref_alt_by_variantkey(mmfile_t vknr)
{
    char ref[256], alt[256];
    size_t sizeref,sizealt;
    uint64_t tstart, tend;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        find_ref_alt_by_variantkey(vknr.src, vknr.last, 0xb000c35b64690b25, ref, &sizeref, alt, &sizealt);
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/(size*4));
}

int test_reverse_variantkey(mmfile_t vknr)
{
    int errors = 0;
    int i;
    variantkey_rev_t rev = {0};
    size_t len;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        len = reverse_variantkey(vknr.src, vknr.last, test_data[i].vk, &rev);
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
            fprintf(stderr, "%s (%d) Expecting POS size %"PRIu32", got %"PRIu32"\n", __func__, i, test_data[i].pos, rev.pos);
            ++errors;
        }
    }
    return errors;
}

void benchmark_reverse_variantkey(mmfile_t vknr)
{
    variantkey_rev_t rev = {0};
    uint64_t tstart, tend;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        reverse_variantkey(vknr.src, vknr.last, 0xb000c35b64690b25, &rev);
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/(size*4));
}

int main()
{
    int errors = 0;
    int err;

    mmfile_t vknr = {0};
    mmap_binfile("vknr.10.bin", &vknr);

    if ((vknr.last + 1) != TEST_DATA_SIZE)
    {
        fprintf(stderr, "Expecting %d items, got instead: %"PRIu64"\n", TEST_DATA_SIZE, vknr.last + 1);
        return 1;
    }

    errors += test_find_ref_alt_by_variantkey(vknr);
    errors += test_find_ref_alt_by_variantkey_notfound(vknr);
    errors += test_reverse_variantkey(vknr);

    benchmark_find_ref_alt_by_variantkey(vknr);
    benchmark_reverse_variantkey(vknr);

    err = munmap_binfile(vknr);
    if (err != 0)
    {
        fprintf(stderr, "Got %d error while unmapping the vknr file\n", err);
        return 1;
    }

    return errors;
}
