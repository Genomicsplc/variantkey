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

#include "astring.h"
#include "binsearch.h"

/**
 * Load the index from the genome reference.
 *
 * @param src  Address of the memory mapped input file contaning the genome reference data (fasta.bin).
 * @param idx  Index of sequences offset by chromosome number (1 to 25). The index 26 contains the file length. Requires 27 elements: uint32_t idx[27];
 *
 * @return void
 */
void load_genoref_index(const unsigned char *src, uint32_t idx[]);

/**
 * Returns the nucleotide at the specified chromosome and position.
 *
 * @param src     Address of the memory mapped input file contaning the genome reference data (fasta.bin).
 * @param idx     Index of sequences offset by chromosome number (1 to 25).
 * @param chrom   Encoded Chromosome number (see encode_chrom).
 * @param pos     Position. The reference position, with the 1st base having position 0.
 *
 * @return The nucleotide letter or 0 in case of invalid position.
 */
char get_genoref_seq(const unsigned char *src, const uint32_t idx[], uint8_t chrom, uint32_t pos);

/**
 * Check if the reference allele matches the reference genome data.
 *
 * @param src     Address of the memory mapped input file contaning the genome reference data (fasta.bin).
 * @param idx     Index of sequences offset by chromosome number (1 to 25).
 * @param chrom   Encoded Chromosome number (see encode_chrom).
 * @param pos     Position. The reference position, with the 1st base having position 0.
 * @param ref     Reference allele. String containing a sequence of nucleotide letters.
 * @param sizeref Length of the ref string, excluding the terminating null byte.
 *
 * @return Positive number in case of success, negative in case of error:
 *         0 the reference allele match the reference genome;
 *         1 the reference allele is inconsistent with the genome reference (i.e. when contains nucleotide letters other than A, C, G and T);
 *        -1 the reference allele don't match the reference genome;
 *        -2 the reference allele is longer than the genome reference sequence.
 */
int check_reference(const unsigned char *src, const uint32_t idx[], uint8_t chrom, uint32_t pos, const char *ref, size_t sizeref);

/**
 * Flip the allele nucleotides.
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
 * @param src        Address of the memory mapped input file contaning the genome reference data (fasta.bin).
 * @param idx        Index of sequences offset by chromosome number (1 to 25).
 * @param chrom      Chromosome encoded number.
 * @param pos        Position. The reference position, with the 1st base having position 0.
 * @param ref        Reference allele. String containing a sequence of nucleotide letters.
 * @param sizeref    Length of the ref string, excluding the terminating null byte.
 * @param alt        Alternate non-reference allele string.
 * @param sizealt    Length of the alt string, excluding the terminating null byte.
 *
 * @return Positive number in case of success, negative in case of error:
 *        -2 the reference allele is longer than the genome reference sequence.
 *        -1 the reference allele don't match the reference genome;
 *        ret &  1 == 1 : the reference allele is inconsistent with the genome reference (i.e. when contains nucleotide letters other than A, C, G and T);
 *        ret &  2 == 1 : the alleles have been flipped;
 *        ret &  4 == 1 : left extended;
 *        ret &  8 == 1 : right trimmed;
 *        ret & 16 == 1 : left trimmed;
 */
int normalize_variant(const unsigned char *src, const uint32_t idx[], uint8_t chrom, uint32_t *pos, char *ref, size_t *sizeref, char *alt, size_t *sizealt);

#ifdef __cplusplus
}
#endif

#endif  // GENOREF_H
