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
    decode_refalt(INTEGER(code)[0], ref, &sizeref, alt, &sizealt);
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
    char hex[17];
    uint64_t code = encode_variantkey(asInteger(chrom), asReal(pos), asReal(refalt));
    variantkey_hex(code, hex);
    return Rf_mkString(hex);
}

SEXP R_variantkey(SEXP chrom, SEXP pos, SEXP ref, SEXP alt)
{
    const char *chr = CHAR(STRING_ELT(chrom, 0));
    const char *r = CHAR(STRING_ELT(ref, 0));
    const char *a = CHAR(STRING_ELT(alt, 0));
    char hex[17];
    uint64_t code = variantkey(chr, strlen(chr), asReal(pos), r, strlen(r), a, strlen(a));
    variantkey_hex(code, hex);
    return Rf_mkString(hex);
}

SEXP R_variantkey_range(SEXP chrom, SEXP pos_min, SEXP pos_max)
{
    char vk_min[17];
    char vk_max[17];
    vkrange_t r = {0};
    variantkey_range(asInteger(chrom), asReal(pos_min), asReal(pos_max), &r);
    variantkey_hex(r.min, vk_min);
    variantkey_hex(r.max, vk_max);
    const char *names[] = {"MIN", "MAX", ""};
    SEXP res = PROTECT(mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, Rf_mkString(vk_min));
    SET_VECTOR_ELT(res, 1, Rf_mkString(vk_max));
    UNPROTECT(1);
    return res;
}

SEXP R_reverse_variantkey(SEXP hexcode)
{
    char chrom[3] = "";
    char ref[12] = "", alt[12] = "";
    size_t sizeref = 0, sizealt = 0;
    const char *hex = CHAR(STRING_ELT(hexcode, 0));
    uint64_t code = parse_variantkey_hex(hex);
    variantkey_t v = {0};
    decode_variantkey(code, &v);
    decode_chrom(v.chrom, chrom);
    decode_refalt(v.refalt, ref, &sizeref, alt, &sizealt);
    const char *names[] = {"CHROM", "POS", "REF", "ALT", "SIZE_REF", "SIZE_ALT", ""};
    SEXP res = PROTECT(mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, Rf_mkString(chrom));
    SET_VECTOR_ELT(res, 1, ScalarInteger(v.pos));
    SET_VECTOR_ELT(res, 2, Rf_mkString(ref));
    SET_VECTOR_ELT(res, 3, Rf_mkString(alt));
    SET_VECTOR_ELT(res, 4, ScalarInteger(sizeref));
    SET_VECTOR_ELT(res, 5, ScalarInteger(sizealt));
    UNPROTECT(1);
    return res;
}
