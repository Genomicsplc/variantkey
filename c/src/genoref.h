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
char get_genoref_seq(const unsigned char *src, uint32_t idx[], uint8_t chrom, uint32_t pos);

#ifdef __cplusplus
}
#endif

#endif  // GENOREF_H
