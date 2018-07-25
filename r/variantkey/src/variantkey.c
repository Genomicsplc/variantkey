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
    SEXP res;
    PROTECT(res = NEW_NUMERIC(1));
    REAL(res)[0] = encode_variantkey(asInteger(chrom), asInteger(pos), asInteger(refalt));
    UNPROTECT(1);
    return res;
}

SEXP R_extract_variantkey_chrom(SEXP vk)
{
    SEXP res;
    PROTECT(res = NEW_INTEGER(1));
    INTEGER(res)[0] = extract_variantkey_chrom(asReal(vk));
    UNPROTECT(1);
    return res;
}

SEXP R_extract_variantkey_pos(SEXP vk)
{
    SEXP res;
    PROTECT(res = NEW_INTEGER(1));
    INTEGER(res)[0] = extract_variantkey_pos(asReal(vk));
    UNPROTECT(1);
    return res;
}

SEXP R_extract_variantkey_refalt(SEXP vk)
{
    SEXP res;
    PROTECT(res = NEW_INTEGER(1));
    INTEGER(res)[0] = extract_variantkey_refalt(asReal(vk));
    UNPROTECT(1);
    return res;
}

SEXP R_decode_variantkey(SEXP vk)
{
    variantkey_t v = {0};
    decode_variantkey(asReal(vk), &v);
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
    SEXP res;
    PROTECT(res = NEW_NUMERIC(1));
    REAL(res)[0] = variantkey(chr, strlen(chr), asInteger(pos), r, strlen(r), a, strlen(a));
    UNPROTECT(1);
    return res;
}

SEXP R_variantkey_range(SEXP chrom, SEXP pos_min, SEXP pos_max)
{
    vkrange_t r = {0};
    variantkey_range(asInteger(chrom), asInteger(pos_min), asInteger(pos_max), &r);
    const char *names[] = {"MIN", "MAX", ""};
    SEXP res = PROTECT(mkNamed(VECSXP, names));
    SET_VECTOR_ELT(res, 0, ScalarReal(r.min));
    SET_VECTOR_ELT(res, 1, ScalarReal(r.max));
    UNPROTECT(1);
    return res;
}

SEXP R_compare_variantkey_chrom(SEXP vka, SEXP vkb)
{
    SEXP res;
    PROTECT(res = NEW_INTEGER(1));
    INTEGER(res)[0] = compare_variantkey_chrom(asReal(vka), asReal(vkb));
    UNPROTECT(1);
    return res;
}

SEXP R_compare_variantkey_chrom_pos(SEXP vka, SEXP vkb)
{
    SEXP res;
    PROTECT(res = NEW_INTEGER(1));
    INTEGER(res)[0] = compare_variantkey_chrom_pos(asReal(vka), asReal(vkb));
    UNPROTECT(1);
    return res;
}

SEXP R_variantkey_hex(SEXP vk)
{
    char vs[17] = "";
    variantkey_hex(asReal(vk), vs);
    return Rf_mkString(vs);
}

SEXP R_parse_variantkey_hex(SEXP vs)
{
    const char* vkstr = CHAR(STRING_ELT(vs, 0));
    SEXP res;
    PROTECT(res = NEW_NUMERIC(1));
    REAL(res)[0] = parse_variantkey_hex(vkstr);
    UNPROTECT(1);
    return res;
}
