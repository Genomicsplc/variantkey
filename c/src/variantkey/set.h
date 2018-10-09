// VariantKey
//
// set.h
//
// @category   Libraries
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

/**
 * @file set.h
 * @brief Utility functions to sort and join uint64_t arrays.
 *
 * Collection of utility functions to sort and join uint64_t arrays.
 */

#ifndef VARIANTKEY_SET_H
#define VARIANTKEY_SET_H

#include <inttypes.h>
#include <stdlib.h>

/**
 * Sorts in-memory an array of uint64_t values in ascending order.
 *
 * @param arr    Pointer to the first element of the array to process.
 * @param tmp    Pointer to the first element of a temporary array.
 * @param nitems Number of elements in the array.
 */
static inline void sort_uint64_t(uint64_t *arr, uint64_t *tmp, uint64_t nitems)
{
    uint32_t c7[256]= {0}, c6[256]= {0}, c5[256]= {0}, c4[256]= {0}, c3[256]= {0}, c2[256]= {0}, c1[256]= {0}, c0[256]= {0};
    uint32_t o7=0, o6=0, o5=0, o4=0, o3=0, o2=0, o1=0, o0=0;
    uint32_t t7, t6, t5, t4, t3, t2, t1, t0;
    uint32_t i;
    uint64_t v;
    // calculate counts
    for(i = 0; i < nitems; i++)
    {
        v = arr[i];
        c7[(v & 0xff)]++;
        c6[((v >> 8) & 0xff)]++;
        c5[((v >> 16) & 0xff)]++;
        c4[((v >> 24) & 0xff)]++;
        c3[((v >> 32) & 0xff)]++;
        c2[((v >> 40) & 0xff)]++;
        c1[((v >> 48) & 0xff)]++;
        c0[((v >> 56) & 0xff)]++;
    }
    // convert counts to offsets
    for(i = 0; i < 256; i++)
    {
        t7 = (o7 + c7[i]);
        c7[i] = o7;
        o7 = t7;
        t6 = (o6 + c6[i]);
        c6[i] = o6;
        o6 = t6;
        t5 = (o5 + c5[i]);
        c5[i] = o5;
        o5 = t5;
        t4 = (o4 + c4[i]);
        c4[i] = o4;
        o4 = t4;
        t3 = (o3 + c3[i]);
        c3[i] = o3;
        o3 = t3;
        t2 = (o2 + c2[i]);
        c2[i] = o2;
        o2 = t2;
        t1 = (o1 + c1[i]);
        c1[i] = o1;
        o1 = t1;
        t0 = (o0 + c0[i]);
        c0[i] = o0;
        o0 = t0;
    }
    // radix
    for (i = 0; i < nitems; i++)
    {
        v = arr[i];
        tmp[c7[(v & 0xff)]++] = v;
    }
    for (i = 0; i < nitems; i++)
    {
        v = tmp[i];
        arr[c6[((v >> 8) & 0xff)]++] = v;
    }
    for (i = 0; i < nitems; i++)
    {
        v = arr[i];
        tmp[c5[((v >> 16) & 0xff)]++] = v;
    }
    for (i = 0; i < nitems; i++)
    {
        v = tmp[i];
        arr[c4[((v >> 24) & 0xff)]++] = v;
    }
    for (i = 0; i < nitems; i++)
    {
        v = arr[i];
        tmp[c3[((v >> 32) & 0xff)]++] = v;
    }
    for (i = 0; i < nitems; i++)
    {
        v = tmp[i];
        arr[c2[((v >> 40) & 0xff)]++] = v;
    }
    for (i = 0; i < nitems; i++)
    {
        v = arr[i];
        tmp[c1[((v >> 48) & 0xff)]++] = v;
    }
    for (i = 0; i < nitems; i++)
    {
        v = tmp[i];
        arr[c0[((v >> 56) & 0xff)]++] = v;
    }
}

/**
 * Reverse in-place an array of uint64_t values.
 *
 * @param arr    Pointer to the first element of the array to process.
 * @param nitems Number of elements in the array.
 */
static inline void reverse_uint64_t(uint64_t *arr, uint64_t nitems)
{
    uint64_t *last = (arr + nitems);
    uint64_t tmp;
    while ((arr != last) && (arr != --last))
    {
        tmp = *last;
        *last = *arr;
        *arr++ = tmp;
    }
}

/**
 * Eliminates all but the first element from every consecutive group of equal values.
 *
 * @param arr    Pointer to the first element of the array to process.
 * @param nitems Number of elements in the array.
 *
 * @return Pointer to the end of the array.
 */
static inline uint64_t *unique_uint64_t(uint64_t *arr, uint64_t nitems)
{
    if (nitems == 0)
    {
        return arr;
    }
    uint64_t *last = (arr + nitems);
    uint64_t *p = arr;
    while (++arr != last)
    {
        if ((*p != *arr) && (*p++ != *arr))
        {
            *p = *arr;
        }
    }
    return ++p;
}

/**
 * Returns the intersection of two sorted uint64_t arrays.
 *
 * @param a_arr    Pointer to the first element of the first array to process.
 * @param a_nitems Number of elements in the first array.
 * @param b_arr    Pointer to the first element of the second array to process.
 * @param b_nitems Number of elements in the second array.
 * @param o_arr    Pointer to the first element or the output array.
 *
 * @return Pointer to the end of the array.
 */
static inline uint64_t *intersection_uint64_t(uint64_t *a_arr, uint64_t a_nitems, uint64_t *b_arr, uint64_t b_nitems, uint64_t *o_arr)
{
    uint64_t *a_last = (a_arr + a_nitems);
    uint64_t *b_last = (b_arr + b_nitems);
    while ((a_arr != a_last) && (b_arr != b_last))
    {
        if (*a_arr < *b_arr)
        {
            ++a_arr;
            continue;
        }
        if (*a_arr == *b_arr)
        {
            *o_arr++ = *a_arr++;
        }
        ++b_arr;
    }
    return o_arr;
}

/**
 * Returns the union of two sorted uint64_t arrays.
 *
 * @param a_arr    Pointer to the first element of the first array to process.
 * @param a_nitems Number of elements in the first array.
 * @param b_arr    Pointer to the first element of the second array to process.
 * @param b_nitems Number of elements in the second array.
 * @param o_arr    Pointer to the first element or the output array.
 *
 * @return Pointer to the end of the array.
 */
static inline uint64_t *union_uint64_t(uint64_t *a_arr, uint64_t a_nitems, uint64_t *b_arr, uint64_t b_nitems, uint64_t *o_arr)
{
    uint64_t *a_last = (a_arr + a_nitems);
    uint64_t *b_last = (b_arr + b_nitems);
    while ((a_arr != a_last) && (b_arr != b_last))
    {
        if (*a_arr < *b_arr)
        {
            *o_arr++ = *a_arr++;
            continue;
        }
        if (*a_arr > *b_arr)
        {
            *o_arr++ = *b_arr++;
            continue;
        }
        *o_arr++ = *a_arr++;
        b_arr++;
    }
    while (a_arr != a_last)
    {
        *o_arr++ = *a_arr++;
    }
    while (b_arr != b_last)
    {
        *o_arr++ = *b_arr++;
    }
    return o_arr;
}

#endif  // VARIANTKEY_SET_H
