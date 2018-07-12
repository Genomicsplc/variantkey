# VariantKey

*64 bit Encoding for Human Genetic Variants*

[![Master Build Status](https://secure.travis-ci.org/Genomicsplc/variantkey.png?branch=master)](https://travis-ci.org/Genomicsplc/variantkey?branch=master)
[![Master Coverage Status](https://coveralls.io/repos/Genomicsplc/variantkey/badge.svg?branch=master&service=github)](https://coveralls.io/github/Genomicsplc/variantkey?branch=master)

* **category**    Libraries
* **license**     MIT (see LICENSE)
* **copyright**   2017-2018 GENOMICS plc
* **link**        https://github.com/Genomicsplc/variantkey


## Description

A genetic variant is often referred as a single entity but, for a given genome assembly, it is usually represented as a set of four components with variable length: *chromosome*, *position*, *reference* and *alternate* alleles. There is no guarantee that these components are represented in a consistent way across different data sources. The numerical dbSNP reference record representation (rs#) only covers a subset of all possible variants and it is not bijective. Processing variant-based data can be really inefficient due to the necessity to perform four different comparison operations for each variant, three of which are string comparisons. Working with strings, in contrast of numbers, poses extra challenges on memory allocation and data-representation.

**VariantKey**, a novel reversible numerical encoding schema for human genetic variants, overcomes these limitations by allowing to process variants as a single 64 bit numeric entities while preserving the ability to be searched and sorted per chromosome and position.

The individual components of short variants (up to 11 bases between REF and ALT alleles) can be directly read back from the VariantKey, while long variants requires a lookup table to retrieve the reference and alternate allele strings.

This software library can be used to generate and reverse VariantKeys.


## Quick Start

This project includes a Makefile that allows you to test and build the project in a Linux-compatible system with simple commands.

To see all available options, from the project root type:

```
make help
```

To build all the VriantKey versions inside a Docker container (requires Docker):

```
make dbuild
```

An arbitrary make target can be executed inside a [Docker](https://www.docker.com/) container by specifying the `MAKETARGET` parameter:

```
MAKETARGET='build' make dbuild
```
The list of make targets can be obtained by typing ```make```


The base Docker building environment is defined in the following Dockerfile:

```
resources/Docker/Dockerfile.dev
```

To build and test only a specific language version, `cd` into the language directory and use the `make` command.
For example:

```
cd c
make test
```


## Human Genetic Variant Definition

In this context, the human genetic variant for a given genome assembly is defined as the set of four components compatible with the VCF format:

* **`CHROM`** - chromosome: An identifier from the reference genome. It only has 26 valid values: autosomes from 1 to 22, the sex chromosomes X=23 and Y=24, mitochondria MT=25 and a symbol NA=0 to indicate missing data.
* **`POS`** - position: The reference position in the chromosome, with the 1st nucleotide having position 0. The largest expected value is 247,199,718 to represent the last base pair in the chromosome 1.
* **`REF`** - reference allele: String containing a sequence of reference nucleotide letters. The value in the POS field refers to the position of the first nucleotide in the String.
* **`ALT`** - alternate allele: Single alternate non-reference allele. String containing a sequence of nucleotide letters. Multialleic variants must be decomposed in individual bialleic variants.


## Variant Decomposition and Normalization

The *VariantKey* model assumes that the variants have been decomposed and normalized.

### Decomposition

In the common *Variant Call Format* (VCF) the alternate field can contain comma-separated strings for multialleic variants, while in this context we only consider bialleic variants to allow for allelic comparisons between different data sets.

For example, the multialleic variant:

```
    {CHROM=1, POS=3759889, REF=TA, ALT=TAA,TAAA,T}
```

can be decomposed as three bialleic variants:

```
    {CHROM=1, POS=3759889, REF=TA, ALT=TAA}
    {CHROM=1, POS=3759889, REF=TA, ALT=TAAA}
    {CHROM=1, POS=3759889, REF=TA, ALT=T}
```

In VCF files the decomposition from multialleic to bialleic variants can be performed using the '[vt](https://genome.sph.umich.edu/wiki/Vt#Decompose)' software tool with the command:

```
    vt decompose -s source.vcf -o decomposed.vcf
```

The `-s` option (smart decomposition) splits up `INFO` and `GENOTYPE` fields that have number counts of `R` and `A` appropriately.


#### Example:

* input

```
  #CHROM  POS     ID   REF     ALT         QUAL   FILTER  INFO                  FORMAT    S1                                      S2
  1       3759889 .    TA      TAA,TAAA,T  .      PASS    AF=0.342,0.173,0.037  GT:DP:PL  1/2:81:281,5,9,58,0,115,338,46,116,809  0/0:86:0,30,323,31,365,483,38,291,325,567
```

* output

```
  #CHROM  POS     ID   REF     ALT         QUAL   FILTER  INFO                                                 FORMAT   S1               S2
  1       3759889 .    TA      TAA         .      PASS    AF=0.342;OLD_MULTIALLELIC=1:3759889:TA/TAA/TAAA/T    GT:PL    1/.:281,5,9      0/0:0,30,323
  1       3759889 .    TA      TAAA        .      .       AF=0.173;OLD_MULTIALLELIC=1:3759889:TA/TAA/TAAA/T    GT:PL    ./1:281,58,115   0/0:0,31,483
  1       3759889 .    TA      T           .      .       AF=0.037;OLD_MULTIALLELIC=1:3759889:TA/TAA/TAAA/T    GT:PL    ./.:281,338,809  0/0:0,38,567
```

### Normalization

A normalization step is required to ensure a consistent and unambiguous representation of variants.
As shown in Fig. 1, there are multiple ways to represent the same variant, but only one can be considered "normalized" as defined by [Tan et al., 2015](https://doi.org/10.1093/bioinformatics/btv112):

* A variant representation is normalized if and only if it is left aligned and parsimonious.
* A variant representation is left aligned if and only if its base position is smallest among all potential representations having the same allele length and representing the same variant.
* A variant representation is parsimonious if and only if the entry has the shortest allele length among all VCF entries representing the same variant.

Example of VCF entries representing the same variant:

```
                               VARIANT    POS: 0
                                          REF: GGGCACACACAGGG
                                          ALT: GGGCACACAGGG
    
                      NOT-LEFT-ALIGNED    POS:      5
                                          REF:      CAC
                                          ALT:      C
    
    NOT-LEFT-ALIGNED, NOT-PARSIMONIOUS    POS:   2
                                          REF:   GCACA
                                          ALT:   GCA
    
                      NOT-PARSIMONIOUS    POS:  1
                                          REF:  GGCA
                                          ALT:  GG
    
                            NORMALIZED    POS:   2
                                          REF:   GCA
                                          ALT:   G
```

In VCF files the variant normalization can be performed using the [vt](https://genome.sph.umich.edu/wiki/Vt#Normalization) software tool with the command:

```
    vt normalize decomposed.vcf -m -r genome.fa -o normalized.vcf
```

#### Normalization Function

Individual bialleic variants can be normalized using the `normalize_variant` function provided by this library.  
The `normalize_variant` function has the ability to check the consistency of the variant against the genome reference and swap and/or flip the alleles if required.

The genome reference binary file can be obtained from a FASTA file using the `resources/tools/fastabin.sh` script.
This script only extract the first 25 sequences for chromosomes 1 to 22, X, Y and MT.  
The first line of the binary fasta file contains the index composed by 26 blocks of 32 bit numbers, one for each of the 25 chromosomes plus one to indicate the end of the file.
Each index number represents the file byte offset of the corresponding chromosome sequence.
The index is followed by 25 lines, one for each chromosome sequence.


## VariantKey Format

The VariantKey is composed of 3 sections arranged in 64 bit:

```
    1      8      16      24      32      40      48      56      64
    |      |       |       |       |       |       |       |       |
    0123456789012345678901234567890123456789012345678901234567890123
    CHROM|<--------- POS ----------->||<-------- REF+ALT --------->|
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
    The chromosome is encoded as unsigned integer number: 1 to 22, X=23, Y=24, MT=25, NA=0.
    This section is 5 bit long, so it can store up to 2^5=32 symbols, enough to contain the required 26 chromosome symbols.
    The largest value is: 25 dec = 19 hex = 11001 bin.
                
* **`POS`**     : 28 bit for the reference position (`POS`), with the 1<sup>st</sup> nucleotide having position 0.

    ```
    0    5                             32
    |    |                              |
    00000111 11111111 11111111 11111111 10000000 00000000 00000000 00000000
         |                              |
        MSB                            LSB
    ```
    This section is 28 bit long, so it can store up to 2^28=268,435,456 symbols, enough to contain the maximum position 247,199,718 found on the largest human chromosome.
                  

* **`REF+ALT`** : 31 bit for the encoding of the `REF` and `ALT` strings.

    ```
    0                                   33                               63
    |                                    |                                |
    00000000 00000000 00000000 00000000 01111111 11111111 11111111 11111111
                                         |                                |
                                        MSB                              LSB
    ```
    This section allow two different type of encodings:

    * **Non-reversible encoding**

        If the total number of nucleotides between REF and ALT is more then 11, or if any of the alleles contains nucleotide letters other than base A, C, G and T, then the LSB (least significant bit) is set to 1 and the remaining 30 bit are filled with an hash value of the REF and ALT strings.  
        The hash value is calulated using a custom fast non-cryptographic algorithm based on MurmurHash3.  
        A lookup table is required to reverse the REF and ALT values.  
        In the normalized dbSNP VCF file GRCh37.p13.b150 there are only 0.365% (1229769 / 337162128) variants that requires this encoding. Amongst those, the maximum number of variants that share the same chromosome and position is 15. With 30 bit the probability of hash collision is approximately 10<sup>-7</sup> for 15 elements, 10<sup>-6</sup> for 46 and 10<sup>-5</sup> for 146.

    * **Reversible encoding**

        If the total number of nucleotides between REF and ALT is 11 or less, and they only contain base letters A, C, G and T, then the LSB is set to 0 and the remaining 30 bit are used as follows:
        * bit 1-4 indicate the number of bases in REF - the capacity of this section is 2^4=16; the maximum expected value is 10 dec = 1010 bin;
        * bit 5-8 indicate the number of bases in ALT - the capacity of this section is 2^4=16; the maximum expected value is 10 dec = 1010 bin;
        * the following 11 groups of 2 bit are used to represent REF bases followed by ALT (A = 0 dec = 00 bin, C = 1 dec = 01 bin, G = 2 dec = 10 bin, T = 4 dec = 11 bin).  
        This encoding covers 99.635% of the variants in the normalized dbSNP VCF file GRCh37.p13.b150.
        
        Examples:
        
        ```
        REF     ALT        REF+ALT BINARY ENCODING
        A       G          0001 0001 00 10 00 00 00 00 00 00 00 00 00 0
        GGG     GA         0011 0010 10 10 10 10 00 00 00 00 00 00 00 0
        ACGT    CGTACGT    0100 0111 00 01 10 11 01 10 11 00 01 10 11 0
                           |                                          |
                         33 (MSB)                                   63 (LSB)
        ```

### VariantKey Properties

* Sorting the VariantKey is equivalent of sorting by CHROM and POS.
* The 64 bit VariantKey can be exported as a single 16 character hexadecimal string.
* Sorting the hexadecimal representation of VariantKey in alphabetical order is equivalent of sorting the VariantKey numerically.
* Comparing two variants by VariantKey only requires comparing two numbers, a very well optimized operation in current computer architectures. In contrast, comparing two normalized variants in VCF format requires comparing one numbers and three strings.
* VariantKey can be used as a main database key to index data by "variant". This simplify common searching, merging and filtering operations.
* All types of database joins between two data sets (inner, left, right and full) can be easily performed using the VariantKey as index.
* When CHROM, REF and ALT are the only strings in a table, replacing them with VariantKey allows to work with numeric only tables with obvious advantages. This also allows to represent the data in a compact binary format where each column uses a fixed number of bit or bytes, with the ability to perform a quick binary search algorithm on the first sorted column.


## Input values

* **`CHROM`** - *chromosome*     : Identifier from the reference genome, no white-space permitted.

* **`POS`**   - *position*       : The reference position, with the 1<sup>st</sup> nucleotide having position 0.

* **`REF`**   - *reference allele* :
    String containing a sequence of [nucleotide letters](https://en.wikipedia.org/wiki/Nucleic_acid_notation).   
    The value in the `POS` field refers to the position of the first nucleotide in the string.

* **`ALT`**   - *alternate non-reference allele* : 
    String containing a sequence of [nucleotide letters](https://en.wikipedia.org/wiki/Nucleic_acid_notation).


## Binary file format for lookup tables 

The input binary lookup-table files can be generated from a normalized VCF file using the `resources/tools/vkhexbin.sh`.  
The VCF file can be normalized using the `resources/tools/vcfnorm.sh` script.  
The `vkhexbin.sh` requires [bcftools](https://github.com/samtools/bcftools) compiled with the plugins in `resources/bcftools/plugins` folder.

* **`rsvk.bin`**  
Lookup table to retrieve VariantKey from rsID.
The file contains adjacent 12 bytes (96 bit) binary blocks with the following structure:

```
    00 01 02 03 04 05 06 07 08 09 10 11
    +---------+ +---------------------+
    |  RSID   | |     VARIANTKEY      |
    +---------+ +---------------------+
```

* **`vkrs.bin`**  
Lookup table to retrieve rsID from VariantKey.
The file contains adjacent 12 bytes (96 bit) binary blocks with the following structure:

```
    00 01 02 03 04 05 06 07 08 09 10 11
    +---------------------+ +---------+
    |      VARIANTKEY     | |  RSID   |
    +---------------------+ +---------+
```

* **`vknr.bin`**  
Lookup table to retrieve the original REF and ALT string for the non-reversible VariantKey.
The binary file has the following format :

```
    [8 BYTE VARIANTKEY][8 BYTE VALUE ADDRESS]
    ...
    [1 BYTE REF LENGTH][1 BYTE ALT LENGTH][REF STRING][ALT STRING]
    ...
    [4 BYTE NUM VARIANTS]
```


## C Library

This project includes a Makefile that allows you to test and build the project in a Linux-compatible system with simple commands.  
All the artifacts and reports produced using this Makefile are stored in the *target* folder.  

* To see all available options:
```
make help
```
* To execute all the default test builds and generate reports in the current environment:
```
make test
```
* To format the code (please use this command before submitting any pull request):
```
make format
```
* To build a shared library:
```
make build
```

### Command-Line tool

The code inside the `c/vk` folder is used to generate the `vk` command line tool.  
This tools requires the pre-normalized positional arguments `CHROM`, `POS`, `REF`, `ALT` and returns the VariantKey in hexadecimal representation.


## GO Library

A go wrapper is located in the `go` directory.

### Test

Use the following commands to test the go wrapper and generate reports.

```
make go
```


## Python Module

The python module is located in the `python` directory.

### Build

A Conda package can be built using the following command:

```
make python
```

### Usage Example

```
#!/usr/bin/env python3

import variantkey as vk

vkey = vk.variantkey(b"X", 193330, b"GCA", b"G")
print(vkey)
# 13259012476408233984

vkrange = vk.variantkey_range(15, 12002028, 12152133)
print(vkrange)
# (8672685443424190464, 8673007793604657151)

s = vk.variantkey_hex(vkey)
print(s)
# b'b801799918c90000'

v = vk.parse_variantkey_hex(s)
print(v)
# 13259012476408233984

chrom, pos, ref, alt, sizeref, sizealt, slen = vk.reverse_variantkey(v)
print(chrom, pos, ref, alt, sizeref, sizealt, slen)
# b'X' 193330 b'GCA' b'G' 3 1 4

chrom, pos, refalt = vk.decode_variantkey(v)
print(chrom, pos, refalt)
# 23 193330 415825920

c = vk.decode_chrom(chrom)
print(c)
# b'X'

ref, alt, reflen, altlen = vk.decode_refalt(refalt)
print(ref, alt, reflen, altlen)
# b'GCA' b'G' 3 1
```

## R Module (limited support)

Use the following command to build the R wrapper.

```
make r
```

In R the VariantKey is represented as hexadecimal string because there is no native support for 64 bit unsigned integers in R.
Alternatively it is possible to use the encoding of the individual components (i.e. CHROM, POS and REF+ALT).

### Usage Example

```
#!/usr/bin/env Rscript

args <- commandArgs(trailingOnly = F)  
script.path <- normalizePath(dirname(sub("^--file=", "", args[grep("^--file=", args)])))
wrapper.variantkey <- paste(script.path, "/variantkey.R", sep="")

source(wrapper.variantkey)

vkey <- VariantKey("X", 193330, "GCA", "G")
print(vkey)
# [1] "b801799918c90000"

var <- ReverseVariantKey(vkey)
print(var)
# $CHROM
# [1] "X"
# 
# $POS
# [1] 193330
# 
# $REF
# [1] "GCA"
# 
# $ALT
# [1] "G"
# 
# $SIZE_REF
# [1] 3
# 
# $SIZE_ALT
# [1] 1

vkrange <- VariantKeyRange(15, 12002028, 12152133)
print(vkrange)
# $MIN
# [1] "785b917600000000"
# 
# $MAX
# [1] "785cb6a2ffffffff"

cc <- EncodeChrom("MT")
print(cc)
# [1] 25

dc <- DecodeChrom(25)
print(dc)
# [1] "MT"

era <- EncodeRefAlt("A", "T")
print(era)
# [1] 144179200

dra <- DecodeRefAlt(144179200)
print(dra)
#$REF
#[1] "A"
#
#$ALT
#[1] "T"
#
#$REF_LEN
#[1] 1
#
#$ALT_LEN
#[1] 1
```


## Javascript library (limited support)

Use the following command to test and minify the Javascript implementation.

```
make javascript
```
