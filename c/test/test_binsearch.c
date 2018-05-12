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
#include "../src/binsearch.h"

#define TEST_DATA_SIZE 13

typedef struct t_test_uint8_t
{
    uint64_t blkpos;
    uint64_t first;
    uint64_t last;
    uint8_t search;
    uint64_t foundFirst;
    uint64_t foundFFirst;
    uint64_t foundFLast;
    uint64_t foundLast;
    uint64_t foundLFirst;
    uint64_t foundLLast;
} t_test_uint8_t;

static t_test_uint8_t test_data_uint8_t[TEST_DATA_SIZE] =
{
    {6, 0, 99,  0x27, 0, 0, 1, 18, 19, 18},
    {6, 0, 99,  0x33, 87, 87, 86, 99, 100, 99},
    {0, 0, 99,  0x00, 0, 0, 1, 99, 100, 99},
    {6, 0, 99,  0xff, 100, 100, 99, 100, 100, 99},
    {6, 0, 99,  0x27, 0, 0, 1, 18, 19, 18},
    {6, 13, 99, 0x27, 13, 13, 12, 18, 19, 18},
    {6, 14, 99, 0x27, 14, 14, 13, 18, 19, 18},
    {6, 0, 0,   0x01, 1, 0, 0, 1, 0, 0},
    {6, 0, 0,   0xf0, 1, 1, 0, 1, 1, 0},
    {6, 99, 99, 0x01, 100, 99, 98, 100, 99, 98},
    {6, 99, 99, 0xf0, 100, 100, 99, 100, 100, 99},
    {6, 0, 99,  0x28, 19, 19, 18, 44, 45, 44},
    {6, 0, 99,  0x2A, 57, 57, 56, 61, 62, 61},
};

static t_test_uint8_t test_data_sub_uint8_t[TEST_DATA_SIZE] =
{
    {6, 0, 99, 0x4, 0, 0, 1, 18, 19, 18},
    {6, 0, 99, 0x6, 80, 80, 79, 99, 100, 99},
    {0, 0, 99, 0x0, 0, 0, 1, 99, 100, 99},
    {6, 0, 99, 0x1f, 100, 100, 99, 100, 100, 99},
    {6, 0, 99, 0x4, 0, 0, 1, 18, 19, 18},
    {6, 13, 99, 0x4, 13, 13, 12, 18, 19, 18},
    {6, 14, 99, 0x4, 14, 14, 13, 18, 19, 18},
    {6, 0, 0, 0x0, 1, 0, 0, 1, 0, 0},
    {6, 0, 0, 0x1e, 1, 1, 0, 1, 1, 0},
    {6, 99, 99, 0x0, 100, 99, 98, 100, 99, 98},
    {6, 99, 99, 0x1e, 100, 100, 99, 100, 100, 99},
    {6, 0, 99, 0x5, 19, 19, 18, 79, 80, 79},
    {6, 0, 99, 0x5, 19, 19, 18, 79, 80, 79},
};

typedef struct t_test_uint16_t
{
    uint64_t blkpos;
    uint64_t first;
    uint64_t last;
    uint16_t search;
    uint64_t foundFirst;
    uint64_t foundFFirst;
    uint64_t foundFLast;
    uint64_t foundLast;
    uint64_t foundLFirst;
    uint64_t foundLLast;
} t_test_uint16_t;

static t_test_uint16_t test_data_uint16_t[TEST_DATA_SIZE] =
{
    {6, 0, 99,  0x2722, 0, 0, 1, 0, 1, 0},
    {6, 0, 99,  0x33f5, 99, 99, 98, 99, 100, 99},
    {0, 0, 99,  0x0001, 100, 100, 99, 100, 100, 99},
    {6, 0, 99,  0xf7f3, 100, 100, 99, 100, 100, 99},
    {6, 0, 99,  0x27f3, 13, 13, 12, 14, 15, 14},
    {6, 13, 99, 0x27f3, 13, 13, 12, 14, 15, 14},
    {6, 14, 99, 0x27f3, 14, 14, 13, 14, 15, 14},
    {6, 0, 0,   0x0001, 1, 0, 0, 1, 0, 0},
    {6, 0, 0,   0xfff0, 1, 1, 0, 1, 1, 0},
    {6, 99, 99, 0x0001, 100, 99, 98, 100, 99, 98},
    {6, 99, 99, 0xfff0, 100, 100, 99, 100, 100, 99},
    {6, 0, 99,  0x2805, 100, 19, 18, 100, 19, 18},
    {6, 0, 99,  0x28ff, 100, 45, 44, 100, 45, 44},
};

static t_test_uint16_t test_data_sub_uint16_t[TEST_DATA_SIZE] =
{
    {6, 0, 99, 0x4e4, 0, 0, 1, 0, 1, 0},
    {6, 0, 99, 0x67e, 99, 99, 98, 99, 100, 99},
    {0, 0, 99, 0x0, 0, 0, 1, 99, 100, 99},
    {6, 0, 99, 0x1efe, 100, 100, 99, 100, 100, 99},
    {6, 0, 99, 0x4fe, 13, 13, 12, 16, 17, 16},
    {6, 13, 99, 0x4fe, 13, 13, 12, 16, 17, 16},
    {6, 14, 99, 0x4fe, 14, 14, 13, 16, 17, 16},
    {6, 0, 0, 0x0, 1, 0, 0, 1, 0, 0},
    {6, 0, 0, 0x1ffe, 1, 1, 0, 1, 1, 0},
    {6, 99, 99, 0x0, 100, 99, 98, 100, 99, 98},
    {6, 99, 99, 0x1ffe, 100, 100, 99, 100, 100, 99},
    {6, 0, 99, 0x500, 19, 19, 18, 20, 21, 20},
    {6, 0, 99, 0x51f, 41, 41, 40, 44, 45, 44},
};

typedef struct t_test_uint32_t
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
} t_test_uint32_t;

static t_test_uint32_t test_data_uint32_t[TEST_DATA_SIZE] =
{
    {4, 0, 99,  0x00002722, 0, 0, 1, 0, 1, 0},
    {4, 0, 99,  0x000033f5, 99, 99, 98, 99, 100, 99},
    {0, 0, 99,  0x00000001, 0, 0, 1, 99, 100, 99},
    {4, 0, 99,  0xf00027f3, 100, 100, 99, 100, 100, 99},
    {4, 0, 99,  0x000027f3, 13, 13, 12, 14, 15, 14},
    {4, 13, 99, 0x000027f3, 13, 13, 12, 14, 15, 14},
    {4, 14, 99, 0x000027f3, 14, 14, 13, 14, 15, 14},
    {4, 0, 0,   0x00000001, 1, 0, 0, 1, 0, 0},
    {4, 0, 0,   0xfffffff0, 1, 1, 0, 1, 1, 0},
    {4, 99, 99, 0x00000001, 100, 99, 98, 100, 99, 98},
    {4, 99, 99, 0xfffffff0, 100, 100, 99, 100, 100, 99},
    {4, 0, 99,  0x00002805, 100, 19, 18, 100, 19, 18},
    {4, 0, 99,  0x000028ff, 100, 45, 44, 100, 45, 44},
};

static t_test_uint32_t test_data_sub_uint32_t[TEST_DATA_SIZE] =
{
    {4, 0, 99, 0x4e4, 0, 0, 1, 0, 1, 0},
    {4, 0, 99, 0x67e, 99, 99, 98, 99, 100, 99},
    {0, 0, 99, 0x0, 0, 0, 1, 99, 100, 99},
    {4, 0, 99, 0x60004fe, 100, 100, 99, 100, 100, 99},
    {4, 0, 99, 0x4fe, 13, 13, 12, 16, 17, 16},
    {4, 13, 99, 0x4fe, 13, 13, 12, 16, 17, 16},
    {4, 14, 99, 0x4fe, 14, 14, 13, 16, 17, 16},
    {4, 0, 0, 0x0, 1, 0, 0, 1, 0, 0},
    {4, 0, 0, 0x7fffffe, 1, 1, 0, 1, 1, 0},
    {4, 99, 99, 0x0, 100, 99, 98, 100, 99, 98},
    {4, 99, 99, 0x7fffffe, 100, 100, 99, 100, 100, 99},
    {4, 0, 99, 0x500, 19, 19, 18, 20, 21, 20},
    {4, 0, 99, 0x51f, 41, 41, 40, 44, 45, 44},
};

typedef struct t_test_uint64_t
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
} t_test_uint64_t;

static t_test_uint64_t test_data_uint64_t[TEST_DATA_SIZE] =
{
    {4, 0, 99,  0x000027225fb6e591, 0, 0, 1, 0, 1, 0},
    {4, 0, 99,  0x000033f522a78fd9, 99, 99, 98, 99, 100, 99},
    {0, 0, 99,  0x0000000100002722, 0, 0, 1, 0, 1, 0},
    {4, 0, 99,  0xf0000001f00027f3, 100, 100, 99, 100, 100, 99},
    {4, 0, 99,  0x000027f35fb6e591, 13, 13, 12, 13, 14, 13},
    {0, 13, 99, 0x00000001000027f3, 13, 13, 12, 14, 15, 14},
    {0, 14, 99, 0x00000001000027f3, 14, 14, 13, 14, 15, 14},
    {4, 0, 0,   0x0000000000000001, 1, 0, 0, 1, 0, 0},
    {4, 0, 0,   0xfffffffffffffff0, 1, 1, 0, 1, 1, 0},
    {4, 99, 99, 0x0000000000000001, 100, 99, 98, 100, 99, 98},
    {4, 99, 99, 0xfffffffffffffff0, 100, 100, 99, 100, 100, 99},
    {4, 0, 99,  0x000028060981ef0a, 100, 19, 18, 100, 19, 18},
    {4, 0, 99,  0x000028fca24c9149, 100, 45, 44, 100, 45, 44},
};

static t_test_uint64_t test_data_sub_uint64_t[TEST_DATA_SIZE] =
{
    {4, 0, 99, 0x4e44bf6dcb2, 0, 0, 1, 0, 1, 0},
    {4, 0, 99, 0x67ea454f1fb, 99, 99, 98, 99, 100, 99},
    {0, 0, 99, 0x200004e4, 0, 0, 1, 0, 1, 0},
    {4, 0, 99, 0x60000003e0004fe, 100, 100, 99, 100, 100, 99},
    {4, 0, 99, 0x4fe6bf6dcb2, 13, 13, 12, 13, 14, 13},
    {0, 13, 99, 0x200004fe, 13, 13, 12, 16, 17, 16},
    {0, 14, 99, 0x200004fe, 14, 14, 13, 16, 17, 16},
    {4, 0, 0, 0x0, 1, 0, 0, 1, 0, 0},
    {4, 0, 0, 0x7fffffffffffffe, 1, 1, 0, 1, 1, 0},
    {4, 99, 99, 0x0, 100, 99, 98, 100, 99, 98},
    {4, 99, 99, 0x7fffffffffffffe, 100, 100, 99, 100, 100, 99},
    {4, 0, 99, 0x500c1303de1, 19, 19, 18, 19, 20, 19},
    {4, 0, 99, 0x51f94499229, 44, 44, 43, 44, 45, 44},
};

#define define_test_find_first(T) \
int test_find_first_##T(mmfile_t mf, uint64_t blklen, uint8_t bitstart, uint8_t bitend) \
{ \
    int errors = 0; \
    int i; \
    uint64_t found, first, last; \
    for (i=0 ; i < TEST_DATA_SIZE; i++) \
    { \
        first = test_data_##T[i].first; \
        last = test_data_##T[i].last; \
        found = find_first_##T(mf.src, blklen, test_data_##T[i].blkpos, bitstart, bitend, &first, &last, test_data_##T[i].search); \
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
        first = test_data_sub_##T[i].first; \
        last = test_data_sub_##T[i].last; \
        found = find_first_##T(mf.src, blklen, test_data_sub_##T[i].blkpos, bitstart + 2, bitend - 3, &first, &last, test_data_sub_##T[i].search); \
        if (found != test_data_sub_##T[i].foundFirst) \
        { \
            fprintf(stderr, "%s SUB (%d) Expected found %"PRIx64", got %"PRIx64"\n", __func__, i, test_data_sub_##T[i].foundFirst, found); \
            ++errors; \
        } \
        if (first != test_data_sub_##T[i].foundFFirst) \
        { \
            fprintf(stderr, "%s SUB (%d) Expected first %"PRIx64", got %"PRIx64"\n", __func__, i, test_data_sub_##T[i].foundFFirst, first); \
            ++errors; \
        } \
        if (last != test_data_sub_##T[i].foundFLast) \
        { \
            fprintf(stderr, "%s SUB (%d) Expected last %"PRIx64", got %"PRIx64"\n", __func__, i, test_data_sub_##T[i].foundFLast, last); \
            ++errors; \
        } \
    } \
    return errors; \
}

#define define_test_find_last(T) \
int test_find_last_##T(mmfile_t mf, uint64_t blklen, uint8_t bitstart, uint8_t bitend) \
{ \
    int errors = 0; \
    int i; \
    uint64_t found, first, last; \
    for (i=0 ; i < TEST_DATA_SIZE; i++) \
    { \
        first = test_data_##T[i].first; \
        last = test_data_##T[i].last; \
        found = find_last_##T(mf.src, blklen, test_data_##T[i].blkpos, bitstart, bitend, &first, &last, test_data_##T[i].search); \
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
        first = test_data_sub_##T[i].first; \
        last = test_data_sub_##T[i].last; \
        found = find_last_##T(mf.src, blklen, test_data_sub_##T[i].blkpos, bitstart + 2, bitend - 3, &first, &last, test_data_sub_##T[i].search); \
        if (found != test_data_sub_##T[i].foundLast) \
        { \
            fprintf(stderr, "%s (%d) Expected found %"PRIx64", got %"PRIx64"\n", __func__, i, test_data_sub_##T[i].foundLast, found); \
            ++errors; \
        } \
        if (first != test_data_sub_##T[i].foundLFirst) \
        { \
            fprintf(stderr, "%s (%d) Expected first %"PRIx64", got %"PRIx64"\n", __func__, i, test_data_sub_##T[i].foundLFirst, first); \
            ++errors; \
        } \
        if (last != test_data_sub_##T[i].foundLLast) \
        { \
            fprintf(stderr, "%s (%d) Expected last %"PRIx64", got %"PRIx64"\n", __func__, i, test_data_sub_##T[i].foundLLast, last); \
            ++errors; \
        } \
    } \
    return errors; \
}

define_test_find_first(uint8_t)
define_test_find_last(uint8_t)
define_test_find_first(uint16_t)
define_test_find_last(uint16_t)
define_test_find_first(uint32_t)
define_test_find_last(uint32_t)
define_test_find_first(uint64_t)
define_test_find_last(uint64_t)

// returns current time in nanoseconds
uint64_t get_time()
{
    struct timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return (((uint64_t)t.tv_sec * 1000000000) + (uint64_t)t.tv_nsec);
}

#define define_benchmark_find_first(T) \
void benchmark_find_first_##T(mmfile_t mf, uint64_t blklen, uint8_t bitstart, uint8_t bitend, uint64_t nitems) \
{ \
    uint64_t tstart, tend; \
    uint64_t first = 0; \
    uint64_t last = (nitems - 1); \
    int i; \
    int size = 10000; \
    tstart = get_time(); \
    for (i=0 ; i < size; i++) \
    { \
        find_first_##T(mf.src, blklen, test_data_##T[4].blkpos, bitstart, bitend, &first, &last, test_data_##T[4].search); \
    } \
    tend = get_time(); \
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/(size*4)); \
}

#define define_benchmark_find_last(T) \
void benchmark_find_last_##T(mmfile_t mf, uint64_t blklen, uint8_t bitstart, uint8_t bitend, uint64_t nitems) \
{ \
    uint64_t tstart, tend; \
    uint64_t first = 0; \
    uint64_t last = (nitems - 1); \
    int i; \
    int size = 10000; \
    tstart = get_time(); \
    for (i=0 ; i < size; i++) \
    { \
        find_last_##T(mf.src, blklen, test_data_##T[4].blkpos, bitstart, bitend, &first, &last, test_data_##T[4].search); \
    } \
    tend = get_time(); \
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/(size*4)); \
}

define_benchmark_find_first(uint8_t)
define_benchmark_find_last(uint8_t)
define_benchmark_find_first(uint16_t)
define_benchmark_find_last(uint16_t)
define_benchmark_find_first(uint32_t)
define_benchmark_find_last(uint32_t)
define_benchmark_find_first(uint64_t)
define_benchmark_find_last(uint64_t)

int test_mmap_binfile_error(const char* file)
{
    mmfile_t mf = {0};
    mmap_binfile(file, &mf);
    if (mf.src != MAP_FAILED)
    {
        fprintf(stderr, "An mmap error was expected\n");
        return 1;
    }
    return 0;
}

int test_munmap_binfile_error()
{
    mmfile_t mf = {0};
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

    mmfile_t mf = {0};
    mmap_binfile(file, &mf);

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

    errors += test_find_first_uint8_t(mf, blklen, 0, 7);
    errors += test_find_last_uint8_t(mf, blklen, 0, 7);

    errors += test_find_first_uint16_t(mf, blklen, 0, 15);
    errors += test_find_last_uint16_t(mf, blklen, 0, 15);

    errors += test_find_first_uint32_t(mf, blklen, 0, 31);
    errors += test_find_last_uint32_t(mf, blklen, 0, 31);

    errors += test_find_first_uint64_t(mf, blklen, 0, 63);
    errors += test_find_last_uint64_t(mf, blklen, 0, 63);

    benchmark_find_first_uint8_t(mf, blklen, nitems, 0, 7);
    benchmark_find_last_uint8_t(mf, blklen, nitems, 0, 7);

    benchmark_find_first_uint16_t(mf, blklen, 0, 15, nitems);
    benchmark_find_last_uint16_t(mf, blklen, 0, 15, nitems);

    benchmark_find_first_uint32_t(mf, blklen, 0, 31, nitems);
    benchmark_find_last_uint32_t(mf, blklen, 0, 31, nitems);

    benchmark_find_first_uint64_t(mf, blklen, 0, 63, nitems);
    benchmark_find_last_uint64_t(mf, blklen, 0, 63, nitems);

    int e = munmap_binfile(mf);
    if (e != 0)
    {
        fprintf(stderr, "Got %d error while unmapping the file\n", e);
        return 1;
    }

    return errors;
}
