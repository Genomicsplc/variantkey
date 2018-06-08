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
#include "../src/genoref.h"

// returns current time in nanoseconds
uint64_t get_time()
{
    struct timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return (((uint64_t)t.tv_sec * 1000000000) + (uint64_t)t.tv_nsec);
}

int test_get_genoref_seq(const unsigned char *src, uint32_t idx[])
{
    int errors = 0;
    char ref, exp;
    uint8_t chrom;
    for (chrom = 1; chrom <= 25; chrom++)
    {
        ref = get_genoref_seq(src, idx, chrom, 0); // first
        if (ref != 'A')
        {
            fprintf(stderr, "%s (first): Expected reference 'A', got '%c'\n", __func__, ref);
            ++errors;
        }
        ref = get_genoref_seq(src, idx, chrom, (26 - chrom)); // last
        exp = ('Z' + 1 - chrom);
        if (ref != exp)
        {
            fprintf(stderr, "%s (last): Expected reference '%c', got '%c'\n", __func__, exp, ref);
            ++errors;
        }
        ref = get_genoref_seq(src, idx, chrom, (27 - chrom)); // invalid
        if (ref != 0)
        {
            fprintf(stderr, "%s (invalid): Expected reference 0, got '%c'\n", __func__, ref);
            ++errors;
        }
    }
    return errors;
}

void benchmark_get_genoref_seq(const unsigned char *src, uint32_t idx[])
{
    uint8_t chrom;
    uint64_t tstart, tend;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        for (chrom = 1; chrom <= 25; chrom++)
        {
            get_genoref_seq(src, idx, chrom, 1);
        }
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/(size*25));
}

int test_check_reference(const unsigned char *src, uint32_t idx[])
{
    int errors = 0;
    int ret;
    int i;
    typedef struct test_ref_t
    {
        uint8_t chrom;
        uint32_t pos;
        const char *ref;
        size_t sizeref;
        int exp;
    } test_ref_t;
    static test_ref_t test_ref[42] =
    {
        {1, 0, "A", 1, 0},
        {1, 25, "Z", 1, 0},
        {25, 0, "A", 1, 0},
        {25, 1, "B", 1, 0},
        {2, 0, "ABCDEFGHIJKLmnopqrstuvwxy", 25, 0},
        {1, 26, "ZABC", 4, -2},
        {1, 0, "ABCDEFGHIJKLmnopqrstuvwxyJ", 26, -1},
        {14, 2, "ZZZ", 3, -1},
        {1, 0, "N", 1, 1},
        {10, 13, "A", 1, 1},
        {1, 3, "B", 1, 1},
        {1, 1, "C", 1, 1},
        {1, 0, "D", 1, 1},
        {1, 3, "A", 1, 1},
        {1, 0, "H", 1, 1},
        {1, 7, "A", 1, 1},
        {1, 0, "V", 1, 1},
        {1, 21, "A", 1, 1},
        {1, 0, "W", 1, 1},
        {1, 19, "W", 1, 1},
        {1, 22, "A", 1, 1},
        {1, 22, "T", 1, 1},
        {1, 2, "S", 1, 1},
        {1, 6, "S", 1, 1},
        {1, 18, "C", 1, 1},
        {1, 18, "G", 1, 1},
        {1, 0, "M", 1, 1},
        {1, 2, "M", 1, 1},
        {1, 12, "A", 1, 1},
        {1, 12, "C", 1, 1},
        {1, 6, "K", 1, 1},
        {1, 19, "K", 1, 1},
        {1, 10, "G", 1, 1},
        {1, 10, "T", 1, 1},
        {1, 0, "R", 1, 1},
        {1, 6, "R", 1, 1},
        {1, 17, "A", 1, 1},
        {1, 17, "G", 1, 1},
        {1, 2, "Y", 1, 1},
        {1, 19, "Y", 1, 1},
        {1, 24, "C", 1, 1},
        {1, 24, "T", 1, 1},
    };
    for (i = 0; i < 42; i++)
    {
        ret = check_reference(src, idx, test_ref[i].chrom, test_ref[i].pos, test_ref[i].ref, test_ref[i].sizeref);
        if (ret != test_ref[i].exp)
        {
            fprintf(stderr, "%s (%d): Expected %d, got %d\n", __func__, i, test_ref[i].exp, ret);
            ++errors;
        }
    }
    return errors;
}

int test_flip_allele()
{
    int errors = 0;
    char allele[] =   "ATCGMKRYBVDHWSNatcgmkrybvdhwsn";
    char expected[] = "TAGCKMYRVBHDWSNTAGCKMYRVBHDWSN";
    flip_allele(allele, 30);
    if (strcmp(allele, expected) != 0)
    {
        fprintf(stderr, "%s : Expected %s, got %s\n", __func__, expected, allele);
        ++errors;
    }
    return errors;
}

int main()
{
    int errors = 0;
    int err;

    mmfile_t genoref = {0};
    mmap_binfile("genoref.bin", &genoref);

    uint32_t idx[27];
    load_genoref_index(genoref.src, idx);

    if ((uint64_t)idx[26] != genoref.size)
    {
        fprintf(stderr, "Expecting size %"PRIu64", got instead: %"PRIu32"\n", genoref.size, idx[26]);
        return 1;
    }

    errors += test_get_genoref_seq(genoref.src, idx);
    errors += test_check_reference(genoref.src, idx);
    errors += test_flip_allele();

    benchmark_get_genoref_seq(genoref.src, idx);

    err = munmap_binfile(genoref);
    if (err != 0)
    {
        fprintf(stderr, "Got %d error while unmapping the genoref file\n", err);
        return 1;
    }

    return errors;
}
