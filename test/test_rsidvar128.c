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
#include "rsidvar128.h"

#define TEST_DATA_SIZE 10

typedef struct test_data_128_t
{
    uint32_t rsid;
    uint32_t assembly;
    uint32_t chrom;
    uint32_t pos;
    uint32_t refalt;
} test_data_128_t;

static test_data_128_t test_data[TEST_DATA_SIZE] =
{
    {0X00000001, 0X8B29D2C7, 0X00000001, 0X000FDDC2, 0X014A1C00},
    {0X00000007, 0X8B29D2C7, 0X00000009, 0X001173EF, 0X00708000},
    {0X0000000B, 0X8B29D2C7, 0X00000009, 0X00130A1C, 0X00708000},
    {0X00000061, 0X8B29D2C7, 0X00000016, 0X0014A049, 0X0413D021},
    {0X00000065, 0X8B29D2C7, 0X00000016, 0X00163676, 0X02138400},
    {0X000003E5, 0X8B29D2C7, 0X00000016, 0X0017CCA3, 0X027A1C00},
    {0X000003F1, 0X8B29D2C7, 0X00000019, 0X001962D0, 0X027A1C00},
    {0X000026F5, 0X8B29D2C7, 0X00000019, 0X001AF8FD, 0X01408000},
    {0X000186A3, 0X8B29D2C7, 0X00000019, 0X001C8F2A, 0X00708000},
    {0X00019919, 0X8B29D2C7, 0X00000019, 0X001E2557, 0X003A0000},
};

// returns current time in nanoseconds
uint64_t get_time()
{
    struct timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return (((uint64_t)t.tv_sec * 1000000000) + (uint64_t)t.tv_nsec);
}

int test_get_vr128_rsid(mmfile_t vr)
{
    int errors = 0;
    int i;
    uint32_t rsid;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        rsid = get_vr128_rsid(vr.src, i);
        if (rsid != test_data[i].rsid)
        {
            fprintf(stderr, "%s (%d) Expected %"PRIx32", got %"PRIx32"\n", __func__, i, test_data[i].rsid, rsid);
            ++errors;
        }
    }
    return errors;
}

int test_get_rv128_variantkey(mmfile_t rv)
{
    int errors = 0;
    int i;
    variantkey128_t vh;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        vh = get_rv128_variantkey(rv.src, i);
        if (vh.assembly != test_data[i].assembly)
        {
            fprintf(stderr, "%s (%d) Expected assembly %"PRIx32", got %"PRIx32"\n", __func__, i, test_data[i].assembly, vh.assembly);
            ++errors;
        }
        if (vh.chrom != test_data[i].chrom)
        {
            fprintf(stderr, "%s (%d) Expected chrom %"PRIx32", got %"PRIx32"\n", __func__, i, test_data[i].chrom, vh.chrom);
            ++errors;
        }
        if (vh.pos != test_data[i].pos)
        {
            fprintf(stderr, "%s (%d) Expected pos %"PRIx32", got %"PRIx32"\n", __func__, i, test_data[i].pos, vh.pos);
            ++errors;
        }
        if (vh.refalt != test_data[i].refalt)
        {
            fprintf(stderr, "%s (%d) Expected refalt %"PRIx32", got %"PRIx32"\n", __func__, i, test_data[i].refalt, vh.refalt);
            ++errors;
        }
    }
    return errors;
}

int test_find_rv128_variantkey_by_rsid(mmfile_t rv)
{
    int errors = 0;
    int i;
    variantkey128_t vh;
    uint64_t first;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        first = 0;
        vh = find_rv128_variantkey_by_rsid(rv.src, &first, 9, test_data[i].rsid);
        if (first != (uint64_t)i)
        {
            fprintf(stderr, "%s (%d) Expected first %d, got %"PRIu64"\n", __func__, i, i, first);
            ++errors;
        }
        if (vh.assembly != test_data[i].assembly)
        {
            fprintf(stderr, "%s (%d) Expected assembly %"PRIx32", got %"PRIx32"\n", __func__, i, test_data[i].assembly, vh.assembly);
            ++errors;
        }
        if (vh.chrom != test_data[i].chrom)
        {
            fprintf(stderr, "%s (%d) Expected chrom %"PRIx32", got %"PRIx32"\n", __func__, i, test_data[i].chrom, vh.chrom);
            ++errors;
        }
        if (vh.pos != test_data[i].pos)
        {
            fprintf(stderr, "%s (%d) Expected pos %"PRIx32", got %"PRIx32"\n", __func__, i, test_data[i].pos, vh.pos);
            ++errors;
        }
        if (vh.refalt != test_data[i].refalt)
        {
            fprintf(stderr, "%s (%d) Expected refalt %"PRIx32", got %"PRIx32"\n", __func__, i, test_data[i].refalt, vh.refalt);
            ++errors;
        }
    }
    return errors;
}

int test_find_rv128_variantkey_by_rsid_notfound(mmfile_t rv)
{
    int errors = 0;
    variantkey128_t vh;
    uint64_t first = 0;
    vh = find_rv128_variantkey_by_rsid(rv.src, &first, 9, 0xfffffff0);
    if (first != 10)
    {
        fprintf(stderr, "%s : Expected first 10, got %"PRIu64"\n", __func__, first);
        ++errors;
    }
    if (vh.assembly != 0)
    {
        fprintf(stderr, "%s : Expected assembly 0, got %"PRIu32"\n", __func__, vh.assembly);
        ++errors;
    }
    if (vh.chrom != 0)
    {
        fprintf(stderr, "%s : Expected chrom 0, got %"PRIu32"\n", __func__, vh.chrom);
        ++errors;
    }
    if (vh.pos != 0)
    {
        fprintf(stderr, "%s : Expected pos 0, got %"PRIu32"\n", __func__, vh.pos);
        ++errors;
    }
    if (vh.refalt != 0)
    {
        fprintf(stderr, "%s : Expected refalt 0, got %"PRIu32"\n", __func__, vh.refalt);
        ++errors;
    }
    return errors;
}

int test_find_vr128_chrom_range(mmfile_t vr)
{
    int errors = 0;
    uint32_t rsid;
    uint64_t first = 0;
    uint64_t last = 9;
    rsid = find_vr128_chrom_range(vr.src, &first, &last, test_data[3].chrom);
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

int test_find_vr128_chrom_range_notfound(mmfile_t vr)
{
    int errors = 0;
    uint32_t rsid;
    uint64_t first = 0;
    uint64_t last = 8;
    rsid = find_vr128_chrom_range(vr.src, &first, &last, 0xfffffffe);
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
    rsid = find_vr128_chrom_range(vr.src, &first, &last, 0);
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

int test_find_vr128_pos_range(mmfile_t vr)
{
    int errors = 0;
    uint32_t rsid;
    uint64_t first = 6;
    uint64_t last = 9;
    rsid = find_vr128_pos_range(vr.src, &first, &last, test_data[7].pos, 0xfffffff0);
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

int test_find_vr128_pos_range_notfound(mmfile_t vr)
{
    int errors = 0;
    uint32_t rsid;
    uint64_t first = 0;
    uint64_t last = 9;
    rsid = find_vr128_pos_range(vr.src, &first, &last, 0xffffff00, 0xfffffff0);
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

int test_find_vr128_chrompos_range(mmfile_t vr)
{
    int errors = 0;
    uint32_t rsid;
    uint64_t first = 0;
    uint64_t last = 9;
    rsid = find_vr128_chrompos_range(vr.src, &first, &last, test_data[6].chrom, test_data[7].pos, test_data[8].pos);
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

int test_find_vr128_chrompos_range_notfound(mmfile_t vr)
{
    int errors = 0;
    uint32_t rsid;
    uint64_t first = 0;
    uint64_t last = 9;
    rsid = find_vr128_chrompos_range(vr.src, &first, &last, 0xfffffff0, 0xffffff00, 0xfffffff0);
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

int test_find_vr128_rsid_by_variantkey(mmfile_t vr)
{
    int errors = 0;
    int i;
    uint32_t rsid;
    uint64_t first;
    variantkey128_t vh;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        first = 0;
        vh.assembly = test_data[i].assembly;
        vh.chrom = test_data[i].chrom;
        vh.pos = test_data[i].pos;
        vh.refalt = test_data[i].refalt;
        rsid = find_vr128_rsid_by_variantkey(vr.src, &first, 9, vh);
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

int test_find_vr128_rsid_by_variantkey_notfound(mmfile_t vr)
{
    int errors = 0;
    uint32_t rsid;
    uint64_t first;
    variantkey128_t vh;
    first = 0;
    vh.assembly = 0xfffffff0;
    vh.chrom = 0xfffffff0;
    vh.pos = 0xfffffff0;
    vh.refalt = 0xfffffff0;
    rsid = find_vr128_rsid_by_variantkey(vr.src, &first, 9, vh);
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

void benchmark_get_vr128_rsid(mmfile_t vr)
{
    uint64_t tstart, tend;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        get_vr128_rsid(vr.src, 3);
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/(size*4));
}

void benchmark_get_rv128_variantkey(mmfile_t rv)
{
    uint64_t tstart, tend;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        get_rv128_variantkey(rv.src, 3);
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/(size*4));
}

void benchmark_find_rv128_variantkey_by_rsid(mmfile_t rv)
{
    uint64_t tstart, tend;
    uint64_t first = 0;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        first = 0;
        find_rv128_variantkey_by_rsid(rv.src, &first, 9, 0x2F81F575);
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/(size*4));
}

void benchmark_find_vr128_chrompos_range(mmfile_t vr)
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
        find_vr128_chrompos_range(vr.src, &first, &last, 0X00000019, 0X001AF8FD, 0X001C8F2A);
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/(size*4));
}

void benchmark_find_vr128_rsid_by_variantkey(mmfile_t vr)
{
    uint64_t tstart, tend;
    uint64_t first = 0;
    variantkey128_t vh;
    vh.assembly =  0X8B29D2C7;
    vh.chrom = 0X00000019;
    vh.pos = 0X001AF8FD;
    vh.refalt = 0X01408000;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        first = 0;
        find_vr128_rsid_by_variantkey(vr.src, &first, 9, vh);
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/(size*4));
}

int main()
{
    int errors = 0;
    int err;

    uint64_t nitems; // number of binary blocks in the file

    mmfile_t rv = mmap_binfile("rsid_variantkey128.10.bin");
    nitems = (uint64_t)(rv.size / RSIDVAR128_BIN_BLKLEN);
    if (nitems != TEST_DATA_SIZE)
    {
        fprintf(stderr, "Expecting %d items, got instead: %"PRIu64"\n", TEST_DATA_SIZE, nitems);
        return 1;
    }

    mmfile_t vr = mmap_binfile("variantkey128_rsid.10.bin");
    nitems = (uint64_t)(vr.size / RSIDVAR128_BIN_BLKLEN);
    if (nitems != TEST_DATA_SIZE)
    {
        fprintf(stderr, "Expecting %d items, got instead: %"PRIu64"\n", TEST_DATA_SIZE, nitems);
        return 1;
    }

    errors += test_get_vr128_rsid(vr);
    errors += test_get_rv128_variantkey(rv);
    errors += test_find_rv128_variantkey_by_rsid(rv);
    errors += test_find_rv128_variantkey_by_rsid_notfound(rv);
    errors += test_find_vr128_chrom_range(vr);
    errors += test_find_vr128_chrom_range_notfound(vr);
    errors += test_find_vr128_pos_range(vr);
    errors += test_find_vr128_pos_range_notfound(vr);
    errors += test_find_vr128_chrompos_range(vr);
    errors += test_find_vr128_chrompos_range_notfound(vr);
    errors += test_find_vr128_rsid_by_variantkey(vr);
    errors += test_find_vr128_rsid_by_variantkey_notfound(vr);

    benchmark_get_vr128_rsid(vr);
    benchmark_get_rv128_variantkey(rv);
    benchmark_find_rv128_variantkey_by_rsid(rv);
    benchmark_find_vr128_chrompos_range(vr);
    benchmark_find_vr128_rsid_by_variantkey(vr);

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
