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

x <- CompareVariantKeyChrom(hexToUint64(hexToUint64("B800026900000000"), hexToUint64("B8000B177FFFFFFF"))
print(x)
# [1] 0

x <- CompareVariantKeyChromPos(hexToUint64("B800026900000000"), hexToUint64("B8000B177FFFFFFF"))
print(x)
# [1] -1


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# GENOREF FUNCTIONS
# -----------------

# Load the reference genome binary file.
# The input reference binary files can be generated from a FASTA file using the resources/tools/fastabin.sh script.
# This example uses the "c/test/data/genoref.bin".
genoref <- MmapGenorefFile("genoref.bin")
print(genoref$SIZE)
# [1] 598

x <- GetGenorefSeq(genoref$MF, chrom=23, pos=0)
print(x)
# [1] "A"

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

MunmapBinfile(genoref$MF)
# [1] 0


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# NRVK
# ----

# Load the lookup table for non-reversible variantkeys.
# The input binary files can be generated from a normalized VCF file using the resources/tools/nrvk.sh script.
# The VCF file can be normalized using the `resources/tools/vcfnorm.sh` script.
# This example uses the "c/test/data/nrvk.10.bin".
nrvk <- MmapNRVKFile("nrvk.10.bin")
print(nrvk$NROWS)
# [1] 10

x <- FindRefAltByVariantKey(nrvk$MC, vk="2306057766690362795")
print(x)
# $REF
# [1] "ACGTACGT"
#
# $ALT
# [1] "ACGT"

# Reverse all VariantKeys, including the ones that are not directly reversible by using a lookup table.
x <- ReverseVariantKey(nrvk$MC, vk="2306057766690362795")
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

x <- GetVariantKeyRefLength(nrvk$MC, vk="2306057766690362795")
print(x)
# [1] 8

x <- GetVariantKeyEndPos(nrvk$MC, vk="2306057766690362795")
print(x)
# [1] 100012

x <- GetVariantKeyChromStartPos(vk="2306057766690362795")
print(x)
# [1] "1073841828"

x <- GetVariantKeyChromEndPos(nrvk$MC, vk="2306057766690362795")
print(x)
# [1] "1073841836"

MunmapBinfile(nrvk$MF)
# [1] 0


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# RSIDVAR
# -------

# Load the lookup table for rsID to VariantKey.
# The input binary file can be generated using the resources/tools/rsvk.sh script.
# This example uses the "c/test/data/rsvk.10.bin".
rsvk <- MmapRSVKFile("rsvk.10.bin", as.integer(c(4, 8)))
print(rsvk$NROWS)
# [1] 10

x <- FindRvVariantKeyByRsid(rsvk$MC, 0, rsvk$NROWS, rsid=0x00000061)
print(x)
# $VK
# [1] "9223656209074749440"
#
# $FIRST
# [1] 3

x <- GetNextRvVariantKeyByRsid(rsvk$MC, 2, rsvk$NROWS, rsid=0x00000061)
print(x)
# $VK
# [1] "9223656209074749440"
#
# $POS
# [1] 3

MunmapBinfile(rsvk$MF)
# [1] 0

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# Load the lookup table for rsID to VariantKey.
# The input binary file can be generated using the resources/tools/rsvk.sh script.
# This example uses the "c/test/data/rsvk.m.10.bin".
rsvkm <- MmapRSVKFile("rsvk.m.10.bin", as.integer(c()))

x <- FindAllRvVariantKeyByRsid(rsvkm$MC, 0, rsvkm$NROWS, rsid=0x00000003)
print(x)
# [[1]]
# [1] "9223656209074749440"
#
# [[2]]
# [1] "9223656316446408704"
#
# [[3]]
# [1] "9223656367992733696"

MunmapBinfile(rsvkm$MF)
# [1] 0

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# Load the lookup table for VariantKey ro rsID
# The input binary file can be generated using the resources/tools/vkrs.sh script.
# This example uses the "c/test/data/vkrs.10.bin".
vkrs <- MmapVKRSFile("vkrs.10.bin", as.integer(c(8, 4)))

x <- FindVrRsidByVariantKey(vkrs$MC, 0, vkrs$NROWS, vk="9223656209074749440")
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
#[1] 9

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

x <- AreOverlappingVariantKeyRegionKey(NULL, "2882303770385055744", "2882303767959568440")
print(x)
# [1] 1

x <- VariantToRegionkey(NULL, "2882303770385055744")
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


