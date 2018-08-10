// VariantKey
//
// test_hex.c
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

// Test for hex

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
#include "../src/hex.c"

// returns current time in nanoseconds
uint64_t get_time()
{
    struct timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return (((uint64_t)t.tv_sec * 1000000000) + (uint64_t)t.tv_nsec);
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

void benchmark_hex_uint64_t()
{
    char s[17] = "";
    uint64_t tstart, tend;
    int i;
    int size = 1000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        hex_uint64_t(size, s);
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/size);
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

void benchmark_parse_hex_uint64_t()
{
    uint64_t k;
    uint64_t tstart, tend;
    int i;
    int size = 1000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        k = parse_hex_uint64_t("1234567890AbCdEf");
    }
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op (%" PRIx64 ")\n", __func__, (tend - tstart)/size, k);
}

int main()
{
    int errors = 0;

    errors += test_hex_uint64_t();
    errors += test_parse_hex_uint64_t();

    benchmark_hex_uint64_t();
    benchmark_parse_hex_uint64_t();

    return errors;
}
