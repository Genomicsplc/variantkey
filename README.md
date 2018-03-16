# VariantHash

*Genetic Variant Hash*

[![Master Build Status](https://secure.travis-ci.org/genomicsplc/varianthash.png?branch=master)](https://travis-ci.org/genomicsplc/varianthash?branch=master)
[![Master Coverage Status](https://coveralls.io/repos/genomicsplc/varianthash/badge.svg?branch=master&service=github)](https://coveralls.io/github/genomicsplc/varianthash?branch=master)

* **category**    Libraries
* **license**     MIT (see LICENSE)
* **link**        https://github.com/genomicsplc/varianthash


## Description

This project contains tools to generate and process Genetic Variant numerical representations.
internal numerical variant IDs that can be used to match and cross-reference genetic variants across different databases and sources.

**IMPORTANT**: This model assumes that the variants have been:

a. **decomposed** to convert multialleic variants to bialleic ones (`REF` and single `ALT`).  
This can be done in [VCF]() files using the [vt decompose](https://genome.sph.umich.edu/wiki/Vt#Decompose) tool.

b. **normalized** as in ["Unified representation of genetic variants" - Tan et al. 2015](https://academic.oup.com/bioinformatics/article/31/13/2202/196142).
This can be done in VCF files using the [vt normalize](https://genome.sph.umich.edu/wiki/Vt#Decompose) tool.

## VariantHash128

The full VariantHash is composed of 4 sections arranged in 128 bit:

    00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
    +---------+ +---------+ +---------+ +---------+
    | ASSBLY  | |  CHROM  | |   POS   | | REF_ALT |
    +---------+ +---------+ +---------+ +---------+
    |                VARIANT_HASH                 |
    +---------------------------------------------+

* **`ASSBLY`**  : 32 bit (4 bytes, 8 hex bytes) to represent the hash of Genome Assembly string.  
    The Genome Assembly string should be in the form used by [Genome Reference Consortium](https://www.ncbi.nlm.nih.gov/grc), including the patch number and build number separated by a dot. For example: `GRCh37.p13.b150`.  
    This value is reversible only using a lookup table, as the encoding is not reversible.

* **`CHROM`**   : 32 bit (4 bytes, 8 hex bytes) to represent the chromosome.  
    Chromosomes are always encoded as numbers.
    A function should be provided to convert chromosome strings to numbers and vice-versa.
    The default string conversion functions includes the encoding for human chromosomes from `1` to `22` plus `X=23`, `Y=24`, `MT=25`, `NA=0`.
    This value is reversible.
                
* **`POS`**     : 32 bit (4 bytes, 8 hex bytes) for the reference position (`POS`), with the 1<sup>st</sup> base having position 0.  

* **`REF_ALT`** : 32 bit (4 bytes, 8 hex bytes) for the encoding of the `REF` and `ALT` strings.  
    The encoding of this field depends on the tolat lenght of the `REF`+`ALT` string.  
    If the total number of nucleotides in `REF`+`ALT` is more then 5, then the "`REF_ALT`" string is encoded using an hashing algoritm and a lookup table is required to reverse the values.  
    If the total number of nucleotides in `REF`+`ALT` is 5 or less, then a reversible encoding is used:
    * the first 5 bit (0 to 4) are set to 0;
    * the bit 5 and 6 indicates the number of nucleotides in `REF` minus 1;
    * the following 5 groups of 5 bit represent each a nucleotide of `REF` followed by `ALT`.


The 128 bit VariantHash can be exported as a single 32 character hexadecimal string.  
The `CHROM` and `POS` 32 sections of the VariantHash key are sortable.


## VariantHash64

This is a short version of the VariantHash that supports up to 255 chromosomes and omit the genome assembly reference.

The short VariantHash is composed of 3 sections arranged in 64 bit:

    00          01 02 03 04 05 06 07
    +---------+ +---------+ +---------+
    |  CHROM  | |   POS   | | REF_ALT |
    +---------+ +---------+ +---------+
    |          VARIANT_HASH           |
    +---------------------------------+

* **`CHROM`**   : 8 bit (1 byte, 2 hex bytes) to represent the chromosome.  
    Chromosomes are always encoded as numbers.  
    The default string conversion functions includes the encoding for human chromosomes from `1` to `22` plus `X=23`, `Y=24`, `MT=25`, `NA=0`.  
    This value is reversible.
                
* **`POS`**     : 32 bit (4 bytes, 8 hex bytes) for the reference position (`POS`), with the 1<sup>st</sup> base having position 0.  

* **`REF_ALT`** : 24 bit (3 bytes, 6 hex bytes) for the encoding of the `REF` and `ALT` strings.  
    The encoding of this field depends on the tolat lenght of the `REF`+`ALT` string.  
    If the total number of nucleotides in `REF`+`ALT` is more then 4, then the "`REF_ALT`" string is encoded using an hashing algoritm and a lookup table is required to reverse the values.  
    If the total number of nucleotides in `REF`+`ALT` is 4 or less, then a reversible encoding is used:
    * the first 2 bit (0 and 1) are set to 0;
    * the bit 2 and 3 indicates the number of nucleotides in `REF` minus 1;
    * the following 4 groups of 5 bit represent each a nucleotide of `REF` followed by `ALT`.


The 64 bit VariantHash can be exported as a single 16 character hexadecimal string.  
The `CHROM` and `POS` sections of the VariantHash64 key are sortable.


## Input values

* **`ASSBLY`** - *genome Assembly* : String identifying the Genome Assembly (*only required for Varianthash128*).  
    The Genome Assembly string should be in the form used by [Genome Reference Consortium](https://www.ncbi.nlm.nih.gov/grc), including the patch number and build number separated by a dot. For example: `GRCh37.p13.b150`.  

* **`CHROM`** - *chromosome*     : Identifier from the reference genome, no white-space or leading zeros permitted.

* **`POS`**   - *position*       : The reference position, with the 1st base having position 0.

* **`REF`**   - *reference allele* :
    String containing a sequence of [nucleotide letters](https://en.wikipedia.org/wiki/Nucleic_acid_notation).   
    The value in the `POS` field refers to the position of the first nucleotide in the String.

* **`ALT`**   - *alternate non-reference allele* : 
    String containing a sequence of [nucleotide letters](https://en.wikipedia.org/wiki/Nucleic_acid_notation).


## Binary file format for RSID-VariantHash128 index

The functions provided here allows fast search for RSID and VariantHash128 values from binary files
made of adjacent constant-length binary blocks sorted in ascending order.

The input binary files can be generated using the rsidvar128.sh script in the resources/tools folder.

The rsid_varhash128.bin file contains adjacent 20 bytes binary blocks
with the following structure:

    00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19
    +---------+ +---------+ +---------+ +---------+ +---------+ 
    |  RSID   | | ASSBLY  | |  CHROM  | |   POS   | | REF_ALT |
    +---------+ +---------+ +---------+ +---------+ +---------+
                |                VARIANT_HASH                 |
                +---------------------------------------------+


The varhash128_rsid.bin file contains adjacent 20 bytes binary blocks
with the following structure:

    00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19
    +---------+ +---------+ +---------+ +---------+ +---------+
    | ASSBLY  | |  CHROM  | |   POS   | | REF_ALT | |  RSID   |
    +---------+ +---------+ +---------+ +---------+ +---------+
    |                VARIANT_HASH                 |
    +---------------------------------------------+


## Binary file format for RSID-VariantHash64 index

The functions provided here allows fast search for RSID and VariantHash64 values from binary files
made of adjacent constant-length binary blocks sorted in ascending order.

The input binary files can be generated using the rsidvar64.sh script in the resources/tools folder.

The rsid_varhash64.bin file contains adjacent 12 bytes binary blocks
with the following structure:

    00 01 02 03 04          05 06 07    08 09 10 11
    +---------+ +---------+ +---------+ +---------+ 
    |  RSID   | |  CHROM  | |   POS   | | REF_ALT |
    +---------+ +---------+ +---------+ +---------+
                |          VARIANT_HASH           |
                +---------------------------------+


The varhash64_rsid.bin file contains adjacent 12 bytes binary blocks
with the following structure:

    00          01 02 03 04 05 06 07    08 09 10 11
    +---------+ +---------+ +---------+ +---------+
    |  CHROM  | |   POS   | | REF_ALT | |  RSID   |
    +---------+ +---------+ +---------+ +---------+
    |          VARIANT_HASH           |
    +---------------------------------+


 
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

The python module is located in the ```python``` directory.

### Build

A shared library can be built using the following command inside the python directory (requires python-dev):

```
make build python pytest
```

To build and install:

```
cd python
sudo python setup.py install build_ext --include-dirs=../c/src
```

### Usage Example

```
import libpyvarianthash as vh

h = vh.variant_hash("6", 193330, "TCA", "TGTCG")
print(h[0], h[1], h[2])

s = vh.variant_hash_string(h[0], h[1], h[2])
print(s)

d = vh.decode_variant_hash_string("000000060002f332e683c0ceef5adee0")
print(d[0], d[1], d[2])
```


## GO Library

A go wrapper is located in the ```go``` directory.

### Test

Use the following commands to test the go wrapper and generate reports.

```
make golang
```
