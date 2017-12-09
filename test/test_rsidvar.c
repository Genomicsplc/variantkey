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
    uint32_t chrom;
    uint32_t pos;
    uint64_t refalt;
} test_data_rv_t;

typedef struct test_data_vr_t
{
    uint32_t chrom;
    uint32_t pos;
    uint64_t refalt;
    uint32_t rsid;
} test_data_vr_t;

static test_data_rv_t test_data_rv[TEST_DATA_SIZE] =
{
    {0x2F81F572, 0x00000017, 0x0509C4A7, 0xCCD3C1D35DF0CE04},
    {0x2F81F573, 0x00000017, 0x05098728, 0x4EECA287F5C90712},
    {0x2F81F574, 0x0000000B, 0x054CAF51, 0xBB9EE46DF5C4C2E0},
    {0x2F81F575, 0x00000002, 0x095E7925, 0x90C4DEFF765F63B8},
    {0x2F81F576, 0x0000000F, 0x060C1468, 0x0981EF0F500126C2},
    {0x2F81F577, 0x00000011, 0x0199B8D6, 0x22A78FD91ACC7B43},
    {0x2F81F578, 0x0000000A, 0x07D8C4BE, 0x90C4DEFF765F63B8},
    {0x2F81F579, 0x00000001, 0x0DE63E19, 0x453EA229CAA9A652},
    {0x2F81F57A, 0x00000011, 0x026140E3, 0xCCD3C1D35DF0CE04},
    {0x2F81F57B, 0x00000011, 0x02610DFC, 0x26CAA8F5488B8717},
};

static test_data_vr_t test_data_vr[TEST_DATA_SIZE] =
{
    {0x0000001A, 0x00004005, 0x26CAA8F5488B8717, 0x0277648B},
    {0x0000001A, 0x00004006, 0x3D9C29BE7B4387A2, 0x020B67DE},
    {0x0000001A, 0x00004006, 0xCCD3C1D35DF0CE04, 0x020B67DE},
    {0x0000001A, 0x0000400E, 0x4EECA287F5C90712, 0x0849000D},
    {0x0000001A, 0x0000403C, 0x0981EF0F500126C2, 0x162BA962},
    {0x0000001A, 0x00004072, 0x4EECA287F5C90712, 0x167638AD},
    {0x0000001A, 0x0000407F, 0x0981EF0F500126C2, 0x1649ED55},
    {0x0000001A, 0x00004086, 0x0981EF0F500126C2, 0x003C1309},
    {0x0000001A, 0x0000408F, 0x22A78FD91ACC7B43, 0x170E8C03},
    {0x0000001A, 0x00004090, 0x0981EF0F500126C2, 0x161885A7},
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
            fprintf(stderr, "test_get_rv_varhash (%d) Expected refalt %"PRIx64", got %"PRIx64"\n", i, test_data_rv[i].refalt, vh.refalt);
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
            fprintf(stderr, "test_find_rv_varhash_by_rsid (%d) Expected refalt %"PRIx64", got %"PRIx64"\n", i, test_data_rv[i].refalt, vh.refalt);
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
        fprintf(stderr, "test_find_rv_varhash_by_rsid_error : Expected refalt 0, got %"PRIu64"\n", vh.refalt);
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
    rsid = find_vr_chrompos_range(vr.src, &first, &last, test_data_vr[0].chrom, test_data_vr[3].pos, test_data_vr[7].pos);
    if (rsid != test_data_vr[3].rsid)
    {
        fprintf(stderr, "test_find_vr_chrompos_range : Expected rsid %"PRIx32", got %"PRIx32"\n", test_data_vr[3].rsid, rsid);
        ++errors;
    }
    if (first != 3)
    {
        fprintf(stderr, "test_find_vr_chrompos_range : Expected first 3, got %"PRIu64"\n", first);
        ++errors;
    }
    if (last != 7)
    {
        fprintf(stderr, "test_find_vr_chrompos_range : Expected last 7, got %"PRIu64"\n", last);
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
    vh.chrom = 0xfffffff0;
    vh.pos = 0xfffffff0;
    vh.refalt = 0xfffffffffffffff0;
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
        find_vr_chrompos_range(vr.src, &first, &last, 0x0000001A, 0x0000400E, 0x00004086);
    }
    tend = get_time();
    fprintf(stdout, " * find_vr_chrompos_range : %lu ns/op\n", (tend - tstart)/(size*4));
}

void benchmark_find_vr_rsid_by_varhash(mmfile_t vr)
{
    uint64_t tstart, tend;
    uint64_t first = 0;
    varhash_t vh;
    vh.chrom = 0x0000001A;
    vh.pos = 0x0000400E;
    vh.refalt = 0x4EECA287F5C90712;
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
