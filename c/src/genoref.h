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
 *
 * The input binary file has the following format:
 * The first line of the binary fasta file contains the index composed by 26 blocks of 32 bit
 * numbers, one for each of the 25 chromosomes (1 to 22, X, Y and MT), plus one to indicate the end
 * of the file. Each index number represents the file byte offset of the corresponding chromosome
 * sequence. The index is followed by 25 lines, one for each chromosome sequence.
 */

#ifndef GENOREF_H
#define GENOREF_H

#ifdef __cplusplus
extern "C" {
#endif

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
 * Load the genome reference index.
 *
 * @param src  Address of the memory mapped input file containing the genome reference data (fasta.bin).
 * @param idx  Index of sequences offset by chromosome number (1 to 25). The index 26 contains the file length. Requires 27 elements: uint32_t idx[27].
 *
 * @return void
 */
void load_genoref_index(const unsigned char *src, uint32_t idx[]);

/**
 * Returns the genome reference nucleotide at the specified chromosome and position.
 *
 * @param src     Address of the memory mapped input file containing the genome reference data (fasta.bin).
 * @param idx     Index of sequences offset by chromosome number (1 to 25).
 * @param chrom   Encoded Chromosome number (see encode_chrom).
 * @param pos     Position. The reference position, with the first base having position 0.
 *
 * @return The nucleotide letter or 0 in case of invalid position.
 */
char get_genoref_seq(const unsigned char *src, const uint32_t idx[], uint8_t chrom, uint32_t pos);

/**
 * Check if the reference allele matches the reference genome data.
 *
 * @param src     Address of the memory mapped input file containing the genome reference data (fasta.bin).
 * @param idx     Index of sequences offset by chromosome number (1 to 25).
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
int check_reference(const unsigned char *src, const uint32_t idx[], uint8_t chrom, uint32_t pos, const char *ref, size_t sizeref);

/**
 * Flip the allele nucleotides (replaces each letter with its complement).
 * The resulting string is always in uppercase.
 * Support extended nucleotide letters.
 *
 * @param allele  Allele. String containing a sequence of nucleotide letters.
 * @param size    Length of the allele string.
 */
void flip_allele(char *allele, size_t size);

/**
 * Normalize a variant.
 * Flip alleles if required and apply the normalization algorithm described at:
 * https://genome.sph.umich.edu/wiki/Variant_Normalization
 *
 * @param src        Address of the memory mapped input file containing the genome reference data (fasta.bin).
 * @param idx        Index of sequences offset by chromosome number (1 to 25).
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
int normalize_variant(const unsigned char *src, const uint32_t idx[], uint8_t chrom, uint32_t *pos, char *ref, size_t *sizeref, char *alt, size_t *sizealt);

#ifdef __cplusplus
}
#endif

#endif  // GENOREF_H
