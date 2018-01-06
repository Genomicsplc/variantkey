// This is a C99 translation of the Fingerprint64 (farmhashna::Hash64) code
// from Google's FarmHash (https://github.com/google/farmhash).

// Copyright (c) 2014 Google, Inc.
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
 * @file farmhash64.h
 * @brief File containing the definition of public functions.
 *
 * FarmHash is a family of hash functions.
 *
 * This is a C99 translation of the Fingerprint64 (farmhashna::Hash64) code from Google's FarmHash
 * (https://github.com/google/farmhash).
 *
 * FarmHash64 provides a portable 64-bit hash function for strings (byte array).
 * The function mix the input bits thoroughly but is not suitable for cryptography.
 *
 * All members of the FarmHash family were designed with heavy reliance on previous work by Jyrki Alakuijala, Austin Appleby, Bob Jenkins, and others.
 * For more information please consult https://github.com/google/farmhash
 */

#ifndef FARMHASH64_H
#define FARMHASH64_H

#include <stdint.h>
#include <stdlib.h>

/**
 * @brief 64 bit hash.
 *
 * Returns a 64-bit fingerprint hash for a byte array.
 *
 * This function is not suitable for cryptography.
 *
 * @param s   string to process
 * @param len string length
 *
 * @return 64-bit hash code
 */
uint64_t farmhash64(const char* s, size_t len);

/**
 * @brief 32 bit hash.
 *
 * Returns a 32-bit fingerprint hash for a byte array.
 *
 * NOTE: This is NOT equivalent to the original Fingerprint32 function.
 * It is a new function based on farmhash64.
 *
 * This function is not suitable for cryptography.
 *
 * @param s   string to process
 * @param len string length
 *
 * @return 32-bit hash code
 */
uint32_t farmhash32(const char* s, size_t len);

#endif  // FARMHASH64_H
