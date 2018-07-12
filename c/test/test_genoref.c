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
#include "../src/genoref.c"

// returns current time in nanoseconds
uint64_t get_time()
{
    struct timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return (((uint64_t)t.tv_sec * 1000000000) + (uint64_t)t.tv_nsec);
}

int test_swap_sizes()
{
    int errors = 0;
    size_t first = 123;
    size_t second = 456;
    swap_sizes(&first, &second);
    if ((first != 456) || (second != 123))
    {
        fprintf(stderr, "%s : Error while swapping sizes\n", __func__);
        ++errors;
    }
    return errors;
}

int test_swap_alleles()
{
    int errors = 0;
    char first[ALLELE_MAXSIZE] = "ABC";
    char second[ALLELE_MAXSIZE] = "DEFGHI";
    size_t sizefirst = 3;
    size_t sizesecond = 6;
    swap_alleles(first, &sizefirst, second, &sizesecond);
    if ((strcmp(first, "DEFGHI") != 0) || (strcmp(second, "ABC") != 0) || (sizefirst != 6) || (sizesecond != 3))
    {
        fprintf(stderr, "%s : Error while swapping alleles: %s %s\n", __func__, first, second);
        ++errors;
    }
    return errors;
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
    const char expected[] = "TAGCKMYRVBHDWSNTAGCKMYRVBHDWSN";
    flip_allele(allele, 30);
    if (strcmp(allele, expected) != 0)
    {
        fprintf(stderr, "%s : Expected %s, got %s\n", __func__, expected, allele);
        ++errors;
    }
    return errors;
}

void benchmark_flip_allele()
{
    char allele[] =   "ATCGMKRYBVDHWSNatcgmkrybvdhwsn";
    uint64_t tstart, tend;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        flip_allele(allele, 30);
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/(size*25));
}

int test_normalize_variant(const unsigned char *src, uint32_t idx[])
{
    int errors = 0;
    int ret;
    int i;
    typedef struct test_norm_t
    {
        uint8_t chrom;
        uint32_t pos;
        char ref[256];
        size_t sizeref;
        char alt[256];
        size_t sizealt;
        uint32_t exp_pos;
        const char *exp_ref;
        size_t exp_sizeref;
        const char *exp_alt;
        size_t exp_sizealt;
        int exp;
    } test_norm_t;
    static test_norm_t test_norm[12] =
    {
        {1, 26, "A", 1, "C", 1, 26, "A", 1, "C", 1, -2},         // invalid position
        {1, 0, "J", 1, "C", 1, 0, "J", 1, "C", 1, -1},           // invalid reference
        {1, 0, "T", 1, "G", 1, 0, "A", 1, "C", 1, 2},            // flip
        {1, 0, "A", 1, "C", 1, 0, "A", 1, "C", 1, 0},            // OK
        {13, 2, "CDE", 3, "CD", 2, 3, "DE", 2, "D", 1, 32},      // left trim
        {13, 2, "CDE", 3, "CFE", 3, 3, "D", 1, "F", 1, 48},      // left trim + right trim
        {1, 0, "aBCDEF", 6, "aBKDEF", 6, 2, "C", 1, "K", 1, 48}, // left trim + right trim
        {1, 0, "A", 1, "", 0, 0, "A", 1, "", 0, 0},              // OK
        {1, 3, "D", 1, "", 0, 2, "CD", 2, "C", 1, 8},            // left extend
        {1, 24, "Y", 1, "CK", 2, 24, "Y", 1, "CK", 2, 0},        // OK
        {1, 0, "G", 1, "A", 1, 0, "A", 1, "G", 1, 4},            // swap
        {1, 0, "G", 1, "T", 1, 0, "A", 1, "C", 1, 6},            // swap + flip
    };
    for (i = 0; i < 12; i++)
    {
        ret = normalize_variant(src, idx, test_norm[i].chrom, &test_norm[i].pos, test_norm[i].ref, &test_norm[i].sizeref, test_norm[i].alt, &test_norm[i].sizealt);
        if (ret != test_norm[i].exp)
        {
            fprintf(stderr, "%s (%d): Expected return value %d, got %d\n", __func__, i, test_norm[i].exp, ret);
            ++errors;
        }
        if (test_norm[i].pos != test_norm[i].exp_pos)
        {
            fprintf(stderr, "%s (%d): Expected POS %" PRIu32 ", got %" PRIu32 "\n", __func__, i, test_norm[i].exp_pos, test_norm[i].pos);
            ++errors;
        }
        if (test_norm[i].sizeref != test_norm[i].exp_sizeref)
        {
            fprintf(stderr, "%s (%d): Expected REF size %lu, got %lu\n", __func__, i, test_norm[i].exp_sizeref, test_norm[i].sizeref);
            ++errors;
        }
        if (test_norm[i].sizealt != test_norm[i].exp_sizealt)
        {
            fprintf(stderr, "%s (%d): Expected ALT size %lu, got %lu\n", __func__, i, test_norm[i].exp_sizealt, test_norm[i].sizealt);
            ++errors;
        }
        if (strcmp(test_norm[i].ref, test_norm[i].exp_ref) != 0)
        {
            fprintf(stderr, "%s (%d): Expected REF %s, got %s\n", __func__, i, test_norm[i].exp_ref, test_norm[i].ref);
            ++errors;
        }
        if (strcmp(test_norm[i].alt, test_norm[i].exp_alt) != 0)
        {
            fprintf(stderr, "%s (%d): Expected ALT %s, got %s\n", __func__, i, test_norm[i].exp_alt, test_norm[i].alt);
            ++errors;
        }
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
        fprintf(stderr, "Expecting size %" PRIu64 ", got instead: %" PRIu32 "\n", genoref.size, idx[26]);
        return 1;
    }
    errors += test_swap_sizes();
    errors += test_swap_alleles();
    errors += test_get_genoref_seq(genoref.src, idx);
    errors += test_check_reference(genoref.src, idx);
    errors += test_flip_allele();
    errors += test_normalize_variant(genoref.src, idx);

    benchmark_get_genoref_seq(genoref.src, idx);
    benchmark_flip_allele();

    err = munmap_binfile(genoref);
    if (err != 0)
    {
        fprintf(stderr, "Got %d error while unmapping the genoref file\n", err);
        return 1;
    }

    return errors;
}
