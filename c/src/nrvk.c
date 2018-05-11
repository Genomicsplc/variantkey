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

#include "nrvk.h"

size_t find_ref_alt_by_variantkey(const unsigned char *src, uint64_t last, uint64_t vk, char *ref, size_t *sizeref, char *alt, size_t *sizealt)
{
    uint64_t first = 0;
    uint64_t max = last;
    uint64_t found = find_first_uint64_t(src, BINBLKLEN, 0, 0, 63, &first, &max, vk);
    if (found > last)
    {
        return 0; // not found
    }
    uint64_t offset = bytes_to_uint64_t(src, get_address(BINBLKLEN, 8, found), 0, 63);
    *sizeref = (size_t) bytes_to_uint8_t(src, offset++, 0, 7);
    *sizealt = (size_t) bytes_to_uint8_t(src, offset++, 0, 7);
    size_t i;
    for (i = 0; i < *sizeref; i++)
    {
        ref[i] = src[offset++];
    }
    ref[i] = 0;
    for (i = 0; i < *sizealt; i++)
    {
        alt[i] = src[offset++];
    }
    alt[i] = 0;
    return (*sizeref + *sizealt);
}
