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
#include "rsidvar.h"

#define TEST_DATA_SIZE 10

typedef struct test_data_rv_t
{
    uint32_t rsid;
    uint32_t assembly;
    uint32_t chrom;
    uint32_t pos;
    uint32_t refalt;
} test_data_rv_t;

typedef struct test_data_vr_t
{
    uint32_t assembly;
    uint32_t chrom;
    uint32_t pos;
    uint32_t refalt;
    uint32_t rsid;
} test_data_vr_t;

static test_data_rv_t test_data_rv[TEST_DATA_SIZE] =
{
    {0x00000001, 0x8b29d2c7, 0x00000005, 0x00006edf, 0x387351cb},
    {0x00000002, 0x8b29d2c7, 0x00000007, 0x0022e9c0, 0xd0b64ba3},
    {0x00000003, 0x8b29d2c7, 0x00000004, 0x0000ec56, 0xcef1000f},
    {0x00000004, 0x8b29d2c7, 0x00000008, 0x00028f54, 0x8a59f7ae},
    {0x00000005, 0x8b29d2c7, 0x00000003, 0x000124a3, 0x4c61400f},
    {0x00000006, 0x8b29d2c7, 0x00000009, 0x000143fc, 0x6bddcdbc},
    {0x00000007, 0x8b29d2c7, 0x00000002, 0x00006d6d, 0x181d293a},
    {0x00000008, 0x8b29d2c7, 0x0000000a, 0x00019015, 0x387351cb},
    {0x00000009, 0x8b29d2c7, 0x00000001, 0x0004f442, 0x387351cb},
    {0x0000000a, 0x8b29d2c7, 0x0000000b, 0x00032edc, 0x145dc65c},
};

static test_data_vr_t test_data_vr[TEST_DATA_SIZE] =
{
    {0x8b29d2c7, 0x00000001, 0x0004f442, 0x387351cb, 0x00000009},
    {0x8b29d2c7, 0x00000002, 0x00006d6d, 0x181d293a, 0x00000007},
    {0x8b29d2c7, 0x00000003, 0x000124a3, 0x4c61400f, 0x00000005},
    {0x8b29d2c7, 0x00000004, 0x0000ec56, 0xcef1000f, 0x00000003},
    {0x8b29d2c7, 0x00000005, 0x00006edf, 0x387351cb, 0x00000001},
    {0x8b29d2c7, 0x00000007, 0x0022e9c0, 0xd0b64ba3, 0x00000002},
    {0x8b29d2c7, 0x00000008, 0x00028f54, 0x8a59f7ae, 0x00000004},
    {0x8b29d2c7, 0x00000009, 0x000143fc, 0x6bddcdbc, 0x00000006},
    {0x8b29d2c7, 0x0000000a, 0x00019015, 0x387351cb, 0x00000008},
    {0x8b29d2c7, 0x0000000b, 0x00032edc, 0x145dc65c, 0x0000000a},
};

// returns current time in nanoseconds
uint64_t get_time()
{
    struct timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return (((uint64_t)t.tv_sec * 1000000000) + (uint64_t)t.tv_nsec);
}

int test_get_vr_rsid(mmfile_t vr)
{
    int errors = 0;
    int i;
    uint32_t rsid;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        rsid = get_vr_rsid(vr.src, i);
        if (rsid != test_data_vr[i].rsid)
        {
            fprintf(stderr, "test_get_vr_rsid (%d) Expected %"PRIx32", got %"PRIx32"\n", i, test_data_vr[i].rsid, rsid);
            ++errors;
        }
    }
    return errors;
}

int test_get_rv_varhash(mmfile_t rv)
{
    int errors = 0;
    int i;
    varhash_t vh;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        vh = get_rv_varhash(rv.src, i);
        if (vh.assembly != test_data_rv[i].assembly)
        {
            fprintf(stderr, "test_get_rv_varhash (%d) Expected assembly %"PRIx32", got %"PRIx32"\n", i, test_data_rv[i].assembly, vh.assembly);
            ++errors;
        }
        if (vh.chrom != test_data_rv[i].chrom)
        {
            fprintf(stderr, "test_get_rv_varhash (%d) Expected chrom %"PRIx32", got %"PRIx32"\n", i, test_data_rv[i].chrom, vh.chrom);
            ++errors;
        }
        if (vh.pos != test_data_rv[i].pos)
        {
            fprintf(stderr, "test_get_rv_varhash (%d) Expected pos %"PRIx32", got %"PRIx32"\n", i, test_data_rv[i].pos, vh.pos);
            ++errors;
        }
        if (vh.refalt != test_data_rv[i].refalt)
        {
            fprintf(stderr, "test_get_rv_varhash (%d) Expected refalt %"PRIx32", got %"PRIx32"\n", i, test_data_rv[i].refalt, vh.refalt);
            ++errors;
        }
    }
    return errors;
}

int test_find_rv_varhash_by_rsid(mmfile_t rv)
{
    int errors = 0;
    int i;
    varhash_t vh;
    uint64_t first;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        first = 0;
        vh = find_rv_varhash_by_rsid(rv.src, &first, 9, test_data_rv[i].rsid);
        if (first != (uint64_t)i)
        {
            fprintf(stderr, "test_find_rv_varhash_by_rsid (%d) Expected first %d, got %"PRIu64"\n", i, i, first);
            ++errors;
        }
        if (vh.assembly != test_data_rv[i].assembly)
        {
            fprintf(stderr, "test_find_rv_varhash_by_rsid (%d) Expected assembly %"PRIx32", got %"PRIx32"\n", i, test_data_rv[i].assembly, vh.assembly);
            ++errors;
        }
        if (vh.chrom != test_data_rv[i].chrom)
        {
            fprintf(stderr, "test_find_rv_varhash_by_rsid (%d) Expected chrom %"PRIx32", got %"PRIx32"\n", i, test_data_rv[i].chrom, vh.chrom);
            ++errors;
        }
        if (vh.pos != test_data_rv[i].pos)
        {
            fprintf(stderr, "test_find_rv_varhash_by_rsid (%d) Expected pos %"PRIx32", got %"PRIx32"\n", i, test_data_rv[i].pos, vh.pos);
            ++errors;
        }
        if (vh.refalt != test_data_rv[i].refalt)
        {
            fprintf(stderr, "test_find_rv_varhash_by_rsid (%d) Expected refalt %"PRIx32", got %"PRIx32"\n", i, test_data_rv[i].refalt, vh.refalt);
            ++errors;
        }
    }
    return errors;
}

int test_find_rv_varhash_by_rsid_notfound(mmfile_t rv)
{
    int errors = 0;
    varhash_t vh;
    uint64_t first = 0;
    vh = find_rv_varhash_by_rsid(rv.src, &first, 9, 0xfffffff0);
    if (first != 10)
    {
        fprintf(stderr, "test_find_rv_varhash_by_rsid_error : Expected first 10, got %"PRIu64"\n", first);
        ++errors;
    }
    if (vh.assembly != 0)
    {
        fprintf(stderr, "test_find_rv_varhash_by_rsid_error : Expected assembly 0, got %"PRIu32"\n", vh.assembly);
        ++errors;
    }
    if (vh.chrom != 0)
    {
        fprintf(stderr, "test_find_rv_varhash_by_rsid_error : Expected chrom 0, got %"PRIu32"\n", vh.chrom);
        ++errors;
    }
    if (vh.pos != 0)
    {
        fprintf(stderr, "test_find_rv_varhash_by_rsid_error : Expected pos 0, got %"PRIu32"\n", vh.pos);
        ++errors;
    }
    if (vh.refalt != 0)
    {
        fprintf(stderr, "test_find_rv_varhash_by_rsid_error : Expected refalt 0, got %"PRIu32"\n", vh.refalt);
        ++errors;
    }
    return errors;
}

int test_find_vr_chrompos_range(mmfile_t vr)
{
    int errors = 0;
    uint32_t rsid;
    uint64_t first = 0;
    uint64_t last = 9;
    rsid = find_vr_chrompos_range(vr.src, &first, &last, test_data_vr[4].chrom, test_data_vr[4].pos, test_data_vr[4].pos);
    if (rsid != test_data_vr[4].rsid)
    {
        fprintf(stderr, "test_find_vr_chrompos_range : Expected rsid %"PRIx32", got %"PRIx32"\n", test_data_vr[4].rsid, rsid);
        ++errors;
    }
    if (first != 4)
    {
        fprintf(stderr, "test_find_vr_chrompos_range : Expected first 4, got %"PRIu64"\n", first);
        ++errors;
    }
    if (last != 4)
    {
        fprintf(stderr, "test_find_vr_chrompos_range : Expected last 4, got %"PRIu64"\n", last);
        ++errors;
    }
    return errors;
}

int test_find_vr_chrompos_range_notfound(mmfile_t vr)
{
    int errors = 0;
    uint32_t rsid;
    uint64_t first = 0;
    uint64_t last = 9;
    rsid = find_vr_chrompos_range(vr.src, &first, &last, 0xfffffff0, 0xffffff00, 0xfffffff0);
    if (rsid != 0)
    {
        fprintf(stderr, "test_find_vr_chrompos_range_notfound : Expected rsid 0, got %"PRIx32"\n", rsid);
        ++errors;
    }
    if (first != 10)
    {
        fprintf(stderr, "test_find_vr_chrompos_range_notfound : Expected first 10, got %"PRIu64"\n", first);
        ++errors;
    }
    if (last != 9)
    {
        fprintf(stderr, "test_find_vr_chrompos_range_notfound : Expected last 9, got %"PRIu64"\n", last);
        ++errors;
    }
    return errors;
}

int test_find_vr_rsid_by_varhash(mmfile_t vr)
{
    int errors = 0;
    int i;
    uint32_t rsid;
    uint64_t first;
    varhash_t vh;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        first = 0;
        vh.assembly = test_data_vr[i].assembly;
        vh.chrom = test_data_vr[i].chrom;
        vh.pos = test_data_vr[i].pos;
        vh.refalt = test_data_vr[i].refalt;
        rsid = find_vr_rsid_by_varhash(vr.src, &first, 9, vh);
        if (rsid != test_data_vr[i].rsid)
        {
            fprintf(stderr, "find_vr_rsid_by_varhash (%d) Expected rsid %"PRIx32", got %"PRIx32"\n", i, test_data_vr[i].rsid, rsid);
            ++errors;
        }
        if (first != (uint64_t)i)
        {
            fprintf(stderr, "find_vr_rsid_by_varhash (%d) Expected first %d, got %"PRIu64"\n", i, i, first);
            ++errors;
        }
    }
    return errors;
}

int test_find_vr_rsid_by_varhash_notfound(mmfile_t vr)
{
    int errors = 0;
    uint32_t rsid;
    uint64_t first;
    varhash_t vh;
    first = 0;
    vh.assembly = 0xfffffff0;
    vh.chrom = 0xfffffff0;
    vh.pos = 0xfffffff0;
    vh.refalt = 0xfffffff0;
    rsid = find_vr_rsid_by_varhash(vr.src, &first, 9, vh);
    if (rsid != 0)
    {
        fprintf(stderr, "find_vr_rsid_by_varhash_notfound : Expected rsid 0, got %"PRIx32"\n", rsid);
        ++errors;
    }
    if (first != 10)
    {
        fprintf(stderr, "find_vr_rsid_by_varhash_notfound :  Expected first 10, got %"PRIu64"\n", first);
        ++errors;
    }
    return errors;
}

void benchmark_get_vr_rsid(mmfile_t vr)
{
    uint64_t tstart, tend;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        get_vr_rsid(vr.src, 3);
    }
    tend = get_time();
    fprintf(stdout, " * get_vr_rsid : %lu ns/op\n", (tend - tstart)/(size*4));
}

void benchmark_get_rv_varhash(mmfile_t rv)
{
    uint64_t tstart, tend;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        get_rv_varhash(rv.src, 3);
    }
    tend = get_time();
    fprintf(stdout, " * get_rv_varhash : %lu ns/op\n", (tend - tstart)/(size*4));
}

void benchmark_find_rv_varhash_by_rsid(mmfile_t rv)
{
    uint64_t tstart, tend;
    uint64_t first = 0;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        find_rv_varhash_by_rsid(rv.src, &first, 9, 0x2F81F575);
    }
    tend = get_time();
    fprintf(stdout, " * find_rv_varhash_by_rsid : %lu ns/op\n", (tend - tstart)/(size*4));
}

void benchmark_find_vr_chrompos_range(mmfile_t vr)
{
    uint64_t tstart, tend;
    uint64_t first = 0, last = 9;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        find_vr_chrompos_range(vr.src, &first, &last, 0x00000005, 0x00006f88, 0x00006ed7);
    }
    tend = get_time();
    fprintf(stdout, " * find_vr_chrompos_range : %lu ns/op\n", (tend - tstart)/(size*4));
}

void benchmark_find_vr_rsid_by_varhash(mmfile_t vr)
{
    uint64_t tstart, tend;
    uint64_t first = 0;
    varhash_t vh;
    vh.assembly = 0x8b29d2c7;
    vh.chrom = 0x00000003;
    vh.pos = 0x000124a3;
    vh.refalt = 0x8ffb1a03;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        find_vr_rsid_by_varhash(vr.src, &first, 9, vh);
    }
    tend = get_time();
    fprintf(stdout, " * find_vr_rsid_by_varhash : %lu ns/op\n", (tend - tstart)/(size*4));
}

int main()
{
    int errors = 0;
    int err;

    uint64_t nitems; // number of binary blocks in the file

    mmfile_t rv = mmap_binfile("rsid_varhash.10.bin");
    nitems = (uint64_t)(rv.size / RSIDVAR_BIN_BLKLEN);
    if (nitems != TEST_DATA_SIZE)
    {
        fprintf(stderr, "Expecting %d items, got instead: %"PRIu64"\n", TEST_DATA_SIZE, nitems);
        return 1;
    }

    mmfile_t vr = mmap_binfile("varhash_rsid.10.bin");
    nitems = (uint64_t)(vr.size / RSIDVAR_BIN_BLKLEN);
    if (nitems != TEST_DATA_SIZE)
    {
        fprintf(stderr, "Expecting %d items, got instead: %"PRIu64"\n", TEST_DATA_SIZE, nitems);
        return 1;
    }

    errors += test_get_vr_rsid(vr);
    errors += test_get_rv_varhash(rv);
    errors += test_find_rv_varhash_by_rsid(rv);
    errors += test_find_rv_varhash_by_rsid_notfound(rv);
    errors += test_find_vr_chrompos_range(vr);
    errors += test_find_vr_chrompos_range_notfound(vr);
    errors += test_find_vr_rsid_by_varhash(vr);
    errors += test_find_vr_rsid_by_varhash_notfound(vr);

    benchmark_get_vr_rsid(vr);
    benchmark_get_rv_varhash(rv);
    benchmark_find_rv_varhash_by_rsid(rv);
    benchmark_find_vr_chrompos_range(vr);
    benchmark_find_vr_rsid_by_varhash(vr);

    err = munmap_binfile(rv);
    if (err != 0)
    {
        fprintf(stderr, "Got %d error while unmapping the rv file\n", err);
        return 1;
    }

    err = munmap_binfile(vr);
    if (err != 0)
    {
        fprintf(stderr, "Got %d error while unmapping the vr file\n", err);
        return 1;
    }

    return errors;
}
