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
            fprintf(stderr, "%s (first): Expected reference 'A', got '%c'\n",  __func__, ref);
            ++errors;
        }
        ref = get_genoref_seq(src, idx, chrom, (26 - chrom)); // last
        exp = ('Z' + 1 - chrom);
        if (ref != exp)
        {
            fprintf(stderr, "%s (last): Expected reference '%c', got '%c'\n",  __func__, exp, ref);
            ++errors;
        }
        ref = get_genoref_seq(src, idx, chrom, (27 - chrom)); // invalid
        if (ref != 0)
        {
            fprintf(stderr, "%s (invalid): Expected reference 0, got '%c'\n",  __func__, ref);
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

    benchmark_get_genoref_seq(genoref.src, idx);

    err = munmap_binfile(genoref);
    if (err != 0)
    {
        fprintf(stderr, "Got %d error while unmapping the genoref file\n", err);
        return 1;
    }

    return errors;
}
