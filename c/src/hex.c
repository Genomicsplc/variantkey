// VariantKey
//
// hex.c
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

#include <stdio.h>
#include "hex.h"

size_t hex_uint64_t(uint64_t n, char *str)
{
    return sprintf(str, "%016" PRIx64, n);
}

uint64_t parse_hex_uint64_t(const char *s)
{
    uint64_t v = 0;
    uint8_t b;
    size_t i;
    for (i = 0; i < 16; i++)
    {
        b = s[i];
        if (b >= 'a')
        {
            b -= ('a' - 10); // a-f
        }
        else
        {
            if (b >= 'A')
            {
                b -= ('A' - 10); // A-F
            }
            else
            {
                b -= '0'; // 0-9
            }
        }
        v = ((v << 4) | b);
    }
    return v;
}
