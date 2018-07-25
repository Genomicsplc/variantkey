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
//
// VariantKey by Nicola Asuni

#include <R.h>
#include <Rdefines.h>
#include "../../../c/src/astring.c"
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
