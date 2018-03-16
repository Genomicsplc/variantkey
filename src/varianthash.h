// VariantHash
//
// varianthash.h
//
// @category   Tools
// @author     Nicola Asuni <nicola.asuni@genomicsplc.com>
// @copyright  2017-2018 GENOMICS plc
// @license    MIT (see LICENSE)
// @link       https://github.com/genomicsplc/varianthash
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
// VariantHash by Nicola Asuni

/**
 * @file varianthash.h
 * @brief File containing the definition of public functions.
 *
 * The functions provided here allows to generate or decode
 * a genetic VariantHash128 and VariantHash64.
 */

#ifndef VARIANTHASH_H
#define VARIANTHASH_H

#include "farmhash64.h"

/**
 * VariantHash128 struct
 */
typedef struct varhash128_t
{
    uint32_t assembly; //!< Genome Assembly encoded number.
    uint32_t chrom;    //!< Chromosome encoded number.
    uint32_t pos;      //!< Position. The reference position, with the 1st base having position 0.
    uint32_t refalt;   //!< Hash code for Reference and Alternate
} varhash128_t;

/**
 * VariantHash64 struct
 */
typedef struct varhash64_t
{
    uint8_t chrom;     //!< Chromosome encoded number.
    uint32_t pos;      //!< Position. The reference position, with the 1st base having position 0.
    uint32_t refalt;   //!< Hash code for Reference and Alternate (only the LSB 24 bits are used)
} varhash64_t;

/** @brief Fast ASCII Uppercase function for a-z letters
 *
 * @param c character code to convert
 *
 * @return uppercase character code
 */
int aztoupper(int c);

/** @brief Returns 32-bit genome assembly encoding.
 *
 * @param assembly Genome Assembly string.
 *
 * @return Chrom encoding
 */
uint32_t encode_assembly_32bit(const char *assembly);

/** @brief Returns 32-bit chromosome encoding.
 *
 * @param chrom Chromosome. An identifier from the reference genome, no white-space or leading zeros permitted.
 *
 * @return Chrom encoding
 */
uint32_t encode_chrom_32bit(const char *chrom);

/** @brief Returns 8-bit chromosome encoding.
 *
 * @param chrom Chromosome. An identifier from the reference genome, no white-space or leading zeros permitted.
 *
 * @return Chrom encoding
 */
uint8_t encode_chrom_8bit(const char *chrom);

/** @brief Decode the 32-bit CHROM code.
 *
 * @param code   CHROM code.
 * @param chrom  CHROM string buffer to be returned.
 *
 * @return If successful, the total number of characters written is returned excluding the null-character appended
 *         at the end of the string, otherwise a negative number is returned in case of failure.
 */
size_t decode_chrom_32bit(uint32_t code, char *chrom);

/** @brief Decode the 8-bit CHROM code.
 *
 * @param code   CHROM code.
 * @param chrom  CHROM string buffer to be returned.
 *
 * @return If successful, the total number of characters written is returned excluding the null-character appended
 *         at the end of the string, otherwise a negative number is returned in case of failure.
 */
size_t decode_chrom_8bit(uint8_t code, char *chrom);

/** @brief Returns 32-bit reference+alternate hash code.
 *
 * @param ref   Reference allele. String containing a sequence of nucleotide letters.
 *              The value in the pos field refers to the position of the first nucleotide in the String.
 * @param alt   Alternate non-reference allele string.
 *
 * @return 32-bit hash code
 */
uint32_t encode_refalt_32bit(const char *ref, const char *alt);

/** @brief Returns 24-bit reference+alternate hash code.
 *
 * @param ref   Reference allele. String containing a sequence of nucleotide letters.
 *              The value in the pos field refers to the position of the first nucleotide in the String.
 * @param alt   Alternate non-reference allele string.
 *
 * @return 24-bit hash code (in 32 bit unsignd integer)
 */
uint32_t encode_refalt_24bit(const char *ref, const char *alt);

/** @brief Decode the 32-bit REF+ALT code if reversible (if it has less than 5 nucleotides in total).
 *
 * @param code  REF+ALT code
 * @param ref   REF string buffer to be returned.
 * @param alt   ALT string buffer to be returned.
 *
 * @return      Returns the number of characters in the "alt" string if the code is reversible, 0 otherwise.
 */
size_t decode_refalt_32bit(uint32_t code, char *ref, char *alt);

/** @brief Decode the 24-bit REF+ALT code if reversible (if it has less than 5 nucleotides in total).
 *
 * @param code  REF+ALT code
 * @param ref   REF string buffer to be returned.
 * @param alt   ALT string buffer to be returned.
 *
 * @return      Returns the number of characters in the "alt" string if the code is reversible, 0 otherwise.
 */
size_t decode_refalt_24bit(uint32_t code, char *ref, char *alt);

/** @brief Returns a VariantHash128 structure based on ASSEMBLY, CHROM, POS (0-base), REF, ALT.
 *
 * @param assembly  String identifying the Genome Assembly. It should be in the form used by
 *                  Genome Reference Consortium (https://www.ncbi.nlm.nih.gov/grc),
 *                  including the patch number and build number separated by a dot.
 *                  For example: `GRCh37.p13.b150`.
 * @param chrom     Chromosome. An identifier from the reference genome, no white-space or leading zeros permitted.
 * @param pos       Position. The reference position, with the 1st base having position 0.
 * @param ref       Reference allele. String containing a sequence of nucleotide letters.
 *                  The value in the pos field refers to the position of the first nucleotide in the String.
 * @param alt       Alternate non-reference allele string.
 *
 * @return      A varhash128_t structure.
 */
varhash128_t varianthash128(const char *assembly, const char *chrom, uint32_t pos, const char *ref, const char *alt);


/** @brief Returns a 64-bit variant code based on CHROM, POS (0-base), REF, ALT.
 *
 * @param chrom     Chromosome. An identifier from the reference genome, no white-space or leading zeros permitted.
 * @param pos       Position. The reference position, with the 1st base having position 0.
 * @param ref       Reference allele. String containing a sequence of nucleotide letters.
 *                  The value in the pos field refers to the position of the first nucleotide in the String.
 * @param alt       Alternate non-reference allele string.
 *
 * @return      VariantHash 64-bit code.
 */
uint64_t varianthash64(const char *chrom, uint32_t pos, const char *ref, const char *alt);

/** @brief Returns VariantHash128 hexadecimal string (32 characters).
 *
 * The string represent a 128 bit number or:
 *   - 32 bit  (4 bytes, 8 hex bytes) for ASSBLY Hash
 *   - 32 bit  (4 bytes, 8 hex bytes) for CHROM
 *   - 32 bit  (4 bytes, 8 hex bytes) for POS
 *   - 32 bit  (4 bytes, 8 hex bytes) for REF+ALT Hash
 *
 * @param str   String buffer to be returned.
 * @param size  Size of the string buffer.
 * @param vh    Variant hash structure to be processed.
 *
 * @return      Upon successful return, these function returns the number of characters processed
 *              (excluding the null byte used to end output to strings).
 *              If the buffer size is not sufficient, then the return value is the number of characters required for
 *              buffer string, including the terminating null byte.
 */
size_t varianthash128_string(char *str, size_t size, varhash128_t vh);

/** @brief Returns VariantHash64 hexadecimal string (16 characters).
 *
 * The string represent a 64 bit number or:
 *   -  8 bit  (1 byte,  2 hex bytes) for CHROM
 *   - 32 bit  (4 bytes, 8 hex bytes) for POS
 *   - 24 bit  (3 bytes, 8 hex bytes) for REF+ALT Hash
 *
 * @param str   String buffer to be returned.
 * @param size  Size of the string buffer.
 * @param vh    Variant hash structure to be processed.
 *
 * @return      Upon successful return, these function returns the number of characters processed
 *              (excluding the null byte used to end output to strings).
 *              If the buffer size is not sufficient, then the return value is the number of characters required for
 *              buffer string, including the terminating null byte.
 */
size_t varianthash64_string(char *str, size_t size, uint64_t vh);

/** @brief Parses a VariantHash128 hex string and returns the components as varhash128_t structure.
 *
 * @param vh VariantHash128 hexadecimal string (32 characters).
 *
 * @return A varhash128_t structure.
 */
varhash128_t parse_varianthash128_string(const char *vs);

/** @brief Parses a VariantHash64 hex string and returns the code.
 *
 * @param vh VariantHash64 hexadecimal string (16 characters).
 *
 * @return A VariantHash64 code
 */
uint64_t parse_varianthash64_string(const char *vs);

/** @brief Parses a VariantHash64 code and returns the components as varhash64_t structure.
 *
 * @param code VariantHash64 code.
 *
 * @return A varhash64_t structure.
 */
varhash64_t split_varianthash64(uint64_t code);

#endif  // VARIANTHASH_H
