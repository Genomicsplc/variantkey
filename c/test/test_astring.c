// VariantKey
//
// test_astring.c
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

// Test for astring

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
#include "../src/astring.c"

// returns current time in nanoseconds
uint64_t get_time()
{
    struct timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return (((uint64_t)t.tv_sec * 1000000000) + (uint64_t)t.tv_nsec);
}

int test_aztoupper()
{
    int errors = 0;
    int i, c;
    for (i=97 ; i <= 122; i++)
    {
        c = aztoupper(i);
        if (c != (i - 32))
        {
            fprintf(stderr, "%s : Wrong uppercase value for %d - expecting %d, got %d\n", __func__, i, (i - 32), c);
            ++errors;
        }
    }
    c = aztoupper(96);
    if (c != 96)
    {
        fprintf(stderr, "%s : Wrong uppercase value - expecting 96, got %d\n", __func__, c);
        ++errors;
    }
    return errors;
}

int test_prepend_char()
{
    int errors = 0;
    char original[5] =   "BCD";
    char expected[5] = "ABCD";
    size_t size = 3;
    prepend_char('A', original, &size);
    if (size != 4)
    {
        fprintf(stderr, "%s : Expected size 4, got %lu\n", __func__, size);
        ++errors;
    }
    if (strcmp(original, expected) != 0)
    {
        fprintf(stderr, "%s : Expected %s, got %s\n", __func__, expected, original);
        ++errors;
    }
    return errors;
}

int test_hex_uint64_t()
{
    int errors = 0;
    char s[17] = "";
    hex_uint64_t(0x1234567890abcdef, s);
    if (strcmp(s, "1234567890abcdef"))
    {
        fprintf(stderr, "%s : Unexpected variantkey: expected %s, got %s\n", __func__, "1234567890abcdef", s);
        ++errors;
    }
    return errors;
}

int test_parse_hex_uint64_t()
{
    int errors = 0;
    uint64_t k;
    k = parse_hex_uint64_t("1234567890AbCdEf");
    if (k != 0x1234567890abcdef)
    {
        fprintf(stderr, "%s : Unexpected value: expected 0x1234567890abcdef, got 0x%016" PRIx64 "\n", __func__, k);
        ++errors;
    }
    return errors;
}

static const int k_esid_data_size = 11;

typedef struct esid_data_t
{
    const char str[14];
    size_t size;
    uint64_t esid;
    const char estr[11];
    size_t esize;
} esid_data_t;

static esid_data_t esid_data[] =
{
    {"ABC0123456789", 13, 0xa411493515597619, "0123456789",  10},
    {"\t !\"#$%&'()", 11, 0xafc1083105187209, "_!\"#$%&'()", 10},
    {"*+,-./0123",    10, 0xa28b30d38f411493, "*+,-./0123",  10},
    {"456789:;<=",    10, 0xa51559761969b71d, "456789:;<=",  10},
    {">?@ABCDEFG",    10, 0xa79f8218a39259a7, ">?@ABCDEFG",  10},
    {"HIJKLMNOPQ",    10, 0xaa29aabb2dbafc31, "HIJKLMNOPQ",  10},
    {"RSTUVWXYZ[",    10, 0xacb3d35db7e39ebb, "RSTUVWXYZ[",  10},
    {"\\]^_`abcde",   10, 0xaf3dfbf0218a3925, "\\]^_ ABCDE", 10},
    {"fghijklmno",    10, 0xa9a7a29aabb2dbaf, "FGHIJKLMNO",  10},
    {"pqrstuvwxy",    10, 0xac31cb3d35db7e39, "PQRSTUVWXY",  10},
    {"z{|}~",          5, 0x5ebbf3df80000000, "Z[\\]^",       5},
};

int test_encode_string_id()
{
    int i;
    int errors = 0;
    uint64_t esid;
    for (i=0 ; i < k_esid_data_size; i++)
    {
        esid = encode_string_id(esid_data[i].str, esid_data[i].size);
        if (esid != esid_data[i].esid)
        {
            fprintf(stderr, "%s (%d): Expected 0x%016" PRIx64 ", got 0x%016" PRIx64 "\n", __func__, i, esid_data[i].esid, esid);
            ++errors;
        }
    }
    return errors;
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

int main()
{
    int errors = 0;

    errors += test_aztoupper();
    errors += test_prepend_char();
    errors += test_hex_uint64_t();
    errors += test_parse_hex_uint64_t();
    errors += test_encode_string_id();
    errors += test_decode_string_id();

    return errors;
}
