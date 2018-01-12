// Nicola Asuni

#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/mman.h>
#include "binsearch.h"

#define TEST_DATA_SIZE 13

typedef struct test_data_32_t
{
    uint64_t blkpos;
    uint64_t first;
    uint64_t last;
    uint32_t search;
    uint64_t foundFirst;
    uint64_t foundFFirst;
    uint64_t foundFLast;
    uint64_t foundLast;
    uint64_t foundLFirst;
    uint64_t foundLLast;
} test_data_32_t;

static test_data_32_t test_data_uint32_t[TEST_DATA_SIZE] =
{
    {4, 0, 99, 0x00002722, 0, 0, 1, 0, 1, 0},
    {4, 0, 99, 0x000033f5, 99, 99, 98, 99, 100, 99},
    {0, 0, 99, 0x00000001, 0, 0, 1, 99, 100, 99},
    {4, 0, 99, 0xf00027f3, 100, 100, 99, 100, 100, 99},
    {4, 0, 99, 0x000027f3, 13, 13, 12, 14, 15, 14},
    {4, 13, 99, 0x000027f3, 13, 13, 12, 14, 15, 14},
    {4, 14, 99, 0x000027f3, 14, 14, 13, 14, 15, 14},
    {4, 0, 0, 0x00000001, 1, 0, 0, 1, 0, 0},
    {4, 0, 0, 0xfffffff0, 1, 1, 0, 1, 1, 0},
    {4, 99, 99, 0x00000001, 100, 99, 98, 100, 99, 98},
    {4, 99, 99, 0xfffffff0, 100, 100, 99, 100, 100, 99},
    {4, 0, 99, 0x00002805, 100, 19, 18, 100, 19, 18},
    {4, 0, 99, 0x000028ff, 100, 45, 44, 100, 45, 44},
};

typedef struct test_data_64_t
{
    uint64_t blkpos;
    uint64_t first;
    uint64_t last;
    uint64_t search;
    uint64_t foundFirst;
    uint64_t foundFFirst;
    uint64_t foundFLast;
    uint64_t foundLast;
    uint64_t foundLFirst;
    uint64_t foundLLast;
} test_data_64_t;

static test_data_64_t test_data_uint64_t[TEST_DATA_SIZE] =
{
    {4, 0, 99, 0x000027225fb6e591, 0, 0, 1, 0, 1, 0},
    {4, 0, 99, 0x000033f522a78fd9, 99, 99, 98, 99, 100, 99},
    {0, 0, 99, 0x0000000100002722, 0, 0, 1, 0, 1, 0},
    {4, 0, 99, 0xf0000001f00027f3, 100, 100, 99, 100, 100, 99},
    {4, 0, 99, 0x000027f35fb6e591, 13, 13, 12, 13, 14, 13},
    {0, 13, 99, 0x00000001000027f3, 13, 13, 12, 14, 15, 14},
    {0, 14, 99, 0x00000001000027f3, 14, 14, 13, 14, 15, 14},
    {4, 0, 0, 0x0000000000000001, 1, 0, 0, 1, 0, 0},
    {4, 0, 0, 0xfffffffffffffff0, 1, 1, 0, 1, 1, 0},
    {4, 99, 99, 0x0000000000000001, 100, 99, 98, 100, 99, 98},
    {4, 99, 99, 0xfffffffffffffff0, 100, 100, 99, 100, 100, 99},
    {4, 0, 99, 0x000028060981ef0a, 100, 19, 18, 100, 19, 18},
    {4, 0, 99, 0x000028fca24c9149, 100, 45, 44, 100, 45, 44},
};

typedef struct test_data_128_t
{
    uint64_t blkpos;
    uint64_t first;
    uint64_t last;
    uint128_t search;
    uint64_t foundFirst;
    uint64_t foundFFirst;
    uint64_t foundFLast;
    uint64_t foundLast;
    uint64_t foundLFirst;
    uint64_t foundLLast;
} test_data_128_t;

static test_data_128_t test_data_uint128_t[TEST_DATA_SIZE] =
{
    {4, 0, 99, {0x000027225fb6e591, 0x6eb7abd92e3deb1d}, 0, 0, 1, 0, 1, 0},
    {4, 0, 99, {0x000027c07b9621ec, 0x01f886390c06811d}, 10, 10, 9, 10, 11, 10},
    {4, 0, 0, {0x000027225fb6e591, 0x6eb7abd92e3deb1a}, 1, 0, 0, 1, 0, 0},
    {4, 0, 99, {0x000027c30981ef0f, 0x500126c20c059eb5}, 12, 12, 11, 12, 13, 12},
    {4, 0, 99, {0x000027f35fb6e591, 0x6eb7abd90889e85e}, 13, 13, 12, 13, 14, 13},
    {4, 0, 99, {0x000027f3d41a0ce2, 0xdf116bbc0bf2cf80}, 100, 14, 13, 100, 14, 13},
    {4, 0, 99, {0x000027f53b9e3036, 0x5103b7a62e3fbbcc}, 100, 16, 15, 100, 16, 15},
    {4, 0, 99, {0x000027f690c4deff, 0x765f63b80bf00517}, 16, 16, 15, 16, 17, 16},
    {4, 0, 99, {0x000033f522a78fd9, 0x1acc7b430ac5ca22}, 99, 99, 98, 99, 100, 99},
    {4, 0, 0, {0x0000000000000001, 0x0000000000000001}, 1, 0, 0, 1, 0, 0},
    {4, 0, 0, {0xfffffffffffffff0, 0xfffffffffffffff0}, 1, 1, 0, 1, 1, 0},
    {4, 0, 99, {0x000028060981ef0f, 0x500126c22f813253}, 100, 19, 18, 100, 19, 18},
    {4, 0, 99, {0x000028fca24c9148, 0x830a986a0be5c095}, 100, 45, 44, 100, 45, 44},
};

#define define_test_find_first(T) \
int test_find_first_##T(mmfile_t mf, uint64_t blklen) \
{ \
    int errors = 0; \
    int i; \
    uint64_t found, first, last; \
    for (i=0 ; i < TEST_DATA_SIZE; i++) \
    { \
        first = test_data_##T[i].first; \
        last = test_data_##T[i].last; \
        found = find_first_##T(mf.src, blklen, test_data_##T[i].blkpos, &first, &last, test_data_##T[i].search); \
        if (found != test_data_##T[i].foundFirst) \
        { \
            fprintf(stderr, "%s (%d) Expected found %"PRIx64", got %"PRIx64"\n", __func__, i, test_data_##T[i].foundFirst, found); \
            ++errors; \
        } \
        if (first != test_data_##T[i].foundFFirst) \
        { \
            fprintf(stderr, "%s (%d) Expected first %"PRIx64", got %"PRIx64"\n", __func__, i, test_data_##T[i].foundFFirst, first); \
            ++errors; \
        } \
        if (last != test_data_##T[i].foundFLast) \
        { \
            fprintf(stderr, "%s (%d) Expected last %"PRIx64", got %"PRIx64"\n", __func__, i, test_data_##T[i].foundFLast, last); \
            ++errors; \
        } \
    } \
    return errors; \
}

#define define_test_find_last(T) \
int test_find_last_##T(mmfile_t mf, uint64_t blklen) \
{ \
    int errors = 0; \
    int i; \
    uint64_t found, first, last; \
    for (i=0 ; i < TEST_DATA_SIZE; i++) \
    { \
        first = test_data_##T[i].first; \
        last = test_data_##T[i].last; \
        found = find_last_##T(mf.src, blklen, test_data_##T[i].blkpos, &first, &last, test_data_##T[i].search); \
        if (found != test_data_##T[i].foundLast) \
        { \
            fprintf(stderr, "%s (%d) Expected found %"PRIx64", got %"PRIx64"\n", __func__, i, test_data_##T[i].foundLast, found); \
            ++errors; \
        } \
        if (first != test_data_##T[i].foundLFirst) \
        { \
            fprintf(stderr, "%s (%d) Expected first %"PRIx64", got %"PRIx64"\n", __func__, i, test_data_##T[i].foundLFirst, first); \
            ++errors; \
        } \
        if (last != test_data_##T[i].foundLLast) \
        { \
            fprintf(stderr, "%s (%d) Expected last %"PRIx64", got %"PRIx64"\n", __func__, i, test_data_##T[i].foundLLast, last); \
            ++errors; \
        } \
    } \
    return errors; \
}

define_test_find_first(uint32_t)
define_test_find_last(uint32_t)
define_test_find_first(uint64_t)
define_test_find_last(uint64_t)
define_test_find_first(uint128_t)
define_test_find_last(uint128_t)

// returns current time in nanoseconds
uint64_t get_time()
{
    struct timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return (((uint64_t)t.tv_sec * 1000000000) + (uint64_t)t.tv_nsec);
}

#define define_benchmark_find_first(T) \
void benchmark_find_first_##T(mmfile_t mf, uint64_t blklen, uint64_t nitems) \
{ \
    uint64_t tstart, tend; \
    uint64_t first = 0; \
    uint64_t last = (nitems - 1); \
    int i; \
    int size = 10000; \
    tstart = get_time(); \
    for (i=0 ; i < size; i++) \
    { \
        find_first_##T(mf.src, blklen, test_data_##T[4].blkpos, &first, &last, test_data_##T[4].search); \
    } \
    tend = get_time(); \
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/(size*4)); \
}

#define define_benchmark_find_last(T) \
void benchmark_find_last_##T(mmfile_t mf, uint64_t blklen, uint64_t nitems) \
{ \
    uint64_t tstart, tend; \
    uint64_t first = 0; \
    uint64_t last = (nitems - 1); \
    int i; \
    int size = 10000; \
    tstart = get_time(); \
    for (i=0 ; i < size; i++) \
    { \
        find_last_##T(mf.src, blklen, test_data_##T[4].blkpos, &first, &last, test_data_##T[4].search); \
    } \
    tend = get_time(); \
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/(size*4)); \
}

define_benchmark_find_first(uint32_t)
define_benchmark_find_last(uint32_t)
define_benchmark_find_first(uint64_t)
define_benchmark_find_last(uint64_t)
define_benchmark_find_first(uint128_t)
define_benchmark_find_last(uint128_t)

int test_mmap_binfile_error(const char* file)
{
    mmfile_t mf = mmap_binfile(file);
    if (mf.src != MAP_FAILED)
    {
        fprintf(stderr, "An mmap error was expected\n");
        return 1;
    }
    return 0;
}

int test_munmap_binfile_error()
{
    mmfile_t mf;
    int e = munmap_binfile(mf);
    if (e == 0)
    {
        fprintf(stderr, "An mummap error was expected\n");
        return 1;
    }
    return 0;
}

int main()
{
    int errors = 0;

    errors += test_mmap_binfile_error("ERROR");
    errors += test_mmap_binfile_error("/dev/null");
    errors += test_munmap_binfile_error();

    char *file = "test_data.bin"; // file containing test data
    uint64_t blklen = 20; // length of each binary block
    uint64_t nitems; // number of binary blocks in the file

    mmfile_t mf = mmap_binfile(file);

    if (mf.fd < 0)
    {
        fprintf(stderr, "can't open %s for reading\n", file);
        return 1;
    }
    if (mf.size == 0)
    {
        fprintf(stderr, "fstat error! [%s]\n", strerror(errno));
        return 1;
    }
    if (mf.src == MAP_FAILED)
    {
        fprintf(stderr, "mmap error! [%s]\n", strerror(errno));
        return 1;
    }

    nitems = (uint64_t)(mf.size / blklen);
    if (nitems != 100)
    {
        fprintf(stderr, "Expecting 100 items, got instead: %"PRIu64"\n", nitems);
        return 1;
    }

    errors += test_find_first_uint32_t(mf, blklen);
    errors += test_find_last_uint32_t(mf, blklen);

    errors += test_find_first_uint64_t(mf, blklen);
    errors += test_find_last_uint64_t(mf, blklen);

    errors += test_find_first_uint128_t(mf, blklen);
    errors += test_find_last_uint128_t(mf, blklen);

    benchmark_find_first_uint32_t(mf, blklen, nitems);
    benchmark_find_last_uint32_t(mf, blklen, nitems);

    benchmark_find_first_uint64_t(mf, blklen, nitems);
    benchmark_find_last_uint64_t(mf, blklen, nitems);

    benchmark_find_first_uint128_t(mf, blklen, nitems);
    benchmark_find_last_uint128_t(mf, blklen, nitems);

    int e = munmap_binfile(mf);
    if (e != 0)
    {
        fprintf(stderr, "Got %d error while unmapping the file\n", e);
        return 1;
    }

    return errors;
}
