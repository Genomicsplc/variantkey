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

#include <inttypes.h>
#include <stdio.h>

#define ESID_MAXLEN   10 //!< Maximum number of characters that can be encoded
#define ESID_SHIFT    32 //!< Number used to translate ASCII character values
#define ESID_SHIFTPOS 60 //!< Encoded string ID LEN LSB position from LSB [ ----0000 00111111 22222233 33334444 44555555 66666677 77778888 88999999 ]
#define ESID_CHARBIT  6  //!< Number of bit used to encode a char
#define ESID_NUMPOS   27 //!< Number of bit used to encode a number in the srting_num encoding
#define ESID_MAXPAD   7  //!< Max number of padding zero digits

static inline uint64_t esid_encode_char(int c)
{
    if (c < '!')
    {
        return (uint64_t)('_' - ESID_SHIFT);
    }
    if (c > '_')
    {
        return (uint64_t)(c - ('a' - 'A' + ESID_SHIFT));
    }
    return (uint64_t)(c - ESID_SHIFT);
}

static inline uint8_t esid_decode_char(uint64_t esid, size_t pos)
{
    return (uint8_t)(((esid >> pos) & 0x3f) + ESID_SHIFT); // 0x3f hex = 63 dec = 00111111 bin
}

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
static uint64_t encode_string_id(const char *str, size_t size, size_t start)
{
    size -= start;
    if (size > ESID_MAXLEN)
    {
        size = ESID_MAXLEN;
    }
    str += start;
    const char *pos = (str + size - 1);
    uint64_t h = (size << ESID_SHIFTPOS);
    switch (size)
    {
    case 10:
        h |= esid_encode_char(*pos--);
    // fall through
    case 9:
        h |= esid_encode_char(*pos--) << (ESID_CHARBIT * 1);
    // fall through
    case 8:
        h |= esid_encode_char(*pos--) << (ESID_CHARBIT * 2);
    // fall through
    case 7:
        h |= esid_encode_char(*pos--) << (ESID_CHARBIT * 3);
    // fall through
    case 6:
        h |= esid_encode_char(*pos--) << (ESID_CHARBIT * 4);
    // fall through
    case 5:
        h |= esid_encode_char(*pos--) << (ESID_CHARBIT * 5);
    // fall through
    case 4:
        h |= esid_encode_char(*pos--) << (ESID_CHARBIT * 6);
    // fall through
    case 3:
        h |= esid_encode_char(*pos--) << (ESID_CHARBIT * 7);
    // fall through
    case 2:
        h |= esid_encode_char(*pos--) << (ESID_CHARBIT * 8);
    // fall through
    case 1:
        h |= esid_encode_char(*pos) << (ESID_CHARBIT * 9);
    }
    return h;
}

/**
 * Encode a string composed by a character section followed by a separator character and a numerical section
 * into a 64 bit unsigned integer. For example: "ABCDE:0001234".
 * Encodes up to 5 characters in uppercase, a number up to 2^27, and up to 7 zero padding digits.
 * If the string is 10 character or less, then the encode_string_id() is used.
 *
 * @param str    The string to encode. Support ASCII characters from '!' to 'z'.
 * @param size   Length of the string, excluding the terminating null byte.
 * @param sep    Separator character between string and number.
 *
 * @return Encoded string ID.
 */
static uint64_t encode_string_num_id(const char *str, size_t size, char sep)
{
    if (size <= ESID_MAXLEN)
    {
        return encode_string_id(str, size, 0);
    }
    uint64_t h = 0;
    uint32_t num = 0;
    uint8_t nchr = 0, npad = 0;
    uint8_t bitpos = ESID_SHIFTPOS;
    int c;
    while ((c = *str++) && (size--))
    {
        if (c == sep)
        {
            break;
        }
        if (nchr < 5)
        {
            bitpos -= ESID_CHARBIT;
            h |= (esid_encode_char(c) << bitpos);
            nchr++;
        }
    }
    h |= ((uint64_t)(nchr + ESID_MAXLEN) << ESID_SHIFTPOS); // 4 bit for string length
    while (((c = *str++) == '0') && (npad < ESID_MAXPAD) && (size--))
    {
        npad++;
    }
    h |= (npad << ESID_NUMPOS); // 3 bit for 0 padding length
    while ((c >= '0') && (c <= '9') && (size--))
    {
        num = ((num * 10) + (c - '0'));
        c = *str++;
    }
    h |= ((uint64_t)num & 0x7FFFFFF); // 27 bit for number
    return h;
}

static inline size_t esid_decode_string_id(size_t size, uint64_t esid, char *str)
{
    switch (size)
    {
    case 10:
        str[9] = esid_decode_char(esid, 0);
    // fall through
    case 9:
        str[8] = esid_decode_char(esid, (ESID_CHARBIT * 1));
    // fall through
    case 8:
        str[7] = esid_decode_char(esid, (ESID_CHARBIT * 2));
    // fall through
    case 7:
        str[6] = esid_decode_char(esid, (ESID_CHARBIT * 3));
    // fall through
    case 6:
        str[5] = esid_decode_char(esid, (ESID_CHARBIT * 4));
    // fall through
    case 5:
        str[4] = esid_decode_char(esid, (ESID_CHARBIT * 5));
    // fall through
    case 4:
        str[3] = esid_decode_char(esid, (ESID_CHARBIT * 6));
    // fall through
    case 3:
        str[2] = esid_decode_char(esid, (ESID_CHARBIT * 7));
    // fall through
    case 2:
        str[1] = esid_decode_char(esid, (ESID_CHARBIT * 8));
    // fall through
    case 1:
        str[0] = esid_decode_char(esid, (ESID_CHARBIT * 9));
    }
    str[size] = 0;
    return size;
}

static inline size_t esid_decode_string_num_id(size_t size, uint64_t esid, char *str)
{
    size = esid_decode_string_id(size, esid, str);
    str[size++] = ':';
    uint8_t npad = (uint8_t)((esid >> ESID_NUMPOS) & ESID_MAXPAD);
    while (npad--)
    {
        str[size++] = '0';
    }
    uint64_t num = (esid & 0x7FFFFFF);
    if (num > 0)
    {
        char *ptr = (str + size);
        size += sprintf(ptr, "%" PRIu64, num);
    }
    str[size] = 0;
    return size;
}

/**
 * Decode the encoded string ID.
 * This function is the reverse of encode_string_id.
 * The string is always returned in uppercase mode.
 *
 * @param esid   Encoded string ID code.
 * @param str    String buffer to be returned. Its size should be enough to contain the results (at least 11 bytes).
 *
 * @return The total number of characters excluding the null-character appended at the end of the string.
 */
static size_t decode_string_id(uint64_t esid, char *str)
{
    size_t size = (esid >> ESID_SHIFTPOS);
    if (size > ESID_MAXLEN)
    {
        return esid_decode_string_num_id((size - ESID_MAXLEN), esid, str);
    }
    return esid_decode_string_id(size, esid, str);
}

// Mix two 64 bit hash numbers using a MurmurHash3-like algorithm
static inline uint64_t muxhash64(uint64_t k, uint64_t h)
{
    k *= 0x87c37b91114253d5;
    k = (k >> 33) | (k << 31);
    k *= 0x4cf5ad432745937f;
    h ^= k;
    h = (h >> 37) | (h << 27);
    return ((h * 5) + 0x52dce729);
}

/**
 * Hash the input string into a 64 bit unsigned integer.
 * This function can be used to convert long string IDs into non-reversible numeric IDs.
 *
 * @param str    The string to encode.
 * @param size   Length of the string, excluding the terminating null byte.
 *
 * @return Hash string ID.
 */
static uint64_t hash_string_id(const char *str, size_t size)
{
    const uint64_t *pos = (const uint64_t *)str; // NOTE endianness
    const uint64_t *end = pos + (size / 8);
    uint64_t h = 0;
    while (pos < end)
    {
        h = muxhash64(*pos++, h);
    }
    const uint8_t *tail = (const uint8_t *)pos;
    uint64_t v = 0;
    switch (size & 7)
    {
    case 7:
        v ^= (uint64_t)tail[6] << (8 * 6);
    // fall through
    case 6:
        v ^= (uint64_t)tail[5] << (8 * 5);
    // fall through
    case 5:
        v ^= (uint64_t)tail[4] << (8 * 4);
    // fall through
    case 4:
        v ^= (uint64_t)tail[3] << (8 * 3);
    // fall through
    case 3:
        v ^= (uint64_t)tail[2] << (8 * 2);
    // fall through
    case 2:
        v ^= (uint64_t)tail[1] << (8 * 1);
    // fall through
    case 1:
        v ^= (uint64_t)tail[0];
    }
    if (v > 0)
    {
        h = muxhash64(v, h);
    }
    // MurmurHash3 finalization mix - force all bits of a hash block to avalanche
    h ^= h >> 33;
    h *= 0xff51afd7ed558ccd;
    h ^= h >> 33;
    h *= 0xc4ceb9fe1a85ec53;
    h ^= h >> 33;
    return (h | 0x8000000000000000); // set the first bit to indicate HASH mode
}

#endif  // ESID_H
