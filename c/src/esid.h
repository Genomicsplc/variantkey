// VariantKey
//
// esid.h
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
 * @file esid.h
 * @brief Utility functions to encode strings.
 *
 * Utility functions to encode strings.
 */

#ifndef ESID_H
#define ESID_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

#define ESIDSHIFT_MAXLEN 10 //!< Maximum number of characters that can be encoded 

/**
 * Encode maximum 10 characters of a string into a 64 bit unsigned integer.
 * This function can be used to convert generic string IDs to numeric IDs.
 *
 * @param str    The string to encode. It must be maximum 10 characters long and support ASCII characters from '!' to 'z'.
 * @param size   Length of the string, excluding the terminating null byte.
 * @param start  First character to encode, starting from 0. To encode the last 10 characters, set this value at (size - 10).
 *
 * @return Encoded string ID.
 */
uint64_t encode_string_id(const char *str, size_t size, size_t start);

/**
 * Decode the encoded string ID.
 * This function is the reverse of encode_string_id.
 * The string is always returned in uppercase mode.
 *
 * @param esid   Encoded string ID code.
 * @param str    String buffer to be returned. Its size should be enough to contain the results (at least 11 bytes).
 *
 * @return The total number of characters excluding the null-character appended at the end of the string.
 *         Returns 0 if the string is not reversible.
 */
size_t decode_string_id(uint64_t esid, char *str);

/**
 * Hash the input string into a 64 bit unsigned integer.
 * This function can be used to convert long string IDs to numeric IDs.
 *
 * @param str    The string to encode.
 * @param size   Length of the string, excluding the terminating null byte.
 *
 * @return Hash string ID.
 */
uint64_t hash_string_id(const char *str, size_t size);

#ifdef __cplusplus
}
#endif

#endif  // ESID_H
