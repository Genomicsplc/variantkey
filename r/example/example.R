#!/usr/bin/env Rscript

# Usage example for VariantKey R wrapper
# https://github.com/genomicsplc/variantkey

library(variantkey)

# BASIC VARIANTKEY FUNCTIONS
# --------------------------

x <- EncodeChrom("X")
print(x)
# [1] 23

x <- DecodeChrom(23)
print(x)
# [1] "X"

x <- EncodeRefAlt("AC", "GT")
print(x)
# [1] 286097408

x <- DecodeRefAlt(286097408)
print(x)
# $REF
# [1] "AC"
#
# $ALT
# [1] "GT"

x <- EncodeVariantKey(23, 12345, 286097408)
print(x)
# [1] 13258623813950472192

x <- ExtractVariantKeyChrom("13258623813950472192")
print(x)
# [1] 23

x <- ExtractVariantKeyPos("13258623813950472192")
print(x)
# [1] 12345

x <- ExtractVariantKeyRefAlt("13258623813950472192")
print(x)
# 286097408

x <- DecodeVariantKey("13258623813950472192")
print(x)
# $CHROM
# [1] 23
#
# $POS
# [1] 12345
#
# $REFALT
# [1] 286097408

x <- VariantKey("X", 12345, "AC", "GT")
print(x)
# [1] 13258623813950472192

x <- VariantKeyRange(23, 1234, 5678)
print(x)
# $MIN
# [1] "b800026900000000"
#
# $MAX
# [1] "b8000b177fffffff"

x <- CompareVariantKeyChrom("B800026900000000", "B8000B177FFFFFFF")
print(x)
# [1] 0

x <- CompareVariantKeyChromPos("B800026900000000", "B8000B177FFFFFFF")
print(x)
# [1] -1


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# GENOREF FUNCTIONS
# -----------------

# Load the reference genome binary file.
# The input reference binary files can be generated from a FASTA file using the resources/tools/fastabin.sh script.
# This example uses the "c/test/data/genoref.bin".
genoref <- MmapGenorefFile("../c/test/data/genoref.bin")
print(genoref$SIZE)
# [1] 598

x <- GetGenorefSeq(genoref$MF, chrom=23, pos=0)
print(x)
# [1] 65

x <- CheckReference(genoref$MF, chrom=23, pos=0, ref="A")
print(x)
# [1] 0

x <- FlipAllele("ATCGMKRYBVDHWSNatcgmkrybvdhwsn")
print(x)
# [1] "TAGCKMYRVBHDWSNTAGCKMYRVBHDWSN"

# Normalize a variant - this function should be used before generating a new VariantKey
x <- NormalizeVariant(genoref$MF, chrom=13, pos=2, ref="CDE", alt="CFE")
print(x)
# $RET
# [1] 48
#
# $POS
# [1] 3
#
# $REF
# [1] "D"
#
# $ALT
# [1] "F"
#
# $REF_LEN
# [1] 1
#
# $ALT_LEN
# [1] 1

MunmapBinfile(genoref$MF)
# [1] 0


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# NRVK
# ----

# Load the lookup table for non-reversible variantkeys.
# The input binary files can be generated from a normalized VCF file using the resources/tools/nrvk.sh script.
# The VCF file can be normalized using the `resources/tools/vcfnorm.sh` script.
# This example uses the "c/test/data/nrvk.10.bin".
nrvk <- MmapNRVKFile("../c/test/data/nrvk.10.bin")
print(nrvk$NROWS)
# [1] 10

x <- FindRefAltByVariantKey(nrvk$MC, vk="2000c3521f1c15ab")
print(x)
# $REF
# [1] "ACGTACGT"
#
# $ALT
# [1] "ACGT"
#
# $REF_LEN
# [1] 8
#
# $ALT_LEN
# [1] 4
#
# $LEN
# [1] 12

# Reverse all VariantKeys, including the ones that are not directly reversible by using a lookup table.
x <- ReverseVariantKey(nrvk$MC, vk="2000c3521f1c15ab")
print(x)
# $CHROM
# [1] "4"
#
# $POS
# [1] 100004
#
# $REF
# [1] "ACGTACGT"
#
# $ALT
# [1] "ACGT"
#
# $REF_LEN
# [1] 8
#
# $ALT_LEN
# [1] 4
#
# $LEN
# [1] 12

x <- GetVariantKeyRefLength(nrvk$MC, vk="2000c3521f1c15ab")
print(x)
# [1] 8

x <- GetVariantKeyEndPos(nrvk$MC, vk="2000c3521f1c15ab")
print(x)
# [1] 100012

x <- GetVariantKeyChromStartPos(vk="2000c3521f1c15ab")
print(x)
# [1] "00000000400186a4"

x <- GetVariantKeyChromEndPos(nrvk$MC, vk="2000c3521f1c15ab")
print(x)
# [1] "00000000400186ac"

MunmapBinfile(nrvk$MF)
# [1] 0


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# RSIDVAR
# -------

# Load the lookup table for rsID to VariantKey.
# The input binary file can be generated using the resources/tools/rsvk.sh script.
# This example uses the "c/test/data/rsvk.10.bin".
rsvk <- MmapRSVKFile("../c/test/data/rsvk.10.bin", as.integer(c(4, 8)))
print(rsvk$NROWS)
# [1] 10

x <- FindRvVariantKeyByRsid(rsvk$MC, 0, rsvk$NROWS, rsid=0x00000061)
print(x)
# $VK
# [1] "80010274003a0000"
#
# $FIRST
# [1] 3

x <- GetNextRvVariantKeyByRsid(rsvk$MC, 2, rsvk$NROWS, rsid=0x00000061)
print(x)
# $VK
# [1] "80010274003a0000"
#
# $POS
# [1] 3

MunmapBinfile(rsvk$MF)
# [1] 0

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# Load the lookup table for rsID to VariantKey.
# The input binary file can be generated using the resources/tools/rsvk.sh script.
# This example uses the "c/test/data/rsvk.m.10.bin".
rsvkm <- MmapRSVKFile("../c/test/data/rsvk.m.10.bin", as.integer(c()))

x <- FindAllRvVariantKeyByRsid(rsvkm$MC, 0, rsvkm$NROWS, rsid=0x00000003)
print(x)
# [[1]]
# [1] "80010274003a0000"
#
# [[2]]
# [1] "8001028d00138000"
#
# [[3]]
# [1] "80010299007a0000"

MunmapBinfile(rsvkm$MF)
# [1] 0

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# Load the lookup table for VariantKey ro rsID
# The input binary file can be generated using the resources/tools/vkrs.sh script.
# This example uses the "c/test/data/vkrs.10.bin".
vkrs <- MmapVKRSFile("../c/test/data/vkrs.10.bin", as.integer(c(8, 4)))

x <- FindVrRsidByVariantKey(vkrs$MC, 0, vkrs$NROWS, vk="80010274003A0000")
print(x)
# $RSID
# [1] 97
#
# $FIRST
# [1] 3

x <- FindVrChromposRange(vkrs$MC, 0, vkrs$NROWS, chrom=0x14, pos_min=0x000256C5, pos_max=0x000256CB)
print(x)
#$RSID
#[1] 9973
#
#$FIRST
#[1] 7
#
#$LAST
#[1] 8

MunmapBinfile(vkrs$MF)
# [1] 0


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# REGIONKEY
# ---------

x <- EncodeRegionStrand(-1)
print(x)
# [1] 2

x <- DecodeRegionStrand(2)
print(x)
# [1] -1

x <- EncodeRegionKey(chrom=25, startpos=1000, endpos=2000, strand=2)
print(x)
# [1] "c80001f400003e84"

x <- ExtractRegionKeyChrom("c80001f400003e84")
print(x)
# [1] 25

x <- ExtractRegionKeyStartPos("c80001f400003e84")
print(x)
# [1] 1000

x <- ExtractRegionKeyEndPos("c80001f400003e84")
print(x)
# [1] 2000

x <- ExtractRegionKeyStrand("c80001f400003e84")
print(x)
# [1] 2

x <- DecodeRegionKey("c80001f400003e84")
print(x)
# $CHROM
# [1] 25
#
# $STARTPOS
# [1] 1000
#
# $ENDPOS
# [1] 2000
#
# $STRAND
# [1] 2

x <- ReverseRegionKey("c80001f400003e84")
print(x)
# $CHROM
# [1] "MT"
#
# $STARTPOS
# [1] 1000
#
# $ENDPOS
# [1] 2000
#
# $STRAND
# [1] -1

x <- RegionKey(chrom="MT", startpos=1000, endpos=2000, strand=-1)
print(x)
# [1] "c80001f400003e84"

x <- GetRegionKeyChromStartPos("c80001f400003e84")
print(x)
# [1] "00000001900003e8"

x <- GetRegionKeyChromEndPos("c80001f400003e84")
print(x)
# [1] "00000001900007d0"

x <- AreOverlappingRegions(a_chrom=5, a_startpos=4, a_endpos=6, b_chrom=5, b_startpos=3, b_endpos=7)
print(x)
# [1] 1

x <- AreOverlappingRegionRegionKey(5, 4, 6, "2800000180000038")
print(x)
# [1] 1

x <- AreOverlappingRegionKeys("2800000200000030", "2800000180000038")
print(x)
# [1] 1

x <- AreOverlappingVariantKeyRegionKey(NULL, "2800000210920000", "2800000180000038")
print(x)
# [1] 1

x <- VariantToRegionkey(NULL, "2800000210920000")
print(x)
# [1] "2800000200000030"


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# ESID
# ---------

x <- EncodeStringID("A0A022YWF9", 0)
print(x)
# [1] "a850850492e77999"

x <- DecodeStringID("a850850492e77999")
print(x)
# [1] "A0A022YWF9"

x <- EncodeStringNumID("ABC:0000123456", 58)
print(x)
# [1] "d8628c002001e240"

x <- DecodeStringID("d8628c002001e240")
print(x)
# [1] "ABC:0000123456"

x <- HashStringID("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ")
print(x)
# [1] "b3a5fdb8808cb7dc"


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


