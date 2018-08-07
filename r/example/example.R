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
#
# $REF_LEN
# [1] 2
#
# $ALT_LEN
# [1] 2

x <- EncodeVariantKey(23, 12345, 286097408)
print(x)
# [1] "b800181c910d8000"

x <- ExtractVariantkeyChrom("b800181c910d8000")
print(x)
# [1] 23

x <- ExtractVariantkeyPos("b800181c910d8000")
print(x)
# [1] 12345

x <- ExtractVariantkeyRefAlt("b800181c910d8000")
print(x)
# 286097408

x <- DecodeVariantkey("b800181c910d8000")
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
# [1] "b800181c910d8000"

x <- VariantKeyRange(23, 1234, 5678)
print(x)
# $MIN
# [1] "b800026900000000"
#
# $MAX
# [1] "b8000b177fffffff"

x <- CompareVariantkeyChrom("B800026900000000", "B8000B177FFFFFFF")
print(x)
# [1] 0

x <- CompareVariantkeyChromPos("B800026900000000", "B8000B177FFFFFFF")
print(x)
# [1] -1


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# GENOREF FUNCTIONS
# -----------------

# Load the reference genome binary file.
# The input reference binary files can be generated from a FASTA file using the resources/tools/fastabin.sh script.
# This example uses the "c/test/data/genoref.bin".
genoref <- MmapBinfile("../c/test/data/genoref.bin")
idx <- LoadGenorefIndex(genoref$SRC)

x <- GetGenorefSeq(genoref$SRC, idx, chrom=23, pos=0)
print(x)
# [1] 65

x <- CheckReference(genoref$SRC, idx, chrom=23, pos=0, ref="A")
print(x)
# [1] 0

x <- FlipAllele("ATCGMKRYBVDHWSNatcgmkrybvdhwsn")
print(x)
# [1] "TAGCKMYRVBHDWSNTAGCKMYRVBHDWSN"

# Normalize a variant - this function should be used before generating a new VariantKey
x <- NormalizeVariant(genoref$SRC, idx, chrom=13, pos=2, ref="CDE", alt="CFE")
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

MunmapBinfile(genoref$SRC, genoref$FD, genoref$SIZE)
# [1] 0


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# NRVK
# ----

# Load the lookup table for non-reversible variantkeys.
# The input binary files can be generated from a normalized VCF file using the resources/tools/vkhexbin.sh script.
# The VCF file can be normalized using the `resources/tools/vcfnorm.sh` script.
# This example uses the "c/test/data/vknr.10.bin".
vknr <- MmapBinfile("../c/test/data/vknr.10.bin")

x <- FindRefAltByVariantkey(vknr$SRC, vknr$LAST, vk="2000c3521f1c15ab")
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
x <- ReverseVariantkey(vknr$SRC, vknr$LAST, vk="2000c3521f1c15ab")
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

x <- GetRefLenByVariantkey(vknr$SRC, vknr$LAST, vk="2000c3521f1c15ab")
print(x)
# [1] 8

x <- GetVariantkeyEndPos(vknr$SRC, vknr$LAST, vk="2000c3521f1c15ab")
print(x)
# [1] 100012

MunmapBinfile(vknr$SRC, vknr$FD, vknr$SIZE)
# [1] 0


# /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# RSIDVAR
# -------

# Load the lookup table for rsID to VariantKey.
# The input binary file can be generated using the resources/tools/vkhexbin.sh script.
# This example uses the "c/test/data/rsvk.10.bin".
rsvk <- MmapBinfile("../c/test/data/rsvk.10.bin")

x <- GetRvVariantkey(rsvk$SRC, item=3)
print(x)
# [1] "80010274003a0000"

x <- FindRvVariantkeyByRsid(rsvk$SRC, 0, 9, rsid=0x00000061)
print(x)
# $VK
# [1] "80010274003a0000"
#
# $FIRST
# [1] 3

x <- GetNextRvVariantkeyByRsid(rsvk$SRC, 2, 9, rsid=0x00000061)
print(x)
# $VK
# [1] "80010274003a0000"
#
# $POS
# [1] 3

MunmapBinfile(rsvk$SRC, rsvk$FD, rsvk$SIZE)
# [1] 0

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# Load the lookup table for rsID to VariantKey.
# The input binary file can be generated using the resources/tools/vkhexbin.sh script.
# This example uses the "c/test/data/rsvk.m.10.bin".
rsvkm <- MmapBinfile("../c/test/data/rsvk.m.10.bin")

x <- FindAllRvVariantkeyByRsid(rsvkm$SRC, 0, 9, rsid=0x00000003)
print(x)
# [[1]]
# [1] "80010274003a0000"
#
# [[2]]
# [1] "8001028d00138000"
#
# [[3]]
# [1] "80010299007a0000"

MunmapBinfile(rsvkm$SRC, rsvkm$FD, rsvkm$SIZE)
# [1] 0

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# Load the lookup table for VariantKey ro rsID
# The input binary file can be generated using the resources/tools/vkhexbin.sh script.
# This example uses the "c/test/data/vkrs.10.bin".
vkrs <- MmapBinfile("../c/test/data/vkrs.10.bin")

x <- GetVrRsid(vkrs$SRC, item=3)
print(x)
# [1] 97

x <- FindVrRsidByVariantkey(vkrs$SRC, 0, 9, vk="80010274003A0000")
print(x)
# $RSID
# [1] 97
#
# $FIRST
# [1] 3

x <- FindVrChromposRange(vkrs$SRC, 0, 9, chrom=0x14, pos_min=0x000256C5, pos_max=0x000256CB)
print(x)
#$RSID
#[1] 9973
#
#$FIRST
#[1] 7
#
#$LAST
#[1] 8

MunmapBinfile(vkrs$SRC, vkrs$FD, vkrs$SIZE)
# [1] 0
