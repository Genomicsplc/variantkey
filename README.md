# VariantKey

*Genetic Variant Key*

[![Master Build Status](https://secure.travis-ci.org/genomicsplc/variantkey.png?branch=master)](https://travis-ci.org/genomicsplc/variantkey?branch=master)
[![Master Coverage Status](https://coveralls.io/repos/genomicsplc/variantkey/badge.svg?branch=master&service=github)](https://coveralls.io/github/genomicsplc/variantkey?branch=master)

* **category**    Libraries
* **license**     MIT (see LICENSE)
* **link**        https://github.com/genomicsplc/variantkey


## Description

This project contains tools to generate and process a 64 bit Genetic Variant Key for Human DNA.  

The VariantKey is sortable for chromosome and position, and it is also fully reversible for variants with up to 5 nucleotides between Reference and Alternate alleles. It can be used as a matching key for matching and merging human genetic variants across different databases and sources.

**IMPORTANT**: This model assumes that the variants have been:

a. **decomposed** to convert multialleic variants to bialleic ones (`REF` and single `ALT`).  
This can be done in [VCF]() files using the [vt decompose](https://genome.sph.umich.edu/wiki/Vt#Decompose) tool with the `-s` option.

b. **normalized** as in ["Unified representation of genetic variants" - Tan et al. 2015](https://academic.oup.com/bioinformatics/article/31/13/2202/196142).  
This can be done in VCF files using the [vt normalize](https://genome.sph.umich.edu/wiki/Vt#Decompose) tool.

## VariantKey Format

The VariantKey is composed of 3 sections arranged in 64 bit:

```
    0       8      16      24      32       40      48      56      64
    01234 5678901234567890123456789012 3456789012345678901234567890123
    +---+ +--------------------------+ +-----------------------------+
    |CHR| |           POS            | |           REF+ALT           |
    +---+ +--------------------------+ +-----------------------------+
    |                          VARIANTKEY                            |
    +----------------------------------------------------------------+
```

* **`CHR`**   : 5 bit to represent the chromosome.  
    `MASK: [11111000 00000000 00000000 00000000 00000000 00000000 00000000 00000000]`  
    Chromosomes are always encoded as numbers.  
    The default string conversion functions includes the encoding for human chromosomes from `1` to `22` plus `X=23`, `Y=24`, `MT=25`, `NA=0`.  
    This value is always reversible.
                
* **`POS`**     : 28 bit for the reference position (`POS`), with the 1<sup>st</sup> nucleotide having position 0.  
    `MASK: [00000111 11111111 11111111 11111111 10000000 00000000 00000000 00000000]`  
    The largest human chromosome contains 249M base pairs that can be easily contained in 28 bit (2^28 = 268,435,456).
                  

* **`REF+ALT`** : 31 bit for the encoding of the `REF` and `ALT` strings.  
    `MASK: [00000000 00000000 00000000 00000000 01111111 11111111 11111111 11111111]`  
    The encoding of this field depends on the tolat lenght of the `REF`+`ALT` string.  
    If the total number of nucleotides in `REF`+`ALT` is more then 5, then the "`REF+ALT`" string is encoded using an hashing algoritm and a lookup table is required to reverse the values.  
    If the total number of nucleotides in `REF`+`ALT` is 5 or less, then a reversible encoding is used:
    * the first 2 bit are set to 0;
    * the 3<sup>th</sup> and 4<sup>th</sup> bit indicates the number of nucleotides in `REF` minus 1;
    * the 5<sup>th</sup> and 6<sup>th</sup> indicates the number of nucleotides in `ALT` minus 1;
    * the following 5 groups of 5 bit represent each a nucleotide of `REF` followed by `ALT`.


The 64 bit VariantKey can be exported as a single 16 character hexadecimal string.  
The `CHROM` and `POS` sections of the VariantKey are sortable.


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

The input binary files can be generated using the rsidvar.sh script in the resources/tools folder.

The `rsid_variantkey.bin` file contains adjacent 12 bytes (96 bit) binary blocks with the following structure:

```
    00 01 02 03 04 05 06 07 08 09 10 11
    +---------+ +---------------------+
    |  RSID   | |     VARIANTKEY      |
    +---------+ +---------------------+
```


The `variantkey_rsid.bin` file contains adjacent 12 bytes (96 bit) binary blocks with the following structure:

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
make build python pytest
```

### Usage Example

```
import variantkey as vh

vk = vh.variantkey("6", 193330, "TCA", "TGTCG")
print(vk)

s = vh.variantkey_string(vk)
print(s)

d = vh.decode_variantkey(vk)
print(d[0], d[1], d[2])
```


## GO Library

A go wrapper is located in the ```go``` directory.

### Test

Use the following commands to test the go wrapper and generate reports.

```
make golang
```
