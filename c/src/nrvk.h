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
 * @file nrvk.h
 * @brief Functions to retrieve REF and ALT values by VariantKey from binary data file.
 *
 * The functions provided here allows to retrieve the REF and ALT strings for a
 * given VariantKey from a binary file.
 *
 * The input binary files can be generated from a normalized VCF file using the `resources/tools/vkhexbin.sh`.
 * The VCF file can be normalized using the `resources/tools/vcfnorm.sh` script.
 *
 * The input binary file has the following format :
 *     [8 BYTE VARIANTKEY][8 BYTE VALUE ADDRESS]...
 *     [1 BYTE REF LENGTH][1 BYTE ALT LENGTH][REF STRING][ALT STRING]...
 *     [4 BYTE NUM VARIANTS]
 */

#ifndef NRVK_H
#define NRVK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "binsearch.h"
#include "variantkey.h"

#define BINBLKLEN 16 //!< Length in bytes of a binary block containing VARIANTKEY + OFFSET ADDRESS

/**
 * Retrieve the REF and ALT strings for the specified VariantKey.
 *
 * @param src      Memory mapped input file address.
 * @param last     Number of variants in the file -1.
 * @param vk       VariantKey to search.
 * @param ref      REF string buffer to be returned.
 * @param sizeref  Pointer to the size of the ref buffer, excluding the terminating null byte.
 *                 This will contain the final ref size.
 * @param alt      ALT string buffer to be returned.
 * @param sizealt  Pointer to the size of the alt buffer, excluding the terminating null byte.
 *                 This will contain the final alt size.
 *
 * @return REF+ALT length or 0 if the VariantKey is not found.
 */
size_t find_ref_alt_by_variantkey(const unsigned char *src, uint64_t last, uint64_t vk, char *ref, size_t *sizeref, char *alt, size_t *sizealt);

#ifdef __cplusplus
}
#endif

#endif  // NRVK_H
