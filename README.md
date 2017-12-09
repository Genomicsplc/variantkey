# VariantHash

*Genetic Variant Hash*

[![Master Build Status](https://secure.travis-ci.org/genomicsplc/varianthash.png?branch=master)](https://travis-ci.org/genomicsplc/varianthash?branch=master)
[![Master Coverage Status](https://coveralls.io/repos/genomicsplc/varianthash/badge.svg?branch=master&service=github)](https://coveralls.io/github/genomicsplc/varianthash?branch=master)

* **category**    Libraries
* **license**     MIT (see LICENSE)
* **link**        https://github.com/genomicsplc/varianthash


## Description

This project contains tools to generate and process a **Genetic Variant Hash**,
a variant ID that can be used to match and cross-reference genetic variants across different databases and sources.

The *Genetic Variant* Hash is composed of 3 sections that can be also used separately:

* **CHROM**   : 32 bits (8 hex bytes) to represent the chromosome.
                Chromosomes are always encoded as numbers.
                Non numerical human chromosomes (X, Y, XY, MT) are auomatically converted to numbers.
                For other species than human the string-to-number chromosome conversion should happen before calling the hashing function.

* **POS**     : 32 bits (8 hex bytes) for the reference position (POS), with the 1st base having position 0.

* **REF_ALT** : 64 bits (16 hex bytes) for the farmhash64 of the "REF_ALT" string.

Each Variant Hash is unique to a given *Genome Assembly Model* (species + build number).  
The full 128 bits can be exported as a single 32 character hexadecimal string.  
The first 64 bits of the Variant Hash forms a sortable key.


## Input values

* **CHROM** - *chromosome*     : Uppercase identifier from the reference genome, no white-space or leading zeros permitted.

* **POS**   - *position*       : The reference position, with the 1st base having position 0.

* **REF**   - *reference base* : Each base must be one of A,C,G,T,N (always in uppercase). Multiple bases are permitted.
                                 The value in the POS field refers to the position of the first base in the String.

* **ALT**   - *alternate base* : Non-reference allele.
                                 Options are base Strings made up of the bases A,C,G,T,N,*,  (always in uppercase).
                                 The ‘*’ allele is reserved to indicate that the allele is missing due to a upstream deletion.
                                 If there are no alternative alleles, then the missing value should be used.

## Binary file format for RSID-VariantHash index

The functions provided here allows to search RSIDs and VariantHashes from binary files
made of adjacent constant-length binary blocks sorted in ascending order.

The input binary files can be generated using some open source tools:

* split multialleic variants in dbSNP

```
    time bcftools norm --multiallelics -any --output ~/Downloads/dbSNP_All_20151104_split.vcf ~/Downloads/All_20151104.vcf.gz
```

* create RSID to Variant map

```
    time bcftools query -f '%RSID_HEX%VARIANT_HASH_HEX\n' ~/Downloads/dbSNP_All_20151104_split.vcf > ~/Downloads/rsid_varhash.txt
```

* create Variant to RSID map

```
    time bcftools query -f '%VARIANT_HASH_HEX%RSID_HEX\n' ~/Downloads/dbSNP_All_20151104_split.vcf > ~/Downloads/varhash_rsid.txt
```

* sort the maps

```
    LC_ALL=C time sort --parallel=4 --output=rsid_varhash.sorted.txt rsid_varhash.txt
    LC_ALL=C time sort --parallel=4 --output=varhash_rsid.sorted.txt varhash_rsid.txt
```

* convert the maps to binary format

```
    time xxd -r -p rsid_varhash.sorted.txt rsid_varhash.bin
    time xxd -r -p varhash_rsid.sorted.txt varhash_rsid.bin
```


The rsid_varhash.bin file contains adjacent 20 bytes binary blocks
with the following structure:

    01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20
    +---------+ +---------+ +---------+ +---------------------+
    |  RSID   | |  CHROM  | |   POS   | |    REF_ALT_HASH     |
    +---------+ +---------+ +---------+ +---------------------+
                +---------------------------------------------+
                |                VARIANT_HASH                 |
                +---------------------------------------------+


The varhash_rsid.bin file contains adjacent 20 bytes binary blocks
with the following structure:

    01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20
    +---------+ +---------+ +---------------------+ +---------+
    |  CHROM  | |   POS   | |    REF_ALT_HASH     | |  RSID   |
    +---------+ +---------+ +---------------------+ +---------+
    +---------------------------------------------+
    |                VARIANT_HASH                 |
    +---------------------------------------------+


 
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
