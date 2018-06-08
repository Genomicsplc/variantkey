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

#include <string.h>
#include "genoref.h"

inline void load_genoref_index(const unsigned char *src, uint32_t idx[])
{
    idx[0] = 0;
    int i;
    for (i = 1; i <= 26; i++)
    {
        idx[i] = bytes_to_uint32_t(src, ((i - 1) * 4));
    }
}

inline char get_genoref_seq(const unsigned char *src, uint32_t idx[], uint8_t chrom, uint32_t pos)
{
    uint32_t offset = (idx[chrom] + pos);
    if (offset > (idx[(chrom + 1)] - 2))
    {
        return 0; // invalid position
    }
    return (char)src[offset];
}

static inline int aztoupper(int c)
{
    if (c >= 'a')
    {
        return (c ^ ('a' - 'A'));
    }
    return c;
}

inline int check_reference(const unsigned char *src, uint32_t idx[], uint8_t chrom, uint32_t pos, const char *ref, size_t sizeref)
{
    uint32_t offset = (idx[chrom] + pos);
    if ((offset + sizeref - 1) > (idx[(chrom + 1)] - 2))
    {
        return -2; // invalid position
    }
    int i;
    char uref, gref;
    int ret = 0; // return value
    for (i = 0; i < sizeref; i++)
    {
        uref = aztoupper(ref[i]);
        gref = src[(offset + i)];
        if (uref == gref)
        {
            continue;
        }
        if ((uref == 'N')
                || (gref == 'N')
                || ((uref == 'B') && (gref != 'A'))
                || ((gref == 'B') && (uref != 'A'))
                || ((uref == 'D') && (gref != 'C'))
                || ((gref == 'D') && (uref != 'C'))
                || ((uref == 'H') && (gref != 'G'))
                || ((gref == 'H') && (uref != 'G'))
                || ((uref == 'V') && (gref != 'T'))
                || ((gref == 'V') && (uref != 'T'))
                || ((uref == 'W') && ((gref == 'A') || (gref == 'T')))
                || ((gref == 'W') && ((uref == 'A') || (uref == 'T')))
                || ((uref == 'S') && ((gref == 'C') || (gref == 'G')))
                || ((gref == 'S') && ((uref == 'C') || (uref == 'G')))
                || ((uref == 'M') && ((gref == 'A') || (gref == 'C')))
                || ((gref == 'M') && ((uref == 'A') || (uref == 'C')))
                || ((uref == 'K') && ((gref == 'G') || (gref == 'T')))
                || ((gref == 'K') && ((uref == 'G') || (uref == 'T')))
                || ((uref == 'R') && ((gref == 'A') || (gref == 'G')))
                || ((gref == 'R') && ((uref == 'A') || (uref == 'G')))
                || ((uref == 'Y') && ((gref == 'C') || (gref == 'T')))
                || ((gref == 'Y') && ((uref == 'C') || (uref == 'T'))))
        {
            ret = 1; // not consistent
            continue;
        }
        return -1; // invalid reference
    }
    return ret; // sequence OK
}

inline void flip_allele(char *allele, size_t size)
{
    int i;
    int chr;
    for (i = 0; i < size; i++)
    {
        chr = aztoupper(allele[i]);
        switch(chr)
        {
        case 'A':
            allele[i] = 'T';
            break;
        case 'T':
            allele[i] = 'A';
            break;
        case 'C':
            allele[i] = 'G';
            break;
        case 'G':
            allele[i] = 'C';
            break;
        case 'M':
            allele[i] = 'K';
            break;
        case 'K':
            allele[i] = 'M';
            break;
        case 'R':
            allele[i] = 'Y';
            break;
        case 'Y':
            allele[i] = 'R';
            break;
        case 'B':
            allele[i] = 'V';
            break;
        case 'V':
            allele[i] = 'B';
            break;
        case 'D':
            allele[i] = 'H';
            break;
        case 'H':
            allele[i] = 'D';
            break;
        default:
            allele[i] = chr;
        }
    }
    return;
}
