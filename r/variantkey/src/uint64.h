// R UINT64 Module
//
// uint64.h
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

#include <inttypes.h>
#include <R.h>
#include <Rdefines.h>
#include <stdlib.h>
#include "../../../c/src/variantkey/hex.h"
#include "../../../c/src/variantkey/set.h"

#define MAX_UINT64_DEC_CHARS 21

// --- UINT64 ---

#define CMP_EQ(r, x, y) r = (x == y);
#define CMP_NE(r, x, y) r = (x != y);
#define CMP_LE(r, x, y) r = (x <= y);
#define CMP_GE(r, x, y) r = (x >= y);
#define CMP_LT(r, x, y) r = (x < y);
#define CMP_GT(r, x, y) r = (x > y);

#define define_cmpfunc(T) \
SEXP R_##T##_uint64(SEXP x, SEXP y, SEXP r) \
{ \
    uint64_t i, ix, iy; \
    uint64_t n = LENGTH(r), nx = LENGTH(x), ny = LENGTH(y); \
    Rboolean *pr = (Rboolean *)LOGICAL(r); \
    uint64_t *px = (uint64_t *)REAL(x); \
    uint64_t *py = (uint64_t *)REAL(y); \
    for (i = ix = iy = 0; i < n; ix = ((++ix == nx) ? 0 : ix), iy = ((++iy == ny) ? 0 : iy), ++i) \
    { \
        CMP_##T(pr[i], px[ix], py[iy]) \
    } \
    return r; \
}

define_cmpfunc(EQ)
define_cmpfunc(NE)
define_cmpfunc(LE)
define_cmpfunc(GE)
define_cmpfunc(LT)
define_cmpfunc(GT)

SEXP R_decstr_to_uint64(SEXP str, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint64_t *res = (uint64_t *)REAL(ret);
    for(i = 0; i < n; i++)
    {
        res[i] = strtoull(CHAR(STRING_ELT(str, i)), NULL, 10);
    }
    return ret;
}

SEXP R_uint64_to_decstr(SEXP x, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint64_t *px = (uint64_t *)REAL(x);
    char str[MAX_UINT64_DEC_CHARS];
    for(i = 0; i < n; i++)
    {
        snprintf(str, MAX_UINT64_DEC_CHARS, "%" PRIu64, px[i]);
        SET_STRING_ELT(ret, i, mkChar(str));
    }
    return ret;
}

SEXP R_hex_uint64_t(SEXP x, SEXP ret)
{
    uint64_t i, n = LENGTH(x);
    uint64_t *px = (uint64_t *)REAL(x);
    char hex[17];
    for(i = 0; i < n; i++)
    {
        hex_uint64_t(px[i], hex);
        SET_STRING_ELT(ret, i, mkChar(hex));
    }
    return ret;
}

SEXP R_parse_hex_uint64_t(SEXP hex, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint64_t *res = (uint64_t *)REAL(ret);
    for(i = 0; i < n; i++)
    {
        res[i] = parse_hex_uint64_t(CHAR(STRING_ELT(hex, i)));
    }
    return ret;
}

SEXP R_double_to_uint64(SEXP x, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint64_t *res = (uint64_t *)REAL(ret);
    double *px = REAL(x);
    for(i = 0; i < n; i++)
    {
        res[i] = (uint64_t)(px[i]);
    }
    return ret;
}

SEXP R_integer_to_uint64(SEXP x, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint64_t *res = (uint64_t *)REAL(ret);
    uint32_t *px = (uint32_t *)INTEGER(x);
    for(i = 0; i < n; i++)
    {
        res[i] = (uint64_t)(px[i]);
    }
    return ret;
}

SEXP R_sort_uint64(SEXP x, SEXP ret)
{
    uint64_t n = LENGTH(ret);
    uint64_t *res = (uint64_t *)REAL(ret);
    memcpy((void *)res, (void *)REAL(x), (n * sizeof(uint64_t)));
    sort_uint64_t(res, n);
    return ret;
}

SEXP R_reverse_uint64(SEXP x, SEXP ret)
{
    uint64_t n = LENGTH(ret);
    uint64_t *res = (uint64_t *)REAL(ret);
    memcpy((void *)res, (void *)REAL(x), (n * sizeof(uint64_t)));
    reverse_uint64_t(res, n);
    return ret;
}

SEXP R_unique_uint64(SEXP x, SEXP ret)
{
    uint64_t n = LENGTH(ret);
    uint64_t *res = (uint64_t *)REAL(ret);
    memcpy((void *)res, (void *)REAL(x), (n * sizeof(uint64_t)));
    uint64_t *p = unique_uint64_t(res, n);
    SETLENGTH(ret, (p - res));
    return ret;
}

SEXP R_intersect_uint64(SEXP x, SEXP y, SEXP ret)
{
    uint64_t nx = LENGTH(x), ny = LENGTH(y);
    uint64_t *res = (uint64_t *)REAL(ret);
    uint64_t *px = (uint64_t *)REAL(x);
    uint64_t *py = (uint64_t *)REAL(y);
    uint64_t *p = intersection_uint64_t(px, nx, py, ny, res);
    SETLENGTH(ret, (p - res));
    return ret;
}

SEXP R_union_uint64(SEXP x, SEXP y, SEXP ret)
{
    uint64_t nx = LENGTH(x), ny = LENGTH(y);
    uint64_t *res = (uint64_t *)REAL(ret);
    uint64_t *px = (uint64_t *)REAL(x);
    uint64_t *py = (uint64_t *)REAL(y);
    uint64_t *p = union_uint64_t(px, nx, py, ny, res);
    SETLENGTH(ret, (p - res));
    return ret;
}
