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
#include "../../../c/src/binsearch.c"
#include "../../../c/src/esid.c"
#include "../../../c/src/genoref.c"
#include "../../../c/src/hex.c"
#include "../../../c/src/nrvk.c"
#include "../../../c/src/regionkey.c"
#include "../../../c/src/rsidvar.c"
#include "../../../c/src/variantkey.c"

SEXP R_encode_chrom(SEXP chrom)
{
    const char* chr = CHAR(STRING_ELT(chrom, 0));
    SEXP res;
    PROTECT(res = NEW_INTEGER(1));
    INTEGER(res)[0] = encode_chrom(chr, strlen(chr));
    UNPROTECT(1);
    return res;
}

SEXP R_decode_chrom(SEXP code)
{
    char chrom[3] = "";
    decode_chrom(asInteger(code), chrom);
    return Rf_mkString(chrom);
}

SEXP R_encode_refalt(SEXP ref, SEXP alt)
{
    const char* r = CHAR(STRING_ELT(ref, 0));
    const char* a = CHAR(STRING_ELT(alt, 0));
    SEXP res;
    PROTECT(res = NEW_INTEGER(1));
    INTEGER(res)[0] = encode_refalt(r, strlen(r), a, strlen(a));
    UNPROTECT(1);
    return res;
}

SEXP R_decode_refalt(SEXP code)
{
    char ref[12] = "", alt[12] = "";
    size_t sizeref = 0, sizealt = 0;
    decode_refalt(asInteger(code), ref, &sizeref, alt, &sizealt);
    const char *names[] = {"REF", "ALT", "REF_LEN", "ALT_LEN", ""};
    SEXP res = PROTECT(mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, Rf_mkString(ref));
    SET_VECTOR_ELT(res, 1, Rf_mkString(alt));
    SET_VECTOR_ELT(res, 2, ScalarInteger(sizeref));
    SET_VECTOR_ELT(res, 3, ScalarInteger(sizealt));
    UNPROTECT(1);
    return res;
}

SEXP R_encode_variantkey(SEXP chrom, SEXP pos, SEXP refalt)
{
    uint64_t code = encode_variantkey(asInteger(chrom), asInteger(pos), asInteger(refalt));
    char hex[17];
    variantkey_hex(code, hex);
    return Rf_mkString(hex);
}

SEXP R_extract_variantkey_chrom(SEXP vk)
{
    uint64_t code = parse_variantkey_hex(CHAR(STRING_ELT(vk, 0)));
    SEXP res;
    PROTECT(res = NEW_INTEGER(1));
    INTEGER(res)[0] = extract_variantkey_chrom(code);
    UNPROTECT(1);
    return res;
}

SEXP R_extract_variantkey_pos(SEXP vk)
{
    uint64_t code = parse_variantkey_hex(CHAR(STRING_ELT(vk, 0)));
    SEXP res;
    PROTECT(res = NEW_INTEGER(1));
    INTEGER(res)[0] = extract_variantkey_pos(code);
    UNPROTECT(1);
    return res;
}

SEXP R_extract_variantkey_refalt(SEXP vk)
{
    uint64_t code = parse_variantkey_hex(CHAR(STRING_ELT(vk, 0)));
    SEXP res;
    PROTECT(res = NEW_INTEGER(1));
    INTEGER(res)[0] = extract_variantkey_refalt(code);
    UNPROTECT(1);
    return res;
}

SEXP R_decode_variantkey(SEXP vk)
{
    uint64_t code = parse_variantkey_hex(CHAR(STRING_ELT(vk, 0)));
    variantkey_t v = {0};
    decode_variantkey(code, &v);
    const char *names[] = {"CHROM", "POS", "REFALT", ""};
    SEXP res = PROTECT(mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, ScalarInteger(v.chrom));
    SET_VECTOR_ELT(res, 1, ScalarInteger(v.pos));
    SET_VECTOR_ELT(res, 2, ScalarInteger(v.refalt));
    UNPROTECT(1);
    return res;
}

SEXP R_variantkey(SEXP chrom, SEXP pos, SEXP ref, SEXP alt)
{
    const char *chr = CHAR(STRING_ELT(chrom, 0));
    const char *r = CHAR(STRING_ELT(ref, 0));
    const char *a = CHAR(STRING_ELT(alt, 0));
    uint64_t code = variantkey(chr, strlen(chr), asInteger(pos), r, strlen(r), a, strlen(a));
    char hex[17];
    variantkey_hex(code, hex);
    return Rf_mkString(hex);
}

SEXP R_variantkey_range(SEXP chrom, SEXP pos_min, SEXP pos_max)
{
    vkrange_t r = {0};
    variantkey_range(asInteger(chrom), asInteger(pos_min), asInteger(pos_max), &r);
    char vk_min[17];
    char vk_max[17];
    variantkey_hex(r.min, vk_min);
    variantkey_hex(r.max, vk_max);
    const char *names[] = {"MIN", "MAX", ""};
    SEXP res = PROTECT(mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, Rf_mkString(vk_min));
    SET_VECTOR_ELT(res, 1, Rf_mkString(vk_max));
    UNPROTECT(1);
    return res;
}

SEXP R_compare_variantkey_chrom(SEXP vka, SEXP vkb)
{
    uint64_t acode = parse_variantkey_hex(CHAR(STRING_ELT(vka, 0)));
    uint64_t bcode = parse_variantkey_hex(CHAR(STRING_ELT(vkb, 0)));
    SEXP res;
    PROTECT(res = NEW_INTEGER(1));
    INTEGER(res)[0] = compare_variantkey_chrom(acode, bcode);
    UNPROTECT(1);
    return res;
}

SEXP R_compare_variantkey_chrom_pos(SEXP vka, SEXP vkb)
{
    uint64_t acode = parse_variantkey_hex(CHAR(STRING_ELT(vka, 0)));
    uint64_t bcode = parse_variantkey_hex(CHAR(STRING_ELT(vkb, 0)));
    SEXP res;
    PROTECT(res = NEW_INTEGER(1));
    INTEGER(res)[0] = compare_variantkey_chrom_pos(acode, bcode);
    UNPROTECT(1);
    return res;
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
    /*int *px = INTEGER(ctbytes);
    for (int i = 0; i < mf->ncols; i++)
    {
        mf->ctbytes[i] = (uint8_t)px[i];
    }*/
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
    /*int *px = INTEGER(ctbytes);
    for (int i = 0; i < mf->ncols; i++)
    {
        mf->ctbytes[i] = (uint8_t)px[i];
    }*/
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

SEXP R_find_rv_variantkey_by_rsid(SEXP mc, SEXP first, SEXP last, SEXP rsid)
{
    uint64_t pfirst = asInteger(first);
    const rsidvar_cols_t *cmc = get_rsidvar_mc(mc);
    uint64_t vk = find_rv_variantkey_by_rsid(*cmc, &pfirst, asInteger(last), asInteger(rsid));
    char hex[17];
    variantkey_hex(vk, hex);
    const char *names[] = {"VK", "FIRST", ""};
    SEXP res = PROTECT(mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, Rf_mkString(hex));
    SET_VECTOR_ELT(res, 1, ScalarInteger(pfirst));
    UNPROTECT(1);
    return res;
}

SEXP R_get_next_rv_variantkey_by_rsid(SEXP mc, SEXP pos, SEXP last, SEXP rsid)
{
    uint64_t ppos = asInteger(pos);
    const rsidvar_cols_t *cmc = get_rsidvar_mc(mc);
    uint64_t vk = get_next_rv_variantkey_by_rsid(*cmc, &ppos, asInteger(last), asInteger(rsid));
    char hex[17];
    variantkey_hex(vk, hex);
    const char *names[] = {"VK", "POS", ""};
    SEXP res = PROTECT(mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, Rf_mkString(hex));
    SET_VECTOR_ELT(res, 1, ScalarInteger(ppos));
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

SEXP R_find_vr_rsid_by_variantkey(SEXP mc, SEXP first, SEXP last, SEXP vk)
{
    uint64_t varkey = parse_variantkey_hex(CHAR(STRING_ELT(vk, 0)));
    uint64_t pfirst = asInteger(first);
    const rsidvar_cols_t *cmc = get_rsidvar_mc(mc);
    uint32_t rsid = find_vr_rsid_by_variantkey(*cmc, &pfirst, asInteger(last), varkey);
    const char *names[] = {"RSID", "FIRST", ""};
    SEXP res = PROTECT(mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, ScalarInteger(rsid));
    SET_VECTOR_ELT(res, 1, ScalarInteger(pfirst));
    UNPROTECT(1);
    return res;
}

SEXP R_find_vr_chrompos_range(SEXP mc, SEXP first, SEXP last, SEXP chrom, SEXP pos_min, SEXP pos_max)
{
    uint64_t pfirst = asInteger(first);
    uint64_t plast = asInteger(last);
    const rsidvar_cols_t *cmc = get_rsidvar_mc(mc);
    uint32_t rsid = find_vr_chrompos_range(*cmc, &pfirst, &plast, asInteger(chrom), asInteger(pos_min), asInteger(pos_max));
    const char *names[] = {"RSID", "FIRST", "LAST", ""};
    SEXP res = PROTECT(mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, ScalarInteger(rsid));
    SET_VECTOR_ELT(res, 1, ScalarInteger(pfirst));
    SET_VECTOR_ELT(res, 2, ScalarInteger(plast));
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

SEXP R_find_ref_alt_by_variantkey(SEXP mc, SEXP vk)
{
    char ref[ALLELE_MAXSIZE] = "", alt[ALLELE_MAXSIZE] = "";
    size_t sizeref = 0, sizealt = 0;
    uint64_t varkey = parse_variantkey_hex(CHAR(STRING_ELT(vk, 0)));
    const nrvk_cols_t *cmc = get_nrvk_mc(mc);
    size_t len = find_ref_alt_by_variantkey(*cmc, varkey, ref, &sizeref, alt, &sizealt);
    const char *names[] = {"REF", "ALT", "REF_LEN", "ALT_LEN", "LEN", ""};
    SEXP res = PROTECT(mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, Rf_mkString(ref));
    SET_VECTOR_ELT(res, 1, Rf_mkString(alt));
    SET_VECTOR_ELT(res, 2, ScalarInteger(sizeref));
    SET_VECTOR_ELT(res, 3, ScalarInteger(sizealt));
    SET_VECTOR_ELT(res, 4, ScalarInteger(len));
    UNPROTECT(1);
    return res;
}

SEXP R_reverse_variantkey(SEXP mc, SEXP vk)
{
    uint64_t varkey = parse_variantkey_hex(CHAR(STRING_ELT(vk, 0)));
    variantkey_rev_t rev = {0};
    const nrvk_cols_t *cmc = get_nrvk_mc(mc);
    size_t len = reverse_variantkey(*cmc, varkey, &rev);
    const char *names[] = {"CHROM", "POS", "REF", "ALT", "REF_LEN", "ALT_LEN", "LEN", ""};
    SEXP res = PROTECT(mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, Rf_mkString(rev.chrom));
    SET_VECTOR_ELT(res, 1, ScalarInteger(rev.pos));
    SET_VECTOR_ELT(res, 2, Rf_mkString(rev.ref));
    SET_VECTOR_ELT(res, 3, Rf_mkString(rev.alt));
    SET_VECTOR_ELT(res, 4, ScalarInteger(rev.sizeref));
    SET_VECTOR_ELT(res, 5, ScalarInteger(rev.sizealt));
    SET_VECTOR_ELT(res, 6, ScalarInteger(len));
    UNPROTECT(1);
    return res;
}

SEXP R_get_variantkey_ref_length(SEXP mc, SEXP vk)
{
    uint64_t varkey = parse_variantkey_hex(CHAR(STRING_ELT(vk, 0)));
    SEXP res;
    PROTECT(res = NEW_INTEGER(1));
    const nrvk_cols_t *cmc = get_nrvk_mc(mc);
    INTEGER(res)[0] = get_variantkey_ref_length(*cmc, varkey);
    UNPROTECT(1);
    return res;
}

SEXP R_get_variantkey_endpos(SEXP mc, SEXP vk)
{
    uint64_t varkey = parse_variantkey_hex(CHAR(STRING_ELT(vk, 0)));
    SEXP res;
    PROTECT(res = NEW_INTEGER(1));
    const nrvk_cols_t *cmc = get_nrvk_mc(mc);
    INTEGER(res)[0] = get_variantkey_endpos(*cmc, varkey);
    UNPROTECT(1);
    return res;
}

SEXP R_get_variantkey_chrom_startpos(SEXP vk)
{
    uint64_t varkey = parse_variantkey_hex(CHAR(STRING_ELT(vk, 0)));
    uint64_t res = get_variantkey_chrom_startpos(varkey);
    char hex[17];
    hex_uint64_t(res, hex);
    return Rf_mkString(hex);
}

SEXP R_get_variantkey_chrom_endpos(SEXP mc, SEXP vk)
{
    uint64_t varkey = parse_variantkey_hex(CHAR(STRING_ELT(vk, 0)));
    const nrvk_cols_t *cmc = get_nrvk_mc(mc);
    uint64_t res = get_variantkey_chrom_endpos(*cmc, varkey);
    char hex[17];
    hex_uint64_t(res, hex);
    return Rf_mkString(hex);
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

SEXP R_get_genoref_seq(SEXP mf, SEXP chrom, SEXP pos)
{
    SEXP res;
    PROTECT(res = NEW_INTEGER(1));
    const mmfile_t *cmf = get_mmfile_mf(mf);
    INTEGER(res)[0] = get_genoref_seq(*cmf, asInteger(chrom), asInteger(pos));
    UNPROTECT(1);
    return res;
}

SEXP R_check_reference(SEXP mf, SEXP chrom, SEXP pos, SEXP ref)
{
    const char *pref = CHAR(STRING_ELT(ref, 0));
    SEXP res;
    PROTECT(res = NEW_INTEGER(1));
    const mmfile_t *cmf = get_mmfile_mf(mf);
    INTEGER(res)[0] = check_reference(*cmf, asInteger(chrom), asInteger(pos), pref, strlen(pref));
    UNPROTECT(1);
    return res;
}

SEXP R_flip_allele(SEXP allele)
{
    char *s = Rf_acopy_string(CHAR(STRING_ELT(allele, 0)));
    flip_allele(s, strlen(s));
    return Rf_mkString(s);
}

SEXP R_normalize_variant(SEXP mf, SEXP chrom, SEXP pos, SEXP ref, SEXP alt)
{
    const char *cpref = CHAR(STRING_ELT(ref, 0));
    const char *cpalt = CHAR(STRING_ELT(alt, 0));
    size_t sizeref = strlen(cpref);
    size_t sizealt = strlen(cpalt);
    char pref[ALLELE_MAXSIZE] = "";
    char palt[ALLELE_MAXSIZE] = "";
    strncpy(pref, cpref, sizeref);
    strncpy(palt, cpalt, sizealt);
    pref[sizeref] = 0;
    palt[sizealt] = 0;
    uint32_t ppos = asInteger(pos);
    const mmfile_t *cmf = get_mmfile_mf(mf);
    int ret = normalize_variant(*cmf, asInteger(chrom), &ppos, pref, &sizeref, palt, &sizealt);
    const char *names[] = {"RET", "POS", "REF", "ALT", "REF_LEN", "ALT_LEN", ""};
    SEXP res = PROTECT(mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, ScalarInteger(ret));
    SET_VECTOR_ELT(res, 1, ScalarInteger(ppos));
    SET_VECTOR_ELT(res, 2, Rf_mkString(pref));
    SET_VECTOR_ELT(res, 3, Rf_mkString(palt));
    SET_VECTOR_ELT(res, 4, ScalarInteger(sizeref));
    SET_VECTOR_ELT(res, 5, ScalarInteger(sizealt));
    UNPROTECT(1);
    return res;
}

// --- REGIONKEY ---

SEXP R_encode_region_strand(SEXP strand)
{
    SEXP res;
    PROTECT(res = NEW_INTEGER(1));
    INTEGER(res)[0] = encode_region_strand(asInteger(strand));
    UNPROTECT(1);
    return res;
}

SEXP R_decode_region_strand(SEXP strand)
{
    SEXP res;
    PROTECT(res = NEW_INTEGER(1));
    INTEGER(res)[0] = decode_region_strand(asInteger(strand));
    UNPROTECT(1);
    return res;
}

SEXP R_encode_regionkey(SEXP chrom, SEXP startpos, SEXP endpos, SEXP strand)
{
    uint64_t code = encode_regionkey(asInteger(chrom), asInteger(startpos), asInteger(endpos), asInteger(strand));
    char hex[17];
    regionkey_hex(code, hex);
    return Rf_mkString(hex);
}

SEXP R_extract_regionkey_chrom(SEXP rk)
{
    uint64_t code = parse_regionkey_hex(CHAR(STRING_ELT(rk, 0)));
    SEXP res;
    PROTECT(res = NEW_INTEGER(1));
    INTEGER(res)[0] = extract_regionkey_chrom(code);
    UNPROTECT(1);
    return res;
}

SEXP R_extract_regionkey_startpos(SEXP rk)
{
    uint64_t code = parse_regionkey_hex(CHAR(STRING_ELT(rk, 0)));
    SEXP res;
    PROTECT(res = NEW_INTEGER(1));
    INTEGER(res)[0] = extract_regionkey_startpos(code);
    UNPROTECT(1);
    return res;
}

SEXP R_extract_regionkey_endpos(SEXP rk)
{
    uint64_t code = parse_regionkey_hex(CHAR(STRING_ELT(rk, 0)));
    SEXP res;
    PROTECT(res = NEW_INTEGER(1));
    INTEGER(res)[0] = extract_regionkey_endpos(code);
    UNPROTECT(1);
    return res;
}

SEXP R_extract_regionkey_strand(SEXP rk)
{
    uint64_t code = parse_regionkey_hex(CHAR(STRING_ELT(rk, 0)));
    SEXP res;
    PROTECT(res = NEW_INTEGER(1));
    INTEGER(res)[0] = extract_regionkey_strand(code);
    UNPROTECT(1);
    return res;
}

SEXP R_decode_regionkey(SEXP rk)
{
    uint64_t code = parse_regionkey_hex(CHAR(STRING_ELT(rk, 0)));
    regionkey_t v = {0};
    decode_regionkey(code, &v);
    const char *names[] = {"CHROM", "STARTPOS", "ENDPOS", "STRAND", ""};
    SEXP res = PROTECT(mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, ScalarInteger(v.chrom));
    SET_VECTOR_ELT(res, 1, ScalarInteger(v.startpos));
    SET_VECTOR_ELT(res, 2, ScalarInteger(v.endpos));
    SET_VECTOR_ELT(res, 3, ScalarInteger(v.strand));
    UNPROTECT(1);
    return res;
}

SEXP R_reverse_regionkey(SEXP rk)
{
    uint64_t code = parse_regionkey_hex(CHAR(STRING_ELT(rk, 0)));
    regionkey_rev_t v = {0};
    reverse_regionkey(code, &v);
    const char *names[] = {"CHROM", "STARTPOS", "ENDPOS", "STRAND", ""};
    SEXP res = PROTECT(mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, Rf_mkString(v.chrom));
    SET_VECTOR_ELT(res, 1, ScalarInteger(v.startpos));
    SET_VECTOR_ELT(res, 2, ScalarInteger(v.endpos));
    SET_VECTOR_ELT(res, 3, ScalarInteger(v.strand));
    UNPROTECT(1);
    return res;
}

SEXP R_regionkey(SEXP chrom, SEXP startpos, SEXP endpos, SEXP strand)
{
    const char *chr = CHAR(STRING_ELT(chrom, 0));
    uint64_t code = regionkey(chr, strlen(chr), asInteger(startpos), asInteger(endpos), asInteger(strand));
    char hex[17];
    regionkey_hex(code, hex);
    return Rf_mkString(hex);
}

SEXP R_get_regionkey_chrom_startpos(SEXP rk)
{
    uint64_t code = parse_regionkey_hex(CHAR(STRING_ELT(rk, 0)));
    uint64_t cp = get_regionkey_chrom_startpos(code);
    char hex[17];
    regionkey_hex(cp, hex);
    return Rf_mkString(hex);
}

SEXP R_get_regionkey_chrom_endpos(SEXP rk)
{
    uint64_t code = parse_regionkey_hex(CHAR(STRING_ELT(rk, 0)));
    uint64_t cp = get_regionkey_chrom_endpos(code);
    char hex[17];
    regionkey_hex(cp, hex);
    return Rf_mkString(hex);
}

SEXP R_are_overlapping_regions(SEXP a_chrom, SEXP a_startpos, SEXP a_endpos, SEXP b_chrom, SEXP b_startpos, SEXP b_endpos)
{
    SEXP res;
    PROTECT(res = NEW_INTEGER(1));
    INTEGER(res)[0] = are_overlapping_regions(asInteger(a_chrom), asInteger(a_startpos), asInteger(a_endpos), asInteger(b_chrom), asInteger(b_startpos), asInteger(b_endpos));
    UNPROTECT(1);
    return res;
}

SEXP R_are_overlapping_region_regionkey(SEXP chrom, SEXP startpos, SEXP endpos, SEXP rk)
{
    uint64_t rkcode = parse_regionkey_hex(CHAR(STRING_ELT(rk, 0)));
    SEXP res;
    PROTECT(res = NEW_INTEGER(1));
    INTEGER(res)[0] = are_overlapping_region_regionkey(asInteger(chrom), asInteger(startpos), asInteger(endpos), rkcode);
    UNPROTECT(1);
    return res;
}

SEXP R_are_overlapping_regionkeys(SEXP rka, SEXP rkb)
{
    uint64_t rkacode = parse_regionkey_hex(CHAR(STRING_ELT(rka, 0)));
    uint64_t rkbcode = parse_regionkey_hex(CHAR(STRING_ELT(rkb, 0)));
    SEXP res;
    PROTECT(res = NEW_INTEGER(1));
    INTEGER(res)[0] = are_overlapping_regionkeys(rkacode, rkbcode);
    UNPROTECT(1);
    return res;
}

SEXP R_are_overlapping_variantkey_regionkey(SEXP mc, SEXP vk, SEXP rk)
{
    uint64_t vkcode = parse_regionkey_hex(CHAR(STRING_ELT(vk, 0)));
    uint64_t rkcode = parse_regionkey_hex(CHAR(STRING_ELT(rk, 0)));
    SEXP res;
    PROTECT(res = NEW_INTEGER(1));
    const nrvk_cols_t *cmc = get_nrvk_mc(mc);
    INTEGER(res)[0] = are_overlapping_variantkey_regionkey(*cmc, vkcode, rkcode);
    UNPROTECT(1);
    return res;
}

SEXP R_variantkey_to_regionkey(SEXP mc, SEXP vk)
{
    uint64_t vkcode = parse_regionkey_hex(CHAR(STRING_ELT(vk, 0)));
    const nrvk_cols_t *cmc = get_nrvk_mc(mc);
    uint64_t rk = variantkey_to_regionkey(*cmc, vkcode);
    char hex[17];
    regionkey_hex(rk, hex);
    return Rf_mkString(hex);
}

// --- ESID ---

SEXP R_encode_string_id(SEXP str, SEXP start)
{
    const char *chr = CHAR(STRING_ELT(str, 0));
    uint64_t esid = encode_string_id(chr, strlen(chr), asInteger(start));
    char hex[17];
    hex_uint64_t(esid, hex);
    return Rf_mkString(hex);
}

SEXP R_encode_string_num_id(SEXP str, SEXP sep)
{
    const char *chr = CHAR(STRING_ELT(str, 0));
    uint64_t esid = encode_string_num_id(chr, strlen(chr), asInteger(sep));
    char hex[17];
    hex_uint64_t(esid, hex);
    return Rf_mkString(hex);
}

SEXP R_decode_string_id(SEXP esid)
{
    uint64_t code = parse_hex_uint64_t(CHAR(STRING_ELT(esid, 0)));
    char str[23] = "";
    decode_string_id(code, str);
    return Rf_mkString(str);
}

SEXP R_hash_string_id(SEXP str)
{
    const char *chr = CHAR(STRING_ELT(str, 0));
    uint64_t hsid = hash_string_id(chr, strlen(chr));
    char hex[17];
    hex_uint64_t(hsid, hex);
    return Rf_mkString(hex);
}
