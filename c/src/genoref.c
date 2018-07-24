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
#include <string.h>
#include "genoref.h"

inline void load_genoref_index(const unsigned char *src, uint32_t idx[])
{
    idx[0] = 0;
    int i;
    for (i = 1; i <= 26; i++)
    {
        idx[i] = bytes_to_uint32_t(src, ((uint64_t)(i - 1) * 4));
    }
}

inline char get_genoref_seq(const unsigned char *src, const uint32_t idx[], uint8_t chrom, uint32_t pos)
{
    uint32_t offset = (idx[chrom] + pos);
    if (offset > (idx[(chrom + 1)] - 2))
    {
        return 0; // invalid position
    }
    return (char)src[offset];
}

inline int check_reference(const unsigned char *src, const uint32_t idx[], uint8_t chrom, uint32_t pos, const char *ref, size_t sizeref)
{
    uint32_t offset = (idx[chrom] + pos);
    if ((offset + sizeref + 1) > (idx[(chrom + 1)]))
    {
        return -2; // invalid position
    }
    size_t i;
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
        /*
            Abbreviation codes for degenerate bases
            (http://www.sbcs.qmul.ac.uk/iubmb/misc/naseq.html)

            SYMBOL | DESCRIPTION                   | BASES REPRESENTED
            -------+-------------------------------+------------------
               A   | adenine                       | A
               C   | cytosine                      |  C
               G   | guanine                       |   G
               T   | thymine                       |    T
               W   | weak                          | A  T
               S   | strong                        |  CG
               M   | amino                         | AC
               K   | keto                          |   GT
               R   | purine                        | A G
               Y   | pyrimidine                    |  C T
               B   | not A (B comes after A)       |  CGT
               D   | not C (D comes after C)       | A GT
               H   | not G (H comes after G)       | AC T
               V   | not T (V comes after T and U) | ACG
               N   | any base (not a gap)          | ACGT
            -------+-------------------------------+------------------
        */
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
            ret = 1; // valid but not consistent
            continue;
        }
        return -1; // invalid reference
    }
    return ret; // sequence OK
}

inline void flip_allele(char *allele, size_t size)
{
    /*
      Allele flipping:

      A > T
      T > A
      C > G
      G > C
      M > K
      K > M
      R > Y
      Y > R
      B > V
      V > B
      D > H
      H > D
    */
    static const char map[] = "00000000000000000000000000000000"
                              "00000000000000000123456789000000"
                              /*ABCDEFGHIJKLMNOPQRSTUVWXYZ*/
                              "0TVGHEFCDIJMLKNOPQYSAUBWXRZ00000"
                              /*abcdefghijklmnopqrstuvwxyz*/
                              "0TVGHEFCDIJMLKNOPQYSAUBWXRZ00000"
                              "00000000000000000000000000000000"
                              "00000000000000000000000000000000"
                              "00000000000000000000000000000000"
                              "00000000000000000000000000000000";
    size_t i;
    for (i = 0; i < size; i++)
    {
        allele[i] = map[((unsigned char)allele[i])];
    }
    allele[size] = 0;
}

int normalize_variant(const unsigned char *src, const uint32_t idx[], uint8_t chrom, uint32_t *pos, char *ref, size_t *sizeref, char *alt, size_t *sizealt)
{
    char left;
    char fref[ALLELE_MAXSIZE];
    int status;
    status = check_reference(src, idx, chrom, *pos, ref, *sizeref);
    if (status == -2)
    {
        return status; // invalid position
    }
    if (status < 0)
    {
        // flip allele and recheck
        strncpy(fref, ref, *sizeref);
        flip_allele(fref, *sizeref);
        status = check_reference(src, idx, chrom, *pos, fref, *sizeref);
        if (status < 0)
        {
            return status; // invalid reference
        }
        // flip alleles
        strncpy(ref, fref, *sizeref);
        flip_allele(alt, *sizealt);
        status |= 2;
    }
    if ((*sizealt == 1) && (*sizeref == 1))
    {
        return status; // SNP
    }
    while (1)
    {
        // left extend
        if (((*sizealt == 0) || (*sizeref == 0)) && (*pos > 0))
        {
            (*pos)--;
            left = (char)src[(size_t)(idx[chrom] + *pos)];
            prepend_char(left, alt, sizealt);
            prepend_char(left, ref, sizeref);
            status |= 4;
        }
        else
        {
            // right trim
            if ((*sizealt > 1) && (*sizeref > 1) && (aztoupper(alt[(*sizealt - 1)]) == aztoupper(ref[(*sizeref - 1)])))
            {
                (*sizealt)--;
                (*sizeref)--;
                status |= 8;
            }
            else
            {
                break;
            }
        }
    }
    // left trim
    uint8_t offset = 0;
    while ((offset < (*sizealt - 1)) && (offset < (*sizeref - 1)) && (aztoupper(alt[offset]) == aztoupper(ref[offset])))
    {
        offset++;
    }
    if (offset > 0)
    {
        *pos += offset;
        *sizeref -= offset;
        *sizealt -= offset;
        memmove(ref, ref + offset, *sizeref);
        memmove(alt, alt + offset, *sizealt);
        status |= 16;
    }
    ref[*sizeref] = 0;
    alt[*sizealt] = 0;
    return status;
}

