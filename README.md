# VariantKey

*64 bit Encoding for Human Genetic Variants*

[![Master Build Status](https://secure.travis-ci.org/Genomicsplc/variantkey.png?branch=master)](https://travis-ci.org/Genomicsplc/variantkey?branch=master)
[![Master Coverage Status](https://coveralls.io/repos/Genomicsplc/variantkey/badge.svg?branch=master&service=github)](https://coveralls.io/github/Genomicsplc/variantkey?branch=master)

* **category**    Libraries
* **author**      Nicola Asuni
* **copyright**   2017-2018 [GENOMICS plc](https://www.genomicsplc.com/)
* **license**     MIT (see LICENSE)
* **link**        https://github.com/Genomicsplc/variantkey


## TOC

* [Description](#description)
* [Quick Start](#quickstart)
* [Human Genetic Variant Definition](#hgvdefinition)
* [Variant Decomposition and Normalization](#decompandnorm)
    * [Decomposition](#decomposition)
    * [Normalization](#normalization)
        * [Normalization Function](#normfunc)
* **[VariantKey Format](#vkformat)**
    * [VariantKey Properties](#vkproperties)
* [VariantKey Input values](#vkinput)
* **[RegionKey](#regionkey)**
* [Encoding String IDs](#esid)
* [Binary file formats for lookup tables](#binaryfiles)
* [C Library](#clib)
* [GO Library](#golib)
* [Python Module](#pythonlib)
* [Python Class](#pythonclass)
* [R Module](#rlib)
* [Javascript library](#jslib)


<a name="description"></a>
## Description

A genetic variant is often referred as a single entity but, for a given genome assembly, it is usually represented as a set of four components with variable length: *chromosome*, *position*, *reference* and *alternate* alleles. There is no guarantee that these components are represented in a consistent way across different data sources. The numerical *dbSNP* reference record representation (*rs#*) only covers a subset of all possible variants and it is not bijective. Processing variant-based data can be really inefficient due to the necessity to perform four different comparison operations for each variant, three of which are string comparisons. Working with strings, in contrast of numbers, poses extra challenges on memory allocation and data-representation.

**VariantKey**, a novel reversible numerical encoding schema for human genetic variants, overcomes these limitations by allowing to process variants as a single 64 bit numeric entities while preserving the ability to be searched and sorted per chromosome and position.

The individual components of short variants (up to 11 bases between `REF` and `ALT` alleles) can be directly read back from the VariantKey, while long variants requires a lookup table to retrieve the reference and alternate allele strings.

The [VariantKey Format](#vkformat) doesn't represent universal codes, it only encodes `CHROM`, `POS`, `REF` and `ALT`, so each code is unique for a given reference genome. The direct comparisons of two VariantKeys makes sense only if they both refer to the same genome reference.

This software library can be used to generate and reverse VariantKeys.



<a name="quickstart"></a>
## Quick Start

This project includes a Makefile that allows you to test and build the project in a Linux-compatible system with simple commands.

To see all available options, from the project root type:

```
make help
```

To build all the VariantKey versions inside a Docker container (requires Docker):

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

<a name="hgvdefinition"></a>
## Human Genetic Variant Definition

In this context, the human genetic variant for a given genome assembly is defined as the set of four components compatible with the VCF format:

* **`CHROM`** - chromosome: An identifier from the reference genome. It only has 26 valid values: autosomes from 1 to 22, the sex chromosomes X=23 and Y=24, mitochondria MT=25 and a symbol NA=0 to indicate missing data.
* **`POS`** - position: The reference position in the chromosome, with the first nucleotide having position 0. The largest expected value is 247,199,718 to represent the last base pair in the chromosome 1.
* **`REF`** - reference allele: String containing a sequence of reference nucleotide letters. The value in the POS field refers to the position of the first nucleotide in the String.
* **`ALT`** - alternate allele: Single alternate non-reference allele. String containing a sequence of nucleotide letters. Multiallelic variants must be decomposed in individual biallelic variants.


<a name="decompandnorm"></a>
## Variant Decomposition and Normalization

The *VariantKey* model assumes that the variants have been decomposed and normalized.

<a name="decomposition"></a>
### Decomposition

In the common *Variant Call Format* (VCF) the alternate field can contain comma-separated strings for multiallelic variants, while in this context we only consider biallelic variants to allow for allelic comparisons between different data sets.

For example, the multiallelic variant:

```
    {CHROM=1, POS=3759889, REF=TA, ALT=TAA,TAAA,T}
```

can be decomposed as three biallelic variants:

```
    {CHROM=1, POS=3759889, REF=TA, ALT=TAA}
    {CHROM=1, POS=3759889, REF=TA, ALT=TAAA}
    {CHROM=1, POS=3759889, REF=TA, ALT=T}
```

In VCF files the decomposition from multiallelic to biallelic variants can be performed using the '[vt](https://genome.sph.umich.edu/wiki/Vt#Decompose)' software tool with the command:

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

<a name="normalization"></a>
### Normalization

A normalization step is required to ensure a consistent and unambiguous representation of variants.
As shown in the following example, there are multiple ways to represent the same variant, but only one can be considered "normalized" as defined by [Tan et al., 2015](https://doi.org/10.1093/bioinformatics/btv112):

* *A variant representation is normalized if and only if it is left aligned and parsimonious.*
* *A variant representation is left aligned if and only if its base position is smallest among all potential representations having the same allele length and representing the same variant.*
* *A variant representation is parsimonious if and only if the entry has the shortest allele length among all VCF entries representing the same variant.*

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

<a name="normfunc"></a>
#### Normalization Function

Individual biallelic variants can be normalized using the `normalize_variant` function provided by this library.  

The `normalize_variant` function first checks if the reference allele matches the genome reference.
The match is considered valid and consistent if there is a perfect letter-by-letter match, and valid but not consistent if one or more letter matches an equivalent one. The equivalent letters are defined as follows [[Cornish-Bowden, 1984](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC341218/)]:

```
    SYMBOL | DESCRIPTION                   | BASES   | COMPLEMENT
    -------|-------------------------------|---------|-----------
       A   | Adenine                       | A       |  T
       C   | Cytosine                      |   C     |  G
       G   | Guanine                       |     G   |  C
       T   | Thymine                       |       T |  A
       W   | Weak                          | A     T |  W
       S   | Strong                        |   C G   |  S
       M   | aMino                         | A C     |  K
       K   | Keto                          |     G T |  M
       R   | puRine                        | A   G   |  Y
       Y   | pYrimidine                    |   C   T |  R
       B   | not A (B comes after A)       |   C G T |  V
       D   | not C (D comes after C)       | A   G T |  H
       H   | not G (H comes after G)       | A C   T |  D
       V   | not T (V comes after T and U) | A C G   |  B
       N   | aNy base (not a gap)          | A C G T |  N
    -------|-------------------------------|---------|----------
```

If the reference allele is not valid, the `normalize_variant` function tries to find a reference match with one of the following variant transformations:

* **swap** the reference and alternate alleles - *sometimes it is not clear which one is the reference and which one is the alternate allele*.
* **flip** the alleles letters (use the **complement** letters) - *sometimes the alleles refers to the other DNA strand*.
* **swap** and **flip**.

Note that the *swap* and *flip* processes can lead to false positive cases, especially when considering *Single Nucleotide Polymorphisms* (SNPs). The return code of the `normalize_variant` function can be used to discriminate or discard variants that are not consistent.

If the variant doesn't match the genome reference, then the original variant is returned with an error code.

If both alleles have length 1, the normalization is complete and the variant is returned.
Otherwise, a custom implementation of the [vt normalization](https://genome.sph.umich.edu/wiki/Vt#Normalization) algorithm is applied:

```
while break, do
    if any of the alelles is empty and the position is greater than zero, then
        extend both alleles one letter to the left using the nucleotide in the corresponding genome reference position;
    else
        if both alleles end with the same letter and they have length 2 or more, then
            truncate the rightmost letter of each allele;
        else
            break (exit the while loop);

while both alleles start with the same letter and have length 2 or more, do
    truncate leftmost letter of each allele;
```

The genome reference binary file can be obtained from a FASTA file using the `resources/tools/fastabin.sh` script.
This script extracts the first 25 sequences for chromosomes `1` to `22`, `X`, `Y` and `MT`.  
The first line of the binary fasta file contains the index composed by 26 blocks of 32 bit numbers, one for each of the 25 chromosomes plus one to indicate the end of the file.
Each index number represents the file byte offset of the corresponding chromosome sequence.
The index is followed by 25 lines, one for each chromosome sequence.


<a name="vkformat"></a>
## VariantKey Format

For a given reference genome the VariantKey format encodes a *Human Genetic Variant* (`CHROM`, `POS`, `REF` and `ALT`) as 64 bit unsigned integer number (8 bytes or 16 hexadecimal symbols).
If the variant has not more than 11 bases between `REF` and `ALT`, the correspondent VariantKey can be directly reversed to get back the individual `CHROM`, `POS`, `REF` and `ALT` components.
If the variant has more than 11 bases, or non-base nucleotide letters are contained in `REF` or `ALT`, the VariantKey can be fully reversed with the support of a binary lookup table.


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
    This section is 5 bit long, so it can store up to 2<sup>5</sup>=32 symbols, enough to contain the required 26 canonical chromosome symbols.  
    The largest value is: 25 dec = 19 hex = 11001 bin.  
    Values from 26 to 31 are currently reserved. They will be used to indicate 6 alternative modes to interpret the remaining 59 bit. For instance, one of these values can be used to indicate the encoding of variants that occurs in non-canonical contigs.

* **`POS`**     : 28 bit for the reference position (`POS`), with the first nucleotide having position 0.

    ```
    0    5                             32
    |    |                              |
    00000111 11111111 11111111 11111111 10000000 00000000 00000000 00000000
         |                              |
        MSB                            LSB
    ```
    This section is 28 bit long, so it can store up to 2<sup>28</sup>=268,435,456 symbols, enough to contain the maximum position 247,199,718 found on the largest human chromosome.

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

        If the total number of nucleotides between `REF` and `ALT` is more then 11, or if any of the alleles contains nucleotide letters other than base `A`, `C`, `G` and `T`, then the LSB (least significant bit) is set to 1 and the remaining 30 bit are filled with an hash value of the `REF` and `ALT` strings.  
        The hash value is calulated using a custom fast non-cryptographic algorithm based on [MurmurHash3](https://github.com/aappleby/smhasher/wiki/MurmurHash3).  
        A lookup table is required to reverse the `REF` and `ALT` values.  
        In the normalized dbSNP VCF file GRCh37.p13.b150 there are only 0.365% (1229769 / 337162128) variants that requires this encoding. Amongst those, the maximum number of variants that share the same chromosome and position is 15. With 30 bit the probability of hash collision is approximately 10<sup>-7</sup> for 15 elements, 10<sup>-6</sup> for 46 and 10<sup>-5</sup> for 146.

    * **Reversible encoding**

        If the total number of nucleotides between `REF` and `ALT` is 11 or less, and they only contain base letters `A`, `C`, `G` and `T`, then the LSB is set to 0 and the remaining 30 bit are used as follows:
        * bit 1-4 indicates the number of bases in `REF` - the capacity of this section is 2<sup>4</sup>=16; the maximum expected value is 10 dec = 1010 bin;
        * bit 5-8 indicates the number of bases in `ALT` - the capacity of this section is 2<sup>4</sup>=16; the maximum expected value is 10 dec = 1010 bin;
        * the following 11 groups of 2 bit are used to represent `REF` bases followed by `ALT`, with the following encoding:
            * `A` = 0 dec = 00 bin;
            * `C` = 1 dec = 01 bin;
            * `G` = 2 dec = 10 bin;
            * `T` = 4 dec = 11 bin.

        Examples:
        
        ```
        REF     ALT        REF+ALT BINARY ENCODING
        A       G          0001 0001 00 10 00 00 00 00 00 00 00 00 00 0
        GGG     GA         0011 0010 10 10 10 10 00 00 00 00 00 00 00 0
        ACGT    CGTACGT    0100 0111 00 01 10 11 01 10 11 00 01 10 11 0
                           |                                          |
                         33 (MSB)                                   63 (LSB)
        ```

        The reversible encoding covers 99.635% of the variants in the normalized dbSNP VCF file GRCh37.p13.b150.


<a name="vkproperties"></a>
### VariantKey Properties

* Sorting the VariantKey is equivalent of sorting by `CHROM` and `POS`.
* The 64 bit VariantKey can be exported as a 16 character hexadecimal string.
* Sorting the hexadecimal representation of VariantKey in alphabetical order is equivalent of sorting the VariantKey numerically.
* Each VariantKey code is unique for a given reference genome.
* The direct comparisons of two VariantKeys makes sense only if they both refer to the same genome reference.
* Comparing two variants by VariantKey only requires comparing two numbers, a very well optimized operation in current computer architectures. In contrast, comparing two normalized variants in VCF format requires comparing one numbers and three strings.
* VariantKey can be used as a main database key to index data by "variant". This simplify common searching, merging and filtering operations.
* All types of database joins between two data sets (inner, left, right and full) can be easily performed using the VariantKey as index.
* When `CHROM`, `REF` and `ALT` are the only strings in a table, replacing them with VariantKey allows to work with numeric only tables with obvious advantages. This also allows to represent the data in a compact binary format where each column uses a fixed number of bit, with the ability to perform a quick binary search algorithm on the first sorted column.


<a name="vkinput"></a>
## VariantKey Input values

* **`CHROM`** - *chromosome*     : Identifier from the reference genome, no white-space permitted.
* **`POS`**   - *position*       : The reference position, with the first nucleotide having position 0.
* **`REF`**   - *reference allele* :
    String containing a sequence of [nucleotide letters](https://en.wikipedia.org/wiki/Nucleic_acid_notation).
    The value in the `POS` field refers to the position of the first nucleotide in the string.
* **`ALT`**   - *alternate non-reference allele* : 
    String containing a sequence of [nucleotide letters](https://en.wikipedia.org/wiki/Nucleic_acid_notation).


<a name="regionkey"></a>
## RegionKey

*RegionKey* encodes a human genetic region (defined as the set of *chromosome*, *start position*, *end position* and *strand direction*) in a 64 bit unsigned integer number.

RegionKey allows to repesent a region as a single entity, and provides analogous properties as the ones listed in [VariantKey Properties](#vkproperties).

The encoding of the first 33 bit (CROM, STARTPOS) is the same as in VariantKey.

The RegionKey is composed of 4 sections arranged in 64 bit:


```
    1      8      16      24      32      40      48      56      64
    |      |       |       |       |       |       |       |       |
    0123456789012345678901234567890123456789012345678901234567890123
    CHROM|<------ START POS -------->||<------ END POS -------->|||
      |               |                           |              |
      5 bit           28 bit                      28 bit         2 bit STRAND
```

* **`CHROM`**   : 5 bit to represent the chromosome.

    ```
    0   4
    |   |
    11111000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
        |
        LSB
    ```
    The chromosome is encoded as in VariantKey.

* **`STARTPOS`** : 28 bit for the region START position.

    ```
    0    5                             32
    |    |                              |
    00000111 11111111 11111111 11111111 10000000 00000000 00000000 00000000
         |                              |
        MSB                            LSB
    ```
    This section is encoded as in VariantKey POS.

* **`ENDPOS`** : 28 bit for the region END position.

    ```
    0                                   33                            60
    |                                    |                             |
    00000000 00000000 00000000 00000000 01111111 11111111 11111111 11111000
                                         |                             |
                                        MSB                           LSB
    ```
    The end position is equivalent to (STARTPOS + REGION_LENGTH).

* **`STRAND`** : 2 bit to encode the strand direction.

    ```
    0                                                                 61  62
    |                                                                   ||
    00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000110
                                                                        ||
                                                                     MSB  LSB
    ```
    The strand direction is encoded as:
    
    ```
    -1 : 2 dec = "10" bin
     0 : 0 dec = "00" bin
    +1 : 1 dec = "01" bin
    ```

* The last bit of RegionKey is reserved.

This software library provides several functions to operate with *RegionKey* and interact with *VariantKey*.


<a name="esid"></a>
## Encoding String IDs

This library contains extra functions to encode some string IDs to 64 bit unsigned integers:

* The `encode_string_id` function encodes maximum 10 ASCII characters (from '!' to 'z') of a string into a 64 bit unsigned integer. The encoded value can be reversed into a "normalized" version of the original 10 character string using the `decode_string_id` function. The decoded string only support uppercase characters.

* The `encode_string_num_id` function encodes string composed by a character section followed by a separator character and a numerical section into a 64 bit unsigned integer. For example: "ABCDE:0001234". This function encodes up to 5 characters in uppercase, a number up to 2<sup>27</sup>, and up to 7 zero padding digits in a 64 bit unsigned integer. The encoded value can be reversed into a "normalized" version of the original 10 character string using the `decode_string_id` function.

* The `hash_string_id` function creates a 64 bit unsigned integer hash of the input string.


<a name="binaryfiles"></a>
## Binary file formats for lookup tables 

A direct application of the VariantKey representation is the ability to create lookup tables as simple binary files.

The binary lookup-table files are natively supported by the variantkey library and can be generated from a normalized VCF file using the `resources/tools/vkhexbin.sh` shell script.  
The VCF file can be normalized using the `resources/tools/vcfnorm.sh` script.  
The `vkhexbin.sh` requires [bcftools](https://github.com/samtools/bcftools) compiled with the plugins in `resources/bcftools/plugins` folder.

* **`rsvk.bin`**  
Lookup table to retrieve VariantKey from rsID.  
This binary file can be generated by the `resources/tools/rsvk.sh' script from a TSV file.  
This can also be in *Apache Arrow File* format with a single *RecordBatch*, or *Feather* format. The first column must contain the rsID sorted in ascending order.

* **`vkrs.bin`**  
Lookup table to retrieve rsID from VariantKey.  
This binary file can be generated by the `resources/tools/vkrs.sh' script from a TSV file.  
This can also be in *Apache Arrow File* format with a single *RecordBatch*, or *Feather* format. The first column must contain the VariantKey sorted in ascending order.

* **`nrvk.bin`**  
Lookup table to retrieve the original `REF` and `ALT` string for the non-reversible VariantKey.  
This binary file can be generated by the `resources/tools/nrvk.sh' script from a TSV file with the following format:

```
    [16 BYTE VARIANTKEY HEX]\t[REF STRING]\t[ALT STRING]\n...
```
for example:

```
    b800c35bbcece603	AAAAAAAAGG	AG
    1800c351f61f65d3	A	AAGAAAGAAAG
```

<a name="clib"></a>
## C Library

The reference implementation of this library is written in C programming language in a way that is also compatible with C++.

This project includes a Makefile that allows you to test and build the project in a Linux-compatible system with simple commands.  
All the artifacts and reports produced using this Makefile are stored in the *target* folder.  

* To see all available options: `make help`
* To build everything: `make all`

### Example command-Line tool

The code inside the `c/vk` folder is used to generate the `vk` command line tool.  
This tools requires the pre-normalized positional arguments `CHROM`, `POS`, `REF`, `ALT` and returns the VariantKey in hexadecimal representation.

See [C Usage Examples](c/test/test_example.c).


<a name="golib"></a>
## GO Library

A go wrapper is located in the `go` directory.  
Use the "`make go`" command to test the GO wrapper and generate reports.

See [GO Usage Examples](go/example/main.go).


<a name="pythonlib"></a>
## Python Module

The python module is located in the `python` directory.
Use the "`make python`" command to test the Python wrapper and generate reports.

See [Python Usage Examples](python/test/example.py).

<a name="pythonclass"></a>
## Python Class

The python class module is a wrapper for the low-level Python library and it is located in the `python-class` directory.
All methods of this class are vectorized, so they also accept lists or numpy arrays as input.
Use the "`make python`" command to test the Python class and generate reports.

See [Python Usage Examples](python-class/test/example.py).

<a name="rlib"></a>
## R Module

The R module is located in the `r` directory.
Use the "`make r`" command to test the R wrapper and generate reports.

In R the VariantKey is represented with a custom "uint64" class because there is no native support for unsigned 64 bit integers in R.

See [R Usage Examples](r/example/example.R).

<a name="jslib"></a>
## Javascript library (limited support)

Use the "`make javascript`" command to test and minify the Javascript implementation.
