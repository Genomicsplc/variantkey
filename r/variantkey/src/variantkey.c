// R VariantKey Module
//
// variantkey.c
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

#include <R.h>
#include <Rdefines.h>
#include <stdlib.h>
#include "../../../c/src/variantkey/binsearch.h"
#include "../../../c/src/variantkey/esid.h"
#include "../../../c/src/variantkey/genoref.h"
#include "../../../c/src/variantkey/hex.h"
#include "../../../c/src/variantkey/nrvk.h"
#include "../../../c/src/variantkey/regionkey.h"
#include "../../../c/src/variantkey/rsidvar.h"
#include "../../../c/src/variantkey/variantkey.h"

#define ALLELE_BUFFSIZE 12
#define MAX_UINT64_DEC_CHARS 21

// --- LOGIC ---
#define CMP_EQ(x, y, r) r = ((x == y) ? TRUE : FALSE);
#define CMP_NE(x, y, r) r = ((x != y) ? TRUE : FALSE);
#define CMP_LE(x, y, r) r = ((x <= y) ? TRUE : FALSE);
#define CMP_GE(x, y, r) r = ((x >= y) ? TRUE : FALSE);
#define CMP_LT(x, y, r) r = ((x < y) ? TRUE : FALSE);
#define CMP_GT(x, y, r) r = ((x > y) ? TRUE : FALSE);

#define define_cmpfunc(T) \
SEXP R_##T##_uint64(SEXP x, SEXP y, SEXP r) \
{ \
    uint64_t i, n = LENGTH(r); \
    uint64_t ix, nx = LENGTH(x); \
    uint64_t iy, ny = LENGTH(y); \
    uint64_t *vx = (uint64_t *)REAL(x); \
    uint64_t *vy = (uint64_t *)REAL(y); \
    Rboolean *vr = (Rboolean *)LOGICAL(r); \
    for (i = ix = iy = 0; i < n; ix = ((++ix == nx) ? 0 : ix), iy = ((++iy == ny) ? 0 : iy), ++i) \
    { \
        CMP_##T(vx[ix], vy[iy], vr[i]) \
    } \
    return r; \
}

define_cmpfunc(EQ)
define_cmpfunc(NE)
define_cmpfunc(LE)
define_cmpfunc(GE)
define_cmpfunc(LT)
define_cmpfunc(GT)

// --- UINT64 ---

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
        snprintf(str, MAX_UINT64_DEC_CHARS, PRIu64, px[i]);
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

// --- VARIANTKEY ---

SEXP R_encode_chrom(SEXP chrom, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint32_t *res = (uint32_t *)INTEGER(ret);
    for(i = 0; i < n; i++)
    {
        const char *chr = CHAR(STRING_ELT(chrom, i));
        res[i] = encode_chrom(chr, strlen(chr));
    }
    return ret;
}

SEXP R_decode_chrom(SEXP code, SEXP ret)
{
    uint64_t i, n = LENGTH(code);
    uint32_t *c = (uint32_t *)INTEGER(code);
    char chrom[3];
    for(i = 0; i < n; i++)
    {
        chrom[0] = 0;
        decode_chrom((uint8_t)(c[i]), chrom);
        SET_STRING_ELT(ret, i, mkChar(chrom));
    }
    return ret;
}

SEXP R_encode_refalt(SEXP ref, SEXP alt, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint32_t *res = (uint32_t *)INTEGER(ret);
    for(i = 0; i < n; i++)
    {
        const char *r = CHAR(STRING_ELT(ref, i));
        const char *a = CHAR(STRING_ELT(alt, i));
        res[i] = encode_refalt(r, strlen(r), a, strlen(a));
    }
    return ret;
}

SEXP R_decode_refalt(SEXP code, SEXP rref, SEXP ralt)
{
    uint64_t i, n = LENGTH(code);
    uint32_t *c = (uint32_t *)INTEGER(code);
    char ref[ALLELE_BUFFSIZE], alt[ALLELE_BUFFSIZE];
    size_t sizeref = 0, sizealt = 0;
    for(i = 0; i < n; i++)
    {
        ref[0] = 0;
        alt[0] = 0;
        sizeref = 0;
        sizealt = 0;
        decode_refalt(c[i], ref, &sizeref, alt, &sizealt);
        SET_STRING_ELT(rref, i, mkChar(ref));
        SET_STRING_ELT(ralt, i, mkChar(alt));
    }
    const char *names[] = {"REF", "ALT", ""};
    SEXP res = PROTECT(mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, rref);
    SET_VECTOR_ELT(res, 1, ralt);
    UNPROTECT(1);
    return res;
}

SEXP R_encode_variantkey(SEXP chrom, SEXP pos, SEXP refalt, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint64_t *res = (uint64_t *)REAL(ret);
    uint32_t *pchrom = (uint32_t *)INTEGER(chrom);
    uint32_t *ppos = (uint32_t *)INTEGER(pos);
    uint32_t *prefalt = (uint32_t *)INTEGER(refalt);
    for(i = 0; i < n; i++)
    {
        res[i] = encode_variantkey((uint8_t)(pchrom[i]), ppos[i], prefalt[i]);
    }
    return ret;
}

SEXP R_extract_variantkey_chrom(SEXP vk, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint32_t *res = (uint32_t *)INTEGER(ret);
    uint64_t *pvk = (uint64_t *)REAL(vk);
    for(i = 0; i < n; i++)
    {
        res[i] = extract_variantkey_chrom(pvk[i]);
    }
    return ret;
}

SEXP R_extract_variantkey_pos(SEXP vk, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint32_t *res = (uint32_t *)INTEGER(ret);
    uint64_t *pvk = (uint64_t *)REAL(vk);
    for(i = 0; i < n; i++)
    {
        res[i] = extract_variantkey_pos(pvk[i]);
    }
    return ret;
}

SEXP R_extract_variantkey_refalt(SEXP vk, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint32_t *res = (uint32_t *)INTEGER(ret);
    uint64_t *pvk = (uint64_t *)REAL(vk);
    for(i = 0; i < n; i++)
    {
        res[i] = extract_variantkey_refalt(pvk[i]);
    }
    return ret;
}

SEXP R_decode_variantkey(SEXP vk, SEXP rchrom, SEXP rpos, SEXP rrefalt)
{
    uint64_t i, n = LENGTH(rchrom);
    uint32_t *pchrom = (uint32_t *)INTEGER(rchrom);
    uint32_t *ppos = (uint32_t *)INTEGER(rpos);
    uint32_t *prefalt = (uint32_t *)INTEGER(rrefalt);
    uint64_t *pvk = (uint64_t *)REAL(vk);
    variantkey_t v = {0, 0, 0};
    for(i = 0; i < n; i++)
    {
        v.chrom = 0;
        v.pos = 0;
        v.refalt = 0;
        decode_variantkey(pvk[i], &v);
        pchrom[i] = v.chrom;
        ppos[i] = v.pos;
        prefalt[i] = v.refalt;
    }
    const char *names[] = {"CHROM", "POS", "REFALT", ""};
    SEXP res = PROTECT(mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, rchrom);
    SET_VECTOR_ELT(res, 1, rpos);
    SET_VECTOR_ELT(res, 2, rrefalt);
    UNPROTECT(1);
    return res;
}

SEXP R_variantkey(SEXP chrom, SEXP pos, SEXP ref, SEXP alt, SEXP ret)
{
    uint64_t i, n = LENGTH(pos);
    uint64_t *res = (uint64_t *)REAL(ret);
    uint32_t *ppos = (uint32_t *)INTEGER(pos);
    for(i = 0; i < n; i++)
    {
        const char *c = CHAR(STRING_ELT(chrom, i));
        const char *r = CHAR(STRING_ELT(ref, i));
        const char *a = CHAR(STRING_ELT(alt, i));
        res[i] = variantkey(c, strlen(c), ppos[i], r, strlen(r), a, strlen(a));
    }
    return ret;
}

SEXP R_variantkey_range(SEXP chrom, SEXP pos_min, SEXP pos_max, SEXP rmin, SEXP rmax)
{
    uint64_t i, n = LENGTH(chrom);
    uint32_t *pmin = (uint32_t *)INTEGER(rmin);
    uint32_t *pmax = (uint32_t *)INTEGER(rmax);
    uint32_t *pchrom = (uint32_t *)INTEGER(chrom);
    uint32_t *ppos_min = (uint32_t *)INTEGER(pos_min);
    uint32_t *ppos_max = (uint32_t *)INTEGER(pos_max);
    vkrange_t r = {0, 0};
    for(i = 0; i < n; i++)
    {
        r.min = 0;
        r.max = 0;
        variantkey_range((uint8_t)(pchrom[i]), ppos_min[i], ppos_max[i], &r);
        pmin[i] = r.min;
        pmax[i] = r.max;
    }
    const char *names[] = {"MIN", "MAX", ""};
    SEXP res = PROTECT(mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, rmin);
    SET_VECTOR_ELT(res, 1, rmax);
    UNPROTECT(1);
    return res;
}

SEXP R_compare_variantkey_chrom(SEXP vka, SEXP vkb, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    int32_t *res = (int32_t *)INTEGER(ret);
    uint64_t *pvka = (uint64_t *)REAL(vka);
    uint64_t *pvkb = (uint64_t *)REAL(vkb);
    for(i = 0; i < n; i++)
    {
        res[i] = compare_variantkey_chrom(pvka[i], pvkb[i]);
    }
    return ret;
}

SEXP R_compare_variantkey_chrom_pos(SEXP vka, SEXP vkb, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    int8_t *res = (int8_t *)INTEGER(ret);
    uint64_t *pvka = (uint64_t *)REAL(vka);
    uint64_t *pvkb = (uint64_t *)REAL(vkb);
    for(i = 0; i < n; i++)
    {
        res[i] = compare_variantkey_chrom_pos(pvka[i], pvkb[i]);
    }
    return ret;
}

SEXP R_variantkey_hex(SEXP vk, SEXP ret)
{
    return R_hex_uint64_t(vk, ret);
}

SEXP R_parse_variantkey_hex(SEXP hex, SEXP ret)
{
    return R_parse_hex_uint64_t(hex, ret);
}

// --- BINSEARCH ---

static const mmfile_t *get_mmfile_mf(SEXP mf)
{
    if (R_ExternalPtrAddr(mf) == NULL)
    {
        return (const mmfile_t *)Calloc(1, mmfile_t);
    }
    return (const mmfile_t *)R_ExternalPtrAddr(mf);
}

static void destroy_mf(SEXP mf)
{
    const mmfile_t *cmf = get_mmfile_mf(mf);
    if (cmf == NULL)
    {
        return;
    }
    Free(cmf);
    R_ClearExternalPtr(mf);
}

SEXP R_munmap_binfile(SEXP mf)
{
    const mmfile_t *cmf = get_mmfile_mf(mf);
    SEXP res;
    PROTECT(res = NEW_INTEGER(1));
    INTEGER(res)[0] = munmap_binfile(*cmf);
    UNPROTECT(1);
    return res;
}

// --- RSIDVAR ---

static const rsidvar_cols_t *get_rsidvar_mc(SEXP mc)
{
    if (R_ExternalPtrAddr(mc) == NULL)
    {
        return (const rsidvar_cols_t *)Calloc(1, rsidvar_cols_t);
    }
    return (const rsidvar_cols_t *)R_ExternalPtrAddr(mc);
}

static void destroy_rsidvar_mc(SEXP mc)
{
    const rsidvar_cols_t *cmc = get_rsidvar_mc(mc);
    if (cmc == NULL)
    {
        return;
    }
    Free(cmc);
    R_ClearExternalPtr(mc);
}

SEXP R_mmap_rsvk_file(SEXP file, SEXP ctbytes)
{
    mmfile_t *mf = (mmfile_t *)Calloc(1, mmfile_t);
    rsidvar_cols_t *mc = (rsidvar_cols_t *)Calloc(1, rsidvar_cols_t);
    mf->ncols = (uint8_t)(LENGTH(ctbytes));
    int *px = INTEGER(ctbytes);
    for (int i = 0; i < mf->ncols; i++)
    {
        mf->ctbytes[i] = (uint8_t)px[i];
    }
    mmap_rsvk_file(CHAR(STRING_ELT(file, 0)), mf, mc);
    SEXP emf = PROTECT(R_MakeExternalPtr(mf, R_NilValue, R_NilValue));
    R_RegisterCFinalizerEx(emf, destroy_mf, TRUE);
    SEXP emc = PROTECT(R_MakeExternalPtr(mc, R_NilValue, R_NilValue));
    R_RegisterCFinalizerEx(emc, destroy_rsidvar_mc, TRUE);
    const char *names[] = {"MF", "MC", "NROWS", ""};
    SEXP res = PROTECT(mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, emf);
    SET_VECTOR_ELT(res, 1, emc);
    SET_VECTOR_ELT(res, 2, ScalarInteger(mc->nrows));
    UNPROTECT(3);
    return res;
}

SEXP R_mmap_vkrs_file(SEXP file, SEXP ctbytes)
{
    mmfile_t *mf = (mmfile_t *)Calloc(1, mmfile_t);
    rsidvar_cols_t *mc = (rsidvar_cols_t *)Calloc(1, rsidvar_cols_t);
    mf->ncols = (uint8_t)(LENGTH(ctbytes));
    int *px = INTEGER(ctbytes);
    for (int i = 0; i < mf->ncols; i++)
    {
        mf->ctbytes[i] = (uint8_t)px[i];
    }
    mmap_vkrs_file(CHAR(STRING_ELT(file, 0)), mf, mc);
    SEXP emf = PROTECT(R_MakeExternalPtr(mf, R_NilValue, R_NilValue));
    R_RegisterCFinalizerEx(emf, destroy_mf, TRUE);
    SEXP emc = PROTECT(R_MakeExternalPtr(mc, R_NilValue, R_NilValue));
    R_RegisterCFinalizerEx(emc, destroy_rsidvar_mc, TRUE);
    const char *names[] = {"MF", "MC", "NROWS", ""};
    SEXP res = PROTECT(mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, emf);
    SET_VECTOR_ELT(res, 1, emc);
    SET_VECTOR_ELT(res, 2, ScalarInteger(mc->nrows));
    UNPROTECT(3);
    return res;
}

SEXP R_find_rv_variantkey_by_rsid(SEXP mc, SEXP first, SEXP last, SEXP rsid, SEXP rvk, SEXP rfirst)
{
    uint64_t i, n = LENGTH(rvk);
    uint64_t *pvk = (uint64_t *)REAL(rvk);
    uint32_t *pfirst = (uint32_t *)INTEGER(rfirst);
    uint32_t *prsid = (uint32_t *)INTEGER(rsid);
    const rsidvar_cols_t *cmc = get_rsidvar_mc(mc);
    uint64_t ppfirst = asInteger(first);
    uint64_t pplast = asInteger(last);
    uint64_t tfirst;
    for(i = 0; i < n; i++)
    {
        tfirst = ppfirst;
        pvk[i] = find_rv_variantkey_by_rsid(*cmc, &tfirst, pplast, prsid[i]);
        pfirst[i] = tfirst;
    }
    const char *names[] = {"VK", "FIRST", ""};
    SEXP res = PROTECT(mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, rvk);
    SET_VECTOR_ELT(res, 1, rfirst);
    UNPROTECT(1);
    return res;
}

SEXP R_get_next_rv_variantkey_by_rsid(SEXP mc, SEXP pos, SEXP last, SEXP rsid, SEXP rvk, SEXP rpos)
{
    uint64_t i, n = LENGTH(rvk);
    uint64_t *pvk = (uint64_t *)REAL(rvk);
    uint32_t *ppos = (uint32_t *)INTEGER(rpos);
    uint32_t *ipos = (uint32_t *)INTEGER(pos);
    uint32_t *prsid = (uint32_t *)INTEGER(rsid);
    const rsidvar_cols_t *cmc = get_rsidvar_mc(mc);
    uint64_t pplast = asInteger(last);
    uint64_t tpos;
    for(i = 0; i < n; i++)
    {
        tpos = ipos[i];
        pvk[i] = get_next_rv_variantkey_by_rsid(*cmc, &tpos, pplast, prsid[i]);
        ppos[i] = tpos;
    }
    const char *names[] = {"VK", "POS", ""};
    SEXP res = PROTECT(mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, rvk);
    SET_VECTOR_ELT(res, 1, rpos);
    UNPROTECT(1);
    return res;
}

SEXP R_find_all_rv_variantkey_by_rsid(SEXP mc, SEXP first, SEXP last, SEXP rsid)
{
    static const int vecsize = 10; // limit the maximum nuber of results
    SEXP res = PROTECT(allocVector(VECSXP, vecsize));
    uint32_t i = 0;
    char hex[17];
    uint64_t pfirst = asInteger(first);
    const rsidvar_cols_t *cmc = get_rsidvar_mc(mc);
    uint64_t vk = find_rv_variantkey_by_rsid(*cmc, &pfirst, asInteger(last), asInteger(rsid));
    while ((vk > 0) && (i < vecsize))
    {
        variantkey_hex(vk, hex);
        SET_VECTOR_ELT(res, i, Rf_mkString(hex));
        i++;
        vk = get_next_rv_variantkey_by_rsid(*cmc, &pfirst, asInteger(last), asInteger(rsid));
    }
    SETLENGTH(res, i);
    UNPROTECT(1);
    return res;
}

SEXP R_find_vr_rsid_by_variantkey(SEXP mc, SEXP first, SEXP last, SEXP vk, SEXP rrsid, SEXP rfirst)
{
    uint64_t i, n = LENGTH(rrsid);
    uint32_t *prsid = (uint32_t *)INTEGER(rrsid);
    uint32_t *pfirst = (uint32_t *)INTEGER(rfirst);
    uint64_t *pvk = (uint64_t *)REAL(vk);
    const rsidvar_cols_t *cmc = get_rsidvar_mc(mc);
    uint64_t ppfirst = asInteger(first);
    uint64_t pplast = asInteger(last);
    uint64_t tfirst;
    for(i = 0; i < n; i++)
    {
        tfirst = ppfirst;
        prsid[i] = find_vr_rsid_by_variantkey(*cmc, &tfirst, pplast, pvk[i]);
        pfirst[i] = tfirst;
    }
    const char *names[] = {"RSID", "FIRST", ""};
    SEXP res = PROTECT(mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, rrsid);
    SET_VECTOR_ELT(res, 1, rfirst);
    UNPROTECT(1);
    return res;
}

SEXP R_find_vr_chrompos_range(SEXP mc, SEXP first, SEXP last, SEXP chrom, SEXP pos_min, SEXP pos_max, SEXP rrsid, SEXP rfirst, SEXP rlast)
{
    uint64_t i, n = LENGTH(rrsid);
    uint32_t *prsid = (uint32_t *)INTEGER(rrsid);
    uint32_t *pfirst = (uint32_t *)INTEGER(rfirst);
    uint32_t *plast = (uint32_t *)INTEGER(rlast);
    uint32_t *pchrom = (uint32_t *)INTEGER(chrom);
    uint32_t *ppos_min = (uint32_t *)INTEGER(pos_min);
    uint32_t *ppos_max = (uint32_t *)INTEGER(pos_max);
    const rsidvar_cols_t *cmc = get_rsidvar_mc(mc);
    uint64_t ppfirst = asInteger(first);
    uint64_t pplast = asInteger(last);
    uint64_t tfirst;
    uint64_t tlast;
    for(i = 0; i < n; i++)
    {
        tfirst = ppfirst;
        tlast = pplast;
        prsid[i] = find_vr_chrompos_range(*cmc, &tfirst, &tlast, (uint8_t)(pchrom[i]), ppos_min[i], ppos_max[i]);
        pfirst[i] = tfirst;
        plast[i] = tlast;
    }
    const char *names[] = {"RSID", "FIRST", "LAST", ""};
    SEXP res = PROTECT(mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, rrsid);
    SET_VECTOR_ELT(res, 1, rfirst);
    SET_VECTOR_ELT(res, 2, rlast);
    UNPROTECT(1);
    return res;
}

// --- NRVK ---

static const nrvk_cols_t *get_nrvk_mc(SEXP mc)
{
    if (R_ExternalPtrAddr(mc) == NULL)
    {
        return (const nrvk_cols_t *)Calloc(1, nrvk_cols_t);
    }
    return (const nrvk_cols_t *)R_ExternalPtrAddr(mc);
}

static void destroy_nrvk_mc(SEXP mc)
{
    const nrvk_cols_t *cmc = get_nrvk_mc(mc);
    if (cmc == NULL)
    {
        return;
    }
    Free(cmc);
    R_ClearExternalPtr(mc);
}

SEXP R_mmap_nrvk_file(SEXP file)
{
    mmfile_t *mf = (mmfile_t *)Calloc(1, mmfile_t);
    nrvk_cols_t *mc = (nrvk_cols_t *)Calloc(1, nrvk_cols_t);
    mmap_nrvk_file(CHAR(STRING_ELT(file, 0)), mf, mc);
    SEXP emf = PROTECT(R_MakeExternalPtr(mf, R_NilValue, R_NilValue));
    R_RegisterCFinalizerEx(emf, destroy_mf, TRUE);
    SEXP emc = PROTECT(R_MakeExternalPtr(mc, R_NilValue, R_NilValue));
    R_RegisterCFinalizerEx(emc, destroy_nrvk_mc, TRUE);
    const char *names[] = {"MF", "MC", "NROWS", ""};
    SEXP res = PROTECT(mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, emf);
    SET_VECTOR_ELT(res, 1, emc);
    SET_VECTOR_ELT(res, 2, ScalarInteger(mc->nrows));
    UNPROTECT(3);
    return res;
}

SEXP R_find_ref_alt_by_variantkey(SEXP mc, SEXP vk, SEXP rref, SEXP ralt)
{
    uint64_t i, n = LENGTH(vk);
    uint64_t *pvk = (uint64_t *)REAL(vk);
    const nrvk_cols_t *cmc = get_nrvk_mc(mc);
    size_t sizeref = 0, sizealt = 0;
    char ref[ALLELE_MAXSIZE] = "", alt[ALLELE_MAXSIZE] = "";
    for(i = 0; i < n; i++)
    {
        ref[0] = 0;
        alt[0] = 0;
        find_ref_alt_by_variantkey(*cmc, pvk[i], ref, &sizeref, alt, &sizealt);
        SET_STRING_ELT(rref, i, mkChar(ref));
        SET_STRING_ELT(ralt, i, mkChar(alt));
    }
    const char *names[] = {"REF", "ALT", ""};
    SEXP res = PROTECT(mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, rref);
    SET_VECTOR_ELT(res, 1, ralt);
    UNPROTECT(1);
    return res;
}

SEXP R_reverse_variantkey(SEXP mc, SEXP vk, SEXP rchrom, SEXP rpos, SEXP rref, SEXP ralt)
{
    uint64_t i, n = LENGTH(vk);
    uint64_t *pvk = (uint64_t *)REAL(vk);
    uint32_t *ppos = (uint32_t *)INTEGER(rpos);
    const nrvk_cols_t *cmc = get_nrvk_mc(mc);
    variantkey_rev_t v = {0, 0, 0, 0, 0, 0};
    for(i = 0; i < n; i++)
    {
        v.chrom[0] = 0;
        v.pos = 0;
        v.ref[0] = 0;
        v.alt[0] = 0;
        v.sizeref = 0;
        v.sizealt = 0;
        reverse_variantkey(*cmc, pvk[i], &v);
        SET_STRING_ELT(rchrom, i, mkChar(v.chrom));
        ppos[i] = v.pos;
        SET_STRING_ELT(rref, i, mkChar(v.ref));
        SET_STRING_ELT(ralt, i, mkChar(v.alt));
    }
    const char *names[] = {"CHROM", "POS", "REF", "ALT", ""};
    SEXP res = PROTECT(mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, rchrom);
    SET_VECTOR_ELT(res, 1, rpos);
    SET_VECTOR_ELT(res, 2, rref);
    SET_VECTOR_ELT(res, 3, ralt);
    UNPROTECT(1);
    return res;
}

SEXP R_get_variantkey_ref_length(SEXP mc, SEXP vk, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint32_t *res = (uint32_t *)INTEGER(ret);
    uint64_t *pvk = (uint64_t *)REAL(vk);
    const nrvk_cols_t *cmc = get_nrvk_mc(mc);
    for(i = 0; i < n; i++)
    {
        res[i] = get_variantkey_ref_length(*cmc, pvk[i]);
    }
    return ret;
}

SEXP R_get_variantkey_endpos(SEXP mc, SEXP vk, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint32_t *res = (uint32_t *)INTEGER(ret);
    uint64_t *pvk = (uint64_t *)REAL(vk);
    const nrvk_cols_t *cmc = get_nrvk_mc(mc);
    for(i = 0; i < n; i++)
    {
        res[i] = get_variantkey_endpos(*cmc, pvk[i]);
    }
    return ret;
}

SEXP R_get_variantkey_chrom_startpos(SEXP vk, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint64_t *res = (uint64_t *)REAL(ret);
    uint64_t *pvk = (uint64_t *)REAL(vk);
    for(i = 0; i < n; i++)
    {
        res[i] = get_variantkey_chrom_startpos(pvk[i]);
    }
    return ret;
}

SEXP R_get_variantkey_chrom_endpos(SEXP mc, SEXP vk, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint64_t *res = (uint64_t *)REAL(ret);
    uint64_t *pvk = (uint64_t *)REAL(vk);
    const nrvk_cols_t *cmc = get_nrvk_mc(mc);
    for(i = 0; i < n; i++)
    {
        res[i] = get_variantkey_chrom_endpos(*cmc, pvk[i]);
    }
    return ret;
}

SEXP R_nrvk_bin_to_tsv(SEXP mc, SEXP tsvfile)
{
    SEXP res;
    PROTECT(res = NEW_INTEGER(1));
    const nrvk_cols_t *cmc = get_nrvk_mc(mc);
    INTEGER(res)[0] = nrvk_bin_to_tsv(*cmc, CHAR(STRING_ELT(tsvfile, 0)));
    UNPROTECT(1);
    return res;
}

// --- GENOREF ---

SEXP R_mmap_genoref_file(SEXP file)
{
    mmfile_t *mf = (mmfile_t *)Calloc(1, mmfile_t);
    mmap_genoref_file(CHAR(STRING_ELT(file, 0)), mf);
    SEXP emf = PROTECT(R_MakeExternalPtr(mf, R_NilValue, R_NilValue));
    R_RegisterCFinalizerEx(emf, destroy_mf, TRUE);
    const char *names[] = {"MF", "SIZE", ""};
    SEXP res = PROTECT(mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, emf);
    SET_VECTOR_ELT(res, 1, ScalarInteger(mf->size));
    UNPROTECT(2);
    return res;
}

SEXP R_get_genoref_seq(SEXP mf, SEXP chrom, SEXP pos, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint32_t *res = (uint32_t *)INTEGER(ret);
    uint32_t *pchrom = (uint32_t *)INTEGER(chrom);
    uint32_t *ppos = (uint32_t *)INTEGER(pos);
    const mmfile_t *cmf = get_mmfile_mf(mf);
    for(i = 0; i < n; i++)
    {
        res[i] = get_genoref_seq(*cmf, (uint8_t)(pchrom[i]), ppos[i]);
    }
    return ret;
}

SEXP R_check_reference(SEXP mf, SEXP chrom, SEXP pos, SEXP ref, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    int *res = (int *)INTEGER(ret);
    uint32_t *pchrom = (uint32_t *)INTEGER(chrom);
    uint32_t *ppos = (uint32_t *)INTEGER(pos);
    const mmfile_t *cmf = get_mmfile_mf(mf);
    for(i = 0; i < n; i++)
    {
        const char *pref = CHAR(STRING_ELT(ref, i));
        res[i] = check_reference(*cmf, (uint8_t)(pchrom[i]), ppos[i], pref, strlen(pref));
    }
    return ret;
}

SEXP R_flip_allele(SEXP allele, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    char *s;
    for(i = 0; i < n; i++)
    {
        s = Rf_acopy_string(CHAR(STRING_ELT(allele, i)));
        flip_allele(s, strlen(s));
        SET_STRING_ELT(ret, i, mkChar(s));
    }
    return ret;
}

SEXP R_normalize_variant(SEXP mf, SEXP chrom, SEXP pos, SEXP ref, SEXP alt, SEXP ret, SEXP rpos, SEXP rref, SEXP ralt)
{
    uint64_t i, n = LENGTH(ret);
    int *code = (int *)INTEGER(ret);
    uint32_t *ppos = (uint32_t *)INTEGER(rpos);
    uint32_t *ichrom = (uint32_t *)INTEGER(chrom);
    uint32_t *ipos = (uint32_t *)INTEGER(pos);
    const mmfile_t *cmf = get_mmfile_mf(mf);
    size_t sizeref;
    size_t sizealt;
    char *r;
    char *a;
    for(i = 0; i < n; i++)
    {
        r = Rf_acopy_string(CHAR(STRING_ELT(ref, i)));
        a = Rf_acopy_string(CHAR(STRING_ELT(alt, i)));
        sizeref = strlen(r);
        sizealt = strlen(a);
        code[i] = normalize_variant(*cmf, (uint8_t)(ichrom[i]), &(ipos[i]), r, &sizeref, a, &sizealt);
        SET_STRING_ELT(rref, i, mkChar(r));
        SET_STRING_ELT(ralt, i, mkChar(a));
    }
    const char *names[] = {"RET", "POS", "REF", "ALT", ""};
    SEXP res = PROTECT(mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, ret);
    SET_VECTOR_ELT(res, 1, rpos);
    SET_VECTOR_ELT(res, 2, rref);
    SET_VECTOR_ELT(res, 3, ralt);
    UNPROTECT(1);
    return res;
}

// --- REGIONKEY ---

SEXP R_encode_region_strand(SEXP strand, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint32_t *res = (uint32_t *)INTEGER(ret);
    int32_t *pstrand = (int32_t *)INTEGER(strand);
    for(i = 0; i < n; i++)
    {
        res[i] = encode_region_strand((int8_t)(pstrand[i]));
    }
    return ret;
}

SEXP R_decode_region_strand(SEXP strand, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    int32_t *res = (int32_t *)INTEGER(ret);
    uint32_t *pstrand = (uint32_t *)INTEGER(strand);
    for(i = 0; i < n; i++)
    {
        res[i] = decode_region_strand((uint8_t)(pstrand[i]));
    }
    return ret;
}

SEXP R_encode_regionkey(SEXP chrom, SEXP startpos, SEXP endpos, SEXP strand, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint64_t *res = (uint64_t *)REAL(ret);
    uint32_t *pchrom = (uint32_t *)INTEGER(chrom);
    uint32_t *pstartpos = (uint32_t *)INTEGER(startpos);
    uint32_t *pendpos = (uint32_t *)INTEGER(endpos);
    uint32_t *pstrand = (uint32_t *)INTEGER(strand);
    for(i = 0; i < n; i++)
    {
        res[i] = encode_regionkey((uint8_t)(pchrom[i]), pstartpos[i], pendpos[i], (uint8_t)(pstrand[i]));
    }
    return ret;
}

SEXP R_extract_regionkey_chrom(SEXP rk, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint32_t *res = (uint32_t *)INTEGER(ret);
    uint64_t *prk = (uint64_t *)REAL(rk);
    for(i = 0; i < n; i++)
    {
        res[i] = extract_regionkey_chrom(prk[i]);
    }
    return ret;
}

SEXP R_extract_regionkey_startpos(SEXP rk, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint32_t *res = (uint32_t *)INTEGER(ret);
    uint64_t *prk = (uint64_t *)REAL(rk);
    for(i = 0; i < n; i++)
    {
        res[i] = extract_regionkey_startpos(prk[i]);
    }
    return ret;
}

SEXP R_extract_regionkey_endpos(SEXP rk, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint32_t *res = (uint32_t *)INTEGER(ret);
    uint64_t *prk = (uint64_t *)REAL(rk);
    for(i = 0; i < n; i++)
    {
        res[i] = extract_regionkey_endpos(prk[i]);
    }
    return ret;
}

SEXP R_extract_regionkey_strand(SEXP rk, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint32_t *res = (uint32_t *)INTEGER(ret);
    uint64_t *prk = (uint64_t *)REAL(rk);
    for(i = 0; i < n; i++)
    {
        res[i] = extract_regionkey_strand(prk[i]);
    }
    return ret;
}

SEXP R_decode_regionkey(SEXP rk, SEXP rchrom, SEXP rstartpos, SEXP rendpos, SEXP rstrand)
{
    uint64_t i, n = LENGTH(rchrom);
    uint32_t *pchrom = (uint32_t *)INTEGER(rchrom);
    uint32_t *pstartpos = (uint32_t *)INTEGER(rstartpos);
    uint32_t *pendpos = (uint32_t *)INTEGER(rendpos);
    uint32_t *pstrand = (uint32_t *)INTEGER(rstrand);
    uint64_t *prk = (uint64_t *)REAL(rk);
    regionkey_t v = {0, 0, 0, 0};
    for(i = 0; i < n; i++)
    {
        v.chrom = 0;
        v.startpos = 0;
        v. endpos = 0;
        v.strand = 0;
        decode_regionkey(prk[i], &v);
        pchrom[i] = v.chrom;
        pstartpos[i] = v.startpos;
        pendpos[i] = v.endpos;
        pstrand[i] = v.strand;
    }
    const char *names[] = {"CHROM", "STARTPOS", "ENDPOS", "STRAND", ""};
    SEXP res = PROTECT(mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, rchrom);
    SET_VECTOR_ELT(res, 1, rstartpos);
    SET_VECTOR_ELT(res, 2, rendpos);
    SET_VECTOR_ELT(res, 3, rstrand);
    UNPROTECT(1);
    return res;
}

SEXP R_reverse_regionkey(SEXP rk, SEXP rchrom, SEXP rstartpos, SEXP rendpos, SEXP rstrand)
{
    uint64_t i, n = LENGTH(rk);
    uint32_t *pstartpos = (uint32_t *)INTEGER(rstartpos);
    uint32_t *pendpos = (uint32_t *)INTEGER(rendpos);
    int32_t *pstrand = (int32_t *)INTEGER(rstrand);
    uint64_t *prk = (uint64_t *)REAL(rk);
    regionkey_rev_t v = {0, 0, 0, 0};
    for(i = 0; i < n; i++)
    {
        v.chrom[0] = 0;
        v.startpos = 0;
        v. endpos = 0;
        v.strand = 0;
        reverse_regionkey(prk[i], &v);
        SET_STRING_ELT(rchrom, i, mkChar(v.chrom));
        pstartpos[i] = v.startpos;
        pendpos[i] = v.endpos;
        pstrand[i] = v.strand;
    }
    const char *names[] = {"CHROM", "STARTPOS", "ENDPOS", "STRAND", ""};
    SEXP res = PROTECT(mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, rchrom);
    SET_VECTOR_ELT(res, 1, rstartpos);
    SET_VECTOR_ELT(res, 2, rendpos);
    SET_VECTOR_ELT(res, 3, rstrand);
    UNPROTECT(1);
    return res;
}

SEXP R_regionkey(SEXP chrom, SEXP startpos, SEXP endpos, SEXP strand, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint64_t *res = (uint64_t *)REAL(ret);
    uint32_t *pstartpos = (uint32_t *)INTEGER(startpos);
    uint32_t *pendpos = (uint32_t *)INTEGER(endpos);
    int32_t *pstrand = (int32_t *)INTEGER(strand);
    for(i = 0; i < n; i++)
    {
        const char *chr = CHAR(STRING_ELT(chrom, i));
        res[i] = regionkey(chr, strlen(chr), pstartpos[i], pendpos[i], (int8_t)(pstrand[i]));
    }
    return ret;
}

SEXP R_regionkey_hex(SEXP rk, SEXP ret)
{
    return R_hex_uint64_t(rk, ret);
}

SEXP R_parse_regionkey_hex(SEXP hex, SEXP ret)
{
    return R_parse_hex_uint64_t(hex, ret);
}

SEXP R_get_regionkey_chrom_startpos(SEXP rk, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint64_t *res = (uint64_t *)REAL(ret);
    uint64_t *prk = (uint64_t *)REAL(rk);
    for(i = 0; i < n; i++)
    {
        res[i] = get_regionkey_chrom_startpos(prk[i]);
    }
    return ret;
}

SEXP R_get_regionkey_chrom_endpos(SEXP rk, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint64_t *res = (uint64_t *)REAL(ret);
    uint64_t *prk = (uint64_t *)REAL(rk);
    for(i = 0; i < n; i++)
    {
        res[i] = get_regionkey_chrom_endpos(prk[i]);
    }
    return ret;
}

SEXP R_are_overlapping_regions(SEXP a_chrom, SEXP a_startpos, SEXP a_endpos, SEXP b_chrom, SEXP b_startpos, SEXP b_endpos, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint32_t *res = (uint32_t *)INTEGER(ret);
    uint32_t *pachrom = (uint32_t *)INTEGER(a_chrom);
    uint32_t *pastartpos = (uint32_t *)INTEGER(a_startpos);
    uint32_t *paendpos = (uint32_t *)INTEGER(a_endpos);
    uint32_t *pbchrom = (uint32_t *)INTEGER(b_chrom);
    uint32_t *pbstartpos = (uint32_t *)INTEGER(b_startpos);
    uint32_t *pbendpos = (uint32_t *)INTEGER(b_endpos);
    for(i = 0; i < n; i++)
    {
        res[i] = are_overlapping_regions((uint8_t)(pachrom[i]), pastartpos[i], paendpos[i], (uint8_t)(pbchrom[i]), pbstartpos[i], pbendpos[i]);
    }
    return ret;
}

SEXP R_are_overlapping_region_regionkey(SEXP chrom, SEXP startpos, SEXP endpos, SEXP rk, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint32_t *res = (uint32_t *)INTEGER(ret);
    uint32_t *pchrom = (uint32_t *)INTEGER(chrom);
    uint32_t *pstartpos = (uint32_t *)INTEGER(startpos);
    uint32_t *pendpos = (uint32_t *)INTEGER(endpos);
    uint64_t *prk = (uint64_t *)REAL(rk);
    for(i = 0; i < n; i++)
    {
        res[i] = are_overlapping_region_regionkey((uint8_t)(pchrom[i]), pstartpos[i], pendpos[i], prk[i]);
    }
    return ret;
}

SEXP R_are_overlapping_regionkeys(SEXP rka, SEXP rkb, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint32_t *res = (uint32_t *)INTEGER(ret);
    uint64_t *prka = (uint64_t *)REAL(rka);
    uint64_t *prkb = (uint64_t *)REAL(rkb);
    for(i = 0; i < n; i++)
    {
        res[i] = are_overlapping_regionkeys(prka[i], prkb[i]);
    }
    return ret;
}

SEXP R_are_overlapping_variantkey_regionkey(SEXP mc, SEXP vk, SEXP rk, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint32_t *res = (uint32_t *)INTEGER(ret);
    uint64_t *pvk = (uint64_t *)REAL(vk);
    uint64_t *prk = (uint64_t *)REAL(rk);
    const nrvk_cols_t *cmc = get_nrvk_mc(mc);
    for(i = 0; i < n; i++)
    {
        res[i] = are_overlapping_variantkey_regionkey(*cmc, pvk[i], prk[i]);
    }
    return ret;
}

SEXP R_variantkey_to_regionkey(SEXP mc, SEXP vk, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint32_t *res = (uint32_t *)INTEGER(ret);
    uint64_t *pvk = (uint64_t *)REAL(vk);
    const nrvk_cols_t *cmc = get_nrvk_mc(mc);
    for(i = 0; i < n; i++)
    {
        res[i] = variantkey_to_regionkey(*cmc, pvk[i]);
    }
    return ret;
}

// --- ESID ---

SEXP R_encode_string_id(SEXP str, SEXP start, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    size_t pstart = (size_t)asInteger(start);
    uint64_t *res = (uint64_t *)REAL(ret);
    for(i = 0; i < n; i++)
    {
        const char *chr = CHAR(STRING_ELT(str, i));
        res[i] = encode_string_id(chr, strlen(chr), pstart);
    }
    return ret;
}

SEXP R_encode_string_num_id(SEXP str, SEXP sep, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint64_t *res = (uint64_t *)REAL(ret);
    uint8_t psep = (uint8_t)asInteger(sep);
    for(i = 0; i < n; i++)
    {
        const char *chr = CHAR(STRING_ELT(str, i));
        res[i] = encode_string_num_id(chr, strlen(chr), psep);
    }
    return ret;
}

SEXP R_decode_string_id(SEXP esid, SEXP ret)
{
    uint64_t i, n = LENGTH(esid);
    uint64_t *pesid = (uint64_t *)REAL(esid);
    char str[23] = "";
    for(i = 0; i < n; i++)
    {
        str[0] = 0;
        decode_string_id(pesid[i], str);
        SET_STRING_ELT(ret, i, mkChar(str));
    }
    return ret;
}

SEXP R_hash_string_id(SEXP str, SEXP ret)
{
    uint64_t i, n = LENGTH(ret);
    uint64_t *res = (uint64_t *)REAL(ret);
    for(i = 0; i < n; i++)
    {
        const char *chr = CHAR(STRING_ELT(str, i));
        res[i] = hash_string_id(chr, strlen(chr));
    }
    return ret;
}
