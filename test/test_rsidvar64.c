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
#include "rsidvar64.h"

#define TEST_DATA_SIZE 10

typedef struct test_data_64_t
{
    uint32_t rsid;
    uint8_t chrom;
    uint32_t pos;
    uint32_t refalt;
    uint64_t vh;
} test_data_64_t;

static test_data_64_t test_data[TEST_DATA_SIZE] =
{
    {0X00000001, 0X01, 0X000FDDC2, 0X0A50E0, 0X01000FDDC20A50E0},
    {0X00000007, 0X09, 0X001173EF, 0X038400, 0X09001173EF038400},
    {0X0000000B, 0X09, 0X00130A1C, 0X038400, 0X0900130A1C038400},
    {0X00000061, 0X16, 0X0014A049, 0XCC6140, 0X160014A049CC6140},
    {0X00000065, 0X16, 0X00163676, 0X109C20, 0X1600163676109C20},
    {0X000003E5, 0X16, 0X0017CCA3, 0X13D0E0, 0X160017CCA313D0E0},
    {0X000003F1, 0X19, 0X001962D0, 0X13D0E0, 0X19001962D013D0E0},
    {0X000026F5, 0X19, 0X001AF8FD, 0X0A0400, 0X19001AF8FD0A0400},
    {0X000186A3, 0X19, 0X001C8F2A, 0X038400, 0X19001C8F2A038400},
    {0X00019919, 0X19, 0X001E2557, 0X01D000, 0X19001E255701D000},

};

// returns current time in nanoseconds
uint64_t get_time()
{
    struct timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return (((uint64_t)t.tv_sec * 1000000000) + (uint64_t)t.tv_nsec);
}

int test_get_vr64_rsid(mmfile_t vr)
{
    int errors = 0;
    int i;
    uint32_t rsid;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        rsid = get_vr64_rsid(vr.src, i);
        if (rsid != test_data[i].rsid)
        {
            fprintf(stderr, "%s (%d) Expected %"PRIx32", got %"PRIx32"\n", __func__, i, test_data[i].rsid, rsid);
            ++errors;
        }
    }
    return errors;
}

int test_get_rv64_varhash(mmfile_t rv)
{
    int errors = 0;
    int i;
    uint64_t vh;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        vh = get_rv64_varhash(rv.src, i);
        if (vh != test_data[i].vh)
        {
            fprintf(stderr, "%s (%d) Expected %"PRIx64", got %"PRIx64"\n", __func__, i, test_data[i].vh, vh);
            ++errors;
        }
    }
    return errors;
}

int test_find_rv64_varhash_by_rsid(mmfile_t rv)
{
    int errors = 0;
    int i;
    uint64_t vh;
    uint64_t first;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        first = 0;
        vh = find_rv64_varhash_by_rsid(rv.src, &first, 9, test_data[i].rsid);
        if (first != (uint64_t)i)
        {
            fprintf(stderr, "%s (%d) Expected first %d, got %"PRIu64"\n", __func__, i, i, first);
            ++errors;
        }
        if (vh != test_data[i].vh)
        {
            fprintf(stderr, "%s (%d) Expected varianthash %"PRIx64", got %"PRIx64"\n", __func__, i, test_data[i].vh, vh);
            ++errors;
        }
    }
    return errors;
}

int test_find_rv64_varhash_by_rsid_notfound(mmfile_t rv)
{
    int errors = 0;
    uint64_t vh;
    uint64_t first = 0;
    vh = find_rv64_varhash_by_rsid(rv.src, &first, 9, 0xfffffff0);
    if (first != 10)
    {
        fprintf(stderr, "%s : Expected first 10, got %"PRIu64"\n", __func__, first);
        ++errors;
    }
    if (vh != 0)
    {
        fprintf(stderr, "%s : Expected varianthash 0, got %"PRIu64"\n", __func__, vh);
        ++errors;
    }
    return errors;
}

int test_find_vr64_chrom_range(mmfile_t vr)
{
    int errors = 0;
    uint32_t rsid;
    uint64_t first = 0;
    uint64_t last = 9;
    rsid = find_vr64_chrom_range(vr.src, &first, &last, test_data[3].chrom);
    if (rsid != test_data[3].rsid)
    {
        fprintf(stderr, "%s : Expected rsid %"PRIx32", got %"PRIx32"\n", __func__, test_data[3].rsid, rsid);
        ++errors;
    }
    if (first != 3)
    {
        fprintf(stderr, "%s : Expected first 3, got %"PRIu64"\n", __func__, first);
        ++errors;
    }
    if (last != 5)
    {
        fprintf(stderr, "%s : Expected last 5, got %"PRIu64"\n", __func__, last);
        ++errors;
    }
    return errors;
}

int test_find_vr64_chrom_range_notfound(mmfile_t vr)
{
    int errors = 0;
    uint32_t rsid;
    uint64_t first = 0;
    uint64_t last = 8;
    rsid = find_vr64_chrom_range(vr.src, &first, &last, 0xfe);
    if (rsid != 0)
    {
        fprintf(stderr, "%s : Expected rsid 0, got %"PRIx32"\n",  __func__, rsid);
        ++errors;
    }
    if (first != 9)
    {
        fprintf(stderr, "%s : Expected first 9, got %"PRIu64"\n",  __func__, first);
        ++errors;
    }
    if (last != 8)
    {
        fprintf(stderr, "%s : Expected last 8, got %"PRIu64"\n",  __func__, last);
        ++errors;
    }
    return errors;
}

int test_find_vr64_pos_range(mmfile_t vr)
{
    int errors = 0;
    uint32_t rsid;
    uint64_t first = 6;
    uint64_t last = 9;
    rsid = find_vr64_pos_range(vr.src, &first, &last, test_data[7].pos, 0xfffffff0);
    if (rsid != test_data[7].rsid)
    {
        fprintf(stderr, "%s : Expected rsid %"PRIx32", got %"PRIx32"\n", __func__, test_data[7].rsid, rsid);
        ++errors;
    }
    if (first != 7)
    {
        fprintf(stderr, "%s : Expected first 7, got %"PRIu64"\n", __func__, first);
        ++errors;
    }
    if (last != 9)
    {
        fprintf(stderr, "%s : Expected last 9, got %"PRIu64"\n", __func__, last);
        ++errors;
    }
    return errors;
}

int test_find_vr64_pos_range_notfound(mmfile_t vr)
{
    int errors = 0;
    uint32_t rsid;
    uint64_t first = 0;
    uint64_t last = 9;
    rsid = find_vr64_pos_range(vr.src, &first, &last, 0xffffff00, 0xfffffff0);
    if (rsid != 0)
    {
        fprintf(stderr, "%s : Expected rsid 0, got %"PRIx32"\n",  __func__, rsid);
        ++errors;
    }
    if (first != 10)
    {
        fprintf(stderr, "%s : Expected first 10, got %"PRIu64"\n",  __func__, first);
        ++errors;
    }
    if (last != 9)
    {
        fprintf(stderr, "%s : Expected last 9, got %"PRIu64"\n",  __func__, last);
        ++errors;
    }
    return errors;
}

int test_find_vr64_chrompos_range(mmfile_t vr)
{
    int errors = 0;
    uint32_t rsid;
    uint64_t first = 0;
    uint64_t last = 9;
    rsid = find_vr64_chrompos_range(vr.src, &first, &last, test_data[6].chrom, test_data[7].pos, test_data[8].pos);
    if (rsid != test_data[7].rsid)
    {
        fprintf(stderr, "%s : Expected rsid %"PRIx32", got %"PRIx32"\n", __func__, test_data[7].rsid, rsid);
        ++errors;
    }
    if (first != 7)
    {
        fprintf(stderr, "%s : Expected first 7, got %"PRIu64"\n", __func__, first);
        ++errors;
    }
    if (last != 8)
    {
        fprintf(stderr, "%s : Expected last 8, got %"PRIu64"\n", __func__, last);
        ++errors;
    }
    return errors;
}

int test_find_vr64_chrompos_range_notfound(mmfile_t vr)
{
    int errors = 0;
    uint32_t rsid;
    uint64_t first = 0;
    uint64_t last = 8;
    rsid = find_vr64_chrompos_range(vr.src, &first, &last, 0xff, 0xffffff00, 0xfffffff0);
    if (rsid != 0)
    {
        fprintf(stderr, "%s : Expected rsid 0, got %"PRIx32"\n",  __func__, rsid);
        ++errors;
    }
    if (first != 9)
    {
        fprintf(stderr, "%s : Expected first 9, got %"PRIu64"\n",  __func__, first);
        ++errors;
    }
    if (last != 8)
    {
        fprintf(stderr, "%s : Expected last 8, got %"PRIu64"\n",  __func__, last);
        ++errors;
    }
    first = 0;
    last = 8;
    rsid = find_vr64_chrom_range(vr.src, &first, &last, 0);
    if (rsid != 1)
    {
        fprintf(stderr, "%s : Expected rsid 0, got %"PRIx32"\n",  __func__, rsid);
        ++errors;
    }
    if (first != 0)
    {
        fprintf(stderr, "%s : Expected first 0, got %"PRIu64"\n",  __func__, first);
        ++errors;
    }
    if (last != 0)
    {
        fprintf(stderr, "%s : Expected last 0, got %"PRIu64"\n",  __func__, last);
        ++errors;
    }
    return errors;
}

int test_find_vr64_rsid_by_varhash(mmfile_t vr)
{
    int errors = 0;
    int i;
    uint32_t rsid;
    uint64_t first;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        first = 0;
        rsid = find_vr64_rsid_by_varhash(vr.src, &first, 9, test_data[i].vh);
        if (rsid != test_data[i].rsid)
        {
            fprintf(stderr, "%s (%d) Expected rsid %"PRIx32", got %"PRIx32"\n",  __func__, i, test_data[i].rsid, rsid);
            ++errors;
        }
        if (first != (uint64_t)i)
        {
            fprintf(stderr, "%s (%d) Expected first %d, got %"PRIu64"\n",  __func__, i, i, first);
            ++errors;
        }
    }
    return errors;
}

int test_find_vr64_rsid_by_varhash_notfound(mmfile_t vr)
{
    int errors = 0;
    uint32_t rsid;
    uint64_t first;
    first = 0;
    rsid = find_vr64_rsid_by_varhash(vr.src, &first, 9, 0xfffffffffffffff0);
    if (rsid != 0)
    {
        fprintf(stderr, "%s : Expected rsid 0, got %"PRIx32"\n",  __func__, rsid);
        ++errors;
    }
    if (first != 10)
    {
        fprintf(stderr, "%s :  Expected first 10, got %"PRIu64"\n",  __func__, first);
        ++errors;
    }
    return errors;
}

void benchmark_get_vr64_rsid(mmfile_t vr)
{
    uint64_t tstart, tend;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        get_vr64_rsid(vr.src, 3);
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/(size*4));
}

void benchmark_get_rv64_varhash(mmfile_t rv)
{
    uint64_t tstart, tend;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        get_rv64_varhash(rv.src, 3);
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/(size*4));
}

void benchmark_find_rv64_varhash_by_rsid(mmfile_t rv)
{
    uint64_t tstart, tend;
    uint64_t first = 0;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        first = 0;
        find_rv64_varhash_by_rsid(rv.src, &first, 9, 0x2F81F575);
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/(size*4));
}

void benchmark_find_vr64_chrompos_range(mmfile_t vr)
{
    uint64_t tstart, tend;
    uint64_t first = 0, last = 9;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        first = 0;
        last = 9;
        find_vr64_chrompos_range(vr.src, &first, &last, 0x19, 0x001AF8FD, 0x001C8F2A);
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/(size*4));
}

void benchmark_find_vr64_rsid_by_varhash(mmfile_t vr)
{
    uint64_t tstart, tend;
    uint64_t first = 0;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        first = 0;
        find_vr64_rsid_by_varhash(vr.src, &first, 9, 0x160017CCA313d0e0);
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/(size*4));
}

int main()
{
    int errors = 0;
    int err;

    uint64_t nitems; // number of binary blocks in the file

    mmfile_t rv = mmap_binfile("rsid_varhash64.10.bin");
    nitems = (uint64_t)(rv.size / RSIDVAR64_BIN_BLKLEN);
    if (nitems != TEST_DATA_SIZE)
    {
        fprintf(stderr, "Expecting %d items, got instead: %"PRIu64"\n", TEST_DATA_SIZE, nitems);
        return 1;
    }

    mmfile_t vr = mmap_binfile("varhash64_rsid.10.bin");
    nitems = (uint64_t)(vr.size / RSIDVAR64_BIN_BLKLEN);
    if (nitems != TEST_DATA_SIZE)
    {
        fprintf(stderr, "Expecting %d items, got instead: %"PRIu64"\n", TEST_DATA_SIZE, nitems);
        return 1;
    }

    errors += test_get_vr64_rsid(vr);
    errors += test_get_rv64_varhash(rv);
    errors += test_find_rv64_varhash_by_rsid(rv);
    errors += test_find_rv64_varhash_by_rsid_notfound(rv);
    errors += test_find_vr64_chrom_range(vr);
    errors += test_find_vr64_chrom_range_notfound(vr);
    errors += test_find_vr64_pos_range(vr);
    errors += test_find_vr64_pos_range_notfound(vr);
    errors += test_find_vr64_chrompos_range(vr);
    errors += test_find_vr64_chrompos_range_notfound(vr);
    errors += test_find_vr64_rsid_by_varhash(vr);
    errors += test_find_vr64_rsid_by_varhash_notfound(vr);

    benchmark_get_vr64_rsid(vr);
    benchmark_get_rv64_varhash(rv);
    benchmark_find_rv64_varhash_by_rsid(rv);
    benchmark_find_vr64_chrompos_range(vr);
    benchmark_find_vr64_rsid_by_varhash(vr);

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
