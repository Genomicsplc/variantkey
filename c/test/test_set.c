// VariantKey
//
// test_set.c
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

// Test for set

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
#include "../src/variantkey/set.h"

// returns current time in nanoseconds
uint64_t get_time()
{
    struct timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return (((uint64_t)t.tv_sec * 1000000000) + (uint64_t)t.tv_nsec);
}

int test_sort_uint64_t()
{
    int errors = 0;
    uint64_t arr[10] = {8,1,9,3,2,7,4,0,5,6};
    sort_uint64_t(arr, 10);
    uint64_t i;
    for(i = 0; i < 10; i++)
    {
        if (arr[i] != i)
        {
            fprintf(stderr, "%s : Unexpected value: expected %" PRIu64 ", got %" PRIu64 "\n", __func__, i, arr[i]);
            ++errors;
        }
    }
    return errors;
}

void benchmark_sort_uint64_t()
{
    const uint64_t nitems = 100000;
    uint64_t i, arr[nitems];
    for (i=nitems ; i > 0; i--)
    {
        arr[i] = i;
    }
    uint64_t tstart, tend;
    tstart = get_time();
    sort_uint64_t(arr, nitems);
    tend = get_time();
    fprintf(stdout, " * %s : %lu ns/op\n", __func__, (tend - tstart)/nitems);
}

int test_unique_uint64_t()
{
    int errors = 0;
    uint64_t arr[22] = {0,1,2,2,3,3,3,4,4,4,4,5,5,5,5,5,5,6,7,8,9,9};
    uint64_t n = unique_uint64_t(arr, 22);
    if (n != 10)
    {
        fprintf(stderr, "%s : Expected 10, got %" PRIu64 "\n", __func__, n);
        ++errors;
    }
    uint64_t i;
    for(i = 0; i < n; i++)
    {
        if (arr[i] != i)
        {
            fprintf(stderr, "%s : Expected %" PRIu64 ", got %" PRIu64 "\n", __func__, i, arr[i]);
            ++errors;
        }
    }
    return errors;
}

int test_unique_uint64_t_zero()
{
    int errors = 0;
    uint64_t arr[1] = {0};
    uint64_t n = unique_uint64_t(arr, 0);
    if (n != 0)
    {
        fprintf(stderr, "%s : Expected 0, got %" PRIu64 "\n", __func__, n);
        ++errors;
    }
    return errors;
}

int test_intersection_uint64_t()
{
    int errors = 0;
    uint64_t a_arr[11] = {0,1,2,3,3,4,5,6,7,8,9};
    uint64_t b_arr[7] = {0,3,3,5,6,6,9};
    uint64_t o_arr[6] = {0};
    uint64_t n = intersection_uint64_t(a_arr, 11, b_arr, 7, o_arr);
    if (n != 6)
    {
        fprintf(stderr, "%s : Expected 5, got %" PRIu64 "\n", __func__, n);
        ++errors;
    }
    uint64_t i;
    uint64_t e[6] = {0,3,3,5,6,9};
    for(i = 0; i < n; i++)
    {
        if (o_arr[i] != e[i])
        {
            fprintf(stderr, "%s (%" PRIu64 "): Expected %" PRIu64 ", got %" PRIu64 "\n", __func__, i, e[i], o_arr[i]);
            ++errors;
        }
    }
    return errors;
}

int test_union_uint64_t()
{
    int errors = 0;
    uint64_t a_arr[7] = {0,2,3,3,5,8,9};
    uint64_t b_arr[7] = {0,1,4,5,6,6,7};
    uint64_t o_arr[11] = {0};
    uint64_t n = union_uint64_t(a_arr, 6, b_arr, 7, o_arr);
    if (n != 11)
    {
        fprintf(stderr, "%s : Expected 10, got %" PRIu64 "\n", __func__, n);
        ++errors;
    }
    uint64_t i;
    uint64_t e[11] = {0,1,2,3,3,4,5,6,6,7,8};
    for(i = 0; i < n; i++)
    {
        if (o_arr[i] != e[i])
        {
            fprintf(stderr, "%s (%" PRIu64 "): Expected %" PRIu64 ", got %" PRIu64 "\n", __func__, i, e[i], o_arr[i]);
            ++errors;
        }
    }
    return errors;
}

int test_union_uint64_t_ba()
{
    int errors = 0;
    uint64_t a_arr[6] = {0,1,4,5,6,7};
    uint64_t b_arr[7] = {0,2,3,5,8,9,9};
    uint64_t o_arr[20] = {0};
    uint64_t n = union_uint64_t(a_arr, 6, b_arr, 7, o_arr);
    if (n != 11)
    {
        fprintf(stderr, "%s : Expected 5, got %" PRIu64 "\n", __func__, n);
        ++errors;
    }
    uint64_t i;
    uint64_t e[11] = {0,1,2,3,4,5,6,7,8,9,9};
    for(i = 0; i < n; i++)
    {
        if (o_arr[i] != e[i])
        {
            fprintf(stderr, "%s (%" PRIu64 "): Expected %" PRIu64 ", got %" PRIu64 "\n", __func__, i, e[i], o_arr[i]);
            ++errors;
        }
    }
    return errors;
}

int main()
{
    int errors = 0;

    errors += test_sort_uint64_t();
    errors += test_unique_uint64_t();
    errors += test_unique_uint64_t_zero();
    errors += test_intersection_uint64_t();
    errors += test_union_uint64_t();
    errors += test_union_uint64_t_ba();

    benchmark_sort_uint64_t();

    return errors;
}
