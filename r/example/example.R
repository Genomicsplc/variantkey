#!/usr/bin/env Rscript

# Usage example for VariantKey R wrapper
# https://github.com/genomicsplc/variantkey

library(variantkey)

# UINT64 FUNCTIONS
# ----------------

# In R the VariantKey is represented with a custom "uint64" class
# because there is no native support for unsigned 64 bit integers in R.

as.uint64("81985529216486895")
# [1] "81985529216486895"

# It is also possible to load a uint64 from a 16-character hexadecimal string using the hex64 class:
as.uint64(as.hex64(c("0123456789abcdef")))
# [1] "81985529216486895"

u <- as.uint64(as.hex64(c("fffffffffffffff0", "ffffffff00000000", "ffffffffffffffff", "ffffffffff000000", "fffffffff0000000", "ffffffffffffff00", "fffffffffff00000", "0000000000000000", "ffffffffffff0000", "fffffffffffff000")))
print(u)
#  [1] "18446744073709551600" "18446744069414584320" "18446744073709551615"
#  [4] "18446744073692774400" "18446744073441116160" "18446744073709551360"
#  [7] "18446744073708503040" "0"                    "18446744073709486080"
# [10] "18446744073709547520"

s <- sort(u)
print(s)
#  [1] "0"                    "18446744069414584320" "18446744073441116160"
#  [4] "18446744073692774400" "18446744073708503040" "18446744073709486080"
#  [7] "18446744073709547520" "18446744073709551360" "18446744073709551600"
# [10] "18446744073709551615"

rev(s)
#  [1] "18446744073709551615" "18446744073709551600" "18446744073709551360"
#  [4] "18446744073709547520" "18446744073709486080" "18446744073708503040"
#  [7] "18446744073692774400" "18446744073441116160" "18446744069414584320"
# [10] "0"

order.uint64(u)
#  [1]  8  2  5  4  7  9 10  6  1  3

unique(as.uint64(c("1085102592571150095", "1085102592571150095")))
# [1] "1085102592571150095"

# intersection of pre-sorted vectors
a <- as.uint64(c("1085102592571150000", "1085102592571150010"))
b <- as.uint64(c("1085102592571150010", "1085102592571150020"))
intersect.uint64(a, b)
# [1] "1085102592571150010"

# union of pre-sorted vectors
union.uint64(a, b)
# [1] "1085102592571150000" "1085102592571150010" "1085102592571150020"


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# INITIALIZE VARIANTKEY
# ---------------------

# Load support files:
# genoref_file : Name and path of the binary file containing the genome reference (fasta.bin).
#                This file can be generated from a FASTA file using the resources/tools/fastabin.sh script.
# nrvk_file    : Name and path of the binary file containing the non-reversible-VariantKey mapping (nrvk.bin).
#                This file can be generated from a normalized VCF file using the resources/tools/nrvk.sh script.
# rsvk_file    : Name and path of the binary file containing the rsID to VariantKey mapping (rsvk.bin).
#                This file can be generated using the resources/tools/rsvk.sh script.
# vkrs_file    : Name and path of the binary file containing the VariantKey to rsID mapping (vkrs.bin).
#                This file can be generated using the resources/tools/vkrs.sh script.
InitVariantKey(genoref_file = "genoref.bin", nrvk_file = "nrvk.10.bin", rsvk_file = "rsvk.10.bin", vkrs_file = "vkrs.10.bin")


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
# [1] "13258623813950472192"

x <- ExtractVariantKeyChrom("13258623813950472192")
print(x)
# [1] 23

x <- ExtractVariantKeyPos("13258623813950472192")
print(x)
# [1] 12345

x <- ExtractVariantKeyRefAlt("13258623813950472192")
print(x)
# [1] 286097408

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
# [1] "13258623813950472192"

x <- VariantKeyRange(23, 1234, 5678)
print(x)
# $MIN
# [1] "13258599952973561856"
# 
# $MAX
# [1] "13258609498538377215"

x <- CompareVariantKeyChrom(as.hex64("B800026900000000"), as.hex64("B8000B177FFFFFFF"))
print(x)
# [1] 0

x <- CompareVariantKeyChromPos(as.hex64("B800026900000000"), as.hex64("B8000B177FFFFFFF"))
print(x)
# [1] -1


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# GENOREF FUNCTIONS
# -----------------

x <- GetGenorefSeq(chrom=23, pos=0)
print(x)
# [1] "A"

x <- CheckReference(chrom=23, pos=0, ref="A")
print(x)
# [1] 0

x <- FlipAllele("ATCGMKRYBVDHWSNatcgmkrybvdhwsn")
print(x)
# [1] "TAGCKMYRVBHDWSNTAGCKMYRVBHDWSN"

# Normalize a variant - this function should be used before generating a new VariantKey
x <- NormalizeVariant(chrom=13, pos=2, ref="CDE", alt="CFE")
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

# Create a normalized variantkey
x <- NormalizedVariantKey(chrom="13", pos=2, posindex=0, ref="CDE", alt="CFE")
print(x)
# $VK
# [1] "7493989787586955617"
#
# $RET
# [1] 48

# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# NRVK
# ----

x <- FindRefAltByVariantKey(vk="2306057766690362795")
print(x)
# $REF
# [1] "ACGTACGT"
#
# $ALT
# [1] "ACGT"

# Reverse all VariantKeys, including the ones that are not directly reversible by using a lookup table.
x <- ReverseVariantKey(vk="2306057766690362795")
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

x <- GetVariantKeyRefLength(vk="2306057766690362795")
print(x)
# [1] 8

x <- GetVariantKeyEndPos(vk="2306057766690362795")
print(x)
# [1] 100012

x <- GetVariantKeyChromStartPos(vk="2306057766690362795")
print(x)
# [1] "1073841828"

x <- GetVariantKeyChromEndPos(vk="2306057766690362795")
print(x)
# [1] "1073841836"

# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# RSIDVAR
# -------

x <- FindRvVariantKeyByRsid(rsid=0x00000061)
print(x)
# $VK
# [1] "9223656209074749440"
#
# $FIRST
# [1] 3

x <- GetNextRvVariantKeyByRsid(rsid=0x00000061, pos=2)
print(x)
# $VK
# [1] "9223656209074749440"
#
# $POS
# [1] 3

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# This example shows the usage of advanced parameters

# Load the lookup table for rsID to VariantKey.
# The input binary file can be generated using the resources/tools/rsvk.sh script.
# This example uses the "c/test/data/rsvk.m.10.bin".
rsvkm <- MmapRSVKFile("rsvk.m.10.bin", as.integer(c()))

x <- FindAllRvVariantKeyByRsid(rsid=0x00000003, max=10, first=0, last=rsvkm$NROWS, mc=rsvkm$MC)
print(x)
# [1] "9223656209074749440" "9223656316446408704" "9223656367992733696"

MunmapBinfile(rsvkm$MF)
# [1] 0

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

x <- FindVrRsidByVariantKey(vk="9223656209074749440")
print(x)
# $RSID
# [1] 97
#
# $FIRST
# [1] 3

x <- GetNextVrRsidByVariantKey(vk="9223656209074749440", pos=2)
print(x)
# $RSID
# [1] 97
#
# $POS
# [1] 3

x <- FindAllVrRsidByVariantKey(vk="9223656209074749440")
print(x)
# [1] 97

x <- FindVrChromposRange(chrom=0x14, pos_min=0x000256C5, pos_max=0x000256CB)
print(x)
#$RSID
#[1] 9973
#
#$FIRST
#[1] 7
#
#$LAST
#[1] 9

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
# [1] "14411520955069251204"

x <- ExtractRegionKeyChrom("14411520955069251204")
print(x)
# [1] 25

x <- ExtractRegionKeyStartPos("14411520955069251204")
print(x)
# [1] 1000

x <- ExtractRegionKeyEndPos("14411520955069251204")
print(x)
# [1] 2000

x <- ExtractRegionKeyStrand("14411520955069251204")
print(x)
# [1] 2

x <- DecodeRegionKey("14411520955069251204")
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

x <- ReverseRegionKey("14411520955069251204")
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
# [1] "14411520955069251204"

ex <- ExtendRegionKey(x, 100)
print(ex)
# [1] 14411520740320887204
ReverseRegionKey(ex)
# $CHROM
# [1] "MT"
# 
# $STARTPOS
# [1] 900
# 
# $ENDPOS
# [1] 2100
# 
# $STRAND
# [1] -1

x <- GetRegionKeyChromStartPos("14411520955069251204")
print(x)
# [1] "6710887400"

x <- GetRegionKeyChromEndPos("14411520955069251204")
print(x)
# [1] "6710888400"

x <- AreOverlappingRegions(a_chrom=5, a_startpos=4, a_endpos=6, b_chrom=5, b_startpos=3, b_endpos=7)
print(x)
# [1] 1

x <- AreOverlappingRegionRegionKey(5, 4, 6, "2882303767959568440")
print(x)
# [1] 1

x <- AreOverlappingRegionKeys("2882303770107052080", "2882303767959568440")
print(x)
# [1] 1

x <- AreOverlappingVariantKeyRegionKey("2882303770385055744", "2882303767959568440")
print(x)
# [1] 1

x <- VariantToRegionkey("2882303770385055744")
print(x)
# [1] "2882303770107052080"


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# ESID
# ---------

x <- EncodeStringID("A0A022YWF9", 0)
print(x)
# [1] "12128340051199752601"

x <- DecodeStringID("12128340051199752601")
print(x)
# [1] "A0A022YWF9"

x <- EncodeStringNumID("ABC:0000123456", ":")
print(x)
# [1] "15592178792074961472"

x <- DecodeStringID("15592178792074961472")
print(x)
# [1] "ABC:0000123456"

x <- HashStringID("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ")
print(x)
# [1] "12945031672818874332"


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\

# Close the memory-mapped files
CloseVariantKey()
# [1] 0
