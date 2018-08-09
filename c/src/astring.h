// VariantKey
//
// astring.h
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
 * @file astring.h
 * @brief Utility functions to manipulate allele strings.
 *
 * Collection of utility functions to manipulate allele strings.
 */

#ifndef ASTRING_H
#define ASTRING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

#define ESIDSHIFT_MAXLEN 10 //!< Maximum number of characters that can be encoded 
#define ESIDSHIFT_POS 60    //!< Encoded string ID LEN LSB position from LSB [ LLLL0000 00111111 22222233 33334444 44555555 66666677 77778888 88999999 ]

/**
 * Returns the uppercase version of the input character.
 * Note that this is safe to be used only with a-z characters.
 * All characters above 'a' will be changed.
 *
 * @param c Character to uppercase.
 *
 * @return Uppercased character
 */
int aztoupper(int c);

/**
 * Prepend a character to a string.
 *
 * @param pre    Character to prepend.
 * @param string String to modify.
 * @param size   Input string length.
 *
 * @return void
 */
void prepend_char(unsigned char pre, char *string, size_t *size);

/** @brief Returns uint64_t hexadecimal string (16 characters).
 *
 * @param n     Number to parse
 * @param str   String buffer to be returned (it must be sized 17 bytes at least).
 *
 * @return      Upon successful return, these function returns the number of characters processed
 *              (excluding the null byte used to end output to strings).
 *              If the buffer size is not sufficient, then the return value is the number of characters required for
 *              buffer string, including the terminating null byte.
 */
size_t hex_uint64_t(uint64_t n, char *str);

/** @brief Parses a 16 chars hexadecimal string and returns the code.
 *
 * @param s    Hexadecimal string to parse (it must contain 16 hexadecimal characters).
 *
 * @return uint64_t unsigned integer number.
 */
uint64_t parse_hex_uint64_t(const char *s);

/**
 * Encode the last 10 characters of a string into a 64 bit unsigned integer.
 * This function can be used to convert generic string IDs to numeric IDs.
 *
 * @param str    The string to encode. It must be maximum 10 characters long and support ASCII characters from '!' to 'z'.
 * @param size   Length of the string, excluding the terminating null byte.
 *
 * @return Encoded string ID.
 */
uint64_t encode_string_id(const char *str, size_t size);

/**
 * Decode the encoded string ID.
 * This function is the reverse of encode_string_id.
 * The string is always returned in uppercase mode.
 *
 * @param esid   Encoded string ID code.
 * @param str    String buffer to be returned. Its size should be enough to contain the results (at least 11 bytes).
 *
 * @return If successful, the total number of characters written is returned,
 *         excluding the null-character appended at the end of the string,
 *         otherwise a negative number is returned in case of failure.
 */
size_t decode_string_id(uint64_t esid, char *str);

#ifdef __cplusplus
}
#endif

#endif  // ASTRING_H
