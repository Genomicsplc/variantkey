# VariantKey

*Genetic Variant Key*

[![Master Build Status](https://secure.travis-ci.org/genomicsplc/variantkey.png?branch=master)](https://travis-ci.org/genomicsplc/variantkey?branch=master)
[![Master Coverage Status](https://coveralls.io/repos/genomicsplc/variantkey/badge.svg?branch=master&service=github)](https://coveralls.io/github/genomicsplc/variantkey?branch=master)

* **category**    Libraries
* **license**     MIT (see LICENSE)
* **link**        https://github.com/genomicsplc/variantkey


## Description

This project contains tools to generate and process a 64 bit Unsigned Integer Key for Human Genetic Variants

The VariantKey is sortable for chromosome and position, and it is also fully reversible for variants with up to 5 nucleotides between Reference and Alternate alleles. It can be used to sort, search and match variant-based data easily and very quickly.

**IMPORTANT**: This model assumes that the variants have been:

a. **decomposed** to convert multialleic variants to bialleic ones (`REF` and single `ALT`).  
In VCF files this can be done using the [vt decompose -s](https://genome.sph.umich.edu/wiki/Vt#Decompose) command,

b. **normalized** as in ["Unified representation of genetic variants" - Tan et al. 2015](https://academic.oup.com/bioinformatics/article/31/13/2202/196142).  
In VCF files this can be done using the [vt normalize](https://genome.sph.umich.edu/wiki/Vt#Normalization) command.

## VariantKey Format

The VariantKey is composed of 3 sections arranged in 64 bit:

```
           8      16      24      32      40      48      56      64
           |       |       |       |       |       |       |       |
    0123456789012345678901234567890123456789012345678901234567890123
    CHROM|           POS            ||           REF+ALT           |
      |               |                             |
      5 bit           28 bit                        31 bit
```

* **`CHROM`**   : 5 bit to represent the chromosome.

    ```
    0   4
    |   |
    11111000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
        |
        LSB
    ```
    Chromosomes are always encoded as numbers.  
    The default string conversion functions includes the encoding for human chromosomes from `1` to `22` plus `X=23`, `Y=24`, `MT=25`, `NA=0`.  
    This value is always reversible.
                
* **`POS`**     : 28 bit for the reference position (`POS`), with the 1<sup>st</sup> nucleotide having position 0.

```
    0    5                             32
         |                              |
    00000111 11111111 11111111 11111111 10000000 00000000 00000000 00000000
         |                              |
        MSB                            LSB
```
    The largest human chromosome contains 249M base pairs that can be easily contained in 28 bit (2^28 = 268,435,456).
                  

* **`REF+ALT`** : 31 bit for the encoding of the `REF` and `ALT` strings.

```
    0                                   33                               63
                                         |                                |
    00000000 00000000 00000000 00000000 01111111 11111111 11111111 11111111
                                         |                                |
                                        MSB                              LSB
```
    The encoding of this field mainly depends on the total length of the `REF`+`ALT` string.
    If the total number of nucleotides in `REF`+`ALT` is more then 11, or if the alleles contains characters other than ACGT, then the LSB bit is set to 1 and the remaining 30 bit are filled with an hash of the `REF` and `ALT` strings. A lookup table is required to reverse the values.  
    If the total number of nucleotides in `REF`+`ALT` is 11 or less and only contains ACGT letters, then a reversible encoding is used:
    * the bit 1-4 bit indicate the number of bases in `REF`;
    * the bit 5-8 bit indicate the number of bases in `ALT`;
    * the following 11 groups of 2 bit represent each a base of `REF` followed by `ALT`.
    * the last bit (LSB) is set to 0;


The 64 bit VariantKey can be exported as a single 16 character hexadecimal string.  
The `CHROM` and `POS` sections of the VariantKey are sortable.  
The reversible encoding limit of 11 bases covers 99.64% (335,933,068 / 337,162,128) of the variants in the dbSNP GRCh37.p13.b150 VCF file. The remaining variants can be reversed using a lookup table.


## Input values

* **`CHROM`** - *chromosome*     : Identifier from the reference genome, no white-space or leading zeros permitted.

* **`POS`**   - *position*       : The reference position, with the 1<sup>st</sup> nucleotide having position 0.

* **`REF`**   - *reference allele* :
    String containing a sequence of [nucleotide letters](https://en.wikipedia.org/wiki/Nucleic_acid_notation).   
    The value in the `POS` field refers to the position of the first nucleotide in the string.

* **`ALT`**   - *alternate non-reference allele* : 
    String containing a sequence of [nucleotide letters](https://en.wikipedia.org/wiki/Nucleic_acid_notation).


## Binary file format for RSID-VariantKey index

The functions provided here allows fast search for RSID and VariantKey values from binary files
made of adjacent constant-length binary blocks sorted in ascending order.

The input binary files can be generated from a normalized VCF file using the `resources/tools/rsvk.sh`.  
The VCF file can be normalized using the `resources/tools/vcfnorm.sh` script.

The `rsvk.bin` file contains adjacent 12 bytes (96 bit) binary blocks with the following structure:

```
    00 01 02 03 04 05 06 07 08 09 10 11
    +---------+ +---------------------+
    |  RSID   | |     VARIANTKEY      |
    +---------+ +---------------------+
```


The `vkrs.bin` file contains adjacent 12 bytes (96 bit) binary blocks with the following structure:

```
    00 01 02 03 04 05 06 07 08 09 10 11
    +---------------------+ +---------+
    |      VARIANTKEY     | |  RSID   |
    +---------------------+ +---------+
```

 
## C Library

This project includes a Makefile that allows you to test and build the project in a Linux-compatible system with simple commands.  
All the artifacts and reports produced using this Makefile are stored in the *target* folder.  

To see all available options:
```
make help
```

To execute all the default test builds and generate reports in the current environment:
```
make test
```

To format the code (please use this command before submitting any pull request):
```
make format
```

To build a shared library:
```
make build
```


## Python Module

The python module is located in the `python` directory.

### Build

A shared library can be built using the following command inside the python directory (requires python-dev):

```
make python
```

### Usage Example

```
import variantkey as vk

vkey = vk.variantkey("X", 193330, "GCA", "G")
print(vkey)
# 13259012477095908224

s = vk.variantkey_string(vkey)
print(s)
# b'b801799941c61380'

v = vk.parse_variantkey_string(s)
print(v)
# 13259012477095908224

chrom, pos, refalt = vk.decode_variantkey(v)
print(chrom, pos, refalt)
# 23 193330 1103500160

c = vk.decode_chrom(chrom)
print(c)
# X

ref, alt, reflen, altlen = vk.decode_refalt(refalt)
print(ref, alt, reflen, altlen)
# GCA G 3 1
```


## GO Library

A go wrapper is located in the ```go``` directory.

### Test

Use the following commands to test the go wrapper and generate reports.

```
make golang
```
