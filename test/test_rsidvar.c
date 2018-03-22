// Nicola Asuni

#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include "rsidvar.h"

#define TEST_DATA_SIZE 10

typedef struct test_data_t
{
    uint32_t rsid;
    uint64_t vk;
    uint8_t chrom;
    uint32_t pos;
    uint32_t refalt;
} test_data_t;

static test_data_t test_data[TEST_DATA_SIZE] =
{
    {0X00000001, 0x08027a2580338000, 0x01, 0x0004f44b, 0x00338000},
    {0X00000007, 0x4800a1fe439e3918, 0x09, 0x000143fc, 0x439e3918},
    {0X0000000B, 0x4800a1fe7555eb16, 0x09, 0x000143fc, 0x7555eb16},
    {0X00000061, 0x80010274003a0000, 0x10, 0x000204e8, 0x003a0000},
    {0X00000065, 0x8001028d00138000, 0x10, 0x0002051a, 0x00138000},
    {0X000003E5, 0x80010299007a0000, 0x10, 0x00020532, 0x007a0000},
    {0X000003F1, 0xa0012b62003a0000, 0x14, 0x000256c4, 0x003a0000},
    {0X000026F5, 0xa0012b6280708000, 0x14, 0x000256c5, 0x00708000},
    {0X000186A3, 0xa0012b65e3256692, 0x14, 0x000256cb, 0x63256692},
    {0X00019919, 0xa0012b67d5439803, 0x14, 0x000256cf, 0x55439803},

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
        if (rsid != test_data[i].rsid)
        {
            fprintf(stderr, "%s (%d) Expected %"PRIx32", got %"PRIx32"\n", __func__, i, test_data[i].rsid, rsid);
            ++errors;
        }
    }
    return errors;
}

int test_get_rv_variantkey(mmfile_t rv)
{
    int errors = 0;
    int i;
    uint64_t vk;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        vk = get_rv_variantkey(rv.src, i);
        if (vk != test_data[i].vk)
        {
            fprintf(stderr, "%s (%d) Expected %"PRIx64", got %"PRIx64"\n", __func__, i, test_data[i].vk, vk);
            ++errors;
        }
    }
    return errors;
}

int test_find_rv_variantkey_by_rsid(mmfile_t rv)
{
    int errors = 0;
    int i;
    uint64_t vk;
    uint64_t first;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        first = 0;
        vk = find_rv_variantkey_by_rsid(rv.src, &first, 9, test_data[i].rsid);
        if (first != (uint64_t)i)
        {
            fprintf(stderr, "%s (%d) Expected first %d, got %"PRIu64"\n", __func__, i, i, first);
            ++errors;
        }
        if (vk != test_data[i].vk)
        {
            fprintf(stderr, "%s (%d) Expected variantkey %"PRIx64", got %"PRIx64"\n", __func__, i, test_data[i].vk, vk);
            ++errors;
        }
    }
    return errors;
}

int test_find_rv_variantkey_by_rsid_notfound(mmfile_t rv)
{
    int errors = 0;
    uint64_t vk;
    uint64_t first = 0;
    vk = find_rv_variantkey_by_rsid(rv.src, &first, 9, 0xfffffff0);
    if (first != 10)
    {
        fprintf(stderr, "%s : Expected first 10, got %"PRIu64"\n", __func__, first);
        ++errors;
    }
    if (vk != 0)
    {
        fprintf(stderr, "%s : Expected variantkey 0, got %"PRIu64"\n", __func__, vk);
        ++errors;
    }
    return errors;
}

int test_find_vr_rsid_by_variantkey(mmfile_t vr)
{
    int errors = 0;
    int i;
    uint32_t rsid;
    uint64_t first;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        first = 0;
        rsid = find_vr_rsid_by_variantkey(vr.src, &first, 9, test_data[i].vk);
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

int test_find_vr_rsid_by_variantkey_notfound(mmfile_t vr)
{
    int errors = 0;
    uint32_t rsid;
    uint64_t first;
    first = 0;
    rsid = find_vr_rsid_by_variantkey(vr.src, &first, 9, 0xfffffffffffffff0);
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

int test_find_vr_chrompos_range(mmfile_t vr)
{
    int errors = 0;
    uint32_t rsid;
    uint64_t first = 0;
    uint64_t last = 9;
    rsid = find_vr_chrompos_range(vr.src, &first, &last, test_data[6].chrom, test_data[7].pos, test_data[8].pos);
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

int test_find_vr_chrompos_range_notfound(mmfile_t vr)
{
    int errors = 0;
    uint32_t rsid;
    uint64_t first = 0;
    uint64_t last = 8;
    rsid = find_vr_chrompos_range(vr.src, &first, &last, 0xff, 0xffffff00, 0xfffffff0);
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
    rsid = find_vr_chrompos_range(vr.src, &first, &last, 0, 0, 0);
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

/*
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
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/(size*4));
}

void benchmark_get_rv_variantkey(mmfile_t rv)
{
    uint64_t tstart, tend;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        get_rv_variantkey(rv.src, 3);
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/(size*4));
}

void benchmark_find_rv_variantkey_by_rsid(mmfile_t rv)
{
    uint64_t tstart, tend;
    uint64_t first = 0;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        first = 0;
        find_rv_variantkey_by_rsid(rv.src, &first, 9, 0x2F81F575);
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/(size*4));
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
        first = 0;
        last = 9;
        find_vr_chrompos_range(vr.src, &first, &last, 0x19, 0x001AF8FD, 0x001C8F2A);
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/(size*4));
}

void benchmark_find_vr_rsid_by_variantkey(mmfile_t vr)
{
    uint64_t tstart, tend;
    uint64_t first = 0;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        first = 0;
        find_vr_rsid_by_variantkey(vr.src, &first, 9, 0x160017CCA313d0e0);
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/(size*4));
}
*/

int main()
{
    int errors = 0;
    int err;

    uint64_t nitems; // number of binary blocks in the file

    mmfile_t rv = mmap_binfile("rsid_variantkey.10.bin");
    nitems = (uint64_t)(rv.size / BINBLKLEN);
    if (nitems != TEST_DATA_SIZE)
    {
        fprintf(stderr, "Expecting %d items, got instead: %"PRIu64"\n", TEST_DATA_SIZE, nitems);
        return 1;
    }

    mmfile_t vr = mmap_binfile("variantkey_rsid.10.bin");
    nitems = (uint64_t)(vr.size / BINBLKLEN);
    if (nitems != TEST_DATA_SIZE)
    {
        fprintf(stderr, "Expecting %d items, got instead: %"PRIu64"\n", TEST_DATA_SIZE, nitems);
        return 1;
    }

    errors += test_get_vr_rsid(vr);
    errors += test_get_rv_variantkey(rv);
    errors += test_find_rv_variantkey_by_rsid(rv);
    errors += test_find_rv_variantkey_by_rsid_notfound(rv);
    errors += test_find_vr_rsid_by_variantkey(vr);
    errors += test_find_vr_rsid_by_variantkey_notfound(vr);
    errors += test_find_vr_chrompos_range(vr);
    errors += test_find_vr_chrompos_range_notfound(vr);

    //benchmark_get_vr_rsid(vr);
    //benchmark_get_rv_variantkey(rv);
    //benchmark_find_rv_variantkey_by_rsid(rv);
    //benchmark_find_vr_chrompos_range(vr);
    //benchmark_find_vr_rsid_by_variantkey(vr);

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
