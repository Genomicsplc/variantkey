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

int test_map_file_arrow()
{
    int errors = 0;
    char *file = "test_data_arrow.bin"; // file containing test data
    mmfile_t mf = {0};
    mf.ncols = 2;
    mf.ctbytes[0] = 4;
    mf.ctbytes[1] = 8;
    mmap_binfile(file, &mf);
    if (mf.fd < 0)
    {
        fprintf(stderr, "%s can't open %s for reading\n", __func__, file);
        return 1;
    }
    if (mf.size == 0)
    {
        fprintf(stderr, "%s fstat error! [%s]\n", __func__, strerror(errno));
        return 1;
    }
    if (mf.src == MAP_FAILED)
    {
        fprintf(stderr, "%s mmap error! [%s]\n", __func__, strerror(errno));
        return 1;
    }
    if (mf.size != 730)
    {
        fprintf(stderr, "%s mf.size : Expecting 730 bytes, got instead: %" PRIu64 "\n", __func__, mf.size);
        errors++;
    }
    if (mf.doffset != 376)
    {
        fprintf(stderr, "%s mf.doffset : Expecting 376 bytes, got instead: %" PRIu64 "\n", __func__, mf.doffset);
        errors++;
    }
    if (mf.dlength != 136)
    {
        fprintf(stderr, "%s mf.dlength : Expecting 136 bytes, got instead: %" PRIu64 "\n", __func__, mf.dlength);
        errors++;
    }
    if (mf.nrows != 11)
    {
        fprintf(stderr, "%s mf.nrows : Expecting 11 items, got instead: %" PRIu64 "\n", __func__, mf.nrows);
        errors++;
    }
    if (mf.ncols != 2)
    {
        fprintf(stderr, "%s mf.ncols : Expecting 2 items, got instead: %" PRIu8 "\n", __func__, mf.ncols);
        errors++;
    }
    if (mf.index[0] != 376)
    {
        fprintf(stderr, "%s mf.index[0] : Expecting 376 bytes, got instead: %" PRIu64 "\n", __func__, mf.index[0]);
        errors++;
    }
    if (mf.index[1] != 424)
    {
        fprintf(stderr, "%s mf.index[1] : Expecting 424 bytes, got instead: %" PRIu64 "\n", __func__, mf.index[1]);
        errors++;
    }
    int e = munmap_binfile(mf);
    if (e != 0)
    {
        fprintf(stderr, "%s Got %d error while unmapping the file\n", __func__, e);
        errors++;
    }
    return errors;
}

int test_map_file_feather()
{
    int errors = 0;
    char *file = "test_data_feather.bin"; // file containing test data
    mmfile_t mf = {0};
    mf.ncols = 2;
    mf.ctbytes[0] = 4;
    mf.ctbytes[1] = 8;
    mmap_binfile(file, &mf);
    if (mf.fd < 0)
    {
        fprintf(stderr, "%s can't open %s for reading\n", __func__, file);
        return 1;
    }
    if (mf.size == 0)
    {
        fprintf(stderr, "%s fstat error! [%s]\n", __func__, strerror(errno));
        return 1;
    }
    if (mf.src == MAP_FAILED)
    {
        fprintf(stderr, "%s mmap error! [%s]\n", __func__, strerror(errno));
        return 1;
    }
    if (mf.size != 384)
    {
        fprintf(stderr, "%s mf.size : Expecting 384 bytes, got instead: %" PRIu64 "\n", __func__, mf.size);
        errors++;
    }
    if (mf.doffset != 8)
    {
        fprintf(stderr, "%s mf.doffset : Expecting 8 bytes, got instead: %" PRIu64 "\n", __func__, mf.doffset);
        errors++;
    }
    if (mf.dlength != 136)
    {
        fprintf(stderr, "%s mf.dlength : Expecting 136 bytes, got instead: %" PRIu64 "\n", __func__, mf.dlength);
        errors++;
    }
    if (mf.nrows != 11)
    {
        fprintf(stderr, "%s mf.nrows : Expecting 11 items, got instead: %" PRIu64 "\n", __func__, mf.nrows);
        errors++;
    }
    if (mf.ncols != 2)
    {
        fprintf(stderr, "%s mf.ncols : Expecting 2 items, got instead: %" PRIu8 "\n", __func__, mf.ncols);
        errors++;
    }
    if (mf.index[0] != 8)
    {
        fprintf(stderr, "%s mf.index[0] : Expecting 8 bytes, got instead: %" PRIu64 "\n", __func__, mf.index[0]);
        errors++;
    }
    if (mf.index[1] != 56)
    {
        fprintf(stderr, "%s mf.index[1] : Expecting 56 bytes, got instead: %" PRIu64 "\n", __func__, mf.index[1]);
        errors++;
    }
    int e = munmap_binfile(mf);
    if (e != 0)
    {
        fprintf(stderr, "%s Got %d error while unmapping the file\n", __func__, e);
        errors++;
    }
    return errors;
}

int test_map_file_binsrc()
{
    int errors = 0;
    char *file = "test_data_binsrc.bin"; // file containing test data
    mmfile_t mf = {0};
    mmap_binfile(file, &mf);
    if (mf.fd < 0)
    {
        fprintf(stderr, "%s can't open %s for reading\n", __func__, file);
        return 1;
    }
    if (mf.size == 0)
    {
        fprintf(stderr, "%s fstat error! [%s]\n", __func__, strerror(errno));
        return 1;
    }
    if (mf.src == MAP_FAILED)
    {
        fprintf(stderr, "%s mmap error! [%s]\n", __func__, strerror(errno));
        return 1;
    }
    if (mf.size != 176)
    {
        fprintf(stderr, "%s mf.size : Expecting 176 bytes, got instead: %" PRIu64 "\n", __func__, mf.size);
        errors++;
    }
    if (mf.doffset != 40)
    {
        fprintf(stderr, "%s mf.doffset : Expecting 40 bytes, got instead: %" PRIu64 "\n", __func__, mf.doffset);
        errors++;
    }
    if (mf.dlength != 136)
    {
        fprintf(stderr, "%s mf.dlength : Expecting 136 bytes, got instead: %" PRIu64 "\n", __func__, mf.dlength);
        errors++;
    }
    if (mf.nrows != 11)
    {
        fprintf(stderr, "%s mf.nrows : Expecting 11 items, got instead: %" PRIu64 "\n", __func__, mf.nrows);
        errors++;
    }
    if (mf.ncols != 2)
    {
        fprintf(stderr, "%s mf.ncols : Expecting 2 items, got instead: %" PRIu8 "\n", __func__, mf.ncols);
        errors++;
    }
    if (mf.index[0] != 40)
    {
        fprintf(stderr, "%s mf.index[0] : Expecting 40 bytes, got instead: %" PRIu64 "\n", __func__, mf.index[0]);
        errors++;
    }
    if (mf.index[1] != 88)
    {
        fprintf(stderr, "%s mf.index[1] : Expecting 88 bytes, got instead: %" PRIu64 "\n", __func__, mf.index[1]);
        errors++;
    }
    int e = munmap_binfile(mf);
    if (e != 0)
    {
        fprintf(stderr, "%s Got %d error while unmapping the file\n", __func__, e);
        errors++;
    }
    return errors;
}

int test_map_file_col()
{
    int errors = 0;
    char *file = "test_data_col.bin"; // file containing test data
    mmfile_t mf = {0};
    mmap_binfile(file, &mf);
    if (mf.fd < 0)
    {
        fprintf(stderr, "%s can't open %s for reading\n", __func__, file);
        return 1;
    }
    if (mf.size == 0)
    {
        fprintf(stderr, "%s fstat error! [%s]\n", __func__, strerror(errno));
        return 1;
    }
    if (mf.src == MAP_FAILED)
    {
        fprintf(stderr, "%s mmap error! [%s]\n", __func__, strerror(errno));
        return 1;
    }
    if (mf.size != 3776)
    {
        fprintf(stderr, "%s mf.size : Expecting 3776 bytes, got instead: %" PRIu64 "\n", __func__, mf.size);
        errors++;
    }
    if (mf.doffset != 0)
    {
        fprintf(stderr, "%s mf.doffset : Expecting 0 bytes, got instead: %" PRIu64 "\n", __func__, mf.doffset);
        errors++;
    }
    if (mf.dlength != 3776)
    {
        fprintf(stderr, "%s mf.dlength : Expecting 3776 bytes, got instead: %" PRIu64 "\n", __func__, mf.dlength);
        errors++;
    }
    if (mf.nrows != 0)
    {
        fprintf(stderr, "%s mf.nrows : Expecting 0 items, got instead: %" PRIu64 "\n", __func__, mf.nrows);
        errors++;
    }
    if (mf.ncols != 0)
    {
        fprintf(stderr, "%s mf.ncols : Expecting 0 items, got instead: %" PRIu8 "\n", __func__, mf.ncols);
        errors++;
    }
    int e = munmap_binfile(mf);
    if (e != 0)
    {
        fprintf(stderr, "%s Got %d error while unmapping the file\n", __func__, e);
        errors++;
    }
    return errors;
}

int main()
{
    int errors = 0;

    errors += test_mmap_binfile_error("ERROR");
    errors += test_mmap_binfile_error("/dev/null");
    errors += test_munmap_binfile_error();
    errors += test_map_file_arrow();
    errors += test_map_file_feather();
    errors += test_map_file_binsrc();
    errors += test_map_file_col();

    return errors;
}
