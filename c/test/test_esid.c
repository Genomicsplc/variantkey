// VariantKey
//
// test_esid.c
//
// @category   Tools
// @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
// @copyright  2017-2018 GENOMICS plc
// @license    MIT (see LICENSE)
// @link       https://github.com/genomicsplc/variantkey
//
// LICENSE
//
// Copyright (c) 2017-2018 GENOMICS plc
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

// Test for esid

#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include "../src/esid.c"

// returns current time in nanoseconds
uint64_t get_time()
{
    struct timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return (((uint64_t)t.tv_sec * 1000000000) + (uint64_t)t.tv_nsec);
}

typedef struct esid_data_t
{
    const char str[37];
    size_t size;
    size_t start;
    uint64_t esid;
    const char estr[11];
    size_t esize;
    uint64_t hsid;
} esid_data_t;

static const int k_esid_data_size = 36;

static esid_data_t esid_data[] =
{
    {"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", 36,  0, 0x0411493515597619, "0123456789",  10, 0xb3a5fdb8808cb7dc},
    {"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", 36,  1, 0x04524d45565d8661, "123456789A",  10, 0xb3a5fdb8808cb7dc},
    {"0223456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", 36, 10, 0x08628e49669e8a6a, "ABCDEFGHIJ",  10, 0xd93e382010f46f32},
    {"0133456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", 36, 25, 0x0c31cb3d35db7e39, "PQRSTUVWXY",  10, 0xbfc379f4a71cb3be},
    {"1123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", 36, 26, 0x0c72cf4d76df8e7a, "QRSTUVWXYZ",  10, 0xdba299d06b54215d},
    {"0123456789ABCDEFGHIJKLMNOPQRSTUVWXY[", 36, 35, 0x0ec0000000000000, "[",            1, 0x996dcdc8ef7c89c1},
    {"012345",                                6,  0, 0x0411493515000000, "012345",       6, 0xa81279395d9c891e},
    {"012345",                                6,  1, 0x04524d4540000000, "12345",        5, 0xa81279395d9c891e},
    {"012345",                                6,  3, 0x04d4540000000000, "345",          3, 0xa81279395d9c891e},
    {"012345",                                6,  5, 0x0540000000000000, "5",            1, 0xa81279395d9c891e},
    {" !\"#$%&'()",                          10,  0, 0x0fc1083105187209, "_!\"#$%&'()", 10, 0x838c148028a06998},
    {"*+,-./0123",                           10,  0, 0x028b30d38f411493, "*+,-./0123",  10, 0x96e42cf7aefeccf0},
    {"456789:;<=",                           10,  0, 0x051559761969b71d, "456789:;<=",  10, 0xb391c7f288e446ee},
    {">?@ABCDEFG",                           10,  0, 0x079f8218a39259a7, ">?@ABCDEFG",  10, 0xcc0de20381405e0a},
    {"HIJKLMNOPQ",                           10,  0, 0x0a29aabb2dbafc31, "HIJKLMNOPQ",  10, 0xbbfc630eacb6c552},
    {"RSTUVWXYZ[",                           10,  0, 0x0cb3d35db7e39ebb, "RSTUVWXYZ[",  10, 0x803eda0a1781a117},
    {"\\]^_`abcde",                          10,  0, 0x0f3dfbf8218a3925, "\\]^_@ABCDE", 10, 0x878ef2947ee3cc2b},
    {"fghijklmno",                           10,  0, 0x09a7a29aabb2dbaf, "FGHIJKLMNO",  10, 0xbaadaef07826969b},
    {"pqrstuvwxy",                           10,  0, 0x0c31cb3d35db7e39, "PQRSTUVWXY",  10, 0xd3da4d5e28be3590},
    {"z{|}~\t",                               6,  0, 0x0ebbf3dfbf000000, "Z[\\]^",       6, 0xeadc752a50c5b850},
    {"123456781234567",                      15,  0, 0x04524d45565d8452, "1234567812",  10, 0x9af6ee553ba41827},
    {"12345678123456",                       14,  0, 0x04524d45565d8452, "1234567812",  10, 0xc94e10e9fe153fd2},
    {"1234567812345",                        13,  0, 0x04524d45565d8452, "1234567812",  10, 0x9b56411c7abdefe6},
    {"123456781234",                         12,  0, 0x04524d45565d8452, "1234567812",  10, 0xcbec35944ff1c863},
    {"12345678123",                          11,  0, 0x04524d45565d8452, "1234567812",  10, 0x929920298cf56b9b},
    {"1234567812",                           10,  0, 0x04524d45565d8452, "1234567812",  10, 0xbdf006d50a33ad90},
    {"123456781",                             9,  0, 0x04524d45565d8440, "123456781",   10, 0xde094182e93557ae},
    {"12345678",                              8,  0, 0x04524d45565d8000, "12345678",    10, 0xccbc926a73ece95c},
    {"1234567",                               7,  0, 0x04524d45565c0000, "1234567",     10, 0xd18b960e2f99c279},
    {"123456",                                6,  0, 0x04524d4556000000, "123456",      10, 0x811c9c02fcc22096},
    {"12345",                                 5,  0, 0x04524d4540000000, "12345",       10, 0xd863467dedd1cab1},
    {"1234",                                  4,  0, 0x04524d4000000000, "1234",        10, 0xbef404ecb71e4cd9},
    {"123",                                   3,  0, 0x04524c0000000000, "123",         10, 0xfad470644116fa54},
    {"12",                                    2,  0, 0x0452000000000000, "12",          10, 0xd94ae1e7173e781d},
    {"1",                                     1,  0, 0x0440000000000000, "1",           10, 0xfc73100baa96ad81},
    {"",                                      0,  0, 0x0000000000000000, "",            10, 0x8000000000000000},
};

int test_encode_string_id()
{
    int i;
    int errors = 0;
    uint64_t esid;
    for (i=0 ; i < k_esid_data_size; i++)
    {
        esid = encode_string_id(esid_data[i].str, esid_data[i].size, esid_data[i].start);
        if (esid != esid_data[i].esid)
        {
            fprintf(stderr, "%s (%d): Expected 0x%016" PRIx64 ", got 0x%016" PRIx64 "\n", __func__, i, esid_data[i].esid, esid);
            ++errors;
        }
    }
    return errors;
}

void benchmark_encode_string_id()
{
    uint64_t esid;
    uint64_t tstart, tend;
    int i;
    int size = 1000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        esid = encode_string_id("ABC0123456", 10, 0);
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op (%" PRIx64 ")\n", __func__, (tend - tstart)/size, esid);
}

int test_decode_string_id()
{
    int i;
    int errors = 0;
    char esid[11];
    size_t size;
    for (i=0 ; i < 11; i++)
    {
        size = decode_string_id(esid_data[i].esid, esid);
        if (strcmp(esid, esid_data[i].estr) != 0)
        {
            fprintf(stderr, "%s (%d): Expected %s, got %s\n", __func__, i, esid_data[i].estr, esid);
            ++errors;
        }
        if (size != esid_data[i].esize)
        {
            fprintf(stderr, "%s (%d): Expected size %lu, got %lu\n", __func__, i, esid_data[i].esize, size);
            ++errors;
        }
    }
    return errors;
}

void benchmark_decode_string_id()
{
    char esid[11] = "";
    size_t len;
    uint64_t tstart, tend;
    int i;
    int size = 1000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        len = decode_string_id(0x08628e49669e8a6a, esid);
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op (%lu)\n", __func__, (tend - tstart)/size, len);
}

int test_decode_string_id_hash()
{
    int i;
    int errors = 0;
    char esid[11];
    size_t size;
    for (i=0 ; i < 11; i++)
    {
        size = decode_string_id(esid_data[i].hsid, esid);
        if (size != 0)
        {
            fprintf(stderr, "%s (%d): Expected size 0, got %lu\n", __func__, i, size);
            ++errors;
        }
    }
    return errors;
}

int test_hash_string_id()
{
    int i;
    int errors = 0;
    uint64_t hsid;
    for (i=0 ; i < k_esid_data_size; i++)
    {
        hsid = hash_string_id(esid_data[i].str, esid_data[i].size);
        if (hsid != esid_data[i].hsid)
        {
            fprintf(stderr, "%s (%d): Expected 0x%016" PRIx64 ", got 0x%016" PRIx64 "\n", __func__, i, esid_data[i].hsid, hsid);
            ++errors;
        }
    }
    return errors;
}

void benchmark_hash_string_id()
{
    uint64_t hsid;
    uint64_t tstart, tend;
    int i;
    int size = 1000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        hsid = hash_string_id("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", 36);
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op (%" PRIx64 ")\n", __func__, (tend - tstart)/size, hsid);
}

int main()
{
    int errors = 0;

    errors += test_encode_string_id();
    errors += test_decode_string_id();
    errors += test_decode_string_id_hash();
    errors += test_hash_string_id();

    benchmark_encode_string_id();
    benchmark_decode_string_id();
    benchmark_hash_string_id();

    return errors;
}
