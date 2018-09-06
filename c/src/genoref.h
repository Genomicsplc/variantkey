// VariantKey
//
// genoref.h
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
 * @file genoref.h
 * @brief Functions to retrieve genome reference sequences from a binary FASTA file.
 *
 * The functions provided here allows to retrieve genome reference sequences from a binary
 * version of a genome reference FASTA file.
 *
 * The input reference binary files can be generated from a FASTA file using the
 * `resources/tools/fastabin.sh` script.
 */

#ifndef GENOREF_H
#define GENOREF_H

#include <stdio.h>
#include <string.h>
#include "binsearch.h"

#ifndef ALLELE_MAXSIZE
#define ALLELE_MAXSIZE 256 //!< Maximum allele length.
#endif

// Return codes for normalize_variant()
#define NORM_WRONGPOS   (-2) //!< Normalization: Invalid position.
#define NORM_INVALID    (-1) //!< Normalization: Invalid reference.
#define NORM_OK          (0) //!< Normalization: The reference allele perfectly match the genome reference.
#define NORM_VALID       (1) //!< Normalization: The reference allele is inconsistent with the genome reference (i.e. when contains nucleotide letters other than A, C, G and T).
#define NORM_SWAP   (1 << 1) //!< Normalization: The alleles have been swapped.
#define NORM_FLIP   (1 << 2) //!< Normalization: The alleles nucleotides have been flipped (each nucleotide have been replaced with its complement).
#define NORM_LEXT   (1 << 3) //!< Normalization: Alleles have been left extended.
#define NORM_RTRIM  (1 << 4) //!< Normalization: Alleles have been right trimmed.
#define NORM_LTRIM  (1 << 5) //!< Normalization: Alleles have been left trimmed.

/**
 * Memory map the genoref binary file.
 *
 * @param file  Path to the file to map.
 * @param mf    Structure containing the memory mapped file.
 *
 * @return Returns the memory-mapped file descriptors.
 */
static void mmap_genoref_file(const char *file, mmfile_t *mf)
{
    mmap_binfile(file, mf);
    mf->index[26] = mf->size;
    int i = 25;
    while (i > 0)
    {
        mf->index[i] = mf->index[(i - 1)];
        i--;
    }
    mf->ncols = 27;
}

/**
 * Returns the uppercase version of the input character.
 * Note that this is safe to be used only with a-z characters.
 * All characters above 'a' will be changed.
 *
 * @param c Character to uppercase.
 *
 * @return Uppercased character
 */
static inline int aztoupper(int c)
{
    if (c >= 'a')
    {
        return (c ^ ('a' - 'A'));
    }
    return c;
}

/**
 * Prepend a character to a string.
 *
 * @param pre    Character to prepend.
 * @param string String to modify.
 * @param size   Input string length.
 *
 * @return void
 */
static void prepend_char(const uint8_t pre, char *string, size_t *size)
{
    memmove(string + 1, string, (*size + 1));
    string[0] = pre;
    (*size)++;
}

/**
 * Returns the genome reference nucleotide at the specified chromosome and position.
 *
 * @param mf      Structure containing the memory mapped file.
 * @param chrom   Encoded Chromosome number (see encode_chrom).
 * @param pos     Position. The reference position, with the first base having position 0.
 *
 * @return The nucleotide letter or 0 in case of invalid position.
 */
static char get_genoref_seq(mmfile_t mf, uint8_t chrom, uint32_t pos)
{
    uint64_t offset = (mf.index[chrom] + pos);
    if (offset >= mf.index[(chrom + 1)])
    {
        return 0; // invalid position
    }
    return *(mf.src + offset);
}

/**
 * Check if the reference allele matches the reference genome data.
 *
 * @param mf      Structure containing the memory mapped file.
 * @param chrom   Encoded Chromosome number (see encode_chrom).
 * @param pos     Position. The reference position, with the first base having position 0.
 * @param ref     Reference allele. String containing a sequence of nucleotide letters.
 * @param sizeref Length of the ref string, excluding the terminating null byte.
 *
 * @return Positive number in case of success, negative in case of error:
 *       *  0 the reference allele match the reference genome;
 *       *  1 the reference allele is inconsistent with the genome reference (i.e. when contains nucleotide letters other than A, C, G and T);
 *       * -1 the reference allele don't match the reference genome;
 *       * -2 the reference allele is longer than the genome reference sequence.
 */
static int check_reference(mmfile_t mf, uint8_t chrom, uint32_t pos, const char *ref, size_t sizeref)
{
    uint64_t offset = (mf.index[chrom] + pos);
    if ((offset + sizeref - 1) >= mf.index[(chrom + 1)])
    {
        return NORM_WRONGPOS;
    }
    size_t i;
    char uref, gref;
    int ret = 0; // return value
    for (i = 0; i < sizeref; i++)
    {
        uref = aztoupper(ref[i]);
        gref = mf.src[(offset + i)];
        if (uref == gref)
        {
            continue;
        }
        /*
            Abbreviation codes for degenerate bases

            Cornish-Bowden A.
            Nomenclature for incompletely specified bases in nucleic acid sequences: recommendations 1984.
            Nucleic Acids Research. 1985;13(9):3021-3030.

            SYMBOL | DESCRIPTION                   | BASES   | COMPLEMENT
            -------+-------------------------------+---------+-----------
               A   | Adenine                       | A       |  T
               C   | Cytosine                      |   C     |  G
               G   | Guanine                       |     G   |  C
               T   | Thymine                       |       T |  A
               W   | Weak                          | A     T |  W
               S   | Strong                        |   C G   |  S
               M   | aMino                         | A C     |  K
               K   | Keto                          |     G T |  M
               R   | puRine                        | A   G   |  Y
               Y   | pYrimidine                    |   C   T |  R
               B   | not A (B comes after A)       |   C G T |  V
               D   | not C (D comes after C)       | A   G T |  H
               H   | not G (H comes after G)       | A C   T |  D
               V   | not T (V comes after T and U) | A C G   |  B
               N   | aNy base (not a gap)          | A C G T |  N
            -------+-------------------------------+---------+----------
        */
        if ((uref == 'N')
                || (gref == 'N')
                || ((uref == 'B') && (gref != 'A'))
                || ((gref == 'B') && (uref != 'A'))
                || ((uref == 'D') && (gref != 'C'))
                || ((gref == 'D') && (uref != 'C'))
                || ((uref == 'H') && (gref != 'G'))
                || ((gref == 'H') && (uref != 'G'))
                || ((uref == 'V') && (gref != 'T'))
                || ((gref == 'V') && (uref != 'T'))
                || ((uref == 'W') && ((gref == 'A') || (gref == 'T')))
                || ((gref == 'W') && ((uref == 'A') || (uref == 'T')))
                || ((uref == 'S') && ((gref == 'C') || (gref == 'G')))
                || ((gref == 'S') && ((uref == 'C') || (uref == 'G')))
                || ((uref == 'M') && ((gref == 'A') || (gref == 'C')))
                || ((gref == 'M') && ((uref == 'A') || (uref == 'C')))
                || ((uref == 'K') && ((gref == 'G') || (gref == 'T')))
                || ((gref == 'K') && ((uref == 'G') || (uref == 'T')))
                || ((uref == 'R') && ((gref == 'A') || (gref == 'G')))
                || ((gref == 'R') && ((uref == 'A') || (uref == 'G')))
                || ((uref == 'Y') && ((gref == 'C') || (gref == 'T')))
                || ((gref == 'Y') && ((uref == 'C') || (uref == 'T'))))
        {
            ret = NORM_VALID; // valid but not consistent
            continue;
        }
        return NORM_INVALID; // invalid reference
    }
    return ret; // sequence OK
}

/**
 * Flip the allele nucleotides (replaces each letter with its complement).
 * The resulting string is always in uppercase.
 * Support extended nucleotide letters.
 *
 * @param allele  Allele. String containing a sequence of nucleotide letters.
 * @param size    Length of the allele string.
 */
static void flip_allele(char *allele, size_t size)
{
    /*
      Byte map for allele flipping (complement):

      A > T
      T > A
      C > G
      G > C
      M > K
      K > M
      R > Y
      Y > R
      B > V
      V > B
      D > H
      H > D
    */
    static const char map[] = "00000000000000000000000000000000"
                              "00000000000000000123456789000000"
                              /*ABCDEFGHIJKLMNOPQRSTUVWXYZ*/
                              "0TVGHEFCDIJMLKNOPQYSAUBWXRZ00000"
                              /*abcdefghijklmnopqrstuvwxyz*/
                              "0TVGHEFCDIJMLKNOPQYSAUBWXRZ00000"
                              "00000000000000000000000000000000"
                              "00000000000000000000000000000000"
                              "00000000000000000000000000000000"
                              "00000000000000000000000000000000";
    size_t i;
    for (i = 0; i < size; i++)
    {
        allele[i] = map[((uint8_t)allele[i])];
    }
    allele[size] = 0;
}

static inline void swap_sizes(size_t *first, size_t *second)
{
    size_t tmp = *first;
    *first = *second;
    *second = tmp;
}

static inline void swap_alleles(char *first, size_t *sizefirst, char *second, size_t *sizesecond)
{
    char tmp[ALLELE_MAXSIZE];
    strncpy(tmp, first, *sizefirst);
    strncpy(first, second, *sizesecond);
    strncpy(second, tmp, *sizefirst);
    swap_sizes(sizefirst, sizesecond);
    first[*sizefirst] = 0;
    second[*sizesecond] = 0;
}

/**
 * Normalize a variant.
 * Flip alleles if required and apply the normalization algorithm described at:
 * https://genome.sph.umich.edu/wiki/Variant_Normalization
 *
 * @param mf         Structure containing the memory mapped file.
 * @param chrom      Chromosome encoded number.
 * @param pos        Position. The reference position, with the first base having position 0.
 * @param ref        Reference allele. String containing a sequence of nucleotide letters.
 * @param sizeref    Length of the ref string, excluding the terminating null byte.
 * @param alt        Alternate non-reference allele string.
 * @param sizealt    Length of the alt string, excluding the terminating null byte.
 *
 * @return Positive bitmask number in case of success, negative number in case of error.
 *         When positive, each bit has a different meaning when set, has defined by the NORM_* defines:
 *         - bit 0 (NORM_VALID) : The reference allele is inconsistent with the genome reference (i.e. when contains nucleotide letters other than A, C, G and T).
 *         - bit 1 (NORM_SWAP)  : The alleles have been swapped.
 *         - bit 2 (NORM_FLIP)  : The alleles nucleotides have been flipped (each nucleotide have been replaced with its complement).
 *         - bit 3 (NORM_LEXT)  : Alleles have been left extended.
 *         - bit 4 (NORM_RTRIM) : Alleles have been right trimmed.
 *         - bit 5 (NORM_LTRIM) : Alleles have been left trimmed.
 */
static int normalize_variant(mmfile_t mf, uint8_t chrom, uint32_t *pos, char *ref, size_t *sizeref, char *alt, size_t *sizealt)
{
    char left;
    char fref[ALLELE_MAXSIZE];
    char falt[ALLELE_MAXSIZE];
    int status;
    status = check_reference(mf, chrom, *pos, ref, *sizeref);
    if (status == -2)
    {
        return status; // invalid position
    }
    if (status < 0)
    {
        status = check_reference(mf, chrom, *pos, alt, *sizealt);
        if (status >= 0)
        {
            swap_alleles(ref, sizeref, alt, sizealt);
            status |= NORM_SWAP;
        }
        else
        {
            strncpy(fref, ref, *sizeref);
            flip_allele(fref, *sizeref);
            status = check_reference(mf, chrom, *pos, fref, *sizeref);
            if (status >= 0)
            {
                strncpy(ref, fref, *sizealt);
                flip_allele(alt, *sizealt);
                status |= NORM_FLIP;
            }
            else
            {
                strncpy(falt, alt, *sizealt);
                flip_allele(falt, *sizealt);
                status = check_reference(mf, chrom, *pos, falt, *sizealt);
                if (status >= 0)
                {
                    strncpy(ref, falt, *sizealt);
                    strncpy(alt, fref, *sizeref);
                    swap_sizes(sizeref, sizealt);
                    status |= NORM_SWAP + NORM_FLIP;
                }
                else
                {
                    return status; // invalid reference
                }
            }
        }
    }
    if ((*sizealt == 1) && (*sizeref == 1))
    {
        return status; // SNP
    }
    while (1)
    {
        // left extend
        if (((*sizealt == 0) || (*sizeref == 0)) && (*pos > 0))
        {
            (*pos)--;
            left = (char)mf.src[(mf.index[chrom] + *pos)];
            prepend_char(left, alt, sizealt);
            prepend_char(left, ref, sizeref);
            status |= NORM_LEXT;
        }
        else
        {
            // right trim
            if ((*sizealt > 1) && (*sizeref > 1) && (aztoupper(alt[(*sizealt - 1)]) == aztoupper(ref[(*sizeref - 1)])))
            {
                (*sizealt)--;
                (*sizeref)--;
                status |= NORM_RTRIM;
            }
            else
            {
                break;
            }
        }
    }
    // left trim
    uint8_t offset = 0;
    while ((offset < (*sizealt - 1)) && (offset < (*sizeref - 1)) && (aztoupper(alt[offset]) == aztoupper(ref[offset])))
    {
        offset++;
    }
    if (offset > 0)
    {
        *pos += offset;
        *sizeref -= offset;
        *sizealt -= offset;
        memmove(ref, ref + offset, *sizeref);
        memmove(alt, alt + offset, *sizealt);
        status |= NORM_LTRIM;
    }
    ref[*sizeref] = 0;
    alt[*sizealt] = 0;
    return status;
}

#endif  // GENOREF_H
