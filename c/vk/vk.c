// VariantKey Fast Encoder Command Line Application
//
// vk.c
//
// @category   Tools
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

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/variantkey.c"

#ifndef VERSION
#define VERSION "0.0.0-0"
#endif

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        fprintf(stderr, "VariantKey Encoder %s\nUsage: vk CHROM POS REF ALT\n", VERSION);
        return 1;
    }
    fprintf(stdout, "%016" PRIx64, variantkey(argv[1], strlen(argv[1]), strtoull(argv[2], NULL, 10), argv[3], strlen(argv[3]), argv[4], strlen(argv[4])));
}
